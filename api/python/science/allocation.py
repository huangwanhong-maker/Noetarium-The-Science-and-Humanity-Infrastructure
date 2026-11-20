"""Allocation under scarcity, for scripts in Python and the works' servers (ADR 0136): who first, what and how much when there is not
enough, and by what procedure; asked of the engine a work has built around ``api/cpp/science/allocation.hpp``, which documents the
requests (run, judge, procedure, claims, budget, reserves, incidence).

    from science import allocation as al
    R = al.round_("small grants", amount=1200, unit="£",
                  entities=[al.entity("north", level=60, population=10, group="towns"), ...],
                  stages=[al.stage("floor", "sufficiency", who="the panel", why="no place below 100 a head", attribute="level", bound=100),
                          al.stage("proportional", "equality per head", who="the council", why="the rest alike", attribute="population")],
                  sources={"level": "spending returns 2025", "population": "census 2021"})
    al.run(R)                                   # what each receives, stage by stage, with its reasons, and who was reached first
    al.judge(R, [al.principle("leximin"), al.principle("shortfall", parameter=100, alpha=0)])

Nothing here decides. A round is what a person declared; ``judge`` shows where principles agree and conflict and never adds them
together; ``procedure`` checks a record against the rules of procedural justice and leaves relevance and ethicality to people.

**A language model** may take three roles (the survey *Allocative justice under scarcity*, §1.9): propose stages for a person to
declare or set aside (``propose_stages``), restate a person's objection as an appeal they can file (``articulate_appeal``), and draft
the statement of reasons from what the engine computed (``draft_reasons``), a draft refused if it states a number the result does
not hold. It is never asked to choose, to weigh, or to speak for anyone; what it answers is a proposal of grade C, and counts for
nothing until a person takes it.

The engine is a program built by a work: the Urban Studio's plug-in *Allocation and justice* builds
``data/cache/engines/urban-studio/allocation/engine``; ``NOETARIUM_ALLOCATION_ENGINE`` names another.
"""

from __future__ import annotations

import re

from .engines import Engine, EngineError  # noqa: F401

ENGINE = Engine("urban-studio/allocation", env="NOETARIUM_ALLOCATION_ENGINE")

KINDS = ("eligibility", "floor", "priority", "claims", "proportional", "points", "equalisation")
RULES = ("proportional", "equal_awards", "equal_losses", "talmud", "tal", "piniles", "adjusted_proportional", "random_arrival")
PRINCIPLES = ("total", "prioritarian", "leximin", "shortfall", "opportunity")
ACTS = ("declare", "consult", "interest", "oversee", "allocate", "appeal", "review", "commit", "draw")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


# ---- Declarations ---------------------------------------------------------------------------------------------------------------

def entity(id, name: str | None = None, group: str = "", **at) -> dict:
    """An entity to allocate to, with its attributes (numbers)."""
    bad = [k for k, v in at.items() if not isinstance(v, (int, float)) or isinstance(v, bool)]
    if bad:
        raise ValueError(f"an entity's attributes are numbers: {', '.join(bad)}")
    return {"id": str(id), "name": name or str(id), "group": group, "at": {k: float(v) for k, v in at.items()}}


def stage(kind: str, principle: str, who: str = "", why: str = "", when: str = "", **params) -> dict:
    """A stage of a round, serving a principle, declared by someone for a reason (see allocation::rounds for its parameters)."""
    if kind not in KINDS:
        raise ValueError(f"a stage is one of {', '.join(KINDS)}, not {kind!r}")
    if "rule" in params and params["rule"] not in RULES:
        raise ValueError(f"a rationing rule is one of {', '.join(RULES)}, not {params['rule']!r}")
    return {"kind": kind, "principle": principle, "source": {"who": who, "when": when, "why": why}, **params}


def round_(name: str, amount: float, entities: list, stages: list, unit: str = "", resource: str = "", sources: dict | None = None) -> dict:
    """A round: a resource short of what is wanted, the entities that want it, and the stages that divide it."""
    if not entities:
        raise ValueError("a round needs entities")
    ids = [e["id"] for e in entities]
    if len(set(ids)) != len(ids):
        raise ValueError("each entity of a round has its own id")
    if not float(amount) >= 0:
        raise ValueError("a round's amount is not negative")
    return {"name": name, "resource": resource, "unit": unit, "amount": float(amount), "entities": entities, "stages": stages, "sources": dict(sources or {})}


def principle(kind: str, name: str | None = None, parameter: float = 0, alpha: float = 1, types: list | None = None) -> dict:
    """A principle of distribution, an order over outcomes (nomos::welfare): total, prioritarian(ε), leximin, shortfall(τ, α) or
    opportunity (the least mean over types, by default the entities' groups)."""
    if kind not in PRINCIPLES:
        raise ValueError(f"a principle is one of {', '.join(PRINCIPLES)}, not {kind!r}")
    if kind == "shortfall" and not parameter > 0:
        raise ValueError("a shortfall principle needs its threshold above zero")
    p = {"kind": kind, "name": name or (kind if kind in ("total", "leximin", "opportunity") else f"{kind} {parameter:g}"), "parameter": parameter, "alpha": alpha}
    if types is not None:
        p["types"] = list(types)
    return p


