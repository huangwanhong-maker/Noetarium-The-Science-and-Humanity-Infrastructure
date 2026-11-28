"""The instruments a society writing a report may use (the design record of 2026-11-26, a book written by communities, Sections 3–4).

The **shelf** is the data library's (``data/products/sdg_shelf.py``): the World Bank's catalogue of World Development Indicators, the
economies with their regions and income groups, any indicator's values fetched on first use, and the goals and targets of the 2030
Agenda. The **bench** is the API's: each tool runs one of ``science.progress``'s operations (nomos's trend to target, the spread among
countries and its decomposition between regions, convergence, breaks, links) or reads the shelf (levels, coverage), and answers a
result: the tool and its parameters, the digests of the series it read, a short summary for a reader, **facts** (named numbers a
writer may cite and an auditor check), and a figure's data. Tools the society admits later are pure functions in a restricted
language (``Restricted``), run in a separate process with a time limit, and answer the same shape.

Nothing here decides what is analysed: an analyst's plan names the tools and their parameters.
"""

from __future__ import annotations

import hashlib
import json
import math
import sys
from functools import lru_cache
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "data" / "products"))
import sdg_shelf as shelf  # noqa: E402

from science import progress  # noqa: E402

from .tools import Restricted as _Restricted  # noqa: E402

BASE_YEAR, TARGET_YEAR = 2015, 2030
WORLD = "WLD"
REGIONS = ("EAS", "ECS", "LCN", "MEA", "NAC", "SAS", "SSF")
INCOMES = ("LIC", "LMC", "UMC", "HIC")
AGGREGATES = (WORLD,) + REGIONS + INCOMES


@lru_cache(maxsize=1)
def economies() -> dict:
    return shelf.economies()


@lru_cache(maxsize=1)
def catalogue() -> dict:
    return {c["code"]: c for c in shelf.catalogue()}


@lru_cache(maxsize=1)
def framework() -> list:
    return shelf.framework()


def target_text(code: str) -> str:
    for g in framework():
        for t in g["targets"]:
            if t["target"] == code:
                return t["text"]
    return ""


def name(unit: str) -> str:
    e = economies().get(unit)
    return e["name"] if e else unit


def countries() -> dict[str, str]:
    """Each economy that is not an aggregate, with the code of its region."""
    return {k: v["region_code"] for k, v in economies().items() if not v["aggregate"]}


_SERIES: dict[str, dict] = {}


def series(code: str) -> dict:
    """An indicator's values (fetched by the data library on first use) and its digest."""
    if code not in _SERIES:
        _SERIES[code] = shelf.series(code)
    return _SERIES[code]


def described(code: str) -> dict:
    c = catalogue().get(code)
    if c is None:
        return {"code": code, "known": False}
    return {"code": code, "name": c["name"], "organisation": c["organisation"], "topics": c["topics"], "note": c["note"][:400]}


def candidates(words: list[str], topics: list[str] | None = None, limit: int = 120) -> list[dict]:
    """Indicators of the catalogue whose name or note holds any of the words (or that carry one of the topics), the most matches first."""
    ws = [w.lower() for w in words if w and len(w) > 2]
    scored = []
    for c in catalogue().values():
        text = (c["name"] + " " + c["note"][:300]).lower()
        hits = sum(text.count(w) for w in ws) + (3 if topics and set(topics) & set(c["topics"]) else 0)
        if hits:
            scored.append((-hits, c["code"], c))
    scored.sort()
    return [{"code": c["code"], "name": c["name"]} for _, _, c in scored[:limit]]


def coverage_of(code: str) -> dict:
    """How many countries report the series, by year, and its latest year."""
    rows = series(code)["rows"]
    cs = countries()
    by_year: dict[int, int] = {}
    for r in rows:
        if r["unit"] in cs:
            by_year[r["year"]] = by_year.get(r["year"], 0) + 1
    aggregates = sorted({r["unit"] for r in rows if r["unit"] in AGGREGATES})
    return {"by year": dict(sorted(by_year.items())), "aggregates": aggregates,
            "latest year": max((r["year"] for r in rows), default=None), "countries": len({r["unit"] for r in rows if r["unit"] in cs})}


