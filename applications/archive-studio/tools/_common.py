"""What the Archive Studio's tools share: tables read by their columns, rows with what a tool needs kept and the rest counted, and the
foundations asked through science.inquiry. Nothing of the computation is done again here; what is done here is the reading of a
person's tables and the wording of what came back."""

from __future__ import annotations

import math

from science import inquiry


def col(api, name, label, default="", help="", of="table"):
    return api.param(name, label, default, kind="text", role="column", of=of, help=help)


def cols(api, name, label, default="", help="", of="table"):
    return api.param(name, label, default, kind="text", role="columns", of=of, help=help)


def words(s) -> list[str]:
    """A list written in a parameter, separated by commas."""
    return [w.strip() for w in str(s or "").split(",") if w.strip()]


def need(t, names: list[str]) -> None:
    missing = [n for n in names if n and not t.has(n)]
    if missing:
        raise LookupError(f"the table '{t.name}' has no column {', '.join(repr(m) for m in missing)} (it has: {', '.join(t.columns)})")


def text(t, name: str) -> list[str]:
    def word(v):
        if v is None:
            return ""
        if isinstance(v, float):
            return "" if v != v else (str(int(v)) if v.is_integer() else f"{v:g}")
        return str(v).strip()
    return [word(v) for v in t.raw(name)]


def numbers(t, name: str) -> list[float]:
    return [float(x) for x in t.column(name)]


def complete(t, numeric: list[str], textual: list[str] = ()) -> tuple[list[int], int]:
    """The rows with a number in every numeric column and a word in every textual one, and how many were left out."""
    need(t, list(numeric) + list(textual))
    nums = {c: numbers(t, c) for c in numeric}
    txts = {c: text(t, c) for c in textual}
    rows = [i for i in range(t.rows) if all(math.isfinite(nums[c][i]) for c in numeric) and all(txts[c][i] for c in textual)]
    return rows, t.rows - len(rows)


def left_out(n: int, what: str = "rows") -> str:
    return f" {n} {what} without every value it needs {'was' if n == 1 else 'were'} left out." if n else ""


def fmt(x, digits: int = 3) -> str:
    return "—" if x is None or (isinstance(x, float) and not math.isfinite(x)) else f"{x:.{digits}f}"


__all__ = ["inquiry", "col", "cols", "words", "need", "text", "numbers", "complete", "left_out", "fmt"]
