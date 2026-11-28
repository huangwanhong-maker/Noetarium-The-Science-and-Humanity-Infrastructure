"""The working agents of a society that writes a report (the design record of 2026-11-26, a book written by communities, Section 3).

Each community keeps its own stage of every section, by the same cycle of acts as any work in Koinonia (ADR 0169): its keeper posts a
request, agents offer, the keeper takes an offer, the agent delivers a body, the keeper accepts it by the community's standard or
releases it with the reason. Two deliveries of different origins are needed where a stage is checked (a choice of series and its check
in the commons; a plan of analyses and its replication in the guild); one where it is reviewed later (a version in the cooperative).

- ``gatherer`` (the commons): chooses a section's series from the World Bank's catalogue, or checks another's choice;
- ``analyst`` (the guild): plans the tools run on a section's series and runs them, or replicates and judges another's plan; builds a
  tool the council admitted;
- ``writer`` (the cooperative): drafts a section from its results, or revises it after its reviews or an audit;
- ``reviewer`` (the partnership): scores a version assigned to it on the declared scorecard and votes on its adoption.

A model agent asks its model for the judgement; a rule agent, and every agent of a dry run, follows a declared procedure. Any working
agent may ask, with its delivery, for a tool the bench lacks (a petition), which its community's envoy may carry to the council.
"""

from __future__ import annotations

import json
import re

from . import bench
from . import book as bk
from .society import Society, item_of

WORK_KINDS = ["join", "offer", "cancel", "deliver", "hand over", "report", "say", "claim", "leave"]
KINDS = {
    "gatherer": WORK_KINDS, "analyst": WORK_KINDS, "writer": WORK_KINDS,
    "reviewer": ["join", "assess", "vote", "say", "report"],
    "secretary": ["request", "take", "accept", "release", "attest", "report", "propose", "amend", "say"],
    "editor": ["join", "report", "say", "assess", "vote"],
    "auditor": ["attest", "report", "say"],
    "book keeper": ["take", "accept", "release", "request", "answer", "say"],
}
MATTERS = {"commons": "series", "guild": "analysis", "cooperative": "writing"}
OPEN_AT_ONCE = 40
MOST_VERSIONS = 5

PETITION = {"type": "object", "additionalProperties": False, "required": ["kind", "name", "need"],
            "properties": {"kind": {"type": "string", "enum": ["tool", "data"]}, "name": {"type": "string"}, "need": {"type": "string"}}}
PETITION_RULE = ("A petition is of one of two kinds: a tool, a computation over series the shelf holds that no tool of the bench makes (built by "
                 "the guild if the council admits it); or data, a series the World Bank's catalogue does not hold (recorded as a gap of the "
                 "shelf, which no tool can fill). Petition only when the section truly needs it.")
HOUSE = ("The book is a global analysis of the Sustainable Development Goals with proposals, written by five communities of invented agents; "
         "it is no institution's report. Every number comes from the World Bank's World Development Indicators through the bench's tools. "
         "House style: British spelling; declarative headings, never questions; no first person; plain, exact prose; a finding states its "
         "number, its year and its unit; where data are missing or a target can be read in more than one way, say so.")


def ask(s: Society, agent: dict, what: str, system: str, payload: dict, schema: dict, max_tokens: int) -> dict | None:
    if s.asker is None:
        return None
    return s.asker.ask(s, agent, what, system, json.dumps(payload, ensure_ascii=False, default=str), schema, max_tokens)


def dry(s: Society, agent: dict) -> bool:
    return agent["policy"] == "rule" or getattr(s, "dry", False)


def act(agent: dict, community: str, kind: str, s: Society, fields: dict | None = None, **extra) -> tuple[str, dict]:
    a = {"author": agent["id"], "kind": kind, "turn": s.turn, "mandate": agent["communities"][community]["mandate"], "fields": fields or {}}
    a.update(extra)
    return community, a


def community_of(s: Society, agent: dict, preset: str) -> str | None:
    return next((c for c in agent["communities"] if s.communities[c].preset == preset), None)


def section_of(item: str) -> str:
    """The section an item names: d-<sid> and a-<sid>, with -r<k> for a later round of choosing or of analysis; w-<sid>-<n>."""
    body = item.split("-", 1)[1] if "-" in item else item
    if item.startswith("w-"):
        return body.rsplit("-", 1)[0]
    return re.sub(r"-r\d+$", "", body)


def data_item(x: dict) -> str:
    return f"d-{x['id']}" + (f"-r{x['data round']}" if x.get("data round") else "")


def analysis_item(x: dict) -> str:
    return f"a-{x['id']}" + (f"-r{x['analysis round']}" if x.get("analysis round") else "")


def engaged(c, item: str, statuses=("active", "presented", "discharged", "conditional")) -> list[dict]:
    return [x for x in c.state.get("commitments", []) if item_of(x["id"]) == item and x["status"] in statuses]


def delivered(s: Society, c, item: str) -> list[dict]:
    """The bodies accepted for an item, in order."""
    out = []
    for x in c.state.get("commitments", []):
        if item_of(x["id"]) == item and x["status"] == "discharged" and x["id"] in s.work.bodies:
            out.append(s.work.bodies[x["id"]])
    out.sort(key=lambda b: b.get("turn", 0))
    return out


def needed(s: Society, c, item: str) -> int:
    """How many deliveries an item still needs: one for writing; two for a checked stage (a choice and its check, a plan and its
    replication, a tool and its check)."""
    want = 1 if item.startswith("w-") else 2
    return max(0, want - len(delivered(s, c, item)))


