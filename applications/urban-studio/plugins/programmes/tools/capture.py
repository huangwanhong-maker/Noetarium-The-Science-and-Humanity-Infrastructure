"""What a public project could take back from the land it raises in value, and how land readjustment would pay for itself: each declared
instrument (a betterment levy, development rights sold by a formula, tax increment, a developer obligation) with its payments, its
present value, the share of the project's cost it recovers and the share of the counterfactual uplift it takes, kept apart and beside
the statutory rate where one is declared; what tax increment diverts from each overlapping jurisdiction; the parcels whose uplift is
taken more than once over; and a readjustment's area identity, contribution ratio, whether the owners' land keeps its value, the
reserve land's revenue and the project's balance, with the replotting that gives each owner the nearest plot and the settlement that
evens out what each receives against what each is due."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _programmes as P  # noqa: E402
from science import fiscal as fs  # noqa: E402


def create(api):
    class Capture(api.BoardAnalysis):
        id = "capture"
        title = "What a project could take back from the land it raises, and land readjustment"
        description = ("A public project's uplift on the parcels it touches, as declared values with and without it, and what each declared "
                       "instrument would take back: a betterment levy, development rights sold by a formula, tax increment with only the "
                       "project's share of the increment counted as uplift, a developer obligation on new floor area. For each, the share of "
                       "the cost it recovers and the share of the uplift it takes, kept apart, beside a statutory rate; what tax increment "
                       "diverts from each jurisdiction; the parcels charged more than their uplift. Then land readjustment: the areas, the "
                       "contribution ratio, whether the owners' land keeps its value, the reserve land's revenue and the project's balance, and "
                       "the replotting by the least total distance with each owner's settlement. The project, its parcels and every rate are "
                       "invented and declared.")
        params = [api.param("share", "Levy's share of the uplift", 0.3, minimum=0.0, maximum=1.0, step=0.05),
                  api.param("but_for", "Project's share of the tax increment", 0.6, minimum=0.0, maximum=1.0, step=0.05),
                  api.param("rate", "Discount rate", 0.04, minimum=0.0, step=0.005)]

        def run(self, board, params, context):
            E = P.example()
            J = E["project"]
            rate = float(params.get("rate") if params.get("rate") is not None else J["rate"])
            ins = {k: dict(v) for k, v in J["instruments"].items()}
            if params.get("share") is not None:
                ins["levy"]["share"] = float(params["share"])
            if params.get("but_for") is not None:
                ins["increment"]["but_for"] = float(params["but_for"])
            parcels = [fs.parcel(p["id"], p["area"], p["without"], p["with"], p.get("frontage", 0), p.get("x", 0), p.get("y", 0)) for p in J["parcels"]]
            r = fs.capture_compare(parcels, J["cost"], ins, rate=rate, statutory=J.get("statutory"))
            rows = r["instruments"]
            unit = J.get("unit", "")
            out = [api.table("capture_instruments", {
                "instrument": [x["instrument"] for x in rows],
                "present value": [round(x["pv"], 1) for x in rows],
                "cost recovered": [None if x["cost_recovery"] is None else round(x["cost_recovery"], 3) for x in rows],
                "uplift taken": [None if x["capture"] is None else round(x["capture"], 3) for x in rows],
                "statutory": [None if x["statutory"] is None else round(x["statutory"], 3) for x in rows],
                "paid by": ["; ".join(f"{k} {v:,.0f}" for k, v in x["by_payer"].items()) for x in rows]},
                title=f"What each instrument takes back ({unit}, present values at {100 * rate:.1f}%)",
                show={"kind": "chart", "chart": "bar", "x": "instrument", "y": "cost recovered", "how": "sum"})]
            inc = r.get("increment") or {}
            if inc:
                by = inc.get("diverted_by_jurisdiction") or {}
                out.append(api.table("capture_diverted", {"jurisdiction": [k.split(":", 1)[-1] for k in by], "diverted": [round(v, 1) for v in by.values()]},
                                     title="What tax increment diverts from each jurisdiction (present values)"))
            R = J["readjustment"]
            q = fs.readjust(R["areas"], R["value_before"], R["value_after"], R["costs"], R["sale_year"], rate=R.get("rate", rate), cap=R.get("cap"),
                            selling_cost=R.get("selling_cost", 0), subsidies=R.get("subsidies"), owners=R.get("owners"), plots=R.get("plots"))
            rp = q.get("replotting") or {}
            if rp:
                a = rp["assignments"]
                out.append(api.table("capture_replotting", {"owner": [x["owner"] for x in a], "plot": [x["plot"] for x in a], "due": [round(x["entitlement"], 1) for x in a],
                                                            "received": [round(x["received"], 1) for x in a], "settlement": [round(x["settlement"], 1) for x in a]},
                                     title="Land readjustment: each owner's plot, what each is due and receives, and the settlement"))
            taken = sum(x["capture"] or 0 for x in rows)
            recovered = sum(x["cost_recovery"] or 0 for x in rows)
            dc = r["double_charges"]
            text = (f"The project costs {r['pv_cost']:,.0f} and raises the parcels it touches by {r['pv_uplift']:,.0f} ({unit}). "
                    + "; ".join(f"{x['instrument']} recovers {P.percent(x['cost_recovery'])} of the cost and takes {P.percent(x['capture'])} of the uplift" for x in rows) + ". "
                    + f"Together they would recover {P.percent(recovered)} of the cost and take {P.percent(taken)} of the uplift"
                    + (", more than the uplift there is" if taken > 1 else "") + ". "
                    + (("Charged more than its uplift: " + "; ".join(f"{d['parcel']} ({d['taken']:,.0f} taken of {d['uplift']:,.0f})" for d in dc) + ". ") if dc else "No parcel is charged more than its uplift. ")
                    + f"Readjustment: the areas {'balance' if q['areas_balance'] else 'leave ' + format(q['imbalance'], ',.0f') + ' m² unaccounted'}; the owners give "
                    + f"{P.percent(q['contribution'])} of their land ({P.percent(q['public_share'])} to public use, {P.percent(q['reserve_share'])} as reserve), "
                    + f"{'keeping' if q['value_kept'] else 'losing'} value ({q['kept']:.3f} of it), {'within' if q['within_cap'] else 'beyond'} the declared cap; "
                    + f"the reserve land sells for {q['reserve_revenue']:,.0f} and the project's balance is {q['pv_balance']:,.0f}, "
                    + f"{'feasible' if q['feasible'] else 'not feasible'}. "
                    + "Capital revenue such as this does not raise the operating surplus from which debt is serviced. " + J["said"])
            out.append(api.note(text))
            return out

    return Capture()
