"""A stock over time: what is in use, from what enters it each year, or what must enter to keep it on a path, under a declared lifetime law."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402


def create(api):
    class StockOverTime(api.BoardAnalysis):
        id = "stock_over_time"
        title = "A stock over time"
        description = ("Any goods in use (vehicles, appliances, pipes, buildings) as cohorts that leave by a declared lifetime law. From what enters "
                       "each year, the stock and what leaves; or from the stock's path, what must enter (Müller 2006), with early retirements where "
                       "the path falls faster than cohorts leave. Mass balances by construction; the balance is reported.")
        params = [api.param("table", "Series", "series", kind="text", role="table", help="one row a year"),
                  m.col(api, "year", "Year", "year"),
                  m.col(api, "values", "Values", "inflow", help="what enters each year, or the stock at each year's end"),
                  api.param("mode", "The values are", "inflow", kind="choice", options=["inflow", "stock"])] + m.lifetime_params(api, mean=15.0, spread=2.5)

        def run(self, board, params, context):
            t = board.table(params["table"])
            years, values = m.nums(t, params["year"]), m.nums(t, params["values"])
            pairs = sorted((y, v) for y, v in zip(years, values) if y is not None and v is not None)
            if len(pairs) < 2:
                raise ValueError("at least two years with a value")
            ys = [int(y) for y, _ in pairs]
            if any(b - a != 1 for a, b in zip(ys, ys[1:])):
                raise ValueError("one value a year, with no year missing: a cohort's age is counted in whole years")
            a = m.materials.ask("stock", mode=params["mode"], values=[v for _, v in pairs], lifetime=m.lifetime(params))
            rows = {"year": ys, "inflow": a["inflow"], "stock": a["stock"], "outflow": a["outflow"], "retired_early": a["retired_early"],
                    "mean_age": [round(v, 2) for v in a["mean_age"]]}
            long = {"year": [], "series": [], "value": []}
            for k, y in enumerate(ys):
                for s in ("inflow", "outflow", "stock"):
                    long["year"].append(y), long["series"].append(s), long["value"].append(a[s][k])
            early = sum(a["retired_early"])
            return [api.table("stock_model", rows, title=f"The stock, its inflow and outflow, {ys[0]}–{ys[-1]}", roles={"time": "year", "amount": "stock"}),
                    api.table("stock_series", long, title="Inflow, outflow and stock by year", roles={"time": "year", "category": "series", "amount": "value"},
                              show={"kind": "chart", "chart": "line", "x": "year", "y": "value", "split": "series", "how": "sum"}),
                    api.note(f"Under {m.lifetime_words(params)}, the stock goes from {a['stock'][0]:,.0f} to {a['stock'][-1]:,.0f}, its mean age from "
                             f"{a['mean_age'][0]:.1f} to {a['mean_age'][-1]:.1f} years; {sum(a['outflow']):,.0f} leave over the years. The balance "
                             f"S(t) − S(t − 1) = I(t) − O(t) holds to {a['balance']:.1e}."
                             + (f" The path asks for {early:,.0f} to be retired early, where the stock falls faster than cohorts leave." if early > 0 else "")),
                    api.note("Presumed: the lifetime law. The stock starts empty before the first year; a stock in use before it is a work's to state.")]
    return StockOverTime()