# ---- keepers --------------------------------------------------------------------------------------------------------------------------
def book_keeper(s: Society, agent: dict) -> list[tuple[str, dict]]:
    """A working community's keeper: posts its stage's items, takes offers one at a time (a second of another origin than the first),
    accepts a well-formed delivery or releases it with the reason."""
    out = []
    for name in agent["communities"]:
        c = s.communities[name]
        matter = MATTERS.get(c.preset)
        if matter is None:
            continue
        steward = c.roles.get("steward")
        posted = {r["fields"]["id"] for r in c.record if r["kind"] == "request"}
        open_items = [i for i in posted if needed(s, c, i) and not s.work.bodies.get(f"closed:{i}")]
        for item in wanted_items(s, c)[:max(0, OPEN_AT_ONCE - len(open_items))]:
            if item not in posted:
                out.append(act(agent, name, "request", s, {"id": item, "matter": matter if not item.startswith("t-") else "tool"}))
        for x in c.state.get("commitments", []):
            if x["status"] != "conditional" or x["creditor"] != steward:
                continue
            item = item_of(x["id"])
            if s.work.bodies.get(f"closed:{item}") or not needed(s, c, item) or engaged(c, item, ("active", "presented")):
                continue
            origins = {b.get("origin") for b in delivered(s, c, item)}
            if s.origin(x["debtor"]) in origins:
                continue
            if any(a[1]["kind"] == "take" and item_of(a[1]["fields"]["id"]) == item for a in out):
                continue
            out.append(act(agent, name, "take", s, {"id": x["id"]}))
        for x in [x for x in c.state.get("commitments", []) if x["status"] == "presented" and x["creditor"] == steward][:30]:
            body = s.work.bodies.get(x["id"], {})
            why = delivery_faults(s, c, item_of(x["id"]), body)
            out.append(act(agent, name, "release" if why else "accept", s, {"id": x["id"]}, _by=why))
    return out


def wanted_items(s: Society, c) -> list[str]:
    b = s.book
    if c.preset == "commons":
        return [data_item(x) for x in b.at("data")]
    if c.preset == "guild":
        items = [analysis_item(x) for x in b.at("analysis")]
        return items + [f"t-{p['name']}" for p in b.petitions.values() if p["status"] == "to build"]
    if c.preset == "cooperative":
        return [f"w-{x['id']}-{len(b.versions_of(x['id'])) + 1}" for x in b.sections.values() if x["stage"] in ("draft", "revision", "corrections") and x.get("writer") != "editor"]
    return []


def delivery_faults(s: Society, c, item: str, body: dict) -> str:
    b, cat = s.book, bench.catalogue()
    if item.startswith("d-"):
        first = delivered(s, c, item)
        if body.get("kind") == "choice":
            return bk.choice_faults(body, cat) if not first else "the item has its choice; it needs a check"
        return bk.check_faults(body, first[0], cat) if first else "a check needs a choice before it"
    if item.startswith("a-"):
        if body.get("kind") == "plan":
            if delivered(s, c, item):
                return "the item has its plan; it needs a replication"
            why = bk.plan_faults(body, set(bench.TOOLS) | set(b.tools))
            if why:
                return why
            kept = [r for r in body.get("results", []) if "refused" not in r]
            return "" if kept else "no step of the plan gave a result"
        if body.get("kind") == "replication":
            return "" if delivered(s, c, item) and isinstance(body.get("judgements"), list) else "a replication needs a plan and judges each step"
        return "a delivery of the guild is a plan or a replication"
    if item.startswith("t-"):
        if body.get("kind") == "tool":
            why = bench.Restricted.faults(str(body.get("code", "")))
            return ("the code is not admissible: " + "; ".join(why)) if why else ("" if not delivered(s, c, item) else "the tool is written; it needs a check")
        if body.get("kind") == "tool check":
            return "" if delivered(s, c, item) and body.get("verdict") in ("sound", "unsound") else "a check of a tool says sound or unsound"
        return "a delivery for a tool is the tool or its check"
    if item.startswith("w-"):
        sid = section_of(item)
        sec = b.section(sid)
        return bk.version_faults(body, b.facts_of(sid), sec["words"]) if sec else "no such section"
    return "an item the keeper does not know"


# ---- choosing work --------------------------------------------------------------------------------------------------------------
def open_work(s: Society, agent: dict, name: str) -> list[str]:
    """The items posted in the community that still need a delivery the agent may give (none of its origin delivered or engaged)."""
    c = s.communities[name]
    out = []
    for r in c.record:
        if r["kind"] != "request":
            continue
        item = r["fields"]["id"]
        if s.work.bodies.get(f"closed:{item}") or not needed(s, c, item) or engaged(c, item, ("active", "presented")):
            continue
        if any(b.get("origin") == agent["origin"] for b in delivered(s, c, item)):
            continue
        if any(s.origin(x["debtor"]) == agent["origin"] for x in engaged(c, item, ("conditional",))):
            continue
        if item.startswith("t-") and agent["role"] != "analyst":
            continue
        out.append(item)
    return out


def worker(s: Society, agent: dict) -> list[tuple[str, dict]]:
    """Any working agent: deliver what it was given; withdraw offers no longer wanted; offer for open work (one at a time)."""
    out = []
    for name in agent["communities"]:
        c = s.communities[name]
        if MATTERS.get(c.preset) is None:
            continue
        mine = [x for x in c.state.get("commitments", []) if x["by"] == agent["id"]]
        for x in [x for x in mine if x["status"] == "active" and not s.work.bodies.get(f"closed:{item_of(x['id'])}")][:1]:
            out += deliver(s, agent, name, x)
        for x in [x for x in mine if x["status"] == "conditional"]:
            item = item_of(x["id"])
            if s.work.bodies.get(f"closed:{item}") or not needed(s, c, item) or engaged(c, item, ("active", "presented")):
                out.append(act(agent, name, "cancel", s, {"id": x["id"]}))
        busy = any(x["status"] in ("active", "conditional", "presented") for x in mine)
        if busy:
            continue
        items = open_work(s, agent, name)
        if items:
            k = int(bk.key(agent["id"] + str(s.turn))[:6], 16) % len(items)
            item = items[k]
            out.append(act(agent, name, "offer", s, {"id": f"{item}~{agent['id']}~{s.turn}", "to": c.roles.get("steward"), "matter": MATTERS[c.preset]}))
    return out


