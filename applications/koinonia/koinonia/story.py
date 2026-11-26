"""Acts told as sentences a person can read (ADR 0169): who did what, to what, named by their labels rather than their ids, with a
theme for each sentence (work, decisions, circulation, membership, findings, words, refusals) by which a page filters them."""

from __future__ import annotations

THEMES = {"offer": "work", "take": "work", "deliver": "work", "accept": "work", "release": "work", "request": "work", "cancel": "work",
          "hand over": "work", "propose": "decisions", "vote": "decisions", "amend": "decisions", "admit": "decisions", "convert": "membership",
          "commit": "circulation", "assign": "circulation", "join": "membership", "leave": "membership", "mandate": "membership",
          "revoke": "membership", "found": "membership", "assess": "findings", "claim": "decisions", "answer": "decisions",
          "say": "words", "report": "circulation", "attest": "circulation"}


def label(s, ident: str) -> str:
    a = s.agents.get(ident)
    if a:
        return a.get("label") or ident
    p = s.parties.get(ident)
    return (p.get("label") if p else None) or ident


def item(s, ident: str) -> str:
    base = ident.split("~")[0]
    b = s.work.batch(base)
    if b:
        return f"batch {base} ({len(b['entries'])} entries)"
    if base.startswith("d-"):
        e = s.work.entries.get(base[2:], {})
        return f"the disagreement on “{e.get('name', base[2:])}”"
    if base.startswith("t-"):
        e = s.work.entries.get(base[2:], {})
        return f"a trial on “{e.get('name', base[2:])}”"
    if base.startswith("s-"):
        return "a debt for a service"
    return base


def whose(s, commitment_id: str, what: str = "offer") -> str:
    parts = commitment_id.split("~")
    return f"the {what} of {label(s, parts[1])}" if len(parts) > 1 else f"the {what}"


def sentence(s, community: str, act: dict) -> tuple[str, str]:
    """The sentence an admitted act is told by, and its theme."""
    f, kind, who = act.get("fields", {}), act["kind"], label(s, act["author"])
    to = label(s, f.get("to", "")) if f.get("to") else ""
    text = {
        "found": f"{who} founded {community}",
        "join": f"{who} joined {community} as {f.get('position', '')}",
        "leave": f"{who} left its place as {f.get('position', '')}",
        "mandate": f"{who} lent {label(s, act.get('grant', {}).get('holder', ''))} to {community}",
        "revoke": f"{who} revoked the mandate {f.get('which', '')}",
        "request": (f"{who} sent {item(s, f.get('id', ''))} to the guild" if f.get("to") == "guild" else f"{who} posted {item(s, f.get('id', ''))}"),
        "offer": f"{who} offered to do {item(s, f.get('id', ''))}",
        "cancel": f"{who} withdrew its offer for {item(s, f.get('id', ''))}",
        "take": f"{who} took {whose(s, f.get('id', ''))} for {item(s, f.get('id', ''))}",
        "deliver": (f"{who} repaid a debt with a service" if f.get("matter") == "service" else f"{who} delivered its work on {item(s, f.get('id', ''))}"),
        "accept": (f"{who} accepted a repayment" if f.get("id", "").startswith("s-") else f"{who} accepted {whose(s, f.get('id', ''), 'work')} on {item(s, f.get('id', ''))}"),
        "release": ({"set off": f"{who} set off a debt against one owed back", "cleared": f"{who} released a debt cleared around the cycle {f.get('cycle', '')}",
                     "dissolved": f"{who} released a debt, nothing being owed under the arrangement in force"}.get(f.get("why", ""), f"{who} released {item(s, f.get('id', ''))}")),
        "commit": f"{who} entered a debt to {to} for a service received",
        "assign": f"{who} passed a claim it held on to {to} in payment",
        "report": f"{who} reported a service to {to}" if to else f"{who} reported",
        "propose": f"{who} proposed: {f.get('change.compact') or f.get('decision', '')}",
        "vote": f"{who} voted {f.get('value', '')} on proposal {f.get('id', '')}",
        "amend": f"{who} enacted proposal {f.get('id', '')}",
        "admit": f"{who} admitted {label(s, f.get('whom', ''))} as {f.get('position', '')}",
        "convert": f"{who} made {label(s, f.get('whom', ''))} a member",
        "assess": (f"{who} recorded the guild's finding on “{s.work.entries.get(f.get('object', ''), {}).get('name', f.get('object', ''))}”: {f.get('value', '')}"
                   if f.get("criterion") == "adjudication by the guild" else f"{who} judged “{s.work.entries.get(f.get('object', ''), {}).get('name', f.get('object', ''))}”: {f.get('value', '')}"),
        "claim": f"{who} brought a claim against {label(s, f.get('against', ''))}",
        "answer": f"{who} answered a claim: {f.get('finding', '')}",
        "say": f"{who}: “{act.get('body', '')}”",
        "attest": (f"{who} attested that {label(s, f.get('object', ''))} declined work it could do ({f.get('value', '')} idle "
                   f"{'checker' if str(f.get('value', '')) == '1' else 'checkers'} while work lay open elsewhere)"),
    }.get(kind, f"{who} {kind}")
    return text, THEMES.get(kind, "work")


def refusal(s, act: dict, reason: str) -> str:
    return f"{label(s, act['author'])} tried to {act['kind']} and was refused: {reason}"
