"""Latent classes: kinds of respondents found from their answers."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Classes(api.BoardAnalysis):
        id = "classes"
        title = "Latent classes"
        description = ("Kinds of respondents found from yes-or-no answers (1, 0, empty): each class's share and the probability of each answer within it, fitted by "
                       "EM from many starts (Goodman 1974), with each respondent's most likely class and the BIC to compare numbers of classes. tyche's latent.")
        params = [api.param("table", "Responses", "", kind="text", role="table"), c.cols(api, "items", "Items", help="1, 0, or empty"),
                  api.param("classes", "Classes", 2, kind="integer", minimum=1, maximum=8),
                  api.param("seed", "Seed", 20261115, kind="integer")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            items = c.words(params["items"])
            c.need(t, items)
            values = {k: c.numbers(t, k) for k in items}
            responses = [[-1 if not math.isfinite(values[k][i]) else int(values[k][i]) for k in items] for i in range(t.rows)]
            r = c.inquiry.ask("classes", items=items, responses=responses, classes=int(params["classes"]), seed=int(params["seed"]))
            K = len(r["prior"])
            profile = {"item": items}
            for k in range(K):
                profile[f"class_{k + 1}"] = [r["probability"][k][j] for j in range(len(items))]
            columns = {name: t.raw(name) for name in t.columns}
            columns["class"] = [int(m) + 1 for m in r["modal"]]
            return [api.table("classes", profile, title="P(answer = 1 | class)"),
                    api.table(f"{t.name}_classes", columns, title=f"{t.title or t.name}, each with its most likely class"),
                    api.note(f"{K} classes of shares {', '.join(f'{p:.2f}' for p in r['prior'])}; log-likelihood {r['log_likelihood']:.2f}, BIC {r['bic']:.1f}"
                             + ("" if r["converged"] else "; not converged") + ".")]
    return Classes()
