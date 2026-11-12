"""Urban forests as tables, for the ecology domain and the Urban Studio (ADR 0101, ADR 0102).

A city is a case of CASES. Each case writes, under data/cache/products/urban-forest/:

    <city>-trees.csv     one row a tree, in one form whatever the city recorded:
                         id, lat, lon (WGS 84, degrees), species, genus, family (as recorded; a family the record lacks is
                         completed from the GBIF backbone by the genus, and said to be in `family_source`), common, dbh_cm
                         (diameter at breast height, cm; empty where not measured), year_planted and age (empty where unknown
                         or a placeholder), life_lower and life_upper (the remaining useful life an arborist stated, years;
                         life_upper empty for an open class such as "> 41 years"), area (the city's own division: Melbourne's
                         precinct, New York's neighbourhood tabulation area), setting (street or park), status.
    <city>-areas.csv     where a measured canopy is held (Melbourne): one row a census block, its centroid, area, canopy area
                         and cover, its dwellings in the latest census year, and the managed trees standing in it.
    <city>-canopy.csv    the measured canopy on a grid of 25 m cells (lat, lon of each cell's centre, cover), for maps.

What is presumed is said to be: Melbourne records "1900" as the planting year of trees whose year is unknown (26,521 of them in
the record of 2025-09-22), which is read as unknown; its life classes are read as intervals ("21 - 30 years" as [20, 30],
"< 10 years" as [0, 10], "> 41 years" as more than 40, open); it leaves the diameter unmeasured for about half its trees, mostly
in parks. New York measures diameters in inches (converted, × 2.54) and records no genus or family; the genus is the first word
of the Latin name, the family the GBIF backbone's for that genus (kept in data/cache/gbif-names/). Stumps and dead trees are kept,
with their status, for whoever wants them.

Sources: City of Melbourne's Urban Forest and Tree Canopies 2021 (CC BY 4.0), its census of land use and employment's blocks and
dwellings (CC BY 4.0); New York City's 2015 Street Tree Census (NYC Open Data); the GBIF Backbone Taxonomy (CC BY 4.0), through
the data library's datasets melbourne-urban-forest, melbourne-tree-canopies, melbourne-clue-blocks, nyc-street-trees-2015 and the
table melbourne-blocks (city_tables.py).

    python data/products/urban_forest.py [melbourne] [nyc]
"""

from __future__ import annotations

import csv
import datetime as dt
import json
import math
import re
import sys
import time
import urllib.parse
import urllib.request
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

from product_index import OUT as PRODUCTS, register  # noqa: E402

CACHE = HERE.parents[0] / "cache"
OUT = PRODUCTS / "urban-forest"
NAMES = CACHE / "gbif-names"
COLUMNS = ["id", "lat", "lon", "species", "genus", "family", "family_source", "common", "dbh_cm", "year_planted", "age",
           "life_lower", "life_upper", "area", "setting", "status"]
YEAR = dt.date.today().year


def _num(v):
    try:
        x = float(v)
        return x if math.isfinite(x) else None
    except (TypeError, ValueError):
        return None


def _write(path: Path, columns: list[str], rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(".tmp")
    with tmp.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=columns, extrasaction="ignore")
        w.writeheader()
        for r in rows:
            w.writerow({k: ("" if r.get(k) is None else r[k]) for k in columns})
    tmp.replace(path)


# ---- taxonomy from the GBIF backbone ---------------------------------------------------------------------------------

def gbif_family(genus: str) -> str | None:
    """The family of a plant genus in the GBIF Backbone Taxonomy, kept once asked."""
    if not genus:
        return None
    NAMES.mkdir(parents=True, exist_ok=True)
    keep = NAMES / f"{re.sub(r'[^A-Za-z]', '_', genus)}.json"
    if keep.exists():
        return json.loads(keep.read_text(encoding="utf-8")).get("family")
    url = "https://api.gbif.org/v1/species/match?" + urllib.parse.urlencode({"name": genus, "rank": "GENUS", "kingdom": "Plantae", "strict": "false"})
    with urllib.request.urlopen(url, timeout=30) as r:
        answer = json.load(r)
    keep.write_text(json.dumps({"asked": genus, "url": url, "retrieved": dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds"),
                                "family": answer.get("family"), "match": answer.get("matchType"), "rank": answer.get("rank")}, indent=1), encoding="utf-8")
    time.sleep(0.2)
    return answer.get("family")


# ---- Melbourne -------------------------------------------------------------------------------------------------------

LIFE = {"< 10 years": (0, 10), "11 - 20 years": (10, 20), "21 - 30 years": (20, 30), "31 - 40 years": (30, 40), "> 41 years": (40, None)}


