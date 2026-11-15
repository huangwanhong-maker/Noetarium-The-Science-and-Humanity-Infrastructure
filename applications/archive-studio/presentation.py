"""The Archive Studio's holdings in IIIF (ADR 0120): every image a IIIF Image API 3.0 service, the archive a Presentation API 3.0
manifest, and a manifest from elsewhere taken in, its images as holdings and its annotations as the archive's.

    GET  /iiif/<p>/<holding>/info.json                         an image's information document (data/formats/iiif.py)
    GET  /iiif/<p>/<holding>/<region>/<size>/<rotation>/<quality>.<format>   a region of it, scaled, turned, in a quality and format
    GET  /iiif/<p>/manifest.json                               the archive as a manifest: its images (with their services), its
                                                               sounds and moving images, and the annotations on them
    POST /api/projects/<p>/iiif                                {"url"}: a manifest of version 3 or 2 taken in (a contributor)

An image is any holding Pillow opens (JPEG, PNG, TIFF, WebP, GIF); an SVG is drawn by the page itself. Images opened are kept for a
while, so that the tiles of one view are cut from one decoding. A manifest from elsewhere is fetched by the server, its images at
their largest size up to 6000 pixels, at most 60 canvases; what cannot be fetched is said.
"""

from __future__ import annotations

import json
import re
import sys
import threading
import urllib.parse
import urllib.request
import wave
from collections import OrderedDict
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))
sys.path.insert(0, str(ROOT / "data" / "formats"))

import iiif  # noqa: E402
from science import linked  # noqa: E402
from science import studio_host as host  # noqa: E402

OPENED: OrderedDict = OrderedDict()   # (path, mtime) → an image decoded
LOCK = threading.Lock()
RASTER = (".jpg", ".jpeg", ".png", ".tif", ".tiff", ".webp", ".gif")
AGENT = "Noetarium-ArchiveStudio/1"


def _a():
    import archive as a
    return a


def opened(path: Path):
    from PIL import Image
    key = (str(path), path.stat().st_mtime)
    with LOCK:
        if key in OPENED:
            OPENED.move_to_end(key)
            return OPENED[key]
    img = Image.open(path)
    img.load()
    with LOCK:
        OPENED[key] = img
        while len(OPENED) > 4:
            OPENED.popitem(last=False)
    return img


def origin(handler) -> str:
    return f"http://{handler.headers.get('Host', '127.0.0.1')}"


def is_raster(name: str) -> bool:
    return Path(name).suffix.lower() in RASTER


