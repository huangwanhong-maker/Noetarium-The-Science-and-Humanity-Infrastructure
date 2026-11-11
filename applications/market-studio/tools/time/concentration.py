"""Concentration over time: how the sellers' shares stand, period by period."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parents[1] / "market"))
import _market as m  # noqa: E402
from science import markets  # noqa: E402


def create(api):
    class Concentration(api.BoardAnalysis):
        id = "concentration"
        title = "Concentration over time"
        description = ("For each period of a record, how concentrated its sellers are: the Herfindahl–Hirschman index, the share of the four largest, and the "
                       "number of equal sellers that would be as concentrated. By an amount where the record has one, else by the number of outlets.")
        params = [api.param("table", "Record", "record", kind="text", role="table", help="a row for each outlet in each period"),
                  api.param("seller", "Seller", "", kind="text", role="column", of="table", help="the column that names the seller; the name when left out"),
                  api.param("amount", "By", "", kind="text", role="column", of="table", help="a column of amounts (sales); the number of outlets when left out")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            when = t.column(m.role(t, "time", "its time"))
            seller, _ = m.sellers(t.text(m.column_or(t, params.get("seller"), "name", "its seller or its name")))
            seller = np.array(seller)
            amount = np.nan_to_num(t.column(params["amount"]), nan=0.0) if params.get("amount") and t.has(params["amount"]) else np.ones(t.rows)
            out = {"period": [], "outlets": [], "sellers": [], "hhi": [], "four_largest": [], "equal_sellers": []}
            periods = np.unique(when[np.isfinite(when)])
            sets = []
            for p in periods:
                rows = when == p
                held = np.bincount(seller[rows], weights=amount[rows])
                sets.append([float(v) for v in held[held > 0]])
                out["period"].append(float(p)), out["outlets"].append(int(rows.sum())), out["sellers"].append(len(sets[-1]))
            for a in markets.ask("structure", sets=sets)["sets"]:
                out["hhi"].append(round(a["hhi"], 6)), out["four_largest"].append(round(100 * a["cr4"], 3)), out["equal_sellers"].append(round(a["equivalent"], 1))
            return [
                api.table("concentration", out, title="The concentration among sellers, period by period", roles={"time": "period", "amount": "four_largest"},
                          show={"kind": "chart", "chart": "line", "x": "period", "y": "four_largest", "how": "mean"}),
                api.note(f"Over {len(periods)} periods the four largest sellers held {min(out['four_largest']):.1f}% to {max(out['four_largest']):.1f}% "
                         f"({'of the amounts' if params.get('amount') else 'of the outlets: the record carries no sales'})."),
            ]
    return Concentration()
