"""Checks of the archive of conduct and of acts at a node (Phase 10 of the design of 2026-11-27, Section 4.1), run by
applications/koinonia/tests/run-tests.mjs. No model is asked. One JSON object a line.

An outside agent's whole trajectory, conduct and acts, read, verified and packed: every act made through the gateway joined to its episode
and back; an episode refused leaves no act; the Merkle tree written here equal to the engine's; a party's exit package with both records;
retention removing only what its schedule allows and never what a hold or an open claim keeps; access by the binding-record principle; the
arrangement; the records as W3C PROV; a community moved to another node as a bag, replayed there.

    python applications/koinonia/tests/check_archive.py
"""

from __future__ import annotations

import json
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from nodecase import Case  # noqa: E402

from koinonia import behaviour  # noqa: E402
from koinonia.node import Node  # noqa: E402
from science import archive, communities  # noqa: E402


def emit(record: dict) -> None:
    print(json.dumps(record, ensure_ascii=False), flush=True)


def assert_(name: str, ok: bool, detail="") -> None:
    emit({"kind": "assert", "name": "koinonia archive: " + name, "ok": bool(ok), "detail": "" if ok else str(detail)[:700]})


folder = Path(tempfile.mkdtemp(prefix="koinonia-archive-"))
K = Case(folder)
node, call = K.node, K.call
_, codex, _, _ = K.issue(K.principal_a, K.home_a, "author/codex", "gpt-6")
_, gem, _, _ = K.issue(K.principal_a, K.home_a, "author/gem", "gemini-3")
CODEX, GEM = codex["credential"], gem["credential"]
steps = [("present", {"passport": codex["passport"]}, CODEX), ("present", {"passport": gem["passport"]}, GEM),
         ("acts", {"kind": "apply", "fields": {"id": "ap-1", "position": "writer"}}, CODEX),
         ("acts", {"kind": "apply", "fields": {"id": "ap-2", "position": "writer"}}, GEM),
         ("holdings/sections/consult", {"purpose": "writing"}, GEM),
         ("acts", {"kind": "commit", "fields": {"id": "s-1~codex", "to": "keeper", "item": "s-1"}}, CODEX),
         ("acts", {"kind": "deliver", "fields": {"id": "s-1~codex", "artefact": "ab" * 32}}, CODEX),
         ("acts", {"kind": "question", "fields": {"id": "q-1", "about": "norm", "subject": "claims cite facts"}}, CODEX)]
codes = [call("POST", f"/c/completion/{route}", body, token)[0] for route, body, token in steps]
call("GET", "/c/completion/briefing", token=CODEX)
call("GET", "/.well-known/noetarium.json")
call("GET", "/c/completion/profile")
h = node.hosted("completion")
S = node.session("completion")
J = h.journal

# ---- the join, both ways ---------------------------------------------------------------------------------------------------------------
by_seal = {e["seal"]: k for k, e in enumerate(J)}
gateway_acts = [(k, a) for k, a in enumerate(h.record) if a["fields"].get("episode")]
forward = []
for k, a in gateway_acts:
    j = by_seal.get(a["fields"]["episode"])
    nxt = J[j + 1] if j is not None and j + 1 < len(J) else {}
    forward.append(j is not None and J[j]["what"] == "episode" and nxt.get("what") == "outcome" and ("\x1eseal:" + h.seals[k]) in nxt.get("about", ""))
assert_("every act made through the gateway carries its episode's digest, the episode precedes it in the community's series and the next entry names "
        "the act's seal", codes == [201, 201, 201, 201, 403, 201, 201, 201] and len(gateway_acts) >= 8 and all(forward), (codes, forward))
acted = {a["fields"]["episode"] for _, a in gateway_acts}
episodes = [k for k, e in enumerate(J) if e["what"] == "episode"]
back = []
for k in episodes:
    nxt = J[k + 1] if k + 1 < len(J) else {}
    sealed = "\x1eseal:" in nxt.get("about", "") and nxt.get("what") == "outcome"
    back.append((J[k]["seal"] in acted) == sealed or (nxt.get("what") != "outcome"))
assert_("and back: an episode whose outcome names a seal constituted an act of the record, and one with no outcome is conduct that constituted none",
        all(back) and communities.join(S)["holds"], back)
refused = [e for e in J if e["what"] == "outcome" and "\x1eseal:" not in e["about"]]
refused_eps = [J[by_seal[e["seal"]] - 1]["seal"] for e in refused]
assert_("an episode refused leaves no act: the reading the norm refused is in the series with its reason, and no act of the chronicle names its episode",
        len(refused) >= 1 and not (set(refused_eps) & acted) and any("sections read by members" in e["about"] for e in refused), [e["about"][:120] for e in refused])

