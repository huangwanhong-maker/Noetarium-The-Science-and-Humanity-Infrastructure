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

# a provider that cannot be built: the reservation is released and the fault journaled (finding F19 of the reading for error)
failing = Ledger(None)


def unbuildable(agent, n):
    raise RuntimeError("no provider can be made for this agent")


lone = Society("check-asker")
lone_agent = {"id": "lone/agent", "model": "claude-opus-5-5", "memory": {}, "tally": {"calls": 0, "usd": 0.0, "tokens": 0}, "communities": {}}
said = agents.Asker(failing, unbuildable).ask(lone, lone_agent, "check", "a system", "a prompt", {"type": "object"}, 200)
asked = [e for e in lone.behaviour.tail if e["what"] == "ask"]
assert_("a provider that cannot be built leaves no reservation held, and the call is journaled with its fault",
        said is None and failing.reserved == 0 and asked and asked[-1]["data"]["outcome"] == "no provider" and "no provider can be made" in asked[-1]["data"]["reason"] and
        lone_agent["tally"]["calls"] == 1, (failing.reserved, asked[-1]["data"] if asked else None))
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
# ---- model calls kept only by agreement (thesmos's decision 0010) ----------------------------------------------------------------------
declared = [e for e in s.behaviour.tail if e["what"] == "agreement"]
assert_("the case declares its disclosure agreement, its maker consenting for the invented principals, recorded with its terms and basis, and every model call "
        "kept names it",
        len(declared) == 1 and declared[0]["data"]["terms"]["format"] == "noetarium.disclosure/1" and "maker's consent" in declared[0]["data"]["basis"] and
        all(e["data"].get("agreement") == "check-two: model calls" for e in asks), declared[0]["data"] if declared else None)
loose = [e for e in lone.behaviour.tail if e["what"] == "ask"]
assert_("in a society with no agreement a model call is recorded only as having taken place, with its outcome and cost, and neither its prompt, its answer nor its "
        "instructions",
        loose and all(e["data"].get("disclosed") is False and "prompt" not in e["data"] and "answer" not in e["data"] and "instructions" not in e["data"] for e in loose) and
        not any(e["what"] == "instructions" for e in lone.behaviour.tail), loose[-1]["data"] if loose else None)
ended = Society("check-ended")
ended.asker = agents.Asker(Ledger(None), lambda agent, n: Stub(cases.stand_in))
cases.two(ended)
for _ in range(4):  # the checkers ask their models from the third round
    ended.round()
ended.end_agreement("check-ended: model calls", by="the case's maker")
for _ in range(3):
    ended.round()
cut = next(e["n"] for e in ended.behaviour.tail if e["what"] == "agreement ended")  # the calls before and after the end, by their place in the journal
after = [e for e in ended.behaviour.tail if e["what"] == "ask" and e["n"] > cut]
before_end = [e for e in ended.behaviour.tail if e["what"] == "ask" and e["n"] < cut]
assert_("once the agreement ends, the agents' model calls are recorded only as having taken place; those before keep their prompts and the agreement",
        any(e["what"] == "agreement ended" for e in ended.behaviour.tail) and all(e["data"].get("disclosed") is False for e in after) and
        after and before_end and all(e["data"].get("agreement") for e in before_end), (len(after), len(before_end)))
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

# ---- holdings and the norms of access (thesmos's decision 0006) ---------------------------------------------------------------------
shown = next(x for x in s.view()["communities"] if x["name"] == "commons")
assert_("a community's page shows what it holds and its norms of access: the commons' shelf and guide, its norms in words, knowledge open",
        {h["id"] for h in shown["holdings"]} >= {"shelf", "guide"} and any(n["text"].startswith("contributor may deposit to the shelf") for n in shown["norms"]) and
        shown["open"] == {"knowledge": ["consult"]}, shown.get("norms"))
loom_agent = s.agents["loom/checker"]
woke = [e for e in s.behaviour.tail if e["what"] == "wake" and e["who"] == "loom/checker" and e["data"].get("briefed")]
situation = agents.situation_of(s, loom_agent, "commons", 0)
assert_("an agent wakes to its briefing, read from the community's record: the checker may consult the commons' shelf by the commons' norm, and the guide, open to all",
        woke and "consult shelf (the shelf consulted)" in woke[-1]["data"]["briefed"].get("commons", {}).get("may", []) and
        "consult shelf (the shelf consulted)" in situation["briefing"]["may"] and "consult guide (open)" in situation["briefing"]["may"] and situation["briefing"]["acts open to you"],
        (woke[-1]["data"] if woke else None, situation.get("briefing")))
