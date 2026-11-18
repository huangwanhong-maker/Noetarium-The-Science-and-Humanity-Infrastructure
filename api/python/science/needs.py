"""The evolutionary exploration and validation of needs, for scripts in Python and the works' servers (ADR 0125): statements of need
searched for by evolution on the discovery part of a dataset, admitted through a reusable holdout, and confirmed in order by an
online procedure, asked of the engine a work has built.

The computations are composed in ``api/cpp/science/needs.hpp``, which documents the dataset it takes (``noetarium.needs-data/1``, as
``data/products/needs_tables.py`` writes it) and its operations:

    from science import needs
    d = needs.data("melbourne-assistance")                    # the data library's product, from the cache
    needs.ask("describe", data=d)
    run = needs.ask("evolve", data=d, generations=10)          # {state, report, refused}
    run = needs.ask("evolve", data=d, state=run["state"], generations=10, proposals=[{"text": "...", "source": "me"}])

The engine is a program built by a work: the Urban Studio's plug-in *Needs* builds ``data/cache/engines/urban-studio/needs/engine``;
``NOETARIUM_NEEDS_ENGINE`` names another.

**A language model proposes, and never judges.** ``propose`` asks a model (``science.models``) for statements in the domain's
language, given the language, the data's vocabulary and the population's best statements with their scores on the discovery part
alone, as an optimiser's prompt holds the best solutions so far (Yang et al. 2024, OPRO). It is never shown a validation answer, a
confirmation or a standing, so that nothing of the parts kept aside reaches the search through it. What it writes is read by the
engine as anyone's would be; a line that does not read, or that the data cannot judge, is refused with the reason and never
repaired. Its statements are proposals of grade C, credited to it by the evolution like any other operator.

**A model in every part of the search** (ADR 0126). ``fulfil`` answers the engine's ``requests`` (``evolve`` with ``requests={budget,
roles}``) in one call, in four roles: seed, mutate, cross and reflect, the variations proposed with the members they came from as their
parents (``model:<role>``, credited by the evolution, each role's share following how its proposals fared). ``interpret`` asks a model
what a statement claims in plain words and for other readings of its open terms and rival explanations; ``structure`` for causal graphs
and rivals. ``with_proposals`` adds what was proposed to a dataset as grade C, so that the engine's ``interpret`` and ``test`` show it
as proposed, apart from the record's (tekmerion's construal). ``provenance`` writes the lineage as W3C PROV-JSON, a model an agent of
its own. ``rounds`` runs a search in rounds with all of it.
"""

from __future__ import annotations

import json
import re
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("urban-studio/needs", env="NOETARIUM_NEEDS_ENGINE")
DATA_SCHEMA = "noetarium.needs-data/1"
STATE_SCHEMA = "noetarium.needs-search/1"
ROOT = Path(__file__).resolve().parents[3]
PRODUCTS = ROOT / "data" / "cache" / "products" / "needs"


