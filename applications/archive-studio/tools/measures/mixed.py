"""A linear mixed model: an outcome by its predictors, with each group's own intercept and slopes."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Mixed(api.BoardAnalysis):
        id = "mixed"
        title = "A linear mixed model"
        description = ("An outcome by its predictors when observations come in groups (pupils in schools, answers in countries, years of one person): the fixed "
                       "effects with their standard errors, each group's own intercept (and slopes, if named) as random effects with their covariance, the residual "
                       "variance and the intraclass correlation, by profiled REML or maximum likelihood (Bates et al. 2015). tyche's mixed.")
        params = [api.param("table", "Data", "", kind="text", role="table"), c.col(api, "y", "Outcome"), c.cols(api, "x", "Predictors"),
                  c.col(api, "group", "Group"), c.cols(api, "slopes", "Random slopes (optional)", help="some of the predictors"),
                  api.param("method", "Estimated by", "REML", kind="choice", options=["REML", "ML"])]

        def run(self, board, params, context):
            t = board.table(params["table"])
            xs, slopes = c.words(params["x"]), c.words(params["slopes"])
            if any(s not in xs for s in slopes):
                raise ValueError("a random slope is one of the predictors")
            rows, dropped = c.complete(t, [params["y"]] + xs, [params["group"]])
            y, g = c.numbers(t, params["y"]), c.text(t, params["group"])
            X = {k: c.numbers(t, k) for k in xs}
            r = c.inquiry.ask("mixed", y=[y[i] for i in rows], x=[[X[k][i] for i in rows] for k in xs], names=xs, group=[g[i] for i in rows],
                              slopes=[xs.index(s) for s in slopes], reml=params["method"] == "REML")
            fixed = {"term": r["names"], "estimate": r["beta"], "se": r["se"], "t": [b / s if s else None for b, s in zip(r["beta"], r["se"])]}
            random_names = ["intercept"] + slopes
            G = r["G"]
            comp = {"component": [f"var({n})" for n in random_names] + ["residual"], "variance": [G[k][k] for k in range(len(G))] + [r["sigma2"]]}
            blup = {"group": r["labels"]}
            for k, n in enumerate(random_names):
                blup[n] = [row[k] for row in r["blup"]]
            icc = f"; the intraclass correlation is {r['icc']:.3f}" if r.get("icc") is not None else ""
            return [api.table("fixed_effects", fixed, title="Fixed effects"), api.table("variance", comp, title="Variance components"),
                    api.table("group_effects", blup, title="Each group's predicted effects"),
                    api.note(f"{len(rows)} observations in {int(r['groups'])} groups, by {params['method']}{icc}." + ("" if r["converged"] else " Not converged.") + c.left_out(dropped))]
    return Mixed()