# ---- Requests -------------------------------------------------------------------------------------------------------------------

def run(round: dict) -> dict:
    return ask("run", round=round)


def judge(round: dict, principles: list, level: str = "", per: str = "population", rules: list | None = None, orders: bool = True, flips: list | None = None) -> dict:
    """The round against its alternatives under the principles: the undominated options, the pivots, the flips, what is at stake."""
    if not principles:
        raise ValueError("a judgement needs at least one declared principle")
    variation = {"orders": orders}
    if rules is not None:
        variation["rules"] = list(rules)
    return ask("judge", round=round, reading={"level": level, "per": per}, principles=principles, variation=variation, flips=flips or [])


def procedure(round: dict, acts: list) -> dict:
    """The acts replayed into a sealed journal and the rules of procedural justice checked."""
    bad = [a.get("act") for a in acts if a.get("act") not in ACTS]
    if bad:
        raise ValueError(f"an act is one of {', '.join(ACTS)}, not {bad[0]!r}")
    return ask("procedure", round=round, acts=acts)


def claims(estate: float, claims: list, rules: list | None = None, theta: float = 0.5) -> dict:
    req = {"estate": estate, "claims": claims, "theta": theta}
    if rules:
        req["rules"] = rules
    return ask("claims", **req)


def budget(budget: float, costs: list, approvals: list, rules: list | None = None, satisfaction: str = "cost", complete: bool = True,
           groups: list | None = None, group_names: list | None = None, recorded: list | None = None) -> dict:
    """Projects funded by votes under each rule, with the voters left with nothing (approvals: for each voter, project indices),
    read by the voters' groups when given (an index a voter, -1 for none), and the selection recorded in fact judged alike."""
    req = {"budget": budget, "costs": costs, "approvals": approvals, "satisfaction": satisfaction, "complete": complete}
    if rules:
        req["rules"] = rules
    if groups is not None:
        req["groups"], req["group_names"] = list(groups), list(group_names or [])
    if recorded is not None:
        req["recorded"] = list(recorded)
    return ask("budget", timeout=1800.0, **req)


def reserves(individuals: int, categories: list, precedence: list | None = None) -> dict:
    return ask("reserves", individuals=individuals, categories=categories, precedence=precedence or [])


def incidence(**parts) -> dict:
    """Who finally gains: capitalisation, propagation, lm3, network, doughnut, targeting; each part answered or refused apart."""
    return ask("incidence", **parts)


# ---- A language model's roles ---------------------------------------------------------------------------------------------------

def _models():
    from . import models
    return models


ROLE = ("You assist people deciding how to allocate a scarce resource. You never decide, rank options, weigh principles against one "
        "another or speak for anyone who is not present. What you write is a proposal that a person may take or set aside.")

STAGES_SCHEMA = {
    "type": "object", "additionalProperties": False, "required": ["stages"],
    "properties": {"stages": {"type": "array", "items": {"type": "object", "additionalProperties": False,
                   "required": ["kind", "principle", "why", "attribute", "rule"],
                   "properties": {"kind": {"type": "string", "enum": list(KINDS)}, "principle": {"type": "string"}, "why": {"type": "string"},
                                  "attribute": {"type": "string"}, "rule": {"type": "string", "enum": list(RULES)}}}}},
}


