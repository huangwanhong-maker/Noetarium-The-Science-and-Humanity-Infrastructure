"""Inventory and restore saved waits for a previously issued Koinonia book cohort.

    python applications/koinonia/recover_dispatch.py data/cache/koinonia-dispatch-20261128
    python applications/koinonia/recover_dispatch.py data/cache/koinonia-dispatch-20261128 --probe gpt-writer-003
    python applications/koinonia/recover_dispatch.py data/cache/koinonia-dispatch-20261128 --resume

The run supplies its roster, state and receipts; the repository supplies the client and host. Inventory reads
local state only. A probe performs private HTTP GETs with the specified agents' own passports. Resume
requires both previous hosts to have released their locks and may start model turns when waits resolve.
"""

import argparse
import asyncio
from collections import Counter
import json
from pathlib import Path

from koinonia.dispatch.client import Community, ID
from koinonia.dispatch.host import Cohort
from koinonia.dispatch_waits import dispatcher_locks, item_path, load, mark_attempt, observe, saved_waits


def run_host(run, waits):
    class Recovery(Cohort):
        def __init__(self):
            super().__init__(run)
            self.state = load(run / "recovery-status.json").get("agents", {})
            self.original = load(run / "cohort-status.json").get("agents", {})
            self.launches = asyncio.Semaphore(5)
            self.restorations = {}
            self.ended = {}
            self.turns = {}

        def arm(self, name, thread):
            task = self.restorations.get(name)
            if task is None or task.done():
                self.restorations[name] = asyncio.create_task(self.on_signal(name, thread))

        async def on_signal(self, name, thread):
            try:
                signal = await self.io.wake(name)
                if thread:
                    await self.ended[name].wait()
                async with self.launches:
                    if not mark_attempt(self.io, name, signal):
                        return
                    # A subsequent turn can park again, even before turn/start sends its response.
                    self.restorations.pop(name, None)
                    summary = "Your saved community wait resolved: " + json.dumps(signal, ensure_ascii=False)
                    if thread:
                        self.ended[name].clear()
                        self.mark(name, state="working")
                        await self.request("turn/start", {"threadId": thread, "input": [
                            {"type": "text", "text": summary, "text_elements": []}]})
                    else:
                        self.mark(name, state="reconnecting")
                        await self.start(name, summary + ". Continue under your original passport. Read your briefing "
                                         "and official item or receipts before attempting another act.")
            except Exception as error:
                self.mark(name, state="interrupted", error=f"Reconcile saved signal before retrying: {type(error).__name__}")

        async def handle(self, message):
            method, params = message.get("method"), message.get("params") or {}
            name = self.ids.get(params.get("threadId"))
            if name and method == "item/tool/call" and params.get("tool") == "wait_for_community":
                response = await self.io.call(name, params["tool"], params.get("arguments") or {})
                self.send({"id": message["id"], "result": response})
                if response.get("success") and any(json.loads(c["text"]).get("parked") for c in response["contentItems"]):
                    self.ended.setdefault(name, asyncio.Event())
                    self.mark(name, state="waiting")
                    self.arm(name, params["threadId"])
                return
            # Ignore delayed completed messages for earlier turns following a wake.
            if name and method == "turn/started":
                self.turns[name] = params["turn"]["id"]
                self.ended.setdefault(name, asyncio.Event()).clear()
            if name and method == "turn/completed" and self.turns.get(name, params["turn"]["id"]) != params["turn"]["id"]:
                return
            await super().handle(message)
            if name and method == "turn/completed":
                self.ended.setdefault(name, asyncio.Event()).set()
                if name in self.restorations and not self.restorations[name].done():
                    self.mark(name, state="waiting")

        async def run_saved(self):
            reader = await self.boot()
            for name in waits:
                previous = self.state.get(name) or self.original.get(name, {})
                self.state[name] = {**self.people[name], "state": "waiting", "restored_wait": True,
                                    "previous_thread": previous.get("thread_id"), "report": previous.get("report", "")}
                self.arm(name, None)
            while not reader.done():
                self.snapshot()
                if all(self.state[name]["state"] in {"completed", "failed", "interrupted", "launch-failed"}
                       for name in waits) and all(t.done() for t in self.restorations.values()):
                    break
                await asyncio.sleep(2)
            self.snapshot()
            if reader.done() and any(self.state[n]["state"] == "working" for n in waits):
                raise RuntimeError("Host disconnected during a turn; reconcile admitted acts before resumption")
            await self.io.close()
            self.proc.stdin.close()
            await self.proc.wait()
            await reader

    asyncio.run(Recovery().run_saved())


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("run", type=Path, help="Existing campaign directory containing roster.json")
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--probe", nargs="*", metavar="IDENTITY", help="Inspect saved item waits; omit IDs for all")
    action.add_argument("--resume", action="store_true", help="Exclusively restore waits and start turns on wake")
    args = parser.parse_args(argv)
    run = args.run.resolve(strict=True)
    roster = json.loads((run / "roster.json").read_bytes())
    names = [p["agent"] for p in roster]
    if len(set(names)) != len(names) or any(not isinstance(n, str) or not ID.fullmatch(n) for n in names):
        parser.error("Invalid or duplicate roster identities")
    if args.resume:
        if any(not n.startswith("gpt-") for n in names):
            parser.error("This Codex recovery host only resumes the OpenAI book cohort")
        with dispatcher_locks(run):
            waits = saved_waits(run, names)
            if waits:
                run_host(run, waits)
        return
    waits = saved_waits(run, names)
    print(json.dumps({"restorable": len(waits), "conditions": dict(Counter(
        state["arguments"]["until"] for state in waits.values()))}, sort_keys=True))
    if args.probe is not None:
        selected = args.probe or [n for n, s in waits.items() if s["arguments"]["until"] == "item_change"]
        if any(n not in waits or waits[n]["arguments"]["until"] != "item_change" for n in selected):
            parser.error("Probe requires a roster identity with a saved item wait")
        for name in dict.fromkeys(selected):
            state = dict(waits[name])
            item = state["arguments"]["item"]
            path = item_path(item)
            status, _, value = Community(name, run).request("GET", path, path, None)
            wake = observe(state, status, value)
            print(json.dumps({"agent": name, "item": item, "http": status, "would_wake": wake,
                              "disposition": value.get("status", value.get("state")) if status == 200 else None}))


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, ValueError) as exc:
        raise SystemExit(str(exc))
