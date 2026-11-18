"""The risks of a place, for scripts in Python and the works' servers (ADR 0129): which roads matter most to reaching care under a
place's floods, who stays within reach of a station, how a place's people walk out of a hazard's reach, and how its losses are
financed; asked of the engine a work has built around ``api/cpp/science/risk.hpp``, which documents the requests (criticality,
response, evacuation, financing).

    from science import risk
    P = risk.place(8.4757, -13.2332, km=4.5)           # ways, buildings and people about a place, in a frame centred there
    risk.ask("criticality", ways=risk.drive_ways(P), frame=P["frame"], people=P["people"], services=risk.care(P),
             hazard={"maps": risk.hazard_maps("freetown")["maps"]}, curve="vehicles")

The engine is a program built by a work: the Urban Studio's plug-in *Lifelines and evacuation* builds
``data/cache/engines/urban-studio/risk/engine``; ``NOETARIUM_RISK_ENGINE`` names another. Hazard maps' paths are read under the
data library's products, the engine's data root.

``place`` gathers what a request needs and is no computation of the foundations': Overture's ways and buildings from the data
library's city tiles (data/acquisition/world_city.py), GHS-POP's people (data/acquisition/services.py) shared among the buildings
of each cell by floor space and gathered on cells of 50 m. The frame is the engine's: WGS 84's radii of curvature at the centre.
Speeds by class of road and the classes closed to walkers are declarations, said here.
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ROOT = Path(__file__).resolve().parents[3]
PRODUCTS = ROOT / "data" / "cache" / "products"
ENGINE = Engine("urban-studio/risk", env="NOETARIUM_RISK_ENGINE")

KMH = 1 / 3.6
DRIVE = {"motorway": 90, "motorway_link": 50, "trunk": 70, "trunk_link": 40, "primary": 50, "primary_link": 35, "secondary": 40, "secondary_link": 30,
         "tertiary": 35, "tertiary_link": 25, "residential": 25, "unclassified": 25, "living_street": 10, "service": 15}  # km/h, declared
NO_FOOT = {"motorway", "motorway_link", "trunk", "trunk_link"}
CARE = {"hospital", "clinic", "doctors", "medical", "healthcare"}


def ask(op: str, timeout: float = 3600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason. Maps' paths are read under the data library's
    products, which the engine is given as its data root."""
    return ENGINE.ask(op, timeout=timeout, **request)


class Frame:
    """The engine's frame: metres east and north of a centre, with WGS 84's radii of curvature there."""

    def __init__(self, lat0: float, lon0: float):
        a, f = 6378137.0, 1 / 298.257223563
        e2, s = f * (2 - f), math.sin(math.radians(lat0))
        self.lat0, self.lon0 = lat0, lon0
        self.m = a * (1 - e2) / (1 - e2 * s * s) ** 1.5
        self.n = a / math.sqrt(1 - e2 * s * s)

    def local(self, lat: float, lon: float) -> tuple[float, float]:
        return (math.radians(lon - self.lon0) * self.n * math.cos(math.radians(self.lat0)), math.radians(lat - self.lat0) * self.m)

    def geo(self, x: float, y: float) -> tuple[float, float]:
        return (self.lat0 + math.degrees(y / self.m), self.lon0 + math.degrees(x / (self.n * math.cos(math.radians(self.lat0)))))


