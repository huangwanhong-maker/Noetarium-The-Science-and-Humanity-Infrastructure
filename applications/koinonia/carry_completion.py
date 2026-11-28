"""Carries the first attempt to complete the book into the completion re-founded at Koinonia's node (the design of 2026-11-27, Section
11; ADR 0176), then retires the first attempt's server.

The first attempt's folder (``docs/reports/2026-11-26-koinonia-book/completion/``) is read and never altered by the carrying:

- it is packed as a BagIt bag made and validated by archeion (``science.archive``), deposited within the holding ``first attempt``;
- its side journal is deposited there as a holding of kind record, its chain verified first, and, being a record of conduct, each of
  its entries is recorded as an episode of the completion's series (grade A where it carries a digest, else B: attested by the sealed
  journal), naming the entry and its seal;
- the decision ``accept carried work`` is taken as the author made it on 2026-11-27 (the design's Section 15): a carried section is
  accepted when its current version holds the section standard and bears a standing verdict of adopt or minor by an agent of another
  provider than its writer's;
- each carried section is deposited, accepted ones within ``adopted`` and the others within ``first attempt``, naming its author
  (agent, model, provider, run by), its version and its review; each standing review within ``reviews``; each section's results and
  the replay's totals within ``facts``;
- the remaining work is posted: the five sections never written, each carried section not accepted (its specification the carried
  version), and the overview, waiting until every section is accepted.

The retirement then touches the folder twice: one final entry appended to the side journal by its own sealing rule, naming the bag's
digest and the community's head, and ``serve.py`` answering every route with 410 Gone and the new address. No other entry changes.

The person who ran the first attempt is named only as "the author" in everything carried; the bag keeps the first attempt's files as
they were.

    python applications/koinonia/carry_completion.py [--first-attempt DIR] [--store DIR] [--url URL] [--date YYYY-MM-DD]
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path[:0] = [str(APP), str(ROOT / "api" / "python"), str(ROOT / "data" / "products")]

from koinonia import book, completion  # noqa: E402
from koinonia.behaviour import episode  # noqa: E402

FIRST_ATTEMPT = ROOT / "docs" / "reports" / "2026-11-26-koinonia-book" / "completion"
SOCIETY = ROOT / "data" / "cache" / "koinonia" / "societies" / "book-models" / "society.json"
STORE = ROOT / "data" / "cache" / "koinonia" / "node"
SKIP = {"__pycache__", "journal.lock"}
RETIRED = "# ---- Retired (ADR 0176) ----"
JOURNAL_HOLDING, BAG_HOLDING = "the first attempt's journal", "the first attempt as a bag"
DECISION = "accept carried work"


def sha256_file(p: Path) -> str:
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def load(p: Path, default=None):
    return json.loads(Path(p).read_text(encoding="utf-8")) if Path(p).is_file() else default


# ---- The first attempt read -------------------------------------------------------------------------------------------------------

def seal_of(entry: dict) -> str:
    """The side journal's own sealing rule (complete.py): SHA-256 over the entry's canonical JSON without its seal."""
    e = {k: v for k, v in entry.items() if k != "seal"}
    return hashlib.sha256(json.dumps(e, ensure_ascii=False, sort_keys=True, separators=(",", ":")).encode("utf-8")).hexdigest()


def verify_journal(path: Path) -> dict:
    prev, entries = "0" * 64, []
    for k, line in enumerate(Path(path).read_text(encoding="utf-8").splitlines(), 1):
        e = json.loads(line)
        if e.get("previous") != prev or seal_of(e) != e.get("seal") or e.get("n") != k:
            return {"holds": False, "first fault": k, "entries": entries}
        prev = e["seal"]
        entries.append(e)
    return {"holds": True, "count": len(entries), "head": prev, "entries": entries}


def names_of(folder: Path) -> set[str]:
    """The persons named as running the first attempt's agents, to be named "the author" in what is carried."""
    out = {str(p.get("run by")) for p in (load(folder / "participants.json", {}) or {}).values() if p.get("run by")}
    for d in ("sections", "reviews"):
        for f in (folder / d).glob("*.json"):
            r = (load(f, {}) or {}).get("by") or {}
            if isinstance(r, dict) and r.get("run by"):
                out.add(str(r["run by"]))
    return {n for n in out if n and n != "the author"}


