# 0116. The Layers beneath Social Analysis and Policy Making, and a Social-Science Archive across Them

- **Status:** Accepted, provisionally (2026-11-15). The first three strands are built: the archive of data; the archive of evidence (tekmerion's decision 0006, systematic reviews; peira's decision 0011, sensitivity and protocols); zetesis ([ADR 0117](0117-zetesis-case-oriented-inquiry.md)); and the quantitative gaps (nomos's decision 0013, tyche's 0025, peira's 0012). The works may now follow.
- **Date:** 2026-11-15
- **Decided by:**
  - **The request.** The user confirmed the policy domain and the rename of tekmerion's `policy` to `warrant` (ADR 0114). They named two works as important: "one workbench for social analysis and policy making, and law / judical & evidence store and recognization and interpretation & law reasoning". They added that "both of them are substantiate works. policy making depend on social science archieve, evidence archieve foundation, quantitative analysis and traditional analysis foundations. I think we must build such first, before can do policy making."
  - **The choices.** Asked, the user chose: traditional analysis as a new foundation, named **zetesis**; the social-science archive built **across layers** rather than as a foundation of its own; and the **archive of data first**.
  - **The design.** Claude, here.

## 1. Context

What the two works need was set against what the infrastructure holds:

- **Quantitative analysis is mostly there** (tyche, peira, nomos). Missing for policy: microsimulation of reforms on weighted samples, psychometrics, general mixed and latent-variable models, sensitivity analysis, pre-registered protocols.
- **Traditional analysis is missing entirely:** qualitative comparative analysis, coding of texts, concepts and typologies, comparative case designs and case selection, process tracing.
- **No social-science archive.** archeion keeps holdings and the data library fetches datasets, but nothing describes a study as data archives do (codebooks after DDI, versions, harmonisation), protects what is released, or catalogues the studies there are.
- **Half an evidence archive.** tekmerion grades and peira pools, but nothing runs the systematic review that builds an evidence base.

## 2. Decision

**Four strands, built before either work, in this order:**

1. **An archive of data, across layers** (built, below).
2. **An archive of evidence:** systematic reviews in tekmerion (protocol, the screening flow, extraction, risk of bias by declared instruments, evidence gap maps, living updates), and peira's planned `protocol` and `sensitivity`.
3. **zetesis**, a foundation of traditional, case-oriented analysis, on apodeixis, tekmerion, nomos and morphe:
   - qualitative comparative analysis, crisp and fuzzy, with Boolean minimisation;
   - the coding of qualitative data;
   - concepts and typologies;
   - comparative case designs and case selection;
   - process tracing.
4. **The quantitative gaps:** microsimulation and psychometrics in nomos; mixed and latent-variable models in tyche; small areas in peira.

**The archive of data, the first strand:**

- **archeion `datasets`** (its decision 0005):
  - codebooks after DDI-Codebook 2.5, well formed;
  - files held to them cell by cell, with weighted frequencies;
  - versions compared, with what breaks comparison;
  - harmonisation by declared recodes, with what each source loses;
  - the finest common target of two variables as the pushout of their correspondence.
- **peira `disclosure`** (its decision 0010): disclosure control as a coarsening the releaser chooses.
  - Microdata: k-anonymity and l-diversity, with the minimal generalisations found exactly.
  - Tables: withheld cells partially identified within intervals found by linear programmes, and rectangles of secondary suppression audited exactly.
  - Noise: the Laplace and geometric mechanisms, with privacy accounting.
- **The data library:**
  - DDI-Codebook 2.5 read and written (`data/formats/ddi.py`), in the same form as archeion's codebook;
  - a general OAI-PMH harvester (`data/acquisition/oai_pmh.py`);
  - a catalogue of social-science studies harvested from the CESSDA Data Catalogue (`data/products/social_studies.py`; about 33,000 studies). It is kept in the cache only, since the catalogue states no licence for its descriptions.

## 3. Alternatives Considered

- **Traditional analysis within nomos.** nomos would then depend on apodeixis and tekmerion, and two epistemologies would share one library; the user chose a foundation.
- **A foundation for statistical data archives.** Offered; the user chose to build across layers.
- **The works first, foundations as they are needed.** Rejected by the user's own reasoning: policy making stands on these layers, and a work built before them would carry stand-ins.

## 4. Consequences

- **Checks:**
  - the archive of data: archeion 134 (`datasets` 18), peira's `disclosure` 17, and the data library's `check_ddi.py` (15, with the cache's harvest checked where present);
  - the archive of evidence: tekmerion 166 (`reviews` 17), peira's `sensitivity` 12 and `protocol` 8, peira 426 in all.
- **The quantitative gaps:** nomos's `microsimulation` and `psychometrics` (33 checks), tyche's `mixed` and `latent` (32), peira's `small_areas` (12).
- **Next:** the two works, the law work's design discussed first.
- **Not yet:**
  - the variable-level DDI of individual archives, and the harvest of the whole catalogue;
  - harmonised series built from real waves;
  - the archive's composition in the API.
