"""Study A1: the strength of the repository's checks for three units, measured by mutants (the claim *kills*).

For each unit: every single-site mutant under the declared operators; set aside if it does not load; *distinct* if the
differential tests against the unit itself tell it apart (hidden inputs of both tiers, and the constants); *killed* if
at least one of the repository's own checks for the unit fails on it. No model is called.

    python s1_kills.py            writes results/a1-kills.json
"""
from __future__ import annotations

import json
import sys
from collections import Counter, defaultdict
from pathlib import Path

import harness_js as H
from units import UNITS, hidden_inputs, source

OUT = Path(__file__).resolve().parent / "results"


def study(unit) -> dict:
    base = H.sandbox(unit)
    text = source(unit)
    cases = hidden_inputs(unit["name"])

    # The harness is first held to the accepted implementation itself.
    own = H.checks(base, unit, None)
    assert own["passed"] == own["total"] == unit["checks"] and own["crashed"] is None, own
    same = H.differential(base, unit, None, cases)
    assert same["load"] is None and all(c["ok"] for c in same["cases"]) and not any(same["constants"].values()), "the unit differs from itself"

    rows = []
    for mutant in H.mutants(text):
        body = H.apply(text, mutant)
        diff = H.differential(base, unit, body, cases)
        row = {k: mutant[k] for k in ("operator", "line", "old", "new")}
        row["owner"] = H.owner(text, mutant["line"])
        if diff["load"] is not None:
            row["state"] = "does not load"
            rows.append(row)
            continue
        failing = [c for c in diff["cases"] if not c["ok"]]
        by_id = {c["id"]: c for c in cases}
        distinct = bool(failing) or any(diff["constants"].values())
        row["distinct"] = distinct
        row["tiers"] = sorted({by_id[c["id"]]["tier"] for c in failing})
        if not distinct:
            row["state"] = "not told apart"
            rows.append(row)
            continue
        verdict = H.checks(base, unit, body)
        row["killed"] = verdict["passed"] < verdict["total"] or verdict["crashed"] is not None
        row["checks_failed"] = verdict["failed"]
        row["state"] = "killed" if row["killed"] else "survived"
        rows.append(row)

    distinct = [r for r in rows if r.get("distinct")]
    killed = [r for r in distinct if r["killed"]]
    by_operator, by_owner = defaultdict(Counter), defaultdict(Counter)
    for r in distinct:
        by_operator[r["operator"]]["killed" if r["killed"] else "survived"] += 1
        by_owner[r["owner"]]["killed" if r["killed"] else "survived"] += 1
    return {
        "unit": unit["name"], "module": unit["module"], "checks": unit["checks"], "check_names": own["names"],
        "hidden_inputs": {"main": sum(1 for c in cases if c["tier"] == "main"), "edge": sum(1 for c in cases if c["tier"] == "edge")},
        "mutants": len(rows), "do_not_load": sum(1 for r in rows if r["state"] == "does not load"),
        "not_told_apart": sum(1 for r in rows if r["state"] == "not told apart"),
        "distinct": len(distinct), "killed": len(killed), "share_killed": round(len(killed) / len(distinct), 4) if distinct else None,
        "distinct_only_at_the_edge": sum(1 for r in distinct if r["tiers"] == ["edge"]),
        "by_operator": {k: dict(v) for k, v in sorted(by_operator.items())},
        "by_owner": {k: dict(v) for k, v in sorted(by_owner.items())},
        "survivors": [{k: r[k] for k in ("operator", "line", "old", "new", "owner")} for r in distinct if not r["killed"]],
        "rows": rows,
    }


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    OUT.mkdir(exist_ok=True)
    results = {}
    for name in sys.argv[1:] or list(UNITS):
        result = study(UNITS[name])
        results[name] = result
        print(f"{name}: {result['mutants']} mutants, {result['do_not_load']} do not load, {result['not_told_apart']} not told apart, "
              f"{result['distinct']} distinct, {result['killed']} killed by the repository's {result['checks']} checks "
              f"({result['share_killed']:.1%})")
        for operator, counts in result["by_operator"].items():
            print(f"    {operator}: {counts.get('killed', 0)} of {counts.get('killed', 0) + counts.get('survived', 0)} killed")
        for owner, counts in result["by_owner"].items():
            print(f"    {owner}: {counts.get('killed', 0)} of {counts.get('killed', 0) + counts.get('survived', 0)} killed")
    (OUT / "a1-kills.json").write_text(json.dumps(results, indent=1), encoding="utf-8")
