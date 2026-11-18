"""The archive of a project of a studio: every run of a tool kept, never replaced; every file a person brings kept by its digest; the
evidence the runs answered gathered into a ledger (the design of 2026-11-19, `development/ideas/2026-11-19-an-engagement-with-a-city.md`, §4.6).

A project of the studio host (studio_host.py) keeps each step's latest results in `results/<step>.json`, which the next run of the step
replaces. The archive keeps what would be lost:

    archive/runs/<sha256>.json     a run: the step as it was run and everything it answered, under the SHA-256 of its canonical JSON
                                   (keys sorted, no spaces), written once however often the same run recurs
    archive/runs.jsonl             the runs in the order they were made: when, by whom, which step and tool, and the run's digest
    archive/holdings/<sha256>      a file's bytes, under the SHA-256 of its bytes
    archive/holdings.jsonl         the files in the order they were brought: when, by whom, its name, kind and size

The project's journal (studio_host.record, sealed by archeion's rule through archive.sealed) seals each run and each file by its digest, so
that the archive and the journal answer for each other: verify() recomputes every digest and finds each in the journal.

The ledger is the evidence the project's steps currently hold (results of the kind "evidence", board.evidence): each item with the step,
tool and run it came from, and whether its step is stale (what it was run on has changed since).

Nothing here touches the world beyond the project's folder, whose path is given.
"""

from __future__ import annotations

import hashlib
import json
import os
import re
import tempfile
import time
from pathlib import Path

DIGEST = re.compile(r"^[0-9a-f]{64}$")


def canonical(value) -> str:
    """The canonical JSON of a value: keys sorted, no spaces, text as it is."""
    return json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=False)


def digest_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def stamp() -> str:
    now = time.time()
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(now)) + f".{int(now * 1000) % 1000:03d}Z"


def _write_once(path: Path, data: bytes) -> None:
    """Writes a file that is named by its content, once: a file already there is the same file."""
    if path.is_file():
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, tmp = tempfile.mkstemp(dir=path.parent, prefix=".part-")
    try:
        with os.fdopen(fd, "wb") as f:
            f.write(data)
        os.replace(tmp, path)
    finally:
        if os.path.exists(tmp):
            os.unlink(tmp)


