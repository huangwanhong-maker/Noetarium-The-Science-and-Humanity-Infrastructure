"""Appraisal and futures, for scripts in Python (ADR 0108): alternatives, streams, panels, fields or codes stated in plain data,
asked of the engine a work has built.

The computations are nomos's appraisal and agreement of coders, on calx's Perron vectors, composed in ``api/cpp/science/appraisal.hpp``.

    from science import appraisal
    answer = appraisal.ask("cba", benefit=[0, 30, 30], cost=[100, 0, 0], rate=0.035)

The engine is a program built by a work: the Urban Studio's plug-in *Appraisal and futures* builds
``data/cache/engines/urban-studio/appraisal/engine``; ``NOETARIUM_APPRAISAL_ENGINE`` names another. Each call is a process of its own.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/appraisal", env="NOETARIUM_APPRAISAL_ENGINE")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)
