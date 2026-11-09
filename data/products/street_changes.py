"""Changes to a city's streets and what was counted about them, for urban science's interventions (ADR 0087).

A case is a recipe of three records and a network:
- the sensors, each with its outcome by period: the logarithm of a mean daily count;
- the interventions, each with its footprint (a polyline, or a point for a place) and the period from which it stands;
- the ways about the sensors, from Overture Maps' world tiles (data/acquisition/world_city.py, ODbL 1.0), in a local east-north-up
  frame at the sensors' centre.

Adding a city is adding a case. Nothing downstream (the domain's evaluate, the workbench's plug-in, the study) names one.

    melbourne-developments   what a major building completed does to the walkers counted near it. The City of Melbourne's counters
                             (city_tables.py's months, CC BY 4.0) taken to years over the months counted, 2009 to 2019 (the
                             pandemic's years changed the centre's walking too unevenly for parallel trends). A year needs six
                             counted months, a sensor three such years. The interventions are the Development Activity Monitor's
                             developments completed 2010 to 2019 with at least 20,000 m² of non-residential floor space, or 300
                             dwellings, 300 student beds or 200 hotel rooms (a declared threshold for "major"), standing from
                             their year of completion. Walkers use every way but motorways and rails.
    newyork-lanes            what a protected cycle lane does to the riders counted on it and near it. NYC DOT's bicycle counts
                             (data/registry/nyc-bicycle-counts.json), 15-minute counts each way summed by the portal into months
                             a sensor and year at a time; a month's mean daily count is its total over the share of its quarter
                             hours present, where at least 90% are; a sensor needs 24 such months; January 2013 to June 2026. The
                             interventions are the Bike Routes' segments of class I (a protected lane or path) installed within the
                             panel, standing from the month of `instdate`: for a segment upgraded to a protected lane, read as the
                             date of the protected lane (an assumption; the dataset keeps a previous identifier, not a date).
                             Retirements are not modelled. Riders use every way but motorways, rails, footways, sidewalks and steps.

    python data/products/street_changes.py <case>     → data/cache/products/street-changes/<case>.json (it carries ways: ODbL), and
                                                         street-changes/index.json, the cases built and what each holds
"""

from __future__ import annotations

import calendar
import csv
import json
import math
import sys
import time
import urllib.parse
import urllib.request
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(ROOT / "data" / "acquisition"))
sys.path.insert(0, str(ROOT / "data"))

CACHE = ROOT / "data" / "cache"
OUT = CACHE / "products" / "street-changes"

# WGS 84 (NIMA TR8350.2).
A = 6378137.0
F = 1 / 298.257223563
E2 = F * (2 - F)


def ecef(lat, lon):
    p, l = math.radians(lat), math.radians(lon)
    n = A / math.sqrt(1 - E2 * math.sin(p) ** 2)
    return (n * math.cos(p) * math.cos(l), n * math.cos(p) * math.sin(l), n * (1 - E2) * math.sin(p))


def enu(lat, lon, lat0, lon0):
    """East and north (m) of a place in the frame tangent at (lat0, lon0), exactly on WGS 84."""
    x, y, z = ecef(lat, lon)
    x0, y0, z0 = ecef(lat0, lon0)
    dx, dy, dz = x - x0, y - y0, z - z0
    p, l = math.radians(lat0), math.radians(lon0)
    return (-math.sin(l) * dx + math.cos(l) * dy, -math.sin(p) * math.cos(l) * dx - math.sin(p) * math.sin(l) * dy + math.cos(p) * dz)


def tile_of(lat, lon, z=14):
    n = 2 ** z
    return int((lon + 180) / 360 * n), int((1 - math.asinh(math.tan(math.radians(lat))) / math.pi) / 2 * n)


def tiles_about(lat, lon, km, z=14):
    """The tiles of level z (Web Mercator) meeting a box of half-side km about a place."""
    dlat, dlon = km / 111.32, km / (111.32 * math.cos(math.radians(lat)))
    x0, y0 = tile_of(lat + dlat, lon - dlon, z)
    x1, y1 = tile_of(lat - dlat, lon + dlon, z)
    return {(z, x, y) for x in range(x0, x1 + 1) for y in range(y0, y1 + 1)}


