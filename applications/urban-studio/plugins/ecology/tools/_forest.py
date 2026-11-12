"""What the ecology tools share: a table of trees read by its columns, the plug-in's declared schemes, and the domain asked through
science.ecology. Nothing of the ecology domain's computation is done again here; what is done here is the reading of the person's
tables and the wording of what came back."""

from __future__ import annotations

import json
import math
from pathlib import Path

import numpy as np

from science import ecology

HERE = Path(__file__).resolve().parent
SCHEMES = HERE.parent / "schemes"


def scheme_files(prefix: str) -> list[str]:
    return sorted(p.name for p in SCHEMES.glob(f"{prefix}-*.json"))


def scheme(file: str) -> dict:
    path = (SCHEMES / Path(str(file)).name)
    if not path.is_file():
        raise LookupError(f"the plug-in declares no scheme '{file}' (it has {', '.join(p.name for p in SCHEMES.glob('*.json'))})")
    return json.loads(path.read_text(encoding="utf-8"))


def column_params(api, table_default: str = "trees") -> list:
    """The parameters every tool on trees has: the table, and which of its columns hold what."""
    col = lambda name, label, default, help="": api.param(name, label, default, kind="text", role="column", of="table", help=help)  # noqa: E731
    return [api.param("table", "Trees", table_default, kind="text", role="table", help="one row a tree"),
            col("species", "Species", "species", "the Latin binomial; empty where only the genus is known"),
            col("genus", "Genus", "genus"), col("family", "Family", "family"),
            col("dbh", "Diameter (cm)", "dbh_cm", "at breast height; empty where not measured"),
            col("area", "Area", "area", "the division the trees are counted by (a precinct, a neighbourhood); empty for none"),
            api.param("largest", "Largest plausible diameter (cm)", 250, kind="number", minimum=20, maximum=2000,
                      help="diameters above it are taken for errors of the record: set aside and counted, not estimated")]


def allometry_params(api) -> list:
    return [api.param("allometry", "Allometry", (scheme_files("allometry") or [""])[0], kind="choice", options=scheme_files("allometry") or [""],
                      help="a declared scheme of the plug-in: equations, groups and their sources"),
            api.param("beyond", "Beyond the source's diameters", "extrapolate", kind="choice", options=["extrapolate", "cap"],
                      help="the equations carried on past the largest tree their source measured, or the tree estimated as that largest one")]


def allometry(params) -> dict:
    s = scheme(params["allometry"])
    s["beyond_range"] = params.get("beyond", "extrapolate")
    return s


def life_params(api) -> list:
    col = lambda name, label, default, help="": api.param(name, label, default, kind="text", role="column", of="table", help=help)  # noqa: E731
    return [col("age", "Age (years)", "age", "since planting, where known: the increments one inventory tells"),
            col("life_lower", "Life from (years)", "life_lower", "the remaining useful life an arborist stated, its lower end"),
            col("life_upper", "Life to (years)", "life_upper", "its upper end; empty for an open class (\"more than …\")")]


def _num(t, name: str):
    if not name:
        return None
    if not t.has(name):
        raise LookupError(f"the table '{t.name}' has no column '{name}'")
    return [None if not math.isfinite(x) else float(x) for x in t.column(name)]


def _text(t, name: str):
    if not name:
        return None
    if not t.has(name):
        raise LookupError(f"the table '{t.name}' has no column '{name}'")
    def word(v):
        if v is None:
            return ""
        if isinstance(v, float):
            return str(int(v)) if v.is_integer() else f"{v:g}"
        return str(v).strip()
    return [word(v) for v in t.raw(name)]


def read_trees(board, params, life: bool = False) -> tuple[dict, list[str], object, dict]:
    """The trees of the table the parameters name, as the engine reads them, with the names of the areas they stand in, and what was
    set aside (diameters beyond the plausible, counted)."""
    t = board.table(params["table"])
    n = t.rows
    if n == 0:
        raise ValueError(f"the table '{t.name}' has no rows in this slice")
    species = _text(t, params.get("species")) or [""] * n
    genus = _text(t, params.get("genus")) or [""] * n
    family = _text(t, params.get("family")) or [""] * n
    dbh = _num(t, params.get("dbh")) or [None] * n
    largest = float(params.get("largest") or 0)
    aside = 0
    if largest > 0:
        for i, d in enumerate(dbh):
            if d is not None and d > largest:
                dbh[i], aside = None, aside + 1
    area_names, area_index = [], None
    if params.get("area"):
        labels = _text(t, params["area"])
        index = {}
        area_index = []
        for a in labels:
            if not a:
                area_index.append(-1)
                continue
            if a not in index:
                index[a] = len(area_names)
                area_names.append(a)
            area_index.append(index[a])
    request = ecology.trees(species, genus, family, dbh, area=area_index)
    if life:
        for key in ("age", "life_lower", "life_upper"):
            if params.get(key) and t.has(params[key]):
                request[key] = ecology.clean(_num(t, params[key]))
    return request, area_names, t, {"set_aside": aside, "largest": largest}


def aside_words(info: dict) -> str:
    n = info.get("set_aside", 0)
    return f" {count(n)} diameters above {info['largest']:.0f} cm were taken for errors of the record and set aside." if n else ""


def share(x: float) -> str:
    return f"{100 * x:.1f}%" if x == x else "—"


def count(x: float) -> str:
    return f"{x:,.0f}"


def mix_from_text(text: str) -> list[dict]:
    """A planting mix written as lines  'Species name; Genus; Family = share'  (the share in any unit; normalised)."""
    out = []
    for raw in str(text or "").splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        names, _, weight = line.rpartition("=")
        if not _:
            names, weight = line, "1"
        parts = [p.strip() for p in names.split(";")]
        while len(parts) < 3:
            parts.append("")
        try:
            w = float(weight)
        except ValueError as error:
            raise ValueError(f"a line of the mix is  Species; Genus; Family = share : '{line}'") from error
        out.append({"species": parts[0], "genus": parts[1] or (parts[0].split()[0] if parts[0] else ""), "family": parts[2], "share": w})
    return out


def present_mix(request: dict, most: int = 12) -> list[dict]:
    """The forest's own mix of species, its most common ones, as a planting that keeps its composition."""
    counts = {}
    for s, g, f in zip(request["species"], request["genus"], request["family"]):
        if s:
            k = (s, g, f)
            counts[k] = counts.get(k, 0) + 1
    top = sorted(counts.items(), key=lambda kv: -kv[1])[:most]
    return [{"species": s, "genus": g, "family": f, "share": c} for (s, g, f), c in top]


def finite(values) -> list:
    return [None if v is None or (isinstance(v, float) and not math.isfinite(v)) else v for v in values]


__all__ = ["ecology", "np", "scheme", "scheme_files", "column_params", "life_params", "allometry_params", "allometry", "read_trees", "aside_words",
           "share", "count", "mix_from_text", "present_mix", "finite"]
