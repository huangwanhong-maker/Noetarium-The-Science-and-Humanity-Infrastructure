"""Checks of evidence-based policy at the API's edge (science/policy.py and api/cpp/science/policy.hpp), run by
api/tests/run-tests.mjs. A table of studies read as a person pastes it, and a line that does not read named; a model's draft of a
theory written in the domain's form, read by the domain's reader and refused when it does not read (a stand-in: no model is
called); and, where a work has built the engine, a question worked through: the thirteen BCG trials pooled to metafor's published
values, graded step by step under a declared scheme, set on their claim of a theory whose first claim has no evidence; the effect
carried to a place through the trials' strata; an option's net present value held to A v N mu - C worked here from the discount
factors; the decision and its switching worth; an invented theory of two paths held to Wright's rule by hand; a body that cannot
be pooled said without stopping the rest; a scheme that does not read; rules of a body's own. One JSON object a line, as
check_client.py reports.

    python api/tests/check_policy.py
"""

from __future__ import annotations

import json
import math
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import models, policy  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "policy: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "policy: " + name})


def close(a, b, tol=1e-9) -> bool:
    return a is not None and b is not None and abs(a - b) <= tol * max(1.0, abs(b))


# ---- A table of studies ------------------------------------------------------------------------------------------------------------
got = policy.studies_from_csv("id,label,year,stratum,effect,se\na,North,2011,rural,0.09,0.05\nb,\"South, lower\",2014,urban,0.04,0.06\n\n")
assert_("a pasted table of effects is read as studies, a standard error squared into the variance, a quoted name kept whole, a blank line passed over",
        len(got) == 2 and got[0] == {"id": "a", "label": "North", "year": 2011.0, "stratum": "rural", "effect": 0.09, "variance": 0.05 ** 2} and got[1]["label"] == "South, lower", got)
got = policy.studies_from_csv("label,treated_events,treated_others,control_events,control_others\nAronson 1948,4,119,11,128\n")
assert_("a table of 2 x 2 counts is read as counts, and a study without an identifier takes its row's", got == [{"label": "Aronson 1948", "treated_events": 4.0, "treated_others": 119.0, "control_events": 11.0, "control_others": 128.0, "id": "1"}], got)
for text, want in (("effect,se\n0.1,wide\n", "line 2: se is a number"), ("effect\n0.1\n", "the header names effect and variance"), ("effect,se\n0.1\n", "line 2 has 1 cells"), ("effect,se\n", "a header and at least one row")):
    try:
        policy.studies_from_csv(text)
        said = ""
    except ValueError as error:
        said = str(error)
    assert_(f"a table that does not read is refused with its line: {want}", want in said, said)

# ---- A draft written in the domain's form ----------------------------------------------------------------------------------------------
DRAFT = {"name": "Cash for school", "nodes": [{"id": "Cash", "kind": "intervention", "text": "A  transfer to each\nfamily"}, {"id": "enrolment", "kind": "outcome", "text": "Children enrol"}],
         "links": [{"from": "cash", "to": "enrolment", "assumption": "fees keep children out", "support": ["a school within reach", " "], "sign": "+"}]}
text = policy.written(DRAFT, ["weak", "strong"])
assert_("a draft is written a line each as it was answered: names made words, a text's line breaks made spaces, an empty support factor left out, nothing else mended",
        text == "theory cash_for_school\nlevels weak strong\nnode cash intervention: A transfer to each family\nnode enrolment outcome: Children enrol\nlink cash -> enrolment: fees keep children out\nsupport cash -> enrolment: a school within reach\n", text)
try:
    policy.draft_theory("meals", models.Stub(DRAFT))
    said = ""
except models.Refusal as error:
    said = str(error)
assert_("a description too short to draft from is refused before any model is asked", "describe the intervention" in said, said)

# ---- The engine ----------------------------------------------------------------------------------------------------------------------
if not policy.ENGINE.status()["built"]:
    skip("the question worked through by the engine (not built: python applications/policy-studio/engine.py build)")
    sys.exit(0)