def ways_about(places, km, left_out, lat0, lon0):
    """The ways of the world's tiles within km of any of the places (lat, lon), their classes in left_out and rails left out."""
    import world_city
    tiles = set()
    for lat, lon in places:
        tiles |= tiles_about(lat, lon, km)
    ways = []
    for z, x, y in sorted(tiles):
        tile = json.loads(world_city.city_tile(z, x, y))
        for cls, _width, _name, flat, rail in tile["ways"]:
            if rail or cls in left_out:
                continue
            xy = []
            for i in range(0, len(flat), 2):
                e, n = enu(flat[i + 1], flat[i], lat0, lon0)
                xy += [round(e, 2), round(n, 2)]
            ways.append(xy)
    return ways, len(tiles)


def number(v):
    try:
        return float(v or 0)
    except ValueError:
        return 0.0


# ---- Melbourne: major developments and walkers ----------------------------------------------------------------------------------

MEL_FIRST, MEL_LAST = 2009, 2019
NONRES = ["office_flr", "retail_flr", "industrial_flr", "storage_flr", "education_flr", "hospital_flr", "recreation_flr", "publicdispaly_flr", "community_flr"]
MAJOR = {"nonres_m2": 20000, "dwellings": 300, "student_beds": 300, "hotel_rooms": 200}
WALK_LEFT_OUT = {"motorway", "motorway_link"}


def melbourne_developments():
    table = CACHE / "products" / "tables" / "melbourne-counters-month.json"
    if not table.exists():
        import city_tables
        city_tables.main()
    t = json.load(open(table, encoding="utf-8"))
    d, at = t["data"], t["places"]["at"]
    months = defaultdict(list)
    for c, yr, v, days in zip(d["counter"], d["year"], d["walkers_a_day"], d["days"]):
        if MEL_FIRST <= yr <= MEL_LAST and v and v > 0:
            months[c].append((yr, v, days))
    ids = sorted((c for c in months if c in at), key=lambda c: int(c))
    lat0 = sum(at[c][1] for c in ids) / len(ids)
    lon0 = sum(at[c][0] for c in ids) / len(ids)
    sensors, observations = [], []
    for c in ids:
        by_year = defaultdict(list)
        for yr, v, days in months[c]:
            by_year[yr].append((v, days))
        years = {yr: sum(v * n for v, n in vs) / sum(n for _, n in vs) for yr, vs in by_year.items() if len(vs) >= 6}
        if len(years) < 3:
            continue
        k = len(sensors)
        lon, lat = at[c]
        e, n = enu(lat, lon, lat0, lon0)
        sensors.append({"id": c, "name": t["places"]["names"].get(c, c), "lat": lat, "lon": lon, "x": round(e, 2), "y": round(n, 2)})
        for yr in sorted(years):
            observations.append([k, yr, round(math.log(years[yr]), 6)])
    footprints = []
    rows = csv.DictReader(open(CACHE / "melbourne-development-activity" / "custom-1a4dca7e" / "developments.csv", encoding="utf-8-sig"), delimiter=";")
    for r in rows:
        if r["status"] != "COMPLETED" or not r["year_completed"]:
            continue
        year = int(r["year_completed"])
        if not (MEL_FIRST < year <= MEL_LAST):
            continue
        nonres = sum(number(r[k]) for k in NONRES)
        if not (nonres >= MAJOR["nonres_m2"] or number(r["resi_dwellings"]) >= MAJOR["dwellings"] or number(r["student_beds"]) >= MAJOR["student_beds"]
                or number(r["hotel_rooms"]) >= MAJOR["hotel_rooms"]):
            continue
        try:
            lat, lon = float(r["latitude"]), float(r["longitude"])
        except ValueError:
            continue
        e, n = enu(lat, lon, lat0, lon0)
        kind = "offices" if nonres >= MAJOR["nonres_m2"] and number(r["office_flr"]) >= 0.5 * nonres else "homes" if number(r["resi_dwellings"]) >= MAJOR["dwellings"] else "mixed"
        footprints.append({"xy": [round(e, 2), round(n, 2)], "ll": [round(lon, 6), round(lat, 6)], "begin": year, "label": r["street_address"], "kind": kind, "lat": lat, "lon": lon,
                           "nonres_m2": round(nonres), "dwellings": int(number(r["resi_dwellings"]))})
    ways, tiles = ways_about([(s["lat"], s["lon"]) for s in sensors], 1.5, WALK_LEFT_OUT, lat0, lon0)
    return {"case": "melbourne-developments", "city": "Melbourne", "question": "what a major building completed does to the walkers counted near it",
            "outcome": "walkers a day (logarithm)", "period": "year", "periods": list(range(MEL_FIRST, MEL_LAST + 1)), "bounds": [200, 600],
            "origin": {"lat": lat0, "lon": lon0}, "sensors": sensors, "observations": observations, "footprints": footprints, "ways": ways, "tiles": tiles,
            "source": "City of Melbourne: Pedestrian Counting System and Development Activity Monitor (CC BY 4.0); ways, Overture Maps (ODbL 1.0)"}


