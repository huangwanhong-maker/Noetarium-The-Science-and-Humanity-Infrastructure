"""Short-term rental listings of a city, from Inside Airbnb's quarterly snapshot (ADR 0163), written into
data/cache/products/platform-rentals/.

Each listing's area, room type, reviews in the last twelve months, minimum nights, price a night, host and the host's listings,
availability over the next year and place, from the detailed listings file. Inside Airbnb's data are used under the Creative Commons
Attribution 4.0 International licence, credited to Inside Airbnb as the source. A snapshot is what the platform showed on its date;
reviews are what guests left, not the nights booked, which a reading of the review rate turns into nights.

    python data/products/platform_rentals.py [--city prague] [--date 2026-09-20]
"""

from __future__ import annotations

import argparse
import csv
import gzip
import io
import json
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "platform-rentals" / "raw"
OUT = CACHE / "products" / "platform-rentals"
CITIES = {"prague": "czech-republic/prague/prague"}
TYPES = {"Entire home/apt": "entire", "Private room": "private", "Shared room": "shared", "Hotel room": "hotel"}
CREDIT = "Inside Airbnb (insideairbnb.com), CC BY 4.0"


def number(v: str):
    v = (v or "").replace("$", "").replace(",", "").strip()
    try:
        return float(v)
    except ValueError:
        return None


def build(city: str, date: str) -> dict:
    path = RAW / f"{city}-{date}-listings.csv.gz"
    if not path.is_file():
        RAW.mkdir(parents=True, exist_ok=True)
        url = f"https://data.insideairbnb.com/{CITIES[city]}/{date}/data/listings.csv.gz"
        with urllib.request.urlopen(urllib.request.Request(url, headers={"User-Agent": "noetarium-data/1"}), timeout=600) as r:
            path.write_bytes(r.read())
    rows = list(csv.DictReader(io.TextIOWrapper(gzip.open(path), encoding="utf-8", newline="")))
    listings = []
    for r in rows:
        listings.append([r["neighbourhood_cleansed"], TYPES.get(r["room_type"], "other"), number(r["number_of_reviews_ltm"]) or 0.0, number(r["minimum_nights"]),
                         number(r["price"]), r["host_id"], number(r["calculated_host_listings_count"]) or 1.0, number(r["availability_365"]),
                         number(r["latitude"]), number(r["longitude"])])
    return {"city": city, "date": date, "fields": ["area", "type", "reviews_ltm", "minimum_nights", "price", "host", "host_listings", "availability_365", "lat", "lon"],
            "listings": listings, "credit": CREDIT, "fetched": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--city", default="prague", choices=sorted(CITIES))
    ap.add_argument("--date", default="2026-09-20")
    a = ap.parse_args()
    d = build(a.city, a.date)
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{a.city}-{a.date}.json"
    path.write_text(json.dumps(d, ensure_ascii=False) + "\n", encoding="utf-8")
    types = {}
    for x in d["listings"]:
        types[x[1]] = types.get(x[1], 0) + 1
    print(f"{a.city} {a.date}: {len(d['listings']):,} listings {types}; wrote {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
