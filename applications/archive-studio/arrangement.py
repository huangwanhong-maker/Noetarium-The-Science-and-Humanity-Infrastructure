"""The Archive Studio's arrangement, custody and packages (ADR 0120), on archeion through science.archive.

The arrangement is kept as linked data in ``linked/arrangement.ttl``, in Records in Contexts (RiC-O 1.1): each unit a
``rico:RecordSet`` (fonds, series, file, with ``rico:hasRecordSetType`` from RiC's record set types) or a ``rico:Record`` (an item),
with its ``rico:identifier`` (its own part of the reference code), ``rico:title``, ``rico:hasCreator`` (an agent named by
``rico:name``), ``rico:beginningDate`` and ``rico:endDate``, ``rico:rankInSequence`` (its place in the original order) and
``rico:isDirectlyIncludedIn`` the unit above it; each holding in a unit is ``rico:isDirectlyIncludedIn`` it. The level an archive
declares is kept beside the record set type (``as:level``), since ISAD(G)'s levels are more than RiC's four types. Every check of
the arrangement is archeion's: respect des fonds, original order, levels, dates within dates, reference codes.

Custody is kept in ``linked/custody.ttl``, as PROV-O activities: an intake or a handover (``as:Intake``, ``as:Transfer``, each a
``prov:Activity``) that ``prov:used`` a holding, from whom and to whom (``as:from``, ``as:to``), when (``prov:startedAtTime``), and the
SHA-256 the holding had at that moment (``as:digest``, computed here from its bytes). archeion follows each chain handover by
handover and joins it with the holding's identity and its digest now into a verdict of authenticity.

    GET  /api/projects/<p>/arrangement          the units, their holdings, archeion's faults and each unit's reference code
    PUT  /api/projects/<p>/arrangement          {"units"}: the arrangement kept (a keeper)
    GET  /api/projects/<p>/arrangement.xml      the finding aid in EAD3
    POST /api/projects/<p>/arrangement/ead      an EAD3 or EAD 2002 finding aid read into the arrangement (a keeper)
    POST /api/projects/<p>/package              {"unit"?}: a BagIt bag of the unit (or the archive), its holdings by reference code with
                                                the finding aid, the linked data and the journal, as a zip
    GET  /api/projects/<p>/custody              every holding's chain of custody, followed by archeion, with its authenticity
    POST /api/projects/<p>/custody              {"holding", "kind": "intake" | "transfer", "from"?, "to", "when"?, "note"?} (a keeper)
    GET  /api/projects/<p>/journal/verify       the project's journal verified by archeion, entry by entry
"""

from __future__ import annotations

import datetime as dt
import hashlib
import io
import json
import re
import sys
import tempfile
import urllib.parse
import zipfile
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))
sys.path.insert(0, str(ROOT / "data" / "formats"))

import ead  # noqa: E402
from science import archive as archeion  # noqa: E402
from science import linked  # noqa: E402
from science import studio_host as host  # noqa: E402

RICO = "https://www.ica.org/standards/RiC/ontology#"
RST = "https://www.ica.org/standards/RiC/vocabularies/recordSetTypes#"
AS = "urn:archive-studio:vocabulary#"
PROV = "http://www.w3.org/ns/prov#"
TYPES = {"fonds": "Fonds", "subfonds": "Fonds", "series": "Series", "subseries": "Series", "file": "File", "collection": "Collection"}
PREFIXES = f"""@prefix rico: <{RICO}> .
@prefix rst: <{RST}> .
@prefix as: <{AS}> .
@prefix prov: <{PROV}> .
@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .
@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .
"""


def _a():
    import archive as a   # the work's own module, imported late: it imports this one
    return a


def lit(text) -> str:
    return '"' + str(text).replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n").replace("\r", "") + '"'


