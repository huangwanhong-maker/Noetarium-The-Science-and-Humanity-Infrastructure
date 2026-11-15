# 0113. archeion, the Foundation of Archives, and the Law Domain

- **Status:** Accepted, provisionally (2026-11-14); continued by [0115](0115-precedent-proceedings-and-the-archives-continued.md) (precedent, proceedings; archeion's consistency proofs, dates and packages).
- **Date:** 2026-11-14
- **Decided by:**
  - **The request.** The user asked for "a evidence recognize, archieve, and reasoning system, for law domains", and offered a taxonomy of facts: "Legal facts that already through some normative judgement, social facts that in a relational system … pre-symbolic fact that may be hard for us preservation, what we can preserve is pre-interpreted structure, like image, sentence, article … historical facts; custom facts (like what ICJ article.38's ones), normative facts like treatise", asking "What facts we still have." They held that "we in fact need a law domain-specific archieve, on the top of it we can discuss more things, like reasoning, computational models for reconstruction of facts, interpretations", and asked whether "archieve theory and infrastructure worth a domain or foundation? Because it also needed for culture preservation, history material preserving, library management, etc, many domains."
  - **The choices.** Asked, the user chose a foundation named archeion, and to build it and the law domain first, the work to be discussed after.
  - **Two additions while it was built.** "Facts' category theory building, verification, and diagram drawing is needed. We have a existing category theory lib." And the three registers of the user's Generative Relational framework (ontological, relational, manifestational), offered as "insights about reality" that might help.
  - **The design.** Claude, here.

## 1. Context

ADR 0112 founded logic (apodeixis) and evidence (tekmerion) and left law as a row of the README's §4. Nothing in the infrastructure kept records as an archive does, except a small archive inside the server of Histos Harmonia. The user's taxonomy of facts was a flat list, and its items lie on different axes.

## 2. Decision

1. **archeion, a foundation, on apodeixis, morphe and calx.** Archival theory names no object in nature and is needed by law, history, the preservation of culture and libraries. It holds: fixity (SHA-256, a hash-chained journal, Merkle trees with proofs of inclusion); holdings and manifests; arrangement by provenance and original order, described level by level; provenance and what each activity kept; chains of custody and authenticity; retention and access by declared rules.
2. **An archive holds manifestations.** After the user's three registers, three things are kept apart and are the foundation's invariants: a holding's identity is its bytes; its sameness with another names a criterion (bit for bit; up to whitespace; whatever a migration declares it keeps); what it manifests is a stated relation, and subjects whose traces coincide are reported, never merged.
3. **Law, a domain, a field at height 0** on apodeixis, tekmerion, archeion and kategoria. Its first modules: `facts`, `proof`, `sources`, `norms`, `exhibits`.
4. **Facts have a kind on each of five axes:** register and kind; standing in a proceeding; role and scope in reasoning; logical form; traces. The ontological register is left open: an occurrence is referred to and never established, and a finding is a relational fact (it emerges through a procedure, persists, binds) that states its three criteria. The kinds the user's list did not have are named in the domain's theory note: mental and institutional facts; admitted, noticed, presumed and deemed facts; ultimate and legislative facts; negative, counterfactual, dispositional, predictive and statistical facts; evaluative facts; facts about norms; procedural facts.
5. **The taxonomy as categories.** `domains/law/schema/facts.cat` states it in kategoria's language, and kategoria answers its 24 statements as expected: the registers and kinds are orders with a terminal object; standing is a free category that is not thin, with a thin quotient; custom is the product of a practice and its acceptance; a record of facts is a set-valued functor, and two ill-formed records are refused. The C++'s order of kinds is held to the schema's pair by pair. A general tool, `applications/category-studio/tools/draw-schema.mjs`, verifies any such script and saves its diagrams.
6. **Declared, never asserted.** Standards of proof, the shares that make a practice general, the order of the maxims between sources, retention periods and access rules are the caller's to state; nothing is any jurisdiction's until a scheme says so.

## 3. Alternatives Considered

- **Archives as a domain.** Other domains could use it only from above; and it names no object.
- **A law archive inside the law domain.** History, culture and libraries would each write another.
- **The taxonomy as a flat list of kinds.** It would make "historical", "legal" and "negative" rivals, when a fact may be all three.
- **Establishing occurrences.** The user's framework suspends the ontological register, and a record that could mark an occurrence "proved" would hide what a finding is.
- **The work now.** The user chose to discuss reasoning, reconstruction and interpretation on top of the archive first.

## 4. Consequences

- **Checks.** archeion 97; law 79.
- **Shown.** A deed whose custody is broken at one handover is not authentic, and in the chart of evidence the archive's finding undercuts it: the sale it was offered for is no longer established, and is again when a second exhibit with whole custody is offered. A bicycle in a park is prohibited under one reading of "vehicle" and not the other; a statute ranked above the bylaw settles it.
- **Read from a summary.** The Generative Relational framework was read from the author's summary; the papers' servers refused automated reading. The notes say so, and say where they adapt.
- **Repositories.** `huangwanhong-maker/archeion` is the user's to create, with apodeixis and tekmerion (ADR 0112). Commit order: … apodeixis, archeion, tekmerion ….
- **Not built:** the work; a written form of records of facts and of norms; proceedings and dockets; precedent; legislative drafting on evidence; packages for transfer and the profession's formats (EAD, RiC, PREMIS, Akoma Ntoso, LegalRuleML); Histos Harmonia's archive moved onto archeion.
