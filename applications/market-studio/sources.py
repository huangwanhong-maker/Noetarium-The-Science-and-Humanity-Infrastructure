"""A project's sources read as tables (ADR 0093): the person's files, left as they are, with their columns bound to roles.

A table is the data library's: ``{"name", "title", "source", "rows", "columns": [{"name", "kind": "number" | "text"}], "data": {column: [values]}}``,
here with ``roles`` (which column is the latitude, the time, the amount: ``visualization/js/workspace/model.js``) and, for areas,
``places`` (``{"kind": "cells", "column", "rings": {id: [[lon, lat], ...]}}``) or, for ways, ``lines`` (``[{"line": [[lon, lat], ...], "kind"}]``).

What is read:

    .csv .tsv .txt      delimited text; the delimiter is the one that splits its first line most; a column is of numbers when every
                        field that is not empty is one
    .json               a table of the data library (with its places, if it has them); a city model of the data library (its roads,
                        as lines, turned from its frame to degrees); a list of rows; or GeoJSON
    .geojson            points as rows with lat and lon; areas as rows with a cell and the table's places; lines as lines

A source may carry a filter, ``where``: rows are kept when every condition holds (``{"column", "op": "=" | "!=" | "in" | ">" | ">=" | "<" | "<=",
"value"}``). The filter is applied as the file is read, so a large file of which a part is wanted is never held whole.

A slice (a region and an interval of time) keeps the rows within it, by the same rule as the page's (model.js rowsIn): by lat and
lon, or by the middle of a row's area; and by the row's period, [time, until) or the slice's period that holds its time (a row of
the year 2018 stands for that calendar year).
"""

from __future__ import annotations

import csv
import datetime as dt
import json
import math
import re
from pathlib import Path

PERIODS = {"year": 365.2425 * 86400e3, "quarter": 91.310625 * 86400e3, "month": 30.436875 * 86400e3, "week": 7 * 86400e3, "day": 86400e3, "hour": 3600e3}
OPS = ("=", "!=", "in", ">", ">=", "<", "<=")
MOST_ROWS = 2_000_000
ISO = re.compile(r"^(\d{4})(?:-(\d{1,2})(?:-(\d{1,2})(?:[T ](\d{1,2}):(\d{2})(?::(\d{2}(?:\.\d+)?))?\s*(Z|[+-]\d{2}:?\d{2})?)?)?)?$")
EPOCH = dt.datetime(1970, 1, 1, tzinfo=dt.timezone.utc)


class Unreadable(ValueError):
    """A source that cannot be read as a table, with the reason."""


# ---- time, as the page reads it ---------------------------------------------------------------------------------------------


def _utc(y: int, mo: int = 1, d: int = 1, h: int = 0, mi: int = 0, s: float = 0.0) -> float:
    """Milliseconds UTC of a civil date, months beyond 12 running into the next year (as JavaScript's Date.UTC)."""
    y, mo = y + (mo - 1) // 12, (mo - 1) % 12 + 1
    return (dt.datetime(y, mo, d, h, mi, tzinfo=dt.timezone.utc) - EPOCH).total_seconds() * 1000 + s * 1000


def instant(value) -> float | None:
    """An instant in milliseconds UTC: a number from 1000 to 3000 is a year, a larger one milliseconds; text is ISO 8601."""
    if value is None or value == "" or isinstance(value, bool):
        return None
    if isinstance(value, (int, float)):
        if not math.isfinite(value):
            return None
        if 1000 <= value < 3000:
            return _utc(int(math.floor(value))) + (value - math.floor(value)) * PERIODS["year"]
        return float(value) if abs(value) >= 1e11 else None
    m = ISO.match(str(value).strip())
    if not m:
        return None
    y, mo, d, h, mi = int(m[1]), int(m[2] or 1), int(m[3] or 1), int(m[4] or 0), int(m[5] or 0)
    sec = float(m[6] or 0)
    if not (1 <= mo <= 12 and 1 <= d <= 31 and h <= 23 and mi <= 59):
        return None
    try:
        t = _utc(y, mo, d, h, mi, sec)
    except ValueError:
        return None
    if m[7] and m[7] != "Z":
        sign = 1 if m[7][0] == "-" else -1
        t += sign * (int(m[7][1:3]) * 60 + int(m[7][-2:])) * 60e3
    return t


