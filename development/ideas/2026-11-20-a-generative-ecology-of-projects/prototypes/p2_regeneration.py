"""Prototype 2. The genome of a unit, and its regeneration.

Question: is there a compact text from which a model can redevelop a unit's body so that the unit's checks,
which the developer never sees, pass again; and which parts of that text carry the unit's particular rules.

    python p2_regeneration.py            runs the whole prototype and writes results/p2.json

Three measurements on the unit in unit/ (120 lines, 42 checks held by the harness):
  A. sequencing: a model writes the genome from the body, in four bounded parts (identity, laws, exemplars,
     development);
  B. regeneration: a developer that has only a genome writes the body again; four genomes are compared
     (identity; identity and laws; identity and exemplars; all parts), several developments each;
  C. reseeding in a chain: body -> genome -> body -> genome ..., four generations, once with nothing to hold
     it (each generation develops once) and once anchored by the checks (each generation develops three
     bodies and keeps the one that passes most).
"""
from __future__ import annotations

import json
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from ask import CAREFUL, CHEAP, ask, spent

HERE = Path(__file__).resolve().parent
BODY = (HERE / "unit" / "refcode.py").read_text(encoding="utf-8")
CHECKS = HERE / "unit" / "checks.py"
OUT = HERE / "results"
OUT.mkdir(exist_ok=True)
WORK = Path(tempfile.mkdtemp(prefix="ecology-p2-"))

GENOME = {"type": "object", "additionalProperties": False, "required": ["identity", "laws", "exemplars", "development"],
          "properties": {k: {"type": "string"} for k in ("identity", "laws", "exemplars", "development")}}
CODE = {"type": "object", "additionalProperties": False, "required": ["code"], "properties": {"code": {"type": "string"}}}

SEQ_SYSTEM = ("You write the genome of a unit of software: a compact text from which another developer, who will never see "
              "the source, can rebuild the unit so that it behaves the same. State what is particular to this unit and leave "
              "out what any competent developer would do anyway. Do not copy the source.")
DEV_SYSTEM = ("You develop the body of a unit of software from its genome. You have only the genome. Write the complete Python "
              "module, standard library only, with no access to files, the network, the environment or the interpreter's "
              "internals. Follow the genome exactly where it speaks, and use ordinary good practice where it is silent.")


def sequence(body: str, model: str) -> dict:
    prompt = f"""The source of the unit `refcode` follows.

<source>
{body}
</source>

Write its genome in four parts, each within its limit of words:
- identity (at most 110 words): what the unit is for, and its public names with their arguments and what each returns.
- laws (at most 180 words): the rules its behaviour must satisfy, including what it refuses and the order it puts on its objects.
- exemplars (at most 10 lines): worked cases, one a line, as `call -> result`, chosen to show the rules that are easiest to get wrong.
- development (at most 60 words): how its body is best built."""
    return ask(SEQ_SYSTEM, prompt, GENOME, model=model, max_tokens=4000)


def compose(genome: dict, parts: tuple[str, ...]) -> str:
    return "\n\n".join(f"[{part}]\n{genome[part].strip()}" for part in parts)


def develop(genome_text: str, model: str) -> str:
    prompt = f"""The genome of the unit `refcode` follows.

<genome>
{genome_text}
</genome>

Write the complete module `refcode.py`."""
    return ask(DEV_SYSTEM, prompt, CODE, model=model, max_tokens=6000)["code"]


def check(code: str, tag: str) -> dict:
    path = WORK / f"{tag}.py"
    path.write_text(code, encoding="utf-8")
    try:
        done = subprocess.run([sys.executable, "-I", str(CHECKS), str(path)], capture_output=True, text=True, timeout=30, cwd=WORK)
        return json.loads(done.stdout.strip().splitlines()[-1])
    except Exception as error:  # a candidate that hangs or prints nothing passes no check
        return {"passed": 0, "total": 42, "failed": [f"not run: {type(error).__name__}"]}


def words(text: str) -> int:
    return len(text.split())


