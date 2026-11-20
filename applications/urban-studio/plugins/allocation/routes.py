"""The server side of the plug-in Allocation and justice (ADR 0136), under /api/projects/<project>/allocation/.

    GET    allocation                the project's study (the case, the round's settings, the principles, the acts of its record) and the cases
    POST   allocation/compute        {case, amount, per_person, rule, stages?, acts?}
                                     the round run, judged against its alternatives under the case's principles, and its record replayed
    POST   allocation/budget         {case, satisfaction}        a participatory budget counted under several rules, by neighbourhood
    POST   allocation/propose        {case, situation}           a language model's stages, proposed (the work's own page at this computer)
    POST   allocation/articulate     {case, objection, ...}      a person's objection restated as an appeal, proposed (the same)
    PUT    allocation                {study}                     the study kept (the work's own page)

What is computed keeps nothing and any of the people may ask it; an answer is kept in memory while its request is the same. A model's
answer is a proposal of grade C, never applied: a person takes a proposed stage into the round, or files a restated appeal, or not.
"""

from __future__ import annotations

import hashlib
import json
import re
import sys
import threading
from collections import OrderedDict
from pathlib import Path

from science import models
from science import studio_host as host

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "tools"))
import _allocation as A  # noqa: E402
from science import allocation as al  # noqa: E402

TEXT_LIMIT = 4 * 1024 * 1024
LOCK = threading.Lock()
ANSWERS: "OrderedDict[str, dict]" = OrderedDict()
MOST = 16


def study_path(project: str) -> Path:
    return host.project_dir(project) / "allocation" / "study.json"


def study(project: str) -> dict:
    p = study_path(project)
    return json.loads(p.read_text(encoding="utf-8")) if p.is_file() else {}


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


def case_name(body: dict, default: str) -> str:
    name = str(body.get("case") or default)
    if not re.fullmatch(r"[A-Za-z0-9_-]+", name):
        raise ValueError(f"not the name of a case: {name!r}")
    return name


def round_of(body: dict) -> tuple[str, dict]:
    name = case_name(body, "england-iod2025")
    R = A.declared_round(name, body)
    if isinstance(body.get("stages"), list) and body["stages"]:
        R["stages"] = body["stages"]
    return name, R


def compute(body: dict) -> dict:
    name, R = round_of(body)
    acts = body.get("acts") if isinstance(body.get("acts"), list) else []
    key = hashlib.sha256(json.dumps({"r": R, "a": acts}, sort_keys=True).encode()).hexdigest()

    def make():
        reading = A.READINGS[name]
        out = {"case": name, "round": R, "result": al.run(R), "reading": reading}
        try:
            out["judgement"] = al.judge(R, A.PRINCIPLES[name], level=reading["level"], per=reading["per"])
        except al.EngineError as e:
            out["judgement"] = {"refused": str(e)}
        if acts:
            try:
                out["procedure"] = al.procedure(R, acts)
            except (al.EngineError, ValueError) as e:
                out["procedure"] = {"refused": str(e)}
        return out
    return kept(key, make)


def budget(body: dict) -> dict:
    name = case_name(body, "pabulib-Poland_Warszawa_2023")
    sat = "cardinality" if body.get("satisfaction") == "cardinality" else "cost"

    def make():
        C, B = A.budget_of(name, sat)
        return {"case": name, "title": C["title"], "currency": C.get("currency", ""), "budget": C["budget"], "voters": len(C["approvals"]),
                "projects": [{k: p[k] for k in ("name", "category", "cost", "votes", "selected")} for p in C["projects"]], "answer": B,
                "said": A.budget_said(C, B), "credit": C["credit"]}
    path = A.PRODUCTS / f"{name}.json"
    return kept(f"budget:{name}:{sat}:{path.stat().st_mtime_ns if path.is_file() else 0}", make)


def provider():
    s = {}
    p = models.settings_folder() / "urban-studio.json"
    if p.is_file():
        s = json.loads(p.read_text(encoding="utf-8"))
    return models.provider(s.get("provider", "anthropic"), **({"model": s["model"]} if s.get("model") else {}), **({"effort": s["effort"]} if s.get("effort") else {}))


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The plug-in's routes; True when this answered."""
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] == "allocation"):
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
        handler.answer(lambda: (exists(), ({"study": study(project), "cases": A.cases(), "model": models.key_state("anthropic")}, 200))[1])
        return True
    if method == "POST" and what in ("compute", "budget"):   # computed, and keeps nothing
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), ((compute if what == "compute" else budget)(body), 200))[1])
        return True
    refusal = host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    by = host.who(handler)
    if method == "POST" and what in ("propose", "articulate"):
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def ask():
            exists()
            if not host.at_this_computer(handler):
                raise PermissionError("a language model is asked only from the work's own page at this computer")
            _, R = round_of(body)
            try:
                if what == "propose":
                    p = al.propose_stages(str(body.get("situation") or ""), R, provider=provider())
                    host.record(project, by, "asked a language model to propose stages", "", f"{len(p['stages'])} proposed, {len(p['refused'])} refused")
                else:
                    p = al.articulate_appeal(str(body.get("objection") or ""), R, provider=provider())
                    host.record(project, by, "asked a language model to restate an objection as an appeal", p["appeal"].get("entity", ""))
            except models.Refusal as e:
                return {"refused": str(e)}, 200
            return p, 200
        handler.answer(ask)
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
                host.replace_file(p, json.dumps(st, ensure_ascii=False, indent=1))   # it takes the host's lock on files itself
                host.record(project, by, "kept the allocation study", st.get("case") or project, f"{len(st.get('acts') or [])} acts on record")
                host.emit(project, {"what": "allocation kept", "by": by, "about": st.get("case") or ""})
                return {"kept": True}, 200
            handler.answer(put)
        return True
    return False
