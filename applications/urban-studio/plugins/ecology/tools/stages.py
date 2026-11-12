"""The forest as a population by size: its stage model, what each stage's rates are worth, and the forest a planting sustains."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Stages(api.BoardAnalysis):
        id = "stages"
        title = "The forest by size: a stage model"
        description = ("The trees in classes of diameter, each class with its annual survival (from the mean remaining life of its trees) and "
                       "growth into the next (its trees' increment over the class's width): a population model whose dominant eigenvalue says "
                       "whether the forest keeps itself, whose elasticities say which rates matter most, whose life expectancies say how long a "
                       "tree of each size stands, and whose equilibrium is the forest a constant planting sustains.")
        params = f.column_params(api) + f.life_params(api) + f.allometry_params(api)[:1] + [
            api.param("classes", "Class bounds (cm)", "0, 15, 30, 45, 60, 90", kind="text", help="the lower bounds of the diameter classes"),
            api.param("hazard", "Removal without a stated life", 0.03, kind="number", minimum=0, maximum=0.5, step=0.005),
            api.param("planting", "Trees planted a year", 1000, kind="number", minimum=0, maximum=1e6),
            api.param("increment", "Increment where none is told (cm a year)", 0.8, kind="number", minimum=0, maximum=5)]

        def run(self, board, params, context):
            trees, areas, t, info = f.read_trees(board, params, life=True)
            bounds = [float(x) for x in str(params["classes"]).replace(";", ",").split(",") if x.strip()]
            if len(bounds) < 2 or any(b2 <= b1 for b1, b2 in zip(bounds, bounds[1:])):
                raise ValueError("class bounds are increasing numbers, at least two")
            a = f.ecology.ask("stages", trees=trees, scheme=f.allometry(params), removal={"hazard": float(params["hazard"])}, bounds=bounds,
                              planting=float(params["planting"]), increment=float(params["increment"]))
            k = len(a["stages"])
            el = a["rates"]
            cols = {"stage": a["stages"], "trees_now": a["present"], "survival": [round(x, 4) for x in a["survival"]], "growth": [round(x, 4) for x in a["growth"]],
                    "years_standing": [round(x, 1) if x is not None else None for x in a["life_expectancy"]],
                    "stable_share": [round(x, 4) for x in a["stable"]],
                    "elasticity_survival": [round(x, 4) for x in el["survival"]] if el["survival"] else [None] * k,
                    "elasticity_growth": [round(x, 4) for x in el["growth"]] if el["growth"] else [None] * k}
            if a.get("equilibrium"):
                cols["sustained"] = [round(x) for x in a["equilibrium"]]
                cols["sustained_elasticity_survival"] = [round(x, 4) for x in a["sustained"]["survival_elasticity"]]
                cols["sustained_elasticity_growth"] = [round(x, 4) for x in a["sustained"]["growth_elasticity"]]
            lam = a["lambda"]
            total = sum(a.get("equilibrium") or [])
            words = (f"λ = {lam:.4f}: without planting the forest {'shrinks' if lam < 1 else 'holds or grows'} by {abs(lam - 1) * 100:.2f}% a year once its "
                     f"sizes settle (a street forest does not seed itself; its recruits are planted).")
            sustain = (f" A planting of {params['planting']:,.0f} trees a year sustains about {total:,.0f} trees, the sum of the expected years each "
                       f"planted tree stands times the planting." if total else "")
            sus = a.get("sustained")
            if sus:
                se = sus["survival_elasticity"]
                top = max(range(k), key=lambda j: se[j])
                matter = (f" With no recruitment λ is only the survival of the longest-lived class; what matters is what the planting sustains, "
                          f"and that answers most to the survival of trees {a['stages'][top]}: one per cent more of it keeps {se[top]:.2f}% more "
                          f"trees standing.")
            elif el["survival"]:
                top = max(range(k), key=lambda j: el["survival"][j])
                matter = f" The survival of trees {a['stages'][top]} matters most to λ (elasticity {el['survival'][top]:.2f})."
            else:
                matter = ""
            return [api.table("stage_model", cols, title="The forest by diameter class: rates, years standing, what matters, what a planting sustains",
                              roles={"category": "stage", "amount": "trees_now"}, show={"kind": "chart", "chart": "bar", "x": "stage", "y": "sustained_elasticity_survival" if a.get("sustained") else "elasticity_survival", "how": "sum"}),
                    api.note(words + matter + sustain),
                    api.note("Presumed, not observed: a class's survival is the constant hazard with its trees' mean remaining life; its growth the "
                             "fraction of a class, spread evenly across it, that grows out of it in a year (Usher's model). The model is the forest's "
                             "rates read as one population; individual trees differ." + f.aside_words(info))]
    return Stages()
