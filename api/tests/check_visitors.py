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
        # Tourism accounts (ADR 0157): a two-product table whose Leontief inverse is [[1, 0.2], [0.3, 0.9]] / 0.84; 10 bought of product a,
        # 80% of it domestic; and a levy of 5 passed on in full at ε_d = −1.5.
        T = {"products": ["a", "b", "u"], "domestic": [[10, 20, 0], [30, 0, 0], [0, 0, 0]], "final_domestic": [70, 70, 0], "gva": [45, 50, 0],
             "taxes": [5, 10, 0], "imported": [10, 20, 0], "households_domestic_share": [0.8, 1, 1]}
        t = visitors.ask("tourism.accounts", table=T, purchases=[{"product": "a", "per_night": {"DOM": 1}, "basic_share": 1}], nights={"DOM": 10})
        assert_("tourism accounts: a product of no output left out; direct value added 3.6, total (0.45 · 8 + 0.5 · 2.4)/0.84, summing with imported inputs and taxes to 8",
                abs(t["direct_gva"] - 3.6) < 1e-9 and abs(t["total_gva"] - (0.45 * 8 + 0.5 * 2.4) / 0.84) < 1e-9
                and abs(t["total_gva"] + t["induced_imports"] + t["induced_taxes"] - 8) < 1e-9, t)
        lv = visitors.ask("tourism.levy", market={"nights": 1000, "price": 100, "demand_elasticity": -1.5}, levies=[5], cap=300, value_added_per_night=10)
        L = lv["flat"][0]
        assert_("a levy of 5 passed on in full: the visitor pays 105, the revenue 5 · 1000 · 1.05^−1.5, the value added lost 10 a night lost",
                abs(L["consumer_price"] - 105) < 1e-9 and abs(L["revenue"] - 5 * 1000 * 1.05 ** -1.5) < 1e-6 and abs(L["value_added_lost"] - 10 * L["nights_lost"]) < 1e-6
                and abs(lv["peak"]["levy"] - 200) < 1e-3, lv)
        # Recovery (ADR 0158): on the same table (outputs 100 and 100, L = [[1, 0.2], [0.3, 0.9]] / 0.84), 36.5 a year of a's final demand
        # lost for 10 days: alone a day's output times 3.65, that is 1; with the interdependencies (1 + 0.3) / 0.84; the long path equal to
        # the unbounded closed form.
        rc = visitors.ask("recovery.path", table=T, demand=[{"from": 0, "to": 10, "lost": {"a": 36.5}}], recovery_periods=5, periods=3000, periods_a_year=365)
        assert_("recovery: lost demand alone costs 1, through the interdependencies (1 + 0.3)/0.84, and the long path equals the unbounded closed form",
                abs(rc["unbounded_alone"] - 1) < 1e-9 and abs(rc["unbounded"] - 1.3 / 0.84) < 1e-9 and abs(rc["total"] - rc["unbounded"]) < 1e-9, rc)
        try:
            visitors.ask("visitors.stock", places=[{**place, "nights": {"registered": {"DOM": [1, 2, 3]}}}], readings=visitors.readings()[:1])
            assert_("eleven months short are refused", False)
        except visitors.EngineError:
            assert_("eleven months short are refused", True)
    except visitors.EngineError as e:
        assert_("the engine answers", False, e)


if __name__ == "__main__":
    main()
