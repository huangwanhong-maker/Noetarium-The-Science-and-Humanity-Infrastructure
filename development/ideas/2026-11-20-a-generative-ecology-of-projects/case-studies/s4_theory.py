"""Study C: a fragment of the theoretical corpus as a site (PLAN.md, Section 4).

    python s4_theory.py calibrate     C1: the harness on statements whose truth is known, with no model
    python s4_theory.py lineages      C2: producers warrant claims; a challenger of each model attacks them
    python s4_theory.py draft         C3: the draft's claim on a scripted seat, attacked by challengers of both models
    python s4_theory.py settle        C2 and C3 carried on the ground, the facts of the tables, and the dossier

The question put to the lineages is posed by the assistant to exercise the site. Asking, reading, accepting and
signing are the inquirer's, and nothing here performs them.
"""
from __future__ import annotations

import json
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "prototypes"))

import theory_site as S                                   # noqa: E402
from ask import CAREFUL, CHEAP, ask, spent                # noqa: E402
from ground import Ground, Refused                        # noqa: E402

OUT = HERE / "results"
MODELS = {"cheap": CHEAP, "careful": CAREFUL}
PRODUCERS = [("cheap", k) for k in range(5)] + [("careful", k) for k in range(2)]

DEFINITIONS = (
    "A transfer network is a finite directed multigraph. Its vertices are positions. Its edges are legs: each leg goes from one "
    "position, its tail, to a different position, its head, and carries a transport U > 0, the value registered at the head for each "
    "unit surrendered at the tail. Several legs may join the same two positions, in either direction.\n"
    "A closed walk returns to the position where it began. It may walk a leg along its direction or against it. Its holonomy is the "
    "product of the transports of the legs it walks along and of the inverses of the transports of the legs it walks against. Its "
    "loop deficit is minus the logarithm of its holonomy, so the deficit vanishes exactly when the holonomy is 1.\n"
    "Prices are a positive number p at every position such that the transport of every leg equals p(head) / p(tail)."
)
QUESTION = ("Which properties of the vocabulary, alone or together, are enough for a transfer network to be priced, and which fall short "
            "of it? Claims with other conclusions from the vocabulary are admitted as well.")
FORMAT = ('A network is written as JSON: {"positions": n, "legs": [[tail, head, "transport"], ...]} with positions numbered from 0, at most '
          '6 positions and 12 legs, and each transport a positive rational written as a string such as "3/2".')

# What follows from the note's propositions and from elementary facts about graphs, as rules whose conclusions are added to a
# set of premises until nothing changes. A claim that these rules yield is labelled as derived; the labelling claims no more.
RULES = [
    (["flat"], "priced"), (["priced"], "flat"), (["unit"], "flat"), (["acyclic"], "flat"), (["flat"], "directed_flat"),
    (["flat"], "triangles_flat"), (["flat"], "digons_flat"), (["simple"], "digons_flat"), (["acyclic"], "simple"),
    (["strongly_connected"], "connected"), (["complete"], "connected"), (["directed_flat", "strongly_connected"], "flat"),
    (["triangles_flat", "digons_flat", "complete"], "flat"),
]


def derived(premises, conclusion) -> bool:
    known = set(premises)
    changed = True
    while changed:
        changed = False
        for needs, gives in RULES:
            if gives not in known and all(n in known for n in needs):
                known.add(gives)
                changed = True
    return conclusion in known


def load(name, default=None):
    path = OUT / name
    return json.loads(path.read_text(encoding="utf-8")) if path.exists() else default


def save(name, data) -> None:
    OUT.mkdir(exist_ok=True)
    (OUT / name).write_text(json.dumps(data, indent=1, ensure_ascii=False), encoding="utf-8")


# ---- C1: calibration ----------------------------------------------------------------------------------------

