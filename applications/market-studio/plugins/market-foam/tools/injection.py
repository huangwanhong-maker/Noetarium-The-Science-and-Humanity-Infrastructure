"""New money, and who gains: an addition at one subject followed outward along the record's own paths of spending."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402


def create(api):
    class Injection(api.BoardAnalysis):
        id = "injection"
        title = "New money, and who gains"
        description = ("Money added at one subject of an economy at rest, and followed outward. Each subject spends what it holds at one rate, in the shares in which the "
                       "record shows it paying the others, and each sells a fixed output at the price that clears it. In the end every price has risen alike and "
                       "nothing real has changed; on the way there, those the new money reaches first buy at the old prices. The tool gives each subject's gain "
                       "over the whole passage, and when its price has made half its rise. The shares are read from the record; the rest is a model, and says so.")
        params = [*r.common(api),
                  api.param("of", "Money is the kind", "", kind="text", help="the kind paid with; the kind moved most often when left out"),
                  api.param("at", "Added at", "", kind="text", help="a subject; the one that pays most when left out"),
                  api.param("added", "Added, as a share of one balance", 0.01, minimum=0.0001, maximum=5, step=0.01),
                  api.param("mutual", "Take payments as mutual", False, kind="boolean", help="count what passed between two subjects in either direction as spending both ways")]

        def run(self, board, params, context):
            s = r.stated(board, params)
            k = r.kind_index(s, params.get("of") or None)
            rec, subjects = s["record"], s["subjects"]
            n = len(subjects)
            flow = np.zeros((n, n))
            for i, j, kind, q in zip(rec["from"], rec["to"], rec["kind"], rec["quantity"]):
                if kind == k:
                    flow[i, j] += abs(q)
            if params.get("mutual"):
                flow = flow + flow.T
            # The subjects that both pay and are paid: the others hold no place in a round of spending.
            keep = np.arange(n)
            while True:
                inner = flow[np.ix_(keep, keep)]
                alive = (inner.sum(axis=1) > 0) & (inner.sum(axis=0) > 0)
                if alive.all():
                    break
                keep = keep[alive]
            if len(keep) < 3:
                raise ValueError(f"fewer than three subjects both pay and are paid in {s['kinds'][k]}: there is no round of spending to follow")
            if len(keep) > 400:
                raise ValueError(f"{len(keep)} subjects pay and are paid: more than this tool follows (400). Take a slice, or a region")
            inner = flow[np.ix_(keep, keep)]
            shares = inner / inner.sum(axis=1, keepdims=True)
            names = [subjects[i] for i in keep]
            wanted = str(params.get("at") or "").strip()
            if wanted and wanted not in names:
                raise LookupError(f"'{wanted}' is not among the subjects that pay and are paid in {s['kinds'][k]}")
            site = names.index(wanted) if wanted else int(np.argmax(inner.sum(axis=1)))
            m = len(keep)
            context.progress(0.4, "following the money")
            try:
                a = r.ask("injection", shares=shares.tolist(), site=site, money=float(m), added=float(params.get("added", 0.01)), horizon=400.0, steps=8000)
            except Exception as error:
                if "irreducible" in str(error):
                    raise ValueError("the payments do not join all the subjects into one round (money that reaches some never returns): take payments as mutual, or a slice in which they do") from error
                raise
            gain = np.array(a["gain"])
            order = np.argsort(-gain)
            first = a.get("first_order")
            rise = np.array([np.nan if v is None else v for v in a["half_rise"]], dtype=float)
            columns = {"subject": [names[i] for i in order], "gain": np.round(gain[order], 8),
                       "share_from_the_source": np.round(shares[site, order], 4), "price_half_risen_at": [None if np.isnan(rise[i]) else round(float(rise[i]), 4) for i in order]}
            if first:
                columns["gain_to_first_order"] = np.round(np.array(first)[order], 8)
            gainers = int((gain > 1e-12).sum())
            return [
                api.table("gains", columns, title=f"New money at {names[site]}: each subject's gain over the passage, in units of one subject's output a unit of time",
                          roles={"name": "subject", "amount": "gain"}, show={"kind": "chart", "chart": "bar", "x": "subject", "y": "gain", "how": "sum", "most": 40}),
                api.note(f"An addition of {float(params.get('added', 0.01)):g} of one balance at {names[site]}, among {m} subjects that pay and are paid in {s['kinds'][k]}. "
                         f"{names[site]} gains {gain[site]:.3g}; of the {m}, {gainers} gain{'s' if gainers == 1 else ''} and {m - gainers} lose{'s' if m - gainers == 1 else ''}; together they gain {gain.sum():.1g}: what is consumed is what is produced at every moment "
                         f"(greatest departure {a['conserved']:.1g}). Every price ends {100 * (a['price_after'][0] / a['price_before'][0] - 1):.3g}% higher. "
                         + ("At rest the subjects hold alike. " if a["balanced"] else "At rest the subjects hold unequal balances, in proportion to what the shares return to each. ")
                         + "Presumed: each subject spends at one rate and sells a fixed output at the price that clears it at once. The record gives the shares only."),
            ]
    return Injection()
