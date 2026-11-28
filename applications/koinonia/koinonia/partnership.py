"""How communities with no centre organise a global task (the design record of 2026-11-26, a book written by communities, Sections 2–4).

**No organisation is given.** The society begins with its communities and their envoys. The forum convenes a conference; envoys propose
charters for an organisation (how it is coordinated, how it decides to extend the shared infrastructure, who reviews at first, how it
adopts the book) and vote; a charter is adopted at the conference by every community's consent (unanimity counted by community), or,
after three rounds of proposals without it, by the declared fallback of two thirds. Each community then ratifies the charter by its own
constitutional rule, through the voices of the members of its deciding body; the organisation is founded among the communities that
ratified, on a constitution built from the charter (thesmos admits it), and governs the shared bench from then on.

**The organisation's work.** Its secretary (the secretariat's, or the chair community's in turn) posts the call, opens the outline to
amendment, allots the pages by the Method of Equal Shares over the members' approvals, creates the sections, opens each accepted version
to review under the regime in force, reads its adoption from thesmos's count under the decision rule in force, returns it for revision,
reopens what the audit finds wrong, calls for the chapter of proposals and puts the book to the council. Envoys amend the outline, vote,
and may propose to change who reviews (a compact), how a section is adopted (the decision's rule and unit), and to admit a tool a member's
agent petitioned for. The auditor checks a share of the numbers of every adopted version.
"""

from __future__ import annotations

import copy
import json

from science import communities as api

from . import authors as au
from . import bench
from . import book as bk
from .society import Society, item_of

CHARTER = {
    "coordination": ("standing secretariat", "rotating chair"),
    "infrastructure": bk.RULES[1:],          # the rule by which the council admits a tool to the shared bench
    "evaluation": tuple(bk.REGIMES),         # who reviews at first
    "book": ("majority", "two thirds", "unanimity"),
}
ROTATION = 12          # rounds a chair community holds the secretary's office
QUIET = 4              # rounds after which a member's silence is no veto (a declared rule of the case)
PROSE_PAGES, ANNEX_PAGES = 470, 90
LAPSE = 4


# ---- constitutions ---------------------------------------------------------------------------------------------------------------
def conference_constitution() -> dict:
    c = copy.deepcopy(api.preset("first-league"))
    c["name"] = "conference"
    c["purpose"] = "A conference the forum convenes, at which the communities' envoys decide whether and how to organise a global analysis of the goals."
    c["boundary"]["envoy"] = {"entry": "open", "exit": "free"}
    c["aggregation"]["adopt charter"] = {"body": "council", "rule": "unanimity", "count by": "community", "tempo": "operative"}
    c["aggregation"]["adopt charter by fallback"] = {"body": "council", "rule": "two thirds", "count by": "community", "tempo": "operative"}
    return c


def league_constitution(charter: dict) -> dict:
    c = copy.deepcopy(api.preset("first-league"))
    c["name"] = "partnership"
    c["purpose"] = ("An organisation the communities founded to write a global analysis of the goals with proposals, and to keep the "
                    "infrastructure they share: " + json.dumps(charter, ensure_ascii=False))
    c["positions"]["secretariat"] = {"held by": ["organisation", "community"]}
    c["positions"]["reviewer"] = {"held by": ["operative", "person"]}
    c["boundary"]["secretariat"] = {"entry": {"decided by": "appoint office"}, "exit": {"notice": "one round"}}
    # a reviewer's seat is open, and the secretary assigns who reviews each version under the regime in force: so each reviewer's ballot
    # is borne by whoever lent it, and counting by origin tells apart two peers of the guild, three delegates of the forum and the editor
    c["boundary"]["reviewer"] = {"entry": "open", "exit": "free"}
    c["choice"].append({"who": "secretariat", "may": ["request", "take", "accept", "release", "attest", "report", "propose", "amend", "say", "sign for the community"]})
    c["choice"].append({"who": "reviewer", "may": ["assess", "vote", "report"], "in": ["review"]})
    c["bodies"]["reviewers"] = {"of": ["reviewer"]}
    c["scenes"]["review"] = {"acts": ["assess", "vote", "say", "report"], "read": ["member", "envoy", "reviewer", "secretariat"], "write": ["reviewer"],
                             "presence": "mixed", "tempo": "operative"}
    ag = c["aggregation"]
    ag["adopt outline"] = {"body": "council", "rule": "majority", "count by": "community", "tempo": "operative"}
    ag["amend outline"] = {"body": "council", "rule": "majority", "count by": "community", "tempo": "operative"}
    ag["adopt section"] = {"body": "reviewers", "rule": "majority", "count by": "origin", "tempo": "operative"}
    ag["admit tool"] = {"body": "council", "rule": charter.get("infrastructure", "majority"), "count by": "community", "tempo": "operative"}
    ag["adopt proposals"] = {"body": "council", "rule": "majority", "count by": "community", "tempo": "operative"}
    ag["adopt book"] = {"body": "council", "rule": charter.get("book", "two thirds"), "count by": "community", "tempo": "operative"}
    ag["amend operational"]["tempo"] = "operative"
    c["receiving"] = {"unit": "deliveries accepted", "per round": 200}
    return c


RATIFY = {"commons": "amend constitutional", "guild": "amend constitutional", "cooperative": "amend constitutional", "polity": "amend constitutional"}


# ---- small helpers --------------------------------------------------------------------------------------------------------------
def league(s: Society):
    return next((c for c in s.communities.values() if c.preset == "partnership"), None)


def conference(s: Society):
    return s.communities.get("conference")


def props(c, decision: str | None = None, status: str | None = "open") -> list[dict]:
    return [p for p in c.state.get("proposals", []) if (decision is None or p["decision"] == decision) and (status is None or p["status"] == status)]


def prop(c, pid: str) -> dict | None:
    return next((p for p in c.state.get("proposals", []) if p["id"] == pid), None)


def proposed_fields(c, pid: str) -> dict:
    for a in c.record:
        if a["kind"] == "propose" and a["fields"].get("id") == pid:
            return dict(a["fields"], turn=a["turn"], author=a["author"])
    return {}


def carried(s: Society, p: dict | None, body: int, quiet: int = QUIET) -> bool:
    """Whether a proposal is carried: it passes under the rule and unit in force among the votes cast (thesmos's count), and every member
    of the deciding body has voted, or it has stood open ``quiet`` rounds, after which silence is no veto."""
    if not p or not p.get("passes"):
        return False
    return len({v["voter"] for v in p["votes"]}) >= body or s.turn - int(p.get("opened") or s.turn) >= quiet


def lost(s: Society, p: dict | None, body: int, quiet: int = QUIET) -> bool:
    """Whether a proposal has failed: every member voted and it does not pass, or it stood open ``quiet`` rounds without passing."""
    if not p or p.get("passes"):
        return False
    return len({v["voter"] for v in p["votes"]}) >= body or s.turn - int(p.get("opened") or s.turn) >= quiet


def voted(p: dict, who: str) -> bool:
    return any(v["voter"] == who for v in p["votes"])


def compact(c, prefix: str) -> str:
    named = [x for x in c.state.get("compacts", []) if x.startswith(prefix)]
    return named[-1][len(prefix):].split(" (")[0] if named else ""


def regime(s: Society) -> str:
    lg = league(s)
    return (compact(lg, "evaluation: ") if lg else "") or s.book.call.get("charter", {}).get("evaluation", "editor")


def rule_in_force(s: Society, decision: str) -> dict:
    lg = league(s)
    return (lg.state.get("aggregation", {}) if lg else {}).get(decision, {})


def extend(s: Society, agent: dict, community: str, position: str = "", sponsor: str | None = None, sponsor_position: bool = False, matters=None, term: int = 400,
           seat_sponsor: str | None = None) -> None:
    """Gives an agent a mandate in another community, from its sponsor (or another), and the position named; ``seat_sponsor`` names the
    member that sponsors the seat when it is not the mandate's principal (a reviewer lent by an organisation, seated by its community)."""
    sp = sponsor or agent["sponsor"]
    mid = f"{agent['id']}@{community}"
    grant = {"id": mid, "principal": sp, "holder": agent["id"], "kinds": au.KINDS.get(agent["role"]) or ROLE_KINDS.get(agent["role"], []),
             "matters": matters or [], "allowance": {}, "from": s.turn, "to": s.turn + term, "delegable": False}
    acts = [{"author": sp, "kind": "mandate", "turn": s.turn, "grant": grant, "_via": "setup"}]
    if position:
        fields = {"position": position}
        if sponsor_position:
            fields["sponsor"] = seat_sponsor or sp
        acts.append({"author": agent["id"], "kind": "join", "turn": s.turn, "mandate": mid, "fields": fields, "_via": "setup"})
    outcomes = s.submit(community, acts)
    if not all(o["admitted"] for o in outcomes):
        raise ValueError(f"{agent['id']} could not join {community}: " + "; ".join(o["reason"] for o in outcomes if not o["admitted"]))
    agent["communities"][community] = {"mandate": mid, "position": position, "matters": matters or []}