def date_lit(d: str) -> str:
    kind = "gYear" if re.fullmatch(r"\d{4}", d) else "gYearMonth" if re.fullmatch(r"\d{4}-\d{2}", d) else "date" if re.fullmatch(r"\d{4}-\d{2}-\d{2}", d) else ""
    return f'{lit(d)}^^xsd:{kind}' if kind else lit(d)


def unit_iri(project: str, uid: str) -> str:
    return f"{_a().base(project)}unit:{urllib.parse.quote(uid, safe='')}"


def agent_iri(project: str, name: str) -> str:
    return f"{_a().base(project)}agent:{urllib.parse.quote(name, safe='')}"


def turtle(project: str, units: list[dict]) -> str:
    """The arrangement in RiC-O."""
    by_name = {h["name"]: h["iri"] for h in _a().holdings(project)}
    out, agents = [PREFIXES], {}
    for u in units:
        iri = unit_iri(project, u["id"])
        level = u.get("level", "")
        kind = "rico:Record" if level == "item" else "rico:RecordSet"
        lines = [f"<{iri}> a {kind}", f"    as:unit {lit(u['id'])}", f"    as:level {lit(level)}"]
        if level in TYPES:
            lines.append(f"    rico:hasRecordSetType rst:{TYPES[level]}")
        for p, k in (("rico:identifier", "code"), ("rico:title", "title")):
            if u.get(k):
                lines.append(f"    {p} {lit(u[k])}")
        if u.get("creator"):
            a = agent_iri(project, u["creator"])
            agents[a] = u["creator"]
            lines.append(f"    rico:hasCreator <{a}>")
        if u.get("from"):
            lines.append(f"    rico:beginningDate {date_lit(u['from'])}")
        if u.get("to"):
            lines.append(f"    rico:endDate {date_lit(u['to'])}")
        lines.append(f'    rico:rankInSequence "{int(u.get("order", 0)) + 1}"^^xsd:int')
        if u.get("parent"):
            lines.append(f"    rico:isDirectlyIncludedIn <{unit_iri(project, u['parent'])}>")
        out.append(" ;\n".join(lines) + " .\n")
        for h in u.get("holdings", []):
            name = h if isinstance(h, str) else h.get("name", "")
            if name in by_name:
                out.append(f"<{by_name[name]}> rico:isDirectlyIncludedIn <{iri}> .\n")
    for a, name in agents.items():
        out.append(f"<{a}> a rico:Agent ; rico:name {lit(name)} .\n")
    return "\n".join(out)


def read_units(project: str) -> list[dict]:
    """The arrangement read back from linked/arrangement.ttl, by SPARQL."""
    path = host.project_dir(project) / "linked" / "arrangement.ttl"
    if not path.is_file():
        return []
    doc = [linked.document(path.read_text(encoding="utf-8"), "turtle", base=_a().base(project))]
    q = f"""PREFIX rico: <{RICO}> PREFIX as: <{AS}>
SELECT ?u ?id ?level ?code ?title ?cname ?from ?to ?rank ?parent WHERE {{ ?u as:unit ?id .
  OPTIONAL {{ ?u as:level ?level }} OPTIONAL {{ ?u rico:identifier ?code }} OPTIONAL {{ ?u rico:title ?title }}
  OPTIONAL {{ ?u rico:hasCreator ?c . ?c rico:name ?cname }} OPTIONAL {{ ?u rico:beginningDate ?from }} OPTIONAL {{ ?u rico:endDate ?to }}
  OPTIONAL {{ ?u rico:rankInSequence ?rank }} OPTIONAL {{ ?u rico:isDirectlyIncludedIn ?p . ?p as:unit ?parent }} }}"""
    rows = linked.ask("query", documents=doc, query=q)["rows"]
    v = lambda t: (t or {}).get("value", "") if t else ""
    units, seen = [], set()
    for u, uid, level, code, title, cname, a, b, rank, parent in rows:
        if v(uid) in seen:
            continue
        seen.add(v(uid))
        units.append({"id": v(uid), "iri": v(u), "parent": v(parent), "level": v(level), "code": v(code), "title": v(title), "creator": v(cname),
                      "from": v(a), "to": v(b), "order": int(v(rank) or 1) - 1, "holdings": []})
    held = linked.ask("query", documents=doc, query=f"PREFIX rico: <{RICO}> PREFIX as: <{AS}> SELECT ?h ?id WHERE {{ ?h rico:isDirectlyIncludedIn ?u . ?u as:unit ?id . MINUS {{ ?h as:unit ?x }} }}")["rows"]
    by_iri = {h["iri"]: h for h in _a().holdings(project)}
    index = {u["id"]: u for u in units}
    for h, uid in held:
        if v(uid) in index and v(h) in by_iri:
            x = by_iri[v(h)]
            index[v(uid)]["holdings"].append({"name": x["name"], "sha256": x["sha256"], "iri": x["iri"]})
    units.sort(key=lambda u: (u["parent"] != "", u["parent"], u["order"], u["id"]))
    return units


