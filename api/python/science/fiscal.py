"""A local or regional government's finances, for scripts in Python and the works' servers (ADR 0145): asked of the engine a work has
built around ``api/cpp/science/fiscal.hpp``, which documents the requests.

    from science import fiscal as fs
    A = fs.accounts("thousands", "an invented currency", chart=[fs.line("110", "property tax", "own"), ...],
                    years=[fs.year(2024, {"110": 600, ...})])
    fs.assess(A, ratios=[fs.ratio("operating surplus", [("own", 1), ("operating", -1)], [("own", 1)])],
              thresholds=[fs.threshold("operating surplus", "at_least", warning=0.1, intervention=0.0)],
              projection=fs.projection(2024, 5, {"own": 0.03}, surplus=[("own", 1), ("operating", -1)], loans=[fs.loan("bank", 1000, 0.05, 2025, 10)]),
              capacity=fs.capacity(0.06, 10, 2025, floor=1.5))
    R = fs.local_government("FR")                # Eurostat's local government sector, from the data library
    fs.debt(R["debt_share"][-1], r=[...], g=[...], pb=[...], record=fs.record_of(R))

Every threshold, rate, floor and ceiling is the caller's declaration; nothing here is any jurisdiction's limit.
"""

from __future__ import annotations

import json
from pathlib import Path

from .programmes import ask

ROOT = Path(__file__).resolve().parents[3]
PRODUCT = ROOT / "data" / "cache" / "products" / "local-government-finance" / "eurostat-s1313.json"


def line(code: str, name: str, category: str) -> dict:
    if not category:
        raise ValueError(f"the line {code} needs its category")
    return {"code": str(code), "name": str(name), "category": str(category)}


def year(y: int, amounts: dict) -> dict:
    return {"year": int(y), "amounts": {str(k): float(v) for k, v in amounts.items()}}


def accounts(unit: str, currency: str, chart: list, years: list) -> dict:
    codes = [l["code"] for l in chart]
    if len(set(codes)) != len(codes):
        raise ValueError("the chart maps each line once")
    return {"unit": str(unit), "currency": str(currency), "chart": chart, "years": years}


def terms(pairs) -> list:
    return [{"category": str(c), "sign": float(s)} for c, s in pairs]


def ratio(name: str, numerator, denominator) -> dict:
    return {"name": str(name), "numerator": terms(numerator), "denominator": terms(denominator)}


def threshold(ratio: str, sound: str, warning: float | None = None, intervention: float | None = None) -> dict:
    if sound not in ("at_least", "at_most"):
        raise ValueError("the sound side is at_least or at_most")
    t = {"ratio": str(ratio), "sound": sound}
    if warning is not None:
        t["warning"] = float(warning)
    if intervention is not None:
        t["intervention"] = float(intervention)
    return t


def loan(name: str, principal: float, rate: float, first_year: int, tenor: int, bullet: bool = False) -> dict:
    if principal < 0 or tenor < 1:
        raise ValueError("a loan has a principal that is not negative and a tenor of a year or more")
    return {"name": str(name), "principal": float(principal), "rate": float(rate), "first_year": int(first_year), "tenor": int(tenor), "bullet": bool(bullet)}


def projection(base_year: int, years: int, growth: dict, surplus, loans: list | None = None) -> dict:
    return {"base_year": int(base_year), "years": int(years), "growth": {k: float(v) for k, v in growth.items()}, "surplus": terms(surplus), "loans": list(loans or [])}


def capacity(rate: float, tenor: int, first_year: int, floor: float, revenue=None, ceiling: float | None = None) -> dict:
    if floor <= 0:
        raise ValueError("a positive floor of coverage")
    c = {"rate": float(rate), "tenor": int(tenor), "first_year": int(first_year), "floor": float(floor)}
    if revenue is not None and ceiling is not None:
        c["revenue"], c["ceiling"] = terms(revenue), float(ceiling)
    return c


def assess(accounts: dict, ratios: list | None = None, thresholds: list | None = None, projection: dict | None = None, capacity: dict | None = None) -> dict:
    req = {"accounts": accounts, "ratios": list(ratios or []), "thresholds": list(thresholds or [])}
    if projection:
        req["projection"] = projection
    if capacity:
        if not projection:
            raise ValueError("debt capacity is read from a projection")
        req["capacity"] = capacity
    return ask("fiscal.assess", **req)