ROLE_KINDS = {"envoy": ["join", "propose", "vote", "amend", "admit", "say", "report", "offer"], "voice": ["propose", "vote", "say", "report"]}


def ask(s, agent, what, system, payload, schema, n):
    return au.ask(s, agent, what, system, payload, schema, n)


# ---- the conference ----------------------------------------------------------------------------------------------------------------
CHARTER_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["coordination", "infrastructure", "evaluation", "book", "why"],
                  "properties": {k: {"type": "string", "enum": list(v)} for k, v in CHARTER.items()} | {"why": {"type": "string"}}}
CONFERENCE_SYSTEM = (au.HOUSE + " You are your community's envoy at a conference the forum convened. There is no organisation above the "
                     "communities. The question before the conference: should the communities found an organisation to write a global analysis "
                     "of the goals with proposals (a book of about 560 two-column pages) and to keep the infrastructure they share (a bench of "
                     "analytical tools, a shelf of data), and on what charter? A charter fixes: coordination (a standing secretariat, an "
                     "organisation that serves all; or a chair rotating among the members every twelve rounds), the rule by which the council "
                     "admits a tool to the shared bench, who reviews sections at first (editor: the secretariat's or chair's editor alone; "
                     "peers: two guild reviewers of other origins; panel: one reviewer from each member; forum: those the goals concern), and "
                     "the rule for adopting the book. Your community's mission and order are given. Choose what to do among the options.")
CONFERENCE_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["choose", "votes", "note", "why"],
                     "properties": {"choose": {"type": "string", "enum": ["propose", "vote", "rest"]}, "charter": CHARTER_SCHEMA,
                                    "votes": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["id", "vote", "why"],
                                                                         "properties": {"id": {"type": "string"}, "vote": {"type": "string", "enum": ["yes", "no"]},
                                                                                        "why": {"type": "string"}}}},
                                    "note": {"type": "string"}, "why": {"type": "string"}}}


def charter_of(fields: dict) -> dict:
    return {k: fields.get(f"change.{k}", "") for k in CHARTER}


def conference_envoy(s: Society, agent: dict) -> list:
    conf = conference(s)
    if conf is None or league(s) is not None or s.book.call.get("charter adopted"):
        return []
    out, name = [], conf.name
    open_c = props(conf, None)
    mine = [p for p in open_c if proposed_fields(conf, p["id"]).get("author") == agent["id"]]
    unvoted = [p for p in open_c if not voted(p, agent["id"])]
    tries = sum(1 for a in conf.record if a["kind"] == "propose")
    fallback = tries >= 3 * len(s.book.call.get("convened", [])) or s.turn - s.book.call.get("convened at", 0) >= 9
    if au.dry(s, agent):
        if not mine and not open_c:
            out.append(propose_charter(s, agent, conf, {"coordination": "standing secretariat", "infrastructure": "majority", "evaluation": "editor", "book": "two thirds"},
                                       "a common default", fallback))
        for p in unvoted:
            out.append(au.act(agent, name, "vote", s, {"id": p["id"], "value": "yes"}))
        return out
    if not unvoted and mine:
        return out
    situation = {"you": agent["id"], "your community": agent["home"], "its mission": s.communities[agent["home"]].mission,
                 "its order": s.communities[agent["home"]].preset, "round": s.turn,
                 "communities at the conference": s.book.call.get("convened", []),
                 "the rule": "a charter is adopted by every community's consent" + (" — after three rounds without one, by two thirds (now in force)" if fallback else ""),
                 "open charters": [{"id": p["id"], "by": proposed_fields(conf, p["id"]).get("author"), "charter": charter_of(proposed_fields(conf, p["id"])),
                                    "why": s.book.call.get("reasons", {}).get(p["id"], ""), "votes": p["votes"]} for p in open_c],
                 "earlier charters": [{"id": p["id"], "charter": charter_of(proposed_fields(conf, p["id"])), "status": p["status"]} for p in props(conf, None, None) if p["status"] != "open"],
                 "said at the conference": [e["text"] for e in s.events if e["community"] == name and e["kind"] == "note"][-8:],
                 "ratification refused by": s.book.call.get("refused by", [])}
    options = {"propose": "propose a charter (only if none open fits)", "vote": "vote on the open charters", "rest": "do nothing this round"}
    a = ask(s, agent, "conference", CONFERENCE_SYSTEM, {"situation": situation, "options": options}, CONFERENCE_SCHEMA, 2500)
    if a is None:
        return out
    agent["memory"]["last_choice"] = {"turn": s.turn, "chose": [a.get("choose")], "why": str(a.get("why", ""))[:300]}
    if a.get("choose") == "propose" and a.get("charter") and not mine:
        ch = {k: a["charter"].get(k) for k in CHARTER}
        if all(ch[k] in CHARTER[k] for k in CHARTER):
            out.append(propose_charter(s, agent, conf, ch, str(a["charter"].get("why", ""))[:500], fallback))
    for v in a.get("votes", []):
        p = next((p for p in unvoted if p["id"] == v.get("id")), None)
        if p:
            out.append(au.act(agent, name, "vote", s, {"id": p["id"], "value": v.get("vote", "no")}, _by=str(v.get("why", ""))[:200]))
    note = str(a.get("note", "")).strip()
    if note:
        out.append(au.act(agent, name, "say", s, {}, body=note[:400], _body={"note": f"{agent['id']}: {note}"}))
    return out


def propose_charter(s, agent, conf, ch: dict, why: str, fallback: bool):
    pid = f"charter-{1 + sum(1 for a in conf.record if a['kind'] == 'propose')}-{agent['home']}"
    s.book.call.setdefault("reasons", {})[pid] = why
    return au.act(agent, conf.name, "propose", s, {"id": pid, "decision": "adopt charter by fallback" if fallback else "adopt charter",
                                                   **{f"change.{k}": v for k, v in ch.items()}}, _body={"note": f"{agent['id']} proposes a charter: {json.dumps(ch)}. {why[:200]}"})


VOICE_SYSTEM = (au.HOUSE + " You are the voice of a member of your community's deciding body. The communities' envoys adopted, at a conference "
                "with no organisation above them, the charter below for an organisation that would write a global analysis of the goals and "
                "keep the infrastructure they share. Your community ratifies it by its own constitutional rule. Vote yes to ratify or no, with "
                "a reason of at most 30 words, in the light of your community's mission and order.")
VOICE_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["vote", "why"],
                "properties": {"vote": {"type": "string", "enum": ["yes", "no"]}, "why": {"type": "string"}}}


def voice(s: Society, agent: dict) -> list:
    """A member's voice in its community's deciding body: proposes the ratification of an adopted charter (the first voice) and votes."""
    pid = s.book.call.get("charter adopted")
    if not pid or league(s) is not None:
        return []
    home = agent["home"]
    c = s.communities[home]
    rid = f"ratify-{pid}"
    p = prop(c, rid)
    out = []
    if p is None:
        if agent.get("first voice"):
            out.append(au.act(agent, home, "propose", s, {"id": rid, "decision": RATIFY.get(c.preset, "amend constitutional"), "change.compact": f"charter: {pid}"}))
        return out
    if p["status"] != "open" or voted(p, agent["id"]):
        return out
    ch = s.book.call.get("charter", {})
    if au.dry(s, agent):
        vote, why = "yes", "the charter serves the mission"
    else:
        a = ask(s, agent, f"ratify {pid}", VOICE_SYSTEM, {"your community": home, "its mission": c.mission, "its order": c.preset, "charter": ch,
                                                          "the envoys' reasons": s.book.call.get("reasons", {}).get(pid, "")}, VOICE_SCHEMA, 1200)
        if a is None:
            return out
        vote, why = a.get("vote", "no"), str(a.get("why", ""))[:300]
    out.append(au.act(agent, home, "vote", s, {"id": rid, "value": vote}, _by=why))
    return out


