"""Histos Harmonia, served locally (ADR 0110).

The page stands on the visualization layer and asks this server, which asks the work's engine (``engine.py``): historical objects
by the history domain, tunings by musicology, argument maps by philosophy. ``domains/``, ``foundations/`` and ``visualization/``
are mounted beside the page, and the data cache at ``/data/``.

An archive is a folder that its institution owns (``--archive``, by default ``data/cache/histos-harmonia/archive``):

    records/<id>.jsonl      working copies of records, made from the library's (domains/history/records) at the first change
    holdings/<aa>/<sha256>  the materials, each kept under the hash of its content and never altered
    holdings.jsonl          what each holding is: its name, size, type, the objects it is of, its rights and access
    content/<record>/<object>/<activity>.json   what is written for an activity on an object
    log.jsonl               what was done, one event a line, only ever added to

What reads:   GET  /api/records · /api/record · /api/object · /api/frontier · /api/verify · /api/tuning · /api/plugins ·
                   /api/content · /api/holdings · /api/holding/<sha256> · /api/log · /api/export · /setup
What writes:  POST /api/gather · /api/map (these two only compute) · /api/assert · /api/source · /api/holdings · /api/holding ·
                   /api/fixity · /setup/run;  PUT /api/content

Writing is for the server's own page at this computer's own address: the first version serves one institution on one machine,
and has no accounts. An assertion of grade B is taken only with its passage; when the holding it is read from is named, the
passage must be found in that holding's text, and the server looks.
"""

from __future__ import annotations

import argparse
import hashlib
import http.server
import io
import json
import mimetypes
import re
import sys
import threading
import time
import urllib.parse
import webbrowser
import zipfile
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
LIBRARY = ROOT / "domains" / "history" / "records"
SCHEMA = ROOT / "domains" / "history" / "schema" / "historical-objects.cat"
ARCHIVE = ROOT / "data" / "cache" / "histos-harmonia" / "archive"
MOUNTS = {
    "/domains/": ROOT / "domains",
    "/foundations/": ROOT / "foundations",
    "/visualization/": ROOT / "visualization",
    "/data/": ROOT / "data" / "cache",
}
TYPES = {".html": "text/html; charset=utf-8", ".js": "text/javascript; charset=utf-8", ".mjs": "text/javascript; charset=utf-8", ".css": "text/css; charset=utf-8",
         ".json": "application/json", ".svg": "image/svg+xml", ".png": "image/png", ".geojson": "application/geo+json"}
NAME = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._\-]{0,79}$")
HASH = re.compile(r"^[0-9a-f]{64}$")
ACCESS = ("public", "restricted", "dark")
LIMIT = 64 * 1024 * 1024  # the bytes of one holding taken over the page

sys.path.insert(0, str(APP))
import engine  # noqa: E402

RECIPE = None
LOCK = threading.Lock()
CACHE: dict[tuple, dict] = {}
PUBLIC = {"on": False}


def now() -> str:
    return time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())


def safe(name: str) -> str:
    if not NAME.match(name):
        raise ValueError(f"not a name this archive keeps: {name!r}")
    return name


def record_file(record: str) -> Path:
    """The working copy when the archive has one, else the library's."""
    own = ARCHIVE / "records" / f"{safe(record)}.jsonl"
    if own.exists():
        return own
    lib = LIBRARY / f"{record}.jsonl"
    if not lib.exists():
        raise ValueError(f"no record named {record}")
    return lib


def working_copy(record: str) -> Path:
    own = ARCHIVE / "records" / f"{safe(record)}.jsonl"
    if not own.exists():
        own.parent.mkdir(parents=True, exist_ok=True)
        own.write_bytes(record_file(record).read_bytes())
        log("copied", record=record, note="a working copy made from the library's record")
    return own


def log(event: str, **what) -> None:
    ARCHIVE.mkdir(parents=True, exist_ok=True)
    with (ARCHIVE / "log.jsonl").open("a", encoding="utf-8") as f:
        f.write(json.dumps({"at": now(), "event": event, **what}, ensure_ascii=False) + "\n")


