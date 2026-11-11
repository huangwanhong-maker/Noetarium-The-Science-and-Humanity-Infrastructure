"""Market Studio's engine, built.

``engine/main.cpp`` is a program around the API's composition of the markets domain (``api/cpp/science/markets.hpp``): header-only
C++20 on the markets domain, the urban-science domain's access, peira's survival, the domains and foundations beneath them, and the
API's JSON. It is compiled here by g++ 11 or later, natively where there is one and otherwise in WSL, into
``data/cache/engines/market-studio/``, beside a ``build.json`` holding the hash of every file it was built from: its source and the
headers it includes, followed transitively, so that work elsewhere in the foundations does not make it stale. Under WSL the build
takes the lock that the sessions working on this repository share (``%TEMP%/science-wsl.lock``, a directory created atomically, with
an ``owner`` file), so that two compilations do not run there at once.

The built engine answers one request on its input (or, with ``--serve``, a request a line). The tools ask it through
``api/python/science/markets.py``, each request a process of its own.

Usage::

    python applications/market-studio/engine.py build [--force]
    python applications/market-studio/engine.py status
    python applications/market-studio/engine.py ask '<request json>'     (or - to read the request from the input)
"""

from __future__ import annotations

import argparse
import contextlib
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
SOURCE = APP / "engine" / "main.cpp"
OUT = ROOT / "data" / "cache" / "engines" / "market-studio"
PROGRAM = OUT / "markets"
INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)


def include_dirs() -> list[Path]:
    dirs = [ROOT / "foundations" / "morphe" / "include"]
    for pattern in ("foundations/morphe/*/include", "foundations/calx/numerics/*/include", "foundations/calx/compute/*/include",
                    "foundations/calx/compute/backends/*/include", "foundations/physis/*/include", "foundations/tyche/*/include",
                    "foundations/peira/*/include", "foundations/nomos/*/include", "domains/markets/*/include", "domains/economics/*/include",
                    "domains/demography/*/include", "domains/urban-science/*/include", "domains/architecture/*/include"):
        dirs += sorted(p for p in ROOT.glob(pattern) if p.is_dir())
    dirs += [ROOT / "api" / "cpp", APP / "engine"]
    return [d for d in dirs if d.is_dir()]


def sources() -> list[Path]:
    """The engine's source and the project headers it includes, transitively."""
    dirs = include_dirs()
    seen: set[Path] = set()
    todo = [SOURCE] if SOURCE.exists() else []
    while todo:
        f = todo.pop()
        if f in seen:
            continue
        seen.add(f)
        for inc in INCLUDE.findall(f.read_text(encoding="utf-8", errors="replace")):
            for d in [f.parent, *dirs]:
                candidate = d / inc
                if candidate.is_file():
                    todo.append(candidate.resolve())
                    break
    return sorted(seen)


def digest() -> str:
    h = hashlib.sha256()
    for f in sources():
        h.update(f.relative_to(ROOT).as_posix().encode())
        h.update(b"\0")
        h.update(f.read_bytes())
    return h.hexdigest()


def wsl_path(p: Path) -> str:
    s = str(p.resolve())
    if len(s) > 1 and s[1] == ":":
        return f"/mnt/{s[0].lower()}{s[2:]}".replace("\\", "/")
    return s.replace("\\", "/")


def toolchain() -> str | None:
    """'native' where g++ is on the PATH (outside Windows), 'wsl' where it is in WSL, else None."""
    if shutil.which("g++") and os.name != "nt":
        return "native"
    if os.name == "nt" and shutil.which("wsl"):
        try:
            r = subprocess.run(["wsl", "--exec", "sh", "-c", "command -v g++"], capture_output=True, timeout=60)
            if r.returncode == 0 and r.stdout.strip():
                return "wsl"
        except (OSError, subprocess.TimeoutExpired):
            pass
    return "native" if shutil.which("g++") else None


def command(kind: str, args: list[str]) -> list[str]:
    return ["wsl", "--exec", *args] if kind == "wsl" else args


def path_for(kind: str, p: Path) -> str:
    return wsl_path(p) if kind == "wsl" else str(p)


@contextlib.contextmanager
def wsl_lock(kind: str, log=print, wait: float = 1800):
    """The sessions' shared lock on WSL, held while compiling there."""
    if kind != "wsl":
        yield
        return
    lock = Path(tempfile.gettempdir()) / "science-wsl.lock"
    t0, told = time.time(), False
    while True:
        try:
            lock.mkdir()
            break
        except FileExistsError:
            if not told:
                owner = (lock / "owner").read_text(encoding="utf-8", errors="replace").strip() if (lock / "owner").exists() else "another session"
                log(f"Waiting for WSL, held by {owner}.")
                told = True
            if time.time() - t0 > wait:
                raise RuntimeError("WSL stayed busy; try again later")
            time.sleep(5)
    try:
        (lock / "owner").write_text(f"market-studio build {time.strftime('%Y-%m-%dT%H:%M:%S%z')}", encoding="utf-8")
        yield
    finally:
        shutil.rmtree(lock, ignore_errors=True)


def read_build() -> dict | None:
    try:
        return json.loads((OUT / "build.json").read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None


def status() -> dict:
    b = read_build()
    built = b is not None and PROGRAM.exists()
    return {"built": built, "toolchain": b.get("toolchain") if b else None, "built_at": b.get("built_at") if b else None,
            "current": built and b.get("sources") == digest()}


def build(force: bool = False, log=print) -> bool:
    if not force and status()["current"]:
        log("Market Studio's engine is current.")
        return True
    kind = toolchain()
    if kind is None:
        log("No C++ compiler: install g++ 11 or later (on Windows, in WSL).")
        return False
    OUT.mkdir(parents=True, exist_ok=True)
    stamp = digest()
    args = ["g++", "-std=c++20", "-O2", "-DNDEBUG", "-pthread"]
    args += [f"-I{path_for(kind, d)}" for d in include_dirs()]
    args += [path_for(kind, SOURCE), "-o", path_for(kind, PROGRAM)]
    log(f"Compiling Market Studio's engine with g++ ({kind}); some minutes.")
    t0 = time.time()
    with wsl_lock(kind, log):
        r = subprocess.run(command(kind, args), capture_output=True, text=True, encoding="utf-8", errors="replace")
    if r.returncode != 0:
        log(r.stderr[-6000:])
        log("The engine did not compile.")
        return False
    record = {"sources": stamp, "toolchain": kind, "built_at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()), "seconds": round(time.time() - t0, 1)}
    (OUT / "build.json").write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
    log(f"Built in {record['seconds']} s: {PROGRAM}")
    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = parser.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("build")
    b.add_argument("--force", action="store_true")
    sub.add_parser("status")
    a = sub.add_parser("ask")
    a.add_argument("request")
    args = parser.parse_args()
    if args.cmd == "build":
        return 0 if build(args.force) else 1
    if args.cmd == "status":
        print(json.dumps(status(), indent=1))
        return 0
    sys.path.insert(0, str(ROOT / "api" / "python"))
    from science import markets
    sys.stdout.reconfigure(encoding="utf-8")
    request = json.loads(sys.stdin.read() if args.request == "-" else args.request)
    try:
        print(json.dumps(markets.ask(request.pop("op", "status"), **request), ensure_ascii=False))
    except markets.EngineError as error:
        print(str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
