"""Designing a levy: a flat levy and a rate compared by class of accommodation (hotels, holiday lodging, camping, platform rentals) on a
place's nights, a cap on nights as the levy that holds it and the scarcity rent it creates, and dual prices for residents and visitors
at an attraction (ADR 0162). Prices, elasticities and the attraction are declarations."""

from science import visitors

CLASSES = (("hotels", "hotels"), ("holiday", "holiday lodging"), ("camping", "camping"), ("platform", "platform rentals"))


def create(api):
    class Design(api.BoardAnalysis):
        id = "design"
        title = "Designing a levy (bands, a cap, dual prices)"
        description = ("A levy on a place's paid nights designed three ways. By band: a flat levy a night and a rate on the price compared class "
                       "by class (hotels, holiday lodging, camping, platform rentals, their nights Eurostat's), each class's levy over its price "
                       "showing how a flat levy weighs on the cheap. A cap on nights: the levy that would hold the nights at the cap, and the "
                       "scarcity rent between the visitor's and the operator's prices that a cap creates, the city's if the right to a night is "
                       "sold and the operators' if it is given. Dual prices at an attraction: visits, revenue and surplus by group, and the price "
                       "that maximises each group's margin by Lerner's rule where its demand is elastic. Every price, elasticity and the "
                       "attraction are declarations."
                       " Platform nights may overlap registered ones by an amount Eurostat cannot measure; they are counted less a declared share of the largest overlap possible, the registered short-stay nights, all of it by default, so that nothing is counted twice (ADR 0164).")
        params = [api.param("case", "Case of the data library", "european-cities", kind="text"),
                  api.param("year", "Year", 2024, minimum=2020, maximum=2030, step=1),
                  api.param("place", "Place", "prague", kind="text"),
                  api.param("prices", "Price a guest-night by class (EUR, declared)", "hotels=90; holiday=50; camping=25; platform=60", kind="text"),
                  api.param("levy", "Flat levy a night (EUR)", 2, minimum=0, maximum=50, step=0.5),
                  api.param("rate", "Or a rate on the price", 0.03, minimum=0, maximum=0.5, step=0.005),
                  api.param("demand_elasticity", "Elasticity of demand (declared)", -0.6, minimum=-5, maximum=-0.01, step=0.05),
                  api.param("supply_elasticity", "Elasticity of supply (declared)", 2, minimum=0.1, maximum=50, step=0.5),
                  api.param("cap_share", "A cap on nights, as a share of today's", 0.9, minimum=0.1, maximum=1, step=0.01),
                  api.param("attraction", "An attraction (group = visits@reference price@elasticity@charged; declared)",
                            "residents = 300000@10@-0.6@10; visitors = 2000000@10@-0.3@20", kind="text"),
                  api.param("cost", "The attraction's cost a visit (EUR, declared)", 4, minimum=0, maximum=200, step=0.5),
                  api.param("overlap", "Share of the largest overlap of platform and registered nights removed (1: nothing counted twice)", 1, minimum=0, maximum=1, step=0.05)]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            P = visitors.product(str(params.get("case") or "european-cities"), int(g("year", 2024)))
            pid = str(params.get("place") or "prague").strip().lower()
            place = next((p for p in P["places"] if p["id"] == pid), None)
            if place is None:
                raise LookupError(f"no place {pid} in the case")
            if not place.get("by_class") or any(v is None for v in place["by_class"].values()):
                raise LookupError(f"Eurostat publishes no nights by class for {place['name']}'s code")
            prices = {}
            for part in str(params.get("prices") or "hotels=90; holiday=50; camping=25; platform=60").split(";"):
                if "=" in part:
                    k, v = part.split("=", 1)
                    prices[k.strip()] = float(v)
            nights = dict(place["by_class"])
            if not place.get("platform_region"):
                # Platform nights less the declared share of those that may be registered holiday lodging already (ADR 0164).
                pn = visitors.paid_nights(place)
                nights["platform"] = pn["platform"] - g("overlap", 1) * pn["overlap_max"]
            ed, es = g("demand_elasticity", -0.6), g("supply_elasticity", 2)
            bands_flat, bands_rate, names = [], [], []
            for key, word in CLASSES:
                if not nights.get(key) or key not in prices:
                    continue
                m = {"nights": nights[key], "price": prices[key], "demand_elasticity": ed, "supply_elasticity": es}
                bands_flat.append({"name": word, "market": m, "levy": g("levy", 2)})
                bands_rate.append({"name": word, "market": m, "levy": g("rate", 0.03), "rate": True})
                names.append(word)
            total = sum(b["market"]["nights"] for b in bands_flat)
            mean_price = sum(b["market"]["nights"] * b["market"]["price"] for b in bands_flat) / total
            att = []
            for part in str(params.get("attraction") or "residents = 300000@10@-0.6@10; visitors = 2000000@10@-0.3@20").split(";"):
                if "=" in part:
                    k, v = part.split("=", 1)
                    vis, pr, el, ch = (float(x) for x in v.split("@"))
                    att.append({"name": k.strip(), "visits": vis, "price": pr, "elasticity": el, "charged": ch})
            flat = visitors.ask("tourism.design", bands=bands_flat, cap={"market": {"nights": total, "price": mean_price, "demand_elasticity": ed, "supply_elasticity": es},
                                                                           "cap": g("cap_share", 0.9) * total}, attraction={"groups": att, "cost": g("cost", 4)} if att else None)
            rate = visitors.ask("tourism.design", bands=bands_rate)
            F, R = flat["bands"], rate["bands"]
            m = lambda v: round(v / 1e6, 2)  # noqa: E731
            rows = {"class": [], "levy": [], "share_of_price": [], "revenue_meur": [], "nights_lost": []}
            for word, label in ((F, f"flat {g('levy', 2):g} €"), (R, f"rate {g('rate', 0.03):.1%}")):
                for c in word["classes"]:
                    rows["class"].append(c["name"]), rows["levy"].append(label), rows["share_of_price"].append(round(c["share_of_price"], 4))
                    rows["revenue_meur"].append(m(c["revenue"])), rows["nights_lost"].append(round(c["nights_lost"]))
            out = [api.table("design_bands", rows, title=f"A flat levy and a rate by class in {place['name']}: each class's levy over its price",
                             show={"kind": "chart", "chart": "bar", "x": "class", "y": "share_of_price", "split": "levy", "how": "sum"})]
            C = flat["cap"]
            out.append(api.table("design_cap", {
                "measure": ["cap on nights", "levy that holds it (EUR a night)", "visitor's price (EUR)", "operator's price (EUR)", "scarcity rent (M EUR)"],
                "value": [round(C["cap"]), round(C["equivalent"]["levy"], 2), round(C["equivalent"]["consumer_price"], 2), round(C["equivalent"]["producer_price"], 2), m(C["rent"])]},
                title=f"A cap of {g('cap_share', 0.9):.0%} of today's paid nights"))
            if att:
                A = flat["attraction"]
                out.append(api.table("design_attraction", {
                    "group": [x["name"] for x in A], "charged": [x["charged"] for x in A], "visits": [round(x["visits"]) for x in A],
                    "revenue_meur": [m(x["revenue"]) for x in A], "margin_meur": [m(x["margin"]) for x in A], "surplus_lost_meur": [m(x["surplus_lost"]) for x in A],
                    "best_price": [None if x["best_price"] is None else round(x["best_price"], 2) for x in A]},
                    title="Dual prices at an attraction (declared)", show={"kind": "chart", "chart": "bar", "x": "group", "y": "revenue_meur", "how": "sum"}))
            cheap = max(F["classes"], key=lambda c: c["share_of_price"])
            dear = min(F["classes"], key=lambda c: c["share_of_price"])
            words = [f"On {total / 1e6:.1f} million paid nights in {place['name']}, a flat levy of {g('levy', 2):g} euro raises {m(F['revenue']):,.1f} million and a rate of "
                     f"{g('rate', 0.03):.1%} raises {m(R['revenue']):,.1f} million. The flat levy is {cheap['share_of_price']:.1%} of a night in {cheap['name']} and "
                     f"{dear['share_of_price']:.1%} in {dear['name']}; the rate is the same share everywhere.",
                     f"A cap at {g('cap_share', 0.9):.0%} of today's nights would be held by a levy of {C['equivalent']['levy']:.2f} euro a night, and creates a scarcity rent of "
                     f"{m(C['rent']):,.1f} million a year, the city's if the right to a night is sold, the operators' if it is given."]
            if att:
                A = flat["attraction"]
                words.append("At the declared attraction, " + "; ".join(f"{x['name']} charged {x['charged']:g} come {x['visits']:,.0f} times" for x in A) +
                             (". Demand that is inelastic has no price that maximises its margin, so a dual price there raises revenue at the cost of surplus." if any(x["best_price"] is None for x in A) else "."))
            words.append("Prices, elasticities and the attraction are declarations; no city's levy is claimed. " + P["credit"] + ".")
            out.append(api.note(" ".join(words)))
            return out

    return Design()