def deliver(s: Society, agent: dict, name: str, x: dict) -> list[tuple[str, dict]]:
    c = s.communities[name]
    item = item_of(x["id"])
    first = delivered(s, c, item)
    if item.startswith("d-"):
        body = check_series(s, agent, item, first[0]) if first else choose_series(s, agent, item)
    elif item.startswith("a-"):
        body = replicate(s, agent, item, first[0]) if first else plan(s, agent, item)
    elif item.startswith("t-"):
        body = check_tool(s, agent, item, first[0]) if first else build_tool(s, agent, item)
    elif item.startswith("w-"):
        body = write(s, agent, item)
    else:
        body = None
    if body is None:
        return []
    body.update({"by": agent["id"], "origin": agent["origin"], "model": agent.get("model") if not dry(s, agent) else "rule", "turn": s.turn})
    return [act(agent, name, "deliver", s, {"id": x["id"], "matter": MATTERS[c.preset], "artefact": bk.digest(body)}, _body=body)]


# ---- the commons: series -----------------------------------------------------------------------------------------------------------
GATHER_SYSTEM = (HOUSE + " You are a gatherer lent to the commons of data. For the section below choose the series of the World Development "
                 "Indicators that best answer its question: one to five, each with a reason of at most 25 words (what it measures, why it fits "
                 "the targets, its known limits). Prefer series that measure a target's own indicator; prefer wide coverage. Choose only codes "
                 "from the candidates. " + PETITION_RULE)
GATHER_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["series", "note"],
                 "properties": {"series": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["code", "why"],
                                                                     "properties": {"code": {"type": "string"}, "why": {"type": "string"}}}},
                                "note": {"type": "string"}, "petition": PETITION}}
CHECK_SYSTEM = (HOUSE + " You are a gatherer of another origin, checking another gatherer's choice of series for the section below. Keep or drop "
                "each series (a reason of at most 20 words each), and add at most two candidates it missed that the section needs.")
CHECK_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["verdicts", "add"],
                "properties": {"verdicts": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["code", "verdict", "why"],
                                                                      "properties": {"code": {"type": "string"}, "verdict": {"type": "string", "enum": ["keep", "drop"]},
                                                                                     "why": {"type": "string"}}}},
                               "add": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["code", "why"],
                                                                  "properties": {"code": {"type": "string"}, "why": {"type": "string"}}}},
                               "petition": PETITION}}


def section_brief(s: Society, sid: str) -> dict:
    sec = s.book.section(sid)
    ch = s.book.chapter(sec["chapter"]) or {}
    targets = [{"target": t, "text": bench.target_text(t)[:300]} for t in sec.get("targets", [])]
    return {"book": s.book.outline.get("title", ""), "chapter": ch.get("title", ""), "chapter kind": ch.get("kind", ""), "section": sec["title"],
            "question": sec["question"], "targets": targets}


def candidates_for(s: Society, sid: str) -> list[dict]:
    sec = s.book.section(sid)
    words = sec["question"].split() + sec["title"].split()
    for g in bench.framework():
        for t in g["targets"]:
            if t["target"] in sec.get("targets", []):
                for i in t["indicators"]:
                    words += i["text"].split()
    stop = {"the", "and", "for", "with", "that", "from", "this", "which", "their", "by", "of", "all", "per", "proportion", "number", "total", "rate"}
    return bench.candidates([w.strip(",.()").lower() for w in words if w.lower() not in stop], limit=90)


def choose_series(s: Society, agent: dict, item: str) -> dict | None:
    sid = section_of(item)
    cands = candidates_for(s, sid)
    if dry(s, agent):
        chosen = [{"code": c["code"], "why": "the candidate that most matches the targets' indicators"} for c in cands[:3]]
        return {"kind": "choice", "series": chosen, "note": ""}
    a = ask(s, agent, f"choose series for {sid}", GATHER_SYSTEM, {"section": section_brief(s, sid), "candidates": cands}, GATHER_SCHEMA, 2500)
    if a is None:
        return None
    return {"kind": "choice", "series": a.get("series", [])[:6], "note": str(a.get("note", ""))[:300], "petition": a.get("petition")}


def check_series(s: Society, agent: dict, item: str, first: dict) -> dict | None:
    sid = section_of(item)
    cands = candidates_for(s, sid)
    shown = [dict(bench.described(c["code"]), why=c.get("why", ""), coverage=bench.coverage_of(c["code"])["countries"]) for c in first["series"]
             if c["code"] in bench.catalogue()]
    if dry(s, agent):
        verdicts = [{"code": c["code"], "verdict": "keep" if bench.coverage_of(c["code"])["countries"] >= 20 else "drop", "why": "coverage of countries"} for c in first["series"]]
        return {"kind": "check", "verdicts": verdicts, "add": []}
    a = ask(s, agent, f"check series for {sid}", CHECK_SYSTEM, {"section": section_brief(s, sid), "chosen": shown, "candidates": cands[:60]}, CHECK_SCHEMA, 2500)
    if a is None:
        return None
    return {"kind": "check", "verdicts": a.get("verdicts", []), "add": a.get("add", [])[:2], "petition": a.get("petition")}


# ---- the guild: analyses --------------------------------------------------------------------------------------------------------------
PLAN_SYSTEM = (HOUSE + " You are an analyst of the guild. Plan the analyses the section's writer will need for a substantive section: four to "
               "eight steps, each a tool of the bench with its parameters, using only the section's accepted series; analyse each main series "
               "for the regions and for the countries, and read the distribution, the convergence, the breaks and the links between series "
               "where they answer the question. Use trend with the target the targets' text states "
               "(give its reading in target_reading; null when the target names no number) and desired direction; levels for the regional "
               "picture; spread, convergence, breaks and links where they answer the question. " + PETITION_RULE)
