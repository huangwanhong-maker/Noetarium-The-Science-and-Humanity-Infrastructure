"""Multilateral Phronimos's own routes (ADR 0132): a project's record of an undertaking among many parties and its partnership, what
the multilateral engine makes of it on a date, the a priori power of the rules it declares, ideal points from its votes, bargaining and
the division of what is shared, on the API's host for project-based works (studio_host.configure(routes=...)).

A project is a folder under data/cache/multilateral-phronimos/projects/<id>/ (the host's project.json beside it):

- ``record.json``: ``noetarium.multilateral/1``, the whole of what the engine is asked, nothing else read:
    {schema, title, about, at,                           the date the record is read on (empty: every tie, whenever it held)
     undertaking: noetarium.undertaking/1,                participants, relation types, ties in time, designs of observation,
                                                          decisions with their authorities, evidence, those affected and venues
     partnership: noetarium.partnership/1,                members, goals, commitments, reports, contributions, dimensions,
                                                          values and each party's criteria of its benefit
     names: {id: name},                                   how participants are named on the page
     standard: {kind, threshold, least},                  by which each party's benefit is judged
     thresholds: {...},                                   of the diagnostics and of the signs of failure
     rules: [{id, title, source, text, reading, members, rule, sizes?}],   decision rules of the bodies in the undertaking
     votes: {members, items, votes, names, acknowledgment}?                a record of votes, for ideal points
     declared: [noetarium.rule/1],                        rules as their instruments state them, with their open terms (ADR 0134)
     questions: [{rule, question}],                       questions decided under a declared rule, with each member's act
     talks: {issues, parties, drafts},                    a negotiation at the table and at home, draft by draft (the page's
                                                          own bargaining problem is kept apart, as `negotiation`)
     events: [...], channels: {...}?,                     events as the reports they rest on, and the channels declared
     mandates: {mandates, reviews},                       mandates and their lifecycles
     reviews: {cycles, recommendations, answers, progress},  review cycles
     institutions: {landscape, transfers, windows},       the institutions the parties belong to, and what they gave one another
     sources: [{title, url, licence}]}                    where what the record holds came from

Routes, under /api/projects/<project>/:

  GET  record                          the record, with the SHA-256 it is kept under
  PUT  record      {record, base}      keep it (refused with the current one when it changed since `base`)
  POST reading     {at?}               the engine's answers on the date: the undertaking's and the partnership's, and every affair
                                       the record holds (questions, talks, events, mandates, reviews, institutions), each
                                       answered or refused apart (not `read`, which the host keeps for reading a source as a table)
  POST rule        {index}             a declared rule's a priori power and properties
  POST votes       {anchor?, bootstrap?}   ideal points of the record's votes
  POST example     {template?}         a plug-in's template laid out in the project

and, of no project:

  GET  /api/templates                  the templates and the rules the plug-ins bring
  POST /api/bargaining  {outcomes, disagreement}
  POST /api/division    {issues, a, b}
  POST /api/coalitional {members, worths}

A template may name a product of the data library (``from_product``): its records are read from data/cache/products/multilateral/
<product>/ when the template is laid out, never committed; when the product is not there, the template says how to make it. Every
change is written in the project's journal (with --people, sealed and told to the others' pages).
"""

from __future__ import annotations

import datetime as dt
import hashlib
import json
import sys
import threading
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import multilateral  # noqa: E402
from science import studio_host as host  # noqa: E402

TEXT_LIMIT = 32 * 1024 * 1024
PLUGINS = APP / "plugins"
PRODUCTS = ROOT / "data" / "cache" / "products" / "multilateral"
SCHEMA = "noetarium.multilateral/1"
BY: dict[int, str] = {}
LOCK = threading.Lock()


