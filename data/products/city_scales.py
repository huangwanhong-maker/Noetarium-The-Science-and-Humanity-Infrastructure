"""A city's units across scales for a model of them (ADR 0128): Melbourne's CLUE blocks within CLUE's small areas within the City of
Melbourne, with each block's floor space by use for every census year from 2002 to 2024, what the census withheld, its businesses
and dwellings, where it lies and which blocks are its neighbours. Kept under data/cache/products/city-scales/melbourne.json as

    {"schema": "noetarium.city-scales/1", "title", "credit", "licence", "years": [2002 … 2024],
     "levels": [{"name": "block", "units": [ids], "parent": [small areas], "measures": {"area_m2": [...]},
                 "xy": [[x, y] metres east and north of the centre], "lonlat": [[lon, lat]], "rings": {id: [[lon, lat], ...]}},
                {"name": "area", "units": [small areas], "parent": ["City of Melbourne", ...]},
                {"name": "city", "units": ["City of Melbourne"]}],
     "edges": [[i, j, 1]],                       blocks whose centres lie within 250 m, at most eight to a block
     "columns": [CLUE's 37 space uses], "groups": {use: [columns]},
     "years_of": {"2002": {"stated": [[m² | null per column] per block], "total": [...], "withheld": [...],
                            "prior": [[weight | null per column] per block], "establishments": [...], "dwellings": [...]}}}

What the census withheld. A cell that pertains to two or fewer businesses is blank, while the row's total is the true total: a blank
cell is not a zero, and the floor space withheld in a row, total − Σ stated, is shared among its blank cells in a way the census does
not say. In 2002–2024 it is 30% of all the floor space counted. This product estimates nothing: it writes, for each blank cell, a
prior weight (the same block's floor space of that use in the nearest year in which the cell was stated, else the small area's floor
space of that use in that year, else nothing), from which a dasymetric carriage (tyche's, asked by the Urban Studio's plug-in
*Cities across scales*) shares each row's withheld floor among its blank cells. Each blank cell lies between 0 and the row's
withheld floor whatever the weights: that interval is the census's own bound.

A block that is not in a year's census has nulls for it. Distances are on a plane tangent at the City's centre (an equirectangular
projection, good to well under 0.1% across the municipality).

    python data/products/city_scales.py
"""

from __future__ import annotations

import json
import math
import sys
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from city_tables import CACHE, USES, number, outer_ring, rows_of, spherical_area_km2  # noqa: E402

OUT = CACHE / "products" / "city-scales"
CITY = "City of Melbourne"
CENTRE = (144.9631, -37.8136)  # the City's centre (Town Hall), degrees
R_EARTH = 6371008.8  # mean radius, m


def local_xy(lon: float, lat: float) -> tuple[float, float]:
    k = math.pi / 180
    return (R_EARTH * (lon - CENTRE[0]) * k * math.cos(CENTRE[1] * k), R_EARTH * (lat - CENTRE[1]) * k)


