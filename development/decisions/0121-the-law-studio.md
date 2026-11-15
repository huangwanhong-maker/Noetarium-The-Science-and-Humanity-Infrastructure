# 0121. The Law Studio: Evidence, Recognition, Interpretation and Reasoning for Practice, on International Privacy Law First

- **Status:** Accepted, provisionally (2026-11-15). The work's name, colour and port are provisional.
- **Date:** 2026-11-15
- **Decided by:**
  - **The request.** Asked what to build next among many ideas, the user named "the law reasoning, evidence archieve/recognization/analysis studio" and "policy making one". Asked four questions, they answered: two works sharing an evidence layer ("ok"); "mainly for practice? evidence store/archieve, recognization, interpretation, reasoning"; "For computational models, you may utilize API or other method by surveying paper. Make them extensible"; a language model may be used, with the key they gave; and "real jurisdiction's, stated clause by clause? What about international law? Especially international privacy law."
  - **The design.** Claude, here, after two surveys (`research/literature/reports/Models and tools for legal practice.md`, `research/literature/reports/Sources of international privacy law.md`). ADR 0116 had set the order: the law work's design first, then the policy work.

## 1. Context

The layers beneath the work were built (ADR 0112–0117): logic and derivability under interpretation (apodeixis), evidence (tekmerion), archives (archeion), and the law domain's facts, proof, sources, norms, exhibits, precedent and proceedings. Three things were missing for practice.

- **No body of law.** "No limit in it is any jurisdiction's until a scheme states it", and nothing stated one. A scheme needs a form, a check, and the words of the law from their official texts.
- **No authority.** A term was read by arguments of the parties; nothing said that a court's reading binds one forum and only persuades another.
- **No plan.** The domain said whether a consequence follows. Counsel asks what remains to be proved.

The surveys added two findings.

- **Transfers are an open gap.** No formal model of a transfer impact assessment, and no machine-readable treatment of Chapter V of the GDPR beyond vocabulary (DPV names the tools and does not say whether one suffices), was found. The structure is an argument, which the infrastructure's pieces fit.
- **The failure that matters with a language model is the invented quotation** (*Mata v. Avianca*, S.D.N.Y. 2023; research tools still invent on a sixth to a third of queries, Magesh et al. 2025). The remedy is to find every quotation in its source.

## 2. Decision

A work, the **Law Studio**, on the shared studio host and page, with what it needs placed in the layers beneath it.

1. **The law domain** gains three modules ([its theory note](../../domains/law/theory/schemes-authority-and-the-plan-of-proof.md)).
   - `schemes`: a body of law stated clause by clause. Instruments with their provisions; rules, exceptions and counts-as links, each cited to provisions, with a quotation that must stand in a cited clause; a vocabulary; courts and holdings. `check` reports every fault.
   - `authority`: a forum declares the legal orders that bind it. A holding is binding, persuasive or left out (given after the date of the question). Holdings become interpretive arguments whose kinds are ordered: persuasive below binding, lower court below higher, earlier below later, the parties' canons below every binding holding. A dominant holding then fixes its reading (Proposition 1 of the note).
   - `issues`: the plan of proof. The ways to establish a question and to resist it are labels in apodeixis's sense over the alleged facts and the readings left open; a fact in every way is necessary (Propositions 2 and 3).
2. **The official texts** are the data library's: `formats/eurlex.py` reads an act of the Official Journal into provisions and a judgment into numbered paragraphs, and `products/legal_texts.py` fetches them from the Publications Office by CELEX number. A scheme names a clause; its words are filled from the fetched text and are not kept in the repository.
3. **The API** composes them: `science/law.hpp` (a scheme checked; a matter evaluated: evidence, facts, authority, each issue's label, what it rests on and its plan), `science/program.hpp` (a program around a composition, moved from the Archive Studio at its second use), and in Python `science.law`, `science.models` (providers of language models, shared with `science.interpret`; keys kept in the person's settings folder) and `science.recognise`.
4. **A model proposes and never decides.**
   - *Recognition.* A model reads a document against the matter's facts and must copy each passage it relies on. Each passage is looked for in the document; one that is not there is refused, never repaired. What passes is a proposal that counts for nothing until a person accepts it.
   - *Interpretation.* A model's reading of an open term is a proposal of grade C (ADR 0114). It fixes nothing until counsel makes an argument of it.
   - A provider is a class; Claude through Anthropic's SDK is the first, a stand-in the second. A model is asked only from the work's own page at this computer's address, as a script is run.
