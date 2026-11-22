"""Who lives within reach of public transport: SDG 11.2.1 read under every declared reading of its open terms, and every stop's
service at the peak, on a place's walking ways and people and a feed's stops on a declared weekday."""

from science import risk, transit


def create(api):
    class Access(api.BoardAnalysis):
        id = "access"
        title = "Who lives within reach of public transport (SDG 11.2.1)"
        description = ("The share of people within 500 m along the streets of a stop of a low-capacity system (bus) or 1 km of a stop of a "
                       "high-capacity one (rail, metro, ferry), as UN-Habitat's metadata defines SDG 11.2.1. Its open terms are read every way "
                       "declared: whether a tram is of low or high capacity (the metadata names neither), and whether a stop counts only where "
                       "its mean wait at the morning peak is at most 20 minutes (the metadata's secondary indicator). Each reading gives its own "
                       "share; the indicator's value is one of them only once a reading is chosen. People are GHS-POP's, shared among buildings; "
                       "stops and departures the feed's on its declared weekday.")
        params = [api.param("place", "Feed", "melbourne", kind="text", help="a place whose feed the data library holds: melbourne"),
                  api.param("lat", "Latitude of the centre", -37.8136, step=0.0001),
                  api.param("lon", "Longitude of the centre", 144.9631, step=0.0001),
                  api.param("km", "Radius (km)", 2.5, minimum=0.5, maximum=6, step=0.5),
                  api.param("peak_from", "Peak from (hour)", 7, minimum=0, maximum=23, step=0.5),
                  api.param("peak_to", "Peak to (hour)", 9, minimum=1, maximum=24, step=0.5),
                  api.param("frequent_minutes", "Frequent: a mean wait at the peak of at most (min)", 20, minimum=1, maximum=60, step=1)]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            T = transit.service(str(params.get("place") or "melbourne").strip())
            P = risk.place(g("lat", -37.8136), g("lon", 144.9631), km=g("km", 2.5))
            stops = transit.stops_near(T, P)
            if not stops:
                raise ValueError("no stop of the feed lies within the place")
            f = g("frequent_minutes", 20)
            readings = [{"trams_high": th, "frequent_minutes": fm} for th in (False, True) for fm in (0, f)]
            readings.append({"trams_high": False, "frequent_minutes": 0, "along_network": False})
            a = transit.ask("transit.access", ways=transit.walking(P), places=P["people"], stops=stops,
                            peak=[g("peak_from", 7) * 3600, g("peak_to", 9) * 3600], readings=readings)
            R = a["readings"]
            short = lambda r: r.replace("trams ", "").replace(", every stop", " · all").replace(", frequent stops", " · frequent").replace(", straight line", " · line")  # noqa: E731
            out = [api.table("transit_readings", {
                "trams, stops": [short(x["reading"]) for x in R], "reading": [x["reading"] for x in R], "share_within_reach": [round(x["share"], 4) for x in R],
                "people_within": [round(x["within"]) for x in R], "people": [round(x["people"]) for x in R],
                "low_capacity_stops": [int(x["low_stops"]) for x in R], "high_capacity_stops": [int(x["high_stops"]) for x in R]},
                title="SDG 11.2.1 under each reading of its open terms",
                show={"kind": "chart", "chart": "bar", "x": "trams, stops", "y": "share_within_reach", "how": "sum"})]
            F = P["F"]
            S = a["stops"]
            geo = [F.geo(s["x"], s["y"]) for s in S]
            wait = lambda s: None if s["mean_wait"] is None else round(s["mean_wait"] / 60, 1)  # noqa: E731
            out.append(api.table("transit_stops", {
                "stop": [s["name"] for s in S], "lat": [round(p[0], 6) for p in geo], "lon": [round(p[1], 6) for p in geo],
                "modes": [", ".join(sorted(s["modes"])) for s in S], "departures_an_hour_at_peak": [round(s["per_hour"], 1) for s in S],
                "mean_wait_at_peak_min": [wait(s) for s in S], "departures_in_the_day": [int(s["day_departures"]) for s in S]},
                title=f"Every stop's service on {T['date']} at the peak", roles={"lat": "lat", "lon": "lon", "amount": "departures_an_hour_at_peak"},
                show={"kind": "layer", "layer": "points", "colour": "departures_an_hour_at_peak", "size": "departures_in_the_day", "map": "orchid"}))
            pl = a["places"]
            pgeo = [F.geo(p[0], p[1]) for p in pl]
            out.append(api.table("transit_people", {
                "lat": [round(p[0], 6) for p in pgeo], "lon": [round(p[1], 6) for p in pgeo], "people": [p[2] for p in pl],
                "to_a_bus_or_tram_m": [None if p[3] is None else round(p[3]) for p in pl], "to_rail_m": [None if p[4] is None else round(p[4]) for p in pl],
                "within_reach": ["yes" if p[5] else "no" for p in pl]},
                title=f"Where people live and how far they walk to a stop (under the first reading, {R[0]['reading']})",
                roles={"lat": "lat", "lon": "lon", "amount": "people"}, show={"kind": "layer", "layer": "points", "colour": "within_reach", "size": "people", "map": "orchid"}))
            lo, hi = min(R[:4], key=lambda x: x["share"]), max(R[:4], key=lambda x: x["share"])
            line = R[4]
            out.append(api.note(
                f"Of {round(R[0]['people']):,} people within {g('km', 2.5):g} km, between {lo['share']:.1%} ({lo['reading']}) and {hi['share']:.1%} ({hi['reading']}) "
                f"live within reach of public transport. In a straight line instead of along the streets, every stop counting and trams of low capacity, "
                f"{line['share']:.1%}. The readings that matter here are said in the table; none is the indicator's value until one is chosen. "
                f"{T['credit']}; people from GHS-POP, ways from Overture Maps."))
            return out

    return Access()
