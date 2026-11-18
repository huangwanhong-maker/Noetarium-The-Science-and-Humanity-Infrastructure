"""What the tools of cities across scales share: a city's units across scales from the data library, the cells the census withheld
estimated by the engine (tyche's dasymetric carriage, never here), the indexes of the urban-science domain's catalogue computed at every
level by the model's own operators, and the models the plug-in offers, written as documents for science.scales. Nothing of the
computation is done here: this module only lays data out as the engine reads it and reads its answers back."""

from __future__ import annotations

import csv
import hashlib
import json
import math
from pathlib import Path

from science import scales

ROOT = Path(__file__).resolve().parents[5]
PRODUCTS = ROOT / "data" / "cache" / "products" / "city-scales"
KEPT = ROOT / "data" / "cache" / "urban-studio" / "scales"
CATALOGUE = ROOT / "domains" / "urban-science" / "indicators" / "catalogue.tsv"

USES = ["homes", "office", "retail", "hotels", "education", "health", "culture", "community", "workshops", "parking", "other"]
VACANT = ["unused", "renovation", "construction", "demolition", "undeveloped"]
WALK = {"speed": 80.0, "detour": 1.3}  # metres a minute, and the ratio of a walk to the straight line: declared, not measured


def num(x):
    return None if x is None or (isinstance(x, float) and not math.isfinite(x)) else x


def pct(x) -> str:
    return "—" if x is None else f"{100 * x:.1f}%"


def cases() -> list[dict]:
    index = PRODUCTS / "index.json"
    return json.loads(index.read_text(encoding="utf-8")).get("cases", []) if index.is_file() else []


def product(case: str = "melbourne") -> dict:
    path = PRODUCTS / f"{case}.json"
    if not path.is_file():
        raise LookupError(f"the data library has not made {case}: python data/products/city_scales.py")
    p = json.loads(path.read_text(encoding="utf-8"))
    if p.get("schema") != "noetarium.city-scales/1":
        raise ValueError(f"{path} is not a noetarium.city-scales/1")
    return p


def catalogue() -> dict[str, dict]:
    with open(CATALOGUE, encoding="utf-8") as f:
        return {r["id"]: r for r in csv.DictReader(f, delimiter="\t")}


# ---- what the census withheld ---------------------------------------------------------------------------------------------------

def withheld(p: dict, case: str = "melbourne") -> dict:
    """Every cell as stated, or estimated where the census left it blank: each row's withheld floor shared among its blank cells by a
    dasymetric carriage on their prior weights (the engine's `carry`), kept while the product does not change. Returns
    {year: {"value": [[per column] per block], "lower": ..., "upper": ...}} with None for blocks not in that year's census."""
    stamp = hashlib.sha256((PRODUCTS / f"{case}.json").read_bytes()).hexdigest()[:16]
    cache = KEPT / f"{case}-withheld-{stamp}.json"
    if cache.is_file():
        return json.loads(cache.read_text(encoding="utf-8"))
    out = {}
    cols = p["columns"]
    for year, y in p["years_of"].items():
        sources, values, parts, where = [], [], [], []
        for b, row in enumerate(y["stated"]):
            if row is None or not y["withheld"][b]:
                continue
            s = len(sources)
            sources.append(b)
            values.append(y["withheld"][b])
            for k, v in enumerate(row):
                if v is None:
                    parts.append({"source": s, "target": len(where), "measure": 1.0, "ancillary": float(y["prior"][b][k] or 0.0)})
                    where.append((b, k))
        carried = scales.ask("carry", method="dasymetric", values=values, parts=parts, targets=len(where)) if where else {"values": []}
        value, lower, upper = [], [], []
        for b, row in enumerate(y["stated"]):
            if row is None:
                value.append(None), lower.append(None), upper.append(None)
                continue
            value.append([v if v is not None else 0.0 for v in row])
            lower.append([v if v is not None else 0.0 for v in row])
            upper.append([v if v is not None else float(y["withheld"][b] or 0.0) for v in row])
        for (b, k), v in zip(where, carried["values"]):
            value[b][k] = v
        out[year] = {"value": value, "lower": lower, "upper": upper}
    KEPT.mkdir(parents=True, exist_ok=True)
    cache.write_text(json.dumps(out, separators=(",", ":")), encoding="utf-8")
    return out


