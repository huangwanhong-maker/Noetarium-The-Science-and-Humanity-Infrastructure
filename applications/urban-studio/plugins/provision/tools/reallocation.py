"""Time moved between activities: an outcome regressed on the day as a composition, and what moving minutes from one activity to another goes with."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402


def create(api):
    class Reallocation(api.BoardAnalysis):
        id = "reallocation"
        title = "Time moved between activities"
        description = ("A day holds 1440 minutes, so an activity cannot grow without others shrinking. An outcome (wellbeing, health, a score) "
                       "is regressed on the day's log-ratios, and the table shows what moving minutes to each activity from each other goes with, "
                       "with an interval for each: the isotemporal substitution table. It needs one row a person (or a unit) with their minutes and "
                       "their outcome; what it finds are associations, causal only under a design's assumptions.")
        params = [api.param("table", "Days", "days", kind="text", role="table", help="one row a person, with minutes by part of the day and an outcome"),
                  api.param("parts", "Parts of the day", "", kind="text", role="columns", of="table", help="the columns of minutes, together making the day"),
                  s.col(api, "outcome", "Outcome", "outcome"),
                  api.param("covariates", "Covariates", "", kind="text", role="columns", of="table", help="columns of numbers held fixed (optional)"),
                  api.param("minutes", "Minutes moved", 30, kind="number", minimum=1, maximum=600),
                  api.param("resolution", "Diary slot (minutes)", 10, kind="number", minimum=0.1,
                            help="a part with no time recorded is taken to hold 0.65 of a slot (declared)")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            names = [p.strip() for p in str(params["parts"]).split(",") if p.strip()]
            if len(names) < 2:
                raise ValueError("name at least two columns of minutes as the parts of the day")
            covs = [p.strip() for p in str(params["covariates"] or "").split(",") if p.strip()]
            cols = [s.nums(t, n) for n in names]
            ys = s.nums(t, params["outcome"])
            zs = [s.nums(t, c) for c in covs]
            days, y, z = [], [], []
            for i in range(len(ys)):
                row = [c[i] for c in cols]
                extra = [c[i] for c in zs]
                if ys[i] is None or any(v is None for v in row) or any(v is None for v in extra) or sum(row) <= 0:
                    continue
                days.append(row), y.append(ys[i]), z.append(extra)
            if len(days) < len(names) + len(covs) + 2:
                raise ValueError(f"{len(days)} complete rows are too few for {len(names)} parts and {len(covs)} covariates")
            req = {"parts": names, "days": days, "outcome": y, "minutes": float(params["minutes"]), "resolution": float(params["resolution"])}
            if covs:
                req["covariates"] = z
            a = s.society.ask("reallocation", **req)
            m = int(params["minutes"]) if float(params["minutes"]).is_integer() else params["minutes"]
            rows = {"to": [], "from": [], "change": [], "lower": [], "upper": [], "p": []}
            for i, to in enumerate(names):
                for j, fr in enumerate(names):
                    if i == j or a["change"][i][j] is None:
                        continue
                    rows["to"].append(to), rows["from"].append(fr)
                    rows["change"].append(a["change"][i][j]), rows["lower"].append(a["lower"][i][j])
                    rows["upper"].append(a["upper"][i][j]), rows["p"].append(a["p"][i][j])
            if not rows["to"]:
                raise ValueError(f"no part of the reference day holds more than {m} minutes to give")
            k = max(range(len(rows["to"])), key=lambda q: rows["change"][q])
            j = min(range(len(rows["to"])), key=lambda q: rows["change"][q])
            sure = sum(1 for lo, hi in zip(rows["lower"], rows["upper"]) if lo > 0 or hi < 0)
            ref = ", ".join(f"{n} {v:.0f}" for n, v in zip(names, a["reference"]))
            return [api.table("reallocations", rows, title=f"{m} minutes moved to one activity from another: the change in {params['outcome']}",
                              roles={"amount": "change"}, show={"kind": "chart", "chart": "bar", "x": "to", "y": "change", "how": "mean"}),
                    api.note(f"From the sample's central day ({ref} minutes), {m} minutes moved to {rows['to'][k]} from {rows['from'][k]} go with the largest "
                             f"rise in {params['outcome']}, {rows['change'][k]:+.3g} ({rows['lower'][k]:+.3g} to {rows['upper'][k]:+.3g}); to {rows['to'][j]} from "
                             f"{rows['from'][j]}, the largest fall, {rows['change'][j]:+.3g}. {sure} of {len(rows['to'])} moves have intervals that exclude no change. "
                             f"The day's log-ratios explain {s.share(a['r_squared'])} of the outcome's variance across {a['n']:.0f} rows."),
                    api.note("An association, not an effect: the model compares people whose days differ, it does not move anyone's time. The moves are "
                             "evaluated by the linear model in isometric log-ratios, exact for that model; a part a diary left empty is taken to hold 0.65 "
                             f"of a {params['resolution']}-minute slot.")]
    return Reallocation()
