"""The vacancy cycle across scales: what the record identifies of it, what must be declared, and what the declarations do."""

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _scales as s  # noqa: E402


def create(api):
    class Vacancy(api.BoardAnalysis):
        id = "vacancy"
        title = "The vacancy cycle, and what must be declared"
        description = ("After DiPasquale and Wheaton: the city's occupied floor is given and shared among the areas by their stock; an area's "
                       "gap is what its stock does not hold and its vacancy that gap where positive; its rent relative to the cost of building falls "
                       "while the gap is above the natural vacancy rate and rises below it; its stock is built while that rent exceeds one; every block reads its area's vacancy. The rents are held "
                       "by no record here, so their starting values are declared. The four rates are first fitted to the areas' stock and vacancy; "
                       "those the record does not identify are then declared with the ranges given here, and the model is run over draws of every "
                       "declaration, with the declarations ranked by how much each moves the city's vacancy.")
        params = [api.param("case", "City", "melbourne", kind="text"),
                  api.param("fit_to", "Fitted to", 2016, minimum=2006, maximum=2022, step=1),
                  api.param("rho_low", "Rent over the cost of building in the first year: at least", 0.8, minimum=0.2, maximum=2, step=0.05),
                  api.param("rho_high", "... at most", 1.2, minimum=0.2, maximum=3, step=0.05),
                  api.param("lambda_range", "How fast rents answer vacancy, a year (low, high)", "0.5, 5", kind="text"),
                  api.param("kappa_range", "How fast stock answers rents, a year (low, high)", "0.02, 0.3", kind="text"),
                  api.param("vstar_range", "The natural vacancy rate (low, high)", "0.03, 0.10", kind="text"),
                  api.param("gamma_range", "How demand among areas follows their stock (low, high)", "0.8, 1.2", kind="text"),
                  api.param("draws", "Draws", 64, minimum=16, maximum=400, step=16)]

        def run(self, board, params, context):
            case = str(params.get("case") or "melbourne")
            fit_to = int(params.get("fit_to") or 2016)
            p = s.product(case)
            uses = s.grouped(p, s.withheld(p, case))
            t0, t1 = p["years"][0], p["years"][-1]
            rho = (float(params.get("rho_low") or 0.8), float(params.get("rho_high") or 1.2))
            if not rho[0] < rho[1]:
                raise ValueError("the declared range of the starting rents runs from low to high")
            doc, obs = s.vacancy_document(p, uses, t0, t1, rho)
            f = s.scales.ask("fit", document=doc, observations=obs, held_after=fit_to)
            ranges = {}
            for name in ("lambda", "kappa", "vstar", "gamma"):
                lo, hi = (float(x) for x in str(params.get(f"{name}_range")).split(","))
                if not lo < hi:
                    raise ValueError(f"the declared range of {name} runs from low to high")
                ranges[name] = (lo, hi)
            unidentified = {n.split(":")[0].split(" and ")[0] for n in f["not_identified"]} | {n.split(" and ")[1].split(" move")[0] for n in f["not_identified"] if " and " in n}
            declared = {}
            for u in f["unknowns"]:
                if u["name"] in unidentified or u["se"] is None:
                    declared[u["name"]] = {"source": "declared", "lower": ranges[u["name"]][0], "upper": ranges[u["name"]][1]}
                else:
                    declared[u["name"]] = {"value": u["value"], "source": "estimated", "sigma": u["se"]}
            d = json.loads(json.dumps(doc))
            d["inputs"]["parameters"] = declared
            e = s.scales.ask("ensemble", document=d, draws=int(params.get("draws") or 64), outputs=["vc", "v"])
            sc = s.scales.ask("screen", document=d, output="vc", trajectories=8)
            years = e["times"]
            stock = {y: sum(v for v in p["years_of"][str(y)]["total"] if v is not None) for y in years}
            vac = {y: sum(sum(uses[k][str(y)][0][b] or 0.0 for k in s.VACANT) for b in range(len(p["levels"][0]["units"]))
                          if p["years_of"][str(y)]["total"][b] is not None) for y in years}
            out = [api.table("vacancy_unknowns", {
                "unknown": [u["name"] for u in f["unknowns"]], "fitted": [u["value"] for u in f["unknowns"]],
                "standard error": [u["se"] for u in f["unknowns"]],
                "used as": ["declared, " + f"{declared[u['name']]['lower']:g} to {declared[u['name']]['upper']:g}" if declared[u["name"]]["source"] == "declared"
                            else "estimated" for u in f["unknowns"]]},
                title=f"The vacancy cycle in {case.title()}: what the record of {t0}–{fit_to} identifies")]
            b = e["bands"]["vc"]
            out.append(api.table("vacancy_city", {
                "year": years, "recorded": [vac[y] / stock[y] for y in years],
                "model, 5%": [r[0] for r in b["low"]], "model, median": [r[0] for r in b["middle"]], "model, 95%": [r[0] for r in b["high"]]},
                title=f"The city's vacancy, recorded and under the declarations ({e['draws']:.0f} draws)",
                show={"kind": "chart", "chart": "line", "x": "year", "y": "recorded"}))
            ranked = sorted(sc["factors"], key=lambda r: -(r["mu_star"] or 0))
            out.append(api.table("vacancy_declarations", {
                "declared": [r["name"] for r in ranked], "kind": [r["what"] for r in ranked],
                "moves the city's vacancy by (μ*)": [r["mu_star"] for r in ranked]},
                title="Which declaration matters most to the city's vacancy in the last year (Morris's screening)"))
            words = []
            if f["not_identified"]:
                words.append(f"The areas' stock and vacancy over {t0}–{fit_to} do not identify " + "; ".join(f["not_identified"]) +
                             ". These are declared, with the ranges given, rather than read from a fit the record cannot support.")
            words.append(f"On the years held aside the fit errs by {f['held_rms']:.2f} σ against standing still's {f['persistence_rms']:.2f}.")
            if ranked:
                words.append(f"Of the declarations, {ranked[0]['name']} moves the city's vacancy most; it is the one worth finding a record for.")
            words.append("The rents are relative to the cost of building and unobserved; the model says what they would have to be, not what they were.")
            out.append(api.note(" ".join(words)))
            return out
    return Vacancy()
