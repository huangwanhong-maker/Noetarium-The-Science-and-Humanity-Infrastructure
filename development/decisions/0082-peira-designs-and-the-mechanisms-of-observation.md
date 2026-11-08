# 0082. peira: A Foundation of Designs and the Mechanisms of Observation

- **Status:** Accepted, provisionally (2026-11-08). Amends ADR 0048 (tyche's scope) and ADR 0056 (nomos's scope).
- **Date:** 2026-11-08
- **Decided by:**
  - **The proposal.** The user: "a quantitative (experiment) science foundation", holding "classical and modern advanced methods for social science experiment, cohort analysis & visualization, hypothesis validation", to which "some quantitative economics can move", because such methods are "utilized in many sciences, economics, clinical experiments, social science, psychology".
  - **The answers.** The user agreed:
    - to the name *peira*;
    - to a scope including surveys, survival and missing data;
    - to moving difference in differences and instrumental variables out of nomos, and panel fixed effects to tyche.
  - **The criterion and the first stage.** Claude.

## 1. Context

nomos (ADR 0056) held the methods of the social sciences: measures of inequality and poverty, index numbers, games, matching, social choice, agents, and an econometrics of panels, instruments, differences in differences, marginal effects and Poisson regression with absorbed effects. Its econometrics was placed there because the economics domain needed it first. But differences in differences, instruments, trials, surveys and cohorts serve every science that experiments or observes people, animals, patients or plots: clinical trials, epidemiology, psychology, ecology, education, urban science. A clinical domain should not have to depend on a library of the social sciences.

These methods are also not tyche's. tyche infers within a given model (ADR 0048). What these methods share is a different object: the mechanism by which the data came to be seen.

## 2. Decision

### 2.1 An Eighth Foundation

**peira** (πεῖρα, a trial; whence "empirical") holds the designs and mechanisms by which units come to be treated and observed, and what is identified through them. Its organising theory is the coarsened-data model (its `docs/theory.md`): full data hidden in part by a mechanism.

| Setting | What the mechanism reveals |
|---|---|
| Experiments | one potential outcome of each unit |
| Surveys | some units |
| Cohorts | part of each life, through censoring |
| Missing data | some values, through nonresponse |

Its theorems are identification theorems. Horvitz and Thompson's estimator of a survey and the inverse-probability-weighted estimator of an experiment are one estimator. The fundamental problem of causal inference is a missing-data problem.

**The boundary** (peira's decision 0002):
- **peira:** a method whose object is the mechanism of assignment, sampling, censoring or response, and what it identifies.
- **tyche:** inference within a given model.
- **nomos:** measures and mechanisms of social life.

peira depends on calx, morphe, physis and tyche, and not on nomos. Its repository is `https://github.com/huangwanhong-maker/peira`, a submodule at `foundations/peira`. In the commit order it comes after tyche.

### 2.2 Its Stages

| Stage | Content | State |
|---|---|---|
| 1 | `design` (assignment mechanisms, power), `randomisation` (Neyman, Fisher), `weighting` (Horvitz–Thompson, Hájek, IPW, the doubly robust estimator), `surveys` (strata and clusters, the ultimate-cluster variance, raking), `survival` (Kaplan–Meier, Nelson–Aalen, log-rank, Cox, Aalen–Johansen), `missing` (Rubin's rules, the approximate Bayesian bootstrap) | written, 89 checks |
| 2 | **The move.** Panel fixed effects, Poisson regression with absorbed effects and marginal effects from nomos to tyche's `regression`. Difference in differences and instrumental variables from nomos to peira's `quasi`, joined there by staggered adoption and event studies, regression discontinuity and synthetic control. Causal graphs (`graphs`) on morphe. The toolbox of `api/cpp/science/toolbox` points at their new homes; nomos keeps forwarding headers for one release, then drops them. | planned |
| 3 | `cohorts` (person-time, standardisation, the Lexis diagram, age–period–cohort and its exact non-identification), `synthesis` (meta-analysis), `sequential`, `sensitivity` (Rosenbaum, E-values, Manski), `protocol` (pre-registered plans and deviations) | planned |

Drawing (Kaplan–Meier curves, forest plots, Lexis diagrams, event-study plots, balance plots) is the infrastructure's `visualization/`, not peira's. The Studio's chart frames (ADR 0081) can draw them.

### 2.3 What Stage 1 Is Checked Against

- **Every assignment or sample of a design small enough to enumerate:** the expectations hold exactly. That covers unbiasedness, Neyman's variance identity, the conservativeness of his estimator, the level of Fisher's test, and the unbiasedness of the Horvitz–Thompson, Sen–Yates–Grundy and ultimate-cluster variance estimators.
- **statsmodels 0.15.0 and lifelines 0.30.3,** run by a script beside the fixtures, on Rossi's recidivism study and Walton's lifetimes: Kaplan–Meier with Greenwood's errors and log(−log) intervals, Nelson–Aalen, log-rank, and Cox with Breslow's and Efron's ties, to about 10⁻¹⁴.
- **Simulations with stated Monte Carlo error:** double robustness, the coverage of Neyman's and Rubin's intervals.

## 3. Alternatives Considered

- **A domain of quantitative social science.** Rejected, because a domain holds the objects of a science and these are methods. The social sciences' domains stand on peira, and the first is proposed separately (demography, already placed by the README's §4).
- **Extending tyche.** Rejected: designs are about how data come to be. Their theorems concern identification, not estimation within a model, and keeping them apart keeps both scopes crisp.
- **Leaving the quasi-experiments in nomos.** Rejected, as the user observed, because every experimental science would depend on a library of the social sciences.

## 4. Consequences

- **The documents change:**
  - CLAUDE.md lists eight foundations, with peira's row in §3.1 and its place in the layout and in the commit order;
  - the README's map gains peira;
  - the map of mathematics gives MSC 62D (sampling, missing data, causal inference from observational studies), 62K (design of experiments) and 62N (survival analysis and censored data) to peira;
  - the dependency checks know peira's place.
- **Amendments.** ADR 0048's tyche keeps everything within a model. ADR 0056's nomos gives up its quasi-experimental designs to peira and its estimators to tyche, in stage 2.
- **Repository.** peira's repository on GitHub is created by the user.
