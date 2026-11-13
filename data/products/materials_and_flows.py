"""Materials and flows as tables, for the industrial-ecology domain and the Urban Studio (ADR 0106).

Under data/cache/products/materials-and-flows/:

    rasmi-scheme.json        RASMI's material intensities as a scheme of the domain's buildings module: one row per region (R5_32),
                             function (RS, RM, NR) and structure (C, M, S, T), with the 5th, 50th and 95th percentiles of kg per m²
                             of gross floor area for eight materials as its lower, central and upper bounds (declared: the bounds
                             are RASMI's range, not a confidence interval)
    rasmi-intensities.csv    the same, long: region, function, structure, material, p0, p5, p25, p50, p75, p95, p100
    melbourne-buildings.csv  the Hoddle Grid's buildings in CLUE's latest census, each joined to the OpenStreetMap footprint that holds
                             its point (the city model's): CLUE's use and its RASMI function (declared below), storeys, the footprint's
                             area (shared equally among CLUE's buildings in one footprint), floor area = footprint × storeys, the
                             year of construction, the block, the point; structure is not recorded and is left empty
    eu27-io-2022.csv         the EU27's domestic input–output table for 2022, industry by industry (64 industries, L68A and L68B
                             joined as L68), million euros: deliveries, then final uses (households, government, non-profit
                             institutions, gross fixed capital formation, inventories, valuables, exports) and output (the row sum)
    eu27-air-2022.csv        the EU27's air emissions accounts for 2022 by the same 64 industries, and households' own, tonnes:
                             greenhouse gases (CO₂-eq) and their CO₂, CH₄ and N₂O, acidifying gases (SO₂-eq), PM2.5

The functions of CLUE's uses, declared: residential apartments, student and institutional accommodation are residential
multi-family (RM); houses and townhouses residential single-family (RS); every other use non-residential (NR); a building recorded
as unoccupied (unused, under renovation, construction or demolition) has no known function, and the domain's rule then widens its
intensity over the three. Storeys are CLUE's, else OpenStreetMap's levels; a building with neither has no floor area and is kept,
empty, to be counted. A CLUE point outside every footprint is joined to the nearest footprint within 10 m, else left out and counted.

    python data/products/materials_and_flows.py
"""

from __future__ import annotations

import csv
import datetime as dt
import json
import math
import re
import sys
import xml.etree.ElementTree as ET
import zipfile
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

from product_index import OUT as PRODUCTS, register  # noqa: E402
from time_and_care import jsonstat  # noqa: E402

CACHE = HERE.parents[0] / "cache"
OUT = PRODUCTS / "materials-and-flows"

MATERIALS = ["concrete", "brick", "wood", "steel", "glass", "plastics", "aluminum", "copper"]
FUNCTIONS = {"RS": "residential single-family", "RM": "residential multi-family", "NR": "non-residential"}
STRUCTURES = {"C": "concrete", "M": "masonry", "S": "steel", "T": "timber"}
FUNCTION_OF_USE = {"Residential Apartment": "RM", "Student Accommodation": "RM", "Institutional Accommodation": "RM", "House/Townhouse": "RS"}

# ---------------------------------------------------------------- an Excel workbook, by the standard library

NS = {"m": "http://schemas.openxmlformats.org/spreadsheetml/2006/main", "r": "http://schemas.openxmlformats.org/officeDocument/2006/relationships"}


