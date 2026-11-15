"""The Archive Studio's own resources, served beside the studio host's (ADR 0119): a project's linked data, its holdings, and the catalogue of
social-science studies.

A project of the Archive Studio is a folder of the studio host (project.json, data/, scripts/, results/, boards/) with two more:

    linked/      its linked data: Turtle (.ttl), TriG (.trig), N-Triples (.nt) and N-Quads (.nq) files, each kept as text, refused
                 when it does not read (with its line and column), and edited by changes that come back with their undo
    holdings/    files of any kind (images, sound, video, documents, tables), each kept under its name with its SHA-256; each is
                 described in linked/holdings.ttl (Dublin Core terms and the DCMI types), and annotated in linked/annotations.ttl
                 after the W3C Web Annotation model

    GET  /api/projects/<p>/linked                       the files of linked data
    GET  /api/projects/<p>/linked/<file>                one file's text and format
    PUT  /api/projects/<p>/linked/<file>                {"text"}: keep it, if it reads; {"remove": true}: remove it
    POST /api/projects/<p>/linked/<file>/edit           {"removed", "added"}: a change applied to one file, kept, and its undo
    POST /api/projects/<p>/ask                          {"op", ...}: an operation of science.linked (summary, describe, query,
                                                        validate, entail, compare) over the project's files of linked data
                                                        ("files": a subset; "shapes": a file read as the shapes graph)
    GET  /api/projects/<p>/holdings                     the holdings, with their sizes and digests
    POST /api/projects/<p>/holdings?name=…               a file's bytes: kept, digested, described
    GET  /api/projects/<p>/holdings/<file>              a holding's bytes, for the page's viewers
    GET  /api/catalogue?q=&offset=&limit=               studies of the CESSDA Data Catalogue as harvested into the data cache
    POST /api/projects/<p>/catalogue                    {"id"}: a study written into linked/studies.ttl in DISCO, the DDI
                                                        Alliance's vocabulary for the discovery of studies
    POST /api/projects/<p>/example                      lays out the example archive (examples/reading-room): four holdings,
                                                        one of them a sound synthesised here, described, linked, annotated,
                                                        with a small vocabulary and shapes; every piece invented and said to be

Writes keep the host's rule (write_refusal): only the work's own page, and with --token only with it. Everything computed on linked
data is symploke's, asked of the work's engine through science.linked.
"""

from __future__ import annotations

import datetime as dt
import hashlib
import io
import json
import math
import mimetypes
import re
import struct
import sys
import urllib.parse
import wave
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import linked  # noqa: E402
from science import studio_host as host  # noqa: E402

FILE = re.compile(r'^(?![.\s])[^\\/:*?"<>|\x00-\x1f]{1,120}(?<![.\s])$')  # any name a folder can hold: no separators, no reserved characters, not hidden
HOLDING_LIMIT = 512 * 1024 * 1024
TEXT_LIMIT = 32 * 1024 * 1024
CATALOGUE = ROOT / "data" / "cache" / "social-studies" / "catalogue.jsonl"
EXAMPLE = APP / "examples" / "reading-room"

PREFIXES = """@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .
@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .
@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .
@prefix dcterms: <http://purl.org/dc/terms/> .
@prefix dcmitype: <http://purl.org/dc/dcmitype/> .
@prefix oa: <http://www.w3.org/ns/oa#> .
@prefix skos: <http://www.w3.org/2004/02/skos/core#> .
@prefix disco: <http://rdf-vocabulary.ddialliance.org/discovery#> .
@prefix schema: <https://schema.org/> .
"""

KINDS = {"image": "Image", "audio": "Sound", "video": "MovingImage", "text": "Text"}
# The types of the common holdings, the same on every computer (Windows' registry would call a table application/vnd.ms-excel).
for _type, _ext in (("text/csv", ".csv"), ("text/tab-separated-values", ".tsv"), ("audio/wav", ".wav"), ("image/svg+xml", ".svg"), ("text/markdown", ".md"),
                    ("text/turtle", ".ttl"), ("application/trig", ".trig"), ("application/n-triples", ".nt"), ("application/n-quads", ".nq"), ("audio/mpeg", ".mp3"),
                    ("audio/ogg", ".ogg"), ("video/mp4", ".mp4"), ("video/webm", ".webm"), ("application/pdf", ".pdf"), ("application/json", ".json")):
    mimetypes.add_type(_type, _ext, strict=True)