# A step's parameters: those of the bench's tools, each optional, and "other" (JSON text) for parameters an admitted tool declares; structured
# output closes every object, so the parameters are listed rather than free.
PARAMS = {"type": "object", "additionalProperties": False,
          "properties": {"series": {"type": "string"}, "other": {"type": "string"}, "desired": {"type": "string", "enum": ["increase", "decrease"]},
                         "target": {"type": ["number", "null"]}, "target_reading": {"type": "string"}, "units": {"type": "string", "enum": ["aggregates", "countries"]},
                         "years": {"type": "array", "items": {"type": "integer"}}, "from": {"type": "integer"}, "to": {"type": "integer"},
                         "log": {"type": "boolean"}, "unit": {"type": "string"}, "more": {"type": "string"}}}
PLAN_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["plan", "note"],
               "properties": {"plan": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["tool", "params", "why"],
                                                                  "properties": {"tool": {"type": "string"}, "params": PARAMS, "why": {"type": "string"}}}},
                              "note": {"type": "string"}, "petition": PETITION}}


def params_of(step: dict) -> dict:
    """A step's parameters as a tool takes them: the listed ones, and those given as JSON text in "more"."""
    raw = dict(step.get("params") or {})
    more = raw.pop("more", "")
    if more:
        try:
            extra = json.loads(more)
            if isinstance(extra, dict):
                raw.update({k: v for k, v in extra.items() if k not in raw})
        except ValueError:
            pass
    return raw
REPLICATE_SYSTEM = (HOUSE + " You are an analyst of the guild of another origin. The plan below was run; the work ran it again for you and "
                    "reports whether each step's result is the same. Judge each step: sound (it answers the question with the right series, "
                    "target and direction) or unsound, with a reason of at most 25 words; you may add at most two steps the writer will need.")
REPLICATE_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["judgements", "add"],
                    "properties": {"judgements": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["step", "verdict", "why"],
                                                                             "properties": {"step": {"type": "integer"}, "verdict": {"type": "string", "enum": ["sound", "unsound"]},
                                                                                            "why": {"type": "string"}}}},
                                   "add": PLAN_SCHEMA["properties"]["plan"], "petition": PETITION}}


def bench_menu(s: Society) -> dict:
    menu = {k: v for k, v in bench.TOOLS.items()}
    for n, t in s.book.tools.items():
        menu[n] = {"about": t["about"], "params": t["params"], "admitted": f"admitted by the council (round {t.get('admitted')})"}
    return menu


def series_shown(s: Society, sid: str) -> list[dict]:
    return [dict(bench.described(c), coverage=bench.coverage_of(c)["countries"], latest=bench.coverage_of(c)["latest year"]) for c in s.book.section(sid).get("series", [])]


def run_plan(s: Society, steps: list[dict], allowed: list[str]) -> list[dict]:
    out = []
    for p in steps[:8]:
        params = {k: v for k, v in params_of(p).items() if v not in ("", None)}
        tool = s.book.tools.get(str(p.get("tool")))
        named = (tool.get("series params") if tool else None) or ["series", "other"]
        if any(params.get(k) not in allowed for k in named if k in params):
            out.append({"refused": "the step uses a series the commons did not accept for the section", "step": p})
            continue
        r = bench.run(str(p.get("tool")), params, s.book.tools)
        r["why"] = str(p.get("why", ""))[:200]
        if "refused" not in r:
            r["digest"] = bench.digest(r)
        out.append(r)
    return out


def rule_plan(s: Society, sid: str) -> list[dict]:
    sec = s.book.section(sid)
    steps = []
    for c in sec.get("series", [])[:2]:
        steps.append({"tool": "levels", "params": {"series": c}, "why": "the regional picture"})
        steps.append({"tool": "trend", "params": {"series": c, "desired": "increase", "target": None, "units": "aggregates"}, "why": "the trend by its rate"})
    if sec.get("series"):
        steps.append({"tool": "spread", "params": {"series": sec["series"][0]}, "why": "the spread among countries"})
    return steps


def plan(s: Society, agent: dict, item: str) -> dict | None:
    sid = section_of(item)
    allowed = s.book.section(sid).get("series", [])
    if dry(s, agent):
        steps, note, petition = rule_plan(s, sid), "", None
    else:
        a = ask(s, agent, f"plan analyses for {sid}", PLAN_SYSTEM, {"section": section_brief(s, sid), "series": series_shown(s, sid), "bench": bench_menu(s)}, PLAN_SCHEMA, 3000)
        if a is None:
            return None
        steps, note, petition = a.get("plan", [])[:6], str(a.get("note", ""))[:300], a.get("petition")
    return {"kind": "plan", "plan": steps, "results": run_plan(s, steps, allowed), "note": note, "petition": petition}


def replicate(s: Society, agent: dict, item: str, first: dict) -> dict | None:
    sid = section_of(item)
    allowed = s.book.section(sid).get("series", [])
    again = run_plan(s, first["plan"], allowed)
    same = [("refused" not in a and "refused" not in b and a.get("digest") == b.get("digest")) for a, b in zip(first["results"], again)]
    if dry(s, agent):
        return {"kind": "replication", "same": same, "judgements": [{"step": i, "verdict": "sound", "why": "replicated"} for i in range(len(first["plan"]))], "add": [], "added": []}
    shown = [{"step": i, "tool": p.get("tool"), "params": p.get("params"), "why": p.get("why"), "same when run again": same[i] if i < len(same) else False,
              "summary": (first["results"][i].get("summary") or first["results"][i].get("refused", ""))[:700] if i < len(first["results"]) else ""}
             for i, p in enumerate(first["plan"])]
    a = ask(s, agent, f"replicate analyses for {sid}", REPLICATE_SYSTEM, {"section": section_brief(s, sid), "series": series_shown(s, sid), "plan": shown,
                                                                           "bench": bench_menu(s)}, REPLICATE_SCHEMA, 3000)
    if a is None:
        return None
    add = a.get("add", [])[:2]
    return {"kind": "replication", "same": same, "judgements": a.get("judgements", []), "add": add, "added": run_plan(s, add, allowed), "petition": a.get("petition")}


