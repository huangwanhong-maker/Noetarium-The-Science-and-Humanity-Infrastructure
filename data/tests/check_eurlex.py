"""Checks of the reader of European Union law (data/formats/eurlex.py), offline, on markup written here after the Publications
Office's: an act with a recital, an article of numbered paragraphs whose points are rows of tables (one nested), an article of
definitions with no numbered paragraph, and an annex of clauses; a judgment in the newer markup and one in the older. Each provision
has the identifier that says where it stands, the label a lawyer cites, its words without footnote marks, and its parent. Where
the official texts are in the cache (data/products/legal_texts.py), the GDPR is held to what a reader finds in it: Article 46(2)(c),
Article 4(1), Recital 108, and the Court's paragraph 105 in Schrems II.

    python data/tests/check_eurlex.py
"""

from __future__ import annotations

import json

from checks import PRODUCTS, assert_, skip

import eurlex

ACT = """<html><body>
<div class="eli-container">
 <div class="eli-subdivision" id="rct_1"><table><tbody><tr><td><p class="oj-normal">(1)</p></td>
   <td><p class="oj-normal">Transfers  are necessary <a class="oj-super oj-note-tag" href="#n1">(1)</a> for trade.</p></td></tr></tbody></table></div>
 <div class="eli-subdivision" id="art_4"><p class="oj-ti-art">Article 4</p><div class="eli-title"><p class="oj-sti-art">Definitions</p></div>
   <p class="oj-normal">For the purposes of this Regulation:</p>
   <table><tbody><tr><td><p class="oj-normal">(1)</p></td><td><p class="oj-normal">&#8216;datum&#8217; means a thing given;</p></td></tr></tbody></table>
   <table><tbody><tr><td><p class="oj-normal">(2)</p></td><td><p class="oj-normal">&#8216;holder&#8217; means who has it.</p></td></tr></tbody></table></div>
 <div class="eli-subdivision" id="art_46"><p class="oj-ti-art">Article 46</p><div class="eli-title"><p class="oj-sti-art">Safeguards</p></div>
   <div id="046.001"><p class="oj-normal">1.   A holder may transfer a datum only with safeguards.</p></div>
   <div id="046.002"><p class="oj-normal">2.   The safeguards may be provided by:</p>
     <table><tbody><tr><td><p class="oj-normal">(a)</p></td><td><p class="oj-normal">an instrument;</p></td></tr></tbody></table>
     <table><tbody><tr><td><p class="oj-normal">(b)</p></td><td><p class="oj-normal">clauses, being:</p>
        <table><tbody><tr><td><p class="oj-normal">(i)</p></td><td><p class="oj-normal">standard; or</p></td></tr>
                      <tr><td><p class="oj-normal">(ii)</p></td><td><p class="oj-normal">approved.</p></td></tr></tbody></table></td></tr></tbody></table>
     <p class="oj-normal">The first subparagraph is without prejudice to paragraph 1.</p></div></div>
 <p class="oj-ti-grseq-1">SECTION I</p>
 <p class="oj-ti-grseq-1"><span class="oj-italic">Clause 14</span></p>
 <p class="oj-ti-grseq-1"><span class="oj-bold">Local laws</span></p>
 <table><tbody><tr><td><p class="oj-normal">(a)</p></td><td><p class="oj-normal">The Parties warrant it.</p></td></tr></tbody></table>
 <p class="oj-ti-grseq-1"><span class="oj-italic">Clause 15</span></p>
 <p class="oj-ti-grseq-1"><span class="oj-bold">Access</span></p>
 <p class="oj-normal">The importer shall notify.</p>
</div></body></html>"""

p = {x["id"]: x for x in eurlex.read_act(ACT, "act")}
assert_("eurlex: a recital is read under its number, without the mark of its footnote, its whitespace evened",
        p["act/rct-1"]["label"] == "Recital 1" and p["act/rct-1"]["text"] == "Transfers are necessary for trade.", json.dumps(p.get("act/rct-1")))
assert_("eurlex: an article is its title; a numbered paragraph its words without its number, under the article",
        p["act/art-46"]["text"] == "Safeguards" and p["act/art-46"]["title"] == "Safeguards" and p["act/art-46/1"]["label"] == "Article 46(1)"
        and p["act/art-46/1"]["text"] == "A holder may transfer a datum only with safeguards." and p["act/art-46/1"]["parent"] == "act/art-46")
assert_("eurlex: a point is a row of a table, cited by its marker, under its paragraph; a point within a point under that point",
        p["act/art-46/2/a"]["label"] == "Article 46(2)(a)" and p["act/art-46/2/a"]["text"] == "an instrument;" and p["act/art-46/2/b"]["text"] == "clauses, being:"
        and p["act/art-46/2/b/ii"]["label"] == "Article 46(2)(b)(ii)" and p["act/art-46/2/b/ii"]["text"] == "approved." and p["act/art-46/2/b/ii"]["parent"] == "act/art-46/2/b")
