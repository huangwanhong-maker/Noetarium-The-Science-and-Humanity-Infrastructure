"""The society's case studies (ADR 0169). Every person, organisation and agent in them is invented, and their names say so (trees,
plants and workshops); the catalogue they check is the infrastructure's own, and unchecked.

- ``two``: a commons that checks the catalogue's entries, open to any lent agent, and a guild entered by trial that settles the
  checks' disagreements; the guild's findings carried back to the commons' record.
"""

from __future__ import annotations

import json

from . import agents, missions
from .society import Society


def whose(name: str) -> str:
    """The possessive of a community's name: the commons' clerk, the guild's clerk."""
    return f"{name}'" if name.endswith("s") else f"{name}'s"


def declare_agreement(s: Society, maker: str = "the case's maker") -> dict:
    """The disclosure agreement a case declares (thesmos's decision 0010): its maker, who invented every principal of its agents,
    consents for each that the agents' model calls (their prompts and answers) be kept in the behaviour journal, for review and
    research and never as evidence of intent, so that the case runs as it was recorded and the basis of the record is stated."""
    return s.agree(f"{s.name}: model calls", principal=maker, consented_by=maker, every_agent=True,
                   basis="declared by the case: its maker's consent for the invented principals of its agents")


def stand_in(system: str, prompt: str, schema: dict) -> dict:
    """A model's stand-in for checks and dry runs: it checks by the declared rules, doubts every entry whose definition is longer
    than 110 characters (so that disagreements arise), adjudicates by the rules, and always chooses to work."""
    if "verdicts" in schema.get("properties", {}):
        out = []
        for line in prompt.splitlines():
            e = json.loads(line)
            v = missions.rule_check(e)
            if len(e["definition"]) > 110 and v["verdict"] == "holds":
                v = {"entry": e["id"], "verdict": "doubt", "why": "the definition does not settle how the index is carried"}
            out.append(v)
        return {"verdicts": out}
    if "choose" in schema.get("properties", {}):
        options = json.loads(prompt)["options"]
        return {"choose": ["work"] if "work" in options else ["rest"], "note": "", "why": "the work is open"}
    e = json.loads(prompt)["entry"]
    v = missions.rule_check(e)
    return {"verdict": v["verdict"], "why": v["why"][:200]}


def two(s: Society, size: int = 48) -> Society:
    declare_agreement(s)
    s.description = ("Two communities, invented: a commons that checks a catalogue a language model drafted, open to any lent agent, and "
                     "a guild entered by trial that settles the checks' disagreements.")
    for p in ("aster", "birch", "cedar", "fern", "elm", "dahlia"):
        s.party(p, "person", label=p.capitalize())
    for o in ("orchard-lab", "loom-collective", "quarry-works"):
        s.party(o, "organisation", label=o.replace("-", " ").title())
    s.work = missions.Work.catalogue(size=size)

    s.found("commons", "commons", "aster", [["aster", "steward"], ["birch", "reviewer"]],
            "To check the entries of the urban-science catalogue of indexes, which a language model drafted and nobody checked: each entry "
            "checked twice by agents of different origins, disagreements settled, the findings kept as proposals for the catalogue's keeper.",
            roles={"steward": "aster"})
    s.found("guild", "guild", "fern", [["fern", "steward"], ["fern", "member"], ["elm", "reviewer"], ["elm", "member"], ["dahlia", "member"], ["orchard-lab", "member"]],
            "To settle disagreements between checks by the guild's method: the entry's own text read against the declared rules, one reasoned "
            "verdict. Entered by trial: an applicant's agent must give the agreed finding on two entries whose finding is known.",
            roles={"steward": "fern"})

    cat = {"matters": ["catalogue"]}
    agents.lend(s, "aster/keeper", "aster", "keeper", "rule", {"commons": {"matters": ["catalogue", "referral"]}}, label="Aster's keeper")
    agents.lend(s, "birch/assessor", "birch", "assessor", "rule", {"commons": {}}, label="Birch's assessor")
    agents.lend(s, "loom/checker", "loom-collective", "checker", "rule", {"commons": {**cat, "position": "contributor", "allowance": {"entries": 400}}},
                label="Loom's rule checker", tempo=2)
    for ident, sponsor, label in (("orchard/checker", "orchard-lab", "Orchard's model checker"), ("cedar/checker", "cedar", "Cedar's model checker")):
        agents.lend(s, ident, sponsor, "checker", "model", {"commons": {**cat, "position": "contributor", "allowance": {"entries": 400, "tokens": 400000}}},
                    provider="anthropic", label=label, tempo=2)

    adj = {"matters": ["adjudication", "trial"]}
    agents.lend(s, "fern/keeper", "fern", "keeper", "rule", {"guild": adj}, label="Fern's keeper")
    agents.lend(s, "elm/assessor", "elm", "assessor", "rule", {"guild": {}}, label="Elm's assessor")
    agents.lend(s, "orchard/adjudicator", "orchard-lab", "adjudicator", "model", {"guild": {**adj, "position": "aide", "sponsor_position": True}},
                provider="anthropic", label="Orchard's adjudicator", tempo=1)
    s.submit("guild", [{"author": "quarry-works", "kind": "join", "turn": s.turn, "fields": {"position": "applicant"}}])
    agents.lend(s, "quarry/adjudicator", "quarry-works", "adjudicator", "model", {"guild": adj}, provider="anthropic", label="Quarry's adjudicator (on trial)", tempo=1)
    s.save()
    return s


