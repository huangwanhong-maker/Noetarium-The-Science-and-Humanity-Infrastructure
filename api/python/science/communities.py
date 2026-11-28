"""Communities for scripts in Python and the works' servers (ADR 0167): constitutions admitted by form, a community's record run on its
constitution by thesmos's fold, decisions counted by origin with their sensitivity to who controls whom, and the communities domain's
jar, asked of the API's own engine (``api/cpp/engines/communities/main.cpp``, built on first use).

    from science import communities
    communities.admit(communities.preset("commons"))                  # {admitted, faults, profile}
    r = communities.run(constitution, parties, acts)                  # {outcomes, seals, state, head, digest}
    communities.run(constitution, parties, acts, seals=r["seals"])    # ... and "replays": whether the record seals as before
    communities.count(parties, ballots, unit="origin", rule="majority")
    communities.jar(regime="provision with duty", need=4, free_riders=0.5, epistemic=0.1, runs=20)
    communities.holdings(constitution, parties, acts)                 # {holdings, norms, open, accesses}
    communities.access(constitution, parties, acts, "ana", "shelf", "consult")   # {decision, fold}
    communities.briefing(constitution, parties, acts, "ana/agent", mandate="m1")  # {mission, entries, may, duties, work, ...}
    communities.run(constitution, parties, acts, brief=["ana", {"as": "ana/agent", "mandate": "m1"}])   # ... and "briefings"
    communities.profile(constitution, parties, acts)                  # the public profile a directory lists; reader="ana" for hers

Passports (thesmos's decision 0007): a person's household (the preset ``household``) issues a passport for a mandate it has entered,
and anyone who knows the household's head, or the head of a notary that witnessed it, verifies it from the document alone.

    p = communities.issue_passport(household, parties, acts, grant, home="ana-household", at="2026-12-03T00:00:00Z")
    # {passport, credential, acts, head}: the credential is shown once; only its digest is in the mandate
    w = communities.witness(league, league_parties, league_acts, "notary", p["head"])   # {witness, act, head}
    communities.verify_passport(p["passport"], homes=[p["head"]], at=..., host="atelier", credential=p["credential"])

Sessions (Phase 9 of the design of 2026-11-27): a record folded once in one kept process of the engine and stepped there, with the
community's behaviour journal beside it as a series of its own; views of the session; the two records verified as joined, a party's
exit package packed as BagIt, the records as W3C PROV, value readings by declared mechanisms with H7's marks, measures of power, and
the registries.

    s = communities.Session(constitution, parties, acts)             # the record folded once
    s.step([act, {**act, "conduct": episode}, {"observe": episode}])   # outcomes, seals, heads; Stale when another step moved it
    s.profile(); s.briefing("ana"); s.items("ana"); s.work(); s.mandate("ana/agent"); s.behaviour(); s.exit("ana")
    communities.join(s); communities.exit_package(s, "ana", folder); communities.provenance(s)
    communities.reading(s, "share of decisions", act="answer", field="outcome", kinds="adopted, returned")
    communities.power(s, "tenure by office", position="steward"); communities.listing(s, node, "notary", "2026-12-01")

The computations are composed in ``api/cpp/science/communities.hpp``, which documents the shapes of parties, acts and ballots.
"""

from __future__ import annotations

import hashlib
import json
import secrets
import subprocess
import threading
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ROOT = Path(__file__).resolve().parents[3]
SUBDOMAIN = ROOT / "domains" / "communities" / "human-ai"
ENGINE = Engine("api/communities", source=ROOT / "api" / "cpp" / "engines" / "communities" / "main.cpp",
                extra_includes=[SUBDOMAIN / "jar" / "include", SUBDOMAIN / "lending" / "include"], env="NOETARIUM_COMMUNITIES_ENGINE")
# Measures of power and value readings rest on nomos and tekmerion (the domain's trajectories): an engine of their own, so that the
# engine of sessions stays small (the design's Section 9.7).
READINGS = Engine("api/community-readings", source=ROOT / "api" / "cpp" / "engines" / "community-readings" / "main.cpp",
                  extra_includes=[SUBDOMAIN / "jar" / "include", SUBDOMAIN / "lending" / "include"], env="NOETARIUM_COMMUNITY_READINGS_ENGINE")
