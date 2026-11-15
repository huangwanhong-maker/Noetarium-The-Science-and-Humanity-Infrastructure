"""The Archive Studio's engine, built: a program around the API's composition of symploke (``api/cpp/science/linked.hpp``),
compiled by ``science.engines`` with g++ (natively, or in WSL under the sessions' shared lock) into
``data/cache/engines/archive-studio/linked/``, and current while none of the files it was built from changes. The server and
the tools ask it through the API (``science.linked``), each request a process of its own.

Usage::

    python applications/archive-studio/engine.py build [--force]
    python applications/archive-studio/engine.py status
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science.engines import Engine  # noqa: E402

ENGINE = Engine("archive-studio/linked", APP / "engine" / "main.cpp")


def status() -> dict:
    s = ENGINE.status()
    return {"built": s["built"], "current": s["current"], "engines": {"linked": s}}


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="The Archive Studio's engine")
    sub = parser.add_subparsers(dest="command", required=True)
    b = sub.add_parser("build")
    b.add_argument("--force", action="store_true")
    sub.add_parser("status")
    args = parser.parse_args(argv)
    if args.command == "build":
        ok = ENGINE.build(force=args.force)
        print(json.dumps(status(), indent=2))
        return 0 if ok else 1
    print(json.dumps(status(), indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
