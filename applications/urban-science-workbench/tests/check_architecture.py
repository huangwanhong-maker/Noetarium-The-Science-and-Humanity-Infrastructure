"""Checks of the plug-in Architecture (ADR 0089), run by tests/run-tests.mjs, on a grid of streets 100 m by 60 m whose block about
(50, 30) holds four buildings known by hand, with buildings in the blocks about it. The engine answers on the API's composition
(api/cpp/science/design.hpp): the block found is the grid's cell; each lot's street edges are the ones a ray reaches the street by;
the block as built holds Σ area × storeys; every massing on the front lies within the rules; every design keeps the floor and street
wall asked and the plot ratio allowed; the search is the same when asked again; the hours of sun lie between none and the day's. One
JSON object a line.

    python applications/urban-science-workbench/tests/check_architecture.py
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
APP = HERE.parent
sys.path.insert(0, str(APP))
import engine  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def check(name: str, value: float, limit: float) -> None:
    emit({"kind": "check", "name": name, "value": float(value), "limit": float(limit), "unit": ""})


def assert_(name: str, condition: bool, detail: str = "") -> None:
    emit({"kind": "assert", "name": name, "ok": bool(condition), "detail": str(detail)})


def skip(what: str) -> None:
    emit({"kind": "skip", "name": what})


if not engine.status("architecture")["current"]:
    skip("the engine of Architecture is not built from the present sources (python applications/urban-science-workbench/engine.py build architecture)")
    sys.exit(0)
worker = engine.Worker("architecture")


def ask(op: str, request: dict) -> dict:
    a = worker.ask({**request, "op": op})
    if not a.get("ok"):
        raise RuntimeError(a.get("error"))
    return a["result"]


def box(x0, y0, x1, y1):
    return [x0, y0, x1, y0, x1, y1, x0, y1]


# Streets split where they meet, as the world's tiles hold them; four buildings in the block about (50, 30), one in each other block.
streets = [[x, y, x, y + 60] for x in range(-200, 401, 100) for y in range(-180, 240, 60)]
streets += [[x, y, x + 100, y] for y in range(-180, 241, 60) for x in range(-200, 400, 100)]
LOTS = [(12, (8, 8, 48, 52)), (20, (52, 8, 92, 30)), (9, (52, 30, 72, 52)), (15, (72, 30, 92, 52))]
buildings = [[h] + box(*b) for h, b in LOTS]
buildings += [[18 + (bx + by) % 30] + box(bx + 8, by + 8, bx + 92, by + 52) for bx in range(-200, 400, 100) for by in range(-180, 240, 60) if (bx, by) != (0, 0)]
deg = math.pi / 180
suns = [[a * deg, (z % 360) * deg, 1, t] for a, z, t in [(14, 315, 8), (22, 335, 9.5), (27, 0, 11), (22, 25, 11.5), (14, 45, 11)]]
RULES = {"max_height": 60, "plot_ratio": 6, "street_setback": 0, "side_setback": 0, "street_wall": 20, "upper_setback": 6, "storey": 3.2, "min_frontage": 0.5}
request = {"at": [50, 30], "streets": streets, "ways": streets, "buildings": buildings, "rules": RULES, "suns": suns,
           "night": {"air": 6, "hours": 19}, "generations": 20, "seed": 3}

try:
    a = ask("design", request)
except RuntimeError as error:
    assert_("architecture: the engine designs the grid's block", False, str(error))
    sys.exit(0)

block = a["block"]
xs, ys = block[0::2], block[1::2]
assert_("the block about (50, 30) is the grid's cell, 100 m by 60 m", (min(xs), min(ys), max(xs), max(ys)) == (0, 0, 100, 60) and len(xs) == 4, str(block))
streets_of = [l["street"] for l in a["lots"]]
# Lot 0 faces the streets west, south and north, lot 1 south and east, lot 2 north, lot 3 east and north (edges from the south-west corner,
# counter-clockwise: south, east, north, west).
assert_("each lot's street edges are those a ray reaches the street by before another lot",
        streets_of == [[1, 0, 1, 1], [1, 1, 0, 0], [0, 0, 1, 0], [0, 1, 1, 0]], str(streets_of))
floor = sum((b[2] - b[0]) * (b[3] - b[1]) * max(1, round(h / 3.2)) for h, b in LOTS)
check("the block as built holds Σ area × storeys (15,840 m²)", a["asBuilt"]["floor"] - floor, 0.5)
check("... on 3,520 m² of lots", a["lotArea"] - 3520, 0.5)
assert_("two passages through the block: across its middle and along it",
        len(a["passages"]) == 2 and [round(v) for v in a["passages"][0]] in ([50, 0, 50, 60], [50, 60, 50, 0]), str(a["passages"]))
assert_("every massing on the front lies within the rules", a["complying"] == a["pieces"] and a["pieces"] > 0 and not a["failing"], f"{a['complying']} of {a['pieces']}, {a['failing']}")
D = a["designs"]
assert_("a front of designs", len(D) >= 10, str(len(D)))
least = min(d["measures"]["floor"] for d in D) if D else 0
assert_("every design keeps half the floor standing now", least >= 0.5 * floor - 1, f"least {least}")
assert_("every design keeps the plot ratio of 6", all(d["measures"]["floor"] <= 6 * 3520 + 1 for d in D))
assert_("every design walls at least half the street line", all(d["measures"]["frontage"] >= 0.5 - 1e-9 for d in D))
assert_("every design within the height limit of 60 m", all(d["measures"]["height"] <= 60 + 1e-9 for d in D))
assert_("each design's passage changes only its configuration's variant",
        all(d["measures"]["configuration"] == a["variants"][d["variant"]]["configuration"] for d in D))
again = ask("design", request)
assert_("the search is the same when asked again", json.dumps(again["designs"]) == json.dumps(D))
s = ask("sunmap", {"design": 0})
assert_("the hours of sun on the open ground lie between none and the day's five", len(s["points"]) > 0 and all(0 <= p[2] <= s["hours"] for p in s["points"]) and s["hours"] == 5,
        f"{len(s['points'])} points")
built = ask("sunmap", {"design": -1})
assert_("... and as built too, its ground summing to the street sun", abs(sum(p[2] for p in built["points"]) * 16 - a["asBuilt"]["streetSun"]) < 1, f"{sum(p[2] for p in built['points']) * 16} against {a['asBuilt']['streetSun']}")
