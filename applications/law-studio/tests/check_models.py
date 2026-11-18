"""The Law Studio's use of a language model, with a stand-in (no model is called): a document of the example matter read against
its facts, the passages found joining the matter as proposals, one that is not in the document dropped and said; a proposal counting
for nothing until a person accepts it; the open terms read, the proposal kept beside the matter and fixing nothing; and a key never
given to a page. Prints one JSON line a check; run-tests.mjs reads them.

    python applications/law-studio/tests/check_models.py
"""

from __future__ import annotations

import json
import sys
import tempfile
from pathlib import Path

APP = Path(__file__).resolve().parents[1]
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

import matters  # noqa: E402
from science import law, models  # noqa: E402
from science import studio_host as host  # noqa: E402


def say(name, ok, detail=""):
    print(json.dumps({"name": name, "ok": bool(ok), "detail": "" if ok else str(detail)[:500]}, ensure_ascii=False), flush=True)


if not law.ENGINE.status()["built"]:
    print(json.dumps({"name": "the law engine is not built", "skip": True}))
    sys.exit(0)

with tempfile.TemporaryDirectory() as folder:
    host.STORE["dir"] = Path(folder)
    project = host.project_dir("the-bicycle")
    project.mkdir(parents=True)
    (project / "project.json").write_text("{}", encoding="utf-8")
    laid = matters.example("the-bicycle", "park/the-bicycle")
    say("the park's matter is laid out with its three documents, each kept with its digest", len(laid["documents"]) == 3 and all(d["sha256"] and d["text"] for d in matters.documents("the-bicycle")), laid)
    exhibits = matters.read_matter("the-bicycle")["exhibits"]
    say("an exhibit's chain is taken in with the digest of its document's bytes", all(len(e["chain"]["digest"]) == 64 and e["chain"]["digest"] == matters.observed("the-bicycle", e["document"]) for e in exhibits))

    answer = {"candidates": [
        {"fact": "bicycle", "negated": False, "passage": "A man rode in at the east gate on a bicycle", "speaker": "J. Okoro, warden", "note": "seen"},
        {"fact": "motor_assisted", "negated": False, "passage": "I took it for an electric bicycle from the hum", "speaker": "J. Okoro, warden", "note": "an inference"},
        {"fact": "motor_assisted", "negated": False, "passage": "I saw the battery on the frame", "speaker": "J. Okoro, warden", "note": "not in the log"},
    ]}
    before = len(matters.read_matter("the-bicycle")["candidates"])
    got = matters.recognise_in("the-bicycle", "The warden's log.txt", model=models.Stub(answer))
    m = matters.read_matter("the-bicycle")
    added = [c for c in m["candidates"] if c.get("by") == "a stand-in, stub"]
    say("a model's passages found in the log join the matter as proposals; the one that is not in the log is dropped and said",
        len(got["added"]) == 2 and len(m["candidates"]) == before + 2 and len(got["refused"]) == 1 and "not in the document" in got["refused"][0]["reason"]
        and all(c["status"] == "proposed" and c["document"] == "The warden's log.txt" for c in added), got)
    again = matters.recognise_in("the-bicycle", "The warden's log.txt", model=models.Stub(answer))
    say("read again, the same passages are not put forward twice", len(again["added"]) == 0 and len(matters.read_matter("the-bicycle")["candidates"]) == before + 2)

    def evidence_of(fact):
        ev = matters.evaluate("the-bicycle")["evaluation"]
        return next(f for f in ev["facts"] if f["id"] == fact), ev

    f0, _ = evidence_of("motor_assisted")
    motor = next(c for c in added if c["proposition"] == "motor_assisted")
    matters.review("the-bicycle", motor["id"], "accepted")
    f1, _ = evidence_of("motor_assisted")
    say("a proposal counts for nothing until a person accepts it: the motor is overruled by the rider's denial alone, then defensible with the warden's passage against it",
        f0["evidence"] == "overruled" and f1["evidence"] == "defensible" and f1["used"] == "alleged", (f0, f1))
    reviewed = next(c for c in matters.read_matter("the-bicycle")["candidates"] if c["id"] == motor["id"])
    say("who accepted it, and when, is kept with it", reviewed["status"] == "accepted" and reviewed.get("reviewed_by") and reviewed.get("reviewed_at"))

    stub = models.Stub({"readings": {"vehicle": {"reading": "broad", "rationale": "a bicycle carries a person on wheels"}}})
    p = matters.propose_readings("the-bicycle", model=stub)["proposal"]
    _, ev = evidence_of("bicycle")
    say("a model's reading of the open term is kept as a proposal of grade C, and fixes nothing: the offence is as arguable as before",
        p["proposed"] == {"vehicle": "broad"} and p["grade"] == "C" and len(matters.proposals("the-bicycle")) == 1 and ev["issues"][0]["class"] == "arguable" and ev["admissibility"]["fixed"] == [], p)
    asked = stub.asked[0]["prompt"]
    say("the model was told the clauses in their words, the open term with its readings, and the holdings as they weigh for the forum",
        "No vehicle may be taken into the park." in asked and "vehicle" in asked and "broad" in asked and "Borough v Skater" in asked and "persuasive" in asked, asked[:400])
    try:
        matters.propose_readings("the-bicycle", model=models.Stub({"readings": {"vehicle": {"reading": "motorised", "rationale": ""}}}))
        say("a reading the term does not admit is refused, never repaired", False)
    except models.Refusal as error:
        say("a reading the term does not admit is refused, never repaired", "does not declare" in str(error), error)
    journal = host.journal_file("the-bicycle").read_text(encoding="utf-8") if host.journal_file("the-bicycle").is_file() else ""
    say("each of these is written in the project's journal", "had a document read by a model" in journal and "accepted a candidate of evidence" in journal and "had the open terms read by a model" in journal, journal[-400:])


    # Theories of the case (ADR 0127): a model in the search's roles, with a stand-in. It is shown the evidence as the search sees it,
    # never what is held back; what it writes is read by the engine, entered as model:<role> (grade C), or refused with its line.
    fire = host.project_dir("the-boathouse")
    fire.mkdir(parents=True)
    (fire / "project.json").write_text("{}", encoding="utf-8")
    matters.example("the-boathouse", "incident/the-boathouse")
    written = {"answers": [{"request": 0, "theories": [
        {"text": "event ignite actor=grey at 1 1 when 21:46:30\nbelieve jogger when 21:49:00", "why": "the man on the bridge"},
        {"text": "event ignite actor=nobody at 0 0 when 21:45:00", "why": "someone else"}]}]}
    stub = models.Stub(written)
    got = matters.model_theories("the-boathouse", budget=2, model=stub)
    told = stub.asked[0]["prompt"]
    say("a model asked for theories is told the evidence as the search sees it, and never the till roll held back",
        "inn-camera" in told and "jogger" in told and "late-till" not in told and "21:44:20" not in told, told[:300])
    say("what it writes is read by the engine: one theory entered as a seed of grade C, one naming no one in the matter refused with its line",
        len(got["proposed"]["read"]) == 1 and len(got["proposed"]["refused"]) == 1 and got["proposed"]["grade"] == "C" and "line 1" in str(got["proposed"]["refused"][0]["errors"])
        and "model:seed" in (got["report"]["statistics"][-1].get("proposals") or {}), got["proposed"])
    journal = host.journal_file("the-boathouse").read_text(encoding="utf-8")
    say("and the request is in the project's journal, with the search run after it", "had theories proposed by a model" in journal and "ran the search for theories" in journal, journal[-300:])
