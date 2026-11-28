"""The work of writing a report together (the design record of 2026-11-26, a book written by communities): the call, the outline and
its amendments, the sections and their stages, the bodies of every delivery (choices of series, checks, analyses, replications,
versions, reviews), the audits, the petitions to extend the bench and the tools admitted, kept by the work beside the communities'
chronicles. A chronicle reads envelopes, never bodies; what counts as a well-formed body is here, so that every agent and every page
reads the same standard.

A section passes through stages: **data** (the commons chooses its series and checks the choice), **analysis** (the guild runs and
replicates a plan of tools), **draft** (the cooperative writes it), **review** (the partnership's reviewers under the regime in force
score it and vote), **adopted** (or returned for **revision**, or **reopened** by an audit). The book is adopted by the council.
"""

from __future__ import annotations

import hashlib
import json
import re

STAGES = ("data", "analysis", "draft", "review", "revision", "corrections", "adopted")
LEVELS = ("fails", "partly", "meets")
SCORECARD = [
    ("S1", "accuracy", "every number in the text is a fact of a cited result, written as the result gives it"),
    ("S2", "answer", "the section answers its question for the targets it names"),
    ("S3", "inference", "every finding follows from the results, and every proposal from a finding"),
    ("S4", "limits", "the section says what its data do not cover: years, countries, measures, readings of a target"),
    ("S5", "craft", "the section is clear, ordered and readable by an informed reader"),
]
REGIMES = {
    "editor": "the secretariat's editor reviews each section alone",
    "peers": "two reviewers of the guild, of different origins and of neither origin that analysed or wrote the section, review each section",
    "panel": "one reviewer of each member community reviews each section",
    "forum": "the forum's reviewers, those the goals concern, review each section",
}
RULES = ("any one", "majority", "two thirds", "unanimity")
UNITS = ("person", "origin", "community")
KINDS_OF_CHAPTER = ("overview", "goal", "region", "theme", "proposals")
WORDS_PER_PAGE = 450          # half the density first declared (950), at the user's word to stay within the budget: sections drafted after round 91
MOST_WORDS = 5000             # the longest section a writer is asked for in one delivery
AUDIT_SHARE = 3               # the auditor checks one claim in three of each adopted version, at least two


