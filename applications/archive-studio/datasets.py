"""The Archive Studio's datasets (ADR 0120): a table among the holdings and the codebook that describes it, on archeion's datasets.

A codebook is kept in the project as ``codebooks/<name>.json``, in the form of archeion's ``archeion::datasets::Codebook`` (the study, its
variables with their measures, categories and missing codes, and its files with their digests), which ``data/formats/ddi.py`` reads
from DDI-Codebook 2.5 and writes back. The codebooks are also described as linked data, in ``linked/codebooks.ttl``, in DISCO (the
DDI Alliance's Discovery vocabulary, 2013): each a ``disco:LogicalDataSet`` with its ``disco:variable``s, each variable with its
label, question and the scheme of its codes (``skos:Concept``s), so that the graph, queries and shapes reach them. Every check is
archeion's: a codebook well formed, a file held to it cell by cell, two versions compared, a harmonisation assessed, and the
finest common target of two variables.

    GET  /api/projects/<p>/codebooks                  the codebooks
    GET  /api/projects/<p>/codebooks/<name>           one, with archeion's faults
    GET  /api/projects/<p>/codebooks/<name>.xml       it in DDI-Codebook 2.5
    PUT  /api/projects/<p>/codebooks/<name>           {"codebook"}: kept, described in DISCO (a contributor)
    POST /api/projects/<p>/codebooks/<name>/ddi       a DDI-Codebook 2.5 file read into it
    POST /api/projects/<p>/codebooks/draft            {"holding", "name"?}: a draft codebook from a table among the holdings
    POST /api/projects/<p>/codebooks/<name>/conform   {"holding"}: the table held to its codebook, with each variable's frequencies
    POST /api/projects/<p>/codebooks/compare          {"earlier", "later"}: what changed between two versions
    POST /api/projects/<p>/codebooks/common           {"a": {codebook, variable}, "b": {...}, "corresponds": [[a, b]]}: their common target
"""

from __future__ import annotations

import csv
import io
import json
import re
import sys
import urllib.parse
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(ROOT / "api" / "python"))
sys.path.insert(0, str(ROOT / "data" / "formats"))

import ddi  # noqa: E402
from science import archive as archeion  # noqa: E402
from science import studio_host as host  # noqa: E402

NAME = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_.-]{0,63}$")


def _a():
    import archive as a
    return a


def folder(project: str) -> Path:
    return host.project_dir(project) / "codebooks"


def names(project: str) -> list[str]:
    return sorted(p.stem for p in folder(project).glob("*.json")) if folder(project).is_dir() else []


def read(project: str, name: str) -> dict:
    path = folder(project) / f"{check(name)}.json"
    if not path.is_file():
        raise KeyError(f"there is no codebook '{name}'")
    return json.loads(path.read_text(encoding="utf-8"))


def check(name: str) -> str:
    if not NAME.match(name or ""):
        raise ValueError(f"'{name}' is not a codebook's name: letters, digits, _ . -")
    return name


def listing(project: str) -> list[dict]:
    out = []
    for n in names(project):
        c = read(project, n)
        out.append({"name": n, "title": c.get("study", {}).get("title", ""), "variables": len(c.get("variables", [])), "files": [f.get("name") for f in c.get("files", [])],
                    "version": c.get("study", {}).get("version", "")})
    return out


def lit(text) -> str:
    return '"' + str(text).replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n").replace("\r", "") + '"'


def disco(project: str) -> str:
    """Every codebook of the project in DISCO."""
    b = _a().base(project)
    holdings = {h["name"]: h["iri"] for h in _a().holdings(project)}
    out = ["""@prefix disco: <http://rdf-vocabulary.ddialliance.org/discovery#> .
@prefix dcterms: <http://purl.org/dc/terms/> .
@prefix skos: <http://www.w3.org/2004/02/skos/core#> .
@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .
"""]
    for n in names(project):
        c = read(project, n)
        cb = f"{b}codebook:{urllib.parse.quote(n, safe='')}"
        s = c.get("study", {})
        vs = [f"{b}variable:{urllib.parse.quote(n, safe='')}/{urllib.parse.quote(v['name'], safe='')}" for v in c.get("variables", [])]
        head = [f"<{cb}> a disco:LogicalDataSet", f"    dcterms:title {lit(s.get('title') or n)}"]
        if s.get("version"):
            head.append(f"    dcterms:hasVersion {lit(s['version'])}")
        head += [f"    disco:variable <{v}>" for v in vs]
        out.append(" ;\n".join(head) + " .")
        for f in c.get("files", []):
            if f.get("name") in holdings:
                out.append(f"<{holdings[f['name']]}> dcterms:conformsTo <{cb}> .")
        for v, iri in zip(c.get("variables", []), vs):
            lines = [f"<{iri}> a disco:Variable", f"    skos:notation {lit(v['name'])}"]
            if v.get("label"):
                lines.append(f"    skos:prefLabel {lit(v['label'])}")
            if v.get("question"):
                lines.append(f"    disco:question [ a disco:Question ; disco:questionText {lit(v['question'])} ]")
            if v.get("categories"):
                scheme = f"{iri}/codes"
                lines.append(f"    disco:representation <{scheme}>")
                out.append(" ;\n".join(lines) + " .")
                out.append(f"<{scheme}> a skos:ConceptScheme ; skos:prefLabel {lit((v.get('label') or v['name']) + ': codes')} .")
                for k in v["categories"]:
                    out.append(f"<{scheme}/{urllib.parse.quote(str(k['code']), safe='')}> a skos:Concept ; skos:inScheme <{scheme}> ; skos:notation {lit(k['code'])} ; skos:prefLabel {lit(k.get('label', ''))}"
                               + (" ; skos:note \"missing\"" if k.get("missing") else "") + " .")
            else:
                out.append(" ;\n".join(lines) + " .")
    return "\n".join(out) + "\n"


