# 0166. The Structure of Regional Economies

- **Status:** Accepted, provisionally
- **Date:** 2026-11-23
- **Decided by:** the user's question of what economics remains for urban science, with their word to continue. Of the remaining work (the structure and diversification of regional economies; losses from the vulnerability module carried into the recovery; uplift estimated from records; rents and short-term rentals; distributions of income and wealth; interregional tables), the first is the most directly urban, rests on open data, and is the economics of evolution's planned `diversification` (ADR 0165), the survey *Evolutionary economics models and methods* (its Part IV) giving its measures.

## 1. Context

A city's or region's economy is read by how its employment changed against its country's, how varied and how related its industries are, which industries lie near those it holds, and how its set of industries places it among others. The literature (Dunn; Esteban-Marquillas; Frenken, Van Oort and Verburg; Hidalgo, Hausmann and co-authors; Mealy, Farmer and Teytelboym) gives exact measures and one out-of-sample test, whether relatedness predicts the industries regions take up, and it warns that the economic complexity index is only the position along the strongest division of the industries.

## 2. Decision

1. **The foundations first.** nomos's `structure` (its decision 0018): shift-share with Esteban-Marquillas's split, related and unrelated variety, relatedness (proximity and density), the complexity indices of regions and activities on tyche's bipartite ordering, fitness and complexity. tyche's `ranks` (its decision 0036): Mann and Whitney's U and the area under the curve.
2. **The economics domain's `diversification`** (theory note `theory/regional-structure.md`, 10 checks): a table of regions' employment by industry at two dates, each region with its reference (its country, whose employment is the sum of its regions) and each industry with its group (its section); for each region shift-share, variety in bits, industries held with their complexity, entries and exits, and the nearest industries not held; over the table, the areas under the curve of density for entries and for exits. A cell withheld at either date is left out at both, so that withholding does not appear as change. The complexity index is reported as a position on the strongest division, and refused where the regions fall apart into blocks.
3. **The data library's `regional_structure.py`**: Eurostat's structural business statistics by NUTS 2 region (sbs_r_nuts06_r2, persons employed in local units by NACE division), reused with the source acknowledged.
4. **The API's `science/diversification.hpp`** (`diversification.assess`, 5 checks) and `science.diversification`.
5. **The Urban Studio's plug-in *A region's economy***, with the tool *A region's economy: its structure and how it changed* and the template *The economies of Europe's regions, 2011–2019* (Czechia's regions, then Germany's).

## 3. Alternatives Considered

- **Withheld cells as nothing at each date.** A cell present in one year and withheld in the other then appears as a gain or a loss; leaving it out at both dates removes that and keeps the count.
- **The complexity index as "complexity".** Among Europe's regions the most diversified are those of manufacturing, so the conventional sign places coal mining, textiles and apparel at the top and capital regions of services at the bottom; reading it as sophistication would mislead. It is reported as the position on the division it is.
- **Employment by NACE section from the regional accounts.** Ten groups cannot show related variety, which needs divisions within sections; the business statistics have them, for the business economy alone (B to N less K).

## 4. Consequences

- **What Europe's regions show** (188 NUTS 2 regions, 67 divisions, 2011 and 2019; 2,870 of 25,192 cells withheld):
  - relatedness density in 2011 told the industries regions took up by 2019 (620 of 8,323 candidates) from those they did not at an area under the curve of 0.659 (p ≈ 10⁻⁴⁰), and the industries kept from those given up at 0.627: the principle of relatedness, out of sample, on European regions;
  - the strongest division of the industries runs from advertising, film, head offices and air transport to coal mining, textiles and apparel; Praha and Köln lie toward the first end, Severovýchod and Tübingen toward the second;
  - Praha's employment grew by 133,600, of which 67,100 beyond what its industries did nationally (its competitive term); Severozápad's fell by 12,100 while its country's growth alone would have added 24,200;
  - Köln's and Darmstadt's competitive terms (−136,500 and −109,600) are large and survive leaving withheld cells out; whether they are economic or a change in how local units are recorded is not established here.
- **Pictures:** three in the gallery's §55 (Czechia's shift-share, the strongest division of the industries, the regions in figures).
- **Not built:** entries predicted as a model held out in time with its calibration; employment by finer classes; the regions' own industrial histories (Neffke's skill relatedness from labour flows); the branching model of Frenken and Boschma, until its equations are read.
- **Provisional:** the plug-in's name and colour; the threshold of one.
