# A Survey of the Sources of International Privacy and Data-Protection Law

*Prepared 15 November 2026 for the studio of legal practice. Its first body of law is the transfer of personal data across borders. Status: research note (`draft`).*

---

## 1. Scope, Method and Conventions

### 1.1 Scope

This survey maps the sources that a studio of legal practice needs before it states any rule. The sources are:

- the United Nations;
- the Council of Europe, the European Convention on Human Rights (ECHR) and the European Court of Human Rights (ECtHR);
- the OECD;
- APEC and the Global CBPR Forum;
- the European Union (the GDPR, the Charter, adequacy decisions, standard contractual clauses, EDPB guidance and the 2025–2026 amendments);
- the decisive case law;
- the principal regional instruments outside Europe.

For each source it gives:

- the official location;
- the machine-readable forms, and whether a script can fetch them;
- the terms of reuse;
- the structure that matters for encoding clause by clause;
- the status, or the parties, and where that data can be fetched.

It ends by proposing the first practice scheme.

### 1.2 Method of Verification

Every rule quoted here was read from an official text on 15 November 2026. The official texts used were:

- the Publications Office's Cellar for EU acts and CJEU judgments;
- CURIA's legacy document server;
- HUDOC's own endpoints;
- the UN Treaty Collection and the UN's Official Document System;
- the Council of Europe's document store (`rm.coe.int`);
- the OECD Compendium;
- the publishing bodies' own PDFs.

Texts were fetched by script and quoted from the downloaded files, not from summaries. Local copies are kept for re-checking:

- `scratchpad\eu\celex\<CELEX>.txt`, holding every EU act cited;
- `scratchpad\cases\`, holding every judgment cited;
- `scratchpad\mine\`, holding the SPARQL queries and Formex samples.

Where an official page refused automated access, this is stated. Where a fact rests only on secondary sources (law firms, aggregators, press) or on search-engine snippets, it is marked **UNVERIFIED**.

### 1.3 Marks Used

- A quoted rule is in quotation marks and is verbatim, with its source.
- **UNVERIFIED** marks a fact not confirmed from an official page.
- *Caveat.* marks a limit on what a source permits or shows.
- Reuse assessments are a reading of the publishers' notices, not legal advice.

### 1.4 Corrections to the Brief

1. **The OECD Declaration on Government Access to Personal Data Held by Private Sector Entities** is OECD/LEGAL/0487, adopted on **14 December 2022**, not 2023 or 2024. Its only later entry is a change of 13 February 2023, C(2023)15.
2. **Case C-319/22** is *Gesamtverband Autoteile-Handel eV v Scania CV AB* (9 November 2023), not "Gesamtverband Autoversicherer".
3. **Formations of two judgments.** *Breyer* (C-582/14) was decided by the Second Chamber. *EDPS v SRB* (C-413/23 P) was decided by the First Chamber.
4. **The legal bases of two judgments.** *EDPS v SRB* and *Bindl* (T-354/22) interpret Regulation (EU) 2018/1725, which governs the EU institutions and whose definitions mirror the GDPR's. They do not interpret the GDPR itself.
5. **The GDPR has never been amended.** Its only consolidated version is CELEX 02016R0679-20160504, with the corrigenda incorporated. The Digital Omnibus is a proposal only, and it **does not touch Chapter V**.
6. **The United Kingdom's adequacy decisions** were not replaced by new decisions. They were extended to 27 December 2025 and then renewed by amendment, to expire on **27 December 2031**.

---

## 2. Principal Findings

1. **Convention 108+ is not in force.** The amending Protocol CETS No. 223 needs either every party to ETS 108 or, after 11 October 2023, at least thirty-eight parties.
   - It stands at **46 signatures and 34 ratifications**. Moldova was the 34th, depositing on 15 May 2026.
   - Source: the Committee of Convention 108's 50th plenary report, 11 June 2026, and the Council of Europe's news item.
   - No later ratification was found up to 15 November 2026 (**UNVERIFIED** against the live chart, which refuses scripts).
2. **The EU–US Data Privacy Framework stands, under appeal.**
   - Implementing Decision (EU) 2023/1795 is in force.
   - The General Court dismissed *Latombe v Commission* (T-553/23) on 3 September 2025 (ECLI:EU:T:2025:831). It held that, "on the date of adoption of the contested decision", the United States ensured an adequate level of protection.
   - The appeal, **C-703/25 P**, lodged on 31 October 2025, is pending. No Advocate General's opinion or judgment was found.
   - The Commission's next periodic review is due about October 2027.
3. **The list of adequacy decisions now holds eighteen acts.** Two are new: the European Patent Organisation (2025/1382, the first international organisation) and Brazil (2026/179, 26 January 2026). The United Kingdom's decisions were renewed to 2031.
4. **Reuse.**
   - **Clean for an MIT repository, provided the source is credited and the texts are kept as third-party material:**
     - EU legislation in EUR-Lex: "Unless otherwise specified, you can re-use the legal documents published in EUR-Lex for commercial or non-commercial purposes" (EUR-Lex copyright notice, which rests on Decision 2011/833/EU);
     - CJEU case law: "can be reproduced provided the source is acknowledged";
     - EDPB documents;
     - UN documents issued under a symbol, which are in the public domain under ST/AI/189/Add.9/Rev.2;
     - Council of Europe treaties: "official texts … in the public domain", with a stated credit line.
   - **Restrictive:**
     - ECtHR texts: reproduction only "for private use or for the purposes of information and education", free of charge; commercial use needs written permission;
     - OECD instruments: "not altered", "may not be sold";
     - APEC: non-profit use, no modification, no mirroring;
     - the AU, ASEAN, ITU/SADC: all rights reserved;
     - the Global CBPR Forum and the RIPD state no terms.
5. **What can be fetched by script.**
   - Cellar, by content negotiation on a CELEX number and by SPARQL, serves Formex 4 XML and XHTML for every EU act and judgment checked here.
   - CURIA's legacy host serves judgments.
   - HUDOC's front-end endpoints serve JSON search results and HTML, DOCX and PDF texts.
   - The UN Treaty Collection serves an XML export of a treaty's status.
   - The UN's Official Document System serves PDFs by symbol.
   - The OECD Compendium serves JSON for an instrument.
   - EUR-Lex HTML, the Council of Europe's treaty charts, coe.int and ohchr.org pages refuse scripts.
   - **No Akoma Ntoso** is published for any act checked here; Cellar holds only the AKN4EU schema itself.
6. **Proposed first scheme.** The question is "may this transfer proceed, on what ground, with what safeguards, and what does it rest on?" The scheme reads it as a chain of determinations:
   1. whether the data are personal data, under Art 4(1) and Recital 26, as read in *Breyer*, *Scania*, *IAB Europe* and *EDPS v SRB*;
   2. whether the GDPR applies to the exporter (Art 3);
   3. whether there is a transfer (EDPB Guidelines 05/2021; *Bindl*, under appeal);
   4. whether the underlying processing is lawful (Arts 5, 6, 9 and 28);
   5. which ground applies, in the GDPR's order: Art 45 adequacy, then Art 46 safeguards (the SCCs of Decision 2021/914 with the Clause 14 assessment and supplementary measures, after *Schrems II* and EDPB Recommendations 01/2020 and 02/2020), then the Art 49 derogations, read restrictively.

   All of this sits under Charter Arts 7, 8 and 52, and through Art 52(3) under ECHR Art 8 as read in *Big Brother Watch* and *Centrum för rättvisa*. ICCPR Art 17 with General Comment 16, and Convention 108, its Additional Protocol and 108+ (marked not in force), form the international frame.

---

## 3. Terms of Reuse and Their Bearing on an MIT-Licensed Repository

### 3.1 The Principle: Third-Party Texts Stand Outside the MIT Grant

The MIT licence can grant only what the repository's authors own. Verbatim official texts are not theirs. Each such text is reproduced under its publisher's own permission, and that permission must travel with it.

*Rationale.* Two of the publishers below (the ECtHR and APEC) permit only non-commercial use, and one (the OECD) forbids alteration and sale. MIT's grant includes commercial use, modification and sale. Placing their texts under MIT without qualification would misstate the permission.

The recommended practice is in §3.8.

### 3.2 European Union

#### 3.2.1 EUR-Lex

The EUR-Lex copyright notice is quoted below. The live page refuses scripts, so the text was read from the Internet Archive's capture of 29 May 2025 of `https://eur-lex.europa.eu/content/legal-notice/legal-notice.html`. Re-check it in a browser before relying on it.

> "The Commission's document reuse policy is based on Decision 2011/833/EU. Unless otherwise specified, you can re-use the legal documents published in EUR-Lex for commercial or non-commercial purposes. Some documents, like the International Accounting Standards, may be subject to special conditions of use; these are mentioned in the respective Official Journal/document. … The copyright for the editorial content of this website, the summaries of EU legislation and the consolidated texts, which is owned by the EU, is licensed under the Creative Commons Attribution 4.0 International licence. This means that you can re-use the content provided you acknowledge the source and indicate any changes you have made. … EUR-Lex metadata is dedicated to the public domain in accordance with the Creative Commons CC0 1.0 Universal Public Domain Dedication deed."

The same capture says on authenticity:

> "Only European Union documents published in the Official Journal of the European Union are deemed authentic. For the rules governing authenticity of the Official Journal, see Council Regulation (EU) No. 216/2013 of 7 March 2013. With respect to case-law, only the versions of documents published in the "European Court Reports" are deemed official sources."

#### 3.2.2 Commission Decision 2011/833/EU on the Reuse of Commission Documents

The Decision is dated 12 December 2011, published at OJ L 330, 14.12.2011, p. 39, CELEX 32011D0833. Text read from Cellar:

- **Art 4:** "All documents shall be available for reuse: (a) for commercial or non-commercial purposes under the conditions laid down in Article 6; (b) without charge, subject to the provisions laid down in Article 9; and (c) without the need to make an individual application, unless otherwise provided in Article 7."
- **Art 6(1):** "Documents shall be made available for reuse without application unless otherwise specified and without restrictions or, where appropriate, an open licence or disclaimer setting out conditions explaining the rights of reusers."
- **Art 6(2)** lists the possible conditions: "(a) the obligation for the reuser to acknowledge the source of the documents; (b) the obligation not to distort the original meaning or message of the documents; (c) the non-liability of the Commission for any consequence stemming from the reuse."
- **Art 2(2)(b):** it does not apply "to documents for which the Commission is not in a position to allow their reuse in view of intellectual property rights of third parties".

*Remark.* The Decision governs Commission documents, and documents published by the Publications Office on the Commission's behalf. EUR-Lex extends the same policy to all legal documents it publishes. The Commission's websites, including the adequacy and SCC pages and the Q&A, apply CC BY 4.0 to EU-owned content (Commission legal notice, seen through search; **UNVERIFIED** by fetch).

#### 3.2.3 EDPB

Source: `https://edpb.europa.eu/copyright_en`.

> "The reuse of any information of this website is authorized for commercial and non-commercial purposes, under the following conditions: The re-user is obliged to acknowledge the source of the document; The original meaning or the message of the documents should not be distorted; The European Data Protection Board cannot be held liable for any consequence stemming from the reuse."

#### 3.2.4 Court of Justice of the European Union

Source: the Copyright page, `https://curia.europa.eu/site/jcms/d2_5167/en/copyright`.

> "The original content on this website is copyright of the European Union. Editorial content and the case-law can be reproduced provided the source is acknowledged. Some content, in particular images, may be copyrighted by third parties." … "You may reproduce the judicial decisions found in the case-law database."

For editorial content: "you must acknowledge the source – "Court of Justice of the European Union". You must not distort the original meaning or message of the content. You must also indicate any changes that you have made."

The older InfoCuria disclaimer (`https://curia.europa.eu/common/juris/en/avertissement.html`) adds:

> "The texts available on this site may be reproduced provided the source is acknowledged as well as the fact that they are not authentic and are free of charge."

The legal notice says: "Only the versions of these documents that are published in the Reports of Cases or the EU Official Journal are authentic."

**Bearing on MIT.** EU legislation, EDPB documents and CJEU judgments may be quoted verbatim in the repository under credit, and commercial reuse is permitted. Each record should still carry three things:

- its credit (for example "Source: EUR-Lex, © European Union", or "Court of Justice of the European Union");
- a statement that the text is not the authentic Official Journal or Reports edition;
- a pointer to the permission.

### 3.3 Council of Europe and the European Court of Human Rights

#### 3.3.1 Council of Europe

Source: the licensing page, `https://www.coe.int/en/web/portal/copyright-licensing-permissions`.

> "Official texts No permission is required to reproduce and translate official texts (conventions and treaties, Committee of Ministers resolutions and recommendations) which are in the public domain. Proper credits are required: "From [full title of the publication], [report date, adopted on (date)]. Source: Council of Europe [year]"."
>
> "No permission is required for the use of excerpts containing less than 500 words. Any use exceeding this requires prior written permission."

*Caveat.* Explanatory reports are not named among the "official texts". Quote them in excerpts under 500 words, or ask permission.

#### 3.3.2 European Court of Human Rights

Source: `https://www.echr.coe.int/copyright-and-disclaimer`.

> "The information and texts available on the Court's website may be reproduced provided the source is acknowledged (© ECHR-CEDH) and the reproduction is made for private use or for the purposes of information and education in connection with the Court's activities. This authorisation is subject to the condition that the source is indicated and that any such reproduction is free of charge." … "Persons wishing to use information and texts on the Court's website for any use other than those listed above, and in particular for commercial use, must request prior written permission using the form available on the website."

Translations on HUDOC are reported to be "copyright protected" and not reproducible without approval. This rests on search snippets (**UNVERIFIED**).

**Bearing on MIT.**

- Treaty clauses of the Council of Europe (ECHR Art 8, ETS 108, ETS 181, CETS 223) may be quoted verbatim, with the stated credit line.
- ECtHR judgments may not be placed under an MIT grant. Either:
  - ask for written permission through the Court's form (recommended before publishing the scheme); or
  - hold the paragraphs in a directory under the Court's notice, kept to the passages the scheme needs, and say so.

### 3.4 United Nations

**ST/AI/189/Add.9/Rev.2**, "Copyright in United Nations publications" (17 September 1987), para 2:

> "The following categories of material will, as at present, be left in the public domain": (a) Official Records, "a series of printed publications relating to the proceedings of organs or conferences of the United Nations"; (b) United Nations documents, "written material officially issued under a United Nations document symbol".

Para 7 restates that these "will be in the public domain". *Caveat.* The official scan is image-only, so this text was read from a transcription.

The un.org website terms are restrictive: copying is permitted "for the User's personal, non-commercial use, without any right to resell or redistribute them or to compile or create derivative works therefrom". They govern the website, not documents issued under a symbol.

**Bearing on MIT.** Quote from documents bearing a symbol, citing the symbol and ST/AI/189/Add.9/Rev.2 para 2(b) as the basis:

- the UDHR, A/RES/217 A (III);
- the ICCPR, annexed to A/RES/2200 A (XXI);
- HRI/GEN/1/Rev.9 (Vol. I) for General Comment 16;
- the resolutions and reports.

Fetch them from the Official Document System, not by scraping un.org or ohchr.org pages.

### 3.5 OECD

The notice printed in the instrument PDFs reads:

> "This document is provided free of charge. It may be reproduced and distributed free of charge without requiring any further permissions, as long as it is not altered in any way. It may not be sold."

The Compendium's footer reads:

> "The present Compendium of OECD Legal Instruments is owned by the OECD. … shall not be reproduced in whole or significant part without express permission from the OECD."

