"""What passes between communities, and how a league keeps account of it (ADR 0169).

When an agent of one community does accepted work in another (a check, an adjudication), that is a **service** from its home
community to the other. A league declares by a compact its council ratifies ("circulation: credit (proposal p3-commons)") how
services are kept, so that the arrangement in force is read from the league's record and changes only by the league's rule. Six
arrangements are declared (the design record of 2026-11-25, Section 6.10, the jar's regimes carried between communities):

- **gift**: services are reported; nothing is owed, and debts left from an earlier arrangement are released.
- **credit**: a service is a debt of the community served to the community that served, in units of one service; the debtor's
  service to its creditor repays its oldest debt there; mutual debts are set off; no agent serves a community whose net debt to
  its home has reached the limit.
- **transferable credit**: as credit, and a community near its limit may pay a debt by passing on a claim it holds on a third.
- **cleared credit**: as credit, and every cycle of three or more communities in the debts is cleared.
- **provision**: work is served by need (home first, then the community with the most open work); nothing is owed.
- **provision with duty**: as provision, and no agent serves a community in default: one the league's notary has attested, within
  the last four rounds, to have declined work it could do.

**The books are kept once a round, by one plan.** At the end of each round, after the work, the league's books are planned from its
state: repayments and new debts for the round's services, acceptance of repayments presented, set-off, clearing, assignment; each
unit of debt takes part in at most one operation, so that no operation changes a net position it should keep. Each community's
clerk then enters its part, and a payment by assignment is entered only once the assignment is admitted. The notary attests after
the round's acts, counting a checker only if it put nothing forward that round and has nothing in flight after it.
"""

from __future__ import annotations

ARRANGEMENTS = {
    "gift": "give freely; services are reported and nothing is owed",
    "credit": "a service is a debt of the community served, repaid in kind; no community serves one whose net debt to it has reached the limit",
    "transferable credit": "debts as under credit, and a community may pay one by passing on a claim it holds on a third",
    "cleared credit": "debts as under credit, and every cycle of debts the league's record shows is cleared by each creditor releasing its part",
    "provision": "each community's open work is served by need from the others' capacity, read from the record; nothing is owed",
    "provision with duty": "work is served by need, and a community the league's notary attests has declined work it could do is not served until it serves",
}
CREDIT = ("credit", "transferable credit", "cleared credit")
LIMIT = 3
WINDOW, DEFAULT = 4, 2  # in default: at least two declinings attested in the last four completed rounds
LAPSE = 4               # a proposal that has not passed four rounds after it was made lapses for the council's purposes
CRITERION = "declined work it could do"


def league(s):
    return next((c for c in s.communities.values() if c.preset in ("first-league", "league")), None)


def in_force(s) -> str:
    """The arrangement the league's last ratified compact of circulation declares, else the society's declared default."""
    lg = league(s)
    named = [c for c in (lg.state.get("compacts", []) if lg else []) if c.startswith("circulation: ")]
    if named:
        word = named[-1][len("circulation: "):].split(" (")[0]
        if word in ARRANGEMENTS:
            return word
    return s.work.regime or "gift"


def debts(s, status: str = "active") -> list[dict]:
    lg = league(s)
    return [c for c in (lg.state.get("commitments", []) if lg else []) if c["matter"] == "service" and c["status"] == status]


def owed(s, debtor: str, creditor: str) -> int:
    return sum(1 for d in debts(s) if d["debtor"] == debtor and d["creditor"] == creditor)


def attested(s, community: str) -> int:
    """Declinings the notary attested of a community in the last WINDOW completed rounds (it attests at a round's end)."""
    lg = league(s)
    return sum(int(a["fields"].get("value", 0)) for a in (lg.record if lg else []) if a["kind"] == "attest" and a["fields"].get("object") == community
               and a["fields"].get("criterion") == CRITERION and s.turn - WINDOW <= a["turn"] < s.turn)


def in_default(s, community: str) -> bool:
    return attested(s, community) >= DEFAULT


def may_serve(s, provider: str, target: str) -> bool:
    """Whether the arrangement in force lets the provider's agents serve the target now."""
    if provider == target:
        return True
    regime = in_force(s)
    if regime == "provision with duty":
        return not in_default(s, target)
    if regime not in CREDIT:
        return True
    return owed(s, target, provider) - owed(s, provider, target) < LIMIT


