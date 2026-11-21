"""A community's greenhouse gas inventory, read by sector and scope: the totals at BASIC, BASIC+ and the territorial level with their
uncertainty, biogenic CO2 apart, whether each level is complete and what leaves it incomplete, the emissions per resident, and the key
categories that make up 95% of the total (ADR 0147)."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _emissions as E  # noqa: E402


def create(api):
    class Inventory(api.BoardAnalysis):
        id = "inventory"
        title = "A community's greenhouse gas inventory, by sector and scope"
        description = ("An inventory as a table of entries, each an activity times a factor (kg of a gas per unit), a stated amount, or a "
                       "notation key instead of a figure (NO not occurring, IE included elsewhere, NE not estimated, C confidential), read in "
                       "the structure of the Global Protocol for Community-Scale Greenhouse Gas Emission Inventories: six sectors, three scopes "
                       "by the boundary, BASIC and BASIC+ as sums over cells, the territorial total, generation supplied to the grid and waste "
                       "from outside kept out of BASIC, biogenic CO2 apart. Each level's uncertainty by the IPCC's Approach 1, its completeness "
                       "with the cells that leave it incomplete, and the key categories by the IPCC's level assessment. Every factor and "
                       "potential is declared.")
        params = E.inventory_params(api) + [api.param("level", "Level for key categories", "basic+", kind="choice", options=["basic", "basic+", "territorial"])]

        def run(self, board, params, context):
            I = E.inventory_from(board, params)
            r = E.em.totals(I, level=params.get("level") or "basic+")
            cells = r["cells"]
            out = [api.table("inventory_cells", {
                "sector": [c["sector"] for c in cells], "scope": [c["scope"] for c in cells], "tonnes CO2e": [round(c["tonnes"], 1) for c in cells],
                "entries estimated": [c["estimated"] for c in cells], "entries keyed": [c["keyed"] for c in cells],
                "status": ["no entry" if c["missing"] else "not estimated (NE)" if c["not_estimated"] else "accounted for" for c in cells]},
                title="The inventory by sector and scope", show={"kind": "chart", "chart": "bar", "x": "sector", "y": "tonnes CO2e", "how": "sum"})]
            ks = r["key_categories"]
            out.append(api.table("inventory_key_categories", {
                "sector": [k["sector"] for k in ks], "scope": [k["scope"] for k in ks], "subsector": [k["subsector"] for k in ks], "gas": [k["gas"] for k in ks],
                "tonnes CO2e": [round(k["tonnes"], 1) for k in ks], "share": [round(k["share"], 4) for k in ks], "cumulative": [round(k["cumulative"], 4) for k in ks],
                "key": ["yes" if k["key"] else "no" for k in ks]}, title=f"Key categories by the IPCC's Approach 1 ({params.get('level') or 'basic+'})"))
            gaps = r["gaps_basic_plus"]
            text = (f"Under {r['potentials']['name']}: BASIC {E.tonnes(r['basic'])} CO2e (± {100 * r['u_basic']:.1f}% at 95%), BASIC+ {E.tonnes(r['basic_plus'])} "
                    f"(± {100 * r['u_basic_plus']:.1f}%), the territorial total {E.tonnes(r['territorial'])}; biogenic CO2 {E.tonnes(r['biogenic'])}, reported apart. "
                    + (f"BASIC+ per resident: {r['per_capita']:.2f} t. " if r.get("per_capita") is not None else "")
                    + ("BASIC is complete. " if r["basic_complete"] else "BASIC is incomplete: " + "; ".join(r["gaps_basic"]) + ". ")
                    + ("BASIC+ is complete. " if r["basic_plus_complete"] else "BASIC+ is incomplete: " + "; ".join(gaps) + ". ")
                    + f"{sum(1 for k in ks if k['key'])} of {len(ks)} categories make up 95% of the total. "
                    "The uncertainty assumes independent entries; entries that share a factor (one grid's) are correlated and the true "
                    "uncertainty is larger.")
            out.append(api.note(text))
            return out

    return Inventory()