The OECD's CC BY 4.0 default for content published from 1 July 2024 has not been confirmed to extend to legal instruments (**UNVERIFIED**: oecd.org refuses scripts).

**Bearing on MIT.** "Not altered" and "may not be sold" are incompatible with MIT. Keep OECD text verbatim in a directory under its own notice, and do not mirror the Compendium.

### 3.6 APEC, the Global CBPR Forum and Regional Bodies

| Body | Notice | Bearing |
|---|---|---|
| APEC | `https://www.apec.org/termsofuse`: permission "provided that the use is for non-profit making purposes; … users do not modify the materials … Users also may not … 'mirror' any material contained on this website on any other server without expressed permission." | Short quotation only, or ask permission |
| Global CBPR Forum | Only "© 2024 Global Cross-Border Privacy Rules Forum"; no reuse terms | Short quotation only, or ask permission |
| African Union | `https://au.int/en/legal_notice`: "All rights reserved. … cannot be used, transmitted or reproduced in whole or part … without permission in writing" | Encode by reference, or ask permission |
| ASEAN (Model Contractual Clauses) | "may not be reproduced, republished or transmitted in any form or by any means, in whole or in part, without written permission" | Encode by reference, or ask permission |
| ITU (SADC Model Law) | "All rights reserved. No part of this publication may be reproduced … without the prior written permission of ITU." | Encode by reference |
| RIPD (Ibero-American Standards) | No statement found | Short quotation, or ask permission |
| ECOWAS | Not checked | Unknown |

*Remark.* Berne Convention Art 2(4) leaves the protection of official legislative, administrative and legal texts to each country's law. That may free some of these texts in some jurisdictions. This survey did not verify any national rule and does not rely on it.

### 3.7 Summary of Reuse Terms

| Source | Verbatim in the repository | Commercial reuse | Conditions | Permission rests on |
|---|---|---|---|---|
| EU legislation (EUR-Lex/Cellar) | Yes | Yes | Credit; do not distort; not authentic | EUR-Lex copyright notice; Decision 2011/833/EU Arts 4, 6 |
| EUR-Lex consolidated texts and summaries | Yes | Yes | CC BY 4.0: credit, indicate changes | EUR-Lex copyright notice |
| EUR-Lex metadata | Yes | Yes | None | CC0 1.0 (EUR-Lex notice) |
| EDPB guidance | Yes | Yes | Credit; do not distort; no liability | edpb.europa.eu/copyright_en |
| CJEU and General Court judgments | Yes | Yes | Credit; state not authentic and free of charge | curia.europa.eu Copyright page and disclaimer |
| CoE treaties (ECHR, ETS 108, ETS 181, CETS 223) | Yes | Yes (public domain) | Stated credit line | coe.int licensing page, "Official texts" |
| CoE explanatory reports | Excerpts under 500 words | Not stated | Credit | coe.int licensing page |
| ECtHR judgments (HUDOC) | Only for information or education, free of charge | **No**, without written permission | © ECHR-CEDH | echr.coe.int copyright and disclaimer |
| UN symbol documents (UDHR, ICCPR, GC 16, resolutions, reports) | Yes | Yes (public domain) | Cite symbol | ST/AI/189/Add.9/Rev.2 para 2(b) |
| OECD instruments | Unaltered only | **No sale** | Not altered; free of charge | Notice in the instrument PDFs |
| APEC Privacy Framework | Short quotation | **Non-profit only** | No modification, no mirroring | apec.org terms of use |
| AU, ASEAN, ITU/SADC | By reference | No | All rights reserved | Their notices |

### 3.8 Recommended Practice

1. Keep verbatim official text in data files apart from code. Mark each file with:
   - a `source` (CELEX, ECLI, ETS/CETS number, UN symbol, OECD/LEGAL number);
   - a `notice` naming the permission and its exact location;
   - a `credit` line in the form the publisher asks.
2. Add a `NOTICE` file per publisher, quoting its terms, and state in the repository's licence section that the MIT grant does not extend to these texts.
3. **Before publishing:** write to the ECtHR, using its form, and to the OECD and APEC, for permission to hold their texts in a public repository. Until then, hold ECtHR paragraphs, OECD paragraphs and APEC paragraphs only as needed, in their own directories.
4. Record each fetch as a holding: the bytes' SHA-256, the URL and the date, using archeion's holdings. A quoted clause can then always be traced to the exact text it was taken from.

---

## 4. Machine-Readable Access to the Sources

### 4.1 EUR-Lex and Cellar

#### 4.1.1 What Blocks Scripts

EUR-Lex HTML pages, such as `https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX:32016R0679` and `https://eur-lex.europa.eu/eli/reg/2016/679/oj`, refuse automated clients. They answer `HTTP 202` with an empty body and the header `x-amzn-waf-action: challenge`. This holds for the legal notice and help pages too. The links remain right for people to click.

#### 4.1.2 Content Negotiation by CELEX (Verified)

The Cellar resolves `http://publications.europa.eu/resource/celex/{CELEX}` by content negotiation, with no special user agent needed. Tested requests:

- **XHTML, one file:**
  `curl -L -H "Accept: application/xhtml+xml" -H "Accept-Language: eng" http://publications.europa.eu/resource/celex/32016R0679`
- **The Formex 4 package, a zip of the act and its document descriptor:**
  `curl -L -H "Accept: application/zip;mtype=fmx4" -H "Accept-Language: eng" http://publications.europa.eu/resource/celex/32016R0679`
- **A list of the Formex content streams, with file names:**
  `curl -L -H "Accept: application/list;mtype=fmx4" -H "Accept-Language: eng" http://publications.europa.eu/resource/celex/32016R0679`
  For the GDPR this lists `L_2016119EN.01000101.doc.xml` (the descriptor, `DOC_1`) and `L_2016119EN.01000101.xml` (the act, `DOC_2`). For Decision 2021/914 it lists seven streams, the Annex among them.
- **Encoding.** CELEX numbers with `/` or brackets must be URL-encoded: `12012P%2FTXT`, `32016R0679R%2802%29`.
- **Judgments.** Use `62018CJ0311` with `Accept: application/xhtml+xml`. With `Accept: text/html` the request returns 404.
- **Proposals.** A COM proposal returns `HTTP 300` with a list of `DOC_n` files.
- **Newer acts.** Acts published since the 2023 act-by-act Official Journal ("L series") come as Formex and Formex-derived XHTML.

**Exact item URLs of the three acts the first scheme needs** (listed on 15 November 2026 with `Accept: application/list;mtype=fmx4`; all are fetchable by plain `curl`):

- **GDPR, 32016R0679**:
  - the act: `http://publications.europa.eu/resource/cellar/3e485e15-11bd-11e6-ba9a-01aa75ed71a1.0006.02/DOC_2` (`L_2016119EN.01000101.xml`; the article structure is shown in §4.1.4);
  - the descriptor: `…/DOC_1` (`L_2016119EN.01000101.doc.xml`).
- **SCCs, 32021D0914**, under `http://publications.europa.eu/resource/cellar/55862dbf-c72b-11eb-a925-01aa75ed71a1.0006.02/`:
  - `DOC_1` is the descriptor;
  - `DOC_2` is the Decision (`L_2021199EN.01003101.xml`);
  - `DOC_3` is the Annex holding the clauses (`L_2021199EN.01003701.xml`, `GR.SEQ` structure, §4.1.4);
  - `DOC_4` to `DOC_7` are the remaining parts of the Annex (`…01005701`, `…01005801`, `…01006001`, `…01006101`).
- **DPF, 32023D1795**: the Formex is one zip, `http://publications.europa.eu/resource/cellar/abb0ba05-5750-11ee-9220-01aa75ed71a1.0006.02/DOC_1` (`JOL_2023_231_R_0005_ENG.fmx4.zip`). Its members can also be fetched one by one as `…/DOC_1/{file}`:
  - the Decision is `L_2023231EN.01011801.xml`;
  - the annexes are `L_2023231EN.01017101.xml`, `…01019201.xml`, `…01019601.xml`, `…01019801.xml`, `…01020701.xml`, `…01021201.xml`, `…01021601.xml`, `…01022001.xml`, `…01022401.xml` and `…01022801.xml`;
  - several `.tif` images sit among the annexes' files (`…01019602.tif`, `…01019701.tif` and others). They are probably the US letters' letterheads and signatures. That is **UNVERIFIED**: the images were not opened.
  - The XHTML of the same act (`Accept: application/xhtml+xml`) is the simpler source of its text.

#### 4.1.3 The SPARQL Endpoint and an Example Query (Verified)

The endpoint is `https://publications.europa.eu/webapi/rdf/sparql`, open, with results in JSON or CSV. The ontology is the Common Data Model (CDM). Its chain runs:

- work → expression (a language) → manifestation (a format) → item (a file);
- the CELEX number is the literal `cdm:resource_legal_id_celex`.

**Articles are not Cellar resources.** A CELEX number's articles are reached in two steps: find its Formex items by SPARQL, then parse the Formex `ARTICLE` elements (§4.1.4).

The following query was run on 15 November 2026 and returned the Formex items of the three acts named:

```sparql
PREFIX cdm: <http://publications.europa.eu/ontology/cdm#>
PREFIX cmr: <http://publications.europa.eu/ontology/cdm/cmr#>
PREFIX owl: <http://www.w3.org/2002/07/owl#>
SELECT ?celex ?manif ?item ?mime ?file WHERE {
  VALUES ?celex { "32016R0679" "32021D0914" "32025D1382" }
  ?work cdm:resource_legal_id_celex ?c . FILTER(str(?c) = ?celex)
  ?expr cdm:expression_belongs_to_work ?work ;
        cdm:expression_uses_language
          <http://publications.europa.eu/resource/authority/language/ENG> .
  ?manif cdm:manifestation_manifests_expression ?expr ;
         cdm:manifestation_type ?type .
  FILTER(str(?type) = "fmx4")            # or "xhtml", "pdfa1a", "pdfa2a"
  { ?manif cdm:manifestation_has_item ?item }
  UNION { ?item cdm:item_belongs_to_manifestation ?manif }
  OPTIONAL { ?item cmr:manifestationMimeType ?mime }
  OPTIONAL { ?item owl:sameAs ?file .
             FILTER(STRSTARTS(STR(?file), "http://publications.europa.eu/resource/oj/")) }
}
```

*Caveat.* Three details matter:

- Compare literals with `str()`. Typed literals (`"fmx4"^^xsd:string`) match some acts and not others.
- Use the `UNION`, because items are linked in both directions in some acts and only one in others.
- Each item URI (`…/cellar/{uuid}.0006.02/DOC_2`) is fetched directly, and `owl:sameAs` gives its OJ file name.

The manifestations held in English for the acts and judgment checked:

| CELEX | Manifestation types |
|---|---|
| 32016R0679 (GDPR) | fmx4, pdfa1a, xhtml |
| 02016R0679-20160504 (consolidated) | fmx4, pdfa1a, xhtml |
| 12012P/TXT (Charter) | fmx4, pdfa1a, print, xhtml |
| 32021D0914 (SCCs) | fmx4, pdfa2a, xhtml |
| 32023D1795 (DPF) | fmx4, pdfa2a, xhtml |
| 32025D1382 (EPO adequacy) | fmx4, pdfa2a, xhtml |
| 32026D0179 (Brazil adequacy) | fmx4, pdfa2a, xhtml |
| 32025R2518 (GDPR procedural rules) | fmx4, pdfa2a, xhtml |
| 62018CJ0311 (*Schrems II*) | fmx4, html, pdfa1a, xhtml, xml |

#### 4.1.4 Formex 4: The Structure That Clause-Level Encoding Needs

The GDPR's Formex (schema `formex-05.55-20141201`) was inspected element by element. Article 49 shows the pattern:

```
<ARTICLE IDENTIFIER="049">
  <TI.ART>Article 49</TI.ART><STI.ART>Derogations for specific situations</STI.ART>
  <PARAG IDENTIFIER="049.001"><NO.PARAG>1.</NO.PARAG>
    <ALINEA><P>In the absence of an adequacy decision …:</P>
      <LIST TYPE="alpha"><ITEM><NP><NO.P>(a)</NO.P><TXT>the data subject has explicitly consented …</TXT></NP></ITEM> …</LIST>
    </ALINEA>
    <ALINEA>Where a transfer could not be based on a provision in Article 45 or 46, …</ALINEA>
```

The elements map onto legal units as follows:

| Formex element | Legal unit | Has an identifier |
|---|---|---|
| `DIVISION` with `TITLE/TI` and `STI` | Chapter or section | No |
| `ARTICLE IDENTIFIER="049"` | Article | Yes |
| `PARAG IDENTIFIER="049.001"` | Paragraph | Yes |
| `ALINEA` | Subparagraph | No: count them |
| `LIST TYPE="alpha"`, `ITEM/NP/NO.P` "(a)" | Point | No: read `NO.P` |
| `CONSID/NP/NO.P` "(26)" | Recital | No: read `NO.P` |

The GDPR holds 99 `ARTICLE`, 372 `PARAG`, 397 `ALINEA` and 173 `CONSID` elements. Subparagraphs and points carry no identifiers, so the encoder must derive their addresses by position. The GDPR itself cites them that way ("point (f) of the first subparagraph", Art 6(1); "the second subparagraph", Art 49(1)).

**Annexes are structured differently.** In Decision 2021/914 the SCCs are an `ANNEX` of nested `GR.SEQ LEVEL="n"` groups:

- clause titles are `TITLE/TI` (for example "8.6 *Security of processing*");
- module variants appear as headed groups inside a clause, and options in square brackets in the text;
- there are no `ARTICLE` elements.

A parser of the SCCs must therefore walk `GR.SEQ` and treat a module as a filter over clauses.

#### 4.1.5 XHTML and ELI Subdivision Identifiers

The GDPR's XHTML carries ELI subdivision identifiers:

- `rct_1` … `rct_173`;
- `cpt_V`, and `cpt_III.sct_2` for a section;
- `art_44`, and `art_44.tit_1` for an article's title.

It carries none below the article. Some newer acts add paragraph identifiers (Decision 2021/914: `001.001`, `004.002`); its Annex carries none.

ELI URIs for articles exist and resolve to EUR-Lex. Both `http://data.europa.eu/eli/reg/2016/679/art_49/oj` and `http://data.europa.eu/eli/reg/2016/679/oj` were followed by 301 and 307 redirects to EUR-Lex; whether EUR-Lex shows the article alone could not be tested by script. They serve as stable citation URIs, not as a source of text.

#### 4.1.6 Akoma Ntoso

None of the nine CELEX numbers above has an Akoma Ntoso manifestation. A search of Cellar for manifestation types containing "akn" returned only resources typed `SCHEMA_AKN4EU`, the schema of AKN4EU, the EU's Akoma Ntoso profile used in drafting. Formex 4 is therefore the structured source.

#### 4.1.7 Other EUR-Lex Channels

- EUR-Lex offers a registered SOAP web service and RSS. These were not tested here (**UNVERIFIED**); Cellar suffices.
- Cellar reuse questions go to `op-copyright@publications.europa.eu` (Publications Office copyright page).

### 4.2 CURIA and InfoCuria

- `https://curia.europa.eu/juris/document/document.jsf?docid=…` now redirects (301) to the new InfoCuria (`infocuria.curia.europa.eu`, launched January 2026). It is a JavaScript application with no documented interface.
- The **legacy host still answers scripts:**
  - `https://juris.curia.europa.eu/juris/document/document.jsf?docid={docid}&doclang=EN` gives static XHTML with the ECLI, formation, date, numbered paragraphs and operative part. It was verified for every judgment in §10, *Latombe* included.
  - `https://juris.curia.europa.eu/juris/liste.jsf?num=C-413/23&language=EN` lists a case's documents.
  - *Caveat.* No notice says how long the legacy host will remain. Prefer Cellar (§4.1.2) for the text, and take the ECLI from CURIA or from Cellar's metadata, since the Cellar XHTML of judgments has no ECLI header.