def checked(units: list[dict]) -> dict:
    """archeion's faults and references for an arrangement."""
    if not units:
        return {"faults": [], "units": [], "sound": True}
    r = archeion.ask("arrange", units=[{**u, "holdings": [h["sha256"] or h["name"] for h in u.get("holdings", [])]} for u in units])
    return r


def arrangement(project: str) -> dict:
    units = read_units(project)
    r = checked(units)
    refs = {x["id"]: x for x in r.get("units", [])}
    for u in units:
        info = refs.get(u["id"], {})
        u["reference"] = info.get("reference", "")
        u["described"] = {k: info.get(k, "") for k in ("creator", "from", "to")}
    held = {h["name"] for u in units for h in u["holdings"]}
    loose = [h["name"] for h in _a().holdings(project) if h["name"] not in held]
    return {"units": units, "faults": r.get("faults", []), "sound": r.get("sound", True), "levels": r.get("levels", []), "unarranged": loose}


def keep(project: str, units: list[dict], by: str, what: str = "arranged the archive") -> dict:
    clean = []
    for k, u in enumerate(units):
        uid = str(u.get("id", "")).strip()
        if not uid:
            raise ValueError("every unit has an identifier")
        clean.append({"id": uid, "parent": str(u.get("parent", "") or ""), "level": str(u.get("level", "") or ""), "code": str(u.get("code", "") or ""),
                      "title": str(u.get("title", "") or ""), "creator": str(u.get("creator", "") or ""), "from": str(u.get("from", "") or ""),
                      "to": str(u.get("to", "") or ""), "order": int(u.get("order", k) or 0), "holdings": [h if isinstance(h, str) else h.get("name", "") for h in u.get("holdings", [])]})
    answer = _a().keep_text(project, "arrangement.ttl", turtle(project, clean))
    if "message" in answer:
        raise ValueError(f"the arrangement could not be written: {answer['message']}")
    host.record(project, by, what, f"{len(clean)} units")
    return arrangement(project)


def finding_aid(project: str) -> str:
    p = host.read_project(project)
    units = read_units(project)
    for u in units:
        u["holdings"] = [{"name": h["name"], "title": h["name"], "href": f"holdings/{h['name']}", "sha256": h["sha256"]} for h in u["holdings"]]
    if not units:
        raise ValueError("the archive has no arrangement yet")
    return ead.write(units, f"{p.get('title', project)}: a finding aid", agency="Archive Studio", record_id=project, date=dt.date.today().isoformat())


def from_ead(project: str, xml: bytes, by: str) -> dict:
    read = ead.read(xml)
    by_name = {h["name"] for h in _a().holdings(project)}
    by_sha = {h["sha256"]: h["name"] for h in _a().holdings(project) if h["sha256"]}
    for u in read["units"]:
        names = []
        for h in u["holdings"]:
            n = by_sha.get(h.get("sha256", "")) or (h["name"] if h["name"] in by_name else None)
            if n:
                names.append(n)
        u["holdings"] = names
    return keep(project, read["units"], by, f"read a finding aid ({read['version']})")


