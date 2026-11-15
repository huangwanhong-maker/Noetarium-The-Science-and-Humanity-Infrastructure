"""The Archive Studio's own resources, served beside the studio host's (ADR 0119): a project's linked data, its holdings, and the catalogue of
social-science studies.

A project of the Archive Studio is a folder of the studio host (project.json, data/, scripts/, results/, boards/) with two more:

    linked/      its linked data: Turtle (.ttl), TriG (.trig), N-Triples (.nt), N-Quads (.nq) and JSON-LD (.jsonld) files, each kept
                 as text, refused when it does not read (with its line and column), and edited by changes that come back with their
                 undo; a JSON-LD file's remote contexts are fetched here, kept in the cache, and given to the engine
    codebooks/   the codebooks of its tables (datasets.py)
    holdings/    files of any kind (images, sound, video, documents, tables), each kept under its name with its SHA-256; each is
                 described in linked/holdings.ttl (Dublin Core terms and the DCMI types), and annotated in linked/annotations.ttl
                 after the W3C Web Annotation model

    GET  /api/projects/<p>/linked                       the files of linked data
    GET  /api/projects/<p>/linked/<file>                one file's text, format and SHA-256
    PUT  /api/projects/<p>/linked/<file>                {"text", "base"?, "base_text"?}: keep it, if it reads; {"remove": true}: remove it.
                                                        When the file changed since it was read (its SHA-256 is not "base"), the
                                                        change is merged by statements: what this writer added and removed since
                                                        "base_text", applied to what is there now; without "base_text", refused (409)
    GET  /api/projects/<p>/export.<trig|nq|jsonld>      the project's linked data in one document
    GET  /api/projects/<p>/codings                      the passages coded with codes (oa:classifying), by whom, as tables for the tools
    GET  /api/forms                                     the forms of description (forms/*.ttl: SHACL shapes), parsed
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

Writes keep the host's rule (write_refusal): only the work's own page, and with --token only with it; with --people, by the role of the
person who asks. Every change is written in the project's journal (studio_host.record). Everything computed on linked data is
symploke's, asked of the work's engine through science.linked. The arrangement, custody and packages are arrangement.py's, the
codebooks datasets.py's, IIIF presentation.py's (ADR 0120).
"""

from __future__ import annotations

import datetime as dt
import hashlib
import urllib.request
import io
import json
import math
import mimetypes
import re
import struct
import sys
import threading
import urllib.parse
import wave
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))

from science import linked  # noqa: E402
from science import studio_host as host  # noqa: E402

import arrangement  # noqa: E402
import datasets  # noqa: E402
import presentation  # noqa: E402

FILE = re.compile(r'^(?![.\s])[^\\/:*?"<>|\x00-\x1f]{1,120}(?<![.\s])$')  # any name a folder can hold: no separators, no reserved characters, not hidden
HOLDING_LIMIT = 512 * 1024 * 1024
BY: dict = {}   # thread → who asks, for what a request's helpers write in the journal
TEXT_LIMIT = 32 * 1024 * 1024
CATALOGUE = ROOT / "data" / "cache" / "social-studies" / "catalogue.jsonl"
EXAMPLE = APP / "examples" / "reading-room"
CONTEXTS = ROOT / "data" / "cache" / "archive-studio" / "contexts"
FORMS = APP / "forms"
EXPORTS = {"trig": ("trig", "application/trig"), "nq": ("nquads", "application/n-quads"), "jsonld": ("jsonld", "application/ld+json")}

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