def ask(op: str, timeout: float = 1800.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def data(case: str | Path) -> dict:
    """A product's dataset, by its case's name or by the path of its file."""
    path = Path(case)
    if not path.suffix:
        path = PRODUCTS / f"{case}.json"
    if not path.is_file():
        raise LookupError(f"there is no dataset at {path}; make it with python data/products/needs_tables.py {case}")
    d = json.loads(path.read_text(encoding="utf-8"))
    if d.get("schema") != DATA_SCHEMA:
        raise ValueError(f"{path} is not a {DATA_SCHEMA}")
    return d


def cases() -> list[dict]:
    """The cases the data library has made, from its index."""
    index = PRODUCTS / "index.json"
    if not index.is_file():
        return []
    return [{"case": k} | v for k, v in json.loads(index.read_text(encoding="utf-8")).get("cases", {}).items()]


# ---- a language model's proposals ----

GRAMMAR = """need      ::= "need" domain "for" group [ "within" group ] "by" standard
group     ::= "everyone" | condition { "and" condition }
condition ::= attribute ( "=" | "!=" | "<" | "<=" | ">" | ">=" ) value | attribute "in" "{" value { "," value } "}"
standard  ::= "normative" indicator ( "above" | "below" ) level          (a level may be a share, 0.3, or a percentage, 30%)
            | "comparative" indicator ( "higher" | "lower" ) [ "than" group ]
            | "expressed" demand "over" provision
            | "felt" indicator ( "above" | "below" ) level
A value with a space or that reads as a number but is a category is quoted: area = "North Melbourne", sa1 = "20604112201"."""

PROPOSE_SYSTEM = """You propose hypotheses of need for a search that tests them against data. A statement of need says that a group of people, \
within a frame, has a gap in a domain of satisfaction, judged on an indicator by a standard (Bradshaw's four: normative, comparative, \
expressed, felt). You write statements in a small formal language, one per item, exactly as its grammar says, naming only the \
attributes, levels, indicators and domains you are given, each indicator in the direction in which it is worse. Your statements are \
proposals: each will be read by a parser that refuses what does not fit, tested on data you do not see, and kept only if the tests \
uphold it. Propose groups that are specific and plausible, that differ from those already found, and that a person could act on; give \
for each, in a sentence, why it might hold. Do not invent numbers about the data; you are not told how the statements fared beyond \
the scores given, and must not guess."""

PROPOSE_SCHEMA = {
    "type": "object",
    "properties": {
        "statements": {
            "type": "array",
            "items": {"type": "object", "properties": {"statement": {"type": "string"}, "why": {"type": "string"}},
                      "required": ["statement", "why"], "additionalProperties": False},
        }
    },
    "required": ["statements"],
    "additionalProperties": False,
}


def vocabulary(described: dict) -> str:
    """The data's vocabulary for a prompt: indicators, domains, and the attributes with their conditions."""
    lines = ["Indicators (name: what it measures; its domain; the direction in which it is worse; its value over everyone):"]
    for i in described.get("indicators", []):
        worse = "higher" if i.get("higher_is_worse", True) else "lower"
        value = i.get("value")
        shown = f"{value:.4g}" if isinstance(value, (int, float)) else "unknown"
        lines.append(f"  {i['name']}: {i.get('label') or i['name']}; domain {i.get('domain')}; worse when {worse}; {shown} over everyone")
    lines.append("Domains: " + ", ".join(described.get("domains", [])))
    by: dict[str, list[str]] = {}
    for c in described.get("conditions", []):
        by.setdefault(c.split(" ", 1)[0], []).append(c)
    lines.append("Attributes, with conditions the search uses (others of the same form are allowed):")
    for a in described.get("attributes", []):
        cs = by.get(a, [])
        lines.append(f"  {a}: " + ("; ".join(cs[:14]) + (" …" if len(cs) > 14 else "") if cs else "(no condition)"))
    return "\n".join(lines)


def best_so_far(report: dict | None, most: int = 15) -> str:
    """The population's best statements with their scores on the discovery part only, for a prompt."""
    if not report:
        return "Nothing has been found yet."
    names = report.get("objectives", [])
    rows = []
    for m in (report.get("population") or [])[:most]:
        obj = (m.get("objectives") or {}).get("objectives") or []
        said = []
        for n, v in zip(names, obj):
            if n == "gap":
                said.append(f"gap z {-v:.1f}")
            elif n == "coverage":
                said.append(f"{10 ** (-v):,.0f} people")
            elif n == "simplicity":
                said.append(f"{v:.0f} conditions")
        rows.append(f"  {m['text']}   [{', '.join(said)}]")
    return "The search's best statements so far, scored on the part of the data it sees:\n" + ("\n".join(rows) if rows else "  (none)")


def prompt_for(described: dict, report: dict | None, n: int, description: str | None = None, vary: list[str] | None = None) -> str:
    parts = ["The language of statements:\n" + GRAMMAR, vocabulary(described), best_so_far(report)]
    if description:
        parts.append("The person asks you to look for needs of this kind:\n" + description.strip()[:2000])
    if vary:
        parts.append("Vary these statements, each in a way the search's own operators would not (a condition another person would think of, "
                     "a related indicator or domain, a frame that changes the comparison):\n" + "\n".join("  " + v for v in vary[:12]))
    parts.append(f"Propose {n} new statements, distinct from those above.")
    return "\n\n".join(parts)


def propose(d: dict, report: dict | None = None, n: int = 12, provider=None, *, description: str | None = None,
            vary: list[str] | None = None, described: dict | None = None, name: str = "a language model") -> dict:
    """A model's proposals: ``{text, read, refused, by, grade: "C", source}``: ``text`` the statements that read, one to a line, for
    ``evolve``'s proposals; ``refused`` the lines refused with the engine's reasons. models.Refusal when no answer could be had."""
    from . import models
    provider = provider or models.Anthropic()
    described = described or ask("describe", data=d, options={"simulate": False})
    prompt = prompt_for(described, report, n, description, vary)
    answer, served = provider.answer(PROPOSE_SYSTEM, prompt, PROPOSE_SCHEMA)
    items = [x for x in (answer.get("statements") if isinstance(answer, dict) else None) or [] if isinstance(x, dict)]
    lines = [" ".join(str(x.get("statement", "")).split()) for x in items]
    why = {" ".join(str(x.get("statement", "")).split()): str(x.get("why", ""))[:400] for x in items}
    read = ask("read", data=d, text="\n".join(lines)) if lines else []
    kept, refused = [], []
    for r in read:
        (kept if r["read"] else refused).append({"line": r["text"], "statement": r.get("statement"), "why": r.get("why") or why.get(r["text"], "")})
    for k in kept:
        k["reason"] = why.get(k["line"], "")
    return {"text": "\n".join(k["statement"] for k in kept), "read": kept, "refused": refused, "by": f"{provider.name}, {served}",
            "source": name, "grade": "C", "status": "proposed", "prompt_chars": len(prompt)}


def rounds(d: dict, options: dict | None = None, *, count: int = 3, generations: int = 5, provider=None, n: int = 12,
           state: dict | None = None, description: str | None = None, roles: tuple | list = None, log=print) -> dict:
    """A search in rounds: `generations` generations, then what the engine asks of a model (``requests``: seed, mutate, cross,
    reflect, each role's share by how its proposals fared), answered by the model in one call and proposed to the next round with
    the members each was made from as its parents. ``{state, report, proposals}``, each round's answers with their refused lines."""
    options = options or {}
    roles = list(roles or ROLES)
    described = ask("describe", data=d, options=dict(options, simulate=False))
    run = {"state": state, "report": None, "requests": None}
    made = []
    for k in range(count):
        proposals = []
        if provider is not None:
            asked = run.get("requests")
            if asked is None and (k > 0 or description) and "seed" in roles:
                asked = [{"role": "seed", "count": n, "parents": [], "texts": [], "notes": []}]
            if asked:
                f = fulfil(d, asked, provider, report=run["report"], described=described, description=description if k == 0 else None)
                made.append(f)
                proposals = f["proposals"]
                log(f"  round {k + 1}: the model answered {len(asked)} requests: {len(f['read'])} statements read, {len(f['refused'])} refused")
        request = {"data": d, "options": options, "generations": generations, "proposals": proposals}
        if provider is not None:
            request["requests"] = {"budget": n, "roles": roles}
        if run["state"]:
            request["state"] = run["state"]
        run = ask("evolve", **request)
        rep = run["report"]
        log(f"  round {k + 1}: generation {rep['generation']}, {len(rep['archive'])} kept, {sum(c['confirmed'] for c in rep['confirmations'])} confirmed")
    return {"state": run["state"], "report": run["report"], "proposals": made}


# ---- a model in every part of the search (ADR 0126) ----
#
# A model may seed the search, vary its members, join two, refine a kept statement, interpret a statement (its words, other readings
# of its open terms, rivals), and propose causal structure. Whatever it writes is read by the engine as anyone's would be, enters as a
# proposal of grade C (a statement under the source "model:<role>", a reading or a graph or a rival with source "proposal"), and is
# shown "as proposed", apart from the record's: it never judges, and it is never shown the validation or confirmation parts.

ROLES = ("seed", "mutate", "cross", "reflect")

ROLE_TASK = {
    "seed": "Write {count} new statements of need, specific and plausible, unlike those the search already has.",
    "mutate": "Vary this statement once, in a way the search's own operators would not: a condition another person would think of, "
              "a related indicator or domain, a frame or a reference that changes the comparison.",
    "cross": "Write one statement that joins what is most telling in these two.",
    "reflect": "This statement was kept by the search. From what the part of the data the search sees says of it, write one "
               "refinement that would make it sharper or more robust: a narrower or broader group, another reference, another measure.",
}

EVOLVE_SYSTEM = PROPOSE_SYSTEM + """ You are given several numbered requests, each in a role, some with the statements they start from and \
what the part of the data the search sees says of them. Answer every request by its number, with as many statements as it asks for."""

EVOLVE_SCHEMA = {
    "type": "object",
    "properties": {
        "answers": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "request": {"type": "integer"},
                    "statements": PROPOSE_SCHEMA["properties"]["statements"],
                },
                "required": ["request", "statements"],
                "additionalProperties": False,
            },
        }
    },
    "required": ["answers"],
    "additionalProperties": False,
}


