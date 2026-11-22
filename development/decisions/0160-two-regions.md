# 0160. Two Regions: Where a City's Effects Fall, in the City and in the Rest of Its Country

- **Status:** Accepted, provisionally; its figures amended by [ADR 0164](0164-platform-and-registered-nights-overlap.md), which removes the overlap of platform and registered nights
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue with economics first. ADR 0158 and ADR 0159 named the rest of the country's own losses from a region's shock as not counted.

## 1. Context

A region's own table (ADR 0159) treats its purchases from the rest of its country as a leak: the effects there, and what comes back, are lost. The national table counts everything and says nothing of where. The two-region model of Miller and Blair keeps both regions, and the location quotients that built the region's table build the rest's from the rest's employment.

## 2. Decision

1. **The economics domain's `regional` gains `two_region`** (theory note §3, 5 checks): each region's coefficients by its own location quotient, what they leave bought from the other region; Proposition 3 (the four blocks sum to the national flows exactly), Proposition 4 (feedback: the region's block of the two-region inverse is at least its one-region inverse), Proposition 5 (aggregation: $G(I - A_2)^{-1} = (I - A)^{-1} G$, so totals are the country's).
2. **The API's `regional.two`** answers a table of 2n products ("r:" and "s:" before each code) with the split households' purchases follow; `tourism.accounts` splits a purchase made in the region among the region, the rest of the country and abroad, and reports value added in each; `recovery.path` reads the 2n table as it is (1 check).
3. **The visitors tools** take a third table, *the region and the rest (two regions)*: value added in the city and elsewhere; output lost in the city and elsewhere. The template gains the step *When Prague's visitors stay away: the losses in Prague and in the rest of Czechia*.

## 3. Alternatives Considered

- **A multiregional table of every region.** It needs employment by industry for all regions and gives the same location-quotient coefficients a heavier form; two regions answer the question asked, a city against the rest.
- **Trade between the regions by gravity.** It needs distances and flows not in the open data; the location quotients' remainders are the trade, and Proposition 3 holds them to the national flows.
- **The region's table with the rest added afterwards.** The leak's effects in the rest, and their return (feedback), need the joint inverse.

## 4. Consequences

- **What Prague shows** (Czechia's table of 2022; Prague's employment of 2022; FLQ with δ 0.3):
  - Prague's visitors' spending gives the country's 1,829 million euro of value added exactly (Proposition 5), 1,498 million of it in Prague and 331 million in the rest of Czechia;
  - half the visitors' spending lost for 90 days costs the country 544 million euro of output, the national total, of which 430 million falls in Prague and 114 million elsewhere; the elsewhere's largest loser is the food industry.
- **Not built:** more regions; regional productivity in the regions' outputs; trade by gravity.
- **Provisional:** the default of the country's table in the tools; δ.
