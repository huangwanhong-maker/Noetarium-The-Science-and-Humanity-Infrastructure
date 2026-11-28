"""Portable mock tests for the book dispatcher's saved waits. No community or provider is contacted."""

import asyncio
from collections import defaultdict
from contextlib import suppress
import json
from pathlib import Path
import subprocess
import sys
import tempfile
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from koinonia.dispatch_waits import dispatcher_locks, digest, item_path, mark_attempt, observe, owner_transport, saved_waits
from koinonia.dispatch.host import Cohort
from koinonia.dispatch.transport import DirectIO
from recover_dispatch import run_host


def check(name, value):
    print(json.dumps({"kind": "assert", "name": "dispatch: " + name, "ok": bool(value)}))
    if not value:
        raise AssertionError(name)


def wait(item, **fields):
    return {"arguments": {"until": "item_change", "item": item}, "since": "previous run", "pending": True, **fields}


class Transport:
    def __init__(self, data):
        self.data = data
        self.logs = []
        self.wait_locks = defaultdict(asyncio.Lock)
        self.pending = {}
        self.closed = False
        self.poll_seconds = 0.012
        self.states = {}
        self.routes = []

    async def bounded(self, function, *args):
        return function(*args)

    def client(self, identity):
        transport = self

        class Identity:
            @staticmethod
            def request(method, path, route, body):
                transport.routes.append((identity, method, path, route, body))
                status, value = transport.data[identity]
                return status, b"", value
        return Identity()

    def load_wait(self, name):
        return dict(self.states.get(name, {}))

    def save_wait(self, name, state):
        self.states[name] = dict(state)

    async def wait(self, name, args):
        return "normal board rules"

    async def close(self):
        self.closed = True


async def until(test):
    async def check_again():
        while not test():
            await asyncio.sleep(0.003)
    await asyncio.wait_for(check_again(), 1)


async def check_transport():
    a, b = "r-a", "r-b"
    heads = {"root": "A"}
    head_requests = []

    def head(path, logs):
        head_requests.append(path)
        return {"head": dict(heads)}

    data = {a: (200, {"id": "question 1", "status": "open", "reader": a}),
            b: (200, {"id": "question 1", "status": "open", "reader": b})}
    io = owner_transport(Transport, head)(data)
    io.states[a] = wait("question 1", baseline=digest(None))
    w1 = asyncio.create_task(io.wait(a, io.states[a]["arguments"]))
    w2 = asyncio.create_task(io.wait(b, {"until": "item_change", "item": "question 1"}))
    await until(lambda: all(io.states.get(n, {}).get("source") == "own-item" for n in (a,b)))
    check("private item requests use the waiting identity and its encoded item URL", io.routes == [
        (n, "GET", "/c/completion/items/question%201", "/c/completion/items/question%201", None) for n in (a,b)])
    check("migrating an absent public baseline establishes the owner's open item without waking the model",
          io.states[a]["pending"] and io.states[a]["baseline"] == digest(data[a][1]) and not w1.done())
    await asyncio.sleep(0.05)
    check("unchanged head coalesces owner polling while public head checks continue", len(io.routes) == 2 and len(head_requests) > 1)
    data[a] = 503, {}
    heads["root"] = "B"
    await until(lambda: len(io.routes) >= 4)
    check("an interrupted read preserves the owner's valid baseline", io.states[a]["baseline"] == digest(
        {"id": "question 1", "status": "open", "reader": a}) and not w1.done())
    data[a] = 200, {"id": "question 1", "status": "ruled", "reader": a}
    signal = await asyncio.wait_for(w1, 1)
    check("one owner's ruling wakes only that owner, even for the same textual ID", signal["state"] == "ruled" and not w2.done())
    check("a recovered signal is submitted at most once", mark_attempt(io, a, signal) and not mark_attempt(io, a, signal))
    check("existing eligibility/work waits keep using their previous routing", await io.wait(a, {"until":"eligible"}) == "normal board rules")
    await io.close()
    with suppress(RuntimeError):
        await w2
    check("shutdown preserves the other agent's durable wait", io.states[b]["pending"])


