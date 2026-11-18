"""Where growth goes, across scales: the city's change of a use shared among its areas and, within each, among its blocks; fitted on the
years before those held aside and judged on them at every level."""

import json
import math
import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _scales as s  # noqa: E402


def create(api):
    class Growth(api.BoardAnalysis):
        id = "growth"
        title = "Where growth goes, across scales"
        description = ("Each year the city's change in the floor space of a use is given; it is shared among the areas by a logit on their floor of "
                       "that use (α), and within each area among its blocks by a logit on their own floor (β), their neighbours' (γ) and their distance "
                       "from the centre (δ). With α = β = 1 and γ = δ = 0 every unit keeps its share: proportional allocation, which the model nests. "
                       "The four are fitted to every block's floor on the years before those held aside, and the model is judged on them at each "
                       "level: the city (given), the areas and the blocks, against standing still (persistence) and against proportional allocation.")
        params = [api.param("case", "City", "melbourne", kind="text"),
                  api.param("use", "Use", "homes", kind="text", help="homes, office, retail, hotels, education, health, culture, community, workshops"),
                  api.param("fit_to", "Fitted to", 2016, minimum=2006, maximum=2022, step=1),
                  api.param("most", "Iterations at most", 80, minimum=10, maximum=300, step=10)]

        def run(self, board, params, context):
            case = str(params.get("case") or "melbourne")
            use = str(params.get("use") or "homes")
            fit_to = int(params.get("fit_to") or 2016)
            p = s.product(case)
            uses = s.grouped(p, s.withheld(p, case))
            if use not in uses:
                raise ValueError(f"no use is called {use}")
            t0, t1 = p["years"][0], p["years"][-1]
            doc, obs = s.growth_document(p, uses, use, t0, t1)
            f = s.scales.ask("fit", document=doc, observations=obs, held_after=fit_to, most=int(params.get("most") or 80))
            fitted = {u["name"]: u["value"] for u in f["unknowns"]}

            def run_with(values):
                d = json.loads(json.dumps(doc))
                d["inputs"]["parameters"] = values
                return s.scales.ask("run", document=d, run={"t0": t0, "t1": t1, "record": ["h", "H", "Hc"]})
            rf, rp = run_with(fitted), run_with({"alpha": 1, "beta": 1, "gamma": 0, "delta": 0})
            T = rf["times"]
            kf, k1 = T.index(fit_to), T.index(t1)
            units, parent, areas = p["levels"][0]["units"], p["levels"][0]["parent"], p["levels"][1]["units"]
            obs_f = np.array([x or 0.0 for x in uses[use][str(fit_to)][0]])
            obs_1 = np.array([x or 0.0 for x in uses[use][str(t1)][0]])
            A_f = np.array([obs_f[[b for b in range(len(units)) if parent[b] == a]].sum() for a in areas])
            A_1 = np.array([obs_1[[b for b in range(len(units)) if parent[b] == a]].sum() for a in areas])

            def judge(r):
                h_f, h_1 = np.array(r["values"]["h"][kf]), np.array(r["values"]["h"][k1])
                H_f, H_1 = np.array(r["values"]["H"][kf]), np.array(r["values"]["H"][k1])
                return {"block": (float(np.corrcoef(obs_1 - obs_f, h_1 - h_f)[0, 1]), float(np.abs(h_1 - obs_1).sum() / max(obs_1.sum(), 1))),
                        "area": (float(np.corrcoef(A_1 - A_f, H_1 - H_f)[0, 1]), float(np.abs(H_1 - A_1).sum() / max(A_1.sum(), 1)))}
            jf, jp = judge(rf), judge(rp)
            persist = {"block": float(np.abs(obs_f - obs_1).sum() / max(obs_1.sum(), 1)), "area": float(np.abs(A_f - A_1).sum() / max(A_1.sum(), 1))}
            out = [api.table("growth_unknowns", {
                "unknown": [u["name"] for u in f["unknowns"]], "value": [u["value"] for u in f["unknowns"]], "standard error": [u["se"] for u in f["unknowns"]],
                "what it says": [next(q["unit"] for q in doc["model"]["parameters"] if q["name"] == u["name"]) for u in f["unknowns"]]},
                title=f"Where {use} grows in {case.title()}: the unknowns fitted on {t0}–{fit_to}")]
            out.append(api.table("growth_judged", {
                "level": ["areas", "blocks"],
                "change placed, fitted (correlation)": [jf["area"][0], jf["block"][0]],
                "change placed, proportional (correlation)": [jp["area"][0], jp["block"][0]],
                f"error in {t1}, fitted (share of the total)": [jf["area"][1], jf["block"][1]],
                f"error in {t1}, proportional": [jp["area"][1], jp["block"][1]],
                f"error in {t1}, standing still since {fit_to}": [persist["area"], persist["block"]]},
                title=f"How well the model places {fit_to}–{t1}'s change, which it was not shown"))
            gain = obs_1 - obs_f
            pos = np.sort(gain[gain > 0])[::-1]
            top = int(math.ceil(0.05 * len(units)))
            lumpy = float(pos[:top].sum() / pos.sum()) if pos.sum() > 0 else None
            out.append(api.table("growth_blocks", {
                "block": units, "area": parent, f"change {fit_to}–{t1}, recorded (m²)": gain.tolist(),
                f"change {fit_to}–{t1}, model (m²)": (np.array(rf["values"]["h"][k1]) - np.array(rf["values"]["h"][kf])).tolist()},
                title=f"{use.title()} gained by each block, {fit_to}–{t1}: the record and the model",
                places={"kind": "cells", "column": "block", "rings": p["levels"][0]["rings"]},
                show={"kind": "layer", "column": f"change {fit_to}–{t1}, recorded (m²)"}))
            words = [f"Fitted: α {fitted['alpha']:.2f}, β {fitted['beta']:.2f}, γ {fitted['gamma']:.2f}, δ {fitted['delta']:.2f}"
                     f" ({'converged' if f['converged'] else 'not converged in the iterations allowed'})."]
            if f["not_identified"]:
                words.append("Not identified by this record: " + "; ".join(f["not_identified"]) + ".")
            words.append(f"Over {fit_to}–{t1} the {top} blocks that gained most took {s.pct(lumpy)} of all the {use} added: growth comes in "
                         "buildings, not in shares, and a model of shares spreads it. Among the areas the fitted model places the change with a "
                         f"correlation of {jf['area'][0]:.2f} (proportional allocation {jp['area'][0]:.2f}); among the blocks {jf['block'][0]:.2f} "
                         f"({jp['block'][0]:.2f}). Standing still errs by {s.pct(persist['block'])} of the blocks' total in {t1}, the model by "
                         f"{s.pct(jf['block'][1])}. The city's change is given, not forecast.")
            out.append(api.note(" ".join(words)))
            return out
    return Growth()
