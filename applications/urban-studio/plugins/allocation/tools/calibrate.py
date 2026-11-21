"""An economy of places' habits calibrated to records (ADR 0141): the accounts first, then behaviour. From an input–output table, the
share of what households buy that is made at home, and a firm's wages and domestic supplies over its output, are read, not fitted; the
share of its holding a household spends in a month is then matched, by simulated runs, to the velocity of households' money (their
consumption over the money stock). What the records leave unidentified is said, and a velocity the declared places cannot reach is
refused with the range they can."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _allocation as A  # noqa: E402
from science import allocation as al  # noqa: E402


def create(api):
    class Calibrate(api.BoardAnalysis):
        id = "calibrate"
        title = "Calibrate an economy's habits to records"
        description = ("The economy of places the allocation is evaluated in (households and firms whose every payment is posted) given habits "
                       "from records: from the EU27's input–output table of 2022, the share of households' purchases made at home and a firm's wages "
                       "and domestic supplies over its output, read; then the share of its holding a household spends in a month, matched by "
                       "simulated runs to the euro area's velocity of households' money that year. The places are the case's areas, one household "
                       "of the model for every thousand residents. The records are a continent's, not the case's: what that leaves unidentified is "
                       "said. Eurostat's data with the source acknowledged; the ECB's with the ECB cited as the source.")
        params = [api.param("case", "Areas", "melbourne-areas", kind="text", help="a case of areas built by data/products/allocation_cases.py"),
                  api.param("short_below", "A household is short below (A$ thousands for a thousand residents)", 300, minimum=0, step=50)]

        def run(self, board, params, context):
            D = A.case(str(params.get("case") or "melbourne-areas").strip())
            ents = [e for e in D["entities"] if e["at"].get("population", 0) >= 500]
            places = A.economy_places(ents)
            cal = A.calibrated_habits(places, float(params.get("short_below") if params.get("short_below") not in (None, "") else 300))
            C, H = cal["calibration"], cal["record"]
            h = C["habits"]
            out = [api.table("calibrated_habits", {
                "habit": ["households' local share", "... when short of money", "firms' wages, over what they receive", "firms' domestic supplies",
                          "share of holdings spent a month", "short of money below", "outside spends at the places' firms"],
                "value": [h["local"], h["short_local"], h["wages"], h["supplies"], h["spend"], h["short_below"], h["exports"]],
                "how": ["read: the accounts (domestic over all of households' final consumption)", "declared", "read: compensation of employees over output",
                        "read: domestic intermediate use over output", f"fitted: the velocity {C['target']:.4f} a month, reached {C['achieved']:.4f} in {C['runs']} runs",
                        "declared", "declared"]}, title="The economy's habits, and how each was found")]
            out.append(api.table("calibration_places", {"area": [p["name"] for p in places], "households": [p["households"] for p in places], "firms": [p["firms"] for p in places],
                                                        "opening_holding": [p["household_money"] for p in places]},
                                 title="The places of the economy (one household for every thousand residents; holdings declared, less where more are on low incomes)"))
            out.append(api.note(
                f"The habits are calibrated to {H['accounts']['source']} and {H['velocity_source']} ({H['velocity']:.4f}); these places reach velocities "
                f"from {C['lowest']:.4f} to {C['highest']:.4f}, and the share spent that matches is {h['spend']:.3f}. The records are a continent's: its "
                f"domestic share ({h['local']:.1%}) bounds from above what an area within that economy buys at home, and the case's areas are not within it, "
                f"so here it is a declaration borrowed from records, not a measurement. "
                f"Not identified by the records: {'; '.join(C['unidentified'])}. {H['credit']}."))
            return out

    return Calibrate()
