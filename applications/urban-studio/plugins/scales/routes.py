"""The server side of the plug-in Cities across scales (ADR 0128), under /api/projects/<project>/scales/.

    GET    scales                    the project's study (the city, the index and the model chosen) and the cities there are
    GET    scales/catalogue          the urban-science domain's catalogue of indexes, in brief, with its counts
    POST   scales/indexes            {case}                      every index at every level and every year, with the blocks' outlines
    POST   scales/describe           {case, model, use?}         a model of the plug-in read by the engine: levels, quantities,
                                                                 operators, laws, and what it still needs
    PUT    scales                    {study}                     the study kept (the work's own page)

What is computed keeps nothing and any of the people may ask it; an answer is kept in memory while its city's product is the same.
"""

from __future__ import annotations

import csv
import hashlib
import json
import re
import sys
import threading
from collections import Counter, OrderedDict
from pathlib import Path

from science import studio_host as host

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "tools"))
import _scales as s  # noqa: E402

TEXT_LIMIT = 1024 * 1024
LOCK = threading.Lock()
ANSWERS: "OrderedDict[str, dict]" = OrderedDict()
MOST = 24


def study_path(project: str) -> Path:
    return host.project_dir(project) / "scales" / "study.json"


def study(project: str) -> dict:
    p = study_path(project)
    return json.loads(p.read_text(encoding="utf-8")) if p.is_file() else {}


def case_of(body: dict) -> str:
    case = str(body.get("case") or "melbourne")
    if not re.fullmatch(r"[A-Za-z0-9_-]+", case):
        raise ValueError(f"not the name of a city: {case!r}")
    return case


def kept(key: str, make):
    with LOCK:
        if key in ANSWERS:
            ANSWERS.move_to_end(key)
            return ANSWERS[key]
    answer = make()
    with LOCK:
        ANSWERS[key] = answer
        while len(ANSWERS) > MOST:
            ANSWERS.popitem(last=False)
    return answer


def stamp(case: str) -> str:
    path = s.PRODUCTS / f"{case}.json"
    return f"{case}:{path.stat().st_mtime_ns}" if path.is_file() else case


def catalogue() -> dict:
    def make():
        rows = list(csv.DictReader(open(s.CATALOGUE, encoding="utf-8"), delimiter="\t"))
        brief = [{k: r[k] for k in ("id", "name", "class", "kind", "unit", "aggregation", "estimability", "role", "computed_by", "definition", "source", "estimable_from", "law")}
                 | {"section": r["sections"].split("; ")[0]} for r in rows]
        counts = {k: Counter(r[k] for r in rows) for k in ("class", "kind", "estimability", "role")}
        counts["aggregation"] = Counter(r["aggregation"].split(":")[0] for r in rows)
        counts["computed"] = sum(1 for r in rows if r["computed_by"])
        return {"indexes": brief, "counts": counts, "status": "drafted from the atlas; definitions and sources to be checked"}
    return kept(f"catalogue:{s.CATALOGUE.stat().st_mtime_ns}", make)


def indexes(body: dict) -> dict:
    case = case_of(body)

    def make():
        p = s.product(case)
        uses = s.grouped(p, s.withheld(p, case))
        r = s.scales.ask("run", document=s.index_document(p, uses, p["years"]))
        cat = s.catalogue()
        names = [lv["name"] for lv in p["levels"]]
        out = []
        for i, (cid, label, expr, words) in enumerate(s.INDEXES):
            out.append({"id": cid, "label": label, "words": words, "catalogue": cat[cid]["name"] if cid in cat else None,
                        "aggregation": cat[cid]["aggregation"] if cid in cat else "a value on summed quantities",
                        "levels": {lv: r["values"][f"i{i}" + ("" if k == 0 else f"_{lv}")] for k, lv in enumerate(names)}})
        for i, (cid, label, reducer, of, weight, words) in enumerate(s.AMONG):
            out.append({"id": cid, "label": label, "words": words, "catalogue": cat[cid]["name"] if cid in cat else None,
                        "aggregation": "recomputed from the units beneath",
                        "levels": {lv: r["values"][f"a{i}_{lv}"] for lv in names[1:]}})
        fine = p["levels"][0]
        return {"case": case, "title": p["title"], "years": r["times"], "levels": [{"name": lv["name"], "units": lv["units"], "parent": lv.get("parent")} for lv in p["levels"]],
                "rings": fine["rings"], "indexes": out, "credit": p["credit"]}
    return kept(f"indexes:{stamp(case)}", make)


def describe(body: dict) -> dict:
    case = case_of(body)
    model = str(body.get("model") or "growth")
    use = str(body.get("use") or "homes")

    def make():
        p = s.product(case)
        uses = s.grouped(p, s.withheld(p, case))
        t0, t1 = p["years"][0], p["years"][-1]
        if model == "growth":
            doc, _ = s.growth_document(p, uses, use, t0, t1)
        elif model == "vacancy":
            doc, _ = s.vacancy_document(p, uses, t0, t1)
            if body.get("undeclared"):
                doc["inputs"]["values"].pop("rho", None)   # to show what the model asks for when nothing is declared
        else:
            raise ValueError(f"no model is called {model} (growth, vacancy)")
        d = s.scales.ask("describe", document=doc)
        return {"model": model, "use": use, "described": d}
    return kept(f"describe:{stamp(case)}:{model}:{use}:{bool(body.get('undeclared'))}", make)


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The plug-in's routes; True when this answered."""
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] == "scales"):
        return False
    project, rest = parts[2], parts[4:]
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the project is seen by its people: ask the keeper for a key"}, 403)
        return True

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    what = rest[0] if rest else ""
    if method == "GET" and not rest:
        handler.answer(lambda: (exists(), ({"study": study(project), "cases": s.cases()}, 200))[1])
        return True
    if method == "GET" and what == "catalogue":
        handler.answer(lambda: (exists(), (catalogue(), 200))[1])
        return True
    if method == "POST" and what in ("indexes", "describe"):   # computed, and keeps nothing
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), ((indexes if what == "indexes" else describe)(body), 200))[1])
        return True
    refusal = host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    if method == "PUT" and not rest:
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def put():
                exists()
                st = body.get("study")
                if not isinstance(st, dict):
                    raise ValueError("a study is a JSON object")
                p = study_path(project)
                p.parent.mkdir(parents=True, exist_ok=True)
                with host.FILES:
                    p.write_text(json.dumps(st, ensure_ascii=False, indent=1), encoding="utf-8")
                host.record(project, host.who(handler), "kept the study across scales", st.get("case") or project, "")
                return {"kept": True}, 200
            handler.answer(put)
        return True
    return False