THEORY = """theory bcg
levels very_low low moderate high
node programme intervention: The district offers BCG vaccination to every newborn
node vaccinated output: Children are vaccinated
node cases outcome: Fewer children fall ill with tuberculosis
link programme -> vaccinated: clinics reach the births
support programme -> vaccinated: a cold chain to every clinic
link vaccinated -> cases: the vaccine protects those who receive it
support vaccinated -> cases: the vaccine's potency is kept
sign vaccinated -> cases: -
"""
read = policy.ask("theory", text=THEORY)
assert_("a theory is read from its written form: three states, two causal claims, each claim with its assumption, its support factors and the side it expects",
        read["ok"] and [n["id"] for n in read["theory"]["nodes"]] == ["programme", "vaccinated", "cases"] and read["theory"]["links"][1]["sign"] == -1
        and read["theory"]["links"][0]["support"] == ["a cold chain to every clinic"] and all(l["rank"] == -1 and "no evidence" in l["why"] for l in read["theory"]["links"]), read)
bad = policy.ask("theory", text=THEORY + "node what\nlink cases -> nowhere: a claim to a state there is not\n")
assert_("lines that do not read are each named with their number, and no theory is answered", not bad["ok"] and "theory" not in bad and any(e.startswith("line 11") for e in bad["errors"]) and len(bad["errors"]) >= 2, bad)
cyc = policy.ask("theory", text=THEORY + "link cases -> programme: fewer cases keep the programme funded\n")
assert_("a cycle is refused, and named", not cyc["ok"] and any("cycle" in e for e in cyc["errors"]), cyc)

accepted = policy.draft_theory("A cash transfer to each family, so that children enrol in school.", models.Stub(DRAFT), levels=["weak", "strong"])
assert_("a model's draft that reads is a proposal of grade C, with who drafted it, and carries no evidence: every claim stands as not shown",
        accepted["grade"] == "C" and accepted["status"] == "proposed" and accepted["by"] == "a stand-in, stub" and accepted["theory"]["levels"] == ["weak", "strong"]
        and all(l["rank"] == -1 and not l["evidence"]["given"] for l in accepted["theory"]["links"]) and "evidence" not in accepted["text"], accepted)
for wrong, want in (({**DRAFT, "links": DRAFT["links"] + [{"from": "enrolment", "to": "cash", "assumption": "x", "support": [], "sign": "+"}]}, "does not read"),
                    ({**DRAFT, "nodes": [{**n, "kind": "outcome"} for n in DRAFT["nodes"]]}, "names no intervention"), ({"name": "x", "nodes": DRAFT["nodes"], "links": []}, "no causal claim"), ("not an object", "names no intervention")):
    try:
        policy.draft_theory("A cash transfer to each family, so that children enrol in school.", models.Stub(wrong))
        said = ""
    except models.Refusal as error:
        said = str(error)
    assert_(f"a draft is refused, never repaired, when it {want}", want in said, said)

# The thirteen BCG trials (Colditz et al. 1994, as dat.bcg of the R package metadat): vaccinated with and without tuberculosis, not
# vaccinated with and without, and the absolute latitude of the trial.
BCG = [(4, 119, 11, 128, 44), (6, 300, 29, 274, 55), (3, 228, 11, 209, 42), (62, 13536, 248, 12619, 52), (33, 5036, 47, 5761, 13), (180, 1361, 372, 1079, 44), (8, 2537, 10, 619, 19),
       (505, 87886, 499, 87892, 13), (29, 7470, 45, 7232, 27), (17, 1699, 65, 1600, 42), (186, 50448, 141, 27197, 18), (5, 2493, 3, 2338, 33), (27, 16886, 29, 17825, 33)]
SCHEME = "scheme after_GRADE\nlevels very_low low moderate high\nstart randomised high\nstart observational low\ndown risk_of_bias 2\ndown inconsistency 2\ndown imprecision 2\ndown publication_bias 2\nup large_effect 2\n"
RULES = [{"factor": "imprecision", "reading": "crosses", "values": [0]}, {"factor": "inconsistency", "reading": "heterogeneous", "values": [0.5, 0.75]},
         {"factor": "publication_bias", "reading": "small_studies", "values": [0.1], "least": 10}, {"factor": "large_effect", "reading": "large", "values": [0.5]}]
