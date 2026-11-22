# 0152. A Project's Financing Plan: Captured Land Value Carried into the Years Ahead

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's word to continue designing and implementing while they slept. Their news piece on the day's work named the link as missing: "Captured land revenue is not yet carried automatically into fiscal projections." ADR 0146's "Not built" listed it too.

## 1. Context

The economics domain could do two things apart:

- **`fiscal` (ADR 0145):** project a city's accounts and loans, and find the largest new loan it could service at a floor of coverage;
- **`capture` (ADR 0146):** turn a project into streams of payments by instrument.

Nothing joined them, so the question a city asks of a project went unanswered: can we afford it, and how much does what we capture help? The two must also be kept apart where public accounts keep them apart. Capital receipts pay for capital and do not raise the operating surplus from which debt is serviced.

## 2. Decision

1. **The economics domain's `fiscal` gains `project.hpp`** (theory note `domains/economics/theory/local-government-finance.md` §6, 14 checks).
   - **The plan year by year.** The cost is met first by receipts, grants and a reserve that earns a declared rate; new annuities are drawn for the rest. Receipts left over may be pledged to the new loans' service before the reserve keeps them.
   - **Coverage.** The coverage is the operating surplus over the debt service the budget carries.
   - **The largest scale.** The largest scale of the project the city can afford is found by bisection. This is valid because the least coverage does not rise with the scale (Proposition 1, proved by induction).
2. **The API's `fiscal.plan`.** It answers the plan with the receipts and without them, with the largest scale of each. The projection's building is now shared with `fiscal.assess`.
3. **The Urban Studio's *Programmes and public finance*** gains the tool *project*, and the cohort's template a step. The tool takes the instruments chosen from the tram line's capture, maps their years from the valuation date onto Ardel's own years ahead, and leaves out receipts falling beyond them, saying how much.

## 3. Alternatives Considered

- **Receipts as revenue in the operating surplus.** That would let land value captured raise borrowing capacity directly, which public accounts and most fiscal rules do not allow. Receipts pay for capital or service the debt they are pledged to, and the operating budget carries the rest.
- **One loan for the whole project.** Drawing as the costs fall, a loan a year, is how construction is financed. It makes the coverage depend on the timing of receipts, which is what the comparison is for.
- **The plan inside `capture`.** `capture` knows nothing of a city's accounts and should not. The plan takes receipts as plain flows with their sources, so any instrument, grant or sale of land enters it alike.

## 4. Consequences

- **Checks:**
  - the economics domain's `fiscal` plan: 14 (the domain 243);
  - the API's check of programmes: 2 more (35);
  - the Urban Studio: one more, and the plug-in's six tools counted.
- **What the checks showed:**
  - a cost met by receipts and a loan, its coverage by hand;
  - receipts pledged against kept in the reserve;
  - the largest scale in closed form with and without receipts;
  - Proposition 1 on 200 random plans.
- **What the invented example shows:**
  - with the receipts of four instruments pledged, Ardel borrows 5,677 of the tram line's 6,000, its least coverage 1.94 in 2033;
  - without them it borrows 6,000, the least coverage 1.67;
  - it could afford 2.17 times the line with the receipts and 1.85 times without them;
  - unpledged, the receipts barely help (1.90), since most arrive after the works.
- **Not built:**
  - receipts uncertain (a range of uplift);
  - the timing of the works searched;
  - grants with conditions;
  - a ceiling on the stock of debt in the plan;
  - the waste account carried into the emissions inventory (ADR 0153).
- **Provisional:** the header's and the tool's names.
