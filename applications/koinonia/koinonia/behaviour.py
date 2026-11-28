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


# ---- Episodes of conduct (the design of 2026-11-27, Section 3.5) ------------------------------------------------------------------
#
# An episode is what a participant did, as the node saw it (grade A), as the agent reported it (grade C) or as an observer attested it
# (grade B), in the features the society's base taxonomy declares: an operation from an open vocabulary, resources in their units,
# the digests of its inputs and outputs, the holdings and items it touched, the provider and model, the community and node, its
# instants. Each community keeps its episodes as a series of its own beside its chronicle, in the API's session (Phase 9): an act the
# gateway submits carries its episode, which the engine records first, then the act with the episode's digest in its field "episode",
# then the outcome (Definition 31). The episode holds no text: texts are kept as payloads by their digests (``Payloads``), so that
# retention can remove what its schedule allows while the series still seals.

OPERATIONS = ("run tool", "call model", "read", "write", "send", "compute", "wait", "message", "act", "check status", "issue passport", "other")
GRADES = ("A", "B", "C")


def episode(party: str, operation: str, *, community: str = "", node: str = "", route: str = "", method: str = "", turn: int = 0,
            begun: str = "", ended: str = "", provider: str = "", model: str = "", bearer: str = "", resources: dict | None = None,
            inputs=(), outputs=(), holdings=(), items=(), grade: str = "A", address: str = "", **features) -> dict:
    """An episode as the API's engine reads it (``science.communities``: {party, bearer, provider, model, community, node, operation,
    kind, turn, begun, ended, resources, inputs, outputs, holdings, items, features}), the operation one of the vocabulary or "other"
    with a description in the feature "described". Addresses are kept only as local or remote (the design's Section 9.5)."""
    if grade not in GRADES:
        raise ValueError(f"an episode's grade is A, B or C, not {grade!r}")
    if address not in ("", "local", "remote"):
        raise ValueError("an address is kept only as local or remote")
    if operation not in OPERATIONS:
        features.setdefault("described", operation)
        operation = "other"
    feats = {"grade": grade, **({"route": route} if route else {}), **({"method": method} if method else {}), **({"address": address} if address else {})}
    for k, v in features.items():
        if v is None:
            continue
        feats[k.replace("_", " ")] = v if isinstance(v, (str, int, float, bool)) else canonical(v)
    return {"party": party, "bearer": bearer, "provider": provider, "model": model, "community": community, "node": node, "operation": operation,
            "kind": "request" if route else "conduct", "turn": int(turn), "begun": begun, "ended": ended,
            "resources": {k: int(v) for k, v in (resources or {}).items()}, "inputs": sorted(set(inputs)), "outputs": sorted(set(outputs)),
            "holdings": sorted(set(holdings)), "items": sorted(set(items)), "features": feats}


def fields_of(about: str) -> dict:
    """The fields of an entry of a community's series (``key:length:value;`` as thesmos's conduct writes them), read back."""
    out, k = {}, 0
    while k < len(about):
        colon = about.find(":", k)
        if colon < 0:
            break
        second = about.find(":", colon + 1)
        if second < 0:
            break
        key, n = about[k:colon], int(about[colon + 1:second])
        out[key] = about[second + 1:second + 1 + n]
        k = second + 1 + n + 1
    return out


# ---- A Merkle tree over a journal, by archeion's rule ---------------------------------------------------------------------------------
#
# archeion's fixity (journal.hpp, after RFC 6962, §2.1): a leaf's hash is H(0x00 ‖ data), a node's H(0x01 ‖ left ‖ right), the tree over n
# leaves split at the largest power of two less than n, the root of no leaves H(""). thesmos's crossings take an entry's leaf to be its
# seal, the record separator and its fields (``leaf``). Written here so that a node can prove an episode's inclusion as it works; the
# checks hold it to the engine's head of each community's series.

def _h(data: bytes) -> bytes:
    return hashlib.sha256(data).digest()


def leaf(entry: dict) -> str:
    return entry["seal"] + "\x1e" + entry.get("about", "")


def _split(n: int) -> int:
    k = 1
    while 2 * k < n:
        k *= 2
    return k


def _root(hashes: list[bytes], lo: int, n: int) -> bytes:
    if n == 1:
        return hashes[lo]
    k = _split(n)
    return _h(b"\x01" + _root(hashes, lo, k) + _root(hashes, lo + k, n - k))


def merkle_root(data: list[str]) -> str:
    if not data:
        return hashlib.sha256(b"").hexdigest()
    return _root([_h(b"\x00" + d.encode("utf-8")) for d in data], 0, len(data)).hex()


