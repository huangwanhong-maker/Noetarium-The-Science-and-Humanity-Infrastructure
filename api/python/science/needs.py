"""The evolutionary exploration and validation of needs, for scripts in Python and the works' servers (ADR 0125): statements of need
searched for by evolution on the discovery part of a dataset, admitted through a reusable holdout, and confirmed in order by an
online procedure, asked of the engine a work has built.

The computations are composed in ``api/cpp/science/needs.hpp``, which documents the dataset it takes (``noetarium.needs-data/1``, as
``data/products/needs_tables.py`` writes it) and its operations:

    from science import needs
    d = needs.data("melbourne-assistance")                    # the data library's product, from the cache
    needs.ask("describe", data=d)
    run = needs.ask("evolve", data=d, generations=10)          # {state, report, refused}
    run = needs.ask("evolve", data=d, state=run["state"], generations=10, proposals=[{"text": "...", "source": "me"}])

The engine is a program built by a work: the Urban Studio's plug-in *Needs* builds ``data/cache/engines/urban-studio/needs/engine``;
``NOETARIUM_NEEDS_ENGINE`` names another.

**A language model proposes, and never judges.** ``propose`` asks a model (``science.models``) for statements in the domain's
language, given the language, the data's vocabulary and the population's best statements with their scores on the discovery part
alone, as an optimiser's prompt holds the best solutions so far (Yang et al. 2024, OPRO). It is never shown a validation answer, a
confirmation or a standing, so that nothing of the parts kept aside reaches the search through it. What it writes is read by the
engine as anyone's would be; a line that does not read, or that the data cannot judge, is refused with the reason and never
repaired. Its statements are proposals of grade C, credited to it by the evolution like any other operator.
"""

from __future__ import annotations

import json
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/needs", env="NOETARIUM_NEEDS_ENGINE")
DATA_SCHEMA = "noetarium.needs-data/1"
STATE_SCHEMA = "noetarium.needs-search/1"
ROOT = Path(__file__).resolve().parents[3]
PRODUCTS = ROOT / "data" / "cache" / "products" / "needs"


