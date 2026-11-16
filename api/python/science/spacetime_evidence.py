"""Evidence kept in space and time, for scripts in Python and the works' servers (ADR 0123): what is kept and whether it is still
what it was (archeion), what it is worth for a claim about a place and a time (tekmerion), and what to do about it and what to look
at first (tyche), asked of the engine a work has built.

The computations are composed in ``api/cpp/science/spacetime_evidence.hpp``, which documents the archive it takes
(``noetarium.evidence/1``) and its operations:

    from science import spacetime_evidence as se
    se.ask("all", archive=se.prepared(archive))     # keep, weigh, and decide and timeline where the archive states them

The engine is a program built by a work: the Urban Studio's plug-in *Evidence in space and time* builds
``data/cache/engines/urban-studio/evidence/engine``; ``NOETARIUM_EVIDENCE_ENGINE`` names another.

What is here beside the asking is what an archive needs before it is asked and that is no computation of the foundations':

- ``instant`` reads a time as a person writes it (ISO 8601, a date alone, a year and month, a year) into seconds since 1970 in
  UTC, and ``span`` the interval a coarse time covers (a date is its day, a month its month);
- ``prepared`` gives the engine an archive whose times are numbers and whose holdings' digests are observed now from the files
  (``digest``, the SHA-256 of a file's bytes, which archeion's own SHA-256 is held to);
- ``read_table`` reads items, sources, claims or holdings from a table's rows as a person keeps them in a spreadsheet;
- ``calibrate`` estimates a kind of report's channel (its hit and false-alarm rates) against a reference kind in the archive's own
  items, with tyche's intervals for a proportion: a count, said to be one, never a declared credibility.
"""

from __future__ import annotations

import calendar
import datetime as dt
import hashlib
import math
import re
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/evidence", env="NOETARIUM_EVIDENCE_ENGINE")
SCHEMA = "noetarium.evidence/1"
UTC = dt.timezone.utc


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


# ---- Times ---------------------------------------------------------------------------------------------------------------------------


def span(text) -> tuple[float, float] | None:
    """The interval a written time covers, in seconds since 1970 (UTC): a year, a year and month, a date, or an instant (ISO 8601;
    without a zone it is taken as UTC). A number is an instant already. None for nothing; ValueError for what is not a time."""
    if text is None or (isinstance(text, float) and math.isnan(text)):
        return None
    if isinstance(text, (int, float)):
        return float(text), float(text)
    s = str(text).strip()
    if not s:
        return None
    m = re.fullmatch(r"(\d{4})(?:-(\d{2})(?:-(\d{2}))?)?", s)
    if m:
        y, mo, d = int(m.group(1)), m.group(2), m.group(3)
        if d:
            a = dt.datetime(y, int(mo), int(d), tzinfo=UTC)
            b = a + dt.timedelta(days=1)
        elif mo:
            a = dt.datetime(y, int(mo), 1, tzinfo=UTC)
            b = a + dt.timedelta(days=calendar.monthrange(y, int(mo))[1])
        else:
            a, b = dt.datetime(y, 1, 1, tzinfo=UTC), dt.datetime(y + 1, 1, 1, tzinfo=UTC)
        return a.timestamp(), b.timestamp()
    try:
        t = dt.datetime.fromisoformat(s.replace("Z", "+00:00").replace(" ", "T", 1))
    except ValueError:
        raise ValueError(f"'{s}' is not a time (write 2024, 2024-03, 2024-03-17 or 2024-03-17T14:30)") from None
    if t.tzinfo is None:
        t = t.replace(tzinfo=UTC)
    return t.timestamp(), t.timestamp()


def instant(text) -> float | None:
    """The beginning of what a written time covers."""
    s = span(text)
    return None if s is None else s[0]


def written(seconds: float | None) -> str:
    """A time in seconds since 1970 as it is read: a date, with the hour when it is not midnight."""
    if seconds is None or not math.isfinite(seconds):
        return ""
    t = dt.datetime.fromtimestamp(seconds, UTC)
    return t.strftime("%Y-%m-%d") if (t.hour, t.minute, t.second) == (0, 0, 0) else t.strftime("%Y-%m-%d %H:%M")


