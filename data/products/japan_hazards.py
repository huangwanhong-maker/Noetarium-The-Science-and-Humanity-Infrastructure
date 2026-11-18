"""A Japanese municipality's hazards, made ready to draw and to evacuate from: the assumed depth of a tsunami and of the largest
assumed river flood, from the Hazard Map Portal's tiles (国土交通省・国土地理院, ハザードマップポータルサイト), and the designated
emergency evacuation sites with the hazards each is designated for (国土地理院, 指定緊急避難場所データ).

The tiles are paletted PNGs of depth classes, each class an exact colour (the portal's legend, shinsui_legend3.png). A class is
taken at its middle (the first at 0.15 m; those beyond 10 m at the byte's top, 12.75 m), and the map is carried from Web Mercator onto
a grid of longitude and latitude by the nearest pixel, a byte a cell of 0.05 m, rows from the south: the form of the data library's
other depth maps, which urban science's exposure grids read. A tile the portal does not hold (404) is land outside the hazard.

    python data/products/japan_hazards.py                 # Miyako, Iwate (03202), its bay-side centre
    python data/products/japan_hazards.py --code 03202 --box 39.615 141.925 39.665 141.985

writes data/cache/products/japan-hazards/<code>.json, with its maps under data/cache/products/japan-hazards/<code>/:
    tsunami.bin    the tsunami's assumed depth (津波浸水想定), one byte a cell
    flood.bin      the largest assumed river flood (洪水浸水想定区域, 想定最大規模), one byte a cell

Terms. The portal's tiles: commercial and non-commercial use allowed, credited 「ハザードマップポータルサイト」を加工して作成. The
evacuation sites: the GSI's terms (compatible with CC BY 4.0), credited 出典：国土地理院ウェブサイト. Neither is committed; both are
fetched into the cache.
"""

from __future__ import annotations

import argparse
import csv
import io
import json
import math
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path

import numpy as np
from PIL import Image

HERE = Path(__file__).resolve().parent
CACHE = HERE.parent / "cache"
TILES = CACHE / "japan-hazards" / "tiles"
OUT = CACHE / "products" / "japan-hazards"
PORTAL = "https://disaportaldata.gsi.go.jp/raster/{layer}/{z}/{x}/{y}.png"
SITES = "https://hinanmap.gsi.go.jp/hinanjocp/defaultFtpData/csv/{code}_2.csv"
LAYERS = {"tsunami": "04_tsunami_newlegend_data", "flood": "01_flood_l2_shinsuishin_data"}
UNIT = 0.05  # metres a byte
# The legend's colours and the depth taken for each class (its middle; beyond 10 m the byte's top).
CLASSES = [
    ((255, 255, 179), "under 0.3 m", 0.15),
    ((247, 245, 169), "0.3 to 0.5 m", 0.40),
    ((248, 225, 166), "0.5 to 1 m", 0.75),
    ((255, 216, 192), "1 to 3 m", 2.0),
    ((255, 183, 183), "3 to 5 m", 4.0),
    ((255, 145, 145), "5 to 10 m", 7.5),
    ((242, 133, 201), "10 to 20 m", 12.75),
    ((220, 122, 220), "20 m or more", 12.75),
]
HAZARDS = ["洪水", "崖崩れ、土石流及び地滑り", "高潮", "地震", "津波", "大規模な火事", "内水氾濫", "火山現象"]
HAZARD_EN = {"洪水": "flood", "崖崩れ、土石流及び地滑り": "landslide", "高潮": "storm surge", "地震": "earthquake", "津波": "tsunami",
             "大規模な火事": "large fire", "内水氾濫": "inland flood", "火山現象": "volcano"}
AGENT = "Noetarium data library (research; https://github.com/huangwanhong-maker/Noetarium-The-Science-and-Humanity-Infrastructure)"


def fetch(url: str) -> bytes | None:
    req = urllib.request.Request(url, headers={"User-Agent": AGENT})
    for attempt in range(3):
        try:
            with urllib.request.urlopen(req, timeout=60) as r:
                return r.read()
        except urllib.error.HTTPError as e:
            if e.code == 404:
                return None
            if attempt == 2:
                raise
        except (urllib.error.URLError, TimeoutError):
            if attempt == 2:
                raise
        time.sleep(1.5 * (attempt + 1))
    return None


def tile_of(lat: float, lon: float, z: int) -> tuple[float, float]:
    """Web Mercator tile coordinates (fractional) of a point."""
    n = 2 ** z
    x = (lon + 180) / 360 * n
    y = (1 - math.asinh(math.tan(math.radians(lat))) / math.pi) / 2 * n
    return x, y


def tile(layer: str, z: int, x: int, y: int) -> np.ndarray | None:
    """A tile as depths in bytes (256 × 256), kept in the cache; None where the portal holds none."""
    path = TILES / layer / str(z) / str(x) / f"{y}.png"
    absent = path.with_suffix(".none")
    if absent.exists():
        return None
    if not path.exists():
        data = fetch(PORTAL.format(layer=layer, z=z, x=x, y=y))
        path.parent.mkdir(parents=True, exist_ok=True)
        if data is None:
            absent.write_text("")
            return None
        path.write_bytes(data)
    img = Image.open(io.BytesIO(path.read_bytes())).convert("RGBA")
    a = np.asarray(img)
    out = np.zeros(a.shape[:2], dtype=np.uint8)
    rgb, alpha = a[..., :3].astype(np.int32), a[..., 3]
    for colour, _, depth in CLASSES:
        hit = (alpha > 0) & (np.abs(rgb - np.array(colour)).sum(axis=2) <= 6)
        out[hit] = min(255, int(round(depth / UNIT)))
    return out


