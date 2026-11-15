"""IIIF: images served in tiles at any scale (Image API 3.0), and manifests that present a work's images, sounds and texts with
their annotations (Presentation API 3.0), written and read.

The International Image Interoperability Framework is how libraries, archives and museums publish images so that any viewer
(Mirador, the Universal Viewer, a page's own) can zoom into them, cut regions out and lay annotations on them.

Image API 3.0 (https://iiif.io/api/image/3.0/). A request is ``{id}/{region}/{size}/{rotation}/{quality}.{format}``:
- ``region()`` reads ``full``, ``square``, ``x,y,w,h`` and ``pct:x,y,w,h`` (§4.1), clipped to the image; a region outside it,
  or with no width or height, is refused.
- ``size()`` reads ``max``, ``w,``, ``,h``, ``pct:n``, ``w,h`` and ``!w,h``, each with ``^`` to allow upscaling (§4.2). Without
  ``^``, a size larger than the region is refused, as the API requires.
- ``rotation()`` reads ``n`` and ``!n`` (mirrored first, §4.3). Multiples of 90° are served; other angles are refused with 501,
  as the API allows a server that does not support them.
- Qualities ``default``, ``color``, ``gray`` and ``bitonal`` (§4.4); formats ``jpg``, ``png`` and ``webp`` (§4.5).
- ``info()`` writes the image's information document (§5): its size, its tiles and the scale factors that cover it, the
  compliance level ``level2`` and what is offered beyond it.
- ``serve()`` answers a request's path with a status, a media type and the bytes, rendered by Pillow.

Presentation API 3.0 (https://iiif.io/api/presentation/3.0/):
- ``manifest()`` writes a manifest: each item a Canvas painted with its image (with its image service), its sound or its text,
  and its annotations as an AnnotationPage after the Web Annotation model.
- ``read_manifest()`` reads a manifest of version 3 or 2 into canvases: label, size, duration, the image service or the image,
  and the annotations a version 3 manifest carries inline.

Errors are ``IIIFError`` with the HTTP status the API asks for (400 for a request that is wrong, 501 for one not supported). Pillow
renders; the rest is the standard library. A manifest comes from elsewhere: it is read as data, and nothing in it is fetched or
executed here.
"""

from __future__ import annotations

import io
import math
import re

IMAGE_CONTEXT = "http://iiif.io/api/image/3/context.json"
PRESENTATION_CONTEXT = "http://iiif.io/api/presentation/3/context.json"
FORMATS = {"jpg": "image/jpeg", "png": "image/png", "webp": "image/webp"}
QUALITIES = ("default", "color", "gray", "bitonal")
TILE = 512


class IIIFError(ValueError):
    def __init__(self, status: int, message: str):
        super().__init__(message)
        self.status = status


def _bad(message: str, status: int = 400):
    raise IIIFError(status, message)


def _number(s: str) -> float:
    if not re.fullmatch(r"\d+(\.\d+)?", s):
        _bad(f"'{s}' is not a number")
    return float(s)


# ---- Image API 3.0 -------------------------------------------------------------------------------------------------------------

def region(spec: str, width: int, height: int) -> tuple[int, int, int, int]:
    """The region (x, y, w, h) in pixels of an image of width × height (§4.1)."""
    if spec == "full":
        return 0, 0, width, height
    if spec == "square":
        s = min(width, height)
        return (width - s) // 2, (height - s) // 2, s, s
    pct = spec.startswith("pct:")
    parts = (spec[4:] if pct else spec).split(",")
    if len(parts) != 4:
        _bad(f"the region '{spec}' is not full, square, x,y,w,h or pct:x,y,w,h")
    if pct:
        x, y, w, h = (_number(p) for p in parts)
        x, y, w, h = x * width / 100, y * height / 100, w * width / 100, h * height / 100
        x, y, w, h = round(x), round(y), round(w), round(h)
    else:
        if not all(re.fullmatch(r"\d+", p) for p in parts):
            _bad(f"the region '{spec}' must be whole pixels")
        x, y, w, h = (int(p) for p in parts)
    if w <= 0 or h <= 0:
        _bad("a region has a width and a height")
    if x >= width or y >= height:
        _bad("the region lies outside the image")
    return x, y, min(w, width - x), min(h, height - y)


