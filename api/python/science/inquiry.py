"""Inquiry, for scripts in Python and the works' tools (ADR 0120): case-oriented analysis (zetesis: calibration, QCA, the coding of
texts, concept lattices, Mill's methods, process tracing) and the quantitative analyses of an archive's data (nomos's reliability,
item response theory and the agreement of coders; tyche's factors, latent classes and mixed models; peira's k-anonymity), asked of
the engine a work has built.

The computations are the foundations' C++, composed in ``api/cpp/science/inquiry.hpp``, which documents every operation:

    from science import inquiry
    inquiry.ask("reliability", items=["q1", "q2", "q3"], scores=[[1, 2, 2], [3, 3, 4], [2, 2, 3]])

The engine is a program built by a work: the Archive Studio builds ``data/cache/engines/archive-studio/inquiry/engine``;
``NOETARIUM_INQUIRY_ENGINE`` names another. Each call is a process of its own.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("archive-studio/inquiry", env="NOETARIUM_INQUIRY_ENGINE")


def ask(op: str, timeout: float = 300.0, **request) -> dict:
    """One request to the inquiry engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)
