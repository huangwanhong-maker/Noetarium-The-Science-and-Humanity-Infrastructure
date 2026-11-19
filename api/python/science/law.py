"""Law, for scripts in Python and the works' servers (ADR 0121): schemes stated clause by clause, matters read through them, and
candidates of evidence recognised, asked of the engine a work has built.

The computations are the law domain's C++ with apodeixis, tekmerion and archeion, composed in ``api/cpp/science/law.hpp``, which
documents every operation and the two documents they take, a scheme (``noetarium.scheme/1``) and a matter
(``noetarium.matter/1``):

    from science import law
    law.ask("scheme", scheme=scheme)                        # its faults, and what it holds
    law.ask("evaluate", scheme=scheme, matter=matter)        # evidence, facts, authority, each issue's label and plan of proof

The engine is a program built by a work: the Law Studio builds ``data/cache/engines/law-studio/law/engine``;
``NOETARIUM_LAW_ENGINE`` names another.

A scheme's provisions may be kept without their words, which are fetched from the official texts into the data library's cache
(``data/products/legal_texts.py``); ``with_texts`` fills them in before a scheme is asked, so that its quotations are checked
against the words of the text and not against memory.
"""

from __future__ import annotations

import json
from pathlib import Path

from .engines import ROOT, Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("law-studio/law", env="NOETARIUM_LAW_ENGINE")
TEXTS = ROOT / "data" / "cache" / "products" / "legal-texts"


def ask(op: str, timeout: float = 300.0, **request) -> dict:
    """One request to the law engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def instrument_text(instrument: str, folder: Path = TEXTS) -> dict | None:
    """An instrument's provisions as fetched from its official text ({provision id: {label, text}}), or None if not fetched."""
    path = folder / f"{instrument}.json"
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    return {p["id"]: p for p in data.get("provisions", [])}


READINGS_SYSTEM = (
    "You assist counsel. A matter is to be decided under a body of law stated clause by clause. Some of its terms are open: each "
    "admits the readings listed, and a conclusion may follow under some readings and not others. For each open term, propose the "
    "reading you think the law should be given in this matter, or \"none\" where the matter does not let you choose, and give your "
    "rationale in one to three sentences: the words of the clause, its purpose, the system it belongs to, the holdings of the courts "
    "given to you. Rely only on what you are given and on law you can name exactly; never invent a case, a citation or a quotation. "
    "Say plainly where you are unsure. Your answer is a proposal: it stands beside the parties' arguments and the courts' holdings, "
    "and fixes nothing until counsel makes an argument for it."
)


def readings_prompt(scheme: dict, matter: dict, evaluation: dict) -> str:
    """What a model is told of a matter to propose readings: the questions, the clauses the rules cite, the open terms, the holdings
    weighed for the forum, and the facts as they entered the case."""
    provisions = {p["id"]: (ins, p) for ins in scheme.get("instruments", []) for p in ins.get("provisions", [])}
    lines = [f"The matter: {matter.get('title') or matter.get('id', '')}" + (f", as of {matter['date']}" if matter.get("date") else "")]
    forum = matter.get("forum") or {}
    if forum.get("name"):
        lines.append(f"The forum: {forum['name']}.")
    lines += ["", "The questions:"] + [f"- {a['question']}: now {a['class']}" for a in evaluation.get("issues", [])]
    cited = []
    for kind in ("rules", "exceptions", "counts"):
        for x in scheme.get(kind, []):
            for pid in x.get("cites", []):
                if pid in provisions and pid not in cited:
                    cited.append(pid)
    if cited:
        lines += ["", "The clauses the rules state:"]
        for pid in cited:
            ins, p = provisions[pid]
            lines.append(f"- {ins.get('citation') or ins.get('title')}, {p.get('label', pid)}: {p.get('text') or '(its words not fetched)'}")
    lines += ["", "The open terms:"]
    for t in evaluation.get("terms", []):
        rs = "; ".join(f"{r['name']}" + (f" ({r['gloss']})" if r.get("gloss") else "") for r in t["readings"])
        lines.append(f"- {t['term']}" + (f", {t['gloss']}" if t.get("gloss") else "") + f": {rs}")
    held = [h for h in (evaluation.get("authority") or {}).get("assessed", []) if h["force"] != "left out"]
    if held:
        lines += ["", "The holdings, as they weigh for the forum:"] + [f"- {h['name'] or h['holding']} ({h['citation']}): {h['term']} read as {h['reading']}; {h['force']}: {h['reason']}" for h in held]
    lines += ["", "The facts as they entered the case:"] + [f"- {f['id']}: {f['used']}" for f in evaluation.get("facts", [])]
    lines += ["", "Propose a reading of each open term, or none, with your rationale."]
    return "\n".join(lines)


