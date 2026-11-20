"""Checks of allocation at the API's edge (science/allocation.py and api/cpp/science/allocation.hpp), run by api/tests/run-tests.mjs
where the Urban Studio's plug-in *Allocation and justice* has built its engine: the declarations and their refusals; a round run with
its reasons and who was reached first; a judgement's undominated options and pivot; a procedure replayed with an appeal heard for
everyone and a lottery that repeats; claims, a budget by votes, a reserve system, and who finally gains, a refused part refused alone;
and a stand-in model's proposals, kept or refused, a drafted statement refused for a number it invented. One JSON object a line, as
check_client.py reports.

    python api/tests/check_allocation.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import allocation as al  # noqa: E402
from science import models  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "allocation: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "allocation: " + name})


def refused(f) -> bool:
    try:
        f()
        return False
    except (ValueError, models.Refusal):
        return True


# ---- declarations, without the engine -------------------------------------------------------------------------------------------
assert_("an attribute that is not a number is refused", refused(lambda: al.entity("a", level="high")))
assert_("a stage of an unknown kind is refused", refused(lambda: al.stage("auction", "efficiency")))
assert_("a rationing rule that is not known is refused", refused(lambda: al.stage("claims", "claims", rule="first come")))
assert_("two entities with one id are refused", refused(lambda: al.round_("r", 1, [al.entity("a"), al.entity("a")], [])))
assert_("a shortfall principle without its threshold is refused", refused(lambda: al.principle("shortfall")))

R = al.round_("small grants", 1200, unit="£", resource="grants",
              entities=[al.entity("north", level=60, population=10, group="towns"), al.entity("south", level=90, population=20, group="towns"),
                        al.entity("east", level=120, population=30, group="cities")],
              stages=[al.stage("floor", "sufficiency", who="the panel", why="no place below 100 a head", when="2026-11-01", attribute="level", bound=100),
                      al.stage("proportional", "equality per head", who="the council", why="the rest alike", when="2026-11-01", attribute="population")],
              sources={"level": "the spending returns of 2025", "population": "the census of 2021"})

if not al.ENGINE.program.exists():
    skip("the engine is not built (python applications/urban-studio/engine.py build allocation)")
    sys.exit(0)

try:
    a = al.run(R)
    amounts = {e["id"]: e["amount"] for e in a["entities"]}
    assert_("a floor then the rest by population: north 500, south 400, east 300", amounts == {"north": 500, "south": 400, "east": 300}, amounts)
    assert_("who first: north and south (the floor), then east", [o["entity"] for o in a["order"]] == ["north", "south", "east"], a["order"])
    assert_("each amount with its reasons, naming the principle and who declared it", "sufficiency" in a["entities"][0]["reasons"][0] and "the panel" in a["entities"][0]["reasons"][0], a["entities"][0])
    assert_("the round and its result named by their digests", len(a["digest"]) == 64 and len(a["result_digest"]) == 64)

    C = al.round_("claims", 60, entities=[al.entity("small", claim=10, population=1), al.entity("large", claim=100, population=1)],
                  stages=[al.stage("claims", "claims", attribute="claim", rule="equal_losses")])
    J = al.judge(C, [al.principle("leximin"), al.principle("shortfall", "headcount below 54.8", parameter=54.8, alpha=0)], orders=False)
    und = [J["options"][k] for k in J["undominated"]]
    assert_("a judgement: equal losses dominated, two options undominated", not J["declared_undominated"] and len(und) == 2, J["said"])
    assert_("each undominated option named by how it was made", any(any("equal_awards" in n for n in o["names"]) for o in und) and any(any("talmud" in n for n in o["names"]) for o in und), [o["names"] for o in und])

    acts = [{"act": "declare", "when": "2026-11-02T09:00:00Z", "who": "the council"},
            {"act": "consult", "when": "2026-11-03T09:00:00Z", "who": "the council", "group": "towns", "note": "a meeting"},
            {"act": "consult", "when": "2026-11-03T10:00:00Z", "who": "the council", "group": "cities", "note": "a meeting"},
            {"act": "allocate", "when": "2026-11-10T09:00:00Z", "who": "the council"},
            {"act": "appeal", "when": "2026-11-11T09:00:00Z", "who": "north's clerk", "entity": "north", "kind": "datum", "key": "population", "value": 12, "reason": "a new estate"},
            {"act": "review", "when": "2026-11-12T09:00:00Z", "who": "the panel", "appeal": 0, "upheld": True, "reason": "the estate is occupied"},
            {"act": "allocate", "when": "2026-11-13T09:00:00Z", "who": "the council"},
            {"act": "commit", "when": "2026-11-14T09:00:00Z", "who": "the clerk", "seed": "drawn in public"},
            {"act": "draw", "when": "2026-11-15T09:00:00Z", "who": "the clerk", "seed": "drawn in public", "among": ["north", "south", "east"], "k": 1}]
    P = al.procedure(R, acts)
    status = {r["name"]: r["status"] for r in P["rules"]}
    assert_("a procedure replayed: consistency, publicity, correctability and the lottery hold, the record seals",
            all(status[k] == "holds" for k in ("consistency", "publicity", "correctability", "lottery", "record sealed")), status)
    assert_("relevance and ethicality are left to people", status["relevance"] == "undecided" and status["ethicality"] == "undecided", status)
    assert_("the appeal heard for everyone: all three amounts change", len(P["hearings"][0]["hearing"]["changed"]) == 3, P["hearings"])
    assert_("the journal holds every act, sealed", len(P["journal"]) == 10 and P["head"] == P["journal"][-1]["seal"], len(P["journal"]))
    P2 = al.procedure(R, acts[:4] + acts[4:5])
    assert_("an appeal not heard: correctability fails", next(r for r in P2["rules"] if r["name"] == "correctability")["status"] == "fails")

    T = al.claims(200, [100, 200, 300], rules=["talmud", "proportional"])
    assert_("the Talmud rule on the estate of 200: (50, 75, 75)", T["talmud"] == [50, 75, 75], T)

    approvals = [list(range(10))] * 6 + [list(range(10, 20))] * 4
    B = al.budget(10, [1] * 20, approvals, satisfaction="cardinality")
    by = {r["rule"]: r for r in B["rules"]}
    assert_("a budget by votes: utilitarian greedy leaves the four with nothing, equal shares no one",
            by["greedy"]["voters_with_nothing"] == 4 and by["equal_shares"]["voters_with_nothing"] == 0, {k: v["voters_with_nothing"] for k, v in by.items()})
    assert_("EJR checked for ten voters: broken by greedy, kept by equal shares", by["greedy"]["ejr"]["satisfied"] is False and by["equal_shares"]["ejr"]["satisfied"] is True)

    Rs = al.reserves(2, [{"name": "open", "capacity": 1, "priority": [0, 1]}, {"name": "reserved", "capacity": 1, "priority": [0]}], precedence=[0, 1])
    assert_("hard reserves processed open first leave the reserved unit idle, the three axioms holding",
            Rs["holders"][1] == [] and Rs["axioms"]["eligible"] and Rs["axioms"]["non_wasteful"] and Rs["axioms"]["respects_priorities"], Rs)
    assert_("and who is served under every order is reported", len(Rs["orders"]) == 2, Rs["orders"])

    I = al.incidence(lm3=[100, 80, 40], capitalisation={"amount": 100, "renters_share": 0.6, "landlords_local": 0.5},
                     propagation={"amount": 100, "spent": 1, "kept": 1},
                     network={"positions": ["A", "B", "platform"], "legs": [[0, 2, 10], [2, 0, 3], [1, 2, 10], [2, 1, 8]], "generators": [0, 1]})
    assert_("LM3 of 100, 80 and 40 is 2.2", I["lm3"] == 2.2, I)
    assert_("equal elasticities capitalise half", I["capitalisation"]["share"] == [0.5, 0.5], I["capitalisation"])
    assert_("a series that never leaks is refused alone, the other parts answered", "refused" in I["propagation"] and I["network"]["pooled_ratio"] == 0.55, I)
except al.EngineError as e:
    assert_("the engine answered", False, e)

# ---- a stand-in model's roles ---------------------------------------------------------------------------------------------------
stub = models.Stub({"stages": [{"kind": "floor", "principle": "sufficiency", "why": "first close the shortfalls", "attribute": "level", "rule": "proportional"},
                               {"kind": "claims", "principle": "desert", "why": "reward effort", "attribute": "effort", "rule": "talmud"}]})
p = al.propose_stages("Grants for three places whose spending per head falls short.", R, provider=stub)
assert_("a model's stages: one that reads kept as proposed, grade C; one reading an attribute the entities lack refused",
        len(p["stages"]) == 1 and p["stages"][0]["status"] == "proposed" and p["stages"][0]["grade"] == "C" and len(p["refused"]) == 1, p)
assert_("the model was asked for no numbers", "Propose no numbers" in stub.asked[0]["prompt"])
good = models.Stub({"kind": "datum", "entity": "north", "stage": 0, "key": "population", "value": 12, "reason": "a new estate"})
assert_("an objection restated as an appeal the person may file", al.articulate_appeal("North has more people than counted: a new estate.", R, provider=good)["appeal"]["value"] == 12)
bad = models.Stub({"kind": "datum", "entity": "west", "stage": 0, "key": "population", "value": 12, "reason": "?"})
assert_("an appeal naming an entity the round lacks is refused", refused(lambda: al.articulate_appeal("West was left out entirely.", R, provider=bad)))
if al.ENGINE.program.exists():
    res = al.run(R)
    honest = models.Stub({"text": "Of the 1200, north received 500 and south 400 to reach 100 a head; east received 300 by population."})
    assert_("a drafted statement holding only the result's numbers is kept as a proposal", al.draft_reasons(R, res, provider=honest)["status"] == "proposed")
    invented = models.Stub({"text": "North received 500, which is 37% more than last year."})
    assert_("a drafted statement inventing a number is refused", refused(lambda: al.draft_reasons(R, res, provider=invented)))
