"""What the tools of emissions share (ADR 0147): an inventory and its measures read from a project's tables by their columns, as the
API's science.emissions states them. Nothing of the computation is done here; the industrial-ecology domain's `emissions` does it."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402
from science import emissions as em  # noqa: E402

INVENTORY_COLUMNS = [("id", "Id", "id"), ("sector", "Sector", "sector"), ("scope", "Scope", "scope"), ("subsector", "Subsector", "subsector"),
                     ("gas", "Gas", "gas"), ("activity", "Activity", "activity"), ("unit", "Unit", "unit"), ("factor", "Factor (kg of gas per unit)", "factor"),
                     ("amount", "Amount (kg of gas)", "amount"), ("u_activity", "Uncertainty of activity", "u_activity"),
                     ("u_factor", "Uncertainty of factor", "u_factor"), ("key", "Notation key", "key"), ("included_in", "Included in", "included_in"),
                     ("flag", "Flag (grid_supply, imported_waste, biogenic)", "flag")]
MEASURE_COLUMNS = [("m_id", "Measure", "id"), ("m_name", "Name", "name"), ("m_sector", "Sector (empty: every)", "sector"), ("m_scope", "Scope (0 or empty: any)", "scope"),
                   ("m_subsector", "Subsector", "subsector"), ("m_gas", "Gas", "gas"), ("m_on", "On (activity or factor)", "on"),
                   ("m_reduction", "Reduction", "reduction"), ("m_cost", "Cost a year", "cost"), ("m_start", "First year", "start"), ("m_ramp", "Years to full effect", "ramp")]


def inventory_params(api, table="inventory"):
    return [api.param("table", "Inventory", table, kind="text", role="table", help="one row an entry of the inventory")] + \
           [api.param(k, label, d, kind="text", role="column", of="table") for k, label, d in INVENTORY_COLUMNS] + \
           [api.param("potentials", "Global warming potentials", "AR6", kind="choice", options=["AR6", "AR5"]),
            api.param("year", "Year of the inventory", 2024, step=1), api.param("population", "Residents", 0, minimum=0, step=1000),
            api.param("gdp", "GDP (0: not given)", 0, minimum=0)]


def inventory_from(board, params) -> dict:
    t = board.table(params["table"])
    T = {k: m.texts(t, params.get(k) or d, optional=k not in ("id", "sector", "scope")) for k, _, d in INVENTORY_COLUMNS if k not in ("activity", "factor", "amount", "u_activity", "u_factor")}
    N = {k: m.nums(t, params.get(k) or d, optional=True) for k, _, d in INVENTORY_COLUMNS if k in ("activity", "factor", "amount", "u_activity", "u_factor")}
    entries = []
    for i in range(t.rows):
        flag = T["flag"][i].strip().lower()
        entries.append(em.entry(T["id"][i], T["sector"][i].strip().lower(), int(float(T["scope"][i] or 1)), T["subsector"][i], T["gas"][i] or "CO2",
                                activity=N["activity"][i], unit=T["unit"][i], factor=N["factor"][i], amount=N["amount"][i],
                                u_activity=N["u_activity"][i] or 0.0, u_factor=N["u_factor"][i] or 0.0, key=T["key"][i].strip().upper(),
                                included_in=T["included_in"][i], grid_supply=flag == "grid_supply", imported_waste=flag == "imported_waste", biogenic=flag == "biogenic"))
    pop = float(params.get("population") or 0)
    gdp = float(params.get("gdp") or 0)
    return em.inventory(params["table"], int(params.get("year") or 2024), entries, potentials=params.get("potentials") or "AR6",
                        population=pop if pop > 0 else None, gdp=gdp if gdp > 0 else None)


def measure_params(api, table="measures"):
    return [api.param("measures", "Measures", table, kind="text", role="table", help="one row a measure")] + \
           [api.param(k, label, d, kind="text", role="column", of="measures") for k, label, d in MEASURE_COLUMNS]


def measures_from(board, params) -> list:
    t = board.table(params["measures"])
    get = lambda k, d: m.texts(t, params.get(k) or d, optional=k not in ("m_id", "m_on", "m_reduction"))  # noqa: E731
    cols = {k: get(k, d) for k, _, d in MEASURE_COLUMNS}
    out = []
    for i in range(t.rows):
        num = lambda k, fallback: float(cols[k][i]) if cols[k][i] not in ("", None) else fallback  # noqa: E731
        out.append(em.measure(cols["m_id"][i], cols["m_sector"][i].strip().lower(), cols["m_on"][i].strip().lower(), num("m_reduction", 0.0), cost=num("m_cost", 0.0),
                              scope=int(num("m_scope", 0)), subsector=cols["m_subsector"][i], gas=cols["m_gas"][i], start=int(num("m_start", 0)),
                              ramp=int(num("m_ramp", 1)), name=cols["m_name"][i]))
    return out


def tonnes(x) -> str:
    return "—" if x is None else f"{x:,.0f} t"
