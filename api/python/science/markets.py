"""The markets domain, for scripts in Python (ADR 0093): a market stated in plain data, asked of the engine a work has built.

The computations are the markets domain's C++ (demand, structure, trade areas, diversion, a new outlet, sites, sizes), urban science's
access (seconds walked on ways) and peira's survival, composed in ``api/cpp/science/markets.hpp``. A script states its market and asks;
it does not compute these again.

    from science import markets
    m = markets.market(mass, spend, seller, pairs=(b, o, cost), huff={"beta": 2}, outside=5)
    answer = markets.ask("market", market=m)             # {"demand": [...], "by_seller": [...], "structure": {...}, ...}
    pairs = markets.ask("costs", model="city-model-melbourne.json", **{"from": buyers_xy, "to": offers_xy}, limit=900)

The engine is a program built by a work (Market Studio builds ``data/cache/engines/market-studio/markets``); ``NOETARIUM_MARKETS_ENGINE``
names another. Each call is a process of its own, given the request on its input, so a script that fails harms no one.
"""

from __future__ import annotations

import json
import os
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
ENGINE = ROOT / "data" / "cache" / "engines" / "market-studio"
PRODUCTS = ROOT / "data" / "cache" / "products"


class EngineError(RuntimeError):
    """The engine is not built, did not answer, or refused the request (with its reason)."""


def _wsl(path: Path) -> str:
    s = str(Path(path).resolve())
    if len(s) > 1 and s[1] == ":":
        return f"/mnt/{s[0].lower()}{s[2:]}".replace("\\", "/")
    return s.replace("\\", "/")


def engine() -> tuple[Path, str]:
    """The engine's program and how it is run: 'native', or 'wsl' where it was built there."""
    named = os.environ.get("NOETARIUM_MARKETS_ENGINE")
    program = Path(named) if named else ENGINE / "markets"
    kind = "native"
    try:
        kind = json.loads((program.parent / "build.json").read_text(encoding="utf-8")).get("toolchain") or "native"
    except (OSError, ValueError):
        pass
    if not program.exists():
        raise EngineError(f"the markets engine is not built ({program}): run the work's recipe")
    return program, kind


def ask(op: str, timeout: float = 300.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    program, kind = engine()
    args = [(_wsl(program) if kind == "wsl" else str(program)), "--data", (_wsl(PRODUCTS) if kind == "wsl" else str(PRODUCTS))]
    if kind == "wsl":
        if not shutil.which("wsl"):
            raise EngineError("the engine was built in WSL, which is not here")
        args = ["wsl", "--exec", *args]
    try:
        done = subprocess.run(args, input=json.dumps({"op": op, **request}), capture_output=True, text=True, encoding="utf-8", timeout=timeout)
    except subprocess.TimeoutExpired as error:
        raise EngineError(f"the engine did not answer within {timeout:.0f} s") from error
    except OSError as error:
        raise EngineError(f"the engine could not be started: {error}") from error
    try:
        answer = json.loads(done.stdout)
    except ValueError as error:
        raise EngineError((done.stderr or done.stdout or "the engine gave no answer").strip()[-400:]) from error
    if not answer.get("ok"):
        raise EngineError(str(answer.get("error", "the engine failed")))
    return answer["result"]


def market(mass, spend, seller, pairs, attraction=None, huff: dict | None = None, outside: float = 0.0) -> dict:
    """A market as the engine reads it: cells of buyers (mass, spend), offers (their sellers by index, their attractions), the pairs within
    reach (cells, offers, costs), Huff's parameters, and the cost at which not buying pulls as an offer (0: no pull)."""
    b, o, cost = pairs
    offers = {"seller": [int(s) for s in seller]}
    if attraction is not None:
        offers["attraction"] = [float(a) for a in attraction]
    return {"buyers": {"mass": [float(v) for v in mass], "spend": [float(v) for v in spend]}, "offers": offers,
            "pairs": {"b": [int(v) for v in b], "o": [int(v) for v in o], "cost": [float(v) for v in cost]},
            "huff": dict(huff or {}), "outside": float(outside)}
