"""What the tools of appraisal and futures share: tables read by their columns, and nomos asked through science.appraisal. Nothing of
the computation is done again here; what is done here is the reading of the person's tables and the wording of what came back."""

from __future__ import annotations

import math

from science import appraisal


def col(api, name, label, default, help=""):
    return api.param(name, label, default, kind="text", role="column", of="table", help=help)


def texts(t, name, optional=False):
    if not name or not t.has(name):
        if optional:
            return [""] * t.rows
        raise LookupError(f"the table '{t.name}' has no column '{name}'")

    def word(v):
        if v is None:
            return ""
        if isinstance(v, float):
            return "" if v != v else (str(int(v)) if v.is_integer() else f"{v:g}")
        return str(v).strip()
    return [word(v) for v in t.raw(name)]


def nums(t, name, optional=False):
    if not name or not t.has(name):
        if optional:
            return [None] * t.rows
        raise LookupError(f"the table '{t.name}' has no column '{name}'")
    return [float(x) if math.isfinite(x) else None for x in t.column(name)]


def words(s):
    """A list written in a parameter, separated by commas."""
    return [w.strip() for w in str(s or "").split(",") if w.strip()]


__all__ = ["appraisal", "col", "texts", "nums", "words"]
