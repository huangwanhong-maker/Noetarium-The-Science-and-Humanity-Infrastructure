"""What a node tells anyone without a credential: its description (``/.well-known/noetarium.json``), the communities it hosts, each
public profile byte for byte as the communities domain's registries write it, and listings made from them (the design of 2026-11-27,
Sections 5.17, 7.2, 7.3 and 9.2).

A listing names the profile document's SHA-256 and the community's head; a reader verifies it by fetching ``/c/<community>/profile``,
comparing the digest, and checking the head against the witness of a notary it trusts (``science.communities.registry("verify", …)``).
"""

from __future__ import annotations

import datetime as dt

from science import communities as api

from .node import RELEASE


def profile(node, community: str) -> dict:
    """The public profile document (noetarium.profile/1) as the node serves it, {document, sha256}."""
    return api.profile_document(node.session(community))


def describe(node) -> dict:
    """The node: its release, its operator's office, the communities it hosts (name, order, profile URL, head) and its notary."""
    hosted = []
    for name, h in sorted(node.communities.items()):
        head = node.session(name).head
        hosted.append({"name": name, "order": h.constitution.get("order", ""), "purpose": h.constitution.get("purpose", "")[:300],
                       "profile": f"{node.url}c/{name}/profile", "head": {"size": head["size"], "root": head["root"]}})
    return {**RELEASE, "node": node.name, "url": node.url, "operator": node.operator, "notary": node.notary, "communities": hosted,
            "guide": node.url, "prompt": "GET / with Accept: text/markdown"}


def listing(node, community: str, listed: str = "") -> dict:
    """A listing of a hosted community (noetarium.listing/1), witnessed by the node's notary when it keeps one."""
    return api.listing(node.session(community), node.url, node.notary or "", listed or dt.date.today().isoformat())


def verify(node, community: str, written: str, registry: str = "") -> dict:
    """Verifies a listing as a reader would: against the profile the node serves, the witness of the listed head by the node's notary,
    and the notary's own head. ``registry`` (``<keeper>/<holding>``) names where the listing was read, so that the community's own
    ``listed in`` can claim it. Without a notary the listing can be read only as unwitnessed."""
    import json

    doc = profile(node, community)
    where = {"registry": registry} if registry else {}
    if not node.notary:
        return api.registry("verify", listing=written, evidence={"profile": doc["document"], **where})
    head = json.loads(written)["head"]
    witness = node.witness({"community": community, "size": head["size"], "root": head["root"]})
    notary = {"community": node.notary, **{k: node.session(node.notary).head[k] for k in ("size", "root")}}
    return api.registry("verify", listing=written, evidence={"profile": doc["document"], "witnesses": [witness], "notaries": [notary], **where})
