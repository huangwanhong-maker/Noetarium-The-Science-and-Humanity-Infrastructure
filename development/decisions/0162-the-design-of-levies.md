# 0162. The Design of Levies: Bands, Caps and Dual Prices

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue with economics first. The survey *Evidence, methods and actions for steering cities* names "levy and fare design: bands, dual prices and caps, their revenue and incidence on visitors and residents" among chapter H's new modules (its §H.5), and ADR 0157 left levies by band, dual prices and caps as not built.

## 1. Context

ADR 0157 weighed one levy a night on all paid nights. Cities levy by class of accommodation or as a share of the price, cap nights or rentals, and charge residents and visitors differently at their attractions. Eurostat publishes the nights in hotels, holiday lodging and camping by region each year.

## 2. Decision

1. **The economics domain's `tourism` gains the design of levies** (theory note §2.1, 4 checks):
   - `banded`: each class its own market and levy (flat or a rate), with each class's levy over its price, which a flat levy makes larger for the cheap;
   - `cap_on_nights`: the levy whose equilibrium holds the nights at a cap (Brent's method), and the scarcity rent the cap creates between the visitor's and the operator's prices, the city's if the right to a night is sold and the operators' if given (the equivalence of a price and a quantity under certainty);
   - `dual_prices`: residents and visitors at an attraction, each with constant-elasticity demand, their visits, revenue, margin and surplus lost, and Lerner's price $c/(1 + 1/\varepsilon)$ where demand is elastic.
2. **The data library's `visitor_stock.py`** adds each place's nights by class of establishment (tour_occ_nin2).
3. **The API's `tourism.design`** (1 check) and the visitors plug-in's tool *Designing a levy*, with a step on Prague.

## 3. Alternatives Considered

- **A cap as a rule without a price.** Under certainty a cap and a levy reach the same nights (Weitzman 1974); stating the levy that holds the cap gives the cap a price, and the rent says who gains from it.
- **Optimal dual prices as Ramsey prices under a budget.** They need the attraction's budget and cross-effects; Lerner's rule for each group's margin, and the visits and surplus at declared prices, are what can be stated without them.
- **Prices by class from a survey.** None is open; prices are declared, and the nights are Eurostat's.

## 4. Consequences

- **What Prague shows** (2024 nights by class; declared prices of 90, 50, 25 and 60 euro a guest-night in hotels, holiday lodging, camping and platform rentals; elasticities −0.6 and 2):
  - a flat levy of 2 euro raises 47.4 million euro a year and a rate of 3% raises 56.6 million; the flat levy is 8.0% of a camping night and 2.2% of a hotel night, the rate 3% of each;
  - holding nights at 90% of today's would take a levy of 19.54 euro a night, with a scarcity rent of 422 million a year;
  - at a declared attraction where visitors pay 20 and residents 10, visitors' demand being inelastic, no price maximises its margin: a dual price raises revenue at the cost of their surplus.
- **Not built:** budgets and Ramsey prices; caps on rentals by area; levies' use (what the revenue pays for).
- **Provisional:** the declared prices, elasticities and attraction.
