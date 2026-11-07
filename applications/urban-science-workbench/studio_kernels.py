"""The Studio's kernels (the design note of 2026-11-07, §8.7): processes the server keeps running, which hold the systems of the projects'
Python and C++ scripts between ticks, so that a script created or changed while a project plays joins the running schedule at once.

- PythonKernel: a CPython (kernels/python_kernel.py) speaking science.studio's protocol, a request a line.
- CppKernel: a host program (kernels/cpp_host.cpp) built once, which loads each script as a shared library and swaps it when the
  script is compiled again. A script is compiled alone, against only the include directories its headers are found in (ADR 0079's
  fault: every directory searched for every header across WSL's mount took minutes).

Each kernel answers one request at a time; a kernel that stops (a script that crashed) is started again at the next request, and its
scripts must be loaded again, which the answer says.

    python applications/urban-science-workbench/studio_kernels.py check
"""

from __future__ import annotations

import hashlib
import json
import queue
import re
import subprocess
import sys
import threading
import time
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))
import engine  # noqa: E402
from science.networks import dirs_needed  # noqa: E402

OUT = engine.OUT / "studio"
HOST_SOURCE = APP / "kernels" / "cpp_host.cpp"
ERROR = re.compile(r"^(?P<file>[^:\n]+):(?P<line>\d+):(?P<col>\d+): (?:fatal )?error: (?P<message>.*)$", re.M)


class KernelError(Exception):
    def __init__(self, message: str, line: int | None = None, log: list | None = None):
        super().__init__(message)
        self.line = line
        self.log = log or []


class Process:
    """A process kept running, asked one request at a time, a JSON line each way; started again if it has ended."""

    def __init__(self, name: str, command: list[str]):
        self.name, self.command = name, command
        self.lock = threading.Lock()
        self.process: subprocess.Popen | None = None
        self.lines: queue.Queue = queue.Queue()
        self.restarted = 0

    def _start(self) -> None:
        self.process = subprocess.Popen(self.command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True,
                                        encoding="utf-8", bufsize=1)
        self.lines = queue.Queue()
        out, lines = self.process.stdout, self.lines

        def read() -> None:
            for line in out:
                lines.put(line)
            lines.put(None)
        threading.Thread(target=read, daemon=True).start()

    def alive(self) -> bool:
        return self.process is not None and self.process.poll() is None

    def ask(self, request: dict, timeout: float = 60) -> dict:
        with self.lock:
            fresh = False
            if not self.alive():
                if self.process is not None:
                    self.restarted += 1
                self._start()
                fresh = True
            try:
                self.process.stdin.write(json.dumps(request, ensure_ascii=False) + "\n")
                self.process.stdin.flush()
                line = self.lines.get(timeout=timeout)
            except queue.Empty:
                self.stop()
                raise KernelError(f"the {self.name} kernel did not answer within {timeout:.0f} s, and was stopped") from None
            except OSError:
                line = None
            if line is None:
                self.stop()
                raise KernelError(f"the {self.name} kernel stopped (a script crashed it, perhaps); its scripts are loaded again at their next run")
            answer = json.loads(line)
            if fresh:
                answer["fresh"] = True
            return answer

    def stop(self) -> None:
        if self.process is not None:
            try:
                self.process.kill()
            except Exception:
                pass
        self.process = None


def unwrap(answer: dict) -> dict:
    if not answer.get("ok"):
        raise KernelError(str(answer.get("error", "the script failed")), answer.get("line"), answer.get("log"))
    return answer["result"]


class PythonKernel:
    def __init__(self):
        self.process = Process("Python", [sys.executable, "-I", "-u", str(APP / "kernels" / "python_kernel.py")])

    def load(self, id: str, source_path: Path) -> dict:
        return unwrap(self.process.ask({"op": "load", "id": id, "source": source_path.read_text(encoding="utf-8")}, timeout=60))

    def tick(self, id: str, request: dict) -> dict:
        return unwrap(self.process.ask({**request, "op": "tick", "id": id}, timeout=30))

    def unload(self, id: str) -> dict:
        return unwrap(self.process.ask({"op": "unload", "id": id}, timeout=30)) if self.process.alive() else {}

    def status(self) -> dict:
        return {"alive": self.process.alive(), "restarted": self.process.restarted}


