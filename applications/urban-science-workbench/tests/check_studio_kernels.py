"""Checks of the Studio's kernels (the design note of 2026-11-07, §8.7): scripts in Python and C++ loaded while others run, ticked with what
they read, loaded again with their state handed over, their errors told with the line of their own source, a version that fails to load
leaving the one before in place, and let go. And a new project as the server seeds it. Each line is "ok  name" or "FAIL  name: why", read by
run-tests.mjs; "skip  name: why" where a toolchain is missing.
"""

from __future__ import annotations

import json
import math
import sys
import tempfile
import time
from pathlib import Path

APP = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(APP))
import studio_kernels as sk  # noqa: E402
import studio_project  # noqa: E402

EXAMPLES = APP / "src" / "studio" / "examples"
failures = 0


def check(name: str, ok: bool, why: str = "") -> None:
    global failures
    if not ok:
        failures += 1
    print(f"{'ok  ' if ok else 'FAIL'}  {name}" + (f": {why}" if why and not ok else ""), flush=True)


def skip(name: str, why: str) -> None:
    print(f"skip  {name}: {why}", flush=True)


CROWD = {"lon": [144.90, 144.91, 145.30, 144.905, 145.31], "lat": [-37.80, -37.805, -37.60, -37.801, -37.61]}

# A system with a state, which save() and load() hand over: a counter of its ticks.
COUNTER_PY = '''
def create(api):
    class Counter(api.System):
        title = "A counter"
        writes = ["count"]
        resources = {"count": {"type": "number", "initial": 0}}

        def __init__(self):
            self.n = 0

        def tick(self, ctx, io):
            self.n += STEP
            io.set("count", self.n)

        def save(self):
            return {"n": self.n}

        def load(self, state):
            self.n = state["n"]

    return Counter()
'''

FAILING_PY = '''def create(api):
    class Failing(api.System):
        title = "It fails"

        def tick(self, ctx, io):
            x = 1
            return x / 0

    return Failing()
'''

COUNTER_CPP = r'''#include "studio_plugin.hpp"
namespace st = science::studio;
using science::Json;
struct Counter : st::System {
  double n = 0;
  Json describe() const override { return st::declaration("A counter", "tick", {}, {"count"}, {}, Json::array(), Json::object()); }
  void tick(const st::Context&, st::Io& io) override { n += STEP; io.set("count", Json(n)); }
  Json save() const override { return Json::object({{"n", Json(n)}}); }
  void load(const Json& s) override { if (s.is_object() && s.has("n")) n = s["n"].as_number(); }
};
SCIENCE_STUDIO_SYSTEM(Counter)
'''


def tick(kernel, ident, values=None, n=1):
    return kernel.tick(ident, {"tick": n, "dt": 0.1, "time_ms": 0, "params": {}, "view": {"lon": 145, "lat": -37.8}, "values": values or {}, "previous": {}})


