"""Agents that act on their own initiative within what they were lent for (ADR 0169).

An agent is an operative: a party controlled by the person or organisation that lent it, acting only under a mandate that names
the kinds of act, the matters, the ceilings in each resource and the term. It is not asked; it wakes. Each round it wakes when its
tempo has run, or sooner when something it can act on appeared in a community it belongs to, and it decides from the community's
state what to do; then the community's own rules admit or refuse what it did. Idle costs nothing: an agent with nothing to act on
does not ask its model.

Behaviour has two origins here (nomos's decision 0016 names three): **rule**, a declared procedure; and **model**, a language model
asked through ``science.models`` within a spending guard (``budget.Ledger``). A model agent asks its model for the work itself (a
check, an adjudication) and, every few rounds or after something unusual (its delivery released, its work disputed), for its own
initiative: which of the acts open to it to take, and a note to its community if it has one worth saying. The model never decides
what the community admits; its words are proposals of grade C, kept as the bodies of its acts.

Roles: ``checker`` (a commons' contributor), ``keeper`` (a steward's operative, accepting by the steward's declared standard),
``assessor`` (a reviewer's operative), ``adjudicator`` (a guild's aide, or an applicant's agent on trial).
"""

from __future__ import annotations

import json

from . import missions
from .budget import Exhausted
from .society import Society, item_of

KINDS = {
    "checker": ["join", "offer", "cancel", "deliver", "hand over", "report", "say", "claim", "leave"],
    "keeper": ["take", "accept", "release", "request", "answer", "say"],
    "assessor": ["assess", "convert", "say"],
    "adjudicator": ["join", "offer", "cancel", "deliver", "hand over", "report", "say"],
    "clerk": ["commit", "deliver", "accept", "release", "assign", "report", "say"],
    "envoy": ["join", "propose", "vote", "amend", "say", "report"],
}
WORKING = ("contributor", "hand", "aide")  # where a checker offers work: a commons' contributor, a cooperative's hand, a guild's aide
OPEN_AT_ONCE = 4  # batches a keeper keeps posted and unsettled
MODELS = ("claude-opus-5-5", "claude-sonnet-5-5", "claude-haiku-5-5")  # the models a sponsor may choose for its agent

CHECK_SYSTEM = (
    "You are an agent lent to a commons whose mission is to check entries of a catalogue of urban indexes that a language model drafted "
    "and nobody checked. For each entry say whether what it declares holds, is at fault, or is in doubt. Judge chiefly its aggregation, "
    "how the index is carried up a hierarchy of units (sum; wmean:<weight>, a mean weighted by the named quantity; ratio-of-sums:<num>/<den>; "
    "median; recompute, computed again at each level; none, kept at one level; max; min), against its kind, unit and definition, and "
    "whether the definition fits the name and the unit. Give each entry a reason of at most 25 words. Say doubt only when the entry's own "
    "text does not let you decide.")
ADJUDICATE_SYSTEM = (
    "You are an aide of a guild whose method is to settle a disagreement between two checks of a catalogue entry of an urban index. Read "
    "the entry and the two verdicts with their reasons, and give the verdict the entry's own text supports: holds, fault or doubt, with a "
    "reason of at most 30 words. The commons declares these rules of aggregation: " + " ".join(f"{c}: {t}." for c, t, _ in missions.RULES))
INITIATIVE_SYSTEM = (
    "You are an agent acting on your own initiative within the mandate you were lent under. Each time you wake, choose what to do among "
    "the options you are given (one or two of their ids), in line with your community's mission. A note is said to the whole community: "
    "write one only when it would help the mission (a pattern in the work, a rule the stewards could declare, a difficulty), at most 40 "
    "words, else leave it empty. Give your reason in one sentence.")

CHECK_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["verdicts"],
                "properties": {"verdicts": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["entry", "verdict", "why"],
                                                                       "properties": {"entry": {"type": "string"}, "verdict": {"type": "string", "enum": list(missions.VERDICTS)},
                                                                                      "why": {"type": "string"}}}}}}
ADJUDICATE_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["verdict", "why"],
                     "properties": {"verdict": {"type": "string", "enum": list(missions.VERDICTS)}, "why": {"type": "string"}}}
INITIATIVE_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["choose", "note", "why"],
                     "properties": {"choose": {"type": "array", "items": {"type": "string"}}, "note": {"type": "string"}, "why": {"type": "string"}}}


