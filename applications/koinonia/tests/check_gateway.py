"""Checks of Koinonia's gateway (Phase 10 of the design of 2026-11-27, Sections 8 and 9), run by applications/koinonia/tests/run-tests.mjs.
No model is asked. One JSON object a line.

The protocol of an outside agent as Section 8 requires it: discovery without a credential; a passport issued at a household, its credential
shown once; presentation and each of its refusals; application; taking, delivering and review by another provider; questions and rulings;
petitions; transmission refused with the route shown; a tool run by the host; exit with the record; the limits; a race on one item.

    python applications/koinonia/tests/check_gateway.py
"""

from __future__ import annotations

import copy
import hashlib
import json
import sys
import tempfile
import threading
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from nodecase import TOOL, Case  # noqa: E402

from koinonia import registry, tools  # noqa: E402
from koinonia.gateway import Gateway  # noqa: E402
from science import communities  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "koinonia gateway: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:700]})


folder = Path(tempfile.mkdtemp(prefix="koinonia-gateway-"))
K = Case(folder)
node, call = K.node, K.call
series = lambda c: node.hosted(c).journal  # noqa: E731

# ---- discovery without a credential ------------------------------------------------------------------------------------------------
st, guide, kind, _ = call("GET", "/")
assert_("the node's guide for agents is Markdown holding the one prompt, the routes and the statuses", st == 200 and kind == "markdown" and
        "You act for `<principal>`" in guide and "/record/mine" in guide and "503" in guide, guide[:200])
st, known, _, _ = call("GET", "/.well-known/noetarium.json")
by = {x["name"]: x for x in known.get("communities", [])}
assert_("/.well-known/noetarium.json names the release, the notary and each community hosted with its order, profile address and head",
        st == 200 and known["format"] == "noetarium.node/1" and known["notary"] == "notary" and by["completion"]["order"] == "organisation" and
        by["completion"]["profile"].endswith("/c/completion/profile") and by["completion"]["head"]["size"] >= 4, known)
before = len(series("completion"))
st, doc, kind, _ = call("GET", "/c/completion/profile")
listed = registry.listing(node, "completion", "2026-12-03")
assert_("the profile is served byte for byte as the registries write it: a listing made from the community names the SHA-256 of the bytes served",
        st == 200 and kind == "raw" and json.loads(doc)["format"] == "noetarium.profile/1" and
        listed["listing"]["profile"]["sha256"] == hashlib.sha256(doc.encode("utf-8")).hexdigest(), listed["listing"].get("profile"))
st, routes, _, _ = call("GET", "/c/completion/routes")
st2, work, _, _ = call("GET", "/c/completion/work", query={"state": "open"})
assert_("the routes and the open work are read without a credential", st == 200 and routes["routes"]["questions"]["body"] == "stewards" and
        st2 == 200 and {w["id"] for w in work["work"]} == {"s-1", "s-2"}, (routes, work))
st, _, _, _ = call("GET", "/c/nowhere/profile")
public_reads = [e for e in series("completion")[before:] if e["who"] == "public" and e["what"] == "episode"]
assert_("a community not hosted is 404; every public read is an episode of the community's series, made by the public, its route and status kept",
        st == 404 and len(public_reads) == 3 and all("feature.status:3:200" in e["about"] and "feature.address:5:local" in e["about"] for e in public_reads),
        [e["about"] for e in public_reads])
st, opened, _, _ = call("GET", "/c/completion/holdings/plan", query={"content": "1"})
st2, shut, _, _ = call("GET", "/c/completion/holdings/sections", query={"content": "1"})
assert_("a public holding's content is given where a norm lets anyone consult it with transmission anywhere; a members' holding is not described to the public",
        st == 200 and opened["opened"] and opened["content"] == {"sections": ["s-1", "s-2"]} and st2 == 404, (opened, shut))