def three(s: Society, size: int = 60, regime: str = "gift", envoys: bool = False) -> Society:
    declare_agreement(s)
    """Three communities of different orders whose wants run in a circle, and a league that declares how what passes between them is
    kept (Wicksell's triangle, made of work).

    - the commons (open; stewards accept by any one; returns by acknowledgement) has checkers by rule, and declares that its batches
      need a check by rule and a reading by the guild's method;
    - the guild (entered by assessment; standing within its method) has a reader by model, and declares that its batches need a
      reading and a check by an outside model;
    - the cooperative (members; one vote an origin; a pool and mutual credit inside) has checkers by model, and declares that its
      batches need a check by model and a check by rule.

    So the guild serves the commons, the cooperative the guild, the commons the cooperative, and none can repay in kind the one that
    serves it. The organisations that lend checkers are members where their agents serve (the cooperative's members include the
    commons' rule labs; the guild's, the cooperative's model labs). The guild also settles every community's disagreements. The
    league (the first league's charter) declares the arrangement of circulation by a compact; with ``envoys`` each member sends an
    envoy by model to its council, who may propose another arrangement and vote."""
    from . import circulation
    s.description = (f"Three communities of different orders, invented, whose wants run in a circle, joined in a league whose declared "
                     f"arrangement of circulation is at first {regime}" + (", with envoys by model in its council." if envoys else "."))
    for p in ("aster", "birch", "fern", "elm", "dahlia", "rowan"):
        s.party(p, "person", label=p.capitalize())
    for o in ("loom-collective", "cedar-works", "orchard-lab", "willow-works", "quarry-works", "ledgerwright"):
        s.party(o, "organisation", label=o.replace("-", " ").title())
    share = max(4, (size // 3) // 4 * 4)
    s.work = missions.Work.lists({"commons": share, "guild": share, "cooperative": share})
    s.work.regime = regime
    std = "origin and kind"
    s.found("commons", "commons", "aster", [["aster", "steward"], ["birch", "reviewer"]],
            "To check its share of the catalogue's entries, each batch once by rule and once read by the guild's method.",
            roles={"steward": "aster", "standard": std, "kinds": ["rule", "reading"]})
    s.found("guild", "guild", "fern",
            [["fern", "steward"], ["fern", "member"], ["elm", "reviewer"], ["elm", "member"], ["quarry-works", "member"], ["orchard-lab", "member"], ["willow-works", "member"]],
            "To read entries by its method and settle the disagreements of every community's checks; its own cases checked once by an outside model.",
            roles={"steward": "fern", "standard": std, "kinds": ["reading", "anthropic"]})
    s.found("cooperative", "cooperative", "dahlia",
            [["dahlia", "board"], ["dahlia", "member"], ["rowan", "member"], ["orchard-lab", "member"], ["willow-works", "member"], ["loom-collective", "member"], ["cedar-works", "member"]],
            "To have its members' share of the catalogue checked once by model and once by rule; its members pool the capacity of their models.",
            roles={"steward": "dahlia", "standard": std, "kinds": ["anthropic", "rule"]})
    s.found("league", "first-league", "commons", [["commons", "member"], ["cooperative", "member"], ["guild", "member"], ["ledgerwright", "clearer"], ["ledgerwright", "notary"]],
            "To keep what passes between its members by the arrangement its council declares, and to let each member keep its own order.",
            roles={})

    cat = {"matters": ["catalogue", "referral"]}
    agents.lend(s, "aster/keeper", "aster", "keeper", "rule", {"commons": cat}, label="Aster's keeper")
    agents.lend(s, "birch/assessor", "birch", "assessor", "rule", {"commons": {}}, label="Birch's assessor")
    agents.lend(s, "dahlia/keeper", "dahlia", "keeper", "rule", {"cooperative": cat}, label="Dahlia's keeper")
    agents.lend(s, "rowan/assessor", "rowan", "assessor", "rule", {"cooperative": {}}, label="Rowan's assessor")
    agents.lend(s, "fern/keeper", "fern", "keeper", "rule", {"guild": {"matters": ["adjudication", "trial", "catalogue", "referral"]}}, label="Fern's keeper")
    agents.lend(s, "elm/assessor", "elm", "assessor", "rule", {"guild": {}}, label="Elm's assessor")
    agents.lend(s, "quarry/adjudicator", "quarry-works", "adjudicator", "model", {"guild": {"matters": ["adjudication", "trial"], "position": "aide", "sponsor_position": True}},
                provider="anthropic", label="Quarry's adjudicator", home="guild")
    tokens = {"entries": 400, "tokens": 400000}
    reader = agents.lend(s, "quarry/reader", "quarry-works", "checker", "model", {
        "guild": {"matters": ["catalogue"], "position": "aide", "sponsor_position": True, "allowance": tokens},
        "commons": {"matters": ["catalogue"], "position": "contributor", "allowance": tokens}}, provider="anthropic", label="Quarry's reader", home="guild")
    reader["kind"] = "reading"
    for ident, sponsor, label in (("loom/checker", "loom-collective", "Loom's rule checker"), ("cedar/checker", "cedar-works", "Cedar's rule checker")):
        agents.lend(s, ident, sponsor, "checker", "rule", {
            "commons": {"matters": ["catalogue"], "position": "contributor", "allowance": {"entries": 400}},
            "cooperative": {"matters": ["catalogue"], "position": "hand", "sponsor_position": True, "allowance": {"entries": 400}}}, label=label, home="commons")
    for ident, sponsor, label in (("orchard/checker", "orchard-lab", "Orchard's model checker"), ("willow/checker", "willow-works", "Willow's model checker")):
        agents.lend(s, ident, sponsor, "checker", "model", {
            "cooperative": {"matters": ["catalogue"], "position": "hand", "sponsor_position": True, "allowance": tokens},
            "guild": {"matters": ["catalogue"], "position": "aide", "sponsor_position": True, "allowance": tokens}}, provider="anthropic", label=label, home="cooperative")
    for c in ("commons", "guild", "cooperative"):
        agents.lend(s, f"{c}/clerk", c, "clerk", "rule", {"league": {"matters": ["service"]}}, label=f"The {whose(c)} clerk", home=c)
        if envoys:
            agents.lend(s, f"{c}/envoy", c, "envoy", "model", {"league": {"position": "envoy", "sponsor_position": True}}, provider="anthropic",
                        label=f"The {whose(c)} envoy", home=c, reflect_every=3)
    s.event("league", f"the league keeps what passes between its members by {regime}: {circulation.ARRANGEMENTS[regime]}", kind="clock")
    s.save()
    return s


def four(s: Society, size: int = 96, regime: str = "gift", envoys: bool = False, arrival: int = 6, free_rider: str = "reacts") -> Society:
    # its agreement is declared by the case three on which it is built
    """The circle of three, a free rider among them, and a fourth community that cannot repay: the test of the design's hypothesis
    that a currency can be dispensed with when what each community can do is read from the record (the design record of 2026-11-25,
    Section 6.10).

    - The cooperative free-rides by one of two declared rules, each the same under every arrangement: `free_rider="reacts"`, its
      checkers serve abroad only when its community was refused service in one of the last three rounds before the present one;
      `free_rider="reads standing"`, they serve abroad when the league's record shows its community would be refused (lately refused,
      at the limit of a debt under credit, or in default under provision with duty).
    - The polity (persons who hold their own positions; residents open to any agent) has no checkers at all. It applies to the league
      and is admitted by its council (two thirds counted by community); its entries (what `size` leaves after the circle's share;
      48 in 12 batches at the default size) need a check by rule and a check by model, and it can never repay in kind.
    - The league's notary (ledgerwright's agent, by rule) attests at each round's end which communities' checkers put nothing forward
      while work they could do lay open elsewhere: what each community could do, read from the chronicles.
    - Work arrives over time: each community's batches are released one every `arrival` rounds.

    The arrangements to compare: gift and provision serve by need but cannot tell will not from cannot; credit compels by debt but
    caps what the polity can receive; provision with duty serves by need and withholds from the community the notary attests."""
    from . import circulation
    share = max(4, (size // 9) // 4 * 4 * 2)
    three(s, size=share * 3, regime=regime, envoys=envoys)
    s.description = (f"Four communities, invented: three whose needs run in a circle, the cooperative among them free-riding, and a polity "
                     f"with no checkers that joins the league; the league's arrangement at first {regime}" + (", with envoys by model." if envoys else "."))
    polity_n = size - share * 3
    s.work.batches += [{"id": f"poli-{k + 1:02d}", "owner": "polity", "entries": e} for k, e in enumerate(
        [[x["id"] for x in polity_entries(s, polity_n)][i:i + 4] for i in range(0, polity_n, 4)])]
    for p in ("hazel", "ivy"):
        s.party(p, "person", label=p.capitalize())
    s.found("polity", "polity", "hazel", [["hazel", "magistrate"], ["hazel", "resident"], ["hazel", "citizen"], ["ivy", "citizen"], ["ivy", "resident"], ["ivy", "juror"]],
            "To have its residents' share of the catalogue checked once by rule and once by model; it has no checkers of its own and asks the league.",
            roles={"steward": "hazel", "standard": "origin and kind", "kinds": ["rule", "anthropic"]})
    agents.lend(s, "hazel/keeper", "hazel", "keeper", "rule", {"polity": {"matters": ["catalogue", "referral"]}}, label="Hazel's keeper")
    agents.lend(s, "ivy/assessor", "ivy", "assessor", "rule", {"polity": {}}, label="Ivy's assessor")
    allowance_rule, allowance_model = {"entries": 400}, {"entries": 400, "tokens": 400000}
    for ident in ("loom/checker", "cedar/checker", "orchard/checker", "willow/checker"):
        a = s.agents[ident]
        grant = {"id": f"{ident}@polity", "principal": a["sponsor"], "holder": ident, "kinds": agents.KINDS["checker"], "matters": ["catalogue"],
                 "allowance": allowance_model if a["policy"] == "model" else allowance_rule, "from": s.turn, "to": s.turn + 200, "delegable": False}
        s.submit("polity", [{"author": a["sponsor"], "kind": "mandate", "turn": s.turn, "grant": grant},
                            {"author": ident, "kind": "join", "turn": s.turn, "mandate": grant["id"], "fields": {"position": "resident"}}])
        a["communities"]["polity"] = {"mandate": grant["id"], "position": "resident", "matters": ["catalogue"]}
        if a["home"] == "cooperative":
            a["abroad"] = "when withheld" if free_rider == "reacts" else "when it would be refused"  # the free rider's declared rule
    agents.lend(s, "polity/clerk", "polity", "clerk", "rule", {"league": {"matters": ["service"]}}, label="The polity's clerk", home="polity")
    agents.lend(s, "ledgerwright/notary", "ledgerwright", "notary", "rule", {"league": {}}, label="Ledgerwright's notary", home="league")
    if not envoys:
        for c in ("commons", "guild", "cooperative"):
            agents.lend(s, f"{c}/envoy", c, "envoy", "rule", {"league": {"position": "envoy", "sponsor_position": True}}, label=f"The {whose(c)} envoy", home=c)
    s.work.applicants = ["polity"]
    # work keeps arriving: each community's batches are released one every `arrival` rounds, so that needs last and sanctions can bind
    seen: dict[str, int] = {}
    for b in s.work.batches:
        k = seen.get(b["owner"], 0)
        b["from"], seen[b["owner"]] = k * arrival, k + 1
    s.event("league", "the polity applies to join the league; it has no checkers and cannot repay in kind", kind="membership")
    s.event("cooperative", ("the cooperative's checkers serve abroad only when the cooperative was refused service in one of the last three rounds" if free_rider == "reacts" else "the cooperative's checkers serve abroad only when the league's record shows the cooperative would be refused service"), kind="membership")
    s.save()
    return s


def polity_entries(s: Society, n: int) -> list[dict]:
    """Entries of the declared list not yet given to another community, taken in the list's order."""
    taken = {e for b in s.work.batches for e in b["entries"]}
    rest = [e for e in missions.Work.catalogue(size=len(taken) + n * 2).entries.values() if e["id"] not in taken][:n]
    for e in rest:
        s.work.entries[e["id"]] = e
    return rest


CASES = {"two": two, "three": three, "four": four}
