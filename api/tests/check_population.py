"""Checks of population dynamics at the API's edge (science/population.py and api/cpp/science/population.hpp), run by
api/tests/run-tests.mjs. The helpers' order of cells and their refusals; and, where the Urban Studio has built the engine, fields made
here by a separate integrator (numpy, the five-point Laplacian, small Runge–Kutta steps), so that what the engine finds is held to a
truth it did not make: Fisher's law recovered from six times five years apart, judged on the two kept aside; a logistic total's rate
and ceiling recovered; a source of people adding exactly its people under a law that only moves them; a drift toward the centre
gathering people there; two modes and their eigenvalues; what is refused, with the reason. One JSON object a line, as
check_client.py reports.

    python api/tests/check_population.py
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

import numpy as np  # noqa: E402

from science import population as pd  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "population dynamics: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "population dynamics: " + name})


def rel(a, b) -> float:
    return abs(a - b) / max(abs(b), 1e-300)


# ---- A field made here ------------------------------------------------------------------------------------------------------------
R, C = 12, 12
LAND = np.ones((R, C), dtype=bool)
LAND[0, 0] = LAND[0, 1] = LAND[1, 0] = False  # a corner of sea


def laplacian(u: np.ndarray) -> np.ndarray:
    """The five-point Laplacian on the land cells, no flux into the sea or past the edges (h = 1 km)."""
    out = np.zeros_like(u)
    for di, dj in ((0, 1), (0, -1), (1, 0), (-1, 0)):
        nb = np.zeros_like(u)
        ok = np.zeros_like(LAND)
        src = np.roll(np.roll(u, -di, 0), -dj, 1)
        srcl = np.roll(np.roll(LAND, -di, 0), -dj, 1)
        valid = np.ones_like(LAND)
        if di == 1: valid[-1, :] = False
        if di == -1: valid[0, :] = False
        if dj == 1: valid[:, -1] = False
        if dj == -1: valid[:, 0] = False
        ok = valid & srcl & LAND
        nb[ok] = src[ok] - u[ok]
        out += nb
    return out


def run(u0: np.ndarray, law, years: list[float], h: float = 0.02) -> list[np.ndarray]:
    u, t, out = u0.copy(), years[0], [u0.copy()]
    f = lambda v: law(v) * LAND  # noqa: E731
    for target in years[1:]:
        while t < target - 1e-12:
            s = min(h, target - t)
            k1 = f(u); k2 = f(u + s / 2 * k1); k3 = f(u + s / 2 * k2); k4 = f(u + s * k3)
            u = u + s / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
            t += s
        out.append(u.copy())
    return out


def field_of(frames: list[np.ndarray], epochs: list[int], projected: list[int]) -> dict:
    return {"schema": pd.SCHEMA, "case": "made-here", "title": "A field made here", "grid": {"rows": R, "cols": C, "dx": 1000.0},
            "epochs": epochs, "projected": projected, "land": LAND.astype(int).tolist(),
            "values": {str(e): [[float(fr[i, j]) if LAND[i, j] else None for j in range(C)] for i in range(R)] for e, fr in zip(epochs, frames)}}


ii, jj = np.mgrid[0:R, 0:C]
bump = (200 + 3000 * np.exp(-((ii - 6.5) ** 2 + (jj - 5.5) ** 2) / 5.0)) * LAND

# ---- The helpers ------------------------------------------------------------------------------------------------------------------
f0 = field_of([bump] * 3, [2000, 2005, 2010], [])
cells = pd.cells(f0, Path("/nowhere"))
assert_("the cells are the land's, row by row from the north, the sea left out", len(cells) == R * C - 3 and (cells[0]["row"], cells[0]["col"]) == (0, 2) and (cells[-1]["row"], cells[-1]["col"]) == (R - 1, C - 1))
try:
    pd.table(f0, {"x": [1, 2]}, Path("/nowhere"))
    assert_("a column that is not one value per cell is refused", False)
except ValueError as e:
    assert_("a column that is not one value per cell is refused", "has 2 values" in str(e), e)
assert_("a field's totals and its projected years read", pd.totals({**f0, "totals": {"2000": 1, "2005": 2, "2010": 3}, "projected": [2010]})["projected"] == [False, False, True])

if not pd.ENGINE.status().get("built"):
    skip("the engine (the Urban Studio's population plug-in has not built it: python applications/urban-studio/engine.py build population)")
    sys.exit(0)

# ---- Fisher's law, recovered ------------------------------------------------------------------------------------------------------
r_, K_, D_ = 0.04, 5000.0, 0.3
fisher = lambda u: r_ * u * (1 - u / K_) + D_ * laplacian(u)  # noqa: E731
epochs = [2000, 2005, 2010, 2015, 2020, 2025, 2030]
frames = run(bump, fisher, [float(e) for e in epochs])
F = field_of(frames, epochs, [2030])
L = pd.ask("learn", field=F, models=["persistence"], law=["linear", "quadratic", "diffusion"], weights="none")
m = [x for x in L["models"] if x["name"] == "declared"][0]
co = {t["term"]: t["coefficient"] for t in m["law"]}
worst = max(rel(co["linear"], r_), rel(co["quadratic"], -r_ / K_), rel(co["diffusion"], D_))
assert_("Fisher's law, from six times five years apart, fitted on four: its rate, its ceiling and its spread, to 0.5%", worst < 5e-3, co)
assert_("its years kept aside are the last two recorded, the projected year left out", L["held_out"] == [2020, 2025] and L["fitted_on"] == [2000, 2005, 2010, 2015], L)
assert_("it forecasts them with all the skill there is to have", m["held_out"]["skill"] > 0.999 and m["held_out"]["pattern_skill"] > 0.99, m["held_out"])
rd = m["reading"]
assert_("what it says: growth at 4% a year toward 5000 per km², spread at 0.3 km² a year, a front at 2√(rD)",
        rel(rd["growth_rate_a_year"], r_) < 5e-3 and rel(rd["ceiling_per_km2"], K_) < 1e-2 and rel(rd["front_speed_km_a_year"], 2 * math.sqrt(r_ * D_)) < 5e-3, rd)
assert_("its currents are given at the last time fitted, one per cell", len(L["currents"]["declared"]["east"]) == R * C - 3 and L["currents_at"] == 2015)

# ---- The whole, as one quantity --------------------------------------------------------------------------------------------------
years = list(range(1975, 2021, 5))
P0, K, r = 2e6, 8e6, 0.05
people = [K / (1 + (K / P0 - 1) * math.exp(-r * (y - 1975))) for y in years]
G = pd.ask("growth", years=years, people=people, to=2040)
lg = [x for x in G["models"] if x["name"] == "logistic"][0]
assert_("a logistic total's rate and ceiling recovered", rel(lg["reading"]["rate_a_year"], r) < 2e-3 and rel(lg["reading"]["ceiling"], K) < 2e-3, lg["reading"])
at2040 = K / (1 + (K / P0 - 1) * math.exp(-r * (2040 - 1975)))
assert_("and run on year by year to 2040, where Verhulst's solution has it", lg["curve"]["years"][-1] == 2040 and rel(lg["curve"]["people"][-1], at2040) < 2e-3, (lg["curve"]["people"][-1], at2040))

# ---- A source, a drift ----------------------------------------------------------------------------------------------------------
flat = np.full((R, C), 1000.0) * LAND
Fs = field_of([flat, flat, flat], [2000, 2005, 2010], [])
S = pd.ask("simulate", field=Fs, law=["diffusion"], coefficients=[0.5], sources=[{"row": 6, "col": 6, "people_per_year": 1000}], to=2030, every=5)
gain = [t - S["totals"][0] for t in S["totals"]]
assert_("under a law that only moves people, a source of 1000 a year adds exactly 5000 every five years",
        all(abs(g - 5000 * k) < 1e-6 * S["totals"][0] for k, g in enumerate(gain)), gain)
D = pd.ask("simulate", field=Fs, law=["drift:centre", "diffusion"], coefficients=[0.2, 0.05], to=2020, every=10)
mid = [k for k, c in enumerate(pd.cells(Fs, Path("/nowhere"))) if (c["row"], c["col"]) in ((5, 5), (5, 6), (6, 5), (6, 6))]
edge = [k for k, c in enumerate(pd.cells(Fs, Path("/nowhere"))) if c["row"] == R - 1]
assert_("a drift toward the centre gathers people there and thins the edge, keeping everyone",
        all(D["frames"][-1][k] > 1000 for k in mid) and all(D["frames"][-1][k] < 1000 for k in edge) and rel(D["totals"][-1], D["totals"][0]) < 1e-6,
        [D["frames"][-1][k] for k in mid])
assert_("and says its speed toward the centre", rel(D["reading"]["speed_toward_centre_km_a_year"], 0.2) < 1e-12, D["reading"])

# ---- Modes --------------------------------------------------------------------------------------------------------------------------
phi1 = (1000 + 0 * ii) * LAND
phi2 = (np.cos(ii / 3.0) * np.sin(jj / 4.0) * 400) * LAND
mframes = [phi1 * 1.02 ** k + phi2 * 0.85 ** k for k in range(8)]
M = pd.ask("modes", field=field_of(mframes, list(range(1980, 2020, 5)), []))
eig = sorted(complex(*x["eigenvalue"]).real for x in M["modes"])
assert_("a field of two modes has rank two, and their eigenvalues 1.02 and 0.85 a step", M["rank"] == 2 and abs(eig[0] - 0.85) < 1e-9 and abs(eig[1] - 1.02) < 1e-9, M)

# ---- Refusals -------------------------------------------------------------------------------------------------------------------------
def refused(request: dict, words: str) -> tuple[bool, str]:
    try:
        pd.ask(**request)
        return False, "answered"
    except pd.EngineError as e:
        return words in str(e), str(e)

ok, said = refused({"op": "spin", "field": F}, "growth, learn, modes, simulate")
assert_("an operation there is not is refused with those there are", ok, said)
ok, said = refused({"op": "learn", "field": {**F, "land": F["land"][:3]}}, "rows, not")
assert_("a land mask of the wrong size is refused", ok, said)
ok, said = refused({"op": "simulate", "field": F, "law": ["drift:nowhere"], "coefficients": [1.0]}, "no covariate nowhere")
assert_("a drift up a covariate that is not given is refused", ok, said)
ok, said = refused({"op": "simulate", "field": F, "model": "persistence"}, "not a law that runs")
assert_("a baseline cannot be run on", ok, said)