# ---- the guild: tools the council admitted ------------------------------------------------------------------------------------------
BUILD_SYSTEM = (HOUSE + " You are an analyst of the guild asked to build a tool the partnership's council admitted. Write it in the restricted "
                "language: one function def tool(data, params) in plain Python using only arithmetic, comparisons, loops, comprehensions, "
                "lists, dicts, strings and the modules math and statistics (import math / import statistics, nothing else); no names or "
                "attributes starting with an underscore; no I/O. data[code] is {unit: {year: value}} for each series its parameters name "
                "(the parameters listed in series_params); data['regions'] maps each country's code to its region's code (aggregates are "
                "absent from it); data['names'] maps codes to names. Return {'facts': {name: number or short text}, 'summary': text}. Give "
                "two cases: parameters to run it with on real series and what a checker should see.")
BUILD_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["code", "about", "params", "series_params", "cases"],
                "properties": {"code": {"type": "string"}, "about": {"type": "string"},
                               "params": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["name", "meaning"],
                                                                     "properties": {"name": {"type": "string"}, "meaning": {"type": "string"}}}},
                               "series_params": {"type": "array", "items": {"type": "string"}},
                               "cases": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["params_json", "expect"],
                                                                    "properties": {"params_json": {"type": "string"}, "expect": {"type": "string"}}}}}}


def case_params(c: dict) -> dict:
    try:
        v = json.loads(c.get("params_json") or "{}")
        return v if isinstance(v, dict) else {}
    except ValueError:
        return {}
TOOLCHECK_SYSTEM = (HOUSE + " You are an analyst of the guild of another origin, checking a tool another built before the bench admits it. "
                    "The work ran it on its declared cases; read the code and the outputs and say whether it computes what it claims (sound) "
                    "or not (unsound), with a reason of at most 40 words.")
TOOLCHECK_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["verdict", "why"],
                    "properties": {"verdict": {"type": "string", "enum": ["sound", "unsound"]}, "why": {"type": "string"}}}


def build_tool(s: Society, agent: dict, item: str) -> dict | None:
    name = item[2:]
    p = next((p for p in s.book.petitions.values() if p["name"] == name), None)
    if p is None or dry(s, agent):
        return None
    a = ask(s, agent, f"build tool {name}", BUILD_SYSTEM, {"tool": name, "need": p["need"], "shelf examples": ["SH.DYN.MORT", "SI.POV.DDAY", "SP.POP.TOTL"]}, BUILD_SCHEMA, 5000)
    if a is None:
        return None
    tool = {"name": name, "code": str(a.get("code", "")), "about": str(a.get("about", ""))[:400],
            "params": {str(x.get("name")): str(x.get("meaning", ""))[:200] for x in a.get("params") or [] if isinstance(x, dict)},
            "series params": [str(x) for x in a.get("series_params") or ["series"]][:4]}
    runs = [{"params": case_params(cse), "expect": cse.get("expect"), "output": bench.Restricted.run(tool, case_params(cse))} for cse in (a.get("cases") or [])[:2]]
    return {"kind": "tool", **tool, "cases": runs}


def check_tool(s: Society, agent: dict, item: str, first: dict) -> dict | None:
    if dry(s, agent):
        return None
    runs = [{"params": r["params"], "expect": r["expect"], "output": bench.Restricted.run(first, r["params"] or {})} for r in first.get("cases", [])]
    a = ask(s, agent, f"check tool {first.get('name')}", TOOLCHECK_SYSTEM, {"tool": {k: first.get(k) for k in ("name", "about", "params", "code")}, "runs": runs},
            TOOLCHECK_SCHEMA, 1500)
    if a is None:
        return None
    return {"kind": "tool check", "verdict": a.get("verdict"), "why": str(a.get("why", ""))[:400], "runs": runs}


# ---- the cooperative: versions ----------------------------------------------------------------------------------------------------
WRITE_SYSTEM = (HOUSE + " You are a writer of the cooperative. Write the section below for the book, a substantive piece of analysis an informed "
                "reader would learn from: continuous prose in paragraphs separated by blank lines, with three to six subheadings written as lines "
                "beginning '### '. Its length is the words asked: write at least that many words, and no section shorter than four fifths of "
                "it will be accepted. Interpret the results: what they show, why it matters, how regions and countries differ, what changed "
                "after 2015 and after 2020, what the distribution and the trends imply for 2030; you may explain with well-established knowledge "
                "of causes, policies and history, stated without numbers. Every number you write must be one of the facts, rounded as a reader "
                "needs it (shares and rates to one decimal place, ratios to two) and never to more digits than the fact has; list each in "
                "claims with the fact's exact key and the value as you wrote it. Name figures to show by the result ids that have figures. End "
                "with proposals that follow from your findings (each one or two sentences, saying who should act) and say in gaps what the data "
                "do not cover. Do not repeat the other sections of the chapter. " + PETITION_RULE)
REVISE_SYSTEM = (WRITE_SYSTEM + " This is a revision: the previous version, its reviews and any audit findings follow. Answer every point the "
                 "reviewers raised and correct every number the audit found wrong; keep what they found sound, and keep the section's length.")
CORRECT_SYSTEM = (WRITE_SYSTEM + " The section was adopted subject to minor corrections: the adopted version and the reviewers' comments follow. "
                  "Make the corrections the comments ask for and nothing else; keep the text, its length and its claims otherwise as they are.")
WRITE_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["text", "claims", "figures", "proposals", "gaps"],
                "properties": {"text": {"type": "string"},
                               "claims": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["fact", "value"],
                                                                     "properties": {"fact": {"type": "string"}, "value": {"type": "string"}}}},
                               "figures": {"type": "array", "items": {"type": "string"}}, "proposals": {"type": "array", "items": {"type": "string"}},
                               "gaps": {"type": "string"}, "petition": PETITION}}


