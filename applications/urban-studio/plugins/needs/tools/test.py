"""Statements of need, put to every test: the gap on the areas the search may see, its stability, causal support, the simulation of
seeking, the declared rules, and the standing their argument gives."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _needs as n  # noqa: E402


def create(api):
    class Test(api.BoardAnalysis):
        id = "test"
        title = "Test statements of need"
        description = ("Each line a statement in the language of needs (need care for age >= 75 by comparative need_assist higher). Each is read and "
                       "checked against the data, or refused with the reason; each that reads is put to every test on the half of the areas a search may "
                       "see: its gap with its interval (the areas' clusters as the design), its stability over resampled areas, causal support where the "
                       "data declare a provision and a graph, the simulation of seeking where they declare suppliers, the declared rules, and its standing.")
        params = n.case_params(api) + [api.param("statements", "Statements, one to a line",
                                                 "need care for age >= 75 by comparative need_assist higher\nneed income for age >= 65 and sex = female by comparative low_income higher",
                                                 kind="text", help="one statement to a line")]

        def run(self, board, params, context):
            d, case = n.data_of(params)
            lines = [l.strip() for l in str(params.get("statements") or "").splitlines() if l.strip() and not l.strip().startswith("#")]
            answers = n.nd.ask("test", data=d, statements=lines)
            rows, words = [], []
            for a in answers:
                if a.get("error"):
                    words.append(f"Refused: {a['text']} ({a['error']}).")
                    continue
                ev = a["evidence"]
                rows.append(n.row(a["statement"], ev))
                g = ev["discovery"]
                words.append(f"{a['statement']}: {n.pct(g.get('group_value'), 2)} against {n.pct(g.get('reference_value'), 2)} "
                             f"({g.get('coverage', 0):,.0f} people, z {g.get('z') or 0:.1f}); {n.standing_words(ev)}.")
            out = [api.table("tested", n.columns(rows), title="Statements of need, tested")] if rows else []
            out.append(api.note(" ".join(words) or "No statement was given."))
            out.append(api.note("A statement tested here alone is not confirmed: confirmation is a search's, once for each statement it admits, on the areas "
                                "it never sees. A gap is not a cause; causal support tests the declared graph, not the world."))
            return out
    return Test()
