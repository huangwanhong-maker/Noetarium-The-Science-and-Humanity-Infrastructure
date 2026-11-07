"""Scripts of the Studio in Python (the design note of 2026-11-07, §8; its kernels, §8.7): the same contract as JavaScript's and C++'s.

A script is a module with a function `create(api)` answering a system:

    def create(api):
        class Crowd(api.System):
            title = "A crowd"
            phase = "tick"
            reads, writes, previous = ["radius"], ["crowd"], ["crowd"]
            def tick(self, ctx, io):
                io.set("crowd", (io.prev("crowd") or 0) + 1)
            def draw(self, ctx, io):
                return [api.points("The crowd", lon=[144.96], lat=[-37.81], value=[1])]
        return Crowd()

`io.get(name)` the value of this tick, `io.prev(name)` the value at the end of the last, `io.set(name, v)` to write, `io.add(name, v)` to
contribute to a merged resource, `io.log(...)`. Marks are declared on WGS 84, in degrees and metres; the page draws them.

The kernel (`serve()`) keeps a process for the project: it loads a script's source when it changes, keeping its state where it can
(`save` and `load`), and answers ticks, one JSON request a line on its input, one answer a line on its output. A script's prints are
caught and given back with its answer, never mixed into the protocol.
"""

from __future__ import annotations

import io as _io
import json
import math
import sys
import traceback
import types
from contextlib import redirect_stdout


class System:
    """A system of a project: the scheduler orders it by what it reads and writes."""

    title = "A system"
    phase = "tick"
    reads: list = []
    writes: list = []
    previous: list = []
    after: list = []
    params: list = []
    resources: dict = {}

    def start(self, ctx, io):  # noqa: D401 - a hook
        pass

    def tick(self, ctx, io):
        pass

    def draw(self, ctx, io):
        return []

    def save(self):
        return None

    def load(self, state):
        pass


def _plain(v):
    """numpy arrays and scalars as plain lists and numbers, NaN as None, for JSON."""
    if hasattr(v, "tolist"):
        v = v.tolist()
    if isinstance(v, float):
        return v if math.isfinite(v) else None
    if isinstance(v, (list, tuple)):
        return [_plain(x) for x in v]
    if isinstance(v, dict):
        return {str(k): _plain(x) for k, x in v.items()}
    return v


def param(name, label, value, kind=None, minimum=None, maximum=None, step=None):
    k = kind or ("boolean" if isinstance(value, bool) else "text" if isinstance(value, str) else "number")
    return {"name": name, "label": label, "default": value, "kind": k, "min": minimum, "max": maximum, "step": step}


def points(title, lon, lat, height=0, value=None, size=6, colour=None, map="plum", domain=None):
    return {"kind": "points", "title": title, "lon": _plain(lon), "lat": _plain(lat), "height": _plain(height), "value": _plain(value),
            "size": size, "colour": colour, "map": map, "domain": _plain(domain)}


def columns(title, lon, lat, value, radius=60, top=300, map="plum", domain=None):
    return {"kind": "columns", "title": title, "lon": _plain(lon), "lat": _plain(lat), "value": _plain(value), "radius": radius, "top": top, "map": map,
            "domain": _plain(domain)}


def lines(title, paths, height=0, colour="#EE8FBD", value=None, map="plum", domain=None):
    return {"kind": "lines", "title": title, "paths": _plain(paths), "height": height, "colour": colour, "value": _plain(value), "map": map, "domain": _plain(domain)}


def describe(s: System) -> dict:
    return {"title": str(s.title), "phase": "update" if s.phase == "update" else "tick", "reads": list(s.reads), "writes": list(s.writes),
            "previous": list(s.previous), "after": list(s.after), "params": list(s.params), "resources": dict(s.resources)}


class Io:
    def __init__(self, values, previous):
        self.values, self.previous = values or {}, previous or {}
        self.writes, self.adds, self.lines = {}, {}, []

    def get(self, name):
        return self.values.get(name)

    def prev(self, name):
        return self.previous.get(name)

    def set(self, name, value):
        self.writes[name] = _plain(value)

    def add(self, name, value):
        self.adds.setdefault(name, []).append(_plain(value))

    def log(self, *a):
        self.lines.append(" ".join(x if isinstance(x, str) else json.dumps(_plain(x)) for x in a))


def chart(frame, chart="line", x=None, y=None, series=None, title="", xlabel="", ylabel=""):
    """A statistical figure for a frame of the canvas: "line", "bars" or "scatter", of x against y, or several series [{name, x, y}]."""
    return {"kind": "chart", "frame": frame, "chart": chart, "x": _plain(x), "y": _plain(y), "series": _plain(series), "title": title, "xlabel": xlabel, "ylabel": ylabel}