def home_of(s, agent_id: str) -> str:
    a = s.agents.get(agent_id) or {}
    return a.get("home") or next(iter(a.get("communities", {})), "")


def withheld(s, agent: dict, provider: str, target: str) -> None:
    """An agent kept from serving by the arrangement in force: counted, and recorded in the behaviour journal."""
    key = f"{provider}>{target}"
    s.work.withheld[key] = s.work.withheld.get(key, 0) + 1
    turns = s.work.withheld_at.setdefault(target, [])
    if not isinstance(turns, list):  # a society kept before the history was kept
        turns = s.work.withheld_at[target] = [turns]
    if not turns or turns[-1] != s.turn:
        turns.append(s.turn)
        del turns[:-8]
    s.behaviour.record(s.turn, agent["id"], "withheld", target, provider=provider, arrangement=in_force(s))


def withheld_lately(s, community: str, rounds: int = 3) -> bool:
    """Whether another community withheld its service from this one in one of the last `rounds` rounds before this one (a
    withholding made earlier in this round is left out, so that no agent's decision depends on the order agents decide in)."""
    turns = s.work.withheld_at.get(community, [])
    turns = turns if isinstance(turns, list) else [turns]
    return any(0 < s.turn - t <= rounds for t in turns)


def would_be_refused(s, community: str) -> bool:
    """Whether the league's record shows the community would be refused service: lately refused; at the limit of its net debt to
    another community under credit; in default under provision with duty. What a free rider that reads its standing responds to,
    the same rule under every arrangement."""
    if withheld_lately(s, community):
        return True
    regime = in_force(s)
    if regime == "provision with duty":
        return in_default(s, community)
    if regime in CREDIT:
        creditors = {d["creditor"] for d in debts(s) if d["debtor"] == community}
        return any(owed(s, community, p) - owed(s, p, community) >= LIMIT for p in creditors)
    return False


def account(s) -> dict:
    """The league's account of circulation, read from its record."""
    lg = league(s)
    rec = lg.record if lg else []
    paid_by = {a["fields"].get("id"): a["fields"].get("artefact", "") for a in rec if a["kind"] == "deliver" and a["fields"].get("matter") == "service"}
    discharged = [c for c in (lg.state.get("commitments", []) if lg else []) if c["matter"] == "service" and c["status"] == "discharged"]
    by_claim = sum(1 for c in discharged if paid_by.get(c["id"], "").startswith("s-"))
    out = {"in force": in_force(s), "services": len(s.work.services), "by pair": {}, "debts outstanding": len(debts(s)) + len(debts(s, "presented")),
           "debts entered": sum(1 for a in rec if a["kind"] == "commit" and a["fields"].get("matter") == "service"),
           "repaid in kind": len(discharged) - by_claim, "paid by assignment": by_claim, "repaid": len(discharged),
           "set off": sum(1 for a in rec if a["kind"] == "release" and a["fields"].get("why") == "set off"),
           "released by the arrangement": sum(1 for a in rec if a["kind"] == "release" and a["fields"].get("why") == "dissolved"),
           "cleared around cycles": sum(1 for a in rec if a["kind"] == "release" and a["fields"].get("why") == "cleared"),
           "assignments": sum(1 for a in rec if a["kind"] == "assign"), "withheld": dict(s.work.withheld),
           "compacts": [c for c in (lg.state.get("compacts", []) if lg else []) if c.startswith("circulation: ")]}
    for v in s.work.services.values():
        k = f"{v['provider']}>{v['target']}"
        out["by pair"][k] = out["by pair"].get(k, 0) + 1
    passed: dict[str, int] = {}
    for a in rec:
        if a["kind"] == "assign":
            passed[a["fields"]["id"]] = passed.get(a["fields"]["id"], 0) + 1
    out["claims passed on more than once"] = sum(1 for n in passed.values() if n > 1)
    return out


# ---- services found after a round --------------------------------------------------------------------------------------------------
def find_services(s) -> None:
    """Every accepted piece of work an agent did outside its home community, once."""
    for c in s.communities.values():
        if c.preset in ("first-league", "league"):
            continue
        for x in c.state.get("commitments", []):
            if x["status"] != "discharged" or x["id"] in s.work.services:
                continue
            body = s.work.bodies.get(x["id"])
            if not body:
                continue
            provider = home_of(s, body["by"])
            target = c.name
            if body.get("kind") == "adjudication":
                d = s.work.dispute(x["id"].split("~")[0])
                target = owner_of(s, d) if d else target
            if provider and target and provider != target:
                s.work.services[x["id"]] = {"provider": provider, "target": target, "turn": s.turn, "kind": body.get("kind", ""), "state": "new"}


