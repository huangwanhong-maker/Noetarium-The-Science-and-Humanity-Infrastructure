"""What the tools and the page of *Allocation and justice* share: the cases the data library holds (data/products/allocation_cases.py)
and the rounds declared on them. A round is a declaration: its amount, its stages, their principles and their parameters are said
by whoever declares it, and the examples here are illustrative, no authority's. Nothing of the computation is done here; the engine
(the API's science/allocation.hpp) does it."""

from __future__ import annotations

import copy
import json
from pathlib import Path

from science import allocation as al

ROOT = Path(__file__).resolve().parents[5]
PRODUCTS = ROOT / "data" / "cache" / "products" / "allocation"
BUILD = "python data/products/allocation_cases.py"


def case(name: str) -> dict:
    p = PRODUCTS / f"{name}.json"
    if not p.is_file():
        raise LookupError(f"the case {name} is not in the data library's cache: {BUILD}")
    return json.loads(p.read_text(encoding="utf-8"))


def cases() -> list:
    index = PRODUCTS / "index.json"
    if not index.is_file():
        return []
    return [{"name": k[:-5], "title": v["title"], "credit": v["credit"]} for k, v in json.loads(index.read_text(encoding="utf-8")).items()]


# ---- Rounds declared on the cases (illustrative) --------------------------------------------------------------------------------

DECLARED = "an illustrative declaration of this plug-in's example, no authority's"


def england_round(amount: float = 1e9, per_person: float = 80.0, rule: str = "proportional", share: float = 0.85) -> dict:
    """England's districts: a fund whose first part answers a claim of `per_person` pounds for every income-deprived person (rationed by
    `rule` when the fund cannot meet them all), and whose rest is shared by banded points on income and employment deprivation and
    size, after Porto Alegre's procedure."""
    D = case("england-iod2025")
    ents = []
    for e in D["entities"]:
        x = copy.deepcopy(e)
        x["at"]["need_claim"] = round(e["at"]["income_deprived"] * per_person, 2)
        ents.append(x)
    stages = [al.stage("claims", "need: a sum for every income-deprived person", who="the example", why=DECLARED, attribute="need_claim", rule=rule, share=share),
              al.stage("points", "deprivation and size, banded", who="the example", why=DECLARED,
                       indicators=[{"name": "income_rate", "edges": [0.15, 0.25, 0.35], "rising": True, "weight": 2},
                                   {"name": "employment_rate", "edges": [0.08, 0.12, 0.16], "rising": True, "weight": 1},
                                   {"name": "population", "edges": [100000, 200000, 400000], "rising": True, "weight": 1}])]
    sources = dict(D["sources"], need_claim=f"income-deprived people × £{per_person:g}, declared")
    return al.round_(f"A fund of £{amount / 1e6:,.0f} m for England's districts", amount, ents, stages, unit="£", resource="a grant", sources=sources)


def melbourne_round(amount: float = 2e6, per_person: float = 2000.0, least: float = 500) -> dict:
    """The City of Melbourne's areas: a care fund three quarters of which answer a claim of `per_person` dollars for every person needing
    assistance, served area by area from the highest share needing it (who first), the rest by people on low incomes; areas of fewer
    than `least` people are not places people live and take no part."""
    D = case("melbourne-areas")
    ents = []
    for e in D["entities"]:
        x = copy.deepcopy(e)
        x["at"]["care_claim"] = round(e["at"]["need"] * per_person, 2)
        ents.append(x)
    stages = [al.stage("eligibility", "residence: places where people live", who="the example", why=DECLARED, attribute="population", bound=least),
              al.stage("priority", "urgency: the highest share needing assistance first", who="the example", why=DECLARED, attribute="need_rate", claim="care_claim", rule="proportional", share=0.75),
              al.stage("proportional", "low income", who="the example", why=DECLARED, attribute="low_income")]
    sources = dict(D["sources"], care_claim=f"people needing assistance × A${per_person:g}, declared")
    return al.round_(f"A care fund of A${amount / 1e6:,.1f} m for the City of Melbourne's areas", amount, ents, stages, unit="A$", resource="care grants", sources=sources)


