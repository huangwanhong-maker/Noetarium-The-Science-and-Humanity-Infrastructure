"""The Policy Studio's engine, built: a program around the API's composition of the policy domain (``api/cpp/science/policy.hpp``),
compiled by ``science.engines`` with g++ (natively, or in WSL under the sessions' shared lock) into
``data/cache/engines/policy-studio/policy/``, and current while none of the files it was built from changes. The server keeps it
running and scripts ask it through ``science.policy``.

Usage::

    python applications/policy-studio/engine.py build [--force]
    python applications/policy-studio/engine.py status
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

ENGINES = {"policy": Engine("policy-studio/policy", APP / "engine" / "main.cpp")}
ENGINE = ENGINES["policy"]


def status() -> dict:
    each = {name: e.status() for name, e in ENGINES.items()}
    return {"built": all(s["built"] for s in each.values()), "current": all(s["current"] for s in each.values()), "engines": each}


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="The Policy Studio's engine")
    sub = parser.add_subparsers(dest="command", required=True)
    b = sub.add_parser("build")
    b.add_argument("--force", action="store_true")
    sub.add_parser("status")
    args = parser.parse_args(argv)
    if args.command == "build":
        ok = all(e.build(force=args.force) for e in ENGINES.values())
        print(json.dumps(status(), indent=2))
        return 0 if ok else 1
    print(json.dumps(status(), indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