def asked(op: str, record: str | None = None, **request) -> dict:
    """The engine's answer, kept while the record's file does not change."""
    key_file = record_file(record) if record else None
    stamp = (key_file.stat().st_mtime_ns, str(key_file)) if key_file else ()
    key = (op, stamp, json.dumps(request, sort_keys=True, ensure_ascii=False))
    if key in CACHE:
        return CACHE[key]
    if key_file:
        request["path"] = engine.path_for_engine(key_file)
    answer = engine.ask(op, **request)
    with LOCK:
        if len(CACHE) > 600:
            CACHE.clear()
        CACHE[key] = answer
    return answer


def records() -> list[dict]:
    out = {}
    for folder, where in ((LIBRARY, "library"), (ARCHIVE / "records", "archive")):
        for f in sorted(folder.glob("*.jsonl")) if folder.is_dir() else []:
            try:
                head = json.loads(f.read_text(encoding="utf-8").split("\n", 1)[0])
            except ValueError:
                head = {}
            out[f.stem] = {"id": f.stem, "title": head.get("title", f.stem), "about": head.get("about", ""), "where": where}
    return list(out.values())


def plugins() -> list[dict]:
    out = []
    for f in sorted((APP / "plugins").glob("*/plugin.json")):
        try:
            p = json.loads(f.read_text(encoding="utf-8"))
            p["id"] = f.parent.name
            out.append(p)
        except ValueError as error:
            out.append({"id": f.parent.name, "fault": f"plugin.json is not read: {error}", "activities": []})
    return out


# ---- holdings ------------------------------------------------------------------------------------------------------------------

def holdings() -> list[dict]:
    f = ARCHIVE / "holdings.jsonl"
    rows: dict[str, dict] = {}
    for line in f.read_text(encoding="utf-8").splitlines() if f.exists() else []:
        if line.strip():
            row = json.loads(line)
            rows[row["sha256"]] = {**rows.get(row["sha256"], {}), **row}
    return [r for r in rows.values() if not r.get("withdrawn")]


def holding_path(sha: str) -> Path:
    if not HASH.match(sha):
        raise ValueError("a holding is named by the SHA-256 of its content")
    return ARCHIVE / "holdings" / sha[:2] / sha


def describe(row: dict) -> None:
    with (ARCHIVE / "holdings.jsonl").open("a", encoding="utf-8") as f:
        f.write(json.dumps(row, ensure_ascii=False) + "\n")


def ingest(data: bytes, name: str, objects: list[str], rights: str, access: str, note: str) -> dict:
    if access not in ACCESS:
        raise ValueError("access is public, restricted or dark")
    if not rights.strip():
        raise ValueError("every holding carries a statement of its rights")
    sha = hashlib.sha256(data).hexdigest()
    path = holding_path(sha)
    fresh = not path.exists()
    if fresh:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
    row = {"sha256": sha, "name": name, "bytes": len(data), "type": mimetypes.guess_type(name)[0] or "application/octet-stream", "objects": objects,
           "rights": rights.strip(), "access": access, "note": note, "ingested": now()}
    describe(row)
    log("ingested" if fresh else "described", sha256=sha, name=name, bytes=len(data), objects=objects, access=access)
    return row


def fixity() -> dict:
    rows, wrong, missing = holdings(), [], []
    for r in rows:
        p = holding_path(r["sha256"])
        if not p.exists():
            missing.append(r["sha256"])
        elif hashlib.sha256(p.read_bytes()).hexdigest() != r["sha256"]:
            wrong.append(r["sha256"])
    log("fixity", checked=len(rows), altered=wrong, missing=missing)
    return {"checked": len(rows), "altered": wrong, "missing": missing, "at": now()}


def holding_text(sha: str) -> str:
    row = next((r for r in holdings() if r["sha256"] == sha), None)
    if row is None:
        raise ValueError("the archive holds nothing under that hash")
    if not (row["type"].startswith("text/") or row["name"].endswith((".txt", ".md", ".xml", ".tei", ".html"))):
        raise ValueError("a passage can be looked for only in a holding of text")
    return holding_path(sha).read_text(encoding="utf-8", errors="replace")