s.submit("commons", [{"author": "cedar", "kind": "join", "turn": s.turn, "fields": {"position": "contributor"}}])
table = "an invented table of the streets of a district, with the year each was named"
put = agents.page_act(s, "cedar", "commons", "deposit", {"holding": "streets", "within": "shelf"}, table)
deposited = s.submit("commons", [put])[0]
held = next((h for h in s.communities["commons"].state["holdings"] if h["id"] == "streets"), {})
import hashlib  # noqa: E402
assert_("a person deposits a text to the commons' shelf from the page, held by its digest, awaiting a check by another origin",
        deposited["admitted"] and held.get("digest") == hashlib.sha256(table.encode("utf-8")).hexdigest() and held.get("pending") and held.get("depositor") == "cedar",
        (deposited, held))
steward_deposit = s.submit("commons", [agents.page_act(s, "aster", "commons", "deposit", {"holding": "aster-table", "within": "shelf"}, "a table of its own")])[0]
checks = agents.powers(s, "birch")["commons"]["checks"]
checked = s.submit("commons", [agents.page_act(s, "birch", "commons", "attest", {"holding": "streets"})])[0]
used = s.submit("commons", [agents.page_act(s, "cedar", "commons", "use", {"holding": "streets"})])[0]
read = s.submit("commons", [agents.page_act(s, "aster", "commons", "consult", {"holding": "streets"})])[0]
assert_("the steward, no contributor, is refused a deposit for want of a norm; the reviewer is asked to check the deposit and does; no norm lets the shelf be used, and the steward reads it",
        not steward_deposit["admitted"] and "no norm permits aster" in steward_deposit["reason"] and checks == ["streets"] and checked["admitted"] and
        not used["admitted"] and read["admitted"] and s.communities["commons"].state["accesses"][-1]["norm"] == "the shelf consulted",
        (steward_deposit["reason"], checks, checked, used["reason"], read))
listed = {p["name"]: p for p in s.directory()}
guild_public, commons_public = listed["guild"], listed["commons"]
assert_("the society's directory lists each community's public profile: the commons shows its mission, its admission, its route for claims and its public shelf "
        "with the deposit on it; the guild keeps its holders and its bench to its members",
        commons_public["purpose"] and commons_public["admission"]["contributor"]["entry"] == "open" and commons_public["claims"]["route"] and
        {"shelf", "guide", "streets"} <= {h["id"] for h in commons_public.get("holdings", [])} and "holders" in guild_public["withheld"] and
        not guild_public.get("holdings") and guild_public["admission"], (commons_public.get("holdings"), guild_public["withheld"]))
try:
    agents.page_act(s, "loom/checker", "commons", "deposit", {"holding": "x"}, "x")
    assert_("an agent cannot act as a person from the page", False)
except ValueError:
    assert_("an agent cannot act as a person from the page", True)

# ---- posted work whose life the community declares (thesmos's decision 0012) ----------------------------------------------------
w = Society("check-work")
for ident, kind, label in (("steward", "person", "the steward"), ("reviewer", "person", "the reviewer"), ("ash", "person", "Ash"), ("birch", "person", "Birch"),
                           ("fern", "person", "Fern"), ("lab-a", "organisation", "a provider"), ("lab-b", "organisation", "another provider")):
    w.party(ident, kind, label=label)
# Three agents lent: Ash's writer and Fern's editor run on one provider, Birch's critic on another.
for ident, sponsor, provider in (("ash/writer", "ash", "lab-a"), ("fern/editor", "fern", "lab-a"), ("birch/critic", "birch", "lab-b")):
    w.party(ident, "operative", controller=sponsor, provider=provider, label=ident)
workshop = copy.deepcopy(communities.preset("commons"))
workshop["work"] = {"kinds": {"entry": {"posted by": ["steward"], "taken by": "commit", "eligible": {"who": "contributor"}, "term": 4,
                                        "review": {"by": "another provider", "verdicts": ["adopt", "revise"], "accepts": ["adopt"]}, "accepted by": "steward"}}}
w.found("workshop", "commons", "steward", [["steward", "steward"], ["reviewer", "reviewer"]], "entries written, reviewed and accepted", constitution=workshop)
w.turn = 1
lent = [{"author": sponsor, "kind": "mandate", "turn": 1, "grant": {"id": f"{agent}@workshop", "principal": sponsor, "holder": agent, "kinds": kinds, "matters": [],
                                                                     "allowance": {}, "from": 1, "to": 40, "delegable": False}}
        for agent, sponsor, kinds in (("ash/writer", "ash", ["join", "commit", "deliver"]), ("fern/editor", "fern", ["assess"]), ("birch/critic", "birch", ["assess"]))]
w.submit("workshop", lent + [
    {"author": "ash/writer", "kind": "join", "turn": 1, "mandate": "ash/writer@workshop", "fields": {"position": "contributor"}},
    {"author": "steward", "kind": "request", "turn": 1, "fields": {"id": "e1", "work": "entry", "matter": "an index"}},
    {"author": "ash/writer", "kind": "commit", "turn": 1, "mandate": "ash/writer@workshop", "fields": {"id": "c1", "item": "e1"}},
    {"author": "ash/writer", "kind": "deliver", "turn": 1, "mandate": "ash/writer@workshop", "fields": {"id": "c1", "artefact": "a definition"}}])
