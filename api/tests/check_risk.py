"""Checks of the risks of a place at the API's edge (api/cpp/science/risk.hpp), run by api/tests/run-tests.mjs where the Urban Science
Workbench's plug-in *Lifelines and evacuation* has built the engine: on a lattice of streets with a band of water written as a map
here, the links under the band are closed and carry the scenarios' loss, every link has a criticality and a risk, an area across the
water from hospitals along the south edge is reached by as many link-disjoint paths as streets cross it; coverage by a station falls when the band closes; people walking
out of the band reach the dry ground, the simulation keeps its account and stays within the bound; a strategy of financing costed as
by hand; a parametric cover refused without an index. One JSON object a line, as check_client.py reports.

    python api/tests/check_risk.py
"""

from __future__ import annotations

import json
import math
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ENGINE = ROOT / "data" / "cache" / "engines" / "urban" / "lifelines" / "lifelines"


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "risks of a place: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "risks of a place: " + name})


def runner():
    """The engine as a function of a request, natively or in WSL."""
    if ENGINE.exists() and sys.platform != "win32":
        cmd = [str(ENGINE)]
    elif ENGINE.exists():
        drive = str(ENGINE)[0].lower()
        cmd = ["wsl", "--exec", "/mnt/" + drive + str(ENGINE)[2:].replace("\\", "/")]
    else:
        return None

    def ask(op: str, data: str, **req) -> dict:
        req["op"] = op
        args = cmd + ["--data", data]
        p = subprocess.run(args, input=json.dumps(req) + "\n", capture_output=True, text=True, encoding="utf-8", timeout=900)
        line = p.stdout.strip().splitlines()[-1] if p.stdout.strip() else "{}"
        out = json.loads(line)
        if not out.get("ok"):
            raise RuntimeError(out.get("error") or p.stderr[-400:])
        return out["result"]

    return ask


def wsl_path(p: Path) -> str:
    s = str(p.resolve())
    return "/mnt/" + s[0].lower() + s[2:].replace("\\", "/") if sys.platform == "win32" else s


# ---- the helpers of science.risk, which need no engine ------------------------------------------------------------------------
sys.path.insert(0, str(ROOT / "api" / "python"))
from science import risk as sr  # noqa: E402

Fm = sr.Frame(8.45, -13.2)
la, lo = Fm.geo(*Fm.local(8.4617, -13.1871))
assert_("science.risk's frame carries a point there and back within a millimetre", abs(la - 8.4617) * 110600 < 1e-3 and abs(lo + 13.1871) * 110600 < 1e-3)
Pf = {"ways": [{"cls": "primary", "width": 10, "xy": [0, 0, 100, 0]}, {"cls": "motorway", "width": 20, "xy": [0, 0, 0, 100]}, {"cls": "footway", "width": 2, "xy": [0, 0, 50, 50]}],
      "buildings": [{"x": 1, "y": 2, "floor": 300, "kind": "hospital", "name": "", "height": 20, "footprint": 100}, {"x": 5, "y": 5, "floor": 50, "kind": "house", "name": "", "height": 3, "footprint": 50}]}
assert_("by road: a primary road at 50 km/h, a footway closed to cars", sr.drive_ways(Pf) == [[round(50 / 3.6, 3), 10, 0, 0, 100, 0], [round(90 / 3.6, 3), 20, 0, 0, 0, 100]])
assert_("on foot: a motorway closed to walkers, a footway open", [w[2:] for w in sr.walk_ways(Pf, 1.0)] == [[0, 0, 100, 0], [0, 0, 50, 50]])
assert_("care is the buildings mapped as such; a tall building's refuge its floors from the third up", sr.care(Pf) == [[1, 2, 300, "hospital"]] and sr.tall(Pf)[0][2] == 100 * 4)

ask = runner()
if ask is None:
    skip("the engine (the Urban Science Workbench's plug-in has not built it: python applications/urban-science-workbench/engine.py build lifelines)")
    sys.exit(0)

# A 6 × 6 lattice of streets 100 m apart about the frame's centre, and water 0.6 m deep in a band across y = 250 ± 15 m.
FRAME = {"lat": 8.45, "lon": -13.2}
DEG = math.pi / 180
A, F = 6378137.0, 1 / 298.257223563
E2 = F * (2 - F)
SN = math.sin(FRAME["lat"] * DEG)
M, NV = A * (1 - E2) / (1 - E2 * SN * SN) ** 1.5, A / math.sqrt(1 - E2 * SN * SN)
lat = lambda y: FRAME["lat"] + y / (DEG * M)  # noqa: E731
lon = lambda x: FRAME["lon"] + x / (DEG * NV * math.cos(FRAME["lat"] * DEG))  # noqa: E731
ways = []
for i in range(6):
    h, v = [10, 8], [10, 8]
    for j in range(6):
        h += [j * 100, i * 100]
        v += [i * 100, j * 100]
    ways += [h, v]
