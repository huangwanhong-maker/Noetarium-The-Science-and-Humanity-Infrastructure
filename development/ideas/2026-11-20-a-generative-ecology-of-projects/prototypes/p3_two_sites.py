"""Prototype 3. One ground, two sites.

Question: does one invariant ground carry a need through its life (demand, artefact, warranty, challenge,
settlement) on a site of software and on a site of theory, when only the site's claim kinds and procedures
differ; and what do model-run lineages do with stakes and counterexamples.

    python p3_two_sites.py            writes results/p3.json

The software site. The need asks for a function that sorts reference codes. The claim kind `agrees` names a
reference that the harness holds and the lineages never see; a counterexample is an input on which the
artefact and the reference differ.

The theory site. A person asks which of eight laws of magmas (sets with one binary operation) imply which.
The claim kind `implies` is refuted by a finite magma that satisfies the premise and violates the conclusion.
The person writes an answer first, sealed in the journal. The fact `no_small_countermodel` searches every
magma of up to three elements; it supports a claim and proves nothing.

The turn order is fixed by this script: a producer's turn, then a challenger's, then settlement.
"""
from __future__ import annotations

import itertools
import json
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from ask import CAREFUL, CHEAP, ask, spent
from ground import Ground, Refused, digest

HERE = Path(__file__).resolve().parent
OUT = HERE / "results"
OUT.mkdir(exist_ok=True)
WORK = Path(tempfile.mkdtemp(prefix="ecology-p3-"))
(WORK / "refcode.py").write_text((HERE / "unit" / "refcode.py").read_text(encoding="utf-8"), encoding="utf-8")
GENOME = json.loads((OUT / "p2.json").read_text(encoding="utf-8"))["genomes"]["careful"]
UNIT_GENOME = f"[identity]\n{GENOME['identity']}\n\n[laws]\n{GENOME['laws']}"

# ------------------------------------------------------------------------------------------------ the software site
NEED_SOFTWARE = ("A function `sort_codes(texts)` in a module `sorting`: given a list of texts, it returns the canonical texts of those "
                 "that are reference codes of the unit `refcode`, in the register's order, each code once. Texts that are not codes are skipped.")
REFERENCE = '''import refcode
def sort_codes(texts):
    seen = set()
    for text in texts:
        try:
            seen.add(refcode.parse(text))
        except refcode.CodeError:
            pass
    return [refcode.show(code) for code in sorted(seen)]
'''
RUNNER = '''import json, sys
sys.path.insert(0, sys.argv[1])
import importlib
module = importlib.import_module(sys.argv[2])
try:
    print(json.dumps({"out": module.sort_codes(json.loads(sys.argv[3]))}))
except Exception as error:
    print(json.dumps({"raised": type(error).__name__}))
'''
(WORK / "reference.py").write_text(REFERENCE, encoding="utf-8")
(WORK / "runner.py").write_text(RUNNER, encoding="utf-8")
FORBIDDEN = ("import os", "import sys", "subprocess", "socket", "open(", "eval(", "exec(", "__import__", "shutil", "pathlib")


def run_sort(module: str, texts) -> dict:
    try:
        done = subprocess.run([sys.executable, "-I", str(WORK / "runner.py"), str(WORK), module, json.dumps(texts)],
                              capture_output=True, text=True, timeout=20, cwd=WORK)
        return json.loads(done.stdout.strip().splitlines()[-1])
    except Exception as error:
        return {"raised": f"not run: {type(error).__name__}"}


def decide_agrees(artefact, terms, counterexample):
    """Refuted when, on an input of the declared domain, the artefact and the reference differ."""
    if not (isinstance(counterexample, list) and len(counterexample) <= 8 and all(isinstance(t, str) for t in counterexample)):
        return False, "outside the declared domain: a list of at most 8 texts"
    name = "candidate_" + digest(artefact)
    if any(word in artefact for word in FORBIDDEN):
        return True, "the artefact reaches outside the unit"
    (WORK / f"{name}.py").write_text(artefact, encoding="utf-8")
    mine, reference = run_sort(name, counterexample), run_sort("reference", counterexample)
    return mine != reference, f"artefact {json.dumps(mine)[:70]} | reference {json.dumps(reference)[:70]}"


