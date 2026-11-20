"""A ground in miniature: the invariant part of a garden, with nothing in it that names a kind of project.

It keeps a store of content by digest, a sealed journal, accounts of energy and escrows, and it knows four
things about work: a need (energy locked on a statement of demand), a warranty (a claim with a stake), a
challenge (a counterexample put to a claim) and a fact (the outcome of a procedure that the site supplies).
What a claim means, and how a counterexample is decided, belongs to the site. Division, upkeep and the term
are left out: this is the framework's second rung, without heredity.

Conservation is checked after every act: energy emitted = energy in accounts + energy in escrow + energy dissipated.
"""
from __future__ import annotations

import hashlib
import json

COST = {"write": 0.5, "fact": 1.0, "challenge": 1.0}


def digest(content) -> str:
    text = content if isinstance(content, str) else json.dumps(content, sort_keys=True, ensure_ascii=False)
    return hashlib.sha256(text.encode("utf-8")).hexdigest()[:16]


class Refused(Exception):
    """An act that the ground does not carry out. Nothing changes when it is raised."""


class Ground:
    def __init__(self, site):
        self.site = site                      # {"name", "kinds": {kind: {"object": str, "decide": fn}}, "reserved": [...]}
        self.store: dict[str, object] = {}
        self.journal: list[dict] = []
        self.accounts: dict[str, float] = {}
        self.escrows: dict[str, dict] = {}
        self.needs: dict[str, dict] = {}
        self.claims: dict[str, dict] = {}
        self.persons: set[str] = set()
        self.emitted = self.dissipated = 0.0

    # ---- the record
    def _log(self, party: str, act: str, **what) -> dict:
        entry = {"n": len(self.journal), "party": party, "act": act, **what,
                 "before": self.journal[-1]["seal"] if self.journal else ""}
        entry["seal"] = digest(entry)
        self.journal.append(entry)
        self._conserved()
        return entry

    def sealed(self) -> bool:
        before = ""
        for entry in self.journal:
            body = {k: v for k, v in entry.items() if k != "seal"}
            if entry["before"] != before or digest(body) != entry["seal"]:
                return False
            before = entry["seal"]
        return True

    def _conserved(self) -> None:
        held = sum(self.accounts.values()) + sum(e["amount"] for e in self.escrows.values()) + self.dissipated
        if abs(held - self.emitted) > 1e-9:
            raise AssertionError(f"energy is not conserved: emitted {self.emitted}, held {held}")

    # ---- energy
    def admit(self, party: str, person: bool = False) -> None:
        self.accounts.setdefault(party, 0.0)
        if person:
            self.persons.add(party)

    def emit(self, person: str, amount: float) -> None:
        if person not in self.persons:
            raise Refused("energy enters only from a person at the membrane")
        self.emitted += amount
        self.accounts[person] += amount
        self._log(person, "emit", amount=amount)

    def _pay(self, party: str, amount: float, why: str) -> None:
        if self.accounts.get(party, 0.0) < amount - 1e-12:
            raise Refused(f"{party} has not the energy for {why}")
        self.accounts[party] -= amount
        self.dissipated += amount

    def transfer(self, giver: str, taker: str, amount: float) -> None:
        if amount < 0 or self.accounts.get(giver, 0.0) < amount - 1e-12:
            raise Refused("a transfer needs the energy it moves")
        self.accounts[giver] -= amount
        self.accounts[taker] += amount
        self._log(giver, "transfer", to=taker, amount=amount)

    def _lock(self, party: str, amount: float, terms: str) -> str:
        if amount <= 0 or self.accounts.get(party, 0.0) < amount - 1e-12:
            raise Refused(f"{party} has not the energy to lock {amount}")
        self.accounts[party] -= amount
        key = f"e{len(self.escrows)}"
        self.escrows[key] = {"amount": amount, "by": party, "terms": terms}
        return key

    def _release(self, key: str, shares: dict[str, float]) -> None:
        escrow = self.escrows[key]
        if abs(sum(shares.values()) - escrow["amount"]) > 1e-9:
            raise AssertionError("an escrow is released whole")
        for party, amount in shares.items():
            self.accounts[party] += amount
        escrow["amount"] = 0.0

    # ---- content
    def write(self, party: str, kind: str, content) -> str:
        self._pay(party, COST["write"], "writing")
        key = digest(content)
        self.store.setdefault(key, content)
        self._log(party, "write", kind=kind, content=key)
        return key

    # ---- demand, claims, challenges and facts
    def need(self, person: str, text: str, bounty: float, kinds: list[str]) -> str:
        if person not in self.persons:
            raise Refused("a need is posted by a person")           # an act reserved by every site
        for kind in kinds:
            if kind not in self.site["kinds"]:
                raise Refused(f"the site knows no claim of the kind {kind}")
        key = f"n{len(self.needs)}"
        self.needs[key] = {"text": text, "kinds": kinds, "by": person, "escrow": self._lock(person, bounty, f"bounty of {key}")}
        self._log(person, "need", need=key, bounty=bounty, kinds=kinds, text=digest(text))
        return key

    def warrant(self, maker: str, need: str, artefact: str, kind: str, terms: dict, stake: float) -> str:
        if kind not in self.needs[need]["kinds"]:
            raise Refused("the need asks for another kind of claim")
        if artefact not in self.store:
            raise Refused("a claim is about content in the store")
        key = f"c{len(self.claims)}"
        self.claims[key] = {"maker": maker, "need": need, "artefact": artefact, "kind": kind, "terms": terms,
                            "standing": "standing", "escrow": self._lock(maker, stake, f"stake of {key}"), "attempts": 0}
        self._log(maker, "warrant", claim=key, need=need, artefact=artefact, kind=kind, terms=terms, stake=stake)
        return key

    def challenge(self, challenger: str, claim: str, counterexample) -> bool:
        """Puts a counterexample to a claim. The site decides; a refuted claim pays its stake to the challenger."""
        c = self.claims[claim]
        if c["standing"] != "standing":
            raise Refused("the claim no longer stands")
        if challenger == c["maker"]:
            raise Refused("a maker does not collect its own stake")
        self._pay(challenger, COST["challenge"], "a challenge")
        refuted, note = self.site["kinds"][c["kind"]]["decide"](self.store[c["artefact"]], c["terms"], counterexample)
        c["attempts"] += 1
        if refuted:
            c["standing"], c["witness"] = "refuted", counterexample
            self._release(c["escrow"], {challenger: self.escrows[c["escrow"]]["amount"]})
        self._log(challenger, "challenge", claim=claim, counterexample=digest(counterexample), refuted=refuted, note=note[:160])
        return refuted

    def fact(self, party: str, name: str, *arguments):
        """Runs one of the site's procedures and records its outcome."""
        self._pay(party, COST["fact"], "a fact")
        outcome = self.site["facts"][name](*arguments)
        self._log(party, "fact", fact=name, outcome=outcome if isinstance(outcome, (bool, int, float, str)) else digest(outcome))
        return outcome

    def settle(self, need: str) -> dict:
        """At the end of the period: the bounty is divided among the claims that stand, and their stakes go back."""
        n = self.needs[need]
        mine = {k: c for k, c in self.claims.items() if c["need"] == need}
        standing = {k: c for k, c in mine.items() if c["standing"] == "standing"}
        bounty = self.escrows[n["escrow"]]["amount"]
        if standing:
            weight = {k: self.escrows[c["escrow"]]["amount"] for k, c in standing.items()}
            total = sum(weight.values())
            shares: dict[str, float] = {}
            for k, c in standing.items():
                shares[c["maker"]] = shares.get(c["maker"], 0.0) + bounty * weight[k] / total
            self._release(n["escrow"], shares)
            for k, c in standing.items():
                self._release(c["escrow"], {c["maker"]: self.escrows[c["escrow"]]["amount"]})
                c["standing"] = "stood"
        else:
            self._release(n["escrow"], {n["by"]: bounty})       # the bounty lapses to its source
        entry = self._log("ground", "settle", need=need, stood=sorted(standing), refuted=sorted(k for k, c in mine.items() if c["standing"] == "refuted"))
        return entry
