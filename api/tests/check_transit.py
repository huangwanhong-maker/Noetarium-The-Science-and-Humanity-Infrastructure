"""Checks of public transport and who can reach it at the API's edge (api/cpp/science/transit.hpp, asked through science.transit), run
where the Urban Studio's plug-in *Public transport and who can reach it* has built its engine, every expected value worked by hand: a
regular headway's mean wait half the headway; a street's share within reach under trams of low and of high capacity; a straight line
round a corner; a mode's capacity refused when it is neither low nor high; open space reached along a street, its area within a box
(ADR 0154). One JSON object a line.

    python api/tests/check_transit.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import transit  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "transit: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def main() -> None:
    if not transit.ENGINE.program.exists():
        emit({"kind": "skip", "name": "transit: the composition (the engine is not built: python applications/urban-studio/engine.py build transit)"})
        return
    every = lambda step: list(range(6 * 3600, 22 * 3600 + 1, step))  # noqa: E731
    try:
        # A street of 2 km: a bus stop at 0 every 10 minutes, a tram stop at 1,500 every 5; places at 300, 900 and 1,900.
        stops = [{"id": "b", "name": "b", "x": 0, "y": 0, "departures": {"bus": every(600)}},
                 {"id": "t", "name": "t", "x": 1500, "y": 0, "departures": {"tram": every(300)}}]
        a = transit.ask("transit.access", ways=[[0, 0, 2000, 0]], places=[[300, 0, 10], [900, 0, 10], [1900, 0, 10]], stops=stops,
                        readings=[{"trams_high": False}, {"trams_high": True}])
        s = {x["id"]: x for x in a["stops"]}
        assert_("a bus every ten minutes: six an hour at the peak, a mean wait of five minutes", abs(s["b"]["per_hour"] - 6) < 1e-12 and abs(s["b"]["mean_wait"] - 300) < 1e-9, s["b"])
        low, high = a["readings"]
        assert_("trams of low capacity: 300 m from the bus and 400 m from the tram within 500 m, 900 m from both not: 2/3",
                abs(low["share"] - 2 / 3) < 1e-12, low)
        assert_("trams of high capacity: the place 600 m from the tram within 1 km as well: 3/3", abs(high["share"] - 1) < 1e-12, high)
        b = transit.ask("transit.access", ways=[[0, 0, 0, 1000], [0, 0, 1000, 0]], places=[[0, 400, 1]],
                        stops=[{"id": "b", "name": "b", "x": 300, "y": 0, "departures": {"bus": every(600)}}],
                        readings=[{"trams_high": False}, {"trams_high": False, "along_network": False}])
        assert_("round a corner: 700 m along the streets (not within 500 m), 500 m in a straight line (within)",
                b["readings"][0]["share"] == 0 and b["readings"][1]["share"] == 1 and abs(b["places"][0][3] - 700) < 1e-9, b)
        try:
            transit.ask("transit.access", ways=[[0, 0, 100, 0]], places=[[0, 0, 1]], stops=stops, readings=[{"name": "x", "capacity": {"tram": "medium"}}])
            assert_("a capacity neither low nor high is refused", False)
        except transit.EngineError:
            assert_("a capacity neither low nor high is refused", True)
        # Open space (ADR 0154): a street of 1.2 km, a park 100 m square across it at 500–600 m, a plaza of 20 m at 1,150; places at 0,
        # 160, 550 and 1,000; the analysis area the box from 0 to 1,000 along the street and 500 m either side of it.
        spaces = [{"kind": "park", "ring": [500, -50, 600, -50, 600, 50, 500, 50]}, {"kind": "plaza", "ring": [1150, 10, 1170, 10, 1170, 30, 1150, 30]}]
        o = transit.ask("openspace.access", ways=[[0, 0, 1200, 0]], places=[[0, 0, 10], [160, 0, 10], [550, 0, 10], [1000, 0, 10]], spaces=spaces,
                        box=[0, -500, 1000, 500], cell=1, readings=[{"name": "parks", "kinds": ["park"], "metres": 350}, {"name": "parks and plazas", "kinds": ["park", "plaza"], "metres": 350},
                                                              {"name": "line", "kinds": ["park"], "metres": 450, "along_network": False}])
        r0, r1, r2 = o["readings"]
        assert_("open space: parks within 350 m reach the place 340 m away and the one inside (2/4), 10,000 m² of a box of 10⁶",
                abs(r0["share"] - 0.5) < 1e-12 and abs(r0["area"] - 1e4) < 1e-6 and abs(r0["area_share"] - 0.01) < 1e-12 and o["places"][2][3] == 0, o)
        assert_("... the plaza 150 m along from the place at 1,000 adds it (3/4), but lies outside the box: the area unchanged", abs(r1["share"] - 0.75) < 1e-12 and abs(r1["area"] - 1e4) < 1e-6, r1)
        assert_("... in a straight line within 450 m, the place at 1,000 m is 400 m from the park's edge, the place at 0 500 m: 3/4", abs(r2["share"] - 0.75) < 1e-12, r2)
    except transit.EngineError as e:
        assert_("the engine answers", False, e)


if __name__ == "__main__":
    main()