def _append(path: Path, entry: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as f:
        f.write(json.dumps(entry, ensure_ascii=False) + "\n")


def _lines(path: Path) -> list[dict]:
    if not path.is_file():
        return []
    return [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines() if line.strip()]


# ---- runs ---------------------------------------------------------------------------------------------------------------------------


def keep_run(folder: Path, step: dict, results: list, by: str = "", when: str | None = None) -> str:
    """Keeps a run of a step (the step as it was run, and what it answered) under its digest; answers the digest."""
    text = canonical({"step": step, "results": results})
    d = digest_text(text)
    _write_once(folder / "archive" / "runs" / f"{d}.json", text.encode("utf-8"))
    _append(folder / "archive" / "runs.jsonl", {"when": when or stamp(), "by": by, "step": step.get("id"), "tool": step.get("tool"),
                                                "title": step.get("title", ""), "digest": d,
                                                "evidence": sum(len(r.get("items", [])) for r in results if isinstance(r, dict) and r.get("kind") == "evidence")})
    return d


def runs(folder: Path, step: str | None = None) -> list[dict]:
    """The runs kept, oldest first; of one step when it is named."""
    out = _lines(folder / "archive" / "runs.jsonl")
    return [r for r in out if step is None or r.get("step") == step]


def run(folder: Path, digest: str) -> dict:
    """A run kept, read and checked against its digest."""
    if not DIGEST.match(digest or ""):
        raise KeyError("a run is named by the 64 hexadecimal digits of its SHA-256")
    path = folder / "archive" / "runs" / f"{digest}.json"
    if not path.is_file():
        raise KeyError("the archive keeps no such run")
    text = path.read_text(encoding="utf-8")
    if digest_text(text) != digest:
        raise ValueError(f"the run kept under {digest[:12]}… has been altered")
    return json.loads(text)


# ---- holdings -----------------------------------------------------------------------------------------------------------------------


def keep_holding(folder: Path, name: str, data: bytes, kind: str = "", by: str = "", when: str | None = None) -> dict:
    """Keeps a file's bytes under their digest; answers what the archive now says of it."""
    clean = re.sub(r"[^\w.\- ]+", "-", Path(name or "file").name).strip("-. ")[:160] or "file"
    d = hashlib.sha256(data).hexdigest()
    _write_once(folder / "archive" / "holdings" / d, data)
    entry = {"when": when or stamp(), "by": by, "name": clean, "kind": str(kind)[:80], "bytes": len(data), "digest": d}
    _append(folder / "archive" / "holdings.jsonl", entry)
    return entry


def holdings(folder: Path) -> list[dict]:
    """The files brought, oldest first (a file brought twice under two names is listed twice and kept once)."""
    return _lines(folder / "archive" / "holdings.jsonl")


def holding(folder: Path, digest: str) -> bytes:
    if not DIGEST.match(digest or ""):
        raise KeyError("a file is named by the 64 hexadecimal digits of its SHA-256")
    path = folder / "archive" / "holdings" / digest
    if not path.is_file():
        raise KeyError("the archive keeps no such file")
    data = path.read_bytes()
    if hashlib.sha256(data).hexdigest() != digest:
        raise ValueError(f"the file kept under {digest[:12]}… has been altered")
    return data


# ---- the ledger ---------------------------------------------------------------------------------------------------------------------


def ledger(folder: Path, project: dict) -> dict:
    """The evidence the project's steps currently hold, each item with where it came from."""
    items, steps = [], []
    for s in project.get("steps", []):
        path = folder / "results" / f"{s.get('id', '')}.json"
        if not re.match(r"^[A-Za-z0-9_-]{1,48}$", str(s.get("id", ""))) or not path.is_file():
            continue
        kept = json.loads(path.read_text(encoding="utf-8"))
        found = 0
        for r in kept.get("results", []):
            if isinstance(r, dict) and r.get("kind") == "evidence":
                for k, it in enumerate(r.get("items", [])):
                    items.append({**it, "from": {"step": s.get("id"), "tool": s.get("tool"), "title": s.get("title", ""), "run": kept.get("step", {}).get("run"),
                                                 "result": r.get("title", ""), "k": k, "stale": bool(s.get("stale"))}})
                    found += 1
        if found:
            steps.append({"step": s.get("id"), "tool": s.get("tool"), "title": s.get("title", ""), "items": found, "stale": bool(s.get("stale")), "ran": s.get("ran")})
    return {"items": items, "steps": steps}


# ---- verification -------------------------------------------------------------------------------------------------------------------


def verify(folder: Path, journal_entries: list[dict] | None = None) -> dict:
    """Every run and file recomputed from its bytes; and, when the project's journal is given, each found sealed in it."""
    faults, sealed = [], {e.get("note") for e in journal_entries or []} | {e.get("about") for e in journal_entries or []}
    listed = runs(folder)
    for r in listed:
        try:
            run(folder, r["digest"])
        except (KeyError, ValueError) as error:
            faults.append(f"the run of {r.get('step')} kept at {r.get('when')}: {error.args[0]}")
            continue
        if journal_entries is not None and r["digest"] not in sealed:
            faults.append(f"the run of {r.get('step')} kept at {r.get('when')} is not sealed in the project's journal")
    files = holdings(folder)
    for h in files:
        try:
            holding(folder, h["digest"])
        except (KeyError, ValueError) as error:
            faults.append(f"the file {h.get('name')}: {error.args[0]}")
            continue
        if journal_entries is not None and h["digest"] not in sealed:
            faults.append(f"the file {h.get('name')} is not sealed in the project's journal")
    return {"holds": not faults, "faults": faults, "runs": len(listed), "files": len(files)}
