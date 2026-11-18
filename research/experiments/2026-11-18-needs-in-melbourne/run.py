"""The search for needs on the City of Melbourne at the 2021 Census (ADR 0125): ten generations with the defaults, and a search with
every strategy of survival, the archive's needs and their confirmations written as tables beside this script.

    python research/experiments/2026-11-18-needs-in-melbourne/run.py

Needs the Urban Studio's needs engine (python applications/urban-studio/engine.py build needs) and the dataset
(python data/products/needs_tables.py melbourne-assistance).
"""

from __future__ import annotations

import csv
import json
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[2] / "api" / "python"))
from science import needs  # noqa: E402

d = needs.data("melbourne-assistance")
t = time.time()
run = needs.ask("evolve", data=d, generations=10)
seconds = time.time() - t
rep = run["report"]
rows = []
for k in rep["archive"]:
    ev = k.get("evidence") or {}
    g, c = ev.get("discovery") or {}, ev.get("confirmation") or {}
    rows.append({"statement": k["text"], "domain": k["domain"], "standing": (ev.get("standing") or {}).get("standing"),
                 "group_value": g.get("group_value"), "reference_value": g.get("reference_value"), "people": g.get("coverage"), "z": g.get("z"),
                 "stability": (ev.get("stability") or {}).get("share"), "validation": (ev.get("validation") or {}).get("verdict"),
                 "confirmed": c.get("confirmed"), "confirmation_estimate": c.get("estimate"), "confirmation_p": c.get("p"), "level": c.get("level"),
                 "causal": (ev.get("causal") or {}).get("verdict"), "simulation": (ev.get("simulation") or {}).get("verdict"), "rules": (ev.get("rules") or {}).get("verdict")})
rows.sort(key=lambda r: (r["standing"] != "robust", -(r["z"] or 0)))
with open(HERE / "kept.csv", "w", newline="", encoding="utf-8") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
    w.writeheader()
    w.writerows(rows)

by_survival = {}
for survival in ["plus", "generational", "aging", "deterministic-crowding", "restricted-tournament", "clearing", "nsga2", "alps", "niches"]:
    r = needs.ask("evolve", data=d, generations=10, options={"strategy": {"survival": survival}})["report"]
    by_survival[survival] = {"kept": len(r["archive"]), "admitted": len(r["admissions"]), "confirmed": sum(1 for c in r["confirmations"] if c["confirmed"]),
                             "judged": r["evaluations"], "domains": sorted({k["domain"] for k in r["archive"]})}

summary = {"seconds": round(seconds, 2), "generations": rep["generation"], "judged": rep["evaluations"], "admitted": len(rep["admissions"]),
           "kept": len(rep["archive"]), "retired": len(rep["retired"]), "confirmed": sum(1 for c in rep["confirmations"] if c["confirmed"]),
           "holdout": rep["holdout"], "online": rep["online"], "journal_head": rep["journal_head"], "by_survival": by_survival}
(HERE / "summary.json").write_text(json.dumps(summary, indent=1), encoding="utf-8")
print(json.dumps(summary, indent=1))
for r in rows[:12]:
    print(f"{r['standing']:10s} {100 * (r['group_value'] or 0):6.2f}% vs {100 * (r['reference_value'] or 0):6.2f}%  n={r['people'] or 0:7.0f}  {r['statement']}")