def size(spec: str, rw: int, rh: int, max_width: int | None = None, max_height: int | None = None, max_area: int | None = None) -> tuple[int, int]:
    """The size (w, h) a region of rw × rh is scaled to (§4.2), within the server's limits."""
    up = spec.startswith("^")
    s = spec[1:] if up else spec
    mw, mh = max_width or 10**9, max_height or max_width or 10**9
    if s == "max":
        scale = min(1.0 if not up else math.inf, mw / rw, mh / rh)
        if max_area:
            scale = min(scale, math.sqrt(max_area / (rw * rh)))
        w, h = max(1, round(rw * scale)), max(1, round(rh * scale))
    elif s.startswith("pct:"):
        n = _number(s[4:])
        if n <= 0 or (n > 100 and not up):
            _bad("a percentage above 100 needs ^, and one of 0 gives nothing")
        w, h = max(1, round(rw * n / 100)), max(1, round(rh * n / 100))
    else:
        confined = s.startswith("!")
        if confined:
            s = s[1:]
        m = re.fullmatch(r"(\d*),(\d*)", s)
        if not m or (not m.group(1) and not m.group(2)) or (confined and not (m.group(1) and m.group(2))):
            _bad(f"the size '{spec}' is not max, w,, ,h, pct:n, w,h or !w,h")
        a, b = int(m.group(1) or 0), int(m.group(2) or 0)
        if (m.group(1) and a == 0) or (m.group(2) and b == 0):
            _bad("a size of 0 gives nothing")
        if confined:
            scale = min(a / rw, b / rh)
            w, h = max(1, round(rw * scale)), max(1, round(rh * scale))
        elif a and b:
            w, h = a, b
        elif a:
            w, h = a, max(1, round(rh * a / rw))
        else:
            w, h = max(1, round(rw * b / rh)), b
    if not up and (w > rw or h > rh):
        _bad("a size larger than its region needs ^")
    if w > mw or h > mh or (max_area and w * h > max_area):
        _bad("the size asked for is beyond the server's limits")
    return w, h


def rotation(spec: str) -> tuple[bool, int]:
    """Whether to mirror, and the rotation in degrees clockwise (§4.3); only multiples of 90 are served."""
    mirror = spec.startswith("!")
    n = _number(spec[1:] if mirror else spec)
    if n < 0 or n > 360:
        _bad("a rotation lies between 0 and 360")
    if n % 90:
        _bad("only rotations by multiples of 90° are served here", 501)
    return mirror, int(n) % 360


def scale_factors(width: int, height: int, tile: int = TILE) -> list[int]:
    """The powers of two by which the image is scaled down until it fits one tile."""
    out, f = [1], 1
    while math.ceil(width / f) > tile or math.ceil(height / f) > tile:
        f *= 2
        out.append(f)
    return out


def info(identifier: str, width: int, height: int, tile: int = TILE, max_area: int | None = None) -> dict:
    """The image information document (§5)."""
    factors = scale_factors(width, height, tile)
    d = {"@context": IMAGE_CONTEXT, "id": identifier, "type": "ImageService3", "protocol": "http://iiif.io/api/image",
         "profile": "level2", "width": width, "height": height,
         "sizes": [{"width": math.ceil(width / f), "height": math.ceil(height / f)} for f in reversed(factors)],
         "tiles": [{"width": tile, "scaleFactors": factors}],
         "extraFormats": ["webp"], "extraQualities": ["gray", "bitonal"], "extraFeatures": ["mirroring", "rotationBy90s", "sizeUpscaling"]}
    if max_area:
        d["maxArea"] = max_area
    return d


def parse(path: str) -> dict:
    """The parameters of an image request's path after the identifier: region/size/rotation/quality.format."""
    parts = path.strip("/").split("/")
    if len(parts) != 4:
        _bad("an image request is region/size/rotation/quality.format")
    quality, _, fmt = parts[3].rpartition(".")
    if quality not in QUALITIES:
        _bad(f"the quality '{quality}' is not one of {', '.join(QUALITIES)}")
    if fmt not in FORMATS:
        _bad(f"the format '{fmt}' is not served (jpg, png, webp)")
    return {"region": parts[0], "size": parts[1], "rotation": parts[2], "quality": quality, "format": fmt}


