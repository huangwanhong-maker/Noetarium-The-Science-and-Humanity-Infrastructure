"""How the whole place grew: its people over time as one quantity, by exponential, logistic and Gompertz's growth."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _population as p  # noqa: E402


def create(api):
    class Growth(api.BoardAnalysis):
        id = "growth"
        title = "How the whole place grew"
        description = ("Everyone in the place as one quantity, learned by three laws of growth: exponential (Malthus), logistic (Verhulst) and Gompertz's. "
                       "Each is fitted on the years before those kept aside, judged on them, fitted again on every year recorded, and run on to the year asked. "
                       "A law whose growth accelerates has no ceiling, and is drawn up to where it runs away.")
        params = p.case_params(api) + [api.param("held_out", "Years kept aside (epochs)", 2, minimum=1, maximum=4, step=1),
                                       api.param("to", "Run on to", 2050, minimum=2030, maximum=2100, step=5)]

        def run(self, board, params, context):
            f, case = p.field_of(params)
            g = p.pd.ask("growth", field=f, held_out=int(params.get("held_out") or 2), to=float(params.get("to") or 2050))
            years = sorted({y for m in g["models"] for y in (m.get("curve") or {}).get("years", [])} | set(g["years"]))
            cols = {"year": years, "recorded": [g["people"][g["years"].index(y)] if y in g["years"] and not g["projected"][g["years"].index(y)] else None for y in years],
                    "projected_by_the_JRC": [g["people"][g["years"].index(y)] if y in g["years"] and g["projected"][g["years"].index(y)] else None for y in years]}
            words = []
            for m in g["models"]:
                c = m.get("curve") or {"years": [], "people": []}
                at = dict(zip(c["years"], c["people"]))
                cols[m["name"]] = [p.num(at.get(y)) for y in years]
                if m.get("error"):
                    words.append(f"{m['title']}: {m['error']}.")
                    continue
                r = m["reading"]
                s = f"{m['title']}: {p.pct(r['rate_a_year'])} a year"
                if r.get("ceiling"):
                    s += f" toward {r['ceiling']:,.0f}"
                if r.get("accelerating"):
                    s += ", accelerating with no ceiling"
                if m.get("runs_away"):
                    s += f", running away before {m['runs_away']:.0f}"
                s += f"; on the years kept aside off by {p.pct(m['held_out']['relative_rmse'])}."
                words.append(s)
            out = [api.table("growth", cols, title=f"{f['title'].split(':')[0]}: its people, recorded and by three laws",
                             show={"kind": "chart", "chart": "line", "x": "year", "y": "recorded", "how": "sum"})]
            out.append(api.note(f"Fitted on {g['fitted_on'][0]:.0f}–{g['fitted_on'][-1]:.0f}, judged on {', '.join(f'{y:.0f}' for y in g['held_out'])}. " + " ".join(words)))
            out.append(api.note("A law of the whole knows nothing of why a place grows (births, deaths, who moves in and out); a change of regime in those "
                                "is a change the law cannot foresee, and the years kept aside say how far that mattered."))
            return out
    return Growth()
