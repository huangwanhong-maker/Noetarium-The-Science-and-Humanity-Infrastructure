"""Regions' employment by industry at two dates, NACE divisions within their sections, for the structure of regional economies (ADR
0166), written into data/cache/products/regional-structure/.

From Eurostat's structural business statistics by NUTS 2 region (sbs_r_nuts06_r2, NACE Rev. 2, 2008–2020): persons employed in local
units (V16110) by NACE division (two digits) in the business economy (sections B to N less K). Cells Eurostat withholds for confidentiality
stay empty and are said to be. A case is a list of countries; every NUTS 2 region of each is read. Eurostat's data are reused with the
source acknowledged.

    python data/products/regional_structure.py [--case europe] [--from 2011] [--to 2019]
"""

from __future__ import annotations

import argparse
import json
import re
import sys
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "regional-structure" / "raw"
OUT = CACHE / "products" / "regional-structure"
EUROSTAT = "https://ec.europa.eu/eurostat/api/dissemination/statistics/1.0/data/"
CREDIT = "Eurostat (sbs_r_nuts06_r2), reused with the source acknowledged"

CASES = {
    "europe": ["AT", "BE", "BG", "CZ", "DE", "DK", "EE", "EL", "ES", "FI", "FR", "HR", "HU", "IE", "IT", "LT", "LU", "LV", "NL", "PL", "PT",
               "RO", "SE", "SI", "SK", "NO"],
    "central-europe": ["AT", "CZ", "DE", "HU", "PL", "SI", "SK"],
}
DIVISION = re.compile(r"^[B-N]\d\d$")


def fetch_all(years: list[int]) -> dict:
    path = RAW / f"sbs_r_nuts06_r2-nuts2-{'-'.join(map(str, years))}.json"
    if not path.is_file():
        RAW.mkdir(parents=True, exist_ok=True)
        t = "".join(f"&time={y}" for y in years)
        url = f"{EUROSTAT}sbs_r_nuts06_r2?indic_sb=V16110&geoLevel=nuts2{t}&lang=EN"
        with urllib.request.urlopen(urllib.request.Request(url, headers={"User-Agent": "noetarium-data/1"}), timeout=900) as r:
            path.write_bytes(r.read())
    d = json.loads(path.read_text(encoding="utf-8"))
    if "error" in d or not d.get("value"):
        raise LookupError(f"Eurostat holds no regional employment by division for {years}")
    return d


def build(case: str, y0: int, y1: int) -> dict:
    d = fetch_all([y0, y1])
    ids, sizes = d["id"], d["size"]
    idx = {k: d["dimension"][k]["category"]["index"] for k in ids}
    lab = {k: d["dimension"][k]["category"].get("label", {}) for k in ids}

    def get(nace: str, geo: str, year: int):
        k = 0
        for dim, size in zip(ids, sizes):
            code = {"nace_r2": nace, "geo": geo, "time": str(year)}.get(dim)
            pos = 0 if code is None else idx[dim].get(code)
            if pos is None:
                return None
            k = k * size + pos
        v = d["value"].get(str(k))
        return None if v is None else float(v)

    countries = set(CASES[case])
    regions = sorted(g for g in idx["geo"] if len(g) == 4 and g[:2] in countries and not g.endswith("ZZ"))
    divisions = sorted(c for c in idx["nace_r2"] if DIVISION.match(c))
    table = {str(y): [[get(c, g, y) for c in divisions] for g in regions] for y in (y0, y1)}
    # Keep the regions with employment recorded at both dates, and the divisions recorded somewhere at both.
    keep_r = [i for i, g in enumerate(regions) if any(v for v in table[str(y0)][i]) and any(v for v in table[str(y1)][i])]
    keep_c = [j for j in range(len(divisions)) if any(table[str(y0)][i][j] for i in keep_r) and any(table[str(y1)][i][j] for i in keep_r)]
    out_regions = [{"nuts": regions[i], "name": lab["geo"].get(regions[i], regions[i]), "country": regions[i][:2]} for i in keep_r]
    out_ind = [{"code": divisions[j], "label": lab["nace_r2"].get(divisions[j], divisions[j]), "section": divisions[j][0]} for j in keep_c]
    emp = {str(y): [[table[str(y)][i][j] for j in keep_c] for i in keep_r] for y in (y0, y1)}
    withheld = sum(v is None for y in (y0, y1) for row in emp[str(y)] for v in row)
    return {"case": case, "years": [y0, y1], "indicator": "persons employed in local units (V16110)", "regions": out_regions, "industries": out_ind,
            "employment": emp, "withheld_cells": withheld, "credit": CREDIT, "fetched": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--case", default="europe", choices=sorted(CASES))
    ap.add_argument("--from", dest="y0", type=int, default=2011)
    ap.add_argument("--to", dest="y1", type=int, default=2019)
    a = ap.parse_args()
    p = build(a.case, a.y0, a.y1)
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{a.case}-{a.y0}-{a.y1}.json"
    path.write_text(json.dumps(p, ensure_ascii=False), encoding="utf-8")
    cells = len(p["regions"]) * len(p["industries"]) * 2
    print(f"{len(p['regions'])} regions × {len(p['industries'])} divisions, {a.y0} and {a.y1}; {p['withheld_cells']} of {cells} cells withheld")
    print(f"wrote {path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
