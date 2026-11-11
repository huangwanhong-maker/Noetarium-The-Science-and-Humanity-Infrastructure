"""Serve Agora on this computer.

The page is served from this directory, with the repository's ``domains/``, ``foundations/`` and ``visualization/`` under the same
paths and the data cache at ``/data/``. Its engine, C++ on the markets domain and the urban-science domain's access, is built by
``engine.py`` and kept running here, one process, which keeps a city's streets and the last market it computed.

    GET  /api/status          whether the engine is built from the present sources, and which cities the data library holds
    POST /api/<op>            {city, market, …}: one request to the engine (market, offer, enter, sites, size)
    GET  /api/boards/<id>     the board kept for a city's market: findings sent to it, and what was written about them
    PUT  /api/boards/<id>     keep it (noetarium.board/1, as Tabula keeps boards; under data/cache/agora/boards/)
    GET  /setup               the recipe (recipe.json): the engine and the data, step by step, and how far they are
    POST /setup/run           run it

The kernel of a market here is presumed: the answers follow from assumptions the request declares, not from transactions.
Only 127.0.0.1 is bound unless ``--host`` says otherwise; on a public address, building and keeping need ``--token``.

Usage::

    python applications/agora/serve.py [--host 127.0.0.1] [--port 9030] [--open]

then open http://127.0.0.1:9030/ in a browser.
"""

from __future__ import annotations

import argparse
import http.server
import json
import os
import re
import sys
import threading
import time
import urllib.parse
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
}
OPS = ("market", "offer", "enter", "sites", "size")
BOARDS = ROOT / "data" / "cache" / "agora" / "boards"
BOARD_ID = re.compile(r"^[A-Za-z0-9_-]{1,64}$")
SCHEMA = "noetarium.board/1"

sys.path.insert(0, str(APP))
import engine  # noqa: E402

RECIPE = None
WRITES = {"allowed": True, "token": None}
WORKER = engine.Worker()
STATUS: dict = {"at": 0.0, "state": None}


def recipe_job():
    global RECIPE
    if RECIPE is None:
        sys.path.insert(0, str(ROOT / "data" / "acquisition"))
        import recipe
        RECIPE = recipe.Job(APP / "recipe.json", ROOT)
    return RECIPE


def engine_status(fresh: bool = False) -> dict:
    """The engine's status, kept for 30 s: judging it hashes every source and header it includes."""
    now = time.monotonic()
    if fresh or STATUS["state"] is None or now - STATUS["at"] > 30:
        STATUS.update(at=now, state=engine.status())
    return STATUS["state"]


