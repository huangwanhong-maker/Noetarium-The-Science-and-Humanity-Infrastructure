"""Checks of a community's greenhouse gas inventory and its mitigation at the API's edge (science/emissions.py and
api/cpp/science/emissions.hpp), run where the Urban Studio's plug-in *Materials and flows* has built its engine, every expected value
worked by hand: declarations refused where they are wrong; a small town's BASIC, BASIC+ and territorial totals with generation supplied
to the grid, waste from outside and biogenic CO2 kept where they belong; completeness with its gaps; two measures on one source
overlapping, the overlap counted once and shared by the Shapley value, the curve's order changing what each is credited with; a
pathway against a target; and the plug-in's invented town read whole. One JSON object a line.

    python api/tests/check_emissions.py
"""

from __future__ import annotations

import csv
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import emissions as em  # noqa: E402

DATA = ROOT / "applications" / "urban-studio" / "plugins" / "materials" / "templates" / "data"


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "emissions: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "emissions: " + name})


def main() -> None:
    for bad, why in ((lambda: em.entry("x", "energy", 1), "a sector it does not know"), (lambda: em.entry("x", "waste", 1, key="XX"), "a notation key it does not know"),
                     (lambda: em.measure("m", "waste", "price", 0.1), "a measure on neither activity nor factor"), (lambda: em.measure("m", "waste", "factor", 1.2), "a reduction above one")):
        try:
            bad()
            assert_(f"{why} is refused", False)
        except ValueError:
            assert_(f"{why} is refused", True)
    from science import materials
    if not materials.ENGINE.program.exists():
        skip("the compositions (the materials engine is not built: run the Urban Studio's recipe)")
        return
    try:
        # Homes burn 1,000 t and use 40,000 t of grid power; cars 20,000 t; a plant supplies the grid (5,000 t); a landfill 100 t of
        # methane (2,800 t under AR5); a neighbour's waste 20 t (560 t); wood 700 t biogenic.
        I = em.inventory("a town", 2025, [
            em.entry("h1", "stationary energy", 1, "homes", "CO2", amount=1.0e6), em.entry("h2", "stationary energy", 2, "homes", "CO2", activity=1.0e5, factor=400),
            em.entry("g", "stationary energy", 1, "generation", "CO2", amount=5.0e6, grid_supply=True), em.entry("c", "transportation", 1, "cars", "CO2", amount=2.0e7),
            em.entry("t2", "transportation", 2, "", key="NO"), em.entry("w1", "waste", 1, "landfill", "CH4", amount=1.0e5),
            em.entry("w3", "waste", 3, "landfill", "CH4", amount=0.0), em.entry("wi", "waste", 1, "landfill", "CH4", amount=2.0e4, imported_waste=True),
            em.entry("b", "stationary energy", 1, "homes", "CO2", amount=7.0e5, biogenic=True)], potentials="AR5", population=10000)
        T = em.totals(I)
        assert_("BASIC 1,000 + 40,000 + 20,000 + 2,800 = 63,800 t; the territorial total adds the plant and the neighbour's waste (+5,560) and drops grid power",
                abs(T["basic"] - 63800) < 1e-9 and abs(T["territorial"] - (1000 + 5000 + 20000 + 2800 + 560)) < 1e-9 and abs(T["biogenic"] - 700) < 1e-9, T)
        assert_("BASIC is complete; BASIC+ is not, four of its cells without an entry, each named",
                T["basic_complete"] and not T["basic_plus_complete"] and len(T["gaps_basic_plus"]) == 4, T["gaps_basic_plus"])
        assert_("per resident: 63,800 / 10,000 (BASIC+ equals BASIC here)", abs(T["per_capita"] - 6.38) < 1e-12, T["per_capita"])
        try:
            em.totals(em.inventory("x", 2025, [em.entry("a", "waste", 2, amount=1)]))
            assert_("scope 2 in waste is refused by the engine", False)
        except em.EngineError:
            assert_("scope 2 in waste is refused by the engine", True)
        J = em.inventory("one", 2025, [em.entry("p", "stationary energy", 2, "homes", "CO2", activity=1000, factor=400)])
        M = [em.measure("efficiency", "stationary energy", "activity", 0.2, cost=4000), em.measure("grid", "", "factor", 0.5, cost=6000, scope=2)]
        C = em.curve(J, M)
        a = {x["id"]: x for x in C["measures"]}
        assert_("two measures on one source: alone 80 and 200 t, together 240, the overlap 40 counted once",
                abs(a["efficiency"]["alone"] - 80) < 1e-9 and abs(a["grid"]["alone"] - 200) < 1e-9 and abs(C["joint"] - 240) < 1e-9 and abs(C["overlap"] - 40) < 1e-9, C)
        assert_("the curve takes the grid first (30 a tonne), and efficiency then abates 40 t at 100 a tonne; Shapley gives 60 and 180",
                C["measures"][0]["id"] == "grid" and abs(a["efficiency"]["in_order"] - 40) < 1e-9 and abs(a["efficiency"]["cost_per_tonne_in_order"] - 100) < 1e-9
                and abs(a["efficiency"]["shapley"] - 60) < 1e-9 and abs(a["grid"]["shapley"] - 180) < 1e-9, C)
        D = em.curve(J, M, order=["efficiency", "grid"])
        assert_("in the other order efficiency is credited 80 and the grid 160", abs(D["measures"][0]["in_order"] - 80) < 1e-9 and abs(D["measures"][1]["in_order"] - 160) < 1e-9, D)
        P = em.pathway(J, [dict(M[0], start=2026), dict(M[1], start=2026)], {"stationary energy": 0.0}, 2028, target={"year": 2027, "reduction": 0.6})
        assert_("a pathway with no growth: both measures from 2026 leave 160 t, meeting a target of 400 · 0.4 = 160 in 2027",
                all(abs(y["with_measures"] - 160) < 1e-9 for y in P["years"]) and abs(P["target_level"] - 160) < 1e-9 and P["meets_target"], P)

        def rows(name):
            with open(DATA / name, encoding="utf-8", newline="") as f:
                return list(csv.DictReader(f))
        f = lambda v: float(v) if v not in ("", None) else None  # noqa: E731
        entries = [em.entry(r["id"], r["sector"], int(r["scope"]), r["subsector"], r["gas"] or "CO2", activity=f(r["activity"]), unit=r["unit"], factor=f(r["factor"]),
                            amount=f(r["amount"]), u_activity=f(r["u_activity"]) or 0.0, u_factor=f(r["u_factor"]) or 0.0, key=r["key"], included_in=r["included_in"],
                            grid_supply=r["flag"] == "grid_supply", imported_waste=r["flag"] == "imported_waste", biogenic=r["flag"] == "biogenic") for r in rows("inventory.csv")]
        A = em.inventory("Ardel", 2024, entries, potentials="AR6", population=320000)
        TA = em.totals(A)
        assert_("the invented town: BASIC complete, BASIC+ held back by livestock not estimated; its total about 2.4 Mt",
                TA["basic_complete"] and not TA["basic_plus_complete"] and TA["gaps_basic_plus"] == ["agriculture, forestry and other land use, scope 1: not estimated (NE)"]
                and 2.0e6 < TA["basic_plus"] < 3.0e6, (TA["basic_plus"], TA["gaps_basic_plus"]))
        ms = [em.measure(r["id"], r["sector"], r["on"], float(r["reduction"]), cost=float(r["cost"]), scope=int(r["scope"] or 0), subsector=r["subsector"],
                         start=int(r["start"]), ramp=int(r["ramp"]), name=r["name"]) for r in rows("measures.csv")]
        CA = em.curve(A, ms)
        assert_("the town's seven measures: Shapley values summing to the joint abatement, the overlap of measures on one source positive",
                abs(sum(x["shapley"] for x in CA["measures"]) - CA["joint"]) < 1e-6 * CA["joint"] and CA["overlap"] > 0, CA)
    except em.EngineError as e:
        assert_("the engine answered", False, e)


if __name__ == "__main__":
    main()
