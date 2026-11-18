"""The server side of the plug-in *Needs* (ADR 0125): a project's search for needs, kept in the project's folder, on the API's host for
project-based works (a plug-in's routes.py).

A project that searches for needs has, beside what every project has:

    needs/study.json       the dataset chosen (a case of the data library) and the search's options, stage by stage
    needs/state.json       the search's saved state, noetarium.needs-search/1: the evolution, the admissions, the confirmations and
                           their sealed journal, the holdout's and the online rule's states (api/cpp/science/needs.hpp)
    needs/report.json      what the last generations found, for the page
    needs/proposals.json   what was proposed from outside (a person, a language model), each with its source and what was refused

Routes, under /api/projects/<project>/needs:

    GET  (none)                  the study, the cases there are, the run in progress, the last report and the proposals
    GET  place?case=             a case's areas and suppliers, for drawing (not its rows)
    POST describe {case, options}         what the data hold
    POST read     {case, text}            statements read and checked, line by line
    POST test     {case, statements}      statements put to every test, with their confirmations where the search has one
    POST map      {case, statement}       a statement's people and gap area by area
    POST generate {case, generator, n}    what a generator makes
    PUT  (none)   {study}                 the study kept
    POST run      {case?, options?, generations, step?, fresh?, proposals?, model?}   a search started, or continued from its state,
                                          in steps of a few generations, kept after each; with `model` ({rounds, n, description}),
                                          a language model's proposals are asked between steps (the work's own page at this
                                          computer's own address only)
    GET  run                     the run in progress: its generation, what it is doing, and its last words
    POST stop                    the run stopped after its current step
    POST propose  {case, description?, vary?, n}   a language model's proposals, read by the engine, kept in proposals.json
                                          (the work's own page at this computer's own address only)
    POST forget                  the search's state and report set aside (kept as state-<time>.json), to begin again

What is computed is the needs domain's, asked of the plug-in's engine through science.needs. Writes keep the host's rule
(write_refusal); every change is written in the project's journal.
"""

from __future__ import annotations

import datetime as dt
import json
import re
import threading
import time
from pathlib import Path

from science import models
from science import needs as nd
from science import studio_host as host

TEXT_LIMIT = 16 * 1024 * 1024
LOCK = threading.Lock()
RUNS: dict[str, dict] = {}
DEFAULTS = {"case": "melbourne-assistance", "options": {}}


def now() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def folder(project: str) -> Path:
    return host.project_dir(project) / "needs"


def read_json(path: Path, fallback):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return fallback


def write_json(path: Path, value) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    host.replace_file(path, json.dumps(value, ensure_ascii=False, separators=(",", ":")))


def study(project: str) -> dict:
    s = read_json(folder(project) / "study.json", {})
    return {**DEFAULTS, **s}


def check_case(case: str) -> str:
    if not re.fullmatch(r"[A-Za-z0-9_.-]+", case or ""):
        raise ValueError(f"not the name of a dataset: {case!r}")
    return case


DATA: dict[str, tuple[str, dict]] = {}


def dataset(case: str) -> dict:
    """A case's dataset, read once while its file does not change."""
    path = nd.PRODUCTS / f"{check_case(case)}.json"
    if not path.is_file():
        raise KeyError(f"the data library has made no dataset called {case}: python data/products/needs_tables.py {case}")
    stamp = f"{path.stat().st_mtime_ns}:{path.stat().st_size}"
    with LOCK:
        if case in DATA and DATA[case][0] == stamp:
            return DATA[case][1]
    d = nd.data(path)
    with LOCK:
        DATA[case] = (stamp, d)
    return d


def place(case: str) -> dict:
    d = dataset(case)
    return {k: d.get(k) for k in ("case", "title", "place", "indicators", "domains", "attributes", "where", "suppliers", "reach", "declared", "credit", "map", "rules", "report")}


def options_of(project: str, body: dict) -> dict:
    o = body.get("options")
    return o if isinstance(o, dict) else study(project).get("options", {})


def case_of(project: str, body: dict) -> str:
    return check_case(str(body.get("case") or study(project).get("case") or DEFAULTS["case"]))


def model(project: str):
    s = read_json(models.settings_folder() / "urban-studio.json", {})
    return models.provider(s.get("provider", "anthropic"), **({"model": s["model"]} if s.get("model") else {}), **({"effort": s["effort"]} if s.get("effort") else {}))