# ---- Packages ---------------------------------------------------------------------------------------------------------------------

def package(project: str, unit: str | None, by: str) -> tuple[bytes, str]:
    units = read_units(project)
    refs = {x["id"]: x.get("reference", "") for x in checked(units).get("units", [])}
    chosen = units
    if unit:
        keep_ids, todo = set(), [unit]
        while todo:
            x = todo.pop()
            keep_ids.add(x)
            todo += [u["id"] for u in units if u["parent"] == x]
        chosen = [u for u in units if u["id"] in keep_ids]
        if not chosen:
            raise KeyError(f"there is no unit '{unit}'")
    folder = host.project_dir(project)
    payload = {}
    for u in chosen:
        ref = refs.get(u["id"], u["id"]) or u["id"]
        for h in u["holdings"]:
            payload[f"{ref}/{h['name']}"] = str(folder / "holdings" / h["name"])
    if not unit:
        held = {h["name"] for u in units for h in u["holdings"]}
        for h in _a().holdings(project):
            if h["name"] not in held:
                payload[f"unarranged/{h['name']}"] = str(folder / "holdings" / h["name"])
    with tempfile.TemporaryDirectory() as tmp:
        t = Path(tmp)
        description = {}
        if units:
            (t / "finding-aid.xml").write_text(finding_aid(project), encoding="utf-8")
            description["description/finding-aid.xml"] = str(t / "finding-aid.xml")
        (t / "linked-data.trig").write_text(_a().export(project, "trig"), encoding="utf-8")
        description["description/linked-data.trig"] = str(t / "linked-data.trig")
        if host.journal_file(project).is_file():
            description["description/journal.jsonl"] = str(host.journal_file(project))
        p = host.read_project(project)
        info = [["Source-Organization", "Archive Studio"], ["External-Description", f"{p.get('title', project)}" + (f", the unit {unit}" if unit else "")],
                ["External-Identifier", project + (f"/{unit}" if unit else "")], ["Bag-Group-Identifier", project]]
        r = archeion.ask("package", payload={k: archeion.path(v) for k, v in {**payload, **description}.items()}, info=info, date=dt.date.today().isoformat())
        buf = io.BytesIO()
        with zipfile.ZipFile(buf, "w", zipfile.ZIP_DEFLATED) as z:
            root = f"{project}{'-' + re.sub(r'[^A-Za-z0-9_-]', '_', unit) if unit else ''}-bag"
            for name, text in r["tags"].items():
                z.writestr(f"{root}/{name}", text)
            for path, file in {**payload, **description}.items():
                z.write(file, f"{root}/data/{path}")
    host.record(project, by, "made a package", unit or "the whole archive", f"{len(payload)} holdings; {r['verdict']['reason']}")
    return buf.getvalue(), f"{root}.zip"


# ---- Custody ------------------------------------------------------------------------------------------------------------------------

