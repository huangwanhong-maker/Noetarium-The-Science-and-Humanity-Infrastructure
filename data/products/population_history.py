"""The population of a place over half a century, cell by cell, for learning how it moves and grows (the Urban Studio's plug-in
Population dynamics).

GHS-POP R2023A (Schiavina, Freire, Carioli and MacManus 2023, European Commission JRC, CC BY 4.0) estimates the residential
population of the Earth at every fifth year from 1975 to 2030, on a grid of 1 km in the World Mollweide projection (ESRI:54009),
cut into tiles of 1000 x 1000 cells. Each epoch is disaggregated from the censuses then available into the built-up surface of
the GHSL; 2025 and 2030 are projections made in 2023 from the UN's World Population Prospects 2022, not counts. A tile is a
GeoTIFF of people per cell (float64), the sea -200.

Mollweide keeps area and not shape: at Melbourne its grid's two axes meet on the ground at 134 degrees, its steps 1.0 and 1.4 km
long, so that a neighbour on its grid is not a neighbour at a distance. Here each epoch is carried onto a square grid of 1 km in
the place's UTM zone (conformal, its scale within 0.04% of one), with no person lost: each source cell is cut into k x k parts,
each carrying 1/k^2 of its people to the cell of the new grid in which its centre falls. A cell of the new grid is land when at
least half its parts came from land. The resampling smooths by at most a cell, and keeps every epoch's total to rounding.

The projection's sphere is WGS 84's semi-major axis (6 378 137 m), as PROJ's moll uses it: with it a tile lines up with GHS-POP's
grid on WGS 84 at a correlation of 0.93 at Melbourne, with the authalic radius at 0.14. (The projection belongs to geodesy; it
is here, beside the data library's transverse Mercator, until that domain has its Python.)

A case is a place and a half-width; adding a city is adding a case, or naming a place on the command line.

    melbourne     Melbourne, 80 km square about the City (UTM 55S)
    lagos         Lagos, 70 km square about Lagos Island (UTM 31N)

    python data/products/population_history.py [case ...]
    python data/products/population_history.py --at LAT,LON --half KM --name NAME [--zone EPSG]
        → data/cache/products/population-history/<case>/  field.json · cells.csv · totals.csv;  population-history/index.json

The tiles are kept as received under data/cache/ghsl-population-tiles/E<epoch>/, each with its retrieval record.
"""

from __future__ import annotations

import csv
import datetime as dt
import hashlib
import io
import json
import math
import sys
import time
import urllib.request
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(ROOT / "data"))

from formats.tiff import GeoTIFF, utm  # noqa: E402

OUT = ROOT / "data" / "cache" / "products" / "population-history"
TILES = ROOT / "data" / "cache" / "ghsl-population-tiles"
EPOCHS = list(range(1975, 2031, 5))
PROJECTED = [2025, 2030]
BASE = "https://jeodpp.jrc.ec.europa.eu/ftp/jrc-opendata/GHSL/GHS_POP_GLOBE_R2023A"
A = 6_378_137.0                       # the sphere of ESRI:54009 as PROJ's moll uses it
TILE, X0, Y0 = 1_000_000.0, -18_041_000.0, 9_000_000.0     # GHSL's tiling of 54009: 1000 km tiles from the north-west corner
CELL = 1000.0
PARTS = 8                             # each source cell cut into 8 x 8 parts
CREDIT = "GHS-POP R2023A (Schiavina et al. 2023), European Commission JRC, CC BY 4.0; resampled to a UTM grid of 1 km here"

CASES = {
    "melbourne": {"title": "Melbourne", "lat": -37.8136, "lon": 144.9631, "half_km": 40, "epsg": 32755},
    "lagos": {"title": "Lagos", "lat": 6.4550, "lon": 3.3941, "half_km": 35, "epsg": 32631},
}


def log(text: str) -> None:
    print(text, file=sys.stderr, flush=True)


# ---- World Mollweide on the sphere of radius A (Snyder 1987, Map Projections: A Working Manual, USGS PP 1395, pp. 249-252) ----

