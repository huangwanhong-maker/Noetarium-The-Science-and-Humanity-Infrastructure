"""Checks of a place's population over half a century (data/products/population_history.py, ADR 0124), offline where they can be: World
Mollweide forward and back, and at its known points; the tile that holds Melbourne, as JRC's tiling names it; a uniform tile carried onto a
UTM grid with nobody lost and every cell near its density; and, where Melbourne's field has been made, its form, and its 2025 field lined up
with GHS-POP's own WGS 84 grid at the cells' centres.
"""

from __future__ import annotations

import csv
import json
import math
import sys

import numpy as np

from checks import PRODUCTS, ROOT, assert_, check, skip

sys.path.insert(0, str(ROOT / "data" / "products"))
sys.path.insert(0, str(ROOT / "data"))
import population_history as ph  # noqa: E402

# ---- Mollweide ------------------------------------------------------------------------------------------------------------------------
lons = np.array([0.0, 144.96, -73.98, 3.39, 179.0, -120.0])
lats = np.array([0.0, -37.81, 40.73, 6.45, 70.0, -60.0])
x, y = ph.mollweide(lons, lats)
lo, la = ph.mollweide_inverse(x, y)
check("population history: Mollweide there and back, to a billionth of a degree", float(max(np.max(np.abs(lo - lons)), np.max(np.abs(la - lats)))), 1e-9)
xe, ye = ph.mollweide(np.array([180.0, 0.0]), np.array([0.0, 90.0]))
check("population history: the equator's end at 2√2 R and the pole at √2 R", max(abs(xe[0] - 2 * math.sqrt(2) * ph.A), abs(ye[1] - math.sqrt(2) * ph.A)), 1e-6)

# ---- The tiling -------------------------------------------------------------------------------------------------------------------------
mx, my = ph.mollweide(144.96, -37.81)
assert_("population history: Melbourne lies in JRC's tile R14 C31 (the tile fetched from it)", ph.tile_of(float(mx), float(my)) == (14, 31), ph.tile_of(float(mx), float(my)))

# ---- Resampling, on a tile made here --------------------------------------------------------------------------------------------------
grid = ph.grid_for(-37.81, 144.96, 20, 32755)
proj = ph.utm(32755)
real_tile = ph.tile
def uniform(epoch, row, col):
    values = np.full((1000, 1000), 100.0)
    return values, ph.X0 + (col - 1) * ph.TILE, ph.Y0 - (row - 1) * ph.TILE
ph.tile = uniform
try:
    people, share = ph.resample(2020, grid, proj)
finally:
    ph.tile = real_tile
n = grid["rows"] * grid["cols"]
check("population history: a uniform 100 people per km² carried onto 1600 UTM cells keeps its total, to half a per cent", people.sum() / (100.0 * n) - 1, 5e-3)
inner = people[2:-2, 2:-2]
check("population history: and every inner cell is near 100, within the parts' granularity", float(np.max(np.abs(inner - 100))) / 100, 0.2)
assert_("population history: every cell is land when the tile has no sea", bool(np.all(share == 1.0)))

# ---- Melbourne's field, where it has been made ---------------------------------------------------------------------------------------
base = PRODUCTS / "population-history" / "melbourne"
if not (base / "field.json").is_file():
    skip("population history: Melbourne's field (not made: python data/products/population_history.py melbourne)")
else:
    f = json.loads((base / "field.json").read_text(encoding="utf-8"))
    with open(base / "cells.csv", encoding="utf-8") as fh:
        rows = list(csv.DictReader(fh))
    land = sum(sum(r) for r in f["land"])
    assert_("population history: Melbourne's field has its twelve years, two of them projected, a row of cells.csv for each land cell",
            f["schema"] == "noetarium.population-field/1" and f["epochs"] == list(range(1975, 2031, 5)) and f["projected"] == [2025, 2030] and len(rows) == land, (len(rows), land))
    worst = max(abs(sum(float(r[f"p{e}"]) for r in rows) - f["totals"][str(e)]) for e in f["epochs"])
    check("population history: its totals are the sums of its cells", worst, 2 * len(rows) * 0.05 + 1)
    glob = ROOT / "data" / "cache" / "ghsl-population" / "GHS_POP_E2025_GLOBE_R2023A_4326_30ss_V1_0.tif"
    if not glob.is_file():
        skip("population history: lined up against GHS-POP's WGS 84 grid (not in the cache: python data/acquisition/fetch.py ghsl-population)")
    else:
        from formats.tiff import GeoTIFF
        g = GeoTIFF.open(glob)
        T = g._transform()
        image = g.levels()[0][0]
        la = np.array([float(r["lat"]) for r in rows]); lo = np.array([float(r["lon"]) for r in rows])
        c0, r0 = int((lo.min() - 0.05 - T[0]) / T[1]), int((T[2] - (la.max() + 0.05)) / T[3])
        nc, nr = int((lo.max() - lo.min() + 0.1) / T[1]) + 2, int((la.max() - la.min() + 0.1) / T[3]) + 2
        win = np.asarray(image.window(c0, r0, nc, nr), dtype=float).reshape(nr, nc)
        cc = ((lo - T[0]) / T[1]).astype(int) - c0; rr = ((T[2] - la) / T[3]).astype(int) - r0
        theirs = np.maximum(win[rr, cc], 0)
        ours = np.array([float(r["p2025"]) for r in rows])
        corr = float(np.corrcoef(np.log1p(ours), np.log1p(theirs))[0, 1])
        assert_("population history: its 2025 field lines up with GHS-POP's own WGS 84 grid at the cells' centres (log correlation above 0.9)", corr > 0.9, corr)
