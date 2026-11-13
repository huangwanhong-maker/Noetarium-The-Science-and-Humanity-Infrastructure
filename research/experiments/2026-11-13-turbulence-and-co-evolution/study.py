"""Turbulence, and systems of different kinds evolving together: the numbers of the record, from the Dynamics Lab's engine.

Every system is declared in the Lab's examples (applications/dynamics-lab/examples/13 to 16), and every number here is asked
of the engine through the Lab's server, as the page asks it. Nothing is computed in this script but sums of what was answered.

    python research/experiments/2026-11-13-turbulence-and-co-evolution/study.py

It starts the Lab's server on a port of its own, writes results/*.json beside this file, and prints what the README quotes.
The engine must be built (python applications/dynamics-lab/engine.py build).
"""

from __future__ import annotations

import json
import math
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
EXAMPLES = ROOT / "applications" / "dynamics-lab" / "examples"
PORT = 9097
AT = f"http://127.0.0.1:{PORT}"


def script_of(name: str) -> str:
    """An example's script, without the lines of its studies."""
    return "\n".join(line for line in (EXAMPLES / name).read_text(encoding="utf-8").split("\n") if not line.startswith("#!"))


def ask(request: dict) -> dict:
    body = json.dumps(request).encode("utf-8")
    with urllib.request.urlopen(urllib.request.Request(f"{AT}/api/ask", data=body, headers={"Content-Type": "application/json"}), timeout=300) as r:
        return json.loads(r.read().decode("utf-8"))


def means(answer: dict) -> dict:
    return {n: {"mean": m, "error": e, "second": s} for n, m, e, s in zip(answer["names"], answer["mean"], answer["error"], answer["second"])}


def turbulence() -> dict:
    script = script_of("13-turbulence.dyn")
    start = {"x1": 1, "y2": 0.5, "x3": 0.3}
    out = {}
    for label, nu, step, shells in (("thin", 3e-5, 5e-4, range(3, 12)), ("thick", 1e-4, 2e-4, range(3, 10))):
        a = ask({"op": "averages", "script": script, "system": "sabra", "initial": start, "time": 600, "step": step, "transient": 50, "set": {"nu": nu},
                 "of": [f"e{n}" for n in shells], "against": [2 ** (n - 1) for n in shells]})
        m = means(a)
        below = {through: sum(2 * nu * 4 ** (n - 1) * m[f"e{n}"]["mean"] for n in range(1, through + 1)) for through in (3, 6)}
        out[label] = {"viscosity": nu, "time": a["time"], "samples": a["samples"], "means": m, "scaling": a["scaling"],
                      "flux_plus_dissipated_below": {str(k): m[f"flux{k}"]["mean"] + v for k, v in below.items()}}
    out["exponents"] = ask({"op": "lyapunov", "script": script, "system": "sabra", "initial": start, "time": 100, "step": 5e-4, "transient": 30, "count": 3})
    out["exponents"] = {k: out["exponents"][k] for k in ("exponents", "error", "time")}
    return out


