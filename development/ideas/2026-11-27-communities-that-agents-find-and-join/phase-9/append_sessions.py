

# ---- Sessions (the design's Section 9.7) -------------------------------------------------------------------------------------------

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
    "episode"), their seals and the behaviour journal's entries, so that it can open itself again, from its record, if the engine's process
    is lost; a step names the head the session stood at, and is refused as stale (Stale) when another holder of the same session moved it
    first. The outcomes of its steps are those a full fold of the record and the new acts gives."""

    def __init__(self, constitution: dict | str, parties: list[dict] | None = None, acts: list[dict] | None = None, seals: list[str] | None = None,
                 journal: list[dict] | None = None, host_: Host | None = None):
        self.constitution = constitution
        self.parties = [dict(p) for p in (parties or [])]
        self.host = host_ or host()
        self.record: list[dict] = []
        self.seals: list[str] = []
        self.journal: list[dict] = [dict(e) for e in (journal or [])]
        self.id, self.generation, self.head, self.digest, self.community = "", -1, {}, "", ""
        self.opened = self._open([dict(a) for a in (acts or [])], seals)

    def _open(self, acts: list[dict], seals: list[str] | None = None) -> dict:
        request = {"constitution": self.constitution, "parties": self.parties, "acts": acts}
        if seals is not None:
            request["seals"] = seals
        if self.journal:
            request["journal"] = self.journal
        r = self.host.ask("open", **request)
        refused = {x["n"] for x in r["refused"]}
        self.record = [a for k, a in enumerate(acts, 1) if k not in refused]
        self.seals = list(r["seals"])
        self.id, self.generation = r["session"], self.host.generation
        self._took(r)
        return r

    def _took(self, r: dict) -> None:
        self.head = {"size": r["head"]["size"], "root": r["head"]["root"]}
        self.digest, self.community = r["digest"], r["head"].get("community", self.community)

    def reopen(self) -> dict:
        """Opens the session again from its record, which must replay as it was sealed."""
        request = {"constitution": self.constitution, "parties": self.parties, "acts": self.record, "seals": self.seals}
        if self.journal:
            request["journal"] = self.journal
        r = self.host.ask("open", **request)
        if r["refused"] or r.get("replays") is False:
            raise EngineError(f"the session's record does not replay as it was sealed (it differs at {r.get('differs_at')}; {len(r['refused'])} acts refused)")
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
        the episode's entry in the journal), the seals, the new head, the journal's new entries, and the state and briefings if asked. An
        item {"observe": episode} records conduct that constitutes no act; {"disclose": epistemic} an epistemic episode by agreement."""
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
        """The chronicle's head and the journal's, with a proof of consistency from an earlier size when asked."""
        return self.view("head", **({"from": since} if since is not None else {}))

    def issue_passport(self, grant: dict, home: str | None = None, at: str = "", turn: int | None = None, model: str = "", label: str = "", node: str = "",
                       status_at: str = "") -> dict:
        """At a home: mints a bearer credential for the grant's holder, steps the act by which the grant's principal issues the mandate
        with the credential's digest, and assembles its passport. Answers {passport, credential, head}, the credential shown once."""
        token, digest = credential()
        issued = dict(grant, credential=digest)
        r = self.step([{"author": issued["principal"], "kind": "mandate", "turn": turn if turn is not None else (self.record[-1].get("turn", 0) if self.record else 0),
                        "grant": issued}])
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


def _chronicles(chronicles: dict, op: str, **request) -> dict:
    """An operation over several communities' chronicles, each a session or a record; the sessions must share one host."""
    sessions = [c for c in chronicles.values() if isinstance(c, Session)]
    if len({id(s.host) for s in sessions}) > 1:
        raise ValueError("the sessions read together are kept by one host")
    for attempt in (0, 1):
        for s in sessions:
            if s.generation != s.host.generation or not s.host.alive():
                s.reopen()
        named = {name: ({"session": c.id} if isinstance(c, Session) else dict(c)) for name, c in chronicles.items()}
        try:
            return sessions[0].host.ask(op, chronicles=named, **request) if sessions else ask(op, chronicles=named, **request)
        except EngineError as error:
            if attempt or not sessions or (not isinstance(error, Lost) and "no session" not in str(error)):
                raise
            for s in sessions:
                s.generation = -1
    raise Lost("the chronicles could not be read")


