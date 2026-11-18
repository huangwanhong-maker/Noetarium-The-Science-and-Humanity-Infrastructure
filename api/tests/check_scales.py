"""Checks of systems across scales at the API's edge (science/scales.py and api/cpp/science/scales.hpp), run by api/tests/run-tests.mjs,
where the Urban Studio's plug-in *Cities across scales* has built the engine: a document's hierarchy read with parents by name, what a
model lacks named with its level and count; a run held to a closed form; a ratio index as the ratio of sums at every level; nomos's
measures as reducers held to values computed here, and a measure undefined for a unit left without a value; costs from points held to the
interaction model computed here; an estimate of an exact truth, and one withheld outside the area of applicability; carriages that keep
their totals; a logistic's rate and ceiling recovered by a fit; refusals with their reasons. One JSON object a line, as check_client.py
reports.

    python api/tests/check_scales.py
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import scales  # noqa: E402
from science.engines import EngineError  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "systems across scales: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "systems across scales: " + name})


# ---- helpers that need no engine ------------------------------------------------------------------------------------------------
h = scales.levels([{"b": "b0", "a": "x"}, {"b": "b1", "a": "x"}, {"b": "b2", "a": "y"}], ["block", "area"], {"block": "b", "area": "a"})
assert_("a hierarchy laid out from rows: units in the order met, each with its parent", h["levels"][0]["parent"] == ["x", "x", "y"] and h["levels"][1]["units"] == ["x", "y"], h)
try:
    scales.levels([{"b": "b0", "a": "x"}, {"b": "b0", "a": "y"}], ["block", "area"], {"block": "b", "area": "a"})
    assert_("a unit with two parents is refused", False)
except ValueError:
    assert_("a unit with two parents is refused", True)
assert_("a declared value carries its range and no value of its own", scales.declared(1, 3) == {"source": "declared", "lower": 1.0, "upper": 3.0})

if not scales.ENGINE.status().get("built"):
    skip("the engine (the Urban Studio's plug-in of cities across scales has not built it: python applications/urban-studio/engine.py build scales)")
    sys.exit(0)

THREE = {"levels": [{"name": "block", "units": ["b0", "b1", "b2", "b3"], "parent": ["x", "x", "y", "y"]},
                    {"name": "area", "units": ["x", "y"], "parent": ["c", "c"]}, {"name": "city", "units": ["c"]}]}

# ---- what is missing, and a closed form ----------------------------------------------------------------------------------------
growth = {"hierarchy": THREE,
          "model": {"step": 0.01, "quantities": [{"name": "x", "level": "block", "kind": "extensive", "role": "state"}],
                    "derived": [{"name": "X", "level": "area", "op": "sum", "of": "x"}, {"name": "Xc", "level": "city", "op": "sum", "of": "X"}],
                    "laws": [{"target": "x", "form": "flow", "expression": "r * x"}], "parameters": [{"name": "r"}]},
          "inputs": {"values": {"x": [1, 2, None, 4]}}, "run": {"t0": 0, "t1": 1}}
d = scales.ask("describe", document=growth)
names = {(m["quantity"], m["units"]) for m in d["missing"]}
assert_("what a model lacks is named: x at one block, and the parameter r", ("x", 1) in names and ("r", 0) in names, d["missing"])
try:
    scales.ask("run", document=growth)
    assert_("a run with something missing is refused", False)
except EngineError as e:
    assert_("a run with something missing is refused, saying what to give", "declare it with a range" in str(e), e)
growth["inputs"]["values"]["x"][2] = 3
growth["inputs"]["parameters"] = {"r": scales.declared(0.1, 0.3)}
r = scales.ask("run", document=growth, run={"t0": 0, "t1": 1, "report": [1]})
assert_("the city's total grows as Σ x₀ e^{rt} at the declared range's centre", abs(r["values"]["Xc"][0][0] - 10 * math.exp(0.2)) < 1e-8, r["values"]["Xc"])
e = scales.ask("ensemble", document=growth, run={"t0": 0, "t1": 1, "report": [1]}, draws=200, outputs=["Xc"])
med = e["bands"]["Xc"]["middle"][0][0]
assert_("over draws of the declared rate the median total is 10 e^{0.2}", abs(med - 10 * math.exp(0.2)) < 0.03, med)

# ---- indexes at every level, and reducers ---------------------------------------------------------------------------------------
vac = [1, 2, 3, 0]
tot = [10, 10, 40, 20]
idx = {"hierarchy": THREE,
       "model": {"quantities": [{"name": "v", "level": "block", "kind": "extensive", "role": "fixed"}, {"name": "T", "level": "block", "kind": "extensive", "role": "fixed"}],
                 "derived": [{"name": "Va", "level": "area", "op": "sum", "of": "v"}, {"name": "Ta", "level": "area", "op": "sum", "of": "T"},
                             {"name": "rate", "level": "area", "op": "value", "of": "Va / Ta"},
                             {"name": "g", "level": "area", "op": "reduce", "reducer": "gini", "of": "v / T", "weight": "T"},
                             {"name": "dis", "level": "area", "op": "reduce", "reducer": "dissimilarity", "of": "v", "weight": "T"},
                             {"name": "mix", "level": "city", "op": "reduce", "reducer": "entropy_normalised", "of": "Ta"},
                             {"name": "bad", "level": "area", "op": "reduce", "reducer": "dissimilarity", "of": "v * 0", "weight": "T"}]},
       "inputs": {"values": {"v": vac, "T": tot}}, "run": {"t0": 0, "t1": 1, "report": [0]}}
r = scales.ask("run", document=idx)
assert_("an area's ratio index is the ratio of its sums: 3/20 and 3/60", abs(r["values"]["rate"][0][0] - 0.15) < 1e-15 and abs(r["values"]["rate"][0][1] - 0.05) < 1e-15)


def gini_w(x, w):  # the weighted Gini as the mean absolute difference over twice the mean, computed here
    W = sum(w)
    mu = sum(a * b for a, b in zip(x, w)) / W
    return sum(wi * wj * abs(xi - xj) for xi, wi in zip(x, w) for xj, wj in zip(x, w)) / (2 * W * W * mu)


g0 = gini_w([0.1, 0.2], [10, 10])
assert_("nomos's Gini as a reducer, against the mean absolute difference computed here", abs(r["values"]["g"][0][0] - g0) < 1e-12, (r["values"]["g"][0][0], g0))
dis = 0.5 * (abs(3 / 3 - 40 / 60) + abs(0 / 3 - 20 / 60))
assert_("the dissimilarity of v against T in the second area, by hand", abs(r["values"]["dis"][0][1] - dis) < 1e-12, r["values"]["dis"])
p = [20 / 80, 60 / 80]
mix = -sum(q * math.log(q) for q in p) / math.log(2)
assert_("the normalised entropy of the areas' totals", abs(r["values"]["mix"][0][0] - mix) < 1e-12)
assert_("a measure undefined for a unit leaves it without a value, and the run goes on", r["values"]["bad"][0][0] is None, r["values"]["bad"])

# ---- costs from points ---------------------------------------------------------------------------------------------------------
pts = [[0, 0], [300, 400], [600, 0]]
inter = {"hierarchy": {"levels": [{"name": "centre", "units": ["p", "q", "s"], "costs": {"walk": {"points": pts, "speed": 50, "detour": 1}}}]},
         "model": {"quantities": [{"name": "O", "level": "centre", "role": "fixed"}, {"name": "A", "level": "centre", "role": "fixed"}],
                   "derived": [{"name": "D", "level": "centre", "op": "inflow", "of": "O", "weight": "A", "costs": "walk", "beta": "0.1"}]},
         "inputs": {"values": {"O": [10, 0, 5], "A": [1, 2, 1]}}, "run": {"t0": 0, "t1": 1, "report": [0]}}
r = scales.ask("run", document=inter)
C = [[math.dist(a, b) / 50 for b in pts] for a in pts]
want = [sum(O * A_j * math.exp(-0.1 * C[i][j]) / sum(A_k * math.exp(-0.1 * C[i][k]) for k, A_k in enumerate([1, 2, 1]))
            for i, O in enumerate([10, 0, 5])) for j, A_j in enumerate([1, 2, 1])]
got = r["values"]["D"][0]
assert_("spatial interaction on walking minutes from points, against the formula computed here", max(abs(a - b) for a, b in zip(got, want)) < 1e-10, (got, want))

# ---- estimates and carriages ---------------------------------------------------------------------------------------------------
train = [{"covariates": [i, (i * 7) % 5], "value": 1 + 2 * i - 0.5 * ((i * 7) % 5), "block": i % 4} for i in range(30)]
est = scales.ask("estimate", training=train, targets=[{"covariates": [10, 2]}, {"covariates": [1000, 2]}])
assert_("an exact linear truth is estimated where it was not observed", abs(est["estimate"][0] - 20) < 1e-9 and not est["withheld"][0], est)
assert_("a target far outside what was learnt from is withheld, with the reason", est["withheld"][1] and "area of applicability" in est["reason"][1], est["reason"])
c = scales.ask("carry", method="dasymetric", values=[40, 60], targets=3,
               parts=[{"source": 0, "target": 0, "ancillary": 30}, {"source": 0, "target": 1, "ancillary": 10}, {"source": 1, "target": 1, "ancillary": 1}, {"source": 1, "target": 2, "ancillary": 2}])
assert_("a dasymetric carriage keeps the totals and follows the ancillary amounts", abs(sum(c["values"]) - 100) < 1e-12 and abs(c["values"][0] - 30) < 1e-12, c)
zone = [0] * 8 + [1] * 8
pyc = scales.ask("carry", method="pycnophylactic", grid={"rows": 4, "cols": 4, "zone": zone}, totals=[16, 48])
assert_("a pycnophylactic surface keeps each zone's total", abs(sum(pyc["values"][:8]) - 16) < 1e-9 and abs(sum(pyc["values"][8:]) - 48) < 1e-9)

# ---- a fit ----------------------------------------------------------------------------------------------------------------------
logistic = {"hierarchy": {"levels": [{"name": "unit", "units": ["u"]}]},
            "model": {"step": 0.05, "quantities": [{"name": "P", "level": "unit", "role": "state"}],
                      "laws": [{"target": "P", "form": "flow", "expression": "r * P * (1 - P / K)"}],
                      "parameters": [{"name": "r", "value": 0.2, "lower": 0.01, "upper": 2, "fit": True}, {"name": "K", "value": 100, "lower": 1, "upper": 500, "fit": True}]},
            "inputs": {"values": {"P": [5]}}, "run": {"t0": 0, "t1": 20}}
obs = [{"quantity": "P", "unit": "u", "time": t, "value": 50 / (1 + 9 * math.exp(-0.4 * t))} for t in range(21)]
f = scales.ask("fit", document=logistic, observations=obs, held_after=12)
got = {u["name"]: u["value"] for u in f["unknowns"]}
assert_("a logistic's rate and ceiling are recovered from its record", abs(got["r"] - 0.4) < 1e-4 and abs(got["K"] - 50) < 1e-3, got)
assert_("judged on the years held aside, far better than standing still", f["skill"] > 0.99, f["skill"])

# ---- refusals --------------------------------------------------------------------------------------------------------------------
wrong = json.loads(json.dumps(idx))
wrong["model"]["derived"].append({"name": "w", "level": "block", "op": "value", "of": "Ta"})
try:
    scales.ask("describe", document=wrong)
    assert_("a quantity named at another level is refused", False)
except EngineError as e:
    assert_("a quantity named at another level is refused, with the way to bring it down", "carry it to this level" in str(e), e)
