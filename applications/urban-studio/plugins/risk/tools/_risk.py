"""What the tools of lifelines and evacuation share: a place's hazard maps and refuges from the data library, its ways, buildings and
people (science.risk.place), and the answers of the plug-in's engine (the API's science/risk.hpp) laid out as tables placed on the
map. Nothing of the computation is done here."""

from __future__ import annotations

import math

from science import risk

CASES = "freetown (river floods at six return periods; its 1-in-100-year flood as the zone to leave), or a Japanese municipality's code made by data/products/japan_hazards.py (03202, Miyako: its largest assumed flood; its assumed tsunami as the zone)"

# The 2011 survey's shares (Cabinet Office, FDMA and JMA, N = 870): 57% left as soon as the shaking stopped, 31% after doing something
# first, 11% only as the tsunami neared; the times when each group left are a declaration (from 2 min, as the FDMA's manual assumes).
SURVEY = {"time": [0, 120, 600, 1500, 2400, 2700], "share": [0, 0, 0.57, 0.88, 0.99, 1],
          "source": "the shares of the 2011 survey by the Cabinet Office, FDMA and JMA (57% at once, 31% after doing something, 11% as the tsunami neared); the times when each group left are declared"}
AT_ONCE = {"time": [0, 1e-6], "share": [0, 1], "source": "everyone at once"}


def where(params: dict) -> tuple[dict, float, float, float]:
    """The case's hazard and the place asked (its centre unless a latitude and longitude are given), and how far about it (km)."""
    case = str(params.get("case") or "freetown").strip()
    H = risk.hazard_maps(case)
    lat = params.get("lat")
    lon = params.get("lon")
    lat, lon = (float(lat), float(lon)) if lat not in (None, "") and lon not in (None, "") else H["centre"]
    km = float(params.get("km") or 3.0)
    return H, lat, lon, km


def midpoints(P: dict, polylines: list) -> tuple[list, list, list]:
    """Each link's middle (by length) in latitude and longitude, and its length (m)."""
    lat, lon, length = [], [], []
    for xy in polylines:
        pts = [(xy[i], xy[i + 1]) for i in range(0, len(xy), 2)]
        seg = [math.hypot(b[0] - a[0], b[1] - a[1]) for a, b in zip(pts, pts[1:])]
        total, half, run = sum(seg), sum(seg) / 2, 0.0
        x, y = pts[0]
        for (a, b), s in zip(zip(pts, pts[1:]), seg):
            if run + s >= half and s > 0:
                t = (half - run) / s
                x, y = a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1])
                break
            run += s
        la, lo = P["F"].geo(x, y)
        lat.append(round(la, 6))
        lon.append(round(lo, 6))
        length.append(round(total, 1))
    return lat, lon, length


def minutes(seconds) -> str:
    return "not within the time asked" if seconds is None or not math.isfinite(float(seconds)) else f"{round(float(seconds) / 60)} min"


def n0(v) -> str:
    return f"{round(float(v)):,}"
