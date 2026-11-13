"""What final demand causes: an economy's input–output table extended by its industries' emissions, read as the pressures each product and
each kind of final use causes along its supply chains."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402


def create(api):
    class Footprint(api.BoardAnalysis):
        id = "footprint"
        title = "What final demand causes"
        description = ("An input–output table (industries by industries, then columns of final use) and a table of each industry's emissions or "
                       "extraction, read by the Leontief model (Miller and Blair 2009): each industry's pressure per unit of output and along all its "
                       "supply chains, what each product bought for final use causes, what each kind of final use causes (households, government, "
                       "investment, exports), and the chains that carry it. A single-region table treats imports as coming from outside; what they "
                       "embody abroad needs a multi-regional table, and is not counted.")
        params = [api.param("table", "Input–output table", "io", kind="text", role="table", help="one row an industry; columns the industries, then final uses"),
                  m.col(api, "industry", "Industry", "industry"),
                  api.param("final", "Final uses", "households, government, non-profit institutions, gross fixed capital formation, inventories, valuables, exports",
                            kind="text", help="the table's columns of final use, separated by commas"),
                  api.param("pressures", "Pressures", "air", kind="text", role="table", help="one row an industry (and one for households' own), a column a pressure"),
                  api.param("pressure", "Pressure", "GHG (t CO2-eq)", kind="text", role="column", of="pressures"),
                  api.param("direct", "Households' own row", "households", kind="text", help="the row of the pressures table that final users emit themselves"),
                  api.param("unit", "Money unit of the table", "million EUR", kind="text")]

        def run(self, board, params, context):
            t, e = board.table(params["table"]), board.table(params["pressures"])
            ind = m.texts(t, params["industry"])
            final = [c.strip() for c in str(params["final"]).split(",") if c.strip()]
            for c in ind + final:
                if c and c not in final and not t.has(c):
                    raise LookupError(f"the input–output table has no column for the industry '{c}'")
            Z = [[v or 0.0 for v in m.nums(t, j)] for j in ind]  # columns, then transpose
            Z = [list(row) for row in zip(*Z)]
            Y = [list(row) for row in zip(*[[v or 0.0 for v in m.nums(t, c)] for c in final])]
            # An industry with no output (extraterritorial organisations, often) has no coefficients: it is left out, and said to be.
            output = [sum(Z[i]) + sum(Y[i]) for i in range(len(ind))]
            empty = [ind[i] for i in range(len(ind)) if not output[i] > 0]
            if empty:
                keep = [i for i in range(len(ind)) if output[i] > 0]
                Z = [[Z[i][j] for j in keep] for i in keep]
                Y = [Y[i] for i in keep]
                ind = [ind[i] for i in keep]
            eind, ev = m.texts(e, "industry"), m.nums(e, params["pressure"])
            byind = dict(zip(eind, ev))
            lost = sum(byind.get(x) or 0 for x in empty)
            missing = [i for i in ind if byind.get(i) is None]
            F = [[byind.get(i) or 0.0 for i in ind]]
            Fy = [[byind.get(params["direct"]) or 0.0]]
            labels = dict(zip(ind, m.texts(t, "label", optional=True)))
            context.progress(0.3, f"{len(ind)} industries")
            a = m.materials.ask("footprint", regions=["economy"], sectors=ind, Z=Z, Y=Y, categories=final, direct=final[0],
                                stressors=[params["pressure"]], F=F, F_Y=Fy, paths={"stressor": 0, "cutoff": 2e-3})
            byp = a["by_product"][0]
            total = a["consumption"][0][0]
            mult = a["multipliers"][0]
            direct_int = a["intensities"][0]
            products = {"industry": ind, "label": [labels.get(i) or i for i in ind], "caused": byp, "share": [v / total for v in byp],
                        "per_unit": mult, "direct_per_unit": direct_int}
            order = sorted(range(len(ind)), key=lambda k: -byp[k])
            products = {k: [v[i] for i in order] for k, v in products.items()}
            cats = {"final_use": a["categories"], "caused": a["by_category"][0], "share": [v / total for v in a["by_category"][0]]}
            chains = {"chain": [" ← ".join(labels.get(s, s) for s in reversed([p.split(" · ")[-1] for p in w["path"]])) for w in a["paths"]["walks"]],
                      "caused": [w["value"] for w in a["paths"]["walks"]], "share": [w["value"] / total for w in a["paths"]["walks"]]}
            first = products["label"][0]
            hh = cats["caused"][0] / total
            exp = next((v for c, v in zip(cats["final_use"], cats["share"]) if c == "exports"), None)
            return [api.table("footprint_by_product", products, title=f"What each product bought for final use causes: {params['pressure']}",
                              roles={"name": "label", "amount": "caused"}, show={"kind": "chart", "chart": "bar", "x": "industry", "y": "caused", "how": "sum"}),
                    api.table("footprint_by_use", cats, title=f"What each kind of final use causes: {params['pressure']}",
                              roles={"name": "final_use", "amount": "caused"}, show={"kind": "chart", "chart": "bar", "x": "final_use", "y": "caused", "how": "sum"}),
                    api.table("supply_chains", chains, title=f"The supply chains that carry the most {params['pressure']}", roles={"name": "chain", "amount": "caused"}),
                    api.note(f"Final demand causes {m.big(total)} {params['pressure']} along its supply chains within the economy, households' own included. "
                             f"Households cause {100 * hh:.0f}% of it (their own heating and travel among it)"
                             + (f", exports {100 * exp:.0f}%" if exp is not None else "") + f". The product whose final purchase causes most is {first} "
                             f"({100 * products['share'][0]:.0f}%); the most intense per {params['unit']} of final demand is "
                             f"{labels.get(ind[max(range(len(ind)), key=lambda k: mult[k])]) or ind[max(range(len(ind)), key=lambda k: mult[k])]}."),
                    api.note("What the model assumes: each industry makes one output with fixed shares of inputs (the Leontief model), its pressure "
                             "proportional to its output. Imports are inputs from outside: what they embody where they were made is not counted here."
                             + (f" No pressure is recorded for {', '.join(missing)}; taken as zero." if missing else "")
                             + (f" Left out for having no output: {', '.join(empty)} (their recorded pressure, {lost:g}, is left out with them)." if empty else ""))]
    return Footprint()
