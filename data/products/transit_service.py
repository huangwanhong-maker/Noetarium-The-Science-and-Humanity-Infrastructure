"""Public transport's stops and their service for a place, from a GTFS Schedule feed: each stop with its modes and its departures on a
declared weekday, for the urban-science domain's `transit` (ADR 0151), which reads frequency, waits and SDG 11.2.1's access from them.

The first case is Melbourne, from the Department of Transport and Planning's GTFS Schedule (CC BY 4.0), a zip of one feed for each
mode: 2 metropolitan trains, 3 trams, 4 metropolitan buses (others, regional and interstate, are not needed for a city's centre). Each
inner feed is read from the outer archive by range requests and kept in the cache; nothing is committed.

    python data/products/transit_service.py [--place melbourne] [--date YYYYMMDD]

writes data/cache/products/transit/<place>.json:
    {schema, credit, source, place, box, date, window: [start, end] seconds, stops: [{id, name, lat, lon, station, departures: {mode: [s, …]}}]}

A stop within its parent station is kept as a platform, with the station named; platforms of one station are not merged here, since
the domain reads access from each platform's place. The date is the first Wednesday at least seven days after the feed begins
unless one is given; departures are those from 06:00 to 22:00 of its service day.
"""

from __future__ import annotations

import argparse
import datetime as dt
import io
import json
import sys
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "acquisition"))
sys.path.insert(0, str(HERE.parent / "formats"))
sys.path.insert(0, str(HERE))
import gtfs  # noqa: E402
import remotezip  # noqa: E402
from product_index import OUT as PRODUCTS, register  # noqa: E402

CACHE = HERE.parent / "cache" / "transit-feeds"
PLACES = {
    "melbourne": {
        "title": "Melbourne's centre and inner suburbs",
        "url": "https://opendata.transport.vic.gov.au/dataset/3f4e292e-7f8a-4ffe-831f-1953be0fe448/resource/8af7737c-5c9e-4022-aa98-911aa04370a7/download/gtfs.zip",
        "members": {"2": "metropolitan trains", "3": "trams", "4": "metropolitan buses"},
        "box": [-37.86, 144.88, -37.77, 145.02],  # S, W, N, E: the City of Melbourne and a margin of about 1.5 km
        "credit": "Department of Transport and Planning, Victoria: GTFS Schedule (CC BY 4.0)",
    },
}
WINDOW = (6 * 3600, 22 * 3600)


def feeds(place: str) -> dict[str, Path]:
    P = PLACES[place]
    out = {}
    folder = CACHE / place
    folder.mkdir(parents=True, exist_ok=True)
    outer = None
    for member, label in P["members"].items():
        path = folder / f"{member}.zip"
        if not path.exists():
            if outer is None:
                outer = zipfile.ZipFile(remotezip.RemoteFile(P["url"]))
            print(f"  {label}: reading {member}/google_transit.zip", flush=True)
            path.write_bytes(outer.read(f"{member}/google_transit.zip"))
        out[member] = path
    return out


def first_wednesday(feed: zipfile.ZipFile) -> dt.date:
    lo, _ = gtfs.span(feed)
    d = dt.datetime.strptime(lo, "%Y%m%d").date() + dt.timedelta(days=7)
    while d.weekday() != 2:
        d += dt.timedelta(days=1)
    return d


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--place", default="melbourne", choices=sorted(PLACES))
    ap.add_argument("--date", default="")
    a = ap.parse_args()
    P = PLACES[a.place]
    S, W, N, E = P["box"]
    paths = feeds(a.place)
    day = dt.datetime.strptime(a.date, "%Y%m%d").date() if a.date else None
    stops: dict[str, dict] = {}
    for member, path in paths.items():
        feed = zipfile.ZipFile(path)
        if day is None:
            day = first_wednesday(feed)
        services = gtfs.services_on(feed, day)
        R = gtfs.routes(feed)
        trips = gtfs.trips_of(feed, services)
        here = [s for s in gtfs.stops(feed) if s["location_type"] == 0 and S <= s["lat"] <= N and W <= s["lon"] <= E]
        names = {s["id"]: s["name"] for s in gtfs.stops(feed) if s["location_type"] == 1}
        deps = gtfs.departures(feed, trips, {s["id"] for s in here}, *WINDOW)
        for s in here:
            d = deps.get(s["id"])
            if not d:
                continue
            key = f"{member}:{s['id']}"
            by_mode: dict[str, list[int]] = {}
            for t, route in d:
                by_mode.setdefault(R[route]["mode"], []).append(t)
            stops[key] = {"id": key, "name": s["name"], "lat": round(s["lat"], 6), "lon": round(s["lon"], 6), "station": names.get(s["parent"], ""),
                          "departures": by_mode}
        print(f"  {P['members'][member]}: {len(services)} services on {day}, {sum(1 for k in stops if k.startswith(member + ':'))} stops served within the box", flush=True)
    out = {"schema": "transit-service/1", "credit": P["credit"], "source": P["url"], "place": a.place, "title": P["title"], "box": P["box"],
           "date": day.isoformat(), "window": list(WINDOW), "stops": list(stops.values())}
    (PRODUCTS / "transit").mkdir(parents=True, exist_ok=True)
    (PRODUCTS / "transit" / f"{a.place}.json").write_text(json.dumps(out, ensure_ascii=False) + "\n", encoding="utf-8")
    register("transit", {"name": a.place, "title": f"Public transport's stops and service: {P['title']}", "file": f"transit/{a.place}.json", "credit": P["credit"]})
    print(f"transit/{a.place}.json: {len(stops)} stops with service on {day}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
