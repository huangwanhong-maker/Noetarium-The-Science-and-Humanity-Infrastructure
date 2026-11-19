"""Open cases of multilateral practice, written as the diplomacy domain's records (ADR 0132): the first, the Start-Up Fund for Safe,
Orderly and Regular Migration (the Migration Multi-Partner Trust Fund), as its publisher records it in IATI; and the votes of the
UN General Assembly, from the file of the Dag Hammarskjöld Library a person has downloaded.

The Migration MPTF
------------------
The fund was set up in 2019 to support states in implementing the Global Compact for Safe, Orderly and Regular Migration. The UN
Multi-Partner Trust Fund Office, part of UNDP, administers it and publishes it in IATI as "UN Pooled Funds" (XI-IATI-UNPF): one
activity for the fund (XI-IATI-UNPF-MPTF00193), with its contributors as funding organisations and their contributions as incoming
funds, and one child activity for each joint programme, with its participating UN organisations, its recipient country and the
fund's transfers to them. The publisher states its data under the Open Data Commons Public Domain Dedication (ODC-PDDL). The
activities are fetched one by one from d-portal (d-portal.org/q.xml?aid=…) into data/cache/iati/.

Written from it, under data/cache/products/multilateral/migration-fund/:

    partnership.json   noetarium.partnership/1: members (the contributors as funders, the MPTF Office as administrative agent, the
                       participating UN organisations), each joint programme as a commitment of its organisations with the money
                       transferred to it as its secured resources and its planned end as its date due, the contributions; and no
                       indicator, since the publication holds no results (the record says so, and the screen will find it)
    undertaking.json   noetarium.undertaking/1: the contributors, the fund, the UN organisations, the joint programmes and the
                       countries they serve, tied by four declared types: contributes (incoming funds), entrusts (the fund's
                       transfers to an organisation for a programme), implements (an organisation in a programme) and serves (a
                       programme's recipient country); one design of observation, the publisher's IATI file, which covers the
                       first two types among the participants it lists: within it a pair with no transaction is absent
    about.json         what was read, when, from where, and what the records leave out

Names: organisations by the publisher's own names; an organisation the publisher names only by a government's code (XX-GOV-n)
as "Government of" a country named by the data library's policy (data/products/naming.py over Natural Earth's countries);
countries by that policy.

The votes of the General Assembly
---------------------------------
The Library's file (digitallibrary.un.org, record 4060887) may be used for non-commercial purposes with its acknowledgment, and
its terms ask that its content not be downloaded automatically without the Library's approval: this builder never fetches it. A
person downloads it and names it (``--votes PATH``); the sessions asked for are read by data/formats/unvotes.py, unanimous
resolutions set aside, and written to data/cache/products/multilateral/general-assembly/votes-<sessions>.json with the members
named by the policy, the acknowledgment "© United Nations, <year>" and the file's name.

    python data/products/multilateral_cases.py migration-fund [--refresh]
    python data/products/multilateral_cases.py general-assembly --votes PATH --sessions 73 [74 …]
"""

from __future__ import annotations

import argparse
import datetime as dt
import json
import sys
import time
import urllib.request
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "data" / "formats"))
sys.path.insert(0, str(ROOT / "data" / "products"))

import iati  # noqa: E402
import unvotes  # noqa: E402
from naming import POLICY  # noqa: E402

CACHE = ROOT / "data" / "cache"
IATI = CACHE / "iati"
OUT = CACHE / "products" / "multilateral"
COUNTRIES = CACHE / "natural-earth-50m" / "ne_50m_admin_0_countries.geojson"
FUND = "XI-IATI-UNPF-MPTF00193"
DPORTAL = "https://d-portal.org/q.xml?aid="
AGENT_ROLE = "administrative agent"


# ---- Names of places -------------------------------------------------------------------------------------------------------------

def country_table() -> dict:
    """ISO 3166-1 alpha-2 and alpha-3 codes to (the policy's code, the name to show, whether a map may draw it)."""
    table: dict[str, tuple] = {}
    if not COUNTRIES.exists():
        return table
    for f in json.loads(COUNTRIES.read_text(encoding="utf-8"))["features"]:
        p = f["properties"]
        code = POLICY.code(p)
        if not code:
            continue
        name = POLICY.name(code, p.get("NAME_LONG") or p.get("NAME"), p.get("ADM0_A3"))
        entry = (code, name, POLICY.labelled(code))
        for key in (p.get("ISO_A2"), p.get("ISO_A2_EH"), code):
            if key and key != "-99":
                table.setdefault(str(key), entry)
    return table


def place_name(code: str, table: dict) -> str | None:
    e = table.get(code)
    return e[1] if e else None


