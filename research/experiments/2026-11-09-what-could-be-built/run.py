"""The what-could-be-built study's runs: the API's composition (api/cpp/science/design.hpp) on three blocks of Melbourne, from the data
library's city model, under the three rule sets the workbench's plug-in offers, keeping half or a tenth of the floor standing now; and
the first block's first rules under three seeds.

The requests are built as the plug-in's page builds them (plugins/architecture/layers/design.js): streets of the classes that bound
blocks within 700 m, the ways walked within 1,100 m, the buildings within 600 m with their heights, the Sun at 10:00–14:00 solar time
on 21 June 2026 from the astronomy domain (sun.mjs), the air 8 °C by day and 4 °C by night. The page also takes the world's tiles'
buildings beside a city model; here the city model alone stands.

Writes results/runs.json (committed: the measures of the designs, as built and on the front; no geometry) and results/requests/ with
the answers (extracts of OpenStreetMap under ODbL, not committed).
Run from the repository's root, after `python data/products/street_records.py` has built the city model of Melbourne:
    python research/experiments/2026-11-09-what-could-be-built/run.py
"""

import json
import pathlib
import subprocess
import sys
import time

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "data" / "products"))
from street_records import enu  # noqa: E402

CITY = ROOT / "data" / "cache" / "products" / "city-model-melbourne.json"
OUT = HERE / "results"
BUILD = "$HOME/.cache/science-build/exp-what-could-be-built"  # expanded by WSL's bash, outside quotes
BLOCKS = {
    "collins": {"name": "Collins Street to Flinders Lane, Swanston to Russell", "lat": -37.8158, "lon": 144.9672},
    "bourke": {"name": "Bourke Street to Little Bourke, Elizabeth to Swanston", "lat": -37.8133, "lon": 144.9636},
    "carlton": {"name": "a block of Carlton", "lat": -37.7990, "lon": 144.9690},
}
STREETS = {"motorway", "trunk", "primary", "secondary", "tertiary", "residential", "living_street", "unclassified", "pedestrian", "road",
           "motorway_link", "trunk_link", "primary_link", "secondary_link", "tertiary_link"}
NOT_WALKED = {"motorway", "motorway_link"}
STOREY = 3.2
# The same rules as the page's (design.js), declared: none is any city's planning scheme.
RULES = {
    "wall": {"max_height": 100, "plot_ratio": 12, "street_setback": 0, "side_setback": 0, "street_wall": 24, "upper_setback": 10, "storey": STOREY, "min_frontage": 0.6},
    "mid": {"max_height": 32, "plot_ratio": 4.5, "street_setback": 0, "side_setback": 0, "street_wall": 16, "upper_setback": 5, "storey": STOREY, "min_frontage": 0.8},
    "apart": {"max_height": 150, "plot_ratio": 8, "street_setback": 6, "side_setback": 6, "street_wall": 9.6, "upper_setback": 4, "storey": STOREY, "min_frontage": 0},
}
AIR = {"day": 8, "night": 4}
GENERATIONS = 60
INCLUDE = ["foundations/morphe/include", "foundations/morphe/*/include", "foundations/calx/numerics/*/include", "foundations/calx/compute/*/include",
           "foundations/calx/compute/backends/*/include", "foundations/physis/*/include", "foundations/tyche/*/include", "foundations/nomos/*/include",
           "foundations/heuresis/*/include", "foundations/syntaxis/*/include", "domains/urban-science/*/include", "domains/architecture/*/include", "api/cpp"]


def wsl_path(p: pathlib.Path) -> str:
    return subprocess.run(["wsl", "-e", "wslpath", "-a", str(p)], capture_output=True, text=True).stdout.strip()