def owner_of(s, dispute: dict) -> str:
    b = s.work.batch(dispute["batch"]) or {}
    return b.get("owner", "commons")


# ---- the books, once a round, by one plan --------------------------------------------------------------------------------------------
def cycles(active: list[dict]) -> list[list[tuple[str, str]]]:
    """Cycles of three or more communities in the graph of debts (debtor to creditor), each unit of debt in at most one, found in an
    order fixed by the names, so that the same debts always give the same cycles (two-way debts are set off apart)."""
    units: dict[tuple[str, str], int] = {}
    for d in active:
        units[(d["debtor"], d["creditor"])] = units.get((d["debtor"], d["creditor"]), 0) + 1
    found = []
    while True:
        cycle = None
        for start in sorted({a for a, _ in units}):
            stack = [(start, [start])]
            while stack and cycle is None:
                node, path = stack.pop()
                for (a, b), n in sorted(units.items(), reverse=True):
                    if a != node or n <= 0:
                        continue
                    if b == start and len(path) >= 3:
                        cycle = path
                        break
                    if b not in path:
                        stack.append((b, path + [b]))
            if cycle:
                break
        if not cycle:
            return found
        edges = [(cycle[i], cycle[(i + 1) % len(cycle)]) for i in range(len(cycle))]
        for e in edges:
            units[e] -= 1
        found.append(edges)


def plan(s) -> tuple[list[tuple[str, dict]], list[tuple[str, dict, str]], dict[str, str]]:
    """The round's operations on the league's books, from its state, each unit of debt used at most once: a list of (community, act)
    for the first entry, a list of (community, act, the assignment it waits on) for payments by assignment, and the services each act
    records (act id -> service)."""
    regime = active_regime = in_force(s)
    active = sorted(debts(s), key=lambda d: d["id"])
    presented = sorted(debts(s, "presented"), key=lambda d: d["id"])
    new = sorted(((k, v) for k, v in s.work.services.items() if v["state"] == "new"), key=lambda kv: (kv[1]["turn"], kv[0]))
    used: set[str] = set()
    first: list[tuple[str, dict]] = []
    later: list[tuple[str, dict, str]] = []
    records: dict[str, str] = {}

    def unused(debtor=None, creditor=None, exclude=()):
        return [d for d in active if d["id"] not in used and (debtor is None or d["debtor"] == debtor) and (creditor is None or d["creditor"] == creditor)
                and d["debtor"] not in exclude]

    if active_regime in CREDIT:
        for d in presented:  # a repayment presented is accepted by its creditor
            first.append((d["creditor"], {"kind": "accept", "fields": {"id": d["id"]}}))
        for k, v in new:
            mine = unused(debtor=v["provider"], creditor=v["target"])
            if mine:  # a service to a community it owes repays its oldest debt there
                used.add(mine[0]["id"])
                first.append((v["provider"], {"kind": "deliver", "fields": {"id": mine[0]["id"], "matter": "service", "artefact": k}}))
            else:
                first.append((v["target"], {"kind": "commit", "fields": {"id": f"s-{k}", "to": v["provider"], "matter": "service", "unit": "services", "amount": 1}}))
            records[f"service:{k}"] = k
        pairs = sorted({tuple(sorted((d["debtor"], d["creditor"]))) for d in active})
        for x, y in pairs:  # mutual debts set off, a unit each way per pair, as often as both ways remain
            while True:
                xy, yx = unused(debtor=x, creditor=y), unused(debtor=y, creditor=x)
                if not xy or not yx:
                    break
                used.update({xy[0]["id"], yx[0]["id"]})
                first.append((y, {"kind": "release", "fields": {"id": xy[0]["id"], "why": "set off"}}))
                first.append((x, {"kind": "release", "fields": {"id": yx[0]["id"], "why": "set off"}}))
        if regime == "cleared credit":
            for cycle in cycles(unused()):
                for debtor, creditor in cycle:
                    d = unused(debtor=debtor, creditor=creditor)[0]
                    used.add(d["id"])
                    first.append((creditor, {"kind": "release", "fields": {"id": d["id"], "why": "cleared",
                                                                         "cycle": " > ".join([c[0] for c in cycle] + [cycle[0][0]])}}))
        if regime == "transferable credit":
            for c in sorted({d["debtor"] for d in active}):
                for x in sorted({d["creditor"] for d in unused(debtor=c)}):
                    if owed(s, c, x) - owed(s, x, c) < LIMIT - 1:
                        continue
                    claims, mine = unused(creditor=c, exclude=(x, c)), unused(debtor=c, creditor=x)
                    if claims and mine:  # pays a debt by passing on a claim on a third; the payment waits on the assignment
                        used.update({claims[0]["id"], mine[0]["id"]})
                        first.append((c, {"kind": "assign", "fields": {"id": claims[0]["id"], "to": x}}))
                        later.append((c, {"kind": "deliver", "fields": {"id": mine[0]["id"], "matter": "service", "artefact": claims[0]["id"]}}, claims[0]["id"]))
    else:
        for d in active + presented:  # nothing is owed: debts left from credit, open or with a repayment presented, are released
            first.append((d["creditor"], {"kind": "release", "fields": {"id": d["id"], "why": "dissolved"}}))
        for k, v in new:
            first.append((v["provider"], {"kind": "report", "fields": {"matter": "service", "service": k, "to": v["target"]}}))
            records[f"service:{k}"] = k
    return first, later, records


