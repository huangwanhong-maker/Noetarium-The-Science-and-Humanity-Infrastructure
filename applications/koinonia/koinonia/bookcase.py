"""The case of a book written by communities (the design record of 2026-11-26): four communities of different orders, no organisation
above them, a global analysis of the Sustainable Development Goals with proposals to write. Every person, organisation and agent is
invented, and their names say so (plants, trees and workshops); the data are the World Bank's.

- the **commons** of data (open; stewards accept by any one): gatherers lent by many origins choose and check series;
- the **guild** of analysts (entered by assessment): analysts plan, run and replicate analyses on the bench;
- the **cooperative** of writers (one vote an origin): writers draft and revise;
- the **forum** of the affected (a polity of persons): it convenes the conference, and its delegates' agents review as those the goals
  concern.

Each community sends an envoy to the conference the forum convenes, and two voices of members of its deciding body ratify (or refuse) a
charter at home. Reviewers are lent by members of each community, to sit in whatever organisation is founded.
"""

from __future__ import annotations

from . import agents
from .book import Book
from .society import Society

SONNET, HAIKU, OPUS = "claude-sonnet-5-5", "claude-haiku-5-5", "claude-opus-5-5"
TASK = ("To write a global analysis of the Sustainable Development Goals with proposals, as a book of about 560 two-column pages (some "
        "470 of prose and the rest a statistical annex and an account of how the book was made), from the World Bank's World Development "
        "Indicators, with base year 2015 and target year 2030; every number traceable to its data, every analysis replicated, every section "
        "reviewed.")


