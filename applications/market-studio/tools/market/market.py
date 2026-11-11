"""The market: who sells, how much of the spend buys, and where it buys nothing."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _market as m  # noqa: E402
from science import markets  # noqa: E402


def create(api):
    class Market(api.BoardAnalysis):
        id = "market"
        title = "The market"
        description = ("Who sells and how much of the spend buys: each outlet's sales and each seller's, the concentration among sellers, "
                       "and what of each buyer's spend finds nothing within reach. The kernel is presumed (Huff's model).")
        params = m.common(api)

        def run(self, board, params, context):
            s = m.stated(board, params)
            context.progress(0.4, "asking the engine")
            a = markets.ask("market", market=s["market"])
            sales, by = np.array(a["demand"]), np.array(a["by_seller"])
            bought = a["bought"]
            outlets_of = np.bincount(s["seller"], minlength=len(by))
            order = np.argsort(-by)
            columns, roles, places = m.by_place(s["buyers"], brings=a["brings"], unserved=a["unserved"])
            brings, unserved = np.asarray(columns.pop("brings"), dtype=float), np.asarray(columns.pop("unserved"), dtype=float)
            share = np.divide(unserved, brings, out=np.zeros_like(unserved), where=brings > 0)
            st = a["structure"]
            return [
                api.table("outlets", {"name": s["names"], "seller": [s["seller_names"][k] for k in s["seller"]], "lat": s["olat"], "lon": s["olon"], "sales": np.round(sales),
                                      "share": np.round(100 * sales / bought, 4) if bought else sales * 0},
                          title="Outlets, by what each would sell", roles={"lat": "lat", "lon": "lon", "name": "name", "amount": "sales", "category": "seller"},
                          show={"kind": "layer", "layer": "points", "size": "sales", "colour": "sales"}),
                api.table("sellers", {"seller": [s["seller_names"][k] for k in order], "outlets": outlets_of[order], "sales": np.round(by[order]),
                                      "share": np.round(100 * by[order] / bought, 3) if bought else by[order] * 0},
                          title="Sellers, by what each would sell", roles={"name": "seller", "amount": "sales"}, show={"kind": "chart", "chart": "bar", "x": "seller", "y": "sales", "how": "sum", "most": 15}),
                api.table("unserved", {**columns, "brings": np.round(brings), "unserved": np.round(unserved), "share_unserved": np.round(share, 4)},
                          title="What each buyer's place brings, and what of it buys nothing", roles={**roles, "amount": "unserved"}, places=places,
                          show={"kind": "layer", "layer": "areas" if places else "points", "colour": "share_unserved", "size": "unserved"}),
                api.note(f"{len(sales):,} outlets of {len(by):,} sellers. All who could buy bring {a['total']:,.0f} a period; {100 * a['penetration']:.1f}% of it buys, "
                         f"walking {a['mean_cost']:.1f} minutes on average. The four largest sellers hold {100 * st['cr4']:.1f}%; the index of concentration is {st['hhi']:.4f}, "
                         f"that of {st['equivalent']:.0f} equal sellers. {s['assumed']}"),
            ]
    return Market()
