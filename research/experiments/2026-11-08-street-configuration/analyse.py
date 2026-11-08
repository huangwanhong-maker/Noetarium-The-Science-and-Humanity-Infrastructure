"""The configuration of Melbourne's streets against its walkers: statistics and figures (ADR 0084).

Reads results/counts.json (prep.py), results/measures.json and results/maps.json (configuration.cpp) and the flow study's
calibration (../2026-11-05-a-district-economy-as-a-flow/results/calibration.json). Writes results/results.json and the gallery's
pictures config-01 to config-04.

- The response: the logarithm of each counter's mean daily count (both directions, days with all 24 hours counted).
- Each predictor: a configuration measure of the counter's nearest segment (NACH, NAIN, or log(choice + 1)), for each network, cost
  and radius. For each: Spearman's rho; the least-squares line's R^2 and its leave-one-out R^2 (1 - PRESS/SST, from the hat matrix).
- The primary tests, declared before the results were seen: on the streets, angular NACH and NAIN at 800 m and at n. Each has a
  permutation p-value for rho (20,000 permutations, seeded), adjusted by Holm's method over the four. Everything else is exploratory.
- Edge effects: at a radius R, the counters at least R from the edge of the city model, apart.
- Against the walkers: on the flow study's 16 counters, its observed daily totals, its walkers' leave-one-out predictions, and the
  primary measure that did best on the 87 counters, its line refitted leaving each counter out.
- Configuration and attraction (Hillier, Penn, Hanson, Grajewski and Xu 1993, "Natural movement", Environ. Plann. B 20, 29-66): the
  log count on the best primary measure, on the log of the non-residential floor space within 250 m (CLUE), and on both; each with
  its R^2 and leave-one-out R^2.

Run from the repository's root after prep.py and configuration: python research/experiments/2026-11-08-street-configuration/analyse.py
"""

import json
import math
import pathlib
import sys

import numpy as np

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "style"))
from search_style import CYCLE, HAIRLINE, INK, MUTED, ORCHID, PAPER, figure, save, source, title  # noqa: E402

import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.collections import LineCollection  # noqa: E402

RESULTS = HERE / "results"
FLOW = HERE.parent / "2026-11-05-a-district-economy-as-a-flow" / "results" / "calibration.json"
MEASURES = {"nach": "NACH", "integration": "NAIN", "logchoice": "log(choice + 1)"}
PRIMARY = [("streets", "angular", 800, "nach"), ("streets", "angular", 800, "integration"), ("streets", "angular", "n", "nach"), ("streets", "angular", "n", "integration")]
RADII = [400, 800, 1200, 2000, "n"]


def ranks(v):
    v = np.asarray(v, float)
    order = np.argsort(v, kind="mergesort")
    r = np.empty(len(v))
    r[order] = np.arange(len(v), dtype=float)
    # Ties share their mean rank.
    sv = v[order]
    i = 0
    while i < len(v):
        j = i
        while j + 1 < len(v) and sv[j + 1] == sv[i]:
            j += 1
        r[order[i:j + 1]] = (i + j) / 2
        i = j + 1
    return r


def spearman(x, y):
    return float(np.corrcoef(ranks(x), ranks(y))[0, 1])


def permutation_p(x, y, n=20000, seed=20261108):
    rng = np.random.default_rng(seed)
    rx, ry = ranks(x), ranks(y)
    observed = abs(np.corrcoef(rx, ry)[0, 1])
    hits = sum(abs(np.corrcoef(rx, rng.permutation(ry))[0, 1]) >= observed for _ in range(n))
    return (hits + 1) / (n + 1)


def line(x, y):
    """The least-squares line, its R^2 and its leave-one-out R^2 (1 - PRESS/SST)."""
    x, y = np.asarray(x, float), np.asarray(y, float)
    X = np.column_stack([np.ones_like(x), x])
    beta, *_ = np.linalg.lstsq(X, y, rcond=None)
    e = y - X @ beta
    H = X @ np.linalg.inv(X.T @ X) @ X.T
    sst = float(((y - y.mean()) ** 2).sum())
    press = float(((e / (1 - np.diag(H))) ** 2).sum())
    return {"a": float(beta[0]), "b": float(beta[1]), "r2": 1 - float((e ** 2).sum()) / sst, "loo_r2": 1 - press / sst}


