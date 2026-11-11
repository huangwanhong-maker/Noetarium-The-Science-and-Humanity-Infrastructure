"""The agent's side of the server: who answers (a provider and its models), with what key, and one step of a conversation.

The page holds the conversation and the board; it asks here for the next step, and carries out on the board what the model asks for. A key
is given once, kept in the person's own settings folder on this computer (``ai-agent.json`` in the folder the work's server names: outside
the repository and outside everything any of the works' servers serve) and never sent back to the page; with none kept, the provider's own
credentials are tried (``ANTHROPIC_API_KEY``, or a profile of ``ant auth login``).

    GET  settings        the provider and model chosen, which plug-ins the agent may use, and whether a key is kept (never the key)
    POST settings        {"provider", "model", "effort", "allowed": [...], "modules": bool, "key"?: "..." | ""}: keep them
    GET  providers       the providers, each with its models and whether it can be used here
    POST messages        {"system", "tools", "messages"}: the next message of the conversation, in the form below

**The form of a conversation** is the Messages API's, which is the richest of the forms in use: a message is a role and a list of content
blocks (``text``, ``tool_use`` with an ``id``, a ``name`` and an ``input``, ``tool_result`` with the ``tool_use_id`` it answers; and blocks
of the provider's own, such as ``thinking``, which are passed back as they came). An answer is ``{"content": [...], "stop_reason",
"usage", "model"}``. A provider of another form translates to and from this one.

**A conversation is only added to.** The provider keeps a model's reasoning (its ``thinking`` blocks) only while everything before it
stands as it was sent: the system prompt, the tools, the earlier messages. So the page sends the same system prompt and tools for the whole
of a conversation, tells a change of settings in a new message, and takes messages only off the end.

**Another provider** is a class with ``id``, ``name``, ``models``, ``unavailable()`` (why it cannot be used here, or None) and
``complete(key, request)``, added to ``PROVIDERS``.
"""

from __future__ import annotations

import json
import os
import sys
from pathlib import Path

EFFORTS = ("low", "medium", "high", "xhigh", "max")
MOST_TOKENS = 16000


class Refusal(Exception):
    """A request that cannot be answered, with the HTTP status that says how."""

    def __init__(self, message: str, code: int = 400):
        super().__init__(message)
        self.code = code


class Anthropic:
    """Claude, through the Anthropic SDK."""

    id = "anthropic"
    name = "Anthropic (Claude)"
    key_hint = "sk-ant-…"
    models = [
        {"id": "claude-opus-5-5", "name": "Claude Opus 5.5"},
        {"id": "claude-sonnet-5-5", "name": "Claude Sonnet 5.5"},
        {"id": "claude-haiku-5-5", "name": "Claude Haiku 5.5"},
        {"id": "claude-fable-5-1", "name": "Claude Fable 5.1"},
    ]
    # The models that a request declined for safety may be answered by another, chosen by the API.
    fallback = ("claude-opus-5-5", "claude-sonnet-5-5", "claude-fable-5-1")

    def unavailable(self) -> str | None:
        try:
            import anthropic  # noqa: F401
        except ImportError:
            return "the Anthropic SDK is not installed for the Python that runs this server: pip install anthropic"
        return None

    def complete(self, key: str | None, request: dict) -> dict:
        import anthropic

        # With no key kept, the SDK finds the environment's own credentials.
        client = anthropic.Anthropic(api_key=key) if key else anthropic.Anthropic()
        model = request["model"]
        asked: dict = {
            "model": model,
            "max_tokens": MOST_TOKENS,
            "system": request["system"],
            "tools": request["tools"],
            "messages": request["messages"],
            # The tools and the system prompt are the same at every step of a conversation: they are read from the cache.
            "cache_control": {"type": "ephemeral"},
            "output_config": {"effort": request.get("effort") or "medium"},
            # The model thinks as much as the step needs. Its thinking is kept only while what came before it stands as it was sent: the
            # page keeps a conversation so (it only adds to it), and if something before a block has changed all the same, the block is
            # dropped and the step goes on, where it would otherwise be refused and the conversation could not continue.
            "thinking": {"type": "adaptive", "block_binding": {"prefix_mismatch_behavior": "drop_block"}},
        }
        betas = ["thinking-binding-controls-2026-08-01"]
        if model in self.fallback:
            asked["fallbacks"] = "default"
            betas.append("server-side-fallback-2026-07-01")
        try:
            message = client.beta.messages.create(**asked, betas=betas)
        except anthropic.AuthenticationError as error:
            raise Refusal("the key was not accepted: set it again in the agent's settings", 401) from error
        except anthropic.PermissionDeniedError as error:
            raise Refusal(f"the key may not do this: {error.message}", 403) from error
        except anthropic.NotFoundError as error:
            raise Refusal(f"the model {model} was not found for this key", 404) from error
        except anthropic.RateLimitError as error:
            raise Refusal(f"too many requests for now; try again in {error.response.headers.get('retry-after', 'a few')} seconds", 429) from error
        except anthropic.BadRequestError as error:
            raise Refusal(f"the request was not accepted: {error.message}", 400) from error
        except anthropic.APIStatusError as error:
            raise Refusal(f"the provider failed ({error.status_code}): {error.message}", 502) from error
        except anthropic.APIConnectionError as error:
            raise Refusal("the provider could not be reached: is this computer on the network?", 502) from error
        except anthropic.AnthropicError as error:  # no credentials at all, among others
            raise Refusal(f"no key is kept and none was found in the environment: {error}", 401) from error
        answer = message.to_dict()
        # Thinking dropped because the conversation was changed behind it is a fault of the page's to mend: it is said where the server is watched.
        dropped = [t for t in answer.get("input_transformations") or [] if t.get("type") == "thinking_dropped" and t.get("reason") == "prefix_binding_mismatch"]
        if dropped:
            print(f"the agent: {len(dropped)} block(s) of the model's thinking were dropped, the conversation having changed before them ({dropped[0].get('path')})", file=sys.stderr, flush=True)
        return {"content": answer.get("content", []), "stop_reason": answer.get("stop_reason"), "stop_details": answer.get("stop_details"),
                "usage": answer.get("usage", {}), "model": answer.get("model", model), "dropped": len(dropped)}


