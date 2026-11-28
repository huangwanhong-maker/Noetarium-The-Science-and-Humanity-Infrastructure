"""Checks of the case of a book written by communities (the design record of 2026-11-26), run by run-tests.mjs. Nothing here asks a
model: the society runs dry (every agent by its declared procedure). One JSON object a line.

The bench: levels, the trend to target and the spread on a series kept by the data library, the restricted language refusing what it
forbids and running what it allows. The standards: a claim's number held to its fact as written; a version too short, or citing facts
the section lacks, refused; the scorecard's weakest link. The constitutions: the conference's and the partnership's under every charter
admitted by thesmos. The society: from the conference to the book adopted, every section adopted or withdrawn, the charter ratified by
every community, every journal sealed, every chronicle replayed.

    python applications/koinonia/tests/check_book.py
"""

from __future__ import annotations

import itertools
import json
import shutil
import sys
import tempfile
from pathlib import Path

APP = Path(__file__).resolve().parents[1]
ROOT = APP.parents[1]
sys.path[:0] = [str(APP), str(ROOT / "api" / "python")]

from koinonia import bench, bookcase, cases, partnership  # noqa: E402
from koinonia import agents as ag  # noqa: E402
from koinonia import book as bk  # noqa: E402
from koinonia.behaviour import entries_of, verify  # noqa: E402
from koinonia.budget import Ledger  # noqa: E402
from koinonia.society import Society  # noqa: E402
from science import communities  # noqa: E402
from science.engines import toolchain  # noqa: E402
from science.models import Stub  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "koinonia book: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def main() -> int:
    # ---- the standards (no engine needed) --------------------------------------------------------------------------------------
    facts = {"r1: World 2015": 42.87, "r1: share": 0.3712, "r1: trend": "moderate progress, but acceleration needed"}
    assert_("a claim written to the fact's precision holds; one off by a rounding step does not",
            bk.claim_holds({"fact": "r1: World 2015", "value": "42.9"}, facts)[0] and bk.claim_holds({"fact": "r1: World 2015", "value": "43"}, facts)[0]
            and not bk.claim_holds({"fact": "r1: World 2015", "value": "42.7"}, facts)[0])
    assert_("a share may be written as a percentage", bk.claim_holds({"fact": "r1: share", "value": "37.1%"}, facts)[0])
    assert_("a class is held to its words, and a fact the section lacks fails",
            bk.claim_holds({"fact": "r1: trend", "value": "moderate progress"}, facts)[0] and not bk.claim_holds({"fact": "r9: x", "value": "1"}, facts)[0])
    short = {"text": "too short", "claims": [{"fact": "r1: share", "value": "0.37"}]}
    many = {"text": " ".join(["word"] * 900), "claims": [{"fact": "r7: none", "value": "1"}] * 5}
    assert_("a version too short, or citing facts the section lacks, is refused", bk.version_faults(short, facts, 900) != "" and bk.version_faults(many, facts, 900) != "")
    assert_("the scorecard's weakest link", bk.weakest({"S1": "meets", "S2": "partly", "S3": "meets", "S4": "meets", "S5": "meets"}) == "partly")

    if toolchain() is None:
        emit({"kind": "skip", "name": "koinonia book: the engines (no C++ toolchain here)"})
        return 0
    communities.ENGINE.keep(1)
    # ---- the constitutions ------------------------------------------------------------------------------------------------------
    conf = communities.admit(partnership.conference_constitution())
    assert_("the conference's constitution is admitted", conf["admitted"], conf.get("faults"))
    bad = []
    for values in itertools.product(*partnership.CHARTER.values()):
        ch = dict(zip(partnership.CHARTER, values))
        r = communities.admit(partnership.league_constitution(ch))
        if not r["admitted"]:
            bad.append((ch, r["faults"][:1]))
    assert_(f"the partnership's constitution is admitted under every charter ({len(list(itertools.product(*partnership.CHARTER.values())))})", not bad, bad[:2])

    # ---- the bench (needs the data library's series, fetched once) --------------------------------------------------------------
    try:
        bench.series("SH.DYN.MORT")
        have = True
    except Exception as why:  # no network and nothing kept
        emit({"kind": "skip", "name": f"koinonia book: the bench and the dry society (no series kept: {why})"})
        have = False
    if have:
        lv = bench.run("levels", {"series": "SH.DYN.MORT"})
        assert_("levels give the world's value at the base year and the latest", any(k.startswith("World 2015") for k in lv.get("facts", {})), lv)
        tr = bench.run("trend", {"series": "SH.DYN.MORT", "desired": "decrease", "target": 25, "units": "countries"})
        n = tr.get("facts", {}).get("countries assessed", 0)
        classes = sum(v for k, v in tr.get("facts", {}).items() if k.startswith("countries ") and k != "countries assessed")
        assert_("the trend to target classes every country assessed, once", n > 100 and classes == n, (n, classes))
        sp = bench.run("spread", {"series": "SH.DYN.MORT"})
        assert_("the spread gives Gini and the between-region share of Theil by year", any(k.startswith("gini ") for k in sp["facts"]) and any(k.startswith("between-region") for k in sp["facts"]))
        assert_("a tool on a series the shelf lacks is refused", "refused" in bench.run("levels", {"series": "NOT.A.CODE"}))
        code = "import statistics\ndef tool(data, params):\n    v = [ys[max(ys)] for u, ys in data[params['series']].items() if u in data['regions'] and ys]\n    return {'facts': {'median': statistics.median(v)}, 'summary': 'm'}\n"
        ran = bench.run("median", {"series": "SH.DYN.MORT"}, admitted={"median": {"name": "median", "code": code}})
        assert_("an admitted tool in the restricted language runs in its own interpreter", isinstance(ran.get("facts", {}).get("median"), (int, float)), ran)
        forbidden = ["import os\ndef tool(d, p): return {}", "def tool(d, p): return ().__class__", "def tool(d, p): return open('x')", "def tool(d, p): return '{0.x}'.format(d)"]
        assert_("the restricted language refuses imports, dunders, unknown names and format strings", all(bench.Restricted.faults(c) for c in forbidden),
                [bench.Restricted.faults(c) for c in forbidden])

        # ---- the society, dry, from the conference to the book --------------------------------------------------------------------
        tmp = Path(tempfile.mkdtemp(prefix="koinonia-book-"))
        try:
            s = Society("book-check", tmp)
            s.asker = ag.Asker(Ledger(None), lambda agent, n: Stub(cases.stand_in))
            bookcase.setup(s, dry=True)
            s.parallel = 1
            for _ in range(120):
                s.round()
                if s.book.adopted_book:
                    break
            b = s.book
            assert_("the conference adopted a charter and every community ratified it",
                    b.call.get("charter adopted") and all(v == "ratified" for v in b.call.get("ratifications", {}).values()), b.call.get("ratifications"))
            assert_("the partnership was founded on the charter, and its constitution names the rule the charter chose for the book",
                    partnership.league(s) is not None and partnership.rule_in_force(s, "adopt book").get("rule") == b.call["charter"]["book"])
            assert_("the outline was adopted, and every section ended adopted or withdrawn", b.outline and all(x["stage"] == "adopted" for x in b.sections.values()),
                    {k: x["stage"] for k, x in b.sections.items()})
            assert_("every body section rests on series the commons accepted and results the guild replicated",
                    all(x.get("series") and b.results_of(x["id"]) for x in b.sections.values() if b.chapter(x["chapter"])["kind"] not in ("overview", "proposals")))
            assert_("the council adopted the book by its rule", bool(b.adopted_book) and b.adopted_book.get("rule") == b.call["charter"]["book"], b.adopted_book)
            assert_("every adopted version was audited", all(any(a["version"] == x["adopted"] for a in b.audits) for x in b.sections.values() if x.get("adopted") in b.versions))
            j = verify(entries_of(tmp / "behaviour.jsonl"), s.behaviour.head, s.behaviour.n)
            assert_("the behaviour journal is sealed entry by entry and ends at the head kept", j["holds"], j)
            replays = [communities.run(c.constitution, s.engine_parties(), c.record, seals=c.result.get("seals")).get("replays") for c in s.communities.values()]
            assert_("every community's chronicle replays to the seals it kept", all(replays), replays)
            from koinonia import typeset
            tex = typeset.build(s.to_json(), tmp / "typeset")
            assert_("the typesetter sets the adopted book, its statistical annex and the account of its making",
                    tex.is_file() and (tmp / "typeset" / "annex-making.tex").read_text(encoding="utf-8").count(r"\section") >= 5)
        finally:
            shutil.rmtree(tmp, ignore_errors=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
