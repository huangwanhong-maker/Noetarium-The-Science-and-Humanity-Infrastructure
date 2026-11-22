"""Can the city afford the project, with and without the land value it captures? The project's cost year by year met by the receipts
of the instruments chosen (and grants), a reserve that carries what is left, and new loans for the rest, on the city's own years
ahead; its coverage of debt service against the floor, and the largest scale of the project it can afford with the receipts and
without them."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _programmes as P  # noqa: E402
from science import fiscal as fs  # noqa: E402


def create(api):
    class Project(api.BoardAnalysis):
        id = "project"
        title = "Can the city afford the project, with and without the land value it captures?"
        description = ("The project's cost year by year is met first by the receipts of the instruments chosen (a betterment levy, development "
                       "rights, tax increment, a developer obligation), then by new loans; receipts left over after the cost are pledged to the "
                       "new loans' service, or kept in a reserve. Capital receipts never enter the operating surplus. The city's coverage of its "
                       "debt service is read year by year against the floor, and the largest scale of the project it could afford is found with "
                       "the receipts and without them (a scale of 1 is the project as declared). The city, its accounts and the project are "
                       "invented, and every rate and floor is a declaration.")
        params = [api.param("instruments", "Instruments whose receipts pay for it", "levy, rights, increment, obligation", kind="text"),
                  api.param("valuation_year", "The year of the valuation date", 2026, minimum=2026, step=1),
                  api.param("years", "Years ahead", 20, minimum=5, maximum=40, step=1),
                  api.param("rate", "New loans' rate", 0.05, minimum=0.0, step=0.005),
                  api.param("tenor", "New loans' tenor (years)", 20, minimum=1, step=1),
                  api.param("floor", "Coverage floor", 1.5, minimum=1.0, step=0.1),
                  api.param("pledge", "Receipts left over pay the new loans' service", True, kind="boolean")]

        def run(self, board, params, context):
            E = P.example()
            A, J = E["accounts"], E["project"]
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            y0 = int(g("valuation_year", 2026))
            chosen = {x.strip() for x in str(params.get("instruments") or "").split(",") if x.strip()}
            parcels = [fs.parcel(p["id"], p["area"], p["without"], p["with"], p.get("frontage", 0), p.get("x", 0), p.get("y", 0)) for p in J["parcels"]]
            ins = {k: dict(v) for k, v in J["instruments"].items() if k in chosen}
            unknown = chosen - set(J["instruments"])
            if unknown:
                raise ValueError(f"the project declares no instrument {', '.join(sorted(unknown))}: {', '.join(J['instruments'])}")
            cap = fs.capture_compare(parcels, J["cost"], ins, rate=J["rate"], statutory=J.get("statutory")) if ins else {"payments": {}}
            projection = dict(A["projection"], years=int(g("years", 20)))
            last = projection["base_year"] + projection["years"]
            receipts, outside = [], 0.0
            for name, pays in cap["payments"].items():
                for p in pays:
                    year = y0 + int(p["year"])
                    if year > last:
                        outside += p["amount"]
                        continue
                    receipts.append({"year": year, "amount": p["amount"], "source": name})
            cost = [{"year": y0 + int(c["year"]), "amount": c["amount"], "source": J["title"]} for c in J["cost"]]
            plan = {"cost": cost, "receipts": receipts, "grants": [], "rate": g("rate", 0.05), "tenor": int(g("tenor", 20)), "reserve_rate": 0.0,
                    "pledge": params.get("pledge") not in (False, "false", "0", 0)}
            floor = g("floor", 1.5)
            r = fs.ask("fiscal.plan", accounts=fs.accounts(A["unit"], A["currency"], A["chart"], A["years"]), projection=projection, plan=plan, floor=floor, most=10)
            W, N = r["with_receipts"], r["without_receipts"]
            rows = list(zip(W["years"], N["years"]))
            cov = lambda v: None if v is None else round(v, 3)  # noqa: E731
            out = [api.table("project_years", {
                "year": [w["year"] for w, _ in rows], "cost": [round(w["cost"]) for w, _ in rows], "receipts": [round(w["receipts"]) for w, _ in rows],
                "borrowed": [round(w["borrowed"]) for w, _ in rows], "new service": [round(w["new_service"]) for w, _ in rows], "pledged": [round(w["pledged"]) for w, _ in rows],
                "coverage with receipts": [cov(w["coverage"]) for w, _ in rows], "coverage without": [cov(n["coverage"]) for _, n in rows]},
                title=f"The project year by year, with the receipts of {', '.join(sorted(ins)) or 'no instrument'} and without them ({A['unit']})")]
            lines = [("with the receipts", w["year"], w["coverage"]) for w, _ in rows] + [("without them", n["year"], n["coverage"]) for _, n in rows]
            lines += [(f"the floor, {floor:g}", w["year"], floor) for w, _ in rows]
            out.append(api.table("project_coverage", {"year": [y for _, y, v in lines if v is not None], "line": [n for n, _, v in lines if v is not None],
                                                      "coverage": [round(v, 3) for _, _, v in lines if v is not None]},
                                 title="The coverage of debt service, with the receipts and without them, against the floor",
                                 show={"kind": "chart", "chart": "line", "x": "year", "y": "coverage", "how": "mean", "split": "line"}))
            LW, LN = r["largest_with"], r["largest_without"]
            scale = lambda L: "none" if L["fails_at_none"] else (f"{L['scale']:.2f}" + ("" if L["bounded"] else " or more"))  # noqa: E731
            out.append(api.table("project_affordable", {
                "plan": ["with the receipts", "without them"], "borrowed": [round(W["borrowed"]), round(N["borrowed"])],
                "least coverage": [cov(W["least_coverage"]), cov(N["least_coverage"])], "in": [W["least_year"], N["least_year"]],
                "largest scale affordable": [scale(LW), scale(LN)], "what binds it": [LW["binding"], LN["binding"]]},
                title=f"What the city can afford at a floor of {floor:g}"))
            holds = lambda R: R["least_coverage"] is None or R["least_coverage"] >= floor  # noqa: E731
            text = (f"The project costs {W['cost']:,.0f} ({A['unit']}) from {y0}. With the receipts of {', '.join(sorted(ins)) or 'no instrument'} ({W['received']:,.0f} within the years ahead), "
                    f"the city borrows {W['borrowed']:,.0f} and its least coverage is {W['least_coverage']:.2f} in {W['least_year']}"
                    + (", above" if holds(W) else ", below") + f" the floor of {floor:g}; without them it borrows {N['borrowed']:,.0f} and its least coverage is "
                    f"{N['least_coverage']:.2f} in {N['least_year']}" + (", above" if holds(N) else ", below") + " the floor. "
                    f"The largest project it could afford is {scale(LW)} times this one with the receipts and {scale(LN)} without them. ")
            if outside > 0:
                text += f"Receipts of {outside:,.0f} fall after {last} and are left out. "
            text += "Capital receipts pay for capital and never raise the operating surplus. Every city, account, rate and floor here is invented."
            out.append(api.note(text))
            return out

    return Project()