def scrub(value, names: set[str]):
    text = json.dumps(value, ensure_ascii=False)
    for n in sorted(names, key=len, reverse=True):
        text = text.replace(n, "the author")
    return json.loads(text)


def author_of(by, names: set[str]) -> dict:
    """{agent, model, provider, run by} from a journal's "by" (an invitation's record, or "agent / model / provider / run by …")."""
    if isinstance(by, dict):
        out = {k: str(by.get(k, "")) for k in ("agent", "model", "provider", "run by")}
    elif isinstance(by, str):
        parts = [p.strip() for p in by.split(" / ")]
        run = parts[3][len("run by "):] if len(parts) >= 4 and parts[3].startswith("run by ") else ""
        out = {"agent": parts[0], "model": parts[1] if len(parts) > 1 else "", "provider": parts[2] if len(parts) > 2 else "", "run by": run}
    else:
        out = {"agent": "", "model": "", "provider": "", "run by": ""}
    return scrub(out, names)


def read_first_attempt(folder: Path) -> dict:
    """Everything the carrying needs, read from the folder: the plan, each checked section with its file's digest and its standing
    review, the results, the replay, the journal verified."""
    folder = Path(folder)
    names = names_of(folder)
    plan = load(folder / "plan.json", {})
    sections = {}
    for f in sorted((folder / "sections").glob("*.json")):
        sid, body = f.stem, load(f)
        audit = load(folder / "audit" / f"{sid}.json", {}) or {}
        d = book.digest(body)
        sections[sid] = {"file": f"sections/{sid}.json", "sha256": sha256_file(f), "version": audit.get("version", ""), "version digest": d,
                         "holds": bool(audit.get("ok")) and audit.get("digest") == d, "by": scrub(body.get("by") or {}, names)}
    reviews = {}
    for f in sorted((folder / "reviews").glob("*.json")):
        sid, rv = f.stem, load(f)
        hist = sorted((folder / "reviews" / sid).glob("*.json")) if (folder / "reviews" / sid).is_dir() else []
        s = sections.get(sid, {})
        reviews[sid] = {"file": f"reviews/{sid}.json", "sha256": sha256_file(f), "verdict": rv.get("verdict", ""), "by": scrub(rv.get("by") or {}, names),
                        "of version": rv.get("of version", ""), "current": bool(s) and rv.get("of version") == s.get("version digest"),
                        "faults": len(rv.get("faults") or []), "reviews kept": len(hist), "as": hist[-1].stem if hist else "standing"}
    results = {f.stem: sha256_file(f) for f in sorted((folder / "results").glob("*.json"))}
    j = verify_journal(folder / "journal.jsonl")
    return {"folder": folder, "names": names, "plan": plan, "sections": sections, "reviews": reviews, "results": results,
            "replay": sha256_file(folder / "replay.json") if (folder / "replay.json").is_file() else "",
            "journal": j, "journal sha256": sha256_file(folder / "journal.jsonl")}


def decide(fa: dict) -> dict:
    """The author's decision on carried work (Section 15), applied: accepted, to revise (a standing verdict of revise on the current
    version), or to review (no standing review of the current version by another provider)."""
    out = {"accepted": [], "revise": [], "review": []}
    for sid, s in sorted(fa["sections"].items()):
        r = fa["reviews"].get(sid)
        other = bool(r) and r["by"].get("provider", "") not in ("", s["by"].get("provider", ""))
        if s["holds"] and r and r["current"] and other and r["verdict"] in ("adopt", "minor"):
            out["accepted"].append(sid)
        elif s["holds"] and r and r["current"] and other and r["verdict"] == "revise":
            out["revise"].append(sid)
        else:
            out["review"].append(sid)
    return out


# ---- The bag ----------------------------------------------------------------------------------------------------------------------