ROUNDS = {"england-iod2025": england_round, "melbourne-areas": melbourne_round}

PRINCIPLES = {
    "england-iod2025": [al.principle("leximin", "the least per income-deprived person"), al.principle("prioritarian", "prioritarian (ε = 1)", parameter=1),
                        al.principle("opportunity", "the least for a kind of authority")],
    "melbourne-areas": [al.principle("leximin", "the least per person needing assistance"), al.principle("shortfall", "areas short of A$1,000 each", parameter=1000, alpha=0),
                        al.principle("prioritarian", "prioritarian (ε = 1)", parameter=1)],
}
READINGS = {"england-iod2025": {"level": "", "per": "income_deprived"}, "melbourne-areas": {"level": "", "per": "need"}}


# ---- Resources of many kinds allocated by evolution, declared on a case (ADR 0137; illustrative) -----------------------------------

EVOLVING = {"melbourne-areas": "Melbourne: resources of many kinds, by evolution",
            "melbourne-economy": "Melbourne: a care fund in an economy of places"}


def evolving(name: str = "melbourne-areas", fund: float = 2000.0, crews: int = 6, duties: int = 4, rise: float = 1.05, least: float = 500,
             generations: int = 300, mu: int = 50, seed: int = 1) -> dict:
    """A declared city case on the City of Melbourne's areas: a care fund in thousands of dollars claimed by the areas, granted in an
    order while it lasts (a claim it can no longer meet met in part), each grant raising the price of the next (contractors' demand); outreach crews in units, at most two to an
    area; maintenance duties that may not be thrown away, all to be assigned; three sites, each to one area or none, a shade canopy on
    each facing a direction (the sun stands to the north); the centre's west granted before Southbank's east (a shared service,
    declared). Every number and the value they are judged by are this example's declarations, illustrative and no authority's."""
    if name == "melbourne-economy":
        return economy_case(generations=generations, seed=seed, least=least)
    D = case(name)
    ents = [e for e in D["entities"] if e["at"].get("population", 0) >= least]
    recipients = [al.entity(e["id"], e["name"], e.get("group", ""), **e["at"]) for e in ents]
    ids = [e["id"] for e in ents]
    places = {e["id"]: [e["lat"], e["lon"]] for e in ents if "lat" in e}
    resources = [
        al.resource("care fund", al.simple_type("care fund (A$ thousands)", "natural"), "claims", int(fund), partial=True),
        al.resource("outreach crews", al.simple_type("outreach crews", "natural"), "units", int(crews), caps=[2] * len(ents)),
        al.resource("maintenance duties", al.simple_type("maintenance duties", "natural", disposable=False), "units", int(duties)),
        al.resource("sites", al.items_type("sites", ["community hub", "clinic room", "cool refuge"]), "items", {"community hub": 1, "clinic room": 1, "cool refuge": 1}),
    ]
    precedence = []
    if "melbourne-cbd---west" in ids and "southbank---east" in ids:
        precedence.append({"before": "melbourne-cbd---west", "after": "southbank---east", "why": "a service shared with the centre's west, set up there first (declared)"})
    P = al.problem("A care fund, crews, duties and sites for the City of Melbourne's areas (declared, illustrative)", recipients, resources,
                   attributes=[al.attribute("canopy direction", "circle")], precedence=precedence, price={"resource": "care fund", "start": 1.0, "factor": rise})
    value = al.evaluator("value to the people in need", [
        al.term("works", "care fund", "log1p", coefficient=1.0, weight="need", per="need"),
        al.term("amount", "outreach crews", "linear", coefficient=40.0, weight="need_rate"),
        al.term("amount", "maintenance duties", "square", coefficient=-6.0),
        al.term("items", "sites", "sqrt", coefficient=60.0, weight="low_income_rate", align={"attribute": "canopy direction", "toward": [0, 1]}),
    ], standing="computed", units_per="population", units_scale=1000)
    comparing = {"metric": "value to the people in need", "per": "population",
                 "principles": [al.principle("total", "the total"), al.principle("leximin", "the least served area, per thousand residents"),
                                al.principle("shortfall", "areas below 1 a thousand residents", parameter=1, alpha=0)]}
    settings = {"generations": int(generations), "mu": int(mu), "seed": int(seed), "keep": 8, "survival": "alps", "when": "2026-11-20T00:00:00Z"}
    return {"case": name, "title": EVOLVING.get(name, name), "problem": P, "evaluators": [value], "comparing": comparing, "settings": settings,
            "places": places, "credit": D.get("credit", ""), "said": DECLARED,
            "words": {"total": "total value", "least": "the least served area", "unit": "a thousand residents"}}


