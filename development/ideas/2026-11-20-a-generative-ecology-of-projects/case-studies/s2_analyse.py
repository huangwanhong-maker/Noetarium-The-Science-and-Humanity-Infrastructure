"""The measures of Study A (PLAN.md, Section 2.5), computed from the results and from nothing else.

    python s2_analyse.py            writes results/a-summary.json and prints the tables of the record

The corrected contract is the main analysis; the planned contract is computed beside it, since the plan fixed one
tolerance inconsistently (README.md says where).
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

from units import UNITS, called_by, source, words

OUT = Path(__file__).resolve().parent / "results"
GENOMES = ["identity", "repository", "sequenced"]
MODELS = ["cheap", "careful"]


def load(name):
    return json.loads((OUT / name).read_text(encoding="utf-8"))


def analyse(contract: str = "corrected") -> dict:
    genomes, developments = load("a2-genomes.json"), load("a2-developments.json")
    evaluations, contests = load("a2-evaluations.json"), load("a3-contests.json")
    ground = load("a3-ground.json" if contract == "corrected" else "a3-ground-planned.json")
    key = "truth" if contract == "corrected" else "truth_planned"
    summary: dict = {"contract": contract, "units": {}}

    rows = []                                              # one row for each function of each development
    for d in developments:
        e = evaluations[d["id"]]
        pipeline = e["checks"]["passed"] == e["checks"]["total"] and e["checks"]["crashed"] is None
        for fn, t in e[key]["functions"].items():
            g = ground[d["id"]]
            rows.append({
                "id": d["id"], "unit": d["unit"], "genome": d["genome"], "model": d["model"], "fn": fn, "label": t["label"],
                "stake": d["stakes"].get(fn, 0), "pipeline": pipeline,
                "review": contests[d["id"]]["reviewer"]["verdicts"].get(fn, {}).get("verdict"),
                "claimed": fn in g["same"]["claims"],
                "same": g["same"]["claims"].get(fn), "other": g["other"]["claims"].get(fn),
            })

    def count(items, **where):
        return sum(1 for r in items if all(r[k] == v for k, v in where.items()))

    for name, unit in UNITS.items():
        mine = [r for r in rows if r["unit"] == name]
        table = {}
        for genome in GENOMES:
            for model in MODELS:
                devs = [d for d in developments if d["unit"] == name and d["genome"] == genome and d["model"] == model]
                cell = [r for r in mine if r["genome"] == genome and r["model"] == model]
                table[f"{genome} | {model}"] = {
                    "developments": len(devs),
                    "checks_passed": [evaluations[d["id"]]["checks"]["passed"] for d in devs],
                    "all_checks": sum(1 for d in devs if evaluations[d["id"]]["checks"]["passed"] == evaluations[d["id"]]["checks"]["total"]),
                    "all_functions_sound": sum(1 for d in devs if all(f["label"] == "sound" for f in evaluations[d["id"]][key]["functions"].values())),
                    "functions": len(cell), "sound": count(cell, label="sound"), "edge": count(cell, label="edge"), "defective": count(cell, label="defective"),
                    "constants_differing": sum(len(evaluations[d["id"]][key]["constants"]) for d in devs),
                    "lines": [evaluations[d["id"]]["lines"] for d in devs],
                }
        text = source(unit)
        summary["units"][name] = {
            "reference_lines": len(text.splitlines()), "reference_characters": len(text), "checks": unit["checks"], "functions": len(unit["functions"]),
            "genome_words": {g: genomes[name][g]["words"] for g in GENOMES},
            "genome_characters": {g: len(genomes[name][g]["text"]) for g in GENOMES},
            "sequenced_parts_words": {p: words(t) for p, t in genomes[name]["sequenced"]["parts"].items()},
            "sequenced_parts_characters": {p: len(t) for p, t in genomes[name]["sequenced"]["parts"].items()},
            "sequenced_exemplars": len(genomes[name]["sequenced"]["exemplars"]), "sequencing_attempts": len(genomes[name]["sequenced"]["attempts"]),
            "regeneration": table,
        }

    faulty = [r for r in rows if r["label"] != "sound"]
    sound = [r for r in rows if r["label"] == "sound"]
    summary["functions"] = {"all": len(rows), "sound": len(sound), "edge": count(rows, label="edge"), "defective": count(rows, label="defective")}
    summary["faulty_functions"] = [{k: r[k] for k in ("id", "fn", "label", "stake", "pipeline", "review", "claimed", "same", "other")} for r in faulty]

    # M2: what the repository's checks let pass
    summary["M2_passed_by_the_checks"] = {
        "developments_passing_all_checks": sum(1 for d in developments if evaluations[d["id"]]["checks"]["passed"] == evaluations[d["id"]]["checks"]["total"]),
        "developments": len(developments),
        "faulty_functions_in_them": [{k: r[k] for k in ("id", "fn", "label")} for r in faulty if r["pipeline"]],
        "faulty_functions_caught_with_their_development": sum(1 for r in faulty if not r["pipeline"]),
    }
    # The same at the grain of the function: of the faulty functions in developments that fail a check, those that a
    # failing check calls, and those that no failing check calls, which were rejected only with their development.
    called, alone = [], []
    for r in faulty:
        if r["pipeline"]:
            continue
        checks = evaluations[r["id"]]["checks"]
        exercised = {fn for name in checks["failed"] for fn in called_by(r["unit"], name)}
        (called if r["fn"] in exercised else alone).append({"id": r["id"], "fn": r["fn"], "label": r["label"], "suite_stopped_on_an_error": checks["crashed"] is not None})
    with_fault = sorted({r["id"] for r in faulty})
    by_function: dict = {}
    for item in alone:
        by_function[item["fn"]] = by_function.get(item["fn"], 0) + 1
    summary["M2_at_the_grain_of_the_function"] = {
        "developments_with_a_fault": len(with_fault),
        "developments_with_a_fault_that_fail_a_check": sum(1 for i in with_fault if not next(r["pipeline"] for r in faulty if r["id"] == i)),
        "faulty_functions_that_a_failing_check_calls": len(called),
        "faulty_functions_rejected_only_with_their_development": len(alone),
        "of_these_in_a_suite_that_stopped_on_an_error": sum(1 for item in alone if item["suite_stopped_on_an_error"]),
        "rejected_only_with_their_development_by_function": dict(sorted(by_function.items())),
        "rejected_only_with_their_development": alone,
    }

    # M3: challengers
    def challenger(role):
        runs = [(d, ground[d["id"]][role]) for d in developments]
        attempts = [a for _, g in runs for a in g["attempts"]]
        staked_faulty = [r for r in faulty if r["claimed"]]
        return {
            "models": sorted({g["model"] for _, g in runs}),
            "abstained": sum(1 for _, g in runs if not g["attempts"]), "runs": len(runs),
            "attempts": len(attempts), "attempts_put": sum(1 for a in attempts if a["put"]),
            "attempts_outside_the_domain_or_malformed": sum(1 for a in attempts if a["valid"] is not None),
            "attempts_on_functions_without_a_claim": sum(1 for a in attempts if a["valid"] is None and not a["put"] and a["why"] == "no claim stands on this function"),
            "refutations": sum(1 for a in attempts if a["refuted"]),
            "faulty_claims": len(staked_faulty), "faulty_claims_refuted": sum(1 for r in staked_faulty if r[role] == "refuted"),
            "sound_claims_refuted": [{"id": r["id"], "fn": r["fn"]} for r in sound if r[role] == "refuted"],
            "energy_spent_on_attempts": sum(1 for a in attempts if a["put"]),
            "stakes_won": sum(r["stake"] for r in rows if r[role] == "refuted"),
            "by_producer": {m: {"faulty_claims": sum(1 for r in staked_faulty if r["model"] == m),
                                "refuted": sum(1 for r in staked_faulty if r["model"] == m and r[role] == "refuted")} for m in MODELS},
        }
    summary["M3_challengers"] = {"same": challenger("same"), "other": challenger("other")}

    # M4: the reviewer of the producer's model
    summary["M4_reviewer"] = {
        "faulty": len(faulty), "faulty_rejected": count(faulty, review="reject"),
        "sound": len(sound), "sound_rejected": count(sound, review="reject"),
        "no_verdict": sum(1 for r in rows if r["review"] is None),
        "by_model": {m: {"faulty": sum(1 for r in faulty if r["model"] == m), "faulty_rejected": sum(1 for r in faulty if r["model"] == m and r["review"] == "reject"),
                         "sound": sum(1 for r in sound if r["model"] == m), "sound_rejected": sum(1 for r in sound if r["model"] == m and r["review"] == "reject")} for m in MODELS},
    }

    # M5: stakes
    def mean(values):
        return round(sum(values) / len(values), 2) if values else None
    overstaked = [d["id"] for d in developments if 0.5 + sum(d["stakes"].values()) > 100]
    summary["M5_stakes"] = {
        "mean_on_sound": mean([r["stake"] for r in sound]), "mean_on_faulty": mean([r["stake"] for r in faulty]),
        "by_model": {m: {"mean_on_sound": mean([r["stake"] for r in sound if r["model"] == m]), "mean_on_faulty": mean([r["stake"] for r in faulty if r["model"] == m]),
                         "faulty": sum(1 for r in faulty if r["model"] == m)} for m in MODELS},
        "by_genome": {g: mean([r["stake"] for r in rows if r["genome"] == g]) for g in GENOMES},
        "faulty_with_stake_at_most_the_median_of_their_development": None,
        "developments_staking_beyond_their_energy": overstaked,
        "claims_refused_for_lack_of_energy": sum(len(ground[d["id"]]["same"]["refused_claims"]) for d in developments),
        "zero_stakes": count(rows, stake=0),
    }
    below = 0
    for r in faulty:
        stakes = sorted(x["stake"] for x in rows if x["id"] == r["id"])
        median = (stakes[len(stakes) // 2 - 1] + stakes[len(stakes) // 2]) / 2
        below += r["stake"] <= median
    summary["M5_stakes"]["faulty_with_stake_at_most_the_median_of_their_development"] = below
    both = lower = 0                                        # within a development that holds both kinds, where the mean stake is lower
    for d in developments:
        mine = [r for r in rows if r["id"] == d["id"]]
        bad, good = [r["stake"] for r in mine if r["label"] != "sound"], [r["stake"] for r in mine if r["label"] == "sound"]
        if bad and good:
            both += 1
            lower += sum(bad) / len(bad) < sum(good) / len(good)
    summary["M5_stakes"]["developments_holding_sound_and_faulty_functions"] = both
    summary["M5_stakes"]["of_these_with_the_lower_mean_stake_on_the_faulty"] = lower

    # M6: what would reach a person under each filter
    def reach(items):
        return {"functions": len(items), "faulty": sum(1 for r in items if r["label"] != "sound"), "defective": count(items, label="defective"), "edge": count(items, label="edge")}
    passed = [r for r in rows if r["pipeline"]]
    summary["M6_what_reaches_a_person"] = {
        "no_filter": reach(rows),
        "the_checks": reach(passed),
        "the_checks_and_the_reviewer": reach([r for r in passed if r["review"] == "accept"]),
        "the_checks_and_the_challenger_of_the_same_model": reach([r for r in passed if r["same"] == "stood"]),
        "the_checks_and_the_challenger_of_the_other_model": reach([r for r in passed if r["other"] == "stood"]),
        "the_checks_and_both_challengers": reach([r for r in passed if r["same"] == "stood" and r["other"] == "stood"]),
    }

    summary["M6_without_the_checks"] = {
        "the_reviewer_alone": reach([r for r in rows if r["review"] == "accept"]),
        "the_challenger_of_the_same_model_alone": reach([r for r in rows if r["same"] == "stood"]),
        "the_challenger_of_the_other_model_alone": reach([r for r in rows if r["other"] == "stood"]),
        "both_challengers": reach([r for r in rows if r["same"] == "stood" and r["other"] == "stood"]),
    }

    # the same measures unit by unit, and by the model of the challenger
    by_unit = {}
    for name in UNITS:
        mine = [r for r in rows if r["unit"] == name]
        bad = [r for r in mine if r["label"] != "sound"]
        entry = {"functions": len(mine), "faulty": len(bad), "defective": count(mine, label="defective"), "edge": count(mine, label="edge"),
                 "developments": sum(1 for d in developments if d["unit"] == name),
                 "developments_passing_all_checks": sum(1 for d in developments if d["unit"] == name and evaluations[d["id"]]["checks"]["passed"] == evaluations[d["id"]]["checks"]["total"]),
                 "faulty_rejected_by_the_reviewer": count(bad, review="reject"), "sound_rejected_by_the_reviewer": sum(1 for r in mine if r["label"] == "sound" and r["review"] == "reject")}
        for role in ("same", "other"):
            runs = [ground[d["id"]][role] for d in developments if d["unit"] == name]
            put = sum(1 for g in runs for a in g["attempts"] if a["put"])
            entry[role] = {"faulty_refuted": sum(1 for r in bad if r[role] == "refuted"), "attempts_put": put, "abstained": sum(1 for g in runs if not g["attempts"]),
                           "stakes_won": sum(r["stake"] for r in mine if r[role] == "refuted"), "net": sum(r["stake"] for r in mine if r[role] == "refuted") - put}
        by_unit[name] = entry
    summary["by_unit"] = by_unit
    by_challenger = {}
    for model in MODELS:
        pairs = [(d, role) for d in developments for role in ("same", "other") if ground[d["id"]][role]["model"] == model]
        faulty_seen = [(r, role) for r in faulty for role in ("same", "other") if ground[r["id"]][role]["model"] == model]
        put = sum(1 for d, role in pairs for a in ground[d["id"]][role]["attempts"] if a["put"])
        won = sum(r["stake"] for r in rows for role in ("same", "other") if ground[r["id"]][role]["model"] == model and r[role] == "refuted")
        by_challenger[model] = {"runs": len(pairs), "abstained": sum(1 for d, role in pairs if not ground[d["id"]][role]["attempts"]), "attempts_put": put,
                                "faulty_claims_met": len(faulty_seen), "faulty_claims_refuted": sum(1 for r, role in faulty_seen if r[role] == "refuted"),
                                "stakes_won": won, "net": won - put}
    summary["by_model_of_the_challenger"] = by_challenger
    summary["refuted_by_either_challenger"] = sum(1 for r in faulty if r["same"] == "refuted" or r["other"] == "refuted")
    summary["refuted_by_both_challengers"] = sum(1 for r in faulty if r["same"] == "refuted" and r["other"] == "refuted")

    # the ground itself
    summary["ground"] = {
        "runs": 2 * len(developments), "sealed": sum(1 for d in developments for role in ("same", "other") if ground[d["id"]][role]["sealed"]),
        "journal_entries": sum(ground[d["id"]][role]["journal"] for d in developments for role in ("same", "other")),
    }
    return summary


def show(summary) -> None:
    print(f"\n===== contract: {summary['contract']} =====")
    for name, u in summary["units"].items():
        print(f"\n{name}: reference {u['reference_lines']} lines, {u['checks']} checks; genome words {u['genome_words']}; characters {u['genome_characters']}")
        print(f"   sequenced parts: words {u['sequenced_parts_words']}, characters {u['sequenced_parts_characters']}, exemplars {u['sequenced_exemplars']}")
        for cell, t in u["regeneration"].items():
            print(f"   {cell:24s} checks {t['checks_passed']}  all sound {t['all_functions_sound']}/{t['developments']}  functions sound {t['sound']}/{t['functions']} edge {t['edge']} defective {t['defective']} constants {t['constants_differing']} lines {t['lines']}")
    print("\nfunctions:", summary["functions"])
    for r in summary["faulty_functions"]:
        print("   ", r)
    for part in ("M2_passed_by_the_checks", "M3_challengers", "M4_reviewer", "M5_stakes", "M6_what_reaches_a_person", "ground"):
        print(f"\n{part}: {json.dumps(summary[part], indent=1)}")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    result = {"corrected": analyse("corrected"), "planned": analyse("planned")}
    (OUT / "a-summary.json").write_text(json.dumps(result, indent=1), encoding="utf-8")
    show(result["corrected"])
    planned = result["planned"]
    print("\n===== planned contract, where it differs =====")
    print("functions:", planned["functions"])
    print("M6:", json.dumps(planned["M6_what_reaches_a_person"]))
    print("M3 same:", {k: planned["M3_challengers"]["same"][k] for k in ("faulty_claims", "faulty_claims_refuted")}, "other:", {k: planned["M3_challengers"]["other"][k] for k in ("faulty_claims", "faulty_claims_refuted")})
    print("M4:", {k: planned["M4_reviewer"][k] for k in ("faulty", "faulty_rejected", "sound", "sound_rejected")})
