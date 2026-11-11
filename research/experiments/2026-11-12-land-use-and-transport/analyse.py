"""The land-use and transport study's figures, from run.py's results (results/summary.json, results/tracts.csv).

- luti-04-decay: the decay of New York City's journeys to work with distance, year by year, 2002–2023: the calibrated β and the mean
  journey, with the share of the flows the model places where they are (the common part of commuters).
- luti-05-forecast: 2019's table from 2010's β against 2010's table carried to 2019's margins, 2019's own calibration, and 2010's
  table unchanged; the same from 2019 to 2023 and from 2002 to 2019.
- luti-06-access: each tract's change of accessibility to jobs, 2010 to 2019, and the change of its resident workers.
- luti-07-model: the composed model's change of resident workers, 2010 to 2019, against the record's.

Writes the figures to assets/gallery/.
Run from the repository's root: python research/experiments/2026-11-12-land-use-and-transport/analyse.py
"""

import csv
import json
import math
import pathlib
import sys

import numpy as np

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "style"))
from search_style import CYCLE, DUSK, INK, MUTED, PAPER, figure, save, source, title  # noqa: E402

RESULTS = HERE / "results"
SOURCE = "LODES 8 (US Census Bureau), New York State's main files, jobs whose home and workplace lie in New York City's five boroughs, by 2020 census tract; costs the great-circle kilometres between the tracts' points."


def tracts():
    with open(RESULTS / "tracts.csv", encoding="utf-8") as f:
        rows = list(csv.DictReader(f))
    num = lambda r, k: float(r[k]) if r[k] not in ("", "null") else math.nan
    return {k: np.array([num(r, k) for r in rows]) for k in ("lat", "lon", "workers2010", "workers2019", "jobs2010", "jobs2019", "access2010", "access2019", "model2019")}


def decay(s):
    d = s["decay"]
    years = [r["year"] for r in d]
    fig, (a, b) = figure(1, 2)
    fig.subplots_adjust(top=0.82, bottom=0.12, wspace=0.25)
    a.plot(years, [r["beta"] for r in d], marker="o", color=CYCLE[0])
    a.set_ylabel("β, per km (the doubly constrained model, Hyman's calibration)")
    a.set_xlabel("year")
    for r in d:
        a.annotate(f"CPC {r['cpc']:.2f}", (r["year"], r["beta"]), textcoords="offset points", xytext=(0, 9), ha="center", fontsize=9, color=MUTED)
    b.plot(years, [r["meanKm"] for r in d], marker="o", color=CYCLE[1], label="the record")
    b.set_ylabel("the mean journey to work, km (straight)")
    b.set_ylim(0, 12)
    b.set_xlabel("year")
    b2 = b.twinx()
    b2.bar(years, [r["jobs"] / 1e6 for r in d], width=0.8, color=CYCLE[2], alpha=0.25)
    b2.set_ylabel("jobs held by the boroughs' residents in the boroughs, millions", color=MUTED)
    b2.grid(False)
    b2.spines["right"].set_visible(True)
    title(fig, "New York's journeys to work, and how they fall with distance",
          "the decay of the doubly constrained model fitted to each year's table between the 2,327 tracts; the common part of commuters (CPC) the share of the flows it places where they are")
    source(fig, SOURCE)
    save(fig, "luti-04-decay")


def forecast(s):
    f = s["forecast"]
    kinds = [("cpcGravity", "the earlier year's β at the later year's margins"), ("cpcCarried", "the earlier table carried to the later margins, its empty cells by gravity"),
             ("cpcOwn", "the later year's own β (fitted to it)"), ("cpcUnchanged", "the earlier table unchanged")]
    fig, ax = figure()
    fig.subplots_adjust(top=0.82, bottom=0.14)
    x = np.arange(len(f))
    w = 0.2
    for k, (key, name) in enumerate(kinds):
        vals = [r[key] for r in f]
        ax.bar(x + (k - 1.5) * w, vals, w, color=CYCLE[k], label=name)
        for i, v in enumerate(vals):
            ax.text(x[i] + (k - 1.5) * w, v + 0.005, f"{v:.3f}", ha="center", fontsize=10, color=INK)
    ax.set_xticks(x, [f"{r['from']} to {r['to']}" for r in f])
    ax.set_ylabel("common part of commuters with the later year's table")
    ax.set_ylim(0, 1)
    ax.legend(loc="upper right")
    first = f[0]
    title(fig, "Forecasting where New Yorkers work, from an earlier year",
          "the flows between tracts forecast from an earlier year's, each scored by the share it places where the later year's are (the common part of commuters)")
    source(fig, SOURCE)
    save(fig, "luti-05-forecast")


