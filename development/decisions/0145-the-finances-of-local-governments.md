# 0145. The Finances of Local Governments

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude. The survey of steering cities ranks municipal finance first among the domains' lacks (*Evidence, methods and actions for steering cities*, §4.2: "a new domain or module of municipal finance ... the MFSA's ratios; cash-flow projection; debt capacity; a shadow rating as an ordinal scorecard; statutory threshold schemes"). Its placement as a module of economics, not a domain of its own, is provisional and the user's to revisit (CLAUDE.md §6.5).

## 1. Context

What a city can invest depends on what it can finance: its own revenue against its transfers, the surplus its operations leave, the debt it carries and can service, and the rules that bound its borrowing. The economics domain held value carried between places, growth, the short run, trade, and an economy's accounts. It held no government's accounts read into ratios, no projection of a government's years ahead with its loans, and no measure of how much more it could borrow.

## 2. Decision

1. **The economics domain's `fiscal`** (theory note `domains/economics/theory/local-government-finance.md`):
   - a chart mapping lines to declared categories, a line it lacks refused;
   - ratios as signed sums of categories, judged year by year against a declared scheme of warning and intervention thresholds;
   - the years ahead, with categories grown at declared rates and annuity and bullet loans scheduled;
   - debt capacity: the largest new annuity keeping the coverage of debt service at a declared floor in every repayment year (Proposition 1, which the survey's Part II states independently), bounded by a declared ceiling on the stock;
   - debt dynamics with the stabilising primary balance, and a fan by the moving-block bootstrap of the record's joint deviations;
   - capacity and effort by the representative tax system.
2. **The data library's `local_government_finance.py`.** The local government sector (S1313) of 28 European countries from Eurostat:
   - revenue, expenditure, net lending and interest from gov_10a_main;
   - debt from the fourth quarter of gov_10q_ggdebt, since the annual table holds only recent years for many countries;
   - GDP from nama_10_gdp;
   - the effective rate, growth, primary balance and stock-flow adjustment derived.
3. **The API.** `science/fiscal.hpp` (`fiscal.assess`, `fiscal.debt`, `fiscal.effort`) and `science.fiscal`, the latter reading the product.
4. **The Urban Studio's plug-in *Programmes and public finance*** (ADR 0144): the tools *finance* (an invented city's accounts, ratios, the years ahead and its debt capacity) and *local_debt* (a European country's local government debt with a fan).

## 3. Alternatives Considered

- **A named ratio such as "the DSCR" in the code.** The survey found three different quantities under that name: a cash-flow coverage, a liquidity coverage and a synthetic annuity coverage. Ratios are stated by their terms.
- **A jurisdiction's statutory scheme or a rating agency's matrix as code.** They differ by country and edition, and the survey could not verify several. A scheme of thresholds is data the caller declares, and a shadow rating is a scorecard (ADR 0144).
- **A domain of municipal finance.** That would be founding a domain, which is the user's decision. The module stands in economics until the user says otherwise.

## 4. Consequences

- **Checks:**
  - economics's fiscal: 25;
  - the data library: 7;
  - the API's check of programmes (shared with ADR 0144);
  - the Urban Studio: the plug-in's checks.
- **What the checks showed:**
  - the largest loan, once taken, holds the coverage exactly at its floor in the year that binds it, and a ceiling on the stock binds it lower;
  - the stabilising balance holds debt constant for ten years;
  - capacities under the representative tax system sum to what the whole raised;
  - on Eurostat's records the identities of the product hold exactly, and Germany's local debt runs from 2000.
- **Not built:**
  - a canonical chart of accounts with its basis (cash or accrual);
  - the MFSA's template as declared data;
  - statutory schemes and rating matrices as versioned data;
  - land value capture and land readjustment (the survey's Part IV gives their identities);
  - fiscal capacity estimated by regression;
  - debt dynamics against revenue rather than GDP.
- **Provisional:** the module's place and name.
