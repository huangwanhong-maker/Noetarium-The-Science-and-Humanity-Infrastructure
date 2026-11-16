"""Archives of evidence in space and time, for the Urban Studio's plug-in of that name (ADR 0123): reports of something at a place
and a time, the sources that made them, the files they rest on, and claims about places and times that they bear on.

A case is a recipe that writes an archive (noetarium.evidence/1, api/cpp/science/spacetime_evidence.hpp) with the files it rests on
beside it. Adding a city or a subject is adding a case; nothing downstream names one.

    nyc-rodents   Rats in the East Village and the Lower East Side of Manhattan, 2024, from two public records of New York City:
                  the Department of Health and Mental Hygiene's Rodent Inspection (NYC Open Data p937-wjvj: every initial
                  inspection, its lot, its date and its result) and 311's service requests (erm2-nwe9: complaints of the type
                  Rodent, with their address and time). Three quarters are used apart:
                  - January to March, to count how a resident's complaint stands to what inspectors find: among the lots
                    inspected, the share of those found with rat activity from which a complaint came, and the share of those
                    passed (calibration.csv);
                  - April to June, the archive: each complaint of a rat sighted or of signs and each initial inspection is an item,
                    at its place and time; complaints from one address share an origin, so that a household that complains five
                    times is one report; the claims are that rats are active within 40 m of each point of a lattice 80 m apart, each item
                    reporting an event of its own (a lot's rats, a household's sighting);
                  - July to September, what inspectors then found within each claim's area (outcome.csv), kept apart to see how the
                    archive's conclusions fared.
                  What is counted is counted here and said; what is declared (the inspector's channel, the prior, the acts'
                  worths, the budget) is an assumption of the example, written in the archive's own words. The place is drawn
                  from the data library's world tiles (data/acquisition/world_city.py: Overture Maps' ways and footprints,
                  ODbL), clipped to the box and written beside the archive (basemap.json), kept in the cache only.

    python data/products/urban_evidence.py [case]    → data/cache/products/urban-evidence/<case>/ and urban-evidence/index.json
"""

from __future__ import annotations

import csv
import datetime as dt
import hashlib
import json
import math
import sys
import time
import urllib.parse
import urllib.request
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
OUT = ROOT / "data" / "cache" / "products" / "urban-evidence"
NYC = "https://data.cityofnewyork.us/resource/"
BOX = {"south": 40.7185, "north": 40.7335, "west": -73.9925, "east": -73.9745}   # the East Village and the Lower East Side
QUARTERS = {"count": ("2024-01-01", "2024-03-31"), "archive": ("2024-04-01", "2024-06-30"), "after": ("2024-07-01", "2024-09-30")}
SPACING, RADIUS, PLACED = 80.0, 40.0, 15.0   # metres: the lattice of claims, a claim's radius, how well an address is placed


def _get(dataset: str, query: dict, attempts: int = 4) -> list:
    url = f"{NYC}{dataset}.json?{urllib.parse.urlencode(query)}"
    for a in range(attempts):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "Noetarium data library"})
            with urllib.request.urlopen(req, timeout=300) as r:
                return json.loads(r.read().decode("utf-8"))
        except Exception:  # noqa: BLE001
            if a + 1 == attempts:
                raise
            time.sleep(3 * (a + 1))
    return []


def _all(dataset: str, where: str, order: str, select: str) -> list:
    rows, offset = [], 0
    while True:
        page = _get(dataset, {"$select": select, "$where": where, "$order": order, "$limit": 20000, "$offset": offset})
        rows += page
        if len(page) < 20000:
            return rows
        offset += len(page)


def _box(lat: str = "latitude", lon: str = "longitude") -> str:
    return f"{lat} between {BOX['south']} and {BOX['north']} and {lon} between {BOX['west']} and {BOX['east']}"


def _metres(lat1, lon1, lat2, lon2) -> float:
    """A distance good to a part in a thousand over a few hundred metres, for sorting rows into areas here; the engine's are geodesy's."""
    k = 111_320.0
    return math.hypot((lat2 - lat1) * k, (lon2 - lon1) * k * math.cos(math.radians((lat1 + lat2) / 2)))