def propose_readings(scheme: dict, matter: dict, evaluation: dict, provider=None, *, name: str = "a language model") -> dict:
    """A model's proposal for the open terms of an evaluated matter, accepted term by term and reading by reading as
    science.interpret accepts one, or refused: a proposal of grade C, never an argument."""
    from . import interpret
    points = [{"name": t["term"], "readings": [r["name"] for r in t["readings"]]} for t in evaluation.get("terms", [])]
    if not points:
        raise interpret.Refusal("the matter's questions turn on no open term")
    provider = provider or interpret.Anthropic()
    data, served = provider.answer(READINGS_SYSTEM, readings_prompt(scheme, matter, evaluation), interpret.schema(points))
    return interpret.accept(points, data, by=f"{provider.name}, {served}", name=name)


def with_texts(scheme: dict, folder: Path = TEXTS) -> tuple[dict, list[str]]:
    """The scheme with each provision's words filled from its instrument's fetched text where the scheme gives none, and the
    instruments whose text has not been fetched."""
    out = json.loads(json.dumps(scheme))
    missing = []
    for ins in out.get("instruments", []):
        fetched = instrument_text(ins["id"], folder)
        if fetched is None:
            if any(not p.get("text") for p in ins.get("provisions", [])):   # one that carries its own words needs no fetching
                missing.append(ins["id"])
            continue
        listed, filled = {p["id"] for p in ins.get("provisions", [])}, []
        for p in ins.get("provisions", []):
            got = fetched.get(p["id"])
            if got is None:
                filled.append(p)   # the scheme names a provision the text does not have: its check will say so (no words)
                continue
            # The provisions it stands under come with it, so that a clause is read in its place.
            chain, up = [], got.get("parent", "")
            while up and up not in listed and up in fetched:
                chain.append(fetched[up])
                listed.add(up)
                up = fetched[up].get("parent", "")
            for a in reversed(chain):
                filled.append({"id": a["id"], "label": a.get("label", ""), "text": a.get("text", ""), "parent": a.get("parent", ""), **({"title": a["title"]} if a.get("title") else {})})
            p.setdefault("label", got.get("label", ""))
            p.setdefault("parent", got.get("parent", ""))
            if got.get("title"):
                p.setdefault("title", got["title"])
            if not p.get("text"):
                p["text"] = got.get("text", "")
            filled.append(p)
        ins["provisions"] = filled
    return out, missing


def judgments(scheme: dict, folder: Path = TEXTS) -> dict:
    """The text of the judgment each holding names (its ``judgment``, a fetched text's id), by the holding's id: what a holding's
    passage is looked for in."""
    out = {}
    for h in scheme.get("holdings", []):
        if not h.get("judgment"):
            continue
        try:
            data = json.loads((folder / f"{h['judgment']}.json").read_text(encoding="utf-8"))
        except (OSError, ValueError):
            continue
        if data.get("text"):
            out[h["id"]] = data["text"]
    return out


# ---- Theories of the case (ADR 0127) -------------------------------------------------------------------------------------------


def theories(scheme: dict | None, matter: dict, *, state: dict | None = None, generations: int | None = None, proposals: list | None = None,
             requests: dict | None = None, now: str | None = None, timeout: float = 600.0) -> dict:
    """The search for theories of the case run a few generations from its state (or begun): the theories kept, each tested once on
    what the matter holds back, the issues over them, whether each person could have been at the scene, and the state to give back.
    ``api/cpp/science/theories.hpp`` documents the matter's parts it reads."""
    request = {"matter": matter}
    if scheme is not None:
        request["scheme"] = scheme
    if state is not None:
        request["state"] = state
    if generations is not None:
        request["generations"] = int(generations)
    if proposals:
        request["proposals"] = proposals
    if requests:
        request["requests"] = requests
    if now:
        request["now"] = now
    return ask("theories", timeout=timeout, **request)


