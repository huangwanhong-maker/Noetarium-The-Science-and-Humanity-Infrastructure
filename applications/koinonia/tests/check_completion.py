"""Checks of the completion re-founded at a node (Phase 12 of the design of 2026-11-27, Section 11.4; ADR 0176).

The founding and the carrying run on a copy of the first attempt's folder and a temporary store, so that the real folder and the node's
real store are never touched here. What the carrying should have done is counted again from the copy's files by the small readers
below, written apart from ``carry_completion.py``; no model is asked.
"""

from __future__ import annotations

import hashlib
import http.client
import importlib.util
import json
import shutil
import sys
import tempfile
import threading
from http.server import ThreadingHTTPServer
from pathlib import Path

APP = Path(__file__).resolve().parents[1]
ROOT = APP.parents[1]
sys.path[:0] = [str(APP), str(APP / "tests"), str(ROOT / "api" / "python"), str(ROOT / "data" / "products")]

import carry_completion  # noqa: E402
from koinonia import book, completion, passports  # noqa: E402
from koinonia.gateway import Gateway  # noqa: E402
from koinonia.node import Node  # noqa: E402
from science import archive  # noqa: E402

FIRST = ROOT / "docs" / "reports" / "2026-11-26-koinonia-book" / "completion"


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "koinonia completion: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:700]})


if not (FIRST / "plan.json").is_file() or not (FIRST / "journal.jsonl").is_file():
    emit({"kind": "skip", "name": "koinonia completion: the first attempt's folder is not here"})
    sys.exit(0)

tmp = Path(tempfile.mkdtemp(prefix="koinonia-completion-"))
copy = tmp / "completion"
shutil.copytree(FIRST, copy, ignore=shutil.ignore_patterns("__pycache__", "journal.lock", "*.pyc"))
store = tmp / "node"


# ---- Independent readers of the copy (before the carrying touches it) ----------------------------------------------------------------

