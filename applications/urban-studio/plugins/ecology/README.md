# Ecology and Life

*The Urban Studio's first plug-in: the living city, on the ecology domain.* Status: **draft** (2026-11-13; [ADR 0101](../../../../development/decisions/0101-ecology-founded-the-urban-forest.md), [ADR 0102](../../../../development/decisions/0102-the-urban-studio.md)). Version 0.1.

## 1. Concepts

A city's trees are a population of individuals: counted, named, measured, removed and planted by people. The life around them is seen only in part, at the visits someone made. The plug-in asks of them what a city's ecologist and its planners ask: what the forest is made of; how diverse it is, and what a fuller count would find; what it holds; what it will become under the plantings tried; who lives under its shade; how it holds together as habitat; where a species lives though not always seen.

The mathematics is the ecology domain's, on calx, morphe, physis, tyche and peira, asked through the API's engine (`science/ecology.hpp`). Its theory note, [`domains/ecology/theory/the-urban-forest.md`](../../../../domains/ecology/theory/the-urban-forest.md), says what is proved. The tools say their results plainly, and say what is presumed.

## 2. Tools

| Tool | Asks of | Answers |
|---|---|---|
| What the forest is made of (`forest.py`) | trees | the composition by species, genus and family, overall and by area, against declared limits rank by rank; size classes; the carbon held, by declared allometry, with how each tree matched the scheme |
| Diversity, and what a fuller count would find (`diversity.py`) | trees | Hill numbers of orders 0, 1, 2 at a rank, counted and estimated; coverage; each area at one common coverage; the curve of rarefaction and extrapolation with its bootstrap band; how many distinct communities the areas amount to |
| How far apart its kinds are (`distinctness.py`) | trees | each area's taxonomic distinctness against the funnel of random draws from the whole forest |
| Carbon, tree by tree (`carbon.py`) | trees with a place | each tree's carbon and CO₂, the rank matched and whether beyond the source's range, for the map and the Workbench |
| The forest's future (`future.py`) | trees, their lives | standing trees and carbon year by year with their exact spread, under removal and planting; the planting that holds today's carbon |
| The forest by size: a stage model (`stages.py`) | trees, their lives | classes of diameter with survival and growth; λ; years standing; the forest a planting sustains and the elasticities of what it sustains |
| A population by stages (`population.py`) | a table of vital rates | λ, structure, reproductive values, elasticities, expected lives, R₀; projection with an input and its equilibrium |
| Occupancy, though not always seen (`occupancy.py`) | sites by visits | ψ and p with site covariates (peira's imperfect detection); each site's chance of being occupied |
| The canopy people live under (`canopy.py`) | areas with cover and residents | the mean canopy over a home and the share of homes below each cover |
| How the canopy holds together (`connectivity.py`) | a grid of canopy cover | patches, the current between them (where movement is funnelled), effective resistances and least-cost distances, each patch's part in the probability of connectivity |

## 3. Declared Schemes

- `allometry-jenkins-2003.json`: Jenkins et al.'s (2003) national-scale estimators, with Nowak's (1994) open-grown factor of 0.8 and carbon at half the dry biomass. Genera and species are assigned to groups as their Tables 1 and 3 assign them; the mappings they do not make are marked as ours.
- `limits-santamour-1990.json`: no more than 10% of a species, 20% of a genus, 30% of a family.

A scheme is a file. Another city's equations, or another rule of composition, is a new file, and the tools offer it as a choice.

## 4. Templates

- **Melbourne's urban forest.** A plan of nine steps:
  - 82,064 trees with their arborists' assessments;
  - the measured canopy of 2021 by census block, with its dwellings;
  - composition and its limits, diversity of species and of genera, distinctness, carbon, the future with 3,000 trees planted a year, the stage model, the canopy homes stand under, and how the canopy holds together.
- **New York's street trees, 2015.** A plan of five steps:
  - the living trees of the 2015 census, their families from the GBIF backbone;
  - by neighbourhood: composition, diversity and distinctness;
  - Manhattan's street forest: its future and its stage model.

## 5. What Melbourne Shows

On the City's record of 2025-09-22, with the presumptions each answer states:

- **Composition.** Every rank fails Santamour's limits: *Eucalyptus camaldulensis* 10.3%, *Eucalyptus* 21.6%, Myrtaceae 36.0%.
- **Diversity.** 642 species are counted, and a fuller count would find about 707. The 20 precincts amount to 4.1 distinct communities of order 1.
- **Distinctness.** Five precincts lie below the funnel: Docklands, Port Melbourne, Parkville, Fishermans Bend and Princes Hill.
- **Carbon.** The 37,172 trees with a diameter hold about 20,000 t of carbon by the declared scheme.
- **Canopy.** Homes stand under 15.8% canopy where the mean block has 20.1%, and three quarters of homes lie in blocks under 20%.
- **Planting.** With 3,000 trees planted a year about 105,000 trees are sustained, the total answering most to the survival of the youngest class.
- **Connectivity.** The eight densest patches of canopy amount to an equivalent connected area of 66 ha of their 90.

## 6. Not Built

- species distributions as point processes;
- an allometry for southern trees;
- growth measured from two inventories;
- a city's own surveys of birds and bats for the occupancy tool;
- ecosystem accounts.
