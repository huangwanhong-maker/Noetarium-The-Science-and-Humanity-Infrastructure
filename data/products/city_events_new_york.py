"""Build the record of a district's dated facts for the Urban Science Workbench's Event Spacetime (ADR 0067, ADR 0171): New
York's Financial District, 2008-2024, from the City of New York's open data, in the form of city_events.py's record of Melbourne.

The record holds facts, not a complex: the dated events the sources record, each with its type, its time label at its own
resolution, its match and its source. The urban-science domain's C++ builds the couplings, primitives and footprints from the city
model; no event is inferred here, and every event is "recorded".

1. **The district.** The workbench's area new-york (applications/urban-science-workbench/areas.json, its district: the Financial
   District's old streets about Wall Street, Stone Street and Hanover Square), its corners carried into the city model's local frame
   and widened by MARGIN_M on every side, as city_events.district_box does for Melbourne.
2. **Buildings.** The city model's (Valdrada's compiled city of New York), joined by position: a source's point to the footprint that
   holds it, else the nearest outline within JOIN_M (city_events.Footprints).
3. **Events.** Each with a time label [from, to): a year is 1 January to the next 1 January, a day that day to the next. Kept when
   the label begins in the window, 2008-01-01 to 2025-01-01. Recorded:
   - births (kind "constructed"): the city's building footprints (5zhs-2jue) whose construction year lies in the window, at that
     year, matched by the footprint's centroid;
   - completions (kind "development completed"): the first certificate of occupancy of each new building's job (DOB's
     Certificates of Occupancy, bs8b-p36w, and DOB NOW's, pkdm-hqz6, a job counted once), matched by its point; a birth, which the
     engine keeps as an alteration when the building has an earlier account of its making (detail.same_as names a footprint's birth
     within a year);
   - alterations (kind "major alteration completed"): the first certificate of occupancy of each major alteration's job (A1,
     ALTERATION TYPE 1, Alteration CO);
   - landmarks (institution, kind "landmark designated"): the Landmarks Preservation Commission's individual landmark sites
     (buis-pvji) designated in the window, matched by their point;
   - permitted events (regime, kind the event's type): the Street Activity Permit Office's and other agencies' permitted events in
     Manhattan's community district 1 (bkfu-528j), each block of their location ("WALL STREET between BROAD STREET and WILLIAM
     STREET") kept when its street and both cross streets are the district's; a full closure of the street ("Full Street
     Closure", "Sidewalk and Street Closure") marked detail.closure, so that the event meets the street spaces' cross-sections;
   - film permits (regime, kind "Filming - <category>"): the Mayor's Office of Media and Entertainment's (tg4x-b46p), each block
     of the parking they hold, kept likewise; parking held is no closure.
4. **Report.** Counts by type and source; the joins' outcomes; rows dropped and why; blocks whose streets are not the district's.

The data are fetched from NYC Open Data's Socrata API (each source's query in data/registry/nyc-district-events.json) into
data/cache/nyc-district-events/financial-district/, once; --offline refuses to fetch.

Output: data/cache/products/city-events-new-york.json (compact JSON; tables as {"columns": [...], "rows": [[...]]}), listed in
the index under the kind `city-events`.

Usage::

    python data/products/street_records.py --only city          # the city model, if not yet built
    python data/products/city_events_new_york.py                # fetches what the cache lacks
    python data/products/city_events_new_york.py --offline      # ... or refuses to
"""

from __future__ import annotations

import argparse
import datetime as dt
import json
import math
import re
import sys
import time
import urllib.parse
import urllib.request
from collections import Counter, defaultdict
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
from city_events import (CACHE, DATA, WALK, Footprints, Frame, day_label, district_box, inside_box, join_label,  # noqa: E402
                         table, year_label)
from product_index import OUT, register  # noqa: E402
from street_records import write  # noqa: E402

UTC = dt.timezone.utc
AREA = "new-york"
CITY_MODEL = OUT / f"city-model-{AREA}.json"
FOLDER = CACHE / "nyc-district-events" / "financial-district"
PORTAL = "https://data.cityofnewyork.us/resource/"
DISTRICT = {"name": "The Financial District: the old streets about Wall Street, Stone Street and Hanover Square",
            "south": 40.703, "west": -74.0128, "north": 40.7082, "east": -74.0055}