def writing_brief(s: Society, sid: str) -> dict:
    sec = s.book.section(sid)
    ch = s.book.chapter(sec["chapter"]) or {}
    siblings = [s.book.section(x)["title"] for x in ch.get("sections", []) if x != sid and s.book.section(x)]
    results = [{"id": r["id"], "tool": r["tool"], "summary": r.get("summary", "")[:1200], "has figure": bool(r.get("figure"))} for r in s.book.results_of(sid)]
    return {**section_brief(s, sid), "words asked": sec["words"], "other sections of the chapter": siblings, "series": series_shown(s, sid),
            "results": results, "facts": s.book.facts_of(sid)}


def rule_text(s: Society, sid: str) -> dict:
    sec, facts = s.book.section(sid), s.book.facts_of(sid)
    paras, claims = [f"### {sec['title']}", sec["question"]], []
    for k, v in list(facts.items())[:40]:
        paras.append(f"The value of {k.split(': ', 1)[1]} is {v}.")
        claims.append({"fact": k, "value": str(v)})
    while len(" ".join(paras).split()) < sec["words"] + 10:
        paras.append("The data reported here are those the commons accepted and the guild replicated; further years and countries are not covered.")
    figs = [r["id"] for r in s.book.results_of(sid) if r.get("figure")][:2]
    return {"text": "\n\n".join(paras), "claims": claims, "figures": figs, "proposals": ["Keep the series current."], "gaps": "Coverage is partial."}


PARTS_SYSTEM = (HOUSE + " You are a writer of the cooperative, planning the section below before writing it. Divide it into two to six parts of about 400 to 900 words, "
                "each a subheading of the section with what it will say, the results it rests on (by their ids) and its words; the parts' words "
                "sum to the words asked. Order them as an argument: the global picture, how regions and countries differ, what changed and why, "
                "what it implies for 2030; the last part ends with the proposals.")
PARTS_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["parts"],
                "properties": {"parts": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["title", "focus", "results", "words"],
                                                                    "properties": {"title": {"type": "string"}, "focus": {"type": "string"},
                                                                                   "results": {"type": "array", "items": {"type": "string"}}, "words": {"type": "integer"}}}}}}
PART_SYSTEM = (HOUSE + " You are a writer of the cooperative, writing one part of a section of the book: the part named, about the words given "
               "(write at least that many), continuous prose in paragraphs separated by blank lines, without its subheading (it is added). "
               "Interpret the results: what they show, why it matters, how regions and countries differ, what changed after 2015 and after "
               "2020, what it implies for 2030; you may explain with well-established knowledge of causes, policies and history, stated "
               "without numbers. Every number you write must be one of the facts given, rounded as a reader needs it (shares and rates to one "
               "decimal place, ratios to two) and never to more digits than the fact has; list each in claims with the fact's exact key and the "
               "value as you wrote it. Never compute a number the results do not give (a difference, a ratio, a sum, a count of countries): cite only "
               "facts. Read each tool as its summary says how; claim nothing a result does not show (what drives a change, who holds most of a "
               "total), and give causes only as context, stated as such. Do not repeat what the parts already written say. Name figures to show "
               "by result ids that have figures. "
               "Give proposals and gaps only in the last part: proposals that follow from the section's findings (each one or two sentences, "
               "saying who should act), and what the data do not cover. " + PETITION_RULE)
PART_REVISE = (" This is a revision of the part: its previous text, the reviewers' comments on the whole section and any numbers the audit "
               "found wrong follow. Answer every point that bears on this part and correct every wrong number; keep what is sound, and keep "
               "its length.")
PART_CORRECT = (" The section was adopted subject to minor corrections: the part's adopted text and the reviewers' comments follow. Make the "
                "corrections that bear on this part and nothing else; keep the text, its length and its claims otherwise as they are.")


def split_parts(text: str) -> list[tuple[str, str]]:
    """A version's text cut at its '### ' subheadings: (title, text) for each part."""
    parts, title, buf = [], "", []
    for line in str(text).splitlines():
        if line.startswith("### "):
            if title or buf:
                parts.append((title, "\n".join(buf).strip()))
            title, buf = line[4:].strip(), []
        else:
            buf.append(line)
    if title or buf:
        parts.append((title, "\n".join(buf).strip()))
    return [p for p in parts if p[1]]


def part_payload(s: Society, sid: str, results: list[str]) -> dict:
    """What a writer is shown for one part: the section's brief, the series, and only the results (and their facts) the part rests on."""
    sec = s.book.section(sid)
    ch = s.book.chapter(sec["chapter"]) or {}
    all_results = s.book.results_of(sid)
    chosen = [r for r in all_results if r["id"] in results] or all_results
    facts = {f"{r['id']}: {k}": v for r in chosen for k, v in (r.get("facts") or {}).items()}
    series = [{k: x.get(k) for k in ("code", "name", "coverage", "latest")} | {"note": str(x.get("note", ""))[:150]} for x in series_shown(s, sid)]
    return {**section_brief(s, sid), "other sections of the chapter": [s.book.section(x)["title"] for x in ch.get("sections", []) if x != sid and s.book.section(x)],
            "series": series, "results": [{"id": r["id"], "tool": r["tool"], "summary": r.get("summary", "")[:700], "has figure": bool(r.get("figure"))} for r in chosen],
            "facts": facts}


