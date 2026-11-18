"""Checks of steering at the API's edge (api/cpp/science/steering.hpp, api/python/science/steering.py), run by api/tests/run-tests.mjs where
the Urban Studio's plug-in *From evidence to action* has built the engine. An invented engagement, everything in it invented and said to be:
evidence of a city's evacuation and refuges is observed; the constitution declares open terms, operators and a standard; a turn is run by
the predefined methods, and the evaluations come with their standing over the readings and the terms they turn on; the practice that
answers a need is a candidate; an analyst's choice reaches the decision; a branch re-reads a term; the record verifies, replays and is
refused when altered; a transfer by components finds its support factor. One JSON object a line, as check_client.py reports.

    python api/tests/check_steering.py
"""

from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import steering  # noqa: E402
from science.engines import EngineError  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "steering: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def block(view: dict, branch: str, turn: int, name: str) -> dict:
    b = next(x for x in view["branches"] if x["name"] == branch)
    return next(r for r in b["turns"][turn]["blocks"] if r["block"] == name)


ITEMS = [
    {"quantity": "evacuation_share_30min", "label": "people out of the hazard's reach within 30 minutes", "value": 0.62, "low": 0.55, "high": 0.70,
     "scale": "absolute", "readings": {"leaving": "as in 2011"}, "source": "risk/evacuation.py"},
    {"quantity": "evacuation_share_30min", "value": 0.95, "low": 0.92, "high": 0.97, "scale": "absolute", "readings": {"leaving": "at once"}, "source": "risk/evacuation.py"},
    {"quantity": "refuge_reach_share", "value": 0.78, "low": 0.70, "high": 0.85, "readings": {"within reach": "15 minutes"}, "source": "risk/evacuation.py"},
    {"quantity": "refuge_reach_share", "value": 0.55, "low": 0.45, "high": 0.62, "readings": {"within reach": "10 minutes"}, "source": "risk/evacuation.py"},
]
TERMS = {"leaving": [{"reading": "as in 2011", "source": "data"}, {"reading": "at once", "source": "declared"}],
         "within reach": [{"reading": "15 minutes", "source": "convention"}, {"reading": "10 minutes", "source": "analyst"}]}
OPERATORS = [
    {"id": "need-evacuation", "label": "too few out of the hazard's reach within 30 minutes", "kind": "need", "quantity": "evacuation_share_30min",
     "holds_when": "<=", "standard": "target-90", "terms": ["leaving"], "scale": "absolute"},
    {"id": "asset-refuges", "label": "refuges within reach of most people", "kind": "asset", "quantity": "refuge_reach_share", "holds_when": ">=",
     "standard": 0.7, "terms": ["within reach"]},
]
STANDARDS = [{"id": "target-90", "value": 0.9, "held_by": "an invented city's plan"}]
PRACTICE = [{"id": "practice-drills", "title": "designated sites, warnings and drills", "answers": ["need-evacuation"], "source": "an invented Japanese city"}]