def grouped(p: dict, est: dict) -> dict:
    """The floor space of each use (a group of CLUE's columns) by block and year: {use: {year: (value, lower, upper) per block}}.
    A use's lower bound is what is stated of it; its upper bound adds the row's withheld floor when one of its cells is blank."""
    idx = {c: k for k, c in enumerate(p["columns"])}
    out = {}
    for use, cols in p["groups"].items():
        ks = [idx[c] for c in cols if c in idx]
        per = {}
        for year, e in est.items():
            y = p["years_of"][year]
            v, lo, hi = [], [], []
            for b, row in enumerate(e["value"]):
                if row is None:
                    v.append(None), lo.append(None), hi.append(None)
                    continue
                stated = y["stated"][b]
                v.append(sum(row[k] for k in ks))
                lo.append(sum(stated[k] or 0.0 for k in ks))
                blank = any(stated[k] is None for k in ks)
                hi.append(lo[-1] + (float(y["withheld"][b] or 0.0) if blank else 0.0))
            per[year] = (v, lo, hi)
        out[use] = per
    return out


# ---- the hierarchy ----------------------------------------------------------------------------------------------------------------

def hierarchy(p: dict, costs: bool = False) -> dict:
    levels = []
    for lv in p["levels"]:
        out = {"name": lv["name"], "units": lv["units"]}
        if "parent" in lv:
            out["parent"] = lv["parent"]
        if lv.get("measures"):
            out["measures"] = lv["measures"]
        if lv["name"] == p["levels"][0]["name"]:
            out["edges"] = p["edges"]
            if costs:
                out["costs"] = {"walk": {"points": lv["xy"], **WALK}}
        levels.append(out)
    return {"levels": levels}


def series(per_year: dict, years: list[int], fill=None) -> dict:
    """A driver's series from values by year and block; a block not in a year's census takes `fill` (or its nearest year's value)."""
    at = []
    for y in years:
        row = per_year[str(y)]
        at.append([v if v is not None else fill for v in row])
    if fill is None:
        n = len(at[0])
        for u in range(n):
            known = [(k, at[k][u]) for k in range(len(years)) if at[k][u] is not None]
            for k in range(len(years)):
                if at[k][u] is None:
                    at[k][u] = min(known, key=lambda kv: abs(kv[0] - k))[1] if known else 0.0
    return {"times": years, "at": at}


# ---- the indexes, at every level --------------------------------------------------------------------------------------------------

BASE = USES + VACANT + ["total", "establishments", "dwellings"]

# Each index: its catalogue id (empty when the atlas does not name it), what it is, and its expression on a level's summed quantities.
INDEXES = [
    ("commercial_vacancy_rate", "floor vacancy rate", "unused / total", "the share of floor space unoccupied and unused (a ratio of sums)"),
    ("", "share under construction", "construction / total", "floor space being built, a share of all"),
    ("floor_area_ratio", "floor area ratio", "total / land", "floor space over the land it stands on"),
    ("net_residential_density", "dwellings a hectare", "dwellings / (land / 10000)", "dwellings over the land, in hectares"),
    ("", "businesses a hectare", "establishments / (land / 10000)", "establishments counted over the land, in hectares"),
    ("", "homes' share of occupied floor", "homes / occupied", "residential floor over all occupied floor"),
    ("", "offices' share of occupied floor", "office / occupied", "office floor over all occupied floor"),
    ("", "shops' share of occupied floor", "retail / occupied", "retail floor over all occupied floor"),
    ("land_use_mix_entropy", "land-use mix", "-(" + " + ".join(f"({u} / occupied) * log(({u} + 0.000000001) / occupied)" for u in USES) + f") / log({len(USES)})",
     "the entropy of the occupied floor's uses over its largest value: 0 for one use, 1 for all alike"),
    ("herfindahl_hirschman_index", "concentration of uses", " + ".join(f"({u} / occupied)^2" for u in USES),
     "the sum of the squared shares of the uses: 1 for one use"),
]
AMONG = [  # indexes of the units beneath, recomputed from them (never averaged)
    ("dissimilarity_index", "dissimilarity of homes and offices", "dissimilarity", "homes", "office",
     "½ Σ |homes_i/H − office_i/O| over the blocks beneath: the share of either that would have to move for both to be spread alike"),
    ("gini_coefficient", "inequality of floor area ratio", "gini", "total / land", "land", "Gini's coefficient of the blocks' floor area ratio, weighted by land"),
]


