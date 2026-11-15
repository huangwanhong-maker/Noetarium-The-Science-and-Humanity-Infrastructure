"""Checks of law at the API's edge (science/models.py, science/recognise.py, science/law.py and api/cpp/science/law.hpp), run by
api/tests/run-tests.mjs. A key kept in a settings folder is found and never shown; a model's candidates from a document are each
looked for in it (found across a line break, refused when a word differs, when the fact is not declared, or when the passage is
not there at all), found once in overlapping windows, made proposals of grade B awaiting review; and an invented park bylaw stated
clause by clause asked of the engine: a scheme whole and one with a fault; a matter whose evidence makes a bicycle proved, a binding
holding fixing the broad reading, the ban clear and resting on its section, its plan of proof with a fact no one alleges set apart; a model's candidate not yet reviewed
counted at grade C; and the warden's log with its custody broken undercut, so that the ban is no longer clear. No model is called:
the provider is a stand-in. One JSON object a line, as check_client.py reports.

    python api/tests/check_law.py
"""

from __future__ import annotations

import json
import os
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import law, models, recognise  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail: str = "") -> None:
    emit({"kind": "assert", "name": "law: " + name, "ok": bool(ok), "detail": "" if ok else detail})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "law: " + name})


# ---- Keys --------------------------------------------------------------------------------------------------------------------
with tempfile.TemporaryDirectory() as folder:
    old = {k: os.environ.get(k) for k in ("APPDATA", "XDG_CONFIG_HOME")}
    os.environ["APPDATA"] = folder
    os.environ["XDG_CONFIG_HOME"] = folder
    try:
        assert_("with no key kept, none is found and the state says so", models.kept_key("anthropic") is None and models.key_state("anthropic") == {"kept": False, "ends": ""})
        (Path(folder) / "noetarium").mkdir()
        (Path(folder) / "noetarium" / "models.json").write_text(json.dumps({"keys": {"anthropic": "sk-test-0000wxyz"}}), encoding="utf-8")
        state = models.key_state("anthropic")
        assert_("a key kept in the person's settings folder is found, and only how it ends is shown", models.kept_key("anthropic") == "sk-test-0000wxyz" and state == {"kept": True, "ends": "…wxyz"})
        assert_("Claude's provider takes the kept key when none is given", models.Anthropic().key == "sk-test-0000wxyz" and models.Anthropic(key="given").key == "given")
    finally:
        for k, v in old.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v
try:
    models.provider("nobody")
    assert_("a provider that does not exist is refused", False)
except models.Refusal as e:
    assert_("a provider that does not exist is refused, naming those there are", "anthropic" in str(e) and "stub" in str(e))

# ---- Recognition by a model (a stand-in) ---------------------------------------------------------------------------------------------
FACTS = [{"id": "bicycle", "text": "a bicycle was ridden in the park"}, {"id": "emergency", "text": "there was an emergency"}]
LOG = "Warden's log, 3 May.\nAt 9.10 a man rode a bicycle\npast the fountain. No one was hurt; there was no emergency.\n"
answer = {"candidates": [
    {"fact": "bicycle", "negated": False, "passage": "a man rode a bicycle past the fountain", "speaker": "the warden", "note": "seen riding"},
    {"fact": "emergency", "negated": True, "passage": "there was no emergency", "speaker": "the warden", "note": "denied"},
    {"fact": "bicycle", "negated": False, "passage": "a man rode a red bicycle past the fountain", "speaker": "the warden", "note": "a word that is not there"},
    {"fact": "tricycle", "negated": False, "passage": "At 9.10", "speaker": "", "note": "not declared"},
    {"fact": "bicycle", "negated": False, "passage": "he was arrested", "speaker": "", "note": "nowhere"},
]}
stub = models.Stub(answer)
found = recognise.propose({"id": "log", "title": "the warden's log", "text": LOG}, FACTS, stub, name="the stand-in")
cands, refused = found["candidates"], found["refused"]
assert_("two passages are found in the document, the first across its line break, each at its byte offset",
        len(cands) == 2 and cands[0]["at"] == LOG.encode("utf-8").find(b"a man rode") and cands[1]["negated"] and cands[1]["proposition"] == "emergency", json.dumps(cands)[:400])
