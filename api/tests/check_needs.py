"""Checks of the search for needs at the API's edge (science/needs.py and api/cpp/science/needs.hpp), run by api/tests/run-tests.mjs.
A dataset made here with a need planted, whose truth is known; and, where the Urban Studio has built the engine: the dataset described
and read; a statement tested, its gap as worked here by hand; a search run, the planted need found and confirmed; a search continued
from its saved state equal to one run straight; a saved state whose journal was altered refused; a language model's proposals (a
stand-in, which calls no model) read by the engine, a bad line refused and never repaired, and the prompt holding nothing of the
parts kept aside. One JSON object a line, as check_client.py reports.

    python api/tests/check_needs.py
"""

from __future__ import annotations

import copy
import json
import random
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import models  # noqa: E402
from science import needs as nd  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "needs: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "needs: " + name})


# ---- A dataset made here: 240 areas, people by age and tenure; those 75 and over who rent are without help 15 points more often ----
rng = random.Random(7)
cols = {"area": [], "age": [], "tenure": [], "k": [], "n": []}
for a in range(240):
    base = 0.08 + rng.gauss(0, 0.015)
    for age in (40, 70, 85):
        for tenure in ("owner", "renting"):
            n = rng.randint(20, 60)
            p = min(0.9, max(0.01, base + (0.15 if age >= 75 and tenure == "renting" else 0.0)))
            cols["area"].append(f"a{a}"), cols["age"].append(age), cols["tenure"].append(tenure), cols["n"].append(n)
            cols["k"].append(sum(rng.random() < p for _ in range(n)))
DATA = {
    "schema": nd.DATA_SCHEMA,
    "columns": [{"name": "area", "kind": "category", "values": cols["area"]}, {"name": "age", "kind": "number", "values": cols["age"]},
                {"name": "tenure", "kind": "category", "values": cols["tenure"]}, {"name": "k", "kind": "number", "values": cols["k"]},
                {"name": "n", "kind": "number", "values": cols["n"]}],
    "attributes": ["age", "tenure"],
    "indicators": [{"name": "unassisted", "share": True, "column": "k", "total": "n", "higher_is_worse": True, "domain": "care", "label": "without help"}],
    "domains": ["care"],
    "cluster": "area",
}
OPTIONS = {"strategy": {"mu": 30, "lambda": 30}, "stability_resamples": 50}

assert_("the dataset made here is a noetarium.needs-data/1", DATA["schema"] == "noetarium.needs-data/1")
try:
    nd.data("no-such-case-at-all")
    assert_("a case the data library has not made is refused, with how to make it", False)
except LookupError as e:
    assert_("a case the data library has not made is refused, with how to make it", "needs_tables.py" in str(e), e)

# ---- the prompt, and a stand-in model -----------------------------------------------------------------------------------------------
described = {"indicators": [{"name": "unassisted", "label": "without help", "domain": "care", "higher_is_worse": True, "value": 0.1}],
             "domains": ["care"], "attributes": ["age", "tenure"], "conditions": ["age <= 40", "age > 40", "tenure = owner", "tenure = renting"]}
report = {"objectives": ["gap", "coverage", "simplicity"],
          "population": [{"text": "need care for tenure = renting by comparative unassisted higher", "objectives": {"objectives": [-6.1, -3.4, 1]}}],
          "confirmations": [{"text": "need care for tenure = renting by comparative unassisted higher", "p": 0.000123456, "confirmed": True}],
          "archive": [{"text": "need care for tenure = renting by comparative unassisted higher", "evidence": {"standing": {"standing": "robust"}}}]}
prompt = nd.prompt_for(described, report, 5)
assert_("the prompt holds the language, the vocabulary and the population's best with their discovery scores",
        "need      ::=" in prompt and "unassisted: without help" in prompt and "gap z 6.1" in prompt and "2,512 people" in prompt, prompt[:400])
assert_("the prompt holds nothing of the parts kept aside: no confirmation, no standing", "0.000123" not in prompt and "robust" not in prompt and "confirm" not in prompt.lower(), prompt)

engine = nd.ENGINE.status()
if not engine.get("built"):
    skip("the engine is not built (python applications/urban-studio/engine.py build needs): the engine's checks")
    sys.exit(0)

desc = nd.ask("describe", data=DATA, options=OPTIONS)
assert_("describe: 1440 rows in 240 clusters, split into three parts of whole clusters",
        desc["rows"] == 1440 and desc["clusters"] == 240 and sum(p["clusters"] for p in desc["parts"]) == 240, json.dumps(desc["parts"]))
