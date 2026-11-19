"""Checks of the readers and the cases of multilateral practice (data/formats/iati.py, data/formats/unvotes.py,
data/products/multilateral_cases.py), offline on markup written here, and on the cache where the Migration MPTF has been read.

- IATI: an activity of two funders, an administrative agent and an implementer, incoming funds from both funders and one from a
  funder not listed (added, and said so), an outgoing disbursement kept apart, a result with an indicator of three periods (the
  last target the commitment's, each actual a report), a qualitative indicator (no target, said so), narratives in two languages.
- UN votes: a file of the Library's form with two sessions; yes, no, abstention and not voting read under both readings of an
  abstention; a member absent from a resolution missing; unanimous resolutions set aside.
- The Migration MPTF, where it is in the cache: every incoming transaction of the publisher's file is a contribution, in total;
  each joint programme is a commitment with no indicator, as the publication holds no results; every place is named by the policy.

    python data/tests/check_multilateral.py
"""

from __future__ import annotations

import json

from checks import ROOT, assert_, check, skip

import iati
import unvotes

ACTIVITY = """<?xml version="1.0" encoding="UTF-8"?>
<iati-activities version="2.03">
 <iati-activity default-currency="USD" last-updated-datetime="2026-10-01T00:00:00">
  <iati-identifier>XX-TEST-1</iati-identifier>
  <reporting-org ref="XX-FUND" type="40"><narrative xml:lang="fr">Fonds</narrative><narrative xml:lang="en">The Fund</narrative></reporting-org>
  <title><narrative xml:lang="en">An invented pooled fund</narrative></title>
  <participating-org ref="AA-GOV-1" role="1" type="10"><narrative>A ministry</narrative></participating-org>
  <participating-org ref="BB-2" role="1" type="10"><narrative>An agency</narrative></participating-org>
  <participating-org ref="XX-AGENT" role="2"><narrative>The agent</narrative></participating-org>
  <participating-org ref="XX-AGENT" role="4"><narrative>The agent</narrative></participating-org>
  <participating-org ref="ZZ-IMPL" role="4"><narrative>An implementer</narrative></participating-org>
  <activity-date type="1" iso-date="2020-01-01"/>
  <activity-date type="2" iso-date="2020-02-01"/>
  <activity-date type="3" iso-date="2025-12-31"/>
  <transaction><transaction-type code="1"/><value currency="USD" value-date="2020-03-01">1000000</value>
   <provider-org ref="AA-GOV-1"><narrative>A ministry</narrative></provider-org></transaction>
  <transaction><transaction-type code="11"/><value value-date="2021-03-01">500000</value>
   <provider-org ref="BB-2"><narrative>An agency</narrative></provider-org></transaction>
  <transaction><transaction-type code="1"/><value currency="EUR" value-date="2022-03-01">250000</value>
   <provider-org ref="CC-3"><narrative>A latecomer</narrative></provider-org></transaction>
  <transaction><transaction-type code="3"/><value value-date="2022-06-01">700000</value>
   <receiver-org ref="ZZ-IMPL"><narrative>An implementer</narrative></receiver-org></transaction>
  <result type="2">
   <title><narrative>Migrants reach services</narrative></title>
   <indicator measure="1" ascending="1">
    <title><narrative>People served</narrative></title>
    <baseline year="2019" value="100"/>
    <period><period-start iso-date="2021-01-01"/><period-end iso-date="2021-12-31"/><target value="500"/><actual value="420"/></period>
    <period><period-start iso-date="2022-01-01"/><period-end iso-date="2022-12-31"/><target value="900"/><actual value="910"/></period>
    <period><period-start iso-date="2023-01-01"/><period-end iso-date="2023-12-31"/><actual value="1200"/></period>
   </indicator>
   <indicator measure="5"><title><narrative>How services are perceived</narrative></title></indicator>
  </result>
 </iati-activity>
</iati-activities>"""