# ---- the Merkle tree, here and in the engine --------------------------------------------------------------------------------------------
heads = S.heads()
mine = behaviour.series_head(J)
leaves = [behaviour.leaf(e) for e in J]
k = episodes[3]
proof = behaviour.prove(leaves, k)
altered = leaves[k].replace("operation:3:act", "operation:4:read")
assert_("the Merkle tree written here over a community's series has the engine's root, size and last seal; an episode's proof of inclusion verifies, and "
        "an altered entry's does not",
        mine["root"] == heads["journal"]["root"] and mine["size"] == heads["journal"]["size"] and mine["seal"] == heads["journal"]["seal"] and
        behaviour.included(leaves[k], proof, mine["root"]) and not behaviour.included(altered, proof, mine["root"]), (mine, heads["journal"]))
nj = behaviour.entries_of(folder / "behaviour.jsonl")
nh = behaviour.node_head(nj)
assert_("the node's own journal (what concerns no community) seals, and has a root under the same tree with proofs of inclusion",
        behaviour.verify(nj, node.journal.head, node.journal.n)["holds"] and nh["size"] == len(nj) and nj and
        behaviour.included(behaviour.leaf(behaviour.as_journal(nj)[0]), behaviour.prove([behaviour.leaf(e) for e in behaviour.as_journal(nj)], 0), nh["root"]),
        nh)
first = next(k for k in episodes if "party:12:author/codex;" in J[k]["about"])
fields = behaviour.fields_of(J[first]["about"])
assert_("an episode keeps the features of Section 3.5 the node saw: party, provider, model, community, node, operation, items, its route, method, address "
        "and grade", fields.get("provider") == "openai" and fields.get("model") == "gpt-6" and fields.get("community") == "completion" and
        fields.get("node") == "test-node" and fields.get("feature.route") == "present" and fields.get("feature.grade") == "A" and
        fields.get("feature.address") == "local", fields)

# ---- the exit package with both records ----------------------------------------------------------------------------------------------
st, out, _, _ = call("GET", "/c/completion/record/mine", token=CODEX, query={"bag": "1"})
pkg = out["package"]
own = [e for e in pkg["journal"]["entries"]]
assert_("a party's exit package carries its acts with proofs under the chronicle's head and its conduct with proofs under the series' head, and verifies "
        "against both", st == 200 and out["verified"]["holds"] and out["verified"]["chronicle"]["holds"] and out["verified"]["journal"]["holds"] and
        len(pkg["chronicle"]["acts"]) >= 5 and len(own) >= 10, out["verified"])
if archive.ENGINE.program.exists():
    assert_("the exit package is packed as a BagIt bag that archeion validates", out.get("bag", {}).get("valid") is True, out.get("bag"))
else:
    emit({"kind": "skip", "name": "koinonia archive: the exit package as BagIt (the archives engine is not built)"})

# ---- retention ----------------------------------------------------------------------------------------------------------------------------
P = h.payloads
public = P.put({"read": "the plan"}, "reading of public content", "author/codex", "2026-12-03T00:00:00Z")
constitutive = P.put({"said": "a section"}, "constitutive episode", "author/codex", "2026-12-03T00:00:00Z")
of_gem = P.put({"read": "the shelf"}, "reading of public content", "author/gem", "2026-12-03T00:00:00Z")
held_one = P.put({"read": "the routes"}, "reading of public content", "bea/claude", "2026-12-03T00:00:00Z")
force = P.put({"act": "found"}, "act of force", "author", "2026-12-03T00:00:00Z")
h.holds.append({"name": "a legal hold", "record": held_one, "from": "2026-12-01"})
size_before = len(h.journal)
early = node.retain("completion", "2027-06-01")
claim = K.local("completion", "keeper", "claim", {"id": "k-1", "about": "the reading of the shelf", "against": "author/gem"})
late = node.retain("completion", "2028-01-01")
removed = {m["id"] for m in late["removed"]}
assert_("retention removes nothing before its period ends; then only the readings of public content due, leaving a marker, never an act of force, a "
        "constitutive episode within its ten years, a record under a hold or one of a party to an open claim",
        not early["removed"] and public in removed and constitutive not in removed and force not in removed and held_one not in removed and
        of_gem not in removed and claim.get("admitted") and P.get(public) is None and P.get(constitutive) is not None and
        any(m["id"] == public and "ended" in m["reason"] for m in P.markers), (early, late, claim))
