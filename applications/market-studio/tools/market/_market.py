"""What the market's tools share: where a table's rows are, the costs between buyers and offers, and the market stated for the engine.

The computations are the markets domain's, asked of the engine through science.markets; nothing of them is done again here. What is
done here is the reading of the person's tables by their roles.
"""

from __future__ import annotations

import json
import math
from pathlib import Path

import numpy as np

from science import markets

EARTH = 6371008.8
WALK = 1.34   # m/s


def role(table, name: str, what: str) -> str:
    """The column a table binds to a role, or the reason it is needed."""
    column = (getattr(table, "roles", None) or {}).get(name)
    if not column or not table.has(column):
        raise LookupError(f"the table '{table.name}' has no column bound as {what}: bind one in its source")
    return column


def column_or(table, column: str | None, fallback_role: str, what: str) -> str:
    if column:
        if not table.has(column):
            raise LookupError(f"the table '{table.name}' has no column '{column}'")
        return column
    return role(table, fallback_role, what)


def areas(table) -> dict:
    """The area of each of a table's cells, in m², by its id (a ring's area on the sphere, small as it is, as on a plane)."""
    out = {}
    for ident, ring in ((getattr(table, "places", None) or {}).get("rings") or {}).items():
        if len(ring) < 3:
            continue
        lat0 = sum(p[1] for p in ring) / len(ring)
        k = math.cos(math.radians(lat0))
        xy = [(math.radians(p[0]) * k * EARTH, math.radians(p[1]) * EARTH) for p in ring]
        out[str(ident)] = abs(sum(x0 * y1 - x1 * y0 for (x0, y0), (x1, y1) in zip(xy, xy[1:] + xy[:1]))) / 2
    return out


def where(table) -> tuple[np.ndarray, np.ndarray]:
    """The latitude and longitude of each row: its own, or the middle of its cell."""
    roles = getattr(table, "roles", None) or {}
    if roles.get("lat") and roles.get("lon"):
        return table.column(roles["lat"]), table.column(roles["lon"])
    rings = (getattr(table, "places", None) or {}).get("rings")
    if roles.get("cell") and rings:
        centre = {str(k): (sum(p[1] for p in r) / len(r), sum(p[0] for p in r) / len(r)) for k, r in rings.items() if r}
        at = [centre.get(str(c), (math.nan, math.nan)) for c in table.raw(roles["cell"])]
        return np.array([p[0] for p in at]), np.array([p[1] for p in at])
    raise LookupError(f"the table '{table.name}' has no place: bind its latitude and longitude, or its cell, in its source")


def spread(table, speed: float) -> np.ndarray:
    """The minutes walked within each row's cell, from where its buyers stand to its middle: the mean distance of a square's points
    from its centre, 0.3826 of its side, for a square of the cell's area. Zero for rows that are points."""
    roles = getattr(table, "roles", None) or {}
    area = areas(table)
    if not (roles.get("cell") and area):
        return np.zeros(table.rows)
    return np.array([0.3826 * math.sqrt(area.get(str(c), 0.0)) / speed / 60 for c in table.raw(roles["cell"])])


def local(lat: np.ndarray, lon: np.ndarray, lat0: float, lon0: float) -> tuple[np.ndarray, np.ndarray]:
    """Metres east and north of a point: a plane true at that point."""
    return np.radians(lon - lon0) * math.cos(math.radians(lat0)) * EARTH, np.radians(lat - lat0) * EARTH


