"""Taxonomic distinctness: how far apart in the tree of life an area's species are, against what random draws would give."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Distinctness(api.BoardAnalysis):
        id = "distinctness"
        title = "How far apart its kinds are"
        description = ("Each area's average taxonomic distinctness Δ⁺ (Clarke and Warwick): the mean distance between its species through genus "
                       "and family, which does not grow with how many were counted, against the funnel of what the same number of species "
                       "drawn at random from the whole forest would give. An area below the funnel holds closely related trees: one pest "
                       "of a genus or a family would take more of it.")
        params = [q for q in f.column_params(api) if q["name"] in ("table", "species", "genus", "family", "area")] + [
            api.param("draws", "Random draws", 500, kind="integer", minimum=100, maximum=5000),
            api.param("least", "Least species in an area", 5, kind="integer", minimum=2, maximum=200)]

        def run(self, board, params, context):
            trees, areas, t, info = f.read_trees(board, params)
            a = f.ecology.ask("distinctness", trees=trees, areas=areas, draws=int(params["draws"]))
            rows = {"area": [], "species": [], "distinctness": [], "expected": [], "lower": [], "upper": [], "below_funnel": [], "variation": []}
            for x in a["areas"]:
                if x["species"] < int(params["least"]):
                    continue
                rows["area"].append(x["area"])
                rows["species"].append(x["species"])
                rows["distinctness"].append(round(x["average"], 2))
                rows["expected"].append(round(x["expected"], 2))
                rows["lower"].append(round(x["lower"], 2))
                rows["upper"].append(round(x["upper"], 2))
                rows["below_funnel"].append("below" if x["below"] else "")
                rows["variation"].append(round(x["variation"], 1))
            below = [n for n, b in zip(rows["area"], rows["below_funnel"]) if b]
            return [api.table("distinctness", rows, title="Each area's taxonomic distinctness against the funnel of random draws",
                              roles={"name": "area", "amount": "distinctness"}, show={"kind": "chart", "chart": "scatter", "x": "species", "y": "distinctness"}),
                    api.note(f"The whole forest's {a['species']:.0f} species are {a['average']:.1f} apart on average (100: different families; 66.7: one family; "
                             f"33.3: one genus). " + (f"Below the funnel, more closely related than chance: {', '.join(below)}." if below
                                                       else "No area lies below the funnel.")
                             + f" The funnel is {params['draws']} random draws for each number of species.")]
    return Distinctness()
