"""Scorecards and knowledge programmes, for scripts in Python and the works' servers (ADR 0144): asked of the engine a work has built
around ``api/cpp/science/scorecards.hpp`` and ``api/cpp/science/learning.hpp``, which document the requests.

    from science import programmes as pg
    S = pg.scheme("a resilience scorecard", levels=[pg.level("0", "none"), ..., pg.level("5", "complete")],
                  statements=[pg.statement("e1.1", "the hazard scenarios are agreed"), ...],
                  nodes=[pg.node("e1", ["e1.1", "e1.2"], "median"), pg.node("all", ["e1", ...], "profile")], root="all")
    C = pg.card("a city", "2026-03-01", "the panel", [pg.score_of("e1.1", "2", evidence=[pg.evidence("the risk assessment", "the archive")]), ...])
    pg.score(S, C, weak_at="2")                  # each node's level with what decided it; the weaknesses at 2 or below with their evidence
    pg.assess(programme, "2026-09-30", needs_from=[{"participant": "A", "scheme": S, "card": C, "weak_at": "2"}])

Nothing here decides what a scheme should be or whether a programme was worth doing. Ordinal levels are aggregated only by rules
meaningful on them, and a mean of levels is computed only when a scheme declares numbers for them, marked as assuming them equally
spaced. Every contribution claim is read from its chain of steps and its rivals, a chain no better shown than its weakest step.

The engine is a program built by a work: the Urban Studio's plug-in *Programmes and public finance* builds
``data/cache/engines/urban-studio/programmes/engine``; ``NOETARIUM_PROGRAMMES_ENGINE`` names another.
"""

from __future__ import annotations

from .engines import Engine, EngineError  # noqa: F401

ENGINE = Engine("urban-studio/programmes", env="NOETARIUM_PROGRAMMES_ENGINE")

RULES = ("weakest_link", "conversion", "median", "threshold", "profile", "interval_mean")
STATES = ("started", "completed", "dropped", "revised")


def ask(op: str, timeout: float = 300.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


# ---- Scorecards ------------------------------------------------------------------------------------------------------------------

def level(code: str, meaning: str = "") -> dict:
    if not code or code in ("NA", "NR"):
        raise ValueError("a level has a code of its own, other than NA and NR")
    return {"code": str(code), "meaning": str(meaning)}


def statement(id: str, text: str, guidance: str = "") -> dict:
    return {"id": str(id), "text": str(text), "guidance": str(guidance)}


def node(id: str, children: list, rule: str = "weakest_link", title: str = "", plus: bool = False, table: dict | None = None,
         thresholds: list | None = None, exclude_unrated: bool = False) -> dict:
    """A node of a scheme aggregating statements or nodes by a declared rule; `table` for a conversion ({"A B": "B+"}), `thresholds`
    [{level, at_least, of_level}] for a threshold rule, read in order."""
    if rule not in RULES:
        raise ValueError(f"a rule is one of {', '.join(RULES)}, not {rule!r}")
    if not children:
        raise ValueError("a node aggregates something")
    if rule == "conversion" and not table:
        raise ValueError("a conversion needs its table")
    if rule == "threshold" and not thresholds:
        raise ValueError("a threshold rule needs its conditions")
    n = {"id": str(id), "title": title or str(id), "children": [str(c) for c in children], "rule": rule, "plus": bool(plus), "exclude_unrated": bool(exclude_unrated)}
    if table:
        n["table"] = dict(table)
    if thresholds:
        n["thresholds"] = list(thresholds)
    return n


def scheme(name: str, levels: list, statements: list, nodes: list, root: str | None = None, source: str = "", values: list | None = None) -> dict:
    """A scorecard: levels lowest first, statements, nodes in a tree under the root; `values` (a number for each level) only for an
    interval mean."""
    if len(levels) < 2:
        raise ValueError("a scheme has at least two levels")
    s = {"name": str(name), "source": str(source), "levels": levels, "statements": statements, "nodes": nodes, "root": root or nodes[-1]["id"]}
    if values is not None:
        if len(values) != len(levels):
            raise ValueError("a number for each level")
        s["values"] = [float(v) for v in values]
    return s


def evidence(text: str, source: str = "") -> dict:
    return {"text": str(text), "source": str(source)}


def score_of(statement: str, level: str, evidence: list | None = None, assessor: str = "", note: str = "") -> dict:
    return {"statement": str(statement), "level": str(level), "evidence": list(evidence or []), "assessor": str(assessor), "note": str(note)}


def card(unit: str, date: str, assessor: str, scores: list) -> dict:
    return {"unit": str(unit), "date": str(date), "assessor": str(assessor), "scores": scores}


def score(scheme: dict, card: dict, weak_at: str | None = None) -> dict:
    req = {"scheme": scheme, "card": card}
    if weak_at is not None:
        req["weak_at"] = str(weak_at)
    return ask("scorecard.score", **req)


def compare(scheme: dict, before: dict, after: dict) -> dict:
    return ask("scorecard.compare", scheme=scheme, before=before, after=after)


def agreement(scheme: dict, cards: list) -> dict:
    if len(cards) < 2:
        raise ValueError("the agreement of at least two assessors")
    return ask("scorecard.agreement", scheme=scheme, cards=cards)


# ---- Knowledge and learning programmes ----------------------------------------------------------------------------------------------

def update(date: str, state: str, note: str = "", source: str = "") -> dict:
    if state not in STATES:
        raise ValueError(f"an update is one of {', '.join(STATES)}, not {state!r}")
    return {"date": str(date), "state": state, "note": str(note), "source": str(source)}


def assess(programme: dict, date: str, needs_from: list | None = None, timeout: float = 300.0) -> dict:
    """A programme read at a date: needs and their coverage, action plans with their pace, results on the ladder, contribution claims,
    formats compared, asks. With `needs_from` [{participant, scheme, card, weak_at}], a scorecard's weaknesses join the needs."""
    for k in ("ladder", "grades", "participants"):
        if not programme.get(k):
            raise ValueError(f"a programme declares its {k}")
    req = {"programme": programme, "date": str(date)}
    if needs_from:
        req["needs_from"] = list(needs_from)
    return ask("learning.assess", timeout=timeout, **req)