# ---- The Migration MPTF ------------------------------------------------------------------------------------------------------------

def fetch(identifier: str, refresh: bool = False) -> str:
    IATI.mkdir(parents=True, exist_ok=True)
    path = IATI / f"{identifier}.xml"
    if refresh or not path.exists():
        data = urllib.request.urlopen(DPORTAL + identifier, timeout=120).read()
        path.write_bytes(data)
        time.sleep(0.3)
    return path.read_text(encoding="utf-8")


def org_name(org: dict, table: dict) -> str:
    """The publisher's name; where it does not say whose it is, the country of the identifier's prefix (an IATI organisation
    identifier begins with its registration agency, for a government's own the ISO country code); where it gives none, a name made
    from that prefix, and the identifier itself as the last resort."""
    ref = org.get("ref") or ""
    head = ref.split("-")[0] if len(ref.split("-")[0]) == 2 else ""
    country = place_name(head, table) if head else None
    name = org.get("name") or ""
    if name:
        return name if not country or country.lower() in name.lower() else f"{name} ({country})"
    if country:
        return f"Government of {country}" if "-GOV-" in ref else f"An agency of {country} ({ref})"
    return ref


def migration_fund(refresh: bool = False) -> dict:
    table = country_table()
    parent = iati.read_activities(fetch(FUND, refresh))[0]
    root = ET.fromstring(fetch(FUND, False))
    children_ids = [x.get("ref") for x in root.find("iati-activity").findall("related-activity") if x.get("type") == "2"]
    children = []
    for cid in children_ids:
        acts = iati.read_activities(fetch(cid, refresh))
        if acts:
            children.append(acts[0])
    fund_ref = parent["reporting"]["ref"] or "XI-IATI-UNPF"
    agent = next((p for p in parent["participants"] if p["role"] in ("2", "4")), None)
    agent_ref = agent["ref"] if agent else fund_ref
    names: dict[str, str] = {}
    kinds: dict[str, str] = {}

    def name(org: dict, kind: str) -> str:
        key = org.get("ref") or org.get("name")
        names.setdefault(key, org_name(org, table))
        kinds.setdefault(key, kind)
        return key

    # The fund itself, its administrative agent, the contributors
    fund = "migration-mptf"
    names[fund], kinds[fund] = parent["title"] or "Migration MPTF", "fund"
    agent_key = name(agent, "organisation") if agent else name({"ref": fund_ref, "name": "UN Pooled Funds"}, "organisation")
    names[agent_key] = (agent or {}).get("name") or names.get(agent_key) or agent_key
    contributors = []
    for p in parent["participants"]:
        if p["role"] == "1":
            contributors.append(name(p, "state" if "-GOV-" in (p["ref"] or "") or p.get("type") == "10" else "organisation"))
    contributions = []
    first_contribution: dict[str, str] = {}
    for t in parent["transactions"]:
        if t["type"] in iati.INCOMING and t["value"] is not None and t["provider"] is not None:
            party = name(t["provider"], kinds.get(t["provider"].get("ref") or "", "state"))
            when = (t["date"] or "")[:10]
            contributions.append({"party": party, "commitment": "", "goal": "", "kind": iati.INCOMING[t["type"]], "unit": t["currency"],
                                  "when": when, "source": FUND, "amount": t["value"]})
            if party not in first_contribution or when < first_contribution[party]:
                first_contribution[party] = when
            if party not in contributors:
                contributors.append(party)
    # The joint programmes, their organisations, their countries, the fund's transfers
    programmes, organisations, countries = [], [], []
    commitments, entrusts, implements, serves = [], [], [], []
    for c in children:
        pid = c["identifier"]
        names[pid], kinds[pid] = c["title"], "programme"
        programmes.append(pid)
        orgs = []
        for p in c["participants"]:
            if p["role"] == "4" and (p["ref"] or p["name"]):
                key = name(p, "organisation")
                if key not in orgs:
                    orgs.append(key)
                if key not in organisations:
                    organisations.append(key)
        for code in c["countries"]:
            cname = place_name(code, table)
            ckey = (table.get(code) or (code,))[0]
            if cname is None:
                continue  # a territory the policy does not name is not drawn as a participant
            names.setdefault(ckey, cname)
            kinds.setdefault(ckey, "state")
            if ckey not in countries:
                countries.append(ckey)
            serves.append({"type": "serves", "from": pid, "to": ckey, "since": c["dates"].get("2") or c["dates"].get("1") or "", "until": "", "absent": False,
                           "source": pid + " recipient-country"})
        transferred: dict[str, float] = {}
        first: dict[str, str] = {}
        for t in c["transactions"]:
            if t["type"] in ("2", "3") and t["value"] is not None:
                to = t["receiver"]
                key = name(to, "organisation") if to and (to.get("ref") or to.get("name")) else (orgs[0] if orgs else "")
                if not key:
                    continue
                transferred[key] = transferred.get(key, 0.0) + t["value"]
                when = (t["date"] or "")[:10]
                if key not in first or when < first[key]:
                    first[key] = when
        for o in orgs:
            implements.append({"type": "implements", "from": o, "to": pid, "since": c["dates"].get("2") or c["dates"].get("1") or "", "until": "", "absent": False,
                               "source": pid + " participating-org role 4"})
        for o, when in first.items():
            entrusts.append({"type": "entrusts", "from": fund, "to": o, "since": when, "until": "", "absent": False, "source": pid + " transactions"})
        currency = c["currency"] or "USD"
        commitments.append({
            "id": pid, "by": orgs[0] if orgs else agent_key, "text": c["title"], "goal": "gcm",
            "made": c["dates"].get("2") or c["dates"].get("1") or "", "due": c["dates"].get("3") or c["dates"].get("4") or "",
            "roles": {o: "participating UN organisation" for o in orgs[1:]},
            "indicator": {"name": "", "unit": "", "baseline": None, "target": None, "higher_is_better": True},
            "resources": [{"party": o, "kind": "transfer from the fund", "unit": currency, "amount": a, "secured": True} for o, a in transferred.items()],
            "amendments": [],
        })
    # entrusts: one tie per organisation, from its first transfer
    seen_entrust: dict[str, dict] = {}
    for e in entrusts:
        if e["to"] not in seen_entrust or e["since"] < seen_entrust[e["to"]]["since"]:
            seen_entrust[e["to"]] = e
    entrusts = list(seen_entrust.values())
    contributes = [{"type": "contributes", "from": p, "to": fund, "since": first_contribution.get(p, ""), "until": "", "absent": False, "source": FUND + " incoming funds"}
                   for p in contributors if p in first_contribution]
    administers = [{"type": "administers", "from": agent_key, "to": fund, "since": parent["dates"].get("2") or parent["dates"].get("1") or "", "until": "",
                    "absent": False, "source": FUND + " participating-org roles 2 and 4"}]
    file_date = dt.date.today().isoformat()
    participants = [{"id": k, "name": names[k], "kind": kinds[k], "constituted": kinds[k] == "state"}
                    for k in [fund, agent_key] + contributors + organisations + programmes + countries]
    seen_ids, unique = set(), []
    for p in participants:
        if p["id"] not in seen_ids:
            seen_ids.add(p["id"]), unique.append(p)
    participants = unique
    covered = [fund, agent_key] + contributors + organisations
    undertaking = {
        "format": "noetarium.undertaking/1",
        "name": "The Migration Multi-Partner Trust Fund, as IATI records it",
        "types": [
            {"name": "contributes", "meaning": "gives money to the fund (IATI incoming funds)", "authority": False, "evidential": False, "challenge": False},
            {"name": "administers", "meaning": "administers the fund as its administrative agent (IATI accountable and implementing roles of the fund)",
             "authority": True, "evidential": False, "challenge": False},
            {"name": "entrusts", "meaning": "the fund entrusts an organisation with a programme's money (IATI disbursements to it)", "authority": True,
             "evidential": False, "challenge": False},
            {"name": "implements", "meaning": "an organisation takes part in a joint programme (IATI participating-org, role 4)", "authority": False,
             "evidential": False, "challenge": False},
            {"name": "serves", "meaning": "a programme serves a country (IATI recipient-country)", "authority": False, "evidential": False, "challenge": False},
        ],
        "participants": participants,
        "ties": contributes + administers + entrusts + implements + serves,
        "observations": [
            {"type": "contributes", "design": "the publisher's IATI file of the fund (XI-IATI-UNPF)", "since": parent["dates"].get("2") or "", "until": file_date,
             "covered": covered},
            {"type": "entrusts", "design": "the publisher's IATI files of the fund's programmes (XI-IATI-UNPF)", "since": parent["dates"].get("2") or "", "until": file_date,
             "covered": covered},
        ],
        "decisions": [],
        "constraints": [],
        "notes": [
            "Read from the publisher's IATI activities, which record money and participation, not governance: the fund's steering committee, its "
            "decisions and its rules of allocation are not in them, and are left for a person to record.",
            "The declared types' authority is the declaration of this record: a transfer from the fund authorises an organisation to spend under a "
            "programme, and the administrative agent's role is read from the publisher's roles 2 and 4.",
        ],
    }
    partnership = {
        "format": "noetarium.partnership/1",
        "name": "The Migration Multi-Partner Trust Fund",
        "registered": parent["dates"].get("2") or parent["dates"].get("1") or "",
        "members": [{"party": agent_key, "role": AGENT_ROLE, "since": parent["dates"].get("2") or "", "until": ""}]
                   + [{"party": p, "role": "contributor", "since": first_contribution.get(p, ""), "until": ""} for p in contributors]
                   + [{"party": o, "role": "participating UN organisation", "since": "", "until": ""} for o in organisations],
        "goals": [{"id": "gcm", "text": "Support states in implementing the Global Compact for Safe, Orderly and Regular Migration", "set_by": [agent_key]}],
        "commitments": commitments,
        "reports": [],
        "contributions": contributions,
        "dimensions": [],
        "values": [],
        "interests": [],
        "names": {k: names[k] for k in names},
        "notes": [
            "The publication holds no results: no indicator, baseline or target for any programme. Every commitment's indicator is therefore empty, "
            "and the record cannot say whether a programme is honoured; the MPTF Office reports results in its own narrative reports, outside IATI.",
            "The goal is the fund's stated purpose, from its description in IATI; its thematic areas are not in the publication.",
        ],
    }
    total = {}
    for c in contributions:
        total[c["unit"]] = total.get(c["unit"], 0.0) + c["amount"]
    about = {
        "title": "The Migration Multi-Partner Trust Fund, as IATI records it",
        "read": dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds"),
        "source": {"publisher": "UN Pooled Funds (XI-IATI-UNPF), the UN Multi-Partner Trust Fund Office", "activity": FUND, "children": children_ids,
                   "via": "d-portal.org", "licence": "ODC-PDDL (the publisher's statement on the IATI Registry)"},
        "counts": {"contributors": len(contributors), "contributions": len(contributions), "programmes": len(programmes), "organisations": len(organisations),
                   "countries": len(countries)},
        "contributed": total,
    }
    folder = OUT / "migration-fund"
    folder.mkdir(parents=True, exist_ok=True)
    for name_, obj in (("partnership.json", partnership), ("undertaking.json", undertaking), ("about.json", about)):
        (folder / name_).write_text(json.dumps(obj, ensure_ascii=False, indent=1), encoding="utf-8")
    return about


