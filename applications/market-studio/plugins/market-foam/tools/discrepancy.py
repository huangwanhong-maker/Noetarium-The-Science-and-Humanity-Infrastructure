"""Two mechanisms, one record: how much of their difference a record shows, and how much it hides."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402


def create(api):
    class Discrepancy(api.BoardAnalysis):
        id = "discrepancy"
        title = "Two mechanisms, one record"
        description = ("Two accounts of how histories come about, each a probability for every history, and what a record keeps of a history (its outcome, say). "
                       "Their whole difference splits in two: what the record shows, and what it hides. Collecting more of the same record sharpens only the first; "
                       "the second moves only when the record itself is made finer. Two procedures with one law of outcomes differ wholly in what outcomes hide.")
        params = [api.param("table", "Histories", "histories", kind="text", role="table", help="a row for each history that can happen"),
                  api.param("p", "Probability under the first", "p", kind="text", role="column", of="table"),
                  api.param("q", "Probability under the second", "q", kind="text", role="column", of="table"),
                  api.param("record", "What the record keeps", "outcome", kind="text", role="columns", of="table", help="one column, or several with commas between: histories alike in them leave the same record")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            for column in (params.get("p"), params.get("q")):
                if not column or not t.has(column):
                    raise LookupError(f"say which columns of '{t.name}' are the two probabilities (it has: {', '.join(t.columns)})")
            kept = [c.strip() for c in str(params.get("record") or "").split(",") if c.strip()]
            if not kept or any(not t.has(c) for c in kept):
                raise LookupError(f"say which columns of '{t.name}' the record keeps (it has: {', '.join(t.columns)})")
            p, q = t.column(params["p"]), t.column(params["q"])
            if not (np.isfinite(p).all() and np.isfinite(q).all()) or abs(p.sum() - 1) > 1e-6 or abs(q.sum() - 1) > 1e-6:
                raise ValueError(f"each column of probabilities adds up to one over the histories (they add up to {p.sum():.6g} and {q.sum():.6g})")
            parts = [t.text(c) for c in kept]
            seen = [" · ".join(x[i] for x in parts) for i in range(t.rows)]
            cells = list(dict.fromkeys(seen))
            a = r.ask("discrepancy", p=[float(x) / float(p.sum()) for x in p], q=[float(x) / float(q.sum()) for x in q], record=[cells.index(v) for v in seen])
            if a["infinite"]:
                note = ("The first gives weight to a history the second holds impossible: one such history, seen in full, decides between them. "
                        + ("The record shows it." if a["visible"] is None else "The record does not show it: what it shows of their difference is "
                           f"{a['visible']:.4f} nats."))
            else:
                share = a["visible"] / a["total"] if a["total"] > 0 else 0.0
                note = (f"The two differ by {a['total']:.4f} nats in all. A record of {', '.join(kept)} shows {a['visible']:.4f} of it ({100 * share:.1f}%) and hides {a['hidden']:.4f}. "
                        + ("They leave the same record: no amount of it tells them apart, and all their difference lies in how the outcome came about. " if a["visible"] < 1e-12 and a["total"] > 0 else "")
                        + "More of the same record sharpens only what it shows; what it hides moves only with a finer record, or with an intervention that makes the two behave apart.")
            return [
                api.table("shown", {"record": cells, "under_the_first": np.round(a["record_p"], 8), "under_the_second": np.round(a["record_q"], 8),
                                    "hidden_within": [None if v is None else round(v, 6) for v in a["hidden_at"]]},
                          title="What the record shows: the law of each record under the two, and the difference hidden within it", roles={"name": "record", "amount": "under_the_first"},
                          show={"kind": "chart", "chart": "bar", "x": "record", "y": "hidden_within", "how": "sum"}),
                api.table("parts", {"part": ["in all", "the record shows", "the record hides"], "nats": [a["total"], a["visible"], a["hidden"]]},
                          title="The difference between the two, and its two parts", roles={"name": "part", "amount": "nats"}),
                api.note(note),
            ]
    return Discrepancy()
