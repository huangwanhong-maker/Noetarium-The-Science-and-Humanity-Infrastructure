"""The Urban Studio's engines, built: one for each plug-in that brings an ``engine/main.cpp``.

A plug-in's engine is a program around one of the API's compositions (the ecology plug-in's around ``api/cpp/science/ecology.hpp``),
compiled by ``science.engines`` with g++ (natively, or in WSL under the sessions' shared lock) into
``data/cache/engines/urban-studio/<plug-in>/``, and current while none of the files it was built from changes. The tools ask it
through the API (``science.ecology`` for the ecology plug-in), each request a process of its own.

Usage::

    python applications/urban-studio/engine.py build [--force] [plug-in ...]
    python applications/urban-studio/engine.py status
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


def engines() -> dict[str, Engine]:
    out = {}
    for main in sorted((APP / "plugins").glob("*/engine/main.cpp")):
        name = main.parent.parent.name
        out[name] = Engine(f"urban-studio/{name}", main)
    return out


def status() -> dict:
    each = {name: e.status() for name, e in engines().items()}
    return {"built": all(s["built"] for s in each.values()), "current": all(s["current"] for s in each.values()), "engines": each}


def build(force: bool = False, names: list[str] | None = None, log=print) -> bool:
    ok = True
    for name, e in engines().items():
        if names and name not in names:
            continue
        ok = e.build(force=force, log=log) and ok
    return ok


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = parser.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("build")
    b.add_argument("--force", action="store_true")
    b.add_argument("names", nargs="*")
    sub.add_parser("status")
    args = parser.parse_args()
    if args.cmd == "build":
        return 0 if build(args.force, args.names) else 1
    print(json.dumps(status(), indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
