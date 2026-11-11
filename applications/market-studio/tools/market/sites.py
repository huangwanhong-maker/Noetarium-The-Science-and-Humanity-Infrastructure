"""Where to open: sites chosen among candidates for a seller, and what an outlet at each would sell."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _market as m  # noqa: E402
from science import markets  # noqa: E402


def create(api):
    class Sites(api.BoardAnalysis):
        id = "sites"
        title = "Where to open"
        description = ("Sites for new outlets, chosen among candidate places for a seller of the market or a newcomer: each the site of greatest gain, "
                       "given those chosen before. That gains at least 63% of what the best set would (the seller's sales are submodular in its sites). "
                       "With one site asked for and one candidate, it is what an outlet there would sell, and from whom.")
        params = [api.param("candidates", "Candidates", "candidates", kind="text", role="table", help="a table of places where an outlet could stand"),
                  api.param("k", "How many", 3, kind="integer", minimum=1, maximum=20),
                  api.param("for_seller", "For", "", kind="text", help="a seller's name; a newcomer when left out"), *m.common(api)]

        def run(self, board, params, context):
            s = m.stated(board, params)
            candidates = board.table(params["candidates"])
            clat, clon = m.where(candidates)
            ok = np.flatnonzero(np.isfinite(clat) & np.isfinite(clon))
            # One candidate for each 25 m: places at one address are one site.
            seen, unique = set(), []
            for i in ok:
                key = (round(clat[i] * 4000), round(clon[i] * 4000))
                if key not in seen:
                    seen.add(key)
                    unique.append(int(i))
            if not unique:
                raise ValueError(f"the slice holds no candidate places in '{candidates.name}'")
            unique = np.array(unique)
            context.progress(0.2, f"the minutes to {len(unique):,} candidates")
            b, c, minutes = m.pairs(s["buyers"], (clat[unique], clon[unique]), params.get("walk", "straight"), float(params.get("limit", 15)), s["speed"], str(params.get("model") or ""))
            reach = [{"b": [], "cost": []} for _ in unique]
            for bi, ci, t in zip(b.tolist(), c.tolist(), minutes.tolist()):
                reach[ci]["b"].append(bi), reach[ci]["cost"].append(t)
            wanted = " ".join(str(params.get("for_seller") or "").upper().split())
            seller = -1
            if wanted:
                found = [k for k, n in enumerate(s["seller_names"]) if " ".join(n.upper().split()) == wanted]
                if not found:
                    raise LookupError(f"no seller is named '{params['for_seller']}'")
                seller = found[0]
            who = s["seller_names"][seller] if seller >= 0 else "a newcomer"
            context.progress(0.6, "choosing")
            k = min(int(params.get("k", 3)), len(unique))
            a = markets.ask("sites", market=s["market"], sites=reach, k=k, seller=seller)
            chosen = np.array(a["chosen"], dtype=int)
            # What each chosen site would sell alone, and from whom: an outlet entering the market as it stands.
            alone = [markets.ask("enter", market=s["market"], site=reach[int(i)], seller=seller) for i in chosen]
            names = candidates.text(candidates.roles["name"]) if (getattr(candidates, "roles", None) or {}).get("name") else [""] * candidates.rows
            return [
                api.table("sites", {"site": [f"Site {i + 1}" for i in range(len(chosen))], "at": [names[unique[i]] for i in chosen], "lat": clat[unique[chosen]], "lon": clon[unique[chosen]],
                                    "gain": np.round(a["gains"]), "sells_alone": [round(e["demand"]) for e in alone], "from_rivals": [round(e["from_rivals"]) for e in alone],
                                    "from_those_who_bought_nothing": [round(e["from_outside"]) for e in alone], "from_its_own": [round(e["cannibalised"]) for e in alone]},
                          title=f"Sites for {who}, in the order chosen", roles={"lat": "lat", "lon": "lon", "name": "site", "amount": "gain"},
                          show={"kind": "layer", "layer": "points", "size": "gain", "colour": "gain"}),
                api.note(f"{k} of {len(unique):,} candidates for {who}: sales of {a['after']:,.0f} a period, from {a['before']:,.0f} now. Each was the site of greatest gain given those before it, "
                         f"which gains at least {100 * a['bound']:.0f}% of what the best {k} would. Rivals are taken not to respond. {s['assumed']}"),
            ]
    return Sites()