COST = [400000] + [150000] * 10
QUESTION = {
    "schema": "noetarium.policy/1", "intervention": "programme", "outcome": "cases", "least": "moderate", "theory": THEORY, "scheme": SCHEME, "rules": RULES,
    "bodies": [{"id": "trials", "link": {"from": "vaccinated", "to": "cases"}, "claim": "BCG lowers the risk of tuberculosis", "design": "randomised", "measure": "log_risk_ratio", "higher_is_better": False,
                "assessed": {"risk_of_bias": 1},
                "studies": [{"id": str(k + 1), "stratum": "low" if lat < 30 else "high", "treated_events": a, "treated_others": b, "control_events": c, "control_others": d} for k, (a, b, c, d, lat) in enumerate(BCG)]}],
    "place": {"name": "the district", "same_principle": "stated", "factors": {"a cold chain to every clinic": "stated", "a factor no path needs": "against"}, "strata": {"names": ["low", "high"], "masses": [1, 0]}},
    "scale": {"kind": "risk_ratio", "baseline": 0.0015}, "discount": 0.03,
    "options": [{"name": "nothing", "effect": 0, "se": 0, "value": 4000, "population": 0, "cost": [0] * 11}, {"name": "vaccinate", "effect": "here", "value": 4000, "population": 200000, "cost": COST}],
    "decision": {"option": "vaccinate", "study": {"unit_variance": 0.006, "fixed_cost": 250000, "unit_cost": 12, "most": 200000, "delay": 0.94}, "worth": [1500, 12000]},
    "draws": 20000, "seed": 7,
}
r = policy.ask("appraise", question=QUESTION)
b = r["bodies"][0]
p = b["pooled"]
# metafor's rma(measure="RR", method="REML") on dat.bcg: estimate -0.7145, se 0.1798, tau^2 0.3132, Q(12) = 152.233. (Its I^2 is the
# share of tau^2 in tau^2 plus a typical sampling variance, 92.22%; peira's is Higgins and Thompson's (Q - df)/Q, which is held here.)
assert_("the thirteen BCG trials pool to metafor's published values: a log risk ratio of -0.7145 (se 0.1798), tau^2 0.3132, Q = 152.233 on 12 degrees of freedom, and I^2 = (Q - 12)/Q",
        abs(p["estimate"] + 0.7145) < 5e-5 and abs(p["se"] - 0.1798) < 5e-5 and abs(p["tau2"] - 0.3132) < 5e-5 and abs(p["q"] - 152.233) < 1e-3 and close(p["i2"], (p["q"] - 12) / p["q"], 1e-9) and p["k"] == 13, p)
s1 = b["studies"][0]
assert_("a study's effect is computed from its counts: log((4/123)/(11/139)) with variance 1/4 - 1/123 + 1/11 - 1/139, its interval and its weight beside it",
        close(s1["effect"], math.log((4 / 123) / (11 / 139))) and close(s1["variance"], 1 / 4 - 1 / 123 + 1 / 11 - 1 / 139) and close(s1["upper"] - s1["lower"], 2 * 1.959963984540054 * math.sqrt(s1["variance"]), 1e-7)
        and close(sum(s["weight"] for s in b["studies"]), 1.0, 1e-9), s1)
assert_("graded step by step under the declared scheme: it starts high, inconsistency lowers it two steps, the large effect raises it one, the reader's risk of bias lowers it one, to low",
        b["grade"] == "low" and b["steps"][0] == "a body of randomised evidence starts at high" and sum(" lowers it " in s or " raises it " in s for s in b["steps"]) == 3
        and any(s.startswith("risk_of_bias lowers it 1 step, from moderate to low") for s in b["steps"]), b["steps"])
low = next(z for z in b["strata"] if z["name"] == "low")
assert_("each stratum the studies name is pooled, and the pool is given with each study left out", [z["k"] for z in b["strata"]] == [8, 5] and len(b["leave_one_out"]) == 13 and low["effect"] > p["estimate"], b["strata"])
sch = r["scheme"]
assert_("the scheme is answered with where each design starts and its factors, those a rule reads told from those a reader assesses",
        sch["start"] == {"observational": "low", "randomised": "high"} and {f["name"]: f["read_by_rule"] for f in sch["factors"]}["risk_of_bias"] is False and {f["name"]: f["read_by_rule"] for f in sch["factors"]}["inconsistency"] is True, sch)
th, a = r["theory"], r["assessment"]
assert_("the evidence is set on its claim, shown at low; the first claim has none; so the outcome is not yet shown to follow, the gap on every path named",
        th["ok"] and th["bodies"] == {"vaccinated -> cases": "trials"} and th["links"][1]["level"] == "low" and th["links"][0]["rank"] == -1 and a["verdict"]["status"] == "undecided"
        and a["paths"] == 1 and a["gaps"] == [0] and a["strongest"]["limiting"] == 0 and a["strongest"]["weakest"] is None, a)