def base(project: str) -> str:
    return f"urn:archive:{urllib.parse.quote(project, safe='')}:"


def files_of(project: str) -> list[Path]:
    folder = host.project_dir(project) / "linked"
    return sorted(p for p in folder.glob("*") if p.suffix in linked.FORMATS) if folder.is_dir() else []


def document_of(path: Path, project: str) -> dict:
    return linked.document(path.read_text(encoding="utf-8"), linked.FORMATS[path.suffix], base=base(project))


def documents(project: str, names: list[str] | None = None) -> list[dict]:
    chosen = files_of(project)
    if names:
        wanted = set(names)
        chosen = [p for p in chosen if p.name in wanted]
        missing = wanted - {p.name for p in chosen}
        if missing:
            raise KeyError(f"the project has no file of linked data {sorted(missing)[0]}")
    return [document_of(p, project) for p in chosen]


def check_name(name: str) -> str:
    if not FILE.match(name) or ".." in name:
        raise ValueError(f"'{name}' is not a file's name this studio keeps")
    return name


def keep_text(project: str, name: str, text: str) -> dict:
    """Keep a file of linked data if it reads; its statements counted."""
    path = host.project_dir(project) / "linked" / check_name(name)
    if path.suffix not in linked.FORMATS:
        raise ValueError("a file of linked data ends in .ttl, .trig, .nt or .nq")
    parsed = linked.ask("parse", document=linked.document(text, linked.FORMATS[path.suffix], base=base(project)))
    if "error" in parsed:
        return {"message": parsed["error"], "line": parsed["line"], "column": parsed["column"]}
    host.replace_file(path, text)  # whole or not at all, under the host's lock of files
    return {"kept": name, "statements": parsed["count"]}


def append_turtle(project: str, name: str, turtle: str) -> dict:
    """Add statements (Turtle, without prefixes) to a file, creating it with the studio's prefixes."""
    path = host.project_dir(project) / "linked" / name
    old = path.read_text(encoding="utf-8") if path.is_file() else PREFIXES
    return keep_text(project, name, old.rstrip("\n") + "\n\n" + turtle.strip() + "\n")


def literal(text: str) -> str:
    return '"' + str(text).replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n").replace("\r", "") + '"'


def holdings(project: str) -> list[dict]:
    folder = host.project_dir(project) / "holdings"
    out = []
    if folder.is_dir():
        for p in sorted(folder.iterdir()):
            if p.is_file() and not p.name.endswith(".sha256"):
                digest = (folder / (p.name + ".sha256")).read_text(encoding="utf-8").strip() if (folder / (p.name + ".sha256")).is_file() else None
                mime = mimetypes.guess_type(p.name)[0] or "application/octet-stream"
                out.append({"name": p.name, "bytes": p.stat().st_size, "sha256": digest, "type": mime, "iri": base(project) + "holding:" + urllib.parse.quote(p.name, safe="")})
    return out


def add_holding(project: str, name: str, data: bytes) -> dict:
    folder = host.project_dir(project) / "holdings"
    folder.mkdir(parents=True, exist_ok=True)
    stem, suffix = Path(check_name(name)).stem, Path(name).suffix
    target, k = folder / name, 1
    while target.exists():
        target, k = folder / f"{stem} ({k}){suffix}", k + 1
    digest = hashlib.sha256(data).hexdigest()
    with host.FILES:
        target.write_bytes(data)
        (folder / (target.name + ".sha256")).write_text(digest + "\n", encoding="utf-8")
    mime = mimetypes.guess_type(target.name)[0] or "application/octet-stream"
    kind = "Dataset" if mime in ("text/csv", "text/tab-separated-values", "application/json") else KINDS.get(mime.split("/")[0], "Text" if mime == "application/pdf" else "PhysicalObject")
    iri = base(project) + "holding:" + urllib.parse.quote(target.name, safe="")
    now = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    described = append_turtle(project, "holdings.ttl", f"""<{iri}> a dcmitype:{kind} ;
    dcterms:title {literal(target.stem)} ;
    dcterms:format {literal(mime)} ;
    dcterms:extent "{len(data)} bytes" ;
    dcterms:identifier "sha256:{digest}" ;
    dcterms:dateSubmitted "{now}"^^xsd:dateTime ;
    schema:contentUrl {literal('holdings/' + target.name)} .""")
    return {"name": target.name, "iri": iri, "sha256": digest, "bytes": len(data), "type": mime, "described": described}


