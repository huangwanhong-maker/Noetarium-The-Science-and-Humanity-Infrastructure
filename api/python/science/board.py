"""Modules of a board (visualization/js/board): scripts in Python that are given what stands on a board, its tables, the quantities its notes
define and the notes themselves, and answer tables and notes of their own, which the board keeps beside the thought they serve: a map or a chart
on the board draws a table by its name, whoever made it. They belong to no application: a work binds a `Host` to a directory of modules, as the
Urban Science Workbench binds spacetime.py.

A table is the data library's (data/products/economic_tables.py): named columns of equal length, each of numbers (null where missing) or of text,

    {"name", "title", "source", "rows", "columns": [{"name", "kind": "number" | "text", "label"?}], "data": {column: [values]}}

and a place is columns: `lat` and `lon` in degrees on WGS 84 for points, `iso3` for economies. A module gets data where the data library keeps
them (cache(), library()), and may use whatever its Python has: numpy always, scikit-learn or torch where they are installed.

A module is a script that exports

    def create(api):
        class Mine(api.BoardAnalysis):
            id = "mine"; title = "..."; description = "..."
            params = [api.param("table", "Table", "customers", kind="text", role="table"),
                      api.param("k", "Segments", 3, kind="integer", minimum=2, maximum=12)]
            def run(self, board, params, context):
                t = board.table(params["table"])
                x = t.column("spend")                       # floats, NaN where missing
                return [api.table("spenders", {"name": t.text("name"), "spend": x}), api.note("…")]
        return Mine()

or, for a few lines written on a board itself, only

    def run(board, api):
        return [api.note(f"{len(board.tables)} tables")]

where `api` is this module. A parameter of kind "text" may say what it names by `role`: "table", "column" or "columns" (of the table named by
the parameter `of`), "quantity", or a "file" (a path on this computer, `data:<dataset>/<file>` of the data library's cache, or `kept:<name>` of what the
work keeps; file() finds it); the board offers what it has. Beside tables and notes a module may answer a figure it has drawn (figure()). Modules run as processes behind networks.py's file contract, so that one can
neither stop the server nor outlive its time: input.json, {"board": {"tables", "quantities", "notes"}, "given": {params}}, and output.json,
{"results"}.

    python -m science.board describe <script>
    python -m science.board run <script> <dir>
"""

from __future__ import annotations

import importlib.util
import json
import math
import re
import sys
import traceback
from pathlib import Path

import numpy as np

from .networks import Context, plain, say_progress, typed
from .networks import Host as _ProcessHost
from .networks import param as _param
from .plugins import note

__all__ = ["Board", "Table", "Quantity", "BoardAnalysis", "Host", "table", "note", "figure", "param", "cache", "file", "library", "read_table", "read_delimited", "Context"]

ROOT = Path(__file__).resolve().parents[3]
CACHE = ROOT / "data" / "cache"
NAME = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
MOST_CELLS = 2_000_000          # of the tables a run answers: a board is kept as one file
MOST_FIGURE = 4 * 1024 * 1024   # bytes of one figure
ROLES = ("table", "column", "columns", "quantity", "file")


# ---- what a module is given -----------------------------------------------------------------------------------------------------------


class Table:
    """A table of a board: its columns by name, as floats (column) or as text (text)."""

    def __init__(self, name: str, data: dict):
        self.name = name
        self.title = str(data.get("title") or "")
        self.source = str(data.get("source") or "")
        described = data.get("columns") or [{"name": k} for k in (data.get("data") or {})]
        self.columns = [c["name"] for c in described]
        self.kinds = {c["name"]: c.get("kind", "text") for c in described}
        self.labels = {c["name"]: c["label"] for c in described if c.get("label")}
        self._data = data.get("data") or {}
        lengths = {len(self._data.get(c, [])) for c in self.columns}
        if len(lengths) > 1:
            raise ValueError(f"the columns of the table {name!r} differ in length")
        self.rows = lengths.pop() if lengths else 0

    def _need(self, column: str) -> list:
        if column not in self._data:
            raise LookupError(f"the table {self.name!r} has no column {column!r} (it has: {', '.join(self.columns) or 'none'})")
        return self._data[column]

    def has(self, column: str) -> bool:
        return column in self._data

    def raw(self, column: str) -> list:
        return list(self._need(column))

    def column(self, column: str) -> np.ndarray:
        """A column as floats: NaN where a value is missing or is not a number."""
        def number(v) -> float:
            if isinstance(v, bool) or v is None:
                return math.nan
            if isinstance(v, (int, float)):
                return float(v)
            try:
                return float(str(v).replace(",", ""))
            except ValueError:
                return math.nan
        return np.array([number(v) for v in self._need(column)], dtype=float)

    def text(self, column: str) -> list:
        return ["" if v is None else str(v) for v in self._need(column)]

    def numeric(self) -> list:
        return [c for c in self.columns if self.kinds.get(c) == "number"]

    def __repr__(self) -> str:
        return f"<Table {self.name}: {self.rows} rows of {', '.join(self.columns)}>"


