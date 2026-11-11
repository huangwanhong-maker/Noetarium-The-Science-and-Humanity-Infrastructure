"""Serve Tabula on this computer.

The page is served from this directory, and the code it shares from the repository's ``domains/``,
``foundations/`` and ``visualization/`` under the same paths, with the data cache at ``/data/``.
The boards a person makes are theirs: they are kept under ``data/cache/tabula/`` (never committed),
each a JSON file of the schema ``noetarium.board/1``, and the images laid on them beside, by the
hash of their content.

    GET    /api/boards              the boards kept, the latest first: id, title, modified, items
    GET    /api/boards/<id>         a board
    PUT    /api/boards/<id>         keep a board (its JSON as the body)
    DELETE /api/boards/<id>         put a board aside, in ``trash/`` (nothing is destroyed)
    POST   /api/files               keep an image (its bytes as the body, its type as Content-Type): {"src"}
    POST   /api/files?name=x.csv    keep a file of data (csv, tsv, txt, json, geojson) for a module to read: {"ref": "kept:<name>"}
    GET    /files/<name>            an image kept
    GET    /api/plugins             the plug-ins of this work: id and module
    GET    /api/modules             the modules of the plug-ins (plugins/<name>/modules/*.py), each described: its parameters
    POST   /api/run                 {"module": "<plug-in>/<file>" | "code": "...", "params", "board"}: run a module: {"job"}
    GET    /api/job/<id>            how a run stands: its progress, and its results when it is done
    POST   /api/job/<id>/cancel     stop a run
    GET | POST /api/plugin/<name>/<path>   the server side a plug-in brings (plugins/<name>/server.py), for this work's own page

A plug-in may bring a server side: ``plugins/<name>/server.py`` with ``handle(method, path, body, context)``
answering ``(a JSON object, a status)``. It is asked only by this work's own page at this computer's own
address, as a module is run. ``context`` gives it ``settings``, a folder for what the person sets up: it is
theirs alone, outside the repository and outside everything this server or another work's serves (a key is
kept there, never under ``data/cache/``, which every work's server mounts); and ``store`` and ``root``.

A module is a script in Python on the board's contract (api/python/science/board.py): it is given the
board's tables, quantities and notes and answers tables, notes and figures, as a process of its own
that is stopped after ten minutes. A module runs with the authority of whoever started the server, so
only the server's own page, at this computer's own address, may ask for a run; a board opened from
a file never runs anything by itself.

Only 127.0.0.1 is bound unless ``--host`` says otherwise. On a public address, keeping, putting
aside and running need the server's token (``--token``).

Usage::

    python applications/tabula/serve.py [--host 127.0.0.1] [--port 9010] [--open] [--store DIR] [--python EXE] [--settings DIR]

``--python`` names the Python that runs the modules (one with scikit-learn or torch, say); by default
the server's own. ``--settings`` names the folder of the person's settings; by default
``%APPDATA%\noetarium\tabula`` on Windows, ``$XDG_CONFIG_HOME/noetarium/tabula`` (``~/.config``) elsewhere.

then open http://127.0.0.1:9010/ in a browser.
"""

from __future__ import annotations

