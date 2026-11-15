"""Derivability under interpretation, for scripts in Python (ADR 0112, ADR 0114): a model proposes how a norm's open points
should be read, and the infrastructure weighs what follows.

apodeixis decides exactly under which interpretations a sentence follows; tekmerion's interpreters read the norm (a law, intervals,
precedents, a decision over purposes, a POMDP over a sequence of cases) and a panel weighs them. Both are pure C++, composed in
``api/cpp/science/interpretation.hpp`` and asked through an engine built from ``api/cpp/engines/interpretation/main.cpp``. What
cannot be pure is a language model: it is asked here, at the edge, and its answer enters the panel as a *proposal*: a reading
of each point it chose to read, with its rationale, graded C (a model, nothing cited). A proposal weighs nothing in the panel; its
readings are fixed only when an argument is made for them.

    from science import interpret
    case = open("park.case").read()                    # apodeixis's written form: open points, norms, arguments, queries
    proposal = interpret.propose(case)                 # Claude reads it (Anthropic's SDK; the key from the environment)
    answer = interpret.weigh(case, "prohibited", [proposal, {"name": "stated", "method": "law", "law": {...}}])

**What a model is asked.** The case in its written form, its open points with the readings each admits, and what the question
states of the case (its features, the cases before it). It answers, under a JSON schema whose readings are the declared ones, a
reading or ``none`` for each point with a rationale; nothing it says is taken as a source. Its answer is checked again here, point by
point and reading by reading, since a proposal that names what the case does not declare is refused, never repaired.

**Providers.** ``Anthropic`` asks Claude (Opus 5.5 by default) through the Anthropic SDK, with adaptive thinking, structured output and,
for the models that take it, the API's server-side fallback on a declined request (``fallbacks: "default"``); the model that answered
is recorded in ``by``. ``Stub`` answers as told, for checks and for working without a network. Another provider is a class with
``id``, ``name``, ``unavailable()`` and ``answer(system, prompt, schema) -> (data, model)``.

**Keys.** A key is given as ``Anthropic(key=...)`` or found by the SDK in the environment (``ANTHROPIC_API_KEY``, or a profile of
``ant auth login``). Nothing here writes a key anywhere; a key never belongs in a file of the repository.
"""

from __future__ import annotations

import json
import time
from pathlib import Path

from .engines import ROOT, Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("api/interpretation", source=ROOT / "api" / "cpp" / "engines" / "interpretation" / "main.cpp", env="NOETARIUM_INTERPRETATION_ENGINE")

GRADE = "C"
SYSTEM = (
    "You read a norm for a case. The case is written in a small formal language: open points with the readings each admits "
    "(\"open vehicle: broad | narrow\"), premises, rules strict and defeasible, interpretive arguments, and queries. For each open "
    "point, propose the reading you think the norm should be given in this case, or \"none\" when the case does not let you choose, "
    "and give your rationale in one to three sentences: the words of the norm, its purpose, the system it belongs to, earlier cases. "
    "Say what you rely on; do not invent sources or cite what you cannot name. Your answer is a proposal: it will be weighed "
    "beside other interpreters and stands as proposed until an argument is made for its readings, so say plainly where you are unsure."
)


class Refusal(Exception):
    """A proposal that could not be had or could not be accepted, with the reason."""


# ---- providers ------------------------------------------------------------------------------------------------------------------


