"""A levy on visitors' nights: what it raises, who bears it, the nights it loses and the value added lost with them, under declared
elasticities of demand and supply (ADR 0157)."""

from science import visitors


def create(api):
    class Levy(api.BoardAnalysis):
        id = "levy"
        title = "A levy on visitors' nights (revenue, who bears it, what is lost)"
        description = ("A levy a night on a place's paid nights (registered establishments and platform rentals), in a market of declared price "
                       "and declared elasticities of demand and supply: the visitor's and the operator's prices at the equilibrium, the nights "
                       "lost, the revenue, the visitor's and the operator's burden (which sum to the revenue), the excess burden, and the value "
                       "added lost with the nights not displaced to stays the levy does not reach, at the value added a night that the "
                       "accounts give. The levy that raises most within a cap is found. Every elasticity, price and displaced share is a "
                       "declaration; the levies of any real city are not claimed.")
        params = [api.param("case", "Case of the data library", "european-cities", kind="text"),
                  api.param("year", "Year of the nights", 2024, minimum=2020, maximum=2030, step=1),
                  api.param("place", "Place", "prague", kind="text"),
                  api.param("levy", "Levy a night (EUR)", 2, minimum=0, maximum=100, step=0.5),
                  api.param("price", "Price of a night a guest (EUR, declared)", 60, minimum=1, maximum=2000, step=1),
                  api.param("demand_elasticity", "Elasticity of demand (declared, < 0)", -0.6, minimum=-5, maximum=-0.01, step=0.05),
                  api.param("supply_elasticity", "Elasticity of supply (declared, > 0; 0 for passed on in full)", 2, minimum=0, maximum=50, step=0.5),
                  api.param("displaced", "Share of the nights lost that move to stays the levy does not reach (declared)", 0.3, minimum=0, maximum=1, step=0.05),
                  api.param("value_added_per_night", "Value added a night (EUR; from the accounts' step)", 95, minimum=0, maximum=2000, step=1),
                  api.param("cap", "Highest levy to search (EUR)", 40, minimum=1, maximum=500, step=1)]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            P = visitors.product(str(params.get("case") or "european-cities"), int(g("year", 2024)))
            pid = str(params.get("place") or "prague").strip().lower()
            place = next((p for p in P["places"] if p["id"] == pid), None)
            if place is None:
                raise LookupError(f"no place {pid} in the case")
            a = visitors.ask("visitors.stock", places=[visitors.place(place, P["year"])], readings=visitors.readings()[1:2])
            nights = a["places"][0]["readings"][0]["nights"]
            es = g("supply_elasticity", 2)
            market = {"nights": nights, "price": g("price", 60), "demand_elasticity": g("demand_elasticity", -0.6), "displaced": g("displaced", 0.3)}
            if es > 0:
                market["supply_elasticity"] = es
            t0 = g("levy", 2)
            grid = sorted({round(0.5 * k, 2) for k in range(0, int(2 * max(10, 2 * t0)) + 1)} | {t0})
            va = g("value_added_per_night", 95)
            r = visitors.ask("tourism.levy", market=market, levies=grid, cap=g("cap", 40), value_added_per_night=va)
            F = r["flat"]
            m = lambda v: round(v / 1e6, 2)  # noqa: E731
            curve = {"levy_eur": [], "measure": [], "million_eur": []}
            for L in F:
                for k, word in (("revenue", "revenue"), ("visitors_burden", "borne by visitors"), ("operators_burden", "borne by operators"), ("value_added_lost", "value added lost")):
                    curve["levy_eur"].append(L["levy"]), curve["measure"].append(word), curve["million_eur"].append(m(L[k] or 0))
            out = [api.table("levy_curve", curve, title=f"A levy a night on {place['name']}'s {nights / 1e6:.1f} million paid nights: revenue, burdens and value added lost",
                             show={"kind": "chart", "chart": "line", "x": "levy_eur", "y": "million_eur", "split": "measure", "how": "sum"})]
            L = next(x for x in F if abs(x["levy"] - t0) < 1e-9)
            out.append(api.table("levy_at", {
                "measure": ["levy a night (EUR)", "visitor's price (EUR)", "operator's price (EUR)", "visitor's share of the levy", "nights lost", "of them displaced",
                            "revenue (M EUR)", "borne by visitors (M EUR)", "borne by operators (M EUR)", "excess burden (M EUR)", "value added lost (M EUR)"],
                "value": [t0, round(L["consumer_price"], 2), round(L["producer_price"], 2), round(L["pass_through"], 4), round(L["nights_lost"]), round(L["displaced_nights"]),
                          m(L["revenue"]), m(L["visitors_burden"]), m(L["operators_burden"]), m(L["excess_burden"]), m(L["value_added_lost"] or 0)]},
                title=f"At the declared levy of {t0:g} euro a night"))
            pk = r["peak"]
            peak = (f"Within the cap of {g('cap', 40):g} euro, the revenue {'still rises at the cap' if pk['at_cap'] else 'is highest'} at {pk['levy']:.2f} euro a night "
                    f"({m(pk['revenue']):,.1f} million).") if pk else ""
            out.append(api.note(
                f"A levy of {t0:g} euro a night on {nights / 1e6:.1f} million paid nights in {place['name']} raises {m(L['revenue']):,.1f} million euro a year. Visitors bear "
                f"{L['pass_through']:.0%} of it and operators the rest, at the declared elasticities ({market['demand_elasticity']:g} of demand, "
                f"{'infinite' if es <= 0 else f'{es:g}'} of supply). It loses {L['nights_lost']:,.0f} nights, {L['displaced_nights']:,.0f} of them moving to stays it does not reach, "
                f"and with the rest {m(L['value_added_lost'] or 0):,.1f} million of value added at {va:g} euro a night. The excess burden is {m(L['excess_burden']):,.2f} million. {peak} "
                f"Every price, elasticity and share here is declared; the levies of real cities are not claimed."))
            return out

    return Levy()