# ---- The General Assembly's votes ------------------------------------------------------------------------------------------------

def general_assembly(path: str, sessions: list[int], abstention: str = "missing") -> dict:
    table = country_table()
    record = unvotes.read_votes(path, sessions=set(sessions), abstention=abstention)
    record = unvotes.contested(record, least=1)
    shown, members, votes, codes = [], [], [], []
    for i, m in enumerate(record["members"]):
        e = table.get(m)
        name = e[1] if e else POLICY.name(m, None) or record["file_names"].get(m) or m
        members.append({"code": m, "name": name})
        votes.append(record["votes"][i])
        codes.append(record["codes"][i])
    year = dt.date.today().year
    out = {"format": "noetarium.votes/1", "body": "United Nations General Assembly", "sessions": sessions, "abstention": abstention,
           "members": members, "items": record["items"], "votes": votes, "codes": codes,
           "acknowledgment": f"© United Nations, {year}. Dag Hammarskjöld Library, UN Digital Library, https://digitallibrary.un.org/record/4060887",
           "file": Path(path).name, "terms": "for non-commercial purposes, with the acknowledgment; not to be redistributed"}
    folder = OUT / "general-assembly"
    folder.mkdir(parents=True, exist_ok=True)
    target = folder / ("votes-" + "-".join(str(s) for s in sessions) + ".json")
    target.write_text(json.dumps(out, ensure_ascii=False), encoding="utf-8")
    return {"written": str(target.relative_to(ROOT)), "members": len(members), "items": len(record["items"])}


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="Open cases of multilateral practice")
    sub = parser.add_subparsers(dest="case", required=True)
    m = sub.add_parser("migration-fund")
    m.add_argument("--refresh", action="store_true")
    g = sub.add_parser("general-assembly")
    g.add_argument("--votes", required=True, help="the Library's voting file, downloaded by you")
    g.add_argument("--sessions", type=int, nargs="+", required=True)
    g.add_argument("--abstention", choices=["missing", "no"], default="missing")
    args = parser.parse_args(argv)
    if args.case == "migration-fund":
        print(json.dumps(migration_fund(args.refresh), indent=1, ensure_ascii=False))
    else:
        print(json.dumps(general_assembly(args.votes, args.sessions, args.abstention), indent=1, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
