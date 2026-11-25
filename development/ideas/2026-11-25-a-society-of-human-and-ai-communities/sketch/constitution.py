"""Admission of a constitution: the nine conditions of form of the design (Section 3.2).

A constitution is a document in the provisional format `noetarium.constitution/1` (see ../presets).
`admission` returns the faults found, each with the number of the condition it breaks. An empty
list means that the constitution is admitted. Admission is a matter of form: nothing here judges
whether an order is open or closed, equal or directed.
"""
from __future__ import annotations

KINDS_OF_PARTY = {"person", "organisation", "operative", "community"}
UNITS_OF_COUNT = {"person", "origin", "operative", "community"}
FORMS = {"gift", "common provision", "reciprocal commitment", "mutual credit", "general claim", "outside money"}
LEVELS = ("operational", "collective choice", "constitutional")


def _list(value) -> list:
    return value if isinstance(value, list) else [value]


def admission(c: dict) -> list[tuple[int, str]]:
    faults: list[tuple[int, str]] = []
    positions = set(c.get("positions", {}))
    bodies = c.get("bodies", {})
    rules = c.get("aggregation", {})
    scenes = c.get("scenes", {})
    conversions = c.get("conversions")

    def known_positions(names, where: str) -> None:
        for name in _list(names):
            if name != "anyone" and name not in positions:
                faults.append((1, f"{where} names the position {name}, which the constitution lacks"))

    # 1. Closed: every position, body, scene and rule mentioned exists.
    for name, position in c.get("positions", {}).items():
        for kind in position.get("held by", []):
            if kind not in KINDS_OF_PARTY:
                faults.append((1, f"the position {name} is held by an unknown kind of party, {kind}"))
    for name, rule in c.get("boundary", {}).items():
        known_positions(name, "a boundary rule")
        entry = rule.get("entry")
        if isinstance(entry, dict):
            if "decided by" in entry and entry["decided by"] not in rules:
                faults.append((1, f"entry to {name} is decided by {entry['decided by']}, which has no rule"))
            for key in ("by lot among", "sponsored by"):
                if key in entry:
                    known_positions(entry[key], f"entry to {name}")
            if "after" in entry and (conversions is None or entry["after"] not in conversions):
                faults.append((5, f"entry to {name} follows the conversion {entry['after']}, which is not listed"))
    for rule in c.get("choice", []):
        known_positions(rule.get("who", "anyone"), "a choice rule")
        for scene in rule.get("in", []):
            if scene not in scenes:
                faults.append((1, f"a choice rule names the scene {scene}, which the constitution lacks"))
    for name, body in bodies.items():
        known_positions(body.get("of", []), f"the body {name}")
    for name, scene in scenes.items():
        for side in ("read", "write"):
            if isinstance(scene.get(side), list):
                known_positions(scene[side], f"the scene {name}")
    for name, conversion in (conversions or {}).items():
        known_positions(conversion.get("to position", []), f"the conversion {name}")
        if conversion.get("decided by") not in rules:
            faults.append((1, f"the conversion {name} is decided by a rule that is lacking"))
    claims = c.get("claims")
    if claims is not None:
        for key in ("route", "review"):
            if claims.get(key) is not None and claims[key] not in bodies:
                faults.append((1, f"claims name the body {claims[key]}, which the constitution lacks"))
        if claims.get("decided by") is not None and claims["decided by"] not in rules:
            faults.append((1, "claims are decided by a rule that is lacking"))

    # 2. Every kind of decision has a body and a rule with a declared unit of count, and the body can be filled.
    boundary = c.get("boundary", {})
    for name, rule in rules.items():
        body = rule.get("body")
        if body not in bodies:
            faults.append((2, f"the decision {name} has no body"))
            continue
        if not rule.get("rule"):
            faults.append((2, f"the decision {name} has no rule"))
        if rule.get("count by") not in UNITS_OF_COUNT:
            faults.append((2, f"the decision {name} declares no unit of count"))
        if not any("entry" in boundary.get(position, {}) for position in bodies[body].get("of", [])):
            faults.append((2, f"the body {body} cannot be filled: none of its positions has a rule of entry"))

    # 3. Each level has a rule of amendment, and the constitutional level names what is entrenched.
    amendment = c.get("amendment", {})
    for level in LEVELS:
        if amendment.get(level) not in rules:
            faults.append((3, f"the {level} level has no rule of amendment"))
    if not isinstance(amendment.get("entrenched"), list):
        faults.append((3, "what is entrenched is not named"))

    # 4. The route for claims is declared (it may be declared empty).
    if claims is None or "route" not in claims:
        faults.append((4, "the route for claims is not declared"))

    # 5. Every conversion between registers is listed.
    if not isinstance(conversions, dict):
        faults.append((5, "the conversions between registers are not listed"))

    # 6. The custody of means is declared, and executable powers are recognised or held as a declared reserve.
    custody = c.get("custody")
    if custody is None or "powers" not in custody:
        faults.append((6, "the custody of means is not declared"))
    else:
        recognised: dict[str, set[str]] = {}
        for rule in c.get("choice", []):
            for kind in _list(rule.get("may", [])) + _list(rule.get("must", [])):
                recognised.setdefault(rule.get("who", "anyone"), set()).add(kind)
        reserve = {(r.get("holder"), power) for r in custody.get("reserve", [])
                   if r.get("scope") and r.get("term") for power in r.get("powers", [])}
        for power, holders in custody["powers"].items():
            for holder in holders:
                known_positions(holder, "the custody of means")
                allowed = power in recognised.get(holder, set()) or power in recognised.get("anyone", set())
                if not allowed and (holder, power) not in reserve:
                    faults.append((6, f"{holder} can {power} and no rule recognises it: overreach"))

    # 7. The properties to be preserved through amendments are declared.
    if not isinstance(c.get("preserved"), list):
        faults.append((7, "the properties preserved through amendments are not declared"))

    # 8. Receiving capacity and the forms of circulation are declared.
    receiving = c.get("receiving", {})
    if "unit" not in receiving or "per round" not in receiving:
        faults.append((8, "receiving capacity is not declared"))
    circulation = c.get("circulation")
    if not isinstance(circulation, list) or not circulation:
        faults.append((8, "the forms of circulation are not declared"))
    else:
        for form in circulation:
            if form not in FORMS:
                faults.append((8, f"{form} is not a form of circulation of the design"))

    # 9. The policy of records is declared.
    if "captured" not in c.get("records", {}):
        faults.append((9, "the policy of records is not declared"))
    return faults


def profile(c: dict) -> dict:
    """What a reader compares between constitutions."""
    return {
        "positions": len(c["positions"]),
        "decisions": len(c["aggregation"]),
        "counted by": sorted({rule["count by"] for rule in c["aggregation"].values()}),
        "open positions": sorted(p for p, b in c["boundary"].items() if b.get("entry") == "open"),
        "positions that operatives may hold": sorted(p for p, d in c["positions"].items() if "operative" in d["held by"]),
        "positions that communities may hold": sorted(p for p, d in c["positions"].items() if "community" in d["held by"]),
        "conversions": sorted(c["conversions"]),
        "circulation": c["circulation"],
        "route for claims": c["claims"]["route"],
    }
