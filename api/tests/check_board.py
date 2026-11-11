"""Checks of the modules of a board (science/board.py), run by api/tests/run-tests.mjs: what a module is given (tables as columns of floats and of text, a
missing value NaN; quantities with their ranges; a table or a quantity that is not there refused with what is), what it answers (tables whose kinds are told
from their values, refused when their columns differ in length or two share a name; figures as SVG or PNG and nothing else), parameters that say what they
name, files named by a parameter (within the data library's cache, among what a work keeps, never outside either), a file of rows and columns read, and
modules written to a directory of their own, described and run through Host: with create(api) and with run(board, api) alone; one that raises reported with
its reason; a file outside the directory refused. One JSON object a line, as check_client.py reports.

    python api/tests/check_board.py
"""

from __future__ import annotations

import json
import math
import os
import shutil
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))
import numpy as np  # noqa: E402

from science import board as bd  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def check(name: str, value: float, limit: float) -> None:
    emit({"kind": "check", "name": name, "value": float(value), "limit": float(limit), "unit": ""})


def assert_(name: str, condition: bool, detail: str = "") -> None:
    emit({"kind": "assert", "name": name, "ok": bool(condition), "detail": str(detail)})


def refused(call, *words: str) -> tuple[bool, str]:
    try:
        call()
    except Exception as error:  # the reason is what is checked
        return all(w in str(error) for w in words), str(error)
    return False, "it was not refused"


# ---- what a module is given ---------------------------------------------------------------------------------------------------------------------------------
GIVEN = {
    "tables": {"towns": {"title": "Towns", "source": "a survey", "columns": [{"name": "city", "kind": "text"}, {"name": "shops", "kind": "number", "label": "shops open"}, {"name": "rent", "kind": "number"}],
                         "data": {"city": ["A", "B", None], "shops": [12, None, 7], "rent": [3400, 5100, 2250]}}},
    "quantities": {"adoption": {"lo": 0.01, "mid": 0.02, "hi": 0.04, "unit": ""}},
    "notes": [{"type": "assumption", "text": "What we believe", "source": "a guess"}],
}
b = bd.Board(GIVEN)
t = b.table("towns")
assert_("board api: a table's columns as floats, a missing value NaN, and as text, a missing value empty",
        t.rows == 3 and t.column("shops").dtype == float and math.isnan(t.column("shops")[1]) and t.column("rent").tolist() == [3400.0, 5100.0, 2250.0]
        and t.text("city") == ["A", "B", ""] and t.numeric() == ["shops", "rent"] and t.labels == {"shops": "shops open"} and t.source == "a survey")
assert_("board api: a column of text asked for as numbers is NaN where it is not one", np.isnan(t.column("city")).all())
q = b.quantity("adoption")
assert_("board api: a quantity carries its range, and stands for its likely value", (q.lo, q.mid, q.hi) == (0.01, 0.02, 0.04) and float(q) == 0.02 and b.notes[0]["source"] == "a guess")
for name, call, words in (("a table that is not on the board", lambda: b.table("cities"), ("cities", "towns")), ("a column the table has not", lambda: t.column("area"), ("area", "city, shops, rent")),
                          ("a quantity no note defines", lambda: b.quantity("wallet"), ("wallet", "adoption"))):
    ok, why = refused(call, *words)
    assert_(f"board api: {name} is refused, and what there is is said", ok, why)
assert_("board api: a quantity not defined may be given a value to stand for it", float(b.quantity("wallet", 0.5)) == 0.5)
ok, why = refused(lambda: bd.Table("x", {"columns": [{"name": "a"}, {"name": "b"}], "data": {"a": [1], "b": [1, 2]}}), "differ in length")
assert_("board api: a table whose columns differ in length is refused", ok, why)

