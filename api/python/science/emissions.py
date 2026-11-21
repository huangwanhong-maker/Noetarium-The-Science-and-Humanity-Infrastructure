"""A community's greenhouse gas inventory and its mitigation, for scripts in Python and the works' servers (ADR 0147): asked of the engine
the Urban Studio's plug-in *Materials and flows* builds around ``api/cpp/science/emissions.hpp``, which documents the requests.

    from science import emissions as em
    I = em.inventory("a town", 2024, [em.entry("I.1.2", "stationary energy", 2, "residential", "CO2", activity=7.5e8, unit="kWh", factor=0.35),
                                      em.entry("V.1", "afolu", 1, "livestock", key="NE")], potentials="AR6", population=320000)
    em.totals(I)                                   # BASIC, BASIC+, territorial, uncertainty, completeness, key categories
    M = [em.measure("grid", "", "factor", 0.5, cost=2.5e7, scope=2)]      # a cleaner grid, on every sector's scope 2
    em.curve(I, M)                                 # each measure alone, in the curve's order, by its Shapley value; the overlap
    em.pathway(I, M, {"stationary energy": 0.01}, 2035, target={"year": 2030, "reduction": 0.5})

Every factor, potential, rate, cost and target is the caller's declaration; nothing here is any city's inventory or commitment.
"""

from __future__ import annotations

from .engines import EngineError  # noqa: F401
from .materials import ask

SECTORS = ("stationary energy", "transportation", "waste", "ippu", "afolu", "other")
KEYS = ("", "NO", "IE", "NE", "C")


def entry(id: str, sector: str, scope: int, subsector: str = "", gas: str = "CO2", activity: float | None = None, unit: str = "", factor: float | None = None,
          amount: float | None = None, u_activity: float = 0.0, u_factor: float = 0.0, key: str = "", included_in: str = "", grid_supply: bool = False,
          imported_waste: bool = False, biogenic: bool = False) -> dict:
    """One line of an inventory: activity × factor (kg of the gas per unit), or an amount in kg, or a notation key instead of a figure."""
    if sector not in SECTORS:
        raise ValueError(f"the sector is one of {', '.join(SECTORS)}")
    if key not in KEYS:
        raise ValueError("a notation key is NO, IE, NE or C")
    e = {"id": str(id), "sector": sector, "scope": int(scope), "subsector": str(subsector), "gas": str(gas), "unit": str(unit),
         "u_activity": float(u_activity), "u_factor": float(u_factor), "key": key, "included_in": str(included_in),
         "grid_supply": bool(grid_supply), "imported_waste": bool(imported_waste), "biogenic": bool(biogenic)}
    for k, v in (("activity", activity), ("factor", factor), ("amount", amount)):
        if v is not None:
            e[k] = float(v)
    return e


def inventory(name: str, year: int, entries: list, potentials="AR5", population: float | None = None, gdp: float | None = None, boundary: str = "") -> dict:
    I = {"name": str(name), "year": int(year), "entries": list(entries), "potentials": potentials, "boundary": str(boundary)}
    if population is not None:
        I["population"] = float(population)
    if gdp is not None:
        I["gdp"] = float(gdp)
    return I


def measure(id: str, sector: str, on: str, reduction: float, cost: float = 0.0, scope: int = 0, subsector: str = "", gas: str = "", start: int = 0,
            ramp: int = 1, name: str = "") -> dict:
    """A measure scaling by (1 − reduction) what remains of an activity or a factor in the entries it acts on; sector "" acts on every
    sector."""
    if on not in ("activity", "factor"):
        raise ValueError("a measure acts on an activity or a factor")
    if sector and sector not in SECTORS:
        raise ValueError(f"the sector is empty (every sector) or one of {', '.join(SECTORS)}")
    if not 0 <= reduction <= 1:
        raise ValueError("a reduction between 0 and 1")
    return {"id": str(id), "name": str(name or id), "sector": sector, "on": on, "reduction": float(reduction), "cost": float(cost), "scope": int(scope),
            "subsector": str(subsector), "gas": str(gas), "start": int(start), "ramp": int(ramp)}


def totals(inventory: dict, level: str = "basic+") -> dict:
    return ask("emissions.inventory", inventory=inventory, level=level)


def curve(inventory: dict, measures: list, level: str = "basic+", order: list | None = None) -> dict:
    return ask("emissions.curve", inventory=inventory, measures=list(measures), level=level, order=list(order or []))


def pathway(inventory: dict, measures: list, growth: dict, last: int, target: dict | None = None, level: str = "basic+") -> dict:
    for s in growth:
        if s not in SECTORS:
            raise ValueError(f"growth by sector: {', '.join(SECTORS)}")
    return ask("emissions.pathway", inventory=inventory, measures=list(measures), growth={k: float(v) for k, v in growth.items()}, last=int(last),
               target=dict(target or {}), level=level)
