"""The years ahead under a law: a law learned on every year recorded, run on from the last, with its currents of people; a new source
of people may be placed."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _population as p  # noqa: E402


def create(api):
    class Ahead(api.BoardAnalysis):
        id = "ahead"
        title = "The years ahead under a law"
        description = ("A law of the field learned on every year recorded and run on from the last, every five years: the people of each cell, the total "
                       "against the JRC's own projections, and the currents the law carries people by. A new source of people (a town planned) may be "
                       "placed at a latitude and longitude, and the law's spread scaled to see what it does.")
        params = p.case_params(api) + [
            api.param("model", "Law", "drift", kind="choice", options=p.RUNS),
            api.param("to", "Run on to", 2050, minimum=2025, maximum=2100, step=5),
            api.param("source_lat", "A new source: latitude", "", kind="text"),
            api.param("source_lon", "A new source: longitude", "", kind="text"),
            api.param("source_people", "A new source: people a year", 2000, minimum=0, maximum=200000, step=100),
            api.param("spread", "The spread scaled by", 1, minimum=0, maximum=10, step=0.5)]

        def run(self, board, params, context):
            f, case = p.field_of(params)
            cs = p.cells(f, case)
            request = {"model": params.get("model") or "drift", "to": float(params.get("to") or 2050)}
            sl, so = str(params.get("source_lat") or "").strip(), str(params.get("source_lon") or "").strip()
            placed = None
            if sl and so:
                lat, lon = float(sl), float(so)
                near = min((c for c in cs if c.get("lat") is not None), key=lambda c: (c["lat"] - lat) ** 2 + ((c["lon"] - lon) * math.cos(math.radians(lat))) ** 2)
                request["sources"] = [{"row": near["row"], "col": near["col"], "people_per_year": float(params.get("source_people") or 0)}]
                placed = near
            if float(params.get("spread") or 1) != 1:
                request["scale"] = {"diffusion": float(params["spread"]), "crowding": float(params["spread"])}
            s = p.pd.ask("simulate", field=f, **request)
            cols = {"cell": [f"r{c['row']:03d}c{c['col']:03d}" for c in cs], "lat": [c.get("lat") for c in cs], "lon": [c.get("lon") for c in cs]}
            for y, frame in zip(s["years"], s["frames"]):
                cols[f"p{int(y)}"] = frame
            last = int(s["years"][-1])
            cols["east"], cols["north"] = s["currents"][-1]["east"], s["currents"][-1]["north"]
            out = [api.table("ahead", cols, title=f"{f['title'].split(':')[0]}: people in each km² to {last} under the law", roles={"lat": "lat", "lon": "lon", "name": "cell", "amount": f"p{last}"},
                             show={"kind": "layer", "layer": "points", "colour": f"p{last}", "map": "orchid"})]
            out.append(api.table("ahead_totals", {"year": s["years"], "people": s["totals"]}, title="Everyone in the square, under the law",
                                 show={"kind": "chart", "chart": "line", "x": "year", "y": "people", "how": "sum"}))
            words = [f"Under the law learned on {s['fitted']['on'][0]:.0f}–{s['fitted']['on'][1]:.0f} ({p.reading_words(s['reading'])}): "
                     f"{s['totals'][0]:,.0f} people in {s['years'][0]:.0f}, {s['totals'][-1]:,.0f} in {last}."]
            for a in s.get("against_projections", []):
                words.append(f"For {a['year']:.0f} the JRC projects {a['projected_total']:,.0f} and the law runs to {a['run_total']:,.0f}.")
            if placed:
                words.append(f"A source of {float(params.get('source_people') or 0):,.0f} people a year was placed in the cell at {placed['lat']:.4f}, {placed['lon']:.4f}.")
            if s.get("clipped"):
                words.append(f"The run set {s['clipped']:,.0f} people's worth of negative densities to zero: the law was asked beyond what it describes.")
            out.append(api.note(" ".join(words)))
            out.append(api.note("A run is what the law does if nothing it does not hold changes: no new road, no rezoning, no change in who comes. It is a "
                                "consequence of the law, not a forecast of the place."))
            return out
    return Ahead()
