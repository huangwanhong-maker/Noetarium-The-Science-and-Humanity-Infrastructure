"""Who has the better of whom: a record's exchanges against one valuation of its kinds."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402


def create(api):
    class Reciprocity(api.BoardAnalysis):
        id = "reciprocity"
        title = "Who has the better of whom"
        description = ("The exchanges of a record against one valuation of its kinds: the prices that fit all its ratios best; how far each exchange stands from them; "
                       "and for each subject the sum of what it gained on those terms less what it gave. None of it changes with the units the kinds are counted in, "
                       "and the subjects' advantages add up to nothing. The valuation is a benchmark the record supplies, not a verdict: what counts as fair is "
                       "a frame's to say.")
        params = [*r.common(api),
                  api.param("unit", "Prices in", "", kind="text", help="the kind prices are stated in; the kind exchanged most when left out")]

        def run(self, board, params, context):
            s = r.stated(board, params)
            a = r.ask("reciprocity", record=s["record"])
            if a["exchanges"] == 0:
                raise ValueError("the record holds no exchange: an exchange is two movements of one event, in opposite directions, of two kinds")
            unit = r.kind_index(s, params.get("unit") or None)
            pot = np.array(a["potential"])
            subjects, kinds = s["subjects"], s["kinds"]
            adv, count = np.array(a["advantage"]), np.array(a["trades_of"])
            order = np.argsort(-adv)
            took = count > 0
            share = a["fitted"] / (a["fitted"] + a["unfitted"]) if a["fitted"] + a["unfitted"] > 0 else 1.0
            return [
                api.table("advantages", {"subject": [subjects[i] for i in order], "advantage": np.round(adv[order], 6), "exchanges": count[order],
                                         "a_exchange": [round(float(adv[i] / count[i]), 6) if count[i] else None for i in order]},
                          title="Each subject's advantage against the record's own valuation, in log units", roles={"name": "subject", "amount": "advantage"},
                          show={"kind": "chart", "chart": "bar", "x": "subject", "y": "advantage", "how": "sum", "most": 40}),
                api.table("valuation", {"kind": kinds, "price": np.round(np.exp(pot - pot[unit]), 6), "log_price": np.round(pot - pot[unit], 6)},
                          title=f"The valuation that fits the record's ratios best, in {kinds[unit]}", roles={"name": "kind", "amount": "price"}),
                api.table("terms", {"event": [s["events"][e] for e in a["event"]], "time": a["time"], "gave": [subjects[i] for i in a["giver"]], "took": [subjects[i] for i in a["taker"]],
                                    "of": [kinds[k] for k in a["k"]], "for": [kinds[k] for k in a["l"]], "ratio": np.round(np.exp(a["log_ratio"]), 6), "residual": np.round(a["residual"], 6)},
                          title="Each exchange: its ratio, and how far it stands from the valuation (above nothing: the giver had the better of it)",
                          roles={"key": "event", "amount": "residual", "name": "gave"}, show={"kind": "chart", "chart": "histogram", "y": "residual", "bins": 40}),
                api.note(f"{a['exchanges']:,} exchanges among {int(took.sum())} subjects in {len(kinds)} kinds. One valuation accounts for {100 * share:.1f}% of the variation of their ratios; "
                         f"what is left is each exchange's residual. {subjects[order[0]]} stands best against it ({adv[order[0]]:+.3f} over {count[order[0]]} exchanges), "
                         f"{subjects[order[-1]]} worst ({adv[order[-1]]:+.3f}). The advantages add up to {adv.sum():.1g}. "
                         "A residual says that an exchange departs from the record's own valuation, not why: a subsidy consented to, a quality unseen and a bargain forced read alike."),
            ]
    return Reciprocity()
