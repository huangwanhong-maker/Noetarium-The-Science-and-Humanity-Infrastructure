"""A provisional executable sketch of the ground of the society of communities.

This file states the design's definitions as code so that they can be checked. It is a sketch
for the design record, written in Python for speed of revision. The implementation proposed in
the design is C++ in the infrastructure, held to the same checks.

Sections
  A  parties and origins
  B  mandates: chains that only narrow, and envelopes charged along the whole chain
  C  acts, the chronicle and the fold that gives the relational state
  D  set-off and novation on a graph of obligations
  E  sufficiency of a record for a query
  F  returns passed upstream along a graph of use
  G  counts by origin
  H  gates of access, routes of claims, recognised and executable powers, facets of a norm

Run the checks with:  python checks.py
"""
from __future__ import annotations

import copy
import hashlib
import json
from dataclasses import dataclass, field
from fractions import Fraction

# --------------------------------------------------------------------------------------------
# A. Parties and origins

PERSON, ORGANISATION, OPERATIVE, COMMUNITY = "person", "organisation", "operative", "community"


@dataclass(frozen=True)
class Party:
    id: str
    kind: str
    controller: str | None = None  # of an operative: the party that holds its credentials and configuration


def origin(parties: dict[str, Party], pid: str) -> str:
    """The first party that is not an operative, reached by following controllers.

    Sameness of origin is a criterion: two operatives are one origin when the declared relation of
    control leads both to the same party. An operative with no declared controller is its own origin.
    """
    seen: set[str] = set()
    while True:
        party = parties[pid]
        if party.kind != OPERATIVE or party.controller is None or pid in seen:
            return pid
        seen.add(pid)
        pid = party.controller


# --------------------------------------------------------------------------------------------
# B. Mandates


@dataclass(frozen=True)
class Mandate:
    id: str
    principal: str                      # the party that grants
    holder: str                         # the party that may act
    kinds: frozenset[str]               # kinds of act
    matters: frozenset[str]             # subject matters
    allowance: tuple[tuple[str, int], ...]  # (resource, ceiling), each resource in its own unit
    term: tuple[int, int]               # first and last turn of validity
    delegable: bool = False
    parent: str | None = None
    slice: int | None = None            # the most that may be charged between two checks of status


def chain_of(mandates: dict[str, Mandate], mid: str) -> list[Mandate]:
    """The chain of a mandate, root first."""
    links: list[Mandate] = []
    seen: set[str] = set()
    cursor: str | None = mid
    while cursor is not None:
        if cursor in seen:
            raise ValueError("a mandate chain closes on itself")
        seen.add(cursor)
        links.append(mandates[cursor])
        cursor = mandates[cursor].parent
    return links[::-1]


def chain_faults(parties: dict[str, Party], mandates: dict[str, Mandate], mid: str,
                 revoked: frozenset[str] | set[str] = frozenset()) -> list[str]:
    """Every reason for which a chain carries no authority. An empty list means the chain stands."""
    faults: list[str] = []
    links = chain_of(mandates, mid)
    if parties[links[0].principal].kind == OPERATIVE:
        faults.append("the root principal is an operative: nobody answers")
    for parent, child in zip(links, links[1:]):
        if child.principal != parent.holder:
            faults.append(f"{child.id} was not issued by the holder of {parent.id}")
        if not parent.delegable:
            faults.append(f"{parent.id} may not be delegated")
        if not child.kinds <= parent.kinds:
            faults.append(f"{child.id} widens the kinds of act")
        if not child.matters <= parent.matters:
            faults.append(f"{child.id} widens the matters")
        ceiling = dict(parent.allowance)
        for resource, amount in child.allowance:
            if resource not in ceiling or amount > ceiling[resource]:
                faults.append(f"{child.id} widens the allowance of {resource}")
        if not (parent.term[0] <= child.term[0] and child.term[1] <= parent.term[1]):
            faults.append(f"{child.id} widens the term")
    for link in links:
        if link.id in revoked:
            faults.append(f"{link.id} is revoked")
    return faults


