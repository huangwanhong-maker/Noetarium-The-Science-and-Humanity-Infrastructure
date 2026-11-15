"""EAD: finding aids, the arrangement of an archive and its description level by level, written and read.

Encoded Archival Description is how archives exchange finding aids (EAD3, the Society of American Archivists, 2015, namespace
``http://ead3.archivists.org/schema/``; EAD 2002, namespace ``urn:isbn:1-931666-22-9``). A finding aid's ``archdesc`` describes
the fonds; its ``dsc`` holds the components ``c`` (or ``c01`` … ``c12``) nested as the arrangement is: series within the fonds,
files within series, items within files. Each component's ``did`` gives its reference code (``unitid``), its title
(``unittitle``), its dates (``unitdatestructured`` in EAD3, ``unitdate`` with ``normal`` in EAD 2002) and its creator
(``origination``); digital objects are ``dao``.

The arrangement is read into, and written from, units of the form of archeion's ``archeion::arrangement::Unit`` (ISAD(G)'s
levels), each with the holdings it contains:

    {"id", "parent", "level", "code", "title", "creator", "from", "to", "order",
     "holdings": [{"name", "title", "href", "sha256"}]}

- ``write(units, title, agency, record_id, date)`` writes EAD3, each ``unitid`` the full reference code (ISAD(G) 3.1.1). A level
  that is not one of EAD's is written ``otherlevel`` with its name. A holding is a ``dao`` whose ``href`` is where it is kept, its digest in a ``descriptivenote``.
- ``read(xml)`` reads EAD3 or EAD 2002: the ``archdesc`` as the fonds, every component beneath it with its parent and its place
  among its siblings (the original order); dates from ``unitdatestructured``, from ``unitdate``'s ``normal`` (YYYY/YYYY) or as
  written; reference codes as the record gives them, each component's own part where the code repeats its parent's.

What a record does not say is left empty, never guessed. Standard library only; the XML is parsed with ``xml.etree.ElementTree``,
which does not fetch external entities, and nothing in it is executed.
"""

from __future__ import annotations

import re
import xml.etree.ElementTree as ET

EAD3 = "http://ead3.archivists.org/schema/"
EAD2002 = "urn:isbn:1-931666-22-9"
XLINK = "http://www.w3.org/1999/xlink"
LEVELS = ("class", "collection", "file", "fonds", "item", "otherlevel", "recordgrp", "series", "subfonds", "subgrp", "subseries")


def _sub(parent: ET.Element, tag: str, text: str = "", **attrs) -> ET.Element:
    e = ET.SubElement(parent, f"{{{EAD3}}}{tag}", {k: str(v) for k, v in attrs.items() if v not in (None, "")})
    if text:
        e.text = text
    return e


def _level(e: ET.Element, level: str) -> None:
    if level in LEVELS:
        e.set("level", level)
    elif level:
        e.set("level", "otherlevel")
        e.set("otherlevel", level)


def _did(parent: ET.Element, unit: dict, code: str) -> None:
    did = _sub(parent, "did")
    if code:
        _sub(did, "unitid", code)
    _sub(did, "unittitle", unit.get("title") or unit.get("id", ""))
    a, b = unit.get("from", ""), unit.get("to", "")
    if a or b:
        uds = _sub(did, "unitdatestructured")
        if a and b and a != b:
            rng = _sub(uds, "daterange")
            _sub(rng, "fromdate", a, standarddate=a)
            _sub(rng, "todate", b, standarddate=b)
        else:
            _sub(uds, "datesingle", a or b, standarddate=a or b)
    if unit.get("creator"):
        orig = _sub(did, "origination")
        name = _sub(orig, "persname" if unit.get("creator_kind", "person") == "person" else "corpname")
        _sub(name, "part", unit["creator"])
    for h in unit.get("holdings", []) or []:
        dao = _sub(did, "dao", daotype="borndigital" if h.get("born_digital") else "derived", href=h.get("href", h.get("name", "")),
                   linktitle=h.get("title") or h.get("name", ""))
        if h.get("sha256"):
            note = _sub(dao, "descriptivenote")
            _sub(note, "p", f"SHA-256 {h['sha256']}")