def sha(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def who() -> str:
    return BY.get(threading.get_ident(), host.PEOPLE["keeper"])


def check_name(name: str) -> str:
    name = Path(str(name)).name
    if not name or name.startswith(".") or any(c in name for c in '\\/:*?"<>|'):
        raise ValueError(f"not a name: {name!r}")
    return name


# ---- What the plug-ins bring -------------------------------------------------------------------------------------------------------


def templates() -> list[dict]:
    out = []
    for path in sorted(PLUGINS.glob("*/templates/*/template.json")):
        try:
            t = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        ready = True
        if t.get("from_product"):
            ready = (PRODUCTS / check_name(t["from_product"]) / "undertaking.json").is_file()
        out.append({"id": f"{path.parent.parent.parent.name}/{path.parent.name}", "title": t.get("title", path.parent.name), "about": t.get("about", ""),
                    "ready": ready, "make": t.get("make", "")})
    return out


def rules() -> list[dict]:
    out = []
    for path in sorted(PLUGINS.glob("*/rules/*.json")):
        try:
            r = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        r["id"] = f"{path.parent.parent.name}/{path.stem}"
        out.append(r)
    return out


# ---- The record ---------------------------------------------------------------------------------------------------------------------


def blank() -> dict:
    return {"schema": SCHEMA, "title": "", "about": "", "at": "",
            "undertaking": {"format": "noetarium.undertaking/1", "name": "", "types": [], "participants": [], "ties": [], "observations": [], "decisions": [], "constraints": []},
            "partnership": {"format": "noetarium.partnership/1", "name": "", "registered": "", "members": [], "goals": [], "commitments": [], "reports": [],
                            "contributions": [], "dimensions": [], "values": [], "interests": []},
            "names": {}, "standard": {"kind": "standing", "least": "clear", "threshold": 0.5}, "thresholds": {}, "rules": [], "sources": [],
            "declared": [], "questions": []}


def record_path(project: str) -> Path:
    return host.project_dir(project) / "record.json"


def read_record(project: str) -> dict:
    path = record_path(project)
    return json.loads(path.read_text(encoding="utf-8")) if path.is_file() else blank()


def keep_record(project: str, record: dict, base: str | None, what: str = "kept the record", note: str = "") -> dict:
    path = record_path(project)
    with LOCK:
        if base is not None and path.is_file():
            current = path.read_text(encoding="utf-8")
            if sha(current) != base:
                return {"message": "the record changed since it was read", "current": json.loads(current), "sha256": sha(current), "conflict": True}
        if not isinstance(record, dict) or record.get("schema", SCHEMA) != SCHEMA:
            raise ValueError(f"a record is a JSON object of the schema {SCHEMA}")
        record["schema"] = SCHEMA
        text = json.dumps(record, ensure_ascii=False, indent=1)
        with host.FILES:
            path.write_text(text, encoding="utf-8")
    host.record(project, who(), what, record.get("title") or project, note)
    return {"kept": True, "sha256": sha(text)}


def read(project: str, at: str | None = None) -> dict:
    r = read_record(project)
    date = r.get("at", "") if at is None else at
    out: dict = {"at": date}
    u = r.get("undertaking") or {}
    if u.get("participants"):
        out["undertaking"] = multilateral.ask("undertaking", undertaking=u, at=date, thresholds=r.get("thresholds") or {})
    p = r.get("partnership") or {}
    if p.get("members"):
        out["partnership"] = multilateral.ask("partnership", partnership=p, at=date, standard=r.get("standard") or {}, thresholds=r.get("thresholds") or {})
    affairs(r, date, out)
    return out


def affairs(r: dict, date: str, out: dict) -> None:
    """The affairs a record holds (ADR 0134), each asked of the engine apart; one that the engine refuses is said so in `wrong` and
    keeps none of the others from being read. A date left open is read as today, since mandates and reviews fall due."""
    day = date or dt.date.today().isoformat()
    wrong: dict = {}
    u = r.get("undertaking") or {}
    parties = [x["id"] for x in u.get("participants") or []]

    def attempt(part: str, call):
        try:
            out[part] = call()
        except Exception as e:  # noqa: BLE001 -- the engine's reason, said on the page
            wrong[part] = str(e)

    declared = {x.get("id"): x for x in r.get("declared") or []}
    if r.get("questions"):
        answers = []
        for q in r["questions"]:
            question = q.get("question") or {}
            rule = declared.get(q.get("rule"))
            if rule is None:
                answers.append({"question": question.get("id", ""), "rule": q.get("rule", ""), "wrong": f"no declared rule is called {q.get('rule')!r}"})
                continue
            try:
                answers.append({"question": question.get("id", ""), "rule": q.get("rule"), "answer": multilateral.ask("outcome", rule=rule, question=question)})
            except Exception as e:  # noqa: BLE001
                answers.append({"question": question.get("id", ""), "rule": q.get("rule"), "wrong": str(e)})
        out["questions"] = answers
    if (r.get("talks") or {}).get("issues"):
        attempt("talks", lambda: multilateral.ask("negotiation", negotiation=r["talks"]))
    if r.get("events"):
        request = {"events": r["events"], "parties": parties}
        if r.get("channels"):
            request["channels"] = r["channels"]
        attempt("events", lambda: multilateral.ask("events", **request))
    if (r.get("mandates") or {}).get("mandates"):
        request = {"registry": r["mandates"], "at": day}
        if parties:
            request["undertaking"] = u
        attempt("mandates", lambda: multilateral.ask("mandates", **request))
    if (r.get("reviews") or {}).get("recommendations"):
        attempt("reviews", lambda: multilateral.ask("reviews", record=r["reviews"], at=day, mild=int((r.get("thresholds") or {}).get("mild", 2))))
    inst = r.get("institutions") or {}
    if inst.get("landscape"):
        attempt("institutions", lambda: multilateral.ask("institutions", landscape=inst["landscape"], at=date, transfers=inst.get("transfers") or [],
                                                         windows=inst.get("windows") or [91, 365]))
    if wrong:
        out["wrong"] = wrong


def rule(project: str, index: int) -> dict:
    r = read_record(project)
    rs = r.get("rules") or []
    if not 0 <= index < len(rs):
        raise KeyError(f"the record declares no rule {index}")
    x = rs[index]
    request = {"members": x["members"], "rule": x["rule"]}
    if x.get("sizes"):
        request["sizes"] = x["sizes"]
    return {"rule": x, "answer": multilateral.ask("rule", **request)}


def votes(project: str, anchor: str = "", bootstrap: int = 0) -> dict:
    r = read_record(project)
    v = r.get("votes")
    if not v or not v.get("members"):
        raise ValueError("the record holds no votes: bring a record of votes (data/products/multilateral_cases.py general-assembly) to read ideal points")
    request = {"members": v["members"], "items": v["items"], "votes": [[-1 if x is None else int(x) for x in row] for row in v["votes"]],
               "anchor": anchor or v.get("anchor") or v["members"][0]}
    if bootstrap:
        request["bootstrap"] = int(bootstrap)
    return {"answer": multilateral.ask("ideal_points", timeout=900.0, **request), "names": v.get("names", {}), "acknowledgment": v.get("acknowledgment", "")}


def example(project: str, template: str | None = None) -> dict:
    """A plug-in's template laid out in the project."""
    choices = templates()
    if not choices:
        raise KeyError("no plug-in brings a template")
    ident = template or choices[0]["id"]
    plugin, _, name = ident.partition("/")
    folder = PLUGINS / check_name(plugin) / "templates" / check_name(name)
    path = folder / "template.json"
    if not path.is_file():
        raise KeyError(f"there is no template {ident}")
    t = json.loads(path.read_text(encoding="utf-8"))
    rec = blank()
    if (folder / "record.json").is_file():
        rec.update(json.loads((folder / "record.json").read_text(encoding="utf-8")))
    if t.get("from_product"):
        product = PRODUCTS / check_name(t["from_product"])
        if not (product / "undertaking.json").is_file():
            raise FileNotFoundError(f"the template reads a product of the data library that is not made yet: {t.get('make', '')}".strip())
        und = json.loads((product / "undertaking.json").read_text(encoding="utf-8"))
        par = json.loads((product / "partnership.json").read_text(encoding="utf-8"))
        rec["undertaking"], rec["partnership"] = und, par
        names = dict(par.pop("names", {}) or {})
        for p in und.get("participants", []):
            names.setdefault(p["id"], p.get("name") or p["id"])
        rec["names"] = names
        about = product / "about.json"
        if about.is_file():
            a = json.loads(about.read_text(encoding="utf-8"))
            rec["sources"] = [{"title": a.get("title", ""), "url": "https://d-portal.org/ctrack.html#view=act&aid=" + a.get("source", {}).get("activity", ""),
                               "licence": a.get("source", {}).get("licence", ""), "read": a.get("read", "")}]
    for key in ("title", "about", "at", "standard", "thresholds"):
        if key in t:
            rec[key] = t[key]
    for rid in t.get("rules", []):
        found = next((x for x in rules() if x["id"] == rid), None)
        if found:
            rec["rules"].append(found)
    keep_record(project, rec, None, "laid out a template", ident)
    return {"template": ident, "title": rec.get("title", "")}


# ---- The routes ----------------------------------------------------------------------------------------------------------------------


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The work's own routes; True when this answered."""
    BY[threading.get_ident()] = host.who(handler)
    if parts[:2] == ["api", "templates"] and method == "GET" and len(parts) == 2:
        handler.answer(lambda: ({"templates": templates(), "rules": rules()}, 200))
        return True
    for op in ("bargaining", "division", "coalitional"):
        if parts[:2] == ["api", op] and method == "POST" and len(parts) == 2:
            body = handler.json_body(TEXT_LIMIT)
            if body is not None:
                handler.answer(lambda: (multilateral.ask(op, **{k: v for k, v in body.items() if k != "op"}), 200))
            return True
    OWN = ("record", "reading", "rule", "votes", "example")
    if not (len(parts) == 4 and parts[:2] == ["api", "projects"] and parts[3] in OWN):
        return False
    project, what = parts[2], parts[3]
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the record is seen by its people: ask the keeper for a key"}, 403)
        return True

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    if method == "GET" and what == "record":
        def get():
            exists()
            r = read_record(project)
            return {"record": r, "sha256": sha(json.dumps(r, ensure_ascii=False, indent=1)) if record_path(project).is_file() else None}, 200
        handler.answer(get)
        return True
    # What is computed changes nothing and may be asked by any of the people; what is kept, by a contributor.
    if method == "POST" and what in ("reading", "rule", "votes"):
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            if what == "reading":
                handler.answer(lambda: (exists(), (read(project, body.get("at")), 200))[1])
            elif what == "rule":
                handler.answer(lambda: (exists(), (rule(project, int(body.get("index", 0))), 200))[1])
            else:
                handler.answer(lambda: (exists(), (votes(project, str(body.get("anchor", "")), int(body.get("bootstrap", 0) or 0)), 200))[1])
        return True
    refusal = host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    if what == "record" and method == "PUT":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def put():
                exists()
                answer = keep_record(project, body.get("record"), body.get("base"), body.get("what") or "kept the record", body.get("note", ""))
                return answer, 409 if answer.get("conflict") else 200
            handler.answer(put)
        return True
    if what == "example" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (example(project, body.get("template")), 200))[1])
        return True
    return False
