"""Per-agent dynamic tools for one shared Codex app-server dispatcher."""

import asyncio
import hashlib
import http.client
import json
import os
import re
import time
import uuid
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path
from urllib.parse import parse_qs, urlsplit

from ..dispatch_waits import owner_transport
from .client import BASE, ID, TOOLS, Community, clean, dumps, gateway_path


WAIT = {
    "name": "wait_for_community",
    "description": "Wait for eligible work or a change. If the response says parked:true, end your current turn immediately. The dispatcher watches without model turns and wakes this same agent when the condition matches. The tool takes no work or acts for you.",
    "inputSchema": {"type": "object", "properties": {
        "until": {"type": "string", "enum": ["eligible", "item_change", "work_change", "community_change"], "description": "Defaults to eligible for your issued role."},
        "item": {"type": "string", "description": "Work, question, or application ID for item_change. The owner-scoped items route supplies its state."},
    }, "additionalProperties": False},
}
AWAIT = {"name": "await_community_request", "description": "Collect an outstanding node request. Pass the request_id returned by community_read or community_act. A pending receipt means the original operation is still running; do not send it again.", "inputSchema": {"type": "object", "properties": {"request_id": {"type": "string"}}, "required": ["request_id"], "additionalProperties": False}}
DYNAMIC_TOOLS = [{"type": "function", **tool} for tool in [*TOOLS, WAIT, AWAIT]]


def now():
    return datetime.now(timezone.utc).isoformat()


def signature(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, ensure_ascii=False, separators=(",", ":")).encode()).hexdigest()


def audit(directory, name, row):
    with (directory / (name + ".jsonl")).open("a", encoding="utf-8", newline="\n") as log:
        log.write(dumps({"at": now(), **row}) + "\n")


def result(value, ok=True):
    return {"success": ok, "contentItems": [{"type": "inputText", "text": dumps(value)}]}


class LoggedCommunity(Community):
    def __init__(self, identity, logs):
        super().__init__(identity, logs.parent)
        self.logs = logs

    def exchange(self, method, path, route, wire, headers, body):
        event = {"method": method, "path": route.replace(self.bearer, "[withheld]"), "status": None, "directContact": True, "authenticated": "Authorization" in headers}
        if method == "POST" and path == BASE + "/acts" and isinstance(body, dict):
            kind = body.get("kind")
            if isinstance(kind, str) and len(kind) < 50:
                event["kind"] = kind.replace(self.bearer, "[withheld]")
        try:
            response = super().exchange(method, path, route, wire, headers, body)
            event["status"] = response[0]
            if isinstance(response[2], dict):
                for key in ("admitted", "seal"):
                    val = response[2].get(key)
                    if isinstance(val, bool) or key == "seal" and isinstance(val, str) and len(val) == 64 and all(ch in "0123456789abcdef" for ch in val):
                        event[key] = val
                item = response[2].get("item")
                if isinstance(item, dict) and isinstance(item.get("state"), str):
                    event["state"] = item["state"][:50]
            return response
        finally:
            audit(self.logs, self.identity, event)


def public_read(path, logs):
    connection = http.client.HTTPConnection("127.0.0.1", 9230, timeout=100)
    status = None
    try:
        connection.request("GET", path, headers={"Accept": "application/json"})
        response = connection.getresponse()
        status = response.status
        if status != 200:
            raise OSError("Public node is temporarily unavailable.")
        return json.loads(response.read())
    finally:
        connection.close()
        audit(logs, "shared-monitor", {"method": "GET", "path": path, "status": status})


def work_snapshot(work):
    items = {item["id"]: item for item in work if isinstance(item, dict) and "id" in item}
    eligible = {"writer": [], "reviewer": []}
    for name, item in items.items():
        if item.get("state") != "open":
            continue
        if item.get("kind") in ("section", "overview"):
            eligible["writer"].append(name)
        elif item.get("kind") == "review":
            parent = items.get(item.get("of"), {})
            deliveries = parent.get("deliveries") or []
            provider = deliveries[-1].get("provider") if deliveries else None
            if provider and provider.lower() != "openai":
                eligible["reviewer"].append(name)
    for group in eligible.values():
        group.sort()
    return {"work": signature(items), "eligible": eligible}


