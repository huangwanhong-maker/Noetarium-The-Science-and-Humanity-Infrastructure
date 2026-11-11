"""Checks of the agent plug-in's side of the server (plugins/ai-agent/server.py), with a provider that asks no one: a key is kept and
never given back; settings that name no provider, model or effort are refused and change nothing; a step is asked of the provider chosen,
with the key kept for it and the conversation as it was given; a provider's refusal comes back with its reason and status.

    python check_agent_server.py <path to server.py>

Prints lines "ok  …" or "FAIL  …", as the suites' harness counts them.
"""

from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
from pathlib import Path

spec = importlib.util.spec_from_file_location("ai_agent_server", sys.argv[1])
server = importlib.util.module_from_spec(spec)
spec.loader.exec_module(server)


def say(name: str, condition: bool, detail: str = "") -> None:
    print(f"{'ok  ' if condition else 'FAIL'}  {name}{': ' + detail if detail else ''}")


class Echo:
    """A provider that answers with what it was asked."""

    id = "echo"
    name = "Echo"
    models = [{"id": "echo-1", "name": "Echo 1"}]
    asked: list = []

    def unavailable(self):
        return None

    def complete(self, key, request):
        self.asked.append((key, request))
        if request["messages"][-1]["content"] == "refuse":
            raise server.Refusal("the key was not accepted", 401)
        return {"content": [{"type": "text", "text": "heard"}], "stop_reason": "end_turn", "usage": {"input_tokens": 1, "output_tokens": 1}, "model": request["model"]}


class Missing(Echo):
    id = "missing"
    name = "Missing"

    def unavailable(self):
        return "its library is not installed"


server.PROVIDERS["echo"] = Echo()
server.PROVIDERS["missing"] = Missing()
SECRET = "sk-test-a-key-that-must-not-come-back-7Q2x"

with tempfile.TemporaryDirectory() as folder:
    store = Path(folder)
    context = {"settings": store}
    ask = lambda method, path, body=None: server.handle(method, path, body, context)  # noqa: E731

    first, code = ask("GET", "settings")
    say("agent server: with nothing kept, the settings are the defaults: Claude, every plug-in allowed, modules allowed, no key",
        code == 200 and first["provider"] == "anthropic" and first["model"] == "claude-opus-5-5" and first["allowed"] is None and first["modules"] is True
        and all(not k["kept"] for k in first["keys"].values()) and not list(store.iterdir()) and first["kept_in"].endswith(store.name) and (first["kept_in"].startswith("~") or first["kept_in"] == str(store)))

    who, code = ask("GET", "providers")
    claude = next(p for p in who["providers"] if p["id"] == "anthropic")
    say("agent server: the providers are told with their models and whether each can be used here",
        code == 200 and len(claude["models"]) >= 3 and all(m["id"].startswith("claude-") for m in claude["models"]) and "medium" in who["efforts"]
        and next(p for p in who["providers"] if p["id"] == "missing")["unavailable"] == "its library is not installed")

    kept, code = ask("POST", "settings", {"provider": "echo", "model": "echo-1", "effort": "high", "allowed": ["general"], "modules": False, "key": f"  {SECRET}  "})
    on_disk = json.loads((store / "ai-agent.json").read_text(encoding="utf-8"))
    say("agent server: a key is kept by the server, for its provider, and the answer says only that one is kept and how it ends",
        code == 200 and SECRET not in json.dumps(kept) and kept["keys"]["echo"] == {"kept": True, "ends": "…7Q2x"} and not kept["keys"]["anthropic"]["kept"]
        and on_disk["keys"] == {"echo": SECRET} and kept["allowed"] == ["general"] and kept["modules"] is False and kept["effort"] == "high")
    again, _ = ask("GET", "settings")
    say("agent server: asked again, the settings are as kept, still without the key", again == kept and SECRET not in json.dumps(again))

    before = (store / "ai-agent.json").read_text(encoding="utf-8")
    refusals = [ask("POST", "settings", {"provider": "nobody"}), ask("POST", "settings", {"model": "claude-opus-5-5"}), ask("POST", "settings", {"effort": "furious"}),
                ask("POST", "settings", {"allowed": "everything"}), ask("POST", "settings", {"allowed": [1, 2]})]
    say("agent server: a provider, a model of another provider, an effort or a list of plug-ins that is none is refused with the reason, and nothing kept changes",
        all(code == 400 and len(answer["message"]) > 10 for answer, code in refusals) and (store / "ai-agent.json").read_text(encoding="utf-8") == before,
        " | ".join(answer["message"] for answer, _ in refusals)[:200])

    partial, _ = ask("POST", "settings", {"effort": "low"})
    say("agent server: a part of the settings is changed alone, and the key stays kept", partial["effort"] == "low" and partial["provider"] == "echo" and partial["keys"]["echo"]["kept"])

    asked = {"system": "S", "tools": [{"name": "t", "description": "d", "input_schema": {"type": "object"}}], "messages": [{"role": "user", "content": "hello"}]}
    answer, code = ask("POST", "messages", asked)
    key, request = Echo.asked[-1]
    say("agent server: a step is asked of the provider chosen, with the key kept for it, the model and effort chosen, and the conversation as it was given",
        code == 200 and answer["content"][0]["text"] == "heard" and answer["stop_reason"] == "end_turn" and key == SECRET and request["model"] == "echo-1" and request["effort"] == "low"
        and request["system"] == "S" and request["tools"] == asked["tools"] and request["messages"] == asked["messages"] and SECRET not in json.dumps(answer))

    bad = [ask("POST", "messages", {}), ask("POST", "messages", {"system": "S", "tools": [], "messages": []}), ask("POST", "messages", {"system": 1, "tools": [], "messages": [{}]})]
    say("agent server: a step without a system prompt, tools and messages is refused", all(code == 400 for _, code in bad) and len(Echo.asked) == 1)
    refused, code = ask("POST", "messages", {**asked, "messages": [{"role": "user", "content": "refuse"}]})
    say("agent server: a provider's refusal comes back with its reason and its status", code == 401 and "not accepted" in refused["message"])

    ask("POST", "settings", {"provider": "missing", "model": "echo-1"})
    away, code = ask("POST", "messages", asked)
    say("agent server: a provider that cannot be used here says why, and is not asked", code == 503 and "not installed" in away["message"])

    ask("POST", "settings", {"provider": "echo", "model": "echo-1", "key": ""})
    forgotten = json.loads((store / "ai-agent.json").read_text(encoding="utf-8"))
    ask("POST", "messages", asked)
    say("agent server: a key is forgotten when an empty one is given, and the provider is then asked without one (its environment's own is tried)",
        forgotten["keys"] == {} and Echo.asked[-1][0] is None and SECRET not in json.dumps(forgotten))
    nothing, code = ask("GET", "keys")
    say("agent server: it answers no other request", code == 404 and ask("DELETE", "settings")[1] == 404)

# The provider that is Claude asks through the Anthropic SDK, with the key it is given, and declares the models of the present family.
source = Path(sys.argv[1]).read_text(encoding="utf-8")
say("agent server: Claude is asked through the Anthropic SDK, and the file holds no key",
    "import anthropic" in source and "anthropic.Anthropic(api_key=key)" in source and "sk-ant-" + "api" not in source and "sk-ant-" + "usr" not in source
    and server.DEFAULTS["model"] in [m["id"] for m in server.PROVIDERS["anthropic"].models])
