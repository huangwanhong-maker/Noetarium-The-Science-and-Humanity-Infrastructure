"""Lifelines and evacuation on real data (ADR 0129): Freetown's roads under its river floods, its people walking out of the 1-in-100-year
flood's reach and its flood losses met by layers of funds; Miyako's people walking out of the assumed tsunami's reach. The same composition the Urban Science Workbench's plug-in asks
(api/cpp/science/risk.hpp, through the plug-in's engine), on the same data the page holds: Overture's ways and buildings from the
data library's city tiles, GHS-POP's people shared among buildings by floor space, the data library's hazard maps.

    python research/experiments/2026-11-18-lifelines-and-evacuation/run.py [--criticality]

writes results.json beside it; README.md reads them.
"""

from __future__ import annotations

import json
import math
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "data" / "acquisition"))
import services  # noqa: E402
import world_city  # noqa: E402

ENGINE = ROOT / "data" / "cache" / "engines" / "urban" / "lifelines" / "lifelines"
PRODUCTS = ROOT / "data" / "cache" / "products"
KMH = 1 / 3.6
DRIVE = {"motorway": 90, "motorway_link": 50, "trunk": 70, "trunk_link": 40, "primary": 50, "primary_link": 35, "secondary": 40, "secondary_link": 30,
         "tertiary": 35, "tertiary_link": 25, "residential": 25, "unclassified": 25, "living_street": 10, "service": 15}
NO_FOOT = {"motorway", "motorway_link", "trunk", "trunk_link"}
CARE = {"hospital", "clinic", "doctors", "medical", "healthcare"}


class Frame:
    """The engine's frame: WGS 84's radii of curvature at a centre (disaster_risk::disruption::Frame)."""

    def __init__(self, lat0: float, lon0: float):
        a, f = 6378137.0, 1 / 298.257223563
        e2, s = f * (2 - f), math.sin(math.radians(lat0))
        self.lat0, self.lon0 = lat0, lon0
        self.m = a * (1 - e2) / (1 - e2 * s * s) ** 1.5
        self.n = a / math.sqrt(1 - e2 * s * s)

    def local(self, lat: float, lon: float) -> tuple[float, float]:
        return (math.radians(lon - self.lon0) * self.n * math.cos(math.radians(self.lat0)), math.radians(lat - self.lat0) * self.m)

    def geo(self, x: float, y: float) -> tuple[float, float]:
        return (self.lat0 + math.degrees(y / self.m), self.lon0 + math.degrees(x / (self.n * math.cos(math.radians(self.lat0)))))


def place(lat: float, lon: float, km: float) -> dict:
    """Ways, buildings and people within km of a place, in the frame centred there."""
    F = Frame(lat, lon)
    z, n = 14, 2 ** 14
    dlat, dlon = km / 111.32, km / (111.32 * math.cos(math.radians(lat)))
    tx = lambda lo: int((lo + 180) / 360 * n)  # noqa: E731
    ty = lambda la: int((1 - math.asinh(math.tan(math.radians(la))) / math.pi) / 2 * n)  # noqa: E731
    ways, buildings = [], []
    for x in range(tx(lon - dlon), tx(lon + dlon) + 1):
        for y in range(ty(lat + dlat), ty(lat - dlat) + 1):
            t = json.loads(world_city.city_tile(z, x, y))
            for cls, width, name, flat, rail in t["ways"]:
                if rail:
                    continue
                pts = [F.local(flat[i + 1], flat[i]) for i in range(0, len(flat), 2)]
                ways.append({"cls": cls, "width": width, "xy": [round(v, 1) for p in pts for v in p]})
            for b in t["buildings"]:
                ring = b[3]
                pts = [F.local(ring[i + 1], ring[i]) for i in range(0, len(ring), 2)]
                area = 0.0
                cx = cy = 0.0
                for i in range(len(pts)):
                    (x0, y0), (x1, y1) = pts[i], pts[(i + 1) % len(pts)]
                    c = x0 * y1 - x1 * y0
                    area += c
                    cx += (x0 + x1) * c
                    cy += (y0 + y1) * c
                if abs(area) < 1e-9:
                    continue
                h = b[0] if b[0] and b[0] > 0 else 3.2
                kind = (b[6] if len(b) > 6 else None) or ""
                name = (b[5] if len(b) > 5 else None) or ""
                buildings.append({"x": cx / (3 * area), "y": cy / (3 * area), "footprint": abs(area) / 2, "height": h,
                                  "floor": abs(area) / 2 * max(1, round(h / 3.2)), "kind": str(kind).lower(), "name": name})
    pop = services.population_window(lat, lon, km)
    dx, dy = pop["cell_degrees"]
    cells = {}
    for la, lo, people, _ in pop["cells"]:
        cells[(round(la / dy), round(lo / dx))] = {"la": la, "lo": lo, "n": people, "floor": 0.0}
    for b in buildings:
        la, lo = F.geo(b["x"], b["y"])
        b["cell"] = (round(la / dy), round(lo / dx))
        if b["cell"] in cells:
            cells[b["cell"]]["floor"] += b["floor"]
    bins = {}
    for b in buildings:
        c = cells.get(b["cell"])
        if c and c["floor"] > 0:
            k = (round(b["x"] / 50), round(b["y"] / 50))
            v = bins.setdefault(k, [0.0, 0.0, 0.0])
            share = c["n"] * b["floor"] / c["floor"]
            v[0] += b["x"] * share
            v[1] += b["y"] * share
            v[2] += share
    people = [[round(x / w), round(y / w), round(w, 1)] for x, y, w in bins.values() if w > 0.05]
    return {"frame": {"lat": lat, "lon": lon}, "F": F, "ways": ways, "buildings": buildings, "people": people}


