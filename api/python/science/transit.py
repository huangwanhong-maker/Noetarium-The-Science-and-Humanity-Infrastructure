"""Public transport and who can reach it, for scripts in Python and the works' servers (ADR 0151): every stop's service at the peak and
SDG 11.2.1 under declared readings, asked of the engine a work has built around ``api/cpp/science/transit.hpp``, which documents the
request.

    from science import transit, risk
    P = risk.place(-37.8136, 144.9631, km=3)              # ways and people about a place, in a frame centred there
    S = transit.stops_near(transit.service("melbourne"), P)
    transit.ask("transit.access", ways=transit.walking(P), places=P["people"], stops=S)

The engine is the Urban Studio's plug-in *Public transport and who can reach it*: ``data/cache/engines/urban-studio/transit/engine``;
``NOETARIUM_TRANSIT_ENGINE`` names another. Stops and their departures come from the data library's ``transit_service.py`` (a GTFS
Schedule feed read for a declared weekday); ways and people from ``science.risk.place``.
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ROOT = Path(__file__).resolve().parents[3]
PRODUCTS = ROOT / "data" / "cache" / "products"
ENGINE = Engine("urban-studio/transit", env="NOETARIUM_TRANSIT_ENGINE")
NO_FOOT = {"motorway", "motorway_link", "trunk", "trunk_link"}


def ask(op: str, timeout: float = 1800.0, **request) -> dict:
    return ENGINE.ask(op, timeout=timeout, **request)


def service(place: str) -> dict:
    """A place's stops and their departures, as the data library made them."""
    path = PRODUCTS / "transit" / f"{place}.json"
    if not path.is_file():
        raise LookupError(f"no transit service for {place}: python data/products/transit_service.py --place {place}")
    return json.loads(path.read_text(encoding="utf-8"))


def walking(P: dict) -> list:
    """A place's ways on foot as coordinates, motorways and trunk roads closed to walkers."""
    return [w["xy"] for w in P["ways"] if w["cls"] not in NO_FOOT and len(w["xy"]) >= 4]


def stops_near(T: dict, P: dict, margin_m: float = 1500.0) -> list:
    """The product's stops in the place's frame, those within the place's reach and a margin."""
    F = P["F"]
    xs = [w["xy"][k] for w in P["ways"] for k in range(0, len(w["xy"]), 2)] or [0.0]
    ys = [w["xy"][k] for w in P["ways"] for k in range(1, len(w["xy"]), 2)] or [0.0]
    lo_x, hi_x, lo_y, hi_y = min(xs) - margin_m, max(xs) + margin_m, min(ys) - margin_m, max(ys) + margin_m
    out = []
    for s in T["stops"]:
        x, y = F.local(s["lat"], s["lon"])
        if lo_x <= x <= hi_x and lo_y <= y <= hi_y:
            out.append({"id": s["id"], "name": s["name"], "x": round(x, 1), "y": round(y, 1), "departures": s["departures"]})
    return out


# ---- open space (ADR 0154): science/openspace.hpp, answered by the same engine --------------------------------------------------

# Overture's land use and land cover read as kinds of open space, a declaration a caller may replace: parks and plazas are open to the
# public; pedestrian areas and sports grounds often are; green land, gardens and cemeteries may not be. A golf course is a kind of its own,
# since UN-Habitat's metadata removes recreation that charges a fee. What the base theme maps is potential open public space in the
# metadata's words: what is free and open to all is for validation on the ground.
SPACE_KINDS = {
    ("land_use", "park"): "park", ("land_use", "recreation", "park"): "park", ("land_use", "recreation", "playground"): "park",
    ("land_use", "recreation", "recreation_ground"): "park", ("land_use", "recreation", "dog_park"): "park", ("land_use", "recreation", "common"): "park",
    ("land_use", "recreation", "village_green"): "park", ("land_use", "pedestrian", "plaza"): "plaza", ("land_use", "pedestrian", "pedestrian"): "pedestrian area",
    ("land_use", "recreation", "pitch"): "sports", ("land_use", "recreation", "track"): "sports", ("land_use", "recreation", "stadium"): "sports",
    ("land_use", "recreation", "sports_centre"): "sports", ("land_use", "recreation", "golf_course"): "golf course",
    ("land_use", "managed", "grass"): "green", ("land_use", "agriculture", "meadow"): "green", ("land_use", "protected"): "green",
    ("land", "forest"): "green", ("land", "shrub"): "green", ("land", "grass"): "green", ("land", "wetland"): "green",
    ("land_use", "horticulture", "garden"): "garden", ("land_use", "cemetery"): "cemetery",
}


def kind_of(space: dict) -> str | None:
    """A polygon's kind of open space by SPACE_KINDS: by its layer, subtype and class, else by its layer and subtype; None if not open."""
    return SPACE_KINDS.get((space["layer"], space["subtype"], space["class"])) or SPACE_KINDS.get((space["layer"], space["subtype"]))


def open_land(P: dict, km: float) -> dict:
    """The open land within km of a place's centre, as kinds in its frame: {spaces: [{kind, ring, holes}], box: [x0, y0, x1, y1], counts}."""
    sys.path.insert(0, str(ROOT / "data" / "acquisition"))
    import world_city  # noqa: E402
    F, (lat, lon) = P["F"], (P["frame"]["lat"], P["frame"]["lon"])
    dlat, dlon = km / 111.32, km / (111.32 * math.cos(math.radians(lat)))
    raw = world_city.open_spaces({"south": lat - dlat, "north": lat + dlat, "west": lon - dlon, "east": lon + dlon})
    to_xy = lambda flat: [round(v, 1) for i in range(0, len(flat), 2) for v in F.local(flat[i + 1], flat[i])]  # noqa: E731
    spaces, counts = [], {}
    for s in raw["spaces"]:
        k = kind_of(s)
        if not k:
            continue
        counts[k] = counts.get(k, 0) + 1
        spaces.append({"kind": k, "ring": to_xy(s["ring"]), "holes": [to_xy(h) for h in s["holes"]]})
    half = km * 1000
    return {"spaces": spaces, "box": [-half, -half, half, half], "counts": counts, "credit": raw["credit"]}