def setup(s: Society, dry: bool = False) -> Society:
    from .cases import declare_agreement
    declare_agreement(s)
    s.description = ("Four communities of different orders, invented, with no organisation above them, set to write together a global "
                     "analysis of the Sustainable Development Goals with proposals, a book of more than five hundred two-column pages.")
    s.dry, s.parallel = dry, 16
    s.book = Book()
    s.book.call = {"task": TASK, "base year": 2015, "target year": 2030, "convened": ["commons", "guild", "cooperative", "forum"], "convened at": 0}
    persons = ("aster", "birch", "fern", "elm", "dahlia", "rowan", "amara", "bao", "chidi", "dagny", "ewan", "farah", "goran", "hana", "idris", "juno")
    for p in persons:
        s.party(p, "person", label=p.capitalize())
    orgs = ("orchard-lab", "loom-collective", "quarry-works", "willow-works", "cedar-works", "the-secretariat", "fernhill-institute", "saltmarsh-lab",
            "juniper-works", "heath-collective", "tamarind-press", "birchwood-analytics")
    for o in orgs:
        s.party(o, "organisation", label=o.replace("-", " ").title())
    guild_members = ("fern", "elm", "orchard-lab", "quarry-works", "willow-works", "fernhill-institute", "saltmarsh-lab", "birchwood-analytics", "goran", "hana")
    coop_members = ("dahlia", "rowan", "loom-collective", "cedar-works", "willow-works", "juniper-works", "heath-collective", "tamarind-press", "ewan", "farah")

    s.found("commons", "commons", "aster", [["aster", "steward"], ["birch", "reviewer"]],
            "To keep a shelf of data for a global analysis of the goals: for each section, series chosen from the World Bank's catalogue by one "
            "gatherer and checked by another of a different origin.", roles={"steward": "aster"})
    s.found("guild", "guild", "fern", [["fern", "steward"], ["elm", "reviewer"]] + [[m, "member"] for m in guild_members],
            "To analyse by its method: every plan of analyses run on the shared bench and replicated by an analyst of another origin before any "
            "writer may use it.", roles={"steward": "fern"})
    s.found("cooperative", "cooperative", "dahlia", [["dahlia", "board"]] + [[m, "member"] for m in coop_members],
            "To write, its members pooling their writers: each section drafted from analyses the guild replicated, and revised until it is adopted.",
            roles={"steward": "dahlia"})
    s.found("forum", "polity", "amara", [["amara", "magistrate"], ["amara", "citizen"], ["bao", "citizen"], ["chidi", "citizen"], ["dagny", "citizen"]],
            "To speak for those the goals concern: what matters and to whom, and to judge whether what is written serves them.", roles={"steward": "amara"})
    s.found("conference", "conference", "forum", [[c, "member"] for c in ("commons", "guild", "cooperative", "forum")],
            "A conference the forum convenes, at which the communities' envoys decide whether and how to organise a global analysis of the goals.",
            constitution=__import__("koinonia.partnership", fromlist=["x"]).conference_constitution(), via="setup")

    def lend(ident, sponsor, role, policy, where, model=SONNET, label="", home="", effort="low", **kw):
        return agents.lend(s, ident, sponsor, role, policy, where, provider="anthropic" if policy == "model" else "", label=label or ident,
                           model=model, effort=effort, home=home or next(iter(where)), via="setup", term=4000, **kw)

    def tag(sponsor):
        return sponsor.split("-")[0]

    # keepers of each working community (the steward's operative, by rule)
    lend("aster/keeper", "aster", "book keeper", "rule", {"commons": {"matters": ["series"]}}, label="Aster's keeper")
    lend("fern/keeper", "fern", "book keeper", "rule", {"guild": {"matters": ["analysis", "tool"]}}, label="Fern's keeper")
    lend("dahlia/keeper", "dahlia", "book keeper", "rule", {"cooperative": {"matters": ["writing"]}}, label="Dahlia's keeper")
    # gatherers: the commons is open, so anyone answerable may lend one
    for sponsor in ("orchard-lab", "loom-collective", "cedar-works", "quarry-works", "bao", "saltmarsh-lab", "juniper-works", "idris", "juno", "heath-collective"):
        lend(f"{tag(sponsor)}/gatherer", sponsor, "gatherer", "model", {"commons": {"matters": ["series"], "position": "contributor"}},
             label=f"{tag(sponsor).capitalize()}'s gatherer")
    # analysts: aides of the guild's members
    for sponsor, k in (("orchard-lab", 2), ("quarry-works", 2), ("willow-works", 2), ("fernhill-institute", 2), ("saltmarsh-lab", 2),
                       ("birchwood-analytics", 2), ("elm", 1), ("goran", 1), ("hana", 1), ("fern", 1)):
        for i in range(k):
            lend(f"{tag(sponsor)}/analyst-{i + 1}", sponsor, "analyst", "model", {"guild": {"matters": ["analysis", "tool"], "position": "aide", "sponsor_position": True}},
                 label=f"{tag(sponsor).capitalize()}'s analyst {i + 1}")
    # writers: hands of the cooperative's members; three members lend writers on a larger model
    for sponsor, k, model in (("loom-collective", 4, SONNET), ("tamarind-press", 4, OPUS), ("ewan", 2, OPUS), ("cedar-works", 4, SONNET),
                              ("willow-works", 3, SONNET), ("juniper-works", 4, SONNET), ("heath-collective", 3, SONNET), ("rowan", 2, SONNET),
                              ("dahlia", 2, SONNET), ("farah", 2, SONNET)):
        for i in range(k):
            lend(f"{tag(sponsor)}/writer-{i + 1}", sponsor, "writer", "model", {"cooperative": {"matters": ["writing"], "position": "hand", "sponsor_position": True}},
                 model=model, label=f"{tag(sponsor).capitalize()}'s writer {i + 1}")
    # reviewers, lent by members of each community, to sit in whatever organisation is founded (a mandate at home until then)
    for sponsor, home in (("orchard-lab", "guild"), ("quarry-works", "guild"), ("elm", "guild"), ("fernhill-institute", "guild"), ("saltmarsh-lab", "guild"),
                          ("birchwood-analytics", "guild"), ("goran", "guild"), ("hana", "guild"),
                          ("birch", "commons"), ("idris", "commons"), ("juno", "commons"),
                          ("rowan", "cooperative"), ("ewan", "cooperative"), ("farah", "cooperative"),
                          ("bao", "forum"), ("chidi", "forum"), ("dagny", "forum"), ("amara", "forum")):
        lend(f"{tag(sponsor)}/reviewer", sponsor, "reviewer", "model", {home: {}}, label=f"{tag(sponsor).capitalize()}'s reviewer", home=home)
    # envoys of the four communities at the conference
    for c in ("commons", "guild", "cooperative", "forum"):
        lend(f"{c}/envoy", c, "envoy", "model", {"conference": {"position": "envoy"}}, model=OPUS, label=f"The {c}'s envoy", home=c, reflect_every=8)
    # voices of members of each community's deciding body
    for sponsor, home, first in (("aster", "commons", True), ("birch", "commons", False), ("fern", "guild", True), ("orchard-lab", "guild", False),
                                 ("dahlia", "cooperative", True), ("loom-collective", "cooperative", False), ("amara", "forum", True), ("bao", "forum", False)):
        a = lend(f"{tag(sponsor)}/voice", sponsor, "voice", "model", {home: {}}, model=HAIKU, label=f"{tag(sponsor).capitalize()}'s voice", home=home)
        a["first voice"] = first
    s.event("conference", "The forum convenes a conference of the four communities on a global analysis of the goals: whether and how to organise it", kind="clock")
    s.save()
    return s


def offices(s: Society, coordination: str, members: list[str]) -> None:
    """The organisation's offices, lent when it is founded: by the secretariat (a standing secretariat) or by each member (a chair that
    rotates), each office acting while its sponsor holds the chair."""
    lg = "partnership"
    sponsors = ["the-secretariat"] if coordination == "standing secretariat" else members
    for sp in sponsors:
        tag = sp.split("-")[-1] if sp == "the-secretariat" else sp
        agents.lend(s, f"{tag}/secretary", sp, "secretary", "rule", {lg: {}}, label=f"The {tag}'s secretary", home=sp if sp in s.communities else lg, via="setup", term=2000)
        agents.lend(s, f"{tag}/auditor", sp, "auditor", "rule", {lg: {}}, label=f"The {tag}'s auditor", home=sp if sp in s.communities else lg, via="setup", term=2000)
        agents.lend(s, f"{tag}/editor", sp, "editor", "model", {lg: {"position": "reviewer", "sponsor_position": True}}, provider="anthropic", model=OPUS, effort="low",
                    label=f"The {tag}'s editor", home=sp if sp in s.communities else lg, via="setup", term=2000)
