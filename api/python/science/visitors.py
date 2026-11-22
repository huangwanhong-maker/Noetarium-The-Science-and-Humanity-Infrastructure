"""The visitor stock, for scripts in Python and the works' servers (ADR 0156): how many visitors are present in a place month by month
and from where, under declared readings of "visitor", with intensity and seasonality against declared bands, asked of the engine a work
has built around ``api/cpp/science/visitors.hpp``, which documents the request.

    from science import visitors
    P = visitors.product("european-cities", 2024)
    visitors.ask("visitors.stock", places=[visitors.place(p, P["year"]) for p in P["places"]], readings=visitors.readings())

The engine is the Urban Studio's plug-in *Visitors, how many and when*: ``data/cache/engines/urban-studio/visitors/engine``;
``NOETARIUM_VISITORS_ENGINE`` names another. The accommodation returns come from the data library's ``visitor_stock.py`` (Eurostat).
"""

from __future__ import annotations

import json
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ROOT = Path(__file__).resolve().parents[3]
PRODUCTS = ROOT / "data" / "cache" / "products" / "visitors"
ENGINE = Engine("urban-studio/visitors", env="NOETARIUM_VISITORS_ENGINE")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    return ENGINE.ask(op, timeout=timeout, **request)


def product(case: str = "european-cities", year: int = 2024) -> dict:
    """A case's accommodation returns, as the data library made them."""
    path = PRODUCTS / f"{case}-{year}.json"
    if not path.is_file():
        raise LookupError(f"no visitors' product {case} for {year}: python data/products/visitor_stock.py --case {case} --year {year}")
    return json.loads(path.read_text(encoding="utf-8"))


def place(p: dict, year: int) -> dict:
    """A place of the product as the engine takes it; a region's platform nights carried to it by its registered nights, origin by origin
    (a declared key: platform rentals are taken to lie where registered nights do)."""
    out = {"id": p["id"], "name": p["name"], "year": year, "nights": {"registered": p["registered"], "platform": p["platform"]},
           "population": p.get("population"), "area_km2": p.get("area_km2")}
    if all(v is not None for v in (p.get("arrivals") or {}).values()) and p.get("arrivals"):
        out["arrivals"] = p["arrivals"]
    if p.get("platform_region"):
        out["allocate"] = {"source": "platform", "key_place": p["registered"], "key_region": p["registered_region"]}
    return out


def readings(unregistered: float = 0.25, same_day: float = 0.5, same_day_hours: float = 6.0) -> list:
    """Four readings of "visitor": registered establishments only; with platform rentals; with stays with friends and relatives and in
    second homes at a declared rate per registered night; with same-day visitors at a declared rate per overnight night, present for
    declared hours. The rates are declarations, no statistic's."""
    return [
        {"name": "registered only", "sources": ["registered"], "source": "Eurostat, tourist accommodation establishments (NACE I551–I553)"},
        {"name": "with platform rentals", "sources": ["registered", "platform"], "source": "with Eurostat's short-stay rentals booked through four platforms"},
        {"name": "with friends, relatives, second homes", "sources": ["registered", "platform"], "unregistered": unregistered,
         "source": f"a declared {unregistered:g} unregistered nights per registered night"},
        {"name": "with same-day visitors", "sources": ["registered", "platform"], "unregistered": unregistered, "same_day": same_day, "same_day_hours": same_day_hours,
         "source": f"a declared {same_day:g} same-day visits per overnight night, each {same_day_hours:g} hours"},
    ]


def bands(text: str) -> list:
    """Bands from text such as ``nights_per_resident<=20; peak_to_mean<=1.5; per_thousand_peak>=0``: each a declared standard."""
    out = []
    for part in str(text or "").split(";"):
        part = part.strip()
        if not part:
            continue
        for op in ("<=", ">="):
            if op in part:
                m, v = part.split(op, 1)
                out.append({"measure": m.strip(), ("upper" if op == "<=" else "lower"): float(v), "holder": "declared"})
                break
        else:
            raise ValueError(f"a band is a measure, <= or >=, and a number: {part!r}")
    return out


# ---- tourism accounts and levies (ADR 0157): science/tourism.hpp, answered by the same engine -----------------------------------

IO = ROOT / "data" / "cache" / "products" / "input-output"

# A declared basket of a visitor's spending in the place, by product of the input–output table: accommodation and food, retail,
# land transport, culture, recreation, travel agencies. No survey's: a caller states their own.
BASKET = {"I": 0.55, "G47": 0.20, "H49": 0.08, "R90-92": 0.07, "R93": 0.05, "N79": 0.05}


def table(geo: str, year: int) -> dict:
    """A country's symmetric input–output table, as the data library made it (millions of euro)."""
    path = IO / f"{geo}-{year}.json"
    if not path.is_file():
        raise LookupError(f"no input–output table for {geo} {year}: python data/products/input_output.py --geo {geo} --year {year}")
    return json.loads(path.read_text(encoding="utf-8"))


def basket(text: str) -> dict:
    """A basket from text such as ``I=0.55; G47=0.2``: product codes and shares, which must sum to one."""
    out = {}
    for part in str(text or "").split(";"):
        if "=" in part:
            k, v = part.split("=", 1)
            out[k.strip()] = float(v)
    if not out:
        return dict(BASKET)
    if abs(sum(out.values()) - 1) > 1e-6:
        raise ValueError(f"a basket's shares sum to one, not {sum(out.values()):g}")
    return out


def purchases(per_night: dict, shares: dict, basic_share: float = 0.85) -> list:
    """The engine's purchases: each product's share of each origin's spending a night, carried to basic prices by a declared share."""
    return [{"product": k, "per_night": {o: v * s for o, v in per_night.items()}, "basic_share": basic_share} for k, s in shares.items()]


# ---- a region's own table (ADR 0159): science/regional.hpp, answered by the same engine -----------------------------------------

EMPLOYMENT = ROOT / "data" / "cache" / "products" / "regional-employment"


def employment(geo: str, year: int) -> dict:
    """A region's employment by industry against its country's, as the data library made it."""
    path = EMPLOYMENT / f"{geo}-{year}.json"
    if not path.is_file():
        raise LookupError(f"no employment by industry for {geo} {year}: python data/products/regional_employment.py --geo {geo} --year {year}")
    return json.loads(path.read_text(encoding="utf-8"))


def region_table(T: dict, geo: str, quotient: str = "flq", delta: float = 0.3) -> dict:
    """The region's own table from its country's by a location quotient, in the country's table's form, with its labels and credits."""
    E = employment(geo, T["year"])
    R = ask("regional.table", table=T, units=1e-6, shares=E["shares"], size=E["size"], quotient=quotient, delta=delta)
    labels = dict(zip(T["products"], T["labels"]))
    return {**R, "geo": T["geo"], "region": geo, "year": T["year"], "labels": [labels.get(c, c) for c in R["products"]],
            "credit": f"{T['credit']}; {E['credit']}", "employment": E}