def write(s: Society, agent: dict, item: str) -> dict | None:
    """A version of a section, written part by part: a draft is planned first (its parts, their words and the results each rests on); a
    revision or a correction takes the previous version's parts. Each part is one call, so that a long section is written at its length."""
    sid = section_of(item)
    sec = s.book.section(sid)
    n = int(item.rsplit("-", 1)[1])
    prev = s.book.latest(sid)
    if dry(s, agent):
        a = rule_text(s, sid)
        return {"kind": "version", "section": sid, "n": n, "parent": prev["id"] if prev else None, "text": str(a.get("text", "")), "claims": a.get("claims", []),
                "figures": [f for f in a.get("figures", []) if f in s.book.results][:4], "proposals": a.get("proposals", []), "gaps": a.get("gaps", ""), "petition": None}
    words = sec["words"]
    mode = "draft" if not (prev and n > 1) else "correct" if sec["stage"] == "corrections" else "revise"
    ids = [r["id"] for r in s.book.results_of(sid)]
    if mode == "draft":
        plan = ask(s, agent, f"plan {sid} v{n}", PARTS_SYSTEM, {**section_brief(s, sid), "words asked": words,
                                                             "results": [{"id": r["id"], "tool": r["tool"], "summary": r.get("summary", "")[:500]} for r in s.book.results_of(sid)]},
                   PARTS_SCHEMA, 2500)
        if plan is None:
            return None
        parts = [{"title": str(p.get("title", "")).strip() or f"Part {i + 1}", "focus": str(p.get("focus", "")), "results": [r for r in p.get("results", []) if r in ids],
                  "words": max(250, int(p.get("words") or 0)), "previous": ""} for i, p in enumerate(plan.get("parts", [])[:6])]
        if not parts:
            return None
        scale = words / max(1, sum(p["words"] for p in parts))
        for p in parts:
            p["words"] = max(250, int(p["words"] * scale))
    else:
        old = split_parts(prev["text"])
        parts = [{"title": tt or f"Part {i + 1}", "focus": "", "results": ids, "words": max(250, int(len(tx.split()) * (1.0 if mode == "correct" else 1.05))), "previous": tx}
                 for i, (tt, tx) in enumerate(old)] or [{"title": sec["title"], "focus": "", "results": ids, "words": words, "previous": prev["text"]}]
        reviews = [{"by community": r.get("community"), "scores": r["scores"], "comments": r.get("comments", ""), "recommend": r["recommend"]} for r in s.book.reviews_of(prev["id"])]
        audit = [w for a_ in s.book.audits if a_["version"] == prev["id"] for w in a_["wrong"]]
    texts, claims, figures, proposals, gaps, petition = [], [], [], [], "", None
    if mode in ("correct", "revise"):  # a revision or a correction rewrites only the parts its faults point to; the others are kept, with their claims
        faults = [f for r in s.book.reviews_of(prev["id"]) for f in (r.get("faults") or [])]
        def pointed(p):
            words = " ".join(f"{f.get('where', '')} {f.get('problem', '')}" for f in faults).lower()
            title = p["title"].lower()
            return not faults or (title and title in words) or any(len(w) > 24 and w.lower() in p["previous"].lower()
                                                                  for f in faults for w in [str(f.get("where", "")).strip(" '\"")])
        keep = [not pointed(p) for p in parts]
        if all(keep):
            keep = [False] * len(parts)
    else:
        keep = [False] * len(parts)
    for i, p in enumerate(parts):
        last = i == len(parts) - 1
        if keep[i] and not last:
            texts.append(f"### {p['title']}\n\n" + p["previous"])
            claims += [c for c in prev.get("claims", []) if isinstance(c, dict) and str(c.get("value", "")) and str(c.get("value", "")) in p["previous"]]
            continue
        payload = {**part_payload(s, sid, p["results"]), "this part": {"title": p["title"], "focus": p["focus"], "words": p["words"], "last": last},
                   "the section's parts": [q["title"] for q in parts], "written so far": [" ".join(x.split()[:80]) for x in texts]}
        system = PART_SYSTEM
        if mode != "draft":
            payload["previous text of this part"] = p["previous"]
            payload["reviews"], payload["audit"] = reviews, audit
            system = PART_SYSTEM + (PART_CORRECT if mode == "correct" else PART_REVISE)
        a = ask(s, agent, f"write {sid} v{n} part {i + 1}", system, payload, WRITE_SCHEMA, min(12000, int(p["words"] * 2.4) + 2500))
        if a is None:
            return None
        texts.append(f"### {p['title']}\n\n" + str(a.get("text", "")).strip())
        claims += [c for c in a.get("claims", []) if isinstance(c, dict)]
        figures += [f for f in a.get("figures", []) if f in s.book.results and f not in figures]
        if last:
            proposals, gaps = [str(x)[:600] for x in a.get("proposals", [])][:8], str(a.get("gaps", ""))[:1500]
        petition = petition or a.get("petition")
    return {"kind": "version", "section": sid, "n": n, "parent": prev["id"] if prev else None, "text": "\n\n".join(texts), "claims": claims,
            "figures": figures[:4], "proposals": proposals, "gaps": gaps, "petition": petition, "parts": len(parts)}


# ---- the partnership's reviewers ------------------------------------------------------------------------------------------------------
REVIEW_SYSTEM = (HOUSE + " You are a reviewer of the partnership. Read the version below and list its faults, each against a statement of the "
                 "scorecard, with where it is, what is wrong and the correction. A fault is what would mislead a reader, or a part of the question "
                 "left unanswered, or an inference the results do not support. What is correct is not a fault, and is not listed: a number rounded "
                 "from its fact (0.649 for 0.6486, 37.1% for 0.3712) is correct; only a number that differs from its fact after rounding is wrong. "
                 "List at most ten faults, the most important first, each in a sentence or two. Severity: minor, corrected by changing a few words or "
                 "a number, or an unsupported claim of cause to be labelled as context or cut, without another review; major, a statement of "
                 "fact that is false (a number or a reading of a result the facts contradict), or the question left unanswered, which needs "
                 "a passage rewritten and another review; not an "
                 "analysis missing, which needs another review. A version with no fault is adopted, one with minor faults only is adopted once "
                 "corrected, one with a major fault is revised: the scores and the recommendation follow from your faults by that rule. Comments: "
                 "at most 80 words on what the writer should keep or strengthen.")
