"""Checks of the plug-in Land use and transport (ADR 0094), run by tests/run-tests.mjs, on a place made by hand: a grid of residential
streets every 500 m over 4 km, two primary roads crossing at its centre, sixteen cells of 1 km with households, and jobs gathered at
the centre. The engine answers on the API's composition (api/cpp/science/luti.hpp):

- the network is contracted (fewer nodes than points), and every year's equilibrium reaches the gap asked;
- as things stand nothing changes, year after year (a revealed place is a fixed point while no rent invites building);
- households located and outside sum to the households who consider the place;
- allowing more dwellings where no rent invites building changes nothing;
- widening the primary road from the west lowers its load and gives the cells it serves, farthest west, more households than as things
  stand, after their rents rose (the cells next to the centre may lose a little: the traffic the road draws crowds the links
  into the centre they use);
- closing it lengthens the mean journey to work the year it closes;
- jobs added in a corner cell draw households to the cells about it.

    python applications/urban-science-workbench/tests/check_land_use.py
"""

from __future__ import annotations

import json
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


if not engine.status("land-use")["current"]:
    skip("the engine of Land use and transport is not built from the present sources (python applications/urban-science-workbench/engine.py build land-use)")
    sys.exit(0)
worker = engine.Worker("land-use")


def ask(op: str, request: dict) -> dict:
    a = worker.ask({**request, "op": op})
    if not a.get("ok"):
        raise RuntimeError(a.get("error"))
    return a["result"]


KMH = 1 / 3.6
ways = []
# Residential streets every 500 m, split where they meet (as the world's tiles hold them); the primary roads along x = 0 and y = 0.
for x in range(-2000, 2001, 500):
    for y in range(-2000, 2000, 500):
        primary = x == 0
        ways.append([50 * KMH if primary else 25 * KMH, 1800 if primary else 500, x, y, x, y + 500])
for y in range(-2000, 2001, 500):
    for x in range(-2000, 2000, 500):
        primary = y == 0
        ways.append([50 * KMH if primary else 25 * KMH, 1800 if primary else 500, x, y, x + 500, y])
west = [k for k, w in enumerate(ways) if w[3] == 0 and w[5] == 0 and w[2] < 0]   # the primary road west of the centre
zones = []
for cy in range(4):
    for cx in range(4):
        x, y = -1500 + 1000 * cx, -1500 + 1000 * cy
        central = cx in (1, 2) and cy in (1, 2)
        zones.append([x, y, 8000.0, 20000.0 if central else 2000.0])
west_cells = [k for k, z in enumerate(zones) if z[0] == -1500 and abs(z[1]) == 500]   # the cells the widened road serves, farthest from the centre
corner = 0
near_corner = [1, 4]
far = [15]
base = {"ways": ways, "zones": zones, "years": 8, "parameters": {"gap": 1e-3, "peak": 0.2}}   # a fifth of the workers on the roads in the peak hour: congested

plain = ask("run", {**base, "scenario": {}})
p = plain["plain"]
assert_("the network is contracted to its junctions", plain["network"]["nodes"] < plain["network"]["points"], json.dumps(plain["network"]))
assert_("every year's equilibrium reaches the gap asked", all(y["gap"] <= 1e-3 for y in p), str([y["gap"] for y in p]))
drift = max(abs(y["zones"][i][0] - p[0]["zones"][i][0]) / p[0]["zones"][i][0] for y in p for i in range(len(zones)))
check("as things stand nothing changes, year after year (households)", drift, 1e-6)
rent = max(abs(y["zones"][i][1] - 100) for y in p for i in range(len(zones)))
check("as things stand the rents stay at their index of 100", rent, 1e-3)
total = plain["households"] * 2
check("households located and outside sum to the households who consider the place", max(abs(y["located"] + y["outside"] - total) / total for y in p), 1e-3)

built = ask("run", {**base, "scenario": {"year": 1, "built": [[k, 2.0] for k in range(len(zones))]}})
same = max(abs(a["zones"][i][0] - b["zones"][i][0]) / b["zones"][i][0] for a, b in zip(built["changed"], built["plain"]) for i in range(len(zones)))
check("allowing more dwellings where no rent invites building changes nothing", same, 1e-6)

wide = ask("run", {**base, "scenario": {"year": 1, "widened": [[k, 3.0] for k in west]}})
load = lambda year, ks: max((v for k, v in year["loads"] if k in ks), default=0)
assert_("widening the primary road from the west lowers its load in the peak hour", load(wide["changed"][1], west) < load(wide["plain"][1], west),
        f"{load(wide['changed'][1], west):.3f} against {load(wide['plain'][1], west):.3f}")
gain = [wide["changed"][-1]["zones"][k][0] / wide["plain"][-1]["zones"][k][0] - 1 for k in west_cells]
assert_("... and gives the cells it serves, farthest west, more households than as things stand, by the last year", min(gain) > 0, str([round(g, 5) for g in gain]))
assert_("... where their rents rose first", all(wide["changed"][2]["zones"][k][1] > 100 for k in west_cells), str([wide["changed"][2]["zones"][k][1] for k in west_cells]))

shut = ask("run", {**base, "scenario": {"year": 2, "closed": west}})
assert_("closing it lengthens the mean journey to work the year it closes", shut["changed"][2]["commute"] > shut["plain"][2]["commute"],
        f"{shut['changed'][2]['commute']} against {shut['plain'][2]['commute']} minutes")

jobs = ask("run", {**base, "scenario": {"year": 1, "jobs": [[corner, 25000]]}})
near = [jobs["changed"][-1]["zones"][k][0] / jobs["plain"][-1]["zones"][k][0] - 1 for k in near_corner]
away = [jobs["changed"][-1]["zones"][k][0] / jobs["plain"][-1]["zones"][k][0] - 1 for k in far]
assert_("jobs added in a corner draw households to the cells about it more than to the far corner", min(near) > max(away), f"{near} against {away}")
emit({"kind": "note", "seconds": [plain["seconds"], wide["seconds"], jobs["seconds"]]})