def propose_stages(situation: str, round: dict, provider=None) -> dict:
    """A model's proposal of stages for a round (kinds, principles, the attribute each reads, a rationing rule, and why), each to be
    declared by a person or set aside. Numbers (floors, shares, bounds) are never proposed: a person declares them. Stages that read
    an attribute the entities do not have are refused."""
    models = _models()
    situation = " ".join(str(situation).split())
    if len(situation) < 12:
        raise models.Refusal("describe what is allocated, to whom and why it is scarce, in a sentence or two")
    have = sorted({k for e in round["entities"] for k in e["at"]})
    provider = provider or models.Anthropic()
    prompt = (f"{situation}\n\nThe entities have these attributes: {', '.join(have)}.\n"
              "Propose an ordered sequence of stages (eligibility, floor, priority, claims, proportional, points, equalisation), each naming "
              "the principle it serves, the attribute it reads (one of those listed), a rationing rule, and why. Propose no numbers.")
    data, served = provider.answer(ROLE, prompt, STAGES_SCHEMA)
    out, refused = [], []
    for s in (data or {}).get("stages", []):
        if s.get("kind") not in KINDS or s.get("rule") not in RULES:
            refused.append(f"a stage of kind {s.get('kind')!r} with rule {s.get('rule')!r}")
        elif s.get("attribute") not in have:
            refused.append(f"a stage reading {s.get('attribute')!r}, which the entities do not have")
        else:
            out.append({"kind": s["kind"], "principle": s["principle"], "attribute": s["attribute"], "rule": s["rule"],
                        "source": {"who": f"{provider.name}, {served}", "when": "", "why": s["why"]}, "grade": "C", "status": "proposed"})
    if not out:
        raise models.Refusal("the model proposed no stage that reads: " + "; ".join(refused[:4]))
    return {"stages": out, "refused": refused, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed"}


APPEAL_SCHEMA = {
    "type": "object", "additionalProperties": False, "required": ["kind", "entity", "stage", "key", "value", "reason"],
    "properties": {"kind": {"type": "string", "enum": ["datum", "parameter", "stage"]}, "entity": {"type": "string"}, "stage": {"type": "integer"},
                   "key": {"type": "string"}, "value": {"type": "number"}, "reason": {"type": "string"}},
}


def articulate_appeal(objection: str, round: dict, provider=None) -> dict:
    """A person's objection restated as an appeal they can file: on a datum (an entity's attribute), a stage's parameter, or a stage's
    applicability. Refused when it names an entity, attribute or stage the round does not have. The person files it, or not."""
    models = _models()
    objection = " ".join(str(objection).split())
    if len(objection) < 8:
        raise models.Refusal("say what is wrong, and for whom")
    provider = provider or models.Anthropic()
    listing = "; ".join(f"{e['id']} ({', '.join(f'{k}={v:g}' for k, v in e['at'].items())})" for e in round["entities"][:60])
    stages = "; ".join(f"{k}: {s['kind']} for {s.get('principle', '')}" for k, s in enumerate(round["stages"]))
    prompt = (f"The round: entities {listing}. Stages (from 0): {stages}.\n\nAn objection: {objection}\n\n"
              "Restate it as one appeal: kind datum (entity, key = the attribute, value = the value claimed), parameter (stage, key = bound, "
              "share, theta or lambda, value claimed) or stage (stage, its applicability contested). Keep the objector's reason in their terms.")
    a, served = provider.answer(ROLE, prompt, APPEAL_SCHEMA)
    ids = {e["id"] for e in round["entities"]}
    if a.get("kind") == "datum":
        e = next((e for e in round["entities"] if e["id"] == a.get("entity")), None)
        if e is None or a.get("key") not in e["at"]:
            raise models.Refusal("the restated appeal names an entity or attribute the round does not have")
    elif not (0 <= int(a.get("stage", -1)) < len(round["stages"])):
        raise models.Refusal("the restated appeal names a stage the round does not have")
    elif a.get("kind") == "parameter" and a.get("key") not in ("bound", "share", "theta", "lambda"):
        raise models.Refusal("the restated appeal names a parameter that is not bound, share, theta or lambda")
    if a.get("entity") and a["entity"] not in ids and a.get("kind") == "datum":
        raise models.Refusal("the restated appeal names an entity the round does not have")
    return {"appeal": {k: a[k] for k in ("kind", "entity", "stage", "key", "value", "reason")}, "objection": objection,
            "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed"}


REASONS_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["text"], "properties": {"text": {"type": "string"}}}
NUMBER = re.compile(r"(?<![\w.])-?\d[\d,]*(?:\.\d+)?")


def _numbers_of(x, out: set) -> set:
    if isinstance(x, bool):
        return out
    if isinstance(x, (int, float)):
        out.add(round(float(x), 2))
        out.add(round(float(x)))
    elif isinstance(x, dict):
        for v in x.values():
            _numbers_of(v, out)
    elif isinstance(x, list):
        for v in x:
            _numbers_of(v, out)
    elif isinstance(x, str):
        for m in NUMBER.findall(x):
            v = float(m.replace(",", ""))
            out.add(round(v, 2))
            out.add(round(v))
    return out


def draft_reasons(round: dict, result: dict, provider=None, audience: str = "the people the round allocates to") -> dict:
    """A draft of the statement of reasons for a round's result, from what the engine computed. Refused when it states a number
    neither the round nor its result holds, so that no figure is invented."""
    models = _models()
    provider = provider or models.Anthropic()
    facts = {"round": {k: round[k] for k in ("name", "resource", "unit", "amount")}, "stages": [{k: s.get(k) for k in ("kind", "principle", "source", "bound", "rule", "share")} for s in round["stages"]],
             "entities": [{"id": e["id"], "amount": e["amount"], "excluded": e["excluded"], "reasons": e["reasons"]} for e in result["entities"]], "left": result["left"], "notes": result["notes"]}
    import json as _json
    prompt = (f"Write, for {audience}, a short plain statement of the procedure and principles applied in this allocation and what each "
              "received, using only these facts; state no number they do not contain, and do not say whether the result is just.\n\n" + _json.dumps(facts, ensure_ascii=False))
    data, served = provider.answer(ROLE, prompt, REASONS_SCHEMA)
    text = str((data or {}).get("text", "")).strip()
    if not text:
        raise models.Refusal("the model gave no statement")
    known = _numbers_of(facts, set()) | _numbers_of(round, set())
    stray = []
    for m in NUMBER.findall(text):
        v = float(m.replace(",", ""))
        if round_known(v, known) is False and not (1 <= v <= 12 and float(v).is_integer()):   # small counts (stage numbers) pass
            stray.append(m)
    if stray:
        raise models.Refusal("the draft states numbers the result does not hold: " + ", ".join(stray[:6]))
    return {"text": text, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed"}


def round_known(v: float, known: set) -> bool:
    return round(v, 2) in known or round(v) in known
