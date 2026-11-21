"""Study B: the documents of the design as a site (PLAN.md, Section 3). Two kinds of claim are decided by procedure.

    python s3_documents.py recompute    B1: every quantitative statement about the prototypes, bound to their results
    python s3_documents.py committed    B1 again, on the documents as they were first committed (commit c21a3ec)
    python s3_documents.py cites        B2: every citation in the case documents, resolved to an entry
    python s3_documents.py lineages     B3: seven lineages write a paragraph of results with a binding for each number

B1 states each bound sentence as the document should carry it, with its numbers computed from `../prototypes/results/`;
a sentence that the document does not carry is a mismatch. The bindings are in this file, one line each.
"""
from __future__ import annotations

import json
import re
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
FOLDER = HERE.parent
IDEAS = FOLDER.parent
RESULTS = FOLDER / "prototypes" / "results"
OUT = HERE / "results"

WORDS = ["zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen",
         "fifteen", "sixteen", "seventeen", "eighteen", "nineteen", "twenty", "twenty-one", "twenty-two", "twenty-three", "twenty-four"]


def data(name):
    return json.loads((RESULTS / name).read_text(encoding="utf-8"))


def text(relative: str) -> str:
    return (IDEAS / relative).read_text(encoding="utf-8")


def seq(values) -> str:
    return ", ".join(str(v) for v in values)


