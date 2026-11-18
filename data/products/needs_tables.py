"""Tables of units for the analysis of needs (ADR 0125): people by place and kind, with indicators of their state, the attributes
groups may be stated in, and what the tests of a need may use (clusters, places, suppliers, a causal graph, rules), written as one
document, noetarium.needs-data/1, which the API's science.needs reads and the needs domain judges.

A case is a recipe that writes a document; adding a city or a subject is adding a case, and nothing downstream names one.

    melbourne-assistance   The City of Melbourne's people at the 2021 Census, by Statistical Area 1, sex and age band:
                           - need for assistance (G18): persons with a profound or severe disability who need help with self-care,
                             mobility or communication, of those who stated whether they do (k of n);
                           - low personal income (G17A–C): persons aged 15 and over with a total personal income under $500 a week,
                             of those who stated it;
                           - unpaid care given (G25): persons aged 15 and over who gave unpaid help to someone with a disability, a
                             long-term condition or old age, of those who stated it;
                           with each area's tenure, flats, household income and moves (G37, G36, G02, G44) as attributes, the area its
                           cluster and its place; and as suppliers of care, CLUE's blocks by their floor space of hospitals and clinics,
                           community use and institutional accommodation in 2021. Areas are the SA1s whose point lies in a CLUE block
                           (data/products/housing_records.py, which writes them).
                           Declared, not measured, and said to be in the document: the persons a square metre of care floor space serves
                           (one per 40 m²); the catchment (800 m); the causal graph; the illustrative rules (no institution's).

    python data/products/needs_tables.py [case]     → data/cache/products/needs/<case>.json, <case>-areas.csv and needs/index.json

Beside the document, a table of its areas (<case>-areas.csv): each area's point in latitude and longitude, its people and the counts of
each indicator summed over its rows, for the studio's maps and boards. The points are carried from the city model's local frame by
the tangent plane's inverse at its origin, good to about a metre across the city.

The ABS perturbs small cells (its confidentiality), so that a table's cells need not sum to its totals; counts are taken as given.
Source: ABS, 2021 Census of Population and Housing, General Community Profile DataPack (CC BY 4.0), ABS data used with permission from
the Australian Bureau of Statistics; CLUE: City of Melbourne (CC BY 4.0).
"""

from __future__ import annotations

import csv
import json
import math
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
CACHE = ROOT / "data" / "cache"
OUT = CACHE / "products" / "needs"
CENSUS = CACHE / "abs-census-2021-sa1-vic"
HOUSING = CACHE / "products" / "housing-melbourne.json"
FLOOR = CACHE / "melbourne-clue-floor-space" / "all" / "floor-space.csv"

G18_BANDS = [("0_4", 0), ("5_14", 5), ("15_19", 15), ("20_24", 20), ("25_34", 25), ("35_44", 35), ("45_54", 45), ("55_64", 55),
             ("65_74", 65), ("75_84", 75), ("85_over", 85)]                       # then the total, in groups of four columns
G17_INCOME = ["neg_nil", "1_149", "150_299", "300_399", "400_499", "500_649", "650_799", "800_999", "1000_1249", "1250_1499",
              "1500_1749", "1750_1999", "2000_2999", "3000_3499", "3500_more", "not_stated", "total"]   # each with ten ages
G17_LOW = 5                                                                          # the first five bands: under $500 a week
ADULT_BANDS = [15, 20, 25, 35, 45, 55, 65, 75, 85]                                   # G17's and G25's ages, then the total
CARE_USES = ["hospital_clinic", "community_use", "institutional_accommodation"]
M2_PER_PERSON = 40.0
REACH = 800.0


def read(path: Path, delimiter: str = ",") -> tuple[list[str], list[list[str]]]:
    with open(path, newline="", encoding="utf-8-sig") as f:
        rows = list(csv.reader(f, delimiter=delimiter))
    return rows[0], rows[1:]


def num(text) -> float:
    try:
        return float(text)
    except (TypeError, ValueError):
        return float("nan")


def by_code(path: Path) -> dict[str, list[float]]:
    head, rows = read(path)
    if head[0] != "SA1_CODE_2021":
        raise SystemExit(f"needs: {path.name} does not begin with SA1_CODE_2021")
    return {r[0]: [num(x) for x in r[1:]] for r in rows}


def need_for_assistance() -> dict[str, dict]:
    """G18 by position: for each sex (M, F, P) twelve bands (eleven and the total) of four columns (need, no need, not stated, total)."""
    head, _ = read(CENSUS / "2021Census_G18_VIC_SA1.csv")
    if len(head) != 1 + 3 * 12 * 4 or not head[1].startswith("M_0_4") or not head[49].startswith("F_0_4"):
        raise SystemExit("needs: G18's columns are not as expected")
    out = {}
    for code, v in by_code(CENSUS / "2021Census_G18_VIC_SA1.csv").items():
        cells = {}
        for s, sex in enumerate(["male", "female"]):
            for b, (_, age) in enumerate(G18_BANDS):
                base = (s * 12 + b) * 4
                cells[(sex, age)] = {"need": v[base], "no_need": v[base + 1], "not_stated": v[base + 2], "persons": v[base + 3]}
        out[code] = cells
    return out


