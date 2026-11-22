"""A class of buildings' fragility, learnt from the damage an event left: fitted from a survey's counts by intensity, and a fragility
held before the event (from analysis, or transferred from elsewhere) updated by the same counts; what the earlier fragility predicted
set against what the survey found."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from science import risk  # noqa: E402


def create(api):
    class Fragility(api.BoardAnalysis):
        id = "fragility"
        title = "A fragility learnt from the damage an event left"
        description = ("A survey after an event counts the buildings of a class in each damage state at each intensity. From those counts each "
                       "state's lognormal fragility is fitted by maximum likelihood (the probit model in the logarithm of intensity), with its "
                       "standard errors; and a fragility held before the event, its medians and dispersions declared below, is updated by the same "
                       "counts on a grid of its parameters, with a credible band. What the earlier fragility predicted is set against what the "
                       "survey found, state by state. An intensity at a building is itself an estimate: its error widens the fitted dispersion, "
                       "which the answer says and does not remove.")
        params = [api.param("survey", "Damage survey", "survey", kind="text", role="table",
                            help="an intensity column, buildings, then a column of counts for each state from none to the worst"),
                  api.param("prior", "The fragility held before", "prior", kind="text", role="table", help="state, ratio, median, beta (one row a state, in order)"),
                  api.param("intensity", "The survey's column of intensity", "pga_g", kind="text"),
                  api.param("log_median_sd", "Prior spread of ln median", 0.5, minimum=0.05, maximum=3, step=0.05),
                  api.param("log_beta_sd", "Prior spread of ln β", 0.3, minimum=0.05, maximum=2, step=0.05),
                  api.param("level", "Credible level", 0.9, minimum=0.5, maximum=0.99, step=0.01)]

        def run(self, board, params, context):
            for k in ("survey", "prior"):
                if str(params.get(k) or "") not in board.tables:
                    raise ValueError("a survey and the fragility held before are both tables on the board")
            cols = lambda t: {c: t.raw(c) for c in t.columns}  # noqa: E731
            S = cols(board.table(params["survey"]))
            P = cols(board.table(params["prior"]))
            col = str(params.get("intensity") or "pga_g")
            if any(c not in P for c in ("state", "ratio", "median", "beta")):
                raise ValueError("the fragility held before has the columns state, ratio, median and beta")
            states = [str(s) for s in P["state"]]
            if col not in S or "buildings" not in S:
                raise ValueError(f"the survey has a column {col} and a column buildings")
            missing = [s for s in states if s not in S]
            if missing:
                raise ValueError(f"the survey has no column for {', '.join(missing)}")
            x = [float(v) for v in S[col]]
            n = [float(v) for v in S["buildings"]]
            counts = [[float(v) for v in S[s]] for s in states]
            # At or beyond each state: the counts of that state and every worse one.
            reaching = [[sum(counts[j][i] for j in range(k, len(states))) for k in range(len(states))] for i in range(len(x))]
            tallies = [{"intensity": x[i], "buildings": n[i], "reaching": reaching[i]} for i in range(len(x))]
            fits = risk.ask("vulnerability.fit", tallies=tallies, states=states)["curves"]
            grid = sorted(set([round(min(x) / 2 * (max(x) * 3 / (min(x) / 2)) ** (k / 24), 4) for k in range(25)]))
            level = float(params.get("level") or 0.9)
            rows = {"state": [], "intensity": [], "held_before": [], "fitted": [], "updated": [], "lower": [], "upper": []}
            summary = {"state": [], "median_before": [], "beta_before": [], "median_fitted": [], "se_ln_median": [], "beta_fitted": [], "se_beta": [],
                       "median_updated": [], "beta_updated": [], "at_grid_edge": []}
            Phi = lambda z: 0.5 * math.erfc(-z / math.sqrt(2))  # noqa: E731
            for k, s in enumerate(states):
                med, beta = float(P["median"][k]), float(P["beta"][k])
                U = risk.ask("vulnerability.update", prior={"median": med, "beta": beta, "log_median_sd": float(params.get("log_median_sd") or 0.5),
                                                            "log_beta_sd": float(params.get("log_beta_sd") or 0.3)},
                             observed=[{"intensity": x[i], "buildings": n[i], "reaching": reaching[i][k]} for i in range(len(x))], at=grid, level=level)
                f = fits[k]
                for b in U["bands"]:
                    rows["state"].append(s)
                    rows["intensity"].append(b["at"])
                    rows["held_before"].append(round(b["prior_mean"], 4))
                    rows["fitted"].append(round(Phi(math.log(b["at"] / f["median"]) / f["beta"]), 4))
                    rows["updated"].append(round(b["mean"], 4))
                    rows["lower"].append(round(b["lower"], 4))
                    rows["upper"].append(round(b["upper"], 4))
                for key, v in (("state", s), ("median_before", med), ("beta_before", beta), ("median_fitted", round(f["median"], 4)), ("se_ln_median", round(f["se_log_median"], 4)),
                               ("beta_fitted", round(f["beta"], 4)), ("se_beta", round(f["se_beta"], 4)), ("median_updated", round(U["mean_median"], 4)),
                               ("beta_updated", round(U["mean_beta"], 4)), ("at_grid_edge", "yes" if U["at_edge"] else "no")):
                    summary[key].append(v)
            ratios = [{"name": s, "ratio": float(P["ratio"][k])} for k, s in enumerate(states)]
            before = {"name": "held before", "states": ratios, "curves": [{"median": float(P["median"][k]), "beta": float(P["beta"][k])} for k in range(len(states))]}
            buildings = []
            for i in range(len(x)):
                for k in range(len(states) + 1):
                    c = n[i] - sum(counts[j][i] for j in range(len(states))) if k == 0 else counts[k - 1][i]
                    buildings += [{"intensity": x[i], "state": k}] * int(round(c))
            bt = risk.ask("vulnerability.back_test", fragility=before, buildings=buildings)["states"]
            out = [api.table("fragility_curves", summary, title="Each state's fragility: held before, fitted from the survey, and updated"),
                   api.table("fragility_bands", rows, title=f"P(at or beyond each state) by intensity: held before, fitted, updated with its {round(level * 100)}% band",
                             show={"kind": "chart", "chart": "line", "x": "intensity", "y": "updated", "how": "mean"}),
                   api.table("fragility_back_test", {"state": [b["state"] for b in bt], "observed": [b["observed"] for b in bt],
                                                     "expected_before": [round(b["expected"], 1) for b in bt], "z": [round(b["z"], 2) for b in bt]},
                             title="What the fragility held before predicted, against what the survey found (at or beyond each state)")]
            worst = max(bt, key=lambda b: abs(b["z"]))
            out.append(api.note(f"{round(sum(n)):,} buildings surveyed at {len(x)} intensities. The fragility held before predicted {worst['expected']:.0f} at or beyond "
                                f"{worst['state']}; the survey found {worst['observed']:.0f} (z = {worst['z']:.1f}). Fitted alone, the {states[-1]} state's median is "
                                f"{fits[-1]['median']:.2f} (± {fits[-1]['se_log_median']:.2f} in its logarithm); updated from what was held, {summary['median_updated'][-1]:.2f}. "
                                f"The intensities at the buildings are estimates, so the fitted dispersions include their error. "
                                + ("A posterior lies at its grid's edge: the prior bounds what the survey could say, and should be widened. " if "yes" in summary["at_grid_edge"] else "")))
            return out

    return Fragility()
