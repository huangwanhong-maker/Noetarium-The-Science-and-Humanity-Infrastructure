"""Cases of allocation under scarcity (ADR 0136): what is allocated, to whom, and the records a round needs, written for the allocation
domain's plug-in of the Urban Studio and for scripts, into data/cache/products/allocation/.

- ``pabulib-<instance>.json``: a participatory budget from Pabulib (data/formats/pabulib.py): its budget, the projects' costs, names,
  categories and places, what the city selected, and every voter's approval set with their neighbourhood. Pabulib states no licence,
  so the file stays in the cache and only results derived from it are published, with its credit.
- ``england-iod2025.json``: England's 296 local authority districts (lower tier, 2024 boundaries) from the English Indices of
  Deprivation 2025 (File 7, OGL v3.0): population, the IMD score weighted by population, people income-deprived and
  employment-deprived (each LSOA's rate times its denominator, summed), and each LSOA's population, income-deprived people and
  decile, for targeting. The kind of authority (London borough, metropolitan district, unitary, shire district) is read from its
  code. Districts under a county are one of two tiers that both receive funding; the second tier is not modelled.
- ``melbourne-areas.json``: the City of Melbourne's SA2 areas at the 2021 Census, from the needs product
  (data/products/needs_tables.py): persons, people needing assistance with core activities, people on low incomes, renting and
  social housing, as the ABS counts them (CC BY 4.0).

    python data/products/allocation_cases.py [pabulib [--instance Poland_Warszawa_2023]] [england] [melbourne]

The amounts any round allocates on these cases are declared by whoever declares the round; nothing here says what any authority
receives or should receive.
"""

from __future__ import annotations

import argparse
import csv
import json
import sys
import urllib.request
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "data" / "cache" / "products" / "allocation"
RAW = OUT / "raw"
sys.path.insert(0, str(ROOT / "data"))

from formats import pabulib as pbf  # noqa: E402

IOD = "https://assets.publishing.service.gov.uk/media/691ded56d140bbbaa59a2a7d/File_7_IoD2025_All_Ranks_Scores_Deciles_Population_Denominators.csv"
PABULIB = "https://pabulib.org/download/{instance}.pb"
KINDS = {"E09": "London borough", "E08": "metropolitan district", "E06": "unitary authority", "E07": "shire district"}


def fetch(url: str, path: Path) -> Path:
    if path.is_file() and path.stat().st_size > 0:
        return path
    path.parent.mkdir(parents=True, exist_ok=True)
    req = urllib.request.Request(url, headers={"User-Agent": "noetarium-data/1"})
    with urllib.request.urlopen(req, timeout=300) as r, open(path.with_suffix(path.suffix + ".part"), "wb") as f:
        while chunk := r.read(1 << 20):
            f.write(chunk)
    path.with_suffix(path.suffix + ".part").replace(path)
    return path


def write(name: str, doc: dict) -> Path:
    OUT.mkdir(parents=True, exist_ok=True)
    p = OUT / name
    p.write_text(json.dumps(doc, ensure_ascii=False, separators=(",", ":")), encoding="utf-8")
    index_path = OUT / "index.json"
    index = json.loads(index_path.read_text(encoding="utf-8")) if index_path.is_file() else {}
    index[name] = {"title": doc["title"], "credit": doc["credit"]}
    index_path.write_text(json.dumps(index, ensure_ascii=False, indent=1), encoding="utf-8")
    return p


def pabulib(instance: str = "Poland_Warszawa_2023") -> Path:
    raw = fetch(PABULIB.format(instance=instance), RAW / f"{instance}.pb")
    E = pbf.election(pbf.read(raw))
    hoods = sorted({v.get("neighborhood", "") for v in E["voters"] if v.get("neighborhood")})
    where = {h: k for k, h in enumerate(hoods)}
    doc = {
        "schema": "noetarium.allocation-case/1", "case": f"pabulib-{instance}", "kind": "election",
        "title": f"The participatory budget of {E['meta'].get('unit', instance)}, {E['meta'].get('instance', '')}: {len(E['costs'])} projects, {len(E['approvals']):,} voters",
        "budget": E["budget"], "currency": E["currency"], "costs": E["costs"], "approvals": E["approvals"],
        "projects": [{"id": E["ids"][k], "name": E["names"][k], "category": E["categories"][k], "cost": E["costs"][k], "votes": E["votes"][k],
                      "selected": E["selected"][k], "lat": E["lat"][k], "lon": E["lon"][k]} for k in range(len(E["costs"]))],
        "neighbourhoods": hoods, "voter_neighbourhood": [where.get(v.get("neighborhood", ""), -1) for v in E["voters"]],
        "city_rule": E["meta"].get("rule", ""), "reading": E["reading"], "unknown_votes": E["unknown_votes"],
        "credit": "Pabulib (Faliszewski et al. 2023), https://pabulib.org; no licence stated: kept in the cache, results derived from it published with this credit",
        "source": {"registry": "pabulib", "instance": instance, "url": PABULIB.format(instance=instance)},
    }
    return write(f"pabulib-{instance}.json", doc)


