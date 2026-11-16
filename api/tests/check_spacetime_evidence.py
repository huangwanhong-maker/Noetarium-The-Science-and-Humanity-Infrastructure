"""Checks of evidence in space and time at the API's edge (science/spacetime_evidence.py and api/cpp/science/spacetime_evidence.hpp),
run by api/tests/run-tests.mjs. Written times read as the page reads them; a pasted table of items read and one that does not read
refused; an archive prepared with its holdings' digests observed from their files; and, where the Urban Studio has built the engine,
a small archive worked through by hand: custody followed and a file altered found, the items resting on it not counted; an item's
share of a claim's place by the area two discs share; reports of events of their own multiplied, and of one event settling it; a
household's repeated complaints counted once, with what counting them apart would have given; a proposal left out; the decision and
the looks bought within a budget against every subset; a timeline in conflict; a channel counted with tyche's intervals. One JSON
object a line, as check_client.py reports.

    python api/tests/check_spacetime_evidence.py
"""

from __future__ import annotations

import hashlib
import itertools
import json
import math
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import spacetime_evidence as se  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "evidence in space and time: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


def skip(name: str) -> None:
    emit({"kind": "skip", "name": "evidence in space and time: " + name})


def close(a, b, tol=1e-9) -> bool:
    return a is not None and b is not None and abs(a - b) <= tol * max(1.0, abs(b))


# ---- Times and tables ---------------------------------------------------------------------------------------------------------------
day = se.span("2024-04-03")
assert_("a written time is the interval it covers: a date its day, a month its month, a year its year; an instant without a zone is UTC",
        day[1] - day[0] == 86400 and se.span("2024-02")[1] - se.span("2024-02")[0] == 29 * 86400 and se.span("2024")[1] - se.span("2024")[0] == 366 * 86400
        and se.instant("2024-04-03T10:00:00") == se.instant("2024-04-03T06:00:00-04:00") and se.span(None) is None, day)
assert_("from the beginning of one written time to the end of another", se.interval("2024-04", "2024-06") == (se.span("2024-04")[0], se.span("2024-06")[1]))
try:
    se.span("the spring")
    said = ""
except ValueError as error:
    said = str(error)
assert_("what is not a time is refused, and named", "'the spring' is not a time" in said, said)
rows = [{"id": "i1", "kind": "complaint", "lat": "40.7", "lon": "-73.9", "claims": "k1; k2", "note": ""}, {"id": "", "kind": ""}]
got = se.read_table(rows, "items")
assert_("a table's rows are read as items: numbers as numbers, several claims cut at semicolons, empty cells and empty rows left out",
        got == [{"id": "i1", "kind": "complaint", "lat": 40.7, "lon": -73.9, "claims": ["k1", "k2"]}], got)
for bad, want in (([{"id": "a", "lat": "north"}], "lat is a number"), ([{"kind": "x"}], "has no id"), ([{"id": "a"}, {"id": "a"}], "name a more than once")):
    try:
        se.read_table(bad, "items")
        said = ""
    except ValueError as error:
        said = str(error)
    assert_(f"a table that does not read is refused: {want}", want in said, said)

with tempfile.TemporaryDirectory() as folder:
    (Path(folder) / "note.txt").write_bytes(b"rats seen at the back\n")
    digest = hashlib.sha256(b"rats seen at the back\n").hexdigest()
    p = se.prepared({"items": [{"id": "i", "from": "2024-04", "lat": ""}], "holdings": [{"id": "h", "name": "note.txt"}, {"id": "g", "name": "gone.txt"}]}, folder)
    assert_("prepared: times made numbers, an empty cell left out, each holding's digest observed now from its file, a missing file's as nothing",
            p["items"][0]["from"] == se.span("2024-04")[0] and p["items"][0]["until"] == se.span("2024-04")[1] and "lat" not in p["items"][0]
            and p["holdings"][0]["observed"] == digest and p["holdings"][1]["observed"] == "", p)
    assert_("a file's digest is the SHA-256 of its bytes", se.digest(Path(folder) / "note.txt") == digest and se.digest(Path(folder) / "none") == "")

# ---- The engine ------------------------------------------------------------------------------------------------------------------------
if not se.ENGINE.status()["built"]:
    skip("an archive worked through by the engine (not built: python applications/urban-studio/engine.py build evidence)")
    sys.exit(0)

