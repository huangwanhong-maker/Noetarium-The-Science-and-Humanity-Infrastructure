"""Multilateral practice, for scripts in Python and the works' servers (ADR 0132): an undertaking among many parties and a
partnership's record read on a date, a decision rule's a priori power, ideal points from votes, bargaining, the division of issues
and of a joint gain, asked of the engine a work has built.

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

The engine is a program built by a work: Multilateral Phronimos builds ``data/cache/engines/multilateral-phronimos/multilateral/
engine``; ``NOETARIUM_MULTILATERAL_ENGINE`` names another.

The rules of two principal organs of the United Nations are given as ``RULES``, read from the Charter: the Security Council's on
matters other than procedure (Article 27(3): nine members, the permanent members concurring) and the General Assembly's on
important questions (Article 18(2): two thirds of the members present and voting, read here of all members, which assumes every
member present and voting). They are rules of the text; whose practice reads them otherwise (an abstention of a permanent member
not counted as a veto, since 1946) is said beside them, and a caller declares the reading.

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