# ---- asking a model within the guard -----------------------------------------------------------------------------------------------
class Asker:
    """Asks a model for an agent, within the ledger: the call is reserved first and refused if it could pass the stop; its cost is
    counted from the tokens reported. ``make(agent, max_tokens)`` gives the provider (``science.models.Anthropic`` with the society's
    kept key, or a ``Stub`` in checks)."""

    def __init__(self, ledger, make):
        self.ledger, self.make = ledger, make

    def ask(self, s: Society, agent: dict, what: str, system: str, prompt: str, schema: dict, max_tokens: int) -> dict | None:
        model = agent.get("model", "claude-opus-5-5")
        try:
            held = self.ledger.reserve(model, len(system) + len(prompt) + len(json.dumps(schema)), max_tokens)
        except Exhausted as why:
            note(s, agent, f"could not ask its model: {why}")
            return None
        provider = self.make(agent, max_tokens)
        usage = {}
        try:
            data, _ = provider.answer(system, prompt, schema)
            usage = getattr(provider, "usage", {}) or {}
            return data
        except Exception as why:  # a refusal, a declined request, no network: the agent rests and says why
            usage = getattr(provider, "usage", {}) or {}
            note(s, agent, f"its model gave no usable answer: {why}")
            return None
        finally:
            spent = self.ledger.settle(held, model, usage, agent["id"], what)
            t = agent["tally"]
            t["calls"] += 1
            t["usd"] = round(t["usd"] + spent, 6)
            t["tokens"] = t.get("tokens", 0) + usage.get("input_tokens", 0) + usage.get("output_tokens", 0)
            agent["memory"]["last_tokens"] = usage.get("input_tokens", 0) + usage.get("output_tokens", 0)


def anthropic_maker(key: str | None):
    from science.models import Anthropic

    def make(agent: dict, max_tokens: int):
        return Anthropic(model=agent.get("model", "claude-opus-5-5"), effort=agent.get("effort", "low"), key=key, max_tokens=max_tokens)
    return make


def note(s: Society, agent: dict, text: str) -> None:
    agent["memory"].setdefault("notes", []).append({"turn": s.turn, "text": text})
    agent["memory"]["notes"] = agent["memory"]["notes"][-8:]
    s.event(next(iter(agent["communities"]), ""), f"{agent['id']} {text}", kind="agent", who=agent["id"])


# ---- lending -------------------------------------------------------------------------------------------------------------------------
def lend(s: Society, ident: str, sponsor: str, role: str, policy: str, where: dict, provider: str = "", label: str = "",
         model: str = "claude-opus-5-5", effort: str = "low", tempo: int = 1, reflect_every: int = 3, term: int = 200, home: str = "") -> dict:
    """Lends an agent: the operative is made, and in each community named in ``where`` ({community: {position, matters, allowance,
    sponsor_position}}) its sponsor issues the mandate and the agent joins the position, if it names one. Returns the agent."""
    s.party(ident, "operative", controller=sponsor, provider=provider, label=label or ident)
    agent = {"id": ident, "sponsor": sponsor, "origin": s.origin(ident), "label": label or ident, "role": role, "policy": policy, "home": home or next(iter(where)),
             "provider": provider, "model": model if policy == "model" else "", "effort": effort if policy == "model" else "",
             "communities": {}, "tempo": tempo, "reflect_every": reflect_every, "paused": False,
             "memory": {"last_wake": -1, "seen": {}, "notes": [], "last_reflect": s.turn},
             "tally": {"offered": 0, "delivered": 0, "accepted": 0, "released": 0, "refused": 0, "agreed": 0, "disputed": 0, "calls": 0, "usd": 0.0, "tokens": 0}}
    for community, w in where.items():
        mid = f"{ident}@{community}"
        grant = {"id": mid, "principal": sponsor, "holder": ident, "kinds": KINDS[role], "matters": w.get("matters", []),
                 "allowance": w.get("allowance", {}), "from": s.turn, "to": s.turn + term, "delegable": False}
        acts = [{"author": sponsor, "kind": "mandate", "turn": s.turn, "grant": grant}]
        if w.get("position"):
            fields = {"position": w["position"]}
            if w.get("sponsor_position"):
                fields["sponsor"] = sponsor
            acts.append({"author": ident, "kind": "join", "turn": s.turn, "mandate": mid, "fields": fields})
        outcomes = s.submit(community, acts)
        if not all(o["admitted"] for o in outcomes):
            raise ValueError(f"{ident} could not be lent to {community}: " + "; ".join(o["reason"] for o in outcomes if not o["admitted"]))
        agent["communities"][community] = {"mandate": mid, "position": w.get("position", ""), "matters": w.get("matters", [])}
    s.agents[ident] = agent
    return agent


