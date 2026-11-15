"""Checks of IIIF (data/formats/iiif.py) and EAD (data/formats/ead.py), offline.

IIIF: the Image API's own worked examples of regions and sizes (§4.1, §4.2) on its 6000 × 4000 image, upscaling refused without
^, rotations other than by 90° refused with 501, the information document's tiles covering the image, a tile rendered from an
image whose every pixel is known and found to be the pixels asked for, mirrored and gray; a manifest written with an image, a
sound and their annotations, and read back as it was, and a version 2 manifest read. EAD: an arrangement written as EAD3 and read
back unit for unit, with its levels, codes, dates, creators, holdings and original order; reference codes written in full;
an EAD 2002 finding aid read.

    python data/tests/check_iiif_ead.py
"""

from __future__ import annotations

import io
import json

from checks import assert_, skip

import ead
import iiif


def refused(f, status=400):
    try:
        f()
    except iiif.IIIFError as e:
        return e.status == status
    return False


# ---- Image API: the specification's examples (6000 × 4000) --------------------------------------------------------------------
W, H = 6000, 4000
assert_("iiif region: full, square, pixels and percentages as the Image API's examples (§4.1)",
        iiif.region("full", W, H) == (0, 0, 6000, 4000) and iiif.region("square", W, H) == (1000, 0, 4000, 4000)
        and iiif.region("125,15,120,140", W, H) == (125, 15, 120, 140) and iiif.region("pct:41.6,7.5,40,70", W, H) == (2496, 300, 2400, 2800)
        and iiif.region("125,15,200,200", 300, 200) == (125, 15, 175, 185))
assert_("iiif region: one outside the image, or with no width, refused",
        refused(lambda: iiif.region("7000,0,10,10", W, H)) and refused(lambda: iiif.region("0,0,0,10", W, H)) and refused(lambda: iiif.region("a,b,c,d", W, H)))
assert_("iiif size: max, w,, ,h, pct:n, w,h and !w,h as the Image API's examples (§4.2)",
        iiif.size("max", 3000, 2000) == (3000, 2000) and iiif.size("150,", 3000, 2000) == (150, 100) and iiif.size(",150", 3000, 2000) == (225, 150)
        and iiif.size("pct:50", 3000, 2000) == (1500, 1000) and iiif.size("225,100", 3000, 2000) == (225, 100) and iiif.size("!225,100", 3000, 2000) == (150, 100))
assert_("iiif size: larger than the region only with ^, and max within the server's area",
        refused(lambda: iiif.size("4000,", 3000, 2000)) and iiif.size("^4000,", 3000, 2000) == (4000, 2667)
        and iiif.size("max", 3000, 2000, max_area=600_000) == (949, 632) and refused(lambda: iiif.size("pct:150", 30, 20)))
assert_("iiif rotation: mirrored and by 90°s served, other angles refused with 501",
        iiif.rotation("!90") == (True, 90) and iiif.rotation("360") == (False, 0) and refused(lambda: iiif.rotation("22.5"), 501) and refused(lambda: iiif.rotation("400")))