# ---- The archive of conduct and of acts (the design's Section 4) -----------------------------------------------------------------

def join(source, journal: list[dict] | None = None) -> dict:
    """Whether a chronicle and a behaviour journal are joined act to episode in both directions (thesmos's Proposition 15), the verdict
    failing at the place the join breaks; and whether epistemic behaviour is kept only by agreement. A session's own journal unless one is
    given."""
    return _on(source, "join.verify", **({"journal": journal} if journal is not None else {}))


def verify_exit(package: dict, chronicle: dict | None = None, journal: dict | None = None) -> dict:
    """Whether every act and every entry of an exit package is under the heads known ({size, root} of the chronicle and of the journal)."""
    request: dict = {"package": package}
    if chronicle:
        request["chronicle"] = chronicle
    if journal:
        request["journal"] = journal
    return ask("exit.verify", **request)


def exit_package(session: Session, party: str, folder: Path | str | None = None, date: str = "") -> dict:
    """A party's exit package (the design's Definition 50): the acts that concern it with their proofs under the chronicle's head, the
    entries of the behaviour journal that are its own or constituted those acts with their proofs under the journal's head, and its
    positions, mandates and commitments; verified against both heads as the session gives them. With a folder, the package is written
    there and packed as a BagIt bag by science.archive (archeion), and the bag validated."""
    package = session.exit(party)
    heads = session.heads()
    verified = verify_exit(package, chronicle=heads["head"], journal={"size": heads["journal"]["size"], "root": heads["journal"]["root"]})
    out = {"package": package, "heads": heads, "verified": verified}
    if folder is None:
        return out
    import datetime as _dt

    from . import archive
    run = Path(folder)
    data = run / "data"
    data.mkdir(parents=True, exist_ok=True)
    parts = {"exit.json": package, "chronicle.json": package["chronicle"], "journal.json": package["journal"],
             "heads.json": {"chronicle": heads["head"], "journal": heads["journal"]}}
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


def _material(source) -> tuple[dict, Session | None]:
    if isinstance(source, Session):
        return source.view("provenance"), None
    s = Session(source["constitution"], source.get("parties", []), source.get("acts", []), journal=source.get("journal"))
    return s.view("provenance"), s


def provenance(source) -> dict:
    """The two records as W3C PROV-JSON (PROV-DM; the design's Section 4.2): parties as agents (persons, organisations and communities,
    operatives as software agents), control and the mandate chain as delegation, each episode of conduct an activity that used and
    generated what its digests name, each admitted act an activity of the community informed by the episode that constituted it,
    holdings as entities generated by the acts that deposited them and attributed to their depositors."""
    m, temporary = _material(source)
    try:
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
    finally:
        if temporary is not None:
            temporary.close()


def graph(source) -> dict:
    """The graph of provenance archeion reads (entities by digest, activities with their agent, kind, instant, what they used and
    generated): each episode of conduct an activity whose kind is its declared role (its feature "role") or else its operation; what an
    episode generated includes the digest deposited by the act it constituted."""
    m, temporary = _material(source)
    try:
        acts = {a["episode"]: a for a in m["acts"] if a.get("episode")}
        entities, activities = set(), []
        for e in m["episodes"]:
            generated = list(e.get("outputs", []))
            a = acts.get(e["entry"])
            if a and a.get("digest") and a["digest"] not in generated:
                generated.append(a["digest"])
            entities.update(list(e.get("inputs", [])) + generated)
            activities.append({"id": e["entry"], "kind": (e.get("features") or {}).get("role") or e.get("operation", ""), "agent": e["party"],
                               "when": e.get("begun", ""), "used": list(e.get("inputs", [])), "generated": generated})
        return {"entities": sorted(entities), "activities": activities}
    finally:
        if temporary is not None:
            temporary.close()


