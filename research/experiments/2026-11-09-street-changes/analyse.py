"""The street-changes study's figures and summary, from changes.cpp's results (results/<city>-<rings>.json).

- changes-01-identification: what each record can tell: the sensors entering each ring, those counting before their change came, and
  those never reached, under the declared rings.
- changes-02-event-studies: the event study of each ring against the sensors not yet reached (Melbourne under its declared rings;
  New York under 100 and 400 m, the narrowest set its record can estimate), with bands of two bootstrap standard errors.
- changes-03-estimates: every estimate under every set of rings, in the two-way regression and against those not yet reached, with
  their 95% intervals.

Writes results/summary.json (committed) and the figures to assets/gallery/.
Run from the repository's root: python research/experiments/2026-11-09-street-changes/analyse.py
"""

import json
import math
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "style"))
from search_style import CYCLE, HAIRLINE, INK, MUTED, figure, save, source, title  # noqa: E402

import matplotlib.pyplot as plt  # noqa: E402

RESULTS = HERE / "results"
CASES = {
    "melbourne": {"name": "Melbourne: a major building and the walkers near it", "declared": "200-600", "sets": ["200-600", "150-400", "100-300-600"], "event": "200-600", "unit": "years"},
    "newyork": {"name": "New York: a protected lane and the riders on it and near it", "declared": "100-600-1500", "sets": ["100-600-1500", "100-400", "50-300", "200-800"],
                "event": "100-400", "unit": "months"},
}


def load(city, rings):
    return json.load(open(RESULTS / f"{city}-{rings}.json", encoding="utf-8"))


def pct(v):
    return f"{100 * math.expm1(v):+.0f}%"