MARGIN_M = 60.0
JOIN_M = 15.0
WINDOW = (2008, 2025)            # [2008-01-01, 2025-01-01): the permitted events from 2008
SAME_YEAR = 1                    # a completion and a construction year this close are one building's making
CLOSURES = {"Full Street Closure", "Sidewalk and Street Closure"}
NEW_BUILDING = {"NB", "NEW BUILDING", "New Building", "CO - New Building with Existing Elements to Remain"}
ALTERATION = {"A1", "ALTERATION TYPE 1", "Alteration CO"}


# ---- fetching ------------------------------------------------------------------------------------------------------------------

def get(ident: str, query: dict, attempts: int = 5):
    url = PORTAL + ident + ".json?" + urllib.parse.urlencode(query)
    for a in range(attempts):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "Noetarium data library"})
            with urllib.request.urlopen(req, timeout=300) as r:
                return json.loads(r.read().decode("utf-8"))
        except Exception as e:  # noqa: BLE001
            if a + 1 == attempts:
                raise
            print(f"  retrying {ident}: {e}", file=sys.stderr)
            time.sleep(10 * (a + 1))


def kept(name: str, offline: bool, make):
    """The cached rows of a source, fetched by make() when not yet held (with the time of retrieval beside them)."""
    path = FOLDER / f"{name}.json"
    if path.exists():
        return json.loads(path.read_text(encoding="utf-8"))["rows"]
    if offline:
        raise FileNotFoundError(f"{path} is not held; run without --offline to fetch it")
    rows = make()
    FOLDER.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps({"retrieved_at": dt.datetime.now(UTC).isoformat(timespec="seconds"), "rows": rows}), encoding="utf-8")
    print(f"  {name}: {len(rows)} rows fetched")
    return rows


def paged(ident: str, query: dict, page: int = 10000) -> list:
    rows, offset = [], 0
    while True:
        part = get(ident, {**query, "$limit": str(page), "$offset": str(offset)})
        rows += part
        if len(part) < page:
            return rows
        offset += page


# ---- names and blocks ----------------------------------------------------------------------------------------------------------

LATER = {"ST": "STREET", "STR": "STREET", "AVE": "AVENUE", "AV": "AVENUE", "PL": "PLACE", "LN": "LANE", "SQ": "SQUARE", "DR": "DRIVE",
         "RD": "ROAD", "BLVD": "BOULEVARD"}
FIRST = {"E": "EAST", "W": "WEST", "N": "NORTH", "S": "SOUTH"}


def street_key(name: str) -> str:
    """The key the engine compares street names by (urban-science's events/network.hpp, street_key), written again here only to
    keep the blocks of the district's streets: upper case, punctuation dropped, abbreviations written out, ordinals dropped."""
    words = re.sub(r"[.']", "", name or "").upper()
    words = re.findall(r"[A-Z0-9\u0080-￿]+", words)
    out = []
    for i, w in enumerate(words):
        if i == 0 and len(words) > 1 and w in FIRST:
            w = FIRST[w]
        elif i > 0 and w in LATER:
            w = LATER[w]
        m = re.fullmatch(r"(\d+)(ST|ND|RD|TH)", w)
        out.append(m.group(1) if m else w)
    return " ".join(out)


BLOCK = re.compile(r"^\s*(.+?)\s+between\s+(.+?)\s+and\s+(.+?)\s*$", re.IGNORECASE)


def blocks(text: str) -> list[list[str]]:
    """The blocks a location names, each [street, from, to], from text such as "WALL STREET between BROAD STREET and WILLIAM
    STREET, NEW STREET between ..."."""
    out = []
    for part in (text or "").split(","):
        m = BLOCK.match(part)
        if m:
            out.append([" ".join(g.split()) for g in m.groups()])
    return out


# ---- dates --------------------------------------------------------------------------------------------------------------------

def iso_date(text) -> dt.date | None:
    try:
        return dt.date.fromisoformat((text or "").strip()[:10])
    except ValueError:
        return None


def dobnow_date(text) -> dt.date | None:
    """DOB NOW's '02/15/22 11:08:46 AM', or a date alone ('6/21/2011'), else ISO."""
    text = " ".join((text or "").split())
    for fmt in ("%m/%d/%y %I:%M:%S %p", "%m/%d/%Y %I:%M:%S %p", "%m/%d/%Y", "%m/%d/%y"):
        try:
            return dt.datetime.strptime(text, fmt).date()
        except ValueError:
            continue
    return iso_date(text)


def in_window(d: dt.date | None) -> bool:
    return d is not None and dt.date(WINDOW[0], 1, 1) <= d < dt.date(WINDOW[1], 1, 1)


# ---- the record ----------------------------------------------------------------------------------------------------------------