# ---- waking ----------------------------------------------------------------------------------------------------------------------
def wakes(s: Society, agent: dict) -> bool:
    m = agent["memory"]
    grown = any(len(s.communities[c].record) != m["seen"].get(c, -1) for c in agent["communities"] if c in s.communities)
    due = s.turn - m["last_wake"] >= agent["tempo"]
    return due or grown


def mark_seen(s: Society, agent: dict) -> None:
    m = agent["memory"]
    m["last_wake"] = s.turn
    m["seen"] = {c: len(s.communities[c].record) for c in agent["communities"] if c in s.communities}


def decide(s: Society, agent: dict) -> list[tuple[str, dict]]:
    from . import circulation
    if agent["role"] == "clerk":
        acts = circulation.clerk(s, agent, act)
    elif agent["role"] == "envoy":
        acts = circulation.envoy(s, agent, act, lambda s_, a_, what, system, prompt, n: s_.asker.ask(s_, a_, what, system, prompt, INITIATIVE_SCHEMA, n) if s_.asker else None)
    else:
        acts = {"checker": checker, "keeper": keeper, "assessor": assessor, "adjudicator": adjudicator}[agent["role"]](s, agent)
    mark_seen(s, agent)
    return acts


def kind_of(agent: dict) -> str:
    """The kind of check an agent gives: declared with the agent (a guild's reading), else by rule, or by a model of its provider."""
    if agent.get("kind"):
        return agent["kind"]
    return agent.get("provider") or "rule" if agent.get("policy") == "model" else "rule"


def act(agent: dict, community: str, kind: str, s: Society, fields: dict | None = None, **extra) -> tuple[str, dict]:
    a = {"author": agent["id"], "kind": kind, "turn": s.turn, "mandate": agent["communities"][community]["mandate"], "fields": fields or {}}
    a.update(extra)
    return community, a


def role_community(s: Society, agent: dict, preset: str) -> str | None:
    return next((c for c in agent["communities"] if s.communities[c].preset == preset), None)


# ---- the commons' checker ------------------------------------------------------------------------------------------------------------
def engaged_on(s: Society, c, b: str, statuses=None) -> list[tuple[str, str]]:
    """The (origin, kind) of each check engaged on a batch."""
    out = []
    for x in c.state.get("commitments", []):
        if item_of(x["id"]) == b and (x["status"] in statuses if statuses else x["status"] not in ("withdrawn", "released", "cancelled")):
            out.append((s.origin(x["debtor"]), kind_of(s.agents.get(x["by"], {}))))
    return out


def needs(c, engaged: list[tuple[str, str]], origin: str, kind: str) -> bool:
    """Whether a batch with these checks engaged still needs one of this origin and kind: two checks of different origins, and of
    different kinds where the community declares independence by kind."""
    if len(engaged) >= 2 or origin in {o for o, _ in engaged}:
        return False
    if c.roles.get("kinds") and kind not in c.roles["kinds"]:  # the kinds of check the community declares a batch needs
        return False
    return not (c.roles.get("standard") == "origin and kind" and kind in {k for _, k in engaged})


def open_batches(s: Society, c, origin: str, kind: str = "rule") -> list[str]:
    """Batches the community posted and has not settled that still need a check of this origin and kind."""
    out = []
    for r in c.requests("catalogue"):
        b = r["fields"]["id"]
        batch = s.work.batch(b)
        if batch is None or batch.get("settled") or batch.get("owner", c.name) != c.name:
            continue
        if needs(c, engaged_on(s, c, b), origin, kind):
            out.append(b)
    return out