# ---- A care fund in an economy of places, its habits calibrated to records (ADR 0141; illustrative) -------------------------------

_CALIBRATED: dict = {}


def economy_places(ents: list, per: float = 1000.0, holding: float = 800.0, firm: float = 50.0) -> list:
    """The areas as places of an economy: one household of the model for every `per` residents, holding `holding` (A$ thousands) at the
    start less in proportion to the share on low incomes, and one firm for every two households, holding `firm`; all declared."""
    out = []
    for e in ents:
        h = max(2, round(e["at"]["population"] / per))
        out.append(al.place(e["name"], h, max(1, round(h / 2)), round(holding * (1 - e["at"].get("low_income_rate", 0))), round(firm)))
    return out


def calibrated_habits(places: list, short_below: float = 300.0) -> dict:
    """The economy's habits calibrated to the data library's record (the EU27's accounts of 2022 and the euro area's velocity of
    households' money that year, a period a month), once for these places; a household below `short_below` (A$ thousands for a
    thousand residents) is short of money, declared."""
    key = json.dumps([places, short_below], sort_keys=True)
    if key not in _CALIBRATED:
        H = al.habits_record()
        C = al.calibrate(places, H["accounts"], H["velocity"], habits=al.habits(short_below=short_below, short_local=0.95, exports=0.02, outside_money=20000))
        _CALIBRATED[key] = {"calibration": C, "record": H}
    return _CALIBRATED[key]


