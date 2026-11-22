"""Visitors' nights, arrivals and the places they stay in, month by month, for European city-regions (ADR 0156), written into
data/cache/products/visitors/.

From Eurostat's tourism statistics, for a declared year:
- nights spent at tourist accommodation establishments (hotels, holiday and other short-stay accommodation, camping grounds;
  NACE I551–I553) by month and residence of the guest (domestic, foreign), by NUTS 2 or NUTS 3 region: tour_occ_nin2m;
- nights spent and stays in short-stay accommodation booked through online platforms (Airbnb, Booking, Expedia Group, Tripadvisor),
  by month and residence, by NUTS 2 region: tour_ce_omn12;
- arrivals at tourist accommodation establishments by residence, annual, NUTS 2: tour_occ_arn2;
- population on 1 January (demo_r_pjanaggr3) and area (reg_area3) of each place;
- nights by class of establishment (hotels, holiday and other short-stay accommodation, camping grounds), annual, NUTS 2: tour_occ_nin2
  (ADR 0162), null where not published for the place's code.

A place whose platform nights are published only for the NUTS 2 region around it (Venice within Veneto) carries the region's
platform nights and the registered nights of both, so that a user of the product can allocate them by a declared key; the product
allocates nothing. Months Eurostat withholds are null. Eurostat's data are reused with the source acknowledged.

    python data/products/visitor_stock.py [--year 2024] [--case european-cities]
"""

from __future__ import annotations

import argparse
import json
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "visitors" / "raw"
OUT = CACHE / "products" / "visitors"
EUROSTAT = "https://ec.europa.eu/eurostat/api/dissemination/statistics/1.0/data/"
CREDIT = "Eurostat (tour_occ_nin2m, tour_ce_omn12, tour_occ_arn2, tour_occ_nin2, demo_r_pjanaggr3, reg_area3), reused with the source acknowledged"
MONTHS = [f"M{m:02d}" for m in range(1, 13)]

# A case: places, each with a short name for charts, the NUTS code of its registered nights and, where platform nights are
# published only for the region around it, that region's code. Six, so that a chart's six inks tell them apart.
CASES = {
    "european-cities": [
        {"id": "vienna", "name": "Vienna", "short": "Vienna", "nuts": "AT13"},
        {"id": "berlin", "name": "Berlin", "short": "Berlin", "nuts": "DE30"},
        {"id": "prague", "name": "Prague", "short": "Prague", "nuts": "CZ01"},
        {"id": "brussels", "name": "Brussels-Capital Region", "short": "Brussels", "nuts": "BE10"},
        {"id": "lisbon", "name": "Grande Lisboa", "short": "Lisbon", "nuts": "PT1A"},
        {"id": "venice", "name": "Venice (Metropolitan City)", "short": "Venice", "nuts": "ITH35", "platform_region": "ITH3"},
    ],
}


def fetch(dataset: str, query: str, name: str) -> dict:
    path = RAW / name
    if not path.is_file():
        RAW.mkdir(parents=True, exist_ok=True)
        req = urllib.request.Request(EUROSTAT + dataset + "?" + query, headers={"User-Agent": "noetarium-data/1"})
        with urllib.request.urlopen(req, timeout=600) as r:
            path.write_bytes(r.read())
    d = json.loads(path.read_text(encoding="utf-8"))
    if "error" in d:
        raise RuntimeError(f"Eurostat refused {dataset}: {d['error']}")
    return d


def cube(d: dict) -> dict:
    """A JSON-stat dataset as {(code of each dimension, in order): value}, with the dimensions' names in order under the key None."""
    ids, sizes = d["id"], d["size"]
    cats = []
    for dim in ids:
        idx = d["dimension"][dim]["category"]["index"]
        cats.append(sorted(idx, key=idx.get) if isinstance(idx, dict) else list(idx))
    out = {None: ids}
    for k, v in (d["value"].items() if isinstance(d["value"], dict) else enumerate(d["value"])):
        if v is None:
            continue
        k = int(k)
        key = []
        for size, order in zip(reversed(sizes), reversed(cats)):
            key.append(order[k % size])
            k //= size
        out[tuple(reversed(key))] = float(v)
    return out