# ---- what it answers ------------------------------------------------------------------------------------------------------------------------------------------
out = bd.table("answer", {"name": ["x", None], "value": np.array([1.5, np.nan]), "n": [1, 2]}, title="An answer", labels={"value": "worth"})
done = bd.finish([out, bd.note("said")])
assert_("board api: a table answered tells its columns' kinds from their values; NaN and None are null",
        out["rows"] == 2 and [c["kind"] for c in out["columns"]] == ["text", "number", "number"] and done[0]["data"]["value"] == [1.5, None] and done[0]["data"]["name"] == ["x", None]
        and out["columns"][1]["label"] == "worth" and done[1] == {"kind": "note", "text": "said"})
for name, call, words in (("a name that is not one", lambda: bd.table("2 x", {"a": [1]}), ("name",)), ("columns of different lengths", lambda: bd.table("x", {"a": [1], "b": [1, 2]}), ("differ in length",)),
                          ("two tables of one name", lambda: bd.finish([out, out]), ("answer",)), ("a result of no kind", lambda: bd.finish([{"kind": "smoke"}]), ("table",)),
                          ("more cells than a board keeps", lambda: bd.finish([bd.table("big", {"a": np.zeros(bd.MOST_CELLS + 1)})]), ("cells",))):
    ok, why = refused(call, *words)
    assert_(f"board api: {name} is refused", ok, why[:160])
svg = bd.figure('<svg xmlns="http://www.w3.org/2000/svg" width="4" height="4"/>', "a drawing")
png = bd.figure(b"\x89PNG\r\n\x1a\n" + b"0" * 16)
assert_("board api: a figure is SVG as text or PNG as bytes", svg["format"] == "svg" and svg["title"] == "a drawing" and png["format"] == "png" and bd.finish([svg])[0]["kind"] == "figure")
for name, call in (("text that is no drawing", lambda: bd.figure("hello")), ("bytes that are no PNG", lambda: bd.figure(b"GIF89a"))):
    ok, why = refused(call, "figure")
    assert_(f"board api: {name} is refused as a figure", ok, why)

# ---- parameters and files -------------------------------------------------------------------------------------------------------------------------------------
p = bd.param("columns", "Alike in", "a, b", kind="text", role="columns", of="table")
assert_("board api: a parameter says what it names", p["role"] == "columns" and p["of"] == "table" and p["kind"] == "text" and "role" not in bd.param("k", "K", 3, kind="integer"))
ok, why = refused(lambda: bd.param("k", "K", 3, kind="integer", role="table"), "role")
assert_("board api: only a text parameter names something", ok, why)
for name, call, words in (("a path that leaves the data library's cache", lambda: bd.cache("..", "CLAUDE.md"), ("not within",)), ("a dataset that is not fetched", lambda: bd.cache("no-such-dataset", "x.csv"), ("fetch",)),
                          ("a file that is not there", lambda: bd.file("C:/no/such/file.csv" if os.name == "nt" else "/no/such/file.csv"), ("no file",)), ("no file named", lambda: bd.file(""), ("no file is named",))):
    ok, why = refused(call, *words)
    assert_(f"board api: {name} is refused", ok, why[:160])

