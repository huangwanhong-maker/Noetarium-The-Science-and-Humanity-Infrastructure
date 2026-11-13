"""Society, for scripts in Python (ADR 0104, ADR 0105): a day, a population's health and care, a labour market's flows or a network stated in
plain data, asked of the engine a work has built.

The computations are the sociology and social-provision domains' C++, with demography's health expectancies and economics' labour flows,
on tyche's compositions and relational statistics, composed in ``api/cpp/science/society.hpp``. A script states what it asks and asks.

    from science import society
    answer = society.ask("sullivan", ages=ages, l=l, L=L, bands=[16, 25, ...], prevalence=[...], unit="percent")

The engine is a program built by a work: the Urban Studio's plug-in *Time, care and provision* builds
``data/cache/engines/urban-studio/provision/engine``; ``NOETARIUM_SOCIETY_ENGINE`` names another. Each call is a process of its own.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/provision", env="NOETARIUM_SOCIETY_ENGINE")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)
