"""IATI activities (the Activity Standard, versions 2.01 to 2.03) read into the infrastructure's record of a partnership,
``noetarium.partnership/1`` (ADR 0132; the diplomacy domain's ``partnerships``).

The International Aid Transparency Initiative's standard records an activity: who takes part in it and in which role, its dates,
its money (transactions and budgets) and, where the publisher reports them, its results with indicators, baselines, targets and
actual values by period. A multi-donor programme is one activity with several funding organisations, each giving incoming funds.
Read into a partnership:

    members        each participating organisation, by its ``@ref`` (or its name when it has none), with its role
                   (1 funding, 2 accountable, 3 extending, 4 implementing; codelist OrganisationRole) as ``funder``,
                   ``accountable``, ``extending``, ``implementer``; the reporting organisation too, as ``reporting``
    registered     the actual start (activity-date type 2), else the planned start (type 1)
    goals          each result, its title as the goal's text, set by the reporting organisation
    commitments    each indicator of a result, held by the reporting organisation: its baseline, and as its target and date
                   due the target and end of its last period that states a target; its measure (1 unit, 2 percentage) and whether
                   it ascends (``@ascending``, true unless "0" or "false") as its direction. An indicator of a qualitative or nominal
                   measure (3, 5) or with no numeric target is kept as a commitment with no target, which the screen will find
    reports        each period's actual value, dated at the period's end
    contributions  each transaction of incoming funds or incoming commitment (types 1, 11; codelist TransactionType), from its
                   provider organisation, in its currency (``@currency`` or the activity's ``@default-currency``), dated at its
                   value date; outgoing transactions are kept apart (``flows``), since a partnership's contributions are what its
                   members give to it

Nothing is inferred beyond the publication: an organisation named in a transaction but not listed as participating is added as a
member with the role ``named in a transaction``, and the record says so (``notes``). A transaction with no provider counts as the
reporting organisation's only when the publisher says nothing else, and that too is noted.

Standard library only.

    from iati import read_activities, partnership
    activities = read_activities(xml_text)           # each iati-activity as a dictionary close to the XML
    record = partnership(activities[0])               # noetarium.partnership/1

References. IATI Standard, Activity Standard 2.03 (iatistandard.org/en/iati-standard/203/activity-standard/); codelists
OrganisationRole, TransactionType, ActivityDateType, IndicatorMeasure, ResultType (iatistandard.org/en/iati-standard/203/codelists/).
"""

from __future__ import annotations

import xml.etree.ElementTree as ET

ROLES = {"1": "funder", "2": "accountable", "3": "extending", "4": "implementer"}
INCOMING = {"1": "incoming funds", "11": "incoming commitment", "13": "incoming pledge"}
OUTGOING = {"2": "outgoing commitment", "3": "disbursement", "4": "expenditure", "12": "outgoing pledge"}
XML_LANG = "{http://www.w3.org/XML/1998/namespace}lang"


def _narrative(element, language: str = "en") -> str:
    """The narrative in the language asked for, else the first; for IATI 1.x, the element's own text."""
    if element is None:
        return ""
    narratives = element.findall("narrative")
    if not narratives:
        return (element.text or "").strip()
    for n in narratives:
        if (n.get(XML_LANG) or "").lower() == language:
            return (n.text or "").strip()
    return (narratives[0].text or "").strip()


def _number(text):
    if text is None:
        return None
    try:
        return float(str(text).strip())
    except ValueError:
        return None


def _date(element) -> str:
    if element is None:
        return ""
    return (element.get("iso-date") or "")[:10]


def _org(element, language: str) -> dict:
    return {"ref": (element.get("ref") or "").strip(), "name": _narrative(element, language), "type": element.get("type") or "",
            "role": element.get("role") or "", "activity": element.get("activity-id") or ""}


def read_activities(text: str, language: str = "en") -> list[dict]:
    """Each iati-activity of a document (an iati-activities file or a single activity) as a dictionary close to its XML."""
    root = ET.fromstring(text)
    elements = [root] if root.tag == "iati-activity" else root.findall("iati-activity")
    out = []
    for a in elements:
        act = {
            "identifier": (a.findtext("iati-identifier") or "").strip(),
            "title": _narrative(a.find("title"), language),
            "description": _narrative(a.find("description"), language),
            "currency": a.get("default-currency") or "",
            "updated": a.get("last-updated-datetime") or "",
            "reporting": _org(a.find("reporting-org"), language) if a.find("reporting-org") is not None else {"ref": "", "name": ""},
            "participants": [_org(p, language) for p in a.findall("participating-org")],
            "dates": {d.get("type"): _date(d) for d in a.findall("activity-date")},
            "status": (a.find("activity-status").get("code") if a.find("activity-status") is not None else ""),
            "countries": [c.get("code") for c in a.findall("recipient-country") if c.get("code")],
            "sectors": [{"code": s.get("code"), "vocabulary": s.get("vocabulary") or "1"} for s in a.findall("sector")],
            "transactions": [],
            "budgets": [],
            "results": [],
        }
        for t in a.findall("transaction"):
            tt = t.find("transaction-type")
            value = t.find("value")
            prov, recv = t.find("provider-org"), t.find("receiver-org")
            act["transactions"].append({
                "type": tt.get("code") if tt is not None else "",
                "value": _number(value.text if value is not None else None),
                "currency": (value.get("currency") if value is not None else None) or act["currency"],
                "date": (value.get("value-date") if value is not None else "") or _date(t.find("transaction-date")),
                "provider": _org(prov, language) if prov is not None else None,
                "receiver": _org(recv, language) if recv is not None else None,
                "description": _narrative(t.find("description"), language),
            })
        for b in a.findall("budget"):
            value = b.find("value")
            act["budgets"].append({"start": _date(b.find("period-start")), "end": _date(b.find("period-end")),
                                   "value": _number(value.text if value is not None else None),
                                   "currency": (value.get("currency") if value is not None else None) or act["currency"]})
        for r in a.findall("result"):
            res = {"type": r.get("type") or "", "title": _narrative(r.find("title"), language), "indicators": []}
            for i in r.findall("indicator"):
                base = i.find("baseline")
                ind = {"title": _narrative(i.find("title"), language), "measure": i.get("measure") or "1",
                       "ascending": (i.get("ascending") or "1").strip().lower() not in ("0", "false"),
                       "baseline": _number(base.get("value") if base is not None else None),
                       "baseline_year": (base.get("year") if base is not None else "") or "",
                       "periods": []}
                for p in i.findall("period"):
                    target, actual = p.find("target"), p.find("actual")
                    ind["periods"].append({"start": _date(p.find("period-start")), "end": _date(p.find("period-end")),
                                           "target": _number(target.get("value") if target is not None else None),
                                           "actual": _number(actual.get("value") if actual is not None else None)})
                res["indicators"].append(ind)
            act["results"].append(res)
        out.append(act)
    return out