def render(image, params: dict, max_area: int | None = None) -> bytes:
    """The bytes of a request on an image opened with Pillow."""
    from PIL import Image, ImageOps   # needed to render, not to parse
    width, height = image.size
    x, y, w, h = region(params["region"], width, height)
    sw, sh = size(params["size"], w, h, max_area=max_area)
    mirror, angle = rotation(params["rotation"])
    out = image.crop((x, y, x + w, y + h))
    if (sw, sh) != (w, h):
        out = out.resize((sw, sh), Image.LANCZOS)
    if mirror:
        out = ImageOps.mirror(out)
    if angle:
        out = out.rotate(-angle, expand=True)
    q = params["quality"]
    if q == "gray":
        out = out.convert("L")
    elif q == "bitonal":
        out = out.convert("L").point(lambda v: 255 if v >= 128 else 0).convert("1")
    elif out.mode not in ("RGB", "L") and params["format"] == "jpg":
        out = out.convert("RGB")
    elif out.mode == "P":
        out = out.convert("RGBA")
    buf = io.BytesIO()
    kind = {"jpg": "JPEG", "png": "PNG", "webp": "WEBP"}[params["format"]]
    if kind == "JPEG" and out.mode not in ("RGB", "L", "1"):
        out = out.convert("RGB")
    out.save(buf, kind, **({"quality": 88} if kind in ("JPEG", "WEBP") else {}))
    return buf.getvalue()


def serve(open_image, identifier: str, rest: str, max_area: int | None = 40_000_000) -> tuple[int, str, bytes]:
    """The answer to a request: ``rest`` is ``info.json`` or region/size/rotation/quality.format; ``open_image`` gives the image
    opened with Pillow. Returns (status, media type, bytes)."""
    import json
    try:
        image = open_image()
        if rest in ("", "info.json"):
            body = json.dumps(info(identifier, image.size[0], image.size[1], max_area=max_area)).encode("utf-8")
            return 200, "application/ld+json;profile=\"http://iiif.io/api/image/3/context.json\"", body
        p = parse(rest)
        return 200, FORMATS[p["format"]], render(image, p, max_area)
    except IIIFError as e:
        return e.status, "text/plain; charset=utf-8", str(e).encode("utf-8")


# ---- Presentation API 3.0 -----------------------------------------------------------------------------------------------------

def _label(text, language: str = "none") -> dict:
    if isinstance(text, dict):
        return text
    return {language: [str(text)]}


def label_text(label, prefer: str = "en") -> str:
    """A language map (or a version 2 label) as one string, in the language preferred, else none, else the first."""
    if label is None:
        return ""
    if isinstance(label, str):
        return label
    if isinstance(label, list):
        return "; ".join(label_text(x, prefer) for x in label)
    if isinstance(label, dict):
        if "@value" in label:
            return str(label["@value"])
        for k in (prefer, "none", *label.keys()):
            if k in label and label[k]:
                v = label[k]
                return "; ".join(v) if isinstance(v, list) else str(v)
    return str(label)


def manifest(identifier: str, label, items: list[dict], metadata: list[tuple[str, str]] | None = None, summary: str = "",
             rights: str = "", attribution: str = "", language: str = "none") -> dict:
    """A manifest. Each item is {"id", "label", "kind": "Image" | "Sound" | "Video" | "Text", "url", "format", "width"?, "height"?,
    "duration"?, "service"? (an image service's id), "annotations"?: [{"id", "motivation", "body": text, "target": fragment
    such as "xywh=10,20,30,40" or "t=1.5,2.5", "creator"?, "created"?}]}."""
    canvases = []
    for k, it in enumerate(items):
        cid = it.get("canvas") or f"{identifier.rsplit('/', 1)[0]}/canvas/{k + 1}"
        body = {"id": it["url"], "type": it.get("kind", "Image"), "format": it.get("format", "")}
        canvas = {"id": cid, "type": "Canvas", "label": _label(it.get("label", f"item {k + 1}"), language)}
        if it.get("width") and it.get("height"):
            canvas["width"], canvas["height"] = int(it["width"]), int(it["height"])
            body["width"], body["height"] = int(it["width"]), int(it["height"])
        if it.get("duration"):
            canvas["duration"] = body["duration"] = float(it["duration"])
        if it.get("service"):
            body["service"] = [{"id": it["service"], "type": "ImageService3", "profile": "level2"}]
        canvas["items"] = [{"id": f"{cid}/page/painting", "type": "AnnotationPage",
                            "items": [{"id": f"{cid}/annotation/painting", "type": "Annotation", "motivation": "painting", "body": body, "target": cid}]}]
        notes = it.get("annotations") or []
        if notes:
            page = []
            for n, a in enumerate(notes):
                target = cid + (f"#{a['target']}" if a.get("target") else "")
                anno = {"id": a.get("id") or f"{cid}/annotation/{n + 1}", "type": "Annotation", "motivation": a.get("motivation", "commenting"),
                        "body": {"type": "TextualBody", "value": a.get("body", ""), "format": "text/plain"}, "target": target}
                if a.get("creator"):
                    anno["creator"] = a["creator"]
                if a.get("created"):
                    anno["created"] = a["created"]
                page.append(anno)
            canvas["annotations"] = [{"id": f"{cid}/page/annotations", "type": "AnnotationPage", "items": page}]
        canvases.append(canvas)
    m = {"@context": PRESENTATION_CONTEXT, "id": identifier, "type": "Manifest", "label": _label(label, language), "items": canvases}
    if metadata:
        m["metadata"] = [{"label": _label(k, language), "value": _label(v, language)} for k, v in metadata]
    if summary:
        m["summary"] = _label(summary, language)
    if rights:
        m["rights"] = rights
    if attribution:
        m["requiredStatement"] = {"label": _label("Attribution", "en"), "value": _label(attribution, language)}
    return m


