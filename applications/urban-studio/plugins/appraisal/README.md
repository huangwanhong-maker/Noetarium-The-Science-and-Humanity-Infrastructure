# Appraisal and Futures

*Status: draft (2026-11-13; [ADR 0108](../../../../development/decisions/0108-appraisal-futures-and-the-agreement-of-coders.md)).* The Urban Studio's fourth plug-in. It brings the handbook's appraisal, futures and coding methods (§8–§10) to a project's own tables. Its mathematics is nomos's `appraisal` and its agreement of coders, through the API's `science/appraisal.hpp`.

## 1. Tools

| Tool | What it answers | On |
|---|---|---|
| *Alternatives compared* | Limits applied first; a ranking by a weighted sum, TOPSIS or PROMETHEE II; how far each weight can move with the leader still first; how often each ranks first when no weight is declared (SMAA) | any table of alternatives scored on criteria |
| *Costs and benefits* | Present values at a constant or declining rate, NPV, ratio, IRR, discounted payback, switching values | any table of benefits and costs by year |
| *Delphi rounds and morphological fields* | A panel's rounds, and when it agrees and settles; the configurations a field leaves once the impossible pairs are struck out | a table of estimates by round; a table of parameters' values and one of exclusions |
| *How far coders agree* | Krippendorff's α at any level of measurement, Cohen's and Fleiss's κ | any table of codes (unit, coder, code) |

## 2. What Is Declared

- **Weights, limits and preference functions:** the analyst's. The tool reports how far the answer depends on them, and does not choose them.
- **Discount rates and bands:** declared in each run. What has no price is left out of the sum, and the note says it should be set beside the sum.
- **Consensus and stability bounds:** declared. Agreement is not truth.
- **Exclusions of a morphological field:** declared. What is left is what they allow, not a forecast.