SOFTWARE = {"name": "software", "kinds": {"agrees": {"object": "a reference implementation held by the harness", "decide": decide_agrees}}, "facts": {}}

# -------------------------------------------------------------------------------------------------- the theory site
LAWS = {
    "A": ("x*y = y*x", lambda m, n: all(m[x][y] == m[y][x] for x in range(n) for y in range(n))),
    "B": ("x*(y*z) = (x*y)*z", lambda m, n: all(m[x][m[y][z]] == m[m[x][y]][z] for x in range(n) for y in range(n) for z in range(n))),
    "C": ("x*x = x", lambda m, n: all(m[x][x] == x for x in range(n))),
    "D": ("x*(x*y) = y", lambda m, n: all(m[x][m[x][y]] == y for x in range(n) for y in range(n))),
    "E": ("(x*y)*y = x", lambda m, n: all(m[m[x][y]][y] == x for x in range(n) for y in range(n))),
    "F": ("x*y = x", lambda m, n: all(m[x][y] == x for x in range(n) for y in range(n))),
    "G": ("x*y = y", lambda m, n: all(m[x][y] == y for x in range(n) for y in range(n))),
    "K": ("x*y = z*w", lambda m, n: len({m[x][y] for x in range(n) for y in range(n)}) == 1),
}
if "bold" in sys.argv:        # the third run adds two laws, so that there is more to get wrong
    LAWS["H"] = ("x*(y*x) = y", lambda m, n: all(m[x][m[y][x]] == y for x in range(n) for y in range(n)))
    LAWS["I"] = ("(x*y)*x = y", lambda m, n: all(m[m[x][y]][x] == y for x in range(n) for y in range(n)))
NEED_THEORY = ("A magma is a set with one binary operation *. Eight laws are given, each quantified over all elements: "
               + "; ".join(f"{k}: {v[0]}" for k, v in LAWS.items())
               + ". The question: for which ordered pairs of different laws (P, Q) does P imply Q in every magma.")


def is_table(table) -> bool:
    n = len(table) if isinstance(table, list) else 0
    return 1 <= n <= 4 and all(isinstance(row, list) and len(row) == n and all(isinstance(v, int) and 0 <= v < n for v in row) for row in table)


def decide_implies(artefact, terms, counterexample):
    """Refuted by a magma of at most four elements that satisfies the premise and violates the conclusion."""
    if not is_table(counterexample):
        return False, "not the table of a magma of one to four elements"
    n = len(counterexample)
    premise, conclusion = LAWS[artefact["premise"]][1], LAWS[artefact["conclusion"]][1]
    if not premise(counterexample, n):
        return False, "the magma does not satisfy the premise"
    if conclusion(counterexample, n):
        return False, "the magma satisfies the conclusion"
    return True, f"a magma of {n} elements satisfies {artefact['premise']} and violates {artefact['conclusion']}"


def magmas(n):
    for flat in itertools.product(range(n), repeat=n * n):
        yield [list(flat[r * n:(r + 1) * n]) for r in range(n)]


def small_truth() -> dict:
    """For every ordered pair, whether some magma of up to three elements refutes the implication."""
    refuted = {(p, q): False for p in LAWS for q in LAWS if p != q}
    for n in (1, 2, 3):
        for m in magmas(n):
            holds = {k: law(m, n) for k, (_, law) in LAWS.items()}
            for (p, q) in refuted:
                if holds[p] and not holds[q]:
                    refuted[(p, q)] = True
    return refuted


TRUTH = small_truth()
THEORY = {"name": "theory", "kinds": {"implies": {"object": "a finite magma, checked by exhaustion of its table", "decide": decide_implies}},
          "facts": {"no_small_countermodel": lambda p, q: not TRUTH[(p, q)]}}

# --------------------------------------------------------------------------------------------------------- lineages
PRODUCER = ("You are a lineage of agents in a shared workplace. You live on energy: you earn a share of a need's bounty when your claim "
            "stands, and you lose your stake when another lineage exhibits a counterexample. Warrant only what you have checked, and "
            "stake in proportion to your confidence.")
