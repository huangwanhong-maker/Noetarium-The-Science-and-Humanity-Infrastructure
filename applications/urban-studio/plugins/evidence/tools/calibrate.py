"""A kind of report's channel, counted: how often it is made where a reference finds the thing, and where the reference finds none."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _evidence as e  # noqa: E402


def create(api):
    class Calibrate(api.BoardAnalysis):
        id = "calibrate"
        title = "A kind of report's channel, counted"
        description = ("A table of units (a place in a period) that a reference looked at, each with what the reference found and whether a report of "
                       "the kind was made there. The hit rate is the share of the reference's findings that were reported, the false-alarm rate the "
                       "share of its clearances that were, each with Wilson's interval and the exact one; and the likelihood ratios of a report and of "
                       "a silence. It is what a source's channel in an archive can be set to, counted and said to be counted.")
        params = [api.param("table", "Units", "calibration", kind="text", role="table", help="one row a unit the reference looked at"),
                  api.param("reference", "What the reference found", "reference", kind="text", role="column", of="table"),
                  api.param("found", "Its word for finding the thing", "found", kind="text"),
                  api.param("reported", "Whether a report was made", "reported", kind="text", role="column", of="table", help="yes or no, true or false, 1 or 0"),
                  api.param("confidence", "Confidence", 0.95, kind="number", minimum=0.5, maximum=0.999, step=0.005)]

        def run(self, board, params, context):
            t = board.table(params["table"])
            ref, rep = e.texts(t, params["reference"]), e.texts(t, params["reported"])
            yes = {"yes", "true", "1", "y", "reported"}
            cells = [{"reference": r == str(params["found"]).strip(), "reported": p.lower() in yes} for r, p in zip(ref, rep) if r]
            r = e.se.calibrate(cells, confidence=float(params["confidence"]))
            h, f = r["hit"], r["false_alarm"]
            pct = lambda x: f"{100 * x:.1f}%"
            out = [api.table("channel", {"rate": ["hit: reported where found", "false alarm: reported where cleared"], "estimate": [h["rate"], f["rate"]],
                                         "reported": [h["reported"], f["reported"]], "of": [h["of"], f["of"]],
                                         "wilson_low": [h["wilson"][0], f["wilson"][0]], "wilson_high": [h["wilson"][1], f["wilson"][1]],
                                         "exact_low": [h["exact"][0], f["exact"][0]], "exact_high": [h["exact"][1], f["exact"][1]]},
                             title="The channel, counted", roles={"name": "rate"})]
            out.append(api.note(f"A report is made at {pct(h['rate'])} of the units where the reference found the thing ({h['reported']} of {h['of']}; "
                                f"{pct(h['wilson'][0])} to {pct(h['wilson'][1])} by Wilson's interval) and at {pct(f['rate'])} of those it cleared ({f['reported']} of {f['of']}; "
                                f"{pct(f['wilson'][0])} to {pct(f['wilson'][1])}). A report is then {r['ratio_of_a_report']:.3g} times as likely where the thing is, "
                                f"and a silence {1 / r['ratio_of_a_silence']:.3g} times as likely where it is not." if r.get("ratio_of_a_report") and r.get("ratio_of_a_silence") else
                                f"Hit rate {pct(h['rate'])}, false-alarm rate {pct(f['rate'])}."))
            out.append(api.note("It is a count on the units the reference looked at. Where the reference looks because reports were made, both rates are higher "
                                "than they would be on units chosen otherwise, and the count cannot say by how much."))
            return out
    return Calibrate()