class Anthropic:
    """Claude, through the Anthropic SDK."""

    id = "anthropic"
    name = "Anthropic (Claude)"
    # The models that take the API's server-side fallback on a declined request.
    fallback = ("claude-opus-5-5", "claude-sonnet-5-5", "claude-fable-5-2", "claude-fable-5-1", "claude-opus-5")

    def __init__(self, model: str = "claude-opus-5-5", effort: str = "medium", key: str | None = None, max_tokens: int = 16000):
        if effort not in ("low", "medium", "high", "xhigh", "max"):
            raise ValueError("effort is one of low, medium, high, xhigh, max")
        self.model, self.effort, self.key, self.max_tokens = model, effort, key, max_tokens

    def unavailable(self) -> str | None:
        try:
            import anthropic  # noqa: F401
        except ImportError:
            return "the Anthropic SDK is not installed for this Python: pip install anthropic"
        return None

    def answer(self, system: str, prompt: str, schema: dict) -> tuple[dict, str]:
        why = self.unavailable()
        if why:
            raise Refusal(why)
        import anthropic

        client = anthropic.Anthropic(api_key=self.key) if self.key else anthropic.Anthropic()
        asked: dict = {
            "model": self.model,
            "max_tokens": self.max_tokens,
            "system": system,
            "messages": [{"role": "user", "content": prompt}],
            "thinking": {"type": "adaptive"},
            "output_config": {"effort": self.effort, "format": {"type": "json_schema", "schema": schema}},
        }
        betas: list[str] = []
        if self.model in self.fallback:
            asked["fallbacks"] = "default"
            betas.append("server-side-fallback-2026-07-01")
        try:
            message = client.beta.messages.create(**asked, betas=betas) if betas else client.messages.create(**asked)
        except anthropic.AuthenticationError as error:
            raise Refusal("the key was not accepted") from error
        except anthropic.PermissionDeniedError as error:
            raise Refusal(f"the key may not do this: {error.message}") from error
        except anthropic.NotFoundError as error:
            raise Refusal(f"the model {self.model} was not found for this key") from error
        except anthropic.RateLimitError as error:
            raise Refusal(f"too many requests for now; try again in {error.response.headers.get('retry-after', 'a few')} seconds") from error
        except anthropic.BadRequestError as error:
            raise Refusal(f"the request was not accepted: {error.message}") from error
        except anthropic.APIStatusError as error:
            raise Refusal(f"the provider failed ({error.status_code}): {error.message}") from error
        except anthropic.APIConnectionError as error:
            raise Refusal("the provider could not be reached") from error
        except anthropic.AnthropicError as error:  # no credentials at all, among others
            raise Refusal(f"no key was given and none was found in the environment: {error}") from error
        if message.stop_reason == "refusal":
            details = message.stop_details
            raise Refusal("the model declined to read the case" + (f" ({details.category}: {details.explanation})" if details else ""))
        if message.stop_reason == "max_tokens":
            raise Refusal("the model's answer was cut off at its limit of tokens")
        text = next((b.text for b in message.content if b.type == "text"), None)
        if text is None:
            raise Refusal("the model gave no answer in words")
        try:
            return json.loads(text), message.model
        except ValueError as error:
            raise Refusal("the model's answer was not JSON") from error


class Stub:
    """Answers given in advance: a dict, or a function of (system, prompt, schema) giving one."""

    id = "stub"
    name = "a stand-in"

    def __init__(self, answers, model: str = "stub"):
        self.answers, self.model = answers, model
        self.asked: list[dict] = []

    def unavailable(self) -> str | None:
        return None

    def answer(self, system: str, prompt: str, schema: dict) -> tuple[dict, str]:
        self.asked.append({"system": system, "prompt": prompt, "schema": schema})
        data = self.answers(system, prompt, schema) if callable(self.answers) else self.answers
        return json.loads(json.dumps(data)), self.model


# ---- what is asked, and what is accepted ----------------------------------------------------------------------------------------


def schema(points: list[dict]) -> dict:
    """The JSON schema of an answer: for every open point, a reading among those declared (or "none") and a rationale."""
    props = {}
    for p in points:
        props[p["name"]] = {
            "type": "object",
            "properties": {"reading": {"type": "string", "enum": list(p["readings"]) + ["none"]}, "rationale": {"type": "string"}},
            "required": ["reading", "rationale"],
            "additionalProperties": False,
        }
    return {
        "type": "object",
        "properties": {"readings": {"type": "object", "properties": props, "required": [p["name"] for p in points], "additionalProperties": False}},
        "required": ["readings"],
        "additionalProperties": False,
    }