- **Structure.** A judgment has numbered paragraphs, then "On those grounds, the Court (…) hereby rules:" and the numbered points of the operative part.

### 4.3 HUDOC

The Court publishes no documented API. Its front-end endpoints answer scripts:

- **Search, returning JSON** with `itemid`, `ecli`, `kpdate` and `conclusion`:
  `https://hudoc.echr.coe.int/app/query/results?query=contentsitename:ECHR AND appno:"58170/13" AND languageisocode:ENG AND documentcollectionid2:"GRANDCHAMBER"&select=itemid,docname,ecli,kpdate,doctype,conclusion&sort=&start=0&length=10`
- **Text** as HTML: `https://hudoc.echr.coe.int/app/conversion/docx/html/body?library=ECHR&id={itemid}`. DOCX and PDF come from `…/app/conversion/docx/?library=ECHR&id=…&filename=x.docx` and `…/app/conversion/pdf/?library=ECHR&id=…&filename=x.pdf`.
- **Permalink:** `https://hudoc.echr.coe.int/eng?i={itemid}`.
- **Structure.** Paragraphs are numbered (cited "§ 361"). The operative provisions follow "FOR THESE REASONS, THE COURT" as numbered points beginning "Holds".
- **ECHR-OD** (`https://echr-opendata.cs.put.poznan.pl`) is an academic, unofficial corpus, last built 1 March 2024.
- *Caveat.* The reuse terms (§3.3.2) restrict what may be done with the text.

### 4.4 United Nations Treaty Collection and the Official Document System

- **ICCPR status page:** `https://treaties.un.org/Pages/ViewDetails.aspx?src=TREATY&mtdsg_no=IV-4&chapter=4&clang=_en`. It fetches by script (3.3 MB HTML). It showed "STATUS AS AT : 15-11-2026 04:15:26 EDT", Signatories 74, Parties 175.
- **XML export:** `https://treaties.un.org/doc/Publication/MTDSG/Volume%20I/Chapter%20IV/IV-4.en.xml`. It is encoded ISO-8859-1 but served as `text/html`.
  - It holds `<Status><Signatories>74</Signatories><Parties>175</Parties>`.
  - It holds a `<Participants><Table>` of rows (participant, signature, ratification or accession, with "a" for accession).
  - Declarations and reservations follow.
  - A PDF of the same chapter is at `…/IV-4.en.pdf`.
  - The same pattern serves any treaty by its chapter and number (Optional Protocol: IV-5).
- **UN documents by symbol:** `https://documents.un.org/api/symbol/access?s={SYMBOL}&l=en&t=pdf`. It fetches by script and yields PDFs with a text layer, for example HRI/GEN/1/Rev.9(Vol.I), A/RES/79/175 and A/HRC/60/45.
- `ohchr.org` HTML pages return 403 to scripts. The OHCHR PDF of the Covenant (`https://www.ohchr.org/sites/default/files/ccpr.pdf`) downloads.

### 4.5 Council of Europe Treaty Office

- **Treaty texts and explanatory reports** are PDFs on `rm.coe.int` and download by script (§6).
- **Charts of signatures and ratifications**, `https://www.coe.int/en/web/conventions/full-list?module=signatures-by-treaty&treatynum={n}`, return a Cloudflare challenge (403) to scripts.
  - The chart is drawn in the browser from `https://conventions-ws.coe.int/WS_LFRConventions/api/…` (for example `api/signatures?numSTE=…`), which answers 401 without the page's token.
  - It is not a public interface.
- **Recommendation:** record dated manual snapshots of the charts for ETS 5, 108, 181 and CETS 223, as holdings, or ask the Treaty Office for a data feed. The Committee of Convention 108's plenary reports on `rm.coe.int` (fetchable) state the count of 108+ ratifications at each plenary.

### 4.6 OECD Compendium of Legal Instruments

- **Instrument record, JSON:** `https://legalinstruments.oecd.org/api/instruments/OECD-LEGAL-0188?lang=en`, the Compendium's own undocumented interface. It returns dates, adherence, `statusSummary.adherents` and a pointer to the HTML body.
- **Country names:** `https://legalinstruments.oecd.org/api/lists/countries?lang=en`.
- **PDFs:** `https://legalinstruments.oecd.org/public/doc/114/114.en.pdf` for the Guidelines and `…/public/doc/693/693.en.pdf` for the Declaration.
- `oecd.org` itself returns 403.

### 4.7 Other Bodies

These PDFs fetch by script:

- APEC Privacy Framework: `https://www.apec.org/apecapi/publication/getfile?publicationId=42d9fa81-f683-46a8-858b-1cde61fdb8f8`
- Global CBPR documents (`globalcbpr.org/wp-content/uploads/…`)
- the AU status list (§11.1)
- the ASEAN Model Contractual Clauses
- the RIPD Standards
- the ECOWAS communiqué

The AU's English text of the Malabo Convention is image-only and needs OCR.

### 4.8 Summary of Access

| Source | Scriptable | Best route | Structured form |
|---|---|---|---|
| EU acts | Yes | Cellar by CELEX, or SPARQL | Formex 4 XML; XHTML with ELI ids to article level |
| EU judgments | Yes | Cellar (`62018CJ0311`); CURIA legacy host for the ECLI | XHTML; Formex; XML |
| EUR-Lex HTML | No (WAF) | Links for people | — |
| ECtHR judgments | Yes (undocumented) | HUDOC `app/query` and `app/conversion` | JSON metadata; HTML/DOCX/PDF text |
| UN treaty status | Yes | UNTC MTDSG XML | XML table |
| UN documents | Yes | ODS by symbol | PDF with text layer |
| CoE treaties | Yes | rm.coe.int PDFs | PDF |
| CoE charts | No | Manual snapshot | — |
| OECD instruments | Yes (undocumented) | Compendium API | JSON + HTML body |
| APEC, CBPR, ASEAN, RIPD, AU | Yes | PDFs | PDF (AU text image-only) |

---

## 5. United Nations Instruments

### 5.1 Universal Declaration of Human Rights, Article 12

GA resolution 217 A (III), 10 December 1948. Source: `https://www.un.org/en/about-us/universal-declaration-of-human-rights`.

> "No one shall be subjected to arbitrary interference with his privacy, family, home or correspondence, nor to attacks upon his honour and reputation. Everyone has the right to the protection of the law against such interference or attacks."

The Declaration is not a treaty. In a scheme it is a source of principle, not a binding rule.

### 5.2 International Covenant on Civil and Political Rights, Article 17

The Covenant was adopted by GA resolution 2200 A (XXI) of 16 December 1966 and entered into force on 23 March 1976 (UNTS vol. 999, p. 171, registration no. 14668). Source: OHCHR PDF, `https://www.ohchr.org/sites/default/files/ccpr.pdf`.

> "1. No one shall be subjected to arbitrary or unlawful interference with his privacy, family, home or correspondence, nor to unlawful attacks on his honour and reputation.
> 2. Everyone has the right to the protection of the law against such interference or attacks."

**Status.** **175 parties and 74 signatories**, as at 15 November 2026 (UNTC; fetch as in §4.4).

**Structure.** Article and paragraph. Reservations and declarations are kept per party in the UNTC XML; a scheme must read them per state.

### 5.3 Human Rights Committee, General Comment No. 16 (1988)

"General comment No. 16: Article 17 (Right to privacy)", thirty-second session (1988), in HRI/GEN/1/Rev.9 (Vol. I), pp. 191–193.

- Official PDF: `https://documents.un.org/api/symbol/access?s=HRI/GEN/1/Rev.9(Vol.I)&l=en&t=pdf`
- Date of adoption 8 April 1988: Refworld only, **UNVERIFIED** from an official page (the compilation gives "(1988)").
- Eleven numbered paragraphs.

Quoted passages:

- **Para 3:** "The term "unlawful" means that no interference can take place except in cases envisaged by the law. Interference authorized by States can only take place on the basis of law, which itself must comply with the provisions, aims and objectives of the Covenant."
- **Para 4:** "…the expression "arbitrary interference" can also extend to interference provided for under the law. The introduction of the concept of arbitrariness is intended to guarantee that even interference provided for by law should be in accordance with the provisions, aims and objectives of the Covenant and should be, in any event, reasonable in the particular circumstances."
- **Para 10:** "The gathering and holding of personal information on computers, data banks and other devices, whether by public authorities or private individuals or bodies, must be regulated by law. Effective measures have to be taken by States to ensure that information concerning a person's private life does not reach the hands of persons who are not authorized by law to receive, process and use it, and is never used for purposes incompatible with the Covenant. …"

**No newer general comment on Article 17** was found. The Committee's current drafting work is General Comment No. 38, on Article 22. That no Article 17 draft exists is **UNVERIFIED** (search only).

### 5.4 The Right to Privacy in the Digital Age

These documents were verified from their first pages, fetched by symbol:

- **A/RES/79/175**, "The right to privacy in the digital age", adopted without a vote on 17 December 2024. Para 17 "Decides to continue its consideration of the question at its eighty-first session". A resolution of late 2026 is therefore expected; one adopted is **UNVERIFIED**.
- **A/HRC/RES/54/21**, Human Rights Council, 12 October 2023. This is the latest Council resolution found; none from session 60 was found (**UNVERIFIED** absence).
- **OHCHR reports, each titled "The right to privacy in the digital age":**
  - A/HRC/39/29 (3 August 2018);
  - A/HRC/48/31 (13 September 2021);
  - A/HRC/51/17 (4 August 2022);
  - A/HRC/60/45 (29 August 2025, on discrimination in data processing).
- **The Special Rapporteur on the right to privacy** is Ana Brian Nougrères. The mandate was renewed on 3 April 2024; the symbol A/HRC/RES/55/3 is **UNVERIFIED**. Her report of March 2026 on cross-border data collection is known from secondary sources only (**UNVERIFIED**).

*Remark.* The resolutions and reports are interpretive material, not binding rules. In a scheme they support a reading of "arbitrary" in Art 17, as General Comment 16 does with more authority.

---

## 6. Council of Europe Instruments

### 6.1 European Convention on Human Rights, Article 8

Official text: `https://www.echr.coe.int/documents/d/echr/Convention_ENG`, as amended by Protocols 11, 14 and 15. English and French are authentic.

> "1. Everyone has the right to respect for his private and family life, his home and his correspondence.
> 2. There shall be no interference by a public authority with the exercise of this right except such as is in accordance with the law and is necessary in a democratic society in the interests of national security, public safety or the economic well-being of the country, for the prevention of disorder or crime, for the protection of health or morals, or for the protection of the rights and freedoms of others."

Its readings that matter for transfers are in §10.3. Through Charter Art 52(3), its meaning and scope are the floor of Charter Art 7.

### 6.2 Convention 108 and Its Additional Protocol (the Texts in Force)

| Instrument | Date | Official text |
|---|---|---|
| Convention for the Protection of Individuals with regard to Automatic Processing of Personal Data, ETS No. 108 | 28 January 1981 | `https://rm.coe.int/1680078b37` |
| Additional Protocol regarding supervisory authorities and transborder data flows, ETS No. 181 | 8 November 2001 | `https://rm.coe.int/1680080626` |

- **ETS 108 Art 12(2):** "A Party shall not, for the sole purpose of the protection of privacy, prohibit or subject to special authorisation transborder flows of personal data going to the territory of another Party."
- **ETS 181 Art 2(1):** "Each Party shall provide for the transfer of personal data to a recipient that is subject to the jurisdiction of a State or organisation that is not Party to the Convention only if that State or organisation ensures an adequate level of protection for the intended data transfer."
- **ETS 181 Art 2(2)(b)** allows "safeguards, which can in particular result from contractual clauses, … found adequate by the competent authorities".

**Parties to ETS 108: 55**, per several Council of Europe pages seen through search.

- The pages name the 46 member states and Argentina, Cabo Verde, Morocco, Mauritius, Mexico, Senegal, Tunisia and Uruguay. That makes 54.
- The 55th is presumably the Russian Federation, which remains a party to this convention open to non-members. Exact composition **UNVERIFIED**.
- The party count of ETS 181 is **UNVERIFIED**, because the chart refuses scripts.

### 6.3 Protocol CETS No. 223 (Convention 108+): Its Entry into Force

The Protocol amending the Convention, CETS No. 223, is dated 10 October 2018. Text: `https://rm.coe.int/16808ac918`. Article 37:

> "1. This Protocol shall enter into force on the first day of the month following the expiration of a period of three months after the date on which all Parties to the Convention have expressed their consent to be bound by the Protocol, in accordance with the provisions of paragraph 1 of Article 36.
> 2. In the event this Protocol has not entered into force in accordance with paragraph 1, following the expiry of a period of five years after the date on which it has been opened for signature, the Protocol shall enter into force in respect of those States which have expressed their consent to be bound by it in accordance with paragraph 1, provided that the Protocol has at least thirty-eight Parties. As between the Parties to the Protocol, all provisions of the amended Convention shall have effect immediately upon entry into force."

- Art 37(3) allows declarations of provisional application.
- Art 37(4) repeals ETS 181 from entry into force.

**Status on 15 November 2026: not in force.** There are 46 signatures and 34 ratifications.

- The abridged report of the Committee of Convention 108's 50th plenary, T-PD(2026)50RAPAbr_EN of 11 June 2026, `https://rm.coe.int/t-pd-2026-50rapabr-en/48802c2217`, says: "46 signatures out of which, 34 ratifications. Since the November 2025 plenary, the Republic of Moldova deposited on 15 May 2026 the instrument of ratification".
- The Council of Europe's news item "Republic of Moldova Becomes the 34th State to Ratify the Convention 108+" agrees.
- The para 1 route (every party to ETS 108) has not been met. The para 2 route needs four more ratifications.

*Caveat.* Art 37(2) does not itself say on which day the Protocol takes effect once the thirty-eighth consent is given. Confirm the Treaty Office's reading when the count nears thirty-eight.

### 6.4 Structure and Transfer Article of the Amended Convention