def books(s) -> None:
    """Keeps the league's books for the round: the plan entered by each community's clerk, payments by assignment after their
    assignments are admitted, and each service marked recorded once its entry is admitted."""
    lg = league(s)
    if lg is None:
        return
    clerks = {a["home"]: a for a in s.agents.values() if a["role"] == "clerk" and lg.name in a["communities"] and not a.get("paused")}
    first, later, _ = plan(s)

    def act_of(community, a):
        clerk = clerks.get(community)
        if clerk is None:
            return None
        return {"author": clerk["id"], "kind": a["kind"], "turn": s.turn, "mandate": clerk["communities"][lg.name]["mandate"], "fields": a["fields"]}

    acts = [x for x in (act_of(c, a) for c, a in first) if x]
    outcomes = s.submit(lg.name, acts) if acts else []
    admitted_assignments = {a["fields"]["id"] for a, o in zip(acts, outcomes) if a["kind"] == "assign" and o["admitted"]}
    payments = [x for x in (act_of(c, a) for c, a, claim in later if claim in admitted_assignments) if x]
    if payments:
        s.submit(lg.name, payments)
    entered = set()
    for a in lg.record:
        f = a["fields"]
        if a["kind"] == "commit" and f.get("id", "").startswith("s-"):
            entered.add(f["id"][2:])
        elif a["kind"] in ("deliver", "report") and (f.get("artefact") or f.get("service")):
            entered.add(f.get("artefact") or f.get("service"))
    for k, v in s.work.services.items():
        if v["state"] == "new" and k in entered:
            v["state"] = "recorded"


# ---- the notary: what the record shows of each community's capacity, after the round ----------------------------------------------
def notary(s, acted: set[str]) -> None:
    """Attests, at the round's end, community by community, how many of its checkers put nothing forward this round, have nothing
    offered, taken or delivered after it, could do no open batch at home, and could do an open batch in another community where they
    hold a working place, their mandate unrevoked, and may serve: the capacity a community declined, read from the chronicles."""
    from .agents import WORKING, kind_of, open_batches, revoked
    lg = league(s)
    notaries = [a for a in s.agents.values() if a["role"] == "notary" and lg is not None and lg.name in a["communities"] and not a.get("paused")]
    if not notaries:
        return
    declined: dict[str, int] = {}
    for a in s.agents.values():
        if a["role"] != "checker" or a.get("paused") or a["id"] in acted:
            continue
        home = a.get("home", "")
        if any(x["by"] == a["id"] and x["status"] in ("conditional", "active", "presented") for c in s.communities.values() for x in c.state.get("commitments", [])):
            continue
        places = [c for c, m in a["communities"].items() if m.get("position") in WORKING and c in s.communities and not revoked(s, a, c)]
        if home in places and open_batches(s, s.communities[home], a["origin"], kind_of(a)):
            continue
        if any(open_batches(s, s.communities[c], a["origin"], kind_of(a)) for c in places if c != home and may_serve(s, home, c)):
            declined[home] = declined.get(home, 0) + 1
    n = notaries[0]
    acts = [{"author": n["id"], "kind": "attest", "turn": s.turn, "mandate": n["communities"][lg.name]["mandate"],
             "fields": {"object": c, "criterion": CRITERION, "value": k}} for c, k in sorted(declined.items())]
    if acts:
        s.submit(lg.name, acts)


