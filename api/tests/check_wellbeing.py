"""Checks of household well-being losses at the API's edge (api/cpp/science/wellbeing.hpp, asked through science.risk), run where the
Urban Studio's plug-in *Lifelines and evacuation* has built its engine, every expected value worked by hand: the log-utility series;
P1's Γ for a small loss; the step weights over return periods; measures ranked twice, a protection that saves assets of the rich
ranking below support by well-being; refusals. One JSON object a line.

    python api/tests/check_wellbeing.py
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import risk  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "well-being: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def main() -> None:
    if not risk.ENGINE.program.exists():
        emit({"kind": "skip", "name": "well-being: the composition (the risk engine is not built: python applications/urban-studio/engine.py build risk)"})
        return
    try:
        Pi, N, rho = 0.3, 3.0, 0.06
        lam = math.log(20) / N
        c, x = 1000.0, 0.5
        v = x * Pi / (Pi + lam)
        one = [{"id": "h", "group": "all", "persons": 1, "income": c, "vulnerability": v}]
        a = risk.ask("wellbeing.assess", households=one, events=[{"return_period": 100, "exposed": [1.0]}],
                     readings={"eta": 1, "rho": rho, "productivity": Pi, "rebuild_years": N, "horizon": 60})
        e = a["baseline"]["events"][0]
        series = sum(x ** n / (n * (rho + n * lam)) for n in range(1, 200))
        # One person at the mean: ΔC_eq = ΔW / c^{−1} = c ΔW.
        assert_("log utility: the equivalent of a household at the mean is c Σ xⁿ/(n(ρ + nλ))", abs(e["equivalent"] / (c * series) - 1) < 1e-9, e)
        small = [{"id": "h", "group": "all", "persons": 1, "income": c, "vulnerability": 1e-6 * Pi / (Pi + lam)}]
        s = risk.ask("wellbeing.assess", households=small, events=[{"return_period": 100, "exposed": [1.0]}], readings={"productivity": Pi, "rebuild_years": N, "horizon": 60})
        gamma = (Pi + lam) / (rho + lam)
        assert_("a small loss: resilience 1/Γ = (ρ + λ)/(Π + λ), P1's Eq. 1", abs(s["baseline"]["events"][0]["resilience"] * gamma - 1) < 1e-5, s["baseline"]["events"][0])
        two = risk.ask("wellbeing.assess", households=one, events=[{"return_period": 10, "exposed": [0.2]}, {"return_period": 100, "exposed": [0.6]}],
                       readings={"productivity": Pi})
        w = [ev["weight"] for ev in two["baseline"]["events"]]
        assert_("the step weights 1/10 − 1/100 and 1/100", abs(w[0] - 0.09) < 1e-15 and abs(w[1] - 0.01) < 1e-15, w)
        H = [{"id": "rich", "group": "waterfront", "persons": 100, "income": 6000, "transfers": 100, "savings": 2000, "vulnerability": 0.14},
             {"id": "poor", "group": "riverside", "persons": 100, "income": 500, "transfers": 50, "savings": 20, "vulnerability": 0.7}]
        E = [{"return_period": 20, "exposed": [0.4, 0.4]}, {"return_period": 100, "exposed": [0.7, 0.7]}]
        M = [{"name": "dyke", "kind": "protect", "group": "waterfront", "value": 0.9},
             {"name": "support", "kind": "support", "support": {"share": 0.8, "reference_group": "riverside"}}]
        m = risk.ask("wellbeing.assess", households=H, events=E, readings={"productivity": Pi, "optimal_rebuilding": True}, measures=M)["measures"]
        by = {x["name"]: x for x in m}
        assert_("a dyke for the rich saves the most assets and support none; by well-being support ranks first",
                by["dyke"]["rank_by_assets"] == 1 and by["support"]["avoided_asset_loss"] == 0 and by["support"]["rank_by_wellbeing"] == 1, m)
        try:
            risk.ask("wellbeing.assess", households=one, events=[{"return_period": 10, "exposed": [1.0]}], measures=[{"name": "?", "kind": "magic"}])
            assert_("an unknown measure is refused", False)
        except risk.EngineError:
            assert_("an unknown measure is refused", True)
    except risk.EngineError as e:
        assert_("the engine answers", False, e)


if __name__ == "__main__":
    main()