def assess_theory(scheme: dict | None, matter: dict, text: str) -> dict:
    """One theory written as lines (instants as clock times), assessed against the evidence and read under the law."""
    request = {"matter": matter, "text": text}
    if scheme is not None:
        request["scheme"] = scheme
    return ask("assess", **request)


THEORY_FORM = """A theory is written one line each:
  event <kind> [<role>=<person> ...] at <x> <y> when <HH:MM:SS>   an event: its kind, who had each role (or none), where (metres east
                                                                and north of the scene's reference point) and when
  doubt <item>                                                   an item of evidence the theory holds to be mistaken
  believe <item> when <HH:MM:SS>                                 a sighting the theory places at that instant within its span
Items not named are believed. Write nothing else on a line."""

THEORIES_SYSTEM = """You help counsel imagine what may have happened. You are given the evidence of a matter as it stands (where and \
when each item places someone or something, and how credible it is), the people, the kinds of event and what a complete account must \
contain. You write theories of the case in a small formal language; a search assesses them, keeps the coherent ones and weighs them. \
Propose accounts a careful person would consider, alternatives above all: where the evidence leaves room for another hand, another \
kind of event or another time, write that account. A person must be able to travel between the places a theory puts them at the \
speed given. Never invent evidence, people, kinds or items: use only those named. You never say which account is true or how likely \
it is: the search and the court weigh them."""

THEORIES_SCHEMA = {
    "type": "object",
    "properties": {
        "answers": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "request": {"type": "integer"},
                    "theories": {"type": "array", "items": {"type": "object", "properties": {"text": {"type": "string"}, "why": {"type": "string"}},
                                                             "required": ["text", "why"], "additionalProperties": False}},
                },
                "required": ["request", "theories"],
                "additionalProperties": False,
            },
        }
    },
    "required": ["answers"],
    "additionalProperties": False,
}

ROLE_TASK = {
    "seed": "write {count} new theories, as different from one another as the evidence allows",
    "mutate": "write one variation of the theory below: another hand, time or place, or another item doubted",
    "cross": "write one theory that joins the two below where they can be joined",
    "reflect": "write one refinement of the kept theory below, from what its assessment says",
}


def evidence_words(matter: dict, report: dict | None = None) -> str:
    """The evidence as the search sees it, in words: never what the matter holds back."""
    lines = [f"The matter: {matter.get('title') or matter.get('id', '')}"]
    scene = (matter.get("place") or {}).get("scene") or {}
    if scene:
        lines.append(f"The scene: {scene.get('name', 'the scene')}, at ({scene.get('x', 0)}, {scene.get('y', 0)}) m.")
    if matter.get("epoch"):
        lines.append(f"Times are clock times on the day of {matter['epoch']}.")
    lines += ["", "The people (and the most they are taken to move, in metres a second):"]
    lines += [f"- {a['id']}: {a.get('name', a['id'])}, {a.get('speed', 1.4)} m/s" + ("" if a.get("identified", True) else " (known only by what was seen of them)")
              for a in matter.get("agents", [])]
    lines += ["", "The kinds of event:"] + [f"- {k['id']}: {k.get('gloss', '')}" + (f"; roles: {', '.join(k['roles'])}" if k.get("roles") else "; no one's hand")
                                           for k in matter.get("kinds", [])]
    if matter.get("story"):
        lines += ["", "A complete account has:"] + [f"- {s['name']} ({s.get('gloss', '')}): one of {', '.join(s['kinds'])}" for s in matter["story"]]
    if matter.get("rooms"):
        lines += ["", "Where and when events may be:"] + [f"- {r['kind']}: within {r.get('radius', 0)} m of ({r.get('x', 0)}, {r.get('y', 0)}), from {r['from']} to {r['until']}"
                                                         for r in matter["rooms"]]
    lines += ["", "The evidence (credibility: the chance that what it says is so):"]
    for i in matter.get("located", []):
        if i.get("held_back"):
            continue
        when = i.get("at") or f"{i.get('from')} to {i.get('until', i.get('from'))}"
        what = {"sighting": f"places {i.get('agent')}", "event": f"reports {i.get('kind')}" + (f" by {i['agent']}" if i.get("agent") else ""),
                "absence": f"reports no {i.get('kind') or i.get('agent')}",
                "interval": f"says {i.get('after')} came {i.get('least')} to {i.get('most')} s after {i.get('before')}"}.get(i.get("says", "sighting"), "")
        lines.append(f"- {i['id']} ({i.get('modality', 'item')}, {i.get('source', '')}): {what} within {i.get('radius', 0)} m of ({i.get('x', 0)}, {i.get('y', 0)}) at {when}; "
                     f"credibility {i.get('credibility', 0.5)}")
    if report and report.get("kept"):
        lines += ["", "Theories kept so far, best first:"] + [f"- #{t['serial']} ({t['central']}), score {t['score']:.3g}:\n{t['text']}" for t in report["kept"][:6]]
    return "\n".join(lines)


