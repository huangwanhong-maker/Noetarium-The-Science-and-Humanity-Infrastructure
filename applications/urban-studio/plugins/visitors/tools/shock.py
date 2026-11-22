"""When visitors stay away: a fall in a place's visitors for some months, and lodging capacity closed, run through the country's
interdependencies over the recovery by the dynamic inoperability input–output model (ADR 0158)."""

import importlib.util
from pathlib import Path

from science import visitors

_spec = importlib.util.spec_from_file_location("visitors_spending", Path(__file__).with_name("spending.py"))
_spending = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_spending)


def create(api):
    class Shock(api.BoardAnalysis):
        id = "shock"
        title = "When visitors stay away (losses over the recovery)"
        description = ("Visitors' spending falls by a declared share for declared months, and a declared share of lodging capacity may close; the "
                       "lost demand and capacity run through the country's interdependencies over the recovery by the dynamic inoperability "
                       "input–output model, each sector recovering at its declared pace. The losses of output and value added are given with "
                       "the interdependencies and without them (each sector alone), so that indirect losses are seen, and sectors are ranked by "
                       "how inoperable they become and by what they lose, which differ. The visitors' spending is the accounts' step's, at its "
                       "declared amounts and basket.")
        params = [api.param("case", "Case of the data library", "european-cities", kind="text"),
                  api.param("year", "Year of the nights", 2024, minimum=2020, maximum=2030, step=1),
                  api.param("place", "Place", "prague", kind="text"),
                  api.param("geo", "Country of the input–output table", "CZ", kind="text"),
                  api.param("table_year", "Year of the table", 2022, minimum=2010, maximum=2030, step=1),
                  api.param("foreign", "A foreign visitor's spending a night (EUR, declared)", 130, minimum=0, maximum=2000, step=5),
                  api.param("domestic", "A domestic visitor's spending a night (EUR, declared)", 80, minimum=0, maximum=2000, step=5),
                  api.param("basket", "Basket (product = share)", "I=0.55; G47=0.20; H49=0.08; R90-92=0.07; R93=0.05; N79=0.05", kind="text"),
                  api.param("basic_share", "Basic over purchasers' prices (declared)", 0.85, minimum=0.5, maximum=1, step=0.01),
                  api.param("fall", "Share of visitors' spending lost (declared)", 0.5, minimum=0, maximum=1, step=0.05),
                  api.param("days", "For how many days", 90, minimum=1, maximum=365, step=1),
                  api.param("lodging_closed", "Share of lodging and food capacity closed at first (declared)", 0.0, minimum=0, maximum=1, step=0.05),
                  api.param("recovery_days", "Days for a sector alone to recover to 1% (declared)", 60, minimum=1, maximum=1000, step=1),
                  api.param("horizon", "Days followed", 365, minimum=30, maximum=1500, step=5)]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            T = visitors.table(str(params.get("geo") or "CZ"), int(g("table_year", 2022)))
            # Domestic tourism demand by product, a year, in euro, from the accounts.
            P = visitors.product(str(params.get("case") or "european-cities"), int(g("year", 2024)))
            pid = str(params.get("place") or "prague").strip().lower()
            place = next(p for p in P["places"] if p["id"] == pid)
            nights = {o: round(v) for o, v in zip(("DOM", "FOR"), _nights(place, P["year"]))}
            B = visitors.basket(params.get("basket") or "")
            held = {c for c, row, f in zip(T["products"], T["domestic"], T["final_domestic"]) if sum(row) + f > 0}
            for k in list(B):
                if k not in held and _spending.FALLBACK.get(k) in held:
                    B[_spending.FALLBACK[k]] = B.get(_spending.FALLBACK[k], 0) + B.pop(k)
            acc = visitors.ask("tourism.accounts", table=T, purchases=visitors.purchases({"DOM": g("domestic", 80), "FOR": g("foreign", 130)}, B, g("basic_share", 0.85)),
                               nights=nights, units=1e-6)
            fall, days = g("fall", 0.5), int(g("days", 90))
            lost = {x["product"]: fall * x["demand_domestic"] for x in acc["products"] if x.get("demand_domestic", 0) > 0}
            first = {"I": g("lodging_closed", 0.0)} if g("lodging_closed", 0.0) > 0 else {}
            r = visitors.ask("recovery.path", table=T, units=1e-6, first=first, demand=[{"from": 0, "to": days, "lost": lost}],
                             recovery_periods={"default": g("recovery_days", 60)}, periods=int(g("horizon", 365)), periods_a_year=365, track=6)
            m = lambda v: round(v / 1e6, 2)  # noqa: E731
            labels = dict(zip(T["products"], T["labels"]))
            S = r["sectors"][:14]
            out = [api.table("shock_sectors", {
                "sector": [_spending.short(x["product"], labels.get(x["product"], x["product"])) for x in S], "code": [x["product"] for x in S],
                "output_lost_meur": [m(x["loss"]) for x in S], "alone_meur": [m(x["alone"]) for x in S], "value_added_lost_meur": [m(x["value_added_lost"]) for x in S],
                "mean_inoperability": [round(x["mean_inoperability"], 6) for x in S], "rank_by_loss": [int(x["rank_by_loss"]) for x in S],
                "rank_by_inoperability": [int(x["rank_by_inoperability"]) for x in S], "recovered_day": [int(x["recovered"]) for x in S]},
                title=f"Output lost over {int(g('horizon', 365))} days, by sector: with the interdependencies and alone",
                show={"kind": "chart", "chart": "bar", "x": "sector", "y": "output_lost_meur", "how": "sum"})]
            path = {"day": [], "sector": [], "inoperability": []}
            for p in r["paths"]:
                for t, q in enumerate(p["q"]):
                    if t % 5 == 0:
                        path["day"].append(t), path["sector"].append(_spending.short(p["product"], labels.get(p["product"], p["product"]))), path["inoperability"].append(round(q, 6))
            out.append(api.table("shock_paths", path, title="Inoperability day by day, the six sectors losing most",
                                 show={"kind": "chart", "chart": "line", "x": "day", "y": "inoperability", "split": "sector", "how": "sum"}))
            first_words = f" and {g('lodging_closed', 0):.0%} of lodging and food capacity closed at first" if first else ""
            out.append(api.note(
                f"Visitors' spending in {place['name']} falling by {fall:.0%} for {days} days{first_words}, each sector alone recovering to 1% in {g('recovery_days', 60):g} days "
                f"(declarations), costs {T['geo']} {m(r['total']):,.1f} million euro of output over {int(g('horizon', 365))} days and {m(r['value_added_lost']):,.1f} million of value added. "
                f"Each sector alone would lose {m(r['alone']):,.1f} million of output; the interdependencies add {m(r['indirect']):,.1f} million. "
                f"Over an unbounded horizon the loss is {m(r['unbounded']):,.1f} million, the Leontief total of the lost demand plus the capacity lost over each sector's pace; "
                f"the {int(g('horizon', 365))} days followed hold {r['total'] / r['unbounded']:.1%} of it. "
                f"The table is national and the model linear, without inventories, rationing or reconstruction (Hallegatte's ARIO holds them). {T['credit']}."))
            return out

    return Shock()


def _nights(place: dict, year: int):
    """A place's nights by origin, platform rentals carried from its region where they are published only there."""
    nights = {o: sum(v or 0 for v in place["registered"][o]) for o in ("DOM", "FOR")}
    if place.get("platform_region"):
        a = visitors.ask("visitors.stock", places=[visitors.place(place, year)], readings=visitors.readings()[1:2])
        for o in ("DOM", "FOR"):
            nights[o] += sum(v or 0 for v in a["places"][0]["allocated"]["nights"][o])
    else:
        for o in ("DOM", "FOR"):
            nights[o] += sum(v or 0 for v in place["platform"][o])
    return nights["DOM"], nights["FOR"]
