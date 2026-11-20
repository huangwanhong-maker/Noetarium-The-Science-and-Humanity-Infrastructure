"""Prototype 2, continued. Reseeding in a longer chain.

body -> genome -> body -> genome ..., ten generations: three chains with nothing to hold them (one development
a generation), and one chain anchored by the unit's checks (three developments a generation, the one that
passes most is kept). The first run of four generations lost one rule without an anchor; this run measures
the rate, whether a lost rule returns, and what happens to the length of the body.

    python p2b_chain.py            writes results/p2-chain.json
"""
from __future__ import annotations

import json
from concurrent.futures import ThreadPoolExecutor

from ask import CHEAP, spent
from p2_regeneration import BODY, OUT, compose, regenerate, sequence, words

GENERATIONS = 10
PARTS = ("identity", "laws", "exemplars", "development")


def chain(tag: str, tries: int) -> list[dict]:
    body, steps = BODY, []
    for generation in range(1, GENERATIONS + 1):
        try:
            genome = sequence(body, CHEAP)
        except Exception as error:
            steps.append({"generation": generation, "ended": f"no genome: {error}"[:120]})
            break
        text = compose(genome, PARTS)
        with ThreadPoolExecutor(max_workers=tries) as inner:
            bodies = list(inner.map(lambda k: regenerate(text, CHEAP, f"d-{tag}-{generation}-{k}"), range(tries)))
        kept = max(bodies, key=lambda b: b["passed"])
        steps.append({"generation": generation, "genome_words": words(text), "passed": [b["passed"] for b in bodies],
                      "kept": kept["passed"], "lines": kept["lines"], "failed": kept["failed"][:14]})
        if not kept.get("code"):
            steps[-1]["ended"] = "no body"
            break
        body = kept["code"]
    return steps


def main() -> None:
    jobs = [("u0", 1), ("u1", 1), ("u2", 1), ("a0", 3)]
    with ThreadPoolExecutor(max_workers=4) as pool:
        chains = dict(zip((j[0] for j in jobs), pool.map(lambda j: chain(*j), jobs)))
    record = {"generations": GENERATIONS, "unanchored": [chains["u0"], chains["u1"], chains["u2"]], "anchored": chains["a0"], "spent": spent()}
    (OUT / "p2-chain.json").write_text(json.dumps(record, indent=1, ensure_ascii=False), encoding="utf-8")
    for name, steps in chains.items():
        print(name, "kept", [s.get("kept") for s in steps], "| lines", [s.get("lines") for s in steps])
        print("    lost at the end:", steps[-1].get("failed"))
    print(record["spent"])


if __name__ == "__main__":
    main()
