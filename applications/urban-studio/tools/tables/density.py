"""Places gathered into squares of a grid: how many there are, or how much, where."""

import math

import numpy as np


def create(api):
    class Density(api.BoardAnalysis):
        id = "density"
        title = "Where they are thick"
        description = "The places of a table gathered into the squares of a grid of a given side: the count in each, or the sum of a column. The squares are areas of the map."
        params = [api.param("table", "Table", "", kind="text", role="table", help="a table whose rows have a latitude and a longitude"),
                  api.param("side", "Side of a square, m", 200, minimum=10, maximum=100000),
                  api.param("of", "Add up", "", kind="text", role="column", of="table", help="a column to add up; the count when left out"),
                  api.param("name", "Call it", "density", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            roles = getattr(t, "roles", None) or {}
            if not (roles.get("lat") and roles.get("lon")):
                raise LookupError(f"the table '{t.name}' has no latitude and longitude bound")
            lat, lon = t.column(roles["lat"]), t.column(roles["lon"])
            ok = np.isfinite(lat) & np.isfinite(lon)
            if not ok.any():
                raise ValueError("no row has a place")
            value = np.nan_to_num(t.column(params["of"]), nan=0.0) if params.get("of") else np.ones(t.rows)
            lat0, side = float(np.mean(lat[ok])), float(params.get("side", 200))
            dlat, dlon = side / 110574.0, side / (111319.49 * math.cos(math.radians(lat0)))
            i, j = np.floor(lat / dlat), np.floor(lon / dlon)
            cells: dict = {}
            for k in np.flatnonzero(ok):
                key = (int(i[k]), int(j[k]))
                c = cells.setdefault(key, [0, 0.0])
                c[0] += 1
                c[1] += value[k]
            ids, rings, count, total = [], {}, [], []
            for (a, b), (n, s) in sorted(cells.items()):
                ident = f"{a}_{b}"
                ids.append(ident), count.append(n), total.append(s)
                south, west = a * dlat, b * dlon
                rings[ident] = [[west, south], [west + dlon, south], [west + dlon, south + dlat], [west, south + dlat]]
            label = f"sum_of_{params['of']}" if params.get("of") else "count"
            columns = {"cell": ids, "count": count, **({label: total} if params.get("of") else {})}
            return [api.table(params.get("name") or "density", columns, title=f"{t.title or t.name}: {label.replace('_', ' ')} in squares of {side:g} m",
                              roles={"cell": "cell", "amount": label}, places={"kind": "cells", "column": "cell", "rings": rings},
                              show={"kind": "layer", "layer": "areas", "colour": label})]
    return Density()