def manifest(project: str, at: str) -> dict:
    p = host.read_project(project)
    hs = _a().holdings(project)
    titles, notes = {}, {}
    try:
        rows = linked.ask("query", documents=_a().documents(project, ["holdings.ttl"]), query="PREFIX dcterms: <http://purl.org/dc/terms/> SELECT ?h ?t WHERE { ?h dcterms:identifier ?i OPTIONAL { ?h dcterms:title ?t } }")["rows"]
        titles = {h["value"]: (t or {}).get("value", "") for h, t in rows}
        if (host.project_dir(project) / "linked" / "annotations.ttl").is_file():
            q = """PREFIX oa: <http://www.w3.org/ns/oa#> PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> PREFIX dcterms: <http://purl.org/dc/terms/>
SELECT ?a ?src ?frag ?text ?mot ?creator WHERE { ?a a oa:Annotation ; oa:hasTarget ?t . ?t oa:hasSource ?src .
  OPTIONAL { ?t oa:hasSelector ?s . ?s rdf:value ?frag } OPTIONAL { ?a oa:hasBody ?b . ?b rdf:value ?text } OPTIONAL { ?a oa:motivatedBy ?mot } OPTIONAL { ?a dcterms:creator ?creator } }"""
            for a, src, frag, text, mot, creator in linked.ask("query", documents=_a().documents(project, ["annotations.ttl"]), query=q)["rows"]:
                f = (frag or {}).get("value", "")
                if f and not re.match(r"^(xywh|t)=", f):
                    continue
                notes.setdefault(src["value"], []).append({"id": a["value"], "body": (text or {}).get("value", ""), "target": f,
                                                           "motivation": ((mot or {}).get("value", "") or "commenting").split("#")[-1], "creator": (creator or {}).get("value", "")})
    except (KeyError, linked.EngineError):
        pass
    items = []
    for h in hs:
        q = urllib.parse.quote(h["name"], safe="")
        url = f"{at}/api/projects/{project}/holdings/{q}"
        label = titles.get(h["iri"]) or h["name"]
        if is_raster(h["name"]):
            try:
                w, hgt = opened(host.project_dir(project) / "holdings" / h["name"]).size
            except Exception:  # noqa: BLE001 (a file Pillow cannot open is left out, as an image)
                continue
            items.append({"label": label, "kind": "Image", "url": f"{at}/iiif/{project}/{q}/full/max/0/default.jpg", "format": "image/jpeg", "width": w, "height": hgt,
                          "service": f"{at}/iiif/{project}/{q}", "annotations": notes.get(h["iri"], []), "canvas": f"{at}/iiif/{project}/canvas/{q}"})
        elif h["type"] == "image/svg+xml":
            items.append({"label": label, "kind": "Image", "url": url, "format": h["type"], "width": 1000, "height": 1000, "annotations": notes.get(h["iri"], []),
                          "canvas": f"{at}/iiif/{project}/canvas/{q}"})
        elif h["type"].startswith(("audio/", "video/")):
            duration = None
            if h["name"].lower().endswith(".wav"):
                try:
                    with wave.open(str(host.project_dir(project) / "holdings" / h["name"])) as w:
                        duration = w.getnframes() / w.getframerate()
                except (wave.Error, OSError):
                    pass
            items.append({"label": label, "kind": "Sound" if h["type"].startswith("audio/") else "Video", "url": url, "format": h["type"], "duration": duration or 1.0,
                          "annotations": notes.get(h["iri"], []), "canvas": f"{at}/iiif/{project}/canvas/{q}"})
    return iiif.manifest(f"{at}/iiif/{project}/manifest.json", p.get("title", project), items, summary=f"The archive {p.get('title', project)}, kept in the Archive Studio.")


def fetch(url: str, limit: int) -> bytes:
    if not re.match(r"^https?://", url):
        raise ValueError("a manifest is fetched by http or https")
    req = urllib.request.Request(url, headers={"User-Agent": AGENT, "Accept": "application/ld+json, application/json, image/*;q=0.9, */*;q=0.5"})
    with urllib.request.urlopen(req, timeout=60) as r:  # noqa: S310 (the scheme was checked)
        data = r.read(limit + 1)
    if len(data) > limit:
        raise ValueError(f"{url} is larger than {limit // (1024 * 1024)} MB")
    return data