def index_model(p: dict) -> dict:
    """A model with no law: the base quantities as drivers at the finest level, summed up the hierarchy, and every index as a value on
    each level's sums, so that a ratio is a ratio of sums at every level; indexes among the units beneath are reduced from them."""
    names = [lv["name"] for lv in p["levels"]]
    fine = names[0]
    q = [{"name": b, "level": fine, "kind": "extensive", "role": "driver", "unit": "m²" if b in USES + VACANT + ["total"] else "count"} for b in BASE]
    q.append({"name": "land", "level": fine, "kind": "extensive", "role": "fixed", "unit": "m²"})
    derived = []
    for li, lv in enumerate(names):
        tag = "" if li == 0 else f"_{lv}"
        if li > 0:
            below = "" if li == 1 else f"_{names[li - 1]}"
            for b in BASE + ["land"]:
                derived.append({"name": f"{b}{tag}", "level": lv, "op": "sum", "of": f"{b}{below}", "kind": "extensive"})
        occ = f"occupied{tag}"
        derived.append({"name": occ, "level": lv, "op": "value", "of": f"total{tag} - (" + " + ".join(f"{v}{tag}" for v in VACANT) + ")", "kind": "extensive"})
        for k, (_, label, expr, _) in enumerate(INDEXES):
            e = expr
            for b in sorted(BASE + ["land", "occupied"], key=len, reverse=True):
                e = _rename(e, b, f"{b}{tag}")
            derived.append({"name": f"i{k}{tag}", "level": lv, "op": "value", "of": e, "kind": "intensive"})
        if li > 0:
            below = "" if li == 1 else f"_{names[li - 1]}"
            for k, (_, label, reducer, of, weight, _) in enumerate(AMONG):
                derived.append({"name": f"a{k}{tag}", "level": lv, "op": "reduce", "reducer": reducer,
                                "of": _rename_all(of, below), "weight": _rename_all(weight, below), "kind": "other"})
    return {"step": 1, "quantities": q, "derived": derived}


def _rename(expr: str, name: str, to: str) -> str:
    import re
    return re.sub(rf"(?<![A-Za-z0-9_]){name}(?![A-Za-z0-9_])", to, expr)


def _rename_all(expr: str, suffix: str) -> str:
    for b in sorted(BASE + ["land", "occupied"], key=len, reverse=True):
        expr = _rename(expr, b, f"{b}{suffix}")
    return expr


def index_document(p: dict, uses: dict, years: list[int]) -> dict:
    fine = p["levels"][0]
    ser = {}
    for b in USES + VACANT:
        ser[b] = series({y: uses[b][y][0] for y in uses[b]}, years, fill=0.0)
    ser["total"] = series({y: p["years_of"][y]["total"] for y in p["years_of"]}, years, fill=0.0)
    ser["establishments"] = series({y: p["years_of"][y]["establishments"] for y in p["years_of"]}, years, fill=0.0)
    ser["dwellings"] = series({y: p["years_of"][y]["dwellings"] for y in p["years_of"]}, years, fill=0.0)
    return {"hierarchy": hierarchy(p), "model": index_model(p),
            "inputs": {"values": {"land": fine["measures"]["area_m2"]}, "series": ser},
            "run": {"t0": years[0], "t1": years[-1] if years[-1] > years[0] else years[0] + 1}}


# ---- the models ---------------------------------------------------------------------------------------------------------------------