def squeezed(text: str) -> str:
    return re.sub(r"\s+", " ", text).strip().casefold()


# ---- the record's working copy ----------------------------------------------------------------------------------------------------

def rows_of(path: Path) -> list[dict | str]:
    out: list[dict | str] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        try:
            out.append(json.loads(line) if line.strip() and not line.lstrip().startswith("#") else line)
        except ValueError:
            out.append(line)
    return out


def write_rows(path: Path, rows: list[dict | str]) -> None:
    path.write_text("\n".join(json.dumps(r, ensure_ascii=False) if isinstance(r, dict) else r for r in rows) + "\n", encoding="utf-8")


def add_assertion(body: dict) -> dict:
    """An assertion added to a statement of the record: who says it, with what grade, and who confirmed it against the source."""
    record, row_kind, row_id = body["record"], body["row"], body["id"]
    grade, by = body.get("grade", ""), body.get("by", "")
    if grade not in ("A", "B"):
        raise ValueError("a person adds assertions of grade A (a structured source records it) or B (a text states it); C is a model's")
    quote = str(body.get("quote", "")).strip()
    if grade == "B" and not quote:
        raise ValueError("an assertion of grade B quotes the passage that states it")
    confirmed = str(body.get("confirmed", "")).strip()
    found = None
    if body.get("holding"):
        found = squeezed(quote) in squeezed(holding_text(body["holding"])) if quote else None
        if quote and not found:
            raise ValueError("the passage is not found in the holding named: nothing was added")
    path = working_copy(record)
    rows = rows_of(path)
    if not any(isinstance(r, dict) and r.get("row") == "source" and r.get("id") == by for r in rows):
        raise ValueError(f"\"{by}\" is no source of the record: add the source first")
    assertion = {"by": by, "grade": grade}
    if quote:
        assertion["quote"] = quote
    for key in ("where",):
        if body.get(key):
            assertion[key] = str(body[key])
    if body.get("documentary"):
        assertion["documentary"] = True
    if body.get("disputes"):
        assertion["disputes"] = True
    if confirmed:
        assertion["confirmed"] = f"{confirmed}, {now()[:10]}" + (f"; passage found in holding {body['holding'][:12]}" if found else "")
    target = None
    for r in rows:
        if not isinstance(r, dict):
            continue
        if row_kind in ("object", "relation", "emergence") and r.get("row") == row_kind and r.get("id") == row_id:
            target = r
        elif row_kind == "premise" and r.get("row") == "emergence" and r.get("id") == row_id:
            target = next((p for p in r.get("premises", []) if p.get("what") == body.get("what")), None)
    if target is None:
        raise ValueError(f"the record holds no {row_kind} {row_id}")
    target.setdefault("said", []).append(assertion)
    write_rows(path, rows)
    log("asserted", record=record, row=row_kind, id=row_id, by=by, grade=grade, confirmed=bool(confirmed), passage_found=found)
    return {"added": assertion}


def add_source(body: dict) -> dict:
    record = body["record"]
    row = {"row": "source", "id": safe(str(body.get("id", ""))), "kind": body.get("kind", ""), "cite": str(body.get("cite", "")).strip()}
    if row["kind"] not in ("primary", "reference", "structured"):
        raise ValueError("a source a person adds is primary, a reference or structured")
    if not row["cite"]:
        raise ValueError("a source is cited in words")
    for key in ("url", "language", "object"):
        if body.get(key):
            row[key] = str(body[key])
    path = working_copy(record)
    rows = rows_of(path)
    if any(isinstance(r, dict) and r.get("row") == "source" and r.get("id") == row["id"] for r in rows):
        raise ValueError("the source is there already")
    at = max((i for i, r in enumerate(rows) if isinstance(r, dict) and r.get("row") == "source"), default=0)
    rows.insert(at + 1, row)
    write_rows(path, rows)
    log("source added", record=record, id=row["id"], kind=row["kind"])
    return {"added": row}