import argparse
import hashlib
import http.server
import importlib.util
import json
import os
import re
import sys
import threading
import time
import urllib.parse
import uuid
import webbrowser
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
MOUNTS = {
    "/domains/": ROOT / "domains",
    "/foundations/": ROOT / "foundations",
    "/visualization/": ROOT / "visualization",
    "/data/": ROOT / "data" / "cache",
}
TYPES = {
    ".html": "text/html; charset=utf-8",
    ".js": "text/javascript; charset=utf-8",
    ".mjs": "text/javascript; charset=utf-8",
    ".css": "text/css; charset=utf-8",
    ".json": "application/json",
    ".svg": "image/svg+xml",
    ".png": "image/png",
    ".jpg": "image/jpeg",
    ".gif": "image/gif",
    ".webp": "image/webp",
}
SCHEMA = "noetarium.board/1"
# Images are kept only in formats that carry no script.
IMAGES = {"image/png": ".png", "image/jpeg": ".jpg", "image/gif": ".gif", "image/webp": ".webp"}
ID = re.compile(r"^[A-Za-z0-9_-]{1,64}$")
FILE = re.compile(r"^[0-9a-f]{32}\.(png|jpg|gif|webp)$")
DATA = (".csv", ".tsv", ".txt", ".json", ".geojson")
MODULE = re.compile(r"^([a-z0-9][a-z0-9-]{0,47})/([A-Za-z0-9][A-Za-z0-9_-]{0,63}\.py)$")
PLUGIN = re.compile(r"^/api/plugin/([a-z0-9][a-z0-9-]{0,47})/([a-z0-9][a-z0-9/_-]{0,95})$")
LIMITS = {"board": 24 * 1024 * 1024, "image": 20 * 1024 * 1024, "data": 256 * 1024 * 1024, "run": 192 * 1024 * 1024, "plugin": 48 * 1024 * 1024}
RUN: dict = {"seconds": 600.0, "jobs": {}, "hosts": {}, "python": None}


def own_settings() -> Path:
    """The folder of the person's settings on this computer: theirs alone, and outside every tree a server serves."""
    base = os.environ.get("APPDATA") if os.name == "nt" else os.environ.get("XDG_CONFIG_HOME")
    return (Path(base) if base else Path.home() / ".config") / "noetarium" / "tabula"


STORE = {"dir": ROOT / "data" / "cache" / "tabula", "settings": own_settings()}
WRITES = {"allowed": True, "token": None}
SERVERS: dict = {"loaded": {}, "lock": threading.Lock()}


def boards_dir() -> Path:
    return STORE["dir"] / "boards"


def files_dir() -> Path:
    return STORE["dir"] / "files"


def list_boards() -> list[dict]:
    out = []
    if boards_dir().is_dir():
        for path in boards_dir().glob("*.json"):
            try:
                board = json.loads(path.read_text(encoding="utf-8"))
            except (OSError, ValueError):
                continue
            if not isinstance(board, dict) or board.get("schema") != SCHEMA:
                continue
            out.append({"id": path.stem, "title": str(board.get("title", "")), "items": len(board.get("items", {})),
                        "modified": (board.get("meta") or {}).get("modified") or time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(path.stat().st_mtime))})
    return sorted(out, key=lambda b: b["modified"], reverse=True)


def keep_board(board_id: str, body: bytes) -> tuple[dict, int]:
    try:
        board = json.loads(body.decode("utf-8"))
    except (UnicodeDecodeError, ValueError) as error:
        return {"message": f"not a board: {error}"}, 400
    if not isinstance(board, dict) or board.get("schema") != SCHEMA:
        return {"message": f"not a board of the schema {SCHEMA}"}, 400
    if not all(isinstance(board.get(k), t) for k, t in (("items", dict), ("place", dict), ("order", list))):
        return {"message": "a board has items, places and an order"}, 400
    if board.get("id") != board_id:
        return {"message": f"the board says it is '{board.get('id')}', and is kept as '{board_id}'"}, 400
    boards_dir().mkdir(parents=True, exist_ok=True)
    target = boards_dir() / f"{board_id}.json"
    partial = target.with_suffix(".json.part")
    partial.write_text(json.dumps(board, ensure_ascii=False), encoding="utf-8")
    os.replace(partial, target)
    return {"kept": board_id, "items": len(board["items"])}, 200


def set_aside(board_id: str) -> tuple[dict, int]:
    source = boards_dir() / f"{board_id}.json"
    if not source.is_file():
        return {"message": f"there is no board '{board_id}'"}, 404
    trash = STORE["dir"] / "trash"
    trash.mkdir(parents=True, exist_ok=True)
    os.replace(source, trash / f"{board_id}-{time.strftime('%Y%m%dT%H%M%S', time.gmtime())}.json")
    return {"aside": board_id}, 200


