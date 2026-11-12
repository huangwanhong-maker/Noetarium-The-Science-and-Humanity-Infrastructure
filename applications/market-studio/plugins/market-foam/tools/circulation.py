"""Money's round: how often what is held is passed on."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402


def create(api):
    class Circulation(api.BoardAnalysis):
        id = "circulation"
        title = "Money's round"
        description = ("For one kind of a record, period by period: what stood (the holdings above nothing, added up and averaged over the period), what passed between "
                       "subjects, and the velocity, what passed for each unit that stood for a unit of time. It is the equation of exchange read as a count of "
                       "events: the same exchanges counted once by their money and once by their goods.")
        params = [*r.common(api), r.opening(api),
                  api.param("of", "Of the kind", "", kind="text", help="the kind followed; the kind moved most often when left out"),
                  api.param("periods", "Periods", 10, kind="integer", minimum=1, maximum=400)]

        def run(self, board, params, context):
            s = r.stated(board, params)
            k = r.kind_index(s, params.get("of") or None)
            rec = s["record"]
            edges = np.linspace(rec["begin"], rec["end"], int(params.get("periods", 10)) + 1)
            a = r.ask("circulation", record=rec, kind=k, edges=[float(e) for e in edges])
            o = a["overall"]
            if not o["area"]:
                raise ValueError(f"no holding of {s['kinds'][k]} stands above nothing in the record: give the opening positions, without which a holding is read only as its change")
            stated = "opening" in rec
            return [
                api.table("round", {"from": np.round(edges[:-1], 6), "until": np.round(edges[1:], 6), "stood": np.round(a["stock"], 6), "passed": np.round(a["moved"], 6),
                                    "movements": [int(c) for c in a["movements"]], "velocity": [None if v is None else round(v, 6) for v in a["velocity"]]},
                          title=f"The round of {s['kinds'][k]}: what stood, what passed, and the velocity, period by period", roles={"amount": "velocity"},
                          show={"kind": "chart", "chart": "line", "x": "from", "y": "velocity", "how": "sum"}),
                api.note(f"Over the record {o['moved']:,.6g} of {s['kinds'][k]} passed between subjects in {o['movements']:,} movements, while {o['area'] / (rec['end'] - rec['begin']):,.6g} stood on average: "
                         f"a velocity of {o['velocity']:.4g} a unit of time ({s['unit']})."
                         + ("" if stated else " No opening positions were given: what stood is read from the changes alone, and is too small by what was held and never moved.")),
            ]
    return Circulation()