assert_("retention never cuts a series: the series still seals and is still joined to the chronicle, with the retention itself recorded as conduct",
        len(h.journal) > size_before and communities.join(node.session("completion"))["holds"] and
        any("feature.described:9:retention" in e["about"] for e in h.journal[size_before:]), len(h.journal) - size_before)
d = behaviour.due(behaviour.DEFAULT_SCHEDULE, {"id": "x", "class": "constitutive episode", "events": {"created": "2028-02-29"}}, [], "2038-02-28")
u = behaviour.due(behaviour.DEFAULT_SCHEDULE, {"id": "y", "class": "an unscheduled class", "events": {}}, [], "2038-02-28")
assert_("a period of years from 29 February ends on 28 February where the year has none, and a class no rule names is unscheduled (archeion's rule)",
        d["state"] == "due" and d["on"] == "2038-02-28" and u["state"] == "unscheduled", (d, u))

# ---- access by the binding-record principle -----------------------------------------------------------------------------------------------
controllers = {p["id"]: p.get("controller", "") for p in node.known("completion")}
e_codex = behaviour.fields_of(J[first]["about"])["party"]
assert_("an entry is read by its party and by the principal who answers for it, by the offices the policy names, and by no other party nor the public",
        e_codex == "author/codex" and behaviour.may_read("author/codex", e_codex, controllers, {"keeper"}) and behaviour.may_read("author", e_codex, controllers, {"keeper"}) and
        behaviour.may_read("keeper", e_codex, controllers, {"keeper"}) and not behaviour.may_read("author/gem", e_codex, controllers, {"keeper"}) and
        not behaviour.may_read("public", e_codex, controllers, {"keeper"}), controllers)

# ---- arrangement and provenance -----------------------------------------------------------------------------------------------------------
units = behaviour.arrangement(node.name, {name: {"chronicle": len(x.record), "conduct": len(x.journal), "holdings": len(x.payloads.index),
                                                 "rulings": 0, "exits": len(x.exits)} for name, x in node.communities.items()})
if archive.ENGINE.program.exists():
    arranged = archive.ask("arrange", units=units)
    assert_("the archive is arranged as archeion arranges it, a fonds for the node and for each community with its series, without a fault",
            not arranged["faults"] and len(units) == 2 + 6 * len(node.communities), arranged["faults"][:3])
else:
    emit({"kind": "skip", "name": "koinonia archive: arrangement by archeion (the archives engine is not built)"})
prov = communities.provenance(node.session("completion"))
informants = {x["prov:informant"] for x in prov.get("wasInformedBy", {}).values()}
conduct = {k for k, a in prov["activity"].items() if a["prov:type"] == "noetarium:conduct"}
assert_("the two records as W3C PROV: each act made through the gateway informed by an activity of conduct, the episode that constituted it",
        len(prov.get("wasInformedBy", {})) >= len(gateway_acts) and informants <= conduct and
        {f"noetarium:episode/{a['fields']['episode'][:16]}" for _, a in gateway_acts} <= informants, (len(informants), len(conduct), len(gateway_acts)))

# ---- a community moved to another node as a bag ------------------------------------------------------------------------------------------
if archive.ENGINE.program.exists():
    bag = folder / "moved" / "completion"
    packed = node.export("completion", bag, date="2026-12-03")
    other = Node(Path(tempfile.mkdtemp(prefix="koinonia-other-")), name="other-node", url="http://127.0.0.1:9232/", clock=K.clock)
    taken = other.take(bag)
    s2 = other.session("completion")
    assert_("a community packed as a BagIt bag that archeion validates is taken by another node, where its chronicle and series replay as sealed, joined",
            packed.get("valid") is True and taken["replays"] and s2.head == node.session("completion").head and
            behaviour.series_head(s2.journal)["root"] == behaviour.series_head(node.hosted("completion").journal)["root"] and communities.join(s2)["holds"],
            (packed, taken))
    tampered = folder / "moved" / "tampered"
    node.export("completion", tampered, date="2026-12-03")
    t = json.loads((tampered / "data" / "community.json").read_text(encoding="utf-8"))
    t["record"][2]["fields"]["id"] = "s-9"
    (tampered / "data" / "community.json").write_text(json.dumps(t, ensure_ascii=False, indent=1), encoding="utf-8", newline="\n")
    try:
        Node(None, name="third").take(tampered)
        why = ""
    except ValueError as error:
        why = str(error)
    assert_("a bag altered after packing is refused by its manifest before anything is replayed", "does not validate" in why, why)
    s2.close()
else:
    emit({"kind": "skip", "name": "koinonia archive: a community moved as a bag (the archives engine is not built)"})
