"""Histos Harmonia's engine, built and asked.

``engine/main.cpp`` is a program around three of the API's compositions: historical objects (``science/histos.hpp``, on the
history domain's ``objects`` and ``dependence`` with kategoria's verdict), tunings (``science/sounding.hpp``, on musicology) and
argument maps (``science/arguments.hpp``, on philosophy). It is built by the API's builder of engines (``science.engines``) into
``data/cache/engines/histos-harmonia/`` and asked one request a process.

Usage::

    python applications/histos-harmonia/engine.py build [--force]
    python applications/histos-harmonia/engine.py status
    python applications/histos-harmonia/engine.py ask <request json>
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science.engines import Engine, EngineError, wsl_path  # noqa: E402

ENGINE = Engine("histos-harmonia", source=APP / "engine" / "main.cpp", extra_includes=[ROOT / "api" / "cpp"])


def path_for_engine(p: Path) -> str:
    """A path as the built engine reads it: as WSL names it when the engine was built there."""
    kind = (ENGINE.record() or {}).get("toolchain") or "native"
    return wsl_path(p) if kind == "wsl" else str(p)


def ask(op: str, **request) -> dict:
    return ENGINE.ask(op, timeout=120.0, **request)


def main(argv: list[str]) -> int:
    if not argv or argv[0] not in ("build", "status", "ask"):
        print(__doc__)
        return 2
    if argv[0] == "build":
        return 0 if ENGINE.build(force="--force" in argv) else 1
    if argv[0] == "status":
        print(json.dumps(ENGINE.status(), indent=1))
        return 0
    sys.stdout.reconfigure(encoding="utf-8")
    try:
        request = json.loads(argv[1])
        print(json.dumps(ask(request.pop("op"), **request), ensure_ascii=False))
    except EngineError as error:
        print(str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