class CampaignTransport:
    """Allocate one instance per dispatcher. All public snapshots are shared among pending waits."""

    def __init__(self, directory, concurrency=4, poll_seconds=25, request_window=20, park_window=10, public_ttl=30):
        self.directory = Path(directory)
        self.logs = self.directory / "io-log"
        self.waits = self.directory / "wait-state"
        self.logs.mkdir(parents=True, exist_ok=True)
        self.waits.mkdir(parents=True, exist_ok=True)
        self.nodes = asyncio.Semaphore(concurrency)
        self.long_polls = asyncio.Semaphore(1)
        self.locks = defaultdict(asyncio.Lock)
        self.wait_locks = defaultdict(asyncio.Lock)
        self.clients = {}
        self.pending = {}
        self.poll_seconds = poll_seconds
        self.monitor = None
        self.closed = False
        self.changed = asyncio.Event()
        self.snapshot = None
        self.revision = 0
        self.inflight = set()
        self.request_window = request_window
        self.park_window = park_window
        self.public_ttl = public_ttl
        self.public_cache = {}
        self.fetching = {}
        self.requests = {}
        self.operations = {}
        self.sleepers = {}
        self.requests_dir = self.directory / "request-state"
        self.requests_dir.mkdir(exist_ok=True)

    async def bounded(self, fn, *args, events=False):
        async def run():
            # Long polls get a single separate lane. The node's work and admissions always retain capacity.
            lane = self.long_polls if events else self.nodes
            async with lane:
                return await asyncio.to_thread(fn, *args)
        job = asyncio.create_task(run())
        self.inflight.add(job)
        def finish(task):
            self.inflight.discard(task)
            if not task.cancelled():
                task.exception()
        job.add_done_callback(finish)
        return await asyncio.shield(job)

    def state_file(self, identity):
        if not isinstance(identity, str) or not ID.fullmatch(identity):
            raise ValueError("Invalid campaign identity.")
        return self.waits / (identity + ".json")

    def save_wait(self, identity, state):
        path = self.state_file(identity)
        temp = path.with_suffix(".tmp")
        temp.write_text(dumps(state), encoding="utf-8")
        os.replace(temp, path)

    def load_wait(self, identity):
        path = self.state_file(identity)
        return json.loads(path.read_bytes()) if path.is_file() else {}

    async def track(self):
        delay = self.poll_seconds
        while not self.closed and self.pending:
            try:
                public = await self.bounded(public_read, BASE + "/work", self.logs)
                new = work_snapshot(public["work"])
                if "community_change" in self.pending.values():
                    data = await self.bounded(public_read, BASE + "/head", self.logs)
                    head = data["head"]
                    new["head"] = {"size": head["size"], "root": head["root"]}
                self.snapshot = new
                self.revision += 1
                self.changed.set()
                self.changed = asyncio.Event()
                delay = self.poll_seconds
            except (OSError, http.client.HTTPException, ValueError, KeyError, TypeError):
                delay = min(120, max(1, delay * 2))
            await asyncio.sleep(delay)

    async def wait(self, identity, arguments):
        if set(arguments) - {"until", "item"}:
            raise ValueError("Invalid wait arguments.")
        until = arguments.get("until", "eligible")
        item = arguments.get("item")
        if until not in {"eligible", "work_change", "community_change"}:
            raise ValueError("Unknown wait condition.")
        if item is not None:
            raise ValueError("Only item_change uses an item ID.")
        args = {"until": until}
        async with self.wait_locks[identity]:
            previous = self.load_wait(identity)
            same = previous.get("arguments") == args
            is_resume = same and previous.get("pending") is True
            state = previous if is_resume else {"arguments": args, "pending": True, "since": now(), "baseline": None,
                                                "lastSignal": previous.get("lastSignal") if same and until == "eligible" else None}
            self.save_wait(identity, state)
            self.pending[identity] = until
            # Establish a current baseline after registering. A durable baseline survives relaunch.
            register_revision = self.revision
            if self.monitor is None or self.monitor.done():
                self.monitor = asyncio.create_task(self.track())
            try:
                while not self.closed:
                    event = self.changed
                    snap = self.snapshot
                    if self.revision > register_revision and snap is not None and (until != "community_change" or "head" in snap):
                        details = {}
                        if until == "eligible":
                            role = identity.split("-")[1]
                            candidates = snap["eligible"][role]
                            key = signature(candidates)
                            ready = bool(candidates) and key != state["lastSignal"]
                            if not candidates and state["lastSignal"] is not None:
                                state["lastSignal"] = None
                                self.save_wait(identity, state)
                            if ready:
                                ranked = sorted(candidates, key=lambda name: signature([identity, name]))
                                details = {"count": len(candidates), "sampleIds": ranked[:10]}
                        else:
                            if until == "community_change":
                                details = {"head": snap["head"]}
                                key = signature(snap["head"])
                            else:
                                key = snap["work"]
                            if state["baseline"] is None:
                                state["baseline"] = key
                                self.save_wait(identity, state)
                            ready = key != state["baseline"]
                        if ready:
                            state.update(pending=False, lastSignal=key, woke=now(), details=details)
                            self.save_wait(identity, state)
                            return {"condition": until, "since": state["since"], "woke": state["woke"], **details}
                    await event.wait()
                raise RuntimeError("Dispatcher is closing; this waiting condition is saved.")
            finally:
                self.pending.pop(identity, None)

    async def resume_wait(self, identity):
        """Reattach a restored outstanding tool call to its persisted condition."""
        state = self.load_wait(identity)
        if state.get("pending") is not True:
            return result({"error": "This identity has no pending wait."}, False)
        return await self.call(identity, "wait_for_community", state["arguments"])

    def client(self, identity):
        if identity not in self.clients:
            self.clients[identity] = LoggedCommunity(identity, self.logs)
        return self.clients[identity]

    def discovery_route(self, raw):
        if not isinstance(raw, str) or not raw or any(ord(c) < 32 or ord(c) == 127 for c in raw):
            return None
        url = urlsplit(raw)
        path = url.path
        if url.query:
            return None
        profile = re.fullmatch(r"/c/([A-Za-z0-9_-]+)(?:/profile)?/?", path)
        if profile:
            path = "/c/" + profile.group(1) + "/profile"
        elif path.rstrip("/") in {"", "/c", "/.well-known/noetarium.json", "/c/platform-league/registry/registry"}:
            path = path.rstrip("/") or "/"
        else:
            return None
        return gateway_path("GET", url._replace(path=path).geturl())[1]

    async def public_view(self, identity, client, route, arguments):
        async def fetch():
            # The initiating agent makes a real, anonymous GET. No passport or bearer goes on it.
            try:
                response = await self.bounded(client.exchange, "GET", route, route, None, {"Accept": "application/json, text/markdown"}, None)
                entry = (time.monotonic(), now(), response)
                if response[0] == 200:
                    self.public_cache[route] = entry
                return entry
            finally:
                self.fetching.pop(route, None)
        entry = self.public_cache.get(route)
        if entry and time.monotonic() - entry[0] < self.public_ttl:
            source = "cached-public"
        else:
            job = self.fetching.get(route)
            if job is None:
                source = "public-fetch"
                job = self.fetching[route] = asyncio.create_task(fetch())
            else:
                source = "coalesced-public"
            entry = await asyncio.shield(job)
        _, stamp, (status, _, value) = entry
        receipt = {"source": source, "directContact": source == "public-fetch", "authenticated": False,
                   "canonicalPath": route, "retrievedAt": stamp}
        if source != "public-fetch":
            # Deliberately omit method and status: this agent did not contact the node.
            audit(self.logs, identity, {"event": "shared-public-response", **receipt, "publicStatus": status})
        client.last = (route, status, clean(value, client.bearer), None)
        client.last_public = receipt
        output = client.call("community_read", {**arguments, "path": "@last"})
        return {**output, "publicReceipt": receipt}

    async def execute(self, identity, tool_name, arguments):
        try:
            route = self.discovery_route(arguments.get("path")) if tool_name == "community_read" and arguments.get("path") != "@last" else None
            async with self.locks[identity]:
                client = self.client(identity)
                if route:
                    output = await self.public_view(identity, client, route, arguments)
                elif tool_name == "community_read" and arguments.get("path") == "@last":
                    output = client.call(tool_name, arguments)
                    receipt = getattr(client, "last_public", None)
                    if receipt:
                        output["publicReceipt"] = {**receipt, "source": "local-view", "directContact": False}
                else:
                    events = False
                    if tool_name == "community_read":
                        path, route = gateway_path("GET", arguments.get("path"))
                        events = path == BASE + "/events" and int(parse_qs(urlsplit(route).query).get("wait", ["0"])[0]) > 0
                    client.last_public = None
                    output = await self.bounded(client.call, tool_name, arguments, events=events)
                return result(output, output["status"] < 400), client.last, getattr(client, "last_public", None)
        except (KeyError, TypeError):
            return result({"error": "Invalid tool arguments or node response."}, False), None, None
        except (ValueError, RuntimeError) as exc:
            return result({"error": str(exc)}, False), None, None
        except (OSError, http.client.HTTPException):
            return result({"error": "Local node or storage did not answer. Check the item state before retrying a mutation."}, False), None, None

    def request_path(self, identity, rid):
        if not isinstance(rid, str) or not re.fullmatch(r"[a-f0-9]{32}", rid):
            raise ValueError("Invalid request_id.")
        return self.requests_dir / identity / (rid + ".json")

    def store_request(self, op, outcome=None):
        path = self.request_path(op["identity"], op["id"])
        path.parent.mkdir(exist_ok=True)
        data = {"request_id": op["id"], "fingerprint": op["key"], "started": op["started"], "host": os.getpid(),
                "tool": op["tool"], "complete": outcome is not None}
        if outcome is not None:
            data["response"], data["last"], data["publicReceipt"] = outcome
        temp = path.with_suffix("." + str(os.getpid()) + ".tmp")
        temp.write_text(dumps(data), encoding="utf-8")
        os.replace(temp, path)
        if outcome is None and op["tool"] == "community_act":
            marker = path.parent / (op["key"] + ".inflight")
            staged = marker.with_suffix("." + str(os.getpid()) + ".tmp")
            staged.write_text(op["id"], encoding="ascii")
            os.replace(staged, marker)

    def pending_receipt(self, rid):
        return result({"pending": True, "request_id": rid, "message": "Your original request continues. Call await_community_request with this request_id, or community_read with path @request/" + rid + ". Do not resubmit the original act."})

    async def collect(self, identity, rid, restore=False):
        path = self.request_path(identity, rid)
        op = self.requests.get((identity, rid))
        if op is None:
            if not path.is_file():
                return result({"error": "No such request for this identity."}, False)
            data = json.loads(path.read_bytes())
            if not data["complete"]:
                return result({"request_id": rid, "outcome": "unconfirmed", "message": "A previous dispatcher owned this request. Check your community record before any additional act. You can collect this ID again if that host is still running."}, False)
            outcome = data["response"], data["last"], data["publicReceipt"]
        else:
            done, _ = await asyncio.wait({op["job"]}, timeout=self.request_window)
            if not done:
                if not op["durable"]:
                    self.store_request(op)
                    op["durable"] = True
                return self.pending_receipt(rid)
            outcome = op["job"].result()
            if op["durable"]:
                self.store_request(op, outcome)
            if self.operations.get((identity, op["key"])) == rid:
                self.operations.pop((identity, op["key"]), None)
            self.requests.pop((identity, rid), None)
        key = op["key"] if op is not None else data["fingerprint"]
        marker = path.parent / (key + ".inflight")
        if marker.is_file() and marker.read_text(encoding="ascii") == rid:
            marker.unlink(missing_ok=True)
        response, snapshot, public_receipt = outcome
        if restore and snapshot is not None and not self.locks[identity].locked():
            client = self.client(identity)
            client.last = tuple(snapshot)
            client.last_public = public_receipt
        return response

    async def node_call(self, identity, tool_name, arguments):
        key = signature([tool_name, arguments])
        old = self.operations.get((identity, key))
        if old:
            return await self.collect(identity, old)
        if tool_name == "community_act":
            # A host crash can leave a mutation in flight. Never automatically send the same body again.
            marker = self.requests_dir / identity / (key + ".inflight")
            if marker.is_file():
                return await self.collect(identity, marker.read_text(encoding="ascii"), restore=True)
        rid = uuid.uuid4().hex
        job = asyncio.create_task(self.execute(identity, tool_name, arguments))
        op = {"identity": identity, "id": rid, "key": key, "started": now(), "tool": tool_name, "job": job, "durable": False}
        self.operations[identity, key] = rid
        self.requests[identity, rid] = op
        def completed(task):
            if op["durable"] and not task.cancelled():
                self.store_request(op, task.result())
        job.add_done_callback(completed)
        return await self.collect(identity, rid)

    async def wake(self, identity):
        """Scheduler-only: wait with no model call, even when the signal preceded this await."""
        self.state_file(identity)
        sleeper = self.sleepers.get(identity)
        if sleeper is None:
            state = self.load_wait(identity)
            if state.get("pending") is True:
                sleeper = self.sleepers[identity] = asyncio.create_task(self.wait(identity, state["arguments"]))
            elif state.get("woke"):
                return {"condition": state["arguments"]["until"], "since": state["since"], "woke": state["woke"], **state.get("details", {})}
            else:
                raise ValueError("No outstanding wait for this agent.")
        return await asyncio.shield(sleeper)

    async def call(self, identity, tool_name, arguments):
        try:
            self.state_file(identity)
            if self.closed or not isinstance(arguments, dict):
                raise ValueError("Invalid tool invocation or dispatcher closed.")
            if tool_name == "wait_for_community":
                sleeper = self.sleepers.get(identity)
                if sleeper is None or sleeper.done():
                    sleeper = self.sleepers[identity] = asyncio.create_task(self.wait(identity, arguments))
                done, _ = await asyncio.wait({sleeper}, timeout=self.park_window)
                if done:
                    return result(sleeper.result())
                return result({"parked": True, "condition": self.pending.get(identity), "message": "End your current turn now. The dispatcher watches without model calls and resumes this same agent when the condition matches."})
            if tool_name == "await_community_request":
                if set(arguments) != {"request_id"}:
                    raise ValueError("Supply only the request_id.")
                return await self.collect(identity, arguments["request_id"], restore=True)
            if tool_name == "community_read" and isinstance(arguments.get("path"), str) and arguments["path"].startswith("@request/"):
                return await self.collect(identity, arguments["path"][9:], restore=True)
            if tool_name not in ("community_read", "community_act"):
                raise ValueError("Unknown community tool.")
            return await self.node_call(identity, tool_name, arguments)
        except (KeyError, TypeError):
            return result({"error": "Invalid tool arguments or node response."}, False)
        except (ValueError, RuntimeError) as exc:
            return result({"error": str(exc)}, False)
        except (OSError, http.client.HTTPException):
            return result({"error": "Local node or storage did not answer. Check the item state before retrying a mutation."}, False)

    async def close(self):
        self.closed = True
        self.changed.set()
        if self.monitor is not None:
            self.monitor.cancel()
            await asyncio.gather(self.monitor, return_exceptions=True)
        if self.sleepers:
            await asyncio.gather(*self.sleepers.values(), return_exceptions=True)
        if self.requests:
            await asyncio.gather(*(op["job"] for op in list(self.requests.values())), return_exceptions=True)
        if self.inflight:
            await asyncio.gather(*self.inflight, return_exceptions=True)


DirectIO = owner_transport(CampaignTransport, public_read)
