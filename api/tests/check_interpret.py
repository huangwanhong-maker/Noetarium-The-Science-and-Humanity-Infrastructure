"""Checks of derivability under interpretation at the API's edge (science/interpret.py and api/cpp/science/interpretation.hpp), run
by api/tests/run-tests.mjs: a case read for its points, and a case that cannot be read refused with its lines; a model's answer asked
under a schema of the declared readings, accepted as a proposal of grade C, and refused when it names a point or a reading the case
does not declare or leaves a point out; and the panel weighed by the engine against tekmerion's own figures on Hart's park (a stated
law 0.42, precedents from the question's history 1/2, a decision over purposes certain, a proposal apart; with purpose ranked over
text the law conditioned to 0.6; with text over purpose the ban untenable and the panel failing), a sequence of cases that probes,
and interpreters stated wrongly reported with their reasons while the rest weighs. No model is called: the provider is a stand-in. One JSON object a line, as
check_client.py reports.

    python api/tests/check_interpret.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import interpret as it  # noqa: E402
from science.engines import EngineError, toolchain  # noqa: E402

PARK = """open vehicle: broad | narrow
open park: whole | lawns
premise bicycle
premise on_path
strict counts: bicycle, [vehicle=broad] -> vehicle
strict within: on_path, [park=whole] -> in_park
defeasible ban: vehicle, in_park => prohibited
query prohibited
"""
ARGUED = PARK + "argue a1 for [vehicle=narrow] by text: a bicycle is not a motor vehicle\nargue a2 for [vehicle=broad] by purpose: the rule keeps paths safe\n"


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def check(name: str, value: float, limit: float) -> None:
    emit({"kind": "check", "name": name, "value": float(value), "limit": float(limit), "unit": ""})


def assert_(name: str, condition: bool, detail: str = "") -> None:
    emit({"kind": "assert", "name": name, "ok": bool(condition), "detail": str(detail)})


def refused(call, *words: str) -> tuple[bool, str]:
    try:
        call()
    except Exception as error:  # the reason is what is checked
        return all(w in str(error) for w in words), str(error)
    return False, "it was not refused"


def answer(vehicle="broad", park="none", **extra) -> dict:
    readings = {"vehicle": {"reading": vehicle, "rationale": "a bicycle is a vehicle under the road code"}, "park": {"reading": park, "rationale": "the case does not say"}}
    readings.update(extra)
    return {"readings": readings}


# ---- what a model is asked, and what is accepted (no engine needed) ---------------------------------------------------------------
POINTS = [{"name": "vehicle", "readings": ["broad", "narrow"]}, {"name": "park", "readings": ["whole", "lawns"]}]
stub = it.Stub(answer())
p = it.propose(PARK, stub, points=POINTS, question={"name": "a bicycle on the path", "features": {"speed": 12}})
asked = stub.asked[0]
enum = asked["schema"]["properties"]["readings"]["properties"]["vehicle"]["properties"]["reading"]["enum"]
assert_("a model is asked under a schema whose readings are the case's own, and none", enum == ["broad", "narrow", "none"], enum)
assert_("its message holds the case, its points with their readings, and what is stated of the case",
        "defeasible ban" in asked["prompt"] and "- vehicle: broad | narrow" in asked["prompt"] and "- speed: 12" in asked["prompt"])
assert_("its answer becomes a proposal of grade C, by who answered, with the park left open",
        p["method"] == "proposal" and p["grade"] == "C" and p["proposed"] == {"vehicle": "broad", "park": None} and p["by"] == "a stand-in, stub", p)
ok, why = refused(lambda: it.propose(PARK, it.Stub(answer(vehicle="motorised")), points=POINTS), "motorised", "does not declare")
assert_("an answer that reads a point as what the case does not declare is refused, never repaired", ok, why)
ok, why = refused(lambda: it.propose(PARK, it.Stub(answer(lane={"reading": "x", "rationale": ""})), points=POINTS), "lane", "no open point")
assert_("an answer that reads a point the case does not have is refused", ok, why)
ok, why = refused(lambda: it.propose(PARK, it.Stub({"readings": {"vehicle": {"reading": "broad", "rationale": ""}}}), points=POINTS), "no reading of park")
assert_("an answer that leaves a point out is refused", ok, why)
ok, why = refused(lambda: it.Anthropic(effort="enormous"), "effort")
assert_("a provider asked for an effort that does not exist is refused", ok, why)
assert_("Claude's provider says whether its SDK is here, and takes the API's fallback for Opus 5.5", it.Anthropic().unavailable() in (None, "the Anthropic SDK is not installed for this Python: pip install anthropic") and "claude-opus-5-5" in it.Anthropic.fallback)

# ---- the engine ---------------------------------------------------------------------------------------------------------------------
if toolchain() is None:
    emit({"kind": "skip", "name": "interpret: the engine (no C++ compiler here)"})
    sys.exit(0)
if not it.build(log=lambda *_: None):
    assert_("interpret: the engine builds", False, "it did not compile")
    sys.exit(0)

pts = it.read_points(PARK)
assert_("the engine reads the case's open points and their readings", pts == POINTS, pts)
ok, why = refused(lambda: it.read_points("open vehicle broad narrow\nnonsense here\n"), "could not be read")
assert_("a case that cannot be read is refused with its lines", ok, why)

history = [{"adopted": {"vehicle": "broad" if i < 8 else "narrow", "park": "whole" if i < 7 else "lawns"}, "response": "upheld"} for i in range(10)]
utility = {"vehicle": {"broad": {"safety": 10, "quiet": 2}, "narrow": {"safety": 1, "quiet": 1}}, "park": {"whole": {"safety": 5, "quiet": 5}, "lawns": {"safety": 3, "quiet": 2}}}
panel = [
    {"name": "a stated law", "method": "law", "law": {"vehicle": {"broad": 0.7, "narrow": 0.3}, "park": {"whole": 0.6, "lawns": 0.4}}},
    {"name": "the earlier cases", "method": "precedents"},
    {"name": "the purposes", "method": "decision", "belief": {"safety": 0.8, "quiet": 0.2}, "utility": utility},
    it.propose(PARK, it.Stub(answer()), points=pts),
]
w = it.weigh(PARK, "prohibited", panel, {"history": history})
ans = {a["interpreter"]: a for a in w["panel"]["answers"]}
check("the engine weighs the stated law at 0.42", abs(ans["a stated law"]["probability"] - 0.42), 1e-12)
check("the precedents of the question's history at 9/12 × 8/12 = 1/2", abs(ans["the earlier cases"]["probability"] - 0.5), 1e-12)
assert_("the purposive decision chooses broad and the whole park, under which the ban follows", ans["the purposes"]["chosen"] == {"vehicle": "broad", "park": "whole"} and ans["the purposes"]["verdict"]["status"] == "holds")
assert_("the model's proposal stands apart, proposed, at grade C", ans["a language model"]["verdict"]["status"] == "undecided" and {"role": "standing", "value": "proposed"} in ans["a language model"]["verdict"]["evidence"] and {"role": "grade", "value": "C"} in ans["a language model"]["verdict"]["evidence"])
assert_("the panel: three weigh, one apart, bounds 0.42 (the stated law) and 1 (the purposes)",
        w["panel"]["weighing"] == 3 and w["panel"]["apart"] == 1 and abs(w["panel"]["lower"] - 0.42) < 1e-12 and w["panel"]["upper"] == 1 and w["panel"]["lowest"] == ["a stated law"], w["panel"]["verdict"]["reason"])
assert_("apodeixis: with no arguments the ban turns on the readings, its label [vehicle=broad], [park=whole]",
        w["derivation"]["classification"] == "arguable" and w["derivation"]["label"] == [{"vehicle": "broad", "park": "whole"}])

purpose = it.weigh(ARGUED + "rank purpose > text\n", "prohibited", panel[:1])
check("with purpose ranked over text, broad is fixed and the law conditioned on it gives 0.6", abs(purpose["panel"]["answers"][0]["probability"] - 0.6), 1e-12)
text = it.weigh(ARGUED + "rank text > purpose\n", "prohibited", panel[:1] + panel[2:3])
assert_("with text ranked over purpose the ban is untenable, the purposive choice inadmissible, and the panel fails",
        text["derivation"]["classification"] == "untenable" and not text["panel"]["answers"][1]["admissible"] and text["panel"]["verdict"]["status"] == "fails"
        and "[vehicle=narrow]" in text["derivation"]["fixed"], text["derivation"])

probe_case = "open reading: first | second | probe\npremise act\nstrict r: act, [reading=first] -> liable\nquery liable\n"
sequence = {"name": "over the cases", "method": "sequence", "purposes": ["safety", "quiet"], "signals": ["upheld", "reversed"],
            "utility": {"reading": {"first": {"safety": 1, "quiet": 0}, "second": {"safety": 0, "quiet": 1}, "probe": {"safety": 0.45, "quiet": 0.45}}},
            "responses": [{"when": {}, "law": {"safety": {"upheld": 0.5, "reversed": 0.5}, "quiet": {"upheld": 0.5, "reversed": 0.5}}},
                          {"when": {"reading": "probe"}, "law": {"safety": {"upheld": 1, "reversed": 0}, "quiet": {"upheld": 0, "reversed": 1}}}],
            "epsilon": 1e-4}
s = it.weigh(probe_case, "liable", [sequence])
assert_("a sequence of cases stated in JSON probes first, worth 9.45 against the one-step decision's 5", s["panel"]["answers"][0]["chosen"] == {"reading": "probe"} and "9.45" in s["panel"]["answers"][0]["basis"], s["panel"]["answers"][0]["basis"])
after = it.weigh(probe_case, "liable", [sequence], {"history": [{"adopted": {"reading": "probe"}, "response": "upheld"}]})
assert_("the probe upheld, the first reading is adopted and liability follows", after["panel"]["answers"][0]["chosen"] == {"reading": "first"} and after["panel"]["verdict"]["status"] == "holds")

failing = it.weigh(PARK, "prohibited", [{"name": "speedy", "method": "law", "law": {"vehicle": {"broad": 1}, "park": {"whole": 1}}}])
assert_("an interpreter whose law leaves a reading out is reported as unable to read the case, with why", failing["panel"]["failed"] and "narrow" in failing["panel"]["failed"][0]["why"], failing["panel"]["failed"])
odd = it.weigh(PARK, "prohibited", [{"name": "stars", "method": "astrology"}, {"name": "colours", "method": "proposal", "proposed": {"colour": "red"}}, panel[0]])
why = {f["interpreter"]: f["why"] for f in odd["panel"]["failed"]}
assert_("a method that does not exist, and a proposal naming a point the case does not have, are reported with their reasons while the rest of the panel weighs",
        "no method of interpretation named astrology" in why.get("stars", "") and "no open point colour" in why.get("colours", "") and odd["panel"]["weighing"] == 1, why)
ok, why = refused(lambda: it.weigh("open vehicle broad\nnonsense here\n", "prohibited", []), "apodeixis::interpretation::read")
assert_("a case that cannot be read is refused by the engine with its reason", ok, why)