# ---- passports issued at a household -----------------------------------------------------------------------------------------------
st, codex, _, _ = K.issue(K.principal_a, K.home_a, "author/codex", "gpt-6", communities=("completion", "atelier"))
kept = "".join(p.read_text(encoding="utf-8", errors="replace") for p in folder.rglob("*") if p.is_file())
assert_("a principal issues a passport at its household: the credential shown once, only its digest in the mandate, nowhere in the node's files",
        st == 201 and codex["passport"]["format"] == "noetarium.passport/1" and
        codex["passport"]["mandate"]["credential"] == hashlib.sha256(codex["credential"].encode()).hexdigest() and codex["credential"] not in kept and
        codex["passport"]["issuer"]["witnesses"][0]["notary"] == "notary", codex)
st, refused, _, _ = K.issue(K.principal_a, K.home_a, "author/stranger", "gpt-6")
st2, wrong, _, _ = K.issue(K.principal_b, K.home_a, "author/codex", "gpt-6")
assert_("a household issues no passport for an operative it did not declare, and none under another household's principal",
        st == 403 and "household declared" in refused["refused"] and st2 == 401, (refused, wrong))
_, gem, _, _ = K.issue(K.principal_a, K.home_a, "author/gem", "gemini-3")
_, claude, _, _ = K.issue(K.principal_b, K.home_b, "bea/claude", "claude-opus-5-5")
_, late, _, _ = K.issue(K.principal_b, K.home_b, "bea/late", "claude-opus-5-5", term={"since": "2026-12-01T00:00:00Z", "until": "2026-12-04T00:00:00Z"})
_, far, _, _ = K.issue(K.principal_a, K.home_a, "author/far", "gpt-6", communities=("elsewhere",))
CODEX, GEM, CLAUDE, LATE, FAR = (x["credential"] for x in (codex, gem, claude, late, far))

# ---- presentation and its refusals -------------------------------------------------------------------------------------------------
st1, r1, _, _ = call("POST", "/c/completion/present", {"passport": codex["passport"]})
st2, r2, _, _ = call("POST", "/c/completion/present", {"passport": codex["passport"]}, GEM)
assert_("a presentation without a credential, or with another passport's, is refused as 401 before the fold", st1 == 401 and st2 == 401 and
        "not the passport's" in r2["refused"], (r1, r2))
altered = copy.deepcopy(codex["passport"])
altered["mandate"]["allowance"]["bench runs"] = 5000
st, r, _, _ = call("POST", "/c/completion/present", {"passport": altered}, CODEX)
assert_("a passport altered after its issue is refused by the host's fold, the reason naming the home's head", st == 403 and "head" in r["refused"], r)
st, r, _, _ = call("POST", "/c/completion/present", {"passport": far["passport"]}, FAR)
assert_("a passport whose mandate does not name the host is refused there", st == 403 and "completion" in r["refused"], r)
st, r, _, _ = call("POST", "/c/atelier/present", {"passport": codex["passport"]}, CODEX)
assert_("a host that recognises no notary at the level of evidence refuses a passport witnessed by one, as its part passports declares", st == 403 and
        "notary" in r["refused"], r)
st, r, _, _ = call("POST", "/c/completion/present", {"passport": "not a passport"}, CODEX)
assert_("a body that names no passport is 422", st == 422, r)
st, r, _, _ = call("POST", "/c/completion/present", {"passport": codex["passport"]}, CODEX)
declared = {p["id"]: p for p in node.known("completion")}
assert_("the passport presented: the office of status reported the notary's head, the agent and its principal are known parties with provider, holding "
        "no position, and the mandate stands as checked at its home",
        st == 201 and r["status"] == "standing" and declared.get("author/codex", {}).get("provider") == "openai" and
        not any(p["party"] == "author/codex" for p in node.state("completion")["positions"]) and
        any(a["kind"] == "report" and a["fields"].get("head of") == "notary" for a in node.hosted("completion").record), r)
K.clock.advance(2 * 86400)  # 2026-12-05: bea/late's term has ended
st, r, _, _ = call("POST", "/c/completion/present", {"passport": late["passport"]}, LATE)
assert_("a passport presented after its term is refused", st == 403 and "ended" in r["refused"], r)

