"""A participatory budget as a sequential mechanism (ADR 0141): every rule that walks an order of the projects and funds each that still
fits is an order decoded by the budget, so the orders of the projects are searched by evolution under two declared objectives (the
share of voters left with none of their projects funded, and the approvals met), and the front of what orders reach is set beside
utilitarian greedy (the city's count), greedy per unit of cost and the Method of Equal Shares, whose orders are proposed to the
search's first generation. Neither objective is the right one; the front is the answer."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _allocation as A  # noqa: E402
from science import allocation as al  # noqa: E402


def orders_of(name: str, generations: int = 100, seed: int = 1) -> tuple:
    C = A.case(name)
    B = al.budget_orders(C["budget"], C["costs"], C["approvals"], groups=C.get("voter_neighbourhood"), group_names=C.get("neighbourhoods"),
                         names=[p.get("name", str(k)) for k, p in enumerate(C["projects"])],
                         settings={"generations": int(generations), "mu": 40, "seed": int(seed), "keep": 16, "when": "2026-11-21T00:00:00Z"})
    return C, B


def orders_said(C: dict, B: dict) -> str:
    recorded = sorted(k for k, p in enumerate(C["projects"]) if p["selected"])
    g, pc, es = B["greedy"], B["greedy_per_cost"], B["equal_shares"]
    beyond = [f for f in B["front"] if all(not (r["share_with_nothing"] <= f["share_with_nothing"] and r["approvals"] >= f["approvals"]) for r in (g, pc, es))]
    best_nothing = min(B["front"], key=lambda f: f["share_with_nothing"]) if B["front"] else None
    c = B["counts"]
    return (f"{B['voters']:,} voters, {B['projects']} projects. Utilitarian greedy {'reproduces' if sorted(g['funded']) == recorded else 'does not reproduce'} "
            f"the city's selection and leaves {g['share_with_nothing']:.1%} of voters with nothing ({g['approvals']:,.0f} approvals met); greedy per unit of "
            f"cost {pc['share_with_nothing']:.1%} ({pc['approvals']:,.0f}); Equal Shares {es['share_with_nothing']:.1%} ({es['approvals']:,.0f}), "
            f"{'itself the decoding of an order' if B['equal_shares_is_an_order'] else 'not the decoding of any order'}. "
            f"Searching {c['allocations']:,} orders, with the rules' own orders proposed, the front holds {len(B['front'])} selection(s)"
            + (f"; the fewest with nothing {best_nothing['share_with_nothing']:.1%} ({best_nothing['approvals']:,.0f} approvals)" if best_nothing else "")
            + (f"; {len(beyond)} of them beyond every rule on the two measures" if beyond else "; none beyond the rules on both measures")
            + f". The search's record is sealed (verified {'yes' if B['verified'] else 'no'}). {C['credit']}.")


def create(api):
    class Orders(api.BoardAnalysis):
        id = "orders"
        title = "A participatory budget as orders of its projects"
        description = ("Every rule that walks the projects in some order and funds each that still fits is an order decoded by the budget, so "
                       "the orders of the projects are searched by evolution under two declared objectives: the share of voters left with none of "
                       "their projects funded, and the approvals met. The front of what orders reach is set beside utilitarian greedy (as the city "
                       "counts), greedy per unit of cost and the Method of Equal Shares, whose orders enter the search's first generation. Neither "
                       "objective is the right one; the front is the answer. Pabulib states no licence: its files stay in the cache.")
        params = [api.param("case", "Budget", "pabulib-Poland_Warszawa_2023", kind="text", help="a Pabulib case built by data/products/allocation_cases.py"),
                  api.param("generations", "Generations", 100, minimum=5, step=5, help="about a third of a second each for Warsaw's 78,904 ballots"),
                  api.param("seed", "Seed", 1, minimum=1, step=1)]

        def run(self, board, params, context):
            C, B = orders_of(str(params.get("case") or "pabulib-Poland_Warszawa_2023").strip(), int(params.get("generations") or 100), int(params.get("seed") or 1))
            rows = [("utilitarian greedy (the city's count)", B["greedy"]), ("greedy per unit of cost", B["greedy_per_cost"]), ("Method of Equal Shares", B["equal_shares"])]
            rows += [(f"an order the search kept ({k + 1})", f) for k, f in enumerate(B["front"])]
            out = [api.table("orders_front", {"selection": [r[0] for r in rows], "share_with_nothing": [r[1]["share_with_nothing"] for r in rows],
                                              "approvals_met": [r[1]["approvals"] for r in rows], "projects_funded": [len(r[1]["funded"]) for r in rows],
                                              "spent": [r[1]["spent"] for r in rows]},
                             title=f"{C['title']}: the rules and the front of what orders reach",
                             show={"kind": "chart", "chart": "scatter", "x": "share_with_nothing", "y": "approvals_met"})]
            names = B.get("group_names") or C.get("neighbourhoods") or []
            if names and B["greedy"]["by_group"]:
                cols = {"neighbourhood": names}
                for label, r in rows[:3] + rows[3:4]:
                    cols[f"nothing: {label}"] = r["by_group"]
                out.append(api.table("orders_neighbourhoods", cols, title="The share of each neighbourhood's voters left with nothing"))
            out.append(api.note(orders_said(C, B)))
            return out

    return Orders()