assert_("each is a proposal of grade B by the provider, awaiting a person's review",
        all(c["grade"] == "B" and c["status"] == "proposed" and c["by"] == "a stand-in, stub" and c["source"] == "the warden" for c in cands))
reasons = [r["reason"] for r in refused]
assert_("a passage with a word not in the document, a fact not declared, and a passage that is nowhere are refused, never repaired",
        len(refused) == 3 and sum("not in the document" in r for r in reasons) == 2 and any("does not declare" in r for r in reasons), json.dumps(refused)[:400])
assert_("the model was asked under a schema whose facts are the declared ones, with the document cached",
        stub.asked[0]["schema"]["properties"]["candidates"]["items"]["properties"]["fact"]["enum"] == ["bicycle", "emergency"] and stub.asked[0]["cached"] == LOG)
long = ("filler text. " * 12000) + LOG
found2 = recognise.propose({"id": "long", "text": long}, FACTS, models.Stub(answer))
assert_("a long document is read in overlapping windows, and a passage found in two of them is kept once",
        found2["windows"] >= 2 and len(found2["candidates"]) == 2, f"{found2['windows']} windows, {len(found2['candidates'])} candidates")
try:
    recognise.propose({"id": "scan", "text": "   "}, FACTS, stub)
    assert_("a document with no text is refused", False)
except models.Refusal as e:
    assert_("a document with no text is refused: a scan must be transcribed first", "transcribed" in str(e))

# ---- The engine ------------------------------------------------------------------------------------------------------------------------
SCHEME = {
    "id": "park", "title": "The Park Bylaw, an example", "as_of": "2026-01-01",
    "instruments": [{"id": "bylaw", "title": "Park Bylaw 2010", "citation": "PB 2010", "kind": "bylaw", "as_of": "2010-01-01",
                     "provisions": [{"id": "bylaw/s1/1", "label": "Section 1(1)", "text": "No vehicle may be taken into the park."},
                                    {"id": "bylaw/s1/2", "label": "Section 1(2)", "text": "Subsection (1) does not apply in an emergency."}]}],
    "rules": [{"id": "ban", "elements": ["vehicle", "in_park"], "consequence": "prohibited", "cites": ["bylaw/s1/1"], "quote": "No vehicle may be taken"}],
    "exceptions": [{"id": "emergency_exception", "rule": "ban", "elements": ["emergency"], "cites": ["bylaw/s1/2"]}],
    "terms": [{"name": "vehicle", "readings": ["broad", "narrow"]}, {"name": "unused", "readings": ["a", "b"]}],
    "counts": [{"id": "bicycle_counts", "fact": "bicycle", "term": "vehicle", "reading": "broad", "as": "vehicle", "cites": ["bylaw/s1/1"]}],
    "vocabulary": [{"id": i, "use": "fact"} for i in ("bicycle", "in_park", "emergency")] + [{"id": "vehicle", "use": "consequence"}, {"id": "prohibited", "use": "consequence"}],
    "courts": [{"id": "supreme", "name": "the Supreme Court", "order": "Avalon", "level": 2, "instruments": ["bylaw"]}],
    "holdings": [{"id": "s1", "court": "supreme", "name": "Rider v Council", "date": "2015-06-01", "instrument": "bylaw", "term": "vehicle", "reading": "broad"}],
}


def matter(**changes) -> dict:
    m = {"id": "m1", "date": "2026-05-03", "forum": {"id": "mag", "name": "the magistrates", "bound_by": ["Avalon"]},
         "standard": {"name": "on the balance", "argument": "justified"},
         "issues": [{"id": "ban", "question": "prohibited", "burden": "the council"}],
         "facts": [{"id": "bicycle", "text": "a bicycle was ridden in", "standing": "alleged"}, {"id": "in_park", "standing": "admitted"}],
         "documents": [{"id": "log", "text": LOG}],
         "candidates": [{"id": "c1", "proposition": "bicycle", "source": "the warden", "document": "log", "passage": "a man rode a bicycle past the fountain", "grade": "B"}],
         "exhibits": [{"id": "x1", "reports": "bicycle", "source": "the warden", "kind": "record", "text": "the warden's log",
                       "chain": {"digest": "aa", "custodian": "warden", "since": "2026-05-03", "transfers": [{"when": "2026-05-04", "from": "warden", "to": "clerk", "digest": "aa"}]}}]}
    m.update(changes)
    return m


