"""A host for project-based works (ADR 0102): projects as folders, data left as it is with its columns bound to roles, tools that are
modules of the board's contract, plug-ins that bring tools, templates, engines and schemes, boards, and runs as processes of their own.

It is Market Studio's server (ADR 0093) made general: a work configures it with its directory, its name and title, where it keeps its
projects and how its engines stand, and serves its own page beside the repository's ``domains/``, ``foundations/`` and
``visualization/`` and the data cache at ``/data/``. A project is a folder under the work's store (never committed):

    project.json     the project (noetarium.workspace/1): its sources, slices, quantities, steps, views and boards
    data/            files the person added
    scripts/         tools the person wrote
    results/         what each step answered, by the step's id
    boards/          its boards (noetarium.board/1)

    GET  /api/status                         the engines, whether tools can be run here, the plug-ins
    GET  /api/templates                      projects to begin from (templates/*.json and each plug-in's)
    GET  /api/projects                       the projects kept
    POST /api/projects                       {"title", "template"?}: a new project: {"name", "project"}
    GET  /api/projects/<p>                   {"name", "project", "scripts": [file, ...]}
    PUT  /api/projects/<p>                   keep the project (its JSON as the body)
    POST /api/projects/<p>/files?name=x.csv  keep a file of data in the project: {"ref": "project:data/x.csv"}
    POST /api/projects/<p>/read              {"source", "slice"?, "limit"?}: a source read as a table
    GET  /api/projects/<p>/table?name=&slice=&limit=   a table of the project by its name, a source's or a step's, within a slice
    GET  /api/tools?project=<p>              the tools: the work's (tools/<group>/*.py), its plug-ins' and the project's scripts
    POST /api/projects/<p>/run               {"tool", "params", "slice"?, "step"?, "title"?}: run a tool: {"job"}
    GET  /api/job/<id>                       how a run stands; when it is done, the step it made
    GET  /api/projects/<p>/result?step=<id>  the figures a step answered
    POST /api/job/<id>/cancel                stop it
    GET | PUT /api/projects/<p>/scripts/<file>   a tool of the project's own: its text
    GET | PUT /api/projects/<p>/boards/<id>      a board of the project
    POST /api/projects/<p>/export            {"table", "value"?}: a table written as a record in space and time that another work reads
                                             (the Urban Science Workbench's studio): {"file", "url", "events"}
    GET  /setup, POST /setup/run             the work's recipe

A plug-in is a directory under the work's ``plugins/``: ``plugin.json`` (its title, what it is about, its version and colour, the
domains it stands on), ``tools/*.py`` (a group of tools under the plug-in's name), ``templates/*.json`` (projects to begin from) with
``templates/data/`` (small files a template's project is given), ``engine/main.cpp`` (a C++ engine on the API's compositions, built by
the work's recipe through science.engines) and ``schemes/`` (declared files its tools offer: rules, equations, limits). Nothing else of
the work changes when one is added.

A tool runs as a process of its own, stopped after ten minutes, with the authority of whoever started the server: only the work's own
page, at this computer's own address, may ask for a run or keep a script. Only 127.0.0.1 is bound unless ``--host`` says otherwise; on a
public address keeping and running need ``--token``.
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
import unicodedata
import urllib.parse
import uuid
import webbrowser
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
APP = ROOT   # the work's directory, set by configure()
WORK = {"name": "studio", "title": "Studio", "port": 9090, "engines": None}
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
SCHEMA = "noetarium.workspace/1"
BOARD = "noetarium.board/1"
PROJECT = re.compile(r"^[a-z0-9][a-z0-9-]{0,47}$")
ID = re.compile(r"^[A-Za-z0-9_-]{1,64}$")
SCRIPT = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_-]{0,63}\.py$")
TOOL = re.compile(r"^([a-z0-9][a-z0-9-]{0,47})/([A-Za-z0-9][A-Za-z0-9_-]{0,63}\.py)$")
DATA = (".csv", ".tsv", ".txt", ".json", ".geojson")
LIMITS = {"project": 24 * 1024 * 1024, "file": 512 * 1024 * 1024, "run": 4 * 1024 * 1024, "script": 1024 * 1024}
MOST_CELLS_SENT = 3_000_000   # of a table sent to the page: beyond it the table is cut, and says so
STORE = {"dir": ROOT / "data" / "cache" / "studio"}
WRITES = {"allowed": True, "token": None}
RUN: dict = {"seconds": 600.0, "jobs": {}, "hosts": {}, "python": None}
TABLES: dict = {}   # sources read, by their file, its time and their filter
FILES = threading.Lock()   # one request at a time reads or replaces a project's files: on Windows a file being read cannot be replaced
RECIPE = None

from . import workspace as sources  # noqa: E402


def configure(app: Path, name: str, title: str, port: int, engines=None) -> None:
    """The work this host serves: its directory (its page, tools/, templates/, plugins/, recipe.json), its name (its store, data/cache/<name>),
    its title, its port, and `engines`, a function answering how its engines stand ({"built", "current", ...})."""
    global APP
    APP = Path(app).resolve()
    WORK.update(name=name, title=title, port=port, engines=engines)
    STORE["dir"] = ROOT / "data" / "cache" / name


def engine_status() -> dict:
    try:
        return WORK["engines"]() if WORK["engines"] else {"built": True, "current": True, "engines": []}
    except Exception as error:  # the page is told, the server lives
        return {"built": False, "current": False, "error": str(error)}


def projects_dir() -> Path:
    return STORE["dir"] / "projects"


def project_dir(name: str) -> Path:
    if not PROJECT.match(name or ""):
        raise KeyError("a project is named by lower-case letters, digits and -")
    return projects_dir() / name


def read_project(name: str) -> dict:
    path = project_dir(name) / "project.json"
    if not path.is_file():
        raise KeyError(f"there is no project '{name}'")
    with FILES:
        return json.loads(path.read_text(encoding="utf-8"))


def replace_file(target: Path, text: str) -> None:
    """A file written whole or not at all: beside itself first, then put in its place."""
    target.parent.mkdir(parents=True, exist_ok=True)
    partial = target.with_name(f"{target.name}.{uuid.uuid4().hex[:8]}.part")
    with FILES:
        partial.write_text(text, encoding="utf-8")
        for attempt in range(6):
            try:
                os.replace(partial, target)
                return
            except PermissionError:   # a reader outside this server (a scanner, an editor) holds it for a moment
                if attempt == 5:
                    partial.unlink(missing_ok=True)
                    raise
                time.sleep(0.05 * (attempt + 1))


def stamp() -> str:
    return time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())


def slug(title: str) -> str:
    plain = "".join(c for c in unicodedata.normalize("NFKD", title) if not unicodedata.combining(c))   # é as e; what is no letter or digit parts words
    base = re.sub(r"[^a-z0-9]+", "-", plain.lower().replace("'", "").replace("’", "")).strip("-")[:40] or "project"
    name, k = base, 2
    while (projects_dir() / name).exists():
        name, k = f"{base}-{k}", k + 1
    return name


def plugins() -> list[dict]:
    """The plug-ins: each a directory with a plugin.json."""
    out = []
    if (APP / "plugins").is_dir():
        for directory in sorted((APP / "plugins").iterdir()):
            try:
                about = json.loads((directory / "plugin.json").read_text(encoding="utf-8"))
            except (OSError, ValueError):
                continue
            if PROJECT.match(directory.name):
                out.append({"name": directory.name, "title": about.get("title", directory.name), "about": about.get("about", ""), "dir": directory,
                            "version": about.get("version", ""), "colour": about.get("colour", ""), "domains": about.get("domains", []),
                            "tools": len([q for q in (directory / "tools").glob("*.py") if not q.name.startswith("_")]) if (directory / "tools").is_dir() else 0,
                            "templates": len(list((directory / "templates").glob("*.json"))) if (directory / "templates").is_dir() else 0,
                            "engine": (directory / "engine" / "main.cpp").is_file(),
                            "schemes": sorted(q.name for q in (directory / "schemes").glob("*.json")) if (directory / "schemes").is_dir() else []})
    return out


def tool_groups() -> list[tuple[str, Path, str]]:
    """The groups of predefined tools: the work's own (tools/<group>), then each plug-in's (plugins/<name>/tools)."""
    groups = []
    if (APP / "tools").is_dir():
        for d in sorted((APP / "tools").iterdir()):
            if d.is_dir() and not d.name.startswith("_"):
                title = d.name
                try:
                    title = json.loads((d / "group.json").read_text(encoding="utf-8")).get("title", d.name)
                except (OSError, ValueError):
                    pass
                groups.append((d.name, d, title))
    for plug in plugins():
        if (plug["dir"] / "tools").is_dir() and not any(g[0] == plug["name"] for g in groups):
            groups.append((plug["name"], plug["dir"] / "tools", plug["title"]))
    return groups


def template_file(ident: str) -> Path | None:
    """A template by its id: `<name>` of the work's own, `<plug-in>.<name>` of a plug-in's."""
    plug, _, name = str(ident).rpartition(".")
    if not ID.match(name) or (plug and not PROJECT.match(plug)):
        return None
    path = (APP / "plugins" / plug / "templates" / f"{name}.json") if plug else (APP / "templates" / f"{name}.json")
    return path if path.is_file() else None


def templates() -> list[dict]:
    out = []
    files = [(path.stem, path) for path in sorted((APP / "templates").glob("*.json"))]
    for plug in plugins():
        files += [(f"{plug['name']}.{path.stem}", path) for path in sorted((plug["dir"] / "templates").glob("*.json"))]
    for ident, path in files:
        try:
            t = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        out.append({"id": ident, "title": t.get("title", path.stem), "about": t.get("about", ""), "needs": t.get("needs", [])})
    return out


def new_project(body: dict) -> tuple[dict, int]:
    title = str(body.get("title") or "Untitled project").strip()[:120]
    project = {"schema": SCHEMA, "title": title, "meta": {"created": stamp(), "saved": stamp()}, "frame": None,
               "sources": [], "slices": [], "quantities": [], "steps": [], "views": [], "boards": []}
    template = body.get("template")
    given: list[Path] = []
    if template:
        path = template_file(template)
        if path is None:
            return {"message": f"there is no template '{template}'"}, 404
        t = json.loads(path.read_text(encoding="utf-8"))
        # The small files a template gives its project: beside it, under data/, by their plain names.
        for name in t.get("files", []):
            source = path.parent / "data" / Path(str(name)).name
            if not source.is_file() or source.suffix.lower() not in DATA:
                return {"message": f"the template '{template}' names a file it does not bring: {name}"}, 500
            given.append(source)
        project.update({k: t["project"][k] for k in ("frame", "sources", "slices", "quantities", "views", "boards") if k in t["project"]})
        project["plan"] = t["project"].get("plan", [])   # the steps the template proposes, to be run
        if not body.get("title"):
            project["title"] = t.get("title", title)
    name = slug(project["title"])
    folder = projects_dir() / name
    for sub in ("data", "scripts", "results", "boards"):
        (folder / sub).mkdir(parents=True, exist_ok=True)
    for source in given:
        (folder / "data" / source.name).write_bytes(source.read_bytes())
    (folder / "project.json").write_text(json.dumps(project, ensure_ascii=False, indent=1), encoding="utf-8")
    return {"name": name, "project": project}, 200


def list_projects() -> list[dict]:
    out = []
    if projects_dir().is_dir():
        for path in projects_dir().glob("*/project.json"):
            try:
                p = json.loads(path.read_text(encoding="utf-8"))
            except (OSError, ValueError):
                continue
            if p.get("schema") == SCHEMA:
                out.append({"name": path.parent.name, "title": p.get("title", ""), "saved": (p.get("meta") or {}).get("saved", ""),
                            "sources": len(p.get("sources", [])), "steps": len(p.get("steps", []))})
    return sorted(out, key=lambda p: p["saved"], reverse=True)


# ---- tables: a project's sources and what its steps answered ---------------------------------------------------------------


def resolve(project: str, ref: str) -> Path:
    """The file a source names: `project:data/<file>` of the project, `data:<path>` of the data library's cache."""
    kind, _, rest = str(ref).partition(":")
    base = {"project": project_dir(project), "data": ROOT / "data" / "cache"}.get(kind)
    if base is None or not rest:
        raise sources.Unreadable(f"a source names a file as project:data/<file> or data:<path>, not '{ref}'")
    target = (base / rest).resolve()
    if base.resolve() not in target.parents:
        raise sources.Unreadable(f"'{ref}' is outside what a source may name")
    return target