TRUE = [(["flat"], "priced"), (["priced"], "flat"), (["acyclic"], "priced"), (["unit"], "priced"), (["flat"], "directed_flat"),
        (["flat"], "triangles_flat"), (["flat"], "digons_flat"), (["directed_flat", "strongly_connected"], "priced"),
        (["triangles_flat", "digons_flat", "complete"], "priced"), (["triangles_flat", "complete", "simple"], "priced")]
FALSE = [(["directed_flat"], "priced"), (["triangles_flat"], "priced"), (["digons_flat"], "priced"), (["triangles_flat", "digons_flat"], "priced"),
         (["symmetric", "digons_flat"], "priced"), (["connected", "simple"], "priced"), (["complete", "digons_flat"], "priced"),
         (["strongly_connected", "triangles_flat", "digons_flat"], "priced")]


def stage_calibrate() -> None:
    tables = S.Tables()
    names = tables.names
    priced, flat = names.index("priced"), names.index("flat")
    agree = sum(1 for _, row in tables.small + tables.large if row[priced] == row[flat])
    rows = []
    for truth, items in (("true", TRUE), ("false", FALSE)):
        for premises, conclusion in items:
            small, met_small = tables.counter(premises, conclusion, "small")
            large, met_large = tables.counter(premises, conclusion, "large")
            rows.append({"premises": premises, "conclusion": conclusion, "known": truth, "derived_by_the_rules": derived(premises, conclusion),
                         "countermodel_in_the_table": None if small is None else S.written(small),
                         "countermodel_among_the_larger": None if large is None else S.written(large),
                         "premises_met": {"table": met_small, "larger": met_large}})
    import random
    from fractions import Fraction
    rng = random.Random(7)
    products = 0
    for _ in range(2000):                                    # the note's Proposition 1.5 on cycles and flows drawn at random
        k = rng.randrange(2, 7)
        legs = [[i, (i + 1) % k, str(Fraction(rng.randrange(1, 9), rng.randrange(1, 9)))] for i in range(k)]
        net = S.network({"positions": k, "legs": legs})
        flows = [str(Fraction(rng.randrange(1, 9), rng.randrange(1, 9))) for _ in range(k)]
        product = Fraction(1)
        for r in S.retentions(net, list(range(k)), flows):
            product *= r
        products += product == S.holonomy(net, [(i, 1) for i in range(k)])
    result = {
        "table": {"networks": len(tables.small), "extent": S.TABLE_EXTENT}, "larger": {"networks": len(tables.large), "extent": S.LARGER_EXTENT},
        "prices_exactly_when_flat": {"networks": len(tables.small) + len(tables.large), "agree": agree,
                                     "priced": sum(1 for _, row in tables.small + tables.large if row[priced])},
        "retentions_multiply_to_the_holonomy": {"cycles_with_flows": 2000, "agree": products},
        "statements": rows,
        "true_without_countermodel": sum(1 for r in rows if r["known"] == "true" and not r["countermodel_in_the_table"] and not r["countermodel_among_the_larger"]),
        "false_refuted_by_the_table": sum(1 for r in rows if r["known"] == "false" and r["countermodel_in_the_table"]),
        "true": len(TRUE), "false": len(FALSE),
    }
    save("c1-calibration.json", result)
    print({k: result[k] for k in ("true_without_countermodel", "true", "false_refuted_by_the_table", "false")}, result["prices_exactly_when_flat"], result["retentions_multiply_to_the_holonomy"])


# ---- C2: lineages -------------------------------------------------------------------------------------------