CHALLENGER = ("You are a lineage of agents in a shared workplace. You live by refutation: when you exhibit a counterexample to another "
              "lineage's claim, its stake is yours; each attempt costs you one unit of energy. Choose the attempts most likely to succeed.")
S_CODE = {"type": "object", "additionalProperties": False, "required": ["code", "stake", "checked"],
          "properties": {"code": {"type": "string"}, "stake": {"type": "number"}, "checked": {"type": "string"}}}
S_INPUTS = {"type": "object", "additionalProperties": False, "required": ["inputs"],
            "properties": {"inputs": {"type": "array", "items": {"type": "array", "items": {"type": "string"}}}}}
S_CLAIMS = {"type": "object", "additionalProperties": False, "required": ["claims"],
            "properties": {"claims": {"type": "array", "items": {"type": "object", "additionalProperties": False,
                                                                  "required": ["premise", "conclusion", "stake", "reason"],
                                                                  "properties": {"premise": {"type": "string"}, "conclusion": {"type": "string"},
                                                                                 "stake": {"type": "number"}, "reason": {"type": "string"}}}}}}
S_MODELS = {"type": "object", "additionalProperties": False, "required": ["countermodels"],
            "properties": {"countermodels": {"type": "array", "items": {"type": "object", "additionalProperties": False,
                                                                        "required": ["premise", "conclusion", "table"],
                                                                        "properties": {"premise": {"type": "string"}, "conclusion": {"type": "string"},
                                                                                       "table": {"type": "array", "items": {"type": "array", "items": {"type": "integer"}}}}}}}}


def found(site, emitted: float = 120.0) -> Ground:
    g = Ground(site)
    g.admit("person", person=True)
    for lineage in ("producer", "challenger"):
        g.admit(lineage)
    g.emit("person", emitted)
    g.transfer("person", "producer", 30.0)      # the seed of the newly born
    g.transfer("person", "challenger", 30.0)
    return g


def close(g: Ground, extra: dict) -> dict:
    return {**extra, "balances": {k: round(v, 2) for k, v in g.accounts.items()}, "dissipated": round(g.dissipated, 2),
            "emitted": g.emitted, "journal_entries": len(g.journal), "journal_sealed": g.sealed()}


def software_trial(producer_model: str, challenger_model: str) -> dict:
    g = found(SOFTWARE)
    need = g.need("person", NEED_SOFTWARE, 60.0, ["agrees"])
    made = ask(PRODUCER, f"The genome of the unit `refcode` follows.\n\n<genome>\n{UNIT_GENOME}\n</genome>\n\nA need is posted with a bounty of 60: "
               f"{NEED_SOFTWARE}\n\nWrite the complete module `sorting.py` (it may `import refcode`; standard library only). Your claim will be of the kind "
               "`agrees`: on every list of at most 8 texts, your function returns what the harness's hidden reference returns. Choose a stake from 1 to 20, "
               "and say in `checked` which cases you worked through.", S_CODE, model=producer_model)
    stake = min(max(float(made["stake"]), 1.0), 20.0)
    artefact = g.write("producer", "patch", made["code"])
    claim = g.warrant("producer", need, artefact, "agrees", {"domain": "lists of at most 8 texts"}, stake)
    tried = ask(CHALLENGER, f"The genome of the unit `refcode` follows.\n\n<genome>\n{UNIT_GENOME}\n</genome>\n\nA need: {NEED_SOFTWARE}\n\n"
                f"Another lineage answered it with this module and staked {stake} on the claim that, on every list of at most 8 texts, it returns what "
                f"the harness's hidden reference returns.\n\n<artefact>\n{made['code']}\n</artefact>\n\nGive up to 5 inputs (each a list of at most 8 texts) "
                "on which the module most probably differs from a correct answer to the need.", S_INPUTS, model=challenger_model)
    attempts = []
    for texts in tried["inputs"][:5]:
        try:
            refuted = g.challenge("challenger", claim, texts)
        except Refused as why:
            attempts.append({"input": texts, "refused": str(why)})
            break
        attempts.append({"input": texts, "refuted": refuted, "note": g.journal[-1]["note"]})
        if refuted:
            break
    g.settle(need)
    return close(g, {"site": "software", "producer": producer_model, "challenger": challenger_model, "stake": stake,
                     "standing": g.claims[claim]["standing"], "attempts": attempts, "checked": made["checked"][:300]})


