"""Which roads matter most to reaching care: a place's road links ranked by what its floods cost through each (its risk: what protecting
it would save) and by what losing each would add (its criticality), in person-minutes added to one trip to the nearest building mapped
as care by everyone, expected over the year's largest flood."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _risk as r  # noqa: E402
from science import risk  # noqa: E402


def create(api):
    class Roads(api.BoardAnalysis):
        id = "roads"
        title = "Which roads matter most to reaching care"
        description = ("A city's road links are the components of a system whose service is reaching care. A flood map closes or slows each link by "
                       "the deepest water on it (cars stop in 30 cm, Pregnolato et al. 2017); the maps at their return periods are the scenarios, each "
                       "standing for the floods of its band and nothing counted below the first. Each link's risk is what the floods cost through "
                       "it, what protecting it would save; its criticality what losing it would add, in dry years and in floods alike. The loss is "
                       "the person-minutes added to one trip to the nearest building mapped as care by everyone, a person cut off counted at the cap, "
                       "expected over the year's largest flood: how long a flood stands is not in the maps, so this is no total over a year. People are "
                       "GHS-POP's, shared among buildings by floor space; ways and buildings are Overture's.")
        params = [api.param("case", "Place", "freetown", kind="text", help=r.CASES),
                  api.param("km", "How far about it (km)", 3.0, minimum=0.5, maximum=6, step=0.5),
                  api.param("threshold", "Beyond reach after (minutes)", 30, minimum=5, maximum=120, step=5),
                  api.param("cap", "A person cut off counts as (minutes)", 120, minimum=30, maximum=600, step=10),
                  api.param("lat", "Latitude (blank: the place's centre)", "", kind="text"),
                  api.param("lon", "Longitude (blank: the place's centre)", "", kind="text")]

        def run(self, board, params, context):
            H, lat, lon, km = r.where(params)
            P = risk.place(lat, lon, km)
            care = risk.care(P)
            if not care:
                raise ValueError("no building about the place is mapped as care: nothing to reach")
            a = risk.ask("criticality", ways=risk.drive_ways(P), frame=P["frame"], people=P["people"], services=care,
                         measure={"threshold": float(params.get("threshold") or 30) * 60, "cap": float(params.get("cap") or 120) * 60},
                         hazard={"maps": H["maps"]}, curve="vehicles", top=10)
            la, lo, length = r.midpoints(P, a["links"])
            keep = [k for k in range(len(a["links"])) if a["risk"][k] > 1e-9 or a["criticality"][k] > 1e-9]
            keep.sort(key=lambda k: (-a["risk"][k], -a["criticality"][k]))
            keep = keep[:3000]
            depth = a["depth"] or [0.0] * len(a["links"])
            out = [api.table("roads", {
                "link": keep, "lat": [la[k] for k in keep], "lon": [lo[k] for k in keep], "length_m": [length[k] for k in keep],
                "risk": [a["risk"][k] for k in keep], "criticality": [a["criticality"][k] for k in keep],
                "deepest_water_m": [depth[k] for k in keep]},
                title="Road links by what the floods cost through them (risk) and what losing them would add (criticality), person-minutes a trip",
                roles={"lat": "lat", "lon": "lon", "amount": "risk"}, show={"kind": "layer", "layer": "points", "colour": "risk", "size": "criticality", "map": "orchid"})]
            S = a["scenarios"]
            out.append(api.table("roads_scenarios", {
                "scenario": [s["name"] for s in S], "weight": [s["weight"] for s in S], "links_closed": [s["closed"] for s in S],
                "links_slowed": [s["slowed"] for s in S], "people_cut_off": [s["isolated"] for s in S],
                "people_beyond_reach": [s["beyond"] for s in S], "mean_minutes_to_care": [s["mean_minutes"] for s in S]},
                title="What each flood does to the roads and to reaching care", show={"kind": "chart", "chart": "bar", "x": "scenario", "y": "people_cut_off", "how": "sum"}))
            top = a["top_risk"][0] if a["top_risk"] else None
            crit = a["top_criticality"][0] if a["top_criticality"] else None
            worst = max(S, key=lambda s: s["isolated"])
            text = (f"About the place ({km:g} km): {r.n0(P['total'])} people, {len(care)} buildings mapped as care, {len(a['links']):,} road links. "
                    f"The worst flood mapped closes {worst['closed']} links and cuts {r.n0(worst['isolated'])} people off from care. "
                    + (f"Expected over the year's largest flood, the floods add {r.n0(a['expected'])} person-minutes to a trip to care by everyone." if any(m["return_period"] > 1 for m in H["maps"])
                     else f"Under the map's flood, which has no return period, it adds {r.n0(a['expected'])} person-minutes to a trip to care by everyone."))
            if top and top["fussell_vesely"] >= 0.005:
                text += f" Protecting the link of greatest risk ({top['length']:.0f} m) would remove {100 * top['fussell_vesely']:.1f}% of that; links of similar risk are often alternative ways out of one district, and their shares do not add."
            if crit and crit["raw"] >= 1.05:
                text += f" Losing the most critical link ({crit['length']:.0f} m) would multiply the expected loss by {crit['raw']:.1f}."
            out.append(api.note(text + f" {H['credit']}"))
            return out

    return Roads()
