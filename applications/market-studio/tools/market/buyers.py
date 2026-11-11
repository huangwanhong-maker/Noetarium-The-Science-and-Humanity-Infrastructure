"""Buyers from a census of dwellings and floor space: residents and workers, by two assumptions."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _market as m  # noqa: E402


def create(api):
    class Buyers(api.BoardAnalysis):
        id = "buyers"
        title = "Buyers from dwellings and floor space"
        description = ("A census that counts dwellings and floor space, not people, made into who could buy: the residents of each place's dwellings and the "
                       "workers of its offices, each with what one brings in a period. The numbers of people a dwelling and a floor hold, and what each brings, "
                       "are quantities of the project, assumed.")
        params = [api.param("places", "Places", "blocks", kind="text", role="table", help="a table of places, each with its dwellings and its floor space"),
                  api.param("dwellings", "Dwellings", "dwellings", kind="text", role="column", of="places"),
                  api.param("floor", "Office floor, m²", "office", kind="text", role="column", of="places"),
                  api.param("residents_per_dwelling", "Residents a dwelling", "residents_per_dwelling", kind="text", role="quantity"),
                  api.param("workers_per_100m2", "Workers per 100 m²", "workers_per_100m2", kind="text", role="quantity"),
                  api.param("spend_resident", "A resident brings, a week", "spend_resident", kind="text", role="quantity"),
                  api.param("spend_worker", "A worker brings, a week", "spend_worker", kind="text", role="quantity"),
                  api.param("weeks", "Weeks of a period", "weeks", kind="text", role="quantity")]

        def run(self, board, params, context):
            t = board.table(params["places"])
            lat, lon = m.where(t)
            dwellings, floor = np.nan_to_num(t.column(params["dwellings"]), nan=0.0), np.nan_to_num(t.column(params["floor"]), nan=0.0)
            q = {k: float(board.quantity(params[k])) for k in ("residents_per_dwelling", "workers_per_100m2", "spend_resident", "spend_worker", "weeks")}
            residents, workers = dwellings * q["residents_per_dwelling"], floor / 100 * q["workers_per_100m2"]
            roles = getattr(t, "roles", None) or {}
            cells = t.raw(roles["cell"]) if roles.get("cell") else [None] * t.rows
            n = t.rows
            columns = {"cell": cells + cells, "who": ["residents"] * n + ["workers"] * n, "lat": np.concatenate([lat, lat]), "lon": np.concatenate([lon, lon]),
                       "mass": np.round(np.concatenate([residents, workers]), 1), "spend": [q["spend_resident"] * q["weeks"]] * n + [q["spend_worker"] * q["weeks"]] * n}
            rings = (getattr(t, "places", None) or {}).get("rings") if roles.get("cell") else None
            places = {"kind": "cells", "column": "cell", "rings": rings} if rings else None
            return [
                api.table("buyers", columns, title="Who could buy: residents and workers, by place", roles={"lat": "lat", "lon": "lon", "amount": "mass", "category": "who", **({"cell": "cell"} if places else {})},
                          places=places, show={"kind": "layer", "layer": "areas" if places else "points", "colour": "mass", "size": "mass"}),
                api.note(f"{residents.sum():,.0f} residents ({dwellings.sum():,.0f} dwellings × {q['residents_per_dwelling']:g}) and {workers.sum():,.0f} workers "
                         f"({floor.sum():,.0f} m² of offices × {q['workers_per_100m2']:g} per 100 m²). A resident brings {q['spend_resident']:g} a week and a worker {q['spend_worker']:g}, "
                         f"over {q['weeks']:g} weeks. The census states neither people nor what they spend: these are assumptions."),
            ]
    return Buyers()
