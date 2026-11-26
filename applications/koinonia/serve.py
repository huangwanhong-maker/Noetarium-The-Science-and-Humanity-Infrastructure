"""Serve Koinonia on this computer (ADR 0169).

Koinonia is a work in which communities are founded on constitutions, agents are lent to them and act on their own initiative for
their communities' missions, and every act is put to the community's own rules by thesmos's fold through the API (ADR 0167). Each
society has a clock: while it runs, a round is played every few seconds whether or not a page is open, and agents that have nothing
to act on cost nothing. Model agents ask a stand-in until the person at this computer switches Claude on; then each call is reserved
against the ledger kept in their settings folder and refused past its stop.

Usage::

    python applications/koinonia/serve.py [--host 127.0.0.1] [--port 9230] [--open]

Societies are kept under data/cache/koinonia/societies/. Changes are taken only from this computer's own address.
"""

from __future__ import annotations

import argparse
import http.server
import json
import mimetypes
import re
import sys
import threading
import time
import webbrowser
from pathlib import Path
from urllib.parse import parse_qs, urlparse

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

from koinonia import agents, cases  # noqa: E402
from koinonia.budget import Ledger  # noqa: E402
from koinonia.society import Society  # noqa: E402
from science import communities  # noqa: E402
from science.models import Stub, kept_key  # noqa: E402

STORE = ROOT / "data" / "cache" / "koinonia" / "societies"
NAME = re.compile(r"^[a-z0-9][a-z0-9-]{0,40}$")
LEDGER = Ledger.kept()


class Running:
    """A society in memory with its clock."""

    def __init__(self, society: Society):
        self.society, self.models = society, False
        self.seconds, self.running, self.idle = 3.0, False, 0
        self.thread: threading.Thread | None = None
        self.ask_with(False)

    def ask_with(self, models: bool) -> None:
        self.models = models
        if models:
            self.society.asker = agents.Asker(LEDGER, agents.anthropic_maker(kept_key("koinonia") or kept_key("anthropic")))
        else:
            self.society.asker = agents.Asker(Ledger(None), lambda agent, max_tokens: Stub(cases.stand_in))

    def start(self, seconds: float) -> None:
        self.seconds = max(0.5, min(120.0, seconds))
        if self.running:
            return
        self.running, self.idle = True, 0
        self.thread = threading.Thread(target=self.loop, daemon=True)
        self.thread.start()

    def loop(self) -> None:
        while self.running:
            started = time.time()
            try:
                r = self.society.round()
                self.idle = self.idle + 1 if r["acts"] == 0 else 0
                if self.idle >= 6:  # six quiet rounds: the clock stops itself and says so
                    self.society.event("", "the clock stopped after six rounds in which no one acted", kind="clock")
                    self.running = False
            except Exception as error:  # the clock stops on a fault and says why, rather than playing on a broken state
                self.society.event("", f"the clock stopped: {error}", kind="clock")
                self.running = False
            time.sleep(max(0.0, self.seconds - (time.time() - started)))

    def view(self) -> dict:
        v = self.society.view()
        v["clock"] = {"running": self.running, "seconds": self.seconds, "models": self.models}
        return v


SOCIETIES: dict[str, Running] = {}
LOCK = threading.Lock()


def society(name: str) -> Running:
    with LOCK:
        if name not in SOCIETIES:
            folder = STORE / name
            if not (folder / "society.json").is_file():
                raise KeyError(name)
            SOCIETIES[name] = Running(Society.load(folder))
        return SOCIETIES[name]


def listing() -> list[dict]:
    out = []
    for f in sorted(STORE.glob("*/society.json")):
        try:
            d = json.loads(f.read_text(encoding="utf-8"))
            out.append({"name": d["name"], "turn": d["turn"], "communities": [c["name"] for c in d["communities"]], "description": d.get("description", "")})
        except (OSError, ValueError, KeyError):
            continue
    return out