def _quarter(date: str) -> str | None:
    for name, (a, b) in QUARTERS.items():
        if a <= date[:10] <= b:
            return name
    return None


def basemap(box: dict) -> dict:
    """The ways and building footprints within a box: what a page draws of the place (world_city.within)."""
    sys.path.insert(0, str(ROOT / "data" / "acquisition"))
    import world_city
    return world_city.within(box)


def nyc_rodents(log=print) -> dict:
    out = OUT / "nyc-rodents"
    out.mkdir(parents=True, exist_ok=True)
    span = f"between '{QUARTERS['count'][0]}T00:00:00' and '{QUARTERS['after'][1]}T23:59:59'"
    log("Fetching the initial inspections …")
    inspections = _all("p937-wjvj", f"inspection_type='Initial' and inspection_date {span} and {_box()}", "inspection_date, job_id",
                       "job_id,inspection_date,result,house_number,street_name,bbl,latitude,longitude")
    log(f"  {len(inspections)} inspections")
    log("Fetching the complaints …")
    complaints = _all("erm2-nwe9", f"complaint_type='Rodent' and created_date {span} and {_box()}", "created_date, unique_key",
                      "unique_key,created_date,descriptor,incident_address,bbl,latitude,longitude,location_type")
    log(f"  {len(complaints)} complaints")
    fetched = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    files = {}
    for name, rows in (("inspections-2024.json", inspections), ("complaints-2024.json", complaints)):
        text = json.dumps(rows, ensure_ascii=False, indent=0)
        (out / name).write_text(text, encoding="utf-8", newline="\n")
        files[name] = hashlib.sha256((out / name).read_bytes()).hexdigest()

    def result(r) -> bool | None:
        return True if "Rat Activity" in r.get("result", "") else False if r.get("result") == "Passed" else None

    sighting = [c for c in complaints if c.get("descriptor") in ("Rat Sighting", "Signs of Rodents")]

    # ---- January to March: a complaint against what inspectors found, lot by lot ------------------------------------------------------
    lots: dict[str, dict] = {}
    for r in inspections:
        if _quarter(r["inspection_date"]) != "count" or result(r) is None or not r.get("bbl"):
            continue
        lot = lots.setdefault(r["bbl"], {"lot": r["bbl"], "lat": float(r["latitude"]), "lon": float(r["longitude"]), "found": False, "inspections": 0})
        lot["found"] = lot["found"] or result(r)
        lot["inspections"] += 1
    complained = {c["bbl"] for c in sighting if _quarter(c["created_date"]) == "count" and c.get("bbl")}
    for lot in lots.values():
        lot["reported"] = lot["lot"] in complained
    found = [x for x in lots.values() if x["found"]]
    clear = [x for x in lots.values() if not x["found"]]
    hit = sum(x["reported"] for x in found) / len(found)
    false_alarm = sum(x["reported"] for x in clear) / len(clear)
    with (out / "calibration.csv").open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["lot", "lat", "lon", "inspections", "reference", "reported"])
        for x in sorted(lots.values(), key=lambda x: x["lot"]):
            w.writerow([x["lot"], x["lat"], x["lon"], x["inspections"], "found" if x["found"] else "clear", "yes" if x["reported"] else "no"])

    # ---- The lattice of claims --------------------------------------------------------------------------------------------------------
    mid = (BOX["south"] + BOX["north"]) / 2
    dlat, dlon = SPACING / 111_320.0, SPACING / (111_320.0 * math.cos(math.radians(mid)))
    points = []
    j = 0
    lat = BOX["south"] + dlat / 2
    while lat < BOX["north"]:
        i = 0
        lon = BOX["west"] + dlon / 2
        while lon < BOX["east"]:
            points.append((f"c{j:02d}-{i:02d}", round(lat, 6), round(lon, 6)))
            lon += dlon
            i += 1
        lat += dlat
        j += 1

    def near(la, lo):
        return [p for p in points if abs(p[1] - la) < dlat and abs(p[2] - lo) < dlon and _metres(la, lo, p[1], p[2]) <= RADIUS]

    # Where rats were found somewhere in an area in the first quarter, how often a lot inspected there was found with them: what a
    # lot's finding says of its area.
    by_area: dict[str, list] = {}
    for r in inspections:
        if _quarter(r["inspection_date"]) == "count" and result(r) is not None:
            for p in near(float(r["latitude"]), float(r["longitude"])):
                by_area.setdefault(p[0], []).append(result(r))
    active = [v for v in by_area.values() if any(v)]
    lot_given_area = sum(sum(v) for v in active) / sum(len(v) for v in active)
    prior = len(active) / len(by_area)

    # ---- April to June: the archive ---------------------------------------------------------------------------------------------------
    items = []
    for r in inspections:
        if _quarter(r["inspection_date"]) != "archive" or result(r) is None:
            continue
        address = f"{r.get('house_number', '').strip()} {r.get('street_name', '').strip()}".strip().title()
        items.append({"id": f"i-{r['job_id']}", "title": f"Initial inspection, {address}: {r['result'].lower()}", "kind": "inspection", "holding": "inspections",
                      "lat": float(r["latitude"]), "lon": float(r["longitude"]), "radius": PLACED, "from": r["inspection_date"][:19], "source": "inspector",
                      "says": "holds" if result(r) else "denies", "status": "accepted"})
    for c in sighting:
        if _quarter(c["created_date"]) != "archive":
            continue
        address = (c.get("incident_address") or "").strip().title()
        items.append({"id": f"c-{c['unique_key']}", "title": f"Complaint to 311, {address or 'no address'}: {c['descriptor'].lower()}", "kind": "complaint", "holding": "complaints",
                      "lat": float(c["latitude"]), "lon": float(c["longitude"]), "radius": PLACED, "from": c["created_date"][:19], "source": "resident",
                      "origin": f"address: {address}" if address else "", "says": "holds", "status": "accepted"})
    claims = [{"id": ident, "text": f"Rats are active within {RADIUS:.0f} m of this point, April to June 2024", "lat": la, "lon": lo, "radius": RADIUS,
               "from": "2024-04", "until": "2024-06", "prior": round(prior, 3), "e_given_h": round(lot_given_area, 3), "e_given_not_h": 0.0, "event": "each"} for ident, la, lo in points]

    # ---- July to September: what was then found ---------------------------------------------------------------------------------------
    after: dict[str, list] = {p[0]: [] for p in points}
    for r in inspections:
        if _quarter(r["inspection_date"]) == "after" and result(r) is not None:
            for p in near(float(r["latitude"]), float(r["longitude"])):
                after[p[0]].append(result(r))
    with (out / "outcome.csv").open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["claim", "lat", "lon", "looked", "found"])
        for ident, la, lo in points:
            w.writerow([ident, la, lo, len(after[ident]), sum(after[ident])])

    publisher = "NYC Open Data, for the City of New York"
    archive = {
        "schema": "noetarium.evidence/1",
        "title": "Rats in the East Village and the Lower East Side, April to June 2024",
        "about": (f"Two public records of New York City read as an archive of evidence: {sum(1 for x in items if x['kind'] == 'inspection')} initial inspections by the Department of Health and "
                  f"Mental Hygiene and {sum(1 for x in items if x['kind'] == 'complaint')} complaints to 311 of rats sighted or their signs, each at its place and time; and {len(claims)} claims, "
                  f"that rats are active within {RADIUS:.0f} m of each point of a lattice {SPACING:.0f} m apart. Counted in the quarter before (January to March), lot by lot: of {len(found)} "
                  f"lots found with rat activity a complaint had come from {sum(x['reported'] for x in found)} ({100 * hit:.1f}%), and of {len(clear)} lots passed, from "
                  f"{sum(x['reported'] for x in clear)} ({100 * false_alarm:.1f}%): that is the resident's channel here. Inspectors go where complaints are, so both shares are too high by "
                  f"an amount this record cannot tell. A lot inspected in an area where rats were found somewhere was itself found with them {100 * lot_given_area:.0f}% of the time, and "
                  f"{100 * prior:.0f}% of the areas looked at had some: those are P(E | H) and the prior. Declared, not counted: an inspector finds rat activity that is there nine times "
                  "in ten and reports it where there is none twice in a hundred; the worths of acting and of leaving; the cost and the number of the inspections that can be made. "
                  "Complaints from one address share an origin. Times are New York's as published. None of this is advice to anyone."),
        "frame": {"lat": mid, "lon": (BOX["west"] + BOX["east"]) / 2, "box": BOX},
        "basemap": "data:products/urban-evidence/nyc-rodents/basemap.json",
        "sources": [
            {"id": "resident", "name": "A resident's complaint to 311", "hit": round(hit, 4), "false_alarm": round(false_alarm, 4),
             "note": f"counted lot by lot, January to March 2024: {sum(x['reported'] for x in found)} of {len(found)} lots found with rat activity, {sum(x['reported'] for x in clear)} of {len(clear)} passed"},
            {"id": "inspector", "name": "An inspector's initial inspection", "hit": 0.9, "false_alarm": 0.02, "note": "declared for this example, not counted: no record says how often an inspection errs"},
        ],
        "holdings": [
            {"id": "inspections", "name": "inspections-2024.json", "digest": files["inspections-2024.json"], "custodian": publisher, "since": fetched,
             "attributes": {"title": "Rodent Inspection (p937-wjvj), initial inspections in the box, January to September 2024", "creator": "NYC Department of Health and Mental Hygiene"},
             "transfers": [{"when": fetched, "from": publisher, "to": "the data library's cache", "digest": files["inspections-2024.json"], "note": "fetched by data/products/urban_evidence.py"}]},
            {"id": "complaints", "name": "complaints-2024.json", "digest": files["complaints-2024.json"], "custodian": publisher, "since": fetched,
             "attributes": {"title": "311 Service Requests (erm2-nwe9), complaints of the type Rodent in the box, January to September 2024", "creator": "NYC 311"},
             "transfers": [{"when": fetched, "from": publisher, "to": "the data library's cache", "digest": files["complaints-2024.json"], "note": "fetched by data/products/urban_evidence.py"}]},
        ],
        "items": items,
        "claims": claims,
        "rule": {"established": 0.9, "refuted": 0.05},
        "decision": {"acts": [{"name": "treat the area", "holds": -1, "not": -1}, {"name": "leave it", "holds": -6, "not": 0}],
                     "inspection": {"name": "a survey of the area", "sensitivity": 0.8, "specificity": 0.95, "cost": 0.25}, "budget": 10, "charged": True,
                     "note": "declared for this example: worths in units of one treatment's cost; rats left alone for a quarter cost six treatments; a survey of an area's lots finds activity that is there four times in five and reports it where there is none once in twenty, and costs a quarter of a treatment; forty surveys can be made"},
        "credit": "NYC Department of Health and Mental Hygiene, Rodent Inspection; NYC 311, Service Requests; both on NYC Open Data",
    }
    (out / "archive.json").write_text(json.dumps(archive, ensure_ascii=False, indent=1), encoding="utf-8")
    log("Reading the place from the world tiles …")
    try:
        place = basemap(BOX)
        (out / "basemap.json").write_text(json.dumps(place, ensure_ascii=False, separators=(",", ":")), encoding="utf-8")
        log(f"  {len(place['ways'])} ways and {len(place['buildings'])} footprints")
    except Exception as error:  # noqa: BLE001  the archive stands without its drawing
        log(f"  the place could not be read ({error}); the archive is drawn without it")
    summary = {"case": "nyc-rodents", "title": archive["title"], "items": len(items), "claims": len(claims), "lots_counted": len(lots), "hit": hit, "false_alarm": false_alarm,
               "lot_given_area": lot_given_area, "prior": prior, "fetched": fetched, "files": files}
    log(json.dumps(summary, indent=1))
    return summary


CASES = {"nyc-rodents": nyc_rodents}


def main(argv: list[str]) -> int:
    names = argv or list(CASES)
    index = []
    for name in names:
        if name not in CASES:
            print(f"there is no case {name} (there is: {', '.join(CASES)})", file=sys.stderr)
            return 2
        index.append(CASES[name]())
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "index.json").write_text(json.dumps({"cases": index}, indent=1), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