class Quantity:
    """A quantity a note or a figure of the board defines: its likely value within the range it is believed to lie in."""

    def __init__(self, name: str, data: dict):
        self.name, self.lo, self.mid, self.hi, self.unit = name, float(data["lo"]), float(data["mid"]), float(data["hi"]), str(data.get("unit") or "")

    def __float__(self) -> float:
        return self.mid

    def __repr__(self) -> str:
        return f"<Quantity {self.name} = {self.mid:g} [{self.lo:g}, {self.hi:g}] {self.unit}>".replace(" >", ">")


class Board:
    """What stands on a board, as a module sees it: tables and quantities by name, and the notes."""

    def __init__(self, data: dict | None = None):
        data = data or {}
        self.tables = {name: Table(name, t) for name, t in (data.get("tables") or {}).items()}
        self.quantities = {name: Quantity(name, q) for name, q in (data.get("quantities") or {}).items()}
        self.notes = list(data.get("notes") or [])

    def table(self, name: str) -> Table:
        if name not in self.tables:
            raise LookupError(f"there is no table {name!r} on the board (it has: {', '.join(self.tables) or 'none'})")
        return self.tables[name]

    def quantity(self, name: str, default=None) -> Quantity:
        if name not in self.quantities:
            if default is not None:
                return Quantity(name, {"lo": default, "mid": default, "hi": default})
            raise LookupError(f"no note of the board defines {name!r}: write a line such as  {name} = 3% [2%, 5%]  on a note"
                              + (f" (defined: {', '.join(self.quantities)})" if self.quantities else ""))
        return self.quantities[name]


class BoardAnalysis:
    """A module of a board: identity, parameters, and run(), which answers tables and notes."""

    id = ""
    title = ""
    description = ""
    params: list = []

    def describe(self) -> dict:
        ident = self.id or type(self).__name__.lower()
        return {"kind": "board-analysis", "id": ident, "title": self.title or ident, "description": self.description, "params": list(self.params), "language": "python"}

    def run(self, board: Board, params: dict, context: Context) -> list:
        raise NotImplementedError


class _Plain(BoardAnalysis):
    """A script that only defines run(board, api)."""

    def __init__(self, module, name: str):
        self._run = module.run
        self.id = name
        self.title = (module.__doc__ or name).strip().splitlines()[0]

    def run(self, board, params, context):
        from science import board as api
        return self._run(board, api)


# ---- what a module answers ------------------------------------------------------------------------------------------------------------


def param(name: str, label: str, default, kind: str = "number", role: str | None = None, of: str | None = None, **more) -> dict:
    """A parameter, as networks.param; a text one may say what it names: a table, a column or columns of the table named by the parameter `of`,
    a quantity. The board then offers what it has."""
    out = _param(name, label, default, kind=kind, **more)
    if role is not None:
        if kind != "text" or role not in ROLES:
            raise ValueError(f"a role is one of {', '.join(ROLES)}, of a text parameter")
        out["role"] = role
        if of:
            out["of"] = of
    return out


def table(name: str, columns: dict, title: str = "", source: str = "", labels: dict | None = None) -> dict:
    """A table for the board, under a name other items draw it by: columns {name: values}, of numbers (NaN or None where missing) or of text."""
    if not NAME.match(str(name)):
        raise ValueError(f"a table's name is letters, digits and _, not beginning with a digit: {name!r}")
    data, described = {}, []
    for column, values in columns.items():
        values = plain(values if isinstance(values, (list, tuple, np.ndarray)) else list(values))
        numbers = all(v is None or (isinstance(v, (int, float)) and not isinstance(v, bool)) for v in values)
        data[str(column)] = values if numbers else [None if v is None else str(v) for v in values]
        described.append({"name": str(column), "kind": "number" if numbers else "text", **({"label": labels[column]} if labels and column in labels else {})})
    lengths = {len(v) for v in data.values()}
    if len(lengths) > 1:
        raise ValueError(f"the columns of the table {name!r} differ in length: {', '.join(f'{k} {len(v)}' for k, v in data.items())}")
    return {"kind": "table", "name": str(name), "title": title, "source": source, "rows": lengths.pop() if lengths else 0, "columns": described, "data": data}


