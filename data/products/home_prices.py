"""Homes sold, what they are and what is near them, for urban science's demand (ADR 0088): the records of a hedonic model of prices.

A case is a recipe: each home sold with its price, its characteristics and its distances to amenities, in a local east-north-up
frame. Adding a city is adding a case; nothing downstream names one.

    brooklyn-homes   NYC Department of Finance's annualized sales (NYC Open Data w2pb-icbu): every sale in Brooklyn in 2025 of a
                     one-, two- or three-family dwelling (building class categories 01–03) at more than $100,000, with its gross and
                     land floor area, its year built and its place; sales of a price under a tenth of the median, or of no floor area,
                     are left out (not at arm's length, or not a dwelling). Amenities:
                     - the subway: the straight distance to the nearest station of MTA Subway Stations (data.ny.gov 39hk-dx4f);
                     - parks: the straight distance to the edge of the nearest property of NYC Parks (enfh-gkve) of 1 hectare
                       (2.47 acres) or more, zero inside one;
                     - protected lanes: whether a segment of class I of the Bike Routes (mzxg-pwib) installed before the sale lies
                       within 300 m;
                     - the centre: the straight distance to Grand Central Terminal, Midtown Manhattan.

    python data/products/home_prices.py [case]    → data/cache/products/home-prices/<case>.json and home-prices/index.json
"""

from __future__ import annotations

import json
import math
import statistics
import sys
import time
import urllib.parse
import urllib.request
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(HERE))
from street_changes import enu  # noqa: E402

CACHE = ROOT / "data" / "cache"
OUT = CACHE / "products" / "home-prices"
NYC = "https://data.cityofnewyork.us/resource/"
GRAND_CENTRAL = (40.752726, -73.977229)


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


def number(v):
    try:
        return float(str(v).replace(",", ""))
    except (TypeError, ValueError):
        return 0.0


def segment_distance(px, py, ax, ay, bx, by):
    dx, dy = bx - ax, by - ay
    L2 = dx * dx + dy * dy
    t = 0.0 if L2 == 0 else max(0.0, min(1.0, ((px - ax) * dx + (py - ay) * dy) / L2))
    return math.hypot(px - ax - t * dx, py - ay - t * dy)


def inside(px, py, ring):
    c = False
    for i in range(len(ring)):
        (x0, y0), (x1, y1) = ring[i - 1], ring[i]
        if (y0 > py) != (y1 > py) and px < x0 + (py - y0) * (x1 - x0) / (y1 - y0):
            c = not c
    return c


