"""Whose custom: one outlet's trade area, its reach, and where its custom would go if it closed."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _market as m  # noqa: E402
from science import markets  # noqa: E402


def create(api):
    class Custom(api.BoardAnalysis):
        id = "custom"
        title = "Whose custom"
        description = ("One outlet: where its custom comes from (its trade area, a density over the buyers' places, with the places that hold "
                       "most of it in the fewest buyers), how far its custom walks, and the rivals its custom would go to if it closed.")
        params = [api.param("outlet", "Outlet", "", kind="text", help="its name, or a part of it; the one that sells most when left out"),
                  api.param("share", "The core holds", 0.6, minimum=0.05, maximum=1, step=0.05), *m.common(api)]

        def run(self, board, params, context):
            s = m.stated(board, params)
            wanted = str(params.get("outlet") or "").strip().lower()
            if wanted:
                found = [k for k, n in enumerate(s["names"]) if wanted == n.lower()] or [k for k, n in enumerate(s["names"]) if wanted in n.lower()]
                if not found:
                    raise LookupError(f"no outlet is named like '{params['outlet']}'")
                o = found[0]
            else:
                o = int(np.argmax(markets.ask("market", market=s["market"])["demand"]))
            context.progress(0.5, "asking the engine")
            a = markets.ask("offer", market=s["market"], offer=o, share=float(params.get("share", 0.6)))
            buyers = s["buyers"]
            trade = np.array(a["trade"])
            core = np.zeros(buyers.rows)
            core[np.array(a["core"]["cells"], dtype=int)] = a["core"]["part"]
            # Place by place (the residents and the workers of a block are one area): its custom, and the part of that custom in the core.
            columns, roles, places = m.by_place(buyers, custom=trade, of_core=trade * core)
            custom, of_core = np.asarray(columns.pop("custom"), dtype=float), np.asarray(columns.pop("of_core"), dtype=float)
            keep = np.flatnonzero(custom > 0)
            columns = {k: [v[i] for i in keep] for k, v in columns.items()}
            to = np.array(a["diversion"]["to"])
            rivals = np.argsort(-to)[:25]
            rivals = rivals[to[rivals] > 0]
            overlap = dict((int(k), v) for k, v in a["diversion"]["overlap"])
            name = s["names"][o]
            return [
                api.table("custom", {**columns, "custom": np.round(custom[keep]), "in_core": np.round(of_core[keep] / custom[keep], 3)}, title=f"The custom of {name}, by where it comes from",
                          roles={**roles, "amount": "custom"}, places=places, show={"kind": "layer", "layer": "areas" if places else "points", "colour": "custom", "size": "custom"}),
                api.table("rivals", {"name": [s["names"][k] for k in rivals], "lat": s["olat"][rivals], "lon": s["olon"][rivals], "part": np.round(100 * to[rivals], 3),
                                     "shares_buyers": [round(100 * overlap.get(int(k), float("nan")), 1) for k in rivals]},
                          title=f"Where the custom of {name} would go, in per cent", roles={"lat": "lat", "lon": "lon", "name": "name", "amount": "part"},
                          show={"kind": "chart", "chart": "bar", "x": "name", "y": "part", "how": "sum", "most": 15}),
                api.note(f"{name}: {a['demand']:,.0f} a period, {100 * a['share']:.2f}% of what is bought. Half its custom lies within {a['reach']['half']:.1f} minutes, four fifths within "
                         f"{a['reach']['most']:.1f}; it walks {a['reach']['mean']:.1f} on average. The core holds {100 * a['core']['share']:.0f}% of its custom in {a['core']['buyers']:,.0f} buyers. "
                         f"If it closed, {100 * a['diversion']['outside']:.1f}% of its custom would buy nothing"
                         + (f" and {100 * a['diversion']['own']:.1f}% would stay with its seller's other outlets" if a["diversion"]["own"] > 0 else "") + f". {s['assumed']}"),
            ]
    return Custom()