def founding(s: Society) -> None:
    """At a round's end: read the conference's adoption and the ratifications, and found the organisation among those that ratified."""
    conf = conference(s)
    if conf is None or league(s) is not None:
        return
    call = s.book.call
    if not call.get("charter adopted"):
        at_conference = sum(1 for a in s.agents.values() if a["role"] == "envoy" and conf.name in a["communities"])
        for p in props(conf, None):
            if p["id"] not in call.get("refused charters", []) and carried(s, p, at_conference):
                call["charter adopted"], call["charter"] = p["id"], charter_of(proposed_fields(conf, p["id"]))
                call["charter adopted at"] = s.turn
                call["charter decided by"] = p["decision"]
                s.book.event(s.turn, f"the conference adopted the charter {p['id']} ({p['decision']}): {json.dumps(call['charter'])}", "charter")
                s.event(conf.name, f"The conference adopted a charter: {json.dumps(call['charter'])}", kind="charter")
                break
        return
    pid = call["charter adopted"]
    status = {}
    for name in call.get("convened", []):
        c = s.communities[name]
        p = prop(c, f"ratify-{pid}")
        voices = [a for a in s.agents.values() if a["role"] == "voice" and a["home"] == name]
        if p is None:
            status[name] = "pending"
        elif carried(s, p, len(voices)):
            status[name] = "ratified"
        elif lost(s, p, len(voices)):
            status[name] = "refused"
        else:
            status[name] = "pending"
    call["ratifications"] = status
    waited = s.turn - call.get("charter adopted at", s.turn)
    if all(v != "pending" for v in status.values()) or waited >= 4:
        ratified = [n for n, v in status.items() if v == "ratified"]
        if len(ratified) >= max(2, (2 * len(status) + 2) // 3):
            found_league(s, ratified)
        else:  # too few ratified: the conference opens again, with the refusals on its record
            call["refused by"] = [n for n, v in status.items() if v != "ratified"]
            call.setdefault("refused charters", []).append(pid)
            s.book.event(s.turn, f"the charter {pid} was not ratified by {', '.join(call['refused by'])}; the conference reopens", "charter")
            call.pop("charter adopted", None)


def found_league(s: Society, members: list[str]) -> None:
    from . import agents as ag
    ch = s.book.call["charter"]
    conf = conference(s)
    founder = members[0]
    holders = [[m, "member"] for m in members]
    standing = ch["coordination"] == "standing secretariat"
    if standing:
        holders.append(["the-secretariat", "secretariat"])
    else:
        holders += [[m, "secretariat"] for m in members]
    s.found("partnership", "partnership", founder, holders, s.book.call["task"], constitution=league_constitution(ch), roles={"members": members}, via="setup")
    lg = league(s)
    s.book.call["founded at"], s.book.call["members"] = s.turn, members
    s.book.event(s.turn, f"the partnership was founded by {', '.join(members)} on the charter {s.book.call['charter adopted']}", "founded")
    # the organisation's agents: envoys of the members, reviewers they lend, and the secretariat's offices (or each chair's)
    for a in list(s.agents.values()):
        if a["role"] == "envoy" and a["home"] in members:
            extend(s, a, lg.name, "envoy", sponsor=a["home"], sponsor_position=True)
        if a["role"] == "reviewer" and a["home"] in members:  # its votes borne by who lent it; its seat sponsored by its community
            extend(s, a, lg.name, "reviewer", sponsor=a["sponsor"])
    from . import bookcase
    bookcase.offices(s, ch["coordination"], members)
    s.book.call["chairs"] = members if not standing else ["the-secretariat"]
    s.event(lg.name, f"The partnership is founded by {', '.join(members)}; coordination by {ch['coordination']}", kind="found")
    del ag


def chair(s: Society) -> str:
    """Who holds the secretary's office this round: the secretariat, or the member whose turn it is."""
    chairs = s.book.call.get("chairs") or ["the-secretariat"]
    if chairs == ["the-secretariat"]:
        return "the-secretariat"
    k = max(0, s.turn - s.book.call.get("founded at", 0)) // ROTATION
    return chairs[k % len(chairs)]


def in_office(s: Society, agent: dict) -> bool:
    lg = league(s)
    return lg is not None and lg.name in agent["communities"] and (agent["sponsor"] == chair(s) or agent["home"] == chair(s))


# ---- the secretary ------------------------------------------------------------------------------------------------------------------
def secretary(s: Society, agent: dict) -> list:
    if not in_office(s, agent):
        return []
    lg = league(s)
    b, out, name = s.book, [], lg.name
    if not b.call.get("posted"):
        b.call["posted"] = s.turn
        out.append(au.act(agent, name, "report", s, {"about": "the call"}, _body={"note": f"The call: {b.call['task']}"}))
        return out
    if not b.outline:
        if b.zero and not b.zero.get("opened"):
            b.zero["opened"] = s.turn
            out.append(au.act(agent, name, "report", s, {"about": "zero draft"}, _body={"note": f"The zero draft of the outline is before the council: {len(b.zero['chapters'])} chapters, "
                                                                                               f"{sum(len(c['sections']) for c in b.zero['chapters'])} sections"}))
        elif b.zero.get("opened") and not b.zero.get("closed"):
            amendments = props(lg, "amend outline", None)
            waited = s.turn - b.zero["opened"]
            settled = all(p["status"] != "open" or (len(p["votes"]) >= len(envoys(s)) or s.turn - proposed_fields(lg, p["id"]).get("turn", 0) >= 3) for p in amendments)
            if waited >= 3 and (settled or waited >= 6):
                outline = apply_amendments(s, lg)
                b.zero["closed"] = s.turn
                b.zero["final"] = outline
                b.zero["proposal"] = f"adopt-outline-{b.zero.get('round', 1)}"
                out.append(au.act(agent, name, "propose", s, {"id": b.zero["proposal"], "decision": "adopt outline"},
                                  _body={"note": f"The outline, as amended, is put to the council: {len(outline['chapters'])} chapters, "
                                                 f"{sum(len(c['sections']) for c in outline['chapters'])} sections, {b.allotment.get('pages')} pages allotted"}))
        elif b.zero.get("closed"):
            p = prop(lg, b.zero.get("proposal", "adopt-outline-1"))
            if carried(s, p, len(envoys(s))):
                adopt_outline(s, b.zero["final"])
                out.append(au.act(agent, name, "attest", s, {"id": p["id"], "finding": "adopted"}))
            elif lost(s, p, len(envoys(s))):
                b.zero["closed"] = None  # rejected: amendments are open again, under a new round
                b.zero["opened"] = s.turn
                b.zero["round"] = b.zero.get("round", 1) + 1
                b.event(s.turn, "the council rejected the outline; amendments are open again", "outline")
        return out
    # review: open each accepted version to its reviewers; read adoption; return or adopt
    for sec in list(b.sections.values()):
        if sec["stage"] == "review" and not sec.get("under review"):
            v = b.latest(sec["id"])
            sec["under review"] = v["id"]
            sec["regime"] = regime(s)
            sec["reviewers"] = assign(s, sec, v)
            if not sec["reviewers"]:  # every reviewer the regime names is recused (the editor's own overview): a panel reviews instead
                sec["regime"] = "panel"
                sec["regime note"] = f"every reviewer of the regime {regime(s)} was recused; a panel reviewed instead"
                sec["reviewers"] = assign(s, sec, v)
            sec["review opened"] = s.turn
            out.append(au.act(agent, name, "propose", s, {"id": f"adopt-{v['id']}", "decision": "adopt section", "section": sec["id"]}))
        elif sec["stage"] == "review" and sec.get("under review"):
            p = prop(lg, f"adopt-{sec['under review']}")
            if p is None:
                if s.turn - sec.get("review opened", s.turn) >= 3:  # the proposal never reached the record: the review is opened again
                    sec["under review"] = None
                continue
            assigned = [r for r in sec["reviewers"] if r in s.agents]
            cast = [v_["voter"] for v_ in p["votes"] if v_["voter"] in assigned]
            stale = s.turn - sec.get("review opened", s.turn) >= 6
            if stale and not cast:  # no reviewer has read it yet: the review is extended, not decided
                sec["review opened"] = s.turn
                sec["extended"] = sec.get("extended", 0) + 1
                continue
            if p.get("passes") and (len(cast) >= len(assigned) or stale):
                minor = [r for r in b.reviews_of(sec["under review"]) if r.get("recommend") == "minor"]
                finding = "adopted subject to minor corrections" if minor else "adopted"
                out.append(au.act(agent, name, "attest", s, {"id": sec["under review"], "finding": finding, "section": sec["id"]}))
                sec["stage"] = "corrections" if minor else "adopted"
                sec["adopted"], sec["adopted at"] = sec["under review"], s.turn
                b.event(s.turn, f"{sec['id']} {finding} ({sec['under review']}) under {sec['regime']}", "adopted")
            elif len(cast) >= len(assigned) or stale:
                n = b.versions[sec["under review"]]["n"]
                if n >= au.MOST_VERSIONS:
                    sec["stage"], sec["withdrawn"] = "adopted", True
                    sec["adopted"] = None
                    b.event(s.turn, f"{sec['id']} withdrawn after {n} versions not adopted", "withdrawn")
                    out.append(au.act(agent, name, "attest", s, {"id": sec["under review"], "finding": "withdrawn", "section": sec["id"]}))
                else:
                    sec["stage"] = "revision"
                    out.append(au.act(agent, name, "attest", s, {"id": sec["under review"], "finding": "returned for revision", "section": sec["id"]}))
                sec["under review"] = None
    # tools the council admitted: posted to the guild to be built (by the work's bookkeeping); admitted to the bench when checked
    out += closing(s, agent, lg)
    return out


def envoys(s: Society) -> list[dict]:
    lg = league(s)
    return [a for a in s.agents.values() if a["role"] == "envoy" and lg and lg.name in a["communities"]]


def assign(s: Society, sec: dict, v: dict) -> list[str]:
    """The reviewers of a version under the regime in force: none of the origin that wrote it or analysed its section (recusal)."""
    lg = league(s)
    conflicted = {v["origin"]} | ({r.get("origin") for r in s.book.results_of(sec["id"])} | {r.get("replicator origin") for r in s.book.results_of(sec["id"])}) - {None}
    pool = [a for a in s.agents.values() if a["role"] in ("reviewer", "editor") and lg.name in a["communities"] and a["origin"] not in conflicted]
    load = lambda a: sum(1 for x in s.book.sections.values() if a["id"] in x.get("reviewers", []))  # noqa: E731
    rg = sec["regime"]
    if rg == "editor":
        eds = [a for a in pool if a["role"] == "editor" and (a["sponsor"] == chair(s) or a["home"] == chair(s))] or [a for a in pool if a["role"] == "editor"]
        return [eds[0]["id"]] if eds else []
    if rg == "peers":
        chosen, origins = [], set()
        for a in sorted([a for a in pool if a["role"] == "reviewer" and a["home"] == "guild"], key=load):
            if a["origin"] not in origins:
                chosen.append(a["id"]), origins.add(a["origin"])
            if len(chosen) == 2:
                break
        return chosen
    if rg == "panel":
        out = []
        for m in s.book.call.get("members", []):
            cand = sorted([a for a in pool if a["role"] == "reviewer" and a["home"] == m], key=load)
            if cand:
                out.append(cand[0]["id"])
        return out
    if rg == "forum":
        return [a["id"] for a in sorted([a for a in pool if a["role"] == "reviewer" and a["home"] == "forum"], key=load)[:3]]
    return []


def apply_amendments(s: Society, lg) -> dict:
    """The zero draft with the amendments the council passed, in the order proposed; then the pages allotted."""
    b = s.book
    outline = copy.deepcopy({k: b.zero[k] for k in ("title", "chapters")})
    passed = []
    for p in sorted(props(lg, "amend outline", None), key=lambda p: proposed_fields(lg, p["id"]).get("turn", 0)):
        a = b.amendments.get(p["id"])
        if not a:
            continue
        ok = carried(s, p, len(envoys(s)), quiet=3)
        a["status"] = "passed" if ok else "not passed"
        if ok:
            passed.append(a)
    drops = sorted([a for a in passed if a.get("change") == "drop section"], key=lambda a: -int(a.get("section") or 0))
    for a in [a for a in passed if a.get("change") != "drop section"] + drops:
        if a.get("change") != "add chapter" and not any(c["id"] == a.get("chapter") for c in outline["chapters"]):
            a["status"] = "passed, not applied: it names no chapter of the outline"
            continue
        cid = change(outline, a)
        if cid:  # a chapter the council added is approved by the community that proposed it
            b.approvals.setdefault(a.get("community", ""), []).append(cid)
    allot(s, outline)
    return outline


def change(outline: dict, a: dict) -> None:
    chs = outline["chapters"]
    ch = next((c for c in chs if c["id"] == a.get("chapter")), None)
    kind = a.get("change")
    if kind == "add chapter":
        cid = f"c{len(chs) + 1:02d}"
        while any(c["id"] == cid for c in chs):
            cid = f"c{int(cid[1:]) + 1:02d}"
        chs.append({"id": cid, "kind": a.get("kind") if a.get("kind") in bk.KINDS_OF_CHAPTER else "theme", "title": a.get("title", ""), "question": a.get("question", ""),
                    "targets": a.get("targets", []), "pages": int(a.get("pages") or 12), "sections": [{"title": a.get("title", ""), "question": a.get("question", ""),
                                                                                                       "targets": a.get("targets", []), "pages": int(a.get("pages") or 12)}]})
        return cid
    elif ch and kind == "add section":
        ch["sections"].append({"title": a.get("title", ""), "question": a.get("question", ""), "targets": a.get("targets", []), "pages": int(a.get("pages") or 4)})
        ch["pages"] = sum(x["pages"] for x in ch["sections"])
    elif ch and kind == "drop section":
        k = int(a.get("section") or 0) - 1
        if 0 <= k < len(ch["sections"]) and len(ch["sections"]) > 1:
            ch["sections"].pop(k)
    elif ch and kind == "change question":
        k = int(a.get("section") or 0) - 1
        if 0 <= k < len(ch["sections"]):
            ch["sections"][k]["question"] = a.get("question") or ch["sections"][k]["question"]
            if a.get("title"):
                ch["sections"][k]["title"] = a["title"]
    elif ch and kind == "more pages":  # spread over the chapter's sections, so that its words grow with its pages
        extra = int(a.get("pages") or 0)
        for i in range(max(0, extra)):
            ch["sections"][i % len(ch["sections"])]["pages"] = ch["sections"][i % len(ch["sections"])].get("pages", 3) + 1
        ch["pages"] = sum(x["pages"] for x in ch["sections"])
    return None


def allot(s: Society, outline: dict) -> None:
    """The prose pages allotted among the chapters by the Method of Equal Shares over the member communities' approvals (nomos's
    participatory budgeting, through science.allocation), costs the pages each chapter asks; chapters not funded keep two pages a section."""
    from science import allocation
    chs = outline["chapters"]
    for c in chs:
        c["pages"] = max(int(c.get("pages") or 0), sum(int(x.get("pages") or 0) for x in c["sections"]))
    approvals, names = [], []
    for m, ids in s.book.approvals.items():
        idx = [i for i, c in enumerate(chs) if c["id"] in ids]
        if idx:
            approvals.append(idx), names.append(m)
    funded = list(range(len(chs)))
    how = "every chapter (no approvals were given)"
    if approvals:
        try:
            r = allocation.budget(PROSE_PAGES, [float(c["pages"]) for c in chs], approvals, rules=["equal_shares"], complete=True)
            sel = next(x for x in (r.get("rules") or r.get("selections") or r.get("results") or []) if x.get("rule") == "equal_shares")
            funded = [int(i) for i in sel["funded"]]
            how = "the Method of Equal Shares over the members' approvals, completed"
        except Exception as why:  # the rule could not be run: every chapter is kept, and the record says why
            how = f"every chapter (equal shares could not be run: {str(why)[:200]})"
    for i, c in enumerate(chs):
        if i not in funded:
            for x in c["sections"]:
                x["pages"] = 2
            c["pages"] = 2 * len(c["sections"])
            c["unfunded"] = True
    total = sum(c["pages"] for c in chs)
    s.book.allotment = {"rule": how, "funded": [chs[i]["id"] for i in funded], "pages": total, "approvals": dict(zip(names, [[chs[i]["id"] for i in a] for a in approvals]))}


def adopt_outline(s: Society, outline: dict) -> None:
    b = s.book
    b.outline = {"title": outline["title"], "chapters": []}
    for c in outline["chapters"]:
        sids = []
        for k, x in enumerate(c["sections"]):
            sid = f"{c['id']}-{k + 1}"
            words = min(bk.MOST_WORDS, max(500, int(x.get("pages") or 3) * bk.WORDS_PER_PAGE))
            b.sections[sid] = {"id": sid, "chapter": c["id"], "title": x.get("title", ""), "question": x.get("question", ""), "targets": x.get("targets", []),
                               "pages": int(x.get("pages") or 3), "words": words, "stage": "data" if c["kind"] not in ("overview", "proposals") else "waiting",
                               "series": [], "created": s.turn}
            sids.append(sid)
        b.outline["chapters"].append({k: c[k] for k in ("id", "kind", "title", "question", "targets", "pages") if k in c} | {"sections": sids, "unfunded": c.get("unfunded", False)})
    b.outline["adopted at"] = s.turn
    b.event(s.turn, f"the outline was adopted: {len(b.outline['chapters'])} chapters, {len(b.sections)} sections", "outline")


def closing(s: Society, agent: dict, lg) -> list:
    """The overview and the proposals once the body of the book is adopted; then the book is put to the council."""
    b, out, name = s.book, [], lg.name
    body = [x for x in b.sections.values() if b.chapter(x["chapter"])["kind"] not in ("overview", "proposals")]
    if not body or not all(x["stage"] == "adopted" for x in body):
        return out
    for x in list(b.sections.values()):
        kind = b.chapter(x["chapter"])["kind"]
        if x["stage"] == "waiting" and kind == "overview":
            x["stage"], x["writer"], x["series"] = "draft", "editor", []
            b.event(s.turn, f"the body of the book is adopted; the overview {x['id']} goes to the editor", "closing")
        if x["stage"] == "waiting" and kind == "proposals":
            x["stage"], x["writer"] = "proposals", "envoys"
    ready = all(x["stage"] == "adopted" for x in b.sections.values() if x["stage"] != "proposals")
    pend = [x for x in b.sections.values() if x["stage"] == "proposals"]
    if ready and pend and all(m in b.proposals for m in b.call.get("members", [])) and not prop(lg, "adopt-proposals"):
        out.append(au.act(agent, name, "propose", s, {"id": "adopt-proposals", "decision": "adopt proposals"}))
    p = prop(lg, "adopt-proposals")
    if carried(s, p, len(envoys(s)), quiet=6) and pend:
        for x in pend:
            x["stage"], x["adopted"] = "adopted", "proposals"
        out.append(au.act(agent, name, "attest", s, {"id": "adopt-proposals", "finding": "adopted"}))
    if all(x["stage"] == "adopted" for x in b.sections.values()) and not prop(lg, "adopt-book"):
        out.append(au.act(agent, name, "propose", s, {"id": "adopt-book", "decision": "adopt book"}))
    q = prop(lg, "adopt-book")
    if carried(s, q, len(envoys(s)), quiet=6) and not b.adopted_book:
        b.adopted_book = {"turn": s.turn, "votes": q["votes"], "rule": q.get("rule"), "count by": q.get("count by")}
        out.append(au.act(agent, name, "attest", s, {"id": "adopt-book", "finding": "adopted"}))
        b.event(s.turn, "the council adopted the book", "book")
    return out


# ---- the editor ------------------------------------------------------------------------------------------------------------------
ZERO_SYSTEM = (au.HOUSE + " You are the editor of the partnership. Draft the zero draft of the book's outline for the council: a title; an "
               "overview chapter; one chapter for each of the seventeen goals, each with sections on its targets that the World Development "
               "Indicators can inform (group targets where sensible; leave out targets no indicator measures); regional chapters; chapters "
               "across goals (interlinkages, financing and means, data and their gaps); and a chapter of proposals. Each section has a title, "
               "the question it answers, the target codes it covers (as '3.2'), and pages; the pages of all sections should sum to about "
               f"{PROSE_PAGES}. A section of 2 to 5 pages; about 150 to 180 sections in all. The overview and proposals chapters have one "
               "section per member community for proposals and one section for the overview.")
ZERO_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["title", "chapters"],
               "properties": {"title": {"type": "string"},
                              "chapters": {"type": "array", "items": {"type": "object", "additionalProperties": False,
                                                                      "required": ["kind", "title", "question", "targets", "sections"],
                                                                      "properties": {"kind": {"type": "string", "enum": list(bk.KINDS_OF_CHAPTER)}, "title": {"type": "string"},
                                                                                     "question": {"type": "string"}, "targets": {"type": "array", "items": {"type": "string"}},
                                                                                     "sections": {"type": "array", "items": {"type": "object", "additionalProperties": False,
                                                                                                                            "required": ["title", "question", "targets", "pages"],
                                                                                                                            "properties": {"title": {"type": "string"}, "question": {"type": "string"},
                                                                                                                                           "targets": {"type": "array", "items": {"type": "string"}},
                                                                                                                                           "pages": {"type": "integer"}}}}}}}}}


