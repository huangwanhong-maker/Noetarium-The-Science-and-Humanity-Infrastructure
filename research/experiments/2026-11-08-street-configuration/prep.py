"""The inputs of the street-configuration study: Melbourne's street network from the city model, its pedestrian counters placed in
the model's frame, and each counter's mean daily count.

- The network. The city model's ways (data/cache/products/city-model-melbourne.json, OpenStreetMap, ODbL 1.0), in its exact local
  frame (WGS 84 to ECEF, rotated to east-north-up at its origin). Two networks are declared:
  * streets: the street centre lines, the network Space Syntax analyses: every highway class but footways, paths, steps,
    cycleways, motorways and their links, and busways (a footway in Melbourne's OpenStreetMap is mostly a pavement drawn beside its
    road, which would double the network);
  * walking: the streets and the footways, paths and steps, for a sensitivity analysis;
  * roads: the streets without service ways and tracks (Melbourne's laneways are mostly service ways), a second sensitivity
    analysis: a fine grid of lanes raises every segment's local integration alike.
- The counters. The City of Melbourne's Pedestrian Counting System (data/cache/melbourne-pedestrian-counts/current, CC BY 4.0):
  each sensor's position, in the same frame.
- The counts. Each counter's daily totals, both directions summed, over the days with all 24 hours counted; their mean, and the
  number of such days. A counter with fewer than 60 such days is left out.
- What draws walkers. The City's Census of Land Use and Employment (CLUE; data/cache/melbourne-clue-floor-space and -blocks, CC BY
  4.0): the floor space of every use in each block at the latest census, outdoor space, parks and uncovered parking left out, summed
  over the blocks whose centres lie within 250 m of the counter (a declared radius); and its non-residential part.

Writes results/inputs.json (not committed: it carries the ways, ODbL) and results/counts.json (committed).
Run from the repository's root: python research/experiments/2026-11-08-street-configuration/prep.py
"""

import csv
import io
import json
import math
import os
from collections import defaultdict

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
HERE = os.path.dirname(os.path.abspath(__file__))
MODEL = os.path.join(ROOT, "data", "cache", "products", "city-model-melbourne.json")
FLOOR = os.path.join(ROOT, "data", "cache", "melbourne-clue-floor-space", "all", "floor-space.csv")
BLOCKS = os.path.join(ROOT, "data", "cache", "melbourne-clue-blocks", "all", "blocks.geojson")
SENSORS = os.path.join(ROOT, "data", "cache", "melbourne-pedestrian-counts", "current", "sensors.csv")
COUNTS = os.path.join(ROOT, "data", "cache", "melbourne-pedestrian-counts", "current", "counts.csv")

LEFT_OUT = {"footway", "path", "steps", "cycleway", "motorway", "motorway_link", "busway"}
WALKING_ADDS = {"footway", "path", "steps"}
MIN_DAYS = 60
ROADS_LEAVE_OUT = {"service", "track"}
NOT_FLOOR = {"census_year", "block_id", "clue_small_area", "park_reserve", "private_outdoor_space", "square_promenade", "parking_commercial_uncovered",
             "parking_private_uncovered", "transport_storage_uncovered", "sports_and_recreation_outdoor"}
RESIDENTIAL = {"house_townhouse", "residential_apartment", "student_accommodation", "institutional_accommodation"}
ATTRACTION_RADIUS = 250.0

# WGS 84 (NIMA TR8350.2).
A = 6378137.0
F = 1 / 298.257223563
E2 = F * (2 - F)


def ecef(lat, lon):
    p, l = math.radians(lat), math.radians(lon)
    n = A / math.sqrt(1 - E2 * math.sin(p) ** 2)
    return (n * math.cos(p) * math.cos(l), n * math.cos(p) * math.sin(l), n * (1 - E2) * math.sin(p))


def enu(lat, lon, lat0, lon0):
    """East and north (m) of a point about an origin, the up component dropped: the city model's frame."""
    x, y, z = ecef(lat, lon)
    x0, y0, z0 = ecef(lat0, lon0)
    dx, dy, dz = x - x0, y - y0, z - z0
    p, l = math.radians(lat0), math.radians(lon0)
    east = -math.sin(l) * dx + math.cos(l) * dy
    north = -math.sin(p) * math.cos(l) * dx - math.sin(p) * math.sin(l) * dy + math.cos(p) * dz
    return east, north


def read_semicolon(path):
    with open(path, encoding="utf-8-sig", newline="") as f:
        return list(csv.DictReader(f, delimiter=";"))


