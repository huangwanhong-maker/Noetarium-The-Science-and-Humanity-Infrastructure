"""Journeys to work between places, year by year: workers by where they live, jobs by where they are, and the flows between,
for urban science's land use and transport (ADR 0094). A case is a recipe: a state's files and the counties kept. Adding a
region is adding a case; nothing downstream names one.

    nyc-lodes   LEHD Origin-Destination Employment Statistics, version 8 (US Census Bureau), New York State's main files (jobs
                whose worker lives and works in the state; JT00, all jobs; S000, their count), every year asked of 2002–2023, on
                2020 census blocks. Kept: the pairs whose home and workplace both lie in New York City's five boroughs (the
                counties 36005 the Bronx, 36047 Brooklyn, 36061 Manhattan, 36081 Queens, 36085 Staten Island), summed from
                blocks to census tracts. A tract's point is the mean of its blocks' internal points (the crosswalk's blklatdd and
                blklondd), each weighted by the jobs and workers of the block summed over the years fetched, so that it lies
                where people live and work rather than in a park or the water; a tract with neither takes its blocks' plain mean.

    python data/products/commuting.py [case] [year ...]   → data/cache/products/commuting/<case>/tracts.csv,
                                                             flows-<year>.csv (home, work, jobs) and index.json

The flows are counts of jobs, a worker counted once per job held at the reference week (LODES's definition of a job: a
worker's primary and secondary jobs are both counted in JT00). LODES perturbs the block-level counts for confidentiality
(noise infusion and synthetic residences); summed to tracts the perturbation largely cancels, which is how the Census Bureau
recommends using them.
"""

from __future__ import annotations

import csv
import gzip
import io
import json
import sys
import time
import urllib.request
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "raw" / "lodes"
OUT = CACHE / "products" / "commuting"
BASE = "https://lehd.ces.census.gov/data/lodes/LODES8/"

CASES = {
    "nyc-lodes": {
        "state": "ny",
        "counties": {"36005": "Bronx", "36047": "Kings", "36061": "New York", "36081": "Queens", "36085": "Richmond"},
        "years": [2002, 2005, 2008, 2010, 2012, 2015, 2017, 2019, 2021, 2023],
        "description": "New York City's five boroughs: journeys to work between census tracts, LODES 8",
    },
}


def _fetch(url: str, path: Path, attempts: int = 4) -> Path:
    if path.exists() and path.stat().st_size > 0:
        return path
    path.parent.mkdir(parents=True, exist_ok=True)
    for a in range(attempts):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "Noetarium data library"})
            with urllib.request.urlopen(req, timeout=600) as r, open(path.with_suffix(".part"), "wb") as f:
                while True:
                    chunk = r.read(1 << 20)
                    if not chunk:
                        break
                    f.write(chunk)
            path.with_suffix(".part").replace(path)
            return path
        except Exception as e:  # noqa: BLE001
            if a + 1 == attempts:
                raise
            print("retrying", url, e, file=sys.stderr)
            time.sleep(5 * (a + 1))
    return path


def _rows(path: Path):
    with gzip.open(path, "rt", encoding="utf-8", newline="") as f:
        yield from csv.DictReader(f)


def build(case: str, years: list[int] | None = None) -> dict:
    spec = CASES[case]
    st = spec["state"]
    counties = spec["counties"]
    years = years or spec["years"]
    xwalk = _fetch(f"{BASE}{st}/{st}_xwalk.csv.gz", RAW / f"{st}_xwalk.csv.gz")
    tract_of, point = {}, {}
    for r in _rows(xwalk):
        if r["cty"] in counties:
            tract_of[r["tabblk2020"]] = r["trct"]
            point[r["tabblk2020"]] = (float(r["blklatdd"]), float(r["blklondd"]))
    flows = {}
    activity = defaultdict(float)
    for y in years:
        od = _fetch(f"{BASE}{st}/od/{st}_od_main_JT00_{y}.csv.gz", RAW / f"{st}_od_main_JT00_{y}.csv.gz")
        table = defaultdict(int)
        for r in _rows(od):
            h, w = r["h_geocode"], r["w_geocode"]
            th, tw = tract_of.get(h), tract_of.get(w)
            if th is None or tw is None:
                continue
            n = int(r["S000"])
            table[(th, tw)] += n
            activity[h] += n
            activity[w] += n
        flows[y] = table
        print(f"{case} {y}: {sum(table.values()):,} jobs in {len(table):,} pairs of tracts", file=sys.stderr)

    blocks = defaultdict(list)
    for b, t in tract_of.items():
        blocks[t].append(b)
    tracts = sorted({t for table in flows.values() for pair in table for t in pair})
    index = {t: i for i, t in enumerate(tracts)}
    out = OUT / case
    out.mkdir(parents=True, exist_ok=True)
    with open(out / "tracts.csv", "w", encoding="utf-8", newline="") as f:
        wr = csv.writer(f)
        wr.writerow(["index", "geoid", "county", "borough", "lat", "lon", "blocks"])
        for t in tracts:
            bs = blocks[t]
            wsum = sum(activity[b] for b in bs)
            if wsum > 0:
                lat = sum(point[b][0] * activity[b] for b in bs) / wsum
                lon = sum(point[b][1] * activity[b] for b in bs) / wsum
            else:
                lat = sum(point[b][0] for b in bs) / len(bs)
                lon = sum(point[b][1] for b in bs) / len(bs)
            wr.writerow([index[t], t, t[:5], counties[t[:5]], f"{lat:.6f}", f"{lon:.6f}", len(bs)])
    summary = {}
    for y, table in flows.items():
        with open(out / f"flows-{y}.csv", "w", encoding="utf-8", newline="") as f:
            wr = csv.writer(f)
            wr.writerow(["home", "work", "jobs"])
            for (h, w), n in sorted(table.items(), key=lambda kv: (index[kv[0][0]], index[kv[0][1]])):
                wr.writerow([index[h], index[w], n])
        summary[str(y)] = {"jobs": sum(table.values()), "pairs": len(table), "file": f"flows-{y}.csv"}
    record = {
        "case": case,
        "description": spec["description"],
        "source": "LEHD Origin-Destination Employment Statistics (LODES) 8, US Census Bureau, " + f"{BASE}{st}/",
        "licence": "Public domain: a work of the US Government (17 U.S.C. § 105)",
        "geography": "2020 census tracts; points weighted by the jobs and workers of their blocks over the years fetched",
        "counties": counties,
        "tracts": len(tracts),
        "years": summary,
        "retrieved": time.strftime("%Y-%m-%d"),
    }
    json.dump(record, open(out / "index.json", "w", encoding="utf-8"), indent=1)
    return record


def main() -> None:
    args = sys.argv[1:]
    case = args[0] if args and not args[0].isdigit() else "nyc-lodes"
    years = [int(a) for a in args if a.isdigit()] or None
    r = build(case, years)
    print(json.dumps({k: r[k] for k in ("case", "tracts", "years")}, indent=1))


if __name__ == "__main__":
    main()
