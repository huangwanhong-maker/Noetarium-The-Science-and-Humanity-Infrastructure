"""The server side of the plug-in *Evidence in space and time* (ADR 0123): a project's archive of evidence, kept in the project's
folder, on the API's host for project-based works (a plug-in's routes.py).

A project that keeps evidence has, beside what every project has:

    evidence/archive.json     the archive, noetarium.evidence/1 (api/cpp/science/spacetime_evidence.hpp): its sources, its holdings
                              with their chains of custody, its items (each a report of something at a place and a time), its
                              claims, the rule for a claim's standing, and the decision
    evidence/holdings/        the files as they were taken in, each kept under its name; the SHA-256 of its bytes at that moment
                              is in the archive, and is observed again whenever the archive is appraised

Routes, under /api/projects/<project>/evidence:

    GET  (none)                    the archive, the SHA-256 it is kept under, and the examples that can be laid out
    PUT  (none)   {archive, base}  keep it (refused with the current one when it changed since `base`)
    POST appraise {archive?}       the engine's answer: what is kept and whether it is still what it was, each claim weighed, and
                                   the decision; each holding's digest observed now from its file
    POST holdings?name=<name>      a file taken in (its bytes as the body): kept, digested, and entered among the holdings with
                                   whoever took it in as its first custodian
    GET  holdings/<name>           a holding's bytes
    POST transfer {holding, to, note}   a handover written into a holding's chain, with the digest observed at that moment
    POST example  {case}           an example laid out from the data library's cache (data/products/urban_evidence.py)
    POST take     {table, as}      rows of a table of the project taken in as items, claims or sources
    GET  place                     the ways and footprints of the archive's place, from the data library's world tiles
                                   (data/acquisition/world_city.py, Overture Maps, ODbL), for the page to draw it

What is computed is the foundations', asked of the plug-in's engine through science.spacetime_evidence. Writes keep the host's
rule (write_refusal); every change is written in the project's journal.
"""

from __future__ import annotations

import datetime as dt
import hashlib
import json
import mimetypes
import shutil
import sys
import threading
import urllib.parse
from pathlib import Path

from science import spacetime_evidence as se
from science import studio_host as host

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "data" / "acquisition"))
import world_city  # noqa: E402

ROOT = Path(__file__).resolve().parents[4]
CASES = ROOT / "data" / "cache" / "products" / "urban-evidence"
TEXT_LIMIT = 64 * 1024 * 1024
FILE_LIMIT = 512 * 1024 * 1024
LOCK = threading.Lock()
BY: dict[int, str] = {}