def workbook(path: Path) -> dict[str, list[list]]:
    """Every sheet's rows as lists of cell values (text or number), by the sheet's name."""
    z = zipfile.ZipFile(path)
    shared = []
    if "xl/sharedStrings.xml" in z.namelist():
        for si in ET.fromstring(z.read("xl/sharedStrings.xml")).findall("m:si", NS):
            shared.append("".join(t.text or "" for t in si.iter(f"{{{NS['m']}}}t")))
    rels = {r.get("Id"): r.get("Target") for r in ET.fromstring(z.read("xl/_rels/workbook.xml.rels"))}
    sheets = {}
    for s in ET.fromstring(z.read("xl/workbook.xml")).find("m:sheets", NS):
        target = rels[s.get(f"{{{NS['r']}}}id")].lstrip("/")
        target = target if target.startswith("xl/") else "xl/" + target
        rows = []
        for row in ET.fromstring(z.read(target)).find("m:sheetData", NS):
            out = []
            for c in row:
                # Place the cell by its reference (an unwritten cell leaves a gap, as the header's first column does).
                ref = c.get("r") or ""
                letters = "".join(ch for ch in ref if ch.isalpha())
                col = 0
                for ch in letters:
                    col = col * 26 + (ord(ch.upper()) - 64)
                while letters and len(out) < col - 1:
                    out.append(None)
                kind, v = c.get("t"), c.find("m:v", NS)
                if kind == "inlineStr":
                    val = "".join(t.text or "" for t in c.iter(f"{{{NS['m']}}}t"))
                elif v is None:
                    val = None
                elif kind == "s":
                    val = shared[int(v.text)]
                elif kind in ("str", "b"):
                    val = v.text
                else:
                    val = float(v.text)
                out.append(val)
            rows.append(out)
        sheets[s.get("name")] = rows
    return sheets


def write(name: str, columns: list[str], rows: list[dict]) -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    with (OUT / name).open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=columns)
        w.writeheader()
        for r in rows:
            w.writerow({k: ("" if r.get(k) is None else r[k]) for k in columns})
    return len(rows)


# ---------------------------------------------------------------- RASMI


