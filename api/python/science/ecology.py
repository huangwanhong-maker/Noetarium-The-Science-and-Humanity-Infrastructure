"""The ecology domain, for scripts in Python (ADR 0101): a forest, a population or a landscape stated in plain data, asked of the engine a work
has built.

The computations are the ecology domain's C++ (taxonomy and distinctness, biodiversity on tyche's classes, stage-classified populations, the
forest's allometry, composition, future and crowns, the connectivity of habitat) and peira's imperfect detection, composed in
``api/cpp/science/ecology.hpp``. A script states what it asks and asks; it does not compute these again.

    from science import ecology
    answer = ecology.ask("forest", trees=ecology.trees(species, genus, family, dbh), scheme=..., limits=...)

The engine is a program built by a work: the Urban Studio's ecology plug-in builds ``data/cache/engines/urban-studio/ecology/engine``;
``NOETARIUM_ECOLOGY_ENGINE`` names another. Each call is a process of its own, so a request that fails harms no one.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/ecology", env="NOETARIUM_ECOLOGY_ENGINE")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the ecology engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def trees(species, genus, family, dbh, age=None, life_lower=None, life_upper=None, area=None) -> dict:
    """Trees as the engine reads them: columns of equal length (names "" where unknown; numbers None or NaN where unknown; each tree's
    area an index into the request's "areas", or -1)."""
    out = {"species": [str(x or "") for x in species], "genus": [str(x or "") for x in genus], "family": [str(x or "") for x in family], "dbh": list(dbh)}
    for key, value in (("age", age), ("life_lower", life_lower), ("life_upper", life_upper), ("area", area)):
        if value is not None:
            out[key] = list(value)
    n = len(out["dbh"])
    if any(len(v) != n for v in out.values()):
        raise ValueError("trees: every column of one length")
    return clean(out)
