"""The OR-Library's benchmark instances (ADR 0141): the multidimensional knapsack and the generalised assignment problem, fetched into
data/cache/or-library/ as published and written as products into data/cache/products/or-library/ with the optima beside them.

- ``mknap1.json``: Petersen's seven problems, each with its optimum as the file gives it.
- ``mknapcb<k>.json`` (k = 1 … 9, as asked): Chu and Beasley's thirty problems of each size, each with the best feasible value they
  found (mkcbres), named "m.n-kk".
- ``gap<k>.json`` (k = 1 … 12, as asked): the five problems of each file, with the optima gapinfo publishes (maximisation).

The OR-Library states no licence; its files stay in the cache, never committed, and are credited "OR-Library (J. E. Beasley)".
The raw files are kept as published, so that the C++ readers (heuresis's benchmarks/orlib.hpp) read them too.

    python data/products/or_library.py [mknap1] [mknapcb1 …] [gap1 …]     (by default mknap1, mknapcb1 and gap1–gap12)
"""

from __future__ import annotations

import argparse
import json
import sys
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RAW = ROOT / "data" / "cache" / "or-library"
OUT = ROOT / "data" / "cache" / "products" / "or-library"
BASE = "https://people.brunel.ac.uk/~mastjjb/jeb/orlib/"
CREDIT = "OR-Library (J. E. Beasley), people.brunel.ac.uk/~mastjjb/jeb/orlib; no licence stated, kept in the cache"
sys.path.insert(0, str(ROOT / "data"))

from formats import orlib  # noqa: E402


def fetch(name: str, path: str) -> Path:
    target = RAW / name
    if target.is_file() and target.stat().st_size > 0:
        return target
    RAW.mkdir(parents=True, exist_ok=True)
    req = urllib.request.Request(BASE + path, headers={"User-Agent": "noetarium-data/1"})
    with urllib.request.urlopen(req, timeout=300) as r:
        data = r.read()
    part = target.with_suffix(target.suffix + ".part")
    part.write_bytes(data)
    part.replace(target)
    record = RAW / "retrieval.json"
    log = json.loads(record.read_text(encoding="utf-8")) if record.is_file() else {"source": BASE, "files": {}}
    log["files"][name] = {"url": BASE + path, "bytes": len(data), "retrieved_at": datetime.now(timezone.utc).isoformat(timespec="seconds")}
    record.write_text(json.dumps(log, indent=2), encoding="utf-8")
    return target


def write(name: str, doc: dict) -> Path:
    OUT.mkdir(parents=True, exist_ok=True)
    p = OUT / name
    p.write_text(json.dumps(doc, separators=(",", ":")), encoding="utf-8")
    return p


def knapsacks(name: str) -> dict:
    text = fetch(f"{name}.txt", f"files/{name}.txt").read_text(encoding="latin-1")
    problems = orlib.read_knapsacks(text)
    best = {}
    if name.startswith("mknapcb"):
        best = orlib.read_best_values(fetch("mkcbres.txt", "files/mkcbres.txt").read_text(encoding="latin-1"))
    for k, p in enumerate(problems):
        if name.startswith("mknapcb"):
            p["name"] = f"{p['m']}.{p['n']}-{k:02d}"
            p["best_known"] = best.get(p["name"])
        else:
            p["name"] = f"{name}-{k + 1}"
    write(f"{name}.json", {"source": name, "kind": "multidimensional knapsack", "credit": CREDIT, "problems": problems})
    return {"file": name, "problems": len(problems)}


def assignments(name: str) -> dict:
    text = fetch(f"{name}.txt", f"files/{name}.txt").read_text(encoding="latin-1")
    problems = orlib.read_assignments(text)
    optima = orlib.read_gap_optima(fetch("gapinfo.html", "gapinfo.html").read_text(encoding="latin-1")).get(name, [])
    for k, p in enumerate(problems):
        p["name"] = f"{name}-{k + 1}"
        p["optimum"] = optima[k] if k < len(optima) else None
        p["sense"] = "maximise"
    write(f"{name}.json", {"source": name, "kind": "generalised assignment", "credit": CREDIT, "problems": problems})
    return {"file": name, "problems": len(problems), "optima": len(optima)}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("files", nargs="*", default=["mknap1", "mknapcb1"] + [f"gap{k}" for k in range(1, 13)])
    args = ap.parse_args()
    done = []
    for f in args.files:
        done.append(knapsacks(f) if f.startswith("mknap") else assignments(f) if f.startswith("gap") else {"file": f, "refused": "unknown file"})
    print(json.dumps(done, indent=1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
