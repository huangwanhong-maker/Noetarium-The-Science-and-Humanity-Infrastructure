"""Households in slums, informal settlements and inadequate housing, for scripts in Python and the works' servers (ADR 0155): SDG 11.1.1
from a household survey under declared readings of its open terms, asked of the engine a work has built around
``api/cpp/science/shelter.hpp``, which documents the request.

    from science import shelter
    hs = shelter.households(rows)                          # rows of a survey: dicts with the columns below
    shelter.ask("shelter.assess", households=hs, readings=shelter.readings())

The engine is the Urban Studio's plug-in *Homes and who lacks them*: ``data/cache/engines/urban-studio/shelter/engine``;
``NOETARIUM_SHELTER_ENGINE`` names another. A survey's columns: weight, stratum, cluster, district (the group reported by), persons,
rooms, water, sanitation, shared_toilet, walls, roof, floor, hazardous_site, documented, fears_eviction, housing_cost, income; a
missing answer is an empty cell.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/shelter", env="NOETARIUM_SHELTER_ENGINE")

# The codes the readings below call improved or durable, after UN-Habitat's expert group (2002) as the metadata of SDG 11.1.1 gives
# them, and the JMP's ladder where a reading says so. A declaration: a survey with other codes declares its own.
WATER_EGM = ["piped into dwelling", "piped to yard", "public tap", "tube well", "protected well", "protected spring", "rainwater", "bottled water"]
WATER_JMP = WATER_EGM + ["tanker truck", "cart with small tank"]
SANITATION = ["flush to sewer", "flush to septic tank", "flush to pit", "ventilated improved pit latrine", "pit latrine with slab", "composting toilet"]
DURABLE = ["cement blocks", "baked bricks", "stone with mortar", "concrete", "metal sheets", "tiles", "cement", "ceramic tiles"]
NUMBERS = ("weight", "stratum", "cluster", "persons", "rooms", "shared_toilet", "hazardous_site", "documented", "fears_eviction", "housing_cost", "income")
CODES = ("water", "sanitation", "walls", "roof", "floor")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    return ENGINE.ask(op, timeout=timeout, **request)


def households(rows) -> list:
    """A survey's rows as the engine takes them: numbers as numbers, codes as text, the district as the group, an empty cell omitted."""
    out = []
    for r in rows:
        h = {}
        for k in NUMBERS:
            v = r.get(k)
            if v not in (None, "") and v == v:
                h[k] = float(v)
        for k in CODES:
            v = r.get(k)
            if v not in (None, "") and v == v:
                h[k] = str(v).strip()
        if r.get("district") not in (None, ""):
            h["group"] = str(r["district"])
        out.append(h)
    return out


def readings(afford: float = 0.30, unknowns: str = "left out") -> list:
    """Six readings of SDG 11.1.1's open terms: the four components most often reported; the metadata's five, tenure secure with a
    document and no fear of eviction; the JMP's ladder (a shared toilet not improved, delivered water improved); two to a room; walls
    and roof only; a hazardous site counted against durability."""
    base = {"water_improved": WATER_EGM, "sanitation_improved": SANITATION, "durable": DURABLE, "most_per_room": 3, "afford": afford, "unknowns": unknowns}
    five = {**base, "tenure": "documented and unthreatened"}
    return [
        {**base, "name": "four, tenure left out", "source": "UN-Habitat's expert group (2002): water, sanitation, living area, durability"},
        {**five, "name": "five, the metadata's tenure", "source": "SDG 11.1.1 metadata: tenure secure with a document and protection from eviction"},
        {**five, "name": "five, JMP's ladder", "water_improved": WATER_JMP, "shared_deprives": True, "source": "the JMP: delivered water improved, a shared toilet limited"},
        {**five, "name": "five, two to a room", "most_per_room": 2, "source": "the expert group's range of two to four people to a room"},
        {**five, "name": "five, walls and roof only", "parts": ["walls", "roof"], "source": "the floor's material not counted"},
        {**five, "name": "five, a hazardous site counts", "hazard_deprives": True, "source": "SDG 11.1.1 metadata: a durable dwelling on a non-hazardous site"},
    ]
