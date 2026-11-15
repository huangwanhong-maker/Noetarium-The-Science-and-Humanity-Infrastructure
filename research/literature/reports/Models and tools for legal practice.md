# Models and Tools for a Studio of Legal Practice: A Survey

*Evidence store, recognition of evidence, interpretation of norms and legal reasoning, with international privacy and data-protection law as the first body of law.*

Status: `draft` (a survey for design, 2026-11-15). Activity: research, in service of use.

## 0. Scope, Method and Conventions

### 0.1 Purpose

This survey collects the literature, standards and implementations that a studio for counsel (preparing and arguing a case) could stand on, in four activities: (1) an evidence store, (2) recognition of evidence in documents, (3) interpretation of norms, (4) legal reasoning. The first body of law is international privacy and data-protection law: Chapter V of the GDPR (international transfers), Convention 108+, Article 17 ICCPR, Article 8 ECHR, and the OECD and APEC frameworks.

It does not re-survey what the infrastructure already holds (apodeixis's calculi, Dung and ASPIC+, D(φ) under interpretation; tekmerion's charts, Schum's chain, evidence idioms, Dempster–Shafer, scenarios, graded candidates, interpreters and reviews; archeion's fixity, journals, Merkle proofs, custody, PROV, BagIt, retention and access; the law domain's facts, proof, sources, norms, exhibits, Horty's reason model and proceedings). Where the literature goes beyond those holdings, each section says so under *Beyond what we hold*.

### 0.2 Verification Markers

Every item carries one marker.

- **[V]** verified this session against a primary record: the DOI resolved and its Crossref metadata (authors, title, venue) matched, or the official page (standard body, court, repository API) was read.
- **[S]** confirmed only through secondary sources (law-firm notes, catalogues, citing papers); the primary text was not read.
- **[M]** from prior knowledge, not re-verified this session. Treat as a lead, not a citation.

Licences of software were read from the GitHub API (`license.spdx_id`) or the repository's LICENCE file on 2026-11-15, unless marked otherwise. "No licence declared" means the repository metadata names none, which in law means all rights reserved.

### 0.3 The Contract Every Component Should Keep

One principle runs through the recommendations (§G): a component returns a *candidate with its evidence*, and a separate checker that does not share its code verifies it. This is already the infrastructure's rule for kategoria, apodeixis and tekmerion; the legal-technology literature (B.7 on hallucinated citations, A.5 on certified recall) shows why practice needs it too.

---

## A. Evidence Management in Practice

### A.1 Process Models and Standards

**EDRM, the Electronic Discovery Reference Model** [V] (https://edrm.net/resources/frameworks-and-standards/edrm-model/). The reference process of e-discovery. EDRM 2.0 (2026) names Identification, Preservation, Collection, Processing, Review, Production, Presentation, Analysis and Disposition; Identification to Processing are grouped as "Data Acquisition", and Analysis runs across every phase. The page states that the diagram is conceptual and iterative, not a linear workflow. Licence: the diagram is CC BY 4.0. Beside it EDRM keeps the Information Governance Reference Model, a Metrics Model and a Privacy & Security Risk Reduction Model.
*Computable:* nothing by itself; it is a vocabulary of stages. *Use in the studio:* name the states of a holding's life (an enumeration on archeion's units) so that every operation on a holding records which stage it belongs to.

