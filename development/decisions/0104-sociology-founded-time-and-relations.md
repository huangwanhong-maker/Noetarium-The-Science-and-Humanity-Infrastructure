# 0104. Sociology Founded: Time and Relations

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user shared a research map that puts "people's lives in the future", rather than future city design, at the centre of inquiry. Its programmes begin with "markets and social provision, followed closely by time, care, and relational life". The user asked: "Does all imgrated and implemented?"
  - **The decision.** Claude answered that time, care and relational life were absent and set out where they could live. To the question which domains to found, the user chose "sociology and economics": sociology founded now with `time` (how a day is organised) and `relations` (networks, belonging), and work as a module `labour` of economics.
  - **The design.** Claude, here. [ADR 0105](0105-social-provision.md), the bridge of social provision, is its companion.

## 1. Context

README §4 listed sociology as a planned domain of the social sciences, with no module written. The map asks how people's days are divided among paid work, care, rest and others, who carries the unpaid work, and how lives are held among others. No layer held the mathematics those questions need.

- **Compositions.** Time use is compositional: a day holds 1440 minutes, so minutes cannot be compared one at a time.
- **Relational data.** Ties among people are dependent observations, tested by permutation.
- **Labour.** The flows between employment, unemployment and inactivity were nowhere in economics.

## 2. Decision

### 2.1 The Foundations First

tyche gains two modules ([tyche decision 0018](https://github.com/huangwanhong-maker/tyche/blob/main/development/decisions/0018-compositions.md), [0019](https://github.com/huangwanhong-maker/tyche/blob/main/development/decisions/0019-relational-data.md)):

- `compositions` (138 checks): Aitchison's geometry of the simplex, balances and pivot coordinates, the centre and variation of a sample, multiplicative replacement of zeros, and compositions as predictors and responses in linear models, with one-for-one and one-for-remaining reallocations.
- `networks` (101 checks): networks of people as data; censuses of dyads and triads, clustering, components and cores; assortativity, the E–I index and inbreeding homophily; Burt's brokerage; QAP and MRQAP; conditional uniform graph tests.

Strong components and core numbers are written in tyche's `networks` for now. They name no statistical object and belong in morphe's `combinatorics`. They move there when morphe is free of the other session's uncommitted work.

### 2.2 The Domain

**sociology** is a field at height 0 on the foundations alone, C++ from the start (55 checks). Its theory note is `domains/sociology/theory/time-and-relations.md`.

- `time` (30 checks):
  - a declared classification of activities into parts;
  - a day closed to 1440 minutes, its zeros replaced at the diary's slot;
  - a group's day, and two days contrasted;
  - who carries a part, and the balance of two kinds of time;
  - places compared by Aitchison's distances and their principal coordinates;
  - the isotemporal substitution table.
- `relations` (25 checks):
  - the whole network;
  - belonging by group, with a QAP test;
  - each person's ties: constraint, effective size, core, reach within two ties, diversity of contacts;
  - closure against networks of the same degrees.

  It is held to networkx 3.7 on Zachary's karate club.
- `institutions`, `stratification` and `culture`: planned.

### 2.3 Economics' Labour

economics gains `labour` (14 checks):

- a three-state chain of employment, unemployment and inactivity, read from flows;
- its steady state (Perron's vector on calx), the unemployment and participation rates it would hold, the mean spells and the half-life of a deviation;
- the change of the steady unemployment rate split exactly among the six flows by the Shapley value (nomos).

### 2.4 The API and the Work

- The API's `science/society.hpp` answers `day`, `reallocation` and `relations` beside ADR 0105's operations.
- The Urban Studio's plug-in *Time, care and provision* gains three tools:
  - *How a day is divided*, on any table of days;
  - *Time moved between activities*, on any sample of days with an outcome;
  - *Ties among people*, on any table of ties.
- Its template *Time and care in Europe* opens with the days of women and men in eighteen countries (Eurostat's harmonised time-use surveys of 2010).

## 3. Alternatives Considered

- **Time use in social provision only.** Rejected: a day's division is a sociological object before it is a measure of provision, and other domains (urban science's rhythms, health) will compose it.
- **Network statistics in morphe.** Graph algorithms are morphe's, but QAP, homophily and CUG tests are statistics of dependent observations, which are tyche's. Only the two algorithms named in §2.1 belong to morphe.
- **Compositional methods in nomos.** Rejected: compositions are general (geochemistry, microbiomes, budgets), not methods of the social sciences.
- **Labour as a domain of its own.** Not now: its first module is a market's flows, which economics holds.

## 4. Consequences

- **Checks.** tyche 239 more; sociology 55; economics 14 more; the Urban Studio's suite 10 more, on tables answered by hand and on the template's first steps (one of them guarding that groups of places are never added to their members).
- **Shown.** Across eighteen countries' days, household and family care separates women's from men's days most in sixteen. Women's and men's days lie on opposite sides of the first principal axis in every country. The balance of unpaid against paid work is positive for every country's women and negative for every country's men.
- **Not built:** days as sequences; essential zeros; networks in time (for kinesis); institutions and stratification; an open table of ties in the data library.
- **Provisional:** strong components and core numbers in tyche, until they move to morphe.