# ---- content of activities --------------------------------------------------------------------------------------------------------

def content_file(record: str, obj: str, activity: str) -> Path:
    return ARCHIVE / "content" / safe(record) / safe(obj) / f"{safe(activity)}.json"


def content(record: str, obj: str, activity: str) -> dict:
    """What is written for an activity on an object: its own, or that of the nearest object it stands under."""
    lineage = asked("object", record, id=obj, layer="proposed")["lineage"]
    for candidate, through in [(obj, None)] + [(a["object"], a["object"]) for a in lineage]:
        f = content_file(record, candidate, activity)
        if f.exists():
            return {"content": json.loads(f.read_text(encoding="utf-8")), "inherited": through}
    return {"content": None, "inherited": None}


def export(record: str) -> bytes:
    """The record with the holdings of its objects as one package, laid out as a bag (BagIt): data, a manifest of SHA-256, bag-info."""
    data = asked("open", record)
    ids = {o["id"] for o in data["objects"]}
    out = io.BytesIO()
    manifest = []
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        text = record_file(record).read_bytes()
        z.writestr(f"data/{record}.jsonl", text)
        manifest.append((hashlib.sha256(text).hexdigest(), f"data/{record}.jsonl"))
        kept = [h for h in holdings() if ids & set(h.get("objects", [])) and h["access"] != "dark"]
        for h in kept:
            p = holding_path(h["sha256"])
            if p.exists():
                z.write(p, f"data/holdings/{h['sha256'][:12]}-{h['name']}")
                manifest.append((h["sha256"], f"data/holdings/{h['sha256'][:12]}-{h['name']}"))
        described = "\n".join(json.dumps(h, ensure_ascii=False) for h in kept).encode("utf-8")
        z.writestr("data/holdings.jsonl", described)
        manifest.append((hashlib.sha256(described).hexdigest(), "data/holdings.jsonl"))
        z.writestr("bagit.txt", "BagIt-Version: 1.0\nTag-File-Character-Encoding: UTF-8\n")
        z.writestr("manifest-sha256.txt", "".join(f"{sha}  {name}\n" for sha, name in manifest))
        z.writestr("bag-info.txt", f"Source-Organization: Histos Harmonia\nBagging-Date: {now()[:10]}\nExternal-Description: {data['title']}\n"
                                   f"External-Identifier: {record}\nPayload-Oxum: {sum(len(z.read(n)) for _, n in manifest)}.{len(manifest)}\n")
    log("exported", record=record, files=len(manifest))
    return out.getvalue()


def write_refusal(handler) -> str | None:
    if PUBLIC["on"]:
        return "this server is open to others and changes nothing: an archive is worked on at its own computer"
    origin = handler.headers.get("Origin")
    if origin is not None and urllib.parse.urlparse(origin).netloc != handler.headers.get("Host", ""):
        return "this server takes changes only from its own page"
    return None


def recipe_job():
    global RECIPE
    if RECIPE is None:
        sys.path.insert(0, str(ROOT / "data" / "acquisition"))
        import recipe
        RECIPE = recipe.Job(APP / "recipe.json", ROOT)
    return RECIPE


