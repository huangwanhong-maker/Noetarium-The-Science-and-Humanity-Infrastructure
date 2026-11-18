"""What the tools of needs share: the dataset a run is about (a case the data library made), the engine asked through science.needs,
and what a statement's evidence says in words and as a row. Nothing of the computation is done again here."""

from __future__ import annotations

import math

from science import needs as nd


def case_params(api):
    made = [c["case"] for c in nd.cases()]
    return [api.param("case", "Dataset", made[0] if made else "melbourne-assistance", kind="text",
                      help="a dataset the data library has made (" + (", ".join(made) or "none yet: python data/products/needs_tables.py") + ")")]


def data_of(params: dict) -> tuple[dict, str]:
    case = str(params.get("case") or "").strip() or "melbourne-assistance"
    return nd.data(case), case


def num(x):
    return x if isinstance(x, (int, float)) and math.isfinite(x) else None


def pct(x, digits: int = 1) -> str:
    return "–" if num(x) is None else f"{100 * x:.{digits}f}%"


def standing_words(ev: dict) -> str:
    st = (ev.get("standing") or {}).get("standing", "?")
    c = ev.get("confirmation")
    if c:
        return f"{st}: " + ("confirmed on the areas kept aside" if c.get("confirmed") else "the opposite gap on the areas kept aside" if c.get("opposite") else "not confirmed on the areas kept aside")
    return f"{st}: not yet tested on the areas kept aside"


def row(text: str, ev: dict) -> dict:
    d = ev.get("discovery") or {}
    c = ev.get("confirmation") or {}
    return {"statement": text, "standing": (ev.get("standing") or {}).get("standing"), "group_value": num(d.get("group_value")),
            "reference_value": num(d.get("reference_value")), "gap": num(d.get("estimate")), "low": num(d.get("low")), "high": num(d.get("high")),
            "z": num(d.get("z")), "people": num(d.get("coverage")), "stability": num((ev.get("stability") or {}).get("share")),
            "confirmed": c.get("confirmed") if c else None, "confirmation_p": num(c.get("p")) if c else None,
            "causal": (ev.get("causal") or {}).get("verdict"), "simulation": (ev.get("simulation") or {}).get("verdict"), "rules": (ev.get("rules") or {}).get("verdict")}


def columns(rows: list[dict]) -> dict:
    keys = list(rows[0].keys()) if rows else ["statement"]
    return {k: [r.get(k) for r in rows] for k in keys}