def theories_prompt(matter: dict, requests: list[dict], report: dict | None = None, description: str | None = None) -> str:
    """The prompt for a round of requests: the form of a theory, the evidence as the search sees it, the theories kept, and each
    request with its role and the theories it starts from, each with its assessment."""
    parts = [THEORY_FORM, evidence_words(matter, report)]
    if description:
        parts.append("Counsel asks you to consider:\n" + description.strip()[:2000])
    lines = []
    for i, q in enumerate(requests):
        task = ROLE_TASK.get(q.get("role"), ROLE_TASK["seed"]).format(count=q.get("count", 1))
        lines.append(f"Request {i} ({q.get('role')}): {task}")
        for t, note in zip(q.get("texts") or [], q.get("notes") or []):
            lines.append("    " + t.strip().replace("\n", "\n    ") + f"\n      ({note})")
    parts.append("Requests:\n" + "\n".join(lines))
    return "\n\n".join(parts)


def fulfil_theories(scheme: dict | None, matter: dict, requests: list[dict], provider=None, *, report: dict | None = None,
                    description: str | None = None) -> dict:
    """A model's answers to the search's requests, in one call: ``{proposals, read, refused, by, grade}``. Each theory is read by the
    engine; one that does not read is refused with the reason and never repaired. Proposals enter as ``model:<role>``, grade C."""
    from . import models
    provider = provider or models.Anthropic()
    prompt = theories_prompt(matter, requests, report, description)
    answer, served = provider.answer(THEORIES_SYSTEM, prompt, THEORIES_SCHEMA)
    kept, refused, proposals = [], [], []
    for a in (answer.get("answers") if isinstance(answer, dict) else None) or []:
        if not isinstance(a, dict) or not isinstance(a.get("request"), int) or not 0 <= a["request"] < len(requests):
            continue
        q = requests[a["request"]]
        for x in [x for x in a.get("theories") or [] if isinstance(x, dict)][: max(1, int(q.get("count", 1)))]:
            text = str(x.get("text", "")).strip()
            entry = {"text": text, "role": q.get("role"), "parents": q.get("parents") or [], "why": str(x.get("why", ""))[:600]}
            read = assess_theory(scheme, matter, text) if text else {"read": False, "errors": ["empty"]}
            if read.get("read"):
                kept.append(entry | {"coherent": read.get("coherent"), "central": read.get("central")})
                proposals.append({"text": text, "source": f"model:{q.get('role')}", "parents": q.get("parents") or []})
            else:
                refused.append(entry | {"errors": read.get("errors")})
    return {"proposals": proposals, "read": kept, "refused": refused, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed",
            "requests": len(requests), "prompt_chars": len(prompt)}


# ---- Theories of the law (ADR 0133) --------------------------------------------------------------------------------------------


