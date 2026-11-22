"""A country's symmetric input–output table at basic prices, product by product, domestic and imported flows apart (ADR 0157), written
into data/cache/products/input-output/.

From Eurostat's naio_10_cp1750 (product by product, millions of euro): the domestic intermediate flows among 65 products (the CPA's
64 of the published tables, real estate split into imputed rents and the rest, L68A and L68B), each product's domestic final use, and,
column by column, its gross value added (B1G), net taxes on products (D21X31) and imported intermediate inputs (the column total of
imported uses); and the domestic share of each product in households' consumption (P3_S14). Eurostat's data are reused with the source
acknowledged.

    python data/products/input_output.py [--geo CZ] [--year 2022]
"""

from __future__ import annotations

import argparse
import json
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "input-output" / "raw"
OUT = CACHE / "products" / "input-output"
EUROSTAT = "https://ec.europa.eu/eurostat/api/dissemination/statistics/1.0/data/"
PRODUCTS = ("A01,A02,A03,B,C10-12,C13-15,C16,C17,C18,C19,C20,C21,C22,C23,C24,C25,C26,C27,C28,C29,C30,C31_32,C33,D,E36,E37-39,F,G45,G46,G47,"
            "H49,H50,H51,H52,H53,I,J58,J59_60,J61,J62_63,K64,K65,K66,L68A,L68B,M69_70,M71,M72,M73,M74_75,N77,N78,N79,N80-82,O,P,Q86,Q87_88,"
            "R90-92,R93,S94,S95,S96,T,U").split(",")


def fetch(geo: str, year: int) -> dict:
    path = RAW / f"naio_10_cp1750-{geo}-{year}.json"
    if not path.is_file():
        RAW.mkdir(parents=True, exist_ok=True)
        url = f"{EUROSTAT}naio_10_cp1750?geo={geo}&time={year}&unit=MIO_EUR&lang=EN"
        with urllib.request.urlopen(urllib.request.Request(url, headers={"User-Agent": "noetarium-data/1"}), timeout=600) as r:
            path.write_bytes(r.read())
    d = json.loads(path.read_text(encoding="utf-8"))
    if "error" in d:
        raise RuntimeError(f"Eurostat refused naio_10_cp1750 for {geo} {year}: {d['error']}")
    if not d.get("value"):
        raise LookupError(f"Eurostat holds no table for {geo} {year}")
    return d


def reader(d: dict):
    ids, sizes = d["id"], d["size"]
    idx = {dim: d["dimension"][dim]["category"]["index"] for dim in ids}
    values = d["value"]

    def get(**codes):
        k = 0
        for dim, size in zip(ids, sizes):
            cat = idx[dim]
            code = codes.get(dim)
            if code is None:
                if size != 1:
                    raise KeyError(dim)
                pos = 0
            else:
                if code not in cat:
                    return None
                pos = cat[code]
            k = k * size + pos
        v = values.get(str(k)) if isinstance(values, dict) else values[k]
        return None if v is None else float(v)
    return get


def build(geo: str, year: int) -> dict:
    d = fetch(geo, year)
    g = reader(d)
    labels = d["dimension"]["ind_use"]["category"]["label"]
    z = [[g(ind_ava=r, ind_use=c, stk_flow="DOM") or 0.0 for c in PRODUCTS] for r in PRODUCTS]
    final = [g(ind_ava=r, ind_use="TFU", stk_flow="DOM") or 0.0 for r in PRODUCTS]
    gva = [g(ind_ava="B1G", ind_use=c, stk_flow="TOTAL") or 0.0 for c in PRODUCTS]
    taxes = [g(ind_ava="D21X31", ind_use=c, stk_flow="TOTAL") or 0.0 for c in PRODUCTS]
    imported = [g(ind_ava="TOTAL", ind_use=c, stk_flow="IMP") or 0.0 for c in PRODUCTS]
    output = [g(ind_ava="P1", ind_use=c, stk_flow="TOTAL") or 0.0 for c in PRODUCTS]
    share = []
    for r in PRODUCTS:
        dom, tot = g(ind_ava=r, ind_use="P3_S14", stk_flow="DOM"), g(ind_ava=r, ind_use="P3_S14", stk_flow="TOTAL")
        share.append(round(dom / tot, 6) if dom is not None and tot else 1.0)
    rows = [sum(z[i]) + final[i] for i in range(len(PRODUCTS))]
    cols = [sum(z[i][j] for i in range(len(PRODUCTS))) + imported[j] + taxes[j] + gva[j] for j in range(len(PRODUCTS))]
    worst = max((abs(rows[j] - cols[j]) / rows[j] for j in range(len(PRODUCTS)) if rows[j] > 0), default=0.0)
    return {"geo": geo, "year": year, "unit": "million euro", "products": PRODUCTS, "labels": [labels.get(c, c) for c in PRODUCTS],
            "domestic": z, "final_domestic": final, "gva": gva, "taxes": taxes, "imported": imported, "output": output,
            "households_domestic_share": share, "largest_imbalance": round(worst, 9),
            "credit": "Eurostat (naio_10_cp1750), reused with the source acknowledged",
            "fetched": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--geo", default="CZ")
    ap.add_argument("--year", type=int, default=2022)
    a = ap.parse_args()
    t = build(a.geo, a.year)
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{a.geo}-{a.year}.json"
    path.write_text(json.dumps(t, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"{a.geo} {a.year}: {len(t['products'])} products, output {sum(t['output']):,.0f} M EUR, value added {sum(t['gva']):,.0f} M EUR, "
          f"the largest column's imbalance {t['largest_imbalance']:.2e} of its output")
    print(f"wrote {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