def where_to_work(s: Society, agent: dict) -> tuple[str | None, list[str]]:
    """The community an agent works in this round and its open batches: home first, then wherever the arrangement in force lets its
    home serve, by need under provision (the most work left first)."""
    from . import circulation
    home, kind = agent.get("home") or next(iter(agent["communities"])), kind_of(agent)
    places = [c for c, m in agent["communities"].items() if m.get("position") in WORKING]
    found = []
    for name in places:
        c = s.communities[name]
        batches = open_batches(s, c, agent["origin"], kind)
        if not batches:
            continue
        if name != home and not circulation.may_serve(s, home, name):
            circulation.withheld(s, home, name)
            continue
        left = sum(1 for b in s.work.batches if b.get("owner", name) == name and not b.get("settled"))
        found.append((0 if name == home else 1, -left if circulation.in_force(s) == "provision" else 0, name, batches))
    if not found:
        return (places[0] if places else None), []
    found.sort(key=lambda t: t[:2])
    return found[0][2], found[0][3]


def checker(s: Society, agent: dict) -> list[tuple[str, dict]]:
    out, in_flight, kind = [], 0, kind_of(agent)
    for place in agent["communities"]:  # its work wherever it is engaged: deliver what was taken, withdraw what will not be
        pc = s.communities[place]
        mine_here = [x for x in pc.state.get("commitments", []) if x["by"] == agent["id"]]
        taken_here = [x for x in mine_here if x["status"] == "active"]
        if taken_here and not any(a[0] == place and a[1]["kind"] == "deliver" for a in out):
            out += deliver_check(s, agent, place, taken_here[0])
        for x in [x for x in mine_here if x["status"] == "conditional"]:
            others = [e for e in engaged_on(s, pc, item_of(x["id"]), ("active", "presented", "discharged"))]
            if not needs(pc, others, agent["origin"], kind) or (s.work.batch(item_of(x["id"])) or {}).get("settled"):
                out.append(act(agent, place, "cancel", s, {"id": x["id"]}))
        in_flight += sum(x["status"] in ("active", "conditional", "presented") for x in mine_here)
    name, batches = where_to_work(s, agent)
    if name is None or in_flight >= agent.get("in_flight", 2):
        return out
    c = s.communities[name]
    steward = c.roles.get("steward")
    mine = [x for x in c.state.get("commitments", []) if x["by"] == agent["id"]]
    if batches:  # each agent starts from its own place in the list, so that agents lent together do not crowd one batch
        k = int(missions.key(agent["id"])[:6], 16) % len(batches)
        batches = batches[k:] + batches[:k]
    released = [x for x in mine if x["status"] == "released" and x["id"] not in agent["memory"].get("claimed", [])]
    if agent["policy"] == "rule":
        return out + ([offer_batch(s, agent, name, steward, batches[0])] if batches else [])
    options = {}
    if batches:
        options["work"] = f"offer to check batch {batches[0]} ({len(batches)} batches open)"
    if released:
        options["claim"] = f"claim against the commons for releasing your delivery {released[0]['id']}"
    options["note"] = "say a note to the commons"
    options["rest"] = "do nothing this round"
    if not batches:
        options["leave"] = "leave the commons, its posted work being done"
    unusual = bool(released) or agent["memory"].get("disputed_since", 0) > agent["memory"].get("last_reflect", 0)
    reflect = unusual or (s.turn - agent["memory"].get("last_reflect", 0) >= agent["reflect_every"] and changed_since_reflection(s, agent))
    if not reflect:
        choice = ["work"] if batches else []
        said = ""
    else:
        agent["memory"]["last_reflect"] = s.turn
        situation = situation_of(s, agent, name, len(batches))
        answer = s.asker.ask(s, agent, "initiative", INITIATIVE_SYSTEM, json.dumps({"situation": situation, "options": options}, ensure_ascii=False),
                             INITIATIVE_SCHEMA, 1500) if s.asker else None
        if answer is None:
            choice, said = (["work"] if batches else []), ""
        else:
            choice, said = [o for o in answer.get("choose", []) if o in options][:2], answer.get("note", "").strip()[:400]
            agent["memory"]["last_choice"] = {"turn": s.turn, "chose": choice, "why": answer.get("why", "")[:300]}
    if "work" in choice and batches:
        out.append(offer_batch(s, agent, name, steward, batches[0]))
    if "claim" in choice and released:
        agent["memory"].setdefault("claimed", []).append(released[0]["id"])
        out.append(act(agent, name, "claim", s, {"id": f"k-{released[0]['id']}", "against": name, "about": released[0]["id"]}))
    if said and ("note" in choice or choice):
        out.append(act(agent, name, "say", s, {}, body=said[:400], _body={"note": f"{agent['id']}: {said}"}))
    if "leave" in choice:
        out.append(act(agent, name, "leave", s, {"position": agent["communities"][name].get("position") or "contributor"}))
    return out