def melbourne_trees() -> list[dict]:
    src = CACHE / "melbourne-urban-forest" / "all" / "trees.json"
    rows = []
    for r in json.loads(src.read_text(encoding="utf-8")):
        lat, lon = _num(r.get("latitude")), _num(r.get("longitude"))
        if lat is None or lon is None:
            continue
        year = _num(r.get("year_planted"))
        year = None if year is None or year <= 1900 or year > YEAR else int(year)
        life = LIFE.get(r.get("useful_life_expectency") or "", (None, None))
        dbh = _num(r.get("diameter_breast_height"))
        name = (r.get("scientific_name") or "").strip()
        rows.append({
            "id": r.get("com_id"), "lat": round(lat, 7), "lon": round(lon, 7),
            "species": name if len(name.split()) >= 2 and not name.endswith((" sp.", " spp.", " spp")) else "",
            "genus": (r.get("genus") or "").strip(), "family": (r.get("family") or "").strip(), "family_source": "record",
            "common": r.get("common_name") or "", "dbh_cm": dbh if dbh and dbh > 0 else None,
            "year_planted": year, "age": (YEAR - year) if year else None,
            "life_lower": life[0], "life_upper": life[1],
            "area": r.get("precinct") or "", "setting": (r.get("located_in") or "").lower(), "status": "alive",
        })
    return rows


def _rings(geometry: dict) -> list[list[list[float]]]:
    if not geometry:
        return []
    if geometry["type"] == "Polygon":
        return [geometry["coordinates"][0]] + geometry["coordinates"][1:]
    if geometry["type"] == "MultiPolygon":
        return [ring for poly in geometry["coordinates"] for ring in poly]
    return []


