"""Before a table is shared: its k-anonymity, the rows at risk, and the diversity of a sensitive column."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Anonymity(api.BoardAnalysis):
        id = "anonymity"
        title = "Can it be shared? k-anonymity"
        description = ("Whether a table of people can be released: the columns someone could link to another source (age, place, occupation: the quasi-identifiers) "
                       "group the rows into classes of rows alike in all of them; the table is k-anonymous when every class has at least k rows (Sweeney 2002). "
                       "The rows in smaller classes are at risk and are listed; with a sensitive column, its l-diversity (Machanavajjhala et al. 2007) says how "
                       "many different values the poorest class holds. peira's disclosure control: what to generalise or withhold is the archive's to decide.")
        params = [api.param("table", "Table", "", kind="text", role="table"), c.cols(api, "quasi", "Quasi-identifiers"),
                  api.param("k", "k asked for", 5, kind="integer", minimum=2), c.col(api, "sensitive", "Sensitive column (optional)")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            quasi = c.words(params["quasi"])
            c.need(t, quasi + ([params["sensitive"]] if params["sensitive"] else []))
            columns = quasi + ([params["sensitive"]] if params["sensitive"] else [])
            values = {k: c.text(t, k) for k in columns}
            r = c.inquiry.ask("anonymity", columns=columns, rows=[[values[k][i] for k in columns] for i in range(t.rows)], quasi=quasi, k=int(params["k"]),
                              sensitive=params["sensitive"] or "")
            risk = [int(i) for i in r["at_risk"]]
            at_risk = {"row": [i + 1 for i in risk], **{k: [values[k][i] for i in risk] for k in quasi}}
            div = f"; the poorest class holds {int(r['distinct'])} distinct values of {params['sensitive']}" if params["sensitive"] else ""
            ok = r["k"] >= int(params["k"])
            return [api.table("at_risk", at_risk, title=f"Rows in classes of fewer than {int(params['k'])}"),
                    api.note(f"The table is {int(r['k'])}-anonymous over {', '.join(quasi)}: {'it meets' if ok else 'it does not meet'} the k of {int(params['k'])} asked for. "
                             f"{int(r['rows'])} rows in {int(r['classes'])} classes, {len(risk)} rows at risk, {int(r['uniques'])} alone in their class{div}.")]
    return Anonymity()
