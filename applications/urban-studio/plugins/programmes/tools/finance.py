"""A city's finances from its accounts: the lines summed through a declared chart, declared ratios judged against a declared scheme of
thresholds year by year, the years ahead with the loans it carries and the coverage of its debt service, and the largest new loan
it could service while keeping that coverage at a declared floor, bounded by a ceiling on its stock of debt."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _programmes as P  # noqa: E402
from science import fiscal as fs  # noqa: E402


def create(api):
    class Finance(api.BoardAnalysis):
        id = "finance"
        title = "Read a city's finances, and how much more it can borrow"
        description = ("A city's accounts summed through a declared chart; declared ratios (own-source revenue, the operating surplus, debt "
                       "service) judged against a declared scheme of warning and intervention thresholds, year by year; the years ahead with "
                       "the loans it carries; and the largest new loan it could service at a declared rate and tenor while keeping its debt "
                       "service coverage at a declared floor, bounded by a ceiling on its stock of debt. Every threshold, rate and floor is the "
                       "example's declaration, no jurisdiction's. The city and its accounts are invented.")
        params = [api.param("floor", "Coverage floor", 1.5, minimum=1.0, step=0.1),
                  api.param("rate", "New loan's rate", 0.05, minimum=0.0, step=0.005),
                  api.param("tenor", "New loan's tenor (years)", 10, minimum=1, step=1)]

        def run(self, board, params, context):
            E = P.example()
            A = E["accounts"]
            cap = dict(A["capacity"], floor=float(params.get("floor") or 1.5), rate=float(params.get("rate") or 0.05), tenor=int(params.get("tenor") or 10))
            r = fs.assess(fs.accounts(A["unit"], A["currency"], A["chart"], A["years"]), A["ratios"], A["thresholds"], A["projection"], cap)
            rv = r["ratios"]
            out = [api.table("finance_ratios", {"year": [x["year"] for x in rv], "ratio": [x["ratio"] for x in rv], "value": [None if x["value"] is None else round(x["value"], 4) for x in rv],
                                                "status": [x["status"] for x in rv]}, title="Ratios, judged against the declared scheme")]
            p = r["projection"]
            out.append(api.table("finance_years_ahead", {"year": [x["year"] for x in p], "surplus": [round(x["surplus"]) for x in p], "debt service": [round(x["debt_service"]) for x in p],
                                                         "coverage": [None if x["coverage"] is None else round(x["coverage"], 2) for x in p], "debt outstanding": [round(x["outstanding"]) for x in p]},
                                 title=f"The years ahead ({A['unit']})", show={"kind": "chart", "chart": "line", "x": "year", "y": "coverage"}))
            c = r["capacity"]
            name = next(p_["name"] for p_ in E["programme"]["participants"] if p_["id"] == A["city"])
            warn = [x for x in rv if x["status"] in ("warning", "intervention")]
            out.append(api.note(f"{name} could take a new loan of about {c['largest']:,.0f} {A['unit']} at {100 * cap['rate']:.1f}% over {cap['tenor']} years from {cap['first_year']}, "
                                f"keeping its coverage at {cap['floor']:g} or above; what binds it is {c['binding']}"
                                + (f" in {c['binding_year']}" if c["binding_year"] else "") + ". "
                                + (f"{len(warn)} ratio-year(s) cross a declared threshold: " + "; ".join(f"{x['ratio']} in {x['year']} ({x['status']})" for x in warn) + ". " if warn else "No ratio crosses a declared threshold. ")
                                + E["said"]))
            return out

    return Finance()
