"""Checks of scorecards, knowledge programmes, local government finance and land value capture at the API's edge (science/programmes.py,
science/fiscal.py and api/cpp/science/{scorecards,learning,fiscal,capture}.hpp), run where the Urban Studio's plug-in *Programmes and public finance* has built
its engine, on the plug-in's invented cohort, every expected value worked by hand: declarations refused where they are wrong; a city's
scorecard scored part by part, its weaknesses with their evidence, two assessors' agreement, two cards compared by order; the programme
read at a date, the scorecards' weaknesses joining its needs, each action's state, the claims substantiated or not and why, the
formats' frontier; a city's ratios judged, its years ahead and the largest loan it can service, which once taken holds the coverage at
its floor; debt dynamics and capacity and effort in closed form; a European country's local debt with a fan; land value capture's instruments
kept apart by what they recover and what they take, a parcel charged twice over, and a readjustment with its replotting. One JSON object
a line.

    python api/tests/check_programmes.py
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")   # the names carry α and ↦
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import fiscal as fs  # noqa: E402
from science import programmes as pg  # noqa: E402

E = json.loads((ROOT / "applications" / "urban-studio" / "plugins" / "programmes" / "examples" / "cohort.json").read_text(encoding="utf-8"))


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "programmes: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "programmes: " + name})


def refused(f) -> bool:
    try:
        f()
        return False
    except ValueError:
        return True


assert_("a node of an unknown rule is refused", refused(lambda: pg.node("x", ["a"], "average")))
assert_("a scheme of one level is refused", refused(lambda: pg.scheme("s", [pg.level("1")], [], [pg.node("x", ["a"])])))
assert_("an update that is not started, completed, dropped or revised is refused", refused(lambda: pg.update("2026-01-01", "paused")))
assert_("a threshold whose sound side is neither at least nor at most is refused", refused(lambda: fs.threshold("r", "near")))
assert_("a loan with a negative principal is refused", refused(lambda: fs.loan("x", -1, 0.05, 2027, 10)))

if not pg.ENGINE.program.exists():
    skip("the engine is not built (the Urban Studio's plug-in builds it)")
else:
    try:
        S = E["scheme"]
        r = pg.score(S, E["cards"]["ardel"], weak_at="1")
        by = {n["node"]: n for n in r["nodes"]}
        assert_("Ardel's parts: governance the weakest of 3 and 2, knowing the risk of 1 and 2, finance of 1 and 0",
                by["governance"]["code"] == "2" and by["risk"]["code"] == "1" and by["finance"]["code"] == "0", by)
        assert_("... preparedness the median of what was rated (3; the unrated left out, as the node declares)", by["response"]["code"] == "3" and by["response"]["left_out"] == 1, by["response"])
        assert_("... the scorecard as a profile, one part at each of 0, 1, 2 and 3", by["all"]["profile"] == [1, 1, 1, 1, 0, 0], by["all"]["profile"])
        assert_("... its weaknesses at 1 or below, lowest first, each with its evidence: f2, then r1 and f1",
                [w["statement"] for w in r["weaknesses"]] == ["f2", "r1", "f1"] and all(w["evidence"] for w in r["weaknesses"]), r["weaknesses"])
        a = pg.agreement(S, [E["cards"]["ardel"], E["cards"]["ardel_second"]])
        assert_("two assessors of Ardel: Krippendorff's ordinal α between 0 and 1", 0 < a["alpha"] <= 1, a)
        c = pg.compare(S, E["cards"]["ardel"], E["cards"]["ardel_second"])
        ch = {x["id"]: x for x in c["changes"]}
        assert_("two cards compared by order: knowing the risk rises from 1 to 2, the comparison meaningful on the ordinal scale",
                ch["risk"]["before"] == "1" and ch["risk"]["after"] == "2" and ch["risk"]["direction"] == 1 and ch["risk"]["meaningful"] is True, ch["risk"])
        corvo = pg.score(S, E["cards"]["corvo"])
        assert_("a score without evidence is named (Corvo's warnings)", corvo["unjustified"] == ["w1"], corvo["unjustified"])

        P = pg.assess(E["programme"], "2026-10-15", needs_from=[{"participant": p["id"], "scheme": S, "card": E["cards"][p["id"]], "weak_at": "1"} for p in E["programme"]["participants"]])
        assert_("the scorecards' weaknesses join the needs: Ardel three, Brisa one, Corvo five", len(P["needs_from_scorecards"]) == 9, P["needs_from_scorecards"])
        assert_("... Brisa's contingent finance reached by no activity, named unmet", "brisa:f2" in P["unmet"], P["unmet"])
        st = {x["action"]: x["state"] for x in P["actions"]}
        assert_("the actions on 15 October: completed on time, in progress, overdue, completed on time, not yet due, dropped",
                [st[k] for k in ("a1", "a2", "a3", "a4", "a5", "a6")] == ["completed on time", "in progress", "overdue", "completed on time", "not yet due", "dropped"], st)
        cl = {x["claim"]: x for x in P["claims"]}
        assert_("the budget line's claim substantiated, its rival addressed; the heat scenario's held back by a heatwave not addressed",
                cl["c1"]["substantiated"] and not cl["c2"]["substantiated"] and cl["c2"]["unaddressed"] == ["a heatwave in July"] and cl["c3"]["substantiated"], cl)
        fm = {x["format"]: x for x in P["formats"]}
        assert_("the formats: the peer exchange credited with both substantiated claims; the frontier the peer exchange and the cheaper workshop",
                abs(fm["peer exchange"]["credit"] - 2) < 1e-12 and fm["peer exchange"]["frontier"] and fm["diagnostic workshop"]["frontier"]
                and not fm["study visit"]["frontier"] and not fm["advisory"]["frontier"], fm)

        A = E["accounts"]
        F = fs.assess(fs.accounts(A["unit"], A["currency"], A["chart"], A["years"]), A["ratios"], A["thresholds"], A["projection"], A["capacity"])
        rv = {(x["ratio"], x["year"]): x for x in F["ratios"]}
        osr = rv[("operating surplus over operating revenue", 2025)]
        assert_("Ardel's operating surplus in 2025: 7,800 over 89,600, sound against a warning below 0.08",
                abs(osr["value"] - 7800 / 89600) < 1e-12 and osr["status"] == "sound", osr)
        cap = F["capacity"]
        loans = A["projection"]["loans"] + [fs.loan("new", cap["largest"], A["capacity"]["rate"], A["capacity"]["first_year"], A["capacity"]["tenor"])]
        G = fs.assess(fs.accounts(A["unit"], A["currency"], A["chart"], A["years"]), [], [], dict(A["projection"], loans=loans))
        rep = [x["coverage"] for x in G["projection"] if A["capacity"]["first_year"] <= x["year"] < A["capacity"]["first_year"] + A["capacity"]["tenor"] and x["coverage"] is not None]
        bound = cap["binding"] == "coverage"
        assert_("the largest new loan, once taken, keeps the coverage at or above its floor in every repayment year (and at it where coverage binds)",
                cap["largest"] > 0 and min(rep) >= A["capacity"]["floor"] - 1e-9 and (not bound or abs(min(rep) - A["capacity"]["floor"]) < 1e-9), (cap, min(rep)))
        D = fs.debt(0.05, [0.03] * 5, [0.02] * 5, [0.0] * 5)
        assert_("debt dynamics: a year multiplies 0.05 by 1.03/1.02; the stabilising balance 0.05·0.01/1.02",
                abs(D["path"][1] - 0.05 * 1.03 / 1.02) < 1e-15 and abs(D["stabilising_balance"] - 0.05 * 0.01 / 1.02) < 1e-15, D)
        Q = fs.effort(["one", "two"], [[100, 50], [300, 50]], [[10, 5], [20, 15]], ["property", "business"])
        assert_("capacity and effort: the first government's capacity 17.5 and effort 15/17.5", abs(Q["capacity"][0] - 17.5) < 1e-12 and abs(Q["effort"][0] - 15 / 17.5) < 1e-12, Q)
        try:
            C = fs.local_government("FR")
            rec = fs.record_of(C)
            d0 = next(v for v in reversed(C["debt_share"]) if v is not None)
            H = fs.debt(d0, [rec["r"][-1]] * 8, [rec["g"][-1]] * 8, [rec["pb"][-1]] * 8, record=rec, draws=1000, threshold=0.1)
            qs = H["fan"]["paths_at"]
            assert_("France's local government debt from Eurostat: a fan of 1,000 paths, its quantiles ordered every year",
                    0 < d0 < 0.2 and all(q[0] <= q[1] <= q[2] for q in qs) and len(qs) == 9, (d0, qs[-1]))
        except LookupError:
            skip("France's local government debt (python data/products/local_government_finance.py)")

        # Land value capture and readjustment (ADR 0146), on four invented parcels whose uplift is 100, 50, 0 and −10 (taken as none).
        ps = [fs.parcel("A", 400, 1000, 1100, 20), fs.parcel("B", 200, 600, 650, 10), fs.parcel("C", 400, 800, 800, 20), fs.parcel("D", 100, 300, 290, 10)]
        try:
            fs.capture_compare(ps, [], {"bonds": {}})
            assert_("an instrument the composition does not know is refused", False)
        except ValueError:
            assert_("an instrument the composition does not know is refused", True)
        try:
            fs.capture_compare([], [{"year": 0, "amount": 1}], {})
            assert_("a project with no parcels is refused by the engine", False)
        except pg.EngineError:
            assert_("a project with no parcels is refused by the engine", True)
        K = fs.capture_compare(ps, [{"year": 0, "amount": 120}], {
            "levy": {"cost": 120, "markup": 0.1, "share": 0.5, "basis": "uplift"},
            "rights": {"added_floor": 100, "basic_far": 2, "land_price": 2, "factor": 0.6, "parcel": "A"},
            "increment": {"assessed": [110, 130, 160], "base": 100, "share": 1.0, "rates": {"city": 0.01, "schools": 0.02}, "but_for": 0.6},
            "obligation": {"units": 100, "rate": 0.5, "to_land": 0.6, "to_buyers": 0.4}}, rate=0.0, statutory={"levy": 0.5})
        ins = {x["instrument"]: x for x in K["instruments"]}
        assert_("the cost 120 and the uplift 150 at a rate of zero", abs(K["pv_cost"] - 120) < 1e-12 and abs(K["pv_uplift"] - 150) < 1e-12, K)
        assert_("the levy of min(120 · 1.1, half of 150) = 75 recovers 0.625 of the cost and takes 0.5 of the uplift, the statutory 0.5 beside it",
                abs(ins["levy"]["pv"] - 75) < 1e-12 and abs(ins["levy"]["cost_recovery"] - 0.625) < 1e-12 and abs(ins["levy"]["capture"] - 0.5) < 1e-12
                and abs(ins["levy"]["statutory"] - 0.5) < 1e-12, ins["levy"])
        assert_("100 m² of floor over a basic ratio of 2 at 2 a m², factor 0.6: 60, taking 0.4 of the uplift", abs(ins["rights"]["pv"] - 60) < 1e-12 and abs(ins["rights"]["capture"] - 0.4) < 1e-12, ins["rights"])
        assert_("tax increment: 3% of 10, 30 and 60 is 3, of which 1.2 diverted, 0.8 from the schools; only the project's 0.6 of it taken from the uplift (1.8/150)",
                abs(K["increment"]["pv_district"] - 3) < 1e-12 and abs(K["increment"]["pv_diverted"] - 1.2) < 1e-12
                and abs(K["increment"]["diverted_by_jurisdiction"]["jurisdiction:schools"] - 0.8) < 1e-12 and abs(ins["increment"]["capture"] - 1.8 / 150) < 1e-12, (K["increment"], ins["increment"]))
        assert_("an obligation of 0.5 on 100 units recovers 50 and takes nothing from the uplift; 30 borne by land, 20 by buyers",
                abs(ins["obligation"]["pv"] - 50) < 1e-12 and ins["obligation"]["capture"] == 0 and abs(K["obligation"]["on_land"] - 30) < 1e-12, (ins["obligation"], K["obligation"]))
        assert_("parcel A, its uplift of 100 taken twice over (50 by the levy, 60 for rights granted on it), is named, and only it",
                [(d["parcel"], round(d["taken"], 9)) for d in K["double_charges"]] == [("A", 110.0)], K["double_charges"])
        owners = [fs.parcel("o1", 100, 1000, 0, x=0, y=0), fs.parcel("o2", 100, 500, 0, x=10, y=0), fs.parcel("o3", 100, 500, 0, x=0, y=10)]
        plots = [{"id": f"p{i + 1}", "area": a, "unit_value": 20, "x": x, "y": y} for i, (a, x, y) in enumerate([(60, 9, 1), (60, 1, 1), (50, 1, 9), (80, 20, 20)])]
        L = fs.readjust({"public_before": 200, "private_before": 1000, "public_after": 400, "returned": 650, "reserve": 150}, 100, 160, [{"year": 1, "amount": 20000}], 2,
                        rate=0.05, cap=0.4, selling_cost=0.05, owners=owners, plots=plots)
        assert_("readjustment: the areas balance, the owners give (200 + 150)/1000 = 0.35, (0.65)(1.6) = 1.04 keeps their value, within a cap of 0.4",
                L["areas_balance"] and abs(L["contribution"] - 0.35) < 1e-12 and abs(L["kept"] - 1.04) < 1e-12 and L["value_kept"] and L["within_cap"], L)
        assert_("... the reserve sells for 150 · 160 · 0.95 = 22,800, and the balance 22,800/1.05² − 20,000/1.05 is feasible",
                abs(L["reserve_revenue"] - 22800) < 1e-9 and abs(L["pv_balance"] - (22800 / 1.1025 - 20000 / 1.05)) < 1e-9 and L["feasible"], L)
        rp = L["replotting"]
        assert_("... each owner the nearest plot (o1 p2, o2 p1, o3 p3) at a total distance of 3√2; o1 due half of 3,400; the settlements sum to zero",
                [x["plot"] for x in rp["assignments"]] == ["p2", "p1", "p3"] and abs(rp["distance"] - 3 * 2 ** 0.5) < 1e-9
                and abs(rp["assignments"][0]["entitlement"] - 1700) < 1e-9 and abs(sum(x["settlement"] for x in rp["assignments"])) < 1e-9, rp)
        # A project's financing plan (ADR 0152): a surplus of 100 a year and no debt; a cost of 300 in 2026, receipts of 100, loans at no
        # interest over four years. Borrowed 200, repaid 50 a year: coverage 2. At a floor of 2 the largest scale is (200 + 100)/300 = 1
        # with the receipts and 200/300 without.
        acc = fs.accounts("thousands", "an invented currency", [fs.line("1", "taxes", "own"), fs.line("2", "running", "operating")], [fs.year(2025, {"1": 100, "2": 0})])
        proj = {"base_year": 2025, "years": 6, "growth": {}, "surplus": [{"category": "own", "sign": 1}, {"category": "operating", "sign": -1}], "loans": []}
        Pn = fs.ask("fiscal.plan", accounts=acc, projection=proj, plan={"cost": [{"year": 2026, "amount": 300}], "receipts": [{"year": 2026, "amount": 100, "source": "levy"}],
                                                                       "rate": 0, "tenor": 4}, floor=2, most=10)
        W = Pn["with_receipts"]
        assert_("a project's plan: 200 borrowed, 50 a year from 2027, the coverage 100/50 = 2",
                abs(W["borrowed"] - 200) < 1e-9 and abs(W["years"][1]["new_service"] - 50) < 1e-9 and abs(W["least_coverage"] - 2) < 1e-9, W)
        assert_("... the largest scale at a floor of 2: 1 with the receipts, 2/3 without",
                abs(Pn["largest_with"]["scale"] - 1) < 2e-6 and abs(Pn["largest_without"]["scale"] - 2 / 3) < 2e-6, (Pn["largest_with"], Pn["largest_without"]))
    except pg.EngineError as e:
        assert_("the engine answered", False, e)
