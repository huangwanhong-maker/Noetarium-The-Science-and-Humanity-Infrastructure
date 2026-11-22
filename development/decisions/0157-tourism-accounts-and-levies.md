# 0157. Tourism Accounts and Levies: What Visitors' Spending Becomes, and What a Levy Does

- **Status:** Accepted, provisionally; its figures amended by [ADR 0164](0164-platform-and-registered-nights-overlap.md), which removes the overlap of platform and registered nights
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word, before sleeping, to continue and to "slightly make economics related features higher priority next". The survey *Evidence, methods and actions for steering cities* lists "a new module of tourism accounts and levies" (tourism ratios, direct value added kept apart from total effects; the design of levies, with their incidence) among the domains' next modules (its §4.2), and a tourism demand vector and leakage account, and levy design, among chapter H's (its §H.5).

## 1. Context

ADR 0156 gave the visitor stock: nights by month and origin in a city-region. What those visitors' spending is worth to an economy, and what a levy on their nights would raise, cost and fall on, needed economics the infrastructure had in parts: the economics domain's Leontief model, but no tourism demand, no split of value added, imports and taxes, and no incidence.

Eurostat publishes symmetric input–output tables at basic prices, product by product, with domestic and imported flows apart, for most member states.

## 2. Decision

1. **The economics domain's `tourism`** (theory note `domains/economics/theory/tourism-accounts-and-levies.md`, 16 checks):
   - **accounts**: visitors' spending by product carried to basic prices and split into domestic demand and imports; domestic demand through the domestic Leontief inverse to direct, indirect and total value added, induced imported inputs and taxes; Proposition 1 (they sum to domestic demand), leakage and the multiplier;
   - **levies**: the equilibrium of a flat levy (Brent's method on calx) and of a rate (in closed form) under constant elasticities; Proposition 2 (the burdens sum to the revenue; the small-levy pass-through ε_s/(ε_s − ε_d); the excess burden not negative and Harberger's triangle); the levy that raises most within a cap; a declared share of lost nights displaced to stays the levy does not reach.
2. **The data library's `input_output.py`** reads a country's naio_10_cp1750 into domestic flows, final use, value added, taxes, imported inputs and households' domestic shares, 65 products.
3. **The API's `science/tourism.hpp`** (`tourism.accounts`, `tourism.levy`), answered by the visitors plug-in's engine; products of no output are left out, which changes nothing else (2 checks).
4. **The Urban Studio's visitors plug-in** becomes *Visitors: how many, what they spend, what a levy does*, with the tools *What visitors' spending becomes* and *A levy on visitors' nights*, and the template's two new steps on Prague with Czechia's table.

## 3. Alternatives Considered

- **A full Tourism Satellite Account.** Its ten tables need inbound and internal consumption by product at purchasers' prices with margins reallocated, which no open source gives for a city. The direct measure the TSA defines (tourism ratio times value added) is computed, and the rest said to be missing.
- **Multipliers without the identity.** A value-added multiplier above one is a common error in impact studies (Crompton 2006, which the survey cites); Proposition 1 holds every result to the table's own accounting.
- **Regionalised coefficients now.** Location quotients need regional employment by product that Eurostat publishes only at a coarse breakdown; the national table is used and its bias toward too little leakage is said.
- **A levy fully passed on.** Incidence depends on supply: a declared supply elasticity gives the operators' share, and the full pass-through remains a reading (an infinite elasticity).

## 4. Consequences

- **What Prague shows** (24.0 million nights in 2024 in registered establishments and platform rentals; a declared 130 euro a foreign night and 80 a domestic one; Czechia's table of 2022):
  - 2,966 million euro of spending, 2,522 million at basic prices; 1,063 million of value added directly (0.41% of the country's) and 1,829 million along the supply chains, 0.73 per euro of domestic demand; 24.6% leaks abroad as imports;
  - a declared levy of 2 euro a night on a declared 60 euro night, with elasticities −0.6 of demand and 2 of supply: 47.2 million euro a year, 77% borne by visitors; 363,000 nights lost, 109,000 of them displaced, and 19.3 million of value added lost with the rest at 76 euro a night (the accounts' value added a night); the excess burden 0.36 million. With demand this inelastic the revenue rises to the end of the range searched.
- **Found while building:** Czechia's table records retail with wholesale and imputed rents with real estate; a basket product the table leaves empty is carried to the product that records it, and the note says so.
- **Not built:** a full TSA; regionalised coefficients; levies by band, dual prices and caps; the levy's use and its incidence on residents; elasticities estimated from records.
- **Provisional:** the basket, spending, price, elasticities and displaced share of the template; the plug-in's new name.