with tempfile.TemporaryDirectory() as tmp:
    tmp = Path(tmp)
    kernels = sk.Kernels()
    try:
        # ---- Python ----------------------------------------------------------------------------------------------------
        py = kernels.python
        a = py.load("p/gathering", EXAMPLES / "gathering.py")
        d = a["describe"]
        check("python: a script's declaration read on loading", d["reads"] == ["crowd"] and d["writes"] == ["densest"] and d["phase"] == "tick", json.dumps(d))
        r = tick(py, "p/gathering", {"crowd": CROWD})
        # Cells of 20 km: three walkers about 144.90°E, two about 145.30°E.
        check("python: a tick answers what it writes and draws", r["writes"].get("densest") == 3 and r["marks"] and r["marks"][0]["kind"] == "columns", json.dumps(r["writes"]))
        line = next((m for m in r["marks"] if m["kind"] == "chart"), None)
        check("python: a chart for the canvas's frame, its values the script's", line is not None and line["frame"] == "densest" and line["y"] == [3] and line["x"] == [1], json.dumps(line)[:200])

        (tmp / "counter.py").write_text(COUNTER_PY.replace("STEP", "1"), encoding="utf-8")
        py.load("p/counter", tmp / "counter.py")
        for k in range(3):
            r = tick(py, "p/counter", n=k + 1)
        (tmp / "counter.py").write_text(COUNTER_PY.replace("STEP", "10"), encoding="utf-8")
        a = py.load("p/counter", tmp / "counter.py")
        r = tick(py, "p/counter", n=4)
        check("python: loaded again while it runs, its state handed over through save() and load()", a["carried"] and r["writes"]["count"] == 13, f"carried {a['carried']}, count {r['writes'].get('count')}")

        (tmp / "broken.py").write_text("def create(api):\n    return (\n", encoding="utf-8")
        try:
            py.load("p/counter", tmp / "broken.py")
            check("python: a version that does not load is refused with its line", False, "it loaded")
        except sk.KernelError as e:
            check("python: a version that does not load is refused with its line", e.line in (2, 3), f"line {e.line}: {e}")
        r = tick(py, "p/counter", n=5)
        check("python: the version before goes on running", r["writes"]["count"] == 23, json.dumps(r["writes"]))

        (tmp / "failing.py").write_text(FAILING_PY, encoding="utf-8")
        py.load("p/failing", tmp / "failing.py")
        try:
            tick(py, "p/failing")
            check("python: an error in a tick is told with the line of the script", False, "no error")
        except sk.KernelError as e:
            check("python: an error in a tick is told with the line of the script", e.line == 7 and "ZeroDivisionError" in str(e), f"line {e.line}: {e}")

        py.unload("p/counter")
        try:
            tick(py, "p/counter")
            check("python: a script let go answers no more", False, "it answered")
        except sk.KernelError:
            check("python: a script let go answers no more", True)

        # ---- C++ -------------------------------------------------------------------------------------------------------
        cpp = kernels.cpp
        try:
            cpp._kind()
            have = True
        except sk.KernelError as e:
            have = False
            skip("c++: the kernel", str(e))
        if have:
            t0 = time.time()
            a = cpp.load("p/spread", EXAMPLES / "spread.cpp")
            check(f"c++: a script compiled alone and loaded ({a.get('compile_s')} s to compile, {time.time() - t0:.1f} s in all)", a["describe"]["writes"] == ["spread_km"], json.dumps(a["describe"]))
            r = tick(cpp, "p/spread", {"crowd": CROWD})
            clon, clat = sum(CROWD["lon"]) / 5, sum(CROWD["lat"]) / 5
            kx, ky = 111.32 * math.cos(math.radians(clat)), 110.57
            rms = math.sqrt(sum(((x - clon) * kx) ** 2 + ((y - clat) * ky) ** 2 for x, y in zip(CROWD["lon"], CROWD["lat"])) / 5)
            check("c++: a tick answers what it writes (the crowd's spread, as computed here)", abs(r["writes"]["spread_km"] - rms) < 1e-9, f"{r['writes'].get('spread_km')} against {rms}")
            bars = next((m for m in r["marks"] if m["kind"] == "chart"), None)
            check("c++: a histogram for the canvas's frame, every walker in one bin", bars is not None and bars["frame"] == "distances" and bars["chart"] == "bars" and len(bars["y"]) == 12 and sum(bars["y"]) == 5,
                  json.dumps(bars)[:200])

            (tmp / "counter.cpp").write_text(COUNTER_CPP.replace("STEP", "1"), encoding="utf-8")
            cpp.load("p/counter", tmp / "counter.cpp")
            for k in range(3):
                tick(cpp, "p/counter", n=k + 1)
            (tmp / "counter.cpp").write_text(COUNTER_CPP.replace("STEP", "10"), encoding="utf-8")
            a = cpp.load("p/counter", tmp / "counter.cpp")
            r = tick(cpp, "p/counter", n=4)
            check("c++: compiled again and swapped in while it runs, its state handed over", a["carried"] and r["writes"]["count"] == 13, f"carried {a['carried']}, count {r['writes'].get('count')}")

            (tmp / "counter.cpp").write_text(COUNTER_CPP.replace("STEP", "1").replace("n += 1;", "n += undeclared;"), encoding="utf-8")
            try:
                cpp.load("p/counter", tmp / "counter.cpp")
                check("c++: a version that does not compile is refused with its line", False, "it compiled")
            except sk.KernelError as e:
                check("c++: a version that does not compile is refused with its line", e.line == 7 and "undeclared" in str(e), f"line {e.line}: {e}")
            r = tick(cpp, "p/counter", n=5)
            check("c++: the version before goes on running", r["writes"]["count"] == 23, json.dumps(r["writes"]))

            cpp.unload("p/counter")
            try:
                tick(cpp, "p/counter")
                check("c++: a script let go answers no more", False, "it answered")
            except sk.KernelError:
                check("c++: a script let go answers no more", True)
    finally:
        kernels.stop()

    # ---- a new project ----------------------------------------------------------------------------------------------------
    name, project = studio_project.create(tmp / "projects", "Walkers and the weather")
    files = sorted(f.name for f in (tmp / "projects" / name / "scripts").iterdir())
    check("project: a new one is seeded with four scripts in the three languages, a layer for each that draws on the world", files == ["compass.js", "crowd.js", "gathering.py", "spread.cpp"]
          and [s["language"] for s in project["scripts"]] == ["js", "python", "cpp", "js"] and len(project["layers"]) == 3, ",".join(files))
    kinds = [f["kind"] for f in project["frames"]]
    inside = all(0 <= f["x"] and 0 <= f["y"] and f["x"] + f["w"] <= project["canvas"]["width"] and f["y"] + f["h"] <= project["canvas"]["height"] for f in project["frames"])
    check("project: its canvas holds the world, a title, a map, two charts and a drawing, all on it", kinds == ["world", "text", "map", "chart", "chart", "drawing"] and inside, ",".join(kinds))
    hour = (project["scene"]["clock"]["start_ms"] / 3600000 + 11) % 24
    check("project: its clock starts in Melbourne's afternoon (UTC+11)", abs(hour - 13.5) < 1e-9, f"{hour}")
    name2, _ = studio_project.create(tmp / "projects", "Walkers and the weather")
    check("project: a second of the same title has a folder of its own", name2 != name and (tmp / "projects" / name2 / "project.json").is_file(), name2)

sys.exit(1 if failures else 0)
