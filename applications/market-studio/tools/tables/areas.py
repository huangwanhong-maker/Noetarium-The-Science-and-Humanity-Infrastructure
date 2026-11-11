"""Places counted in areas: the rows of one table that lie in each area of another."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parents[1] / "market"))
import _market as m  # noqa: E402


def inside(ring: np.ndarray, x: np.ndarray, y: np.ndarray) -> np.ndarray:
    """Which points lie in a ring: a ray to the east crosses its edges an odd number of times."""
    hit = np.zeros(len(x), dtype=bool)
    for (a, b), (c, d) in zip(ring, np.roll(ring, -1, axis=0)):
        if b == d:
            continue
        hit ^= ((b > y) != (d > y)) & (x < a + (y - b) * (c - a) / (d - b))
    return hit


def create(api):
    class Areas(api.BoardAnalysis):
        id = "areas"
        title = "Count places in areas"
        description = ("The places of one table gathered into the areas of another: how many lie in each, the sum of a column of theirs, and both for each hectare. "
                       "A place on a boundary is counted once, in the first area that holds it.")
        params = [api.param("places", "Places", "", kind="text", role="table", help="a table whose rows have a latitude and a longitude"),
                  api.param("areas", "Areas", "", kind="text", role="table", help="a table whose rows are areas (its cell bound, with outlines)"),
                  api.param("of", "Add up", "", kind="text", role="column", of="places", help="a column of the places to add up; only the count when left out"),
                  api.param("name", "Call it", "in_areas", kind="text")]

        def run(self, board, params, context):
            pts, areas = board.table(params["places"]), board.table(params["areas"])
            roles = getattr(areas, "roles", None) or {}
            rings = (getattr(areas, "places", None) or {}).get("rings")
            if not (roles.get("cell") and rings):
                raise LookupError(f"the table '{areas.name}' has no areas: bind its cell, and give it outlines")
            lat, lon = m.where(pts)
            value = np.nan_to_num(pts.column(params["of"]), nan=0.0) if params.get("of") else None
            cells = list(dict.fromkeys(str(c) for c in areas.raw(roles["cell"]) if str(c) in rings))   # an area once, though the table may hold it at many times
            size = m.areas(areas)
            free = np.isfinite(lat) & np.isfinite(lon)
            count, total = [], []
            for k, c in enumerate(cells):
                ring = np.array(rings[c], dtype=float)
                near = free & (lon >= ring[:, 0].min()) & (lon <= ring[:, 0].max()) & (lat >= ring[:, 1].min()) & (lat <= ring[:, 1].max())
                index = np.flatnonzero(near)
                hit = index[inside(ring, lon[index], lat[index])] if len(index) else index
                free[hit] = False
                count.append(len(hit)), total.append(float(value[hit].sum()) if value is not None else None)
                if k % 50 == 0:
                    context.progress(k / max(1, len(cells)), f"{k} of {len(cells)} areas")
            hectares = np.array([size.get(c, 0.0) / 1e4 for c in cells])

            def per(v):
                return [round(a / h, 3) if h > 0 else None for a, h in zip(v, hectares)]
            columns = {"cell": cells, "count": count, "count_per_hectare": per(count), "hectares": np.round(hectares, 3)}
            label = "count"
            if value is not None:
                label = f"sum_of_{params['of']}"
                columns[label], columns[f"{label}_per_hectare"] = total, per(total)
            placed = int(np.sum(count))
            return [api.table(params.get("name") or "in_areas", columns, title=f"{pts.title or pts.name} in the areas of {areas.title or areas.name}", roles={"cell": "cell", "amount": label},
                              places={"kind": "cells", "column": "cell", "rings": {c: rings[c] for c in cells}}, show={"kind": "layer", "layer": "areas", "colour": f"{label}_per_hectare"}),
                    api.note(f"{placed:,} of {pts.rows:,} places lie in the {len(cells):,} areas; {pts.rows - placed:,} lie in none.")]
    return Areas()
