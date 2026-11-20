"""The prototypes' one way to a language model: the infrastructure's own provider (api/python/science/models.py).

The key is read from the environment (ECOLOGY_MODEL_KEY) or, failing that, from the person's settings folder,
as the provider does; it is never written in this folder. Every call is counted, with its tokens, in a ledger
that the prototype's record reports.
"""
from __future__ import annotations

import json
import os
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(ROOT / "api" / "python"))
from science import models  # noqa: E402

CHEAP = "claude-haiku-5-5"
CAREFUL = "claude-sonnet-5-5"
LEDGER: list[dict] = []
TEXT = {"type": "object", "properties": {"text": {"type": "string"}}, "required": ["text"], "additionalProperties": False}


def ask(system: str, prompt: str, schema: dict = TEXT, *, model: str = CHEAP, effort: str = "low", max_tokens: int = 6000, tries: int = 3) -> dict:
    """One answer as JSON meeting `schema`. A refusal is tried again a few times and then raised."""
    provider = models.Anthropic(model=model, effort=effort, key=os.environ.get("ECOLOGY_MODEL_KEY") or None, max_tokens=max_tokens)
    last = None
    for attempt in range(tries):
        started = time.time()
        try:
            data, served = provider.answer(system, prompt, schema)
        except models.Refusal as refusal:
            last = refusal
            LEDGER.append({"model": model, "ok": False, "why": str(refusal)[:160], "seconds": round(time.time() - started, 1)})
            time.sleep(2 + 3 * attempt)
            continue
        LEDGER.append({"model": served, "ok": True, "seconds": round(time.time() - started, 1),
                       "chars_in": len(system) + len(prompt), "chars_out": len(json.dumps(data))})
        return data
    raise last


def spent() -> dict:
    """What the calls so far came to: counts, and characters in and out (tokens are about a quarter of these)."""
    ok = [c for c in LEDGER if c["ok"]]
    return {"calls": len(LEDGER), "answered": len(ok), "chars_in": sum(c["chars_in"] for c in ok),
            "chars_out": sum(c["chars_out"] for c in ok), "seconds": round(sum(c["seconds"] for c in LEDGER), 1)}


if __name__ == "__main__":
    print(ask("You answer in one short sentence.", "Name one property that a checksum has."), spent())
