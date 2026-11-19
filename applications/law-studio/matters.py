"""The Law Studio's own routes (ADR 0121): a project's matter, its documents, what the law engine makes of them, and what a language
model proposes, on the API's host for project-based works (studio_host.configure(routes=...)).

A project is a folder under data/cache/law-studio/projects/<id>/ (the host's project.json beside it):

- ``matter.json``: the matter, ``noetarium.matter/1`` (api/cpp/science/law.hpp), with ``scheme`` naming the scheme it is read under
  (``<plug-in>/<scheme>``) and, kept with it, the candidates of evidence, the parties' arguments and the exhibits with their custody;
- ``documents/``: the documents as they were given, each with the SHA-256 of its bytes beside it, and the text read from it (a PDF's
  text layer, a Word file's paragraphs, an HTML page's words) under ``documents/.text/``;
- ``proposals.json``: what a model proposed for the readings of the open terms, each with who proposed it and when;
- ``theories/``: the search for theories of the case (ADR 0127): ``state.json`` (the engine's state: the evolution, the journal of
  tests, sealed, and the tests), ``report.json`` (what the last run found) and ``proposals.json`` (what a model or a person proposed);
- ``doctrines/``: the search for theories of the law (ADR 0133), kept the same way.

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
  GET  theories                       the last report of the search for theories of the case (ADR 0127), without its state
  POST theories/run  {generations?}   the search run a few generations from its state (begun when there is none): theories kept,
                                      each tested once on what the matter holds back, the issues over them, whether each person
                                      could have been at the scene; the state kept in theories/state.json
  POST theories/model {budget?, roles?, about?}   a model asked in its four roles (seed, mutate, cross, reflect) for theories, each
                                      read by the engine, entered as proposals of grade C, then a generation run
  POST theories/assess {text}         one theory written by a person, assessed and read under the law (nothing kept)
  POST theories/propose {text}        a person's theory entered into the search, then a generation run
  POST theories/reset                 the search begun again
  GET  doctrines, POST doctrines/run|model|assess|propose|reset   the same for theories of the law (ADR 0133): doctrines (readings
                                      of the open terms, ratios of precedents whose reasons were unstated) kept by kind, each judged by
                                      how it fits the decided cases and by the declared purposes and tested once on decisions held back

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


# ---- Theories of the case (ADR 0127) ------------------------------------------------------------------------------------------


# The searches a matter may run: theories of the case (ADR 0127) and theories of the law (ADR 0133). Each keeps its folder in the
# project (state.json, report.json, proposals.json), runs from its state a few generations at a time, asks a model in its four roles,
# and takes a person's own as a proposal; its engine's operations are named in law.py.
SEARCHES = {
    "theories": {"run": lambda s, m, **k: law.theories(s, m, **k), "assess": lambda s, m, t: law.assess_theory(s, m, t),
                 "fulfil": lambda *a, **k: law.fulfil_theories(*a, **k), "noun": "theories",
                 "best": lambda r: r["kept"][0]["central"] if r.get("kept") else "none yet", "best_word": "the most plausible"},
    "doctrines": {"run": lambda s, m, **k: law.doctrines(s, m, **k), "assess": lambda s, m, t: law.assess_doctrine(s, m, t),
                  "fulfil": lambda *a, **k: law.fulfil_doctrines(*a, **k), "noun": "doctrines",
                  "best": lambda r: r["kept"][0]["kind_words"] if r.get("kept") else "none yet", "best_word": "the best of the kind"},
}


def search_dir(project: str, kind: str) -> Path:
    return host.project_dir(project) / kind


def search_file(project: str, kind: str, name: str, empty=None):
    try:
        return json.loads((search_dir(project, kind) / name).read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return empty


def run_search(project: str, kind: str, generations: int | None = None, proposals: list | None = None, requests: dict | None = None) -> dict:
    """A search run from its state with the matter as it is now: its report kept (and its state, apart), the run journalled."""
    m = read_matter(project)
    s = scheme(m["scheme"])[0] if m.get("scheme") else None
    how = SEARCHES[kind]
    with LOCK:
        result = how["run"](s, m, state=search_file(project, kind, "state.json"), generations=generations, proposals=proposals, requests=requests, now=now())
        d = search_dir(project, kind)
        d.mkdir(parents=True, exist_ok=True)
        state = result.pop("state")
        (d / "state.json").write_text(json.dumps(state, ensure_ascii=False), encoding="utf-8")
        result["kept_at"] = now()
        (d / "report.json").write_text(json.dumps(result, ensure_ascii=False, indent=1), encoding="utf-8")
    host.record(project, who(), f"ran the search for {how['noun']}", m.get("title") or project,
                f"generation {result['generation']}: {len(result.get('kept', []))} {how['noun']} kept, {how['best_word']} {how['best'](result)}")
    return result


def model_search(project: str, kind: str, budget: int = 6, roles: list | None = None, about: str = "", model=None) -> dict:
    """A model asked in its four roles, its proposals read by the engine and entered (grade C), then a generation run."""
    m = read_matter(project)
    s = scheme(m["scheme"])[0] if m.get("scheme") else None
    how = SEARCHES[kind]
    asked = run_search(project, kind, generations=0, requests={"budget": int(budget), "roles": roles or ["seed", "mutate", "cross", "reflect"]})
    found = how["fulfil"](s, m, asked.get("requests", []), model or provider(), report=asked, description=about or None)
    log = search_file(project, kind, "proposals.json", []) + [{"at": now(), "by": found["by"], "grade": "C", "read": found["read"], "refused": found["refused"]}]
    (search_dir(project, kind) / "proposals.json").write_text(json.dumps(log, ensure_ascii=False, indent=1), encoding="utf-8")
    host.record(project, who(), f"had {how['noun']} proposed by a model", m.get("title") or project, f"{len(found['proposals'])} read, {len(found['refused'])} refused")
    result = run_search(project, kind, generations=1, proposals=found["proposals"])
    return {"report": result, "proposed": found}


def propose_own(project: str, kind: str, text: str) -> dict:
    """A person's own, read by the engine, entered into the search (under their name), then a generation run."""
    m = read_matter(project)
    s = scheme(m["scheme"])[0] if m.get("scheme") else None
    read = SEARCHES[kind]["assess"](s, m, text)
    if not read.get("read"):
        return {"read": False, "errors": read.get("errors", [])}
    search_dir(project, kind).mkdir(parents=True, exist_ok=True)
    log = search_file(project, kind, "proposals.json", []) + [{"at": now(), "by": who(), "grade": "B", "read": [{"text": text, "role": "a person"}], "refused": []}]
    (search_dir(project, kind) / "proposals.json").write_text(json.dumps(log, ensure_ascii=False, indent=1), encoding="utf-8")
    result = run_search(project, kind, generations=1, proposals=[{"text": text, "source": f"person:{who()}", "parents": []}])
    return {"read": True, "report": result}


