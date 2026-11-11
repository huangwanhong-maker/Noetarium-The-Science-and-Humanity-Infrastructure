"""Change between two times: what each thing, kind or area had at the beginning and at the end."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parents[1] / "market"))
import _market as m  # noqa: E402


def create(api):
    class Change(api.BoardAnalysis):
        id = "change"
        title = "Change between two times"
        description = ("For each value of a column (an area, a kind, a seller), what a record held in the first period of the slice and in the last: the count of its rows "
                       "or the sum of a column, the change, and the rate a period that would carry the one to the other.")
        params = [api.param("table", "Record", "", kind="text", role="table", help="a table with a time"),
                  api.param("by", "For each", "", kind="text", role="column", of="table", help="an area, a kind, a seller; the table's cell or kind when left out"),
                  api.param("of", "Add up", "", kind="text", role="column", of="table", help="a column to add up; the count of rows when left out"),
                  api.param("name", "Call it", "change", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            roles = getattr(t, "roles", None) or {}
            when = t.column(m.role(t, "time", "its time"))
            by = params.get("by") or roles.get("cell") or roles.get("category")
            if not by or not t.has(by):
                raise LookupError("say for each of what: a column of areas, kinds or sellers")
            periods = np.unique(when[np.isfinite(when)])
            if len(periods) < 2:
                raise ValueError("a record of one period has no change: widen the slice's time")
            first, last = float(periods[0]), float(periods[-1])
            value = np.nan_to_num(t.column(params["of"]), nan=0.0) if params.get("of") else np.ones(t.rows)
            a, b = {}, {}
            for k, w, v in zip(t.text(by), when, value):
                if w == first:
                    a[k] = a.get(k, 0.0) + v
                elif w == last:
                    b[k] = b.get(k, 0.0) + v
            names = sorted(set(a) | set(b), key=lambda k: -(b.get(k, 0.0) - a.get(k, 0.0)))
            before, after = np.array([a.get(k, 0.0) for k in names]), np.array([b.get(k, 0.0) for k in names])
            span = len(periods) - 1
            with np.errstate(all="ignore"):
                rate = np.where((before > 0) & (after > 0), 100 * ((after / np.where(before > 0, before, 1)) ** (1 / span) - 1), np.nan)
            what = f"sum of {params['of']}" if params.get("of") else "count"
            columns = {by: names, f"at_{first:g}": before, f"at_{last:g}": after, "change": after - before, "percent_a_period": [round(float(r), 3) if np.isfinite(r) else None for r in rate]}
            is_cell = bool(by == roles.get("cell") and (getattr(t, "places", None) or {}).get("rings"))
            return [api.table(params.get("name") or "change", columns, title=f"{t.title or t.name}: the {what} for each {by}, {first:g} to {last:g}",
                              roles={("cell" if is_cell else "category"): by, "amount": "change"}, places={"kind": "cells", "column": by, "rings": t.places["rings"]} if is_cell else None,
                              show={"kind": "layer", "layer": "areas", "colour": "change"} if is_cell else {"kind": "chart", "chart": "bar", "x": by, "y": "change", "how": "sum", "most": 20}),
                    api.note(f"From {first:g} to {last:g} the {what} went from {before.sum():,.0f} to {after.sum():,.0f} over {len(names):,} values of {by}: "
                             f"{int((after > before).sum()):,} rose, {int((after < before).sum()):,} fell.")]
    return Change()
