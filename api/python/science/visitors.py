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


def holiday_share(p: dict) -> float | None:
    """The share of a place's registered nights spent in holiday and other short-stay accommodation (NACE I552) over the year, from its
    own classes or, where those are not published (a NUTS 3 place), its region's: a declared stand-in, since Eurostat publishes no
    monthly nights by class."""
    for key in ("by_class", "by_class_region"):
        c = p.get(key) or {}
        if c and all(v is not None for v in c.values()) and sum(c.values()) > 0:
            return c["holiday"] / sum(c.values())
    return None


def registered_short(p: dict) -> dict | None:
    """Registered short-stay nights by origin and month: registered nights times the year's holiday share. They bound how many platform
    nights may be registered ones already (ADR 0164)."""
    h = holiday_share(p)
    if h is None:
        return None
    return {o: [None if v is None else v * h for v in months] for o, months in p["registered"].items()}


def place(p: dict, year: int) -> dict:
    """A place of the product as the engine takes it; a region's platform nights carried to it by its registered nights, origin by origin
    (a declared key: platform rentals are taken to lie where registered nights do). Its registered short-stay nights come with it, so that
    a reading may remove the platform's overlap with them (ADR 0164)."""
    out = {"id": p["id"], "name": p["name"], "year": year, "nights": {"registered": p["registered"], "platform": p["platform"]},
           "population": p.get("population"), "area_km2": p.get("area_km2")}
    short = registered_short(p)
    if short is not None:
        out["nights"]["registered_short"] = short
    if all(v is not None for v in (p.get("arrivals") or {}).values()) and p.get("arrivals"):
        out["arrivals"] = p["arrivals"]
    if p.get("platform_region"):
        out["allocate"] = {"source": "platform", "key_place": p["registered"], "key_region": p["registered_region"]}
    return out


def readings(unregistered: float = 0.25, same_day: float = 0.5, same_day_hours: float = 6.0) -> list:
    """Five readings of "visitor": registered establishments only; with platform rentals, the overlap with registered short-stay nights
    taken as large as it can be (a lower bound, nothing counted twice) and as nothing (an upper bound, Eurostat's caution unheeded); then,
    on the lower bound, stays with friends and relatives and in second homes at a declared rate per registered night, and same-day visitors
    at a declared rate per overnight night, present for declared hours. The rates are declarations, no statistic's (ADR 0156, ADR 0164)."""
    return [
        {"name": "registered only", "sources": ["registered"], "source": "Eurostat, tourist accommodation establishments (NACE I551–I553)"},
        {"name": "with platform rentals", "sources": ["registered", "platform"], "overlap": 1,
         "source": "with Eurostat's short-stay rentals booked through four platforms, less as many as could be registered already (a lower bound)"},
        {"name": "with platform rentals, none registered", "sources": ["registered", "platform"], "overlap": 0,
         "source": "platform nights added whole, as Eurostat advises against (an upper bound)"},
        {"name": "with friends, relatives, second homes", "sources": ["registered", "platform"], "overlap": 1, "unregistered": unregistered,
         "source": f"a declared {unregistered:g} unregistered nights per registered night, on the lower bound"},
        {"name": "with same-day visitors", "sources": ["registered", "platform"], "overlap": 1, "unregistered": unregistered, "same_day": same_day,
         "same_day_hours": same_day_hours, "source": f"a declared {same_day:g} same-day visits per overnight night, each {same_day_hours:g} hours, on the lower bound"},
    ]


def paid_nights(p: dict) -> dict:
    """A place's nights paid for in the year (registered and platform), as a lower and an upper bound: the platform's nights less as many as
    could be registered short-stay nights already, month by month, and whole (ADR 0164). Region's platform nights are carried to the
    place by its registered nights first, as ``place`` declares."""
    reg = [sum(v[m] or 0 for v in p["registered"].values()) for m in range(12)]
    plat = [sum(v[m] or 0 for v in p["platform"].values()) for m in range(12)]
    if p.get("platform_region"):
        region = [sum(v[m] or 0 for v in p["registered_region"].values()) for m in range(12)]
        plat = [plat[m] * reg[m] / region[m] if region[m] else 0 for m in range(12)]
    short = registered_short(p)
    short = [sum(v[m] or 0 for v in short.values()) for m in range(12)] if short else [0] * 12
    overlap = sum(min(plat[m], short[m]) for m in range(12))
    return {"registered": sum(reg), "platform": sum(plat), "overlap_max": overlap,
            "low": sum(reg) + sum(plat) - overlap, "high": sum(reg) + sum(plat)}


def nights(p: dict, year: int, overlap: float = 1.0) -> dict:
    """A place's paid nights in the year by origin (registered and platform), asked of the engine: the platform's carried from its region
    where they are published only there, less the declared share of their largest overlap with registered short-stay nights (1, the
    default: nothing counted twice, a lower bound; 0: an upper bound). Keys DOM, FOR and total (ADR 0164)."""
    a = ask("visitors.stock", places=[place(p, year)], readings=[{"name": "paid nights", "sources": ["registered", "platform"], "overlap": overlap}])
    r = a["places"][0]["readings"][0]
    out = {"DOM": 0.0, "FOR": 0.0}
    for M in r["months"]:
        for o, v in (M.get("by_origin") or {}).items():
            out[o] = out.get(o, 0.0) + v * M["days"]
    out["total"] = r["nights"]
    return out


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


def two_table(T: dict, geo: str, quotient: str = "flq", delta: float = 0.3) -> dict:
    """The region and the rest of its country as one table of 2n products ("r:" and "s:" before each code), ADR 0160."""
    E = employment(geo, T["year"])
    W = ask("regional.two", table=T, units=1e-6, shares=E["shares"], size=E["size"], quotient=quotient, delta=delta)
    labels = dict(zip(T["products"], T["labels"]))
    return {**W, "geo": T["geo"], "region": geo, "year": T["year"], "labels": [labels.get(c[2:], c) for c in W["products"]],
            "credit": f"{T['credit']}; {E['credit']}", "employment": E}


# ---- jobs (ADR 0161): persons employed a year per million euro of each product's output ------------------------------------------

def with_jobs(T: dict, geo: str) -> dict:
    """The country's table with each product's persons employed a year per million euro of output: its group's national employment
    over the group's national output (a group's products share one coefficient). From the data library's regional employment."""
    E = employment(geo, T["year"])
    out = dict(zip(T["products"], T["output"]))
    group_output = {}
    for p, g in E["group_of"].items():
        group_output[g] = group_output.get(g, 0.0) + (out.get(p) or 0.0)
    jobs = [1000 * E["groups"][E["group_of"][p]][1] / group_output[E["group_of"][p]] if group_output.get(E["group_of"][p]) else 0.0 for p in T["products"]]
    return {**T, "jobs": jobs, "jobs_credit": E["credit"]}


# ---- short-term rentals (ADR 0163): science/rentals.hpp, answered by the same engine ----------------------------------------------

RENTALS = ROOT / "data" / "cache" / "products" / "platform-rentals"


def listings(city: str = "prague", date: str = "2026-09-20") -> dict:
    """A city's short-term rental listings, as the data library made them from Inside Airbnb's snapshot (CC BY 4.0)."""
    path = RENTALS / f"{city}-{date}.json"
    if not path.is_file():
        raise LookupError(f"no listings for {city} on {date}: python data/products/platform_rentals.py --city {city} --date {date}")
    return json.loads(path.read_text(encoding="utf-8"))