# ---- New York: protected lanes and riders ---------------------------------------------------------------------------------------

NY_PORTAL = "https://data.cityofnewyork.us/resource/"
NY_FIRST, NY_LAST = (2013, 1), (2026, 6)
NY_MIN_SHARE, NY_MIN_MONTHS = 0.9, 24
RIDE_LEFT_OUT = {"motorway", "motorway_link", "footway", "sidewalk", "steps", "crosswalk", "pedestrian", "bridleway"}
NY_CACHE = CACHE / "nyc-bicycle-counts"


def month(y, m):
    return y * 12 + (m - 1)


def _get(url, attempts=4):
    for a in range(attempts):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "Noetarium data library"})
            with urllib.request.urlopen(req, timeout=300) as r:
                return json.loads(r.read().decode("utf-8"))
        except Exception as e:  # noqa: BLE001
            if a + 1 == attempts:
                raise
            print("retrying", url[:100], e, file=sys.stderr)
            time.sleep(5 * (a + 1))


def _kept(path: Path, make):
    if path.exists():
        return json.load(open(path, encoding="utf-8"))
    value = make()
    path.parent.mkdir(parents=True, exist_ok=True)
    json.dump(value, open(path, "w", encoding="utf-8"))
    return value


def _monthly(sensor, year):
    def make():
        where = f"sensor_id='{sensor}' AND travelmode='bike' AND timestamp between '{year}-01-01T00:00:00' and '{year}-12-31T23:59:59'"
        q = {"$select": "date_trunc_ym(timestamp) as m, sum(counts) as n, count(*) as k, count(distinct flowid) as f", "$where": where, "$group": "m",
             "$order": "m", "$limit": "100"}
        return _get(NY_PORTAL + "ct66-47at.json?" + urllib.parse.urlencode(q))
    return _kept(NY_CACHE / "monthly" / f"{sensor}-{year}.json", make)