def joined() -> dict:
    script = script_of("14-joined-by-an-action.dyn")
    run = ask({"op": "simulate", "script": script, "system": "joined", "initial": {"a_q": 0.4}, "t1": 70, "step": 0.002, "every": 5})
    b = run["x"][run["names"].index("b_q")]
    # The first time the second pendulum swings widest: the envelope's first maximum, read from the peaks of |b_q|.
    peaks = [(abs(b[k]), run["t"][k]) for k in range(1, len(b) - 1) if abs(b[k]) >= abs(b[k - 1]) and abs(b[k]) >= abs(b[k + 1])]
    widest = max(peaks[: len(peaks) // 2])
    shares = means(ask({"op": "averages", "script": script, "system": "joined", "initial": {"a_q": 0.4}, "time": 68, "step": 0.002, "transient": 0}))
    w1, w2 = math.sqrt(9.81), math.sqrt(9.81 + 2 * 0.6)
    led = ask({"op": "simulate", "script": script, "system": "led", "initial": {"a_q": 0.4}, "t1": 70, "step": 0.002, "every": 5})
    energy = lambda q, v: [vk * vk / 2 - 9.81 * math.cos(qk) for qk, vk in zip(q, v)]
    ea, eb = energy(led["x"][0], led["x"][1]), energy(led["x"][2], led["x"][3])
    return {"widest_swing_of_b": {"size": widest[0], "at": widest[1]}, "half_a_beat_for_small_swings": math.pi / (w2 - w1), "shares": shares,
            "energy_at_the_start": 9.81 * (1 - math.cos(0.4)) + 0.6 / 2 * 0.16,
            "one_way": {"range_of_the_energy_of_a": max(ea) - min(ea), "range_of_the_energy_of_b": max(eb) - min(eb)}}


def sea() -> dict:
    script = script_of("15-a-sea-with-life-in-it.dyn")
    full = {"fluid_x": 1, "fluid_y": 1, "fluid_z": 20, "water_N": 2, "water_P": 0.5, "life_F": 0.3}
    bare = {k: v for k, v in full.items() if k != "life_F"}
    box = {"fluid_x": [6, 9], "fluid_y": [6, 9], "fluid_z": [22, 26], "water_N": [1.5, 3.5], "water_P": [0.35, 0.5], "life_F": [0.5, 1.5]}

    def growth(warming: float) -> float:
        found = ask({"op": "equilibria", "script": script, "system": "sea", "box": box, "seeds": 400, "set": {"warming": warming}})["equilibria"]
        alive = [q for q in found if q["x"][4] > 0.3 and q["x"][5] > 0.1]
        return max(ev[0] for ev in alive[0]["eigenvalues"])

    lo, hi = 6.0, 8.0  # the steady state is stable at 6 and not at 8
    for _ in range(14):
        mid = (lo + hi) / 2
        lo, hi = (mid, hi) if growth(mid) < 0 else (lo, mid)
    out = {"threshold_of_warming": (lo + hi) / 2, "threshold_if_the_fluid_were_alone": (24.74 - 22) / (0.1 / 0.24), "growth_rate_at_6.9": growth(6.9)}
    out["with_fish"] = means(ask({"op": "averages", "script": script, "system": "sea", "initial": full, "time": 3000, "step": 0.005, "transient": 500}))
    out["without_fish"] = means(ask({"op": "averages", "script": script, "system": "barren", "initial": bare, "time": 3000, "step": 0.005, "transient": 500}))
    # The fish die faster (they are fished): what that does to the plankton, the heating and the overturning.
    out["fished"] = {}
    for death in (0.06, 0.08, 0.1, 0.12, 0.14):
        # The parameter of a part is set by a declaration of the whole with that part's value: the script is edited, as a person would.
        edited = script.replace("  part life : fish\n  param warming = 8, stirring = 0.004, appetite = 0.8", f"  part life : fish with death = {death}\n  param warming = 8, stirring = 0.004, appetite = 0.8")
        assert edited != script
        out["fished"][str(death)] = means(ask({"op": "averages", "script": edited, "system": "sea", "initial": full, "time": 2000, "step": 0.005, "transient": 500}))
    return out


def behaviours() -> dict:
    script = script_of("16-behaviours-together.dyn")
    start = {"a_th": 0, "b_th": 2}
    out = {"duet": []}
    for a_listens, b_listens in ((1, 0.2), (0.6, 0.6), (0.2, 1), (0.25, 0.1), (0.2, 0.05), (0.1, 0.1)):
        m = means(ask({"op": "averages", "script": script, "system": "duet", "initial": start, "time": 2000, "step": 0.01, "transient": 200, "set": {"a_listens": a_listens, "b_listens": b_listens}}))
        locks = a_listens + b_listens > 0.3
        out["duet"].append({"a_listens": a_listens, "b_listens": b_listens, "pace_of_a": m["pace_of_a"]["mean"], "pace_of_b": m["pace_of_b"]["mean"],
                            "locks_by_the_closed_form": locks, "pace_by_the_closed_form": (b_listens * 1 + a_listens * 1.3) / (a_listens + b_listens) if locks else None})
    whole = {"a_th": 0, "b_th": 2, "c_v": -1.2, "c_w": -0.62}
    out["trio"] = {}
    for label, setting in (("in_step", {}), ("drifting", {"a_listens": 0.1, "b_listens": 0.1})):
        out["trio"][label] = means(ask({"op": "averages", "script": script, "system": "trio", "initial": whole, "time": 6000, "step": 0.01, "transient": 200, "set": setting}))
    return out


def main() -> int:
    server = subprocess.Popen([sys.executable, str(ROOT / "applications" / "dynamics-lab" / "serve.py"), "--port", str(PORT)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        for _ in range(100):
            try:
                with urllib.request.urlopen(f"{AT}/api/status", timeout=2) as r:
                    status = json.loads(r.read().decode("utf-8"))
                break
            except OSError:
                time.sleep(0.2)
        else:
            raise SystemExit("the Lab's server did not come up")
        if not status.get("current"):
            raise SystemExit("the Lab's engine is not built from the present sources: python applications/dynamics-lab/engine.py build")
        (HERE / "results").mkdir(exist_ok=True)
        for name, study in (("turbulence", turbulence), ("joined", joined), ("sea", sea), ("behaviours", behaviours)):
            t0 = time.time()
            result = study()
            (HERE / "results" / f"{name}.json").write_text(json.dumps(result, indent=1), encoding="utf-8", newline="\n")
            print(f"{name}: {time.time() - t0:.0f} s")
        t = json.loads((HERE / "results" / "turbulence.json").read_text(encoding="utf-8"))
        for label in ("thin", "thick"):
            m, s = t[label]["means"], t[label]["scaling"]
            print(f"  viscosity {t[label]['viscosity']:g}: input {m['input']['mean']:.3f} +- {m['input']['error']:.3f}, dissipation {m['dissipation']['mean']:.3f} +- {m['dissipation']['error']:.3f}, "
                  f"flux3 {m['flux3']['mean']:.3f}, flux6 {m['flux6']['mean']:.3f}; exponent {s['exponent']:.3f} +- {s['error']:.3f}; flatness e4 {m['e4']['second']:.2f}, e9 {m['e9']['second']:.2f}")
        print("  exponents", [round(v, 2) for v in t["exponents"]["exponents"]], "+-", [round(v, 2) for v in t["exponents"]["error"]])
        j = json.loads((HERE / "results" / "joined.json").read_text(encoding="utf-8"))
        print(f"  joined: b swings widest ({j['widest_swing_of_b']['size']:.3f}) at t = {j['widest_swing_of_b']['at']:.2f}; half a beat for small swings {j['half_a_beat_for_small_swings']:.2f}; "
              f"shares {j['shares']['energy_a']['mean']:.3f}, {j['shares']['energy_b']['mean']:.3f}, {j['shares']['in_the_spring']['mean']:.4f} of {j['energy_at_the_start']:.4f}; "
              f"one way: {j['one_way']['range_of_the_energy_of_a']:.1e}, {j['one_way']['range_of_the_energy_of_b']:.3f}")
        s = json.loads((HERE / "results" / "sea.json").read_text(encoding="utf-8"))
        print(f"  sea: threshold {s['threshold_of_warming']:.3f} (fluid alone {s['threshold_if_the_fluid_were_alone']:.3f}); growth at 6.9: {s['growth_rate_at_6.9']:.4f}")
        for label in ("with_fish", "without_fish"):
            m = s[label]
            print(f"    {label}: plankton {m['plankton']['mean']:.4f}, heating {m['heating']['mean']:.2f}, overturning {m['overturning']['mean']:.1f}")
        for death, m in s["fished"].items():
            print(f"    death {death}: plankton {m['plankton']['mean']:.4f}, fish {m['fish']['mean']:.3f}, heating {m['heating']['mean']:.2f}, overturning {m['overturning']['mean']:.1f} +- {m['overturning']['error']:.1f}")
        b = json.loads((HERE / "results" / "behaviours.json").read_text(encoding="utf-8"))
        for d in b["duet"]:
            print(f"    duet {d['a_listens']}, {d['b_listens']}: paces {d['pace_of_a']:.5f}, {d['pace_of_b']:.5f}; closed form {d['pace_by_the_closed_form']}")
        for label, m in b["trio"].items():
            print(f"    trio {label}: paces {m['pace_of_a']['mean']:.5f}, {m['pace_of_b']['mean']:.5f}; answering {m['answering']['mean']:.4f} (second moment {m['answering']['second']:.2f}); 1.25 + 0.05 answering = {1.25 + 0.05 * m['answering']['mean']:.5f}")
    finally:
        server.kill()
    return 0


if __name__ == "__main__":
    sys.exit(main())