def regress(columns, y):
    """Least squares on several columns and an intercept: R^2 and leave-one-out R^2."""
    y = np.asarray(y, float)
    X = np.column_stack([np.ones(len(y))] + [np.asarray(c, float) for c in columns])
    beta, *_ = np.linalg.lstsq(X, y, rcond=None)
    e = y - X @ beta
    H = X @ np.linalg.inv(X.T @ X) @ X.T
    sst = float(((y - y.mean()) ** 2).sum())
    press = float(((e / (1 - np.diag(H))) ** 2).sum())
    return {"beta": beta.tolist(), "r2": 1 - float((e ** 2).sum()) / sst, "loo_r2": 1 - press / sst}


def loo_predictions(x, y):
    x, y = np.asarray(x, float), np.asarray(y, float)
    out = np.empty_like(y)
    for i in range(len(y)):
        keep = np.arange(len(y)) != i
        fit = line(x[keep], y[keep])
        out[i] = fit["a"] + fit["b"] * x[i]
    return out


def holm(ps):
    order = np.argsort(ps)
    adjusted = np.empty(len(ps))
    running = 0.0
    for k, i in enumerate(order):
        running = max(running, min(1.0, (len(ps) - k) * ps[i]))
        adjusted[i] = running
    return adjusted.tolist()


def value(point, measure):
    return math.log(point["choice"] + 1) if measure == "logchoice" else point[measure]


def r2(y, yhat):
    y, yhat = np.asarray(y, float), np.asarray(yhat, float)
    return 1 - float(((y - yhat) ** 2).sum()) / float(((y - y.mean()) ** 2).sum())