def keep_image(kind: str, body: bytes) -> tuple[dict, int]:
    ext = IMAGES.get(kind.split(";")[0].strip().lower())
    if not ext:
        return {"message": f"an image is kept as PNG, JPEG, GIF or WebP, not as '{kind}'"}, 415
    name = hashlib.sha256(body).hexdigest()[:32] + ext
    files_dir().mkdir(parents=True, exist_ok=True)
    target = files_dir() / name
    if not target.exists():
        target.write_bytes(body)
    return {"src": f"/files/{name}"}, 200


def keep_data(name: str, body: bytes) -> tuple[dict, int]:
    ext = Path(name).suffix.lower()
    if ext not in DATA:
        return {"message": f"a file of data is kept as one of {', '.join(DATA)}, not as '{ext or name}'"}, 415
    kept = hashlib.sha256(body).hexdigest()[:32] + ext
    files_dir().mkdir(parents=True, exist_ok=True)
    target = files_dir() / kept
    if not target.exists():
        target.write_bytes(body)
    return {"ref": f"kept:{kept}", "name": Path(name).name}, 200


# ---- modules: scripts on the board's contract, each run a process of its own -------------------------


def host_of(plugin: str | None):
    """The host of a plug-in's modules, or (None) of the scripts written on boards themselves."""
    key = plugin or "#scripts"
    if key not in RUN["hosts"]:
        sys.path.insert(0, str(ROOT / "api" / "python"))
        from science import board as contract
        directory = APP / "plugins" / plugin / "modules" if plugin else STORE["dir"] / "scripts"
        RUN["hosts"][key] = contract.Host(directory, STORE["dir"] / "runs", python=RUN["python"])
    return RUN["hosts"][key]


def modules() -> list[dict]:
    out = []
    for directory in sorted((APP / "plugins").glob("*/modules")):
        plugin = directory.parent.name
        try:
            host = host_of(plugin)
        except Exception as error:  # no numpy for the contract, say: the page is told, the server lives
            return [{"ref": f"{plugin}/", "plugin": plugin, "title": plugin, "error": f"modules cannot be run here: {error}"}]
        for listed in host.files():
            name = listed["file"].split("/", 1)[1]
            try:
                d = host.describe(name)
                out.append({"ref": f"{plugin}/{name}", "plugin": plugin, "id": d.get("id"), "title": d.get("title"), "description": d.get("description", ""), "params": d.get("params", [])})
            except Exception as error:
                out.append({"ref": f"{plugin}/{name}", "plugin": plugin, "title": name, "error": str(error)[-600:]})
    return out


def run_job(job: dict, host, file: str, board: dict, params: dict) -> None:
    def progress(fraction: float, text: str) -> None:
        job["progress"], job["text"] = fraction, text
    try:
        job["results"] = host.run(file, board, params, progress=progress, cancel=job["cancel"], seconds=RUN["seconds"])
        job["state"], job["progress"] = "done", 1.0
    except Exception as error:  # a module's failure is told to the page, never raised in the server
        job["state"] = "cancelled" if job["cancel"].is_set() else "failed"
        job["message"] = str(error)
    job["ended"] = time.monotonic()


def start_run(body: dict) -> tuple[dict, int]:
    module, code = body.get("module"), body.get("code")
    try:
        if module:
            m = MODULE.match(str(module))
            if not m or not (APP / "plugins" / m[1] / "modules" / m[2]).is_file():
                return {"message": f"there is no module '{module}'"}, 404
            host, file = host_of(m[1]), m[2]
        elif isinstance(code, str) and code.strip():
            scripts = STORE["dir"] / "scripts"
            scripts.mkdir(parents=True, exist_ok=True)
            file = "s" + hashlib.sha256(code.encode("utf-8")).hexdigest()[:24] + ".py"
            if not (scripts / file).exists():
                (scripts / file).write_text(code, encoding="utf-8")
            host = host_of(None)
        else:
            return {"message": "a run names a module or carries a script"}, 400
    except Exception as error:
        return {"message": f"modules cannot be run here: {error}"}, 500
    # Runs that ended a while ago and were never asked after are forgotten.
    for ident in [k for k, j in RUN["jobs"].items() if j.get("ended") and time.monotonic() - j["ended"] > 900]:
        RUN["jobs"].pop(ident, None)
    job = {"id": uuid.uuid4().hex[:16], "state": "running", "progress": 0.0, "text": "", "cancel": threading.Event()}
    RUN["jobs"][job["id"]] = job
    board = body.get("board") if isinstance(body.get("board"), dict) else {}
    params = body.get("params") if isinstance(body.get("params"), dict) else {}
    threading.Thread(target=run_job, args=(job, host, file, board, params), daemon=True).start()
    return {"job": job["id"]}, 200


