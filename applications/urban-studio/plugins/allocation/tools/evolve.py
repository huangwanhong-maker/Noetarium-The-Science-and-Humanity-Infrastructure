"""Resources of many kinds allocated by evolution on a case (ADR 0137): a care fund claimed by the areas and granted in an order while it
lasts, outreach crews in units, maintenance duties that may not be thrown away, sites with a shade canopy's direction; each resource's
type verified by kategoria, every allocation found checked by it, the kept compared under declared principles, never one score."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _allocation as A  # noqa: E402
from science import allocation as al  # noqa: E402


def create(api):
    class Evolve(api.BoardAnalysis):
        id = "evolve"
        title = "Allocate resources of many kinds by evolution"
        description = ("Money claimed by the areas and granted in an order while it lasts (each grant raising the price of the next), crews in "
                       "units, duties that may not be thrown away, and distinct sites with a canopy's direction, allocated together by an "
                       "evolutionary search. Each resource's type is verified by kategoria before the search, and every allocation found is "
                       "checked by it: one that leaves a duty unassigned is refused. The allocations kept are compared under three principles, "
                       "never added into one score; whether the order of grants mattered is tested. Every number is the example's declaration, "
                       "illustrative and no authority's.")
        params = [api.param("case", "Case", "melbourne-areas", kind="text", help=", ".join(A.EVOLVING)),
                  api.param("generations", "Generations", 300, minimum=10, step=10),
                  api.param("seed", "Seed", 1, minimum=1, step=1)]

        def run(self, board, params, context):
            name = str(params.get("case") or "melbourne-areas").strip()
            if name not in A.EVOLVING:
                raise ValueError(f"no case is declared for evolution: {', '.join(A.EVOLVING)}")
            E = A.evolving(name, generations=int(params.get("generations") or 300), seed=int(params.get("seed") or 1))
            r = al.evolve(E["problem"], E["evaluators"], E["settings"], E["comparing"])
            P = E["problem"]
            names = [e["name"] for e in P["recipients"]]
            metric = E["evaluators"][0]["name"]
            kept = r["kept"]
            if not kept:
                return [api.note("The search kept no lawful allocation.")]
            def m(a):
                return next(x for x in a["metrics"] if x["name"] == metric)
            out = [api.table("evolved_kept", {"allocation": [a["rank"] for a in kept], "total value": [round(-m(a)["value"], 3) for a in kept],
                                              "least per thousand": [round(min(m(a)["units"]), 3) for a in kept], "granted": [len(a["granted"]) for a in kept],
                                              "lawful": ["yes" if a["lawful"] else "no" for a in kept]},
                             title="The allocations kept, best first by the tiers of evidence",
                             show={"kind": "chart", "chart": "scatter", "x": "total value", "y": "least per thousand"})]
            best = kept[0]
            cols = {"area": names}
            for res in P["resources"]:
                if res["carrier"] != "items":
                    cols[res["name"]] = best["amounts"][res["name"]]
            for res in P["resources"]:
                if res["carrier"] == "items":
                    own = best["owners"][res["name"]]
                    cols[res["name"]] = [", ".join(i for i, who in own.items() if who == e["id"]) for e in P["recipients"]]
            order = {rid: k + 1 for k, rid in enumerate(best["granted"])}
            cols["granted"] = [order.get(e["id"], "not granted") for e in P["recipients"]]
            places = E["places"]
            roles = None
            if all(e["id"] in places for e in P["recipients"]):
                cols["lat"] = [places[e["id"]][0] for e in P["recipients"]]
                cols["lon"] = [places[e["id"]][1] for e in P["recipients"]]
                roles = {"lat": "lat", "lon": "lon", "amount": P["resources"][0]["name"]}
            out.append(api.table("evolved_best", cols, title="The first allocation kept: what each area receives, and the order of grants", roles=roles))
            C = r.get("comparison") or {}
            undominated = ", ".join(C["options"][i] for i in C.get("maximal", [])) if C else ""
            c = r["counts"]
            said = (f"{E['title']}. {c['allocations']:,} allocations evaluated ({c['genotypes']:,} genotypes, {c['reused']:,} repeats given a remembered "
                    f"evaluation); {c['unlawful']:,} were found unlawful by kategoria and refused. The first kept grants {len(best['granted'])} of "
                    f"{len(names)} areas" + (f", leaving {', '.join(names[[e['id'] for e in P['recipients']].index(u)] for u in best['unmet'])} without a grant" if best["unmet"] else "") +
                    f". Order {'mattered' if r['order']['may_matter'] else 'was not found to matter'}" +
                    (f" ({r['order']['witnesses'][0]})" if r['order']['witnesses'] else "") + ". " +
                    (f"Under the total, the least served area and a shortfall below 1 a thousand residents, the undominated are {undominated}. " if undominated else "") +
                    (f"The comparison was refused: {r['comparison_refused']}. " if r.get("comparison_refused") else "") +
                    f"The record has {len(r['record'])} sealed entries, verified {'yes' if r['verified'] else 'no'}. {E['said']}. {E['credit']}.")
            out.append(api.note(said))
            return out

    return Evolve()
