# ecology

*Status: draft (2026-11-13; [ADR 0101](../../development/decisions/0101-ecology-founded-the-urban-forest.md)). Field, height 5, on geography, climate and evolution; C++ from the start, 144 checks.* Founded with code at the user's request, ahead of its place in tier 3, for the life of cities: its first object is a forest of individual trees, counted, named, measured once and planted by people; its theory note is [the-urban-forest.md](theory/the-urban-forest.md). Planned before then: Organisms among each other and in their environment: populations and their growth, the interactions that join species into communities, behaviour, biodiversity and its measures, the distributions of species and the biogeography that explains them, the flows of energy and matter through ecosystems, and the fisheries that harvest them. Behavioural ecology, biodiversity, biogeography and fisheries are its modules. Placed in the order with the other domains of life at the user's request (2026-10-31, [ADR 0061](../../development/decisions/0061-the-domains-of-life.md)); its work is tier 3.7 of the [README's order](../../README.md#46-the-order-of-implementation).

## 1. Purpose

- **A population is a dynamical system, often a structured one.** For one species: logistic growth, delays and harvest. For a population structured by age or stage: a non-negative projection matrix A. When A is primitive, its dominant eigenvalue λ is the asymptotic rate of growth, its right eigenvector the stable structure and its left eigenvector the reproductive values (Perron–Frobenius; Caswell 2001).
- **A community is a system of interacting populations.** Its models are Lotka–Volterra and consumer–resource equations; the Lotka–Volterra equations for n species are equivalent to the replicator equation for n + 1 strategies (Hofbauer and Sigmund 1998). A large random community whose self-regulation is scaled to one is stable, almost surely, only when σ√(nC) < 1, for n species, connectance C and interaction strength σ (May 1972). Food webs follow simple rules of niche (Williams and Martinez 2000).
- **Diversity is a family of means.** Hill's numbers ^qD = (Σᵢ pᵢ^q)^{1/(1−q)} are the effective numbers of species of order q: richness at q = 0, the exponential of Shannon's entropy as q → 1, the inverse of Simpson's index at q = 2 (Hill 1973). Rarefaction and estimators correct for what a sample misses (Chao 1984).
- **Distributions follow climate and history.** Where a species lives is a point process over the land whose intensity depends on climate and terrain; MaxEnt is such a model, a Poisson process fitted by penalised likelihood (Renner and Warton 2013). Island biogeography balances immigration against extinction (MacArthur and Wilson 1967); neutral theory asks what follows when species are ecologically equivalent (Hubbell 2001).
- **An ecosystem is a network of stocks and flows.** Energy enters by photosynthesis and passes up trophic levels, with losses at each (Lindeman 1942); carbon and nitrogen cycle through compartments.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `taxonomy` | Individuals named at species, genus and family, a rank possibly unknown; tallies at a rank with the unnamed apart; a species given two genera refused; taxonomic distinctness Δ⁺, Λ⁺ and Δ* (Warwick and Clarke) and the funnel of random draws from a master list | draft (15 checks) |
| `biodiversity` | An assemblage's Hill numbers of orders 0, 1, 2 at a rank, counted and estimated, with its coverage and Chao1 (tyche's `classes`); areas compared at one common coverage, set by the areas large enough; the partition among areas, equally weighted, with its overlaps | draft (18 checks) |
| `populations` | Stage-classified models from vital rates (Lefkovitch); λ, the stable structure, reproductive values, sensitivities and elasticities to entries and to vital rates (calx's non-negative matrices); life expectancy by stage and R₀; projection with an input and its equilibrium; what a constant input sustains and its exact sensitivities; demographic stochasticity. Logistic, delayed and harvested growth, Fisher–KPP spread: planned | draft (24 checks) |
| `forest` | A forest as an inventory of trees: allometry as declared schemes (biomass, roots, carbon; height and crown where a scheme states them), with the rank each estimate matched and the range of its source; composition against declared limits by rank; size classes; increments from one inventory; the stock of carbon; the forest's future with exact moments, the planting that holds carbon, and the stage model a forest implies; crowns on a grid, and the canopy the residents of each cell live under | draft (48 checks) |
| `connectivity` | Habitat as a network: the integral index and the probability of connectivity with each patch's importance split into intra, flux and connector parts; least-cost distances (morphe's Dijkstra) and circuit theory's effective resistances and current on resistance grids (physis's Kirchhoff networks) | draft (22 checks) |
| `communities` | Interacting populations: Lotka–Volterra and consumer–resource models, the replicator equation, food webs and the niche model, May's stability of random communities, assembly | planned |
| `behaviour` | Behavioural ecology: optimal foraging and the marginal value theorem (Charnov 1976), movement as random walks read from tracks, evolutionary games of behaviour | planned |
| `distributions` | Occurrences (GBIF, OBIS) against climate and terrain: species distribution models as point processes, ranges, biomes on climate's classes, island biogeography; occupancy from repeated surveys is peira's, used by the Urban Studio's plug-in | planned |
| `ecosystems` | Ecosystem services on a grid of land cover, InVEST's four closed-form models (ADR 0109): carbon stored in four pools; habitat quality under threats decaying linearly or exponentially, with the half-saturation curve; the annual water yield by Budyko's curve in Fu's form; soil loss by RUSLE with McCool's L and S. Every coefficient declared by the caller. Planned: trophic levels, primary production from NDVI, nutrient cycles, ecosystem accounts (SEEA) | draft (17 checks) |
| `fisheries` | Stock assessment: surplus production (Schaefer), stock and recruitment (Beverton–Holt, Ricker), yield per recruit, maximum sustainable yield | planned |