h = r["here"]
assert_("the effect is carried to the place through the strata: with all its mass in the low stratum, the effect here is that stratum's, weaker than the pool",
        h["transported"] and close(h["effect"], low["effect"], 1e-9) and h["effect"] > h["there"]["effect"] and close(h["there"]["effect"], p["estimate"]), h)
assert_("the argument for the place: it worked there and the principle is stated; of the path's two support factors one is stated and one unknown, so it is not complete; a factor no path needs is not asked",
        h["worked_there"] and h["verdict"]["status"] == "undecided" and h["present"] == ["a cold chain to every clinic"] and h["unknown"] == ["the vaccine's potency is kept"]
        and [f["links"] for f in h["factors"]] == [[0], [1]] and "a factor no path needs" not in h["needed"], h)
u = r["unit"]
z = 1.959963984540054
se_here = (h["upper"] - h["lower"]) / (2 * z)
assert_("on a baseline risk the log risk ratio becomes events averted a unit and year, b (1 - e^theta), with the delta method's standard error b e^theta se",
        close(u["effect"], 0.0015 * (1 - math.exp(h["effect"])), 1e-12) and close(u["se"], 0.0015 * math.exp(h["effect"]) * se_here, 1e-9), u)
A = sum(1.03 ** -t for t in range(1, 11))
C = 400000 + 150000 * A
o = next(x for x in r["options"]["appraised"] if x["name"] == "vaccinate")
assert_("the option's net present value is A v N mu - C, from the discount factors of years 1 to 10 at 3%: its mean, its standard deviation |A v N| sigma, its costs and its switching worth C / (A N mu)",
        close(o["costs"], C, 1e-9) and close(o["mean"], A * 4000 * 200000 * u["effect"] - C, 1e-9) and close(o["sd"], A * 4000 * 200000 * u["se"], 1e-9) and close(o["switching"], C / (A * 200000 * u["effect"]), 1e-9), o)
cmp_ = r["options"]
phi = 0.5 * (1 + math.erf((o["mean"] / o["sd"]) / math.sqrt(2)))
assert_("against doing nothing it has the greater expected value, and is best on about Phi(mu / sigma) of the draws", cmp_["chosen"] == ["vaccinate"] and abs(cmp_["best"][1] - phi) < 0.01 and abs(sum(cmp_["best"]) - 1) < 1e-9, [cmp_["best"], phi])
d = r["decision"]
loss = o["sd"] * (math.exp(-0.5 * (o["mean"] / o["sd"]) ** 2) / math.sqrt(2 * math.pi) - (o["mean"] / o["sd"]) * (1 - phi))
assert_("the decision is to adopt: knowing the effect first would be worth the unit normal loss sigma L(mu / sigma), which the draws agree with, and less than any study considered costs",
        d["chosen"] == ["adopt"] and close(d["adopt"], o["mean"], 1e-9) and d["reject"] == 0 and close(d["evpi"], loss, 1e-6) and abs(cmp_["evpi"] - loss) < 4 * cmp_["evpi_error"] and d["study"] < d["adopt"], [d["evpi"], loss, cmp_["evpi"]])
assert_("over the bounds on a unit's worth the decision switches, which is said, with the choice by the worst case", d["robust"] is False and d["maximin"] == "reject" and 1500 < o["switching"] < 12000, d["verdict"])
curve = d["curve"]
assert_("the worth of a study is given over its size: its cost fixed plus a unit's each, its expected gain rising toward the value of perfect information and never past it",
        curve[0]["n"] == 1 and curve[-1]["n"] == 200000 and all(close(c["cost"], 250000 + 12 * c["n"]) for c in curve) and all(x["evsi"] <= y["evsi"] + 1e-6 for x, y in zip(curve, curve[1:])) and curve[-1]["evsi"] < d["evpi"], curve[-1])

# A body that cannot be pooled is said, and the rest is still answered; a scheme that does not read grades nothing.
broken = json.loads(json.dumps(QUESTION))
broken["bodies"].append({"id": "empty", "link": {"from": "programme", "to": "vaccinated"}, "design": "randomised", "measure": "as_given", "studies": [{"id": "x", "effect": 0.2, "variance": 0}]})
broken["bodies"].append({"id": "elsewhere", "link": {"from": "cases", "to": "nowhere"}, "design": "observational", "measure": "as_given", "studies": [{"id": "y", "effect": 0.2, "se": 0.05}]})
rb = policy.ask("appraise", question=broken)
by = {x["id"]: x for x in rb["bodies"]}
assert_("a study with no variance stops its own body, with the reason, and no other; a body set on a claim the theory lacks is named among the faults",
        "positive variance" in by["empty"].get("error", "") and by["trials"]["grade"] == "low" and by["elsewhere"]["grade"] == "low" and [f["body"] for f in rb["theory"]["faults"]] == ["elsewhere"] and rb["decision"]["chosen"] == ["adopt"], by["empty"])