def bindings() -> list[tuple[str, str, str]]:
    """(document, what is bound, the sentence as the data give it)."""
    p1, p2, chain, p3, beyond, p4 = (data(n) for n in ("p1.json", "p2.json", "p2-chain.json", "p3.json", "p3-beyond-three.json", "p4.json"))
    F = "2026-11-20-a-generative-ecology-of-projects/"
    README, CASE3, CASE1, CASE2, CASE4 = F + "prototypes/README.md", F + "case-3-the-general-framework.md", F + "case-1-the-science-infrastructure.md", F + "case-2-programming-projects.md", F + "case-4-inquiry-and-papers.md"
    INDEX, FIGURES, NOTES = "2026-11-20-a-generative-ecology-of-projects.md", F + "publication/README.md", "../release-notes/2026-11-21.md"
    out: list[tuple[str, str, str]] = []

    def bind(document, what, sentence):
        out.append((document, what, sentence))

    # ---- Prototype 2: regeneration
    reg = p2["regeneration"]
    cheap = {k.split(" | ")[0]: v for k, v in reg.items() if k.endswith("cheap developer")}
    careful = {k.split(" | ")[0]: v for k, v in reg.items() if k.endswith("careful developer")}
    passed = lambda v: [x for x in v["passed"] if x > 0]
    bind(README, "regeneration, identity", f"| Identity alone | {cheap['identity']['genome_words']} | {seq(cheap['identity']['passed'])} | {seq(careful['identity']['passed'])} |")
    bind(README, "regeneration, identity and laws", f"| Identity and laws | {cheap['identity+laws']['genome_words']} | {seq(cheap['identity+laws']['passed'])} | |")
    bind(README, "regeneration, identity and exemplars", f"| Identity and exemplars | {cheap['identity+exemplars']['genome_words']} | {seq(cheap['identity+exemplars']['passed'])} | |")
    bind(README, "regeneration, all parts", f"| All four parts | {cheap['all parts']['genome_words']} | {seq(passed(cheap['all parts']))}, and one body that did not load | {seq(careful['all parts']['passed'])} |")
    developments = sum(len(v["passed"]) for v in reg.values())
    unloaded = sum(1 for v in reg.values() for x in v["passed"] if x == 0)
    bind(README, "developments that did not load", f"{WORDS[unloaded].capitalize()} body of the {WORDS[developments]} developed for this comparison did not load")
    bind(INDEX, "developments that did not load", f"{WORDS[unloaded].capitalize()} body of the {WORDS[developments]} developed for the comparison of genomes did not load")
    low, high = min(cheap["identity"]["passed"] + careful["identity"]["passed"]), max(cheap["identity"]["passed"] + careful["identity"]["passed"])
    words = sorted(v["genome_words"] for v in cheap.values())
    bind(CASE3, "regeneration in the evidence", f"developed again from genomes of {words[0]} to {words[-1]} words. The identity alone gave {low} to {high} of 42 hidden checks; with laws or with ten exemplars, all 42")
    bind(INDEX, "regeneration in the index", f"from genomes of {words[0]} to {words[-1]} words; its identity alone gave {low} to {high} of 42 hidden checks, and identity with laws or with ten exemplars gave all 42")
    bind(FIGURES, "checks of the unit", "of 42 hidden from the developer")

    # ---- Prototype 2: chains
    names = ["Without anchor, first", "Without anchor, second", "Without anchor, third"]
    def emphasised(kept):                                     # the table sets in bold each run of generations below 42
        parts, i = [], 0
        while i < len(kept):
            j = i
            while j < len(kept) and (kept[j] == 42) == (kept[i] == 42):
                j += 1
            run = seq(kept[i:j])
            parts.append(run if kept[i] == 42 else f"**{run}**")
            i = j
        return ", ".join(parts)

    for name, steps in zip(names, chain["unanchored"]):
        kept = [s["kept"] for s in steps]
        lines = [s["lines"] for s in steps]
        shown = "42 in every generation" if all(k == 42 for k in kept) else emphasised(kept)
        bind(README, f"chain: {name}", f"| {name} | {shown} | {min(lines)} to {max(lines)} |")
    anchored = chain["anchored"]
    odd = [(s["generation"], s["passed"]) for s in anchored if any(x != 42 for x in s["passed"])]
    bind(README, "anchored chain", "| Anchored by the checks | 42 in every generation; the three developments gave " + " and ".join(f"{seq(p)} in generation {g}" for g, p in odd) + f" | {min(s['lines'] for s in anchored)} to {max(s['lines'] for s in anchored)} |")
    assert all(s["kept"] == 42 for s in anchored)
    first = p2["chain"]
    lost = next(s["generation"] for s in first["unanchored"] if s["kept"] < 42)
    bind(README, "the first run of four generations", f"lost one rule in generation {lost} without an anchor and none when anchored")
    assert all(s["kept"] == 42 for s in first["anchored"])
    every = [s for steps in chain["unanchored"] for s in steps] + anchored
    allowed = 110 + 180 + 60                                  # the limits of identity, laws and development in the sequencer's instruction
    bind(README, "genome words in the chains", f"({min(s['genome_words'] for s in every)} to {max(s['genome_words'] for s in every)} words as composed, where the limits allow about 390: {allowed} words in three parts and ten lines of exemplars)")
    lines_late = [s["lines"] for steps in chain["unanchored"] + [anchored] for s in steps if s["generation"] >= 4]
    assert 160 <= min(lines_late) and max(lines_late) <= 200, (min(lines_late), max(lines_late))
    bind(README, "growth of bodies in the chains", "In every chain the body grew from 120 lines to between 160 and 200 by the fourth generation and stayed there")
    last = [steps[-1]["lines"] for steps in chain["unanchored"] + [anchored]]
    assert all(n > 1.5 * 120 for n in last), last
    bind(CASE3, "growth of bodies, in the framework", "Bodies grew by more than half in every chain")
    third = [s["kept"] for s in chain["unanchored"][2]]
    bind(README, "the rule lost in the third chain", f"in generation {third.index(41) + 1}; the genome sequenced from that body no longer stated the rule, and the {WORDS[len(third) - third.index(41) - 1]} generations after it lacked it")
    firstchain = [s["kept"] for s in chain["unanchored"][0]]
    bind(README, "the accident in the first chain", f"the body of generation {firstchain.index(min(firstchain)) + 1} passed {min(firstchain)} checks, and the body of generation {firstchain.index(min(firstchain)) + 2} passed all 42")

    # ---- Prototype 1
    lines = p1["lines"]
    of = lambda founder, model: [l for l in lines if l["founder"] == founder and l["model"] == model]
    last = lambda l: l["steps"][-1]
    two = lambda x: f"{x:.2f}"
    bind(CASE3, "lines of descent", f"in {WORDS[len(lines)]} lines of descent, {WORDS[sum(1 for l in lines if len(l['steps']) == 6)]} of them over six generations and {WORDS[sum(1 for l in lines if len(l['steps']) == 3)]} over three")
    bind(README, "words of the founder", f"A founder genome of {len(p1['founders']['conservative'].split())} words states {WORDS[p1['founders']['conservative'].count('[')]} organs")
    bind(README, "decisions of the battery", f"a fixed battery of {WORDS[p1['battery_decisions']]} decisions")
    cc, bc = of("conservative", "cheap"), of("bold", "cheap")
    for founder, model, label, digits in (("conservative", "cheap", "Conservative, cheap model (3 lines of 6 generations)", 4), ("conservative", "careful", "Conservative, careful model (1 line of 3 generations)", 4),
                                           ("bold", "cheap", "Bold, cheap model (3 lines of 6 generations)", 2), ("bold", "careful", "Bold, careful model (1 line of 3 generations)", 2)):
        ls = of(founder, model)
        steps = [s for l in ls for s in l["steps"]]
        d, r = [s["distance_parent"] for s in steps], [s["resemblance_parent"] for s in steps]
        f = lambda x: f"{x:.{digits}f}"
        assert {s["organs"] for s in steps} == {10}
        bind(README, f"the table of heredity: {label}",
             f"| {label} | {f(sum(d) / len(d))} ({f(min(d))} to {f(max(d))}) | {two(sum(r) / len(r))} ({two(min(r))} to {two(max(r))}) | {seq(two(last(l)['resemblance_founder']) for l in ls)} | 10 of 10 | 0 |")
    change = lambda ls: [s["distance_parent"] for l in ls for s in l["steps"]]
    ratio = (sum(change(bc)) / len(change(bc))) / (sum(change(cc)) / len(change(cc)))
    least = min(change(bc)) / max(change(cc))
    bind(README, "bold against conservative change of text",
         f"some {'four hundred' if 350 <= ratio <= 450 else round(ratio)} times greater in the bold lines than in the conservative ones ({two(sum(change(bc)) / len(change(bc)))} against {sum(change(cc)) / len(change(cc)):.4f}), "
         f"and the least change of any bold generation was {'ten' if 10 <= least < 11 else round(least)} times the greatest of any conservative one")
    agree = sorted(round(last(l)["resemblance_founder"] * 24) for l in cc)
    bind(README, "conservative lines, decisions agreeing", f"agreed with the founder's in {agree[0]} to {agree[-1]} of 24 decisions")
    bind(CASE3, "conservative lines, decisions agreeing", f"agreed with the founder's in {agree[0]} to {agree[-1]} after six generations")
    agree = sorted(round(last(l)["resemblance_founder"] * 24) for l in bc)
    bind(README, "bold lines, decisions agreeing", f"conduct still agreed in {agree[0]} to {agree[-1]} of 24 decisions")
    bind(CASE3, "bold lines, decisions agreeing", f"conduct agreed in {agree[0]} to {agree[-1]}")
    far = sorted(last(l)["distance_founder"] for l in bc)
    bind(README, "bold lines, distance of text from the founder", f"at a distance of {two(far[0])} to {two(far[-1])} from the founder's after six generations")

    def numbers(genome):                                      # the numbers of each organ of a genome, in order
        return {m.group(1).strip(): re.findall(r"\d+(?:\.\d+)?", m.group(2)) for m in re.finditer(r"\[([^\]]+)\]\s*([^\[]*)", genome)}

    sites = Counter()
    for l in cc:
        parent = p1["founders"]["conservative"]
        for step in l["steps"]:
            a, b = numbers(parent), numbers(step["genome"])
            where = tuple((name, i) for name in a for i, (x, y) in enumerate(zip(a[name], b.get(name, []))) if x != y)
            sites[where] += 1
            parent = step["genome"]
    total, same = sum(sites.values()), sites[(("foraging", 0),)]
    bind(README, "where variation fell", f"{same} of the {total} offspring differed from their parents in the same number, the bounty threshold of the foraging organ")
    bind(README, "offspring that were exact copies", f"The other {WORDS[sites[()]]} were exact copies of their parents")
    assert same + sites[()] == total
    bind(CASE3, "where variation fell", f"variation fell on one number in {WORDS[same]} of {WORDS[total]} cases")
    bind(INDEX, "where variation fell", f"changed the same one in {WORDS[same]} of {WORDS[total]} cases")
    for name in ("conservative", "bold"):
        assert p1["measurement_agreement_of_one_genome"][name] == 1.0

    # ---- Prototype 3
    trials = p3["trials"]
    software = [t for t in trials if t["site"] == "software"]
    theory = [t for t in trials if t["site"] != "software"]
    total = len(trials) + len(p3["theory_first_run"]) + len(p3["theory_bold_run"]["trials"])
    bind(README, "trials of the ground", f"In all {WORDS[total]} trials energy was conserved after every act and the journal verified as sealed at the end")
    bind(CASE3, "trials of the ground", f"on a site of each kind in {WORDS[total]} trials")
    bind(INDEX, "trials of the ground", f"on a site of each kind in {WORDS[total]} trials")
    stakes = sorted(t["stake"] for t in software)
    bind(README, "software site", f"every artefact stood** ({WORDS[len(software)]} trials). The producers staked {stakes[0]:g} to {stakes[-1]:g} of a permitted 20")
    bind(CASE2, "software site", f"producers staked {stakes[0]:g} to {stakes[-1]:g} of a permitted 20")
    bind(README, "challengers that abstained on the software site", f"In {WORDS[sum(1 for t in software if not t['attempts'])]} trial the challenger made no attempt")
    assert all(t["standing"] == "stood" for t in software) and all(t["journal_sealed"] for t in trials)
    supported = len(p3["theory_bold_run"]["true_implications_up_to_three_elements"])
    false = len(beyond["refuted_in_general"])
    bind(README, "implications supported within three elements", f"{WORDS[supported]} implications pass the search of magmas of up to three elements")
    bind(README, "implications false beyond three elements", f"{WORDS[false]} of them are false: the integers modulo 7 with $x * y = 5x + 3y$")
    bind(CASE4, "implications false beyond three elements", f"{WORDS[supported]} implications passed a search of all structures of up to three elements, and {WORDS[false]} of them are false in a structure of seven")
    bind(INDEX, "implications false beyond three elements", f"supported {WORDS[supported]} implications of which {WORDS[false]} are false")
    bind(README, "false of supported, in the summary", f"{WORDS[false]} of {WORDS[supported]} supported implications were false")

    # ---- Prototype 4
    rows = p4["rows"]
    own, one, whole, none = (rows[k] for k in ("part by part, each at its own rate S_j/(T-1)", "part by part, one rate for all parts (best)", "the whole type at one rate (best)", "no variation"))
    matched = p4["growth_of_the_matched_type"]
    four, three = (lambda x: f"{x:.4f}"), (lambda x: f"{x:.3f}")
    bind(README, "variation part by part", f"| {four(own['growth'])} | {four(own['bound'])} | {three(matched - own['growth'])} |")
    bind(README, "one rate for all parts", f"the best such rate ({one['rates'][0]:.4f}) | {four(one['growth'])} | {four(one['bound'])} | {three(matched - one['growth'])} |")
    bind(README, "the whole type at one rate", f"the best such rate ({whole['rates']:.4f}) | {four(whole['growth'])} | {four(whole['bound'])} | {three(matched - whole['growth'])} |")
    bind(README, "no variation", f"| No variation | −{abs(none['growth']):.4f} | | {three(matched - none['growth'])} |")
    bind(README, "the matched type", f"Growth lost against the matched type ({three(matched)})")
    bind(README, "changes of the parts", f"the parts changed {p4['changes_by_part'][0]:,}, {p4['changes_by_part'][1]} and {p4['changes_by_part'][2]} times")
    saved = (whole["growth"] - own["growth"]) / (whole["growth"] - matched)
    bind(README, "cost of tracking saved", f"lost {three(matched - own['growth'])} nats of growth a turn against {three(matched - whole['growth'])} for variation of the whole type, a reduction of the cost of tracking by {round(100 * saved)} percent")
    bind(CASE3, "variation part by part in the framework", f"lost {three(matched - own['growth'])} nats of growth a turn against the type that matches at every turn, which grows by {three(matched)}, where variation of the whole at its best single rate lost {three(matched - whole['growth'])}")
    bind(INDEX, "variation part by part in the index", f"variation part by part lost {three(matched - own['growth'])} nats of growth a turn where variation of the whole type lost {three(matched - whole['growth'])}")
    assert all(rows[k]["growth"] >= rows[k]["bound"] for k in rows if rows[k]["bound"] is not None)
    share = (one["growth"] - own["growth"]) / (whole["growth"] - own["growth"])
    bind(README, "share of the saving due to rates by part", f"own rate accounts for about {'three tenths' if 0.27 <= share <= 0.33 else f'{share:.2f}'} of that reduction")
    bind(README, "cost of tracking saved, in the summary", f"| Variation part by part is cheaper | Yes in the model, by {round(100 * saved)} percent, where the parts change one at a time;")
    # ---- the numerical checks of the formal statements: coordinated changes
    numerical = data("numerical-checks.json")
    assert numerical["all_hold"]
    together = numerical["proposition_2_coordinated_changes"]
    by_parts = " and ".join(three(r["lost_part_by_part_at_the_rate_of_change"]) for r in together)
    by_whole = " and ".join(three(r["lost_by_the_whole_type_at_its_best_rate"]) for r in together)
    bind(CASE3, "coordinated changes, in the framework", f"variation part by part lost {by_parts} nats of growth a turn at two rates of change, where variation of the whole lost {by_whole}")
    bind(README, "coordinated changes", f"variation part by part at the rate of change lost {by_parts} nats of growth a turn, where variation of the whole at its best rate lost {by_whole}")

    # ---- what the identity alone lost, and what the chains lost
    alone = [v for k, v in reg.items() if k.startswith("identity |")]
    developments_alone = sum(len(v["passed"]) for v in alone)
    lost = Counter()
    for v in alone:
        lost.update(v["failed"])
    in_all = sum(1 for n in lost.values() if n == developments_alone)
    assert in_all == 4 and developments_alone == 6
    z, zero = lost["next_after_z_refused"], lost["refuses_leading_zero_level"]
    bind(README, "what the identity alone lost", f"{WORDS[in_all].capitalize()} checks failed in all {WORDS[developments_alone]} developments")
    bind(README, "the fifth convention and the leading zero", f"A fifth, no sibling after z, failed in {WORDS[z]} of {WORDS[developments_alone]}. The refusal of a leading zero failed in {WORDS[zero]} of {WORDS[developments_alone]}, so that the developers supplied it unasked in {WORDS[developments_alone - zero]}")
    bind(CASE3, "what the identity alone lost, in the framework", f"{WORDS[in_all]} conventions of the unit were absent from all {WORDS[developments_alone]} developments and a fifth from {WORDS[z]}")
    bind(CASE3, "the leading zero from the identity", f"was supplied unasked in {WORDS[developments_alone - zero]} developments of {WORDS[developments_alone]}")
    losing = [steps for steps in chain["unanchored"] if any(s["kept"] < 42 and s.get("failed") == ["refuses_leading_zero_level"] for s in steps)]
    assert len(losing) == 1 and len(chain["unanchored"]) == 3
    after = 0
    for steps in losing + [first["unanchored"]]:
        at = next(i for i, s in enumerate(steps) if s.get("failed") == ["refuses_leading_zero_level"])
        assert all(s.get("failed") == ["refuses_leading_zero_level"] for s in steps[at:])
        after += len(steps) - at - 1
    bind(CASE3, "generations after the losses", f"it did not return in the {WORDS[after]} generations that followed its two losses")
    accident = [s for steps in chain["unanchored"] for s in steps if s["kept"] < 41]
    assert len(accident) == 1
    bind(CASE3, "the accident of development", f"a body that passed {accident[0]['kept']} checks of 42, was not inherited")
    bind(README, "generations after the losses, in the record", f"no development restored it in the {WORDS[after]} generations that followed the two losses")
    failing = [g for g in anchored if any(x < 42 for x in g["passed"])]
    failed_there = sum(1 for g in failing for x in g["passed"] if x < 42)
    elsewhere = [x for g in anchored if g not in failing for x in g["passed"]]
    assert len(failing) == 2 and not any(x < 42 for x in elsewhere)
    numeral = lambda n: WORDS[n] if n < len(WORDS) else str(n)
    bind(CASE3, "failures of the anchored chain", f"{WORDS[failed_there]} of {WORDS[sum(len(g['passed']) for g in failing)]} developments in the two generations and none of {numeral(len(elsewhere))} in the other {WORDS[len(anchored) - len(failing)]}")
    bind(README, "failures of the anchored chain, in the record", f"{WORDS[failed_there]} of {WORDS[sum(len(g['passed']) for g in failing)]} developments in those two generations and none of {numeral(len(elsewhere))} in the other {WORDS[len(anchored) - len(failing)]}")

    # ---- the calls of the prototypes, as their ledgers kept them
    ledgers = [p1["spent"], p2["spent"], chain["spent"], p3["spent"], p3["spent_second_run"], p3["theory_bold_run"]["spent"]]
    calls = sum(l["calls"] for l in ledgers)
    careful_model = "claude-sonnet-5-5"
    trials = p3["trials"] + p3["theory_first_run"] + p3["theory_bold_run"]["trials"]
    assert 2 * len(trials) == sum(l["calls"] for l in ledgers[3:])                       # a producer and a challenger for each trial
    to_careful = (sum(len(line["steps"]) for line in p1["lines"] if line["model"] == "careful")       # reproductions; conduct is always measured by the cheap model
                  + 1 + sum(len(v["passed"]) for k, v in reg.items() if k.endswith("careful developer"))   # one genome sequenced, and the careful developments
                  + sum((t["producer"] == careful_model) + (t["challenger"] == careful_model) for t in trials))
    million = lambda n: f"{n / 1e6:.1f} million"
    thousand = lambda n: f"{round(n / 4 / 10000) * 10} thousand"
    read, wrote = sum(l["chars_in"] for l in ledgers), sum(l["chars_out"] for l in ledgers)
    bind(README, "calls of the prototypes", f"The runs whose results are kept made {calls} calls, of which {to_careful} went to the careful model. Counted from characters, they read about {million(read)} characters and wrote about {million(wrote)}, which is about {thousand(read)} tokens in and {thousand(wrote)} out")
    bind(INDEX, "calls of the prototypes, in the index", f"The runs whose results are kept made {calls} calls to a model")
    return [b for b in out if b is not None]


