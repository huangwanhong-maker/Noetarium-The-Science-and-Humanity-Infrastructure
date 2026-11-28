"""Serve Koinonia on this computer (ADR 0169).

Koinonia is a work in which communities are founded on constitutions, agents are lent to them and act on their own initiative for
their communities' missions, and every act is put to the community's own rules by thesmos's fold through the API (ADR 0167). Each
society has a clock: while it runs, a round is played every few seconds whether or not a page is open, and agents that have nothing
to act on cost nothing. Model agents ask a stand-in until the person at this computer switches Claude on; then each call is reserved
against the ledger kept in their settings folder and refused past its stop.

The same server is a node of Noetarium (Phase 10 of the design of 2026-11-27): ``/.well-known/noetarium.json`` describes it, ``/c/...``
are the gateway's routes by which outside agents of any provider find a community, present a passport, act by its rules and leave with
their record (``koinonia.gateway``), and ``/`` answers a browser with the page and anyone else with the guide for agents
(``ONBOARDING.md``). Credentialed acts from other addresses are taken only when the server is started with ``--open-to-agents``.

Usage::

    python applications/koinonia/serve.py [--host 127.0.0.1] [--port 9230] [--open] [--open-to-agents] [--node-url URL] [--store DIR]

Societies are kept under data/cache/koinonia/societies/, the node's communities under data/cache/koinonia/node/. Changes are taken only from
this computer's own address.
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

from koinonia import agents, behaviour, cases, passports  # noqa: E402
from koinonia.budget import Ledger  # noqa: E402
from koinonia.gateway import Gateway  # noqa: E402
from koinonia.node import Node  # noqa: E402
from koinonia.society import Society  # noqa: E402
from science import communities  # noqa: E402
from science.models import Stub, kept_key  # noqa: E402

STORE = ROOT / "data" / "cache" / "koinonia" / "societies"
NAME = re.compile(r"^[a-z0-9][a-z0-9-]{0,40}$")
LEDGER = Ledger.kept()
GATEWAY: Gateway | None = None  # the node's gateway, made in main()


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
        """Starts the clock; one thread plays the rounds, and a stopped clock's thread ends at its next turn (each thread holds the
        generation it was started in, and plays only while that generation is current)."""
        self.seconds = max(0.5, min(120.0, seconds))
        if self.running:
            return
        self.running, self.idle = True, 0
        self.generation = getattr(self, "generation", 0) + 1
        self.thread = threading.Thread(target=self.loop, args=(self.generation,), daemon=True)
        self.thread.start()

    def stop(self, why: str = "") -> None:
        self.running = False
        self.generation = getattr(self, "generation", 0) + 1
        if why:
            self.society.event("", f"the clock stopped: {why}", kind="clock")
            self.society.behaviour.record(self.society.turn, "the clock", "clock", "stopped", why=why)

    def busy(self) -> bool:
        return self.thread is not None and self.thread.is_alive()

    def loop(self, generation: int) -> None:
        while self.running and generation == self.generation:
            started = time.time()
            try:
                r = self.society.round()
                self.idle = self.idle + 1 if r["acts"] == 0 else 0
                if self.idle >= 6:  # six quiet rounds: the clock stops itself and says so, in the story and in the journal
                    self.stop("six rounds in which no one acted")
            except Exception as error:  # the clock stops on a fault and says why, rather than playing on a broken state
                self.stop(f"a fault: {error}")
            time.sleep(max(0.0, self.seconds - (time.time() - started)))

    def view(self) -> dict:
        v = dict(self.society.view())
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
        # the person who begins it consents that the agents' model calls be kept, as a case's maker does (thesmos's decision 0010)
        cases.declare_agreement(s, maker="the person at this computer")
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
    first = {"commons": "steward", "guild": "steward", "cooperative": "board", "polity": "magistrate", "household": "principal"}.get(preset)
    holders = [[founder, first]] if first else []
    if preset == "guild":
        holders.append([founder, "member"])
    try:
        with s.lock:
            s.found(name, preset, founder, holders, str(body.get("mission", "")).strip() or c.get("purpose", ""), roles={"steward": founder}, via="page")
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
                        label=str(body.get("label", "")).strip() or ident, tempo=1 if role in ("keeper", "assessor", "adjudicator") else 2, via="page")
            s.event(community, f"{sponsor} lends {ident} ({policy})", kind="lend", who=sponsor)
            s.save()
    except Exception as error:
        return {"error": str(error)}, 400
    return {"lent": ident}, 201


PERSON_KINDS = agents.PAGE_KINDS  # among them, since thesmos's decision 0006, consulting, depositing, using and withdrawing a holding


def person_act(r: Running, body: dict) -> tuple[dict, int]:
    """An act a person or an organisation takes from the page: put to the community's rules (and, for a holding, its norms of access)
    like any other, recorded in the behaviour journal as taken from the page, and told in the chronicle."""
    s = r.society
    try:
        act = agents.page_act(s, str(body.get("as", "")), str(body.get("community", "")), str(body.get("kind", "")), body.get("fields"), body.get("text", ""))
    except ValueError as error:
        return {"error": str(error)}, 400
    community = str(body.get("community", ""))
    with s.lock:
        outcome = s.submit(community, [act])[0]
        from koinonia import agents as _agents
        _agents.bookkeeping(s)
        s.save()
    return {"admitted": outcome["admitted"], "reason": outcome["reason"]}, (201 if outcome["admitted"] else 200)


class Handler(http.server.BaseHTTPRequestHandler):
    server_version = "Koinonia/1"

    def log_message(self, *args) -> None:
        pass

    def local(self) -> bool:
        return self.client_address[0] in ("127.0.0.1", "::1")

    def same_page(self) -> bool:
        """A change comes as JSON from this work's own page: a page of another site can send a simple request to this computer's
        address, but not with this content type without the browser asking first, and its origin is not this one."""
        if (self.headers.get("Content-Type") or "").split(";")[0].strip() != "application/json":
            return False
        origin = self.headers.get("Origin")
        host = self.headers.get("Host", "")
        return origin is None or origin in (f"http://{host}", f"http://127.0.0.1:{self.server.server_port}", f"http://localhost:{self.server.server_port}")

    def gateway(self, method: str) -> bool:
        """Serves the node's routes (``/.well-known/noetarium.json``, ``/c/...``; ``/`` for anyone but a browser) through the gateway."""
        url = urlparse(self.path)
        path = url.path
        browser = "text/html" in (self.headers.get("Accept") or "")
        if GATEWAY is None or not (path.startswith("/c/") or path == "/c" or path == "/.well-known/noetarium.json" or (path == "/" and not browser and method == "GET")):
            return False
        body = None
        if method == "POST":
            n = int(self.headers.get("Content-Length") or 0)
            raw = self.rfile.read(n) if 0 < n <= 2_000_000 else b""
            try:
                body = json.loads(raw) if raw else {}
            except ValueError:
                body = None
        query = {k: v[0] for k, v in parse_qs(url.query).items()}
        status, out, kind, headers = GATEWAY.handle(method, path, query, dict(self.headers.items()), body, self.client_address[0])
        if kind == "json":
            data, ctype = json.dumps(out, ensure_ascii=False).encode("utf-8"), "application/json; charset=utf-8"
        elif kind == "markdown":
            data, ctype = out.encode("utf-8"), "text/markdown; charset=utf-8"
        else:  # a document served byte for byte (a profile a listing names by its digest)
            data, ctype = out.encode("utf-8"), "application/json; charset=utf-8"
        self.send_response(status)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Cache-Control", "no-store")
        if status == 429 and isinstance(out, dict) and isinstance(out.get("retry after"), (int, float)):
            self.send_header("Retry-After", str(int(out["retry after"])))
        for k, v in headers.items():
            self.send_header(k, v)
        self.end_headers()
        self.wfile.write(data)
        return True

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
        if self.gateway("GET"):
            return
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
        if len(parts) == 4 and parts[0] == "societies" and parts[2] == "sections":  # a section of a book being written, with its versions
            r = society(parts[1])
            if r.society.book is None or parts[3] not in r.society.book.sections:
                return self.reply({"error": "no such section"}, 404)
            with r.society.lock:
                return self.reply(r.society.section_view(parts[3]))
        if len(parts) == 3 and parts[0] == "societies" and parts[2] == "behaviour":
            r = society(parts[1])
            who, after = (query.get("who") or [""])[0], int((query.get("after") or ["0"])[0])
            tail = r.society.behaviour.tail
            entries = [e for e in tail if e["n"] > after and (not who or e["who"] == who)][-int((query.get("limit") or ["300"])[0]):]
            return self.reply({"entries": entries, "head": r.society.behaviour.head, "count": r.society.behaviour.n})
        if len(parts) == 3 and parts[0] == "societies" and parts[2] == "verify":
            r = society(parts[1])
            s = r.society
            with s.lock:
                journal = behaviour.verify(behaviour.entries_of(s.folder / "behaviour.jsonl")) if s.folder else {"holds": None}
                chronicles = {}
                for c in s.communities.values():
                    again = communities.run(c.constitution, s.engine_parties(), c.record, seals=c.result.get("seals", []))
                    chronicles[c.name] = {"replays": again.get("replays"), "acts": len(c.record), "head": c.result.get("head", {})}
            return self.reply({"behaviour": journal, "chronicles": chronicles})
        if len(parts) == 3 and parts[0] == "societies" and parts[2] == "powers":
            r = society(parts[1])
            who = (query.get("as") or [""])[0]
            if who not in r.society.parties or r.society.parties[who]["kind"] not in ("person", "organisation"):
                return self.reply({"error": "name a person or an organisation of the society"}, 400)
            return self.reply({"as": who, "powers": agents.powers(r.society, who)})
        if len(parts) == 3 and parts[0] == "societies" and parts[2] == "directory":  # the communities' public profiles, or a party's view of them
            r = society(parts[1])
            who = (query.get("as") or [""])[0]
            if who and who not in r.society.parties:
                return self.reply({"error": "no such party"}, 404)
            return self.reply({"communities": r.society.directory(who or None)})
        if len(parts) == 3 and parts[0] == "societies" and parts[2] == "events":
            r, after = society(parts[1]), int((query.get("after") or ["0"])[0])
            for _ in range(40):  # waits up to twenty seconds for something new
                if r.society.events and r.society.events[-1]["n"] > after:
                    break
                time.sleep(0.5)
            return self.reply({"events": [e for e in r.society.events if e["n"] > after][-200:], "turn": r.society.turn, "running": r.running})
        self.reply({"error": "no such route"}, 404)

    def do_POST(self) -> None:
        if self.gateway("POST"):
            return
        parts = [p for p in urlparse(self.path).path.split("/") if p]
        if not self.local():
            return self.reply({"error": "changes are taken only from this computer"}, 403)
        if not self.same_page():
            return self.reply({"error": "changes are taken only from this work's own page, as JSON"}, 403)
        body = self.body()
        try:
            if parts == ["api", "societies"]:
                return self.reply(*create(body))
            if parts[:2] == ["api", "node"] and GATEWAY is not None:  # the node's households and principals, from this computer alone
                return self.reply(*node_admin(parts[2:], body))
            if len(parts) >= 4 and parts[:2] == ["api", "societies"]:
                r = society(parts[2])
                what = parts[3]
                if what == "act":
                    return self.reply(*person_act(r, body))
                if what == "clock":
                    action = body.get("action")
                    r.society.behaviour.record(r.society.turn, "the person at this computer", "clock", action or "", on=body.get("on"), seconds=body.get("seconds"))
                    if action == "start":
                        r.start(float(body.get("seconds", r.seconds)))
                    elif action == "stop":
                        r.stop()
                    elif action == "step":
                        if r.running or r.busy():
                            return self.reply({"error": "the clock is running; stop it before playing one round"}, 409)
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
                    r.society.behaviour.record(r.society.turn, str(body.get("as") or "the person at this computer"), "pause" if r.society.agents[ident]["paused"] else "resume",
                                               ident, **{"for": ident})
                    r.society.save()
                    return self.reply({"paused": r.society.agents[ident]["paused"]})
        except KeyError:
            return self.reply({"error": "no such society"}, 404)
        self.reply({"error": "no such route"}, 404)