def write(units: list[dict], title: str, agency: str = "", record_id: str = "", date: str = "") -> str:
    """EAD3 for an arrangement: the unit without a parent (the first, if several) as the archdesc, the rest as nested components."""
    ET.register_namespace("", EAD3)
    roots = [u for u in units if not u.get("parent")]
    if not roots:
        raise ValueError("an arrangement has a unit with no parent: its fonds")
    root = roots[0]
    children: dict[str, list[dict]] = {}
    for u in units:
        if u.get("parent"):
            children.setdefault(u["parent"], []).append(u)
    for k in children:
        children[k].sort(key=lambda u: (u.get("order", 0), u.get("id", "")))
    ead = ET.Element(f"{{{EAD3}}}ead")
    control = _sub(ead, "control")
    _sub(control, "recordid", record_id or root.get("id", "finding-aid"))
    fd = _sub(control, "filedesc")
    ts = _sub(fd, "titlestmt")
    _sub(ts, "titleproper", title)
    _sub(control, "maintenancestatus", value="new")
    ma = _sub(control, "maintenanceagency")
    _sub(ma, "agencyname", agency or "unstated")
    mh = _sub(control, "maintenancehistory")
    me = _sub(mh, "maintenanceevent")
    _sub(me, "eventtype", value="created")
    _sub(me, "eventdatetime", date or "", standarddatetime=date or None)
    _sub(me, "agenttype", value="machine")
    _sub(me, "agent", "Noetarium")
    arch = _sub(ead, "archdesc", id=re.sub(r"[^A-Za-z0-9_.-]", "_", "u-" + root.get("id", "")))
    _level(arch, root.get("level", "fonds"))
    _did(arch, root, root.get("code", ""))

    # A unitid is the full reference code (ISAD(G) 3.1.1): the codes from the fonds down, joined by '/', as archeion joins them.
    def component(parent: ET.Element, unit: dict, above: str) -> None:
        c = _sub(parent, "c", id=re.sub(r"[^A-Za-z0-9_.-]", "_", "u-" + unit.get("id", "")))
        _level(c, unit.get("level", ""))
        ref = "/".join(x for x in (above, unit.get("code", "")) if x)
        _did(c, unit, ref)
        for child in children.get(unit["id"], []):
            component(c, child, ref)

    kids = children.get(root["id"], [])
    if kids:
        dsc = _sub(arch, "dsc")
        for u in kids:
            component(dsc, u, root.get("code", ""))
    ET.indent(ead, space="  ")
    return '<?xml version="1.0" encoding="UTF-8"?>\n' + ET.tostring(ead, encoding="unicode") + "\n"


def _ns(e: ET.Element) -> str:
    return e.tag[1:].split("}")[0] if e.tag.startswith("{") else ""


def _local(e: ET.Element) -> str:
    return e.tag.split("}", 1)[-1]


def _child(e: ET.Element | None, name: str) -> ET.Element | None:
    if e is None:
        return None
    for c in e:
        if _local(c) == name:
            return c
    return None


def _children(e: ET.Element | None, name: str) -> list[ET.Element]:
    return [c for c in e if _local(c) == name] if e is not None else []


def _text(e: ET.Element | None) -> str:
    return " ".join("".join(e.itertext()).split()) if e is not None else ""


def _dates(did: ET.Element) -> tuple[str, str]:
    uds = _child(did, "unitdatestructured")
    if uds is not None:
        rng = _child(uds, "daterange")
        if rng is not None:
            f, t = _child(rng, "fromdate"), _child(rng, "todate")
            return (f.get("standarddate", _text(f)) if f is not None else "", t.get("standarddate", _text(t)) if t is not None else "")
        single = _child(uds, "datesingle")
        if single is not None:
            d = single.get("standarddate", _text(single))
            return d, d
    ud = _child(did, "unitdate")
    if ud is not None:
        normal = ud.get("normal", "")
        if normal:
            a, _, b = normal.partition("/")
            return a, b or a
        m = re.findall(r"\b(\d{4})\b", _text(ud))
        if m:
            return m[0], m[-1]
    return "", ""