def log_proposal(project: str, p: dict) -> None:
    path = folder(project) / "proposals.json"
    items = read_json(path, [])
    items.append({"when": now(), **{k: p[k] for k in ("source", "by", "grade", "read", "refused") if k in p}})
    write_json(path, items[-200:])


# ---- the run ----------------------------------------------------------------------------------------------------------------------

def run_status(project: str) -> dict:
    with LOCK:
        r = RUNS.get(project)
        return dict(r) if r else {"running": False}


def run(project: str, by: str, body: dict) -> dict:
    """Starts a run in a thread: `generations` more, `step` at a time, the state kept after each."""
    with LOCK:
        if RUNS.get(project, {}).get("running"):
            raise RuntimeError("a search is already running in this project")
    case = case_of(project, body)
    d = dataset(case)
    options = options_of(project, body)
    generations = max(1, min(400, int(body.get("generations") or 10)))
    step = max(1, min(generations, int(body.get("step") or 2)))
    fresh = bool(body.get("fresh"))
    proposals = [p for p in body.get("proposals") or [] if isinstance(p, dict) and p.get("text")]
    wants_model = isinstance(body.get("model"), dict)
    s = study(project)
    s.update({"case": case, "options": options})
    write_json(folder(project) / "study.json", s)
    status = {"running": True, "by": by, "started": now(), "case": case, "of": generations, "done": 0, "phase": "starting", "words": [], "stop": False}
    with LOCK:
        RUNS[project] = status

    def say(text: str) -> None:
        with LOCK:
            status["words"] = (status["words"] + [f"{now()[11:19]} {text}"])[-40:]
        host.emit(project, {"what": "needs run", "by": by, "about": text})

    def work() -> None:
        state_path, report_path = folder(project) / "state.json", folder(project) / "report.json"
        state = None if fresh else read_json(state_path, None)
        pending = list(proposals)
        provider = None
        if wants_model:
            try:
                provider = model(project)
                why = provider.unavailable()
                if why:
                    raise models.Refusal(why)
            except models.Refusal as e:
                say(f"no language model: {e}")
                provider = None
        try:
            done, report = 0, None
            m = body.get("model") or {}
            if provider is not None and m.get("description"):
                status["phase"] = "asking the model"
                p = nd.propose(d, None, int(m.get("n") or 12), provider, description=str(m["description"]))
                log_proposal(project, p)
                say(f"the model proposed {len(p['read'])} statements that read, {len(p['refused'])} refused")
                if p["text"]:
                    pending.append({"text": p["text"], "source": p["source"]})
            while done < generations and not status["stop"]:
                k = min(step, generations - done)
                status["phase"] = f"generations {done + 1}–{done + k}"
                request = {"data": d, "options": options, "generations": k, "proposals": pending}
                if state:
                    request["state"] = state
                t = time.time()
                answer = nd.ask("evolve", **request)
                state, report = answer["state"], answer["report"]
                for r in answer.get("refused", []):
                    say(f"refused from {r['source']}: {r['why']}")
                pending = []
                write_json(state_path, state)
                write_json(report_path, report)
                done += k
                with LOCK:
                    status["done"] = done
                confirmed = sum(1 for c in report["confirmations"] if c["confirmed"])
                say(f"generation {report['generation']}: {len(report['archive'])} kept, {len(report['confirmations'])} tested on the part kept aside, "
                    f"{confirmed} confirmed ({time.time() - t:.0f} s)")
                if provider is not None and done < generations and int(m.get("rounds") or 0) > 0:
                    status["phase"] = "asking the model"
                    try:
                        p = nd.propose(d, report, int(m.get("n") or 12), provider)
                        log_proposal(project, p)
                        say(f"the model proposed {len(p['read'])} statements that read, {len(p['refused'])} refused")
                        if p["text"]:
                            pending.append({"text": p["text"], "source": p["source"]})
                    except models.Refusal as e:
                        say(f"the model gave nothing: {e}")
            host.record(project, by, "searched for needs", case, f"to generation {report['generation'] if report else '?'}")
            say("stopped" if status["stop"] else "done")
        except Exception as e:  # noqa: BLE001
            say(f"the search failed: {e}")
        finally:
            with LOCK:
                status["running"] = False
                status["phase"] = "done"
            host.emit(project, {"what": "needs run done", "by": by, "about": case})

    threading.Thread(target=work, daemon=True).start()
    return {"started": True, "of": generations, "step": step}


