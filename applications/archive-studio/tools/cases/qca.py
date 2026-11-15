"""Qualitative comparative analysis: the truth table and its solutions, every minimal model."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class QCA(api.BoardAnalysis):
        id = "qca"
        title = "Qualitative comparative analysis"
        description = ("Which combinations of conditions are sufficient for an outcome, across cases given as memberships of sets (crisp 0/1 or fuzzy). "
                       "The truth table codes each configuration by its consistency and its cases; the conservative solution uses only what was observed, "
                       "the parsimonious every remainder, and the intermediate (when the expected direction of each condition is declared) only the easy "
                       "counterfactuals. Every minimal model is given, never one chosen silently (Ragin 2008; Baumgartner and Thiem 2017). Each condition "
                       "and its negation is also tested as necessary.")
        params = [api.param("table", "Cases", "", kind="text", role="table"),
                  c.col(api, "case", "Case"),
                  c.cols(api, "conditions", "Conditions", help="memberships between 0 and 1, at most 16"),
                  c.col(api, "outcome", "Outcome"),
                  api.param("consistency", "Least consistency of a sufficient row", 0.8, kind="number", minimum=0, maximum=1, step=0.05),
                  api.param("frequency", "Fewest cases in a coded row", 1, kind="integer", minimum=1),
                  api.param("pri", "Least PRI of a sufficient row", 0.5, kind="number", minimum=0, maximum=1, step=0.05,
                            help="the proportional reduction in inconsistency: a row consistent with the outcome and with its absence alike is not coded sufficient"),
                  api.param("expect", "Expected directions", "", kind="text", help="one for each condition, in order: + present, - absent, 0 either (for the intermediate solution)"),
                  api.param("negate", "Explain the outcome's absence", "no", kind="choice", options=["no", "yes"])]

        def run(self, board, params, context):
            t = board.table(params["table"])
            conds = c.words(params["conditions"])
            if not conds:
                raise ValueError("name the conditions")
            rows, dropped = c.complete(t, conds + [params["outcome"]], [params["case"]])
            cases = c.text(t, params["case"])
            data = {k: c.numbers(t, k) for k in conds + [params["outcome"]]}
            request = {"cases": [cases[i] for i in rows], "conditions": conds, "members": [[data[k][i] for k in conds] for i in rows],
                       "outcome": [data[params["outcome"]][i] for i in rows], "outcome_name": params["outcome"], "consistency": params["consistency"], "pri": params.get("pri", 0.5),
                       "frequency": params["frequency"], "negate": params["negate"] == "yes"}
            if c.words(params["expect"].replace(" ", ",")):
                signs = [w for w in params["expect"].replace(",", " ").split() if w]
                if len(signs) != len(conds):
                    raise ValueError(f"give one expected direction for each of the {len(conds)} conditions")
                request["expectations"] = [{"+": 1, "-": -1, "0": 0}.get(x, 0) for x in signs]
            r = c.inquiry.ask("qca", **request)
            tt = {"configuration": [], "cases": [], "n": [], "consistency": [], "pri": [], "code": [], "deviant": []}
            for row in r["rows"]:
                tt["configuration"].append(row["configuration"]), tt["cases"].append(", ".join(row["cases"])), tt["n"].append(len(row["cases"]))
                tt["consistency"].append(row["consistency"]), tt["pri"].append(row["pri"]), tt["code"].append(row["code"]), tt["deviant"].append(", ".join(row["deviant"]))
            sol = {"solution": [], "model": [], "term": [], "consistency": [], "raw_coverage": [], "unique_coverage": [], "cases": []}
            said = []
            for s in r["solutions"]:
                models = [m["model"] for m in s["models"]]
                said.append(f"{s['kind']}: {' | '.join(models) if models else 'none (no row is sufficient)'}")
                for k, m in enumerate(s["models"]):
                    for term in m["terms"]:
                        sol["solution"].append(s["kind"]), sol["model"].append(f"M{k + 1}: {m['model']}"), sol["term"].append(term["term"])
                        sol["consistency"].append(term["consistency"]), sol["raw_coverage"].append(term["raw_coverage"]), sol["unique_coverage"].append(term["unique_coverage"])
                        sol["cases"].append(", ".join(term["cases"]))
            nec = {"condition": [n["condition"] for n in r["necessity"]], "consistency": [n["relation"]["consistency"] for n in r["necessity"]],
                   "coverage": [n["relation"]["coverage"] for n in r["necessity"]], "relevance": [n["relation"].get("relevance") for n in r["necessity"]]}
            outcome = ("~" if params["negate"] == "yes" else "") + params["outcome"]
            return [api.table("truth_table", tt, title=f"Truth table for {outcome}"),
                    api.table("solutions", sol, title=f"Solutions for {outcome}"),
                    api.table("necessity", nec, title=f"Necessary conditions for {outcome}"),
                    api.note(f"{len(rows)} cases, {len(conds)} conditions, {len(r['rows'])} configurations observed, {int(r['remainders'])} remainders. "
                             + "; ".join(said) + "." + (f" Cases at exactly 0.5 somewhere, in no row: {', '.join(r['unplaced'])}." if r["unplaced"] else "") + c.left_out(dropped))]
    return QCA()
