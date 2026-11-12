"""Serve the Category Studio on this computer.

The page is served from this directory, and the code it shares from the repository's ``domains/``, ``foundations/`` and
``visualization/`` under the same paths, with the data cache at ``/data/``. A script is carried out by the engine
(``engine.py``: kategoria's script language behind a program), which reads no file and keeps nothing between requests; the
server keeps one alive and gives it a request a line.

    GET    /api/status                 whether the engine is built and current, and the limits of a request
    GET    /api/examples               the example scripts: [{id, title, about, text}]
    GET    /api/grammar                the grammar of a statement, in BNF, as the engine reads it
    POST   /api/run                    {"script"}: its outcomes, statement by statement
    GET    /api/scripts                one's own scripts kept on this computer: [{name, modified}]
    GET    /api/scripts/<name>         one of them: {name, text}
    PUT    /api/scripts/<name>         {"text"}: keep it (under data/cache/category-studio/scripts/)
    DELETE /api/scripts/<name>         put it away (it is renamed, not destroyed)

The engine is built by the application's recipe (``recipe.json``), which the page may run:

    GET  /setup                        its one step, and whether it is done
    POST /setup/run[?again=1]          build the engine (again)

Only 127.0.0.1 is bound unless ``--host`` says otherwise. On a public address nothing is built or kept without the server's
token (``--token``), and scripts are held within smaller limits.

Usage::

    python applications/category-studio/serve.py [--host 127.0.0.1] [--port 9070] [--open] [--scripts <a directory>]

then open http://127.0.0.1:9070/ in a browser.
"""

from __future__ import annotations

import argparse
import http.server
import json
import re
import subprocess
import sys
import threading
import time
import urllib.parse
import webbrowser
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
KEPT = ROOT / "data" / "cache" / "category-studio" / "scripts"
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
}
NAME = re.compile(r"^[A-Za-z0-9][A-Za-z0-9 _\-]{0,59}$")

sys.path.insert(0, str(APP))
import engine  # noqa: E402

RECIPE = None
WRITES = {"allowed": True, "token": None}
# The limits of a request: the bytes of a script, the arrows a statement may build, the seconds a script may take.
LIMITS = {"bytes": 200_000, "arrows": 3000, "seconds": 60.0}


class Engine:
    """The engine kept alive: a request a line, one at a time. A script that outlasts its time ends the process, and the next
    request starts another."""

    def __init__(self) -> None:
        self.lock = threading.Lock()
        self.process: subprocess.Popen | None = None
        self.stamp = None

    def start(self) -> None:
        build = engine.read_build()
        if build is None or not engine.PROGRAM.exists():
            raise RuntimeError("the engine is not built")
        kind = build.get("toolchain", "native")
        self.process = subprocess.Popen(engine.command(kind, [engine.path_for(kind, engine.PROGRAM), "--serve"]), stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                        stderr=subprocess.DEVNULL, text=True, encoding="utf-8", errors="replace")
        self.stamp = build.get("built_at")

    def stop(self) -> None:
        if self.process is not None:
            try:
                self.process.kill()
            except OSError:
                pass
            self.process = None

    def ask(self, request: dict, seconds: float) -> dict:
        with self.lock:
            build = engine.read_build()
            if self.process is None or self.process.poll() is not None or (build or {}).get("built_at") != self.stamp:
                self.stop()
                self.start()
            process = self.process
            answer: list[str] = []

            def read() -> None:
                try:
                    answer.append(process.stdout.readline())
                except (OSError, ValueError):
                    pass

            try:
                process.stdin.write(json.dumps(request, ensure_ascii=False) + "\n")
                process.stdin.flush()
            except (OSError, ValueError):
                self.stop()
                raise RuntimeError("the engine stopped; ask again")
            reader = threading.Thread(target=read, daemon=True)
            reader.start()
            reader.join(seconds)
            if reader.is_alive() or not answer or not answer[0].strip():
                self.stop()
                raise TimeoutError(f"the script took more than {seconds:g} seconds and was stopped: a search in it is too large"
                                   if reader.is_alive() else "the engine stopped while carrying out the script")
            reply = json.loads(answer[0])
            if not reply.get("ok"):
                raise RuntimeError(reply.get("error", "the engine refused"))
            return reply["result"]


ENGINE = Engine()


def examples() -> list[dict]:
    """The example scripts: a file's first comment line is its title, its second what it is about."""
    out = []
    for f in sorted((APP / "examples").glob("*.cat")):
        text = f.read_text(encoding="utf-8")
        lines = [line[1:].strip() for line in text.splitlines()[:2] if line.startswith("#")]
        out.append({"id": f.stem, "title": lines[0] if lines else f.stem, "about": lines[1] if len(lines) > 1 else "", "text": text})
    return out


def kept_file(name: str) -> Path:
    if not NAME.match(name):
        raise ValueError("a script's name is letters, digits, spaces, _ and -, at most 60")
    return KEPT / f"{name}.cat"


