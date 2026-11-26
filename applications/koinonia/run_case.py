"""Run one of Koinonia's case studies without its page (ADR 0169), and write what it did.

    python applications/koinonia/run_case.py two --rounds 16                 # a model's stand-in: nothing is spent
    python applications/koinonia/run_case.py two --rounds 16 --models        # Claude, within the kept budget

With ``--models`` the agents ask Claude with the key kept as ``koinonia`` (else ``anthropic``) in the person's models.json, each call
reserved against the ledger in the settings folder (``koinonia-budget.json``) and refused past its stop. The society is kept under
``data/cache/koinonia/societies/<name>/`` and a summary is printed as JSON.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

from koinonia import agents, cases  # noqa: E402
from koinonia.budget import Ledger  # noqa: E402
from koinonia.society import Society  # noqa: E402
from science import communities  # noqa: E402
from science.models import Stub, kept_key  # noqa: E402

STORE = ROOT / "data" / "cache" / "koinonia" / "societies"


def summary(s: Society, ledger: Ledger) -> dict:
    from koinonia import circulation
    left = {c: sum(1 for b in s.work.batches if b.get("owner", "commons") == c and not b.get("settled")) for c in {b.get("owner", "commons") for b in s.work.batches}}
    return {"society": s.name, "rounds": s.turn, "work": s.work.counts(), "batches left": left, "budget": ledger.summary(),
            "circulation": circulation.account(s) if circulation.league(s) else None,
            "communities": {c.name: {"acts": len(c.record), "refused": len(c.refused), "head": c.result.get("head", {})} for c in s.communities.values()},
            "agents": {a["id"]: {k: a["tally"][k] for k in ("offered", "accepted", "released", "refused", "agreed", "disputed", "calls", "usd")} for a in s.agents.values()},
            "notes": [e["text"] for e in s.events if e["kind"] == "note"][-12:]}


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("case", choices=sorted(cases.CASES))
    ap.add_argument("--rounds", type=int, default=16)
    ap.add_argument("--models", action="store_true", help="ask Claude (spends within the kept budget)")
    ap.add_argument("--name", default="")
    ap.add_argument("--size", type=int, default=48)
    ap.add_argument("--record", default="", help="a folder for the trajectory (rounds.jsonl, society.json, summary.json)")
    ap.add_argument("--regime", default="gift", choices=["gift", "credit", "transferable credit", "cleared credit", "provision"], help="three: the league's arrangement at first")
    ap.add_argument("--envoys", action="store_true", help="three: envoys by model in the league's council")
    ap.add_argument("--models-for", default="all", choices=["all", "envoys"], help="with --models: every model agent, or the envoys alone")
    args = ap.parse_args(argv)
    communities.ENGINE.keep(1)
    name = args.name or f"case-{args.case}" + ("-models" if args.models else "-stand-in")
    s = Society(name, STORE / name)
    if args.models:
        ledger = Ledger.kept()
        claude = agents.anthropic_maker(kept_key("koinonia") or kept_key("anthropic"))
        if args.models_for == "envoys":  # only the envoys ask Claude; the checks are the stand-in's, and cost nothing
            s.asker = agents.Asker(ledger, lambda agent, n: claude(agent, n) if agent["role"] == "envoy" else Stub(cases.stand_in))
        else:
            s.asker = agents.Asker(ledger, claude)
    else:
        ledger = Ledger(None)
        s.asker = agents.Asker(ledger, lambda agent, max_tokens: Stub(cases.stand_in))
    options = {"regime": args.regime, "envoys": args.envoys} if args.case == "three" else {}
    cases.CASES[args.case](s, size=args.size, **options)
    record = Path(args.record) if args.record else None
    trajectory, start = [], ledger.summary()["spent_usd"]
    for _ in range(args.rounds):
        r = s.round()
        row = {"turn": r["turn"], "woke": r["woke"], "acts": r["acts"], "work": s.work.counts(), "usd": round(ledger.summary()["spent_usd"] - start, 4),
               "communities": {c.name: {"acts": len(c.record), "refused": len(c.refused)} for c in s.communities.values()},
               "agents": {a["id"]: dict(a["tally"]) for a in s.agents.values()}}
        trajectory.append(row)
        print(json.dumps({k: row[k] for k in ("turn", "acts", "work", "usd")}), flush=True)
        if s.work.counts()["unchecked"] == 0 and all(d["status"] == "carried" for d in s.work.disputes):
            break
        if len(trajectory) >= 8 and all(t["acts"] == 0 for t in trajectory[-8:]):  # eight rounds in which no one acted: the society is stuck
            break
    out = summary(s, ledger)
    out["usd this run"] = round(ledger.summary()["spent_usd"] - start, 4)
    print(json.dumps(out, indent=1, ensure_ascii=False))
    if record is not None:  # the whole trajectory kept beside the record: each round, the society as it ended, the summary
        record.mkdir(parents=True, exist_ok=True)
        (record / "rounds.jsonl").write_text("".join(json.dumps(t, ensure_ascii=False) + "\n" for t in trajectory), encoding="utf-8")
        (record / "society.json").write_text(json.dumps(s.to_json(), ensure_ascii=False, indent=1), encoding="utf-8")
        (record / "summary.json").write_text(json.dumps(out, ensure_ascii=False, indent=1), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
