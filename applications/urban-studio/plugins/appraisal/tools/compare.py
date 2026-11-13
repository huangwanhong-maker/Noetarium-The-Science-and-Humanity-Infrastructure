"""Alternatives compared on several criteria: limits first, then a ranking by declared weights, how far each weight can move, and how often
each alternative ranks first when no weight is declared."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _appraisal as a  # noqa: E402


def create(api):
    class Compare(api.BoardAnalysis):
        id = "compare"
        title = "Alternatives compared"
        description = ("A table of alternatives scored on criteria. Limits that no score can buy back are applied first (a right, a legal bound); "
                       "the rest are ranked by declared weights, by a weighted sum (fully compensatory), TOPSIS (closeness to the ideal) or "
                       "PROMETHEE II (outranking). Each weight's interval of stability says how far it can move before the leader changes; SMAA "
                       "says how often each alternative ranks first when the weights are not declared at all.")
        params = [api.param("table", "Alternatives", "alternatives", kind="text", role="table", help="one row an alternative, a column a criterion"),
                  a.col(api, "name", "Name", "alternative"),
                  api.param("criteria", "Criteria", "cost:min:0.4, benefit:max:0.4, risk:min:0.2", kind="text",
                            help="column:max|min:weight, separated by commas (the weights are declared, and normalised)"),
                  api.param("limits", "Limits", "", kind="text", help="column>=value or column<=value, separated by commas: set aside whatever else is offered"),
                  api.param("method", "Method", "weighted sum", kind="choice", options=["weighted sum", "TOPSIS", "PROMETHEE II"])]

        def run(self, board, params, context):
            t = board.table(params["table"])
            names = a.texts(t, params["name"])
            crit, benefit, weight = [], [], []
            for part in a.words(params["criteria"]):
                bits = [b.strip() for b in part.split(":")]
                if len(bits) != 3 or bits[1] not in ("max", "min"):
                    raise ValueError(f"a criterion is column:max|min:weight, not '{part}'")
                crit.append(bits[0]), benefit.append(bits[1] == "max"), weight.append(float(bits[2]))
            cols = [a.nums(t, c) for c in crit]
            keep = [i for i in range(t.rows) if names[i] and all(c[i] is not None for c in cols)]
            limits = []
            for part in a.words(params["limits"]):
                op = ">=" if ">=" in part else "<=" if "<=" in part else None
                if not op:
                    raise ValueError(f"a limit is column>=value or column<=value, not '{part}'")
                c, v = [x.strip() for x in part.split(op)]
                if c not in crit:
                    raise LookupError(f"a limit names '{c}', which is not among the criteria")
                limits.append({"criterion": crit.index(c), "bound": float(v), "at_least": op == ">="})
            method = {"weighted sum": "weighted_sum", "TOPSIS": "topsis", "PROMETHEE II": "promethee"}[params["method"]]
            r = a.appraisal.ask("mcda", alternatives=[names[i] for i in keep], criteria=crit, score=[[c[i] for c in cols] for i in keep], benefit=benefit,
                                weight=weight, limits=limits, method=method, preferences=[{"kind": "usual"}] * len(crit))
            out = []
            if r.get("kept"):
                rank = {"alternative": r["kept"], "value": r["value"], "rank": r["rank"]}
                if r.get("acceptability"):
                    rank["first_under_any_weights"] = [row[0] for row in r["acceptability"]]
                out.append(api.table("ranking", rank, title=f"The ranking by {params['method']}", roles={"name": "alternative", "amount": "value"},
                                     show={"kind": "chart", "chart": "bar", "x": "alternative", "y": "value", "how": "sum"}))
                if r.get("stability"):
                    st = r["stability"]
                    total = sum(weight)
                    out.append(api.table("weight_stability", {"criterion": crit, "declared": [w / total for w in weight], "lowest": st["lower"], "highest": st["upper"]},
                                         title=f"How far each weight can move with {st['best']} still first (the others scaled in proportion)", roles={"name": "criterion"}))
            leader = r["kept"][r["rank"].index(1)] if r.get("kept") else None
            words = []
            if r["set_aside"]:
                words.append(f"Set aside by the limits, whatever else they offer: {', '.join(f'{n} ({w})' for n, w in zip(r['set_aside'], r['why']))}.")
            if leader:
                words.append(f"{leader} ranks first by {params['method']}.")
                if r.get("acceptability"):
                    first = {n: row[0] for n, row in zip(r["kept"], r["acceptability"])}
                    words.append(f"With no weights declared at all, it ranks first under {100 * first[leader]:.0f}% of the weights one could choose.")
                if r.get("stability"):
                    st = r["stability"]
                    tight = min(range(len(crit)), key=lambda j: st["upper"][j] - st["lower"][j])
                    words.append(f"The answer is most sensitive to the weight on {crit[tight]}: it holds for weights from {st['lower'][tight]:.2f} to {st['upper'][tight]:.2f}.")
            else:
                words.append("Every alternative breaks a limit: none is left to rank.")
            out.append(api.note(" ".join(words)))
            out.append(api.note("The weights are declared, and the weighted sum and TOPSIS let much of one criterion make up for little of another; the limits do not. "
                                "SMAA's shares are of weights drawn uniformly, a statement about the weights, not about the world."))
            return out
    return Compare()
