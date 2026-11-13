"""Materials and flows, for scripts in Python (ADR 0106): a stock, a system of flows, a product system or an economy stated in plain data,
asked of the engine a work has built.

The computations are the industrial-ecology domain's C++, on economics' Leontief model, calx's linear algebra and leading walks, tyche's
laws and peira's survival, composed in ``api/cpp/science/materials.hpp``. A script states what it asks and asks.

    from science import materials
    answer = materials.ask("stock", mode="inflow", values=[...], lifetime={"law": "weibull", "shape": 3, "mean": 80})

The engine is a program built by a work: the Urban Studio's plug-in *Materials and flows* builds
``data/cache/engines/urban-studio/materials/engine``; ``NOETARIUM_MATERIALS_ENGINE`` names another. Each call is a process of its own.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/materials", env="NOETARIUM_MATERIALS_ENGINE")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)
