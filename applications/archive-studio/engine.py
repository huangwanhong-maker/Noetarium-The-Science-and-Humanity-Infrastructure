"""The Archive Studio's engines, built: three programs around the API's compositions, each compiled by ``science.engines`` with g++
(natively, or in WSL under the sessions' shared lock) into ``data/cache/engines/archive-studio/<name>/``, and current while none of
the files it was built from changes. The server and the tools ask them through the API, each request a process of its own:

- ``linked``: symploke, linked data (``api/cpp/science/linked.hpp``; ``science.linked``);
- ``archive``: archeion, arrangement, custody, access, the journal, packages and datasets (``api/cpp/science/archive.hpp``;
  ``science.archive``);
- ``inquiry``: zetesis, tyche, nomos and peira, the analyses of the studio's tools (``api/cpp/science/inquiry.hpp``;
  ``science.inquiry``).

Usage::

    python applications/archive-studio/engine.py build [--force] [name ...]
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

ENGINES = {
    "linked": Engine("archive-studio/linked", APP / "engine" / "main.cpp"),
    "archive": Engine("archive-studio/archive", APP / "engine" / "archive.cpp"),
    "inquiry": Engine("archive-studio/inquiry", APP / "engine" / "inquiry.cpp"),
}
ENGINE = ENGINES["linked"]


def status() -> dict:
    each = {name: e.status() for name, e in ENGINES.items()}
    return {"built": all(s["built"] for s in each.values()), "current": all(s["current"] for s in each.values()), "engines": each}


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="The Archive Studio's engines")
    sub = parser.add_subparsers(dest="command", required=True)
    b = sub.add_parser("build")
    b.add_argument("--force", action="store_true")
    b.add_argument("names", nargs="*", help="the engines to build (default: all)")
    sub.add_parser("status")
    args = parser.parse_args(argv)
    if args.command == "build":
        ok = True
        for name in args.names or list(ENGINES):
            if name not in ENGINES:
                print(f"no engine {name} ({', '.join(ENGINES)})")
                return 2
            ok = ENGINES[name].build(force=args.force) and ok   # one at a time: WSL is not asked for two builds at once
        print(json.dumps(status(), indent=2))
        return 0 if ok else 1
    print(json.dumps(status(), indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
