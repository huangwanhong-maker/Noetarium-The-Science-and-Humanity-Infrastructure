"""What the tools of *Programmes and public finance* share: the plug-in's example (examples/cohort.json, an invented cohort of three
cities whose every number is invented and says so) and the requests built from it. Nothing of the computation is done here; the
engine (the API's science/scorecards.hpp, science/learning.hpp and science/fiscal.hpp) does it."""

from __future__ import annotations

import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXAMPLES = HERE.parent / "examples"


def example(name: str = "cohort") -> dict:
    p = EXAMPLES / f"{name}.json"
    if not p.is_file():
        raise LookupError(f"no example {name} in the plug-in: {', '.join(x.stem for x in EXAMPLES.glob('*.json'))}")
    return json.loads(p.read_text(encoding="utf-8"))


def needs_from(E: dict) -> list:
    """Each participant's own card as a source of needs: its statements at or below the example's cut."""
    out = []
    for p in E["programme"]["participants"]:
        if p["id"] in E["cards"]:
            out.append({"participant": p["id"], "scheme": E["scheme"], "card": E["cards"][p["id"]], "weak_at": E.get("weak_at", "1")})
    return out


def percent(x) -> str:
    return "—" if x is None else f"{100 * x:.1f}%"