def main():
    counts = json.load(open(RESULTS / "counts.json", encoding="utf-8"))
    measures = json.load(open(RESULTS / "measures.json", encoding="utf-8"))
    counters = {c["id"]: c for c in counts["counters"]}
    rows = []
    for network, data in measures["networks"].items():
        for run in data["runs"]:
            pts = [p for p in run["points"] if p["id"] in counters]
            for measure in MEASURES:
                x = [value(p, measure) for p in pts]
                y = [math.log(counters[p["id"]]["mean_daily"]) for p in pts]
                fit = line(x, y)
                row = {"network": network, "cost": run["cost"], "radius": run["radius"], "measure": measure, "n": len(pts), "rho": spearman(x, y), **fit}
                if run["radius"] != "n":
                    safe = [p for p in pts if counters[p["id"]]["edge_m"] >= run["radius"]]
                    if len(safe) >= 8:
                        xs = [value(p, measure) for p in safe]
                        ys = [math.log(counters[p["id"]]["mean_daily"]) for p in safe]
                        row["edge_safe"] = {"n": len(safe), "rho": spearman(xs, ys), **line(xs, ys)}
                rows.append(row)

    def find(network, cost, radius, measure):
        return next(r for r in rows if r["network"] == network and r["cost"] == cost and r["radius"] == radius and r["measure"] == measure)

    def points_of(network, cost, radius):
        run = next(r for r in measures["networks"][network]["runs"] if r["cost"] == cost and r["radius"] == radius)
        return {p["id"]: p for p in run["points"] if p["id"] in counters}

    primary = []
    for network, cost, radius, measure in PRIMARY:
        pts = points_of(network, cost, radius)
        x = [value(pts[k], measure) for k in sorted(pts)]
        y = [math.log(counters[k]["mean_daily"]) for k in sorted(pts)]
        primary.append({**find(network, cost, radius, measure), "p": permutation_p(x, y)})
    for row, adj in zip(primary, holm([p["p"] for p in primary])):
        row["p_holm"] = adj
    best = max(primary, key=lambda r: r["loo_r2"])

    # Against the walkers, on the flow study's counters.
    flow = json.load(open(FLOW, encoding="utf-8"))
    ids = [c["id"] for c in flow["counters"]]
    obs = np.array([math.log(sum(c["observed"]) / 7) for c in flow["counters"]])
    fitted = np.array([math.log(max(sum(c["fitted"]), 1e-9) / 7) for c in flow["counters"]])
    loo = np.array([math.log(max(sum(c["loo"]), 1e-9) / 7) for c in flow["counters"]])
    pts = points_of(best["network"], best["cost"], best["radius"])
    xb = np.array([value(pts[k], best["measure"]) for k in ids])
    conf_loo = loo_predictions(xb, obs)
    head = {"counters": len(ids), "measure": {k: best[k] for k in ("network", "cost", "radius", "measure")},
            "walkers_r2": r2(obs, fitted), "walkers_loo_r2": r2(obs, loo), "configuration_r2": line(xb, obs)["r2"], "configuration_loo_r2": r2(obs, conf_loo),
            "rho_configuration": spearman(xb, obs), "rho_walkers_loo": spearman(loo, obs)}

    # Configuration and attraction.
    pb = points_of(best["network"], best["cost"], best["radius"])
    keys = sorted(pb)
    yy = [math.log(counters[k]["mean_daily"]) for k in keys]
    xc = [value(pb[k], best["measure"]) for k in keys]
    xa = [math.log(counters[k]["floor_nonres_m2"]) for k in keys]
    xall = [math.log(counters[k]["floor_m2"]) for k in keys]
    attraction = {"configuration": regress([xc], yy), "attraction": regress([xa], yy), "both": regress([xc, xa], yy),
                  "attraction_all_uses": regress([xall], yy), "rho_attraction": spearman(xa, yy),
                  "radius_m": counts.get("attraction_radius_m"), "clue_year": counts.get("clue_year")}

    # The counters the best measure misses most, by their residuals on the log scale.
    pb = points_of(best["network"], best["cost"], best["radius"])
    resid = sorted(((math.log(counters[k]["mean_daily"]) - (best["a"] + best["b"] * value(pb[k], best["measure"])), counters[k]["name"], counters[k]["mean_daily"]) for k in pb),
                   key=lambda t: t[0])
    out = {"period": counts["period"], "counters": len(counters), "networks": {k: {"junctions": v["junctions"], "segments": v["segments"], "seconds": round(v["seconds"], 1)}
                                                                          for k, v in measures["networks"].items()},
           "primary": primary, "best_primary": {k: best[k] for k in ("network", "cost", "radius", "measure", "r2", "loo_r2", "rho")},
           "walkers": head, "attraction": attraction, "most_under": [{"name": n, "mean_daily": d, "residual": round(r, 3)} for r, n, d in resid[:5]],
           "most_over": [{"name": n, "mean_daily": d, "residual": round(r, 3)} for r, n, d in resid[::-1][:5]], "all": rows}
    with open(RESULTS / "results.json", "w", encoding="utf-8") as f:
        json.dump(out, f, indent=1)

    # ---- figures ----
    maps = json.load(open(RESULTS / "maps.json", encoding="utf-8"))
    seg = np.array(maps["segments"])
    for key, label, name in (("angular_n", "radius n", "config-01-choice-n"), ("angular_800", "radius 800 m", "config-02-choice-800")):
        v = np.array(maps[key]["nach"])
        lo, hi = np.percentile(v, 2), np.percentile(v, 99.5)
        u = np.clip((v - lo) / (hi - lo), 0, 1)
        order = np.argsort(u)
        fig, ax = figure()
        fig.subplots_adjust(left=0.04, right=0.80, top=0.86, bottom=0.07)
        lines = [[(s[0], s[1]), (s[2], s[3])] for s in seg[order]]
        ax.add_collection(LineCollection(lines, colors=ORCHID(0.15 + 0.85 * u[order]), linewidths=0.4 + 2.6 * u[order] ** 2, capstyle="round"))
        cx = np.array([counters[k]["x"] for k in counters])
        cy = np.array([counters[k]["y"] for k in counters])
        cs = np.array([counters[k]["mean_daily"] for k in counters])
        ax.scatter(cx, cy, s=6 + 220 * np.sqrt(cs / cs.max()), facecolors="none", edgecolors=INK, linewidths=0.9, zorder=3)
        ax.set_xlim(seg[:, [0, 2]].min(), seg[:, [0, 2]].max())
        ax.set_ylim(seg[:, [1, 3]].min(), seg[:, [1, 3]].max())
        ax.set_aspect("equal")
        ax.grid(False)
        ax.set_xticks([]), ax.set_yticks([])
        for sp in ax.spines.values():
            sp.set_visible(False)
        cax = fig.add_axes([0.83, 0.30, 0.012, 0.40])
        cb = fig.colorbar(plt.cm.ScalarMappable(cmap=ORCHID, norm=plt.Normalize(lo, hi)), cax=cax)
        cb.outline.set_visible(False)
        cb.set_label("angular choice, NACH", color=INK)
        fig.text(0.83, 0.25, "circles: the counters,\narea by mean daily count", fontsize=11, color=MUTED, va="top")
        r = find("streets", "angular", 800 if key.endswith("800") else "n", "nach")
        title(fig, f"How each street of Melbourne's centre stands in the whole: angular choice at {label}",
              f"{len(seg):,} segments of the street centre lines; against the {r['n']} counters' mean daily walkers, Spearman's ρ = {r['rho']:.2f}, "
              f"R² = {r['r2']:.2f} (leaving each out, {r['loo_r2']:.2f})")
        source(fig, "OpenStreetMap contributors (ODbL), the data library's city model; City of Melbourne Pedestrian Counting System, "
                    f"{counts['period'][0]} to {counts['period'][1]} (CC BY 4.0). urban-science configuration on morphe's centralities.")
        save(fig, name)

    fig, axes = figure(1, 2)
    fig.subplots_adjust(left=0.06, right=0.97, top=0.82, bottom=0.12, wspace=0.18)
    xs = list(range(len(RADII)))
    for ax, measure in zip(axes, ("nach", "integration")):
        for k, cost in enumerate(("angular", "metric", "topological")):
            ys = [find("streets", cost, rad, measure)["loo_r2"] for rad in RADII]
            ax.plot(xs, ys, marker="o", color=CYCLE[k], label=f"{cost}, streets")
        for net, colour, label, radii in (("walking", CYCLE[3], "angular, with footways", (800, "n")), ("roads", CYCLE[4], "angular, without lanes", (400, 800, 1200, "n"))):
            ys = [find(net, "angular", rad, measure)["loo_r2"] if rad in radii else None for rad in RADII]
            ax.plot([x for x, y in zip(xs, ys) if y is not None], [y for y in ys if y is not None], marker="s", linestyle="--", color=colour, label=label)
        ax.set_xticks(xs, [str(r) if r != "n" else "n" for r in RADII])
        ax.set_xlabel("radius (m)")
        ax.set_ylabel("R², each counter left out")
        ax.set_title(MEASURES[measure], loc="left", color=INK)
        ax.axhline(0, color=HAIRLINE, linewidth=0.8)
    axes[0].legend(loc="lower right")
    title(fig, "How much of where people walk the shape of the streets explains, by cost and radius",
          f"log mean daily count at {len(counters)} counters on a configuration measure of the nearest segment; R² with each counter left out of the line")
    source(fig, "Exploratory beside the four primary tests (angular NACH and NAIN at 800 m and n). urban-science configuration; morphe centralities.")
    save(fig, "config-03-by-radius")

    fig, axes = figure(1, 2)
    fig.subplots_adjust(left=0.06, right=0.97, top=0.82, bottom=0.12, wspace=0.2)
    pts = points_of(best["network"], best["cost"], best["radius"])
    x = np.array([value(pts[k], best["measure"]) for k in pts])
    y = np.array([math.log10(counters[k]["mean_daily"]) for k in pts])
    axes[0].scatter(x, y, s=28, color=CYCLE[0], alpha=0.85, edgecolors="none")
    gx = np.linspace(x.min(), x.max(), 50)
    axes[0].plot(gx, (best["a"] + best["b"] * gx) / math.log(10), color=INK, linewidth=1.2)
    for r, n, d in resid[:3] + resid[::-1][:3]:
        k = next(k for k in pts if counters[k]["name"] == n)
        axes[0].annotate(n, (value(pts[k], best["measure"]), math.log10(d)), fontsize=9, color=MUTED, xytext=(5, 3), textcoords="offset points")
    axes[0].set_xlabel(f"{best['cost']} {MEASURES[best['measure']]} at radius {best['radius']}")
    axes[0].set_ylabel("mean daily count (log₁₀)")
    axes[0].set_title(f"{len(x)} counters", loc="left", color=INK)
    lim = [min(obs.min(), loo.min(), conf_loo.min()) / math.log(10) - 0.1, max(obs.max(), loo.max(), conf_loo.max()) / math.log(10) + 0.1]
    axes[1].plot(lim, lim, color=HAIRLINE, linewidth=1)
    axes[1].scatter(loo / math.log(10), obs / math.log(10), s=40, color=CYCLE[1], label=f"walkers' model, left out (R² {head['walkers_loo_r2']:.2f})")
    axes[1].scatter(conf_loo / math.log(10), obs / math.log(10), s=40, marker="s", color=CYCLE[0], label=f"configuration, left out (R² {head['configuration_loo_r2']:.2f})")
    axes[1].set_xlabel("predicted daily count (log₁₀)")
    axes[1].set_ylabel("observed daily count (log₁₀)")
    axes[1].set_title("the walkers' model's 16 counters", loc="left", color=INK)
    axes[1].legend(loc="upper left")
    title(fig, "The shape of the streets against where people walk, and against the walkers' model",
          "left: each counter's mean daily count on its street's configuration; right: each of the flow study's counters predicted with itself left out")
    source(fig, "The walkers' model: the district-economy flow study (destinations, widths and hours; calibrated to these counters). Configuration: one line on one measure.")
    save(fig, "config-04-against-walkers")

    fig, axes = figure(1, 2)
    fig.subplots_adjust(left=0.06, right=0.97, top=0.82, bottom=0.12, wspace=0.22)
    names = ["configuration", "attraction", "both"]
    labels = [f"{best['cost']} {MEASURES[best['measure']]}\nat {best['radius']} m", "non-residential floor\nspace within 250 m", "both"]
    w = 0.36
    axes[0].bar([i - w / 2 for i in range(3)], [attraction[n]["r2"] for n in names], w, color=CYCLE[0], label="R²")
    axes[0].bar([i + w / 2 for i in range(3)], [attraction[n]["loo_r2"] for n in names], w, color=CYCLE[2], label="R², each counter left out")
    axes[0].set_xticks(range(3), labels)
    axes[0].axhline(0, color=HAIRLINE, linewidth=0.8)
    axes[0].set_ylabel("share of the variance of log daily count")
    axes[0].legend(loc="upper left")
    axes[0].set_title(f"{len(keys)} counters", loc="left", color=INK)
    axes[1].scatter(np.array(xa) / math.log(10), np.array(yy) / math.log(10), s=28, color=CYCLE[2], alpha=0.85, edgecolors="none")
    fa = attraction["attraction"]["beta"]
    gx = np.linspace(min(xa), max(xa), 50)
    axes[1].plot(gx / math.log(10), (fa[0] + fa[1] * gx) / math.log(10), color=INK, linewidth=1.2)
    axes[1].set_xlabel("non-residential floor space within 250 m (log₁₀ m²)")
    axes[1].set_ylabel("mean daily count (log₁₀)")
    axes[1].set_title(f"Spearman's ρ = {attraction['rho_attraction']:.2f}", loc="left", color=INK)
    title(fig, "Neither the shape of the streets nor the floor space about them explains much of where walkers are counted",
          "left: the log daily count on configuration, on the floor space about each counter, and on both; right: against the floor space")
    source(fig, f"City of Melbourne, Census of Land Use and Employment {counts.get('clue_year')} (floor space by use by block, CC BY 4.0); Pedestrian Counting System. "
                "Hillier et al. (1993): movement as configuration and attraction.")
    save(fig, "config-05-configuration-and-attraction")
    print(json.dumps({k: out[k] for k in ("best_primary", "walkers", "attraction")}, indent=1))
    for p in primary:
        print(p["cost"], p["radius"], p["measure"], f"rho {p['rho']:.3f} r2 {p['r2']:.3f} loo {p['loo_r2']:.3f} p {p['p']:.5f} holm {p['p_holm']:.5f}")


if __name__ == "__main__":
    main()