def newyork_lanes():
    sensors_all = _kept(NY_CACHE / "sensors.json", lambda: _get(NY_PORTAL + "6up2-gnw8.json?$limit=1000"))
    bikes = [s for s in sensors_all if "bike" in (s.get("travelmodes") or "") and s.get("lat") and s.get("lon") and s.get("firstdata")]
    jobs = [(s["id"], y) for s in bikes for y in range(max(NY_FIRST[0], int(s["firstdata"][:4])), min(NY_LAST[0], int((s.get("lastdata") or "2026")[:4])) + 1)]
    with ThreadPoolExecutor(4) as pool:
        rows = dict(zip(jobs, pool.map(lambda j: _monthly(*j), jobs)))
    periods = list(range(month(*NY_FIRST), month(*NY_LAST) + 1))
    lat0 = sum(float(s["lat"]) for s in bikes) / len(bikes)
    lon0 = sum(float(s["lon"]) for s in bikes) / len(bikes)
    sensors, observations = [], []
    for s in bikes:
        good = []
        for (sid, _y), rs in rows.items():
            if sid != s["id"]:
                continue
            for r in rs:
                if "m" not in r:
                    continue
                yy, mm = int(r["m"][:4]), int(r["m"][5:7])
                t = month(yy, mm)
                if not (periods[0] <= t <= periods[-1]):
                    continue
                n, k, f = float(r["n"]), float(r["k"]), max(1.0, float(r.get("f") or 1))
                share = k / (calendar.monthrange(yy, mm)[1] * 96 * f)
                if share >= NY_MIN_SHARE and n > 0:
                    good.append((t, math.log(n / (share * calendar.monthrange(yy, mm)[1]))))
        if len(good) < NY_MIN_MONTHS:
            continue
        k = len(sensors)
        e, n = enu(float(s["lat"]), float(s["lon"]), lat0, lon0)
        sensors.append({"id": s["id"], "name": s.get("name"), "lat": float(s["lat"]), "lon": float(s["lon"]), "x": round(e, 2), "y": round(n, 2)})
        observations += [[k, t, round(v, 6)] for t, v in sorted(good)]
    routes = _kept(NY_CACHE / "routes.geojson", lambda: _get(NY_PORTAL + "mzxg-pwib.geojson?$limit=60000&$where=" + urllib.parse.quote("facilitycl='I'")))
    footprints = []
    for f in routes["features"]:
        p, g = f["properties"], f.get("geometry")
        d = p.get("instdate")
        if not d or g is None:
            continue
        t = month(int(d[:4]), int(d[5:7]))
        if not (periods[0] < t <= periods[-1]):
            continue
        for part in (g["coordinates"] if g["type"] == "MultiLineString" else [g["coordinates"]]):
            if len(part) < 2:
                continue
            xy, ll = [], []
            for lon, lat in part:
                e, n = enu(lat, lon, lat0, lon0)
                xy += [round(e, 2), round(n, 2)]
                ll += [round(lon, 6), round(lat, 6)]
            footprints.append({"xy": xy, "ll": ll, "begin": t, "label": f"{p.get('street')}, {p.get('fromstreet')} to {p.get('tostreet')}", "kind": p.get("ft_facilit") or "Protected",
                               "lat": part[0][1], "lon": part[0][0]})
    ways, tiles = ways_about([(s["lat"], s["lon"]) for s in sensors], 2.5, RIDE_LEFT_OUT, lat0, lon0)
    return {"case": "newyork-lanes", "city": "New York", "question": "what a protected cycle lane does to the riders counted on it and near it",
            "outcome": "riders a day (logarithm)", "period": "month (year × 12 + month − 1)", "periods": periods, "bounds": [100, 600, 1500],
            "origin": {"lat": lat0, "lon": lon0}, "sensors": sensors, "observations": observations, "footprints": footprints, "ways": ways, "tiles": tiles,
            "source": "NYC DOT on NYC Open Data: Bicycle and Pedestrian Counts and Count Sensors, Bike Routes; ways, Overture Maps (ODbL 1.0)"}


CASES = {"melbourne-developments": melbourne_developments, "newyork-lanes": newyork_lanes}


def build(case: str) -> Path:
    if case not in CASES:
        raise ValueError(f"no case {case!r}: {', '.join(CASES)}")
    started = time.time()
    record = CASES[case]()
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{case}.json"
    json.dump(record, open(path, "w", encoding="utf-8"), separators=(",", ":"))
    # The index of the cases built, for those who list them without reading their ways.
    index_path = OUT / "index.json"
    index = json.load(open(index_path, encoding="utf-8")) if index_path.exists() else {}
    index[case] = {k: record[k] for k in ("case", "city", "question", "outcome", "period", "bounds", "origin", "source")}
    index[case].update({"sensors": len(record["sensors"]), "observations": len(record["observations"]), "footprints": len(record["footprints"]),
                        "first": record["periods"][0], "last": record["periods"][-1], "file": f"street-changes/{case}.json"})
    json.dump(index, open(index_path, "w", encoding="utf-8"), indent=1, ensure_ascii=False)
    print(f"{case}: {len(record['sensors'])} sensors, {len(record['observations'])} observations, {len(record['footprints'])} footprints, "
          f"{len(record['ways'])} ways from {record['tiles']} tiles, in {time.time() - started:.0f} s")
    return path


if __name__ == "__main__":
    for c in (sys.argv[1:] or list(CASES)):
        build(c)