VOTES = """undl_id,ms_code,ms_name,ms_vote,date,session,resolution,draft,committee_report,meeting,title,agenda_title,subjects,vote_note,total_yes,total_no,total_abstentions,total_non_voting,total_ms,undl_link
1,AAA,ALPHA,Y,2018-12-19,73,A/RES/73/1,,,,On one thing,,MIGRATION,,2,1,1,0,4,x
1,BBB,BETA,N,2018-12-19,73,A/RES/73/1,,,,On one thing,,MIGRATION,,2,1,1,0,4,x
1,CCC,GAMMA,A,2018-12-19,73,A/RES/73/1,,,,On one thing,,MIGRATION,,2,1,1,0,4,x
1,DDD,DELTA,Y,2018-12-19,73,A/RES/73/1,,,,On one thing,,MIGRATION,,2,1,1,0,4,x
2,AAA,ALPHA,Y,2018-12-20,73,A/RES/73/2,,,,On another,,DISARMAMENT,,3,0,0,1,4,x
2,BBB,BETA,Y,2018-12-20,73,A/RES/73/2,,,,On another,,DISARMAMENT,,3,0,0,1,4,x
2,CCC,GAMMA,Y,2018-12-20,73,A/RES/73/2,,,,On another,,DISARMAMENT,,3,0,0,1,4,x
2,DDD,DELTA,X,2018-12-20,73,A/RES/73/2,,,,On another,,DISARMAMENT,,3,0,0,1,4,x
3,AAA,ALPHA,N,2019-10-01,74,A/RES/74/1,,,,Later,,MIGRATION,,1,1,0,0,3,x
3,BBB,BETA,Y,2019-10-01,74,A/RES/74/1,,,,Later,,MIGRATION,,1,1,0,0,3,x
"""


