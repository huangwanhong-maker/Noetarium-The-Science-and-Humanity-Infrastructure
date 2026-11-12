"""What a table holds: each column's kind, how much of it is filled, and its values in brief."""

import collections

import numpy as np


def create(api):
    class Describe(api.BoardAnalysis):
        id = "describe"
        title = "What a table holds"
        description = "Each column of a table in a row: how many of its values are filled and how many differ; for numbers the least, the mean, the median and the greatest; for text the commonest value."
        params = [api.param("table", "Table", "", kind="text", role="table"),
                  api.param("name", "Call it", "described", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            out = {k: [] for k in ("column", "kind", "filled", "distinct", "least", "mean", "median", "greatest", "commonest")}
            for c in t.columns:
                raw = [v for v in t.raw(c) if v is not None and v != ""]
                out["column"].append(c), out["kind"].append(t.kinds.get(c, "text")), out["filled"].append(len(raw)), out["distinct"].append(len(set(raw)))
                v = t.column(c)
                v = v[np.isfinite(v)]
                numeric = t.kinds.get(c) == "number" and len(v) > 0
                out["least"].append(float(v.min()) if numeric else None), out["mean"].append(float(v.mean()) if numeric else None)
                out["median"].append(float(np.median(v)) if numeric else None), out["greatest"].append(float(v.max()) if numeric else None)
                out["commonest"].append(None if numeric or not raw else str(collections.Counter(raw).most_common(1)[0][0]))
            return [api.table(params.get("name") or "described", out, title=f"{t.title or t.name}: what its {len(t.columns)} columns hold, over {t.rows:,} rows", roles={"name": "column"})]
    return Describe()
