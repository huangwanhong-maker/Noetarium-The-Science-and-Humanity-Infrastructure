"""Time poverty: who has too little time left after paid and unpaid work, against a declared line."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402


def create(api):
    class TimePoverty(api.BoardAnalysis):
        id = "time_poverty"
        title = "Time poverty"
        description = ("Of people's committed time (paid work, its travel, unpaid work and care), the share above a line: by default 1.5 times the "
                       "median, after Bardasi and Wodon, or a number of minutes declared; with the gap, how far above the line the time-poor are, and "
                       "the rate by group. It asks for one row a person, as a time-use survey's diaries give.")
        params = [api.param("table", "People", "people", kind="text", role="table", help="one row a person"),
                  s.col(api, "committed", "Committed minutes", "committed", "paid and unpaid work in a day"),
                  s.col(api, "weight", "Weight", "", "a survey weight; empty for none"),
                  s.col(api, "group", "Group", "", "a column to give the rate by; empty for none"),
                  api.param("multiple", "Line, times the median", 1.5, kind="number", minimum=1, maximum=3, step=0.1),
                  api.param("line", "Line in minutes", 0, kind="number", minimum=0, maximum=1440, help="a declared line, used instead when above zero")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            c = s.nums(t, params["committed"])
            w = s.nums(t, params["weight"]) if params.get("weight") else [1.0] * t.rows
            keep = [i for i in range(t.rows) if c[i] is not None and w[i] is not None]
            line = float(params["line"]) or None
            whole = s.society.ask("time_poverty", committed=[c[i] for i in keep], weight=[w[i] for i in keep], multiple=float(params["multiple"]), **({"line": line} if line else {}))
            rows = {"group": ["all"], "people": [len(keep)], "rate": [round(whole["rate"], 4)], "gap_minutes": [round(whole["gap"], 1)]}
            if params.get("group"):
                g = s.texts(t, params["group"])
                for name in sorted({g[i] for i in keep}):
                    idx = [i for i in keep if g[i] == name]
                    a = s.society.ask("time_poverty", committed=[c[i] for i in idx], weight=[w[i] for i in idx], line=whole["line"])
                    rows["group"].append(name), rows["people"].append(len(idx)), rows["rate"].append(round(a["rate"], 4)), rows["gap_minutes"].append(round(a["gap"], 1))
            how = "declared" if line else f"{params['multiple']} times the median of {whole['median']:.0f}"
            return [api.table("time_poverty", rows, title="Time poverty by group, against one line", roles={"name": "group", "amount": "rate"},
                              show={"kind": "chart", "chart": "bar", "x": "group", "y": "rate", "how": "sum"}),
                    api.note(f"Against a line of {whole['line']:.0f} committed minutes a day ({how}), {s.share(whole['rate'])} are time-poor, "
                             f"{whole['gap']:.0f} minutes above it on average. Presumed: the line, which no survey gives.")]
    return TimePoverty()
