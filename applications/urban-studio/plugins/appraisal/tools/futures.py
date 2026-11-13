"""Futures that compute: a Delphi panel's rounds and when it agrees, and the configurations a morphological field leaves open."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _appraisal as a  # noqa: E402


def create(api):
    class Futures(api.BoardAnalysis):
        id = "futures"
        title = "Delphi rounds and morphological fields"
        description = ("Two methods of futures studies that compute, neither a forecast. A Delphi panel's estimates of one quantity, round by round: "
                       "the median and interquartile range of each round, and whether the panel has come to agree (a declared bound on the "
                       "spread) and settled (a declared bound on how far the median moves). A morphological field: parameters, each with its "
                       "values, and the pairs of values that cannot hold together; what is left is every configuration with no such pair.")
        params = [api.param("method", "Method", "Delphi rounds", kind="choice", options=["Delphi rounds", "morphological field"]),
                  api.param("table", "Table", "panel", kind="text", role="table", help="Delphi: one row an estimate (round, estimate); field: one row a value (parameter, value)"),
                  a.col(api, "group", "Round, or parameter", "round"),
                  a.col(api, "value", "Estimate, or value", "estimate"),
                  api.param("exclusions", "Exclusions (field)", "", kind="text", role="table", help="one row a pair that cannot hold: parameter_a, value_a, parameter_b, value_b"),
                  api.param("consensus", "Consensus: IQR / median at most", 0.25, kind="number", minimum=0),
                  api.param("stability", "Stability: the median moves at most", 0.05, kind="number", minimum=0)]

        def run(self, board, params, context):
            t = board.table(params["table"])
            if params["method"] == "Delphi rounds":
                g, v = a.nums(t, params["group"]), a.nums(t, params["value"])
                rounds = sorted({int(x) for x in g if x is not None})
                data = [[val for gg, val in zip(g, v) if gg is not None and int(gg) == r and val is not None] for r in rounds]
                res = a.appraisal.ask("delphi", rounds=data, consensus=float(params["consensus"]), stability=float(params["stability"]))["rounds"]
                rows = {"round": rounds, "panel": [x["panel"] for x in res], "median": [x["median"] for x in res], "q1": [x["q1"] for x in res],
                        "q3": [x["q3"] for x in res], "relative_iqr": [x["relative_iqr"] for x in res], "consensus": ["yes" if x["consensus"] else "no" for x in res],
                        "stable": ["yes" if x["stable"] else "no" for x in res]}
                agreed = next((r for r, x in zip(rounds, res) if x["consensus"] and x["stable"]), None)
                return [api.table("delphi_rounds", rows, title="The panel's rounds", roles={"time": "round", "amount": "median"},
                                  show={"kind": "chart", "chart": "line", "x": "round", "y": "median", "how": "sum"}),
                        api.note((f"The panel agrees and has settled by round {agreed}, on a median of {res[rounds.index(agreed)]['median']:g}."
                                  if agreed is not None else f"After {len(rounds)} rounds the panel has not both agreed and settled; its last median is {res[-1]['median']:g}.")
                                 + " Agreement is not truth: a panel can converge on what none of its members could check.")]
            par, val = a.texts(t, params["group"]), a.texts(t, params["value"])
            parameters = []
            for p in par:
                if p and p not in parameters:
                    parameters.append(p)
            values = [[v for pp, v in zip(par, val) if pp == p and v] for p in parameters]
            excluded = []
            if str(params.get("exclusions") or "").strip():
                e = board.table(params["exclusions"])
                cols = [a.texts(e, c) for c in ("parameter_a", "value_a", "parameter_b", "value_b")]
                for pa, va, pb, vb in zip(*cols):
                    if not pa:
                        continue
                    i, j = parameters.index(pa), parameters.index(pb)
                    excluded.append([i, values[i].index(va), j, values[j].index(vb)])
            res = a.appraisal.ask("morphology", parameters=parameters, values=values, excluded=excluded, keep=2000)
            rows = {p: [c[k] for c in res["configurations"]] for k, p in enumerate(parameters)}
            return [api.table("configurations", rows, title=f"The configurations the field leaves open ({res['count']:,} of {res['size']:,})"),
                    api.note(f"Of {res['size']:,} configurations of {len(parameters)} parameters, {res['count']:,} contain no pair declared impossible "
                             f"({len(excluded)} exclusions). What is left is what the exclusions allow, not what will be.")]
    return Futures()