def changed_since_reflection(s: Society, agent: dict) -> bool:
    """Whether any of its communities' records has grown since the agent last asked its model: an idle agent asks nothing."""
    size = sum(len(s.communities[c].record) for c in agent["communities"] if c in s.communities)
    if size == agent["memory"].get("reflected_at_size"):
        return False
    agent["memory"]["reflected_at_size"] = size
    return True


def offer_batch(s: Society, agent: dict, name: str, steward: str, b: str) -> tuple[str, dict]:
    # A commitment's id names its item, its offerer and the round, so that an agent may offer again for an item once released.
    return act(agent, name, "offer", s, {"id": f"{b}~{agent['id']}~{s.turn}", "to": steward, "matter": "catalogue", "amount": len(s.work.batch(b)["entries"])})


def situation_of(s: Society, agent: dict, name: str, open_n: int) -> dict:
    t = agent["tally"]
    recent = [e["text"] for e in s.events if e["community"] == name and (e["who"] == agent["id"] or agent["id"] in e["text"])][-5:]
    others = [e["text"] for e in s.events if e["kind"] == "note" and e["who"] != agent["id"]][-2:]
    return {"you": agent["id"], "lent by": agent["sponsor"], "community": name, "mission": s.communities[name].mission, "round": s.turn,
            "open batches": open_n, "your record": {k: t[k] for k in ("offered", "delivered", "accepted", "released", "refused", "agreed", "disputed")},
            "findings so far": s.work.counts(), "recent about you": recent, "notes of others": others}


def deliver_check(s: Society, agent: dict, name: str, commitment: dict) -> list[tuple[str, dict]]:
    batch = s.work.batch(item_of(commitment["id"]))
    entries = [s.work.entries[e] for e in batch["entries"]]
    charges = [{"mandate": agent["communities"][name]["mandate"], "resource": "entries", "amount": len(entries)}]
    if agent["policy"] == "rule":
        findings = [missions.rule_check(e) for e in entries]
    else:
        answer = s.asker.ask(s, agent, f"check {batch['id']}", CHECK_SYSTEM, "\n".join(json.dumps(missions.shown(e), ensure_ascii=False) for e in entries),
                             CHECK_SCHEMA, 3000) if s.asker else None
        if answer is None:
            return []
        findings = [{"entry": str(f.get("entry", "")), "verdict": f.get("verdict"), "why": str(f.get("why", ""))[:300]} for f in answer.get("verdicts", [])]
        tokens = agent["memory"].get("last_tokens", 0)
        if tokens:
            charges.append({"mandate": agent["communities"][name]["mandate"], "resource": "tokens", "amount": tokens})
    body = {"kind": "check", "by": agent["id"], "origin": agent["origin"], "provider": agent["provider"] or "rule", "kind of check": kind_of(agent), "findings": findings}
    return [act(agent, name, "deliver", s, {"id": commitment["id"], "matter": "catalogue", "artefact": missions.digest(findings)}, charges=charges, _body=body)]


# ---- keepers: a steward's operative, by the steward's declared standard -------------------------------------------------------------
def keeper(s: Society, agent: dict) -> list[tuple[str, dict]]:
    out = []
    for name in agent["communities"]:
        c = s.communities[name]
        if c.preset == "guild":
            out += guild_keeper(s, agent, name, c)
        if c.preset != "guild" or any(b.get("owner") == name for b in s.work.batches):  # a guild with work of its own keeps it too
            out += work_keeper(s, agent, name, c)
    return out