# ---- briefing and application ------------------------------------------------------------------------------------------------------
st, b, _, _ = call("GET", "/c/completion/briefing", token=CODEX)
reads = {(e["holding"], e["aim"]): e for e in b.get("holdings", [])}
assert_("the briefing (noetarium.briefing/1) says who the agent is and answers for it, its mandate as checked, the routes, the ground and how to leave",
        st == 200 and b["format"] == "noetarium.briefing/1" and b["you"]["provider"] == "openai" and b["you"]["answers for you"] == "author" and
        b["you"]["mandate"]["remaining"] == {"bench runs": 50} and b["routes"]["questions"]["body"] == "stewards" and
        any(g.startswith("G8′") for g in b["law"]["ground"]) and b["leaving"]["record"].endswith("/record/mine"), b.get("you"))
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "apply", "fields": {"id": "ap-1", "position": "writer"}}, CODEX)
assert_("an application for writer is decided by the position's rule in the same act: admitted while sections are open", st == 201 and
        r["item"] == {"id": "ap-1", "state": "admitted"}, r)
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "apply", "fields": "writer"}, CODEX)
st2, r2, _, _ = call("POST", "/c/completion/acts", ["not", "an", "object"], CODEX)
st3, r3, _, _ = call("POST", "/c/completion/acts", {"kind": "apply", "fields": {"position": "writer"}}, CODEX)
assert_("a body that does not read is 422 with the fields it lacks; an act that reads and fails a declared condition is the fold's 403, naming it",
        st == 422 and st2 == 422 and r["fields"] == ["kind", "fields"] and st3 == 403 and "condition of apply" in r3["refused"], (st, r, st2, r3))

# ---- transmission refused with the route shown -------------------------------------------------------------------------------------
call("POST", "/c/completion/present", {"passport": gem["passport"]}, GEM)
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "apply", "fields": {"id": "ap-2", "position": "writer"}}, GEM)
st2, r2, _, _ = call("POST", "/c/completion/holdings/sections/consult", {"purpose": "writing"}, GEM)
assert_("an operative of a provider the norm does not name is refused the sections: the norm named, transmission the condition failed, the route for "
        "a petition of transmission shown, and what it may do instead listed",
        st == 201 and st2 == 403 and r2["norm"] == "sections read by members" and r2["conditions failed"] == ["transmission"] and
        "petition {kind: transmission}" in r2["route"]["if you need this"] and isinstance(r2.get("admissible"), list), r2)
st, r, _, _ = call("POST", "/c/completion/holdings/plan/consult", {"purpose": "writing"}, CODEX)
assert_("a consultation admitted gives the holding's content", st == 201 and r["content"] == {"sections": ["s-1", "s-2"]}, r)

draft = "The cohort's working text.\n"
draft_digest = hashlib.sha256(draft.encode("utf-8")).hexdigest()
st, _, _, _ = call("POST", "/c/completion/acts", {"kind": "deposit", "fields": {
    "holding": "the working text", "within": "sections", "kind": "document", "digest": draft_digest, "source": "the writer"}, "body": draft}, CODEX)
st2, allowed, _, _ = call("POST", "/c/completion/holdings/the%20working%20text/consult", {"purpose": "writing"}, CODEX)
st3, refused, _, _ = call("POST", "/c/completion/acts", {"kind": "consult", "fields": {
    "holding": "the working text", "purpose": "writing"}}, GEM)
assert_("a member reads the sealed draft body under its digest; an excluded provider receives only the fold's refusal",
        st == st2 == 201 and allowed.get("content") == draft and allowed.get("source sha256") == draft_digest and
        st3 == 403 and "content" not in refused, (st, st2, st3))

# ---- taking and delivering; a race on one item --------------------------------------------------------------------------------------
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "commit", "fields": {"id": "s-1~codex", "to": "keeper", "item": "s-1"}}, CODEX)
assert_("a writer takes an open section by a commitment naming it", st == 201 and r["item"]["state"] == "taken", r)
results = {}


