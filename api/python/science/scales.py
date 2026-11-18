"""Systems across scales, for scripts in Python and the works' servers (ADR 0128): a hierarchy of units (blocks within districts
within a city), quantities on its levels with how each is known (observed, estimated, declared, modelled), the operators that carry
them between levels, and laws that advance them each at its own pace; estimates where a quantity was not observed, withheld where
they cannot be trusted; asked of the engine a work has built.

The computations are composed in ``api/cpp/science/scales.hpp``, which documents the document it takes (``noetarium.scales/1``) and
its operations: describe, run, ensemble, screen, fit, aggregate, estimate, carry.

    from science import scales
    scales.ask("describe", document=doc)                 # what the model is, and what it still needs
    scales.ask("run", document=doc)                      # what it does
    scales.ask("fit", document=doc, observations=obs, held_after=2016)

The engine is a program built by a work: the Urban Studio's plug-in *Cities across scales* builds
``data/cache/engines/urban-studio/scales/engine``; ``NOETARIUM_SCALES_ENGINE`` names another.

Beside the asking are the helpers a document needs and that are no computation of the foundations': ``value`` writes a value with its
provenance, ``declared`` one declared with a range, ``levels`` lays out a hierarchy from a table of units and their parents.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/scales", env="NOETARIUM_SCALES_ENGINE")
SCHEMA = "noetarium.scales/1"


def ask(op: str, timeout: float = 900.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def value(v: float | None, source: str = "observed", sigma: float | None = None) -> dict | None:
    """A value with how it is known; None stays None (no value)."""
    if v is None:
        return None
    out = {"value": float(v), "source": source}
    if sigma:
        out["sigma"] = float(sigma)
    return out


def declared(lower: float, upper: float, value: float | None = None) -> dict:
    """A value a person declares, with the range they are sure of; the centre is the middle unless given."""
    if not lower <= upper:
        raise ValueError("a declared range runs from its lower bound to its upper")
    out = {"source": "declared", "lower": float(lower), "upper": float(upper)}
    if value is not None:
        out["value"] = float(value)
    return out


def levels(rows: list[dict], order: list[str], unit_of: dict[str, str]) -> dict:
    """A hierarchy from rows that name a unit at every level: ``order`` the levels finest first, ``unit_of`` the column naming each
    level's unit. Units are listed in the order first met; every unit's parent is the unit above it in its row, and a unit met with
    two parents is refused."""
    names: dict[str, list[str]] = {lv: [] for lv in order}
    parent: dict[str, dict[str, str]] = {lv: {} for lv in order}
    for r in rows:
        for i, lv in enumerate(order):
            u = str(r[unit_of[lv]])
            if u not in names[lv]:
                names[lv].append(u)
            if i + 1 < len(order):
                p = str(r[unit_of[order[i + 1]]])
                if parent[lv].setdefault(u, p) != p:
                    raise ValueError(f"the unit {u} of {lv} has two parents, {parent[lv][u]} and {p}")
    out = []
    for i, lv in enumerate(order):
        level = {"name": lv, "units": names[lv]}
        if i + 1 < len(order):
            level["parent"] = [parent[lv][u] for u in names[lv]]
        out.append(level)
    return {"levels": out}
