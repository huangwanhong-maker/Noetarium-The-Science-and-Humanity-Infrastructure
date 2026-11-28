"""A node: communities hosted, each a constitution, its chronicle and its series of conduct, folded once in the API's engine and stepped
there under a lock of its own (the design of 2026-11-27, Sections 5.18, 6.4 and 9).

A node keeps for each community what replays it: the constitution, the acts admitted (each act the gateway submitted carrying the digest
of the episode that constituted it), their seals and the community's series of conduct (``science.communities.Session``'s journal). The
parties are read from the chronicle (a founding declares its founders, a presentation its agent and principal; thesmos's decision 0009),
so a community opens from no registry. Payloads (what was said: bodies, answers, tools' outputs) are kept by their digests beside the
series (``behaviour.Payloads``), and holdings' contents by the holding's name.

The node acts in a community only through the office its part ``passports`` names (``status by``): the gateway's office of status
reports the heads of the notaries whose witness a passport carries and the status of each foreign mandate at its home, once each window
(five minutes by default), each report an act constituted by the office's episode, so that the fold stays pure and the record says on
what it relied. A home the node does not host cannot be reached from here: the mandate is reported suspended until it can.

The node is pure of HTTP: ``gateway.Gateway`` answers the routes, ``serve.py`` serves them.
"""

from __future__ import annotations

import copy
import datetime as dt
import json
import threading
import time
from pathlib import Path

from science import communities as api

from .behaviour import Behaviour, Payloads, episode

RELEASE = {"ground": "thesmos 0014", "gateway": "koinonia gateway 1", "format": "noetarium.node/1"}


