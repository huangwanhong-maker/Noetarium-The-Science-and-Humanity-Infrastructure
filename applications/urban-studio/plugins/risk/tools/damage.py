"""What a city's floods would cost its buildings, read through every admissible damage function at once: the buildings as the place's
flood assessment valued them, their depth in each map of a return period, and the JRC's global depth–damage curves for the regions
declared to resemble the place, each region a model applied to every building of a class at once. The answer is a range, never one
number; a mean over the models is not taken, since no weight is declared."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _risk as r  # noqa: E402
from science import risk  # noqa: E402


def create(api):
    class Damage(api.BoardAnalysis):
        id = "damage"
        title = "What the floods would cost the buildings, under every damage function"
        description = ("The place's buildings, valued as its flood assessment valued them, meet each map of a return period at the depth the map "
                       "gives them. A damage function turns a depth into the share of a building's value lost; which one holds is not known, so "
                       "every model of the JRC's global library (Huizinga, de Moel and Szewczyk 2017) for the regions declared below is applied, "
                       "each to every building of a class at once, and the losses are given as their range over the models, with the wider "
                       "envelope that would let each building take its own worst or best curve beside it. A model with a curve moved by one "
                       "standard deviation of the library's spread shows the uncertainty within a region. The average annual loss is the area "
                       "under the losses against 1/T; a map gives every place its T-year depth at once, so this reads the hazard more severely "
                       "than an event set would.")
        params = [api.param("case", "Place", "freetown", kind="text", help="a place whose buildings and flood maps the data library holds: freetown"),
                  api.param("regions", "Regions whose curves are admitted", "Africa, Asia, Central and South America", kind="text",
                            help="the JRC's regions: Europe, North America, Central and South America, Asia, Africa, Oceania; a class a region lacks takes the global curve"),
                  api.param("spread", "Also each region's curves moved by one standard deviation", True, kind="boolean"),
                  api.param("cell", "Cells of the map (degrees)", 0.0025, minimum=0.0005, maximum=0.02, step=0.0005)]

        def run(self, board, params, context):
            case = str(params.get("case") or "freetown").strip()
            if case != "freetown":
                raise ValueError("the data library holds valued buildings and flood maps for freetown only")
            regions = tuple(x.strip() for x in str(params.get("regions") or "Africa").split(",") if x.strip())
            spread = params.get("spread") not in (False, "false", "0", 0)
            cell = float(params.get("cell") or 0.0025)
            X = risk.freetown_exposure()
            H = risk.hazard_maps(case)
            L = risk.jrc_library(regions=regions, spread=spread)
            T = [m["return_period"] for m in H["maps"]]
            a = risk.ask("vulnerability.assess", library=L, assets={k: X[k] for k in ("file", "fields", "classes")}, hazard={"maps": H["maps"]},
                         return_periods=T, cell=cell)
            M = a["models"]
            at = lambda m, t: next(q["loss"] for q in m["at"] if q["return_period"] == t)  # noqa: E731
            out = [api.table("damage_models", {
                "model": [m["model"] for m in M], "aal_usd": [round(m["aal"]) for m in M],
                "residential_aal_usd": [round(m["by_class"].get("residential", 0)) for m in M],
                "commercial_aal_usd": [round(m["by_class"].get("commercial", 0)) for m in M],
                "industrial_aal_usd": [round(m["by_class"].get("industrial", 0)) for m in M],
                **{f"loss_1_in_{t}_usd": [round(at(m, t)) for m in M] for t in T}},
                title="Each model's losses: a year on average and in each map's flood (USD; the buildings' values the assessment's)",
                show={"kind": "chart", "chart": "bar", "x": "model", "y": "aal_usd", "how": "sum"})]
            cells = a.get("cells", [])
            out.append(api.table("damage_cells", {
                "lat": [round(c["lat"], 6) for c in cells], "lon": [round(c["lon"], 6) for c in cells],
                "value_usd": [round(c["value"]) for c in cells],
                "aal_least_usd": [round(min(c["aal"])) for c in cells], "aal_greatest_usd": [round(max(c["aal"])) for c in cells]},
                title=f"Where the losses fall: each cell's average annual loss, least and greatest over the {len(M)} models",
                roles={"lat": "lat", "lon": "lon", "amount": "aal_greatest_usd"},
                show={"kind": "layer", "layer": "points", "colour": "aal_greatest_usd", "size": "value_usd", "map": "orchid"}))
            reported = (X.get("reported_aal") or {}).get("all")
            m = lambda v: f"${v / 1e6:.2f} m"  # noqa: E731
            wettest = max(a["events"], key=lambda e: e["return_period"])
            note = (f"Of {r.n0(a['assets'])} buildings worth {m(a['value'])}, the 1-in-{round(wettest['return_period'])}-year river flood reaches {r.n0(wettest['reached'])}. "
                    f"Over {len(M)} models the average annual loss lies between {m(a['aal_least'])} and {m(a['aal_greatest'])}, a factor of "
                    f"{a['aal_greatest'] / a['aal_least']:.1f}; were each building free to take its own curve, between {m(a['envelope_low'])} and {m(a['envelope_high'])}. "
                    f"The 1-in-100-year flood costs between {m(a['least_at'][T.index(100)])} and {m(a['greatest_at'][T.index(100)])}. ")
            if reported:
                note += (f"The assessment reports {m(reported)} a year from its own event set and damage functions; the maps' reading gives every place its "
                         f"T-year depth at once, and so is no like-for-like comparison. ")
            note += f"No region's curves are the place's own: which resemble it is declared above. {X['credit']}; {risk.damage_functions()['credit']}."
            out.append(api.note(note))
            return out

    return Damage()