def low_income() -> dict[str, dict]:
    """G17A–C joined by position: for each sex (M, F, P) seventeen categories of income, each with ten ages (nine and the total)."""
    heads, values = [], {}
    for part in "ABC":
        head, rows = read(CENSUS / f"2021Census_G17{part}_VIC_SA1.csv")
        heads += head[1:]
        for r in rows:
            values.setdefault(r[0], []).extend(num(x) for x in r[1:])
    if len(heads) != 3 * 17 * 10 or not heads[0].startswith("M_Neg") or not heads[170].startswith("F_Neg"):
        raise SystemExit("needs: G17's columns are not as expected")
    out = {}
    for code, v in values.items():
        cells = {}
        for s, sex in enumerate(["male", "female"]):
            for a, age in enumerate(ADULT_BANDS):
                col = lambda c: v[(s * 17 + c) * 10 + a]  # noqa: E731
                low = sum(col(c) for c in range(G17_LOW))
                stated = col(16) - col(15)
                cells[(sex, age)] = {"low": low, "stated": stated}
        out[code] = cells
    return out


def unpaid_care() -> dict[str, dict]:
    """G25 by position: for each sex (M, F, P) ten ages (nine and the total) of four columns (gave, did not, not stated, total)."""
    head, _ = read(CENSUS / "2021Census_G25_VIC_SA1.csv")
    if len(head) != 1 + 3 * 10 * 4 or not head[1].startswith("M_15_19"):
        raise SystemExit("needs: G25's columns are not as expected")
    out = {}
    for code, v in by_code(CENSUS / "2021Census_G25_VIC_SA1.csv").items():
        cells = {}
        for s, sex in enumerate(["male", "female"]):
            for a, age in enumerate(ADULT_BANDS):
                base = (s * 10 + a) * 4
                cells[(sex, age)] = {"gave": v[base], "did_not": v[base + 1]}
        out[code] = cells
    return out


def share(a, b) -> float:
    return a / b if b and b > 0 and not math.isnan(a) else float("nan")


def lat_lon(frame: dict, x: float, y: float) -> tuple[float, float]:
    """A point of the local east–north frame in latitude and longitude, by the tangent plane at the origin (WGS 84's radii there)."""
    a, f = 6378137.0, 1 / 298.257223563
    e2 = f * (2 - f)
    lat0, lon0 = math.radians(frame["origin"]["lat"]), math.radians(frame["origin"]["lon"])
    w = math.sqrt(1 - e2 * math.sin(lat0) ** 2)
    meridian, normal = a * (1 - e2) / w ** 3, a / w
    return math.degrees(lat0 + y / meridian), math.degrees(lon0 + x / (normal * math.cos(lat0)))


def areas_table(doc: dict) -> list[dict]:
    cols = {c["name"]: c["values"] for c in doc["columns"]}
    by: dict[str, dict] = {}
    for i, code in enumerate(cols["sa1"]):
        a = by.setdefault(code, {"sa1": code, "area": cols["area"][i], "sa2": cols["sa2"][i], "persons": 0.0, "need_k": 0.0, "need_n": 0.0,
                                 "low_k": 0.0, "low_n": 0.0, "carer_k": 0.0, "carer_n": 0.0})
        for k in ("persons", "need_k", "need_n", "low_k", "low_n", "carer_k", "carer_n"):
            v = cols[k][i]
            a[k] += v if isinstance(v, (int, float)) else 0.0
        for k in ("renting", "social_housing", "flats", "household_income", "moved", "care_m2_near"):
            a[k] = cols[k][i]
    out = []
    for code, a in by.items():
        x, y = doc["where"][code]
        lat, lon = lat_lon(doc["place"]["frame"], x, y)
        a["lat"], a["lon"] = round(lat, 6), round(lon, 6)
        a["need_share"] = round(a["need_k"] / a["need_n"], 5) if a["need_n"] else None
        a["low_share"] = round(a["low_k"] / a["low_n"], 5) if a["low_n"] else None
        a["carer_share"] = round(a["carer_k"] / a["carer_n"], 5) if a["carer_n"] else None
        out.append(a)
    return out


