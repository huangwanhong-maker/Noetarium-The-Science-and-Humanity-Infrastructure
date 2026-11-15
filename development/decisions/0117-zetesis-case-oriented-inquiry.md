# 0117. zetesis, a Foundation of Case-Oriented Inquiry

- **Status:** Accepted, provisionally (2026-11-15).
- **Date:** 2026-11-15
- **Decided by:**
  - **The request.** The user held that policy making depends on "social science archieve, evidence archieve foundation, quantitative analysis and traditional analysis foundations", to be built before the works (ADR 0116).
  - **The choice.** Asked, the user chose a new foundation for traditional analysis, named zetesis.
  - **The design.** Claude, here and in zetesis's decisions 0002–0003, apodeixis's decision 0004 and morphe's decision 0018.

## 1. Context

ADR 0116 found the case-oriented tradition missing entirely: qualitative comparative analysis, the coding of qualitative data, concepts and typologies, comparative designs and case selection, process tracing. It ordered the strands of the layers beneath policy making, and this is the third. The archives of data and of evidence, the first two, are built.

## 2. Decision

1. **A fifteenth foundation, zetesis** (ζήτησις, inquiry), on apodeixis, tekmerion, nomos, tyche and morphe, and through them peira, physis and calx. Its methods name no object in nature and serve the social sciences and the humanities.
2. **Six modules** (zetesis's decision 0003; 97 checks):
   - `sets`: calibration, sufficiency and necessity with consistency, coverage, PRI and relevance;
   - `qca`: truth tables crisp and fuzzy, and the conservative, parsimonious and intermediate solutions with every minimal model, untenable remainders, simplifying assumptions and contradictory ones;
   - `coding`: codebooks, passages verified against their documents, the agreement of coders by nomos, matrices for QCA, saturation, revisions;
   - `concepts`: Goertz's structures, the ladder of abstraction and subtypes on morphe's concept lattice, typologies checked cell by cell;
   - `cases`: Mill's methods, selection from a regression on tyche, set-theoretic selection after QCA;
   - `tracing`: Van Evera's tests, rival hypotheses in decibans with reports through tekmerion's channels, causal models of mechanisms on tyche's networks with the clue worth seeking.
3. **General methods first in the foundations where they belong:**
   - Boolean minimisation in apodeixis's new `boolean` (every prime implicant, every minimal sum, a checker apart; its decision 0004);
   - formal concepts in morphe (NextClosure, the concept lattice, the canonical basis of implications; its decision 0018).
4. **Declared, never decided.** Anchors, thresholds, directional expectations, untenable remainders, codebooks, structures, priors and likelihoods are the researcher's, and every answer reports them.

## 3. Alternatives Considered

- **Within nomos or tekmerion.** Rejected (ADR 0116, and zetesis's decision 0002): two epistemologies in one library, and dependencies nomos should not carry.
- **QCA written whole in zetesis.** Rejected: Boolean minimisation is logic and formal concepts are order theory, and the reuse principle puts each where it belongs.

## 4. Consequences

- **Checks:** zetesis 97, apodeixis 228 (`boolean` 9), morphe 821 (`concepts` 9).
- **Repository.** zetesis's repository on GitHub is the user's to create; until then it lives in the working tree as apodeixis, tekmerion and archeion do.
- **Next:** the quantitative gaps (strand 4): microsimulation and psychometrics in nomos, mixed and latent-variable models in tyche, small areas in peira.
- **Not yet:**
  - interpretive analysis of texts;
  - multi-value and temporal QCA;
  - robustness of QCA over ranges of thresholds;
  - unitising agreement;
  - a model as a third coder;
  - the composition of zetesis in the API.