def files_of(folder: Path) -> list[Path]:
    return sorted(p for p in Path(folder).rglob("*") if p.is_file() and not (SKIP & set(p.relative_to(folder).parts)) and p.suffix != ".pyc")


def make_bag(folder: Path, out: Path, date: str) -> dict:
    """The first attempt's folder as a BagIt bag made and validated by archeion: its files copied as they are into data/."""
    from science import archive
    if out.exists():
        shutil.rmtree(out)
    data = out / "data"
    payload = {}
    for p in files_of(folder):
        rel = p.relative_to(folder).as_posix()
        q = data / rel
        q.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(p, q)
        payload[rel] = archive.path(q)
    info = [["Source-Organization", "the completion of the book, first attempt (Koinonia, 2026-11-26)"],
            ["External-Description", "the first attempt to complete Seventeen Goals, Fifteen Years, carried into the completion re-founded at the node"],
            ["External-Identifier", "first attempt"]]
    bag = archive.ask("package", timeout=900.0, payload=payload, info=info, date=date)
    for path, text in bag["tags"].items():
        (out / path).write_text(text, encoding="utf-8", newline="\n")
    files = {p.relative_to(out).as_posix(): archive.path(p) for p in out.rglob("*") if p.is_file()}
    v = archive.ask("validate", timeout=900.0, files=files)
    tag = out / "tagmanifest-sha256.txt"
    digest = sha256_file(tag if tag.is_file() else out / "manifest-sha256.txt")
    return {"valid": bool(v.get("valid")), "validation": v, "digest": digest, "files": len(payload), "folder": out}


# ---- Episodes from the journal ----------------------------------------------------------------------------------------------------

DIGESTS = ("digest", "of version")


def episode_of(e: dict, names: set[str]) -> dict:
    """An entry of the side journal as an episode of conduct: grade A where it carries a digest, else B (attested by the sealed journal)."""
    data = e.get("data") if isinstance(e.get("data"), dict) else {}
    who = author_of(e.get("by"), names)
    party = f"first attempt/{who['agent']}" if who["agent"] else "public"
    act = e.get("act", "")
    digests = [str(data[k]) for k in DIGESTS if data.get(k)]
    if act == "request":
        operation = "read" if data.get("method") == "GET" else "act"
    else:
        operation = {"read": "read", "run": "run tool", "run refused": "run tool", "check": "compute", "review": "write"}.get(act, "act")
    features = {"entry": e.get("n"), "seal": e.get("seal"), "first attempt act": act, "agent": who["agent"] or None,
                "run by": "the author" if who["run by"] else None}
    if act == "request":
        features.update(path=scrub(data.get("path", ""), names), status=data.get("status"), ms=data.get("ms"))
    if act == "run refused":
        features["refused"] = "yes"
    if act in ("check", "review"):
        features["verdict"] = str(data.get("ok") if act == "check" else data.get("verdict", ""))
    return {"observe": episode(party, operation, route=f"first attempt: {act}", method=str(data.get("method", "")) if act == "request" else "",
                               begun=str(e.get("at", "")), provider=who["provider"], model=who["model"], bearer="the author" if who["run by"] else "",
                               inputs=[str(data["of version"])] if data.get("of version") else [],
                               outputs=[str(data["digest"])] if data.get("digest") else [], holdings=[JOURNAL_HOLDING],
                               items=[str(data["section"])] if data.get("section") else [], grade="A" if digests else "B", **features)}


# ---- The carrying -----------------------------------------------------------------------------------------------------------------

