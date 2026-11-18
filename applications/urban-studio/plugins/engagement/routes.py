"""The server side of the plug-in *From evidence to action* (the design of 2026-11-19, development/ideas/2026-11-19-an-engagement-with-a-city.md):
an engagement with a city kept as kybernesis's record, in the project's folder, on the API's host for project-based works.

A project that keeps an engagement has, beside what every project has:

    engagement/trajectory.json   the trajectory (noetarium.trajectory/1, api/cpp/science/steering.hpp): every block of the loop at every
                                 turn, made by a method, observed, entered by a person, amended or reviewed, with its files, sealed in
                                 archeion's journal; verified by the engine whenever it is read again
    engagement/transfers.json    the transfers assessed, newest first: a practice carried from one system to another by a lens, with its
                                 support factors

Files people bring are kept in the project's archive (science.project_archive), by their digests, and named in the trajectory.

Routes, under /api/projects/<project>/engagement:

    GET  (none)                         the trajectory, its view (per branch and turn, each block's entry and values), the predefined
                                        methods, the transfers, and the SHA-256 the trajectory is kept under
    GET  examples                       the plug-in's examples (examples/*.json): a constitution, practices and transfers declared
    POST open      {rules?}             an engagement opened on kybernesis's standard architecture with its first values
    POST start     {example}            an engagement opened (if none is kept) and laid out from an example at turn 0: its constitution
                                        filled, its practices observed, no proposals; each act by whoever asks, saying it came from the example
    POST act       {act, …, base}       one act (observe, fill, amend, review, branch): by whoever asks, in the role they state
    POST turn      {turn, branch?, from?, base}   a turn run by the predefined methods, or every turn from `from` again
    POST gather    {turn, branch?, base}          the project's ledger (the evidence its tools answered) observed at Collection
    POST files?name=&kind=&base=        a file kept in the project's archive and recorded in the trajectory
    POST transfer  {source, target, actions, practice, readings, …}   a transfer assessed, kept among the transfers
    GET  verify | replay | prov         whether it verifies; every computed value recomputed; the record as W3C PROV-JSON
    GET  diff?a=<branch>:<turn>&b=<branch>:<turn>                       what differs between two turns
    GET  resting?entry=<n> | agent=<name> | file=<digest>               what rests on it

`base` is the SHA-256 the page last read the trajectory under: an act on a trajectory that changed since is refused with the current one
(409), so that two people's acts never silently overwrite each other. Writes keep the host's rule (write_refusal); every act is written in
the project's journal as well as sealed in the trajectory's own.
"""

from __future__ import annotations

import hashlib
import json
import threading
from pathlib import Path

from science import project_archive
from science import steering
from science import studio_host as host
from science.engines import EngineError

EXAMPLES = Path(__file__).resolve().parent / "examples"
TEXT_LIMIT = 64 * 1024 * 1024
FILE_LIMIT = 512 * 1024 * 1024
LOCK = threading.Lock()


def folder(project: str) -> Path:
    return host.project_dir(project) / "engagement"