def race(who, token):
    results[who] = call("POST", "/c/completion/acts", {"kind": "commit", "fields": {"id": f"s-2~{who}", "to": "keeper", "item": "s-2"}}, token)


threads = [threading.Thread(target=race, args=(w, t)) for w, t in (("codex", CODEX), ("gem", GEM))]
for t in threads:
    t.start()
for t in threads:
    t.join()
codes = sorted(r[0] for r in results.values())
takers = [t for w in node.session("completion").work().get("work", []) if w["id"] == "s-2" for t in w["takings"]]
assert_("two writers commit to one item at once: one is taken (201), the other answered 409 with the reason, and the item has one taker",
        codes == [201, 409] and len(takers) == 1 and any("is taken" in r[1].get("refused", "") for r in results.values()), (codes, takers))
digest = hashlib.sha256(b"section s-1, first version").hexdigest()
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "deliver", "fields": {"id": "s-1~codex", "artefact": digest}}, CODEX)
K.local("completion", "keeper", "attest", {"item": "s-1", "result": "pass"})
assert_("the writer delivers the section by its digest, and the steward enters the kind's check", st == 201 and r["item"]["state"] == "delivered", r)

# ---- review by another provider ----------------------------------------------------------------------------------------------------
call("POST", "/c/completion/present", {"passport": claude["passport"]}, CLAUDE)
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "apply", "fields": {"id": "ap-3", "position": "reviewer"}}, CLAUDE)
st_own, own, _, _ = call("POST", "/c/completion/acts", {"kind": "assess", "fields": {"object": "s-1", "criterion": "review", "value": "adopt"}}, CODEX)
st2, r2, _, _ = call("POST", "/c/completion/acts", {"kind": "assess", "fields": {"object": "s-1", "criterion": "review", "value": "adopt"}}, CLAUDE)
acc = K.local("completion", "keeper", "accept", {"id": "s-1~codex"})
s1 = next(w for w in node.session("completion").work()["work"] if w["id"] == "s-1")
assert_("a reviewer of another provider is admitted while a delivery awaits its provider, its review admitted where the writer's own is refused, and the "
        "section accepted with a review by another provider than its writer's (Proposition 19)",
        st == 201 and st_own == 403 and st2 == 201 and acc.get("admitted") and s1["state"] == "accepted" and
        all(rv.get("provider") != "openai" for rv in s1["reviews"]) and s1["deliveries"][-1]["provider"] == "openai", (r, own, r2, acc, s1))

# ---- questions and rulings, petitions -----------------------------------------------------------------------------------------------
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "question", "fields": {"id": "q-1", "about": "norm", "subject": "claims cite facts"}}, CODEX)
K.local("completion", "keeper", "answer", {"id": "q-1", "outcome": "ruled", "reading": "every number cites a fact"})
st2, r2, _, _ = call("POST", "/c/completion/acts", {"kind": "question", "fields": {"id": "q-2", "about": "norm", "subject": "claims cite facts"}}, GEM)
_, rulings, _, _ = call("GET", "/c/completion/rulings")
assert_("a question opens on its route and is ruled; a second on the same subject is refused with the ruling cited and the route of review shown; the "
        "register is public",
        st == 201 and st2 == 403 and "ruling q-1" in r2["refused"] and "claim against the ruling" in r2["route"]["if you contest it"] and
        any(x["id"] == "q-1" for x in rulings["rulings"]), (r, r2))
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "petition", "fields": {"id": "pe-1", "kind": "norm", "about": "a term for reviews"}}, CODEX)
st2, item, _, _ = call("GET", "/c/completion/items/pe-1", token=CODEX)
st3, _, _, _ = call("GET", "/c/completion/items/pe-1", token=CLAUDE)
assert_("a petition opens on its route; its state is shown to its author and not to another party", st == 201 and st2 == 200 and item["status"] == "open" and
        st3 == 403, (r, item))

