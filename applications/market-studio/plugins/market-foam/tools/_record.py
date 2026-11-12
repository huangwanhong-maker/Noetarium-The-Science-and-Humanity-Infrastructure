"""What the plug-in's tools share: a record of movements read from a table, as the engine takes it.

A record is rows of movements: at a time, a quantity of a kind passes from one subject to another. Rows that share an event are
one event (an exchange is two rows in opposite directions). Nothing is asked of the table's column names: the tool's
parameters say which column is which. The computations are the markets domain's, asked of the engine through science.markets.
"""

from __future__ import annotations

import datetime as dt

import numpy as np

from science import markets


def common(api) -> list:
    """The parameters that say where a record's columns are."""
    return [
        api.param("table", "Record", "record", kind="text", role="table", help="a row for each movement: who gave what to whom, when"),
        api.param("time", "When", "time", kind="text", role="column", of="table", help="a number, or a date"),
        api.param("giver", "From", "from", kind="text", role="column", of="table"),
        api.param("taker", "To", "to", kind="text", role="column", of="table"),
        api.param("kind", "Of what", "kind", kind="text", role="column", of="table", help="the kind moved: money, a good, a claim"),
        api.param("quantity", "How much", "quantity", kind="text", role="column", of="table"),
        api.param("event", "Together with", "event", kind="text", role="column", of="table", help="rows that share this are one event (the two sides of an exchange); each row alone when left out"),
    ]


def opening(api):
    return api.param("opening", "Opening positions", "", kind="text", role="table", help="a table of where each stood at the beginning, in columns subject, kind and quantity; nothing when left out")


def clock(values: list) -> tuple[np.ndarray, str]:
    """Times as numbers: numbers as they are, dates as days."""
    out, days = [], False
    for v in values:
        if isinstance(v, (int, float)) and not isinstance(v, bool):
            out.append(float(v))
            continue
        try:
            out.append(float(str(v)))
        except ValueError:
            try:
                stamp = dt.datetime.fromisoformat(str(v).strip().replace("Z", "+00:00"))
            except ValueError as error:
                raise ValueError(f"'{v}' is neither a number nor a date") from error
            if stamp.tzinfo is None:
                stamp = stamp.replace(tzinfo=dt.timezone.utc)
            out.append(stamp.timestamp() / 86400.0)
            days = True
    return np.array(out, dtype=float), "days" if days else "the record's own unit of time"


def stated(board, params: dict, booked: bool = False) -> dict:
    """The record a tool's parameters describe, read from the board's table: what the engine is asked, and the names beside it."""
    t = board.table(params["table"])
    need = {k: params.get(k) for k in ("time", "giver", "taker", "kind", "quantity")}
    for what, column in need.items():
        if not column or not t.has(column):
            raise LookupError(f"say which column of '{t.name}' is the {what} (it has: {', '.join(t.columns)})")
    if t.rows == 0:
        raise ValueError(f"the slice holds no row of '{t.name}'")
    when, unit = clock(t.raw(need["time"]))
    quantity = t.column(need["quantity"])
    giver, taker, kind = t.text(need["giver"]), t.text(need["taker"]), t.text(need["kind"])
    keep = np.isfinite(when) & np.isfinite(quantity) & np.array([a != b and a != "" and b != "" for a, b in zip(giver, taker)])
    dropped = int((~keep).sum())
    together = t.text(params["event"]) if params.get("event") and t.has(params["event"]) else [str(i) for i in range(t.rows)]
    # Events in time; the rows of one event together, at its first row's time.
    first: dict = {}
    for i in np.flatnonzero(keep):
        first.setdefault(together[i], (when[i], len(first)))
    order = sorted(np.flatnonzero(keep), key=lambda i: (first[together[i]][0], first[together[i]][1]))
    index = {name: n for n, name in enumerate(sorted(first, key=lambda name: first[name]))}
    subjects = sorted({giver[i] for i in order} | {taker[i] for i in order})
    kinds = sorted({kind[i] for i in order})
    s_of, k_of = {n: i for i, n in enumerate(subjects)}, {n: i for i, n in enumerate(kinds)}
    record = {
        "subjects": len(subjects), "kinds": len(kinds),
        "event": [index[together[i]] for i in order], "time": [float(first[together[i]][0]) for i in order],
        "from": [s_of[giver[i]] for i in order], "to": [s_of[taker[i]] for i in order],
        "kind": [k_of[kind[i]] for i in order], "quantity": [float(quantity[i]) for i in order],
    }
    record["begin"], record["end"] = record["time"][0], record["time"][-1]
    # Where each subject stood in each kind at the beginning, when a table says so; else every position begins at nothing
    # and is read as its change since.
    opening = params.get("opening")
    if opening and opening in board.tables:
        o = board.table(opening)
        for column in ("subject", "kind", "quantity"):
            if not o.has(column):
                raise LookupError(f"the opening positions in '{o.name}' are columns subject, kind and quantity")
        start = [[0.0] * len(kinds) for _ in subjects]
        for who, what, much in zip(o.text("subject"), o.text("kind"), o.column("quantity")):
            if who in s_of and what in k_of and np.isfinite(much):
                start[s_of[who]][k_of[what]] = float(much)
        record["opening"] = start
    if booked:
        record["booked"] = [1] * len(kinds)
    return {"record": record, "subjects": subjects, "kinds": kinds, "events": list(index), "unit": unit, "dropped": dropped, "table": t,
            "rows": [int(i) for i in order]}


def ask(op: str, **request) -> dict:
    return markets.ask(op, **request)


def kind_index(s: dict, name: str | None, what: str = "kind") -> int:
    """A kind of the record by its name; the one moved most often when none is named."""
    if name:
        if name not in s["kinds"]:
            raise LookupError(f"the record has no {what} '{name}' (it has: {', '.join(s['kinds'])})")
        return s["kinds"].index(name)
    counts = np.bincount(s["record"]["kind"], minlength=len(s["kinds"]))
    return int(np.argmax(counts))
