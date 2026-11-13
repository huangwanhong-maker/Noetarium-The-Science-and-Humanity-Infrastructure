"""Time, health, care and work in Europe as tables, for the sociology and social-provision domains and the Urban Studio (ADR 0104, ADR 0105).

From Eurostat's datasets (data/registry/eurostat-*.json), under data/cache/products/time-and-care/:

    time-use.csv            country, wave, sex, age group, activity code and label, minutes a day (HETUS 2000 and 2010; 'hh:mm' read as
                            minutes); the activities of the first level of Eurostat's classification (AC0 to AC9) and their parts
    days.csv                one row a country, wave, sex and age group: the minutes of its day in eight parts (personal care and sleep;
                            paid work, its travel within it; study; household and family care; voluntary work and help; social life,
                            culture and rest; sport, hobbies and media; other travel) and their total, within minutes of 1,440
    healthy-life.csv        country, sex, year, life expectancy and healthy life years at birth and at 65, as Eurostat publishes them
    limitation.csv          country, sex, year, age band, the percentage with some or severe and with severe limitation (EU-SILC)
    life-tables.csv         country, sex, year, age, survivors l, person-years L, life expectancy e; single ages and the open last group
                            (95+), Eurostat's overlapping group 85+ left out
    population.csv          country, year, age, persons (1 January); single ages and the open last group (100+); country codes include
                            Eurostat's aggregates (EU27_2020, EA20, …), which tools must not add to their members
    projections.csv         country, year, age, persons (EUROPOP2023, baseline); single ages and the open last group (100+), the
                            overlapping groups (under 15, 15–64, 65+, 75+, 80+, …) left out
    labour-flows.csv        country, sex, quarter, from, to, thousands of persons (EMP, UNE, INAC), seasonally adjusted

Nothing is presumed here: what a cell does not hold is left empty, with Eurostat's own status flags dropped (they are in the source).

    python data/products/time_and_care.py
"""

from __future__ import annotations

import csv
import datetime as dt
import itertools
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

from product_index import OUT as PRODUCTS, register  # noqa: E402

CACHE = HERE.parents[0] / "cache"
OUT = PRODUCTS / "time-and-care"

# The eight parts of a day, from Eurostat's activities (acl00) of HETUS. AC0 + AC1_TR + AC2 + AC3 + AC4-8 + AC9A + AC99NSP is the whole day
# to within the rounding of its parts (a few minutes); commuting is within AC1_TR, the parts of AC4-8 sum to it. Unspecified time
# (AC99NSP) is left out; a tool closes the eight parts to 1,440 minutes.
PARTS = {
    "personal care and sleep": ["AC0"],
    "paid work": ["AC1_TR"],
    "study": ["AC2"],
    "household and family care": ["AC3"],
    "voluntary work and help": ["AC41", "AC42", "AC43"],
    "social life, culture and rest": ["AC51A", "AC51B", "AC52", "AC53"],
    "sport, hobbies and media": ["AC6A", "AC611", "AC7A", "AC7B", "AC733", "AC811", "AC812", "AC82", "AC83", "AC4-8NSP"],
    "travel": ["AC9A"],
}


def jsonstat(path: Path):
    """Rows of a JSON-stat 2.0 dataset: a dict of each dimension's code, and the value."""
    d = json.loads(path.read_text(encoding="utf-8"))
    ids, sizes = d["id"], d["size"]
    cats = []
    for k in ids:
        index = d["dimension"][k]["category"]["index"]
        order = sorted(index, key=lambda c: index[c]) if isinstance(index, dict) else list(index)
        cats.append(order)
    labels = {k: d["dimension"][k]["category"].get("label", {}) for k in ids}
    values = d["value"]
    getv = (lambda i: values.get(str(i))) if isinstance(values, dict) else (lambda i: values[i] if i < len(values) else None)
    for flat, combo in enumerate(itertools.product(*cats)):
        v = getv(flat)
        if v is None:
            continue
        yield dict(zip(ids, combo)), v, labels


def write(name: str, columns: list[str], rows: list[dict]) -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    path = OUT / name
    with path.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=columns)
        w.writeheader()
        for r in rows:
            w.writerow({k: ("" if r.get(k) is None else r[k]) for k in columns})
    return len(rows)


def minutes(v) -> float | None:
    if isinstance(v, (int, float)):
        return float(v) * 60
    try:
        h, m = str(v).split(":")
        return int(h) * 60 + int(m)
    except ValueError:
        return None


def single_age(code: str) -> int | None:
    if code == "Y_LT1":
        return 0
    if code.startswith("Y") and code[1:].isdigit():
        return int(code[1:])
    return None


def ages_of(path: Path):
    """The ages of a dataset's codes: single ages, and its one open last group, at the age it opens.

    Eurostat's age dimensions mix single ages with groups: proj_23np holds Y_GE65, Y_GE75 and Y_GE80 beside Y65 … Y99 and the open
    Y_GE100; demo_mlifetable holds Y_GE85 beside Y85 and the open Y_GE95; demo_pjan closes with Y_OPEN. Only the open last group is
    an age of the table; a group that overlaps single ages would count its people twice."""
    d = json.loads(path.read_text(encoding="utf-8"))
    index = d["dimension"]["age"]["category"]["index"]
    codes = list(index) if isinstance(index, dict) else list(index)
    singles = [a for a in (single_age(c) for c in codes) if a is not None]
    top = max(singles) if singles else -1
    groups = [int(c[4:]) for c in codes if c.startswith("Y_GE") and c[4:].isdigit()]
    open_code = None
    if groups and max(groups) > top:
        open_code = f"Y_GE{max(groups)}"
    elif "Y_OPEN" in codes:
        open_code = "Y_OPEN"

    def age_of(code: str) -> int | None:
        if code == open_code:
            return max(groups) if code.startswith("Y_GE") else top + 1
        return single_age(code)
    return age_of


