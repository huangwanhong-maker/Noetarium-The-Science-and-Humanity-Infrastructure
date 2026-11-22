"""Checks of the visitor stock at the API's edge (api/cpp/science/visitors.hpp, asked through science.visitors), run where the Urban
Studio's plug-in *Visitors, how many and when* has built its engine, every expected value worked by hand: a flat year's stock and its
intensity; platform rentals carried from a region by a key; a band's verdict; bands read from text; a month without twelve values
refused. One JSON object a line.

    python api/tests/check_visitors.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import visitors  # noqa: E402

DAYS = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "visitors: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def main() -> None:
    assert_("bands read from text: an upper and a lower", visitors.bands("nights_per_resident<=20; gini>=0.1") == [
        {"measure": "nights_per_resident", "upper": 20.0, "holder": "declared"}, {"measure": "gini", "lower": 0.1, "holder": "declared"}])
    if not visitors.ENGINE.program.exists():
        emit({"kind": "skip", "name": "visitors: the composition (the engine is not built: python applications/urban-studio/engine.py build visitors)"})
        return
    flat = lambda per_day: [per_day * d for d in DAYS]  # noqa: E731
    try:
        # A place of 50,000 residents on 25 km² with 600 domestic and 400 foreign registered nights a day; its region's platform rentals,
        # 300 a day, carried to it by its registered nights, a quarter of the region's.
        place = {"id": "a", "year": 2023, "population": 50000, "area_km2": 25, "nights": {"registered": {"DOM": flat(600), "FOR": flat(400)},
                                                                                        "platform": {"DOM": flat(100), "FOR": flat(200)}},
                 "arrivals": {"DOM": 100000, "FOR": 82500},
                 "allocate": {"source": "platform", "key_place": {"DOM": flat(600), "FOR": flat(400)}, "key_region": {"DOM": flat(2400), "FOR": flat(1600)}}}
        a = visitors.ask("visitors.stock", places=[place], readings=visitors.readings()[:2], bands=visitors.bands("per_thousand_mean<=20"))
        reg, plat = a["places"][0]["readings"]
        assert_("registered only: 1,000 present every day, 20 per thousand of 50,000, 40 per km², 7.3 nights a resident, the Gini nought",
                abs(reg["mean"] - 1000) < 1e-9 and abs(reg["per_thousand_mean"] - 20) < 1e-9 and abs(reg["per_km2_mean"] - 40) < 1e-9
                and abs(reg["nights_per_resident"] - 7.3) < 1e-9 and abs(reg["gini"]) < 1e-12, reg)
        assert_("platform rentals carried from the region by a quarter: 25 + 50 a day, 1,075 present; above a band of 20 per thousand",
                abs(plat["mean"] - 1075) < 1e-9 and plat["verdicts"]["per_thousand_mean"] == "above" and reg["verdicts"]["per_thousand_mean"] == "within", plat)
        assert_("the registered mean stay: 365,000 nights over 182,500 arrivals, two nights", abs(a["places"][0]["mean_stay"] - 2) < 1e-12, a["places"][0]["mean_stay"])
        try:
            visitors.ask("visitors.stock", places=[{**place, "nights": {"registered": {"DOM": [1, 2, 3]}}}], readings=visitors.readings()[:1])
            assert_("eleven months short are refused", False)
        except visitors.EngineError:
            assert_("eleven months short are refused", True)
    except visitors.EngineError as e:
        assert_("the engine answers", False, e)


if __name__ == "__main__":
    main()
