"""Checks of DDI-Codebook 2.5 (data/formats/ddi.py) and of OAI-PMH pages (data/acquisition/oai_pmh.py), offline: a codebook
written and read back as it was, every measure kept, the language asked for chosen, reduced dates kept as given, an OAI-PMH page
with its resumption token and its errors read; and, where a harvest is in the cache, the catalogue of social-science studies:
every study with its identifier, title and datestamp, no study twice, and a page of the harvest read into as many codebooks as it
holds records.

    python data/tests/check_ddi.py
"""

from __future__ import annotations

import json

from checks import ROOT, assert_, skip

import ddi
from oai_pmh import OaiError, parse_page

CODEBOOK = {
    "study": {"id": "example-survey-1", "title": "An example social survey, round 1", "producer": "an example consortium", "version": "1.0",
              "released": "2025-06-30", "doi": "10.0000/example", "citation": "An example consortium (2025), An example social survey, round 1.",
              "universe": "persons aged 15 and over in private households", "sampling": "stratified probability sample", "mode": "face to face",
              "from": "2024-09", "until": "2025-02", "countries": ["AT", "BE"], "access": "registered users", "licence": "CC BY 4.0",
              "authors": ["A. Researcher", "B. Researcher"], "distributors": ["an example archive"], "keywords": ["trust", "politics"],
              "topics": ["Political behaviour and attitudes"], "unit": "Individual", "time_method": "Cross-section", "identifiers": {}, "holdings": ""},
    "variables": [
        {"name": "gndr", "label": "Gender", "question": "Code the respondent's gender", "universe": "", "unit": "", "derivation": "", "notes": "",
         "measure": "nominal", "categories": [{"code": "1", "label": "Male", "missing": False}, {"code": "2", "label": "Female", "missing": False},
                                              {"code": "9", "label": "No answer", "missing": True}], "least": None, "most": None, "weight": False},
        {"name": "trstprl", "label": "Trust in parliament", "question": "How much do you personally trust the parliament?", "universe": "All", "unit": "",
         "derivation": "", "notes": "", "measure": "ordinal", "categories": [{"code": str(k), "label": str(k), "missing": False} for k in range(11)] + [{"code": "88", "label": "Don't know", "missing": True}],
         "least": None, "most": None, "weight": False},
        {"name": "agea", "label": "Age of respondent", "question": "", "universe": "", "unit": "", "derivation": "From the year of birth", "notes": "",
         "measure": "discrete", "categories": [{"code": "999", "label": "Not available", "missing": True}], "least": 15.0, "most": 120.0, "weight": False},
        {"name": "pspwght", "label": "Post-stratification weight", "question": "", "universe": "", "unit": "", "derivation": "", "notes": "Raked to the census",
         "measure": "continuous", "categories": [], "least": 0.0, "most": 10.5, "weight": True},
        {"name": "comment", "label": "Interviewer's comment", "question": "", "universe": "", "unit": "", "derivation": "", "notes": "",
         "measure": "text", "categories": [], "least": None, "most": None, "weight": False},
        {"name": "inwdate", "label": "Date of interview", "question": "", "universe": "", "unit": "", "derivation": "", "notes": "",
         "measure": "date", "categories": [], "least": None, "most": None, "weight": False},
    ],
    "files": [{"name": "round1.csv", "digest": "", "variables": ["gndr", "trstprl", "agea", "pspwght", "comment", "inwdate"], "cases": 1200}],
}


