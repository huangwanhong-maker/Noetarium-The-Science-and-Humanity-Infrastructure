"""Checks of Koinonia (ADR 0169), run by applications/koinonia/tests/run-tests.mjs. Nothing here asks a model: every model agent asks a
stand-in, and the ledger is kept in memory. One JSON object a line.

The two communities: the commons and the guild founded on their presets, nine agents lent under mandates, the catalogue's 48 entries
checked twice by agents of different origins and every disagreement settled by the guild and carried back; the applicant admitted
after two accepted trials; a fault in a record found by replay. The guard: a call that could pass the stop is refused before it is
made, and an agent with nothing to act on asks nothing. The triangle: under bilateral credit the circle of wants stalls, under
transferable credit a claim is passed on, under cleared credit a cycle of debts is cleared, under gift and provision nothing is owed,
and every arrangement but bilateral credit finishes the work.

    python applications/koinonia/tests/check_koinonia.py
"""

from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

APP = Path(__file__).resolve().parents[1]
ROOT = APP.parents[1]
sys.path[:0] = [str(APP), str(ROOT / "api" / "python")]

from koinonia import agents, cases, circulation  # noqa: E402
from koinonia.budget import Exhausted, Ledger  # noqa: E402
from koinonia.society import Society  # noqa: E402
from science import communities  # noqa: E402
from science.engines import toolchain  # noqa: E402
from science.models import Stub  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "koinonia: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:600]})


# ---- without the engine ------------------------------------------------------------------------------------------------------------
ledger = Ledger(None, budget_usd=1.0, stop_at_usd=0.01)
try:
    ledger.reserve("claude-opus-5-5", 4000, 3000)
    assert_("a call that could pass the stop is refused before it is made", False)
except Exhausted as why:
    assert_("a call that could pass the stop is refused before it is made", "allowed is spent" in str(why), why)
try:
    ledger.reserve("a-model-of-no-price", 10, 10)
    assert_("a model with no known price cannot be bounded and is refused", False)
except Exhausted:
    assert_("a model with no known price cannot be bounded and is refused", True)
assert_("cost is counted from the tokens reported: 1,000 in and 500 out of Opus 5.5 cost $0.014",
        abs(__import__("koinonia.budget", fromlist=["cost"]).cost({"input_tokens": 1000, "output_tokens": 500}, "claude-opus-5-5") - 0.014) < 1e-9)
assert_("cycles of debts are found once each, whatever order the debts come in",
        circulation.cycles([{"debtor": "a", "creditor": "b"}, {"debtor": "b", "creditor": "c"}, {"debtor": "c", "creditor": "a"}, {"debtor": "a", "creditor": "b"}])
        == [[("a", "b"), ("b", "c"), ("c", "a")]])

if not toolchain():
    emit({"kind": "skip", "name": "koinonia: societies (no C++ compiler here for the API's engine)"})
    sys.exit(0)
communities.ENGINE.keep(1)


def society(name: str, build, **options) -> tuple[Society, Ledger]:
    s, led = Society(name), Ledger(None)
    calls = []

    def make(agent, n):
        calls.append(agent["id"])
        return Stub(cases.stand_in)
    s.asker = agents.Asker(led, make)
    s.calls = calls
    build(s, **options)
    return s, led


def play(s: Society, rounds: int) -> int:
    for _ in range(rounds):
        s.round()
        if s.work.counts()["unchecked"] == 0 and all(d["status"] == "carried" for d in s.work.disputes):
            break
    return s.turn


# ---- two communities ---------------------------------------------------------------------------------------------------------------
s, _ = society("check-two", cases.two)
assert_("the commons and the guild are founded on their presets and nine agents are lent under mandates",
        set(s.communities) == {"commons", "guild"} and len(s.agents) == 9 and all(c.result.get("head", {}).get("size") for c in s.communities.values()))
n = play(s, 40)
w = s.work.counts()
assert_("all 48 entries are settled within forty rounds, each by two checks of different origins or by the guild", w["unchecked"] == 0 and w["open"] == 0 and n <= 40, (n, w))
loom = s.agents["loom/checker"]["tally"]
assert_("each agent's record of what it did is kept round by round: offered, accepted", loom["offered"] > 0 and 0 < loom["accepted"] <= loom["offered"], loom)
settled_apart = all(len({v["origin"] for v in f["verdicts"]}) == 2 for f in s.work.findings.values())
assert_("every finding rests on two checks of different origins", settled_apart)
assert_("every disagreement is adjudicated in the guild and carried back into the commons' record",
        s.work.disputes and all(d["status"] == "carried" for d in s.work.disputes) and
        sum(1 for a in s.communities["commons"].record if a["kind"] == "assess") >= len(s.work.disputes), [d["status"] for d in s.work.disputes])