def debt(d0: float, r: list, g: list, pb: list, record: dict | None = None, block: int = 2, draws: int = 2000, threshold: float | None = None,
         quantiles: list | None = None, seed: int = 1) -> dict:
    """The path of debt as a share of its base, the balance that stabilises it, and (with a record {r, g, pb}) a fan of paths."""
    if not (len(r) == len(g) == len(pb)):
        raise ValueError("a rate, a growth and a balance for every year ahead")
    req = {"d0": float(d0), "r": list(map(float, r)), "g": list(map(float, g)), "pb": list(map(float, pb))}
    if record:
        req.update({"record": record, "block": int(block), "draws": int(draws), "seed": int(seed), "quantiles": list(quantiles or [0.1, 0.5, 0.9])})
        if threshold is not None:
            req["threshold"] = float(threshold)
    return ask("fiscal.debt", **req)


def effort(governments: list, bases: list, revenues: list, base_names: list | None = None) -> dict:
    return ask("fiscal.effort", governments=list(governments), bases=bases, revenues=revenues, base_names=list(base_names or []))


def local_government(geo: str) -> dict:
    """A country's local government sector from the data library's product (Eurostat S1313)."""
    if not PRODUCT.is_file():
        raise LookupError("the local government sector is not in the data library's cache: python data/products/local_government_finance.py")
    d = json.loads(PRODUCT.read_text(encoding="utf-8"))
    for c in d["countries"]:
        if c["geo"] == geo:
            return dict(c, credit=d["credit"])
    raise LookupError(f"no country {geo} in the product: {', '.join(c['geo'] for c in d['countries'])}")


def record_of(country: dict) -> dict:
    """The years with every one of rate, growth and primary balance, as a record for a fan."""
    rows = [(r, g, p) for r, g, p in zip(country["interest_rate"], country["growth"], country["primary_balance_share"]) if None not in (r, g, p)]
    if len(rows) < 2:
        raise ValueError("fewer than two years with rate, growth and balance")
    return {"r": [x[0] for x in rows], "g": [x[1] for x in rows], "pb": [x[2] for x in rows]}


# ---- Land value capture and land readjustment (ADR 0146) -------------------------------------------------------------------------

def parcel(id: str, area: float, without: float, with_: float, frontage: float = 0.0, x: float = 0.0, y: float = 0.0) -> dict:
    """A parcel with its value without and with the project at the valuation date (the counterfactual is the caller's)."""
    return {"id": str(id), "area": float(area), "without": float(without), "with": float(with_), "frontage": float(frontage), "x": float(x), "y": float(y)}


def capture_compare(parcels: list, cost: list, instruments: dict, rate: float = 0.04, statutory: dict | None = None) -> dict:
    """Each declared instrument's payments, present value, cost recovery and capture of the uplift beside the statutory rate, who pays,
    what tax increment diverts, and the parcels charged more than their uplift."""
    known = {"levy", "rights", "auction", "increment", "obligation"}
    if set(instruments) - known:
        raise ValueError(f"instruments are {', '.join(sorted(known))}; not {', '.join(sorted(set(instruments) - known))}")
    return ask("capture.compare", parcels=parcels, cost=cost, instruments=instruments, rate=float(rate), statutory=dict(statutory or {}))


def readjust(areas: dict, value_before: float, value_after: float, costs: list, sale_year: int, rate: float = 0.04, cap: float | None = None,
             selling_cost: float = 0.0, subsidies: list | None = None, owners: list | None = None, plots: list | None = None) -> dict:
    """Land readjustment: the area identity, the contribution ratio, whether value is kept, the cap, feasibility; with owners and plots,
    the replotting by the least total distance and each owner's settlement."""
    for k in ("public_before", "private_before", "public_after", "returned", "reserve"):
        if k not in areas:
            raise ValueError(f"the areas give {k}")
    req = {"areas": areas, "value_before": float(value_before), "value_after": float(value_after), "costs": costs, "sale_year": int(sale_year), "rate": float(rate),
           "selling_cost": float(selling_cost), "subsidies": list(subsidies or [])}
    if cap is not None:
        req["cap"] = float(cap)
    if owners is not None and plots is not None:
        req["owners"], req["plots"] = owners, plots
    return ask("capture.readjust", **req)