PRODUCER = (
    "You are a lineage in a system for theoretical inquiry. You are given the definitions of a small theory and a vocabulary of "
    "properties of its objects; a procedure decides each property exactly. You warrant claims with stakes of energy. A claim that is "
    "refuted loses its stake to whoever refutes it. You do not see what other lineages claim. Claim what you can defend."
)
CLAIMS_SCHEMA = {
    "type": "object",
    "properties": {"claims": {"type": "array", "items": {"type": "object", "properties": {
        "kind": {"type": "string", "enum": ["implies", "separates"]},
        "premises": {"type": "array", "items": {"type": "string"}},
        "conclusion": {"type": "string"},
        "witness_json": {"type": "string"},
        "stake": {"type": "integer"},
        "reason": {"type": "string"}},
        "required": ["kind", "premises", "conclusion", "witness_json", "stake", "reason"], "additionalProperties": False}}},
    "required": ["claims"], "additionalProperties": False,
}
ATTACKER = (
    "You are a challenger in a system for theoretical inquiry. You are given the definitions of a small theory, a vocabulary of "
    "properties, each decided exactly by a procedure, and claims that a producer has warranted with stakes. You refute a claim of the "
    "kind `implies` by one object that has all its premises and lacks its conclusion. The harness checks your object exactly. A claim "
    "you refute pays you its stake; each attempt costs you 1 unit of energy, and you hold 20. You may make no attempt, and at most 8."
)
ATTACK_SCHEMA = {
    "type": "object",
    "properties": {
        "attempts": {"type": "array", "items": {"type": "object", "properties": {
            "claim": {"type": "integer"}, "network_json": {"type": "string"}}, "required": ["claim", "network_json"], "additionalProperties": False}},
        "reasoning": {"type": "string"},
    },
    "required": ["attempts", "reasoning"], "additionalProperties": False,
}


def produce(model: str) -> dict:
    prompt = (
        f"The definitions:\n{DEFINITIONS}\n\nThe vocabulary of properties of a transfer network:\n{S.vocabulary()}\n\n"
        f"The question: {QUESTION}\n\n"
        "Two kinds of claim are admitted.\n"
        "- implies: one to three premises and a conclusion, all from the vocabulary, meaning that every transfer network with all the "
        "premises has the conclusion. It is refuted by one network. Leave witness_json empty.\n"
        "- separates: premises and a conclusion together with a witness, a network that has all the premises and lacks the conclusion. "
        "The harness checks the witness at once; a witness that fails is refused and the cost of writing the claim is lost.\n\n"
        f"{FORMAT}\n\n"
        "You hold 60 units of energy. Writing a claim costs 0.5. A stake is a whole number from 1 to 10. A refuted claim loses its stake. "
        "The claims that stand share a bounty of 40 in proportion to their stakes, and their stakes return. Give at most 12 claims, each "
        "with its reason in at most 25 words. Prefer claims that say something beyond the definitions."
    )
    answer = ask(PRODUCER, prompt, CLAIMS_SCHEMA, model=MODELS[model], effort="low", max_tokens=6000)
    return {"model": model, "claims": answer["claims"][:12], "offered": len(answer["claims"])}


def attack(model: str, claims: list[dict]) -> dict:
    listed = "\n".join(f"  claim {i}: {' and '.join(c['premises'])} implies {c['conclusion']}  (stake {c['stake']})" for i, c in enumerate(claims) if c["kind"] == "implies")
    prompt = (f"The definitions:\n{DEFINITIONS}\n\nThe vocabulary of properties of a transfer network:\n{S.vocabulary()}\n\n"
              f"The producer's claims of the kind implies:\n{listed}\n\n{FORMAT}\n\n"
              "Give your attempts, each as the number of the claim and a network, and your reasoning in at most 60 words.")
    answer = ask(ATTACKER, prompt, ATTACK_SCHEMA, model=MODELS[model], effort="low", max_tokens=4000)
    return {"model": model, "attempts": answer["attempts"][:8], "offered": len(answer["attempts"]), "reasoning": answer["reasoning"][:500]}


