"""Checks of a city's municipal solid waste and SDG 11.6.1 at the API's edge (science/waste.py and api/cpp/science/waste.hpp), run where
the Urban Studio's plug-in *Materials and flows* has built its engine, every expected value worked by hand: declarations refused; a
survey of two strata of two areas, its total and interval; a cumulative ladder; residues counted once at the gate; the indicator with
the survey's interval carried; an imbalance; and the plug-in's invented city read whole. One JSON object a line.

    python api/tests/check_waste.py
"""

from __future__ import annotations

import csv
import math
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import materials  # noqa: E402
from science import waste as ws  # noqa: E402

DATA = ROOT / "applications" / "urban-studio" / "plugins" / "materials" / "templates" / "data"


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "waste: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def main() -> None:
    for bad, why in ((lambda: ws.household(1, 1, 0, 5, 7), "a household of no one"), (lambda: ws.facility("x", "transfer", 1), "a facility neither recovery nor disposal")):
        try:
            bad()
            assert_(f"{why} is refused", False)
        except ValueError:
            assert_(f"{why} is refused", True)
    if not materials.ENGINE.program.exists():
        emit({"kind": "skip", "name": "waste: the composition (the materials engine is not built: run the Urban Studio's recipe)"})
        return
    try:
        S = ws.survey({1: 1000, 2: 500}, [ws.household(1, 1, 4, 14, 7), ws.household(1, 1, 2, 7, 7), ws.household(1, 2, 3, 21, 7), ws.household(1, 2, 5, 14, 7),
                                         ws.household(2, 1, 2, 28, 7), ws.household(2, 1, 4, 35, 7), ws.household(2, 2, 3, 21, 7), ws.household(2, 2, 1, 14, 7)])
        L = {"levels": ["very limited", "limited", "basic", "improved", "full"], "criteria": [[], ["fenced"], ["compacted", "covered"], ["leachate"], ["gas"]], "controlled_from": 2}
        F = [ws.facility("R", "recovery", 1.0, 0.4, "Lf", ["fenced", "compacted", "covered"]), ws.facility("Lf", "disposal", 2.6, met=["fenced", "compacted", "covered", "leachate"]),
             ws.facility("D", "disposal", 0.8, met=["compacted"])]
        A = ws.account(S, L, F, other={"markets": 1.25}, not_delivered=0.1)
        assert_("households 3.75 t a day, standard error √0.5 on 2 degrees of freedom; 5 t in all with the markets",
                abs(A["households"]["value"] - 3.75) < 1e-12 and abs(A["households"]["se"] - 0.5 ** 0.5) < 1e-12 and A["households"]["df"] == 2 and abs(A["generated"] - 5) < 1e-12, A["households"])
        lv = {f["id"]: f["level_name"] for f in A["facilities"]}
        assert_("the landfill improved, the plant basic, the dumpsite very limited (compacted, not fenced)", lv == {"R": "basic", "Lf": "improved", "D": "very limited"}, lv)
        assert_("managed in control 0.4 + 2.6 = 3, the residue counted once; SDG 11.6.1 = 0.6; coverage 3.9/5",
                abs(A["managed_controlled"] - 3) < 1e-12 and abs(A["indicator"] - 0.6) < 1e-12 and abs(A["collection_coverage"] - 0.78) < 1e-12, A)
        assert_("the indicator's interval the image of the survey's", abs(A["indicator_lower"] - 3 / (A["households"]["upper"] + 1.25)) < 1e-12
                and abs(A["indicator_upper"] - 3 / (A["households"]["lower"] + 1.25)) < 1e-12, A)
        B = ws.account(S, L, [F[0], dict(F[1], received=0.5), F[2]], other={"markets": 1.25})
        assert_("residues beyond what a gate weighed: an imbalance of 0.1, said", abs(B["imbalance"] - 0.1) < 1e-12 and B["remarks"], B["remarks"])
        try:
            ws.account(S, L, [dict(F[0], recovered=2.0), F[1], F[2]])
            assert_("a plant recovering more than it receives is refused by the engine", False)
        except ws.EngineError:
            assert_("a plant recovering more than it receives is refused by the engine", True)

        def rows(name):
            with open(DATA / name, encoding="utf-8", newline="") as f:
                return list(csv.DictReader(f))
        strata = {int(r["stratum"]): float(r["households"]) for r in rows("waste-strata.csv")}
        sample = [ws.household(int(r["stratum"]), int(r["area"]), float(r["persons"]), float(r["kg"]), float(r["days"])) for r in rows("waste-households.csv")]
        lad = rows("waste-ladder.csv")
        levels, criteria = [], []
        for r in sorted(lad, key=lambda r: int(r["level"])):
            if not levels or levels[-1] != r["name"]:
                levels.append(r["name"]), criteria.append([])
            if r["criterion"]:
                criteria[-1].append(r["criterion"])
        fac = [ws.facility(r["id"], r["kind"], float(r["received"]), float(r["recovered"] or 0), r["residue_to"], [c.strip() for c in r["met"].split(";") if c.strip()])
               for r in rows("waste-facilities.csv")]
        so = rows("waste-sorted.csv")
        sorted_ = [ws.sorted_sample(int(r["stratum"]), int(r["area"]), {k: float(v) for k, v in r.items() if k not in ("stratum", "area")}) for r in so]
        C = ws.account(ws.survey(strata, sample), {"levels": levels, "criteria": criteria, "controlled_from": levels.index("basic control")}, fac,
                       other={"markets": 25, "commerce": 45, "institutions": 12}, not_delivered=8, sorted=sorted_)
        assert_("the invented city: ninety households, an indicator within its interval, the composting site not controlled, the shares summing to one",
                len(sample) == 90 and C["indicator_lower"] <= C["indicator"] <= C["indicator_upper"]
                and {f["id"]: f["controlled"] for f in C["facilities"]} == {"mrf": True, "compost": False, "landfill": True, "dumpsite": False}
                and abs(sum(x["value"] for x in C["composition"]) - 1) < 1e-12, C)
        # What waste emits (ADR 0153): the Annex's series, composting by Table 4.1, and the GPC's scopes and BASIC rule.
        annex = ws.ask("waste.emissions", edition="2006", climate="tropical wet", year=2021, potentials="AR6",
                       sites=[{"id": "s", "type": "managed anaerobic", "composition": {"food": 1.0}, "deposited": {"2020": 1000}, "origin": "inside"}])
        g = 1000 * 0.15 * 0.5 * (1 - math.exp(-0.4)) * 0.5 * 16 / 12
        assert_("a site's methane by first-order decay: 1,000 t of food in 2020 emit 1000 · 0.15 · 0.5 (1 − e^{−0.4}) · 0.5 · 16/12 t in 2021",
                abs(annex["sites"][0]["emitted"] - g) < 1e-9 and annex["sites"][0]["series"][0]["emitted"] == 0, annex["sites"])
        mix = ws.ask("waste.emissions", edition="2006", climate="tropical wet", year=2021, potentials="AR6",
                     composting=[{"id": "c", "tonnes": 1000, "origin": "outside"}],
                     sites=[{"id": "n", "type": "managed anaerobic", "composition": {"food": 1.0}, "deposited": {"2020": 1000}, "origin": "imported"}])
        codes = {e["id"].split(" ")[0]: e for e in mix["entries"]}
        assert_("composting outside is III.2.2 in scope 3, 4 t CH4 and 0.24 t N2O; a neighbour's waste at the site III.1.3 outside BASIC",
                codes["III.2.2"]["scope"] == 3 and abs(sum(e["kg"] for e in mix["entries"] if e["id"].startswith("III.2.2") and e["gas"] == "N2O") - 240) < 1e-9
                and codes["III.1.3"]["imported_waste"] and abs(mix["basic"] - (4 * 27.0 + 0.24 * 273)) < 1e-9, mix)
    except ws.EngineError as e:
        assert_("the engine answered", False, e)


if __name__ == "__main__":
    main()