# ---- routes -----------------------------------------------------------------------------------------------------------------------

def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The plug-in's routes; True when this answered."""
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] == "needs"):
        return False
    project, rest = parts[2], parts[4:]
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the search is seen by its people: ask the keeper for a key"}, 403)
        return True

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    if method == "GET":
        if not rest:
            def get():
                exists()
                f = folder(project)
                return {"study": study(project), "cases": nd.cases(), "run": run_status(project), "report": read_json(f / "report.json", None),
                        "proposals": read_json(f / "proposals.json", []), "model": models.key_state("anthropic")}, 200
            handler.answer(get)
            return True
        if rest == ["place"]:
            given = query.get("case")
            given = given[0] if isinstance(given, list) and given else given
            handler.answer(lambda: (exists(), (place(check_case(str(given or study(project)["case"]))), 200))[1])
            return True
        if rest == ["run"]:
            handler.answer(lambda: (exists(), (run_status(project), 200))[1])
            return True
        return False
    what = rest[0] if rest else ""
    computed = {"describe", "read", "test", "map", "generate"}
    if what in computed and method == "POST":  # computed, and keeps nothing: any of the people may ask
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def ask():
            exists()
            d = dataset(case_of(project, body))
            o = options_of(project, body)
            if what == "describe":
                return nd.ask("describe", data=d, options=o), 200
            if what == "read":
                return nd.ask("read", data=d, text=str(body.get("text") or "")), 200
            if what == "test":
                state = read_json(folder(project) / "state.json", None)
                request = {"data": d, "options": o, "statements": [str(s) for s in body.get("statements") or []][:40]}
                if state and study(project).get("case") == case_of(project, body):
                    request["state"] = state
                return nd.ask("test", **request), 200
            if what == "map":
                return nd.ask("map", data=d, options=o, statement=str(body.get("statement") or "")), 200
            return nd.ask("generate", data=d, options=o, generator=str(body.get("generator") or "beam"), n=int(body.get("n") or 20)), 200
        handler.answer(ask)
        return True
    refusal = host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    by = host.who(handler)
    if not rest and method == "PUT":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def put():
                exists()
                s = study(project)
                given = body.get("study") or {}
                if "case" in given:
                    s["case"] = check_case(str(given["case"]))
                if isinstance(given.get("options"), dict):
                    s["options"] = given["options"]
                write_json(folder(project) / "study.json", s)
                host.record(project, by, "kept the search's options", s["case"])
                return {"study": s}, 200
            handler.answer(put)
        return True
    if what == "run" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def go():
                exists()
                if isinstance(body.get("model"), dict) and not host.at_this_computer(handler):
                    raise PermissionError("a language model is asked only from the work's own page at this computer's own address")
                return run(project, by, body), 200
            handler.answer(go)
        return True
    if what == "stop" and method == "POST":
        def stop():
            with LOCK:
                if project in RUNS:
                    RUNS[project]["stop"] = True
            return {"stopping": True}, 200
        handler.answer(stop)
        return True
    if what == "propose" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def propose():
                exists()
                if not host.at_this_computer(handler):
                    raise PermissionError("a language model is asked only from the work's own page at this computer's own address")
                d = dataset(case_of(project, body))
                report = read_json(folder(project) / "report.json", None)
                try:
                    p = nd.propose(d, report, int(body.get("n") or 12), model(project), description=body.get("description") or None,
                                   vary=[str(v) for v in body.get("vary") or []] or None)
                except models.Refusal as e:
                    return {"message": str(e)}, 502
                log_proposal(project, p)
                host.record(project, by, "asked a language model for statements", "", f"{len(p['read'])} read, {len(p['refused'])} refused")
                return p, 200
            handler.answer(propose)
        return True
    if what == "forget" and method == "POST":
        def forget():
            exists()
            if run_status(project).get("running"):
                raise RuntimeError("a search is running: stop it first")
            f = folder(project)
            stamp = now().replace(":", "")
            for name in ("state.json", "report.json"):
                if (f / name).is_file():
                    (f / name).rename(f / f"{name[:-5]}-{stamp}.json")
            host.record(project, by, "set the search aside to begin again")
            return {"forgotten": True}, 200
        handler.answer(forget)
        return True
    return False
