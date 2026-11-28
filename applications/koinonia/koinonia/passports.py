"""Households and the passports they issue at a node (the design of 2026-11-27, Sections 5.3, 5.4 and 8.3; thesmos's decision 0007).

A person's household is a community on the preset ``household``: the person its principal, the operatives they answer for declared at
its founding and assigned as its agents. The person acts in it with a credential minted at this computer (``principal_credential``),
shown once; the node keeps only its digest. To send an operative to other communities the person issues a passport: the node mints a
bearer credential, the household enters the mandate with the credential's digest (an act of the principal constituted by the request's
episode), the node's notary witnesses the household's new head, and the passport (``noetarium.passport/1``) is assembled from the
household's record with every link's proof. The credential is shown once and kept nowhere; whoever holds it acts under the mandate.

*Caveat.* Only a founding and a presentation declare parties (thesmos's decision 0009): an operative the household did not declare at its
founding cannot be given a passport there until the ground has an act that declares one.
"""

from __future__ import annotations

import hashlib
import secrets

from science import communities as api

from .behaviour import episode

KINDS = ["present", "apply", "commit", "deliver", "hand over", "assess", "attest", "question", "petition", "claim", "consult", "use", "deposit",
         "leave", "say", "report", "vote"]


def found_household(node, person: str, operatives: list[dict], name: str = "") -> str:
    """Founds a person's household at the node, declaring the person and their operatives ({id, provider}) and assigning each as an
    agent of the household; answers its name."""
    name = name or f"{person}-household"
    parties = [{"id": person, "kind": "person"}] + [{"id": o["id"], "kind": "operative", "controller": person, "provider": o.get("provider", "")} for o in operatives]
    node.found(name, "household", person, [[person, "principal"]], parties)
    admits = []
    for o in operatives:
        a = node.act(person, "admit", {"position": "agent", "whom": o["id"]})
        a["conduct"] = episode(person, "act", route="admit", address="local")
        admits.append(a)
    if admits:
        r = node.step(name, admits)
        bad = [o["reason"] for o in r["outcomes"] if not o.get("admitted")]
        if bad:
            raise ValueError(f"the household did not assign its agents: {bad[0]}")
    return name


def principal_credential(node, home: str, person: str) -> str:
    """A credential with which a person acts in their household from this computer, shown once; only its digest is kept."""
    st = node.state(home)
    if not any(p["party"] == person and p["position"] == "principal" for p in st.get("positions", [])):
        raise ValueError(f"{person} is not a principal of {home}")
    token = secrets.token_urlsafe(32)
    node.principals[hashlib.sha256(token.encode("utf-8")).hexdigest()] = {"home": home, "party": person}
    node.save()
    return token


def issue(node, home: str, principal: str, body: dict, address: str = "local") -> dict:
    """Issues a passport at a household for one of its operatives: {"operative", "provider", "model", "kinds", "matters", "communities",
    "allowance", "slice", "term": {"since", "until", "from", "to"}, "parent"}. Answers {passport, credential, mandate, head}: the
    credential shown once. Raises ValueError with the household's reason when it refuses."""
    st = node.state(home)
    operative = str(body.get("operative", ""))
    declared = {p["id"]: p for p in st.get("declared", [])}
    if operative not in declared or declared[operative]["kind"] != "operative":
        raise ValueError(f"{operative or 'no operative'} is not an operative this household declared")
    provider = declared[operative].get("provider", "")
    if body.get("provider") and body["provider"] != provider:
        raise ValueError(f"the household declared {operative}'s provider as {provider}, not {body['provider']}")
    communities = [str(c) for c in body.get("communities") or []]
    if not communities:
        raise ValueError("a passport names the communities in which it may be used")
    term = body.get("term") or {}
    n = 1 + sum(1 for m in st.get("mandates", []) if m["holder"] == operative)
    token, digest = api.credential()
    grant = {"id": str(body.get("id") or f"m-{operative.replace('/', '-')}-{n}"), "principal": principal, "holder": operative,
             "kinds": list(body.get("kinds") or KINDS), "matters": list(body.get("matters") or []), "allowance": dict(body.get("allowance") or {}),
             "slice": dict(body.get("slice") or {}), "from": int(term.get("from", 0)), "to": int(term.get("to", 100000)), "communities": communities,
             "credential": digest, **({"since": term["since"]} if term.get("since") else {}), **({"until": term["until"]} if term.get("until") else {})}
    if body.get("parent"):
        grant.update(parent=str(body["parent"]), reserves=True)
    act = node.act(principal, "mandate", grant=grant)
    act["conduct"] = episode(principal, "issue passport", route="passports", method="POST", address=address, provider=provider, model=str(body.get("model", "")),
                             items=[grant["id"]])
    o = node.step(home, [act])["outcomes"][0]
    if not o.get("admitted"):
        raise ValueError(f"the household refused the mandate: {o['reason']}")
    p = node.session(home).ask("passport.issue", mandate=grant["id"], home=home, at=node.now(), model=str(body.get("model", "")),
                               label=str(body.get("label", operative)), node=node.url)
    if not p["admitted"]:
        raise ValueError(f"no passport for {grant['id']}: {p['reason']}")
    passport = p["passport"]
    if node.notary:
        passport["issuer"]["witnesses"] = [node.witness({"community": home, "size": p["head"]["size"], "root": p["head"]["root"]})]
    return {"passport": passport, "credential": token, "mandate": grant["id"], "head": p["head"]}


def revoke(node, home: str, principal: str, mandate: str, address: str = "local") -> dict:
    """The principal revokes a mandate at the household; hosts learn it at their next check of its status."""
    act = node.act(principal, "revoke", {"which": mandate})
    act["conduct"] = episode(principal, "act", route="revoke", address=address, items=[mandate])
    return node.step(home, [act])["outcomes"][0]
