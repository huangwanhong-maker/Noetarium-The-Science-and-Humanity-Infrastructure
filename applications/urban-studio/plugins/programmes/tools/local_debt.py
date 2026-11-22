"""The debt of a country's local government sector, from Eurostat's accounts: its share of GDP over the years, the primary balance
that would hold it where it is, and a fan of the years ahead drawn from the record's own joint variation in interest, growth and
balance, with the share of paths above a declared threshold."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from science import fiscal as fs  # noqa: E402


def create(api):
    class LocalDebt(api.BoardAnalysis):
        id = "local_debt"
        title = "Local government debt of a European country, and its fan ahead"
        description = ("The local government sector of a European country from Eurostat (S1313): its debt as a share of GDP year by year, the "
                       "effective interest rate, nominal growth and primary balance; the primary balance that would hold the debt where it is; "
                       "and a fan of the years ahead, the record's joint variation drawn in blocks of years around a baseline of its recent "
                       "averages, with the share of paths above a declared threshold. Eurostat's data, reused with the source acknowledged.")
        params = [api.param("geo", "Country", "FR", kind="text", help="a Eurostat code (FR, DE, IT, ES, NL, PL, …)"),
                  api.param("years", "Years ahead", 10, minimum=1, step=1),
                  api.param("threshold", "Threshold (share of GDP)", 0.1, minimum=0.0, step=0.01)]

        def run(self, board, params, context):
            C = fs.local_government(str(params.get("geo") or "FR").strip().upper())
            rec = fs.record_of(C)
            H = int(params.get("years") or 10)
            recent = lambda v: sum(v[-5:]) / len(v[-5:])  # noqa: E731
            r, g, pb = [recent(rec["r"])] * H, [recent(rec["g"])] * H, [recent(rec["pb"])] * H
            d0 = next(v for v in reversed(C["debt_share"]) if v is not None)
            last = max(y for y, v in zip(C["years"], C["debt_share"]) if v is not None)
            thr = float(params.get("threshold") if params.get("threshold") not in (None, "") else 0.1)
            out_ = fs.debt(d0, r, g, pb, record=rec, block=2, draws=4000, threshold=thr, quantiles=[0.1, 0.5, 0.9], seed=1)
            hist = [(y, v) for y, v in zip(C["years"], C["debt_share"]) if v is not None]
            out = [api.table("local_debt_record", {"year": [y for y, _ in hist], "debt (share of GDP)": [round(v, 4) for _, v in hist]},
                             title=f"{C['name']}: local government debt, a share of GDP", show={"kind": "chart", "chart": "line", "x": "year", "y": "debt (share of GDP)"})]
            F = out_["fan"]
            years = [last + k for k in range(H + 1)]
            out.append(api.table("local_debt_fan", {"year": years, "lower (10%)": [round(q[0], 4) for q in F["paths_at"]], "median": [round(q[1], 4) for q in F["paths_at"]],
                                                    "upper (90%)": [round(q[2], 4) for q in F["paths_at"]], f"share above {thr:g}": [round(x, 3) for x in F["above"]]},
                                 title="The years ahead: a fan of 4,000 paths"))
            # The record and the fan on one chart: one line for the record, and the fan's three lines from the record's last year.
            lines = [("the record", y, v) for y, v in hist]
            for k, name in ((0, "lower (10%)"), (1, "median"), (2, "upper (90%)")):
                lines += [(name, y, q[k]) for y, q in zip(years, F["paths_at"])]
            out.append(api.table("local_debt_paths", {"year": [y for _, y, _ in lines], "line": [n for n, _, _ in lines], "debt (share of GDP)": [round(v, 4) for _, _, v in lines]},
                                 title=f"{C['name']}: the record and the fan of the years ahead",
                                 show={"kind": "chart", "chart": "line", "x": "year", "y": "debt (share of GDP)", "how": "mean", "split": "line"}))
            out.append(api.note(f"{C['name']}'s local government owed {100 * d0:.2f}% of GDP in {last}. At the recent averages (interest {100 * r[0]:.2f}%, growth {100 * g[0]:.2f}%, "
                                f"primary balance {100 * pb[0]:.2f}% of GDP), the balance that would hold the debt where it is is {100 * out_['stabilising_balance']:.3f}% of GDP; "
                                f"after {H} years the median path stands at {100 * F['paths_at'][-1][1]:.2f}%, and {100 * F['above'][-1]:.1f}% of paths lie above {100 * thr:g}%. "
                                f"The baseline repeats recent averages and the fan the record's own variation; neither is a forecast. {C['credit']}."))
            return out

    return LocalDebt()