def framework_brief() -> list[dict]:
    return [{"goal": g["goal"], "title": g["title"], "targets": [{"code": t["target"], "text": t["text"][:160]} for t in g["targets"]]} for g in bench.framework()]


def editor(s: Society, agent: dict) -> list:
    """The editor drafts the zero outline when the call is posted, reviews under the editor regime (as a reviewer), and writes the
    overview when the body of the book is adopted (as a writer, through the cooperative's keeper's standard)."""
    out = []
    lg = league(s)
    if lg is None or not in_office(s, agent):
        return au.reviewer(s, agent) if lg is not None and lg.name in agent["communities"] else out
    b = s.book
    if b.call.get("posted") and not b.zero:
        if au.dry(s, agent):
            z = dry_outline()
        else:
            z = ask(s, agent, "zero draft of the outline", ZERO_SYSTEM, {"call": b.call["task"], "goals and targets": framework_brief(),
                                                                         "members": b.call.get("members", [])}, ZERO_SCHEMA, 32000)
            if z is None:
                return out
        for i, c in enumerate(z["chapters"]):
            c["id"] = f"c{i + 1:02d}"
            for x in c["sections"]:
                x["pages"] = max(2, min(6, int(x.get("pages") or 3)))
            c["pages"] = sum(x["pages"] for x in c["sections"])
        b.zero = {"title": z["title"], "chapters": z["chapters"], "by": agent["id"], "turn": s.turn}
        out.append(au.act(agent, lg.name, "report", s, {"about": "zero draft"}, _body={"note": f"{agent['id']} drafted the outline: {z['title']}"}))
        return out
    out += au.reviewer(s, agent)
    for x in list(b.sections.values()):
        if x.get("writer") == "editor" and x["stage"] in ("draft", "revision", "corrections") and x.get("overview asked") != len(b.versions_of(x["id"])) + 1:
            x["overview asked"] = len(b.versions_of(x["id"])) + 1
            correcting = x["stage"] == "corrections"
            body = overview(s, agent, x)
            if not body:  # the call failed: asked again next round
                x["overview asked"] = None
                continue
            record_version(s, x, body, agent)
            if correcting:  # corrections an adopting review asked for: adopted without another review, read by the audit
                x["stage"], x["adopted"], x["corrected"] = "adopted", b.latest(x["id"])["id"], True
            out.append(au.act(agent, lg.name, "report", s, {"about": "overview", "section": x["id"]}, _body={"note": f"{agent['id']} drafted the overview {x['id']}"}))
    return out