status = law.ENGINE.status()
if not status["built"]:
    skip("the engine (not built: python applications/law-studio/engine.py build)")
else:
    sc = law.ask("scheme", scheme=SCHEME)
    assert_("a scheme stated clause by clause is whole, and what it holds is counted", sc["faults"] == [] and sc["counts"]["rules"] == 1 and sc["counts"]["holdings"] == 1, json.dumps(sc)[:300])
    broken = json.loads(json.dumps(SCHEME))
    broken["rules"][0]["quote"] = "No vehicle shall enter"
    assert_("a quotation not in its clause is a fault of the scheme", any(f["where"] == "citation/ban" for f in law.ask("scheme", scheme=broken)["faults"]))
    ev = law.ask("evaluate", scheme=SCHEME, matter=matter())
    fact = {f["id"]: f for f in ev["facts"]}
    assert_("the warden's passage and log make the bicycle's evidence justified, and the alleged fact enters as proved", fact["bicycle"]["evidence"] == "justified" and fact["bicycle"]["used"] == "proved", json.dumps(ev["facts"])[:300])
    issue = ev["issues"][0]
    steps = [s["id"] for s in issue["rests_on"]["steps"]]
    assert_("the binding holding fixes the broad reading, and the ban is clear, resting on Section 1(1)",
            ev["authority"]["assessed"][0]["force"] == "binding" and ev["admissibility"]["fixed"] == ["[vehicle=broad]"] and issue["class"] == "clear"
            and "ban" in steps and issue["rests_on"]["steps"][0]["provisions"][0]["label"] == "Section 1(1)", json.dumps(issue)[:500])
    assert_("only the open terms the issue turns on enter the case", [t["term"] for t in ev["terms"]] == ["vehicle"])
    assert_("its plan: the emergency, which no one alleges, is listed apart and left out of the ways", issue["plan"]["open"] == [] and issue["plan"]["absent"] == ["emergency"] and issue["plan"]["must_disprove"] == [])
    alleged = matter()
    alleged["facts"].append({"id": "emergency", "text": "it was an emergency", "standing": "alleged"})
    plan = law.ask("evaluate", scheme=SCHEME, matter=alleged)["issues"][0]["plan"]
    assert_("once the emergency is alleged it is open, to be shown not to hold in every way of establishing the ban, and made out to resist it",
            plan["open"] == ["emergency"] and plan["must_disprove"] == ["emergency"] and plan["resist"] == [{"prove": ["emergency"], "disprove": [], "readings": []}], json.dumps(plan))
    proposed = matter(candidates=[{**matter()["candidates"][0], "status": "proposed", "by": "a model"}], exhibits=[])
    ev2 = law.ask("evaluate", scheme=SCHEME, matter=proposed)
    assert_("a model's candidate not yet reviewed counts at grade C and enters no chart: the bicycle stays alleged, the ban untenable",
            {f["id"]: f for f in ev2["facts"]}["bicycle"]["used"] == "alleged" and ev2["evidence"]["recognised"][0]["standing"] == "proposed" and ev2["issues"][0]["class"] == "untenable",
            json.dumps(ev2["facts"])[:300])
    broken_chain = matter(candidates=[], exhibits=[{**matter()["exhibits"][0], "chain": {"digest": "aa", "custodian": "warden", "since": "2026-05-03",
                                                                                         "transfers": [{"when": "2026-05-04", "from": "the porter", "to": "clerk", "digest": "aa"}]}}])
    ev3 = law.ask("evaluate", scheme=SCHEME, matter=broken_chain)
    assert_("with the log handed on by someone who never held it, the archive's finding undercuts it, the bicycle stays alleged, and the ban is not clear",
            ev3["evidence"]["authenticity"][0]["verdict"]["status"] == "fails" and {f["id"]: f for f in ev3["facts"]}["bicycle"]["used"] == "alleged" and ev3["issues"][0]["class"] != "clear",
            json.dumps(ev3["evidence"]["authenticity"])[:300])
