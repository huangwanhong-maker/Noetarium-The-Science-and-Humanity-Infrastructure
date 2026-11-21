"""Studies A2 and A3: regeneration of three units from three genomes, and the warranted change on the ground.

Stages, each keeping its results before the next reads them (PLAN.md, Sections 2.3 and 2.4):

    python s2_units.py genomes    the three genomes of each unit; the sequenced one within enforced bounds
    python s2_units.py develop    seven developments of each genome, each with a stake on every function
    python s2_units.py evaluate   the repository's checks and the hidden differential tests on every development
    python s2_units.py contest    a reviewer of the producer's model, and a challenger of each model
    python s2_units.py ground     every contest carried on the ground, with energy conserved and the journal sealed

A unit name after the stage restricts it to that unit. The key of the model comes from the environment (../prototypes/ask.py).
"""
from __future__ import annotations

import json
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "prototypes"))

import harness_js as H                                    # noqa: E402
from ask import CAREFUL, CHEAP, ask, spent                # noqa: E402
from ground import Ground, Refused                        # noqa: E402
from units import UNITS, hidden_inputs, identity, in_domain, repository_texts, source, note, neutral, words  # noqa: E402

OUT = HERE / "results"
MODELS = {"cheap": CHEAP, "careful": CAREFUL}
BOUNDS = {"identity": 120, "laws": 220, "development": 80}
EXEMPLARS = 12
PLAN = [("cheap", 5), ("careful", 2)]
GENOMES = ["identity", "repository", "sequenced"]


def load(name, default=None):
    path = OUT / name
    return json.loads(path.read_text(encoding="utf-8")) if path.exists() else default


def save(name, data) -> None:
    OUT.mkdir(exist_ok=True)
    (OUT / name).write_text(json.dumps(data, indent=1, ensure_ascii=False), encoding="utf-8")


# ---- genomes ------------------------------------------------------------------------------------------------

SEQUENCER = (
    "You write the genome of a unit of software: a short text from which a developer who never sees the unit's body "
    "can write the body again so that the unit's hidden checks pass. You are given the body and the unit's documents. "
    "Write what a developer could not guess: the conventions, constants, formulas with their coefficients, units, ranges, "
    "boundary cases and returned shapes that the body fixes. Stay inside the bounds; a genome beyond a bound is refused."
)
SEQUENCE_SCHEMA = {
    "type": "object",
    "properties": {
        "identity": {"type": "string"},
        "laws": {"type": "string"},
        "exemplar_calls": {"type": "array", "items": {"type": "object", "properties": {
            "function": {"type": "string"}, "args_json": {"type": "string"}}, "required": ["function", "args_json"], "additionalProperties": False}},
        "development": {"type": "string"},
    },
    "required": ["identity", "laws", "exemplar_calls", "development"], "additionalProperties": False,
}