def sha(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def read(project: str) -> tuple[dict | None, str | None]:
    path = folder(project) / "trajectory.json"
    if not path.is_file():
        return None, None
    text = path.read_text(encoding="utf-8")
    return json.loads(text), sha(text)


def keep(project: str, trajectory: dict) -> str:
    text = json.dumps(trajectory, ensure_ascii=False)
    folder(project).mkdir(parents=True, exist_ok=True)
    host.replace_file(folder(project) / "trajectory.json", text)
    return sha(text)


def transfers(project: str) -> list:
    path = folder(project) / "transfers.json"
    return json.loads(path.read_text(encoding="utf-8")) if path.is_file() else []


def examples() -> list:
    out = []
    for path in sorted(EXAMPLES.glob("*.json")):
        e = json.loads(path.read_text(encoding="utf-8"))
        out.append({"name": path.stem, **e})
    return out


def state(project: str) -> dict:
    t, digest = read(project)
    out = {"kept": t is not None, "sha256": digest, "methods": steering.ask("methods")["methods"], "transfers": transfers(project)}
    if t is not None:
        out["trajectory"] = t
        out["view"] = steering.ask("view", trajectory=t)
    return out


class Conflict(Exception):
    pass


def changing(project: str, base: str | None, change):
    """Applies a change to the kept trajectory under the lock, refusing it when the trajectory changed since `base`."""
    with LOCK:
        t, digest = read(project)
        if t is None:
            raise KeyError("this project keeps no engagement yet: open one")
        if base and base != digest:
            raise Conflict(digest)
        answer = change(t)
        answer["sha256"] = keep(project, answer["trajectory"])
        return answer


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    if len(parts) < 4 or parts[:2] != ["api", "projects"] or parts[3] != "engagement":
        return False
    project, rest = parts[2], parts[4:]

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    def answer(f):
        def run():
            exists()
            try:
                return f()
            except Conflict as c:
                current = state(project)
                return {"message": "the engagement changed since you read it: here it is as it stands now", "conflict": True, "sha256": c.args[0], **current}, 409
            except EngineError as error:
                return {"message": str(error)}, 400
        handler.answer(run)
        return True

    if method == "GET":
        if not rest:
            return answer(lambda: (state(project), 200))
        if rest == ["examples"]:
            return answer(lambda: ({"examples": examples()}, 200))
        t_needed = lambda: read(project)[0] or (_ for _ in ()).throw(KeyError("this project keeps no engagement yet"))
        if rest == ["verify"]:
            return answer(lambda: (steering.ask("verify", trajectory=t_needed()), 200))
        if rest == ["replay"]:
            return answer(lambda: (steering.ask("replay", trajectory=t_needed()), 200))
        if rest == ["prov"]:
            return answer(lambda: (steering.ask("prov", trajectory=t_needed()), 200))
        if rest == ["diff"]:
            def diff():
                a, b = (str(query.get(k, "main:0")).rsplit(":", 1) for k in ("a", "b"))
                return steering.ask("diff", trajectory=t_needed(), a={"branch": a[0], "turn": int(a[1])}, b={"branch": b[0], "turn": int(b[1])}), 200
            return answer(diff)
        if rest == ["resting"]:
            def resting():
                key = next((k for k in ("entry", "agent", "file") if k in query), None)
                if key is None:
                    raise KeyError("say what to look from: entry, agent or file")
                value = int(query[key]) if key == "entry" else query[key]
                return steering.ask("resting", trajectory=t_needed(), **{key: value}), 200
            return answer(resting)
        return False

    what = rest[0] if rest else ""
    if what == "transfer" and method == "POST":   # computed; keeps the assessment among the transfers
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True
        refusal = host.write_refusal(handler)

        def assess():
            request = {k: body[k] for k in ("source", "target", "actions", "practice", "readings", "tolerance", "unshared", "factor_sources") if k in body}
            result = steering.ask("transfer", **request)
            if not refusal:
                kept = [{"title": str(body.get("title", "")), "when": steering.stamp(), "by": host.who(handler), "request": request, "result": result}] + transfers(project)
                folder(project).mkdir(parents=True, exist_ok=True)
                host.replace_file(folder(project) / "transfers.json", json.dumps(kept[:50], ensure_ascii=False))
                host.record(project, host.who(handler), "assessed a transfer", str(body.get("title", "")), result.get("standing", ""))
            return {"result": result, "kept": not refusal}, 200
        return answer(assess)

    refusal = host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    by = host.who(handler)

    if what == "open" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def open_():
            with LOCK:
                if read(project)[0] is not None and not body.get("again"):
                    raise ValueError("this project keeps an engagement already")
                t = steering.open_engagement(rules=body.get("rules"), by=by)
                digest = keep(project, t)
            host.record(project, by, "opened an engagement", "engagement")
            return {**state(project), "sha256": digest}, 200
        return answer(open_)

    if what == "start" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def start():
            e = next((x for x in examples() if x["name"] == body.get("example")), None)
            if e is None:
                raise KeyError(f"there is no example '{body.get('example')}'")
            with LOCK:
                t, _ = read(project)
                if t is None:
                    t = steering.open_engagement(by=by)
                c = e["constitution"]
                why = f"from the example {e['title']}"
                for what_, block_, outs in (("fill", "Constitution", [c.get("terms", {}), c.get("operators", []), c.get("standards", []), c.get("rules", [])]),
                                            ("observe", "Practice", [e.get("practices", [])]), ("fill", "Proposals", [[]])):
                    r = steering.act(t, what_, block=block_, turn=0, by=by, role="analyst", outputs=outs, reason=why)
                    if not r["outcome"]["holds"] and not r["outcome"]["undecided"]:
                        raise ValueError(r["outcome"]["reason"])
                    t = r["trajectory"]
                digest = keep(project, t)
            host.record(project, by, "laid out an engagement from an example", e["name"], e["title"])
            return {**state(project), "sha256": digest}, 200
        return answer(start)

    if what in ("act", "turn", "gather") and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True
        base = body.get("base")

        def act():
            if what == "act":
                fields = {k: v for k, v in body.items() if k not in ("base", "act", "by", "when", "trajectory")}
                r = changing(project, base, lambda t: steering.act(t, str(body.get("act", "")), by=by, **fields))
                host.record(project, by, f"{body.get('act')} in the engagement", f"{fields.get('branch', 'main')}/{fields.get('turn', '')}/{fields.get('block', fields.get('entry', ''))}",
                            r["outcome"].get("reason", "")[:300])
            elif what == "turn":
                r = changing(project, base, lambda t: steering.turn(t, int(body.get("turn", 0)), str(body.get("branch", "main")), body.get("from")))
                host.record(project, by, "ran a turn of the engagement", f"{body.get('branch', 'main')}/{body.get('turn', body.get('from', ''))}",
                            f"{len(r['progress']['made'])} made, {len(r['progress']['waiting'])} waiting, {len(r['progress']['stale'])} stale")
            else:
                ledger = project_archive.ledger(host.project_dir(project), host.read_project(project))
                items = steering.items_of(ledger)
                if not items:
                    raise ValueError("the project's tools have answered no evidence yet: run a tool that answers evidence first")
                role = str(body.get("role", "analyst"))
                r = changing(project, base, lambda t: steering.act(t, "observe", block="Collection", turn=int(body.get("turn", 0)), branch=str(body.get("branch", "main")),
                                                                 by=by, role=role, outputs=[items], reason=f"the project's ledger: {len(items)} items from {len(ledger['steps'])} steps"))
                host.record(project, by, "gathered the ledger into the engagement", f"{body.get('branch', 'main')}/{body.get('turn', 0)}", f"{len(items)} items")
            r["view"] = steering.ask("view", trajectory=r["trajectory"])
            return r, 200
        return answer(act)

    if what == "files" and method == "POST":
        data = handler.body(FILE_LIMIT)
        if data is None:
            return True

        def take():
            entry = project_archive.keep_holding(host.project_dir(project), query.get("name", "file"), data, query.get("kind", ""), by)
            host.record(project, by, "kept a file", entry["digest"], entry["name"])
            r = changing(project, query.get("base"), lambda t: steering.act(t, "attach", file={"name": entry["name"], "digest": entry["digest"], "kind": entry["kind"],
                                                                                               "bytes": entry["bytes"]}, by=by))
            r["file"] = entry
            return r, 200
        return answer(take)
    return False
