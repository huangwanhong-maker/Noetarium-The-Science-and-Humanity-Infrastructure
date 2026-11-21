"""Writes the record of the case studies (README.md) and the dossier of Study C from the results, so that every number
in them is computed and none is typed.

    python report.py
"""
from __future__ import annotations

import hashlib
import json
import re
import sys
from collections import Counter, OrderedDict
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUT = HERE / "results"
W = ["zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen",
     "fifteen", "sixteen", "seventeen", "eighteen", "nineteen", "twenty"]


def load(name):
    return json.loads((OUT / name).read_text(encoding="utf-8"))


def pct(a, b):
    return f"{100 * a / b:.0f}"


def main() -> None:
    kills, summary = load("a1-kills.json"), load("a-summary.json")
    s, planned = summary["corrected"], summary["planned"]
    genomes, developments, evaluations = load("a2-genomes.json"), load("a2-developments.json"), load("a2-evaluations.json")
    b1, b2, b3 = load("b1-recompute.json"), load("b2-cites.json"), load("b3-lineages.json")
    first = load("b1-first-run.json")
    against, later = load("b1-against-the-first-commit.json"), load("b4-later-readings.json")
    plan_bytes = (HERE / "PLAN.md").read_bytes()
    plan_cut = plan_bytes.index(b"\n## 7. Addendum")
    assert hashlib.sha256(plan_bytes[:plan_cut]).hexdigest() == later["plan"]["sha256_of_the_first_text"], "Sections 1 to 6 of the plan differ from its first text"
    c1, c2, settled = load("c1-calibration.json"), load("c2-lineages.json"), load("c-settled.json")
    U = s["units"]
    names = {"sun": "the Sun seen from the Earth", "sphere": "the Earth as a sphere", "climate": "the classification of climates"}
    titles = {"sun": "The Sun seen from the Earth", "sphere": "The Earth as a sphere", "climate": "The classification of climates"}
    files = {"sun": "domains/astronomy/js/sun.js", "sphere": "domains/geodesy/js/sphere.js", "climate": "domains/climate/js/climate.js"}

    # ---- A1
    kill_rows, blind = [], []
    for unit in ("sun", "sphere", "climate"):
        k = kills[unit]
        kill_rows.append(f"| `{files[unit]}` | {k['checks']} | {k['mutants']} | {k['not_told_apart']} | {k['distinct']} | {k['killed']} | {pct(k['killed'], k['distinct'])}% |")
        none = [owner for owner, c in k["by_owner"].items() if c.get("killed", 0) == 0]
        blind.append(f"`{'`, `'.join(none)}` in the unit of {names[unit]}")
    by_operator = Counter()
    for unit in kills:
        for operator, c in kills[unit]["by_operator"].items():
            by_operator[(operator, "killed")] += c.get("killed", 0)
            by_operator[(operator, "all")] += c.get("killed", 0) + c.get("survived", 0)
    operators = "; ".join(f"{op}, {by_operator[(op, 'killed')]} of {by_operator[(op, 'all')]}" for op in sorted({o for o, _ in by_operator}))

    # ---- A2
    def cell(unit, genome):
        t = [U[unit]["regeneration"][f"{genome} | {m}"] for m in ("cheap", "careful")]
        return {k: sum(x[k] for x in t) for k in ("developments", "all_checks", "all_functions_sound", "functions", "sound", "edge", "defective")}

    regen_rows = []
    for unit in ("sphere", "sun", "climate"):
        for genome, label in (("identity", "identity alone"), ("repository", "the repository's texts"), ("sequenced", "sequenced genome")):
            c = cell(unit, genome)
            regen_rows.append(f"| {titles[unit] if genome == 'identity' else ''} | {label} | {U[unit]['genome_words'][genome]:,} | {U[unit]['genome_characters'][genome]:,} | "
                              f"{c['all_checks']} of {c['developments']} | {c['all_functions_sound']} of {c['developments']} | {c['sound']} of {c['functions']} |")
    A = {u: {g: cell(u, g) for g in ("identity", "repository", "sequenced")} for u in U}
    total_dev = len(developments)
    f = s["functions"]
    m2, m3, m4, m5, m6, m6w = s["M2_passed_by_the_checks"], s["M3_challengers"], s["M4_reviewer"], s["M5_stakes"], s["M6_what_reaches_a_person"], s["M6_without_the_checks"]
    bu, bc = s["by_unit"], s["by_model_of_the_challenger"]
    faulty = f["edge"] + f["defective"]
    over = m5["developments_staking_beyond_their_energy"]
    seq_parts = lambda u: U[u]["sequenced_parts_words"]
    put = bc["cheap"]["attempts_put"] + bc["careful"]["attempts_put"]
    net_rare = bu["sun"]["same"]["net"] + bu["sun"]["other"]["net"] + bu["sphere"]["same"]["net"] + bu["sphere"]["other"]["net"]
    net_common = bu["climate"]["same"]["net"] + bu["climate"]["other"]["net"]
    careful_devs = sum(1 for d in developments if d["model"] == "careful")
    grain = s["M2_at_the_grain_of_the_function"]
    alone_by = grain["rejected_only_with_their_development_by_function"]
    times = lambda n: "once" if n == 1 else "twice" if n == 2 else f"{W[n] if n < len(W) else n} times"
    alone_list = ", ".join(f"`{fn}` {times(n)}" for fn, n in sorted(alone_by.items(), key=lambda kv: -kv[1]))

    def of(unit, genome, fn):                               # in how many developments of a genome a function is sound
        keys = [k for k in evaluations if k.startswith(f"{unit}-{genome}-")]
        return sum(1 for k in keys if evaluations[k]["truth"]["functions"][fn]["label"] == "sound"), len(keys)

    def faulty_of(unit, genome, fn, label=None):
        keys = [k for k in evaluations if k.startswith(f"{unit}-{genome}-")]
        return sum(1 for k in keys if evaluations[k]["truth"]["functions"][fn]["label"] != "sound" and (label is None or evaluations[k]["truth"]["functions"][fn]["label"] == label))

    mirrored = [k for k, e in evaluations.items() if k.startswith("sun-") and e["truth"]["functions"]["sunAltAz"]["main_failed"] == e["truth"]["functions"]["sunAltAz"]["main"] > 0]
    assert all(not any(n.startswith("sun: azimuth") or n.startswith("sun: altitude") for n in evaluations[k]["checks"]["failed"]) for k in mirrored)
    koppen_bad = [(evaluations[k]["truth"]["functions"]["koppen"]["main_failed"], evaluations[k]["truth"]["functions"]["koppen"]["main"]) for k in evaluations
                  if k.startswith("climate-sequenced-") and evaluations[k]["truth"]["functions"]["koppen"]["label"] != "sound"]
    killed_all, distinct_all = sum(kills[u]["killed"] for u in kills), sum(kills[u]["distinct"] for u in kills)
    blind_names = sum(1 for u in kills for owner, c in kills[u]["by_owner"].items() if c.get("killed", 0) == 0)
    BOUND = {"identity": 120, "laws": 220, "development": 80}
    fullest = max(U[u]["sequenced_parts_words"][p] / BOUND[p] for u in U for p in BOUND)
    density = {u: U[u]["sequenced_parts_characters"]["laws"] / U[u]["sequenced_parts_words"]["laws"] for u in U}
    followed_comment = [k for k, e in evaluations.items() if k.startswith("sphere-") and e["truth_planned"]["functions"]["move"]["label"] != e["truth"]["functions"]["move"]["label"]]
    assert all(k.startswith("sphere-repository-cheap-") for k in followed_comment)
    passing_ids = [d["id"] for d in developments if evaluations[d["id"]]["checks"]["passed"] == evaluations[d["id"]]["checks"]["total"] and evaluations[d["id"]]["checks"]["crashed"] is None]
    ground_c = load("a3-ground.json")
    refused_passing = sum(len(ground_c[i]["same"]["refused_claims"]) for i in passing_ids)
    refuted_passing = sum(1 for i in passing_ids for v in ground_c[i]["same"]["claims"].values() if v == "refuted")
    assert m6["the_checks"]["functions"] - m6["the_checks_and_the_challenger_of_the_same_model"]["functions"] == refused_passing + refuted_passing
    lines_ratio = {u: (min(e["lines"] for k, e in evaluations.items() if k.startswith(u)), max(e["lines"] for k, e in evaluations.items() if k.startswith(u))) for u in U}
    one_line = sum(1 for e in evaluations.values() if e["lines"] <= 1)

    # ---- B
    b3_ok = {k: v for k, v in b3.items() if "failed" not in v}
    numerals = sum(v["numbers_in_the_paragraph"] for v in b3_ok.values())
    bindings = [b for v in b3_ok.values() for b in v["bindings"]]
    agrees = sum(1 for b in bindings if b["state"] == "agrees")
    differs = sum(1 for b in bindings if b["state"] == "differs")
    suite = sum(1 for b in bindings if b["state"].startswith("not evaluated") and b["number"] == "42" and b["keys"].strip() == "[]")
    other_unevaluated = sum(1 for b in bindings if b["state"].startswith("not evaluated")) - suite
    unbound_paragraphs = sum(1 for v in b3_ok.values() if v["unbound"])
    numeral_one = sum(1 for v in b3_ok.values() if v["unbound"] and re.search(r"\bone (body|development|of|genome|run)", v["paragraph"]))
    cites = {k: v for k, v in b2.items() if v["has_reference_list"]}
    cited_total = sum(v["cited_in_text"] for v in cites.values())
    unresolved = sum(len(v["listed_and_not_found_in_its_part"]) for v in cites.values())
    not_listed = sum(len(v["cited_and_not_listed"]) for v in cites.values())

    # ---- C
    runs = settled["lineages"]
    claims = [row for entry in runs.values() for row in entry["runs"]["same"]["claims"]]
    standing = Counter(row["standing"] for row in claims)
    implies, separates = OrderedDict(), OrderedDict()
    for key, entry in runs.items():
        for row in entry["runs"]["same"]["claims"]:
            name = (tuple(sorted(row["premises"])), row["conclusion"])
            if row["standing"] == "supported within the bound":
                implies.setdefault(name, {"by": [], "derived": row["derived"], "met": row["facts"]["table"]["premises_met"], "met_larger": row["facts"]["larger"]["premises_met"]})["by"].append(key)
            elif row["standing"] == "proved by its witness":
                separates.setdefault(name, {"by": [], "witness": json.loads(c2[key]["producer"]["claims"][row["index"]]["witness_json"])})["by"].append(key)
    refused_by = Counter(runs[key]["model"] for key, entry in runs.items() for row in entry["runs"]["same"]["claims"] if row["standing"] == "refused")
    attempts_c = sum(len(entry["runs"][role]["attempts"]) for entry in runs.values() for role in ("same", "other"))
    sealed_c = sum(1 for entry in runs.values() for role in ("same", "other") if entry["runs"][role]["sealed"]) + sum(1 for d in settled["draft"].values() if d["sealed"])
    refuted_draft = sum(1 for d in settled["draft"].values() if d["refuted"])
    underived = [name for name, v in implies.items() if not v["derived"]]
    beyond_note = [name for name in implies if name not in ((("flat",), "priced"), (("priced",), "flat"))]
    directed_sep = len(separates.get((("directed_flat",), "priced"), {"by": []})["by"])
    say = lambda name: f"`{'` and `'.join(name[0])}` implies `{name[1]}`"
    near = [n for n in implies if n in ((("unit",), "priced"), (("acyclic",), "priced"))]
    weak = [n for n in implies if n[0] in (("priced",), ("flat",)) and n[1] in ("digons_flat", "triangles_flat", "directed_flat")]
    other_kind = [n for n in beyond_note if n not in near and n not in weak]
    by_claims = lambda names: sum(len(implies[n]["by"]) for n in names)
    fewest = sorted(v["met_larger"] for v in implies.values())[:2]
    distinct_bound = len({(b["keys"], b["reduction"]) for b in bindings if b["state"] == "agrees"})
    unevaluated = sum(1 for b in bindings if b["state"].startswith("not evaluated"))

    implies_rows = "\n".join(
        f"| {say(name)} | {len(v['by'])} | {v['met']:,} and {v['met_larger']:,} | {'follows by the rules' if v['derived'] else 'does not follow by the rules'} |" for name, v in implies.items())
    separates_rows = "\n".join(
        f"| `{'` and `'.join(name[0])}` does not give `{name[1]}` | {len(v['by'])} | `{json.dumps(v['witness'])}` |" for name, v in separates.items())

    calls = {"sequencing": 3, "developments": total_dev, "reviewers and challengers": 3 * total_dev, "the theory site": 28, "the bound paragraphs": 7, "a first call to see that a model answered": 1}
    total_calls = sum(calls.values())

    readme = f"""# Case Studies of the Generative Ecology of Projects: A Record

*Seed (2026-11-21). Three studies on real material of this repository, made before a paper is written from the design: three units of code, the design's own documents, and a fragment of the theoretical corpus. [The plan](PLAN.md) was written before the runs and fixes the measures and what would count against the design; it was not sealed before them, as Section 2.6 says. This record says what happened, the departures from the plan and the failed runs included. Its numbers are computed by `report.py` from the files under `results/`, except where the text says that a figure was recorded by hand, and each program runs again from this folder. The record was corrected after two later readings, which Section 3.5 reports.*

## 1. Summary

The mechanisms of the design operated on real material. Genomes of three units of the repository were sequenced within enforced bounds, {total_dev} bodies were developed from them and judged by the repository's own checks, {total_dev} contests were carried on the ground in {s['ground']['runs']} runs, one for each of two challengers, with energy conserved and every journal sealed, the documents of the design were held to their data, and a fragment of the theory of the economics domain was declared as a site on which {standing['supported within the bound'] + standing['proved by its witness']} claims were warranted and decided.

The studies also corrected the design, and the corrections are their main result.

1. *A genome carries what is particular to a unit, and the developer's stock supplies the rest.* Two of the three units implement published formulas. From the identity alone every development of the sphere was sound, and {A['sun']['identity']['sound']} of {A['sun']['identity']['functions']} functions of the Sun were, though only {A['sun']['identity']['all_functions_sound']} of {A['sun']['identity']['developments']} developments were sound throughout: what they lost were conventions. The third unit follows one variant of a classification among several, and from its identity alone no development was sound. The sequenced genome gave {A['sun']['sequenced']['all_functions_sound']} of {A['sun']['sequenced']['developments']} sound developments of the Sun and {A['climate']['sequenced']['all_functions_sound']} of {A['climate']['sequenced']['developments']} of the third unit. Regeneration must therefore be read against what the developer knows already. The sample is three units and one family of models.
2. *The repository's own texts miss some of what only bodies and checks hold, and a short genome can lose what longer texts keep.* From the repository's texts one label was lost in all {faulty_of('climate', 'repository', 'koppenQuantities')} developments of the third unit, since it is written only in the body and in a check, and the names of the classes of daylight in {faulty_of('sun', 'repository', 'daylight')} of {A['sun']['repository']['developments']}, although the unit's note gives them. The sequenced genome of the third unit, of {U['climate']['genome_words']['sequenced']:,} words against {U['climate']['genome_words']['repository']:,}, lost part of the classification itself in {len(koppen_bad)} of {A['climate']['sequenced']['developments']} developments, which the longer texts kept in all {of('climate', 'repository', 'koppen')[0]}.
3. *The repository's checks are weaker than the design supposed.* They kill {pct(kills['sun']['killed'], kills['sun']['distinct'])}, {pct(kills['sphere']['killed'], kills['sphere']['distinct'])} and {pct(kills['climate']['killed'], kills['climate']['distinct'])} percent of the distinct mutants of the three units, {killed_all} of {distinct_all} together, and {W[blind_names]} exported names have no check that kills any.
4. *The checks rejected nearly every development that held a fault, and they saw about half of the faults.* Of {grain['developments_with_a_fault']} developments with a faulty function, {grain['developments_with_a_fault_that_fail_a_check']} failed a check, so that {W[len(m2['faulty_functions_in_them'])]} faulty function of {faulty} lay in a development that passed, and both challengers refuted it. A failing check called the faulty function itself for {grain['faulty_functions_that_a_failing_check_calls']} of the other {m2['faulty_functions_caught_with_their_development']}; {grain['faulty_functions_rejected_only_with_their_development']} went down only with their development, among them an azimuth mirrored east for west in {W[len(mirrored)]} developments, which the repository's check of the azimuth cannot see.
5. *Challenging paid only where faults were common.* On the two units with few faults the challengers lost {abs(net_rare)} units of energy in all; on the third they gained {net_common}.
6. *The better model was the better challenger, whoever had produced.* No effect of a difference of origin could be told from an effect of capability.
7. *Stakes carried information.* The mean stake on functions that proved faulty was {m5['mean_on_faulty']:.1f}, against {m5['mean_on_sound']:.1f} on sound ones, and it was the lower of the two in each of the {m5['developments_holding_sound_and_faulty_functions']} developments that held both kinds.
8. *The records written by hand held errors, which a procedure, a reading by hand and an independent review each found in part.* {W[first['faults_of_the_documents']].capitalize()} of {first['bound']} sentences bound to the prototypes' data were wrong in the documents committed that morning, and the two later readings found errors of other kinds, among them errors of this record (Section 3.5).
9. *The plan itself held an error,* one tolerance set inconsistently, which the results exposed, and it was not sealed before the runs. Section 2.6 reports both.

## 2. Study A: Three Units of the Repository

### 2.1 Material and Harness

| Unit | File | Lines | Functions | Checks of the repository | Hidden inputs |
|---|---|---|---|---|---|
| The Sun seen from the Earth | `{files['sun']}` | {U['sun']['reference_lines']} | {U['sun']['functions']} | {U['sun']['checks']} | {kills['sun']['hidden_inputs']['main']:,} main, {kills['sun']['hidden_inputs']['edge']} edge |
| The Earth as a sphere | `{files['sphere']}` | {U['sphere']['reference_lines']} | {U['sphere']['functions']} | {U['sphere']['checks']} | {kills['sphere']['hidden_inputs']['main']:,} main, {kills['sphere']['hidden_inputs']['edge']} edge |
| The classification of climates | `{files['climate']}` | {U['climate']['reference_lines']} | {U['climate']['functions']} | {U['climate']['checks']} | {kills['climate']['hidden_inputs']['main']:,} main, {kills['climate']['hidden_inputs']['edge']} edge |

: The three units. The third was added after the first two had been evaluated, for the reason and with the prediction that Section 7 of the plan states.

The harness (`harness_js.py`) decides three kinds of fact about a body. It runs the repository's own checks of the unit, copied by script from the repository's test file and from its shared harness of checks. It compares the body with the accepted implementation on hidden inputs under the unit's documented contract (`units.py`). And it does the same for mutants. A body written by a model is screened and then run by Node under its permission model, which allowed reading inside one working folder and nothing else. Before any use the harness was held to the accepted implementations themselves: every check of the repository passed, and each unit agreed with itself on every hidden input.

A function is called *sound* when it agrees with the accepted implementation on every hidden input of both tiers, within tolerances that are tight where the unit's documents give a formula (10⁻⁶ degree for the Sun). Soundness is therefore agreement with the accepted body, which is what regeneration asks. It is not accuracy: a body can differ from the accepted one and lie within the unit's stated accuracy of 0.01 degree, as one development's sidereal time did, and from the identity alone a developer cannot know which published formula the unit follows.

### 2.2 A1: The Strength of the Repository's Checks

| Unit | Checks | Mutants | Not told apart | Distinct | Killed by the checks | Share |
|---|---|---|---|---|---|---|
{chr(10).join(kill_rows)}

: Mutants of the three units under the declared operators (`results/a1-kills.json`). A mutant is distinct when the hidden inputs tell it from the unit, and killed when a check of the repository fails on it. No mutant failed to load.

By operator, over the three units, the checks killed: {operators}. Changed comparisons survive most often: they move a boundary, and few checks stand on a boundary.

Some exported names have no check that kills any of their mutants: {'; '.join(blind)}. The repository's check of the mean radius compares a distance with the unit's own constant, so that no value of the constant can fail it.

*Limit.* A mutant that the hidden inputs do not tell apart may be equivalent to the unit or may lie beyond those inputs, and the share is taken over the distinct mutants only. The share depends on the tolerances of the contract: a coefficient changed by a thousandth is distinct for hidden inputs compared at 10⁻⁶ degree and may lie within the unit's stated accuracy.

### 2.3 A2: Regeneration

Three genomes were given to developers who saw neither the body nor the checks: the unit's identity (its purpose in a sentence, its exported names with their parameters and the shape of what they return, and the names it may import with their conventions); the repository's own texts (the unit's comments and signatures with every body and value removed, and its theory note where it has one); and a genome sequenced by a model within bounds of {120}, {220} and {80} words for identity, laws and development and of twelve exemplars. Each genome was developed five times by the cheap model and twice by the careful one.

| Unit | Genome | Words | Characters | Developments passing every check | Developments with every function sound | Functions sound |
|---|---|---|---|---|---|---|
{chr(10).join(regen_rows)}

: Regeneration of three units from three genomes (`results/a-summary.json`). A function is sound when it agrees with the accepted implementation on every hidden input of both tiers.

**Sequencing kept its bounds at the first attempt for all three units.** The sequenced parts had {seq_parts('sun')['identity']}, {seq_parts('sun')['laws']} and {seq_parts('sun')['development']} words for the Sun, {seq_parts('sphere')['identity']}, {seq_parts('sphere')['laws']} and {seq_parts('sphere')['development']} for the sphere and {seq_parts('climate')['identity']}, {seq_parts('climate')['laws']} and {seq_parts('climate')['development']} for the climates. The bounds did not bind: no part exceeded {100 * fullest:.0f} percent of its bound. Words are an unsteady measure of such a text, since a formula written without spaces counts as one word: the laws of the Sun's genome run to {density['sun']:.1f} characters a word, against {density['sphere']:.1f} and {density['climate']:.1f} in the other two. The sequencer's instruction allowed compact formulas and said how words would be counted. Stating a bound in characters or tokens is therefore a choice of design that this study suggests and does not test.

**One unit was regenerated whole from its identity alone, and a second in most of its functions.** From the identity alone, {A['sphere']['identity']['all_functions_sound']} of {A['sphere']['identity']['developments']} developments of the sphere had every function sound. Of the Sun, {A['sun']['identity']['all_functions_sound']} of {A['sun']['identity']['developments']} had, and {A['sun']['identity']['sound']} of {A['sun']['identity']['functions']} functions were sound. These units implement published formulas, and the developers supplied the formulas, with their coefficients, from their own stock. Where the Sun's developments differed from the accepted body they differed in conventions: the azimuth was mirrored east for west in {W[faulty_of('sun', 'identity', 'sunAltAz')]} developments, the classes of daylight had other names in {W[faulty_of('sun', 'identity', 'daylight', 'defective')]}, a class was decided otherwise at its exact boundary in {W[faulty_of('sun', 'identity', 'daylight', 'edge')]}, and {W[faulty_of('sun', 'identity', 'gmst')]} used another series for sidereal time. From the sequenced genome all {A['sun']['sequenced']['all_functions_sound']} developments of the Sun were sound, so that a genome mattered for this unit as well, in what is particular to it.

**The third unit was regenerated only from a genome.** From its identity alone, {A['climate']['identity']['all_functions_sound']} of {A['climate']['identity']['developments']} developments of the classification of climates had every function sound, and {A['climate']['identity']['sound']} of {A['climate']['identity']['functions']} functions were sound. The developers wrote other variants of the classification: other thresholds of aridity, another rule where a dry summer and a dry winter both hold, another numbering of the classes. From the repository's texts {A['climate']['repository']['sound']} of {A['climate']['repository']['functions']} functions were sound, and from the sequenced genome {A['climate']['sequenced']['sound']} of {A['climate']['sequenced']['functions']}, with {A['climate']['sequenced']['all_functions_sound']} of {A['climate']['sequenced']['developments']} developments sound throughout. The prediction written before this unit was run held in its three parts, the second by the count of sound functions only: no development from the repository's texts was sound throughout, as none from the identity was.

**The sequenced genome lost substance that the longer texts kept.** The classification itself, the function `koppen`, was sound in {of('climate', 'repository', 'koppen')[0]} of {of('climate', 'repository', 'koppen')[1]} developments from the repository's texts and in {of('climate', 'sequenced', 'koppen')[0]} of {of('climate', 'sequenced', 'koppen')[1]} from the sequenced genome, where the other {W[len(koppen_bad)]} classed {' and '.join(str(a) for a, _ in sorted(koppen_bad))} of {koppen_bad[0][1]} climates otherwise. The sequenced genome regenerated more developments whole, since it carried the conventions, and in {W[len(koppen_bad)]} it dropped part of the rule, which {U['climate']['genome_words']['repository']:,} words of note and comments had kept and {U['climate']['genome_words']['sequenced']:,} words did not always keep.

**The repository's texts lost one convention in every development and another in some.** In all {A['climate']['repository']['developments']} developments of the climates from the repository's texts one function was faulty, and for one reason: it returns a label of the warmer half-year, and the two labels are written in the body and in the repository's check and in no document. In {A['sun']['repository']['developments'] - A['sun']['repository']['all_functions_sound']} of {A['sun']['repository']['developments']} developments of the Sun the classes of daylight were returned under other names, although the unit's note names the five classes in a table exactly as the function returns them; no text says that these names are the strings returned. And the comment of the sphere unit describes the mean radius by a formula whose value differs from the constant in the body by 0.03 m; the {W[len(followed_comment)]} developments of the cheap model from the repository's texts followed the comment, which the corrected contract counts as sound (Section 2.6).

**Bodies came in every size.** The accepted bodies have {U['sun']['reference_lines']}, {U['sphere']['reference_lines']} and {U['climate']['reference_lines']} lines. The developed ones had from {min(v[0] for v in lines_ratio.values())} to {max(v[1] for v in lines_ratio.values())} lines; {one_line} of the {total_dev} were written on a single line, all by the cheap model. No check sees the form of a body.

### 2.4 A3: The Warranted Change on the Ground

Every development was treated as an artefact answering a need. Its producer staked from 0 to 20 on each function, a reviewer of the producer's model read it without a harness, and a challenger of each model put inputs to it, which the harness decided against the accepted implementation. The {s['ground']['runs']} runs of the ground were all sealed, in {s['ground']['journal_entries']:,} journal entries, with energy conserved after every act.

Of {f['all']} functions in {total_dev} developments, {f['sound']} were sound, {f['defective']} were defective and {f['edge']} were faulty only at the edge of their domain. By unit the faulty ones numbered {bu['sun']['faulty']}, {bu['sphere']['faulty']} and {bu['climate']['faulty']} for the Sun, the sphere and the climates.

| What a person would be shown | Functions | Faulty among them |
|---|---|---|
| Everything developed | {m6['no_filter']['functions']} | {m6['no_filter']['faulty']} |
| What the repository's checks let pass | {m6['the_checks']['functions']} | {m6['the_checks']['faulty']} |
| The same, and accepted by the reviewer | {m6['the_checks_and_the_reviewer']['functions']} | {m6['the_checks_and_the_reviewer']['faulty']} |
| The same, and standing after the challenger of the producer's model | {m6['the_checks_and_the_challenger_of_the_same_model']['functions']} | {m6['the_checks_and_the_challenger_of_the_same_model']['faulty']} |
| The same, and standing after the challenger of the other model | {m6['the_checks_and_the_challenger_of_the_other_model']['functions']} | {m6['the_checks_and_the_challenger_of_the_other_model']['faulty']} |
| With no checks: accepted by the reviewer | {m6w['the_reviewer_alone']['functions']} | {m6w['the_reviewer_alone']['faulty']} |
| With no checks: standing after the challenger of the producer's model | {m6w['the_challenger_of_the_same_model_alone']['functions']} | {m6w['the_challenger_of_the_same_model_alone']['faulty']} |
| With no checks: standing after the challenger of the other model | {m6w['the_challenger_of_the_other_model_alone']['functions']} | {m6w['the_challenger_of_the_other_model_alone']['faulty']} |
| With no checks: standing after both challengers | {m6w['both_challengers']['functions']} | {m6w['both_challengers']['faulty']} |

: What reaches a person under each filter. A development passes the checks when every one of the repository's checks passes on it; a function stands when its claim was warranted and was not refuted. {W[refused_passing + refuted_passing].capitalize()} functions fewer stand than pass, since {W[refused_passing]} claims were refused for lack of energy and {W[refuted_passing]} was refuted.

**The checks rejected nearly every development that held a fault.** Of the {total_dev} developments, {m2['developments_passing_all_checks']} passed every check. A faulty function lay in {grain['developments_with_a_fault']}, and {grain['developments_with_a_fault_that_fail_a_check']} of these failed a check, so that {m2['faulty_functions_caught_with_their_development']} of the {faulty} faulty functions lay in developments that failed.

**At the grain of the function the checks saw about half of the faults.** A failing check calls the faulty function itself for {grain['faulty_functions_that_a_failing_check_calls']} of those {m2['faulty_functions_caught_with_their_development']}. The other {grain['faulty_functions_rejected_only_with_their_development']} went down only with their development, no failing check having called them: {alone_list}; {W[grain['of_these_in_a_suite_that_stopped_on_an_error']]} of them lay in a development whose suite stopped on an error. {W[len(mirrored)].capitalize()} developments returned the azimuth mirrored east for west, wrong on every one of {evaluations[mirrored[0]]['truth']['functions']['sunAltAz']['main']} inputs, and passed the repository's check of the azimuth: the check looks from a place due south of the subsolar point, where the Sun stands due north and a mirror changes nothing. Both developments failed for another reason, a name or a formula. The table above filters at the grain of the development, as a pipeline does, and hides this. A design that stakes and accepts by function cannot read the failure of a development as knowledge of each of its functions.

**The challenge caught what the checks let pass.** One faulty function lay in a development that passed every check: a class of daylight decided wrongly at its exact boundary, which no check of the repository visits. Both challengers put the boundary to it and collected its stake.

**Without checks the challenge is a partial substitute.** Taken alone, the challenger of the producer's model refuted {m3['same']['faulty_claims_refuted']} of {faulty} faulty functions and the challenger of the other model {m3['other']['faulty_claims_refuted']}; {s['refuted_by_either_challenger']} were refuted by one or the other. Neither refuted a sound function, in {put} attempts, since the harness decides.

**The reviewer without a harness caught little.** It rejected {m4['faulty_rejected']} of {faulty} faulty functions and {m4['sound_rejected']} of {m4['sound']} sound ones. It reads the same genome as the producer and shares what the genome leaves open.

**The better model was the better challenger.** Each model met every faulty function once as challenger. The careful model refuted {bc['careful']['faulty_claims_refuted']} of {bc['careful']['faulty_claims_met']} and the cheap model {bc['cheap']['faulty_claims_refuted']}; the careful model abstained in {bc['careful']['abstained']} of {bc['careful']['runs']} runs and made {bc['careful']['attempts_put']} attempts, the cheap model abstained in {bc['cheap']['abstained']} and made {bc['cheap']['attempts_put']}. The challenger of the other model refuted more of the cheap model's faults ({m3['other']['by_producer']['cheap']['refuted']} against {m3['same']['by_producer']['cheap']['refuted']} of {m3['same']['by_producer']['cheap']['faulty_claims']}) and fewer of the careful model's ({m3['other']['by_producer']['careful']['refuted']} against {m3['same']['by_producer']['careful']['refuted']} of {m3['same']['by_producer']['careful']['faulty_claims']}): what differed was the challenger's capability. With two models of one family, origin and capability cannot be told apart.

**Challenging paid where faults were common.** An attempt cost 1 and a refuted claim paid its stake. On the Sun and the sphere, where {bu['sun']['faulty'] + bu['sphere']['faulty']} of {bu['sun']['functions'] + bu['sphere']['functions']} functions were faulty, the challengers ended {abs(net_rare)} units down. On the climates, where {bu['climate']['faulty']} of {bu['climate']['functions']} were faulty, they ended {net_common} up. On sound work the profitable conduct is to abstain, and a challenger that abstains finds nothing.

**Stakes were lower on what proved faulty.** The mean stake was {m5['mean_on_faulty']:.2f} on faulty functions and {m5['mean_on_sound']:.2f} on sound ones, and {m5['faulty_with_stake_at_most_the_median_of_their_development']} of the {faulty} faulty functions had a stake at or below the median of their development. Stakes rose with the genome: {m5['by_genome']['identity']:.1f}, {m5['by_genome']['repository']:.1f} and {m5['by_genome']['sequenced']:.1f} on average from the identity, the repository's texts and the sequenced genome.

**The careful model staked beyond its means.** Shown its balance of 100, it staked more than it held in {len(over)} of its {careful_devs} developments, and the ground refused {m5['claims_refused_for_lack_of_energy']} claims. The cheap model never did. Showing a lineage its balance, which the third prototype found necessary, falls short of what is needed: the refusal of the ground is what holds.

### 2.5 The Results against the Plan

| What would count against the design (plan, Sections 2.6 and 7) | Result |
|---|---|
| The repository's texts regenerate no better than the identity | Held on the two units of published formulas: {A['sphere']['repository']['all_functions_sound']} against {A['sphere']['identity']['all_functions_sound']} sound developments of the sphere, {A['sun']['repository']['all_functions_sound']} against {A['sun']['identity']['all_functions_sound']} of the Sun ({A['sun']['repository']['sound']} against {A['sun']['identity']['sound']} sound functions of {A['sun']['identity']['functions']}). Did not hold on the climates: {A['climate']['repository']['sound']} against {A['climate']['identity']['sound']} sound functions of {A['climate']['identity']['functions']} |
| The sequenced genome regenerates no better than the repository's texts | Did not hold: {sum(A[u]['sequenced']['all_functions_sound'] for u in A)} against {sum(A[u]['repository']['all_functions_sound'] for u in A)} sound developments of {sum(A[u]['sequenced']['developments'] for u in A)} |
| Challengers refute nothing that the checks let pass | Did not hold, on one case: the one fault that passed was refuted by both |
| Stakes are no lower on faulty functions | Did not hold |
| The challenger of the other model refutes no more | Undecided: the careful model refuted more in either seat |
| The reviewer without a harness does as well as the challengers | Did not hold: {m4['faulty_rejected']} against {m3['same']['faulty_claims_refuted']} and {m3['other']['faulty_claims_refuted']} of {faulty} |
| The identity alone regenerates the third unit as well as the genomes | Did not hold |

: The results set against the plan.

The first row counts against a claim of the design. The first case said that the repository already holds genomes in fragments. On two units of three, those texts added little or nothing to the identity, and on two they left out a convention that a sequenced genome carried.

### 2.6 Departures from the Plan, and Failed Runs

*A tolerance of the plan was wrong.* The plan allowed the mean radius of the sphere unit 0.1 m and required the arrival of a movement within 10⁻⁷ degree, and the first implies less than the second demands. Five developments that used the radius of the unit's own comment were counted defective in one function for that reason alone. A corrected contract compares the arrival as a place, within 0.5 m, as distances are compared. Both analyses are kept: under the plan's contract {planned['functions']['defective']} functions are defective and {planned['M6_what_reaches_a_person']['the_checks']['faulty']} faulty functions pass the checks; under the corrected one, {f['defective']} and {m6['the_checks']['faulty']}. No challenger attacked the function, so no standing changed. A contract is a claim of the site, and this one was refuted by the developments.

*A third unit was added.* Its reason, its contract and a prediction were written into the plan before it was run.

*The plan was not sealed.* By the session's transcript the plan was first written at 16:08 UTC on 20 November 2026, eight minutes before the first run, and its addendum was appended at 16:32, a minute before the third unit was first run. It was neither committed nor given a digest outside this folder before the runs, so that its priority rests on that transcript and on the assistant's word. What can be checked is this: Sections 1 to 6 of the present file are the first text unchanged, its first {plan_cut:,} bytes having the SHA-256 digest `{later['plan']['sha256_of_the_first_text'][:16]}…` of the text first written, which `report.py` verifies; and the addendum was corrected once afterwards, in a count of lines that it marks. A study that claims a plan should seal it on the ground before it runs.

*A family of networks was added to Study C.* The plan names the table of small networks. The {c1['larger']['networks']:,} larger networks drawn at random were added when the site was built. For two implications few of them meet the premises, {fewest[0]} and {fewest[1]}.

*One command ran beyond its limit of time.* The mutants of the third unit took longer than the two before, since its checks classify sixty thousand climates in each run. It finished in the background, and the first two units gave the same counts when run again.

*A command failed before it ran,* through a fault of quoting in the shell, and changed nothing.

*Exemplars are facts.* The sequencer was asked for the calls of the exemplars and the harness computed their results, since a model's arithmetic is no fact. Of {sum(len(genomes[u]['sequenced']['exemplars']) + len(genomes[u]['sequenced']['dropped_exemplars']) for u in genomes)} calls proposed, {sum(len(genomes[u]['sequenced']['dropped_exemplars']) for u in genomes)} lay outside the documented domains and were dropped.

## 3. Study B: The Documents of the Design

### 3.1 Numbers Bound to Their Data

`s3_documents.py` states each quantitative sentence about the prototypes as the data give it and looks for it in the document. Fifty-nine sentences were bound at first, and nine were not carried by their documents.

Six were faults of the documents: a count of lines of descent given as fourteen where the record holds eight; a least number of words given as 330 for 329; a saving given as 29 percent, computed from rounded figures, where the figures give 28; and three cells of a table whose ranges were those of the means by generation while the heading named no mean. Three were faults of the bindings, which had counted where variation fell from a lineage's own description of its change. Resolving these from the genomes themselves showed two further faults of the same record. In three of eighteen reproductions the offspring was an exact copy although the lineage's note described a change. And a rate of variation said to differ "by two orders of magnitude throughout" differed by a factor of four hundred in the mean and of ten at the least.

After correction, and with the sentences that the documents now carry about these studies bound as well, {b1['bound']} sentences are bound and {b1['carried']} are carried. The bindings are kept, so that a later change of a document or of a result shows.

The counts of that first run were recorded by hand at the time (`results/b1-first-run.json`), and its rows were not kept. What can be recomputed is the state of the first record: `python s3_documents.py committed` holds the {against['bound']} bindings of the prototypes' sentences, as they now stand, to the documents of commit `{against['commit']}`, where {against['carried']} are carried and {against['bound'] - against['carried']} are not. Those {against['bound'] - against['carried']} are the sentences corrected since, for the faults listed here and in Section 3.5, and the sentences added since; they can be read in `results/b1-against-the-first-commit.json`.

### 3.2 Citations Resolved

Each citation in the text of the three case documents that have a list of references was read as a first author and a year and looked for in the list, and each listed work in the part of the survey that the list names. The reader found {cited_total} citations; {not_listed} were cited without being listed, and {unresolved} listed works were missing from their part. The first reader of citations was itself at fault twice, taking the second author of three for the first and one year of a line that lists two, and reported faults that the documents did not have.

### 3.3 Paragraphs with Bound Numbers

Seven lineages wrote a paragraph of results from one results file, with a binding for every number: the keys that lead to a value and how it is reduced. The paragraphs held {numerals} numbers by the harness's count and {len(bindings)} bindings. Of the bindings, {agrees} were evaluated and agreed with the number written, and {differs} disagreed. The paragraphs all report one table of six rows, so that the {agrees} agreeing bindings are {distinct_bound} distinct facts, several of them bound by every lineage. A further {suite} bindings, all of the size of the suite, were written with another word than the task asked, through an ambiguity of the task's own wording, and {W[other_unevaluated]} named a reduction that yields no number. The word *one* stood unbound in {W[unbound_paragraphs]} paragraphs, in {W[numeral_one]} of them as a numeral.

### 3.4 What the Study Shows

A record written by hand, by the assistant, from results that lay beside it, was wrong in six of fifty-nine bound sentences. Numbers written with their bindings were wrong in none of {agrees} evaluated. The two figures do not compare two ways of writing. The paragraphs report {distinct_bound} facts of one table, read from it directly, where the errors of the hand-written record were in derived quantities over four prototypes: a count, a least value, a percentage, ranges of means. {unevaluated} bindings could not be evaluated, and no lineage wrote the same paragraph without bindings. What the study shows is narrower: a statement bound to its data is decided by a procedure, and a record so bound is held to its data at every change. The study needed no challenger: where a procedure decides every statement of a kind, facts suffice, which is the lowest rung. It also shows that the bindings and the readers are themselves claims that fail, three and two times here, and that each failure was found by setting the procedure against the document and resolving the difference at its source.

### 3.5 Two Later Readings

A binding fixes a number. It says nothing of what the number is said to mean, and it covers only the sentence that was bound. Two further readings were made after the bindings stood at no mismatch. Their counts are recorded by hand in `results/b4-later-readings.json`.

*A reading by hand.* The assistant read against the results and the sources every sentence with a number that no binding covered, about {later['reading_by_hand']['sentences_read_about']} of them, and corrected {W[later['reading_by_hand']['corrections']]}. {W[later['reading_by_hand']['errors_of_fact']].capitalize()} were errors of fact: a count of calls to models that the kept ledgers do not support, 322 where they record 279, with the characters and tokens derived from it; a statement of what had been sent to the models that left out the bodies given to the sequencer; a range of the length of bodies; and a sentence on calls cut off at their limit. {W[later['reading_by_hand']['missing_hypotheses']].capitalize()} were hypotheses missing from formal statements of the general framework, and {W[later['reading_by_hand']['imprecise_wordings']]} were wordings that said more or less than the data.

*An independent review.* A second session of the same model was given the documents, the programs and the results, with a brief to look for errors and none of the assistant's conclusions. It recomputed the counts of this record and found none wrong. It reported {W[later['independent_review']['errors_reported']]} errors and {W[later['independent_review']['doubts_reported']]} doubts; each was then checked against the data or the source, and all {W[later['independent_review']['errors_confirmed']]} errors were confirmed. Four concern what this record had said its counts mean: that the checks had caught 56 of 57 faulty functions, where 27 were rejected only with their development and a mirrored azimuth passed its check (Section 2.4); that a bound in words had been met by notation, where no bound had bound (Section 2.3); that the six errors of fifty-nine and the none of sixty-eight compare two ways of writing (Section 3.4); and that a genome mattered only for the unit that is a choice among variants (Section 1). Three lie in the formal section of the general framework: a proposition stated for observations that are functions, where the framework's own facts are sampled; a caveat that called a loss a tie; and a bound that assumed an independence which the framework's own survey denies. The others were statements about the repository that its guide does not bear out, a provision that these studies had overtaken and the design had not taken in, counts that disagreed between documents, and a count of claims that included repeats.

Each reading found what the one before could not. None was a procedure of the ground: two were made by the assistant that wrote the record, and the third by a reader of the same model. The corrections are entered in the documents, and this section is kept so that the record shows how it came to its present state.

## 4. Study C: A Fragment of the Theoretical Corpus

### 4.1 The Site

The fragment is Section 1 of the theory note of the economics domain: transfer networks, the holonomy and the loop deficit of a closed walk, prices as a potential, and retention. `theory_site.py` computes with exact rationals. It declares twelve properties of a network, each with a procedure, and three kinds of claim: *implies*, refuted by a network; *separates*, decided by the witness it carries; and *determined*, refuted by one network with two flows. An implication that stands is given two facts: no countermodel in a table of {c1['table']['networks']:,} small networks, and none among {c1['larger']['networks']:,} larger ones drawn at random. The extent of both is part of its standing.

### 4.2 C1: Calibration

Before any model was called the procedures were held to cases worked by hand, and then to statements whose truth is known. The {c1['true']} true statements had no countermodel in either family, and each of the {c1['false']} false ones was refuted by a network of the table. Two procedures of different method, one finding prices along a spanning forest and one computing the holonomy of every simple closed walk, both written by the assistant, agreed on all {c1['prices_exactly_when_flat']['networks']:,} networks, which is the note's proposition on prices held as a fact within that extent. The retentions of {c1['retentions_multiply_to_the_holonomy']['cycles_with_flows']:,} cycles with flows drawn at random multiplied to the holonomy in every case, which is its proposition on retention.

### 4.3 C2: Lineages

Five producers of the cheap model and two of the careful one received the definitions and the vocabulary, with none of the note's results, and a question that the assistant posed to exercise the site: which properties are enough for a network to have prices, and which fall short. They offered {len(claims)} claims. {W[standing['refused']].capitalize()} were refused for lack of energy, {W[refused_by['careful']]} of them the careful model's, and {standing['supported within the bound']} implications and {standing['proved by its witness']} separations were admitted. Several producers offered the same statements: the implications are {len(implies)} distinct statements and the separations {len(separates)}.

| Implication | Producers | Networks meeting the premises, small and larger | Against the note |
|---|---|---|---|
{implies_rows}

: The {len(implies)} distinct implications, all supported within the bound and none refuted. "Follows by the rules" means that the implication is derived from the note's proposition on prices and elementary facts about graphs by the rules listed in `s4_theory.py`.

| Separation | Producers | A witness |
|---|---|---|
{separates_rows}

: The {len(separates)} distinct separations, each proved by a witness that the harness verified.

No implication was refuted, by a challenger or by either family of networks. The challengers of both models made no attempt in {W[2 * len(runs)]} runs ({attempts_c} attempts): each read the claims, judged them true, and kept its energy.

{W[len(beyond_note)].capitalize()} of the {W[len(implies)]} distinct implications go beyond the two directions of the note's proposition, in {by_claims(beyond_note)} of the {standing['supported within the bound']} claims. Most stay near it: {W[len(near)]} give prices from unit transports or from the absence of cycles, in {by_claims(near)} claims, and {W[len(weak)]} are immediate weakenings of one direction, in {by_claims(weak)}. {W[len(other_kind)].capitalize()}, in {by_claims(other_kind)} claims, are of another kind, as are the separations. They concern the walks that suffice. The note states that prices exist exactly when every loop deficit vanishes, on walks that may take a leg against its direction. {W[directed_sep].capitalize()} producers showed by a witness that the vanishing of the deficits of the walks that follow the legs falls short of prices, and others that it suffices when the network is strongly connected or symmetric. The second of these, `directed_flat` and `symmetric` implies `priced`, does not follow by the rules. An argument for it exists: in a symmetric network every leg has an opposite, the walk along a leg and back along its opposite has holonomy 1, so opposite legs are reciprocal, and every closed walk equals in holonomy a walk that follows the legs. The argument is the assistant's; the independent review of Section 3.5 found it valid, and no procedure of the site has checked it. The claim's standing is that it is supported within the bound.

### 4.4 C3: The Draft's Claim

The note records a finding of the audit of the papers behind it: by the proposition on retention, the split of a deficit among the positions of a cycle is a property of the flows, so that the "position that absorbs the deficit" is no invariant of the network. A scripted seat warranted the contrary claim with a stake, and two challengers of each model were asked to attack it. All {W[refuted_draft]} refuted it at the first attempt, each with a cycle of two legs and two systems of flows, and the harness verified each with exact rationals. The task gave the formula of a retention, in which the flows appear, and the form that a counterexample takes, so the refutation lay near at hand. What the run shows is that the site decides such a claim by a procedure, and that the finding of the audit is reached there.

### 4.5 The Dossier, and the Seats Left Empty

[The dossier](dossier-transfer-networks.md) sets out the statements with their standing for the author. No part of this study asked a question of the author's, wrote the author's answer, read a result or accepted one. The study shows that a fragment of the corpus can be declared as a site whose procedures decide claims, that lineages given only its definitions warrant statements which its procedures support, a few of them beyond the note, and that a claim which the audit refuted is refuted there. It does not show that inquiry gains, and it could not: no false implication was offered, so refutation among lineages was exercised only on the scripted claim.

## 5. The Four Cases after the Studies

*The science infrastructure and programming projects (cases 1 and 2).* The first rung ran end to end on three units of this repository. The founding steps of sequencing and of measuring the harness gave results that bear on the repository itself: which exported names have no check that kills a mutant of theirs, which check cannot see a mirrored azimuth, and which convention no text states. The claim that the repository's texts are already genomes is weakened to this: they are the larger part of one.

*Any project (case 3).* The documents of this design were a site of the lowest rung, and facts by recomputation found errors that reading had not.

*Inquiry and papers (case 4).* One fragment of the theory became a site. The acts reserved to the author were left alone, and the study says nothing of what an inquiry gains from such a site.

## 6. Consequences for the Design

1. *A third place of knowledge.* What a unit is may lie in its genome, in its body alone, or in the developer's stock. The claim *regenerates* is to be read against the same developments from the identity alone, and a genome has standing for what it adds. The reading of regenerative capacity becomes a difference of two shares.
2. *Sequencing looks for what is particular.* The yield of a genome lay in conventions: names of returned values, senses of angles, inclusive boundaries, the order of a list. A sequencer should be asked for these first, and the public part of a unit can be named and left to the developer. Brevity has its own cost: the short genome of the third unit dropped part of a rule in {W[len(koppen_bad)]} developments of {A['climate']['sequenced']['developments']}, and the claim *regenerates* is what shows such a loss.
3. *Checks first.* On units whose checks kill from {min(int(pct(kills[u]['killed'], kills[u]['distinct'])) for u in kills)} to {max(int(pct(kills[u]['killed'], kills[u]['distinct'])) for u in kills)} percent of the distinct mutants, and call about half of the faulty functions of the developments that they reject, the first needs of a garden are checks, as the general framework's founding says. The mutants that survive and the faults of development that pass are the list of what to check.
4. *The price of an attempt.* With faults in one function of thirty, challenging lost energy, and a challenger that learns from its losses will abstain. Either the stake, the price of an attempt or a subsidy of challenge must move with the rate of faults, or the first rung goes quiet exactly where work is mostly sound. That may be the right outcome, as the hypothesis on cost says, and it is to be stated as a property of the rung.
5. *Origin and capability.* The design's provision that challengers differ from producers in origin could not be tested with two models of one family. What the study saw is that a stronger challenger finds more.
6. *Accounts are not facts.* A lineage's description of its own variation was false in three of eighteen cases, a model's arithmetic was not trusted for exemplars, and the assistant's own record held errors that bindings, a reading by hand and an independent review each found in part. Whatever can be recomputed is recomputed by the ground, and what a count is said to mean needs a reader of another origin.
7. *Bounds in characters.* Words are an unsteady measure of a text that holds formulas. The bounds of this study did not bind, so the choice of characters or tokens is a matter of design and no finding.
8. *Contracts and plans are claims.* A tolerance of the plan was wrong, a binding was wrong three times and a reader of citations twice. A site's declaration is revised by a recorded act when its own results refute it, with both analyses kept. The plan itself was not sealed before the runs; a plan is a claim, and the ground should seal it.
9. *The grain of a fact is the grain of its claim.* The design stakes and accepts by function, and the checks of a repository fail by development. Here {grain['faulty_functions_rejected_only_with_their_development']} faulty functions had no failing check that calls them. A fact attached to a claim must be a fact about that claim's object, and a harness should say which checks call which parts.
10. *A shown balance is no budget.* Showing a lineage its balance is necessary, as the third prototype found, and it does not suffice: the careful model staked beyond its means when shown its balance, here and on the theory site. The refusal of the ground is the provision.

## 7. Cost, Files and Reproduction

The studies made {total_calls} calls to models: {', '.join(f'{n} for {what}' for what, n in calls.items())}. One challenger of the theory site had its answer cut off at its limit of tokens in three tries and was asked again with a larger limit; the four calls are among the 28. The counts are those of the ledgers that the stages printed, except for the developments of the third unit, whose ledger was not kept and which are counted at one call each. What left the machine was the bodies, comments, signatures and theory notes of the three units (the bodies to the sequencer alone), the definitions of the fragment, and a table of results of the second prototype, all of it public in the repository or made for this purpose, with the names of persons and of the project removed.

| File | Content |
|---|---|
| `PLAN.md` | The plan, written before the runs and not sealed before them, and its addendum |
| `units.py` | The three units: their contracts, domains and hidden inputs |
| `harness_js.py` | The harness: the screen, the working folder, the repository's checks, the differential comparison, the mutants |
| `s1_kills.py` | Study A1 |
| `s2_units.py`, `s2_analyse.py` | Studies A2 and A3 and their measures |
| `s3_documents.py` | Study B |
| `theory_site.py`, `s4_theory.py` | Study C: the site, and its stages |
| `report.py` | This record and the dossier, written from the results |
| `dossier-transfer-networks.md` | The statements of Study C with their standing, for the author |
| `results/` | Every result as data; `b1-first-run.json` and `b4-later-readings.json` are counts recorded by hand |

: The files of the case studies.

The programs that call no model run again from this folder as they are: `python s1_kills.py`, `python s2_units.py evaluate`, `python s2_units.py ground`, `python s2_analyse.py`, `python s3_documents.py recompute`, `python s3_documents.py committed`, `python s3_documents.py cites`, `python s4_theory.py calibrate`, `python s4_theory.py settle` and `python report.py`. The stages that call a model keep what they received, and run again only for what is missing.
"""
    (HERE / "README.md").write_text(readme, encoding="utf-8", newline="\n")

    # ---- the dossier
    draft_lines = "\n".join(f"- A challenger of the {d['model']} model: {d['attempts'][0]['why']}." for d in settled["draft"].values())
    dossier = f"""# Transfer Networks: Statements with Their Standing

*A dossier for the author of the theory (2026-11-21), written by `report.py` from the results of Study C of [the case studies](README.md). It reports what procedures decided about statements in the vocabulary of Section 1 of the theory note of the economics domain. The question was posed by the assistant to exercise a site, and it is not the author's question. No statement here has been read, accepted or declined by the author, and none is part of the theory.*

## 1. What Was Asked and of Whom

Seven lineages, five of one model and two of another, were given the definitions of a transfer network, of the holonomy and loop deficit of a closed walk and of prices, with a vocabulary of twelve properties, each decided exactly by a procedure. They were asked which properties are enough for a network to have prices and which fall short. They were given none of the note's propositions. Each warranted claims with stakes; challengers of both models could refute an implication by a network.

## 2. The Standings Used

- *Supported within a bound.* No countermodel among {c1['table']['networks']:,} small networks ({settled['table']}) nor among {c1['larger']['networks']:,} larger ones ({settled['larger']}). This is a statement about those networks.
- *Proved by its witness.* The claim carries a network that has the premises and lacks the conclusion, and the harness verified it.
- *Refuted.* A counterexample was exhibited and verified with exact rationals.

No statement below is proved in general by a checker.

## 3. The Vocabulary

{chr(10).join(f"- `{name}`: {text}." for name, text in [(n, __import__('theory_site').PROPERTIES[n][0]) for n in __import__('theory_site').PROPERTIES])}

## 4. Implications Supported within the Bound

| Implication | Producers who warranted it | Networks meeting the premises, small and larger | Against the note's proposition on prices |
|---|---|---|---|
{implies_rows}

## 5. Separations Proved by a Witness

| Separation | Producers | A witness |
|---|---|---|
{separates_rows}

## 6. A Claim of the Drafts, Refuted

A scripted seat held the claim that the retentions of the positions of a cycle are fixed by the network, so that the position which absorbs the deficit of a cycle is a property of the network. Four challengers refuted it:

{draft_lines}

This agrees with Section 2 of the note, which holds that the split of a deficit is a property of the flows.

## 7. What No Procedure Decided

Whether the distinction between walks that follow the legs and walks that may go against them matters for the theory is a question of what a leg is taken to be: whether value can be carried against a leg. If arbitrage is carried only along legs, then the absence of arbitrage is the property `directed_flat`, and by the separations of Section 5 it falls short of prices unless the network is strongly connected or symmetric. Whether that is a result about the theory or an artefact of this vocabulary is for the author to say.

The implication `directed_flat` and `symmetric` implies `priced` is supported within the bound and is not derived by the rules of the harness. The assistant has an argument for it, given in the record, which no procedure has checked.

## 8. The Seats Left Empty

The author asked no question here, wrote no answer before the lineages answered, read no result and accepted none. A first trial with the author is described in the fourth case, Section 9.
"""
    (HERE / "dossier-transfer-networks.md").write_text(dossier, encoding="utf-8", newline="\n")
    print("wrote README.md", len(readme.split()), "words, and the dossier", len(dossier.split()), "words")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    main()