def carry(f, folder: Path, bag_dir: Path, date: str) -> dict:
    node, C = f.node, completion.COMMUNITY
    fa = read_first_attempt(folder)
    if not fa["journal"]["holds"]:
        raise ValueError(f"the first attempt's journal does not seal at entry {fa['journal']['first fault']}: nothing is carried")
    bag = make_bag(folder, bag_dir, date)
    if not bag["valid"]:
        raise ValueError(f"the bag does not validate: {bag['validation']}")
    j = fa["journal"]
    f.step(C, [f.act(C, f.steward, "deposit", {"holding": BAG_HOLDING, "within": "first attempt", "kind": "record", "digest": bag["digest"],
                                               "source": "the first attempt's folder as a BagIt bag made and validated by archeion (its tag manifest's SHA-256)",
                                               "files": str(bag["files"]), "valid": "yes"}),
               f.act(C, f.steward, "deposit", {"holding": JOURNAL_HOLDING, "within": "first attempt", "kind": "record", "digest": fa["journal sha256"],
                                               "source": "the first attempt's journal.jsonl, its chain verified before the deposit",
                                               "entries": str(j["count"]), "head": j["head"], "verified": "the chain holds"})],
           "the first attempt's deposits")
    # The decision on carried work, as the author made it on 2026-11-27.
    d = decide(fa)
    accepted = [{"section": sid, "version": fa["sections"][sid]["version"], "sha256": fa["sections"][sid]["sha256"]} for sid in d["accepted"]]
    listed = json.dumps(accepted, ensure_ascii=False, sort_keys=True, separators=(",", ":"))
    f.step(C, [f.act(C, completion.AUTHOR, "propose", {"id": DECISION, "decision": DECISION, "rule": completion.RULE, "accepted": listed,
                                                       "digest": hashlib.sha256(listed.encode("utf-8")).hexdigest()},
                     decided="2026-11-27, the design's Section 15", entered="by Claude Code at the author's request"),
               f.act(C, completion.AUTHOR, "vote", {"id": DECISION, "value": "yes"})], "the decision on carried work")
    prop = next(p for p in node.state(C).get("proposals", []) if p["id"] == DECISION)
    if not prop.get("passes"):
        raise ValueError("the decision on carried work did not pass")
    deposits = []
    for sid, s in sorted(fa["sections"].items()):
        r = fa["reviews"].get(sid)
        kept = sid in d["accepted"]
        deposits.append(f.act(C, f.steward, "deposit", {
            "holding": f"{sid} {s['version']}", "within": "adopted" if kept else "first attempt", "kind": "document", "digest": s["sha256"],
            "source": f"the first attempt, {s['file']}, version {s['version']}", "section": sid, "version": s["version"], "version digest": s["version digest"],
            "by": json.dumps(s["by"], ensure_ascii=False, sort_keys=True), "standing": "accepted" if kept else ("to revise" if sid in d["revise"] else "to review"),
            "review": f"review {sid} {r['as']}" if r else "none", **({"decision": DECISION} if kept else {})}))
    for sid, r in sorted(fa["reviews"].items()):
        deposits.append(f.act(C, f.steward, "deposit", {
            "holding": f"review {sid} {r['as']}", "within": "reviews", "kind": "document", "digest": r["sha256"], "source": f"the first attempt, {r['file']}",
            "section": sid, "by": json.dumps(r["by"], ensure_ascii=False, sort_keys=True), "verdict": r["verdict"], "of version": r["of version"],
            "bears on": "the current version" if r["current"] else "an earlier version", "faults": str(r["faults"]), "reviews kept": str(r["reviews kept"])}))
    for sid, h in sorted(fa["results"].items()):
        deposits.append(f.act(C, f.steward, "deposit", {"holding": f"results {sid}", "within": "facts", "kind": "facts", "digest": h,
                                                        "source": f"the first attempt, results/{sid}.json, computed on the bench", "section": sid}))
    if fa["replay"]:
        deposits.append(f.act(C, f.steward, "deposit", {"holding": "the replay's totals", "within": "facts", "kind": "facts", "digest": fa["replay"],
                                                        "source": "the first attempt's replay.json: every result the guild kept, run again on the bench"}))
    f.step(C, deposits, "a carried deposit")
    # The remaining work.
    plan = fa["plan"]
    unwritten = sorted(set(plan) - set(fa["sections"]) - {"c01-1"})
    posts = [f.act(C, f.steward, "request", {"id": sid, "work": "section", "spec": "the plan of the completion", "matter": "the book",
                                             "note": "never written in the first attempt"}) for sid in unwritten]
    for sid in d["revise"] + d["review"]:
        s, r = fa["sections"][sid], fa["reviews"].get(sid)
        why = ("its standing review asks for revision" if sid in d["revise"] else
               "its standing review bears on an earlier version" if r else "it has no review by another provider")
        posts.append(f.act(C, f.steward, "request", {"id": sid, "work": "section", "spec": f"{sid} {s['version']}", "matter": "the book",
                                                     "note": f"carried from the first attempt, version {s['version']}: {why}; deliver it as carried or mended, "
                                                             f"to be reviewed by another provider than its writer's ({s['by'].get('provider', '')})",
                                                     "carried": "to revise" if sid in d["revise"] else "to review"}))
    if "c01-1" in plan:
        posts.append(f.act(C, f.steward, "request", {"id": "c01-1", "work": "overview", "spec": "the plan of the completion", "matter": "the book",
                                                     "note": "written last, after every section is accepted"}))
    f.step(C, posts, "the posted work")
    # The side journal's entries as episodes of the completion's archive.
    f.step(C, [episode_of(e, fa["names"]) for e in j["entries"]])
    h = node.hosted(C)
    h.content["first attempt"] = {"bag": {"sha256": bag["digest"], "files": bag["files"], "folder": bag["folder"].name},
                                  "journal": {"sha256": fa["journal sha256"], "entries": j["count"], "head": j["head"]}}
    node.save(C)
    return {"read": fa, "decision": d, "bag": bag, "unwritten": unwritten}