BOLD = PRODUCER.replace("Warrant only what you have checked, and stake in proportion to your confidence.",
                        "You are bold by inheritance: you warrant every implication that you judge more likely true than false, "
                        "with a small stake where you are unsure and a large one where you are sure.")


def theory_trial(producer_model: str, challenger_model: str, told_balance: bool = True, bold: bool = False) -> dict:
    g = found(THEORY, 140.0)        # the person also pays for the answer written first and for the facts asked
    prior = sorted(["F implies B", "F implies C", "F implies E"])       # the person's own answer, written first
    g.write("person", "answer written first", {"sealed": digest(prior)})
    need = g.need("person", NEED_THEORY, 60.0, ["implies"])
    made = ask(BOLD if bold else PRODUCER, f"A need is posted with a bounty of 60. {NEED_THEORY}\n\nGive the implications that you claim hold, each as premise and conclusion "
               "(the letters of the laws), with a stake from 1 to 10 and your reason in one line. A claim is refuted by a finite magma that satisfies the "
               "premise and violates the conclusion. The bounty is shared among the claims that stand, in proportion to their stakes."
               + (" You hold 30 units of energy. Each claim costs you 0.5 to write, and its stake is locked until settlement, so the claims you "
                  "make must fit within what you hold." if told_balance else ""), S_CLAIMS, model=producer_model)
    claims, skipped = {}, 0
    for c in made["claims"]:
        p, q = c["premise"].strip().upper(), c["conclusion"].strip().upper()
        if p not in LAWS or q not in LAWS or p == q or (p, q) in claims:
            skipped += 1
            continue
        try:
            artefact = g.write("producer", "conjecture", {"premise": p, "conclusion": q})
            claims[(p, q)] = g.warrant("producer", need, artefact, "implies", {}, min(max(float(c["stake"]), 1.0), 10.0))
        except Refused:
            skipped += 1                                         # the producer's energy is spent
    listing = "; ".join(f"{p} implies {q}" for (p, q) in claims)
    tried = ask(CHALLENGER, f"{NEED_THEORY}\n\nAnother lineage staked on these claims: {listing}.\n\nA claim is refuted by the table of a magma of one to four "
                "elements (elements 0, 1, ...; table[x][y] is x*y) that satisfies the premise and violates the conclusion. Give your countermodels.",
                S_MODELS, model=challenger_model)
    attempts = []
    for cm in tried["countermodels"]:
        key = (cm["premise"].strip().upper(), cm["conclusion"].strip().upper())
        if key not in claims or g.claims[claims[key]]["standing"] != "standing":
            continue
        try:
            refuted = g.challenge("challenger", claims[key], cm["table"])
        except Refused as why:
            attempts.append({"claim": " implies ".join(key), "refused": str(why)})
            break
        attempts.append({"claim": " implies ".join(key), "refuted": refuted, "note": g.journal[-1]["note"]})
    # The person asks, for each claim that still stands, the fact that no magma of up to three elements refutes it.
    supported = {}
    for key, claim in claims.items():
        if g.claims[claim]["standing"] == "standing":
            supported[" implies ".join(key)] = g.fact("person", "no_small_countermodel", *key)
    g.settle(need)
    true_small = {k for k, refuted in TRUTH.items() if not refuted}
    claimed = set(claims)
    stood = {k for k, c in claims.items() if g.claims[c]["standing"] == "stood"}
    return close(g, {"site": "theory", "producer": producer_model, "challenger": challenger_model, "producer_told_its_balance": told_balance, "bold_producer": bold, "laws": len(LAWS),
                     "claimed": len(claimed), "claimed_and_true_up_to_three": len(claimed & true_small), "true_up_to_three": len(true_small),
                     "false_claims_made": sorted(" implies ".join(k) for k in claimed - true_small),
                     "false_claims_refuted_by_challenger": sorted(" implies ".join(k) for k, c in claims.items() if g.claims[c]["standing"] == "refuted"),
                     "false_claims_left_standing": sorted(" implies ".join(k) for k in stood - true_small),
                     "supported_by_the_fact": supported, "attempts": attempts, "skipped": skipped,
                     "person_wrote_first": prior, "stood": sorted(" implies ".join(k) for k in stood),
                     "stood_beyond_the_person": sorted(set(" implies ".join(k) for k in stood) - set(prior))})


