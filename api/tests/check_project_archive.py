"""Checks of a project's archive (api/python/science/project_archive.py) and of evidence as a kind of result (board.evidence), run by
api/tests/run-tests.mjs. In a project folder made here: evidence items are checked where they are made (a quantity, a value, an interval or
a level, a scale among the five); a run is kept under its digest once however often it recurs, read back and found altered when it is; a
file is kept by the digest of its bytes; the ledger gathers the evidence the steps hold, each item with its step and run, a stale step said
to be; verification finds a run or a file not sealed in the journal, and an altered one. One JSON object a line, as check_client.py reports.

    python api/tests/check_project_archive.py
"""

from __future__ import annotations

import json
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import board, project_archive  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "project archive: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def refused(f) -> str | None:
    try:
        f()
    except ValueError as error:
        return str(error)
    return None


def main() -> int:
    item = {"quantity": "evacuation_share_30min", "label": "people out of the hazard's reach within 30 minutes", "value": 0.85, "low": 0.80, "high": 0.92,
            "unit": "share", "scale": "absolute", "place": {"name": "Miyako", "lat": 39.64, "lon": 141.955}, "period": {"from": "2011-03-11"},
            "readings": {"leaving": "as in 2011"}, "source": "risk/evacuation.py"}
    ev = board.evidence([item, {"quantity": "scorecard_essential_1", "level": "3 of 5", "scale": "ordinal"}], "evacuation")
    assert_("evidence items are checked and kept", ev["kind"] == "evidence" and len(ev["items"]) == 2 and ev["items"][0]["value"] == 0.85)
    faults = [refused(lambda: board.evidence([{"value": 1}])), refused(lambda: board.evidence([{"quantity": "q"}])),
              refused(lambda: board.evidence([{"quantity": "q", "value": 2, "low": 0, "high": 1}])),
              refused(lambda: board.evidence([{"quantity": "q", "value": 1, "scale": "loud"}])),
              refused(lambda: board.evidence([{"quantity": "q", "value": float("nan")}])), refused(lambda: board.evidence([]))]
    assert_("an item without a quantity, without a value, outside its interval, on no scale, not finite, or none at all, is refused", all(faults), faults)
    finished = board.finish([{"kind": "note", "text": "x"}, ev])
    assert_("finish accepts evidence beside notes, and checks it", [r["kind"] for r in finished] == ["note", "evidence"]
            and refused(lambda: board.finish([{"kind": "evidence", "items": [{"quantity": ""}]}])) is not None)

    with tempfile.TemporaryDirectory() as tmp:
        folder = Path(tmp)
        step = {"id": "s-evac", "tool": "risk/evacuation.py", "title": "Walking out", "params": {"speed": 1.0}}
        results = [ev, {"kind": "note", "text": "done"}]
        d1 = project_archive.keep_run(folder, step, results, "an analyst")
        d2 = project_archive.keep_run(folder, step, results, "an analyst")
        files = list((folder / "archive" / "runs").glob("*.json"))
        assert_("a run is kept under its digest, once however often it recurs, and listed each time", d1 == d2 and len(files) == 1 and len(project_archive.runs(folder)) == 2)
        back = project_archive.run(folder, d1)
        assert_("a run is read back as it was kept", back["step"]["id"] == "s-evac" and back["results"][0]["items"][0]["quantity"] == "evacuation_share_30min")
        h = project_archive.keep_holding(folder, "minutes of the meeting.txt", "the council met; the drill is in May".encode(), "text/plain", "an analyst")
        assert_("a file is kept by the digest of its bytes", project_archive.holding(folder, h["digest"]).startswith(b"the council") and len(h["digest"]) == 64)

        # the ledger: a step's latest results, as the studio host keeps them, with its run
        (folder / "results").mkdir()
        (folder / "results" / "s-evac.json").write_text(json.dumps({"step": {**step, "run": d1}, "results": results}), encoding="utf-8")
        led = project_archive.ledger(folder, {"steps": [{**step, "stale": True, "ran": "2026-11-19T02:00:00Z"}, {"id": "s-none", "tool": "x"}]})
        assert_("the ledger gathers the evidence a step holds, with its step, run and staleness", len(led["items"]) == 2 and led["items"][0]["from"]["run"] == d1
                and led["items"][0]["from"]["stale"] and led["steps"][0]["items"] == 2, led)

        journal = [{"what": "kept a run", "about": "s-evac", "note": d1}, {"what": "kept a file", "about": h["digest"], "note": h["name"]}]
        ok = project_archive.verify(folder, journal)
        unsealed = project_archive.verify(folder, journal[:1])
        assert_("the archive verifies against the journal that sealed it, and a file not sealed there is found", ok["holds"] and not unsealed["holds"]
                and "not sealed" in unsealed["faults"][0], unsealed)
        path = folder / "archive" / "runs" / f"{d1}.json"
        path.write_text(path.read_text(encoding="utf-8").replace("Walking out", "Walking back"), encoding="utf-8")
        altered = project_archive.verify(folder, journal)
        try:
            project_archive.run(folder, d1)
            read_refused = False
        except ValueError:
            read_refused = True
        assert_("a run altered on disk is found, by verification and by reading it", not altered["holds"] and "altered" in altered["faults"][0] and read_refused, altered)
        try:
            project_archive.run(folder, "../../etc")
            named = False
        except KeyError:
            named = True
        assert_("a run is named only by a digest", named)
    return 0


if __name__ == "__main__":
    sys.exit(main())