def drawing(frame, items):
    """A drawing of one's own for a frame: items in the frame's pixels, each {"type": "line" | "circle" | "rect" | "text" | "path", ...}."""
    return {"kind": "drawing", "frame": frame, "items": _plain(items)}


class _Api(types.SimpleNamespace):
    pass


API = _Api(System=System, param=param, points=points, columns=columns, lines=lines, chart=chart, drawing=drawing)


def _line_of(error: BaseException, filename: str) -> int | None:
    for frame in reversed(traceback.extract_tb(error.__traceback__)):
        if frame.filename == filename:
            return frame.lineno
    if isinstance(error, SyntaxError) and error.filename == filename:
        return error.lineno
    return None


class Kernel:
    """The systems of a project, kept between runs."""

    def __init__(self):
        self.systems: dict[str, dict] = {}

    def load(self, id: str, source: str) -> dict:
        filename = f"<studio {id}>"
        module = types.ModuleType(f"studio_{len(self.systems)}")
        module.__dict__["__file__"] = filename
        caught = _io.StringIO()
        try:
            with redirect_stdout(caught):
                exec(compile(source, filename, "exec"), module.__dict__)
                if "create" not in module.__dict__:
                    raise ValueError("a script defines create(api), answering its system")
                system = module.create(API)
        except Exception as error:
            raise ScriptError(f"{type(error).__name__}: {error}", _line_of(error, filename), caught.getvalue()) from None
        old = self.systems.get(id)
        carried = False
        if old is not None:
            try:
                state = old["system"].save()
                if state is not None:
                    system.load(state)
                    carried = True
            except Exception:
                carried = False
        self.systems[id] = {"system": system, "started": carried, "filename": filename}
        return {"describe": describe(system), "carried": carried, "log": [x for x in caught.getvalue().splitlines() if x]}

    def tick(self, id: str, request: dict) -> dict:
        entry = self.systems.get(id)
        if entry is None:
            raise ScriptError(f"no system {id} is loaded", None, "")
        ctx = types.SimpleNamespace(tick=request.get("tick", 0), dt=request.get("dt", 0.0), time_ms=request.get("time_ms", 0.0),
                                    params=request.get("params") or {}, view=request.get("view"), frames=request.get("frames") or {})
        io = Io(request.get("values"), request.get("previous"))
        caught = _io.StringIO()
        try:
            with redirect_stdout(caught):
                if not entry["started"]:
                    entry["started"] = True
                    entry["system"].start(ctx, io)
                entry["system"].tick(ctx, io)
                marks = entry["system"].draw(ctx, io) or []
        except Exception as error:
            raise ScriptError(f"{type(error).__name__}: {error}", _line_of(error, entry["filename"]), caught.getvalue()) from None
        log = io.lines + [x for x in caught.getvalue().splitlines() if x]
        return {"writes": io.writes, "adds": io.adds, "marks": _plain(marks), "log": log}

    def unload(self, id: str) -> dict:
        self.systems.pop(id, None)
        return {}


class ScriptError(Exception):
    def __init__(self, message: str, line: int | None, printed: str):
        super().__init__(message)
        self.line = line
        self.printed = printed


def serve() -> int:
    """The kernel's loop: one JSON request a line on its input, one answer a line on its output."""
    out = sys.stdout
    kernel = Kernel()
    for raw in sys.stdin:
        raw = raw.strip()
        if not raw:
            continue
        try:
            r = json.loads(raw)
            op, id = r.get("op"), r.get("id", "")
            if op == "load":
                result = kernel.load(id, r["source"])
            elif op == "tick":
                result = kernel.tick(id, r)
            elif op == "unload":
                result = kernel.unload(id)
            elif op == "status":
                result = {"systems": sorted(kernel.systems)}
            else:
                raise ScriptError(f"no operation {op}", None, "")
            reply = {"ok": True, "result": result}
        except ScriptError as error:
            reply = {"ok": False, "error": str(error), "line": error.line, "log": [x for x in error.printed.splitlines() if x]}
        except Exception as error:  # a fault of the kernel's own, reported, never fatal
            reply = {"ok": False, "error": f"{type(error).__name__}: {error}", "line": None, "log": []}
        out.write(json.dumps(reply, ensure_ascii=False, allow_nan=False, default=str) + "\n")
        out.flush()
    return 0