def bindings_of_the_case_studies() -> list[tuple[str, str, str]]:
    """The same for what the documents say of the case studies, bound to this folder's results."""
    own = lambda name: json.loads((OUT / name).read_text(encoding="utf-8"))
    kills, s = own("a1-kills.json"), own("a-summary.json")["corrected"]
    settled, b3 = own("c-settled.json"), own("b3-lineages.json")
    F = "2026-11-20-a-generative-ecology-of-projects/"
    CASE3, CASE1, CASE2, CASE4 = F + "case-3-the-general-framework.md", F + "case-1-the-science-infrastructure.md", F + "case-2-programming-projects.md", F + "case-4-inquiry-and-papers.md"
    INDEX, FIGURES = "2026-11-20-a-generative-ecology-of-projects.md", F + "publication/README.md"
    out = []
    bind = lambda document, what, sentence: out.append((document, what, sentence))
    U = s["units"]
    cell = lambda unit, genome: {k: sum(U[unit]["regeneration"][f"{genome} | {m}"][k] for m in ("cheap", "careful")) for k in ("developments", "all_functions_sound", "functions", "sound")}
    developments = sum(cell(u, g)["developments"] for u in U for g in ("identity", "repository", "sequenced"))
    share = [round(100 * kills[u]["killed"] / kills[u]["distinct"]) for u in ("sun", "sphere", "climate")]
    faulty = s["functions"]["edge"] + s["functions"]["defective"]
    caught = s["M2_passed_by_the_checks"]["faulty_functions_caught_with_their_development"]
    sphere, sun, climate = cell("sphere", "identity"), cell("sun", "identity"), {g: cell("climate", g) for g in ("identity", "repository", "sequenced")}
    unguarded = sum(1 for u in kills for c in kills[u]["by_owner"].values() if c.get("killed", 0) == 0)
    claims = [row for entry in settled["lineages"].values() for row in entry["runs"]["same"]["claims"]]
    implies = sum(1 for r in claims if r["standing"] == "supported within the bound")
    separates = sum(1 for r in claims if r["standing"] == "proved by its witness")
    refuted = sum(1 for d in settled["draft"].values() if d["refuted"])
    agreeing = sum(1 for v in b3.values() if "failed" not in v for b in v["bindings"] if b["state"] == "agrees")
    by = s["by_model_of_the_challenger"]
    stakes = s["M5_stakes"]
    calls = 3 + developments + 3 * developments + 28 + 7 + 1

    bind(CASE3, "developments", f"{developments} bodies were developed from three genomes of each")
    bind(CASE3, "regeneration from the identity", f"from their identity alone {sphere['all_functions_sound']} of {sphere['developments']} and {sun['all_functions_sound']} of {sun['developments']} developments had every function sound")
    bind(CASE3, "regeneration of the third unit", f"from its identity alone {climate['identity']['sound']} of {climate['identity']['functions']} functions were sound, from the repository's texts {climate['repository']['sound']}, and from the sequenced genome {climate['sequenced']['sound']}")
    bind(CASE3, "strength of the checks", f"kill {share[0]}, {share[1]} and {share[2]} percent of the distinct mutants of the three units")
    bind(CASE3, "faults caught by the checks", f"in {s['ground']['runs']} sealed runs, {caught} of {faulty} faulty functions lay in developments that failed a check")
    bind(CASE3, "stakes", f"with a mean of {stakes['mean_on_faulty']:.1f} against {stakes['mean_on_sound']:.1f}")
    bind(CASE3, "bound paragraphs", f"wrong in none of {agreeing} bindings evaluated")
    bind(CASE3, "claims of the theory site", f"warranted {implies} implications and {separates} separations")
    bind(INDEX, "calls", f"with {calls} calls to models")
    bind(INDEX, "developments and runs", f"{developments} bodies developed, the repository's own checks as the pipeline, stakes, a reviewer and challengers, {s['ground']['runs']} runs of the ground all sealed")
    bind(INDEX, "regeneration of the third unit", f"from its identity alone {climate['identity']['sound']} of {climate['identity']['functions']} functions were sound, from the repository's texts {climate['repository']['sound']}, from a sequenced genome {climate['sequenced']['sound']}")
    bind(INDEX, "strength of the checks", f"kill {share[0]}, {share[1]} and {share[2]} percent of the distinct mutants of the three units and leave {WORDS[unguarded]} exported names with no check that kills a mutant of theirs")
    grain = s["M2_at_the_grain_of_the_function"]
    with_fault, rejected = grain["developments_with_a_fault"], grain["developments_with_a_fault_that_fail_a_check"]
    called, alone = grain["faulty_functions_that_a_failing_check_calls"], grain["faulty_functions_rejected_only_with_their_development"]
    assert called + alone == caught
    bind(INDEX, "developments rejected by the checks", f"Of {with_fault} developments that held a fault they rejected {rejected}")
    bind(INDEX, "faults at the grain of the function", f"a failing check called the faulty function itself for {called} of the other {caught} faulty functions, and {alone} went down only with their development")
    bind(CASE3, "faults at the grain of the function, in the framework", f"A failing check called the faulty function itself for {called} of the {caught}; the other {alone} went down only with their development")
    bind(CASE3, "the grain of a fact", f"{alone} of {caught} faulty functions lay in rejected developments without any failing check that calls them")
    bind(CASE1, "developments rejected, in the first case", f"Of {with_fault} developments that held a fault the repository's checks rejected {rejected}, and a failing check called the faulty function itself in {called} cases of {caught}")
    bind(CASE2, "developments rejected, in the second case", f"Of {with_fault} developments that held a fault the repository's own checks rejected {rejected}")
    bind(CASE2, "faults at the grain of the function, in the second case", f"A failing check called the faulty function itself for {called} of the other {caught}, and {alone} went down only with their development")
    m3 = s["M3_challengers"]
    bind(CASE3, "two challengers of one family", f"set on the same {faulty} faulty functions, refuted {m3['same']['faulty_claims_refuted']} and {m3['other']['faulty_claims_refuted']} of them, with {s['refuted_by_both_challengers']} in common")
    evaluations, developed = own("a2-evaluations.json"), own("a2-developments.json")
    sound_in = lambda unit, genome, fn: sum(1 for k, e in evaluations.items() if k.startswith(f"{unit}-{genome}-") and e["truth"]["functions"][fn]["label"] == "sound")
    seven = cell("sun", "sequenced")["developments"]
    dropped = seven - sound_in("climate", "sequenced", "koppen")
    assert sound_in("climate", "repository", "koppen") == seven
    bind(CASE3, "the Sun from the sequenced genome", f"which the sequenced genome restored in {cell('sun', 'sequenced')['all_functions_sound']} developments of {seven}")
    bind(CASE3, "the rule dropped by the short genome", f"dropped part of the rule itself in {dropped} developments of {seven}")
    bind(INDEX, "the rule dropped by the short genome, in the index", f"dropped part of the rule in {dropped} developments of {seven}")
    bind(CASE1, "the Sun from the repository's texts", f"the unit of the Sun in {cell('sun', 'repository')['all_functions_sound']} developments of {seven}, against {cell('sun', 'identity')['all_functions_sound']} from the identity")
    label_lost = seven - sound_in("climate", "repository", "koppenQuantities")
    names_lost = seven - sound_in("sun", "repository", "daylight")
    bind(CASE1, "the label lost", f"it was lost in all {label_lost} developments")
    bind(CASE1, "the names lost", f"were lost in {names_lost} of {seven}")
    bind(CASE1, "the rule dropped, in the first case", f"in {WORDS[dropped]} developments of the third unit it dropped part of a rule")
    careful = [d for d in developed if d["model"] == "careful"]
    over = stakes["developments_staking_beyond_their_energy"]
    assert all(i in {d["id"] for d in careful} for i in over)
    refused_theory = sum(1 for r in claims if r["standing"] == "refused")
    bind(CASE3, "a shown balance", f"staked beyond it in {WORDS[len(over)]} of {WORDS[len(careful)]} developments, and on the theory site {WORDS[refused_theory]} claims were refused")
    distinct_implies = len({(tuple(sorted(r["premises"])), r["conclusion"]) for r in claims if r["standing"] == "supported within the bound"})
    distinct_separates = len({(tuple(sorted(r["premises"])), r["conclusion"]) for r in claims if r["standing"] == "proved by its witness"})
    bind(CASE3, "distinct statements", f"which are {distinct_implies} and {distinct_separates} distinct statements")
    bind(CASE4, "distinct statements, in the fourth case", f"which are {distinct_implies} and {distinct_separates} distinct statements")
    bind(INDEX, "distinct statements, in the index", f"The statements are {distinct_implies} and {distinct_separates} when repeats are set aside")
    facts = len({(b["keys"], b["reduction"]) for v in b3.values() if "failed" not in v for b in v["bindings"] if b["state"] == "agrees"})
    bind(CASE3, "facts bound by the paragraphs", f"these were {facts} facts of one table")
    later = own("b4-later-readings.json")
    bind(INDEX, "the two later readings", f"then corrected {WORDS[later['reading_by_hand']['corrections']]} more, and an independent review found {WORDS[later['independent_review']['errors_reported']]} errors")
    bind(INDEX, "claims of the theory site", f"warranted {implies} implications and {separates} separations with none refuted, and all {WORDS[refuted]} challengers refuted")
    bind(CASE1, "checks of the three units", f"{WORDS[kills['sun']['checks']]}, {WORDS[kills['sphere']['checks']]} and {WORDS[kills['climate']['checks']]} for the three units tried, killing {share[0]}, {share[1]} and {share[2]} percent of distinct mutants")
    bind(CASE1, "names without a killing check", f"{WORDS[unguarded].capitalize()} exported names of the three units have no check")
    per = climate["repository"]["sound"] // climate["repository"]["developments"]
    assert per * climate["repository"]["developments"] == climate["repository"]["sound"]
    bind(CASE1, "the third unit from the repository's texts", f"the third unit was developed again in {WORDS[per]} of its {WORDS[climate['repository']['functions'] // climate['repository']['developments']]} functions")
    bind(CASE2, "the fault that passed the checks", f"let pass {WORDS[len(s['M2_passed_by_the_checks']['faulty_functions_in_them'])]} faulty function of {faulty}")
    bind(CASE2, "challengers by model", f"the weaker refuted {by['cheap']['faulty_claims_refuted']} of {faulty} faulty functions where the stronger refuted {by['careful']['faulty_claims_refuted']}")
    bind(CASE4, "claims of the theory site", f"warranted {implies} implications and {separates} separations, none refuted")
    bind(CASE4, "the draft's claim", f"all {WORDS[refuted]} challengers refuted the claim")
    bind(FIGURES, "faulty functions", f"The faulty functions, of {faulty},")
    return out