def melbourne_canopy(trees: list[dict]) -> tuple[list[dict], list[dict]]:
    """The measured canopy, rasterised at 2 m in a local frame, by census block and on a 25 m grid."""
    from PIL import Image, ImageDraw
    blocks = json.loads((CACHE / "melbourne-clue-blocks" / "all" / "blocks.geojson").read_text(encoding="utf-8"))["features"]
    lon0, lat0 = 144.95, -37.81
    kx, ky = 111320.0 * math.cos(math.radians(lat0)), 110574.0   # metres per degree near the city (an equirectangular frame)
    pts = [(lon, lat) for f in blocks for ring in _rings(f["geometry"])[:1] for lon, lat in ring]
    xmin = min((lon - lon0) * kx for lon, _ in pts) - 200
    xmax = max((lon - lon0) * kx for lon, _ in pts) + 200
    ymin = min((lat - lat0) * ky for _, lat in pts) - 200
    ymax = max((lat - lat0) * ky for _, lat in pts) + 200
    res = 2.0
    W, H = int((xmax - xmin) / res) + 1, int((ymax - ymin) / res) + 1

    def px(lon, lat):
        return ((lon - lon0) * kx - xmin) / res, (ymax - (lat - lat0) * ky) / res

    canopy = Image.new("1", (W, H), 0)
    draw = ImageDraw.Draw(canopy)
    with (CACHE / "melbourne-tree-canopies" / "all" / "canopies.geojson").open(encoding="utf-8") as f:
        feats = json.load(f)["features"]
    for feat in feats:
        rings = _rings(feat.get("geometry"))
        for k, ring in enumerate(rings):
            if len(ring) >= 3:
                draw.polygon([px(lon, lat) for lon, lat in ring], fill=0 if k else 1)
    label = Image.new("I", (W, H), 0)
    ldraw = ImageDraw.Draw(label)
    ids = []
    for i, feat in enumerate(blocks, start=1):
        rings = _rings(feat["geometry"])
        if not rings:
            continue
        ldraw.polygon([px(lon, lat) for lon, lat in rings[0]], fill=i)
        ids.append((i, feat))
    import numpy as np
    c = np.asarray(canopy, dtype=np.uint8)
    lab = np.asarray(label, dtype=np.int64)
    area = np.bincount(lab.ravel(), minlength=len(blocks) + 1) * res * res
    cov = np.bincount(lab.ravel(), weights=c.ravel().astype(float), minlength=len(blocks) + 1) * res * res
    # Dwellings: the latest census year of the blocks' table.
    dwell = {}
    table = json.loads((PRODUCTS / "tables" / "melbourne-blocks.json").read_text(encoding="utf-8"))
    names = [col["name"] for col in table["columns"]]
    data = table["data"]
    col = {n: data[n] if isinstance(data, dict) else [row[k] for row in data] for k, n in enumerate(names)} if isinstance(data, (dict, list)) else {}
    years = col.get("census_year", [])
    latest = max(y for y in years if y is not None) if years else None
    dw_name = next((n for n in names if n.startswith("dwellings")), None)
    for b, y, d in zip(col.get("block_id", []), years, col.get(dw_name, []) if dw_name else []):
        if y == latest:
            dwell[str(b)] = d
    # Managed trees in each block, by the label under each tree.
    count = np.zeros(len(blocks) + 1)
    for t in trees:
        x, y = px(t["lon"], t["lat"])
        if 0 <= int(x) < W and 0 <= int(y) < H:
            count[lab[int(y), int(x)]] += 1
    areas = []
    for i, feat in ids:
        p = feat["properties"]
        ring = _rings(feat["geometry"])[0]
        cx = sum(q[0] for q in ring) / len(ring)
        cy = sum(q[1] for q in ring) / len(ring)
        a = float(area[i])
        areas.append({"block_id": str(p.get("block_id")), "small_area": p.get("area_name") or p.get("clue_area") or "", "lat": round(cy, 6), "lon": round(cx, 6),
                      "area_m2": round(a), "canopy_m2": round(float(cov[i])), "cover": round(float(cov[i]) / a, 4) if a > 0 else None,
                      "dwellings": dwell.get(str(p.get("block_id"))), "trees": int(count[i])})
    # The 25 m grid.
    g = int(25 / res)
    h2, w2 = (H // g) * g, (W // g) * g
    cg = c[:h2, :w2].reshape(h2 // g, g, w2 // g, g).mean(axis=(1, 3))
    grid = []
    for r in range(cg.shape[0]):
        for q in range(cg.shape[1]):
            if cg[r, q] <= 0:
                continue
            x = xmin + (q + 0.5) * 25
            y = ymax - (r + 0.5) * 25
            grid.append({"lat": round(lat0 + y / ky, 6), "lon": round(lon0 + x / kx, 6), "cover": round(float(cg[r, q]), 3)})
    return areas, grid


# ---- New York --------------------------------------------------------------------------------------------------------

def nyc_trees() -> list[dict]:
    src = CACHE / "nyc-street-trees-2015" / "all" / "trees.csv"
    rows = []
    with src.open(encoding="utf-8") as f:
        for r in csv.DictReader(f):
            lat, lon = _num(r.get("latitude")), _num(r.get("longitude"))
            if lat is None or lon is None:
                continue
            name = (r.get("spc_latin") or "").strip()
            genus = name.split()[0] if name else ""
            dbh = _num(r.get("tree_dbh"))
            rows.append({
                "id": r.get("tree_id"), "lat": lat, "lon": lon,
                "species": " ".join(name.split()[:3]) if " x " in name else " ".join(name.split()[:2]) if len(name.split()) >= 2 else "",
                "genus": genus, "family": "", "family_source": "", "common": r.get("spc_common") or "",
                "dbh_cm": round(dbh * 2.54, 1) if dbh and dbh > 0 else None, "year_planted": None, "age": None,
                "life_lower": None, "life_upper": None, "area": r.get("nta_name") or r.get("nta") or "", "setting": "street",
                "status": (r.get("status") or "").lower(),
            })
    genera = sorted({r["genus"] for r in rows if r["genus"]})
    families = {g: gbif_family(g) for g in genera}
    for r in rows:
        fam = families.get(r["genus"])
        if fam:
            r["family"], r["family_source"] = fam, "GBIF backbone, by genus"
    return rows


# ---- the cases ------------------------------------------------------------------------------------------------------

def make_melbourne() -> dict:
    trees = melbourne_trees()
    _write(OUT / "melbourne-trees.csv", COLUMNS, trees)
    areas, grid = melbourne_canopy(trees)
    _write(OUT / "melbourne-areas.csv", ["block_id", "small_area", "lat", "lon", "area_m2", "canopy_m2", "cover", "dwellings", "trees"], areas)
    _write(OUT / "melbourne-canopy.csv", ["lat", "lon", "cover"], grid)
    total, canopy = sum(a["area_m2"] for a in areas), sum(a["canopy_m2"] for a in areas)
    return {"name": "melbourne", "title": "Melbourne's urban forest", "trees": len(trees), "measured": sum(1 for t in trees if t["dbh_cm"]),
            "blocks": len(areas), "canopy_cover": round(canopy / total, 4) if total else None,
            "files": ["melbourne-trees.csv", "melbourne-areas.csv", "melbourne-canopy.csv"],
            "credit": "City of Melbourne: Urban Forest, Tree Canopies 2021 and CLUE (CC BY 4.0)"}


def make_nyc() -> dict:
    trees = nyc_trees()
    _write(OUT / "nyc-trees.csv", COLUMNS, trees)
    return {"name": "nyc", "title": "New York City's street trees, 2015", "trees": len(trees), "measured": sum(1 for t in trees if t["dbh_cm"]),
            "files": ["nyc-trees.csv"], "credit": "NYC Parks: 2015 Street Tree Census (NYC Open Data); families: GBIF Backbone Taxonomy (CC BY 4.0)"}


CASES = {"melbourne": make_melbourne, "nyc": make_nyc}


def main(argv: list[str]) -> int:
    wanted = argv or list(CASES)
    for name in wanted:
        t0 = time.time()
        entry = CASES[name]()
        entry["made"] = dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds")
        register("urban-forests", entry)
        print(f"{name}: {entry['trees']} trees ({entry['measured']} measured) in {time.time() - t0:.0f} s")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
