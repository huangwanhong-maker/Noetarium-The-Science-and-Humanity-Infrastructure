"""A society of communities, each a constitution and a chronicle run by thesmos's fold through the API (ADR 0167, ADR 0169).

A society keeps its parties (persons, organisations, operatives and communities, with who controls each and which provider runs
an operative), its communities and its agents, and a clock of rounds. A community is a constitution, its mission and its record:
the acts admitted, in order. Every act is put to the community's own rules on its session (``science.communities.Session``): the
record folded once in the API's engine and stepped there, as the full fold would fold it; an act refused changes nothing and is kept
apart with its reason, so that what an agent tried and was not allowed is seen. In a round every agent that wakes decides from the state at the round's start, and the acts of the round are put to each
community together; the record is the acts admitted, and seals as before (refused acts are never sealed).

Bodies of acts (findings, notes) are not read by the fold; the mission keeps them (``missions.Work``).

A model's prompts and answers are an agent's epistemic behaviour, and the behaviour journal keeps them only where a disclosure
agreement covers the agent (thesmos's decision 0010): the society keeps its agreements, each with its terms in thesmos's written form
(``noetarium.disclosure/1``), its basis and who consented, and every model call it records names the agreement that allowed it.
Outside an agreement the journal keeps only that the call took place, with its outcome and cost. A case declares its agreement, its
maker consenting for the invented principals of its agents.
"""

from __future__ import annotations

import json
import threading
from pathlib import Path

from science import communities as api

from . import missions, story
from .behaviour import Behaviour


class Community:
    def __init__(self, name: str, preset: str, constitution: dict, mission: str, record=None, refused=None, roles=None):
        self.name, self.preset, self.constitution, self.mission = name, preset, constitution, mission
        self.record: list[dict] = record or []
        self.refused: list[dict] = refused or []
        self.roles: dict = roles or {}  # names the work reads: the steward its keeper answers for, and so on
        self.result: dict = {}
        self.session: api.Session | None = None  # the record folded once in the API's engine (Phase 9), opened on the first act

    def to_json(self) -> dict:
        return {"name": self.name, "preset": self.preset, "constitution": self.constitution, "mission": self.mission,
                "record": self.record, "refused": self.refused[-300:], "roles": self.roles}

    @property
    def state(self) -> dict:
        return self.result.get("state", {})

    def holders(self, position: str) -> list[str]:
        return [p["party"] for p in self.state.get("positions", []) if p["position"] == position]

    def holds(self, party: str, position: str) -> bool:
        return party in self.holders(position)

    def commitments(self, **match) -> list[dict]:
        return [c for c in self.state.get("commitments", []) if all(c.get(k) == v for k, v in match.items())]

    def briefing(self, party: str) -> dict:
        """What the community's record tells a party that acts in it (thesmos's briefing): its mission, what the party may do with
        each holding and by which norm, its duties and the open work. Given for the agents lent to the community, on each fold."""
        return self.result.get("briefings", {}).get(party, {})

    def requests(self, matter: str | None = None) -> list[dict]:
        """The items posted as work by request acts, in order, each once."""
        seen, out = set(), []
        for a in self.record:
            if a["kind"] == "request" and a["fields"].get("id") not in seen and (matter is None or a["fields"].get("matter") == matter):
                seen.add(a["fields"].get("id"))
                out.append(a)
        return out


def item_of(commitment_id: str) -> str:
    """Commitment ids are '<item>~<agent>': the batch, dispute or trial they perform, and who offered."""
    return commitment_id.split("~")[0]


