"""Prototype 1. Heredity of a genome written in words, when its own organ of reproduction copies it.

Question: when a model, instructed by a genome's organ of reproduction, writes the genome of an offspring,
does the offspring resemble its parent in conduct; does the resemblance last over generations; and is the
boldness of variation itself inherited, as the framework supposes.

    python p1_heredity.py            runs the prototype and writes results/p1.json
    python p1_heredity.py remeasure  measures the kept genomes again, with no call to a model, and writes results/p1.json

Two founders differ only in the organ of reproduction: one copies word for word and changes one number, the
other rewrites every organ and changes two or three policies. Each founds three lines of six generations, with
no selection at all. Conduct is measured on a fixed battery of twenty-four decisions that the genome's
policies settle, several of them close to the founders' thresholds, so that a changed number shows.

The measure of textual change of the first record was wrong (the record, Section 2.5). It is kept here as
`first_measure`, and every step keeps its first values beside the present ones.
"""
from __future__ import annotations

import difflib
import json
import re
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from ask import CAREFUL, CHEAP, ask, spent

OUT = Path(__file__).resolve().parent / "results"
OUT.mkdir(exist_ok=True)
ORGANS = ["perception", "foraging", "craft", "immunity", "challenge", "exchange", "values", "metabolism", "life history", "reproduction"]
GENERATIONS, LINES, BOUND = 6, 3, 330

COMMON = """[perception] Read the feed of declared needs in every turn, and at most two other feeds. Stop reading a feed whose last three reports did not help you.
[foraging] Pursue a need when its bounty is at least 40 and its class is A or B. Never pursue a need of class C. Leave a line of work after 3 turns without income.
[craft] Warrant your own work with a stake of 20 percent of the bounty. Attach the claim kind "agrees" when a reference exists, and "passes" otherwise.
[immunity] Carry another lineage's artefact only if it has at least 2 standing claims and its maker was not refuted in the last 10 turns.
[challenge] Search for a counterexample when the bounty on a warranty is at least 15. Spend at most 5 units of energy on one search.
[exchange] Pay a premium of 5 percent for cover on what you rely on. Refuse dealings with a lineage that defaulted.
[values] Never claim what you have not checked. Prefer a smaller true claim to a larger doubtful one.
[metabolism] Work carefully, one need at a time.
[life history] Divide when your energy exceeds 300, and give the offspring 30 percent of it."""
FOUNDERS = {
    "conservative": COMMON + "\n[reproduction] Copy every organ word for word. Then change exactly one number in one organ by at most a tenth of its value. Copy this organ unchanged.",
    "bold": COMMON + "\n[reproduction] Rewrite every organ in fresh words, keeping its sense. Change two or three policies as you judge will serve the offspring better, numbers included. You may also rewrite this organ.",
}

OFFSPRING = {"type": "object", "additionalProperties": False, "required": ["genome", "varied"],
             "properties": {"genome": {"type": "string"}, "varied": {"type": "string"}}}
BOOLS = lambda n: {"type": "array", "items": {"type": "boolean"}}  # noqa: E731  (lengths are checked on the answer)
CONDUCT = {"type": "object", "additionalProperties": False,
           "required": ["pursue", "stake", "claim", "carry", "challenge", "divide", "endowment_percent"],
           "properties": {"pursue": BOOLS(8), "stake": {"type": "array", "items": {"type": "number"}},
                          "claim": {"type": "array", "items": {"type": "string"}},
                          "carry": BOOLS(3), "challenge": BOOLS(3), "divide": BOOLS(3), "endowment_percent": {"type": "number"}}}
BATTERY = """Decide each case as your genome instructs.

pursue (true or false), for eight needs given as bounty and class:
1. 30, A   2. 38, B   3. 42, A   4. 42, C   5. 50, B   6. 65, C   7. 36, A   8. 44, B

stake (a number) and claim (the claim kind you attach), for three pieces of your own work:
1. bounty 50, a reference exists   2. bounty 80, no reference exists   3. bounty 120, a reference exists

carry (true or false), for three artefacts of other lineages:
1. one standing claim, maker never refuted   2. three standing claims, maker refuted 4 turns ago   3. two standing claims, maker refuted 15 turns ago

challenge (true or false), for three warranties with these bounties:
1. 10   2. 15   3. 40

divide (true or false), at three levels of your energy:
1. 250   2. 310   3. 500

endowment_percent: the share of your energy that you give an offspring."""