def plugins() -> list[dict]:
    found = []
    for path in sorted((APP / "plugins").glob("*/plugin.js")):
        style = path.parent / "plugin.css"
        found.append({"id": path.parent.name, "module": f"/plugins/{path.parent.name}/plugin.js",
                      **({"style": f"/plugins/{path.parent.name}/plugin.css"} if style.is_file() else {})})
    return found


def write_refusal(handler) -> str | None:
    if not WRITES["allowed"]:
        return "this server keeps nothing: start it with --token to allow keeping with a token"
    token = WRITES["token"]
    if token and handler.headers.get("Authorization", "") != f"Bearer {token}":
        return "keeping on this server needs its token"
    # Another page open in the same browser could otherwise post here.
    origin = handler.headers.get("Origin")
    if origin is not None and urllib.parse.urlparse(origin).netloc != handler.headers.get("Host", ""):
        return "this server keeps only what its own page sends"
    return None


def run_refusal(handler) -> str | None:
    """Why this server would not run a module for this request, or None. A module runs with the authority of whoever started the server:
    only the server's own page, at its own address, may ask."""
    refusal = write_refusal(handler)
    if refusal:
        return refusal
    host = handler.headers.get("Host", "")
    origin = handler.headers.get("Origin")
    if handler.headers.get("X-Tabula") != "run" or "application/json" not in handler.headers.get("Content-Type", "") \
            or origin is None or urllib.parse.urlparse(origin).netloc != host:
        return "a module is run only from this work's own page"
    if not WRITES["token"] and host.rsplit(":", 1)[0] not in ("127.0.0.1", "localhost", "[::1]"):
        return "a module is run only at this computer's own address"
    return None


def plugin_refusal(handler, method: str) -> str | None:
    """Why this server would not ask a plug-in's server side for this request, or None. It acts with the authority of whoever started
    the server, and with what they have set up (a key), so it is asked as a module is run: by this work's own page, at this computer's own
    address. A request to read says so by the page's header, which a page elsewhere cannot send without being asked about."""
    if method != "GET":
        refusal = run_refusal(handler)
        return refusal.replace("a module is run", "a plug-in's server is asked") if refusal else None
    refusal = write_refusal(handler)
    if refusal:
        return refusal
    host = handler.headers.get("Host", "")
    if handler.headers.get("X-Tabula") != "run":
        return "a plug-in's server is asked only from this work's own page"
    if not WRITES["token"] and host.rsplit(":", 1)[0] not in ("127.0.0.1", "localhost", "[::1]"):
        return "a plug-in's server is asked only at this computer's own address"
    return None


def plugin_server(name: str):
    """The server side a plug-in brings, loaded once; None when it brings none."""
    with SERVERS["lock"]:
        if name not in SERVERS["loaded"]:
            path = APP / "plugins" / name / "server.py"
            if not path.is_file():
                return None
            spec = importlib.util.spec_from_file_location(f"tabula_plugin_{name.replace('-', '_')}", path)
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            SERVERS["loaded"][name] = module
        return SERVERS["loaded"][name]


