"""A participatory budget decided again by votes: the projects each rule funds from the same ballots (utilitarian greedy, as most cities
count; the Method of Equal Shares; sequential Phragmén) beside what the city selected, with the voters left with none of their projects
funded, overall and neighbourhood by neighbourhood."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _allocation as A  # noqa: E402


def create(api):
    class Budget(api.BoardAnalysis):
        id = "budget"
        title = "A participatory budget, by votes, under several rules"
        description = ("The ballots of a participatory budget from Pabulib counted again under three rules: utilitarian greedy (the most approved "
                       "projects first while they fit, as most cities count), the Method of Equal Shares (each voter holds an equal share of the "
                       "budget and pays for the projects they approve), and sequential Phragmén. Each selection, and the city's own, is read by how "
                       "many voters are left with none of their projects funded, overall and in each neighbourhood. Extended justified representation "
                       "is checked where the voters are few enough to enumerate; with tens of thousands it is not, and the tool says so. Pabulib states "
                       "no licence: its files stay in the cache and only these derived results are shown, with its credit.")
        params = [api.param("case", "Budget", "pabulib-Poland_Warszawa_2023", kind="text", help="a Pabulib case built by data/products/allocation_cases.py"),
                  api.param("satisfaction", "Equal shares counts satisfaction by", "cost", kind="text", help="cost or cardinality")]

        def run(self, board, params, context):
            C, B = A.budget_of(str(params.get("case") or "pabulib-Poland_Warszawa_2023").strip(), str(params.get("satisfaction") or "cost"))
            rules = B["rules"]
            cur = C.get("currency", "")
            out = [api.table("budget_rules", {"rule": [r["rule"] for r in rules], "projects_funded": [len(r["funded"]) for r in rules], "spent": [r["spent"] for r in rules],
                                              "voters_with_nothing": [r["voters_with_nothing"] for r in rules], "share_with_nothing": [r["share_with_nothing"] for r in rules],
                                              "mean_projects_funded_a_voter": [r["mean_funded"] for r in rules]},
                             title=f"{C['title']}: the rules compared ({cur})", show={"kind": "chart", "chart": "bar", "x": "rule", "y": "share_with_nothing", "how": "sum"})]
            names = C["neighbourhoods"]
            cols = {"neighbourhood": names, "voters": [rules[0]["groups"][k]["voters"] for k in range(len(names))]}
            for r in rules:
                cols[f"nothing_{r['rule']}"] = [r["groups"][k]["share_with_nothing"] for k in range(len(names))]
            out.append(api.table("budget_neighbourhoods", cols, title="The share of each neighbourhood's voters left with none of their projects funded"))
            out.append(api.note(A.budget_said(C, B)))
            return out

    return Budget()
