"""Steering a system known through evidence, for scripts in Python and the works' servers (the design of 2026-11-19,
``development/ideas/2026-11-19-an-engagement-with-a-city.md``): the trajectory of an engagement kept by kybernesis's record, and the
transfer of a practice assessed by a lens, asked of the engine a work has built.

The computations are composed in ``api/cpp/science/steering.hpp``, which documents the trajectory (``noetarium.trajectory/1``), the
engagement's predefined methods and the operations:

    from science import steering
    t = steering.open_engagement(rules=steering.DEFAULT_RULES, by="an analyst")   # the standard architecture, its first values
    t = steering.act(t, "observe", block="Collection", turn=0, by="the ledger", role="analyst", outputs=[items])["trajectory"]
    t = steering.act(t, "fill", block="Constitution", turn=0, by="an analyst", role="analyst", outputs=[terms, operators, standards, rules])["trajectory"]
    r = steering.ask("turn", trajectory=t, branch="main", turn=0, when=steering.stamp())   # {trajectory, progress}
    steering.ask("view", trajectory=r["trajectory"])
    steering.ask("transfer", source=…, target=…, actions=[…], readings=[…])

The engine is a program built by a work: the Urban Studio's plug-in *From evidence to action* builds
``data/cache/engines/urban-studio/engagement/engine``; ``NOETARIUM_STEERING_ENGINE`` names another.

Instants are given here, by the edge, in UTC: the record keeps no clock and refuses an act dated before its last.
"""

from __future__ import annotations

import hashlib
import json
import time
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/engagement", env="NOETARIUM_STEERING_ENGINE")
ROOT = Path(__file__).resolve().parents[3]
STANDARD = ROOT / "foundations" / "kybernesis" / "architectures" / "standard.arch"
SCHEMA = "noetarium.trajectory/1"

# Who may do what at which block, by role, unless an engagement states its own: anyone of the roles observes and fills the boundaries
# and the blocks a person usually holds; an analyst amends; a lead reviews, fills the policy and the decision, and chooses which of
# a block's methods fills it.
DEFAULT_RULES = [
    {"block": "*", "act": "observe", "role": "analyst"},
    {"block": "Constitution", "act": "fill", "role": "analyst"},
    {"block": "Proposals", "act": "fill", "role": "*"},
    {"block": "Interpretation", "act": "fill", "role": "analyst"},
    {"block": "Estimation", "act": "fill", "role": "analyst"},
    {"block": "Evaluation", "act": "fill", "role": "analyst"},
    {"block": "*", "act": "amend", "role": "analyst"},
    {"block": "PolicyMaking", "act": "fill", "role": "lead"},
    {"block": "Policy", "act": "fill", "role": "lead"},
    {"block": "Decision", "act": "fill", "role": "lead"},
    {"block": "*", "act": "review", "role": "lead"},
    {"block": "*", "act": "choose", "role": "lead"},
]

# The first values of the wires the standard architecture carries to the next turn.
FIRST = {
    "Estimation.belief -> Estimation.prior next": {},
    "Policy.current -> PolicyMaking.current next": {},
    "Decision.intervention -> Estimation.action next": {},
    "Decision.inquiry -> Collection.inquiry next": {},
}

_LAST = [""]


def stamp() -> str:
    """Now, in UTC, to the millisecond, never before the last instant given here (the record refuses instants that go back)."""
    now = time.time()
    s = time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(now)) + f".{int(now * 1000) % 1000:03d}Z"
    s = max(s, _LAST[0])
    _LAST[0] = s
    return s


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def standard() -> str:
    """kybernesis's standard architecture, as its script."""
    return STANDARD.read_text(encoding="utf-8")


def open_engagement(rules: list | None = None, by: str = "someone", script: str | None = None, when: str | None = None) -> dict:
    """A new trajectory on an architecture (the standard one unless a script is given), with the first values of its carried wires."""
    t = ask("open", script=script or standard(), rules=rules if rules is not None else DEFAULT_RULES, by=by, when=when or stamp())["trajectory"]
    for wire, value in FIRST.items():   # a revised architecture without one of these wires refuses it, and is left as it is
        r = ask("act", trajectory=t, act="initial", wire=wire, value=json.dumps(value), by=by, when=stamp())
        if r["outcome"]["holds"]:
            t = r["trajectory"]
    return t


def act(trajectory: dict, what: str, when: str | None = None, **fields) -> dict:
    """One act on a trajectory: {trajectory, outcome}. Values given as Python objects are written as JSON."""
    if "outputs" in fields:
        fields["outputs"] = [o if isinstance(o, str) else json.dumps(o, ensure_ascii=False) for o in fields["outputs"]]
    return ask("act", trajectory=trajectory, act=what, when=when or stamp(), **fields)


def attach(trajectory: dict, data: bytes, name: str, kind: str = "", by: str = "someone") -> tuple[dict, str]:
    """A file recorded by the SHA-256 of its bytes (the work keeps the bytes): (trajectory, digest)."""
    digest = hashlib.sha256(data).hexdigest()
    r = act(trajectory, "attach", file={"name": name, "digest": digest, "kind": kind, "bytes": len(data)}, by=by)
    if not r["outcome"]["holds"]:
        raise EngineError(r["outcome"]["reason"])
    return r["trajectory"], digest


def turn(trajectory: dict, number: int, branch: str = "main", start: int | None = None, carry: bool = False, by: str = "someone", role: str = "") -> dict:
    """A turn run by the methods chosen, or every turn from `start` again: {trajectory, progress}. With `carry`, the boundaries without a
    value at the turn first take the values they held at the turn before, entered by `by` in `role` (progress["carried"])."""
    request = {"trajectory": trajectory, "branch": branch, "turn": number, "when": stamp()}
    if start is not None:
        request["from"] = start
    if carry:
        request.update(carry=True, by=by, role=role)
    return ask("turn", **request)


def items_of(ledger: dict) -> list:
    """The items of a project's ledger (project_archive.ledger) as Collection's observation: each with its provenance in `from` kept as
    its source."""
    out = []
    for it in ledger.get("items", []):
        item = {k: v for k, v in it.items() if k != "from"}
        f = it.get("from", {})
        item.setdefault("source", f"{f.get('tool', '')} (step {f.get('step', '')}, run {str(f.get('run', ''))[:12]})")
        if f.get("stale"):
            item["note"] = (item.get("note", "") + " The step it came from is stale.").strip()
        out.append(item)
    return out