def _rows(code: str, units) -> list[dict]:
    keep = set(units)
    return [r for r in series(code)["rows"] if r["unit"] in keep]


def _population() -> dict[str, float]:
    rows = series("SP.POP.TOTL")["rows"]
    latest: dict[str, tuple[int, float]] = {}
    for r in rows:
        if r["unit"] not in latest or r["year"] > latest[r["unit"]][0]:
            latest[r["unit"]] = (r["year"], r["value"])
    return {k: v for k, (_, v) in latest.items()}


def _lines(code: str, units, start: int = 2000) -> dict:
    by: dict[str, dict[int, float]] = {}
    for r in _rows(code, units):
        if r["year"] >= start:
            by.setdefault(name(r["unit"]), {})[r["year"]] = r["value"]
    return {k: sorted(v.items()) for k, v in by.items()}


def _num(x, digits: int = 4):
    if x is None or (isinstance(x, float) and not math.isfinite(x)):
        return None
    return round(float(x), digits) if abs(float(x)) < 1e6 else round(float(x))


# ---- the tools ----------------------------------------------------------------------------------------------------------------------
TOOLS = {
    "levels": {"about": "a series' values for the world, its seven regions and four income groups at the base year (2015, else the earliest after) and the latest year, with the countries reporting",
               "params": {"series": "an indicator's code"}},
    "coverage": {"about": "how many countries report a series in each year, and its latest year", "params": {"series": "an indicator's code"}},
    "trend": {"about": "the trend to target after the United Nations Statistics Division's method (compound rate from 2015, the value expected in 2030, the share of the distance to the target it covers, five classes), for the aggregates or for every country, with the count of countries in each class",
              "params": {"series": "code", "desired": "increase or decrease", "target": "the 2030 target value, or null for a series without one (then classed by its rate)",
                         "target_reading": "where the target comes from (a target's text, a reading of it)", "units": "aggregates or countries"}},
    "spread": {"about": "the spread of a series among countries by year (Gini, Theil's T, coefficient of variation), population-weighted, and Theil's T split between and within the seven regions",
               "params": {"series": "code", "years": "a list of years (optional)"}},
    "convergence": {"about": "whether countries further behind moved faster: the annual change regressed on the initial level across countries (β, its p-value, R²) and the coefficient of variation at both years",
                    "params": {"series": "code", "from": "a year", "to": "a year", "log": "true to use logarithms"}},
    "breaks": {"about": "breaks in the annual changes of the world's (or a region's) series by PELT, with each segment's mean change", "params": {"series": "code", "unit": "WLD or a region's code"}},
    "links": {"about": "whether two series moved together across countries: rank correlations (Spearman, Kendall) of their changes between two years and of their later levels",
              "params": {"series": "code", "other": "code", "from": "a year", "to": "a year"}},
}


def run(tool: str, params: dict, admitted: dict | None = None) -> dict:
    """Runs a tool of the bench (or one the society admitted) and answers a result, or {"refused": reason}."""
    try:
        if admitted and tool in admitted:
            return Restricted.run(admitted[tool], params)
        if tool not in TOOLS:
            return {"refused": f"no tool {tool!r} on the bench"}
        code = str(params.get("series", ""))
        if code not in catalogue():
            return {"refused": f"{code!r} is not an indicator on the shelf"}
        return globals()["_" + tool](code, params)
    except (ValueError, KeyError, TypeError, progress.EngineError, RuntimeError) as why:
        return {"refused": f"{type(why).__name__}: {str(why)[:300]}"}


def _inputs(*codes) -> list[dict]:
    return [{"code": c, "sha256": series(c)["sha256"], "retrieved": series(c)["retrieved"]} for c in codes]


