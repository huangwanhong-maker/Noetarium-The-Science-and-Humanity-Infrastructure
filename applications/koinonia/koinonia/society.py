"""A society of communities, each a constitution and a chronicle run by thesmos's fold through the API (ADR 0167, ADR 0169).

A society keeps its parties (persons, organisations, operatives and communities, with who controls each and which provider runs
an operative), its communities and its agents, and a clock of rounds. A community is a constitution, its mission and its record:
the acts admitted, in order. Every act is put to the community's own rules by ``science.communities.run``, which folds the whole
record again; an act refused changes nothing and is kept apart with its reason, so that what an agent tried and was not allowed is
seen. In a round every agent that wakes decides from the state at the round's start, and the acts of the round are put to each
community together; the record is the acts admitted, and seals as before (refused acts are never sealed).

Bodies of acts (findings, notes) are not read by the fold; the mission keeps them (``missions.Work``).
"""

from __future__ import annotations

import json
import threading
from pathlib import Path

from science import communities as api

from . import missions


class Community:
    def __init__(self, name: str, preset: str, constitution: dict, mission: str, record=None, refused=None, roles=None):
        self.name, self.preset, self.constitution, self.mission = name, preset, constitution, mission
        self.record: list[dict] = record or []
        self.refused: list[dict] = refused or []
        self.roles: dict = roles or {}  # names the work reads: the steward its keeper answers for, and so on
        self.result: dict = {}

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

    def found(self, name: str, preset: str, founder: str, holders: list[list[str]], mission: str, constitution: dict | None = None, roles=None) -> dict:
        c = constitution or api.preset(preset)
        admitted = api.admit(c)
        if not admitted["admitted"]:
            raise ValueError(f"the constitution of {name} is not admitted: {admitted['faults'][:2]}")
        self.communities[name] = Community(name, preset, c, mission, roles=roles)
        self.party(name, "community", label=name)
        out = self.submit(name, [{"author": founder, "kind": "found", "turn": self.turn, "holders": holders}])
        self.event(name, f"{name} founded by {founder} on the {preset} preset", kind="found")
        return out[0]

    # ---- acts ---------------------------------------------------------------------------------------------------------------------
    def submit(self, name: str, acts: list[dict]) -> list[dict]:
        """Puts acts to a community's rules at once; keeps those admitted and the refusals with their reasons. Each act may carry
        a ``_body`` (kept by the mission when admitted) and ``_by`` (the agent's note of why); neither reaches the fold."""
        c = self.communities[name]
        if not acts:
            if not c.result:
                c.result = api.run(c.constitution, self.engine_parties(), c.record)
            return []
        clean = [{k: v for k, v in a.items() if not k.startswith("_")} for a in acts]
        for a in clean:
            a.setdefault("fields", {})
            a.setdefault("turn", self.turn)
        result = api.run(c.constitution, self.engine_parties(), c.record + clean)
        outcomes = result["outcomes"][len(c.record):]
        admitted = [a for a, o in zip(clean, outcomes) if o["admitted"]]
        for a, raw, o in zip(clean, acts, outcomes):
            if o["admitted"]:
                self.admitted(name, a, raw)
            else:
                c.refused.append({"turn": self.turn, "act": a, "reason": o["reason"]})
                self.event(name, f"refused: {a['author']} {a['kind']} ({o['reason']})", kind="refused", who=a["author"])
        if len(admitted) != len(clean):  # the record is the admitted acts; its fold equals the fold with the refused left out
            result = api.run(c.constitution, self.engine_parties(), c.record + admitted)
        c.record.extend(admitted)
        c.result = result
        return outcomes

    def admitted(self, name: str, act: dict, raw: dict) -> None:
        f, kind, who = act["fields"], act["kind"], act["author"]
        if "_body" in raw and kind in ("deliver", "say", "report", "assess", "claim", "answer", "propose"):
            body = raw["_body"]
            if kind == "deliver":
                self.work.bodies[f["id"]] = body
            text = body.get("note") if isinstance(body, dict) else None
            if text:
                self.event(name, text, kind="note", who=who)
        if kind == "found":
            return
        words = {"join": f"joins as {f.get('position', '')}", "offer": f"offers for {item_of(f.get('id', ''))}", "take": f"takes {f.get('id', '')}",
                 "deliver": f"delivers {f.get('id', '')}", "accept": f"accepts {f.get('id', '')}", "release": f"releases {f.get('id', '')}",
                 "request": f"posts {f.get('id', '')}", "assess": f"assesses {f.get('object', '')}: {f.get('value', '')}",
                 "claim": f"claims against {f.get('against', '')}", "answer": f"answers {f.get('id', '')}: {f.get('finding', '')}",
                 "convert": f"converts {f.get('whom', '')} to member", "mandate": f"lends {act.get('grant', {}).get('holder', '')}",
                 "leave": f"leaves {f.get('position', '')}", "commit": f"enters a debt to {f.get('to', '')} ({f.get('id', '')})",
                 "assign": f"passes the claim {f.get('id', '')} on to {f.get('to', '')}", "propose": f"proposes {f.get('change.compact', f.get('id', ''))}",
                 "vote": f"votes {f.get('value', '')} on {f.get('id', '')}", "amend": f"enacts {f.get('id', '')}", "cancel": f"withdraws {f.get('id', '')}"}
        if kind == "release" and f.get("why"):
            words["release"] = f"releases {f.get('id', '')} ({f['why']})"
        if kind == "deliver" and f.get("matter") == "service":
            words["deliver"] = f"repays {f.get('id', '')} with {f.get('artefact', '')}"
        if kind != "say":
            why = f" ({raw['_by']})" if raw.get("_by") else ""
            self.event(name, f"{who} {words.get(kind, kind)}{why}", kind=kind, who=who)

    def event(self, community: str, text: str, kind: str = "", who: str = "") -> None:
        self.events.append({"n": len(self.events) + 1, "turn": self.turn, "community": community, "kind": kind, "who": who, "text": text})
        self.events = self.events[-2000:]

    # ---- the clock ----------------------------------------------------------------------------------------------------------------
    def round(self) -> dict:
        """One round: every agent that wakes decides from the round's start; the acts are put to each community; the mission's
        bookkeeping follows what was admitted."""
        from . import agents as _agents
        # Agents decide outside the lock (a model may take seconds to answer), from states that only this thread replaces; the
        # acts are then put to the communities, and the books kept, under it.
        with self.lock:
            self.turn += 1
        pending: dict[str, list[dict]] = {n: [] for n in self.communities}
        woke = []
        for agent in list(self.agents.values()):
            if agent.get("paused"):
                continue
            if _agents.wakes(self, agent):
                woke.append(agent["id"])
                for community, act in _agents.decide(self, agent):
                    pending.setdefault(community, []).append(act)
        with self.lock:
            for name, acts in pending.items():
                self.submit(name, acts)
            _agents.bookkeeping(self)
            self.save()
        return {"turn": self.turn, "woke": woke, "acts": sum(len(v) for v in pending.values())}

    # ---- persistence ----------------------------------------------------------------------------------------------------------------
    def to_json(self) -> dict:
        return {"format": "noetarium.society/1", "name": self.name, "description": self.description, "turn": self.turn, "parties": self.parties,
                "communities": [c.to_json() for c in self.communities.values()], "agents": self.agents,
                "work": self.work.to_json(), "events": self.events[-2000:]}

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
        for c in data["communities"]:
            s.communities[c["name"]] = Community(c["name"], c["preset"], c["constitution"], c["mission"], c["record"], c.get("refused"), c.get("roles"))
            s.submit(c["name"], [])
        return s

    def view(self) -> dict:
        """What a page is shown: the communities with their positions, commitments and heads, the agents, the work's tallies and
        the recent events."""
        from . import circulation as _circulation
        with self.lock:
            comms = []
            for c in self.communities.values():
                st = c.state
                comms.append({"name": c.name, "preset": c.preset, "mission": c.mission, "purpose": c.constitution.get("purpose", ""),
                              "positions": st.get("positions", []), "commitments": st.get("commitments", [])[-60:], "claims": st.get("claims", []),
                              "proposals": st.get("proposals", []), "head": c.result.get("head", {}), "acts": len(c.record), "refused": len(c.refused),
                              "refusals": c.refused[-8:]})
            return {"name": self.name, "description": self.description, "turn": self.turn, "parties": list(self.parties.values()),
                    "communities": comms, "agents": list(self.agents.values()), "work": self.work.counts(),
                    "batches": [{"id": b["id"], "entries": b["entries"], "settled": b.get("settled")} for b in self.work.batches],
                    "entries": self.work.entries, "findings": self.work.findings, "disputes": self.work.disputes[-40:], "events": self.events[-120:],
                    "circulation": _circulation.account(self) if _circulation.league(self) else None}
