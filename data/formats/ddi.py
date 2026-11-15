"""DDI-Codebook 2.5: the description of a study and its variables as data archives exchange it, read and written.

The Data Documentation Initiative's Codebook (DDI-C 2.5, namespace ``ddi:codebook:2_5``) is how the social-science data archives
describe a study: its citation (titles, identifiers, authors, producers, distributors, dates, version), what was studied and how
(collection dates, nations, universe, unit of analysis, sampling, mode, time method), access, and in ``dataDscr`` each variable
with its label, question, universe, categories (with ``missing="Y"``) and valid range. This module reads such XML into the
infrastructure's codebook, a plain dictionary of the same form as archeion's ``archeion::datasets::Codebook``, and writes a
codebook back as DDI-C 2.5:

    {"study": {"id", "title", "producer", "version", "released", "doi", "citation", "universe", "sampling", "mode",
               "from", "until", "countries": [...], "access", "licence",
               "authors": [...], "distributors": [...], "keywords": [...], "topics": [...], "unit", "time_method",
               "identifiers": {agency: value}, "holdings": url},
     "variables": [{"name", "label", "question", "universe", "unit", "derivation", "notes", "measure",
                    "categories": [{"code", "label", "missing"}], "least", "most", "weight"}],
     "files": [{"name", "digest", "variables": [...], "cases"}]}

What a record does not say is left empty, never guessed. Text in several languages is read in the language asked for, else in
English, else the first given. Dates are kept as the record gives them (ISO 8601, possibly a year or a month). A measure is read
from ``nature``, ``intrvl`` and ``varFormat`` and written back so: nominal and ordinal as discrete with their nature; discrete
and continuous numbers by ``intrvl``; text and dates as character formats.

Standard library only. The XML comes from archives and catalogues: it is parsed with ``xml.etree.ElementTree``, which does not
fetch external entities; nothing in it is executed.
"""

from __future__ import annotations

import xml.etree.ElementTree as ET

NS = "ddi:codebook:2_5"
XML_LANG = "{http://www.w3.org/XML/1998/namespace}lang"
MEASURES = ("nominal", "ordinal", "discrete", "continuous", "text", "date")


def _q(tag: str) -> str:
    return f"{{{NS}}}{tag}"


def _text(e: ET.Element | None) -> str:
    """The element's text with its children's, whitespace collapsed."""
    if e is None:
        return ""
    return " ".join("".join(e.itertext()).split())


def _pick(elements: list[ET.Element], lang: str) -> ET.Element | None:
    """The element in the language asked for, else English, else the first."""
    if not elements:
        return None
    for want in (lang, "en"):
        for e in elements:
            if e.get(XML_LANG, "").split("-")[0] == want:
                return e
    return elements[0]


def _find(parent: ET.Element | None, path: str, lang: str) -> ET.Element | None:
    if parent is None:
        return None
    return _pick(parent.findall(path, {"d": NS}), lang)


def _all(parent: ET.Element | None, path: str, lang: str) -> list[str]:
    """Every distinct text at the path, in the language asked for where the record gives several."""
    if parent is None:
        return []
    found = parent.findall(path, {"d": NS})
    langs = {e.get(XML_LANG, "").split("-")[0] for e in found}
    keep = lang if lang in langs else ("en" if "en" in langs else None)
    out: list[str] = []
    for e in found:
        if keep is not None and e.get(XML_LANG, "").split("-")[0] not in (keep, ""):
            continue
        t = _text(e)
        if t and t not in out:
            out.append(t)
    return out


def _measure(var: ET.Element) -> str:
    nature = var.get("nature", "").lower()
    intrvl = var.get("intrvl", "").lower()
    fmt = var.find("d:varFormat", {"d": NS})
    if nature in ("nominal", "ordinal"):
        return nature
    if fmt is not None and fmt.get("type", "").lower() == "character":
        return "date" if "date" in (fmt.get("formatname", "") + fmt.get("category", "")).lower() else "text"
    if intrvl == "contin":
        return "continuous"
    if var.findall("d:catgry", {"d": NS}) and not var.findall("d:valrng", {"d": NS}) and nature not in ("interval", "ratio"):
        return "nominal"
    return "discrete"