class Handler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {**http.server.SimpleHTTPRequestHandler.extensions_map, **TYPES}

    def translate_path(self, path: str) -> str:
        path = urllib.parse.unquote(path.split("?", 1)[0].split("#", 1)[0])
        prefix = next((p for p in MOUNTS if path.startswith(p)), None)
        base, rel = (MOUNTS[prefix], path[len(prefix):]) if prefix else (APP, path.lstrip("/"))
        target = (base / rel).resolve()
        if target != base and base not in target.parents:
            return str(base / "__outside__")
        return str(target)

    def reply(self, body, code: int = 200) -> None:
        data = json.dumps(body, ensure_ascii=False).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def reply_bytes(self, data: bytes, kind: str, name: str | None = None) -> None:
        self.send_response(200)
        self.send_header("Content-Type", kind)
        self.send_header("Content-Length", str(len(data)))
        if name:
            self.send_header("Content-Disposition", f"attachment; filename*=UTF-8''{urllib.parse.quote(name)}")
        self.end_headers()
        self.wfile.write(data)

    def body(self) -> bytes:
        length = int(self.headers.get("Content-Length", "0") or 0)
        if length > LIMIT:
            raise ValueError(f"longer than this server takes over its page ({LIMIT // (1024 * 1024)} MB)")
        return self.rfile.read(length)

    def answer(self, work) -> None:
        try:
            result = work()
            if result is not None:
                self.reply(result)
        except (ValueError, KeyError) as error:
            self.reply({"message": str(error)}, 400)
        except engine.EngineError as error:
            self.reply({"message": str(error)}, 422)
        except OSError as error:
            self.reply({"message": str(error)}, 500)

    def do_GET(self) -> None:
        url = urllib.parse.urlparse(self.path)
        q = dict(urllib.parse.parse_qsl(url.query))
        p = url.path
        if p == "/setup":
            refusal = write_refusal(self)
            return self.answer(lambda: {**recipe_job().view(), "writable": refusal is None, "refusal": refusal})
        if not p.startswith("/api/"):
            return super().do_GET()
        layer = q.get("layer", "established")
        if p == "/api/records":
            return self.answer(lambda: {"records": records(), "engine": engine.ENGINE.status(), "writable": write_refusal(self) is None})
        if p == "/api/plugins":
            return self.answer(lambda: {"plugins": plugins()})
        if p == "/api/record":
            return self.answer(lambda: asked("open", q["record"]))
        if p == "/api/object":
            return self.answer(lambda: asked("object", q["record"], id=q["id"], layer=layer, depth=int(q.get("depth", "0"))))
        if p == "/api/frontier":
            return self.answer(lambda: asked("frontier", q["record"], date=q["date"], layer=layer))
        if p == "/api/verify":
            return self.answer(lambda: asked("verify", q["record"], schema=engine.path_for_engine(SCHEMA)))
        if p == "/api/tuning":
            extra = {k: int(q[k]) for k in ("from", "to") if k in q}
            return self.answer(lambda: asked("tuning", None, system=q["system"], first_hz=float(q["first_hz"]), **extra))
        if p == "/api/content":
            return self.answer(lambda: content(q["record"], q["object"], q["activity"]))
        if p == "/api/holdings":
            return self.answer(lambda: {"holdings": [h for h in holdings() if not q.get("object") or q["object"] in h.get("objects", [])]})
        if p == "/api/log":
            f = ARCHIVE / "log.jsonl"
            return self.answer(lambda: {"log": [json.loads(l) for l in f.read_text(encoding="utf-8").splitlines()[-400:]] if f.exists() else []})
        if p == "/api/export":
            return self.answer(lambda: self.reply_bytes(export(q["record"]), "application/zip", f"{q['record']}-bag.zip"))
        if p.startswith("/api/holding/"):
            def served():
                sha = p.rsplit("/", 1)[1]
                row = next((r for r in holdings() if r["sha256"] == sha), None)
                if row is None:
                    raise ValueError("the archive holds nothing under that hash")
                if row["access"] == "dark" or (row["access"] == "restricted" and PUBLIC["on"]):
                    return self.reply({"message": f"this holding is {row['access']}: it is described and not shown"}, 403)
                self.reply_bytes(holding_path(sha).read_bytes(), row["type"])
            return self.answer(served)
        self.reply({"message": "no such thing is asked here"}, 404)

    def do_POST(self) -> None:
        url = urllib.parse.urlparse(self.path)
        q = dict(urllib.parse.parse_qsl(url.query))
        p = url.path
        if p in ("/api/gather", "/api/map"):  # these compute and change nothing
            def computed():
                body = json.loads(self.body().decode("utf-8") or "{}")
                if p == "/api/map":
                    return asked("map", None, text=str(body.get("text", "")))
                return asked("gather", body["record"], classes=list(body["classes"]), layer=body.get("layer", "established"))
            return self.answer(computed)
        refusal = write_refusal(self)
        if refusal:
            return self.reply({"message": refusal}, 403)
        if p == "/setup/run":
            return self.answer(lambda: recipe_job().start(None, again=q.get("again") == "1"))
        if p == "/api/holdings":
            return self.answer(lambda: ingest(self.body(), safe(q.get("name", "")), [o for o in q.get("objects", "").split(",") if o], q.get("rights", ""),
                                              q.get("access", "restricted"), q.get("note", "")))
        if p == "/api/fixity":
            return self.answer(fixity)
        def changed():
            body = json.loads(self.body().decode("utf-8") or "{}")
            if p == "/api/assert":
                return add_assertion(body)
            if p == "/api/source":
                return add_source(body)
            if p == "/api/holding":  # a holding described again: its objects, rights, access; or withdrawn from the lists (its bytes are kept)
                row = next((r for r in holdings() if r["sha256"] == body.get("sha256")), None)
                if row is None:
                    raise ValueError("the archive holds nothing under that hash")
                change = {k: body[k] for k in ("objects", "rights", "access", "note", "withdrawn") if k in body}
                if change.get("access", "public") not in ACCESS:
                    raise ValueError("access is public, restricted or dark")
                describe({"sha256": row["sha256"], **change})
                log("described", sha256=row["sha256"], **change)
                return {"holding": {**row, **change}}
            raise ValueError("no such thing is done here")
        self.answer(changed)

    def do_PUT(self) -> None:
        url = urllib.parse.urlparse(self.path)
        q = dict(urllib.parse.parse_qsl(url.query))
        refusal = write_refusal(self)
        if refusal:
            return self.reply({"message": refusal}, 403)
        if url.path != "/api/content":
            return self.reply({"message": "no such thing is kept here"}, 404)
        def kept():
            doc = json.loads(self.body().decode("utf-8"))
            if not isinstance(doc, dict) or doc.get("kind") not in ("document", "program"):
                raise ValueError("what is written for an activity is a document or a program")
            f = content_file(q["record"], q["object"], q["activity"])
            f.parent.mkdir(parents=True, exist_ok=True)
            doc["kept"] = now()
            f.write_text(json.dumps(doc, ensure_ascii=False, indent=1), encoding="utf-8")
            log("content kept", record=q["record"], object=q["object"], activity=q["activity"], kind=doc["kind"])
            return {"kept": doc["kept"]}
        self.answer(kept)

    def end_headers(self) -> None:
        self.send_header("Cache-Control", "no-cache")
        super().end_headers()

    def log_message(self, format: str, *args) -> None:
        if args and str(args[1]).startswith(("4", "5")):
            super().log_message(format, *args)


