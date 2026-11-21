# 0142. The Stand-in Inside the Search

- **Status:** Accepted, provisionally
- **Date:** 2026-11-21
- **Decided by:** the user's "Continuer" after ADR 0141, taken as leave to build what the design of evolutionary allocation left for later (`development/ideas/2026-11-20-evolutionary-allocation-of-heterogeneous-resources.md`, *What Is Not Built*), its first item first: "the stand-in inside the evolutionary loop: a search that screens each generation by the calibrated surrogate and runs only what could be kept, its coverage tracked as it goes".

## 1. Context

ADR 0140 gave the economy of places a stand-in: a Gaussian process with split conformal intervals, fitted once on runs drawn beforehand, screening a fixed list of candidates. Inside a search the candidates are not known beforehand, and the economy is the cost: Melbourne's case in ADR 0141 ran 776 allocations in 213 s. The survey's rule stands (Part VII §6.1): nothing may be chosen on a surrogate alone.

## 2. Decision

1. **A screened evaluator** (the allocation domain's `evaluators/surrogate.hpp`: `ScreenSpec`, `ScreenLog`, `screened`, `screened_economy`). A costly evaluator is wrapped so that the search screens as it goes:
   - its first `warm` candidates are run, and what they answered for the declared metric is kept with their features (each recipient's amount over the pool, its place in the order of grants);
   - every `refit` new runs, a Gaussian process is fitted to the even-numbered of the most recent `window` runs, and its split conformal intervals calibrated on the odd-numbered;
   - a candidate whose interval lies wholly above the `keep`-th best value run so far is not run. The evaluator abstains, saying why, and an abstention ranks below every assessed candidate (heuresis's tiers), so it is never kept;
   - every other candidate is run, and whether its value fell in the interval predicted before the run is counted.

   The answer reports the runs made, the candidates screened out, the refits, and the online coverage on the candidates run after a prediction.
2. **At the API** (`science/evolve.hpp`). An evaluator of kind `economy` may declare `screen: {alpha, keep, warm, refit, window, scale}`; the search's answer gains `screened`. `science.allocation.economy(..., screen=...)` declares it.
3. **heuresis's tiers mended.** `tiered_ranking` compared every candidate's number of selecting objectives with the first candidate's, and refused the ranking when the first had abstained or failed, since it carries none. It now compares the assessed candidates among themselves (a check added; heuresis 781).

## 3. Alternatives Considered

- **The stand-in's estimate entering the population as a value.** An estimated objective is not selecting under the default tiers. Mixed with computed values, the ranking is refused, and an estimate that won would be a choice made on a surrogate. An abstention keeps the population honest.
- **Refitting only between generations.** That would keep the stand-in independent of the order of evaluation within a generation. But the composition's search evaluates in a fixed order, and a run repeats exactly; refitting by runs keeps the wrapper free of the loop's structure.
- **A guarantee claimed from the theorem.** Split conformal coverage needs exchangeable runs, and a search's candidates drift. The coverage claimed is the online coverage, measured on the candidates the stand-in let through.

## 4. Consequences

- **Checks:**
  - the allocation domain: 10 (148);
  - heuresis: 1 (781);
  - the API's allocation checks: 2 new.
- **What the check showed.** A fund for eight places, its claims met in full or in part in the order searched, was judged by an economy whose households spend by whether they are short, with the same search run alone and behind the stand-in:
  - at 90% confidence the economy ran 420 times instead of 488 and the online coverage was 0.91; at 70% it ran 337 times, with coverage 0.83, conservative on the candidates let through;
  - the best found was the same in every case, and every allocation kept was run in the economy.
- **Where it is meant for.** Screening reads one metric. With other objectives, a candidate refused on the costly metric might have been kept for another, so it is meant for searches in which the costly metric is the one that selects. Melbourne's case, judged by two evaluators kept apart, is left unscreened.
- **Not built:** screening on several metrics at once (a candidate refused only when its intervals are dominated); a stand-in shared between searches; the generalised assignment problems, which need a carrier by which each recipient draws from one of several pools chosen by the genotype.