def drive_ways(P: dict) -> list:
    return [[round(DRIVE[w["cls"]] * KMH, 3), w["width"]] + w["xy"] for w in P["ways"] if w["cls"] in DRIVE and len(w["xy"]) >= 4]


def walk_ways(P: dict, speed: float) -> list:
    return [[speed, w["width"]] + w["xy"] for w in P["ways"] if w["cls"] not in NO_FOOT and len(w["xy"]) >= 4]


def ask(op: str, **req) -> dict:
    req["op"] = op
    s = str(ENGINE.resolve())
    cmd = ["wsl", "--exec", "/mnt/" + s[0].lower() + s[2:].replace("\\", "/"), "--data", "/mnt/" + str(PRODUCTS)[0].lower() + str(PRODUCTS)[2:].replace("\\", "/")] \
        if sys.platform == "win32" else [s, "--data", str(PRODUCTS)]
    t0 = time.time()
    p = subprocess.run(cmd, input=json.dumps(req) + "\n", capture_output=True, text=True, encoding="utf-8", timeout=3600)
    out = json.loads(p.stdout.strip().splitlines()[-1])
    if not out.get("ok"):
        raise RuntimeError(out.get("error"))
    print(f"  {op}: {time.time() - t0:.1f} s")
    return out["result"]


# Departures as Miyako's people left in 2011: the shares of the survey by the Cabinet Office, FDMA and JMA (N = 870: 57% at once, 31%
# after doing something first, 11% as the tsunami neared), the times when each group left declared by this work.
DEP_2011 = {"time": [0, 120, 600, 1500, 2400, 2700], "share": [0, 0, 0.57, 0.88, 0.99, 1], "source": "the 2011 survey's shares; times declared"}


def miyako_criticality(results: dict) -> None:
    """Miyako's roads to care under its largest assumed flood (a map without a return period)."""
    print("Miyako (criticality)")
    jp = json.loads((PRODUCTS / "japan-hazards" / "03202.json").read_text(encoding="utf-8"))
    M = place(39.64, 141.955, 3.2)
    flood = [{"return_period": 0, "path": m["file"], "box": m["box"], "width": m["width"], "height": m["height"], "unit": m["unit"]} for m in jp["maps"] if m["peril"] == "flood"]
    care_m = [[round(b["x"]), round(b["y"]), round(b["floor"]), b["name"] or b["kind"]] for b in M["buildings"] if b["kind"] in CARE]
    if not care_m:
        return
    CM = ask("criticality", ways=drive_ways(M), frame=M["frame"], people=M["people"], services=care_m, measure={"threshold": 1800, "cap": 7200},
             hazard={"maps": flood}, curve="vehicles", top=10)
    results["miyako_criticality"] = {k: CM[k] for k in ("scenarios", "expected", "top_criticality", "junctions", "people", "services")}
    results["miyako_criticality"]["links"] = len(CM["links"])