class CppKernel:
    def __init__(self):
        self.kind = None
        self.process: Process | None = None
        self.build_lock = threading.Lock()

    def _kind(self) -> str:
        if self.kind is None:
            self.kind = engine.toolchain()
            if self.kind is None:
                raise KernelError("no C++ compiler: install g++ 11 or later (on Windows, in WSL)")
        return self.kind

    def _host(self) -> Path:
        """The host program, built when its source or the JSON header changed."""
        kind = self._kind()
        stamp = hashlib.sha256(HOST_SOURCE.read_bytes() + (ROOT / "api" / "cpp" / "science" / "json.hpp").read_bytes()).hexdigest()[:16]
        program = OUT / f"host-{stamp}"
        if not program.exists():
            OUT.mkdir(parents=True, exist_ok=True)
            args = ["g++", "-std=c++20", "-O2", "-pthread", f"-I{engine.path_for(kind, ROOT / 'api' / 'cpp')}", engine.path_for(kind, HOST_SOURCE), "-o",
                    engine.path_for(kind, program), "-ldl"]
            r = subprocess.run(engine.command(kind, args), capture_output=True, text=True, encoding="utf-8", errors="replace")
            if r.returncode != 0:
                raise KernelError("the C++ host did not compile:\n" + r.stderr[-3000:])
        return program

    def _process(self) -> Process:
        host = self._host()
        command = engine.command(self.kind, [engine.path_for(self.kind, host)])
        if self.process is None or self.process.command != command:
            if self.process is not None:
                self.process.stop()
            self.process = Process("C++", command)
        return self.process

    def compile(self, id: str, source_path: Path) -> tuple[Path, float]:
        """The script compiled alone as a shared library, named by its content, against the include directories it needs."""
        kind = self._kind()
        dirs = [ROOT / "api" / "cpp"] + [d for d in engine.include_dirs("studio") if d != ROOT / "api" / "cpp"]
        needed = dirs_needed(source_path, dirs)
        if ROOT / "api" / "cpp" not in needed:
            needed.insert(0, ROOT / "api" / "cpp")
        text = source_path.read_bytes()
        stamp = hashlib.sha256(text + "|".join(str(d) for d in needed).encode()).hexdigest()[:16]
        safe = re.sub(r"[^A-Za-z0-9_-]", "_", id)
        library = OUT / "scripts" / f"{safe}-{stamp}.so"
        if library.exists():
            return library, 0.0
        library.parent.mkdir(parents=True, exist_ok=True)
        args = ["g++", "-std=c++20", "-O2", "-shared", "-fPIC", *[f"-I{engine.path_for(kind, d)}" for d in needed], engine.path_for(kind, source_path), "-o",
                engine.path_for(kind, library)]
        t0 = time.time()
        with self.build_lock:
            r = subprocess.run(engine.command(kind, args), capture_output=True, text=True, encoding="utf-8", errors="replace")
        seconds = time.time() - t0
        if r.returncode != 0:
            errors = [m for m in ERROR.finditer(r.stderr) if Path(m.group("file").replace("\\", "/")).name == source_path.name]
            first = errors[0] if errors else None
            message = (first.group("message") if first else r.stderr.strip().splitlines()[-1] if r.stderr.strip() else "the compiler failed")
            raise KernelError(f"did not compile: {message}", int(first.group("line")) if first else None, r.stderr.strip().splitlines()[-24:])
        return library, seconds

    def load(self, id: str, source_path: Path) -> dict:
        library, seconds = self.compile(id, source_path)
        result = unwrap(self._process().ask({"op": "load", "id": id, "path": engine.path_for(self.kind, library)}, timeout=60))
        result["compile_s"] = round(seconds, 2)
        return result

    def tick(self, id: str, request: dict) -> dict:
        if self.process is None:
            raise KernelError(f"no system {id} is loaded")
        return unwrap(self.process.ask({**request, "op": "tick", "id": id}, timeout=30))

    def unload(self, id: str) -> dict:
        return unwrap(self.process.ask({"op": "unload", "id": id}, timeout=30)) if self.process is not None and self.process.alive() else {}

    def status(self) -> dict:
        return {"alive": bool(self.process and self.process.alive()), "restarted": self.process.restarted if self.process else 0, "toolchain": self.kind}


class Kernels:
    """The Studio's kernels, one for each language, its scripts named by project and script."""

    def __init__(self):
        self.python = PythonKernel()
        self.cpp = CppKernel()

    def of(self, language: str):
        if language == "python":
            return self.python
        if language == "cpp":
            return self.cpp
        raise KernelError(f"no kernel for {language}: JavaScript runs in the page's worker")

    def stop(self) -> None:
        self.python.process.stop()
        if self.cpp.process is not None:
            self.cpp.process.stop()


if __name__ == "__main__" and sys.argv[1:2] == ["check"]:
    k = Kernels()
    print(k.python.status(), k.cpp.status())
