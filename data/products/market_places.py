"""The places of a market in a city, for the markets domain and Agora (ADR 0092): who could buy, block by block, and every place of
business, in the frame of the city model, so that the minutes between them can be walked on its streets.

A city is a case of CASES. Melbourne's centre is the first:

    blocks    each CLUE block whose centroid lies within the city model's box, in the latest census year: its ring, area and centroid
              (metres east and north of the model's origin, and degrees), its dwellings, and its floor space of offices and of
              shops (m²). A cell the census withheld is null, not zero. The census counts dwellings and floor space, not people:
              how many residents a dwelling holds and how much floor a worker takes are assumptions a work declares, with ranges.
    places    every establishment of the census year within the box: its trading name, its ANZSIC class (four digits), its block and
              where it is. The census gives no size of an establishment (seats, floor space of its own): attraction is a work's to
              presume.
    kinds     the ANZSIC classes with at least five establishments, by their number.

Sources: the City of Melbourne's Census of Land Use and Employment (CLUE), CC BY 4.0, through the data library's datasets
melbourne-clue-establishments, melbourne-clue-blocks, melbourne-clue-floor-space and melbourne-clue-dwellings (the table
melbourne-blocks of city_tables.py); the frame is that of city-model-melbourne.json (street_records.py).

    python data/products/market_places.py
"""

from __future__ import annotations

import csv
import datetime as dt
import json
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

from product_index import OUT as PRODUCTS, register  # noqa: E402
from street_records import enu  # noqa: E402

CACHE = HERE.parents[0] / "cache"

CASES = {
    "melbourne": {
        "name": "Melbourne's centre",
        "model": "city-model-melbourne.json",
        "blocks": "tables/melbourne-blocks.json",
        "establishments": "melbourne-clue-establishments/all/establishments.csv",
        "currency": "A$",
        "credit": "City of Melbourne, Census of Land Use and Employment (CLUE), CC BY 4.0; streets © OpenStreetMap contributors, ODbL",
    },
}


def area(ring: list) -> float:
    """The area a ring encloses, in the square of its unit."""
    return abs(sum(x0 * y1 - x1 * y0 for (x0, y0), (x1, y1) in zip(ring, ring[1:] + ring[:1]))) / 2


def centroid(ring: list) -> tuple[float, float]:
    """The centroid of a polygon's area (its vertices' mean when it has none)."""
    a = cx = cy = 0.0
    for (x0, y0), (x1, y1) in zip(ring, ring[1:] + ring[:1]):
        cross = x0 * y1 - x1 * y0
        a += cross
        cx += (x0 + x1) * cross
        cy += (y0 + y1) * cross
    if abs(a) < 1e-9:
        return sum(p[0] for p in ring) / len(ring), sum(p[1] for p in ring) / len(ring)
    return cx / (3 * a), cy / (3 * a)


def case(key: str) -> dict | None:
    spec = CASES[key]
    model_path, blocks_path, places_path = PRODUCTS / spec["model"], PRODUCTS / spec["blocks"], CACHE / spec["establishments"]
    if not (model_path.exists() and blocks_path.exists() and places_path.exists()):
        return None
    model = json.loads(model_path.read_text(encoding="utf-8"))
    lat0, lon0 = model["origin"]["lat"], model["origin"]["lon"]
    south, west, north, east = model["bbox"]
    local = lambda lon, lat: enu(lat, lon, lat0, lon0)[:2]  # noqa: E731
    inside = lambda lon, lat: south <= lat <= north and west <= lon <= east  # noqa: E731

    table = json.loads(blocks_path.read_text(encoding="utf-8"))
    data, rings = table["data"], table["places"]["rings"]
    year = max(data["census_year"])
    blocks = []
    for k, (block_id, census_year) in enumerate(zip(data["block_id"], data["census_year"])):
        ring = rings.get(str(block_id))
        if census_year != year or not ring:
            continue
        lon, lat = centroid(ring)
        if not inside(lon, lat):
            continue
        x, y = local(lon, lat)
        outline = [[round(v, 1) for v in local(p[0], p[1])] for p in ring]
        blocks.append({
            "id": str(block_id), "small_area": data["small_area"][k], "x": round(x, 1), "y": round(y, 1), "lon": round(lon, 6), "lat": round(lat, 6),
            "ring": outline, "area_m2": round(area(outline)),
            "dwellings": data["dwellings"][k], "office_m2": data["office"][k], "retail_m2": data["retail"][k], "floor_m2": data["floor_total"][k],
        })

    places = {k: [] for k in ("name", "code", "block", "address", "x", "y", "lon", "lat")}
    kinds: Counter = Counter()
    names: dict = {}
    with open(places_path, encoding="utf-8-sig", newline="") as f:
        for row in csv.DictReader(f, delimiter=";"):
            if row["census_year"] != str(year) or not row["longitude"] or not row["latitude"]:
                continue
            lon, lat = float(row["longitude"]), float(row["latitude"])
            code = row["industry_anzsic4_code"].strip()
            if not inside(lon, lat) or not code:
                continue
            x, y = local(lon, lat)
            places["name"].append(row["trading_name"].strip())
            places["code"].append(code)
            places["block"].append(row["block_id"].strip())
            places["address"].append(row["business_address"].strip())
            places["x"].append(round(x, 1)), places["y"].append(round(y, 1))
            places["lon"].append(round(lon, 6)), places["lat"].append(round(lat, 6))
            kinds[code] += 1
            names[code] = row["industry_anzsic4_description"].strip()
    return {
        "product": "market-places", "id": key, "name": spec["name"], "built": dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "year": year, "currency": spec["currency"], "origin": model["origin"], "bbox": model["bbox"], "frame": model["frame"], "model": spec["model"],
        "blocks": blocks,
        "kinds": [{"code": code, "name": names[code], "count": n} for code, n in kinds.most_common() if n >= 5],
        "places": places, "credit": spec["credit"],
        "notes": ["The census counts dwellings and floor space, not people: residents per dwelling and floor per worker are a work's assumptions.",
                  "A cell the census withheld is null, not zero.", "The census gives no size of an establishment."],
    }


def main() -> int:
    made = 0
    for key in CASES:
        product = case(key)
        if product is None:
            print(key, "skipped: its city model, its blocks or its establishments are not in the cache", flush=True)
            continue
        path = PRODUCTS / f"market-{key}.json"
        path.write_text(json.dumps(product, ensure_ascii=False, separators=(",", ":")), encoding="utf-8")
        print(key, product["year"], len(product["blocks"]), "blocks", len(product["places"]["name"]), "places", len(product["kinds"]), "kinds", path.stat().st_size // 1024, "kB", flush=True)
        register("markets", {"name": key, "title": product["name"], "file": path.name, "year": product["year"], "places": len(product["places"]["name"]),
                             "blocks": len(product["blocks"]), "credit": product["credit"]})
        made += 1
    return 0 if made else 1


if __name__ == "__main__":
    sys.exit(main())