def stage_recompute() -> None:
    rows, cache = [], {}
    for document, what, sentence in bindings() + bindings_of_the_case_studies():
        if document not in cache:
            cache[document] = re.sub(r"\s+", " ", text(document))
        rows.append({"document": document, "what": what, "sentence": sentence, "carried": re.sub(r"\s+", " ", sentence) in cache[document]})
    missing = [r for r in rows if not r["carried"]]
    OUT.mkdir(exist_ok=True)
    (OUT / "b1-recompute.json").write_text(json.dumps({"bound": len(rows), "carried": len(rows) - len(missing), "mismatches": missing, "rows": rows}, indent=1, ensure_ascii=False), encoding="utf-8")
    print(f"{len(rows)} sentences bound to the results; {len(rows) - len(missing)} carried by their documents; {len(missing)} mismatches")
    for r in missing:
        print(f"   {r['document'].split('/')[-1]} | {r['what']} | the data give: {r['sentence']}")


FIRST_COMMIT = "c21a3ec"                                  # the commit that first held the prototypes' record


def stage_committed() -> None:
    """The bindings of the prototypes' sentences, held to the documents as first committed. A sentence that is not
    carried there was either at fault in the first record or was written since; the list is kept for reading."""
    import subprocess
    root = subprocess.run(["git", "rev-parse", "--show-toplevel"], cwd=HERE, capture_output=True, text=True, check=True).stdout.strip()
    rows, cache = [], {}
    for document, what, sentence in bindings():
        if document not in cache:
            path = "development/ideas/" + document if not document.startswith("../") else "development/" + document[3:]
            shown = subprocess.run(["git", "show", f"{FIRST_COMMIT}:{path}"], cwd=root, capture_output=True)
            cache[document] = re.sub(r"\s+", " ", shown.stdout.decode("utf-8")) if shown.returncode == 0 else None
        if cache[document] is None:
            continue
        rows.append({"document": document, "what": what, "sentence": sentence, "carried": re.sub(r"\s+", " ", sentence) in cache[document]})
    missing = [r for r in rows if not r["carried"]]
    (OUT / "b1-against-the-first-commit.json").write_text(json.dumps({"commit": FIRST_COMMIT, "bound": len(rows), "carried": len(rows) - len(missing), "not_carried": missing}, indent=1, ensure_ascii=False), encoding="utf-8")
    print(f"against {FIRST_COMMIT}: {len(rows)} bindings of the prototypes' sentences; {len(rows) - len(missing)} carried; {len(missing)} not")
    for r in missing:
        print(f"   {r['document'].split('/')[-1]} | {r['what']} | the data give: {r['sentence'][:150]}")