def melbourne_assistance() -> dict:
    housing = json.loads(HOUSING.read_text(encoding="utf-8"))
    census = housing["census"]
    cols = census["columns"]
    at = {c: i for i, c in enumerate(cols)}
    g36, g37, g02, g44 = (census[t] for t in ("G36", "G37", "G02", "G44"))
    need, income, care = need_for_assistance(), low_income(), unpaid_care()
    # Suppliers of care: CLUE's blocks in the year nearest 2021, at the blocks' points.
    blocks = {b[0]: (b[2], b[3]) for b in housing["blocks"]["rows"]}
    head, rows = read(FLOOR, ";")
    year_col, block_col = head.index("census_year"), head.index("block_id")
    years = sorted({int(r[year_col]) for r in rows})
    year = min(years, key=lambda y: (abs(y - 2021), -y))
    suppliers = []
    for r in rows:
        if int(r[year_col]) != year:
            continue
        bid = int(float(r[block_col]))
        m2 = sum(num(r[head.index(u)]) for u in CARE_USES if not math.isnan(num(r[head.index(u)])))
        if m2 > 0 and bid in blocks:
            x, y = blocks[bid]
            suppliers.append({"id": f"block {bid}", "x": x, "y": y, "capacity": round(m2 / M2_PER_PERSON, 1), "floor_m2": m2, "domain": "care"})
    # Provision near each area: care floor space within the catchment, per person of the area's 65 and over (a declared measure).
    table = {k: [] for k in ["sa1", "area", "sa2", "sex", "age", "persons", "need_k", "need_n", "low_k", "low_n", "carer_k", "carer_n",
                             "renting", "social_housing", "flats", "household_income", "moved", "care_m2_near"]}
    where, rings, dropped = {}, {}, {"no G18": 0, "nobody": 0}
    for row in census["rows"]:
        code = row[at["sa1"]]
        if code not in need:
            dropped["no G18"] += 1
            continue
        x, y = row[at["x"]], row[at["y"]]
        where[code] = [x, y]
        rings[code] = row[at["rings"]]
        t36, t37, t02, t44 = row[at["G36"]], row[at["G37"]], row[at["G02"]], row[at["G44"]]
        v = lambda t, names, c: (t[names.index(c)] or 0.0) if t and t[names.index(c)] is not None else float("nan")  # noqa: E731
        flats = share(v(t36, g36, "OPDs_Flt_apart_Tot_Dwgs"), v(t36, g36, "OPDs_Tot_OPDs_Dwellings") - v(t36, g36, "OPDs_Dwlling_structur_NS_Dwgs"))
        tenure_stated = v(t37, g37, "Total_Total") - v(t37, g37, "Ten_type_NS_Total")
        renting = share(v(t37, g37, "R_Tot_Total"), tenure_stated)
        social = share(v(t37, g37, "R_ST_h_auth_Total") + v(t37, g37, "R_Com_Hp_Total"), tenure_stated)
        income_hh = v(t02, g02, "Median_tot_hhd_inc_weekly")
        moved = share(v(t44, g44, "Difnt_Usl_add_1_yr_ago_Tot_P"), v(t44, g44, "Tot_P") - v(t44, g44, "N_stated_P"))
        near = sum(s["floor_m2"] for s in suppliers if math.hypot(s["x"] - x, s["y"] - y) <= REACH)
        for sex in ["female", "male"]:
            for _, age in G18_BANDS:
                c = need[code][(sex, age)]
                if not c["persons"] or c["persons"] <= 0:
                    dropped["nobody"] += 1
                    continue
                i = income.get(code, {}).get((sex, age))
                u = care.get(code, {}).get((sex, age))
                table["sa1"].append(code), table["area"].append(row[at["small_area"]]), table["sa2"].append(row[at["sa2_name"]])
                table["sex"].append(sex), table["age"].append(age), table["persons"].append(c["persons"])
                table["need_k"].append(c["need"]), table["need_n"].append(c["need"] + c["no_need"])
                table["low_k"].append(i["low"] if i else float("nan")), table["low_n"].append(i["stated"] if i else float("nan"))
                table["carer_k"].append(u["gave"] if u else float("nan")), table["carer_n"].append(u["gave"] + u["did_not"] if u else float("nan"))
                table["renting"].append(renting), table["social_housing"].append(social), table["flats"].append(flats)
                table["household_income"].append(income_hh)
                table["moved"].append(moved), table["care_m2_near"].append(near)
    kinds = {"sa1": "category", "area": "category", "sa2": "category", "sex": "category"}
    columns = [{"name": n, "kind": kinds.get(n, "number"), "values": [None if isinstance(v, float) and math.isnan(v) else v for v in vals]}
               for n, vals in table.items()]
    return {
        "schema": "noetarium.needs-data/1",
        "case": "melbourne-assistance",
        "title": "The City of Melbourne's people at the 2021 Census by area, sex and age: need for assistance, low income and unpaid care",
        "place": {"name": "City of Melbourne", "frame": housing["frame"]},
        "columns": columns,
        "weight": None,
        "attributes": ["area", "sex", "age", "renting", "social_housing", "flats", "household_income", "moved"],
        "indicators": [
            {"name": "need_assist", "share": True, "column": "need_k", "total": "need_n", "higher_is_worse": True, "domain": "care",
             "label": "need help with self-care, mobility or communication", "unit": "share of persons who stated it", "levels": []},
            {"name": "low_income", "share": True, "column": "low_k", "total": "low_n", "higher_is_worse": True, "domain": "income",
             "label": "personal income under $500 a week", "unit": "share of persons 15 and over who stated it", "levels": []},
            {"name": "unpaid_care", "share": True, "column": "carer_k", "total": "carer_n", "higher_is_worse": True, "domain": "carer-support",
             "label": "gave unpaid help to someone with a disability, a condition or old age", "unit": "share of persons 15 and over who stated it", "levels": []},
        ],
        "domains": ["care", "income", "carer-support"],
        "cluster": "sa1",
        "unit_place": "sa1",
        "where": where,
        "suppliers": [{k: s[k] for k in ("id", "x", "y", "capacity", "domain")} for s in suppliers],
        "reach": REACH,
        "graph": [["household_income", "need_k"], ["flats", "need_k"], ["age", "need_k"], ["household_income", "care_m2_near"],
                  ["flats", "care_m2_near"], ["care_m2_near", "need_k"]],
        "provision": {"care": "care_m2_near"},
        "rules": [{"text": "need care for age >= 65 by comparative need_assist higher", "source": "an illustrative scheme, no institution's"},
                  {"text": "need income for age >= 65 by comparative low_income higher", "source": "an illustrative scheme, no institution's"}],
        "implies": [],
        "map": {"rings": rings},
        "declared": {
            "persons_per_m2": f"one person served per {M2_PER_PERSON:g} m² of care floor space (hospitals and clinics, community use, institutional accommodation)",
            "reach": f"{REACH:g} m, straight line, between an area's point and a block's",
            "graph": "household income and the share of flats shape both where care is provided and the need for it; age shapes the need",
            "rules": "illustrative, no institution's",
            "floor_space_year": year,
        },
        "sources": [{"id": "abs-census-2021-sa1-vic", "tables": ["G18", "G17A", "G17B", "G17C", "G25", "G36", "G37", "G02", "G44"]},
                    {"id": "melbourne-clue-floor-space", "year": year}, {"id": "abs-asgs-2021-sa1"}, {"product": "housing-melbourne"}],
        "credit": "Source: ABS, 2021 Census of Population and Housing (CC BY 4.0), ABS data used with permission from the Australian Bureau of Statistics; "
                  "CLUE: City of Melbourne (CC BY 4.0)",
        "report": {"areas": len(where), "rows": len(table["sa1"]), "suppliers": len(suppliers), "dropped": dropped,
                   "persons": sum(table["persons"]), "need": sum(v for v in table["need_k"] if v == v)},
    }


