"""The Law Studio's own routes (ADR 0121): a project's matter, its documents, what the law engine makes of them, and what a language
model proposes, on the API's host for project-based works (studio_host.configure(routes=...)).

A project is a folder under data/cache/law-studio/projects/<id>/ (the host's project.json beside it):

- ``matter.json``: the matter, ``noetarium.matter/1`` (api/cpp/science/law.hpp), with ``scheme`` naming the scheme it is read under
  (``<plug-in>/<scheme>``) and, kept with it, the candidates of evidence, the parties' arguments and the exhibits with their custody;
- ``documents/``: the documents as they were given, each with the SHA-256 of its bytes beside it, and the text read from it (a PDF's
  text layer, a Word file's paragraphs, an HTML page's words) under ``documents/.text/``;
- ``proposals.json``: what a model proposed for the readings of the open terms, each with who proposed it and when.

Routes, under /api/projects/<project>/:

  GET  matter                         the matter and the scheme it names
  PUT  matter        {matter, base}   keep it (refused with the current one when it changed since `base`, its SHA-256)
  POST evaluate      {matter?}        the engine's evaluation: evidence, facts, authority, each issue's label, what it rests on
                                      and its plan of proof; an exhibit's digest observed now from its document's bytes
  GET  documents                      the documents, with their digests and whether text was read from them
  GET  documents/<name>               a document's text
  POST documents?name=<name>          a document added (its bytes as the body)
  POST recognise     {document}       a model reads a document against the matter's facts; its passages, each found in the text,
                                      join the matter's candidates as proposals; those not found are said and dropped
  POST review        {candidate, status}   a candidate accepted or rejected by the person asking
  POST readings      {}               a model proposes readings of the open terms (grade C), kept in proposals.json
  POST example       {template?}      a plug-in's template laid out in the project

and /api/schemes (the schemes the plug-ins bring), /api/schemes/<plug-in>/<scheme> (one, with its provisions' words filled from the
official texts fetched into the data library's cache), /api/models (whether a key is kept for each provider, never the key).

Every change is written in the project's journal (with --people, sealed and told to the others' pages).
"""

from __future__ import annotations

import datetime as dt
import hashlib
import html
import io
import json
import re
import sys
import threading
import zipfile
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import law, models, recognise  # noqa: E402
from science import studio_host as host  # noqa: E402

TEXT_LIMIT = 16 * 1024 * 1024
DOCUMENT_LIMIT = 64 * 1024 * 1024
PLUGINS = APP / "plugins"
BY: dict[int, str] = {}
LOCK = threading.Lock()