def contribution(source, artefact: str, weights: dict | None = None, faulty: list[str] | None = None, graph_: dict | None = None) -> dict:
    """Each party's contribution to an artefact over the graph of provenance (the design's Definition 32; the communities domain's
    trajectories): the activities beneath it by role, attributed to the party that answers for their agents, each party's exact share
    under declared weights of the roles, what faulty inputs taint, and the faults of the graph (archeion)."""
    g = graph_ if graph_ is not None else graph(source)
    request = {"do": "contribution", "graph": g, "artefact": artefact, "weights": weights or {}, "faulty": faulty or []}
    if isinstance(source, Session):
        return source.ask("trajectories", **request)
    return ask("trajectories", parties=source.get("parties", []), **request)


def recognise(source, fact: str, subject: str, standard: dict | None = None, traces: list[str] | None = None, journal: list[dict] | None = None) -> dict:
    """A fact about conduct as the record recognises it (the design's Definition 29): its standing under the standard in force (or the
    one given, {grade, another origin}), what it emerged from, since when it stands and what rests on it, reports of one origin counted
    once; a session's journal, or the one given, holds the traces its observations may cite."""
    request = {"do": "recognise", "fact": fact, "subject": subject, "traces": traces or []}
    if standard:
        request["standard"] = standard
    if journal is not None:
        request["journal"] = journal
    return _on(source, "trajectories", **request)


def weigh(source, fact: str, subject: str, value: str, channels: dict, relay: dict | None = None) -> dict:
    """The weight of the observations of a fact for a value, each origin's reports counted once through the channel its best grade
    declares ({grade: {hit, false_alarm}}): the likelihood ratio, what counting each report apart would give, and the overstatement."""
    return _on(source, "trajectories", **{"do": "weigh", "fact": fact, "subject": subject, "value": value, "channels": channels, **({"relay": relay} if relay else {})})


def tenure(source) -> dict:
    """Every interval during which a party held a position, read by replaying the record from its founding."""
    return _on(source, "trajectories", do="tenure")


def power(source, mechanism: str, **parameters) -> dict:
    """A measure of power (the design's Section 4.6) as a declared mechanism (tenure by office, acts of force by origin, authorship of
    amendments, rulings by origin, petitions by origin, voting power, long offices), with its digest and the head it read."""
    return _on(source, "power", mechanism={"name": mechanism, "parameters": {k.replace("_", " "): v for k, v in parameters.items()}})


def _threshold(text: str, total: int) -> int:
    """The least whole weight that meets a threshold ("at least p/q", "more than p/q", or a bare p/q) of a total weight."""
    strict = text.startswith("more than ")
    share = Fraction(text.removeprefix("at least ").removeprefix("more than ").strip())
    bound = share * total
    return math.floor(bound) + 1 if strict else math.ceil(bound)


