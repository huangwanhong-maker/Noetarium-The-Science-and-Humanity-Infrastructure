# 0085. Demography, the First Domain on peira

- **Status:** Accepted, provisionally (2026-11-08).
- **Date:** 2026-11-08
- **Decided by:**
  - **The question.** The user asked whether quantitative social science "deserve[s] a domain base on" peira (ADR 0082).
  - **The proposal.** Claude proposed demography as the first, the README having planned it (§4, priority 3.9).
  - **The go-ahead.** The user: "all proceed".
  - **The design.** Claude.

## 1. Context

peira holds the designs and mechanisms by which units are observed. A domain on it should hold the objects of one science. Demography is the science whose objects (populations by age, their deaths, births and growth) are read most directly through peira's cohorts:
- person-time on the Lexis diagram;
- rates;
- standardisation.

Epidemiology, planned above demography, will stand on both. "Quantitative social science" as a whole is not one science but the methods of many, which peira and nomos hold.

## 2. Decision

### 2.1 peira Begins Its Third Stage

peira gains `cohorts` (its decision 0005):
- the Lexis diagram, splitting lives into person-time and events by age and period exactly;
- rates with Garwood's exact intervals;
- direct and indirect standardisation.

### 2.2 The Demography Domain

`domains/demography`, a field at height 0, C++ from the start, on peira, tyche and calx:
- **`lifetable`:**
  - the period life table, with a_x by default or exact for a constant hazard;
  - rates read from peira's Lexis diagram, and death rates standardised through peira;
  - Arriaga's decomposition of a difference in life expectancy.
- **`fertility`:** the total, gross and net reproduction rates and the mean ages at childbearing.
- **`projection`:** the Leslie matrix with Keyfitz's first row, projection, the stable population (λ, r, its structure, R₀) and Lotka's intrinsic rate.

The domain has 22 checks against closed forms and exact identities. Its theory note proves them:
- the exponential law reproduced;
- Arriaga's telescoping;
- R₀ = NRR;
- the strong ergodic theorem's consequences and the sign of r.

## 3. Alternatives Considered

- **A domain of "quantitative social science".** Rejected: its methods are peira's and nomos's. A domain holds one science's objects, and population is such an object.
- **Demography inside economics.** Rejected: births and deaths are not value carried between positions; economics may declare demography when it needs ageing.
- **Python first.** Rejected: the domains are C++ from the start since ADR 0052, and the stable population's linear algebra is calx's.

## 4. Consequences

- **The order.** It gains a field at height 0, and epidemiology (priority 3.10) can stand on it.
- **Next:**
  - real tables through the data library (the ABS's, the UN's World Population Prospects);
  - mortality models (Lee–Carter);
  - migration in cohort-component projections;
  - age–period–cohort analysis on peira's `cohorts`, where its non-identification is shown exactly.
