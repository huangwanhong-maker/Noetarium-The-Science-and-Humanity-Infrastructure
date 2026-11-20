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