def depth_map(layer: str, box: tuple[float, float, float, float], z: int, cell_m: float) -> tuple[np.ndarray, int, int, dict]:
    """The layer's depth on a grid of longitude and latitude over the box, rows from the south, by the nearest Mercator pixel."""
    s, w, n, e = box
    lat0 = (s + n) / 2
    width = int(math.ceil((e - w) * 111320 * math.cos(math.radians(lat0)) / cell_m))
    height = int(math.ceil((n - s) * 110574 / cell_m))
    lats = s + (np.arange(height) + 0.5) * (n - s) / height
    lons = w + (np.arange(width) + 0.5) * (e - w) / width
    tx = (lons + 180) / 360 * 2 ** z
    ty = (1 - np.arcsinh(np.tan(np.radians(lats))) / math.pi) / 2 * 2 ** z
    grid = np.zeros((height, width), dtype=np.uint8)
    counts = {"tiles": 0, "absent": 0}
    for X in range(int(math.floor(tx.min())), int(math.floor(tx.max())) + 1):
        for Y in range(int(math.floor(ty.min())), int(math.floor(ty.max())) + 1):
            t = tile(layer, z, X, Y)
            counts["tiles"] += 1
            if t is None:
                counts["absent"] += 1
                continue
            cols = np.where(np.floor(tx) == X)[0]
            rows = np.where(np.floor(ty) == Y)[0]
            if not len(cols) or not len(rows):
                continue
            px = np.clip(((tx[cols] - X) * 256).astype(int), 0, 255)
            py = np.clip(((ty[rows] - Y) * 256).astype(int), 0, 255)
            grid[np.ix_(rows, cols)] = t[np.ix_(py, px)]
    return grid, width, height, counts


def sites(code: str) -> list[dict]:
    """The designated emergency evacuation sites of a municipality, with the hazards each is designated for."""
    keep = CACHE / "japan-hazards" / f"sites-{code}.csv"
    if not keep.exists():
        data = fetch(SITES.format(code=code))
        if data is None:
            raise SystemExit(f"The GSI holds no evacuation sites for {code}")
        keep.parent.mkdir(parents=True, exist_ok=True)
        keep.write_bytes(data)
    text = keep.read_bytes().decode("utf-8-sig")
    rows = list(csv.reader(io.StringIO(text)))
    head = rows[0]
    col = {name: i for i, name in enumerate(head)}

    def find(*names: str) -> int | None:
        for nm in names:
            for k, i in col.items():
                if nm in k:
                    return i
        return None

    i_name, i_addr, i_lat, i_lon = find("施設・場所名", "名称"), find("住所"), find("緯度"), find("経度")
    out = []
    for r in rows[1:]:
        if not r or i_lat is None or i_lon is None or not r[i_lat]:
            continue
        hz = [HAZARD_EN[h] for h in HAZARDS if (j := find(h)) is not None and j < len(r) and r[j].strip() in ("1", "○", "◯")]
        out.append({"name": r[i_name] if i_name is not None else "", "address": r[i_addr] if i_addr is not None else "",
                    "lat": float(r[i_lat]), "lon": float(r[i_lon]), "hazards": hz})
    return out


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--code", default="03202")
    ap.add_argument("--name", default="Miyako")
    ap.add_argument("--box", type=float, nargs=4, default=[39.615, 141.925, 39.665, 141.985], metavar=("S", "W", "N", "E"))
    ap.add_argument("--zoom", type=int, default=15)
    ap.add_argument("--cell", type=float, default=4.0, help="metres a cell")
    a = ap.parse_args()
    box = tuple(a.box)
    (OUT / a.code).mkdir(parents=True, exist_ok=True)
    maps = []
    for peril, layer in LAYERS.items():
        grid, width, height, counts = depth_map(layer, box, a.zoom, a.cell)
        (OUT / a.code / f"{peril}.bin").write_bytes(grid.tobytes())
        wet = int((grid > 0).sum())
        maps.append({"peril": peril, "layer": layer, "file": f"japan-hazards/{a.code}/{peril}.bin", "box": list(box), "width": width, "height": height,
                     "unit": UNIT, "wet_cells": wet, "wet_share": round(wet / grid.size, 4), "tiles": counts})
        print(f"{peril}: {width} × {height} cells, {wet} wet ({100 * wet / grid.size:.1f}%), {counts['tiles']} tiles, {counts['absent']} absent")
    S = [x for x in sites(a.code) if box[0] <= x["lat"] <= box[2] and box[1] <= x["lon"] <= box[3]]
    print(f"{len(S)} designated emergency evacuation sites within the box ({sum('tsunami' in x['hazards'] for x in S)} for tsunami)")
    record = {
        "schema": "noetarium.japan-hazards/1", "code": a.code, "name": a.name, "box": list(box), "zoom": a.zoom, "cell_m": a.cell,
        "classes": [{"colour": "#%02X%02X%02X" % c, "class": label, "taken_m": d} for c, label, d in CLASSES],
        "maps": maps, "sites": S,
        "credit": "「ハザードマップポータルサイト」を加工して作成 (MLIT and GSI, Hazard Map Portal; tsunami: 津波浸水想定, flood: 洪水浸水想定区域 想定最大規模). "
                  "Evacuation sites: 出典：国土地理院ウェブサイト (指定緊急避難場所データ). Depth classes taken at their middles; a site's capacity is not published.",
        "retrieved": time.strftime("%Y-%m-%d"),
    }
    (OUT / f"{a.code}.json").write_text(json.dumps(record, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
    print(f"wrote {OUT / (a.code + '.json')}")


if __name__ == "__main__":
    main()