def mollweide(lon, lat):
    """(x, y) in metres of longitude and latitude in degrees: 2θ + sin 2θ = π sin φ by Newton's method (Snyder eqs. 31-1 to 31-3)."""
    import numpy as np
    phi = np.radians(np.asarray(lat, dtype=np.float64))
    lam = np.radians(np.asarray(lon, dtype=np.float64))
    theta = phi.copy()
    for _ in range(40):
        f = 2 * theta + np.sin(2 * theta) - math.pi * np.sin(phi)
        d = 2 + 2 * np.cos(2 * theta)
        step = np.where(np.abs(d) > 1e-12, f / np.where(np.abs(d) > 1e-12, d, 1.0), 0.0)
        theta = theta - step
        if np.max(np.abs(step)) < 1e-14:
            break
    return 2 * math.sqrt(2) / math.pi * A * lam * np.cos(theta), math.sqrt(2) * A * np.sin(theta)


def mollweide_inverse(x, y):
    """Longitude and latitude in degrees of (x, y) in metres (Snyder eqs. 31-4 to 31-6)."""
    import numpy as np
    theta = np.arcsin(np.clip(np.asarray(y, dtype=np.float64) / (math.sqrt(2) * A), -1, 1))
    phi = np.arcsin(np.clip((2 * theta + np.sin(2 * theta)) / math.pi, -1, 1))
    lam = math.pi * np.asarray(x, dtype=np.float64) / (2 * math.sqrt(2) * A * np.cos(theta))
    return np.degrees(lam), np.degrees(phi)


# ---- Tiles ----------------------------------------------------------------------------------------------------------------------