def write_refusal(handler) -> str | None:
    if not WRITES["allowed"]:
        return "this server keeps and builds nothing: start it with --token to allow it with a token"
    if WRITES["token"] and handler.headers.get("Authorization", "") != f"Bearer {WRITES['token']}":
        return "keeping and building on this server need its token"
    origin = handler.headers.get("Origin")
    if origin is not None and urllib.parse.urlparse(origin).netloc != handler.headers.get("Host", ""):
        return "this server keeps only what its own page sends"
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

    def json_body(self, limit: int) -> dict | None:
        length = int(self.headers.get("Content-Length", "0") or 0)
        if length > limit:
            if length <= 16 * 1024 * 1024:
                self.rfile.read(length)  # taken and dropped, so that the sender hears the refusal
            self.reply_json({"message": f"the request is longer than this server takes ({LIMITS['bytes']:,} bytes of script)"}, 413)
            return None
        try:
            body = json.loads(self.rfile.read(length).decode("utf-8") or "{}")
            if not isinstance(body, dict):
                raise ValueError("a request is a JSON object")
            return body
        except ValueError as error:
            self.reply_json({"message": f"not a request: {error}"}, 400)
            return None

    def script_name(self, url) -> str | None:
        return urllib.parse.unquote(url.path[len("/api/scripts/"):]) if url.path.startswith("/api/scripts/") else None

    def do_GET(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path == "/api/status":
            return self.reply_json({**engine.status(), "limits": LIMITS, "keeps": write_refusal(self) is None}, 200)
        if url.path == "/api/examples":
            return self.reply_json(examples(), 200)
        if url.path == "/api/grammar":
            return self.engine_request({"op": "grammar"})
        if url.path == "/api/scripts":
            KEPT.mkdir(parents=True, exist_ok=True)
            return self.reply_json([{"name": f.stem, "modified": time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(f.stat().st_mtime))}
                                    for f in sorted(KEPT.glob("*.cat"), key=lambda f: -f.stat().st_mtime)], 200)
        name = self.script_name(url)
        if name is not None:
            try:
                return self.reply_json({"name": name, "text": kept_file(name).read_text(encoding="utf-8")}, 200)
            except (OSError, ValueError) as error:
                return self.reply_json({"message": f"no script of that name: {error}"}, 404)
        if url.path == "/setup":
            return self.reply_json(*self.recipe_request(url))
        return super().do_GET()

    def do_POST(self) -> None:
        url = urllib.parse.urlparse(self.path)
        if url.path == "/setup/run":
            return self.reply_json(*self.recipe_request(url))
        if url.path == "/api/run":
            body = self.json_body(LIMITS["bytes"] + 1024)
            if body is None:
                return None
            script = body.get("script")
            if not isinstance(script, str):
                return self.reply_json({"message": "a request holds a script, as text"}, 400)
            if len(script.encode("utf-8")) > LIMITS["bytes"]:
                return self.reply_json({"message": f"the script is longer than this server takes ({LIMITS['bytes']:,} bytes)"}, 413)
            return self.engine_request({"op": "run", "script": script, "most": LIMITS["arrows"]})
        self.send_error(404)

    def do_PUT(self) -> None:
        url = urllib.parse.urlparse(self.path)
        name = self.script_name(url)
        if name is None:
            return self.send_error(404)
        refusal = write_refusal(self)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        body = self.json_body(LIMITS["bytes"] + 1024)
        if body is None:
            return None
        try:
            KEPT.mkdir(parents=True, exist_ok=True)
            kept_file(name).write_text(str(body.get("text", "")), encoding="utf-8")
        except (OSError, ValueError) as error:
            return self.reply_json({"message": str(error)}, 400)
        self.reply_json({"name": name}, 200)

    def do_DELETE(self) -> None:
        url = urllib.parse.urlparse(self.path)
        name = self.script_name(url)
        if name is None:
            return self.send_error(404)
        refusal = write_refusal(self)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        try:
            f = kept_file(name)
            away = KEPT / "put-away"
            away.mkdir(parents=True, exist_ok=True)
            f.replace(away / f"{name} {time.strftime('%Y%m%d-%H%M%S')}.cat")
        except (OSError, ValueError) as error:
            return self.reply_json({"message": str(error)}, 400)
        self.reply_json({"name": name}, 200)

    def engine_request(self, request: dict) -> None:
        state = engine.status()
        if not state["built"]:
            return self.reply_json({"message": "the engine is not built: build it first", "engine": state}, 503)
        try:
            self.reply_json(ENGINE.ask(request, LIMITS["seconds"]), 200)
        except TimeoutError as error:
            self.reply_json({"message": str(error)}, 408)
        except (RuntimeError, ValueError) as error:
            self.reply_json({"message": str(error)}, 500)

    def recipe_request(self, url) -> tuple[dict, int]:
        refusal = write_refusal(self)
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

    def reply_json(self, body, code: int) -> None:
        data = json.dumps(body, ensure_ascii=False).encode("utf-8")
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
    parser = argparse.ArgumentParser(description="Serve the Category Studio locally.")
    parser.add_argument("--host", default="127.0.0.1", help="the address to bind; 0.0.0.0 for every interface")
    parser.add_argument("--port", type=int, default=9070)
    parser.add_argument("--open", action="store_true", help="open the page in the default browser")
    parser.add_argument("--token", help="on a public address, the token that allows building the engine and keeping scripts")
    parser.add_argument("--scripts", help="the directory in which scripts are kept (default: data/cache/category-studio/scripts)")
    args = parser.parse_args(argv)
    if args.scripts:
        global KEPT
        KEPT = Path(args.scripts).resolve()
    public = args.host not in ("127.0.0.1", "localhost", "::1")
    if public:
        WRITES.update(allowed=bool(args.token), token=args.token)
        LIMITS.update(bytes=40_000, arrows=600, seconds=15.0)
    if not engine.status()["current"]:
        print("The engine is not built from the present sources. Build it from the page, or run:\n"
              "  python applications/category-studio/engine.py build", file=sys.stderr)
    server = Server((args.host, args.port), Handler)
    url = f"http://127.0.0.1:{args.port}/"
    where = url if not public else f"{url} and on {args.host}"
    print(f"Category Studio at {where}  (Ctrl+C to stop)", flush=True)
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        ENGINE.stop()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
