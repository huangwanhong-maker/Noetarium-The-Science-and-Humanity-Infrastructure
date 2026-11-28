"""Observe a book agent's saved wait using the same access the gateway grants that agent."""

from __future__ import annotations

import asyncio
from contextlib import ExitStack, contextmanager
from datetime import datetime, timezone
import hashlib
import http.client
import json
import os
import time
from urllib.parse import quote


def digest(value):
    data = json.dumps(value, sort_keys=True, ensure_ascii=False, separators=(",", ":"))
    return hashlib.sha256(data.encode("utf-8")).hexdigest()


def now():
    return datetime.now(timezone.utc).isoformat()


def item_path(item):
    if not isinstance(item, str) or not 1 <= len(item) <= 160 or item in {".", ".."}:
        raise ValueError("Invalid community item ID")
    if not all(c.isalnum() or c in "-_~. " for c in item):
        raise ValueError("Invalid character in a community item ID")
    return "/c/completion/items/" + quote(item, safe="")


def observe(state, status, value):
    """Update one wait from its owner's GET. Return true exactly when the wait should wake."""
    item = state["arguments"]["item"]
    if status == 200:
        if not isinstance(value, dict) or value.get("id") != item:
            raise ValueError("Community returned a different item")
        status_text = value.get("status") or value.get("state")
        if not status_text:
            raise ValueError("Community item is missing its status")
        key = digest(value)
    elif status == 404:
        status_text, key = "unavailable", digest(None)
    elif status in (401, 403):
        status_text, key = "access refused", digest({"http": status})
    else:
        raise OSError(f"Owner item read returned HTTP {status}")

    old = state.get("baseline")
    legacy_missing = state.get("source") != "own-item" and old == digest(None)
    first_read = old is None or legacy_missing
    # Applications and questions report their disposition as 'status'. The gateway uses
    # 'state' for posted work. An already disposed question must survive a missed transition.
    disposed = status == 200 and "status" in value and value["status"] != "open"
    ready = (disposed or status in (401, 403)) if first_read else old != key
    if first_read and key == state.get("lastSignal"):
        ready = False
    state.update(source="own-item", baseline=key)
    if ready:
        state.update(pending=False, lastSignal=key, woke=now(),
                     details={"item": item, "state": status_text, "http": status})
    return ready


class SharedHead:
    """Use one public chronicle head for a polling period. The head contains no private items."""

    def __init__(self, read, period):
        self.read, self.period = read, period
        self.lock = asyncio.Lock()
        self.head = None
        self.until = 0

    async def current(self):
        async with self.lock:
            if time.monotonic() >= self.until:
                # Back off failed reads too; many waiting agents can observe the same failure.
                self.until = time.monotonic() + self.period
                self.head = None
                self.head = await self.read()
            if self.head is None:
                raise OSError("Community head unavailable")
            return self.head


def owner_transport(transport, public_read):
    """Adapt the campaign transport. Authenticated reads never overwrite a model's @last view."""

    class OwnerTransport(transport):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)
            self.owner_heads = SharedHead(self.read_head, self.poll_seconds)
            self.owner_lane = asyncio.Semaphore(2)
            self.owner_closing = asyncio.Event()

        def save_wait(self, identity, state):
            state["recoveryArmed"] = True
            super().save_wait(identity, state)

        async def read_head(self):
            reply = await self.bounded(public_read, "/c/completion/head", self.logs)
            return reply["head"]["root"]

        async def read_item(self, identity, item):
            path = item_path(item)
            async with self.owner_lane:
                status, _, value = await self.bounded(self.client(identity).request, "GET", path, path, None)
            return status, value

        async def wait(self, identity, arguments):
            if arguments.get("until") != "item_change":
                return await super().wait(identity, arguments)
            if set(arguments) != {"until", "item"}:
                raise ValueError("An item-change wait requires one item ID")
            item = arguments["item"]
            item_path(item)
            async with self.wait_locks[identity]:
                previous = self.load_wait(identity)
                same = previous.get("arguments") == arguments
                if same and previous.get("pending"):
                    state = previous
                else:
                    state = {"arguments": arguments, "since": now(), "pending": True,
                             "lastSignal": previous.get("lastSignal") if same else None}
                self.save_wait(identity, state)
                self.pending[identity] = "item_change"
                last_head = None
                try:
                    while not self.closed:
                        try:
                            head = await self.owner_heads.current()
                            if last_head != head:
                                status, value = await self.read_item(identity, item)
                                before = dict(state)
                                ready = observe(state, status, value)
                                if state != before:
                                    self.save_wait(identity, state)
                                last_head = head
                                if ready:
                                    return {"condition": "item_change", "since": state["since"],
                                            "woke": state["woke"], **state["details"]}
                        except (OSError, http.client.HTTPException, KeyError, ValueError):
                            # Keep the last valid baseline until this owner can read the item again.
                            pass
                        try:
                            await asyncio.wait_for(self.owner_closing.wait(), timeout=self.poll_seconds)
                        except asyncio.TimeoutError:
                            pass
                    raise RuntimeError("Dispatcher closing; the owner's wait is saved")
                finally:
                    self.pending.pop(identity, None)

        async def close(self):
            self.owner_closing.set()
            await super().close()

    return OwnerTransport


def load(path):
    return json.loads(path.read_bytes()) if path.is_file() else {}


def saved_waits(run, roster):
    """Select pending or unclaimed signals from the roster, including ended original turns."""
    waits = {}
    for identity in roster:
        state = load(run / "wait-state" / (identity + ".json"))
        if not state.get("pending") and not state.get("woke"):
            continue
        log = run / "io-log" / (identity + ".jsonl")
        left = False
        if log.is_file():
            for line in log.read_text(encoding="utf-8").splitlines():
                if line.endswith("}"):
                    receipt = json.loads(line)
                    if receipt.get("admitted") is True:
                        if receipt.get("kind") == "leave":
                            left = True
                        elif receipt.get("path", "").endswith("/present"):
                            left = False
        if left:
            continue
        # Prior hosts did not persist signal acknowledgements. Only an outstanding legacy wait
        # is safe to restore; an older 'woke' file may have already resumed its owner.
        if not state.get("pending") and not state.get("recoveryArmed"):
            continue
        if state.get("resumeAttempt") == state.get("woke") and state.get("woke"):
            continue
        waits[identity] = state
    return waits


@contextmanager
def dispatcher_locks(run):
    """A replacement owns both original slots. A probe never opens either lock."""
    with ExitStack() as stack:
        for name in ("cohort", "recovery"):
            handle = stack.enter_context((run / (name + ".lock")).open("a+b"))
            handle.seek(0)
            try:
                if os.name == "nt":
                    import msvcrt
                    msvcrt.locking(handle.fileno(), msvcrt.LK_NBLCK, 1)
                else:
                    import fcntl
                    fcntl.flock(handle, fcntl.LOCK_EX | fcntl.LOCK_NB)
            except OSError as exc:
                raise RuntimeError(f"The {name} dispatcher still owns its lock") from exc
        yield


def mark_attempt(io, identity, signal):
    """Write before a paid turn. An ambiguous crash at submission requires receipt reconciliation."""
    state = io.load_wait(identity)
    if state.get("pending") or state.get("woke") != signal.get("woke") or state.get("resumeAttempt") == signal["woke"]:
        return False
    state["resumeAttempt"] = signal["woke"]
    io.save_wait(identity, state)
    return True