def main() -> int:
    made = {}
    # Time use.
    rows, labels = [], {}
    for k, v, lab in jsonstat(CACHE / "eurostat-time-use/all/time-use.json"):
        m = minutes(v)
        if m is None:
            continue
        labels = lab
        rows.append({"country": k["geo"], "wave": k["time"], "sex": k["sex"], "age": k["age"], "activity": k["acl00"],
                     "label": lab["acl00"].get(k["acl00"], ""), "minutes": m})
    made["time-use.csv"] = write("time-use.csv", ["country", "wave", "sex", "age", "activity", "label", "minutes"], rows)
    days = {}
    for r in rows:
        key = (r["country"], r["wave"], r["sex"], r["age"])
        days.setdefault(key, {})[r["activity"]] = r["minutes"]
    drows = []
    for (country, wave, sex, age), acts in sorted(days.items()):
        if any(all(c not in acts for c in codes) for codes in PARTS.values()):
            continue  # a day missing a whole part is not a composition of the day
        row = {"country": country, "wave": wave, "sex": sex, "age": age, "name": labels.get("geo", {}).get(country, country)}
        total = 0
        for part, codes in PARTS.items():
            row[part] = sum(acts.get(c, 0) for c in codes)
            total += row[part]
        row["total"] = total
        drows.append(row)
    made["days.csv"] = write("days.csv", ["country", "name", "wave", "sex", "age", *PARTS, "total"], drows)
    # Healthy life years.
    hl = {}
    for k, v, _ in jsonstat(CACHE / "eurostat-healthy-life-years/all/healthy-life-years.json"):
        hl.setdefault((k["geo"], k["sex"], k["time"]), {})[k["hlth_hle"]] = v
    hrows = [{"country": g, "sex": s, "year": y, "le0": d.get("LE_Y0"), "hly0": d.get("HLY_Y0"), "le65": d.get("LE_Y65"), "hly65": d.get("HLY_Y65")}
             for (g, s, y), d in sorted(hl.items())]
    made["healthy-life.csv"] = write("healthy-life.csv", ["country", "sex", "year", "le0", "hly0", "le65", "hly65"], hrows)
    # Limitation.
    lim = {}
    for k, v, _ in jsonstat(CACHE / "eurostat-activity-limitation/all/activity-limitation.json"):
        lim.setdefault((k["geo"], k["sex"], k["time"], k["age"]), {})[k["lev_limit"]] = v
    lrows = [{"country": g, "sex": s, "year": y, "age": a, "from": int(a[1:3]) if a[1:3].isdigit() else int(a[4:]), "some_or_severe": d.get("SM_SEV"), "severe": d.get("SEV")}
             for (g, s, y, a), d in sorted(lim.items())]
    made["limitation.csv"] = write("limitation.csv", ["country", "sex", "year", "age", "from", "some_or_severe", "severe"], lrows)
    # Life tables.
    lt = {}
    age_of = ages_of(CACHE / "eurostat-life-tables/all/life-tables.json")
    for k, v, _ in jsonstat(CACHE / "eurostat-life-tables/all/life-tables.json"):
        age = age_of(k["age"])
        if age is None:
            continue
        lt.setdefault((k["geo"], k["sex"], k["time"], age), {})[k["indic_de"]] = v
    trows = [{"country": g, "sex": s, "year": y, "age": a, "l": d.get("SURVIVORS"), "L": d.get("PYLIVED"), "e": d.get("LIFEXP")}
             for (g, s, y, a), d in sorted(lt.items())]
    made["life-tables.csv"] = write("life-tables.csv", ["country", "sex", "year", "age", "l", "L", "e"], trows)
    # Population and projections.
    for src, name in (("eurostat-population/all/population.json", "population.csv"), ("eurostat-population-projections/all/projections.json", "projections.csv")):
        prow = []
        age_of = ages_of(CACHE / src)
        for k, v, _ in jsonstat(CACHE / src):
            age = age_of(k["age"])
            if age is None:
                continue
            prow.append({"country": k["geo"], "year": k["time"], "age": age, "persons": v})
        made[name] = write(name, ["country", "year", "age", "persons"], prow)
    # Labour flows.
    frows = []
    for k, v, _ in jsonstat(CACHE / "eurostat-labour-transitions/all/labour-transitions.json"):
        a, _, b = k["wstatus"].partition("_")
        if a not in ("EMP", "UNE", "INAC") or b not in ("EMP", "UNE", "INAC"):
            continue
        frows.append({"country": k["geo"], "sex": k["sex"], "quarter": k["time"], "from": a, "to": b, "thousands": v})
    made["labour-flows.csv"] = write("labour-flows.csv", ["country", "sex", "quarter", "from", "to", "thousands"], frows)
    register("time-and-care", {"name": "europe", "title": "Time, health, care and work in Europe (Eurostat)", "files": sorted(made),
                               "rows": made, "made": dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds"),
                               "credit": "Eurostat: HETUS (tus_00age), healthy life years (hlth_hlye), EU-SILC (hlth_silc_06), life tables (demo_mlifetable), population (demo_pjan), EUROPOP2023 (proj_23np), LFS transitions (lfsi_long_q)"})
    for k, n in made.items():
        print(f"{k}: {n} rows")
    return 0


if __name__ == "__main__":
    sys.exit(main())