class Envelopes:
    """Use charged along the whole chain, so that the root's ceiling bounds everything beneath it."""

    def __init__(self, mandates: dict[str, Mandate], shared: bool = True):
        self.mandates = mandates
        self.shared = shared            # False reproduces the fault of independent ceilings
        self.used: dict[str, dict[str, int]] = {mid: {} for mid in mandates}

    def debit(self, mid: str, resource: str, amount: int) -> bool:
        links = chain_of(self.mandates, mid) if self.shared else [self.mandates[mid]]
        for link in links:
            ceiling = dict(link.allowance).get(resource)
            if ceiling is None or self.used[link.id].get(resource, 0) + amount > ceiling:
                return False
        for link in links:
            self.used[link.id][resource] = self.used[link.id].get(resource, 0) + amount
        return True

    def total(self, mid: str, resource: str) -> int:
        return self.used[mid].get(resource, 0)


# --------------------------------------------------------------------------------------------
# C. Acts, the chronicle and the fold

EXPRESSIVE, ASSERTIVE, DIRECTIVE, COMMISSIVE, DECLARATIVE = (
    "expressive", "assertive", "directive", "commissive", "declarative")

FORCE = {
    "say": EXPRESSIVE, "acknowledge": EXPRESSIVE,
    "report": ASSERTIVE, "deliver": ASSERTIVE, "assess": ASSERTIVE, "attest": ASSERTIVE,
    "request": DIRECTIVE, "claim": DIRECTIVE, "propose": DIRECTIVE,
    "offer": COMMISSIVE, "take": COMMISSIVE, "commit": COMMISSIVE, "grant": COMMISSIVE,
    "found": DECLARATIVE, "join": DECLARATIVE, "admit": DECLARATIVE, "leave": DECLARATIVE,
    "mandate": DECLARATIVE, "revoke": DECLARATIVE,
    "accept": DECLARATIVE, "release": DECLARATIVE, "cancel": DECLARATIVE, "assign": DECLARATIVE,
    "use": DECLARATIVE, "answer": DECLARATIVE, "convert": DECLARATIVE,
    "vote": DECLARATIVE, "amend": DECLARATIVE, "handover": ASSERTIVE,
}

GENESIS = "0" * 64


def canonical(value) -> str:
    return json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=False, default=_plain)


def _plain(value):
    if isinstance(value, (set, frozenset)):
        return sorted(value)
    if isinstance(value, Fraction):
        return [value.numerator, value.denominator]
    if isinstance(value, tuple):
        return list(value)
    if hasattr(value, "__dict__"):
        return value.__dict__
    raise TypeError(type(value))


def seal(previous: str, act: dict) -> str:
    return hashlib.sha256((previous + canonical(act)).encode("utf-8")).hexdigest()


@dataclass
class Constitution:
    """The part of a constitution that the sketch needs. The design's schema holds more."""
    positions: dict[str, frozenset[str]]          # choice rules: position -> kinds of act it may make
    open_positions: frozenset[str] = frozenset()  # boundary rule: positions that anyone answerable may take
    admit_by: dict[str, str] = field(default_factory=dict)  # position -> position whose holders admit to it
    count_by: str = "origin"                      # unit of count of the aggregation rule
    threshold: Fraction = Fraction(1, 2)          # a proposal passes with strictly more than this share
    conversions: dict[str, dict] = field(default_factory=dict)  # declared conversions between registers
    claim_route: str | None = None                # position that answers claims
    receiving: int = 0                            # receiving capacity of a round, in units of review


