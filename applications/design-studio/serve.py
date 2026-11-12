"""Serve the Design Studio on this computer.

The page is served from this directory, with the repository's ``domains/``, ``foundations/`` and
``visualization/`` under the same paths and the data cache at ``/data/``. The engine (``engine.py``) is kept
running here and reads the building the page holds:

    GET  /api/status                 whether the engine is built from the present sources
    GET  /api/vocabulary             the uses of rooms the domain knows, the elements a rate may be declared for
    GET  /api/schemes                the schemes of rules that come with the studio (schemes/*.json)
    POST /api/read                   a building (noetarium.building/1), or {building, around}: everything that follows from it
    POST /api/street                 {street, around}: a street's section along its length, its sun, what its buildings may rise to
    GET  /api/projects               the buildings kept here: [{id, name, saved}]
    GET  /api/projects/<id>          one of them
    PUT  /api/projects/<id>          keep one (under data/cache/design-studio/projects/)
    DELETE /api/projects/<id>        forget one
    PUT  /api/models                 a model of one's own for a piece (a binary glTF file): kept by its content's hash, {id}
    GET  /api/models/<id>            that file
    GET  /world/city/<z>/<x>/<y>     any place's buildings and ways (Overture Maps; data/acquisition/world_city.py), kept once asked
    GET  /setup                      the recipe (recipe.json): the engine
    POST /setup/run[?again=1]        run it (again)

Only 127.0.0.1 is bound unless ``--host`` says otherwise; on a public address, building the engine and keeping
projects need ``--token``.

Usage::

    python applications/design-studio/serve.py [--host 127.0.0.1] [--port 9050] [--open] [--store <directory>]

then open http://127.0.0.1:9050/ in a browser.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import hashlib
import http.server
import json
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
    ".md": "text/markdown; charset=utf-8",
    ".svg": "image/svg+xml",
}
KEPT = ROOT / "data" / "cache" / "design-studio" / "projects"
SCHEMES = APP / "schemes"
PROJECT = re.compile(r"^/api/projects/([a-z0-9][a-z0-9-]{0,63})$")
MODEL = re.compile(r"^/api/models/([0-9a-f]{64})$")
LARGEST_MODEL = 48 * 1024 * 1024
WORLD = re.compile(r"^/world/city/(\d+)/(\d+)/(\d+)$")
LARGEST = 4 * 1024 * 1024

sys.path.insert(0, str(APP))
import engine  # noqa: E402

RECIPE = None
WRITES = {"allowed": True, "token": None}
WORKER = engine.Worker()
READERS = concurrent.futures.ThreadPoolExecutor(max_workers=4)
VOCABULARY: dict[str, str] = {}
WORLD_CITY = None


def recipe_job():
    global RECIPE
    if RECIPE is None:
        sys.path.insert(0, str(ROOT / "data" / "acquisition"))
        import recipe
        RECIPE = recipe.Job(APP / "recipe.json", ROOT)
    return RECIPE


def world_city():
    """The data library's reader of any place's buildings and ways; imported when first asked, as it needs numpy."""
    global WORLD_CITY
    if WORLD_CITY is None:
        sys.path.insert(0, str(ROOT / "data"))
        sys.path.insert(0, str(ROOT / "data" / "acquisition"))
        import world_city as module
        WORLD_CITY = module
    return WORLD_CITY


def schemes() -> list[dict]:
    """The schemes of rules that come with the studio, each with its file's name as its id."""
    out = []
    for path in sorted(SCHEMES.glob("*.json")):
        try:
            scheme = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        out.append({"id": path.stem, **scheme})
    return out