def sequence(unit, base) -> dict:
    prompt = (
        f"The unit's interface and what it may import:\n\n{identity(unit)}\n"
        f"The unit's body:\n\n```js\n{neutral(source(unit))}```\n\n"
        + (f"The unit's theory note:\n\n{note(unit)}\n\n" if unit["note"] else "")
        + f"Write the genome in four parts.\n"
          f"- identity: at most {BOUNDS['identity']} words, what the unit is for and how its interface is to be read.\n"
          f"- laws: at most {BOUNDS['laws']} words, what must hold of the body. Formulas may be written compactly with their coefficients.\n"
          f"- exemplar_calls: at most {EXEMPLARS} calls, chosen for the rules that are easiest to get wrong. Give each as the function's name "
          f"and a JSON array of its arguments. The harness computes their results from the body; do not compute them yourself.\n"
          f"- development: at most {BOUNDS['development']} words, how the body is produced: methods, sources, order of work.\n"
          f"Words are counted as the parts separated by spaces."
    )
    attempts, extra = [], ""
    for attempt in range(3):
        answer = ask(SEQUENCER, prompt + extra, SEQUENCE_SCHEMA, model=CAREFUL, effort="low", max_tokens=6000)
        counts = {part: words(answer[part]) for part in BOUNDS}
        over = {part: n for part, n in counts.items() if n > BOUNDS[part]}
        many = len(answer["exemplar_calls"]) > EXEMPLARS
        attempts.append({"words": counts, "exemplar_calls": len(answer["exemplar_calls"]), "refused": bool(over) or many})
        if not over and not many:
            break
        extra = "\n\nYour last genome was refused: " + "; ".join(
            [f"{part} had {n} words against a bound of {BOUNDS[part]}" for part, n in over.items()]
            + ([f"{len(answer['exemplar_calls'])} exemplar calls against a bound of {EXEMPLARS}"] if many else [])) + ". Write it again within the bounds."
    else:
        raise RuntimeError(f"no genome within the bounds after three attempts: {attempts}")
    calls, dropped = [], []
    for item in answer["exemplar_calls"]:
        try:
            args = json.loads(item["args_json"])
        except json.JSONDecodeError:
            dropped.append({**item, "why": "the arguments are not JSON"})
            continue
        why = "no such function" if item["function"] not in unit["functions"] else (None if isinstance(args, list) else "the arguments are not an array") or in_domain(unit["name"], item["function"], args)
        if why:
            dropped.append({**item, "why": why})
        else:
            calls.append({"fn": item["function"], "args": args})
    values = H.reference_values(base, unit, calls)
    lines = [f"{c['fn']}({', '.join(json.dumps(a) for a in c['args'])}) -> {json.dumps(v['value'])}" for c, v in zip(calls, values) if v["ok"]]
    text = (identity(unit) + "\nIdentity.\n" + answer["identity"].strip() + "\n\nLaws.\n" + answer["laws"].strip()
            + "\n\nExemplars, computed from the accepted body.\n" + "\n".join(lines) + "\n\nDevelopment.\n" + answer["development"].strip() + "\n")
    return {"text": text, "parts": {part: answer[part] for part in BOUNDS}, "exemplars": lines, "dropped_exemplars": dropped, "attempts": attempts}


def stage_genomes(names) -> None:
    genomes = load("a2-genomes.json", {})
    for name in names:
        unit = UNITS[name]
        base = H.sandbox(unit)
        if name in genomes and "sequenced" in genomes[name]:
            print(name, "genomes kept")
            continue
        sequenced = sequence(unit, base)
        genomes[name] = {
            "identity": {"text": identity(unit)},
            "repository": {"text": repository_texts(unit)},
            "sequenced": sequenced,
        }
        for kind in GENOMES:
            genomes[name][kind]["words"] = words(genomes[name][kind]["text"])
        save("a2-genomes.json", genomes)
        print(name, {kind: genomes[name][kind]["words"] for kind in GENOMES}, "attempts", sequenced["attempts"], "dropped", len(sequenced["dropped_exemplars"]))
    print(spent())


# ---- developments -------------------------------------------------------------------------------------------

DEVELOPER = (
    "You are a lineage in a system that develops units of software from their genomes. A genome is a short text from which "
    "a unit's body can be written. You are given the genome of one unit and nothing else: you never see the unit's existing body "
    "or its checks. Write the unit's body as one ES module in JavaScript. Export exactly the names that the genome lists, with "
    "those signatures and returned shapes. Import nothing except what the genome allows. Write pure functions: no input or "
    "output, no timers, no randomness, no access to the environment. Then warrant your work with a stake on each function."
)
WARRANTY = (
    "Warranty. For each exported function you may warrant this claim: on its documented domain the function agrees with the accepted "
    "implementation of the unit, which a harness holds and you cannot see, up to rounding, and up to the documented accuracy where "
    "the genome gives only an accuracy. You hold 100 units of energy, and writing the module costs 0.5. A stake is a whole number "
    "from 0 to 20 on each function; a stake of 0 means that you do not warrant that function. Challengers will put inputs to your "
    "functions and the harness decides each one. A claim refuted on any input loses its stake to the challenger. The claims that "
    "stand share a bounty of 60 in proportion to their stakes, and their stakes return to you. Stake by your confidence in each function."
)
DEVELOP_SCHEMA = {
    "type": "object",
    "properties": {
        "source": {"type": "string"},
        "stakes": {"type": "array", "items": {"type": "object", "properties": {
            "function": {"type": "string"}, "stake": {"type": "integer"}}, "required": ["function", "stake"], "additionalProperties": False}},
        "left_open": {"type": "string"},
    },
    "required": ["source", "stakes", "left_open"], "additionalProperties": False,
}