def doctrines(scheme: dict, matter: dict, *, state: dict | None = None, generations: int | None = None, proposals: list | None = None,
              requests: dict | None = None, now: str | None = None, timeout: float = 600.0) -> dict:
    """The search for theories of the law run a few generations from its state (or begun): the doctrines kept by kind (the readings
    of the open terms and the ratios of precedents whose reasons were unstated), each judged by fit and justification and tested once
    on the decisions held back; the questions' standings over them; the exact best of each kind when the doctrines are few; and the
    state to give back. ``api/cpp/science/doctrine.hpp`` documents the scheme's and the matter's parts it reads."""
    request = {"scheme": scheme, "matter": matter}
    if state is not None:
        request["state"] = state
    if generations is not None:
        request["generations"] = int(generations)
    if proposals:
        request["proposals"] = proposals
    if requests:
        request["requests"] = requests
    if now:
        request["now"] = now
    return ask("doctrines", timeout=timeout, **request)


def assess_doctrine(scheme: dict, matter: dict, text: str) -> dict:
    """One doctrine written as lines ('reading <term> = <reading>', 'ratio <fact>/<case>: <factor>, ...'), read and assessed."""
    return ask("assess_doctrine", scheme=scheme, matter=matter, text=text)


DOCTRINE_FORM = """A doctrine is written one line each:
  reading <term> = <reading>                    how an open term of the rules is read
  ratio <fact>/<case>: <factor>, <factor>, ...  the reason a precedent is read as decided on: some of the factors it had for its outcome
Terms and cases not named keep their present reading or ratio. Write nothing else on a line."""

DOCTRINES_SYSTEM = """You help counsel read the law of a matter. You are given its rules with their open terms and the readings each \
admits, the decided cases that bear on those readings (each with its facts, what it held and the force it has for this court), the \
binding readings that fix some terms, the precedents decided on a fact by factors whose courts left their reasons unstated, and the \
purposes declared for weighing readings. You write doctrines in a small formal language: a reading of each open term and a ratio for \
each precedent whose reason is to be chosen. A search assesses them by how well they fit the decided cases, the binding readings and \
one another, and by the declared purposes, and keeps the best of each kind. Propose doctrines a careful lawyer would argue, and above \
all the best case for each side. Never invent terms, readings, cases or factors: use only those named. You never say which doctrine is \
right or how a court will decide: the search and the court weigh them."""

DOCTRINES_SCHEMA = {
    "type": "object",
    "properties": {
        "answers": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "request": {"type": "integer"},
                    "doctrines": {"type": "array", "items": {"type": "object", "properties": {"text": {"type": "string"}, "why": {"type": "string"}},
                                                              "required": ["text", "why"], "additionalProperties": False}},
                },
                "required": ["request", "doctrines"],
                "additionalProperties": False,
            },
        }
    },
    "required": ["answers"],
    "additionalProperties": False,
}

DOCTRINE_TASK = {
    "seed": "write {count} new doctrines, as different from one another as the law allows, the best case for each side among them",
    "mutate": "write one variation of the doctrine below: another reading of a term, or another ratio for a precedent",
    "cross": "write one doctrine that joins the two below where they can be joined",
    "reflect": "write one refinement of the kept doctrine below, from what its assessment says it gets wrong",
}


def law_words(problem: dict, report: dict | None = None) -> str:
    """The law of the matter as the search sees it, in words: never a decision held back."""
    lines = ["The questions: " + ", ".join(problem.get("questions", []))]
    lines += ["", "The open terms and their readings:"]
    for pt in problem.get("points", []):
        lines.append(f"- {pt['name']}" + (f" ({pt['gloss']})" if pt.get("gloss") else "") + ": "
                     + "; ".join(r["name"] + (f" ({r['gloss']})" if r.get("gloss") else "") for r in pt.get("readings", [])))
    if problem.get("admissible"):
        lines.append("Readings the binding holdings and arguments leave admissible: " + "; ".join(problem["admissible"]))
    if problem.get("decided"):
        lines += ["", "Decided cases (their facts, the question, what was held, and their force here):"]
        for d in problem["decided"]:
            lines.append(f"- {d['id']} {d.get('name', '')} ({d.get('court')}, {d.get('date')}): facts {', '.join(d.get('facts', []))}; "
                         f"{d['question']} {'held' if d.get('held') else 'not held'}; {d.get('force')}")
    for b in problem.get("bases", []):
        lines += ["", f"Precedents deciding whether {b['fact']} (sides: {b['sides'][0]} / {b['sides'][1]}):"]
        lines += [f"  factor {f['id']} favours {f['side']}" + (f": {f['text']}" if f.get("text") else "") for f in b.get("factors", [])]
        for c in b.get("cases", []):
            lines.append(f"- {c['name']}: factors {', '.join(c['facts'])}; decided for {c['outcome']}; "
                         + (f"reason stated: {', '.join(c['reason'])}" if c.get("stated") else f"reason to be chosen among {', '.join(c['for_outcome'])}"))
        lines.append(f"  the matter's factors: {', '.join(b.get('situation', []))}")
    if problem.get("purposes"):
        lines += ["", "Declared purposes (the worth of each reading, 0 to 1):"]
        for pu in problem["purposes"]:
            lines.append(f"- {pu['name']} (weight {pu['weight']}): " + "; ".join(f"{t}: {w}" for t, w in pu.get("worth", {}).items()))
    if report and report.get("kept"):
        lines += ["", "Doctrines kept so far, best first:"] + [f"- #{d['serial']} ({d['kind_words']}), score {d['assessment']['score']:.3g}:\n{d['text']}" for d in report["kept"][:6]]
    return "\n".join(lines)