def now() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def sha(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def who() -> str:
    return BY.get(threading.get_ident(), host.PEOPLE["keeper"])


def folder(project: str) -> Path:
    return host.project_dir(project) / "evidence"


def check_name(name: str) -> str:
    name = Path(str(name)).name
    if not name or name.startswith(".") or any(c in name for c in '\\/:*?"<>|'):
        raise ValueError(f"not a name a holding may have: {name!r}")
    return name


def blank() -> dict:
    return {"schema": se.SCHEMA, "title": "", "about": "", "sources": [], "holdings": [], "items": [], "claims": [],
            "rule": {"established": 0.95, "refuted": 0.05}, "decision": None}


def read(project: str) -> dict:
    path = folder(project) / "archive.json"
    return json.loads(path.read_text(encoding="utf-8")) if path.is_file() else blank()


def keep(project: str, archive: dict, base: str | None, what: str = "kept the archive of evidence", note: str = "") -> dict:
    path = folder(project) / "archive.json"
    with LOCK:
        if base is not None and path.is_file():
            current = path.read_text(encoding="utf-8")
            if sha(current) != base:
                return {"message": "the archive changed since it was read", "current": json.loads(current), "sha256": sha(current), "conflict": True}
        if not isinstance(archive, dict) or archive.get("schema", se.SCHEMA) != se.SCHEMA:
            raise ValueError(f"an archive of evidence is a JSON object of the schema {se.SCHEMA}")
        for key in ("sources", "holdings", "items", "claims"):
            ids = [str(x.get("id", "")) for x in archive.get(key, [])]
            if any(not i for i in ids):
                raise ValueError(f"each of the {key} has an id")
            twice = sorted({i for i in ids if ids.count(i) > 1})
            if twice:
                raise ValueError(f"the {key} name {twice[0]} more than once")
        archive["schema"] = se.SCHEMA
        text = json.dumps(archive, ensure_ascii=False, indent=1)
        path.parent.mkdir(parents=True, exist_ok=True)
        with host.FILES:
            path.write_text(text, encoding="utf-8")
    host.record(project, who(), what, archive.get("title") or project, note)
    return {"kept": True, "sha256": sha(text)}


def cases() -> list[dict]:
    out = []
    for path in sorted(CASES.glob("*/archive.json")) if CASES.is_dir() else []:
        try:
            a = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        out.append({"id": path.parent.name, "title": a.get("title", path.parent.name), "about": a.get("about", ""), "items": len(a.get("items", [])), "claims": len(a.get("claims", []))})
    return out


def appraise(project: str, archive: dict | None = None) -> dict:
    a = archive if archive is not None else read(project)
    return {"appraisal": se.ask("all", archive=se.prepared(a, folder(project) / "holdings"))}


def take_in(project: str, name: str, data: bytes, note: str = "") -> dict:
    """A file taken in as a holding: kept under a name no other has, its digest taken, entered in the archive with its custodian."""
    d = folder(project) / "holdings"
    d.mkdir(parents=True, exist_ok=True)
    name = check_name(name)
    stem, suffix = Path(name).stem, Path(name).suffix
    target, k = d / name, 1
    while target.exists():
        target, k = d / f"{stem} ({k}){suffix}", k + 1
    digest = hashlib.sha256(data).hexdigest()
    with host.FILES:
        target.write_bytes(data)
    a = read(project)
    ids = {h["id"] for h in a["holdings"]}
    ident, k = "h1", 1
    while ident in ids:
        k += 1
        ident = f"h{k}"
    holding = {"id": ident, "name": target.name, "digest": digest, "custodian": who(), "since": now(), "transfers": [], "bytes": len(data), "note": note}
    a["holdings"].append(holding)
    keep(project, a, None, "took a file in as evidence", f"{target.name}, SHA-256 {digest}")
    return {"holding": holding}


def transfer(project: str, holding: str, to: str, note: str = "") -> dict:
    """A handover: from whoever holds it now to `to`, with the digest of the file as it is at this moment."""
    if not to.strip():
        raise ValueError("a handover names who takes the holding")
    a = read(project)
    for h in a["holdings"]:
        if h["id"] == holding:
            held_by = h["transfers"][-1]["to"] if h.get("transfers") else h.get("custodian", "")
            observed = se.digest(folder(project) / "holdings" / check_name(h.get("name", "")))
            h.setdefault("transfers", []).append({"when": now(), "from": held_by, "to": to.strip(), "digest": observed, "note": note})
            keep(project, a, None, "handed a holding over", f"{h.get('name')}: from {held_by} to {to.strip()}")
            return {"holding": h}
    raise KeyError(f"there is no holding {holding}")


def example(project: str, case: str) -> dict:
    """An example laid out from the data library's cache: its archive, and the files it rests on taken in as they are."""
    base = CASES / check_name(case)
    if not (base / "archive.json").is_file():
        raise KeyError(f"there is no example {case} in the cache: python data/products/urban_evidence.py")
    a = json.loads((base / "archive.json").read_text(encoding="utf-8"))
    d = folder(project) / "holdings"
    d.mkdir(parents=True, exist_ok=True)
    copied = 0
    for h in a.get("holdings", []):
        src = base / check_name(h.get("name", ""))
        if src.is_file() and not (d / src.name).exists():
            with host.FILES:
                shutil.copyfile(src, d / src.name)
            copied += 1
    keep(project, a, None, "laid out an example of evidence", f"{case}: {len(a.get('items', []))} items, {len(a.get('claims', []))} claims, {copied} files taken in")
    return {"case": case, "title": a.get("title", ""), "items": len(a.get("items", [])), "claims": len(a.get("claims", [])), "holdings": copied}


def box_of(a: dict) -> dict | None:
    """The archive's box: its frame's, else the one that holds its placed items and claims, with a margin of a tenth."""
    if isinstance((a.get("frame") or {}).get("box"), dict):
        return a["frame"]["box"]
    pts = [x for x in a.get("items", []) + a.get("claims", []) if isinstance(x.get("lat"), (int, float)) and isinstance(x.get("lon"), (int, float))]
    if not pts:
        return None
    s, n = min(p["lat"] for p in pts), max(p["lat"] for p in pts)
    w, e = min(p["lon"] for p in pts), max(p["lon"] for p in pts)
    dl, dn = max(n - s, 0.002) * 0.1, max(e - w, 0.002) * 0.1
    return {"south": s - dl, "north": n + dl, "west": w - dn, "east": e + dn}


def place(project: str) -> dict:
    """The place the archive is about, as a page draws it: the case's basemap when the archive names one, else the world tiles'."""
    a = read(project)
    ref = str(a.get("basemap") or "")
    if ref.startswith("data:"):
        path = (ROOT / "data" / "cache" / ref[5:]).resolve()
        if (ROOT / "data" / "cache").resolve() in path.parents and path.is_file():
            return json.loads(path.read_text(encoding="utf-8"))
    box = box_of(a)
    if box is None:
        return {"box": None, "ways": [], "buildings": [], "credit": ""}
    return world_city.within(box)


def take(project: str, table: str, what: str) -> dict:
    """Rows of a table of the project taken in as items, claims or sources: a row whose id the archive has replaces it."""
    if what not in ("items", "claims", "sources"):
        raise ValueError("rows are taken in as items, claims or sources")
    t = host.find_table(project, host.read_project(project), table)
    names = [c["name"] for c in t["columns"]]
    rows = [{n: t["data"][n][i] for n in names} for i in range(t["rows"])]
    got = se.read_table(rows, what)
    a = read(project)
    by = {x["id"]: k for k, x in enumerate(a[what])}
    new = 0
    for x in got:
        if x["id"] in by:
            a[what][by[x["id"]]] = x
        else:
            a[what].append(x)
            new += 1
    keep(project, a, None, f"took a table's rows in as {what}", f"{table}: {len(got)} rows, {new} of them new")
    return {"taken": len(got), "new": new}


# ---- The routes ----------------------------------------------------------------------------------------------------------------------


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The plug-in's routes; True when this answered."""
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] == "evidence"):
        return False
    BY[threading.get_ident()] = host.who(handler)
    project, rest = parts[2], parts[4:]
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the archive is seen by its people: ask the keeper for a key"}, 403)
        return True

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    if method == "GET":
        if not rest:
            def get():
                exists()
                a = read(project)
                kept = (folder(project) / "archive.json").is_file()
                return {"archive": a, "sha256": sha(json.dumps(a, ensure_ascii=False, indent=1)) if kept else None, "kept": kept, "cases": cases()}, 200
            handler.answer(get)
            return True
        if rest == ["place"]:
            handler.answer(lambda: (exists(), (place(project), 200))[1])
            return True
        if len(rest) == 2 and rest[0] == "holdings":
            path = folder(project) / "holdings" / check_name(urllib.parse.unquote(rest[1]))
            if not path.is_file():
                handler.reply_json({"message": f"there is no holding {rest[1]}"}, 404)
                return True
            data = path.read_bytes()
            handler.send_response(200)
            handler.send_header("Content-Type", mimetypes.guess_type(path.name)[0] or "application/octet-stream")
            handler.send_header("Content-Length", str(len(data)))
            handler.send_header("Cache-Control", "no-store")
            handler.end_headers()
            handler.wfile.write(data)
            return True
        return False
    what = rest[0] if rest else ""
    if what == "appraise" and method == "POST":   # computed, and keeps nothing: any of the people may ask
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (appraise(project, body.get("archive")), 200))[1])
        return True
    refusal = host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    if not rest and method == "PUT":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def put():
                exists()
                answer = keep(project, body.get("archive"), body.get("base"), body.get("what") or "kept the archive of evidence", body.get("note", ""))
                return answer, 409 if answer.get("conflict") else 200
            handler.answer(put)
        return True
    if what == "holdings" and method == "POST":
        data = handler.body(FILE_LIMIT)
        if data is not None:
            handler.answer(lambda: (exists(), (take_in(project, query.get("name", "evidence"), data, query.get("note", "")), 200))[1])
        return True
    if method == "POST" and what in ("transfer", "example", "take"):
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            act = {"transfer": lambda: transfer(project, str(body.get("holding", "")), str(body.get("to", "")), str(body.get("note", ""))),
                   "example": lambda: example(project, str(body.get("case", ""))),
                   "take": lambda: take(project, str(body.get("table", "")), str(body.get("as", "items")))}[what]
            handler.answer(lambda: (exists(), (act(), 200))[1])
        return True
    return False
