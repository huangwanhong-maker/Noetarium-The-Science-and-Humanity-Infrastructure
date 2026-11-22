"""The global flood depth–damage functions of the European Commission's Joint Research Centre (Huizinga, de Moel and Szewczyk 2017,
Global flood depth-damage functions: methodology and the database with guidelines, EUR 28552 EN, doi:10.2760/16510), made ready
for the disaster-risk domain's library of damage functions (ADR 0149).

The workbook gives, for six categories (residential, commercial and industrial buildings, transport, roads, agriculture), the share
of an asset damaged at flood depths of 0 to 6 m, one curve for each continent where the literature allowed one and a global curve
where it did not, each with the standard deviation of the curves it was averaged from; and, by country, maximum damage values in
euros of 2010 a square metre. A continent with no curve for a category is left out of that category ("-" in the workbook), never
filled. The workbook states no licence; the Commission's reuse policy (Decision 2011/833/EU) authorises reuse with the source
acknowledged. It is fetched into the cache and never committed.

    python data/products/damage_functions.py

writes data/cache/products/damage-functions.json:
    {schema, credit, source, retrieved, sha256, depth_unit,
     curves: [{category, region, depth: [m], ratio: [...], sd: [...] or null}],
     max_damage: {country: {category: {structure, content, total}}}}   (EUR 2010 per m², building-based)
"""

from __future__ import annotations

import datetime as dt
import hashlib
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "acquisition"))
sys.path.insert(0, str(HERE))
from fetch import download  # noqa: E402
from materials_and_flows import workbook  # noqa: E402
from product_index import OUT as PRODUCTS, register  # noqa: E402

CACHE = HERE.parent / "cache"
SOURCE = CACHE / "jrc-damage-functions"
URL = "https://publications.jrc.ec.europa.eu/repository/bitstream/JRC105688/copy_of_global_flood_depth-damage_functions__30102017.xlsx"
FILE = "copy_of_global_flood_depth-damage_functions__30102017.xlsx"
CREDIT = ("Huizinga, de Moel and Szewczyk (2017), Global flood depth-damage functions, European Commission JRC, EUR 28552 EN, "
          "doi:10.2760/16510; reuse with the source acknowledged (Commission Decision 2011/833/EU)")
REGIONS = ["Europe", "North America", "Central and South America", "Asia", "Africa", "Oceania", "Global"]
CATEGORIES = {"Residential buildings": "residential", "Commercial buildings": "commercial", "Industrial buildings": "industrial",
              "Transport": "transport", "Infrastructure - roads": "roads", "Agriculture": "agriculture"}
MAX_SHEETS = {"MaxDamage-Residential": "residential", "MaxDamage-Commercial": "commercial", "MaxDamage-Industrial": "industrial"}


def number(v):
    return v if isinstance(v, float) else None


def curves(rows: list[list]) -> list[dict]:
    """The sheet 'Damage functions': a category's name opens its block of nine depths; columns 2–8 the curves, 9–15 their spreads."""
    out, category, block = [], None, []

    def close():
        if not category or not block:
            return
        for k, region in enumerate(REGIONS):
            ratio = [number(r[2 + k]) if len(r) > 2 + k else None for r in block]
            if any(v is None for v in ratio):
                continue      # no curve for this region: left out, never filled
            sd = [number(r[9 + k]) if len(r) > 9 + k else None for r in block]
            out.append({"category": CATEGORIES[category], "region": region, "depth": [r[1] for r in block], "ratio": ratio,
                        "sd": sd if all(v is not None for v in sd) else None})

    for r in rows:
        if not r or len(r) < 2:
            continue
        head = r[0].strip() if isinstance(r[0], str) else None
        if head in CATEGORIES:
            close()
            category, block = head, []
        if category and isinstance(r[1], float):
            block.append(r)
    close()
    return out


def max_damage(sheets: dict) -> dict:
    out: dict = {}
    for sheet, category in MAX_SHEETS.items():
        for r in sheets.get(sheet, []):
            if len(r) >= 4 and isinstance(r[0], str) and all(isinstance(v, float) for v in r[1:4]):
                out.setdefault(r[0].strip(), {})[category] = {"structure": r[1], "content": r[2], "total": r[3]}
    return out


def main() -> int:
    SOURCE.mkdir(parents=True, exist_ok=True)
    path = SOURCE / FILE
    if not path.exists():
        download(URL, path, timeout=120, attempts=3)
    sha = hashlib.sha256(path.read_bytes()).hexdigest()
    sheets = workbook(path)
    C = curves(sheets["Damage functions"])
    M = max_damage(sheets)
    for c in C:
        if c["depth"] != [0.0, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0]:
            raise ValueError(f"unexpected depths for {c['category']} in {c['region']}: {c['depth']}")
        if any(b < a for a, b in zip(c["ratio"], c["ratio"][1:])) or not all(0 <= v <= 1 for v in c["ratio"]):
            raise ValueError(f"a curve that falls or leaves [0, 1]: {c['category']} in {c['region']}")
    product = {"schema": "damage-functions/1", "credit": CREDIT, "source": URL, "sha256": sha,
               "retrieved": dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"), "depth_unit": "m",
               "max_damage_unit": "EUR 2010 per m² of building footprint", "curves": C, "max_damage": M}
    PRODUCTS.mkdir(parents=True, exist_ok=True)
    (PRODUCTS / "damage-functions.json").write_text(json.dumps(product, indent=1, ensure_ascii=False) + "\n", encoding="utf-8")
    register("damage-functions", {"name": "jrc-2017", "title": "Global flood depth–damage functions (JRC, Huizinga et al. 2017)",
                                  "file": "damage-functions.json", "credit": CREDIT})
    by = {}
    for c in C:
        by.setdefault(c["category"], []).append(c["region"])
    print(f"damage-functions.json: {len(C)} curves ({'; '.join(f'{k}: {len(v)}' for k, v in by.items())}), maximum damage for {len(M)} countries")
    return 0


if __name__ == "__main__":
    sys.exit(main())