# ---- The retirement ---------------------------------------------------------------------------------------------------------------

def close_journal(folder: Path, data: dict, by: str) -> dict:
    """Appends the final entry to the first attempt's journal by its own sealing rule; refuses when the chain does not hold."""
    import time
    path = Path(folder) / "journal.jsonl"
    v = verify_journal(path)
    if not v["holds"]:
        raise ValueError(f"the journal does not seal at entry {v['first fault']}: it is not closed")
    last = v["entries"][-1] if v["entries"] else None
    entry = {"n": (last["n"] + 1) if last else 1, "at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()), "act": "close", "by": by, "data": data,
             "previous": last["seal"] if last else "0" * 64}
    entry["seal"] = seal_of(entry)
    with path.open("a", encoding="utf-8", newline="\n") as out:
        out.write(json.dumps(entry, ensure_ascii=False) + "\n")
    return entry


GONE = '''
{marker}
# The completion was re-founded as a community at Koinonia's node on {date}; this server answers every route with 410 Gone and the
# new address, and invites no one. Its code above is kept as it was; the first attempt is carried as a bag ({bag}).
MOVED_TO = {address!r}


def _gone(self):
    body = json.dumps({{"gone": "the first attempt's server is retired: the completion is a community at Koinonia's node",
                        "address": MOVED_TO, "guide": {guide!r}, "bag": {bag!r}}}, ensure_ascii=False).encode("utf-8")
    self.send_response(410)
    self.send_header("Location", MOVED_TO)
    self.send_header("Content-Type", "application/json; charset=utf-8")
    self.send_header("Content-Length", str(len(body)))
    self.end_headers()
    self.wfile.write(body)


Handler.do_GET = Handler.do_POST = _gone


def invite(*args, **kwargs):
    raise SystemExit(f"the first attempt's server is retired; the completion is at {{MOVED_TO}}")


'''


def retire_server(folder: Path, address: str, guide: str, bag: str, date: str) -> bool:
    """Makes the first attempt's serve.py answer 410 Gone with the new address (the block inserted before its main guard); False when
    it was retired already."""
    p = Path(folder) / "serve.py"
    text = p.read_text(encoding="utf-8")
    if RETIRED in text:
        return False
    guard = '\nif __name__ == "__main__":'
    if guard not in text:
        raise ValueError("the first attempt's serve.py has no main guard to retire before")
    block = GONE.format(marker=RETIRED, date=date, address=address, guide=guide, bag=bag)
    k = text.rindex(guard)
    p.write_text(text[:k] + "\n" + block + text[k + 1:], encoding="utf-8", newline="\n")
    return True