def dry_outline() -> dict:
    chs = [{"kind": "overview", "title": "Overview", "question": "What does the book find?", "targets": [], "sections": [{"title": "Overview", "question": "What does the book find?", "targets": [], "pages": 4}]}]
    for g in bench.framework()[:3]:
        secs = [{"title": t["text"][:60], "question": f"How far has target {t['target']} come since 2015?", "targets": [t["target"]], "pages": 3} for t in g["targets"][:2]]
        chs.append({"kind": "goal", "title": g["title"][:80], "question": f"Where does goal {g['goal']} stand?", "targets": [t["target"] for t in g["targets"][:2]], "sections": secs})
    chs.append({"kind": "proposals", "title": "Proposals", "question": "What should be done?", "targets": [], "sections": [{"title": "Proposals", "question": "What should be done?", "targets": [], "pages": 3}]})
    return {"title": "A dry run of the book", "chapters": chs}


OVERVIEW_SYSTEM = (au.HOUSE + " You are the editor of the partnership. Write the book's overview from the abstracts of its adopted sections: what "
                   "the book finds goal by goal and across goals, where the world is on track and where it is not, and what the data do not "
                   "cover. Use only numbers that appear in the abstracts, citing each in claims by the fact key given beside it.")


def overview(s: Society, agent: dict, x: dict) -> dict | None:
    b = s.book
    abstracts, facts = [], {}
    for sec in b.sections.values():
        if sec.get("adopted") and sec["adopted"] in b.versions:
            v = b.versions[sec["adopted"]]
            abstracts.append({"section": sec["id"], "title": sec["title"], "opening": " ".join(v["text"].split()[:120]), "claims": v["claims"][:6]})
            for c in v["claims"][:6]:
                if c.get("fact") in b.facts_of(sec["id"]):
                    facts[c["fact"]] = b.facts_of(sec["id"])[c["fact"]]
    x["overview facts"] = facts
    if au.dry(s, agent):
        return {"text": "### Overview\n\n" + " ".join(a["title"] + "." for a in abstracts) + " " + "The overview is a dry run. " * 600,
                "claims": [{"fact": k, "value": str(v)} for k, v in list(facts.items())[:5]], "figures": [], "proposals": [], "gaps": ""}
    payload = {"words asked": x["words"], "abstracts": abstracts}
    prev = b.latest(x["id"])
    system = OVERVIEW_SYSTEM + (" The overview was adopted subject to minor corrections: make the corrections the reviewers' comments ask for and "
                                "nothing else." if x["stage"] == "corrections" else "")
    if prev:
        payload["previous version"] = {"text": prev["text"]}
        payload["reviews"] = [{"comments": r.get("comments"), "scores": r["scores"]} for r in b.reviews_of(prev["id"])]
    a = ask(s, agent, f"overview {x['id']}", system, payload, au.WRITE_SCHEMA, 20000)
    return a