assert_("eurlex: a paragraph's words are those before its points and those after, the points apart",
        p["act/art-46/2"]["text"] == "The safeguards may be provided by:\nThe first subparagraph is without prejudice to paragraph 1.")
assert_("eurlex: an article with no numbered paragraph keeps its own words, and its points stand directly under it",
        p["act/art-4"]["text"] == "For the purposes of this Regulation:" and p["act/art-4/1"]["label"] == "Article 4(1)" and p["act/art-4/1"]["text"] == "‘datum’ means a thing given;" and p["act/art-4/2"]["parent"] == "act/art-4")
assert_("eurlex: an annex's clauses are opened by their headings: the clause's title, its words, its points; a section's heading opens none",
        p["act/clause-14"]["title"] == "Local laws" and p["act/clause-14/a"]["label"] == "Clause 14(a)" and p["act/clause-14/a"]["text"] == "The Parties warrant it."
        and p["act/clause-15"]["text"] == "The importer shall notify." and not any("SECTION" in x["text"] for x in p.values()))
assert_("eurlex: every identifier is given once, and every parent is there", len(p) == len(eurlex.read_act(ACT, "act")) and all(not x["parent"] or x["parent"] in p for x in p.values()))

NEW = """<html><body><p class="coj-normal">JUDGMENT OF THE COURT</p>
<table><tbody><tr><td><p class="coj-count">104</p></td><td><p class="coj-normal">It follows that the level  must be equivalent.</p></td></tr></tbody></table>
<table><tbody><tr><td><p class="coj-count">105</p></td><td><p class="coj-normal">Therefore the answer is:</p>
  <table><tbody><tr><td><p class="coj-count">&#8212;</p></td><td><p class="coj-normal">yes.</p></td></tr></tbody></table></td></tr></tbody></table></body></html>"""
OLD = NEW.replace("coj-count", "count").replace("coj-normal", "normal")
for name, markup in (("the newer", NEW), ("the older", OLD)):
    j = eurlex.read_judgment(markup)
    assert_(f"eurlex: a judgment in {name} markup is read into its numbered paragraphs, a list within a paragraph kept in it, and its whole text",
            [x["n"] for x in j["paragraphs"]] == [104, 105] and j["paragraphs"][0]["text"] == "It follows that the level must be equivalent."
            and j["paragraphs"][1]["text"].startswith("Therefore the answer is:") and "yes." in j["paragraphs"][1]["text"] and "JUDGMENT OF THE COURT" in j["text"], json.dumps(j["paragraphs"]))

# ---- The official texts, where fetched ------------------------------------------------------------------------------------------
texts = PRODUCTS / "legal-texts"
if not (texts / "gdpr.json").is_file():
    skip("eurlex: the GDPR's official text is not in the cache (python data/products/legal_texts.py)")
else:
    g = json.loads((texts / "gdpr.json").read_text(encoding="utf-8"))
    by = {x["id"]: x for x in g["provisions"]}
    assert_("eurlex: the Regulation has its 99 articles and 173 recitals, every identifier once, every parent there",
            sum(1 for x in g["provisions"] if "/art-" in x["id"] and not x["parent"]) == 99 and sum(1 for x in g["provisions"] if "/rct-" in x["id"]) == 173
            and len(by) == len(g["provisions"]) and all(not x["parent"] or x["parent"] in by for x in g["provisions"]))
    assert_("eurlex: Article 46(2)(c) is the standard data protection clauses adopted by the Commission, under Article 46(2)",
            by["gdpr/art-46/2/c"]["text"].startswith("standard data protection clauses adopted by the Commission") and by["gdpr/art-46/2/c"]["parent"] == "gdpr/art-46/2" and by["gdpr/art-46"]["text"] == "Transfers subject to appropriate safeguards")
    assert_("eurlex: Article 4(1) defines personal data, and Recital 108 speaks of measures to compensate",
            "any information relating to an identified or identifiable natural person" in by["gdpr/art-4/1"]["text"] and "compensate for the lack of data protection in a third country" in by["gdpr/rct-108"]["text"])
    assert_("eurlex: the record says where the text came from, when, the digest of what was received, and the terms of its reuse",
            g["celex"] == "32016R0679" and g["source"].endswith("/celex/32016R0679") and len(g["sha256"]) == 64 and len(g["fetched"]) == 10 and "source acknowledged" in g["reuse"])
    if (texts / "schrems-ii.json").is_file():
        s2 = json.loads((texts / "schrems-ii.json").read_text(encoding="utf-8"))
        para = {x["id"]: x for x in s2["provisions"]}["schrems-ii/para-105"]["text"]
        assert_("eurlex: paragraph 105 of Schrems II holds that the level of protection must be essentially equivalent, and the judgment has its 203 paragraphs",
                "afforded a level of protection essentially equivalent to that guaranteed within the European Union" in para and len(s2["provisions"]) == 203 and para in s2["text"])
    else:
        skip("eurlex: Schrems II is not in the cache")