def read_source(project: str, source: dict) -> dict:
    """A source as a table, with its roles: read once for its file's time and its filter."""
    path = resolve(project, source.get("ref", ""))
    key = (str(path), path.stat().st_mtime if path.exists() else 0, json.dumps(source.get("where") or [], sort_keys=True))
    if key not in TABLES:
        if len(TABLES) > 12:
            TABLES.pop(next(iter(TABLES)))
        TABLES[key] = sources.read_file(path, source.get("where"))
    table = dict(TABLES[key])
    names = {c["name"] for c in table["columns"]}
    roles = {**(table.get("roles") or {}), **(source.get("roles") or {})} if source.get("roles") is not None else dict(table.get("roles") or {})
    table["roles"] = {role: column for role, column in roles.items() if column in names}
    table.update(name=source.get("name", "table"), title=source.get("title") or source.get("name", ""), source=source.get("credit") or source.get("ref", ""))
    return table


def find_table(project: str, p: dict, name: str) -> dict:
    """A table of a project by its name: a source's, or the last a step answered."""
    for s in p.get("sources", []):
        if s.get("name") == name:
            return read_source(project, s)
    for step in reversed(p.get("steps", [])):
        if any(o.get("kind") == "table" and o.get("name") == name for o in step.get("outputs", [])):
            path = project_dir(project) / "results" / f"{step['id']}.json"
            if not ID.match(str(step.get("id", ""))) or not path.is_file():
                break
            with FILES:
                kept = json.loads(path.read_text(encoding="utf-8"))
            for r in kept.get("results", []):
                if r.get("kind") == "table" and r.get("name") == name:
                    return r
    raise KeyError(f"the project has no table '{name}'")