def remote_contexts(text: str, depth: int = 0, found: dict | None = None) -> dict:
    """The remote contexts a JSON-LD document cites (by @context and @import), fetched once and kept in the cache: {IRI: JSON text}.
    The engine reads no network; what cannot be fetched is left out, and the engine then names it."""
    found = {} if found is None else found
    try:
        doc = json.loads(text)
    except ValueError:
        return found
    wanted = []

    def walk(x, in_context=False):
        if isinstance(x, dict):
            for k, v in x.items():
                if k == "@context":
                    walk(v, True)
                elif k == "@import" and isinstance(v, str):
                    wanted.append(v)
                else:
                    walk(v, in_context and k not in ("@context",))
        elif isinstance(x, list):
            for v in x:
                walk(v, in_context)
        elif isinstance(x, str) and in_context and re.match(r"^https?://", x):
            wanted.append(x)
    walk(doc)
    for url in wanted:
        if url in found:
            continue
        cached = CONTEXTS / (hashlib.sha256(url.encode("utf-8")).hexdigest() + ".jsonld")
        if not cached.is_file():
            try:
                req = urllib.request.Request(url, headers={"Accept": "application/ld+json, application/json", "User-Agent": "Noetarium-ArchiveStudio/1"})
                with urllib.request.urlopen(req, timeout=20) as r:  # noqa: S310 (http or https only, checked above)
                    data = r.read(4 * 1024 * 1024 + 1)
                if len(data) > 4 * 1024 * 1024:
                    continue
                CONTEXTS.mkdir(parents=True, exist_ok=True)
                cached.write_bytes(data)
            except Exception:  # noqa: BLE001 (a context that cannot be fetched is named by the engine)
                continue
        found[url] = cached.read_text(encoding="utf-8", errors="replace")
        if depth < 4:
            remote_contexts(found[url], depth + 1, found)
    return found


def make_document(text: str, suffix: str, project: str) -> dict:
    fmt = linked.FORMATS[suffix]
    return linked.document(text, fmt, base=base(project), contexts=remote_contexts(text) if fmt == "jsonld" else None)


def document_of(path: Path, project: str) -> dict:
    return make_document(path.read_text(encoding="utf-8"), path.suffix, project)


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
        raise ValueError("a file of linked data ends in .ttl, .trig, .nt, .nq or .jsonld")
    parsed = linked.ask("parse", document=make_document(text, path.suffix, project))
    if "error" in parsed:
        return {"message": parsed["error"], "line": parsed["line"], "column": parsed["column"], **({"missing": parsed["missing"]} if parsed.get("missing") else {})}
    host.replace_file(path, text)  # whole or not at all, under the host's lock of files
    return {"kept": name, "statements": parsed["count"]}


def append_turtle(project: str, name: str, turtle: str) -> dict:
    """Add statements (Turtle, without prefixes) to a file, creating it with the studio's prefixes."""
    path = host.project_dir(project) / "linked" / name
    old = path.read_text(encoding="utf-8") if path.is_file() else PREFIXES
    return keep_text(project, name, old.rstrip("\n") + "\n\n" + turtle.strip() + "\n")


def literal(text: str) -> str:
    return '"' + str(text).replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n").replace("\r", "") + '"'