unread = policy.ask("appraise", question={**QUESTION, "scheme": SCHEME + "sideways bias 1\n"})
assert_("a scheme that does not read is said with its line, and no body is graded by it", "error" in unread["scheme"] and "error" in unread["bodies"][0] and unread["theory"]["links"][1]["rank"] == -1, unread["scheme"])
own = json.loads(json.dumps(QUESTION))
own["bodies"][0]["rules"] = [r_ for r_ in RULES if r_["factor"] != "large_effect"]
assert_("a body may state rules of its own: without the rule for a large effect, the same trials are graded very low", policy.ask("appraise", question=own)["bodies"][0]["grade"] == "very_low")

# An invented theory of two paths, each claim with an effect as given: the strongest path, and Wright's rule by hand.
TWO = """theory two_paths
levels very_low low moderate high
node meals intervention: A free meal
node attendance outcome: Pupils attend
node nutrition outcome: Pupils are nourished
node learning impact: Pupils learn
link meals -> attendance: families send them
link attendance -> learning: days are taught
link meals -> nutrition: the meal adds
link nutrition -> learning: the fed attend to lessons
"""


def body(ident, a, b_, design, effect, se):
    return {"id": ident, "link": {"from": a, "to": b_} if a else {}, "design": design, "measure": "as_given", "studies": [{"id": ident, "effect": effect, "se": se}]}


two = {"intervention": "meals", "outcome": "learning", "least": "moderate", "theory": TWO, "scheme": SCHEME, "rules": RULES[:1],
       "bodies": [body("ma", "meals", "attendance", "randomised", 6.0, 1.0), body("al", "attendance", "learning", "randomised", 0.01, 0.002), body("mn", "meals", "nutrition", "randomised", 0.2, 0.05),
                  body("nl", "nutrition", "learning", "observational", 0.3, 0.1), body("direct", "", "", "randomised", 0.08, 0.02)],
       "place": {"name": "here", "same_principle": "stated", "factors": {}}, "discount": 0.035,
       "options": [{"name": "meals", "effect": "here", "value": 900, "population": 12000, "cost": [720000] * 6}]}
rt = policy.ask("appraise", question=two)
at = rt["assessment"]
lo, hi = (6 - z) * (0.01 - z * 0.002) + (0.2 - z * 0.05) * (0.3 - z * 0.1), (6 + z) * (0.01 + z * 0.002) + (0.2 + z * 0.05) * (0.3 + z * 0.1)
assert_("of two paths the strongest is the one whose weakest claim is best shown: through attendance at high, the other held to low by its observational claim; so the outcome is shown at moderate or better",
        at["paths"] == 2 and at["strongest"]["links"] == [0, 1] and at["strongest"]["weakest"] == "high" and at["verdict"]["status"] == "holds" and at["necessary"] == [] and rt["theory"]["links"][3]["level"] == "low", at)
assert_("every claim stating a linear effect, Wright's rule gives the effect implied on the outcome, 6 x 0.01 + 0.2 x 0.3 = 0.12, with the enclosure of the intervals' products",
        at["linear"] and not at["shared"] and close(at["implied"]["effect"], 0.12, 1e-12) and close(at["implied"]["lower"], lo, 1e-9) and close(at["implied"]["upper"], hi, 1e-9), at["implied"])
ht = rt["here"]
assert_("the effect found there is taken from the trials of the whole intervention, set on no claim, before any body that only bears on the outcome; with no strata it is carried over as it stands",
        ht["body"] == "direct" and not ht["transported"] and close(ht["effect"], 0.08) and ht["verdict"]["status"] == "holds" and ht["factors"] == [], ht)
assert_("one option alone is appraised and not compared; the decision on it is still taken against doing nothing",
        "note" in rt["options"] and len(rt["options"]["appraised"]) == 1 and rt["decision"]["option"] == "meals" and "study" in rt["decision"] and rt["decision"]["study"] is None, rt["options"])
try:
    policy.ask("nothing")
    said = ""
except policy.EngineError as error:
    said = str(error)
assert_("an operation there is not is refused with those there are", "theory, appraise" in said, said)
