# 0088. What People Pay For

- **Status:** Accepted, provisionally (2026-11-09).
- **Date:** 2026-11-09
- **Decided by:**
  - **The order.** Stage 4 of the urban priorities (`development/ideas/2026-11-08-urban-science-priorities.md`): "Demand estimation. Discrete choice estimated; stated-choice designs; spatial-lag and spatial-error models; hedonic rents".
  - **The go-ahead.** The user: "continue the next feature".
  - **The condition, from stage 3.** That what is built be reusable: "We don't want it just for a city".
  - **The design.** Claude.

## 1. Context

Observed use is demand passed through constraints (the priorities' §4). Preferences are measured in two ways:
- **revealed** in what people pay and choose: a home's price for what is near it, a route or a destination taken;
- **stated** in what they answer when shown alternatives that do not yet exist: a street with a lane and trees and a levy.

The workbench had neither. The infrastructure held Moran's I but no spatial regression, the multinomial logit but no model of alternatives' attributes, and designs of assignment but none of choice sets.

## 2. Decision

Layers, each general where it stands.

### 2.1 Spatial Regression in tyche

tyche's decision 0015:
- the spatial lag and spatial error models by maximum likelihood;
- the Lagrange multiplier tests, plain and robust;
- the lag model's impacts.

It has 14 checks against spreg on Anselin's Columbus data and on an unsymmetric W.

### 2.2 Choice Among Alternatives in nomos

nomos's decision 0009:
- the conditional logit, with willingness to pay by the delta method;
- the nested logit;
- the mixed logit with normal coefficients simulated by Halton draws.

It has 11 checks against statsmodels and on exact properties.

### 2.3 Stated-Choice Designs in peira

peira's decision 0007: D-errors at priors and coordinate exchange. It has 8 checks: the bound D₀ = 1/S is attained, and the search reaches it.

### 2.4 Demand in Urban Science

A module `demand`:
- **`hedonic.hpp`:** a hedonic model of prices on k nearest neighbours, the spatial model chosen by Anselin's rule, both spatial models and the impacts;
- **`stated.hpp`:** the plan of a stated-choice survey, a design at priors and the respondents each willingness to pay needs, held to simulation.

It has 6 checks.

### 2.5 Homes Sold as Cases in the Data Library

`data/products/home_prices.py`, with the registry's `nyc-property-sales`. Its first case is Brooklyn's one- to three-family homes sold in 2025, with straight distances to the subway, parks, protected lanes (from ADR 0087's record) and the centre.

### 2.6 The Workbench's Plug-in

*What people pay for* (lens Economy) has two layers:
- **what a home's price pays for:** each home coloured by its residual in the chosen model; the amenities' premiums under each model; the money at the median;
- **a survey about a street:** priors, a design, the respondents needed, and the cards to download.

## 3. Alternatives Considered

- **Revealed destination choice** (bicycle trips between stations, by the conditional logit with sampled alternatives). Deferred: the estimator is here; the trips' record and the sampling of alternatives are the next step.
- **Sparse log-determinants.** Deferred. The dense models take about 80 s for 2,000 homes; the page samples 1,200 or 2,000, and the study draws three samples.
- **Network distances to amenities.** Deferred: straight distances are the hedonic literature's usual measure, and the network's cost at the scale of a borough is large.

## 4. Consequences

- **What Brooklyn's prices pay for** (`research/experiments/2026-11-09-what-homes-pay-for`, three samples of 2,000 homes):
  - **Spatial dependence is strong.** Moran's I of the least-squares residuals is 0.37, and the rule chooses the lag model in every run (ρ 0.52–0.70).
  - **Protected lanes.** Least squares credits a protected lane within 300 m with +21% to +25%. The spatial models give +7% to +11% (lag, direct; +7%, 2% to 13%, in the first sample) or +4% to +11%, some intervals reaching zero (error).
  - **The subway.** Its −0.9% for 10% farther under least squares falls to −0.1% to −0.3%.
  - **What the gap means.** Part of what least squares credits to an amenity is the neighbourhood it was placed in. Neither model separates the amenity from an unmeasured advantage that comes with it.
- **The survey.** A design of 12 sets of three at plausible priors needs about 7 respondents for a lane's willingness to pay within ±$5 a month, and about 13 for trees. The counts scale as 1/h².
- **Next.**
  - Revealed destination choice on bicycle trips.
  - Sparse log-determinants for whole years of sales.
  - Stage 5, the generative design loop, which can score designs by these preferences.