def _key(org: dict) -> str:
    return org["ref"] or org["name"]


def partnership(activity: dict) -> dict:
    """The activity as noetarium.partnership/1, with the publication's own words kept and what was added said in ``notes``."""
    notes: list[str] = []
    members: list[dict] = []
    names: dict[str, str] = {}
    seen: dict[str, int] = {}

    def member(org: dict, role: str) -> str:
        key = _key(org)
        if not key:
            return ""
        names.setdefault(key, org.get("name") or key)
        if key not in seen:
            seen[key] = len(members)
            members.append({"party": key, "role": role, "since": "", "until": ""})
        elif role and role not in members[seen[key]]["role"].split(" · "):
            members[seen[key]]["role"] += " · " + role
        return key

    reporting = member(activity["reporting"], "reporting") if _key(activity["reporting"]) else ""
    for p in activity["participants"]:
        member(p, ROLES.get(p["role"], "participant"))
    start = activity["dates"].get("2") or activity["dates"].get("1") or ""
    goals, commitments, reports = [], [], []
    for r_index, r in enumerate(activity["results"], start=1):
        goal = f"r{r_index}"
        goals.append({"id": goal, "text": r["title"] or f"result {r_index}", "set_by": [reporting] if reporting else []})
        for i_index, ind in enumerate(r["indicators"], start=1):
            cid = f"{goal}.i{i_index}"
            with_target = [p for p in ind["periods"] if p["target"] is not None]
            last = max(with_target, key=lambda p: p["end"]) if with_target else None
            numeric = ind["measure"] in ("1", "2")
            commitment = {
                "id": cid, "by": reporting, "text": ind["title"] or r["title"], "goal": goal,
                "made": start, "due": last["end"] if last else "", "roles": {},
                "indicator": {"name": ind["title"], "unit": "%" if ind["measure"] == "2" else "",
                              "baseline": ind["baseline"], "target": last["target"] if (last and numeric) else None,
                              "higher_is_better": ind["ascending"]},
                "resources": [], "amendments": [],
            }
            if not numeric:
                notes.append(f"{cid}: its measure ({ind['measure']}) is not a number, so it has no target to reach")
            commitments.append(commitment)
            for p in ind["periods"]:
                if p["actual"] is not None and p["end"]:
                    reports.append({"commitment": cid, "when": p["end"], "by": reporting, "source": activity["identifier"], "value": p["actual"]})
    contributions, flows = [], []
    for t in activity["transactions"]:
        if t["value"] is None:
            continue
        if t["type"] in INCOMING:
            prov = t["provider"]
            if prov is not None and _key(prov):
                listed = _key(prov) in seen
                if not listed:
                    notes.append(f"{_key(prov)} provided funds and is not listed as participating; it is added with the role 'named in a transaction'")
                party = member(prov, "" if listed else "named in a transaction")
            else:
                party = reporting
                notes.append(f"a transaction of {t['date']} names no provider; it is counted as {reporting}'s")
            contributions.append({"party": party, "commitment": "", "goal": "", "kind": INCOMING[t["type"]], "unit": t["currency"],
                                  "when": t["date"][:10], "source": activity["identifier"], "amount": t["value"]})
        elif t["type"] in OUTGOING:
            recv = t["receiver"]
            flows.append({"to": _key(recv) if recv else "", "kind": OUTGOING[t["type"]], "unit": t["currency"], "when": t["date"][:10], "amount": t["value"]})
    for c in commitments:
        for m in members:
            if m["party"] != c["by"] and ("funder" in m["role"] or "implementer" in m["role"]):
                c["roles"][m["party"]] = m["role"]
    return {
        "format": "noetarium.partnership/1",
        "name": activity["title"] or activity["identifier"],
        "source": {"standard": "IATI", "identifier": activity["identifier"], "updated": activity["updated"]},
        "registered": start,
        "members": members,
        "names": names,
        "goals": goals,
        "commitments": commitments,
        "reports": reports,
        "contributions": contributions,
        "flows": flows,
        "budgets": activity["budgets"],
        "dimensions": [],
        "values": [],
        "interests": [],
        "notes": notes,
    }
