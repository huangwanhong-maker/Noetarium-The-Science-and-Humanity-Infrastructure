# A Region's Economy

*Status: draft (2026-11-23; [ADR 0166](../../../../development/decisions/0166-the-structure-of-regional-economies.md)).* A plug-in of the Urban Studio: how regions' economies are built and how they changed between two dates.

## 1. Concepts

- **Shift-share.** A region's change of employment split into what its country's growth, its mix of industries and its own departure from its industries' national growth account for; the country's employment is the sum of its regions.
- **Related and unrelated variety.** The entropy of a region's employment over industries, split exactly into the variety among sections (unrelated) and within them (related), in bits.
- **Industries held, and their complexity.** A region holds an industry when its location quotient is at least a threshold; the economic complexity index orders regions by the strongest division of the industries, and is not given where the regions fall apart into blocks.
- **The nearest industries.** For each region, the industries it does not hold that lie nearest those it does, by relatedness density.
- **Whether relatedness told.** Over every region, whether density at the first date told the industries regions took up by the second from those they did not, as an area under the curve.

## 2. Tools

| Tool | What it answers |
|---|---|
| *A region's economy: its structure and how it changed* | The regions of a country with their shift-share, variety, industries held, complexity, entries and exits, withheld cells and nearest industries; the shift-share terms by region; industries by complexity; the test of relatedness in a note |

## 3. Templates

- *The economies of Europe's regions, 2011–2019*: 188 NUTS 2 regions in 67 NACE divisions of the business economy, Czechia's regions first, then Germany's.

## 4. Data

The data library's `regional_structure.py` reads Eurostat's structural business statistics by NUTS 2 region (sbs_r_nuts06_r2: persons employed in local units by NACE division), reused with the source acknowledged, into `data/cache/products/regional-structure/`. Cells withheld for confidentiality are read as nothing and counted.

## 5. Engine

`engine/main.cpp`, around the API's `science/diversification.hpp`, is built by the Urban Studio's `engine.py` into `data/cache/engines/urban-studio/economy/`, and asked one request a process through `science.diversification`.
