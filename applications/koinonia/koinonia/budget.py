"""What the society's model agents may spend, kept for the person at this computer (ADR 0169).

The ledger is ``koinonia-budget.json`` in the person's settings folder (``science.models.settings_folder()``), never under the
repository: ``{"budget_usd": 50, "stop_at_usd": 45, "spent_usd": 0, "calls": [...]}``. Before a call the largest it could cost
(its prompt at the input price, its whole limit of tokens at the output price) is reserved; a call that could pass the stop is
refused with the reason, and nothing is asked of the provider. After the call its cost is counted from the tokens the provider
reports. Prices are in dollars per million tokens, the provider's published prices as of 2026-11-03 (the claude-api reference
kept with Claude Code); thinking is counted as output.

A ledger made with ``path=None`` is kept in memory only, for checks and dry runs.
"""

from __future__ import annotations

import json
import threading
from pathlib import Path

# Dollars per million tokens: input, output, cache written (five minutes), cache read.
PRICES = {
    "claude-opus-5-5": (4.00, 20.00, 5.00, 0.20),
    "claude-sonnet-5-5": (2.00, 10.00, 2.50, 0.10),
    "claude-haiku-5-5": (0.10, 0.50, 0.125, 0.01),
    "claude-fable-5-2": (8.00, 40.00, 10.00, 0.20),
}
CHARS_PER_TOKEN = 3.0  # a cautious reading of a prompt's length in tokens, so that the reserve is never short


class Exhausted(Exception):
    """A call refused before it was made, because it could pass the stop."""


def price(model: str) -> tuple[float, float, float, float]:
    if model not in PRICES:
        raise Exhausted(f"no price is known for {model}; a call to it cannot be bounded")
    return PRICES[model]


def cost(usage: dict, model: str) -> float:
    """The dollars a call cost, from the tokens its provider reported."""
    pin, pout, pwrite, pread = price(model)
    return (usage.get("input_tokens", 0) * pin + usage.get("output_tokens", 0) * pout
            + usage.get("cache_creation_input_tokens", 0) * pwrite + usage.get("cache_read_input_tokens", 0) * pread) / 1e6


class Ledger:
    def __init__(self, path: Path | None, budget_usd: float = 50.0, stop_at_usd: float = 45.0):
        self.path, self.lock = path, threading.Lock()
        self.data = {"budget_usd": budget_usd, "stop_at_usd": stop_at_usd, "spent_usd": 0.0, "calls": []}
        if path is not None and path.is_file():
            try:
                self.data.update(json.loads(path.read_text(encoding="utf-8-sig")))
            except ValueError:
                pass
        self.reserved = 0.0

    @classmethod
    def kept(cls) -> "Ledger":
        from science.models import settings_folder
        return cls(settings_folder() / "koinonia-budget.json")

    def remaining(self) -> float:
        return self.data["stop_at_usd"] - self.data["spent_usd"] - self.reserved

    def _reread(self) -> None:
        """Takes the spending another process (the server, a case run) has written since: what is spent is the larger of the two."""
        if self.path is None or not self.path.is_file():
            return
        try:
            kept = json.loads(self.path.read_text(encoding="utf-8-sig"))
        except ValueError:
            return
        if float(kept.get("spent_usd", 0)) > self.data["spent_usd"]:
            self.data["spent_usd"] = float(kept["spent_usd"])
            self.data["calls"] = kept.get("calls", self.data["calls"])

    def reserve(self, model: str, prompt_chars: int, max_tokens: int) -> float:
        pin, pout, _, _ = price(model)
        most = (prompt_chars / CHARS_PER_TOKEN) * pin / 1e6 + max_tokens * pout / 1e6
        with self.lock:
            self._reread()
            if self.data["spent_usd"] + self.reserved + most > self.data["stop_at_usd"]:
                raise Exhausted(f"a call could cost up to ${most:.3f}, and ${self.data['spent_usd']:.2f} of the ${self.data['stop_at_usd']:.2f} "
                                f"allowed is spent")
            self.reserved += most
        return most

    def settle(self, held: float, model: str, usage: dict, who: str, what: str) -> float:
        spent = cost(usage, model) if usage else 0.0
        with self.lock:
            self._reread()
            self.reserved = max(0.0, self.reserved - held)
            self.data["spent_usd"] = round(self.data["spent_usd"] + spent, 6)
            self.data["calls"].append({"who": who, "what": what, "model": model, "usd": round(spent, 6),
                                       **{k: usage.get(k, 0) for k in ("input_tokens", "output_tokens", "cache_read_input_tokens")}})
            self.data["calls"] = self.data["calls"][-1000:]
            self._keep()
        return spent

    def summary(self) -> dict:
        with self.lock:
            return {"budget_usd": self.data["budget_usd"], "stop_at_usd": self.data["stop_at_usd"], "spent_usd": round(self.data["spent_usd"], 4),
                    "calls": len(self.data["calls"]), "kept": self.path is not None}

    def _keep(self) -> None:
        if self.path is None:
            return
        self.path.parent.mkdir(parents=True, exist_ok=True)
        temp = self.path.with_suffix(".tmp")
        temp.write_text(json.dumps(self.data, indent=1), encoding="utf-8")
        temp.replace(self.path)
