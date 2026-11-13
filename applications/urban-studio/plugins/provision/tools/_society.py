"""What the tools of time, care and provision share: tables read by their columns into groups, and the domains asked through science.society.
Nothing of the domains' computation is done again here; what is done here is the reading of the person's tables and the wording of
what came back."""

from __future__ import annotations

import math
from collections import defaultdict

from science import society


def col(api, name, label, default, help=""):
    return api.param(name, label, default, kind="text", role="column", of="table", help=help)


def texts(t, name):
    if not name or not t.has(name):
        raise LookupError(f"the table '{t.name}' has no column '{name}'")

    def word(v):
        if v is None:
            return ""
        if isinstance(v, float):
            return str(int(v)) if v.is_integer() else f"{v:g}"
        return str(v).strip()
    return [word(v) for v in t.raw(name)]


def nums(t, name):
    if not name or not t.has(name):
        raise LookupError(f"the table '{t.name}' has no column '{name}'")
    return [float(x) if math.isfinite(x) else None for x in t.column(name)]


def keep(values, wanted):
    """The rows whose value is wanted (an empty wanted keeps all)."""
    wanted = [w.strip() for w in str(wanted or "").split(",") if w.strip()]
    return [not wanted or v in wanted for v in values]


def share(x):
    return f"{100 * x:.1f}%" if x is not None and x == x else "—"


def grouped(keys, *columns):
    """Rows grouped by a key: {key: [tuple of the columns' values], ...}."""
    out = defaultdict(list)
    for i, k in enumerate(keys):
        out[k].append(tuple(c[i] for c in columns))
    return out


__all__ = ["society", "col", "texts", "nums", "keep", "share", "grouped"]
