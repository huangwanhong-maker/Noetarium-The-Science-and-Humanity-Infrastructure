"""The Category Studio's engine, built.

``engine/main.cpp`` is a program around the API's composition of kategoria's script language
(``api/cpp/science/categories.hpp``): header-only C++20 on kategoria, morphe and the API's JSON. It is compiled here by g++ 11 or
later, natively where there is one and otherwise in WSL, into ``data/cache/engines/category-studio/``, beside a ``build.json``
holding the hash of every file it was built from: its source and the headers it includes, followed transitively, so that work
elsewhere in the foundations does not make it stale. Under WSL the build takes the lock that the sessions working on this
repository share (``%TEMP%/science-wsl.lock``, a directory created atomically, with an ``owner`` file), so that two compilations
do not run there at once.

The built engine answers one request on its input or, with ``--serve``, a request a line; the studio's server keeps one alive.

Usage::

    python applications/category-studio/engine.py build [--force]
    python applications/category-studio/engine.py status
    python applications/category-studio/engine.py ask '<request json>'     (or - to read the request from the input)
    python applications/category-studio/engine.py run <a script's file>
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
OUT = ROOT / "data" / "cache" / "engines" / "category-studio"
PROGRAM = OUT / "categories"
INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)


def include_dirs() -> list[Path]:
    dirs = [ROOT / "foundations" / "morphe" / "include"]
    for pattern in ("foundations/morphe/*/include", "foundations/kategoria/*/include"):
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
        (lock / "owner").write_text(f"category-studio build {time.strftime('%Y-%m-%dT%H:%M:%S%z')}", encoding="utf-8")
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
        log("The Category Studio's engine is current.")
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
    log(f"Compiling the Category Studio's engine with g++ ({kind}); a minute or two.")
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


def ask(request: dict, timeout: float = 120) -> dict:
    """One request answered by a process of its own; the result, or RuntimeError with the engine's reason."""
    b = read_build()
    if b is None or not PROGRAM.exists():
        raise RuntimeError("the engine is not built: python applications/category-studio/engine.py build")
    kind = b.get("toolchain", "native")
    r = subprocess.run(command(kind, [path_for(kind, PROGRAM)]), input=json.dumps(request), capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=timeout)
    if r.returncode != 0 or not r.stdout.strip():
        raise RuntimeError(r.stderr.strip()[-2000:] or "the engine answered nothing")
    reply = json.loads(r.stdout)
    if not reply.get("ok"):
        raise RuntimeError(reply.get("error", "the engine refused"))
    return reply["result"]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("build")
    b.add_argument("--force", action="store_true")
    sub.add_parser("status")
    a = sub.add_parser("ask")
    a.add_argument("request")
    r = sub.add_parser("run")
    r.add_argument("script")
    args = parser.parse_args()
    if args.cmd == "build":
        return 0 if build(args.force) else 1
    if args.cmd == "status":
        print(json.dumps(status(), indent=1))
        return 0
    sys.stdout.reconfigure(encoding="utf-8")
    try:
        if args.cmd == "run":
            for o in ask({"op": "run", "script": Path(args.script).read_text(encoding="utf-8")})["outcomes"]:
                print(f"{o['line']:>4}  {o['status']:<9}  {o.get('claim') or o['text'].splitlines()[0]}")
                if o.get("reason"):
                    print(f"{'':17}{o['reason']}")
        else:
            print(json.dumps(ask(json.loads(sys.stdin.read() if args.request == "-" else args.request)), ensure_ascii=False))
    except RuntimeError as error:
        print(str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
