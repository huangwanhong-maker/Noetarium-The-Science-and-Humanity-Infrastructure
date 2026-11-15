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
