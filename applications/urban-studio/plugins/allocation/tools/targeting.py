"""Who a fund meant for the deprived reaches, and who finally gains: a fund given to England's most deprived neighbourhoods against the
income-deprived people it is meant for (many live elsewhere), and a sum spent in a place followed through re-spending and rents."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _allocation as A  # noqa: E402
from science import allocation as al  # noqa: E402


def create(api):
    class Targeting(api.BoardAnalysis):
        id = "targeting"
        title = "Who the money meant for is reached, and who finally gains"
        description = ("A fund given to the neighbourhoods (LSOAs) in England's most deprived deciles, in proportion to their people, set against the "
                       "income-deprived people it is meant for: the share that reaches them if each neighbourhood spreads its amount over its people, "
                       "and how many of them live where nothing goes. Then a sum spent in a place followed past its first recipients: re-spending "
                       "kept in the place round by round (with LM3), and the share capitalised into rents, between declared bounds of how residents "
                       "and housing respond, what renters keep and what landlords receive. Every response and share is declared.")
        params = [api.param("deciles", "Neighbourhoods funded: the most deprived deciles", 1, minimum=1, maximum=10, step=1),
                  api.param("amount", "The fund (£)", 1e9, minimum=0, step=1e6),
                  api.param("spent", "Share of income spent again", 0.8, minimum=0, maximum=1, step=0.05),
                  api.param("kept", "Share of spending kept in the place", 0.4, minimum=0, maximum=0.99, step=0.05),
                  api.param("renters", "Share of residents renting", 0.5, minimum=0, maximum=1, step=0.05),
                  api.param("migration_lo", "Residents' response to the place, from", 0.5, minimum=0, step=0.1),
                  api.param("migration_hi", "to", 2, minimum=0, step=0.1),
                  api.param("supply_lo", "Housing's response to rents, from", 0.3, minimum=0, step=0.1),
                  api.param("supply_hi", "to", 1.5, minimum=0, step=0.1)]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            D = A.case("england-iod2025")
            L = D["lsoas"]
            k, F = int(g("deciles", 1)), g("amount", 1e9)
            pop_in = sum(p for p, dec in zip(L["population"], L["decile"]) if dec <= k)
            amount = [F * p / pop_in if dec <= k else 0.0 for p, dec in zip(L["population"], L["decile"])]   # the scheme declared: by people, in the deciles funded
            I = al.incidence(targeting={"amount": amount, "population": L["population"], "meant": L["income_deprived"]},
                             propagation={"amount": F, "spent": g("spent", 0.8), "kept": g("kept", 0.4)},
                             capitalisation={"amount": F, "renters_share": g("renters", 0.5), "landlords_local": 0.3,
                                             "migration": [g("migration_lo", 0.5), g("migration_hi", 2)], "supply": [g("supply_lo", 0.3), g("supply_hi", 1.5)]})
            T, P, C = I["targeting"], I["propagation"], I["capitalisation"]
            out = [api.table("targeting", {"measure": ["share of the fund reaching income-deprived people", "income-deprived people in England", "of them where nothing goes",
                                                       "the fund per income-deprived person, given directly"],
                                           "value": [T["reaching"], T["meant"], T["missed"], T["per_person_meant"]]},
                             title=f"A fund to the {k} most deprived decile{'s' if k > 1 else ''} of neighbourhoods against the people it is meant for"),
                   api.table("respending", {"round": [f"round {j + 1}" for j in range(len(P["rounds"]))], "income": P["rounds"]},
                             title=f"Re-spending kept in the place: a multiplier of {P['multiplier']:.2f}, LM3 {P['lm3']:.2f}"),
                   api.table("capitalisation", {"part": ["capitalised share", "what renters keep", "owner-occupiers", "landlords in the place", "landlords elsewhere"],
                                                "from": [C["share"][0], C["renters"][0], C["owner_occupiers"], C["landlords_local"][0], C["landlords_away"][0]],
                                                "to": [C["share"][1], C["renters"][1], C["owner_occupiers"], C["landlords_local"][1], C["landlords_away"][1]]},
                             title="Of the value the fund buys, what prices carry to whom (bounds from the declared responses)")]
            out.append(api.note(f"{T['said']}. {C['said']}. Re-spending and capitalisation are readings under declared shares and responses, not counterfactual impacts. {D['credit']}."))
            return out

    return Targeting()