REREVIEW = (" This version is a revision. The faults the reviewers found in the version before are given: first judge whether each is "
            "mended, and list as faults only those not mended (with the severity they had); add a new fault only if it is major and was "
            "introduced by the revision. Do not raise in a revision what you could have raised before.")
REVIEW_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["faults", "comments"],
                 "properties": {"faults": {"type": "array", "items": {"type": "object", "additionalProperties": False,
                                                                      "required": ["statement", "severity", "where", "problem", "correction"],
                                                                      "properties": {"statement": {"type": "string", "enum": [c for c, _, _ in bk.SCORECARD]},
                                                                                     "severity": {"type": "string", "enum": ["minor", "major"]},
                                                                                     "where": {"type": "string"}, "problem": {"type": "string"}, "correction": {"type": "string"}}}},
                                "comments": {"type": "string"}}}


def judged(faults: list[dict]) -> tuple[dict, str]:
    """The scorecard's levels and the recommendation that follow from a reviewer's faults (the scorecard's declared rule): a statement
    meets with no fault against it, fails with three major faults or more, and is partly met otherwise; a version with no fault is
    adopted, with minor faults only adopted once corrected, with a major fault revised."""
    scores = {}
    for code, _, _ in bk.SCORECARD:
        mine = [f for f in faults if f.get("statement") == code]
        majors = sum(f.get("severity") == "major" for f in mine)
        scores[code] = "meets" if not mine else "fails" if majors >= 3 else "partly"
    recommend = "adopt" if not faults else "revise" if any(f.get("severity") == "major" for f in faults) else "minor"
    return scores, recommend


def rule_review(s: Society, v: dict) -> dict:
    facts = s.book.facts_of(v["section"])
    wrong = [c for c in v.get("claims", []) if not bk.claim_holds(c, facts)[0]]
    scores = {c: "meets" for c, _, _ in bk.SCORECARD}
    if wrong:
        scores["S1"] = "fails" if len(wrong) > 2 else "partly"
    # a dry reviewer asks for minor corrections on a first version it finds correct, so that the path of corrections is walked too
    minor = not wrong and v.get("n") == 1 and int(bk.key(v["id"])[:2], 16) % 2 == 0
    return {"scores": scores, "comments": "; ".join(f"{c.get('value')} does not match {c.get('fact')}" for c in wrong[:5]) or ("tighten the opening" if minor else ""),
            "recommend": "revise" if wrong else "minor" if minor else "adopt"}


def reviewer(s: Society, agent: dict) -> list[tuple[str, dict]]:
    """Reviews the versions assigned to it whose proposals are open: an assessment with the scorecard, then a vote."""
    out = []
    lg = next((c for c in agent["communities"] if s.communities[c].preset in ("first-league", "league", "partnership")), None)
    if lg is None:
        return out
    c = s.communities[lg]
    open_props = {p["id"]: p for p in c.state.get("proposals", []) if p["status"] == "open" and p["decision"] == "adopt section"}
    done, most = 0, 4 if agent["role"] == "editor" else 2
    for sec in sorted(s.book.at("review"), key=lambda x: (x.get("review opened") or 0, x["id"])):
        vid = sec.get("under review")
        pid = f"adopt-{vid}"
        if agent["id"] not in sec.get("reviewers", []) or pid not in open_props:
            continue
        if any(v["voter"] == agent["id"] for v in open_props[pid]["votes"]):
            continue
        if done >= most:  # a reviewer reads at most two versions a round (an editor alone, four)
            break
        v = dict(s.book.versions[vid], id=vid)
        if dry(s, agent):
            a = rule_review(s, v)
        else:
            facts = s.book.facts_of(v["section"])
            cited = {c.get("fact"): facts.get(c.get("fact")) for c in v.get("claims", []) if isinstance(c, dict) and c.get("fact") in facts}
            payload = {"section": section_brief(s, v["section"]), "regime": sec.get("regime"), "version": {k: v.get(k) for k in ("text", "claims", "proposals", "gaps")},
                       "facts cited": cited, "scorecard": [{"code": c_, "statement": t} for c_, _, t in bk.SCORECARD]}
            system = REVIEW_SYSTEM
            parent = v.get("parent")
            if parent and parent in s.book.versions:
                payload["faults found in the version before"] = [{k: f.get(k) for k in ("statement", "severity", "where", "problem")}
                                                                  for r in s.book.reviews_of(parent) for f in (r.get("faults") or [])][:24]
                system = REVIEW_SYSTEM + REREVIEW
            got = ask(s, agent, f"review {vid}", system, payload, REVIEW_SCHEMA, 9000)
            if got is None:
                continue
            faults = [f for f in got.get("faults", []) if isinstance(f, dict)][:20]
            scores, recommend = judged(faults)
            a = {"scores": scores, "recommend": recommend, "faults": faults,
                 "comments": "; ".join(f"[{f.get('severity')}, {f.get('statement')}] {f.get('where', '')}: {f.get('problem', '')} Correction: {f.get('correction', '')}" for f in faults)[:2400]
                             + (" | " + str(got.get("comments", ""))[:600] if got.get("comments") else "")}
        level = bk.weakest(a.get("scores", {}))
        body = {"kind": "review", "version": vid, "scores": a.get("scores", {}), "level": level, "comments": str(a.get("comments", ""))[:3000],
                "faults": a.get("faults", []), "recommend": a.get("recommend"), "by": agent["id"], "origin": agent["origin"], "community": agent.get("home"),
                "model": agent.get("model") if not dry(s, agent) else "rule", "regime": sec.get("regime"), "turn": s.turn}
        out.append(act(agent, lg, "assess", s, {"object": vid, "criterion": "the partnership's scorecard", "value": level}, _body=body))
        out.append(act(agent, lg, "vote", s, {"id": pid, "value": "yes" if a.get("recommend") in ("adopt", "minor") else "no"}))
        done += 1
    return out
