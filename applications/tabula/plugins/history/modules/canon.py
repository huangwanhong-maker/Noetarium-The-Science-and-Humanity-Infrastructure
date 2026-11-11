"""The events of a history: the occasions of one of the history domain's canons, each with its date, its place and those present."""

import json
import re
from pathlib import Path

CANONS = Path(__file__).resolve().parents[5] / "domains" / "history" / "canons"
DATE = re.compile(r"^(-?\d{1,6})(?:-(\d{2})(?:-(\d{2}))?)?$")


def reading(date: str):
    """A date of the codebook (EDTF, level 1: a year, a month or a day, or an interval of two, perhaps marked uncertain or approximate) as its
    beginning and its end as written, and the decimal year of its beginning."""
    parts = [p.strip().rstrip("?~%") for p in str(date or "").split("/")]
    parts = [p for p in parts if p and p != ".."]
    if not parts or not DATE.match(parts[0]):
        return None, None, None
    m = DATE.match(parts[0])
    year = int(m[1]) + ((int(m[2]) - 1) / 12.0 if m[2] else 0.0) + ((int(m[3]) - 1) / 365.0 if m[3] else 0.0)
    until = parts[1] if len(parts) > 1 and DATE.match(parts[1]) else None
    return parts[0], until, year


def create(api):
    names = sorted(p.stem for p in CANONS.glob("*.jsonl")) if CANONS.is_dir() else []

    class Canon(api.BoardAnalysis):
        id = "canon"
        title = "The events of a history"
        description = ("The occasions of one of the history domain's canons, each with its date as its source writes it, its place, its kind and those "
                       "present; and the beings of the canon with their years. A canon is a record of a history's classical facts: these were drafted by a "
                       "language model from general knowledge, and none has yet been checked against the reference it names.")
        params = [api.param("canon", "Canon", names[0] if names else "", kind="choice", options=names or [""]),
                  api.param("holding", "Only those that hold (empty: all)", "", kind="text"),
                  api.param("name", "Name its table", "events", kind="text")]

        def run(self, board, params, context):
            path = CANONS / f"{params['canon']}.jsonl"
            if not path.is_file():
                raise FileNotFoundError(f"the history domain has no canon {params['canon']!r}")
            records = [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines() if line.strip()]
            head = next((r for r in records if r.get("kind") == "canon"), {})
            beings = {r["id"]: r for r in records if r.get("kind") == "being"}
            want = params["holding"].strip().lower()
            rows = []
            for r in records:
                if r.get("kind") != "occasion":
                    continue
                when, until, year = reading(r.get("date"))
                if when is None:
                    continue
                who = ", ".join(beings.get(b, {}).get("name", b) for b, _role in r.get("present", []))
                if want and want not in f"{r.get('label', '')} {r.get('type', '')} {r.get('place', '')} {who}".lower():
                    continue
                rows.append((year, when, until, r.get("label", ""), r.get("type", ""), r.get("place", ""), who, r.get("confidence"), r.get("calendar", "")))
            rows.sort(key=lambda row: row[0])
            lived = {b: reading(r.get("lived")) for b, r in beings.items()}
            results = [
                api.table(params["name"] or "events", {
                    "when": [r[1] for r in rows], "until": [r[2] for r in rows], "year": [r[0] for r in rows], "label": [r[3] for r in rows], "kind": [r[4] for r in rows],
                    "place": [r[5] for r in rows], "who": [r[6] for r in rows], "confidence": [r[7] for r in rows], "calendar": [r[8] for r in rows],
                }, title=head.get("title", params["canon"]), source=f"the history domain's canon {params['canon']} (a seed, not yet checked against its sources)",
                    labels={"when": "its date, as its source writes it", "year": "the year of its beginning, as a number", "calendar": "the calendar its date is written in"}),
                api.table(f"{params['name'] or 'events'}_beings", {
                    "name": [r.get("name", b) for b, r in beings.items()], "kind": [r.get("type", "") for r in beings.values()],
                    "born": [lived[b][0] for b in beings], "died": [lived[b][1] for b in beings],
                }, title="The beings of the canon"),
            ]
            kept = f" that hold '{params['holding']}'" if want else ""
            return results + [api.note(f"{len(rows)} occasions{kept} of {head.get('title', params['canon'])}, among {len(beings)} beings. Drafted by a language model "
                                       "and not yet checked against its sources; dates are as their sources write them, in the calendar each row names.")]

    return Canon()
