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

# ---- a model in every part (ADR 0126): prompts, provenance and proposals merged, without the engine ----
asks = [{"role": "mutate", "count": 1, "parents": [11], "texts": ["need care for tenure = renting by comparative unassisted higher"],
         "notes": ["on discovery: group 0.2 against 0.1, z 6.10, effect 0.80, 2512 people in 40 areas"]}, {"role": "seed", "count": 3, "parents": [], "texts": [], "notes": []}]
rp = nd.requests_prompt(described, report, asks)
assert_("requests: the prompt numbers each request with its role, its statements and their discovery notes",
        "Request 0 (mutate, 1 statement)" in rp and "Request 1 (seed, 3 statements)" in rp and "on discovery: group 0.2" in rp, rp[-500:])
assert_("requests: and holds nothing of the parts kept aside", "0.000123" not in rp and "robust" not in rp and "confirm" not in rp.lower())
toy = {"operators": ["random", "add"], "population": [{"ticket": 3, "text": "need a"}], "archive": [], "retired": [], "admissions": [],
       "lineage": [[1, 0, "random", [], 2, [0]], [2, 0, "random", [], 0, [0]], [3, 1, "model:cross", [1, 2], 6, [0]], [4, 1, "a person", [], 0, [0]]]}
prov = nd.provenance(toy)
agents = {v["prov:label"]: v for v in prov["agent"].values()}
assert_("provenance: four statements, each generated by its operator in its generation",
        len(prov["entity"]) == 4 and len(prov["wasGeneratedBy"]) == 4 and prov["entity"]["needs:s3"]["prov:label"] == "need a" and prov["entity"]["needs:s3"]["needs:admitted"])
assert_("provenance: a model's cross derived from both parents, the model a software agent of grade C, a person a person",
        len(prov["wasDerivedFrom"]) == 2 and agents["model:cross"]["prov:type"] == "prov:SoftwareAgent" and agents["model:cross"]["needs:grade"] == "C"
        and agents["a person"]["prov:type"] == "prov:Person" and "needs:search" in prov["agent"], prov["agent"])
assert_("structure: a cycle among arrows is found", nd._cyclic([["a", "b"], ["b", "c"], ["c", "a"]]) and not nd._cyclic([["a", "b"], ["a", "c"], ["b", "c"]]))
base = {"points": [{"name": "reference", "kind": "reference", "readings": [{"text": "the rest"}]}], "graphs": [], "rivals": []}
merged = nd.with_proposals(base, [{"by": "a stand-in", "readings": [{"kind": "reference", "text": "tenure = owner", "indicator": "unassisted", "rationale": "owners"},
                                                                    {"kind": "level", "text": "20%", "indicator": "unassisted", "rationale": ""}],
                                   "rivals": [{"name": "tenure", "strata": ["tenure = owner", "tenure = renting"], "rationale": ""}]}],
                           [{"by": "a stand-in", "graphs": [{"name": "g1", "arrows": [["a", "b"]], "rationale": ""}], "rivals": []}])
ref = merged["points"][0]["readings"]
assert_("proposals merged: a reference joins the point that concerns it, as a proposal of grade C; a level makes a point of its own",
        len(ref) == 2 and ref[1]["source"] == "proposal" and ref[1]["grade"] == "C" and merged["points"][1]["kind"] == "level" and merged["points"][1]["indicators"] == ["unassisted"])
assert_("proposals merged: graphs and rivals beside the record's, as proposals, the record's own untouched",
        merged["graphs"][0]["source"] == "proposal" and merged["rivals"][0]["grade"] == "C" and len(base["points"][0]["readings"]) == 1)

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

# ---- interpretation, requests and a model's roles, with the engine (ADR 0126) ----
D2 = dict(DATA, points=[{"name": "reference", "kind": "reference", "readings": [{"text": "the rest"}, {"text": "everyone"}, {"text": "age < 75"}]}],
          rivals=[{"name": "tenure", "attribute": "tenure"}])
planted_text = "need care for age >= 75 and tenure = renting by comparative unassisted higher"
d2 = nd.ask("describe", data=D2, options=OPTIONS)
assert_("describe: the points and rivals declared", d2["points"][0]["kind"] == "reference" and len(d2["points"][0]["readings"]) == 3 and d2["rivals"][0]["strata"] == ["tenure = owner", "tenure = renting"],
        (d2["points"], d2["rivals"]))
