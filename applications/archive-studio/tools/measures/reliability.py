"""Reliability of a scale: Cronbach's α, each item's part, and the split half."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Reliability(api.BoardAnalysis):
        id = "reliability"
        title = "Reliability of a scale"
        description = ("How consistently the items of a scale measure one thing: Cronbach's α (1951), each item's correlation with the rest and the α the scale "
                       "would have without it, and the split half stepped up by Spearman and Brown. Classical test theory, nomos's psychometrics.")
        params = [api.param("table", "Responses", "", kind="text", role="table"), c.cols(api, "items", "Items", help="numbers, one column an item")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            items = c.words(params["items"])
            if len(items) < 2:
                raise ValueError("a scale has at least two items")
            rows, dropped = c.complete(t, items)
            values = {k: c.numbers(t, k) for k in items}
            r = c.inquiry.ask("reliability", items=items, scores=[[values[k][i] for k in items] for i in rows])
            a = r["alpha"]
            word = "high" if a is not None and a >= 0.8 else "acceptable" if a is not None and a >= 0.7 else "low"
            return [api.table("items", {"item": items, "rest_correlation": r["rest_correlation"], "alpha_without": r["alpha_without"]}, title="Each item's part in the scale"),
                    api.note(f"Cronbach's α is {c.fmt(a)} over {len(rows)} respondents ({word} by the usual reading, 0.7 and 0.8); the split half, stepped up, is "
                             f"{c.fmt(r['split_half'])}." + c.left_out(dropped, "respondents"))]
    return Reliability()
