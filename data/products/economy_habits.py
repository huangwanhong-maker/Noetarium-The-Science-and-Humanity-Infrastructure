"""Records an economy's habits are calibrated to (ADR 0141), written into data/cache/products/economy-habits/.

- From a symmetric input–output table at basic prices (Eurostat naio_10_cp1750, the EU27 by default, or any country it covers):
  households' local share of their consumption (the domestic part of final consumption by households, P3_S14, over the whole);
  firms' wages (compensation of employees, D1, over output, P1), their domestic supplies (domestic intermediate use over output) and
  their imported supplies (imported intermediate use over output); what remains of output (taxes on production, operating surplus,
  consumption of fixed capital) has no place in an economy of households and firms and is said to leave it.
- The velocity of households' money: households' final consumption expenditure (nama_10_gdp, P31_S14, the euro area) over the
  money stock M1 (the ECB's BSI, euro area, averaged over the year's months), per year and per month.

The accounts are the EU27's and the money the euro area's (no input–output table is published for the euro area); the record says so.
Eurostat's data are reused with the source acknowledged; the ECB's with the ECB cited as the source.

    python data/products/economy_habits.py [--geo EU27_2020] [--year 2022]
"""

from __future__ import annotations

import argparse
import json
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "economy-habits" / "raw"
OUT = CACHE / "products" / "economy-habits"
EUROSTAT = "https://ec.europa.eu/eurostat/api/dissemination/statistics/1.0/data/"
ECB_M1 = "https://data-api.ecb.europa.eu/service/data/BSI/M.U2.Y.V.M10.X.1.U2.2300.Z01.E?format=jsondata&startPeriod={y}-01&endPeriod={y}-12"


def fetch(url: str, name: str) -> dict:
    path = RAW / name
    if not path.is_file():
        RAW.mkdir(parents=True, exist_ok=True)
        req = urllib.request.Request(url, headers={"User-Agent": "noetarium-data/1"})
        with urllib.request.urlopen(req, timeout=300) as r:
            path.write_bytes(r.read())
    return json.loads(path.read_text(encoding="utf-8"))


def jsonstat_value(d: dict, **coords: str) -> float | None:
    """The value of a JSON-stat dataset at the given category codes (dimensions not named must have one category)."""
    ids, sizes = d["id"], d["size"]
    index = 0
    for dim, size in zip(ids, sizes):
        cat = d["dimension"][dim]["category"]["index"]
        code = coords.get(dim)
        if code is None:
            if size != 1:
                raise ValueError(f"the dimension {dim} has {size} categories; name one")
            pos = 0
        else:
            if code not in cat:
                return None
            pos = cat[code] if isinstance(cat, dict) else cat.index(code)
        index = index * size + pos
    v = d["value"].get(str(index)) if isinstance(d["value"], dict) else d["value"][index]
    return None if v is None else float(v)


def accounts(geo: str, year: int) -> dict:
    cached = CACHE / "eurostat-io-tables" / "eu27-2022" / "io-table.json"
    if geo == "EU27_2020" and year == 2022 and cached.is_file():
        d = json.loads(cached.read_text(encoding="utf-8"))
        source = "the data library's cached naio_10_cp1750 (EU27_2020, 2022)"
    else:
        d = fetch(f"{EUROSTAT}naio_10_cp1750?format=JSON&lang=EN&geo={geo}&time={year}&unit=MIO_EUR", f"io-{geo}-{year}.json")
        source = f"naio_10_cp1750 ({geo}, {year})"
    v = lambda ava, use, flow: jsonstat_value(d, ind_ava=ava, ind_use=use, stk_flow=flow)  # noqa: E731
    hh_dom, hh_all = v("TOTAL", "P3_S14", "DOM"), v("TOTAL", "P3_S14", "TOTAL")
    output, wages = v("P1", "TOTAL", "TOTAL"), v("D1", "TOTAL", "TOTAL")
    inter_dom, inter_imp = v("TOTAL", "TOTAL", "DOM"), v("TOTAL", "TOTAL", "IMP")
    if None in (hh_dom, hh_all, output, wages, inter_dom, inter_imp) or not output or not hh_all:
        raise ValueError(f"the table for {geo} {year} lacks a needed cell")
    shares = {"local": hh_dom / hh_all, "wages": wages / output, "supplies": inter_dom / output, "imported": inter_imp / output}
    shares["other"] = 1 - shares["wages"] - shares["supplies"] - shares["imported"]
    return {"geo": geo, "year": year, "source": source, "cells_meur": {"households_domestic": hh_dom, "households_all": hh_all, "output": output,
            "compensation_of_employees": wages, "intermediate_domestic": inter_dom, "intermediate_imported": inter_imp}, "shares": shares}


def velocity(year: int) -> dict:
    m1 = fetch(ECB_M1.format(y=year), f"ecb-m1-{year}.json")
    series = next(iter(m1["dataSets"][0]["series"].values()))["observations"]
    months = [float(o[0]) for o in series.values()]
    hfce = fetch(f"{EUROSTAT}nama_10_gdp?format=JSON&lang=EN&geo=EA20&na_item=P31_S14&unit=CP_MEUR&time={year}", f"hfce-ea20-{year}.json")
    consumption = jsonstat_value(hfce)
    m = sum(months) / len(months)
    return {"economy": "the euro area (EA20)", "year": year, "consumption_meur": consumption, "m1_meur_mean": m, "months": len(months),
            "per_year": consumption / m, "per_month": consumption / m / 12}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--geo", default="EU27_2020")
    ap.add_argument("--year", type=int, default=2022)
    a = ap.parse_args()
    doc = {"accounts": accounts(a.geo, a.year), "velocity": velocity(a.year),
           "said": "the accounts are " + a.geo + "'s and the money the euro area's: no input–output table is published for the euro area",
           "credit": "Eurostat (naio_10_cp1750, nama_10_gdp), reused with the source acknowledged; ECB Data Portal (BSI, M1), the ECB cited as the source",
           "retrieved_at": datetime.now(timezone.utc).isoformat(timespec="seconds")}
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / f"{a.geo.lower()}-{a.year}.json"
    path.write_text(json.dumps(doc, indent=1), encoding="utf-8")
    print(json.dumps({"path": str(path.relative_to(ROOT)), "shares": doc["accounts"]["shares"], "velocity": doc["velocity"]}, indent=1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