assert_("describe: the pool of conditions holds both tenures and the ages' cuts", "tenure = renting" in desc["conditions"] and any(c.startswith("age <=") for c in desc["conditions"]))

read = nd.ask("read", data=DATA, text="need care for age >= 75 and tenure = renting by comparative unassisted higher\nneed care for income < 1 by comparative unassisted higher\nneed care for by")
assert_("read: a statement read and written canonically", read[0]["read"] and read[0]["statement"].startswith("need care for age >= 75"))
assert_("read: one stated in no attribute refused, with the reason", not read[1]["read"] and "not an attribute" in read[1]["why"], read[1])
assert_("read: one that is not a statement refused, with its column", not read[2]["read"] and read[2]["why"].startswith("column"), read[2])

# The gap by hand on the discovery part: the design's ratio difference.
tested = nd.ask("test", data=DATA, options=OPTIONS, statements=["need care for age >= 75 and tenure = renting by comparative unassisted higher"])[0]
ev = tested["evidence"]
assert_("test: the planted gap of 0.15 found on the areas the search sees", abs(ev["discovery"]["estimate"] - 0.15) < 4 * ev["discovery"]["se"] and ev["discovery"]["z"] > 6,
        ev["discovery"])
assert_("test: stable in every resample", ev["stability"]["share"] == 1.0, ev["stability"])
assert_("test: not yet confirmed, and said so", ev["standing"]["standing"] == "supported" and "confirmation" not in ev, ev["standing"])

# A search, straight and in two halves.
straight = nd.ask("evolve", data=DATA, options=OPTIONS, generations=8)
rep = straight["report"]
found = [c for c in rep["confirmations"] if c["confirmed"] and "age >" in c["text"] and "tenure = renting" in c["text"]]
assert_("evolve: the planted need found, admitted and confirmed", bool(found), [c["text"] for c in rep["confirmations"]][:8])
assert_("evolve: every statement admitted confirmed once, its test sealed before and after", len(rep["confirmations"]) == len(rep["admissions"]) and len(rep["journal"]) == 2 * len(rep["confirmations"]))
first = nd.ask("evolve", data=DATA, options=OPTIONS, generations=4)
second = nd.ask("evolve", data=DATA, options=OPTIONS, generations=4, state=first["state"])
assert_("evolve: a search continued from its saved state is one that never stopped (the same journal)", second["report"]["journal_head"] == rep["journal_head"],
        f"{second['report']['journal_head'][:16]} against {rep['journal_head'][:16]}")
altered = copy.deepcopy(first["state"])
if altered["journal"]:
    altered["journal"][0]["note"] += " (changed)"
    try:
        nd.ask("evolve", data=DATA, options=OPTIONS, generations=1, state=altered)
        assert_("evolve: a state whose journal was altered is refused, never repaired", False)
    except nd.EngineError as e:
        assert_("evolve: a state whose journal was altered is refused, never repaired", "journal does not verify" in str(e), e)
else:
    skip("evolve: an altered journal (nothing was confirmed in four generations)")

# Proposals, from a person and from a stand-in model.
person = nd.ask("evolve", data=DATA, options=OPTIONS, generations=1, proposals=[{"text": "need care for age >= 75 and tenure = renting by comparative unassisted higher\nnot a statement", "source": "a person"}])
props = [s.get("proposals", {}) for s in person["report"]["statistics"]]
assert_("evolve: a person's proposal enters, credited to its source; a bad line refused with the reason",
        any(p.get("a person") == 1 for p in props) and person["refused"] and person["refused"][0]["source"] == "a person", (props, person["refused"]))
stub = models.Stub({"statements": [{"statement": "need care for age >= 85 and tenure = renting by comparative unassisted higher", "why": "the oldest renters"},
                                   {"statement": "need care for wealth < 3 by comparative unassisted higher", "why": "an attribute not in the data"}]})
p = nd.propose(DATA, rep, 2, stub, described=desc)
assert_("propose: a model's statement that reads is kept, as a proposal of grade C", p["grade"] == "C" and len(p["read"]) == 1 and p["text"].startswith("need care for age >= 85"), p)
assert_("propose: one the data cannot judge is refused with the engine's reason, never repaired", len(p["refused"]) == 1 and "not an attribute" in p["refused"][0]["why"], p["refused"])
asked = stub.asked[0]["prompt"]
assert_("propose: the model was shown no confirmation and no standing", "confirm" not in asked.lower() and "robust" not in asked and "supported" not in asked, asked[-600:])