@dataclass
class State:
    parties: dict[str, Party]
    rules: Constitution
    positions: dict[tuple[str, str], int] = field(default_factory=dict)   # (party, position) -> since turn
    mandates: dict[str, Mandate] = field(default_factory=dict)
    revoked: set[str] = field(default_factory=set)
    commitments: dict[str, dict] = field(default_factory=dict)
    grants: dict[str, dict] = field(default_factory=dict)
    claims: dict[str, dict] = field(default_factory=dict)
    proposals: dict[str, dict] = field(default_factory=dict)
    assessments: list[dict] = field(default_factory=list)
    envelopes: dict[str, dict[str, int]] = field(default_factory=dict)    # mandate -> resource -> used
    journal: list[tuple[str, dict]] = field(default_factory=list)
    head: str = GENESIS

    def relational_digest(self) -> str:
        """The digest of everything that the fold computes, the journal and its bodies left out."""
        return hashlib.sha256(canonical({
            "positions": sorted((p, q, t) for (p, q), t in self.positions.items()),
            "mandates": sorted(self.mandates), "revoked": sorted(self.revoked),
            "commitments": self.commitments, "grants": self.grants, "claims": self.claims,
            "proposals": self.proposals, "assessments": self.assessments, "envelopes": self.envelopes,
            "rules": self.rules,
        }).encode("utf-8")).hexdigest()


def bearer(state: State, act: dict) -> str:
    """The party that answers for an act: the root principal of its mandate, or its author."""
    if act.get("mandate"):
        return chain_of(state.mandates, act["mandate"])[0].principal
    return act["author"]


def _holds(state: State, party: str, kind: str) -> bool:
    return any(kind in state.rules.positions.get(position, frozenset())
               for (holder, position) in state.positions if holder == party)


def _authority(state: State, act: dict) -> str | None:
    """A reason for refusal on grounds of authority, or None."""
    author, kind, turn = act["author"], act["kind"], act.get("turn", 0)
    if author not in state.parties:
        return "the author is unknown"
    party = state.parties[author]
    mid = act.get("mandate")
    if party.kind == OPERATIVE and mid is None:
        return "an operative acts only under a mandate"
    if mid is not None:
        if mid not in state.mandates:
            return "the mandate is unknown"
        mandate = state.mandates[mid]
        if mandate.holder != author:
            return "the mandate is held by another"
        faults = chain_faults(state.parties, state.mandates, mid, state.revoked)
        if faults:
            return faults[0]
        if kind not in mandate.kinds:
            return "the mandate does not cover this kind of act"
        if act.get("matter") is not None and act["matter"] not in mandate.matters:
            return "the mandate does not cover this matter"
        if not mandate.term[0] <= turn <= mandate.term[1]:
            return "the mandate is out of its term"
    return None


FREE = {"found", "say", "acknowledge", "claim", "leave", "join", "mandate", "revoke", "take", "accept",
        "release", "cancel", "assign", "use", "handover", "deliver"}  # governed by their own conditions


def step(state: State, act: dict) -> tuple[bool, str]:
    """Advance the state by one act. A refused act leaves the state as it was and is not journalled."""
    kind = act.get("kind")
    if kind not in FORCE:
        return False, "the kind of act is unknown"
    reason = _authority(state, act)
    if reason:
        return False, reason
    author, who, turn = act["author"], bearer(state, act), act.get("turn", 0)
    if kind not in FREE and not (_holds(state, author, kind) or _holds(state, who, kind)):
        return False, "no position held permits this kind of act"

    trial = copy.deepcopy(state)
    ok, reason = _effect(trial, act, author, who, turn)
    if not ok:
        return False, reason
    for mid, resource, amount in act.get("charges", []):       # use of a mandate's allowance
        for link in chain_of(trial.mandates, mid):
            used = trial.envelopes.setdefault(link.id, {})
            ceiling = dict(link.allowance).get(resource)
            if ceiling is None or used.get(resource, 0) + amount > ceiling:
                return False, f"the allowance of {resource} under {link.id} is exhausted"
            used[resource] = used.get(resource, 0) + amount
    trial.head = seal(state.head, act)
    trial.journal.append((trial.head, act))
    state.__dict__.update(trial.__dict__)
    return True, "admitted"


