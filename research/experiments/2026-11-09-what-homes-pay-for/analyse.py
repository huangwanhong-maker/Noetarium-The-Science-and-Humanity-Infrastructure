"""The what-homes-pay-for study's figures and summary, from run.py's results.

- demand-01-amenities: what the subway, a park, a protected lane and the centre add to a Brooklyn home's price, under least squares,
  the spatial lag model (direct, and total with spillover) and the spatial error model, with 95% intervals (first sample, k = 8).
- demand-02-robustness: the lane's premium and the subway's effect across three samples and three numbers of neighbours.
- demand-03-survey: the respondents a survey about a street needs for each willingness to pay, against the half-width asked.

Writes results/summary.json and the figures to assets/gallery/.
Run from the repository's root: python research/experiments/2026-11-09-what-homes-pay-for/analyse.py
"""

import json
import math
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "style"))
from search_style import CYCLE, INK, MUTED, figure, save, source, title  # noqa: E402

RESULTS = HERE / "results"
AMENITIES = [(7, "the subway, 10% farther", "distance"), (8, "a park, 10% farther", "distance"), (9, "a protected lane within 300 m", "indicator"), (10, "the centre, 10% farther", "distance")]


def as_percent(b, kind):
    return 100 * (b * math.log(1.1) if kind == "distance" else math.expm1(b))


def interval(b, se, kind):
    lo, hi = b - 1.96 * se, b + 1.96 * se
    return as_percent(lo, kind), as_percent(hi, kind)


def main():
    r = json.load(open(RESULTS / "hedonic-1-k8.json", encoding="utf-8"))
    summary = {"chosen": r["chosen"], "homes": r["homes"], "of": r["of"], "moran": r["moran"], "tests": r["tests"], "rho": [r["lag"]["rho"], r["lag"]["rho_se"]],
               "lambda": [r["error"]["lambda"], r["error"]["lambda_se"]], "aic": {"ols": r["ols"]["aic"], "lag": r["lag"]["aic"], "error": r["error"]["aic"]},
               "median_price": r["median_price"], "amenities": {}}
    fig, ax = figure()
    fig.subplots_adjust(top=0.84, bottom=0.12, left=0.24, right=0.97)
    models = [("least squares", r["ols"], CYCLE[8]), ("lag, direct", r["lag"], CYCLE[0]), ("error", r["error"], CYCLE[1])]
    for row, (i, name, kind) in enumerate(AMENITIES):
        y0 = len(AMENITIES) - 1 - row
        entry = {}
        for m, (label, fit, colour) in enumerate(models):
            b, se = fit["beta"][i], fit["se"][i]
            lo, hi = interval(b, se, kind)
            yy = y0 + 0.24 - 0.16 * m
            ax.errorbar(as_percent(b, kind), yy, xerr=[[as_percent(b, kind) - lo], [hi - as_percent(b, kind)]], fmt="o", color=colour, ms=7, lw=1.8, capsize=0,
                        label=label if row == 0 else None)
            entry[label] = [as_percent(b, kind), lo, hi]
        t = as_percent(r["impacts"]["total"][i], kind)
        ax.plot(t, y0 - 0.24, "D", color=CYCLE[0], ms=7, mfc="white", mew=1.8, label="lag, total with spillover" if row == 0 else None)
        entry["lag, total"] = t
        summary["amenities"][name] = entry
    ax.set_yticks(range(len(AMENITIES)), [a[1] for a in AMENITIES][::-1], fontsize=12)
    ax.axvline(0, color=MUTED, lw=0.8)
    ax.set_xlabel("change in a home's price, %")
    ax.legend(frameon=False, loc="upper right", fontsize=11)
    title(fig, "What a Brooklyn home's price pays for, near it", f"{r['homes']:,} of {r['of']:,} one- to three-family homes sold in 2025, at their 8 nearest neighbours; 95% intervals; the tests choose the {r['chosen']} model")
    source(fig, "NYC Department of Finance's sales; MTA Subway Stations; NYC Parks; NYC DOT Bike Routes (NYC Open Data). tyche's spatial regression; urban science's demand.")
    save(fig, "demand-01-amenities")

    fig, axes = figure(1, 2)
    fig.subplots_adjust(top=0.82, bottom=0.14, left=0.07, right=0.98, wspace=0.22)
    runs = [(1, 8), (2, 8), (3, 8), (1, 4), (1, 12)]
    for ax, (i, name, kind) in zip(axes, [AMENITIES[2], AMENITIES[0]]):
        labels = []
        for x, (seed, k) in enumerate(runs):
            q = json.load(open(RESULTS / f"hedonic-{seed}-k{k}.json", encoding="utf-8"))
            labels.append(f"sample {seed}\nk = {k}\n{q['chosen']}")
            for m, (label, key, colour) in enumerate([("least squares", "ols", CYCLE[8]), ("lag, direct", "lag", CYCLE[0]), ("error", "error", CYCLE[1])]):
                b, se = q[key]["beta"][i], q[key]["se"][i]
                lo, hi = interval(b, se, kind)
                ax.errorbar(x + (m - 1) * 0.18, as_percent(b, kind), yerr=[[as_percent(b, kind) - lo], [hi - as_percent(b, kind)]], fmt="o", color=colour, ms=6, lw=1.6,
                            label=label if x == 0 else None)
        ax.axhline(0, color=MUTED, lw=0.8)
        ax.set_xticks(range(len(runs)), labels, fontsize=10)
        ax.set_ylabel("change in a home's price, %")
        ax.set_title(name, loc="left", fontsize=14, color=INK)
        ax.legend(frameon=False, fontsize=11)
    title(fig, "Robust to the sample and the neighbours", "Three seeded samples of 2,000 homes at 8 neighbours, and the first at 4 and 12; 95% intervals")
    source(fig, "The rule's choice under each run is printed beneath it.")
    save(fig, "demand-02-robustness")

    s = json.load(open(RESULTS / "survey.json", encoding="utf-8"))
    fig, ax = figure()
    fig.subplots_adjust(top=0.84, bottom=0.12, left=0.09, right=0.97)
    names = [a["name"] for a in s["request"]["attributes"]]
    cols = ["a protected lane (yes against no)", "many trees (against the mean)", "some trees (against the mean)", "a minute more"]
    hs = [1 + 0.25 * j for j in range(57)]
    for c, label in enumerate(cols):
        se1 = s["wtp_se_one"][c]
        ax.plot(hs, [(1.96 * se1 / h) ** 2 for h in hs], color=CYCLE[c], lw=2, label=f"{label}: ${s['wtp'][c]:.1f} a month at the priors")
    ax.set_yscale("log")
    ax.set_xlabel("half-width of the 95% interval, dollars a month")
    ax.set_ylabel("respondents, each answering every set")
    ax.legend(frameon=False, fontsize=11)
    title(fig, "How many must answer a survey about a street", f"A design of {s['request']['sets']} sets of three ({', '.join(names)}), D-error {s['d_error']:.3f} at the priors; respondents for each willingness to pay")
    source(fig, "peira's coordinate exchange; the count from the design's information, held within 15% of 120 simulations of 200 respondents (urban science's demand checks).")
    save(fig, "demand-03-survey")
    summary["survey"] = {"wtp": s["wtp"], "respondents_5": s["respondents"], "d_error": s["d_error"]}
    json.dump(summary, open(RESULTS / "summary.json", "w", encoding="utf-8"), indent=1)
    print(json.dumps(summary["amenities"], indent=1))


if __name__ == "__main__":
    main()
