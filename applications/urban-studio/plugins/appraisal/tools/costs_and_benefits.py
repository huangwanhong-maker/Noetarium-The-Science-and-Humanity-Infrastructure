"""Costs and benefits over years: present values at a declared rate, constant or declining, the internal rate of return, the year it pays
back, and the values at which the decision would turn."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _appraisal as a  # noqa: E402


def create(api):
    class CostsAndBenefits(api.BoardAnalysis):
        id = "costs_and_benefits"
        title = "Costs and benefits"
        description = ("A stream of benefits and costs by year, brought to the present at a declared rate, constant or declining by bands (as the "
                       "Green Book's 3.5% to year 30, then 3.0%, …): the net present value, the benefit–cost ratio, the internal rate of return, "
                       "the discounted payback year, and the switching values (by what factor benefits could fall, or costs rise, before it no "
                       "longer pays).")
        params = [api.param("table", "Streams", "streams", kind="text", role="table", help="one row a year"),
                  a.col(api, "year", "Year (0 = now)", "year"),
                  a.col(api, "benefit", "Benefits", "benefit"),
                  a.col(api, "cost", "Costs", "cost"),
                  api.param("rate", "Discount rate (%)", 3.5, kind="number", minimum=0),
                  api.param("bands", "Declining bands", "", kind="text", help="from_year:rate%, e.g. 0:3.5, 31:3.0, 76:2.5 (overrides the single rate)"),
                  api.param("currency", "Currency", "EUR", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            yr, b, c = a.nums(t, params["year"]), a.nums(t, params["benefit"]), a.nums(t, params["cost"])
            rows = sorted((int(y), bb or 0.0, cc or 0.0) for y, bb, cc in zip(yr, b, c) if y is not None)
            if not rows or rows[0][0] != 0:
                raise ValueError("the years start at 0 (now)")
            T = rows[-1][0] + 1
            ben, cos = [0.0] * T, [0.0] * T
            for y, bb, cc in rows:
                ben[y] += bb
                cos[y] += cc
            req = {"benefit": ben, "cost": cos, "rate": float(params["rate"]) / 100}
            if str(params.get("bands") or "").strip():
                req["bands"] = [[int(p.split(":")[0]), float(p.split(":")[1].rstrip("%")) / 100] for p in a.words(params["bands"])]
            r = a.appraisal.ask("cba", **req)
            cur = params["currency"]
            stream = {"year": list(range(T)), "benefit": ben, "cost": cos, "factor": r["factors"], "cumulative_present_value": r["cumulative"]}
            verdict = "pays" if r["npv"] > 0 else "does not pay"
            irr = f"{100 * r['irr']:.2f}%" if r.get("irr") is not None else "not defined (the net stream changes sign more than once, or never)"
            return [api.table("present_values", stream, title="The streams, their discount factors and the cumulative present value", roles={"time": "year", "amount": "cumulative_present_value"},
                              show={"kind": "chart", "chart": "line", "x": "year", "y": "cumulative_present_value", "how": "sum"}),
                    api.note(f"At {'the declared bands' if 'bands' in req else str(params['rate']) + '%'}, it {verdict}: benefits worth {r['benefits']:,.0f} {cur} against costs of "
                             f"{r['costs']:,.0f} {cur} today, a net present value of {r['npv']:,.0f} {cur}"
                             + (f" and a ratio of {r['ratio']:.2f}" if r.get("ratio") is not None else "")
                             + f". Its internal rate of return is {irr}"
                             + (f"; it pays back in year {r['payback']:.0f}" if r.get("payback") is not None else "; it does not pay back within the years given")
                             + (f". The decision turns if benefits fall to {100 * r['benefit_switch']:.0f}% of what is expected, or costs rise to "
                                f"{100 * r['cost_switch']:.0f}%." if r.get("benefit_switch") is not None and r.get("cost_switch") is not None else ".")),
                    api.note("Declared, not observed: the rate, and that every benefit and cost has a price. What has none is left out of the sum and "
                             "should be set beside it, not dropped; whom the benefits fall on can be weighted (nomos's distributional weights).")]
    return CostsAndBenefits()