The consolidated "Convention 108+" booklet (June 2018: the Committee of Ministers' decision, the consolidated text and the explanatory report) is at `https://rm.coe.int/convention-108-convention-for-the-protection-of-individuals-with-regar/16808b36f1`.

*Caveat.* The consolidated text is a convenience. The authentic instrument is CETS 223's amending provisions, so encode from the Protocol and check against the consolidation.

The chapters are:

- I, General provisions (Arts 1–3);
- II, Basic principles (Arts 4–13);
- III, Transborder flows (Art 14);
- IV, Supervisory authorities (Art 15);
- V, Co-operation and mutual assistance (Arts 16–21);
- VI, Convention Committee (Arts 22–24);
- VII, Amendments (Art 25);
- VIII, Final clauses (Arts 26–31).

Units nest as article › paragraph "1." › letter "a." › roman "i.".

- **Art 2(a):** "'personal data' means any information relating to an identified or identifiable individual ('data subject')".
- **Art 14(1):** "A Party shall not, for the sole purpose of the protection of personal data, prohibit or subject to special authorisation the transfer of such data to a recipient who is subject to the jurisdiction of another Party to the Convention. Such a Party may, however, do so if there is a real and serious risk that the transfer to another Party, or from that other Party to a non-Party, would lead to circumventing the provisions of the Convention. A Party may also do so, if bound by harmonised rules of protection shared by States belonging to a regional international organisation."
- **Art 14(2):** "When the recipient is subject to the jurisdiction of a State or international organisation which is not Party to this Convention, the transfer of personal data may only take place where an appropriate level of protection based on the provisions of this Convention is secured."
- **Art 14(3):** "An appropriate level of protection can be secured by: a. the law of that State or international organisation, including the applicable international treaties or agreements; or b. ad hoc or approved standardised safeguards provided by legally-binding and enforceable instruments adopted and implemented by the persons involved in the transfer and further processing."
- **Art 14(4)** gives the derogations:
  - (a) "explicit, specific and free consent, after being informed of risks…";
  - (b) specific interests of the data subject;
  - (c) "prevailing legitimate interests, in particular important public interests, are provided for by law and such transfer constitutes a necessary and proportionate measure in a democratic society";
  - (d) freedom of expression.
- Art 14(5)–(6) cover information to, and the powers of, the supervisory authority.

### 6.5 The Explanatory Report

CETS 223 explanatory report: `https://rm.coe.int/cets-223-explanatory-report-to-the-protocol-amending-the-convention-fo/16808ac91a`.

- **Para 6:** "The Committee of Ministers has endorsed the explanatory report. In this respect, the explanatory report forms part of the context in which the meaning of certain terms used in the Convention is to be ascertained (note: ref. Article 31, paragraphs 1 and 2 of the United Nations Vienna Convention on the Law of Treaties)."
- **Paras 102–112** comment on Art 14. Para 106 holds that the circumvention exception "has to be interpreted restrictively", requiring "clear and reliable evidence".

The report is the authoritative source of readings of "appropriate level of protection".

### 6.6 Model Contractual Clauses of the Convention 108 Committee

- **Module 1** (controller to controller) was adopted at the 44th plenary, 14–16 June 2023: `https://www.coe.int/en/web/data-protection/-/model-contractual-clauses-for-the-transfer-of-personal-data`.
- **Module 2** (controller to processor), 7 September 2023, is **UNVERIFIED**.
- The June 2026 plenary records Uruguay as the second party to approve the clauses domestically, and a study of their convergence with other standard clauses.

### 6.7 Status Data and Where to Fetch It

- **Charts:** §4.5 (manual snapshot).
- **Plenary reports:** `rm.coe.int`, T-PD(yyyy)nnRAPAbr.
- **Framework Convention on Artificial Intelligence, CETS 225:** reported ratified by the EU on 15 May 2026. Entry into force is **UNVERIFIED**, and it is of marginal relevance here.

---

## 7. OECD Instruments

### 7.1 The Privacy Guidelines

Recommendation of the Council concerning Guidelines governing the Protection of Privacy and Transborder Flows of Personal Data, OECD/LEGAL/0188. Adopted 23 September 1980 (C(80)58/FINAL), revised 11 July 2013 (C(2013)79). Page: `https://legalinstruments.oecd.org/en/instruments/OECD-LEGAL-0188`. There are **38 adherents**, all OECD Members.

**Structure:**

- Part One, General (paras 1–6);
- Part Two, Basic principles of national application (7–14);
- Part Three, Implementing accountability (15);
- Part Four, International application (16–18);
- Part Five, National implementation (19);
- Part Six, International co-operation and interoperability (20–23).

**Definitions.** Para 1(b): "'personal data' means any information relating to an identified or identifiable individual (data subject)". Para 1(e) defines transborder flows.

**Part Four:**

- **Para 16:** "A data controller remains accountable for personal data under its control without regard to the location of the data."
- **Para 17:** "A Member country should refrain from restricting transborder flows of personal data between itself and another country where (a) the other country substantially observes these Guidelines or (b) sufficient safeguards exist, including effective enforcement mechanisms and appropriate measures put in place by the data controller, to ensure a continuing level of protection consistent with these Guidelines."
- **Para 18:** "Any restrictions to transborder flows of personal data should be proportionate to the risks presented, taking into account the sensitivity of the data, and the purpose and context of the processing."

### 7.2 The Declaration on Government Access to Personal Data Held by Private Sector Entities

OECD/LEGAL/0487, adopted 14 December 2022. It has **39 adherents**: the 38 Members and the European Union.

Its seven principles are:

- I Legal basis;
- II Legitimate aims;
- III Approvals;
- IV Data handling;
- V Transparency;
- VI Oversight;
- VII Redress.

PDF: `https://legalinstruments.oecd.org/public/doc/693/693.en.pdf`.

*Remark.* The Declaration maps closely onto the European Essential Guarantees (§9.7) and the criteria in *Big Brother Watch* (§10.3). A scheme can use it as a cross-check of a third country's law on government access.

---

## 8. The APEC Privacy Framework and the Global CBPR Forum

### 8.1 The APEC Privacy Framework (2015)

- Page: `https://www.apec.org/publications/2017/08/apec-privacy-framework-(2015)`
- Imprint: APEC#217-CT-01.9, © 2017 APEC Secretariat.

**Structure.** Each paragraph is numbered and followed by a commentary:

- Part I, Preamble;
- Part II, Scope;
- Part III, Principles I–IX (Preventing Harm, Notice, Collection Limitation, Uses of Personal Information, Choice, Integrity, Security Safeguards, Access and Correction, Accountability);
- Part IV, Implementation (A, domestic; B, international).

**Definition.** Para 9: "Personal information means any information about an identified or identifiable individual."

**Accountability.** Para 32: "…When personal information is to be transferred to another person or organization, whether domestically or internationally, the personal information controller should obtain the consent of the individual or exercise due diligence and take reasonable steps to ensure that the recipient person or organization will protect the information consistently with these Principles."

### 8.2 The Global CBPR Forum

Site: `https://www.globalcbpr.org`.

- **Declaration** of 21 April 2022, signed by Canada, Japan, the Republic of Korea, the Philippines, Singapore, Chinese Taipei and the United States.
- **Members:** Australia, Canada, Japan, the Republic of Korea, Mexico, the Philippines, Singapore, Chinese Taipei and the United States.
- **Associates:**
  - the United Kingdom (2023);
  - Bermuda and Mauritius (26 August 2024);
  - Nigeria (19 September 2025).
  - The Dubai International Financial Centre was announced in 2024 but does not appear on the current membership page; its status is **UNVERIFIED**.
- **Framework:** `globalcbpr.org/wp-content/uploads/Global-CBPR-Framework-2023.pdf`, with the same four parts and nine principles as APEC's.
- **Certifications.** The Global CBPR and Global PRP systems went live on 2 June 2025 (also METI's press release, `https://www.meti.go.jp/english/press/2025/0602_001.html`). Updated Programme Requirements were announced on 23 March 2026.
  - Certification is "currently available to companies headquartered in Japan, the Republic of Korea, Singapore, Chinese Taipei and the United States".
  - Accountability Agents:
    - United States: VeraSafe, BBB National Programs, NCC Group, Schellman and TRUSTe;
    - Singapore: IMDA;
    - Chinese Taipei: III;
    - Republic of Korea: KISA;
    - Japan: JIPDEC.
- **Transition from APEC CBPR.** APEC CBPR certifications were reportedly carried over automatically, with the two systems running in parallel (**UNVERIFIED**; secondary sources only).

*Remark.* A Global CBPR certification is not an "appropriate safeguard" under GDPR Art 46. The certification of Art 46(2)(f) must be approved under Art 42. In a transfer scheme the Global CBPR is relevant only to transfers governed by the laws of its members.

---

## 9. European Union Law

### 9.1 The GDPR: Identity, Structure, Corrigenda

**Identity.** Regulation (EU) 2016/679, OJ L 119, 4.5.2016, p. 1, CELEX 32016R0679.

- ELI `http://data.europa.eu/eli/reg/2016/679/oj`
- Cellar work `3e485e15-11bd-11e6-ba9a-01aa75ed71a1`

**Structure.** 173 recitals, 99 articles, 11 chapters, with sections in Chapters III, IV, VI and VII. Chapter V (Arts 44–50) holds transfers. Units nest article › paragraph "1." › subparagraph › point "(a)". Art 4's definitions are numbered (1)–(26).

**Corrigendum** 32016R0679R(02), OJ L 127, 23.5.2018, p. 2. It corrects recital 71 and Arts 37, 41, 42, 43, 57, 64, 65, 69 and 70. It touches none of Arts 3–6, 9, 28 or 44–50, nor recitals 26 or 101–116. Corrigenda R(01) and R(03) have no English manifestation; their OJ references are **UNVERIFIED**.

**Consolidated version.** 02016R0679-20160504, the only one: the GDPR has never been amended. Regulation (EU) 2025/2518 does not amend it (§9.8).

### 9.2 Provisions Quoted (Official English Text, Read from Cellar)

**Art 3(1):** "This Regulation applies to the processing of personal data in the context of the activities of an establishment of a controller or a processor in the Union, regardless of whether the processing takes place in the Union or not."

**Art 3(2):** "This Regulation applies to the processing of personal data of data subjects who are in the Union by a controller or processor not established in the Union, where the processing activities are related to: (a) the offering of goods or services, irrespective of whether a payment of the data subject is required, to such data subjects in the Union; or (b) the monitoring of their behaviour as far as their behaviour takes place within the Union."

**Art 4(1):** "'personal data' means any information relating to an identified or identifiable natural person ('data subject'); an identifiable natural person is one who can be identified, directly or indirectly, in particular by reference to an identifier such as a name, an identification number, location data, an online identifier or to one or more factors specific to the physical, physiological, genetic, mental, economic, cultural or social identity of that natural person;"

**Art 5(1)** opens "Personal data shall be:" and lists, each with its label:

- (a) "processed lawfully, fairly and in a transparent manner in relation to the data subject ('lawfulness, fairness and transparency');"
- (b) "… ('purpose limitation');"
- (c) "adequate, relevant and limited to what is necessary in relation to the purposes for which they are processed ('data minimisation');"
- (d) "… ('accuracy');"
- (e) "… ('storage limitation');"
- (f) "… ('integrity and confidentiality')."

Art 5(2) adds "('accountability')".

**Art 6(1)** opens: "Processing shall be lawful only if and to the extent that at least one of the following applies:"

- (a) "the data subject has given consent to the processing of his or her personal data for one or more specific purposes;"
- (b)–(e) contract, legal obligation, vital interests, public task;
- (f) "processing is necessary for the purposes of the legitimate interests pursued by the controller or by a third party, except where such interests are overridden by the interests or fundamental rights and freedoms of the data subject which require protection of personal data, in particular where the data subject is a child."

Second subparagraph: "Point (f) of the first subparagraph shall not apply to processing carried out by public authorities in the performance of their tasks."

**Art 9(1):** "Processing of personal data revealing racial or ethnic origin, political opinions, religious or philosophical beliefs, or trade union membership, and the processing of genetic data, biometric data for the purpose of uniquely identifying a natural person, data concerning health or data concerning a natural person's sex life or sexual orientation shall be prohibited." Art 9(2)(a)–(j) give the exceptions.

**Art 28(1):** "Where processing is to be carried out on behalf of a controller, the controller shall use only processors providing sufficient guarantees to implement appropriate technical and organisational measures in such a manner that processing will meet the requirements of this Regulation and ensure the protection of the rights of the data subject."

**Art 44:** "Any transfer of personal data which are undergoing processing or are intended for processing after transfer to a third country or to an international organisation shall take place only if, subject to the other provisions of this Regulation, the conditions laid down in this Chapter are complied with by the controller and processor, including for onward transfers of personal data from the third country or an international organisation to another third country or to another international organisation. All provisions in this Chapter shall be applied in order to ensure that the level of protection of natural persons guaranteed by this Regulation is not undermined."

**Art 45(1):** "A transfer of personal data to a third country or an international organisation may take place where the Commission has decided that the third country, a territory or one or more specified sectors within that third country, or the international organisation in question ensures an adequate level of protection. Such a transfer shall not require any specific authorisation."

**Art 45(2)** opens: "When assessing the adequacy of the level of protection, the Commission shall, in particular, take account of the following elements:". The elements are:

- (a) the rule of law, legislation, public authorities' access, data-subject rights and redress;
- (b) independent supervisory authorities;
- (c) international commitments.

Art 45(9) keeps the decisions adopted under Directive 95/46 in force until amended, replaced or repealed.

**Art 46(1):** "In the absence of a decision pursuant to Article 45(3), a controller or processor may transfer personal data to a third country or an international organisation only if the controller or processor has provided appropriate safeguards, and on condition that enforceable data subject rights and effective legal remedies for data subjects are available."

**Art 46(2):** "The appropriate safeguards referred to in paragraph 1 may be provided for, without requiring any specific authorisation from a supervisory authority, by:"

- (a) "a legally binding and enforceable instrument between public authorities or bodies;"
- (b) "binding corporate rules in accordance with Article 47;"
- (c) "standard data protection clauses adopted by the Commission in accordance with the examination procedure referred to in Article 93(2);"
- (d) "standard data protection clauses adopted by a supervisory authority and approved by the Commission pursuant to the examination procedure referred to in Article 93(2);"
- (e) "an approved code of conduct pursuant to Article 40 together with binding and enforceable commitments of the controller or processor in the third country to apply the appropriate safeguards, including as regards data subjects' rights; or"
- (f) "an approved certification mechanism pursuant to Article 42 together with binding and enforceable commitments of the controller or processor in the third country to apply the appropriate safeguards, including as regards data subjects' rights."

**Art 49(1)**, first subparagraph, opens: "In the absence of an adequacy decision pursuant to Article 45(3), or of appropriate safeguards pursuant to Article 46, including binding corporate rules, a transfer or a set of transfers of personal data to a third country or an international organisation shall take place only on one of the following conditions:"

- (a) "the data subject has explicitly consented to the proposed transfer, after having been informed of the possible risks of such transfers for the data subject due to the absence of an adequacy decision and appropriate safeguards;"
- (b) "the transfer is necessary for the performance of a contract between the data subject and the controller or the implementation of pre-contractual measures taken at the data subject's request;"
- (c) "the transfer is necessary for the conclusion or performance of a contract concluded in the interest of the data subject between the controller and another natural or legal person;"
- (d) "the transfer is necessary for important reasons of public interest;"
- (e) "the transfer is necessary for the establishment, exercise or defence of legal claims;"
- (f) "the transfer is necessary in order to protect the vital interests of the data subject or of other persons, where the data subject is physically or legally incapable of giving consent;"
- (g) "the transfer is made from a register which according to Union or Member State law is intended to provide information to the public and which is open to consultation either by the public in general or by any person who can demonstrate a legitimate interest, but only to the extent that the conditions laid down by Union or Member State law for consultation are fulfilled in the particular case."

**Art 49(1)**, second subparagraph: "Where a transfer could not be based on a provision in Article 45 or 46, including the provisions on binding corporate rules, and none of the derogations for a specific situation referred to in the first subparagraph of this paragraph is applicable, a transfer to a third country or an international organisation may take place only if the transfer is not repetitive, concerns only a limited number of data subjects, is necessary for the purposes of compelling legitimate interests pursued by the controller which are not overridden by the interests or rights and freedoms of the data subject, and the controller has assessed all the circumstances surrounding the data transfer and has on the basis of that assessment provided suitable safeguards with regard to the protection of personal data. The controller shall inform the supervisory authority of the transfer. The controller shall, in addition to providing the information referred to in Articles 13 and 14, inform the data subject of the transfer and on the compelling legitimate interests pursued."

**Art 49(3)** excludes points (a)–(c) and the second subparagraph from "activities carried out by public authorities in the exercise of their public powers". **Art 49(4):** the public interest "shall be recognised in Union law or in the law of the Member State to which the controller is subject".

**Recital 26**, sentences 2–4: "Personal data which have undergone pseudonymisation, which could be attributed to a natural person by the use of additional information should be considered to be information on an identifiable natural person. To determine whether a natural person is identifiable, account should be taken of all the means reasonably likely to be used, such as singling out, either by the controller or by another person to identify the natural person directly or indirectly. To ascertain whether means are reasonably likely to be used to identify the natural person, account should be taken of all objective factors, such as the costs of and the amount of time required for identification, taking into consideration the available technology at the time of the processing and technological developments."

**Recital 104:** "The third country should offer guarantees ensuring an adequate level of protection essentially equivalent to that ensured within the Union, in particular where personal data are processed in one or several specific sectors."

**Recital 105** names accession to Convention 108 and its Additional Protocol as a factor in assessing adequacy.

**Recital 111**, first sentence: "Provisions should be made for the possibility for transfers in certain circumstances where the data subject has given his or her explicit consent, where the transfer is occasional and necessary in relation to a contract or a legal claim, regardless of whether in a judicial procedure or whether in an administrative or any out-of-court procedure, including procedures before regulatory bodies."

*Remark.* "Occasional" appears in Recital 111, not in Art 49(1)(b), (c) or (e). Its force as a condition of those points rests on the recital and on EDPB Guidelines 2/2018 (§9.7), and no CJEU judgment has fixed it. A scheme must encode it as an open point whose reading is soft law.

### 9.3 Charter of Fundamental Rights, Articles 7, 8 and 52

Charter 2012/C 326/02, OJ C 326, 26.10.2012, p. 391, CELEX 12012P/TXT.

- **Art 7:** "Everyone has the right to respect for his or her private and family life, home and communications."
- **Art 8:**
  - "1. Everyone has the right to the protection of personal data concerning him or her."
  - "2. Such data must be processed fairly for specified purposes and on the basis of the consent of the person concerned or some other legitimate basis laid down by law. Everyone has the right of access to data which has been collected concerning him or her, and the right to have it rectified."
  - "3. Compliance with these rules shall be subject to control by an independent authority."
- **Art 52(1):** "Any limitation on the exercise of the rights and freedoms recognised by this Charter must be provided for by law and respect the essence of those rights and freedoms. Subject to the principle of proportionality, limitations may be made only if they are necessary and genuinely meet objectives of general interest recognised by the Union or the need to protect the rights and freedoms of others."
- **Art 52(3):** "In so far as this Charter contains rights which correspond to rights guaranteed by the Convention for the Protection of Human Rights and Fundamental Freedoms, the meaning and scope of those rights shall be the same as those laid down by the said Convention. This provision shall not prevent Union law providing more extensive protection."

### 9.4 Adequacy Decisions in Force, November 2026

Source of the list: `https://commission.europa.eu/law/law-topic/data-protection/international-dimension-data-protection/adequacy-decisions_en`. Every title, date and OJ reference was read from the act's own text in Cellar.

| Partner, scope | Act | Date | OJ | CELEX |
|---|---|---|---|---|
| Switzerland | Decision 2000/518/EC | 26.7.2000 | L 215, 25.8.2000, p. 1 | 32000D0518 |
| Canada: recipients subject to PIPEDA ("commercial organisations") | Decision 2002/2/EC | 20.12.2001 | L 2, 4.1.2002, p. 13 | 32002D0002 |
| Argentina | Decision 2003/490/EC | 30.6.2003 | L 168, 5.7.2003, p. 19 | 32003D0490 |
| Guernsey | Decision 2003/821/EC | 21.11.2003 | L 308, 25.11.2003, p. 27 | 32003D0821 |
| Isle of Man | Decision 2004/411/EC | 28.4.2004 | L 151, 30.4.2004, p. 48 | 32004D0411 |
| Jersey | Decision 2008/393/EC | 8.5.2008 | L 138, 28.5.2008, p. 21 | 32008D0393 |
| Faroe Islands | Decision 2010/146/EU | 5.3.2010 | L 58, 9.3.2010, p. 17 | 32010D0146 |
| Andorra | Decision 2010/625/EU | 19.10.2010 | L 277, 21.10.2010, p. 27 | 32010D0625 |
| Israel: automated processing | Decision 2011/61/EU | 31.1.2011 | L 27, 1.2.2011, p. 39 | 32011D0061 |
| Uruguay: automated processing | Implementing Decision 2012/484/EU | 21.8.2012 | L 227, 23.8.2012, p. 11 | 32012D0484 |
| New Zealand | Implementing Decision 2013/65/EU | 19.12.2012 | L 28, 30.1.2013, p. 12 | 32013D0065 |
| Japan: business operators under the APPI, with Supplementary Rules (Annex I) | Implementing Decision (EU) 2019/419 | 23.1.2019 | L 76, 19.3.2019, p. 1 | 32019D0419 |
| Republic of Korea: entities subject to PIPA (Annexes I–II) | Implementing Decision (EU) 2022/254 | 17.12.2021 | L 44, 24.2.2022, p. 1 | 32022D0254 |
| United Kingdom: GDPR | Implementing Decision (EU) 2021/1772 | 28.6.2021 | L 360, 11.10.2021, p. 1 | 32021D1772 |
| United Kingdom: Law Enforcement Directive | Implementing Decision (EU) 2021/1773 | 28.6.2021 | L 360, 11.10.2021, p. 69 | 32021D1773 |
| United States: organisations on the "Data Privacy Framework List" | Implementing Decision (EU) 2023/1795 | 10.7.2023 | L 231, 20.9.2023, p. 118 | 32023D1795 |
| European Patent Organisation (first international organisation) | Implementing Decision (EU) 2025/1382 | 15.7.2025 | L series 2025/1382, 18.7.2025 | 32025D1382 |
| Brazil: controllers and processors subject to the LGPD | Implementing Decision (EU) 2026/179 | 26.1.2026 | L series 2026/179, 28.1.2026 | 32026D0179 |

**Amending and renewing acts:**

- **Implementing Decision (EU) 2016/2295** (16.12.2016, OJ L 344, 17.12.2016, p. 83, CELEX 32016D2295) amended the eleven Directive-era decisions after *Schrems I*.
- **United Kingdom, extension:** Implementing Decisions (EU) 2025/1226 (GDPR) and 2025/1225 (LED), both 24.6.2025, CELEX 32025D1226 and 32025D1225, extended both decisions to 27 December 2025.
- **United Kingdom, renewal:** Implementing Decisions (EU) 2025/2574 (GDPR) and 2025/2571 (LED), both 19.12.2025, CELEX 32025D2574 and 32025D2571, renewed them by amendment. Article 4 now reads: "This Decision shall expire on 27 December 2031, unless extended …".

*Remark.* The Official Journal prints 2021/1772's heading as "Implementing Regulation". This is a slip: its CELEX type is D and it is a Decision.

**Reviews:**

- COM(2024) 7 final, 15.1.2024 (CELEX 52024DC0007), reviewed the eleven Directive-era decisions; all remain.
- COM(2024) 451 final, 9.10.2024 (CELEX 52024DC0451), was the first review of the DPF. It said "the Commission considers it appropriate to carry out the next periodic review after three years".
- Japan's first review was published on 4.4.2023.
- Korea's first review concluded on 23.7.2026 with the decision maintained (Commission page).

**Encoding.** Encode each decision's **Article 1**, which states its scope (a sector, a class of recipients, or a list), as the clause that answers "is the destination covered?". For example, the DPF's Art 1 refers to the "Data Privacy Framework List" kept under Section I.3 of Annex I.

### 9.5 The EU–US Data Privacy Framework and *Latombe*

- **Decision.** Implementing Decision (EU) 2023/1795 of 10 July 2023, CELEX 32023D1795. Annexes I–VIII: Annex I holds the DPF Principles; the others hold the US government's representations, among them the ODNI letter and Executive Order 14086's redress.
- ***Latombe v Commission*, T-553/23.** Judgment of 3 September 2025, General Court (Tenth Chamber, Extended Composition), ECLI:EU:T:2025:831, `https://juris.curia.europa.eu/juris/document/document.jsf?docid=303827&doclang=EN`. The language of the case is French; the English is a translation.
  - **Operative part:** "1. Dismisses the action; 2. Orders Mr Philippe Latombe to bear his own costs and to pay those incurred by the European Commission, including in the interlocutory proceedings; 3. Orders Ireland to bear its own costs; 4. Orders the United States of America to bear its own costs."
  - **Para 105:** "there is nothing in the judgment in Schrems II, in particular in paragraph 183 of that judgment or in the expression 'any judicial review', to suggest that the bulk collection of personal data must necessarily be the subject matter of prior authorisation issued by an independent authority. On the contrary, it is apparent from reading that expression in conjunction with paragraphs 186 to 197 of that judgment that the decision authorising bulk collection must be subject, as a minimum, to an ex post facto judicial review."
  - **Para 106** finds that bulk collection under Executive Order 14086 is subject to ex post judicial oversight by the Data Protection Review Court, "whose decisions are final and binding".
  - **Press release No 106/25:** the Court "confirms that, on the date of adoption of the contested decision, the United States of America ensured an adequate level of protection".
- **Appeal C-703/25 P.** Lodged on 31 October 2025; notice in OJ C, C/2025/6610, 22.12.2025 (CELEX 62025CN0703). **Pending**: no opinion, hearing or judgment found as of 15 November 2026. That nothing newer exists is **UNVERIFIED**, as are reports of an intervention by Microsoft.

*Caveat.* *Latombe* tested legality at the date of adoption, 10 July 2023. Later changes in US law (for example to the Privacy and Civil Liberties Oversight Board, or to Executive Order 14086) fall to the Commission's periodic review and to the Court of Justice on appeal. The scheme must flag the DPF reading as "upheld at first instance; under appeal".

### 9.6 Standard Contractual Clauses

**Implementing Decision (EU) 2021/914** of 4 June 2021, OJ L 199, 7.6.2021, p. 31, CELEX 32021D0914, adopted under GDPR Arts 28(7) and 46(2)(c).

- **Art 1(1):** "The standard contractual clauses set out in the Annex are considered to provide appropriate safeguards within the meaning of Article 46(1) and (2)(c) of Regulation (EU) 2016/679 for the transfer by a controller or processor of personal data processed subject to that Regulation (data exporter) to a controller or (sub-)processor whose processing of the data is not subject to that Regulation (data importer)."
- **Art 1(2):** "The standard contractual clauses also set out the rights and obligations of controllers and processors with respect to the matters referred to in Article 28(3) and (4) of Regulation (EU) 2016/679, as regards the transfer of personal data from a controller to a processor, or from a processor to a sub-processor."
- **Recital 7:** "The standard contractual clauses may be used for such transfers only to the extent that the processing by the importer does not fall within the scope of Regulation (EU) 2016/679."
- **Art 4** repealed Decisions 2001/497/EC and 2010/87/EU from 27 September 2021. Old contracts were grandfathered to 27 December 2022.

**Modules:**

- One, "Transfer controller to controller";
- Two, "Transfer controller to processor";
- Three, "Transfer processor to processor";
- Four, "Transfer processor to controller".

**Structure of the Annex:**

- **Section I:** Clause 1 Purpose and scope; 2 Effect and invariability; 3 Third-party beneficiaries; 4 Interpretation; 5 Hierarchy; 6 Description of the transfer(s); 7 Docking clause (optional).
- **Section II:** 8 Data protection safeguards (per module); 9 Use of sub-processors (Modules Two and Three); 10 Data subject rights; 11 Redress; 12 Liability; 13 Supervision.
- **Section III:** 14 "Local laws and practices affecting compliance with the Clauses"; 15 "Obligations of the data importer in case of access by public authorities".
- **Section IV:** 16 Non-compliance and termination; 17 Governing law; 18 Choice of forum and jurisdiction.
- **Appendix:** Annex I (A, parties; B, description of the transfer; C, competent supervisory authority); Annex II (technical and organisational measures; not for Module Four); Annex III (sub-processors; Modules Two and Three).

**Clause 14(a):** "The Parties warrant that they have no reason to believe that the laws and practices in the third country of destination applicable to the processing of the personal data by the data importer, including any requirements to disclose personal data or measures authorising access by public authorities, prevent the data importer from fulfilling its obligations under these Clauses. This is based on the understanding that laws and practices that respect the essence of the fundamental rights and freedoms and do not exceed what is necessary and proportionate in a democratic society to safeguard one of the objectives listed in Article 23(1) of Regulation (EU) 2016/679, are not in contradiction with these Clauses."

- Clause 14(b) lists the factors (i)–(iii).
- Clause 14(d) requires the assessment to be documented.
- Clause 14(f) requires suspension where appropriate safeguards cannot be ensured.

**Implementing Decision (EU) 2021/915** of 4 June 2021 holds the controller–processor clauses under Art 28(7), OJ L 199, 7.6.2021, p. 18, CELEX 32021D0915.

**Commission guidance:**

- Q&A of 25 May 2022: `https://commission.europa.eu/law/law-topic/data-protection/international-dimension-data-protection/new-standard-contractual-clauses-questions-and-answers-overview_en`
- The EU–ASEAN Joint Guide (24 May 2023) is linked from the Commission's SCC page.

**Clauses for importers subject to Art 3(2): not adopted.** The Commission's SCC page says it "is in the process of developing additional sets of SCCs", with no date.

### 9.7 EDPB Guidance

| Document | Adopted | URL |
|---|---|---|
| Recommendations 01/2020 on measures that supplement transfer tools | v2.0, 18 June 2021 | `https://www.edpb.europa.eu/our-work-tools/our-documents/recommendations/recommendations-012020-measures-supplement-transfer_en` |
| Recommendations 02/2020 on the European Essential Guarantees for surveillance measures | 10 November 2020 | `https://www.edpb.europa.eu/our-work-tools/our-documents/recommendations/recommendations-022020-european-essential-guarantees_en` |
| Guidelines 2/2018 on derogations of Article 49 | 25 May 2018 | `https://www.edpb.europa.eu/our-work-tools/our-documents/guidelines/guidelines-22018-derogations-article-49-under-regulation_en` |
| Guidelines 05/2021 on the interplay between Article 3 and Chapter V | v2.0, 14 February 2023 | `https://www.edpb.europa.eu/our-work-tools/our-documents/guidelines/guidelines-052021-interplay-between-application-article-3_en` |
| Guidelines 02/2024 on Article 48 | final v2.0, 4 June 2025 (v2.1, 20 June 2025) | `https://www.edpb.europa.eu/our-work-tools/our-documents/guidelines/guidelines-022024-article-48-gdpr_en` |
| Guidelines 01/2025 on pseudonymisation | 16 January 2025, for consultation; final not found (**UNVERIFIED**) | `https://www.edpb.europa.eu/our-work-tools/documents/public-consultations/2025/guidelines-012025-pseudonymisation_en` |
| Guidelines 02/2026 on anonymisation (takes *EDPS v SRB* into account) | v1.0, 7 July 2026; consultation closed 30 October 2026; not final | `https://www.edpb.europa.eu/documents/guideline/guidelines-022026-on-anonymisation_en` |

**Recommendations 01/2020, the six steps:**

1. Know your transfers.
2. Identify the transfer tools you are relying on.
3. Assess whether the Article 46 GDPR transfer tool you are relying on is effective in light of all circumstances of the transfer.
4. Adopt supplementary measures.
5. Procedural steps if you have identified effective supplementary measures.
6. Re-evaluate at appropriate intervals.

**Recommendations 02/2020, the four guarantees:**

- "Guarantee A - Processing should be based on clear, precise and accessible rules";
- "Guarantee B - Necessity and proportionality with regard to the legitimate objectives pursued need to be demonstrated";
- "Guarantee C - Independent oversight mechanism";
- "Guarantee D - Effective remedies need to be available to the individual".

**Guidelines 2/2018:**

- "the derogations must be interpreted restrictively so that the exception does not become the rule."
- On "occasional" and "not repetitive": "These terms indicate that such transfers may happen more than once, but not regularly, and would occur outside the regular course of actions, for example, under random, unknown circumstances and within arbitrary time intervals."
- Under Art 49(1)(b), necessity "requires a close and substantial connection between the data transfer and the purposes of the [contract]".

**Guidelines 05/2021.** A transfer exists where three cumulative criteria are met:

1. the exporter is subject to the GDPR for the processing;
2. it discloses or makes available personal data to another controller or processor;
3. the importer is in a third country, whether or not it is subject to the GDPR under Art 3(2), or is an international organisation.

*Remark.* EDPB recommendations and guidelines are not binding on courts. In a scheme they are readings of soft-law rank, below any CJEU reading of the same term.

### 9.8 Amendments in 2025–2026

**The Digital Omnibus.** COM(2025) 837 final, 19 November 2025, procedure 2025/0360(COD), CELEX 52025PC0837. It would amend Regulations 2016/679, 2018/1724, 2018/1725 and 2023/2854, and Directives 2002/58/EC, 2022/2555 and 2022/2557.

- **The proposed new sentences in Art 4(1):** "Information relating to a natural person is not necessarily personal data for every other person or entity, merely because another entity can identify that natural person. Information shall not be personal for a given entity where that entity cannot identify the natural person to whom the information relates, taking into account the means reasonably likely to be used by that entity. Such information does not become personal for that entity merely because a potential subsequent recipient has means reasonably likely to be used to identify the natural person to whom the information relates."
- **Its other changes:**
  - new definitions in Art 4(32)–(38), among them "scientific research";
  - Art 5(1)(b), on research;
  - Art 9(2)(k) (AI) and (l) (biometric verification under the data subject's sole control), and a new Art 9(5);
  - Arts 12, 13, 22, 33 (breaches: high risk only, 96 hours) and 35;
  - a new Art 41a (implementing acts on when pseudonymised data cease to be personal);
  - new Arts 88a–88c.
- **It does not amend Chapter V, Art 3, Art 6(1) or Art 28.**
- **EDPB–EDPS Joint Opinion 2/2026** (10 February 2026) "strongly urge[s] the co-legislators to not adopt the proposed changes to the definition of personal data".
- **Status on 15 November 2026:**
  - Parliament: ITRE (rapporteur Aura Salla) jointly with LIBE (Marina Kaljurand); draft report PE786.818 of 22 June 2026; "Awaiting committee decision".
  - Council: mandate agreed in Coreper on 21 October 2026. The press release returned 403. Its content, as reported, keeps the present definition of personal data and adds a separate provision on pseudonymisation (**UNVERIFIED**).
  - No trilogue yet.

**Regulation (EU) 2025/2518** of 26 November 2025 "laying down additional procedural rules on the enforcement of Regulation (EU) 2016/679", OJ L series 12.12.2025, CELEX 32025R2518. "This Regulation shall apply from 2 April 2027." It covers cross-border complaints and investigations and does not amend the GDPR.

**Omnibus IV** (relief from Art 30(5) record-keeping for enterprises under 750 employees). COM(2025) 501, 21 May 2025, procedure 2025/0130(COD).

- Trilogue agreement confirmed by Coreper on 26 June 2026; committee approval on 2 July 2026.
- "Awaiting Parliament's position in 1st reading", plenary forecast for 23 November 2026.
- Not adopted.

---

## 10. Case Law

### 10.1 Court of Justice of the European Union

Each judgment's official text: `https://juris.curia.europa.eu/juris/document/document.jsf?docid={docid}&doclang=EN`, and `https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX:{CELEX}` for people.

| Case | Formation, date | ECLI | CELEX | docid |
|---|---|---|---|---|
| *Schrems* (I), C-362/14 | Grand Chamber, 6 October 2015 | ECLI:EU:C:2015:650 | 62014CJ0362 | 169195 |
| *Facebook Ireland and Schrems* (II), C-311/18 | Grand Chamber, 16 July 2020 | ECLI:EU:C:2020:559 | 62018CJ0311 | 228677 |
| *Breyer*, C-582/14 | Second Chamber, 19 October 2016 | ECLI:EU:C:2016:779 | 62014CJ0582 | 184668 |
| *Google Spain and Google*, C-131/12 | Grand Chamber, 13 May 2014 | ECLI:EU:C:2014:317 | 62012CJ0131 | 152065 |
| *Digital Rights Ireland and Others*, C-293/12 and C-594/12 | Grand Chamber, 8 April 2014 | ECLI:EU:C:2014:238 | 62012CJ0293 | 150642 |
| *EDPS v SRB*, C-413/23 P | First Chamber, 4 September 2025 | ECLI:EU:C:2025:645 | 62023CJ0413 | 303863 |
| *Gesamtverband Autoteile-Handel v Scania*, C-319/22 | Third Chamber, 9 November 2023 | ECLI:EU:C:2023:837 | 62022CJ0319 | 279492 |
| *IAB Europe*, C-604/22 | Fourth Chamber, 7 March 2024 | ECLI:EU:C:2024:214 | 62022CJ0604 | 283529 |

**Schrems I (C-362/14).** The adequacy standard is essential equivalence, and generalised access to content compromises the essence of the right.

- Para 73: "…the term 'adequate level of protection' must be understood as requiring the third country in fact to ensure, by reason of its domestic law or its international commitments, a level of protection of fundamental rights and freedoms that is essentially equivalent to that guaranteed within the European Union by virtue of Directive 95/46 read in the light of the Charter."
- Para 94: "…legislation permitting the public authorities to have access on a generalised basis to the content of electronic communications must be regarded as compromising the essence of the fundamental right to respect for private life, as guaranteed by Article 7 of the Charter…"
- Operative part: point 1, an adequacy decision "does not prevent a supervisory authority of a Member State … from examining the claim of a person concerning the protection of his rights and freedoms…"; point 2, "Decision 2000/520 is invalid."

**Schrems II (C-311/18).** Transfers on SCCs must also give essential equivalence, which may require supplementary measures. Decision 2010/87 was valid; the Privacy Shield was invalid.

- Para 105, which is also operative point 2: "…the appropriate safeguards, enforceable rights and effective legal remedies required by those provisions must ensure that data subjects whose personal data are transferred to a third country pursuant to standard data protection clauses are afforded a level of protection essentially equivalent to that guaranteed within the European Union by that regulation, read in the light of the Charter. To that end, the assessment … must, in particular, take into consideration both the contractual clauses agreed … and, as regards any access by the public authorities of that third country to the personal data transferred, the relevant aspects of the legal system of that third country, in particular those set out, in a non-exhaustive manner, in Article 45(2) of that regulation."
- Para 133: SCCs "may require, depending on the prevailing position in a particular third country, the adoption of supplementary measures by the controller in order to ensure compliance with that level of protection."
- Para 134: "It is therefore, above all, for that controller or processor to verify, on a case-by-case basis and, where appropriate, in collaboration with the recipient of the data, whether the law of the third country of destination ensures adequate protection … by providing, where necessary, additional safeguards to those offered by those clauses."
- Para 185: the US limitations "are not circumscribed in a way that satisfies requirements that are essentially equivalent to those required, under EU law, by the second sentence of Article 52(1) of the Charter."
- Para 202: invalidating an adequacy decision "is not liable to create such a legal vacuum" because Art 49 "details the conditions under which transfers … may take place".
- Operative point 1: the GDPR applies to a commercial transfer "irrespective of whether, at the time of that transfer or thereafter, that data is liable to be processed by the authorities of the third country in question for the purposes of public security, defence and State security."
- Point 3: absent a valid adequacy decision, the supervisory authority "is required to suspend or prohibit a transfer … if … those clauses are not or cannot be complied with in that third country and the protection of the data transferred that is required by EU law … cannot be ensured by other means…"
- Point 4: examination of Decision 2010/87/EU "has disclosed nothing to affect the validity of that decision."
- Point 5: "Commission Implementing Decision (EU) 2016/1250 … on the adequacy of the protection provided by the EU-US Privacy Shield is invalid."

**Breyer (C-582/14).** Identifiability is relative to the means lawfully and reasonably available to the holder.

- Operative point 1: "a dynamic IP address registered by an online media services provider when a person accesses a website that the provider makes accessible to the public constitutes personal data within the meaning of that provision, in relation to that provider, where the latter has the legal means which enable it to identify the data subject with additional data which the internet service provider has about that person."
- Para 43: "…it is not required that all the information enabling the identification of the data subject must be in the hands of one person."
- Para 46: there are no means likely reasonably to be used "if the identification of the data subject was prohibited by law or practically impossible on account of the fact that it requires a disproportionate effort in terms of time, cost and man-power, so that the risk of identification appears in reality to be insignificant."

**Google Spain (C-131/12).** The case fixes the reading of "in the context of the activities of an establishment" and the controller status of a search engine.

- Operative point 2: processing "is carried out in the context of the activities of an establishment of the controller on the territory of a Member State … when the operator of a search engine sets up in a Member State a branch or subsidiary which is intended to promote and sell advertising space offered by that engine and which orientates its activity towards the inhabitants of that Member State."
- Point 1: the operator "must be regarded as the 'controller'".
- Point 3: the operator "is obliged to remove from the list of results displayed following a search made on the basis of a person's name links to web pages … even, as the case may be, when its publication in itself on those pages is lawful."

**Digital Rights Ireland (C-293/12 and C-594/12).** Limitations must be strictly necessary and set by clear and precise rules. Retention outside the EU defeats independent control.

- Para 52: "derogations and limitations in relation to the protection of personal data must apply only in so far as is strictly necessary".
- Para 54: "clear and precise rules governing the scope and application of the measure … and imposing minimum safeguards".
- Para 68: "that directive does not require the data in question to be retained within the European Union, with the result that it cannot be held that the control, explicitly required by Article 8(3) of the Charter, by an independent authority of compliance with the requirements of protection and security, as referred to in the two previous paragraphs, is fully ensured."
- Para 69: "the EU legislature has exceeded the limits imposed by compliance with the principle of proportionality in the light of Articles 7, 8 and 52(1) of the Charter."
- Operative part: "Directive 2006/24/EC … is invalid."

**EDPS v SRB (C-413/23 P), interpreting Regulation 2018/1725.** Pseudonymised data are not personal data for every person. For the duty to inform, identifiability is judged from the controller's side at collection.

- Para 86: "pseudonymised data must not be regarded as constituting, in all cases and for every person, personal data …, in so far as pseudonymisation may, depending on the circumstances of the case, effectively prevent persons other than the controller from identifying the data subject in such a way that, for them, the data subject is not or is no longer identifiable."
- Para 77: for the recipient this "presupposes, first, that Deloitte is not in a position to lift those measures during any processing … Second, those measures must in fact be such as to prevent Deloitte from attributing those comments to the data subject including by recourse to other means of identification such as cross-checking with other factors…"
- Para 111: "for the purposes of applying the obligation to provide information … the identifiable nature of the data subject must be assessed at the time of collection of the data and from the point of view of the controller."
- Para 58: personal opinions, "as an expression of a person's thinking, are necessarily closely linked to that person."
- Operative part: sets aside T-557/20 (EU:T:2023:219) and refers the case back. The General Court's ruling on remand is **UNVERIFIED**.

**Scania (C-319/22).** A datum can be personal for one holder and, through it, for another.

- Para 46: a VIN "becomes personal as regards someone who reasonably has means enabling that datum to be associated with a specific person."
- Para 49: the VIN "constitutes personal data for them … and, indirectly, for the vehicle manufacturers making it available, even if the VIN is not, in itself, personal data for them".

**IAB Europe (C-604/22).**

- Operative point 1: the TC String "constitutes personal data … in so far as, where those data may, by reasonable means, be associated with an identifier, such as, inter alia, the IP address of that user's device, they allow the data subject to be identified."
- The organisation's inability to access or combine it "does not preclude that string from constituting personal data".

### 10.2 General Court

- **Latombe, T-553/23:** see §9.5.
- **Bindl v Commission, T-354/22.** Sixth Chamber, Extended Composition, 8 January 2025, ECLI:EU:T:2025:4, CELEX 62022TJ0354, docid 294090. It interprets Regulation 2018/1725.
  - Para 95: a transfer requires the controller to "make the personal data available, by transmission or otherwise, to a recipient" that is "established in a third country".
  - Para 133: "access by the authorities of a third country, under the legislation of that country, to personal data processed in the EEA constitutes a transfer".
  - Para 135: "the mere risk of access … cannot amount to a transfer".
  - Para 188: by the "Sign in with Facebook" hyperlink, the IP address "was transmitted to Meta Platforms, an undertaking established in the United States. That transmission amounts, therefore, to a transfer of personal data to a third country".
  - Damages: EUR 400.
  - **Appeal C-206/25 P, *Commission v Bindl*, is pending.**

### 10.3 European Court of Human Rights

ECLIs were taken from HUDOC's JSON.

| Case | Date | ECLI | HUDOC |
|---|---|---|---|
| *S. and Marper v the United Kingdom* [GC], nos. 30562/04 and 30566/04 | 4 December 2008 | ECLI:CE:ECHR:2008:1204JUD003056204 | `https://hudoc.echr.coe.int/eng?i=001-90051` |
| *Big Brother Watch and Others v the United Kingdom* [GC], nos. 58170/13, 62322/14 and 24960/15 | 25 May 2021 | ECLI:CE:ECHR:2021:0525JUD005817013 | `https://hudoc.echr.coe.int/eng?i=001-210077` |
| *Centrum för rättvisa v Sweden* [GC], no. 35252/08 | 25 May 2021 | ECLI:CE:ECHR:2021:0525JUD003525208 | `https://hudoc.echr.coe.int/eng?i=001-210078` |

**S. and Marper.** Storage alone is an interference. Blanket and indiscriminate retention is not necessary in a democratic society.

- § 67: "The mere storing of data relating to the private life of an individual amounts to an interference within the meaning of Article 8".
- § 103: "The protection of personal data is of fundamental importance to a person's enjoyment of his or her right to respect for private and family life…"
- § 125: "the blanket and indiscriminate nature of the powers of retention … fails to strike a fair balance … and cannot be regarded as necessary in a democratic society."
- Operative point 1, unanimous: "Holds that there has been a violation of Article 8 of the Convention".

**Big Brother Watch.** Bulk interception needs end-to-end safeguards and the eight criteria. Sharing with foreign states requires safeguards against abuse in the receiving state, not identical protection.

- § 350: "end-to-end safeguards", meaning necessity and proportionality assessed "at each stage", "independent authorisation at the outset", and "supervision and independent ex post facto review".
- § 361: the framework must clearly define "(i) the grounds on which bulk interception may be authorised; (ii) the circumstances in which an individual's communications may be intercepted; (iii) the procedure to be followed for granting authorisation; (iv) the procedures to be followed for selecting, examining and using intercept material; (v) the precautions to be taken when communicating the material to other parties; (vi) the limits on the duration of interception, the storage of intercept material and the circumstances in which such material must be erased and destroyed; (vii) the procedures and modalities for supervision by an independent authority …; (viii) the procedures for independent ex post facto review…".
- § 362: "the transferring State must ensure that the receiving State, in handling the data, has in place safeguards capable of preventing abuse and disproportionate interference … This does not necessarily mean that the receiving State must have comparable protection to that of the transferring State…"
- Operative part:
  - violation of Article 8 for the s. 8(4) regime and the Chapter II regime (unanimous);
  - "by twelve votes to five, that there has been no violation of Article 8 … in respect of the receipt of intelligence from foreign intelligence services";
  - violation of Article 10 for the s. 8(4) and Chapter II regimes;
  - no violation of Article 10 for the receipt of intelligence (twelve to five).

**Centrum för rättvisa.**

- § 369 names, among three shortcomings, "the absence of a requirement … that, when making a decision to transmit intelligence material to foreign partners, consideration is given to the privacy interests of individuals".
- § 373: the regime "did not contain sufficient "end-to-end" safeguards".
- Operative part: "Holds, by fifteen votes to two, that there has been a violation of Article 8 of the Convention".

*Remark.* ECtHR § 362's "not necessarily … comparable protection" is a lower threshold than the CJEU's "essentially equivalent". The scheme must keep the two apart: Charter Art 52(3) makes the ECHR a floor, and "Union law providing more extensive protection" is what the CJEU applies.

### 10.4 Pending and Reported Matters

- **C-703/25 P** (*Latombe*, appeal): pending.
- **C-206/25 P** (*Commission v Bindl*): pending.
- **C-804/25**, *Autorité de protection des données*: a pending reference on Art 46(2)(a) and Art 49(1)(d) for international agreements concluded before 2016. CURIA lists the case with no documents.
- **C-585/26**, on processors outside the EU: reported on 9 November 2026 (**UNVERIFIED**).

---

## 11. Regional Instruments Outside Europe

### 11.1 African Union: The Malabo Convention

African Union Convention on Cyber Security and Personal Data Protection, Malabo, 27 June 2014. Treaty page: `https://au.int/en/treaties/african-union-convention-cyber-security-and-personal-data-protection`.

**Status list dated 2 February 2026:** `https://au.int/sites/default/files/AFRICAN%20UNION%20CONVENTION%20ON%20CYBER%20SECURITY%20AND%20PERSONAL%20DATA%20PROTECTION%20%281%29.pdf`.

- It reads "Note of signature : 21 of ratification : 20 of deposit : 20" and "This Convention entered into force on 8th June 2023, after the deposit of the 15th instrument of ratification."
- Recent ratifications include Djibouti (16 February 2025), the Central African Republic (27 June 2025) and the DR Congo (15 September 2025).
- Rows misalign in text extraction, so read the PDF by eye for the full list.

**Text.** The English PDF is image-only. Its structure is therefore **UNVERIFIED** until OCR: Chapter II, personal data protection, is believed to span Arts 8–23, with a transfer rule in Art 14(6).

### 11.2 ECOWAS

- **Supplementary Act A/SA.1/01/10** on Personal Data Protection, 16 February 2010. No official ECOWAS copy was found, so its structure is **UNVERIFIED**.
- **A revised Supplementary Act was adopted on 19 July 2026.** The final communiqué of the 69th Ordinary Session (`https://www.ecowas.int/wp-content/uploads/2026/07/Final-Communique_-ENG.pdf`), para 33, reads: "The Authority adopts the Revised Supplementary Act on the Protection of Personal Data in the ECOWAS Region as recommended by the Council of Ministers". Its text and number are not yet published (**UNVERIFIED**).

### 11.3 ASEAN Model Contractual Clauses for Cross Border Data Flows

- `https://asean.org/wp-content/uploads/3-ASEAN-Model-Contractual-Clauses-for-Cross-Border-Data-Flows_Final.pdf`
- "Final Copy Endorsed by the 2nd ASEAN Digital Senior Officials' Meeting (ADGSOM), January 2021".
- Module 1 is controller to processor; Module 2 is controller to controller.
- The ASEAN–EU Joint Guide to the MCCs and the EU SCCs was launched on 24 May 2023: `https://commission.europa.eu/system/files/2023-05/%28Final%29%20Joint_Guide_to_ASEAN_MCC_and_EU_SCC.pdf`.
- *Caveat.* All rights are reserved (§3.6).

### 11.4 Ibero-American Data Protection Network (RIPD)

- **Standards** for Personal Data Protection for Ibero-American States, 20 June 2017: `https://www.redipd.org/sites/default/files/inline-files/Estandares_Esp_Con_logo_RIPD.pdf`.
- **Chapters:** I to X. Chapter V, international transfers, is paras 36–37.
- **Para 36.1** permits transfers on five grounds:
  - (a) a recognised adequate level;
  - (b) sufficient guarantees;
  - (c) "cláusulas contractuales o cualquier otro instrumento jurídico que ofrezca garantías suficientes";
  - (d) binding self-regulation or certification;
  - (e) authorisation by the supervisory authority.
- **Model clauses.** The RIPD's model contractual clauses have been adopted by Argentina (AAIP Res. 198/2023) and Uruguay (Res. 50/022), per those regulators. The RIPD's own date is **UNVERIFIED**.

### 11.5 Others

- **SADC Model Law on Data Protection.** Adopted by the SADC ministers in November 2012 and published by ITU in 2013: `https://www.itu.int/en/ITU-D/Projects/ITU-EC-ACP/HIPSSA/Documents/FINAL%20DOCUMENTS/FINAL%20DOCS%20ENGLISH/sadc_model_law_data_protection.pdf`.
- **The EAC framework** was not verified.

---

## 12. Other Regimes Relevant to Cross-Border Practice (Leads)

These are named for later schemes. Except where marked, they rest on secondary sources and must be verified before encoding.

- **United Kingdom.** The Data (Use and Access) Act 2025 (2025 c. 18) replaces the transfer regime of the UK GDPR through s. 85 and Schedule 7, "Transfers of personal data to third countries etc: general processing". These references were verified on `https://www.legislation.gov.uk/ukpga/2025/18/contents`.
  - It brings in a "data protection test" of "not materially lower" protection in place of essential equivalence.
  - It was reportedly commenced on 5 February 2026 by Commencement Regulations No. 6 (**UNVERIFIED**).
  - legislation.gov.uk publishes under the Open Government Licence v3.0, which is compatible with MIT (**UNVERIFIED** in this survey).
  - The EU's renewal of UK adequacy in December 2025 post-dates the Act.
- **Brazil.** ANPD Resolution CD/ANPD No. 19/2024 (23 August 2024) holds international transfers and Brazilian SCCs, mandatory from August 2025 (**UNVERIFIED**). The EU–Brazil mutual adequacy dates from 26–27 January 2026 (§9.4).
- **China.** The CAC's Provisions on Promoting and Regulating Cross-Border Data Flows (22 March 2024), the 2023 Standard Contract Measures, and Measures for the Certification of Cross-Border Provision of Personal Information effective 1 January 2026 (**UNVERIFIED**).
- **India, Switzerland (revised FADP), Japan (APPI) and Korea (PIPA):** not verified in this survey.

---

## 13. Structure for Clause-by-Clause Encoding

### 13.1 Units by Source

| Source | Units, in order | Identifiers available |
|---|---|---|
| GDPR, EU acts (Formex) | chapter › section › article › paragraph › subparagraph › point › (indent); recitals | `ARTICLE@IDENTIFIER`, `PARAG@IDENTIFIER`; subparagraphs and points by position and `NO.P` |
| SCC Annex (Formex) | section › clause › sub-clause "8.6" › point "(a)"; module as a filter; options in brackets | `GR.SEQ@LEVEL`, `TI` numbers |
| Charter | title › article › paragraph | ELI ids `art_n` |
| CJEU judgments | paragraph; operative point | numbered paragraphs; "hereby rules" points |
| ECtHR judgments | § paragraph; operative point | numbered §; "Holds" points |
| ECHR, ETS 108, ETS 181, CETS 223 | (chapter) › article › paragraph › letter › roman | printed numbers |
| ICCPR, UDHR | article › paragraph | printed numbers |
| General Comment 16 | paragraph | 1–11 |
| OECD Guidelines | part › paragraph › letter | 1–23 |
| APEC Framework | part › principle › paragraph (with commentary) | printed numbers |

### 13.2 A Proposed Addressing Grammar

The grammar is a path of typed steps under an instrument identifier. Where an ELI subdivision exists, the path coincides with it, so that a path maps to `http://data.europa.eu/eli/reg/2016/679/art_49/oj` at article level.

```
address  ::= instrument "/" step ("/" step)*
step     ::= kind ":" label
kind     ::= "cpt" | "sct" | "art" | "par" | "sub" | "pnt" | "ind" | "rct"
           | "cls" | "mod" | "anx" | "op" | "para"
```

Examples:

- `eu.32016R0679/art:49/par:1/sub:2` is the second subparagraph of Art 49(1).
- `eu.32016R0679/art:49/par:1/sub:1/pnt:a`
- `eu.32016R0679/rct:111`
- `eu.32021D0914/anx:1/cls:14/pnt:a`, with `mod:2` as a qualifier
- `coe.ets223/art:37/par:2`
- `coe.ets108+consolidated/art:14/par:3/pnt:b` is a convenience text, checked against `coe.ets223`.
- `un.iccpr/art:17/par:1`
- `un.HRI/GEN/1/Rev.9(Vol.I)/gc:16/para:10`
- `ecli:EU:C:2020:559/para:105` and `ecli:EU:C:2020:559/op:2`
- `ecli:CE:ECHR:2021:0525JUD005817013/para:361`

### 13.3 The Record of a Clause

Each clause record should hold:

- its address;
- the instrument's official identifier (CELEX, ECLI, ETS/CETS number, UN symbol);
- the official URL;
- the language, and whether that language is authentic;
- the verbatim text;
- its temporal standing: in force from and to, amended by, under challenge (for example `under-appeal: C-703/25 P`);
- the holding it was read from: URL, retrieval date, SHA-256 of the fetched bytes;
- the `notice` and `credit` of §3.8;
- for case-law readings, the term read and the provision it reads.

---

## 14. Proposed First Practice Scheme: Transfers of Personal Data

### 14.1 The Question as a Chain of Determinations

*May this transfer of personal data from A to B proceed, on what ground, with what safeguards, and what does it rest on?*

The scheme answers by a fixed chain. Each link is a determination with the provision that grounds it, and a link may fail, hold, or stand open on an interpretation:

1. **Personal data.** Are the data personal data for the exporter? For the importer?
   - Rests on Art 4(1), Recital 26 and Art 4(5) (pseudonymisation).
   - Fixed readings: *Breyer* op. 1 and paras 43 and 46; *Scania* paras 46 and 49; *IAB Europe* op. 1; *EDPS v SRB* paras 77, 86 and 111.
   - *Caveat.* The proposed Art 4(1) sentences are not law.
   - Special categories go through Art 9(1); a later scheme adds criminal data under Art 10.
2. **Territorial scope.** Does the GDPR apply to the exporter's processing?
   - Art 3(1): "in the context of the activities of an establishment", read in *Google Spain* op. 2.
   - Art 3(2): offering or monitoring.
3. **Transfer.** Is there a transfer?
   - The GDPR does not define it. The three criteria of EDPB Guidelines 05/2021 apply (soft law).
   - *Bindl* paras 95, 133, 135 and 188 give the reading of the General Court, under appeal and on Regulation 2018/1725.
   - If the importer is subject to the GDPR under Art 3(2), there is still a transfer (Guidelines 05/2021), but the SCCs of 2021/914 are not available (Art 1(1), Recital 7). The scheme must report "no Commission clauses for this case yet".
4. **Lawful processing.** Is the underlying processing lawful? Art 44 says "subject to the other provisions of this Regulation", so:
   - a basis under Art 6(1);
   - the Art 5(1) principles (purpose limitation and minimisation first);
   - an exception under Art 9(2) for special categories;
   - an Art 28(3) contract if the importer is a processor (SCC Modules Two and Three satisfy it: 2021/914 Art 1(2)).
5. **A Chapter V ground, in the GDPR's order.**
   1. **Art 45.** Is the destination covered by an adequacy decision in force (the §9.4 table, each decision's Art 1 scope)? For the United States, is the importer on the DPF List?
   2. **Art 46.** Otherwise, are there appropriate safeguards (SCCs with module selection, BCRs, others) **and** "enforceable data subject rights and effective legal remedies"? This calls for:
      - the Clause 14 assessment of the destination's laws and practices against the European Essential Guarantees;
      - supplementary measures where needed (*Schrems II* paras 105, 133 and 134; Recommendations 01/2020 and 02/2020).
   3. **Art 49(1), first subparagraph.** Otherwise, does a derogation apply, read restrictively? Art 49(3) excludes points (a)–(c) for public authorities, and Art 49(4) requires the public interest to be recognised in law.
   4. **Art 49(1), second subparagraph.** Otherwise, the compelling-legitimate-interests residue: not repetitive, a limited number of data subjects, an assessment, suitable safeguards, notice to the supervisory authority and to the data subject.
   5. **Art 48** is a check, not a ground: a foreign order does not of itself authorise a transfer (Guidelines 02/2024).
6. **The frame.** What does the conclusion rest on? A derivation whose premises are the clauses above and whose readings are the judgments, under:
   - Charter Arts 7, 8 and 52(1), with Art 8(3)'s independent control (*Digital Rights Ireland* para 68);
   - through Art 52(3), ECHR Art 8 as read in *S. and Marper* §§ 67 and 125, *Big Brother Watch* §§ 350, 361 and 362, and *Centrum* § 369;
   - for the international layer, ICCPR Art 17 with General Comment 16 paras 3, 4 and 10, and Convention 108 Art 12 with ETS 181 Art 2 (in force), and 108+ Art 14 (signed, not in force).

### 14.2 Layers and the Order of Authority

| Rank | Layer | Binds whom | Role in the scheme |
|---|---|---|---|
| 1 | EU primary law: Charter Arts 7, 8, 52 | Union and Member States implementing EU law | Validity test of every lower rule and reading |
| 2 | CJEU judgments interpreting the Charter and the GDPR | Erga omnes as interpretation; invalidity declarations final | Fixed readings of open terms |
| 3 | GDPR Arts 3–6, 9, 28, 44–49; Recitals 26, 101–116 as aids | Controllers and processors | The rules |
| 4 | Commission implementing decisions: adequacy, SCCs | As acts of Union law, until annulled or declared invalid (only the Court of Justice may declare an act invalid) | Grounds and safeguards |
| 5 | General Court judgments under appeal (*Latombe*, *Bindl*) | Parties; persuasive, provisional | Readings marked provisional |
| 6 | EDPB recommendations and guidelines | Not binding on courts | Soft-law readings; defeasible |
| 7 | ECHR Art 8 and ECtHR case law | Member States as Contracting Parties; floor of the Charter through Art 52(3) | Minimum standard; cross-check |
| 8 | Convention 108 and ETS 181 (in force); CETS 223 (not in force) | States parties | International frame; adequacy factor (Recital 105) |
| 9 | ICCPR Art 17, General Comment 16, UDHR Art 12, UN resolutions | States parties (the ICCPR); principles | Outermost frame |
| 10 | OECD Guidelines and Declaration, APEC Framework, Global CBPR | Soft law among adherents | Comparison; other jurisdictions' schemes |

*Rationale.* The ranking is a declared scheme, in the manner of the law domain's `sources`. Its key steps are fixed by text:

- Art 52(3) makes the ECHR the floor.
- Art 45(9) keeps the Directive-era decisions.
- *Schrems I* reserves invalidity to the Court of Justice (its operative point 1 lets supervisory authorities examine claims; invalidity is the Court's alone).

Ranks 7–9 bind states, not exporters. In a private transfer they enter as standards that the EU rules must meet, and as the source of the readings of "necessary" and "in accordance with the law" for government access in the destination.

### 14.3 Provisions to Encode First

**Tier A: the rules (EU; verbatim from Cellar; credit EUR-Lex).**

- GDPR:
  - Art 3(1)–(2);
  - Art 4, points (1), (2), (5), (7), (8), (9), (11) and (26);
  - Art 5(1)–(2);
  - Art 6(1), both subparagraphs;
  - Art 9(1) and 9(2)(a)–(j);
  - Art 28(1) and 28(3);
  - Art 44;
  - Art 45(1)–(3) and (9);
  - Art 46(1)–(3);
  - Art 48;
  - Art 49(1)–(6).
- GDPR Recitals 26 and 101–116 as interpretive aids, typed `recital`, never as rules.
- Charter Arts 7, 8, 52(1) and 52(3).
- Decision 2021/914:
  - Arts 1–4;
  - Annex Clauses 1–18, encoded per module (Modules One to Four);
  - Clauses 8, 14 and 15 first, then Annex I's fields as the facts the clauses need.
- The eighteen adequacy decisions' Art 1, with Art 4 of 2021/1772 as amended by 2025/2574 (the 2031 expiry).
- Decision 2023/1795 Art 1 and Annex I Section I.3 (the DPF List).

**Tier B: fixed readings (CJEU; credit the Court of Justice).**

- *Schrems I*: para 73, para 94, operative points 1–2.
- *Schrems II*: paras 94, 105, 133, 134, 185 and 202; operative points 1–5.
- *Breyer*: op. 1 and paras 43 and 46.
- *Scania*: paras 46 and 49.
- *IAB Europe*: op. 1.
- *EDPS v SRB*: paras 77, 86 and 111.
- *Google Spain*: op. 1–2.
- *Digital Rights Ireland*: paras 52, 54, 68 and 69.
- **Provisional:** *Latombe* paras 105–106 and its operative part, flagged `under-appeal: C-703/25 P`; *Bindl* paras 95, 133, 135 and 188, flagged `under-appeal: C-206/25 P`.

**Tier C: soft-law readings (EDPB; credit the EDPB).**

- Recommendations 01/2020: the six steps.
- Recommendations 02/2020: Guarantees A–D.
- Guidelines 2/2018: restrictive interpretation; "occasional" and "not repetitive"; the "close and substantial connection" test of necessity.
- Guidelines 05/2021: the three criteria.
- Guidelines 02/2024: Art 48.

**Tier D: the international frame.**

- ECHR Art 8 (public domain, CoE credit).
- *S. and Marper* §§ 67, 103 and 125; *Big Brother Watch* §§ 350, 361 and 362 and its operative points; *Centrum* §§ 369 and 373. Hold these under the ECtHR's notice until permission is given.
- ICCPR Art 17; General Comment 16 paras 3, 4 and 10; UDHR Art 12.
- ETS 108 Art 12; ETS 181 Art 2.
- CETS 223 Art 37, and the amended Convention's Art 2(a) and Art 14, flagged `not-in-force`.
- OECD Guidelines paras 16–18, in a carve-out under the OECD notice.
- APEC para 32, as a short quotation.

### 14.4 Facts the Scheme Needs

**The exporter:**

- its establishments, with whether each is in the Union;
- its role (controller or processor);
- whether it is a public authority exercising public powers (Art 49(3)).

**The importer:**

- its country or international organisation;
- its role;
- whether it is subject to the GDPR under Art 3(2);
- whether it is on the DPF List;
- whether it is covered by another decision's scope (sector or class).

**The data:**

- its categories;
- whether it is pseudonymised, and who holds the key;
- whether it includes special categories;
- the identification means available to each party.

**The transfer:**

- its mode (transmission, remote access, an onward transfer);
- its purpose;
- its frequency and volume (for Art 49: "occasional", "not repetitive", "limited number");
- the number of data subjects.

**Instruments in place:**

- SCCs signed (module, date, the options chosen);
- BCRs (approval);
- codes or certifications;
- consent records (explicit; risks informed).

**The destination's law on public-authority access:**

- legal basis and clarity;
- necessity and proportionality;
- independent oversight;
- remedies.

These are assessed against Guarantees A–D and the BBW § 361 criteria. They are facts with sources, handled through the law domain's `facts` and `exhibits` and graded by tekmerion.

### 14.5 Open Texture and Where the Courts Have Fixed a Reading

| Term (where) | Open | Fixed reading | Authority of the reading |
|---|---|---|---|
| "personal data", "identifiable" (Art 4(1); Rec 26) | Yes | Relative to the holder's means, lawful and reasonable: *Breyer* op. 1, paras 43 and 46; *Scania* paras 46 and 49; *IAB Europe* op. 1; for pseudonymised data, per recipient: *EDPS v SRB* para 86, conditions para 77; for the information duty, the controller's view at collection: para 111 | CJEU (on the GDPR and on Reg. 2018/1725); omnibus proposal pending; EDPB Guidelines 01/2025 and 02/2026 not final |
| "in the context of the activities of an establishment" (Art 3(1)) | Yes | *Google Spain* op. 2 (a sales subsidiary orientated to the Member State) | CJEU (under Directive 95/46; carried over) |
| "transfer" (Ch. V; undefined) | Yes | EDPB Guidelines 05/2021, three criteria; *Bindl* paras 95, 133, 135 and 188 | Soft law; General Court under appeal |
| "adequate level of protection" (Art 45(1)) | Yes | "essentially equivalent": *Schrems I* para 73; Rec 104; *Schrems II* para 94 | CJEU |
| "appropriate safeguards", "enforceable … rights", "effective legal remedies" (Art 46(1)) | Yes | Essential equivalence applies to SCC transfers; assessment of the destination's access regime; supplementary measures: *Schrems II* para 105 (op. 2), paras 133 and 134 | CJEU; method in EDPB Recommendations 01/2020 |
| Limits on government access ("necessary and proportionate in a democratic society", Clause 14(a); Charter 52(1)) | Yes | Strict necessity and clear, precise rules: *Digital Rights Ireland* paras 52 and 54; generalised access to content violates the essence: *Schrems I* para 94; US limits not essentially equivalent under the Privacy Shield: *Schrems II* para 185; ex post judicial review suffices for bulk collection: *Latombe* para 105 (under appeal); BBW § 361 criteria | CJEU; General Court (provisional); ECtHR |
| Sharing intelligence with foreign states | Yes | Receiving state must have safeguards against abuse, "not necessarily … comparable protection": BBW § 362; privacy interests considered: *Centrum* § 369 | ECtHR, binding on the states |
| "necessary" (Art 49(1)(b)–(f)) | Yes | "close and substantial connection" (Guidelines 2/2018) | Soft law only; no CJEU reading found |
| "occasional", "not repetitive" (Rec 111; Art 49(1) second subparagraph) | Yes | "more than once, but not regularly … outside the regular course of actions" (Guidelines 2/2018) | Soft law only |
| "explicitly consented … after having been informed of the possible risks" (Art 49(1)(a)) | Yes | Guidelines 2/2018 | Soft law only |
| "important reasons of public interest" (Art 49(1)(d), (4)) | Yes | Must be recognised in Union or Member State law (text of Art 49(4)) | Text; reference C-804/25 pending |
| "compelling legitimate interests", "limited number of data subjects" (Art 49(1) second subparagraph) | Yes | None beyond Guidelines 2/2018 | Soft law only |
| "arbitrary", "unlawful" (ICCPR Art 17) | Yes | General Comment 16 paras 3 and 4 | Treaty body; authoritative, not binding |
| "in accordance with the law", "necessary in a democratic society" (ECHR 8(2)) | Yes | *S. and Marper* § 125 (blanket and indiscriminate fails); BBW §§ 350 and 361 | ECtHR |
| "appropriate level of protection" (108+ Art 14(2)) | Yes | Explanatory report paras 102–112 | Report endorsed by the Committee of Ministers; Protocol not in force |
| "adequate level of protection" (ETS 181 Art 2(1)) | Yes | Not fixed in this survey | — |
| "substantially observes", "sufficient safeguards", "proportionate to the risks" (OECD paras 17–18) | Yes | None | Soft law |

In the infrastructure each open term is an **open point**, as apodeixis defines it. Each fixed reading is a **reading** with its source and rank, so that derivability Γ ⊢_ι φ is computed relative to the readings admitted. A reading of rank 5 or 6 can be withdrawn without touching the rules, and a CJEU judgment that overrules one replaces it. The interpretive arguments of apodeixis rank readings by kind; tekmerion's interpreters may propose a reading where none is fixed. Any such proposal is of grade C and never the record's.

### 14.6 The Shape of the Answer

The answer is a record with five parts:

1. **Verdict:** may proceed / may not proceed / open, with the open points named.
2. **Ground:** for example GDPR Art 45(1) with Decision 2023/1795 Art 1 (DPF List); or Art 46(2)(c) with Decision 2021/914 Module Two; or Art 49(1)(e).
3. **Safeguards required:**
   - the clauses (by address);
   - the Clause 14 assessment, with its findings against Guarantees A–D;
   - any supplementary measures;
   - the information and notifications Art 49 requires.
4. **What it rests on:** the derivation, each step citing its clause and each open point its reading, up to Charter Arts 7, 8 and 52 and, through 52(3), ECHR Art 8.
5. **Standing:**
   - provisions under challenge (C-703/25 P; C-206/25 P);
   - proposals not yet law (the Digital Omnibus' Art 4(1));
   - instruments not in force (CETS 223);
   - drafts of soft law (EDPB Guidelines 01/2025 and 02/2026);
   - the dates on which each was last checked.

### 14.7 Fit With the Infrastructure

| Part | Home |
|---|---|
| Clauses as data files, each a holding by digest | archeion |
| Norms (rules, exceptions, open terms, counts-as links) applied to a record of facts | the law domain's `norms` |
| The order of authority (§14.2) | the law domain's `sources`, as a declared ranking |
| The derivation, its open points and readings, D(φ) and its label (clear, arguable, untenable) | apodeixis |
| Readings from judgments | the law domain's `precedent`, where a case base fits; elsewhere as interpretive arguments |
| Evidence of the destination's law (statutes, reports, the Commission's reviews) | exhibits weighed by tekmerion |
| The schema of the answer and of the facts | categories verified by kategoria, as the facts schema is |

*Remark.* The law domain says that no standard in it is any jurisdiction's until a scheme states it. This would be the first scheme to state one, and the first to bind the domain's general machinery to dated, cited texts.

### 14.8 Deferred to Later Schemes

- BCRs (Art 47) and codes and certifications (Arts 40–42).
- Onward transfers under each SCC module.
- The Law Enforcement Directive's Chapter V and Regulation 2018/1725's Chapter V.
- Transfers under Convention 108+ once it is in force.
- The United Kingdom's regime after the Data (Use and Access) Act 2025; Brazil's (ANPD Res. 19/2024); China's; the Global CBPR among its members.
- The AU, ECOWAS (revised 2026) and RIPD instruments, once their texts are verified and reuse is cleared.
- Damages for unlawful transfers (Art 82; *Bindl*).

---

## 15. Items Not Verified

1. The live text of the EUR-Lex copyright notice. It was read from the Internet Archive's capture of 29 May 2025, because the live page refuses scripts.
2. The Commission website's CC BY 4.0 notice; ECtHR translations' terms; OECD CC BY coverage of legal instruments.
3. Ratifications of CETS 223 after 15 May 2026 (the live chart refuses scripts); the composition of the 55 parties to ETS 108; the party count of ETS 181.
4. The date of General Comment 16's adoption (8 April 1988, from Refworld); the absence of a newer general comment on Article 17; a General Assembly resolution of late 2026.
5. The Special Rapporteur's mandate renewal symbol (A/HRC/RES/55/3) and her report of March 2026.
6. The Council's Digital Omnibus mandate content (21 October 2026) and document ST 14756/26.
7. Developments in C-703/25 P after the notice of appeal, and Microsoft's reported intervention; the General Court's ruling on remand in T-557/20; C-585/26.
8. The OJ references of GDPR corrigenda R(01) and R(03); a final version of EDPB Guidelines 01/2025.
9. The Global CBPR Forum's carry-over of APEC certifications; the DIFC's status.
10. The Malabo Convention's article structure (needs OCR); the ECOWAS 2010 Act's structure; the number and text of the revised ECOWAS Act; the RIPD model clauses' date; module 2 of the Council of Europe clauses.
11. All the national regimes in §12, apart from the citation of the UK Act.
12. EUR-Lex's SOAP web service and RSS, which were not tested.

---

## 16. Sources Consulted

**European Union**

- Cellar: `http://publications.europa.eu/resource/celex/{CELEX}`; SPARQL: `https://publications.europa.eu/webapi/rdf/sparql`
- EUR-Lex copyright notice: `https://eur-lex.europa.eu/content/legal-notice/legal-notice.html` (archived 29 May 2025)
- Commission Decision 2011/833/EU, CELEX 32011D0833
- Publications Office copyright: `https://op.europa.eu/en/web/about-us/legal-notices/publications-office-of-the-european-union-copyright`
- Commission adequacy page: `https://commission.europa.eu/law/law-topic/data-protection/international-dimension-data-protection/adequacy-decisions_en`
- Commission SCC page: `https://commission.europa.eu/law/law-topic/data-protection/international-dimension-data-protection/standard-contractual-clauses-scc_en`
- EDPB copyright: `https://edpb.europa.eu/copyright_en`; EDPB documents as listed in §9.7
- Legislative Observatory, procedures 2025/0360(COD) and 2025/0130(COD)

**Court of Justice of the European Union**

- CURIA copyright: `https://curia.europa.eu/site/jcms/d2_5167/en/copyright`
- CURIA disclaimer: `https://curia.europa.eu/common/juris/en/avertissement.html`
- CURIA legal notice: `https://curia.europa.eu/site/jcms/d2_5166/en/legal-notice`
- Judgments on `juris.curia.europa.eu` by docid (§10.1)
- Press release 106/25: `https://curia.europa.eu/site/upload/docs/application/pdf/2025-09/cp250106en.pdf`

**European Court of Human Rights**

- Copyright and disclaimer: `https://www.echr.coe.int/copyright-and-disclaimer`
- Convention: `https://www.echr.coe.int/documents/d/echr/Convention_ENG`
- HUDOC: `https://hudoc.echr.coe.int`

**Council of Europe**

- Licensing: `https://www.coe.int/en/web/portal/copyright-licensing-permissions`
- Texts on `rm.coe.int` (§6)
- T-PD(2026)50RAPAbr_EN: `https://rm.coe.int/t-pd-2026-50rapabr-en/48802c2217`
- Moldova's ratification: `https://www.coe.int/en/web/data-protection/-/republic-of-moldova-becomes-the-34th-state-to-ratify-the-convention-108-`

**United Nations**

- UDHR: `https://www.un.org/en/about-us/universal-declaration-of-human-rights`
- ICCPR: `https://www.ohchr.org/sites/default/files/ccpr.pdf`
- UNTC status page and XML (§4.4)
- Official Document System by symbol (§4.4)
- Terms of use: `https://www.un.org/en/about-us/terms-of-use`
- ST/AI/189/Add.9/Rev.2

**OECD**

- `https://legalinstruments.oecd.org/en/instruments/OECD-LEGAL-0188`; the Compendium API and PDFs (§4.6)

**APEC and the Global CBPR Forum**

- `https://www.apec.org/publications/2017/08/apec-privacy-framework-(2015)`
- `https://www.apec.org/termsofuse`
- `https://www.globalcbpr.org`
- `https://www.meti.go.jp/english/press/2025/0602_001.html`

**Regional**

- AU treaty page and status list (§11.1)
- ECOWAS communiqué (§11.2)
- ASEAN MCCs and the Joint Guide (§11.3)
- RIPD Standards (§11.4)
- ITU/SADC Model Law (§11.5)

**United Kingdom**

- `https://www.legislation.gov.uk/ukpga/2025/18/contents`