assert_("the applicant is made a member of the guild after two trials accepted", s.communities["guild"].holds("quarry-works", "member"))
assert_("the checkers' rules found faults in the catalogue's entries", w["fault"] > 0, w)
c = s.communities["commons"]
again = communities.run(c.constitution, s.engine_parties(), c.record, seals=c.result["seals"])
assert_("the commons' record replays as it was sealed", again.get("replays") is True)
altered = copy.deepcopy(c.record)
k = next(i for i, a in enumerate(altered) if a["kind"] == "offer")
altered[k]["fields"]["amount"] = 999
moved = communities.run(c.constitution, s.engine_parties(), altered, seals=c.result["seals"])
assert_("an act altered after the fact is found by its place in the record", moved.get("replays") is False and moved.get("differs_at") == k + 1, moved.get("differs_at"))
before = len(s.calls)
quiet = Society("check-quiet")
quiet.asker = s.asker
cases.two(quiet)
for a in quiet.agents.values():
    a["paused"] = a["role"] != "checker" or a["policy"] != "model"
for _ in range(8):  # past several heartbeats: the records stop growing once the agents have joined, and nothing is asked
    quiet.round()
assert_("model agents with nothing new in their communities ask nothing of their model, heartbeat or not", len(s.calls) - before <= 2, len(s.calls) - before)

# ---- every behaviour recorded ----------------------------------------------------------------------------------------------------
from koinonia import behaviour  # noqa: E402

kinds = {e["what"] for e in s.behaviour.tail}
assert_("the behaviour journal records wakes with their reasons, decisions, every call to a model, every act and every refusal",
        {"wake", "decide", "ask", "act", "instructions"} <= kinds and all(e["data"].get("why") for e in s.behaviour.tail if e["what"] == "wake"), sorted(kinds))
asks = [e for e in s.behaviour.tail if e["what"] == "ask"]
assert_("each call to a model is recorded with its prompt, its answer and the digest of its instructions",
        asks and all(e["data"].get("prompt") and e["data"].get("instructions") and "answer" in e["data"] for e in asks))
entries = list(s.behaviour.tail)
assert_("the journal's seals hold, recomputed from its entries", behaviour.verify(entries)["holds"] is True)
tampered = copy.deepcopy(entries)
k = len(tampered) // 2
tampered[k]["data"]["why"] = "something it never said"
v = behaviour.verify(tampered)
assert_("an entry changed after the fact is found, by its number", v["holds"] is False and v["first fault"] == tampered[k]["n"], v)
dropped = entries[:k] + entries[k + 1:]
assert_("an entry removed is found", behaviour.verify(dropped)["holds"] is False)

# ---- persons take part ---------------------------------------------------------------------------------------------------------
d = next(x for x in s.work.disputes if x["status"] == "carried")
model_verdict = d["adjudication"]["verdict"]
other = next(v for v in ("holds", "fault", "doubt") if v != model_verdict)
s.submit("guild", [{"author": "elm", "kind": "assess", "turn": s.turn, "_via": "page",
                    "fields": {"object": d["entry"], "criterion": "adjudication", "dispute": d["id"], "value": other, "why": "read against the definition"}}])
agents.bookkeeping(s)
assert_("a person's adjudication from the page takes precedence over a model's and is recorded as a person's",
        d["adjudication"]["verdict"] == other and d["adjudication"]["grade"] == "a person" and d["status"] == "adjudicated")
s.round()
s.round()
assert_("the person's finding is carried back into the commons' record", d["status"] == "carried" and s.work.findings[d["entry"]]["finding"] == other, d["status"])
page = [e for e in s.behaviour.tail if e["what"] in ("act", "refused") and e["data"].get("via") == "page"]
assert_("a person's act is recorded in the journal as taken from the page", page and page[-1]["who"] == "elm")
s.submit("commons", [{"author": "orchard-lab", "kind": "revoke", "turn": s.turn, "_via": "page", "fields": {"which": "orchard/checker@commons"}}])
assert_("a sponsor revokes its agent's mandate through the community's rules, and the agent no longer wakes",
        agents.wakes(s, s.agents["orchard/checker"]) is None and any("revoked" in n["text"] for n in s.agents["orchard/checker"]["memory"]["notes"]))
pw = agents.powers(s, "elm")
assert_("a person's powers are read from the constitution: Elm, a reviewer of the guild, may assess there", "assess" in pw["guild"]["kinds"] and "reviewer" in pw["guild"]["positions"])

# ---- the triangle ----------------------------------------------------------------------------------------------------------------
results = {}
for regime in circulation.ARRANGEMENTS:
    t, _ = society(f"check-three-{regime}", cases.three, regime=regime)
    idle = 0
    for _ in range(60):
        r = t.round()
        idle = idle + 1 if r["acts"] == 0 else 0
        if (t.work.counts()["unchecked"] == 0 and all(d["status"] == "carried" for d in t.work.disputes)) or idle >= 6:
            break
    left = sum(1 for b in t.work.batches if not b.get("settled"))
    results[regime] = (left, circulation.account(t))
acc = {k: v[1] for k, v in results.items()}
assert_("under bilateral credit the circle of wants stalls: work is left and services are withheld at the limit",
        results["credit"][0] > 0 and sum(acc["credit"]["withheld"].values()) > 0, results["credit"])
