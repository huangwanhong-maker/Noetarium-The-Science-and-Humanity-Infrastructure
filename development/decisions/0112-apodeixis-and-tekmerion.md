# 0112. apodeixis and tekmerion: Logic and Verification, Evidence and its Interpretation

- **Status:** Accepted, provisionally (2026-11-14); amended by [0114](0114-evidence-based-policy-and-interpretation-by-models.md) (tekmerion's `policy` renamed `warrant`, its value of information moved to tyche; readings, panels and sequential interpretation).
- **Date:** 2026-11-14
- **Decided by:**
  - **The request.** The user asked about decision theory, "complex evidence recognization and reasoning, and a formal logical reasoning domain/foundation", usable on formal languages such as a derivation tree of "Γ ⊢ a,b,c,d" with its verification; and pointed out that "different facts in the society involve interpretation under some normative (like a model), this is why sometime law is an art of interpretation. So whether X → Y, can be derivated, some time need interpretation." They agreed to the formulation Γ ⊢_ι φ, adding that the norm "can be abstract, we can interpret in different method, like a classical statistics model, e.g., beyesian, or a POMDP based model, Even an LLM", and asked that "evidence, evidence reconstruction and recognization, and reasoning, formal verification" be built first, with EBPM, "evidence based policy making framework", before their uses in "urban nomo building/legistration, jurisprudence, judiciary" are discussed.
  - **The choices.** Asked, the user chose two foundations rather than one, and the names apodeixis and tekmerion.
  - **The design.** Claude, here; tyche's networks and belief functions and peira's synthesis were written by helpers of the same session on briefs, and their checks run again here.

## 1. Context

The infrastructure had pieces of what was asked and nothing that derives or verifies. tyche's `decision` held decisions under partial observation, nomos games, social choice and appraisal; morphe planned a module `logic`; the philosophy domain held Dung's argumentation frameworks, waiting for a second user to move them down; the history domain graded who says each statement. No foundation checked a derivation, decided a sentence with a countermodel, or said what follows under which reading of a norm. Law is a row of the README's §4, at priority 4.

## 2. Decision

1. **Two foundations.**
   - **apodeixis** (ἀπόδειξις, demonstration), on morphe and calx: formulas and sequents read from text; calculi declared as rule schemata, derivation trees and a small checker; G3cp and G4ip decided, G3c searched, any declared calculus searched to a bound; models and countermodels (valuations, finite structures, Kripke models, dyadic obligation); satisfiability with refutations a separate checker verifies; argumentation (Dung's frameworks moved from philosophy, ASPIC+); and derivability under an interpretation. Low in the order, so that the other foundations may use logic without chance. morphe's planned `logic` points here.
   - **tekmerion** (τεκμήριον, the sure sign), on apodeixis, peira, tyche, physis, morphe and calx: charts of evidence read as argument and as probability; Schum's force of a report through its source's credibility; networks of the idioms of evidence and testimony as belief functions; reconstruction as parsimonious covering and scenarios; recognition of candidates with their passages checked and a standing by a declared rule; interpreters; evidence for policy.
2. **Γ ⊢_ι φ.** An open point of a norm is declared with its readings; an interpretation chooses one at each; for each, an engine (defeasible, strict, classical) says whether φ follows. apodeixis computes exactly the set D(φ), its label (the least readings sufficient for φ, after de Kleer's ATMS), and whether φ is clear, arguable or untenable over the admissible interpretations (Hart's core and penumbra as sceptical and credulous acceptance), with explanations point by point. Admissibility is argued: interpretive arguments of kinds a case ranks (MacCormick and Summers' canons) are evaluated by Dung's grounded semantics. Nothing decides which reading is right.
3. **Interpreters of any method.** At the user's word, tekmerion's interpreters weigh the readings: a law, a credal set with exact bounds, precedents under a Dirichlet prior, a logistic regression on earlier cases, a decision over the norm's purposes, a proposal. A language model is not called by a foundation; its answer enters as a proposal and stays proposed until an argument is made for its readings.
4. **EBPM.** Grading by a scheme a person declares (examples written after GRADE and the Maryland scale); the argument that a policy will work here after Cartwright and Hardie, on peira's synthesis; the value of perfect and of sample information.
5. **General methods went down first.** tyche gained `graphical` (Bayesian networks: elimination, likelihood ratios, the most probable explanation, belief propagation, EM) and `beliefs` (Dempster–Shafer belief functions), its decision 0023, 138 checks. peira gained `synthesis` (effect sizes, fixed and random effects by DerSimonian–Laird, Paule–Mandel and REML, Hartung–Knapp, prediction intervals, leave-one-out, cumulative pooling, Egger's test; transport with its condition checked by d-separation), its decision 0009, 97 checks.
6. **Decision theory stays in tyche.** The one-shot core (expected utility, decisions under ignorance and ambiguity, imprecise probability, decision trees and influence diagrams, the value of information in general) is planned there; tekmerion holds only the value of information for adopting a policy.

## 3. Alternatives Considered

- **One foundation.** Simpler to find; rejected by the user's choice, since logic would stand above chance and designs.
- **morphe's `logic`.** As for kategoria (ADR 0100): the body is large and of another kind than morphe's structures.
- **A law domain now.** The user asked for the foundations first, and their uses to be discussed after.
- **Interpretation by a model alone.** A model's reading is one interpreter among others and is never the source, as in Histos Harmonia (ADR 0110).

## 4. Consequences

- **Checks.** apodeixis 219 (53 of them moved from philosophy), tekmerion 105, tyche's two modules 138, peira's 97; philosophy keeps 37.
- **Repositories.** The user is to create `huangwanhong-maker/apodeixis` and `huangwanhong-maker/tekmerion` on GitHub; until then the two are directories under `foundations/`, attached as submodules when the repositories exist. Commit order: … kinesis, apodeixis, tekmerion, neuroscience, Cosmopoiesis, the root.
- **The philosophy domain** declares apodeixis; its argument maps stand on the moved frameworks, and its theory note's §1–§6 now live in apodeixis's argumentation note, numbered as they were.
- **Not built:** a law domain (statutes as normative systems, precedent after Horty's reason model, a tradition's canons and standards of proof); deontic and defeasible logics by their own calculi; timelines as temporal constraints; dependent sources; interpretation over a sequence of cases as a POMDP; a work (a studio of derivations and evidence); the API's compositions.
- **Provisional:** the names are the user's; the status of both foundations is draft.