def _creator(did: ET.Element) -> tuple[str, str]:
    orig = _child(did, "origination")
    if orig is None:
        return "", ""
    for kind in ("persname", "corpname", "famname", "name"):
        n = _child(orig, kind)
        if n is not None:
            return _text(n), "person" if kind == "persname" else "body"
    return _text(orig), ""


def _holdings(did: ET.Element) -> list[dict]:
    out = []
    for name in ("dao", "daogrp"):
        for dao in _children(did, name):
            href = dao.get("href") or dao.get(f"{{{XLINK}}}href") or ""
            title = dao.get("linktitle") or dao.get(f"{{{XLINK}}}title") or dao.get("title") or ""
            note = _text(_child(dao, "descriptivenote")) or _text(_child(dao, "daodesc"))
            m = re.search(r"\b([0-9a-f]{64})\b", note)
            out.append({"name": href.rsplit("/", 1)[-1], "title": title, "href": href, "sha256": m.group(1) if m else ""})
    return out


def read(xml_text: str | bytes) -> dict:
    """A finding aid read into {"version": "EAD3" | "EAD 2002", "title", "record_id", "units": [...]}."""
    root = ET.fromstring(xml_text)
    ns = _ns(root)
    if _local(root) != "ead" or ns not in (EAD3, EAD2002, ""):
        raise ValueError("this is not an EAD finding aid (EAD3 or EAD 2002)")
    version = "EAD3" if ns == EAD3 else "EAD 2002"
    head = _child(root, "control") if version == "EAD3" else _child(root, "eadheader")
    title = _text(root.find(f".//{{{ns}}}titleproper")) if ns else _text(root.find(".//titleproper"))
    record_id = _text(_child(head, "recordid") if version == "EAD3" else _child(head, "eadid"))
    arch = _child(root, "archdesc")
    if arch is None:
        raise ValueError("a finding aid without an archdesc")
    units: list[dict] = []
    ids: set[str] = set()

    def unit(e: ET.Element, parent: dict | None, order: int) -> dict:
        did = _child(e, "did")
        level = e.get("level", "")
        if level == "otherlevel":
            level = e.get("otherlevel", "otherlevel")
        code = _text(_child(did, "unitid"))
        if parent and parent.get("reference") and code.startswith(parent["reference"]) and len(code) > len(parent["reference"]):
            own = code[len(parent["reference"]):].lstrip("/-. ")
        else:
            own = code
        uid = e.get("id") or code or f"u{len(units) + 1}"
        if uid.startswith("u-"):
            uid = uid[2:]
        while uid in ids:
            uid += "'"
        ids.add(uid)
        a, b = _dates(did) if did is not None else ("", "")
        creator, kind = _creator(did) if did is not None else ("", "")
        u = {"id": uid, "parent": parent["id"] if parent else "", "level": level, "code": own, "title": _text(_child(did, "unittitle")),
             "creator": creator, "creator_kind": kind, "from": a, "to": b, "order": order, "holdings": _holdings(did) if did is not None else [],
             "reference": code}
        units.append(u)
        return u

    fonds = unit(arch, None, 0)

    def walk(container: ET.Element, parent: dict) -> None:
        k = 0
        for c in container:
            if re.fullmatch(r"c(0[1-9]|1[0-2])?", _local(c)):
                u = unit(c, parent, k)
                k += 1
                walk(c, u)

    for dsc in _children(arch, "dsc"):
        walk(dsc, fonds)
    for u in units:
        u.pop("reference", None)
    return {"version": version, "title": title, "record_id": record_id, "units": units}