def figure(drawing, title: str = "") -> dict:
    """A figure for the board: SVG as text, PNG as bytes, or anything with savefig (a matplotlib figure), which is written as SVG."""
    import base64
    import io
    if hasattr(drawing, "savefig"):
        buffer = io.StringIO()
        drawing.savefig(buffer, format="svg", bbox_inches="tight")
        drawing = buffer.getvalue()
    if isinstance(drawing, (bytes, bytearray)):
        if not bytes(drawing).startswith(b"\x89PNG"):
            raise ValueError("a figure given as bytes is a PNG")
        out = {"kind": "figure", "title": title, "format": "png", "data": base64.b64encode(bytes(drawing)).decode("ascii")}
    elif isinstance(drawing, str) and "<svg" in drawing[:2000]:
        out = {"kind": "figure", "title": title, "format": "svg", "data": drawing}
    else:
        raise ValueError("a figure is SVG text, PNG bytes or a matplotlib figure")
    if len(out["data"]) > MOST_FIGURE:
        raise ValueError(f"the figure is {len(out['data']):,} bytes: a board keeps at most {MOST_FIGURE:,} of one figure")
    return out


def finish(results) -> list:
    """The results of a run, checked: each a table, a note or a figure, tables named apart, and no more cells than a board can keep."""
    if isinstance(results, dict):
        results = [results]
    out, names, cells = [], set(), 0
    for r in results or []:
        if not isinstance(r, dict) or r.get("kind") not in ("table", "note", "figure"):
            raise ValueError(f"a result is a table (api.table), a note (api.note) or a figure (api.figure): {str(r)[:80]}")
        if r["kind"] == "table":
            if r["name"] in names:
                raise ValueError(f"two tables answered are named {r['name']!r}")
            names.add(r["name"])
            if len({len(v) for v in r["data"].values()}) > 1:
                raise ValueError(f"the columns of the table {r['name']!r} differ in length")
            cells += r["rows"] * len(r["columns"])
        out.append(plain(r))
    if cells > MOST_CELLS:
        raise ValueError(f"the tables answered hold {cells:,} cells: a board keeps at most {MOST_CELLS:,} from one run (answer fewer rows or columns)")
    return out


# ---- data, where the data library keeps them ------------------------------------------------------------------------------------------


def cache(*parts: str) -> Path:
    """A path within the data library's cache (data/cache), refused when it leaves it or is not there."""
    path = CACHE.joinpath(*parts).resolve()
    if path != CACHE and CACHE.resolve() not in path.parents:
        raise ValueError(f"{'/'.join(parts)} is not within the data library's cache")
    if not path.exists():
        raise FileNotFoundError(f"{'/'.join(parts)} is not in the data library's cache: fetch the dataset '{parts[0]}' first (python data/acquisition/fetch.py {parts[0]})")
    return path


def file(ref: str) -> Path:
    """The file a parameter of the role "file" names: `data:<dataset>/<file>` within the data library's cache, `kept:<name>` among what the work
    keeps (its directory is in NOETARIUM_BOARD_FILES), or a path on this computer."""
    import os
    ref = str(ref or "").strip()
    if not ref:
        raise ValueError("no file is named: set the parameter in the panel")
    if ref.startswith("data:"):
        return cache(*[part for part in ref[5:].split("/") if part])
    if ref.startswith("kept:"):
        kept = os.environ.get("NOETARIUM_BOARD_FILES")
        if not kept:
            raise ValueError("this work keeps no files")
        path = (Path(kept) / ref[5:]).resolve()
        if Path(kept).resolve() not in path.parents or not path.is_file():
            raise FileNotFoundError(f"{ref} is not among the files the work keeps")
        return path
    path = Path(ref).expanduser()
    if not path.is_file():
        raise FileNotFoundError(f"there is no file {ref}")
    return path


