"""Population dynamics, for scripts in Python and the works' servers (ADR 0124): a place's population recorded cell by cell at a
few times, learned as balance laws of a field on its cells (kinesis's balance laws), judged on the times kept aside, and run ahead,
asked of the engine a work has built.

The computations are composed in ``api/cpp/science/population.hpp``, which documents the field it takes
(``noetarium.population-field/1``, as ``data/products/population_history.py`` writes it) and its operations:

    from science import population as pd
    f = pd.field("melbourne")                        # the data library's product, from the cache
    pd.ask("learn", field=f)                         # every model fitted, judged on the years kept aside
    pd.ask("simulate", field=f, model="drift", to=2050)

The engine is a program built by a work: the Urban Studio's plug-in *Population dynamics* builds
``data/cache/engines/urban-studio/population/engine``; ``NOETARIUM_POPULATION_ENGINE`` names another.

What is here beside the asking is what a field needs before and after it is asked, and is no computation of the foundations':
``field`` reads a product; ``cells`` lists its land cells in the order the engine answers in (row-major), each with its row,
column, latitude and longitude where the product gives them; ``table`` lays a field or several beside the cells, as rows.
"""

from __future__ import annotations

import csv
import json
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/population", env="NOETARIUM_POPULATION_ENGINE")
SCHEMA = "noetarium.population-field/1"
ROOT = Path(__file__).resolve().parents[3]
PRODUCTS = ROOT / "data" / "cache" / "products" / "population-history"


def ask(op: str, timeout: float = 900.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def field(case: str | Path) -> dict:
    """A product's field, by its case's name or by the path of its field.json."""
    path = Path(case)
    if not path.suffix:
        path = PRODUCTS / str(case) / "field.json"
    if not path.is_file():
        raise LookupError(f"there is no field at {path}; make it with python data/products/population_history.py {case}")
    f = json.loads(path.read_text(encoding="utf-8"))
    if f.get("schema") != SCHEMA:
        raise ValueError(f"{path} is not a {SCHEMA}")
    return f


def cases() -> list[dict]:
    """The cases the data library has made, from its index."""
    index = PRODUCTS / "index.json"
    return json.loads(index.read_text(encoding="utf-8")).get("cases", []) if index.is_file() else []


def cells(f: dict, folder: Path | None = None) -> list[dict]:
    """The land cells in the engine's order: row, column, and where the product's cells.csv gives them, latitude and longitude."""
    out = [{"row": i, "col": j} for i, row in enumerate(f["land"]) for j, v in enumerate(row) if v]
    where = {}
    csv_path = (folder or (PRODUCTS / f.get("case", ""))) / "cells.csv"
    if csv_path.is_file():
        with open(csv_path, newline="", encoding="utf-8") as fh:
            for r in csv.DictReader(fh):
                where[(int(r["row"]), int(r["col"]))] = (float(r["lat"]), float(r["lon"]))
    for c in out:
        if (c["row"], c["col"]) in where:
            c["lat"], c["lon"] = where[(c["row"], c["col"])]
    return out


def table(f: dict, columns: dict[str, list], folder: Path | None = None) -> list[dict]:
    """Rows of the land cells with each named column beside them (a field, a current), in the engine's order."""
    rows = cells(f, folder)
    for name, values in columns.items():
        if len(values) != len(rows):
            raise ValueError(f"the column {name} has {len(values)} values for {len(rows)} cells")
        for r, v in zip(rows, values):
            r[name] = v
    return rows


def totals(f: dict) -> dict:
    """Each epoch's people, as the product counted them, and which are projections."""
    return {"years": f["epochs"], "people": [f["totals"][str(e)] for e in f["epochs"]], "projected": [e in f.get("projected", []) for e in f["epochs"]]}