A_DIGEST, B_DIGEST = "a" * 64, "b" * 64
LAT, LON = 40.725, -73.985
M = 111320.0
north = lambda metres: LAT + metres / M                               # noqa: E731
INSPECTOR = {"id": "inspector", "hit": 0.9, "false_alarm": 0.02}
RESIDENT = {"id": "resident", "hit": 0.12, "false_alarm": 0.02}
CLAIM = {"id": "k", "text": "rats here", "lat": LAT, "lon": LON, "radius": 40, "from": "2024-04", "until": "2024-06", "prior": 0.5, "e_given_h": 0.25, "e_given_not_h": 0.0}
ARCHIVE = {
    "sources": [INSPECTOR, RESIDENT],
    "holdings": [
        {"id": "hA", "name": "a.json", "digest": A_DIGEST, "observed": A_DIGEST, "custodian": "the city", "since": "2024-07-01T00:00:00Z", "attributes": {"title": "inspections"},
         "transfers": [{"when": "2024-07-02T00:00:00Z", "from": "the city", "to": "the cache", "digest": A_DIGEST}]},
        {"id": "hB", "name": "b.json", "digest": B_DIGEST, "observed": "c" * 64, "custodian": "the city", "since": "2024-07-01T00:00:00Z", "attributes": {"title": "complaints"}, "transfers": []},
    ],
    "items": [
        {"id": "i1", "kind": "inspection", "holding": "hA", "lat": LAT, "lon": LON, "radius": 0, "from": "2024-05-02", "source": "inspector", "says": "holds"},
        {"id": "i2", "kind": "inspection", "holding": "hA", "lat": north(10), "lon": LON, "radius": 0, "from": "2024-05-03", "source": "inspector", "says": "denies"},
        {"id": "i3", "kind": "inspection", "holding": "hA", "lat": north(40), "lon": LON, "radius": 20, "from": "2024-05-04", "source": "inspector", "says": "holds"},
        {"id": "i4", "kind": "inspection", "holding": "hA", "lat": north(500), "lon": LON, "radius": 0, "from": "2024-05-05", "source": "inspector", "says": "holds"},
        {"id": "i5", "kind": "inspection", "holding": "hA", "lat": LAT, "lon": LON, "radius": 0, "from": "2024-08-01", "source": "inspector", "says": "holds"},
        {"id": "r1", "kind": "complaint", "lat": north(5), "lon": LON, "radius": 0, "from": "2024-04-10", "source": "resident", "origin": "address: 1 A", "says": "holds"},
        {"id": "r2", "kind": "complaint", "lat": north(5), "lon": LON, "radius": 0, "from": "2024-04-12", "source": "resident", "origin": "address: 1 A", "says": "holds"},
        {"id": "r3", "kind": "complaint", "lat": north(5), "lon": LON, "radius": 0, "from": "2024-04-14", "source": "resident", "origin": "address: 1 A", "says": "holds"},
        {"id": "r4", "kind": "complaint", "lat": north(-5), "lon": LON, "radius": 0, "from": "2024-04-15", "source": "resident", "says": "holds", "status": "proposed"},
        {"id": "r5", "kind": "complaint", "holding": "hB", "lat": north(-5), "lon": LON, "radius": 0, "from": "2024-04-16", "source": "resident", "says": "holds"},
    ],
    "claims": [CLAIM],
    "rule": {"established": 0.95, "refuted": 0.05},
}
r = se.ask("all", archive=se.prepared(ARCHIVE))
k = r["keep"]
hold = {h["id"]: h for h in k["holdings"]}
items = {i["id"]: i for i in k["items"]}
assert_("custody followed: the inspections' file intact, handed over once by its holder; the complaints' file altered since it was taken in, said so",
        hold["hA"]["fixity"] == "intact" and hold["hA"]["sound"] and hold["hB"]["fixity"] == "altered" and not hold["hB"]["sound"] and k["verdict"]["status"] == "fails", k["verdict"])
assert_("an item resting on an altered file does not count, with why; an item resting on no file rests on its source's word",
        not items["r5"]["sound"] and "changed" in items["r5"]["why"] and items["r1"]["sound"] and not items["r1"]["kept"], items["r5"])
c = r["weigh"]["claims"][0]
by = {i["id"]: i for i in c["items"]}
left = {l["id"]: l["why"] for l in c["left_out"]}
assert_("the items about the claim's place and time bear on it; one 500 m away and one in August do not; a proposal and the item on the altered file are left out, each with why",
        set(by) == {"i1", "i2", "i3", "r1", "r2", "r3"} and set(left) == {"r4", "r5"} and "proposal" in left["r4"] and "changed" in left["r5"], [sorted(by), left])