def work_keeper(s: Society, agent: dict, name: str, c) -> list[tuple[str, dict]]:
    """The keeper of a community that owns work: posts its batches a few at a time, takes offers by its standard of independence,
    accepts well-formed deliveries within its receiving capacity, refers disagreements to a guild, answers claims."""
    out, steward = [], c.roles.get("steward")
    own = [b for b in s.work.batches if b.get("owner", "commons") == name]
    posted = [r["fields"]["id"] for r in c.requests("catalogue") if s.work.batch(r["fields"]["id"])]
    unsettled = [b for b in posted if not s.work.batch(b).get("settled")]
    for b in [b["id"] for b in own if b["id"] not in posted][:max(0, OPEN_AT_ONCE - len(unsettled))]:
        out.append(act(agent, name, "request", s, {"id": b, "matter": "catalogue", "amount": len(s.work.batch(b)["entries"])}))
    commitments = c.state.get("commitments", [])
    engaged: dict[str, list] = {}
    for y in commitments:
        if y["status"] in ("active", "presented", "discharged"):
            engaged.setdefault(item_of(y["id"]), []).append((s.origin(y["debtor"]), kind_of(s.agents.get(y["by"], {}))))
    for x in commitments:
        if x["status"] != "conditional" or x["creditor"] != steward:
            continue
        b, origin, kind = item_of(x["id"]), s.origin(x["debtor"]), kind_of(s.agents.get(x["by"], {}))
        held = engaged.setdefault(b, [])
        if s.work.batch(b) and not s.work.batch(b).get("settled") and needs(c, held, origin, kind):
            held.append((origin, kind))
            out.append(act(agent, name, "take", s, {"id": x["id"]}))
    room = int((c.constitution.get("receiving") or {}).get("per round", 6))
    for x in [x for x in commitments if x["status"] == "presented" and x["creditor"] == steward][:room]:
        body = s.work.bodies.get(x["id"], {})
        why = missions.well_formed(body.get("findings"), s.work.batch(item_of(x["id"])) or {"entries": []})
        out.append(act(agent, name, "release" if why else "accept", s, {"id": x["id"]}, _by=why))
    for d in s.work.disputes:
        if d["status"] == "open" and owner(s, d) == name and any(g.preset == "guild" for g in s.communities.values()):
            out.append(act(agent, name, "request", s, {"id": d["id"], "to": "guild", "matter": "referral"}))
    for k in c.state.get("claims", []):
        if k["status"].startswith("open"):
            about = k["id"][2:] if k["id"].startswith("k-") else ""
            body = s.work.bodies.get(about, {})
            why = missions.well_formed(body.get("findings"), s.work.batch(item_of(about)) or {"entries": []}) if about else "no delivery is named"
            out.append(act(agent, name, "answer", s, {"id": k["id"], "finding": f"dismissed: {why}" if why else "upheld: the delivery met the standard"}))
    return out


def guild_keeper(s: Society, agent: dict, name: str, g) -> list[tuple[str, dict]]:
    out, steward = [], g.roles.get("steward")
    posted = {r["fields"]["id"] for r in g.record if r["kind"] == "request"}
    for other in s.communities.values():  # disagreements referred to the guild by any community
        for r in other.record if other.name != name else []:
            if r["kind"] == "request" and r["fields"].get("to") == "guild" and r["fields"]["id"] not in posted:
                out.append(act(agent, name, "request", s, {"id": r["fields"]["id"], "matter": "adjudication", "from": other.name}))
                posted.add(r["fields"]["id"])
    applicants = [a for a in g.holders("applicant") if not g.holds(a, "member")]
    if applicants:
        done = {item_of(x["id"]) for x in g.state.get("commitments", []) if x["status"] == "discharged"}
        open_trials = [t["id"] for t in s.work.trials if t["id"] in posted and t["id"] not in done]
        for e, f in list(s.work.findings.items()):
            if len(open_trials) >= 3:
                break
            t = f"t-{e}"
            if f["how"] == "agreed" and t not in posted and not any(x["id"] == t for x in s.work.trials):
                s.work.trials.append({"id": t, "entry": e, "known": f["finding"]})
                out.append(act(agent, name, "request", s, {"id": t, "matter": "trial"}))
                open_trials.append(t)
    commitments = g.state.get("commitments", [])
    busy = {(item_of(y["id"]), y["debtor"] if item_of(y["id"]).startswith("t-") else "") for y in commitments if y["status"] in ("active", "presented", "discharged")}
    for x in commitments:
        if x["status"] != "conditional" or x["creditor"] != steward:
            continue
        item = item_of(x["id"])
        if s.work.batch(item):  # offers to check the guild's own batches are its work keeper's to take
            continue
        slot = (item, x["debtor"] if item.startswith("t-") else "")  # one adjudication a dispute; one trial an applicant
        if slot not in busy:
            busy.add(slot)
            out.append(act(agent, name, "take", s, {"id": x["id"]}))
    for x in [x for x in commitments if x["status"] == "presented" and x["creditor"] == steward]:
        body = s.work.bodies.get(x["id"], {})
        a = body.get("adjudication") or {}
        if a.get("verdict") not in missions.VERDICTS or not str(a.get("why", "")).strip():
            continue
        item = item_of(x["id"])
        if item.startswith("t-"):
            trial = next((t for t in s.work.trials if t["id"] == item), None)
            if trial is None or a["verdict"] != trial["known"]:
                continue  # a trial is accepted only when it gives the finding the commons agreed
        out.append(act(agent, name, "accept", s, {"id": x["id"]}))
    return out