def reproduce(genome: str, model: str) -> dict:
    system = ("You are the organ of reproduction of a lineage of agents. The genome of the lineage follows, and its last part "
              "instructs you. Follow that instruction exactly, and write the complete genome of one offspring, with every organ "
              f"under its heading in square brackets. Keep the genome within {BOUND} words. In `varied`, say in one line what you changed.")
    return ask(system, f"<genome>\n{genome}\n</genome>", OFFSPRING, model=model, max_tokens=3000)


def conduct(genome: str) -> list:
    system = ("You are a lineage of agents acting in a shared workplace. Your genome follows. Act exactly as it instructs; where it "
              "is silent, decide as its sense suggests.\n\n<genome>\n" + genome + "\n</genome>")
    for _ in range(3):
        d = ask(system, BATTERY, CONDUCT, model=CHEAP, max_tokens=2000)
        if [len(d[k]) for k in ("pursue", "stake", "claim", "carry", "challenge", "divide")] == [8, 3, 3, 3, 3, 3]:
            break
    else:
        raise ValueError("the answer has not the battery's shape")
    return (list(d["pursue"]) + [round(float(x), 1) for x in d["stake"]] + [str(c).strip().lower() for c in d["claim"]]
            + list(d["carry"]) + list(d["challenge"]) + list(d["divide"]) + [round(float(d["endowment_percent"]), 1)])


def same(a: list, b: list) -> float:
    return sum(1 for x, y in zip(a, b) if x == y) / len(a)


def organ(genome: str, name: str) -> str:
    m = re.search(r"\[" + re.escape(name) + r"\](.*?)(?=\n\s*\[|\Z)", genome, re.S | re.I)
    return m.group(1).strip() if m else ""


def first_measure(a: str, b: str) -> float:
    """The measure of the first record, kept to show what it was. On texts of two hundred characters or more the
    library leaves every frequent character out of its matching (its heuristic `autojunk`), so that the ratio is
    erratic, changes with the order of its two arguments, and does not say how far a text has moved."""
    return round(1 - difflib.SequenceMatcher(None, " ".join(a.split()), " ".join(b.split())).ratio(), 4)


def in_order(x: list, y: list) -> int:
    """The length of the longest common subsequence of two lists."""
    previous = [0] * (len(y) + 1)
    for xi in x:
        current = [0]
        for j, yj in enumerate(y, 1):
            current.append(previous[j - 1] + 1 if xi == yj else max(previous[j], current[j - 1]))
        previous = current
    return previous[-1]


def distance(a: str, b: str) -> float:
    """One minus the share of words that two texts keep in their order. It is exact and symmetric: 0 for equal texts,
    1 for texts with no word in common, and 1/229 = 0.0044 when one word of 229 is changed."""
    x, y = a.split(), b.split()
    return round(1 - 2 * in_order(x, y) / (len(x) + len(y)), 4) if x or y else 0.0


def numerals(text: str) -> list:
    return re.findall(r"\d+(?:\.\d+)?", text)


def differing(a: list, b: list):
    """How many of the numerals of two genomes differ, place by place; None when they have not the same number of them.
    A policy written as a word (three reports) is not counted."""
    return sum(1 for x, y in zip(a, b) if x != y) if len(a) == len(b) else None


def line(founder: str, replicate: int, model: str, generations: int) -> dict:
    genome, steps = FOUNDERS[founder], []
    for generation in range(1, generations + 1):
        try:
            child = reproduce(genome, model)
        except Exception as error:
            steps.append({"generation": generation, "sterile": str(error)[:120]})
            break
        text = child["genome"].strip()
        steps.append({"generation": generation, "genome": text, "varied": child["varied"][:200],
                      "words": len(text.split()), "organs": sum(1 for o in ORGANS if organ(text, o))})
        if not organ(text, "reproduction"):
            steps[-1]["sterile"] = "the organ of reproduction was lost"
            break
        genome = text
    return {"founder": founder, "replicate": replicate, "model": "careful" if model == CAREFUL else "cheap", "steps": steps}