def keep(project: str, name: str, codebook: dict, by: str, what: str = "kept a codebook") -> dict:
    check(name)
    if not isinstance(codebook, dict) or not isinstance(codebook.get("variables", []), list):
        raise ValueError("a codebook is {study, variables, files}")
    codebook.setdefault("study", {}), codebook.setdefault("variables", []), codebook.setdefault("files", [])
    r = archeion.ask("codebook", codebook=codebook)
    folder(project).mkdir(parents=True, exist_ok=True)
    host.replace_file(folder(project) / f"{name}.json", json.dumps(codebook, ensure_ascii=False, indent=1))
    answer = _a().keep_text(project, "codebooks.ttl", disco(project))
    host.record(project, by, what, name, f"{len(codebook['variables'])} variables")
    return {"name": name, "codebook": codebook, "faults": r["faults"], "verdict": r["verdict"], "described": "message" not in answer}


def table_of(project: str, holding: str) -> tuple[list[str], list[list[str]], str]:
    path = host.project_dir(project) / "holdings" / holding
    if not path.is_file():
        raise KeyError(f"there is no holding '{holding}'")
    raw = path.read_bytes()
    text = raw.decode("utf-8-sig", errors="replace")
    try:
        dialect = csv.Sniffer().sniff(text[:4096], delimiters=",;\t|")
    except csv.Error:
        dialect = csv.excel
    rows = list(csv.reader(io.StringIO(text), dialect))
    rows = [r for r in rows if any(c.strip() for c in r)]
    if not rows:
        raise ValueError(f"the holding '{holding}' holds no table")
    import hashlib
    return rows[0], rows[1:], hashlib.sha256(raw).hexdigest()


def number(s: str):
    try:
        return float(s.replace(",", "")) if s.strip() else None
    except ValueError:
        return None


def draft(project: str, holding: str, name: str, by: str) -> dict:
    """A draft codebook from a table: each column a variable whose measure and codes are guessed from its cells, and said to be."""
    columns, rows, digest = table_of(project, holding)
    variables = []
    for j, col in enumerate(columns):
        cells = [r[j].strip() for r in rows if j < len(r) and r[j].strip()]
        nums = [number(c) for c in cells]
        distinct = sorted(set(cells))
        numeric = cells and all(n is not None for n in nums)
        v = {"name": re.sub(r"[^A-Za-z0-9_]", "_", col.strip()) or f"v{j + 1}", "label": col.strip(), "question": "", "universe": "", "unit": "", "derivation": "",
             "notes": "drafted from the table: its measure and codes are a guess to be checked", "categories": [], "least": None, "most": None, "weight": False}
        if numeric and len(distinct) <= 10 and all(float(n).is_integer() for n in nums):
            v["measure"] = "nominal" if len(distinct) <= 3 else "ordinal"
            v["categories"] = [{"code": c, "label": c, "missing": False} for c in sorted(distinct, key=lambda x: float(x))]
        elif numeric:
            v["measure"] = "discrete" if all(float(n).is_integer() for n in nums) else "continuous"
            v["least"], v["most"] = min(nums), max(nums)
        elif re.fullmatch(r"\d{4}(-\d{2}(-\d{2})?)?", cells[0] if cells else "") and all(re.fullmatch(r"\d{4}(-\d{2}(-\d{2})?)?", c) for c in cells):
            v["measure"] = "date"
        elif len(distinct) <= 12:
            v["measure"] = "nominal"
            v["categories"] = [{"code": c, "label": c, "missing": False} for c in distinct]
        else:
            v["measure"] = "text"
        variables.append(v)
    title = Path(holding).stem
    codebook = {"study": {"id": name, "title": title, "version": "draft"}, "variables": variables,
                "files": [{"name": holding, "digest": digest, "variables": [v["name"] for v in variables], "cases": len(rows)}]}
    # The file's columns are named as written; a variable's name is a cleaned form, so the file is described by its columns' names.
    codebook["files"][0]["variables"] = [v["name"] for v in variables]
    return keep(project, check(name or re.sub(r"[^A-Za-z0-9_.-]", "-", title)[:60] or "codebook"), codebook, by, "drafted a codebook")


