"""Engines in C++ for any work (ADR 0102): a program around one of the API's compositions, built once and asked by scripts.

A work, or a plug-in of a work, names its engine by a source file (a ``main.cpp`` that includes ``science/<composition>.hpp``) and the
directory it is built into. Building compiles it with g++ 11 or later, natively where there is one and otherwise in WSL, against
every foundation's and domain's include directory and the API's own, and keeps beside the program a ``build.json`` holding the
hash of every file it was built from (its source and the headers it includes, followed transitively), so that work elsewhere in
the repository does not make it stale. Under WSL the build takes the lock the sessions on this repository share
(``%TEMP%/science-wsl.lock``, a directory created atomically, with an ``owner`` file), so that two compilations do not run at once.

An engine answers one JSON request on its input with one JSON line, ``{"ok": true, "result": …}`` or ``{"ok": false, "error": …}``;
``ask`` runs it once per request, so that a request that fails harms no one.

    from science.engines import Engine
    e = Engine("urban-studio/urban-forest", source=Path(".../engine/main.cpp"))
    e.build()                         # compiles if stale; True when current
    e.ask("forest", trees=..., scheme=...)

Market Studio, the Design Studio and the other works keep builders of their own, written before this one; they can move onto it.
"""

from __future__ import annotations

import contextlib
import hashlib
import json
import os
import re
import shutil
import subprocess
import tempfile
import threading
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
ENGINES = ROOT / "data" / "cache" / "engines"
# WSL starts few processes well at once: a page that asks several things together would otherwise start an engine for each, and WSL
# can then fail to start them and stop answering. So at most this many engines run in WSL at a time, the rest waiting their turn.
WSL_ASKS = threading.BoundedSemaphore(max(1, int(os.environ.get("NOETARIUM_WSL_ENGINES", "3"))))
PRODUCTS = ROOT / "data" / "cache" / "products"
INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)
PATTERNS = ("foundations/morphe/include", "foundations/*/*/include", "foundations/calx/numerics/*/include", "foundations/calx/compute/*/include",
            "foundations/calx/compute/backends/*/include", "domains/*/*/include")


# Each engine's sources with their times, and its digest, kept between requests by what it is built from (Engine.sources).
_KEPT: dict[tuple, list] = {}
_DIGESTS: dict[tuple, tuple] = {}


class EngineError(RuntimeError):
    """The engine is not built, did not answer, or refused the request (with its reason)."""


def include_dirs(extra: list[Path] | None = None) -> list[Path]:
    dirs: list[Path] = []
    for pattern in PATTERNS:
        dirs += sorted(p for p in ROOT.glob(pattern) if p.is_dir())
    dirs += [ROOT / "api" / "cpp"] + list(extra or [])
    seen, out = set(), []
    for d in dirs:
        if d.is_dir() and d not in seen:
            seen.add(d)
            out.append(d)
    return out


def wsl_path(p: Path) -> str:
    s = str(Path(p).resolve())
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


@contextlib.contextmanager
def wsl_lock(kind: str, owner: str, log=print, wait: float = 1800):
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
                holder = (lock / "owner").read_text(encoding="utf-8", errors="replace").strip() if (lock / "owner").exists() else "another session"
                log(f"Waiting for WSL, held by {holder}.")
                told = True
            if time.time() - t0 > wait:
                raise EngineError("WSL stayed busy; try again later")
            time.sleep(5)
    try:
        (lock / "owner").write_text(f"{owner} build {time.strftime('%Y-%m-%dT%H:%M:%S%z')}", encoding="utf-8")
        yield
    finally:
        shutil.rmtree(lock, ignore_errors=True)