def requests_prompt(described: dict, report: dict | None, requests: list[dict], description: str | None = None) -> str:
    """The prompt for a round's requests: the language, the vocabulary, the best so far on discovery, and each request with its role,
    its statements and their discovery notes (the engine writes the notes from the discovery part alone)."""
    parts = ["The language of statements:\n" + GRAMMAR, vocabulary(described), best_so_far(report)]
    if description:
        parts.append("The person asks you to look for needs of this kind:\n" + description.strip()[:2000])
    lines = []
    for i, q in enumerate(requests):
        task = ROLE_TASK.get(q.get("role"), ROLE_TASK["seed"]).format(count=q.get("count", 1))
        lines.append(f"Request {i} ({q.get('role')}, {q.get('count', 1)} statement{'s' if q.get('count', 1) != 1 else ''}): {task}")
        for t, note in zip(q.get("texts") or [], q.get("notes") or []):
            lines.append(f"    {t}\n      {note}")
    parts.append("Requests:\n" + "\n".join(lines))
    return "\n\n".join(parts)


def fulfil(d: dict, requests: list[dict], provider=None, *, report: dict | None = None, described: dict | None = None,
           description: str | None = None) -> dict:
    """A model's answers to the engine's requests, in one call: ``{proposals, read, refused, by, grade}``. ``proposals`` are for
    ``evolve`` ([{text, source: "model:<role>", parents}]); each statement was read by the engine, a line that does not read refused
    with the reason and never repaired."""
    from . import models
    provider = provider or models.Anthropic()
    described = described or ask("describe", data=d, options={"simulate": False})
    prompt = requests_prompt(described, report, requests, description)
    answer, served = provider.answer(EVOLVE_SYSTEM, prompt, EVOLVE_SCHEMA)
    lines, origin, why = [], [], {}
    for a in (answer.get("answers") if isinstance(answer, dict) else None) or []:
        if not isinstance(a, dict) or not isinstance(a.get("request"), int) or not 0 <= a["request"] < len(requests):
            continue
        q = requests[a["request"]]
        wanted = int(q.get("count", 1))
        for x in [x for x in a.get("statements") or [] if isinstance(x, dict)][:wanted]:
            line = " ".join(str(x.get("statement", "")).split())
            lines.append(line), origin.append(q), why.setdefault(line, str(x.get("why", ""))[:400])
    read = ask("read", data=d, text="\n".join(lines)) if lines else []
    kept, refused, proposals = [], [], []
    for r, q in zip(read, origin):
        entry = {"line": r["text"], "statement": r.get("statement"), "role": q.get("role"), "parents": q.get("parents") or [], "reason": why.get(r["text"], "")}
        if r["read"]:
            kept.append(entry)
            proposals.append({"text": r["statement"], "source": f"model:{q.get('role')}", "parents": q.get("parents") or []})
        else:
            refused.append(entry | {"why": r.get("why")})
    return {"proposals": proposals, "read": kept, "refused": refused, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed",
            "requests": len(requests), "prompt_chars": len(prompt)}