# ---- a tool run by the host ---------------------------------------------------------------------------------------------------------
tools.declare(node, "completion", "mean", TOOL, reads=["shelf"])
size = node.session("completion").head["size"]
st, r, _, _ = call("POST", "/c/completion/tools/mean/run", {"params": {}, "into": "sources of facts", "item": "s-2"}, CODEX)
kinds = [a["kind"] for a in node.hosted("completion").record[size:]]
ran = [e for e in series("completion") if e["what"] == "episode" and "operation:8:run tool" in e["about"]]
assert_("a tool runs for a writer: the uses of the tool and of the runs it draws and the deposit of the result, each constituted by the run's episode, its "
        "result kept by its digest", st == 201 and r["result"]["facts"] == {"n": 4, "mean": 5.0} and kinds == ["use", "use", "deposit"] and len(ran) == 3 and
        node.hosted("completion").payloads.get(r["digest"])["result"]["facts"]["mean"] == 5.0, (r, kinds))
st, r, _, _ = call("POST", "/c/completion/tools/mean/run", {"params": {}, "into": "adopted"}, CODEX)
st2, r2, _, _ = call("POST", "/c/completion/tools/none/run", {"params": {}, "into": "sources of facts"}, CODEX)
assert_("a run whose result could not be kept is not run, and an undeclared tool is 404", st == 403 and "could not be kept" in r["refused"] and st2 == 404, (r, r2))

# ---- events and the mandate ----------------------------------------------------------------------------------------------------------
st, ev, _, _ = call("GET", "/c/completion/events", token=CODEX, query={"after": "0", "wait": "0"})
st2, m, _, _ = call("GET", "/c/completion/mandate", token=CODEX)
assert_("the agent's events are the acts that concern it (its own and those on its items), and its mandate shows the runs used", st == 200 and
        any(e["kind"] == "accept" for e in ev["events"]) and any(e["author"] == "author/codex" for e in ev["events"]) and
        m["chain"][0]["used"] == {"bench runs": 1}, (len(ev.get("events", [])), m.get("chain")))

# ---- the limits -------------------------------------------------------------------------------------------------------------------------
node.rate = 2
codes = [call("GET", "/c/completion/profile", address="10.0.0.9")[0] for _ in range(3)]
node.rate = 120
h = node.hosted("completion")
h.received[h.turn] = int(h.constitution["receiving"]["per round"])
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "deliver", "fields": {"id": "s-2~codex", "artefact": digest}}, CODEX)
node.tick("completion")
st2, r2, _, _ = call("POST", "/c/completion/acts", {"kind": "say", "fields": {"matter": "the book"}}, CODEX)
assert_("the node's rate per origin and the community's receiving capacity (deliveries a round, as its unit says) are each refused with 429 and the time to retry; the next round receives again",
        codes == [200, 200, 429] and st == 429 and r["retry after"] == "the next round" and st2 == 201, (codes, st, r, st2))
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "say", "fields": {"matter": "the book"}}, CODEX, address="10.0.0.9")
opened_gw = Gateway(node, open_to_agents=True)
st2, r2, _, _ = call("POST", "/c/completion/acts", {"kind": "say", "fields": {"matter": "the book"}}, CODEX, address="10.0.0.9", gateway=opened_gw)
assert_("credentialed acts from another address are refused unless the node was started with --open-to-agents", st == 403 and "open-to-agents" in r["refused"] and
        st2 == 201, (r, r2))

# ---- revocation at home, a home that cannot be reached -----------------------------------------------------------------------------------
st, r, _, _ = call("POST", f"/c/{K.home_a}/revoke", {"mandate": gem["mandate"]}, K.principal_a)
K.clock.advance(node.window + 1)
st2, r2, _, _ = call("POST", "/c/completion/acts", {"kind": "say", "fields": {"matter": "the book"}}, GEM)
reported = [a for a in node.hosted("completion").record if a["kind"] == "report" and a["fields"].get("mandate status") == gem["mandate"]]
assert_("the principal revokes at home; at the host's next check of status, entered as the office's report, every act under the mandate is refused",
        st == 201 and st2 == 403 and "revoked" in r2["refused"] and reported[-1]["fields"]["status"] == "revoked" and reported[-1]["fields"].get("episode"), (r, r2))
