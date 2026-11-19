"""Critical transitions, for scripts in Python and the works' servers (ADR 0135): kinesis's detectors of critical transitions read on
a record of any state space (a quantity, a field over units, compositions with their totals, histograms, covariance matrices, on a
neighbourhood and a hierarchy), each saying what it sees and is blind to, how often its test is wrong at the record's length, and a
verdict of signal, no signal or undecidable with its reasons; test models with known answers; the climate domain's models with folds.

The computations are composed in ``api/cpp/science/transitions.hpp``, which documents the space, the record and the options it takes,
and answered by the Dynamics Lab's engine (``data/cache/engines/dynamics-lab/dynamics``, built by
``applications/dynamics-lab/engine.py build``); ``NOETARIUM_TRANSITIONS_ENGINE`` names another.

    from science import transitions as tr
    rec = tr.ask("model", model="fold", length=600, step=0.2)["record"]
    tr.assess(rec)                                                     # one quantity
    tr.assess(rec, space=tr.space(units=8, edges=[[0, 1], [1, 2]]))    # a field over units on a neighbourhood
    tr.assess(rec, space=tr.space(parts=5, chart="composition", with_total=True))   # a population by age, its size kept apart

Beside the asking are the helpers that write a space and a record and that are no computation of the foundations'.
"""

from __future__ import annotations

from .engines import ENGINES, Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("dynamics-lab", env="NOETARIUM_TRANSITIONS_ENGINE")
if not __import__("os").environ.get("NOETARIUM_TRANSITIONS_ENGINE"):
    ENGINE.program = ENGINES / "dynamics-lab" / "dynamics"

CHARTS = ("identity", "composition", "histogram", "covariance")
NULLS = ("phase", "iaaft", "block")
RATES = ("ar1", "increment", "red", "drift")
MODELS = ("fold", "transcritical", "pitchfork", "hopf", "flip", "stationary", "rate_tipping", "double_well", "reddening", "network")


def ask(op: str, timeout: float = 1800.0, **request) -> dict:
    """One request of critical transitions (assess, likelihood, wells, model, climate): its answer, or EngineError with the reason."""
    return ENGINE.ask("transitions." + op, timeout=timeout, **request)


def space(units: int = 1, parts: int = 1, chart: str = "identity", kind: str = "intensive", with_total: bool = False,
          edges: list | None = None, unit_names: list[str] | None = None, part_names: list[str] | None = None,
          hierarchy: dict | None = None, bin_edges: list[float] | None = None, probabilities: list[float] | None = None) -> dict:
    """A state space: units × parts read in a chart, with a neighbourhood (edges [a, b] or [a, b, weight]) and a hierarchy."""
    if chart not in CHARTS:
        raise ValueError(f"a chart is one of {', '.join(CHARTS)}")
    if kind not in ("intensive", "extensive"):
        raise ValueError("a quantity is intensive (averaged up a hierarchy) or extensive (summed)")
    if chart == "histogram" and (not bin_edges or not probabilities):
        raise ValueError("a histogram is read at declared probabilities between declared bin edges")
    out = {"units": int(units), "parts": int(parts), "chart": chart, "kind": kind, "with_total": bool(with_total)}
    if edges:
        out["edges"] = [list(e) for e in edges]
    if unit_names:
        if len(unit_names) != units:
            raise ValueError("name every unit or none")
        out["unit_names"] = list(unit_names)
    if part_names:
        if len(part_names) != parts:
            raise ValueError("name every part or none")
        out["part_names"] = list(part_names)
    if hierarchy:
        out["hierarchy"] = hierarchy
    if bin_edges:
        out["bin_edges"] = list(bin_edges)
    if probabilities:
        out["probabilities"] = list(probabilities)
    return out


def record(times: list[float], states: list) -> dict:
    """A record: a native state for each time (a number, or units × parts unit by unit)."""
    if len(times) != len(states):
        raise ValueError("a record needs a state for each time")
    return {"times": [float(t) for t in times], "states": [[float(v)] if isinstance(v, (int, float)) else [float(x) for x in v] for v in states]}


def levels(rows: list[dict], order: list[str], unit_of: dict[str, str]) -> dict:
    """A hierarchy from rows naming a unit at every level, finest first (as science.scales.levels lays it out)."""
    from .scales import levels as scales_levels
    return scales_levels(rows, order, unit_of)


def assess(rec: dict, space: dict | None = None, **options) -> dict:
    """Every indicator the space admits on the record, with verdicts and reasons; options as api/cpp/science/transitions.hpp names them."""
    request = {"record": rec, "options": options}
    if space:
        request["space"] = space
    return ask("assess", **request)


def verdicts(assessment: dict) -> list[tuple[str, str, str, str]]:
    """(channel, indicator, verdict, first reason) for each finding of an assessment."""
    return [(f["channel"], f["indicator"], f["verdict"], (f["reasons"] or [""])[0]) for f in assessment.get("findings", [])]