def main():
    model = json.load(open(MODEL, encoding="utf-8"))
    lat0, lon0 = model["origin"]["lat"], model["origin"]["lon"]
    s_lat, w_lon, n_lat, e_lon = model["bbox"]
    corners = [enu(s_lat, w_lon, lat0, lon0), enu(n_lat, e_lon, lat0, lon0)]
    box = [corners[0][0], corners[0][1], corners[1][0], corners[1][1]]
    streets, walking, roads = [], [], []
    for road in model["roads"]:
        line = road.get("line") or []
        if len(line) < 2:
            continue
        flat = [round(c, 2) for p in line for c in p]
        kind = road.get("kind")
        if kind not in LEFT_OUT:
            streets.append(flat)
            walking.append(flat)
            if kind not in ROADS_LEAVE_OUT:
                roads.append(flat)
        elif kind in WALKING_ADDS:
            walking.append(flat)

    sensors = {}
    for row in read_semicolon(SENSORS):
        try:
            lat, lon = float(row["latitude"]), float(row["longitude"])
        except (TypeError, ValueError):
            continue
        x, y = enu(lat, lon, lat0, lon0)
        sensors[int(row["location_id"])] = {"id": int(row["location_id"]), "name": row["sensor_description"], "x": round(x, 2), "y": round(y, 2),
                                            "inside": box[0] <= x <= box[2] and box[1] <= y <= box[3],
                                            "edge_m": round(min(x - box[0], box[2] - x, y - box[1], box[3] - y), 1)}

    # Floor space by block at the latest census, and each block's centre in the frame.
    floor_rows = read_semicolon(FLOOR)
    latest = max(int(r["census_year"]) for r in floor_rows)
    floor, nonres = defaultdict(float), defaultdict(float)
    for r in floor_rows:
        if int(r["census_year"]) != latest:
            continue
        for k, v in r.items():
            if k in NOT_FLOOR or not v:
                continue
            try:
                a = float(v)
            except ValueError:
                continue
            floor[int(r["block_id"])] += a
            if k not in RESIDENTIAL:
                nonres[int(r["block_id"])] += a
    centres = {}
    for f in json.load(open(BLOCKS, encoding="utf-8"))["features"]:
        g = f["properties"].get("geo_point_2d")
        if g:
            centres[int(f["properties"]["block_id"])] = enu(g["lat"], g["lon"], lat0, lon0)
    for c in sensors.values():
        near = [b for b, (bx, by) in centres.items() if math.hypot(bx - c["x"], by - c["y"]) <= ATTRACTION_RADIUS]
        c["floor_m2"] = round(sum(floor[b] for b in near))
        c["floor_nonres_m2"] = round(sum(nonres[b] for b in near))
        c["blocks"] = len(near)

    hours = defaultdict(lambda: defaultdict(set))
    totals = defaultdict(lambda: defaultdict(float))
    first, last = None, None
    for row in read_semicolon(COUNTS):
        try:
            k, day, hour, count = int(row["location_id"]), row["sensing_date"], int(row["hourday"]), float(row["pedestriancount"])
        except (TypeError, ValueError, KeyError):
            continue
        hours[k][day].add(hour)
        totals[k][day] += count
        first = day if first is None or day < first else first
        last = day if last is None or day > last else last
    counts = []
    for k, days in totals.items():
        full = [t for d, t in days.items() if len(hours[k][d]) == 24]
        if len(full) < MIN_DAYS or k not in sensors:
            continue
        counts.append({**sensors[k], "days": len(full), "mean_daily": round(sum(full) / len(full), 1)})
    counts.sort(key=lambda c: c["id"])

    os.makedirs(os.path.join(HERE, "results"), exist_ok=True)
    with open(os.path.join(HERE, "results", "inputs.json"), "w", encoding="utf-8") as f:
        json.dump({"box": box, "networks": {"streets": streets, "walking": walking, "roads": roads},
                   "points": [{"id": c["id"], "x": c["x"], "y": c["y"]} for c in counts if c["inside"]]}, f)
    with open(os.path.join(HERE, "results", "counts.json"), "w", encoding="utf-8") as f:
        json.dump({"period": [first, last], "min_days": MIN_DAYS, "box": [round(b, 1) for b in box], "clue_year": latest, "attraction_radius_m": ATTRACTION_RADIUS,
                   "sources": {"counts": "City of Melbourne, Pedestrian Counting System (counts per hour per sensor), CC BY 4.0",
                               "floor space": "City of Melbourne, Census of Land Use and Employment, floor space by use by block, CC BY 4.0",
                               "network": "OpenStreetMap contributors, ODbL 1.0, through the data library's city model of Melbourne"},
                   "counters": counts}, f, indent=1)
    inside = sum(c["inside"] for c in counts)
    print(f"CLUE {latest}; streets: {len(streets)} ways; walking: {len(walking)} ways; roads: {len(roads)} ways; counters with {MIN_DAYS}+ full days: {len(counts)}, {inside} inside the model; "
          f"period {first} to {last}")


if __name__ == "__main__":
    main()