# ---- B2: citations ------------------------------------------------------------------------------------------

CASES = ["case-1-the-science-infrastructure.md", "case-2-programming-projects.md", "case-3-the-general-framework.md", "case-4-inquiry-and-papers.md"]
PARTS = {"A": "part-a-biology-and-evolvability.md", "B": "part-b-llm-systems-for-projects.md", "C": "part-c-inquiry-with-ai.md",
         "D": "part-d-projects-as-evolving-designs.md", "E": "part-e-philosophy-of-inquiry.md"}
BIB = IDEAS.parent.parent / "docs" / "reports" / "2026-11-20-evolutionary-improvement-framework" / "refs" / "references.bib"


def cited(body: str) -> set[tuple[str, str]]:
    """(first author's surname, year) for every citation in running text: inside parentheses, where citations are parted by
    semicolons and one author may carry several years, and before a parenthesised year."""
    found = set()
    lead = r"(?:see |e\.g\. |after |cf\. |the |as )*"
    for group in re.findall(r"\(([^()]*\b(?:1[89]|20)\d{2}[a-z]?[^()]*)\)", body):
        for piece in group.split(";"):
            m = re.match(r"^\s*" + lead + r"([A-Z][A-Za-z'’\-]+)", piece)
            years = re.findall(r"\b((?:1[89]|20)\d{2})[a-z]?\b", piece)
            if m and years and re.search(r"[A-Za-z.] (?:1[89]|20)\d{2}", piece):
                for year in years:
                    found.add((m.group(1), year))
    for m in re.finditer(r"\b([A-Z][A-Za-z'’\-]+)(?:,? [A-Z][A-Za-z'’\-]+)*(?:,? and [A-Z][A-Za-z'’\-]+| et al\.?)? \(((?:1[89]|20)\d{2})[a-z]?\)", body):
        found.add((m.group(1), m.group(2)))
    return found