w.turn = 2
own = w.submit("workshop", [{"author": "fern/editor", "kind": "assess", "turn": 2, "mandate": "fern/editor@workshop",
                             "fields": {"object": "e1", "criterion": "review", "value": "adopt"}}])
other = w.submit("workshop", [{"author": "birch/critic", "kind": "assess", "turn": 2, "mandate": "birch/critic@workshop",
                               "fields": {"object": "e1", "criterion": "review", "value": "adopt"}},
                              {"author": "steward", "kind": "accept", "turn": 2, "fields": {"id": "c1"}}])
shown_work = next(x for x in w.view()["communities"] if x["name"] == "workshop")["posted work"]
e1 = next((x for x in shown_work if x["id"] == "e1"), {})
assert_("a community's page shows its posted work: an entry posted by the steward, taken and delivered by Ash's writer, its review refused from Fern's editor, of "
        "another origin and the writer's provider, and admitted from Birch's critic, of another provider; then accepted, each step an act of its record",
        not own[0]["admitted"] and "provider" in own[0]["reason"] and all(o["admitted"] for o in other) and e1.get("state") == "accepted" and
        [r["by"] for r in e1.get("reviews", [])] == ["birch/critic"] and any(x["act"]["author"] == "fern/editor" for x in w.communities["workshop"].refused),
        (own, other, e1))

# ---- transmission by operatives and a compact with a reservation (thesmos's decision 0013) ------------------------------------------
r7 = Society("check-transmission")
for ident, kind, label in (("steward", "person", "the steward"), ("reviewer", "person", "the reviewer"), ("oak", "person", "Oak")):
    r7.party(ident, kind, label=label)
r7.party("oak/reader", "operative", controller="oak", provider="lab-c", label="Oak's reader")
room = copy.deepcopy(communities.preset("commons"))
room["access"]["operatives transmit to their provider"] = True
room["holdings"]["drafts"] = {"kind": "document", "custodian": "steward", "visibility": "public"}
room["access"]["norms"].append({"id": "drafts read", "who": "anyone", "may": "consult", "object": "drafts", "transmit": ["lab-a", "lab-b"]})
r7.found("reading room", "commons", "steward", [["steward", "steward"], ["reviewer", "reviewer"]], "drafts read where their norms allow", constitution=room)
r7.turn = 1
r7.submit("reading room", [{"author": "oak", "kind": "mandate", "turn": 1, "grant": {"id": "oak/reader@room", "principal": "oak", "holder": "oak/reader", "kinds": ["consult"],
                                                                                  "matters": [], "allowance": {}, "from": 1, "to": 40, "delegable": False}}])
sent = r7.submit("reading room", [{"author": "oak/reader", "kind": "consult", "turn": 1, "mandate": "oak/reader@room", "fields": {"holding": "drafts"}}])
open_terms = {"format": "noetarium.compact/1", "id": "reading compact", "articles": ["article 1", "article 2", "article 3"], "entry into force": 2, "withdrawal": {"notice": 2}}
r7.submit("reading room", [{"author": "steward", "kind": "propose", "turn": 1, "fields": {"id": "ratify", "decision": "amend operational", "change.compact": "reading compact",
                                                                                      "change.terms": json.dumps(open_terms), "change.reservations": "article 3"}},
                           {"author": "steward", "kind": "vote", "turn": 1, "fields": {"id": "ratify", "value": "yes"}},
                           {"author": "reviewer", "kind": "vote", "turn": 1, "fields": {"id": "ratify", "value": "yes"}}])
r7.turn = 2
ratified7 = r7.submit("reading room", [{"author": "steward", "kind": "amend", "turn": 2, "fields": {"id": "ratify"}}])
shown7 = next(x for x in r7.view()["communities"] if x["name"] == "reading room")
kept7 = shown7["compact records"][0] if shown7["compact records"] else {}
assert_("a community that declares that operatives transmit to their provider refuses an agent whose provider the drafts' norm does not name, the refusal shown on its "
        "card; it ratifies a compact by its own amendment with a reservation of article 3, and its card shows how it decides and the compact it holds",
        not sent[0]["admitted"] and "lab-c" in sent[0]["reason"] and any(x["act"]["author"] == "oak/reader" for x in shown7["refusals"]) and ratified7[0]["admitted"] and
        kept7.get("id") == "reading compact" and [q["article"] for q in kept7.get("reservations", [])] == ["article 3"] and
        shown7["aggregation"]["amend operational"]["rule"] == "majority",
        (sent, ratified7, kept7))

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