# ---- interpretation and structure ----

INTERPRET_SYSTEM = """You help a person read a statement of need, written in a small formal language, for a search that tests it against \
data you do not see. Say in one or two plain sentences what it claims. Then name the terms in it that are open to other readings, and \
propose other readings, each with a sentence of why: another group to compare with (a reference, written as a group of the language, \
"everyone" for the whole frame, or "the rest" for everyone in the frame outside the group), another of the given indicators that \
measures the same need, or another level for a normative standard. Propose rival explanations too: an attribute of the data within \
whose groups (strata) the gap might vanish, each stratum written as a group of the language. Name only attributes, levels and \
indicators you are given. You never say whether the statement holds, how strong it is, or what the data show: your readings and rivals \
are proposals, which the person and the tests will weigh."""

INTERPRET_SCHEMA = {
    "type": "object",
    "properties": {
        "words": {"type": "string"},
        "readings": {
            "type": "array",
            "items": {"type": "object", "properties": {"kind": {"type": "string", "enum": ["reference", "indicator", "level"]},
                                                        "text": {"type": "string"}, "rationale": {"type": "string"}},
                      "required": ["kind", "text", "rationale"], "additionalProperties": False},
        },
        "rivals": {
            "type": "array",
            "items": {"type": "object", "properties": {"name": {"type": "string"}, "strata": {"type": "array", "items": {"type": "string"}},
                                                        "rationale": {"type": "string"}},
                      "required": ["name", "strata", "rationale"], "additionalProperties": False},
        },
    },
    "required": ["words", "readings", "rivals"],
    "additionalProperties": False,
}