def game(decision: dict, members: list[str]) -> dict:
    """A decision's simple game as science.multilateral states a rule: whole weights (declared weights scaled by their common
    denominator, one each otherwise) and the quota its rule sets, every member casting a ballot; a double majority as both games at once."""
    if decision.get("count by") == "weight":
        ws = [Fraction(str(decision.get("weights", {}).get(m, "0"))) for m in members]
        scale = math.lcm(*[w.denominator for w in ws]) if ws else 1
        weights = [int(w * scale) for w in ws]
    else:
        weights = [1] * len(members)
    total, rule = sum(weights), decision["rule"]
    if rule == "any one":
        return {"weights": weights, "quota": 1}
    if rule == "majority":
        return {"weights": weights, "quota": total // 2 + 1}
    if rule == "two thirds":
        return {"weights": weights, "quota": math.ceil(Fraction(2, 3) * total)}
    if rule == "unanimity" or rule.startswith("consensus"):
        return {"weights": weights, "quota": total}
    if rule.startswith("at least ") or rule.startswith("more than "):
        return {"weights": weights, "quota": _threshold(rule, total)}
    if rule.startswith("double majority of members "):
        members_part, weight_part = rule.removeprefix("double majority of members ").split(" and weight ")
        ones = [1 if w > 0 else 0 for w in weights]
        return {"all_of": [{"weights": ones, "quota": _threshold(members_part, sum(ones))}, {"weights": weights, "quota": _threshold(weight_part, total)}]}
    raise ValueError(f"no simple game for the rule {rule!r}")


def voting_power(source, decision: str) -> dict:
    """A decision's a priori voting power through science.multilateral (nomos's games in the diplomacy domain's composition): the members
    and the decision as the record leaves them (its rule, unit of count and weights), the decision's simple game, and each member's
    Banzhaf and Shapley–Shubik index, beside the measure the communities engine reads from thesmos's own tally."""
    from . import multilateral
    measured = power(source, "voting power", decision=decision)
    if measured["refused"]:
        raise ValueError(measured["refused"][0])
    members = measured["members"]
    rule = game(measured["decision"], members)
    answer = multilateral.ask("rule", members=members, rule=rule)
    by = {p["member"]: p for p in answer["power"]}
    return {"decision": measured["decision"], "members": members, "game": rule,
            "banzhaf": {m: by[m]["banzhaf"] for m in members}, "shapley_shubik": {m: by[m]["shapley_shubik"] for m in members},
            "measured": {"banzhaf": measured["banzhaf"], "shapley_shubik": measured["shapley_shubik"]}}


def reading(source, mechanism: str, journal: list[dict] | None = None, seals: list[str] | None = None, **parameters) -> dict:
    """A value reading (the design's Definition 33): a declared mechanism of value formation (practised norm, readings of open terms,
    share of decisions, conduct by category) applied to the chronicle and the behaviour journal through their join, acts decided under a
    reading (their field "under reading") left out (H7), naming its mechanism's digest, both heads, its result, grade B and its own digest.
    On a record, the seals given are checked as it replays."""
    request: dict = {"mechanism": {"name": mechanism, "parameters": parameters}}
    if journal is not None:
        request["journal"] = journal
    if seals is not None:
        request["seals"] = seals
    return _on(source, "reading", **request)


def verify_reading(value: dict, source, journal: list[dict] | None = None, seals: list[str] | None = None) -> dict:
    """A second reader's reading of the same mechanism on the records it holds (Proposition 15): whether it is the reading given, digest
    for digest; where its records differ, the act at which its chronicle no longer replays as the seals given say, or the journal's
    verification; else whether the mechanism differs."""
    m = value["mechanism"]
    again = reading(source, m["name"], journal=journal, seals=seals, **m["parameters"])
    out = {"same": again["digest"] == value["digest"], "reading": again}
    if not out["same"]:
        if again.get("replays") is False:
            out["differs at"] = {"record": "chronicle", "act": again.get("differs_at")}
        elif again["join"] not in ("", "joined") and value["join"] == "joined":
            out["differs at"] = {"record": "journal", "reason": again["join"]}
        elif again["mechanism digest"] != value["mechanism digest"]:
            out["differs at"] = {"record": "mechanism"}
        elif again["chronicle"] != value["chronicle"]:
            out["differs at"] = {"record": "chronicle", "head": again["chronicle"]}
        else:
            out["differs at"] = {"record": "journal", "head": again["journal"]}
    return out


# ---- Compacts and custom across communities, through the law domain (science.law) --------------------------------------------------

def _turn(n: int) -> str:
    """A turn written as a text that sorts as the turns do, for the law domain's dates."""
    return f"{n:012d}"


def compact(chronicles: dict, compact_id: str, turn: int | None = None, parties: list[str] | None = None) -> dict:
    """A compact's standing read from each party's chronicle (thesmos's compacts, the communities domain's federation): its terms,
    entry, who is bound, every ratification with its reservations, and every article binding each pair."""
    request: dict = {"compact": compact_id}
    if turn is not None:
        request["turn"] = turn
    if parties:
        request["parties"] = parties
    return _chronicles(chronicles, "compact", **request)


def bindings(chronicles: dict, compact_id: str, turn: int | None = None, parties: list[str] | None = None) -> dict:
    """Whom a compact binds, article by article, through the law domain's sources (science.law): the compact's standing at the turn read
    from each party's chronicle becomes a treaty in the law's terms (its parties those whose ratification stands, in force from its entry
    while at least two stand and its last turn has not passed), the law says whom it binds and why (pacta tertiis), and an article binds two
    parties when the law binds both and neither reserves it. Beside it, whether the communities domain's own reading agrees."""
    from . import law
    c = compact(chronicles, compact_id, turn, parties)
    t = c["turn"]
    if c["in force"]:
        span = {"from": _turn(c["entered"]), "until": _turn(c["terms"]["last turn"]) if c["terms"]["last turn"] is not None else ""}
    elif c["entered"] is not None and c["entered"] <= t:
        span = {"from": _turn(c["entered"]), "until": _turn(t - 1)}  # it entered, and stands no longer
    else:
        span = {"from": _turn(t + 1), "until": ""}  # not yet in force
    treaty = {"id": compact_id, "title": compact_id, "kind": "treaty", "parties": c["ratified"], **span}
    told = law.bindings(treaty, c["parties"], _turn(t))
    bound = {b["state"]: b["holds"] for b in told["binds"]}
    reserved = {}
    for p, rs in c["ratifications"].items():
        standing = [r for r in rs if r["stands"]]
        reserved[p] = set(standing[-1]["reserved"]) if standing else set()
    articles = []
    for a in c["terms"]["articles"]:
        for i, p in enumerate(c["parties"]):
            for q in c["parties"][i + 1:]:
                if bound.get(p) and bound.get(q) and a["id"] not in reserved.get(p, set()) and a["id"] not in reserved.get(q, set()):
                    articles.append({"article": a["id"], "between": [p, q]})
    own = {(b["article"], tuple(b["between"])) for b in c["binds"]}
    return {"compact": compact_id, "turn": t, "treaty": treaty, "law": told, "binds": articles, "standing": c,
            "agrees": own == {(b["article"], tuple(b["between"])) for b in articles}}


def custom(chronicles: dict, statement: str, formed: int, scheme: dict | None = None, concerned: list[str] | None = None) -> dict:
    """Whether a custom exists among communities, through the law domain's sources (science.law): each community's practice read from
    its chronicle (every episode the norm's statement applies to, conforming or not), its acceptance as law from the turn an amendment of
    the community adopts the norm, and its persistent objection by claims challenging the norm (thesmos's facets of a norm), identified
    under a declared scheme by ``formed``, the turn by which the rule is taken to have formed; and whom it binds."""
    from . import law
    pr = _chronicles(chronicles, "practice", statement=statement)
    practice = []
    for c, x in pr["chronicles"].items():
        adopted = min(x["adopted"]) if x["adopted"] else None
        for i in x["instances"]:
            practice.append({"state": c, "date": _turn(i["turn"]), "conforming": i["conforming"], "accepted_as_law": adopted is not None and adopted <= i["turn"],
                             "objection": False})
        for t in x["objections"]:
            practice.append({"state": c, "date": _turn(t), "conforming": False, "accepted_as_law": False, "objection": True})
    found = law.custom(practice, concerned or sorted(pr["chronicles"]), _turn(formed), scheme)
    return {"norm": pr["norm"], "practice": practice, **found}


# ---- Registries, organisations (the communities domain) -----------------------------------------------------------------------------

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


def organisation(source, **request) -> dict:
    """A treaty organisation's accounts read from its record (the communities domain's organisations): calls, payments, each member's
    account at a turn, core against earmarked, staff by origin, withdrawals; with a period, the calls against the scale."""
    return _on(source, "organisation", **request)