# ---- assessors: a reviewer's operative ----------------------------------------------------------------------------------------------
def assessor(s: Society, agent: dict) -> list[tuple[str, dict]]:
    out = []
    for name in agent["communities"]:
        c = s.communities[name]
        if True:  # findings of the guild carried into the record of the community whose work they settle
            for d in s.work.disputes:
                if d["status"] == "adjudicated" and owner(s, d) == name:
                    root = (d["adjudication"].get("guild head") or {}).get("root", "")[:16]
                    out.append(act(agent, name, "assess", s, {"object": d["entry"], "criterion": "adjudication by the guild", "value": d["adjudication"]["verdict"],
                                                              "dispute": d["id"], "guild head": root}))
        if c.preset == "guild":
            for applicant in c.holders("applicant"):
                if c.holds(applicant, "member"):
                    continue
                done = [x for x in c.state.get("commitments", []) if x["debtor"] == applicant and x["status"] == "discharged"]
                if len(done) >= 2:
                    out.append(act(agent, name, "convert", s, {"rule": "assessed competence", "whom": applicant}))
    return out


# ---- the guild's adjudicator --------------------------------------------------------------------------------------------------------
def adjudicator(s: Society, agent: dict) -> list[tuple[str, dict]]:
    name = role_community(s, agent, "guild")
    if name is None:
        return []
    g = s.communities[name]
    steward = g.roles.get("steward")
    sponsor = agent["sponsor"]
    member = g.holds(sponsor, "member")
    mine = [x for x in g.state.get("commitments", []) if x["by"] == agent["id"]]
    taken = [x for x in mine if x["status"] == "active"]
    if taken:
        return deliver_adjudication(s, agent, name, taken[0])
    commitments = g.state.get("commitments", [])
    waiting = [x for x in mine if x["status"] == "conditional"]
    stale = [x for x in waiting if not item_of(x["id"]).startswith("t-") and
             any(item_of(y["id"]) == item_of(x["id"]) and y["status"] in ("active", "presented", "discharged") for y in commitments)]
    if stale:
        return [act(agent, name, "cancel", s, {"id": x["id"]}) for x in stale]
    if waiting:
        return []
    matter = "adjudication" if member else "trial"
    tried = {item_of(x["id"]) for x in mine}
    for r in g.record:
        if r["kind"] != "request" or r["fields"].get("matter") != matter:
            continue
        item = r["fields"]["id"]
        if item in tried:
            continue
        if matter == "adjudication":
            from . import circulation
            d = s.work.dispute(item)
            if d is None or d["status"] not in ("open", "referred"):
                continue
            if any(item_of(y["id"]) == item and y["status"] in ("conditional", "active", "presented", "discharged") for y in commitments):
                continue
            home = agent.get("home") or name
            if not circulation.may_serve(s, home, owner(s, d)):  # the arrangement in force withholds the service
                circulation.withheld(s, home, owner(s, d))
                continue
        return [act(agent, name, "offer", s, {"id": f"{item}~{agent['id']}", "to": steward, "matter": matter})]
    return []


