"""Checks of multilateral practice at the API's edge (api/cpp/science/multilateral.hpp, api/python/science/multilateral.py), run by
api/tests/run-tests.mjs where Multilateral Phronimos has built the engine. Invented records, each said to be: an undertaking whose
secretariat holds authority together; a partnership whose parties' criteria are met, arguable and not observed. Then the Charter's
rules, ideal points, bargaining, a division of issues and a joint gain; the affairs of ADR 0134 (a question under Article 27(3) as
declared, a negotiation in drafts, events relaying one wire, mandates, reviews, institutions and reciprocity); and, where the data
library has read it, the Migration MPTF as IATI records it. One JSON object a line, as check_client.py reports.

    python api/tests/check_multilateral.py
"""

from __future__ import annotations

import json
import math
import random
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import multilateral  # noqa: E402
from science.engines import EngineError  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "multilateral: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


UNDERTAKING = {
    "name": "an invented programme of two cities",
    "types": [{"name": "authorises", "meaning": "gives authority", "authority": True},
              {"name": "relies", "meaning": "relies on claims", "evidential": True},
              {"name": "challenges", "meaning": "may contest", "challenge": True}],
    "participants": [{"id": "fund", "name": "the fund", "kind": "organisation", "constituted": True},
                     {"id": "sec", "name": "the secretariat", "kind": "organisation"},
                     {"id": "north", "name": "the northern office", "kind": "city"},
                     {"id": "south", "name": "the southern office", "kind": "city"},
                     {"id": "model", "name": "a model", "kind": "AI system"},
                     {"id": "panel", "name": "the panel", "kind": "forum", "constituted": True}],
    "ties": [{"type": "authorises", "from": "fund", "to": "sec", "since": "2025-01-01"},
             {"type": "authorises", "from": "sec", "to": "north", "since": "2025-02-01"},
             {"type": "authorises", "from": "sec", "to": "south", "since": "2025-02-01"},
             {"type": "relies", "from": "sec", "to": "model", "since": "2025-03-01"},
             {"type": "challenges", "from": "north", "to": "panel", "since": "2025-01-01"}],
    "observations": [{"type": "authorises", "design": "the register of delegations", "since": "2025-01-01", "until": "2026-01-01",
                      "covered": ["fund", "sec", "north", "south"]}],
    "decisions": [{"id": "d1", "label": "tariffs set", "when": "2025-06-01", "authorities": ["sec"], "evidence": ["model"], "affected": ["north", "south"], "venues": ["panel"]}],
}

PARTNERSHIP = {
    "name": "an invented partnership", "registered": "2024-01-01",
    "members": [{"party": "a", "role": "city"}, {"party": "b", "role": "city"}, {"party": "c", "role": "funder"}],
    "goals": [{"id": "g", "text": "clean water", "set_by": ["a", "b"]}],
    "commitments": [{"id": "c1", "by": "a", "text": "a plant", "goal": "g", "made": "2024-02-01", "due": "2025-12-31",
                     "indicator": {"name": "people served", "unit": "people", "baseline": 0, "target": 1000},
                     "resources": [{"party": "c", "kind": "grant", "unit": "EUR", "amount": 1e6, "secured": True}]}],
    "reports": [{"commitment": "c1", "when": "2025-06-30", "value": 1200}],
    "contributions": [{"party": "c", "commitment": "c1", "kind": "grant", "unit": "EUR", "when": "2024-03-01", "amount": 1e6}],
    "dimensions": [{"name": "water", "kind": "mission", "unit": "days", "scale": "ratio"},
                   {"name": "standing", "kind": "indirect", "scale": "ordinal", "levels": ["less", "same", "more"]}],
    "values": [{"party": "a", "dimension": "water", "value": 70, "lower": 60, "upper": 80, "error": 5},
               {"party": "b", "dimension": "water", "value": 70, "lower": 60, "upper": 80, "error": 5}],
    "interests": [{"party": "a", "dimension": "water", "threshold": 50, "reading": "days fit for bathing", "source": "its council"},
                  {"party": "b", "dimension": "water", "threshold": 75, "reading": "days fit for bathing", "source": "its mayor"},
                  {"party": "c", "dimension": "standing", "threshold": 1, "reading": "its reputation", "source": "its strategy"}],
}


def tenths(issue: str) -> dict:
    return {"id": issue, "text": issue, "options": [{"id": str(k), "text": f"{k}/10"} for k in range(11)]}