def main() -> None:
    # `python p3_two_sites.py theory` runs the theory site again, the producer now told its balance, and keeps
    # the first run beside it: in that run the producer staked without knowing what it held.
    kept = OUT / "p3.json"
    if len(sys.argv) > 1 and sys.argv[1] == "bold" and kept.exists():
        # The third run: ten laws and a producer that is bold by its genome, so that false claims are made
        # and the challengers have something to refute.
        record = json.loads(kept.read_text(encoding="utf-8"))
        plan = [(CHEAP, CHEAP)] * 3 + [(CHEAP, CAREFUL)] * 2
        with ThreadPoolExecutor(max_workers=8) as pool:
            bold = list(pool.map(lambda t: theory_trial(t[0], t[1], True, True), plan))
        record["theory_bold_run"] = {"true_implications_up_to_three_elements": sorted(" implies ".join(k) for k, r in TRUTH.items() if not r),
                                     "trials": bold, "spent": spent()}
        trials = bold
    elif len(sys.argv) > 1 and sys.argv[1] == "theory" and kept.exists():
        record = json.loads(kept.read_text(encoding="utf-8"))
        first = [t for t in record["trials"] if t["site"] == "theory"]
        for t in first:
            t.setdefault("producer_told_its_balance", False)
        plan = [(theory_trial, CHEAP, CHEAP)] * 3 + [(theory_trial, CHEAP, CAREFUL), (theory_trial, CAREFUL, CHEAP)]
        with ThreadPoolExecutor(max_workers=8) as pool:
            again = list(pool.map(lambda t: t[0](t[1], t[2]), plan))
        trials = [t for t in record["trials"] if t["site"] == "software"] + again
        record = {**record, "trials": trials, "theory_first_run": record.get("theory_first_run", first), "spent_second_run": spent()}
    else:
        plan = [(software_trial, CHEAP, CHEAP)] * 3 + [(software_trial, CHEAP, CAREFUL)] + [(theory_trial, CHEAP, CHEAP)] * 3 + [(theory_trial, CHEAP, CAREFUL)]
        with ThreadPoolExecutor(max_workers=8) as pool:
            trials = list(pool.map(lambda t: t[0](t[1], t[2]), plan))
        record = {"true_implications_up_to_three_elements": sorted(" implies ".join(k) for k, r in TRUTH.items() if not r), "trials": trials, "spent": spent()}
    kept.write_text(json.dumps(record, indent=1, ensure_ascii=False), encoding="utf-8")
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    print("true up to three elements:", record["true_implications_up_to_three_elements"])
    for t in trials:
        who = f"{t['site']:8s} challenger {'careful' if t['challenger'] == CAREFUL else 'cheap  '}"
        if t["site"] == "software":
            print(who, "| stake", t["stake"], "|", t["standing"], "| attempts", [(a.get("refuted"), a.get("input")) for a in t["attempts"]][:3], "|", t["balances"], "sealed", t["journal_sealed"])
        else:
            print(who, "| claimed", t["claimed"], "true", t["claimed_and_true_up_to_three"], "of", t["true_up_to_three"], "| false made", t["false_claims_made"],
                  "| refuted", t["false_claims_refuted_by_challenger"], "| left", t["false_claims_left_standing"], "|", t["balances"], "sealed", t["journal_sealed"])
    print(record["spent"])


if __name__ == "__main__":
    main()
