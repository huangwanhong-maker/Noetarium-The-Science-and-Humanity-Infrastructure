"""Evidence-based policy, for scripts in Python and the works' servers (ADR 0122): a question worked through from its theory of
change to the decision, asked of the engine a work has built.

The computations are the policy domain's C++ with peira, tekmerion, nomos and tyche, composed in ``api/cpp/science/policy.hpp``,
which documents both operations and the question they take (``noetarium.policy/1``):

    from science import policy
    policy.ask("theory", text=theory)              # a theory of change read from its written form, or its lines that do not read
    policy.ask("appraise", question=question)       # bodies pooled and graded, the strongest path, here, the options, the decision

The engine is a program built by a work: the Policy Studio builds ``data/cache/engines/policy-studio/policy/engine``;
``NOETARIUM_POLICY_ENGINE`` names another.

``studies_from_csv`` reads a table of studies as a person pastes it: a header, then a row a study. Columns named ``effect`` and
``variance`` (or ``se``) give effects as they are; ``treated_events``, ``treated_others``, ``control_events``, ``control_others``
give 2 × 2 counts; ``id``, ``label``, ``year`` and ``stratum`` are kept where present.

``draft_theory`` asks a language model (``science.models``) for a theory of change of an intervention described in words. What it
answers is a proposal and nothing more: it is written in the domain's form and read by the engine's own reader; a draft that does
not read is refused with its lines, never repaired; and it carries no evidence, since a model is never a source, so that every link
of a draft stands as not shown until studies are set on it.
"""

from __future__ import annotations

import csv
import io

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("policy-studio/policy", env="NOETARIUM_POLICY_ENGINE")
NUMBERS = ("effect", "variance", "se", "year", "treated_events", "treated_others", "control_events", "control_others")


def ask(op: str, timeout: float = 300.0, **request) -> dict:
    """One request to the policy engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def studies_from_csv(text: str) -> list[dict]:
    """The studies of a pasted table; ValueError with the line that does not read."""
    rows = list(csv.reader(io.StringIO(text.strip())))
    if len(rows) < 2:
        raise ValueError("a table of studies has a header and at least one row")
    header = [h.strip().lower() for h in rows[0]]
    counts = all(c in header for c in ("treated_events", "treated_others", "control_events", "control_others"))
    if not counts and not ("effect" in header and ("variance" in header or "se" in header)):
        raise ValueError("the header names effect and variance (or se), or the four counts treated_events, treated_others, control_events, control_others")
    out = []
    for n, row in enumerate(rows[1:], start=2):
        if not any(c.strip() for c in row):
            continue
        if len(row) != len(header):
            raise ValueError(f"line {n} has {len(row)} cells, and the header {len(header)}")
        study: dict = {}
        for name, cell in zip(header, row):
            cell = cell.strip()
            if name in NUMBERS:
                if cell == "":
                    continue
                try:
                    study[name] = float(cell)
                except ValueError:
                    raise ValueError(f"line {n}: {name} is a number, not {cell!r}") from None
            elif name in ("id", "label", "stratum"):
                study[name] = cell
        if "se" in study and "variance" not in study:
            study["variance"] = study.pop("se") ** 2
        study.setdefault("id", str(n - 1))
        out.append(study)
    return out


# ---- A model's draft of a theory -----------------------------------------------------------------------------------------------------

DRAFT_SYSTEM = (
    "You draft theories of change for people who appraise policies. A theory of change is an acyclic graph: its nodes are states "
    "of affairs, from the intervention through outputs and outcomes to the impact, and its links are causal claims, each resting on "
    "an assumption and needing support factors (what else must be present, in the place, for the cause to work there). Draft what "
    "the description implies and no more: between four and nine nodes, every node on a path from the intervention, more than one "
    "path only where the description gives two mechanisms. Identifiers are single lowercase words. State no evidence, effect or "
    "grade: you are not a source, and the reader will set studies on the links. Mark a link's sign as '-' only where the cause is "
    "meant to lower what it leads to."
)

DRAFT_SCHEMA = {
    "type": "object", "additionalProperties": False, "required": ["name", "nodes", "links"],
    "properties": {
        "name": {"type": "string"},
        "nodes": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["id", "kind", "text"], "properties": {
            "id": {"type": "string"}, "kind": {"type": "string", "enum": ["intervention", "output", "outcome", "impact"]}, "text": {"type": "string"}}}},
        "links": {"type": "array", "items": {"type": "object", "additionalProperties": False, "required": ["from", "to", "assumption", "support", "sign"], "properties": {
            "from": {"type": "string"}, "to": {"type": "string"}, "assumption": {"type": "string"},
            "support": {"type": "array", "items": {"type": "string"}}, "sign": {"type": "string", "enum": ["+", "-"]}}}},
    },
}


def _word(text: str) -> str:
    return "_".join(str(text).strip().lower().split())


def _line(text: str) -> str:
    return " ".join(str(text).split())


def written(draft: dict, levels: list[str]) -> str:
    """A drafted theory in the domain's written form, as it was answered: nothing is mended on the way."""
    lines = [f"theory {_word(draft.get('name', '')) or 'a_theory'}", "levels " + " ".join(levels)]
    for n in draft.get("nodes", []):
        lines.append(f"node {_word(n.get('id', ''))} {_word(n.get('kind', ''))}: {_line(n.get('text', ''))}")
    for l in draft.get("links", []):
        a, b = _word(l.get("from", "")), _word(l.get("to", ""))
        lines.append(f"link {a} -> {b}: {_line(l.get('assumption', ''))}")
        for f in l.get("support", []):
            if _line(f):
                lines.append(f"support {a} -> {b}: {_line(f)}")
        if l.get("sign") == "-":
            lines.append(f"sign {a} -> {b}: -")
    return "\n".join(lines) + "\n"


def draft_theory(description: str, provider=None, *, levels: list[str] | None = None, name: str = "a language model") -> dict:
    """A model's draft of a theory of change for the intervention described: ``{text, theory, by, grade: "C", status: "proposed"}``,
    its text read by the engine's reader. models.Refusal when no answer could be had, or when the draft does not read (with the
    reader's lines), or when it states no intervention."""
    from . import models
    description = _line(description)
    if len(description) < 12:
        raise models.Refusal("describe the intervention and what it is for in a sentence or two")
    provider = provider or models.Anthropic()
    data, served = provider.answer(DRAFT_SYSTEM, "Draft a theory of change for this intervention.\n\n" + description, DRAFT_SCHEMA)
    text = written(data if isinstance(data, dict) else {}, levels or ["very_low", "low", "moderate", "high"])
    read = ask("theory", text=text)
    if not read.get("ok"):
        raise models.Refusal("the model's draft does not read as a theory of change: " + "; ".join(read.get("errors", [])[:4]))
    kinds = [n["kind"] for n in read["theory"]["nodes"]]
    if "intervention" not in kinds or len(read["theory"]["links"]) == 0:
        raise models.Refusal("the model's draft names no intervention, or no causal claim")
    return {"text": text, "theory": read["theory"], "by": f"{provider.name}, {served}", "name": name, "grade": "C", "status": "proposed"}