def access(s, t):
    keep = (t["workers2010"] >= 200) & (t["workers2019"] > 0) & np.isfinite(t["access2010"]) & np.isfinite(t["access2019"])
    dA = t["access2019"] - t["access2010"]
    dH = np.log(np.where(keep, t["workers2019"], 1) / np.where(keep, t["workers2010"], 1))
    fig, (a, b) = figure(1, 2)
    fig.subplots_adjust(top=0.82, bottom=0.1, wspace=0.08)
    coslat = math.cos(math.radians(40.71))
    for ax, v, name in ((a, dA, "accessibility to jobs, km of the logsum at 2010's β"), (b, dH, "the log of resident workers")):
        m = np.nanmax(np.abs((v - np.nanmedian(v[keep]))[keep]))
        lim = np.nanpercentile(np.abs(v[keep] - np.nanmedian(v[keep])), 97)
        sc = ax.scatter(t["lon"][keep], t["lat"][keep], c=v[keep] - np.nanmedian(v[keep]), cmap=DUSK, vmin=-lim, vmax=lim, s=9, linewidths=0)
        ax.set_aspect(1 / coslat)
        ax.set_xticks([])
        ax.set_yticks([])
        ax.grid(False)
        for sp in ax.spines.values():
            sp.set_visible(False)
        ax.set_title(f"the change of {name}", fontsize=13, color=INK)
        cb = fig.colorbar(sc, ax=ax, orientation="horizontal", fraction=0.04, pad=0.02)
        cb.set_label("2010 to 2019, against the median tract", color=MUTED)
        cb.outline.set_visible(False)
    acc = s["access"]
    title(fig, "Where access to jobs rose, and where resident workers came",
          f"{acc['tracts']:,} tracts of 200 resident workers or more; within a borough, a kilometre of access goes with {100 * acc['semSlope']:+.1f}% resident workers (± {100 * acc['semSe']:.1f}, the spatial error model)")
    source(fig, SOURCE)
    save(fig, "luti-06-access")


def model(s, t):
    keep = (t["workers2010"] >= 200) & (t["workers2019"] > 0) & np.isfinite(t["model2019"])
    obs = np.log(t["workers2019"][keep] / t["workers2010"][keep])
    pred = np.log(t["model2019"][keep] / np.maximum(t["workers2010"][keep], 1))
    fig, ax = figure()
    fig.subplots_adjust(top=0.82, bottom=0.12)
    ax.scatter(pred, obs, s=10, color=CYCLE[0], alpha=0.45, linewidths=0)
    g = math.log(1 + s["model"]["growthObserved"])
    ax.axhline(g, color=MUTED, lw=0.8, ls="--")
    ax.axvline(g, color=MUTED, lw=0.8, ls="--")
    lo, hi = np.percentile(np.concatenate([obs, pred]), [0.5, 99.5])
    ax.plot([lo, hi], [lo, hi], color=INK, lw=0.8)
    ax.set_xlim(lo, hi)
    ax.set_ylim(lo, hi)
    ax.set_xlabel("the model's change of the log of resident workers, 2010 to 2019")
    ax.set_ylabel("the record's")
    m = s["model"]
    title(fig, "The composed model from 2010 to 2019: where the resident workers went",
          f"revealed in 2010, the jobs of the record year by year, the speed of building set so that the total is the record's (+{100 * m['growthObserved']:.1f}%); "
          f"correlation {m['correlation']:.2f} over {m['tracts']:,} tracts, R² {m['r2']:.2f} (uniform growth {m['r2Uniform']:.2f})")
    source(fig, SOURCE.replace("; costs the great-circle kilometres between the tracts' points.", ". The model: urban science's luti, one class of household, building after rents."))
    save(fig, "luti-07-model")


def main():
    s = json.load(open(RESULTS / "summary.json", encoding="utf-8"))
    t = tracts()
    decay(s)
    forecast(s)
    access(s, t)
    model(s, t)
    print("wrote luti-04 to luti-07")


if __name__ == "__main__":
    main()
