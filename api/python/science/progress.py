"""Progress among units toward declared targets, for scripts in Python and the works' servers: nomos's trend to target (its decision
0019, after the United Nations Statistics Division's method), the spread among units and its decomposition between and within groups,
convergence, breaks in a trend and the links between two series across units, asked of the API's own engine
(``api/cpp/engines/progress/main.cpp``, built on first use). ``api/cpp/science/progress.hpp`` documents each request.

    from science import progress
    rows = [{"unit": "A", "year": 2015, "value": 40.0}, {"unit": "A", "year": 2023, "value": 31.0}, ...]
    progress.trend(rows, desired="decrease", target=25)         # each unit's trend to target, and the count in each class
    progress.spread(rows, groups={"A": "north"})                # Gini, Theil's T within and between groups, by year
    progress.convergence(rows, 2015, 2023)                      # the annual change regressed on the initial level
    progress.breaks(rows, unit="A")                             # breaks in the annual changes
    progress.links(rows_a, rows_b, 2015, 2023)                  # rank correlations of two series' changes across units

Units, groups and years are the caller's; nothing here names a goal, a country or an indicator.
"""

from __future__ import annotations

from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ROOT = Path(__file__).resolve().parents[3]
ENGINE = Engine("api/progress", source=ROOT / "api" / "cpp" / "engines" / "progress" / "main.cpp", env="NOETARIUM_PROGRESS_ENGINE")
OPS = ("trend", "spread", "convergence", "breaks", "links")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    return ENGINE.ask(op if op.startswith("progress.") else f"progress.{op}", timeout=timeout, **request)


def trend(rows: list, desired: str, target: float | None = None, target_year: int = 2030, base_year: int = 2015, scheme: dict | None = None,
          weights: dict | None = None) -> dict:
    req = {"rows": rows, "desired": desired, "target_year": target_year, "base_year": base_year}
    if target is not None:
        req["target"] = float(target)
    if scheme:
        req["scheme"] = scheme
    if weights:
        req["weights"] = weights
    return ask("trend", **req)


def spread(rows: list, groups: dict | None = None, weights: dict | None = None, years: list | None = None) -> dict:
    req = {"rows": rows}
    for k, v in (("groups", groups), ("weights", weights), ("years", years)):
        if v:
            req[k] = v
    return ask("spread", **req)


def convergence(rows: list, start: int, end: int, log: bool = False) -> dict:
    return ask("convergence", rows=rows, **{"from": start, "to": end, "log": bool(log)})


def breaks(rows: list, unit: str | None = None) -> dict:
    return ask("breaks", rows=rows, **({"unit": unit} if unit else {}))


def links(a: list, b: list, start: int, end: int) -> dict:
    return ask("links", a=a, b=b, **{"from": start, "to": end})