5. **The visualization layer** gains `evidence`: a matter as its argument (a drawing, or an outline for a matter too large to draw), the plan of proof, the readings of open terms, and passages marked in a text.
6. **The work** has five views: the matter, the evidence, the law, the readings, the plan of proof. A body of law is a plug-in bringing schemes and templates.
   - *park*: an invented bylaw after Hart's vehicle in the park, carrying its own words.
   - *privacy*: transfers of personal data out of the Union. Chapter V of the GDPR with Articles 3 and 4, the standard contractual clauses, the EU–US adequacy decision; the Court of Justice's readings (*Breyer*, *EDPS v SRB*, *Schrems II*), each passage found in its judgment; the treaty frame (ICCPR Article 17, ECHR Article 8, Convention 108) as texts.

## 3. Alternatives Considered

- **One evidence studio with law and policy as plug-ins.** Rejected with the user: a case and a theory of change want different pages. The evidence layer is shared instead.
- **Adopting a formalisation of the GDPR** (DAPRECO, PrOnto). Rejected: no licence is declared for either, and neither covers Chapter V. LegalRuleML and DPV remain formats to import and export, never the internal model.
- **A model that extracts facts freely.** Rejected: its output could not be checked. A model is given the facts to look for and must quote.
- **Repairing a near-miss quotation.** Rejected: a quotation that is almost right is the failure itself.
- **A plan over every fact the question turns on.** Rejected: a body of law has many routes, and a plan over all of them is large and says little. The plan is over what someone has alleged; the rest is listed.
- **The texts of the law committed with the scheme.** Rejected: fetched texts carry their own terms of reuse and their own dates. A scheme names clauses; the data library fetches them.

## 4. Consequences

- **Checks:** the law domain's 142 (35 new); the API's law checks (19, with a stand-in for the model); the data library's 14 for the reader of EUR-Lex; the visualization layer's `evidence` (16 of its 642); the studio's 46. The page was driven in headless Chrome through both matters (19 steps, not in the suite), once with a real model.
- **What the first privacy matter shows** (invented facts, real law): the prohibition of the transfer is arguable. It follows if "personal data" is judged from the exporter's side, on which no court has ruled; three readings are fixed (two by the Court of Justice's binding holdings, one by the Board's guidance, unopposed). There are four ways to establish it and three to resist it.
- **A scheme is its author's reading of the law.** Its check shows that it is whole and that its quotations stand in the clauses; whether a rule renders its clause rightly is a lawyer's judgment. The privacy scheme says what it does not encode and what is under appeal. It is not legal advice.
- **Not built:**
  - the lawfulness of the processing itself (Articles 5, 6, 9, 28), binding corporate rules in detail, Article 48, the second subparagraph of Article 49(1), onward transfers;
  - the treaty frame as rules (it binds states, and is given as texts);
  - technology-assisted review, near-duplicates and threading, legal hold, privilege logs (the first survey's §A);
  - tekmerion's other interpreters (a stated law over readings, precedents, a regression, a POMDP over a sequence of cases) weighed as a panel in the Readings view: they are reached from scripts through `science.interpret`, and not yet from the page;
  - defeasible deontic logic with reparation chains, precedent by dimensions;
  - LegalRuleML, Akoma Ntoso and DPV as formats;
  - the transcription of scans;
  - the policy studio.
- **Provisional:** the work's name, its colour (a claret and a periwinkle) and its port, 9170.