def check_state():
    state = wait("a1", baseline=digest(None))
    check("a previously decided application survives the missed legacy transition", observe(state,200,{"id":"a1","status":"declined"}))
    state = wait("q1")
    check("missing initial item stays pending", not observe(state,404,{}))
    check("first appearance wakes a previously missing item", observe(state,200,{"id":"q1","status":"open"}))
    state = wait("q1")
    check("an inaccessible item reports a refusal without reading as another agent", observe(state,403,{}) and state["details"]["http"] == 403)
    try:
        item_path("../other")
    except ValueError:
        check("item IDs cannot escape their owner-scoped route", True)
    else:
        raise AssertionError("unsafe item ID")
    with tempfile.TemporaryDirectory() as tmp:
        root = Path(tmp)
        (root / "wait-state").mkdir()
        (root / "io-log").mkdir()
        states = {"ended": wait("q"), "left": wait("u"), "ack": {**wait("a"),"pending":False,"recoveryArmed":True,
                  "woke":"old","resumeAttempt":"old"}, "unsent": {**wait("u"),"pending":False,"recoveryArmed":True,"woke":"new"},
                  "legacy_signalled": {**wait("g"),"pending":False,"woke":"ambiguous"}}
        for name, value in states.items():
            (root / "wait-state" / (name+".json")).write_text(json.dumps(value), encoding="utf-8")
        (root / "io-log/left.jsonl").write_text('{"kind":"leave","admitted":true}\n',encoding="utf-8")
        check("recovery restores ended-turn waits and unsent signals while preserving departures and prior attempts",
              set(saved_waits(root, states)) == {"ended", "unsent"})


def check_host():
    with tempfile.TemporaryDirectory() as tmp:
        root = Path(tmp)
        name = "gpt-writer-003"
        person = {"agent":name, "role":"writer", "model":"gpt-6.1-terra", "effort":"low"}
        state = wait("q1", baseline=digest(None))
        (root / "roster.json").write_text(json.dumps([person]), encoding="utf-8")
        (root / "cohort-status.json").write_text(json.dumps({"agents":{name:{"state":"completed","thread_id":"old"}}}), encoding="utf-8")
        (root / "wait-state").mkdir()
        (root / "wait-state" / (name+".json")).write_text(json.dumps(state), encoding="utf-8")
        launched = []

        async def boot(host):
            stopped = asyncio.Event()
            host.proc = SimpleNamespace(stdin=SimpleNamespace(close=stopped.set), wait=stopped.wait)
            return asyncio.create_task(stopped.wait())

        async def node_head(io):
            return "a"

        async def private_read(io, identity, target):
            if (identity,target) != (name,"q1"):
                raise ValueError("wrong identity or route")
            return 200, {"id":"q1","status":"ruled"}

        async def start(host, identity, notice):
            launched.append((identity, host.people[identity]["model"], "ruled" in notice))
            host.mark(identity, state="completed")

        with patch.object(Cohort,"boot",boot), patch.object(Cohort,"start",start), \
             patch.object(DirectIO,"read_head",node_head), patch.object(DirectIO,"read_item",private_read):
            run_host(root, saved_waits(root,[name]))
        report = json.loads((root/"recovery-status.json").read_bytes())["agents"][name]
        check("the repository host reconnects the ended original identity only after its private ruling",
              launched == [(name, "gpt-6.1-terra", True)] and report["state"] == "completed" and report["previous_thread"] == "old")
        check("restarting the host does not resubmit an acknowledged turn", not saved_waits(root,[name]))


def check_exclusive():
    with tempfile.TemporaryDirectory() as tmp:
        command = [sys.executable, "-B", "-c",
                   "from pathlib import Path; import sys; from koinonia.dispatch_waits import dispatcher_locks\n"
                   "with dispatcher_locks(Path(sys.argv[1])): pass", tmp]
        app = Path(__file__).resolve().parents[1]
        with dispatcher_locks(Path(tmp)):
            occupied = subprocess.run(command, cwd=app, capture_output=True, text=True, timeout=10)
        free = subprocess.run(command, cwd=app, capture_output=True, text=True, timeout=10)
        check("another process cannot take over until the previous dispatcher releases both slots",
              occupied.returncode != 0 and "still owns its lock" in occupied.stderr and free.returncode == 0)


if __name__ == "__main__":
    check_state()
    asyncio.run(check_transport())
    check_host()
    check_exclusive()
