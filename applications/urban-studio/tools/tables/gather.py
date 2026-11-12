"""Count or add up the rows of a table by the values of one or two of its columns."""

import numpy as np


def create(api):
    class Gather(api.BoardAnalysis):
        id = "gather"
        title = "Count or add up, by a column"
        description = "The rows of a table gathered by the values of a column (and of a second): how many there are, or the sum, the mean, the least or the greatest of another column."
        params = [api.param("table", "Table", "", kind="text", role="table"),
                  api.param("by", "By", "", kind="text", role="column", of="table"),
                  api.param("then", "And by", "", kind="text", role="column", of="table", help="a second column, if wanted"),
                  api.param("how", "Take", "count", kind="choice", options=["count", "sum", "mean", "least", "greatest"]),
                  api.param("of", "Of", "", kind="text", role="column", of="table", help="the column to add up; not needed for the count"),
                  api.param("name", "Call it", "gathered", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            by = [c for c in (params.get("by"), params.get("then")) if c]
            if not by:
                raise ValueError("say which column to gather by")
            how = params.get("how", "count")
            keys = [t.raw(c) for c in by]
            values = t.column(params["of"]) if how != "count" else np.ones(t.rows)
            groups: dict = {}
            for i in range(t.rows):
                key = tuple(k[i] for k in keys)
                if any(v is None for v in key) or not np.isfinite(values[i]):
                    continue
                groups.setdefault(key, []).append(values[i])
            take = {"count": len, "sum": sum, "mean": lambda v: sum(v) / len(v), "least": min, "greatest": max}[how]
            ordered = sorted(groups, key=lambda k: tuple((0, x) if isinstance(x, (int, float)) else (1, str(x)) for x in k))
            label = "count" if how == "count" else f"{how}_of_{params['of']}"
            out = {c: [k[n] for k in ordered] for n, c in enumerate(by)}
            out[label] = [float(take(groups[k])) for k in ordered]
            roles = getattr(t, "roles", None) or {}
            given = {"amount": label, **({"time": by[0]} if roles.get("time") == by[0] else {"category": by[0]})}
            return [api.table(params.get("name") or "gathered", out, title=f"{t.title or t.name}: {label.replace('_', ' ')} by {' and '.join(by)}", roles=given,
                              show={"kind": "chart", "chart": "line" if "time" in given else "bar", "x": by[0], "y": label, "how": "sum", **({"split": by[1]} if len(by) > 1 else {})})]
    return Gather()