def linear(issue: str, a: float, b: float) -> dict:
    return {str(k): a * k + b for k in range(11)}


def affairs() -> None:
    """The affairs of an undertaking (ADR 0134), each operation on an invented record."""
    rule = multilateral.declared("article-27-3")
    acts = {m: "no" for m in rule["members"]}
    for m in multilateral.PERMANENT[:4] + [f"elected member {k}" for k in range(1, 6)]:
        acts[m] = "yes"
    acts[multilateral.PERMANENT[4]] = "abstain"
    o = multilateral.ask("outcome", rule=rule, question={"id": "q1", "acts": acts})
    assert_("nine affirmative votes with a permanent member abstaining: adopted, the words alone set aside by the Court's reading",
            o["answer"] == "adopted" and len(o["set_aside"]) == 1 and len(o["decisive"]) == 10 and o["tally"]["met"], o)
    two = {i["readings"]["concurring vote"]: i["gives"] for i in o["interpretations"]}
    assert_("each interpretation says what it gives and whether it is admissible",
            two == {"an affirmative vote": "not adopted", "any vote but a negative one": "adopted"} and [i["admissible"] for i in o["interpretations"]] == [False, True], o["interpretations"])
    g = multilateral.ask("rule_game", rule=rule)
    assert_("the declared rule's game: a permanent member's Shapley–Shubik index 421/2145, each a veto",
            abs(g["members"][0]["shapley_shubik"] - 421 / 2145) < 1e-12 and all(x["veto"] for x in g["members"][:5]) and not g["members"][5]["veto"], g["members"][:6])
    w = multilateral.declared("wto-ix-1")
    q = {"id": "q2", "records": {"consensus": {"procedure": "consensus", "acts": {m: "yes" for m in w["members"]} | {"member 2": "objection"}},
                                 "vote": {"procedure": "vote", "acts": {m: "yes" for m in w["members"]} | {"member 2": "no", "member 3": "no"}}}}
    ow = multilateral.ask("outcome", rule=w, question=q)
    assert_("an objection blocks the WTO's consensus, and a majority of the votes cast then decides it", ow["answer"] == "adopted"
            and ow["tally"]["kind"] == "fallback" and not ow["tally"]["parts"][0]["met"] and ow["tally"]["parts"][1]["met"], ow["tally"])

    negotiation = {
        "name": "two issues traded at different rates", "issues": [tenths("X"), tenths("Y")],
        "parties": [{"id": "A", "valuation": {"values": {"X": linear("X", 10, 0), "Y": linear("Y", 2, 0)}, "none": {"X": 60, "Y": 10}}},
                    {"id": "B", "valuation": {"values": {"X": linear("X", -2, 20), "Y": linear("Y", -10, 100)}, "none": {"X": 10, "Y": 60}}}],
        "drafts": [{"id": "second reading", "open": {"X": ["8"]}}, {"id": "chair's text", "open": {"X": ["8"], "Y": ["4", "5"]}}],
    }
    n = multilateral.ask("negotiation", negotiation=negotiation)
    assert_("linkage creates a zone of sixteen packages where neither issue alone has one", n["linkage"]["created"] and n["linkage"]["linked"] == 16
            and n["every_option"]["clear"] == 16, n["linkage"])
    assert_("the second reading leaves four packages, and the chair's text empties the zone, said so", n["drafts"][0]["zone"]["clear"] == 4
            and not n["drafts"][1]["zone"]["agreeable"] and any("emptied" in x for x in n["drafts"][1]["notes"]), n["drafts"][1]["notes"])
    assert_("value claimed between the two is read on a declared basis, its Nash point a package of the zone",
            "midpoints" in n["claimed"]["basis"] and n["claimed"]["nash"]["package"].startswith("X: "), n["claimed"])
    assert_("each party's win-set is counted", [x["party"] for x in n["win_sets"]] == ["A", "B"] and n["win_sets"][0]["of"] == 121, n["win_sets"])

    reports = [{"id": f"r{k}", "source": f"outlet {k}", "origin": "the wire", "passage": "…"} for k in range(1, 51)]
    e = multilateral.ask("events", events=[{"id": "summit", "category": "CONSULT", "actors": ["north"], "targets": ["south"], "date": "2026-02-01", "reports": reports}],
                         channels={"origins": {"the wire": [0.8, 0.05]}, "relay": [0.9, 0.1]})
    ev = e["events"][0]
    assert_("fifty outlets relaying one wire: one origin, its force just under the wire's 16, the count as independent overstating it",
            ev["origins"] == 1 and 15.99 < ev["ratio"] <= 16 and ev["overstatement"] > 1e30 and e["series"][0]["periods"][0]["events"]["verbal cooperation"] == 1, ev)
    assert_("and the sign says it is attested once", any(x["kind"] == "one origin" for x in e["signs"]), e["signs"])

    registry = {"mandates": [
        {"id": "m1", "issued_by": "assembly", "addressees": ["secretariat"], "themes": ["water"], "begins": "2020-01-01", "ends": "2022-01-01", "resourced": True},
        {"id": "m2", "issued_by": "assembly", "addressees": ["secretariat", "agency"], "themes": ["water", "climate"], "begins": "2022-01-01",
         "earlier": [{"link": "renews", "mandate": "m1"}]}]}
    md = multilateral.ask("mandates", registry=registry, at="2024-01-01", undertaking=UNDERTAKING)
    m2 = next(x for x in md["mandates"] if x["id"] == "m2")
    assert_("a renewed mandate's lineage, its creep, and its signs against an undertaking", m2["lineage"]["roots"] == ["m1"] and m2["lineage"]["themes_added"] == ["climate"]
            and next(x for x in md["mandates"] if x["id"] == "m1")["state"] == "renewed"
            and {s["kind"] for s in md["signs"]} >= {"open-ended", "unresourced", "creep", "authority", "addressee"}, md["signs"])

    record = {"cycles": [{"id": "c1", "opened": "2017-05-01", "closed": "2017-09-30"}, {"id": "c2", "opened": "2022-05-01", "closed": "2022-09-30"}],
              "recommendations": [{"id": "r1", "cycle": "c1", "from": "south", "to": "north", "theme": "schools", "level": 5},
                                  {"id": "r2", "cycle": "c2", "from": "south", "to": "north", "theme": "schools", "level": 5}],
              "answers": [{"recommendation": "r1", "date": "2017-09-20", "response": "supported"}],
              "progress": [{"recommendation": "r1", "date": "2019-01-01", "by": "north", "done": "implemented"}]}
    rv = multilateral.ask("reviews", record=record, at="2023-01-01")
    kinds = {(x["kind"], x["where"]) for x in rv["signs"]}
    assert_("an implementation claimed by the party reviewed alone and recommended again", rv["recommendations"][0]["standing"] == "claimed"
            and rv["repeats"] == [{"later": "r2", "earlier": "r1", "declared": False}] and ("claimed", "recommendation r1") in kinds
            and ("recommended again", "recommendation r1") in kinds and ("unanswered", "recommendation r2") in kinds, rv["signs"])

    landscape = {"parties": ["a", "b", "c"],
                 "institutions": [{"id": "REG", "issues": ["trade"]}, {"id": "SUB", "issues": ["trade"]}],
                 "memberships": [{"party": p, "institution": "REG"} for p in "abc"] + [{"party": p, "institution": "SUB"} for p in "ab"]}
    transfers = [{"from": "a", "to": "b", "date": "2026-01-05", "value": 10}, {"from": "b", "to": "c", "date": "2026-01-10", "value": 10},
                 {"from": "c", "to": "a", "date": "2026-01-15", "value": 10}]
    it = multilateral.ask("institutions", landscape=landscape, at="2026-01-01", transfers=transfers, windows=[30])
    pair = it["complexes"][0]["pairs"][0]
    rec = it["reciprocity"][0]
    assert_("every party belongs to the regional body, the subregional one nested in it, and a cycle of gifts diffusely reciprocal and not specifically",
            pair["relation"] == "nested" and pair["inner"] == "SUB" and abs(rec["pairwise"] - 1) < 1e-15 and abs(rec["group"]) < 1e-15
            and {"premise": [], "conclusion": ["REG"]} in it["regularities"], (pair, rec, it["regularities"]))