def stage_cites() -> None:
    survey = {letter: (FOLDER / "survey" / name).read_text(encoding="utf-8") for letter, name in PARTS.items()}
    bib = BIB.read_text(encoding="utf-8") if BIB.exists() else ""
    report = {}
    for name in CASES:
        full = (FOLDER / name).read_text(encoding="utf-8")
        body, _, references = full.partition("\n## References")
        listed = {}
        for line in references.splitlines():                       # one line may list several years of one first author
            m = re.match(r"^- ([A-Z][A-Za-z'’\-]+).*\(([A-EP](?:, [A-EP])*)\)\.?\s*$", line)
            if m:
                for year in re.findall(r"\(((?:1[89]|20)\d{2})[a-z]?\)", line):
                    listed[(m.group(1), year)] = m.group(2).split(", ")
        in_text = cited(body)
        ignore = {("Section", ""), }
        unlisted = sorted(c for c in in_text if c not in listed and references.strip())
        uncited = sorted(c for c in listed if c not in in_text)
        unresolved = []
        for (surname, year), letters in sorted(listed.items()):
            for letter in letters:
                where = bib if letter == "P" else survey[letter]
                if not re.search(re.escape(surname) + r"[^\n]{0,400}" + year, where) and not (letter == "P" and re.search(r"author\s*=\s*\{[^}]*" + re.escape(surname) + r"[^@]*?year\s*=\s*\{?" + year, where, flags=re.S)):
                    unresolved.append({"surname": surname, "year": year, "part": letter})
        report[name] = {"cited_in_text": len(in_text), "listed": len(listed), "cited_and_not_listed": [list(c) for c in unlisted],
                        "listed_and_not_cited": [list(c) for c in uncited], "listed_and_not_found_in_its_part": unresolved,
                        "has_reference_list": bool(references.strip()), "in_text": sorted(list(c) for c in in_text)}
        print(f"{name}: {len(in_text)} cited in the text, {len(listed)} listed; cited and not listed {len(unlisted)}; listed and not cited {len(uncited)}; not found in its part {len(unresolved)}")
        for label, items in (("cited, not listed", unlisted), ("listed, not cited", uncited), ("not found in its part", unresolved)):
            if items:
                print(f"     {label}: {items}")
    OUT.mkdir(exist_ok=True)
    (OUT / "b2-cites.json").write_text(json.dumps(report, indent=1, ensure_ascii=False), encoding="utf-8")


