"""A round against the rounds it could have been: every rationing rule a stage could have used and every order of its stages, compared
under declared principles at once. The undominated options, the best under each principle, the pivots, what is at stake for each
entity. Principles are never added together."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _allocation as A  # noqa: E402
from science import allocation as al  # noqa: E402


def create(api):
    class Judge(api.BoardAnalysis):
        id = "judge"
        title = "The round against its alternatives"
        description = ("The declared round is one of many that could have been declared: each rationing stage could have used another rule, "
                       "and the stages could have come in another order. Each alternative's result is read per person in need and compared under "
                       "the principles at once: the options no other beats under every principle, the principle under which each is best, and, "
                       "for the declared round against each other undominated option, which principles favour which. Where they disagree the "
                       "choice is a person's, and the tool says so rather than adding the principles into one score.")
        params = [api.param("case", "Case", "england-iod2025", kind="text", help="england-iod2025 or melbourne-areas"),
                  api.param("amount", "The sum", 1e9, minimum=0, step=1e6),
                  api.param("per_person", "Declared sum a person in need", 80, minimum=0, step=10),
                  api.param("rule", "Declared rule for claims", "proportional", kind="text", help=", ".join(al.RULES))]

        def run(self, board, params, context):
            name = str(params.get("case") or "england-iod2025").strip()
            R = A.declared_round(name, params)
            reading = A.READINGS[name]
            J = al.judge(R, A.PRINCIPLES[name], level=reading["level"], per=reading["per"])
            names = [o["names"][0] if k else "as declared" for k, o in enumerate(J["options"])]
            cols = {"option": names, "undominated": ["yes" if o["undominated"] else "no" for o in J["options"]], "made_also_by": ["; ".join(o["names"][1:]) for o in J["options"]]}
            for j, _p in enumerate(J["principles"]):
                cols[f"p{j + 1}"] = [o["values"][j] for o in J["options"]]
            labels = {f"p{j + 1}": p for j, p in enumerate(J["principles"])}
            E = {e["id"]: e["name"] for e in R["entities"]}
            st = sorted(J["stakes"], key=lambda s: s["least"] - s["most"])[:30]
            out = [api.table("allocation_options", cols, title=f"{R['name']}: the options under {', '.join(J['principles'])}", labels=labels),
                   api.table("allocation_stakes", {"entity": [E[s["entity"]] for s in st], "least": [s["least"] for s in st], "most": [s["most"] for s in st],
                                                   "declared": [s["declared"] for s in st]},
                             title="What is at stake among the undominated options, widest first")]
            pivots = " ".join(f"Against {names[p['option']]}: {p['pivot']}." for p in J["pivots"][:4])
            out.append(api.note(". ".join(J["said"]) + ". " + (pivots or "No undominated alternative disagrees with the declared round.") +
                                " Read per " + reading["per"].replace("_", " ") + "; every amount and parameter is the example's declaration."))
            return out

    return Judge()
