"""Factors: what a set of variables measures in common, by maximum likelihood, rotated by varimax."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Factors(api.BoardAnalysis):
        id = "factors"
        title = "Factor analysis"
        description = ("What several variables measure in common: the loadings of each on a number of factors, fitted to their correlations by maximum likelihood "
                       "(Rubin and Thayer's EM), rotated by varimax (Kaiser 1958) when there is more than one, each variable's communality and uniqueness, and "
                       "the likelihood-ratio test of so many factors. tyche's latent.")
        params = [api.param("table", "Data", "", kind="text", role="table"), c.cols(api, "variables", "Variables"),
                  api.param("factors", "Factors", 2, kind="integer", minimum=1, maximum=10),
                  api.param("rotate", "Rotate by varimax", "yes", kind="choice", options=["yes", "no"])]

        def run(self, board, params, context):
            t = board.table(params["table"])
            names = c.words(params["variables"])
            rows, dropped = c.complete(t, names)
            values = {k: c.numbers(t, k) for k in names}
            k = int(params["factors"])
            r = c.inquiry.ask("factors", variables=names, data=[[values[v][i] for v in names] for i in rows], factors=k, rotate=params["rotate"] == "yes")
            table = {"variable": names}
            for j in range(k):
                table[f"factor_{j + 1}"] = [row[j] for row in r["loadings"]]
            table["communality"], table["uniqueness"] = r["communality"], r["uniqueness"]
            test = f"; χ² {r['chi2']:.2f} on {int(r['df'])} degrees of freedom" if r.get("chi2") is not None and r["df"] > 0 else ""
            heywood = f" Heywood cases (a uniqueness at its floor): {', '.join(names[i] for i in r['heywood'])}." if r["heywood"] else ""
            return [api.table("loadings", table, title=f"Loadings on {k} factor{'s' if k > 1 else ''}" + (", varimax" if r["rotated"] else "")),
                    api.note(f"{len(rows)} cases, {len(names)} variables, {k} factors{test}" + ("" if r["converged"] else "; not converged") + "." + heywood + c.left_out(dropped))]
    return Factors()