**ISO/IEC 27037:2012**, Guidelines for identification, collection, acquisition and preservation of digital evidence [S] (https://www.iso.org/standard/44381.html; listed as confirmed in 2018, a later review outcome not found). It defines the roles of the first responder and specialist and the principles of auditability, repeatability, reproducibility and justifiability [M for the principle list].
**ISO/IEC 27041:2015**, Guidance on assuring suitability and adequacy of incident investigative method [S]: method development, assurance models and the production of evidence that a method is fit for purpose.
**ISO/IEC 27042:2015**, Guidelines for the analysis and interpretation of digital evidence [S]: continuity, validity, reproducibility, repeatability, with enough recorded for independent scrutiny.
**ISO/IEC 27043:2015**, Incident investigation principles and processes [M].
**ISO/IEC 27050**, Electronic discovery, parts 1–4 [S]: Part 1 overview and concepts (second edition 2019), Part 2 governance, Part 3 code of practice, Part 4 ICT readiness (2021). The editions of Parts 2 and 3 were not confirmed.
All ISO texts are paywalled. *Computable:* none directly; they are requirements on process and record. *Use:* a declared scheme of required records per stage (who, what tool and version, hash before and after, method validation reference), checked by archeion like any other scheme. ISO/IEC 27041's "evidence for assurance" maps onto recording, for every method a recogniser or reviewer runs, its version and its validation set.

**Federal Rule of Evidence 902(13) and (14)** (United States, in force 1 December 2017) [S]: certified records generated by an electronic process, and data copied from a device, storage medium or file, become self-authenticating on a certificate of a qualified person; the Advisory Committee's note says such data are ordinarily authenticated by hash value, and allows other reliable methods. This is the clearest instance in positive law of what archeion already computes.

**Long-term evidence records** [M]: RFC 3161 (Time-Stamp Protocol), RFC 4998 (Evidence Record Syntax, hash-tree based records that can be renewed when a hash algorithm weakens) and RFC 6283 (its XML form). *Beyond what we hold:* archeion has Merkle trees and consistency proofs; ERS adds the *renewal* of evidence records over decades (re-hashing under a new algorithm, chaining time-stamps), which matters for holdings that must outlive SHA-256's comfortable life, and an external time-stamp that a court can verify without trusting the studio.

### A.2 Forensic Formats, Provenance and Interchange

| Item | What it is | Licence | Marker |
|---|---|---|---|
| libewf | Reads the Expert Witness Format (E01), the de facto image format of forensic acquisition | LGPL-3.0 | [V] |
| AFF4 (pyaff4) | Advanced Forensic Format 4, an RDF-described container for images and their metadata | Apache-2.0 | [V] |
| DFXML | Garfinkel, "Digital forensics XML and the DFXML toolset", *Digital Investigation* 8 (2012) 161–174, doi:10.1016/j.diin.2011.11.002 | tool repository not located | [V] paper |
| CASE / UCO | Cyber-investigation Analysis Standard Expression on the Unified Cyber Ontology: investigations, provenance records, actions, tools and chains of custody as OWL/RDF. Casey et al., *Digital Investigation* 22 (2017) 14–45, doi:10.1016/j.diin.2017.08.002 | Apache-2.0 (casework/CASE, ucoProject/UCO) | [V] |
| Plaso (log2timeline) | Extracts events from hundreds of artefact types into one timeline | Apache-2.0 | [V] |
| Apache Tika | Text and metadata from over a thousand file types | Apache-2.0 | [V] |
| The Sleuth Kit | File-system analysis library | no SPDX licence declared (mixed licences, [M]) | [V] metadata |
| Aleph (OCCRP) | Search and cross-referencing of documents and entities for investigations | MIT | [V] |
| Datashare (ICIJ) | Self-hosted document search | AGPL-3.0 | [V] |

*Beyond what we hold:* CASE is the forensic community's provenance vocabulary and stands where archeion's W3C PROV records stand; since symploke reads RDF, a holding's custody can be exported as CASE without a new model. Plaso's timeline is a natural input to tekmerion's `timelines` (a simple temporal network), each event carrying the artefact it came from.

### A.3 Legal Hold

**The Sedona Conference, Commentary on Legal Holds, Second Edition: The Trigger & The Process**, 20 Sedona Conf. J. 341 (2019) [S] (https://www.thesedonaconference.org/publication/Commentary_on_Legal_Holds): twelve guidelines, from when the duty to preserve arises ("reasonable anticipation of litigation": notice of a credible probability of involvement) through scope, notices, monitoring compliance (Guideline 10) and release (Guideline 11). It reflects the 2015 amendments to the Federal Rules of Civil Procedure; Rule 37(e) governs failure to preserve ESI [M]. *Zubulake v. UBS Warburg*, 220 F.R.D. 212 (S.D.N.Y. 2003), is the classical statement of the trigger [M]. In England and Wales, Practice Direction 57AD (Business and Property Courts, from October 2022) sets preservation duties and invites technology-assisted review [M; the official page could not be read].

*Computable:* a hold is an override of retention on a set defined by custodians, sources, a date range and queries; its events (trigger, issue, acknowledgement, reminder, release) are dated and attributable. *Beyond what we hold:* archeion has retention schedules with holds; Sedona adds the *trigger reasoning* (when the duty arose, an argument from dated facts) and the obligation to monitor and release, which the docket of the law domain can carry as steps with deadlines.

### A.4 Processing: Duplicates, Near-Duplicates and Threads

- **Exact duplicates** by digest, at document and at family level (an e-mail with its attachments). Archeion already identifies holdings by SHA-256.
- **Near-duplicates.** Broder, "On the resemblance and containment of documents", SEQUENCES 1997, 21–29, doi:10.1109/SEQUEN.1997.666900 [V] (shingling and MinHash, Jaccard resemblance estimated without bias); Charikar, "Similarity estimation techniques from rounding algorithms", STOC 2002, 380–388, doi:10.1145/509907.509965 [V] (SimHash, angular similarity); Manku, Jain and Das Sarma, "Detecting near-duplicates for web crawling", WWW 2007, 141–150, doi:10.1145/1242572.1242592 [V] (SimHash at scale with Hamming-distance tables). *Computable:* unbiased estimates of resemblance with variance known in closed form, so a near-duplicate claim can carry its error.
- **E-mail threading.** RFC 5322 header fields `Message-ID`, `In-Reply-To`, `References`; RFC 5256 (IMAP SORT and THREAD, the ORDEREDSUBJECT and REFERENCES algorithms); Zawinski's threading algorithm (https://www.jwz.org/doc/threading.html) [M]. Practice adds "inclusive" messages (those whose text contains no other message's), which lets review skip messages wholly quoted elsewhere [M]. *Computable:* a forest of messages, exact from headers where they exist, heuristic (subject and quoted-text containment) where they do not; the heuristic links should be marked as inferred, never merged with header links.
- **Known-file exclusion** ("de-NISTing") against NIST's National Software Reference Library hash sets [M].

### A.5 Technology-Assisted Review

**Foundations.**
- Grossman and Cormack, "Technology-Assisted Review in E-Discovery Can Be More Effective and More Efficient Than Exhaustive Manual Review", 17 Rich. J.L. & Tech. 11 (2011) [V] (https://scholarship.richmond.edu/jolt/vol17/iss3/5/): on TREC 2009 Legal Track data, TAR matched or exceeded exhaustive manual review in recall and precision.
- Cormack and Grossman, "Evaluation of machine-learning protocols for technology-assisted review in electronic discovery", SIGIR 2014, 153–162, doi:10.1145/2600428.2609601 [V]: continuous active learning (CAL: train, rank, review the top, retrain) against simple active learning (SAL) and simple passive learning (SPL); CAL reaches a given recall with least effort.
- Cormack and Grossman, "Multi-faceted recall of continuous active learning for TAR", SIGIR 2015, doi:10.1145/2766462.2767771 [V]; "Autonomy and reliability of continuous active learning for technology-assisted review", arXiv:1504.06868 [M].
- Cormack and Grossman, "Engineering quality and reliability in technology-assisted review", SIGIR 2016, 75–84, doi:10.1145/2911451.2911510 [V]: any TAR method can be augmented to *guaranteed* reliability at a quantifiable cost; the "knee" and "target" stopping methods; argues for a Taguchi loss over a single recall goal-post.
- TREC Total Recall Track 2015 and 2016 overviews, doi:10.6028/nist.sp.500-319.recall-overview and doi:10.6028/nist.sp.500-321.recall-overview [V]; the Baseline Model Implementation (BMI, CAL with logistic regression on tf-idf features) was supplied there [M for the BMI's distribution terms].

**Stopping and certification.**
- Lewis, Yang and Frieder, "Certifying one-phase technology-assisted reviews", CIKM 2021, 893–902, doi:10.1145/3459637.3482415 [V]: the first broadly applicable, statistically valid sample-based stopping rules for one-phase TAR, from quantile estimation; overshooting the recall target is a major source of excess cost.
- Yang, Lewis and Frieder, "Heuristic stopping rules for technology-assisted review", DocEng 2021, doi:10.1145/3469096.3469873 [V] (Quant and QuantCI); and "On minimizing cost in legal document review workflows", DocEng 2021, doi:10.1145/3469096.3469872 [V].
- Webber, "Approximate recall confidence intervals", *ACM TOIS* 31 (2013) 1–33, doi:10.1145/2414782.2414784 [V]: the normal approximation covers poorly; beta-binomial posteriors on retrieved and unretrieved yield with a Monte Carlo estimate of recall give near-nominal coverage.
- Callaghan and Müller-Hansen, "Statistical stopping criteria for automated screening in systematic reviews", *Systematic Reviews* 9 (2020) 273, doi:10.1186/s13643-020-01521-4 [V resolves]: a hypergeometric test of the null that recall is below target, on random samples of the unscreened remainder.
- Li and Kanoulas, "When to stop reviewing in technology-assisted reviews", *ACM TOIS* 38 (2020), doi:10.1145/3411755 [V].
- Sneyd and Stevenson, "Modelling stopping criteria for search results using Poisson processes", EMNLP 2019 (arXiv:1909.06239) [S]; "Stopping criteria for technology assisted reviews based on counting processes", SIGIR 2021, 2293–2297, doi:10.1145/3404835.3463013 [V]; Stevenson and Bin-Hezam, "Stopping methods for technology-assisted reviews based on point processes", *ACM TOIS* 42 (2023), doi:10.1145/3631990 [V].

**Practice and case law.** EDRM and the Bolch Judicial Institute (Duke Law), *Technology Assisted Review (TAR) Guidelines* (January 2019, Creative Commons) [S]; The Sedona Conference, *TAR Case Law Primer, Second Edition* (2023) [S]; *Da Silva Moore v. Publicis Groupe*, 287 F.R.D. 182 (S.D.N.Y. 2012) and *Rio Tinto v. Vale*, 306 F.R.D. 125 (S.D.N.Y. 2015) [M]; *Pyrrho Investments v MWB Property* [2016] EWHC 256 (Ch), the first express English approval of predictive coding [S].

**Implementations and data.**

| Item | Licence | Marker |
|---|---|---|
| TARexp (Yang et al.), a framework for TAR experiments | Apache-2.0 | [V] |
| HiCAL, high-recall retrieval with an assessing interface | GPL-3.0 | [V] |
| CLEF TAR collections (technology-assisted review in empirical medicine) | MIT | [V] |
| Enron corpus: Klimt and Yang, ECML 2004, doi:10.1007/978-3-540-30115-8_22 | research use; EDRM's processed version's licence [M] | [V] paper |
| TREC Legal Track (2006–2011), TREC Total Recall (2015–2016) collections | NIST/LDC agreements [M] | [V] overviews |

*Computable:* everything here is statistical and testable: a ranker; a sampling design over the unreviewed set; a recall estimate with an interval; a stopping rule with a stated guarantee; a cost model. *Beyond what we hold:* tekmerion's systematic reviews already screen with two reviewers, κ and a PRISMA flow. TAR adds active learning, recall certification and stopping with guarantees; the two share the sampling theory, and peira already holds the survey estimators (Horvitz–Thompson, strata) on which an elusion test rests.

### A.6 Privilege and its Log

United States: FRCP 26(b)(5)(A) requires a party withholding on privilege to describe what is withheld so that the claim can be assessed; FRE 502 limits waiver and allows clawback orders [M]. The Sedona Conference, *Commentary on Privilege Logs* (public comment February 2024, final May 2024, Sedona Conf. J. vol. 25) [S] sets out categorical and metadata logs and other means of proportionality.

*Computable:* (i) a log as a projection of the holding's metadata (date, author, recipients, privilege basis, description) per document or per category; (ii) consistency checks: the same document, or members of one family or thread, given different calls; near-duplicates with different calls; a third party among recipients of a document logged as privileged (a waiver flag); (iii) a classifier proposing privilege candidates, always reviewed. None of these needs new mathematics; they need the record to be explicit.

### A.7 How it Sits behind an Interface

An **EvidenceModel** pipeline of pluggable stages over archeion holdings: `Acquirer` (forensic image or export → holdings with CASE/PROV records), `Processor` (extraction, deduplication, near-duplicate clusters, threads; each output a relation between holdings with its method and error), `Reviewer` (a protocol: CAL, SAL, linear; a ranker behind it), `StoppingRule` (returns *stop* with its stated guarantee and the sample it used), `Certifier` (recall estimate with interval, separate code from the ranker), `PrivilegeLogger`, `Producer`. Each stage writes to the sealed journal.

---

## B. Recognition of Evidence in Documents

### B.1 Identifiers and Document Standards

- **ECLI**, the European Case Law Identifier: Council conclusions of 2011, revised as Council conclusions on ECLI and a minimum set of uniform metadata for case law, OJ C 360, 24 October 2019 (2019/C 360/01) [S]. Five colon-separated parts (`ECLI:country:court:year:number`) [M], machine-parsable by a regular grammar. The CJEU and the ECtHR assign ECLIs (e.g. *Schrems II*, ECLI:EU:C:2020:559 [M]).
- **Akoma Ntoso Version 1.0**, OASIS Standard, 29 August 2018 [V] (https://docs.oasis-open.org/legaldocml/akn-core/v1.0/): XML for legislative, regulatory and judicial documents, with FRBR-style identifiers; namespace `http://docs.oasis-open.org/legaldocml/ns/akn/3.0`.
- **ELI**, the European Legislation Identifier [M]; **CELEX** numbers for EU law in EUR-Lex [M].

*Computable:* exact parsing and resolution. A citation to *Schrems II* by ECLI, to Article 46(2)(c) GDPR by ELI and article path, or to an ECtHR judgment by application number can be resolved against the authoritative register at the edge (EUR-Lex/CELLAR, HUDOC), and the passage fetched for verification.

### B.2 Citations

- **eyecite** (Free Law Project): Cushman et al., "eyecite: A tool for parsing legal citations", *JOSS* 6 (2021) 3617, doi:10.21105/joss.03617 [V]; licence BSD-2-Clause [V]; with **reporters-db** (BSD-2-Clause) [V]. US-centred: full, short, *supra* and *id.* citations, with resolution of short forms to their antecedents.
- **CourtListener Citation Lookup and Verification API** (April 2024) [S] (https://www.courtlistener.com/help/api/rest/v3/citation-lookup): a text in, every citation out with a status; up to 250 citations a request; normalises reporter abbreviations. A "not found" is not proof of fabrication (coverage gaps).
- **Blackstone** (ICLR&D), a spaCy pipeline for UK legal text (case names, citations, instruments, provisions) [V], Apache-2.0, last pushed 2024.
- **LexNLP** (LexPredict) [V], AGPL-3.0: citations, dates, amounts, definitions, courts. Bommarito, Katz and Detterman, arXiv:1806.03688 [M].

### B.3 Entities, Facts and Sentence Roles

- Leitner, Rehm and Moreno-Schneider, "Fine-grained named entity recognition in legal documents", SEMANTiCS 2019, doi:10.1007/978-3-030-33220-4_20 [V] (German court decisions, 19 classes; dataset licence [M]).
- Walker et al., "Semantic types for computational legal reasoning: propositional connectives and sentence roles in the veterans' claims dataset", ICAIL 2017, 217–226, doi:10.1145/3086512.3086535 [V]: sentences labelled as finding, evidence, reasoning, legal rule, citation.
- Šavelka and Ashley, "Segmenting U.S. court decisions into functional and issue specific parts", JURIX 2018, doi:10.3233/978-1-61499-935-5-111 [V].
- Šavelka and Ashley, "The unreasonable effectiveness of large language models in zero-shot semantic annotation of legal texts", *Frontiers in AI* 6 (2023), doi:10.3389/frai.2023.1279794 [V]; Gray, Šavelka, Oliver and Ashley, "Can GPT alleviate the burden of annotation?", JURIX 2023, doi:10.3233/faia230961 [V]; Gray et al., *Phil. Trans. R. Soc. A* (2024), doi:10.1098/rsta.2023.0155 [V resolves] on automatic identification of legally relevant factors.

### B.4 Rhetorical Roles

Hachey and Grover, "Extractive summarisation of legal texts", *AI & Law* 14 (2006) 305–345, doi:10.1007/s10506-007-9039-z [V] (House of Lords judgments; roles such as fact, proceedings, background, framing, disposal); Saravanan and Ravindran, *AI & Law* 18 (2010) 45–76, doi:10.1007/s10506-010-9087-7 [V]; Bhattacharya et al., JURIX 2019, doi:10.3233/faia190301 [V]; Kalamkar et al., "Corpus for automatic structuring of legal documents", LREC 2022, 4420–4429 [V]; Modi et al., SemEval-2023 Task 6 (LegalEval), doi:10.18653/v1/2023.semeval-1.318 [V resolves].
*Computable:* sequence labelling with known agreement ceilings. *Use:* a role label is a *Recogniser* output on spans; it steers what else is extracted (facts from the facts section, holdings from the reasoning), never a fact itself.

### B.5 Argument Mining in Legal Texts

- Palau and Moens, "Argumentation mining: the detection, classification and structure of arguments in text", ICAIL 2009, 98–107, doi:10.1145/1568234.1568246 [V]; Mochales and Moens, "Argumentation mining", *AI & Law* 19 (2011) 1–22, doi:10.1007/s10506-010-9104-x [V]: premises, conclusions and their structure in ECHR judgments, partly by a context-free grammar of argument structure.
- Poudyal et al., "ECHR: Legal corpus for argument mining", ArgMining 2020, 67–75 (https://aclanthology.org/2020.argmining-1.8/) [V]: 42 ECtHR decisions annotated for premises, conclusions and their relations.
- Habernal et al., "Mining legal arguments in court decisions", *AI & Law* 32 (2024) 557–594, doi:10.1007/s10506-023-09361-y [V]: an annotation scheme from legal argumentation theory; 373 ECtHR decisions on Articles 3, 7 and **8**, about 15,000 argument spans (2.4 M tokens); data and code at trusthlt/mining-legal-arguments, **Apache-2.0** [V]. This is the most directly usable recogniser for the first body of law: Article 8 is the ECHR's privacy article, and the scheme types arguments by kind (including the Court's interpretive and proportionality arguments).

*Beyond what we hold:* tekmerion holds charts and ASPIC+ theories written by hand; argument mining proposes their nodes from the judgment's text, each with its span.

### B.6 Benchmarks and Corpora, with Privacy Corpora Apart

General:

| Corpus | Reference | Licence | Marker |
|---|---|---|---|
| LexGLUE (ECtHR A/B, SCOTUS, EUR-LEX, LEDGAR, UNFAIR-ToS, CaseHOLD) | Chalkidis et al., ACL 2022, doi:10.18653/v1/2022.acl-long.297 | dataset card CC BY 4.0; code repository no licence declared | [V] |
| ECtHR judgment prediction | Chalkidis et al., ACL 2019, doi:10.18653/v1/P19-1424 | [M] | [V] paper |
| ECHR-OD | Quemy and Wrembel, *Information Systems* 106 (2022) 101822, doi:10.1016/j.is.2021.101822 | [M] | [V] paper |
| LegalBench, 162 tasks | Guha et al., NeurIPS 36 (2023), doi:10.52202/075280-1915 | per task; repository no licence declared | [V] |
| CUAD (contracts) | Hendrycks et al., NeurIPS 2021 Datasets | CC BY 4.0 [M]; repository no licence declared | [S] |
| ContractNLI | Koreeda and Manning, Findings EMNLP 2021, doi:10.18653/v1/2021.findings-emnlp.164 | [M] | [V] paper |
| MultiEURLEX | Chalkidis et al., EMNLP 2021, doi:10.18653/v1/2021.emnlp-main.559 | [M] | [V] paper |

Privacy and data protection:

| Corpus or system | Reference | Licence | Marker |
|---|---|---|---|
| OPP-115 (115 privacy policies, data-practice annotations) | Wilson et al., ACL 2016, doi:10.18653/v1/P16-1126 | CC BY-NC-SA, research and teaching only (terms stated inconsistently across usableprivacy.org pages) | [V] paper, [S] licence |
| PrivacyQA | Ravichander et al., EMNLP-IJCNLP 2019, doi:10.18653/v1/D19-1500 | [M] | [V] paper |
| MAPS / APP-350 | Zimmeck et al., *PoPETs* 2019, doi:10.2478/popets-2019-0037 | [M] | [V] paper |
| Polisis | Harkous et al., USENIX Security 2018 | no DOI found | [M] |
| CLAUDETTE (unfair clauses in terms of service) | Lippi et al., *AI & Law* 27 (2019) 117–139, doi:10.1007/s10506-019-09243-2 | [M] | [V] paper |
| Completeness of privacy policies against GDPR | Torre et al., RE 2020, doi:10.1109/re48521.2020.00025; Amaral et al., *IEEE TSE* 48 (2022) 4647–4674, doi:10.1109/TSE.2021.3124332 | [M] | [V] |
| Data processing agreements against GDPR (Art. 28) | Cejas et al., *IEEE TSE* 49 (2023) 4282–4303, doi:10.1109/TSE.2023.3288901 | [M] | [V] |
| GDPRhub (noyb): summaries of DPA and court decisions | https://gdprhub.eu | not verified (site blocked automated reading); noyb's own site content is CC BY-NC 3.0 [S] | [S] |

*Remark.* Benchmarks of judgment *prediction* (LexGLUE's ECtHR tasks) measure something counsel should not lean on (§G.4). The corpora of argument, roles and privacy-policy practices are those worth having behind a Recogniser.

### B.7 Extraction by Language Models, Grounding and its Failures

**Failure modes in practice.**
- *Mata v. Avianca, Inc.*, 678 F. Supp. 3d 443 (S.D.N.Y. 2023) (Castel J, 22 June 2023) [S]: sanctions under Rule 11 for filing citations to cases a chatbot invented, and for failing to withdraw them when challenged.
- *R (Ayinde) v London Borough of Haringey; Al-Haroun v Qatar National Bank* [2025] EWHC 1383 (Admin) (Divisional Court, Dame Victoria Sharp P and Johnson J, 6 June 2025) [S] (https://caselaw.nationalarchives.gov.uk/ewhc/admin/2025/1383): fictitious authorities and misquotations; the duties of counsel and solicitors restated; the first-instance decision is [2025] EWHC 1040 (Admin).
- Dahl, Magesh, Suzgun and Ho, "Large legal fictions: profiling legal hallucinations in large language models", *Journal of Legal Analysis* 16 (2024) 64–93, doi:10.1093/jla/laae003 [V].
- Magesh et al., "Hallucination-free? Assessing the reliability of leading AI legal research tools", *J. Empirical Legal Studies* 22 (2025) 216, doi:10.1111/jels.12413 [V resolves]: retrieval-augmented commercial tools still hallucinated on roughly 17–33% of queries [S for the figures]; a hallucination includes a false assertion that a source supports a proposition.
- Charlotin's database of AI hallucination cases in courts [S]: counts reported by third parties range from about 1,200 (early 2026) to about 1,600 (June 2026); read the database itself for a figure.

**Grounding methods.**
- Gao et al., "Enabling large language models to generate text with citations" (ALCE), EMNLP 2023, 6465–6488, doi:10.18653/v1/2023.emnlp-main.398 [V]; Rashkin et al., "Measuring attribution in natural language generation models" (AIS), *Computational Linguistics* 49 (2023) 777–840, doi:10.1162/coli_a_00486 [V resolves]; Min et al., "FActScore", EMNLP 2023, doi:10.18653/v1/2023.emnlp-main.741 [V].
- **LangExtract** (Google) [V]: structured extraction by language models with each extraction aligned to character offsets in the source; Apache-2.0.

**Mitigations that are computable.**
1. *Quotation verification*: every quoted passage must match the holding's text exactly or under a declared normalisation (whitespace, quotation marks, hyphenation), with offsets; anything else is refused, not repaired.
2. *Citation resolution*: every authority is parsed (ECLI, CELEX, ELI, neutral citation, reporter) and resolved against an authoritative register at the edge; "not found" is reported as unresolved, not as false.
3. *Closed world*: a model may cite only holdings already in the store; it proposes, and the store disposes.
4. *Support*: whether the passage supports the proposition is a second question (a human, or a second model as a graded proposal), apart from whether the passage exists.
5. *Grade*: an extraction by a model enters at grade C and rises only by verification, which is already the infrastructure's rule (tekmerion's candidates, the API's `science.interpret`).

### B.8 How it Sits behind an Interface

A **Recogniser** takes a holding (by digest) and optionally a span, and returns candidates `{kind, span offsets in the holding, normalised value, method and version, confidence, grade}`. Kinds: citation, entity, date, quotation, rhetorical role, argument component, argument relation, data-practice label (DPV concept), fact candidate. A **Verifier** per kind (offset match, register resolution, schema validation by symploke's SHACL) runs in separate code. Recognisers may be rules (ECLI grammar on morphe's BNF reader), statistical models (Habernal's), or language models (LangExtract-style), all under one contract.

---

## C. Interpretation of Norms

### C.1 Typologies of Interpretive Argument

- MacCormick and Summers (eds), *Interpreting Statutes: A Comparative Study* (Dartmouth 1991) [S]: nine systems compared; Chapter 12 sets out eleven major types of interpretive argument (linguistic, systemic, teleological/evaluative, and transcategorical such as intention) and a "universalist" thesis that all the systems share them as a common core [S]. DOI not found.
- For treaties, the rules of the Vienna Convention on the Law of Treaties, Articles 31–33 (ordinary meaning in context and in the light of object and purpose; subsequent agreement and practice; relevant rules of international law; supplementary means; languages) [M]. Linderfalk, *On the Interpretation of Treaties* (Springer 2007) reconstructs them as 44 rules from a pragmatic model of what a rule of interpretation is [S; DOI not verified]. Gardiner, *Treaty Interpretation* (2nd edn, OUP 2015) is the standard reference [V chapter record].
- For the first body of law, the courts' own canons matter: the ECtHR's living-instrument reading, autonomous concepts, effectiveness and the margin of appreciation; the CJEU's teleological reading and *effet utile*, and its reading of GDPR terms in the light of the Charter [M]. No computational treatment of these was found.

No computational model of VCLT Articles 31–33 was found [S, after search].

### C.2 Argumentation Schemes for Interpretation

- Sartor, Walton, Macagno and Rotolo, "Argumentation schemes for statutory interpretation: a logical analysis", JURIX 2014, doi:10.3233/978-1-61499-468-8-11 [V resolves].
- Walton, Sartor and Macagno, "An argumentation framework for contested cases of statutory interpretation", *AI & Law* 24 (2016) 51–91, doi:10.1007/s10506-016-9179-0 [V]: interpretive canons as defeasible schemes with critical questions, evaluated in an argumentation framework.
- Walton, Macagno and Sartor, *Statutory Interpretation: Pragmatics and Argumentation* (CUP 2021), doi:10.1017/9781108554572 [V resolves].
- Rotolo, Governatori and Sartor, "Deontic defeasible reasoning in legal interpretation: two options for modelling interpretive arguments", ICAIL 2015, 99–108, doi:10.1145/2746090.2746100 [V].
- Atkinson and Bench-Capon, "Argumentation schemes in AI and Law", *Argument & Computation* 12 (2021) 417–434, doi:10.3233/AAC-200543 [V].

*Beyond what we hold:* apodeixis already ranks interpretive arguments by kind; the literature supplies the *catalogue* (schemes with their critical questions), and Rotolo et al. show two ways of placing the interpretive conclusion inside a defeasible deontic theory, either as a meta-rule over which rule applies or as a rule that rewrites the term.

### C.3 Open Texture

Hart's open texture (1958, 1961) [M] is the problem; the computational treatment closest to our holdings is Canavotto, "Reasoning with hierarchies of open-textured predicates", ICAIL 2023, 52–61, doi:10.1145/3594536.3595148 [V]: the reason model of precedential constraint over a hierarchy of intermediate legal concepts; constraint present in the hierarchy can be lost when it is flattened, and new constraint can appear. Šavelka and Ashley, "Legal information retrieval for understanding statutory terms", *AI & Law* 30 (2022) 245–289, doi:10.1007/s10506-021-09293-5 [V], retrieves sentences that elaborate a statutory term (a dataset of terms and sentences).
*Use:* the open terms of the first body of law are exactly of this kind: "personal data", "essentially equivalent", "necessary in a democratic society", "effective" supplementary measures, "appropriate safeguards". A hierarchy of intermediate concepts (essential equivalence ← clear and precise rules, necessity and proportionality, independent oversight, effective remedies) is the natural carrier of precedent about them.

### C.4 Purposes, Values and Balancing

- Berman and Hafner, "Representing teleological structure in case-based legal reasoning: the missing link", ICAIL 1993, 50–59, doi:10.1145/158976.158982 [V].
- Bench-Capon and Sartor, "A model of legal reasoning with cases incorporating theories and values", *Artificial Intelligence* 150 (2003) 97–143, doi:10.1016/S0004-3702(03)00108-5 [V]: theories constructed from cases, factors promoting values, preferences between values revealed by decisions.
- Bench-Capon, "Persuasion in practical argument using value-based argumentation frameworks", *J. Logic Comput.* 13 (2003) 429–448, doi:10.1093/logcom/13.3.429 [V].
- Grabmair, "Predicting trade secret case outcomes using argument schemes and learned quantitative value effect tradeoffs" (VJAP), ICAIL 2017, 89–98, doi:10.1145/3086512.3086521 [V].
- Gordon and Walton, "Formalizing balancing arguments", COMMA 2016, doi:10.3233/978-1-61499-686-6-327 [V].
*Use:* proportionality under Article 8(2) ECHR, Article 52(1) of the Charter and the EDPB's necessity test is a balancing; it can be carried as a value-based framework whose value ordering is declared per audience (a court, a supervisory authority), so that the studio shows which ordering each conclusion needs.

### C.5 Language Models as Interpreters

- *Snell v. United Specialty Insurance Co.*, 102 F.4th 1208 (11th Cir. 2024), Newsom J concurring [S]: a "modest proposal" to consult LLMs as one tool among several for ordinary meaning, explicitly not to be applied mechanically.
- Engel and McAdams, "Asking GPT for the ordinary meaning of statutory terms", 2024 U. Ill. J.L. Tech. & Pol'y 235 [S]: GPT-3.5 benchmarked against Tobia's survey on "vehicle"; answers move when told the rule's purpose; five lessons for use.
- Arbel and Hoffman, "Generative interpretation", 99 N.Y.U. L. Rev. 451 (2024) [S] (arXiv:2308.06907): LLMs to estimate contractual meaning, quantify ambiguity and fill gaps.
- Waldon, Schneider, Wilcox, Zeldes and Tobia, "Large language models for legal interpretation? Don't take their word for it", *Georgetown Law Journal* 114 (2025) [S]: the design of chatbot products invites misuse by interpreters; guidance for responsible use.
- Purushothama, Min, Waldon and Schneider, "Prompting from the bench" (formerly "Not ready for the bench"), arXiv:2510.25356 [S]: interpretive judgments are unstable across question formats and only moderately correlated with human judgments.
- Choi (2025), reported by the preceding paper as generating 2,000 paraphrases for each of five scenarios and finding models unreliable under variation [S; title and venue not verified].
- Blair-Stanek, Holzenberger and Van Durme, "Can GPT-3 perform statutory reasoning?", ICAIL 2023, 22–31, doi:10.1145/3594536.3595163 [V] (on the SARA tax-statute dataset).
- Tobia, "Testing ordinary meaning", 134 Harv. L. Rev. 726 (2020) [M]; Lee and Mouritsen, "Judging ordinary meaning", 127 Yale L.J. 788 (2018) [M] (corpus linguistics as evidence of meaning).

*Evaluation that the literature converges on:* stability under paraphrase and format; agreement with surveys of ordinary speakers; sensitivity to disclosure of purpose; separation of ordinary from legal meaning. *Use:* tekmerion already holds a model's proposal as an interpreter at grade C. The literature argues for two additions: run a *panel of prompts* (paraphrases, formats, with and without purpose) and read their spread as imprecision (tekmerion's panels already do this for interpreters), and report the stability figure beside every reading a model proposes.

### C.6 How it Sits behind an Interface

The **Interpreter** contract already exists in tekmerion. What this survey adds is a **catalogue of interpretive argument schemes** as data: each scheme with its type (MacCormick–Summers or VCLT rule), its premises, its critical questions and its admissibility conditions (e.g. VCLT Article 32 means only to confirm, or where Article 31 leaves the meaning ambiguous or leads to a manifestly absurd result), and a declared ordering of types per legal order. An interpretive argument is then an instance of a scheme, attacked through its critical questions, and D(φ) is computed as now.

---

## D. Legal Reasoning

### D.1 Case-Based Reasoning and Precedential Constraint

- HYPO: Rissland and Ashley, "A case-based system for trade secrets law", ICAIL 1987, doi:10.1145/41735.41743 [V resolves]; Ashley, *Modeling Legal Argument* (MIT Press 1990) [M]. Dimensions, three-ply arguments (cite, distinguish, counter-example), hypotheticals.
- CATO: Aleven, *Artificial Intelligence* 150 (2003) 183–237, doi:10.1016/S0004-3702(03)00105-X [V]. A factor hierarchy and argument moves that emphasise or downplay distinctions.
- IBP: Brüninghaus and Ashley, ICAIL 2003, doi:10.1145/1047788.1047838 [V]; Ashley and Brüninghaus, "Automatically classifying case texts and predicting outcomes", *AI & Law* 17 (2009) 125–165, doi:10.1007/s10506-009-9077-9 [V] (SMILE+IBP: factors found in text, then issue-based prediction).
- VJAP (Grabmair 2017, above).
- ANGELIC: Al-Abdulkarim, Atkinson and Bench-Capon, "A methodology for designing systems to reason with legal cases using abstract dialectical frameworks", *AI & Law* 24 (2016) 1–49, doi:10.1007/s10506-016-9178-1 [V]; Mumford, Atkinson and Bench-Capon, ICAIL 2023, 167–176, doi:10.1145/3594536.3595158 [V] (an ANGELIC knowledge model combined with machine learning for factor ascription).
- Precedential constraint beyond the reason model we hold: Horty, *Legal Theory* 17 (2011) 1–33, doi:10.1017/S1352325211000036 [V]; Horty and Bench-Capon, *AI & Law* 20 (2012) 181–214, doi:10.1007/s10506-012-9125-8 [V]; Rigoni, "Representing dimensions within the reason model of precedent", *AI & Law* 26 (2018) 1–22, doi:10.1007/s10506-017-9216-7 [V]; Horty, "Reasoning with dimensions and magnitudes", *AI & Law* 27 (2019) 309–345, doi:10.1007/s10506-019-09245-0 [V resolves]; Bench-Capon and Atkinson, "Dimensions and values for legal CBR", JURIX 2017, doi:10.3233/978-1-61499-838-9-27 [V]; Prakken, "A formal analysis of some factor- and precedent-based accounts of precedential constraint", *AI & Law* 29 (2021) 559–585, doi:10.1007/s10506-021-09284-6 [V]; Canavotto, "Precedential constraint derived from inconsistent case bases", JURIX 2022, doi:10.3233/faia220445 [V] (no consistency assumption); Canavotto 2023 on hierarchies (C.3); van Woerkom, Grossi, Prakken and Verheij, "Landmarks in case-based reasoning: from theory to data", HHAI 2022, doi:10.3233/faia220200 [V], and "A fortiori case-based reasoning: from theory to data", *JAIR* 81 (2024) 401–441, doi:10.1613/jair.1.15178 [V] (an implementation that computes with the definitions; which datasets suit the theory); Prakken and Ratsma, "A top-level model of case-based argumentation for explanation", *Argument & Computation* 13 (2022) 159–194, doi:10.3233/aac-210009 [V]; Peters et al., "Model- and data-agnostic justifications with a fortiori case-based argumentation", ICAIL 2023, doi:10.1145/3594536.3595164 [V]; Prakken and Sartor, "Modelling reasoning with precedents in a formal dialogue game", *AI & Law* 6 (1998) 231–287, doi:10.1023/A:1008278309945 [V].

*Implementations:* no maintained public implementation of HYPO, CATO or IBP was found [M]. *Beyond what we hold:* dimensions and magnitudes; case bases that are inconsistent (as real ones are); hierarchies of intermediate concepts; explanation by case-based argument; constraint read from data.

### D.2 Argument Models with Burdens and Standards

- Carneades: Gordon, Prakken and Walton, "The Carneades model of argument and burden of proof", *Artificial Intelligence* 171 (2007) 875–896, doi:10.1016/j.artint.2007.04.010 [V]; Gordon and Walton, "Proof burdens and standards", in *Argumentation in AI* (2009) 239–258, doi:10.1007/978-0-387-98197-0_12 [V] (scintilla, preponderance, clear and convincing evidence, beyond reasonable doubt, dialectical validity, as acceptability conditions on weighed pro and con arguments). Implementations: carneades-4 (Go), **MPL-2.0** [V]; carneades-3, no licence declared [V].
- The Argument Interchange Format: Chesñevar et al., *Knowledge Engineering Review* 21 (2006) 293–316, doi:10.1017/S0269888906001044 [V].
- Prakken and Sartor, "Law and logic: a review from an argumentation perspective", *Artificial Intelligence* 227 (2015) 214–245, doi:10.1016/j.artint.2015.06.005 [V].

### D.3 Defeasible Deontic Logic and Contrary-to-Duty Obligations

- Governatori, "Representing business contracts in RuleML", *IJCIS* 14 (2005) 181–216, doi:10.1142/S0218843005001092 [V]; Governatori and Rotolo, "Logic of violations: a Gentzen system for reasoning with contrary-to-duty obligations", *Australasian J. Logic* 4 (2006), doi:10.26686/ajl.v4i0.1780 [V resolves] (the ⊗ operator: *A ⊗ B* reads "A is obligatory, and if A is violated, B is"); Governatori, Olivieri, Rotolo and Scannapieco, "Computing strong and weak permissions in defeasible logic", *J. Phil. Logic* 42 (2013) 799–829, doi:10.1007/s10992-013-9295-1 [V]; Governatori, "Practical normative reasoning with defeasible deontic logic", Reasoning Web 2018, LNCS, 1–25, doi:10.1007/978-3-030-00338-8_1 [V].
- Reasoners. **SPINdle**: Lam and Governatori, "The making of SPINdle", RuleML 2009, doi:10.1007/978-3-642-04985-9_29 [V]; LGPL (SourceForge says v2, CSIRO says v3) [S]; ports to Rust and Racket are reported [S]. **Turnip**, a typed functional implementation of defeasible deontic logic, used in Witt et al., "Encoding legislation: a methodology for enhancing technical validation, legal alignment and interdisciplinarity", *AI & Law* 32 (2024) 293–324, doi:10.1007/s10506-023-09350-1 [V paper]; its public availability and licence were not found [S]. **Defeasible deontic logic in ASP** (gvdgdo/Defeasible-Deontic-Logic), no licence declared [V]. **Houdini** [S].
- Contrary-to-duty foundations: Chisholm, *Analysis* 24 (1963) 33–36, doi:10.1093/analys/24.2.33 [V]; Prakken and Sergot, *Studia Logica* 57 (1996) 91–115, doi:10.1007/BF00370671 [V]; Carmo and Jones, "Deontic logic and contrary-to-duties", *Handbook of Philosophical Logic* vol. 8 (2002) 265–343, doi:10.1007/978-94-010-0387-2_4 [V]; Makinson and van der Torre, "Input/output logics", *J. Phil. Logic* 29 (2000) 383–408, doi:10.1023/A:1004748624537 [V].
- LogiKEy: Benzmüller, Parent and van der Torre, "Designing normative theories for ethical and legal reasoning: LogiKEy framework, methodology, and tool support", *Artificial Intelligence* 287 (2020) 103348, doi:10.1016/j.artint.2020.103348 [V]: normative logics embedded in Isabelle/HOL, with a GDPR example involving a contrary-to-duty obligation [M for the example's article]; repository cbenzmueller/LogiKEy, no licence declared [V].

*Beyond what we hold:* apodeixis has dyadic obligation keeping Chisholm's sentences. Defeasible deontic logic adds the ⊗ chains of reparation (which is how GDPR obligations and their remedies read: process lawfully; if not, notify, erase, compensate), strong and weak permission, and linear-time computation of extensions.

### D.4 Compliance Checking

- Hashmi, Governatori, Lam and Wynn, "Are we done with business process compliance: state of the art and challenges ahead", *KAIS* 57 (2018) 79–133, doi:10.1007/s10115-017-1142-1 [V]; Ly et al., "Compliance monitoring in business processes: functionalities, application, and tool-support", *Information Systems* 54 (2015) 209–234, doi:10.1016/j.is.2015.02.007 [V].
- Governatori, "The Regorous approach to process compliance", EDOCW 2015, 33–40, doi:10.1109/EDOCW.2015.28 [V]; Governatori et al., "Semantic business process regulatory compliance checking using LegalRuleML", EKAW 2016, LNCS, 746–761, doi:10.1007/978-3-319-49004-5_48 [V]; Palmirani and Governatori, "Modelling legal knowledge for GDPR compliance checking", JURIX 2018, doi:10.3233/978-1-61499-935-5-101 [V] (GDPR, BPMN and the Regorous engine). Regorous's source was not found publicly [M].
- Robaldo et al., "Compliance checking on first-order knowledge with conflicting and compensatory norms: a comparison among currently available technologies", *AI & Law* 32 (2024) 505–555, doi:10.1007/s10506-023-09360-z [V]: one use case in several freely available reasoners, compared on shared synthetic data; concludes that much remains to integrate their benefits in one first-order framework; code at liviorobaldo/compliancecheckers, **MIT** [V].
- Francesconi and Governatori, "Patterns for legal compliance checking in a decidable framework of linked open data", *AI & Law* 31 (2023) 445–464, doi:10.1007/s10506-022-09317-8 [V]: deontic norms as OWL 2 classes and property restrictions, defeasibility handled, decidable reasoners.
- Temporal monitoring: Basin, Klaedtke, Müller and Zălinescu, "Monitoring metric first-order temporal properties", *JACM* 62 (2015), doi:10.1145/2699444 [V]; the MonPoly monitor (OCaml; repository moved to Bitbucket; licence not verified [S]).

### D.5 Logic Programming and Rules as Code

- Sergot et al., "The British Nationality Act as a logic program", *CACM* 29 (1986) 370–386, doi:10.1145/5689.5920 [V].
- **Catala**: Merigoux, Chataing and Protzenko, "Catala: a programming language for the law", *Proc. ACM Program. Lang.* 5 (ICFP 2021) article 77, doi:10.1145/3473582 [V]; Huttner and Merigoux, *AI & Law* (2022), doi:10.1007/s10506-022-09328-5 [V]; **Apache-2.0** [V]. Literate programming against the statute's text; default logic for exceptions; compiles to several languages.
- **s(CASP)**: Arias et al., "Constraint answer set programming without grounding", *TPLP* 18 (2018) 337–354, doi:10.1017/S1471068418000285 [V]; **Apache-2.0** (SWI-Prolog/sCASP) [V]; goal-directed ASP that returns a justification tree for each answer. **Blawx** on it: Morris, ICAIL 2021, doi:10.1145/3462757.3466084 [V]; **MIT** [V].
- **clingo**, **MIT** [V].
- **Logical English**: Kowalski, Dávila, Sartor and Calejo, "Logical English for law and education", LNCS (2023) 287–299, doi:10.1007/978-3-031-35254-6_24 [V]; implementation location and licence not verified.
- **L4** (smucclaw/dsl), Apache-2.0 per its LICENCE file [V]; **OpenFisca**, AGPL-3.0 [V].
- Lawsky, "A logic for statutes", 21 Fla. Tax Rev. (2017), doi:10.5744/ftr.2017.0002 [V] (statutes as default logic).
- Witt et al. 2024 (D.3) report that coders of the same provisions diverge on syntactic ambiguity, intra- and intertextuality and factual indeterminacy, which needed legal evaluation beyond coding validation: an empirical warning that encoding is interpretation.

### D.6 Standards for Rules and Legal Knowledge

- **LegalRuleML Core Specification Version 1.0**, OASIS Standard, 30 August 2021 [V] (https://docs.oasis-open.org/legalruleml/legalruleml-core-spec/v1.0/): RuleML extended with defeasibility, deontic operators, sources, temporal parameters, jurisdiction and authority; XSD and RelaxNG schemas; RF on Limited Terms.
- **Akoma Ntoso** (B.1).
- **LKIF Core**: Hoekstra, Breuker, Di Bello and Boer, "The LKIF Core ontology of basic legal concepts", LOAIT 2007 (CEUR-WS vol. 321) [M]; repository RinkeHoekstra/lkif-core, no licence declared [V].

### D.7 How it Sits behind an Interface

A **Reasoner** declares its theory language (a calculus in apodeixis's sense, a defeasible deontic theory, an ASP program, a Catala program, an OWL 2 ontology, an MFOTL formula, a case base), takes a theory and a query, and answers with a verdict and its evidence: a derivation, a justification tree, a countermodel, an argument graph, a violation trace. A checker of the evidence, apart from the reasoner, accepts or refuses it. External engines (SPINdle, s(CASP), clingo, Catala's compiler, MonPoly, SPECIAL's engine) sit behind adaptors; the studio's own engines stay the reference.

---

## E. Formalisations of Privacy and Data-Protection Law

### E.1 The Instruments of the First Body of Law

- GDPR Chapter V (Articles 44–50); Commission Implementing Decision (EU) 2021/914 of 4 June 2021 on standard contractual clauses for transfers [M]; CJEU, C-311/18 *Facebook Ireland and Schrems* (*Schrems II*), ECLI:EU:C:2020:559 [M]; EDPB Recommendations 01/2020 on supplementary measures (version 2.0, 18 June 2021) and 02/2020 on the European Essential Guarantees [M]; EDPB Guidelines 2/2018 on Article 49 derogations [M].
- CNIL, *Transfer Impact Assessment: practical guide*, final version January 2025 [S] (https://www.cnil.fr/sites/cnil/files/2025-01/guide_tia.pdf), a stepwise method.
- Commission Implementing Decision (EU) 2023/1795 (EU–US Data Privacy Framework); the General Court dismissed the action for annulment in *Latombe v Commission*, T-553/23, 3 September 2025 (ECLI:EU:T:2025:831) [S]; an appeal's status not checked.
- Convention 108+ (Protocol CETS No. 223): 34 ratifications after the Republic of Moldova's, reported at the T-PD plenary of 9–11 June 2026; entry into force needs 38; not in force on the evidence found [S] (https://www.coe.int/en/web/data-protection/).
- OECD Privacy Guidelines (OECD/LEGAL/0188, revised 2013) [S]; OECD Declaration on Government Access to Personal Data Held by Private Sector Entities (OECD/LEGAL/0487, 14 December 2022), not legally binding, seven principles [S].
- Article 17 ICCPR with Human Rights Committee General Comment No. 16 (1988) [M]; Article 8 ECHR with the Registry's *Guide on Article 8* and *Guide to the case-law: data protection* [M]; APEC Privacy Framework (2015) and the Cross-Border Privacy Rules, now also the Global CBPR Forum [M].

None of the international instruments outside the EU has a machine-readable formalisation that was found.

### E.2 The W3C Data Privacy Vocabulary (DPV)

- Pandit et al., "Creating a vocabulary for data privacy", ODBASE 2019, LNCS, 714–730, doi:10.1007/978-3-030-33246-4_44 [V]; Pandit, Esteves, Krog, Ryan, Golpayegani and Flake, "Data Privacy Vocabulary (DPV) – Version 2.0", ISWC 2024, 171–193 (arXiv:2404.13426) [S].
- The specification is a W3C Community Group final report, not a W3C standard: version 2.0 (1 August 2024), version 2.3 (25 February 2026) [V] (https://w3id.org/dpv). Licence: W3C Software and Document License (repository LICENCE file) [V].
- The GDPR extension (`eu-gdpr`, https://w3id.org/dpv/legal/eu/gdpr#) [V] encodes Chapter V's tools as concepts: `A45-3` and `AdequacyDecision`; `A46-2-a` to `A46-2-f` (public-body instruments, BCRs, Commission SCCs, authority SCCs, codes of conduct, certification); `A46-3-a`, `A46-3-b`; `A49-1-a` to `A49-1-g` and `A49-2`; classes `DataTransferTool`, `StandardContractualClauses`, `SCCByCommission`, `SCCBySupervisoryAuthority`, `AdHocContractualClauses`, `BindingCorporateRules`, `SupplementaryMeasure`. It also encodes the legal bases of Articles 6(1) and 9(2), the rights of Articles 15–22, DPIA concepts and compliance statuses (`GDPRCompliant`, `GDPRNonCompliant`, `GDPRComplianceUnknown`). The core vocabulary has `dpv:DataTransferImpactAssessment` in its risk module [S].
- Neighbours: Ryan, Brennan and Pandit, "DPCat", *Information* 13 (2022) 244, doi:10.3390/info13050244 [V] (a machine-readable register of processing activities); Pandit, "A semantic specification for data protection impact assessments (DPIA)", *Studies on the Semantic Web* (2022), doi:10.3233/ssw220007 [V].
*Computable:* classification and validation of records of transfers (with SHACL shapes in symploke), not legal reasoning. DPV names *which* tool a transfer relies on; it does not say whether the tool suffices.

### E.3 ODRL

- ODRL Information Model 2.2 and Vocabulary & Expression 2.2, W3C Recommendations (February 2018) [M]. The Community Group's formal semantics was still a natural-language draft in August 2025 [S] (https://w3c.github.io/odrl/formal-semantics/); Bonatti, Fornara and Harth proposed a declarative semantics for ODRL 2.2 at OPAL 2025 (CEUR-WS vol. 3977) [S].
- De Vos, Kirrane, Padget and Satoh, "ODRL policy modelling and compliance checking", RuleML+RR 2019, LNCS, 36–51, doi:10.1007/978-3-030-31095-0_3 [V]: an ODRL profile for business policies and regulatory requirements, translated through InstAL into ASP for compliance checking with explanation of non-compliance, with GDPR elements modelled [S for content].
- Agarwal, Steyskal, Antunović and Kirrane, "Legislative compliance assessment: framework, model and GDPR instantiation", APF 2018, LNCS, 131–149, doi:10.1007/978-3-030-02547-2_8 [V].
- Esteves and Rodríguez-Doncel, "Analysis of ontologies and policy languages to represent information flows in GDPR", *Semantic Web* 15 (2024) 709–743, doi:10.3233/SW-223009 [V]: 57 informational items in GDPR rights and obligations; 13 policy languages and 9 vocabularies compared; ODRL and LegalRuleML complemented by DPV and GDPRtEXT model 39 of the 57.
- Licence of the ODRL repository: W3C Software and Document License [V].

### E.4 PrOnto

Palmirani, Martoni, Rossi, Bartolini and Robaldo, "PrOnto: Privacy ontology for legal reasoning", EGOVIS 2018, LNCS 11032, 139–152, doi:10.1007/978-3-319-98349-3_11 [V]; "Legal ontology for modelling GDPR concepts and norms", JURIX 2018, doi:10.3233/978-1-61499-935-5-91 [V]. OWL, in five modules: documents and data; actors and roles; processing and workflow; legal rules and deontic formulae; purposes and legal bases [S]. No public repository or licence for the ontology files was found [S].

### E.5 The DAPRECO Knowledge Base

Robaldo, Bartolini, Palmirani, Rossi, Martoni and Lenzini, "Formalizing GDPR provisions in reified I/O logic: the DAPRECO knowledge base", *J. Logic, Language and Information* 29 (2020) 401–449, doi:10.1007/s10849-019-09309-z [V]; Robaldo, Bartolini and Lenzini, "The DAPRECO knowledge base: representing the GDPR in LegalRuleML", LREC 2020, 5688–5697 (https://aclanthology.org/2020.lrec-1.698/) [V]. GDPR provisions as if-then rules (constitutive rules as first-order implications; obligations and permissions as reified I/O logic pairs) on PrOnto's concepts, codified in LegalRuleML; the largest freely available LegalRuleML base by its authors' account. Repository dapreco/daprecokb (`gdpr/rioKB_GDPR.xml`; also `iso27018/` and a GDPR–ISO 27018 mapping), **no licence declared** [V]; the number of formulae was not found in the sources read.

### E.6 SPECIAL

- Bonatti, "Fast compliance checking in an OWL2 fragment", IJCAI 2018, 1746–1752, doi:10.24963/ijcai.2018/241 [V]: a company's policy, a data subject's consent and part of the GDPR as class expressions in an OWL 2 fragment; compliance as subsumption; a complete and tractable structural subsumption algorithm, and intractability of a natural generalisation.
- Bonatti, Kirrane, Petrova and Sauro, "Machine understandable policies and GDPR compliance checking", *KI* 34 (2020) 303–315, doi:10.1007/s13218-020-00677-4 [V]; Bonatti et al., "Transparent personal data processing: the road ahead", LNCS (2017) 337–349, doi:10.1007/978-3-319-66284-8_28 [V]; Kirrane et al., "A scalable consent, transparency and compliance architecture", ESWC 2018 (demo), doi:10.1007/978-3-319-98192-5_25 [V].
- Code: GitHub organisation `specialprivacy`, repositories `compliance-checker` and `SPECIAL-compliance-engine` ("specialized reasoner for compliance checking over SPECIAL's policies"), **Apache-2.0** [V].

### E.7 GDPRtEXT and Related Linked Data

Pandit, Fatema, O'Sullivan and Lewis, "GDPRtEXT – GDPR as a linked data resource", ESWC 2018, LNCS, 481–495, doi:10.1007/978-3-319-93417-4_31 [V]: the GDPR's text as linked data with a SKOS glossary of its concepts; repository coolharsh55/GDPRtEXT, no licence declared in metadata [V] (CC BY 4.0 by the project's statement [M]).

### E.8 Temporal Monitoring of GDPR Obligations

Arfelt, Basin and Debois, "Monitoring the GDPR", ESORICS 2019, LNCS 11735, 681–699, doi:10.1007/978-3-030-29959-0_33 [V]: Articles 5(1)(c),(e), 6(1), 7(3), 13(1), 15(1), 17(1–2), 18(1–2) and 21(1) in metric first-order temporal logic, checked by MonPoly on industrial logs, with actual violations found [S for the article list]. Hublet, Basin and Krstić, "Enforcing the GDPR", ESORICS 2023, LNCS (2024) 400–422, doi:10.1007/978-3-031-51476-0_20 [V]. *Remark.* Chapter V is not among the formalised articles.

### E.9 Formalisations of United States Privacy Law

Barth, Datta, Mitchell and Nissenbaum, "Privacy and contextual integrity: framework and applications", IEEE S&P 2006, doi:10.1109/SP.2006.32 [V] (norms of information flow in a temporal logic, applied to HIPAA, COPPA and GLBA); DeYoung et al., "Experiences in the logical specification of the HIPAA and GLBA privacy laws", WPES 2010, 73–82, doi:10.1145/1866919.1866930 [V]; Garg, Jia and Datta, "Policy auditing over incomplete logs: theory, implementation and applications", CCS 2011, 151–162, doi:10.1145/2046707.2046726 [V] (an algorithm that checks what the log decides and returns a residual policy for what it does not); Sen et al., "Bootstrapping privacy compliance in big data systems" (Legalease and Grok, at Bing), IEEE S&P 2014, 327–342 [S; DOI not verified].
*Beyond what we hold:* Garg et al.'s residual formula is the logical shape of counsel's position with an incomplete record: what the evidence settles, and exactly what remains to be proved.

### E.10 Transfers and Impact Assessments: the Gap

No ontology, formal model or knowledge base of transfer impact assessments was found [S, after targeted searches]. What exists: DPV's vocabulary of transfer tools and the concept of a data transfer impact assessment (E.2); the EDPB's roadmap and the CNIL's stepwise guide; Clause 14 of the 2021 SCCs [M]. The structure is clear enough to formalise, and it is an argument, not a lookup:

1. *The transfer as facts*: exporter, importer, onward recipients, data categories, purposes, the destination (DPV and the law domain's facts, each with its source and grade).
2. *The tool*: Article 45, else Article 46, else Article 49 restrictively, a declared order (a priority among rules).
3. *The destination's law and practice against the European Essential Guarantees*: four open terms (clear, precise and accessible rules; necessity and proportionality; independent oversight; effective remedies) [M for the formulation], each read by interpreters (precedent: *Schrems II*, *Latombe*; EDPB recommendations; reports on the destination's law graded as evidence through Schum's chain of credibility).
4. *Supplementary measures*: technical, contractual, organisational, each effective or not *under a scenario* (e.g. strong encryption with keys held in the EEA is effective against access in transit; not where the importer must process in clear) [M for the EDPB's use cases].
5. *Conclusion under a standard*: whether the essential equivalence the transfer needs is shown, with the open terms' readings as labels, i.e. Γ ⊢_ι φ with D(φ) clear, arguable or untenable.

This fits the infrastructure's existing pieces exactly; what is missing is the scheme as data and a body of decided cases (CJEU, General Court, EDPB binding decisions, national DPA decisions on Chapter V) as a case base.

### E.11 How it Sits behind an Interface

Privacy formalisations divide into three roles: **vocabulary** (DPV, GDPRtEXT, PrOnto's concepts) loaded as data by symploke; **policy languages** (ODRL, SPECIAL) as Reasoners with their own semantics (subsumption, ASP); **norm bases** (DAPRECO in LegalRuleML, Palmirani and Governatori's GDPR rules) as theories a Reasoner reads. The studio should keep its own representation of the norm (the law domain's norms with exceptions and counts-as links) and *import* or *export* LegalRuleML and DPV, never make either its internal model.

---

## F. Evidential Reasoning beyond what we Hold

### F.1 Bayesian Networks of Legal Evidence

- Held: Fenton, Neil and Lagnado, "A general structure for legal arguments about evidence using Bayesian networks", *Cognitive Science* 37 (2013) 61–102, doi:10.1111/cogs.12004 [V].
- Lagnado, Fenton and Neil, "Legal idioms: a framework for evidential reasoning", *Argument & Computation* 4 (2013) 46–63, doi:10.1080/19462166.2012.682656 [V]; Fenton, Neil and Berger, "Bayes and the law", *Annu. Rev. Stat. Appl.* 3 (2016) 51–77, doi:10.1146/annurev-statistics-041715-033428 [V].
- **Competing arguments compared:** Neil, Fenton, Lagnado and Gill, "Modelling competing legal arguments using Bayesian model comparison and averaging", *AI & Law* 27 (2019) 403–430, doi:10.1007/s10506-019-09250-3 [V]: the prosecution's and defence's networks as rival models, compared by Bayes factors and averaged, instead of forcing one network on both sides.
- **The Simonshaven case** as a common test: Fenton et al., *Topics in Cognitive Science* 12 (2020) 1092–1114, doi:10.1111/tops.12417 [V resolves]; Verheij, ibid. 1175–1199, doi:10.1111/tops.12436 [V]; Prakken, ibid. 1068–1091, doi:10.1111/tops.12418 [V]; Dahlman, "De-biasing legal fact-finders with Bayesian thinking", ibid. 1115–1131, doi:10.1111/tops.12419 [V].
- Hepler, Dawid and Leucari, "Object-oriented graphical representations of complex patterns of evidence", *Law, Probability and Risk* 6 (2007) 275–293, doi:10.1093/lpr/mgm005 [V resolves] (object-oriented networks from Wigmore charts); Taroni et al., *Bayesian Networks for Probabilistic Inference and Decision Analysis in Forensic Science* (2nd edn, Wiley 2014), doi:10.1002/9781118914762 [V resolves].
- Software: pgmpy, **MIT** [V]; AgenaRisk is commercial [M].

### F.2 Hybrid Argument–Scenario Models

- Bex, van Koppen, Prakken and Verheij, "A hybrid formal theory of arguments, stories and criminal evidence", *AI & Law* 18 (2010) 123–152, doi:10.1007/s10506-010-9092-x [V]; Bex, *Arguments, Stories and Criminal Evidence* (Springer 2011), doi:10.1007/978-94-007-0140-3 [V]; Bex et al., "Sense-making software for crime investigation: how to combine stories and arguments?" (AVERs), *Law, Probability and Risk* 6 (2007) 145–168, doi:10.1093/lpr/mgm007 [V].
- Verheij, "To catch a thief with and without numbers", *Law, Probability and Risk* 13 (2014) 307–325, doi:10.1093/lpr/mgu011 [V]; Verheij, "Proof with and without probabilities", *AI & Law* 25 (2017) 127–154, doi:10.1007/s10506-017-9199-4 [V] (case models: a preference order over mutually exclusive cases from which coherent, presumptively valid and conclusive arguments are defined, so that argument, scenario and probability have one semantics); Verheij et al., "Arguments, scenarios and probabilities: connections between three normative frameworks for evidential reasoning", *Law, Probability and Risk* 15 (2016) 35–70, doi:10.1093/lpr/mgv013 [V].
- Vlek, Prakken, Renooij and Verheij, "Building Bayesian networks for legal evidence with narratives: a case study evaluation", *AI & Law* 22 (2014) 375–421, doi:10.1007/s10506-014-9161-7 [V] (scenario idioms, merged scenarios); Timmer et al., "A two-phase method for extracting explanatory arguments from Bayesian networks", *IJAR* 80 (2017) 475–494, doi:10.1016/j.ijar.2016.09.002 [V]; Bex and Renooij, "From arguments to constraints on a Bayesian network", COMMA 2016, doi:10.3233/978-1-61499-686-6-95 [V]; Wieten, Bex, Prakken and Renooij, JURIX 2018, doi:10.3233/978-1-61499-935-5-151 [V] and ECSQARU 2019, doi:10.1007/978-3-030-29765-7_9 [V] (network graphs constructed from labelled arguments).
- Prakken, "Analysing reasoning about evidence with formal models of argumentation", *Law, Probability and Risk* 3 (2004) 33–50, doi:10.1093/lpr/3.1.33 [V].

*Beyond what we hold:* tekmerion has scenarios against a chart and networks of idioms. The literature adds (i) a common semantics of the three (Verheij's case models), (ii) networks built from arguments rather than by hand (Bex and Renooij; Wieten et al.), (iii) explanations read back out of a network as arguments (Timmer et al.), and (iv) rival networks compared rather than one imposed (Neil et al.).

### F.3 Standards of Proof Formalised

- Probability thresholds: Kaplan, "Decision theory and the fact-finding process", 20 Stan. L. Rev. 1065 (1968) [M] (the threshold from the relative disutilities of the two errors); Cheng, "Reconceptualizing the burden of proof", 122 Yale L.J. 1254 (2013) [M] (a comparative likelihood ratio of the parties' stories).
- Explanation and plausibility: Allen and Pardo, "Relative plausibility and its critics", *Int. J. Evidence & Proof* 23 (2019) 5–59, doi:10.1177/1365712718813781 [V]; Urbaniak, "Narration in judiciary fact-finding: a probabilistic explication", *AI & Law* 26 (2018) 345–376, doi:10.1007/s10506-018-9219-z [V].
- Weight and normic support: Nance, *The Burdens of Proof* (CUP 2016), doi:10.1017/CBO9781316415771 [V] (Keynesian weight: the completeness of the evidence as distinct from its balance); Smith, "When does evidence suffice for conviction?", *Mind* 127 (2018) 1193–1218, doi:10.1093/mind/fzx026 [V] (normic support: a conclusion is supported if its falsity would need explaining).
- Argument-based: Gordon and Walton 2009 (D.2); Prakken and Sartor, "A logical analysis of burdens of proof", in Kaptein, Prakken and Verheij (eds), *Legal Evidence and Proof: Statistics, Stories, Logic* (Ashgate 2009) [M; no DOI]; Farley and Freeman, "Burden of proof in legal argumentation", ICAIL 1995, doi:10.1145/222092.222258 [V resolves].
*Beyond what we hold:* the law domain declares standards as a probability, an argument's standing or both. Nance's weight suggests a third dimension worth recording: whether the record is complete enough for any standard to be applied (a court may refuse to decide on thin evidence), which connects to TAR's certified recall and to Garg et al.'s residual formula.

### F.4 Evaluative Opinions on Digital Evidence

Casey, "Standardization of forming and expressing preliminary evaluative opinions on digital evidence", *Forensic Science International: Digital Investigation* 32 (2020) 200888, doi:10.1016/j.fsidi.2019.200888 [V]; ENFSI *Guideline for Evaluative Reporting in Forensic Science* (2015) [M]; *R v T* [2010] EWCA Crim 2439 on likelihood ratios before juries [M]. In privacy litigation most evidence is digital (logs, transfer records, contracts, configurations), so the form in which an expert states its weight matters as much as the weight.

### F.5 How it Sits behind an Interface

An **EvidenceModel** (in tekmerion's sense) declares what it takes (a chart, a scenario set, a network, a case model) and returns a weighing with its evidence: posterior odds with the network and its sensitivity; a case model's preference order with the arguments it makes valid; rival models' Bayes factor. Model comparison (Neil et al.) and explanation extraction (Timmer et al.) are operations on any network, so they belong beside tyche's `graphical`, not inside one model.

---

## G. Recommendations for an Extensible Design

### G.1 The Interfaces

Four pluggable contracts, each answering with a candidate and its evidence for a separate checker:

| Contract | Takes | Answers | Checked by |
|---|---|---|---|
| **Recogniser** | a holding (digest), a span | candidates with offsets, normalised value, method, grade | offset match, register resolution, SHACL shapes |
| **Interpreter** (exists) | an open term, a norm, a case | readings, with the argument scheme instance that yields each | apodeixis (D(φ)), the scheme's critical questions |
| **Reasoner** | a theory in a declared language, a query | verdict with derivation, justification, countermodel or trace | a checker per evidence kind |
| **EvidenceModel** and **ReviewProtocol** | a chart, scenarios, a network; a collection and a protocol | weighing with sensitivity; ranking, stopping decision and certified recall | closed forms; a sample drawn apart from the ranker |

### G.2 What to Implement First, for Practice

In order of value to counsel per unit of work, all computable, testable against closed forms or public data, and free of licence trouble:

1. **Citation and quotation verification** (B.2, B.7). ECLI, CELEX, ELI, ECtHR application numbers and neutral citations parsed by grammars on morphe's BNF reader; resolution against EUR-Lex/CELLAR and HUDOC at the edge; every quotation matched to its holding with offsets. This is the guard against the failure that sanctions counsel (*Mata*, *Ayinde*), and it costs little.
2. **Processing of a collection** (A.4): family-aware deduplication, near-duplicate clusters by MinHash with the estimator's variance, e-mail threads from RFC 5322 headers with heuristic links marked as inferred; export of custody as CASE/UCO; a hash certificate in the form FRE 902(14) contemplates; external time-stamps and renewable evidence records (RFC 3161, RFC 4998) on archeion's Merkle trees.
3. **Technology-assisted review with certified recall** (A.5): CAL with a logistic-regression ranker as the reference; stopping rules with stated guarantees (Lewis et al. 2021; Callaghan and Müller-Hansen 2020); recall intervals by Webber's beta-binomial method; elusion tests on peira's estimators. Shared with tekmerion's systematic reviews; reproduced on the TREC Total Recall and CLEF TAR collections.
4. **Legal hold and privilege log** (A.3, A.6): a hold as archeion's retention override with Sedona's twelve guidelines as a declared scheme of steps on the docket; a privilege log as a projection of metadata with the consistency checks of A.6.
5. **Transfers as a structured argument** (E.10): DPV's `eu-gdpr` concepts as the vocabulary of transfer facts (loaded by symploke, SHACL shapes for a complete transfer record); the EDPB/CNIL steps as a scheme in tekmerion; the European Essential Guarantees as open terms read by interpreters; *Schrems II*, *Latombe* and EDPB decisions as the first case base; the conclusion as D(φ). Nothing comparable exists in the literature, so this is also research.
6. **A catalogue of interpretive argument schemes** (C.1, C.2, C.6): MacCormick–Summers's eleven types and the VCLT's rules with their critical questions and a declared ordering per legal order; the ECtHR's and CJEU's canons added as schemes, each instance attached to its source.
7. **Precedent beyond the reason model** (D.1): dimensions and magnitudes (Horty 2019, Rigoni 2018), inconsistent case bases (Canavotto 2022), hierarchies of intermediate concepts (Canavotto 2023), a fortiori reasoning read from data (van Woerkom et al. 2024), explanation by case-based argument (Prakken and Ratsma 2022).
8. **Defeasible deontic logic with reparation chains** (D.3) as a calculus declared in apodeixis, our own implementation as reference (Governatori and Rotolo's ⊗; strong and weak permission), so that GDPR obligations and their remedies read as they are written.
9. **Case models and rival networks** (F.2, F.1): Verheij's case models on tekmerion's scenarios; Neil et al.'s comparison and averaging of the parties' networks and Timmer et al.'s explanation extraction beside tyche's `graphical`.

### G.3 What to Put behind Interfaces (Borrowed, Pluggable)

- **Recognisers:** Habernal et al.'s ECtHR argument model (Apache-2.0); Blackstone (Apache-2.0) for UK text; rhetorical-role and NER models trained on the corpora of B.3–B.4; language-model extractors with offset grounding (LangExtract's design, Apache-2.0); Apache Tika and OCR for text.
- **Reasoners:** SPINdle (LGPL, as a separate process), s(CASP) (Apache-2.0), clingo (MIT), Catala's compiler (Apache-2.0), SPECIAL's compliance engine (Apache-2.0), MonPoly (licence to confirm), Carneades 4 (MPL-2.0, file-level copyleft, acceptable behind a process boundary).
- **Evidence engines:** pgmpy (MIT) as a cross-check of tyche's networks.
- **Forensic readers:** libewf (LGPL-3.0, as a library or a process), pyaff4 (Apache-2.0), Plaso (Apache-2.0).
- **Registers at the edge:** CourtListener's citation API, EUR-Lex/CELLAR, HUDOC, GDPRhub (after its licence is confirmed).

### G.4 What to Leave, or Defer

- **Outcome prediction** from judgment text (LexGLUE's ECtHR tasks and the like) as a tool for counsel: weak construct validity (it predicts from the court's own statement of facts), and ethically fraught. Keep it out of the studio's reasoning; it may serve research elsewhere.
- **HYPO, CATO and IBP re-implemented as they were**: superseded for our purposes by the reason model and its extensions; take their factor hierarchies and argument moves as data.
- **LKIF Core**: dormant, no licence declared; mine for concepts only.
- **DAPRECO and PrOnto wholesale**: no licence declared or found; read them as a check on our own encoding of GDPR provisions, and ask the authors before reusing.
- **LegalRuleML and Akoma Ntoso as internal models**: use them for import and export only; LegalRuleML's semantics is left to the reasoner it is given to.
- **ODRL as a logic of law**: use it to carry data-sharing agreements and consent policies; reason about legal compliance in the studio's own norms.
- **AGPL components** (LexNLP, Datashare, OpenFisca): avoid linking; if ever used, only as separate network services, with the obligation understood.
- **A language model as interpreter without a panel**: the evidence (C.5) is that single answers are unstable; never report one reading without its spread across paraphrases and formats.

### G.5 Licences at a Glance

| Permissive (MIT, BSD, Apache-2.0) | Weak copyleft (LGPL, MPL) | Strong copyleft (GPL, AGPL) | None declared or not found |
|---|---|---|---|
| Catala, eyecite, reporters-db, TARexp, LangExtract, Blackstone, s(CASP), Blawx, clingo, pgmpy, CASE, UCO, Plaso, Tika, pyaff4, Aleph, SPECIAL's repositories, compliancecheckers, CLEF TAR, Habernal's legal-arguments, L4 | SPINdle (LGPL), libewf (LGPL-3.0), Carneades 4 (MPL-2.0) | HiCAL (GPL-3.0), LexNLP, Datashare, OpenFisca (AGPL-3.0) | DAPRECO, PrOnto, LKIF Core, LogiKEy, GDPRtEXT (metadata), LegalBench and LexGLUE code, CUAD repository, Carneades 3, DDL in ASP, Turnip, MonPoly (unverified), The Sleuth Kit (mixed) |

DPV and ODRL are under the W3C Software and Document License; LegalRuleML and Akoma Ntoso under OASIS's IPR policy; EDRM's diagram and TAR Guidelines under Creative Commons.

### G.6 Open Questions for Research

1. A formal model of transfer impact assessments (E.10), with a case base of Chapter V decisions.
2. A mapping between instruments (GDPR, Convention 108+, APEC CBPR, OECD Guidelines and the 2022 Declaration) as relations between norms with their differences stated, for which no machine-readable treatment was found.
3. VCLT Articles 31–33 as a computational scheme catalogue, which nobody appears to have built.
4. Completeness of the record as a dimension of the standard of proof (Nance's weight), joined to certified recall and residual formulae.
5. Stability as a required property of any language-model interpreter, measured and reported.

---

## Appendix. Items not Verified from a Primary Source

Marked [M] or [S] above, and to be checked before being cited in the infrastructure's notes: the PD 57AD text; RFC numbers and their content (3161, 4998, 5256, 5322, 6283); Da Silva Moore, Rio Tinto and Zubulake citations; FRCP 26(b)(5)(A), 37(e) and FRE 502; EDPB Recommendations 01/2020 and 02/2020, Guidelines 2/2018, Decision 2021/914 and the *Schrems II* ECLI; HRC General Comment 16; the APEC Privacy Framework and CBPR; ODRL's Recommendation date; LKIF Core's venue; Polisis's venue; Kaplan 1968, Cheng 2013, Tobia 2020 and Lee and Mouritsen 2018 citations; the ENFSI guideline and *R v T*; Choi (2025); the Sen et al. 2014 DOI; Linderfalk 2007's DOI; MonPoly's and Turnip's licences and availability; GDPRhub's licence; DAPRECO's formula count; the ISO/IEC 27050 editions of Parts 2–4.