def main() -> None:
    acts = iati.read_activities(ACTIVITY, language="en")
    a = acts[0]
    assert_("iati: one activity read, its reporting organisation by its English narrative", len(acts) == 1 and a["reporting"]["name"] == "The Fund", a["reporting"])
    p = iati.partnership(a)
    roles = {m["party"]: m["role"] for m in p["members"]}
    assert_("iati: the funders, the agent (accountable and implementing in one) and the implementer are members with their roles",
            roles.get("AA-GOV-1") == "funder" and roles.get("BB-2") == "funder" and roles.get("XX-AGENT") == "accountable · implementer"
            and roles.get("ZZ-IMPL") == "implementer" and roles.get("XX-FUND") == "reporting", roles)
    assert_("iati: a provider not listed as participating is added, named in a transaction, and the record says so",
            roles.get("CC-3") == "named in a transaction" and any("CC-3" in n for n in p["notes"]), p["notes"])
    contributions = {(c["party"], c["kind"], c["unit"]): c["amount"] for c in p["contributions"]}
    assert_("iati: incoming funds and commitments are contributions in their currencies, the activity's default where none is given",
            contributions == {("AA-GOV-1", "incoming funds", "USD"): 1e6, ("BB-2", "incoming commitment", "USD"): 5e5, ("CC-3", "incoming funds", "EUR"): 2.5e5}, contributions)
    assert_("iati: the outgoing disbursement is kept apart, not counted as a contribution", p["flows"] == [{"to": "ZZ-IMPL", "kind": "disbursement", "unit": "USD", "when": "2022-06-01", "amount": 7e5}], p["flows"])
    c = next(x for x in p["commitments"] if x["indicator"]["name"] == "People served")
    assert_("iati: an indicator is a commitment with its baseline, and as target and date due the last period that states a target",
            c["indicator"]["baseline"] == 100 and c["indicator"]["target"] == 900 and c["due"] == "2022-12-31" and c["made"] == "2020-02-01", c)
    reps = [(r["when"], r["value"]) for r in p["reports"] if r["commitment"] == c["id"]]
    assert_("iati: each period's actual value is a report at the period's end", reps == [("2021-12-31", 420), ("2022-12-31", 910), ("2023-12-31", 1200)], reps)
    q = next(x for x in p["commitments"] if x["indicator"]["name"] == "How services are perceived")
    assert_("iati: a qualitative indicator has no target, and the record says why", q["indicator"]["target"] is None and any("not a number" in n for n in p["notes"]))
    assert_("iati: the record's format and the registration at the actual start", p["format"] == "noetarium.partnership/1" and p["registered"] == "2020-02-01")

    v = unvotes.read_votes(VOTES, sessions={73})
    assert_("un votes: the session's members and resolutions", v["members"] == ["AAA", "BBB", "CCC", "DDD"] and [i["resolution"] for i in v["items"]] == ["A/RES/73/1", "A/RES/73/2"], v["members"])
    assert_("un votes: yes 1, no 0, an abstention and not voting missing, by default", v["votes"] == [[1, 1], [0, 1], [None, 1], [1, None]], v["votes"])
    w = unvotes.read_votes(VOTES, sessions={73}, abstention="no")
    assert_("un votes: an abstention read as a no when so declared", w["votes"][2] == [0, 1], w["votes"])
    assert_("un votes: the raw codes are kept", v["codes"][3] == ["Y", "X"], v["codes"])
    both = unvotes.read_votes(VOTES)
    assert_("un votes: a member with no row on a resolution is missing", both["codes"][2][2] == "" and both["votes"][2][2] is None, both["codes"])
    con = unvotes.contested(v)
    assert_("un votes: the unanimous resolution is set aside, the contested one kept", [i["resolution"] for i in con["items"]] == ["A/RES/73/1"], con["items"])
    assert_("un votes: subjects select resolutions", [i["resolution"] for i in unvotes.read_votes(VOTES, subjects="migration")["items"]] == ["A/RES/73/1", "A/RES/74/1"])

    folder = ROOT / "data" / "cache" / "products" / "multilateral" / "migration-fund"
    parent = ROOT / "data" / "cache" / "iati" / "XI-IATI-UNPF-MPTF00193.xml"
    if not (folder / "partnership.json").exists() or not parent.exists():
        skip("multilateral: the Migration MPTF is not in the cache (python data/products/multilateral_cases.py migration-fund)")
        return
    part = json.loads((folder / "partnership.json").read_text(encoding="utf-8"))
    und = json.loads((folder / "undertaking.json").read_text(encoding="utf-8"))
    act = iati.read_activities(parent.read_text(encoding="utf-8"))[0]
    incoming = sum(t["value"] for t in act["transactions"] if t["type"] in iati.INCOMING and t["value"] is not None)
    check("multilateral: the Migration MPTF's contributions add up to its publisher's incoming transactions", abs(sum(c["amount"] for c in part["contributions"]) - incoming), 1e-6, "USD")
    kids = [x for x in und["participants"] if x["kind"] == "programme"]
    assert_("multilateral: each joint programme is a commitment with no indicator, as the publication holds no results",
            len(part["commitments"]) == len(kids) and all(not c["indicator"]["name"] for c in part["commitments"]) and any("no results" in n for n in part["notes"]),
            f"{len(part['commitments'])} commitments, {len(kids)} programmes")
    from naming import POLICY
    states = [x for x in und["participants"] if x["kind"] == "state" and len(x["id"]) == 3 and x["id"].isupper()]
    assert_("multilateral: every country a programme serves is named by the policy and may be drawn",
            all(POLICY.labelled(x["id"]) for x in states) and len(states) > 0, [x["name"] for x in states])
    types = {t["name"] for t in und["types"]}
    assert_("multilateral: every tie is of a declared type, between declared participants",
            all(t["type"] in types for t in und["ties"]) and all(t["from"] in {x["id"] for x in und["participants"]} and t["to"] in {x["id"] for x in und["participants"]} for t in und["ties"]))


if __name__ == "__main__":
    main()
