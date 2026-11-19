"""Checks of multilateral practice at the API's edge (api/cpp/science/multilateral.hpp, api/python/science/multilateral.py), run by
api/tests/run-tests.mjs where Multilateral Phronimos has built the engine. Invented records, each said to be: an undertaking whose
secretariat holds authority together; a partnership whose parties' criteria are met, arguable and not observed. Then the Charter's
rules, ideal points, bargaining, a division of issues and a joint gain; and, where the data library has read it, the Migration MPTF
as IATI records it. One JSON object a line, as check_client.py reports.

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
