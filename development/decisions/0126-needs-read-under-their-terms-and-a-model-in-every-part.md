# 0126. Needs Read Under Their Terms, and a Model in Every Part

- **Status:** Accepted, provisionally (2026-11-18). The names of the module (`construal`) and of the model's roles are provisional.
- **Date:** 2026-11-18
- **Decided by:**
  - **The request.** After ADR 0125: "The language model can utilize in various parts, including interpretation, next population generation opertors (like, mutation, variation, etc), initial population generation, relational structure discovery, etc. Also our formal modeling, and facts interpretation famework (I think exist in some foundations..) Basically, current is good, next make it more perfect, robust, and flexible and extensible." Then: "You know, these are important, especially fact / evidence interpretation foundation. It even useful in law and fact reasoning domains."
  - **The design.** Claude, here, on tekmerion, apodeixis, peira and heuresis.

## 1. Context

ADR 0125's search judged each statement of need as written. But a statement's terms are open:

- which income counts as low;
- whom a group is compared with;
- at what level a standard is set;
- which causal structure holds.

A need that holds under one reading and not another is a weaker finding than one that holds under all, and nothing said which it was. Nor did anything ask whether something other than the group explained its gap: a gap among the old may be the gap of those who live in aged care.

The same holds of findings of fact in law (what counts as "at the scene") and of the evidence for a policy. apodeixis computes what follows under each reading of a norm, and tekmerion's interpreters weigh readings. A finding of evidence still needed four things: readings with their sources, its pivots, its rivals, and its standing. The interpretation of evidence is a foundation's (tekmerion's), so that law, fact reasoning, policy and needs share it.

A language model took part only as a proposer of whole statements. The user asked for it in every part of the loop: seeding, variation, interpretation and the discovery of structure.

## 2. Decision

### 2.1 The Foundations

- **tekmerion's `construal`** ([its decision 0008](https://github.com/huangwanhong-maker/tekmerion/blob/main/development/decisions/0008-construal.md); 46 checks, tekmerion 271).
  - **Readings and judges.** A finding is a claim, its open points and a judge. Each reading has a source: authority, convention, analyst, data or proposal.
  - **What is computed.** Where the finding holds and its label, and the admissible interpretations. The sources argue: by default data < analyst < convention < authority, so readings of one source stand together and a reading that binds sets aside those beneath it. Then the class over the record's readings (clear, arguable, untenable), and again as proposed, a proposal widening only what is shown as proposed. Then the pivots, and the weight under any interpreter.
  - **Rivals and standing.** Rivals are assessed within their strata, pooled by peira's fixed effect. The standing comes from grounded semantics, and a proposed rival never attacks.
- **heuresis** ([its decision 0007](https://github.com/huangwanhong-maker/heuresis/blob/main/development/decisions/0007-proposals-with-their-parents.md); heuresis 592). A proposal may name the members it was made from: its lineage records them, it inherits their age, and they are credited when it survives.

### 2.2 The Needs Domain

The domain now declares tekmerion; it has 189 checks, 38 of them new.

- **Declarations.** The dataset declares:
  - points of interpretation (indicator, reference or level), each reading with its source and grade;
  - an order of the sources;
  - alternative causal graphs;
  - rivals: strata as groups, or an attribute with cut points.

  All are checked, and refused with the reason.
- **`evidence/interpretation.hpp`:**
  - `interpret` reads a statement under every combination of the readings that concern it, its own among them. Each combination is judged by the gap test on discovery.
  - `interpret_causal` reads causal support under each graph.
  - `rival` assesses a rival on discovery.
- **Standing.** Untenable readings (U) and a declared rival that explains the gap away (R_k) attack the need. Arguable readings, a rival that explains part, causal support that depends on the graph, and any proposed rival qualify it.
- **Admission.** A statement untenable under its readings, or explained away by a declared rival, is not kept; each gate is declared, and "clear only" may be asked. All of it is read on discovery, before the holdout is asked. One need under two declared measures is one need: the distance does not count the difference, and the stronger supersedes.
- **The model's roles.** `requests` says what to ask of a model in four roles: seed, mutate, cross and reflect. Each role's share of a budget is the posterior mean of its proposals' survival, with the members to start from and discovery's notes on them. A model's statements enter under `model:<role>` with their parents.

### 2.3 The Layers Above

- **The API** (`science/needs.hpp`, `science.needs`; 37 checks).
  - The engine:
    - reads the declarations;
    - answers `interpret`;
    - gives each piece of evidence its interpretation, causal readings and rivals, each rival with its source;
    - takes proposals with parents;
    - answers `requests`.
  - In Python:
    - `fulfil` answers a round's requests in one call to the model;
    - `interpret` asks a model for a statement's words, other readings and rivals;
    - `structure` asks for causal graphs and rivals;
    - each is checked against the data's vocabulary and refused with the reason, never repaired;
    - `with_proposals` adds what was proposed to a dataset as grade C;
    - `provenance` writes the lineage as W3C PROV-JSON, a model an agent of its own;
    - `rounds` runs it all.
- **The data library.** `needs_tables.py` gains the census's lower and higher income lines ($400, $650, $800 a week), each area's share of people outside private dwellings (G01, now fetched with its digest), and the points, an alternative graph and two rivals.
- **The plug-in *Needs*.**
  - **The card.** A need opens with its readings: the grid of its interpretations, the stated one ringed, proposals dashed, readings set aside faded. With it: what it turns on, provision under each graph, its rivals, and a model's words.
  - **The loop.** The Generate stage takes the model's roles and a budget; the Admit stage takes the gates.
  - **The composer.** It says how statements are read and what a model proposed, each proposal to be set aside, with a key that asks a model for causal structure.
  - **The head.** It gives the lineage as PROV.
  - **Routes:** `interpret`, `interpret-model`, `structure-model`, `set-aside`, `provenance`; the model only from the work's own page at this computer's own address. The Urban Studio's suite has 106 checks.

## 3. Alternatives Considered

- **Interpretation in the needs domain.** It is general: a finding of fact in law is read the same way, under what counts as "at the scene". The foundation holds it, and the domain declares its points.
- **A model's readings as the weakest arguments.** Under grounded semantics they would always be defeated wherever a point has another reading, so they would be invisible. Shown as proposed, apart from the record's, they say what they would change and change nothing.
- **A call to the model per request.** A round's requests are answered in one call, which keeps the cost of a round to one call however many roles share it.
- **Income lines as separate needs.** Four statements of one group, one per line, are one need read four ways. Counting them as distinct filled the archive with them.

## 4. Consequences

- **Melbourne, ten generations in 2.5 s.** 548 statements were judged, 52 admitted, 36 kept and 27 confirmed. Sixteen were superseded, the variants of one need under another income line among them. Of those admitted, 42 are clear under their readings and one arguable; 9 concern no declared point.
- **The care needs of those 85 and over** are clear under both references. People in non-private dwellings, the rival declared for homes for the aged, does not explain them away: 104% of the gap remains within its strata. For those 75 and over it leaves 75%.
- **An arguable need.** Low income among people of areas where many moved in the past year holds under three of eight readings, and a rival of renting areas explains about half of its gap.
- **A real model in every role** (Claude Opus 5.5, with the key in the settings folder):
  - it interpreted a need in words, with five readings and five rivals, none refused;
  - it proposed three causal graphs and five rivals, a fourth graph refused for a cycle;
  - in a search continued with it in four roles, 33 of its statements entered under their roles, 21 with their parents. One of them, a refinement of a kept need, was kept and confirmed: unpaid care given by women over 45 in settled areas.

  Every one of its contributions is of grade C and shown as proposed.
- **The page** was driven in headless Chrome (31 steps, not in the suite). Seven pictures are added to the gallery's §45.1.
- **Not built:**
  - readings that depend on one another;
  - rivals as a model of the outcome;
  - a person's own readings entered on the page;
  - levels declared for Melbourne's normative statements;
  - the law and policy domains' findings read through `construal`, which they can now use unchanged.