def reset_search(project: str, kind: str) -> dict:
    d = search_dir(project, kind)
    for name in ("state.json", "report.json"):
        try:
            (d / name).unlink()
        except OSError:
            pass
    host.record(project, who(), f"began the search for {SEARCHES[kind]['noun']} again", project, "")
    return {"reset": True}


# The search for theories of the case by its own names (ADR 0127).
def theories_dir(project: str) -> Path:
    return search_dir(project, "theories")


def theories_report(project: str) -> dict | None:
    return search_file(project, "theories", "report.json")


def theories_state(project: str) -> dict | None:
    return search_file(project, "theories", "state.json")


def theories_log(project: str) -> list:
    return search_file(project, "theories", "proposals.json", [])


def run_theories(project: str, generations: int | None = None, proposals: list | None = None, requests: dict | None = None) -> dict:
    return run_search(project, "theories", generations, proposals, requests)


def model_theories(project: str, budget: int = 6, roles: list | None = None, about: str = "", model=None) -> dict:
    return model_search(project, "theories", budget, roles, about, model)


def propose_theory(project: str, text: str) -> dict:
    return propose_own(project, "theories", text)


def reset_theories(project: str) -> dict:
    return reset_search(project, "theories")


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
    OWN = ("matter", "evaluate", "documents", "recognise", "review", "readings", "example", "proposals", "theories", "doctrines")
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
        if what in SEARCHES and not rest:
            handler.answer(lambda: (exists(), ({"report": search_file(project, what, "report.json"), "begun": search_file(project, what, "state.json") is not None,
                                                "proposals": search_file(project, what, "proposals.json", [])}, 200))[1])
            return True
        return False
    # What is computed changes nothing and may be asked by any of the people; what is kept, by a contributor; a model is asked only
    # where scripts may run, since it spends the keeper's key.
    if what == "evaluate" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (evaluate(project, body.get("matter")), 200))[1])
        return True
    if what in SEARCHES and rest == ["assess"] and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def assess():
                exists()
                m = read_matter(project)
                return SEARCHES[what]["assess"](scheme(m["scheme"])[0] if m.get("scheme") else None, m, str(body.get("text", ""))), 200
            handler.answer(assess)
        return True
    refusal = host.run_refusal(handler) if what in ("recognise", "readings") or (what in SEARCHES and rest == ["model"]) else host.write_refusal(handler)
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
    if what in SEARCHES and method == "POST" and rest and rest[0] in ("run", "model", "propose", "reset"):
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def act():
                exists()
                try:
                    if rest[0] == "run":
                        g = body.get("generations")
                        return {"report": run_search(project, what, int(g) if g is not None else None)}, 200
                    if rest[0] == "model":
                        return model_search(project, what, int(body.get("budget", 6)), body.get("roles"), str(body.get("about", ""))), 200
                    if rest[0] == "propose":
                        return propose_own(project, what, str(body.get("text", ""))), 200
                    return reset_search(project, what), 200
                except models.Refusal as e:
                    return {"message": str(e)}, 422
            handler.answer(act)
        return True
    if what == "example" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (example(project, body.get("template")), 200))[1])
        return True
    return False