def within(table: dict, piece: dict | None) -> dict:
    return sources.take(table, sources.rows_in(table, piece)) if piece else table


def sent(table: dict, limit: int | None) -> dict:
    """A table as the page is sent it: cut where it is too large, saying so."""
    most = min(limit or table["rows"], MOST_CELLS_SENT // max(1, len(table["columns"])))
    if table["rows"] > most:
        table = {**sources.take(table, list(range(most))), "cut": {"rows": table["rows"], "sent": most}}
    return table


# ---- tools: modules of the board's contract --------------------------------------------------------------------------------


def host_of(key: str, directory: Path):
    if key not in RUN["hosts"]:
        from science import board as contract
        RUN["hosts"][key] = contract.Host(directory, STORE["dir"] / "runs", python=RUN["python"])
    return RUN["hosts"][key]


def tool_host(project: str | None, ref: str):
    """The host and the file of a tool: `<group>/<file>.py` predefined, `project/<file>.py` the project's own."""
    m = TOOL.match(str(ref))
    if not m:
        raise KeyError(f"there is no tool '{ref}'")
    if m[1] == "project":
        if not project or not (project_dir(project) / "scripts" / m[2]).is_file():
            raise KeyError(f"the project has no script '{m[2]}'")
        return host_of(f"project:{project}", project_dir(project) / "scripts"), m[2]
    directory = next((d for name, d, _ in tool_groups() if name == m[1]), None)
    if directory is None or m[2].startswith("_") or not (directory / m[2]).is_file():
        raise KeyError(f"there is no tool '{ref}'")
    return host_of(m[1], directory), m[2]


def tools(project: str | None) -> list[dict]:
    out = []
    groups = tool_groups()
    if project and (project_dir(project) / "scripts").is_dir():
        groups.append(("project", project_dir(project) / "scripts", "Your scripts"))
    for group, directory, title in groups:
        try:
            host = host_of(group if group != "project" else f"project:{project}", directory)
        except Exception as error:  # no numpy for the contract, say: the page is told, the server lives
            return [{"ref": f"{group}/", "group": group, "title": group, "error": f"tools cannot be run here: {error}"}]
        for path in sorted(directory.glob("*.py")):
            if path.name.startswith("_"):
                continue
            entry = {"ref": f"{group}/{path.name}", "group": group, "group_title": title}
            try:
                d = host.describe(path.name)
                entry.update(id=d.get("id"), title=d.get("title"), description=d.get("description", ""), params=d.get("params", []))
            except Exception as error:
                entry.update(title=path.name, error=str(error)[-600:])
            out.append(entry)
    return out


def named_apart(results: list, taken: set, suffix: str) -> None:
    """One name belongs to one table: a table answered under a name another table of the project has is named again, after the slice
    it is of (outlets_docklands) and then by number."""
    for r in results:
        if r.get("kind") != "table":
            continue
        base = name = r["name"]
        if name in taken and suffix:
            base = name = f"{r['name']}_{suffix}"[:48]
        k = 2
        while name in taken:
            name, k = f"{base[:44]}_{k}", k + 1
        taken.add(name)
        r["name"] = name


def run_job(job: dict, host, file: str, board: dict, params: dict, folder: Path, step: dict, taken: set, suffix: str) -> None:
    def progress(fraction: float, text: str) -> None:
        job["progress"], job["text"] = fraction, text
    t0 = time.monotonic()
    try:
        results = host.run(file, board, params, progress=progress, cancel=job["cancel"], seconds=RUN["seconds"])
        named_apart(results, taken, suffix)
        step.update(ran=stamp(), seconds=round(time.monotonic() - t0, 1),
                    outputs=[{"kind": r["kind"], **({"name": r["name"], "title": r.get("title", ""), "rows": r.get("rows", 0), "show": r.get("show")} if r["kind"] == "table"
                                                    else {"text": r.get("text", "")} if r["kind"] == "note" else {"title": r.get("title", "")})} for r in results])
        replace_file(folder / "results" / f"{step['id']}.json", json.dumps({"step": step, "results": results}, ensure_ascii=False))
        job["step"], job["state"], job["progress"] = step, "done", 1.0
    except Exception as error:  # a tool's failure is told to the page, never raised in the server
        job["state"] = "cancelled" if job["cancel"].is_set() else "failed"
        # The reason in a line (the last the tool said, without its error's class), and the whole of what it printed beside it:
        # a person running a predefined tool wants the first, one writing a script the second.
        lines = [line for line in str(error).splitlines() if line.strip()]
        job["message"] = re.sub(r"^[A-Za-z_.]*(Error|Exception): ", "", lines[-1]) if lines else "the tool failed"
        job["detail"] = str(error)[-6000:]
    job["ended"] = time.monotonic()


def start_run(project: str, body: dict) -> tuple[dict, int]:
    try:
        p = read_project(project)
        host, file = tool_host(project, body.get("tool"))
        described = host.describe(file)
    except KeyError as error:
        return {"message": str(error.args[0])}, 404
    except Exception as error:
        return {"message": f"tools cannot be run here: {error}"}, 500
    params = body.get("params") if isinstance(body.get("params"), dict) else {}
    piece = next((s for s in p.get("slices", []) if s.get("id") == body.get("slice")), None)
    # The tables the tool's parameters name, within the slice.
    tables = {}
    try:
        for q in described.get("params", []):
            name = params.get(q["name"], q.get("default"))
            if q.get("role") == "table" and name:
                tables[name] = within(find_table(project, p, str(name)), piece)
    except (KeyError, sources.Unreadable) as error:
        return {"message": str(error.args[0])}, 400
    when = sources.interval(piece)
    board = {"tables": tables, "notes": [],
             "quantities": {q["name"]: {"lo": q.get("low", q["mid"]), "mid": q["mid"], "hi": q.get("high", q["mid"]), "unit": q.get("unit", "")} for q in p.get("quantities", [])},
             "view": {"slice": piece.get("name") if piece else None, "region": piece.get("region") if piece else None, "period": piece.get("period", "year") if piece else "year",
                      "time": {"from": when[0] if when and when[0] != float("-inf") else None, "until": when[1] if when and when[1] != float("inf") else None} if when else None}}
    for ident in [k for k, j in RUN["jobs"].items() if j.get("ended") and time.monotonic() - j["ended"] > 900]:
        RUN["jobs"].pop(ident, None)
    step_id = str(body.get("step") or "")
    step = {"id": step_id if ID.match(step_id) else "s" + uuid.uuid4().hex[:10], "tool": body.get("tool"), "title": str(body.get("title") or described.get("title") or file),
            "slice": piece.get("id") if piece else None, "params": params}
    # The names the project's other tables hold: its sources', and what its other steps answered.
    taken = {s.get("name") for s in p.get("sources", [])} | {o.get("name") for other in p.get("steps", []) if other.get("id") != step["id"]
                                                             for o in other.get("outputs", []) if o.get("kind") == "table"}
    suffix = re.sub(r"[^a-z0-9]+", "_", str(piece.get("name", "")).lower()).strip("_") if piece else ""
    job = {"id": uuid.uuid4().hex[:16], "state": "running", "progress": 0.0, "text": "", "cancel": threading.Event()}
    RUN["jobs"][job["id"]] = job
    threading.Thread(target=run_job, args=(job, host, file, board, params, project_dir(project), step, taken, suffix), daemon=True).start()
    return {"job": job["id"]}, 200


# ---- what other works read ---------------------------------------------------------------------------------------------------


def export(project: str, body: dict) -> tuple[dict, int]:
    """A table of the project written as a record in space and time, in visualization/js/gis/spacetime.js's form: its rows with a latitude
    and a longitude as events, at their times where the table binds one (else all at the moment of the export), its numbers as values and
    its names as labels, under data/cache/<work>/exports/, which every work's server serves at /data/. The Urban Science Workbench opens it
    by ?record=<url>."""
    p = read_project(project)
    table = find_table(project, p, str(body.get("table", "")))
    roles = table.get("roles") or {}
    names = [c["name"] for c in table["columns"]]
    lat = roles.get("lat") or next((n for n in names if n.lower() in ("lat", "latitude")), None)
    lon = roles.get("lon") or next((n for n in names if n.lower() in ("lon", "lng", "longitude")), None)
    if not lat or not lon:
        raise ValueError(f"the table '{table.get('name')}' has no latitude and longitude to place it")
    data = table["data"]
    numbers = [c["name"] for c in table["columns"] if c.get("kind") == "number" and c["name"] not in (lat, lon)][:12]
    label = roles.get("name") or next((c["name"] for c in table["columns"] if c.get("kind") == "text"), None)
    time_col = roles.get("time")
    now = time.time() * 1000
    rows = [i for i in range(table["rows"]) if isinstance(data[lat][i], (int, float)) and isinstance(data[lon][i], (int, float))]
    times = []
    for i in rows:
        t0 = sources.instant(data[time_col][i]) if time_col else None
        times.append(t0 * 1000 if t0 is not None else now)
    record = {"kind": "events", "title": f"{table.get('title') or table.get('name')}",
              "note": f"From {WORK['title']}: the project '{p.get('title', project)}', its table '{table.get('name')}'.",
              "time": times, "lat": [data[lat][i] for i in rows], "lon": [data[lon][i] for i in rows],
              "values": {n: [data[n][i] for i in rows] for n in numbers}}
    if label:
        record["labels"] = ["" if data[label][i] is None else str(data[label][i]) for i in rows]
    if body.get("value") in numbers or roles.get("amount") in numbers:
        record["show"] = {"value": body.get("value") or roles.get("amount")}
    folder = ROOT / "data" / "cache" / WORK["name"] / "exports"
    folder.mkdir(parents=True, exist_ok=True)
    file = f"{project}-{re.sub(r'[^A-Za-z0-9_-]+', '-', str(table.get('name')))}.json"
    replace_file(folder / file, json.dumps(_no_nan(record), ensure_ascii=False))
    return {"file": file, "url": f"/data/{WORK['name']}/exports/{file}", "events": len(rows)}, 200


# ---- who may ask ----------------------------------------------------------------------------------------------------------------


def write_refusal(handler) -> str | None:
    if not WRITES["allowed"]:
        return "this server keeps nothing: start it with --token to allow keeping with a token"
    if WRITES["token"] and handler.headers.get("Authorization", "") != f"Bearer {WRITES['token']}":
        return "keeping on this server needs its token"
    origin = handler.headers.get("Origin")
    if origin is not None and urllib.parse.urlparse(origin).netloc != handler.headers.get("Host", ""):
        return "this server keeps only what its own page sends"
    return None


def run_refusal(handler) -> str | None:
    """Why this server would not run a tool or keep a script for this request, or None: a script runs with the authority of
    whoever started the server, so only this work's own page, at this computer's own address, may ask."""
    refusal = write_refusal(handler)
    if refusal:
        return refusal
    host, origin = handler.headers.get("Host", ""), handler.headers.get("Origin")
    if handler.headers.get("X-Studio") != "run" or "application/json" not in handler.headers.get("Content-Type", "") or origin is None or urllib.parse.urlparse(origin).netloc != host:
        return "a tool is run, and a script kept, only from this work's own page"
    if not WRITES["token"] and host.rsplit(":", 1)[0] not in ("127.0.0.1", "localhost", "[::1]"):
        return "a tool is run, and a script kept, only at this computer's own address"
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

    def body(self, limit: int) -> bytes | None:
        length = int(self.headers.get("Content-Length", "0") or 0)
        if length > limit:
            self.reply_json({"message": f"the request is too large: at most {limit // (1024 * 1024)} MB"}, 413)
            return None
        return self.rfile.read(length)

    def json_body(self, limit: int) -> dict | None:
        raw = self.body(limit)
        if raw is None:
            return None
        try:
            body = json.loads(raw.decode("utf-8") or "{}")
            if not isinstance(body, dict):
                raise ValueError("a request is a JSON object")
            return body
        except (UnicodeDecodeError, ValueError) as error:
            self.reply_json({"message": f"not a request: {error}"}, 400)
            return None

    def answer(self, act) -> None:
        """Reply with what `act` returns, (body, code); what it cannot find or read is said, not raised."""
        try:
            self.reply_json(*act())
        except KeyError as error:
            self.reply_json({"message": str(error.args[0])}, 404)
        except (sources.Unreadable, ValueError) as error:
            self.reply_json({"message": str(error)}, 400)
        except OSError as error:   # a file that could not be read or kept is said, not dropped
            self.reply_json({"message": f"a file could not be read or kept: {error}"}, 500)

    def do_GET(self) -> None:
        url = urllib.parse.urlparse(self.path)
        query = dict(urllib.parse.parse_qsl(url.query))
        parts = url.path.strip("/").split("/")
        if url.path == "/api/status":
            runs = None
            try:
                import numpy  # noqa: F401
            except ImportError:
                runs = "the Python that runs this server has no numpy: tools cannot be run"
            return self.reply_json({"work": {"name": WORK["name"], "title": WORK["title"]}, "engine": engine_status(), "tools": runs,
                                    "writable": write_refusal(self) is None,
                                    "plugins": [{k: v for k, v in g.items() if k != "dir"} for g in plugins()]}, 200)
        if url.path == "/api/templates":
            return self.reply_json({"templates": templates()}, 200)
        if url.path == "/api/projects":
            return self.reply_json({"projects": list_projects()}, 200)
        if url.path == "/api/tools":
            return self.answer(lambda: ({"tools": tools(query.get("project") or None)}, 200))
        if url.path == "/setup":
            return self.reply_json(*self.recipe_request(url))
        if len(parts) == 3 and parts[:2] == ["api", "job"]:
            job = RUN["jobs"].get(parts[2])
            if job is None:
                return self.reply_json({"message": "there is no such run"}, 404)
            reply = {k: job[k] for k in ("id", "state", "progress", "text", "step", "message", "detail") if k in job}
            if job["state"] != "running":
                RUN["jobs"].pop(job["id"], None)
            return self.reply_json(reply, 200)
        if len(parts) >= 3 and parts[:2] == ["api", "projects"]:
            name = parts[2]
            if len(parts) == 3:
                return self.answer(lambda: ({"name": name, "project": read_project(name),
                                             "scripts": sorted(f.name for f in (project_dir(name) / "scripts").glob("*.py"))}, 200))
            if len(parts) == 4 and parts[3] == "table":
                def table():
                    p = read_project(name)
                    piece = next((s for s in p.get("slices", []) if s.get("id") == query.get("slice")), None)
                    return sent(within(find_table(name, p, query.get("name", "")), piece), int(query["limit"]) if query.get("limit", "").isdigit() else None), 200
                return self.answer(table)
            if len(parts) == 4 and parts[3] == "result":
                def result():
                    path = project_dir(name) / "results" / f"{query.get('step', '')}.json"
                    if not ID.match(query.get("step", "")) or not path.is_file():
                        raise KeyError("there is no such result")
                    results = json.loads(path.read_text(encoding="utf-8")).get("results", [])
                    return {"figures": [{"k": k, "title": r.get("title", ""), "format": r.get("format"), "data": r.get("data")} for k, r in enumerate(results) if r.get("kind") == "figure"]}, 200
                return self.answer(result)
            if len(parts) == 5 and parts[3] in ("boards", "scripts"):
                def kept():
                    ok = ID.match(parts[4]) if parts[3] == "boards" else SCRIPT.match(parts[4])
                    path = project_dir(name) / parts[3] / (f"{parts[4]}.json" if parts[3] == "boards" else parts[4])
                    if not ok:
                        raise KeyError("there is no such file")
                    if parts[3] == "boards":   # a board not yet kept is no error: the page begins an empty one
                        return (json.loads(path.read_text(encoding="utf-8")) if path.is_file() else None), 200
                    if not path.is_file():
                        raise KeyError(f"the project has no script '{parts[4]}'")
                    return {"file": parts[4], "code": path.read_text(encoding="utf-8")}, 200
                return self.answer(kept)
        return super().do_GET()

    def do_PUT(self) -> None:
        url = urllib.parse.urlparse(self.path)
        parts = url.path.strip("/").split("/")
        if not (len(parts) >= 3 and parts[:2] == ["api", "projects"]):
            return self.reply_json({"message": "nothing is kept there"}, 404)
        name = parts[2]
        script = len(parts) == 5 and parts[3] == "scripts"
        refusal = run_refusal(self) if script else write_refusal(self)
        if refusal:
            return self.reply_json({"message": refusal}, 403)
        body = self.json_body(LIMITS["script"] if script else LIMITS["project"])
        if body is None:
            return None

        def keep():
            folder = project_dir(name)
            if not (folder / "project.json").is_file():
                raise KeyError(f"there is no project '{name}'")
            if len(parts) == 3:
                if body.get("schema") != SCHEMA or not all(isinstance(body.get(k), list) for k in ("sources", "slices", "quantities", "steps", "views", "boards")):
                    raise ValueError(f"not a project of the schema {SCHEMA}")
                body.setdefault("meta", {})["saved"] = stamp()
                target = folder / "project.json"
            elif len(parts) == 5 and parts[3] == "boards" and ID.match(parts[4]):
                if body.get("schema") != BOARD or not all(isinstance(body.get(k), t) for k, t in (("items", dict), ("place", dict), ("order", list))):
                    raise ValueError(f"not a board of the schema {BOARD}")
                target = folder / "boards" / f"{parts[4]}.json"
            elif script and SCRIPT.match(parts[4]):
                if not isinstance(body.get("code"), str):
                    raise ValueError("a script is kept as its text")
                (folder / "scripts").mkdir(parents=True, exist_ok=True)
                (folder / "scripts" / parts[4]).write_text(body["code"], encoding="utf-8")
                RUN["hosts"].pop(f"project:{name}", None)
                return {"kept": parts[4]}, 200
            else:
                raise KeyError("nothing is kept there")
            replace_file(target, json.dumps(body, ensure_ascii=False, indent=1 if len(parts) == 3 else None))
            return {"kept": target.stem, "saved": (body.get("meta") or {}).get("saved")}, 200
        self.answer(keep)

    def do_POST(self) -> None:
        url = urllib.parse.urlparse(self.path)
        query = dict(urllib.parse.parse_qsl(url.query))
        parts = url.path.strip("/").split("/")
        if url.path == "/setup/run":
            return self.reply_json(*self.recipe_request(url))
        if len(parts) == 4 and parts[:2] == ["api", "job"] and parts[3] == "cancel":
            refusal = run_refusal(self)
            if refusal:
                return self.reply_json({"message": refusal}, 403)
            job = RUN["jobs"].get(parts[2])
            if job is None:
                return self.reply_json({"message": "there is no such run"}, 404)
            job["cancel"].set()
            return self.reply_json({"cancelled": job["id"]}, 200)
        if url.path == "/api/projects":
            refusal = write_refusal(self)
            if refusal:
                return self.reply_json({"message": refusal}, 403)
            body = self.json_body(LIMITS["run"])
            return None if body is None else self.reply_json(*new_project(body))
        if len(parts) == 4 and parts[:2] == ["api", "projects"]:
            name, what = parts[2], parts[3]
            if what == "files":
                refusal = write_refusal(self)
                if refusal:
                    return self.reply_json({"message": refusal}, 403)
                raw = self.body(LIMITS["file"])
                if raw is None:
                    return None

                def keep_file():
                    file = re.sub(r"[^A-Za-z0-9._-]+", "-", Path(query.get("name", "")).name).strip("-.")
                    if Path(file).suffix.lower() not in DATA:
                        raise ValueError(f"a file of data is one of {', '.join(DATA)}, not '{query.get('name', '')}'")
                    folder = project_dir(name)
                    if not (folder / "project.json").is_file():
                        raise KeyError(f"there is no project '{name}'")
                    (folder / "data").mkdir(parents=True, exist_ok=True)
                    (folder / "data" / file).write_bytes(raw)
                    return {"ref": f"project:data/{file}", "name": file}, 200
                return self.answer(keep_file)
            if what == "read":
                body = self.json_body(LIMITS["run"])
                if body is None:
                    return None

                def read():
                    p = read_project(name)
                    piece = next((s for s in p.get("slices", []) if s.get("id") == body.get("slice")), None)
                    return sent(within(read_source(name, body.get("source") or {}), piece), body.get("limit") if isinstance(body.get("limit"), int) else None), 200
                return self.answer(read)
            if what == "export":
                refusal = write_refusal(self)
                if refusal:
                    return self.reply_json({"message": refusal}, 403)
                body = self.json_body(LIMITS["run"])
                return None if body is None else self.answer(lambda: export(name, body))
            if what == "run":
                refusal = run_refusal(self)
                if refusal:
                    return self.reply_json({"message": refusal}, 403)
                body = self.json_body(LIMITS["run"])
                return None if body is None else self.reply_json(*start_run(name, body))
        self.send_error(404)

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

    def reply_json(self, body, code: int) -> None:
        self.reply_text(json.dumps(_no_nan(body), ensure_ascii=False, default=str), code)

    def reply_text(self, text: str, code: int) -> None:
        data = text.encode("utf-8")
        try:
            self.send_response(code)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            # An answer is the server's state now (its tools, projects, jobs): a browser must not keep one, or a plug-in's new tool
            # stays unseen by a page that asked before it came.
            self.send_header("Cache-Control", "no-store")
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


def _no_nan(value):
    """What JSON cannot write (NaN, the infinities) as null."""
    if isinstance(value, float):
        return None if value != value or value in (float("inf"), float("-inf")) else value
    if isinstance(value, dict):
        return {k: _no_nan(v) for k, v in value.items()}
    if isinstance(value, (list, tuple)):
        return [_no_nan(v) for v in value]
    return value


class Server(http.server.ThreadingHTTPServer):
    request_queue_size = 64
    daemon_threads = True


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=f"Serve {WORK['title']} locally.")
    parser.add_argument("--host", default="127.0.0.1", help="the address to bind; 0.0.0.0 for every interface")
    parser.add_argument("--port", type=int, default=WORK["port"])
    parser.add_argument("--open", action="store_true", help="open the page in the default browser")
    parser.add_argument("--token", help="on a public address, the token that allows keeping projects and running tools")
    parser.add_argument("--store", help=f"where projects are kept (default: data/cache/{WORK['name']})")
    parser.add_argument("--python", help="the Python that runs the tools (default: this one)")
    args = parser.parse_args(argv)
    if args.store:
        STORE["dir"] = Path(args.store).resolve()
    RUN["python"] = args.python
    public = args.host not in ("127.0.0.1", "localhost", "::1")
    if public:
        WRITES.update(allowed=bool(args.token), token=args.token)
    if not engine_status().get("current", True):
        print("An engine is not built from the present sources. Build it from the page, or run the work's recipe:", file=sys.stderr)
        print(f"  python data/acquisition/recipe.py {APP.relative_to(ROOT).as_posix()}/recipe.json", file=sys.stderr)
    server = Server((args.host, args.port), Handler)
    url = f"http://127.0.0.1:{args.port}/"
    print(f"{WORK['title']} at {url if not public else url + ' and on ' + args.host}  (Ctrl+C to stop)", flush=True)
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    return 0


