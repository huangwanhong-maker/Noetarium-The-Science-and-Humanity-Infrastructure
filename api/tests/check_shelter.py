"""Checks of households in slums and inadequate housing at the API's edge (api/cpp/science/shelter.hpp, asked through science.shelter),
run where the Urban Studio's plug-in *Homes and who lacks them* has built its engine, every expected value worked by hand: four households
under the four components and the metadata's five; a missing answer's bounds; housing that costs more than 30% of income; an unknown
rule of tenure refused. One JSON object a line.

    python api/tests/check_shelter.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import shelter  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "shelter: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def main() -> None:
    if not shelter.ENGINE.program.exists():
        emit({"kind": "skip", "name": "shelter: the composition (the engine is not built: python applications/urban-studio/engine.py build shelter)"})
        return
    good = {"weight": 1, "stratum": 1, "rooms": 2, "water": "piped into dwelling", "sanitation": "flush to sewer", "shared_toilet": 0, "walls": "baked bricks",
            "roof": "metal sheets", "floor": "cement", "hazardous_site": 0, "documented": 1, "fears_eviction": 0, "housing_cost": 100, "income": 1000}
    # Four households of 2, 3, 5 and 10 people: the first without improved water, the second without a document, the third's water not
    # known, the fourth well housed and paying 400 of 1,000.
    rows = [{**good, "cluster": 1, "persons": 2, "rooms": 2, "water": "surface water", "district": "A"}, {**good, "cluster": 2, "persons": 3, "rooms": 3, "documented": 0, "district": "A"},
            {**good, "cluster": 3, "persons": 5, "rooms": 5, "water": "", "district": "B"}, {**good, "cluster": 4, "persons": 10, "rooms": 10, "housing_cost": 400, "district": "B"}]
    try:
        R = shelter.readings()
        a = shelter.ask("shelter.assess", households=shelter.households(rows), readings=R[:2])
        four, five = a["readings"]
        assert_("four components: the first household's 2 people of the 15 whose status is known; bounds 2/20 and 7/20",
                abs(four["slum"]["value"] - 2 / 15) < 1e-12 and abs(four["slum"]["lower"] - 0.1) < 1e-12 and abs(four["slum"]["upper"] - 0.35) < 1e-12, four["slum"])
        assert_("the metadata's five: tenure counted adds the second's 3 people: 5/15, bounds 5/20 and 10/20",
                abs(five["slum"]["value"] - 1 / 3) < 1e-12 and abs(five["slum"]["lower"] - 0.25) < 1e-12 and abs(five["slum"]["upper"] - 0.5) < 1e-12, five["slum"])
        assert_("inadequate housing: 400 of 1,000 exceeds 30%: 10/20; and by group A 5/5 under five", abs(five["inadequate"]["value"] - 0.5) < 1e-12 and abs(five["slum_by_group"]["A"]["value"] - 1) < 1e-12, five)
        try:
            shelter.ask("shelter.assess", households=shelter.households(rows), readings=[{**R[1], "tenure": "customary"}])
            assert_("a rule of tenure not declared is refused", False)
        except shelter.EngineError:
            assert_("a rule of tenure not declared is refused", True)
    except shelter.EngineError as e:
        assert_("the engine answers", False, e)


if __name__ == "__main__":
    main()