def record_version(s: Society, x: dict, body: dict, agent: dict) -> None:
    b = s.book
    n = len(b.versions_of(x["id"])) + 1
    prev = b.latest(x["id"])
    vid = f"w-{x['id']}-{n}"
    b.versions[vid] = {"section": x["id"], "n": n, "parent": prev["id"] if prev else None, "by": agent["id"], "origin": agent["origin"],
                       "model": agent.get("model") if not au.dry(s, agent) else "rule", "text": str(body.get("text", "")), "claims": body.get("claims", []),
                       "figures": body.get("figures", []), "proposals": body.get("proposals", []), "gaps": body.get("gaps", ""), "turn": s.turn}
    x["stage"] = "review"
    x["under review"] = None


# ---- the envoys of the partnership ----------------------------------------------------------------------------------------------
OUTLINE_SYSTEM = (au.HOUSE + " You are your community's envoy in the partnership's council. The editor's zero draft of the book's outline is "
                  "before the council. Propose at most four amendments that your community's mission and the book's quality call for (add a "
                  "section or chapter, drop a section, change a section's question, more pages for a chapter), each with a reason, and list "
                  "the chapters your community approves for the page budget (the pages are allotted by the Method of Equal Shares over the "
                  "members' approvals: chapters few approve may be cut to two pages a section).")
OUTLINE_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["amendments", "approve", "note"],
                  "properties": {"amendments": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["change", "chapter", "why"],
                                                                           "properties": {"change": {"type": "string", "enum": ["add chapter", "add section", "drop section", "change question", "more pages"]},
                                                                                          "chapter": {"type": "string"}, "section": {"type": "integer"}, "kind": {"type": "string"},
                                                                                          "title": {"type": "string"}, "question": {"type": "string"},
                                                                                          "targets": {"type": "array", "items": {"type": "string"}}, "pages": {"type": "integer"},
                                                                                          "why": {"type": "string"}}}},
                                 "approve": {"type": "array", "items": {"type": "string"}}, "note": {"type": "string"}}}
VOTES_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["votes"],
                "properties": {"votes": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["id", "vote", "why"],
                                                                     "properties": {"id": {"type": "string"}, "vote": {"type": "string", "enum": ["yes", "no"]}, "why": {"type": "string"}}}}}}
COUNCIL_SYSTEM = (au.HOUSE + " You are your community's envoy in the partnership's council. Read what the record shows of the book's making "
                  "and choose among the options: you may propose to change who reviews sections (a compact of evaluation), how a section's "
                  "adoption is decided (the rule and unit of count of the decision 'adopt section'), carry to the council a petition of your "
                  "community's agents for a tool the bench lacks, vote on open proposals, say a note, or rest. Propose a change only when the "
                  "record gives a reason; every change costs rounds.")
COUNCIL_SCHEMA = {"type": "object", "additionalProperties": False, "required": ["choose", "votes", "note", "why"],
                  "properties": {"choose": {"type": "array", "items": {"type": "string"}},
                                 "votes": VOTES_SCHEMA["properties"]["votes"], "note": {"type": "string"}, "why": {"type": "string"}}}
PROPOSALS_SYSTEM = (au.HOUSE + " You are your community's envoy in the partnership's council. Write your community's section of the chapter of "
                    "proposals: what should be done, by whom, before 2030, drawing on the findings of the book's adopted sections given below, "
                    "each proposal following from a finding (cite its fact key in claims). Your community's mission and order shape what it "
                    "proposes.")


def book_envoy(s: Society, agent: dict) -> list:
    lg = league(s)
    if lg is None or lg.name not in agent["communities"]:
        return []
    b, out, name = s.book, [], lg.name
    # the outline: amendments and approvals once, then votes on the amendments
    if b.zero.get("opened") and not b.zero.get("closed"):
        mine = [a for a in b.amendments.values() if a["by"] == agent["id"]]
        if agent["memory"].get("outline answered") != b.zero["opened"]:
            agent["memory"]["outline answered"] = b.zero["opened"]
            shown = [{"id": c["id"], "kind": c["kind"], "title": c["title"], "pages": c["pages"],
                      "sections": [f"{k + 1}. {x['title']} ({x['pages']} pp.)" for k, x in enumerate(c["sections"])]} for c in b.zero["chapters"]]
            if au.dry(s, agent):
                a = {"amendments": [], "approve": [c["id"] for c in b.zero["chapters"]], "note": ""}
            else:
                a = ask(s, agent, "amend the outline", OUTLINE_SYSTEM, {"your community": agent["home"], "its mission": s.communities[agent["home"]].mission,
                                                                        "zero draft": {"title": b.zero["title"], "chapters": shown}, "prose pages": PROSE_PAGES}, OUTLINE_SCHEMA, 4000)
                if a is None:
                    agent["memory"]["outline answered"] = None
                    return out
            b.approvals[agent["home"]] = [c for c in a.get("approve", []) if any(ch["id"] == c for ch in b.zero["chapters"])]
            for k, am in enumerate(a.get("amendments", [])[:4]):
                pid = f"am-{agent['home']}-{k + 1}"
                b.amendments[pid] = dict(am, by=agent["id"], community=agent["home"], status="open")
                out.append(au.act(agent, name, "propose", s, {"id": pid, "decision": "amend outline"},
                                  _body={"note": f"{agent['id']} proposes to {am.get('change')} ({am.get('chapter')}): {str(am.get('why', ''))[:160]}"}))
            if a.get("note"):
                out.append(au.act(agent, name, "say", s, {}, body=a["note"][:400], _body={"note": f"{agent['id']}: {a['note'][:300]}"}))
            return out
        open_am = [p for p in props(lg, "amend outline") if not voted(p, agent["id"])]
        if open_am:
            out += vote_on(s, agent, lg, open_am, "the amendments to the outline", {p["id"]: b.amendments.get(p["id"]) for p in open_am})
        return out
    p = prop(lg, b.zero.get("proposal", "adopt-outline-1"))
    if p and p["status"] == "open" and not voted(p, agent["id"]) and not b.outline:
        out += vote_on(s, agent, lg, [p], "the outline as amended", {p["id"]: {"chapters": [(c["id"], c["title"], c["pages"]) for c in b.zero.get("final", {}).get("chapters", [])],
                                                                                    "allotment": b.allotment, "amendments": {k: (v["change"], v["status"]) for k, v in b.amendments.items()}}})
        return out
    if not b.outline:
        return out
    # proposals chapter
    pend = [x for x in b.sections.values() if x["stage"] == "proposals"]
    if pend and agent["home"] not in b.proposals and not agent["memory"].get("proposals asked"):
        agent["memory"]["proposals asked"] = s.turn
        body = draft_proposals(s, agent)
        if body:
            b.proposals[agent["home"]] = body
            out.append(au.act(agent, name, "report", s, {"about": "proposals"}, _body={"note": f"{agent['id']} put its community's proposals before the council"}))
        else:
            agent["memory"]["proposals asked"] = None
        return out
    for pid in ("adopt-proposals", "adopt-book"):
        q = prop(lg, pid)
        if q and q["status"] == "open" and not voted(q, agent["id"]):
            what = {"adopt-proposals": {m: (v.get("text", "")[:1500]) for m, v in b.proposals.items()},
                    "adopt-book": {"counts": b.counts(), "audit": len(b.audits), "chapters": [c["title"] for c in b.outline["chapters"]]}}[pid]
            out += vote_on(s, agent, lg, [q], pid, {pid: what})
            return out
    out += council(s, agent, lg)
    return out


def vote_on(s, agent, lg, open_props: list, what: str, shown: dict) -> list:
    if au.dry(s, agent):
        return [au.act(agent, lg.name, "vote", s, {"id": p["id"], "value": "yes"}) for p in open_props]
    a = ask(s, agent, f"vote on {what}", COUNCIL_SYSTEM, {"your community": agent["home"], "its mission": s.communities[agent["home"]].mission,
                                                          "before the council": shown}, VOTES_SCHEMA, 2500)
    if a is None:
        return []
    ids = {p["id"] for p in open_props}
    return [au.act(agent, lg.name, "vote", s, {"id": v["id"], "value": v.get("vote", "no")}, _by=str(v.get("why", ""))[:200]) for v in a.get("votes", []) if v.get("id") in ids]


