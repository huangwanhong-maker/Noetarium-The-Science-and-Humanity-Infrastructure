"""Checks of allocation at the API's edge (science/allocation.py and api/cpp/science/allocation.hpp), run by api/tests/run-tests.mjs
where the Urban Studio's plug-in *Allocation and justice* has built its engine: the declarations and their refusals; a round run with
its reasons and who was reached first; a judgement's undominated options and pivot; a procedure replayed with an appeal heard for
everyone and a lottery that repeats; claims, a budget by votes, a reserve system, and who finally gains, a refused part refused alone;
and a stand-in model's proposals, kept or refused, a drafted statement refused for a number it invented. Resources of many kinds
allocated by evolution (ADR 0137): declarations refused where they are wrong; a type verified and one that breaks its law refused; a
problem composed; a search whose kept allocations are lawful, compared under principles, with order found to matter and its record
verified; Bradley–Terry strengths recovering who is preferred; a stand-in model's pairwise judgements, each pair shown both ways, of
grade C. Economic evaluators (ADR 0141): habits calibrated to accounts and a velocity, one beyond reach refused; a linear law
recovered from a record it made; allocations judged in an economy of places, under a law of holdings and by learned laws; an
unknown kind refused; a budget as orders of its projects, the rules on or behind the front. One JSON object a line, as
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
assert_("a carrier that is not known is refused", refused(lambda: al.resource("money", al.simple_type("money"), "auction", 10)))
assert_("a fixed carrier without the recipients' demand is refused", refused(lambda: al.resource("money", al.simple_type("money"), "fixed", 10)))
assert_("units met in part are refused (only claims are)", refused(lambda: al.resource("crews", al.simple_type("crews", "natural"), "units", 2, partial=True)))
assert_("a precedence naming a recipient the problem lacks is refused",
        refused(lambda: al.problem("p", [al.entity("a"), al.entity("b")], [], precedence=[{"before": "a", "after": "z"}])))
assert_("an evaluator with no terms is refused", refused(lambda: al.evaluator("value", [])))

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
# ---- resources of many kinds allocated by evolution (ADR 0137) -------------------------------------------------------------------
EV_P = al.problem("three areas (declared)", [al.entity("a", "Area A", population=300, need=1.0), al.entity("b", "Area B", population=500, need=1.4),
                                              al.entity("c", "Area C", population=200, need=0.8)],
                  [al.resource("money", al.simple_type("money", "natural"), "unit_claims", 5, caps=[3, 3, 3], partial=True),
                   al.resource("crews", al.simple_type("crews", "natural"), "units", 2),
                   al.resource("duties", al.simple_type("maintenance duties", "natural", disposable=False), "units", 2),
                   al.resource("site", al.items_type("sites", ["river site"]), "items", {"river site": 1})],
                  attributes=[al.attribute("canopy direction", "circle")], precedence=[{"before": "a", "after": "b", "why": "drainage first"}],
                  price={"resource": "money", "start": 1, "factor": 1.25})
EV_E = [al.evaluator("benefit", [al.term("works", "money", "log1p", weight="need"), al.term("amount", "crews", "linear", coefficient=0.5, weight="need"),
                                 al.term("amount", "duties", "square", coefficient=-0.3),
                                 al.term("items", "site", "linear", coefficient=1.2, align={"attribute": "canopy direction", "toward": [0, -1]})])]
if al.ENGINE.program.exists():
    try:
        V = al.verify_type(al.simple_type("money", "natural"))
        assert_("a resource type verified: lawful, every law with its method", V["lawful"] and V["holds"] >= 5 and all(e["method"] for e in V["entries"]), V)
        broken = al.resource_type("tokens", [al.component("tokens", "natural")], conversions=[{"name": "mint", "from": {"tokens": 0}, "to": {"tokens": 1}}],
                                  laws=[{"name": "tokens are conserved", "weights": {"tokens": 1}, "conserved": True}])
        B = al.verify_type(broken)
        assert_("a type whose conservation a conversion breaks: not lawful, the law failing", not B["lawful"] and B["fails"] >= 1, B)
        C = al.compose(EV_P)
        assert_("a problem composed: the genotype's parts, from claims to the order", [p["name"] for p in C["parts"]] ==
                ["money claims", "crews units", "duties units", "site items", "canopy direction", "order"], [p["name"] for p in C["parts"]])
        comparing = {"metric": "benefit", "per": "population", "principles": [al.principle("total"), al.principle("leximin")]}
        S = al.evolve(EV_P, EV_E, {"generations": 150, "mu": 40, "seed": 3, "keep": 5}, comparing)
        kept = S["kept"]
        assert_("a search keeps lawful allocations, the duties all assigned", kept and all(k["lawful"] for k in kept) and all(sum(k["amounts"]["duties"]) == 2 for k in kept),
                [(k["lawful"], k["amounts"]["duties"]) for k in kept])
        assert_("the kept are compared under the principles, never summed", S["comparison"] and S["comparison"]["principles"] == ["total", "leximin"] and S["comparison"]["maximal"], S["comparison"])
        assert_("order is found to matter, with a witness", S["order"]["may_matter"] and S["order"]["witnesses"], S["order"])
        assert_("the record is sealed and verifies", S["verified"] and S["record"][0]["what"] == "declare" and S["head"] == S["record"][-1]["seal"], len(S["record"]))
        assert_("unlawful allocations met were refused by kategoria's verdict", S["counts"]["unlawful"] > 0, S["counts"])
        import random
        rng = random.Random(7)
        truth = [1.0, 0.0, -1.0]
        J = []
        for _ in range(600):
            i, j = rng.sample(range(3), 2)
            import math
            p_first = math.exp(truth[i]) / (math.exp(truth[i]) + math.exp(truth[j]))
            J.append({"first": i, "second": j, "first_preferred": rng.random() < p_first})
        T = al.strengths(3, J, order_effect=False)
        assert_("Bradley–Terry strengths recover who is preferred, in order", T["log_strength"][0] > T["log_strength"][1] > T["log_strength"][2] and T["converged"], T)
        always_first = models.Stub({"preferred": "first", "reason": "shown first"})
        P = al.judge_pairs(EV_P, S, "Serve the areas in need fairly and well.", provider=always_first, top=3, samples=1, audit=1.0)
        assert_("a model's pairwise judgements: each pair shown both ways, audited, of grade C", P["grade"] == "C" and len(P["judgements"]) == 6 and P["asked"] == 12
                and P["audit"]["pairs"] == 3 and P["audit"]["agreed"] == 3, {k: P[k] for k in ("asked", "audit")})
        T2 = al.strengths(P["items"], P["judgements"], prior=0.5)
        assert_("a model that always prefers what it is shown first: the order effect is read, the allocations not told apart", T2["order_effect"] > 3 and max(T2["log_strength"]) - min(T2["log_strength"]) < 0.2, T2)
        assert_("the model is shown the allocations and the purpose, never the search's metrics", "benefit" not in always_first.asked[0]["prompt"] and "Serve the areas" in always_first.asked[0]["prompt"])
        assert_("a purpose too short to judge against is refused", refused(lambda: al.judge_pairs(EV_P, S, "fair", provider=always_first)))
    except al.EngineError as e:
        assert_("the engine answered evolve", False, e)

# ---- economic evaluators, calibration and budgets as orders (ADR 0141) -----------------------------------------------------------
assert_("a place without households is refused", refused(lambda: al.place("north", 0, 2)))
assert_("a habit that is not a share is refused", refused(lambda: al.habits(spend=1.5)))
assert_("habits the economy does not have are refused", refused(lambda: al.habits(thrift=0.2)))
assert_("an economy with no places is refused", refused(lambda: al.economy([])))
assert_("a record of fewer than four states is refused", refused(lambda: al.laws([[1], [2], [3]], [[0], [0]])))
assert_("a budget's groups for some voters only are refused", refused(lambda: al.budget_orders(10, [3, 4], [[0], [1]], groups=[0])))
EC_PLACES = [al.place("north", 4, 2, 2000, 500), al.place("south", 4, 2, 2000, 500), al.place("east", 4, 2, 2000, 500)]
EC_ACCOUNTS = {"local": 0.936239, "wages": 0.232759, "supplies": 0.461391, "imported": 0.077657, "source": "Eurostat naio_10_cp1750, EU27_2020, 2022"}
EC_P = al.problem("three places (declared)", [al.entity("north", population=400, need=1.2, money=0.6, claim=150),
                                               al.entity("south", population=300, need=0.8, money=1.0, claim=150),
                                               al.entity("east", population=200, need=1.0, money=0.4, claim=150)],
                  [al.resource("fund", al.simple_type("fund", "natural"), "claims", 300, partial=True)])
if al.ENGINE.program.exists():
    try:
        K = al.calibrate(EC_PLACES, EC_ACCOUNTS, 0.051319, habits=al.habits(short_below=0))
        assert_("habits calibrated: the accounts' shares read exactly, the velocity matched within 0.1%",
                K["habits"]["local"] == round(EC_ACCOUNTS["local"], 6) and K["habits"]["wages"] == round(EC_ACCOUNTS["wages"], 6)
                and abs(K["achieved"] / K["target"] - 1) < 1e-3 and K["lowest"] < K["target"] < K["highest"], K)
        assert_("... and what the records leave unidentified is said, the money held idle among it", len(K["unidentified"]) == 4 and any("idle" in u for u in K["unidentified"]), K["unidentified"])
        try:
            al.calibrate(EC_PLACES, EC_ACCOUNTS, 50.0, habits=al.habits(short_below=0))
            beyond = False
        except al.EngineError as e:
            beyond = "beyond what the share of holdings spent can reach" in str(e)
        assert_("a velocity beyond the places' reach is refused, with the range they can reach", beyond)
        a, b, c = 0.9, 1.0, 0.5
        X, U = [[10.0, 20.0]], []
        for t in range(12):
            u = [float((3 * t) % 7), float((5 * t) % 4)]
            U.append(u)
            X.append([a * X[-1][i] + b * u[i] + c for i in range(2)])
        L = al.learn(X, U, resamples=8)
        assert_("a linear law learned from a record it generated exactly: its coefficients recovered",
                abs(L["fit"]["a"] - a) < 1e-5 and abs(L["fit"]["b"] - b) < 1e-5 and abs(L["fit"]["c"] - c) < 1e-4 and L["beats_persistence"], L["fit"])
        econ = al.economy(EC_PLACES, resource="fund", habits=K["habits"], unit=10, spacing=1, periods=40)
        S = al.evolve(EC_P, [econ], {"generations": 20, "mu": 12, "seed": 1, "keep": 4})
        k0 = S["kept"][0]
        m = {x["name"]: x for x in k0["metrics"]}
        assert_("in an economy of places: the kept balance their accounts, and the money re-spent is its places' sum",
                all(next(x for x in k["metrics"] if x["name"] == "accounts that do not balance")["value"] == 0 for k in S["kept"])
                and abs(-m["local re-spending lost"]["value"] - sum(m["local re-spending lost"]["units"])) < 1e-3, m)
        assert_("... with LM3 and the share that left as reports, of the standing computed", m["LM3 of the grants"]["role"] == "report"
                and m["local re-spending lost"]["standing"] == "computed" and 1 < m["LM3 of the grants"]["value"] < 3, m)
        dens = al.density("population", "money", resource="fund", unit=0.002, line=0.5)
        S = al.evolve(EC_P, [dens], {"generations": 10, "mu": 10, "seed": 1, "keep": 3})
        d0 = {x["name"]: x for x in S["kept"][0]["metrics"]}
        assert_("under a law of holdings with a mean field: a share below the line, nothing carried past the grid",
                0 <= d0["share holding less than the line"]["value"] <= 1 and d0["units carried past the grid's end"]["value"] < 1e-6, d0)
        LX = [[10.0, 20.0, 5.0]]
        LU = []
        for t in range(16):   # grants of every size, then none, so that the record spans where the laws are asked to go
            u = [float(40 * ((t + i) % 5)) if t < 8 else 0.0 for i in range(3)]
            LU.append(u)
            LX.append([0.8 * LX[-1][i] + 0.01 * u[i] + 1 for i in range(3)])
        lw = al.laws(LX, LU, resource="fund", resamples=6, after=3, margin=0.2)   # states drift a little below the record's least as grants stop: 20% of each range allowed, declared
        S = al.evolve(EC_P, [lw], {"generations": 10, "mu": 10, "seed": 1, "keep": 3})
        l0 = {x["name"]: x for x in S["kept"][0]["metrics"]}
        assert_("under laws learned from a record: the worth an estimate, and the first kept within the record's support",
                l0["worth lost under the learned laws"]["standing"] == "estimated" and l0["steps beyond the record's support"]["value"] == 0, l0)
        SC_P = al.problem("six places (declared)", [al.entity(f"p{i}", population=4, claim=float(4 + (i * 5) % 7)) for i in range(6)],
                          [al.resource("fund", al.simple_type("fund", "natural"), "fixed", 18, demand="claim", partial=True)])
        SC_PLACES = [al.place(f"p{i}", 4, 2, 2000 + 3000 * (i % 3), 500) for i in range(6)]
        sc = al.economy(SC_PLACES, resource="fund", habits=al.habits(short_below=6000), unit=100, spacing=2, periods=30,
                        screen={"alpha": 0.3, "keep": 4, "warm": 16, "refit": 8})
        S = al.evolve(SC_P, [sc], {"generations": 30, "mu": 12, "seed": 2, "keep": 4})
        scr = S.get("screened") or []
        assert_("an economy behind a stand-in: what it ran, screened and covered is answered, and every kept allocation was run",
                len(scr) == 1 and scr[0]["runs"] > 0 and 0 <= scr[0]["coverage"] <= 1 and scr[0]["runs"] + scr[0]["screened"] <= S["counts"]["genotypes"]
                and all(any(m["name"] == "accounts that do not balance" for m in k["metrics"]) for k in S["kept"]), scr)
        assert_("a stand-in declaring what it does not know is refused", refused(lambda: al.economy(SC_PLACES, screen={"confidence": 0.9})))
        bad = dict(econ, kind="oracle")
        try:
            al.evolve(EC_P, [bad], {"generations": 2, "mu": 4})
            unknown = False
        except al.EngineError as e:
            unknown = "kind" in str(e)
        assert_("an evaluator of a kind the engine does not know is refused", unknown)
        costs = [3, 4, 5, 2, 6]
        ballots = [[0, 1], [1, 2], [2], [3, 4], [0, 4], [4], [1, 3]]
        O = al.budget_orders(10, costs, ballots, groups=[0, 0, 1, 1, 1, -1, 0], group_names=["a", "b"], settings={"generations": 30, "mu": 20, "seed": 1})
        G = al.budget(10, costs, ballots, rules=["greedy"])
        assert_("a budget as orders: greedy read as the order by approvals is nomos's greedy",
                sorted(O["greedy"]["funded"]) == sorted(next(r for r in G["rules"] if r["rule"] == "greedy")["funded"]), (O["greedy"], G["rules"]))
        behind = all(any(f["share_with_nothing"] <= r["share_with_nothing"] + 1e-12 and f["approvals"] >= r["approvals"] for f in O["front"])
                     for r in (O["greedy"], O["greedy_per_cost"], O["equal_shares"]))
        assert_("... the rules' orders proposed, each rule on or behind the front; Equal Shares an order; the record sealed",
                behind and O["equal_shares_is_an_order"] and O["verified"], O["front"])
    except al.EngineError as e:
        assert_("the engine answered the economic evaluators", False, e)
else:
    skip("the economic evaluators (the engine is not built)")

if al.ENGINE.program.exists():
    res = al.run(R)
    honest = models.Stub({"text": "Of the 1200, north received 500 and south 400 to reach 100 a head; east received 300 by population."})
    assert_("a drafted statement holding only the result's numbers is kept as a proposal", al.draft_reasons(R, res, provider=honest)["status"] == "proposed")
    invented = models.Stub({"text": "North received 500, which is 37% more than last year."})
    assert_("a drafted statement inventing a number is refused", refused(lambda: al.draft_reasons(R, res, provider=invented)))