class Engine:
    """A program built from `source` into data/cache/engines/<name>/engine."""

    def __init__(self, name: str, source: Path | None = None, extra_includes: list[Path] | None = None, env: str | None = None):
        self.name = name
        self.source = Path(source) if source else Path()   # none: an engine only asked, built by its work
        self.extra = ([self.source.parent] if source else []) + list(extra_includes or [])
        self.out = ENGINES / name
        named = os.environ.get(env) if env else None
        self.program = Path(named) if named else self.out / "engine"

    # ---- what it is built from -----------------------------------------------------------------------------------------------------

    def sources(self) -> list[Path]:
        """The files the engine is built from: its source and every header it includes, followed transitively. Resolving each include
        against every include folder costs many thousands of lookups for an engine on several foundations, which on Windows takes
        tens of seconds; so the list is kept, with each file's time, and resolved again only when one of those files has changed or
        gone (ADR 0120). A header added elsewhere changes nothing until a file that includes it changes."""
        kept = _KEPT.get(self._key())
        if kept is not None:
            try:
                if all(f.stat().st_mtime_ns == t for f, t in kept):
                    return [f for f, _ in kept]
            except OSError:
                pass
        found = self._resolve()
        _KEPT[self._key()] = [(f, f.stat().st_mtime_ns) for f in found]
        return found

    def _key(self) -> tuple:
        # Kept by what the engine is built from, not by this object: a work may make its engines afresh at each request.
        return (str(self.source), tuple(str(d) for d in self.extra))

    def _resolve(self) -> list[Path]:
        dirs = include_dirs(self.extra)
        seen: set[Path] = set()
        shared: dict[str, Path | None] = {}   # an include's place among the include folders, looked up once: <vector> is in none of them
        todo = [self.source.resolve()] if self.source.is_file() else []   # an engine only asked has no source here
        while todo:
            f = todo.pop()
            if f in seen:
                continue
            seen.add(f)
            for inc in INCLUDE.findall(f.read_text(encoding="utf-8", errors="replace")):
                near = f.parent / inc
                if near.is_file():
                    todo.append(near.resolve())
                    continue
                if inc not in shared:
                    shared[inc] = next(((d / inc).resolve() for d in dirs if (d / inc).is_file()), None)
                if shared[inc] is not None:
                    todo.append(shared[inc])
        return sorted(seen)

    def digest(self) -> str:
        files = self.sources()
        stamp = [(f, f.stat().st_mtime_ns) for f in files]
        kept = _DIGESTS.get(self._key())
        if kept and kept[0] == stamp:
            return kept[1]
        h = hashlib.sha256()
        for f in files:
            try:
                h.update(f.relative_to(ROOT).as_posix().encode())
            except ValueError:
                h.update(f.as_posix().encode())
            h.update(b"\0")
            h.update(f.read_bytes())
        _DIGESTS[self._key()] = (stamp, h.hexdigest())
        return _DIGESTS[self._key()][1]

    def record(self) -> dict | None:
        try:
            return json.loads((self.out / "build.json").read_text(encoding="utf-8"))
        except (OSError, ValueError):
            return None

    def status(self) -> dict:
        b = self.record()
        built = b is not None and self.program.exists()
        return {"name": self.name, "built": built, "toolchain": b.get("toolchain") if b else None, "built_at": b.get("built_at") if b else None,
                "current": bool(built and b.get("sources") == self.digest())}

    def build(self, force: bool = False, log=print) -> bool:
        if not force and self.status()["current"]:
            log(f"The engine {self.name} is current.")
            return True
        kind = toolchain()
        if kind is None:
            log("No C++ compiler: install g++ 11 or later (on Windows, in WSL).")
            return False
        self.out.mkdir(parents=True, exist_ok=True)
        stamp = self.digest()
        path = (lambda p: wsl_path(p)) if kind == "wsl" else (lambda p: str(p))
        args = ["g++", "-std=c++20", "-O2", "-DNDEBUG", "-pthread"]
        args += [f"-I{path(d)}" for d in include_dirs(self.extra)]
        args += [path(self.source), "-o", path(self.out / "engine")]
        log(f"Compiling the engine {self.name} with g++ ({kind}); a minute or more.")
        t0 = time.time()
        with wsl_lock(kind, self.name, log):
            r = subprocess.run((["wsl", "--exec"] if kind == "wsl" else []) + args, capture_output=True, text=True, encoding="utf-8", errors="replace")
        if r.returncode != 0:
            log(r.stderr[-6000:])
            log(f"The engine {self.name} did not compile.")
            return False
        rec = {"sources": stamp, "toolchain": kind, "built_at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()), "seconds": round(time.time() - t0, 1)}
        (self.out / "build.json").write_text(json.dumps(rec, indent=2) + "\n", encoding="utf-8")
        log(f"Built in {rec['seconds']} s: {self.out / 'engine'}")
        return True

    # ---- asking -----------------------------------------------------------------------------------------------------------------

    def path(self, p) -> str:
        """A file's path as the engine reads it: in WSL's own form when the engine was built there (ADR 0120)."""
        kind = (self.record() or {}).get("toolchain") or "native"
        return wsl_path(Path(p)) if kind == "wsl" else str(Path(p).resolve())

    def _args(self) -> tuple[list[str], str]:
        kind = (self.record() or {}).get("toolchain") or "native"
        args = [wsl_path(self.program) if kind == "wsl" else str(self.program), "--data", wsl_path(PRODUCTS) if kind == "wsl" else str(PRODUCTS)]
        if kind == "wsl":
            if not shutil.which("wsl"):
                raise EngineError("the engine was built in WSL, which is not here")
            args = ["wsl", "--exec", *args]
        return args, kind

    def keep(self, workers: int = 2) -> "Engine":
        """Keep this many of the engine's processes running (each started with --serve, a request a line), so that a server's requests
        do not each start a process: in WSL each start costs a third of a second, and a start now and then never returns (ADR 0120). A
        process that dies, or does not answer in time, is replaced; a rebuilt engine replaces them all."""
        self._pool = {"size": max(1, workers), "free": [], "all": [], "lock": threading.Lock(), "slots": threading.BoundedSemaphore(max(1, workers)), "built": None}
        return self

    def _worker(self):
        pool = self._pool
        stamp = self.program.stat().st_mtime_ns
        with pool["lock"]:
            if pool["built"] != stamp:   # rebuilt since they started: they all go
                for w in pool["all"]:
                    w.kill()
                pool["free"], pool["all"], pool["built"] = [], [], stamp
            while pool["free"]:
                w = pool["free"].pop()
                if w.poll() is None:
                    return w
                pool["all"].remove(w)
        args, _ = self._args()
        w = subprocess.Popen([*args, "--serve"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        with pool["lock"]:
            pool["all"].append(w)
        return w

    def _ask_kept(self, payload: bytes, timeout: float) -> str:
        pool = self._pool
        with pool["slots"]:
            for attempt in range(2):
                w = self._worker()
                watchdog = threading.Timer(timeout, w.kill)
                watchdog.start()
                try:
                    w.stdin.write(payload.replace(b"\n", b" ") + b"\n")
                    w.stdin.flush()
                    line = w.stdout.readline()
                except OSError:
                    line = b""
                finally:
                    watchdog.cancel()
                if line.strip():
                    with pool["lock"]:
                        pool["free"].append(w)
                    return line.decode("utf-8", errors="replace")
                w.kill()   # it died or stalled: another is started for a second asking
                with pool["lock"]:
                    if w in pool["all"]:
                        pool["all"].remove(w)
            raise EngineError(f"the engine {self.name} gave no answer from its kept processes")

    def ask(self, op: str, timeout: float = 600.0, **request) -> dict:
        """One request to the engine: its answer, or EngineError with the engine's reason."""
        if not self.program.exists():
            raise EngineError(f"the engine {self.name} is not built ({self.program}): run the work's recipe")
        args, kind = self._args()
        payload = json.dumps(clean({"op": op, **request}), allow_nan=False).encode("utf-8")
        if getattr(self, "_pool", None):
            text = self._ask_kept(payload, timeout)
            try:
                answer = json.loads(text)
            except ValueError as error:
                raise EngineError(text.strip()[-400:] or "the engine gave no answer") from error
            if not answer.get("ok"):
                raise EngineError(str(answer.get("error", "the engine failed")))
            return answer["result"]
        try:
            for attempt in range(3):
                with WSL_ASKS if kind == "wsl" else contextlib.nullcontext():
                    raw = subprocess.run(args, input=payload, capture_output=True, timeout=timeout)
                # The engine answers in UTF-8; what WSL says when it cannot start it is in the system's code page.
                out, err = raw.stdout.decode("utf-8", errors="replace"), _said(raw.stderr)
                # WSL sometimes fails to start a process at all (Wsl/Service/E_UNEXPECTED) and then does on a second asking.
                if kind == "wsl" and not out.strip() and "Wsl/" in err and attempt < 2:
                    time.sleep(1.0 + attempt)
                    continue
                break
            done = subprocess.CompletedProcess(args, raw.returncode, out, err)
        except subprocess.TimeoutExpired as error:
            raise EngineError(f"the engine {self.name} did not answer within {timeout:.0f} s") from error
        except OSError as error:
            raise EngineError(f"the engine {self.name} could not be started: {error}") from error
        try:
            answer = json.loads(done.stdout or "")
        except ValueError as error:
            raise EngineError((done.stderr or done.stdout or "the engine gave no answer").strip()[-400:]) from error
        if not answer.get("ok"):
            raise EngineError(str(answer.get("error", "the engine failed")))
        return answer["result"]


def _said(data: bytes) -> str:
    """What a program wrote on its error stream: UTF-8 if it is, else the system's code page (wsl.exe's messages on Windows)."""
    try:
        return data.decode("utf-8")
    except UnicodeDecodeError:
        try:
            return data.decode("mbcs", errors="replace") if os.name == "nt" else data.decode("latin-1")
        except LookupError:
            return data.decode("latin-1")


def clean(value):
    """A request as plain JSON: numpy's arrays and numbers as lists and numbers, NaN and infinities as null."""
    if isinstance(value, dict):
        return {str(k): clean(v) for k, v in value.items()}
    if isinstance(value, (list, tuple)):
        return [clean(v) for v in value]
    if isinstance(value, float):
        return value if value == value and value not in (float("inf"), float("-inf")) else None
    try:
        import numpy as np
        if isinstance(value, np.ndarray):
            return clean(value.tolist())
        if isinstance(value, np.generic):
            return clean(value.item())
    except ImportError:
        pass
    return value