def pairs(buyers, offers_at, how: str, limit_minutes: float, speed: float, model: str = "", detour: float = 1.3):
    """The pairs of buyers and places within reach, with the minutes between them: walked on the streets of a city model of the
    data library, or in a straight line lengthened by a declared detour. To each is added the walk within the buyer's cell."""
    blat, blon = where(buyers)
    olat, olon = offers_at
    ok = np.isfinite(blat) & np.isfinite(blon)
    within = spread(buyers, speed)
    if how == "streets":
        if not model:
            raise ValueError("walking on the streets needs a city model of the data library: name its file")
        path = markets.PRODUCTS / model
        if not path.is_file():
            raise LookupError(f"the data library has no city model '{model}'")
        origin = json.loads(path.read_text(encoding="utf-8"))["origin"]
        bx, by = local(blat, blon, origin["lat"], origin["lon"])
        ox, oy = local(olat, olon, origin["lat"], origin["lon"])
        index = np.flatnonzero(ok)
        answer = markets.ask("costs", model=model, limit=limit_minutes * 60, walk=speed,
                             **{"from": [[float(bx[i]), float(by[i])] for i in index], "to": [[float(x), float(y)] for x, y in zip(ox, oy)]})
        b = index[np.array(answer["b"], dtype=int)] if answer["b"] else np.array([], dtype=int)
        o, minutes = np.array(answer["o"], dtype=int), np.array(answer["seconds"], dtype=float) / 60
    else:
        lat0, lon0 = float(np.nanmean(blat)), float(np.nanmean(blon))
        bx, by = local(blat, blon, lat0, lon0)
        ox, oy = local(olat, olon, lat0, lon0)
        reach = limit_minutes * 60 * speed / detour
        bs, os_, ms = [], [], []
        for start in range(0, len(bx), 2000):
            d = np.hypot(bx[start:start + 2000, None] - ox[None, :], by[start:start + 2000, None] - oy[None, :])
            i, j = np.nonzero(d <= reach)
            bs.append(i + start), os_.append(j), ms.append(d[i, j] * detour / speed / 60)
        b, o, minutes = np.concatenate(bs), np.concatenate(os_), np.concatenate(ms)
        keep = ok[b]
        b, o, minutes = b[keep], o[keep], minutes[keep]
    minutes = minutes + within[b]
    keep = minutes <= limit_minutes
    return b[keep], o[keep], minutes[keep]


def sellers(names: list) -> tuple[list, list]:
    """Sellers from trading names: the same name, whatever its case and spaces, is one seller; a place without a name is its own."""
    index, order, seller = {}, [], []
    for k, name in enumerate(names):
        key = " ".join(str(name or "").upper().split()) or f"(unnamed {k})"
        if key not in index:
            index[key] = len(order)
            order.append(str(name).strip() if str(name or "").strip() else "(unnamed)")
        seller.append(index[key])
    return seller, order


COMMON = None


def common(api) -> list:
    """The parameters every tool of the market shares: who could buy, what is offered, and how they choose."""
    return [
        api.param("buyers", "Buyers", "buyers", kind="text", role="table", help="a table of who could buy, each row a place"),
        api.param("mass", "How many", "mass", kind="text", role="column", of="buyers", help="the column of how many buyers a row stands for"),
        api.param("spend", "What one brings", "spend", kind="text", role="column", of="buyers", help="the column of what one buyer brings in a period"),
        api.param("offers", "Offers", "offers", kind="text", role="table", help="a table of what is offered, each row an outlet at a place"),
        api.param("seller", "Seller", "", kind="text", role="column", of="offers", help="the column that names an outlet's seller; its name, when left out"),
        api.param("attraction", "Attraction", "", kind="text", role="column", of="offers", help="a column of how much an outlet attracts; all alike when left out"),
        api.param("walk", "Minutes are", "straight", kind="choice", options=[{"value": "straight", "label": "in a straight line, lengthened"}, {"value": "streets", "label": "walked on a city model's streets"}]),
        api.param("model", "City model", "", kind="text", help="for streets: a city model of the data library, as city-model-melbourne.json"),
        api.param("limit", "Reach, minutes", 15, minimum=1, maximum=120),
        api.param("decay", "Nearer is better", "power", kind="choice", options=[{"value": "power", "label": "as a power"}, {"value": "exponential", "label": "exponentially"}]),
        api.param("beta", "β", 2.0, minimum=0, maximum=20, step=0.05),
        api.param("outside", "Not buying, as an outlet this many minutes away", 5.0, minimum=0, maximum=120),
    ]