def _effect(s: State, act: dict, author: str, who: str, turn: int) -> tuple[bool, str]:
    kind = act["kind"]
    rules = s.rules

    if kind in ("say", "acknowledge", "report", "attest", "request"):
        return True, ""                                   # recorded; the relational state is unchanged

    if kind == "found":                                   # the first act of a chronicle names the first holders
        if s.journal or s.positions:
            return False, "a community is founded once, by the first act of its chronicle"
        if s.parties[author].kind == OPERATIVE:
            return False, "a founder answers for the founding"
        for party, position in act["holders"]:
            if position not in rules.positions:
                return False, "the founding names a position that the constitution lacks"
            s.positions[(party, position)] = turn
        return True, ""

    if kind == "join":
        position = act["position"]
        if position not in rules.open_positions:
            return False, "the position is not open"
        s.positions[(author, position)] = turn
        return True, ""
    if kind == "admit":
        position, whom = act["position"], act["whom"]
        admitter = rules.admit_by.get(position)
        if admitter is None or not ((author, admitter) in s.positions or (who, admitter) in s.positions):
            return False, "the author may not admit to this position"
        s.positions[(whom, position)] = turn
        return True, ""
    if kind == "leave":
        if (author, act["position"]) not in s.positions:
            return False, "the position is not held"
        del s.positions[(author, act["position"])]         # commitments stay as they are
        return True, ""

    if kind == "mandate":
        m: Mandate = act["grant_of"]
        if m.principal != author:
            return False, "a mandate is issued by its principal"
        if m.id in s.mandates:
            return False, "the mandate exists already"
        if m.parent is None and s.parties[author].kind == OPERATIVE:
            return False, "an operative issues no root mandate"
        trial = dict(s.mandates)
        trial[m.id] = m
        faults = chain_faults(s.parties, trial, m.id, s.revoked)
        if faults:
            return False, faults[0]
        s.mandates[m.id] = m
        return True, ""
    if kind == "revoke":
        mid = act["which"]
        if mid not in s.mandates:
            return False, "the mandate is unknown"
        if author not in {link.principal for link in chain_of(s.mandates, mid)}:
            return False, "only a principal of the chain revokes"
        s.revoked.add(mid)
        return True, ""

    if kind == "offer":
        s.commitments[act["id"]] = {
            "debtor": who, "creditor": act.get("to"), "matter": act.get("matter"),
            "amount": act.get("amount", 1), "unit": act.get("unit"), "due": act.get("due"),
            "status": "conditional", "by": author}
        return True, ""
    if kind == "take":
        c = s.commitments.get(act["id"])
        if c is None or c["status"] != "conditional":
            return False, "there is no such offer"
        if c["creditor"] not in (None, who, author):
            return False, "the offer is addressed to another"
        c["creditor"], c["status"] = who, "active"
        return True, ""
    if kind == "commit":
        s.commitments[act["id"]] = {
            "debtor": who, "creditor": act["to"], "matter": act.get("matter"),
            "amount": act.get("amount", 1), "unit": act.get("unit"), "due": act.get("due"),
            "status": "active", "by": author}
        return True, ""
    if kind in ("deliver", "handover"):
        c = s.commitments.get(act["id"])
        if c is None or c["status"] not in ("active", "presented"):
            return False, "there is no active commitment to perform"
        if c["debtor"] != who:
            return False, "performance is presented by the debtor's side"
        c["status"] = "presented" if kind == "deliver" else "handed over"
        c["artefact"] = act.get("artefact")
        return True, ""
    if kind in ("accept", "release", "assign"):
        c = s.commitments.get(act["id"])
        if c is None:
            return False, "there is no such commitment"
        if c["creditor"] != who:
            return False, "only the creditor's side may do this"
        if kind == "accept":
            if c["status"] != "presented":
                return False, "nothing has been presented"
            c["status"] = "discharged"
        elif kind == "release":
            if c["status"] not in ("active", "presented", "handed over", "cancelled"):
                return False, "the commitment is closed"
            c["status"] = "released"
        else:
            if c["status"] not in ("active", "presented"):
                return False, "the commitment is closed"
            c["creditor"] = act["to"]
        return True, ""
    if kind == "cancel":
        c = s.commitments.get(act["id"])
        if c is None or c["debtor"] != who or c["status"] not in ("active", "conditional"):
            return False, "only the debtor cancels an open commitment"
        c["status"] = "withdrawn" if c["status"] == "conditional" else "cancelled"
        return True, ""

    if kind == "grant":
        s.grants[act["id"]] = {"grantor": who, "holder": act["to"], "resource": act["resource"],
                               "ceiling": act["ceiling"], "used": 0}
        return True, ""
    if kind == "use":
        g = s.grants.get(act["id"])
        if g is None:
            return False, "there is no such grant"
        if g["grantor"] != who:
            return False, "use is recorded by the keeper of the resource"
        if g["used"] + act["amount"] > g["ceiling"]:
            return False, "the grant is exhausted"
        g["used"] += act["amount"]
        return True, ""

    if kind == "assess":
        s.assessments.append({"by": author, "for": who, "object": act["object"],
                              "criterion": act["criterion"], "value": act["value"]})
        return True, ""
    if kind == "claim":
        s.claims[act["id"]] = {"claimant": who, "against": act.get("against"),
                               "status": "open" if rules.claim_route else "open, no route declared"}
        return True, ""
    if kind == "answer":
        claim = s.claims.get(act["id"])
        if claim is None:
            return False, "there is no such claim"
        if rules.claim_route is None or (author, rules.claim_route) not in s.positions:
            return False, "the author is not on the route of claims"
        claim["status"] = "answered"
        claim["finding"] = act.get("finding")
        return True, ""

    if kind == "convert":
        rule = rules.conversions.get(act["rule"])
        if rule is None:
            return False, "no rule in force declares this conversion"
        whom = act["whom"]
        count = sum(1 for c in s.commitments.values() if c["debtor"] == whom and c["status"] == "discharged")
        if count < rule["at_least"]:
            return False, "the condition of the conversion does not hold"
        s.positions[(whom, rule["to_position"])] = turn
        return True, ""

    if kind == "propose":
        s.proposals[act["id"]] = {"change": act["change"], "votes": {}, "status": "open"}
        return True, ""
    if kind == "vote":
        proposal = s.proposals.get(act["id"])
        if proposal is None or proposal["status"] != "open":
            return False, "there is no open proposal"
        proposal["votes"][author] = bool(act["value"])
        return True, ""
    if kind == "amend":
        proposal = s.proposals.get(act["id"])
        if proposal is None or proposal["status"] != "open":
            return False, "there is no open proposal"
        share = tally(s.parties, proposal["votes"], rules.count_by)
        if not share > rules.threshold:
            return False, "the proposal has not passed under the rule in force"
        for key, value in proposal["change"].items():
            setattr(rules, key, value)
        proposal["status"] = "adopted"
        return True, ""

    return False, "the kind of act has no effect defined"