def develop(unit, genome_text: str, model: str) -> dict:
    prompt = (f"The genome of the unit:\n\n{genome_text}\n{WARRANTY}\n\n"
              f"Return the module's whole source, a stake for each of these functions ({', '.join(unit['functions'])}), and in at most "
              f"40 words what the genome left open that you had to decide yourself.")
    answer = ask(DEVELOPER, prompt, DEVELOP_SCHEMA, model=MODELS[model], effort="low", max_tokens=8000)
    stakes = {}
    for item in answer["stakes"]:
        if item["function"] in unit["functions"]:
            stakes[item["function"]] = max(0, min(20, int(item["stake"])))
    body = answer["source"].strip()
    if body.startswith("```"):                                  # a body handed over inside a fence is taken out of it
        body = body.split("\n", 1)[1] if "\n" in body else ""
        body = body.rsplit("```", 1)[0]
    return {"source": body.strip() + "\n", "stakes": {fn: stakes.get(fn, 0) for fn in unit["functions"]}, "left_open": answer["left_open"][:400]}


def stage_develop(names) -> None:
    genomes = load("a2-genomes.json")
    developments = load("a2-developments.json", [])
    have = {d["id"] for d in developments}
    jobs = []
    for name in names:
        for kind in GENOMES:
            for model, count in PLAN:
                for k in range(count):
                    key = f"{name}-{kind}-{model}-{k}"
                    if key not in have:
                        jobs.append((key, name, kind, model, k))

    def run(job):
        key, name, kind, model, k = job
        try:
            made = develop(UNITS[name], genomes[name][kind]["text"], model)
            return {"id": key, "unit": name, "genome": kind, "model": model, "k": k, **made}
        except Exception as error:                              # a development that could not be had is a result too
            return {"id": key, "unit": name, "genome": kind, "model": model, "k": k, "source": "", "stakes": {}, "left_open": "", "failed": str(error)[:300]}

    with ThreadPoolExecutor(max_workers=4) as pool:
        for row in pool.map(run, jobs):
            developments.append(row)
            save("a2-developments.json", developments)
            print(row["id"], "failed: " + row["failed"] if row.get("failed") else f"{len(row['source'].splitlines())} lines, stakes {sum(row['stakes'].values())}")
    print(spent())


# ---- evaluation ---------------------------------------------------------------------------------------------

def stage_evaluate(names) -> None:
    developments = load("a2-developments.json", [])
    evaluations = load("a2-evaluations.json", {})
    for name in names:
        unit = UNITS[name]
        base = H.sandbox(unit)
        cases = hidden_inputs(name)
        for d in developments:
            if d["unit"] != name or d["id"] in evaluations:
                continue
            refused = H.screen(unit, d["source"])
            if refused:
                evaluations[d["id"]] = {"screen": refused, "loads": False, "checks": {"passed": 0, "total": unit["checks"], "crashed": "refused by the screen", "failed": []},
                                        "truth": None}
            else:
                verdict = H.checks(base, unit, d["source"])
                outcome = H.differential(base, unit, d["source"], cases, "corrected")
                planned = H.differential(base, unit, d["source"], cases, "planned")
                evaluations[d["id"]] = {"screen": None, "loads": outcome["load"] is None, "checks": {k: verdict[k] for k in ("passed", "total", "crashed", "failed")},
                                        "truth": H.truth(unit, cases, outcome), "truth_planned": H.truth(unit, cases, planned),
                                        "lines": len(d["source"].splitlines()), "characters": len(d["source"])}
            e = evaluations[d["id"]]
            labels = [f["label"] for f in e["truth"]["functions"].values()] if e["truth"] else []
            print(d["id"], f"checks {e['checks']['passed']}/{e['checks']['total']}", "loads" if e["loads"] else f"does not load ({e['screen'] or e['truth']['load']})",
                  f"sound {labels.count('sound')} edge {labels.count('edge')} defective {labels.count('defective')}")
            save("a2-evaluations.json", evaluations)


# ---- the contest: a reviewer and two challengers ------------------------------------------------------------