def read_delimited(path: Path, name: str = "table") -> Table:
    """A file of rows and columns (commas, semicolons or tabs between, its first row the names) as a table: a column is of numbers when every
    field of it that is not empty is one."""
    import csv
    text = Path(path).read_text(encoding="utf-8-sig", errors="replace")
    try:
        dialect = csv.Sniffer().sniff(text[:20000], delimiters=",;\t")
    except csv.Error:
        dialect = csv.excel
    rows = [r for r in csv.reader(text.splitlines(), dialect) if any(f.strip() for f in r)]
    if len(rows) < 2:
        raise ValueError(f"{Path(path).name} has no rows beneath its names")
    names, columns, described = [n.strip() or f"column_{k + 1}" for k, n in enumerate(rows[0])], {}, []
    for k, column in enumerate(names):
        fields = [(r[k].strip() if k < len(r) else "") for r in rows[1:]]

        def number(f: str):
            try:
                return float(f.replace(",", "")) if f else None
            except ValueError:
                return math.nan
        numbers = [number(f) for f in fields]
        numeric = any(v is not None for v in numbers) and not any(v is not None and math.isnan(v) for v in numbers)
        columns[column] = numbers if numeric else [f or None for f in fields]
        described.append({"name": column, "kind": "number" if numeric else "text"})
    return Table(name, {"title": Path(path).name, "columns": described, "data": columns})


def read_table(path: Path, name: str | None = None) -> Table:
    """A table of the data library's schema, read from a JSON file."""
    data = json.loads(Path(path).read_text(encoding="utf-8"))
    return Table(name or str(data.get("id") or Path(path).stem).replace("-", "_"), data)


def library(ident: str) -> Table:
    """A table of the data library by its id (economies, currencies, currency-rates), made again when its sources have changed."""
    products = str(ROOT / "data" / "products")
    if products not in sys.path:
        sys.path.insert(0, products)
    import economic_tables
    for t in economic_tables.library():
        if t.get("id") == ident:
            return Table(ident.replace("-", "_"), t)
    raise FileNotFoundError(f"the data library has no table {ident!r} yet: python data/products/economic_tables.py makes its tables")


# ---- the runner: a script in a process of its own -------------------------------------------------------------------------------------


def load(script: Path):
    """The module a script makes: its create(api), or its run(board, api) alone."""
    script = Path(script)
    spec = importlib.util.spec_from_file_location(f"board_module_{re.sub(r'[^A-Za-z0-9_]', '_', script.stem)}", script)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    from science import board as api
    if hasattr(module, "create"):
        plugin = module.create(api)
        if not hasattr(plugin, "run") or not hasattr(plugin, "describe"):
            raise ValueError(f"{script.name}: create(api) answers no module (an instance of api.BoardAnalysis)")
        return plugin
    if hasattr(module, "run"):
        return _Plain(module, script.stem)
    raise ValueError(f"{script.name} defines neither create(api) nor run(board, api)")


def run_script(script: Path, work: Path) -> None:
    request = json.loads((Path(work) / "input.json").read_text(encoding="utf-8"))
    plugin = load(script)
    params = typed(plugin.describe().get("params", []), request.get("given"))
    results = finish(plugin.run(Board(request.get("board")), params, Context(say_progress)))
    (Path(work) / "output.json").write_text(json.dumps({"results": results}, ensure_ascii=False, allow_nan=False), encoding="utf-8")


class Host(_ProcessHost):
    """The modules of one directory, listed, described and run, as networks.py's Host, on what stands on a board."""
    module = "science.board"
    kind = "board-analysis"

    def request_of(self, subject: dict, chosen: dict) -> dict:
        return {"board": plain(subject or {}), "given": chosen}


def main(argv: list[str]) -> int:
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8", errors="replace")
    if len(argv) >= 2 and argv[0] == "describe":
        print(json.dumps(plain(load(Path(argv[1])).describe()), ensure_ascii=False))
        return 0
    if len(argv) >= 3 and argv[0] == "run":
        try:
            run_script(Path(argv[1]), Path(argv[2]))
        except Exception as error:
            traceback.print_exc()
            print(f"{type(error).__name__}: {error}", file=sys.stderr, flush=True)
            return 1
        return 0
    print(__doc__.split("\n\n")[-1], file=sys.stderr)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