def synthetic_sound(seconds: float = 4.0, rate: int = 16000) -> bytes:
    """Three notes rising, each with a decaying envelope: a sound made here, for the example, and said to be synthetic."""
    frames = bytearray()
    notes = [(440.0, 0.0), (554.37, 1.3), (659.25, 2.6)]
    for i in range(int(seconds * rate)):
        t = i / rate
        v = sum(math.sin(2 * math.pi * f * (t - t0)) * math.exp(-2.2 * (t - t0)) for f, t0 in notes if t >= t0)
        frames += struct.pack("<h", int(max(-1.0, min(1.0, 0.32 * v)) * 32767))
    out = io.BytesIO()
    with wave.open(out, "wb") as w:
        w.setnchannels(1), w.setsampwidth(2), w.setframerate(rate)
        w.writeframes(bytes(frames))
    return out.getvalue()


def example(project: str) -> dict:
    """The example archive laid out in a project: invented holdings, their descriptions, links, annotations, a vocabulary and shapes."""
    made = {}
    for path in sorted((EXAMPLE / "holdings").iterdir()):
        made[path.name] = add_holding(project, path.name, path.read_bytes())
    made["sound"] = add_holding(project, "A sounding of the listening room (synthetic).wav", synthetic_sound())
    b = base(project)
    for path in sorted((EXAMPLE / "linked").glob("*.ttl")):
        answer = keep_text(project, path.name, path.read_text(encoding="utf-8").replace("{base}", b))
        if "message" in answer:
            raise ValueError(f"the example's {path.name} does not read: {answer['message']}")
    h = {k: v["iri"] for k, v in made.items()}
    minutes, readers, plan, sound = (h["Minutes of the reading-room committee.txt"], h["Readers of the reading room (illustrative).csv"], h["Plan of the reading room.svg"], h["sound"])
    # Described further; the table is left without its maker and the sound with a date in words, for the shapes to find.
    append_turtle(project, "holdings.ttl", f"""@prefix ex: <{b}> .
<{minutes}> a ex:Minutes ; dcterms:creator <{b}committee> ; dcterms:date "1931-10-14" ; dcterms:isPartOf <{b}fonds> ;
    dcterms:subject "access", "lighting", "catalogue" ; dcterms:description "An invented record of a meeting, written for this example." .
<{plan}> a ex:Plan ; dcterms:creator <{b}brandt> ; dcterms:date "1931" ; dcterms:isPartOf <{b}fonds> ; dcterms:relation <{minutes}> .
<{readers}> dcterms:date "1932" ; dcterms:isPartOf <{b}fonds> ; dcterms:description "An illustrative table, invented for this example." .
<{sound}> dcterms:creator <{b}okafor> ; dcterms:date "spring 1931" ; dcterms:isPartOf <{b}fonds> ; dcterms:description "Three tones synthesised by the studio, standing for a recording." .""")
    append_turtle(project, "annotations.ttl", f"""<{b}annotation:maps> a oa:Annotation ; oa:motivatedBy oa:commenting ;
    oa:hasBody [ a oa:TextualBody ; rdf:value "the map cases, where the glare of item 2 falls" ] ;
    oa:hasTarget [ oa:hasSource <{plan}> ; oa:hasSelector [ a oa:FragmentSelector ; dcterms:conformsTo <http://www.w3.org/TR/media-frags/> ; rdf:value "xywh=460,80,110,260" ] ] .
<{b}annotation:registers> a oa:Annotation ; oa:motivatedBy oa:commenting ;
    oa:hasBody [ a oa:TextualBody ; rdf:value "the rule of access to the parish registers" ] ;
    oa:hasTarget [ oa:hasSource <{minutes}> ; oa:hasSelector [ a oa:TextQuoteSelector ; oa:exact "the parish registers may be read by appointment" ] ] .
<{b}annotation:second> a oa:Annotation ; oa:motivatedBy oa:commenting ;
    oa:hasBody [ a oa:TextualBody ; rdf:value "the second tone" ] ;
    oa:hasTarget [ oa:hasSource <{sound}> ; oa:hasSelector [ a oa:FragmentSelector ; dcterms:conformsTo <http://www.w3.org/TR/media-frags/> ; rdf:value "t=1.30,2.50" ] ] .""")
    return {"holdings": len(made), "files": len(files_of(project))}


