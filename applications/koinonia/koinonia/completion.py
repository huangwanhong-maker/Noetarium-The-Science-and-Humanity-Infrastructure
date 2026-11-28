"""The completion re-founded at a node (the design of 2026-11-27, Sections 7.9.10, 11.2 and 15; ADR 0176).

The book *Seventeen Goals, Fifteen Years* was left unfinished when its run ended, and a first attempt to complete it worked through a
tool beside the communities, every question of its government going to the person. Here the completion is founded as a community at
Koinonia's node, its posted work open to agents of any provider through the gateway under its own rules. The sequence, after Section
11.2:

1. the author's household, on the preset ``household``, declaring the operatives the author may send (only a founding declares them);
2. the platform league, on its preset, as a plain community holding its registry, the node's notary and the author its registrar until
   Phase 11 founds its offices (its docket, releases, conformance holding and maintainer's office are Phase 11's);
3. the completion, on Section 7.9.10's constitution with the steward's norms of deposit as archivist (``constitution``), the author its
   principal and, for the office of status, its steward at the founding;
4. recognition of the platform league at the level of evidence;
5. the steward named by the author (Claude Code, which carries the first attempt) under a passport of the household, presented through
   the gateway and admitted to the steward's office by the principal;
6. the holdings at the founding by digest (the plan, the section standard, the society's sealed record);
7. after the carrying (``carry_completion.py``), the completion's listing in the league's registry, checked by another origin and
   verified as a reader would, and the steward's briefing.

The person is named only as "the author". Every act is constituted by an episode recorded in the community's series; every verdict is
the fold's. A credential minted here lives only in the process that minted it.
"""

from __future__ import annotations

import copy
import datetime as dt
import hashlib
import inspect
import json
import re
from pathlib import Path

from science import communities as api

from . import passports, registry
from .behaviour import episode
from .book_roles import STEWARD, STEWARD_KINDS

COMMUNITY = "completion"
LEAGUE = "platform-league"
HOUSEHOLD = "author-household"
AUTHOR = "author"
OPERATIVES = [STEWARD, {"id": "author/claude-review", "provider": "anthropic"}, {"id": "author/codex-steward", "provider": "openai"},
              {"id": "author/codex-a", "provider": "openai"}, {"id": "author/codex-b", "provider": "openai"}, {"id": "author/codex-c", "provider": "openai"}]
# Section 7.9.10 gives the steward no norm to deposit what it carries, and no amendment of the ground changes norms of access: the
# founding constitution declares them.
CARRYING_NORMS = [{"id": f"{h} kept by the steward", "who": "steward", "may": "deposit", "object": h}
                  for h in ("first attempt", "adopted", "reviews", "facts", "plan", "checks", "record of the run")]
RULE = ("A carried section is accepted when its current version holds the section standard and bears a standing verdict of adopt or "
        "minor by an agent of another provider than its writer's; every other carried section is posted again as work.")


def sha256_file(p: Path) -> str:
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def carried_content(store: Path, fields: dict) -> dict:
    """Resolve an admitted consult of a carried JSON holding from this store's first-attempt bag.

    Only the archive layout written by carry_completion is mapped. No path in an act or HTTP
    request is followed. The digest identifies the original bytes, before the runner attribution
    is rendered as "the author", as it is in the carrying record.
    """
    holding, within, sid = fields["holding"], fields.get("within"), fields.get("section", "")
    if holding == "the plan of the completion" and within == "plan":
        relative = Path("plan.json")
    elif holding == "the replay's totals" and within == "facts":
        relative = Path("replay.json")
    elif isinstance(sid, str) and re.fullmatch(r"c[0-9]+-[0-9]+", sid):
        if within == "facts" and holding == f"results {sid}":
            relative = Path("results", sid + ".json")
        elif within in ("adopted", "first attempt") and holding == f"{sid} {fields.get('version')}":
            relative = Path("sections", sid + ".json")
        elif within == "reviews" and holding.startswith(f"review {sid} "):
            relative = Path("reviews", sid + ".json")
        else:
            return {}
    else:
        return {}
    digest = fields.get("digest")
    try:
        data = (store / "communities" / COMMUNITY / "first-attempt" / "data" / relative).read_bytes()
    except OSError:
        return {"content": None, "content status": "archived file unavailable", "source sha256": digest}
    if hashlib.sha256(data).hexdigest() != digest:
        return {"content": None, "content status": "archived file differs from the deposit", "source sha256": digest}
    try:
        content = json.loads(data)
    except (ValueError, UnicodeError):
        return {"content": None, "content status": "archived file is not readable JSON", "source sha256": digest}
    # The archive keeps the original, including its private runner. Keep agent/model/provider as credited.
    records = content.values() if relative.parent == Path("results") else [content]
    for record in records:
        by = record.get("by")
        if isinstance(by, dict) and "run by" in by:
            by["run by"] = "the author"
        elif isinstance(by, str) and " / run by " in by:
            record["by"] = by.rsplit(" / run by ", 1)[0] + " / run by the author"
    return {"content": content, "source sha256": digest}