PRESETS = ROOT / "domains" / "communities" / "presets" / "data"
REGIMES = ("currency", "currency with income", "currency with targeted income", "provision, open", "provision with duty")
UNITS = ("person", "origin", "operative", "community")


def presets() -> list[str]:
    """The names of the presets kept in the communities domain."""
    return sorted(p.stem for p in PRESETS.glob("*.json"))


def preset(name: str) -> dict:
    """A preset constitution, as written."""
    path = PRESETS / f"{name}.json"
    if not path.is_file():
        raise ValueError(f"no preset {name!r}; the presets are {', '.join(presets())}")
    return json.loads(path.read_text(encoding="utf-8"))


def ask(op: str, **request) -> dict:
    if not ENGINE.status().get("current"):
        ENGINE.build(log=lambda *_: None)
    return ENGINE.ask(f"communities.{op}", **request)


def admit(constitution: dict | str) -> dict:
    return ask("admit", constitution=constitution)


def run(constitution: dict | str, parties: list[dict], acts: list[dict], seals: list[str] | None = None, brief: list | None = None) -> dict:
    """The record run on its constitution. With ``brief`` (parties, or {as, mandate, purpose}), each party's briefing on the state
    folded, under "briefings"."""
    request = {"constitution": constitution, "parties": parties, "acts": acts}
    if seals is not None:
        request["seals"] = seals
    if brief:
        request["brief"] = brief
    return ask("run", **request)


AIMS = ("consult", "use", "deposit", "modify", "lend", "withdraw")


def holdings(constitution: dict | str, parties: list[dict] | None = None, acts: list[dict] | None = None) -> dict:
    """What the community holds after its record, its norms of access in words, the kinds open where no norm speaks, and the accesses
    admitted (thesmos's holdings, decision 0006)."""
    return ask("holdings", constitution=constitution, parties=parties or [], acts=acts or [])


def access(constitution: dict | str, parties: list[dict], acts: list[dict], party: str, holding: str, aim: str, **fields) -> dict:
    """Whether ``party`` may ``aim`` the holding now: the norms' decision (the norm that grants or refuses it, or the absence of any) and
    whether the fold would admit the act. ``fields``: amount, purpose, mandate, kind, within, digest, source, licence, to, until, turn."""
    if aim not in AIMS:
        raise ValueError(f"no aim of access {aim!r}; the aims are {', '.join(AIMS)}")
    return ask("access", constitution=constitution, parties=parties, acts=acts, **{"as": party, "holding": holding, "aim": aim}, **fields)


def profile(constitution: dict | str, parties: list[dict] | None = None, acts: list[dict] | None = None, reader: str | None = None) -> dict:
    """What a reader may know of the community without joining: its mission, admission policy and route for claims (always), and the
    parts of its constitution, offices and holders, posted work and holdings that its policy of records shows to the reader. With no
    reader, the public profile a directory lists."""
    request = {"constitution": constitution, "parties": parties or [], "acts": acts or []}
    if reader:
        request["as"] = reader
    return ask("profile", **request)


def briefing(constitution: dict | str, parties: list[dict], acts: list[dict], party: str, mandate: str = "", purpose: str = "") -> dict:
    """What ``party`` learns from the community's record: the mission, its positions and the acts open to it, what it may do with each
    holding and by which norm, its duties and the open work."""
    return ask("briefing", constitution=constitution, parties=parties, acts=acts, **{"as": party, "mandate": mandate, "purpose": purpose})


def count(parties: list[dict], ballots: list[dict], unit: str = "origin", rule: str = "majority") -> dict:
    if unit not in UNITS:
        raise ValueError(f"no unit of count {unit!r}; the units are {', '.join(UNITS)}")
    return ask("count", parties=parties, ballots=ballots, unit=unit, rule=rule)


def statements(constitution: dict | str, texts: list[str] | None = None, parties: list[dict] | None = None, acts: list[dict] | None = None) -> dict:
    """The constitution's rules as institutional statements (or the texts given, read by the grammar), each duty and prohibition
    measured over the record when parties and acts are given."""
    request = {"constitution": constitution, "parties": parties or [], "acts": acts or []}
    if texts is not None:
        request["texts"] = texts
    return ask("statements", **request)


def jar(regime: str = "provision with duty", runs: int = 20, **setting) -> dict:
    if regime not in REGIMES:
        raise ValueError(f"no arrangement {regime!r}; the arrangements are {', '.join(REGIMES)}")
    return ask("jar", regime=regime, runs=runs, **setting)