def catalogue(query: dict) -> dict:
    if not CATALOGUE.is_file():
        return {"studies": [], "total": 0, "harvested": False,
                "message": "no catalogue has been harvested: run python data/products/social_studies.py (the CESSDA Data Catalogue)"}
    words = [w.lower() for w in query.get("q", "").split() if w]
    offset, limit = int(query.get("offset", 0) or 0), min(int(query.get("limit", 40) or 40), 200)
    found, total = [], 0
    with CATALOGUE.open(encoding="utf-8") as f:
        for line in f:
            row = json.loads(line)
            hay = " ".join([row.get("title", ""), " ".join(row.get("keywords", [])), " ".join(row.get("topics", [])), " ".join(row.get("authors", [])),
                            " ".join(row.get("countries", [])), row.get("archive", "")]).lower()
            if all(w in hay for w in words):
                if offset <= total < offset + limit:
                    found.append(row)
                total += 1
    return {"studies": found, "total": total, "harvested": True}


def study_turtle(row: dict) -> str:
    """A study in DISCO and Dublin Core terms (DDI-RDF Discovery Vocabulary, DDI Alliance 2013)."""
    iri = f"https://doi.org/{row['doi']}" if row.get("doi") else f"urn:cessda:{row.get('oai', row.get('id', 'study'))}"
    lines = [f"<{iri}> a disco:Study", f"    dcterms:title {literal(row.get('title', ''))}"]
    if row.get("id"):
        lines.append(f"    dcterms:identifier {literal(row['id'])}")
    for a in row.get("authors", []):
        lines.append(f"    dcterms:creator {literal(a)}")
    for d in row.get("distributors", []):
        lines.append(f"    dcterms:publisher {literal(d)}")
    if row.get("released"):
        lines.append(f"    dcterms:issued {literal(row['released'])}")
    for k in row.get("keywords", []):
        lines.append(f"    dcterms:subject {literal(k)}")
    for c in row.get("countries", []):
        lines.append(f"    dcterms:spatial {literal(c)}")
    if row.get("from") or row.get("until"):
        lines.append(f"    dcterms:temporal {literal((row.get('from') or '') + '/' + (row.get('until') or ''))}")
    if row.get("universe"):
        lines.append(f"    disco:universe [ a disco:Universe ; skos:definition {literal(row['universe'])} ]")
    if row.get("mode"):
        lines.append(f"    disco:kindOfData {literal(row['mode'])}")
    if row.get("holdings"):
        lines.append(f"    dcterms:source <{row['holdings']}>")
    return " ;\n".join(lines) + " ."