REVIEWER = (
    "You review a unit of software that was written from a genome. You have the genome and the module, and no harness: you "
    "cannot run anything. For each exported function, say whether you accept it as agreeing with the unit that the genome "
    "describes on its documented domain, or reject it, with the reason in one sentence."
)
REVIEW_SCHEMA = {
    "type": "object",
    "properties": {"verdicts": {"type": "array", "items": {"type": "object", "properties": {
        "function": {"type": "string"}, "verdict": {"type": "string", "enum": ["accept", "reject"]}, "reason": {"type": "string"}},
        "required": ["function", "verdict", "reason"], "additionalProperties": False}}},
    "required": ["verdicts"], "additionalProperties": False,
}
CHALLENGER = (
    "You are a challenger in a system where producers warrant software with stakes. You read a genome, a module written from it "
    "and the stake on each function. You may put inputs to the functions. For each input a harness, which you cannot see, compares "
    "the module's output with the accepted implementation of the unit; where they disagree beyond rounding, the claim on that "
    "function is refuted and you collect its stake. Each attempt costs you 1 unit of energy whether or not it succeeds, and you hold 30. "
    "An attempt on a function whose stake is 0 can win nothing. Inputs must lie in the documented domains. You may make no attempt "
    "at all, and at most 12. Choose the inputs most likely to expose a difference: conventions, boundaries, units, ranges and signs."
)
CHALLENGE_SCHEMA = {
    "type": "object",
    "properties": {
        "attempts": {"type": "array", "items": {"type": "object", "properties": {
            "function": {"type": "string"}, "args_json": {"type": "string"}}, "required": ["function", "args_json"], "additionalProperties": False}},
        "reasoning": {"type": "string"},
    },
    "required": ["attempts", "reasoning"], "additionalProperties": False,
}


def review(unit, genome_text, d) -> dict:
    prompt = f"The genome:\n\n{genome_text}\nThe module:\n\n```js\n{d['source']}\n```\n\nGive a verdict for each of: {', '.join(unit['functions'])}."
    answer = ask(REVIEWER, prompt, REVIEW_SCHEMA, model=MODELS[d["model"]], effort="low", max_tokens=4000)
    verdicts = {v["function"]: {"verdict": v["verdict"], "reason": v["reason"][:240]} for v in answer["verdicts"] if v["function"] in unit["functions"]}
    return {"model": d["model"], "verdicts": verdicts}


def challenge(unit, genome_text, d, model: str) -> dict:
    domains = "\n".join(f"- {fn}: {spec['domain']}" for fn, spec in unit["functions"].items())
    stakes = ", ".join(f"{fn} {stake}" for fn, stake in d["stakes"].items())
    prompt = (f"The genome:\n\n{genome_text}\nThe module:\n\n```js\n{d['source']}\n```\n\nThe stakes: {stakes}.\n\n"
              f"The documented domains of the inputs:\n{domains}\n\n"
              f"Give your attempts, each as the function's name and a JSON array of its arguments, and your reasoning in at most 60 words.")
    answer = ask(CHALLENGER, prompt, CHALLENGE_SCHEMA, model=MODELS[model], effort="low", max_tokens=4000)
    return {"model": model, "attempts": answer["attempts"][:12], "offered": len(answer["attempts"]), "reasoning": answer["reasoning"][:500]}


def stage_contest(names) -> None:
    genomes = load("a2-genomes.json")
    developments = load("a2-developments.json", [])
    evaluations = load("a2-evaluations.json", {})
    contests = load("a3-contests.json", {})
    jobs = []
    for d in developments:
        if d["unit"] not in names or not evaluations[d["id"]]["loads"]:
            continue
        other = "careful" if d["model"] == "cheap" else "cheap"
        for role, model in (("reviewer", d["model"]), ("same", d["model"]), ("other", other)):
            if role not in contests.get(d["id"], {}):
                jobs.append((d, role, model))

    def run(job):
        d, role, model = job
        unit, text = UNITS[d["unit"]], genomes[d["unit"]][d["genome"]]["text"]
        try:
            return d["id"], role, review(unit, text, d) if role == "reviewer" else challenge(unit, text, d, model)
        except Exception as error:
            return d["id"], role, {"model": model, "failed": str(error)[:300], "verdicts": {}, "attempts": [], "offered": 0, "reasoning": ""}

    with ThreadPoolExecutor(max_workers=4) as pool:
        for key, role, result in pool.map(run, jobs):
            contests.setdefault(key, {})[role] = result
            save("a3-contests.json", contests)
            print(key, role, result.get("failed") or (f"{sum(1 for v in result['verdicts'].values() if v['verdict'] == 'reject')} rejected" if role == "reviewer" else f"{len(result['attempts'])} attempts"))
    print(spent())


# ---- the ground ---------------------------------------------------------------------------------------------