# ---- the envoy: a community's voice in the league's council ---------------------------------------------------------------------
ENVOY_SYSTEM = (
    "You are the envoy of a community in the council of a league of communities that do work for one another (checks and adjudications "
    "of a catalogue's entries). The league declares how what passes between communities is kept, one of six arrangements: gift (nothing "
    "owed); credit (a service is a debt repaid in kind, no service past a limit of net debt); transferable credit (a debt may be paid by "
    "passing on a claim on a third community); cleared credit (cycles of debt cleared on the league's record); provision (work served by "
    "need, nothing owed); provision with duty (served by need, and a community the league's notary attests has declined work it could do "
    "is not served until it serves). Each time you wake, choose among the options you are given (one or two of their ids), for the good "
    "of your community and of the work the league exists for; a change passes with a majority of the member communities. A note to the "
    "council is said only when it helps (at most 40 words). Give your reason in one sentence.")


def members(lg) -> list[str]:
    return sorted(p["party"] for p in lg.state.get("positions", []) if p["position"] == "member")


def proposal_text(lg, pid: str) -> str:
    for a in lg.record:
        if a["kind"] == "propose" and a["fields"].get("id") == pid:
            if a["fields"].get("decision") == "accession":
                return f"admit {a['fields'].get('change.admit', '')} to the league as a member"
            return a["fields"].get("change.compact", "")
    return ""


def proposed_at(lg, pid: str) -> tuple[int, str]:
    for a in lg.record:
        if a["kind"] == "propose" and a["fields"].get("id") == pid:
            return a["turn"], a["author"]
    return -1, ""


def considered(s, lg) -> dict | None:
    """The council considers one change of arrangement at a time: the earliest open proposal of circulation that has not lapsed."""
    for a in lg.record:
        if a["kind"] != "propose" or not a["fields"].get("change.compact", "").startswith("circulation"):
            continue
        p = next((p for p in lg.state.get("proposals", []) if p["id"] == a["fields"]["id"]), None)
        if p and p["status"] == "open" and s.turn - a["turn"] <= LAPSE:
            return p
    return None


def yes_from_members(lg, p: dict) -> int:
    ms = set(members(lg))
    return len({v["bearer"] for v in p["votes"] if v["yes"] and v["bearer"] in ms})


def accession(s, agent: dict, act, lg) -> list:
    """An applicant community's accession, by procedure: the first member's envoy proposes it, envoys by rule vote for it (envoys by
    model are asked), and once two thirds of the member communities agree the proposer admits it, the fold checking rule and tempo."""
    out, name = [], lg.name
    ms = members(lg)
    for applicant in s.work.applicants:
        if lg.holds(applicant, "member"):
            continue
        pid = f"a-{applicant}"
        prop = next((p for p in lg.state.get("proposals", []) if p["id"] == pid), None)
        if prop is None and ms and agent.get("home") == ms[0]:
            out.append(act(agent, name, "propose", s, {"id": pid, "decision": "accession", "change.admit": applicant, "change.position": "member"}))
        elif prop and prop["status"] == "open":
            if agent["policy"] == "rule" and not any(v["voter"] == agent["id"] for v in prop["votes"]):
                out.append(act(agent, name, "vote", s, {"id": pid, "value": "yes"}))
            if proposed_at(lg, pid)[1] == agent["id"] and 3 * yes_from_members(lg, prop) >= 2 * len(ms):
                out.append(act(agent, name, "admit", s, {"position": "member", "whom": applicant, "proposal": pid}))
    return out


