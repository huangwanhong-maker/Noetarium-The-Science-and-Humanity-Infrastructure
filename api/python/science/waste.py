"""A city's municipal solid waste and SDG 11.6.1, for scripts in Python and the works' servers (ADR 0148): asked of the engine the Urban
Studio's plug-in *Materials and flows* builds around ``api/cpp/science/waste.hpp``, which documents the request.

    from science import waste as ws
    S = ws.survey({1: 52000, 2: 31000, 3: 17000}, [ws.household(1, 1, 4, 9.1, 7), ...])
    ws.account(S, ws.WACT_LADDER, [ws.facility("L", "disposal", 310, met=["fenced", "weighbridge"]), ...], other={"markets": 40})

``WACT_LADDER`` names the five levels of control of UN-Habitat's Waste Wise Cities Tool, controlled from basic control as SDG 11.6.1's
metadata reads it; its criteria are placeholders a caller replaces with the checklist it applies. Every amount and design is declared.
"""

from __future__ import annotations

from .engines import EngineError  # noqa: F401
from .materials import ask

WACT_LADDER = {"levels": ["very limited control", "limited control", "basic control", "improved control", "full control"],
               "criteria": [[], ["access controlled"], ["waste placed and compacted", "covered"], ["leachate managed", "records kept"], ["gas managed", "monitored"]],
               "controlled_from": 2}


def household(stratum: int, area: int, persons: float, kg: float, days: float) -> dict:
    if persons <= 0 or days <= 0 or kg < 0:
        raise ValueError("a household has persons, days weighed and a weight that is not negative")
    return {"stratum": int(stratum), "area": int(area), "persons": float(persons), "kg": float(kg), "days": float(days)}


def survey(households: dict, sample: list, confidence: float = 0.95) -> dict:
    return {"households": {str(k): float(v) for k, v in households.items()}, "sample": list(sample), "confidence": float(confidence)}


def facility(id: str, kind: str, received: float, recovered: float = 0.0, residue_to: str = "", met: list | None = None, name: str = "") -> dict:
    if kind not in ("recovery", "disposal"):
        raise ValueError("a facility is a recovery or a disposal facility")
    return {"id": str(id), "name": str(name or id), "kind": kind, "received": float(received), "recovered": float(recovered), "residue_to": str(residue_to),
            "met": list(met or [])}


def sorted_sample(stratum: int, area: int, kg: dict) -> dict:
    return {"stratum": int(stratum), "area": int(area), "kg": {str(k): float(v) for k, v in kg.items()}}


def account(survey: dict, ladder: dict, facilities: list, other: dict | None = None, not_delivered: float = 0.0, sorted: list | None = None) -> dict:
    return ask("waste.account", survey=survey, ladder=ladder, facilities=list(facilities), other={k: float(v) for k, v in (other or {}).items()},
               not_delivered=float(not_delivered), sorted=list(sorted or []))
