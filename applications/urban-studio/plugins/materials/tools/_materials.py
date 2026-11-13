"""What the tools of materials and flows share: tables read by their columns, the data library's declared intensities, lifetime laws as
parameters, and the domain asked through science.materials. Nothing of the domain's computation is done again here; what is done here
is the reading of the person's tables and the wording of what came back."""

from __future__ import annotations

import json
import math
from pathlib import Path

from science import materials

ROOT = Path(__file__).resolve().parents[5]
PRODUCTS = ROOT / "data" / "cache" / "products" / "materials-and-flows"
INTENSITIES = {"RASMI 20230905 (5th, 50th and 95th percentiles)": "rasmi-scheme.json"}


def col(api, name, label, default, help=""):
    return api.param(name, label, default, kind="text", role="column", of="table", help=help)


def texts(t, name, optional=False):
    if not name or not t.has(name):
        if optional:
            return [""] * t.rows
        raise LookupError(f"the table '{t.name}' has no column '{name}'")

    def word(v):
        if v is None:
            return ""
        if isinstance(v, float):
            return "" if v != v else (str(int(v)) if v.is_integer() else f"{v:g}")
        return str(v).strip()
    return [word(v) for v in t.raw(name)]


def nums(t, name, optional=False):
    if not name or not t.has(name):
        if optional:
            return [None] * t.rows
        raise LookupError(f"the table '{t.name}' has no column '{name}'")
    return [float(x) if math.isfinite(x) else None for x in t.column(name)]


def intensity_params(api):
    return [api.param("intensities", "Material intensities", next(iter(INTENSITIES)), kind="choice", options=list(INTENSITIES),
                      help="a declared scheme of kg per m² of floor area with bounds, from the data library"),
            api.param("region", "Region of the scheme", "OECD_AUNZ", kind="text", help="the scheme's region (RASMI's codes: OECD_AUNZ, OECD_EU15, OECD_USA, ...)")]


def scheme(params) -> dict:
    """The declared scheme's rows of one region, as the domain reads a scheme."""
    path = PRODUCTS / INTENSITIES[params["intensities"]]
    if not path.exists():
        raise LookupError("the material intensities are not in the data library yet: run data/products/materials_and_flows.py (the plug-in's recipe does)")
    s = json.loads(path.read_text(encoding="utf-8"))
    region = str(params.get("region") or "").strip()
    rows = [r for r in s["rows"] if not region or r["region"] == region]
    if not rows:
        raise LookupError(f"the scheme has no region '{region}'")
    return {"name": f"{s['name']}, {region}", "materials": s["materials"], "rows": rows, "functions": s.get("functions", {}), "structures": s.get("structures", {})}


LAWS = ["weibull", "normal", "lognormal", "exponential", "fixed"]


def lifetime_params(api, mean=80.0, spread=3.0):
    return [api.param("law", "Lifetime law", "weibull", kind="choice", options=LAWS, help="declared: how long what enters the stock stays in it"),
            api.param("mean", "Mean lifetime (years)", mean, kind="number", minimum=1),
            api.param("spread", "Shape (Weibull) or standard deviation (years)", spread, kind="number", minimum=0.1,
                      help="the Weibull law's shape; the normal and lognormal laws' standard deviation; unused for the exponential and the fixed")]


def lifetime(params) -> dict:
    law, mean, spread = params["law"], float(params["mean"]), float(params["spread"])
    if law == "weibull":
        return {"law": "weibull", "shape": spread, "mean": mean}
    if law in ("normal", "lognormal"):
        return {"law": law, "mean": mean, "sd": spread}
    if law == "exponential":
        return {"law": "exponential", "mean": mean}
    return {"law": "fixed", "years": mean}


def lifetime_words(params) -> str:
    law = params["law"]
    if law == "weibull":
        return f"a Weibull law of mean {params['mean']:g} years and shape {params['spread']:g}"
    if law in ("normal", "lognormal"):
        return f"a {law} law of mean {params['mean']:g} years and standard deviation {params['spread']:g}"
    if law == "exponential":
        return f"an exponential law of mean {params['mean']:g} years"
    return f"a fixed lifetime of {params['mean']:g} years"


def tonnes(kg):
    return None if kg is None else round(kg / 1000, 1)


def big(x, unit=""):
    if x is None:
        return "—"
    for scale, word in ((1e9, " billion"), (1e6, " million"), (1e3, " thousand")):
        if abs(x) >= scale:
            return f"{x / scale:,.2f}{word}{unit}"
    return f"{x:,.0f}{unit}"


__all__ = ["materials", "col", "texts", "nums", "intensity_params", "scheme", "lifetime_params", "lifetime", "lifetime_words", "tonnes", "big"]
