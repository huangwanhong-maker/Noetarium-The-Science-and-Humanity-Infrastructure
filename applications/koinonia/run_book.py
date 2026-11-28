"""Run the case of a book written by communities (the design record of 2026-11-26) without its page, and keep what it did.

    python applications/koinonia/run_book.py --dry --rounds 60                 # every agent by its declared procedure: nothing is spent
    python applications/koinonia/run_book.py --models --rounds 400 --cap 30    # Claude, within the kept budget and this run's cap
    python applications/koinonia/run_book.py --models --resume --rounds 100    # carry on a society kept under the same name

The society is kept under ``data/cache/koinonia/societies/<name>/`` (default ``book-models`` or ``book-dry``) and saved every round, so
that a run stopped for any reason is carried on with ``--resume``. Each round's tallies are printed as a line of JSON and kept in
``rounds.jsonl`` beside the society. With ``--sections k`` only the first k sections of the body are worked until the run is resumed
without it (a pilot, to measure what a section costs). The run stops when the council has adopted the book, when this run has spent its
cap, or after eight rounds in which no one acted.
"""

from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

from koinonia import agents, bookcase, cases  # noqa: E402
from koinonia.budget import Ledger  # noqa: E402
from koinonia.society import Society  # noqa: E402
from science import communities  # noqa: E402
from science.models import Stub, kept_key  # noqa: E402

STORE = ROOT / "data" / "cache" / "koinonia" / "societies"


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--models", action="store_true", help="ask Claude (spends within the kept budget)")
    ap.add_argument("--dry", action="store_true", help="every agent by its declared procedure")
    ap.add_argument("--rounds", type=int, default=60)
    ap.add_argument("--name", default="")
    ap.add_argument("--resume", action="store_true")
    ap.add_argument("--cap", type=float, default=30.0, help="the most this run may spend, in dollars")
    ap.add_argument("--sections", type=int, default=0, help="a pilot: work only the first k sections of the body")
    ap.add_argument("--parallel", type=int, default=8)
    args = ap.parse_args(argv)
    communities.ENGINE.keep(1)
    from science import progress
    progress.ENGINE.keep(6)  # the bench's engine kept alive: analysts run their plans at once, each a request a line
    name = args.name or ("book-models" if args.models else "book-dry")
    folder = STORE / name
    if args.resume and (folder / "society.json").is_file():
        s = Society.load(folder)
    else:
        if folder.exists():
            import shutil
            shutil.rmtree(folder)
        s = Society(name, folder)
        bookcase.setup(s, dry=args.dry or not args.models)
    s.parallel = args.parallel
    s.book.call["pilot sections"] = args.sections
    if args.models:
        ledger = Ledger.kept()
        s.asker = agents.Asker(ledger, agents.anthropic_maker(kept_key("koinonia") or kept_key("anthropic")))
    else:
        ledger = Ledger(None)
        s.asker = agents.Asker(ledger, lambda agent, n: Stub(cases.stand_in))
    start = ledger.summary()["spent_usd"]
    idle = 0
    with (folder / "rounds.jsonl").open("a", encoding="utf-8") as log:
        for _ in range(args.rounds):
            t0 = time.time()
            r = s.round()
            m = s.book.measures.get(str(s.turn), {})
            row = {"turn": s.turn, "acts": r["acts"], "woke": len(r["woke"]), "seconds": round(time.time() - t0, 1),
                   "usd run": round(ledger.summary()["spent_usd"] - start, 4), "usd all": ledger.summary()["spent_usd"],
                   **{k: m.get(k) for k in ("sections", "data", "analysis", "draft", "review", "revision", "adopted", "versions", "reviews", "results",
                                            "words adopted", "petitions", "tools admitted", "wrong found by audit", "regime")}}
            log.write(json.dumps(row) + "\n")
            log.flush()
            print(json.dumps(row), flush=True)
            idle = idle + 1 if r["acts"] == 0 else 0
            if s.book.adopted_book:
                print("the council adopted the book", flush=True)
                break
            if ledger.summary()["spent_usd"] - start >= args.cap:
                print(f"this run's cap of ${args.cap} is reached", flush=True)
                break
            if idle >= 8:
                print("eight rounds in which no one acted: the society is stuck", flush=True)
                break
    print(json.dumps({"rounds": s.turn, "book": s.book.counts(), "usd this run": round(ledger.summary()["spent_usd"] - start, 4),
                      "journal": {"entries": s.behaviour.n, "head": s.behaviour.head}}, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
