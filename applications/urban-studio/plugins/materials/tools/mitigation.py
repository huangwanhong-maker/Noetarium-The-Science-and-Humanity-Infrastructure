"""What a community's measures against its emissions would abate, at what cost a tonne, and whether they reach its target: each measure
alone, in the order of the abatement cost curve, and by its Shapley value over every order, so that two measures on one source are not
both credited with what only one of them can save; then the years ahead with activities growing, measures ramping to full effect, the
gap to a declared target and the cumulative against a declared budget (ADR 0147)."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _emissions as E  # noqa: E402


def create(api):
    class Mitigation(api.BoardAnalysis):
        id = "mitigation"
        title = "Measures against a community's emissions: the abatement cost curve and the way to a target"
        description = ("An inventory and a table of measures, each scaling by (1 − r) what remains of an activity or a factor in the entries it "
                       "acts on, with a yearly cost, a first year and years to full effect. Each measure's abatement alone, in the order of the "
                       "curve (by cost a tonne alone, unless an order is given) and by its Shapley value; their joint abatement and the overlap "
                       "that crediting each alone counts twice. Then the years ahead with activities growing at declared rates, against a "
                       "declared reduction by a target year and a cumulative budget. Every rate, cost and target is declared.")
        params = E.inventory_params(api) + E.measure_params(api) + [
            api.param("level", "Level", "basic+", kind="choice", options=["basic", "basic+", "territorial"]),
            api.param("order", "Order of the curve (ids, comma-separated; empty: by cost a tonne)", "", kind="text"),
            api.param("growth_energy", "Growth of stationary energy a year", 0.01, step=0.005),
            api.param("growth_transport", "Growth of transportation a year", 0.015, step=0.005),
            api.param("growth_waste", "Growth of waste a year", 0.01, step=0.005),
            api.param("last", "Last year", 2035, step=1), api.param("target_year", "Target year", 2030, step=1),
            api.param("target", "Target reduction from the inventory's year", 0.5, minimum=0, maximum=1, step=0.05),
            api.param("budget", "Cumulative budget (t CO2e; 0: none)", 0, minimum=0)]

        def run(self, board, params, context):
            I = E.inventory_from(board, params)
            M = E.measures_from(board, params)
            level = params.get("level") or "basic+"
            order = [x.strip() for x in str(params.get("order") or "").split(",") if x.strip()]
            C = E.em.curve(I, M, level=level, order=order)
            rows = C["measures"]
            r1 = lambda x: None if x is None else round(x, 1)  # noqa: E731
            out = [api.table("mitigation_curve", {
                "position": [a["position"] for a in rows], "measure": [a["name"] for a in rows],
                "abated alone (t)": [r1(a["alone"]) for a in rows], "abated in this order (t)": [r1(a["in_order"]) for a in rows],
                "Shapley (t)": [r1(a["shapley"]) for a in rows], "cost a year": [r1(a["cost"]) for a in rows],
                "cost a tonne alone": [r1(a["cost_per_tonne_alone"]) for a in rows], "cost a tonne in this order": [r1(a["cost_per_tonne_in_order"]) for a in rows],
                "cost a tonne, Shapley": [r1(a["cost_per_tonne_shapley"]) for a in rows]},
                title="The abatement cost curve: each measure alone, in its order, and by its Shapley value",
                show={"kind": "chart", "chart": "bar", "x": "measure", "y": "Shapley (t)", "how": "sum"})]
            growth = {"stationary energy": float(params.get("growth_energy") or 0), "transportation": float(params.get("growth_transport") or 0),
                      "waste": float(params.get("growth_waste") or 0)}
            target = {"year": int(params.get("target_year") or 0), "reduction": float(params.get("target") or 0)}
            if float(params.get("budget") or 0) > 0:
                target["budget"] = float(params["budget"])
            P = E.em.pathway(I, M, growth, int(params.get("last") or I["year"] + 10), target=target, level=level)
            ys = P["years"]
            out.append(api.table("mitigation_pathway", {"year": [y["year"] for y in ys], "without measures": [round(y["baseline"]) for y in ys],
                                                        "with measures": [round(y["with_measures"]) for y in ys], "cost": [round(y["cost"]) for y in ys]},
                                 title=f"The years ahead ({level}, t CO2e)", show={"kind": "chart", "chart": "line", "x": "year", "y": "with measures"}))
            text = (f"Of {E.tonnes(C['baseline'])} ({level}), the measures together abate {E.tonnes(C['joint'])}; credited each alone they would claim "
                    f"{E.tonnes(C['sum_alone'])}, counting {E.tonnes(C['overlap'])} twice where they act on the same sources. ")
            if P.get("gap") is not None:
                text += (f"By {target['year']} the target of {E.tonnes(P['target_level'])} is "
                         + ("met" if P["meets_target"] else f"missed by {E.tonnes(P['gap'])}") + " with activities growing as declared. ")
            if "budget" in target:
                text += f"The cumulative to {params.get('last')} is {E.tonnes(P['cumulative_with'])}, " + ("within" if P["within_budget"] else "beyond") + " the budget. "
            text += ("A measure here scales what remains of one entry's activity or factor; one that moves demand between entries (heat pumps, "
                     "electric cars) needs the new demand entered as its own entry.")
            out.append(api.note(text))
            return out

    return Mitigation()