# ---- Passports (thesmos's decision 0007) ---------------------------------------------------------------------------------------------

def passport(constitution: dict | str, parties: list[dict], acts: list[dict], mandate: str, home: str | None = None, at: str = "",
             model: str = "", label: str = "", node: str = "", status_at: str = "") -> dict:
    """The passport of ``mandate`` as the home's record stands: {admitted, reason, head, passport}. ``home`` names the issuing
    community (its constitution's name by default); ``at`` is the instant of issue, ISO 8601 in UTC."""
    request = {"constitution": constitution, "parties": parties, "acts": acts, "mandate": mandate, "at": at, "model": model, "label": label,
               "node": node, "status_at": status_at}
    if home:
        request["home"] = home
    return ask("passport.issue", **request)


def credential() -> tuple[str, str]:
    """A new bearer credential and its SHA-256 digest; the mandate keeps the digest, and the credential goes to its holder alone."""
    token = secrets.token_urlsafe(32)
    return token, hashlib.sha256(token.encode("utf-8")).hexdigest()


def issue_passport(constitution: dict | str, parties: list[dict], acts: list[dict], grant: dict, home: str | None = None, at: str = "",
                   turn: int | None = None, model: str = "", label: str = "", node: str = "", status_at: str = "") -> dict:
    """Mints a bearer credential for the grant's holder, enters the act by which the grant's principal issues the mandate with the
    credential's digest, and assembles its passport. Answers {passport, credential, acts, head}, the credential shown once and kept
    nowhere; or raises ValueError with the reason the home refused."""
    token, digest = credential()
    issued = dict(grant, credential=digest)
    act = {"author": issued["principal"], "kind": "mandate", "turn": turn if turn is not None else (acts[-1].get("turn", 0) if acts else 0), "grant": issued}
    record = list(acts) + [act]
    r = passport(constitution, parties, record, issued["id"], home=home, at=at, model=model, label=label, node=node, status_at=status_at)
    if not r["admitted"]:
        raise ValueError(f"no passport for {issued['id']}: {r['reason']}")
    return {"passport": r["passport"], "credential": token, "acts": record, "head": r["head"]}


def witness(constitution: dict | str, parties: list[dict], acts: list[dict], attester: str, head: dict, notary: str | None = None,
            turn: int | None = None, when: str = "", mandate: str = "") -> dict:
    """A notary witnesses a head ({community, size, root}): ``attester`` attests it in the notary's record under the notary's rules.
    Answers {admitted, reason, head, witness, act}: the witness as a passport lists it (add it to its issuer's "witnesses"), the act to
    add to the notary's record, and the notary's head after it."""
    request = {"constitution": constitution, "parties": parties, "acts": acts, "attester": attester, "head": head, "when": when, "mandate": mandate}
    if notary:
        request["notary"] = notary
    if turn is not None:
        request["turn"] = turn
    return ask("passport.witness", **request)


def verify_passport(passport: dict | str, homes: list[dict] | None = None, notaries: list[dict] | None = None, at: str = "", host: str = "",
                    credential: str = "") -> dict:
    """Whether a passport holds for a verifier who knows the heads of ``homes`` (named by the passport's issuer unless they say
    otherwise) and of ``notaries`` (each naming its community), shown at the instant ``at``, at the community ``host``, with the bearer
    ``credential``; a part left out is not checked. Answers {read, holds, reason, ...}, or {read: false, faults} for a document that
    does not read."""
    return ask("passport.verify", passport=passport, homes=homes or [], notaries=notaries or [], at=at, host=host, credential=credential)


# ---- Sessions (Phase 9; the design's Section 9.7) -----------------------------------------------------------------------------------

class Lost(EngineError):
    """The process that kept a session ended before it answered; the session is opened again from its record."""


class Stale(EngineError):
    """A step named a head the session no longer stands at: another step moved it first."""