STRUCTURE_SYSTEM = """You help a person state what they believe of the causes among the columns of a dataset about people's needs, for \
tests that use causal graphs to decide which columns to hold fixed when asking whether provision meets need. Given the columns, the \
provision named for each domain and the graph already declared, propose other causal graphs (each a list of arrows between the given \
columns, with no cycle), each with a sentence of why: a confounder the declared graph leaves out, an arrow it has the wrong way. \
Propose rival explanations too: attributes within whose groups (strata, each written as a group of the language) a gap of need might \
vanish. Name only the columns, attributes and levels you are given. You never say what the data show; your graphs and rivals are \
proposals, which the tests will read under each and the person will weigh."""

STRUCTURE_SCHEMA = {
    "type": "object",
    "properties": {
        "graphs": {
            "type": "array",
            "items": {"type": "object", "properties": {
                "name": {"type": "string"}, "rationale": {"type": "string"},
                "arrows": {"type": "array", "items": {"type": "object", "properties": {"from": {"type": "string"}, "to": {"type": "string"}},
                                                      "required": ["from", "to"], "additionalProperties": False}}},
                "required": ["name", "arrows", "rationale"], "additionalProperties": False},
        },
        "rivals": INTERPRET_SCHEMA["properties"]["rivals"],
    },
    "required": ["graphs", "rivals"],
    "additionalProperties": False,
}

_STATEMENT = re.compile(r"^need\s+(\S+)\s+for\s+.*?\s+by\s+(normative|comparative|felt|expressed)\s+(\S+)")


def _parts_of(statement: str) -> tuple[str, str, str]:
    m = _STATEMENT.match(" ".join(statement.split()))
    if not m:
        raise ValueError(f"not a statement of need: {statement!r}")
    return m.group(1), m.group(2), m.group(3)


def _groups_that_read(d: dict, groups: list[str], domain: str, indicator: str, worse: str) -> dict[str, str | None]:
    """Each group text: None when it reads as a group the data can judge, else the reason (the engine reads a statement made of it)."""
    special = {g: None for g in groups if g.strip() in ("everyone", "the rest", "rest")}
    others = [g for g in groups if g not in special]
    lines = [f"need {domain} for {g} by comparative {indicator} {worse}" for g in others]
    out = dict(special)
    for g, r in zip(others, ask("read", data=d, text="\n".join(lines)) if lines else []):
        out[g] = None if r["read"] else (r.get("why") or "it does not read")
    return out


def _level(text: str) -> float | None:
    t = text.strip()
    try:
        return float(t[:-1]) / 100 if t.endswith("%") else float(t)
    except ValueError:
        return None


