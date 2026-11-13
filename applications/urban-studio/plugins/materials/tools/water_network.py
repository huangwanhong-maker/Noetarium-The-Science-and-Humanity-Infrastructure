"""A water network: an EPANET input file solved at an instant, through a day, or with each pipe lost in turn; what each junction receives,
at what pressure, how old its water is, and which pipes the network cannot lose."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402
from science import water  # noqa: E402


def create(api):
    class WaterNetwork(api.BoardAnalysis):
        id = "water_network"
        title = "A water network"
        description = ("A water distribution network as EPANET declares it (an .inp file): the heads, pressures and flows at an instant by the global "
                       "gradient algorithm, as EPANET solves them; what each junction receives under demand-driven or pressure-driven analysis "
                       "(Wagner's law), how old its water is, and Todini's resilience index; through the file's period, the pressures hour by hour "
                       "and the tanks' levels; or each pipe closed in turn, the share of the demand delivered and lost.")
        params = [api.param("network", "Network (.inp)", "data:epanet-examples/all/Net1.inp", kind="text", role="file", help="an EPANET input file"),
                  api.param("analysis", "Analysis", "an instant", kind="choice", options=["an instant", "the day", "each pipe lost"]),
                  api.param("hour", "Hour (for an instant)", 0.0, kind="number", minimum=0),
                  api.param("pressure_driven", "Demands", "pressure-driven", kind="choice", options=["pressure-driven", "demand-driven"]),
                  api.param("required", "Required pressure (m)", 20.0, kind="number", minimum=0, help="the pressure at which a junction draws all it asks (declared)"),
                  api.param("minimum", "Minimum pressure (m)", 0.0, kind="number", minimum=0, help="the pressure below which it draws nothing (declared)")]

        def run(self, board, params, context):
            inp = Path(api.file(params["network"])).read_text(encoding="utf-8", errors="replace")
            pdd = params["pressure_driven"] == "pressure-driven"
            req = {"inp": inp, "pressure_driven": pdd, "required_pressure": float(params["required"]), "minimum_pressure": float(params["minimum"])}
            if params["analysis"] == "each pipe lost":
                a = water.ask("water_criticality", **req)
                # The chart draws the share lost, so that its bars, largest first, are the pipes the network cannot lose.
                rows = {"pipe": a["links"], "delivered": a["delivered"], "lost": [round(1 - d, 6) for d in a["delivered"]], "junctions_below": a["below"]}
                worst = [(p, d) for p, d in zip(a["links"], a["delivered"]) if d < 0.999]
                return [api.table("pipes_lost", rows, title="Each pipe closed in turn: the share of the demand lost", roles={"name": "pipe", "amount": "lost"},
                                  show={"kind": "chart", "chart": "bar", "x": "pipe", "y": "lost", "how": "sum"}),
                        api.note(f"Of {len(a['links'])} pipes, {len(worst)} cannot be lost without some junction drawing less than it asks at "
                                 f"{params['required']:g} m"
                                 + (f"; the worst is {worst[0][0]}, leaving {100 * worst[0][1]:.1f}% of the demand delivered." if worst else ".")
                                 + " Each closure is solved under pressure-driven demand (Wagner's law), the network otherwise as the file has it at its first instant."),
                        api.note(f"Declared: the required pressure {params['required']:g} m and the minimum {params['minimum']:g} m; tanks hold their initial levels.")]
            if params["analysis"] == "the day":
                a = water.ask("water_simulate", **req)
                hours = [t / 3600 for t in a["times"]]
                long = {"hour": [], "node": [], "pressure_m": []}
                junctions = [k for k, kind in enumerate(a["kind"]) if kind == "junction"]
                low = 1e300
                for k, h in enumerate(hours):
                    for j in junctions:
                        p = a["pressure"][k][j]
                        if p is None:
                            continue
                        long["hour"].append(h), long["node"].append(a["nodes"][j]), long["pressure_m"].append(round(p, 3))
                        low = min(low, p)
                tanks = {"hour": [], "tank": [], "level_m": []}
                for tank, levels in a["tank_level"].items():
                    for h, lv in zip(hours, levels):
                        tanks["hour"].append(h), tanks["tank"].append(tank), tanks["level_m"].append(round(lv, 3))
                short = sum(1 for d, q in zip(a["delivered"], a["asked"]) if q > 0 and d < 0.999 * q)
                return [api.table("pressures_by_hour", long, title="Pressure at each node, hour by hour (m)", roles={"time": "hour", "category": "node", "amount": "pressure_m"},
                                  show={"kind": "chart", "chart": "line", "x": "hour", "y": "pressure_m", "split": "node", "how": "mean"}),
                        api.table("tank_levels", tanks, title="Tanks' levels (m)", roles={"time": "hour", "category": "tank", "amount": "level_m"},
                                  show={"kind": "chart", "chart": "line", "x": "hour", "y": "level_m", "split": "tank", "how": "mean"}),
                        api.note(f"Over {hours[-1]:g} hours at a step of {a['step'] / 3600:g} h, the lowest pressure anywhere is {low:.1f} m; "
                                 f"in {short} of {len(hours)} hours some demand goes undelivered."),
                        api.note("Departs from EPANET where a tank fills or empties, or a control's condition is met, within a step: EPANET stops there, "
                                 "this waits for the step's end.")]
            a = water.ask("water_solve", **req, time=float(params["hour"]) * 3600)
            J = sum(1 for k in a["kind"] if k == "junction")
            nodes = {"node": a["nodes"], "kind": a["kind"], "pressure_m": a["pressure"], "head_m": a["head"], "delivered_l_s": [None if d is None else round(1000 * d, 3) for d in a["demand"]],
                     "asked_l_s": [round(1000 * d, 3) for d in a["required"]], "age_h": [None if x is None else round(x / 3600, 2) for x in a["age"]]}
            links = {"link": a["links"], "kind": a["link_kind"], "flow_l_s": [round(1000 * q, 3) for q in a["flow"]], "velocity_m_s": [round(v, 3) for v in a["velocity"]],
                     "headloss_m": [round(h, 3) for h in a["headloss"]], "open": ["open" if o else "closed" for o in a["open"]]}
            pj = [p for p, k in zip(a["pressure"], a["kind"]) if k == "junction" and p is not None]
            ages = [x for x, k in zip(a["age"], a["kind"]) if k == "junction" and x is not None]
            asked, got = sum(a["required"][:J]), sum(d or 0 for d in a["demand"][:J])
            return [api.table("junctions", nodes, title=f"Each node at hour {params['hour']:g}", roles={"name": "node", "category": "kind", "amount": "pressure_m"},
                              show={"kind": "chart", "chart": "bar", "x": "node", "y": "pressure_m", "how": "mean"}),
                    api.table("links", links, title=f"Each link at hour {params['hour']:g}", roles={"name": "link", "amount": "flow_l_s"}),
                    api.note(f"{a['title'] or 'The network'}: {J} junctions draw {1000 * got:,.1f} of the {1000 * asked:,.1f} L/s they ask, at pressures from "
                             f"{min(pj):.1f} to {max(pj):.1f} m; their water is {min(ages) / 3600:.1f} to {max(ages) / 3600:.1f} h old in a steady state. "
                             f"Todini's resilience index at {params['required']:g} m is {a['resilience']:.2f}"
                             + (f"; {', '.join(a['isolated'])} cannot be reached." if a["isolated"] else ".")),
                    api.note("Solved as EPANET solves it (Hazen–Williams or Darcy–Weisbach, pumps' curves, check valves), and held to EPANET 2.2 on its "
                             "example network to a millimetre of head. The ages take tanks' water as fresh, a declared presumption.")]
    return WaterNetwork()