def replay(parties: dict[str, Party], rules: Constitution, journal: list[tuple[str, dict]]) -> State:
    """The state as a function of the record. Raises when the record does not seal or an act is refused."""
    state = State(parties=dict(parties), rules=copy.deepcopy(rules))
    for sealed, act in journal:
        ok, reason = step(state, copy.deepcopy(act))
        if not ok:
            raise ValueError(f"the record holds an act that the rules refuse: {reason}")
        if state.head != sealed:
            raise ValueError("the record does not seal")
    return state


# --------------------------------------------------------------------------------------------
# D. Set-off and novation on a graph of obligations
#
# An obligation o[(i, j)] > 0 says that i owes j that many units of one class in which set-off is
# agreed. The net obligation of a party is what it owes less what it is owed.


def net_obligation(o: dict[tuple[str, str], int]) -> dict[str, int]:
    net: dict[str, int] = {}
    for (i, j), amount in o.items():
        net[i] = net.get(i, 0) + amount
        net[j] = net.get(j, 0) - amount
    return net


def volume(o: dict[tuple[str, str], int]) -> int:
    return sum(o.values())


def find_cycle(o: dict[tuple[str, str], int]) -> list[str] | None:
    adjacent: dict[str, list[str]] = {}
    for (i, j), amount in o.items():
        if amount > 0:
            adjacent.setdefault(i, []).append(j)
    colour: dict[str, int] = {}
    path: list[str] = []

    def visit(u: str) -> list[str] | None:
        colour[u] = 1
        path.append(u)
        for v in sorted(adjacent.get(u, [])):
            if colour.get(v, 0) == 1:
                return path[path.index(v):]
            if colour.get(v, 0) == 0:
                found = visit(v)
                if found:
                    return found
        colour[u] = 2
        path.pop()
        return None

    for start in sorted(adjacent):
        if colour.get(start, 0) == 0:
            found = visit(start)
            if found:
                return found
    return None