def main() -> None:
    try:
        multilateral.ENGINE.status()
    except Exception:
        pass
    if not multilateral.ENGINE.status().get("built"):
        emit({"kind": "skip", "name": "multilateral: the engine is not built (python applications/multilateral-phronimos/engine.py build)"})
        return

    u = multilateral.ask("undertaking", undertaking=UNDERTAKING, at="2025-07-01")
    kinds = {(d["kind"], d["where"]) for d in u["diagnoses"]}
    assert_("an undertaking reads without fault, its diagnostics found: the secretariat holds authority together, d1 rests on a model alone",
            not u["problems"] and ("succession", "participant sec") in kinds and ("AI dependency", "decision d1 (tariffs set)") in kinds, u["diagnoses"])
    layer = next(x for x in u["layers"] if x["type"] == "authorises")
    assert_("within the register of delegations, a pair with no delegation is absent; outside it, not observed",
            layer["present"] == 3 and layer["absent"] == 9 and layer["not_observed"] == 18, layer)
    south = next(p for p in u["positions"] if p["participant"] == "south")
    assert_("the southern office is authorised by the secretariat and can contest nothing", south["authorised_by"] == ["sec"] and south["may_challenge"] == [], south)
    assert_("the timeline runs in order of dates, the decision among the ties", [e["date"] for e in u["timeline"]] == sorted(e["date"] for e in u["timeline"])
            and any(e["kind"] == "decision" for e in u["timeline"]), u["timeline"][:3])

    p = multilateral.ask("partnership", partnership=PARTNERSHIP, at="2026-01-31", standard={"kind": "standing", "least": "clear"})
    state = p["states"][0]
    assert_("the plant reached 1,200 people before its date: honoured", state["state"] == "honoured" and abs(state["progress"] - 1.2) < 1e-12, state)
    screen = p["screens"][0]
    assert_("its screen is undecided only on whether it is achievable, no one having judged it",
            screen["verdict"]["status"] == "undecided" and [c["name"] for c in screen["checks"] if c["verdict"]["status"] != "holds"] == ["achievable"], screen)
    ben = {b["party"]: b for b in p["benefits"]}
    assert_("a's benefit is clear, b's arguable (its 75 days within [60, 80]), c's not observed",
            ben["a"]["standing"] == "clear" and ben["b"]["standing"] == "arguable" and ben["c"]["standing"] == "undecided" and ben["c"]["unobserved"] == ["standing"], ben)
    pb = 1 - 0.5 * math.erfc(-1 / math.sqrt(2))
    assert_("b's measure is P(days ≥ 75) under its estimate 70 ± 5, 1 − Φ(1)", abs(ben["b"]["measure"]["value"] - pb) < 1e-12, ben["b"]["measure"])
    assert_("so mutual benefit fails at b, by the standard of standing", p["mutual"]["verdict"]["status"] == "fails" and "b" in p["mutual"]["verdict"]["reason"], p["mutual"])
    alt = multilateral.ask("partnership", partnership=PARTNERSHIP, at="2026-01-31", standard={},
                           alternatives=[{"name": "x", "values": {"water": {"value": 90}, "standing": {"value": 1}}},
                                         {"name": "y", "values": {"water": {"value": 70}, "standing": {"value": 2}}}],
                           rule={"name": "worth", "declared_by": "an analyst", "reason": "an example", "weights": {"water": 1, "standing": 10}})
    pair = alt["alternatives"]["pairs"][0]
    assert_("two alternatives that cross are not ranked, and a rule that weighs an ordinal dimension is refused",
            pair["relation"] == "crosses" and all(s["refused"] for s in alt["alternatives"]["sums"]) and alt["alternatives"]["undominated"] == ["x", "y"], alt["alternatives"])

    sc = multilateral.ask("rule", **multilateral.rule_request("security-council"))
    pw = {x["member"]: x for x in sc["power"]}
    phi = 84 * math.factorial(8) * math.factorial(6) / math.factorial(15)
    assert_("the Security Council by Article 27(3): an elected member's Shapley–Shubik index is C(9, 3) 8! 6!/15!, the permanent members are its vetoes",
            abs(pw["elected member 1"]["shapley_shubik"] - phi) < 1e-15 and sorted(sc["properties"]["vetoes"]) == sorted(multilateral.PERMANENT), sc["properties"])
    ga = multilateral.ask("rule", **multilateral.rule_request("general-assembly-important"))
    assert_("the General Assembly by Article 18(2), 193 members by generating functions: each holds 1/193", ga["route"] == "generating functions"
            and abs(ga["power"][0]["shapley_shubik"] - 1 / 193) < 1e-12, ga["power"][0])

    rng = random.Random(132)
    xs = [rng.gauss(0, 1) for _ in range(40)]
    items = [(rng.gauss(0, 1), rng.gauss(0, 1.5)) for _ in range(120)]
    votes = [[1 if a + b * x + rng.gauss(0, 1) > 0 else 0 for (a, b) in items] for x in xs]
    anchor = max(range(40), key=lambda i: xs[i])
    ip = multilateral.ask("ideal_points", members=[f"m{i}" for i in range(40)], items=[f"r{j}" for j in range(120)], votes=votes, anchor=f"m{anchor}")
    fitted = [pt["x"] for pt in ip["points"]]
    mx, mf = sum(xs) / 40, sum(fitted) / 40
    r = sum((a - mx) * (b - mf) for a, b in zip(xs, fitted)) / math.sqrt(sum((a - mx) ** 2 for a in xs) * sum((b - mf) ** 2 for b in fitted))
    assert_("ideal points of 40 members on 120 votes recover the true ones (r > 0.95), the anchor on the positive side",
            r > 0.95 and ip["converged"] and next(pt for pt in ip["points"] if pt["member"] == f"m{anchor}")["x"] > 0, f"r = {r}")

    b = multilateral.ask("bargaining", outcomes=[[0, 0], [4, 0], [0, 2]], disagreement=[0, 0])
    assert_("bargaining on u₁ + 2u₂ = 4: Nash and Kalai–Smorodinsky at (2, 1), the egalitarian at (4/3, 4/3)",
            all(abs(u - v) < 1e-12 for u, v in zip(b["nash"] + b["kalai_smorodinsky"] + b["egalitarian"], [2, 1, 2, 1, 4 / 3, 4 / 3])), b)
    d = multilateral.ask("division", issues=["access", "benefits", "monitoring"], a=[50, 30, 20], b=[20, 30, 50])
    assert_("Adjusted Winner on 50/30/20 against 20/30/50: each holds 65 points, the shared issue split", abs(d["points_a"] - 65) < 1e-12
            and abs(d["points_b"] - 65) < 1e-12 and [s["split"] for s in d["shares"]] == [False, True, False], d)
    # w(S) = max(0, 200 − Σ claims outside S) for claims of 100, 200 and 300: only {b, c} (100) and {a, b, c} (200) are worth anything
    c = multilateral.ask("coalitional", members=["a", "b", "c"],
                         worths=[{"coalition": ["b", "c"], "worth": 100}, {"coalition": ["a", "b", "c"], "worth": 200}])
    shares = {x["member"]: x["nucleolus"] for x in c["shares"]}
    assert_("a joint gain divided: the claims game of 200 against 100, 200 and 300 has the Talmud's nucleolus (50, 75, 75)",
            abs(shares["a"] - 50) < 1e-9 and abs(shares["b"] - 75) < 1e-9 and abs(shares["c"] - 75) < 1e-9, c)
    try:
        multilateral.ask("treaty")
        assert_("an unknown operation is refused, naming those there are", False)
    except EngineError as e:
        assert_("an unknown operation is refused, naming those there are", "undertaking" in str(e), e)

    affairs()

    folder = ROOT / "data" / "cache" / "products" / "multilateral" / "migration-fund"
    if not (folder / "undertaking.json").exists():
        emit({"kind": "skip", "name": "multilateral: the Migration MPTF is not in the cache (python data/products/multilateral_cases.py migration-fund)"})
        return
    und = json.loads((folder / "undertaking.json").read_text(encoding="utf-8"))
    par = json.loads((folder / "partnership.json").read_text(encoding="utf-8"))
    mu = multilateral.ask("undertaking", undertaking=und, at="2026-11-19")
    assert_("the Migration MPTF reads without fault", not mu["problems"], mu["problems"][:3])
    cut = [d for d in mu["diagnoses"] if d["kind"] == "succession" and d["where"].startswith("participant")]
    assert_("its authority runs through the fund alone: the fund is the one participant without whom it falls apart",
            [d["where"] for d in cut] == ["participant migration-mptf"], cut)
    mp = multilateral.ask("partnership", partnership=par, at="2026-11-19", standard={})
    gaps = [s for s in mp["signs"] if s["mode"] == "accountability gap"]
    assert_("its programmes show accountability gaps: the publication holds no indicator for any of them",
            len([g for g in gaps if "no indicator" in g["what"]]) == len(par["commitments"]), len(gaps))


if __name__ == "__main__":
    main()
