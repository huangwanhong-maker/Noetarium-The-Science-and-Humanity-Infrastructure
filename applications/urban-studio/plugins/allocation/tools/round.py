"""A round of allocation on a case: a declared sum divided by declared stages, each serving a principle; what each entity receives,
stage by stage, with its reasons, who was excluded and why, and who was reached first."""

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _allocation as A  # noqa: E402
from science import allocation as al  # noqa: E402


def create(api):
    class Round(api.BoardAnalysis):
        id = "round"
        title = "Divide a sum by declared stages"
        description = ("A sum short of what is wanted, divided by stages applied in turn to what remains: who is eligible, a floor closed first, "
                       "priority classes served in order, claims rationed by a declared rule (proportional, equal awards, equal losses, the Talmud "
                       "rule and others), shares by points or in proportion, and fiscal equalisation. Each stage names the principle it serves and "
                       "who declared it. What each entity receives is told stage by stage with its reasons, and the order in which the round reached "
                       "them. The examples' amounts and parameters are illustrative declarations, no authority's; the stages' JSON may be edited.")
        params = [api.param("case", "Case", "england-iod2025", kind="text", help="england-iod2025 or melbourne-areas, from the data library"),
                  api.param("amount", "The sum", 1e9, minimum=0, step=1e6),
                  api.param("per_person", "Declared sum a person in need", 80, minimum=0, step=10),
                  api.param("rule", "Rationing rule for claims", "proportional", kind="text", help=", ".join(al.RULES)),
                  api.param("stages", "Stages (JSON, replacing the example's)", "", kind="text", help="a list of stages as allocation::rounds reads them; empty for the example's")]

        def run(self, board, params, context):
            name = str(params.get("case") or "england-iod2025").strip()
            R = A.declared_round(name, params)
            if str(params.get("stages") or "").strip():
                R["stages"] = json.loads(params["stages"])
            a = al.run(R)
            E = {e["id"]: e for e in R["entities"]}
            order = {o["entity"]: k + 1 for k, o in enumerate(a["order"])}
            per = A.READINGS[name]["per"]
            rows = a["entities"]
            cols = {"entity": [e["name"] for e in rows], "amount": [e["amount"] for e in rows],
                    f"per_{per}": [round(e["amount"] / E[e["id"]]["at"][per], 2) if E[e["id"]]["at"].get(per) else None for e in rows],
                    "reached": [order[e["id"]] for e in rows]}
            for k, st in enumerate(R["stages"]):
                cols[f"stage_{k + 1}_{st['kind']}"] = [e["by_stage"][k] for e in rows]
            cols["excluded"] = [e["excluded"] or "" for e in rows]
            roles = None
            if all("lat" in E[e["id"]] for e in rows):
                cols["lat"], cols["lon"] = [E[e["id"]]["lat"] for e in rows], [E[e["id"]]["lon"] for e in rows]
                roles = {"lat": "lat", "lon": "lon", "amount": "amount"}
            unit = R["unit"]
            out = [api.table("allocation_round", cols, title=f"{R['name']}: what each receives, stage by stage ({unit})", roles=roles,
                             show={"kind": "chart", "chart": "bar", "x": "entity", "y": "amount", "how": "sum"}),
                   api.table("allocation_stages", {"stage": [f"{k + 1}. {s['kind']}" for k, s in enumerate(a["stages"])], "principle": [s["principle"] for s in a["stages"]],
                                                   "given": [s["given"] for s in a["stages"]], "remaining": [s["remaining"] for s in a["stages"]]},
                             title="The stages, what each gave and what remained")]
            first = [E[o["entity"]]["name"] for o in a["order"][:5]]
            out.append(api.note(f"{R['name']}. Reached first: {', '.join(first)}. " + " ".join(a["notes"]) +
                                f" Left unallocated: {A.money(a['left'], unit)}. Every amount and parameter here is a declaration of the example, illustrative and "
                                f"no authority's; the round's digest is {a['digest'][:12]}…"))
            return out

    return Round()
