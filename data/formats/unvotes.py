"""Recorded votes of the United Nations General Assembly, as the Dag Hammarskjöld Library publishes them, read into a matrix of
members by resolutions (ADR 0132; the API's ``ideal_points``, tyche's ``latent``).

The Library's voting data (digitallibrary.un.org, record 4060887) is one CSV, a row a member's vote on an adopted resolution:
``undl_id, ms_code, ms_name, ms_vote, date, session, resolution, draft, committee_report, meeting, title, agenda_title, subjects,
vote_note, total_yes, total_no, total_abstentions, total_non_voting, total_ms, undl_link``. A vote is ``Y`` (yes), ``N`` (no),
``A`` (abstention) or ``X`` (present and not voting, or absent); a member not yet a member, or not listed, has no row. Only
resolutions adopted by a recorded vote are in it: drafts that failed are not (the Library's own note).

Read into a matrix, a vote is 1 for yes, 0 for no, and missing otherwise. How an abstention is read is declared, since practice
differs: ``missing`` (the default: an abstention says neither), or ``no`` (an abstention withholds support). Nothing else is
inferred; the raw codes are kept beside the matrix.

The file is large (some 370 MB in 2026) and is read as a stream, a session or a period at a time. Members are given by their ISO
3166-1 alpha-3 codes, as the Library gives them; the names a work shows follow the data library's naming policy
(``data/products/naming.py``), not the file's.

Standard library only.

    from unvotes import read_votes
    v = read_votes(path, sessions={79})
    v["members"], v["items"], v["votes"], v["codes"]

Reference. United Nations Dag Hammarskjöld Library, "United Nations General Assembly voting data", UN Digital Library, record
4060887 (https://digitallibrary.un.org/record/4060887), the file of 15 October 2026.
"""

from __future__ import annotations

import csv
import io
from pathlib import Path

CODES = {"Y": "yes", "N": "no", "A": "abstention", "X": "not voting"}


def _rows(source):
    if isinstance(source, (str, Path)) and Path(source).exists():
        with open(source, encoding="utf-8", newline="") as f:
            yield from csv.DictReader(f)
    else:
        yield from csv.DictReader(io.StringIO(source) if isinstance(source, str) else source)


def read_votes(source, sessions=None, since: str = "", until: str = "", abstention: str = "missing", subjects: str = "") -> dict:
    """Members by resolutions: votes as 1 (yes), 0 (no) or None, with the raw codes, for the sessions (a set of numbers) or the
    dates [since, until] asked for; ``subjects`` keeps only resolutions whose subject headings contain it."""
    if abstention not in ("missing", "no"):
        raise ValueError("an abstention is read as 'missing' or as 'no'")
    items: dict[str, dict] = {}
    members: dict[str, str] = {}
    cast: dict[tuple[str, str], str] = {}
    for row in _rows(source):
        try:
            session = int(float(row.get("session") or 0))
        except ValueError:
            continue
        date = (row.get("date") or "")[:10]
        if sessions is not None and session not in sessions:
            continue
        if since and date < since:
            continue
        if until and date > until:
            continue
        if subjects and subjects.lower() not in (row.get("subjects") or "").lower():
            continue
        rid = row.get("undl_id") or row.get("resolution") or ""
        code = (row.get("ms_code") or "").strip()
        if not rid or not code:
            continue
        if rid not in items:
            def count(name):
                try:
                    return int(float(row.get(name) or 0))
                except ValueError:
                    return None
            items[rid] = {"id": rid, "resolution": row.get("resolution") or "", "date": date, "session": session, "title": row.get("title") or "",
                          "subjects": row.get("subjects") or "", "draft": row.get("draft") or "", "link": row.get("undl_link") or "",
                          "totals": {"yes": count("total_yes"), "no": count("total_no"), "abstentions": count("total_abstentions"),
                                     "not_voting": count("total_non_voting"), "members": count("total_ms")}}
        members.setdefault(code, (row.get("ms_name") or "").strip())
        cast[(code, rid)] = (row.get("ms_vote") or "").strip().upper()
    order = sorted(items, key=lambda r: (items[r]["date"], items[r]["resolution"], r))
    codes = sorted(members)
    raw = [[cast.get((m, r), "") for r in order] for m in codes]

    def value(c):
        if c == "Y":
            return 1
        if c == "N":
            return 0
        if c == "A" and abstention == "no":
            return 0
        return None

    votes = [[value(c) for c in row] for row in raw]
    return {"members": codes, "file_names": members, "items": [items[r] for r in order], "votes": votes, "codes": raw,
            "abstention": abstention, "source": "UN Digital Library, record 4060887"}


def contested(record: dict, least: int = 1) -> dict:
    """The record with only the resolutions on which at least ``least`` members voted on the losing side: a unanimous vote tells
    nothing about where members stand apart (it fits every line equally)."""
    keep = []
    for j, item in enumerate(record["items"]):
        yes = sum(1 for row in record["votes"] if row[j] == 1)
        no = sum(1 for row in record["votes"] if row[j] == 0)
        if min(yes, no) >= least:
            keep.append(j)
    out = dict(record)
    out["items"] = [record["items"][j] for j in keep]
    out["votes"] = [[row[j] for j in keep] for row in record["votes"]]
    out["codes"] = [[row[j] for j in keep] for row in record["codes"]]
    return out
