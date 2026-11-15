"""Evidence recognised in documents by a language model (ADR 0121): a model reads a document against the facts a matter turns on,
and proposes the passages that bear on them; each passage is looked for in the document, and one that is not there is refused.

tekmerion grades evidence by who says it (``foundations/tekmerion/recognition``): A, a structured source; B, a text with the passage
that says it; C, a model or a memory, nothing cited. A model is never the source here. What it proposes is a candidate whose
source is the document (and whoever speaks in it), whose passage must occur in the document's text up to runs of whitespace, and
whose status is ``proposed`` until a person reads it and accepts or rejects it. The law engine reads a proposed candidate at grade C
whatever it cites, and enters only accepted ones into a matter's chart of evidence (``api/cpp/science/law.hpp``).

    from science import recognise
    found = recognise.propose({"id": "minutes", "title": "Minutes of 3 May", "text": text},
                              [{"id": "transfer_to_third_country", "text": "the data are sent to a country outside the EEA"}, ...])
    found["candidates"]   # each: fact, negated, passage, at (byte offset), speaker, note, status "proposed", by
    found["refused"]      # each a proposal whose passage is not in the document, with the reason

Long documents are read in windows that overlap, each window marked for the provider's cache so that the facts may be asked again
without reading it twice. Nothing here repairs a quotation: a passage that is close to the text but not in it is refused, since a
quotation that is almost right is the failure that matters most in practice (hallucinated citations: Mata v. Avianca, 678 F. Supp.
3d 443, S.D.N.Y. 2023; Magesh et al. 2025, Journal of Empirical Legal Studies, on research tools that still invent).
"""

from __future__ import annotations

import time

from .models import Anthropic, Refusal, Stub  # noqa: F401

WINDOW = 120_000      # characters a model is given at once
OVERLAP = 2_000

SYSTEM = (
    "You read a document for counsel preparing a matter. You are given the facts the matter turns on, each with an id and what it "
    "means. Find the passages of the document that say a fact holds, or that it does not hold. For each, give the fact's id, whether "
    "the passage denies it, the passage copied exactly as the document has it (a sentence or two, never paraphrased, never joined "
    "from separate places), who says it in the document if anyone is named, and a short note on why it bears on the fact. Give only "
    "passages you can copy from the document; if the document says nothing about a fact, give nothing for it. Your answer is a "
    "proposal: every passage will be looked for in the document, and a person will read each before it counts as evidence."
)


def schema(facts: list[dict]) -> dict:
    """The JSON schema of an answer: candidates, each for one of the declared facts."""
    return {
        "type": "object",
        "properties": {
            "candidates": {
                "type": "array",
                "items": {
                    "type": "object",
                    "properties": {
                        "fact": {"type": "string", "enum": [f["id"] for f in facts]},
                        "negated": {"type": "boolean"},
                        "passage": {"type": "string"},
                        "speaker": {"type": "string"},
                        "note": {"type": "string"},
                    },
                    "required": ["fact", "negated", "passage", "speaker", "note"],
                    "additionalProperties": False,
                },
            }
        },
        "required": ["candidates"],
        "additionalProperties": False,
    }


def prompt(facts: list[dict], title: str, part: str = "") -> str:
    lines = [f"The document above is: {title}{part}.", "", "The facts the matter turns on:"]
    lines += [f"- {f['id']}: {f.get('text') or f.get('gloss') or f['id']}" for f in facts]
    lines += ["", "Give the passages that bear on these facts, each copied exactly."]
    return "\n".join(lines)


def find(text: str, passage: str) -> int | None:
    """Where the passage occurs in the text up to runs of whitespace (as tekmerion's verify_passage reads it): the offset in the
    text's UTF-8 bytes, or None."""
    def squeeze(s: str) -> tuple[str, list[int]]:
        out, origin, space = [], [], False
        offset = 0
        for ch in s:
            ws = ch in " \t\n\r"
            if ws:
                if not space and out:
                    out.append(" "), origin.append(offset)
                space = True
            else:
                out.append(ch), origin.append(offset)
                space = False
            offset += len(ch.encode("utf-8"))
        if out and out[-1] == " ":
            out.pop(), origin.pop()
        return "".join(out), origin

    t, origin = squeeze(text)
    p, _ = squeeze(passage)
    if not p:
        return None
    at = t.find(p)
    return None if at < 0 else origin[at]


def windows(text: str, size: int = WINDOW, overlap: int = OVERLAP) -> list[tuple[int, str]]:
    if len(text) <= size:
        return [(0, text)]
    out, start = [], 0
    while start < len(text):
        out.append((start, text[start:start + size]))
        if start + size >= len(text):
            break
        start += size - overlap
    return out


def propose(document: dict, facts: list[dict], provider=None, *, name: str = "a language model") -> dict:
    """A model's candidates for the facts from one document: each passage found in the document, or refused."""
    if not facts:
        raise Refusal("the matter declares no fact to look for")
    text = document.get("text") or ""
    if not text.strip():
        raise Refusal("the document has no text to read (a scanned image must be transcribed first)")
    provider = provider or Anthropic()
    accepted, refused, seen = [], [], set()
    served = ""
    parts = windows(text)
    for k, (start, part) in enumerate(parts):
        where = "" if len(parts) == 1 else f", part {k + 1} of {len(parts)}"
        data, served = provider.answer(SYSTEM, prompt(facts, document.get("title") or document.get("id", "the document"), where), schema(facts), cached=part)
        known = {f["id"] for f in facts}
        for c in (data.get("candidates") or []) if isinstance(data, dict) else []:
            fact, passage = c.get("fact"), str(c.get("passage", "")).strip()
            if fact not in known:
                refused.append({"fact": fact, "passage": passage, "reason": f"it names {fact}, which the matter does not declare"})
                continue
            at = find(text, passage)
            if at is None:
                refused.append({"fact": fact, "passage": passage, "reason": "the passage is not in the document, even with its whitespace evened out"})
                continue
            key = (fact, bool(c.get("negated")), at)
            if key in seen:
                continue
            seen.add(key)
            accepted.append({"fact": fact, "negated": bool(c.get("negated")), "passage": passage, "at": at,
                             "speaker": str(c.get("speaker", "")).strip(), "note": str(c.get("note", "")).strip()})
    by = f"{provider.name}, {served}"
    asked = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    candidates = [{"id": f"{document.get('id', 'doc')}:{n + 1}", "proposition": c["fact"], "negated": c["negated"], "document": document.get("id", ""),
                   "passage": c["passage"], "at": c["at"], "source": c["speaker"] or (document.get("title") or document.get("id", "")),
                   "note": c["note"], "grade": "B", "status": "proposed", "by": by, "proposed_by": name, "asked_at": asked} for n, c in enumerate(accepted)]
    return {"candidates": candidates, "refused": refused, "by": by, "asked_at": asked, "windows": len(parts)}
