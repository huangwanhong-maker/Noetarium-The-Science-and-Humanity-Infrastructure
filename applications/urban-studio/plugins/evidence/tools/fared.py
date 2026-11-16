"""How the conclusions fared: what an archive's claims predicted of later looks, against what those looks found."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _evidence as e  # noqa: E402


def create(api):
    class Fared(api.BoardAnalysis):
        id = "fared"
        title = "How the conclusions fared"
        description = ("A table of later looks, claim by claim: how many units were looked at in each claim's place afterwards and how many were found "
                       "with the thing. What the archive believes of each claim predicts what those looks would find, if the state then is the state "
                       "the archive is about and the looks have a declared source's channel: P(found somewhere | n looks) = p (1 − (1 − a)ⁿ) + (1 − p) "
                       "(1 − (1 − b)ⁿ), with a and b a look's rates of finding under the claim and against it. The predictions are scored against what "
                       "was found (Brier's score and the log loss), beside the prior's and beside the count that takes repeated reports as independent, "
                       "and set out by how sure they were.")
        params = [e.archive_param(api),
                  api.param("table", "Later looks", "outcome", kind="text", role="table", help="one row a claim"),
                  api.param("claim", "Claim", "claim", kind="text", role="column", of="table"),
                  api.param("looked", "Units looked at", "looked", kind="text", role="column", of="table"),
                  api.param("found", "Units found with the thing", "found", kind="text", role="column", of="table"),
                  api.param("source", "The looks' source", "inspector", kind="text", help="a source of the archive: its channel is the looks'")]

        def run(self, board, params, context):
            a, r = e.appraised(params, board)
            src = next((s for s in a.get("sources", []) if s.get("id") == params["source"]), None)
            if not src or "hit" not in src or "false_alarm" not in src:
                raise LookupError(f"the archive declares no source '{params['source']}' with a hit and a false-alarm rate")
            h, f = float(src["hit"]), float(src["false_alarm"])
            claims = {c["id"]: c for c in a.get("claims", [])}
            weighed = {c["id"]: c for c in r["weigh"]["claims"]}
            t = board.table(params["table"])
            ids, looked, found = e.texts(t, params["claim"]), t.column(params["looked"]), t.column(params["found"])
            rows = []
            for cid, n, k in zip(ids, looked, found):
                if cid not in weighed or not (n >= 1) or "posterior" not in weighed[cid]:
                    continue
                c, w = claims[cid], weighed[cid]
                eh, en = float(c.get("e_given_h", 1)), float(c.get("e_given_not_h", 0))
                pa, pb = eh * h + (1 - eh) * f, en * h + (1 - en) * f
                predict = lambda p: p * (1 - (1 - pa) ** n) + (1 - p) * (1 - (1 - pb) ** n)
                prior = float(c.get("prior", w.get("prior", 0.5)))
                o = w.get("as_if_independent", 1) if w.get("items") else 1
                independent = 1.0 if not math.isfinite(o) else prior * o / (prior * o + 1 - prior)
                rows.append({"claim": cid, "looked": int(n), "found": int(k), "posterior": w["posterior"], "predicted": predict(w["posterior"]),
                             "prior_predicted": predict(prior), "independent_predicted": predict(independent), "hit": 1 if k > 0 else 0})
            if not rows:
                raise ValueError("no row of the table names a claim of the archive with a unit looked at")
            score = lambda key: (sum((x[key] - x["hit"]) ** 2 for x in rows) / len(rows), -sum(math.log(max(1e-12, x[key] if x["hit"] else 1 - x[key])) for x in rows) / len(rows))
            (b, ll), (bp, llp), (bi, lli) = score("predicted"), score("prior_predicted"), score("independent_predicted")
            bands = [(0, 0.3), (0.3, 0.5), (0.5, 0.7), (0.7, 1.01)]
            band_rows = {"predicted": [], "claims": [], "mean_predicted": [], "share_found": []}
            for lo, hi in bands:
                xs = [x for x in rows if lo <= x["predicted"] < hi]
                if xs:
                    band_rows["predicted"].append(f"{lo:.1f} to {min(hi, 1):.1f}"), band_rows["claims"].append(len(xs))
                    band_rows["mean_predicted"].append(sum(x["predicted"] for x in xs) / len(xs)), band_rows["share_found"].append(sum(x["hit"] for x in xs) / len(xs))
            out = [api.table("fared", {k: [x[k] for x in rows] for k in ("claim", "looked", "found", "posterior", "predicted", "prior_predicted")}, title="Each claim's prediction and what was found", roles={"name": "claim"}),
                   api.table("fared_by_band", band_rows, title="Predictions by how sure they were, against what was found", roles={"name": "predicted"},
                             show={"kind": "chart", "chart": "bar", "x": "predicted", "y": "share_found", "how": "sum"}),
                   api.table("fared_scores", {"prediction": ["the archive", "the prior alone", "repeated reports counted as independent"], "brier": [b, bp, bi], "log_loss": [ll, llp, lli]},
                             title="Scores of the predictions (lower is better)", roles={"name": "prediction"})]
            better = "better than" if b < bp else "no better than"
            out.append(api.note(f"On {len(rows)} claims looked at afterwards, the archive's predictions score {b:.3f} by Brier's rule and {ll:.3f} by log loss, {better} the prior alone "
                                f"({bp:.3f} and {llp:.3f}); counting repeated reports as independent scores {bi:.3f} and {lli:.3f}. "
                                + " ".join(f"Where it predicted {p}, {100 * s:.0f}% were found against {100 * m:.0f}% predicted." for p, m, s in zip(band_rows['predicted'], band_rows['mean_predicted'], band_rows['share_found']))))
            out.append(api.note("The check presumes that the state the archive is about lasted until the later looks, and that the looks have the declared channel; "
                                "a claim whose prediction misses may be wrong, or the state may have changed."))
            return out
    return Fared()
