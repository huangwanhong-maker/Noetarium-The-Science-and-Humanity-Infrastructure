"""Openings and closings: a record of who was there in each period, read as lives."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parents[1] / "market"))
import _market as m  # noqa: E402


def create(api):
    class Turnover(api.BoardAnalysis):
        id = "turnover"
        title = "Openings and closings"
        description = ("A table that records, period by period, who was there (a census of outlets by year, say) read as lives: each thing with the period it is "
                       "first seen in and the last, and for each period how many were there, how many had opened and how many had closed. "
                       "A thing is the same from one period to the next when its identity is; one seen in the first period was open then, not opened then, "
                       "and one seen in the last has not closed.")
        params = [api.param("table", "Record", "record", kind="text", role="table", help="a row for each thing in each period it was there"),
                  api.param("identity", "The same thing when", "", kind="text", role="columns", of="table", help="the columns that make two rows the same thing; its identity and name when left out")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            when = t.column(m.role(t, "time", "its time"))
            roles = getattr(t, "roles", None) or {}
            wanted = [c.strip() for c in str(params.get("identity") or "").split(",") if c.strip()] or [c for c in (roles.get("key"), roles.get("name")) if c]
            if not wanted:
                raise LookupError(f"say what makes two rows of '{t.name}' the same thing: bind its identity or its name, or name the columns")
            parts = [[" ".join(v.upper().split()) for v in t.text(c)] for c in wanted]
            periods = np.unique(when[np.isfinite(when)])
            if len(periods) < 2:
                raise ValueError("a record of one period has no openings and closings: widen the slice's time")
            first, last = float(periods[0]), float(periods[-1])
            lives: dict = {}
            for i in range(t.rows):
                if not np.isfinite(when[i]):
                    continue
                key = tuple(p[i] for p in parts)
                life = lives.get(key)
                if life is None:
                    lives[key] = [when[i], when[i], i, 1]
                else:
                    life[0], life[1], life[3] = min(life[0], when[i]), max(life[1], when[i]), life[3] + 1
            rows = [life[2] for life in lives.values()]
            began, ended = np.array([life[0] for life in lives.values()]), np.array([life[1] for life in lives.values()])
            step = float(np.median(np.diff(periods)))
            closed = ended < last                      # seen for the last time before the record ends
            until = np.where(closed, ended + step, np.nan)
            columns, out_roles, _ = m.place_columns(t, rows)
            names = [t.text(roles["name"])[i] for i in rows] if roles.get("name") else [" · ".join(k) for k in lives]
            per = {"period": periods, "there": [int(((began <= p) & (ended >= p)).sum()) for p in periods],
                   "opened": [int((began == p).sum()) if p > first else None for p in periods],
                   "closed": [int((closed & (ended + step == p)).sum()) if p > first else None for p in periods]}
            per["net"] = [None if o is None else o - c for o, c in zip(per["opened"], per["closed"])]
            seen = np.array([life[3] for life in lives.values()])
            gaps = int((seen < (ended - began) / step + 1).sum())
            return [
                api.table("lives", {"name": names, **columns, "from": began, "until": until, "periods": (ended - began) / step + 1, "closed": closed.astype(int)},
                          title="Each thing with the period it was first and last seen in", roles={**out_roles, "name": "name", "time": "from", "until": "until", "amount": "periods"},
                          show={"kind": "layer", "layer": "points", "colour": "from"}),
                api.table("turnover", per, title="How many were there, opened and closed, period by period", roles={"time": "period", "amount": "there"},
                          show={"kind": "chart", "chart": "line", "x": "period", "y": "there", "how": "sum"}),
                api.note(f"{len(lives):,} things over {len(periods)} periods, {first:g} to {last:g}: {int(closed.sum()):,} were seen for the last time before the record ends, "
                         f"{int((began > first).sum()):,} for the first time after it begins. {gaps:,} were away for a period or more between their first and last, and are counted as there throughout. "
                         "Observed presence, not sales; a thing that changed its identity (a new name, say) counts as one closing and another opening."),
            ]
    return Turnover()
