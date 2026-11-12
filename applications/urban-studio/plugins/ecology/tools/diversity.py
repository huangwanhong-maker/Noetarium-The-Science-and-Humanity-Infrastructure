"""The diversity of the forest: how many kinds, how evenly held, what a fuller count would find, and how distinct its areas are."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Diversity(api.BoardAnalysis):
        id = "diversity"
        title = "Diversity, and what a fuller count would find"
        description = ("The effective numbers of kinds at a rank (Hill numbers of orders 0, 1 and 2: richness, the exponential of Shannon's "
                       "entropy, the inverse of Simpson's concentration), as counted and as estimated for the whole the count samples; how "
                       "complete the count is (its coverage); each area at one common coverage, so that areas counted more are not favoured; "
                       "the curve of what more counting would find; and how many distinct communities the areas amount to.")
        params = [q for q in f.column_params(api) if q["name"] in ("table", "species", "genus", "family", "area")] + [
            api.param("rank", "Rank", "species", kind="choice", options=["species", "genus", "family"]),
            api.param("least", "Least trees in an area", 50, kind="integer", minimum=5, maximum=10000,
                      help="areas with fewer are shown at their own coverage only, and do not set the common one")]

        def run(self, board, params, context):
            trees, areas, t, info = f.read_trees(board, params)
            a = f.ecology.ask("diversity", trees=trees, areas=areas, rank=params["rank"], least=int(params["least"]), curve={"replicates": 30})
            rank = params["rank"]
            rows = {"area": [], "trees": [], "kinds": [], "shannon": [], "simpson": [], "kinds_estimated": [], "coverage": [],
                    "kinds_at_common": [], "shannon_at_common": [], "simpson_at_common": []}
            for x in a["areas"]:
                rows["area"].append(x["area"])
                rows["trees"].append(x["individuals"])
                rows["kinds"].append(x["observed"][0])
                rows["shannon"].append(round(x["observed"][1], 2))
                rows["simpson"].append(round(x["observed"][2], 2))
                rows["kinds_estimated"].append(round(x["estimated"][0], 1))
                rows["coverage"].append(round(x["coverage"], 4))
                c = x.get("at_coverage")
                rows["kinds_at_common"].append(round(c[0], 1) if c else None)
                rows["shannon_at_common"].append(round(c[1], 2) if c else None)
                rows["simpson_at_common"].append(round(c[2], 2) if c else None)
            cv = a.get("curve") or {}
            curve = {"trees_counted": cv.get("size", []), "order": cv.get("order", []), "estimate": [round(x, 2) for x in cv.get("estimate", [])],
                     "lower": [round(x, 2) for x in cv.get("lower", [])], "upper": [round(x, 2) for x in cv.get("upper", [])], "how": cv.get("method", [])}
            ob, es, rich = a["observed"], a["estimated"], a["richness"]
            parts = {p["q"]: p for p in a.get("partition", [])}
            p1 = parts.get(1.0)
            out = [
                api.table("diversity_by_area", rows, title=f"Each area's diversity of {rank}, as counted and at one common coverage",
                          roles={"name": "area", "amount": "kinds_at_common"}, show={"kind": "chart", "chart": "bar", "x": "area", "y": "shannon_at_common", "how": "sum"}),
                api.note(f"{f.count(a['individuals'])} trees named at the {rank}: {ob[0]:.0f} kinds counted, an effective {ob[1]:.1f} by Shannon and "
                         f"{ob[2]:.1f} by Simpson. A fuller count would find about {es[0]:.0f} kinds (Chao1, {rich['lower']:.0f} to {rich['upper']:.0f}); "
                         f"the count already covers {f.share(a['coverage'])} of the trees of the whole it samples."
                         + (f" Compared at a common coverage of {f.share(a['common_coverage'])}." if a.get("common_coverage") else "")
                         + (f" The {p1['units']:.0f} areas amount to {p1['beta']:.2f} distinct communities of order 1 (β), sharing "
                            f"{f.share(p1['sorensen'])} of their kinds by Horn's overlap." if p1 else "")),
            ]
            if curve["estimate"]:
                out.insert(1, api.table("diversity_curve", curve, title=f"What counting more would find: kinds of {rank} against trees counted, with a bootstrap band",
                                        roles={"category": "order", "amount": "estimate"},
                                        show={"kind": "chart", "chart": "line", "x": "trees_counted", "y": "estimate", "split": "order", "how": "sum"}))
            return out
    return Diversity()