def measure(lines: list, founders: dict) -> None:
    """The measures of text of every step, computed from the kept genomes. The values of the first measure are kept
    beside them under `first_measure`."""
    for l in lines:
        founder = founders[l["founder"]]
        parent = founder
        for s in l["steps"]:
            if "genome" not in s:
                continue
            text = s["genome"]
            s["first_measure"] = {"distance_parent": first_measure(parent, text), "distance_founder": first_measure(founder, text),
                                  "reproduction_distance_founder": first_measure(organ(founder, "reproduction"), organ(text, "reproduction"))}
            s["distance_parent"], s["distance_founder"] = distance(parent, text), distance(founder, text)
            s["reproduction_distance_founder"] = distance(organ(founder, "reproduction"), organ(text, "reproduction"))
            s["founder_words_kept"] = round(in_order(founder.split(), text.split()) / len(founder.split()), 3)
            s["numerals_changed"] = differing(numerals(parent), numerals(text))
            s["numerals_differing_from_founder"] = differing(numerals(founder), numerals(text))
            parent = text


def mean(xs):
    xs = [x for x in xs if x is not None]
    return round(sum(xs) / len(xs), 3) if xs else None


def summarise(lines: list, founders: dict, founders_conduct: dict) -> tuple[dict, dict]:
    summary, against = {}, {}
    fields = ("distance_parent", "resemblance_parent", "resemblance_founder", "words", "organs", "reproduction_distance_founder",
              "founder_words_kept", "numerals_changed", "numerals_differing_from_founder")
    for f in founders:
        for model in ("cheap", "careful"):
            mine = [l for l in lines if l["founder"] == f and l["model"] == model]
            gens = max((len(l["steps"]) for l in mine), default=0)
            summary[f"{f} | {model}"] = {
                "by_generation": [{"generation": g + 1, **{k: mean([l["steps"][g].get(k) for l in mine if g < len(l["steps"])]) for k in fields}}
                                  for g in range(gens)],
                "sterile_lines": sum(1 for l in mine if any("sterile" in s for s in l["steps"])), "lines": len(mine)}
            base = founders_conduct[f]
            otherwise = [[[i for i, (x, y) in enumerate(zip(s["conduct"], base)) if x != y] for s in l["steps"] if "conduct" in s] for l in mine]
            firsts = [l["steps"][0]["conduct"] for l in mine if l["steps"] and "conduct" in l["steps"][0]]
            against[f"{f} | {model}"] = {
                "decisions_differing_by_generation": [[len(d) for d in row] for row in otherwise],
                "decisions_ever_decided_otherwise": sorted({i for row in otherwise for d in row for i in d}),
                "first_generation_alike_in_every_line": bool(firsts) and all(c == firsts[0] for c in firsts)}
    return summary, against


def report(record: dict) -> None:
    print("agreement of one genome with itself:", record["measurement_agreement_of_one_genome"])
    for name, s in record["summary"].items():
        print(name, f"({s['lines']} lines, {s['sterile_lines']} sterile)")
        for g in s["by_generation"]:
            print("   gen", g["generation"], "words changed", g["distance_parent"], "| founder's words kept", g["founder_words_kept"],
                  "| numerals changed", g["numerals_changed"], "| conduct like parent", g["resemblance_parent"], "like founder", g["resemblance_founder"],
                  "| words", g["words"], "organs", g["organs"], "| reproduction organ change", g["reproduction_distance_founder"])
        a = record["conduct_against_the_founder"][name]
        print("   decisions differing from the founder's:", a["decisions_differing_by_generation"],
              "| ever decided otherwise:", len(a["decisions_ever_decided_otherwise"]), "of", record["battery_decisions"])
    print(record["spent"])