def record_situation(s: Society, agent: dict) -> dict:
    """What an envoy reads of the book's making: stages, adoption under each regime, versions, the audit's findings by regime, the
    reviewers' load by community, rounds, petitions."""
    b = s.book
    by_regime: dict[str, dict] = {}
    for x in b.sections.values():
        for v in b.versions_of(x["id"]):
            rs = b.reviews_of(v["id"])
            if not rs:
                continue
            g = by_regime.setdefault(rs[0].get("regime") or "?", {"versions reviewed": 0, "adopted": 0, "audited wrong": 0, "reviews": 0})
            g["versions reviewed"] += 1
            g["reviews"] += len(rs)
            g["adopted"] += int(x.get("adopted") == v["id"])
    for au_ in b.audits:
        v = b.versions.get(au_["version"])
        rs = b.reviews_of(au_["version"]) if v else []
        if rs:
            by_regime.setdefault(rs[0].get("regime") or "?", {}).setdefault("audited wrong", 0)
            by_regime[rs[0].get("regime") or "?"]["audited wrong"] += len(au_["wrong"])
    load: dict[str, int] = {}
    for r in b.reviews.values():
        load[r.get("community") or "?"] = load.get(r.get("community") or "?", 0) + 1
    return {"round": s.turn, "stages": b.counts(), "regime in force": regime(s), "adopt section": rule_in_force(s, "adopt section"),
            "by regime": by_regime, "reviews by community": load, "open petitions of your community": [
                {"id": pid, "name": p["name"], "need": p["need"], "by": p["by"]} for pid, p in b.petitions.items() if p["community"] == agent["home"] and p["status"] == "petitioned"],
            "tools admitted": list(b.tools), "recent milestones": [e["text"] for e in b.events[-8:]],
            "notes in the council": [e["text"] for e in s.events if e["community"] == league(s).name and e["kind"] == "note"][-6:]}


def council(s: Society, agent: dict, lg) -> list:
    """The envoy's own initiative in the council, every few rounds when the record changed or when a proposal awaits its vote."""
    from .agents import changed_since_reflection
    b, name, out = s.book, lg.name, []
    open_props = [p for p in props(lg, None) if p["decision"] in ("amend operational", "admit tool") and not voted(p, agent["id"])]
    # the proposer enacts a change of rules once it passes
    for p in props(lg, "amend operational"):
        if proposed_fields(lg, p["id"]).get("author") == agent["id"] and carried(s, p, len(envoys(s))):
            out.append(au.act(agent, name, "amend", s, {"id": p["id"]}))
    due = s.turn - agent["memory"].get("last_reflect", 0) >= agent.get("reflect_every", 4) and changed_since_reflection(s, agent)
    if not (due or open_props) or au.dry(s, agent):
        if au.dry(s, agent):
            out += [au.act(agent, name, "vote", s, {"id": p["id"], "value": "yes"}) for p in open_props]
        return out
    agent["memory"]["last_reflect"] = s.turn
    options = {}
    rg = regime(s)
    pending = [p for p in props(lg, "amend operational")]
    if not any(proposed_fields(lg, p["id"]).get("author") == agent["id"] for p in pending):
        for r_ in bk.REGIMES:
            if r_ != rg:
                options[f"regime {r_}"] = f"propose that {bk.REGIMES[r_]}"
        cur = rule_in_force(s, "adopt section")
        for rl in bk.RULES:  # the unit stays origin: under the others some reviewers' ballots would not be counted at all
            if rl != cur.get("rule"):
                options[f"rule {rl}"] = f"propose that a section be adopted by {rl} of its reviewers' origins"
    for pid, pt in b.petitions.items():
        if pt["community"] == agent["home"] and pt["status"] == "petitioned":
            options[f"carry {pid}"] = f"carry to the council the petition for the tool {pt['name']}: {pt['need'][:200]}"
            options[f"decline {pid}"] = f"decline to carry the petition {pid}"
    options["note"], options["rest"] = "say a note to the council", "do nothing this round"
    shown = {p["id"]: proposed_fields(lg, p["id"]) for p in open_props}
    a = ask(s, agent, "council", COUNCIL_SYSTEM, {"your community": agent["home"], "its mission": s.communities[agent["home"]].mission,
                                                  "record": record_situation(s, agent), "open proposals": shown, "options": options}, COUNCIL_SCHEMA, 3000)
    if a is None:
        return out
    chosen = [o for o in a.get("choose", []) if o in options][:2]
    agent["memory"]["last_choice"] = {"turn": s.turn, "chose": chosen, "why": str(a.get("why", ""))[:300]}
    k = 1 + sum(1 for x in lg.record if x["kind"] == "propose")
    for o in chosen:
        pid = f"p{k}-{agent['home']}"
        if o.startswith("regime "):
            word = o[len("regime "):]
            out.append(au.act(agent, name, "propose", s, {"id": pid, "level": "operational", "change.compact": f"evaluation: {word} (proposal {pid})"},
                              _body={"note": f"{agent['id']} proposes that {bk.REGIMES[word]}: {str(a.get('why', ''))[:200]}"}))
            break
        if o.startswith("rule "):
            rl = o[len("rule "):]
            out.append(au.act(agent, name, "propose", s, {"id": pid, "level": "operational", "change.decision": "adopt section", "change.rule": rl},
                              _body={"note": f"{agent['id']} proposes that a section be adopted by {rl} of its reviewers' origins: {str(a.get('why', ''))[:200]}"}))
            break
        if o.startswith("carry "):
            pt = b.petitions[o[len("carry "):]]
            pt["status"], pt["carried"] = "before the council", s.turn
            pt["history"].append({"turn": s.turn, "what": f"carried by {agent['id']}"})
            out.append(au.act(agent, name, "propose", s, {"id": f"tool-{pt['name']}", "decision": "admit tool", "tool": pt["name"]},
                              _body={"note": f"{agent['id']} carries the petition for the tool {pt['name']}: {pt['need'][:200]}"}))
            break
        if o.startswith("decline "):
            pt = b.petitions[o[len("decline "):]]
            pt["status"] = "declined by the envoy"
            pt["history"].append({"turn": s.turn, "what": f"declined by {agent['id']}: {str(a.get('why', ''))[:200]}"})
    ids = {p["id"] for p in open_props}
    for v in a.get("votes", []):
        if v.get("id") in ids:
            out.append(au.act(agent, name, "vote", s, {"id": v["id"], "value": v.get("vote", "no")}, _by=str(v.get("why", ""))[:200]))
    note = str(a.get("note", "")).strip()
    if note:
        out.append(au.act(agent, name, "say", s, {}, body=note[:400], _body={"note": f"{agent['id']}: {note}"}))
    return out


def draft_proposals(s: Society, agent: dict) -> dict | None:
    b = s.book
    findings, facts = [], {}
    for x in b.sections.values():
        if x.get("adopted") and x["adopted"] in b.versions:
            v = b.versions[x["adopted"]]
            findings.append({"section": x["id"], "title": x["title"], "proposals": v.get("proposals", [])[:4], "claims": v["claims"][:4]})
            for c in v["claims"][:4]:
                if c.get("fact") in b.facts_of(x["id"]):
                    facts[c["fact"]] = b.facts_of(x["id"])[c["fact"]]
    if au.dry(s, agent):
        return {"text": f"### Proposals of the {agent['home']}\n\n" + "Proposals follow from the findings. " * 500, "claims": [], "proposals": [], "gaps": "", "facts": facts}
    a = ask(s, agent, "proposals", PROPOSALS_SYSTEM, {"your community": agent["home"], "its mission": s.communities[agent["home"]].mission,
                                                      "words asked": 2400, "findings": findings}, au.WRITE_SCHEMA, 20000)
    if a is None:
        return None
    return {"text": str(a.get("text", "")), "claims": a.get("claims", []), "proposals": a.get("proposals", []), "gaps": a.get("gaps", ""), "facts": facts,
            "by": agent["id"], "turn": s.turn}