def take(project: str, url: str, by: str) -> dict:
    """A manifest from elsewhere taken in: its images as holdings, their descriptions and its annotations as the archive's."""
    m = iiif.read_manifest(json.loads(fetch(url, 16 * 1024 * 1024).decode("utf-8")))
    kept, failed, annotated = [], [], 0
    a = _a()
    for k, c in enumerate(m["canvases"][:60]):
        if c["kind"] not in ("Image", "") or not (c["service"] or c["url"]):
            failed.append(f"{c['label'] or k + 1}: not an image")
            continue
        width = min(int(c["width"] or 6000), 6000) if c["service"] else None
        try:
            data = fetch(iiif.image_url(c, width), 96 * 1024 * 1024)
        except Exception as e:  # noqa: BLE001 (what cannot be fetched is said, the rest taken)
            failed.append(f"{c['label'] or k + 1}: {e}")
            continue
        name = re.sub(r'[\\/:*?"<>|\x00-\x1f]+', " ", c["label"] or f"canvas {k + 1}").strip()[:100] or f"canvas {k + 1}"
        made = a.add_holding(project, f"{name}.jpg", data)
        extra = [f"dcterms:source <{c['id']}>"] if c["id"] else []
        if m["id"]:
            extra.append(f"dcterms:isPartOf <{m['id']}>")
        if m["label"]:
            extra.append(f"dcterms:description {a.literal('From the manifest ' + m['label'])}")
        if extra:
            a.append_turtle(project, "holdings.ttl", f"<{made['iri']}> " + " ;\n    ".join(extra) + " .")
        notes = []
        for n, an in enumerate(c["annotations"]):
            if not an.get("body"):
                continue
            selector = (f'; oa:hasSelector [ a oa:FragmentSelector ; dcterms:conformsTo <http://www.w3.org/TR/media-frags/> ; rdf:value {a.literal(an["target"])} ]'
                        if an.get("target") else "")
            aid = f"{a.base(project)}annotation:iiif-{len(kept)}-{n}"
            notes.append(f"<{aid}> a oa:Annotation ; oa:motivatedBy oa:{an.get('motivation') or 'commenting'} ;\n    oa:hasBody [ a oa:TextualBody ; rdf:value {a.literal(an['body'])} ] ;\n"
                         f"    oa:hasTarget [ oa:hasSource <{made['iri']}> {selector} ] .")
        if notes:
            a.append_turtle(project, "annotations.ttl", "\n".join(notes))
            annotated += len(notes)
        kept.append(made["name"])
    host.record(project, by, "took in a IIIF manifest", m["label"] or url, f"{len(kept)} images, {annotated} annotations")
    return {"label": m["label"], "version": m["version"], "kept": kept, "failed": failed, "annotations": annotated, "canvases": len(m["canvases"])}


def routes(handler, method: str, parts: list[str]) -> bool:
    """The IIIF routes under /iiif/; True when this answered."""
    if not parts or parts[0] != "iiif" or method != "GET" or len(parts) < 3:
        return False
    if host.PEOPLE["on"] and host.person(handler) is None:
        handler.reply_json({"message": "the archive is seen by its people"}, 403)
        return True
    project = parts[1]
    if not (host.project_dir(project) / "project.json").is_file():
        handler.reply_json({"message": f"there is no project '{project}'"}, 404)
        return True
    at = origin(handler)
    if parts[2] == "manifest.json" and len(parts) == 3:
        try:
            body = json.dumps(manifest(project, at), ensure_ascii=False).encode("utf-8")
        except (KeyError, ValueError) as e:
            handler.reply_json({"message": str(e)}, 400)
            return True
        return _send(handler, 200, 'application/ld+json;profile="http://iiif.io/api/presentation/3/context.json"', body)
    name = urllib.parse.unquote(parts[2])
    path = host.project_dir(project) / "holdings" / name
    if not _a().FILE.match(name) or not path.is_file() or not is_raster(name):
        handler.reply_json({"message": "there is no such image"}, 404)
        return True
    ident = f"{at}/iiif/{project}/{urllib.parse.quote(name, safe='')}"
    try:
        status, kind, body = iiif.serve(lambda: opened(path), ident, "/".join(parts[3:]))
    except Exception as e:  # noqa: BLE001 (an image Pillow cannot read is said, not raised)
        handler.reply_json({"message": f"the image could not be read: {e}"}, 400)
        return True
    return _send(handler, status, kind, body, cache=status == 200 and not parts[-1] == "info.json")


def _send(handler, status: int, kind: str, body: bytes, cache: bool = False) -> bool:
    handler.send_response(status)
    handler.send_header("Content-Type", kind)
    handler.send_header("Content-Length", str(len(body)))
    handler.send_header("Access-Control-Allow-Origin", "*")   # IIIF is read by viewers elsewhere: Mirador, the Universal Viewer
    if cache:
        handler.send_header("Cache-Control", "max-age=3600")
    handler.end_headers()
    handler.wfile.write(body)
    return True
