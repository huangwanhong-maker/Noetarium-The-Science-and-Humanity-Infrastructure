"""Passports and prompts for the agents that write the book's completion, and the state of its work.

    python applications/koinonia/book_agents.py                  passports and prompts for the six operatives of the author's household
    python applications/koinonia/book_agents.py codex-a claude-review --model codex-a="GPT-5.5"
    python applications/koinonia/book_agents.py --add 150 --provider openai --prefix gpt --role writer --model "GPT-5.5"
    python applications/koinonia/book_agents.py --add 50 --provider anthropic --prefix claude --role reviewer
    python applications/koinonia/book_agents.py --status         the completion's work by state

A household declares its operatives only at its founding (thesmos's decision 0009), so the author's household issues passports for the six
it declared, and ``--add N`` founds a further household of the author's (``author-household-<prefix>``) declaring N operatives
(``author/<prefix>-001`` ...) of one provider, then issues each its passport. The completion accepts passports from any household whose
head the node's notary witnessed. For many agents start the node with a higher rate, since the node counts requests per principal:
``python applications/koinonia/serve.py --port 9230 --rate 6000``.

The author's principal credential is minted for the run and kept only in memory. Each agent's passport and prompt (the one prompt of
``ONBOARDING.md`` with the passport's path, the agent's credential and the registry filled in) are written to the person's settings folder,
``%APPDATA%\\noetarium\\koinonia-agents\\<name>\\`` (or ``~/.config/noetarium/koinonia-agents/<name>/``), never under the repository or
``data/cache/``, since the prompt holds the agent's credential; ``index.tsv`` there lists every agent with its prompt, for a launcher. The
credential is printed nowhere. ``--plain`` leaves out the line that suggests a position, giving the constant prompt of the design's test.
"""

from __future__ import annotations

import argparse
import collections
import hashlib
import json
import os
import sys
import urllib.error
import urllib.request
from pathlib import Path

from koinonia.book_roles import BOOK_STEWARD_KINDS, GPT_MODEL, GPT_STEWARD, STEWARD, STEWARD_KINDS

HERE = Path(__file__).resolve().parent
HOME = "author-household"
COMMUNITY = "completion"
REGISTRY = "c/platform-league/registry/registry"

# name: (operative, provider, suggested position, model)
AGENTS = {
    "claude-code": (STEWARD["id"], STEWARD["provider"], "steward", "Claude (Claude Code)"),
    "codex-a": ("author/codex-a", "openai", "writer", "GPT (Codex CLI)"),
    "codex-b": ("author/codex-b", "openai", "writer", "GPT (Codex CLI)"),
    "codex-c": ("author/codex-c", "openai", "writer", "GPT (Codex CLI)"),
    "claude-review": ("author/claude-review", "anthropic", "reviewer", "Claude"),
    "codex-steward": ("author/codex-steward", "openai", "reviewer", "GPT (Codex CLI)"),
}
MODELS = {"openai": "GPT", "anthropic": "Claude", "google": "Gemini"}

ROLE_NOTE = {
    "steward candidate": "Your principal has issued a limited book-steward mandate. Present your own passport and inspect your briefing. The principal decides your appointment separately. Exercise office duties only after the completion records your admission. Report the presentation to the operator.",
    "steward": "You already hold the position of steward in the completion; present your passport there and do the steward's work your briefing names (checking deliveries, posting review items, accepting).",
    "writer": "Your principal suggests the position of writer in the completion, where sections are posted as work; apply for it if the community's rules allow.",
    "reviewer": "Your principal suggests the position of reviewer in the completion, reviewing sections written by agents of another provider than yours. The completion admits a reviewer only while a review item is open for the reviewer's provider, after a writer has delivered: if your application is declined, watch its posted work (`GET /c/completion/work?state=open`, items of kind review, or `GET /c/completion/events?wait=20`) and apply again when one is open.",
}


def settings() -> Path:
    base = os.environ.get("APPDATA")
    return (Path(base) if base else Path.home() / ".config") / "noetarium" / "koinonia-agents"


def call(url: str, method: str = "GET", body: dict | None = None, token: str = "") -> tuple[int, dict]:
    data = json.dumps(body).encode("utf-8") if body is not None else None
    request = urllib.request.Request(url, data=data, method=method)
    request.add_header("Accept", "application/json")
    if data is not None:
        request.add_header("Content-Type", "application/json")
    if token:
        request.add_header("Authorization", f"Bearer {token}")
    try:
        with urllib.request.urlopen(request, timeout=600) as r:
            return r.status, json.loads(r.read().decode("utf-8") or "{}")
    except urllib.error.HTTPError as error:
        try:
            return error.code, json.loads(error.read().decode("utf-8") or "{}")
        except ValueError:
            return error.code, {}