def cities() -> list[dict]:
    try:
        index = json.loads((engine.PRODUCTS / "index.json").read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return []
    return [c for c in index.get("markets", []) if (engine.PRODUCTS / c.get("file", "")).is_file()]


def write_refusal(handler) -> str | None:
    if not WRITES["allowed"]:
        return "this server keeps and builds nothing: start it with --token to allow it with a token"
    if WRITES["token"] and handler.headers.get("Authorization", "") != f"Bearer {WRITES['token']}":
        return "this server needs its token to keep or build"
    # Another page open in the same browser could otherwise post here.
    origin = handler.headers.get("Origin")
    if origin is not None and urllib.parse.urlparse(origin).netloc != handler.headers.get("Host", ""):
        return "this server keeps only what its own page sends"
    return None


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

    def board_path(self, path: str) -> Path | None:
        name = path[len("/api/boards/"):]
        return BOARDS / f"{name}.json" if BOARD_ID.match(name) else None

    def do_GET(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path == "/api/status":
            return self.reply_json({"engine": engine_status(fresh=True), "cities": cities()}, 200)
        if url.path == "/setup":
            return self.reply_json(*self.recipe_request(url))
        if url.path.startswith("/api/boards/"):
            path = self.board_path(url.path)
            if not path:
                return self.reply_json({"message": "there is no such board"}, 404)
            # A board not yet kept is no error: the page begins an empty one.
            return self.reply_text(path.read_text(encoding="utf-8") if path.is_file() else "null", 200)
        return super().do_GET()

    def do_PUT(self) -> None:
        url = urllib.parse.urlparse(self.path)
        path = self.board_path(url.path) if url.path.startswith("/api/boards/") else None
        if not path:
            return self.reply_json({"message": "a board is kept at /api/boards/<id>"}, 404)
        refusal = write_refusal(self)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        length = int(self.headers.get("Content-Length", "0") or 0)
        if length > 24 * 1024 * 1024:
            return self.reply_json({"message": "the board is too large"}, 413)
        try:
            board = json.loads(self.rfile.read(length).decode("utf-8"))
            if not isinstance(board, dict) or board.get("schema") != SCHEMA or not all(isinstance(board.get(k), t) for k, t in (("items", dict), ("place", dict), ("order", list))):
                raise ValueError(f"not a board of the schema {SCHEMA}")
        except (UnicodeDecodeError, ValueError) as error:
            return self.reply_json({"message": f"not a board: {error}"}, 400)
        BOARDS.mkdir(parents=True, exist_ok=True)
        partial = path.with_suffix(".json.part")
        partial.write_text(json.dumps(board, ensure_ascii=False), encoding="utf-8")
        os.replace(partial, path)
        self.reply_json({"kept": path.stem, "items": len(board["items"])}, 200)

    def do_POST(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path == "/setup/run":
            WORKER.stop()
            STATUS["state"] = None
            return self.reply_json(*self.recipe_request(url))
        parts = url.path.strip("/").split("/")
        if len(parts) == 2 and parts[0] == "api" and parts[1] in OPS:
            return self.engine_request(parts[1])
        self.send_error(404)

    def engine_request(self, op: str) -> None:
        length = int(self.headers.get("Content-Length", "0") or 0)
        if length > 4 * 1024 * 1024:
            return self.reply_json({"message": "the request is too large"}, 413)
        try:
            body = json.loads(self.rfile.read(length).decode("utf-8") or "{}")
            if not isinstance(body, dict):
                raise ValueError("a request is a JSON object")
        except ValueError as error:
            return self.reply_json({"message": f"not a request: {error}"}, 400)
        state = engine_status()
        if not state["current"]:
            return self.reply_json({"message": "the engine is not built from the present sources: build it first", "engine": state}, 503)
        try:
            answer = WORKER.ask({**body, "op": op})
        except RuntimeError as error:
            return self.reply_json({"message": str(error)}, 500)
        if not answer.get("ok"):
            return self.reply_json({"message": str(answer.get("error", "the engine failed"))}, 400)
        self.reply_text(json.dumps(answer["result"], ensure_ascii=False), 200)

    def recipe_request(self, url) -> tuple[dict, int]:
        refusal = None
        if not WRITES["allowed"]:
            refusal = "this server does not build: start it with --token to allow building with a token"
        elif WRITES["token"] and self.headers.get("Authorization", "") != f"Bearer {WRITES['token']}":
            refusal = "building on this server needs its token"
        policy = {"writable": refusal is None, "refusal": refusal}
        try:
            if url.path == "/setup":
                return {**recipe_job().view(), **policy}, 200
            if refusal:
                return {"message": refusal, **policy}, 403
            query = dict(urllib.parse.parse_qsl(url.query))
            return {**recipe_job().start(None, again=query.get("again") == "1"), **policy}, 200
        except RuntimeError as error:
            return {"message": str(error)}, 409

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
            pass

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
    parser = argparse.ArgumentParser(description="Serve Agora locally.")
    parser.add_argument("--host", default="127.0.0.1", help="the address to bind; 0.0.0.0 for every interface")
    parser.add_argument("--port", type=int, default=9030)
    parser.add_argument("--open", action="store_true", help="open the page in the default browser")
    parser.add_argument("--token", help="on a public address, the token that allows building the engine and keeping boards")
    args = parser.parse_args(argv)
    public = args.host not in ("127.0.0.1", "localhost", "::1")
    if public:
        WRITES.update(allowed=bool(args.token), token=args.token)
    if not engine.status()["current"]:
        print("The engine is not built from the present sources. Build it from the page, or run:\n"
              "  python data/acquisition/recipe.py applications/agora/recipe.json", file=sys.stderr)
    server = Server((args.host, args.port), Handler)
    url = f"http://127.0.0.1:{args.port}/"
    print(f"Agora at {url if not public else url + ' and on ' + args.host}  (Ctrl+C to stop)", flush=True)
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        WORKER.stop()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
