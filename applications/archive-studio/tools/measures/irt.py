"""Item response theory: each item's difficulty and discrimination, and each person's score."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class IRT(api.BoardAnalysis):
        id = "irt"
        title = "Item response theory"
        description = ("Items answered right or wrong, yes or no (1 or 0; an empty cell is not asked): each item's difficulty and, in the two-parameter model, its "
                       "discrimination, fitted by marginal maximum likelihood (Bock and Aitkin 1981); each person's score as the posterior mean of the trait with "
                       "its standard deviation; and the information the test gives along the trait. nomos's psychometrics.")
        params = [api.param("table", "Responses", "", kind="text", role="table"), c.cols(api, "items", "Items", help="1, 0, or empty"),
                  c.col(api, "person", "Person (optional)"),
                  api.param("model", "Model", "2pl", kind="choice", options=["2pl", "rasch"])]

        def run(self, board, params, context):
            t = board.table(params["table"])
            items = c.words(params["items"])
            c.need(t, items)
            values = {k: c.numbers(t, k) for k in items}
            responses = []
            for i in range(t.rows):
                row = [-1 if not math.isfinite(values[k][i]) else int(values[k][i]) for k in items]
                if any(x not in (-1, 0, 1) for x in row):
                    raise ValueError("responses are 1, 0 or empty")
                responses.append(row)
            r = c.inquiry.ask("irt", items=items, responses=responses, model=params["model"])
            people = c.text(t, params["person"]) if params["person"] else [str(i + 1) for i in range(t.rows)]
            info = r["information"]
            return [api.table("items", {"item": [x["item"] for x in r["items"]], "difficulty": [x["difficulty"] for x in r["items"]],
                                        "discrimination": [x["discrimination"] for x in r["items"]]}, title=f"Items ({r['model']})"),
                    api.table("scores", {"person": people, "score": [s["eap"] for s in r["scores"]], "sd": [s["sd"] for s in r["scores"]]}, title="Each person's score"),
                    api.table("information", {"trait": [p[0] for p in info], "information": [p[1] for p in info]}, title="The test's information along the trait",
                              show={"kind": "chart", "chart": "lines", "x": "trait", "y": "information"}),
                    api.note(f"{len(items)} items, {t.rows} people, the {r['model']} model; log-likelihood {r['log_likelihood']:.2f}"
                             + ("" if r["converged"] else f", not converged after {int(r['iterations'])} iterations") + ".")]
    return IRT()