def set_off(o: dict[tuple[str, str], int]):
    """Cancel cycles until none is left. Returns the residual, the volume cleared and the cycles."""
    o = {edge: amount for edge, amount in o.items() if amount > 0}
    cleared, cycles = 0, []
    while True:
        cycle = find_cycle(o)
        if cycle is None:
            return o, cleared, cycles
        edges = list(zip(cycle, cycle[1:] + cycle[:1]))
        least = min(o[edge] for edge in edges)
        for edge in edges:
            o[edge] -= least
        o = {edge: amount for edge, amount in o.items() if amount > 0}
        cleared += least * len(edges)
        cycles.append((cycle, least))


def novate(o: dict[tuple[str, str], int]):
    """Replace i owes j, j owes k by i owes k, as far as it goes. Each step needs the three consents."""
    o = {edge: amount for edge, amount in o.items() if amount > 0}
    steps = 0
    while True:
        found = None
        for (i, j) in sorted(o):
            for (j2, k) in sorted(o):
                if j2 == j:
                    found = (i, j, k)
                    break
            if found:
                break
        if found is None:
            return o, steps
        i, j, k = found
        least = min(o[(i, j)], o[(j, k)])
        o[(i, j)] -= least
        o[(j, k)] -= least
        if i != k:
            o[(i, k)] = o.get((i, k), 0) + least
        o = {edge: amount for edge, amount in o.items() if amount > 0}
        steps += 1


# --------------------------------------------------------------------------------------------
# E. Sufficiency of a record for a query


def factors_through(histories, observe, query):
    """Whether the query is a function of what the record keeps: constant on each fibre of observe.

    Returns (True, None), or (False, witness) with two answers that one observation leaves open.
    """
    seen: dict = {}
    for history in histories:
        observed, answer = observe(history), query(history)
        if observed in seen and seen[observed] != answer:
            return False, (observed, seen[observed], answer)
        seen.setdefault(observed, answer)
    return True, None


# --------------------------------------------------------------------------------------------
# F. Returns passed upstream along a graph of use
#
# passes[j] = {i: share}: the artefact j passes that share of what reaches it to its input i.
# The artefacts must be given so that every input comes before what uses it.


def upstream(order: list[str], passes: dict[str, dict[str, Fraction]], pot: dict[str, Fraction]):
    reaching = {name: Fraction(0) for name in order}
    for name, amount in pot.items():
        reaching[name] += amount
    kept: dict[str, Fraction] = {}
    for name in reversed(order):
        out = passes.get(name, {})
        passed = sum(out.values(), Fraction(0))
        if passed > 1:
            raise ValueError("an artefact passes on more than reaches it")
        kept[name] = reaching[name] * (1 - passed)
        for source, share in out.items():
            if order.index(source) >= order.index(name):
                raise ValueError("an input must come before what uses it")
            reaching[source] += reaching[name] * share
    return kept