def retire(f, folder: Path, carried: dict, date: str) -> dict:
    node, C = f.node, completion.COMMUNITY
    head = node.session(C).head
    address = f"{node.url}c/{C}/profile"
    entry = close_journal(folder, {"carried to": C, "node": node.url, "address": address, "bag": {"sha256": carried["bag"]["digest"], "files": carried["bag"]["files"]},
                                   "community head": {"size": head["size"], "root": head["root"]}, "entries carried": carried["read"]["journal"]["count"],
                                   "head carried": carried["read"]["journal"]["head"]},
                          f"the completion's steward ({completion.STEWARD['agent']}) / {completion.STEWARD['model']} / Anthropic / run by the author")
    retired = retire_server(folder, address, node.url, carried["bag"]["digest"], date)
    return {"closing entry": entry, "server retired": retired, "address": address}


# ---- The whole -------------------------------------------------------------------------------------------------------------------

def run(folder: Path, store: Path, url: str, date: str, society: Path | None = SOCIETY, rate: int = 10 ** 6) -> dict:
    """Founds the completion at the node kept in ``store``, carries the first attempt, lists the completion, briefs its steward and
    retires the first attempt's server. Answers the report (no credential in it)."""
    from koinonia.gateway import Gateway
    from koinonia.node import Node
    node = Node(Path(store), url=url, rate=rate)
    gateway = Gateway(node)
    f = completion.found(node, gateway, plan=Path(folder) / "plan.json", society=society)
    carried = carry(f, Path(folder), Path(store) / "communities" / completion.COMMUNITY / "first-attempt", date)
    listed = completion.list_in_registry(f, date)
    brief = completion.brief_steward(f)
    retired = retire(f, Path(folder), carried, date)
    work = node.session(completion.COMMUNITY).work().get("work", [])
    fa, d = carried["read"], carried["decision"]
    report = {"format": "noetarium.completion-carried/1", "date": date, "node": url, "profile": listed["profile"],
              "counts": {"plan sections": len(fa["plan"]), "carried sections": len(fa["sections"]), "carried reviews": len(fa["reviews"]),
                         "results carried": len(fa["results"]), "journal entries": fa["journal"]["count"], "accepted": len(d["accepted"]),
                         "to revise": len(d["revise"]), "to review": len(d["review"]), "unwritten": len(carried["unwritten"]),
                         "work open": sum(1 for w in work if w.get("state") == "open"), "work waiting": sum(1 for w in work if w.get("state") == "waiting")},
              "bag": {"sha256": carried["bag"]["digest"], "files": carried["bag"]["files"], "valid": carried["bag"]["valid"]},
              "listing": {"digest": listed["digest"], "entry": listed["entry"], "verified": listed["verified"]},
              "steward": {"party": f.steward, "mandate": f.mandate, "positions": brief.get("positions"), "acts": [a["kind"] for a in brief.get("acts", [])]},
              "closing entry": {k: retired["closing entry"][k] for k in ("n", "seal")}, "server retired": retired["server retired"],
              "head": dict(node.session(completion.COMMUNITY).head)}
    (Path(store) / "completion-carried.json").write_text(json.dumps(report, ensure_ascii=False, indent=1), encoding="utf-8")
    return report


def main(argv: list[str]) -> int:
    import datetime as dt
    ap = argparse.ArgumentParser()
    ap.add_argument("--first-attempt", default=str(FIRST_ATTEMPT))
    ap.add_argument("--store", default=str(STORE))
    ap.add_argument("--url", default="http://127.0.0.1:9230/")
    ap.add_argument("--date", default=dt.date.today().isoformat())
    a = ap.parse_args(argv)
    r = run(Path(a.first_attempt), Path(a.store), a.url, a.date)
    print(json.dumps({k: r[k] for k in ("profile", "counts", "bag", "listing", "steward", "closing entry", "server retired", "head")}, ensure_ascii=False, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
