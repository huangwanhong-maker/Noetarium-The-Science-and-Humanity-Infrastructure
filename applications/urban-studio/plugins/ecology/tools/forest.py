"""The forest: what it is made of, against declared limits, and the carbon it holds."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Forest(api.BoardAnalysis):
        id = "forest"
        title = "What the forest is made of"
        description = ("The trees by species, genus and family, overall and area by area, found against declared limits of composition rank by "
                       "rank (Santamour's 10-20-30 as one scheme); their sizes; and the carbon they hold by a declared allometry, each tree "
                       "saying which rank of the scheme matched it. The composition is counted; the carbon is presumed, and says so.")
        params = f.column_params(api) + f.life_params(api)[:1] + f.allometry_params(api) + [
            api.param("limits", "Limits", f.scheme_files("limits")[0] if f.scheme_files("limits") else "", kind="choice",
                      options=f.scheme_files("limits") or [""], help="the shares no species, genus or family should exceed"),
            api.param("top", "Names shown", 15, kind="integer", minimum=3, maximum=60)]

        def run(self, board, params, context):
            context.progress(0.1, "reading the trees")
            trees, areas, t, info = f.read_trees(board, params, life=True)
            alloc, limits = f.allometry(params), f.scheme(params["limits"])
            context.progress(0.3, "asking the ecology domain")
            a = f.ecology.ask("forest", trees=trees, areas=areas, scheme=alloc, limits=limits, top=int(params["top"]))
            o = a["overall"]
            rows = {"rank": [], "name": [], "trees": [], "share": [], "above_limit": []}
            for rank in ("species", "genus", "family"):
                tal = o[rank]
                lim = limits[rank]
                for n, c in zip(tal["names"], tal["counts"]):
                    rows["rank"].append(rank)
                    rows["name"].append(n)
                    rows["trees"].append(c)
                    rows["share"].append(round(c / tal["known"], 4) if tal["known"] else None)
                    rows["above_limit"].append("above" if tal["known"] and c / tal["known"] > lim else "")
            by = {"area": [], "trees": [], "largest_species": [], "species_share": [], "largest_genus": [], "genus_share": [], "largest_family": [],
                  "family_share": [], "ranks_failing": [], "carbon_t": [], "measured": []}
            for x in a["areas"]:
                fs = {g["rank"]: g for g in x["findings"]}
                by["area"].append(x["area"])
                by["trees"].append(x["trees"])
                for rank in ("species", "genus", "family"):
                    by[f"largest_{rank}"].append(fs[rank]["largest"])
                    by[f"{rank}_share"].append(round(fs[rank]["share"], 4))
                by["ranks_failing"].append(sum(0 if g["holds"] else 1 for g in x["findings"]))
                by["carbon_t"].append(round(x["stock"]["carbon_t"], 1))
                by["measured"].append(x["stock"]["estimated"])
            st = o["stock"]
            m = st["matched"]
            bounds = a["bounds"]
            size = {"diameter_class": [f"{int(bounds[i])}–{int(bounds[i + 1])} cm" if i + 1 < len(bounds) else f"≥ {int(bounds[i])} cm" for i in range(len(bounds))],
                    "trees": o["size"]}
            find = []
            for g in o["findings"]:
                verdict = "holds" if g["holds"] else "fails"
                over = ", ".join(f"{x['name']} {f.share(x['share'])}" for x in g["over"][:4]) + (" …" if len(g["over"]) > 4 else "")
                find.append(f"{g['rank'].capitalize()}: {verdict} at {f.share(g['limit'])}; the largest is {g['largest']} at {f.share(g['share'])}"
                            + (f" (above it: {over})" if not g["holds"] else "") + (f"; {f.count(g['unnamed'])} trees unnamed at this rank" if g["unnamed"] else "") + ".")
            notes = [
                api.note(f"{f.count(t.rows)} trees in {board.view.get('slice') or 'the whole'}, of {o['species']['kinds']:.0f} species, {o['genus']['kinds']:.0f} genera "
                         f"and {o['family']['kinds']:.0f} families. Against {limits['name']}:\n" + "\n".join(find)),
                api.note(f"Carbon held by the {f.count(st['estimated'])} trees with a diameter: {st['carbon_t']:,.0f} t of carbon ({st['co2e_t']:,.0f} t CO₂), "
                         f"adding about {st['sequestration_t']:,.0f} t a year as they grow by the increments the inventory tells. "
                         f"Presumed, not observed: the allometry is {alloc['name']}; it matched {f.count(m['species'])} trees at the species, "
                         f"{f.count(m['genus'])} at the genus, {f.count(m['family'])} at the family and {f.count(m['fallback'])} by its fallback; "
                         f"{f.count(st['outside_range'])} lie outside the diameters its source measured."
                         + f" Beyond those diameters the equations were {'capped' if alloc['beyond_range'] == 'cap' else 'extrapolated'}."
                         + (f" {f.count(t.rows - st['trees'])} trees have no diameter and hold no estimate." if t.rows > st["trees"] else "") + f.aside_words(info)),
            ]
            return [
                api.table("composition", rows, title="The forest by rank, its most common names", roles={"category": "rank", "name": "name", "amount": "trees"},
                          show={"kind": "chart", "chart": "bar", "x": "name", "y": "trees", "how": "sum"}),
                api.table("areas_composition", by, title="Each area: its largest names, the ranks that fail the limits, its carbon",
                          roles={"name": "area", "amount": "carbon_t"}, show={"kind": "chart", "chart": "bar", "x": "area", "y": "ranks_failing", "how": "sum"}),
                api.table("size_classes", size, title="Trees by diameter", roles={"category": "diameter_class", "amount": "trees"},
                          show={"kind": "chart", "chart": "bar", "x": "diameter_class", "y": "trees", "how": "sum"}),
                *notes,
            ]
    return Forest()
