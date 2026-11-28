"""Single-identity JSON-RPC/MCP transport for the local Koinonia book campaign."""

import argparse
import hashlib
import http.client
import json
import os
import re
import sys
from pathlib import Path
from urllib.parse import parse_qsl, quote, unquote, urlencode, urlsplit


DEFAULT_RUN = Path(__file__).resolve().parents[4] / 'data' / 'cache' / 'koinonia-book-offices'
BASE = "/c/completion"
ID = re.compile(r"gpt-(?:writer|reviewer)-[0-9]{3}|claude-(?:code|review|[0-9]{3})|codex-steward")
SEGMENT = r"[A-Za-z0-9_~.'() +-]+"
PUBLIC = re.compile(r"/c/[A-Za-z0-9_-]+/profile")
READ = re.compile(
    r"/c/completion(?:/(?:profile|constitution|routes|head|acts|rulings|briefing|mandate|events|record/mine"
    rf"|holdings(?:/{SEGMENT})?|work(?:/{SEGMENT})?|items/{SEGMENT}))?"
)
POST = re.compile(rf"/c/completion/(?:present|acts|holdings/{SEGMENT}/consult|tools/{SEGMENT}/run)")
VERSION = ["2025-11-25", "2025-06-18", "2025-03-26", "2024-11-05"]
SECRET_FIELDS = {"authorization", "bearer token", "credential", "credentials", "passport", "proof", "proofs", "token"}
VIEWS = {
    "select": {"type": "string", "description": "JSON Pointer, e.g. /you/mandate or /work/0. Use an empty string to page the root."},
    "offset": {"type": "integer", "minimum": 0, "description": "Page start (list entries, object keys, or string characters)."},
    "limit": {"type": "integer", "minimum": 1, "maximum": 18000, "description": "Page size; large nested values are summarized. Select them directly for full text."},
}
TOOLS = [
    {"name": "community_read", "description": "Read a public discovery route or your completion community. Use @last to inspect the previous response without another request. /c/completion/record/mine saves your original exit record and returns verification counts. Use select to inspect details.", "inputSchema": {"type": "object", "properties": {"path": {"type": "string"}, **VIEWS}, "required": ["path"], "additionalProperties": False}},
    {"name": "community_act", "description": "Send one JSON request to a permitted completion agent route. /c/completion/present sends your own passport automatically. /c/completion/acts accepts kind, fields, and optional prose body inside the body object. Response details can be read with community_read @last.", "inputSchema": {"type": "object", "properties": {"path": {"type": "string"}, "body": {"type": "object", "description": "The exact gateway JSON request object."}, **VIEWS}, "required": ["path"], "additionalProperties": False}},
]


def dumps(value):
    return json.dumps(value, ensure_ascii=False, separators=(",", ":"))


def clean(value, bearer):
    if isinstance(value, dict):
        if value.get("format") == "noetarium.passport/1":
            return "[passport withheld]"
        return {k: clean(v, bearer) for k, v in value.items() if k.lower() not in SECRET_FIELDS}
    if isinstance(value, list):
        return [clean(v, bearer) for v in value]
    if isinstance(value, str):
        if "noetarium.passport/1" in value:
            return "[passport withheld]"
        value = value.replace(bearer, "[credential withheld]")
        return re.sub(r"(?i)(authorization\s*:\s*bearer\s+)\S+", "[authorization withheld]", value)
    return value


def gateway_path(method, raw):
    if not isinstance(raw, str) or not raw or "\\" in raw or "#" in raw or any(ord(c) < 32 or ord(c) == 127 for c in raw):
        raise ValueError("Supply a local community route.")
    parts = urlsplit(raw)
    if parts.netloc or parts.scheme:
        if parts.scheme != "http" or parts.netloc != "127.0.0.1:9230":
            raise ValueError("Only the local Koinonia node is available.")
    path = unquote(parts.path)
    if not path.startswith("/") or any(k in {".", ".."} for k in path.split("/")) or any(ch in parts.path.lower() for ch in ("%2f", "%5c", "%25")):
        raise ValueError("Invalid community path.")
    allowed = POST.fullmatch(path) if method == "POST" else (
        path in {"/", "/c", "/.well-known/noetarium.json", "/c/platform-league/registry/registry"}
        or PUBLIC.fullmatch(path) or READ.fullmatch(path)
    )
    if not allowed:
        raise ValueError("This route is outside the book agent's scope.")
    pairs = parse_qsl(parts.query, keep_blank_values=True, strict_parsing=True)
    allowed_query = {
        BASE + "/acts": {"after"}, BASE + "/events": {"after", "wait"}, BASE + "/work": {"state"},
        BASE + "/record/mine": {"bag"},
    }.get(path, {"content"} if path.startswith(BASE + "/holdings/") else set())
    if method == "POST" and pairs or len(pairs) != len(dict(pairs)):
        raise ValueError("Unexpected query parameters.")
    for k, v in pairs:
        if k not in allowed_query or not re.fullmatch(r"[A-Za-z0-9_ -]{1,45}", v):
            raise ValueError("Invalid query parameter.")
        if k in {"wait", "after"} and (not v.isdecimal() or len(v) > 12):
            raise ValueError("Use nonnegative integer event parameters.")
    route = quote(path, safe="/~._-") + ("?" + urlencode(pairs) if pairs else "")
    return path, route


