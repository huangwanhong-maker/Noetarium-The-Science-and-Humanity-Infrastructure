"""Language models at the edge of the infrastructure: the providers that every work and script asks through (ADR 0114, ADR 0121).

A model is never a source here. What it answers enters the foundations as a *proposal* (tekmerion's grade C): a reading of an
open point (``science.interpret``), a passage put forward as evidence (``science.recognise``). Each caller checks the answer
against what it asked and refuses what does not fit; nothing here repairs an answer.

**A provider** is a class with ``id``, ``name``, ``unavailable() -> str | None`` and
``answer(system, prompt, schema, *, cached=None) -> (data, model)``, where ``schema`` is the JSON schema the answer must meet,
``data`` the answer read as JSON and ``model`` the model that gave it. ``cached`` is a long text (a document) put before the
prompt and marked for the provider's cache, so that several questions about one document read it once. Two are here:

- ``Anthropic``: Claude through Anthropic's SDK (Opus 5.5 by default), with adaptive thinking, structured output, the answer
  streamed, and the API's server-side fallback on a declined request for the models that take it;
- ``Stub``: answers given in advance, for checks and for working without a network.

Another is added to ``PROVIDERS``.

**Keys.** A key is given to the provider, or kept for the person at this computer in ``models.json`` in their settings folder
(``%APPDATA%\\noetarium`` on Windows, ``$XDG_CONFIG_HOME/noetarium`` or ``~/.config/noetarium`` elsewhere), as
``{"keys": {"anthropic": "..."}}``; failing both, the SDK looks in the environment (``ANTHROPIC_API_KEY``, or a profile of
``ant auth login``). A key never belongs in a file of the repository, and no page is ever given one.
"""

from __future__ import annotations

import json
import os
from pathlib import Path


class Refusal(Exception):
    """An answer that could not be had or could not be accepted, with the reason."""


def settings_folder() -> Path:
    """The person's settings folder for the infrastructure (never under the repository, which every work's server serves)."""
    base = os.environ.get("APPDATA") if os.name == "nt" else os.environ.get("XDG_CONFIG_HOME")
    return (Path(base) if base else Path.home() / ".config") / "noetarium"


def kept_key(provider: str) -> str | None:
    """The key kept for a provider in the person's models.json, if any."""
    try:
        data = json.loads((settings_folder() / "models.json").read_text(encoding="utf-8-sig"))  # a file written on Windows may begin with a mark
    except (OSError, ValueError):
        return None
    key = (data.get("keys") or {}).get(provider) if isinstance(data, dict) else None
    return key if isinstance(key, str) and key.strip() else None


def key_state(provider: str) -> dict:
    """Whether a key is kept for the provider and how it ends, never the key."""
    key = kept_key(provider)
    return {"kept": bool(key), "ends": f"…{key[-4:]}" if key else ""}


class Anthropic:
    """Claude, through Anthropic's SDK."""

    id = "anthropic"
    name = "Anthropic (Claude)"
    # The models that take the API's server-side fallback on a declined request.
    fallback = ("claude-opus-5-5", "claude-sonnet-5-5", "claude-fable-5-2", "claude-fable-5-1", "claude-opus-5")
    models = ("claude-opus-5-5", "claude-sonnet-5-5", "claude-haiku-5-5", "claude-fable-5-2")

    def __init__(self, model: str = "claude-opus-5-5", effort: str = "medium", key: str | None = None, max_tokens: int = 16000):
        if effort not in ("low", "medium", "high", "xhigh", "max"):
            raise ValueError("effort is one of low, medium, high, xhigh, max")
        self.model, self.effort, self.max_tokens = model, effort, max_tokens
        self.key = key or kept_key(self.id)
        self.usage: dict = {}  # the tokens of the last answer: input, output, cache written and read, and the model that gave it

    def unavailable(self) -> str | None:
        try:
            import anthropic  # noqa: F401
        except ImportError:
            return "the Anthropic SDK is not installed for this Python: pip install anthropic"
        return None

    def answer(self, system: str, prompt: str, schema: dict, *, cached: str | None = None) -> tuple[dict, str]:
        why = self.unavailable()
        if why:
            raise Refusal(why)
        import anthropic

        client = anthropic.Anthropic(api_key=self.key) if self.key else anthropic.Anthropic()
        content: list[dict] = []
        if cached:
            content.append({"type": "text", "text": cached, "cache_control": {"type": "ephemeral"}})
        content.append({"type": "text", "text": prompt})
        asked: dict = {
            "model": self.model,
            "max_tokens": self.max_tokens,
            "system": system,
            "messages": [{"role": "user", "content": content}],
            "thinking": {"type": "adaptive"},
            "output_config": {"effort": self.effort, "format": {"type": "json_schema", "schema": schema}},
        }
        betas: list[str] = []
        if self.model in self.fallback:
            asked["fallbacks"] = "default"
            betas.append("server-side-fallback-2026-07-01")
        try:
            # Streamed, so that a long document or a long answer does not meet the request's time limit.
            if betas:
                with client.beta.messages.stream(**asked, betas=betas) as stream:
                    message = stream.get_final_message()
            else:
                with client.messages.stream(**asked) as stream:
                    message = stream.get_final_message()
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
            raise Refusal(f"no key was given, none is kept in {settings_folder() / 'models.json'} and none was found in the environment: {error}") from error
        u = getattr(message, "usage", None)
        self.usage = {"model": message.model,
                      **{k: int(getattr(u, k, 0) or 0) for k in ("input_tokens", "output_tokens", "cache_creation_input_tokens", "cache_read_input_tokens")}}
        if message.stop_reason == "refusal":
            details = getattr(message, "stop_details", None)
            raise Refusal("the model declined to answer" + (f" ({details.category}: {details.explanation})" if details else ""))
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

    def answer(self, system: str, prompt: str, schema: dict, *, cached: str | None = None) -> tuple[dict, str]:
        self.asked.append({"system": system, "prompt": prompt, "schema": schema, "cached": cached})
        data = self.answers(system, prompt, schema) if callable(self.answers) else self.answers
        return json.loads(json.dumps(data)), self.model


PROVIDERS = {"anthropic": Anthropic, "stub": Stub}


def provider(name: str = "anthropic", **options):
    """A provider by its id, made with the options given."""
    if name not in PROVIDERS:
        raise Refusal(f"there is no provider {name} (there is: {', '.join(PROVIDERS)})")
    return PROVIDERS[name](**options)
