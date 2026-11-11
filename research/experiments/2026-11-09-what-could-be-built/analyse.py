"""The what-could-be-built study's figures and summary, from run.py's results.

- design-05-front: the front of Collins Street's block under a street wall and towers, every design's six measures against the block
  as it stands, coloured by its passage.
- design-06-blocks: three blocks under three rules, floor against winter sun on the streets and against the heat a winter day needs,
  each design a dot and the block as built a square.

Writes results/summary.json and the figures to assets/gallery/.
Run from the repository's root: python research/experiments/2026-11-09-what-could-be-built/analyse.py
"""

import json
import pathlib
import sys

import numpy as np

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "style"))
from search_style import CYCLE, INK, MUTED, figure, save, source, title  # noqa: E402

RESULTS = HERE / "results"
MEASURES = [("floor", "floor", 1), ("streetSun", "winter sun\non the streets", 1), ("facadeSun", "winter sun on the\nneighbours' faces", 1),
            ("heat", "heat a winter\nday needs (less)", -1), ("configuration", "integration of\nthe streets", 1), ("access", "floor within\nfive minutes", 1)]
RULE_NAME = {"wall": "a street wall and towers", "mid": "mid-rise", "apart": "towers apart"}
PASSAGE = ["no passage", "a passage across the block", "a passage along the block"]
RULE_COLOUR = {"wall": CYCLE[0], "mid": CYCLE[1], "apart": CYCLE[2]}
PASSAGE_COLOUR = [CYCLE[3], CYCLE[4], CYCLE[2]]


def best(designs, key, sign, where=lambda d: True):
    pool = [d for d in designs if where(d)]
    return max(pool, key=lambda d: sign * d[key]) if pool else None


def summary(runs):
    out = []
    for r in runs:
        if "error" in r:
            out.append({k: r[k] for k in ("block", "rules", "keep", "seed", "error")})
            continue
        B, D = r["asBuilt"], r["designs"]
        row = {k: r[k] for k in ("block", "rules", "keep", "seed", "lots", "lotArea", "evaluations", "complying", "pieces")}
        row["asBuilt"] = {k: B[k] for k, _, _ in MEASURES} | {"plotRatio": B["plotRatio"], "height": B["height"]}
        row["front"] = len(D)
        if not D:
            row["why"] = ("the plot ratio's floor below the floor asked" if 0 <= r["plotRatioFloor"] < r["leastFloor"] else "no design keeps the floor asked")
            out.append(row)
            continue
        row["extremes"] = {k: (max if s > 0 else min)(d[k] for d in D) for k, _, s in MEASURES}
        row["relative"] = {k: row["extremes"][k] / B[k] if B[k] else None for k, _, _ in MEASURES}
        # The most winter sun on the streets among designs keeping at least 80% of the floor standing now.
        keep = best(D, "streetSun", 1, lambda d: d["floor"] >= 0.8 * B["floor"])
        row["sunAtFourFifths"] = {"floor": keep["floor"], "streetSun": keep["streetSun"], "gain": keep["streetSun"] / B["streetSun"] - 1} if keep else None
        row["heatBelowAsBuilt"] = sum(d["heat"] < B["heat"] for d in D)
        row["passages"] = [sum(d["variant"] == v for d in D) for v in range(3)]
        out.append(row)
    return out


