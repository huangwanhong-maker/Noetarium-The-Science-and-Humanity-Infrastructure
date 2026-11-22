"""A region's economy: how regions' employment changed against their countries (shift-share), how varied and how related their industries
are, which industries lie nearest those they hold, how complex they are, and whether relatedness told which industries they took up
(ADR 0166)."""

from science import diversification

LABELS = {}


def create(api):
    class Structure(api.BoardAnalysis):
        id = "structure"
        title = "A region's economy: its structure and how it changed"
        description = ("Regions' employment by industry (NACE divisions within their sections) at two dates, from Eurostat's regional business "
                       "statistics. For each region: shift-share against its country (what the country's growth, the region's mix of industries "
                       "and its own competitiveness account for, the country's employment the sum of its regions); variety in bits, among "
                       "sections (unrelated) and within them (related); the industries it holds with a location quotient at or above the "
                       "threshold, the economic complexity of that set, the industries it took up and gave up, and the industries it does not "
                       "hold that lie nearest those it does, by relatedness density. Over all regions: whether density at the first date told "
                       "the industries regions took up by the second from those they did not, as an area under the curve (one half is no "
                       "better than chance). A cell Eurostat withholds at either date is left out at both, and counted. The complexity index is a "
                       "region's position along the strongest division of the industries, not a measure of sophistication. Nothing here says why a "
                       "region diversified.")
        params = [api.param("case", "Case of the data library", "europe", kind="text", help="a case of regional_structure.py: europe, central-europe"),
                  api.param("from_year", "First year", 2011, minimum=2008, maximum=2020, step=1),
                  api.param("to_year", "Second year", 2019, minimum=2008, maximum=2020, step=1),
                  api.param("country", "Show the regions of", "CZ", kind="text", help="a country's code (CZ, DE, …), or all"),
                  api.param("threshold", "A region holds an industry at a location quotient of at least", 1, minimum=0.25, maximum=4, step=0.05)]

        def run(self, board, params, context):
            g = lambda k, d: params.get(k) if params.get(k) not in (None, "") else d  # noqa: E731
            P = diversification.product(str(g("case", "europe")), int(g("from_year", 2011)), int(g("to_year", 2019)))
            y0, y1 = P["years"]
            a = diversification.ask("diversification.assess", table=diversification.table(P), threshold=float(g("threshold", 1)))
            names = {r["nuts"]: r["name"] for r in P["regions"]}
            labels = {i["code"]: i["label"] for i in P["industries"]}
            want = str(g("country", "CZ")).strip().upper()
            regs = [r for r in a["regions"] if want in ("ALL", "") or r["reference"] == want]
            if not regs:
                raise LookupError(f"no region of {want} in the case")
            pct = lambda v, k=4: None if v is None else round(v, k)  # noqa: E731
            cols = {"region": [], "name": [], "employment_" + str(y0): [], "employment_" + str(y1): [], "change": [], "national_share": [], "industry_mix": [],
                    "competitive": [], "related_variety": [], "unrelated_variety": [], "industries_held": [], "complexity": [], "entered": [], "left": [],
                    "withheld_cells": [], "nearest_industries": []}
            for r in regs:
                cols["region"].append(r["region"]), cols["name"].append(names.get(r["region"], r["region"]))
                cols["employment_" + str(y0)].append(round(r["before"])), cols["employment_" + str(y1)].append(round(r["after"]))
                cols["change"].append(round(r["after"] - r["before"])), cols["national_share"].append(round(r["national"]))
                cols["industry_mix"].append(round(r["mix"])), cols["competitive"].append(round(r["competitive"]))
                cols["related_variety"].append(pct(r["variety_after"]["related"], 3)), cols["unrelated_variety"].append(pct(r["variety_after"]["unrelated"], 3))
                cols["industries_held"].append(r["diversity_after"]), cols["complexity"].append(pct(r["complexity_after"], 3))
                cols["entered"].append(r["entries"]), cols["left"].append(r["exits"]), cols["withheld_cells"].append(r["withheld_before"] + r["withheld_after"])
                cols["nearest_industries"].append("; ".join(f"{o['industry']} {labels.get(o['industry'], '')[:40]} ({o['density']:.2f})" for o in r["opportunities"]))
            out = [api.table("economy_regions", cols, title=f"Regions' economies, {y0} to {y1}: shift-share, variety, complexity and the nearest industries not held",
                             show={"kind": "chart", "chart": "bar", "x": "name", "y": "competitive", "how": "sum"})]
            ss = {"region": [], "term": [], "employment": []}
            for r in regs:
                for k, word in (("national", "national share"), ("mix", "industry mix"), ("competitive", "competitive")):
                    ss["region"].append(names.get(r["region"], r["region"])), ss["term"].append(word), ss["employment"].append(round(r[k]))
            out.append(api.table("economy_shift_share", ss, title=f"Employment change {y0}–{y1} split by shift-share against the country",
                                 show={"kind": "chart", "chart": "bar", "x": "region", "y": "employment", "split": "term", "how": "sum"}))
            inds = sorted((i for i in a["industries"] if i["complexity"] is not None), key=lambda i: -i["complexity"])
            out.append(api.table("economy_industries", {"industry": [i["industry"] for i in inds], "label": [labels.get(i["industry"], "") for i in inds],
                                                        "complexity": [pct(i["complexity"], 3) for i in inds]},
                                 title=f"Industries by their place on the strongest division of the industries (the complexity index), every region, {y1}",
                                 show={"kind": "chart", "chart": "bar", "x": "industry", "y": "complexity", "how": "sum"}))
            e, x = a["entry"], a["exit"]
            top = max(regs, key=lambda r: r["complexity_after"] if r["complexity_after"] is not None else -9)
            words = [f"Across {len(a['regions'])} regions and {len(P['industries'])} industries, {int(a['entries'])} of {int(a['candidates'])} industries a region did not hold in {y0} "
                     f"were held by {y1}. Relatedness density in {y0} told them from those not taken up at an area under the curve of {e['auc']:.3f} "
                     f"(one half is chance; p = {e['p_value']:.1e}), and told the industries kept from those given up at {x['auc']:.3f}."]
            if inds:
                hi = ", ".join(labels.get(i["industry"], i["industry"]).split(",")[0].lower() for i in inds[:3])
                lo = ", ".join(labels.get(i["industry"], i["industry"]).split(",")[0].lower() for i in inds[-3:])
                low = min(regs, key=lambda r: r["complexity_after"] if r["complexity_after"] is not None else 9)
                words.append(f"The complexity index places each region along the strongest division of the industries, from {lo} at one end to {hi} at the "
                             f"other; by convention it rises with the number of industries a region holds. Among the regions shown {names.get(top['region'], top['region'])} "
                             f"lies furthest toward the second end ({top['complexity_after']:.2f}) and {names.get(low['region'], low['region'])} toward the first "
                             f"({low['complexity_after']:.2f}). It is a position on that division, not a measure of sophistication (Mealy, Farmer and Teytelboym 2019).")
            if not a["complexity_after"]:
                words.append("The complexity index is not defined here: " + a["complexity_note"])
            words.append(f"A cell Eurostat withholds at either date ({P['withheld_cells']} withheld in the table) is left out at both, so that withholding does not show as change; "
                         "it understates the regions that withhold most. "
                         f"The measures describe and predict; none says why a region diversified. {P['credit']}.")
            out.append(api.note(" ".join(words)))
            return out

    return Structure()
