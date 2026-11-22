"""Checks of the structure of regional economies at the API's edge (api/cpp/science/diversification.hpp, asked through
science.diversification), run where the Urban Studio's plug-in *A region's economy* has built its engine, every expected value worked by
hand: four regions of one reference, the first taking up the industry nearest those it holds (one entry, an area under the curve of one),
its shift-share summing to its change, a withheld cell counted; the product's table read into the request. One JSON object a line.

    python api/tests/check_diversification.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import diversification  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "diversification: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def main() -> None:
    p = {"years": [2000, 2010], "regions": [{"nuts": "a", "country": "X"}, {"nuts": "b", "country": "X"}], "industries": [{"code": "C10", "section": "C"}],
         "employment": {"2000": [[1], [2]], "2010": [[3], [None]]}}
    t = diversification.table(p)
    assert_("a product read into the engine's table: references by country, groups by section, a withheld cell kept as null",
            t["references"] == ["X", "X"] and t["groups"] == ["C"] and t["after"][1][0] is None, t)
    if not diversification.ENGINE.program.exists():
        emit({"kind": "skip", "name": "diversification: the composition (the engine is not built: python applications/urban-studio/engine.py build economy)"})
        return
    table = {"regions": ["a", "b", "c", "d"], "references": ["X"] * 4, "industries": ["p", "q", "s", "t"], "groups": ["P", "P", "P", "T"],
             "before": [[10, 10, 0, 1], [10, 10, 10, 1], [10, 10, 10, None], [0, 0, 0, 30]],
             "after": [[10, 10, 10, 1], [10, 10, 10, 1], [10, 10, 10, 1], [0, 0, 0, 30]]}
    try:
        a = diversification.ask("diversification.assess", table=table)
        r0 = a["regions"][0]
        assert_("the first region takes up the industry nearest those it holds: one entry, its density above every candidate not taken up (AUC 1)",
                a["entries"] == 1 and r0["entries"] == 1 and a["entry"]["auc"] == 1, a["entry"])
        assert_("... its shift-share summing to its change, 10", abs(r0["national"] + r0["mix"] + r0["competitive"] - 10) < 1e-9, r0)
        assert_("... a withheld cell counted for the third region", a["regions"][2]["withheld_before"] == 1, a["regions"][2])
        assert_("... the table fallen apart into two blocks leaves the complexity undefined, with its reason", not a["complexity_after"] and "blocks" in a["complexity_note"], a["complexity_note"])
    except diversification.EngineError as e:
        assert_("the composition answers", False, e)


if __name__ == "__main__":
    main()