def main() -> int:
    if not steering.ENGINE.status()["built"]:
        emit({"kind": "skip", "name": "steering: the engine is not built (python applications/urban-studio/engine.py build engagement)"})
        return 0
    try:
        t = steering.open_engagement(by="an analyst")
    except EngineError as error:
        assert_("an engagement opens", False, error)
        return 0
    v = steering.ask("verify", trajectory=t)
    assert_("an engagement opens on the standard architecture with its first values, and verifies", v["holds"], v)

    t = steering.act(t, "observe", block="Collection", turn=0, by="the project's ledger", role="analyst", outputs=[ITEMS], reason="four items of evidence")["trajectory"]
    t = steering.act(t, "fill", block="Constitution", turn=0, by="an analyst", role="analyst", outputs=[TERMS, OPERATORS, STANDARDS, []], reason="declared")["trajectory"]
    t = steering.act(t, "fill", block="Proposals", turn=0, by="a resident", role="resident", outputs=[[]])["trajectory"]
    t = steering.act(t, "observe", block="Practice", turn=0, by="records of practice", role="analyst", outputs=[PRACTICE])["trajectory"]
    r = steering.turn(t, 0)
    t = r["trajectory"]
    assert_("a turn is run by the predefined methods with nothing waiting", not r["progress"]["waiting"] and len(r["progress"]["made"]) >= 7, r["progress"])

    view = steering.ask("view", trajectory=t)
    evals = {f["operator"]: f for f in block(view, "main", 0, "Evaluation")["values"]["evaluations"]}
    need, asset = evals["need-evacuation"], evals["asset-refuges"]
    assert_("the need is arguable, turning on how leaving is read: it holds as in 2011 and fails if people leave at once",
            need["standing"] == "arguable" and need["turns_on"] == ["leaving"] and {x["status"] for x in need["by_reading"]} == {"holds", "fails"}, need)
    assert_("the asset is arguable, turning on what within reach means", asset["standing"] == "arguable" and asset["turns_on"] == ["within reach"], asset)
    assert_("the standard says who holds it", need.get("held_by") == "an invented city's plan", need)
    pol = block(view, "main", 0, "PolicyMaking")["values"]["policy"]
    assert_("the need is taken up and the practice that answers it is a candidate; what is open is said",
            [x["operator"] for x in pol["taken_up"]] == ["need-evacuation"] and pol["candidates"][0]["id"] == "practice-drills" and len(pol["open"]) == 2, pol)
    dec = block(view, "main", 0, "Decision")["values"]
    assert_("with nothing chosen the decision advises nothing and inquires into the open terms", dec["intervention"]["advice"] is None and len(dec["inquiry"]["ask"]) == 2, dec)

    # an analyst chooses: the policy version amended, and the turn run again
    pm = block(view, "main", 0, "PolicyMaking")
    chosen = dict(pol, chosen="practice-drills")
    a = steering.act(t, "amend", entry=pm["entry"], by="an analyst", role="analyst", outputs=[chosen], reason="the deep dive found the support factors met")
    t = a["trajectory"]
    r = steering.turn(t, 0, start=0)
    t = r["trajectory"]
    view = steering.ask("view", trajectory=t)
    dec = block(view, "main", 0, "Decision")["values"]
    assert_("the analyst's choice is kept beside what was computed, and reaches the decision", a["outcome"]["holds"] and dec["intervention"]["advice"] == "practice-drills", dec)
    guest = steering.act(t, "fill", block="Policy", turn=0, by="a visitor", role="guest", outputs=[{"advice": "nothing"}, {}])
    assert_("a role not empowered enters a proposal, which the view lists and the decision ignores",
            guest["outcome"]["undecided"] and block(steering.ask("view", trajectory=guest["trajectory"]), "main", 0, "Policy").get("proposals"), guest["outcome"])

    # a branch re-reads leaving as at once only
    b = steering.act(t, "branch", **{"from": "main"}, turn=0, name="leaving at once", reason="if people left at once", by="an analyst")
    t = b["trajectory"]
    readings = copy.deepcopy(TERMS)
    readings["leaving"] = [{"reading": "at once", "source": "analyst"}]
    t = steering.act(t, "fill", block="Interpretation", turn=0, branch="leaving at once", by="an analyst", role="analyst", outputs=[readings])["trajectory"]
    t = steering.turn(t, 0, branch="leaving at once")["trajectory"]
    vb = steering.ask("view", trajectory=t, branch="leaving at once")
    nb = {f["operator"]: f for f in block(vb, "leaving at once", 0, "Evaluation")["values"]["evaluations"]}["need-evacuation"]
    changes = steering.ask("diff", trajectory=t, a={"branch": "main", "turn": 0}, b={"branch": "leaving at once", "turn": 0})["changes"]
    assert_("on the branch the need is untenable, and the main line is unchanged", nb["standing"] == "untenable" and any(c["block"] == "Evaluation" for c in changes)
            and {f["operator"]: f for f in block(steering.ask("view", trajectory=t), "main", 0, "Evaluation")["values"]["evaluations"]}["need-evacuation"]["standing"] == "arguable", nb)

    rp = steering.ask("replay", trajectory=t)["replayed"]
    assert_("every value a predefined method made replays", rp and all(x["verdict"]["holds"] for x in rp), [x for x in rp if not x["verdict"]["holds"]][:2])
    resting = steering.ask("resting", trajectory=t, agent="an analyst")["resting"]
    assert_("what rests on the analyst's acts is found, the decision among it", any(x["block"] == "Decision" for x in resting))
    prov = steering.ask("prov", trajectory=t)
    assert_("the record is W3C PROV-JSON with entities, activities and agents", prov["entity"] and prov["activity"] and "an analyst" in prov["agent"])
    altered = copy.deepcopy(t)
    altered["entries"][3]["reason"] = "rewritten"
    try:
        steering.ask("view", trajectory=altered)
        refused = False
    except EngineError as error:
        refused = "does not verify" in str(error)
    assert_("an altered trajectory is refused when read again", refused)

    tr = steering.ask("transfer", actions=["prepare", "none"], practice="prepare", tolerance=0.1,
                      source={"name": "an invented Japanese city", "components": ["refuges within reach", "people leave when warned"],
                              "laws": {"prepare": [{"when": {"refuges within reach": True, "people leave when warned": True}, "p": 0.95}, {"p": 0.05}], "none": [{"p": 0.05}]}},
                      target={"name": "an invented client city", "components": ["refuges within reach", "people leave when warned"],
                              "laws": {"prepare": [{"when": {"refuges within reach": True, "people leave when warned": True}, "p": 0.9},
                                                   {"when": {"refuges within reach": False, "people leave when warned": True}, "p": 0.5}, {"p": 0.05}],
                                       "none": [{"p": 0.05}]}},
                      readings=[{"name": "within 15 minutes", "source": "convention", "probabilities": {"refuges within reach": 1.0, "people leave when warned": 0.6}},
                                {"name": "within 10 minutes", "source": "analyst", "probabilities": {"refuges within reach": 0.5, "people leave when warned": 0.6}}],
                      factor_sources={"refuges within reach": "computed", "people leave when warned": "declared"})
    refuges = next(f for f in tr["factors"] if f["name"] == "refuges within reach")
    # within 10 minutes the square commutes where refuges are within reach (0.5) and where they are far and people do not leave, since both
    # cities then fail alike (0.5 × 0.4): 0.7; within 15 minutes, where the target is believed to be: 1
    assert_("a transfer by components: arguable over the readings, probability in [0.7, 1], refuges within reach sufficient for the square",
            tr["standing"] == "arguable" and refuges["sufficient"] and not refuges["necessary"] and abs(tr["low"] - 0.7) < 1e-9 and abs(tr["high"] - 1.0) < 1e-9, tr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
