"""GTFS Schedule feeds read by the standard library: the services running on a date, stops, routes and their modes, and each stop's
departures within a window of the day, streamed from stop_times.txt without holding it.

A feed is a zip archive (or an open ``zipfile.ZipFile``) holding the GTFS Schedule's text files (https://gtfs.org/documentation/
schedule/reference/). Times past midnight are kept as written (25:10:00 is 90,600 s), so that a service day's trips stay on that day.

    feed = zipfile.ZipFile("google_transit.zip")
    services = services_on(feed, datetime.date(2026, 11, 25))
    trips = trips_of(feed, services)
    stop_list = stops(feed)
    deps = departures(feed, trips, {s["id"] for s in stop_list}, 6 * 3600, 22 * 3600)

Route types follow the reference's basic codes (0 tram, 1 subway or metro, 2 rail, 3 bus, 4 ferry, 5 cable tram, 6 aerial lift,
7 funicular, 11 trolleybus, 12 monorail) and the extended codes' hundreds (100 rail, 400 urban rail, 700 bus, 900 tram, 1000 water,
1200 ferry, 1300 aerial lift, 1400 funicular), each mapped to a mode by ``mode_of``.
"""

from __future__ import annotations

import csv
import datetime as dt
import io
import zipfile
from collections import defaultdict

BASIC = {0: "tram", 1: "metro", 2: "rail", 3: "bus", 4: "ferry", 5: "cable tram", 6: "aerial lift", 7: "funicular", 11: "trolleybus", 12: "monorail"}
EXTENDED = {100: "rail", 200: "coach", 400: "metro", 700: "bus", 800: "trolleybus", 900: "tram", 1000: "ferry", 1100: "air", 1200: "ferry",
            1300: "aerial lift", 1400: "funicular", 1500: "taxi", 1700: "other"}


def mode_of(route_type: int) -> str:
    if route_type in BASIC:
        return BASIC[route_type]
    return EXTENDED.get(route_type // 100 * 100, "other")


def _rows(feed: zipfile.ZipFile, name: str):
    names = {n.rsplit("/", 1)[-1]: n for n in feed.namelist()}
    if name not in names:
        return
    with feed.open(names[name]) as raw:
        text = io.TextIOWrapper(raw, encoding="utf-8-sig", newline="")
        yield from csv.DictReader(text)


def seconds(t: str) -> int:
    h, m, s = (int(x) for x in t.strip().split(":"))
    return h * 3600 + m * 60 + s


def services_on(feed: zipfile.ZipFile, day: dt.date) -> set[str]:
    """The service_ids running on a date: calendar.txt's weekly pattern within its dates, then calendar_dates.txt's additions (1) and
    removals (2)."""
    weekday = ["monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"][day.weekday()]
    key = day.strftime("%Y%m%d")
    out = set()
    for r in _rows(feed, "calendar.txt"):
        if r["start_date"] <= key <= r["end_date"] and r.get(weekday, "0").strip() == "1":
            out.add(r["service_id"])
    for r in _rows(feed, "calendar_dates.txt"):
        if r["date"] == key:
            if r["exception_type"].strip() == "1":
                out.add(r["service_id"])
            elif r["exception_type"].strip() == "2":
                out.discard(r["service_id"])
    return out


def span(feed: zipfile.ZipFile) -> tuple[str, str] | None:
    """The first and last dates calendar.txt covers (YYYYMMDD), or None."""
    lo, hi = None, None
    for r in _rows(feed, "calendar.txt"):
        lo = r["start_date"] if lo is None or r["start_date"] < lo else lo
        hi = r["end_date"] if hi is None or r["end_date"] > hi else hi
    return (lo, hi) if lo else None


def routes(feed: zipfile.ZipFile) -> dict[str, dict]:
    return {r["route_id"]: {"type": int(r["route_type"]), "mode": mode_of(int(r["route_type"])), "name": r.get("route_short_name") or r.get("route_long_name") or ""}
            for r in _rows(feed, "routes.txt")}


def trips_of(feed: zipfile.ZipFile, services: set[str]) -> dict[str, str]:
    """trip_id → route_id for the trips of the services given."""
    return {r["trip_id"]: r["route_id"] for r in _rows(feed, "trips.txt") if r["service_id"] in services}


def stops(feed: zipfile.ZipFile) -> list[dict]:
    """Stops and stations: id, name, lat, lon, location_type (0 a stop or platform, 1 a station), parent station."""
    out = []
    for r in _rows(feed, "stops.txt"):
        if not r.get("stop_lat") or not r.get("stop_lon"):
            continue
        out.append({"id": r["stop_id"], "name": r.get("stop_name", ""), "lat": float(r["stop_lat"]), "lon": float(r["stop_lon"]),
                    "location_type": int(r.get("location_type") or 0), "parent": r.get("parent_station") or ""})
    return out


def departures(feed: zipfile.ZipFile, trips: dict[str, str], wanted: set[str], start: int, end: int) -> dict[str, list[tuple[int, str]]]:
    """Each wanted stop's departures within [start, end) seconds of the service day, as (time, route_id), from the trips given. A stop
    time without a departure takes its arrival; one with neither (an untimed stop) is left out."""
    out: dict[str, list[tuple[int, str]]] = defaultdict(list)
    for r in _rows(feed, "stop_times.txt"):
        route = trips.get(r["trip_id"])
        if route is None or r["stop_id"] not in wanted:
            continue
        t = (r.get("departure_time") or r.get("arrival_time") or "").strip()
        if not t:
            continue
        s = seconds(t)
        if start <= s < end:
            out[r["stop_id"]].append((s, route))
    for v in out.values():
        v.sort()
    return dict(out)
