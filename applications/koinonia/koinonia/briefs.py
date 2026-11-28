"""The briefing an outside agent reads (the design of 2026-11-27, Section 10.2): thesmos's briefing on the community's session (decision
0006 and after) composed with what the node adds: who the party is and its mandate as last checked, the community and the release, the
API's route for each act open to it, the ground's invariants, the routes, what to do next and how to leave.

Nothing here decides anything: every verdict is the fold's, read from the session.
"""

from __future__ import annotations

from .node import RELEASE

GROUND = ["G1 every act of force is signed, admitted and sealed", "G2 standing comes only from acts and declared rules",
          "G3 every act is answered for by a person, an organisation or a community", "G4 resources are charged along the mandate's chain",
          "G5 conduct, acts and assessments are kept in separate registers", "G6 rules are data", "G7 a party leaves with its record",
          "G8′ every directive has a route", "G9 no effect but by an act or a declared rule", "G10 no norm attaches an effect to thought alone"]

ROUTE = {"present": "POST /c/{c}/present", "consult": "POST /c/{c}/holdings/{holding}/consult", "use": "POST /c/{c}/tools/{tool}/run"}


def route_of(kind: str, community: str) -> str:
    return ROUTE.get(kind, "POST /c/{c}/acts").replace("{c}", community)


def briefing(node, community: str, party: str, mandate: str = "", principal: str = "", provider: str = "", model: str = "") -> dict:
    s = node.session(community)
    b = s.briefing(party, mandate=mandate)
    m = s.mandate(party, mandate=mandate) if mandate else {}
    h = node.hosted(community)
    c = h.constitution
    st = s.state()
    head = s.head
    link = (m.get("chain") or [{}])[0] if m else {}
    return {
        "format": "noetarium.briefing/1",
        "you": {"party": party, "provider": provider, "model": model, "answers for you": principal or b.get("bearer", ""),
                "mandate": {"id": mandate, "status": m.get("status", ""), "checked": h.checked.get(mandate, ""), "kinds": link.get("kinds", []),
                            "matters": link.get("matters", []), "term ends": link.get("until", ""), "used": link.get("used", {}),
                            "remaining": link.get("remaining", {}), "slice": link.get("slice", {})} if mandate else {}},
        "community": {"name": community, "order": c.get("order", ""), "mission": b.get("mission", c.get("purpose", "")),
                      "head": {"size": head["size"], "root": head["root"]}, "turn": h.turn, "ground": RELEASE["ground"], "gateway": RELEASE["gateway"]},
        "positions": [{"position": p["position"], "since": p.get("since")} for p in st.get("positions", []) if p["party"] == party],
        "acts": [{"kind": k, "route": route_of(k, community)} for k in b.get("acts", [])],
        "holdings": [{"holding": e["holding"], "aim": e["aim"], "verdict": "permitted" if e["decision"]["permitted"] else "refused",
                      "norm": e["decision"].get("norm", ""), "reason": e["decision"].get("reason", ""), **({"reaches": e["reaches"]} if e.get("reaches") else {})}
                     for e in b.get("entries", [])],
        "duties": b.get("duties", []),
        "work": b.get("work", []),
        "law": {"ground": GROUND, "compacts": st.get("compacts", []), "rulings": [r for r in st.get("rulings", []) if r.get("in force", True)]},
        "routes": {**(c.get("routes") or {}), "claims": c.get("claims", {})},
        "next": b.get("next", []),
        "behaviour": {**(b.get("behaviour") or {}), "recorded": "every request to this node is recorded as an episode of conduct in the community's series; "
                      "an act carries the digest of the episode that constituted it", "kept": "acts of force permanently; constitutive episodes ten years; "
                      "readings of public content one year; holds for open claims"},
        "leaving": {"how": f"POST /c/{community}/acts {{\"kind\": \"leave\", \"fields\": {{\"position\": \"all\"}}}}",
                    "owed": [x for x in st.get("commitments", []) if x.get("debtor") == party and x.get("state") in ("active", "presented")],
                    "record": f"GET /c/{community}/record/mine"},
        "thesmos": {k: v for k, v in b.items() if k not in ("mission", "entries", "acts", "duties", "work", "next", "behaviour")},
    }
