# 0101. Ecology Founded, the Urban Forest First

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user, with a handbook of methods and tools for studying the material and ecological conditions of life: "We will next need tackle with an important topics in urban design, planning, and analysis. The ecosystem, biosystem, and life analysis methods." To the proposal to found ecology ahead of its place, and to begin with the urban forest: "yes!" and "forest is ok, building material stock can also do if have time". Then: "try best implent the things in the notes".
  - **The design.** Claude, here; ADR 0102 is the work in which it is used.

## 1. Context

Ecology was placed in the order at height 5, on geography, climate and evolution, with its modules planned (ADR 0061). It had no code. The handbook the user gave surveys a broad field: accounting (MFA, LCA, EEIO, SEEA), specialist simulators (SWAT+, MODFLOW, EPANET, EnergyPlus, PyPSA), ecological fieldwork and its statistics (diversity, occupancy with imperfect detection, ordination), systems methods (stock and flow, networks, sensitivity), qualitative and historical methods, impact assessment and foresight. It advises beginning with "a well-bounded resource or infrastructure case study", with a transparent method, one specialist model only where warranted, and evidence from the field.

The infrastructure's rule shapes where each method goes: a method that names no object in nature belongs to a foundation, and the domain uses it.

## 2. Decision

### 2.1 Foundations First

| Foundation | What it gained | Its record |
|---|---|---|
| calx | `linalg/nonnegative.hpp`: irreducibility and period; the Perron root with right and left vectors by inverse iteration after the QR's λ, so that imprimitive and reducible matrices are served; sensitivities and elasticities; the fundamental matrix, expected times to leaving, and R₀. A repair: `eigenvalues` never finished on the zero matrix | 0016 |
| tyche | `classes`, a new module: Hill and Rényi numbers, similarity-sensitive diversity, Jost's partition and the overlaps; Chao1, ACE, coverage, the asymptotic estimators; rarefaction and extrapolation with bootstrap curves; incidence data and Chao2. In `multivariate`: dissimilarities, PCoA, PERMANOVA and dispersion, the Mantel test | 0017 |
| peira | `detection`, a new module: site occupancy, the N-mixture model, closed capture–recapture, and distance sampling, each Horvitz–Thompson with an estimated inclusion probability | 0008 |
| physis | `solve_kirchhoff` takes a tolerance (default unchanged), for networks whose terminals have a high degree | 0027 |

### 2.2 The Domain

Ecology is founded with code, status draft, C++ from the start, in five modules (127 checks):

- `taxonomy`: individuals named at three ranks; tallies with the unnamed apart; taxonomic distinctness and its funnel;
- `biodiversity`: an assemblage's Hill numbers at a rank, observed and estimated; areas at one common coverage; the partition among areas, equally weighted;
- `populations`: stage-classified models from vital rates; their analysis on calx; projection with an input and its equilibrium; what a constant input sustains and its exact sensitivities; demographic stochasticity;
- `forest`: allometry as declared schemes; composition against declared limits; size structure; increments from one inventory; the stock of carbon; the forest's future with exact moments, the planting that holds carbon, and the stage model a forest implies; crowns on a grid, and the canopy that people live under;
- `connectivity`: IIC and PC with each patch's importance split three ways; least-cost distances and circuit theory on resistance grids.

It declares geography, climate and evolution (whose C++ it does not yet use) and the foundations calx, morphe, physis, tyche and peira. The theory note `domains/ecology/theory/the-urban-forest.md` states what is computed and proves what holds.

### 2.3 The API and the Data

- `api/cpp/science/ecology.hpp` composes the domain and peira's occupancy behind JSON operations: `forest`, `future`, `stages`, `population`, `distinctness`, `diversity`, `patches`, `circuits`, `canopy`, `occupancy`. `api/python/science/ecology.py` asks it.
- The data library holds Melbourne's urban forest (82,064 trees), its tree canopies of 2021 and New York's 2015 Street Tree Census. `data/products/urban_forest.py` writes them in one form, completing New York's families from the GBIF backbone by genus, and measures Melbourne's canopy block by block. What a record leaves unknown, or holds as a placeholder, is said to be.

### 2.4 Declared Schemes, Not Defaults

Allometry and limits are files that name their sources: Jenkins et al.'s (2003) estimators with Nowak's open-grown factor, and Santamour's (1990) limits. Every mapping the source does not make is marked as ours, and every estimate reports the rank at which it matched. Beyond the source's diameters, a scheme declares extrapolation or a cap. Diameters beyond a stated plausibility bound are set aside and counted.

## 3. Alternatives Considered

- **The methods inside the domain.** Rejected. Diversity estimators, imperfect detection and non-negative matrices name nothing living: vocabularies, k-mer libraries, epidemics and markets need them alike.
- **Ecology as a plug-in of the Urban Science Workbench without a domain.** Rejected. The Workbench draws; the knowledge of living things belongs to a library that any work can use (ADR 0026).
- **Wrapping specialist simulators (SWAT+, EnergyPlus, PyPSA) at once.** Deferred, as the handbook itself advises: one is taken when a case needs it, behind the infrastructure's interfaces.
- **Monte Carlo for the forest's future.** Rejected as the method. Under independent removals the moments are exact and immediate. Simulation is kept as a check.
- **Survival in the stage view from a stated life's first year.** Rejected after the first run on Melbourne: it gave σ ≈ 1 and lives of six centuries. The constant hazard with the same mean remaining life replaces it.
- **Material flow accounting and the stock of buildings.** Deferred. Their place (a domain of social metabolism, or modules of economics and architecture) is a decision of its own. A dynamic stock model is a cohort renewal process, which peira's cohorts and survival could hold.

## 4. Consequences

- **Checks.**
  - ecology: 127;
  - calx: 35 more;
  - tyche: 216 more (1,059 in all);
  - peira: 124 more (292 in all).
- **What Melbourne shows** (the City's record of 2025-09-22; presumptions declared in each answer):
  - Every rank fails Santamour's limits: *Eucalyptus camaldulensis* 10.3%, *Eucalyptus* 21.6%, Myrtaceae 36.0%.
  - 642 species are counted; a fuller count would find about 707.
  - The 20 precincts amount to 4.1 distinct communities of order 1.
  - Five precincts lie below the funnel of distinctness.
  - Homes stand under 15.8% canopy where the mean block has 20.1%; three quarters of homes lie in blocks under 20%.
  - With 3,000 trees planted a year about 105,000 trees are sustained, the total answering most to the survival of the youngest class.
- **Not built:**
  - species distributions as point processes;
  - repeated inventories and measured growth;
  - an allometry for southern trees;
  - ecosystem accounts (SEEA);
  - the material stock of buildings;
  - the handbook's qualitative, assessment and foresight methods (the Urban Studio's boards are where they would be written, ADR 0102).
- **Provisional:** the domain's position above urban science in the order. Urban questions compose the two through the API, not by declaration.