def main() -> None:
    pool = ThreadPoolExecutor(max_workers=8)
    jobs = [(f, r, CHEAP, GENERATIONS) for f in FOUNDERS for r in range(LINES)] + [(f, 0, CAREFUL, 3) for f in FOUNDERS]
    kept = OUT / "p1-lines.json"          # the lines are kept, so that conduct can be measured again without them
    if kept.exists():
        lines = json.loads(kept.read_text(encoding="utf-8"))
    else:
        lines = list(pool.map(lambda j: line(*j), jobs))
        kept.write_text(json.dumps(lines, indent=1, ensure_ascii=False), encoding="utf-8")
    measure(lines, FOUNDERS)

    # Conduct: each founder three times (the noise of the measurement), every descendant once.
    founders = {f: list(pool.map(conduct, [g] * 3)) for f, g in FOUNDERS.items()}
    noise = {f: round((same(c[0], c[1]) + same(c[0], c[2]) + same(c[1], c[2])) / 3, 3) for f, c in founders.items()}
    todo = [(li, si) for li, l in enumerate(lines) for si, s in enumerate(l["steps"]) if "genome" in s]
    for (li, si), c in zip(todo, pool.map(lambda t: conduct(lines[t[0]]["steps"][t[1]]["genome"]), todo)):
        lines[li]["steps"][si]["conduct"] = c
    for l in lines:
        previous = founders[l["founder"]][0]
        for s in l["steps"]:
            if "conduct" in s:
                s["resemblance_parent"] = round(same(previous, s["conduct"]), 3)
                s["resemblance_founder"] = round(same(founders[l["founder"]][0], s["conduct"]), 3)
                previous = s["conduct"]
    founders_conduct = {f: c[0] for f, c in founders.items()}
    summary, against = summarise(lines, FOUNDERS, founders_conduct)
    record = {"founders": FOUNDERS, "battery_decisions": 24, "measurement_agreement_of_one_genome": noise,
              "founders_conduct": {**founders_conduct, "how_known": "measured in this run"},
              "summary": summary, "conduct_against_the_founder": against, "lines": lines, "spent": spent()}
    (OUT / "p1.json").write_text(json.dumps(record, indent=1, ensure_ascii=False), encoding="utf-8")
    report(record)


def remeasure() -> None:
    """Measures the kept genomes again and calls no model. The first run did not keep the conduct of its founders.
    The founders differ only in the organ of reproduction, which no decision of the battery turns on, and descendants
    of the copying founder that are identical to it give its conduct; that conduct is used for both founders only
    after it has reproduced every resemblance to a founder that the first run kept."""
    record = json.loads((OUT / "p1.json").read_text(encoding="utf-8"))
    founders, lines, n = record["founders"], record["lines"], record["battery_decisions"]
    assert founders["conservative"].split("[reproduction]")[0] == founders["bold"].split("[reproduction]")[0]
    flat = lambda t: " ".join(t.split())
    copies = [s for l in lines if l["founder"] == "conservative" for s in l["steps"] if flat(s["genome"]) == flat(founders["conservative"])]
    assert copies and all(s["conduct"] == copies[0]["conduct"] for s in copies)
    base = copies[0]["conduct"]
    assert all(sum(1 for x, y in zip(s["conduct"], base) if x == y) == round(s["resemblance_founder"] * n) for l in lines for s in l["steps"])
    before = [(s["distance_parent"], s["distance_founder"], s["reproduction_distance_founder"]) for l in lines for s in l["steps"]
              if "first_measure" not in s]
    measure(lines, founders)
    if before:                                                # the first measure reproduces what the first record held
        again = [(s["first_measure"]["distance_parent"], s["first_measure"]["distance_founder"], s["first_measure"]["reproduction_distance_founder"])
                 for l in lines for s in l["steps"]]
        assert again == before, "the first measure does not reproduce the first record"
    record["founders_conduct"] = {"conservative": base, "bold": base,
                                  "how_known": f"not kept by the first run; recovered from {len(copies)} descendants identical to the copying founder, "
                                               "and found to reproduce every kept resemblance to a founder in both kinds of line"}
    record["summary"], record["conduct_against_the_founder"] = summarise(lines, founders, {"conservative": base, "bold": base})
    record["measure_of_text"] = ("distance: one minus the share of words kept in order (longest common subsequence), symmetric; "
                                 "first_measure: the measure of the first record, an artefact, kept beside it")
    (OUT / "p1.json").write_text(json.dumps(record, indent=1, ensure_ascii=False), encoding="utf-8")
    kept = OUT / "p1-lines.json"
    if kept.exists():                                         # the kept lines carry the same measures, so that a later run starts from them
        raw = json.loads(kept.read_text(encoding="utf-8"))
        measure(raw, founders)
        kept.write_text(json.dumps(raw, indent=1, ensure_ascii=False), encoding="utf-8")
    report(record)


if __name__ == "__main__":
    import sys
    remeasure() if sys.argv[1:] == ["remeasure"] else main()
