"""Mill's methods: agreement, difference and the joint method, over cases with conditions present or absent."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Mill(api.BoardAnalysis):
        id = "mill"
        title = "Mill's methods"
        description = ("Mill's methods of agreement, difference and the joint method (A System of Logic, 1843, III.8) over cases whose conditions and outcome "
                       "are present (1) or absent (0): the conditions each method leaves standing, and the cases it rests on. They eliminate; they do not "
                       "prove, and they assume one cause where several may act (Ragin's objection, 1987).")
        params = [api.param("table", "Cases", "", kind="text", role="table"),
                  c.col(api, "case", "Case"),
                  c.cols(api, "conditions", "Conditions", help="1 present, 0 absent"),
                  c.col(api, "outcome", "Outcome")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            conds = c.words(params["conditions"])
            rows, dropped = c.complete(t, conds + [params["outcome"]], [params["case"]])
            names = c.text(t, params["case"])
            val = {k: c.numbers(t, k) for k in conds + [params["outcome"]]}
            for k in conds + [params["outcome"]]:
                if any(val[k][i] not in (0, 1) for i in rows):
                    raise ValueError(f"the column '{k}' holds values other than 0 and 1")
            r = c.inquiry.ask("mill", cases=[names[i] for i in rows], conditions=conds, values=[[int(val[k][i]) for k in conds] for i in rows],
                              outcome=[int(val[params["outcome"]][i]) for i in rows])
            out = {"method": [], "conditions_standing": [], "rests_on": [], "pairs": [], "note": []}
            for m in r["methods"]:
                out["method"].append(m["method"]), out["conditions_standing"].append(", ".join(m["candidates"]) or "none"), out["rests_on"].append(", ".join(m["grounds"]))
                out["pairs"].append("; ".join(f"{a} / {b}" for a, b in m["pairs"])), out["note"].append(m["note"])
            return [api.table("mill", out, title="Mill's methods"), api.note(f"{len(rows)} cases, {len(conds)} conditions." + c.left_out(dropped))]
    return Mill()