PROVIDERS: dict = {Anthropic.id: Anthropic()}
DEFAULTS = {"provider": "anthropic", "model": "claude-opus-5-5", "effort": "medium", "allowed": None, "modules": True}


# ---- what is kept -----------------------------------------------------------------------------------------------


def _path(folder: Path) -> Path:
    return Path(folder) / "ai-agent.json"


def _kept(folder: Path) -> dict:
    try:
        data = json.loads(_path(folder).read_text(encoding="utf-8"))
        return data if isinstance(data, dict) else {}
    except (OSError, ValueError):
        return {}


def settings(folder: Path) -> dict:
    """The settings as the page may see them: whether a key is kept for each provider and how it ends, never the key."""
    kept = _kept(folder)
    keys = kept.get("keys") or {}
    out = {k: kept.get(k, v) for k, v in DEFAULTS.items()}
    out["keys"] = {p: {"kept": bool(keys.get(p)), "ends": f"…{keys[p][-4:]}" if keys.get(p) else ""} for p in PROVIDERS}
    out["kept_in"] = _shown(Path(folder))
    return out


def _shown(folder: Path) -> str:
    """A folder as the page is told of it: within the person's home, from there (``~``)."""
    try:
        return str(Path("~") / folder.resolve().relative_to(Path.home().resolve()))
    except ValueError:
        return str(folder)


def keep_settings(folder: Path, given: dict) -> dict:
    kept = _kept(folder)
    provider = given.get("provider", kept.get("provider", DEFAULTS["provider"]))
    if provider not in PROVIDERS:
        raise Refusal(f"there is no provider '{provider}' (there is: {', '.join(PROVIDERS)})")
    model = given.get("model", kept.get("model", DEFAULTS["model"]))
    if model not in [m["id"] for m in PROVIDERS[provider].models]:
        raise Refusal(f"{PROVIDERS[provider].name} has no model '{model}' here")
    effort = given.get("effort", kept.get("effort", DEFAULTS["effort"]))
    if effort not in EFFORTS:
        raise Refusal(f"effort is one of {', '.join(EFFORTS)}")
    allowed = given.get("allowed", kept.get("allowed"))
    if allowed is not None and not (isinstance(allowed, list) and all(isinstance(a, str) for a in allowed)):
        raise Refusal("the plug-ins the agent may use are a list of their names")
    kept.update(provider=provider, model=model, effort=effort, allowed=allowed, modules=bool(given.get("modules", kept.get("modules", True))))
    if "key" in given:
        key = str(given["key"] or "").strip()
        keys = dict(kept.get("keys") or {})
        if key:
            keys[provider] = key
        else:
            keys.pop(provider, None)
        kept["keys"] = keys
    path = _path(folder)
    path.parent.mkdir(parents=True, exist_ok=True)
    partial = path.with_suffix(".json.part")
    partial.write_text(json.dumps(kept, ensure_ascii=False), encoding="utf-8")
    try:
        os.chmod(partial, 0o600)   # the key is this person's alone, where the system keeps such a thing
    except OSError:
        pass
    os.replace(partial, path)
    return settings(folder)


# ---- one step of a conversation ---------------------------------------------------------------------------------


def step(folder: Path, body: dict) -> dict:
    kept = _kept(folder)
    provider = PROVIDERS.get(kept.get("provider", DEFAULTS["provider"]))
    if provider is None:
        raise Refusal("the provider chosen is not one of this server's")
    why = provider.unavailable()
    if why:
        raise Refusal(why, 503)
    system, tools, messages = body.get("system"), body.get("tools"), body.get("messages")
    if not isinstance(system, str) or not isinstance(tools, list) or not isinstance(messages, list) or not messages:
        raise Refusal("a step is asked for with a system prompt, tools and messages")
    return provider.complete((kept.get("keys") or {}).get(provider.id), {
        "model": kept.get("model", DEFAULTS["model"]), "effort": kept.get("effort", DEFAULTS["effort"]), "system": system, "tools": tools, "messages": messages})


def handle(method: str, path: str, body: dict | None, context: dict) -> tuple[dict, int]:
    """One request to the plug-in: (answer, status). `context` gives the folder of the person's settings."""
    store = context["settings"]
    try:
        if method == "GET" and path == "settings":
            return settings(store), 200
        if method == "GET" and path == "providers":
            return {"providers": [{"id": p.id, "name": p.name, "models": p.models, "key_hint": getattr(p, "key_hint", ""), "unavailable": p.unavailable()} for p in PROVIDERS.values()],
                    "efforts": list(EFFORTS)}, 200
        if method == "POST" and path == "settings":
            return keep_settings(store, body or {}), 200
        if method == "POST" and path == "messages":
            return step(store, body or {}), 200
    except Refusal as refusal:
        return {"message": str(refusal)}, refusal.code
    return {"message": "the agent has no such request"}, 404