def envoy_options(s, agent: dict) -> tuple[dict, dict]:
    lg = league(s)
    regime, mine = in_force(s), agent["home"]
    p = considered(s, lg)
    options = {}
    if p is None:
        for word in ARRANGEMENTS:
            if word != regime:
                options[f"propose {word}"] = f"propose that the league change to {word}: {ARRANGEMENTS[word]}"
    open_accession = [q for q in lg.state.get("proposals", []) if q["status"] == "open" and q["id"].startswith("a-")]
    for q in ([p] if p else []) + open_accession:
        if not any(v["voter"] == agent["id"] for v in q["votes"]):
            options[f"vote yes {q['id']}"] = f"vote for proposal {q['id']} ({proposal_text(lg, q['id'])})"
            options[f"vote no {q['id']}"] = f"vote against proposal {q['id']}"
    options["note"] = "say a note to the council"
    options["rest"] = "do nothing this round"
    acc = account(s)
    situation = {"you": agent["id"], "your community": mine, "round": s.turn, "in force": regime, "limit of net debt": LIMIT,
                 "member communities": members(lg),
                 "services by pair (provider>served)": acc["by pair"], "services withheld at the limit or for default": acc["withheld"],
                 "debts outstanding": {f"{d['debtor']} owes {d['creditor']}": owed(s, d["debtor"], d["creditor"]) for d in debts(s)},
                 "set off": acc["set off"], "assignments": acc["assignments"],
                 "declinings the notary attested in the last four rounds (idle checkers while work they could do lay open elsewhere)": {
                     c.name: attested(s, c.name) for c in s.communities.values() if c is not lg},
                 "work left": {c.name: sum(1 for b in s.work.batches if b.get("owner") == c.name and not b.get("settled") and b.get("from", 0) <= s.turn)
                               for c in s.communities.values() if any(b.get("owner") == c.name for b in s.work.batches)},
                 "open proposals": [{"id": q["id"], "change": proposal_text(lg, q["id"]), "votes": q["votes"]} for q in ([p] if p else []) + open_accession]}
    return options, situation


def envoy(s, agent: dict, act, ask) -> list:
    lg = league(s)
    if lg is None:
        return []
    name, out = lg.name, accession(s, agent, act, lg)
    if agent["policy"] == "rule":  # an envoy by rule takes part in accessions and keeps the arrangement as it is
        return out
    p = considered(s, lg)
    # the proposal under consideration is enacted by its proposer once a majority of the member communities voted for it
    if p and proposed_at(lg, p["id"])[1] == agent["id"] and 2 * yes_from_members(lg, p) > len(members(lg)):
        out.append(act(agent, name, "amend", s, {"id": p["id"]}))
    from .agents import changed_since_reflection
    due = s.turn - agent["memory"].get("last_reflect", 0) >= agent["reflect_every"] and changed_since_reflection(s, agent)
    fresh_vote = any(q["status"] == "open" and not any(v["voter"] == agent["id"] for v in q["votes"])
                     for q in ([p] if p else []) + [q for q in lg.state.get("proposals", []) if q["status"] == "open" and q["id"].startswith("a-")])
    if not (due or fresh_vote):
        return out
    agent["memory"]["last_reflect"] = s.turn
    options, situation = envoy_options(s, agent)
    import json
    answer = ask(s, agent, "council", ENVOY_SYSTEM, json.dumps({"situation": situation, "options": options}, ensure_ascii=False), 1500)
    if answer is None:
        return out
    chosen = [o for o in answer.get("choose", []) if o in options][:2]
    agent["memory"]["last_choice"] = {"turn": s.turn, "chose": chosen, "why": str(answer.get("why", ""))[:300]}
    for o in chosen:
        if o.startswith("propose "):
            word = o[len("propose "):]
            pid = f"p{1 + sum(1 for a in lg.record if a['kind'] == 'propose')}-{agent['home']}"
            out.append(act(agent, name, "propose", s, {"id": pid, "level": "operational", "change.compact": f"circulation: {word} (proposal {pid})"},
                           _body={"note": f"{agent['id']} proposes {word}: {answer.get('why', '')[:200]}"}))
            break  # one proposal at a time
        if o.startswith("vote "):
            _, value, pid = o.split(" ", 2)
            out.append(act(agent, name, "vote", s, {"id": pid, "value": value}))
    note = str(answer.get("note", "")).strip()
    if note:
        out.append(act(agent, name, "say", s, {}, body=note[:400], _body={"note": f"{agent['id']}: {note}"}))
    return out