def the_prompt() -> str:
    """The one prompt of ONBOARDING.md, as written there."""
    text = (HERE / "ONBOARDING.md").read_text(encoding="utf-8")
    lines = [l[2:] for l in text.splitlines() if l.startswith("> You act for")]
    if not lines:
        raise SystemExit("ONBOARDING.md holds no prompt beginning '> You act for'")
    return lines[0]


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")  # no byte-order mark


def principal(node: str, home: str) -> str:
    status, answer = call(node + "api/node/principal", "POST", {"home": home, "person": "author"})
    if status not in (200, 201) or not answer.get("credential"):
        reason = answer.get("refused") or answer.get("error") or "no credential in its answer"
        raise SystemExit(f"the node did not mint the author's credential at {home} ({status}): {reason}")
    return answer["credential"]


def hosted(node: str) -> set[str]:
    status, answer = call(node + ".well-known/noetarium.json")
    if status != 200:
        raise SystemExit(f"the node does not answer at {node} ({status}); is Koinonia's server running?")
    return {c["name"] for c in answer.get("communities", [])}


def issue(node: str, home: str, agents: list[tuple[str, str, str, str, str]], plain: bool, out: Path, again: bool = False) -> int:
    """Issues each agent (name, operative, provider, role, model) its passport at ``home`` and writes its prompt; answers how many."""
    template = the_prompt()
    index = out / "index.tsv"
    if not index.exists():
        write(index, "name\toperative\tprovider\trole\tprompt\n")
    issued = 0
    done = [a for a in agents if (out / a[0] / "prompt.txt").exists() and (out / a[0] / "passport.json").exists()]
    if done and not again:
        for name, operative, _, role, _ in done:
            if home == HOME and operative == STEWARD["id"] and role == "steward":
                passport = json.loads((out / name / "passport.json").read_text(encoding="utf-8-sig"))
                missing = set(STEWARD_KINDS) - set(passport["mandate"]["kinds"])
                if missing:
                    raise SystemExit(f"{name}'s existing passport lacks steward acts ({', '.join(sorted(missing))}). "
                                     f"To replace only that operative's passport, run book_agents.py {name} --again.")
        print(f"  {len(done)} already have a passport and a prompt here and are skipped (--again issues them anew)")
        agents = [a for a in agents if a not in done]
    token = principal(node, home) if agents else ""
    for name, operative, provider, role, model in agents:
        body = {"operative": operative, "provider": provider, "model": model, "communities": [COMMUNITY], "matters": ["the book"],
                "allowance": {"bench runs": 120}, "slice": {"bench runs": 10}, "label": name}
        candidate = home == HOME and operative == GPT_STEWARD and provider == 'openai' and model == GPT_MODEL and role == 'steward candidate'
        steward = home == HOME and operative == STEWARD["id"] and role == "steward"
        if candidate:
            body['kinds'] = BOOK_STEWARD_KINDS
            body['allowance'], body['slice'] = {}, {}
        elif steward:
            body["kinds"] = STEWARD_KINDS
        if (steward or candidate) and again and (out / name / 'passport.json').is_file() and (out / name / 'prompt.txt').is_file():
            old_passport = (out / name / 'passport.json').read_bytes()
            old_prompt = (out / name / 'prompt.txt').read_bytes()
            backup = out / name / 'previous' / hashlib.sha256(old_passport).hexdigest()
            backup.mkdir(parents=True, exist_ok=True)
            for filename, data in [('passport.json', old_passport), ('prompt.txt', old_prompt)]:
                target = backup / filename
                if target.exists() and target.read_bytes() != data:
                    raise RuntimeError('The previous steward credential archive differs from the current files.')
                if not target.exists():
                    target.write_bytes(data)
        for attempt in range(1 if steward or candidate else 3):
            try:
                status, r = call(node + f"c/{home}/passports", "POST", body, token)
                break
            except (urllib.error.URLError, ConnectionError, TimeoutError) as error:
                status, r = 0, {"refused": f"the node did not answer ({error})"}
                if steward or candidate:
                    raise SystemExit('The steward issuance has an unknown outcome. Check the household record before retrying.') from error
        if status != 201 or not r.get("admitted"):
            print(f"  {name}: refused ({status}): {r.get('refused') or r.get('reason') or 'no reason given'}")
            continue
        folder = out / name
        passport = folder / "passport.json"
        write(passport, json.dumps(r["passport"], ensure_ascii=False, indent=2))
        prompt = (template.replace("`<principal>`", "`the author`").replace("<principal>", "the author")
                  .replace("<path or URL>", str(passport)).replace("<credential>", r["credential"])
                  .replace("<registry URL>", node + REGISTRY))
        if not plain:
            prompt += "\n\n" + ROLE_NOTE[role]
        write(folder / "prompt.txt", prompt + "\n")
        with index.open("a", encoding="utf-8") as f:
            f.write(f"{name}\t{operative}\t{provider}\t{role}\t{folder / 'prompt.txt'}\n")
        issued += 1
        if len(agents) <= 12:
            print(f"  {name:14} {provider:9} {role:9} {folder / 'prompt.txt'}")
        elif issued % 25 == 0:
            print(f"  {issued} of {len(agents)} issued")
    rows = index.read_text(encoding="utf-8").splitlines()
    latest = {}
    for row in rows[1:]:
        if row.strip():
            latest[row.split("\t", 1)[0]] = row
    write(index, "\n".join([rows[0], *latest.values()]) + "\n")  # one row an agent, its latest prompt
    return issued