def front_figure(run):
    B, D = run["asBuilt"], run["designs"]
    fig, ax = figure()
    fig.subplots_adjust(left=0.07, right=0.97, top=0.82, bottom=0.17)
    x = np.arange(len(MEASURES))
    rel = np.array([[d[k] / B[k] for k, _, _ in MEASURES] for d in D])
    for v in (0, 1, 2):
        rows = [i for i, d in enumerate(D) if d["variant"] == v]
        for i in rows:
            ax.plot(x, rel[i], color=PASSAGE_COLOUR[v], alpha=0.38, lw=1.1)
        ax.plot([], [], color=PASSAGE_COLOUR[v], lw=2.4, label=f"{PASSAGE[v]} ({len(rows)})")
    ax.plot(x, np.ones(len(x)), color=INK, lw=2.2, ls=(0, (5, 3)), label="the block as it stands")
    ax.set_xticks(x, [name for _, name, _ in MEASURES], fontsize=12)
    for i, (k, _, s) in enumerate(MEASURES):
        ax.annotate(("more" if s > 0 else "less") + " is better", (x[i], rel[:, i].max()), xytext=(0, 10), textcoords="offset points", ha="center", fontsize=9.5, color=MUTED)
    ax.set_ylabel("against the block as it stands (= 1)")
    ax.set_xlim(-0.3, len(x) - 0.7)
    ax.legend(loc="lower left", frameon=False, fontsize=11)
    title(fig, f"What could be built: {len(D)} designs no other betters, Collins Street's block",
          "Under a street wall to 24 m with towers set back 10 m above it, to 100 m, at a plot ratio of 12; every design keeps half the floor standing now")
    source(fig, f"The API's composition (science/design.hpp, ADR 0089) on the data library's city model of Melbourne (OpenStreetMap, ODbL): {run['lots']} lots, NSGA-III over "
                f"{run['evaluations']:,} designs;
the Sun at 10:00–14:00 on 21 June (the astronomy domain); heat a balance of the envelope at 8 °C by day, 4 °C by night.")
    return save(fig, "design-05-front")


def blocks_figure(runs, blocks):
    fig, axes = figure(2, 3)
    fig.subplots_adjust(left=0.07, right=0.98, top=0.83, bottom=0.12, hspace=0.32, wspace=0.22)
    for c, b in enumerate(blocks):
        for row, (key, label) in enumerate([("streetSun", "winter sun on the streets (thousand m²·h)"), ("heat", "heat a winter day needs (Wh per m² of floor)")]):
            ax = axes[row][c]
            built = None
            for r in runs:
                if r["block"] != b or r["keep"] != 0.1 or r["seed"] != 1 or "error" in r:
                    continue
                built = r["asBuilt"]
                D = r["designs"]
                if D:
                    ax.scatter([d["floor"] / 1000 for d in D], [d[key] / (1000 if key == "streetSun" else 1) for d in D], s=16, color=RULE_COLOUR[r["rules"]], alpha=0.7,
                               label=RULE_NAME[r["rules"]], edgecolors="none")
            if built:
                ax.scatter([built["floor"] / 1000], [built[key] / (1000 if key == "streetSun" else 1)], marker="s", s=70, facecolors="none", edgecolors=INK, linewidths=1.6, label="as it stands")
            ax.set_xlabel("floor (thousand m²)")
            if c == 0:
                ax.set_ylabel(label)
            if row == 0:
                ax.set_title(blocks[b]["name"], fontsize=12.5, color=INK, loc="left")
            if row == 0 and c == 0:
                ax.legend(frameon=False, fontsize=10, loc="best")
    title(fig, "Three blocks of Melbourne under three rules", "Every design keeps a tenth of the floor standing now; each dot a design no other betters, the square the block as it stands")
    source(fig, "The API's composition (science/design.hpp, ADR 0089) on the data library's city model of Melbourne (OpenStreetMap, ODbL), NSGA-III for 60 generations of 128; "
                "towers apart finds nothing in Carlton, whose row houses its 6 m setbacks swallow.")
    return save(fig, "design-06-blocks")


def main():
    data = json.load(open(RESULTS / "runs.json", encoding="utf-8"))
    runs = data["runs"]
    rows = summary(runs)
    json.dump({"generated_by": "analyse.py", "runs": rows}, open(RESULTS / "summary.json", "w", encoding="utf-8"), indent=1)
    first = next(r for r in runs if r["block"] == "collins" and r["rules"] == "wall" and r["keep"] == 0.5 and r["seed"] == 1)
    print(front_figure(first))
    print(blocks_figure(runs, data["blocks"]))
    for r in rows:
        tag = f"{r['block']}-{r['rules']}-{r['keep']}-{r['seed']}"
        if not r.get("front"):
            print(tag, "nothing:", r.get("why", r.get("error")))
            continue
        rel = {k: round(v, 3) if v else v for k, v in r["relative"].items()}
        print(tag, r["front"], "as built", {k: round(v, 3) for k, v in r["asBuilt"].items()}, "best/as built", rel, "4/5 floor:", r["sunAtFourFifths"], "heat<built", r["heatBelowAsBuilt"], "passages", r["passages"])


if __name__ == "__main__":
    main()