def _levels(code: str, params: dict) -> dict:
    rows = _rows(code, AGGREGATES)
    by: dict[str, dict[int, float]] = {}
    for r in rows:
        by.setdefault(r["unit"], {})[r["year"]] = r["value"]
    facts, lines = {}, []
    for u in AGGREGATES:
        ys = by.get(u, {})
        if not ys:
            continue
        after = sorted(y for y in ys if y >= BASE_YEAR)
        b = after[0] if after else min(ys)
        t = max(ys)
        facts[f"{name(u)} {b}"] = _num(ys[b])
        facts[f"{name(u)} {t}"] = _num(ys[t])
        lines.append(f"{name(u)}: {_num(ys[b])} ({b}) -> {_num(ys[t])} ({t})")
    cov = coverage_of(code)
    last = max(cov["by year"]) if cov["by year"] else None
    if last:
        facts[f"countries reporting {last}"] = cov["by year"][last]
    return {"tool": "levels", "params": {"series": code}, "inputs": _inputs(code), "facts": facts,
            "summary": f"{described(code)['name']}\n" + "\n".join(lines) + (f"\ncountries reporting in {last}: {cov['by year'][last]}" if last else ""),
            "figure": {"kind": "lines", "title": described(code)["name"], "series": _lines(code, AGGREGATES[:8])}}


def _coverage(code: str, params: dict) -> dict:
    cov = coverage_of(code)
    facts = {f"countries reporting {y}": n for y, n in cov["by year"].items()}
    facts["countries reporting in any year"] = cov["countries"]
    return {"tool": "coverage", "params": {"series": code}, "inputs": _inputs(code), "facts": facts,
            "summary": f"{described(code)['name']}: countries reporting by year " + ", ".join(f"{y}: {n}" for y, n in cov["by year"].items()) +
                       f"; aggregates: {', '.join(cov['aggregates']) or 'none'}", "figure": None}


def _trend(code: str, params: dict) -> dict:
    desired = str(params.get("desired", "increase"))
    target = params.get("target")
    target = float(target) if isinstance(target, (int, float)) else None
    units = list(AGGREGATES) if params.get("units", "aggregates") != "countries" else list(countries())
    weights = _population() if units != list(AGGREGATES) else None
    r = progress.trend(_rows(code, units), desired, target=target, weights={u: weights[u] for u in units if weights and u in weights} if weights else None)
    facts, lines = {}, []
    for u in r["units"]:
        n = name(u["unit"])
        if "not assessed" in u:
            continue
        if units == list(AGGREGATES):
            facts[f"{n} trend"] = u["trend"]
            facts[f"{n} rate"] = _num(u["rate"], 5)
            if u.get("ratio") is not None:
                facts[f"{n} ratio"] = _num(u["ratio"], 3)
            if u.get("expected") is not None:
                facts[f"{n} expected {TARGET_YEAR}"] = _num(u["expected"])
            lines.append(f"{n}: {u['baseline']:.4g} ({u['baseline_year']:.0f}) -> {u['latest']:.4g} ({u['latest_year']:.0f}); rate {u['rate']:.4f}; "
                         + (f"expected {u['expected']:.4g} in {TARGET_YEAR}; ratio {u['ratio']:.3f}; " if u.get("ratio") is not None else "") + u["trend"])
    for c in r["classes"]:
        if units != list(AGGREGATES):
            facts[f"countries {c['trend']}"] = c["units"]
            if c.get("weighted share") is not None:
                facts[f"population share {c['trend']}"] = _num(c["weighted share"], 4)
    assessed = sum(c["units"] for c in r["classes"])
    if units != list(AGGREGATES):
        facts["countries assessed"] = assessed
        lines.append("countries by class: " + "; ".join(f"{c['trend']}: {c['units']}" + (f" ({100 * c['weighted share']:.1f}% of their population)" if c.get("weighted share") is not None else "") for c in r["classes"]))
    return {"tool": "trend", "params": {"series": code, "desired": desired, "target": target, "target_reading": str(params.get("target_reading", ""))[:300],
                                        "units": "countries" if units != list(AGGREGATES) else "aggregates"},
            "inputs": _inputs(code, *(["SP.POP.TOTL"] if weights else [])), "facts": facts,
            "summary": f"Trend to target of {described(code)['name']} (target {target if target is not None else 'none'} by {TARGET_YEAR}, {desired}):\n" + "\n".join(lines) +
                       "\nHow to read it: the rate is the compound annual change from the baseline (2015, or the first year after); the expected value "
                       "is where the series would be in 2030 at that rate; the ratio is the share of the distance from the baseline to the target "
                       "that the trend would cover by 2030, classed by the United Nations Statistics Division's floors (0.95 on track, 0.5 moderate, "
                       "0.1 marginal, 0 stagnation, below 0 regression). Without a target the class follows the rate alone. Population shares are "
                       "of the countries assessed.",
            "figure": {"kind": "lines", "title": described(code)["name"], "series": _lines(code, AGGREGATES[:8]), "target": target}}


