"""How far coders agree: the reliability of coded qualitative material, beyond what chance would give."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _appraisal as a  # noqa: E402


def create(api):
    class Agreement(api.BoardAnalysis):
        id = "agreement"
        title = "How far coders agree"
        description = ("Interviews, documents or observations coded by several people: how far their codes agree beyond chance. Krippendorff's α for "
                       "any number of coders and missing codes, at the level the codes are measured on; with two coders also Cohen's κ, and with "
                       "every unit coded by every coder Fleiss's κ. Krippendorff's own guide takes α ≥ 0.8 as reliable and 0.667 as the least for "
                       "tentative conclusions.")
        params = [api.param("table", "Codes", "codes", kind="text", role="table", help="one row a code: the unit, the coder, the code"),
                  a.col(api, "unit", "Unit", "unit"),
                  a.col(api, "coder", "Coder", "coder"),
                  a.col(api, "code", "Code", "code"),
                  api.param("metric", "Level of measurement", "nominal", kind="choice", options=["nominal", "ordinal", "interval", "ratio"])]

        def run(self, board, params, context):
            t = board.table(params["table"])
            units, coders, codes = a.texts(t, params["unit"]), a.texts(t, params["coder"]), a.texts(t, params["code"])
            ulist = sorted(set(units) - {""})
            clist = sorted(set(coders) - {""})
            labels = sorted(set(codes) - {""}, key=lambda x: (float(x) if x.replace(".", "", 1).lstrip("-").isdigit() else float("inf"), x))
            numeric = params["metric"] in ("interval", "ratio")
            if numeric:
                values = sorted({int(float(x)) for x in labels})
                if values and values[0] < 0:
                    raise ValueError("interval and ratio codes are whole numbers ≥ 0")
                K = (values[-1] + 1) if values else 1
                index = {x: int(float(x)) for x in labels}
            else:
                K = len(labels)
                index = {x: k for k, x in enumerate(labels)}
            grid = [[-1] * len(ulist) for _ in clist]
            for u, c, x in zip(units, coders, codes):
                if u and c and x:
                    grid[clist.index(c)][ulist.index(u)] = index[x]
            r = a.appraisal.ask("agreement", codes=grid, categories=K, metric=params["metric"])
            rows = {"coefficient": ["Krippendorff's α (" + params["metric"] + ")"], "value": [r["alpha"]]}
            for key, name in (("kappa", "Cohen's κ"), ("kappa_linear", "Cohen's κ, linear weights"), ("kappa_quadratic", "Cohen's κ, quadratic weights"), ("fleiss", "Fleiss's κ")):
                if key in r:
                    rows["coefficient"].append(name), rows["value"].append(r[key])
            al = r["alpha"]
            verdict = "reliable" if al >= 0.8 else "enough only for tentative conclusions" if al >= 0.667 else "not reliable enough to draw conclusions from"
            return [api.table("agreement", rows, title=f"Agreement of {len(clist)} coders on {len(ulist)} units", roles={"name": "coefficient", "amount": "value"}),
                    api.note(f"Krippendorff's α is {al:.3f}: by his guide, {verdict}. {len(clist)} coders, {len(ulist)} units, {K if not numeric else len(labels)} codes.")]
    return Agreement()
