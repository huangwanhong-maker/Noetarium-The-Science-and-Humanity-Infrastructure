"""Indexes across scales: the urban-science domain's indexes that a city's units can give, at every level of its hierarchy at once."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _scales as s  # noqa: E402


def create(api):
    class Indexes(api.BoardAnalysis):
        id = "indexes"
        title = "Indexes across scales"
        description = ("The indexes of the urban-science domain's catalogue that the city's records can give (vacancy, floor area ratio, densities, "
                       "the shares and mix of uses, how concentrated the uses are, how unlike one another the blocks are), computed for every block, "
                       "every area and the whole city in one run of the model's own operators: a ratio is the ratio of the sums at every level, "
                       "never a mean of the blocks' ratios, and an index among the blocks beneath is recomputed from them. Floor space the census "
                       "withheld enters as its estimate (see *What the census withheld*).")
        params = [api.param("case", "City", "melbourne", kind="text", help="a city the data library has made (data/products/city_scales.py)"),
                  api.param("year", "Year", 2024, minimum=2002, maximum=2024, step=1)]

        def run(self, board, params, context):
            case = str(params.get("case") or "melbourne")
            p = s.product(case)
            year = int(params.get("year") or p["years"][-1])
            if year not in p["years"]:
                raise ValueError(f"{case} has no census in {year}: {p['years'][0]}–{p['years'][-1]}")
            uses = s.grouped(p, s.withheld(p, case))
            r = s.scales.ask("run", document=s.index_document(p, uses, p["years"]))
            k = r["times"].index(year)
            v = r["values"]
            cat = s.catalogue()
            fine, area, city = (lv["name"] for lv in p["levels"])
            out = []
            # Blocks, placed.
            cols = {"block": p["levels"][0]["units"], "area": p["levels"][0]["parent"]}
            for i, (cid, label, _, _) in enumerate(s.INDEXES):
                cols[label] = [s.num(x) for x in v[f"i{i}"][k]]
            out.append(api.table("indexes_blocks", cols, title=f"{case.title()}'s blocks in {year}: the indexes of each",
                                 places={"kind": "cells", "column": "block", "rings": p["levels"][0]["rings"]},
                                 show={"kind": "layer", "column": s.INDEXES[0][1]}))
            # Areas and the city, with the indexes among the blocks beneath.
            acols = {"area": p["levels"][1]["units"]}
            for i, (cid, label, _, _) in enumerate(s.INDEXES):
                acols[label] = [s.num(x) for x in v[f"i{i}_{area}"][k]]
            for i, (cid, label, *_rest) in enumerate(s.AMONG):
                acols[label] = [s.num(x) for x in v[f"a{i}_{area}"][k]]
            out.append(api.table("indexes_areas", acols, title=f"{case.title()}'s areas in {year}: the indexes of each, and among its blocks",
                                 show={"kind": "chart", "chart": "bar", "x": "area", "y": s.INDEXES[0][1]}))
            # The city over the years.
            ccols = {"year": r["times"]}
            for i, (cid, label, _, _) in enumerate(s.INDEXES):
                ccols[label] = [s.num(row[0]) for row in v[f"i{i}_{city}"]]
            for i, (cid, label, *_rest) in enumerate(s.AMONG):
                ccols[label] = [s.num(row[0]) for row in v[f"a{i}_{city}"]]
            out.append(api.table("indexes_city", ccols, title=f"The {p['levels'][2]['units'][0]} over the years",
                                 show={"kind": "chart", "chart": "line", "x": "year", "y": s.INDEXES[0][1]}))
            # A ratio of sums against the mean of ratios, for the first index.
            blocks = [x for x in v["i0"][k] if x is not None]
            mean_of_ratios = sum(blocks) / len(blocks) if blocks else None
            city_value = v[f"i0_{city}"][k][0]
            words = [f"In {year} the city's {s.INDEXES[0][1]} is {s.pct(city_value)}, the ratio of its sums; the mean of its blocks' rates "
                     f"would say {s.pct(mean_of_ratios)}, since a small block counts as much as a large one there."]
            named = [f"{label} ({cat[cid]['name']})" for cid, label, *_ in s.INDEXES + [(a[0], a[1]) for a in s.AMONG] if cid and cid in cat]
            words.append("In the domain's catalogue: " + "; ".join(named) + ". The others are not named by the atlas.")
            words.append("How each index is carried up is its own: ratios and densities as values on summed quantities, the mix and the "
                         "concentration of uses likewise within each unit, and the dissimilarity and inequality among the blocks beneath "
                         "recomputed from them at each level.")
            out.append(api.note(" ".join(words)))
            return out
    return Indexes()
