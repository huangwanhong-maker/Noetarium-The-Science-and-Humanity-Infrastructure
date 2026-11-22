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