class Society:
    def __init__(self, name: str, folder: Path | None = None):
        self.name, self.folder = name, folder
        self.turn = 0
        self.parties: dict[str, dict] = {}
        self.communities: dict[str, Community] = {}
        self.agents: dict[str, dict] = {}
        self.work = missions.Work()
        self.events: list[dict] = []
        self.lock = threading.RLock()
        self.asker = None  # set by the caller: agents.Asker, real or a stand-in
        self.description = ""
        self.behaviour = Behaviour(folder / "behaviour.jsonl" if folder else None)
        self.snapshot: dict | None = None
        self.book = None    # a society writing a report keeps its book (book.Book)
        self.dry = False    # in a dry run every agent follows its declared procedure
        self.parallel = 1   # agents that decide at once within a round
        self.agreements: list[dict] = []  # disclosure agreements: what of the agents' epistemic behaviour the journal may keep

    # ---- parties and communities ------------------------------------------------------------------------------------------------
    def party(self, ident: str, kind: str, controller: str = "", provider: str = "", label: str = "") -> dict:
        p = {"id": ident, "kind": kind, "controller": controller, "provider": provider, "label": label or ident}
        self.parties[ident] = p
        return p

    def origin(self, ident: str) -> str:
        seen = set()
        while ident in self.parties and self.parties[ident]["controller"] and ident not in seen:
            seen.add(ident)
            ident = self.parties[ident]["controller"]
        return ident

    def engine_parties(self) -> list[dict]:
        return [{k: p[k] for k in ("id", "kind", "controller", "provider")} for p in self.parties.values()]

    def found(self, name: str, preset: str, founder: str, holders: list[list[str]], mission: str, constitution: dict | None = None, roles=None, via: str = "party") -> dict:
        c = constitution or api.preset(preset)
        admitted = api.admit(c)
        if not admitted["admitted"]:
            raise ValueError(f"the constitution of {name} is not admitted: {admitted['faults'][:2]}")
        self.communities[name] = Community(name, preset, c, mission, roles=roles)
        self.party(name, "community", label=name)
        out = self.submit(name, [{"author": founder, "kind": "found", "turn": self.turn, "holders": holders, "_via": via}])
        self.event(name, f"{name} founded by {founder} on the {preset} preset", kind="found")
        return out[0]

    # ---- disclosure agreements (thesmos's decision 0010) ----------------------------------------------------------------------------
    def agree(self, ident: str, *, principal: str, consented_by: str, basis: str, agents: list[str] | None = None, every_agent: bool = False,
              kinds: tuple = ("model call",), admits: tuple = ("review", "research"), never: tuple = ("intent",), until: int | None = None) -> dict:
        """Declares a disclosure agreement and records it in the behaviour journal with its basis and who consented: the terms in
        thesmos's written form, the agents it covers (those named; with ``every_agent``, every agent of the society, the maker of an
        invented case consenting for each principal), from this round to ``until`` (exclusive) or without end."""
        terms = {"format": "noetarium.disclosure/1", "id": ident, "principal": principal, "agents": list(agents or []),
                 "scope": {"kinds": list(kinds), "from": self.turn, **({"to": until - 1} if until is not None else {})},
                 "purpose": {"admits": list(admits), "never": list(never)},
                 "custody": {"holding": "the behaviour journal of " + self.name, "custodian": consented_by, "retention": 0},
                 "access": {"principal": True}, "cost": {"borne by": "principal", "unit": "tokens"},
                 "withdrawal": {"by": "either", "notice": 0, "then": "kept"}}
        g = {"id": ident, "terms": terms, "every agent": every_agent, "basis": basis, "consented by": consented_by, "since": self.turn, "until": until}
        self.agreements.append(g)
        self.behaviour.record(self.turn, consented_by, "agreement", ident, terms=terms, basis=basis, every_agent=every_agent, until=until)
        return g

    def end_agreement(self, ident: str, by: str) -> bool:
        """Ends an agreement from this round, by either side; what was recorded is kept, as its terms say."""
        for g in self.agreements:
            if g["id"] == ident and (g["until"] is None or g["until"] > self.turn):
                g["until"] = self.turn
                self.behaviour.record(self.turn, by, "agreement ended", ident)
                return True
        return False

    def covering(self, agent: str, kind: str = "model call") -> str | None:
        """The agreement that covers an agent's epistemic behaviour of a kind in this round, or None."""
        for g in self.agreements:
            t = g["terms"]
            if self.turn < g["since"] or (g["until"] is not None and self.turn >= g["until"]) or kind not in t["scope"]["kinds"]:
                continue
            if g.get("every agent") or agent in t["agents"] or (not t["agents"] and self.origin(agent) == t["principal"] and agent != t["principal"]):
                return g["id"]
        return None

    # ---- acts ---------------------------------------------------------------------------------------------------------------------
    def brief(self, name: str) -> list[dict]:
        """The agents lent to a community, each under its mandate there, whose briefings each fold of its record gives (a society
        writing a book briefs its agents by its own roles)."""
        if self.book is not None:
            return []
        return [{"as": ident, "mandate": a["communities"][name]["mandate"]} for ident, a in self.agents.items() if name in a.get("communities", {})]

    def session(self, name: str) -> api.Session:
        """The community's record folded once in the API's engine and kept there (``science.communities.Session``), opened from the
        record on first use and opened again when the society's parties change (an agent lent mid-run): the state the full fold of the
        record with the society's present parties gives, as before."""
        c = self.communities[name]
        parties = self.engine_parties()
        if c.session is None:
            c.session = api.Session(c.constitution, parties, c.record)
        elif c.session.parties != parties:
            c.session.reopen(parties=parties)
        return c.session

    def submit(self, name: str, acts: list[dict]) -> list[dict]:
        """Puts acts to a community's rules at once, stepped on its session: the outcomes are those the full fold of the record and the
        acts gives, since a refused act changes nothing; keeps those admitted and the refusals with their reasons. Each act may carry a
        ``_body`` (kept by the mission when admitted) and ``_by`` (the agent's note of why); neither reaches the fold."""
        c = self.communities[name]
        if not acts and c.result:
            return []
        clean = [{k: v for k, v in a.items() if not k.startswith("_")} for a in acts]
        for a in clean:
            a.setdefault("fields", {})
            a.setdefault("turn", self.turn)
        s = self.session(name)
        stepped = s.step(clean, state=True, brief=self.brief(name))
        outcomes = stepped["outcomes"]
        result = {"seals": list(s.seals), "state": stepped["state"], "head": dict(s.head), "digest": stepped["digest"], "briefings": stepped.get("briefings", {})}
        if not acts:
            c.result = result
            return []
        admitted = [a for a, o in zip(clean, outcomes) if o["admitted"]]
        for a, raw, o in zip(clean, acts, outcomes):
            # every act put to a community, with its outcome, is in the behaviour journal: the chronicle's seal or the reason
            self.behaviour.record(self.turn, a["author"], "act" if o["admitted"] else "refused", name, kind=a["kind"], fields=a.get("fields", {}),
                                  grant=a.get("grant"), charges=a.get("charges"), body=a.get("body", ""), via=raw.get("_via", "agent" if a["author"] in self.agents else "party"),
                                  admitted=o["admitted"], reason=o["reason"], seal=o.get("seal"), why=raw.get("_by", ""),
                                  carried=raw.get("_body") if isinstance(raw.get("_body"), dict) else None)
            if o["admitted"]:
                self.admitted(name, a, raw)
            else:
                c.refused.append({"turn": self.turn, "act": a, "reason": o["reason"]})
                self.event(name, story.refusal(self, a, o["reason"]), kind="refused", who=a["author"])
        c.record.extend(admitted)  # the record is the admitted acts; its fold equals the fold with the refused left out
        c.result = result
        return outcomes

    def admitted(self, name: str, act: dict, raw: dict) -> None:
        f, kind, who = act["fields"], act["kind"], act["author"]
        if self.book is not None:
            from . import partnership
            partnership.on_admitted(self, name, act, raw)
        if "_body" in raw and kind in ("deliver", "say", "report", "assess", "claim", "answer", "propose"):
            body = raw["_body"]
            if kind == "deliver":
                self.work.bodies[f["id"]] = body
            text = body.get("note") if isinstance(body, dict) else None
            if text:
                self.event(name, text, kind="note", who=who)
        if kind == "found":
            return
        text, theme = story.sentence(self, name, act)
        why = f" ({raw['_by']})" if raw.get("_by") else ""
        if kind != "say":
            self.event(name, text + why, kind=theme, who=who)
        elif not (isinstance(raw.get("_body"), dict) and raw["_body"].get("note")):
            self.event(name, text, kind="words", who=who)

    def directory(self, reader: str | None = None) -> list[dict]:
        """The communities' profiles, read from their records by thesmos (decision 0006): each mission, admission policy, route for
        claims, offices and holders, posted work and holdings, as far as each community's policy of records shows them; with no
        reader, the public profiles an agent reads before it chooses a community to join."""
        with self.lock:
            return [self.session(name).profile(reader) for name in self.communities]

    def event(self, community: str, text: str, kind: str = "", who: str = "") -> None:
        self.events.append({"n": len(self.events) + 1, "turn": self.turn, "community": community, "kind": kind, "who": who, "text": text})
        self.events = self.events[-2000:]

    # ---- the clock ----------------------------------------------------------------------------------------------------------------
    def round(self) -> dict:
        """One round, under the society's lock: every agent that wakes decides from the round's start; the acts are put to each
        community; the mission's books are kept; the league's books are kept by one plan; the notary attests what the round showed.
        A person's act from the page waits for the round to end. The society is saved even if the round fails, and the failure is
        recorded in the behaviour journal."""
        from . import agents as _agents, circulation as _circulation
        with self.lock:
            self.turn += 1
            pending: dict[str, list[dict]] = {n: [] for n in self.communities}
            woke, acted = [], set()
            try:
                waking = []
                for agent in list(self.agents.values()):
                    if agent.get("paused"):
                        continue
                    reason = _agents.wakes(self, agent)
                    if reason:
                        woke.append(agent["id"])
                        # what it wakes to: the reason, and its briefing in each community it acts in, read from the record
                        self.behaviour.record(self.turn, agent["id"], "wake", "", why=reason, briefed=_agents.briefed(self, agent))
                        waking.append((agent, agent["memory"].get("last_choice")))
                # every agent that woke decides from the round's start; with ``parallel`` above one they decide at once (their models
                # asked together), and their acts are gathered in the agents' order, so that the round's acts do not depend on timing
                def decided(w):  # a fault in one agent's decision is recorded in the journal and costs it the round, not the society
                    try:
                        return _agents.decide(self, w[0])
                    except Exception as error:
                        self.behaviour.record(self.turn, w[0]["id"], "fault", "", error=f"{type(error).__name__}: {str(error)[:300]}")
                        return []
                if self.parallel > 1 and len(waking) > 1:
                    from concurrent.futures import ThreadPoolExecutor
                    with ThreadPoolExecutor(max_workers=self.parallel) as pool:
                        decisions = list(pool.map(decided, waking))
                else:
                    decisions = [decided(w) for w in waking]
                for (agent, before), chosen in zip(waking, decisions):
                    for community, act in chosen:
                        pending.setdefault(community, []).append(act)
                        acted.add(agent["id"])
                    choice = agent["memory"].get("last_choice")
                    self.behaviour.record(self.turn, agent["id"], "decide", "", acts=[{"community": c, "kind": a["kind"], "item": a.get("fields", {}).get("id", "")} for c, a in chosen],
                                          chose=choice if choice is not before and choice and choice.get("turn") == self.turn else None)
                for name, acts in pending.items():
                    self.submit(name, acts)
                if self.book is not None:
                    from . import partnership
                    partnership.bookkeeping(self)
                else:
                    _agents.bookkeeping(self)
                    _circulation.notary(self, acted)
            except Exception as error:
                self.behaviour.record(self.turn, "the society", "fault", "", error=f"{type(error).__name__}: {error}")
                raise
            finally:
                self.save()
                self.snapshot = None
            return {"turn": self.turn, "woke": woke, "acts": sum(len(v) for v in pending.values())}

    # ---- persistence ----------------------------------------------------------------------------------------------------------------
    def to_json(self) -> dict:
        return {"format": "noetarium.society/1", "name": self.name, "description": self.description, "turn": self.turn, "parties": self.parties,
                "communities": [c.to_json() for c in self.communities.values()], "agents": self.agents,
                "work": self.work.to_json(), "events": self.events[-2000:], "book": self.book.to_json() if self.book is not None else None,
                "dry": self.dry, "parallel": self.parallel, "agreements": self.agreements,
                "journal": {"entries": self.behaviour.n, "head": self.behaviour.head}}

    def save(self) -> None:
        if self.folder is None:
            return
        self.folder.mkdir(parents=True, exist_ok=True)
        temp = self.folder / "society.tmp"
        temp.write_text(json.dumps(self.to_json(), ensure_ascii=False, indent=1), encoding="utf-8")
        temp.replace(self.folder / "society.json")

    @classmethod
    def load(cls, folder: Path) -> "Society":
        data = json.loads((folder / "society.json").read_text(encoding="utf-8"))
        s = cls(data["name"], folder)
        s.turn, s.parties, s.agents, s.events, s.description = data["turn"], data["parties"], data["agents"], data.get("events", []), data.get("description", "")
        s.work = missions.Work(data.get("work"))
        if data.get("book"):
            from .book import Book
            s.book = Book(data["book"])
        s.dry, s.parallel = data.get("dry", False), data.get("parallel", 1)
        s.agreements = data.get("agreements", [])
        for c in data["communities"]:
            s.communities[c["name"]] = Community(c["name"], c["preset"], c["constitution"], c["mission"], c["record"], c.get("refused"), c.get("roles"))
            s.submit(c["name"], [])
        return s

    def view(self) -> dict:
        """What a page is shown. A round holds the lock while its agents decide, which may take seconds when a model is asked; the
        page is then shown the view taken when the society was last free, and never waits on a model."""
        got = self.lock.acquire(blocking=self.snapshot is None)
        if not got:
            return self.snapshot
        try:
            self.snapshot = self._view()
            return self.snapshot
        finally:
            self.lock.release()

    def _view(self) -> dict:
        """What a page is shown: the communities with their positions, commitments and heads, the agents, the work's tallies and
        the recent events."""
        from . import circulation as _circulation
        with self.lock:
            comms = []
            for c in self.communities.values():
                st = c.state
                comms.append({"name": c.name, "preset": c.preset, "mission": c.mission, "purpose": c.constitution.get("purpose", ""),
                              "positions": st.get("positions", []), "commitments": st.get("commitments", [])[-60:], "claims": st.get("claims", []),
                              "proposals": st.get("proposals", []), "mandates": [{"id": m["id"], "holder": m["holder"], "revoked": m["revoked"]} for m in st.get("mandates", [])], "head": c.result.get("head", {}), "acts": len(c.record), "refused": len(c.refused),
                              "refusals": c.refused[-8:],
                              # what it holds and the norms of access to it (thesmos's holdings), and the latest accesses
                              "holdings": st.get("holdings", []), "norms": st.get("norms", []), "open": st.get("open", {}),
                              "accesses": st.get("accesses", [])[-12:], "access_count": st.get("access_count", 0),
                              # the items opened on routes and the register of rulings (thesmos's decision 0011)
                              "items": st.get("items", [])[-40:], "rulings": st.get("rulings", []),
                              # posted work and the state of each item (thesmos's decision 0012)
                              "posted work": st.get("work", [])[-60:],
                              # how it decides, and the compacts it ratified with their reservations and withdrawals (thesmos's decision 0013)
                              "aggregation": st.get("aggregation", {}), "compact records": st.get("compact records", [])})
            return {"name": self.name, "description": self.description, "turn": self.turn, "parties": list(self.parties.values()),
                    "communities": comms, "agents": list(self.agents.values()), "work": self.work.counts(),
                    "batches": [{"id": b["id"], "entries": b["entries"], "settled": b.get("settled"), "owner": b.get("owner", "commons")} for b in self.work.batches],
                    "journal": {"entries": self.behaviour.n, "head": self.behaviour.head},
                    "agreements": [{k: g.get(k) for k in ("id", "basis", "consented by", "since", "until", "every agent")} for g in self.agreements],
                    "entries": self.work.entries, "findings": self.work.findings, "disputes": self.work.disputes[-60:], "events": self.events[-400:],
                    "circulation": _circulation.account(self) if _circulation.league(self) else None,
                    "book": self.book_view() if self.book is not None else None}

    def book_view(self) -> dict:
        """What a page is shown of a book being written: the charter and the organisation, the regime and rule in force, the
        chapters with each section's stage and versions, the petitions and tools, the milestones."""
        from . import partnership
        b = self.book
        secs = {}
        for sid, x in b.sections.items():
            vs = b.versions_of(sid)
            secs[sid] = {"id": sid, "title": x["title"], "question": x["question"], "stage": "withdrawn" if x.get("withdrawn") else x["stage"],
                         "versions": len(vs), "series": x.get("series", []), "regime": x.get("regime"), "reviewers": x.get("reviewers", []),
                         "words": len(b.versions[x["adopted"]]["text"].split()) if x.get("adopted") in b.versions else 0, "reopened": bool(x.get("reopened"))}
        return {"call": {k: b.call.get(k) for k in ("task", "charter", "charter adopted", "charter decided by", "ratifications", "members", "founded at", "refused by")},
                "title": b.outline.get("title") or b.zero.get("title", ""), "chapters": b.outline.get("chapters") or [
                    {"id": c["id"], "kind": c["kind"], "title": c["title"], "sections": []} for c in b.zero.get("chapters", [])],
                "sections": secs, "counts": b.counts(), "regime": partnership.regime(self) if partnership.league(self) else "",
                "rule": partnership.rule_in_force(self, "adopt section"), "amendments": b.amendments, "allotment": b.allotment,
                "petitions": b.petitions, "tools": {k: {"about": v.get("about"), "admitted": v.get("admitted"), "by": v.get("by")} for k, v in b.tools.items()},
                "events": b.events[-60:], "adopted": bool(b.adopted_book)}

    def section_view(self, sid: str) -> dict:
        b = self.book
        x = b.sections[sid]
        return {"section": {k: x.get(k) for k in ("id", "title", "question", "targets", "stage", "series", "regime", "reviewers", "adopted", "words")},
                "results": [{"id": r["id"], "tool": r["tool"], "by": r.get("by"), "replicated by": r.get("replicated_by"), "summary": r.get("summary", "")[:900]} for r in b.results_of(sid)],
                "versions": [{"id": v["id"], "n": v["n"], "by": v["by"], "model": v.get("model"), "words": len(v["text"].split()), "text": v["text"][:6000],
                              "reviews": [{k: r.get(k) for k in ("by", "community", "model", "scores", "level", "recommend", "comments")} for r in b.reviews_of(v["id"])],
                              "audit": [a for a in b.audits if a["version"] == v["id"]]} for v in b.versions_of(sid)]}