def ask_plugin(name: str, method: str, path: str, body: dict | None) -> tuple[dict, int]:
    try:
        module = plugin_server(name)
    except Exception as error:  # noqa: BLE001 — a plug-in that cannot be read says so; the work goes on
        return {"message": f"the server side of '{name}' could not be read: {error}"}, 500
    if module is None or not callable(getattr(module, "handle", None)):
        return {"message": f"the plug-in '{name}' brings no server side"}, 404
    try:
        answer, code = module.handle(method, path, body, {"settings": STORE["settings"], "store": STORE["dir"], "root": ROOT})
    except Exception as error:  # noqa: BLE001
        print(f"the plug-in '{name}' failed on {method} {path}: {type(error).__name__}: {error}", file=sys.stderr, flush=True)
        return {"message": f"the plug-in '{name}' failed: {type(error).__name__}"}, 500
    return answer, code


class Handler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {**http.server.SimpleHTTPRequestHandler.extensions_map, **TYPES}

    def translate_path(self, path: str) -> str:
        path = urllib.parse.unquote(path.split("?", 1)[0].split("#", 1)[0])
        if path.startswith("/files/"):
            name = path[len("/files/"):]
            return str(files_dir() / name) if FILE.match(name) else str(APP / "__outside__")
        prefix = next((p for p in MOUNTS if path.startswith(p)), None)
        base, rel = (MOUNTS[prefix], path[len(prefix):]) if prefix else (APP, path.lstrip("/"))
        target = (base / rel).resolve()
        if target != base and base not in target.parents:
            return str(base / "__outside__")
        return str(target)

    def board_id(self, path: str) -> str | None:
        name = path[len("/api/boards/"):]
        return name if ID.match(name) else None

    def body(self, limit: int) -> bytes | None:
        length = int(self.headers.get("Content-Length", "0") or 0)
        if length > limit:
            self.reply_json({"message": f"the request is too large: at most {limit // (1024 * 1024)} MB"}, 413)
            return None
        return self.rfile.read(length)

    def do_GET(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path == "/api/boards":
            return self.reply_json({"boards": list_boards(), "writable": write_refusal(self) is None}, 200)
        if url.path == "/api/plugins":
            return self.reply_json({"plugins": plugins()}, 200)
        if url.path == "/api/modules":
            return self.reply_json({"modules": modules(), "runs": write_refusal(self) is None}, 200)
        if url.path.startswith("/api/plugin/"):
            return self.plugin(url.path, "GET")
        if url.path.startswith("/api/job/"):
            job = RUN["jobs"].get(url.path[len("/api/job/"):])
            if job is None:
                return self.reply_json({"message": "there is no such run"}, 404)
            answer = {k: job[k] for k in ("id", "state", "progress", "text", "results", "message") if k in job}
            if job["state"] != "running":
                RUN["jobs"].pop(job["id"], None)
            return self.reply_json(answer, 200)
        if url.path.startswith("/api/boards/"):
            board_id = self.board_id(url.path)
            path = boards_dir() / f"{board_id}.json" if board_id else None
            if not path or not path.is_file():
                return self.reply_json({"message": "there is no such board"}, 404)
            return self.reply_text(path.read_text(encoding="utf-8"), 200)
        return super().do_GET()

    def do_PUT(self) -> None:
        url = urllib.parse.urlparse(self.path)
        board_id = self.board_id(url.path) if url.path.startswith("/api/boards/") else None
        if not board_id:
            return self.reply_json({"message": "a board is kept at /api/boards/<id>, its id of letters, digits, - and _"}, 404)
        refusal = write_refusal(self)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        body = self.body(LIMITS["board"])
        if body is not None:
            self.reply_json(*keep_board(board_id, body))

    def do_DELETE(self) -> None:
        url = urllib.parse.urlparse(self.path)
        board_id = self.board_id(url.path) if url.path.startswith("/api/boards/") else None
        if not board_id:
            return self.reply_json({"message": "there is no such board"}, 404)
        refusal = write_refusal(self)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        self.reply_json(*set_aside(board_id))

    def plugin(self, path: str, method: str) -> None:
        """A request to the server side of a plug-in."""
        match = PLUGIN.match(path)
        if not match:
            return self.reply_json({"message": "a plug-in's server is asked at /api/plugin/<name>/<path>"}, 404)
        refusal = plugin_refusal(self, method)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        request = None
        if method != "GET":
            body = self.body(LIMITS["plugin"])
            if body is None:
                return None
            try:
                request = json.loads(body.decode("utf-8") or "{}")
                if not isinstance(request, dict):
                    raise ValueError("a plug-in is asked by a JSON object")
            except (UnicodeDecodeError, ValueError) as error:
                return self.reply_json({"message": f"not a request: {error}"}, 400)
        return self.reply_json(*ask_plugin(match.group(1), method, match.group(2), request))

    def do_POST(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path.startswith("/api/plugin/"):
            return self.plugin(url.path, "POST")
        if url.path == "/api/run" or (url.path.startswith("/api/job/") and url.path.endswith("/cancel")):
            refusal = run_refusal(self)
            if refusal:
                return self.reply_json({"message": refusal}, 403)
            body = self.body(LIMITS["run"])
            if body is None:
                return None
            if url.path != "/api/run":
                job = RUN["jobs"].get(url.path[len("/api/job/"):-len("/cancel")])
                if job is None:
                    return self.reply_json({"message": "there is no such run"}, 404)
                job["cancel"].set()
                return self.reply_json({"cancelled": job["id"]}, 200)
            try:
                request = json.loads(body.decode("utf-8") or "{}")
                if not isinstance(request, dict):
                    raise ValueError("a run is asked for by a JSON object")
            except (UnicodeDecodeError, ValueError) as error:
                return self.reply_json({"message": f"not a request: {error}"}, 400)
            return self.reply_json(*start_run(request))
        if url.path != "/api/files":
            return self.send_error(404)
        refusal = write_refusal(self)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        name = dict(urllib.parse.parse_qsl(url.query)).get("name")
        body = self.body(LIMITS["data"] if name else LIMITS["image"])
        if body is not None:
            self.reply_json(*(keep_data(name, body) if name else keep_image(self.headers.get("Content-Type", ""), body)))

    def reply_json(self, body: dict, code: int) -> None:
        self.reply_text(json.dumps(body, ensure_ascii=False), code)

    def reply_text(self, text: str, code: int) -> None:
        data = text.encode("utf-8")
        try:
            self.send_response(code)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)
        except (ConnectionAbortedError, ConnectionResetError, BrokenPipeError):
            pass    # the page gave the request up (a step of the agent stopped): there is no one to answer

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
    parser = argparse.ArgumentParser(description="Serve Tabula locally.")
    parser.add_argument("--host", default="127.0.0.1", help="the address to bind; 0.0.0.0 for every interface")
    parser.add_argument("--port", type=int, default=9010)
    parser.add_argument("--open", action="store_true", help="open the page in the default browser")
    parser.add_argument("--token", help="on a public address, the token that allows keeping boards")
    parser.add_argument("--store", help="where boards and images are kept (default: data/cache/tabula)")
    parser.add_argument("--python", help="the Python that runs the modules (default: this one)")
    parser.add_argument("--settings", help="the folder of the person's settings, where a plug-in keeps a key (default: the user's own configuration folder)")
    args = parser.parse_args(argv)
    if args.store:
        STORE["dir"] = Path(args.store).resolve()
    if args.settings:
        STORE["settings"] = Path(args.settings).resolve()
    RUN["python"] = args.python
    # Where a module finds the files the work keeps (science.board.file, 'kept:<name>').
    os.environ["NOETARIUM_BOARD_FILES"] = str(files_dir())
    public = args.host not in ("127.0.0.1", "localhost", "::1")
    if public:
        WRITES.update(allowed=bool(args.token), token=args.token)
    server = Server((args.host, args.port), Handler)
    url = f"http://127.0.0.1:{args.port}/"
    where = url if not public else f"{url} and on {args.host}"
    print(f"Tabula at {where}  (Ctrl+C to stop)", flush=True)
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