CASES = {"melbourne-assistance": melbourne_assistance}


def main(argv: list[str]) -> int:
    which = argv[1:] or list(CASES)
    OUT.mkdir(parents=True, exist_ok=True)
    index_path = OUT / "index.json"
    index = json.loads(index_path.read_text(encoding="utf-8")) if index_path.exists() else {"cases": {}}
    for case in which:
        if case not in CASES:
            print(f"needs: no case {case} (there are {', '.join(CASES)})")
            return 2
        doc = CASES[case]()
        path = OUT / f"{case}.json"
        path.write_text(json.dumps(doc, ensure_ascii=False, separators=(",", ":")), encoding="utf-8")
        rows = areas_table(doc)
        names = ["sa1", "area", "sa2", "lat", "lon", "persons", "need_k", "need_n", "need_share", "low_k", "low_n", "low_share", "carer_k", "carer_n",
                 "carer_share", "renting", "social_housing", "flats", "household_income", "moved", "care_m2_near"]
        with open(OUT / f"{case}-areas.csv", "w", newline="", encoding="utf-8") as fh:
            w = csv.DictWriter(fh, fieldnames=names, extrasaction="ignore")
            w.writeheader()
            for r in rows:
                w.writerow(r)
        index["cases"][case] = {"file": path.name, "areas": f"{case}-areas.csv", "title": doc["title"], "report": doc["report"]}
        r = doc["report"]
        print(f"  {case}: {r['areas']} areas, {r['rows']} rows, {r['suppliers']} suppliers, {r['persons']:.0f} persons, "
              f"{r['need']:.0f} needing assistance; {path.stat().st_size / 1e6:.2f} MB")
    index_path.write_text(json.dumps(index, indent=2, ensure_ascii=False), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
