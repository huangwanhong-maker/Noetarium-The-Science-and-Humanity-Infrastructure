"""Financing a city's flood losses: the modelled events met by layers of a contingency budget, a contingent credit and insurance, each
costed a year and in the year of a rare flood against leaving everything to be found after the event, and the cheapest tower."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _risk as r  # noqa: E402
from science import risk  # noqa: E402


def create(api):
    class Financing(api.BoardAnalysis):
        id = "financing"
        title = "How the city might meet its flood losses"
        description = ("Each source of funds takes a layer of every event's loss: a contingency budget at the bottom, a contingent credit above it, "
                       "insurance above that. What no layer pays is found after the flood, at a cost a dollar that rises with the amount. Each "
                       "strategy is costed a year on average and in the year of the 1-in-100-year flood, against leaving everything unfunded; the "
                       "cheapest tower of the three is found among boundaries at the losses of given return periods, leaving nothing of the "
                       "1-in-100-year flood unfunded. Every cost is a declaration of the parameters below, illustrative and no institution's. A "
                       "parametric cover needs an index for every event, which the catalogue does not give: it is refused, and none is invented.")
        params = [api.param("case", "Place", "freetown", kind="text", help="a place whose event catalogue the data library holds: freetown"),
                  api.param("budget", "Contingency budget (USD)", 1e6, minimum=0, step=1e5),
                  api.param("credit", "Contingent credit, up to (USD)", 1e7, minimum=0, step=1e6),
                  api.param("insured", "Insurance, up to (USD)", 6e7, minimum=0, step=1e6),
                  api.param("holding", "A budget's holding cost a year (share)", 0.03, minimum=0, maximum=0.2, step=0.005),
                  api.param("fee", "A credit's fee a year (share of its limit)", 0.005, minimum=0, maximum=0.05, step=0.001),
                  api.param("repaid", "A credit's cost a dollar drawn", 1.1, minimum=0.5, maximum=3, step=0.05),
                  api.param("multiple", "Insurance's premium (times its expected payment)", 1.8, minimum=1, maximum=5, step=0.1),
                  api.param("cheap", "Unfunded: a dollar up to $2 m costs", 1.2, minimum=1, maximum=3, step=0.05),
                  api.param("dear", "Unfunded: a dollar beyond costs", 2.0, minimum=1, maximum=5, step=0.1)]

        def run(self, board, params, context):
            H = risk.hazard_maps(str(params.get("case") or "freetown").strip())
            if not H.get("events"):
                raise ValueError("the data library holds no event catalogue for this place")
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            unf = [{"up_to": 2e6, "unit": g("cheap", 1.2)}, {"unit": g("dear", 2.0)}]
            B = {"name": "contingency budget", "kind": "budget", "rate": g("holding", 0.03), "unit": 1}
            C = {"name": "contingent credit", "kind": "credit", "rate": g("fee", 0.005), "unit": g("repaid", 1.1)}
            I = {"name": "insurance", "kind": "insurance", "fixed": 50000, "multiple": g("multiple", 1.8)}
            b, c, i = g("budget", 1e6), g("credit", 1e7), g("insured", 6e7)
            m = lambda v: f"${v / 1e6:.1f} m"  # noqa: E731
            strategies = [{"name": f"a contingency budget of {m(b)}", "unfunded": unf, "instruments": [dict(B, attachment=0, limit=b)]}]
            if c > b:
                strategies.append({"name": f"and a contingent credit to {m(c)}", "unfunded": unf, "instruments": [dict(B, attachment=0, limit=b), dict(C, attachment=b, limit=c - b)]})
            if i > c > b:
                strategies.append({"name": f"and insurance from {m(c)} to {m(i)}", "unfunded": unf,
                                   "instruments": [dict(B, attachment=0, limit=b), dict(C, attachment=b, limit=c - b), dict(I, attachment=c, limit=i - c)]})
            E = H["events"]
            a = risk.ask("financing", losses=E["losses"], years=E["catalogue_years"], strategies=strategies, return_periods=[10, 50, 100, 500],
                         tower={"order": [B, C, I], "candidates": [2, 5, 10, 20, 50, 100, 200, 500, 1000], "unfunded": unf, "cap_period": 100, "cap": 0},
                         parametric={"attachment": c, "limit": max(i - c, 1), "multiple": g("multiple", 1.8)})
            rows = [a["baseline"]] + a["strategies"]
            at = lambda s, T: next((q for q in s["at"] if q["return_period"] == T), {})  # noqa: E731
            out = [api.table("financing_strategies", {
                "strategy": [s["name"] for s in rows], "expected_cost_a_year": [s["expected"] for s in rows],
                "saved_a_year": [None] + a["saving"], "funded_share": [s["funded_share"] for s in rows],
                "unfunded_1_in_100": [at(s, 100).get("unfunded") for s in rows], "cost_in_the_1_in_100_year": [at(s, 100).get("cost") for s in rows],
                "cost_in_the_1_in_500_year": [at(s, 500).get("cost") for s in rows]},
                title="Strategies of financing, costed a year and in the year of a rare flood (USD; costs declared)",
                show={"kind": "chart", "chart": "bar", "x": "strategy", "y": "expected_cost_a_year", "how": "sum"})]
            T = a["tower"]
            out.append(api.table("financing_tower", {"instrument": [l["name"] for l in T["layers"]], "from_usd": [l["attachment"] for l in T["layers"]],
                                                     "to_usd": [None if l["limit"] is None else l["attachment"] + l["limit"] for l in T["layers"]]},
                                 title=f"The cheapest tower leaving nothing of the 1-in-100-year flood unfunded ({T['tried']} tried, {T['feasible']} met the cap)"))
            out.append(api.note(f"The {E['count']:,} modelled events cost {m(a['expected_loss'])} a year on average over {E['catalogue_years']:,} simulated years. "
                                f"The cheapest tower costs {m(T['cost']['expected'])} a year. {(a['parametric']['refused'][0].upper() + a['parametric']['refused'][1:] + '.') if a['parametric'].get('refused') else ''} Every cost here is declared; "
                                f"the answer turns on them. {H['credit']}"))
            return out

    return Financing()