def now() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def sha(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def who() -> str:
    return BY.get(threading.get_ident(), host.PEOPLE["keeper"])


def shown(path: Path) -> str:
    """A path as a page is told of it: within the person's home, from there (``~``), so that no page learns the account's name."""
    try:
        return (Path("~") / path.resolve().relative_to(Path.home().resolve())).as_posix()
    except ValueError:
        return path.name


def check_name(name: str) -> str:
    name = Path(str(name)).name
    if not name or name.startswith(".") or any(c in name for c in '\\/:*?"<>|'):
        raise ValueError(f"not a name a document may have: {name!r}")
    return name


# ---- Schemes ------------------------------------------------------------------------------------------------------------------------


def schemes() -> list[dict]:
    out = []
    for path in sorted(PLUGINS.glob("*/schemes/*.json")):
        try:
            s = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        out.append({"id": f"{path.parent.parent.name}/{path.stem}", "title": s.get("title", path.stem), "as_of": s.get("as_of", ""), "about": s.get("about", ""),
                    "instruments": [{"id": i["id"], "title": i.get("title", ""), "citation": i.get("citation", "")} for i in s.get("instruments", [])]})
    return out


def scheme(ident: str) -> tuple[dict, list[str]]:
    """A scheme by its id, its provisions' words filled from the fetched official texts, and the instruments not yet fetched."""
    plugin, _, stem = str(ident).partition("/")
    path = PLUGINS / check_name(plugin) / "schemes" / f"{check_name(stem)}.json"
    if not path.is_file():
        raise KeyError(f"there is no scheme {ident}")
    return law.with_texts(json.loads(path.read_text(encoding="utf-8")))


# ---- Documents ------------------------------------------------------------------------------------------------------------------------


def folder(project: str) -> Path:
    return host.project_dir(project) / "documents"


def read_text(name: str, data: bytes) -> str | None:
    """The text a document holds, where it can be read: plain text, Markdown, HTML's words, a PDF's text layer, a Word file's
    paragraphs. None for what holds no text (a scan must be transcribed first)."""
    suffix = Path(name).suffix.lower()
    if suffix in (".txt", ".md", ".csv", ".eml", ".json", ".ttl"):
        return data.decode("utf-8", errors="replace")
    if suffix in (".html", ".htm", ".xml"):
        text = re.sub(r"(?is)<(script|style)\b.*?</\1>", " ", data.decode("utf-8", errors="replace"))
        text = re.sub(r"(?i)<br\s*/?>|</p>|</div>|</h\d>|</li>", "\n", text)
        return html.unescape(re.sub(r"<[^>]+>", " ", text))
    if suffix == ".pdf":
        try:
            import pypdf
        except ImportError:
            return None
        reader = pypdf.PdfReader(io.BytesIO(data))
        text = "\n\n".join((page.extract_text() or "") for page in reader.pages)
        return text if text.strip() else None
    if suffix == ".docx":
        with zipfile.ZipFile(io.BytesIO(data)) as z:
            xml = z.read("word/document.xml").decode("utf-8", errors="replace")
        paragraphs = re.findall(r"(?s)<w:p\b.*?</w:p>", xml)
        return "\n".join(html.unescape("".join(re.findall(r"<w:t[^>]*>(.*?)</w:t>", p))) for p in paragraphs)
    return None


def documents(project: str) -> list[dict]:
    d = folder(project)
    out = []
    if d.is_dir():
        for p in sorted(d.iterdir()):
            if p.is_file() and not p.name.endswith(".sha256"):
                digest_file = d / (p.name + ".sha256")
                text_file = d / ".text" / (p.name + ".txt")
                out.append({"name": p.name, "bytes": p.stat().st_size, "sha256": digest_file.read_text(encoding="utf-8").strip() if digest_file.is_file() else None,
                            "text": text_file.is_file(), "chars": len(text_file.read_text(encoding="utf-8")) if text_file.is_file() else 0})
    return out


def document_text(project: str, name: str) -> str:
    path = folder(project) / ".text" / (check_name(name) + ".txt")
    if not path.is_file():
        raise KeyError(f"no text was read from {name}")
    return path.read_text(encoding="utf-8")


def add_document(project: str, name: str, data: bytes, record: bool = True) -> dict:
    d = folder(project)
    (d / ".text").mkdir(parents=True, exist_ok=True)
    name = check_name(name)
    stem, suffix = Path(name).stem, Path(name).suffix
    target, k = d / name, 1
    while target.exists():
        target, k = d / f"{stem} ({k}){suffix}", k + 1
    digest = hashlib.sha256(data).hexdigest()
    text = read_text(target.name, data)
    with host.FILES:
        target.write_bytes(data)
        (d / (target.name + ".sha256")).write_text(digest + "\n", encoding="utf-8")
        if text is not None:
            (d / ".text" / (target.name + ".txt")).write_text(text, encoding="utf-8")
    if record:
        host.record(project, who(), "kept a document", target.name, f"SHA-256 {digest}" + ("" if text is not None else "; no text could be read from it"))
    return {"name": target.name, "sha256": digest, "bytes": len(data), "text": text is not None}


def observed(project: str, name: str) -> str:
    """The SHA-256 of a document's bytes as they are now."""
    path = folder(project) / check_name(name)
    return hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else ""


# ---- The matter ----------------------------------------------------------------------------------------------------------------------


def matter_path(project: str) -> Path:
    return host.project_dir(project) / "matter.json"


def read_matter(project: str) -> dict:
    path = matter_path(project)
    if not path.is_file():
        return {"schema": "noetarium.matter/1", "id": project, "title": "", "scheme": "", "issues": [], "facts": [], "candidates": [], "arguments": [], "exhibits": []}
    return json.loads(path.read_text(encoding="utf-8"))


def keep_matter(project: str, matter: dict, base: str | None, what: str = "kept the matter", note: str = "") -> dict:
    path = matter_path(project)
    with LOCK:
        if base is not None and path.is_file():
            current = path.read_text(encoding="utf-8")
            if sha(current) != base:
                return {"message": "the matter changed since it was read", "current": json.loads(current), "sha256": sha(current), "conflict": True}
        if not isinstance(matter, dict) or matter.get("schema", "noetarium.matter/1") != "noetarium.matter/1":
            raise ValueError("a matter is a JSON object of the schema noetarium.matter/1")
        matter["schema"] = "noetarium.matter/1"
        text = json.dumps(matter, ensure_ascii=False, indent=1)
        path.write_text(text, encoding="utf-8")
    host.record(project, who(), what, matter.get("title") or project, note)
    return {"kept": True, "sha256": sha(text)}


def prepared(project: str, matter: dict) -> dict:
    """The matter as the engine takes it: the documents its candidates cite, with their text; each exhibit's digest observed now."""
    m = json.loads(json.dumps(matter))
    names = {c.get("document") for c in m.get("candidates", []) if c.get("document")}
    m["documents"] = []
    for name in sorted(n for n in names if n):
        try:
            m["documents"].append({"id": name, "text": document_text(project, name)})
        except (KeyError, ValueError):
            pass
    for e in m.get("exhibits", []):
        if e.get("document"):
            e["observed"] = observed(project, e["document"])
    return m


def evaluate(project: str, matter: dict | None = None) -> dict:
    m = matter if matter is not None else read_matter(project)
    if not m.get("scheme"):
        raise ValueError("the matter names no scheme to be read under")
    s, missing = scheme(m["scheme"])
    result = law.ask("evaluate", scheme=s, matter=prepared(project, m), judgments=law.judgments(s))
    return {"evaluation": result, "unfetched": missing, "scheme": m["scheme"]}


# ---- What a model proposes ----------------------------------------------------------------------------------------------------------


def settings() -> dict:
    try:
        data = json.loads((models.settings_folder() / "law-studio.json").read_text(encoding="utf-8"))
        return data if isinstance(data, dict) else {}
    except (OSError, ValueError):
        return {}


def provider():
    s = settings()
    return models.provider(s.get("provider", "anthropic"), **({"model": s["model"]} if s.get("model") else {}), **({"effort": s["effort"]} if s.get("effort") else {}))


def recognise_in(project: str, name: str, model=None) -> dict:
    m = read_matter(project)
    s, _ = scheme(m["scheme"]) if m.get("scheme") else ({"vocabulary": []}, [])
    gloss = {v["id"]: v.get("gloss", "") for v in s.get("vocabulary", [])}
    facts = [{"id": f["id"], "text": f.get("text") or gloss.get(f["id"], "")} for f in m.get("facts", [])]
    text = document_text(project, name)
    found = recognise.propose({"id": name, "title": name, "text": text}, facts, model or provider(), name="a language model")
    known = {(c.get("proposition"), c.get("document"), c.get("passage")) for c in m.get("candidates", [])}
    added = []
    for c in found["candidates"]:
        if (c["proposition"], c["document"], c["passage"]) in known:
            continue
        c["id"] = f"{name}:{len(m.get('candidates', [])) + len(added) + 1}"
        added.append(c)
    m.setdefault("candidates", []).extend(added)
    keep_matter(project, m, None, "had a document read by a model", f"{name}: {len(added)} passages proposed, {len(found['refused'])} not found in it")
    return {"added": added, "refused": found["refused"], "by": found["by"]}


def review(project: str, candidate: str, status: str) -> dict:
    if status not in ("accepted", "rejected", "proposed"):
        raise ValueError("a candidate is accepted, rejected or proposed")
    m = read_matter(project)
    for c in m.get("candidates", []):
        if c.get("id") == candidate:
            c["status"] = status
            c["reviewed_by"], c["reviewed_at"] = who(), now()
            keep_matter(project, m, None, f"{status} a candidate of evidence", f"{candidate}: {c.get('proposition')}")
            return {"candidate": c}
    raise KeyError(f"there is no candidate {candidate}")


def proposals(project: str) -> list[dict]:
    path = host.project_dir(project) / "proposals.json"
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return []


def propose_readings(project: str, model=None) -> dict:
    m = read_matter(project)
    s, _ = scheme(m["scheme"])
    ev = law.ask("evaluate", scheme=s, matter=prepared(project, m))
    p = law.propose_readings(s, m, ev, model or provider())
    kept = proposals(project) + [p]
    (host.project_dir(project) / "proposals.json").write_text(json.dumps(kept, ensure_ascii=False, indent=1), encoding="utf-8")
    host.record(project, who(), "had the open terms read by a model", m.get("title") or project, ", ".join(f"{k}: {v or 'none'}" for k, v in p["proposed"].items()))
    return {"proposal": p}


# ---- Templates ---------------------------------------------------------------------------------------------------------------------


def templates() -> list[dict]:
    out = []
    for path in sorted(PLUGINS.glob("*/templates/*/matter.json")):
        m = json.loads(path.read_text(encoding="utf-8"))
        out.append({"id": f"{path.parent.parent.parent.name}/{path.parent.name}", "title": m.get("title", path.parent.name), "about": m.get("about", "")})
    return out


def example(project: str, template: str | None = None) -> dict:
    """A plug-in's template laid out in the project: its documents kept (with their digests) and its matter written."""
    choices = templates()
    if not choices:
        raise KeyError("no plug-in brings a template")
    ident = template or choices[0]["id"]
    plugin, _, name = ident.partition("/")
    base = PLUGINS / check_name(plugin) / "templates" / check_name(name)
    if not (base / "matter.json").is_file():
        raise KeyError(f"there is no template {ident}")
    m = json.loads((base / "matter.json").read_text(encoding="utf-8"))
    kept = []
    for p in sorted((base / "documents").glob("*")) if (base / "documents").is_dir() else []:
        if p.is_file() and not (folder(project) / p.name).exists():
            kept.append(add_document(project, p.name, p.read_bytes(), record=False)["name"])
    # An exhibit's chain is taken in with the digest of its document's bytes, as they are kept here.
    for e in m.get("exhibits", []):
        if e.get("document") and e.get("chain") and e["chain"].get("digest") == "@document":
            digest = observed(project, e["document"])
            e["chain"]["digest"] = digest
            for t in e["chain"].get("transfers", []):
                if t.get("digest") == "@document":
                    t["digest"] = digest
    keep_matter(project, m, None, "laid out a template", f"{ident}: {len(kept)} documents")
    return {"template": ident, "documents": kept, "matter": m.get("title", "")}


# ---- The routes ----------------------------------------------------------------------------------------------------------------------


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The studio's own routes; True when this answered."""
    BY[threading.get_ident()] = host.who(handler)
    if parts[:2] == ["api", "schemes"] and method == "GET":
        if len(parts) == 2:
            handler.answer(lambda: ({"schemes": schemes(), "templates": templates()}, 200))
        else:
            def one():
                s, missing = scheme("/".join(parts[2:4]))
                return {"scheme": s, "unfetched": missing, "check": law.ask("scheme", scheme=s, judgments=law.judgments(s))}, 200
            handler.answer(one)
        return True
    if parts[:2] == ["api", "models"] and method == "GET":
        handler.answer(lambda: ({"providers": {k: models.key_state(k) for k in models.PROVIDERS if k != "stub"}, "settings": settings(),
                                 "models": list(models.Anthropic.models), "kept_in": shown(models.settings_folder() / "models.json")}, 200))
        return True
    OWN = ("matter", "evaluate", "documents", "recognise", "review", "readings", "example", "proposals")
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] in OWN):
        return False
    project, what, rest = parts[2], parts[3], parts[4:]
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the matter is seen by its people: ask the keeper for a key"}, 403)
        return True

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    if method == "GET":
        if what == "matter":
            def get():
                exists()
                m = read_matter(project)
                return {"matter": m, "sha256": sha(json.dumps(m, ensure_ascii=False, indent=1)) if matter_path(project).is_file() else None}, 200
            handler.answer(get)
            return True
        if what == "documents" and not rest:
            handler.answer(lambda: (exists(), ({"documents": documents(project)}, 200))[1])
            return True
        if what == "documents" and len(rest) == 1:
            import urllib.parse
            name = urllib.parse.unquote(rest[0])
            handler.answer(lambda: (exists(), ({"name": name, "text": document_text(project, name), "sha256": observed(project, name)}, 200))[1])
            return True
        if what == "proposals":
            handler.answer(lambda: (exists(), ({"proposals": proposals(project)}, 200))[1])
            return True
        return False
    # What is computed changes nothing and may be asked by any of the people; what is kept, by a contributor; a model is asked only
    # where scripts may run, since it spends the keeper's key.
    if what == "evaluate" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (evaluate(project, body.get("matter")), 200))[1])
        return True
    refusal = host.run_refusal(handler) if what in ("recognise", "readings") else host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    if what == "matter" and method == "PUT":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def put():
                exists()
                answer = keep_matter(project, body.get("matter"), body.get("base"), body.get("what") or "kept the matter", body.get("note", ""))
                return answer, 409 if answer.get("conflict") else 200
            handler.answer(put)
        return True
    if what == "documents" and method == "POST" and not rest:
        data = handler.body(DOCUMENT_LIMIT)
        if data is not None:
            handler.answer(lambda: (exists(), (add_document(project, query.get("name", "document"), data), 200))[1])
        return True
    if what == "recognise" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def read():
                exists()
                try:
                    return recognise_in(project, str(body.get("document", ""))), 200
                except models.Refusal as e:
                    return {"message": str(e)}, 422
            handler.answer(read)
        return True
    if what == "review" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (review(project, str(body.get("candidate", "")), str(body.get("status", ""))), 200))[1])
        return True
    if what == "readings" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def read():
                exists()
                try:
                    return propose_readings(project), 200
                except models.Refusal as e:
                    return {"message": str(e)}, 422
            handler.answer(read)
        return True
    if what == "example" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (example(project, body.get("template")), 200))[1])
        return True
    return False