def interval(start, end=None) -> tuple[float | None, float | None]:
    """From the beginning of `start` to the end of `end` (or of `start`, when no end is written)."""
    a = span(start)
    b = span(end) if end not in (None, "") else None
    if a is None and b is None:
        return None, None
    lo = a[0] if a else b[0]
    hi = b[1] if b else a[1]
    if hi < lo:
        raise ValueError(f"a time ends no earlier than it begins: {start} to {end}")
    return lo, hi


# ---- Holdings ------------------------------------------------------------------------------------------------------------------------


def digest(path) -> str:
    """The SHA-256 of a file's bytes, in hexadecimal; '' when the file is not there."""
    p = Path(path)
    if not p.is_file():
        return ""
    h = hashlib.sha256()
    with p.open("rb") as f:
        for block in iter(lambda: f.read(1 << 20), b""):
            h.update(block)
    return h.hexdigest()


def prepared(archive: dict, folder=None) -> dict:
    """The archive as the engine takes it: every time a number, and each holding's digest observed now from its file under
    `folder` (where a folder is given and the holding names a file). The archive given is left as it was."""
    a = {k: (list(v) if isinstance(v, list) else v) for k, v in archive.items()}
    for key in ("items", "claims"):
        out = []
        for x in archive.get(key, []) or []:
            y = dict(x)
            lo, hi = interval(y.get("from"), y.get("until"))
            y["from"], y["until"] = lo, hi
            for name in ("lat", "lon", "radius", "prior", "e_given_h", "e_given_not_h"):
                if name in y and (y[name] is None or y[name] == ""):
                    del y[name]
            out.append(y)
        a[key] = out
    if folder is not None:
        hs = []
        for h in archive.get("holdings", []) or []:
            y = dict(h)
            if y.get("name"):
                y["observed"] = digest(Path(folder) / Path(str(y["name"])).name)
            hs.append(y)
        a["holdings"] = hs
    return a


# ---- Tables ---------------------------------------------------------------------------------------------------------------------------

NUMBERS = {"lat", "lon", "radius", "prior", "e_given_h", "e_given_not_h", "hit", "false_alarm", "sensitivity", "objectivity", "veracity"}
LISTS = {"claims", "kinds"}


def read_table(rows: list[dict], what: str) -> list[dict]:
    """Items, sources, claims or holdings from a table's rows: empty cells left out, numbers read as numbers, a cell of several
    values (`claims`, `kinds`) cut at semicolons. ValueError naming the row and the column that does not read."""
    out = []
    for n, row in enumerate(rows, start=1):
        x = {}
        for key, value in row.items():
            k = str(key).strip().lower()
            if value is None or (isinstance(value, float) and math.isnan(value)) or str(value).strip() == "":
                continue
            if k in NUMBERS:
                try:
                    x[k] = float(value)
                except (TypeError, ValueError):
                    raise ValueError(f"row {n} of the {what}: {k} is a number, not {value!r}") from None
            elif k in LISTS:
                x[k] = [w.strip() for w in str(value).split(";") if w.strip()]
            else:
                x[k] = value.strip() if isinstance(value, str) else (str(int(value)) if isinstance(value, float) and value.is_integer() else value)
        if not x:
            continue
        if not x.get("id"):
            raise ValueError(f"row {n} of the {what} has no id")
        x["id"] = str(x["id"])
        out.append(x)
    ids = [x["id"] for x in out]
    twice = sorted({i for i in ids if ids.count(i) > 1})
    if twice:
        raise ValueError(f"the {what} name {twice[0]} more than once")
    return out


# ---- A channel counted --------------------------------------------------------------------------------------------------------------


def calibrate(cells: list[dict], confidence: float = 0.95) -> dict:
    """A kind of report's channel counted against a reference: `cells` are units (a place in a period) each with ``reference``
    (True where the reference found the thing, False where it looked and found none) and ``reported`` (whether a report of the
    kind was made there). The hit rate is the share of the reference's findings that were reported, the false-alarm rate the share
    of its clearances that were; each with Wilson's interval and the exact one (tyche's, asked of the engine), and the likelihood
    ratios of a report and of a silence.

    It is a count on the units the reference looked at. Where the reference looks because of reports, both rates are too high and
    the count says so only if told: that is the caller's to state."""
    found = [c for c in cells if c.get("reference") is True]
    clear = [c for c in cells if c.get("reference") is False]
    return ask("rates", found=len(found), clear=len(clear), reported_found=sum(1 for c in found if c.get("reported")),
               reported_clear=sum(1 for c in clear if c.get("reported")), confidence=confidence)
