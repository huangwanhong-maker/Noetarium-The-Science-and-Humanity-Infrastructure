"""The Policy Studio's own routes (ADR 0122): a project's question, what the policy engine makes of it, the studies a person pastes,
and a model's draft of a theory, on the API's host for project-based works (studio_host.configure(routes=...)).

A project is a folder under data/cache/policy-studio/projects/<id>/ (the host's project.json beside it):

- ``question.json``: the question, ``noetarium.policy/1`` (api/cpp/science/policy.hpp): the intervention and the outcome asked
  about, the theory of change in its written form, the grading scheme and its rules, the bodies of evidence with their studies, the
  place, the options and the decision. It is the whole of what the engine is asked: nothing else is read.
- ``drafts.json``: what a model drafted, each with who drafted it and when; a draft is a proposal and enters the question only
  when a person takes it.

Routes, under /api/projects/<project>/:

  GET  question                         the question, with the SHA-256 it is kept under
  PUT  question      {question, base}   keep it (refused with the current one when it changed since `base`)
  POST appraise      {question?}        the engine's answer: the bodies pooled and graded, the theory with each link's standing,
                                        the strongest path, here, the options, the decision
  POST example       {template?}        a plug-in's template laid out in the project
  POST draft         {description}      a model drafts a theory of change (a proposal; read by the engine's reader or refused)
  GET  drafts                           what was drafted

and, of no project:

  GET  /api/schemes                     the grading schemes and the templates the plug-ins bring
  POST /api/theory   {text}             a theory read from its written form: its nodes and links, or the lines that do not read
  POST /api/studies  {csv}              a pasted table read as studies
  GET  /api/models                      whether a key is kept for each provider, never the key

Every change is written in the project's journal (with --people, sealed and told to the others' pages).
"""

from __future__ import annotations

import datetime as dt
import hashlib
import json
import sys
import threading
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import models, policy  # noqa: E402
from science import studio_host as host  # noqa: E402

TEXT_LIMIT = 16 * 1024 * 1024
PLUGINS = APP / "plugins"
SCHEMA = "noetarium.policy/1"
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
        raise ValueError(f"not a name: {name!r}")
    return name


# ---- What the plug-ins bring -------------------------------------------------------------------------------------------------------


def schemes() -> list[dict]:
    out = []
    for path in sorted(PLUGINS.glob("*/schemes/*.json")):
        try:
            s = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        out.append({"id": f"{path.parent.parent.name}/{path.stem}", "title": s.get("title", path.stem), "about": s.get("about", ""), "text": s.get("text", ""), "rules": s.get("rules", [])})
    return out


def templates() -> list[dict]:
    out = []
    for path in sorted(PLUGINS.glob("*/templates/*/question.json")):
        try:
            q = json.loads(path.read_text(encoding="utf-8"))
        except ValueError:
            continue
        out.append({"id": f"{path.parent.parent.parent.name}/{path.parent.name}", "title": q.get("title", path.parent.name), "about": q.get("about", "")})
    return out


# ---- The question --------------------------------------------------------------------------------------------------------------------


def blank(project: str) -> dict:
    return {"schema": SCHEMA, "title": "", "about": "", "intervention": "", "outcome": "", "least": "", "theory": "", "scheme": "", "rules": [], "bodies": [],
            "place": {"name": "", "same_principle": "unknown", "factors": {}}, "scale": {"kind": "as_given"}, "discount": 0.03, "options": [], "decision": {}}


def question_path(project: str) -> Path:
    return host.project_dir(project) / "question.json"


def read_question(project: str) -> dict:
    path = question_path(project)
    return json.loads(path.read_text(encoding="utf-8")) if path.is_file() else blank(project)


def keep_question(project: str, question: dict, base: str | None, what: str = "kept the question", note: str = "") -> dict:
    path = question_path(project)
    with LOCK:
        if base is not None and path.is_file():
            current = path.read_text(encoding="utf-8")
            if sha(current) != base:
                return {"message": "the question changed since it was read", "current": json.loads(current), "sha256": sha(current), "conflict": True}
        if not isinstance(question, dict) or question.get("schema", SCHEMA) != SCHEMA:
            raise ValueError(f"a question is a JSON object of the schema {SCHEMA}")
        question["schema"] = SCHEMA
        text = json.dumps(question, ensure_ascii=False, indent=1)
        with host.FILES:
            path.write_text(text, encoding="utf-8")
    host.record(project, who(), what, question.get("title") or project, note)
    return {"kept": True, "sha256": sha(text)}


def appraise(project: str, question: dict | None = None) -> dict:
    q = question if question is not None else read_question(project)
    if not str(q.get("theory", "")).strip():
        raise ValueError("the question has no theory of change yet")
    return {"appraisal": policy.ask("appraise", question=q)}