def main() -> None:
    # With --criticality, only the two criticality runs are made again, the rest of results.json kept.
    only = "--criticality" in sys.argv
    results = json.loads((HERE / "results.json").read_text(encoding="utf-8")) if only and (HERE / "results.json").exists() else {}
    results["made"] = time.strftime("%Y-%m-%d")
    ft = json.loads((PRODUCTS / "freetown-flood.json").read_text(encoding="utf-8"))
    # ---- Freetown: roads under the river floods ----------------------------------------------------------------------------------
    print("Freetown")
    P = place(8.4757, -13.2332, 4.5)
    maps = [{"return_period": m["return_period"], "path": m["file"], "box": m["box"], "width": m["width"], "height": m["height"], "unit": ft["depth_code"]["metres_per_unit"]}
            for m in ft["maps"] if m["peril"] == "FLRF"]
    care = [[round(b["x"]), round(b["y"]), round(b["floor"]), b["name"] or b["kind"]] for b in P["buildings"] if b["kind"] in CARE]
    print(f"  {len(P['ways'])} ways, {len(P['buildings'])} buildings, {len(care)} of care, {sum(p[2] for p in P['people']):.0f} people")
    C = ask("criticality", ways=drive_ways(P), frame=P["frame"], people=P["people"], services=care, measure={"threshold": 1800, "cap": 7200},
            hazard={"maps": maps}, curve="vehicles", top=15)
    results["freetown_criticality"] = {k: C[k] for k in ("scenarios", "expected", "top_risk", "top_criticality", "junctions", "people", "services", "evaluations")}
    results["freetown_criticality"]["links"] = len(C["links"])
    risk = sorted(C["risk"], reverse=True)
    total = sum(r for r in risk if r > 0)
    results["freetown_criticality"]["risk_share_top"] = {str(k): (sum(risk[:k]) / total if total > 0 else None) for k in (1, 5, 10, 25)}
    results["freetown_criticality"]["links_with_risk"] = sum(1 for r in risk if r > 1e-9)
    crit = sorted(C["criticality"], reverse=True)
    results["freetown_criticality"]["links_with_criticality"] = sum(1 for c in crit if c > 1e-9)
    if only:
        miyako_criticality(results)
        (HERE / "results.json").write_text(json.dumps(results, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
        print("wrote results.json")
        return
    # ---- Freetown: financing the losses ---------------------------------------------------------------------------------------
    UNF = [{"up_to": 2e6, "unit": 1.2}, {"unit": 2.0}]
    B = {"name": "contingency budget", "kind": "budget", "rate": 0.03, "unit": 1}
    CR = {"name": "contingent credit", "kind": "credit", "rate": 0.005, "unit": 1.1}
    INS = {"name": "insurance", "kind": "insurance", "fixed": 50000, "multiple": 1.8}
    strategies = [
        {"name": "a contingency budget of $1 m", "unfunded": UNF, "instruments": [dict(B, attachment=0, limit=1e6)]},
        {"name": "and a contingent credit to $10 m", "unfunded": UNF, "instruments": [dict(B, attachment=0, limit=1e6), dict(CR, attachment=1e6, limit=9e6)]},
        {"name": "and insurance from $10 m to $60 m", "unfunded": UNF, "instruments": [dict(B, attachment=0, limit=1e6), dict(CR, attachment=1e6, limit=9e6), dict(INS, attachment=1e7, limit=5e7)]},
    ]
    Fn = ask("financing", losses=ft["events"]["losses"], years=ft["events"]["catalogue_years"], strategies=strategies, return_periods=[10, 50, 100, 500],
             tower={"order": [B, CR, INS], "candidates": [2, 5, 10, 20, 50, 100, 200, 500, 1000], "unfunded": UNF, "cap_period": 100, "cap": 0},
             parametric={"attachment": 1e7, "limit": 5e7, "multiple": 1.8})
    results["freetown_financing"] = Fn
    # ---- Freetown: walking out of the 1-in-100-year river flood's reach ------------------------------------------------------------
    print("Freetown (evacuation)")
    zone = [m for m in maps if m["return_period"] == 100]
    cands = [[round(b["x"]), round(b["y"]), round(b["footprint"] * max(0, round(b["height"] / 3.2) - 2)), b["name"] or f"a building of {round(b['height'])} m"]
             for b in P["buildings"] if b["height"] >= 12]
    cands = sorted([c for c in cands if c[2] > 0], key=lambda c: -c[2])[:80]
    print(f"  {len(cands)} tall buildings as candidate refuges")
    # Departures: everyone at once, or as Miyako's people left in 2011 (the survey's shares, declared here for Freetown).
    runs = {}
    for label, speed, departure in (("1.0 m/s, at once", 1.0, None), ("1.0 m/s, as in 2011", 1.0, DEP_2011), ("0.5 m/s, as in 2011", 0.5, DEP_2011)):
        req = dict(ways=walk_ways(P, speed), frame=P["frame"], people=P["people"], shelters=[], hazard={"maps": zone}, safe="outside", only_in_zone=True,
                   walking={"speed": speed}, warning=1800, horizon=3600, step=15, read=60, frames=300)
        if departure:
            req["departure"] = departure
        if label.startswith("0.5") and cands:
            req["candidates"], req["k"] = cands, 3
        R = ask("evacuation", **req)
        runs[label] = {k: R[k] for k in ("people", "outside", "within", "housed", "warning", "safe_nodes", "bound", "simulation")}
        if "plan" in R:
            runs[label]["plan"] = R["plan"]
            runs[label]["plan"]["chosen_names"] = [cands[c][3] for c in R["plan"]["chosen"]]
        runs[label]["curve"] = R["curve"]
    results["freetown_evacuation"] = runs
    # ---- Miyako: walking out of the assumed tsunami's reach -------------------------------------------------------------------
    print("Miyako")
    jp = json.loads((PRODUCTS / "japan-hazards" / "03202.json").read_text(encoding="utf-8"))
    M = place(39.64, 141.955, 3.2)
    tsunami = [{"return_period": 0, "path": m["file"], "box": m["box"], "width": m["width"], "height": m["height"], "unit": m["unit"]} for m in jp["maps"] if m["peril"] == "tsunami"]
    shelters = []
    for x in jp["sites"]:
        if "tsunami" in x["hazards"]:
            sx, sy = M["F"].local(x["lat"], x["lon"])
            shelters.append([round(sx), round(sy), 0, x["name"]])
    cands_m = [[round(b["x"]), round(b["y"]), round(b["footprint"] * max(0, round(b["height"] / 3.2) - 2)), b["name"] or f"a building of {round(b['height'])} m"]
               for b in M["buildings"] if b["height"] >= 12]
    cands_m = [c for c in cands_m if c[2] > 0][:80]
    print(f"  {len(M['ways'])} ways, {len(M['buildings'])} buildings, {sum(q[2] for q in M['people']):.0f} people, {len(shelters)} tsunami refuges, {len(cands_m)} tall buildings")
    runs = {}
    for label, speed, departure in (("1.0 m/s, as in 2011", 1.0, DEP_2011), ("1.0 m/s, at once", 1.0, None), ("0.62 m/s, as in 2011", 0.62, DEP_2011), ("0.5 m/s, as in 2011", 0.5, DEP_2011)):
        req = dict(ways=walk_ways(M, speed), frame=M["frame"], people=M["people"], shelters=shelters, hazard={"maps": tsunami}, safe="outside", only_in_zone=True,
                   walking={"speed": speed}, warning=1800, horizon=3600, step=15, read=60, frames=300)
        if departure:
            req["departure"] = departure
        if label.startswith("0.5"):
            req["candidates"], req["k"] = cands_m, 3
        R = ask("evacuation", **req)
        runs[label] = {k: R[k] for k in ("people", "outside", "within", "housed", "warning", "safe_nodes", "bound", "simulation")}
        if "plan" in R:
            runs[label]["plan"] = R["plan"]
            runs[label]["plan"]["chosen_names"] = [cands_m[c][3] for c in R["plan"]["chosen"]]
        runs[label]["curve"] = R["curve"]
    results["miyako_evacuation"] = runs
    miyako_criticality(results)
    (HERE / "results.json").write_text(json.dumps(results, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
    print("wrote results.json")


if __name__ == "__main__":
    main()