def build(offline: bool = False) -> dict:
    model = json.loads(CITY_MODEL.read_text(encoding="utf-8"))
    frame = Frame(model["origin"])
    box = district_box(frame, DISTRICT, MARGIN_M)
    B = box["local"]
    W, S, E, N = box["lonlat"]
    within = f"within_box(%s, {N}, {W}, {S}, {E})"
    footprints = Footprints(model["buildings"], B, JOIN_M)
    report = {"dropped": Counter(), "joins": Counter(), "notes": []}
    events = []

    def emit(ident, typ, kind, label, match, source, detail):
        events.append([ident, typ, kind, label[0], label[1], match, source, "recorded", detail])

    def join(lat, lon, what):
        try:
            x, y = (float(v) for v in frame.xy(float(lat), float(lon)))
        except (TypeError, ValueError):
            report["dropped"][f"{what} without a position"] += 1
            return None, None
        if not inside_box(x, y, B):
            report["dropped"][f"{what} outside the box"] += 1
            return None, None
        bid, kind, d = footprints.join((x, y), JOIN_M)
        report["joins"][f"{what}: {kind}"] += 1
        if bid is None:
            report["dropped"][f"{what} joined to no building"] += 1
        return bid, join_label(kind, d)

    # Births by the footprints' construction years.
    rows = kept("footprints", offline, lambda: paged("5zhs-2jue", {"$select": "bin,construction_year,last_status_type,the_geom", "$where": within % "the_geom"}))
    born = {}  # building ↦ year
    for r in rows:
        y = r.get("construction_year")
        if not y or not str(y).isdigit() or not (WINDOW[0] <= int(y) < WINDOW[1]):
            continue
        geom = r.get("the_geom") or {}
        ring = (geom.get("coordinates") or [[[[]]]])[0][0] if geom.get("type") == "MultiPolygon" else (geom.get("coordinates") or [[]])[0]
        if not ring:
            report["dropped"]["footprints without a shape"] += 1
            continue
        lon, lat = np.mean(np.asarray(ring, float), axis=0)
        bid, how = join(lat, lon, "footprints")
        if bid is None:
            continue
        born[bid] = int(y)
        emit(f"footprint:{r.get('bin')}", "birth", "constructed", year_label(int(y)), {"building": bid}, "nyc-building-footprints",
             {"bin": r.get("bin"), "join": how, "last_status_type": r.get("last_status_type")})

    # Certificates of occupancy: each job's first, from DOB's records and DOB NOW's.
    bis = kept("certificates-bis", offline, lambda: paged("bs8b-p36w", {
        "$select": "job_number,job_type,c_o_issue_date,issue_type,bin_number,latitude,longitude",
        "$where": (within % "location") + " AND job_type in ('A1', 'NB')"}))
    now = kept("certificates-dobnow", offline, lambda: paged("pkdm-hqz6", {
        "$select": "job_filing_name,job_type,c_of_o_issuance_date,c_of_o_filing_type,bin,latitude,longitude",
        "$where": "borough='Manhattan' AND community_board='101' AND job_type in ('ALTERATION TYPE 1', 'Alteration CO', 'NEW BUILDING', "
                  "'New Building', 'CO - New Building with Existing Elements to Remain')"}))
    jobs = {}  # job ↦ (first date, type, lat, lon, bin, source)
    for r in bis:
        d = iso_date(r.get("c_o_issue_date"))
        key = (r.get("job_number") or "").strip()
        if d and key and (key not in jobs or d < jobs[key][0]):
            jobs[key] = (d, r.get("job_type"), r.get("latitude"), r.get("longitude"), r.get("bin_number"), "nyc-certificates-of-occupancy")
    for r in now:
        d = dobnow_date(r.get("c_of_o_issuance_date"))
        key = (r.get("job_filing_name") or "").strip().split("-")[0]
        if d and key and (key not in jobs or d < jobs[key][0]):
            jobs[key] = (d, r.get("job_type"), r.get("latitude"), r.get("longitude"), r.get("bin"), "nyc-dobnow-certificates-of-occupancy")
    for key, (d, jt, lat, lon, binn, source) in sorted(jobs.items()):
        if not in_window(d):
            report["dropped"]["certificates' jobs first certified outside the window"] += 1
            continue
        bid, how = join(lat, lon, "certificates")
        if bid is None:
            continue
        detail = {"job": key, "job_type": jt, "bin": binn, "join": how}
        if jt in NEW_BUILDING:
            if bid in born and abs(born[bid] - d.year) <= SAME_YEAR:
                detail["same_as"] = f"constructed {born[bid]}"
            emit(f"co:{key}", "birth", "development completed", day_label(d), {"building": bid}, source, detail)
        elif jt in ALTERATION:
            emit(f"co:{key}", "alteration", "major alteration completed", day_label(d), {"building": bid}, source, detail)

    # Landmarks designated in the window.
    rows = kept("landmarks", offline, lambda: paged("buis-pvji", {"$select": "lpc_name,lpc_lpnumb,desdate,landmarkty,bbl,latitude,longitude,address",
                                                                  "$where": "borough='MN'"}))
    for r in rows:
        d = dobnow_date(r.get("desdate"))          # '6/21/2011'
        if not in_window(d):
            continue
        bid, how = join(r.get("latitude"), r.get("longitude"), "landmarks")
        if bid is None:
            continue
        emit(f"landmark:{r.get('lpc_lpnumb')}", "institution", "landmark designated", day_label(d), {"building": bid}, "nyc-landmark-sites",
             {"name": r.get("lpc_name"), "lp_number": r.get("lpc_lpnumb"), "type": r.get("landmarkty"), "address": r.get("address"), "join": how})

    # The district's streets, for keeping the blocks that lie in it.
    names = sorted({r["name"] for r in model["roads"] if r.get("name") and r.get("kind") in WALK
                    and any(B[0] <= q[0] <= B[2] and B[1] <= q[1] <= B[3] for q in r.get("line", []))})
    keys = {street_key(n) for n in names}
    outside = Counter()

    def ours(text):
        mine = []
        for blk in blocks(text):
            if all(street_key(x) in keys for x in blk):
                mine.append(blk)
            else:
                for x in blk:
                    if street_key(x) not in keys:
                        outside[street_key(x)] += 1
        return mine

    # Permitted events, one a permit and its dates, its blocks gathered from its rows.
    rows = kept("permitted-events", offline, lambda: paged("bkfu-528j", {
        "$select": "event_id,event_name,start_date_time,end_date_time,event_agency,event_type,event_location,street_closure_type",
        "$where": f"event_borough='Manhattan' AND community_board like '1,%' AND start_date_time >= '{WINDOW[0]}-01-01T00:00:00' "
                  f"AND start_date_time < '{WINDOW[1]}-01-01T00:00:00'",
        "$order": "event_id"}))
    permits = defaultdict(lambda: {"blocks": [], "closure": False, "types": Counter(), "closures": Counter(), "agency": None, "name": None})
    for r in rows:
        start, end = iso_date(r.get("start_date_time")), iso_date(r.get("end_date_time"))
        if start is None:
            report["dropped"]["permitted events without a start"] += 1
            continue
        mine = ours(r.get("event_location"))
        if not mine:
            report["dropped"]["permitted events' rows naming no block of the district"] += 1
            continue
        p = permits[(r.get("event_id"), start, end or start)]
        for blk in mine:
            if blk not in p["blocks"]:
                p["blocks"].append(blk)
        p["closure"] = p["closure"] or (r.get("street_closure_type") or "").strip() in CLOSURES
        p["types"][(r.get("event_type") or "unknown").strip()] += 1
        p["closures"][(r.get("street_closure_type") or "").strip()] += 1
        p["agency"], p["name"] = r.get("event_agency"), r.get("event_name")
    for (eid, start, end), p in sorted(permits.items(), key=lambda kv: (kv[0][1], str(kv[0][0]))):
        end = max(end, start)
        kind = p["types"].most_common(1)[0][0]
        emit(f"permit:{eid}:{start.isoformat()}", "regime", kind, day_label(start, end), {"blocks": p["blocks"]}, "nyc-permitted-events",
             {"name": p["name"], "agency": p["agency"], "closure": p["closure"], "closure_types": dict(p["closures"])})

    # Film permits, each block of the parking they hold.
    rows = kept("film-permits", offline, lambda: paged("tg4x-b46p", {
        "$select": "eventid,eventtype,startdatetime,enddatetime,parkingheld,category,subcategoryname",
        "$where": f"borough='Manhattan' AND communityboard_s like '1,%' AND startdatetime < '{WINDOW[1]}-01-01T00:00:00'"}))
    for r in rows:
        start, end = iso_date(r.get("startdatetime")), iso_date(r.get("enddatetime"))
        if not in_window(start):
            continue
        mine = ours(r.get("parkingheld"))
        if not mine:
            report["dropped"]["film permits naming no block of the district"] += 1
            continue
        emit(f"film:{r.get('eventid')}", "regime", f"Filming - {(r.get('category') or 'unknown').strip()}", day_label(start, max(end or start, start)),
             {"blocks": mine}, "nyc-film-permits", {"event_type": r.get("eventtype"), "subcategory": r.get("subcategoryname"), "closure": False})

    events.sort(key=lambda e: (e[3], e[0]))
    by_type = Counter(e[1] for e in events)
    by_source = defaultdict(Counter)
    for e in events:
        by_source[e[6]][e[1]] += 1
    regimes = Counter(e[2] for e in events if e[1] == "regime")
    product = {
        "product": "city-events",
        "id": AREA,
        "built": dt.datetime.now(UTC).isoformat(timespec="seconds"),
        "frame": model["frame"],
        "origin": model["origin"],
        "city_model": {"file": CITY_MODEL.name, "built": model.get("built")},
        "box": box,
        "window": {"from": f"{WINDOW[0]:04d}-01-01", "to": f"{WINDOW[1]:04d}-01-01", "earliest": f"{WINDOW[0]:04d}-01-01"},
        "sources": [
            {"id": "nyc-district-events", "title": "NYC Open Data: building footprints, certificates of occupancy, landmark sites, permitted events and film permits",
             "credit": "City of New York (DOITT, Department of Buildings, Landmarks Preservation Commission, Street Activity Permit Office, Mayor's Office of Media and Entertainment), NYC Open Data",
             "licence": "NYC Open Data's Terms of Use; New York City's Open Data Law publishes its datasets without restrictions on their use", "retrieved": dt.date.today().isoformat()},
            {"id": f"city-model-{AREA}", "title": "The city model's buildings and ways (data/products/street_records.py)", "credit": model.get("credit"),
             "licence": "ODbL 1.0", "retrieved": model.get("built")}],
        "conventions": {
            "times": "from and to bound a label's resolution, [from, to), civil dates in New York: a year 1 January to the next 1 January, a day that day to the next",
            "match": "building (a city-model building id), or blocks ([street, from, to], each a street between the junctions where it meets the two others)",
            "join": "inside: the footprint holds the point (holes respected); nearest:<m>: the nearest outline within 15 m",
        },
        "properties": table(["id", "base_property_id", "property_ids", "block", "at", "building", "join"], []),
        "censuses": table(["year", "property", "property_id", "construction_year", "refurbished_year", "floors_above", "predominant_use",
                           "name", "accessibility_rating", "bicycle_spaces"], []),
        "establishments": table(["year", "property", "property_id", "name", "anzsic4"], []),
        "anzsic": {},
        "events": table(["id", "type", "kind", "from", "to", "match", "source", "provenance", "detail"], events),
        "report": {
            "counts": {"events": len(events)},
            "events_by_type": dict(sorted(by_type.items())),
            "events_by_source": {s: dict(sorted(c.items())) for s, c in sorted(by_source.items())},
            "regimes_by_kind": dict(regimes.most_common()),
            "regimes_closing_the_street": sum(1 for e in events if e[1] == "regime" and e[8].get("closure")),
            "joins": dict(report["joins"]),
            "dropped": dict(sorted(report["dropped"].items())),
            "streets_named_outside_the_district": dict(outside.most_common(40)),
            "district_street_names": names,
            "notes": ["The construction closures (i6b5-j7bu) and Open Streets (uiay-nctu) datasets hold current works only and none in the window; they are not read.",
                      "Film permits are published from 2023 only."],
        },
    }
    path = OUT / f"city-events-{AREA}.json"
    write(path, product)
    register("city-events", {"name": f"city-events-{AREA}", "file": path.name,
                             "title": "New York's Financial District, 2008-2024: dated events of its buildings and streets",
                             "domain": "urban-science", "window": [product["window"]["from"], product["window"]["to"]],
                             "events": len(events),
                             "source": "City of New York, NYC Open Data (buildings, certificates of occupancy, landmarks, permitted events, film permits)"})
    print(f"  city-events-{AREA}: {len(events)} events {dict(by_type)}; {path.stat().st_size / 1e6:.2f} MB")
    return product


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--offline", action="store_true", help="never fetch what the cache lacks")
    args = parser.parse_args(argv)
    sys.stdout.reconfigure(errors="replace")
    if not CITY_MODEL.exists():
        print(f"city-events-{AREA}: missing {CITY_MODEL.relative_to(DATA.parent)}; run: python data/products/street_records.py --only city", file=sys.stderr)
        return 1
    build(offline=args.offline)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