def iso(t: float) -> str:
    return dt.datetime.fromtimestamp(t, dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def parse_iso(s: str) -> float:
    return dt.datetime.strptime(s, "%Y-%m-%dT%H:%M:%SZ").replace(tzinfo=dt.timezone.utc).timestamp()


def write_into(target: Path, text: str, attempts: int = 40) -> None:
    """Writes ``text`` to a temporary file of this thread's own beside ``target`` and moves it into place. On Windows the move is refused
    while another program holds the target open; it is retried for about two seconds before the refusal is raised."""
    tmp = target.with_name(f"{target.stem}.{threading.get_ident()}.tmp")
    tmp.write_text(text, encoding="utf-8")
    for attempt in range(attempts):
        try:
            tmp.replace(target)
            return
        except PermissionError:
            if attempt == attempts - 1:
                raise
            time.sleep(0.05)


class Hosted:
    """A community hosted at the node."""

    def __init__(self, name: str, constitution: dict, preset: str = "", office: str = "", turn: int = 0, record=None, seals=None, journal=None,
                 tools=None, content=None, checked=None, holds=None, witnessed=None, exits=None, folder: Path | None = None):
        self.name, self.constitution, self.preset, self.office, self.turn = name, constitution, preset, office, turn
        self.record: list[dict] = record or []
        self.seals: list[str] = seals or []
        self.journal: list[dict] = journal or []
        self.tools: dict = tools or {}
        self.content: dict = content or {}       # holding → its content, where the node keeps one
        self.checked: dict = checked or {}       # foreign mandate → the instant its status was last checked
        self.holds: list[dict] = holds or []     # holds on records (retention): {name, record, from, until}
        self.witnessed: list[dict] = witnessed or []  # heads of this community (a notary) at which it witnessed another's head
        self.exits: list[dict] = exits or []
        self.lock = threading.RLock()
        self.changed = threading.Condition(self.lock)
        self.session: api.Session | None = None
        self.payloads = Payloads(folder / "payloads" if folder else None)
        self.received: dict[int, int] = {}       # turn → acts received from outside agents

    def to_json(self) -> dict:
        return {"format": "noetarium.hosted/1", "name": self.name, "preset": self.preset, "office": self.office, "turn": self.turn,
                "constitution": self.constitution, "record": self.record, "seals": self.seals, "journal": self.journal, "tools": self.tools,
                "content": self.content, "checked": self.checked, "holds": self.holds, "witnessed": self.witnessed, "exits": self.exits}


class Node:
    def __init__(self, folder: Path | None = None, name: str = "node", url: str = "http://127.0.0.1:9230/", window: float = 300.0,
                 rate: int = 120, operator: str = "", clock=None):
        self.folder, self.name, self.url, self.window, self.rate, self.operator = folder, name, url.rstrip("/") + "/", window, rate, operator
        self.clock = clock or time.time
        self.last = ""
        self.lock = threading.RLock()
        self.saving = threading.Lock()
        self.communities: dict[str, Hosted] = {}
        self.principals: dict[str, dict] = {}   # a principal's credential's digest → {home, party}; the credential itself is never kept
        self.notary = ""                        # the community that witnesses households' heads at this node
        self.journal = Behaviour(folder / "behaviour.jsonl" if folder else None)  # the node's own journal: what concerns no community
        self.origins: dict[str, list[float]] = {}
        if folder is not None:
            self.load()

    # ---- time -------------------------------------------------------------------------------------------------------------------
    def now(self) -> str:
        """The node's instant, in UTC to the second, never earlier than the last it gave (acts in a chronicle are in order of instant)."""
        t = iso(self.clock())
        self.last = max(self.last, t)
        return self.last

    # ---- persistence ------------------------------------------------------------------------------------------------------------
    def load(self) -> None:
        meta = self.folder / "node.json"
        if meta.is_file():
            m = json.loads(meta.read_text(encoding="utf-8"))
            self.principals, self.notary, self.last = m.get("principals", {}), m.get("notary", ""), m.get("last", "")
        for f in sorted((self.folder / "communities").glob("*/hosted.json")):
            d = json.loads(f.read_text(encoding="utf-8"))
            self.communities[d["name"]] = Hosted(d["name"], d["constitution"], d.get("preset", ""), d.get("office", ""), d.get("turn", 0), d["record"], d["seals"],
                                                 d.get("journal"), d.get("tools"), d.get("content"), d.get("checked"), d.get("holds"), d.get("witnessed"),
                                                 d.get("exits"), folder=f.parent)

    def save(self, community: str | None = None) -> None:
        """Writes the node's file and the hosted communities' files, each to a temporary file of its own and then into place. Saves are
        serialised (requests to several communities run at once), and a replacement Windows refuses for a moment, because another program
        (an indexer, a scanner, an editor) holds the file open, is retried."""
        if self.folder is None:
            return
        with self.saving:
            self.folder.mkdir(parents=True, exist_ok=True)
            write_into(self.folder / "node.json", json.dumps({"format": "noetarium.node-store/1", "name": self.name, "url": self.url,
                                                              "principals": self.principals, "notary": self.notary, "last": self.last},
                                                             ensure_ascii=False))
            for name in ([community] if community else list(self.communities)):
                h = self.communities[name]
                d = self.folder / "communities" / name
                d.mkdir(parents=True, exist_ok=True)
                write_into(d / "hosted.json", json.dumps(h.to_json(), ensure_ascii=False))

    # ---- communities ------------------------------------------------------------------------------------------------------------
    def hosted(self, name: str) -> Hosted:
        if name not in self.communities:
            raise KeyError(name)
        return self.communities[name]

    def session(self, name: str) -> api.Session:
        """The community's session, opened from its record, seals and series on first use; a record that does not replay as sealed is
        refused, never repaired."""
        h = self.hosted(name)
        with h.lock:
            if h.session is None:
                h.session = api.Session(h.constitution, [], h.record, seals=h.seals or None, journal=h.journal or None)
                if h.session.opened.get("replays") is False or h.session.opened["refused"]:
                    raise ValueError(f"the record of {name} does not replay as sealed (it differs at {h.session.opened.get('differs_at')})")
            return h.session

    def found(self, name: str, preset: str, founder: str, holders: list[list[str]], parties: list[dict], office: str = "",
              constitution: dict | None = None, mission: str = "") -> dict:
        """Founds a community at the node on a preset (or a constitution given), its founding naming it and declaring its parties."""
        if name in self.communities:
            raise ValueError(f"a community named {name} is hosted here")
        c = copy.deepcopy(constitution or api.preset(preset))
        c["name"] = name
        if mission:
            c["purpose"] = mission
        admitted = api.admit(c)
        if not admitted["admitted"]:
            raise ValueError(f"the constitution of {name} is not admitted: {admitted['faults'][:2]}")
        office = office or (c.get("passports") or {}).get("status by", "")
        folder = self.folder / "communities" / name if self.folder else None
        h = Hosted(name, c, preset, "", folder=folder)
        self.communities[name] = h
        act = {"author": founder, "kind": "found", "turn": 0, "when": self.now(), "holders": holders, "fields": {"community": name, "parties": parties},
               "conduct": episode(founder, "act", community=name, node=self.name, route="found", address="local")}
        h.session = api.Session(c, [], [])
        r = self.step(name, [act])
        if not r["outcomes"][0].get("admitted"):
            del self.communities[name]
            raise ValueError(f"the founding of {name} is refused: {r['outcomes'][0]['reason']}")
        h.office = next((p for p, pos in holders if pos == office), "")
        self.save(name)
        return r["outcomes"][0]

    def act(self, author: str, kind: str, fields: dict | None = None, mandate: str = "", body: str = "", **extra) -> dict:
        a = {"author": author, "kind": kind, "fields": dict(fields or {}), **extra}
        if mandate:
            a["mandate"] = mandate
        if body:
            a["body"] = body
        return a

    def step(self, name: str, items: list[dict], state: bool = False, brief: list | None = None) -> dict:
        """Puts items to the community's rules on its session under its lock (acts, each perhaps with the episode that constitutes it
        in "conduct"; {"observe": episode}), each act given the community's turn and the node's instant where it names none; keeps the
        record, seals and series, and wakes those waiting for news."""
        h = self.hosted(name)
        with h.lock:
            s = self.session(name)
            for i in items:
                if "observe" in i or "disclose" in i:
                    continue
                i.setdefault("turn", h.turn)
                i.setdefault("when", self.now())
                i.setdefault("fields", {})
                if "conduct" in i:
                    i["conduct"].setdefault("turn", i["turn"])
                    i["conduct"]["community"] = i["conduct"].get("community") or name
                    i["conduct"]["node"] = i["conduct"].get("node") or self.name
                    if not i["conduct"].get("begun"):
                        i["conduct"]["begun"] = i["when"]
            for i in items:
                if "observe" in i:
                    i["observe"].setdefault("community", name)
                    i["observe"].setdefault("node", self.name)
                    i["observe"].setdefault("turn", h.turn)
            r = s.step(items, state=state, brief=brief)
            h.record, h.seals, h.journal = s.record, s.seals, s.journal
            self.save(name)
            h.changed.notify_all()
            return r

    def observe(self, name: str, ep: dict) -> dict:
        """Records conduct that constitutes no act (a read, a refusal before the fold, a run that failed) in the community's series."""
        return self.step(name, [{"observe": ep}])["outcomes"][0]

    def tick(self, name: str, turns: int = 1) -> int:
        """Advances the community's clock of rounds; terms in turns (a taking's, a question's) are read against it."""
        h = self.hosted(name)
        with h.lock:
            h.turn += turns
            self.save(name)
            return h.turn

    def state(self, name: str) -> dict:
        return self.session(name).state()

    # ---- parties known, credentials and passports --------------------------------------------------------------------------------
    def known(self, name: str) -> list[dict]:
        """The parties the community's chronicle declares (its founding, its presentations)."""
        return self.state(name).get("declared", [])

    def credentials(self, name: str) -> dict[str, dict]:
        """The digests of the bearer credentials of the passports presented here, each with its holder, mandate, home, principal,
        provider and model, read from the chronicle's acts of presentation."""
        out = {}
        for a in self.hosted(name).record:
            if a["kind"] != "present":
                continue
            p = a["fields"].get("passport")
            if isinstance(p, str):
                try:
                    p = json.loads(p)
                except ValueError:
                    continue
            m = (p or {}).get("mandate", {})
            if m.get("credential"):
                out[m["credential"]] = {"party": p["holder"]["id"], "mandate": m["id"], "home": (p.get("issuer") or {}).get("community", ""),
                                        "principal": (p.get("principal") or {}).get("id", ""), "provider": p["holder"].get("provider", ""),
                                        "model": p["holder"].get("model", ""), "passport": p}
        return out

    def principal(self, digest: str) -> dict | None:
        return self.principals.get(digest)

    # ---- the office of status ---------------------------------------------------------------------------------------------------
    def report_head(self, name: str, of: str, head: dict, why: str = "") -> dict:
        """The office of status reports another community's head (a notary's, a home's) to the community, constituted by its episode."""
        h = self.hosted(name)
        a = self.act(h.office, "report", {"head of": of, "size": head["size"], "root": head["root"]})
        a["conduct"] = episode(h.office, "check status", community=name, node=self.name, holdings=[], about=f"head of {of}", why=why or "a passport names it")
        return self.step(name, [a])["outcomes"][0]

    def status_at_home(self, home: str, mandate: str) -> tuple[str, dict]:
        """A foreign mandate's status at its home, as the node can learn it: standing or revoked (with the home's head) where the home is
        hosted here; suspended where it cannot be reached."""
        if home not in self.communities:
            return "suspended", {}
        st = self.state(home)
        by = {m["id"]: m for m in st.get("mandates", [])}
        m, seen = by.get(mandate), set()
        while m is not None and m["id"] not in seen:
            seen.add(m["id"])
            if m.get("revoked"):
                return "revoked", self.session(home).head
            m = by.get(m.get("parent") or "")
        return ("standing" if mandate in by else "revoked"), self.session(home).head

    def check_status(self, name: str, mandate: str, force: bool = False) -> str:
        """Checks a foreign mandate's status at its home once each window (or when forced) and enters what was found as a report by the
        office of status; answers the status as the community now reads it."""
        h = self.hosted(name)
        with h.lock:
            foreign = {f["mandate"]: f for f in self.state(name).get("foreign", [])}
            f = foreign.get(mandate)
            if f is None:
                return "unknown"
            now = self.now()
            last = h.checked.get(mandate)
            if not force and last and parse_iso(now) - parse_iso(last) < self.window:
                return f["status"]
            status, head = self.status_at_home(f["home"], mandate)
            a = self.act(h.office, "report", {"mandate status": mandate, "status": status})
            a["conduct"] = episode(h.office, "check status", community=name, node=self.name, about=f"mandate {mandate} at {f['home']}", status=status,
                                   **({"home size": head["size"], "home root": head["root"]} if head else {"home": "unreachable"}))
            o = self.step(name, [a])["outcomes"][0]
            h.checked[mandate] = now
            self.save(name)
            return status if o.get("admitted") else f["status"]

    def witness(self, head: dict) -> dict:
        """The node's notary witnesses a head ({community, size, root}): its office attests it in the notary's chronicle; answers the
        witness as a passport lists it."""
        if not self.notary:
            raise ValueError("this node keeps no notary")
        n = self.hosted(self.notary)
        with n.lock:
            w = api.witness(n.constitution, [], n.record, n.office, head, notary=self.notary, turn=n.turn, when=self.now())
            if not w["admitted"]:
                raise ValueError(f"the notary does not witness the head: {w['reason']}")
            # The witness's act carries no episode: a verifier rebuilds its envelope from the witness's own fields, so a field "episode"
            # would leave it unverifiable. The office's episode is recorded as conduct naming the act's seal instead.
            o = self.step(self.notary, [dict(w["act"])])["outcomes"][0]
            if not o.get("admitted") or {k: self.session(self.notary).head[k] for k in ("size", "root")} != {k: w["head"][k] for k in ("size", "root")}:
                raise ValueError(f"the notary's witness did not seal as the engine said: {o.get('reason')}")
            self.observe(self.notary, episode(n.office, "act", route="witness", about=f"witness of {head.get('community')}", act="attest", seal=o["seal"]))
            n.witnessed.append({"of": head.get("community", ""), "size": w["head"]["size"], "root": w["head"]["root"]})
            self.save(self.notary)
            return w["witness"]

    # ---- the archive: retention and moving a community -------------------------------------------------------------------------
    def holds(self, name: str) -> list[dict]:
        """The holds on a community's payloads: those declared, and one on every payload concerning a party to a claim still open."""
        h = self.hosted(name)
        out = list(h.holds)
        open_claims = [k for k in self.state(name).get("claims", []) if k.get("status") in ("open", "pending", "under review", "referred")]
        parties = set()
        for k in open_claims:
            parties |= {k.get(f) for f in ("claimant", "author", "against", "respondent", "bearer") if k.get(f)}
        controllers = {p["id"]: p.get("controller", "") for p in self.known(name)}

        def answers(who: str) -> bool:  # the party, or an operative whose chain of control reaches a party to the claim
            seen = set()
            while who and who not in seen:
                if who in parties:
                    return True
                seen.add(who)
                who = controllers.get(who, "")
            return False
        for d, rec in h.payloads.index.items():
            if answers(rec["party"]):
                out.append({"name": f"an open claim of {rec['party']}", "record": d, "from": "0000-01-01"})
        return out

    def retain(self, name: str, today: str, schedule: list[dict] | None = None) -> dict:
        """Applies the community's schedule of retention (its policy of records' ``retention``, else the design's default) to its
        payloads at a date; the chronicle and the series are never cut."""
        from .behaviour import DEFAULT_SCHEDULE
        h = self.hosted(name)
        with h.lock:
            sched = schedule or ((h.constitution.get("records") or {}).get("retention")) or DEFAULT_SCHEDULE
            r = h.payloads.retain(sched, self.holds(name), today)
            if r["removed"]:
                from .behaviour import episode as _ep
                self.observe(name, _ep(h.office or "the node", "other", described="retention", removed=len(r["removed"]), today=today))
            return r

    def export(self, name: str, folder: Path, date: str = "") -> dict:
        """Packs a community for another node as a BagIt bag made and validated by archeion (``science.archive``): its constitution,
        acts, seals, series of conduct, tools and contents, and its payloads."""
        from science import archive
        h = self.hosted(name)
        data = folder / "data"
        data.mkdir(parents=True, exist_ok=True)
        keep = {k: v for k, v in h.to_json().items() if k not in ("checked",)}
        (data / "community.json").write_text(json.dumps(keep, ensure_ascii=False, indent=1), encoding="utf-8", newline="\n")
        (data / "payloads.json").write_text(json.dumps({"index": h.payloads.index, "markers": h.payloads.markers,
                                                        "values": {d: h.payloads.get(d) for d in h.payloads.index}}, ensure_ascii=False),
                                            encoding="utf-8", newline="\n")
        payload = {n: archive.path(data / n) for n in ("community.json", "payloads.json")}
        info = [["Source-Organization", f"the node {self.name}"], ["External-Description", f"the community {name} moved to another node"],
                ["External-Identifier", name]]
        bag = archive.ask("package", payload=payload, info=info, date=date or self.now()[:10])
        for path, text in bag["tags"].items():
            (folder / path).write_text(text, encoding="utf-8", newline="\n")
        files = {p.relative_to(folder).as_posix(): archive.path(p) for p in folder.rglob("*") if p.is_file()}
        return archive.ask("validate", files=files)

    def take(self, folder: Path) -> dict:
        """Takes a community moved here as a bag: validated by archeion, then opened from its record, seals and series, which must
        replay as sealed; its chain of custody gains this node."""
        from science import archive
        files = {p.relative_to(folder).as_posix(): archive.path(p) for p in folder.rglob("*") if p.is_file()}
        v = archive.ask("validate", files=files)
        if not v.get("valid"):
            raise ValueError(f"the bag does not validate: {v}")
        d = json.loads((folder / "data" / "community.json").read_text(encoding="utf-8"))
        if d["name"] in self.communities:
            raise ValueError(f"a community named {d['name']} is hosted here")
        h = Hosted(d["name"], d["constitution"], d.get("preset", ""), d.get("office", ""), d.get("turn", 0), d["record"], d["seals"], d.get("journal"),
                   d.get("tools"), d.get("content"), {}, d.get("holds"), d.get("witnessed"), d.get("exits"),
                   folder=(self.folder / "communities" / d["name"]) if self.folder else None)
        kept = json.loads((folder / "data" / "payloads.json").read_text(encoding="utf-8"))
        for k, rec in kept["index"].items():
            if kept["values"].get(k) is not None:
                h.payloads.put(kept["values"][k], rec["class"], rec["party"], rec["events"].get("created", ""))
                h.payloads.index[k] = rec
        h.payloads.markers = kept.get("markers", [])
        h.payloads.save()
        self.communities[h.name] = h
        try:
            s = self.session(h.name)
        except ValueError:
            del self.communities[h.name]
            raise
        self.save(h.name)
        return {"community": h.name, "replays": True, "head": dict(s.head), "valid": v.get("valid")}

    # ---- limits -----------------------------------------------------------------------------------------------------------------
    def admit_rate(self, origin: str) -> float:
        """0 when a request from this origin is within the node's rate (requests a minute), else the seconds to wait."""
        with self.lock:
            t = self.clock()
            seen = [x for x in self.origins.get(origin, []) if t - x < 60.0]
            if len(seen) >= self.rate:
                self.origins[origin] = seen
                return max(1.0, 60.0 - (t - seen[0]))
            seen.append(t)
            self.origins[origin] = seen
            return 0.0

    UNITS = {"deliveries": "deliver", "applications": "apply", "questions": "question", "petitions": "petition", "presentations": "present",
             "claims": "claim", "reviews": "assess"}

    def admit_receiving(self, name: str, kind: str = "") -> bool:
        """Whether the community can receive one more act of this kind from outside agents this round: its constitution's ``receiving``
        counts the acts its unit names by its first word ("deliveries reviewed" counts deliveries), and every act where the unit names
        no kind."""
        h = self.hosted(name)
        receiving = h.constitution.get("receiving") or {}
        per = int(receiving.get("per round") or 0)
        counted = self.UNITS.get(str(receiving.get("unit", "")).split(" ")[0])
        if counted and kind != counted:
            return True
        with h.lock:
            n = h.received.get(h.turn, 0)
            if per and n >= per:
                return False
            h.received[h.turn] = n + 1
            return True
