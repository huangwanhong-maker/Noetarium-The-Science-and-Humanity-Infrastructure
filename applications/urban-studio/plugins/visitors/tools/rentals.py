"""Short-term rentals and housing pressure: the nights a city's listings are booked, read from their reviews under declared readings,
the entire homes they take from residents, and how concentrated their supply is; the estimate held against the platforms' own nights
rented out (Eurostat) (ADR 0163)."""

from science import visitors


def create(api):
    class Rentals(api.BoardAnalysis):
        id = "rentals"
        title = "Short-term rentals and housing pressure"
        description = ("A city's listings on a platform (Inside Airbnb's snapshot, CC BY 4.0) read by the review-based model: a listing's nights "
                       "booked a year are its reviews in the last twelve months over the share of stays that leave a review, times a stay's "
                       "length (at least its minimum nights), capped at a declared share of the year. A stay's length is taken from the "
                       "platforms' own statistics where Eurostat publishes them (nights rented out over stays). The review rate is read three "
                       "ways. An entire home counts as withdrawn from residents when booked more than a declared number of nights, and in a "
                       "fourth reading also when its host holds a declared number of listings. The estimate is set against Eurostat's nights "
                       "rented out through four platforms, of which this one is a part.")
        params = [api.param("city", "City of the listings", "prague", kind="text"),
                  api.param("date", "Date of the snapshot", "2026-09-20", kind="text"),
                  api.param("case", "Case of the visitor stock (for the platforms' own nights)", "european-cities", kind="text"),
                  api.param("year", "Year of the platforms' nights", 2024, minimum=2020, maximum=2030, step=1),
                  api.param("place", "Place in the case", "prague", kind="text"),
                  api.param("review_rate", "Share of stays that leave a review (declared)", 0.5, minimum=0.05, maximum=1, step=0.05),
                  api.param("cap", "The most of a year a listing is booked (declared)", 0.7, minimum=0.1, maximum=1, step=0.05),
                  api.param("withdrawn_nights", "An entire home booked more nights than this is withdrawn (declared)", 90, minimum=0, maximum=365, step=5),
                  api.param("withdrawn_host", "Or held by a host with at least this many listings (declared)", 3, minimum=2, maximum=50, step=1),
                  api.param("currency", "The listings' currency (the platform shows local prices)", "CZK", kind="text")]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            L = visitors.listings(str(params.get("city") or "prague"), str(params.get("date") or "2026-09-20"))
            stay, rented, source = 3.0, None, "a declared 3 nights"
            try:
                P = visitors.product(str(params.get("case") or "european-cities"), int(g("year", 2024)))
                place = next(p for p in P["places"] if p["id"] == str(params.get("place") or "prague"))
                if place.get("platform_rented") and not place.get("platform_region"):
                    rented = sum(v or 0 for o in ("DOM", "FOR") for v in place["platform_rented"][o])
                    stays = sum(v or 0 for o in ("DOM", "FOR") for v in place["platform_stays"][o])
                    if rented > 0 and stays > 0:
                        stay, source = rented / stays, f"Eurostat's {rented:,.0f} nights rented out over {stays:,.0f} stays in {P['year']}"
            except (LookupError, StopIteration):
                pass
            rr, cap, wn, wh = g("review_rate", 0.5), g("cap", 0.7), g("withdrawn_nights", 90), g("withdrawn_host", 3)
            cur = str(params.get("currency") or "CZK").strip()
            base = {"stay": stay, "cap": cap, "withdrawn_nights": wn}
            R = [{**base, "name": f"reviews at {rr:g}", "review_rate": rr, "source": f"a stay of {stay:.2f} nights from {source}"},
                 {**base, "name": "reviews at 0.3", "review_rate": 0.3, "source": "fewer stays reviewed"},
                 {**base, "name": "reviews at 0.7", "review_rate": 0.7, "source": "more stays reviewed"},
                 {**base, "name": f"reviews at {rr:g}, hosts of {wh:g}", "review_rate": rr, "withdrawn_host": wh, "source": "multi-listing hosts' homes withdrawn too"}]
            a = visitors.ask("rentals.assess", listings=L["listings"], readings=R)
            A = a["readings"]
            out = [api.table("rentals_readings", {
                "reading": [x["reading"] for x in A], "nights_booked_m": [round(x["nights"] / 1e6, 3) for x in A], "homes_withdrawn": [round(x["withdrawn"]) for x in A],
                "share_of_platforms_nights": [None if not rented else round(x["nights"] / rented, 3) for x in A], "source": [x["source"] for x in A]},
                title="Nights booked and homes withdrawn under each reading", show={"kind": "chart", "chart": "bar", "x": "reading", "y": "homes_withdrawn", "how": "sum"})]
            m = A[0]
            areas = sorted(m["areas"], key=lambda x: -x["withdrawn"])[:14]
            out.append(api.table("rentals_areas", {
                "area": [x["area"] for x in areas], "listings": [round(x["listings"]) for x in areas], "entire_homes": [round(x["entire"]) for x in areas],
                "homes_withdrawn": [round(x["withdrawn"]) for x in areas], "nights_booked": [round(x["nights"]) for x in areas],
                "share_of_withdrawn": [round(x["withdrawn"] / m["withdrawn"], 4) if m["withdrawn"] else 0 for x in areas]},
                title=f"Homes withdrawn by area, under '{m['reading']}'", show={"kind": "chart", "chart": "bar", "x": "area", "y": "homes_withdrawn", "how": "sum"}))
            out.append(api.table("rentals_hosts", {
                "measure": ["listings", "entire homes", "hosts' HHI", "the largest host's share", "the ten largest hosts' share", "listings of multi-listing hosts",
                            f"revenue over priced listings (millions of {cur})"],
                "value": [round(m["listings"]), round(m["entire"]), round(m["hhi"], 5), round(m["largest_host"], 4), round(m["top10"], 4), round(m["multi_host_share"], 4),
                          round(m["revenue"] / 1e6, 1)]}, title="How concentrated the supply is"))
            top = areas[0] if areas else None
            words = [f"{m['listings']:,.0f} listings in {L['city'].title()} on {L['date']}, {m['entire']:,.0f} of them entire homes. Read with {rr:g} of stays reviewed and a stay of "
                     f"{stay:.2f} nights ({source}), they are booked {m['nights'] / 1e6:.2f} million nights a year, and {m['withdrawn']:,.0f} entire homes are booked more than "
                     f"{wn:g} nights, withdrawn from residents in this reading; between {min(x['withdrawn'] for x in A[:3]):,.0f} and {max(x['withdrawn'] for x in A[:3]):,.0f} "
                     f"as the review rate goes from 0.7 to 0.3, and {A[3]['withdrawn']:,.0f} when the homes of hosts with {wh:g} or more listings count too."]
            if rented:
                words.append(f"The platforms' own nights rented out were {rented / 1e6:.2f} million in {P['year']}; this platform's estimate is {m['nights'] / rented:.0%} of them, "
                             f"which no reading should exceed by much, the others' listings being outside it.")
            if top:
                words.append(f"{top['area']} holds {top['withdrawn'] / m['withdrawn']:.0%} of the withdrawn homes.")
            words.append(f"At the listed prices the nights booked are worth {m['revenue'] / 1e9:.2f} billion {cur} a year. The ten largest hosts hold {m['top10']:.1%} of listings; "
                         f"hosts with more than one listing hold {m['multi_host_share']:.0%}. "
                         f"Reviews are what guests left, not the nights booked, and every rate and threshold is a reading. {L['credit']}.")
            out.append(api.note(" ".join(words)))
            return out

    return Rentals()
