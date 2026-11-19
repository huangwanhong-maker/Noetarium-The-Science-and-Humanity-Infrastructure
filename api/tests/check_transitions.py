"""Checks of critical transitions at the API's edge (science/transitions.py and api/cpp/science/transitions.hpp), run by
api/tests/run-tests.mjs where the Dynamics Lab's engine is built: the helpers; a fold's record from the engine assessed as a signal with
its eigenvalue going to +1; a stationary record not; a network's record with its space, the leading mode found where one unit slows; a
composition with its total; the climate domain's overturning driven towards its fold, with its folds and a true restoring rate that falls;
the energy balance with a ramp albedo keeping its rate; the likelihood test and wells; refusals with their reasons. One JSON object a
line, as check_client.py reports.

    python api/tests/check_transitions.py
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import transitions as tr  # noqa: E402
from science.engines import EngineError  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "critical transitions: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "critical transitions: " + name})


QUICK = {"null_draws": 79, "alternative_draws": 40, "surrogate_draws": 39, "robustness": ["phase"], "grid": False, "replicates": 30, "max_windows": 60}

# ---- helpers that need no engine ------------------------------------------------------------------------------------------------
s = tr.space(units=3, edges=[[0, 1], [1, 2, 0.5]], unit_names=["a", "b", "c"])
assert_("a space with a neighbourhood and names", s["units"] == 3 and s["edges"][1] == [1, 2, 0.5] and s["unit_names"] == ["a", "b", "c"], s)
for bad, why in ((lambda: tr.space(chart="tensor"), "an unknown chart"), (lambda: tr.space(units=2, unit_names=["a"]), "names for some units"),
                 (lambda: tr.space(chart="histogram", parts=3), "a histogram without its edges"), (lambda: tr.record([0, 1], [1]), "a record short of states")):
    try:
        bad()
        assert_(f"{why} is refused", False)
    except ValueError:
        assert_(f"{why} is refused", True)

if not tr.ENGINE.program.exists():
    skip("the engine is not built (python applications/dynamics-lab/engine.py build)")
    sys.exit(0)

try:
    fold = tr.ask("model", model="fold", length=600, step=0.2, seed=3)
    a = tr.assess(fold["record"], **QUICK)
    variance = next(f for f in a["findings"] if f["indicator"] == "variance")
    assert_("a fold's record from the engine is assessed as a signal", a["overall"] == "signal", a["summary"])
    assert_("its variance rising, with the test's detection rate given", variance["verdict"] == "signal" and 0 < variance["detection"] <= 1, variance)
    assert_("its dominant eigenvalue going to +1", a["kind"]["approach"] == "stationary", a["kind"])
    assert_("and what none of its detectors can see is listed, rate-induced tipping among it",
            any("rate-induced" in c for c in a["cannot_see"]), a["cannot_see"])
    assert_("every finding states its null and its reasons", all(f["null"] and f["reasons"] for f in a["findings"]), a["findings"][0])

    flat = tr.ask("model", model="stationary", length=600, step=0.2, seed=5)
    b = tr.assess(flat["record"], **QUICK)
    assert_("a stationary record is not a signal", b["overall"] != "signal", b["summary"])

    net = tr.ask("model", model="network", length=1000, step=0.2, units=8, coupling=0.05, where=2, sigma=0.3, seed=13)
    c = tr.assess(net["record"], space=net["space"], **QUICK)
    top = max(range(8), key=lambda u: c["where"]["loading"][u])
    assert_("on a network where one unit slows, the leading mode shows there", top == 2, c["where"])
    assert_("and the network's spatial indicators are read", any(f["indicator"] == "Moran's I" for f in c["findings"]), [f["indicator"] for f in c["findings"]])

    # A composition of three parts whose total slows: its size and shape kept apart.
    import random
    rng = random.Random(7)
    n, dt = 1500, 0.2
    times = [i * dt for i in range(n)]
    total, b1, b2, states = 0.0, 0.0, 0.0, []
    for i in range(n):
        k = 1.0 - 0.97 * i / (n - 1)
        total += -k * total * dt + 0.2 * math.sqrt(dt) * rng.gauss(0, 1)
        b1 += -1.0 * b1 * dt + 0.2 * math.sqrt(dt) * rng.gauss(0, 1)
        b2 += -1.0 * b2 * dt + 0.2 * math.sqrt(dt) * rng.gauss(0, 1)
        w = [math.exp(b1), math.exp(b2), 1.0]
        size = 1000 * math.exp(total)
        states.append([size * v / sum(w) for v in w])
    d = tr.assess(tr.record(times, states), space=tr.space(parts=3, chart="composition", with_total=True, part_names=["young", "working", "old"]), **{**QUICK, "surrogate_draws": 99})
    lead = next(f for f in d["findings"] if f["indicator"] == "largest covariance eigenvalue")
    assert_("a composition with its log total is read in three coordinates", d["coordinates"] == 3, d["space"])
    assert_("and its slowing total gives a signal", any(f["verdict"] == "signal" for f in d["findings"] if f["indicator"] in
                                                         ("largest covariance eigenvalue", "relative variance growth", "leading propagator modulus")), tr.verdicts(d))

    ov = tr.ask("climate", model="overturning", length=800, step=0.05, sigma=0.02, seed=31)
    assert_("the overturning's folds at η² = 7.5", abs(ov["folds"]["p_lower"] - 0.964) < 5e-4 and abs(ov["folds"]["p_upper"] - 1.480) < 5e-4, ov["folds"])
    assert_("its true restoring rate falling towards the fold", ov["true_rate"][0] > 3 * ov["true_rate"][-1] > 0, (ov["true_rate"][0], ov["true_rate"][-1]))
    e = tr.assess(ov["record"], **QUICK)
    rate = next(f for f in e["findings"] if f["indicator"] == "restoring rate (AR(1))")
    assert_("its record assessed: the restoring rate falls, a signal", rate["verdict"] == "signal", rate["reasons"])

    eb = tr.ask("climate", model="energy_balance", form="ramp", length=600, step=1, sigma=0.05, seed=22, **{"from": 340.0, "to": 291.4})
    assert_("the ramp energy balance keeps its restoring rate B/C to its fold", all(abs(r - 0.209) < 1e-9 for r in eb["true_rate"]), eb["true_rate"][:3])

    lk = tr.ask("likelihood", times=fold["record"]["times"], values=[s[0] for s in fold["record"]["states"]], replicates=30)
    assert_("the likelihood test answers with its error rates and its sentence", 0 <= lk["false_alarm"] <= 1 and 0 <= lk["auc"] <= 1 and lk["sentence"], lk)
    dw = tr.ask("model", model="double_well", length=4000, step=0.1, sigma=0.45, seed=8)
    wl = tr.ask("wells", values=[s[0] for s in dw["record"]["states"]], step=0.1)
    assert_("a double well's record has two wells", wl["wells"] == 2, wl["sentence"])

    try:
        tr.ask("assess", record=tr.record([0, 1, 3, 4], [1, 2, 3, 4]))
        assert_("an irregular record is refused", False)
    except EngineError as err:
        assert_("an irregular record is refused, saying why", "regularly sampled" in str(err), err)
    try:
        tr.ask("model", model="tensor")
        assert_("an unknown test model is refused", False)
    except EngineError as err:
        assert_("an unknown test model is refused, naming the models", "fold" in str(err), err)
except EngineError as err:
    assert_("the engine answered", False, err)
