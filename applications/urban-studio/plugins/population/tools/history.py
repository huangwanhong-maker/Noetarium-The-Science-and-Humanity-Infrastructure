"""A place's people, 1975 to 2030: the field the data library made of a place, cell by cell, as tables for the map and a chart."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _population as p  # noqa: E402


def create(api):
    class History(api.BoardAnalysis):
        id = "history"
        title = "A place's people, 1975 to 2030"
        description = ("The residential population of a place at every fifth year from 1975 to 2030 (GHS-POP R2023A, the European Commission's JRC, CC BY 4.0), "
                       "carried onto a square grid of 1 km with no person lost; 2025 and 2030 are the JRC's projections, not counts. A place not yet made "
                       "is made from its latitude and longitude, about a minute's fetching. Its cells come as a table placed on the map, and its totals as a chart.")
        params = p.case_params(api)

        def run(self, board, params, context):
            f, case = p.field_of(params)
            cs = p.cells(f, case)
            epochs = f["epochs"]
            cols = {"cell": [f"r{c['row']:03d}c{c['col']:03d}" for c in cs], "lat": [c.get("lat") for c in cs], "lon": [c.get("lon") for c in cs]}
            for e in epochs:
                vals = f["values"][str(e)]
                cols[f"p{e}"] = [vals[c["row"]][c["col"]] for c in cs]
            first, last = epochs[0], max(e for e in epochs if e not in f.get("projected", []))
            cols[f"change_{first}_{last}"] = [(b or 0) - (a or 0) for a, b in zip(cols[f"p{first}"], cols[f"p{last}"])]
            out = [api.table("cells", cols, title=f"{f['title']}: people in each km²", roles={"lat": "lat", "lon": "lon", "name": "cell", "amount": f"p{last}"},
                             show={"kind": "layer", "layer": "points", "colour": f"p{last}", "map": "orchid"})]
            t = p.pd.totals(f)
            out.append(api.table("totals", {"year": t["years"], "people": t["people"], "projected": ["yes" if x else "no" for x in t["projected"]]},
                                 title=f"{f['title']}: everyone in the square", show={"kind": "chart", "chart": "line", "x": "year", "y": "people", "how": "sum"}))
            grow = t["people"][epochs.index(last)] / t["people"][0]
            out.append(api.note(f"{f['title'].split(':')[0]}: {t['people'][0]:,.0f} people in {first} and {t['people'][epochs.index(last)]:,.0f} in {last}, "
                                f"{grow:.2f} times as many, on {len(cs):,} land cells of 1 km². The JRC projects {t['people'][-1]:,.0f} for {epochs[-1]}."))
            out.append(api.note("Each epoch is the JRC's disaggregation of the censuses then available into the built-up surface, not a count of each km²: "
                                "where the censuses were coarse, so is what a cell says."))
            return out
    return History()