info = iiif.info("https://example.org/iiif/plan", W, H)
f = info["tiles"][0]["scaleFactors"][-1]
assert_("iiif info: tiles of 512 whose largest scale factor brings the whole image into one tile, sizes smallest first",
        info["type"] == "ImageService3" and info["profile"] == "level2" and f == 16 and -(-W // f) <= 512 and -(-H // f) <= 512
        and info["sizes"][0]["width"] < info["sizes"][-1]["width"] == W)
assert_("iiif parse: a request's path read, a quality or format not served refused",
        iiif.parse("0,0,512,512/256,/0/default.jpg")["size"] == "256," and refused(lambda: iiif.parse("full/max/0/sepia.jpg")) and refused(lambda: iiif.parse("full/max/0/default.bmp")))

try:
    from PIL import Image
except ImportError:
    Image = None
if Image is None:
    skip("iiif render: Pillow is not installed")
else:
    img = Image.new("RGB", (64, 32))
    img.putdata([(x * 4, y * 8, 100) for y in range(32) for x in range(64)])
    tile = Image.open(io.BytesIO(iiif.render(img, iiif.parse("8,4,16,8/max/0/default.png"))))
    ok = tile.size == (16, 8) and all(tile.getpixel((i, j)) == ((8 + i) * 4, (4 + j) * 8, 100) for i in range(16) for j in range(8))
    assert_("iiif render: a region's pixels exactly those of the image there", ok)
    m = Image.open(io.BytesIO(iiif.render(img, iiif.parse("0,0,16,8/max/!0/gray.png"))))
    assert_("iiif render: mirrored and gray", m.mode == "L" and m.size == (16, 8) and m.getpixel((0, 0)) == round(0.299 * 60 + 0.587 * 0 + 0.114 * 100))
    r = Image.open(io.BytesIO(iiif.render(img, iiif.parse("full/32,/90/default.png"))))
    assert_("iiif render: scaled and turned by 90°, width and height exchanged", r.size == (16, 32))
    status, kind, body = iiif.serve(lambda: img, "https://example.org/iiif/x", "info.json")
    status2, _, _ = iiif.serve(lambda: img, "https://example.org/iiif/x", "full/max/45/default.jpg")
    assert_("iiif serve: the information document, and a rotation not served answered 501", status == 200 and json.loads(body)["width"] == 64 and status2 == 501)

# ---- Presentation API ------------------------------------------------------------------------------------------------------------
man = iiif.manifest("https://example.org/iiif/p/manifest.json", "Papers of the reading room", [
    {"label": "Plan", "kind": "Image", "url": "https://example.org/iiif/p/plan/full/max/0/default.jpg", "format": "image/jpeg", "width": 6000, "height": 4000,
     "service": "https://example.org/iiif/p/plan", "annotations": [{"motivation": "commenting", "body": "The map cases.", "target": "xywh=10,20,300,200"}]},
    {"label": "A sounding", "kind": "Sound", "url": "https://example.org/files/sound.wav", "format": "audio/wav", "duration": 4.5,
     "annotations": [{"motivation": "commenting", "body": "The second tone.", "target": "t=1.3,2.5"}]}],
    metadata=[("Creator", "L. Brandt")], summary="An invented archive.", rights="http://creativecommons.org/publicdomain/zero/1.0/")
back = iiif.read_manifest(json.loads(json.dumps(man)))
c0, c1 = back["canvases"]
assert_("iiif manifest: written as Presentation 3.0 and read back canvas for canvas",
        man["@context"] == iiif.PRESENTATION_CONTEXT and back["version"] == 3 and back["label"] == "Papers of the reading room"
        and c0["width"] == 6000 and c0["service"] == "https://example.org/iiif/p/plan" and c0["kind"] == "Image" and c1["kind"] == "Sound" and c1["duration"] == 4.5)
assert_("iiif manifest: annotations kept with their regions and spans",
        c0["annotations"] == [{"motivation": "commenting", "body": "The map cases.", "target": "xywh=10,20,300,200"}] and c1["annotations"][0]["target"] == "t=1.3,2.5"
        and back["metadata"] == [("Creator", "L. Brandt")])
v2 = {"@context": "http://iiif.io/api/presentation/2/context.json", "@id": "https://example.org/m2", "@type": "sc:Manifest", "label": "An older manifest",
      "sequences": [{"canvases": [{"@id": "https://example.org/c1", "label": "folio 1r", "width": 2000, "height": 3000,
                                   "images": [{"resource": {"@id": "https://example.org/i/1/full/full/0/default.jpg", "format": "image/jpeg",
                                                            "service": {"@id": "https://example.org/i/1", "profile": "http://iiif.io/api/image/2/level1.json"}}}]}]}]}
r2 = iiif.read_manifest(v2)
assert_("iiif manifest: a version 2 manifest read, its image fetched by its service's path",
        r2["version"] == 2 and r2["canvases"][0]["label"] == "folio 1r" and iiif.image_url(r2["canvases"][0]) == "https://example.org/i/1/full/full/0/default.jpg"
        and iiif.image_url(c0, 800) == "https://example.org/iiif/p/plan/full/800,/0/default.jpg")
assert_("iiif manifest: what is not a manifest refused", refused(lambda: iiif.read_manifest({"type": "Collection"})))

# ---- EAD ------------------------------------------------------------------------------------------------------------------------
UNITS = [
    {"id": "F", "parent": "", "level": "fonds", "code": "RR", "title": "Papers of the reading room", "creator": "Reading-room committee", "creator_kind": "body", "from": "1928", "to": "1934", "order": 0, "holdings": []},
    {"id": "S1", "parent": "F", "level": "series", "code": "1", "title": "Minutes", "creator": "", "from": "1930", "to": "1931", "order": 0, "holdings": []},
    {"id": "S2", "parent": "F", "level": "series", "code": "2", "title": "Plans and sounds", "creator": "", "from": "", "to": "", "order": 1, "holdings": []},
    {"id": "f1", "parent": "S1", "level": "file", "code": "1", "title": "Minutes of 1931", "creator": "", "from": "1931", "to": "1931", "order": 0,
     "holdings": [{"name": "Minutes.txt", "title": "Minutes of the committee", "href": "holdings/Minutes.txt", "sha256": "a" * 64}]},
    {"id": "f2", "parent": "S2", "level": "file", "code": "1", "title": "The plan", "creator": "L. Brandt", "creator_kind": "person", "from": "1929", "to": "1929", "order": 0,
     "holdings": [{"name": "plan.svg", "title": "Plan of the reading room", "href": "holdings/plan.svg", "sha256": "b" * 64}]},
    {"id": "f3", "parent": "S2", "level": "piece", "code": "2", "title": "A sounding", "creator": "", "from": "", "to": "", "order": 1, "holdings": []},
]
xml = ead.write(UNITS, "Papers of the reading room: a finding aid", agency="The Archive Studio", date="2026-11-15")
read = ead.read(xml)
by = {u["id"]: u for u in read["units"]}
fields = ("parent", "level", "code", "title", "creator", "from", "to", "order")
same = all(all(by[u["id"]][k] == u[k] for k in fields) for u in UNITS)
assert_("ead: an arrangement written as EAD3 and read back unit for unit (levels, codes, titles, creators, dates, original order)", read["version"] == "EAD3" and same and len(read["units"]) == 6,
        json.dumps([(u["id"], {k: by.get(u["id"], {}).get(k) for k in fields}) for u in UNITS if any(by.get(u["id"], {}).get(k) != u[k] for k in fields)])[:400])
assert_("ead: holdings as digital objects with their digests; a level of the archive's own written as otherlevel",
        by["f2"]["holdings"] == [{"name": "plan.svg", "title": "Plan of the reading room", "href": "holdings/plan.svg", "sha256": "b" * 64}]
        and 'level="otherlevel" otherlevel="piece"' in xml and "<unitid>RR/2/1</unitid>" in xml)
EAD2002 = """<?xml version="1.0"?>
<ead xmlns="urn:isbn:1-931666-22-9"><eadheader><eadid>ex-1</eadid><filedesc><titlestmt><titleproper>Old papers</titleproper></titlestmt></filedesc></eadheader>
<archdesc level="collection"><did><unitid>OP</unitid><unittitle>Old papers</unittitle><unitdate normal="1801/1850">1801-1850</unitdate>
<origination><persname>A. Writer</persname></origination></did>
<dsc><c01 level="series"><did><unitid>OP/1</unitid><unittitle>Letters</unittitle><unitdate>1820-1830</unitdate></did>
<c02 level="item"><did><unitid>OP/1/1</unitid><unittitle>A letter</unittitle><unitdate normal="1825">1825</unitdate></did></c02></c01>
<c01 level="series"><did><unitid>OP/2</unitid><unittitle>Accounts</unittitle></did></c01></dsc></archdesc></ead>"""
r = ead.read(EAD2002)
u = r["units"]
assert_("ead: an EAD 2002 finding aid read: nested components, dates from normal or as written, reference codes to their own parts",
        r["version"] == "EAD 2002" and r["title"] == "Old papers" and len(u) == 4 and u[0]["creator"] == "A. Writer" and (u[0]["from"], u[0]["to"]) == ("1801", "1850")
        and u[1]["parent"] == u[0]["id"] and (u[1]["from"], u[1]["to"]) == ("1820", "1830") and u[2]["parent"] == u[1]["id"] and u[2]["code"] == "1" and u[3]["order"] == 1)