def example(project: str, template: str | None = None) -> dict:
    """A plug-in's template laid out in the project."""
    choices = templates()
    if not choices:
        raise KeyError("no plug-in brings a template")
    ident = template or choices[0]["id"]
    plugin, _, name = ident.partition("/")
    path = PLUGINS / check_name(plugin) / "templates" / check_name(name) / "question.json"
    if not path.is_file():
        raise KeyError(f"there is no template {ident}")
    q = json.loads(path.read_text(encoding="utf-8"))
    keep_question(project, q, None, "laid out a template", ident)
    return {"template": ident, "question": q.get("title", "")}


# ---- What a model drafts ---------------------------------------------------------------------------------------------------------------


def settings() -> dict:
    try:
        data = json.loads((models.settings_folder() / "policy-studio.json").read_text(encoding="utf-8"))
        return data if isinstance(data, dict) else {}
    except (OSError, ValueError):
        return {}


def provider():
    s = settings()
    return models.provider(s.get("provider", "anthropic"), **({"model": s["model"]} if s.get("model") else {}), **({"effort": s["effort"]} if s.get("effort") else {}))


def drafts(project: str) -> list[dict]:
    try:
        return json.loads((host.project_dir(project) / "drafts.json").read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return []


def draft(project: str, description: str, model=None) -> dict:
    """A model's draft of a theory for the project, kept beside the question as a proposal; the question is not changed."""
    q = read_question(project)
    levels = None
    for line in str(q.get("scheme", "")).splitlines():
        if line.split()[:1] == ["levels"]:
            levels = line.split()[1:]
    d = policy.draft_theory(description, model or provider(), levels=levels)
    d["description"], d["at"], d["asked_by"] = " ".join(description.split()), now(), who()
    kept = drafts(project) + [d]
    with host.FILES:
        (host.project_dir(project) / "drafts.json").write_text(json.dumps(kept, ensure_ascii=False, indent=1), encoding="utf-8")
    host.record(project, who(), "had a theory drafted by a model", q.get("title") or project,
                f"{len(d['theory']['nodes'])} states and {len(d['theory']['links'])} causal claims, a proposal with no evidence")
    return {"draft": d}


# ---- The routes ----------------------------------------------------------------------------------------------------------------------


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The studio's own routes; True when this answered."""
    BY[threading.get_ident()] = host.who(handler)
    if parts[:2] == ["api", "schemes"] and method == "GET" and len(parts) == 2:
        handler.answer(lambda: ({"schemes": schemes(), "templates": templates()}, 200))
        return True
    if parts[:2] == ["api", "theory"] and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (policy.ask("theory", text=str(body.get("text", ""))), 200))
        return True
    if parts[:2] == ["api", "studies"] and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: ({"studies": policy.studies_from_csv(str(body.get("csv", "")))}, 200))
        return True
    if parts[:2] == ["api", "models"] and method == "GET":
        handler.answer(lambda: ({"providers": {k: models.key_state(k) for k in models.PROVIDERS if k != "stub"}, "settings": settings(),
                                 "kept_in": shown(models.settings_folder() / "models.json")}, 200))
        return True
    OWN = ("question", "appraise", "example", "draft", "drafts")
    if not (len(parts) == 4 and parts[:2] == ["api", "projects"] and parts[3] in OWN):
        return False
    project, what = parts[2], parts[3]
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the question is seen by its people: ask the keeper for a key"}, 403)
        return True

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    if method == "GET":
        if what == "question":
            def get():
                exists()
                q = read_question(project)
                return {"question": q, "sha256": sha(json.dumps(q, ensure_ascii=False, indent=1)) if question_path(project).is_file() else None}, 200
            handler.answer(get)
            return True
        if what == "drafts":
            handler.answer(lambda: (exists(), ({"drafts": drafts(project)}, 200))[1])
            return True
        return False
    # What is computed changes nothing and may be asked by any of the people; what is kept, by a contributor; a model is asked only
    # where scripts may run, since it spends the keeper's key.
    if what == "appraise" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (appraise(project, body.get("question")), 200))[1])
        return True
    refusal = host.run_refusal(handler) if what == "draft" else host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    if what == "question" and method == "PUT":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def put():
                exists()
                answer = keep_question(project, body.get("question"), body.get("base"), body.get("what") or "kept the question", body.get("note", ""))
                return answer, 409 if answer.get("conflict") else 200
            handler.answer(put)
        return True
    if what == "example" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (example(project, body.get("template")), 200))[1])
        return True
    if what == "draft" and method == "POST":
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            def ask():
                exists()
                try:
                    return draft(project, str(body.get("description", ""))), 200
                except models.Refusal as e:
                    return {"message": str(e)}, 422
            handler.answer(ask)
        return True
    return False