# --------------------------------------------------------------------------------------------
# G. Counts by origin


def tally(parties: dict[str, Party], votes: dict[str, bool], count_by: str) -> Fraction:
    """The share in favour. By origin, each origin weighs one, divided equally among its voters."""
    if not votes:
        return Fraction(0)
    if count_by == "operative":
        return Fraction(sum(votes.values()), len(votes))
    groups: dict[str, list[bool]] = {}
    for voter, value in votes.items():
        groups.setdefault(origin(parties, voter), []).append(value)
    favour = sum(Fraction(sum(values), len(values)) for values in groups.values())
    return favour / len(groups)


# --------------------------------------------------------------------------------------------
# H. Gates, routes, powers and facets

ACCUMULATED = {"balance", "credit", "token", "score", "reputation", "count of contributions"}


def gate_of(rule: list[dict]) -> str:
    """The kind of gate an access rule sets. Each condition says what it reads and whether it spends it."""
    kinds = []
    for condition in rule:
        reads = condition["reads"]
        if reads in ACCUMULATED and condition.get("spent"):
            kinds.append("personal purchasing-power gate")     # whatever the unit is called
        elif reads in ACCUMULATED:
            kinds.append("standing gate")
        elif reads in ("membership", "position"):
            kinds.append("membership gate")
        elif reads in ("need", "queue", "rotation", "lot"):
            kinds.append(f"allocation by {reads}")
        else:
            kinds.append("open")
    for rank in ("personal purchasing-power gate", "standing gate", "membership gate"):
        if rank in kinds:
            return rank
    return kinds[0] if kinds else "open"


ROUTE = ("party", "forum", "evidence", "authority", "capacity")


def route_stands(route: dict[str, bool]) -> tuple[bool, list[str]]:
    """A claim has a route when all five complements hold. Returns what is missing."""
    missing = [name for name in ROUTE if not route.get(name)]
    return not missing, missing


def powers(recognised: dict[str, set[str]], holders: dict[str, set[str]], executable: dict[str, set[str]]):
    """For each party: what it may do and can do, what it can do without being empowered, and what it
    is empowered to do without the means."""
    report = {}
    for party in sorted(set(holders) | set(executable)):
        may = set().union(*[recognised.get(position, set()) for position in holders.get(party, set())]) \
            if holders.get(party) else set()
        can = executable.get(party, set())
        report[party] = {"usable": sorted(may & can), "overreach": sorted(can - may),
                         "unsupported": sorted(may - can)}
    return report


def facets(norm: dict, chronicle: list[dict]) -> dict:
    """The four facets of a norm, read from a record: regularity, adoption, operation, justification."""
    applicable = [a for a in chronicle if norm["applies"](a)]
    complying = [a for a in applicable if norm["complies"](a)]
    adopted = any(a["kind"] == "amend" and a.get("norm") == norm["id"] for a in chronicle)
    invoked = sum(1 for a in chronicle if a["kind"] in ("answer", "accept", "admit", "convert")
                  and norm["id"] in a.get("cites", ()))
    reasons = sum(1 for a in chronicle if a["kind"] in ("propose", "say") and a.get("about") == norm["id"]
                  and a.get("reason"))
    challenges = sum(1 for a in chronicle if a["kind"] == "claim" and a.get("about") == norm["id"])
    regularity = Fraction(len(complying), len(applicable)) if applicable else None
    marks = []
    if invoked and not adopted:
        marks.append("operative, unadopted")
    if adopted and not invoked:
        marks.append("adopted, inoperative")
    return {"regularity": regularity, "episodes": len(applicable), "adopted": adopted, "invoked": invoked,
            "reasons": reasons, "challenges": challenges, "marks": marks}