def sha(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def merged(project: str, path: Path, base_text: str, mine: str, theirs: str) -> dict:
    """A change merged by statements: what this writer added and removed since base_text, applied to what is there now. Statements
    are sets, so two writers never contradict each other: one's additions and the other's removals both stand. The file is written
    back by the engine, so its comments and layout are not kept, and that is said."""
    parse = lambda text: linked.ask("parse", document=make_document(text, path.suffix, project))
    b, m, t = parse(base_text), parse(mine), parse(theirs)
    for which, r in (("the version first read", b), ("yours", m), ("the one there now", t)):
        if "error" in r:
            raise ValueError(f"{which} does not read: {r['error']}")
    k = lambda q: json.dumps(q, sort_keys=True)
    bs, ms = {k(q) for q in b["quads"]}, {k(q) for q in m["quads"]}
    added, removed = ms - bs, bs - ms
    result = [q for q in t["quads"] if k(q) not in removed] + [json.loads(x) for x in sorted(added) if x not in {k(q) for q in t["quads"]}]
    prefixes = {**m["prefixes"], **t["prefixes"]}
    fmt = linked.FORMATS[path.suffix]
    if fmt == "turtle" and any(len(q) > 3 for q in result):
        raise ValueError("the merged statements fall in named graphs, which a Turtle file cannot hold")
    text = linked.ask("write", quads=result, prefixes=prefixes, format=fmt)["text"]
    host.replace_file(path, text)
    return {"kept": path.name, "statements": len(result), "merged": True, "yours": {"added": len(added), "removed": len(removed)},
            "note": "the file changed since you read it: your statements were merged with the others', and the file written back without its comments"}


def export(project: str, fmt: str) -> str:
    """The project's linked data in one document, each file's statements together (a Turtle file's in the default graph)."""
    quads, prefixes = [], {}
    for p in files_of(project):
        r = linked.ask("parse", document=document_of(p, project))
        if "error" in r:
            raise ValueError(f"{p.name} does not read: {r['error']}")
        quads += r["quads"]
        prefixes.update(r["prefixes"])
    return linked.ask("write", quads=quads, prefixes=prefixes, format=EXPORTS[fmt][0])["text"]


def forms() -> list[dict]:
    out = []
    for f in sorted(FORMS.glob("*.ttl")):
        r = linked.ask("parse", document=linked.document(f.read_text(encoding="utf-8"), "turtle"))
        if "error" not in r:
            out.append({"id": f.stem, "quads": r["quads"]})
    return out


def utf8_offset(text: str, units: int) -> int:
    """The byte offset in UTF-8 of an offset in UTF-16 code units, as a page counts them."""
    return len(text.encode("utf-16-le")[: 2 * units].decode("utf-16-le", errors="ignore").encode("utf-8"))


def codings(project: str) -> dict:
    """The passages coded (oa:classifying, each body a code) as two tables: the codings (document, begin, end, quote, code, coder), in
    bytes of UTF-8 as zetesis counts them, and the documents (document, case, text)."""
    hs = {h["iri"]: h for h in holdings(project)}
    docs = documents(project, [n for n in ("annotations.ttl", "codes.ttl") if (host.project_dir(project) / "linked" / n).is_file()])
    q = """PREFIX oa: <http://www.w3.org/ns/oa#> PREFIX dcterms: <http://purl.org/dc/terms/> PREFIX skos: <http://www.w3.org/2004/02/skos/core#>
SELECT ?a ?src ?code ?label ?exact ?start ?end ?creator WHERE { ?a oa:motivatedBy oa:classifying ; oa:hasBody ?code ; oa:hasTarget ?t . ?t oa:hasSource ?src .
  OPTIONAL { ?code skos:prefLabel ?label } OPTIONAL { ?t oa:hasSelector ?s . ?s oa:exact ?exact OPTIONAL { ?s oa:refinedBy ?r . ?r oa:start ?start ; oa:end ?end } }
  OPTIONAL { ?a dcterms:creator ?creator } }"""
    rows = linked.ask("query", documents=docs, query=q)["rows"] if docs else []
    out = {"document": [], "begin": [], "end": [], "quote": [], "code": [], "coder": []}
    texts, seen = {}, set()
    for a, src, code, label, exact, start, end, creator in rows:
        h = hs.get(src["value"])
        if not h or a["value"] in seen:
            continue
        seen.add(a["value"])
        if h["name"] not in texts:
            texts[h["name"]] = (host.project_dir(project) / "holdings" / h["name"]).read_text(encoding="utf-8", errors="replace")
        text = texts[h["name"]]
        quote = (exact or {}).get("value", "")
        if start and end:
            b0, b1 = utf8_offset(text, int(start["value"])), utf8_offset(text, int(end["value"]))
        else:
            i = text.find(quote)
            if i < 0:
                continue
            b0 = len(text[:i].encode("utf-8"))
            b1 = b0 + len(quote.encode("utf-8"))
        out["document"].append(h["name"]), out["begin"].append(b0), out["end"].append(b1), out["quote"].append(quote)
        out["code"].append((label or {}).get("value") or code["value"].rsplit(":", 1)[-1]), out["coder"].append((creator or {}).get("value", "") or "unnamed")
    documents_ = {"document": list(texts), "case": list(texts), "text": [texts[n] for n in texts]}
    return {"codings": out, "documents": documents_}


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
    host.record(project, BY.get(threading.get_ident(), host.PEOPLE["keeper"]), "kept a holding", target.name, f"SHA-256 {digest}")
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


def synthetic_facsimile(width: int = 4800, height: int = 3400) -> bytes:
    """A page of a register drawn here, large enough to be read in tiles: ruled paper, columns, lines of a hand that writes nothing,
    a stamp. Synthetic, for the example, and said to be in its name."""
    import random
    from PIL import Image, ImageDraw, ImageFilter, ImageFont
    rng = random.Random(1931)
    grain = Image.effect_noise((width // 8, height // 8), 22).resize((width, height), Image.BICUBIC)
    img = Image.merge("RGB", [grain.point(lambda v, o=o: max(0, min(255, int(o + (v - 128) * 0.12)))) for o in (236, 226, 204)])
    d = ImageDraw.Draw(img)
    for y in range(420, height - 160, 72):
        d.line([(200, y), (width - 200, y)], fill=(160, 180, 198), width=2)
    for x, c in ((420, (186, 88, 92)), (1500, (150, 150, 170)), (3000, (150, 150, 170)), (3900, (150, 150, 170))):
        d.line([(x, 300), (x, height - 140)], fill=c, width=3)
    try:
        font, small = ImageFont.load_default(size=96), ImageFont.load_default(size=44)
    except TypeError:
        font = small = ImageFont.load_default()
    d.text((480, 150), "Register of readers, 1931", fill=(62, 48, 70), font=font)
    d.text((480, 300), "a synthetic facsimile, drawn by the Archive Studio for its example", fill=(110, 92, 110), font=small)
    for k, head in enumerate(["Date", "Reader", "Material asked for", "Returned"]):
        d.text(((240, 520, 1560, 3060)[k], 350), head, fill=(90, 70, 90), font=small)
    for y in range(420 + 72, height - 200, 72):
        for x0, x1 in ((230, 400), (440, 1470), (1520, 2970), (3020, 3870)):
            x, base = x0 + rng.randint(0, 40), y - 18
            pts = []
            while x < x1 - rng.randint(0, (x1 - x0) // 2):
                for t in range(10):
                    pts.append((x + t * 2.4, base - 14 * abs(__import__("math").sin(t * 0.7 + x * 0.05)) + rng.uniform(-2, 2)))
                x += 24 + rng.randint(0, 10)
                if rng.random() < 0.18:
                    if len(pts) > 1:
                        d.line(pts, fill=(48, 40, 66), width=3, joint="curve")
                    pts, x = [], x + rng.randint(12, 30)
            if len(pts) > 1:
                d.line(pts, fill=(48, 40, 66), width=3, joint="curve")
    cx, cy = width - 700, height - 600
    for r, w in ((230, 8), (190, 4)):
        d.ellipse([cx - r, cy - r, cx + r, cy + r], outline=(150, 70, 100), width=w)
    d.text((cx - 150, cy - 30), "READING ROOM", fill=(150, 70, 100), font=small)
    img = img.filter(ImageFilter.GaussianBlur(0.7))
    out = io.BytesIO()
    img.save(out, "JPEG", quality=86)
    return out.getvalue()


def example(project: str) -> dict:
    """The example archive laid out in a project: invented holdings, their descriptions, links, annotations, passages coded by two
    people, an arrangement with one fault, chains of custody with one break, codebooks of its surveys, a vocabulary and shapes."""
    made = {}
    for path in sorted((EXAMPLE / "holdings").iterdir()):
        made[path.name] = add_holding(project, path.name, path.read_bytes())
    made["sound"] = add_holding(project, "A sounding of the listening room (synthetic).wav", synthetic_sound())
    made["facsimile"] = add_holding(project, "Register of readers, 1931 (synthetic facsimile).jpg", synthetic_facsimile())
    b = base(project)
    for path in sorted((EXAMPLE / "linked").iterdir()):
        if path.suffix not in linked.FORMATS:
            continue
        answer = keep_text(project, path.name, path.read_text(encoding="utf-8").replace("{base}", b))
        if "message" in answer:
            raise ValueError(f"the example's {path.name} does not read: {answer['message']}")
    h = {k: v["iri"] for k, v in made.items()}
    minutes, readers, plan, sound, facsimile = (h["Minutes of the reading-room committee.txt"], h["Readers of the reading room (illustrative).csv"],
                                                h["Plan of the reading room.svg"], h["sound"], h["facsimile"])
    # Described further; the table is left without its maker and the sound with a date in words, for the shapes to find.
    append_turtle(project, "holdings.ttl", f"""@prefix ex: <{b}> .
<{minutes}> a ex:Minutes ; dcterms:creator <{b}committee> ; dcterms:date "1931-10-14" ; dcterms:isPartOf <{b}fonds> ;
    dcterms:subject "access", "lighting", "catalogue" ; dcterms:description "An invented record of a meeting, written for this example." .
<{plan}> a ex:Plan ; dcterms:creator <{b}brandt> ; dcterms:date "1931" ; dcterms:isPartOf <{b}fonds> ; dcterms:relation <{minutes}> .
<{readers}> dcterms:date "1932" ; dcterms:isPartOf <{b}fonds> ; dcterms:description "An illustrative table, invented for this example." .
<{sound}> dcterms:creator <{b}okafor> ; dcterms:date "spring 1931" ; dcterms:isPartOf <{b}fonds> ; dcterms:description "Three tones synthesised by the studio, standing for a recording." .
<{facsimile}> dcterms:creator <{b}committee> ; dcterms:date "1931" ; dcterms:isPartOf <{b}fonds> ; dcterms:description "A page of a register drawn by the studio, large enough to be read in tiles." .""")
    for key, title, date in (("Survey of readers, 1932 (illustrative).csv", "A survey of readers, 1932", "1932"), ("Survey of readers, 1934 (illustrative).csv", "A survey of readers, 1934", "1934"),
                             ("Reading rooms compared (illustrative).csv", "Reading rooms compared", "1933")):
        append_turtle(project, "holdings.ttl", f"<{h[key]}> dcterms:title {literal(title)} ; dcterms:creator <{b}committee> ; dcterms:date {literal(date)} ; dcterms:isPartOf <{b}fonds> ; "
                                               f"dcterms:description \"Invented for the example.\" .")
    append_turtle(project, "annotations.ttl", f"""<{b}annotation:maps> a oa:Annotation ; oa:motivatedBy oa:commenting ;
    oa:hasBody [ a oa:TextualBody ; rdf:value "the map cases, where the glare of item 2 falls" ] ;
    oa:hasTarget [ oa:hasSource <{plan}> ; oa:hasSelector [ a oa:FragmentSelector ; dcterms:conformsTo <http://www.w3.org/TR/media-frags/> ; rdf:value "xywh=460,80,110,260" ] ] .
<{b}annotation:registers> a oa:Annotation ; oa:motivatedBy oa:commenting ;
    oa:hasBody [ a oa:TextualBody ; rdf:value "the rule of access to the parish registers" ] ;
    oa:hasTarget [ oa:hasSource <{minutes}> ; oa:hasSelector [ a oa:TextQuoteSelector ; oa:exact "the parish registers may be read by appointment" ] ] .
<{b}annotation:second> a oa:Annotation ; oa:motivatedBy oa:commenting ;
    oa:hasBody [ a oa:TextualBody ; rdf:value "the second tone" ] ;
    oa:hasTarget [ oa:hasSource <{sound}> ; oa:hasSelector [ a oa:FragmentSelector ; dcterms:conformsTo <http://www.w3.org/TR/media-frags/> ; rdf:value "t=1.30,2.50" ] ] .
<{b}annotation:stamp> a oa:Annotation ; oa:motivatedBy oa:commenting ;
    oa:hasBody [ a oa:TextualBody ; rdf:value "the reading room's stamp" ] ;
    oa:hasTarget [ oa:hasSource <{facsimile}> ; oa:hasSelector [ a oa:FragmentSelector ; dcterms:conformsTo <http://www.w3.org/TR/media-frags/> ; rdf:value "xywh=3830,2530,540,540" ] ] .""")
    # Passages of the minutes coded by two people, who agree on most and differ on two.
    text = (EXAMPLE / "holdings" / "Minutes of the reading-room committee.txt").read_text(encoding="utf-8")
    passages = [("the card catalogue is now complete", "catalogue", "catalogue"), ("the photographs are still uncatalogued", "catalogue", "collections"),
                ("the lamps by the north windows be replaced", "lighting", "lighting"), ("readers of the maps complain of the glare", "lighting", "lighting"),
                ("the parish registers may be read by appointment", "access", "access"), ("the oral histories recorded last spring", "access", "collections")]
    coded = []
    for k, (quote, brandt, okafor) in enumerate(passages):
        at = text.index(quote)
        for who, code in (("L. Brandt", brandt), ("J. Okafor", okafor)):
            aid = f"{b}annotation:code-{k}-{who.split()[-1].lower()}"
            coded.append(f"""<{aid}> a oa:Annotation ; oa:motivatedBy oa:classifying ; dcterms:creator {literal(who)} ; oa:hasBody <{b}code-{code}> ;
    oa:hasTarget [ oa:hasSource <{minutes}> ; oa:hasSelector [ a oa:TextQuoteSelector ; oa:exact {literal(quote)} ;
        oa:refinedBy [ a oa:TextPositionSelector ; oa:start "{at}"^^xsd:nonNegativeInteger ; oa:end "{at + len(quote)}"^^xsd:nonNegativeInteger ] ] ] .""")
    append_turtle(project, "annotations.ttl", "\n".join(coded))
    # The arrangement: a fonds, three series, files; one file ends after its series does, for archeion to find.
    keeper = host.PEOPLE["keeper"]
    arrangement.keep(project, [
        {"id": "fonds", "level": "fonds", "code": "RR", "title": "Papers of the reading room", "creator": "The reading-room committee", "from": "1928", "to": "1934"},
        {"id": "minutes", "parent": "fonds", "level": "series", "code": "1", "title": "Minutes", "from": "1930", "to": "1932", "order": 0},
        {"id": "minutes-1931", "parent": "minutes", "level": "file", "code": "1", "title": "Minutes of 1931", "from": "1931", "to": "1931", "order": 0,
         "holdings": ["Minutes of the reading-room committee.txt"]},
        {"id": "plans", "parent": "fonds", "level": "series", "code": "2", "title": "Plans and registers", "from": "1929", "to": "1933", "order": 1},
        {"id": "plan", "parent": "plans", "level": "file", "code": "1", "title": "The plan of the room", "from": "1931", "to": "1931", "order": 0,
         "holdings": ["Plan of the reading room.svg"]},
        {"id": "register", "parent": "plans", "level": "file", "code": "2", "title": "The register of readers", "from": "1931", "to": "1935", "order": 1,
         "holdings": ["Register of readers, 1931 (synthetic facsimile).jpg", "Readers of the reading room (illustrative).csv"]},
        {"id": "surveys", "parent": "fonds", "level": "series", "code": "3", "title": "Surveys", "from": "1932", "to": "1934", "order": 2,
         "holdings": ["Survey of readers, 1932 (illustrative).csv", "Survey of readers, 1934 (illustrative).csv", "Reading rooms compared (illustrative).csv"]},
        {"id": "sounds", "parent": "fonds", "level": "series", "code": "4", "title": "Recordings", "from": "1931", "to": "1931", "order": 3,
         "holdings": ["A sounding of the listening room (synthetic).wav"]},
    ], keeper, "arranged the example archive")
    # Custody: each holding taken in by the committee's secretary, handed to the county record office, then to this archive; one
    # handover of the table is written as made by someone who never held it, for archeion to find.
    for name in made.values():
        n = name["name"]
        arrangement.hand_over(project, {"holding": n, "kind": "intake", "to": "the committee's secretary", "when": "1934-12-01T10:00:00Z", "note": "taken in when the committee closed"}, keeper, False)
        giver = "the parish clerk" if n.startswith("Readers of") else "the committee's secretary"
        arrangement.hand_over(project, {"holding": n, "kind": "transfer", "from": giver, "to": "the county record office", "when": "1962-05-14T09:00:00Z"}, keeper, False)
        arrangement.hand_over(project, {"holding": n, "kind": "transfer", "from": "the county record office", "to": "the Archive Studio", "when": "2026-11-15T09:00:00Z"}, keeper, False)
    # The codebooks of the two surveys, each holding the digest of the file it describes.
    for path in sorted((EXAMPLE / "codebooks").glob("*.json")):
        book = json.loads(path.read_text(encoding="utf-8"))
        for f in book["files"]:
            if f["name"] in made:
                f["digest"] = made[f["name"]]["sha256"]
        datasets.keep(project, path.stem, book, keeper, "kept the example's codebook")
    host.record(project, keeper, "laid out the example archive", "Papers of the reading room", "every piece invented, and said to be")
    return {"holdings": len(made), "files": len(files_of(project)), "codebooks": len(datasets.names(project))}


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
    BY[threading.get_ident()] = host.who(handler)
    if presentation.routes(handler, method, parts):
        return True
    if parts[:2] == ["api", "catalogue"] and method == "GET":
        handler.answer(lambda: (catalogue(query), 200))
        return True
    if parts[:2] == ["api", "forms"] and method == "GET":
        handler.answer(lambda: ({"forms": forms()}, 200))
        return True
    OWN = ("linked", "ask", "holdings", "catalogue", "example", "arrangement", "arrangement.xml", "package", "custody", "journal", "codebooks", "iiif", "codings",
           "export.trig", "export.nq", "export.jsonld")
    if not (len(parts) >= 4 and parts[:2] == ["api", "projects"] and parts[3] in OWN):
        return False
    project, what, rest = parts[2], parts[3], parts[4:]
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the archive is seen by its people: ask the keeper for a key"}, 403)
        return True

    def exists():
        if not (host.project_dir(project) / "project.json").is_file():
            raise KeyError(f"there is no project '{project}'")

    if arrangement.routes(handler, method, parts, query, project, what, rest, exists):
        return True
    if datasets.routes(handler, method, parts, query, project, what, rest, exists):
        return True
    if what.startswith("export.") and method == "GET":
        fmt = what.split(".", 1)[1]
        try:
            exists()
            data = export(project, fmt).encode("utf-8")
        except (KeyError, ValueError, linked.EngineError) as e:
            handler.reply_json({"message": str(e)}, 400)
            return True
        handler.send_response(200)
        handler.send_header("Content-Type", EXPORTS[fmt][1])
        handler.send_header("Content-Disposition", f'attachment; filename="{project}.{fmt}"')
        handler.send_header("Content-Length", str(len(data)))
        handler.end_headers()
        handler.wfile.write(data)
        return True
    if what == "codings" and method == "GET":
        handler.answer(lambda: (exists(), (codings(project), 200))[1])
        return True

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
                text = path.read_text(encoding="utf-8")
                return {"file": path.name, "format": linked.FORMATS.get(path.suffix, "turtle"), "text": text, "sha256": sha(text)}, 200
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
            path = host.project_dir(project) / "linked" / name
            if body.get("remove"):
                if path.is_file():
                    path.unlink()
                host.record(project, host.who(handler), "removed a file of linked data", name)
                return {"removed": name}, 200
            text = str(body.get("text", ""))
            if body.get("base") and path.is_file():
                now = path.read_text(encoding="utf-8")
                if sha(now) != body["base"]:
                    if isinstance(body.get("base_text"), str) and sha(body["base_text"]) == body["base"]:
                        answer = merged(project, path, body["base_text"], text, now)
                        host.record(project, host.who(handler), "merged a change to a file of linked data", name,
                                    f"{answer['yours']['added']} added and {answer['yours']['removed']} removed over another's change")
                        return answer, 200
                    return {"message": "the file changed since it was read", "current": now, "sha256": sha(now)}, 409
            answer = keep_text(project, name, text)
            if "message" not in answer:
                host.record(project, host.who(handler), "kept a file of linked data", name, f"{answer['statements']} statements")
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
            host.record(project, host.who(handler), "edited the linked data", path.name, f"{len(body.get('added', []))} added, {len(body.get('removed', []))} removed")
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
    if method == "POST" and what == "iiif" and not rest:
        body = handler.json_body(TEXT_LIMIT)
        if body is not None:
            handler.answer(lambda: (exists(), (presentation.take(project, str(body.get("url", "")), host.who(handler)), 200))[1])
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
                        kept = append_turtle(project, "studies.ttl", study_turtle(row))
                        host.record(project, host.who(handler), "took a study from the catalogue", row.get("title", wanted))
                        return kept, 200
            raise KeyError(f"the catalogue has no study {wanted}")
        handler.answer(take)
        return True
    return False