class Server(http.server.ThreadingHTTPServer):
    request_queue_size = 64
    daemon_threads = True


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="Serve Histos Harmonia locally.")
    parser.add_argument("--host", default="127.0.0.1", help="the address to bind; 0.0.0.0 for every interface (then nothing is changed)")
    parser.add_argument("--port", type=int, default=9130)
    parser.add_argument("--open", action="store_true", help="open the page in the default browser")
    parser.add_argument("--archive", help="the archive's folder (default: data/cache/histos-harmonia/archive)")
    parser.add_argument("--library", help="the folder of the records read (default: domains/history/records)")
    args = parser.parse_args(argv)
    global ARCHIVE, LIBRARY
    if args.archive:
        ARCHIVE = Path(args.archive).resolve()
    if args.library:
        LIBRARY = Path(args.library).resolve()
    PUBLIC["on"] = args.host not in ("127.0.0.1", "localhost", "::1")
    if not engine.ENGINE.status()["current"]:
        print("The engine is not built from the present sources. Build it from the page, or run:\n  python applications/histos-harmonia/engine.py build", file=sys.stderr)
    server = Server((args.host, args.port), Handler)
    url = f"http://127.0.0.1:{args.port}/"
    print(f"Histos Harmonia at {url}{' and on ' + args.host if PUBLIC['on'] else ''}  (Ctrl+C to stop)", flush=True)
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