def place(lat: float, lon: float, km: float = 3.0) -> dict:
    """Ways, buildings and people within km of a place (at most 6), in the frame centred there:
    {"frame": {lat, lon}, "F": Frame, "ways": [{cls, width, xy}], "buildings": [{x, y, footprint, height, floor, kind, name}],
     "people": [[x, y, n]], "total": people}."""
    sys.path.insert(0, str(ROOT / "data" / "acquisition"))
    import services  # noqa: E402
    import world_city  # noqa: E402

    km = min(max(float(km), 0.5), 6.0)
    F = Frame(lat, lon)
    z, n = 14, 2 ** 14
    dlat, dlon = km / 111.32, km / (111.32 * math.cos(math.radians(lat)))
    tx = lambda lo: int((lo + 180) / 360 * n)  # noqa: E731
    ty = lambda la: int((1 - math.asinh(math.tan(math.radians(la))) / math.pi) / 2 * n)  # noqa: E731
    ways, buildings = [], []
    for x in range(tx(lon - dlon), tx(lon + dlon) + 1):
        for y in range(ty(lat + dlat), ty(lat - dlat) + 1):
            t = json.loads(world_city.city_tile(z, x, y))
            for cls, width, _name, flat, rail in t["ways"]:
                if rail:
                    continue
                pts = [F.local(flat[i + 1], flat[i]) for i in range(0, len(flat), 2)]
                ways.append({"cls": cls, "width": width, "xy": [round(v, 1) for p in pts for v in p]})
            for b in t["buildings"]:
                ring = b[3]
                pts = [F.local(ring[i + 1], ring[i]) for i in range(0, len(ring), 2)]
                area = cx = cy = 0.0
                for i in range(len(pts)):
                    (x0, y0), (x1, y1) = pts[i], pts[(i + 1) % len(pts)]
                    c = x0 * y1 - x1 * y0
                    area += c
                    cx += (x0 + x1) * c
                    cy += (y0 + y1) * c
                if abs(area) < 1e-9:
                    continue
                h = b[0] if b[0] and b[0] > 0 else 3.2
                buildings.append({"x": cx / (3 * area), "y": cy / (3 * area), "footprint": abs(area) / 2, "height": h,
                                  "floor": abs(area) / 2 * max(1, round(h / 3.2)), "kind": str((b[6] if len(b) > 6 else None) or "").lower(),
                                  "name": (b[5] if len(b) > 5 else None) or ""})
    pop = services.population_window(lat, lon, km)
    dx, dy = pop["cell_degrees"]
    cells = {}
    for la, lo, people, _ in pop["cells"]:
        cells[(round(la / dy), round(lo / dx))] = {"la": la, "lo": lo, "n": people, "floor": 0.0}
    for b in buildings:
        la, lo = F.geo(b["x"], b["y"])
        b["cell"] = (round(la / dy), round(lo / dx))
        if b["cell"] in cells:
            cells[b["cell"]]["floor"] += b["floor"]
    bins: dict = {}

    def put(x: float, y: float, w: float) -> None:
        v = bins.setdefault((round(x / 50), round(y / 50)), [0.0, 0.0, 0.0])
        v[0] += x * w
        v[1] += y * w
        v[2] += w

    for b in buildings:
        c = cells.get(b["cell"])
        if c and c["floor"] > 0:
            put(b["x"], b["y"], c["n"] * b["floor"] / c["floor"])
    for c in cells.values():
        if not c["floor"] > 0 and c["n"] > 0:
            put(*F.local(c["la"], c["lo"]), c["n"])
    people = [[round(x / w), round(y / w), round(w, 1)] for x, y, w in bins.values() if w > 0.05]
    return {"frame": {"lat": lat, "lon": lon}, "F": F, "ways": ways, "buildings": buildings, "people": people,
            "total": round(sum(p[2] for p in people), 1)}


def drive_ways(P: dict) -> list:
    """The ways by road, [speed m/s, width m, x0, y0, …], at declared speeds by class."""
    return [[round(DRIVE[w["cls"]] * KMH, 3), w["width"]] + w["xy"] for w in P["ways"] if w["cls"] in DRIVE and len(w["xy"]) >= 4]


def walk_ways(P: dict, speed: float = 1.0) -> list:
    """The ways on foot at a speed, motorways and trunk roads closed to walkers."""
    return [[speed, w["width"]] + w["xy"] for w in P["ways"] if w["cls"] not in NO_FOOT and len(w["xy"]) >= 4]


def care(P: dict) -> list:
    """The buildings mapped as care: [x, y, floor, name]."""
    return [[round(b["x"]), round(b["y"]), round(b["floor"]), b["name"] or b["kind"]] for b in P["buildings"] if b["kind"] in CARE]


def tall(P: dict, least: float = 12.0, most: int = 80) -> list:
    """Tall buildings as candidate refuges: [x, y, capacity, name], the floors from the third up at a person a square metre (declared)."""
    out = [[round(b["x"]), round(b["y"]), round(b["footprint"] * max(0, round(b["height"] / 3.2) - 2)), b["name"] or f"a building of {round(b['height'])} m"]
           for b in P["buildings"] if b["height"] >= least]
    return [c for c in out if c[2] > 0][:most]


def hazard_maps(case: str) -> dict:
    """A place's hazard maps from the data library: 'freetown' (river floods at six return periods, the 100-year one its evacuation zone)
    or a Japanese municipality's code (its largest assumed flood, and its assumed tsunami as the zone), with its refuges and credit."""
    if case == "freetown":
        p = json.loads((PRODUCTS / "freetown-flood.json").read_text(encoding="utf-8"))
        maps = [{"return_period": m["return_period"], "path": m["file"], "box": m["box"], "width": m["width"], "height": m["height"],
                 "unit": p["depth_code"]["metres_per_unit"]} for m in p["maps"] if m["peril"] == "FLRF"]
        return {"maps": maps, "zone": next(m for m in maps if m["return_period"] == 100), "zone_name": "the 1-in-100-year river flood",
                "refuges": [], "events": p["events"], "credit": p["credit"], "centre": (8.4757, -13.2332)}
    path = PRODUCTS / "japan-hazards" / f"{case}.json"
    if not path.is_file():
        raise LookupError(f"no hazard maps for {case}: python data/products/japan_hazards.py --code {case}")
    p = json.loads(path.read_text(encoding="utf-8"))
    pick = lambda peril: next({"return_period": 0, "path": m["file"], "box": m["box"], "width": m["width"], "height": m["height"], "unit": m["unit"]}  # noqa: E731
                              for m in p["maps"] if m["peril"] == peril)
    s, w, n, e = p["box"]
    return {"maps": [pick("flood")], "zone": pick("tsunami"), "zone_name": "the assumed tsunami (津波浸水想定)",
            "refuges": [x for x in p["sites"] if "tsunami" in x["hazards"]], "events": None, "credit": p["credit"], "centre": ((s + n) / 2, (w + e) / 2)}