def projects() -> list[dict]:
    out = []
    for path in sorted(KEPT.glob("*.json"), key=lambda p: -p.stat().st_mtime):
        try:
            name = json.loads(path.read_text(encoding="utf-8")).get("name") or path.stem
        except ValueError:
            continue
        out.append({"id": path.stem, "name": name, "saved": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(path.stat().st_mtime))})
    return out


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

    # ---- requests ----------------------------------------------------------------------------------------------

    def do_GET(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path == "/api/status":
            return self.reply_json({"engine": engine.status()}, 200)
        if url.path == "/api/vocabulary":
            return self.vocabulary()
        if url.path == "/api/schemes":
            return self.reply_json({"schemes": schemes()}, 200)
        if url.path == "/api/projects":
            return self.reply_json({"projects": projects()}, 200)
        match = PROJECT.match(url.path)
        if match:
            path = KEPT / f"{match.group(1)}.json"
            if not path.is_file():
                return self.reply_json({"message": "no such project"}, 404)
            return self.reply_text(path.read_text(encoding="utf-8"), 200)
        match = MODEL.match(url.path)
        if match:
            path = KEPT / "models" / f"{match.group(1)}.glb"
            if not path.is_file():
                return self.reply_json({"message": "no such model"}, 404)
            data = path.read_bytes()
            self.send_response(200)
            self.send_header("Content-Type", "model/gltf-binary")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)
            return None
        match = WORLD.match(url.path)
        if match:
            return self.world(*(int(g) for g in match.groups()))
        if url.path == "/setup":
            return self.reply_json(*self.recipe_request(url))
        return super().do_GET()

    def do_POST(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path == "/setup/run":
            WORKER.stop()
            VOCABULARY.clear()
            return self.reply_json(*self.recipe_request(url))
        if url.path == "/api/read":
            body = self.body()
            if body is None:
                return
            # A building alone, or {building, around}: around it, the neighbours and the Sun's instants of a day.
            if isinstance(body.get("building"), dict):
                return self.ask({"op": "read", "building": body["building"], "around": body.get("around") or {}})
            return self.ask({"op": "read", "building": body})
        if url.path == "/api/street":
            body = self.body()
            if body is None:
                return
            return self.ask({"op": "street", "street": body.get("street") or {}, "around": body.get("around") or {}})
        self.send_error(404)

    def do_PUT(self) -> None:
        if urllib.parse.urlparse(self.path).path == "/api/models":
            return self.keep_model()
        match = PROJECT.match(urllib.parse.urlparse(self.path).path)
        if not match:
            return self.send_error(404)
        refusal = self.refusal()
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        body = self.body()
        if body is None:
            return
        if body.get("schema") != "noetarium.building/1":
            return self.reply_json({"message": "a project is a building of the schema noetarium.building/1"}, 400)
        KEPT.mkdir(parents=True, exist_ok=True)
        (KEPT / f"{match.group(1)}.json").write_text(json.dumps(body, ensure_ascii=False, indent=1), encoding="utf-8")
        self.reply_json({"id": match.group(1), "saved": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())}, 200)

    def do_DELETE(self) -> None:
        match = PROJECT.match(urllib.parse.urlparse(self.path).path)
        if not match:
            return self.send_error(404)
        refusal = self.refusal()
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        (KEPT / f"{match.group(1)}.json").unlink(missing_ok=True)
        self.reply_json({"id": match.group(1)}, 200)

    # ---- what they do ------------------------------------------------------------------------------------------

    def keep_model(self) -> None:
        """A binary glTF file, kept by the hash of its content; nothing in it is run."""
        refusal = self.refusal()
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        length = int(self.headers.get("Content-Length", "0") or 0)
        if not 12 <= length <= LARGEST_MODEL:
            return self.reply_json({"message": "a model is a binary glTF file of at most 48 MB"}, 413 if length > LARGEST_MODEL else 400)
        data = self.rfile.read(length)
        if data[:4] != b"glTF":
            return self.reply_json({"message": "not a binary glTF file (.glb)"}, 400)
        digest = hashlib.sha256(data).hexdigest()
        folder = KEPT / "models"
        folder.mkdir(parents=True, exist_ok=True)
        (folder / f"{digest}.glb").write_bytes(data)
        self.reply_json({"id": digest}, 200)

    def body(self) -> dict | None:
        length = int(self.headers.get("Content-Length", "0") or 0)
        if length > LARGEST:
            self.reply_json({"message": "the request is too large"}, 413)
            return None
        try:
            body = json.loads(self.rfile.read(length).decode("utf-8") or "{}")
            if not isinstance(body, dict):
                raise ValueError("a request is a JSON object")
        except ValueError as error:
            self.reply_json({"message": f"not a request: {error}"}, 400)
            return None
        return body

    def ask(self, request: dict) -> None:
        if not engine.status()["current"]:
            return self.reply_json({"message": "the engine is not built from the present sources: run the recipe"}, 503)
        try:
            answer = WORKER.ask(request)
        except RuntimeError as error:
            return self.reply_json({"message": str(error)}, 500)
        if not answer.get("ok"):
            return self.reply_json({"message": str(answer.get("error", "the engine failed"))}, 400)
        self.reply_text(json.dumps(answer["result"], ensure_ascii=False, separators=(",", ":")), 200)

    def vocabulary(self) -> None:
        state = engine.status()
        if not state["current"]:
            return self.reply_json({"message": "the engine is not built from the present sources: run the recipe"}, 503)
        key = state["sources"]
        if key not in VOCABULARY:
            try:
                answer = WORKER.ask({"op": "vocabulary"})
            except RuntimeError as error:
                return self.reply_json({"message": str(error)}, 500)
            if not answer.get("ok"):
                return self.reply_json({"message": str(answer.get("error", "the engine failed"))}, 500)
            VOCABULARY.clear()
            VOCABULARY[key] = json.dumps(answer["result"], ensure_ascii=False, separators=(",", ":"))
        self.reply_text(VOCABULARY[key], 200)

    def world(self, z: int, x: int, y: int) -> None:
        if z != 14 or not (0 <= x < 2 ** z and 0 <= y < 2 ** z):
            return self.reply_json({"message": "the surroundings are read at level 14"}, 400)
        try:
            text = READERS.submit(world_city().city_tile, z, x, y).result(timeout=240)
        except Exception as error:  # the archive unreachable, a reader missing: the lot can still be drawn without its surroundings
            return self.reply_json({"message": f"the surroundings could not be read: {error}"}, 503)
        self.reply_text(text, 200)

    def refusal(self) -> str | None:
        if not WRITES["allowed"]:
            return "this server keeps nothing: start it with --token to allow it with a token"
        if WRITES["token"] and self.headers.get("Authorization", "") != f"Bearer {WRITES['token']}":
            return "this server needs its token"
        return None

    def recipe_request(self, url) -> tuple[dict, int]:
        refusal = self.refusal()
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
        self.send_response(code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

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
    parser = argparse.ArgumentParser(description="Serve the Design Studio locally.")
    parser.add_argument("--host", default="127.0.0.1", help="the address to bind; 0.0.0.0 for every interface")
    parser.add_argument("--port", type=int, default=9050)
    parser.add_argument("--open", action="store_true", help="open the page in the default browser")
    parser.add_argument("--token", help="on a public address, the token that allows building the engine and keeping projects")
    parser.add_argument("--store", help="where projects are kept, in place of data/cache/design-studio/projects")
    args = parser.parse_args(argv)
    if args.store:
        global KEPT
        KEPT = Path(args.store).resolve()
    public = args.host not in ("127.0.0.1", "localhost", "::1")
    if public:
        WRITES.update(allowed=bool(args.token), token=args.token)
    if not engine.status()["current"]:
        print("The engine is not built from the present sources. Run the recipe from the page, or:\n"
              "  python data/acquisition/recipe.py applications/design-studio/recipe.json", file=sys.stderr)
    server = Server((args.host, args.port), Handler)
    url = f"http://127.0.0.1:{args.port}/"
    print(f"The Design Studio at {url if not public else url + ' and on ' + args.host}  (Ctrl+C to stop)", flush=True)
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
