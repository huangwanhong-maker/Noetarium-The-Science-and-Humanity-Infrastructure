"""Checks of progress among units at the API's edge (api/cpp/science/progress.hpp, api/python/science/progress.py; ADR 0172), run by
api/tests/run-tests.mjs. The engine is the API's own, built here where a C++ compiler is found. A quantity halved in five years toward zero
is moderate progress (ρ = 0.875); the classes are counted and weighted by population; Theil's T splits exactly into its parts within and
between groups; countries starting higher and falling faster give a negative β; a series whose annual fall halves has a break; two series
moving together have a rank correlation of one; a unit without a weight is left out of a weighted spread. One JSON object a line.

    python api/tests/check_progress.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import progress  # noqa: E402
from science.engines import toolchain  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "progress: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def rows(series: dict) -> list:
    return [{"unit": u, "year": y, "value": v} for u, ys in series.items() for y, v in ys.items()]


def main() -> int:
    if toolchain() is None:
        emit({"kind": "skip", "name": "progress: the engine (no C++ toolchain here)"})
        return 0
    progress.ENGINE.build()
    r = progress.trend(rows({"A": {2015: 100, 2020: 50}}), "decrease", target=0)
    u = r["units"][0]
    assert_("a quantity halved in five years toward zero: ρ = 0.875, moderate progress", abs(u["ratio"] - 0.875) < 1e-12 and u["level"] == 1, u)
    r = progress.trend(rows({"A": {2015: 10, 2023: 5}, "B": {2015: 10, 2023: 20}, "C": {2015: 10, 2023: 2}}), "decrease", target=4,
                       weights={"A": 1, "B": 3, "C": 4})
    shares = {c["trend"]: c["weighted share"] for c in r["classes"]}
    assert_("the classes are counted and weighted by population", sum(c["units"] for c in r["classes"]) == 3 and abs(shares["regression"] - 3 / 8) < 1e-12, r["classes"])
    sp = progress.spread(rows({u: {2020: v} for u, v in zip("ABCDEF", [1, 2, 3, 4, 5, 6])}), groups={"A": "x", "B": "x", "C": "x", "D": "y", "E": "y", "F": "y"})
    y = sp["years"][0]
    assert_("Theil's T is its parts within and between groups", abs(y["theil_t"] - y["within"] - y["between"]) < 1e-12, y)
    weighted = progress.spread(rows({u: {2020: v} for u, v in zip("ABCD", [1, 2, 3, 40])}), weights={"A": 1, "B": 1, "C": 1})
    assert_("a unit without a weight is left out of a weighted spread", weighted["years"] == [] or weighted["years"][0]["n"] == 3, weighted)
    cv = progress.convergence(rows({u: {2015: a, 2023: a - 0.5 * a} for u, a in zip("ABCDEF", [10, 20, 30, 40, 50, 60])}), 2015, 2023)
    assert_("countries starting higher that fall faster give a negative β", cv["beta"] < 0 and cv["n"] == 6, cv)
    level, series = 100.0, {}
    for k, yr in enumerate(range(2000, 2021)):
        series[yr] = level
        level -= 4.0 if yr < 2010 else 2.0
    br = progress.breaks(rows({"W": series}), unit="W")
    assert_("a series whose annual fall halves has a break near it", any(2009 <= b <= 2012 for b in br["breaks"]), br)
    a = rows({u: {2015: i, 2022: 2 * i} for i, u in enumerate("ABCDEFG", start=1)})
    b = rows({u: {2015: i, 2022: 3 * i + 1} for i, u in enumerate("ABCDEFG", start=1)})
    lk = progress.links(a, b, 2015, 2022)
    assert_("two series moving together have rank correlations of one", abs(lk["spearman_change"] - 1) < 1e-12 and abs(lk["kendall_level"] - 1) < 1e-12, lk)
    return 0


if __name__ == "__main__":
    sys.exit(main())
