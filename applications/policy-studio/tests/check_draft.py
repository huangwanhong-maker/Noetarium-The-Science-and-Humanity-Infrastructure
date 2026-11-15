"""The Policy Studio's use of a language model, with a stand-in (no model is called): a theory drafted for a project is kept
beside its question as a proposal and changes nothing of the question; a draft that does not read is refused and nothing is kept;
the draft takes the levels of the question's own scheme; and a key is never given to a page. Prints one JSON line a check;
run-tests.mjs reads them.

    python applications/policy-studio/tests/check_draft.py
"""

from __future__ import annotations

import json
import sys
import tempfile
from pathlib import Path

APP = Path(__file__).resolve().parents[1]
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

import questions  # noqa: E402
from science import models, policy  # noqa: E402
from science import studio_host as host  # noqa: E402


def say(name, ok, detail=""):
    print(json.dumps({"name": name, "ok": bool(ok), "detail": "" if ok else str(detail)[:500]}, ensure_ascii=False), flush=True)


if not policy.ENGINE.status()["built"]:
    print(json.dumps({"name": "the policy engine is not built", "skip": True}))
    sys.exit(0)

DRAFT = {"name": "Free school meals", "nodes": [{"id": "meals", "kind": "intervention", "text": "A free meal for every pupil"}, {"id": "attendance", "kind": "outcome", "text": "Pupils attend more days"},
                                               {"id": "learning", "kind": "impact", "text": "Pupils learn more"}],
         "links": [{"from": "meals", "to": "attendance", "assumption": "families send children who would stay home", "support": ["the meal is served before the first lesson"], "sign": "+"},
                   {"from": "attendance", "to": "learning", "assumption": "days in school are days taught", "support": [], "sign": "+"}]}

with tempfile.TemporaryDirectory() as folder:
    host.STORE["dir"] = Path(folder)
    project = host.project_dir("meals")
    project.mkdir(parents=True)
    (project / "project.json").write_text("{}", encoding="utf-8")
    laid = questions.example("meals", "schooling/free-school-meals")
    before = questions.read_question("meals")
    say("the invented question of school meals is laid out in the project", laid["template"] == "schooling/free-school-meals" and before["schema"] == "noetarium.policy/1" and len(before["bodies"]) == 6, laid)

    stub = models.Stub(DRAFT)
    got = questions.draft("meals", "Free meals at school, so that pupils attend and learn more.", model=stub)["draft"]
    kept = questions.drafts("meals")
    say("a model's draft is kept beside the question as a proposal of grade C, with who drafted it, what was asked and when",
        len(kept) == 1 and kept[0]["grade"] == "C" and kept[0]["status"] == "proposed" and kept[0]["by"] == "a stand-in, stub" and kept[0]["description"].startswith("Free meals at school") and kept[0]["at"].endswith("Z"), kept)
    say("the question is not changed by it: a draft is the theory only when a person takes it", questions.read_question("meals") == before)
    say("the draft carries no evidence: each of its claims stands as not shown", all(l["rank"] == -1 and not l["evidence"]["given"] for l in got["theory"]["links"]) and "evidence" not in got["text"], got["text"])
    say("it is written on the levels of the question's own scheme", got["theory"]["levels"] == ["very_low", "low", "moderate", "high"] and "levels very_low low moderate high" in got["text"])
    say("the model is told that it is not a source, and is asked for no effect or grade", "you are not a source" in stub.asked[0]["system"] and "effect" not in json.dumps(stub.asked[0]["schema"]["properties"]))

    cyclic = {**DRAFT, "links": DRAFT["links"] + [{"from": "learning", "to": "meals", "assumption": "x", "support": [], "sign": "+"}]}
    try:
        questions.draft("meals", "Free meals at school, so that pupils attend and learn more.", model=models.Stub(cyclic))
        refused = ""
    except models.Refusal as error:
        refused = str(error)
    say("a draft with a cycle is refused by the domain's reader, and nothing more is kept", "does not read" in refused and "cycle" in refused and len(questions.drafts("meals")) == 1, refused)

    listed = questions.schemes()
    say("the plug-ins' grading schemes each say that they are examples and no institution's own text", len(listed) == 2 and all("example" in s["title"].lower() and s["text"].startswith("scheme ") and s["rules"] for s in listed), [s["title"] for s in listed])
    say("a path within the person's home is told to a page from there, without the account's name", questions.shown(Path.home() / "noetarium" / "models.json") == "~/noetarium/models.json")