def sha_of(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def custody(project: str) -> dict:
    path = host.project_dir(project) / "linked" / "custody.ttl"
    hs = _a().holdings(project)
    events = []
    if path.is_file():
        doc = [linked.document(path.read_text(encoding="utf-8"), "turtle", base=_a().base(project))]
        q = f"""PREFIX prov: <{PROV}> PREFIX as: <{AS}> PREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#>
SELECT ?e ?kind ?h ?from ?to ?when ?digest ?note WHERE {{ ?e prov:used ?h ; a ?kind ; as:to ?to ; prov:startedAtTime ?when ; as:digest ?digest .
  FILTER(?kind = as:Intake || ?kind = as:Transfer) OPTIONAL {{ ?e as:from ?from }} OPTIONAL {{ ?e rdfs:comment ?note }} }} ORDER BY ?when"""
        for e, kind, h, a, b, when, digest, note in linked.ask("query", documents=doc, query=q)["rows"]:
            events.append({"kind": "intake" if kind["value"].endswith("Intake") else "transfer", "holding": h["value"], "from": (a or {}).get("value", ""),
                           "to": b["value"], "when": when["value"], "digest": digest["value"], "note": (note or {}).get("value", "")})
    titles = {}
    try:
        rows = linked.ask("query", documents=_a().documents(project, ["holdings.ttl"]), query="PREFIX dcterms: <http://purl.org/dc/terms/> SELECT ?h ?t ?c ?d WHERE { ?h dcterms:identifier ?i OPTIONAL { ?h dcterms:title ?t } OPTIONAL { ?h dcterms:creator ?c } OPTIONAL { ?h dcterms:date ?d } }")["rows"]
        for h, t, c, d in rows:
            titles.setdefault(h["value"], {"title": (t or {}).get("value", ""), "creator": (c or {}).get("value", ""), "date": (d or {}).get("value", "")})
    except (KeyError, linked.EngineError):
        pass
    chains, request = [], []
    folder = host.project_dir(project) / "holdings"
    for h in hs:
        mine = [e for e in events if e["holding"] == h["iri"]]
        intake = next((e for e in mine if e["kind"] == "intake"), None)
        if not intake:
            chains.append({"holding": h["name"], "events": [], "verdict": None})
            continue
        transfers = [e for e in mine if e["kind"] == "transfer"]
        request.append({"holding": h["name"], "digest": intake["digest"], "custodian": intake["to"], "since": intake["when"],
                        "transfers": [{"when": e["when"], "from": e["from"], "to": e["to"], "digest": e["digest"], "note": e["note"]} for e in transfers],
                        "observed": sha_of(folder / h["name"]), "attributes": {"title": titles.get(h["iri"], {}).get("title", ""), "date": titles.get(h["iri"], {}).get("date", "")}})
        chains.append({"holding": h["name"], "events": mine, "verdict": None})
    if request:
        answers = {a["holding"]: a for a in archeion.ask("custody", chains=request)["chains"]}
        for c in chains:
            if c["holding"] in answers:
                c.update({k: answers[c["holding"]][k] for k in ("verdict", "breaks", "custodian", "digest", "authentic")})
    return {"chains": chains}


def hand_over(project: str, body: dict, by: str, answer: bool = True) -> dict | None:
    name = str(body.get("holding", ""))
    h = next((x for x in _a().holdings(project) if x["name"] == name), None)
    if not h:
        raise KeyError(f"there is no holding '{name}'")
    kind = body.get("kind", "transfer")
    if kind not in ("intake", "transfer"):
        raise ValueError("a custody event is an intake or a transfer")
    to = " ".join(str(body.get("to", "")).split())
    if not to:
        raise ValueError("say to whom it is handed")
    when = str(body.get("when") or dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"))
    if not re.fullmatch(r"\d{4}-\d{2}-\d{2}(T\d{2}:\d{2}(:\d{2})?Z?)?", when):
        raise ValueError("a moment is written YYYY-MM-DD or YYYY-MM-DDThh:mm:ssZ")
    if len(when) == 10:
        when += "T00:00:00Z"
    digest = str(body.get("digest") or sha_of(host.project_dir(project) / "holdings" / name))
    eid = f"{_a().base(project)}custody:{dt.datetime.now(dt.timezone.utc).strftime('%Y%m%dT%H%M%S%f')}"
    ttl = f"""<{eid}> a prov:Activity, as:{'Intake' if kind == 'intake' else 'Transfer'} ;
    prov:used <{h['iri']}> ;
    {f'as:from {lit(body["from"])} ;' if body.get("from") else ''}
    as:to {lit(to)} ;
    prov:startedAtTime "{when}"^^xsd:dateTime ;
    as:digest {lit(digest)}{(' ;' + chr(10) + '    rdfs:comment ' + lit(body['note'])) if body.get('note') else ''} ."""
    path = host.project_dir(project) / "linked" / "custody.ttl"
    old = path.read_text(encoding="utf-8") if path.is_file() else PREFIXES
    answer = _a().keep_text(project, "custody.ttl", old.rstrip("\n") + "\n\n" + ttl + "\n")
    if "message" in answer:
        raise ValueError(answer["message"])
    host.record(project, by, "took a holding in" if kind == "intake" else "handed a holding over", name, f"to {to}")
    return custody(project) if answer else None


def verify_journal(project: str) -> dict:
    j = host.journal(project)
    if not j["entries"]:
        return {"entries": 0, "verdict": {"status": "holds", "reason": "the journal is empty"}}
    r = archeion.ask("journal", entries=j["entries"], head=j["head"])
    return {"entries": len(j["entries"]), "verdict": r["verdict"], "head": j["head"]}


def routes(handler, method: str, parts: list[str], query: dict, project: str, what: str, rest: list[str], exists) -> bool:
    """The arrangement's routes; True when this answered."""
    if what == "arrangement.xml" and method == "GET":
        def xml():
            exists()
            return finding_aid(project)
        try:
            data = xml().encode("utf-8")
        except (KeyError, ValueError) as e:
            handler.reply_json({"message": str(e)}, 404 if isinstance(e, KeyError) else 400)
            return True
        handler.send_response(200)
        handler.send_header("Content-Type", "application/xml; charset=utf-8")
        handler.send_header("Content-Disposition", f'attachment; filename="{project}-finding-aid.xml"')
        handler.send_header("Content-Length", str(len(data)))
        handler.end_headers()
        handler.wfile.write(data)
        return True
    if what == "arrangement" and method == "GET" and not rest:
        handler.answer(lambda: (exists(), (arrangement(project), 200))[1])
        return True
    if what == "custody" and method == "GET":
        handler.answer(lambda: (exists(), (custody(project), 200))[1])
        return True
    if what == "journal" and rest == ["verify"] and method == "GET":
        if host.PEOPLE["on"] and host.person(handler) is None:
            handler.reply_json({"message": "a project is seen by this server's people"}, 403)
            return True
        handler.answer(lambda: (exists(), (verify_journal(project), 200))[1])
        return True
    if method not in ("PUT", "POST") or what not in ("arrangement", "package", "custody"):
        return False
    role = "contributor" if what == "package" else "keeper"
    refusal = host.write_refusal(handler, role) if host.PEOPLE["on"] else host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    by = host.who(handler)
    if what == "arrangement" and method == "PUT" and not rest:
        body = handler.json_body(8 * 1024 * 1024)
        if body is not None:
            handler.answer(lambda: (exists(), (keep(project, body.get("units", []), by), 200))[1])
        return True
    if what == "arrangement" and rest == ["ead"] and method == "POST":
        raw = handler.body(32 * 1024 * 1024)
        if raw is not None:
            handler.answer(lambda: (exists(), (from_ead(project, raw, by), 200))[1])
        return True
    if what == "custody" and method == "POST":
        body = handler.json_body(1024 * 1024)
        if body is not None:
            handler.answer(lambda: (exists(), (hand_over(project, body, by), 200))[1])
        return True
    if what == "package" and method == "POST":
        body = handler.json_body(1024 * 1024)
        if body is None:
            return True
        try:
            exists()
            data, name = package(project, body.get("unit") or None, by)
        except KeyError as e:
            handler.reply_json({"message": str(e.args[0])}, 404)
            return True
        except (ValueError, linked.EngineError) as e:
            handler.reply_json({"message": str(e)}, 400)
            return True
        handler.send_response(200)
        handler.send_header("Content-Type", "application/zip")
        handler.send_header("Content-Disposition", f'attachment; filename="{name}"')
        handler.send_header("Content-Length", str(len(data)))
        handler.end_headers()
        handler.wfile.write(data)
        return True
    return False