def _number(s: str | None):
    try:
        x = float(s)
    except (TypeError, ValueError):
        return None
    return x if x == x and x not in (float("inf"), float("-inf")) else None


def read_codebook(code_book: ET.Element, lang: str = "en") -> dict:
    """A codebook from a ``codeBook`` element."""
    sd = code_book.find("d:stdyDscr", {"d": NS})
    cit = sd.find("d:citation", {"d": NS}) if sd is not None else None
    info = sd.find("d:stdyInfo", {"d": NS}) if sd is not None else None
    summ = info.find("d:sumDscr", {"d": NS}) if info is not None else None
    coll = sd.find("d:method/d:dataColl", {"d": NS}) if sd is not None else None
    ids: dict[str, str] = {}
    for e in (cit.findall("d:titlStmt/d:IDNo", {"d": NS}) if cit is not None else []):
        agency = e.get("agency", "") or "id"
        ids.setdefault(agency, _text(e))
    doi = next((v for k, v in ids.items() if k.upper() == "DOI"), "")
    if doi.lower().startswith("doi:"):
        doi = doi[4:]
    dates = {e.get("event", "single"): (e.get("date") or _text(e)) for e in (summ.findall("d:collDate", {"d": NS}) if summ is not None else [])}
    if not dates and summ is not None:
        dates = {e.get("event", "single"): (e.get("date") or _text(e)) for e in summ.findall("d:timePrd", {"d": NS})}
    released = _find(cit, "d:distStmt/d:distDate", lang)
    produced = _find(cit, "d:prodStmt/d:prodDate", lang)
    version = _find(cit, "d:verStmt/d:version", lang)
    holdings = _find(cit, "d:holdings", lang)
    nations = []
    for e in (summ.findall("d:nation", {"d": NS}) if summ is not None else []):
        n = e.get("abbr") or _text(e)
        if n and n not in nations:
            nations.append(n)
    study = {
        "id": ids.get("id") or next(iter(ids.values()), ""),
        "title": _text(_find(cit, "d:titlStmt/d:titl", lang)),
        "producer": "; ".join(_all(cit, "d:prodStmt/d:producer", lang)),
        "version": (_text(version) or (version.get("date", "") if version is not None else "")),
        "released": (released.get("date", "") if released is not None else "") or (produced.get("date", "") if produced is not None else ""),
        "doi": doi,
        "citation": _text(_find(cit, "d:biblCit", lang)),
        "universe": _text(_find(summ, "d:universe", lang)),
        "sampling": _text(_find(coll, "d:sampProc", lang)),
        "mode": "; ".join(_all(coll, "d:collMode", lang)),
        "from": dates.get("start", dates.get("single", "")),
        "until": dates.get("end", dates.get("single", "")),
        "countries": nations,
        "access": "; ".join(_all(sd, "d:dataAccs/d:useStmt/d:restrctn", lang)) if sd is not None else "",
        "licence": "; ".join(_all(sd, "d:dataAccs/d:useStmt/d:conditions", lang)) if sd is not None else "",
        "authors": _all(cit, "d:rspStmt/d:AuthEnty", lang),
        "distributors": _all(cit, "d:distStmt/d:distrbtr", lang),
        "keywords": _all(info, "d:subject/d:keyword", lang),
        "topics": _all(info, "d:subject/d:topcClas", lang),
        "unit": "; ".join(_all(summ, "d:anlyUnit", lang)),
        "time_method": "; ".join(_all(coll, "d:timeMeth", lang)),
        "identifiers": ids,
        "holdings": holdings.get("URI", "") if holdings is not None else "",
    }
    files = []
    file_ids = {}
    for f in code_book.findall("d:fileDscr", {"d": NS}):
        name = _text(f.find("d:fileTxt/d:fileName", {"d": NS}))
        if not name:
            continue
        cases = _number(_text(f.find("d:fileTxt/d:dimensns/d:caseQnty", {"d": NS})))
        files.append({"name": name, "digest": "", "variables": [], "cases": int(cases) if cases else 0})
        if f.get("ID"):
            file_ids[f.get("ID")] = files[-1]
    variables = []
    for v in code_book.findall("d:dataDscr/d:var", {"d": NS}):
        cats = []
        for c in v.findall("d:catgry", {"d": NS}):
            code = _text(c.find("d:catValu", {"d": NS}))
            if code:
                cats.append({"code": code, "label": _text(_find(c, "d:labl", lang)), "missing": c.get("missing", "").upper() == "Y"})
        rng = v.find("d:valrng/d:range", {"d": NS})
        var = {
            "name": v.get("name", ""),
            "label": _text(_find(v, "d:labl", lang)),
            "question": _text(_find(v, "d:qstn/d:qstnLit", lang)),
            "universe": _text(_find(v, "d:universe", lang)),
            "unit": "",
            "derivation": _text(_find(v, "d:derivation/d:drvdesc", lang)),
            "notes": _text(_find(v, "d:notes", lang)),
            "measure": _measure(v),
            "categories": cats,
            "least": _number(rng.get("min")) if rng is not None else None,
            "most": _number(rng.get("max")) if rng is not None else None,
            "weight": v.get("wgt", "") == "wgt",
        }
        variables.append(var)
        for fid in (v.get("files", "") or "").split():
            if fid in file_ids:
                file_ids[fid]["variables"].append(var["name"])
    return {"study": study, "variables": variables, "files": files}