def stage_ground(names, contract: str = "corrected") -> None:
    developments = load("a2-developments.json", [])
    evaluations = load("a2-evaluations.json", {})
    contests = load("a3-contests.json", {})
    target = "a3-ground.json" if contract == "corrected" else "a3-ground-planned.json"
    record = load(target, {})
    for name in names:
        unit = UNITS[name]
        base = H.sandbox(unit)
        for d in developments:
            if d["unit"] != name or d["id"] not in contests or d["id"] in record:
                continue
            entry = {}
            for role in ("same", "other"):
                attempts = []
                for index, raw in enumerate(contests[d["id"]][role]["attempts"]):
                    item = {"fn": raw["function"], "raw": raw["args_json"][:300], "valid": None}
                    try:
                        args = json.loads(raw["args_json"])
                        item["valid"] = "no such function" if raw["function"] not in unit["functions"] else ("the arguments are not an array" if not isinstance(args, list) else in_domain(name, raw["function"], args))
                        item["args"] = args
                    except json.JSONDecodeError:
                        item["valid"] = "the arguments are not JSON"
                    attempts.append(item)
                cases = [{"id": i, "fn": a["fn"], "args": a["args"], "tier": "challenge"} for i, a in enumerate(attempts) if a["valid"] is None]
                outcome = {c["id"]: c for c in H.differential(base, unit, d["source"], cases, contract)["cases"]} if cases else {}

                def decide(artefact, terms, counterexample, outcome=outcome):
                    result = outcome[counterexample["index"]]
                    if result["skipped"]:
                        return False, "the input is outside the documented domain"
                    return (not result["ok"]), (result["why"] or "agrees")

                site = {"name": name, "kinds": {"agrees": {"object": "the accepted implementation", "decide": decide}},
                        "facts": {"checks": lambda key: evaluations[d["id"]]["checks"]["passed"]}, "reserved": ["need"]}
                g = Ground(site)
                g.admit("person", person=True)
                g.admit("producer")
                g.admit("challenger")
                g.emit("person", 200.0)
                g.transfer("person", "producer", 100.0)
                g.transfer("person", "challenger", 30.0)
                need = g.need("person", f"the unit {name}, developed from its genome", 60.0, ["agrees"])
                artefact = g.write("producer", "module", d["source"])
                claims, refused = {}, []
                for fn, stake in d["stakes"].items():
                    if stake <= 0:
                        continue
                    try:
                        claims[fn] = g.warrant("producer", need, artefact, "agrees", {"function": fn}, float(stake))
                    except Refused as why:
                        refused.append({"function": fn, "stake": stake, "why": str(why)})
                log = []
                for index, a in enumerate(attempts):
                    step = {"fn": a["fn"], "args": a.get("args"), "valid": a["valid"], "put": False, "refuted": False, "why": None}
                    if a["valid"] is None and a["fn"] in claims and g.claims[claims[a["fn"]]]["standing"] == "standing":
                        try:
                            step["refuted"] = g.challenge("challenger", claims[a["fn"]], {"index": index, "fn": a["fn"], "args": a["args"]})
                            step["put"] = True
                            step["why"] = outcome[index]["why"]
                        except Refused as why:
                            step["why"] = str(why)
                    elif a["valid"] is None and a["fn"] not in claims:
                        step["why"] = "no claim stands on this function"
                    elif a["valid"] is None:
                        step["why"] = "the claim was already refuted"
                    log.append(step)
                passed = g.fact("person", "checks", artefact)
                g.settle(need)
                entry[role] = {
                    "model": contests[d["id"]][role]["model"], "attempts": log, "refused_claims": refused, "checks": passed,
                    "claims": {fn: g.claims[key]["standing"] for fn, key in claims.items()},
                    "accounts": {k: round(v, 6) for k, v in g.accounts.items()}, "dissipated": round(g.dissipated, 6),
                    "journal": len(g.journal), "sealed": g.sealed(),
                }
            record[d["id"]] = entry
            save(target, record)
            print(d["id"], {role: sum(1 for s in entry[role]["claims"].values() if s == "refuted") for role in entry}, "refuted of", len(entry["same"]["claims"]), "claims")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    stage = sys.argv[1]
    units = sys.argv[2:] or list(UNITS)
    if stage == "ground-planned":
        stage_ground(units, "planned")
    else:
        {"genomes": stage_genomes, "develop": stage_develop, "evaluate": stage_evaluate, "contest": stage_contest, "ground": stage_ground}[stage](units)