class Host:
    """One process of the engine kept for sessions (started with --serve): a request a line, answered a line, one at a time. A process
    that ends, or an engine rebuilt since it started, is replaced, and the sessions it held are opened again by their owners."""

    def __init__(self, engine: Engine | None = None):
        self.engine = engine or ENGINE
        self.lock = threading.RLock()
        self.proc: subprocess.Popen | None = None
        self.generation = 0
        self.stamp: int | None = None

    def alive(self) -> bool:
        if self.proc is None or self.proc.poll() is not None:
            return False
        try:
            return self.engine.program.stat().st_mtime_ns == self.stamp
        except OSError:
            return False

    def _start(self) -> None:
        if not self.engine.status().get("current"):
            self.engine.build(log=lambda *_: None)
        args, _ = self.engine._args()
        self.proc = subprocess.Popen([*args, "--serve"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        self.stamp = self.engine.program.stat().st_mtime_ns
        self.generation += 1

    def ask(self, op: str, timeout: float = 600.0, **request) -> dict:
        payload = (json.dumps(clean({"op": f"communities.{op}", **request}), allow_nan=False) + "\n").encode("utf-8")
        with self.lock:
            if not self.alive():
                self.close()
                self._start()
            proc = self.proc
            watchdog = threading.Timer(timeout, proc.kill)
            watchdog.start()
            try:
                proc.stdin.write(payload)
                proc.stdin.flush()
                line = proc.stdout.readline()
            except OSError:
                line = b""
            finally:
                watchdog.cancel()
            if not line.strip():
                self.close()
                raise Lost(f"the engine's process for sessions ended while asked {op}")
        answer = json.loads(line.decode("utf-8", errors="replace"))
        if not answer.get("ok"):
            raise EngineError(str(answer.get("error", "the engine failed")))
        return answer["result"]

    def close(self) -> None:
        with self.lock:
            if self.proc is not None:
                try:
                    self.proc.kill()
                except OSError:
                    pass
                self.proc = None


_HOST: Host | None = None
_HOST_LOCK = threading.Lock()


def host() -> Host:
    """The process of this Python process that keeps its sessions."""
    global _HOST
    with _HOST_LOCK:
        if _HOST is None:
            _HOST = Host()
        return _HOST


class Session:
    """A community's record folded once in the engine and stepped there (communities.open, step, view, close).

    The session keeps the record of the acts admitted (as given, each constituted act with the episode that constituted it in its field
    "episode"), their seals and the behaviour journal's entries (a series of the community's own), so that it can open itself again from
    its record if the engine's process is lost. A step names the head the session stood at, and is refused as stale (Stale) when another
    holder of the same session moved it first. The outcomes of its steps are those a full fold of the record and the new acts gives."""

    def __init__(self, constitution: dict | str, parties: list[dict] | None = None, acts: list[dict] | None = None, seals: list[str] | None = None,
                 journal: list[dict] | None = None, host_: Host | None = None, state: bool = False, brief: list | None = None):
        self.constitution = constitution
        self.parties = [dict(p) for p in (parties or [])]
        self.host = host_ or host()
        self.record: list[dict] = []
        self.seals: list[str] = []
        self.journal: list[dict] = [dict(e) for e in (journal or [])]
        self.id, self.generation, self.head, self.digest, self.community = "", -1, {}, "", ""
        self.opened = self._open([dict(a) for a in (acts or [])], seals, state=state, brief=brief)

    def _request(self, acts: list[dict], seals: list[str] | None, state: bool = False, brief: list | None = None) -> dict:
        request = {"constitution": self.constitution, "parties": self.parties, "acts": acts}
        if seals is not None:
            request["seals"] = seals
        if self.journal:
            request["journal"] = self.journal
        if self.id:
            request["replaces"] = self.id
        if state:
            request["state"] = True
        if brief:
            request["brief"] = brief
        return request

    def _open(self, acts: list[dict], seals: list[str] | None = None, state: bool = False, brief: list | None = None) -> dict:
        r = self.host.ask("open", **self._request(acts, seals, state, brief))
        refused = {x["n"] for x in r["refused"]}
        self.record = [a for k, a in enumerate(acts, 1) if k not in refused]
        self.seals = list(r["seals"])
        self.id, self.generation = r["session"], self.host.generation
        self._took(r)
        return r

    def _took(self, r: dict) -> None:
        self.head = {"size": r["head"]["size"], "root": r["head"]["root"]}
        self.digest, self.community = r["digest"], r["head"].get("community", self.community)

    def reopen(self, parties: list[dict] | None = None, state: bool = False, brief: list | None = None) -> dict:
        """Opens the session again from its record (with the parties given, if any). With the same parties the record must replay as it
        was sealed; with others, it is folded again as the full fold would fold it, and what it admits is the record."""
        same = parties is None or [dict(p) for p in parties] == self.parties
        if parties is not None:
            self.parties = [dict(p) for p in parties]
        r = self.host.ask("open", **self._request(self.record, self.seals if same else None, state, brief))
        if same and (r["refused"] or r.get("replays") is False):
            raise EngineError(f"the session's record does not replay as it was sealed (it differs at {r.get('differs_at')}; {len(r['refused'])} acts refused)")
        refused = {x["n"] for x in r["refused"]}
        self.record = [a for k, a in enumerate(self.record, 1) if k not in refused]
        self.seals = list(r["seals"])
        self.id, self.generation = r["session"], self.host.generation
        self._took(r)
        return r

    def ask(self, op: str, **request) -> dict:
        """An operation on this session, opening it again once if the engine lost it."""
        for attempt in (0, 1):
            if self.generation != self.host.generation or not self.host.alive():
                self.reopen()
            try:
                return self.host.ask(op, session=self.id, **request)
            except Lost:
                if attempt:
                    raise
            except EngineError as error:
                if attempt or "no session" not in str(error):
                    raise
                self.generation = -1
        raise Lost("the session could not be asked")

    def step(self, acts: list[dict], brief: list | None = None, state: bool = False) -> dict:
        """Puts acts to the community's rules on the session's state; answers each act's outcome (and, for an act carrying "conduct",
        the episode's entry in the journal), the seals, the new heads, the journal's new entries, and the state and briefings if asked.
        An item {"observe": episode} records conduct that constitutes no act; {"disclose": epistemic} an epistemic episode by agreement."""
        request: dict = {"head": dict(self.head), "acts": acts}
        if brief:
            request["brief"] = brief
        if state:
            request["state"] = True
        r = self.ask("step", **request)
        if r.get("stale"):
            raise Stale(r["reason"])
        for item, o in zip(acts, r["outcomes"]):
            if o.get("admitted"):
                a = {k: v for k, v in item.items() if k != "conduct"}
                if o.get("episode"):
                    a["fields"] = dict(a.get("fields") or {}, episode=o["episode"])
                self.record.append(a)
        self.seals += r["seals"]
        self.journal += r["journal"]["entries"]
        self._took(r)
        return r

    def view(self, what: str = "state", party: str | None = None, **request) -> dict:
        if party:
            request["as"] = party
        return self.ask("view", what=what, **request)

    def state(self) -> dict:
        return self.view("state")

    def profile(self, reader: str | None = None) -> dict:
        """The profile for a reader of the community, or the public profile."""
        return self.view("profile", reader)

    def briefing(self, party: str, mandate: str = "", purpose: str = "") -> dict:
        return self.view("briefing", party, mandate=mandate, purpose=purpose)

    def items(self, party: str | None = None) -> dict:
        return self.view("items", party)

    def work(self, party: str | None = None, mandate: str = "") -> dict:
        return self.view("work", party, mandate=mandate)

    def mandate(self, party: str | None = None, mandate: str = "") -> dict:
        return self.view("mandate", party, mandate=mandate)

    def behaviour(self) -> dict:
        return self.view("behaviour")

    def exit(self, party: str) -> dict:
        return self.view("exit", party)

    def heads(self, since: int | None = None) -> dict:
        """The chronicle's head and the journal's, with a proof of consistency of the chronicle from an earlier size when asked."""
        return self.view("head", **({"from": since} if since is not None else {}))

    def issue_passport(self, grant: dict, home: str | None = None, at: str = "", turn: int | None = None, model: str = "", label: str = "",
                       node: str = "", status_at: str = "") -> dict:
        """At a home: mints a bearer credential for the grant's holder, steps the act by which the grant's principal issues the mandate
        with the credential's digest, and assembles its passport. Answers {passport, credential, head}, the credential shown once."""
        token, digest = credential()
        issued = dict(grant, credential=digest)
        last = self.record[-1].get("turn", 0) if self.record else 0
        r = self.step([{"author": issued["principal"], "kind": "mandate", "turn": turn if turn is not None else last, "grant": issued}])
        if not r["outcomes"][0]["admitted"]:
            raise ValueError(f"no passport for {issued['id']}: {r['outcomes'][0]['reason']}")
        p = self.ask("passport.issue", mandate=issued["id"], at=at, model=model, label=label, node=node, status_at=status_at, **({"home": home} if home else {}))
        if not p["admitted"]:
            raise ValueError(f"no passport for {issued['id']}: {p['reason']}")
        return {"passport": p["passport"], "credential": token, "head": p["head"]}

    def close(self) -> None:
        try:
            self.host.ask("close", session=self.id)
        except EngineError:
            pass


def _on(source, op: str, **request) -> dict:
    """An operation on a session, or on a record ({constitution, parties, acts}) asked of a process of its own."""
    if isinstance(source, Session):
        return source.ask(op, **request)
    return ask(op, **dict(source), **request)


# ---- The archive of conduct and of acts (the design's Section 4) -----------------------------------------------------------------

def join(source, journal: list[dict] | None = None) -> dict:
    """Whether a chronicle and a behaviour journal are joined act to episode in both directions (thesmos's Proposition 15), the verdict
    failing at the place the join breaks; and whether epistemic behaviour is kept only by agreement. A session's own journal unless one is
    given; a record ({constitution, parties, acts}) names its journal."""
    return _on(source, "join.verify", **({"journal": journal} if journal is not None else {}))


def verify_exit(package: dict, chronicle: dict | None = None, journal: dict | None = None) -> dict:
    """Whether every act and every entry of an exit package is under the heads known ({size, root} of the chronicle and of the journal),
    each entry sealing from the seal before it."""
    request: dict = {"package": package}
    if chronicle:
        request["chronicle"] = {k: chronicle[k] for k in ("size", "root", "community") if k in chronicle}
    if journal:
        request["journal"] = {"size": journal["size"], "root": journal["root"]}
    return ask("exit.verify", **request)


def exit_package(session: Session, party: str, folder: Path | str | None = None, date: str = "") -> dict:
    """A party's exit package (the design's Definition 50): the acts that concern it with their proofs under the chronicle's head, the
    entries of the behaviour journal that are its own or constituted those acts with their proofs under the journal's head, and its
    positions, mandates and commitments; verified against both heads as the session gives them. With a folder, the package is written
    there and packed as a BagIt bag by science.archive (archeion), and the bag validated."""
    package = session.exit(party)
    heads = session.heads()
    verified = verify_exit(package, chronicle=heads["head"], journal=heads["journal"])
    out = {"package": package, "heads": heads, "verified": verified}
    if folder is None:
        return out
    import datetime as _dt

    from . import archive
    run = Path(folder)
    data = run / "data"
    data.mkdir(parents=True, exist_ok=True)
    parts = {"exit.json": package, "heads.json": {"chronicle": heads["head"], "journal": heads["journal"]}}
    for name, value in parts.items():
        (data / name).write_text(json.dumps(value, ensure_ascii=False, indent=1), encoding="utf-8", newline="\n")
    payload = {name: archive.path(data / name) for name in parts}
    info = [["Source-Organization", f"the community {package['community']}"], ["External-Description", f"the exit package of {party}"],
            ["External-Identifier", f"{package['community']}/{party}"]]
    bag = archive.ask("package", payload=payload, info=info, date=date or _dt.date.today().isoformat())
    for path, text in bag["tags"].items():
        (run / path).write_text(text, encoding="utf-8", newline="\n")
    files = {p.relative_to(run).as_posix(): archive.path(p) for p in run.rglob("*") if p.is_file()}
    out["bag"] = archive.ask("validate", files=files)
    out["folder"] = str(run)
    return out


PROV = {"prov": "http://www.w3.org/ns/prov#", "noetarium": "https://noetarium.org/ns/communities#"}
PROV_TYPE = {"person": "prov:Person", "organisation": "prov:Organization", "community": "prov:Organization", "operative": "prov:SoftwareAgent"}


def provenance(session: Session) -> dict:
    """The two records as W3C PROV-JSON (PROV-DM; the design's Section 4.2): parties as agents (persons, organisations and communities,
    operatives as software agents), control and the mandate chain as delegation, each episode of conduct an activity that used and
    generated what its digests name, each admitted act an activity of the community informed by the episode that constituted it,
    holdings as entities generated by the acts that deposited them and attributed to their depositors."""
    m = session.view("provenance")
    ns = "noetarium:"
    doc = {"prefix": dict(PROV), "agent": {}, "activity": {}, "entity": {}, "wasAssociatedWith": {}, "actedOnBehalfOf": {}, "used": {},
           "wasGeneratedBy": {}, "wasInformedBy": {}, "wasAttributedTo": {}}
    doc["agent"][f"{ns}community/{m['community']}"] = {"prov:type": "prov:Organization", "prov:label": m["community"]}
    for a in m["agents"]:
        doc["agent"][f"{ns}party/{a['id']}"] = {"prov:type": PROV_TYPE.get(a["kind"], "prov:Agent"), "prov:label": a["id"], f"{ns}kind": a["kind"],
                                                **({f"{ns}provider": a["provider"]} if a.get("provider") else {})}
        if a.get("controller"):
            doc["actedOnBehalfOf"][f"_:control/{a['id']}"] = {"prov:delegate": f"{ns}party/{a['id']}", "prov:responsible": f"{ns}party/{a['controller']}",
                                                              f"{ns}by": "control"}
    for d in m["delegations"]:
        doc["actedOnBehalfOf"][f"_:mandate/{d['mandate']}"] = {"prov:delegate": f"{ns}party/{d['holder']}", "prov:responsible": f"{ns}party/{d['principal']}",
                                                               f"{ns}mandate": d["mandate"], **({f"{ns}parent": d["parent"]} if d.get("parent") else {})}
    for e in m["episodes"]:
        key = e["entry"][:16]
        act = f"{ns}episode/{key}"
        doc["activity"][act] = {"prov:type": f"{ns}conduct", f"{ns}operation": e.get("operation", ""), f"{ns}outcome": e["outcome"], f"{ns}entry": e["entry"],
                                **({"prov:startTime": e["begun"]} if e.get("begun") else {}), **({"prov:endTime": e["ended"]} if e.get("ended") else {})}
        doc["wasAssociatedWith"][f"_:by/{key}"] = {"prov:activity": act, "prov:agent": f"{ns}party/{e['party']}"}
        for d in e.get("inputs", []):
            doc["entity"].setdefault(f"{ns}digest/{d}", {"prov:type": f"{ns}artefact"})
            doc["used"][f"_:used/{key}/{d[:16]}"] = {"prov:activity": act, "prov:entity": f"{ns}digest/{d}"}
        for d in e.get("outputs", []):
            doc["entity"].setdefault(f"{ns}digest/{d}", {"prov:type": f"{ns}artefact"})
            doc["wasGeneratedBy"][f"_:made/{key}/{d[:16]}"] = {"prov:entity": f"{ns}digest/{d}", "prov:activity": act}
            doc["wasAttributedTo"][f"_:of/{key}/{d[:16]}"] = {"prov:entity": f"{ns}digest/{d}", "prov:agent": f"{ns}party/{e['party']}"}
    deposits = {}
    for a in m["acts"]:
        act = f"{ns}act/{a['place']}"
        doc["activity"][act] = {"prov:type": f"{ns}admission", f"{ns}kind": a["kind"], f"{ns}reading": a["reading"], f"{ns}seal": a["seal"], f"{ns}turn": a["turn"]}
        doc["wasAssociatedWith"][f"_:admitted/{a['place']}"] = {"prov:activity": act, "prov:agent": f"{ns}community/{m['community']}"}
        doc["wasAssociatedWith"][f"_:asked/{a['place']}"] = {"prov:activity": act, "prov:agent": f"{ns}party/{a['author']}",
                                                              **({f"{ns}mandate": a["mandate"]} if a.get("mandate") else {})}
        if a.get("episode"):
            doc["wasInformedBy"][f"_:constituted/{a['place']}"] = {"prov:informed": act, "prov:informant": f"{ns}episode/{a['episode'][:16]}"}
        if a["reading"] in ("deposit", "modify") and a.get("holding"):
            deposits[a["holding"]] = act
    for h in m["holdings"]:
        ent = f"{ns}holding/{h['id']}"
        doc["entity"][ent] = {"prov:type": f"{ns}holding", f"{ns}kind": h["kind"], **({f"{ns}digest": h["digest"]} if h.get("digest") else {}),
                              **({f"{ns}within": h["within"]} if h.get("within") else {})}
        if h["id"] in deposits:
            doc["wasGeneratedBy"][f"_:deposited/{h['id']}"] = {"prov:entity": ent, "prov:activity": deposits[h["id"]]}
        if h.get("depositor"):
            doc["wasAttributedTo"][f"_:depositor/{h['id']}"] = {"prov:entity": ent, "prov:agent": f"{ns}party/{h['depositor']}"}
    return {k: v for k, v in doc.items() if v or k == "prefix"}


def _read(op: str, source, **request) -> dict:
    """An operation of the engine of readings on a record ({constitution, parties, acts}, a journal and seals if it has them), or on a
    session's record, seals and journal, which replay as they were sealed."""
    if isinstance(source, Session):
        record = {"constitution": source.constitution, "parties": source.parties, "acts": source.record, "seals": source.seals}
        if source.journal:
            record["journal"] = source.journal
    else:
        record = dict(source)
    if not READINGS.status().get("current"):
        READINGS.build(log=lambda *_: None)
    return READINGS.ask(f"communities.{op}", **{**record, **request})


def power(source, mechanism: str, **parameters) -> dict:
    """A measure of power (the design's Section 4.6) as a declared mechanism of the communities domain's trajectories (tenure by office,
    acts of force by origin, authorship of amendments, rulings by origin, petitions by origin, voting power, long offices), with its
    digest and the head it read. Parameters are written with spaces or underscores ("position", "bound", "force", "decision")."""
    return _read("power", source, mechanism={"name": mechanism, "parameters": {k.replace("_", " "): v for k, v in parameters.items()}})


def reading(source, mechanism: str, journal: list[dict] | None = None, seals: list[str] | None = None, **parameters) -> dict:
    """A value reading (the design's Definition 33): a declared mechanism of value formation (share of decisions, practised norm, conduct
    by category) applied to the chronicle and the behaviour journal through their join, acts decided under a reading (their field "under
    reading") left out with the episodes that constituted them (H7), naming its mechanism's digest, both heads, the join's verdict, its
    result, grade B and its own digest. On a record, the seals given are checked as it replays."""
    request: dict = {"mechanism": {"name": mechanism, "parameters": parameters}}
    if journal is not None:
        request["journal"] = journal
    if seals is not None:
        request["seals"] = seals
    return _read("reading", source, **request)


def verify_reading(value: dict, source, journal: list[dict] | None = None, seals: list[str] | None = None) -> dict:
    """A second reader's reading of the same mechanism on the records it holds (Proposition 15): whether it is the reading given, digest
    for digest; where its records differ, the act at which its chronicle no longer replays as the seals given say, or the journal's
    verification naming its entry; else whether the mechanism differs, or which head."""
    m = value["mechanism"]
    again = reading(source, m["name"], journal=journal, seals=seals, **m["parameters"])
    out = {"same": again["digest"] == value["digest"], "reading": again}
    if not out["same"]:
        if again.get("replays") is False:
            out["differs at"] = {"record": "chronicle", "act": again.get("differs_at")}
        elif again["join"] != value["join"] and value["join"] == "joined":
            out["differs at"] = {"record": "journal", "reason": again["join"]}
        elif again["mechanism digest"] != value["mechanism digest"]:
            out["differs at"] = {"record": "mechanism"}
        elif again["chronicle"] != value["chronicle"]:
            out["differs at"] = {"record": "chronicle", "head": again["chronicle"]}
        else:
            out["differs at"] = {"record": "journal", "head": again["journal"]}
    return out


# ---- Registries (the communities domain's registries) -------------------------------------------------------------------------------

def profile_document(source, community: str | None = None) -> dict:
    """The public profile as the document noetarium.profile/1 a listing names, exactly as a node serves it, and its SHA-256."""
    return _on(source, "registry", **{"do": "profile", **({"community": community} if community else {})})


def listing(source, node: str, witnessed_by: str, listed: str, languages: list[str] | None = None, community: str | None = None) -> dict:
    """A community's listing (noetarium.listing/1) as its record gives it: {listing, written, digest}."""
    return _on(source, "registry", **{"do": "listing", "node": node, "witnessed by": witnessed_by, "listed": listed, "languages": languages or ["en"],
                                       **({"community": community} if community else {})})


def registry(do: str, **request) -> dict:
    """The registries' operations that read no record: "read" {listing}, "verify" {listing, evidence: {profile, now, consistency,
    witnesses, notaries, registry}}, "search" {registries: [{keeper, holding, listings: [{listing, evidence}]}], query}."""
    return ask("registry", do=do, **request)


def registry_entries(source, holding: str) -> dict:
    """The entries of a registry kept as a holding of the keeper's record, with what is wrong with its record."""
    return _on(source, "registry", do="entries", registry=holding)