class Grid:
    """Segments in cells of a size, for the nearest one within a radius."""

    def __init__(self, size):
        self.size, self.cells = size, {}

    def add(self, seg, payload):
        ax, ay, bx, by = seg
        n = max(1, int(math.hypot(bx - ax, by - ay) / (self.size / 2)))
        seen = set()
        for i in range(n + 1):
            x, y = ax + (bx - ax) * i / n, ay + (by - ay) * i / n
            k = (int(x // self.size), int(y // self.size))
            if k not in seen:
                self.cells.setdefault(k, []).append((seg, payload))
                seen.add(k)

    def near(self, x, y, rings=1):
        cx, cy = int(x // self.size), int(y // self.size)
        for i in range(cx - rings, cx + rings + 1):
            for j in range(cy - rings, cy + rings + 1):
                yield from self.cells.get((i, j), [])


def brooklyn_homes():
    where = "borough='3' AND (building_class_category like '01%' OR building_class_category like '02%' OR building_class_category like '03%') AND sale_price>100000 AND date_extract_y(sale_date)=2025"
    sales = _kept(CACHE / "nyc-property-sales" / "brooklyn-2025.json", lambda: _get(NYC + "w2pb-icbu.json?" + urllib.parse.urlencode({"$where": where, "$limit": "50000"})))
    stations = _kept(CACHE / "nyc-property-sales" / "subway-stations.json", lambda: _get("https://data.ny.gov/resource/39hk-dx4f.json?$limit=5000"))
    parks = _kept(CACHE / "nyc-property-sales" / "parks-brooklyn.geojson", lambda: _get(NYC + "enfh-gkve.geojson?" + urllib.parse.urlencode({"$where": "borough='B' AND acres>=2.47", "$limit": "5000"})))
    routes = json.load(open(CACHE / "nyc-bicycle-counts" / "routes.geojson", encoding="utf-8"))
    rows = [s for s in sales if s.get("latitude") and s.get("longitude") and number(s.get("gross_square_feet")) > 0 and number(s.get("year_built")) > 1800]
    median = statistics.median(number(s["sale_price"]) for s in rows)
    rows = [s for s in rows if number(s["sale_price"]) >= 0.1 * median]
    lat0 = sum(float(s["latitude"]) for s in rows) / len(rows)
    lon0 = sum(float(s["longitude"]) for s in rows) / len(rows)
    st = [enu(float(s["gtfs_latitude"]), float(s["gtfs_longitude"]), lat0, lon0) for s in stations if s.get("gtfs_latitude")]
    park_rings, grid_p = [], Grid(400)
    for f in parks["features"]:
        g = f.get("geometry")
        if not g:
            continue
        polys = g["coordinates"] if g["type"] == "MultiPolygon" else [g["coordinates"]]
        for poly in polys:
            ring = [enu(lat, lon, lat0, lon0) for lon, lat in poly[0]]
            park_rings.append(ring)
            for i in range(len(ring)):
                (ax, ay), (bx, by) = ring[i - 1], ring[i]
                grid_p.add((ax, ay, bx, by), len(park_rings) - 1)
    grid_l = Grid(300)
    for f in routes["features"]:
        g, d = f.get("geometry"), f["properties"].get("instdate")
        if not g or not d:
            continue
        for part in (g["coordinates"] if g["type"] == "MultiLineString" else [g["coordinates"]]):
            pts = [enu(lat, lon, lat0, lon0) for lon, lat in part]
            for i in range(1, len(pts)):
                grid_l.add((*pts[i - 1], *pts[i]), d[:10])
    gc = enu(*GRAND_CENTRAL, lat0, lon0)
    records = []
    for s in rows:
        x, y = enu(float(s["latitude"]), float(s["longitude"]), lat0, lon0)
        d_sub = min(math.hypot(x - a, y - b) for a, b in st)
        d_park = math.inf
        for rings in (1, 3, 8):
            for seg, k in grid_p.near(x, y, rings):
                d_park = min(d_park, segment_distance(x, y, *seg))
            if d_park < math.inf:
                break
        for k, ring in enumerate(park_rings):
            if d_park > 0 and abs(ring[0][0] - x) < 3000 and inside(x, y, ring):
                d_park = 0.0
        sold = s["sale_date"][:10]
        lane = any(segment_distance(x, y, *seg) <= 300 and inst < sold for seg, inst in grid_l.near(x, y, 1))
        cls = int(s["building_class_category"][:2])
        records.append([round(x, 1), round(y, 1), round(math.log(number(s["sale_price"])), 6), round(math.log(number(s["gross_square_feet"])), 6),
                        round(math.log(max(1.0, number(s.get("land_square_feet")))), 6), 2025 - int(number(s["year_built"])), cls,
                        round(math.log(max(50.0, d_sub)), 6), round(math.log(max(50.0, d_park)), 6), 1 if lane else 0, round(math.log(math.hypot(x - gc[0], y - gc[1])), 6),
                        int(sold[5:7]), float(s["latitude"]), float(s["longitude"])])
    columns = ["x", "y", "log_price", "log_floor", "log_land", "age", "class", "log_subway", "log_park", "lane_300m", "log_centre", "month", "lat", "lon"]
    return {"case": "brooklyn-homes", "city": "New York (Brooklyn)", "question": "what a home's price pays for: its rooms and land, and the subway, parks and protected lanes near it",
            "year": 2025, "origin": {"lat": lat0, "lon": lon0}, "columns": columns, "records": records, "median_price": median,
            "source": "NYC Department of Finance, annualized sales (NYC Open Data); MTA Subway Stations (data.ny.gov); NYC Parks Properties; NYC DOT Bike Routes"}


CASES = {"brooklyn-homes": brooklyn_homes}


def build(case: str) -> Path:
    started = time.time()
    record = CASES[case]()
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{case}.json"
    json.dump(record, open(path, "w", encoding="utf-8"), separators=(",", ":"))
    index_path = OUT / "index.json"
    index = json.load(open(index_path, encoding="utf-8")) if index_path.exists() else {}
    index[case] = {k: record[k] for k in ("case", "city", "question", "year", "origin", "columns", "source")}
    index[case].update({"homes": len(record["records"]), "file": f"home-prices/{case}.json"})
    json.dump(index, open(index_path, "w", encoding="utf-8"), indent=1, ensure_ascii=False)
    print(f"{case}: {len(record['records'])} homes in {time.time() - started:.0f} s")
    return path


if __name__ == "__main__":
    for c in (sys.argv[1:] or list(CASES)):
        build(c)