def pick(c: dict, **want) -> float | None:
    """The one value whose codes match `want` (every other dimension having a single category)."""
    ids = c[None]
    for key, v in c.items():
        if key is None:
            continue
        if all(key[ids.index(dim)] == code for dim, code in want.items()):
            return v
    return None


def build(case: str, year: int) -> dict:
    places = CASES[case]
    geos = sorted({p["nuts"] for p in places} | {p["platform_region"] for p in places if p.get("platform_region")})
    g = "".join(f"&geo={x}" for x in geos)
    nin = cube(fetch("tour_occ_nin2m", f"unit=NR&nace_r2=I551-I553&time={year}{g}&lang=EN", f"tour_occ_nin2m-{case}-{year}.json"))
    plat = cube(fetch("tour_ce_omn12", f"unit=NR&time={year}{g}&lang=EN", f"tour_ce_omn12-{case}-{year}.json"))
    arr = cube(fetch("tour_occ_arn2", f"unit=NR&nace_r2=I551-I553&time={year}{g}&lang=EN", f"tour_occ_arn2-{case}-{year}.json"))
    pop = cube(fetch("demo_r_pjanaggr3", f"sex=T&age=TOTAL&unit=NR&time={year}{g}&lang=EN", f"demo_r_pjanaggr3-{case}-{year}.json"))
    area_raw = fetch("reg_area3", f"landuse=TOTAL&unit=KM2&lastTimePeriod=1{g}&lang=EN", f"reg_area3-{case}.json")
    classes = cube(fetch("tour_occ_nin2", f"unit=NR&c_resid=TOTAL&nace_r2=I551&nace_r2=I552&nace_r2=I553&time={year}{g}&lang=EN", f"tour_occ_nin2-{case}-{year}.json"))
    area = cube(area_raw)
    area_year = next(iter(area_raw["dimension"]["time"]["category"]["index"]))

    def months(c, geo, **want):
        return [pick(c, geo=geo, month=m, **want) for m in MONTHS]

    out = []
    for p in places:
        geo, region = p["nuts"], p.get("platform_region")
        pg = region or geo
        rec = {"id": p["id"], "name": p["name"], "short": p.get("short", p["name"]), "nuts": geo,
               "registered": {r: months(nin, geo, c_resid=r) for r in ("DOM", "FOR")},
               "platform": {r: months(plat, pg, c_resid=r, indic_to="NGT_SP") for r in ("DOM", "FOR")},
               "platform_stays": {r: months(plat, pg, c_resid=r, indic_to="STY") for r in ("DOM", "FOR")},
               "platform_rented": {r: months(plat, pg, c_resid=r, indic_to="LSTY") for r in ("DOM", "FOR")},
               "arrivals": {r: pick(arr, geo=geo, c_resid=r) for r in ("DOM", "FOR")},
               "population": pick(pop, geo=geo), "area_km2": pick(area, geo=geo),
               "by_class": {k: pick(classes, geo=geo, nace_r2=code) for k, code in (("hotels", "I551"), ("holiday", "I552"), ("camping", "I553"))}}
        if region:
            rec["platform_region"] = region
            rec["registered_region"] = {r: months(nin, region, c_resid=r) for r in ("DOM", "FOR")}
            rec["arrivals"] = {r: None for r in ("DOM", "FOR")}  # arrivals are published for NUTS 2 regions only
        out.append(rec)
    return {"case": case, "year": year, "area_year": area_year, "places": out, "credit": CREDIT,
            "fetched": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--year", type=int, default=2024)
    ap.add_argument("--case", default="european-cities", choices=sorted(CASES))
    a = ap.parse_args()
    d = build(a.case, a.year)
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{a.case}-{a.year}.json"
    path.write_text(json.dumps(d, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
    for p in d["places"]:
        reg = sum(v or 0 for r in p["registered"].values() for v in r)
        pl = sum(v or 0 for r in p["platform"].values() for v in r)
        print(f"{p['name']}: registered nights {reg:,.0f}, platform nights {pl:,.0f}{' (of ' + p['platform_region'] + ')' if p.get('platform_region') else ''}, population {p['population']:,.0f}")
    print(f"wrote {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