def routes(handler, method: str, parts: list[str], query: dict) -> bool:
    """The studio's own routes (studio_host.configure(routes=...)); True when this answered."""
    if parts[:2] == ["api", "catalogue"] and method == "GET":
        handler.answer(lambda: (catalogue(query), 200))
        return True
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] in ("linked", "ask", "holdings", "catalogue", "example")):
        return False
    project, what, rest = parts[2], parts[3], parts[4:]

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    if method == "GET":
        if what == "linked" and not rest:
            def listing():
                exists()
                return {"files": [{"file": p.name, "format": linked.FORMATS[p.suffix], "bytes": p.stat().st_size} for p in files_of(project)], "base": base(project)}, 200
            handler.answer(listing)
            return True
        if what == "linked" and len(rest) == 1:
            def text():
                exists()
                path = host.project_dir(project) / "linked" / check_name(urllib.parse.unquote(rest[0]))
                if not path.is_file():
                    raise KeyError("there is no such file")
                return {"file": path.name, "format": linked.FORMATS.get(path.suffix, "turtle"), "text": path.read_text(encoding="utf-8")}, 200
            handler.answer(text)
            return True
        if what == "holdings" and not rest:
            handler.answer(lambda: (exists(), ({"holdings": holdings(project)}, 200))[1])
            return True
        if what == "holdings" and len(rest) == 1:
            name = urllib.parse.unquote(rest[0])
            path = host.project_dir(project) / "holdings" / name
            if not FILE.match(name) or not path.is_file():
                handler.reply_json({"message": "there is no such holding"}, 404)
                return True
            data = path.read_bytes()
            handler.send_response(200)
            handler.send_header("Content-Type", mimetypes.guess_type(name)[0] or "application/octet-stream")
            handler.send_header("Content-Length", str(len(data)))
            handler.send_header("Cache-Control", "no-store")
            handler.end_headers()
            handler.wfile.write(data)
            return True
        return False

    refusal = host.write_refusal(handler) if method in ("PUT", "POST") and what != "ask" else None
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    if method == "PUT" and what == "linked" and len(rest) == 1:
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def put():
            exists()
            name = check_name(urllib.parse.unquote(rest[0]))
            if body.get("remove"):
                path = host.project_dir(project) / "linked" / name
                if path.is_file():
                    path.unlink()
                return {"removed": name}, 200
            answer = keep_text(project, name, str(body.get("text", "")))
            return answer, (400 if "message" in answer else 200)
        handler.answer(put)
        return True
    if method == "POST" and what == "linked" and len(rest) == 2 and rest[1] == "edit":
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def edit():
            exists()
            path = host.project_dir(project) / "linked" / check_name(urllib.parse.unquote(rest[0]))
            doc = document_of(path, project) if path.is_file() else linked.document(PREFIXES, linked.FORMATS.get(path.suffix, "turtle"), base=base(project))
            answer = linked.ask("edit", document=doc, removed=body.get("removed", []), added=body.get("added", []))
            host.replace_file(path, answer["text"])
            return {"file": path.name, "undo": answer["undo"], "statements": answer["statements"]}, 200
        handler.answer(edit)
        return True
    if method == "POST" and what == "ask":
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def ask():
            exists()
            op = body.pop("op", "")
            if op not in ("summary", "describe", "query", "validate", "entail", "compare"):
                raise ValueError(f"no operation '{op}' here")
            files = body.pop("files", None)
            shapes = body.pop("shapes", None)
            docs = documents(project, files)
            if op == "validate":
                if not shapes:
                    raise ValueError("validation needs a file of shapes")
                body["shapes"] = documents(project, [shapes])
                docs = [d for d, p in zip(docs, [f for f in files_of(project) if not files or f.name in files]) if p.name != shapes]
            try:
                return linked.ask(op, documents=docs, **body), 200
            except linked.EngineError as error:
                raise ValueError(str(error)) from error
        handler.answer(ask)
        return True
    if method == "POST" and what == "holdings" and not rest:
        data = handler.body(HOLDING_LIMIT)
        if data is None:
            return True
        handler.answer(lambda: (exists(), (add_holding(project, query.get("name", "holding"), data), 200))[1])
        return True
    if method == "POST" and what == "example" and not rest:
        handler.json_body(TEXT_LIMIT)
        handler.answer(lambda: (exists(), (example(project), 200))[1])
        return True
    if method == "POST" and what == "catalogue" and not rest:
        body = handler.json_body(TEXT_LIMIT)
        if body is None:
            return True

        def take():
            exists()
            wanted = str(body.get("id", ""))
            with CATALOGUE.open(encoding="utf-8") as f:
                for line in f:
                    row = json.loads(line)
                    if row.get("id") == wanted:
                        return append_turtle(project, "studies.ttl", study_turtle(row)), 200
            raise KeyError(f"the catalogue has no study {wanted}")
        handler.answer(take)
        return True
    return False