def ask(op: str, timeout: float = 1800.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def data(case: str | Path) -> dict:
    """A product's dataset, by its case's name or by the path of its file."""
    path = Path(case)
    if not path.suffix:
        path = PRODUCTS / f"{case}.json"
    if not path.is_file():
        raise LookupError(f"there is no dataset at {path}; make it with python data/products/needs_tables.py {case}")
    d = json.loads(path.read_text(encoding="utf-8"))
    if d.get("schema") != DATA_SCHEMA:
        raise ValueError(f"{path} is not a {DATA_SCHEMA}")
    return d


def cases() -> list[dict]:
    """The cases the data library has made, from its index."""
    index = PRODUCTS / "index.json"
    if not index.is_file():
        return []
    return [{"case": k} | v for k, v in json.loads(index.read_text(encoding="utf-8")).get("cases", {}).items()]


# ---- a language model's proposals ----

GRAMMAR = """need      ::= "need" domain "for" group [ "within" group ] "by" standard
group     ::= "everyone" | condition { "and" condition }
condition ::= attribute ( "=" | "!=" | "<" | "<=" | ">" | ">=" ) value | attribute "in" "{" value { "," value } "}"
standard  ::= "normative" indicator ( "above" | "below" ) level          (a level may be a share, 0.3, or a percentage, 30%)
            | "comparative" indicator ( "higher" | "lower" ) [ "than" group ]
            | "expressed" demand "over" provision
            | "felt" indicator ( "above" | "below" ) level
A value with a space or that reads as a number but is a category is quoted: area = "North Melbourne", sa1 = "20604112201"."""

PROPOSE_SYSTEM = """You propose hypotheses of need for a search that tests them against data. A statement of need says that a group of people, \
within a frame, has a gap in a domain of satisfaction, judged on an indicator by a standard (Bradshaw's four: normative, comparative, \
expressed, felt). You write statements in a small formal language, one per item, exactly as its grammar says, naming only the \
attributes, levels, indicators and domains you are given, each indicator in the direction in which it is worse. Your statements are \
proposals: each will be read by a parser that refuses what does not fit, tested on data you do not see, and kept only if the tests \
uphold it. Propose groups that are specific and plausible, that differ from those already found, and that a person could act on; give \
for each, in a sentence, why it might hold. Do not invent numbers about the data; you are not told how the statements fared beyond \
the scores given, and must not guess."""

PROPOSE_SCHEMA = {
    "type": "object",
    "properties": {
        "statements": {
            "type": "array",
            "items": {"type": "object", "properties": {"statement": {"type": "string"}, "why": {"type": "string"}},
                      "required": ["statement", "why"], "additionalProperties": False},
        }
    },
    "required": ["statements"],
    "additionalProperties": False,
}


def vocabulary(described: dict) -> str:
    """The data's vocabulary for a prompt: indicators, domains, and the attributes with their conditions."""
    lines = ["Indicators (name: what it measures; its domain; the direction in which it is worse; its value over everyone):"]
    for i in described.get("indicators", []):
        worse = "higher" if i.get("higher_is_worse", True) else "lower"
        value = i.get("value")
        shown = f"{value:.4g}" if isinstance(value, (int, float)) else "unknown"
        lines.append(f"  {i['name']}: {i.get('label') or i['name']}; domain {i.get('domain')}; worse when {worse}; {shown} over everyone")
    lines.append("Domains: " + ", ".join(described.get("domains", [])))
    by: dict[str, list[str]] = {}
    for c in described.get("conditions", []):
        by.setdefault(c.split(" ", 1)[0], []).append(c)
    lines.append("Attributes, with conditions the search uses (others of the same form are allowed):")
    for a in described.get("attributes", []):
        cs = by.get(a, [])
        lines.append(f"  {a}: " + ("; ".join(cs[:14]) + (" …" if len(cs) > 14 else "") if cs else "(no condition)"))
    return "\n".join(lines)


def best_so_far(report: dict | None, most: int = 15) -> str:
    """The population's best statements with their scores on the discovery part only, for a prompt."""
    if not report:
        return "Nothing has been found yet."
    names = report.get("objectives", [])
    rows = []
    for m in (report.get("population") or [])[:most]:
        obj = (m.get("objectives") or {}).get("objectives") or []
        said = []
        for n, v in zip(names, obj):
            if n == "gap":
                said.append(f"gap z {-v:.1f}")
            elif n == "coverage":
                said.append(f"{10 ** (-v):,.0f} people")
            elif n == "simplicity":
                said.append(f"{v:.0f} conditions")
        rows.append(f"  {m['text']}   [{', '.join(said)}]")
    return "The search's best statements so far, scored on the part of the data it sees:\n" + ("\n".join(rows) if rows else "  (none)")


def prompt_for(described: dict, report: dict | None, n: int, description: str | None = None, vary: list[str] | None = None) -> str:
    parts = ["The language of statements:\n" + GRAMMAR, vocabulary(described), best_so_far(report)]
    if description:
        parts.append("The person asks you to look for needs of this kind:\n" + description.strip()[:2000])
    if vary:
        parts.append("Vary these statements, each in a way the search's own operators would not (a condition another person would think of, "
                     "a related indicator or domain, a frame that changes the comparison):\n" + "\n".join("  " + v for v in vary[:12]))
    parts.append(f"Propose {n} new statements, distinct from those above.")
    return "\n\n".join(parts)


def propose(d: dict, report: dict | None = None, n: int = 12, provider=None, *, description: str | None = None,
            vary: list[str] | None = None, described: dict | None = None, name: str = "a language model") -> dict:
    """A model's proposals: ``{text, read, refused, by, grade: "C", source}``: ``text`` the statements that read, one to a line, for
    ``evolve``'s proposals; ``refused`` the lines refused with the engine's reasons. models.Refusal when no answer could be had."""
    from . import models
    provider = provider or models.Anthropic()
    described = described or ask("describe", data=d, options={"simulate": False})
    prompt = prompt_for(described, report, n, description, vary)
    answer, served = provider.answer(PROPOSE_SYSTEM, prompt, PROPOSE_SCHEMA)
    items = [x for x in (answer.get("statements") if isinstance(answer, dict) else None) or [] if isinstance(x, dict)]
    lines = [" ".join(str(x.get("statement", "")).split()) for x in items]
    why = {" ".join(str(x.get("statement", "")).split()): str(x.get("why", ""))[:400] for x in items}
    read = ask("read", data=d, text="\n".join(lines)) if lines else []
    kept, refused = [], []
    for r in read:
        (kept if r["read"] else refused).append({"line": r["text"], "statement": r.get("statement"), "why": r.get("why") or why.get(r["text"], "")})
    for k in kept:
        k["reason"] = why.get(k["line"], "")
    return {"text": "\n".join(k["statement"] for k in kept), "read": kept, "refused": refused, "by": f"{provider.name}, {served}",
            "source": name, "grade": "C", "status": "proposed", "prompt_chars": len(prompt)}


def rounds(d: dict, options: dict | None = None, *, count: int = 3, generations: int = 5, provider=None, n: int = 12,
           state: dict | None = None, description: str | None = None, log=print) -> dict:
    """A search in rounds: `generations` generations, then a model's proposals from the population's best, added to the next round.
    ``{state, report, proposals}``, the proposals as each round's model gave them, refused lines with them."""
    options = options or {}
    described = ask("describe", data=d, options=dict(options, simulate=False))
    run = {"state": state, "report": None}
    made = []
    for k in range(count):
        proposals = []
        if provider is not None and (k > 0 or description):
            p = propose(d, run["report"], n, provider, description=description if k == 0 else None, described=described)
            made.append(p)
            if p["text"]:
                proposals.append({"text": p["text"], "source": p["source"]})
            log(f"  round {k + 1}: the model proposed {len(p['read'])} statements that read, {len(p['refused'])} refused")
        request = {"data": d, "options": options, "generations": generations, "proposals": proposals}
        if run["state"]:
            request["state"] = run["state"]
        run = ask("evolve", **request)
        rep = run["report"]
        log(f"  round {k + 1}: generation {rep['generation']}, {len(rep['archive'])} kept, {sum(c['confirmed'] for c in rep['confirmations'])} confirmed")
    return {"state": run["state"], "report": run["report"], "proposals": made}