def prompt(case: str, points: list[dict], question: dict | None = None) -> str:
    """The message a model is given: the case, its points, and what the question states of it."""
    lines = ["The case, in its written form:", "", case.strip(), "", "Its open points:"]
    lines += [f"- {p['name']}: {' | '.join(p['readings'])}" for p in points]
    q = question or {}
    if q.get("name"):
        lines += ["", f"The case is called: {q['name']}."]
    if q.get("features"):
        lines += ["", "What is stated of the case:"] + [f"- {k}: {v}" for k, v in q["features"].items()]
    if q.get("history"):
        lines += ["", "The cases before it, in order:"]
        for e in q["history"]:
            what = ", ".join(f"{k}={v}" for k, v in e["adopted"].items()) if "adopted" in e else f"inquiry: {e.get('inquiry')}"
            lines.append(f"- {what}; met with: {e.get('response')}")
    if q.get("text"):
        lines += ["", q["text"].strip()]
    lines += ["", "Propose a reading of each open point, or none, with your rationale."]
    return "\n".join(lines)


def accept(points: list[dict], data: dict, by: str, name: str = "a language model") -> dict:
    """A model's answer checked point by point and made a proposal, or Refusal with every fault."""
    faults: list[str] = []
    readings = data.get("readings") if isinstance(data, dict) else None
    if not isinstance(readings, dict):
        raise Refusal("the answer holds no readings")
    known = {p["name"]: p for p in points}
    for k in readings:
        if k not in known:
            faults.append(f"it reads {k}, which is no open point of the case")
    proposed, rationales = {}, {}
    for p in points:
        r = readings.get(p["name"])
        if not isinstance(r, dict) or not isinstance(r.get("reading"), str):
            faults.append(f"it gives no reading of {p['name']}")
            continue
        if r["reading"] != "none" and r["reading"] not in p["readings"]:
            faults.append(f"it reads {p['name']} as {r['reading']}, which the case does not declare")
            continue
        proposed[p["name"]] = None if r["reading"] == "none" else r["reading"]
        rationales[p["name"]] = str(r.get("rationale", "")).strip()
    if faults:
        raise Refusal("the proposal was not accepted: " + "; ".join(faults))
    said = "; ".join(f"{k}: {v}" for k, v in rationales.items() if v)
    return {"name": name, "method": "proposal", "by": by, "grade": GRADE, "proposed": proposed, "rationale": said, "rationales": rationales,
            "asked_at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())}


def propose(case: str, provider=None, *, question: dict | None = None, name: str = "a language model", points: list[dict] | None = None) -> dict:
    """A model's proposal for the case: an interpreter of method "proposal", ready for weigh()."""
    pts = points if points is not None else read_points(case)
    if not pts:
        raise Refusal("the case has no open point to read")
    provider = provider or Anthropic()
    data, served = provider.answer(SYSTEM, prompt(case, pts, question), schema(pts))
    return accept(pts, data, by=f"{provider.name}, {served}", name=name)


# ---- the engine -----------------------------------------------------------------------------------------------------------------


def build(log=print) -> bool:
    """Builds the engine if it is stale; True when it is current."""
    return ENGINE.build(log=log)


def ask(op: str, timeout: float = 600.0, **request) -> dict:
    """One request to the engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def read_points(case: str) -> list[dict]:
    """The case's open points, each {"name", "readings"}, read by apodeixis; ValueError with every line it could not read."""
    r = ask("points", case=case)
    if not r.get("ok"):
        raise ValueError("the case could not be read: " + "; ".join(r.get("errors", [])))
    return r["points"]


def weigh(case: str, query: str, interpreters: list[dict], question: dict | None = None) -> dict:
    """apodeixis's answer for the query and the panel's weighing over the admissible interpretations."""
    return ask("weigh", case=case, query=query, interpreters=interpreters, question=question or {})
