"""Water networks, for scripts in Python (ADR 0107): an EPANET input file's text, asked of the engine a work has built.

The computations are the civil-engineering domain's C++ on physis's networks of conduits, composed in ``api/cpp/science/water.hpp``.

    from science import water
    answer = water.ask("water_solve", inp=open("Net1.inp").read(), pressure_driven=True, required_pressure=20)

The engine is the one the Urban Studio's plug-in *Materials and flows* builds (``data/cache/engines/urban-studio/materials/engine``),
which answers water's operations beside those of materials; ``NOETARIUM_WATER_ENGINE`` names another. Each call is a process of its own.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/materials", env="NOETARIUM_WATER_ENGINE")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)