work = Path(tempfile.mkdtemp(prefix="board-check-"))
try:
    kept = work / "kept"
    kept.mkdir()
    (kept / "sales.csv").write_text(chr(0xFEFF) + "year;sales;note\n2020;10;first\n2021;14;\n2022;1,300;last\n", encoding="utf-8")
    (work / "secret.csv").write_text("a,b\n1,2\n", encoding="utf-8")
    os.environ["NOETARIUM_BOARD_FILES"] = str(kept)
    assert_("board api: a file the work keeps is found by its name", bd.file("kept:sales.csv") == (kept / "sales.csv").resolve())
    ok, why = refused(lambda: bd.file("kept:../secret.csv"), "not among")
    assert_("board api: and nothing beside what it keeps", ok, why)
    read = bd.read_delimited(bd.file("kept:sales.csv"), "sales")
    assert_("board api: a file of rows and columns is read: its delimiter found, numbers with their thousands, an empty field missing",
            read.columns == ["year", "sales", "note"] and read.column("sales").tolist() == [10.0, 14.0, 1300.0] and read.kinds == {"year": "number", "sales": "number", "note": "text"} and read.text("note") == ["first", "", "last"])

    # ---- through the host -------------------------------------------------------------------------------------------------------------------------------------
    FULL = '''
import numpy as np
def create(api):
    class Dear(api.BoardAnalysis):
        id = "dear"; title = "The dear ones"; description = "The rows whose rent is above a share of the dearest."
        params = [api.param("table", "Table", "towns", kind="text", role="table"), api.param("share", "Above", 0.5, minimum=0, maximum=1), api.param("label", "Name", "dear", kind="text")]
        def run(self, board, params, context):
            t = board.table(params["table"])
            rent = t.column("rent")
            keep = np.flatnonzero(rent > params["share"] * np.nanmax(rent))
            context.progress(0.5, "halfway")
            scale = float(board.quantity("adoption"))
            return [api.table(params["label"], {"city": [t.text("city")[i] for i in keep], "rent": rent[keep], "scaled": rent[keep] * scale}), api.note(f"{keep.size} of {t.rows}")]
    return Dear()
'''
    modules = work / "modules"
    modules.mkdir()
    (modules / "dear.py").write_text(FULL, encoding="utf-8")
    (modules / "plain.py").write_text('"""Counts what is on the board."""\ndef run(board, api):\n    return [api.note(f"{len(board.tables)} tables, {len(board.quantities)} quantities, {len(board.notes)} notes")]\n', encoding="utf-8")
    (modules / "raises.py").write_text("def run(board, api):\n    raise ValueError('it failed on purpose')\n", encoding="utf-8")
    (modules / "empty.py").write_text("x = 1\n", encoding="utf-8")
    host = bd.Host(modules, work / "work")
    d = host.describe("dear.py")
    assert_("board host: a module describes itself: what it is and the parameters it needs, each with its kind and what it names",
            d["id"] == "dear" and d["kind"] == "board-analysis" and [(q["name"], q["kind"], q.get("role")) for q in d["params"]] == [("table", "text", "table"), ("share", "number", None), ("label", "text", None)], d)
    seen = []
    results = host.run("dear.py", GIVEN, {"share": 0.6}, progress=lambda f, text: seen.append((f, text)))
    assert_("board host: it is run on what stands on the board, its parameters given or by default, and answers a table and a note",
            [r["kind"] for r in results] == ["table", "note"] and results[0]["name"] == "dear" and results[0]["data"]["city"] == ["A", "B"] and results[0]["data"]["rent"] == [3400.0, 5100.0] and results[1]["text"] == "2 of 3")
    check("board host: a quantity of the board scales what the module answers", results[0]["data"]["scaled"][1] - 5100 * 0.02, 1e-9)
    assert_("board host: its progress is told on the way", (0.5, "halfway") in seen, seen)
    named = host.run("dear.py", GIVEN, {"label": "costly"})
    assert_("board host: a text parameter reaches the module as it is written", named[0]["name"] == "costly")
    plain = host.run("plain.py", GIVEN)
    assert_("board host: a script that only defines run(board, api) is a module too, described by its first line",
            plain == [{"kind": "note", "text": "1 tables, 1 quantities, 1 notes"}] and host.describe("plain.py")["title"] == "Counts what is on the board.")
    for name, call, words in (("a module that raises", lambda: host.run("raises.py", GIVEN), ("it failed on purpose",)), ("a parameter out of its bounds", lambda: host.run("dear.py", GIVEN, {"share": 3}), ("share",)),
                              ("a table the board has not", lambda: host.run("dear.py", GIVEN, {"table": "cities"}), ("cities",)), ("a script that defines neither", lambda: host.describe("empty.py"), ("neither",)),
                              ("a file outside the directory", lambda: host.run("../secret.py", GIVEN), ("not a plug-in",))):
        ok, why = refused(call, *words)
        assert_(f"board host: {name} is reported with its reason", ok, why[-200:])
finally:
    shutil.rmtree(work, ignore_errors=True)