def growth_model(use: str) -> dict:
    """Where growth goes: next year's change of the city's floor space of a use is given (a driver), shared among the areas by a logit
    on their floor of the use, and within each area among its blocks by a logit on their own floor, their neighbours' and their
    distance from the centre. With α = β = 1 and γ = δ = 0 every unit keeps its share: proportional allocation, nested in the model."""
    return {
        "step": 1,
        "quantities": [
            {"name": "h", "level": "block", "kind": "extensive", "role": "state", "unit": f"m² of {use}", "lower": 0},
            {"name": "dist", "level": "block", "kind": "other", "role": "fixed", "unit": "km from the centre"},
            {"name": "inc", "level": "city", "kind": "extensive", "role": "driver", "unit": "m² a year"},
        ],
        "derived": [
            {"name": "H", "level": "area", "op": "sum", "of": "h", "kind": "extensive"},
            {"name": "Hc", "level": "city", "op": "sum", "of": "H", "kind": "extensive"},
            {"name": "share_a", "level": "area", "op": "logit", "of": "alpha * log(H + 1)"},
            {"name": "inc_a", "level": "area", "op": "parent", "of": "inc"},
            {"name": "d_a", "level": "area", "op": "value", "of": "share_a * inc_a", "kind": "extensive"},
            {"name": "nb", "level": "block", "op": "neighbours", "of": "h"},
            {"name": "share_b", "level": "block", "op": "logit", "of": "beta * log(h + 1) + gamma * log(nb + 1) - delta * dist"},
            {"name": "d_b", "level": "block", "op": "parent", "of": "d_a"},
        ],
        "laws": [{"target": "h", "form": "map", "expression": "h + share_b * d_b"}],
        "parameters": [
            {"name": "alpha", "value": 1, "lower": 0, "upper": 3, "fit": True, "unit": "how growth among areas follows their size"},
            {"name": "beta", "value": 1, "lower": 0, "upper": 3, "fit": True, "unit": "how growth among blocks follows their own floor"},
            {"name": "gamma", "value": 0, "lower": -2, "upper": 2, "fit": True, "unit": "pull of the neighbours' floor"},
            {"name": "delta", "value": 0, "lower": -2, "upper": 2, "fit": True, "unit": "push of distance from the centre, per km"},
        ],
    }


def vacancy_model() -> dict:
    """The vacancy cycle across scales, after DiPasquale and Wheaton: the city's occupied floor is given (a driver) and shared among the
    areas in proportion to their stock to a power γ; an area's gap is what its stock does not hold (negative when demand exceeds it),
    its vacancy that gap where it is positive; its rent relative to the cost of building (ρ, which no record here holds) falls while the
    gap exceeds the natural vacancy rate and rises while it is below; its stock is built while ρ exceeds one and is not unbuilt (the
    record's twenty years are too short for depreciation to tell). Each block reads its area's vacancy."""
    return {
        "step": 1,
        "quantities": [
            {"name": "S", "level": "area", "kind": "extensive", "role": "state", "unit": "m² of floor space", "lower": 1},
            {"name": "rho", "level": "area", "kind": "other", "role": "state", "unit": "rent over the rent that pays for building", "lower": 0.01},
            {"name": "D", "level": "city", "kind": "extensive", "role": "driver", "unit": "m² occupied"},
        ],
        "derived": [
            {"name": "Dc", "level": "area", "op": "parent", "of": "D"},
            {"name": "sh", "level": "area", "op": "share", "of": "S^gamma"},
            {"name": "O", "level": "area", "op": "value", "of": "sh * Dc", "kind": "extensive"},
            {"name": "gap", "level": "area", "op": "value", "of": "1 - O / S", "kind": "intensive"},
            {"name": "v", "level": "area", "op": "value", "of": "max(gap, 0)", "kind": "intensive"},
            {"name": "Sc", "level": "city", "op": "sum", "of": "S", "kind": "extensive"},
            {"name": "Oc", "level": "city", "op": "sum", "of": "O", "kind": "extensive"},
            {"name": "vc", "level": "city", "op": "value", "of": "max(1 - Oc / Sc, 0)", "kind": "intensive"},
            {"name": "vb", "level": "block", "op": "parent", "of": "v", "kind": "intensive"},
        ],
        "laws": [{"target": "rho", "form": "flow", "expression": "-lambda * (gap - vstar) * rho"},
                 {"target": "S", "form": "flow", "expression": "kappa * max(rho - 1, 0) * S"}],
        "parameters": [
            {"name": "lambda", "value": 2, "lower": 0, "upper": 30, "fit": True, "unit": "how fast rents answer vacancy, a year"},
            {"name": "kappa", "value": 0.1, "lower": 0, "upper": 2, "fit": True, "unit": "how fast stock answers rents, a year"},
            {"name": "vstar", "value": 0.05, "lower": 0, "upper": 0.3, "fit": True, "unit": "the natural vacancy rate"},
            {"name": "gamma", "value": 1, "lower": 0.3, "upper": 3, "fit": True, "unit": "how demand among areas follows their stock"},
        ],
    }