def conform(project: str, name: str, holding: str) -> dict:
    c = read(project, name)
    columns, rows, digest = table_of(project, holding)
    file = next((f for f in c.get("files", []) if f.get("name") == holding), None)
    if not file:
        raise ValueError(f"the codebook '{name}' declares no file '{holding}': add it to its files first")
    clean = [re.sub(r"[^A-Za-z0-9_]", "_", x.strip()) for x in columns]
    weight = next((v["name"] for v in c["variables"] if v.get("weight")), "")
    r = archeion.ask("conform", codebook=c, file=holding, table={"columns": clean, "rows": rows}, observed=digest, weight=weight)
    return r


def routes(handler, method: str, parts: list[str], query: dict, project: str, what: str, rest: list[str], exists) -> bool:
    if what != "codebooks":
        return False
    if method == "GET":
        if not rest:
            handler.answer(lambda: (exists(), ({"codebooks": listing(project)}, 200))[1])
            return True
        if len(rest) == 1 and rest[0].endswith(".xml"):
            try:
                exists()
                data = ddi.write(read(project, urllib.parse.unquote(rest[0])[:-4])).encode("utf-8")
            except (KeyError, ValueError) as e:
                handler.reply_json({"message": str(e)}, 404)
                return True
            handler.send_response(200)
            handler.send_header("Content-Type", "application/xml; charset=utf-8")
            handler.send_header("Content-Disposition", f'attachment; filename="{rest[0]}"')
            handler.send_header("Content-Length", str(len(data)))
            handler.end_headers()
            handler.wfile.write(data)
            return True
        if len(rest) == 1:
            def one():
                exists()
                c = read(project, urllib.parse.unquote(rest[0]))
                r = archeion.ask("codebook", codebook=c)
                return {"name": rest[0], "codebook": c, "faults": r["faults"], "verdict": r["verdict"]}, 200
            handler.answer(one)
            return True
        return False
    refusal = host.write_refusal(handler)
    if refusal:
        handler.reply_json({"message": refusal}, 403)
        return True
    by = host.who(handler)
    if len(rest) == 2 and rest[1] == "ddi" and method == "POST":
        raw = handler.body(64 * 1024 * 1024)
        if raw is None:
            return True

        def imported():
            exists()
            books = ddi.read(raw)
            if not books:
                raise ValueError("the file holds no DDI codebook")
            return keep(project, urllib.parse.unquote(rest[0]), books[0], by, "read a codebook from DDI"), 200
        handler.answer(imported)
        return True
    body = handler.json_body(16 * 1024 * 1024)
    if body is None:
        return True
    if method == "PUT" and len(rest) == 1:
        handler.answer(lambda: (exists(), (keep(project, urllib.parse.unquote(rest[0]), body.get("codebook"), by), 200))[1])
        return True
    if method == "POST" and rest == ["draft"]:
        handler.answer(lambda: (exists(), (draft(project, str(body.get("holding", "")), str(body.get("name", "")), by), 200))[1])
        return True
    if method == "POST" and len(rest) == 2 and rest[1] == "conform":
        handler.answer(lambda: (exists(), (conform(project, urllib.parse.unquote(rest[0]), str(body.get("holding", ""))), 200))[1])
        return True
    if method == "POST" and rest == ["compare"]:
        handler.answer(lambda: (exists(), (archeion.ask("versions", earlier=read(project, str(body.get("earlier"))), later=read(project, str(body.get("later")))), 200))[1])
        return True
    if method == "POST" and rest == ["common"]:
        def common():
            exists()
            a, b = body.get("a", {}), body.get("b", {})
            ca, cb = read(project, str(a.get("codebook"))), read(project, str(b.get("codebook")))
            ca["study"]["id"] = ca["study"].get("id") or str(a.get("codebook"))
            cb["study"]["id"] = cb["study"].get("id") or str(b.get("codebook"))
            if ca["study"]["id"] == cb["study"]["id"]:
                cb["study"]["id"] += " (b)"
            return archeion.ask("common", codebooks=[ca, cb], a={"study": ca["study"]["id"], "variable": a.get("variable")}, b={"study": cb["study"]["id"], "variable": b.get("variable")},
                                corresponds=body.get("corresponds", []), name=str(body.get("name") or "harmonised")), 200
        handler.answer(common)
        return True
    return False