def period_end(t: float, period: str) -> float:
    d = EPOCH + dt.timedelta(milliseconds=t)
    if period == "year":
        return _utc(d.year + 1)
    if period == "month":
        return _utc(d.year, d.month + 1)
    if period == "quarter":
        return _utc(d.year, d.month + 3)
    return t + PERIODS.get(period, PERIODS["day"])


def interval(piece: dict | None) -> tuple[float, float] | None:
    """A slice's interval [from, until) in milliseconds: its last period is within it."""
    if not piece or not piece.get("time"):
        return None
    a, b = instant(piece["time"].get("from")), instant(piece["time"].get("until"))
    if a is None and b is None:
        return None
    return (-math.inf if a is None else a, math.inf if b is None else period_end(b, piece.get("period", "year")))


# ---- reading ------------------------------------------------------------------------------------------------------------------


def _number(field: str):
    f = field.strip()
    if f == "":
        return None
    try:
        v = float(f.replace(",", "")) if not f.endswith("%") else float(f[:-1].replace(",", "")) / 100
    except ValueError:
        return math.nan
    return v if math.isfinite(v) else math.nan


def _holds(conditions: list, names: list):
    """A test of a row (a list of fields in the order of `names`) against a source's filter."""
    tests = []
    for c in conditions or []:
        column, op, value = c.get("column"), c.get("op", "="), c.get("value")
        if column not in names or op not in OPS:
            raise Unreadable(f"the filter names a column or a comparison that is none: {column} {op}")
        k = names.index(column)
        if op == "in":
            wanted = {str(v) for v in (value if isinstance(value, list) else [value])}
            tests.append(lambda row, k=k, wanted=wanted: str(row[k]).strip() in wanted)
        elif op in ("=", "!="):
            same = str(value).strip()
            tests.append(lambda row, k=k, same=same, op=op: (str(row[k]).strip() == same) == (op == "="))
        else:
            bound = float(value)
            compare = {">": lambda a, b: a > b, ">=": lambda a, b: a >= b, "<": lambda a, b: a < b, "<=": lambda a, b: a <= b}[op]

            def test(row, k=k, bound=bound, compare=compare):
                v = _number(str(row[k])) if not isinstance(row[k], (int, float)) else row[k]
                return v is not None and not (isinstance(v, float) and math.isnan(v)) and compare(v, bound)
            tests.append(test)
    return (lambda row: all(t(row) for t in tests)) if tests else None


def _typed(names: list, rows: list) -> tuple[list, dict]:
    """Columns from rows of fields: of numbers when every field that is not empty is one, else of text."""
    columns, data = [], {}
    for k, name in enumerate(names):
        fields = [r[k] if k < len(r) else "" for r in rows]
        numbers = [f if isinstance(f, (int, float)) and not isinstance(f, bool) else (None if f is None else _number(str(f))) for f in fields]
        numeric = any(v is not None for v in numbers) and not any(isinstance(v, float) and math.isnan(v) for v in numbers)
        columns.append({"name": name, "kind": "number" if numeric else "text"})
        data[name] = numbers if numeric else [None if f is None or str(f).strip() == "" else str(f).strip() for f in fields]
    return columns, data


