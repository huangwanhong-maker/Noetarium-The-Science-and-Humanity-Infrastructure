"""What the tools of population dynamics share: the place a run is about (a field the data library made, or one it makes now), its
cells with their places, the engine asked through science.population, and what a law's coefficients say in words. Nothing of the
computation is done again here."""

from __future__ import annotations

import math
import re
import subprocess
import sys
from pathlib import Path

from science import population as pd

ROOT = Path(__file__).resolve().parents[5]
RUNS = ["growth", "logistic", "fisher", "drift", "crowding", "settled", "sparse"]


def case_params(api):
    made = [c["case"] for c in pd.cases()]
    return [api.param("case", "Place", made[0] if made else "melbourne", kind="text",
                      help="a place the data library has made (" + (", ".join(made) or "none yet") + "), or the name of one to make from the latitude and longitude below"),
            api.param("lat", "Latitude (to make a place)", "", kind="text", help="degrees; leave empty to use a place already made"),
            api.param("lon", "Longitude (to make a place)", "", kind="text"),
            api.param("half_km", "Half-width (km, to make a place)", 30, minimum=5, maximum=60, step=1)]


def field_of(params: dict, log=None) -> tuple[dict, str]:
    case = re.sub(r"[^a-z0-9-]+", "-", str(params.get("case") or "").lower()).strip("-")
    lat, lon = str(params.get("lat") or "").strip(), str(params.get("lon") or "").strip()
    if lat and lon:
        name = case or f"at-{float(lat):.3f}-{float(lon):.3f}"
        if not (pd.PRODUCTS / name / "field.json").is_file():
            r = subprocess.run([sys.executable, str(ROOT / "data" / "products" / "population_history.py"), "--at", f"{float(lat)},{float(lon)}",
                                "--half", str(float(params.get("half_km") or 30)), "--name", name], capture_output=True, text=True, timeout=1800, cwd=str(ROOT))
            if r.returncode != 0:
                raise RuntimeError("the data library could not make the place: " + ((r.stderr or r.stdout).strip().splitlines() or ["no reason given"])[-1])
        case = name
    if not case:
        raise LookupError("name a place the data library has made, or give a latitude and longitude to make one")
    return pd.field(case), case


def cells(f: dict, case: str) -> list[dict]:
    return pd.cells(f, pd.PRODUCTS / case)


def num(x):
    return None if x is None or (isinstance(x, float) and not math.isfinite(x)) else x


def pct(x: float) -> str:
    return f"{100 * x:.1f}%"


def reading_words(r: dict) -> str:
    """A law's coefficients in the quantities a reader knows."""
    parts = []
    if r.get("growth_rate_a_year") is not None:
        rate = r["growth_rate_a_year"]
        if r.get("form") == "Gompertz":
            parts.append(f"Gompertz's growth at {pct(rate)} a year" + (f" toward {r['ceiling_per_km2']:,.0f} people per km²" if r.get("ceiling_per_km2") else f", running away from {r.get('floor_per_km2', 0):,.0f} per km²"))
        elif r.get("form") == "logistic":
            parts.append(f"growth at {pct(rate)} a year" + (f" toward a ceiling of {r['ceiling_per_km2']:,.0f} people per km²" if r.get("ceiling_per_km2") else ", accelerating, with no ceiling"))
        else:
            parts.append(f"growth at {pct(rate)} a year everywhere")
    if r.get("diffusivity_km2_a_year") is not None:
        parts.append(f"spread at {r['diffusivity_km2_a_year']:.4g} km² a year")
    if r.get("crowding_km2_a_year_per_density") is not None:
        parts.append(f"crowding at {r['crowding_km2_a_year_per_density']:.3g} km² a year for each person per km²")
    if r.get("speed_toward_centre_km_a_year") is not None:
        v = r["speed_toward_centre_km_a_year"]
        parts.append(f"a drift {'toward' if v > 0 else 'away from'} the centre at {abs(v) * 1000:.1f} m a year")
    if r.get("front_speed_km_a_year") is not None:
        parts.append(f"a front of settlement advancing {r['front_speed_km_a_year'] * 1000:.0f} m a year (Fisher's 2√(rD))")
    for k, v in r.items():
        if k.startswith("drift_up_") and not k.endswith("_se") and v is not None:
            parts.append(f"a drift up {k[9:]} of {v:.3g}")
        if k == "made_everywhere_per_km2_a_year" and v is not None:
            parts.append(f"{v:+.3g} people per km² made everywhere a year")
    return "; ".join(parts) or "no term"