def main():
    summary = {}
    # 1. What each record can tell.
    fig, axes = figure(1, 2)
    fig.subplots_adjust(top=0.82, bottom=0.12, left=0.07, right=0.98, wspace=0.18)
    for ax, (city, c) in zip(axes, CASES.items()):
        r = load(city, c["declared"])
        R = len(r["bounds"])
        entering = [sum(1 for u in r["units"] if u["ring"] == k) for k in range(R)]
        saw = [sum(1 for u in r["units"] if u["ring"] == k and u["before"] > 0) for k in range(R)]
        never = r["entering"][R]
        labels, inner = [], 0
        for b in r["bounds"]:
            labels.append(f"{inner:.0f}–{b:.0f} m")
            inner = b
        labels.append(f"beyond {inner:.0f} m")
        x = range(R + 1)
        ax.bar(x, entering + [never], color=HAIRLINE, width=0.62, label="sensors entering the ring (never reached, last)")
        ax.bar(range(R), saw, color=CYCLE[0], width=0.62, label="of them, counting before their change came")
        for i, (e, s) in enumerate(zip(entering + [never], saw + [None])):
            ax.text(i, e + 0.4, f"{e}" if s is None else f"{s} of {e}", ha="center", va="bottom", color=INK, fontsize=12)
        ax.set_xticks(list(x), labels)
        ax.set_ylabel("sensors")
        ax.set_title(c["name"], loc="left", fontsize=14, color=INK)
        ax.set_ylim(0, max(entering + [never]) * 1.25)
        ax.legend(frameon=False, loc="upper left", fontsize=11)
        summary[city] = {"declared": r["bounds"], "entering": entering, "counting_before": saw, "never": never, "sensors": len(r["units"]) + never}
    title(fig, "What a record of changes can tell", "Sensors by the ring, along the ways, in which a change first reached them; only those counting before it came can say what it did")
    source(fig, "City of Melbourne's counters and Development Activity Monitor (CC BY 4.0); NYC DOT's bicycle counts and bike routes (NYC Open Data); ways, Overture Maps (ODbL).")
    save(fig, "changes-01-identification")

    # 2. Event studies against those not yet reached.
    fig, axes = figure(1, 2)
    fig.subplots_adjust(top=0.82, bottom=0.12, left=0.07, right=0.98, wspace=0.18)
    for ax, (city, c) in zip(axes, CASES.items()):
        r = load(city, c["event"])
        lim = 6 if c["unit"] == "years" else 36
        inner = 0
        for k, ring in enumerate(r["staggered"]):
            pts = [(e, v, se) for e, v, se in ring["by_exposure"] if abs(e) <= lim and v is not None]
            label = f"{inner:.0f}–{r['bounds'][k]:.0f} m"
            inner = r["bounds"][k]
            if len(pts) < 2:
                continue
            xs = [p[0] for p in pts]
            ys = [p[1] for p in pts]
            lo = [p[1] - 2 * (p[2] or 0) for p in pts]
            hi = [p[1] + 2 * (p[2] or 0) for p in pts]
            ax.fill_between(xs, lo, hi, color=CYCLE[k], alpha=0.14, linewidth=0)
            ax.plot(xs, ys, color=CYCLE[k], linewidth=2, label=f"entering {label} ({sum(1 for u in r['units'] if u['ring'] == k)} sensors)")
        ax.axhline(0, color=MUTED, linewidth=0.8)
        ax.axvline(-0.5, color=MUTED, linewidth=0.8, linestyle=":")
        ax.set_xlabel(f"{c['unit']} from entering the ring")
        ax.set_ylabel("change against those not yet reached (log)")
        ax.set_title(f"{c['name'].split(':')[0]}, rings of {c['event'].replace('-', ', ')} m", loc="left", fontsize=14, color=INK)
        ax.legend(frameon=False, loc="upper left", fontsize=11)
    title(fig, "The change at entering a ring, period by period", "Callaway and Sant'Anna's group-time effects by ring against the sensors not yet reached; bands of two bootstrap standard errors (499 draws of sensors)")
    source(fig, "Before entry the estimates are placebos, zero under parallel trends. New York's declared rings leave no sensors to compare with for the inner rings; its narrowest estimable set is shown.")
    save(fig, "changes-02-event-studies")

    # 3. Every estimate under every set of rings.
    fig, axes = figure(1, 2)
    fig.subplots_adjust(top=0.84, bottom=0.1, left=0.16, right=0.98, wspace=0.62)
    for ax, (city, c) in zip(axes, CASES.items()):
        rows = []
        for rings in c["sets"]:
            r = load(city, rings)
            inner = 0
            for k, b in enumerate(r["bounds"]):
                tw, st = r["two_way"][k], r["staggered"][k]
                rows.append((f"{rings.replace('-', ', ')} m · {inner:.0f}–{b:.0f} m", tw["effect"], tw["se"], st["simple"], st["simple_se"]))
                inner = b
        y = list(range(len(rows)))[::-1]
        for yi, (lab, e1, s1, e2, s2) in zip(y, rows):
            if e1 is not None and s1 is not None:
                ax.errorbar(e1, yi + 0.12, xerr=1.96 * s1, fmt="o", color=CYCLE[0], ms=6, capsize=0, lw=1.6)
            if e2 is not None and s2 is not None:
                ax.errorbar(e2, yi - 0.12, xerr=1.96 * s2, fmt="s", color=CYCLE[1], ms=6, capsize=0, lw=1.6)
        ax.set_yticks(y, [r[0] for r in rows], fontsize=10)
        ax.axvline(0, color=MUTED, linewidth=0.8)
        ax.set_xlabel("effect (log; 0.69 is a doubling)")
        ax.set_title(c["name"].split(":")[0], loc="left", fontsize=14, color=INK)
        ax.plot([], [], "o", color=CYCLE[0], label="two-way regression on the rings")
        ax.plot([], [], "s", color=CYCLE[1], label="against those not yet reached")
        ax.legend(frameon=False, loc="upper right", fontsize=11, bbox_to_anchor=(1.0, 1.1))
        summary[city]["estimates"] = [{"rings": lab, "two_way": e1, "two_way_se": s1, "staggered": e2, "staggered_se": s2} for lab, e1, s1, e2, s2 in rows]
    title(fig, "Every estimate, under every set of rings", "95% intervals; the first set of each city was declared before its results were seen")
    source(fig, "Sets of rings of distance along the ways, and the ring of each estimate. A missing estimate had no sensors to compare with.")
    save(fig, "changes-03-estimates")
    json.dump(summary, open(RESULTS / "summary.json", "w", encoding="utf-8"), indent=1)
    for city, s in summary.items():
        print(city, s["entering"], s["counting_before"], s["never"])


if __name__ == "__main__":
    main()