def read(xml_text: str | bytes, lang: str = "en") -> list[dict]:
    """Every codebook in a document: a ``codeBook`` itself, or one inside an OAI-PMH response."""
    root = ET.fromstring(xml_text)
    if root.tag == _q("codeBook"):
        return [read_codebook(root, lang)]
    return [read_codebook(cb, lang) for cb in root.iter(_q("codeBook"))]


def _plain(x) -> str:
    """A number as text: whole numbers without a point, others as Python writes them exactly."""
    x = float(x)
    return str(int(x)) if x == int(x) else repr(x)


def _sub(parent: ET.Element, tag: str, text: str = "", **attrs) -> ET.Element:
    e = ET.SubElement(parent, _q(tag), {k.rstrip("_"): v for k, v in attrs.items() if v not in ("", None)})
    if text:
        e.text = text
    return e


def write(codebook: dict, lang: str = "en") -> str:
    """A codebook as DDI-C 2.5 XML."""
    ET.register_namespace("", NS)
    s = codebook.get("study", {})
    cb = ET.Element(_q("codeBook"), {"version": "2.5"})
    sd = _sub(cb, "stdyDscr")
    cit = _sub(sd, "citation")
    ts = _sub(cit, "titlStmt")
    _sub(ts, "titl", s.get("title", ""), **{XML_LANG: lang})
    if s.get("id"):
        _sub(ts, "IDNo", s["id"], agency="id")
    if s.get("doi"):
        _sub(ts, "IDNo", s["doi"], agency="DOI")
    if s.get("authors"):
        rs = _sub(cit, "rspStmt")
        for a in s["authors"]:
            _sub(rs, "AuthEnty", a)
    ps = _sub(cit, "prodStmt")
    if s.get("producer"):
        _sub(ps, "producer", s["producer"])
    ds = _sub(cit, "distStmt")
    for d in s.get("distributors", []):
        _sub(ds, "distrbtr", d)
    if s.get("released"):
        _sub(ds, "distDate", date=s["released"])
    if s.get("version"):
        _sub(_sub(cit, "verStmt"), "version", s["version"])
    if s.get("citation"):
        _sub(cit, "biblCit", s["citation"])
    if s.get("holdings"):
        _sub(cit, "holdings", URI=s["holdings"])
    info = _sub(sd, "stdyInfo")
    if s.get("keywords") or s.get("topics"):
        subj = _sub(info, "subject")
        for k in s.get("keywords", []):
            _sub(subj, "keyword", k)
        for t in s.get("topics", []):
            _sub(subj, "topcClas", t)
    summ = _sub(info, "sumDscr")
    if s.get("from"):
        _sub(summ, "collDate", s["from"], date=s["from"], event="start")
    if s.get("until"):
        _sub(summ, "collDate", s["until"], date=s["until"], event="end")
    for n in s.get("countries", []):
        _sub(summ, "nation", n, abbr=n if len(n) <= 3 else "")
    if s.get("unit"):
        _sub(summ, "anlyUnit", s["unit"])
    if s.get("universe"):
        _sub(summ, "universe", s["universe"])
    coll = _sub(_sub(sd, "method"), "dataColl")
    if s.get("time_method"):
        _sub(coll, "timeMeth", s["time_method"])
    if s.get("sampling"):
        _sub(coll, "sampProc", s["sampling"])
    if s.get("mode"):
        _sub(coll, "collMode", s["mode"])
    if s.get("access") or s.get("licence"):
        us = _sub(_sub(sd, "dataAccs"), "useStmt")
        if s.get("access"):
            _sub(us, "restrctn", s["access"])
        if s.get("licence"):
            _sub(us, "conditions", s["licence"])
    names: dict[str, str] = {}
    for i, f in enumerate(codebook.get("files", [])):
        fid = f"F{i + 1}"
        fd = _sub(cb, "fileDscr", ID=fid)
        ft = _sub(fd, "fileTxt")
        _sub(ft, "fileName", f.get("name", ""))
        dim = _sub(ft, "dimensns")
        if f.get("cases"):
            _sub(dim, "caseQnty", str(f["cases"]))
        _sub(dim, "varQnty", str(len(f.get("variables", []))))
        for v in f.get("variables", []):
            names[v] = (names.get(v, "") + " " + fid).strip()
    if codebook.get("variables"):
        dd = _sub(cb, "dataDscr")
        for v in codebook["variables"]:
            m = v.get("measure", "nominal")
            if m not in MEASURES:
                raise ValueError(f"ddi.write: the variable {v.get('name')} has a measure, {m}, that is none of {', '.join(MEASURES)}")
            attrs = {"name": v["name"], "ID": "V_" + v["name"], "files": names.get(v["name"], "")}
            if m in ("nominal", "ordinal"):
                attrs.update(intrvl="discrete", nature=m)
            elif m in ("discrete", "continuous"):
                attrs["intrvl"] = "discrete" if m == "discrete" else "contin"
            if v.get("weight"):
                attrs["wgt"] = "wgt"
            var = _sub(dd, "var", **attrs)
            if v.get("label"):
                _sub(var, "labl", v["label"])
            if v.get("question"):
                _sub(_sub(var, "qstn"), "qstnLit", v["question"])
            if v.get("universe"):
                _sub(var, "universe", v["universe"])
            if v.get("least") is not None or v.get("most") is not None:
                rng = _sub(_sub(var, "valrng"), "range")
                if v.get("least") is not None:
                    rng.set("min", _plain(v["least"]))
                if v.get("most") is not None:
                    rng.set("max", _plain(v["most"]))
            for c in v.get("categories", []):
                cat = _sub(var, "catgry", missing="Y" if c.get("missing") else "")
                _sub(cat, "catValu", c["code"])
                if c.get("label"):
                    _sub(cat, "labl", c["label"])
            if v.get("derivation"):
                _sub(_sub(var, "derivation"), "drvdesc", v["derivation"])
            if m in ("text", "date"):
                _sub(var, "varFormat", type="character", formatname="ISO 8601 date" if m == "date" else "")
            elif m in ("discrete", "continuous"):
                _sub(var, "varFormat", type="numeric")
            if v.get("notes"):
                _sub(var, "notes", v["notes"])
    ET.indent(cb, space="  ")
    return '<?xml version="1.0" encoding="UTF-8"?>\n' + ET.tostring(cb, encoding="unicode")