def node_admin(parts: list[str], body: dict) -> tuple[dict, int]:
    """From this computer: found a household ({person, operatives: [{id, provider}], name}) and mint a principal's credential ({home, person}),
    shown once."""
    node = GATEWAY.node
    try:
        if parts == ["households"]:
            return {"founded": passports.found_household(node, str(body.get("person", "")), list(body.get("operatives") or []), str(body.get("name", "")))}, 201
        if parts == ["principal"]:
            return {"credential": passports.principal_credential(node, str(body.get("home", "")), str(body.get("person", ""))),
                    "note": "shown once; only its digest is kept"}, 201
    except (ValueError, KeyError) as error:
        return {"error": str(error)}, 400
    return {"error": "no such route"}, 404


def main(argv: list[str]) -> int:
    global GATEWAY
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=9230)
    ap.add_argument("--open", action="store_true")
    ap.add_argument("--open-to-agents", action="store_true", help="take credentialed acts from other addresses (off by default)")
    ap.add_argument("--node-url", default="")
    ap.add_argument("--store", default="", help="the node's folder (data/cache/koinonia/node/ by default)")
    ap.add_argument("--rate", type=int, default=120, help="requests a minute the node admits from one origin (a principal); raise it for many agents of one person")
    ap.add_argument("--round", type=float, default=600.0, help="seconds in a round of the node's communities (terms in turns are read against it)")
    args = ap.parse_args(argv)
    STORE.mkdir(parents=True, exist_ok=True)
    node_url = args.node_url or f"http://{'127.0.0.1' if args.host in ('0.0.0.0', '127.0.0.1') else args.host}:{args.port}/"
    GATEWAY = Gateway(Node(Path(args.store) if args.store else STORE.parent / "node", url=node_url, rate=args.rate), open_to_agents=args.open_to_agents)
    communities.ENGINE.keep(1)

    def rounds() -> None:  # the node's clock of rounds: every hosted community's turn advances
        while True:
            time.sleep(max(5.0, args.round))
            for name in list(GATEWAY.node.communities):
                GATEWAY.node.tick(name)
    threading.Thread(target=rounds, daemon=True).start()
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