def create(body: dict) -> tuple[dict, int]:
    name = str(body.get("name", "")).strip().lower()
    if not NAME.match(name):
        return {"error": "a society's name is lowercase letters, digits and hyphens"}, 400
    if (STORE / name / "society.json").is_file():
        return {"error": f"a society named {name} exists"}, 409
    s = Society(name, STORE / name)
    case = body.get("case", "blank")
    if case in cases.CASES:
        cases.CASES[case](s)
    elif case == "blank":
        s.party("you", "person", label="You")
        s.description = "A society begun at this computer."
        s.save()
    else:
        return {"error": f"no case {case}"}, 400
    with LOCK:
        SOCIETIES[name] = Running(s)
    return {"created": name}, 201


def found(r: Running, body: dict) -> tuple[dict, int]:
    s = r.society
    name, preset, founder = str(body.get("name", "")).strip().lower(), str(body.get("preset", "")), str(body.get("founder", "you"))
    if not NAME.match(name) or name in s.communities or name in s.parties:
        return {"error": "a community needs a new name of lowercase letters, digits and hyphens"}, 400
    if preset not in communities.presets():
        return {"error": f"no preset {preset}"}, 400
    if founder not in s.parties or s.parties[founder]["kind"] not in ("person", "organisation"):
        return {"error": "a community is founded by a person or an organisation of the society"}, 400
    c = communities.preset(preset)
    first = {"commons": "steward", "guild": "steward", "cooperative": "board", "polity": "magistrate"}.get(preset)
    holders = [[founder, first]] if first else []
    if preset == "guild":
        holders.append([founder, "member"])
    try:
        with s.lock:
            s.found(name, preset, founder, holders, str(body.get("mission", "")).strip() or c.get("purpose", ""), roles={"steward": founder})
            s.save()
    except Exception as error:
        return {"error": str(error)}, 400
    return {"founded": name}, 201


def lend(r: Running, body: dict) -> tuple[dict, int]:
    s = r.society
    sponsor, role, policy, community = str(body.get("sponsor", "")), str(body.get("role", "checker")), str(body.get("policy", "rule")), str(body.get("community", ""))
    if sponsor not in s.parties or s.parties[sponsor]["kind"] not in ("person", "organisation"):
        return {"error": "an agent is lent by a person or an organisation of the society"}, 400
    if role not in agents.KINDS or policy not in ("rule", "model") or community not in s.communities:
        return {"error": "name a role, a policy (rule or model) and a community of the society"}, 400
    if body.get("new_sponsor") and sponsor not in s.parties:
        s.party(sponsor, "organisation")
    n = 1 + sum(1 for a in s.agents if a.startswith(f"{sponsor}/"))
    ident = f"{sponsor}/{role}-{n}"
    c = s.communities[community]
    w = {"matters": ["catalogue"] if c.preset == "commons" else ["adjudication", "trial"]}
    if role == "checker":
        w.update(position="contributor", allowance={"entries": 400, "tokens": 400000} if policy == "model" else {"entries": 400})
    elif role == "adjudicator" and c.holds(sponsor, "member"):
        w.update(position="aide", sponsor_position=True)
    model = str(body.get("model", "claude-opus-5-5"))
    if model not in agents.MODELS:
        return {"error": f"no model {model}"}, 400
    try:
        with s.lock:
            agents.lend(s, ident, sponsor, role, policy, {community: w}, provider="anthropic" if policy == "model" else "", model=model,
                        label=str(body.get("label", "")).strip() or ident, tempo=1 if role in ("keeper", "assessor", "adjudicator") else 2)
            s.event(community, f"{sponsor} lends {ident} ({policy})", kind="lend", who=sponsor)
            s.save()
    except Exception as error:
        return {"error": str(error)}, 400
    return {"lent": ident}, 201