def england() -> Path:
    raw = fetch(IOD, RAW / "iod2025_file7.csv")
    with open(raw, encoding="utf-8-sig", newline="") as f:
        rows = list(csv.reader(f))
    head, body = rows[0], rows[1:]
    col = {h: k for k, h in enumerate(head)}

    def c(name: str) -> int:
        hits = [k for h, k in col.items() if h.startswith(name)]
        if not hits:
            raise KeyError(f"File 7 has no column beginning {name!r}")
        return hits[0]

    i_code, i_name = c("Local Authority District code"), c("Local Authority District name")
    i_imd, i_dec = c("Index of Multiple Deprivation (IMD) Score"), c("Index of Multiple Deprivation (IMD) Decile")
    i_inc, i_emp = c("Income Score (rate)"), c("Employment Score (rate)")
    i_pop, i_work = c("Total population: mid 2022"), c("Working age population 18-66")
    d = defaultdict(lambda: {"population": 0.0, "working_age": 0.0, "imd_weighted": 0.0, "income_deprived": 0.0, "employment_deprived": 0.0, "lsoas": 0, "most_deprived": 0})
    names, lsoa = {}, {"district": [], "population": [], "income_deprived": [], "decile": []}
    order = []
    for r in body:
        code = r[i_code]
        if code not in names:
            names[code] = r[i_name]
            order.append(code)
        pop, work = float(r[i_pop]), float(r[i_work])
        x = d[code]
        x["population"] += pop
        x["working_age"] += work
        x["imd_weighted"] += float(r[i_imd]) * pop
        x["income_deprived"] += float(r[i_inc]) * pop
        x["employment_deprived"] += float(r[i_emp]) * work
        x["lsoas"] += 1
        x["most_deprived"] += int(r[i_dec]) == 1
        lsoa["district"].append(order.index(code) if len(order) < 400 else None)
        lsoa["population"].append(round(pop))
        lsoa["income_deprived"].append(round(float(r[i_inc]) * pop, 1))
        lsoa["decile"].append(int(r[i_dec]))
    districts = []
    for code in order:
        x = d[code]
        districts.append({"id": code, "name": names[code], "group": KINDS.get(code[:3], "other"),
                          "at": {"population": round(x["population"]), "working_age": round(x["working_age"]), "imd_score": round(x["imd_weighted"] / x["population"], 3),
                                 "income_deprived": round(x["income_deprived"]), "employment_deprived": round(x["employment_deprived"]),
                                 "income_rate": round(x["income_deprived"] / x["population"], 4), "employment_rate": round(x["employment_deprived"] / x["working_age"], 4),
                                 "lsoas": x["lsoas"], "most_deprived_share": round(x["most_deprived"] / x["lsoas"], 4)}})
    doc = {
        "schema": "noetarium.allocation-case/1", "case": "england-iod2025", "kind": "entities",
        "title": f"England's {len(districts)} local authority districts by the Indices of Deprivation 2025",
        "entities": districts, "lsoas": lsoa,
        "sources": {"population": "MHCLG, IoD 2025 File 7: total population, mid-2022", "working_age": "MHCLG, IoD 2025 File 7: working-age population 18–66, mid-2022",
                    "imd_score": "MHCLG, IoD 2025 File 7: IMD score, weighted by LSOA population", "income_deprived": "MHCLG, IoD 2025 File 7: income score (rate) × population, summed",
                    "employment_deprived": "MHCLG, IoD 2025 File 7: employment score (rate) × working-age population, summed",
                    "income_rate": "income-deprived people over population", "employment_rate": "employment-deprived people over working-age population",
                    "most_deprived_share": "the share of the district's LSOAs in England's most deprived decile"},
        "caveats": ["The Index is relative and not a time series (the release's own warning).", "Many deprived people live in areas that are not deprived.",
                    "Districts under a county are one of two tiers that receive funding; the county tier is not modelled here."],
        "credit": "Ministry of Housing, Communities and Local Government, English Indices of Deprivation 2025 (Open Government Licence v3.0)",
        "source": {"registry": "mhclg-iod-2025", "url": IOD},
    }
    return write("england-iod2025.json", doc)