def doctrines_prompt(problem: dict, requests: list[dict], report: dict | None = None, description: str | None = None) -> str:
    """The prompt for a round of requests: the form of a doctrine, the law as the search sees it, the doctrines kept, and each request
    with its role and the doctrines it starts from, each with its assessment."""
    parts = [DOCTRINE_FORM, law_words(problem, report)]
    if description:
        parts.append("Counsel asks you to consider:\n" + description.strip()[:2000])
    lines = []
    for i, q in enumerate(requests):
        task = DOCTRINE_TASK.get(q.get("role"), DOCTRINE_TASK["seed"]).format(count=q.get("count", 1))
        lines.append(f"Request {i} ({q.get('role')}): {task}")
        for t, note in zip(q.get("texts") or [], q.get("notes") or []):
            lines.append("    " + t.strip().replace("\n", "\n    ") + f"\n      ({note})")
    parts.append("Requests:\n" + "\n".join(lines))
    return "\n\n".join(parts)


def fulfil_doctrines(scheme: dict, matter: dict, requests: list[dict], provider=None, *, report: dict | None = None, description: str | None = None) -> dict:
    """A model's answers to the search's requests for doctrines, in one call: ``{proposals, read, refused, by, grade}``. Each doctrine
    is read by the engine; one that does not read is refused with the reason, never repaired. Proposals enter as ``model:<role>``,
    grade C. The model is shown the law as the search sees it (``report['problem']``), never a decision held back."""
    from . import models
    provider = provider or models.Anthropic()
    problem = (report or {}).get("problem") or doctrines(scheme, matter, generations=0)["problem"]
    prompt = doctrines_prompt(problem, requests, report, description)
    answer, served = provider.answer(DOCTRINES_SYSTEM, prompt, DOCTRINES_SCHEMA)
    kept, refused, proposals = [], [], []
    for a in (answer.get("answers") if isinstance(answer, dict) else None) or []:
        if not isinstance(a, dict) or not isinstance(a.get("request"), int) or not 0 <= a["request"] < len(requests):
            continue
        q = requests[a["request"]]
        for x in [x for x in a.get("doctrines") or [] if isinstance(x, dict)][: max(1, int(q.get("count", 1)))]:
            text = str(x.get("text", "")).strip()
            entry = {"text": text, "role": q.get("role"), "parents": q.get("parents") or [], "why": str(x.get("why", ""))[:600]}
            read = assess_doctrine(scheme, matter, text) if text else {"read": False, "errors": ["empty"]}
            if read.get("read"):
                kept.append(entry | {"kind_words": read.get("kind_words"), "score": read.get("assessment", {}).get("score")})
                proposals.append({"text": text, "source": f"model:{q.get('role')}", "parents": q.get("parents") or []})
            else:
                refused.append(entry | {"errors": read.get("errors")})
    return {"proposals": proposals, "read": kept, "refused": refused, "by": f"{provider.name}, {served}", "grade": "C", "status": "proposed",
            "requests": len(requests), "prompt_chars": len(prompt)}
