## What People Pay For

*What a home's price pays for, and how to ask people what a street is worth.* Status: **draft**. Activity: research. [ADR 0088](../../../../development/decisions/0088-what-people-pay-for.md).

## 1. Layers

- **What a home's price pays for** (where a record of homes sold is held).
  - **The map.** Each home sold is coloured by what it sold for against what its rooms, land, age and place predict in the chosen model: rose dearer, sea glass cheaper.
  - **The card.** What the subway, a park, a protected lane and the centre add to a price, in three models:
    - least squares;
    - the spatial lag model, directly and with its spillover;
    - the spatial error model.
    
    The card also gives the Lagrange multiplier tests and the model they choose, and the lane's premium in money at the median price.
  - **The sample.** 1,200 or 2,000 homes, a seeded sample: the spatial models are dense, and a minute is the most a page should wait.
  - **The record today:** Brooklyn's one- to three-family homes sold in 2025. A city is added in `data/products/home_prices.py`.
- **A survey about a street** (anywhere).
  - **What it asks.** Priors for what a protected lane, trees and a minute on the way are worth against a levy, as logit coefficients.
  - **What it gives.**
    - A design of 8, 12 or 18 sets of three alternatives (peira's coordinate exchange);
    - each willingness to pay at the priors;
    - how many respondents each needs for a 95% interval of ±$2, ±$5 or ±$10 a month.
    
    The cards download as a table for a survey to field.

## 2. Engine

`engine/main.cpp`, on urban science's `demand`, tyche's spatial regression, nomos's conditional logit and peira's designs:
- `cases`: the records built;
- `hedonic`: the model on a case;
- `survey`: a plan.

## 3. Limits

- **A hedonic coefficient is an implicit price only where buyers sort freely,** and nothing left out moves with the amenity. The spatial models absorb dependence, not that: an amenity built where homes already cost more is credited with part of their price. The study shows least squares doing so for protected lanes.
- **Distances are straight,** to the nearest station, park edge or lane, and the year is one year of sales.
- **The survey's counts are the design's promise at the priors.** Wrong priors give wrong counts; the answers, once collected, are estimated by nomos's conditional logit.