def build() -> None:
    dirs = []
    for pattern in INCLUDE:
        dirs += sorted(p for p in ROOT.glob(pattern) if p.is_dir()) if "*" in pattern else [ROOT / pattern]
    args = ["g++", "-std=c++20", "-O2", "-DNDEBUG", "-pthread"] + [f"-I{wsl_path(d)}" for d in dirs]
    args += [wsl_path(HERE / "study.cpp")]
    started = time.time()
    r = subprocess.run(["wsl", "-e", "bash", "-c", f"mkdir -p {BUILD} && " + " ".join(f"'{a}'" for a in args) + f" -o {BUILD}/study"], capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(r.stderr[-4000:])
    print(f"built the study in {time.time() - started:.0f} s")


def cut(lines, at, radius, keep):
    """Each way's runs of points within the radius, as the page cuts them."""
    out = []
    for kind, line in lines:
        if not keep(kind):
            continue
        run = []
        for x, y in line:
            if (x - at[0]) ** 2 + (y - at[1]) ** 2 <= radius * radius:
                run += [round(x, 1), round(y, 1)]
            else:
                if len(run) >= 4:
                    out.append(run)
                run = []
        if len(run) >= 4:
            out.append(run)
    return out


def centroid(ring):
    a = cx = cy = 0.0
    for (x0, y0), (x1, y1) in zip(ring, ring[1:] + ring[:1]):
        c = x0 * y1 - x1 * y0
        a, cx, cy = a + c, cx + (x0 + x1) * c, cy + (y0 + y1) * c
    return (cx / (3 * a), cy / (3 * a)) if abs(a) > 1e-9 else ring[0]


def request(city, block, rules, keep, seed=1):
    o = city["origin"]
    e, n, _ = enu(block["lat"], block["lon"], o["lat"], o["lon"])
    at = [e, n]
    lines = [(r["kind"], [tuple(p) for p in r["line"]]) for r in city["roads"]]
    buildings = []
    for b in city["buildings"]:
        ring = [tuple(p) for p in b["footprint"]]
        if len(ring) < 3:
            continue
        cx, cy = centroid(ring)
        if (cx - at[0]) ** 2 + (cy - at[1]) ** 2 > 600 ** 2:
            continue
        h = b.get("height_m") or 3 * STOREY
        buildings.append([round(h, 1)] + [round(v, 1) for p in ring for v in p])
    hours = [10, 11, 12, 13, 14]
    suns = json.loads(subprocess.run(["node", str(HERE / "sun.mjs"), json.dumps({"lat": block["lat"], "lon": block["lon"], "date": "2026-06-21", "hours": hours})],
                                     capture_output=True, text=True, check=True).stdout)
    return {"at": at, "streets": cut(lines, at, 700, lambda k: k in STREETS), "ways": cut(lines, at, 1100, lambda k: k not in NOT_WALKED),
            "buildings": buildings, "rules": RULES[rules], "suns": [[a, z, 1, AIR["day"]] for a, z in suns], "night": {"air": AIR["night"], "hours": 24 - len(hours)},
            "generations": GENERATIONS, "seed": seed, "least_floor": keep}


def main():
    (OUT / "requests").mkdir(parents=True, exist_ok=True)
    build()
    city = json.load(open(CITY, encoding="utf-8"))
    runs = [(b, r, k, 1) for b in BLOCKS for r in RULES for k in (0.5, 0.1)] + [("collins", "wall", 0.5, s) for s in (2, 3)]
    records = []
    for b, r, k, seed in runs:
        name = f"{b}-{r}-{int(k * 100)}-{seed}"
        q, a = OUT / "requests" / f"{name}.json", OUT / "requests" / f"{name}.answer.json"
        json.dump(request(city, BLOCKS[b], r, k, seed), open(q, "w", encoding="utf-8"))
        started = time.time()
        subprocess.run(["wsl", "-e", "bash", "-c", f"{BUILD}/study '{wsl_path(q)}' '{wsl_path(a)}'"])
        ans = json.load(open(a, encoding="utf-8"))
        if "error" in ans:
            print(f"{name}: {ans['error']}")
            records.append({"block": b, "rules": r, "keep": k, "seed": seed, "error": ans["error"]})
            continue
        records.append({"block": b, "rules": r, "keep": k, "seed": seed, "lots": len(ans["lots"]), "lotArea": ans["lotArea"], "context": ans["context"],
                        "asBuilt": ans["asBuilt"], "designs": [{"variant": d["variant"], **d["measures"]} for d in ans["designs"]],
                        "variants": [{"name": v["name"], "configuration": v["configuration"], "passageChoice": v["passageChoice"]} for v in ans["variants"]],
                        "complying": ans["complying"], "pieces": ans["pieces"], "failing": ans["failing"], "evaluations": ans["evaluations"],
                        "leastFloor": ans["leastFloor"], "plotRatioFloor": ans["plotRatioFloor"], "seconds": ans["seconds"], "threads": ans["threads"]})
        print(f"{name}: {len(ans['lots'])} lots, front {len(ans['designs'])}, {ans['complying']}/{ans['pieces']} within the rules, "
              f"{ans['seconds']['preparing'] + ans['seconds']['searching']:.1f} s (wall {time.time() - started:.0f} s)")
    json.dump({"blocks": BLOCKS, "rules": RULES, "air": AIR, "generations": GENERATIONS, "runs": records}, open(OUT / "runs.json", "w", encoding="utf-8"), indent=1)


if __name__ == "__main__":
    main()
