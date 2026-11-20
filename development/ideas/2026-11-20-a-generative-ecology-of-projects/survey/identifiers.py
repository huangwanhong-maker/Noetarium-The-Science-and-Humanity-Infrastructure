"""Fetches the record of every DOI and arXiv identifier in the reference lists of the survey's parts.

    python identifiers.py            writes identifiers.tsv and identifiers.json beside this file
    python identifiers.py --recheck  compares again, on the records already fetched

The survey's parts were written by search agents, which marked each work [V] or [P]. This script is a second,
mechanical check of the identifiers alone: it asks Crossref for each DOI and arXiv for each arXiv identifier,
and records the title, first author, year and venue that the record gives, beside the line of the survey that
cites it. A request holds only the identifier. A line whose identifier resolves to another work than the line
names is to be corrected in the part; the ledger makes such a line visible.
"""
from __future__ import annotations

import csv
import io
import json
import re
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET
from pathlib import Path

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")
HERE = Path(__file__).resolve().parent
AGENT = "reference-checker/1.0"
DOI = re.compile(r"\b(10\.\d{4,9}/[^\s,;\]]+)")
ARXIV = re.compile(r"arXiv:\s*(\d{4}\.\d{4,5})", re.I)


def get(url: str, tries: int = 3):
    for attempt in range(tries):
        try:
            request = urllib.request.Request(url, headers={"User-Agent": AGENT, "Accept": "application/json, application/atom+xml, */*"})
            with urllib.request.urlopen(request, timeout=40) as answer:
                return answer.read()
        except urllib.error.HTTPError as error:
            if error.code == 404:
                return None
            time.sleep(2.0 * (attempt + 2))
        except Exception:
            time.sleep(2.0 * (attempt + 1))
    return None


def crossref(doi: str):
    raw = get("https://api.crossref.org/works/" + urllib.parse.quote(doi, safe="/"))
    if not raw:
        return None
    try:
        item = json.loads(raw)["message"]
    except Exception:
        return None
    year = None
    for key in ("published-print", "issued", "published-online", "published"):
        parts = (item.get(key) or {}).get("date-parts") or [[None]]
        if parts and parts[0] and parts[0][0]:
            year = parts[0][0]
            break
    authors = item.get("author") or item.get("editor") or []
    first = (authors[0].get("family") or authors[0].get("name") or "") if authors else ""
    return {"title": " ".join(item.get("title") or []), "first": first, "authors": len(authors), "year": year,
            "venue": " ".join(item.get("container-title") or []) or item.get("publisher", ""), "source": "Crossref"}


def arxiv(ids: list[str]) -> dict:
    out = {}
    for start in range(0, len(ids), 30):
        raw = get("https://export.arxiv.org/api/query?max_results=100&id_list=" + ",".join(ids[start:start + 30]))
        time.sleep(3.2)
        if not raw:
            continue
        ns = {"a": "http://www.w3.org/2005/Atom"}
        try:
            root = ET.fromstring(raw)
        except Exception:
            continue
        for entry in root.findall("a:entry", ns):
            ident = re.sub(r"v\d+$", "", (entry.findtext("a:id", "", ns) or "").rsplit("/abs/", 1)[-1])
            names = [(n.findtext("a:name", "", ns) or "").strip() for n in entry.findall("a:author", ns)]
            out[ident] = {"title": re.sub(r"\s+", " ", entry.findtext("a:title", "", ns) or "").strip(),
                          "first": names[0].split()[-1] if names else "", "authors": len(names),
                          "year": int((entry.findtext("a:published", "", ns) or "0000")[:4]), "venue": "arXiv", "source": "arXiv"}
    return out


def fold(text: str) -> str:
    import unicodedata
    return "".join(c for c in unicodedata.normalize("NFKD", text or "") if not unicodedata.combining(c)).lower()


def agrees(line: str, record) -> bool:
    """Whether the record's first author is named in the survey's line. A record that gives no author
    (some older ones do not) is taken to agree when it resolved."""
    if not record:
        return False
    return not record.get("first") or fold(record["first"]) in fold(line)


def reference_lines(text: str) -> list[str]:
    """The lines of the part's list of references: those that begin with a mark."""
    start = max(text.rfind("\n## 4."), 0)
    return [line.strip().lstrip("- ").strip() for line in text[start:].splitlines() if re.match(r"^\s*(- )?\[(V|P|U)\]", line)]


def write(rows: list[dict]) -> None:
    (HERE / "identifiers.json").write_text(json.dumps(rows, indent=1, ensure_ascii=False), encoding="utf-8")
    with open(HERE / "identifiers.tsv", "w", encoding="utf-8", newline="") as out:
        w = csv.writer(out, delimiter="	")
        w.writerow(["part", "mark", "kind", "identifier", "resolved", "first_author_agrees", "record_first_author", "record_year", "record_title", "record_venue", "line_of_the_survey"])
        for r in rows:
            w.writerow([r["part"], r["mark"], r["kind"], r["identifier"], r["resolved"], r["first_author_agrees"], r["first"], r["year"], r["title"][:160], r["venue"][:60], r["line"]])
    with_id = [r for r in rows if r["kind"] != "none"]
    print("lines", len(rows), "| with an identifier", len(with_id), "| resolved", sum(r["resolved"] for r in with_id),
          "| first author agrees", sum(r["first_author_agrees"] for r in with_id))
    for r in with_id:
        if not r["resolved"] or not r["first_author_agrees"]:
            print("  look at:", r["part"], r["identifier"], "|", r["first"], r["year"], "|", r["title"][:60], "||", r["line"][:70])


def main() -> None:
    if "--recheck" in sys.argv:        # the comparison made again on the records already fetched
        rows = json.loads((HERE / "identifiers.json").read_text(encoding="utf-8"))
        for row in rows:
            row["first_author_agrees"] = agrees(row["line"], row if row.get("resolved") else None)
        write(rows)
        return
    rows, wanted_arxiv = [], []
    for part in sorted(HERE.glob("part-*.md")):
        for line in reference_lines(part.read_text(encoding="utf-8")):
            mark = line[1]
            dois = [d.rstrip(".)") for d in DOI.findall(line)]
            ids = ARXIV.findall(line)
            for d in dois:
                rows.append({"part": part.name[:6], "mark": mark, "kind": "doi", "identifier": d, "line": line[:150]})
            for a in ids:
                rows.append({"part": part.name[:6], "mark": mark, "kind": "arxiv", "identifier": a, "line": line[:150]})
                wanted_arxiv.append(a)
            if not dois and not ids:
                rows.append({"part": part.name[:6], "mark": mark, "kind": "none", "identifier": "", "line": line[:150]})
    found = arxiv(sorted(set(wanted_arxiv)))
    for n, row in enumerate(rows, 1):
        record = None
        if row["kind"] == "doi":
            record = crossref(row["identifier"])
            time.sleep(0.2)
        elif row["kind"] == "arxiv":
            record = found.get(row["identifier"])
        row.update(record or {"title": "", "first": "", "authors": 0, "year": None, "venue": "", "source": ""})
        row["resolved"] = bool(record)
        row["first_author_agrees"] = agrees(row["line"], record)
        if n % 25 == 0:
            print(n, "of", len(rows))
    write(rows)


if __name__ == "__main__":
    main()