def prove(data: list[str], index: int) -> dict:
    """The proof that the datum at ``index`` is under the root: {leaf, size, path}, the siblings from the leaf upward."""
    if not 0 <= index < len(data):
        raise IndexError("no such datum")
    hashes = [_h(b"\x00" + d.encode("utf-8")) for d in data]
    path: list[bytes] = []

    def walk(lo: int, n: int, i: int) -> None:
        if n == 1:
            return
        k = _split(n)
        if i < k:
            walk(lo, k, i)
            path.append(_root(hashes, lo + k, n - k))
        else:
            walk(lo + k, n - k, i - k)
            path.append(_root(hashes, lo, k))
    walk(0, len(hashes), index)
    return {"leaf": index, "size": len(data), "path": [p.hex() for p in path]}


def included(datum: str, proof: dict, root: str) -> bool:
    """Whether the proof shows the datum under the root (RFC 6962, §2.1.1, read upward)."""
    n, i = proof["size"], proof["leaf"]
    if not 0 <= i < n:
        return False
    right = []
    while n > 1:
        k = _split(n)
        if i < k:
            right.append(False)
            n = k
        else:
            right.append(True)
            i, n = i - k, n - k
    if len(right) != len(proof["path"]):
        return False
    h = _h(b"\x00" + datum.encode("utf-8"))
    for s, sibling in enumerate(proof["path"]):
        sib = bytes.fromhex(sibling)
        h = _h(b"\x01" + sib + h) if right[len(right) - 1 - s] else _h(b"\x01" + h + sib)
    return h.hex() == root


def series_head(entries: list[dict]) -> dict:
    """The head of a community's series of conduct, as the engine gives it: its size, Merkle root and last seal."""
    return {"size": len(entries), "root": merkle_root([leaf(e) for e in entries]), "seal": entries[-1]["seal"] if entries else archive.GENESIS}


def node_head(entries: list[dict]) -> dict:
    """The head of the node's own journal (``behaviour.jsonl``) under the same tree, its entries read as archeion's journal."""
    return series_head(as_journal(entries))


# ---- Payloads, retention and access (the design's Section 4.1) -------------------------------------------------------------------
#
# The series seal what was done; what was said (a request's body, an answer, a tool's output) is a payload kept by its SHA-256 beside
# them, each with its class of record. Retention follows a schedule the community declares in its policy of records: for each class a
# trigger and a period in years, and what is done when it ends. A record's disposition at a date is archeion's (retention.hpp, ``due``:
# no rule, unscheduled; no trigger yet, untriggered; a hold in force, held; else due on the trigger's date plus the period, or retained),
# mirrored here because the API does not yet answer it. Retention removes a payload only when it is due for destruction, leaving a
# marker; a series is never cut, so that it still seals and every act is still joined to its episode.

DEFAULT_SCHEDULE = [
    {"class": "act of force", "trigger": "created", "years": None, "then": "keep"},             # kept permanently
    {"class": "constitutive episode", "trigger": "created", "years": 10, "then": "destroy"},   # constituted an act, or touched holdings not public
    {"class": "reading of public content", "trigger": "created", "years": 1, "then": "destroy"},
    {"class": "exit package", "trigger": "created", "years": 10, "then": "destroy"},
]


def _add_years(date: str, years: int) -> str:
    y = int(date[:4]) + years
    out = f"{y:04d}{date[4:]}"
    try:
        import datetime as _dt
        _dt.date.fromisoformat(out)
    except ValueError:  # 29 February in a year without one
        out = out[:8] + "28"
    return out


def due(schedule: list[dict], record: dict, holds: list[dict], today: str) -> dict:
    """A record's disposition at ``today`` under the schedule (archeion's ``retention::due``): {state, on, action, reason}, the state
    one of retained, due, held, unscheduled, untriggered. A rule with no period keeps its class permanently."""
    rule = next((r for r in schedule if r["class"] == record["class"]), None)
    if rule is None:
        return {"state": "unscheduled", "on": "", "action": "review", "reason": f"the schedule has no rule for the class {record['class']}: nothing may be done with it until one is stated"}
    if rule.get("years") is None:
        return {"state": "retained", "on": "", "action": "keep", "reason": f"the class {record['class']} is kept permanently"}
    happened = record.get("events", {}).get(rule["trigger"])
    if not happened:
        return {"state": "untriggered", "on": "", "action": rule["then"], "reason": f"its period runs from \"{rule['trigger']}\", which has not happened: it is kept"}
    on = _add_years(happened[:10], int(rule["years"]))
    for h in holds:
        if h["record"] == record["id"] and h.get("from", "") <= today and (not h.get("until") or today <= h["until"]):
            return {"state": "held", "on": on, "action": rule["then"], "reason": f"the hold \"{h['name']}\" suspends its disposal" + (f", which fell due on {on}" if today >= on else "")}
    if today >= on:
        return {"state": "due", "on": on, "action": rule["then"], "reason": f"its period under the rule for {record['class']} ended on {on}"}
    return {"state": "retained", "on": on, "action": rule["then"], "reason": f"its period ends on {on}"}


