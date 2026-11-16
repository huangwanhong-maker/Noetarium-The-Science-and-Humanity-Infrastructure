"""The server side of the plug-in Population dynamics (ADR 0124): the places the data library has made, a place's field, and the
engine asked about it, under /api/projects/<project>/population/.

    GET    population                the project's study (the place chosen, the law, the sources placed) and the places there are
    GET    population/field?case=    a place's field (noetarium.population-field/1), as the data library wrote it
    POST   population/growth         {case}                                       the whole place's growth, three forms
    POST   population/learn          {case, held_out?, models?, law?, weights?}   every law fitted and judged on the years kept aside
    POST   population/modes          {case, show?}                                the record's dynamic modes
    POST   population/simulate       {case, model | law [+ coefficients], to?, every?, sources?, scale?}   a law run ahead
    PUT    population                {study}                                      the study kept (the work's own page)
    POST   population/place          {lat, lon, half_km, name}                    a place of one's own made by the data library (the work's own page)

What is computed keeps nothing and any of the people may ask it; an answer is kept in memory while its field and question are the
same. What is kept, and making a place (a minute of the data library's fetching), are for the work's own page or its people.
"""

from __future__ import annotations

import hashlib
import json
import re
import subprocess
import sys
import threading
from collections import OrderedDict
from pathlib import Path

from science import population as pd
from science import studio_host as host

ROOT = Path(__file__).resolve().parents[4]
TEXT_LIMIT = 4 * 1024 * 1024
LOCK = threading.Lock()
ANSWERS: "OrderedDict[str, dict]" = OrderedDict()
MOST = 48
MAKING: dict[str, str] = {}


def study_path(project: str) -> Path:
    return host.project_dir(project) / "population" / "study.json"


def study(project: str) -> dict:
    p = study_path(project)
    return json.loads(p.read_text(encoding="utf-8")) if p.is_file() else {}


def cases() -> list[dict]:
    out = []
    for c in pd.cases():
        path = pd.PRODUCTS / c["case"] / "field.json"
        if path.is_file():
            out.append({"case": c["case"], "title": c.get("title", c["case"]), "land_cells": c.get("land_cells"), "totals": c.get("totals", {}),
                        "grid": {k: c.get("grid", {}).get(k) for k in ("crs", "rows", "cols", "dx")}})
    return out


def field_of(case: str) -> tuple[dict, str]:
    if not re.fullmatch(r"[A-Za-z0-9_.-]+", case or ""):
        raise ValueError(f"not the name of a place: {case!r}")
    path = pd.PRODUCTS / case / "field.json"
    if not path.is_file():
        raise KeyError(f"the data library has made no place called {case}: python data/products/population_history.py {case}")
    stamp = f"{path.stat().st_mtime_ns}:{path.stat().st_size}"
    return pd.field(path), stamp


def asked(op: str, body: dict) -> dict:
    case = str(body.get("case") or "")
    f, stamp = field_of(case)
    request = {k: v for k, v in body.items() if k not in ("case", "op")}
    key = hashlib.sha256(json.dumps([op, case, stamp, request], sort_keys=True).encode()).hexdigest()
    with LOCK:
        if key in ANSWERS:
            ANSWERS.move_to_end(key)
            return ANSWERS[key]
    answer = pd.ask(op, field=f, **request)
    answer["case"] = case
    with LOCK:
        ANSWERS[key] = answer
        while len(ANSWERS) > MOST:
            ANSWERS.popitem(last=False)
    return answer


def make_place(body: dict) -> dict:
    lat, lon, half = float(body["lat"]), float(body["lon"]), float(body.get("half_km") or 30)
    name = re.sub(r"[^a-z0-9-]+", "-", str(body.get("name") or f"at-{lat:.3f}-{lon:.3f}").lower()).strip("-")[:40] or "a-place"
    if not (-80 <= lat <= 84 and -180 <= lon <= 180 and 5 <= half <= 60):
        raise ValueError("a place is a latitude within ±80°, a longitude, and a half-width of 5 to 60 km")
    with LOCK:
        if MAKING.get(name) == "making":
            raise ValueError(f"{name} is being made already")
        MAKING[name] = "making"
    try:
        r = subprocess.run([sys.executable, str(ROOT / "data" / "products" / "population_history.py"), "--at", f"{lat},{lon}", "--half", str(half), "--name", name],
                           capture_output=True, text=True, timeout=1800, cwd=str(ROOT))
        if r.returncode != 0:
            raise RuntimeError((r.stderr or r.stdout).strip().splitlines()[-1] if (r.stderr or r.stdout).strip() else "the data library could not make the place")
    finally:
        with LOCK:
            MAKING.pop(name, None)
    return {"case": name, "cases": cases()}


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The plug-in's routes; True when this answered."""
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] == "population"):
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
        handler.answer(lambda: (exists(), ({"study": study(project), "cases": cases()}, 200))[1])
        return True
    if method == "GET" and what == "field":
        case = str(query.get("case") or "")
        handler.answer(lambda: (exists(), (field_of(case)[0], 200))[1])
        return True
    if method == "POST" and what in ("growth", "learn", "modes", "simulate"):   # computed, and keeps nothing
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (asked(what, body), 200))[1])
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
                s = body.get("study")
                if not isinstance(s, dict):
                    raise ValueError("a study is a JSON object")
                p = study_path(project)
                p.parent.mkdir(parents=True, exist_ok=True)
                with host.FILES:
                    p.write_text(json.dumps(s, ensure_ascii=False, indent=1), encoding="utf-8")
                host.record(project, host.who(handler), "kept the study of population", s.get("case") or project, "")
                return {"kept": True}, 200
            handler.answer(put)
        return True
    if method == "POST" and what == "place":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (make_place(body), 200))[1])
        return True
    return False
