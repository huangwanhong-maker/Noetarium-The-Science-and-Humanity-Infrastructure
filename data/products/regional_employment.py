"""A region's employment by industry against its country's, mapped to the 65 products of the input–output tables (ADR 0159), written
into data/cache/products/regional-employment/.

From Eurostat's nama_10r_3empers (employment in thousands of persons by NUTS region and NACE group): ten groups that partition all
activities (agriculture; mining, energy and water; manufacturing; construction; trade, transport and lodging; information; finance; real
estate; professional and support services; public services; arts and other services). Each product of `input_output.py` takes its group's
regional share of national employment, a declared assumption that a region holds every product of a group in the same proportion.
Eurostat's data are reused with the source acknowledged.

    python data/products/regional_employment.py [--geo CZ01] [--country CZ] [--year 2022]
"""

from __future__ import annotations

import argparse
import json
import sys
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "regional-employment" / "raw"
OUT = CACHE / "products" / "regional-employment"
EUROSTAT = "https://ec.europa.eu/eurostat/api/dissemination/statistics/1.0/data/"
sys.path.insert(0, str(Path(__file__).parent))
from input_output import PRODUCTS  # noqa: E402

# The partition of activities: each group by its Eurostat code (one computed as a difference), and the products it holds.
GROUPS = {
    "A": ["A01", "A02", "A03"],
    "B-E less C": ["B", "D", "E36", "E37-39"],
    "C": ["C10-12", "C13-15", "C16", "C17", "C18", "C19", "C20", "C21", "C22", "C23", "C24", "C25", "C26", "C27", "C28", "C29", "C30", "C31_32", "C33"],
    "F": ["F"],
    "G-I": ["G45", "G46", "G47", "H49", "H50", "H51", "H52", "H53", "I"],
    "J": ["J58", "J59_60", "J61", "J62_63"],
    "K": ["K64", "K65", "K66"],
    "L": ["L68A", "L68B"],
    "M_N": ["M69_70", "M71", "M72", "M73", "M74_75", "N77", "N78", "N79", "N80-82"],
    "O-Q": ["O", "P", "Q86", "Q87_88"],
    "R-U": ["R90-92", "R93", "S94", "S95", "S96", "T", "U"],
}


def fetch(geo: str, country: str, year: int) -> dict:
    path = RAW / f"nama_10r_3empers-{geo}-{country}-{year}.json"
    if not path.is_file():
        RAW.mkdir(parents=True, exist_ok=True)
        url = f"{EUROSTAT}nama_10r_3empers?geo={geo}&geo={country}&wstatus=EMP&unit=THS&time={year}&lang=EN"
        with urllib.request.urlopen(urllib.request.Request(url, headers={"User-Agent": "noetarium-data/1"}), timeout=600) as r:
            path.write_bytes(r.read())
    d = json.loads(path.read_text(encoding="utf-8"))
    if "error" in d or not d.get("value"):
        raise LookupError(f"Eurostat holds no employment by industry for {geo} and {country} in {year}")
    return d


def build(geo: str, country: str, year: int) -> dict:
    d = fetch(geo, country, year)
    ids, sizes = d["id"], d["size"]
    idx = {k: d["dimension"][k]["category"]["index"] for k in ids}

    def get(nace: str, g: str):
        k = 0
        for dim, size in zip(ids, sizes):
            code = {"nace_r2": nace, "geo": g}.get(dim)
            pos = 0 if code is None else idx[dim].get(code)
            if pos is None:
                return None
            k = k * size + pos
        v = d["value"].get(str(k))
        return None if v is None else float(v)

    groups = {}
    for name in GROUPS:
        if name == "B-E less C":
            r, n = (get("B-E", geo) or 0) - (get("C", geo) or 0), (get("B-E", country) or 0) - (get("C", country) or 0)
        else:
            r, n = get(name, geo), get(name, country)
        if r is None or n is None:
            raise LookupError(f"Eurostat withholds the group {name} for {geo} or {country} in {year}")
        groups[name] = [r, n]
    total = [get("TOTAL", geo), get("TOTAL", country)]
    of = {p: g for g, ps in GROUPS.items() for p in ps}
    shares = {p: round(groups[of[p]][0] / groups[of[p]][1], 6) if groups[of[p]][1] > 0 else 0.0 for p in PRODUCTS}
    return {"geo": geo, "country": country, "year": year, "unit": "thousand persons", "groups": groups, "total": total,
            "size": round(total[0] / total[1], 6), "shares": shares, "partition_gap": round(sum(v[0] for v in groups.values()) - total[0], 3),
            "credit": "Eurostat (nama_10r_3empers), reused with the source acknowledged",
            "fetched": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--geo", default="CZ01")
    ap.add_argument("--country", default="CZ")
    ap.add_argument("--year", type=int, default=2022)
    a = ap.parse_args()
    e = build(a.geo, a.country, a.year)
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{a.geo}-{a.year}.json"
    path.write_text(json.dumps(e, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
    print(f"{a.geo} in {a.country}, {a.year}: {e['total'][0]:,.1f} of {e['total'][1]:,.1f} thousand employed ({e['size']:.1%}); the groups miss the total by {e['partition_gap']:g}")
    for g, (r, n) in e["groups"].items():
        print(f"  {g:12s} {r:8.1f} of {n:8.1f}  SLQ {r / n / e['size']:.2f}")
    print(f"wrote {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