def _spread(code: str, params: dict) -> dict:
    cs = countries()
    years = [int(y) for y in params.get("years") or []] or None
    r = progress.spread(_rows(code, cs), groups={k: v for k, v in cs.items()}, weights=_population(), years=years)
    facts, lines, fig = {}, [], {}
    for y in r["years"]:
        if y["n"] < 20:
            continue
        yr = int(y["year"])
        for k in ("gini", "theil_t", "cv"):
            if y.get(k) is not None:
                facts[f"{k} {yr}"] = _num(y[k], 4)
                fig.setdefault(k, []).append((yr, y[k]))
        if y.get("between share") is not None:
            facts[f"between-region share of Theil {yr}"] = _num(y["between share"], 4)
            fig.setdefault("between share", []).append((yr, y["between share"]))
        facts[f"countries {yr}"] = int(y["n"])
        lines.append(f"{yr}: n {int(y['n'])}, Gini {y.get('gini') or float('nan'):.3f}, Theil {y.get('theil_t') or float('nan'):.3f}, "
                     f"between regions {100 * (y.get('between share') or float('nan')):.1f}%")
    return {"tool": "spread", "params": {"series": code, "years": years}, "inputs": _inputs(code, "SP.POP.TOTL"), "facts": facts,
            "summary": f"Spread of {described(code)['name']} among countries, population-weighted:\n" + "\n".join(lines[-12:]) +
                       "\nHow to read it: Gini and Theil's T measure inequality of the indicator's values among countries, weighted by population "
                       "(0 is equality); the between-region share is the part of Theil's T due to differences between the regions' means, the rest "
                       "lying within regions. They say nothing about how many countries hold most of a total.",
            "figure": {"kind": "lines", "title": f"Spread of {described(code)['name']}", "series": fig}}


def _convergence(code: str, params: dict) -> dict:
    a, b = int(params.get("from", BASE_YEAR)), int(params.get("to", 2022))
    r = progress.convergence(_rows(code, countries()), a, b, log=bool(params.get("log", False)))
    facts = {"beta": _num(r["beta"], 6), "beta p-value": _num(r["p"], 4), "r squared": _num(r["r_squared"], 4), "countries": int(r["n"]),
             f"cv {a}": _num(r["cv_from"], 4), f"cv {b}": _num(r["cv_to"], 4)}
    return {"tool": "convergence", "params": {"series": code, "from": a, "to": b, "log": bool(params.get("log", False))}, "inputs": _inputs(code), "facts": facts,
            "summary": f"Convergence of {described(code)['name']} {a}-{b} across {int(r['n'])} countries: beta {r['beta']:.5f} (p {r['p']:.3g}), R2 {r['r_squared']:.3f}; "
                       f"CV {r['cv_from']:.3f} -> {r['cv_to']:.3f}. How to read it: beta is the slope of each country's annual change on its {a} level; "
                       f"a negative beta means countries that started higher changed by less each year (rose less, or fell more), so levels drew "
                       f"together; a positive beta that they drew apart; with p above 0.05 the slope is not distinguished from zero. The coefficient "
                       f"of variation falling from {a} to {b} means the spread narrowed relative to the mean.", "figure": None}