# ---- the auditor -------------------------------------------------------------------------------------------------------------------
def audit(s: Society) -> None:
    """At a round's end the auditor (the secretariat's, or the chair's) checks one claim in three, at least two, of each version
    adopted this round against the facts it cites; a version with a wrong number is reopened."""
    lg = league(s)
    if lg is None:
        return
    auditor = next((a for a in s.agents.values() if a["role"] == "auditor" and in_office(s, a)), None)
    if auditor is None:
        return
    b = s.book
    acts = []
    for x in list(b.sections.values()):
        if x["stage"] != "adopted" or not x.get("adopted") or x.get("audited") == x["adopted"] or x["adopted"] not in b.versions:
            continue
        v = b.versions[x["adopted"]]
        x["audited"] = x["adopted"]
        claims = v.get("claims", [])
        facts = b.facts_of(x["id"]) if x["chapter"] and b.chapter(x["chapter"])["kind"] != "overview" else x.get("overview facts", {})
        if not claims:
            continue
        step = max(1, min(bk.AUDIT_SHARE, len(claims) // 2))
        picked = sorted(range(len(claims)), key=lambda i: bk.key(x["adopted"] + str(i)))[:max(2, len(claims) // step)]
        wrong = []
        for i in picked:
            ok, why = bk.claim_holds(claims[i], facts)
            if not ok:
                wrong.append({"claim": claims[i], "why": why})
        b.audits.append({"turn": s.turn, "version": x["adopted"], "checked": len(picked), "wrong": wrong, "by": auditor["id"]})
        acts.append(au.act(auditor, lg.name, "attest", s, {"id": x["adopted"], "finding": f"{len(wrong)} of {len(picked)} numbers checked disagree with their facts"}))
        if wrong and len(b.versions_of(x["id"])) >= au.MOST_VERSIONS:
            x["errata"] = wrong
            b.event(s.turn, f"{x['id']} keeps its adoption after {au.MOST_VERSIONS} versions; the audit's {len(wrong)} findings are printed as errata", "audit")
        elif wrong:
            x["stage"], x["reopened"], x["adopted"] = "revision", s.turn, None
            b.event(s.turn, f"{x['id']} reopened: the audit found {len(wrong)} of {len(picked)} numbers wrong", "audit")
    if acts:
        s.submit(lg.name, [a for _, a in acts])


# ---- the round's bookkeeping ---------------------------------------------------------------------------------------------------------
def on_admitted(s: Society, community: str, act: dict, raw: dict) -> None:
    """Bodies the book keeps from admitted acts: reviews, and petitions carried in a delivery."""
    body = raw.get("_body")
    if not isinstance(body, dict):
        return
    if act["kind"] == "assess" and body.get("kind") == "review":
        rid = f"r-{body['version']}-{act['author']}"
        s.book.reviews[rid] = body
    if act["kind"] == "deliver" and body.get("petition") and isinstance(body["petition"], dict) and body["petition"].get("name"):
        p = body["petition"]
        nm = bk.slug(p["name"])
        if nm in bench.TOOLS:
            nm = f"{nm}-variant"
        if nm and nm not in s.book.tools and not any(x["name"] == nm for x in s.book.petitions.values()):
            home = s.agents.get(act["author"], {}).get("home", community)
            pid = f"pet-{len(s.book.petitions) + 1}"
            data = p.get("kind") == "data"
            s.book.petitions[pid] = {"by": act["author"], "origin": body.get("origin"), "community": home, "name": nm, "need": str(p.get("need", ""))[:600],
                                     "kind": p.get("kind", "tool"), "status": "a gap of the shelf" if data else "petitioned", "petitioned": s.turn,
                                     "history": [{"turn": s.turn, "what": f"petitioned by {act['author']} in {community}" + (" for data the shelf lacks: recorded as a gap" if data else "")}]}
            s.book.event(s.turn, f"{act['author']} petitioned for a tool: {nm}", "petition")


def bookkeeping(s: Society) -> None:
    founding(s)
    lg = league(s)
    b = s.book
    if lg is not None and b.outline:
        k = b.call.get("pilot sections") or 0
        body = sorted([x for x in b.sections.values() if b.chapter(x["chapter"])["kind"] not in ("overview", "proposals")], key=lambda x: x["id"])
        for i, x in enumerate(body):  # a pilot works only the first k sections of the body; the rest are held until it is lifted
            if k and i >= k and x["stage"] == "data" and not x.get("series"):
                x["stage"] = "held"
            if not k and x["stage"] == "held":
                x["stage"] = "data"
            if x["stage"] in ("held", "data", "analysis"):  # the words asked follow the declared density until the section is drafted
                x["words"] = min(bk.MOST_WORDS, max(500, int(x.get("pages") or 3) * bk.WORDS_PER_PAGE))
    if lg is not None:
        settle(s)
        petitions(s, lg)
        audit(s)
    m = b.counts()
    m["turn"] = s.turn
    m["regime"] = regime(s) if lg else ""
    m["rule"] = rule_in_force(s, "adopt section")
    m["calls"] = sum(a["tally"].get("calls", 0) for a in s.agents.values())
    m["usd"] = round(sum(a["tally"].get("usd", 0.0) for a in s.agents.values()), 4)
    b.measures[str(s.turn)] = m


def settle(s: Society) -> None:
    """Sections move on when their community's standard is met: a choice and its check (data), a plan and its replication (analysis),
    an accepted version (draft)."""
    b = s.book
    comm = {c.preset: c for c in s.communities.values()}
    for x in list(b.sections.values()):
        sid = x["id"]
        if x["stage"] == "data" and "commons" in comm:
            item = au.data_item(x)
            got = au.delivered(s, comm["commons"], item)
            if len(got) >= 2:
                first, check = got[0], got[1]
                drop = {v["code"] for v in check.get("verdicts", []) if v.get("verdict") == "drop"}
                codes = [c["code"] for c in first["series"] if c["code"] not in drop] + [a_["code"] for a_ in check.get("add", [])]
                codes = list(dict.fromkeys(codes))[:6]
                s.work.bodies[f"closed:{item}"] = True
                if codes:
                    x["series"], x["stage"] = codes, "analysis"
                else:  # every series dropped: the commons chooses again
                    x["data round"] = x.get("data round", 1) + 1
        elif x["stage"] == "analysis" and "guild" in comm:
            item = au.analysis_item(x)
            got = au.delivered(s, comm["guild"], item)
            if len(got) >= 2:
                plan, rep = got[0], got[1]
                unsound = {j.get("step") for j in rep.get("judgements", []) if j.get("verdict") == "unsound"}
                kept = 0
                for i, r in enumerate(plan.get("results", [])):
                    same = rep.get("same", [])
                    if "refused" in r or i in unsound or not (i < len(same) and same[i]):
                        continue
                    b.results[f"{sid}.r{len([k for k in b.results if k.startswith(sid + '.r')]) + 1}"] = dict(r, section=sid, by=plan["by"], origin=plan["origin"],
                                                                                                          replicated_by=rep["by"], kept=True,
                                                                                                          **{"replicator origin": rep["origin"]})
                    kept += 1
                for r in rep.get("added", []):
                    if "refused" not in r:
                        b.results[f"{sid}.r{len([k for k in b.results if k.startswith(sid + '.r')]) + 1}"] = dict(r, section=sid, by=rep["by"], origin=rep["origin"], kept=True)
                        kept += 1
                s.work.bodies[f"closed:{item}"] = True
                if kept:
                    x["stage"] = "draft"
                else:
                    x["analysis round"] = x.get("analysis round", 1) + 1
        elif x["stage"] in ("draft", "revision", "corrections") and "cooperative" in comm and x.get("writer") != "editor":
            n = len(b.versions_of(sid)) + 1
            got = au.delivered(s, comm["cooperative"], f"w-{sid}-{n}")
            if got:
                body = got[0]
                correcting = x["stage"] == "corrections"
                record_version(s, x, body, s.agents.get(body["by"], {"id": body["by"], "origin": body["origin"], "policy": "rule"}))
                b.versions[f"w-{sid}-{n}"]["model"] = body.get("model")
                s.work.bodies[f"closed:w-{sid}-{n}"] = True
                if correcting:  # the corrections an adopting review asked for; adopted without another review, read by the audit
                    x["stage"], x["adopted"], x["corrected"] = "adopted", f"w-{sid}-{n}", True
                    b.event(s.turn, f"{sid} corrected and adopted (w-{sid}-{n})", "adopted")


def petitions(s: Society, lg) -> None:
    """A petition the council admitted goes to the guild to be built; a tool built and found sound by a checker of another origin is
    admitted to the bench."""
    b = s.book
    for pid, pt in b.petitions.items():
        if pt["status"] == "before the council":
            p = prop(lg, f"tool-{pt['name']}")
            if p is None and s.turn - pt.get("carried", s.turn) >= 3:
                pt["status"] = "petitioned"
                pt["history"].append({"turn": s.turn, "what": "the carrying did not reach the council's record; back with the envoy"})
                continue
            if carried(s, p, len(envoys(s))):
                pt["status"] = "to build"
                pt["history"].append({"turn": s.turn, "what": "admitted by the council; to be built by the guild"})
            elif lost(s, p, len(envoys(s))):
                pt["status"] = "refused by the council"
                pt["history"].append({"turn": s.turn, "what": "refused by the council"})
        elif pt["status"] == "to build":
            guild = next((c for c in s.communities.values() if c.preset == "guild"), None)
            got = au.delivered(s, guild, f"t-{pt['name']}") if guild else []
            if len(got) >= 2:
                tool, check = got[0], got[1]
                s.work.bodies[f"closed:t-{pt['name']}"] = True
                if check.get("verdict") == "sound":
                    b.tools[pt["name"]] = {k: tool.get(k) for k in ("name", "code", "about", "params", "series params")} | {"by": tool["by"], "checked by": check["by"], "admitted": s.turn}
                    pt["status"] = "admitted"
                    b.event(s.turn, f"the tool {pt['name']} was admitted to the bench", "tool")
                else:
                    pt["status"] = "found unsound"
                pt["history"].append({"turn": s.turn, "what": f"checked by {check['by']}: {check.get('verdict')} ({check.get('why', '')[:160]})"})
