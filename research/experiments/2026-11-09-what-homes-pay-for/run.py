"""The what-homes-pay-for study's runs: the workbench's demand engine on the data library's case brooklyn-homes, three seeded samples of
2,000 homes at 8 nearest neighbours, and the first sample at 4 and 12; and the plan of a stated-choice survey about a street.

Writes results/hedonic-<seed>-k<k>.json and results/survey.json (committed: estimates and residuals by place, no ways).
Run from the repository's root, after `python data/products/home_prices.py` and `python applications/urban-science-workbench/engine.py build demand`:
    python research/experiments/2026-11-09-what-homes-pay-for/run.py
"""

import json
import pathlib
import sys
import time

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "applications" / "urban-science-workbench"))
import engine  # noqa: E402

RUNS = [(1, 8), (2, 8), (3, 8), (1, 4), (1, 12)]
SURVEY = {"op": "survey", "price": 4, "sets": 12, "alternatives": 3, "half_width": 5,
          "attributes": [{"name": "a protected lane", "levels": 2}, {"name": "street trees", "levels": 3}, {"name": "minutes more on the way", "values": [0, 5, 10]},
                         {"name": "a levy, dollars a month", "values": [0, 10, 25]}],
          "prior": [0.5, 0.3, 0.1, -0.06, -0.05]}


def main():
    (HERE / "results").mkdir(exist_ok=True)
    w = engine.Worker("demand")
    try:
        for seed, k in RUNS:
            started = time.time()
            r = w.ask({"op": "hedonic", "case": "brooklyn-homes", "sample": 2000, "seed": seed, "neighbours": k})
            r = r.get("result", r)
            json.dump(r, open(HERE / "results" / f"hedonic-{seed}-k{k}.json", "w", encoding="utf-8"), separators=(",", ":"))
            print(f"seed {seed}, k {k}: {r['chosen']}, rho {r['lag']['rho']:.3f}, {time.time() - started:.0f} s")
        r = w.ask(SURVEY)
        r = r.get("result", r)
        r["request"] = SURVEY
        json.dump(r, open(HERE / "results" / "survey.json", "w", encoding="utf-8"), indent=1)
        print("survey: respondents for ±$5 a month:", r["respondents"])
    finally:
        w.stop()


if __name__ == "__main__":
    main()