def _breaks(code: str, params: dict) -> dict:
    u = str(params.get("unit", WORLD))
    r = progress.breaks(_rows(code, [u]), unit=u)
    facts = {f"break {i + 1}": int(y) for i, y in enumerate(r["breaks"])}
    for i, sgm in enumerate(r["segments"]):
        facts[f"mean annual change {int(sgm['from'])}-{int(sgm['to'])}"] = _num(sgm["mean change"], 5)
    return {"tool": "breaks", "params": {"series": code, "unit": u}, "inputs": _inputs(code), "facts": facts,
            "summary": f"Breaks in the annual change of {described(code)['name']} for {name(u)}: " + (", ".join(str(int(y)) for y in r["breaks"]) or "none") + "; segments: " +
                       "; ".join(f"{int(s['from'])}-{int(s['to'])} mean change {s['mean change']:.4g}" for s in r["segments"]),
            "figure": {"kind": "lines", "title": f"{described(code)['name']}, {name(u)}", "series": _lines(code, [u])}}


def _links(code: str, params: dict) -> dict:
    other = str(params.get("other", ""))
    if other not in catalogue():
        raise ValueError(f"{other!r} is not an indicator on the shelf")
    a, b = int(params.get("from", BASE_YEAR)), int(params.get("to", 2022))
    cs = countries()
    r = progress.links(_rows(code, cs), _rows(other, cs), a, b)
    facts = {k: _num(r[k], 4) for k in ("spearman_change", "kendall_change", "spearman_level", "kendall_level")} | {"countries": int(r["n"])}
    return {"tool": "links", "params": {"series": code, "other": other, "from": a, "to": b}, "inputs": _inputs(code, other), "facts": facts,
            "summary": f"Links between {described(code)['name']} and {described(other)['name']} across {int(r['n'])} countries, {a}-{b}: "
                       f"Spearman of changes {r['spearman_change']:.3f}, of {b} levels {r['spearman_level']:.3f}. How to read it: rank correlations "
                       f"from -1 to 1 of the two series' changes and of their later levels across the same countries; an association, not a cause.",
            "figure": None}


def digest(result: dict) -> str:
    keep = {k: result.get(k) for k in ("tool", "params", "inputs", "facts")}
    return hashlib.sha256(json.dumps(keep, sort_keys=True, ensure_ascii=False).encode("utf-8")).hexdigest()[:16]


# ---- tools admitted by the society: pure functions in a restricted language ---------------------------------------------------------
class Restricted(_Restricted):
    """A tool written by an agent and admitted by the society's rules, in the restricted language of ``tools.Restricted`` (moved there
    for the node's host services, Phase 10), given ``data`` = {code: {unit: {year: value}}} for the series its parameters name, with
    ``data["regions"]`` (each country's region) and ``data["names"]``, and answering {"facts": {name: number or text}, "summary": text}."""

    @classmethod
    def run(cls, tool: dict, params: dict, timeout: float = 30.0) -> dict:
        why = cls.faults(tool["code"])
        if why:
            return {"refused": "the tool's code is not admissible: " + "; ".join(why)}
        codes = [str(params[k]) for k in tool.get("series params", ["series"]) if params.get(k)]
        for c in codes:
            if c not in catalogue():
                return {"refused": f"{c!r} is not an indicator on the shelf"}
        data = {}
        for c in codes:
            by: dict[str, dict[str, float]] = {}
            for r in series(c)["rows"]:
                by.setdefault(r["unit"], {})[str(r["year"])] = r["value"]
            data[c] = by
        out = cls.execute(tool["code"], data, params, timeout=timeout, shelf=True, regions=countries(),
                          names={k: v["name"] for k, v in economies().items()})
        if "refused" in out:
            return out
        facts = {str(k)[:120]: (_num(v) if isinstance(v, (int, float)) else str(v)[:120]) for k, v in list(out["facts"].items())[:80]}
        return {"tool": tool["name"], "params": params, "inputs": _inputs(*codes), "facts": facts, "summary": str(out.get("summary", ""))[:2000], "figure": None,
                "admitted tool": tool["name"]}
