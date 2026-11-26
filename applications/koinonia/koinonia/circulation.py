"""What passes between communities, and how a league keeps account of it (ADR 0169).

When an agent of one community does accepted work in another (a check, an adjudication), that is a **service** from its home
community to the other. A league of communities declares how services circulate, by a compact its council ratifies ("circulation:
credit (from round 6)"), so that the arrangement in force is read from the league's record and changes only by the league's own
rule. Four arrangements are declared here (the design record of 2026-11-25, Section 6.10, the jar's regimes carried between
communities):

- **gift**: services are given and reported; nothing is owed.
- **credit**: a service is a debt of the community served to the community that served, entered in the league's chronicle as a
  commitment in units of one service, repaid in kind (the debtor's service to its creditor discharges its oldest debt to it),
  mutual debts set off; no community serves one whose net debt to it has reached the limit.
- **transferable credit**: as credit, and a community near its limit may pay a debt by passing on a claim it holds on a third
  (an assignment): a claim that changes hands is money-like, and its passages are counted.
- **provision**: each community's open work is served by need from the others' capacity; needs and capacities are read from the
  record, nothing is owed, and debts left from an earlier arrangement are released by their creditors.

What the record shows (debts entered, set off, assigned, released; services withheld at the limit) is read by ``account``.
"""

from __future__ import annotations

ARRANGEMENTS = {
    "gift": "give freely; services are reported and nothing is owed",
    "credit": "a service is a debt of the community served, repaid in kind; no community serves one whose net debt to it has reached the limit",
    "transferable credit": "debts as under credit, and a community may pay one by passing on a claim it holds on a third",
    "cleared credit": "debts as under credit, and every cycle of debts the league's record shows is cleared by each creditor releasing its part",
    "provision": "each community's open work is served by need from the others' capacity, read from the record; nothing is owed",
}
CREDIT = ("credit", "transferable credit", "cleared credit")
LIMIT = 3


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


def may_serve(s, provider: str, target: str) -> bool:
    """Whether the arrangement in force lets the provider's agents serve the target now."""
    if provider == target or in_force(s) not in CREDIT:
        return True
    return owed(s, target, provider) - owed(s, provider, target) < LIMIT


def home_of(s, agent_id: str) -> str:
    a = s.agents.get(agent_id) or {}
    return a.get("home") or next(iter(a.get("communities", {})), "")


def withheld(s, provider: str, target: str) -> None:
    key = f"{provider}>{target}"
    s.work.withheld[key] = s.work.withheld.get(key, 0) + 1


