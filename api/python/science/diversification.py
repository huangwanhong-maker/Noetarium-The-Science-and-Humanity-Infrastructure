"""The structure of regional economies, for scripts in Python and the works' servers (ADR 0166): regions' employment by industry at two
dates read for shift-share, related and unrelated variety, relatedness and its test of entries, and economic complexity, asked of the
engine a work has built around ``api/cpp/science/diversification.hpp``, which documents the request.

    from science import diversification
    P = diversification.product("europe", 2011, 2019)
    diversification.ask("diversification.assess", table=diversification.table(P))

The engine is the Urban Studio's plug-in *A region's economy*: ``data/cache/engines/urban-studio/economy/engine``;
``NOETARIUM_ECONOMY_ENGINE`` names another. The employment comes from the data library's ``regional_structure.py`` (Eurostat).
"""

from __future__ import annotations

import json
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ROOT = Path(__file__).resolve().parents[3]
PRODUCTS = ROOT / "data" / "cache" / "products" / "regional-structure"
ENGINE = Engine("urban-studio/economy", env="NOETARIUM_ECONOMY_ENGINE")


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    return ENGINE.ask(op, timeout=timeout, **request)


def product(case: str = "europe", y0: int = 2011, y1: int = 2019) -> dict:
    """A case's employment by region and NACE division at two dates, as the data library made it."""
    path = PRODUCTS / f"{case}-{y0}-{y1}.json"
    if not path.is_file():
        raise LookupError(f"no regional structure {case} for {y0}–{y1}: python data/products/regional_structure.py --case {case} --from {y0} --to {y1}")
    return json.loads(path.read_text(encoding="utf-8"))


def table(p: dict) -> dict:
    """The product as the engine's table: each region's reference its country, each division's group its NACE section."""
    y0, y1 = (str(y) for y in p["years"])
    return {"regions": [r["nuts"] for r in p["regions"]], "references": [r["country"] for r in p["regions"]],
            "industries": [i["code"] for i in p["industries"]], "groups": [i["section"] for i in p["industries"]],
            "before": p["employment"][y0], "after": p["employment"][y1]}
