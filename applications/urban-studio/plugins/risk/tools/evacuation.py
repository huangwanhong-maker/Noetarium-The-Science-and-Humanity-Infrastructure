"""Walking out of a hazard's reach: the people within a zone (an assumed tsunami, a 1-in-100-year flood) walk to the designated
refuges and the dry ground; who can get out within the warning, the most any routing could bring to safety by every minute, and the
crowd as it moves; and the tall buildings that would serve best as refuges."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _risk as r  # noqa: E402
from science import risk  # noqa: E402


def create(api):
    class Evacuation(api.BoardAnalysis):
        id = "evacuation"
        title = "Walking out of the hazard's reach"
        description = ("The people within a hazard's zone walk on the ways to the designated refuges (no capacity being published, none is "
                       "assumed) and to the ground the zone does not reach. Three curves: the most any routing could bring to safety by each minute "
                       "if everyone left at once, the same with the people's departures (a bound by flows over time: no evacuation of these people "
                       "can do better), and the crowd simulated as kinematic waves, routed to the nearest refuge with room. The gap between the bounds "
                       "is what late departure costs; between the second bound and the crowd, what routing and crowding cost. Declared: walking "
                       "speed (1.0 m/s, the FDMA's planning value; 0.62 m/s, MLIT's mean of 2011; 0.5 m/s for those who walk with difficulty), 1.3 people "
                       "a second per metre of width, 5.4 per m² at a standstill, the departures, and a tall building's refuge (its floors from the third "
                       "up, a person a square metre).")
        params = [api.param("case", "Place", "03202", kind="text", help=r.CASES),
                  api.param("km", "How far about it (km)", 3.0, minimum=0.5, maximum=6, step=0.5),
                  api.param("speed", "Walking (m/s)", 1.0, minimum=0.3, maximum=1.6, step=0.02),
                  api.param("warning", "Warning (minutes)", 30, minimum=5, maximum=90, step=5),
                  api.param("departure", "Leaving", "survey", kind="text", help="survey (as in 2011) or once (everyone at once)"),
                  api.param("refuges", "Tall buildings to choose as refuges", 3, minimum=0, maximum=10, step=1),
                  api.param("lat", "Latitude (blank: the place's centre)", "", kind="text"),
                  api.param("lon", "Longitude (blank: the place's centre)", "", kind="text")]

        def run(self, board, params, context):
            H, lat, lon, km = r.where(params)
            P = risk.place(lat, lon, km)
            speed = float(params.get("speed") or 1.0)
            warning = float(params.get("warning") or 30) * 60
            dep = r.AT_ONCE if str(params.get("departure") or "survey").strip() == "once" else r.SURVEY
            shelters = []
            for s in H["refuges"]:
                x, y = P["F"].local(s["lat"], s["lon"])
                shelters.append([round(x), round(y), 0, s["name"]])
            k = int(params.get("refuges") or 0)
            cands = risk.tall(P) if k > 0 else []
            req = dict(ways=risk.walk_ways(P, speed), frame=P["frame"], people=P["people"], shelters=shelters, hazard={"maps": [H["zone"]]},
                       safe="outside", only_in_zone=True, walking={"speed": speed}, departure=dep, warning=warning,
                       horizon=max(3600.0, 2 * warning), step=15, read=60, frames=20)
            if cands:
                req["candidates"], req["k"] = cands, k
            a = risk.ask("evacuation", **req)
            c = a["curve"]
            out = [api.table("evacuation_curve", {
                "minutes": [t / 60 for t in c["t"]], "departed": c["departed"], "bound_at_once": c["bound_at_once"], "bound": c["bound"], "simulated": c["simulated"]},
                title="People safe by each minute: the bounds and the simulated crowd", show={"kind": "chart", "chart": "line", "x": "minutes", "y": "simulated", "how": "sum"})]
            late = a["late"]
            if late:
                geo = [P["F"].geo(p[0], p[1]) for p in late]
                out.append(api.table("evacuation_late", {"lat": [round(g[0], 6) for g in geo], "lon": [round(g[1], 6) for g in geo],
                                                         "people": [p[2] for p in late], "minutes_walking": [None if p[3] is None else p[3] / 60 for p in late]},
                                     title=f"Where people cannot walk to safety within {warning / 60:.0f} minutes", roles={"lat": "lat", "lon": "lon", "amount": "people"},
                                     show={"kind": "layer", "layer": "points", "colour": "people", "size": "people", "map": "orchid"}))
            if a["shelters"]:
                geo = [P["F"].geo(s["x"], s["y"]) for s in a["shelters"]]
                out.append(api.table("evacuation_refuges", {"refuge": [s["name"] for s in a["shelters"]], "lat": [round(g[0], 6) for g in geo], "lon": [round(g[1], 6) for g in geo]},
                                     title="The designated refuges", roles={"lat": "lat", "lon": "lon", "name": "refuge"}, show={"kind": "layer", "layer": "points"}))
            B, S = a["bound"], a["simulation"]
            text = (f"{r.n0(a['people'])} people live within {H['zone_name']} about the place ({r.n0(a['outside'])} outside it). Walking freely at {speed:g} m/s, "
                    f"{r.n0(a['within'])} can reach a refuge or dry ground within {warning / 60:.0f} minutes. However they are routed, at most "
                    f"{r.n0(min(a['people'], B['by_warning_at_once']))} could be safe by then leaving at once, and {r.n0(min(a['people'], B['by_warning']))} leaving as declared "
                    f"({dep['source']}); the simulated crowd brings {r.n0(S['by_warning'])}. ")
            horizon = req["horizon"] / 60
            cleared = round(100 * S["cleared"])
            text += (f"{cleared}% are safe after {r.minutes(S['clearance'])}" if S["clearance"] is not None else f"{cleared}% are not safe within the {horizon:.0f} minutes simulated")
            text += (f"; the bound brings everyone by {r.minutes(B['clearance'])}." if B["clearance"] is not None else f"; even the bound leaves some short of safety after {horizon:.0f} minutes.")
            if "plan" in a:
                pl = a["plan"]
                if pl["chosen"]:
                    names = [cands[i][3] for i in pl["chosen"]]
                    text += f" Of {len(cands)} tall buildings, {', '.join(names)} as refuges would bring {r.n0(pl['value'][-1] - pl['existing'])} more within the warning (no {len(names)} could do more than {r.n0(pl['bound'] - pl['existing'])})."
                else:
                    text += f" None of the {len(cands)} buildings mapped at 12 m or more would bring anyone to safety within the warning who could not already reach a refuge or dry ground."
            out.append(api.note(text + " An illustration under the declarations above, not an official evacuation plan." + f" {H['credit']}"))
            return out

    return Evacuation()
