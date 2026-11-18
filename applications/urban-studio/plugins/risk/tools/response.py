"""Who stays within reach of care in each flood, who is reached twice, and where another station would reach most over the floods."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _risk as r  # noqa: E402
from science import risk  # noqa: E402

KINDS = {"care": {"hospital", "clinic", "doctors", "medical", "healthcare"}, "fire": {"fire_station"}, "police": {"police"}}
CANDIDATES = {"school", "college", "university", "education", "community_centre", "townhall", "civic", "public"}


def create(api):
    class Response(api.BoardAnalysis):
        id = "response"
        title = "Who stays within reach of a station"
        description = ("A place is covered when a station lies within the threshold over the roads at their state in each flood (cars stop in 30 cm). "
                       "Beside who is covered: who is covered twice (backup coverage, Hogan and ReVelle 1986), and what each station alone covers. "
                       "Where to add stations: among buildings of public use (schools, civic buildings) the k that most raise the coverage expected "
                       "over the floods, chosen greedily with a bound on the best k (the maximal covering location problem, Church and ReVelle 1974).")
        params = [api.param("case", "Place", "freetown", kind="text", help=r.CASES),
                  api.param("stations", "Stations", "care", kind="text", help="care, fire or police: the buildings mapped as such"),
                  api.param("km", "How far about it (km)", 3.0, minimum=0.5, maximum=6, step=0.5),
                  api.param("threshold", "Within (minutes)", 10, minimum=2, maximum=60, step=1),
                  api.param("k", "Stations to add", 3, minimum=0, maximum=10, step=1),
                  api.param("lat", "Latitude (blank: the place's centre)", "", kind="text"),
                  api.param("lon", "Longitude (blank: the place's centre)", "", kind="text")]

        def run(self, board, params, context):
            H, lat, lon, km = r.where(params)
            P = risk.place(lat, lon, km)
            kind = str(params.get("stations") or "care").strip()
            if kind not in KINDS:
                raise ValueError(f"stations are one of {', '.join(KINDS)}")
            stations = [[round(b["x"]), round(b["y"]), round(b["floor"]), b["name"] or b["kind"]] for b in P["buildings"] if b["kind"] in KINDS[kind]]
            if not stations:
                raise ValueError(f"no building about the place is mapped as {kind}")
            cands = [[round(b["x"]), round(b["y"]), round(b["floor"]), b["name"] or b["kind"]] for b in P["buildings"] if b["kind"] in CANDIDATES][:80]
            k = int(params.get("k") or 0)
            req = dict(ways=risk.drive_ways(P), frame=P["frame"], people=P["people"], stations=stations, threshold=float(params.get("threshold") or 10) * 60,
                       hazard={"maps": H["maps"]}, curve="vehicles")
            if k > 0 and cands:
                req["candidates"], req["k"] = cands, k
            a = risk.ask("response", **req)
            S = a["scenarios"]
            out = [api.table("response_scenarios", {"scenario": [s["name"] for s in S], "weight": [s["weight"] for s in S], "people": [s["people"] for s in S],
                                                    "covered": [s["covered"] for s in S], "covered_twice": [s["backup"] for s in S], "share_covered": [s["share"] for s in S]},
                             title=f"Coverage by {kind} within {float(params.get('threshold') or 10):g} minutes, in each flood",
                             show={"kind": "chart", "chart": "bar", "x": "scenario", "y": "share_covered", "how": "sum"})]
            geo = [P["F"].geo(s[0], s[1]) for s in stations]
            out.append(api.table("response_stations", {"station": [s[3] for s in stations], "lat": [round(g[0], 6) for g in geo], "lon": [round(g[1], 6) for g in geo],
                                                       "reaches": S[0]["reach"], "alone_reaches": S[0]["only"]},
                                 title="Each station: whom it reaches in a dry year, and whom it alone reaches", roles={"lat": "lat", "lon": "lon", "name": "station", "amount": "alone_reaches"},
                                 show={"kind": "layer", "layer": "points", "colour": "alone_reaches", "size": "reaches", "map": "orchid"}))
            text = f"{len(stations)} buildings mapped as {kind}; in a dry year {100 * S[0]['share']:.0f}% of the people are within reach, {r.n0(S[0]['backup'])} of them twice."
            worst = min(S, key=lambda s: s["share"])
            text += f" In {worst['name']} the share falls to {100 * worst['share']:.0f}%."
            if "siting" in a and a["siting"]["chosen"]:
                si = a["siting"]
                names = [cands[i][3] for i in si["chosen"]]
                text += f" Adding {', '.join(names)} would raise the expected coverage from {r.n0(si['existing'])} to {r.n0(si['value'][-1])} people (no {len(names)} could reach more than {r.n0(si['bound'])})."
            out.append(api.note(text + f" {H['credit']}"))
            return out

    return Response()