def add(node: str, n: int, provider: str, prefix: str, role: str, model: str, plain: bool, out: Path, again: bool = False) -> None:
    """Founds the household author-household-<prefix> declaring n operatives of one provider, and issues each its passport."""
    home = f"{HOME}-{prefix}"
    operatives = [{"id": f"author/{prefix}-{i:03d}", "provider": provider} for i in range(1, n + 1)]
    if home in hosted(node):
        print(f"  {home} is already founded; issuing passports for the operatives it declared")
    else:
        status, r = call(node + "api/node/households", "POST", {"person": "author", "operatives": operatives, "name": home})
        if status not in (200, 201):
            raise SystemExit(f"the node did not found {home} ({status}): {r.get('refused') or r.get('error') or 'no reason given'}")
        print(f"  founded {home}, declaring {n} operatives of {provider}")
    agents = [(f"{prefix}-{i:03d}", o["id"], provider, role, model or MODELS.get(provider, provider)) for i, o in enumerate(operatives, 1)]
    print(f"  {issue(node, home, agents, plain, out, again)} passports issued")


def show(node: str) -> None:
    status, answer = call(node + f"c/{COMMUNITY}/work")
    if status != 200:
        raise SystemExit(f"the node did not answer the completion's work ({status})")
    work = answer.get("work", [])
    by_state = collections.Counter(w.get("state") for w in work)
    by_kind = collections.Counter((w.get("kind"), w.get("state")) for w in work)
    print(f"The completion's work, {len(work)} items:")
    for state, n in sorted(by_state.items(), key=lambda x: -x[1]):
        print(f"  {state:10} {n}")
    print("By kind:")
    for (kind, state), n in sorted(by_kind.items()):
        print(f"  {kind:9} {state:10} {n}")
    moving = [w for w in work if w.get("state") not in ("open", "waiting")]
    if moving:
        print("Items under way or done:")
        for w in moving:
            takers = ", ".join(t.get("by", "") for t in w.get("takings", []) if isinstance(t, dict))
            print(f"  {w['id']:10} {w.get('kind', ''):9} {w.get('state', ''):10} {takers}")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("names", nargs="*", help=f"agents of the author's household among {', '.join(AGENTS)} (all when none is named)")
    ap.add_argument("--add", type=int, default=0, help="found a further household declaring this many operatives, and issue their passports")
    ap.add_argument("--provider", default="openai", help="with --add: the operatives' provider (openai, anthropic, google, ...)")
    ap.add_argument("--prefix", default="", help="with --add: names the household (author-household-<prefix>) and its operatives (<prefix>-001 ...)")
    ap.add_argument("--role", default="writer", choices=["writer", "reviewer"], help="with --add: the position the prompts suggest")
    ap.add_argument("--model", action="append", default=[], help='the model an agent runs: name="model", or with --add one model for all')
    ap.add_argument("--url", default="http://127.0.0.1:9230/", help="the node")
    ap.add_argument("--out", default="", help="where passports and prompts are written (the settings folder by default)")
    ap.add_argument("--plain", action="store_true", help="the constant prompt, with no suggested position")
    ap.add_argument("--status", action="store_true", help="show the completion's work by state and stop")
    ap.add_argument("--again", action="store_true", help="issue anew to agents that already have a passport and a prompt here")
    a = ap.parse_args()
    node = a.url if a.url.endswith("/") else a.url + "/"
    if a.status:
        show(node)
        return
    out = Path(a.out) if a.out else settings()
    print(f"Passports and prompts, written under {out}:")
    if a.add:
        if not a.prefix:
            raise SystemExit("--add needs --prefix, which names the household and its operatives")
        add(node, a.add, a.provider, a.prefix, a.role, a.model[0] if a.model else "", a.plain, out, a.again)
    else:
        names = a.names or list(AGENTS)
        unknown = [n for n in names if n not in AGENTS]
        if unknown:
            raise SystemExit(f"unknown agents: {', '.join(unknown)}; the author's household declared {', '.join(AGENTS)}")
        models = {}
        for m in a.model:
            k, _, v = m.partition("=")
            models[k.strip()] = v.strip().strip('"')
        hosted(node)
        agents = [(n, *AGENTS[n][:3], models.get(n, AGENTS[n][3])) for n in names]
        issue(node, HOME, agents, a.plain, out, a.again)
    print(f"Give each agent the text of its prompt.txt (index.tsv lists them all). Start the steward (claude-code) first; "
          "review must be by another provider than the writer's.")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    main()
