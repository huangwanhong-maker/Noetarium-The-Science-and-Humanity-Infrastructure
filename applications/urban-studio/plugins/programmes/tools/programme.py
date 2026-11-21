"""A knowledge and learning programme read at a date: the needs of its cities (their scorecards' weaknesses among them) and which
activities reached them; each action plan's state and the pace of completion; the results reached on a declared ladder at a
sufficient grade of evidence; each contribution claim, no better shown than its weakest step and held back by any rival not
addressed; and the formats compared by what they contributed for what they cost."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _programmes as P  # noqa: E402
from science import programmes as pg  # noqa: E402


def create(api):
    class Programme(api.BoardAnalysis):
        id = "programme"
        title = "Read a learning programme, from needs to results"
        description = ("A programme for a cohort of cities read at a date: their needs, the weaknesses of their scorecards among them, and which "
                       "activities reached them; each action plan completed, in progress, overdue or dropped, and the pace of completion with open "
                       "actions counted as still open; the highest result each city reached on the ladder at a sufficient grade of evidence; each "
                       "contribution claim, no better shown than its weakest step and held back by any rival not addressed; and the formats "
                       "compared by the credit of substantiated claims for their cost. The example is invented.")
        params = [api.param("date", "Read on", "2026-10-15", kind="text", help="an ISO date")]

        def run(self, board, params, context):
            E = P.example()
            r = pg.assess(E["programme"], str(params.get("date") or "2026-10-15"), needs_from=P.needs_from(E))
            names = {p["id"]: p["name"] for p in E["programme"]["participants"]}
            n = r["needs"]
            out = [api.table("programme_needs", {"city": [names[x["participant"]] for x in n], "need": [x["text"] for x in n], "priority": [x["priority"] for x in n],
                                                 "reached by": [", ".join(x["activities"]) or "none" for x in n], "source": [x["source"] for x in n]},
                             title="Needs and the activities that reached them")]
            a = r["actions"]
            out.append(api.table("programme_actions", {"action": [x["action"] for x in a], "city": [names[x["participant"]] for x in a], "state": [x["state"] for x in a],
                                                       "why": [x["why"] for x in a]}, title=f"Action plans on {r['date']}",
                                 show={"kind": "chart", "chart": "bar", "x": "state", "y": "action", "how": "count"}))
            c = r["claims"]
            out.append(api.table("programme_claims", {"claim": [x["claim"] for x in c], "result": [x["result"] for x in c], "weakest step": [x["weakest"] or "not shown" for x in c],
                                                      "rivals not addressed": [", ".join(x["unaddressed"]) for x in c], "substantiated": ["yes" if x["substantiated"] else "no" for x in c],
                                                      "why": [x["why"] for x in c]}, title="Contribution claims"))
            f = r["formats"]
            out.append(api.table("programme_formats", {"format": [x["format"] for x in f], "cost": [x["cost"] for x in f], "credit": [round(x["credit"], 3) for x in f],
                                                       "credit per cost": [None if x["credit_per_cost"] is None else round(x["credit_per_cost"], 4) for x in f],
                                                       "on the frontier": ["yes" if x["frontier"] else "no" for x in f]},
                                 title="Formats: substantiated contribution for cost", show={"kind": "chart", "chart": "scatter", "x": "cost", "y": "credit"}))
            lad = {x["participant"]: x["highest"] for x in r["ladder"]}
            pace = r["pace"]
            share = r["completed_share"]
            said = (f"On {r['date']}: {len(r['unmet'])} need(s) no activity reached ({', '.join(r['unmet'])}); "
                    + "; ".join(f"{names[k]} completed {P.percent(v)} of its actions" for k, v in share.items()) + ". "
                    + ("Half the actions were completed within " + f"{pace['median_days']:.0f} days. " if pace.get("median_days") is not None else "Fewer than half the actions are completed yet, so no median time can be read. ")
                    + "The highest result at a sufficient grade: " + "; ".join(f"{names[k]} {v or 'none'}" for k, v in lad.items()) + ". "
                    + f"{sum(1 for x in c if x['substantiated'])} of {len(c)} contribution claims substantiated. "
                    + f"{r['asks']['answered']} of {r['asks']['asked']} asks answered. {E['said']}")
            out.append(api.note(said))
            return out

    return Programme()