## 3. Dependencies

| Source | What ecology uses from it |
|---|---|
| geography | Terrain, land cover, remote sensing (NDVI), zonal statistics, regions |
| climate | Climate classes and normals (Köppen–Geiger), and through it meteorology and oceanography |
| evolution | Phylogeny (phylogenetic diversity), population genetics |
| physis | `discretisation` (Kirchhoff networks, for circuits of habitat); later `dynamics`, `chemistry` (reaction–diffusion, for spread), `statistical_mechanics` (birth–death processes) |
| tyche | `classes` (diversity and the number of classes, its decision 0017); later regression, spatial statistics and point processes |
| peira | Imperfect detection (its decision 0008), through the API for occupancy; declared for surveys and cohorts |
| morphe | `combinatorics` (shortest paths, for patches and least-cost distances) |
| calx | `linalg` (non-negative matrices, its decision 0016), random numbers, verification |

Physiology will be declared when `ecosystems` needs photosynthesis, or the metabolic theory of ecology (Brown et al. 2004).

## 4. Used By

The API's `science/ecology.hpp` composes it for any work's engine, and `science.ecology` asks it from Python. The Urban Studio's plug-in *Ecology and life* is its first work ([ADR 0102](../../development/decisions/0102-the-urban-studio.md)): Melbourne's 82,064 trees and New York's 2015 street-tree census through the data library's `urban_forest.py`. The Earth Atlas's vegetation (Sentinel-2's NDVI) and Köppen–Geiger classes are what `distributions` and `ecosystems` will read first.

## 5. Open Questions

- Whether ecology should stand on climate, which makes it the highest domain in the order (height 5, through air–sea), or on meteorology and oceanography directly until climate's bridge is complete. Climate's classes are needed early, so climate is declared.
- Microbial communities: microbiology's `communities`, or here once ecology declares microbiology.
- Eco-evolutionary dynamics, where ecological and evolutionary time meet: a module here, since ecology already stands on evolution.
- GBIF's occurrences carry CC0, CC BY or CC BY-NC record by record; the data library must keep each record's licence.
- Urban questions compose ecology with urban science through the API, since ecology stands above it in the order; a bridge of urban ecology would hold what is neither's, when such content appears.
- An allometry for southern trees (eucalypts, planes and elms by equations fitted to them), as a second declared scheme beside Jenkins et al.'s.
- Growth and mortality measured from two inventories of a city, replacing the increments one inventory suggests.

## 6. References

- Begon, M. and Townsend, C. R. (2021), *Ecology: From Individuals to Ecosystems*, 5th ed., Wiley.
- Kot, M. (2001), *Elements of Mathematical Ecology*, Cambridge University Press.
- May, R. M. (1973), *Stability and Complexity in Model Ecosystems*, Princeton University Press.
- Caswell, H. (2001), *Matrix Population Models*, 2nd ed., Sinauer.
- Hofbauer, J. and Sigmund, K. (1998), *Evolutionary Games and Population Dynamics*, Cambridge University Press.
- May, R. M. (1972), Will a large complex system be stable?, *Nature* 238, 413–414.
- Williams, R. J. and Martinez, N. D. (2000), Simple rules yield complex food webs, *Nature* 404, 180–183.
- Hill, M. O. (1973), Diversity and evenness: a unifying notation and its consequences, *Ecology* 54, 427–432.
- Chao, A. (1984), Nonparametric estimation of the number of classes in a population, *Scandinavian Journal of Statistics* 11, 265–270.
- MacArthur, R. H. and Wilson, E. O. (1967), *The Theory of Island Biogeography*, Princeton University Press.
- Hubbell, S. P. (2001), *The Unified Neutral Theory of Biodiversity and Biogeography*, Princeton University Press.
- Phillips, S. J., Anderson, R. P. and Schapire, R. E. (2006), Maximum entropy modeling of species geographic distributions, *Ecological Modelling* 190, 231–259.
- Renner, I. W. and Warton, D. I. (2013), Equivalence of MAXENT and Poisson point process models for species distribution modeling in ecology, *Biometrics* 69, 274–281.
- Lindeman, R. L. (1942), The trophic-dynamic aspect of ecology, *Ecology* 23, 399–417.
- Charnov, E. L. (1976), Optimal foraging, the marginal value theorem, *Theoretical Population Biology* 9, 129–136.
- Schaefer, M. B. (1954), Some aspects of the dynamics of populations important to the management of the commercial marine fisheries, *Bulletin of the Inter-American Tropical Tuna Commission* 1, 27–56.
- Beverton, R. J. H. and Holt, S. J. (1957), *On the Dynamics of Exploited Fish Populations*, Fishery Investigations, Series II, 19, HMSO.
- Ricker, W. E. (1954), Stock and recruitment, *Journal of the Fisheries Research Board of Canada* 11, 559–623.
- Brown, J. H., Gillooly, J. F., Allen, A. P., Savage, V. M. and West, G. B. (2004), Toward a metabolic theory of ecology, *Ecology* 85, 1771–1789.
- The sources of the written modules (Jenkins et al. 2003, Santamour 1990, Clarke and Warwick 1998, Chao and Jost 2012, Jost 2007, Usher 1966, McRae 2006, Saura and Rubio 2010 and others) are in the theory note's §9.