def key(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def digest(value) -> str:
    return key(json.dumps(value, sort_keys=True, ensure_ascii=False, default=str))[:16]


def slug(text: str) -> str:
    return re.sub(r"[^a-z0-9]+", "-", text.lower()).strip("-")[:40]


class Book:
    def __init__(self, data: dict | None = None):
        d = data or {}
        self.call: dict = d.get("call", {})                 # the secretariat's call
        self.outline: dict = d.get("outline", {})           # {title, chapters: [{id, kind, title, question, targets, pages, sections: [sid]}]}
        self.zero: dict = d.get("zero", {})                 # the editor's zero draft, as proposed
        self.amendments: dict = d.get("amendments", {})     # proposal id -> {by, community, change, why, status}
        self.approvals: dict = d.get("approvals", {})       # community -> [chapter ids it approves for pages]
        self.allotment: dict = d.get("allotment", {})       # how the pages were allotted: rule, funded, pages by chapter
        self.sections: dict = d.get("sections", {})         # sid -> {id, chapter, title, question, targets, words, stage, ...}
        self.bodies: dict = d.get("bodies", {})             # commitment id -> the body delivered
        self.results: dict = d.get("results", {})           # result id -> bench result (with "section", "by", "digest")
        self.versions: dict = d.get("versions", {})         # version id -> {section, n, parent, by, origin, model, text, claims, figures, proposals, gaps}
        self.reviews: dict = d.get("reviews", {})           # review id -> {version, by, origin, community, model, scores, level, comments, recommend}
        self.audits: list = d.get("audits", [])             # {turn, version, checked, wrong: [...], by}
        self.petitions: dict = d.get("petitions", {})       # petition id -> {by, origin, community, kind, name, need, status, history}
        self.tools: dict = d.get("tools", {})               # admitted tools: name -> {name, code, about, params, series params, by, admitted}
        self.proposals: dict = d.get("proposals", {})       # the chapter of proposals: community -> {version, text, ...}
        self.adopted_book: dict = d.get("adopted_book", {})
        self.measures: dict = d.get("measures", {})         # per round tallies kept for the record
        self.events: list = d.get("events", [])             # milestones, for the record

    def to_json(self) -> dict:
        return {k: getattr(self, k) for k in ("call", "outline", "zero", "amendments", "approvals", "allotment", "sections", "bodies", "results",
                                               "versions", "reviews", "audits", "petitions", "tools", "proposals", "adopted_book", "measures", "events")}

    # ---- reading ----------------------------------------------------------------------------------------------------------------
    def section(self, sid: str) -> dict | None:
        return self.sections.get(sid)

    def at(self, stage: str) -> list[dict]:
        return [s for s in list(self.sections.values()) if s["stage"] == stage]

    def chapter(self, cid: str) -> dict | None:
        return next((c for c in self.outline.get("chapters", []) if c["id"] == cid), None)

    def results_of(self, sid: str) -> list[dict]:
        return [dict(r, id=rid) for rid, r in list(self.results.items()) if r.get("section") == sid and r.get("kept")]

    def facts_of(self, sid: str) -> dict:
        """Every fact a writer of the section may cite, by "result id: name"."""
        out = dict(self.sections.get(sid, {}).get("overview facts", {}))  # the overview cites the facts of the sections it sums up
        for r in self.results_of(sid):
            for k, v in (r.get("facts") or {}).items():
                out[f"{r['id']}: {k}"] = v
        return out

    def versions_of(self, sid: str) -> list[dict]:
        return sorted([dict(v, id=vid) for vid, v in list(self.versions.items()) if v["section"] == sid], key=lambda v: v["n"])

    def latest(self, sid: str) -> dict | None:
        vs = self.versions_of(sid)
        return vs[-1] if vs else None

    def reviews_of(self, vid: str) -> list[dict]:
        return [dict(r, id=rid) for rid, r in list(self.reviews.items()) if r["version"] == vid]

    def counts(self) -> dict:
        by = {st: 0 for st in STAGES}
        for s in list(self.sections.values()):
            by[s["stage"]] = by.get(s["stage"], 0) + 1
        words = sum(len(self.versions[s["adopted"]]["text"].split()) for s in self.sections.values() if s.get("adopted") in self.versions)
        words += sum(len(str(v.get("text", "")).split()) for v in self.proposals.values())
        return {"sections": len(self.sections), **by, "versions": len(self.versions), "reviews": len(self.reviews), "results": len(self.results),
                "words adopted": words, "petitions": len(self.petitions), "tools admitted": len(self.tools), "audits": len(self.audits),
                "wrong found by audit": sum(len(a["wrong"]) for a in self.audits)}

    def event(self, turn: int, text: str, kind: str = "") -> None:
        self.events.append({"turn": turn, "kind": kind, "text": text})


# ---- the standards bodies are held to ---------------------------------------------------------------------------------------------
def choice_faults(body: dict, catalogue: dict) -> str:
    """A choice of series: 1 to 6 codes of the catalogue, each with a reason."""
    codes = body.get("series")
    if not isinstance(codes, list) or not 1 <= len(codes) <= 6:
        return "a choice names one to six series"
    for c in codes:
        if not isinstance(c, dict) or c.get("code") not in catalogue or not str(c.get("why", "")).strip():
            return f"a series is not in the catalogue or has no reason ({c.get('code') if isinstance(c, dict) else c})"
    return ""


def check_faults(body: dict, first: dict, catalogue: dict) -> str:
    """A check of a choice: a verdict, keep or drop, on each code of the first choice, and at most two additions of the catalogue."""
    v = body.get("verdicts")
    if not isinstance(v, list):
        return "a check gives a verdict on each series chosen"
    seen = {x.get("code") for x in v if isinstance(x, dict) and x.get("verdict") in ("keep", "drop")}
    missing = [c["code"] for c in first.get("series", []) if c["code"] not in seen]
    if missing:
        return "no verdict on " + ", ".join(missing)
    add = body.get("add") or []
    if len(add) > 2 or any(not isinstance(a, dict) or a.get("code") not in catalogue for a in add):
        return "at most two additions, each in the catalogue"
    return ""


def plan_faults(body: dict, tools: set) -> str:
    plan = body.get("plan")
    if not isinstance(plan, list) or not 1 <= len(plan) <= 8:
        return "a plan runs one to eight tools"
    for p in plan:
        if not isinstance(p, dict) or p.get("tool") not in tools or not isinstance(p.get("params"), dict):
            return f"a step names no tool of the bench ({p.get('tool') if isinstance(p, dict) else p})"
    return ""


def version_faults(body: dict, facts: dict, words: int) -> str:
    """A version: text of at least a third of the words asked, claims that cite facts of the section, figures that name its results."""
    text = str(body.get("text", ""))
    n = len(text.split())
    if n < max(200, (8 * words) // 10 - 100):
        return f"the text has {n} words where about {words} were asked"
    claims = body.get("claims")
    if not isinstance(claims, list) or not claims:
        return "a version states its claims, each citing a fact"
    unknown = [c.get("fact") for c in claims if not isinstance(c, dict) or c.get("fact") not in facts]
    if len(unknown) > max(1, len(claims) // 5):
        return f"{len(unknown)} of {len(claims)} claims cite no fact of the section's results"
    return ""


def review_faults(body: dict) -> str:
    scores = body.get("scores")
    if not isinstance(scores, dict) or any(scores.get(code) not in LEVELS for code, _, _ in SCORECARD):
        return "a review scores every statement of the scorecard on its levels"
    if body.get("recommend") not in ("adopt", "minor", "revise"):
        return "a review recommends adopt, minor or revise"
    return ""


def weakest(scores: dict) -> str:
    """The weakest link of a review's levels (the scorecard's declared rule of aggregation, kybernesis's `weakest_link`)."""
    return min((scores.get(code, "fails") for code, _, _ in SCORECARD), key=LEVELS.index)


NUMBER = re.compile(r"(-?\d[\d,]*(?:\.\d+)?(?:[eE][-+]?\d+)?)\s*(trillion|billion|million|thousand)?", re.IGNORECASE)
SCALE = {"thousand": 1e3, "million": 1e6, "billion": 1e9, "trillion": 1e12}


def numbers_in(text: str) -> list[tuple[float, float]]:
    """Each number written in a text with the half-width of its last written digit: 42.9 is 42.9 ± 0.05; 223,000 is 223,000 ± 500 (its
    trailing zeros read as rounding); 7.9 billion is 7.9e9 ± 5e7; 4e-05 is 4e-05 ± 5e-06."""
    t = str(text).replace("\u2212", "-").replace("\u2009", "").replace("\u202f", "").replace("%", " ")
    out = []
    for m in NUMBER.finditer(t):
        raw, scale = m.group(1).replace(",", ""), SCALE.get((m.group(2) or "").lower(), 1.0)
        try:
            v = float(raw)
        except ValueError:
            continue
        mant = raw.lower().split("e")[0]
        expo = int(raw.lower().split("e")[1]) if "e" in raw.lower() else 0
        if "." in mant:
            half = 0.5 * 10 ** (-len(mant.split(".")[1]) + expo)
        else:
            zeros = len(mant) - len(mant.rstrip("0")) if mant.rstrip("0") else 0
            half = 0.5 * 10 ** (zeros + expo)
        out.append((v * scale, half * scale))
    return out


def claim_holds(claim: dict, facts: dict) -> tuple[bool, str]:
    """Whether a claim's number is its fact's value as written: some number in the claim's value equals the fact (or the fact times a
    hundred, a share written as a percentage) within the half-width of its last written digit. A fact in words is held to its words."""
    key = claim.get("fact")
    if key not in facts:
        return False, "the fact cited is not a fact of the section"
    f = facts[key]
    if f is None:
        return False, "the fact cited has no value"
    value = str(claim.get("value", "")).strip()
    if not isinstance(f, (int, float)):
        ok = len(value) >= 3 and (value.lower() in str(f).lower() or str(f).lower() in value.lower())
        return ok, "" if ok else f"the fact is {f}"
    found = numbers_in(value)
    if not found:
        return False, f"the claim gives no number; the fact is {f}"
    for v, half in found:
        for scale in (1, 100):
            if abs(f * scale - v) <= half + 1e-9 * max(1.0, abs(v)):
                return True, ""
    return False, f"written {value}, the fact is {f}"
