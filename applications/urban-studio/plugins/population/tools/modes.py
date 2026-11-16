"""The record's modes: a place's population field decomposed into dynamic modes, each a map that grows or fades at its own rate."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _population as p  # noqa: E402


def create(api):
    class Modes(api.BoardAnalysis):
        id = "modes"
        title = "The record's modes"
        description = ("The field's dynamic modes, as fluid dynamics finds the coherent structures of a flow (exact DMD by the method of snapshots, in "
                       "kinesis): each a map with a rate at which it grows or fades and, where it turns, a period. The first carries most of the field; the "
                       "others say where the field changed apart from it.")
        params = p.case_params(api) + [api.param("show", "Mode maps", 3, minimum=1, maximum=6, step=1)]

        def run(self, board, params, context):
            f, case = p.field_of(params)
            m = p.pd.ask("modes", field=f, show=int(params.get("show") or 3))
            modes = m["modes"]
            out = [api.table("modes", {"mode": list(range(1, len(modes) + 1)), "rate_a_year": [p.num(x["rate_a_year"]) for x in modes],
                                       "period_years": [p.num(x.get("period_years")) for x in modes], "size": [p.num(x["size"]) for x in modes],
                                       "eigenvalue_real": [x["eigenvalue"][0] for x in modes], "eigenvalue_imaginary": [x["eigenvalue"][1] for x in modes]},
                             title="The record's dynamic modes", show={"kind": "chart", "chart": "bar", "x": "mode", "y": "size", "how": "sum"})]
            cs = p.cells(f, case)
            cols = {"cell": [f"r{c['row']:03d}c{c['col']:03d}" for c in cs], "lat": [c.get("lat") for c in cs], "lon": [c.get("lon") for c in cs]}
            for k, x in enumerate(modes):
                if x.get("map"):
                    cols[f"mode_{k + 1}"] = x["map"]
            if len(cols) > 3:
                out.append(api.table("mode_maps", cols, title="The modes' maps, as each stood in the first year", roles={"lat": "lat", "lon": "lon", "name": "cell", "amount": "mode_2" if "mode_2" in cols else "mode_1"},
                                     show={"kind": "layer", "layer": "points", "colour": "mode_2" if "mode_2" in cols else "mode_1", "map": "lavender-rust"}))
            words = [f"{m['rank']} modes from {len(m['singular_values']) + 1} snapshots {m['step']:.0f} years apart; they carry each snapshot to the next within {p.pct(m['residual'])}."]
            for k, x in enumerate(modes[:4]):
                s = f"Mode {k + 1} {'grows' if x['rate_a_year'] > 0 else 'fades'} at {p.pct(abs(x['rate_a_year']))} a year"
                if x.get("period_years") and math.isfinite(x["period_years"]):
                    s += f" and turns every {x['period_years']:.0f} years"
                words.append(s + ".")
            out.append(api.note(" ".join(words)))
            out.append(api.note("With a dozen snapshots every mode rests on few numbers, and a mode that grows is extrapolated without limit: the modes describe "
                                "the record, and forecast it only as a linear law in the span of what was seen."))
            return out
    return Modes()