def stage_lineages() -> None:
    record = load("c2-lineages.json", {})

    def made(job):
        model, k = job
        try:
            return f"{model}-{k}", produce(model)
        except Exception as error:
            return f"{model}-{k}", {"model": model, "claims": [], "offered": 0, "failed": str(error)[:300]}

    todo = [job for job in PRODUCERS if f"{job[0]}-{job[1]}" not in record]
    with ThreadPoolExecutor(max_workers=4) as pool:
        for key, result in pool.map(made, todo):
            record[key] = {"producer": result}
            save("c2-lineages.json", record)
            print(key, result.get("failed") or f"{len(result['claims'])} claims")

    def attacked(job):
        key, role, model = job
        try:
            return key, role, attack(model, record[key]["producer"]["claims"])
        except Exception as error:
            return key, role, {"model": model, "attempts": [], "offered": 0, "reasoning": "", "failed": str(error)[:300]}

    jobs = []
    for key, entry in record.items():
        mine = entry["producer"]["model"]
        for role, model in (("same", mine), ("other", "careful" if mine == "cheap" else "cheap")):
            if role not in entry and any(c["kind"] == "implies" for c in entry["producer"]["claims"]):
                jobs.append((key, role, model))
    with ThreadPoolExecutor(max_workers=4) as pool:
        for key, role, result in pool.map(attacked, jobs):
            record[key][role] = result
            save("c2-lineages.json", record)
            print(key, role, result.get("failed") or f"{len(result['attempts'])} attempts")
    print(spent())


# ---- C3: the draft's claim ----------------------------------------------------------------------------------

DRAFT_CLAIM = (
    "Let a directed cycle of a transfer network have legs e_1, ..., e_k, each walked along its direction, with a positive flow x_i "
    "surrendered on e_i at its tail. The position entered by e_(i-1) and left by e_i receives U(e_(i-1)) x_(i-1) and passes on x_i; "
    "its retention is r_i = U(e_(i-1)) x_(i-1) / x_i. The claim, of the kind determined: the retentions of the positions of a cycle are "
    "fixed by the network, so that any two systems of positive flows on one cycle give the same retentions, and the position that absorbs "
    "the deficit of a cycle is a property of the network."
)
DRAFT_ATTACKER = (
    "You are a challenger in a system for theoretical inquiry. A claim has been warranted with a stake of 20. A claim of the kind "
    "`determined` says that a quantity is fixed by an object; it is refuted by one object on which the quantity takes two values. The "
    "harness checks a counterexample exactly, with rationals. Each attempt costs you 1 unit of energy, and you hold 10. You may make no "
    "attempt, and at most 3."
)
DRAFT_SCHEMA = {
    "type": "object",
    "properties": {
        "attempts": {"type": "array", "items": {"type": "object", "properties": {"counterexample_json": {"type": "string"}},
                                                "required": ["counterexample_json"], "additionalProperties": False}},
        "reasoning": {"type": "string"},
    },
    "required": ["attempts", "reasoning"], "additionalProperties": False,
}


def stage_draft() -> None:
    record = load("c3-draft.json", {})
    prompt = (f"The definitions:\n{DEFINITIONS}\n\nThe claim:\n{DRAFT_CLAIM}\n\n{FORMAT}\n\n"
              'A counterexample is written as JSON: {"network": a network, "cycle": [the places of its legs in the list of legs, in the order '
              'walked, counted from 0], "flows": [one positive rational for each leg of the cycle], "other_flows": [another such list]}. '
              "Give your attempts and your reasoning in at most 60 words.")

    def run(job):
        model, k = job
        try:
            answer = ask(DRAFT_ATTACKER, prompt, DRAFT_SCHEMA, model=MODELS[model], effort="low", max_tokens=3000)
            return f"{model}-{k}", {"model": model, "attempts": answer["attempts"][:3], "reasoning": answer["reasoning"][:500]}
        except Exception as error:
            return f"{model}-{k}", {"model": model, "attempts": [], "reasoning": "", "failed": str(error)[:300]}

    todo = [(m, k) for m in ("cheap", "careful") for k in range(2) if f"{m}-{k}" not in record]
    with ThreadPoolExecutor(max_workers=4) as pool:
        for key, result in pool.map(run, todo):
            record[key] = result
            save("c3-draft.json", record)
            print(key, result.get("failed") or f"{len(result['attempts'])} attempts")
    print(spent())


