"""The finances of the local government sector of each European country (ADR 0145), written into
data/cache/products/local-government-finance/.

From Eurostat's government finance statistics for the sector S1313 (local government), in millions of euro: total revenue (TR),
total expenditure (TE), net lending or borrowing (B9) and interest payable (D41PAY) from gov_10a_main; the gross debt at nominal
value (GD) at the end of each year, the fourth quarter of gov_10q_ggdebt (the annual gov_10dd_edpt1 holds only recent years for
many countries); and nominal GDP (B1GQ, CP_MEUR) from nama_10_gdp. Derived for each year: debt over GDP, the
effective interest rate (interest over last year's debt), nominal growth, the primary balance over GDP ((B9 + D41PAY) / GDP), and
the stock-flow adjustment the identity d_t = d_{t−1} (1 + r_t)/(1 + g_t) − pb_t leaves.

Aggregates (EU27_2020, EA20 and the like) are kept and marked. Eurostat's data are reused with the source acknowledged.

    python data/products/local_government_finance.py [--from 1995] [--to 2024]
"""

from __future__ import annotations

import argparse
import json
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = ROOT / "data" / "cache"
RAW = CACHE / "local-government-finance" / "raw"
OUT = CACHE / "products" / "local-government-finance"
EUROSTAT = "https://ec.europa.eu/eurostat/api/dissemination/statistics/1.0/data/"
CREDIT = "Eurostat (gov_10a_main, gov_10q_ggdebt, nama_10_gdp), reused with the source acknowledged"


def fetch(query: str, name: str) -> dict:
    path = RAW / name
    if not path.is_file():
        RAW.mkdir(parents=True, exist_ok=True)
        req = urllib.request.Request(EUROSTAT + query, headers={"User-Agent": "noetarium-data/1"})
        with urllib.request.urlopen(req, timeout=600) as r:
            path.write_bytes(r.read())
    return json.loads(path.read_text(encoding="utf-8"))


def cube(d: dict) -> dict:
    """A JSON-stat dataset as {(code of each dimension, in order): value}."""
    ids, sizes = d["id"], d["size"]
    cats = []
    for dim in ids:
        idx = d["dimension"][dim]["category"]["index"]
        order = sorted(idx, key=idx.get) if isinstance(idx, dict) else list(idx)
        cats.append(order)
    out = {}
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


def labels(d: dict, dim: str) -> dict:
    return dict(d["dimension"][dim]["category"].get("label", {}))


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--from", dest="first", type=int, default=1995)
    ap.add_argument("--to", dest="last", type=int, default=2024)
    a = ap.parse_args()
    span = f"&sinceTimePeriod={a.first}&untilTimePeriod={a.last}"
    main_ = fetch("gov_10a_main?format=JSON&lang=EN&sector=S1313&unit=MIO_EUR&na_item=TR&na_item=TE&na_item=B9&na_item=D41PAY" + span, f"gov_10a_main-{a.first}-{a.last}.json")
    debt = fetch("gov_10q_ggdebt?format=JSON&lang=EN&sector=S1313&unit=MIO_EUR&na_item=GD", "gov_10q_ggdebt.json")
    gdp = fetch("nama_10_gdp?format=JSON&lang=EN&na_item=B1GQ&unit=CP_MEUR" + span, f"nama_10_gdp-{a.first}-{a.last}.json")
    m, dd, gg = cube(main_), cube(debt), cube(gdp)
    names = labels(main_, "geo")
    ids_m = main_["id"]
    years = list(range(a.first, a.last + 1))

    def series(c: dict, ids: list, geo: str, item: str | None, quarter: str = "") -> list:
        out = []
        for y in years:
            key = []
            for dim in ids:
                key.append({"geo": geo, "time": f"{y}{quarter}", "na_item": item}.get(dim) or None)
            match = [v for k, v in c.items() if all(kk == want for kk, want in zip(k, key) if want is not None)]
            out.append(match[0] if match else None)
        return out

    countries = []
    for geo in sorted(names):
        tr, te, b9, it = (series(m, ids_m, geo, x) for x in ("TR", "TE", "B9", "D41PAY"))
        gd = series(dd, debt["id"], geo, "GD", "-Q4")
        y = series(gg, gdp["id"], geo, "B1GQ")
        if sum(v is not None for v in gd) < 5 or sum(v is not None for v in y) < 5:
            continue
        d, r, g, pb, sfa = [], [], [], [], []
        for k in range(len(years)):
            d.append(gd[k] / y[k] if gd[k] is not None and y[k] else None)
            pb.append((b9[k] + it[k]) / y[k] if None not in (b9[k], it[k]) and y[k] else None)
            if k and gd[k - 1] and it[k] is not None:
                r.append(it[k] / gd[k - 1])
            else:
                r.append(None)
            g.append(y[k] / y[k - 1] - 1 if k and y[k] and y[k - 1] else None)
            if k and None not in (d[k], d[k - 1], r[k], g[k], pb[k]):
                sfa.append(d[k] - (d[k - 1] * (1 + r[k]) / (1 + g[k]) - pb[k]))
            else:
                sfa.append(None)
        countries.append({"geo": geo, "name": names[geo], "aggregate": geo.startswith(("EU", "EA")), "years": years,
                          "revenue_meur": tr, "expenditure_meur": te, "net_lending_meur": b9, "interest_meur": it, "debt_meur": gd, "gdp_meur": y,
                          "debt_share": d, "interest_rate": r, "growth": g, "primary_balance_share": pb, "stock_flow_adjustment": sfa})
    doc = {"title": "The local government sector of European countries, Eurostat", "sector": "S1313 (local government)", "unit": "millions of euro; shares of GDP",
           "countries": countries, "credit": CREDIT, "retrieved_at": datetime.now(timezone.utc).isoformat(timespec="seconds"),
           "said": "aggregates (EU27_2020, EA20, …) are marked; the effective interest rate is interest payable over last year's gross debt; the stock-flow adjustment is what the identity of debt dynamics leaves"}
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / "eurostat-s1313.json"
    path.write_text(json.dumps(doc, indent=1), encoding="utf-8")
    print(json.dumps({"path": str(path.relative_to(ROOT)), "countries": len(countries)}, indent=1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