def interpret(d: dict, statement: str, provider=None, *, described: dict | None = None) -> dict:
    """A model's reading of a statement: ``{statement, words, readings, rivals, refused, by, grade: "C"}``. ``words`` is what it
    claims in plain words; ``readings`` other readings of its open terms ({kind, text, rationale, indicator}); ``rivals`` rival
    explanations ({name, strata, rationale}). Each was checked against the data's vocabulary (a group read by the engine, an indicator
    of the data in the same direction, a level that is a number); what does not check is refused with the reason, never repaired."""
    from . import models
    provider = provider or models.Anthropic()
    described = described or ask("describe", data=d, options={"simulate": False})
    domain, standard, indicator = _parts_of(statement)
    prompt = "\n\n".join(["The language of statements:\n" + GRAMMAR, vocabulary(described), "The statement:\n  " + " ".join(statement.split())])
    answer, served = provider.answer(INTERPRET_SYSTEM, prompt, INTERPRET_SCHEMA)
    answer = answer if isinstance(answer, dict) else {}
    inds = {i["name"]: i for i in described.get("indicators", [])}
    own = inds.get(indicator, {})
    worse = "higher" if own.get("higher_is_worse", True) else "lower"
    readings, rivals, refused = [], [], []
    proposed = [r for r in answer.get("readings") or [] if isinstance(r, dict)]
    groups = [str(r.get("text", "")).strip() for r in proposed if r.get("kind") == "reference"]
    for rv in answer.get("rivals") or []:
        if isinstance(rv, dict):
            groups += [str(s).strip() for s in rv.get("strata") or []]
    checked = _groups_that_read(d, sorted(set(groups)), domain, indicator, worse) if standard != "expressed" else {}
    for r in proposed:
        kind, text, why = r.get("kind"), " ".join(str(r.get("text", "")).split()), str(r.get("rationale", ""))[:400]
        problem = None
        if kind == "reference":
            problem = "a reference is for a comparative statement" if standard != "comparative" else checked.get(text, "it does not read")
        elif kind == "indicator":
            if standard != "comparative":
                problem = "another measure is read for a comparative statement"
            elif text not in inds:
                problem = f"the data declare no indicator {text}"
            elif inds[text].get("higher_is_worse", True) != own.get("higher_is_worse", True) or text == indicator:
                problem = f"{text} is not another measure in the same direction"
            elif inds[text].get("domain") != own.get("domain"):
                problem = f"{text} measures another domain ({inds[text].get('domain')})"
        elif kind == "level":
            v = _level(text)
            problem = "a level is for a normative or felt statement" if standard not in ("normative", "felt") else None if v is not None else "a level is a number or a percentage"
            if problem is None and own.get("share") and not 0 < v < 1:
                problem = "the level of a share lies between 0 and 1"
        else:
            problem = f"no kind of reading {kind!r}"
        (refused if problem else readings).append({"kind": kind, "text": text, "rationale": why, "indicator": indicator} | ({"why": problem} if problem else {}))
    for rv in answer.get("rivals") or []:
        if not isinstance(rv, dict):
            continue
        strata = [" ".join(str(s).split()) for s in rv.get("strata") or []]
        bad = [f"{s}: {checked.get(s, 'it does not read')}" for s in strata if checked.get(s, "it does not read")]
        entry = {"name": str(rv.get("name", "")).strip()[:120] or "a rival", "strata": strata, "rationale": str(rv.get("rationale", ""))[:400]}
        if len(strata) < 2 or bad:
            refused.append(entry | {"why": "a rival needs at least two strata" if len(strata) < 2 else "; ".join(bad)})
        else:
            rivals.append(entry)
    return {"statement": " ".join(statement.split()), "words": str(answer.get("words", ""))[:1200], "readings": readings, "rivals": rivals,
            "refused": refused, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed"}


def structure(d: dict, provider=None, *, described: dict | None = None) -> dict:
    """A model's proposals of causal structure: ``{graphs, rivals, refused, by, grade: "C"}``, each graph's arrows between the data's
    columns and without a cycle, each rival's strata groups the data can judge; what does not check is refused with the reason."""
    from . import models
    provider = provider or models.Anthropic()
    described = described or ask("describe", data=d, options={"simulate": False})
    columns = set(described.get("columns") or [])
    context = ["Columns: " + ", ".join(described.get("columns") or []),
               "Provision for each domain: " + (", ".join(f"{k}: {v}" for k, v in (described.get("provision") or {}).items()) or "none named"),
               "The graph declared: " + (", ".join(f"{a} -> {b}" for a, b in described.get("graph") or []) or "none")]
    prompt = "\n\n".join(["The language of groups:\n" + GRAMMAR, vocabulary(described), "\n".join(context)])
    answer, served = provider.answer(STRUCTURE_SYSTEM, prompt, STRUCTURE_SCHEMA)
    answer = answer if isinstance(answer, dict) else {}
    graphs, rivals, refused = [], [], []
    names = {"declared"} | {g.get("name") for g in described.get("graphs") or []}
    for g in answer.get("graphs") or []:
        if not isinstance(g, dict):
            continue
        name = " ".join(str(g.get("name", "")).split())[:80] or "a graph"
        arrows = [[str(a.get("from", "")), str(a.get("to", ""))] for a in g.get("arrows") or [] if isinstance(a, dict)]
        unknown = sorted({c for a in arrows for c in a if c not in columns})
        entry = {"name": name, "arrows": arrows, "rationale": str(g.get("rationale", ""))[:400]}
        if not arrows:
            refused.append(entry | {"why": "a graph needs arrows"})
        elif unknown:
            refused.append(entry | {"why": "no column " + ", ".join(unknown)})
        elif _cyclic(arrows):
            refused.append(entry | {"why": "the arrows make a cycle"})
        elif name in names:
            refused.append(entry | {"why": f"a graph is already called {name}"})
        else:
            names.add(name), graphs.append(entry)
    ind = (described.get("indicators") or [{}])[0]
    groups = sorted({" ".join(str(s).split()) for rv in answer.get("rivals") or [] if isinstance(rv, dict) for s in rv.get("strata") or []})
    checked = _groups_that_read(d, groups, ind.get("domain") or (described.get("domains") or ["x"])[0], ind.get("name", "x"),
                                "higher" if ind.get("higher_is_worse", True) else "lower") if groups and ind else {}
    for rv in answer.get("rivals") or []:
        if not isinstance(rv, dict):
            continue
        strata = [" ".join(str(s).split()) for s in rv.get("strata") or []]
        bad = [f"{s}: {checked.get(s, 'it does not read')}" for s in strata if checked.get(s, "it does not read")]
        entry = {"name": str(rv.get("name", "")).strip()[:120] or "a rival", "strata": strata, "rationale": str(rv.get("rationale", ""))[:400]}
        (refused if len(strata) < 2 or bad else rivals).append(entry | ({"why": "a rival needs at least two strata" if len(strata) < 2 else "; ".join(bad)}
                                                                         if len(strata) < 2 or bad else {}))
    return {"graphs": graphs, "rivals": rivals, "refused": refused, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed"}


def _cyclic(arrows: list[list[str]]) -> bool:
    out: dict[str, list[str]] = {}
    for a, b in arrows:
        out.setdefault(a, []).append(b)
    state: dict[str, int] = {}

    def visit(v: str) -> bool:
        if state.get(v) == 1:
            return True
        if state.get(v) == 2:
            return False
        state[v] = 1
        if any(visit(w) for w in out.get(v, [])):
            return True
        state[v] = 2
        return False

    return any(visit(v) for v in list(out))


def with_proposals(d: dict, interpretations: list[dict] = (), structures: list[dict] = ()) -> dict:
    """The dataset with what models proposed added as proposals of grade C: readings into the point of their kind that concerns their
    indicator (a new point when none does), graphs and rivals beside the record's. The engine shows them "as proposed", apart from
    the record's readings, and a model's rival qualifies a statement but never defeats it."""
    out = dict(d)
    points = [dict(p, readings=list(p.get("readings") or []), indicators=list(p.get("indicators") or [])) for p in d.get("points") or []]
    graphs, rivals = list(d.get("graphs") or []), list(d.get("rivals") or [])
    names = {g.get("name") for g in graphs} | {"declared"}
    rival_names = {r.get("name") for r in rivals}

    def concerns(p: dict, kind: str, indicator: str) -> bool:
        if p.get("kind", "reference") != kind:
            return False
        if kind == "indicator":
            return any(r.get("text") == indicator for r in p["readings"])
        return not p["indicators"] or indicator in p["indicators"]

    for it in interpretations:
        by = it.get("by", "a model")
        for r in it.get("readings") or []:
            kind, ind = r["kind"], r["indicator"]
            home = next((p for p in points if concerns(p, kind, ind)), None)
            if kind == "indicator" and any(p is not home and p.get("kind") == "indicator" and any(x.get("text") == r["text"] for x in p["readings"]) for p in points):
                continue  # a measure another point already reads: two points of one kind may not concern the same statements
            if home is None:
                home = {"name": f"{kind} of {ind} (proposed)", "about": "readings a model proposed", "kind": kind, "indicators": [] if kind == "indicator" else [ind],
                        "readings": [{"text": ind, "source": "analyst", "by": "the statement"}] if kind == "indicator" else []}
                points.append(home)
            if all(x.get("text") != r["text"] for x in home["readings"]):
                home["readings"].append({"text": r["text"], "source": "proposal", "grade": "C", "by": by, "rationale": r.get("rationale", "")})
        for rv in it.get("rivals") or []:
            if rv["name"] not in rival_names:
                rival_names.add(rv["name"])
                rivals.append({"name": rv["name"], "about": rv.get("rationale", ""), "source": "proposal", "grade": "C", "by": by, "strata": rv["strata"]})
    for st in structures:
        by = st.get("by", "a model")
        for g in st.get("graphs") or []:
            if g["name"] not in names:
                names.add(g["name"])
                graphs.append({"name": g["name"], "source": "proposal", "grade": "C", "by": by, "rationale": g.get("rationale", ""), "arrows": g["arrows"]})
        for rv in st.get("rivals") or []:
            if rv["name"] not in rival_names:
                rival_names.add(rv["name"])
                rivals.append({"name": rv["name"], "about": rv.get("rationale", ""), "source": "proposal", "grade": "C", "by": by, "strata": rv["strata"]})
    out["points"], out["graphs"], out["rivals"] = points, graphs, rivals
    return out


# ---- provenance ----

def provenance(report: dict, sources: dict | None = None) -> dict:
    """The search's lineage as W3C PROV-JSON (PROV-DM): every statement made an entity, generated by an activity (its operator in its
    generation) and derived from its parents; every activity associated with its agent: the search for its own operators, a person
    or a model for proposals (``model:<role>`` a software agent whose statements are of grade C). ``sources`` may name the agents
    further ({source: {label, type}})."""
    sources = sources or {}
    own = set(report.get("operators") or [])
    texts: dict[int, str] = {}
    for m in (report.get("population") or []) + (report.get("archive") or []) + (report.get("retired") or []) + (report.get("admissions") or []):
        if "ticket" in m and m.get("text"):
            texts[int(m["ticket"])] = m["text"]
    doc = {"prefix": {"needs": "https://noetarium.org/ns/needs#", "prov": "http://www.w3.org/ns/prov#"},
           "entity": {}, "activity": {}, "agent": {"needs:search": {"prov:type": "prov:SoftwareAgent", "prov:label": "the needs search"}},
           "wasGeneratedBy": {}, "wasDerivedFrom": {}, "wasAssociatedWith": {}}
    agents_of: dict[str, str] = {}
    for ticket, generation, operation, parents, flags, _objectives in report.get("lineage") or []:
        t = int(ticket)
        e = f"needs:s{t}"
        doc["entity"][e] = {"prov:label": texts.get(t, ""), "needs:generation": generation, "needs:survived": bool(int(flags) & 2),
                            "needs:admitted": bool(int(flags) & 4), "needs:failed": bool(int(flags) & 1)}
        a = f"needs:g{generation}-{re.sub(r'[^A-Za-z0-9_.-]', '_', operation)}"
        if a not in doc["activity"]:
            doc["activity"][a] = {"prov:label": f"{operation}, generation {generation}"}
            if operation in own:
                agent = "needs:search"
            else:
                agent = agents_of.setdefault(operation, f"needs:agent-{len(agents_of) + 1}")
                info = sources.get(operation, {})
                model = operation.startswith("model:")
                doc["agent"].setdefault(agent, {"prov:type": info.get("type", "prov:SoftwareAgent" if model else "prov:Person"),
                                                "prov:label": info.get("label", operation)} | ({"needs:grade": "C"} if model else {}))
            doc["wasAssociatedWith"][f"_:w{len(doc['wasAssociatedWith'])}"] = {"prov:activity": a, "prov:agent": agent}
        doc["wasGeneratedBy"][f"_:g{t}"] = {"prov:entity": e, "prov:activity": a}
        for p in parents or []:
            doc["wasDerivedFrom"][f"_:d{t}-{int(p)}"] = {"prov:generatedEntity": e, "prov:usedEntity": f"needs:s{int(p)}", "prov:activity": a}
    return doc
