"""What a hazard's losses cost households in well-being over their recovery, after Unbreakable: each household's assets, income and
transfers lost, reconstruction paid from consumption, savings and support capping the deepest part, valued by the utility of what
they consume and expressed as the national consumption that would be as great a loss; and measures compared by the asset losses and
the well-being losses they avoid, which rank them differently."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _risk as r  # noqa: E402
from science import risk  # noqa: E402


def create(api):
    class Wellbeing(api.BoardAnalysis):
        id = "wellbeing"
        title = "What the losses cost households in well-being, and which measures avoid most"
        description = ("Households lose assets when a hazard reaches them, the income those assets earned, and the transfers that fall with the "
                       "nation's assets; they rebuild from what they would have consumed. Savings and support cap the deepest part of the loss. The "
                       "loss of consumption over ten years is valued by its utility (an elasticity of marginal utility η) and expressed as the loss "
                       "of national mean consumption that would cost as much well-being; socioeconomic resilience is the asset loss over that "
                       "(Hallegatte, Bangalore and Vogt-Schilb 2016; Walsh and Hallegatte 2019). Each measure is computed again whole and judged "
                       "twice: by the asset losses it avoids and by the well-being losses it avoids. Every parameter is a declaration.")
        params = [api.param("households", "Households", "households", kind="text", role="table",
                            help="id, district, quintile (optional), persons, income (a person a year), transfers, savings, private_share, vulnerability, warned (yes/no)"),
                  api.param("exposure", "Shares affected by return period", "exposure", kind="text", role="table", help="id, then a column rp<N> for each return period"),
                  api.param("measures", "Measures", "measures", kind="text", role="table",
                            help="name, kind (faster, stronger, warning, savings, protect, support), group, value; for support: share, reference_group, exclusion, inclusion"),
                  api.param("productivity", "Average productivity of capital Π", 0.3, minimum=0.05, maximum=1, step=0.01),
                  api.param("eta", "Elasticity of marginal utility η", 1.5, minimum=0.1, maximum=4, step=0.1),
                  api.param("rho", "Discount rate ρ", 0.06, minimum=0, maximum=0.2, step=0.005),
                  api.param("rebuild_years", "Years to rebuild 95%", 3, minimum=0.5, maximum=15, step=0.5),
                  api.param("poverty_line", "Poverty line (a person a year)", 600, minimum=0, step=10),
                  api.param("optimal", "Each household rebuilds at its own best pace", False, kind="boolean")]

        def run(self, board, params, context):
            for k in ("households", "exposure"):
                if str(params.get(k) or "") not in board.tables:
                    raise ValueError("the households and their exposure are both tables on the board")
            cols = lambda t: {c: t.raw(c) for c in t.columns}  # noqa: E731
            Ht = cols(board.table(params["households"]))
            Xt = cols(board.table(params["exposure"]))
            ids = [str(v) for v in Ht["id"]]
            yes = lambda v: str(v).strip().lower() in ("yes", "true", "1")  # noqa: E731
            f = lambda t, k, i, d=0.0: float(t[k][i]) if k in t and t[k][i] not in (None, "") else d  # noqa: E731
            H = [{"id": ids[i], "group": str(Ht["district"][i]) if "district" in Ht else "", "persons": f(Ht, "persons", i, 1), "income": f(Ht, "income", i),
                  "transfers": f(Ht, "transfers", i), "savings": f(Ht, "savings", i), "private_share": f(Ht, "private_share", i, 1),
                  "vulnerability": f(Ht, "vulnerability", i), "warned": yes(Ht["warned"][i]) if "warned" in Ht else False} for i in range(len(ids))]
            row = {str(v): k for k, v in enumerate(Xt["id"])}
            missing = [i for i in ids if i not in row]
            if missing:
                raise ValueError(f"no exposure for {', '.join(missing)}")
            periods = sorted((int(c[2:]), c) for c in Xt if c.startswith("rp") and c[2:].isdigit())
            if not periods:
                raise ValueError("the exposure table has a column rp<N> for each return period")
            events = [{"return_period": T, "exposed": [float(Xt[c][row[i]]) for i in ids]} for T, c in periods]
            measures = []
            Mt = cols(board.table(params["measures"])) if str(params.get("measures") or "") in board.tables else None
            if Mt is not None:
                for k in range(len(Mt["name"])):
                    m = {"name": str(Mt["name"][k]), "kind": str(Mt["kind"][k]).strip()}
                    if "group" in Mt and Mt["group"][k] not in (None, ""):
                        m["group"] = str(Mt["group"][k])
                    if m["kind"] == "support":
                        m["support"] = {"share": f(Mt, "share", k), "reference_group": str(Mt["reference_group"][k] or "") if "reference_group" in Mt else "",
                                        "exclusion": f(Mt, "exclusion", k), "inclusion": f(Mt, "inclusion", k)}
                    elif m["kind"] != "warning":
                        m["value"] = f(Mt, "value", k)
                    measures.append(m)
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            readings = {"productivity": g("productivity", 0.3), "eta": g("eta", 1.5), "rho": g("rho", 0.06), "rebuild_years": g("rebuild_years", 3),
                        "optimal_rebuilding": params.get("optimal") in (True, "true", "1", 1)}
            a = risk.ask("wellbeing.assess", households=H, events=events, readings=readings, poverty_line=g("poverty_line", 0), measures=measures)
            B = a["baseline"]
            G = B["groups"]
            out = [api.table("wellbeing_districts", {
                "district": [x["group"] for x in G], "persons": [round(x["persons"]) for x in G],
                "asset_loss_a_year": [round(x["asset_loss"]) for x in G], "wellbeing_loss_a_year": [round(x["equivalent"]) for x in G],
                "share_of_asset_losses": [round(x["asset_share"], 3) for x in G], "share_of_wellbeing_losses": [round(x["equivalent_share"], 3) for x in G],
                "resilience": [round(x["resilience"], 3) for x in G], "pushed_below_the_line_a_year": [round(x["pushed_below"], 1) for x in G]},
                title="Each district's losses a year: of assets, and of well-being as national consumption (invented city; currency of the declarations)",
                show={"kind": "chart", "chart": "bar", "x": "district", "y": "wellbeing_loss_a_year", "how": "sum"})]
            if "quintile" in Ht:
                q: dict = {}
                for i, h in enumerate(B["households"]):
                    k = str(Ht["quintile"][i])
                    acc = q.setdefault(k, [0.0, 0.0])
                    acc[0] += h["asset_loss"] * H[i]["persons"]
                    acc[1] += h["equivalent"] * H[i]["persons"]
                ta, te = sum(v[0] for v in q.values()) or 1, sum(v[1] for v in q.values()) or 1
                out.append(api.table("wellbeing_income_groups", {"income_group": list(q), "share_of_asset_losses": [round(v[0] / ta, 3) for v in q.values()],
                                                                 "share_of_wellbeing_losses": [round(v[1] / te, 3) for v in q.values()]},
                                     title="Who bears the losses: each income group's share of asset losses and of well-being losses",
                                     show={"kind": "chart", "chart": "bar", "x": "income_group", "y": "share_of_wellbeing_losses", "how": "sum"}))
            M = a.get("measures", [])
            if M:
                out.append(api.table("wellbeing_measures", {
                    "measure": [m["name"] for m in M], "avoided_asset_loss_a_year": [round(m["avoided_asset_loss"]) for m in M],
                    "avoided_wellbeing_loss_a_year": [round(m["avoided_equivalent"]) for m in M],
                    "rank_by_assets": [int(m["rank_by_assets"]) for m in M], "rank_by_wellbeing": [int(m["rank_by_wellbeing"]) for m in M]},
                    title="Each measure judged twice: by the asset losses and by the well-being losses it avoids a year",
                    show={"kind": "chart", "chart": "bar", "x": "measure", "y": "avoided_wellbeing_loss_a_year", "how": "sum"}))
            first_a = next((m for m in M if m["rank_by_assets"] == 1), None)
            first_w = next((m for m in M if m["rank_by_wellbeing"] == 1), None)
            note = (f"A year on average the hazard costs {r.n0(B['asset_loss'])} of assets and {r.n0(B['equivalent'])} of well-being, as national consumption: "
                    f"socioeconomic resilience {B['resilience']:.0%}. ")
            worst = min(G, key=lambda x: x["resilience"] if x["resilience"] > 0 else 9)
            note += f"The {worst['group']} bears {worst['asset_share']:.0%} of the asset losses and {worst['equivalent_share']:.0%} of the well-being losses. "
            if first_a and first_w:
                note += (f"Ranked by assets saved, the first measure is {first_a['name'].lower()}; ranked by well-being, {first_w['name'].lower()}. "
                         if first_a is not first_w else f"{first_a['name']} ranks first by assets and by well-being alike. ")
            note += ("Every household, share, measure and parameter here is declared; well-being losses are a model's output, after Unbreakable's two working "
                     "papers (CC BY 3.0 IGO), its readings said in the theory note.")
            out.append(api.note(note))
            return out

    return Wellbeing()