def _service_id(service) -> str:
    for s in service if isinstance(service, list) else [service]:
        if isinstance(s, dict):
            sid = s.get("id") or s.get("@id")
            if sid:
                return sid.rstrip("/")
    return ""


def read_manifest(m: dict) -> dict:
    """A manifest (version 3, or 2) read into {"version", "id", "label", "summary", "metadata": [(label, value)], "canvases":
    [{"id", "label", "width", "height", "duration", "kind", "url", "format", "service", "annotations": [{"motivation", "body",
    "target"}]}]}."""
    if not isinstance(m, dict):
        _bad("a manifest is a JSON object")
    v3 = m.get("type") == "Manifest" or "items" in m
    v2 = m.get("@type") == "sc:Manifest" or "sequences" in m
    if not (v3 or v2):
        _bad("this is not a IIIF manifest of version 3 or 2")
    out = {"version": 3 if v3 else 2, "id": m.get("id") or m.get("@id", ""), "label": label_text(m.get("label")),
           "summary": label_text(m.get("summary") or m.get("description")), "metadata": [], "canvases": []}
    for md in m.get("metadata", []) or []:
        if isinstance(md, dict):
            out["metadata"].append((label_text(md.get("label")), label_text(md.get("value"))))
    if v3:
        for c in m.get("items", []) or []:
            if not isinstance(c, dict) or c.get("type") != "Canvas":
                continue
            canvas = {"id": c.get("id", ""), "label": label_text(c.get("label")), "width": c.get("width"), "height": c.get("height"),
                      "duration": c.get("duration"), "kind": "", "url": "", "format": "", "service": "", "annotations": []}
            for page in c.get("items", []) or []:
                for a in (page.get("items", []) if isinstance(page, dict) else []):
                    if a.get("motivation") != "painting":
                        continue
                    body = a.get("body", {})
                    if isinstance(body, list):
                        body = body[0] if body else {}
                    if body.get("type") == "Choice":
                        body = (body.get("items") or [{}])[0]
                    canvas.update(kind=body.get("type", ""), url=body.get("id", ""), format=body.get("format", ""), service=_service_id(body.get("service", [])))
                    break
            for page in c.get("annotations", []) or []:
                for a in (page.get("items", []) if isinstance(page, dict) else []):
                    body = a.get("body", {})
                    if isinstance(body, list):
                        body = body[0] if body else {}
                    target = a.get("target", "")
                    if isinstance(target, dict):
                        target = target.get("id", "") or target.get("source", "")
                    canvas["annotations"].append({"motivation": a.get("motivation", ""), "body": body.get("value", "") if isinstance(body, dict) else str(body),
                                                  "target": target.split("#", 1)[1] if "#" in str(target) else ""})
            out["canvases"].append(canvas)
    else:
        for seq in m.get("sequences", []) or []:
            for c in seq.get("canvases", []) or []:
                canvas = {"id": c.get("@id", ""), "label": label_text(c.get("label")), "width": c.get("width"), "height": c.get("height"),
                          "duration": None, "kind": "Image", "url": "", "format": "", "service": "", "service_v2": True, "annotations": []}
                for img in c.get("images", []) or []:
                    res = img.get("resource", {})
                    canvas.update(url=res.get("@id", ""), format=res.get("format", ""), service=_service_id(res.get("service", {})))
                    break
                out["canvases"].append(canvas)
    return out


def image_url(canvas: dict, width: int | None = None) -> str:
    """Where to fetch a canvas's image, by its service when it has one (version 3's or version 2's path)."""
    if canvas.get("service"):
        whole = "full" if canvas.get("service_v2") else "max"   # version 2.0 knows no "max"; "full" is its whole image
        return f"{canvas['service']}/full/{f'{width},' if width else whole}/0/default.jpg"
    return canvas.get("url", "")