# i3: a disc of 20 m about a point 40 m from the centre of a disc of 40 m: the share of the small disc within the large.
# The engine's distance is the geodesic on WGS 84: along the meridian a degree here is (π/180) a (1 − e²) / (1 − e² sin² φ)^(3/2).
e2, phi = 0.00669437999014, math.radians(LAT + 20 / M)
degree = math.pi / 180 * 6378137.0 * (1 - e2) / (1 - e2 * math.sin(phi) ** 2) ** 1.5
a_, b_, d_ = 20.0, 40.0, 40.0 * degree / M
lens = a_ * a_ * math.acos((d_ * d_ + a_ * a_ - b_ * b_) / (2 * d_ * a_)) + b_ * b_ * math.acos((d_ * d_ + b_ * b_ - a_ * a_) / (2 * d_ * b_)) - 0.5 * math.sqrt((-d_ + a_ + b_) * (d_ + a_ - b_) * (d_ - a_ + b_) * (d_ + a_ + b_))
share = lens / (math.pi * a_ * a_)
assert_("an item good to 20 m, 39.9 m by the geodesic from the centre of a claim of 40 m, is about it by the area the two discs share over its own: 0.45",
        abs(by["i3"]["distance"] - d_) < 0.01 and abs(by["i3"]["space"] - share) < 1e-6 and by["i3"]["time"] == 1 and abs(by["i3"]["about"] - by["i3"]["space"]) < 1e-12, [by["i3"], share, d_])
# Events of their own: each factor [e' a + (1 − e') b] / [en a + (1 − en) b], with e' = q eh + (1 − q) en and en = 0.
eh = 0.25
f_found = lambda q: ((q * eh) * 0.9 + (1 - q * eh) * 0.02) / 0.02          # noqa: E731
f_clear = (eh * 0.1 + (1 - eh) * 0.98) / 0.98
yes, no = 0.12, 0.02                                                     # three complaints of one household: one registration
f_household = (eh * yes + (1 - eh) * no) / no
want = f_found(1) * f_clear * f_found(by["i3"]["about"]) * f_household
assert_("reports of events of their own multiply: two found, one clear, a household's three complaints once, by hand",
        close(c["ratio"], want, 1e-9) and len(c["groups"]) == 4, [c["ratio"], want])
apart = f_found(1) * f_clear * f_found(by["i3"]["about"]) * f_household ** 3
assert_("counted as three independent complaints the household would have weighed the claim's ratio times f³ / f, which is said", close(c["as_if_independent"], apart, 1e-9) and close(c["overstatement"], f_household ** 2, 1e-9), [c["as_if_independent"], apart])
post = want / (1 + want)
assert_("the posterior is the prior's odds times the ratio, and its standing by the declared rule", close(c["posterior"], post, 1e-12) and c["standing"] == ("established" if post >= 0.95 else "open"), [c["posterior"], c["standing"]])
one = se.ask("weigh", archive=se.prepared({**ARCHIVE, "claims": [{**CLAIM, "event": "one"}]}))["claims"][0]
q3 = by["i3"]["about"]
pa = 0.9 * 0.1 * (q3 * 0.9 + (1 - q3) * 0.02) * yes     # each group given the one event
pb = 0.02 * 0.98 * 0.02 * no                              # and given none
want_one = (eh * pa + (1 - eh) * pb) / pb
assert_("the same reports as reports of one event: [P(E | H) Π a + P(¬E | H) Π b] / Π b, by hand, which cannot exceed P(E | H) Π a / Π b + 1 however many agree",
        one["event"] == "one" and close(one["ratio"], want_one, 1e-9), [one["ratio"], want_one])
nothing = se.ask("weigh", archive=se.prepared({**ARCHIVE, "claims": [{**CLAIM, "lat": north(5000)}]}))["claims"][0]
assert_("a claim no item bears on stands at its prior, said so", nothing["standing"] == "no evidence" and nothing["posterior"] == 0.5 and nothing["ratio"] == 1)
tied = se.ask("weigh", archive=se.prepared({**ARCHIVE, "items": ARCHIVE["items"] + [{"id": "far", "kind": "inspection", "lat": north(3000), "lon": LON, "from": "2023-01-01", "source": "inspector", "says": "holds", "claims": ["k"]}]}))["claims"][0]
assert_("an item tied to a claim outright bears on it wherever and whenever it is", "far" in {i["id"] for i in tied["items"]})