def constitution() -> dict:
    """Section 7.9.10's constitution (the preset ``completion``) with the steward's norms of deposit as archivist."""
    c = copy.deepcopy(api.preset("completion"))
    norms = c.setdefault("access", {}).setdefault("norms", [])
    have = {n.get("id") for n in norms}
    norms += [n for n in CARRYING_NORMS if n["id"] not in have]
    return c


def iso(t: float) -> str:
    return dt.datetime.fromtimestamp(t, dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


class Founding:
    """What the founding hands to the carrying: the node, its gateway, the steward's mandate and (in memory only) its credential."""

    def __init__(self, node, gateway):
        self.node, self.gateway = node, gateway
        self.steward, self.mandate, self.credential = STEWARD["id"], "", ""

    def conduct(self, party: str, kind: str, route: str = "carry", **features) -> dict:
        steward = party == self.steward
        return episode(party, "act", route=route, address="local", act=kind, provider=STEWARD["provider"] if steward else "",
                       model=STEWARD["model"] if steward else "", bearer=AUTHOR if steward else "", **features)

    def act(self, community: str, party: str, kind: str, fields: dict, route: str = "carry", **features) -> dict:
        """An act taken at this computer, the steward's under its mandate, constituted by its episode."""
        a = self.node.act(party, kind, fields, mandate=self.mandate if party == self.steward else "")
        a["conduct"] = self.conduct(party, kind, route, **features)
        return a

    def step(self, community: str, acts: list[dict], what: str = "") -> list[dict]:
        """Steps acts; raises with the fold's reason when any is refused (the carrying is all or nothing for each step)."""
        out = []
        for k in range(0, len(acts), 400):
            r = self.node.step(community, acts[k:k + 400])
            for a, o in zip(acts[k:k + 400], r["outcomes"]):
                if "observe" not in a and not o.get("admitted"):
                    raise ValueError(f"{what or a.get('kind')} refused in {community}: {o.get('reason')} ({json.dumps(a.get('fields'), ensure_ascii=False)[:200]})")
            out += r["outcomes"]
        return out

    def one(self, community: str, party: str, kind: str, fields: dict, **features) -> dict:
        return self.step(community, [self.act(community, party, kind, fields, **features)])[0]


def found(node, gateway, plan: Path | None = None, society: Path | None = None) -> Founding:
    """Steps 1 to 6 of the founding at a node that hosts none of the three communities yet."""
    f = Founding(node, gateway)
    for name in (HOUSEHOLD, LEAGUE, COMMUNITY):
        if name in node.communities:
            raise ValueError(f"the node already hosts {name}: the completion is founded once")
    passports.found_household(node, AUTHOR, [{"id": o["id"], "provider": o["provider"]} for o in OPERATIVES], name=HOUSEHOLD)
    node.found(LEAGUE, "platform-league", AUTHOR, [[COMMUNITY, "member"], [HOUSEHOLD, "member"], [AUTHOR, "registrar"]],
               [{"id": AUTHOR, "kind": "person"}, {"id": COMMUNITY, "kind": "community"}, {"id": HOUSEHOLD, "kind": "community"}])
    node.notary = LEAGUE
    node.save()
    node.found(COMMUNITY, "completion", AUTHOR, [[AUTHOR, "principal"], [AUTHOR, "steward"]], [{"id": AUTHOR, "kind": "person"}],
               constitution=constitution())
    f.one(COMMUNITY, AUTHOR, "recognise", {"of": LEAGUE, "level": "evidence"}, why="the league's notary witnesses heads (Section 11.2, 3)")
    t = node.clock()
    r = passports.issue(node, HOUSEHOLD, AUTHOR, {"operative": STEWARD["id"], "model": STEWARD["model"], "kinds": STEWARD_KINDS, "matters": ["the book"],
                                                  "communities": [COMMUNITY], "allowance": {"bench runs": 120}, "slice": {"bench runs": 10},
                                                  "term": {"since": iso(t - 86400), "until": iso(t + 365 * 86400)}, "label": "the completion's steward"})
    f.mandate, f.credential = r["mandate"], r["credential"]
    st, out, _, _ = gateway.handle("POST", f"/c/{COMMUNITY}/present", {}, {"Authorization": f"Bearer {f.credential}"}, {"passport": r["passport"]}, "127.0.0.1")
    if st != 201:
        raise ValueError(f"the steward's passport was not presented: {out}")
    f.one(COMMUNITY, AUTHOR, "admit", {"position": "steward", "whom": STEWARD["id"]}, why="the steward named by the author (Section 11.2, 2)")
    deposits = []
    if plan is not None and Path(plan).is_file():
        deposits.append(f.act(COMMUNITY, f.steward, "deposit", {"holding": "the plan of the completion", "within": "plan", "kind": "document",
                                                              "digest": sha256_file(plan), "source": "the first attempt's plan.json"}))
        node.hosted(COMMUNITY).content["plan"] = json.loads(Path(plan).read_text(encoding="utf-8"))
    from . import book
    standard = "\n".join(inspect.getsource(fn) for fn in (book.version_faults, book.claim_holds))
    deposits.append(f.act(COMMUNITY, f.steward, "deposit", {"holding": "the section standard", "within": "checks", "kind": "check",
                                                          "digest": hashlib.sha256(standard.encode("utf-8")).hexdigest(),
                                                          "source": "koinonia.book.version_faults and claim_holds, as the first attempt checked sections"}))
    if society is not None and Path(society).is_file():
        deposits.append(f.act(COMMUNITY, f.steward, "deposit", {"holding": "the society's sealed record", "within": "record of the run", "kind": "record",
                                                              "digest": sha256_file(society), "source": "the society book-models, its record as sealed at the run's end"}))
    f.step(COMMUNITY, deposits, "a deposit at the founding")
    node.save(COMMUNITY)
    return f


def list_in_registry(f: Founding, listed: str) -> dict:
    """The completion lists itself in the league's registry (a member's deposit), checked by the registrar (another origin), its head
    witnessed by the node's notary, and the listing verified as a reader would: the profile served, the witness and the notary's head."""
    node = f.node
    lst = registry.listing(node, COMMUNITY, listed)
    n = sum(1 for x in node.state(LEAGUE).get("holdings", []) if x.get("within") == "registry" and x["id"].startswith(f"{COMMUNITY} listing"))
    holding = f"{COMMUNITY} listing" + (f" {n + 1}" if n else "")
    dep = node.act(COMMUNITY, "deposit", {"holding": holding, "within": "registry", "kind": "listing", "digest": lst["digest"], "source": f"the listing of {COMMUNITY}, {listed}"})
    dep["conduct"] = episode(COMMUNITY, "act", route="carry", address="local", act="deposit", by=f"the completion's steward, {f.steward}")
    att = node.act(AUTHOR, "attest", {"holding": holding})
    att["conduct"] = episode(AUTHOR, "act", route="carry", address="local", act="attest", office="registrar")
    f.step(LEAGUE, [dep, att], "the listing")
    node.hosted(LEAGUE).content.setdefault("registry", {})[holding] = lst["written"]  # served at /c/<league>/holdings/registry?content=1
    node.save(LEAGUE)
    head = lst["listing"]["head"]
    w = node.witness({"community": COMMUNITY, "size": head["size"], "root": head["root"]})
    notary = {"community": LEAGUE, **{k: node.session(LEAGUE).head[k] for k in ("size", "root")}}
    doc = registry.profile(node, COMMUNITY)
    verified = api.registry("verify", listing=lst["written"], evidence={"profile": doc["document"], "witnesses": [w], "notaries": [notary],
                                                                       "registry": f"{LEAGUE}/registry"})
    entries = api.registry_entries(node.session(LEAGUE), "registry")
    entry = next(e for e in entries["entries"] if e["holding"] == holding)
    return {"listing": lst["listing"], "written": lst["written"], "digest": lst["digest"], "entry": entry, "verified": verified,
            "profile": f"{node.url}c/{COMMUNITY}/profile"}


def brief_steward(f: Founding) -> dict:
    """The steward's briefing, read through the gateway with its credential (the read recorded in the completion's series)."""
    st, b, _, _ = f.gateway.handle("GET", f"/c/{COMMUNITY}/briefing", {}, {"Authorization": f"Bearer {f.credential}"}, None, "127.0.0.1")
    if st != 200:
        raise ValueError(f"the steward was not briefed: {b}")
    return b