it = nd.ask("interpret", data=D2, options=OPTIONS, statements=[planted_text])[0]
inter = it["interpretation"]
assert_("interpret: the planted need under three references, each with its statement and gap; clear", len(inter["interpretations"]) == 3 and inter["record"] == "clear"
        and all(x["holds"] and x["statement"].startswith("need care") for x in inter["interpretations"]), inter)
assert_("interpret: the rival assessed (tenure does not explain a gap that lies among renters)", it["rivals"][0]["explains"] != "explains it away", it["rivals"])
try:
    nd.ask("describe", data=dict(DATA, points=[{"name": "r", "kind": "reference", "readings": [{"text": "wealth > 3"}]}]), options=OPTIONS)
    assert_("declarations: a reading the data cannot judge refuses the dataset, with the reason", False)
except nd.EngineError as e:
    assert_("declarations: a reading the data cannot judge refuses the dataset, with the reason", "wealth" in str(e), e)
run2 = nd.ask("evolve", data=D2, options=OPTIONS, generations=3, requests={"budget": 6})
q = run2.get("requests") or []
assert_("requests: a budget of six shared among the roles", sum(x["count"] for x in q) == 6 and {x["role"] for x in q} <= set(nd.ROLES), q)


def answering(system, prompt, schema):
    import re as _re
    count = len(_re.findall(r"^Request \d+ ", prompt, flags=_re.M))
    return {"answers": [{"request": i, "statements": [{"statement": f"need care for age >= {61 + i} and tenure = renting by comparative unassisted higher", "why": "older renters"}]}
                        for i in range(count)] + [{"request": 99, "statements": [{"statement": "need care for x", "why": ""}]}]}


filler = models.Stub(answering)
f = nd.fulfil(D2, q, filler, report=run2["report"], described=d2)
mut = [p for p in f["proposals"] if p["source"] == "model:mutate"]
assert_("fulfil: every request answered in one call, each statement read, a request that was not asked ignored",
        len(filler.asked) == 1 and len(f["read"]) == len(q) and not f["refused"], (len(f["read"]), len(q), f["refused"]))
if mut:
    run3 = nd.ask("evolve", data=D2, options=OPTIONS, generations=1, state=run2["state"], proposals=f["proposals"])
    lin = [l for l in run3["report"]["lineage"] if l[2] == "model:mutate"]
    assert_("fulfil: a model's variation enters with the member it started from as its parent", lin and lin[0][3] == mut[0]["parents"], (lin[:2], mut[:1]))
else:
    skip("fulfil: no mutate request in the budget")
reader = models.Stub({"words": "Older renters go without help more often than others.",
                      "readings": [{"kind": "reference", "text": "tenure = owner", "rationale": "compare with owners"},
                                   {"kind": "reference", "text": "wealth > 3", "rationale": "no such attribute"},
                                   {"kind": "indicator", "text": "nothing", "rationale": "no such indicator"}],
                      "rivals": [{"name": "age", "strata": ["age < 75", "age >= 75"], "rationale": "the old"}, {"name": "one", "strata": ["age < 75"], "rationale": ""}]})
mi = nd.interpret(D2, planted_text, reader, described=d2)
assert_("interpret by a model: its words kept, a reading the data can judge kept, others refused with reasons, never repaired",
        mi["words"].startswith("Older renters") and [r["text"] for r in mi["readings"]] == ["tenure = owner"] and len(mi["refused"]) == 3 and mi["grade"] == "C", mi)
D3 = nd.with_proposals(D2, [mi])
it3 = nd.ask("interpret", data=D3, options=OPTIONS, statements=[planted_text])[0]["interpretation"]
assert_("interpret by a model: its reading shown as proposed, apart from the record's", sum(x["admissible"] for x in it3["interpretations"]) == 3
        and sum(x["as_proposed"] for x in it3["interpretations"]) == 4, [(x["words"], x["admissible"], x["as_proposed"]) for x in it3["interpretations"]])
graphs = models.Stub({"graphs": [{"name": "via tenure", "arrows": [{"from": "tenure", "to": "k"}, {"from": "age", "to": "k"}], "rationale": ""},
                                 {"name": "loop", "arrows": [{"from": "age", "to": "k"}, {"from": "k", "to": "age"}], "rationale": ""},
                                 {"name": "far", "arrows": [{"from": "wealth", "to": "k"}], "rationale": ""}], "rivals": []})
ms = nd.structure(D2, graphs, described=d2)
assert_("structure by a model: a graph over the data's columns kept; a cycle and an unknown column refused", [g["name"] for g in ms["graphs"]] == ["via tenure"] and len(ms["refused"]) == 2, ms)