def _fine_xy(p: dict) -> list:
    return p["levels"][0]["xy"]


def growth_document(p: dict, uses: dict, use: str, t0: int, t1: int) -> tuple[dict, list]:
    """The model of where growth goes, for one use, from t0 to t1, and its observations: every block's floor of the use each year, with
    a σ that adds to a floor of 500 m² and 5% the uncertainty of what the census withheld (a uniform spread over its bounds)."""
    per = uses[use]
    years = [y for y in p["years"] if t0 <= y <= t1]
    v0 = per[str(t0)][0]
    xy = _fine_xy(p)
    dist = [math.hypot(x, y) / 1000.0 for x, y in xy]
    totals = [sum(v for v in per[str(y)][0] if v is not None) for y in years]
    inc = [totals[k + 1] - totals[k] for k in range(len(years) - 1)] + [0.0]
    doc = {"hierarchy": hierarchy(p), "model": growth_model(use),
           "inputs": {"values": {"h": [v if v is not None else 0.0 for v in v0], "dist": dist},
                      "series": {"inc": {"times": years, "at": [[d] for d in inc]}}},
           "run": {"t0": t0, "t1": t1}}
    units = p["levels"][0]["units"]
    obs = []
    for y in years[1:]:
        v, lo, hi = per[str(y)]
        for b, u in enumerate(units):
            if v[b] is None:
                continue
            spread = (hi[b] - lo[b]) / math.sqrt(12)
            obs.append({"quantity": "h", "unit": u, "time": y, "value": v[b], "sigma": math.sqrt((500 + 0.05 * v[b]) ** 2 + spread ** 2)})
    return doc, obs


def vacancy_document(p: dict, uses: dict, t0: int, t1: int, rho0=(0.8, 1.2)) -> tuple[dict, list]:
    """The vacancy cycle from t0 to t1 and its observations: each area's stock and vacancy each year. The rents relative to the cost of
    building are held by no record here, so their starting values are declared, with the range given."""
    years = [y for y in p["years"] if t0 <= y <= t1]
    areas = p["levels"][1]["units"]
    parent = p["levels"][0]["parent"]
    def area_sums(per_block):
        s = {a: 0.0 for a in areas}
        for b, v in enumerate(per_block):
            if v is not None:
                s[parent[b]] += v
        return [s[a] for a in areas]
    stock = {y: area_sums(p["years_of"][str(y)]["total"]) for y in years}
    vacant = {y: area_sums([sum((uses[k][str(y)][0][b] or 0.0) for k in VACANT) if p["years_of"][str(y)]["total"][b] is not None else None
                            for b in range(len(parent))]) for y in years}
    occupied_city = [sum(stock[y]) - sum(vacant[y]) for y in years]
    doc = {"hierarchy": hierarchy(p), "model": vacancy_model(),
           "inputs": {"values": {"S": stock[t0], "rho": [{"source": "declared", "lower": rho0[0], "upper": rho0[1]} for _ in areas]},
                      "series": {"D": {"times": years, "at": [[d] for d in occupied_city]}}},
           "run": {"t0": t0, "t1": t1}}
    obs = []
    for y in years[1:]:
        for i, a in enumerate(areas):
            if stock[y][i] <= 0:
                continue
            obs.append({"quantity": "S", "unit": a, "time": y, "value": stock[y][i], "sigma": 0.02 * stock[y][i]})
            obs.append({"quantity": "v", "unit": a, "time": y, "value": vacant[y][i] / stock[y][i], "sigma": 0.01})
    return doc, obs
