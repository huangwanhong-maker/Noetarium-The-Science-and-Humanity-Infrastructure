"""Every participant's behaviour, recorded and sealed (ADR 0169).

A community's chronicle records the acts its rules admitted; this journal records everything a participant of the society did and
why, whether an agent or a person: each time an agent woke and what woke it, what it decided (the acts it put forward, the options a
model was shown, what it chose and its reason), every call to a model (the instructions by their digest, the prompt, the answer,
the tokens and the cost, or why there was no answer), every act put to a community and its outcome (admitted with the chronicle's
seal, or refused with the reason), every note, every act a person took from the page, and every change to the clock.

Entries are sealed by archeion's rule (``science.archive.sealed``): each entry's seal is the SHA-256 of the seal before it and of
its fields, its note the SHA-256 of the entry's canonical JSON, so that an entry changed, removed or put out of order breaks every
seal after it, and ``verify`` names the first. A model's instructions are recorded once under their digest. The journal is written
line by line as the society runs (``behaviour.jsonl`` in the society's folder), and the case records verify it with archeion.
"""

from __future__ import annotations

import hashlib
import json
import threading
from pathlib import Path

from science import archive


def canonical(value) -> str:
    return json.dumps(value, sort_keys=True, ensure_ascii=False, separators=(",", ":"))


def digest(value) -> str:
    return hashlib.sha256(canonical(value).encode("utf-8")).hexdigest()


class Behaviour:
    def __init__(self, path: Path | None):
        self.path, self.lock = path, threading.Lock()
        self.tail: list[dict] = []
        self.head, self.n = archive.GENESIS, 0
        self.known: set[str] = set()  # digests of instructions already recorded
        if path is not None and path.is_file():
            for line in path.read_text(encoding="utf-8").splitlines():
                e = json.loads(line)
                self.head, self.n = e["seal"], e["n"]
                if e["what"] == "instructions":
                    self.known.add(e["data"]["digest"])
                self.tail.append(e)
            self.tail = self.tail[-4000:]

    def record(self, turn: int, who: str, what: str, about: str = "", **data) -> dict:
        with self.lock:
            self.n += 1
            body = {"n": self.n, "turn": turn, "who": who, "what": what, "about": about, "data": data}
            sealed = archive.sealed(self.head, when=f"{turn:06d}.{self.n:07d}", who=who, what=what, about=about, note=digest(body))
            entry = {**body, "seal": sealed["seal"]}
            self.head = entry["seal"]
            self.tail.append(entry)
            if len(self.tail) > 6000:
                self.tail = self.tail[-4000:]
            if self.path is not None:
                self.path.parent.mkdir(parents=True, exist_ok=True)
                with self.path.open("a", encoding="utf-8") as f:
                    f.write(json.dumps(entry, ensure_ascii=False) + "\n")
            return entry

    def instructions(self, turn: int, who: str, text: str) -> str:
        """Records a model's instructions once, and returns their digest."""
        d = hashlib.sha256(text.encode("utf-8")).hexdigest()
        if d not in self.known:
            self.known.add(d)
            self.record(turn, who, "instructions", d[:16], digest=d, text=text)
        return d

    def of(self, who: str, limit: int = 200) -> list[dict]:
        return [e for e in self.tail if e["who"] == who or e["data"].get("for") == who][-limit:]


def entries_of(path: Path) -> list[dict]:
    return [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines()] if path.is_file() else []


def verify(entries: list[dict], head: str | None = None, count: int | None = None) -> dict:
    """Recomputes every note and seal; the first entry that does not hold, or none. Given the head and count the society kept, a
    journal whose last entries were removed is found too: every prefix of a sealed chain verifies, but it ends at another seal."""
    seal = archive.GENESIS
    for k, e in enumerate(entries):
        body = {key: e[key] for key in ("n", "turn", "who", "what", "about", "data")}
        expect = archive.sealed(seal, when=f"{e['turn']:06d}.{e['n']:07d}", who=e["who"], what=e["what"], about=e["about"], note=digest(body))["seal"]
        if expect != e["seal"] or e["n"] != k + 1:
            return {"holds": False, "entries": len(entries), "first fault": e["n"], "at": k}
        seal = e["seal"]
    if (head is not None and seal != head) or (count is not None and len(entries) != count):
        return {"holds": False, "entries": len(entries), "first fault": len(entries) + 1, "at": len(entries), "why": "the journal ends before the head kept"}
    return {"holds": True, "entries": len(entries), "head": seal}


def as_journal(entries: list[dict]) -> list[dict]:
    """The entries as archeion's journal reads them (when, who, what, about, note, seal), for its verification."""
    out = []
    for e in entries:
        body = {key: e[key] for key in ("n", "turn", "who", "what", "about", "data")}
        out.append({"when": f"{e['turn']:06d}.{e['n']:07d}", "who": e["who"], "what": e["what"], "about": e["about"], "note": digest(body), "seal": e["seal"]})
    return out