def tile_of(x: float, y: float) -> tuple[int, int]:
    return int((Y0 - y) // TILE) + 1, int((x - X0) // TILE) + 1


def tile(epoch: int, row: int, col: int):
    """The tile's people per cell (1000 x 1000, the sea -200) and its north-west corner, fetched once and kept as received."""
    import numpy as np
    stem = f"GHS_POP_E{epoch}_GLOBE_R2023A_54009_1000_V1_0_R{row}_C{col}"
    folder = TILES / f"E{epoch}"
    tif = folder / f"{stem}.tif"
    if not tif.exists():
        url = f"{BASE}/GHS_POP_E{epoch}_GLOBE_R2023A_54009_1000/V1-0/tiles/{stem}.zip"
        log(f"  fetching {stem}.zip")
        for attempt in range(4):
            try:
                req = urllib.request.Request(url, headers={"User-Agent": "Noetarium data library"})
                with urllib.request.urlopen(req, timeout=300) as r:
                    data = r.read()
                break
            except Exception:  # noqa: BLE001
                if attempt == 3:
                    raise
                time.sleep(3 * (attempt + 1))
        member = zipfile.ZipFile(io.BytesIO(data)).read(f"{stem}.tif")
        folder.mkdir(parents=True, exist_ok=True)
        tif.write_bytes(member)
        record = {"url": url, "member": f"{stem}.tif", "bytes_received": len(data), "sha256_received": hashlib.sha256(data).hexdigest(),
                  "sha256": hashlib.sha256(member).hexdigest(), "fetched": dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds"),
                  "credit": "GHS-POP R2023A, European Commission JRC, CC BY 4.0"}
        (folder / f"{stem}.json").write_text(json.dumps(record, indent=1), encoding="utf-8")
    g = GeoTIFF.open(tif)
    image = g.levels()[0][0]
    x0, dx, y0, dy = g._transform()
    if abs(dx - CELL) > 1e-6 or abs(dy - CELL) > 1e-6:
        raise ValueError(f"{stem}: a cell of {dx} x {dy} m, not 1 km")
    values = np.asarray(image.read(), dtype=np.float64).reshape(round(TILE / CELL), round(TILE / CELL))
    return values, x0, y0


# ---- The place --------------------------------------------------------------------------------------------------------------------

def grid_for(lat: float, lon: float, half_km: float, epsg: int) -> dict:
    proj = utm(epsg)
    e, n = proj.forward(lon, lat)
    e0 = math.floor((float(e) - half_km * 1000) / CELL) * CELL
    n1 = math.ceil((float(n) + half_km * 1000) / CELL) * CELL
    cols = rows = int(round(2 * half_km))
    return {"crs": f"EPSG:{epsg}", "x0": e0, "y0": n1, "dx": CELL, "cols": cols, "rows": rows}


def resample(epoch: int, grid: dict, proj):
    """One epoch's people on the UTM grid (rows from the north), and the share of each cell that is land."""
    import numpy as np
    rows, cols = grid["rows"], grid["cols"]
    # The UTM box's outline in Mollweide, to know which source cells can reach it.
    t = np.linspace(0, 1, 41)
    ex = np.concatenate([grid["x0"] + t * cols * CELL, np.full(41, grid["x0"] + cols * CELL), grid["x0"] + t * cols * CELL, np.full(41, grid["x0"])])
    ny = np.concatenate([np.full(41, grid["y0"]), grid["y0"] - t * rows * CELL, np.full(41, grid["y0"] - rows * CELL), grid["y0"] - t * rows * CELL])
    lon, lat = proj.inverse(ex, ny)
    mx, my = mollweide(lon, lat)
    xmin, xmax = math.floor((mx.min() - 2 * CELL) / CELL) * CELL, math.ceil((mx.max() + 2 * CELL) / CELL) * CELL
    ymin, ymax = math.floor((my.min() - 2 * CELL) / CELL) * CELL, math.ceil((my.max() + 2 * CELL) / CELL) * CELL
    people = np.zeros((rows, cols))
    land = np.zeros((rows, cols))
    parts = np.zeros((rows, cols))
    off = (np.arange(PARTS) + 0.5) / PARTS
    needed = {tile_of(x, y) for x in (xmin + 1, xmax - 1) for y in (ymin + 1, ymax - 1)}
    for (tr, tc) in sorted(needed):
        values, tx0, ty0 = tile(epoch, tr, tc)
        c0, c1 = max(0, int((xmin - tx0) // CELL)), min(values.shape[1], int((xmax - tx0) // CELL) + 1)
        r0, r1 = max(0, int((ty0 - ymax) // CELL)), min(values.shape[0], int((ty0 - ymin) // CELL) + 1)
        if c0 >= c1 or r0 >= r1:
            continue
        block = values[r0:r1, c0:c1]
        cc, rr = np.meshgrid(np.arange(c0, c1), np.arange(r0, r1))
        # Each source cell's parts: x = tx0 + (c + u) CELL, y = ty0 - (r + v) CELL.
        u, v = np.meshgrid(off, off)
        X = (tx0 + (cc[..., None] + u.ravel()) * CELL).ravel()
        Y = (ty0 - (rr[..., None] + v.ravel()) * CELL).ravel()
        val = np.repeat(block.ravel(), PARTS * PARTS)
        lo, la = mollweide_inverse(X, Y)
        E, N = proj.forward(lo, la)
        j = np.floor((E - grid["x0"]) / CELL).astype(int)
        i = np.floor((grid["y0"] - N) / CELL).astype(int)
        ok = (i >= 0) & (i < rows) & (j >= 0) & (j < cols)
        i, j, val = i[ok], j[ok], val[ok]
        island = val > -100
        np.add.at(people, (i[island], j[island]), val[island] / (PARTS * PARTS))
        np.add.at(land, (i[island], j[island]), 1.0)
        np.add.at(parts, (i, j), 1.0)
    share = np.divide(land, parts, out=np.zeros_like(land), where=parts > 0)
    return people, share


def build(name: str, title: str, lat: float, lon: float, half_km: float, epsg: int) -> dict:
    import numpy as np
    log(f"{name}: {title}, {2 * half_km:g} km square about {lat:.4f}, {lon:.4f}, on {epsg}")
    proj = utm(epsg)
    grid = grid_for(lat, lon, half_km, epsg)
    fields, shares = {}, []
    for epoch in EPOCHS:
        people, share = resample(epoch, grid, proj)
        fields[epoch] = people
        shares.append(share)
        log(f"  {epoch}: {people.sum():,.0f} people")
    land = np.mean(shares, axis=0) >= 0.5
    rows, cols = grid["rows"], grid["cols"]
    jj, ii = np.meshgrid(np.arange(cols), np.arange(rows))
    E = grid["x0"] + (jj + 0.5) * CELL
    N = grid["y0"] - (ii + 0.5) * CELL
    clon, clat = proj.inverse(E, N)
    out = OUT / name
    out.mkdir(parents=True, exist_ok=True)
    totals = {e: float(fields[e][land].sum()) for e in EPOCHS}
    lost = {e: float(fields[e][~land].sum()) for e in EPOCHS}
    field = {
        "schema": "noetarium.population-field/1",
        "case": name, "title": f"{title}: residential population, 1975 to 2030",
        "place": {"lat": lat, "lon": lon, "half_km": half_km},
        "grid": {**grid, "unit": "people per cell of 1 km²", "rows_from": "north"},
        "epochs": EPOCHS, "projected": PROJECTED,
        "land": [[int(v) for v in row] for row in land],
        "values": {str(e): [[round(float(v), 1) if land[i, j] else None for j, v in enumerate(row)] for i, row in enumerate(fields[e])] for e in EPOCHS},
        "totals": {str(e): round(totals[e]) for e in EPOCHS},
        "off_land": {str(e): round(lost[e]) for e in EPOCHS},
        "about": (f"GHS-POP R2023A's residential population of {title} at every fifth year from 1975 to 2030, carried from its 1 km Mollweide "
                  f"grid onto a 1 km grid in UTM ({grid['crs']}), {rows} x {cols} cells, with no person lost (each source cell cut into "
                  f"{PARTS} x {PARTS} parts). Each epoch is the JRC's disaggregation of the censuses then available into the built-up surface; "
                  "2025 and 2030 are projections made in 2023, not counts. A cell is land when most of it came from land; the people the "
                  "resampling placed in cells of the sea are left out and counted (off_land)."),
        "source": CREDIT,
    }
    (out / "field.json").write_text(json.dumps(field, separators=(",", ":")), encoding="utf-8")
    with open(out / "cells.csv", "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["cell", "row", "col", "x", "y", "lat", "lon"] + [f"p{e}" for e in EPOCHS])
        for i in range(rows):
            for j in range(cols):
                if land[i, j]:
                    w.writerow([f"r{i:03d}c{j:03d}", i, j, round(float(E[i, j])), round(float(N[i, j])), round(float(clat[i, j]), 6), round(float(clon[i, j]), 6)]
                               + [round(float(fields[e][i, j]), 1) for e in EPOCHS])
    with open(out / "totals.csv", "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["year", "people", "projected"])
        for e in EPOCHS:
            w.writerow([e, round(totals[e]), "yes" if e in PROJECTED else "no"])
    summary = {"case": name, "title": title, "grid": grid, "land_cells": int(land.sum()), "totals": field["totals"], "off_land": field["off_land"],
               "path": f"products/population-history/{name}/field.json"}
    log(json.dumps({k: summary[k] for k in ("case", "land_cells", "totals")}))
    return summary


def main(argv: list[str]) -> int:
    import argparse
    p = argparse.ArgumentParser(description="The population of a place over half a century, on a 1 km UTM grid")
    p.add_argument("cases", nargs="*")
    p.add_argument("--at", help="LAT,LON of the centre")
    p.add_argument("--half", type=float, default=30, help="half-width in km")
    p.add_argument("--name", help="the case's name")
    p.add_argument("--zone", type=int, help="the UTM zone's EPSG code (326zz north, 327zz south); found from the centre if left out")
    a = p.parse_args(argv)
    index_path = OUT / "index.json"
    index = {c["case"]: c for c in json.loads(index_path.read_text(encoding="utf-8")).get("cases", [])} if index_path.exists() else {}
    if a.at:
        lat, lon = (float(v) for v in a.at.split(","))
        epsg = a.zone or ((32700 if lat < 0 else 32600) + int((lon + 180) // 6) + 1)
        name = a.name or f"at-{lat:.3f}-{lon:.3f}"
        index[name] = build(name, a.name or name, lat, lon, a.half, epsg)
    else:
        for name in a.cases or list(CASES):
            if name not in CASES:
                print(f"there is no case {name} (there is: {', '.join(CASES)})", file=sys.stderr)
                return 2
            c = CASES[name]
            index[name] = build(name, c["title"], c["lat"], c["lon"], c["half_km"], c["epsg"])
    OUT.mkdir(parents=True, exist_ok=True)
    index_path.write_text(json.dumps({"cases": list(index.values())}, indent=1), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