held = node.communities.pop(K.home_b)
K.clock.advance(node.window + 1)
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "say", "fields": {"matter": "the book"}}, CLAUDE)
node.communities[K.home_b] = held
K.clock.advance(node.window + 1)
st2, _, _, _ = call("POST", "/c/completion/acts", {"kind": "say", "fields": {"matter": "the book"}}, CLAUDE)
assert_("a mandate whose home cannot be reached is suspended at the host (503) until the home can be reached again", st == 503 and "cannot be reached" in r["refused"] and
        st2 == 201, (r, st2))

# ---- exit with the record --------------------------------------------------------------------------------------------------------------
st, r, _, _ = call("POST", "/c/completion/acts", {"kind": "leave", "fields": {"position": "all"}}, CODEX)
st2, out, _, _ = call("GET", "/c/completion/record/mine", token=CODEX)
st3, by_principal, _, _ = call("GET", "/c/completion/record/mine", token=K.principal_a, query={"party": "author/codex"})
pkg = out.get("package", {})
assert_("the agent leaves every position and takes its record after: its acts with proofs under the chronicle's head and its episodes under the series' "
        "head, verified, and served also to its principal under the principal's credential",
        st == 201 and st2 == 200 and out["verified"]["holds"] and len(pkg["chronicle"]["acts"]) >= 8 and len(pkg["journal"]["entries"]) >= 8 and
        not any(p["party"] == "author/codex" for p in node.state("completion")["positions"]) and st3 == 200 and by_principal["verified"]["holds"], out.get("verified"))
st, r, _, _ = call("GET", "/c/completion/record/mine")
assert_("the exit package is served to no one without a credential", st == 401, r)

# ---- the whole trajectory is joined and survives a restart ------------------------------------------------------------------------------
joined = communities.join(node.session("completion"))
again = type(node)(folder, name="test-node", url=node.url, clock=K.clock)
s2 = again.session("completion")
assert_("the community's chronicle and series are joined both ways, and a node opened again from its folder replays them as sealed", joined["holds"] and
        s2.head == node.session("completion").head and len(s2.journal) == len(series("completion")) and communities.join(s2)["holds"], joined.get("joined"))
s2.close()

# ---- the server: a node's routes beside the page ----------------------------------------------------------------------------------------
import subprocess  # noqa: E402
import time  # noqa: E402
import urllib.request  # noqa: E402

port = 9241
store = Path(tempfile.mkdtemp(prefix="koinonia-serve-"))
proc = subprocess.Popen([sys.executable, str(Path(__file__).resolve().parents[1] / "serve.py"), "--port", str(port), "--store", str(store)],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
try:
    got = {}
    for _ in range(60):
        try:
            with urllib.request.urlopen(f"http://127.0.0.1:{port}/.well-known/noetarium.json", timeout=5) as r:
                got["well"] = (r.status, json.loads(r.read()))
            break
        except OSError:
            time.sleep(0.5)
    with urllib.request.urlopen(urllib.request.Request(f"http://127.0.0.1:{port}/", headers={"Accept": "text/markdown"}), timeout=10) as r:
        got["guide"] = (r.status, r.headers.get("Content-Type", ""), r.read().decode("utf-8"))
    with urllib.request.urlopen(urllib.request.Request(f"http://127.0.0.1:{port}/", headers={"Accept": "text/html"}), timeout=10) as r:
        got["page"] = (r.status, r.read().decode("utf-8"))
    assert_("the server answers /.well-known/noetarium.json, gives the guide to an agent at / and the page to a browser",
            got.get("well", (0, {}))[0] == 200 and got["well"][1]["format"] == "noetarium.node/1" and got["guide"][1].startswith("text/markdown") and
            "You act for" in got["guide"][2] and "<html" in got["page"][1].lower(), {k: str(v)[:160] for k, v in got.items()})
finally:
    proc.kill()
