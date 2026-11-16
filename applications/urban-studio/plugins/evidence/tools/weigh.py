"""Claims weighed and decided: an archive of evidence appraised, its claims and items as tables placed on the map."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _evidence as e  # noqa: E402


def create(api):
    class Weigh(api.BoardAnalysis):
        id = "weigh"
        title = "Claims weighed and decided"
        description = ("The archive appraised as a whole: whether each holding is still what it was when it was taken in; each claim weighed by the items "
                       "about its place and time, through their sources' channels, reports that repeat one registration counted once; its standing by "
                       "the archive's declared rule; and, where the archive declares a decision, the act for each claim and the places worth looking at "
                       "first within the budget. The claims and the items are answered as tables with their places, for the map, a board or the Workbench.")
        params = [e.archive_param(api)]

        def run(self, board, params, context):
            a, r = e.appraised(params, board)
            k, w, d = r["keep"], r["weigh"], r.get("decide") or {}
            decided = {c["claim"]: c for c in d.get("claims", [])}
            cl = [c for c in w["claims"] if c.get("lat") is not None]
            rows = {"claim": [], "lat": [], "lon": [], "radius_m": [], "standing": [], "posterior": [], "decibans": [], "items": [], "groups": [], "overstated": []}
            if decided:
                rows.update({"act": [], "look_first": [], "worth_of_looking": []})
            for c in cl:
                rows["claim"].append(c["id"]), rows["lat"].append(c["lat"]), rows["lon"].append(c["lon"]), rows["radius_m"].append(c.get("radius"))
                rows["standing"].append(c.get("standing", "error")), rows["posterior"].append(e.num(c.get("posterior")))
                db = c.get("decibans")
                rows["decibans"].append(e.num(db) if c.get("items") else 0.0)
                rows["items"].append(len(c.get("items", []))), rows["groups"].append(len(c.get("groups", [])))
                rows["overstated"].append(e.num(c.get("overstatement")))
                if decided:
                    x = decided.get(c["id"], {})
                    rows["act"].append(x.get("act")), rows["look_first"].append("yes" if x.get("inspect") else "no"), rows["worth_of_looking"].append(e.num(x.get("evsi")))
            out = [api.table("claims", rows, title="The claims, weighed" + (" and decided" if decided else ""), roles={"lat": "lat", "lon": "lon", "name": "claim", "amount": "decibans", "category": "standing"},
                             show={"kind": "layer", "layer": "points", "colour": "decibans", "map": "orchid"})]
            counted = {i["id"] for c in w["claims"] for i in c.get("items", [])}
            sound = {i["id"]: i for i in k["items"]}
            items = [i for i in a.get("items", []) if isinstance(i.get("lat"), (int, float)) and isinstance(i.get("lon"), (int, float))]
            out.append(api.table("items", {"item": [i["id"] for i in items], "lat": [i["lat"] for i in items], "lon": [i["lon"] for i in items],
                                           "time": [str(i.get("from") or "") for i in items], "kind": [i.get("kind", "") for i in items], "says": [i.get("says", "holds") for i in items],
                                           "source": [i.get("source", "") for i in items], "counts": ["yes" if i["id"] in counted else "no" for i in items],
                                           "sound": ["yes" if sound.get(i["id"], {}).get("sound", True) else "no" for i in items]},
                                 title="The items, at their places and times", roles={"lat": "lat", "lon": "lon", "time": "time", "name": "item", "category": "kind"},
                                 show={"kind": "layer", "layer": "points", "colour": "kind"}))
            n = {s: sum(1 for c in cl if c.get("standing") == s) for s in ("established", "open", "refuted", "no evidence")}
            words = [f"{k['verdict']['reason'][0].upper()}{k['verdict']['reason'][1:]}." if k["verdict"]["status"] != "holds" else
                     f"Every holding is what it was when it was taken in ({k['intact']} observed now, their custody unbroken).",
                     f"Of {len(cl)} claims, {n['established']} are established, {n['refuted']} refuted and {n['open']} open by the archive's rule "
                     f"(at least {w['rule']['established']:g} to establish, at most {w['rule']['refuted']:g} to refute); no item bears on {n['no evidence']}."]
            worst = max((c for c in cl if c.get("overstatement") and math.isfinite(c["overstatement"])), key=lambda c: c["overstatement"], default=None)
            if worst and worst["overstatement"] > 1.5:
                words.append(f"Counting reports that repeat one registration as independent would have overstated the evidence for {worst['id']} {worst['overstatement']:.1f} times.")
            if d.get("inspects"):
                words.append(f"{d['why'][0].upper()}{d['why'][1:]}.")
            out.append(api.note(" ".join(words)))
            out.append(api.note("A claim's standing follows from what is declared: the sources' channels, the claim's prior and how an event bears on it, the rule. "
                                "Items of the same place are taken as independent given the claim, which overstates them where they share a cause the claim does not name."))
            return out
    return Weigh()
