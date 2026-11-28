"""What the case of a book written by communities did, read from its record (the design record of 2026-11-26, Section 6).

    python applications/koinonia/summarise_book.py <society folder> [--json out.json]

Every number the case's README and account quote is read here from the society (``society.json``) and the behaviour journal
(``behaviour.jsonl``), never copied by hand: the conference and the charter, the outline and its amendments, the pages allotted, the
regimes and rules over time, the sections and their versions, the reviews and audits, petitions and tools, what each kind of agent cost,
and the interventions of the case's maker and of the user.
"""

from __future__ import annotations

import collections
import json
import sys
from pathlib import Path


def summarise(folder: Path) -> dict:
    s = json.loads((folder / "society.json").read_text(encoding="utf-8"))
    b = s["book"]
    journal = [json.loads(x) for x in (folder / "behaviour.jsonl").read_text(encoding="utf-8").splitlines() if x.strip()]
    lg = next((c for c in s["communities"] if c["name"] == "partnership"), None)
    conf = next((c for c in s["communities"] if c["name"] == "conference"), None)

    charters = [{"id": a["fields"]["id"], "by": a["author"], "turn": a["turn"], "decision": a["fields"].get("decision"),
                 "charter": {k[7:]: v for k, v in a["fields"].items() if k.startswith("change.")}} for a in (conf or {}).get("record", []) if a["kind"] == "propose"]
    changes = [{"turn": a["turn"], "by": a["author"], "id": a["fields"]["id"], "compact": a["fields"].get("change.compact"), "rule": a["fields"].get("change.rule")}
               for a in (lg or {}).get("record", []) if a["kind"] == "propose" and (a["fields"].get("change.compact") or a["fields"].get("change.rule"))]
    enacted = {a["fields"]["id"]: a["turn"] for a in (lg or {}).get("record", []) if a["kind"] == "amend"}
    for c in changes:
        c["enacted"] = enacted.get(c["id"])

    secs = b["sections"]
    versions = collections.Counter(sum(1 for v in b["versions"].values() if v["section"] == sid) for sid in secs)
    words = sum(len(b["versions"][x["adopted"]]["text"].split()) for x in secs.values() if x.get("adopted") in b["versions"])
    words += sum(len(str(p.get("text", "")).split()) for p in b.get("proposals", {}).values())
    reviews = list(b["reviews"].values())
    by_model = collections.Counter((r.get("model"), r.get("recommend")) for r in reviews)
    by_regime = collections.Counter((r.get("regime"), r.get("recommend")) for r in reviews)
    faults = collections.Counter((f.get("severity"), f.get("statement")) for r in reviews for f in (r.get("faults") or []))

    cost = collections.defaultdict(lambda: {"calls": 0, "usd": 0.0, "input": 0, "output": 0})
    by_what = collections.defaultdict(float)
    for e in journal:
        if e["what"] != "ask":
            continue
        role = next((a["role"] for a in s["agents"].values() if a["id"] == e["who"]), "?")
        d, t = e["data"], e["data"].get("tokens") or {}
        cost[role]["calls"] += 1
        cost[role]["usd"] += d.get("usd") or 0
        cost[role]["input"] += t.get("input_tokens", 0)
        cost[role]["output"] += t.get("output_tokens", 0)
        what = e["about"].split(" ")[0] if e["about"] else "?"
        by_what[what] += d.get("usd") or 0
    for v in cost.values():
        v["usd"] = round(v["usd"], 3)

    pets = list(b["petitions"].values())
    return {
        "rounds": s["turn"], "agents": len(s["agents"]), "journal entries": s["journal"]["entries"],
        "conference": {"charters proposed": charters, "adopted": b["call"].get("charter adopted"), "charter": b["call"].get("charter"),
                       "decided by": b["call"].get("charter decided by"), "ratifications": b["call"].get("ratifications"),
                       "founded at": b["call"].get("founded at"), "members": b["call"].get("members")},
        "outline": {"title": b["outline"].get("title"), "chapters": len(b["outline"].get("chapters", [])), "sections": len(secs),
                    "amendments": {k: {"by": v.get("community"), "change": v.get("change"), "status": v.get("status")} for k, v in b["amendments"].items()},
                    "allotment": b.get("allotment"), "adopted at": b["outline"].get("adopted at"),
                    "chapters by kind": dict(collections.Counter(c["kind"] for c in b["outline"].get("chapters", [])))},
        "changes of rules": changes,
        "sections": {"by stage": dict(collections.Counter("withdrawn" if x.get("withdrawn") else x["stage"] for x in secs.values())),
                     "versions per section": dict(sorted(versions.items())), "corrected": sum(1 for x in secs.values() if x.get("corrected")),
                     "with errata": sum(1 for x in secs.values() if x.get("errata")), "words adopted": words,
                     "versions": len(b["versions"]), "results kept": len(b["results"])},
        "reviews": {"count": len(reviews), "by model and recommendation": {f"{k[0]} {k[1]}": v for k, v in by_model.items()},
                    "by regime and recommendation": {f"{k[0]} {k[1]}": v for k, v in by_regime.items()},
                    "faults by severity and statement": {f"{k[0]} {k[1]}": v for k, v in faults.most_common()}},
        "audits": {"count": len(b["audits"]), "numbers checked": sum(a["checked"] for a in b["audits"]), "wrong": sum(len(a["wrong"]) for a in b["audits"]),
                   "reopened": sum(1 for x in secs.values() if x.get("reopened"))},
        "petitions": {"count": len(pets), "by kind and status": {f"{k[0]} {k[1]}": v for k, v in collections.Counter((p.get("kind", "tool"), p["status"]) for p in pets).items()},
                      "by community": dict(collections.Counter(p["community"] for p in pets))},
        "tools admitted": {k: {"about": v.get("about"), "by": v.get("by"), "checked by": v.get("checked by"), "admitted": v.get("admitted")} for k, v in b["tools"].items()},
        "cost by role": dict(cost), "cost by call": {k: round(v, 3) for k, v in sorted(by_what.items(), key=lambda kv: -kv[1])},
        "usd": round(sum(v["usd"] for v in cost.values()), 3),
        "interventions": b["call"].get("interventions", []),
        "book adopted": b.get("adopted_book") or None,
    }


def main(argv: list[str]) -> int:
    folder = Path(argv[0])
    out = summarise(folder)
    if "--json" in argv:
        Path(argv[argv.index("--json") + 1]).write_text(json.dumps(out, ensure_ascii=False, indent=1), encoding="utf-8")
    print(json.dumps({k: out[k] for k in ("rounds", "sections", "audits", "usd", "book adopted")}, ensure_ascii=False, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
