"""Dividing what is short: an estate against claims that exceed it, by every rule, and what the order alone can move."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402


def create(api):
    class Division(api.BoardAnalysis):
        id = "division"
        title = "Dividing what is short"
        description = ("An estate that does not cover the claims on it. Paid one after another, the order decides: the tool gives the least and the most each claimant "
                       "can receive by order alone. Two ways of taking the order out give two different rules: drawing it by lot (each receives the mean over all orders), "
                       "and paying all together (in proportion to the claims, or at equal rates). Beside them, equal losses and the Talmud's rule.")
        params = [api.param("table", "Claims", "claims", kind="text", role="table", help="a row for each claimant"),
                  api.param("claimant", "Claimant", "claimant", kind="text", role="column", of="table"),
                  api.param("claim", "Claim", "claim", kind="text", role="column", of="table"),
                  api.param("estate", "The estate", 200.0, minimum=0, maximum=1e15)]

        def run(self, board, params, context):
            t = board.table(params["table"])
            for column in (params.get("claimant"), params.get("claim")):
                if not column or not t.has(column):
                    raise LookupError(f"say which columns of '{t.name}' are the claimant and the claim (it has: {', '.join(t.columns)})")
            names, claims = t.text(params["claimant"]), t.column(params["claim"])
            if not np.isfinite(claims).all() or (claims < 0).any():
                raise ValueError("every claimant has a claim that is not negative")
            estate, total = float(params["estate"]), float(claims.sum())
            if estate >= total:
                raise ValueError(f"the estate ({estate:g}) covers the claims ({total:g}): everyone is paid in full, and no rule is needed")
            a = r.ask("division", estate=estate, claims=[float(c) for c in claims])
            columns = {"claimant": names, "claim": claims, "least_by_order": a["least"], "most_by_order": a["greatest"]}
            if a["random_arrival"]:
                columns["order_by_lot"] = np.round(a["random_arrival"], 6)
            columns.update({"together_by_claim": np.round(a["proportional"], 6), "together_alike": np.round(a["equal_awards"], 6),
                            "equal_losses": np.round(a["equal_losses"], 6), "talmud": np.round(a["talmud"], 6)})
            swing = np.array(a["greatest"]) - np.array(a["least"])
            most = int(np.argmax(swing))
            return [
                api.table("division", columns, title=f"An estate of {estate:g} against claims of {total:g}, by each rule", roles={"name": "claimant", "amount": "together_by_claim"},
                          show={"kind": "chart", "chart": "bar", "x": "claimant", "y": "together_by_claim", "how": "sum"}),
                api.note(f"The estate covers {100 * estate / total:.1f}% of the claims. The order of payment alone moves {names[most]} by {swing[most]:g}, between {a['least'][most]:g} and {a['greatest'][most]:g}. "
                         "Drawing the order by lot and paying all together are both neutral between the orders, and they are different rules; paying together is itself two rules, "
                         "by the rates at which each is paid. Which is just is not the record's to say."
                         + ("" if a["random_arrival"] else " With more than 24 claimants the mean over all orders is not computed.")),
            ]
    return Division()
