"""Loops through time: the same two kinds exchanged at two times, and held between."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402


def rates(text: str, kinds: list) -> list:
    """Lines `kind: rate` as the carry of holding each kind, per unit of the record's time; nothing for a kind not named."""
    carry = [0.0] * len(kinds)
    for raw in str(text or "").splitlines():
        line = raw.strip()
        if not line:
            continue
        name, colon, value = line.rpartition(":")
        if not colon or name.strip() not in kinds:
            raise ValueError(f"a line of the carries is  kind: rate , of a kind of the record ({', '.join(kinds)}): '{line}'")
        carry[kinds.index(name.strip())] = float(value)
    return carry


def create(api):
    class Loops(api.BoardAnalysis):
        id = "loops"
        title = "Loops through time"
        description = ("Two exchanges of the same two kinds at two times, with the two holdings between them, make the smallest loop that takes time. What is gained "
                       "or lost round it has two parts: one set by the two ratios, whatever the time between; one that grows with the time, at the difference of what "
                       "holding each kind carries (interest, storage, decay). A loop with neither is the cost of carry; for two currencies, covered interest parity.")
        params = [*r.common(api),
                  api.param("carry", "Holding carries", "", kind="text", help="lines  kind: rate  per unit of the record's time; nothing for a kind not named"),
                  api.param("pair", "Of the kinds", "", kind="text", help="two kinds with a comma between; every pair when left out")]

        def run(self, board, params, context):
            s = r.stated(board, params)
            kinds = s["kinds"]
            carry = rates(params.get("carry"), kinds)
            a = r.ask("reciprocity", record=s["record"], carry=carry)["lenses"]
            k, l = np.array(a["k"], dtype=int), np.array(a["l"], dtype=int)
            keep = np.ones(len(k), dtype=bool)
            if str(params.get("pair") or "").strip():
                names = [x.strip() for x in str(params["pair"]).split(",")]
                if len(names) != 2 or any(n not in kinds for n in names):
                    raise ValueError(f"the pair is two kinds of the record with a comma between ({', '.join(kinds)})")
                want = sorted(kinds.index(n) for n in names)
                keep = (k == want[0]) & (l == want[1])
            if not keep.any():
                raise ValueError("no two kinds are exchanged twice in the slice: a loop needs two exchanges of the same pair")
            begin, end = np.array(a["begin"])[keep], np.array(a["end"])[keep]
            geo, dyn = np.array(a["geometric"])[keep], np.array(a["dynamical"])[keep]
            h = geo + dyn
            return [
                api.table("loops", {"of": [kinds[i] for i in k[keep]], "for": [kinds[i] for i in l[keep]], "from": begin, "until": end, "lasting": np.round(end - begin, 6),
                                    "by_the_ratios": np.round(geo, 6), "by_the_time": np.round(dyn, 6), "round_the_loop": np.round(h, 6)},
                          title="Each loop of two exchanges and two holdings: what is gained round it, by the ratios and by the time",
                          roles={"amount": "round_the_loop", "category": "of"}, show={"kind": "chart", "chart": "scatter", "x": "lasting", "y": "round_the_loop"}),
                api.note(f"{int(keep.sum()):,} loops. Round them the mean gain is {h.mean():+.4f} (in log units), {np.abs(h).mean():.4f} in size: "
                         f"{np.abs(geo).mean():.4f} by the ratios and {np.abs(dyn).mean():.4f} by the time. "
                         + ("No carry was given, so every loop is read by its ratios alone. " if not any(carry) else "")
                         + "A change of the units of a kind moves neither part; a change of units through time (an inflation every rate follows) moves value from the one to the other and leaves their sum."),
            ]
    return Loops()
