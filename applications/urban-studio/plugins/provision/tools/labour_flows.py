"""A labour market's flows: the unemployment its flows would hold steady, quarter by quarter, and which flow made it change."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402

ORDER = ["EMP", "UNE", "INAC"]


def create(api):
    class LabourFlows(api.BoardAnalysis):
        id = "labour_flows"
        title = "A labour market's flows"
        description = ("People moving between employment, unemployment and inactivity each quarter, read as a chain: the unemployment and "
                       "participation the flows would hold steady, how long a spell of unemployment lasts, how fast the market settles, and the "
                       "change of the steady unemployment rate between two quarters split exactly among the six flows.")
        params = [api.param("table", "Flows", "labour_flows", kind="text", role="table", help="one row a flow: place, sex, quarter, from, to, persons"),
                  api.param("place", "Place", "FR", kind="text", help="the place's code"),
                  api.param("sex", "Sex", "T", kind="text"),
                  api.param("before", "Quarter before", "2019-Q4", kind="text"),
                  api.param("after", "Quarter after", "2020-Q2", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            cells = {}
            for g, sx, q, a, b, n in zip(s.texts(t, "country"), s.texts(t, "sex"), s.texts(t, "quarter"), s.texts(t, "from"), s.texts(t, "to"), s.nums(t, "thousands")):
                if g == params["place"] and sx == params["sex"] and n is not None:
                    cells.setdefault(q, {})[(a, b)] = n
            quarters = [q for q in sorted(cells) if len(cells[q]) == 9]
            if not quarters:
                raise ValueError(f"no quarter of {params['place']} holds all nine flows")
            flows = [[cells[q][(a, b)] for a in ORDER for b in ORDER] for q in quarters]
            compare = None
            if params["before"] in quarters and params["after"] in quarters:
                compare = [quarters.index(params["before"]), quarters.index(params["after"])]
            a = s.society.ask("labour", periods=quarters, flows=flows, **({"compare": compare} if compare else {}))
            per = a["periods"]
            rows = {"quarter": quarters, "steady_unemployment": [round(p["unemployment_rate"], 4) for p in per],
                    "steady_participation": [round(p["participation_rate"], 4) for p in per], "spell_unemployed": [round(p["spell_u"], 2) for p in per],
                    "half_life": [round(p["half_life"], 2) if p["half_life"] is not None else None for p in per],
                    "finding": [round(p["p"][2], 4) for p in per], "separation": [round(p["p"][0], 4) for p in per]}
            out = [api.table("labour_steady", rows, title=f"{params['place']}: the unemployment its flows would hold, quarter by quarter",
                             roles={"time": "quarter", "amount": "steady_unemployment"}, show={"kind": "chart", "chart": "line", "x": "quarter", "y": "steady_unemployment", "how": "sum"})]
            lo, hi = min(rows["steady_unemployment"]), max(rows["steady_unemployment"])
            words = (f"In {params['place']} the flows of {len(quarters)} quarters would hold unemployment between {s.share(lo)} and {s.share(hi)}; a spell of "
                     f"unemployment lasts {min(rows['spell_unemployed']):.1f} to {max(rows['spell_unemployed']):.1f} quarters, and the market settles with a "
                     f"half-life of about {sorted(h for h in rows['half_life'] if h is not None)[len(quarters) // 2]:.1f} quarters.")
            if "contributions" in a:
                c = a["contributions"]
                out.append(api.table("labour_contributions", {"flow": c["flows"], "contribution": [round(x, 5) for x in c["by_flow"]]},
                                     title=f"{params['before']} to {params['after']}: the change of steady unemployment by flow",
                                     roles={"name": "flow", "amount": "contribution"}, show={"kind": "chart", "chart": "bar", "x": "flow", "y": "contribution", "how": "sum"}))
                k = max(range(6), key=lambda i: abs(c["by_flow"][i]))
                words += (f" From {params['before']} to {params['after']} the steady rate moved from {s.share(c['before'])} to {s.share(c['after'])}; the flow "
                          f"{c['flows'][k]} accounts for {c['by_flow'][k] * 100:+.2f} points of the {c['change'] * 100:+.2f}.")
            out.append(api.note(words + " A chain of three states forgets how long a person has been out of work; the steady state is what one quarter's flows would hold, not a forecast."))
            return out
    return LabourFlows()