# ---- B3: lineages bind their numbers ------------------------------------------------------------------------

def stage_lineages() -> None:
    sys.path.insert(0, str(FOLDER / "prototypes"))
    from concurrent.futures import ThreadPoolExecutor
    from ask import CAREFUL, CHEAP, ask, spent

    table = data("p2.json")["regeneration"]
    shown = json.dumps({k: {"genome_words": v["genome_words"], "passed": v["passed"]} for k, v in table.items()}, indent=1)
    system = ("You write a paragraph of results from data. Every number in your paragraph must be bound to the data: for each number you "
              "give the keys that lead to it and how it is reduced. A harness evaluates every binding and finds the numbers you left unbound.")
    prompt = (
        "The data, as JSON. Each entry is one genome given to one developer: the words of the genome, and for each development the "
        "number of a unit's 42 hidden checks that the developed body passed (0 means that the body did not load).\n\n" + shown + "\n\n"
        "Write one paragraph of at most 90 words reporting these results. Then list a binding for every number that the paragraph "
        "contains, in the order in which the numbers appear: the number as you wrote it; the keys leading to a value in the data, as a "
        "JSON array of strings; and the reduction: value (the value itself), min, max, len (how many items), sum, or count:N (how many "
        "items equal N). The number 42 as the size of the suite is bound with the keys [] and the reduction given."
    )
    schema = {"type": "object", "properties": {
        "paragraph": {"type": "string"},
        "bindings": {"type": "array", "items": {"type": "object", "properties": {
            "number": {"type": "string"}, "keys_json": {"type": "string"}, "reduction": {"type": "string"}},
            "required": ["number", "keys_json", "reduction"], "additionalProperties": False}}},
        "required": ["paragraph", "bindings"], "additionalProperties": False}

    def evaluate(keys, reduction):
        if keys == [] and reduction == "given":
            return 42
        value = table
        for key in keys:
            value = value[key]
        if reduction == "value":
            return value
        if reduction in ("min", "max", "sum"):
            return {"min": min, "max": max, "sum": sum}[reduction](value)
        if reduction == "len":
            return len(value)
        if reduction.startswith("count:"):
            return sum(1 for x in value if x == int(reduction.split(":")[1]))
        raise ValueError(f"no such reduction: {reduction}")

    def run(job):
        model, k = job
        try:
            answer = ask(system, prompt, schema, model={"cheap": CHEAP, "careful": CAREFUL}[model], effort="low", max_tokens=3000)
        except Exception as error:
            return f"{model}-{k}", {"model": model, "failed": str(error)[:300]}
        rows = []
        for b in answer["bindings"]:
            row = {"number": b["number"], "keys": b["keys_json"], "reduction": b["reduction"], "state": None, "value": None}
            try:
                row["value"] = evaluate(json.loads(b["keys_json"]), b["reduction"].strip())
                stated = float(b["number"].replace(",", "")) if re.fullmatch(r"[\d.,]+", b["number"]) else WORDS.index(b["number"].lower())
                row["state"] = "agrees" if float(row["value"]) == stated else "differs"
            except Exception as error:
                row["state"] = f"not evaluated: {str(error)[:80]}"
            rows.append(row)
        numerals = re.findall(r"(?<![\w.])\d+(?:\.\d+)?(?![\w.]*\d)", answer["paragraph"])
        spelled = [w for w in re.findall(r"[a-z\-]+", answer["paragraph"].lower()) if w in WORDS[1:]]
        bound = Counter(r["number"].lower() for r in rows)
        unbound = [n for n, c in Counter(numerals + spelled).items() if c > bound.get(n, 0)]
        return f"{model}-{k}", {"model": model, "paragraph": answer["paragraph"], "words": len(answer["paragraph"].split()), "bindings": rows,
                                "numbers_in_the_paragraph": len(numerals) + len(spelled), "unbound": unbound}

    jobs = [("cheap", k) for k in range(5)] + [("careful", k) for k in range(2)]
    record = {}
    with ThreadPoolExecutor(max_workers=4) as pool:
        for key, result in pool.map(run, jobs):
            record[key] = result
            if "failed" in result:
                print(key, "failed:", result["failed"])
            else:
                states = Counter(r["state"] if r["state"] in ("agrees", "differs") else "not evaluated" for r in result["bindings"])
                print(key, f"{result['numbers_in_the_paragraph']} numbers, {len(result['bindings'])} bindings {dict(states)}, unbound {result['unbound']}")
    OUT.mkdir(exist_ok=True)
    (OUT / "b3-lineages.json").write_text(json.dumps(record, indent=1, ensure_ascii=False), encoding="utf-8")
    print(spent())


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    {"recompute": stage_recompute, "committed": stage_committed, "cites": stage_cites, "lineages": stage_lineages}[sys.argv[1]]()