def melbourne() -> dict:
    blocks = json.loads((CACHE / "melbourne-clue-blocks/all/blocks.geojson").read_text(encoding="utf-8"))
    rings = {str(f["properties"]["block_id"]): outer_ring(f["geometry"]) for f in blocks["features"]}
    floor = list(rows_of(CACHE / "melbourne-clue-floor-space/all/floor-space.csv"))
    columns = [c for c in floor[0].keys() if c not in ("census_year", "block_id", "clue_small_area", "total_floor_space_in_block")]
    area_of: dict[str, str] = {}
    by: dict[tuple[str, int], dict] = {}
    for r in floor:
        b, y = str(r["block_id"]), int(r["census_year"])
        if b not in rings:
            continue
        area_of[b] = r["clue_small_area"]
        by[(b, y)] = r
    units = sorted(area_of, key=lambda b: (area_of[b], int(b) if b.isdigit() else 0))
    areas = sorted(set(area_of.values()))
    years = sorted({y for (_, y) in by})
    index = {b: i for i, b in enumerate(units)}

    # Where each block lies, and its neighbours.
    lonlat, xy, area_m2 = [], [], []
    for b in units:
        ring = rings[b]
        lon = sum(p[0] for p in ring[:-1]) / max(1, len(ring) - 1)
        lat = sum(p[1] for p in ring[:-1]) / max(1, len(ring) - 1)
        lonlat.append([round(lon, 6), round(lat, 6)])
        x, y = local_xy(lon, lat)
        xy.append([round(x, 1), round(y, 1)])
        area_m2.append(round(spherical_area_km2(ring) * 1e6, 1))
    edges = set()
    for i in range(len(units)):
        near = sorted((math.dist(xy[i], xy[j]), j) for j in range(len(units)) if j != i)
        for d, j in near[:8]:
            if d <= 250:
                edges.add((min(i, j), max(i, j)))

    # Businesses and dwellings, counted.
    establishments: dict[tuple[str, int], float] = defaultdict(float)
    for r in rows_of(CACHE / "melbourne-clue-establishments/all/establishments.csv"):
        establishments[(str(r["block_id"]), int(r["census_year"]))] += 1
    dwellings: dict[tuple[str, int], float] = defaultdict(float)
    for r in rows_of(CACHE / "melbourne-clue-dwellings/municipality/dwellings.csv"):
        dwellings[(str(r["block_id"]), int(r["census_year"]))] += number(r.get("dwelling_number")) or 0.0

    # Each cell as stated, the floor withheld in each row, and the prior weights of the blank cells.
    stated = {(b, y): [number(by[(b, y)].get(c)) if (b, y) in by else None for c in columns] for b in units for y in years}
    area_use: dict[tuple[str, int], list[float]] = {}
    for a in areas:
        for y in years:
            s = [0.0] * len(columns)
            for b in units:
                if area_of[b] == a and (b, y) in by:
                    for k, v in enumerate(stated[(b, y)]):
                        s[k] += v or 0.0
            area_use[(a, y)] = s
    years_of = {}
    for y in years:
        st, tot, wh, prior, est, dw = [], [], [], [], [], []
        for b in units:
            if (b, y) not in by:
                st.append(None), tot.append(None), wh.append(None), prior.append(None), est.append(None), dw.append(None)
                continue
            row = stated[(b, y)]
            total = number(by[(b, y)]["total_floor_space_in_block"]) or 0.0
            withheld = max(0.0, total - sum(v for v in row if v is not None))
            weights = []
            for k, v in enumerate(row):
                if v is not None:
                    weights.append(None)
                    continue
                w = None
                for d in range(1, len(years)):
                    for yy in (y - d, y + d):
                        vv = stated.get((b, yy), [None] * len(columns))[k]
                        if vv is not None and vv > 0:
                            w = vv
                            break
                    if w is not None:
                        break
                if w is None and area_use[(area_of[b], y)][k] > 0:
                    w = area_use[(area_of[b], y)][k] * 1e-6  # the area's mix, far below any block's own past
                weights.append(round(w, 3) if w is not None else 0.0)
            st.append([round(v, 1) if v is not None else None for v in row])
            tot.append(round(total, 1))
            wh.append(round(withheld, 1))
            prior.append(weights)
            est.append(establishments.get((b, y), 0.0))
            dw.append(dwellings.get((b, y), 0.0))
        years_of[str(y)] = {"stated": st, "total": tot, "withheld": wh, "prior": prior, "establishments": est, "dwellings": dw}

    groups = {use: cols for use, cols in USES.items() if use != "vacant"}
    groups.update({"unused": ["unoccupied_unused"], "renovation": ["unoccupied_under_renovation"],
                   "construction": ["unoccupied_under_construction"], "demolition": ["unoccupied_under_demolition_condemned"],
                   "undeveloped": ["unoccupied_undeveloped_site"]})
    grouped = {c for cols in groups.values() for c in cols}
    groups["other"] = [c for c in columns if c not in grouped]
    return {
        "schema": "noetarium.city-scales/1",
        "title": "Melbourne: CLUE blocks within small areas within the City, 2002–2024",
        "credit": "City of Melbourne, Census of Land Use and Employment (CLUE)",
        "licence": "CC BY 4.0",
        "years": years,
        "levels": [
            {"name": "block", "units": units, "parent": [area_of[b] for b in units], "measures": {"area_m2": area_m2},
             "xy": xy, "lonlat": lonlat, "rings": {b: rings[b] for b in units}},
            {"name": "area", "units": areas, "parent": [CITY] * len(areas)},
            {"name": "city", "units": [CITY]},
        ],
        "edges": [[i, j, 1] for i, j in sorted(edges)],
        "columns": columns,
        "groups": groups,
        "years_of": years_of,
    }


def main() -> int:
    if not (CACHE / "melbourne-clue-floor-space/all/floor-space.csv").exists():
        print("CLUE's floor space is not in the cache: python data/acquisition/fetch.py melbourne-clue-floor-space", file=sys.stderr)
        return 1
    OUT.mkdir(parents=True, exist_ok=True)
    m = melbourne()
    (OUT / "melbourne.json").write_text(json.dumps(m, separators=(",", ":")), encoding="utf-8")
    index = {"cases": [{"name": "melbourne", "title": m["title"], "years": [m["years"][0], m["years"][-1]],
                        "levels": [[lv["name"], len(lv["units"])] for lv in m["levels"]]}]}
    (OUT / "index.json").write_text(json.dumps(index, indent=1), encoding="utf-8")
    withheld = sum(sum(v or 0 for v in y["withheld"]) for y in m["years_of"].values())
    total = sum(sum(v or 0 for v in y["total"]) for y in m["years_of"].values())
    print(f"melbourne: {len(m['levels'][0]['units'])} blocks in {len(m['levels'][1]['units'])} areas, {len(m['years'])} years; "
          f"{len(m['edges'])} pairs of neighbours; {withheld / total:.1%} of the floor space withheld")
    return 0


if __name__ == "__main__":
    sys.exit(main())