S, W, N, E = lat(-50), lon(-50), lat(550), lon(550)
width = height = 600
grid = bytearray(width * height)
for row in range(height):
    y = -50 + row + 0.5
    if abs(y - 250) <= 15:
        for col in range(width):
            grid[row * width + col] = 12  # 0.6 m
tmp = Path(tempfile.mkdtemp())
(tmp / "band.bin").write_bytes(bytes(grid))
hazard = {"maps": [{"return_period": 100, "path": "band.bin", "box": [S, W, N, E], "width": width, "height": height, "unit": 0.05}]}
data = wsl_path(tmp)
people = [[x * 50 + 25, y * 50 + 25, 10] for x in range(10) for y in range(10)]

C = ask("criticality", data, ways=ways, frame=FRAME, people=people, services=[[0, 0], [250, 0], [500, 0]], measure={"threshold": 120, "cap": 600}, hazard=hazard, curve="vehicles",
        areas=[{"name": "north", "polygon": [-20, 270, 520, 270, 520, 520, -20, 520]}])
scen = {s["name"]: s for s in C["scenarios"]}
assert_("the 100-year map closes the six streets that cross the band", scen["100-year"]["closed"] == 6, scen)
assert_("and leaves the people north of it cut off from the hospitals along the south edge", scen["100-year"]["isolated"] > 0 and scen["undisturbed"]["loss"] == 0, scen)
assert_("every link has a criticality and a risk", len(C["criticality"]) == len(C["links"]) == len(C["risk"]) > 0)
top = C["top_risk"][0]
assert_("the links of greatest risk are the band's", top["risk"] > 0 and max(C["depth"]) > 0.5, top)
north = C["areas"][0]
assert_("the north is reached by six link-disjoint paths, and by none when the band floods", north["paths"] == 6 and north["paths_worst"] == 0, north)

Rr = ask("response", data, ways=ways, frame=FRAME, people=people, stations=[[0, 0]], threshold=90, hazard=hazard, curve="vehicles", candidates=[[400, 450], [100, 100]], k=1)
cov = {s["name"]: s for s in Rr["scenarios"]}
assert_("coverage by a station at the south corner falls when the band floods", cov["100-year"]["covered"] < cov["undisturbed"]["covered"], cov)
assert_("a second station north of the band is chosen first", Rr["siting"]["chosen"] == [0], Rr["siting"])

Ev = ask("evacuation", data, ways=ways, frame=FRAME, people=[[x * 50 + 25, 250, 40] for x in range(10)], shelters=[], hazard=hazard, safe="outside",
         walking={"speed": 1.0}, warning=600, step=10, horizon=1200, read=15, frames=60, only_in_zone=True)
assert_("the people in the band walk out of it: all within the warning", abs(Ev["within"] - 400) < 1e-6 and Ev["safe_nodes"] > 0, Ev)
sim, B = Ev["simulation"], Ev["curve"]
assert_("the simulation keeps its account", abs(sim["residual"]) < 1e-6 * 400, sim)
excess = max(s - b for s, b in zip(B["simulated"], B["bound"]))
assert_("and stays within the bound with the people's departures (to 1%)", excess <= 4, excess)

F = ask("financing", data, losses=[10, 50, 200], years=100,
        strategies=[{"name": "insured", "instruments": [{"name": "cover", "kind": "insurance", "attachment": 20, "limit": 80, "fixed": 0.5, "multiple": 2}],
                     "unfunded": [{"up_to": 50, "unit": 1}, {"unit": 3}]}], return_periods=[50, 100], parametric={"attachment": 20, "limit": 80})
assert_("a strategy costed as by hand: 5.6 a year", abs(F["strategies"][0]["expected"] - 5.6) < 1e-9, F["strategies"][0])
assert_("a parametric cover is refused without an index for every event", "refused" in F["parametric"], F.get("parametric"))
try:
    ask("criticality", data, ways=[], frame=FRAME, people=people, services=[[0, 0]])
    assert_("no roads is refused with its reason", False)
except RuntimeError as e:
    assert_("no roads is refused with its reason", "no roads" in str(e), e)