# The decision, and the looks bought within a budget.
claims = [{**CLAIM, "id": f"k{n}", "lat": north(400 * n), "prior": p} for n, p in enumerate((0.05, 0.2, 0.4, 0.6, 0.9))]
decision = {"acts": [{"name": "treat", "holds": -1, "not": -1}, {"name": "leave", "holds": -6, "not": 0}], "inspection": {"name": "a survey", "sensitivity": 0.8, "specificity": 0.95, "cost": 0.3},
            "costs": {"k3": 0.6}, "budget": 0.7, "charged": True}
d = se.ask("decide", archive=se.prepared({**ARCHIVE, "items": [], "claims": claims, "decision": decision}))
dc = {x["claim"]: x for x in d["claims"]}
assert_("each claim's act is the one of greatest expected worth on its posterior: to leave below a sixth, to treat above",
        dc["k0"]["act"] == "leave" and all(dc[f"k{n}"]["act"] == "treat" for n in (1, 2, 3, 4)) and close(dc["k2"]["worth"], -1) and close(dc["k0"]["worth"], -6 * 0.05), dc["k0"])


def evsi(p, sens, spec):
    pf = p * sens + (1 - p) * (1 - spec)
    best = lambda q: max(-1.0, -6 * q)                                  # noqa: E731
    return pf * best(p * sens / pf) + (1 - pf) * best(p * (1 - sens) / (1 - pf)) - best(p)


worth = {f"k{n}": evsi(p, 0.8, 0.95) - (0.6 if n == 3 else 0.3) for n, p in enumerate((0.05, 0.2, 0.4, 0.6, 0.9))}
cost = {f"k{n}": 0.6 if n == 3 else 0.3 for n in range(5)}
best_set, best_gain = (), 0.0
for size in range(6):
    for s in itertools.combinations(sorted(worth), size):
        if sum(cost[x] for x in s) <= 0.7 + 1e-12 and sum(worth[x] for x in s) > best_gain + 1e-12:
            best_set, best_gain = s, sum(worth[x] for x in s)
assert_("each survey is worth its EVSI by preposterior analysis by hand, and the set bought within the budget is the best of every subset, costs charged",
        all(close(dc[x]["evsi"], evsi(p, 0.8, 0.95), 1e-9) for x, p in zip(sorted(worth), (0.05, 0.2, 0.4, 0.6, 0.9))) and tuple(sorted(d["inspect"])) == best_set and close(d["gained"], best_gain, 1e-9) and d["exact"],
        [d["inspect"], best_set, d["gained"], best_gain])
assert_("what would be done after a survey is said either way, with what one would then believe", all(x["after_found"] in ("treat", "leave") and x["after_clear"] in ("treat", "leave") for x in d["claims"]) and d["test"] == "a survey")

# A timeline, and a channel counted.
t = se.ask("timeline", archive={"unit": "minutes", "events": ["20:00", "left", "door"],
                                "statements": [{"id": "w", "source": "witness", "from": "20:00", "to": "left", "least": 85, "most": 95}, {"id": "c", "source": "camera", "from": "20:00", "to": "door", "least": 98, "most": 102},
                                               {"id": "m", "source": "map", "from": "left", "to": "door", "least": 30}]})
assert_("statements of time read as a simple temporal network: these cannot all be true, and the conflict names its statements", not t["consistent"] and set(t["conflict"]) == {"w", "c", "m"} and t["verdict"]["status"] == "fails", t)
rates = se.calibrate([{"reference": True, "reported": i < 26} for i in range(213)] + [{"reference": False, "reported": i < 22} for i in range(1217)])
assert_("a channel counted: 26 of 213 and 22 of 1217, each with Wilson's interval and the exact one about it, and the ratio of a report",
        close(rates["hit"]["rate"], 26 / 213) and rates["hit"]["wilson"][0] < 26 / 213 < rates["hit"]["wilson"][1] and rates["hit"]["exact"][0] < rates["hit"]["wilson"][0]
        and close(rates["ratio_of_a_report"], (26 / 213) / (22 / 1217)), rates["hit"])
try:
    se.ask("nothing", archive={})
    said = ""
except se.EngineError as error:
    said = str(error)
assert_("an operation there is not is refused with those there are", "keep, weigh, decide, timeline, all, rates" in said, said)
