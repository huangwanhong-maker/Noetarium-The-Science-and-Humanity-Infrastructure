"""What the census withheld: floor space left blank for confidentiality, bounded by the census itself and estimated by a dasymetric
carriage of each row's withheld floor onto its blank cells."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _scales as s  # noqa: E402


def create(api):
    class Withheld(api.BoardAnalysis):
        id = "withheld"
        title = "What the census withheld"
        description = ("A cell of the census that pertains to two or fewer businesses is left blank, while the row's total is the true total; the "
                       "floor space withheld in a row is shared among its blank cells in a way the census does not say. Each blank cell lies between "
                       "nothing and the row's withheld floor: that is the census's own bound. Within it the floor is shared by a dasymetric carriage "
                       "on prior weights (the block's floor of that use in the nearest year it was stated, else its area's mix), which keeps every "
                       "row's total exactly. The estimate is one reading of what is withheld; the bound is what is known.")
        params = [api.param("case", "City", "melbourne", kind="text"),
                  api.param("year", "Year", 2024, minimum=2002, maximum=2024, step=1),
                  api.param("use", "Use", "homes", kind="text", help="homes, office, retail, hotels, education, health, culture, community, "
                                                                         "workshops, parking, other, unused, renovation, construction, demolition, undeveloped")]

        def run(self, board, params, context):
            case = str(params.get("case") or "melbourne")
            p = s.product(case)
            year = str(int(params.get("year") or p["years"][-1]))
            use = str(params.get("use") or "homes")
            if use not in p["groups"]:
                raise ValueError(f"no use is called {use}: {', '.join(p['groups'])}")
            uses = s.grouped(p, s.withheld(p, case))
            v, lo, hi = uses[use][year]
            y = p["years_of"][year]
            units, parent = p["levels"][0]["units"], p["levels"][0]["parent"]
            cols = {"block": units, "area": parent, f"{use}, estimated (m²)": v, f"{use}, stated (m²)": lo, f"{use}, at most (m²)": hi,
                    "floor withheld in the block (m²)": y["withheld"], "block total (m²)": y["total"]}
            out = [api.table("withheld_blocks", cols, title=f"{use.title()} by block in {year}: stated, estimated, and at most",
                             places={"kind": "cells", "column": "block", "rings": p["levels"][0]["rings"]},
                             show={"kind": "layer", "column": f"{use}, estimated (m²)"})]
            areas = p["levels"][1]["units"]
            agg = {a: [0.0, 0.0, 0.0, 0.0, 0.0] for a in areas}
            for b, a in enumerate(parent):
                if v[b] is None:
                    continue
                agg[a][0] += lo[b]
                agg[a][1] += v[b]
                agg[a][2] += hi[b] - lo[b]
                agg[a][3] += y["withheld"][b] or 0.0
                agg[a][4] += y["total"][b] or 0.0
            out.append(api.table("withheld_areas", {
                "area": areas,
                f"{use}, stated (m²)": [agg[a][0] for a in areas],
                f"{use}, estimated (m²)": [agg[a][1] for a in areas],
                f"{use}: what is withheld could add at most (m²)": [agg[a][2] for a in areas],
                "share of all floor withheld": [agg[a][3] / agg[a][4] if agg[a][4] else None for a in areas]},
                title=f"{use.title()} by area in {year}, and how much of each area's floor the census withheld",
                show={"kind": "chart", "chart": "bar", "x": "area", "y": "share of all floor withheld"}))
            stated = sum(agg[a][0] for a in areas)
            estimated = sum(agg[a][1] for a in areas)
            total = sum(agg[a][4] for a in areas)
            withheld = sum(agg[a][3] for a in areas)
            out.append(api.note(
                f"In {year} the census withheld {s.pct(withheld / total if total else None)} of the floor space it counted. Of {use}, it states "
                f"{stated:,.0f} m²; with its share of what is withheld estimated, {estimated:,.0f} m². A cell withheld lies between nothing and its "
                "row's withheld floor whatever is assumed, so a model fitted to these values carries that spread in each observation's σ."))
            return out
    return Withheld()