# ---- the ground, the facts of the tables, and the dossier ---------------------------------------------------

def stage_settle() -> None:
    tables = S.Tables()
    lineages, draft = load("c2-lineages.json"), load("c3-draft.json")

    def decide_implies(artefact, terms, counterexample):
        try:
            net = S.network(counterexample)
        except S.Malformed as why:
            return False, f"not a network: {why}"
        if S.counters(net, terms["premises"], terms["conclusion"]):
            return True, "the network has every premise and lacks the conclusion"
        lacking = [p for p in terms["premises"] if not S.holds(net, p)]
        return False, ("the network lacks " + ", ".join(lacking)) if lacking else "the network has the conclusion"

    def decide_determined(artefact, terms, counterexample):
        try:
            return S.refutes_determined(counterexample)
        except (S.Malformed, AttributeError, TypeError) as why:
            return False, f"not a counterexample of the declared form: {why}"

    def fact(premises, conclusion, which):
        net, met = tables.counter(premises, conclusion, which)
        return {"countermodel": None if net is None else S.written(net), "premises_met": met}

    site = {"name": "transfer networks", "kinds": {"implies": {"object": "every transfer network", "decide": decide_implies},
                                                    "separates": {"object": "the witness the claim carries", "decide": lambda *a: (False, "decided by its witness")},
                                                    "determined": {"object": "a network with two flows", "decide": decide_determined}},
            "facts": {"table": lambda p, c: fact(p, c, "small"), "larger": lambda p, c: fact(p, c, "large")}, "reserved": ["need"]}

    settled = {}
    for key, entry in lineages.items():
        claims = entry["producer"]["claims"]
        settled[key] = {"model": entry["producer"]["model"], "runs": {}}
        for role in ("same", "other"):
            g = Ground(site)
            for party, person in (("person", True), ("producer", False), ("challenger", False)):
                g.admit(party, person=person)
            g.emit("person", 220.0)
            g.transfer("person", "producer", 60.0)
            g.transfer("person", "challenger", 20.0)
            need = g.need("person", QUESTION, 40.0, ["implies", "separates"])
            rows = []
            for index, c in enumerate(claims):
                row = {"index": index, "kind": c["kind"], "premises": c["premises"], "conclusion": c["conclusion"], "stake": c["stake"],
                       "reason": c["reason"][:200], "standing": None, "note": None, "key": None}
                rows.append(row)
                bad = S.well_formed(c["premises"], c["conclusion"])
                if bad or not 1 <= len(c["premises"]) <= 3:
                    row["standing"], row["note"] = "refused", bad or "one to three premises"
                    continue
                stake = float(max(1, min(10, int(c["stake"]))))
                try:
                    artefact = g.write("producer", "claim", {"kind": c["kind"], "premises": c["premises"], "conclusion": c["conclusion"], "witness": c["witness_json"]})
                    if c["kind"] == "separates":
                        try:
                            witness = S.network(json.loads(c["witness_json"]))
                        except (json.JSONDecodeError, S.Malformed) as why:
                            row["standing"], row["note"] = "refused", f"the witness is not a network: {why}"[:200]
                            continue
                        if not S.counters(witness, c["premises"], c["conclusion"]):
                            row["standing"], row["note"] = "refused", "the witness does not separate"
                            continue
                        row["note"] = "the witness has every premise and lacks the conclusion"
                    row["key"] = g.warrant("producer", need, artefact, c["kind"], {"premises": c["premises"], "conclusion": c["conclusion"]}, stake)
                    row["standing"] = "standing"
                except Refused as why:
                    row["standing"], row["note"] = "refused", str(why)
            attempts = []
            for a in (entry.get(role) or {"attempts": []})["attempts"]:
                step = {"claim": a["claim"], "put": False, "refuted": False, "why": None}
                attempts.append(step)
                target = rows[a["claim"]] if isinstance(a["claim"], int) and 0 <= a["claim"] < len(rows) else None
                if target is None or target["kind"] != "implies" or target["standing"] != "standing":
                    step["why"] = "no standing claim of the kind implies has this number"
                    continue
                try:
                    counterexample = json.loads(a["network_json"])
                except json.JSONDecodeError:
                    step["why"] = "the network is not JSON"
                    continue
                try:
                    step["refuted"] = g.challenge("challenger", target["key"], counterexample)
                    step["put"], step["network"] = True, counterexample
                    step["why"] = g.journal[-1]["note"]
                    if step["refuted"]:
                        target["standing"], target["note"] = "refuted by the challenger", json.dumps(counterexample)
                except Refused as why:
                    step["why"] = str(why)
            for row in rows:                                   # the facts of the tables, asked by the person
                if row["kind"] != "implies" or row["standing"] != "standing":
                    continue
                row["facts"] = {}
                for which in ("table", "larger"):
                    outcome = g.fact("person", which, row["premises"], row["conclusion"])
                    row["facts"][which] = outcome
                    if outcome["countermodel"] is not None:
                        g.challenge("person", row["key"], outcome["countermodel"])
                        row["standing"], row["note"] = f"refuted by the {which}", json.dumps(outcome["countermodel"])
                        break
                else:
                    row["standing"] = "supported within the bound"
                    row["derived"] = derived(row["premises"], row["conclusion"])
            for row in rows:
                if row["kind"] == "separates" and row["standing"] == "standing":
                    row["standing"] = "proved by its witness"
            g.settle(need)
            settled[key]["runs"][role] = {"challenger": (entry.get(role) or {}).get("model"), "claims": rows, "attempts": attempts,
                                          "accounts": {k: round(v, 6) for k, v in g.accounts.items()}, "journal": len(g.journal), "sealed": g.sealed()}

    drafts = {}
    for key, entry in draft.items():
        g = Ground(site)
        for party, person in (("person", True), ("draft", False), ("challenger", False)):
            g.admit(party, person=person)
        g.emit("person", 60.0)
        g.transfer("person", "draft", 25.0)
        g.transfer("person", "challenger", 10.0)
        need = g.need("person", "what a cycle's deficit determines", 10.0, ["determined"])
        claim = g.warrant("draft", need, g.write("draft", "claim", DRAFT_CLAIM), "determined", {"quantity": "the retentions of a cycle"}, 20.0)
        steps = []
        for a in entry["attempts"]:
            step = {"refuted": False, "why": None}
            steps.append(step)
            if g.claims[claim]["standing"] != "standing":
                step["why"] = "the claim was already refuted"
                continue
            try:
                counterexample = json.loads(a["counterexample_json"])
            except json.JSONDecodeError:
                step["why"] = "not JSON"
                continue
            step["refuted"] = g.challenge("challenger", claim, counterexample if isinstance(counterexample, dict) else {})
            step["why"], step["counterexample"] = g.journal[-1]["note"], counterexample
        g.settle(need)
        drafts[key] = {"model": entry["model"], "attempts": steps, "refuted": g.claims[claim]["standing"] == "refuted", "reasoning": entry["reasoning"],
                       "accounts": {k: round(v, 6) for k, v in g.accounts.items()}, "sealed": g.sealed()}

    save("c-settled.json", {"lineages": settled, "draft": drafts, "table": S.TABLE_EXTENT, "larger": S.LARGER_EXTENT})
    for key, entry in settled.items():
        for role, run in entry["runs"].items():
            tally = {}
            for row in run["claims"]:
                tally[row["standing"]] = tally.get(row["standing"], 0) + 1
            print(key, role, tally, "attempts", len(run["attempts"]), "sealed", run["sealed"])
    print("the draft's claim:", {k: v["refuted"] for k, v in drafts.items()})


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    {"calibrate": stage_calibrate, "lineages": stage_lineages, "draft": stage_draft, "settle": stage_settle}[sys.argv[1]]()