def regenerate(genome_text: str, model: str, tag: str) -> dict:
    try:
        code = develop(genome_text, model)
    except Exception as error:
        return {"passed": 0, "total": 42, "failed": [f"no body: {error}"[:120]], "lines": 0}
    result = check(code, tag)
    result["lines"] = len(code.splitlines())
    result["code"] = code
    return result


def main() -> None:
    record: dict = {"unit": {"lines": len(BODY.splitlines()), "checks": 42}}
    pool = ThreadPoolExecutor(max_workers=6)

    # A. Sequencing, by the careful model and by the cheap one.
    genomes = dict(zip(("careful", "cheap"), pool.map(lambda m: sequence(BODY, m), (CAREFUL, CHEAP))))
    record["genomes"] = {who: {**g, "words": {k: words(v) for k, v in g.items()}} for who, g in genomes.items()}

    # B. Regeneration from four genomes (the careful model's sequencing), four developments each by the cheap
    #    developer, and two by the careful developer for the smallest and the largest genome.
    conditions = {"identity": ("identity",), "identity+laws": ("identity", "laws"),
                  "identity+exemplars": ("identity", "exemplars"), "all parts": ("identity", "laws", "exemplars", "development")}
    jobs = []
    for name, parts in conditions.items():
        text = compose(genomes["careful"], parts)
        for k in range(4):
            jobs.append((name, CHEAP, text, f"b-{name.replace('+', '-').replace(' ', '-')}-cheap-{k}"))
        if name in ("identity", "all parts"):
            for k in range(2):
                jobs.append((name, CAREFUL, text, f"b-{name.replace(' ', '-')}-careful-{k}"))
    done = list(pool.map(lambda j: regenerate(j[2], j[1], j[3]), jobs))
    table: dict = {}
    for (name, model, text, tag), result in zip(jobs, done):
        row = table.setdefault(f"{name} | {'careful' if model == CAREFUL else 'cheap'} developer", {"genome_words": words(text), "passed": [], "failed": {}})
        row["passed"].append(result["passed"])
        for f in result["failed"]:
            row["failed"][f] = row["failed"].get(f, 0) + 1
    record["regeneration"] = table
    best = max(zip(jobs, done), key=lambda jd: jd[1]["passed"])
    (OUT / "p2-sample-body.py").write_text(best[1].get("code", ""), encoding="utf-8")

    # C. Reseeding in a chain, with nothing to hold it and anchored by the checks.
    def chain(anchored: bool) -> list[dict]:
        body, steps = BODY, []
        for generation in range(1, 5):
            genome = sequence(body, CHEAP)
            text = compose(genome, ("identity", "laws", "exemplars", "development"))
            tries = 3 if anchored else 1
            bodies = list(pool.map(lambda k: regenerate(text, CHEAP, f"c-{'a' if anchored else 'u'}-{generation}-{k}"), range(tries)))
            kept = max(bodies, key=lambda b: b["passed"])
            steps.append({"generation": generation, "genome_words": words(text), "passed": [b["passed"] for b in bodies],
                          "kept": kept["passed"], "lines": kept["lines"], "failed": kept["failed"][:12]})
            if not kept.get("code"):
                break
            body = kept["code"]
        return steps

    unanchored, anchored = pool.map(chain, (False, True))
    record["chain"] = {"unanchored": unanchored, "anchored": anchored}
    record["spent"] = spent()
    (OUT / "p2.json").write_text(json.dumps(record, indent=1, ensure_ascii=False), encoding="utf-8")

    print("genome words (careful):", record["genomes"]["careful"]["words"], "| (cheap):", record["genomes"]["cheap"]["words"])
    for name, row in table.items():
        print(f"{name:42s} words {row['genome_words']:4d}  passed {row['passed']} of 42")
    for name in ("unanchored", "anchored"):
        print(name, [(s["generation"], s["passed"], s["kept"]) for s in record["chain"][name]])
    print(record["spent"])


if __name__ == "__main__":
    main()
