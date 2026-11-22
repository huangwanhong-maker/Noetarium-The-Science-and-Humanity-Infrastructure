"""Open space and who can reach it: SDG 11.7.1 read under every declared reading of its open terms, on a place's walking ways and people
and the open land Overture's base theme maps there (ADR 0154)."""

from science import risk, transit

# The readings of SDG 11.7.1's open terms, a declaration: which kinds of land count, how large a space must be, how far, and how.
PUBLIC = ["park", "plaza"]
OFTEN = PUBLIC + ["pedestrian area", "sports"]
EVERY = OFTEN + ["green", "garden", "cemetery"]


def create(api):
    class OpenSpace(api.BoardAnalysis):
        id = "openspace"
        title = "Open space and who can reach it (SDG 11.7.1)"
        description = ("The share of a place's area that is open space for public use, and the share of its people living within 400 m of such "
                       "space along the streets, as UN-Habitat's metadata defines SDG 11.7.1. Its open terms are read every way declared: which "
                       "land counts (parks and plazas; pedestrian areas and sports grounds as well; every green and open land, gardens and "
                       "cemeteries among them), and WHO Europe's rule of thumb (a park of at least 0.5–1 ha within 300 m in a straight line). Each "
                       "reading gives its own shares; the indicator's value is one of them only once a reading is chosen. The open land is "
                       "Overture's land use and land cover, read as kinds by a declared table; people are GHS-POP's, shared among buildings. The "
                       "analysis area is a square about the centre; ways and land are gathered half a kilometre beyond it, so that a park across "
                       "its edge serves the people inside.")
        params = [api.param("lat", "Latitude of the centre", -37.8136, step=0.0001),
                  api.param("lon", "Longitude of the centre", 144.9631, step=0.0001),
                  api.param("km", "Half the side of the square (km)", 2.5, minimum=0.5, maximum=5, step=0.5),
                  api.param("metres", "Within (m, along the streets)", 400, minimum=100, maximum=2000, step=50),
                  api.param("who_least_ha", "WHO Europe: a park of at least (ha)", 0.5, minimum=0, maximum=10, step=0.1),
                  api.param("who_metres", "WHO Europe: within (m, in a straight line)", 300, minimum=100, maximum=2000, step=50)]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            lat, lon, km, m = g("lat", -37.8136), g("lon", 144.9631), g("km", 2.5), g("metres", 400)
            P = risk.place(lat, lon, km=min(km + 0.5, 6.0))
            land = transit.open_land(P, min(km + 0.5, 6.0))
            half = km * 1000
            box = [-half, -half, half, half]
            inside = [p for p in P["people"] if abs(p[0]) <= half and abs(p[1]) <= half]
            if not inside:
                raise ValueError("no one lives within the square")
            readings = [
                {"name": "parks and plazas", "kinds": PUBLIC, "metres": m, "source": "SDG 11.7.1, open space for public use read narrowly"},
                {"name": "+ pedestrian areas, sports grounds", "kinds": OFTEN, "metres": m, "source": "SDG 11.7.1 with the land often open to the public"},
                {"name": "+ green land, gardens, cemeteries", "kinds": EVERY, "metres": m, "source": "SDG 11.7.1 with every green and open land"},
                {"name": f"WHO Europe: parks of {g('who_least_ha', 0.5):g} ha", "kinds": PUBLIC[:1], "least_area": g("who_least_ha", 0.5) * 1e4,
                 "metres": g("who_metres", 300), "along_network": False,
                 "source": "WHO Regional Office for Europe: public green space of at least 0.5–1 ha within 300 m in a straight line"},
            ]
            readings += [{"name": f"{k} alone", "kinds": [k], "metres": m, "source": "each kind apart"} for k in EVERY]
            a = transit.ask("openspace.access", ways=transit.walking(P), places=inside, spaces=land["spaces"], box=box, readings=readings)
            R = a["readings"]
            main, kinds = R[:4], R[4:]
            short = ["parks, plazas", "+ squares, sports", "+ green, gardens", f"WHO: parks ≥ {g('who_least_ha', 0.5):g} ha"]
            out = [api.table("openspace_readings", {
                "land counted": short, "reading": [x["reading"] for x in main], "share_within_reach": [round(x["share"], 4) for x in main],
                "area_share": [round(x["area_share"], 4) for x in main], "area_ha": [round(x["area"] / 1e4, 1) for x in main],
                "spaces": [int(x["spaces"]) for x in main], "people_within": [round(x["within"]) for x in main], "people": [round(x["people"]) for x in main],
                "within_m": [x["metres"] for x in main], "along_the_streets": ["yes" if x["along_network"] else "no" for x in main],
                "least_area_ha": [round(x["least_area"] / 1e4, 2) for x in main], "source": [x["source"] for x in main]},
                title="SDG 11.7.1 under each reading of its open terms: the people within reach",
                show={"kind": "chart", "chart": "bar", "x": "land counted", "y": "share_within_reach", "how": "sum"})]
            out.append(api.table("openspace_kinds", {
                "kind": [x["reading"].replace(" alone", "") for x in kinds], "area_share": [round(x["area_share"], 4) for x in kinds],
                "area_ha": [round(x["area"] / 1e4, 1) for x in kinds], "polygons": [int(x["spaces"]) for x in kinds],
                "share_within_reach": [round(x["share"], 4) for x in kinds]},
                title=f"Each kind of open land alone: its share of the square and the people within {m:g} m of it",
                show={"kind": "chart", "chart": "bar", "x": "kind", "y": "area_share", "how": "sum"}))
            F = P["F"]
            pl = a["places"]
            pgeo = [F.geo(p[0], p[1]) for p in pl]
            out.append(api.table("openspace_people", {
                "lat": [round(p[0], 6) for p in pgeo], "lon": [round(p[1], 6) for p in pgeo], "people": [p[2] for p in pl],
                "to_a_park_or_plaza_m": [None if p[3] is None else round(p[3]) for p in pl], "within_reach": ["yes" if p[4] else "no" for p in pl]},
                title=f"Where people live and how far they walk to a park or a plaza (under the first reading, {m:g} m)",
                roles={"lat": "lat", "lon": "lon", "amount": "people"}, show={"kind": "layer", "layer": "points", "colour": "within_reach", "size": "people", "map": "orchid"}))
            ids, kind, rings = [], [], {}
            for i, s in enumerate(land["spaces"]):
                xy = s["ring"]
                pts = [F.geo(xy[k], xy[k + 1]) for k in range(0, len(xy), 2)]
                if not any(abs(xy[k]) <= half and abs(xy[k + 1]) <= half for k in range(0, len(xy), 2)):
                    continue
                ids.append(f"s{i}")
                kind.append(s["kind"])
                rings[f"s{i}"] = [[round(p[1], 6), round(p[0], 6)] for p in pts]
            out.append(api.table("openspace_land", {"space": ids, "kind": kind}, title="The open land, by the kind it was read as",
                                 roles={"cell": "space"}, places={"kind": "cells", "column": "space", "rings": rings},
                                 show={"kind": "layer", "layer": "areas", "colour": "kind"}))
            lo, hi = min(main[:3], key=lambda x: x["share"]), max(main[:3], key=lambda x: x["share"])
            who = main[3]
            out.append(api.note(
                f"Of {round(main[0]['people']):,} people in the square of {2 * km:g} km about the centre, between {lo['share']:.1%} ({lo['reading']}) and "
                f"{hi['share']:.1%} ({hi['reading']}) live within {m:g} m of open space along the streets; it covers between {main[0]['area_share']:.1%} and "
                f"{main[2]['area_share']:.1%} of the square. Under WHO Europe's rule of thumb, {who['share']:.1%}. Which land is open to the public is read "
                f"from what Overture's base theme calls it, a declaration; UN-Habitat calls such land potential open public space until it is validated on the "
                f"ground, since a sports ground or a garden may be fenced and a cemetery closed at night. None of these is "
                f"the indicator's value until a reading is chosen. {land['credit']}; people from GHS-POP."))
            return out

    return OpenSpace()