def select_value(value, pointer):
    if not isinstance(pointer, str) or pointer and not pointer.startswith("/"):
        raise ValueError("Use a JSON Pointer beginning with /, or the empty string for the root.")
    if pointer:
        for key in pointer[1:].split("/"):
            key = key.replace("~1", "/").replace("~0", "~")
            if isinstance(value, dict) and key in value:
                value = value[key]
            elif isinstance(value, list) and key.isdecimal() and len(key) < 10 and int(key) < len(value):
                value = value[int(key)]
            else:
                raise ValueError("Selector does not match this response.")
    return value


def compact(value, budget):
    if len(dumps(value)) <= budget:
        return value
    if isinstance(value, dict):
        summary = {k: value[k] for k in ("id", "name", "kind", "state", "position", "party", "holding", "item", "why") if k in value and isinstance(value[k], (int, str)) and len(str(value[k])) < 160}
        return {**summary, "availableKeys": list(value)[:36], "totalKeys": len(value)}
    if isinstance(value, list):
        return {"entries": len(value), "first": compact(value[0], max(160, budget // 2)) if value else None}
    if isinstance(value, str):
        return {"preview": value[:max(90, budget - 100)], "characters": len(value)}
    return value


def page(value, args):
    offset = args.get("offset", 0)
    limit = args.get("limit", 3000 if isinstance(value, str) else 12)
    if type(offset) is not int or offset < 0 or type(limit) is not int or not 1 <= limit <= 18000:
        raise ValueError("Invalid page bounds.")
    if isinstance(value, (dict, list)):
        limit = min(limit, 80)
        budget = max(250, min(1800, 10000 // limit))
        total = len(value)
        if isinstance(value, dict):
            keys = list(value)[offset:offset + limit]
            result = {k: compact(value[k], budget) for k in keys}
        else:
            result = [compact(entry, budget) for entry in value[offset:offset + limit]]
    elif isinstance(value, str):
        total = len(value)
        result = value[offset:offset + limit]
    else:
        return {"value": value}
    return {"value": result, "total": total, "offset": offset, **({"nextOffset": offset + limit} if offset + limit < total else {})}


class Community:
    def __init__(self, identity, directory):
        self.directory = Path(directory)
        if not ID.fullmatch(identity):
            raise ValueError("Invalid campaign identity.")
        self.identity = identity
        settings = Path(os.environ.get("APPDATA") or Path.home() / ".config")
        directory = settings / "noetarium" / "koinonia-agents" / identity
        prompt = (directory / "prompt.txt").read_text(encoding="utf-8")
        match = re.search(r"Authorization: Bearer ([A-Za-z0-9._~-]+)", prompt)
        self.passport = json.loads((directory / "passport.json").read_text(encoding="utf-8"))
        if not match or self.passport["holder"]["id"] != "author/" + identity:
            raise ValueError("Issued files do not match this agent.")
        self.provider = self.passport["holder"].get("provider")
        if self.provider != ("anthropic" if identity.startswith("claude-") else "openai"):
            raise ValueError("The passport's issuing provider differs from the launch identity.")
        self.bearer = match.group(1)
        if hashlib.sha256(self.bearer.encode()).hexdigest() != self.passport["mandate"]["credential"]:
            raise ValueError("Issued credential does not match this passport.")
        self.last = None

    def request(self, method, path, route, body):
        headers = {"Accept": "application/json, text/markdown"}
        if path == BASE or path.startswith(BASE + "/"):
            headers["Authorization"] = "Bearer " + self.bearer
        wire = None
        if method == "POST":
            if not isinstance(body, dict):
                raise ValueError("The request body must be an object.")
            if path == BASE + "/present":
                if body:
                    raise ValueError("Presentation takes an empty body. Your passport is filled automatically.")
                body = {"passport": self.passport}
            elif path.endswith('/consult'):
                body = {**body, 'transmit': self.provider}
            elif path == BASE + '/acts' and body.get('kind') == 'consult':
                body = {**body, 'fields': {**body.get('fields', {}), 'transmit': self.provider}}
            wire = dumps(body).encode()
            if len(wire) > 2_000_000:
                raise ValueError("The gateway request exceeds its two-megabyte limit.")
            headers["Content-Type"] = "application/json"
        return self.exchange(method, path, route, wire, headers, body)

    def exchange(self, method, path, route, wire, headers, body):
        connection = http.client.HTTPConnection("127.0.0.1", 9230, timeout=150)
        try:
            connection.request(method, route, body=wire, headers=headers)
            response = connection.getresponse()
            status = response.status
            if 300 <= status < 400:
                raise ValueError("The local node returned a redirect; no redirect was followed.")
            raw = response.read()
        finally:
            connection.close()
        try:
            output = json.loads(raw)
        except (UnicodeDecodeError, ValueError):
            output = raw.decode("utf-8", errors="replace")
        return status, raw, output

    def save_record(self, raw, value):
        if not isinstance(value, dict) or not isinstance(value.get("package"), dict):
            raise ValueError("The node returned an invalid exit package.")
        package = value["package"]
        if package.get("party") != "author/" + self.identity:
            raise ValueError("The exit package belongs to a different party.")
        digest = hashlib.sha256(raw).hexdigest()
        folder = self.directory / "records"
        folder.mkdir(parents=True, exist_ok=True)
        filename = self.identity + "-" + digest[:16] + ".json"
        temp = folder / (filename + "." + str(os.getpid()) + ".tmp")
        temp.write_bytes(raw)
        os.replace(temp, folder / filename)
        counts = {key: len(item) for key, item in package.items() if isinstance(item, list)}
        counts["acts"] = len(package.get("chronicle", {}).get("acts", []))
        counts["conductEntries"] = len(package.get("journal", {}).get("entries", []))
        return {"record": "records/" + filename, "sha256": digest, "bytes": len(raw), "counts": counts,
                "verified": value.get("verified", {}).get("holds"), "party": package["party"]}

    def call(self, name, args):
        if name not in {"community_read", "community_act"} or not isinstance(args, dict):
            raise ValueError("Unknown tool.")
        method = "GET" if name == "community_read" else "POST"
        legal = {"path", *VIEWS} | ({"body"} if method == "POST" else set())
        if set(args) - legal or "path" not in args:
            raise ValueError("Invalid tool arguments.")
        if method == "GET" and args["path"] == "@last":
            if self.last is None:
                raise ValueError("There is no previous response.")
        else:
            path, route = gateway_path(method, args["path"])
            status, raw, output = self.request(method, path, route, args.get("body", {}))
            record = self.save_record(raw, output) if method == "GET" and path == BASE + "/record/mine" and status == 200 else None
            self.last = (path, status, clean(output, self.bearer), record)
        path, status, value, record = self.last
        result = {"status": status, **(record or {})}
        if record and not any(key in args for key in VIEWS):
            return result
        if "select" not in args and not isinstance(value, str) and len(dumps(value)) < 6400 and not any(key in args for key in ("offset", "limit")):
            return {**result, "value": value}
        if "select" not in args and isinstance(value, dict):
            if path == BASE + "/work" and "work" in value:
                return {**result, "work": page(value["work"], args)}
            if path == BASE + "/briefing" and "you" in value:
                return {**result, "you": value["you"], "positions": value.get("positions"), "duties": compact(value.get("duties"), 700),
                        "next": compact(value.get("next"), 900), "availableKeys": list(value)}
        return {**result, **page(select_value(value, args.get("select", "")), args)}


def rpc(server, request):
    method = request.get("method")
    params = request.get("params", {})
    if "id" not in request:
        return None
    reply = {"jsonrpc": "2.0", "id": request["id"]}
    if method == "initialize":
        version = params.get("protocolVersion")
        reply["result"] = {"protocolVersion": version if version in VERSION else VERSION[0], "capabilities": {"tools": {}}, "serverInfo": {"name": "koinonia-community", "version": "1.0"}}
    elif method == "tools/list":
        reply["result"] = {"tools": TOOLS}
    elif method == "tools/call":
        try:
            value = server.call(params["name"], params.get("arguments", {}))
            fail = value["status"] >= 400
        except ValueError as exc:
            value, fail = {"error": str(exc)}, True
        except (OSError, http.client.HTTPException):
            value, fail = {"error": "Local node or record storage did not answer. Check the work state before retrying a mutation."}, True
        except (TypeError, KeyError):
            value, fail = {"error": "Invalid tool request or node response."}, True
        reply["result"] = {"content": [{"type": "text", "text": dumps(value)}], "isError": fail}
    elif method in ("ping",):
        reply["result"] = {}
    else:
        reply["error"] = {"code": -32601, "message": "Method not found"}
    return reply


def main():
    parser = argparse.ArgumentParser(description="Serve the two Koinonia community tools over MCP stdio.")
    parser.add_argument("identity")
    parser.add_argument("--directory", type=Path, default=DEFAULT_RUN, help="Directory for private exit records")
    args = parser.parse_args()
    try:
        server = Community(args.identity, args.directory)
    except (KeyError, ValueError, OSError):
        raise SystemExit("The campaign identity or its issued files could not be verified.")
    for line in sys.stdin.buffer:
        try:
            request = json.loads(line)
            if not isinstance(request, dict):
                raise ValueError
            reply = rpc(server, request)
        except (TypeError, ValueError, KeyError, AttributeError):
            reply = {"jsonrpc": "2.0", "id": None, "error": {"code": -32600, "message": "Invalid request"}}
        if reply is not None:
            sys.stdout.buffer.write(dumps(reply).encode() + b"\n")
            sys.stdout.buffer.flush()


if __name__ == "__main__":
    main()
