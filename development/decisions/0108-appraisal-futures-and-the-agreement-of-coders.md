# 0108. Appraisal, Futures and the Agreement of Coders

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user asked to implement the methods of the handbook *Material and Ecological Conditions of Life*.
  - **The gap.** Its §9 (multi-criteria analysis, cost–benefit analysis, deliberation), §10 (Delphi, morphological analysis, decisions under uncertainty) and §8 (coded interviews) had no code.
  - **The design.** Claude, here. No new domain or foundation was needed: these are methods of the social sciences, nomos's.

## 1. Context

The handbook asks that the impacts of a choice be evaluated across distinct dimensions, "keep[ing] thresholds and rights constraints explicit", and warns against "treating everything as a compensable weighted score". The infrastructure held composite indicators (nomos, decision 0006), social choice, and decisions under uncertainty as POMDPs (tyche). It had no appraisal of alternatives, no discounting, no record of a panel's rounds or of a field of futures, and no measure of how far coders agree.

## 2. Decision

- **nomos** ([its decision 0012](https://github.com/huangwanhong-maker/nomos/blob/main/development/decisions/0012-appraisal-and-agreement.md)):
  - a module `appraisal`:
    - multi-criteria analysis: screening by non-compensatory limits, the weighted sum, TOPSIS, the AHP with its consistency, PROMETHEE II, weight stability, SMAA's acceptability;
    - cost–benefit analysis: constant and declining rates, IRR, switching values, distributional weights;
    - Delphi's rounds, and morphological fields under cross-consistency;
  - in `measurement`, Cohen's and Fleiss's κ and Krippendorff's α.
- **The API:** `science/appraisal.hpp` (six operations) and `science.appraisal`.
- **The Urban Studio's fourth plug-in, *Appraisal and futures*:** four tools on a project's own tables.
  - *Alternatives compared*: the limits are applied before the ranking, and the note says what the limits set aside and how far each weight can move.
  - *Costs and benefits*.
  - *Delphi rounds and morphological fields*.
  - *How far coders agree*.

## 3. Alternatives Considered

- **A domain of policy analysis.** Rejected: the methods name no object and serve every domain that chooses. A domain declares its criteria, rates and limits.
- **Ranking without screening.** Rejected, as the handbook asks: an alternative that breaks a right is not ranked low, it is set aside, and the answer says why.
- **Board templates only.** The handbook's templates for research questions, tool evaluation and study protocols (§16) are writing, not computation. They belong to the board's templates (Tabula's *research* plug-in), and are planned there.

## 4. Consequences

- **Checks.**
  - nomos: appraisal and agreement, by hand and against numpy, statsmodels and the krippendorff package.
  - The Urban Studio's suite: each tool on tables worked by hand.
- **Not built:**
  - ELECTRE with vetoes;
  - minimax regret and scenario discovery across many futures;
  - real options;
  - the handbook's templates on the board.
