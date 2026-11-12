"""A population by stages: any organism counted by size, age or condition, from a table of its vital rates."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Population(api.BoardAnalysis):
        id = "population"
        title = "A population by stages"
        description = ("Any population counted by stages (trees by size, birds by age, insects by instar), from a table with one row a stage and "
                       "its survival, growth into the next stage and fecundity: its rate of growth λ, its stable structure and reproductive "
                       "values, the elasticities of λ to each rate, the expected life from each stage, R₀, and its projection from today's "
                       "counts with a yearly input (planting, stocking, arrivals) and the equilibrium that input sustains.")
        params = [api.param("table", "Stages", "stage_model", kind="text", role="table", help="one row a stage, in order"),
                  api.param("stage", "Stage", "stage", kind="text", role="column", of="table"),
                  api.param("survival", "Survival", "survival", kind="text", role="column", of="table"),
                  api.param("growth", "Growth", "growth", kind="text", role="column", of="table", help="the share that grows into the next stage"),
                  api.param("fecundity", "Fecundity", "", kind="text", role="column", of="table", help="new members of the first stage per member; empty for none"),
                  api.param("now", "Counts today", "trees_now", kind="text", role="column", of="table", help="empty: a hundred in the first stage"),
                  api.param("input", "Yearly input to the first stage", 0, kind="number", minimum=0),
                  api.param("years", "Years", 50, kind="integer", minimum=1, maximum=500)]

        def run(self, board, params, context):
            t = board.table(params["table"])
            num = lambda c: [0.0 if not math.isfinite(x) else float(x) for x in t.column(c)] if c and t.has(c) else None  # noqa: E731
            surv, grow = num(params["survival"]), num(params["growth"])
            if surv is None or grow is None:
                raise LookupError("the table needs a column of survival and one of growth")
            grow[-1] = 0.0  # nothing grows out of the last stage
            fec = num(params["fecundity"]) or [0.0] * len(surv)
            now = num(params["now"]) or ([100.0] + [0.0] * (len(surv) - 1))
            stages = f._text(t, params["stage"]) if t.has(params["stage"]) else [f"stage {k + 1}" for k in range(len(surv))]
            inp = [float(params["input"])] + [0.0] * (len(surv) - 1)
            a = f.ecology.ask("population", stages=stages, survival=surv, growth=grow, fecundity=fec, n0=now, years=int(params["years"]), input=inp)
            proj = a["projection"]
            path = {"year": list(range(len(proj))), "total": [round(sum(r), 1) for r in proj]}
            for k, s in enumerate(stages):
                path[s] = [round(r[k], 1) for r in proj]
            el = a["rates"]
            rows = {"stage": stages, "stable_share": [round(x, 4) for x in a["stable"]], "reproductive_value": [round(x, 4) for x in a["reproductive"]],
                    "years_alive": a["life_expectancy"], "elasticity_survival": el["survival"], "elasticity_growth": el["growth"], "elasticity_fecundity": el["fecundity"]}
            if a.get("equilibrium"):
                rows["sustained"] = [round(x, 1) for x in a["equilibrium"]]
            lam = a["lambda"]
            return [api.table("population_path", path, title="The population projected, stage by stage", roles={"time": "year", "amount": "total"},
                              show={"kind": "chart", "chart": "line", "x": "year", "y": "total", "how": "sum"}),
                    api.table("population_analysis", rows, title="Its stable structure, values, elasticities and expected lives",
                              roles={"category": "stage"}, show={"kind": "chart", "chart": "bar", "x": "stage", "y": "elasticity_survival", "how": "sum"}),
                    api.note(f"λ = {lam:.4f}: the population {'grows' if lam > 1 else 'declines' if lam < 1 else 'holds'} by {abs(lam - 1) * 100:.2f}% a year "
                             f"once its structure settles" + (f" (R₀ = {a['r0']:.3f} new members a member leaves)" if a.get("r0") is not None else "")
                             + (f"; a damping ratio of {a['damping']:.2f} says how fast it settles" if a.get("damping") else "")
                             + (f". The yearly input sustains about {sum(a['equilibrium']):,.0f} members." if a.get("equilibrium") else "."))]
    return Population()