def stated(board, params: dict) -> dict:
    """The market the parameters describe, read from the board's tables: what the engine is asked about, and what the tools need
    beside it (the tables, the sellers' names, the pairs)."""
    buyers, offers = board.table(params["buyers"]), board.table(params["offers"])
    if buyers.rows == 0 or offers.rows == 0:
        raise ValueError(f"the slice holds {buyers.rows} rows of buyers and {offers.rows} of offers: a market needs both")
    mass, spend = buyers.column(column_or(buyers, params.get("mass"), "amount", "how many buyers a row stands for")), buyers.column(params["spend"]) if params.get("spend") and buyers.has(params["spend"]) else None
    if spend is None:
        raise LookupError(f"the table '{buyers.name}' has no column '{params.get('spend')}' of what one buyer brings")
    mass, spend = np.nan_to_num(mass, nan=0.0), np.nan_to_num(spend, nan=0.0)
    olat, olon = where(offers)
    if not (np.isfinite(olat) & np.isfinite(olon)).all():
        raise ValueError(f"{int((~(np.isfinite(olat) & np.isfinite(olon))).sum())} rows of '{offers.name}' have no place")
    named = offers.text(column_or(offers, params.get("seller"), "name", "its seller or its name"))
    seller, seller_names = sellers(named)
    attraction = None
    if params.get("attraction"):
        attraction = np.nan_to_num(offers.column(params["attraction"]), nan=0.0)
        if (attraction <= 0).any():
            raise ValueError(f"the attraction '{params['attraction']}' is not positive in {int((attraction <= 0).sum())} rows")
    speed = WALK
    b, o, minutes = pairs(buyers, (olat, olon), params.get("walk", "straight"), float(params.get("limit", 15)), speed, str(params.get("model") or ""))
    huff = {"beta": float(params.get("beta", 2)), "decay": params.get("decay", "power"), "floor": 1.0}
    market = markets.market(mass, spend, seller, (b, o, minutes), attraction=attraction, huff=huff, outside=float(params.get("outside", 0)))
    return {"market": market, "buyers": buyers, "offers": offers, "names": named, "seller": seller, "seller_names": seller_names, "olat": olat, "olon": olon,
            "pairs": (b, o, minutes), "speed": speed, "assumed": assumed(params)}


def assumed(params: dict) -> str:
    walk = "walked on the streets" if params.get("walk") == "streets" else "in a straight line lengthened by 1.3"
    return (f"Presumed, not observed: Huff's model ({params.get('decay', 'power')}, β {params.get('beta', 2)}) on minutes {walk} at {WALK} m/s, with the walk within "
            f"a buyer's cell added; reach {params.get('limit', 15)} minutes; not buying pulls as an outlet {params.get('outside', 0)} minutes away. "
            "Buyers and outlets beyond the slice are not counted: its edges read as less served than they are.")


def by_place(table, **amounts):
    """Amounts of a table's rows added up place by place: where the rows are cells, a row for each cell (the residents and the workers of
    one block are one area of the map); where they are points, the rows as they are. Answers the columns, the roles and the places."""
    roles = getattr(table, "roles", None) or {}
    rings = (getattr(table, "places", None) or {}).get("rings")
    if not (roles.get("cell") and rings):
        columns, out_roles, places = place_columns(table)
        return {**columns, **amounts}, out_roles, places
    order, index = [], {}
    for c in table.raw(roles["cell"]):
        if c not in index:
            index[c] = len(order)
            order.append(c)
    at = np.array([index[c] for c in table.raw(roles["cell"])])
    summed = {name: np.bincount(at, weights=np.nan_to_num(np.asarray(v, dtype=float)), minlength=len(order)) for name, v in amounts.items()}
    return {"cell": order, **summed}, {"cell": "cell"}, {"kind": "cells", "column": "cell", "rings": rings}


def place_columns(table, rows=None) -> tuple[dict, dict, dict | None]:
    """The columns, roles and places that put a table's rows on the map: to carry into a table answered about them."""
    roles = getattr(table, "roles", None) or {}
    pick = (lambda v: v) if rows is None else (lambda v: [v[i] for i in rows])
    columns, out_roles, places = {}, {}, None
    if roles.get("cell") and (getattr(table, "places", None) or {}).get("rings"):
        columns["cell"] = pick(table.raw(roles["cell"]))
        out_roles["cell"] = "cell"
        places = {"kind": "cells", "column": "cell", "rings": table.places["rings"]}
    if roles.get("lat") and roles.get("lon"):
        columns["lat"], columns["lon"] = pick(table.raw(roles["lat"])), pick(table.raw(roles["lon"]))
        out_roles.update(lat="lat", lon="lon")
    return columns, out_roles, places
