"""Multilateral practice, for scripts in Python and the works' servers (ADR 0132): an undertaking among many parties and a
partnership's record read on a date, a decision rule's a priori power, ideal points from votes, bargaining, the division of issues
and of a joint gain; and, since ADR 0134, the affairs by which an undertaking changes (a question decided under a rule as its
instrument states it, a negotiation draft by draft, events as their reports, mandates, reviews, institutions and reciprocity),
asked of the engine a work has built.

The computations are the diplomacy domain's C++ with nomos and tyche, composed in ``api/cpp/science/multilateral.hpp``, which
documents every operation and the records they take (``noetarium.undertaking/1``, ``noetarium.partnership/1``):

    from science import multilateral
    multilateral.ask("undertaking", undertaking=u, at="2025-12-31")      # faults, layers, positions, diagnostics, timeline
    multilateral.ask("partnership", partnership=p, at="2025-12-31",
                     standard={"kind": "standing", "least": "clear"})      # screens, states, benefits, mutual benefit, signs
    multilateral.ask("rule", members=names, rule={"weights": w, "quota": q})
    multilateral.ask("ideal_points", members=m, items=i, votes=v, anchor=a, bootstrap=100)
    multilateral.ask("bargaining", outcomes=[[u1, u2], ...], disagreement=[d1, d2])
    multilateral.ask("division", issues=names, a=points_a, b=points_b)
    multilateral.ask("coalitional", members=m, worths=[{"coalition": [...], "worth": w}])
    multilateral.ask("outcome", rule=multilateral.declared("article-27-3"), question=q)   # adopted, under which readings, and why
    multilateral.ask("rule_game", rule=r)                                # the a priori power of a declared rule (24 members at most)
    multilateral.ask("negotiation", negotiation=n)                       # zones draft by draft, linkage, win-sets, value claimed
    multilateral.ask("events", events=e, channels=c)                     # support with each origin counted once, series, signs
    multilateral.ask("mandates", registry=m, at="2025-12-31", undertaking=u)
    multilateral.ask("reviews", record=r, at="2025-12-31", mild=2)
    multilateral.ask("institutions", landscape=l, at="2025-12-31", transfers=t, windows=[30, 365])

The engine is a program built by a work: Multilateral Phronimos builds ``data/cache/engines/multilateral-phronimos/multilateral/
engine``; ``NOETARIUM_MULTILATERAL_ENGINE`` names another.

The rules of two principal organs of the United Nations are given as ``RULES``, read from the Charter: the Security Council's on
matters other than procedure (Article 27(3): nine members, the permanent members concurring) and the General Assembly's on
important questions (Article 18(2): two thirds of the members present and voting, read here of all members, which assumes every
member present and voting). They are rules of the text; whose practice reads them otherwise (an abstention of a permanent member
not counted as a veto, since 1946) is said beside them, and a caller declares the reading.

``DECLARED`` holds rules as their instruments state them (``noetarium.rule/1``), with their open terms and the readings each admits:
Article 27(3) with the words alone and the Court's reading of "concurring votes"; Article 18(2) with rule 86's reading of "present
and voting"; the WTO's consensus with its fallback to a majority of the votes cast. Members are named generically; a caller gives
the body's own.

``votes_matrix`` turns a product of the data library (``noetarium.votes/1``, from ``data/products/multilateral_cases.py``) into the
engine's request, members named by the data library's policy.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("multilateral-phronimos/multilateral", env="NOETARIUM_MULTILATERAL_ENGINE")

PERMANENT = ["China", "France", "Russian Federation", "United Kingdom", "United States"]

RULES = {
    "security-council": {
        "title": "The Security Council, on matters other than procedure",
        "source": "Charter of the United Nations, Article 27(3)",
        "text": "Decisions of the Security Council on all other matters shall be made by an affirmative vote of nine members including the "
                "concurring votes of the permanent members.",
        "reading": "The text: a permanent member's vote is needed. In practice since 1946 an abstention of a permanent member has not been "
                   "counted as a veto (ICJ, Namibia Advisory Opinion, 1971, §22); a rule on yes and no votes cannot show that difference.",
        "members": PERMANENT + [f"elected member {k}" for k in range(1, 11)],
        "rule": {"all_of": [{"all": PERMANENT}, {"at_least": 9}]},
    },
    "general-assembly-important": {
        "title": "The General Assembly, on important questions",
        "source": "Charter of the United Nations, Article 18(2)",
        "text": "Decisions of the General Assembly on important questions shall be made by a two-thirds majority of the members present and voting.",
        "reading": "Read here of all 193 members, as if every member were present and voting; each member's a priori power is then equal.",
        "size": 193,
    },
}


ACTS = ["yes", "no", "abstain", "not voting", "absent", "objection", "reservation"]

DECLARED = {
    "article-27-3": {
        "id": "article-27-3", "title": "Article 27(3)", "instrument": "Charter of the United Nations, Article 27(3)",
        "text": RULES["security-council"]["text"],
        "members": PERMANENT + [f"elected member {k}" for k in range(1, 11)],
        "terms": [{"name": "concurring vote", "about": "whether a permanent member that abstains concurs",
                   "readings": [{"name": "an affirmative vote", "acts": ["yes"], "source": "analyst", "grade": "B", "by": "the words alone"},
                                {"name": "any vote but a negative one", "acts": ["yes", "abstain", "not voting"], "source": "authority", "grade": "B",
                                 "by": "International Court of Justice, Namibia Advisory Opinion (1971), §22",
                                 "rationale": "a voluntary abstention by a permanent member does not bar the adoption of a resolution", "stated": True}]}],
        "clause": {"kind": "all of", "parts": [
            {"kind": "threshold", "id": "nine", "text": "an affirmative vote of nine members", "counted": ["yes"], "count": 9},
            {"kind": "concurrence", "id": "permanent", "text": "including the concurring votes of the permanent members", "members": PERMANENT,
             "counted": {"term": "concurring vote"}}]},
    },
    "article-18-2": {
        "id": "article-18-2", "title": "Article 18(2)",
        "instrument": "Charter of the United Nations, Article 18(2); Rules of Procedure of the General Assembly, rule 86",
        "text": RULES["general-assembly-important"]["text"],
        "members": [f"member {k}" for k in range(1, 194)],
        "terms": [{"name": "present and voting", "about": "whether abstaining members are among those present and voting",
                   "readings": [{"name": "members casting an affirmative or negative vote", "acts": ["yes", "no"], "source": "authority", "grade": "B",
                                 "by": "Rules of Procedure of the General Assembly, rule 86", "stated": True}]}],
        "clause": {"kind": "threshold", "id": "two-thirds", "text": "a two-thirds majority of the members present and voting",
                   "counted": ["yes"], "base": {"term": "present and voting"}, "fraction": [2, 3], "strict": False},
    },
    "wto-ix-1": {
        "id": "wto-ix-1", "title": "Article IX:1",
        "instrument": "Marrakesh Agreement Establishing the World Trade Organization, Article IX:1 and its footnote 1",
        "text": "The WTO shall continue the practice of decision-making by consensus followed under GATT 1947. Except as otherwise provided, "
                "where a decision cannot be arrived at by consensus, the matter at issue shall be decided by voting.",
        "members": [f"member {k}" for k in range(1, 9)],
        "terms": [{"name": "formal objection", "about": "what blocks a consensus",
                   "readings": [{"name": "a formal objection by a member present", "acts": ["objection"], "source": "authority", "grade": "B",
                                 "by": "Marrakesh Agreement, footnote 1 to Article IX:1", "stated": True}]}],
        "clause": {"kind": "fallback", "parts": [
            {"kind": "consensus", "id": "consensus", "stage": "consensus", "counted": {"term": "formal objection"}},
            {"kind": "threshold", "id": "a majority of the votes cast", "stage": "vote", "counted": ["yes"], "base": "present and voting",
             "fraction": [1, 2], "strict": True}]},
    },
}


def declared(name: str, members: list[str] | None = None) -> dict:
    """A rule of DECLARED, with the body's own members in place of the generic names when given (in the same number and order)."""
    import copy
    r = copy.deepcopy(DECLARED[name])
    if members is not None:
        if len(members) != len(r["members"]):
            raise ValueError(f"{name} has {len(r['members'])} members, not {len(members)}")
        rename = dict(zip(r["members"], members))
        r["members"] = list(members)

        def walk(c):
            if "members" in c:
                c["members"] = [rename.get(m, m) for m in c["members"]]
            for part in c.get("parts", []):
                walk(part)
        walk(r["clause"])
    return r


def ask(op: str, timeout: float = 300.0, **request) -> dict:
    """One request to the multilateral engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def rule_request(name: str) -> dict:
    """A rule of RULES as the engine's request."""
    r = RULES[name]
    if "size" in r:
        n = r["size"]
        return {"members": [f"member {k}" for k in range(1, n + 1)], "rule": {"weights": [1] * n, "quota": -(-2 * n // 3)}}
    return {"members": r["members"], "rule": r["rule"]}


def votes_matrix(product: dict, anchor: str = "") -> dict:
    """The engine's request for ideal points from a noetarium.votes/1 product: members by code, their names beside."""
    members = [m["code"] for m in product["members"]]
    votes = [[-1 if v is None else int(v) for v in row] for row in product["votes"]]
    return {"members": members, "names": {m["code"]: m["name"] for m in product["members"]}, "items": [i["id"] for i in product["items"]],
            "votes": votes, "anchor": anchor or members[0]}
