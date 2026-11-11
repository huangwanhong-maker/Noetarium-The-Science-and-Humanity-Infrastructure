"""How long things last: Kaplan and Meier's survival, by the period they began in."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parents[1] / "market"))
import _market as m  # noqa: E402
from science import markets  # noqa: E402


def create(api):
    class Survival(api.BoardAnalysis):
        id = "survival"
        title = "How long they last"
        description = ("The share still there after one, two, three periods, for things with a beginning and, for those that ended, an end (the lives of "
                       "'Openings and closings'), by Kaplan and Meier's estimate, which uses those still there for as long as they are seen. "
                       "In groups by when they began, with the log-rank test of whether the groups last alike.")
        params = [api.param("table", "Lives", "lives", kind="text", role="table", help="a row a thing, with its beginning as its time and its end as its until"),
                  api.param("cohorts", "Groups by beginning, each of", 5, kind="integer", minimum=0, maximum=100, help="periods to a group; 0 for one group of all"),
                  api.param("group", "Or groups by", "", kind="text", role="column", of="table", help="a column of kinds, in place of the beginning")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            began = t.column(m.role(t, "time", "its beginning"))
            ended = t.column(m.role(t, "until", "its end"))
            last = float(np.nanmax(np.where(np.isfinite(ended), ended, began)))
            # Things seen from the first period were open before the record began: how long they had lasted is not known, so they are left out.
            first = float(np.nanmin(began))
            keep = np.isfinite(began) & (began > first)
            event = np.isfinite(ended) & keep
            duration = np.where(event, ended - began, last + 1 - began)
            if params.get("group") and t.has(params["group"]):
                labels = t.text(params["group"])
            else:
                width = int(params.get("cohorts", 5))
                labels = ["all"] * t.rows if width <= 0 else [f"{int(first + 1 + ((b - first - 1) // width) * width)}–{int(min(last, first + ((b - first - 1) // width + 1) * width))}" if np.isfinite(b) else "" for b in began]
            kinds = sorted({labels[i] for i in np.flatnonzero(keep)})
            code = {k: n for n, k in enumerate(kinds)}
            index = np.flatnonzero(keep)
            if len(index) < 2:
                raise ValueError("too few things began after the first period to say how long they last")
            a = markets.ask("survival", time=[float(duration[i]) for i in index], event=[int(event[i]) for i in index], group=[code[labels[i]] for i in index])
            out = {"group": [], "after": [], "still_there": [], "lower": [], "upper": [], "at_risk": []}
            medians = []
            for curve in a["curves"]:
                name = kinds[curve["group"]]
                medians.append(f"{name}: half are gone after {curve['median']:g}" if curve["median"] is not None else f"{name}: more than half are still there")
                # The curve is a staircase: it is written at every whole period up to the longest the group was seen for, so that the
                # groups can be read against one another; between its steps it stands where it stood.
                longest = max(duration[i] for i in index if code[labels[i]] == curve["group"])
                step = {float(when): k for k, when in enumerate(curve["time"])}
                share, low, high, risk = 100.0, None, None, curve["n"]
                for after in range(0, int(longest) + 1):
                    k = step.get(float(after))
                    if k is not None:
                        share = round(100 * curve["survival"][k], 2)
                        low = None if curve["lower"][k] is None else round(100 * curve["lower"][k], 2)
                        high = None if curve["upper"][k] is None else round(100 * curve["upper"][k], 2)
                        risk = curve["at_risk"][k]
                    out["group"].append(name), out["after"].append(after), out["still_there"].append(share), out["lower"].append(low), out["upper"].append(high), out["at_risk"].append(risk)
            test = a.get("log_rank")
            return [
                api.table("survival", out, title="The share still there, by how long after they began", roles={"category": "group", "amount": "still_there"},
                          show={"kind": "chart", "chart": "line", "x": "after", "y": "still_there", "split": "group", "how": "sum"}),
                api.note(f"{len(index):,} things that began after {first:g}; {int(event.sum()):,} of them ended within the record. " + "; ".join(medians) + "."
                         + (f" The groups do not last alike (log-rank χ² {test['statistic']:.1f} on {test['df']:g} degrees, p {test['p']:.2g})." if test and test["p"] < 0.05
                            else f" No difference between the groups is shown (log-rank p {test['p']:.2g})." if test else "")
                         + " Those seen from the first period are left out: when they began is not known."),
            ]
    return Survival()