def read_delimited(path: Path, where: list | None = None) -> dict:
    with open(path, encoding="utf-8-sig", newline="") as f:
        first = f.readline()
        if not first.strip():
            raise Unreadable("the file is empty")
        delimiter = max([",", ";", "\t", "|"], key=first.count)
        f.seek(0)
        reader = csv.reader(f, delimiter=delimiter)
        header = [h.strip() or f"column_{k + 1}" for k, h in enumerate(next(reader))]
        names, seen = [], set()
        for h in header:
            n, k = h, 2
            while n in seen:
                n, k = f"{h}_{k}", k + 1
            seen.add(n)
            names.append(n)
        keep = _holds(where, names)
        rows = []
        for row in reader:
            if not row or (len(row) == 1 and not row[0].strip()):
                continue
            row = row + [""] * (len(names) - len(row))
            if keep is None or keep(row):
                rows.append(row[:len(names)])
                if len(rows) > MOST_ROWS:
                    raise Unreadable(f"more than {MOST_ROWS:,} rows are kept: give the source a filter")
    columns, data = _typed(names, rows)
    return {"rows": len(rows), "columns": columns, "data": data}


def _filtered(table: dict, where: list | None) -> dict:
    names = [c["name"] for c in table["columns"]]
    keep = _holds(where, names)
    if keep is None:
        return table
    rows = [i for i in range(table["rows"]) if keep([table["data"][n][i] if table["data"][n][i] is not None else "" for n in names])]
    return {**table, "rows": len(rows), "data": {n: [table["data"][n][i] for i in rows] for n in names}}


def _from_geojson(doc: dict) -> dict:
    features = doc.get("features") if doc.get("type") == "FeatureCollection" else [doc] if doc.get("type") == "Feature" else []
    names: list = []
    for f in features:
        for k in (f.get("properties") or {}):
            if k not in names:
                names.append(k)
    rows, rings, lines, lat, lon, cell = [], {}, [], [], [], []
    for index, f in enumerate(features):
        g, p = f.get("geometry") or {}, f.get("properties") or {}
        kind = g.get("type")
        if kind == "Point":
            lon.append(g["coordinates"][0]), lat.append(g["coordinates"][1]), cell.append(None)
        elif kind in ("Polygon", "MultiPolygon"):
            outer = g["coordinates"][0] if kind == "Polygon" else max((poly[0] for poly in g["coordinates"]), key=len)
            ident = str(f.get("id", p.get("id", index)))
            rings[ident] = [[c[0], c[1]] for c in outer]
            lon.append(None), lat.append(None), cell.append(ident)
        elif kind in ("LineString", "MultiLineString"):
            for line in ([g["coordinates"]] if kind == "LineString" else g["coordinates"]):
                lines.append({"line": [[c[0], c[1]] for c in line], "kind": str(p.get("kind") or p.get("highway") or "")})
            continue
        else:
            continue
        rows.append([p.get(n) for n in names])
    columns, data = _typed(names, rows)
    table = {"rows": len(rows), "columns": columns, "data": data}
    if any(v is not None for v in lat):
        table["columns"] += [{"name": "lat", "kind": "number"}, {"name": "lon", "kind": "number"}]
        table["data"]["lat"], table["data"]["lon"] = lat, lon
        table["roles"] = {"lat": "lat", "lon": "lon"}
    if rings:
        table["columns"].append({"name": "cell", "kind": "text"})
        table["data"]["cell"] = cell
        table["places"] = {"kind": "cells", "column": "cell", "rings": rings}
        table["roles"] = {**table.get("roles", {}), "cell": "cell"}
    if lines:
        table["lines"] = lines
    return table


def _from_city_model(doc: dict) -> dict:
    """The roads of a city model of the data library as lines in degrees: its frame is metres east and north of its origin."""
    lat0, lon0 = doc["origin"]["lat"], doc["origin"]["lon"]
    east, north = 111319.49 * math.cos(math.radians(lat0)), 110574.0
    lines = [{"line": [[round(lon0 + x / east, 6), round(lat0 + y / north, 6)] for x, y in road["line"]], "kind": road.get("kind", "")}
             for road in doc.get("roads", []) if len(road.get("line", [])) >= 2 and road.get("kind") not in ("motorway", "motorway_link")]
    return {"rows": 0, "columns": [], "data": {}, "lines": lines, "frame": {"lat": lat0, "lon": lon0}}


