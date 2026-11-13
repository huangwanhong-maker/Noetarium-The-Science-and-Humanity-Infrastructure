# 0105. Social Provision, a Bridge

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user shared a research map that puts "people's lives in the future", rather than future city design, at the centre of inquiry. It names twelve domains and four programmes, beginning with "markets and social provision, followed closely by time, care, and relational life", and asks: "Does all imgrated and implemented?"
  - **The decision.** To the question where care should live, among options Claude set out, the user chose "a new bridge domain, social provision: households, care, public services and commons".
  - **The design.** Claude, here. ADR 0104 (sociology) is its companion.

## 1. Context

The map distinguishes economic theory, market studies and social provision: "many things that sustain life circulate through households, care relationships, public services, commons, voluntary associations, and non-market arrangements". The infrastructure held economics and markets, demography by age, and access to services in urban science. It had nothing of households, unpaid work, care or commons. README §4 listed no domain for them.

## 2. Decision

### 2.1 The Domain

**social-provision** is a bridge at height 2, over economics, markets, demography and sociology, C++ from the start (34 checks). Its theory note is `domains/social-provision/theory/time-care-and-provision.md`.

- `households`: equivalence scales as declared schemes; the value of unpaid work by three methods (UNECE 2017); full income; time poverty against a declared line.
- `care`: need from population and prevalence, with scenarios; supply and the gap; the care diamond; support ratios; the change of need split exactly among size, age structure and prevalence by the Shapley value.
- `services` and `commons`: planned.

### 2.2 What It Asked of the Domains Beneath

- **demography gains `health`** (13 checks): Sullivan's health expectancy with its sampling error, and the life course of an age-classified absorbing Markov chain of health, dependency and death on calx's fundamental matrix. The two agree exactly in a stationary population.
- **economics gains `labour`**, recorded in ADR 0104.

### 2.3 The API, the Data and the Work

- The API's `science/society.hpp` composes social provision, sociology, demography's `health` and economics' `labour`; `science.society` asks it from Python.
- Seven Eurostat datasets (CC BY 4.0): time use (HETUS), healthy life years, limitation in usual activities (EU-SILC), life tables, population, projections (EUROPOP2023) and labour market transitions. `data/products/time_and_care.py` turns them into tables, among them each country's day in eight parts.
- The Urban Studio's plug-in *Time, care and provision* brings the tools, its engine and the template *Time and care in Europe* (ADR 0102's contract).

## 3. Alternatives Considered

- **Care in epidemiology and sociology, no new domain.** This was the other option offered; the user chose the bridge. Care is not only health, and not only family: it is provision, whoever gives it.
- **Care in the markets domain.** Rejected: most care is not exchanged, and the map asks for exactly that distinction.
- **A dynamic model of care at once.** Deferred to the dynamics foundation, kinesis, in another session: care entered and left over a life course, and the carers' time given, is a composition of systems. This domain states the parts; the multistate chain of demography's `health` is the first.

## 4. Consequences

- **Checks.** social-provision 34; demography 13 more.
- **Validation.** Sullivan's method on Eurostat's life tables and ten-year bands of EU-SILC reproduces Eurostat's healthy life years at 65 to a median absolute difference of 0.10 years over 630 country-sex-years. At birth the answer depends on the prevalence assumed below 16, which is declared.
- **Not built:** public services and commons; satellite accounts of household production; the hours of care by level of need from data; care as a dynamic system.
- **Provisional:** the domain's name.