assert_("under transferable credit a claim on a third community is passed on in payment, and the work is finished",
        results["transferable credit"][0] == 0 and acc["transferable credit"]["assignments"] > 0, results["transferable credit"])
assert_("under cleared credit cycles of debts are cleared on the league's record with no claim passed on, and the work is finished",
        results["cleared credit"][0] == 0 and acc["cleared credit"]["cleared around cycles"] > 0 and acc["cleared credit"]["assignments"] == 0, results["cleared credit"])
assert_("under gift and under provision nothing is owed and the work is finished",
        all(results[r][0] == 0 and acc[r]["debts entered"] == 0 for r in ("gift", "provision")), {r: results[r] for r in ("gift", "provision")})

# ---- the league's books keep every net position (Propositions 2 and 3, on the running society) ------------------------------------
from collections import Counter  # noqa: E402


def net_from_debts(t) -> Counter:
    n = Counter()
    for d in circulation.debts(t):  # open debts; a repayment presented has been made, and waits only on its creditor's acceptance
        n[d["creditor"]] += 1
        n[d["debtor"]] -= 1
    return n


def net_from_services(t) -> Counter:
    n = Counter()
    for v in t.work.services.values():
        if v["state"] == "recorded":
            n[v["provider"]] += 1
            n[v["target"]] -= 1
    return n


broken = {}
for regime in ("credit", "transferable credit", "cleared credit"):
    for build, size in ((cases.three, 60), (cases.four, 96)):
        t, _ = society(f"check-books-{regime}-{build.__name__}", build, size=size, regime=regime)
        for _ in range(45):
            t.round()
            a, b = net_from_debts(t), net_from_services(t)
            if any(a.get(c, 0) != b.get(c, 0) for c in set(a) | set(b)):
                broken[f"{regime}/{build.__name__}"] = (t.turn, dict(a), dict(b))
                break
assert_("under every kind of credit, every round, the net positions the league's debts give equal those its recorded services give: "
        "set-off, clearing, repayment and assignment use each unit of debt once", not broken, broken)
t = society("check-notary", cases.four, size=72, regime="provision with duty")[0]
for _ in range(40):
    t.round()
acted_in = {}
for e in t.behaviour.tail:
    if e["what"] == "decide" and e["data"].get("acts"):
        acted_in.setdefault(e["turn"], set()).add(e["who"])
checkers = {a["id"]: a["home"] for a in t.agents.values() if a["role"] == "checker"}
over = [(a["turn"], a["fields"]["object"]) for a in t.communities["league"].record if a["kind"] == "attest"
        and int(a["fields"]["value"]) > sum(1 for x, h in checkers.items() if h == a["fields"]["object"] and x not in acted_in.get(a["turn"], set()))]
assert_("the notary never counts a checker that put something forward in the round it attests", not over and any(a["kind"] == "attest" for a in t.communities["league"].record), over[:3])
assert_("every withholding is recorded in the behaviour journal, with the arrangement that made it",
        sum(t.work.withheld.values()) == sum(1 for e in t.behaviour.tail if e["what"] == "withheld") and all(e["data"].get("arrangement") for e in t.behaviour.tail if e["what"] == "withheld"))
cut = list(t.behaviour.tail)[:-3]
assert_("a journal whose last entries were removed is found, against the head and count the society kept",
        behaviour.verify(cut)["holds"] is True and behaviour.verify(cut, head=t.behaviour.head, count=t.behaviour.n)["holds"] is False)

# ---- a free rider and a community that cannot repay ------------------------------------------------------------------------------
four = {}
for regime, rider in (("gift", "reacts"), ("credit", "reacts"), ("provision with duty", "reads standing")):
    t, _ = society(f"check-four-{regime}", cases.four, size=96, regime=regime, free_rider=rider)
    idle = 0
    for _ in range(90):
        r = t.round()
        idle = idle + 1 if r["acts"] == 0 else 0
        if idle >= 8 or all(b.get("settled") for b in t.work.batches):
            break
    left = {o: sum(1 for b in t.work.batches if b["owner"] == o and not b.get("settled")) for o in ("polity", "guild")}
    abroad = sum(1 for v in t.work.services.values() if v["provider"] == "cooperative")
    four[regime] = (left, abroad, t.communities["league"].holds("polity", "member"))
assert_("the polity is admitted to the league by its council, two thirds counted by community", all(v[2] for v in four.values()), four)
assert_("under gift the free rider never serves abroad and the polity's work is left", four["gift"][1] == 0 and four["gift"][0]["polity"] > 0, four["gift"])
assert_("under credit the free rider is compelled, and the polity, which cannot repay, is capped at the limit",
        four["credit"][1] > 0 and four["credit"][0]["polity"] > 0, four["credit"])
assert_("under provision with duty, a free rider that reads its standing in the record is compelled and the polity's work is all done",
        four["provision with duty"][1] > 0 and four["provision with duty"][0]["polity"] == 0, four["provision with duty"])