class Handler(http.server.BaseHTTPRequestHandler):
    server_version = "Koinonia/1"

    def log_message(self, *args) -> None:
        pass

    def local(self) -> bool:
        return self.client_address[0] in ("127.0.0.1", "::1")

    def reply(self, body, code: int = 200) -> None:
        data = json.dumps(body, ensure_ascii=False).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(data)

    def body(self) -> dict:
        n = int(self.headers.get("Content-Length") or 0)
        if n > 200_000:
            return {}
        try:
            return json.loads(self.rfile.read(n) or b"{}")
        except ValueError:
            return {}

    def do_GET(self) -> None:
        url = urlparse(self.path)
        parts = [p for p in url.path.split("/") if p]
        try:
            if parts[:1] == ["api"]:
                return self.api_get(parts[1:], parse_qs(url.query))
        except KeyError:
            return self.reply({"error": "no such society"}, 404)
        path = APP / ("index.html" if not parts else "/".join(parts))
        path = path.resolve()
        if APP not in path.parents and path != APP / "index.html" or not path.is_file() or path.suffix not in (".html", ".js", ".css", ".svg"):
            self.send_error(404)
            return
        data = path.read_bytes()
        self.send_response(200)
        self.send_header("Content-Type", (mimetypes.guess_type(path.name)[0] or "text/plain") + ("; charset=utf-8" if path.suffix != ".svg" else ""))
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Cache-Control", "no-cache")
        self.end_headers()
        self.wfile.write(data)

    def api_get(self, parts: list[str], query: dict) -> None:
        if parts == ["state"]:
            key = kept_key("koinonia") or kept_key("anthropic")
            return self.reply({"societies": listing(), "budget": LEDGER.summary(), "key": {"kept": bool(key)}, "presets": communities.presets(),
                               "cases": sorted(cases.CASES), "local": self.local()})
        if len(parts) == 2 and parts[0] == "societies":
            return self.reply(society(parts[1]).view())
        if len(parts) == 3 and parts[0] == "societies" and parts[2] == "events":
            r, after = society(parts[1]), int((query.get("after") or ["0"])[0])
            for _ in range(40):  # waits up to twenty seconds for something new
                if r.society.events and r.society.events[-1]["n"] > after:
                    break
                time.sleep(0.5)
            return self.reply({"events": [e for e in r.society.events if e["n"] > after][-200:], "turn": r.society.turn, "running": r.running})
        self.reply({"error": "no such route"}, 404)

    def do_POST(self) -> None:
        parts = [p for p in urlparse(self.path).path.split("/") if p]
        if not self.local():
            return self.reply({"error": "changes are taken only from this computer"}, 403)
        body = self.body()
        try:
            if parts == ["api", "societies"]:
                return self.reply(*create(body))
            if len(parts) >= 4 and parts[:2] == ["api", "societies"]:
                r = society(parts[2])
                what = parts[3]
                if what == "clock":
                    action = body.get("action")
                    if action == "start":
                        r.start(float(body.get("seconds", r.seconds)))
                    elif action == "stop":
                        r.running = False
                    elif action == "step":
                        if not r.running:
                            r.society.round()
                    elif action == "models":
                        r.ask_with(bool(body.get("on")))
                    return self.reply(r.view())
                if what == "communities":
                    return self.reply(*found(r, body))
                if what == "agents" and len(parts) == 4:
                    return self.reply(*lend(r, body))
                if what == "agents" and len(parts) == 6 and parts[5] == "pause":
                    ident = parts[4].replace("~", "/")
                    if ident not in r.society.agents:
                        return self.reply({"error": "no such agent"}, 404)
                    r.society.agents[ident]["paused"] = bool(body.get("paused", True))
                    r.society.save()
                    return self.reply({"paused": r.society.agents[ident]["paused"]})
        except KeyError:
            return self.reply({"error": "no such society"}, 404)
        self.reply({"error": "no such route"}, 404)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=9230)
    ap.add_argument("--open", action="store_true")
    args = ap.parse_args(argv)
    STORE.mkdir(parents=True, exist_ok=True)
    communities.ENGINE.keep(1)
    server = http.server.ThreadingHTTPServer((args.host, args.port), Handler)
    url = f"http://{'127.0.0.1' if args.host in ('0.0.0.0', '127.0.0.1') else args.host}:{args.port}/"
    print(f"Koinonia at {url}", flush=True)
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
