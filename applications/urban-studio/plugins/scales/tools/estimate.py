"""An index where it was not observed: estimated from what was observed beside it, judged on blocks of units held out, and withheld
wherever it cannot be trusted, so that it is declared instead."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _scales as s  # noqa: E402


def create(api):
    class Estimate(api.BoardAnalysis):
        id = "estimate"
        title = "An index where it was not observed"
        description = ("A column of a table, observed in some rows and empty in others, estimated in the empty rows from other columns of the same "
                       "table. The estimator (least squares, or ridge, on standardised covariates; of the logarithm if asked) is judged by leaving out "
                       "one block of rows at a time (an area, a district), so that what is held out is not the near neighbour of what is fitted; its "
                       "intervals are conformal, from the errors on the blocks held out. An estimate is withheld, with its reason, where the estimator "
                       "does not beat the mean on the blocks held out, where the row's covariates are unlike any it learnt from (Meyer and Pebesma's "
                       "area of applicability), or where its interval is wider than the use allows (by default half the estimate, as statistical "
                       "offices withhold an estimate whose relative standard error passes 50%). What is withheld is to be declared.")
        params = [api.param("table", "Table", "", kind="text", role="table"),
                  api.param("target", "Index to estimate", "", kind="text", role="column", of="table"),
                  api.param("covariates", "From", "", kind="text", role="columns", of="table"),
                  api.param("block", "Blocks held out together", "", kind="text", role="column", of="table",
                            help="a column grouping the rows (an area); empty: ten blocks in the table's order"),
                  api.param("min_skill", "Least skill on blocks held out", 0.0, minimum=-1, maximum=0.9, step=0.05),
                  api.param("max_relative_halfwidth", "Widest interval allowed (share of the estimate)", 0.5, minimum=0.05, maximum=10, step=0.05),
                  api.param("level", "Coverage of the intervals", 0.9, minimum=0.5, maximum=0.99, step=0.01),
                  api.param("log", "Of the logarithm (1) or not (0)", 0, minimum=0, maximum=1, step=1)]

        def run(self, board, params, context):
            t = board.table(params["table"])
            target = params.get("target")
            covs = [c.strip() for c in str(params.get("covariates") or "").split(",") if c.strip()]
            if not target or not covs:
                raise ValueError("name the index to estimate and the columns to estimate it from")
            y = t.column(target)
            X = np.column_stack([t.column(c) for c in covs])
            ok_x = np.all(np.isfinite(X), axis=1)
            seen = np.isfinite(y) & ok_x
            want = ~np.isfinite(y) & ok_x
            if seen.sum() < len(covs) + 3:
                raise ValueError(f"only {int(seen.sum())} rows observe {target} with every covariate")
            if params.get("block"):
                blocks = [str(b) for b in t.raw(params["block"])]
            else:
                blocks = [str(i * 10 // max(1, t.rows)) for i in range(t.rows)]
            training = [{"covariates": X[i].tolist(), "value": float(y[i]), "block": blocks[i]} for i in np.where(seen)[0]]
            targets = [{"covariates": X[i].tolist()} for i in np.where(want)[0]]
            if not targets:
                raise ValueError(f"every row with its covariates already observes {target}: nothing to estimate")
            e = s.scales.ask("estimate", training=training, targets=targets, min_skill=float(params.get("min_skill") or 0),
                             max_relative_halfwidth=float(params.get("max_relative_halfwidth") or 0.5), level=float(params.get("level") or 0.9),
                             log=bool(int(params.get("log") or 0)))
            rows = np.where(want)[0]
            first = t.columns[0]
            out_cols = {first: [t.raw(first)[i] for i in rows], f"{target}, estimated": [None if w else v for v, w in zip(e["estimate"], e["withheld"])],
                        "lower": [None if w else v for v, w in zip(e["lower"], e["withheld"])],
                        "upper": [None if w else v for v, w in zip(e["upper"], e["withheld"])],
                        "dissimilarity": e["dissimilarity"], "given": ["withheld" if w else "estimated" for w in e["withheld"]],
                        "why withheld": [r or "" for r in e["reason"]]}
            out = [api.table("estimated", out_cols, title=f"{target} where it was not observed: estimated, or withheld with the reason")]
            given = sum(1 for w in e["withheld"] if not w)
            words = (f"Judged on {e['blocks']:.0f} blocks held out in turn, the estimator's skill against the mean is {e['cv_skill']:.2f} "
                     f"(1 is perfect, 0 no better than the mean); its {100 * float(params.get('level') or 0.9):.0f}% interval is ±{e['halfwidth']:.3g}"
                     f"{' on the logarithm' if int(params.get('log') or 0) else ''}. Of {len(targets)} rows without {target}, {given} are estimated "
                     f"and {len(targets) - given} withheld. ")
            imp = sorted(zip(covs, e["importance"]), key=lambda kv: -kv[1])
            words += "The covariates weigh, in the index of dissimilarity: " + ", ".join(f"{c} {w:.2f}" for c, w in imp) + ". "
            words += "A withheld row is a quantity to declare, with a range, before a model that needs it may run."
            out.append(api.note(words))
            return out
    return Estimate()