def main() -> None:
    xml = ddi.write(CODEBOOK)
    back = ddi.read(xml)
    assert_("ddi: a codebook written as DDI-C 2.5 reads back as one codebook", len(back) == 1)
    got = back[0]
    want_study = {k: v for k, v in CODEBOOK["study"].items() if k not in ("identifiers",)}
    got_study = {k: v for k, v in got["study"].items() if k in want_study}
    differ = [k for k in want_study if want_study[k] != got_study.get(k) and k != "id"]
    assert_("ddi: the study's citation, dates, places, method and access read back as written", not differ and got["study"]["id"] in ("example-survey-1",), f"differ: {differ}")
    assert_("ddi: every variable reads back with its label, question, measure, categories (missing flags among them), range and weight",
            got["variables"] == CODEBOOK["variables"], json.dumps([v for v, w in zip(got["variables"], CODEBOOK["variables"]) if v != w])[:400])
    assert_("ddi: the file reads back with its variables and cases", got["files"] == CODEBOOK["files"], json.dumps(got["files"]))
    assert_("ddi: reduced dates (a year and a month) are kept as given, not filled", got["study"]["from"] == "2024-09" and got["study"]["until"] == "2025-02")
    multi = ('<codeBook xmlns="ddi:codebook:2_5" version="2.5"><stdyDscr><citation><titlStmt>'
             '<titl xml:lang="de">Eine Umfrage</titl><titl xml:lang="en">A survey</titl><IDNo agency="DOI">doi:10.1/x</IDNo></titlStmt></citation>'
             '<stdyInfo><sumDscr><nation xml:lang="de" abbr="AT">Österreich</nation><nation xml:lang="en" abbr="AT">Austria</nation></sumDscr></stdyInfo></stdyDscr></codeBook>')
    de, en = ddi.read(multi, "de")[0]["study"], ddi.read(multi, "fr")[0]["study"]
    assert_("ddi: a title in several languages is read in the language asked for, else in English", de["title"] == "Eine Umfrage" and en["title"] == "A survey")
    assert_("ddi: a DOI given as doi:… is kept without its prefix, and a nation by its abbreviation once", de["doi"] == "10.1/x" and de["countries"] == ["AT"])
    bad = dict(CODEBOOK, variables=[dict(CODEBOOK["variables"][0], measure="vibes")])
    try:
        ddi.write(bad)
        refused = False
    except ValueError:
        refused = True
    assert_("ddi: a measure that is none of the six is refused when written", refused)

    page = (b'<?xml version="1.0"?><OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/"><ListRecords>'
            b'<record><header><identifier>a</identifier><datestamp>2026-01-01T00:00:00Z</datestamp><setSpec>language:en</setSpec></header>'
            b'<metadata><codeBook xmlns="ddi:codebook:2_5"><stdyDscr><citation><titlStmt><titl>A</titl></titlStmt></citation></stdyDscr></codeBook></metadata>'
            b'<about><provenance xmlns="http://www.openarchives.org/OAI/2.0/provenance"><originDescription><baseURL>https://archive.example/oai</baseURL></originDescription></provenance></about></record>'
            b'<record><header status="deleted"><identifier>b</identifier><datestamp>2026-01-02T00:00:00Z</datestamp></header></record>'
            b'<resumptionToken completeListSize="250" cursor="0">next-page</resumptionToken></ListRecords></OAI-PMH>')
    records, token, size = parse_page(page)
    assert_("oai: a page's records are read with their headers, metadata and origin, a deleted record without metadata, and the token for the next page",
            len(records) == 2 and records[0][2] == "https://archive.example/oai" and records[1][1] is None and records[1][0].get("status") == "deleted" and token == "next-page" and size == 250)
    last = b'<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/"><ListRecords><resumptionToken completeListSize="250" cursor="200"/></ListRecords></OAI-PMH>'
    assert_("oai: an empty token ends the harvest", parse_page(last)[1] is None)
    none = b'<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/"><error code="noRecordsMatch">nothing</error></OAI-PMH>'
    assert_("oai: noRecordsMatch is an empty harvest, not a failure", parse_page(none)[0] == [])
    try:
        parse_page(b'<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/"><error code="badResumptionToken">expired</error></OAI-PMH>')
        raised = ""
    except OaiError as error:
        raised = error.code
    assert_("oai: another error is raised with the repository's code", raised == "badResumptionToken")

    folder = ROOT / "data" / "cache" / "social-studies"
    catalogue = folder / "catalogue.jsonl"
    if not catalogue.exists():
        skip("social studies: no harvest in the cache (python data/products/social_studies.py)")
        return
    rows = [json.loads(line) for line in catalogue.read_text(encoding="utf-8").splitlines() if line.strip()]
    complete = [r for r in rows if r.get("oai") and r.get("title") and r.get("datestamp")]
    assert_(f"social studies: every one of the {len(rows)} studies harvested has its OAI identifier, title and datestamp", len(complete) == len(rows) and rows,
            f"{len(rows) - len(complete)} incomplete")
    assert_("social studies: no study is kept twice", len({r["oai"] for r in rows}) == len(rows))
    pages = sorted(folder.glob("pages/*/page-*.xml"))
    if pages:
        recs, _, _ = parse_page(pages[0].read_bytes())
        books = ddi.read(pages[0].read_bytes())
        live = [r for r in recs if r[1] is not None]
        assert_(f"social studies: a page of the harvest reads into as many codebooks as it holds records with metadata ({len(books)})", len(books) == len(live) and books)


if __name__ == "__main__":
    main()