def sha(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


def chain(lines: list[str]) -> tuple[bool, str]:
    prev = "0" * 64
    for line in lines:
        e = json.loads(line)
        s = e.pop("seal")
        if e["previous"] != prev or hashlib.sha256(json.dumps(e, ensure_ascii=False, sort_keys=True, separators=(",", ":")).encode("utf-8")).hexdigest() != s:
            return False, prev
        prev = s
    return True, prev


plan = json.loads((copy / "plan.json").read_text(encoding="utf-8"))
written = {p.stem: p for p in (copy / "sections").glob("*.json")}
checked, rule = {}, {}
for sid, p in written.items():
    body = json.loads(p.read_text(encoding="utf-8"))
    audit = json.loads((copy / "audit" / f"{sid}.json").read_text(encoding="utf-8"))
    d = book.digest(body)
    if audit.get("ok") and audit.get("digest") == d:
        checked[sid] = sha(p)
    rv_path = copy / "reviews" / f"{sid}.json"
    rv = json.loads(rv_path.read_text(encoding="utf-8")) if rv_path.is_file() else None
    rule[sid] = bool(rv) and sid in checked and rv.get("of version") == d and rv.get("verdict") in ("adopt", "minor") and \
        (rv.get("by") or {}).get("provider") not in (None, "", (body.get("by") or {}).get("provider"))
reviews = {p.stem: sha(p) for p in (copy / "reviews").glob("*.json")}
lines_before = (copy / "journal.jsonl").read_text(encoding="utf-8").splitlines()
held, head_before = chain(lines_before)
with_digest = sum(1 for line in lines_before if any(json.loads(line).get("data", {}).get(k) for k in ("digest", "of version"))
                  if isinstance(json.loads(line).get("data"), dict))
names = {str(v.get("run by")) for v in json.loads((copy / "participants.json").read_text(encoding="utf-8")).values() if v.get("run by")}
assert_("Section 11.4's counts read from the first attempt's files: 129 sections in the plan, 123 checked and holding, 90 standing reviews, "
        "and its journal's chain verified", len(plan) == 129 and len(checked) == 123 and len(reviews) == 90 and held and len(lines_before) >= 7172,
        (len(plan), len(checked), len(reviews), held, len(lines_before)))

# ---- The founding and the carrying, on the copy and a temporary store ----------------------------------------------------------------

retired_before = carry_completion.RETIRED in (copy / "serve.py").read_text(encoding="utf-8")  # the real first attempt, once carried
report = carry_completion.run(copy, store, "http://127.0.0.1:9239/", "2026-11-28", society=None)
node = Node(store, url="http://127.0.0.1:9239/")  # opened again from the store, as the node's server would open it
C = completion.COMMUNITY
record = node.hosted(C).record
deposits = {a["fields"]["holding"]: a["fields"] for a in record if a["kind"] == "deposit"}
carried = {f["section"]: f for f in deposits.values() if f.get("within") in ("adopted", "first attempt") and f.get("section") and f.get("version")}
posted = {a["fields"]["id"]: a["fields"] for a in record if a["kind"] == "request" and a["fields"].get("work")}

replays = []
for name in (completion.HOUSEHOLD, completion.LEAGUE, C):
    try:
        s = node.session(name)
        replays.append(s.opened.get("replays") is not False and not s.opened["refused"])
    except ValueError as error:
        replays.append(str(error))
assert_("the completion, the league and the author's household open again from the store, each record replaying as sealed",
        replays == [True, True, True], replays)
assert_("129 items or carried sections in all: every section of the plan is carried or posted as work, and nothing else",
        set(carried) | set(posted) == set(plan), sorted(set(plan) ^ (set(carried) | set(posted)))[:6])
assert_("123 carried sections whose digests equal the files'", len(carried) == 123 and all(carried[s]["digest"] == checked.get(s) for s in carried),
        [s for s in carried if carried[s]["digest"] != checked.get(s)][:4])
carried_reviews = {f["section"]: f for f in deposits.values() if f.get("within") == "reviews"}
assert_("90 carried reviews within reviews, each its file's digest, verdict, author and the version it bears on",
        len(carried_reviews) == 90 and all(carried_reviews[s]["digest"] == reviews[s] and carried_reviews[s].get("verdict") and carried_reviews[s].get("bears on")
                                           for s in carried_reviews), len(carried_reviews))
by = {s: json.loads(f.get("by", "{}")) for s, f in carried.items()}
assert_("every carried section names its author with agent, model and provider, run by the author",
        len(by) == 123 and all(b.get("agent") and b.get("model") and b.get("provider") and b.get("run by") == "the author" for b in by.values()),
        [s for s, b in by.items() if not (b.get("agent") and b.get("model") and b.get("provider"))][:4])
blob = json.dumps({"record": record, "journal": node.hosted(C).journal}, ensure_ascii=False)
assert_("no person is named in the completion's record or series: the person who ran the first attempt is the author",
        names and not any(n in blob for n in names), [n for n in names if n in blob][:1] and "a name appears")

bag = store / "communities" / C / "first-attempt"
files = {p.relative_to(bag).as_posix(): archive.path(p) for p in bag.rglob("*") if p.is_file()}
v = archive.ask("validate", timeout=900.0, files=files)
bag_digest = sha(bag / "tagmanifest-sha256.txt") if (bag / "tagmanifest-sha256.txt").is_file() else sha(bag / "manifest-sha256.txt")
in_bag = (bag / "data" / "journal.jsonl").read_text(encoding="utf-8").splitlines() if (bag / "data" / "journal.jsonl").is_file() else []
assert_("the bag is made and validated by archeion, its digest the holding's, its journal the first attempt's as it stood and the journal's holding",
        v.get("valid") and deposits.get(carry_completion.BAG_HOLDING, {}).get("digest") == bag_digest and in_bag == lines_before and
        deposits.get(carry_completion.JOURNAL_HOLDING, {}).get("digest") == sha(bag / "data" / "journal.jsonl"), (v.get("valid"), len(in_bag)))

carried_eps = [e for e in node.hosted(C).journal if "first attempt: " in json.dumps(e, ensure_ascii=False)]
graded_a = sum(1 for e in carried_eps if "feature.grade:1:A" in e.get("about", ""))
assert_("the journal's entries are episodes of the completion's archive, one each, grade A exactly where they carry a digest",
        len(carried_eps) == len(lines_before) and graded_a == with_digest, (len(carried_eps), len(lines_before), graded_a, with_digest))
seals = [json.loads(line)["seal"] for line in lines_before]
assert_("each carried episode names its entry's seal, in the journal's order",
        len(carried_eps) == len(seals) and all(s in e.get("about", "") for s, e in zip(seals, carried_eps)), carried_eps[:1])

st = node.state(C)
prop = next((p for p in st.get("proposals", []) if p["id"] == carry_completion.DECISION), {})
listed = json.loads(next(a["fields"]["accepted"] for a in record if a["kind"] == "propose" and a["fields"].get("id") == carry_completion.DECISION))
expected = sorted(s for s, ok in rule.items() if ok)
assert_("the decision accept carried work passes by unanimity of the principals, its list the author's rule read again from the files",
        prop.get("passes") and prop.get("rule") == "unanimity" and sorted(x["section"] for x in listed) == expected and len(expected) == 56,
        (prop.get("passes"), len(listed), len(expected)))
yes = next(s for s in expected)
no = next(s for s in sorted(checked) if not rule[s])
assert_(f"a carried section the rule accepts ({yes}) is kept within adopted and posted again nowhere; one it refuses ({no}) is kept within the first "
        "attempt and posted as a section whose specification is the carried version",
        carried[yes]["within"] == "adopted" and yes not in posted and carried[no]["within"] == "first attempt" and posted.get(no, {}).get("spec") == f"{no} {carried[no]['version']}",
        (carried[yes]["within"], carried[no]["within"], posted.get(no)))

g = Gateway(node)
code, open_work, _, _ = g.handle("GET", f"/c/{C}/work", {"state": "open"}, {}, None, "127.0.0.1")
code2, waiting, _, _ = g.handle("GET", f"/c/{C}/work", {"state": "waiting"}, {}, None, "127.0.0.1")
unwritten = sorted(set(plan) - set(written) - {"c01-1"})
open_ids = {w["id"] for w in open_work.get("work", [])}
assert_("the posted work is open on the gateway without a credential: the five sections never written and every carried section the rule refuses; "
        "the overview waits for every section to be accepted",
        code == 200 and open_ids == set(unwritten) | {s for s in checked if not rule[s]} and len(unwritten) == 5 and
        [w["id"] for w in waiting.get("work", [])] == ["c01-1"], (len(open_ids), unwritten, waiting))
entries = report["listing"]["entry"]
assert_("the completion is listed in the platform league's registry, checked by another origin, and the listing verifies as current and claimed",
        entries["status"] == "listed" and entries["checked by"] == completion.AUTHOR and entries["deposited by"] == C and
        report["listing"]["verified"]["status"] == "current" and report["listing"]["verified"]["claimed"], report["listing"])
assert_("the steward is briefed: it holds the steward's office under its passport, and its briefing lists the acts it may take",
        any(p["position"] == "steward" for p in report["steward"]["positions"]) and {"request", "accept", "deposit"} <= set(report["steward"]["acts"]),
        report["steward"])

lines_after = (copy / "journal.jsonl").read_text(encoding="utf-8").splitlines()
held_after, _ = chain(lines_after)
last = json.loads(lines_after[-1])
assert_("the first attempt's journal is closed by one final entry under its own sealing rule, naming the bag's digest and the community; no other entry changed",
        held_after and lines_after[:-1] == lines_before and last["act"] == "close" and last["data"]["bag"]["sha256"] == bag_digest and
        last["data"]["carried to"] == C and not any(n in lines_after[-1] for n in names), last.get("data"))

spec = importlib.util.spec_from_file_location("first_attempt_serve", copy / "serve.py")
served = importlib.util.module_from_spec(spec)
spec.loader.exec_module(served)
srv = ThreadingHTTPServer(("127.0.0.1", 0), served.Handler)
threading.Thread(target=srv.serve_forever, daemon=True).start()
answers = []
for method in ("GET", "POST"):
    conn = http.client.HTTPConnection("127.0.0.1", srv.server_address[1], timeout=10)
    conn.request(method, "/api/briefing", body=b"{}" if method == "POST" else None)
    r = conn.getresponse()
    answers.append((r.status, r.getheader("Location"), json.loads(r.read().decode("utf-8")).get("address")))
srv.shutdown()
address = served.MOVED_TO if retired_before else f"http://127.0.0.1:9239/c/{C}/profile"
assert_("the first attempt's server answers every route with 410 Gone and the completion's address (the node's that carried it)",
        address.endswith(f"/c/{C}/profile") and answers == [(410, address, address), (410, address, address)], answers)

# An independent writer in this temporary node goes through the same routes as a book agent.
writer = passports.issue(node, completion.HOUSEHOLD, completion.AUTHOR,
                         {"operative": "author/codex-a", "model": "test", "communities": [C], "matters": ["the book"]})
headers = {"Authorization": "Bearer " + writer["credential"]}
def request(path, body, auth=headers):
    status, response, _, _ = g.handle("POST", f"/c/{C}/" + path, {}, auth, body, "127.0.0.1")
    return status, response

presented, _ = request("present", {"passport": writer["passport"]})
applied, _ = request("acts", {"kind": "apply", "fields": {"id": "consult-application", "position": "writer"}})
sid = "c06-2"
path = f"holdings/results%20{sid}/consult"
before, anonymous = request(path, {"purpose": "writing"}, {})
code, data = request(path, {"purpose": "writing"})
expected_file = store / "communities" / C / "first-attempt" / "data" / "results" / (sid + ".json")
original_bytes = expected_file.read_bytes()
original = json.loads(original_bytes)
sample_key = next(iter(original))
assert_("a real named result is consulted through its percent-encoded route only after admission; the carried file digest, fact keys "
        "and numerical facts hold, and the private runner is absent from the writer's view",
        presented == applied == code == 201 and before == 401 and "content" not in anonymous and
        data.get("source sha256") == hashlib.sha256(original_bytes).hexdigest() and data.get("content") == carry_completion.scrub(original, names)
        and data["content"][sample_key]["facts"] == original[sample_key]["facts"], (presented, applied, before, code, data.get("content status")))
code, raw_act = request("acts", {"kind": "consult", "fields": {"holding": carried[sid]["holding"], "purpose": "writing"}})
version = json.loads(written[sid].read_bytes())
assert_("the generic consult returns the carried version with its writing, claims and sealed author attribution",
        code == 201 and raw_act.get("content") == carry_completion.scrub(version, names)
        and raw_act.get("source sha256") == carried[sid]["digest"], (code, raw_act.get("content status")))
code, twice = request(f"holdings/results%2520{sid}/consult", {"purpose": "writing"})
assert_("a literal percent escape in an identifier is decoded once", code == 403 and "content" not in twice, code)
code, compound = request(f"holdings%2Fresults%20{sid}/consult", {"purpose": "writing"})
assert_("an encoded delimiter stays in its original path segment", code == 404 and "content" not in compound, code)

# Change only the test bag to check detection between two admitted consults.
expected_file.write_bytes(b"{}")
code, different = request(path, {"purpose": "writing"})
assert_("a changed archive file returns an explicit unavailable content result while preserving the admission receipt",
        code == 201 and different.get("admitted") and different.get("content") is None
        and "differs" in different.get("content status", "") and different.get("source sha256") == data.get("source sha256"), code)
shutil.rmtree(tmp, ignore_errors=True)