def account(s) -> dict:
    """The league's account of circulation, read from its record."""
    lg = league(s)
    rec = lg.record if lg else []
    out = {"in force": in_force(s), "services": len(s.work.services), "by pair": {}, "debts outstanding": len(debts(s)) + len(debts(s, "presented")),
           "debts entered": sum(1 for a in rec if a["kind"] == "commit" and a["fields"].get("matter") == "service"),
           "repaid": sum(1 for c in (lg.state.get("commitments", []) if lg else []) if c["matter"] == "service" and c["status"] == "discharged"),
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


# ---- the clerk: a community's book-keeper in the league, by rule -------------------------------------------------------------------
def clerk(s, agent: dict, act) -> list:
    lg = league(s)
    if lg is None or lg.name not in agent["communities"]:
        return []
    mine, regime, name, out = agent["home"], in_force(s), lg.name, []
    active = debts(s)
    new = [(k, v) for k, v in s.work.services.items() if v["state"] == "new"]
    if regime in CREDIT:
        for k, v in new:
            if v["provider"] == mine:
                debt = next((d for d in active if d["debtor"] == mine and d["creditor"] == v["target"] and not d.get("_used")), None)
                if debt:  # a service to a community it owes repays its oldest debt there
                    debt["_used"] = True
                    out.append(act(agent, name, "deliver", s, {"id": debt["id"], "matter": "service", "artefact": k}))
            elif v["target"] == mine and not any(d["debtor"] == v["provider"] and d["creditor"] == mine for d in active):
                out.append(act(agent, name, "commit", s, {"id": f"s-{k}", "to": v["provider"], "matter": "service", "unit": "services", "amount": 1}))
        for d in lg.state.get("commitments", []):
            if d["matter"] == "service" and d["status"] == "presented" and d["creditor"] == mine:
                out.append(act(agent, name, "accept", s, {"id": d["id"]}))
        others = {d["debtor"] for d in active if d["creditor"] == mine} & {d["creditor"] for d in active if d["debtor"] == mine}
        for x in sorted(others):  # mutual debts: each creditor releases one, a set-off
            d = next(d for d in active if d["creditor"] == mine and d["debtor"] == x)
            out.append(act(agent, name, "release", s, {"id": d["id"], "why": "set off"}))
        if regime == "transferable credit":
            for x in sorted({d["creditor"] for d in active if d["debtor"] == mine}):
                if owed(s, mine, x) - owed(s, x, mine) < LIMIT - 1:
                    continue
                claim = next((d for d in active if d["creditor"] == mine and d["debtor"] not in (x, mine)), None)
                debt = next((d for d in active if d["debtor"] == mine and d["creditor"] == x and not d.get("_used")), None)
                if claim and debt:  # pays a debt by passing on a claim on a third
                    debt["_used"] = True
                    out.append(act(agent, name, "assign", s, {"id": claim["id"], "to": x}))
                    out.append(act(agent, name, "deliver", s, {"id": debt["id"], "matter": "service", "artefact": claim["id"]}))
        if regime == "cleared credit":  # the league's record shows every debt: each cycle is cleared, each creditor releasing its part
            for cycle in cycles(active):
                for debtor, creditor in cycle:
                    if creditor != mine:
                        continue
                    d = next(d for d in active if d["debtor"] == debtor and d["creditor"] == creditor and not d.get("_used"))
                    d["_used"] = True
                    out.append(act(agent, name, "release", s, {"id": d["id"], "why": "cleared", "cycle": " > ".join([c[0] for c in cycle] + [cycle[0][0]])}))
    else:  # nothing is owed under gift or provision: debts left from credit, open or with a repayment presented, are released
        for d in active + debts(s, "presented"):
            if d["creditor"] == mine:
                out.append(act(agent, name, "release", s, {"id": d["id"], "why": "dissolved"}))
        for k, v in new:
            if v["provider"] == mine:
                out.append(act(agent, name, "report", s, {"matter": "service", "service": k, "to": v["target"]}))
    for d in active:
        d.pop("_used", None)
    return out


def cycles(active: list[dict]) -> list[list[tuple[str, str]]]:
    """Cycles of three or more communities in the graph of debts (debtor to creditor), each unit of debt in at most one, found in an
    order fixed by the names, so that every clerk reading the same record finds the same cycles (two-way debts are set off apart)."""
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


def recorded(s) -> None:
    """Marks services entered in the league's record (as a debt, a repayment or a report)."""
    lg = league(s)
    if lg is None:
        return
    seen = set()
    for a in lg.record:
        f = a["fields"]
        if a["kind"] == "commit" and f.get("id", "").startswith("s-"):
            seen.add(f["id"][2:])
        elif a["kind"] in ("deliver", "report") and (f.get("artefact") or f.get("service")):
            seen.add(f.get("artefact") or f.get("service"))
    for k, v in s.work.services.items():
        if v["state"] == "new" and (k in seen or in_force(s) in ("gift", "provision") and k in seen):
            v["state"] = "recorded"


# ---- the envoy: a community's voice in the league's council -----------------------------------------------------------------------
ENVOY_SYSTEM = (
    "You are the envoy of a community in the council of a league of communities that do work for one another (checks and adjudications "
    "of a catalogue's entries). The league declares how what passes between communities is kept: gift (nothing owed), credit (a service "
    "is a debt repaid in kind, no service past a limit of net debt), transferable credit (a debt may be paid by passing on a claim on a "
    "third community) or provision (work served by need, read from the record, nothing owed). Each time you wake, choose among the "
    "options you are given (one or two of their ids), for the good of your community and of the work the league exists for; decisions "
    "are counted by community. A note to the council is said only when it helps (at most 40 words). Give your reason in one sentence.")


def envoy_options(s, agent: dict) -> tuple[dict, dict]:
    lg = league(s)
    regime, mine = in_force(s), agent["home"]
    open_p = [p for p in lg.state.get("proposals", []) if p["status"] == "open"]
    options = {}
    if not open_p:
        for word in ARRANGEMENTS:
            if word != regime:
                options[f"propose {word}"] = f"propose that the league change to {word}: {ARRANGEMENTS[word]}"
    for p in open_p:
        if not any(v["voter"] == agent["id"] for v in p["votes"]):
            options[f"vote yes {p['id']}"] = f"vote for proposal {p['id']} ({proposal_text(lg, p['id'])})"
            options[f"vote no {p['id']}"] = f"vote against proposal {p['id']}"
    options["note"] = "say a note to the council"
    options["rest"] = "do nothing this round"
    acc = account(s)
    situation = {"you": agent["id"], "your community": mine, "round": s.turn, "in force": regime, "limit of net debt": LIMIT,
                 "services by pair (provider>served)": acc["by pair"], "services withheld at the limit": acc["withheld"],
                 "debts outstanding": {f"{d['debtor']} owes {d['creditor']}": owed(s, d["debtor"], d["creditor"]) for d in debts(s)},
                 "set off": acc["set off"], "assignments": acc["assignments"],
                 "work left": {c.name: sum(1 for b in s.work.batches if b.get("owner") == c.name and not b.get("settled")) for c in s.communities.values() if any(b.get("owner") == c.name for b in s.work.batches)},
                 "open proposals": [{"id": p["id"], "change": proposal_text(lg, p["id"]), "votes": p["votes"], "shares": p["shares"]} for p in open_p]}
    return options, situation


def proposal_text(lg, pid: str) -> str:
    for a in lg.record:
        if a["kind"] == "propose" and a["fields"].get("id") == pid:
            return a["fields"].get("change.compact", "")
    return ""


def envoy(s, agent: dict, act, ask) -> list:
    lg = league(s)
    if lg is None:
        return []
    name, out = lg.name, []
    # A proposal that has passed and is ripe is enacted by the envoy who proposed it (the amend act; the fold checks rule and tempo).
    for p in lg.state.get("proposals", []):
        share = p.get("shares", {}).get("community", "0")
        num, _, den = share.partition("/")
        passed = int(num or 0) * 2 > int(den or 1)  # a majority counted by community, the league's rule for amending its operations
        if p["status"] == "open" and passed and any(a["kind"] == "propose" and a["fields"].get("id") == p["id"] and a["author"] == agent["id"] for a in lg.record):
            out.append(act(agent, name, "amend", s, {"id": p["id"]}))
    from .agents import changed_since_reflection
    due = s.turn - agent["memory"].get("last_reflect", 0) >= agent["reflect_every"] and changed_since_reflection(s, agent)
    fresh_vote = any(p["status"] == "open" and not any(v["voter"] == agent["id"] for v in p["votes"]) for p in lg.state.get("proposals", []))
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
            out.append(act(agent, name, "propose", s, {"id": pid, "level": "operational", "change.compact": f"circulation: {word} (from round {s.turn})"},
                           _body={"note": f"{agent['id']} proposes {word}: {answer.get('why', '')[:200]}"}))
        elif o.startswith("vote "):
            _, value, pid = o.split(" ", 2)
            out.append(act(agent, name, "vote", s, {"id": pid, "value": value}))
    note = str(answer.get("note", "")).strip()
    if note:
        out.append(act(agent, name, "say", s, {}, body=note[:400], _body={"note": f"{agent['id']}: {note}"}))
    return out
