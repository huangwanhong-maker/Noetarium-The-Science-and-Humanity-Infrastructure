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

**Resources of many kinds allocated by evolution** (ADR 0137; ``api/cpp/science/evolve.hpp`` documents the requests): a resource type
is declared (``resource_type``, ``simple_type``, ``items_type``) and verified by kategoria (``verify_type``); a problem names its
recipients, its resources with their carriers (``resource``), attributes on spaces (``attribute``), precedence and a price that rises
(``problem``); evaluators are declared closed forms (``evaluator``, ``term``), each with its standing. ``evolve`` searches and answers
the allocations kept, their comparison under principles, whether order mattered, and the sealed record. A model may compare kept
allocations in pairs (``judge_pairs``: the order of each pair swapped, several samples, a share repeated to audit its consistency);
its judgements become Bradley–Terry strengths (``strengths``) of grade C, shown beside the search's results and never entering it.

**Economic evaluators** (ADR 0141, on the allocation domain's ``evaluators``, ADR 0139): an evaluator may be an economy of places whose
every payment is posted (``economy``, its places by ``place`` and its habits by ``habits``), a declared law of holdings with a mean field
(``density``), or a pooled linear law learned from a record with its refits (``laws``; ``learn`` shows the law alone). An economy's
habits are calibrated to records (``calibrate``, on ``habits_record``: the accounts' shares read, the share of holdings spent matched to
the velocity of households' money, what is left unidentified said). A participatory budget is searched as orders of its projects decoded
by the budget (``budget_orders``), the rules read on the same two measures.

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


# ---- Resources of many kinds, allocated by evolution (ADR 0137) ----------------------------------------------------------------

QUANTITIES = ("natural", "granular", "nonnegative", "integer", "rational", "boolean", "maximum")
CARRIERS = ("shares", "claims", "units", "unit_claims", "items", "fixed")
SPACES = ("circle", "sphere", "levels", "categories", "interval")
FORMS = ("linear", "log1p", "sqrt", "square")
STANDINGS = ("verified", "computed", "estimated", "proposed")


def kind(name: str, disposable: bool = True, capacity=None, distinct: bool = False) -> dict:
    """A kind of a resource: whether it may be thrown away (refuse for duties and waste), the most a bundle may hold, and whether it is
    a labelled item held at most once."""
    k = {"name": str(name), "disposable": bool(disposable), "distinct": bool(distinct)}
    if capacity is not None:
        k["capacity"] = capacity
    return k


def component(name: str, quantity: str = "nonnegative", kinds: list | None = None, grain: int = 1) -> dict:
    if quantity not in QUANTITIES:
        raise ValueError(f"a quantity is one of {', '.join(QUANTITIES)}, not {quantity!r}")
    return {"name": str(name), "quantity": quantity, "grain": int(grain), "kinds": kinds or [kind(name)]}


def resource_type(name: str, components: list, conversions: list | None = None, laws: list | None = None, declared: list | None = None) -> dict:
    """A resource type as kategoria's resources reads it; conversions {name, from: {kind: amount}, to: {...}} and laws {name, weights:
    {kind: number}, conserved} are optional."""
    if not components:
        raise ValueError("a resource type needs components")
    return {"name": str(name), "components": components, "conversions": list(conversions or []), "laws": list(laws or []), "declared": list(declared or [])}


def simple_type(name: str, quantity: str = "nonnegative", disposable: bool = True) -> dict:
    """One component of one kind: an amount of money, units of crews, duties that may not be thrown away."""
    return resource_type(name, [component(name, quantity, [kind(name, disposable)])])


def items_type(name: str, items: list) -> dict:
    """Distinct items, each held at most once."""
    if len(set(items)) != len(items) or not items:
        raise ValueError("items need distinct names")
    return resource_type(name, [component(name, "natural", [kind(i, distinct=True) for i in items])])


def resource(name: str, type: dict, carrier: str, pool, kind: str | None = None, caps: list | None = None, demand: str | None = None,
             partial: bool = False) -> dict:
    """A resource to allocate, carried as shares, claims, units, unit claims, items or fixed demands (an attribute of each recipient);
    `partial` claims are met up to what remains when the pool can no longer meet them, instead of refusing the grant."""
    if carrier not in CARRIERS:
        raise ValueError(f"a carrier is one of {', '.join(CARRIERS)}, not {carrier!r}")
    if carrier == "fixed" and not demand:
        raise ValueError("a fixed carrier needs the recipients' attribute holding their demand")
    r = {"name": str(name), "type": type, "carrier": carrier, "pool": pool}
    if kind:
        r["kind"] = kind
    if caps is not None:
        r["caps"] = [int(c) for c in caps]
    if demand:
        r["demand"] = demand
    if partial:
        if carrier not in ("claims", "unit_claims", "fixed"):
            raise ValueError("only claims and fixed demands can be met in part")
        r["partial"] = True
    return r


def attribute(name: str, space: str, **params) -> dict:
    """An attribute chosen for each recipient on a space with no algebra: circle, sphere (dimension), levels or categories (levels),
    interval (lower, upper)."""
    if space not in SPACES:
        raise ValueError(f"a space is one of {', '.join(SPACES)}, not {space!r}")
    return {"name": str(name), "space": space, **params}


def term(of: str, resource: str | None = None, form: str = "linear", coefficient: float = 1.0, weight: str | None = None, per: str | None = None,
         align: dict | None = None) -> dict:
    """A term of a recipient's value: coefficient × weight × form(x / per), x the amount of a resource, the works a price bought, the
    items held (times the alignment of an attribute with a direction) or whether it was granted."""
    if of not in ("amount", "works", "items", "granted"):
        raise ValueError("a term is of amount, works, items or granted")
    if form not in FORMS:
        raise ValueError(f"a form is one of {', '.join(FORMS)}, not {form!r}")
    if of != "granted" and not resource:
        raise ValueError("a term names its resource")
    t = {"of": of, "form": form, "coefficient": float(coefficient)}
    for k, v in (("resource", resource), ("weight", weight), ("per", per), ("align", align)):
        if v:
            t[k] = v
    return t


def evaluator(name: str, terms: list, standing: str = "computed", objective: str = "maximise", units_per: str | None = None, units_scale: float = 1.0) -> dict:
    """A declared closed form: each recipient's value is the sum of its terms; the metric is the total, maximised or minimised; the
    per-recipient values (per `units_per` / `units_scale` when given) are its outcomes for principles."""
    if standing not in STANDINGS:
        raise ValueError(f"a standing is one of {', '.join(STANDINGS)}, not {standing!r}")
    if objective not in ("maximise", "minimise"):
        raise ValueError("an objective is maximise or minimise")
    if not terms:
        raise ValueError("an evaluator needs terms")
    e = {"name": str(name), "terms": terms, "standing": standing, "objective": objective, "units_scale": float(units_scale)}
    if units_per:
        e["units_per"] = units_per
    return e


def problem(name: str, recipients: list, resources: list, attributes: list | None = None, precedence: list | None = None, ordered: bool = True,
            order_by: str = "permutation", unmet: str = "skip", price: dict | None = None) -> dict:
    """An allocation problem: recipients (``entity``), resources, attributes, precedence [{before, after, why}] by the recipients' ids,
    whether the order of grants is searched (as a permutation or random keys), what a grant that is not allowed does (skip, defer, stop),
    and a price {resource, start, factor} that rises after each grant drawing on the resource."""
    ids = [e["id"] for e in recipients]
    if not ids or len(set(ids)) != len(ids):
        raise ValueError("a problem needs recipients, each with its own id")
    for q in precedence or []:
        if q.get("before") not in ids or q.get("after") not in ids:
            raise ValueError("a precedence names recipients the problem has")
    if order_by not in ("permutation", "keys") or unmet not in ("skip", "defer", "stop"):
        raise ValueError("order_by is permutation or keys; unmet is skip, defer or stop")
    p = {"name": str(name), "recipients": recipients, "resources": resources, "attributes": list(attributes or []), "precedence": list(precedence or []),
         "ordered": bool(ordered), "order_by": order_by, "unmet": unmet}
    if price:
        p["price"] = price
    return p


def verify_type(type: dict) -> dict:
    """kategoria's verdicts on a resource type, each with its method and scope."""
    return ask("evolve.verify", type=type)


def compose(problem: dict) -> dict:
    return ask("evolve.compose", problem=problem)


def evolve(problem: dict, evaluators: list, settings: dict | None = None, comparing: dict | None = None, timeout: float = 1800.0) -> dict:
    """The search: the allocations kept with their metrics, standing and lawfulness; their comparison under principles (comparing:
    {metric, principles, per}); whether order mattered; what the keys saved; the record sealed."""
    if not evaluators:
        raise ValueError("a search needs at least one evaluator")
    req = {"problem": problem, "evaluators": evaluators, "settings": dict(settings or {})}
    if comparing:
        req["comparing"] = comparing
    return ask("evolve.search", timeout=timeout, **req)


def strengths(items: int, judgements: list, order_effect: bool = True, prior: float = 0.0) -> dict:
    """Bradley–Terry strengths of `items` from judgements [{first, second, first_preferred}], with the advantage of being shown first."""
    return ask("evolve.strengths", items=int(items), judgements=judgements, order_effect=bool(order_effect), prior=float(prior))


# ---- Economic evaluators, calibration and budgets as orders (ADR 0141) ------------------------------------------------------------

HABITS = ("short_below", "short_spend", "short_local", "spend", "local", "wages", "supplies", "exports", "outside_money", "source")


def place(name: str, households: int = 4, firms: int = 2, household_money: int = 100, firm_money: int = 200) -> dict:
    """A place of an economy of places: its households and firms, each with an opening balance in whole units of money."""
    if not name:
        raise ValueError("a place has a name")
    if int(households) < 1 or int(firms) < 1:
        raise ValueError(f"a place has households and firms: {name}")
    if int(household_money) < 0 or int(firm_money) < 0:
        raise ValueError(f"opening balances are not negative: {name}")
    return {"name": str(name), "households": int(households), "firms": int(firms), "household_money": int(household_money), "firm_money": int(firm_money)}


def habits(**h) -> dict:
    """An economy's habits (the economics domain's Habits): shares between 0 and 1, a threshold and outside money in whole units."""
    bad = [k for k in h if k not in HABITS]
    if bad:
        raise ValueError(f"habits are {', '.join(HABITS)}; not {', '.join(bad)}")
    for k, v in h.items():
        if k in ("short_spend", "short_local", "spend", "local", "wages", "supplies", "exports") and not (0 <= float(v) <= 1):
            raise ValueError(f"the habit {k} is a share between 0 and 1")
    if float(h.get("wages", 0)) + float(h.get("supplies", 0)) > 1:
        raise ValueError("wages and supplies together are at most what a firm received")
    return dict(h)


def economy(places: list, resource: str | None = None, habits: dict | None = None, name: str = "an economy of places", unit: float = 1.0,
            spacing: float = 1.0, periods: int = 60, rounds: int = 8, trade: list | None = None) -> dict:
    """An evaluator of kind economy: each recipient a place (``place``, one for each, in the problem's order); the money a recipient is
    granted (the resource's amount × `unit`, in whole units) reaches its households in the allocation's order, a grant every `spacing`
    periods; the objective the money re-spent in its own place, lost; the accounts' audit a verified constraint."""
    if not places:
        raise ValueError("an economy needs a place for each recipient")
    if float(unit) <= 0 or float(spacing) < 0 or int(periods) < 1:
        raise ValueError("a positive unit of money, a spacing that is not negative and at least one period")
    e = {"kind": "economy", "name": str(name), "places": places, "unit": float(unit), "spacing": float(spacing), "periods": int(periods), "rounds": int(rounds)}
    if resource:
        e["resource"] = resource
    if habits:
        e["habits"] = habits
    if trade:
        e["trade"] = trade
    return e


def density(population, mean, resource: str | None = None, name: str = "a law of holdings with a mean field", unit: float = 1.0, line: float = 0.5,
            pull: float = 1.0, spread: float = 1 / 3, grid: dict | None = None, dt: float = 0.05, between: int = 10, after: int = 40) -> dict:
    """An evaluator of kind density: each recipient a population of units (an attribute's name or a number each) whose holdings move
    under Bouchaud and Mézard's law with a mean field; grants go first to those below `line`; the objective the share below it."""
    e = {"kind": "density", "name": str(name), "population": population, "mean": mean, "unit": float(unit), "line": float(line), "pull": float(pull),
         "spread": float(spread), "dt": float(dt), "between": int(between), "after": int(after)}
    if resource:
        e["resource"] = resource
    if grid:
        e["grid"] = grid
    return e


def laws(states: list, inputs: list, resource: str | None = None, name: str = "laws learned from a record", unit: float = 1.0, start: list | None = None,
         resamples: int = 20, seed: int = 1, between: int = 1, after: int = 4, margin: float = 0.0) -> dict:
    """An evaluator of kind laws: a pooled linear law x' = a x + b u + c learned from a record (states, one row a step, a value for each
    recipient; inputs, one row fewer) with refits on its steps drawn again; an allocation that drives a law beyond the record's support
    is refused by it. Its standing is estimated."""
    if len(states) < 4 or len(inputs) + 1 != len(states):
        raise ValueError("a record of at least four states and one input fewer, a step apart")
    e = {"kind": "laws", "name": str(name), "record": {"states": states, "inputs": inputs}, "unit": float(unit), "resamples": int(resamples), "seed": int(seed),
         "between": int(between), "after": int(after), "margin": float(margin)}
    if resource:
        e["resource"] = resource
    if start is not None:
        e["start"] = list(start)
    return e


def learn(states: list, inputs: list, resamples: int = 20, seed: int = 1) -> dict:
    """The law a ``laws`` evaluator would learn: its coefficients and refits, its error one step ahead against persistence's, the support."""
    return ask("evolve.learn", record={"states": states, "inputs": inputs}, resamples=int(resamples), seed=int(seed))


def habits_record(geo: str = "EU27_2020", year: int = 2022) -> dict:
    """The record an economy's habits are calibrated to, from the data library (data/products/economy_habits.py): the accounts' shares
    and the velocity of households' money per month, with their sources."""
    from pathlib import Path
    import json
    p = Path(__file__).resolve().parents[3] / "data" / "cache" / "products" / "economy-habits" / f"{geo.lower()}-{year}.json"
    if not p.is_file():
        raise LookupError(f"the record of habits is not in the data library's cache: python data/products/economy_habits.py --geo {geo} --year {year}")
    d = json.loads(p.read_text(encoding="utf-8"))
    sh = d["accounts"]["shares"]
    return {"accounts": {"local": sh["local"], "wages": sh["wages"], "supplies": sh["supplies"], "imported": sh["imported"],
                         "source": f"Eurostat naio_10_cp1750, {d['accounts']['geo']}, {d['accounts']['year']}"},
            "velocity": d["velocity"]["per_month"], "velocity_source": f"{d['velocity']['economy']}, {d['velocity']['year']}: households' consumption over mean M1, a month",
            "said": d.get("said", ""), "credit": d.get("credit", ""), "retrieved_at": d.get("retrieved_at", "")}


def calibrate(places: list, accounts: dict, velocity: float, habits: dict | None = None, short_too: bool = True, timeout: float = 900.0) -> dict:
    """An economy of places' habits calibrated: the accounts' shares read (local, wages, supplies), the share of holdings a household
    spends matched to `velocity` (households' consumption a period over the money households and firms hold), what is left
    unidentified said; a velocity the declared places cannot reach is refused with the range they can."""
    for k in ("local", "wages", "supplies"):
        if k not in accounts:
            raise ValueError(f"the accounts give {k}")
    if not float(velocity) > 0:
        raise ValueError("a velocity to match is positive")
    req = {"places": places, "accounts": accounts, "velocity": float(velocity), "short_too": bool(short_too)}
    if habits:
        req["habits"] = habits
    return ask("evolve.calibrate", timeout=timeout, **req)


def budget_orders(budget: float, costs: list, approvals: list, groups: list | None = None, group_names: list | None = None, names: list | None = None,
                  settings: dict | None = None, timeout: float = 3600.0) -> dict:
    """A participatory budget as a sequential mechanism: the orders of its projects decoded by the budget, searched under the share of
    voters with nothing and the approvals not met, the front of what orders reach, with utilitarian greedy, greedy per cost and the
    Method of Equal Shares read on the same two measures."""
    if not float(budget) > 0 or not costs:
        raise ValueError("a budget and the projects' costs")
    req = {"budget": float(budget), "costs": [float(c) for c in costs], "approvals": [[int(j) for j in a] for a in approvals], "settings": dict(settings or {})}
    if groups is not None:
        if len(groups) != len(approvals):
            raise ValueError("a group for every voter, or none")
        req["groups"] = [int(g) for g in groups]
    if group_names:
        req["group_names"] = list(group_names)
    if names:
        req["names"] = list(names)
    return ask("evolve.budget", timeout=timeout, **req)


PAIR_SCHEMA = {
    "type": "object", "additionalProperties": False, "required": ["preferred", "reason"],
    "properties": {"preferred": {"type": "string", "enum": ["first", "second", "neither"]}, "reason": {"type": "string"}},
}


def describe_allocation(problem: dict, a: dict) -> str:
    """An allocation in words for a reader: what each recipient received, in the order granted, and who was not granted."""
    names = {e["id"]: e["name"] for e in problem["recipients"]}
    parts = []
    for rid in a["granted"]:
        k = [e["id"] for e in problem["recipients"]].index(rid)
        got = [f"{res}: {a['amounts'][res][k]:g}" for res in a["amounts"] if a["amounts"][res][k]]
        items = [item for res, own in a.get("owners", {}).items() for item, who in own.items() if who == rid]
        parts.append(f"{names[rid]} ({', '.join(got + items) or 'nothing'})")
    out = "Granted in this order: " + "; ".join(parts) + "."
    if a.get("unmet"):
        out += " Not granted: " + ", ".join(names[r] for r in a["unmet"]) + "."
    return out


def judge_pairs(problem: dict, result: dict, purpose: str, provider=None, top: int = 4, samples: int = 2, audit: float = 0.25, seed: int = 1) -> dict:
    """A model's pairwise judgements of the kept allocations against a stated purpose. Each pair is shown in both orders (`samples`
    times each way), and a share `audit` of the pairs is asked again in the same order, to report how consistent the model is. The
    judgements are a proposal of grade C, for ``strengths``; the model is shown the allocations and the purpose, never the search's
    metrics, and decides nothing."""
    import random

    models = _models()
    purpose = " ".join(str(purpose).split())
    if len(purpose) < 12:
        raise models.Refusal("state the purpose the allocations are judged against, in a sentence or two")
    kept = result.get("kept", [])[:max(2, int(top))]
    if len(kept) < 2:
        raise models.Refusal("at least two kept allocations are compared")
    provider = provider or models.Anthropic()
    texts = [describe_allocation(problem, a) for a in kept]
    # Its own role: the general one forbids ranking options, and a comparison asks for exactly one judgement between two, recorded as
    # a proposal beside the search's results and deciding nothing.
    role = ("You are asked for one judgement between two allocations of the same resources, against a purpose a person states. Your "
            "judgement is recorded as a proposal of grade C beside a search's results; it decides nothing and is never added to any score. "
            "Say which allocation serves the purpose better, or that you cannot tell when they are too alike to say, with a short reason. "
            "Do not invent numbers or facts not shown, and do not speak for anyone.")
    judgements, served, asked = [], "", 0
    pairs = [(i, j) for i in range(len(kept)) for j in range(i + 1, len(kept))]
    rng = random.Random(seed)
    audited = [p for p in pairs if rng.random() < audit]
    repeats, agreements = 0, 0

    def ask_pair(first: int, second: int) -> str:
        nonlocal served, asked
        prompt = f"Purpose: {purpose}\n\nThe first allocation. {texts[first]}\n\nThe second allocation. {texts[second]}\n\nWhich serves the purpose better?"
        data, served = provider.answer(role, prompt, PAIR_SCHEMA)
        asked += 1
        pref = (data or {}).get("preferred")
        if pref not in ("first", "second", "neither"):
            raise models.Refusal("the model answered something other than first, second or neither")
        return pref

    for i, j in pairs:
        for _ in range(max(1, int(samples))):
            for first, second in ((i, j), (j, i)):
                pref = ask_pair(first, second)
                if pref != "neither":
                    judgements.append({"first": first, "second": second, "first_preferred": pref == "first"})
        if (i, j) in audited:
            a1, a2 = ask_pair(i, j), ask_pair(i, j)
            repeats += 1
            agreements += a1 == a2
    return {"judgements": judgements, "items": len(kept), "asked": asked, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed",
            "audit": {"pairs": repeats, "agreed": agreements}, "purpose": purpose}


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