def rasmi() -> tuple[int, int]:
    sheets = workbook(CACHE / "rasmi-material-intensities" / "all" / "MI_ranges_20230905.xlsx")
    ranges: dict[tuple, dict] = defaultdict(dict)
    long_rows = []
    for material in MATERIALS:
        rows = sheets[material]
        head = [str(h) if h is not None else "" for h in rows[0]]
        col = {h: i for i, h in enumerate(head)}
        for r in rows[1:]:
            if len(r) < len(head) or r[col["function"]] is None:
                continue
            key = (r[col["R5_32"]], r[col["function"]], r[col["structure"]])
            p = {q: float(r[col[q]]) for q in ("p_0", "p_5", "p_25", "p_50", "p_75", "p_95", "p_100")}
            ranges[key][material] = p
            long_rows.append({"region": key[0], "function": key[1], "structure": key[2], "material": material,
                              **{q.replace("_", ""): round(v, 6) for q, v in p.items()}})
    scheme_rows = []
    for (region, function, structure), by in sorted(ranges.items()):
        if set(by) != set(MATERIALS):
            raise ValueError(f"RASMI: {region}/{function}/{structure} lacks a material")
        scheme_rows.append({"region": region, "use": function, "structure": structure,
                            "lower": [round(by[m]["p_5"], 6) for m in MATERIALS],
                            "central": [round(by[m]["p_50"], 6) for m in MATERIALS],
                            "upper": [round(by[m]["p_95"], 6) for m in MATERIALS],
                            "source": "RASMI 20230905"})
    scheme = {"name": "RASMI 20230905 (Fishman et al. 2024): the 5th, 50th and 95th percentiles of kg per m² of gross floor area",
              "materials": MATERIALS, "functions": FUNCTIONS, "structures": STRUCTURES, "rows": scheme_rows,
              "credit": "RASMI (Fishman et al. 2024, Scientific Data 11: 418), CC BY 4.0",
              "declared": "the bounds are RASMI's 5th and 95th percentiles, the central value its median; RASMI states no period of construction"}
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "rasmi-scheme.json").write_text(json.dumps(scheme, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
    write("rasmi-intensities.csv", ["region", "function", "structure", "material", "p0", "p5", "p25", "p50", "p75", "p95", "p100"], long_rows)
    return len(scheme_rows), len(long_rows)


# ---------------------------------------------------------------- Melbourne's buildings

A, F = 6378137.0, 1 / 298.257223563
E2 = F * (2 - F)


def ecef(lat, lon):
    p, l = math.radians(lat), math.radians(lon)
    n = A / math.sqrt(1 - E2 * math.sin(p) ** 2)
    return n * math.cos(p) * math.cos(l), n * math.cos(p) * math.sin(l), n * (1 - E2) * math.sin(p)


def enu_of(origin):
    """The city model's frame: geodetic (WGS 84) to ECEF, rotated to east-north-up at the origin (the up component dropped)."""
    p0, l0 = math.radians(origin["lat"]), math.radians(origin["lon"])
    x0 = ecef(origin["lat"], origin["lon"])

    def enu(lat, lon):
        x = ecef(lat, lon)
        dx, dy, dz = x[0] - x0[0], x[1] - x0[1], x[2] - x0[2]
        e = -math.sin(l0) * dx + math.cos(l0) * dy
        n = -math.sin(p0) * math.cos(l0) * dx - math.sin(p0) * math.sin(l0) * dy + math.cos(p0) * dz
        return e, n
    return enu


def ring_area(ring):
    s = 0.0
    for (x1, y1), (x2, y2) in zip(ring, ring[1:] + ring[:1]):
        s += x1 * y2 - x2 * y1
    return abs(s) / 2


def inside(ring, x, y):
    hit = False
    for (x1, y1), (x2, y2) in zip(ring, ring[1:] + ring[:1]):
        if (y1 > y) != (y2 > y) and x < x1 + (y - y1) * (x2 - x1) / (y2 - y1):
            hit = not hit
    return hit


def edge_distance(ring, x, y):
    best = math.inf
    for (x1, y1), (x2, y2) in zip(ring, ring[1:] + ring[:1]):
        dx, dy = x2 - x1, y2 - y1
        t = 0.0 if dx == dy == 0 else max(0.0, min(1.0, ((x - x1) * dx + (y - y1) * dy) / (dx * dx + dy * dy)))
        best = min(best, math.hypot(x - x1 - t * dx, y - y1 - t * dy))
    return best


def melbourne() -> dict:
    model = json.loads((PRODUCTS / "city-model-melbourne.json").read_text(encoding="utf-8"))
    enu = enu_of(model["origin"])
    prints = []
    for b in model["buildings"]:
        ring = [tuple(p) for p in b["footprint"]]
        if len(ring) < 3:
            continue
        area = ring_area(ring) - sum(ring_area([tuple(p) for p in h]) for h in b.get("holes", []))
        xs, ys = [p[0] for p in ring], [p[1] for p in ring]
        prints.append({"id": b["id"], "ring": ring, "holes": [[tuple(p) for p in h] for h in b.get("holes", [])], "area": area,
                       "box": (min(xs), min(ys), max(xs), max(ys)), "levels": b.get("levels")})
    cell = 50.0
    grid = defaultdict(list)
    for k, p in enumerate(prints):
        x0, y0, x1, y1 = p["box"]
        for i in range(math.floor(x0 / cell), math.floor(x1 / cell) + 1):
            for j in range(math.floor(y0 / cell), math.floor(y1 / cell) + 1):
                grid[(i, j)].append(k)

    rows = list(csv.DictReader((CACHE / "melbourne-clue-buildings" / "hoddle-grid" / "buildings.csv").open(encoding="utf-8-sig"), delimiter=";"))
    year = max(int(r["census_year"]) for r in rows)
    clue = [r for r in rows if int(r["census_year"]) == year]
    joined, outside = [], 0
    for r in clue:
        try:
            lat, lon = float(r["latitude"]), float(r["longitude"])
        except ValueError:
            outside += 1
            continue
        x, y = enu(lat, lon)
        near = set()
        for i in (math.floor(x / cell) - 1, math.floor(x / cell), math.floor(x / cell) + 1):
            for j in (math.floor(y / cell) - 1, math.floor(y / cell), math.floor(y / cell) + 1):
                near.update(grid.get((i, j), []))
        holding = [k for k in near if inside(prints[k]["ring"], x, y) and not any(inside(h, x, y) for h in prints[k]["holes"])]
        if holding:
            k = min(holding, key=lambda k: prints[k]["area"])
            how = "inside"
        else:
            dist = sorted((edge_distance(prints[k]["ring"], x, y), k) for k in near)
            if not dist or dist[0][0] > 10:
                outside += 1
                continue
            k, how = dist[0][1], "nearest within 10 m"
        joined.append((r, k, how, lat, lon))
    sharing = defaultdict(int)
    for _, k, _, _, _ in joined:
        sharing[k] += 1
    out, no_storeys = [], 0
    for r, k, how, lat, lon in joined:
        p = prints[k]
        storeys = r["number_of_floors_above_ground"].strip()
        storeys = float(storeys) if storeys and float(storeys) > 0 else (float(p["levels"]) if p["levels"] else None)
        share = p["area"] / sharing[k]
        use = r["predominant_space_use"].strip()
        function = "" if use.startswith("Unoccupied") else FUNCTION_OF_USE.get(use, "NR")
        if storeys is None:
            no_storeys += 1
        out.append({"id": r["property_id"], "block": r["block_id"], "name": r["building_name"].strip(), "use": use, "function": function,
                    "structure": "", "storeys": storeys, "footprint_m2": round(share, 1),
                    "floor_area_m2": round(share * storeys, 1) if storeys else None,
                    "year": int(r["construction_year"]) if r["construction_year"].strip() else None,
                    "census_year": year, "footprint": p["id"], "join": how, "latitude": lat, "longitude": lon, "region": "OECD_AUNZ"})
    n = write("melbourne-buildings.csv", ["id", "block", "name", "use", "function", "structure", "storeys", "footprint_m2", "floor_area_m2", "year",
                                          "census_year", "footprint", "join", "latitude", "longitude", "region"], out)
    return {"buildings": n, "census_year": year, "left_out": outside, "without_storeys": no_storeys,
            "joined_inside": sum(1 for x in out if x["join"] == "inside")}


# ---------------------------------------------------------------- the EU27's input–output table and its air emissions

INDUSTRIES = ["A01", "A02", "A03", "B", "C10-12", "C13-15", "C16", "C17", "C18", "C19", "C20", "C21", "C22", "C23", "C24", "C25", "C26", "C27",
              "C28", "C29", "C30", "C31_32", "C33", "D", "E36", "E37-39", "F", "G45", "G46", "G47", "H49", "H50", "H51", "H52", "H53", "I", "J58",
              "J59_60", "J61", "J62_63", "K64", "K65", "K66", "L68", "M69_70", "M71", "M72", "M73", "M74_75", "N77", "N78", "N79", "N80-82", "O", "P",
              "Q86", "Q87_88", "R90-92", "R93", "S94", "S95", "S96", "T", "U"]
FINAL = {"P3_S14": "households", "P3_S13": "government", "P3_S15": "non-profit institutions", "P51G": "gross fixed capital formation",
         "P52": "inventories", "P53": "valuables", "P6": "exports"}
POLLUTANTS = {"GHG": "GHG (t CO2-eq)", "CO2": "CO2 (t)", "CH4_CO2E": "CH4 (t CO2-eq)", "N2O_CO2E": "N2O (t CO2-eq)", "ACG": "acidifying (t SO2-eq)", "PM2_5": "PM2.5 (t)"}


def io_code(code: str) -> str:
    """The input–output tables' code of an industry: L68A and L68B are one, L68."""
    return "L68" if code in ("L68A", "L68B") else code


def air_code(code: str) -> str:
    """The air emissions accounts write both ends of a range (C10-C12, J59_J60); the input–output tables write the second without its letter.
    Real estate is the accounts' section L, which holds division 68 alone; their L68A, a part of it, is not taken, lest it be counted twice."""
    if code == "L":
        return "L68"
    if code == "L68A":
        return "(part of L)"
    return re.sub(r"([-_])[A-Z](\d)", r"\1\2", code)


def eu27() -> dict:
    io_path = CACHE / "eurostat-io-tables" / "eu27-2022" / "io-table.json"
    air_path = CACHE / "eurostat-air-emissions" / "eu27-2022" / "air-emissions.json"
    labels = {}
    Z = defaultdict(float)
    for d, v, lab in jsonstat(io_path):
        if d["stk_flow"] != "DOM":
            continue
        labels.update({io_code(k): s for k, s in lab["ind_ava"].items()})
        row, col = io_code(d["ind_ava"]), d["ind_use"]
        col = io_code(col) if col not in FINAL else col
        if row in INDUSTRIES and (col in INDUSTRIES or col in FINAL):
            Z[(row, col)] += float(v)
    labels["L68"] = "Real estate activities"
    io_rows = []
    for i in INDUSTRIES:
        row = {"industry": i, "label": labels.get(i, i)}
        total = 0.0
        for j in INDUSTRIES:
            row[j] = round(Z.get((i, j), 0.0), 3)
            total += row[j]
        for c, name in FINAL.items():
            row[name] = round(Z.get((i, c), 0.0), 3)
            total += row[name]
        row["output"] = round(total, 3)
        io_rows.append(row)
    write("eu27-io-2022.csv", ["industry", "label"] + INDUSTRIES + list(FINAL.values()) + ["output"], io_rows)
    air = defaultdict(float)
    for d, v, lab in jsonstat(air_path):
        code = air_code(d["nace_r2"])
        if code in INDUSTRIES or code == "HH":
            air[(code, d["airpol"])] += float(v)
    air_rows = []
    for i in INDUSTRIES + ["HH"]:
        row = {"industry": "households" if i == "HH" else i, "label": "households, directly (heating, transport, other)" if i == "HH" else labels.get(i, i)}
        for p, name in POLLUTANTS.items():
            row[name] = round(air.get((i, p), 0.0), 3) if (i, p) in air else None
        air_rows.append(row)
    write("eu27-air-2022.csv", ["industry", "label"] + list(POLLUTANTS.values()), air_rows)
    missing = [i for i in INDUSTRIES if (i, "GHG") not in air]
    return {"industries": len(io_rows), "output_meur": round(sum(r["output"] for r in io_rows)), "air_rows": len(air_rows),
            "industries_without_emissions": missing}


def main() -> int:
    made = {}
    rows, cells = rasmi()
    made["rasmi-scheme.json"], made["rasmi-intensities.csv"] = rows, cells
    mel = melbourne()
    made["melbourne-buildings.csv"] = mel["buildings"]
    eu = eu27()
    made["eu27-io-2022.csv"], made["eu27-air-2022.csv"] = eu["industries"], eu["air_rows"]
    register("materials-and-flows", {"name": "materials", "title": "Materials and flows: RASMI's intensities, Melbourne's buildings, the EU27's input–output table and air emissions",
                                     "files": sorted(made), "rows": made, "made": dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds"),
                                     "melbourne": mel, "eu27": eu,
                                     "credit": "RASMI (Fishman et al. 2024), CC BY 4.0; CLUE, City of Melbourne, CC BY 4.0; © OpenStreetMap contributors, ODbL 1.0; Eurostat (naio_10_cp1750, env_ac_ainah_r2)"})
    for k, n in made.items():
        print(f"{k}: {n} rows")
    print("Melbourne:", mel)
    print("EU27:", eu)
    return 0


if __name__ == "__main__":
    sys.exit(main())