class Payloads:
    """What was said, kept by its SHA-256 beside a community's series, with its class of record, the party it concerns and when it was
    made; removed only by retention, which leaves a marker naming the rule."""

    def __init__(self, folder: Path | None):
        self.folder = folder
        self.index: dict[str, dict] = {}
        self.markers: list[dict] = []
        self.memory: dict[str, str] = {}
        if folder is not None and (folder / "index.json").is_file():
            kept = json.loads((folder / "index.json").read_text(encoding="utf-8"))
            self.index, self.markers = kept.get("index", {}), kept.get("markers", [])

    def put(self, value, record_class: str, party: str, created: str) -> str:
        text = canonical(value)
        d = hashlib.sha256(text.encode("utf-8")).hexdigest()
        if d not in self.index:
            self.index[d] = {"id": d, "class": record_class, "party": party, "events": {"created": created}}
            if self.folder is not None:
                self.folder.mkdir(parents=True, exist_ok=True)
                (self.folder / f"{d}.json").write_text(text, encoding="utf-8")
            else:
                self.memory[d] = text
            self.save()
        return d

    def get(self, d: str):
        if d not in self.index:
            return None
        if self.folder is not None:
            p = self.folder / f"{d}.json"
            return json.loads(p.read_text(encoding="utf-8")) if p.is_file() else None
        return json.loads(self.memory[d]) if d in self.memory else None

    def save(self) -> None:
        if self.folder is not None:
            self.folder.mkdir(parents=True, exist_ok=True)
            (self.folder / "index.json").write_text(json.dumps({"index": self.index, "markers": self.markers}, ensure_ascii=False), encoding="utf-8")

    def retain(self, schedule: list[dict], holds: list[dict], today: str) -> dict:
        """Applies the schedule at ``today``: every payload due for destruction is removed and a marker left; what is retained,
        held, unscheduled or untriggered stays. Answers {removed, kept: {state: n}}."""
        removed, kept = [], {}
        for d, rec in list(self.index.items()):
            disp = due(schedule, rec, holds, today)
            if disp["state"] == "due" and disp["action"] == "destroy":
                if self.folder is not None:
                    (self.folder / f"{d}.json").unlink(missing_ok=True)
                self.memory.pop(d, None)
                del self.index[d]
                marker = {"id": d, "class": rec["class"], "party": rec["party"], "removed": today, "reason": disp["reason"]}
                self.markers.append(marker)
                removed.append(marker)
            else:
                kept[disp["state"]] = kept.get(disp["state"], 0) + 1
        self.save()
        return {"removed": removed, "kept": kept}


def may_read(reader: str, entry_party: str, controllers: dict[str, str], offices: set[str]) -> bool:
    """Access to an entry of a community's series by its policy of records under the binding-record principle (the design's
    Section 4.6): a party reads the records that are its own or its operatives' (up its chain of control), and the offices the policy
    names read all; the public reads none."""
    if not reader or reader == "public":
        return False
    if reader in offices:
        return True
    seen, who = set(), entry_party
    while who and who not in seen:
        if who == reader:
            return True
        seen.add(who)
        who = controllers.get(who, "")
    return False


def arrangement(node: str, communities: dict[str, dict]) -> list[dict]:
    """The archive as archeion arranges it (``science.archive`` "arrange"): a fonds for the node and one for each community, with series
    for its chronicle, the episodes concerning it, its holdings, its register of rulings and the exit packages issued, the original
    order kept as the sequence of entries. ``communities``: name → {acts, entries, holdings, rulings, exits}."""
    units = [{"id": "node", "level": "fonds", "code": "N", "title": f"the node {node}", "creator": f"the operator of the node {node}"},
             {"id": "node/journal", "level": "series", "code": "1", "parent": "node", "title": "the node's journal"}]
    for k, (name, c) in enumerate(sorted(communities.items()), 1):
        f = f"c/{name}"
        units.append({"id": f, "level": "fonds", "code": f"C{k}", "title": f"the community {name}", "creator": f"the community {name}"})
        for j, (series, title) in enumerate((("chronicle", "its chronicle"), ("conduct", "the episodes concerning it"), ("holdings", "its holdings"),
                                             ("rulings", "its register of rulings"), ("exits", "the exit packages issued")), 1):
            units.append({"id": f"{f}/{series}", "level": "series", "code": str(j), "parent": f, "title": f"{title} ({c.get(series, 0)})"})
    return units