def melbourne() -> Path:
    src = ROOT / "data" / "cache" / "products" / "needs" / "melbourne-assistance-areas.csv"
    if not src.is_file():
        raise SystemExit("the needs product is not built: python data/products/needs_tables.py")
    with open(src, encoding="utf-8", newline="") as f:
        rows = list(csv.DictReader(f))
    a = defaultdict(lambda: {"persons": 0.0, "need": 0.0, "need_stated": 0.0, "low": 0.0, "low_stated": 0.0, "renting": 0.0, "social": 0.0, "lat": 0.0, "lon": 0.0, "sa1": 0})
    for r in rows:
        x = a[r["sa2"]]
        p = float(r["persons"] or 0)
        x["persons"] += p
        x["need"] += float(r["need_k"] or 0)
        x["need_stated"] += float(r["need_n"] or 0)
        x["low"] += float(r["low_k"] or 0)
        x["low_stated"] += float(r["low_n"] or 0)
        x["renting"] += float(r["renting"] or 0) * p
        x["social"] += float(r["social_housing"] or 0) * p
        x["lat"] += float(r["lat"]) * p
        x["lon"] += float(r["lon"]) * p
        x["sa1"] += 1
    areas = []
    for name, x in sorted(a.items()):
        if x["persons"] <= 0:
            continue
        areas.append({"id": name.lower().replace(" ", "-").replace("(", "").replace(")", ""), "name": name, "group": "",
                      "lat": round(x["lat"] / x["persons"], 6), "lon": round(x["lon"] / x["persons"], 6),
                      "at": {"population": round(x["persons"]), "need": round(x["need"]), "need_rate": round(x["need"] / max(x["need_stated"], 1), 4),
                             "low_income": round(x["low"]), "low_income_rate": round(x["low"] / max(x["low_stated"], 1), 4),
                             "renting": round(x["renting"] / x["persons"], 4), "social_housing": round(x["social"] / x["persons"], 4), "sa1s": x["sa1"]}})
    doc = {
        "schema": "noetarium.allocation-case/1", "case": "melbourne-areas", "kind": "entities",
        "title": f"The City of Melbourne's {len(areas)} SA2 areas at the 2021 Census: need for assistance, low income and housing",
        "entities": areas,
        "sources": {"population": "ABS, 2021 Census, G01 by SA1, summed to SA2", "need": "ABS, 2021 Census, G18: people needing assistance with core activities",
                    "need_rate": "need over people who stated it", "low_income": "ABS, 2021 Census, G17: people with personal income below the needs product's line",
                    "low_income_rate": "low income over people who stated their income", "renting": "ABS, 2021 Census, G36/G37: households renting, by persons",
                    "social_housing": "ABS, 2021 Census, G37: households renting from a state or community provider, by persons"},
        "credit": "Source: ABS, 2021 Census of Population and Housing (CC BY 4.0), via the needs product of the data library",
        "source": {"registry": "abs-census-2021-sa1-vic", "product": "data/cache/products/needs/melbourne-assistance-areas.csv"},
    }
    return write("melbourne-areas.json", doc)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("cases", nargs="*", default=["pabulib", "england", "melbourne"])
    ap.add_argument("--instance", default="Poland_Warszawa_2023")
    a = ap.parse_args(argv)
    for case in a.cases:
        p = pabulib(a.instance) if case == "pabulib" else england() if case == "england" else melbourne() if case == "melbourne" else None
        if p is None:
            raise SystemExit(f"no case {case}: pabulib, england or melbourne")
        print(f"wrote {p.relative_to(ROOT)} ({p.stat().st_size / 1e6:.1f} MB)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