def economy_case(fund: float = 2000.0, least: float = 500, per_person: float = 2.0, generations: int = 40, mu: int = 24, seed: int = 1) -> dict:
    """A care fund of A$2 m in thousands for the City of Melbourne's areas, each claiming A$2,000 for every person needing assistance
    (more than the fund holds), granted in an order while it lasts, the claim it can no longer meet met in part and the rest not at
    all, so that the order decides who is served; judged twice: by the money re-spent in each area after its grant arrives, in an
    economy of places whose every payment is posted (its habits calibrated to the only records the data library holds, the EU27's
    accounts and the euro area's money, which are not Melbourne's), and by a declared value to the people in need. Grants arrive a
    month apart in the allocation's order; households short of money spend all they hold and mostly in their own area, so money
    given first to areas short of it is re-spent there before the next grant comes."""
    D = case("melbourne-areas")
    ents = [e for e in D["entities"] if e["at"].get("population", 0) >= least]
    recipients = []
    for e in ents:
        x = al.entity(e["id"], e["name"], e.get("group", ""), **e["at"])
        x["at"]["claim"] = round(e["at"]["need"] * per_person, 3)
        recipients.append(x)
    places_xy = {e["id"]: [e["lat"], e["lon"]] for e in ents if "lat" in e}
    P = al.problem("A care fund for the City of Melbourne's areas, in an economy of places (declared, illustrative)", recipients,
                   [al.resource("care fund", al.simple_type("care fund (A$ thousands)", "natural"), "fixed", int(fund), demand="claim", partial=True)])
    places = economy_places(ents)
    cal = calibrated_habits(places)
    C = cal["calibration"]
    # named after the metric it answers, which the comparison and the page read by name
    economy = al.economy(places, resource="care fund", habits=C["habits"], name="local re-spending lost", unit=1.0, spacing=1, periods=24)
    value = al.evaluator("value to the people in need", [al.term("amount", "care fund", "log1p", coefficient=1.0, weight="need", per="need")],
                         standing="computed", units_per="population", units_scale=1000)
    comparing = {"metric": "local re-spending lost", "per": "",
                 "principles": [al.principle("total", "the total re-spent"), al.principle("shortfall", "areas whose grant re-spent less than A$100,000", parameter=100, alpha=0)]}
    settings = {"generations": int(generations), "mu": int(mu), "seed": int(seed), "keep": 8, "survival": "alps", "when": "2026-11-21T00:00:00Z"}
    said = (f"{DECLARED}. The economy's habits are calibrated to the EU27's accounts of 2022 and the euro area's velocity of households' "
            f"money (a share spent of {C['habits']['spend']:.3f} a month matches {C['target']:.4f}); those records are a continent's, not "
            f"Melbourne's, and a nation's domestic share is far more local than a city area's")
    return {"case": "melbourne-economy", "title": EVOLVING["melbourne-economy"], "problem": P, "evaluators": [economy, value], "comparing": comparing,
            "settings": settings, "places": places_xy, "credit": D.get("credit", "") + "; " + cal["record"]["credit"], "said": said, "calibration": C,
            "words": {"total": "re-spent in their own areas (A$ thousands)", "least": "the least re-spent in an area", "unit": "A$ thousand"}}


def money(v: float, unit: str = "£") -> str:
    a = abs(v)
    return f"{unit}{v / 1e9:.2f} bn" if a >= 1e9 else f"{unit}{v / 1e6:.2f} m" if a >= 1e6 else f"{unit}{v:,.0f}"


def declared_round(name: str, params: dict) -> dict:
    """The example's round on a case, with the amount, the sum a person in need and the rule a person may set."""
    def g(k, d):
        v = params.get(k)
        return float(v) if v not in (None, "") else d
    if name == "england-iod2025":
        return england_round(g("amount", 1e9), g("per_person", 80), str(params.get("rule") or "proportional"))
    if name == "melbourne-areas":
        return melbourne_round(g("amount", 2e6), g("per_person", 2000))
    raise ValueError(f"no round is declared for the case {name}: england-iod2025 or melbourne-areas")


def budget_of(name: str, satisfaction: str = "cost") -> tuple:
    """A Pabulib case and the engine's answer: greedy, equal shares and Phragmén, with the city's own selection, by neighbourhood."""
    C = case(name)
    recorded = [k for k, p in enumerate(C["projects"]) if p["selected"]]
    B = al.budget(C["budget"], C["costs"], C["approvals"], rules=["greedy", "equal_shares", "phragmen"], satisfaction=satisfaction,
                  groups=C["voter_neighbourhood"], group_names=C["neighbourhoods"], recorded=recorded)
    return C, B


def budget_said(C: dict, B: dict) -> str:
    by = {r["rule"]: r for r in B["rules"]}
    recorded = sorted(k for k, p in enumerate(C["projects"]) if p["selected"])
    agree = sorted(by["greedy"]["funded"]) == recorded
    cur = C.get("currency", "")
    return (f"{len(C['approvals']):,} voters, {len(C['costs'])} projects, a budget of {C['budget']:,.0f} {cur}. "
            f"Under the city's own count {by['recorded']['share_with_nothing']:.1%} of voters saw none of their projects funded; under equal shares "
            f"{by['equal_shares']['share_with_nothing']:.1%}, under Phragmén {by['phragmen']['share_with_nothing']:.1%}. "
            f"Utilitarian greedy {'reproduces' if agree else 'does not reproduce'} the city's selection. {by['greedy']['ejr'].get('why', '')}. {C['credit']}.")