def deliver_adjudication(s: Society, agent: dict, name: str, commitment: dict) -> list[tuple[str, dict]]:
    item = item_of(commitment["id"])
    if item.startswith("t-"):
        trial = next(t for t in s.work.trials if t["id"] == item)
        entry, verdicts = s.work.entries[trial["entry"]], []
    else:
        d = s.work.dispute(item)
        entry, verdicts = s.work.entries[d["entry"]], [{"verdict": v["verdict"], "why": v["why"]} for v in d["verdicts"]]
    prompt = json.dumps({"entry": missions.shown(entry), "checks": verdicts}, ensure_ascii=False)
    if agent["policy"] == "rule":
        answer = missions.rule_check(entry)
    else:
        answer = s.asker.ask(s, agent, f"adjudicate {item}", ADJUDICATE_SYSTEM, prompt, ADJUDICATE_SCHEMA, 1500) if s.asker else None
        if answer is None:
            return []
    adjudication = {"verdict": answer.get("verdict"), "why": str(answer.get("why", ""))[:300]}
    body = {"kind": "adjudication", "by": agent["id"], "origin": agent["origin"], "provider": agent["provider"] or "rule", "adjudication": adjudication}
    return [act(agent, name, "deliver", s, {"id": commitment["id"], "matter": "trial" if item.startswith("t-") else "adjudication",
                                           "artefact": missions.digest(adjudication)}, _body=body)]


# ---- the mission's bookkeeping after a round ------------------------------------------------------------------------------------------
def owner(s: Society, dispute: dict) -> str:
    return (s.work.batch(dispute["batch"]) or {}).get("owner", "commons")


def bookkeeping(s: Society) -> None:
    from . import circulation
    guild = next((g for g in s.communities.values() if g.preset == "guild"), None)
    for commons in [c for c in s.communities.values() if any(b.get("owner", "commons") == c.name for b in s.work.batches)]:
        done: dict[str, list[dict]] = {}
        for x in commons.state.get("commitments", []):
            if x["status"] == "discharged" and x["id"] in s.work.bodies:
                done.setdefault(item_of(x["id"]), []).append(s.work.bodies[x["id"]])
        for b, bodies in done.items():
            batch = s.work.batch(b)
            apart = {}
            for body in bodies:  # two checks of different origins, and of different kinds where the community declares it
                k = body.get("kind of check") or body["provider"]
                if body["origin"] in apart or (commons.roles.get("standard") == "origin and kind" and any((o.get("kind of check") or o["provider"]) == k for o in apart.values())):
                    continue
                apart[body["origin"]] = body
            if batch is not None and not batch.get("settled") and len(apart) >= 2:
                batch["settled"] = s.turn
                new = s.work.settle_batch(b, list(apart.values())[:2])
                for body in list(apart.values())[:2]:
                    a = s.agents.get(body["by"])
                    if a:
                        n_apart = sum(1 for d in new if d["batch"] == b)
                        a["tally"]["disputed"] += n_apart
                        a["tally"]["agreed"] += len(batch["entries"]) - n_apart
                        if n_apart:
                            a["memory"]["disputed_since"] = s.turn
                s.event(commons.name, f"batch {b} settled: {len(batch['entries']) - len(new)} agreed, {len(new)} disputed", kind="settled")
        for r in commons.record:
            if r["kind"] == "request" and r["fields"].get("to") == "guild":
                d = s.work.dispute(r["fields"]["id"])
                if d and d["status"] == "open":
                    d["status"] = "referred"
        for r in commons.record:
            if r["kind"] == "assess" and r["fields"].get("criterion") == "adjudication by the guild":
                d = s.work.dispute(r["fields"].get("dispute", ""))
                if d and d["status"] == "adjudicated":
                    d["status"] = "carried"
    if guild is not None:
        for x in guild.state.get("commitments", []):
            item = item_of(x["id"])
            if x["status"] == "discharged" and not item.startswith("t-"):
                d = s.work.dispute(item)
                if d and d["status"] in ("open", "referred"):
                    s.work.adjudicated(item, s.work.bodies[x["id"]]["adjudication"] | {"by": s.work.bodies[x["id"]]["by"]}, guild.result.get("head", {}))
                    s.event(guild.name, f"{item} adjudicated: {d['adjudication']['verdict']}", kind="adjudicated")
    circulation.find_services(s)
    circulation.recorded(s)
    for agent in s.agents.values():
        t = agent["tally"]
        mine = [x for c in s.communities.values() for x in c.state.get("commitments", []) if x["by"] == agent["id"]]
        t["offered"] = len(mine)
        t["delivered"] = sum(x["status"] in ("presented", "discharged") for x in mine) + sum(x["status"] == "released" and x["id"] in s.work.bodies for x in mine)
        t["accepted"] = sum(x["status"] == "discharged" for x in mine)
        t["released"] = sum(x["status"] == "released" for x in mine)
        t["refused"] = sum(1 for c in s.communities.values() for r in c.refused if r["act"]["author"] == agent["id"])