def read_file(path: Path, where: list | None = None) -> dict:
    if not path.is_file():
        raise Unreadable(f"there is no file {path.name}")
    ext = path.suffix.lower()
    if ext in (".csv", ".tsv", ".txt"):
        return read_delimited(path, where)
    if ext in (".json", ".geojson"):
        try:
            doc = json.loads(path.read_text(encoding="utf-8-sig"))
        except ValueError as error:
            raise Unreadable(f"not JSON: {error}") from error
        if isinstance(doc, dict) and doc.get("type") in ("FeatureCollection", "Feature"):
            return _filtered(_from_geojson(doc), where)
        if isinstance(doc, dict) and isinstance(doc.get("roads"), list) and "origin" in doc:
            return _from_city_model(doc)
        if isinstance(doc, dict) and isinstance(doc.get("columns"), list) and isinstance(doc.get("data"), dict):
            columns = [{"name": c if isinstance(c, str) else c["name"], "kind": "text" if isinstance(c, str) else c.get("kind", "text")} for c in doc["columns"]]
            for c in columns:   # a column the file does not type is typed by what it holds
                values = doc["data"].get(c["name"], [])
                if all(v is None or (isinstance(v, (int, float)) and not isinstance(v, bool)) for v in values) and any(v is not None for v in values):
                    c["kind"] = "number"
            table = {"rows": len(next(iter(doc["data"].values()), [])), "columns": columns, "data": {c["name"]: list(doc["data"][c["name"]]) for c in columns}}
            if isinstance(doc.get("places"), dict) and doc["places"].get("kind") == "cells":
                table["places"] = {"kind": "cells", "column": doc["places"]["column"], "rings": doc["places"]["rings"]}
            if isinstance(doc.get("roles"), dict):
                table["roles"] = doc["roles"]
            return _filtered(table, where)
        if isinstance(doc, list) and doc and all(isinstance(r, dict) for r in doc):
            names = list(dict.fromkeys(k for r in doc for k in r))
            columns, data = _typed(names, [[r.get(n) for n in names] for r in doc])
            return _filtered({"rows": len(doc), "columns": columns, "data": data}, where)
        raise Unreadable("the JSON is neither a table, a list of rows, GeoJSON nor a city model")
    raise Unreadable(f"a source is .csv, .tsv, .txt, .json or .geojson, not {ext or path.name}")


# ---- a slice --------------------------------------------------------------------------------------------------------------------


def rows_in(table: dict, piece: dict | None) -> list:
    """The rows of a table within a slice, by the page's rule."""
    n = table["rows"]
    if not piece:
        return list(range(n))
    roles, region, when = table.get("roles") or {}, piece.get("region"), interval(piece)
    period = piece.get("period", "year")
    centre = None
    if region and roles.get("cell") and (table.get("places") or {}).get("rings") and not (roles.get("lat") and roles.get("lon")):
        centre = {str(k): (sum(p[0] for p in ring) / len(ring), sum(p[1] for p in ring) / len(ring)) for k, ring in table["places"]["rings"].items() if ring}
    inside = lambda lon, lat: lon is not None and lat is not None and region["west"] <= lon <= region["east"] and region["south"] <= lat <= region["north"]  # noqa: E731
    out = []
    for i in range(n):
        if region:
            if roles.get("lat") and roles.get("lon"):
                if not inside(table["data"][roles["lon"]][i], table["data"][roles["lat"]][i]):
                    continue
            elif centre is not None:
                c = centre.get(str(table["data"][roles["cell"]][i]))
                if c is None or not inside(c[0], c[1]):
                    continue
        if when and roles.get("time"):
            a = instant(table["data"][roles["time"]][i])
            if a is None:
                continue
            b = instant(table["data"][roles["until"]][i]) if roles.get("until") else None
            b = (math.inf if roles.get("until") else period_end(a, period)) if b is None else b
            if b <= when[0] or a >= when[1]:
                continue
        out.append(i)
    return out


def take(table: dict, rows: list) -> dict:
    return {**table, "rows": len(rows), "data": {c["name"]: [table["data"][c["name"]][i] for i in rows] for c in table["columns"]}}
