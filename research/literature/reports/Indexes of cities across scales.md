# Indexes of Cities Across Scales: Their Estimation and Their Dynamics. A Survey

*Multi-scale dynamical models of cities and the methods that couple scales and fit them to records; the estimation of urban indexes a dataset does not hold, and the withholding of estimates that cannot be made well; urban indicator frameworks, the measurement of qualitative indexes, and qualitative quantities in dynamical models.*

Status: `draft` (a survey for design, 2026-11-18). Activity: research, in service of use. It serves [ADR 0128](../../../development/decisions/0128-indexes-and-cities-across-scales.md): the urban-science domain's catalogue of indexes (`domains/urban-science/indicators/`), kinesis's systems across scales (its decision 0007), tyche's change of support and prediction with its area of applicability (its decision 0028), and the Urban Studio's plug-in *Cities across scales*.

## 0. Scope, Method and Markers

### 0.1 The Question

A keyword atlas of urban economics, urban science, market analysis, political economy and resilient infrastructure (4,591 keywords in 30 sections) was given with three requests: to identify every qualitative and quantitative index it implies; to let the Urban Studio build multi-scale dynamical models whose quantities are those indexes, and draw them; and to estimate an index where a dataset does not hold it, by any suitable model, while an index that cannot be estimated well is taken up, declared, when a model is applied.

### 0.2 How the Survey Was Made

Three surveys were written in parallel from the literature, each by a separate agent working to one brief (the design they serve, what the infrastructure already holds, and the form of a recommendation), and are kept here whole as Parts I to III:

- **Part I**, multi-scale dynamical models of cities: classical urban dynamics, urban scaling and its critiques, multi-level land-use models, sectoral stock–flow dynamics, general multi-scale methods (fast–slow systems, multirate integration, coarse-graining, consistent aggregation and disaggregation, co-simulation, assimilation across scales), and calibration.
- **Part II**, estimating urban indexes a dataset does not hold: change of support, small-area estimation, proxies (night lights, imagery, phones) and their critiques, synthetic populations, temporal disaggregation and nowcasting, hierarchical Bayesian and machine-learning prediction with spatial cross-validation and the area of applicability, and the rules by which statistical offices withhold estimates.
- **Part III**, indicator frameworks (ISO 37120/37122/37123, UN-Habitat's Urban Monitoring Framework, SDG 11, the OECD and Eurostat), the canonical indexes and their algebra across spatial units, the measurement of qualitative indexes (ordinal and latent models, invariance, ecometrics, rubrics, street audits, fuzzy calibration), and qualitative quantities in dynamical models (fuzzy cognitive maps, qualitative simulation, ordinal state-space models, trust and opinion dynamics).

Part IV says what was built from them, and what was not.

### 0.3 Verification Markers

Every bibliographic item in Parts I–III carries one marker: **[V]** verified this session (2026-11-18) against a primary record (the DOI on Crossref's API, the arXiv API, or the issuing body's own page); **[S]** confirmed only from secondary sources; **[M]** from memory, not verified. Of the 772 items, 725 are [V], 22 [S] and 24 [M] (one item is [V] with an [S] note). The markers vouch for the bibliographic records; a summary of a paper's findings comes from its abstract where the part says so, and otherwise from the agent's knowledge of the paper, to be checked against the text before it is quoted. Formulas written from memory are flagged where they are given (the transition potential of Metronamica, Bettencourt's δ = 1/6, the potential function of Ellam et al.).

### 0.4 The Catalogue Is a Separate Record

The classification of the atlas's keywords into indexes was made by three further agents from the atlas alone, without browsing; its definitions and sources come from their knowledge and are unchecked. It is kept in the urban-science domain (`indicators/catalogue.tsv`, `indicators/keywords.tsv`) and described there, not here.

---


## Part I. Multi-Scale Dynamical Models of Cities, and the Methods That Couple Scales and Fit Them to Records

*Survey L1 for the design "multi-scale dynamical models of cities whose states are indexes". Compiled 2026-11-18.*

### 0. Scope and Markers

This survey covers (§1) the classical models that treat a city or a system of cities as a dynamical system; (§2) urban scaling and the critiques that bear on coupling city-level and intra-urban levels; (§3) multi-level land-use and land-use–transport models, with the mechanics of allocating control totals downward and feeding cells back upward; (§4) sectoral stock–flow dynamics that are standard and can be fitted to records (real estate, business demography, tourism, neighbourhood change, labour flows, municipal finance, infrastructure deterioration, resilience recovery, cascades); (§5) general multi-scale methods (fast–slow reduction, multirate integration, the heterogeneous multiscale method and equation-free computing, lumpability, consistent aggregation and disaggregation on a spatial hierarchy, co-simulation, data assimilation across scales); and (§6) calibration, identifiability and out-of-sample validation. For each model the entry gives **S** (state variables and their index), **P** (parameters, and whether they must be declared or can be fitted) and **D** (data needed). Each area closes with what it means for the design. Markers: **[V]** the bibliographic record (authors, title, venue, year, and volume/pages where given) was verified this session against Crossref's API, the arXiv API, or, for software, by resolving the URL; **[S]** confirmed only from secondary records (library catalogues, RePEc, search results); **[M]** from memory, not verified. A marker attests the *record*; statements about a paper's content come from its abstract where one was retrieved (said so) and otherwise from the reviewer's reading, and should be checked against the text before a check is built on them.

### 1. Classical Urban Dynamics as Dynamical Systems

#### 1.1 Forrester's Urban Dynamics

- **Concept.** Forrester (1969) [S] modelled a bounded urban area as a system-dynamics stock–flow model whose life cycle (growth, maturity, stagnation) is endogenous. Nine levels in three sectors: population (underemployed, labour, managerial–professional), housing (underemployed housing, worker housing, premium housing) and business (new enterprise, mature business, declining industry), joined by flows (arrivals, departures, upward mobility, construction, obsolescence, demolition) whose rates are multiplicative "multiplier" functions of attractiveness ratios. The famous result was counter-intuitive policy: low-cost housing programmes deepen decline, while demolishing slum housing and encouraging new enterprise revive the city.
- **S** nine stocks for one city (no space inside it); **P** about a hundred table functions and normal rates, *declared* by the modeller, never estimated; **D** none required in the original; a calibration would need counts by class of residents, dwellings and establishments over decades.
- **Failures and critiques.** Kadanoff (1971) [V] showed the policy conclusions follow from the assumed attractiveness functions and the closed boundary (the city's surroundings are an infinite reservoir at fixed conditions). Lee (1973) [V], "Requiem for large-scale models", listed the "seven sins" (hypercomprehensiveness, grossness, hungriness, wrongheadedness, complicatedness, mechanicalness, expensiveness) and is still the standard warning against unvalidated urban models. Alfeld (1995) [V] reviews fifty years of urban system dynamics.
- **Reference implementation.** Vensim/XMILE versions circulate; PySD [V] runs such models in Python and could serve as an independent integrator for a check.

#### 1.2 Allen and Sanglier's Self-Organising Central-Place Models

- **Concept.** Allen and Sanglier (1979 [V]; 1981 [V]) wrote coupled logistic equations for population and for employment in functions of several levels at each centre of a lattice, with carrying capacities that depend on the jobs present and demand allocated to centres by a spatial-interaction rule. Stochastic perturbations nucleate new centres ("order through fluctuations"), so the final hierarchy depends on the history of fluctuations, not only on parameters.
- **Form.** For population $x_i$: $\dot x_i = b\,x_i\,(N_i - x_i) - m\,x_i$ with $N_i = \sum_k \gamma_k S_i^k$; for employment of function $k$: $\dot S_i^k = a_k S_i^k (D_i^k - S_i^k)$, where $D_i^k$ is the demand reaching $i$ from all residents through a distance-decay rule.
- **S** $x_i$, $S_i^k$ on a set of centres; **P** growth rates, jobs-per-resident multipliers, distance decay, attractiveness, noise intensity (declared; the original work did not fit them); **D** population and employment by function and centre over time.
- **What it shows.** Path dependence and multiplicity of equilibria are generic, so a forecast is a distribution over histories (Brown et al. 2005 [V] made the same point for land-use ABMs, §6.7).

#### 1.3 Weidlich and Haag's Sociodynamics

- **Concept.** A population distributed over $L$ regions is a configuration $\mathbf n = (n_1,\dots,n_L)$; individuals move with transition rates $w_{ij}(\mathbf n) = \nu_{ij}\exp[u_j(\mathbf n) - u_i(\mathbf n)]$, where $u$ is a dynamic utility and $\nu_{ij}$ a mobility. The configuration's probability follows a **master equation**; its first moments follow mean-value equations $\dot{\bar n}_i = \sum_j [\bar n_j w_{ji}(\bar{\mathbf n}) - \bar n_i w_{ij}(\bar{\mathbf n})]$ (Haag and Weidlich 1984 [V]; Weidlich 1991 [V], the 160-page *Physics Reports* review; Weidlich's *Sociodynamics* (2000) [S]; Haag 2017 [V]).
- **Agglomeration.** With $u_i = \kappa n_i - \sigma n_i^2 + \delta_i$ (agglomeration $\kappa$, saturation $\sigma$), the system has a phase transition from an even distribution to agglomeration as $\kappa$ rises (Weidlich and Haag 1987 [V]).
- **Estimation — the part most useful here.** Because $\ln(F_{ij}/n_i) = \ln \nu_{ij} + u_j - u_i$ for observed flows $F_{ij}$, regional utilities and mobilities are identified up to a constant by a log-linear regression on a panel of origin–destination flows; the utilities are then regressed on regional indicators ("key factors"). *Interregional Migration* (Weidlich and Haag, eds., 1988) [V] applied this to six countries.
- **S** $n_i(t)$ (population by region, or households by class and district); **P** $\nu_{ij}$ (fitted, often $\nu_0 f(d_{ij})$), $u_i(t)$ (fitted per period, then explained by indexes), $\kappa,\sigma$ (fitted or declared); **D** an origin–destination flow table per period, region stocks.
- **Guarantees.** Exact mean-field closure when rates are linear in $\mathbf n$; otherwise the mean-value equations are an approximation valid for large populations (van Kampen expansion). Detailed balance holds for $\nu_{ij}=\nu_{ji}$ and utilities derived from a potential, giving a closed-form stationary distribution — a usable check.

#### 1.4 Harris and Wilson's Structural Dynamics, and Its Bayesian Inference

- **Concept.** Flows are fast, sizes slow. Spending $O_i$ at origin $i$ is allocated to centres $j$ by a production-constrained spatial interaction model, $S_{ij} = O_i W_j^{\alpha} e^{-\beta c_{ij}} / \sum_k W_k^{\alpha} e^{-\beta c_{ik}}$; the floor space $W_j$ grows where revenue $D_j = \sum_i S_{ij}$ exceeds the cost $\kappa W_j$: $\dot W_j = \varepsilon\,W_j\,(D_j - \kappa W_j)$ (Harris and Wilson 1978 [V]).
- **Bifurcations.** For $\alpha > 1$ the equilibrium set changes abruptly as $\alpha$, $\beta$ or $\kappa$ vary: centres vanish or appear discontinuously (Wilson 1981, *Catastrophe Theory and Bifurcation* [M]; Clarke and Wilson 1983 [V]; Dearden and Wilson 2011 [V], 2015 [V]). Wilson (2008) [V] generalised the scheme to Lotka–Volterra competition with entropy-maximising flows; Wilson (2010) [V] reviews entropy in urban modelling.
- **Bayesian inference (the modern line).**
  - Ellam, Girolami, Pavliotis and Wilson (2018) [V] write the structural variables through a single potential function and model their evolution by stochastic differential equations (an overdamped Langevin diffusion), so the stationary density of sizes is a Boltzmann–Gibbs density in the potential. From the abstract: the spatial-interaction parameters "can be estimated from the structure alone, independently of flow data"; the posterior is doubly intractable and is sampled by MCMC; the case study is London's retail system. In log sizes $X_j = \ln W_j$ the potential is, as the reviewer recalls it, $V(X) = -\alpha^{-1}\sum_i O_i \ln \sum_j e^{\alpha X_j - \beta c_{ij}} + \kappa \sum_j e^{X_j}$ plus a small term that keeps vanishing centres off the boundary.
  - Gaskin, Pavliotis and Girolami (2023) [V] train a neural network to output parameters, with the multiagent model as forward solver (abstract: applied to synthetic data and to economic activity across Greater London for Harris–Wilson). The follow-ups infer networks from time series (2024, *PNAS Nexus*) [V] and calibrate epidemic models (2024, *PLOS ONE*) [V]; code in NeuralABM [V].
  - Zachos, Damoulas and Girolami (2024) [V] sample origin–destination tables subject to marginal constraints for population synthesis, and Zachos, Girolami and Damoulas (2024, NeurIPS) [V] generate OD matrices jointly with a neural spatial interaction model.
- **S** $W_j$ (floor space, or any size index: jobs, outlets, attractiveness) per centre, flows $S_{ij}$ (fast, equilibrated); **P** $\alpha$, $\beta$ (fitted, from flows or from sizes alone), $\kappa$ (fitted or declared as a cost per unit size), $\varepsilon$ (time scale; only identifiable from time series of $W$), noise intensity (fitted); **D** origin totals $O_i$, a cost matrix $c_{ij}$, observed sizes $W_j$; flows optional; a time series of $W$ for $\varepsilon$.
- **What it guarantees.** At an interior equilibrium $D_j = \kappa W_j$ exactly; for $\alpha<1$ the equilibrium is unique (the potential is convex), for $\alpha>1$ there are many. Both are closed-form checks.
- **Reference implementation.** PySAL's `spint` [V] (spatial interaction calibration, not the dynamics); NeuralABM [V].

#### 1.5 Pumain's Evolutionary Theory of Urban Systems

- **Concept.** A system of cities co-evolves: cities compete for and adopt innovations that arrive in waves, and the hierarchy is reproduced as each wave propagates from large to small cities; growth is near-proportional (Gibrat) with systematic deviations (Pumain et al. 2006, Cybergeo, record dated 2011 [V]; Pumain 2018 [V]; Pumain and Reuillon 2017 [V]).
- **Gibrat and Zipf.** Proportional random growth $P_{t+1} = (1+g_t)P_t$ gives lognormal sizes; with a lower barrier it gives Zipf's law (Gabaix 1999 [V]); over all places, sizes are lognormal (Eeckhout 2004 [V]); with cities delineated by clustering, Zipf holds for areas as well as populations (Rozenfeld et al. 2011 [V]). Verbavatz and Barthelemy (2020) [V] add migration between cities with heavy-tailed noise to the growth equation and explain the observed deviations from Zipf and Gibrat on several countries' records.
- **Gibrat revisited.** Favaro and Pumain (2011) [V] add spatial interaction and innovation cycles: in each cycle, the growth rate depends on size through the adoption of the innovation, so size–growth correlation changes sign across cycles.
- **SIMPOP family.** SIMPOP (Sanders et al. 1997) [V] is a multi-agent system in which cities with functions trade; SIMPOP2 simulates Europe and the United States over three centuries (Bretagnolle and Pumain 2010) [V]. SimpopLocal was calibrated with half a billion simulations by evolutionary algorithms on a grid (Schmitt et al. 2015) [V]; MARIUS (Soviet and post-Soviet cities) was grown model by model with each mechanism tested on data (Cottineau, Chapron and Reuillon 2015 [V]; Cottineau et al. 2015 [V]). Pattern Space Exploration looks for the patterns a model *can* produce, to falsify it (Chérel, Cottineau and Reuillon 2015) [V]. All run on OpenMOLE (Reuillon, Leclaire and Rey-Coyrehourcq 2013) [V].
- **S** populations of each city in a system (and functions or wealth); **P** growth rates, interaction distance decay, innovation diffusion parameters (fitted by search against long census series); **D** harmonised census populations of cities over one to two centuries, with consistent delineation.

#### 1.6 Synergetics after Haken and Portugali

- **Concept.** Synergetics (Haken 1977 [V]) studies systems whose few slow **order parameters** "enslave" many fast modes near an instability; the slaving principle is adiabatic elimination (the centre-manifold or Tikhonov reduction of §5.1). Portugali (2011) [V] and Haken and Portugali (2003 [V]; 2017 [V]; 2021, *Synergetic Cities* [V]) apply it to cognition and the city: information adaptation, inter-representation networks, and a phase-transition reading of urban allometry.
- **Data.** Conceptual and qualitative; no fitted model to cite. Its value for the design is the language: a level's slow index is an order parameter, and the faster variables beneath it are slaved.

#### 1.7 What This Means for the Design

- **Harris–Wilson is the canonical template for two coupled levels**: a fast flow equilibrium (spatial interaction, already in the infrastructure as Wilson's family and Evans's combined equilibrium) inside a slow size dynamics, with parameters inferable from sizes alone. It serves the indexes *floor space, outlets, jobs by centre, attractiveness*, at the level of centres within a city, coupling by quasi-steady state.
- **Weidlich–Haag gives a fitted law of movement between units of any level** (districts within a city, cities within a region) whose utilities become indexes explained by other indexes; the master-equation tools already present apply.
- **Pumain's line gives the city-in-system level** and a working calibration practice (massive simulation, pattern exploration).
- **Forrester and Lee give the warning**: a model with declared multipliers is a scenario tool, not a forecast; the design should label it so.

### 2. Urban Scaling and Its Critiques

#### 2.1 The Scaling Hypothesis

- **Claim.** For a city of population $N$, an aggregate $Y$ scales as $Y = Y_0 N^\beta$: sublinear for infrastructure ($\beta \approx 0.85$), linear for individual needs, superlinear for socio-economic output ($\beta \approx 1.15$) (Bettencourt et al. 2007 [V]).
- **Theory.** Bettencourt (2013) [V] derives $\beta = 1 + \delta$ for outputs and $1 - \delta$ for networks, $\delta = H/[D(D+H)]$, with $D=2$, $H=1$ giving $\delta = 1/6$ (as the reviewer recalls the formula). Lobo et al. (2013) [V] read it as a production function. Ribeiro and Rybski (2023) [V] review the mechanisms proposed (interaction kernels, networks, hierarchy, combinatorics). Gomez-Lievano, Patterson-Lomba and Hausmann (2016) [V] explain prevalence, scaling and variance by the number of complementary factors a phenomenon needs; Gomez-Lievano, Youn and Bettencourt (2012) [V] connect scaling statistics to Zipf's law.
- **Residuals as indexes.** Scale-adjusted metropolitan indicators $\xi_c = \ln(Y_c / Y_0 N_c^\beta)$ measure a city's performance net of size and are persistent over decades (Bettencourt et al. 2010) [V]. Europe: Bettencourt and Lobo (2016) [V]; Strano and Sood (2016) [V].

#### 2.2 The Critiques

- **City definitions.** Arcaute et al. (2015) [V] built thousands of systems of cities for England and Wales from commuting and density thresholds; abstract: "most urban indicators scale linearly with city size, regardless of the definition", and where nonlinear "the exponent fluctuates considerably". Cottineau et al. (2017) [V] repeat this for France with thousands of definitions. Louf and Barthelemy (2014) [V] show that even the sign of CO₂ scaling depends on the definition. Arcaute et al. (2016) [V] construct the hierarchy by percolation. Harmonised delineations exist (the Degree of Urbanisation, Dijkstra et al. 2021 [V]).
- **Statistics.** Leitão et al. (2016) [V] (abstract): with fluctuations modelled explicitly, the evidence for $\beta \ne 1$ "crucially depend[s] on the fluctuations … how they are modelled, and on the fact that the city sizes are heavy-tailed". Shalizi (2011, arXiv) [V] argues that per-capita and total regressions give different and partly spurious pictures, and that a hierarchy of cities is as consistent with the data. Finance and Cottineau (2019) [V] show that dropping cities with zero counts biases exponents.
- **Composition.** Keuschnigg, Mutgan and Hedström (2019) [V] (abstract): "population characteristics and selective migration explain around half of the previously reported superlinear urban scaling" (Swedish registers).
- **Cross-section is not a growth law.** Depersin and Barthelemy (2018) [V] (abstract): congestion delays "depend on both the population and the whole history of the system, prohibiting the existence of simple scaling forms". Bettencourt et al. (2020) [V] (abstract): cross-sectional and temporal exponents "lead to the measurement of different mixtures of the same fundamental parameters"; temporal exponents diverge when population growth vanishes.
- **Space.** Altmann (2020) [V] and Ribeiro et al. (2017) [V] derive scaling from distance-dependent interactions, so that the exponent is a property of an interaction kernel *and* a boundary.

#### 2.3 What This Means for Coupling City-Level and Intra-Urban Levels

- **Scaling is a diagnostic or a prior, never a law of motion.** A cross-sectional $\beta$ must not be used as the growth law of one city (Bettencourt et al. 2020; Depersin and Barthelemy 2018).
- **Every scaling relation must carry its delineation.** Fit it with an explicit fluctuation model (lognormal or Poisson–lognormal, after Leitão et al.), report $\beta$ with its interval and the evidence for $\beta \neq 1$, and record the delineation (e.g., functional urban areas of the Degree of Urbanisation).
- **The consistent bridge runs from cells upward.** Declare an intra-urban interaction law (Altmann's kernel: $Y_c = \sum_{u,v \in c} K(d_{uv})\,n_u n_v$) at the cell level; the city-level relation is then *derived*, and its dependence on the boundary is explicit (the MAUP of §5.5).
- **Indexes served.** The SAMI is itself an index ("productivity net of size") at city level; a city-level control total for a quantity with no intra-urban record (e.g., patents, GDP) may be set as $Y_0 N^\beta$ with its predictive interval, then allocated downward (§3.6).

### 3. Multi-Level Land-Use Models

#### 3.1 Constrained Cellular Automata Driven by Regional Models

- **Concept.** White and Engelen (1993) [V] showed a CA with neighbourhood rules reproduces fractal urban form; White, Engelen and Uljee (1997) [V] *constrained* it: the number of cells of each active land use per year is fixed exogenously, and cells are allocated in order of transition potential until each demand is met. White and Engelen (2000) [V] stacked three levels: national macro totals, a regional spatial-interaction model distributing activity among regions, and the CA within each region. The Environment Explorer for the Netherlands (Engelen, White and de Nijs 2003) [V] and the commercial Metronamica descend from it; White, Engelen and Uljee (2015) [V] is the monograph.
- **Transition potential** (as recalled): $P_{k,x} = v \cdot A_{k,x} \cdot S_{k,x} \cdot Z_{k,x} \cdot N_{k,x}$, with accessibility $A$, suitability $S$, zoning $Z$, the neighbourhood effect $N$ (distance-weighted sums over land uses within a radius of about eight cells) and a stochastic term $v = 1 + (-\ln r)^{\alpha}$, $r \sim U(0,1)$.
- **Upward feedback.** A region's attractiveness in the regional model includes aggregates of its cells (available suitable land, neighbourhood quality), so the slow regional allocation responds to the cells.
- **S** a land-use class per cell (active, passive, fixed classes); regional activity totals (population, jobs by sector); **P** neighbourhood weights by pair of uses and distance (dozens to hundreds; fitted by hand or by optimisation), $\alpha$, accessibility weights, regional interaction parameters; **D** land-use maps at two or more dates, suitability and zoning layers, networks, regional population and employment series.
- **Calibration and validation.** Newland et al. (2018) [V]: multi-objective optimisation of neighbourhood rules, accounting for stochasticity. van Vliet et al. (2016) [V] review practice and find calibration and validation often not separated. Kappa Simulation measures agreement *in change* relative to persistence (van Vliet, Bregt and Hagen-Zanker 2011) [V]; neutral models give benchmarks (Hagen-Zanker and Lajoie 2008) [V].

#### 3.2 SLEUTH

- **Concept.** A self-modifying CA of urban growth (Clarke, Hoppen and Gaydos 1997 [V]; Clarke and Gaydos 1998 [V]) on six layers (Slope, Land use, Exclusion, Urban extent, Transportation, Hillshade), with four growth rules (spontaneous, new spreading centres, edge, road-influenced) driven by five coefficients (diffusion, breed, spread, slope resistance, road gravity). When growth is fast or slow the coefficients are multiplied up or down ("boom" and "bust").
- **Calibration.** Brute-force Monte Carlo over coarse, fine and final grids, scored by fit metrics; Dietzel and Clarke (2007) [V] proposed a composite (OSM) metric. Regional application and a regionalised version: Jantz, Goetz and Shelley (2004) [V]; Jantz et al. (2010) [V].
- **Levels.** One level, no exogenous demand; the growth total is endogenous to the coefficients. That makes it a contrast case: it cannot respect a control total.
- **S** urban/non-urban per cell (and land use); **P** five coefficients plus self-modification thresholds (fitted by search); **D** at least four historical urban extents, a road network at several dates, slope, exclusion layer. **Code** in C, from the Gigalopolis project and USGS's SLEUTH-3r [S].

#### 3.3 Demand-Driven Allocation (CLUE-S, FLUS)

- **CLUE-S** (Verburg et al. 2002) [V]: regional land demands per year (from trends or a macro model) are allocated to cells by $\text{prob}_{x,u} = P_{x,u} + \text{ELAS}_u + \text{ITER}_u$ (logistic suitability, a conversion elasticity, and an iteration variable adjusted until the allocated area equals the demand), subject to a conversion matrix.
- **FLUS** (Liu et al. 2017) [V]: system dynamics projects demands (top-down); a neural network gives suitability; a CA with "self-adaptive inertia" and roulette selection allocates them (bottom-up). Neural-network CA: Li and Yeh (2002) [V]; review of CA practice: Santé et al. (2010) [V]; Batty and Xie (1994) [V].
- **Guarantee.** The control total is met exactly by construction (iteration or ranking); the spatial pattern is not guaranteed.

#### 3.4 Land-Use–Transport Models with Nested Levels and Time Steps

- **Reviews.** Wegener (1994 [V]; 2004 [V]; 2021 [V]); Acheampong and Silva (2015) [V]; Moeckel et al. (2018) [V]; Waddell (2011) [V]. MEPLAN's spatial input–output with transport (Echenique et al. 1990) [V].
- **Speeds of change.** Wegener's table (1994, 2004; content recalled) orders subsystems by speed: very slow (networks, land use: decades), slow (workplaces, housing: years to decades), fast (employment, population: years), immediate (travel, goods movement: hours to days). This is the time hierarchy the design needs, already argued in the literature.
- **UrbanSim** (Waddell 2002 [V]; Waddell et al. 2003 [V]). Annual steps. Exogenous **control totals** of households and jobs (from regional macro forecasts) are realised by transition models, then placed by location-choice models (multinomial logit with sampled alternatives) at parcel, building or grid-cell level; a developer model builds space by pro forma; a hedonic model sets prices; the travel model runs every few years and its accessibilities are held between runs (a sample-and-hold coupling). **Bayesian melding** (Poole and Raftery 2000 [V]; applied to UrbanSim by Ševčíková, Raftery and Waddell 2007 [V]) turns multiple stochastic runs and observed zone counts into calibrated predictive intervals for household counts by zone. Code: `UDST/urbansim` [V].
  - **S** households and jobs (agents with attributes), buildings and parcels, prices; **P** choice coefficients (fitted by maximum likelihood per submodel), development costs and rules (declared), control totals (exogenous); **D** parcels, buildings, synthetic households, establishments, zoning, sales, travel skims.
- **ILUTE** (Miller et al. 2004 [V]; Salvini and Miller 2005 [V]): microsimulation of persons, households, dwellings and firms that evolve in time (demographic events, housing market with clearing, activity-based travel), with steps from months to a year.
- **PECAS** (Hunt and Abraham 2005) [V]: an aggregate **Activity Allocation** module solves each year for prices that clear a spatial input–output economy among zones (fast equilibrium), and a **Space Development** module microsimulates developers on parcels in disequilibrium (slow); prices pass down, built space passes up the next year. It is the cleanest published instance of the fast-equilibrium/slow-stock split.
- **MATSim** (Horni, Nagel and Axhausen, eds., 2016) [V]: agent plans for one day relaxed by co-evolutionary iterations (execute, score, replan) to a stationary state — the fast level when coupled to a land-use model. Code: `matsim-libs` [V].

#### 3.5 How Control Totals Go Down and Cells Feed Back Up

The models above use five allocation operators, each with its guarantee:

1. **Proportional to a declared weight** (dasymetric shares): exact totals; no behaviour.
2. **Ranked allocation with capacities** (constrained CA, CLUE-S): exact totals; the pattern follows the potential.
3. **Choice with balancing** (UrbanSim): agents choose by a logit; the total is met by the number of agents placed; shares follow utilities.
4. **Minimum information under margins** (iterative proportional fitting, Deming and Stephan 1940 [V]; the same as Wilson's entropy maximisation): the unique table closest in Kullback–Leibler divergence to a seed that meets all margins, when feasible.
5. **Equilibrium prices** (PECAS, the housing model already in the infrastructure): the total is met by prices that clear.

Upward feedback is always an **aggregate of cell states** entering the slow level's attractiveness (available land, mean neighbourhood quality, accessibility): an extensive sum, an intensive weighted mean, or a decomposable index (§5.5).

#### 3.6 What This Means for the Design

- **Reference multi-level land-use model.** The constrained CA under a regional model (White–Engelen) is the one to reproduce: city or regional totals per year (indexes: population, jobs by sector, built area) allocated to cells by ranked potential, cell aggregates fed back.
- **One family of allocation operators.** The design's "allocation downward under control totals" should be one library of operators (§3.5), each with its exactness property as a check (totals met to machine precision; IPF's fixed point equal to a known table).
- **Couplings.** PECAS's fast-equilibrium/slow-development split and UrbanSim's sample-and-hold of accessibility are the two coupling patterns to name in the design (§5.1, §5.2). The infrastructure's year-by-year land use and transport model already follows the first.
- **Uncertainty.** Bayesian melding is the published route from stochastic runs to calibrated zone-level intervals.

### 4. Sectoral Stock–Flow Dynamics That Can Be Fitted

#### 4.1 Housing and Commercial Property

- **Four quadrants.** DiPasquale and Wheaton (1992) [V] link the space market (rent $R$ clears demand $D(R, E)$ against stock $S$), the asset market (price $P = R/i$), construction ($C = f(P)$, positive above replacement cost) and stock adjustment ($\Delta S = C - \delta S$). In steady state $S^* = C^*/\delta$, a closed form.
- **Dynamics.** DiPasquale and Wheaton (1994) [V] estimate a stock-flow model in which prices adjust gradually and construction responds to price. Poterba (1984) [V] gives the asset-market stock-flow model of housing. Wheaton (1999) [V] shows cycles arise from construction lags and myopic expectations, damped or explosive depending on the elasticities and on depreciation; Wheaton (1987) [V] for the US office market; Wheaton, Torto and Evans (1997) [V] for London.
- **Vacancy and rents.** Rents adjust to the gap between vacancy and a "natural" (structural) vacancy rate: $\Delta R_t / R_{t-1} = \lambda (V^* - V_{t-1})$ (Rosen and Smith 1983 [S] for rental housing; Wheaton and Torto 1988 [V] for offices; Gabriel and Nothaft 1988 [V], 2001 [V] on natural vacancy rates differing by metropolitan area and on incidence and duration of vacancy). Error-correction forms: Hendershott (1996) [V]; Hendershott, MacGregor and White (2002) [V].
- **Supply and durability.** Supply elasticity depends on geography and regulation (Saiz 2010 [V]); construction responds to price changes (Mayer and Somerville 2000 [V]); durable housing makes decline slow and asymmetric, with prices below construction cost (Glaeser and Gyourko 2005 [V]); Glaeser et al. (2014) [V] fit a dynamic urban model to price and construction moments.
- **S** stock $S$, vacancy $V$, rent $R$, price $P$, starts or completions $C$ (by submarket: district or city; quarterly or annual); **P** demand elasticities, natural vacancy $V^*$, adjustment speed $\lambda$, construction lag and elasticity, depreciation $\delta$ (all fitted from series; $V^*$ often fitted per submarket); **D** stock counts, vacancy surveys, rent and price indexes, permits or completions, interest rates.

#### 4.2 Business Demography

- **Concepts.** Entry, exit and survival are hazards. The Eurostat–OECD manual (2008) [V] fixes definitions (enterprise births, deaths, survival rates, high-growth enterprises) — the measurement standard for the index. Dunne, Roberts and Samuelson (1988) [V] and Geroski (1995) [V] give stylised facts (high turnover, entry and exit correlated across industries, low survival of entrants). Audretsch and Mahmood (1995) [V] fit hazard models of new-firm survival. Organisational ecology adds density dependence of founding and mortality (Hannan and Freeman 1977 [V]).
- **Model.** Births as a Poisson process with rate depending on the stock and on demand; deaths by a hazard with age dependence (Weibull, Cox); the stock $B_{t+1} = B_t + \text{births} - \text{deaths}$.
- **S** stock of establishments by sector and district, age distribution; **P** entry rates, hazard shape and covariate effects (fitted); **D** a business register or census with opening and closing dates (the infrastructure already reads such a record for Melbourne's cafés).

#### 4.3 Tourism

- **Butler's life cycle** (Butler 1980) [V]: exploration, involvement, development, consolidation, stagnation, then decline or rejuvenation; a logistic curve of visitors with a carrying capacity. Lundtorp and Wanhill (2001) [V] derive it from a diffusion process and estimate it on visitor series; Casagrandi and Rinaldi (2002) [V] write a three-variable ODE (tourists, environment, capital) and analyse sustainable and oscillating regimes. Carrying capacity: O'Reilly (1986) [V]. Doxey's "irridex" of residents' attitudes (euphoria, apathy, annoyance, antagonism; Doxey 1975 [M]) is a qualitative index that a model can take as a state with a declared range.
- **S** visitors (or bed-nights), capacity (beds), environmental quality, resident attitude; **P** growth rate, carrying capacity (fitted from series), feedback strengths (declared); **D** arrivals or bed-nights, bed stock, a resident survey for attitude.

#### 4.4 Gentrification and Neighbourhood Change

- **Mechanisms.** Tipping (Schelling 1971 [V]; tipping points estimated from tract data by Card, Mas and Rothstein 2008 [V]); the rent gap between potential and capitalised ground rent (Smith 1979 [V]; as an ABM, Diappi and Bolchi 2008 [V]); household flows in and out of low-income neighbourhoods (Ellen and O'Regan 2011 [V]); endogenous gentrification with house-price dynamics (Guerrieri, Hartley and Hurst 2013 [V]).
- **Fitted descriptions.** Spatial Markov chains of relative-income classes conditioned on neighbours' class (Rey 2001 [V]; PySAL `giddy` [V]); sequences of neighbourhood types clustered (Delmelle 2016 [V]).
- **Dynamical models.** An ODE model with a desirability-raising subpopulation, desirability and price, with transient chaos under rent control (Shaw, Restrepo and Rodríguez 2024, arXiv) [V]; an ABM of three income groups relocating by living costs, with a network measure detecting gentrification earlier (Mauro et al. 2024, arXiv) [V]. Neither is calibrated to records in the abstracts seen.
- **S** class shares or relative-income class per tract, rents, a desirability index; **P** transition probabilities (fitted, from panel class data), tipping points (fitted), feedback strengths (declared); **D** tract panels of income, rents, tenure and moves (censuses, registers).

#### 4.5 Labour-Market Flows

- **Flows.** Unemployment's steady state $u^* = s/(s+f)$ from the separation rate $s$ and the job-finding rate $f$, estimated from duration data (Shimer 2012) [V]; matching (Mortensen and Pissarides 1994) [V]. Regional adjustment to employment shocks is mostly by migration (Blanchard and Katz 1992) [V]. The infrastructure's economics module `labour` already holds the flows.
- **S** employed, unemployed, inactive by area; **P** flow rates (fitted); **D** labour-force survey flows or registers.

#### 4.6 Municipal Fiscal Stocks and Debt

- **Identity.** Debt ratio $b_{t+1} = \frac{1+r}{1+g} b_t - pb_t$ (primary balance $pb$; Escolano 2010, IMF guide) [V]. Sustainability by a fiscal reaction function $pb_t = \rho b_{t-1} + \dots$ with $\rho > 0$ (Bohn 1998) [V].
- **Local governments.** Panel VARs of revenues and expenditures (Holtz-Eakin, Newey and Rosen 1989) [V]; a VECM of revenue, grants, expenditure and debt service under the intertemporal budget constraint for over a thousand US municipalities, with grants more important for adjustment in large and highly indebted cities (Buettner and Wildasin 2006 [V]; content from a search summary). Fiscal health measures: Hendrick (2004) [V].
- **S** debt, revenue, expenditure, grants per municipality (annual); **P** interest rate, growth, reaction coefficient, adjustment speeds (fitted); **D** municipal accounts series.

#### 4.7 Infrastructure Deterioration and Resilience Recovery

- **Deterioration.** Markov transition probabilities from condition ratings (Madanat, Mishalani and Ibrahim 1995) [V] or from duration models of time in state (Mishalani and Madanat 2002) [V]; already implemented in the infrastructure for road surfaces.
- **Resilience triangle.** Bruneau et al. (2003) [V]: loss of resilience $R = \int_{t_0}^{t_1} [100 - Q(t)]\,dt$ for quality $Q(t)$ (in %) after a shock at $t_0$, recovered at $t_1$; four properties (robustness, redundancy, resourcefulness, rapidity). Cimellaro, Reinhorn and Bruneau (2010) [V]: recovery functions linear, exponential and trigonometric, and a normalised index $\int Q\,dt / T$. Chang and Shinozuka (2004) [V]: robustness and rapidity against performance standards; Zobel (2011) [V]: trade-offs within the triangle; Ouyang, Dueñas-Osorio and Min (2012) [V]: resistant, absorptive and restorative stages; review of measures: Hosseini, Barker and Ramirez-Marquez (2016) [V].
- **Curves from data.** Cao, Miles and Choe (2022) [V]: recovery curves by Gaussian process regression on expert-elicited data, checked against two Japanese earthquakes. Hsu and Mostafavi (2025) [V]: over 2,000 empirical mobility-based resilience curves after US weather hazards, grouped into three archetypes (content from a search summary).
- **S** functionality $Q(t)$ of a system or area; **P** drop, recovery time, functional form (fitted to post-event series, or declared from expert elicitation); **D** a performance series before and after events (service restoration, mobility, business reopening).

#### 4.8 Cascades on Interdependent Networks

- **Models.** Percolation on interdependent networks collapses abruptly (first-order) where single networks degrade gracefully (Buldyrev et al. 2010 [V]; networks of networks, Gao et al. 2012 [V]); load redistribution with capacity margins (Motter and Lai 2002) [V]; blackouts near criticality with power-law sizes (Dobson et al. 2007) [V]. Kinds of interdependency: physical, cyber, geographic, logical (Rinaldi, Peerenboom and Kelly 2001) [V]; modelling approaches compared (Ouyang 2014) [V].
- **S** state of each node or link per system; **P** dependency map (declared), capacity margins (declared or fitted), failure thresholds; **D** network topologies, dependencies, event records (rarely complete).

#### 4.9 What This Means for the Design

- **Each sector is a small stock–flow template with a closed-form steady state.** Each is fittable by least squares or likelihood on short annual or quarterly series. Each lives naturally at district or city level and passes its stock down to cells through the allocation operators of §3.5 (construction to parcels, openings to blocks).
- **Fast–slow pairs inside each sector.** Rent clears fast against a slow stock (four quadrants); flows in labour markets equilibrate within a year against slower population. The same coupling machinery (§5.1) applies.
- **Qualitative indexes enter as declared states.** Resident attitude (Doxey) and desirability (gentrification models) are examples: a state with a declared range and a declared feedback, flagged as such, unless a measurement model estimates it.
- **Resilience curves are a separate time scale.** Weeks to months after a shock, sitting between the hourly and yearly levels, with closed-form integrals to check.

### 5. General Multi-Scale Methods

#### 5.1 Fast–Slow Systems

- **Tikhonov's theorem** (1952 [M]): for $\varepsilon \dot x = f(x,y)$, $\dot y = g(x,y)$, if the slow manifold $x = h(y)$ solving $f(h(y),y) = 0$ is uniformly attracting (eigenvalues of $\partial_x f$ with negative real parts), solutions converge on finite time intervals to the reduced system $\dot y = g(h(y), y)$ as $\varepsilon \to 0$, with $O(\varepsilon)$ error after an initial layer. **Geometric singular perturbation** (Fenichel 1979 [V]) gives an invariant slow manifold within $O(\varepsilon)$ of $h$ that persists for small $\varepsilon > 0$. Monograph: Kuehn (2015) [V].
- **Quasi-steady state.** Segel and Slemrod (1989) [V] give the validity criterion in terms of time scales (the fast relaxation time short against the slow variable's change).
- **Averaging.** When the fast variable is oscillatory or ergodic rather than attracted to a point, the slow dynamics is the average of $g$ over the fast invariant measure (Pavliotis and Stuart 2008) [V]; stochastic fast variables give homogenised drift and diffusion.
- **Failures.** The reduction fails at fold points of the slow manifold (relaxation oscillations, canards; Kuehn), exactly where Harris–Wilson's bifurcations sit, and when the separation ratio is not small.
- **For the design.** The rule "fast variables equilibrated within slow steps" is Tikhonov's reduction and needs two declared checks: the fast equilibrium is attracting (Jacobian), and the ratio of time scales is small. For non-equilibrating fast levels (traffic over a day), the slow level should receive *time averages* of fast outputs over the slow step (averaging principle), not a snapshot.

#### 5.2 Multirate Integration

- **Methods.** Multirate linear multistep methods (Gear and Wells 1984) [V]; multirate partitioned Runge–Kutta (Günther, Kværnø and Rentrop 2001) [V]; multirate infinitesimal GARK, in which the fast system is integrated exactly or by any method between slow stages, with order conditions up to four (Sandu 2019) [V]. Reference implementation: SUNDIALS ARKODE's MRIStep [V].
- **Guarantees.** Order and stability conditions are known; the coupling error of naive sample-and-hold (the slow input held constant over the fast sub-steps) is first order in the slow step.
- **For the design.** Levels stepped at different rates should use either an MRI-type scheme (when the levels are one ODE system split by rate) or a co-simulation master (§5.6) when the levels are of different forms. Either way the coupling order should be declared and tested on a linear two-rate problem with a closed-form solution.

#### 5.3 The Heterogeneous Multiscale Method and Equation-Free Computing

- **HMM** (E and Engquist 2003) [V]: the macro model's form is assumed known, but a constitutive quantity (a flux, a rate) is missing; it is estimated on the fly by short, constrained micro simulations. Error splits into macro discretisation, relaxation and sampling errors. Assumes scale separation.
- **Equation-free** (Kevrekidis et al. 2003 [V]; Kevrekidis and Samaey 2009 [V]): no macro equation is written. *Lift* coarse variables to a micro state, *simulate* briefly, *restrict*, estimate the coarse time derivative, and *project* over a long step (coarse projective integration). Steady states and bifurcations of the coarse system are found by Newton–Krylov on the coarse time-stepper. Applied to agent-based models (Siettos, Gear and Kevrekidis 2012) [V] and social networks (Tsoumanis et al. 2010) [V].
- **Failures.** Both need the coarse variables to close the dynamics after a short healing time; lifting is ill-defined when micro configurations matter (spatial arrangements in Schelling-type models).
- **For the design.** The infrastructure's balance laws of a density on a graph of cells (macro) could take their diffusion and drift coefficients from short runs of the walker or household-move agents (micro) — an HMM. The equation-free coarse bifurcation analysis is a way to give agent-based urban models the existing equilibria and bifurcation tools.

#### 5.4 Coarse-Graining and Lumpability

- **Exact lumpability.** A partition of a Markov chain's states is lumpable iff for all blocks $A, B$ and states $i, j \in A$: $\sum_{k \in B} P_{ik} = \sum_{k \in B} P_{jk}$ (Kemeny and Snell 1960 [M]). Exact and ordinary lumpability are distinguished by Buchholz (1994) [V].
- **Near-decomposability.** When $P = P^* + \varepsilon C$ with $P^*$ block-diagonal, each block reaches internal equilibrium quickly and blocks interact slowly; aggregate variables then follow their own dynamics (Simon and Ando 1961) [V]. This is the theorem behind a spatial hierarchy: neighbourhoods equilibrate internally before districts exchange.
- **Approximate aggregation.** Kullback–Leibler-optimal aggregation via spectral theory (Deng, Mehta and Meyn 2011) [V] and via the information bottleneck (Geiger et al. 2015) [V]; PCCA+ (Deuflhard and Weber 2005) [V], implemented in `deeptime` [V].
- **Agent-based models as Markov chains.** Macro observables are lumpable only for homogeneous interaction structures; heterogeneity breaks lumpability (Banisch, Lima and Araújo 2012 [V]; Banisch 2016 [V]).
- **Ecology's aggregation theory.** Perfect aggregation conditions (Iwasa, Andreasen and Levin 1987) [V]; aggregation by separation of time scales for fast migration between patches and slow demography (Auger et al. 2008) [V] — the direct analogue of fast residential moves within a city and slow population change.
- **For the design.** The infrastructure already holds lumpability and its coupling bound. Two additions: (i) a near-decomposability test (spectral gap within blocks against coupling between blocks) to justify each level of the spatial hierarchy for a Markov model of class or tenure transitions; (ii) KL-optimal aggregation to *choose* districts from cells when the hierarchy is not given.

#### 5.5 Consistent Aggregation and Disaggregation on a Spatial Hierarchy

- **Aggregation operators by kind of index.**
  - Extensive (counts, floor space): sums.
  - Intensive (rents, densities): weighted means with declared weights.
  - Ratios: ratio of sums, never mean of ratios.
  - Inequality: only the generalised-entropy family is additively decomposable into within- and between-group parts (Shorrocks 1980) [V]; the Gini is not.
  - Segregation: the multigroup entropy index decomposes across levels (Reardon and Firebaugh 2002) [V].

  An index without a decomposable form must be recomputed from microdata or carried with its sufficient statistics.
- **Disaggregation.** Volume-preserving smooth interpolation (pycnophylactic; Tobler 1979) [V]; IPF under margins (Deming and Stephan 1940) [V]; change of support for combining incompatible zones (Gotway and Young 2002) [V]; dasymetric weights (another survey's subject).
- **Temporal.** Annual totals and monthly indicators made coherent by Denton's (1971) [V] quadratic adjustment or Chow and Lin's (1971) [V] regression-based interpolation; R `tempdisagg` (Sax and Steiner 2013) [V].
- **Forecast and estimate reconciliation.** Estimates made independently at several levels are made coherent by $\tilde y = S(S^\top W^{-1} S)^{-1} S^\top W^{-1} \hat y$, with summing matrix $S$ and error covariance $W$ (MinT; Hyndman et al. 2011 [V]; Wickramasuriya, Athanasopoulos and Hyndman 2019 [V]). Python `hierarchicalforecast` [V], R `fable`/`hts`.
- **MAUP as a constraint.** Spatial interaction parameters change with the zoning system (Openshaw 1977) [V]; regression coefficients change unpredictably with aggregation (Fotheringham and Wong 1991) [V]; Openshaw's CATMOG (1984) [M] names the scale and zoning effects.
- **For the design.** Three rules follow:
  1. Every index declares its aggregation operator (sum, weighted mean with weights, decomposable form, or "recompute from microdata").
  2. Every fitted parameter records the unit system it was fitted on, and is not transferred to another level without re-estimation.
  3. Estimates at several levels are reconciled (MinT) so that the hierarchy is coherent before a model runs on it.

#### 5.6 Co-Simulation and Master Algorithms

- **FMI.** The Functional Mock-up Interface packages a model as an FMU exposing `doStep` (co-simulation) or derivatives (model exchange) (Blochwitz et al. 2011) [V]; FMI 3.0 adds clocks and events (fmi-standard.org [V]). Reference implementations: FMPy [V], the Modelica Association's Reference-FMUs [V].
- **Master algorithms.** Jacobi (parallel, all parts use last communicated inputs) and Gauss–Seidel (sequential, later parts use updated inputs), with input extrapolation; iterative schemes (waveform relaxation) converge under a contractivity condition (Arnold and Günther 2001) [V]; algebraic loops between parts can make non-iterative coupling unstable (Kübler and Schiehlen 2000) [V]; determinacy needs step negotiation or rollback (Broman et al. 2013) [V]. Survey: Gomes et al. (2018/2019) [V]. Multi-level agent simulations: four design patterns (Mathieu, Morvan and Picault 2018) [V]; Morvan (2012, arXiv) [V].
- **For the design.** The infrastructure's co-simulation of parts of different forms is a master algorithm. It should declare which (Jacobi or Gauss–Seidel, extrapolation order, iteration), its communication step per pair of levels, and a check on a coupled linear oscillator split in two whose exact solution is known (the textbook test in the survey).

#### 5.7 Data Assimilation Across Scales

- **Filters.** The ensemble Kalman filter (Evensen 1994) [V; review Houtekamer and Zhang 2016 [V]] and the particle filter (Gordon, Salmond and Smith 1993) [V]. Particle filters need ensembles growing exponentially with the effective dimension (Snyder et al. 2008 [V]; review van Leeuwen et al. 2019 [V]). Parameters are estimated by augmenting the state.
- **On a hierarchy.** Multiscale recursive estimation on trees (quadtrees): an exact Kalman-type estimator with a fine-to-coarse and a coarse-to-fine sweep, costing linear time in the number of nodes (Chou, Willsky and Benveniste 1994 [V]; Willsky 2002 [V]). This is the natural estimator for a cell → neighbourhood → district → city hierarchy with observations at mixed levels.
- **Mixed frequencies.** Quarterly observations as aggregates of a latent monthly state (Mariano and Murasawa 2003) [V].
- **Urban applications.**
  - Crowd ABMs: an ensemble Kalman filter calibrates a crowd ABM dynamically (Ward, Evans and Malleson 2016) [V]; particle filters for real-time crowds need many particles (Malleson et al. 2020 [V]; Kieu, Malleson and Heppenstall 2020 [V]).
  - Land use: a dual ensemble Kalman filter updates a CA's parameters and states (Zhang et al. 2015) [V]; a particle filter identifies a land-use CA's structure and parameters (Verstegen et al. 2014) [V].
  - Reviews of urban ABM challenges: Malleson et al. (2022) [V]; Heppenstall et al. (2021) [V].
- **Reference implementations.** DAPPER [V] (benchmarked DA methods).

#### 5.8 What This Means for the Design

The hierarchy needs:

- a **fast–slow reducer** with its attraction check;
- a **multirate stepper or co-simulation master** with a declared coupling order;
- **aggregation operators per index** and **allocation operators** under control totals;
- **reconciliation** of estimates across levels;
- a **tree filter** for assimilation with observations at mixed levels and frequencies.

The MAUP enters as the rule that parameters belong to a unit system.

### 6. Fitting and Validating Urban Dynamical Models

#### 6.1 Likelihood-Based Calibration Where a Likelihood Exists

- **Kennedy–O'Hagan** (2001) [V]: a Gaussian process emulator of the simulator plus a model-discrepancy term; parameters and discrepancy are confounded unless the discrepancy is constrained.
- **Bayesian melding** (Poole and Raftery 2000) [V]: pools prior information on inputs and outputs of a deterministic model; UrbanSim application (Ševčíková et al. 2007) [V].
- **Harris–Wilson by MCMC** (§1.4) and **Weidlich–Haag by log-linear regression** (§1.3) are urban models with tractable likelihoods; these should be preferred where they apply.

#### 6.2 Approximate Bayesian Computation, Synthetic Likelihood, Neural Simulation-Based Inference

- **ABC** (Beaumont, Zhang and Balding 2002 [V]; ABC-SMC for dynamical systems, Toni et al. 2009 [V]): accept parameters whose simulated summaries fall within a tolerance of the observed. Results depend on the summaries, the distance and the tolerance; there is no consistency without sufficient summaries.
- **Synthetic likelihood** (Wood 2010 [V]; Bayesian, Price et al. 2018 [V]): a Gaussian likelihood of summaries from simulations; good for chaotic or noisy dynamics.
- **Neural SBI** (Cranmer, Brehmer and Louppe 2020) [V]; `sbi` [V], pyABC [V], ELFI [V]. Statistical inference for stochastic simulation models in ecology: Hartig et al. (2011) [V].
- **Urban application.** MCMC-ABC calibrates a constrained CA growth model from "urban genesis" (simulating from an empty map, then from a known configuration) for Oxford and Swindon, propagating the posterior into validation measures (Yu et al. 2021) [V].

#### 6.3 History Matching with Emulators

- **Method.** Rule out parameter regions where the implausibility $I(x) = |z - \mathbb E[f(x)]| / \sqrt{\operatorname{Var}_{\text{em}} + \operatorname{Var}_{\text{disc}} + \operatorname{Var}_{\text{obs}}}$ exceeds about 3, in waves with a new emulator each wave.
- **Key papers.** Craig et al. (1997) [V]; galaxy formation (Vernon, Goldstein and Bower 2010) [V]; climate (Williamson et al. 2013) [V]; a tutorial on an HIV model (Andrianakis et al. 2015) [V]. For urban ABMs, McCulloch et al. (2022) [V] use history matching with the quantified uncertainty setting the acceptable model error (code: `Urban-Analytics/uncertainty` [V]).
- **Reference implementation.** R `hmer` (Iskauskas et al. 2024, *JSS*) [V].
- **Strength.** It does not claim a single best fit; it returns the not-ruled-out set. That matches the design's need to say what the records cannot pin down.

#### 6.4 Pattern-Oriented Modelling and Pattern Space Exploration

- **Pattern-oriented modelling** (Grimm et al. 2005) [V]: a model is accepted only if it reproduces several patterns at different scales and levels at once; each pattern filters parameters and structures. This fits a multi-level model naturally: one pattern per level (cell cluster sizes, district shares, city totals, rank–size).
- **Pattern Space Exploration** (Chérel et al. 2015) [V] searches for the patterns a model can produce, to find unexpected ones. Calibration at scale: Schmitt et al. (2015) [V]; OpenMOLE [V]. ABM output analysis: Lee et al. (2015) [V]; sensitivity method choice: ten Broeke, van Voorn and Ligtenberg (2016) [V]; a NetLogo/R cookbook: Thiele, Kurth and Grimm (2014) [V].

#### 6.5 Indirect Inference and Simulated Moments

- **Method.** Fit an auxiliary model (a VAR, a spatial regression) to data and to simulations, and choose parameters so the two auxiliary estimates agree (Gouriéroux, Monfort and Renault 1993 [V]; Smith 1993 [V]; efficient method of moments, Gallant and Tauchen 1996 [V]). Consistent if the binding function is injective.
- **For agent-based models.** Bayesian estimation (Grazzini, Richiardi and Tsionas 2017) [V]; a comparison of calibration methods on economic ABMs (Platt 2020) [V]; machine-learning surrogates (Lamperti, Roventini and Sani 2018) [V]; validation (Fagiolo et al. 2019) [V]. The infrastructure's occupancy chain was already calibrated by indirect inference.

#### 6.6 Identifiability

- **Structural identifiability**: can parameters be determined from perfect data (Bellman and Åström 1970) [V]?
- **Practical identifiability** by profile likelihood (Raue et al. 2009) [V]: a parameter whose profile-likelihood confidence interval is unbounded is not identifiable from the record.
- **Sloppiness** (Gutenkunst et al. 2007 [V]; Transtrum et al. 2015 [V]): Fisher-information eigenvalues spread over many decades; many parameters are poorly constrained, yet predictions may still be well constrained.
- **For the design.** This is the operational test for the brief's rule "refuse to estimate and make it a declared quantity". A parameter (or an index used as a parameter) whose profile interval is unbounded, or whose posterior equals its prior, becomes a declared quantity with a range. Predictions are then reported over the declared range.

#### 6.7 Out-of-Sample Judgement of Spatial Forecasts

- **Categorical maps.** Quantity and allocation disagreement replace kappa (Pontius and Millones 2011) [V]: quantity $Q = \tfrac12\sum_g |p_{g+} - p_{+g}|$, allocation $A = \sum_g \min(p_{g+} - p_{gg},\, p_{+g} - p_{gg})$, with $Q + A$ the total disagreement. Validation techniques and the figure of merit (hits over hits + misses + false alarms) are compared on many models in Pontius et al. (2004 [V]; 2008 [V]); most models were less accurate than a null of persistence at fine resolution.
- **Benchmarks and resolution.** Kappa Simulation and neutral models as benchmarks (§3.1); multiple-resolution goodness of fit (Costanza 1989) [V] and fuzzy map comparison (Hagen 2003) [V] — natural for a hierarchy, since they score each level.
- **Path dependence.** Validate the distribution of outcomes and the regions invariant across runs, not one run (Brown et al. 2005) [V]. Challenges and prediction in complex systems: Crooks, Castle and Batty (2008) [V]; Batty and Torrens (2005) [V].
- **Continuous indexes.** Strictly proper scores, CRPS and the log score (Gneiting and Raftery 2007) [V]; tests of equal predictive accuracy (Diebold and Mariano 1995) [V].
- **Spatial cross-validation.**
  - Blocked cross-validation for dependent data (Roberts et al. 2017) [V]; random cross-validation overstates the skill of large-scale maps (Ploton et al. 2020) [V]; the area of applicability (Meyer and Pebesma 2021) [V].
  - **Contested:** Wadoux et al. (2021) [V] argue that spatial cross-validation is pessimistic and not the right way to evaluate map accuracy when a probability sample allows design-based estimation. The two positions answer different questions (interpolation within the sampled region against extrapolation beyond it), and the design should declare which it answers.

#### 6.8 What This Means for the Design

- **Calibration ladder.** Use, in order of preference: an exact likelihood when one exists (Harris–Wilson, Weidlich–Haag, linear stock–flow models with Gaussian errors, already handled by the Bayesian tools); history matching to cut the space; ABC or synthetic likelihood within it; indirect inference for ABMs with an auxiliary model. Pattern-oriented multi-level acceptance applies throughout.
- **Validation suite at every level.** Quantity and allocation disagreement against a persistence null at cell level; CRPS and interval coverage for continuous indexes at district and city level; blocked hold-out in space and time, the question declared.
- **Identifiability by profile likelihood** decides which parameters become declared ranges.

### 7. Summary of Models: States, Parameters and Data

| Model | States (index; level) | Parameters (fit / declare) | Data |
|---|---|---|---|
| Urban Dynamics | 9 stocks; city | ~100 table functions (declare) | class, dwelling, business counts |
| Allen–Sanglier | population, jobs by function; centres | growth, multipliers, decay (declare/fit) | population and jobs by centre over time |
| Weidlich–Haag | population by unit; region/district | mobility, utilities (fit), κ, σ | OD flow panels, stocks |
| Harris–Wilson | sizes $W_j$; centres; flows fast | α, β, κ (fit), ε (fit from series) | $O_i$, $c_{ij}$, $W_j$; flows optional |
| Gibrat/SIMPOP/MARIUS | city populations; system | growth, interaction, innovation (fit by search) | long census series |
| Scaling | city aggregates; city | β, $Y_0$, fluctuation model (fit) | city counts with declared delineation |
| Constrained CA | land use per cell; regional totals | neighbourhood rules (fit), α | land-use maps ≥2 dates, layers, regional series |
| SLEUTH | urban per cell | 5 coefficients (fit) | ≥4 urban extents, roads |
| CLUE-S/FLUS | land use per cell; regional demand | suitability (fit), elasticities (declare) | maps, drivers, demand series |
| UrbanSim | agents, buildings, prices; parcel/zone | choice coefficients (fit), control totals | parcels, synthetic population, sales, skims |
| PECAS | prices and quantities by zone; parcels | I-O technical coefficients, elasticities | I-O table, floor space, parcels |
| Four quadrants | stock, vacancy, rent, price; submarket | elasticities, $V^*$, λ, lags, δ (fit) | stock, vacancy, rents, permits, rates |
| Business demography | establishments by age; district | entry rates, hazards (fit) | register with open/close dates |
| Tourism cycle | visitors, capacity, environment | growth, carrying capacity (fit) | arrivals, beds, surveys |
| Neighbourhood change | class shares; tract | transition probabilities (fit), tipping | tract panels |
| Labour flows | E, U, I; area | flow rates (fit) | flow data |
| Municipal debt | debt, revenue, spending; municipality | r, g, reaction ρ, adjustment (fit) | accounts series |
| Resilience | functionality $Q(t)$; system/area | drop, recovery form and time (fit/declare) | post-event performance series |
| Interdependent cascade | node states; networks | dependencies, capacities (declare) | topologies |

### 8. Recommendations

Ranked by value to the design; each says where it goes and what its checks can hold it to.

1. **Fast–slow reduction with an attraction check** (quasi-steady state after Tikhonov; time-averaging for non-equilibrating fast levels). *Where:* dynamical-systems module, systems of systems. *Checks:* Segel–Slemrod's Michaelis–Menten case and a linear two-scale system with closed-form solution; error $O(\varepsilon)$ measured against the full system.
2. **Harris–Wilson structural dynamics with Bayesian inference from sizes** (potential function, Langevin, MCMC). *Where:* urban domain, on the existing spatial-interaction and Bayesian tools. *Checks:* $D_j = \kappa W_j$ at equilibrium; uniqueness for $\alpha<1$; bifurcation in $(\alpha,\beta)$; posterior recovery on synthetic data; the published London case (Ellam et al. 2018).
3. **One library of allocation operators under control totals** (proportional, ranked with capacities, logit with balancing, IPF/minimum information, price clearing) and **aggregation operators per index kind**. *Where:* measurement module (operators) and urban domain (use). *Checks:* totals met to machine precision; IPF on Deming–Stephan's published table; Tobler's volume preservation; Theil and multigroup entropy decompositions exact.
4. **Reconciliation of estimates across the spatial hierarchy** (MinT) and **temporal disaggregation** (Denton, Chow–Lin). *Where:* statistics module. *Checks:* coherence exact; agreement with `hierarchicalforecast` and `tempdisagg` on their example data.
5. **Profile-likelihood identifiability as the gate between "fitted" and "declared"**. *Where:* statistics module, used by the studio. *Checks:* Raue et al. (2009) examples; a model with a known non-identifiable combination returns an unbounded interval.
6. **Multirate stepping and a declared co-simulation master** (MRI-GARK for split ODEs; Jacobi/Gauss–Seidel with extrapolation for parts of different forms). *Where:* dynamical-systems module. *Checks:* order of convergence on a linear two-rate problem; SUNDIALS MRIStep on its test problems; the split oscillator and the algebraic-loop instability from the co-simulation survey.
7. **Constrained CA under a regional model** (White–Engelen) as the reference multi-level land-use model. *Where:* urban domain. *Checks:* demands met exactly each year; Kappa Simulation and quantity/allocation disagreement against a persistence and a neutral model on held-out dates.
8. **Validation suite for spatial forecasts**: quantity/allocation disagreement, figure of merit, multi-resolution fit, CRPS, interval coverage, blocked hold-out with the question declared. *Where:* statistics module and studio. *Checks:* Pontius and Millones's worked tables; CRPS closed form for the Gaussian.
9. **History matching with emulators**, then ABC-SMC or synthetic likelihood inside the non-implausible set. *Where:* statistics module. *Checks:* `hmer`'s vignettes and the Andrianakis tutorial; ABC posterior equal to the conjugate posterior on a Gaussian toy; pyABC on Toni et al.'s examples.
10. **Weidlich–Haag migration** with log-linear estimation of utilities from flow panels, utilities then explained by indexes. *Where:* urban domain, on the existing master equations. *Checks:* the closed-form stationary distribution under detailed balance; mean-value equations equal to the master equation's mean for linear rates; utilities recovered from synthetic flows.
11. **Multiscale Kalman filtering on a tree**, plus ensemble Kalman with state augmentation for nonlinear levels. *Where:* statistics module (filters already present). *Checks:* the tree estimator equals the full Kalman solution on a small quadtree; mixed-frequency aggregation exact; DAPPER benchmarks.
12. **Sectoral stock–flow templates**: four quadrants with vacancy–rent error correction, business demography as hazards, labour flows, debt dynamics with a reaction function, the tourism cycle. *Where:* urban domain (templates in the dynamical-systems language). *Checks:* steady states $S^* = C^*/\delta$ and $u^* = s/(s+f)$; the debt identity exact; Wheaton's (1999) oscillation conditions; hazard fits against `lifelines` (peira already holds survival).
13. **Near-decomposability and KL-optimal aggregation** to justify or choose the levels of the hierarchy for Markov models of class or tenure. *Where:* physics-style tools (lumpability already present). *Checks:* exact lumpability condition; Simon–Ando block spectra; PCCA+ against `deeptime`; spatial Markov matrices against `giddy`.
14. **Scaling relations as city-level priors**, fitted with an explicit fluctuation model and a declared delineation, with SAMIs as indexes. *Where:* statistics module and measurement module. *Checks:* Leitão et al.'s model comparisons on their datasets; the 2007 exponents on the published US data.
15. **Bayesian melding of stochastic runs into calibrated zone-level intervals**. *Where:* statistics module, used by land-use runs. *Checks:* coverage of intervals on held-out dates (Ševčíková et al. 2007 protocol).
16. **HMM coupling of agents to balance laws**: diffusion and drift coefficients of the density on cells estimated from short agent runs. *Where:* dynamical-systems module (balance laws) with the walker/household agents. *Checks:* recovery of known coefficients from a random-walk micro model, whose macro limit is the heat equation.
17. **Resilience recovery curves** with declared functional forms and the Bruneau loss integral. *Where:* urban domain. *Checks:* closed-form integrals for linear, exponential and trigonometric recovery (Cimellaro et al. 2010).
18. **Equation-free coarse bifurcation analysis of agent-based urban models**. *Where:* dynamical-systems module. *Checks:* reproduce a known mean-field bifurcation (e.g., Schelling-type or herding models with a known mean field) from agent runs.

### 9. Bibliography

Markers as in §0. Volume and pages only where the record gave them.

**Classical urban dynamics**
- Alfeld, L.E. (1995). Urban dynamics—the first fifty years. *System Dynamics Review* 11(3):199–217. doi:10.1002/sdr.4260110303 [V]
- Allen, P.M., Sanglier, M. (1979). A dynamic model of growth in a central place system. *Geographical Analysis* 11(3):256–272. doi:10.1111/j.1538-4632.1979.tb00693.x [V]
- Allen, P.M., Sanglier, M. (1981). Urban evolution, self-organization, and decisionmaking. *Environment and Planning A* 13(2):167–183. doi:10.1068/a130167 [V]
- Barthelemy, M. (2016). *The Structure and Dynamics of Cities*. Cambridge University Press. doi:10.1017/9781316271377 [V]
- Barthelemy, M. (2019). The statistical physics of cities. *Nature Reviews Physics* 1(6):406–415. doi:10.1038/s42254-019-0054-2 [V]
- Bretagnolle, A., Pumain, D. (2010). Simulating urban networks through multiscalar space-time dynamics: Europe and the United States, 17th–20th centuries. *Urban Studies* 47(13):2819–2839. doi:10.1177/0042098010377366 [V]
- Chérel, G., Cottineau, C., Reuillon, R. (2015). Beyond corroboration: strengthening model validation by looking for unexpected patterns. *PLOS ONE* 10(9):e0138212. doi:10.1371/journal.pone.0138212 [V]
- Clarke, M., Wilson, A.G. (1983). The dynamics of urban spatial structure: progress and problems. *Journal of Regional Science* 23(1):1–18. doi:10.1111/j.1467-9787.1983.tb00780.x [V]
- Cottineau, C., Chapron, P., Reuillon, R. (2015). Growing models from the bottom up: an evaluation-based incremental modelling method (EBIMM) applied to the simulation of systems of cities. *JASSS* 18(4):9. doi:10.18564/jasss.2828 [V]
- Cottineau, C., Reuillon, R., Chapron, P., Rey-Coyrehourcq, S., Pumain, D. (2015). A modular modelling framework for hypotheses testing in the simulation of urbanisation. *Systems* 3(4):348–377. doi:10.3390/systems3040348 [V]
- Dearden, J., Wilson, A. (2011). A framework for exploring urban retail discontinuities. *Geographical Analysis* 43(2):172–187. doi:10.1111/j.1538-4632.2011.00812.x [V]
- Dearden, J., Wilson, A. (2015). *Explorations in Urban and Regional Dynamics*. Routledge. doi:10.4324/9781315779126 [V]
- Eeckhout, J. (2004). Gibrat's law for (all) cities. *American Economic Review* 94(5):1429–1451. doi:10.1257/0002828043052303 [V]
- Ellam, L., Girolami, M., Pavliotis, G.A., Wilson, A. (2018). Stochastic modelling of urban structure. *Proc. R. Soc. A* 474(2213):20170700. doi:10.1098/rspa.2017.0700 [V]
- Favaro, J.-M., Pumain, D. (2011). Gibrat revisited: an urban growth model incorporating spatial interaction and innovation cycles. *Geographical Analysis* 43(3):261–286. doi:10.1111/j.1538-4632.2011.00819.x [V]
- Forrester, J.W. (1969). *Urban Dynamics*. MIT Press. ISBN 978-0-262-06026-4 [S]
- Gabaix, X. (1999). Zipf's law for cities: an explanation. *Quarterly Journal of Economics* 114(3):739–767. doi:10.1162/003355399556133 [V]
- Gaskin, T., Pavliotis, G.A., Girolami, M. (2023). Neural parameter calibration for large-scale multiagent models. *PNAS* 120(7):e2216415120. doi:10.1073/pnas.2216415120 [V]
- Gaskin, T., Pavliotis, G.A., Girolami, M. (2024). Inferring networks from time series: a neural approach. *PNAS Nexus* 3(4):pgae063. doi:10.1093/pnasnexus/pgae063 [V]
- Gaskin, T., Conrad, T., Pavliotis, G.A., Schütte, C. (2024). Neural parameter calibration and uncertainty quantification for epidemic forecasting. *PLOS ONE* 19(10):e0306704. doi:10.1371/journal.pone.0306704 [V]
- Haag, G. (2017). *Modelling with the Master Equation*. Springer. doi:10.1007/978-3-319-60300-1 [V]
- Haag, G., Weidlich, W. (1984). A stochastic theory of interregional migration. *Geographical Analysis* 16(4):331–357. doi:10.1111/j.1538-4632.1984.tb00820.x [V]
- Haken, H. (1977). *Synergetics: An Introduction*. Springer. doi:10.1007/978-3-642-96363-6 [V]
- Haken, H., Portugali, J. (2003). The face of the city is its information. *Journal of Environmental Psychology* 23(4):385–408. doi:10.1016/S0272-4944(03)00003-3 [V]
- Haken, H., Portugali, J. (2017). Information and self-organization. *Entropy* 19(1):18. doi:10.3390/e19010018 [V]
- Haken, H., Portugali, J. (2021). *Synergetic Cities: Information, Steady State and Phase Transition*. Springer. doi:10.1007/978-3-030-63457-5 [V]
- Harris, B., Wilson, A.G. (1978). Equilibrium values and dynamics of attractiveness terms in production-constrained spatial-interaction models. *Environment and Planning A* 10(4):371–388. doi:10.1068/a100371 [V]
- Kadanoff, L.P. (1971). From simulation model to public policy: an examination of Forrester's "Urban Dynamics". *Simulation* 16(6):261–268. doi:10.1177/003754977101600603 [V]
- Krugman, P. (1991). Increasing returns and economic geography. *Journal of Political Economy* 99(3):483–499. doi:10.1086/261763 [V]
- Lee, D.B. (1973). Requiem for large-scale models. *Journal of the American Institute of Planners* 39(3):163–178. doi:10.1080/01944367308977851 [V]
- Portugali, J. (2011). *Complexity, Cognition and the City*. Springer. doi:10.1007/978-3-642-19451-1 [V]
- Pumain, D. (2018). An evolutionary theory of urban systems. In *International and Transnational Perspectives on Urban Systems*, Springer, 3–18. doi:10.1007/978-981-10-7799-9_1 [V]
- Pumain, D., Paulus, F., Vacchiani-Marcuzzo, C., Lobo, J. (2006; Crossref record 2011). An evolutionary theory for interpreting urban scaling laws. *Cybergeo*. doi:10.4000/cybergeo.2519 [V]
- Pumain, D., Reuillon, R. (2017). *Urban Dynamics and Simulation Models*. Springer. doi:10.1007/978-3-319-46497-8 [V]
- Reuillon, R., Leclaire, M., Rey-Coyrehourcq, S. (2013). OpenMOLE, a workflow engine specifically tailored for the distributed exploration of simulation models. *Future Generation Computer Systems* 29(8):1981–1990. doi:10.1016/j.future.2013.05.003 [V]
- Rozenfeld, H.D., Rybski, D., Gabaix, X., Makse, H.A. (2011). The area and population of cities: new insights from a different perspective on cities. *American Economic Review* 101(5):2205–2225. doi:10.1257/aer.101.5.2205 [V]
- Sanders, L., Pumain, D., Mathian, H., Guérin-Pace, F., Bura, S. (1997). SIMPOP: a multiagent system for the study of urbanism. *Environment and Planning B* 24(2):287–305. doi:10.1068/b240287 [V]
- Schmitt, C., Rey-Coyrehourcq, S., Reuillon, R., Pumain, D. (2015). Half a billion simulations: evolutionary algorithms and distributed computing for calibrating the SimpopLocal geographical model. *Environment and Planning B* 42(2):300–315. doi:10.1068/b130064p [V]
- Verbavatz, V., Barthelemy, M. (2020). The growth equation of cities. *Nature* 587(7834):397–401. doi:10.1038/s41586-020-2900-x [V]
- Weidlich, W. (1991). Physics and social science—the approach of synergetics. *Physics Reports* 204(1):1–163. doi:10.1016/0370-1573(91)90024-G [V]
- Weidlich, W. (2000). *Sociodynamics: A Systematic Approach to Mathematical Modelling in the Social Sciences*. Harwood (Taylor & Francis). Reviewed at doi:10.1155/S1026022600000601 [S]
- Weidlich, W., Haag, G. (1987). A dynamic phase transition model for spatial agglomeration processes. *Journal of Regional Science* 27(4):529–569. doi:10.1111/j.1467-9787.1987.tb01181.x [V]
- Weidlich, W., Haag, G. (eds.) (1988). *Interregional Migration: Dynamic Theory and Comparative Analysis*. Springer. doi:10.1007/978-3-642-73049-8 [V]
- Wilson, A.G. (1981). *Catastrophe Theory and Bifurcation: Applications to Urban and Regional Systems*. Croom Helm. [M]
- Wilson, A. (2008). Boltzmann, Lotka and Volterra and spatial structural evolution: an integrated methodology for some dynamical systems. *J. R. Soc. Interface* 5(25):865–871. doi:10.1098/rsif.2007.1288 [V]
- Wilson, A. (2010). Entropy in urban and regional modelling: retrospect and prospect. *Geographical Analysis* 42(4):364–394. doi:10.1111/j.1538-4632.2010.00799.x [V]
- Zachos, I., Damoulas, T., Girolami, M. (2024). Table inference for combinatorial origin-destination choices in agent-based population synthesis. *Stat* 13(1):e656. doi:10.1002/sta4.656 [V]
- Zachos, I., Girolami, M., Damoulas, T. (2024). Generating origin-destination matrices in neural spatial interaction models. *Advances in Neural Information Processing Systems* 37:110436–110463. doi:10.52202/079017-3506 [V]

**Urban scaling**
- Altmann, E.G. (2020). Spatial interactions in urban scaling laws. *PLOS ONE* 15(12):e0243390. doi:10.1371/journal.pone.0243390 [V]
- Arcaute, E., Hatna, E., Ferguson, P., Youn, H., Johansson, A., Batty, M. (2015). Constructing cities, deconstructing scaling laws. *J. R. Soc. Interface* 12(102):20140745. doi:10.1098/rsif.2014.0745 [V]
- Arcaute, E., Molinero, C., Hatna, E., Murcio, R., Vargas-Ruiz, C., Masucci, A.P., et al. (2016). Cities and regions in Britain through hierarchical percolation. *R. Soc. Open Sci.* 3(4):150691. doi:10.1098/rsos.150691 [V]
- Batty, M. (2008). The size, scale, and shape of cities. *Science* 319(5864):769–771. doi:10.1126/science.1151419 [V]
- Bettencourt, L.M.A. (2013). The origins of scaling in cities. *Science* 340(6139):1438–1441. doi:10.1126/science.1235823 [V]
- Bettencourt, L.M.A. (2020). Urban growth and the emergent statistics of cities. *Science Advances* 6(34):eaat8812. doi:10.1126/sciadv.aat8812 [V]
- Bettencourt, L.M.A., Lobo, J. (2016). Urban scaling in Europe. *J. R. Soc. Interface* 13(116):20160005. doi:10.1098/rsif.2016.0005 [V]
- Bettencourt, L.M.A., Lobo, J., Helbing, D., Kühnert, C., West, G.B. (2007). Growth, innovation, scaling, and the pace of life in cities. *PNAS* 104(17):7301–7306. doi:10.1073/pnas.0610172104 [V]
- Bettencourt, L.M.A., Lobo, J., Strumsky, D., West, G.B. (2010). Urban scaling and its deviations. *PLoS ONE* 5(11):e13541. doi:10.1371/journal.pone.0013541 [V]
- Bettencourt, L.M.A., Yang, V.C., Lobo, J., Kempes, C.P., Rybski, D., Hamilton, M.J. (2020). The interpretation of urban scaling analysis in time. *J. R. Soc. Interface* 17(163):20190846. doi:10.1098/rsif.2019.0846 [V]
- Cottineau, C., Hatna, E., Arcaute, E., Batty, M. (2017). Diverse cities or the systematic paradox of urban scaling laws. *Computers, Environment and Urban Systems* 63:80–94. doi:10.1016/j.compenvurbsys.2016.04.006 [V]
- Depersin, J., Barthelemy, M. (2018). From global scaling to the dynamics of individual cities. *PNAS* 115(10):2317–2322. doi:10.1073/pnas.1718690115 [V]
- Dijkstra, L., Florczyk, A.J., Freire, S., Kemper, T., Melchiorri, M., Pesaresi, M., et al. (2021). Applying the Degree of Urbanisation to the globe. *Journal of Urban Economics* 125:103312. doi:10.1016/j.jue.2020.103312 [V]
- Finance, O., Cottineau, C. (2019). Are the absent always wrong? Dealing with zero values in urban scaling. *Environment and Planning B* 46(9):1663–1677. doi:10.1177/2399808318785634 [V]
- Gomez-Lievano, A., Patterson-Lomba, O., Hausmann, R. (2016). Explaining the prevalence, scaling and variance of urban phenomena. *Nature Human Behaviour* 1(1):0012. doi:10.1038/s41562-016-0012 [V]
- Gomez-Lievano, A., Youn, H., Bettencourt, L.M.A. (2012). The statistics of urban scaling and their connection to Zipf's law. *PLoS ONE* 7(7):e40393. doi:10.1371/journal.pone.0040393 [V]
- Keuschnigg, M., Mutgan, S., Hedström, P. (2019). Urban scaling and the regional divide. *Science Advances* 5(1):eaav0042. doi:10.1126/sciadv.aav0042 [V]
- Leitão, J.C., Miotto, J.M., Gerlach, M., Altmann, E.G. (2016). Is this scaling nonlinear? *R. Soc. Open Sci.* 3(7):150649. doi:10.1098/rsos.150649 [V]
- Lobo, J., Bettencourt, L.M.A., Strumsky, D., West, G.B. (2013). Urban scaling and the production function for cities. *PLoS ONE* 8(3):e58407. doi:10.1371/journal.pone.0058407 [V]
- Louf, R., Barthelemy, M. (2014). Scaling: lost in the smog. *Environment and Planning B* 41(5):767–769. doi:10.1068/b4105c [V]
- Ribeiro, F.L., Meirelles, J., Ferreira, F.F., Neto, C.R. (2017). A model of urban scaling laws based on distance dependent interactions. *R. Soc. Open Sci.* 4(3):160926. doi:10.1098/rsos.160926 [V]
- Ribeiro, F.L., Rybski, D. (2023). Mathematical models to explain the origin of urban scaling laws. *Physics Reports* 1012:1–39. doi:10.1016/j.physrep.2023.02.002 [V]
- Shalizi, C.R. (2011). Scaling and hierarchy in urban economies. arXiv:1102.4101 [V]
- Strano, E., Sood, V. (2016). Rich and poor cities in Europe: an urban scaling approach to mapping the European economic transition. *PLOS ONE* 11(8):e0159465. doi:10.1371/journal.pone.0159465 [V]

**Multi-level land use and LUTI**
- Acheampong, R.A., Silva, E.A. (2015). Land use–transport interaction modeling: a review of the literature and future research directions. *Journal of Transport and Land Use*. doi:10.5198/jtlu.2015.806 [V]
- Batty, M., Xie, Y. (1994). From cells to cities. *Environment and Planning B* 21(7):S31–S48. doi:10.1068/b21S031 [V]
- Clarke, K.C., Gaydos, L.J. (1998). Loose-coupling a cellular automaton model and GIS: long-term urban growth prediction for San Francisco and Washington/Baltimore. *IJGIS* 12(7):699–714. doi:10.1080/136588198241617 [V]
- Clarke, K.C., Hoppen, S., Gaydos, L. (1997). A self-modifying cellular automaton model of historical urbanization in the San Francisco Bay area. *Environment and Planning B* 24(2):247–261. doi:10.1068/b240247 [V]
- Dietzel, C., Clarke, K.C. (2007). Toward optimal calibration of the SLEUTH land use change model. *Transactions in GIS* 11(1):29–45. doi:10.1111/j.1467-9671.2007.01031.x [V]
- Echenique, M.H., Flowerdew, A.D.J., Hunt, J.D., Mayo, T.R., Skidmore, I.J., Simmonds, D.C. (1990). The MEPLAN models of Bilbao, Leeds and Dortmund. *Transport Reviews* 10(4):309–322. doi:10.1080/01441649008716764 [V]
- Engelen, G., White, R., de Nijs, T. (2003). Environment Explorer: spatial support system for the integrated assessment of socio-economic and environmental policies in the Netherlands. *Integrated Assessment* 4(2):97–105. doi:10.1076/iaij.4.2.97.16707 [V]
- Horni, A., Nagel, K., Axhausen, K.W. (eds.) (2016). *The Multi-Agent Transport Simulation MATSim*. Ubiquity Press. doi:10.5334/baw [V]
- Hunt, J.D., Abraham, J.E. (2005). Design and implementation of PECAS. In *Integrated Land-Use and Transportation Models*, 253–273. doi:10.1108/9781786359520-011 [V]
- Jantz, C.A., Goetz, S.J., Shelley, M.K. (2004). Using the SLEUTH urban growth model to simulate the impacts of future policy scenarios. *Environment and Planning B* 31(2):251–271. doi:10.1068/b2983 [V]
- Jantz, C.A., Goetz, S.J., Donato, D., Claggett, P. (2010). Designing and implementing a regional urban modeling system using the SLEUTH cellular urban model. *CEUS* 34(1):1–16. doi:10.1016/j.compenvurbsys.2009.08.003 [V]
- Li, X., Yeh, A.G.-O. (2002). Neural-network-based cellular automata for simulating multiple land use changes using GIS. *IJGIS* 16(4):323–343. doi:10.1080/13658810210137004 [V]
- Liu, X., Liang, X., Li, X., Xu, X., Ou, J., Chen, Y., et al. (2017). A future land use simulation model (FLUS). *Landscape and Urban Planning* 168:94–116. doi:10.1016/j.landurbplan.2017.09.019 [V]
- Miller, E.J., Hunt, J.D., Abraham, J.E., Salvini, P.A. (2004). Microsimulating urban systems. *CEUS* 28(1–2):9–44. doi:10.1016/S0198-9715(02)00044-3 [V]
- Moeckel, R., Llorca Garcia, C., Moreno Chou, A.T., Okrah, M.B. (2018). Trends in integrated land use/transport modeling. *Journal of Transport and Land Use* 11(1). doi:10.5198/jtlu.2018.1205 [V]
- Newland, C.P., Maier, H.R., Zecchin, A.C., Newman, J.P., van Delden, H. (2018). Multi-objective optimisation framework for calibration of cellular automata land-use models. *Environmental Modelling & Software* 100:175–200. doi:10.1016/j.envsoft.2017.11.012 [V]
- Salvini, P., Miller, E.J. (2005). ILUTE: an operational prototype of a comprehensive microsimulation model of urban systems. *Networks and Spatial Economics* 5(2):217–234. doi:10.1007/s11067-005-2630-5 [V]
- Santé, I., García, A.M., Miranda, D., Crecente, R. (2010). Cellular automata models for the simulation of real-world urban processes. *Landscape and Urban Planning* 96(2):108–122. doi:10.1016/j.landurbplan.2010.03.001 [V]
- Ševčíková, H., Raftery, A.E., Waddell, P.A. (2007). Assessing uncertainty in urban simulations using Bayesian melding. *Transportation Research B* 41(6):652–669. doi:10.1016/j.trb.2006.11.001 [V]
- Verburg, P.H., Soepboer, W., Veldkamp, A., Limpiada, R., Espaldon, V., Mastura, S.S.A. (2002). Modeling the spatial dynamics of regional land use: the CLUE-S model. *Environmental Management* 30(3):391–405. doi:10.1007/s00267-002-2630-x [V]
- Waddell, P. (2002). UrbanSim: modeling urban development for land use, transportation, and environmental planning. *JAPA* 68(3):297–314. doi:10.1080/01944360208976274 [V]
- Waddell, P. (2011). Integrated land use and transportation planning and modelling. *Transport Reviews* 31(2):209–229. doi:10.1080/01441647.2010.525671 [V]
- Waddell, P., Borning, A., Noth, M., Freier, N., Becke, M., Ulfarsson, G. (2003). Microsimulation of urban development and location choices: design and implementation of UrbanSim. *Networks and Spatial Economics* 3(1):43–67. doi:10.1023/A:1022049000877 [V]
- Wegener, M. (1994). Operational urban models: state of the art. *JAPA* 60(1):17–29. doi:10.1080/01944369408975547 [V]
- Wegener, M. (2004). Overview of land use transport models. In *Handbook of Transport Geography and Spatial Systems*, 127–146. doi:10.1108/9781615832538-009 [V]
- Wegener, M. (2021). Land-use transport interaction models. In *Handbook of Regional Science*, 229–246. doi:10.1007/978-3-662-60723-7_41 [V]
- White, R., Engelen, G. (1993). Cellular automata and fractal urban form. *Environment and Planning A* 25(8):1175–1199. doi:10.1068/a251175 [V]
- White, R., Engelen, G. (2000). High-resolution integrated modelling of the spatial dynamics of urban and regional systems. *CEUS* 24(5):383–400. doi:10.1016/S0198-9715(00)00012-0 [V]
- White, R., Engelen, G., Uljee, I. (1997). The use of constrained cellular automata for high-resolution modelling of urban land-use dynamics. *Environment and Planning B* 24(3):323–343. doi:10.1068/b240323 [V]
- White, R., Engelen, G., Uljee, I. (2015). *Modeling Cities and Regions as Complex Systems*. MIT Press. doi:10.7551/mitpress/9780262029568.001.0001 [V]

**Sectoral stock–flow dynamics**
- Audretsch, D.B., Mahmood, T. (1995). New firm survival: new results using a hazard function. *Review of Economics and Statistics* 77(1):97. doi:10.2307/2109995 [V]
- Blanchard, O.J., Katz, L.F. (1992). Regional evolutions. *Brookings Papers on Economic Activity* 1992(1):1. doi:10.2307/2534556 [V]
- Bohn, H. (1998). The behavior of U.S. public debt and deficits. *QJE* 113(3):949–963. doi:10.1162/003355398555793 [V]
- Bruneau, M., Chang, S.E., Eguchi, R.T., Lee, G.C., O'Rourke, T.D., Reinhorn, A.M., et al. (2003). A framework to quantitatively assess and enhance the seismic resilience of communities. *Earthquake Spectra* 19(4):733–752. doi:10.1193/1.1623497 [V]
- Buettner, T., Wildasin, D.E. (2006). The dynamics of municipal fiscal adjustment. *Journal of Public Economics* 90(6–7):1115–1132. doi:10.1016/j.jpubeco.2005.09.002 [V]
- Buldyrev, S.V., Parshani, R., Paul, G., Stanley, H.E., Havlin, S. (2010). Catastrophic cascade of failures in interdependent networks. *Nature* 464(7291):1025–1028. doi:10.1038/nature08932 [V]
- Butler, R.W. (1980). The concept of a tourist area cycle of evolution. *Canadian Geographer* 24(1):5–12. doi:10.1111/j.1541-0064.1980.tb00970.x [V]
- Cao, Y., Miles, S.B., Choe, Y. (2022). Infrastructure recovery curve estimation using Gaussian process regression on expert elicited data. *Reliability Engineering & System Safety* 217:108054. doi:10.1016/j.ress.2021.108054 [V]
- Card, D., Mas, A., Rothstein, J. (2008). Tipping and the dynamics of segregation. *QJE* 123(1):177–218. doi:10.1162/qjec.2008.123.1.177 [V]
- Casagrandi, R., Rinaldi, S. (2002). A theoretical approach to tourism sustainability. *Conservation Ecology* 6(1):13. doi:10.5751/ES-00384-060113 [V]
- Chang, S.E., Shinozuka, M. (2004). Measuring improvements in the disaster resilience of communities. *Earthquake Spectra* 20(3):739–755. doi:10.1193/1.1775796 [V]
- Cimellaro, G.P., Reinhorn, A.M., Bruneau, M. (2010). Framework for analytical quantification of disaster resilience. *Engineering Structures* 32(11):3639–3649. doi:10.1016/j.engstruct.2010.08.008 [V]
- Delmelle, E.C. (2016). Mapping the DNA of urban neighborhoods. *Annals of the AAG* 106(1):36–56. doi:10.1080/00045608.2015.1096188 [V]
- Diappi, L., Bolchi, P. (2008). Smith's rent gap theory and local real estate dynamics: a multi-agent model. *CEUS* 32(1):6–18. doi:10.1016/j.compenvurbsys.2006.11.003 [V]
- DiPasquale, D., Wheaton, W.C. (1992). The markets for real estate assets and space: a conceptual framework. *Real Estate Economics* 20(2):181–198. doi:10.1111/1540-6229.00579 [V]
- DiPasquale, D., Wheaton, W.C. (1994). Housing market dynamics and the future of housing prices. *Journal of Urban Economics* 35(1):1–27. doi:10.1006/juec.1994.1001 [V]
- Dobson, I., Carreras, B.A., Lynch, V.E., Newman, D.E. (2007). Complex systems analysis of series of blackouts. *Chaos* 17(2):026103. doi:10.1063/1.2737822 [V]
- Doxey, G.V. (1975). A causation theory of visitor–resident irritants: methodology and research inferences. Travel Research Association conference proceedings. [M]
- Dunne, T., Roberts, M.J., Samuelson, L. (1988). Patterns of firm entry and exit in U.S. manufacturing industries. *RAND Journal of Economics* 19(4):495. doi:10.2307/2555454 [V]
- Ellen, I.G., O'Regan, K.M. (2011). How low income neighborhoods change: entry, exit, and enhancement. *Regional Science and Urban Economics* 41(2):89–97. doi:10.1016/j.regsciurbeco.2010.12.005 [V]
- Escolano, J. (2010). A practical guide to public debt dynamics, fiscal sustainability, and cyclical adjustment of budgetary aggregates. IMF Technical Notes and Manuals 2010/002. doi:10.5089/9781462396955.005 [V]
- Eurostat, OECD (2008). *Eurostat–OECD Manual on Business Demography Statistics*. doi:10.1787/9789264041882-en [V]
- Gabriel, S.A., Nothaft, F.E. (1988). Rental housing markets and the natural vacancy rate. *Real Estate Economics* 16(4):419–429. doi:10.1111/1540-6229.00465 [V]
- Gabriel, S.A., Nothaft, F.E. (2001). Rental housing markets, the incidence and duration of vacancy, and the natural vacancy rate. *Journal of Urban Economics* 49(1):121–149. doi:10.1006/juec.2000.2187 [V]
- Gao, J., Buldyrev, S.V., Stanley, H.E., Havlin, S. (2012). Networks formed from interdependent networks. *Nature Physics* 8(1):40–48. doi:10.1038/nphys2180 [V]
- Geroski, P.A. (1995). What do we know about entry? *International Journal of Industrial Organization* 13(4):421–440. doi:10.1016/0167-7187(95)00498-X [V]
- Glaeser, E.L., Gyourko, J. (2005). Urban decline and durable housing. *Journal of Political Economy* 113(2):345–375. doi:10.1086/427465 [V]
- Glaeser, E.L., Gyourko, J., Morales, E., Nathanson, C.G. (2014). Housing dynamics: an urban approach. *Journal of Urban Economics* 81:45–56. doi:10.1016/j.jue.2014.02.003 [V]
- Guerrieri, V., Hartley, D., Hurst, E. (2013). Endogenous gentrification and housing price dynamics. *Journal of Public Economics* 100:45–60. doi:10.1016/j.jpubeco.2013.02.001 [V]
- Hannan, M.T., Freeman, J. (1977). The population ecology of organizations. *American Journal of Sociology* 82(5):929–964. doi:10.1086/226424 [V]
- Hendershott, P.H. (1996). Rental adjustment and valuation in overbuilt markets: evidence from the Sydney office market. *Journal of Urban Economics* 39(1):51–67. doi:10.1006/juec.1996.0003 [V]
- Hendershott, P.H., MacGregor, B.D., White, M. (2002). Explaining real commercial rents using an error correction model with panel data. *Journal of Real Estate Finance and Economics* 24(1–2):59–87. doi:10.1023/A:1013930304732 [V]
- Hendrick, R. (2004). Assessing and measuring the fiscal heath of local governments. *Urban Affairs Review* 40(1):78–114. doi:10.1177/1078087404268076 [V]
- Holtz-Eakin, D., Newey, W., Rosen, H.S. (1989). The revenues-expenditures nexus: evidence from local government data. *International Economic Review* 30(2):415. doi:10.2307/2526655 [V]
- Hosseini, S., Barker, K., Ramirez-Marquez, J.E. (2016). A review of definitions and measures of system resilience. *Reliability Engineering & System Safety* 145:47–61. doi:10.1016/j.ress.2015.08.006 [V]
- Hsu, C.-W., Mostafavi, A. (2025). Dissecting resilience curve archetypes and properties in human systems facing weather hazards. *Scientific Reports* 15(1):11897. doi:10.1038/s41598-025-95909-8 [V]
- Lundtorp, S., Wanhill, S. (2001). The resort lifecycle theory: generating processes and estimation. *Annals of Tourism Research* 28(4):947–964. doi:10.1016/S0160-7383(00)00080-3 [V]
- Madanat, S., Mishalani, R., Ibrahim, W.H.W. (1995). Estimation of infrastructure transition probabilities from condition rating data. *Journal of Infrastructure Systems* 1(2):120–125. doi:10.1061/(ASCE)1076-0342(1995)1:2(120) [V]
- Mauro, G., Pedreschi, N., Lambiotte, R., Pappalardo, L. (2024). Dynamic models of gentrification. arXiv:2410.18004 [V]
- Mayer, C.J., Somerville, C.T. (2000). Residential construction: using the urban growth model to estimate housing supply. *Journal of Urban Economics* 48(1):85–109. doi:10.1006/juec.1999.2158 [V]
- Mishalani, R.G., Madanat, S.M. (2002). Computation of infrastructure transition probabilities using stochastic duration models. *Journal of Infrastructure Systems* 8(4):139–148. doi:10.1061/(ASCE)1076-0342(2002)8:4(139) [V]
- Mortensen, D.T., Pissarides, C.A. (1994). Job creation and job destruction in the theory of unemployment. *Review of Economic Studies* 61(3):397–415. doi:10.2307/2297896 [V]
- Motter, A.E., Lai, Y.-C. (2002). Cascade-based attacks on complex networks. *Physical Review E* 66(6):065102. doi:10.1103/PhysRevE.66.065102 [V]
- O'Reilly, A.M. (1986). Tourism carrying capacity: concept and issues. *Tourism Management* 7(4):254–258. doi:10.1016/0261-5177(86)90035-X [V]
- Ouyang, M. (2014). Review on modeling and simulation of interdependent critical infrastructure systems. *Reliability Engineering & System Safety* 121:43–60. doi:10.1016/j.ress.2013.06.040 [V]
- Ouyang, M., Dueñas-Osorio, L., Min, X. (2012). A three-stage resilience analysis framework for urban infrastructure systems. *Structural Safety* 36–37:23–31. doi:10.1016/j.strusafe.2011.12.004 [V]
- Poterba, J.M. (1984). Tax subsidies to owner-occupied housing: an asset-market approach. *QJE* 99(4):729. doi:10.2307/1883123 [V]
- Rey, S.J. (2001). Spatial empirics for economic growth and convergence. *Geographical Analysis* 33(3):195–214. doi:10.1111/j.1538-4632.2001.tb00444.x [V]
- Rinaldi, S.M., Peerenboom, J.P., Kelly, T.K. (2001). Identifying, understanding, and analyzing critical infrastructure interdependencies. *IEEE Control Systems* 21(6):11–25. doi:10.1109/37.969131 [V] (authors from memory; the record lists none)
- Rosen, K.T., Smith, L.B. (1983). The price-adjustment process for rental housing and the natural vacancy rate. *American Economic Review* 73(4):779–786. RePEc:aea:aecrev:v:73:y:1983:i:4:p:779-86 [S]
- Saiz, A. (2010). The geographic determinants of housing supply. *QJE* 125(3):1253–1296. doi:10.1162/qjec.2010.125.3.1253 [V]
- Schelling, T.C. (1971). Dynamic models of segregation. *Journal of Mathematical Sociology* 1(2):143–186. doi:10.1080/0022250X.1971.9989794 [V]
- Shaw, J.D., Restrepo, J.G., Rodríguez, N. (2024). A dynamical system model of gentrification: exploring a simple rent control strategy. arXiv:2406.12092 [V]
- Shimer, R. (2012). Reassessing the ins and outs of unemployment. *Review of Economic Dynamics* 15(2):127–148. doi:10.1016/j.red.2012.02.001 [V]
- Smith, N. (1979). Toward a theory of gentrification: a back to the city movement by capital, not people. *JAPA* 45(4):538–548. doi:10.1080/01944367908977002 [V]
- Wheaton, W.C. (1987). The cyclic behavior of the national office market. *Real Estate Economics* 15(4):281–299. doi:10.1111/1540-6229.00433 [V]
- Wheaton, W.C. (1999). Real estate "cycles": some fundamentals. *Real Estate Economics* 27(2):209–230. doi:10.1111/1540-6229.00772 [V]
- Wheaton, W.C., Torto, R.G. (1988). Vacancy rates and the future of office rents. *Real Estate Economics* 16(4):430–436. doi:10.1111/1540-6229.00466 [V]
- Wheaton, W.C., Torto, R.G., Evans, P. (1997). The cyclic behavior of the Greater London office market. *Journal of Real Estate Finance and Economics* 15(1):77–92. doi:10.1023/A:1007701422238 [V]
- Zobel, C.W. (2011). Representing perceived tradeoffs in defining disaster resilience. *Decision Support Systems* 50(2):394–403. doi:10.1016/j.dss.2010.10.001 [V]

**Multi-scale methods**
- Arnold, M., Günther, M. (2001). Preconditioned dynamic iteration for coupled differential-algebraic systems. *BIT* 41(1):1–25. doi:10.1023/A:1021909032551 [V]
- Auger, P., Bravo de la Parra, R., Poggiale, J.-C., Sánchez, E., Sanz, L. (2008). Aggregation methods in dynamical systems and applications in population and community dynamics. *Physics of Life Reviews* 5(2):79–105. doi:10.1016/j.plrev.2008.02.001 [V]
- Banisch, S. (2016). *Markov Chain Aggregation for Agent-Based Models*. Springer. doi:10.1007/978-3-319-24877-6 [V]
- Banisch, S., Lima, R., Araújo, T. (2012). Agent based models and opinion dynamics as Markov chains. *Social Networks* 34(4):549–561. doi:10.1016/j.socnet.2012.06.001 [V]
- Blochwitz, T., Otter, M., Arnold, M., Bausch, C., Clauss, C., Elmqvist, H., et al. (2011). The Functional Mockup Interface for tool independent exchange of simulation models. *Proc. 8th International Modelica Conference*, 105–114. doi:10.3384/ecp11063105 [V]
- Broman, D., Brooks, C., Greenberg, L., Lee, E.A., Masin, M., Tripakis, S., et al. (2013). Determinate composition of FMUs for co-simulation. *EMSOFT 2013*, 1–12. doi:10.1109/EMSOFT.2013.6658580 [V]
- Buchholz, P. (1994). Exact and ordinary lumpability in finite Markov chains. *Journal of Applied Probability* 31(1):59–75. doi:10.2307/3215235 [V]
- Chou, K.C., Willsky, A.S., Benveniste, A. (1994). Multiscale recursive estimation, data fusion, and regularization. *IEEE Transactions on Automatic Control* 39(3):464–478. doi:10.1109/9.280746 [V]
- Chow, G.C., Lin, A. (1971). Best linear unbiased interpolation, distribution, and extrapolation of time series by related series. *Review of Economics and Statistics* 53(4):372. doi:10.2307/1928739 [V]
- Deming, W.E., Stephan, F.F. (1940). On a least squares adjustment of a sampled frequency table when the expected marginal totals are known. *Annals of Mathematical Statistics* 11(4):427–444. doi:10.1214/aoms/1177731829 [V]
- Deng, K., Mehta, P.G., Meyn, S.P. (2011). Optimal Kullback–Leibler aggregation via spectral theory of Markov chains. *IEEE TAC* 56(12):2793–2808. doi:10.1109/TAC.2011.2141350 [V]
- Denton, F.T. (1971). Adjustment of monthly or quarterly series to annual totals. *JASA* 66(333):99–102. doi:10.1080/01621459.1971.10482227 [V]
- Deuflhard, P., Weber, M. (2005). Robust Perron cluster analysis in conformation dynamics. *Linear Algebra and its Applications* 398:161–184. doi:10.1016/j.laa.2004.10.026 [V]
- E, W., Engquist, B. (2003). The heterogeneous multiscale methods. *Communications in Mathematical Sciences* 1(1):87–132. doi:10.4310/CMS.2003.v1.n1.a8 [V]
- Evensen, G. (1994). Sequential data assimilation with a nonlinear quasi-geostrophic model using Monte Carlo methods to forecast error statistics. *JGR Oceans* 99(C5):10143–10162. doi:10.1029/94JC00572 [V]
- Fenichel, N. (1979). Geometric singular perturbation theory for ordinary differential equations. *Journal of Differential Equations* 31(1):53–98. doi:10.1016/0022-0396(79)90152-9 [V]
- Fotheringham, A.S., Wong, D.W.S. (1991). The modifiable areal unit problem in multivariate statistical analysis. *Environment and Planning A* 23(7):1025–1044. doi:10.1068/a231025 [V]
- Gear, C.W., Wells, D.R. (1984). Multirate linear multistep methods. *BIT* 24(4):484–502. doi:10.1007/BF01934907 [V]
- Geiger, B.C., Petrov, T., Kubin, G., Koeppl, H. (2015). Optimal Kullback–Leibler aggregation via information bottleneck. *IEEE TAC* 60(4):1010–1022. doi:10.1109/TAC.2014.2364971 [V]
- Gomes, C., Thule, C., Broman, D., Larsen, P.G., Vangheluwe, H. (2018; issue 2019). Co-simulation: a survey. *ACM Computing Surveys* 51(3):1–33. doi:10.1145/3179993 [V]
- Gordon, N.J., Salmond, D.J., Smith, A.F.M. (1993). Novel approach to nonlinear/non-Gaussian Bayesian state estimation. *IEE Proceedings F* 140(2):107. doi:10.1049/ip-f-2.1993.0015 [V]
- Gotway, C.A., Young, L.J. (2002). Combining incompatible spatial data. *JASA* 97(458):632–648. doi:10.1198/016214502760047140 [V]
- Günther, M., Kværnø, A., Rentrop, P. (2001). Multirate partitioned Runge–Kutta methods. *BIT* 41(3):504–514. doi:10.1023/A:1021967112503 [V]
- Houtekamer, P.L., Zhang, F. (2016). Review of the ensemble Kalman filter for atmospheric data assimilation. *Monthly Weather Review* 144(12):4489–4532. doi:10.1175/MWR-D-15-0440.1 [V]
- Hyndman, R.J., Ahmed, R.A., Athanasopoulos, G., Shang, H.L. (2011). Optimal combination forecasts for hierarchical time series. *CSDA* 55(9):2579–2589. doi:10.1016/j.csda.2011.03.006 [V]
- Iwasa, Y., Andreasen, V., Levin, S. (1987). Aggregation in model ecosystems. I. Perfect aggregation. *Ecological Modelling* 37(3–4):287–302. doi:10.1016/0304-3800(87)90030-5 [V]
- Kemeny, J.G., Snell, J.L. (1960). *Finite Markov Chains*. Van Nostrand. [M]
- Kevrekidis, I.G., Gear, C.W., Hyman, J.M., Kevrekidis, P.G., Runborg, O., Theodoropoulos, C. (2003). Equation-free, coarse-grained multiscale computation. *Communications in Mathematical Sciences* 1(4):715–762. doi:10.4310/CMS.2003.v1.n4.a5 [V]
- Kevrekidis, I.G., Samaey, G. (2009). Equation-free multiscale computation: algorithms and applications. *Annual Review of Physical Chemistry* 60:321–344. doi:10.1146/annurev.physchem.59.032607.093610 [V]
- Kübler, R., Schiehlen, W. (2000). Two methods of simulator coupling. *Mathematical and Computer Modelling of Dynamical Systems* 6(2):93–113. doi:10.1076/1387-3954(200006)6:2;1-M;FT093 [V]
- Kuehn, C. (2015). *Multiple Time Scale Dynamics*. Springer. doi:10.1007/978-3-319-12316-5 [V]
- Mariano, R.S., Murasawa, Y. (2003). A new coincident index of business cycles based on monthly and quarterly series. *Journal of Applied Econometrics* 18(4):427–443. doi:10.1002/jae.695 [V]
- Mathieu, P., Morvan, G., Picault, S. (2018). Multi-level agent-based simulations: four design patterns. *Simulation Modelling Practice and Theory* 83:51–64. doi:10.1016/j.simpat.2017.12.015 [V]
- Morvan, G. (2012). Multi-level agent-based modeling: a literature survey. arXiv:1205.0561 [V]
- Openshaw, S. (1977). Optimal zoning systems for spatial interaction models. *Environment and Planning A* 9(2):169–184. doi:10.1068/a090169 [V]
- Openshaw, S. (1984). *The Modifiable Areal Unit Problem*. CATMOG 38. Geo Books. [M]
- Pavliotis, G.A., Stuart, A.M. (2008). *Multiscale Methods: Averaging and Homogenization*. Springer. doi:10.1007/978-0-387-73829-1 [V]
- Reardon, S.F., Firebaugh, G. (2002). Measures of multigroup segregation. *Sociological Methodology* 32(1):33–67. doi:10.1111/1467-9531.00110 [V]
- Sandu, A. (2019). A class of multirate infinitesimal GARK methods. *SIAM Journal on Numerical Analysis* 57(5):2300–2327. doi:10.1137/18M1205492 [V]
- Sax, C., Steiner, P. (2013). Temporal disaggregation of time series. *The R Journal* 5(2):80. doi:10.32614/RJ-2013-028 [V]
- Segel, L.A., Slemrod, M. (1989). The quasi-steady-state assumption: a case study in perturbation. *SIAM Review* 31(3):446–477. doi:10.1137/1031091 [V]
- Shorrocks, A.F. (1980). The class of additively decomposable inequality measures. *Econometrica* 48(3):613. doi:10.2307/1913126 [V]
- Siettos, C.I., Gear, C.W., Kevrekidis, I.G. (2012). An equation-free approach to agent-based computation: bifurcation analysis and control of stationary states. *EPL* 99(4):48007. doi:10.1209/0295-5075/99/48007 [V]
- Simon, H.A., Ando, A. (1961). Aggregation of variables in dynamic systems. *Econometrica* 29(2):111. doi:10.2307/1909285 [V]
- Snyder, C., Bengtsson, T., Bickel, P., Anderson, J. (2008). Obstacles to high-dimensional particle filtering. *Monthly Weather Review* 136(12):4629–4640. doi:10.1175/2008MWR2529.1 [V]
- Tikhonov, A.N. (1952). Systems of differential equations containing small parameters in the derivatives. *Matematicheskii Sbornik* 31(73). [M]
- Tobler, W.R. (1979). Smooth pycnophylactic interpolation for geographical regions. *JASA* 74(367):519–530. doi:10.1080/01621459.1979.10481647 [V]
- Tsoumanis, A.C., Siettos, C.I., Bafas, G.V., Kevrekidis, I.G. (2010). Equation-free multiscale computations in social networks. *International Journal of Bifurcation and Chaos* 20(11):3673–3688. doi:10.1142/S0218127410027945 [V]
- van Leeuwen, P.J., Künsch, H.R., Nerger, L., Potthast, R., Reich, S. (2019). Particle filters for high-dimensional geoscience applications: a review. *QJRMS* 145(723):2335–2365. doi:10.1002/qj.3551 [V]
- Wickramasuriya, S.L., Athanasopoulos, G., Hyndman, R.J. (2019). Optimal forecast reconciliation for hierarchical and grouped time series through trace minimization. *JASA* 114(526):804–819. doi:10.1080/01621459.2018.1448825 [V]
- Willsky, A.S. (2002). Multiresolution Markov models for signal and image processing. *Proceedings of the IEEE* 90(8):1396–1458. doi:10.1109/JPROC.2002.800717 [V]

**Calibration, data assimilation and validation**
- Andrianakis, I., Vernon, I.R., McCreesh, N., McKinley, T.J., Oakley, J.E., Nsubuga, R.N., et al. (2015). Bayesian history matching of complex infectious disease models using emulation. *PLoS Computational Biology* 11(1):e1003968. doi:10.1371/journal.pcbi.1003968 [V]
- Batty, M., Torrens, P.M. (2005). Modelling and prediction in a complex world. *Futures* 37(7):745–766. doi:10.1016/j.futures.2004.11.003 [V]
- Beaumont, M.A., Zhang, W., Balding, D.J. (2002). Approximate Bayesian computation in population genetics. *Genetics* 162(4):2025–2035. doi:10.1093/genetics/162.4.2025 [V]
- Bellman, R., Åström, K.J. (1970). On structural identifiability. *Mathematical Biosciences* 7(3–4):329–339. doi:10.1016/0025-5564(70)90132-X [V]
- Brown, D.G., Page, S., Riolo, R., Zellner, M., Rand, W. (2005). Path dependence and the validation of agent-based spatial models of land use. *IJGIS* 19(2):153–174. doi:10.1080/13658810410001713399 [V]
- Costanza, R. (1989). Model goodness of fit: a multiple resolution procedure. *Ecological Modelling* 47(3–4):199–215. doi:10.1016/0304-3800(89)90001-X [V]
- Craig, P.S., Goldstein, M., Seheult, A.H., Smith, J.A. (1997). Pressure matching for hydrocarbon reservoirs. In *Case Studies in Bayesian Statistics*, Lecture Notes in Statistics, 37–93. doi:10.1007/978-1-4612-2290-3_2 [V]
- Cranmer, K., Brehmer, J., Louppe, G. (2020). The frontier of simulation-based inference. *PNAS* 117(48):30055–30062. doi:10.1073/pnas.1912789117 [V]
- Crooks, A., Castle, C., Batty, M. (2008). Key challenges in agent-based modelling for geo-spatial simulation. *CEUS* 32(6):417–430. doi:10.1016/j.compenvurbsys.2008.09.004 [V]
- Diebold, F.X., Mariano, R.S. (1995). Comparing predictive accuracy. *Journal of Business & Economic Statistics* 13(3):253–263. doi:10.1080/07350015.1995.10524599 [V]
- Fagiolo, G., Guerini, M., Lamperti, F., Moneta, A., Roventini, A. (2019). Validation of agent-based models in economics and finance. In *Computer Simulation Validation*, Springer, 763–787. doi:10.1007/978-3-319-70766-2_31 [V]
- Gallant, A.R., Tauchen, G. (1996). Which moments to match? *Econometric Theory* 12(4):657–681. doi:10.1017/S0266466600006976 [V]
- Gneiting, T., Raftery, A.E. (2007). Strictly proper scoring rules, prediction, and estimation. *JASA* 102(477):359–378. doi:10.1198/016214506000001437 [V]
- Gourieroux, C., Monfort, A., Renault, E. (1993). Indirect inference. *Journal of Applied Econometrics* 8(S1):S85–S118. doi:10.1002/jae.3950080507 [V]
- Grazzini, J., Richiardi, M.G., Tsionas, M. (2017). Bayesian estimation of agent-based models. *Journal of Economic Dynamics and Control* 77:26–47. doi:10.1016/j.jedc.2017.01.014 [V]
- Grimm, V., Revilla, E., Berger, U., Jeltsch, F., Mooij, W.M., Railsback, S.F., et al. (2005). Pattern-oriented modeling of agent-based complex systems: lessons from ecology. *Science* 310(5750):987–991. doi:10.1126/science.1116681 [V]
- Gutenkunst, R.N., Waterfall, J.J., Casey, F.P., Brown, K.S., Myers, C.R., Sethna, J.P. (2007). Universally sloppy parameter sensitivities in systems biology models. *PLoS Computational Biology* 3(10):e189. doi:10.1371/journal.pcbi.0030189 [V]
- Hagen, A. (2003). Fuzzy set approach to assessing similarity of categorical maps. *IJGIS* 17(3):235–249. doi:10.1080/13658810210157822 [V]
- Hagen-Zanker, A., Lajoie, G. (2008). Neutral models of landscape change as benchmarks in the assessment of model performance. *Landscape and Urban Planning* 86(3–4):284–296. doi:10.1016/j.landurbplan.2008.04.002 [V]
- Hartig, F., Calabrese, J.M., Reineking, B., Wiegand, T., Huth, A. (2011). Statistical inference for stochastic simulation models—theory and application. *Ecology Letters* 14(8):816–827. doi:10.1111/j.1461-0248.2011.01640.x [V]
- Heppenstall, A., Crooks, A., Malleson, N., Manley, E., Ge, J., Batty, M. (2021). Future developments in geographical agent-based models. *Geographical Analysis* 53(1):76–91. doi:10.1111/gean.12267 [V]
- Iskauskas, A., Vernon, I., Goldstein, M., Scarponi, D., McCreesh, N., McKinley, T.J., et al. (2024). Emulation and history matching using the hmer package. *Journal of Statistical Software* 109(10). doi:10.18637/jss.v109.i10 [V]
- Kennedy, M.C., O'Hagan, A. (2001). Bayesian calibration of computer models. *JRSS B* 63(3):425–464. doi:10.1111/1467-9868.00294 [V]
- Kieu, L.-M., Malleson, N., Heppenstall, A. (2020). Dealing with uncertainty in agent-based models for short-term predictions. *R. Soc. Open Sci.* 7(1):191074. doi:10.1098/rsos.191074 [V]
- Klinger, E., Rickert, D., Hasenauer, J. (2018). pyABC: distributed, likelihood-free inference. *Bioinformatics* 34(20):3591–3593. doi:10.1093/bioinformatics/bty361 [V]
- Lamperti, F., Roventini, A., Sani, A. (2018). Agent-based model calibration using machine learning surrogates. *JEDC* 90:366–389. doi:10.1016/j.jedc.2018.03.011 [V]
- Lee, J.-S., Filatova, T., Ligmann-Zielinska, A., Hassani-Mahmooei, B., Stonedahl, F., Lorscheid, I., et al. (2015). The complexities of agent-based modeling output analysis. *JASSS* 18(4):4. doi:10.18564/jasss.2897 [V]
- Malleson, N., Birkin, M., Birks, D., Ge, J., Heppenstall, A., Manley, E., et al. (2022). Agent-based modelling for urban analytics: state of the art and challenges. *AI Communications* 35(4):393–406. doi:10.3233/AIC-220114 [V]
- Malleson, N., Minors, K., Kieu, L.-M., Ward, J.A., West, A., Heppenstall, A. (2020). Simulating crowds in real time with agent-based modelling and a particle filter. *JASSS* 23(3):3. doi:10.18564/jasss.4266 [V]
- McCulloch, J., Ge, J., Ward, J.A., Heppenstall, A., Polhill, J.G., Malleson, N. (2022). Calibrating agent-based models using uncertainty quantification methods. *JASSS* 25(2):1. doi:10.18564/jasss.4791 [V]
- Meyer, H., Pebesma, E. (2021). Predicting into unknown space? Estimating the area of applicability of spatial prediction models. *Methods in Ecology and Evolution* 12(9):1620–1633. doi:10.1111/2041-210X.13650 [V]
- Platt, D. (2020). A comparison of economic agent-based model calibration methods. *JEDC* 113:103859. doi:10.1016/j.jedc.2020.103859 [V]
- Ploton, P., Mortier, F., Réjou-Méchain, M., Barbier, N., Picard, N., Rossi, V., et al. (2020). Spatial validation reveals poor predictive performance of large-scale ecological mapping models. *Nature Communications* 11(1):4540. doi:10.1038/s41467-020-18321-y [V]
- Pontius, R.G., Huffaker, D., Denman, K. (2004). Useful techniques of validation for spatially explicit land-change models. *Ecological Modelling* 179(4):445–461. doi:10.1016/j.ecolmodel.2004.05.010 [V]
- Pontius, R.G., Boersma, W., Castella, J.-C., Clarke, K., de Nijs, T., Dietzel, C., et al. (2008). Comparing the input, output, and validation maps for several models of land change. *Annals of Regional Science* 42(1):11–37. doi:10.1007/s00168-007-0138-2 [V]
- Pontius, R.G., Millones, M. (2011). Death to Kappa: birth of quantity disagreement and allocation disagreement for accuracy assessment. *International Journal of Remote Sensing* 32(15):4407–4429. doi:10.1080/01431161.2011.552923 [V]
- Poole, D., Raftery, A.E. (2000). Inference for deterministic simulation models: the Bayesian melding approach. *JASA* 95(452):1244–1255. doi:10.1080/01621459.2000.10474324 [V]
- Price, L.F., Drovandi, C.C., Lee, A., Nott, D.J. (2018). Bayesian synthetic likelihood. *Journal of Computational and Graphical Statistics* 27(1):1–11. doi:10.1080/10618600.2017.1302882 [V]
- Raue, A., Kreutz, C., Maiwald, T., Bachmann, J., Schilling, M., Klingmüller, U., et al. (2009). Structural and practical identifiability analysis of partially observed dynamical models by exploiting the profile likelihood. *Bioinformatics* 25(15):1923–1929. doi:10.1093/bioinformatics/btp358 [V]
- Roberts, D.R., Bahn, V., Ciuti, S., Boyce, M.S., Elith, J., Guillera-Arroita, G., et al. (2017). Cross-validation strategies for data with temporal, spatial, hierarchical, or phylogenetic structure. *Ecography* 40(8):913–929. doi:10.1111/ecog.02881 [V]
- Smith, A.A. (1993). Estimating nonlinear time-series models using simulated vector autoregressions. *Journal of Applied Econometrics* 8(S1):S63–S84. doi:10.1002/jae.3950080506 [V]
- ten Broeke, G., van Voorn, G., Ligtenberg, A. (2016). Which sensitivity analysis method should I use for my agent-based model? *JASSS* 19(1):5. doi:10.18564/jasss.2857 [V]
- Tejero-Cantero, A., Boelts, J., Deistler, M., Lueckmann, J.-M., Durkan, C., Gonçalves, P.J., et al. (2020). sbi: a toolkit for simulation-based inference. *JOSS* 5(52):2505. doi:10.21105/joss.02505 [V]
- Thiele, J.C., Kurth, W., Grimm, V. (2014). Facilitating parameter estimation and sensitivity analysis of agent-based models: a cookbook using NetLogo and R. *JASSS* 17(3):11. doi:10.18564/jasss.2503 [V]
- Toni, T., Welch, D., Strelkowa, N., Ipsen, A., Stumpf, M.P.H. (2009). Approximate Bayesian computation scheme for parameter inference and model selection in dynamical systems. *J. R. Soc. Interface* 6(31):187–202. doi:10.1098/rsif.2008.0172 [V]
- Transtrum, M.K., Machta, B.B., Brown, K.S., Daniels, B.C., Myers, C.R., Sethna, J.P. (2015). Perspective: sloppiness and emergent theories in physics, biology, and beyond. *Journal of Chemical Physics* 143(1):010901. doi:10.1063/1.4923066 [V]
- van Vliet, J., Bregt, A.K., Hagen-Zanker, A. (2011). Revisiting Kappa to account for change in the accuracy assessment of land-use change models. *Ecological Modelling* 222(8):1367–1375. doi:10.1016/j.ecolmodel.2011.01.017 [V]
- van Vliet, J., Bregt, A.K., Brown, D.G., van Delden, H., Heckbert, S., Verburg, P.H. (2016). A review of current calibration and validation practices in land-change modeling. *Environmental Modelling & Software* 82:174–182. doi:10.1016/j.envsoft.2016.04.017 [V]
- Vernon, I., Goldstein, M., Bower, R.G. (2010). Galaxy formation: a Bayesian uncertainty analysis. *Bayesian Analysis* 5(4):619–669. doi:10.1214/10-BA524 [V]
- Verstegen, J.A., Karssenberg, D., van der Hilst, F., Faaij, A.P.C. (2014). Identifying a land use change cellular automaton by Bayesian data assimilation. *Environmental Modelling & Software* 53:121–136. doi:10.1016/j.envsoft.2013.11.009 [V]
- Wadoux, A.M.J.-C., Heuvelink, G.B.M., de Bruin, S., Brus, D.J. (2021). Spatial cross-validation is not the right way to evaluate map accuracy. *Ecological Modelling* 457:109692. doi:10.1016/j.ecolmodel.2021.109692 [V]
- Ward, J.A., Evans, A.J., Malleson, N.S. (2016). Dynamic calibration of agent-based models using data assimilation. *R. Soc. Open Sci.* 3(4):150703. doi:10.1098/rsos.150703 [V]
- Williamson, D., Goldstein, M., Allison, L., Blaker, A., Challenor, P., Jackson, L., et al. (2013). History matching for exploring and reducing climate model parameter space using observations and a large perturbed physics ensemble. *Climate Dynamics* 41(7–8):1703–1729. doi:10.1007/s00382-013-1896-4 [V]
- Wood, S.N. (2010). Statistical inference for noisy nonlinear ecological dynamic systems. *Nature* 466(7310):1102–1104. doi:10.1038/nature09319 [V]
- Yu, J., Hagen-Zanker, A., Santitissadeekorn, N., Hughes, S. (2021). Calibration of cellular automata urban growth models from urban genesis onwards: a novel application of Markov chain Monte Carlo approximate Bayesian computation. *CEUS* 90:101689. doi:10.1016/j.compenvurbsys.2021.101689 [V]
- Zhang, Y., Li, X., Liu, X., Qiao, J. (2015). Self-modifying CA model using dual ensemble Kalman filter for simulating urban land-use changes. *IJGIS* 29(9):1612–1631. doi:10.1080/13658816.2015.1037305 [V]

**Related, cited in passing**
- Barbosa, H., Barthelemy, M., Ghoshal, G., James, C.R., Lenormand, M., Louail, T., et al. (2018). Human mobility: models and applications. *Physics Reports* 734:1–74. doi:10.1016/j.physrep.2018.01.001 [V]
- Brunton, S.L., Proctor, J.L., Kutz, J.N. (2016). Discovering governing equations from data by sparse identification of nonlinear dynamical systems. *PNAS* 113(15):3932–3937. doi:10.1073/pnas.1517384113 [V]
- Simini, F., González, M.C., Maritan, A., Barabási, A.-L. (2012). A universal model for mobility and migration patterns. *Nature* 484(7392):96–100. doi:10.1038/nature10856 [V]

**Reference implementations** (URL resolved this session [V], except where marked)
- DAPPER: https://github.com/nansencenter/DAPPER [V]
- deeptime (PCCA+, Markov models): https://github.com/deeptime-ml/deeptime [V]
- ELFI: https://github.com/elfi-dev/elfi [V]
- FMI standard: https://fmi-standard.org [V]
- FMPy: https://github.com/CATIA-Systems/FMPy [V]
- giddy (spatial Markov chains): https://github.com/pysal/giddy [V]
- hierarchicalforecast (MinT): https://github.com/Nixtla/hierarchicalforecast [V]
- hmer (R): https://cran.r-project.org/package=hmer [V]
- MATSim: https://github.com/matsim-org/matsim-libs [V]
- Mesa (ABM): https://github.com/projectmesa/mesa [V]
- NeuralABM: https://github.com/ThGaskin/NeuralABM [V]
- OpenMOLE: https://openmole.org [V]
- pyABC: https://github.com/ICB-DCM/pyABC [V]
- PySD: https://github.com/SDXorg/pysd [V]
- Reference FMUs: https://github.com/modelica/Reference-FMUs [V]
- sbi: https://github.com/sbi-dev/sbi [V]
- SLEUTH (Project Gigalopolis; USGS SLEUTH-3r): http://www.ncgia.ucsb.edu/projects/gig/ (did not respond this session) [S]
- spint (spatial interaction): https://github.com/pysal/spint [V]
- SUNDIALS ARKODE MRIStep: https://sundials.readthedocs.io/en/latest/arkode/Usage/MRIStep/index.html [V]
- tempdisagg (R): https://cran.r-project.org/package=tempdisagg [V]
- UrbanSim: https://github.com/UDST/urbansim [V]
- Urban-Analytics uncertainty (history matching for urban ABMs): https://github.com/Urban-Analytics/uncertainty [V]

---

## Part II. Estimating Urban Indexes a Dataset Does Not Hold, and Withholding Estimates That Cannot Be Made Well

### 0. Scope and Markers

This survey covers the estimators by which an urban index that is not observed at the wanted unit or time can be estimated from what is observed: change of support between spatial units (§1), small-area estimation (§2), gridded and disaggregated estimates from proxies (§3), synthetic populations and spatial microsimulation (§4), temporal disaggregation, nowcasting and intercensal estimation (§5), hierarchical Bayesian spatial models, machine learning with spatial validation, the area of applicability and conformal intervals (§6), and the rules by which an estimate should be withheld and the quantity declared instead (§7); §8 takes the urban indexes that are most often missing one by one, §9 states the contract of an estimator that the literature implies in the studio, §10 ranks recommendations, and a bibliography closes. Every bibliographic item carries a marker: **[V]** its DOI (or arXiv identifier, or the publisher's or office's own page) was resolved this session and authors, title, venue and year checked; **[S]** confirmed only from secondary sources (search summaries, mirrors, citing works); **[M]** from memory, not verified. Volumes and pages are given only where the record gave them. Thresholds of statistical offices are marked the same way, by the page they were read from. Package names are given as reference implementations; those not backed by a verified paper are from knowledge of the ecosystem and should be checked before a check is written against them.

### 1. Change of Support Across Spatial Units

#### 1.1 Concepts

- **Source and target zones.** An index $y$ is known on source zones $S_1,\dots,S_m$ (census tracts, statistical areas, wards) and wanted on target zones $T_1,\dots,T_n$ (blocks, grid cells, catchments, a planner's district). The general problem is the *change of support problem* (COSP): inference about a variable at one spatial support from data at another (Gotway and Young 2002 [V]; Gelfand, Zhu and Carlin 2001 [V]).
- **Extensive and intensive variables.** Extensive indexes (counts, totals, floor area, revenue) add over disjoint parts; intensive ones (densities, rates, shares, means, medians, Gini) do not. Area weighting is defined for extensive variables; an intensive one must be carried as a ratio of two extensive ones (a rate as cases over population, a mean income as total income over persons) or it cannot be moved between supports honestly. A median or a Gini cannot be interpolated by weights at all: it requires the distribution within each zone (binned counts with an interpolation rule, or the micro-units of §4).
- **The pycnophylactic (volume-preserving) property.** An allocation $\hat y(S_s\cap T_t)$ is volume-preserving if $\sum_t \hat y(S_s\cap T_t)=y_s$ for every source zone. It is the one property a check can hold exactly for every method in this section, and it is what makes downward allocation consistent with upward aggregation in a hierarchy of units.

#### 1.2 Methods

- **Areal weighting.** $\hat y_t=\sum_s y_s\,|S_s\cap T_t|/|S_s|$. Assumes uniform density within each source zone; volume-preserving; error grows with the heterogeneity inside source zones and with the ratio of source to target size (Goodchild and Lam 1980 [M]; Flowerdew, Green and Kehris 1991 [V]; Comber and Zeng 2019 [V], a review with code).
- **Tobler's smooth pycnophylactic interpolation** (Tobler 1979 [V]). A density surface on a fine raster that is as smooth as possible (a discrete Laplacian criterion: in the iterative scheme each cell is replaced by the mean of its neighbours) subject to exact preservation of each zone's volume and non-negativity, alternating smoothing and rescaling within zones. Guarantees: volume preservation and non-negativity at every iteration. Assumption: density varies smoothly across zone boundaries, which is false across rivers, rail yards and parks; it puts population into water and smears sharp edges between a tower block and a park. Its value for the design is as the *uninformed* baseline against which an ancillary-data method must show a gain.
- **Dasymetric mapping.** Density within a source zone is redistributed in proportion to an ancillary variable known on the fine support (Wright 1936 [V], Cape Cod). Binary dasymetric: inhabited versus uninhabited land cover; multi-class: relative densities by class. Eicher and Brewer (2001) [V] implemented and evaluated areal weighting against binary, three-class and limiting-variable dasymetric methods on withheld county counts, and found the dasymetric methods more accurate. Mennis (2003) [V] estimated the relative densities of land-cover classes empirically, from source zones dominated by one class, combined with each class's area share; Mennis and Hultgren (2006) [V], *intelligent dasymetric mapping*, sample class densities from zones where a class dominates (or accept densities a user states) and keep the pycnophylactic property. Langford (2007) [V] made the binary method a raster operation.
- **Building-based (volumetric) dasymetric.** In cities the ancillary variable that matters is residential floor area, not land cover: population in proportion to building footprint area, floor area or volume (Lwin and Murayama 2009 [V]); a nation-wide test on a 3D city model found floor area from the model improved estimates over footprints alone (Biljecki et al. 2016 [V]). Jobs and visitors follow non-residential floor area by use in the same way. Failure modes: vacancy, under-occupied dwellings and student or institutional housing break proportionality; heights missing from building data are imputed and should be declared.
- **Statistical (regression) dasymetric.** Counts on source zones are modelled as Poisson with means that are sums of target-part intensities depending on covariates, fitted by EM (the Flowerdew–Green approach, Flowerdew et al. 1991 [V]); Do, Thomas-Agnan and Vanhems (2015) [V] compared these and the classical methods on count data and give accuracy criteria. Nagle, Buttenfield, Leyk and Spielman (2014) [V] cast dasymetric modelling as estimation with an explicit statement of uncertainty, so that an allocation carries an interval and not only a point.
- **Area-to-point kriging.** Kyriakidis (2004) [V]: the point-support covariance is deconvolved from areal data, and kriging predicts at points (or any finer support) with a kriging variance; the predictions are *coherent*: averaged back over a source zone they return its datum, the geostatistical form of volume preservation. Area-to-point Poisson kriging (Goovaerts 2006 [V]) handles rates whose noise depends on population size, the case of small-area rates. Bayesian COSP: block averages of a latent Gaussian process (Gelfand, Zhu and Carlin 2001 [V]); count-valued survey data with spatial change of support, applied to the American Community Survey (Bradley, Wikle and Holan 2016 [V]); *disaggregation regression*, aggregated Poisson counts as sums over pixels of a covariate-driven intensity with a spatial field, fitted with Laplace approximations (Nandi et al. 2023 [V], the R package `disaggregation`).

#### 1.3 Errors and Validation

- **Validation by re-aggregation.** Aggregate a fine level where the truth is known (census blocks, address points) to a coarse support, interpolate back down, compare: RMSE, MAE and relative error per target zone, and the share of target zones within a stated relative error. Every paper cited above validates this way; the test is honest only if the ancillary data are not themselves derived from the truth.
- **What drives error.** Within-zone heterogeneity of the ancillary relation; misclassification of the ancillary layer; a wrong class-density ratio; target zones much smaller than source zones. Errors are spatially structured and largest at the urban fringe and in mixed-use areas.
- **Uncertainty is rarely reported.** Only the kriging and Bayesian methods produce variances by construction; for dasymetric methods Nagle et al. (2014) [V] is the exception. A bootstrap over the class densities, or a posterior over them, is the cheap remedy.

#### 1.4 Reference Implementations

PySAL's `tobler` (areal, dasymetric with a raster mask, and regression-based interpolation; Python); R `sf::st_interpolate_aw`, `areal`, `pycno` (Tobler's method); `disaggregation` (Nandi et al. 2023 [V]); `atakrig` (area-to-area and area-to-point kriging, R). The volume-preservation identity and the exactness of dasymetric allocation when density is truly piecewise constant on ancillary classes are closed forms a check can hold without any of them.

#### 1.5 Consequences for the Design

- **Index served:** every extensive index (population, households, dwellings, jobs, floor area, revenue) and every intensive index carried as a ratio of extensives.
- **Level:** the downward coupling of the hierarchy (district → neighbourhood → block → cell). A volume-preserving allocator *is* "allocation downward under control totals"; with floor area by use as the ancillary layer it is the natural urban one.
- **Coupling:** aggregation upward is exact summation, allocation downward is dasymetric with a declared ancillary relation; the pair commutes on the source zones by construction, which is the invariant to check after every step of a multi-level model.
- **Refusal:** an intensive index without its numerator and denominator, or a quantile without the distribution, is refused at a finer support.

### 2. Small-Area Estimation

#### 2.1 Concepts

A *small area* is a domain whose own sample is too small for a direct estimate of adequate precision. Small-area estimation (SAE) borrows strength from other areas through a model linking the index to auxiliary data known for every area (Rao and Molina 2015 [V], the standard text; Pfeffermann 2013 [V], a review; Jiang and Lahiri 2006 [V], mixed-model prediction). Two families: *area-level* models, which take a direct estimate and its sampling variance per area, and *unit-level* models, which take sampled units with covariates known for the whole population.

#### 2.2 Area-Level Models

- **Fay–Herriot** (Fay and Herriot 1979 [V], income for places under 1,000 people). $\hat\theta_i=\theta_i+e_i$, $e_i\sim N(0,\psi_i)$ with $\psi_i$ *known*; $\theta_i=x_i^\top\beta+v_i$, $v_i\sim N(0,\sigma_v^2)$. The EBLUP is $\tilde\theta_i=\gamma_i\hat\theta_i+(1-\gamma_i)x_i^\top\hat\beta$ with $\gamma_i=\sigma_v^2/(\sigma_v^2+\psi_i)$: imprecise areas shrink to the synthetic regression value. Assumptions: linearity and normality on the scale modelled; $\psi_i$ known (in practice smoothed by a generalised variance function, whose error is then ignored); no informative selection of areas. Out-of-sample areas get the synthetic estimate, whose validity rests wholly on the model. The US Census Bureau's SAIPE programme produces county and school-district poverty this way (Bell, Basel and Maples 2016 [V]).
- **MSE.** Prasad and Rao (1990) [V]: $\mathrm{MSE}\approx g_{1i}+g_{2i}+2g_{3i}$, with $g_{1i}=\gamma_i\psi_i$ (the leading term), $g_{2i}$ from estimating $\beta$, $g_{3i}$ from estimating $\sigma_v^2$; second-order unbiased under normality. Datta, Rao and Smith (2005) [V] give the corrections for other estimators of $\sigma_v^2$. Parametric bootstrap MSE (González-Manteiga et al. 2008 [V]) for cases where the analytic form is not available. Failure: $\hat\sigma_v^2$ truncated at zero makes every area synthetic and the MSE overconfident; model-based MSE is conditional on the model and blind to misspecification.
- **Spatial Fay–Herriot.** Random effects follow a SAR process over an adjacency (Pratesi and Salvati 2008 [V]). Gains are largest when covariates are weak and the residual is spatially smooth.
- **Spatio-temporal Fay–Herriot.** Area effects plus area-by-time effects following AR(1) (Rao and Yu 1994 [V]); SAR in space with AR(1) in time (Marhuenda, Molina and Morales 2013 [V]). This is a filter over a panel of direct estimates: the area-level analogue of a state-space model.
- **Multivariate Fay–Herriot** (Benavent and Morales 2016 [V]): correlated indexes (rent and income, unemployment and informality) estimated jointly, with the sampling covariance between them.
- **Covariates measured with error** (Ybarra and Lohr 2008 [V]): when the auxiliary variable is itself an estimate (a gridded population, a model-based proxy), the naive EBLUP can be worse than the direct estimate; the corrected estimator shrinks less. This applies directly to proxies from §3.
- **Big-data covariates.** Mobility traces (Marchetti et al. 2015 [V]) and mobile-phone indicators (Schmid et al. 2017 [V], literacy in Senegal) used as Fay–Herriot covariates; geospatial covariates for non-monetary poverty (Masaki et al. 2022 [V]). This is the defensible use of proxies: inside a model whose MSE accounts for them, not as standalone predictions.

#### 2.3 Unit-Level Models

- **Battese–Harter–Fuller nested-error model** (Battese, Harter and Fuller 1988 [V], county crop areas from survey and satellite data): $y_{ij}=x_{ij}^\top\beta+v_i+e_{ij}$. The EBLUP of an area mean needs the area's population mean of $x$ (from a census, a register or a land-cover map). Survey weights enter through the pseudo-EBLUP for design consistency (You and Rao 2002 [V]).
- **ELL / World Bank poverty mapping** (Elbers, Lanjouw and Lanjouw 2003 [V]). A household model fitted on the survey is used to simulate welfare for every census household many times, and any indicator (FGT poverty, Gini, Theil) is computed per area with a simulation variance. Critique: the random effect was specified at the survey cluster, not the target area, so between-area variation that the covariates do not explain is attributed to noise and the MSE is badly underestimated (Das and Chambers 2017 [V], with a robust MSE; Molina and Rao 2010 [V]); the assumption that one national model holds in every area (Tarozzi and Deaton 2009 [V]). The World Bank's own update (Corral, Molina and Nguyen 2021 [V], "Pull your small area estimates up by the bootstraps") documents better MSE and accuracy for census empirical-best methods, and its *Guidelines to Small Area Estimation for Poverty Mapping* (Corral et al. 2022 [S]) now recommend empirical best with area-level effects.
- **Empirical best (Molina–Rao).** Under BHF with normality (usually after a log or Box–Cox transform), the EB predictor of any nonlinear indicator of the area's distribution is obtained by Monte Carlo from the conditional distribution of the non-sampled units given the sampled ones; MSE by parametric bootstrap (Molina and Rao 2010 [V]); a hierarchical Bayes version (Molina, Nandram and Rao 2014 [V]). Guarantee: optimal under the model; failures: non-normal residuals after transformation, heteroscedasticity, informative sampling, and the need for unit-level census covariates matched in definition to the survey's.
- **M-quantile models** (Chambers and Tzavidis 2006 [V]): area effects as area-specific M-quantile coefficients; no distributional assumption on random effects, robust to outliers; analytic and bootstrap MSE.
- **Mixed-effects random forests** (Krennmair and Schmid 2022 [V]): a random forest for the fixed part with area random effects, with a bootstrap MSE; flexible, but its MSE is less established than the linear models'.

#### 2.4 Benchmarking and Structure Preservation

- **Benchmarking** forces small-area estimates to sum to a published total (a regional direct estimate, an administrative count): ratio and difference adjustment, Bayesian benchmarking (Datta, Ghosh, Steorts and Maples 2011 [V]), smoothing with benchmarking (Steorts, Schmid and Tzavidis 2020 [V]); state-space models with benchmark constraints for monthly series (Pfeffermann and Tiller 2006 [V], the method behind US state labour-force estimates). Benchmarking costs a little MSE and buys consistency between levels.
- **Structure-preserving estimation (SPREE).** A census cross-classification (area × category) is updated to new margins by iterative proportional fitting, keeping the census's association structure (Purcell and Kish 1980 [V]); generalised SPREE lets the association change by a model (Zhang and Chambers 2004 [V]). This is the tool for updating intercensal tables of households by type and tenure by area under new control totals.

#### 2.5 Validation of Small-Area Estimates

There is rarely truth at the small-area level. Tzavidis et al. (2018) [V] set out a framework from specification to publication: transformation, model diagnostics, comparison of model-based with direct estimates (a regression of direct on model-based with slope one, coverage of direct estimates by model intervals, the gain in CV), and benchmarking. Design-based simulation from a census or a synthetic population (the `emdi` and `saeSim` approach) is the only test of bias.

#### 2.6 Reference Implementations

R `sae` (area-level FH, spatial and spatio-temporal FH, BHF, EB with bootstrap MSE, with example data); `emdi` (Kreutzmann et al. 2019 [V]: direct, EBP and FH estimation of regionally disaggregated indicators, with MSE, diagnostics and maps); `SAEforest`; `hbsae`; `msae`; Stata `sae` (World Bank). Python has fewer: `samplics` implements EBLUP area and unit models.

#### 2.7 Consequences for the Design

- **Index served:** every survey-measured index wanted below the survey's design level: income, poverty and inequality, rents and housing cost burden, vacancy, informality, unemployment, and the perceived indexes (safety, trust, attachment) from social surveys.
- **Level:** neighbourhood and district, where direct estimates exist but are noisy; blocks only with unit-level census covariates.
- **Coupling:** benchmarking is the aggregation constraint between levels; spatio-temporal Fay–Herriot and Pfeffermann–Tiller are the bridge to the dynamical models, since an index's state at a level is then filtered, not just estimated. SPREE carries a cross-classification forward between censuses.
- **What exists:** the statistics layer has Fay–Herriot EBLUP and its MSE; the extensions above (spatial, spatio-temporal, multivariate, measurement error, EB for nonlinear indexes, benchmarking) are the gap.

### 3. Gridded and Disaggregated Estimates from Proxies

#### 3.1 Concepts

A *proxy* is a variable observed everywhere (built-up area, night-time radiance, daytime imagery, phone activity) that is related to the wanted index where both are observed. Two uses must be kept apart: *top-down disaggregation*, in which a proxy only shapes the distribution of a known total (volume-preserving, §1), and *prediction*, in which a model fitted where the index is observed predicts it where it is not. The first inherits the total's accuracy and errs only in placement; the second carries the full error of extrapolation and is the use the critiques below are about.

#### 3.2 Population

- **WorldPop top-down.** A random forest predicts log population density at census-unit level from land cover, night lights, roads, settlement extents, elevation and the like; its predictions at ~100 m become weights for a dasymetric redistribution of each unit's census count (Stevens, Gaughan, Linard and Tatem 2015 [V]; the programme, Tatem 2017 [V]). Validation: fit at a coarse administrative level and compare with counts at a finer one. Guarantee: totals preserved; failures: the forest is trained on unit averages and applied to pixels (an ecological change of scale), and dense vertical housing is under-allocated.
- **Bottom-up.** Where censuses are old or absent: microcensus surveys of enumeration areas plus building footprints and settlement types in a hierarchical Bayesian model that yields a posterior predictive distribution per cell (Wardrop et al. 2018 [V]; Leasure et al. 2020 [V]). This is the one gridded population product that states cell-level uncertainty.
- **GHS-POP.** The JRC's Global Human Settlement Layer disaggregates census units by built-up volume from its own built-surface and height layers; its 2024 assessment joins Earth observation with survey data (Pesaresi et al. 2024 [V]).
- **Fitness for use.** Gridded products agree on totals and disagree at fine scale; the choice of product changes derived indicators (Leyk et al. 2019 [V], review; Tuholske et al. 2021 [V], SDG metrics differ by product); in slums several products underestimate population substantially (Thomson et al. 2021 [V]). There is also a circularity: a population grid built from built-up area and lights cannot then be used to "explain" built-up area or lights.

#### 3.3 Output and Income from Night Lights

- **Henderson, Storeygard and Weil (2012)** [V]: lights as a second, noisy measure of income growth; under the assumption that the measurement errors of lights and of national accounts are uncorrelated, they combine the two, and for the countries with the weakest statistical systems the optimal estimate gives the two roughly equal weight; for countries with good accounts lights add little.
- **Chen and Nordhaus (2011)** [V]: luminosity adds value as a proxy only where statistics are poor; for countries and grid cells with good data its contribution is small.
- **Critiques and corrections.** DMSP sensors saturate in bright city centres (top-coding; Bluhm and Krause 2022 [V] correct it with a Pareto tail and find bright cities' contribution understated) and bloom beyond lit areas; DMSP performs poorly in rural and low-density areas where VIIRS does better (Gibson, Olivia, Boe-Gibson and Li 2021 [V]); intercalibration across satellites is a model in itself (Elvidge et al. 2017 [V], VIIRS; Li, Zhou, Zhao and Zhao 2020 [V], a harmonised 1992–2018 series); lights reveal that autocracies overstate GDP growth (Martínez 2022 [V]), which is a use of lights as an *audit*, not as a substitute. Within a city, radiance measures commercial and street lighting and built intensity; its validity as a measure of residents' income at block level is not established.

#### 3.4 Poverty and Wealth from Imagery and Phones

- **Imagery.** Jean et al. (2016) [V]: a convolutional network trained to predict night lights from daytime imagery yields features that, through ridge regression, predict cluster consumption and assets in five African countries; assets better than consumption, and poorly below the poverty line. Yeh et al. (2020) [V]: public Landsat imagery predicts an asset index across ~20,000 African villages; changes over time are predicted much less well than levels. Engstrom, Hersh and Newhouse (2022) [V]: high-resolution features predict village poverty in Sri Lanka. Burke, Driscoll, Lobell and Ermon (2021) [V] review the field and its need for ground truth; MOSAIKS (Rolf et al. 2021 [V]) gives generic random-convolution features and tests spatial extrapolation explicitly with checkerboard folds.
- **Phones.** Blumenstock, Cadamuro and On (2015) [V] predict wealth from call-detail records in Rwanda; phone-based targeting of aid (Aiken et al. 2022 [V]).
- **The relative wealth index** (Chi, Fang, Chatterjee and Blumenstock 2022 [V]): wealth at 2.4 km for all low- and middle-income countries, from imagery, connectivity and phone data, trained on the DHS asset index. Its authors report spatially stratified cross-validation because random folds overstate skill, and report that accuracy falls at finer resolution (about half of the variance of grid-cell wealth in one national comparison, more at administrative level) and that much of the skill separates urban from rural places [S, from the paper's text via search].
- **Critiques.**
  1. *Spatial leakage.* Neighbouring clusters share imagery and outcomes; random cross-validation inflates performance (acknowledged by Chi et al. 2022 [V]; general case in §6.3).
  2. *Label noise.* DHS cluster coordinates are displaced up to 2 km in urban and 5 km in rural areas, 1% of rural clusters up to 10 km [S, DHS forum and protocols]; the DHS wealth index is a relative, asset-based score within one country and survey.
  3. *Transfer across countries and outcomes.* Models transfer for some indicators and countries and not others (Head, Manguin, Tran and Blumenstock 2017 [V]).
  4. *Within-urban skill.* Satellite poverty maps separate urban from rural well and rank places *within* urban or rural parts poorly; where poverty is rural, urban wealth is under-ranked (Aiken, Rolf and Blumenstock 2023 [V], arXiv:2305.01783); a fairness audit (Kondmann and Zhu 2021 [V], arXiv:2108.02137).
  5. *Against a census-based map.* In Malawi, a map from survey plus satellite predictors reproduces broad patterns and ranks larger groups of areas well, but individual small areas can differ noticeably from the census-based map (van der Weide, Blankespoor, Elbers and Lanjouw 2024 [V]).
  6. *Policy use.* Microtargeting with high-resolution maps (Smythe and Blumenstock 2022 [V]); high-resolution poverty maps for Sub-Saharan Africa (Lee and Braithwaite 2022 [V]); impact evaluation from imagery (Ratledge et al. 2022 [V]).

#### 3.5 Reference Implementations

WorldPop's random-forest code (R) and its released grids; GHS-POP grids; `disaggregation` (R) for model-based top-down allocation; MOSAIKS features (API and code); the RWI's released estimates with their cross-validation folds. For checks, the honest reference is a *protocol*, not a package: aggregate-then-predict-finer validation with spatially separated folds.

#### 3.6 Consequences for the Design

- **Index served:** population and its density (top-down, volume-preserving, legitimate everywhere); output, income and wealth (prediction, legitimate only where validated at the target scale and setting).
- **Level:** cells and blocks for population allocation under census totals; for income and wealth, proxies belong *inside* a small-area model (§2.2, as Fay–Herriot covariates with the Ybarra–Lohr correction) at the neighbourhood level, not as block-level predictions.
- **Coupling:** downward allocation (population) and covariates for the statistical estimators. A registry of proxies, each with its validated setting, scale, target index and validation statistic, is what lets the studio refuse "night lights for block income" mechanically.

### 4. Synthetic Populations and Spatial Microsimulation

#### 4.1 Concepts

A synthetic population is a list of micro-units (persons in households in dwellings) for each small area, consistent with the area's published tables (the *constraints*) and with the joint structure of a sample of micro-records (the *seed*: a public-use sample, a survey). Once it exists, any distributional index (Gini, poverty headcount, housing-cost burden, overcrowding, the joint distribution of rent and income) can be computed for any area or any union of areas, and aggregation upward is exact by construction. The price is an assumption: the joint distribution of variables not linked by any constraint is the seed's, conditional on the constrained ones.

#### 4.2 Methods

- **Iterative proportional fitting (IPF)** (Deming and Stephan 1940 [V]). Scales a seed table to match each margin in turn; when the margins are consistent and the seed has positive cells where needed, it converges to the table of minimum Kullback–Leibler divergence from the seed among those with the given margins, preserving the seed's odds ratios on the unconstrained interactions. Failures: zero cells in the seed stay zero (structural versus sampling zeros); inconsistent margins (from different sources or years) prevent convergence; fractional weights need integerisation, for which *truncate, replicate, sample* is the standard method (Lovelace and Ballas 2013 [V]). Beckman, Baggerly and McKay (1996) [V] built synthetic households by IPF on a public-use sample and drew households from the fitted table; IPF as practice (Lomax and Norman 2016 [V]); new tests of its performance (Lovelace, Birkin, Ballas and van Leeuwen 2015 [V]).
- **Combinatorial optimisation** (Williamson, Birkin and Rees 1998 [V]): select whole records from the sample of anonymised records to fit several small-area tables at once, by simulated annealing; integer by construction; solutions are not unique, so repeated runs give a distribution.
- **Generalised regression reweighting** (GREGWT; Tanton, Vidyattama, Nepal and McNamara 2011 [V]): calibration of survey weights to each area's benchmarks with a distance function and bounds (Deville and Särndal 1992 [V]). Areas where the calibration does not converge within bounds are reported and excluded from release, an explicit refusal rule within the method.
- **Households and persons together:** iterative proportional updating (Ye et al. 2009 [M]).
- **Generative models:** Gibbs sampling from conditionals (Farooq, Bierlaire, Hurtubia and Flötteröd 2013 [V]); Bayesian networks (Sun and Erath 2015 [V]); deep generative models that produce combinations absent from the seed (Borysov, Rich and Pereira 2019 [V]), at the cost of possibly producing impossible ones.
- **Review:** Tanton (2014) [M], spatial microsimulation methods; Lovelace and Dumont (2016) [M], a book with R code.

#### 4.3 Validation and Uncertainty

- **Internal fit:** total absolute error, standardised RMSE and Z-statistics per constraint cell (Voas and Williamson 2001 [V], on goodness-of-fit measures for synthetic microdata).
- **External validation:** tables *not* used as constraints (held-out cross-tabulations) are the real test of the conditional-independence assumption.
- **Uncertainty:** usually not reported. Whitworth, Carter, Ballas and Moon (2017) [V] estimate it by resampling; the sources are the seed's sampling error, the constraint tables' own error (an ACS table carries a margin of error), the integerisation draw, and the structural assumption, which no resampling captures and which must be declared.

#### 4.4 Reference Implementations

R: `mipfp` (multidimensional IPF), `ipfp`, `humanleague` (also Python; integer populations by quasirandom sampling), `rakeR`, `simPop` (Templ, Meindl, Kowarik and Dupriez 2017 [V]), `synthpop` (Nowok, Raab and Dibben 2016 [V], for disclosure-controlled synthetic data, a different purpose); Python `ipfn`; PopulationSim (transport modelling).

#### 4.5 Consequences for the Design

- **Index served:** every distributional index (Gini, poverty, affordability, overcrowding), and any index of a joint distribution, for any area.
- **Level:** the bottom of the hierarchy as micro-units; every coarser level is a sum.
- **Coupling:** the synthetic population is the state of the agent-based and microsimulation models that exist (housing choice, static microsimulation); IPF under new margins is the downward coupling of a slow macro step to the micro state. A distributional index computed from it inherits uncertainty from the seed and the constraints and must report both; one that depends on an unconstrained joint (rent × income when only their margins are constrained) must say so.

### 5. Time: Temporal Disaggregation, Nowcasting and Intercensal Estimation

#### 5.1 Concepts

The time axis has the same change-of-support problem as space: an index is observed annually (a census, an annual survey) and wanted monthly, or observed quarterly with a lag and wanted now. The temporal analogue of volume preservation is the *aggregation constraint*: the high-frequency estimates must sum (flows), average (indexes) or end at (stocks) the low-frequency value.

#### 5.2 Temporal Disaggregation and Benchmarking

- **Chow–Lin** (Chow and Lin 1971 [V]): GLS regression of the low-frequency series on aggregated high-frequency indicators, with AR(1) errors at the high frequency; the BLUE of the high-frequency series satisfies the aggregation constraint exactly. **Fernández** (1981) [V]: random-walk errors (non-stationary residuals). **Litterman** (1983) [V]: random walk–Markov errors.
- **Denton** (1971) [V]: benchmark a high-frequency indicator to low-frequency totals by minimising (additive or proportional) changes in first differences, preserving the indicator's movement; no regression, so usable when the indicator is in other units.
- **State-space formulation** (Proietti 2006 [V]): all of the above as dynamic regressions estimated by the Kalman filter, with diagnostics and missing data handled uniformly.
- **Failures:** an irrelevant indicator gives a smooth interpolation dressed as information; the AR parameter estimated at the boundary of its space; revisions when a new annual value arrives. Reference: R `tempdisagg` (Sax and Steiner 2013 [V]), with worked examples; JDemetra+.

#### 5.3 Nowcasting and Mixed Frequency

- **State-space and the Kalman filter** (Kalman 1960 [V]; Harvey 1989 [M]; Durbin and Koopman 2012 [V]): a latent state evolving by a declared law, observed by noisy, incomplete, mixed-frequency measurements; the filter gives the state's distribution given everything so far, the smoother given everything. A quarterly observation of a monthly state is a linear constraint on three consecutive states (Mariano and Murasawa 2003 [V]).
- **Dynamic factor nowcasts** with ragged edges (Giannone, Reichlin and Small 2008 [V]; Bańbura, Giannone, Modugno and Reichlin 2013 [V]); estimation by EM with arbitrary missing data (Bańbura and Modugno 2014 [V]). Regression alternative: MIDAS (Ghysels, Sinko and Valkanov 2007 [V]).
- **Search and digital proxies:** predicting the present with search volumes (Choi and Varian 2012 [V]); the cautionary case of Google Flu Trends, which drifted as the search engine and behaviour changed and was overfitted to a short record (Lazer, Kennedy, King and Vespignani 2014 [V]). A digital proxy for a nowcast needs continuing recalibration and a stated failure signal.
- **Small areas over time:** state-space models with benchmark constraints (Pfeffermann and Tiller 2006 [V]); Rao–Yu panels (§2.2); Statistics Canada and the ABS have used or tested such composite estimators for sub-national labour-force estimates [S].
- **Reference:** `statsmodels`' state-space classes, including a mixed-frequency dynamic factor model with EM after Bańbura and Modugno; R `KFAS`, `dlm`.

#### 5.4 Intercensal and Postcensal Estimation

- **Cohort-component:** $P_{t+1}=P_t+B-D+I-E$ by age and sex; postcensal estimates carried forward from a census, intercensal ones reconciled with the next census by distributing the *error of closure* across the decade [M on the detail of any office's distribution rule].
- **Housing-unit method:** population = occupied dwellings × persons per household + group quarters, with dwellings from permits, completions, demolitions and utility connections (Smith, Tayman and Swanson 2013 [V], a practitioner's guide). It is the natural method for urban small areas, where the building stock is known better than migration.
- **With uncertainty:** Bayesian reconstruction of past populations from fragmentary data (Wheldon, Raftery, Clark and Gerland 2013 [V]); probabilistic projections (Raftery et al. 2012 [V]; R `bayesPop`, `popReconstruct`).

#### 5.5 Consequences for the Design

- **Index served:** any index with a low-frequency anchor and a high-frequency signal: monthly population from annual estimates and dwelling completions; quarterly rents from annual surveys and listings; footfall and spending nowcasts.
- **Level:** the coupling between time scales. Chow–Lin and Denton *are* the "allocation under control totals" of the time axis; the Kalman smoother with aggregation constraints is the general form, and it is a dynamical system with a declared law and an observation operator, which the dynamical-systems foundation already declares.
- **Coupling:** a slow level (annual) constrains a fast level (monthly) exactly; a fast level aggregated is the slow level's observation. The housing-unit method couples the building stock (the urban domain's buildings and occupancy chain) to population.

### 6. Hierarchical Bayesian Spatial Models, Machine Learning and the Limits of Prediction

#### 6.1 Areal Models

- **BYM** (Besag, York and Mollié 1991 [V]): $\log\theta_i=\mu+x_i^\top\beta+u_i+v_i$, $u$ an intrinsic CAR (improper, rank-deficient, identified with a sum-to-zero constraint per connected component), $v$ unstructured. The two variances are not identifiable separately from one realisation and their priors are not comparable across graphs.
- **BYM2** (Riebler, Sørbye, Simpson and Rue 2016 [V]): the ICAR is scaled so that the geometric mean of its marginal variances is one; then $u+v=\sigma\,(\sqrt{\phi}\,u^\ast+\sqrt{1-\phi}\,v^\ast)$ with $\phi$ the share of spatial variance, and penalised-complexity priors (Simpson et al. 2017 [V]) become meaningful. The scaling factor is a closed-form computation on the graph (a generalised inverse of the ICAR precision), which a check can hold.
- **Leroux** (Leroux, Lei and Breslow 2000 [V]): one random effect with precision $\tau[(1-\lambda)I+\lambda(D-W)]$, proper for $\lambda<1$, interpolating between independence and ICAR.
- **Inference:** INLA for latent Gaussian models (Rue, Martino and Chopin 2009 [V]); Matérn fields as GMRFs on a mesh through the SPDE (Lindgren, Rue and Lindström 2011 [V]), which joins point-referenced and areal data in one model; BYM2 in Stan (Morris et al. 2019 [V]); disease mapping and spatial regression with counts, a review (Wakefield 2007 [V]).
- **With survey data:** smoothing of design-based direct estimates with their variances in space and time, a Fay–Herriot model with BYM2 effects (Mercer et al. 2015 [V]; Wakefield, Okonek and Pedersen 2020 [V], review; R `SUMMER`).
- **Failures:** oversmoothing across sharp urban boundaries (an enclave of wealth beside deprivation is pulled together by adjacency); islands and disconnected graphs need per-component scaling; spatial random effects confounded with covariates change the fixed effects (Hodges and Reich 2010 [V]); MAUP (§7.5).

#### 6.2 Geostatistics and Gaussian Processes

Model-based geostatistics (Diggle, Tawn and Moyeed 1998 [V]; Cressie 1993 [V]); scaling to large data by predictive processes (Banerjee, Gelfand, Finley and Sang 2008 [V]) and nearest-neighbour GPs (Datta, Banerjee, Finley and Gelfand 2016 [V]); space–time (Wikle, Zammit-Mangion and Cressie 2019 [V]); Gaussian processes in general (Rasmussen and Williams 2006 [M]). Guarantee: the posterior predictive variance is right *under the model*; calibration in practice depends on stationarity and the covariance family, which city data (with sharp edges and anisotropy along streets) often violate.

#### 6.3 Machine Learning and Spatial Cross-Validation

- **Learners:** random forests (Breiman 2001 [V]); gradient boosting (Chen and Guestrin 2016 [V], XGBoost); random forests with distances as covariates for spatial prediction (Hengl et al. 2018 [V]); quantile regression forests for intervals.
- **Why random cross-validation misleads.** With spatially dependent data, random folds put near-duplicates of each test point in training and estimate interpolation skill, not the skill of predicting where there are no data. Roberts et al. (2017) [V] set out blocked cross-validation for structured data. Ploton et al. (2020) [V]: a forest-biomass map whose random cross-validation suggested useful skill showed almost none under spatial validation. Meyer, Reudenbach, Hengl, Katurji and Nauss (2018) [V]: target-oriented validation and feature selection, since spatially autocorrelated covariates (coordinates, elevation) let a model memorise places.
- **The counter-argument.** Wadoux, Heuvelink, de Bruin and Brus (2021) [V]: when the validation sample is a *probability sample* of the map area, design-based estimation of map accuracy (random cross-validation, or an independent probability sample) is unbiased, and spatial cross-validation is pessimistic; with clustered samples, weighting or model-based estimation of map accuracy (de Bruin et al. 2022 [V]). The resolution: the validation scheme must imitate the *prediction task*.
- **Matching the prediction task:** nearest-neighbour distance matching, in which folds are built so that the distribution of distances from test to training points matches that from prediction locations to training points (Milà, Mateu, Pebesma and Meyer 2022 [V], leave-one-out; Linnenbrink, Milà, Ludwig and Meyer 2024 [V], k-fold). Clustered samples and global maps (Meyer and Pebesma 2022 [V]).
- **References:** R `CAST` (area of applicability, NNDM, kNNDM), `blockCV` (Valavi et al. 2019 [V]), `spatialsample`; Python scikit-learn group folds.

#### 6.4 The Area of Applicability

- **Definition** (Meyer and Pebesma 2021 [V]). For a new location, the *dissimilarity index* $\mathrm{DI}$ is its minimum distance to the training points in the space of standardised predictors weighted by variable importance, divided by the mean pairwise distance among training points. The threshold is the outlier-removed maximum of the cross-validated DI of the training points (each training point's distance to the training points outside its own fold). The *area of applicability* (AOA) is where $\mathrm{DI}\le$ threshold; outside it, predictions should not be trusted. The DI can also be related to cross-validated error to map expected error, and a later *local point density* counts how many training points support a prediction (Schumacher, Knoth, Ludwig and Meyer 2025 [V]).
- **What it does not do.** It detects covariate novelty, not concept shift (the same covariates, a different relation, as when a model trained in one city is used in another with different housing markets); its threshold is a heuristic without a coverage guarantee; it depends on the importance weights and on Euclidean distance in a high-dimensional space; it requires that the CV folds themselves reflect the prediction task. Large fractions of global ecological maps lie outside their AOA (Ludwig et al. 2023 [V]).
- **For cities:** a district of tower blocks scored by a model trained on detached suburbs, or an informal settlement scored by a model trained on formal housing, falls outside the AOA; that is the mechanical form of "extrapolation outside the covariate space".

#### 6.5 Conformal Prediction

- **Split conformal** (Vovk, Gammerman and Shafer 2005 [V]; Lei, G'Sell, Rinaldo, Tibshirani and Wasserman 2018 [V]; introduction: Angelopoulos and Bates 2023 [V]): with calibration residuals exchangeable with the test point, the interval $\hat f(x)\pm q_{\lceil(1-\alpha)(n+1)\rceil}$ covers with probability between $1-\alpha$ and $1-\alpha+1/(n+1)$, a finite-sample guarantee a simulation can check exactly. Adaptive widths by conformalised quantile regression (Romano, Patterson and Candès 2019 [V], arXiv:1905.03222).
- **Beyond exchangeability:** weighted conformal under known covariate shift (Tibshirani, Barber, Candès and Ramdas 2019 [V], arXiv:1904.06019); weighted quantiles with a coverage gap bounded by the total-variation distance between calibration and test (Barber, Candès, Ramdas and Tibshirani 2023 [V]); localised conformal (Guan 2023 [V]); adaptive conformal for drifting series (Gibbs and Candès 2021 [V], arXiv:2106.00170).
- **Spatial conformal:** valid model-free spatial prediction by local conformal under infill asymptotics (Mao, Martin and Reich 2024 [V]); localised spatial conformal through quantile regression under stationarity and spatial mixing (Jiang and Xie 2024 [V], arXiv:2412.01098); geographically weighted conformal (Lou, Luo and Meng, arXiv:2412.08661 [V], reported as published in the *Annals of the AAG* 2025 [S]).
- **Caveats:** coverage is marginal over the calibration distribution, not conditional at each location; spatial dependence breaks exchangeability; if the calibration points do not represent the prediction locations, the interval is calibrated for the wrong population. Conformal intervals therefore belong *inside* the AOA, and outside it the right output is a refusal. References: Python `MAPIE`, `crepes`.

#### 6.6 Consequences for the Design

- **Index served:** rates and counts on areas (BYM2/Leroux, with a Fay–Herriot sampling layer when the data are survey estimates); continuous surfaces (heat, noise, canopy, prices) by GPs; any index with many covariates by learners.
- **Level:** blocks and neighbourhoods; the SPDE joins point observations (sensors) to areal ones (censuses).
- **Coupling:** the GMRF is a natural prior for the spatial state of a dynamical model at one level; its precision matrix is the graph Laplacian already used by balance laws on graphs of cells.
- **Gatekeeping:** every learned estimator should output, per location, the prediction, a conformal interval, the DI and an in/out-of-AOA flag; the studio refuses outside the AOA.

### 7. Rules for Refusal

#### 7.1 The Grounds

An estimate should be withheld, and the quantity declared with a range by the person, when any of the following holds; each ground has a test the software can run.

| Ground | Test | Source of the rule |
|---|---|---|
| Precision below fitness for use | CV, RSE, interval width or effective sample size beyond an office's threshold (§7.2) | statistical offices |
| Extrapolation in covariate space | outside the AOA (§6.4); leverage beyond the training hull; an out-of-sample area under a model that cannot be checked there | Meyer and Pebesma 2021 [V] |
| Proxy validity unestablished | no validation of this proxy for this index at this scale in a comparable setting (§3) | §3.3–3.4 critiques |
| Cross-level inference | an individual-level quantity asked of area aggregates with no individual model | Robinson 1950 [V] |
| Unit dependence | an index of association or a scaling exponent whose value changes with the zoning | MAUP (§7.5) |
| Constraint failure | calibration or benchmarking does not converge within bounds | Tanton et al. 2011 [V] |
| Model inadequacy | the model-based estimates disagree with direct estimates beyond their joint error | Tzavidis et al. 2018 [V] |

#### 7.2 Thresholds That Statistical Offices Actually Use

- **Australian Bureau of Statistics.** Estimates with relative standard error between 25% and 50% are marked (an asterisk) and "should be used with caution"; above 50% (a double asterisk) they are "considered too unreliable for general use" and kept only for aggregation [S, archived ABS technical notes; the exact boundary wording varies between releases].
- **Statistics Canada.** A letter grade by CV: A excellent (0–4.9%), then B, C, D, E "use with caution" (25.0–34.9%), F "too unreliable to be published" (above 35%) [S, archived Statistics Canada tables; the cut-points of B–D are not confirmed here].
- **Office for National Statistics (UK), Labour Force and Annual Population Surveys** (methodology revised 1 May 2020) [V]: the highest allowed CV is 20%; the minimum unweighted sample size is $n_{\min}=\mathrm{deff}/\mathrm{CV}^2$, giving 25 for cross-sectional LFS and APS (deff 1) and 27 for the longitudinal LFS (deff about 1.1); counts below 3 are suppressed for disclosure; estimates are published from a count of 3 and shaded where the count is above 3 and at most 25 (27 longitudinal); answers to parliamentary questions suppress any estimate with CV above 20%.
- **US Census Bureau, American Community Survey.** One-year products are released only for geographies and sub-populations of 65,000 or more (retained in the 2025 release rules, which removed several unweighted-count rules and noise-based disclosure protection while keeping "all legacy rules related to data quality") [V, user note 2026-06]. A detailed table is filtered (not released for a geography) when the median CV of its detailed cells exceeds 0.61, with zero estimates counted as CV 1 [S, Census working paper of 2011]. The Bureau publishes 90% margins of error and sets no user-facing CV threshold; users' rules such as "CV under 12% reliable, 12–40% use with caution, over 40% unreliable" are conventions of practice [M].
- **US National Center for Health Statistics, proportions** (Data Presentation Standards for Proportions, Vital and Health Statistics Series 2, No. 175, 2017 [V]): suppress when the actual or effective sample size is below 30; suppress when the absolute width of the Korn–Graubard 95% interval is 30 percentage points or more; when it lies between 5 and 30 points, suppress if the relative width exceeds 130%; widths of 5 points or less may be presented; degrees of freedom below 8 require review; for means, RSE of 30% or more is flagged unreliable [V, NHANES tutorial page of NCHS].
- **Eurostat, EU-SILC.** An estimate is not published when based on fewer than 20 sample observations or when item non-response exceeds 50%; it is published with a flag when based on 20–49 observations or when non-response is between 20% and 50% [S, Eurostat guidelines as cited].
- **Reading across offices.** The thresholds cluster: a CV up to about 20–25% for unqualified publication, 25–35% for "use with caution", and refusal above 35–50%; and a minimum of 20–30 (effective) observations. NCHS's rule is the most principled for proportions, because a CV is meaningless near 0 or 1 and interval width is not.

#### 7.3 Precision Rules Applied to Model-Based Estimates

The office thresholds were written for direct survey estimates. For model-based estimates the CV computed from a model-based MSE is conditional on the model; two consequences follow. First, a precision threshold is necessary but not sufficient: the model's adequacy (§2.5) must also pass. Second, out-of-sample areas, whose MSE contains no sampling term, can look precise while being pure extrapolation; they must also pass the covariate-space test. Coarsening is an alternative to refusal: merge units until the estimate meets the threshold (Spielman and Folch 2015 [V], data-driven regionalisation of ACS tracts). Planners routinely ignore margins of error (Jurjevich et al. 2018 [V]), and ACS quality varies systematically across places (Spielman, Folch and Nagle 2014 [V]; Folch, Arribas-Bel, Koschinsky and Spielman 2016 [V]), which is the argument for making refusal mechanical rather than advisory.

#### 7.4 The Ecological Fallacy and Its Honest Alternative

Correlations between area aggregates do not estimate individual-level relations (Robinson 1950 [V]); grouping changes correlations (Gehlke and Biehl 1934 [V]). Ecological inference recovers individual-level tables only under assumptions (King 1997 [M]; Wakefield 2004 [V], 2×2 tables with discussion). The defensible output when no individual model exists is the *identified set*: for a 2×2 table with known margins, the Fréchet bounds on each cell (the method of bounds of Duncan and Davis 1953 [V]) are exact and assumption-free. When the studio refuses a point estimate of a cross-level quantity, it can still return these bounds as the declared range's default.

#### 7.5 The Modifiable Areal Unit Problem

Statistics computed on areal units change with the scale and the zoning of the units (Openshaw 1984 [M]; Fotheringham and Wong 1991 [V], for multivariate analysis); the context that matters for a person is uncertain (Kwan 2012 [V]). The urban-scaling literature shows the stakes: exponents of urban indicators against city size depend on how cities are delimited (Arcaute et al. 2015 [V]) and their apparent nonlinearity is often not statistically supported once fluctuations are modelled properly (Leitão, Miotto, Gerlach and Altmann 2016 [V]). Rule: indexes of association, segregation and scaling are reported with their unit system and a sensitivity analysis over zonings, or not at all.

#### 7.6 Consequences for the Design

A refusal is a typed outcome, not an error: *withheld because* (precision, extrapolation, proxy, cross-level, unit dependence, constraint failure, inadequacy), with the diagnostic value, the threshold and its source, and a *default range* for the declaration: the identified set where one exists, otherwise the parent level's value with the between-area spread ($x_i^\top\hat\beta\pm 2\hat\sigma_v$ under a Fay–Herriot model), otherwise the range across sibling units. A model that needs the index runs only when the person has declared a value and a range, and its outputs carry the declared range forward as an input uncertainty. Thresholds are a declared scheme (an office's table as data, defaulting to the ONS or NCHS rules), never constants in code; a downstream decision's value of information can set a tighter threshold.

### 8. Particular Urban Indexes and Their Estimation

#### 8.1 Rents and Prices

- **Methods.** Hedonic regression, implicit prices of characteristics (Rosen 1974 [V]); hedonic indexes surveyed and classified (Hill 2013 [V]); repeat-sales indexes from pairs of sales of the same dwelling (Bailey, Muth and Nourse 1963 [V]; the weighted repeat-sales method of Case and Shiller 1989 [M]); a repeat-rent index (Ambrose, Coulson and Yoshida 2015 [V]); the methods compared for official statistics in Eurostat's *Handbook on Residential Property Prices Indices* (2013) [M]; machine learning for house values as an applied example (Mullainathan and Spiess 2017 [V]).
- **Failure modes.** Sales and new lettings are a selected sample of the stock; asking rents in listings are not contract rents and lead stock rents; repeat-sales indexes exclude new and renovated dwellings; thin markets give few transactions per small area per period, which calls for SAE or spatial smoothing (§2, §6.1).
- **Design:** prices and rents at neighbourhood level by hedonic or repeat-sales models with spatial random effects; a block-level rent is refused where the transaction count gives a CV beyond threshold.

#### 8.2 Vacancy

- **Sources.** Census vacancy (point in time; includes seasonal, for-sale and between-tenancy dwellings); postal-service vacancy flags aggregated by tract in the US, which often run below ACS vacancy [S, HUD *Cityscape*]; metered electricity: Ireland's CSO counts a dwelling vacant if it used under 180 kWh in a quarter and in each of the three before, against a base of active connections, in an experimental (Frontier) series [V, CSO methodology page]; water-meter shutoffs [S]; smart meters in prospect.
- **Design:** vacancy is definition-dependent; an estimate must name its definition (point-in-time, long-term, structural), and the occupancy chain of the urban domain can be calibrated to a metered series, not to a census figure of a different definition.

#### 8.3 Informal Employment

Measured in labour-force surveys by the ILO's statistical definitions (the 2003 guidelines on informal employment and the 2023 resolution on the informal economy [M]); municipal estimates of labour indicators including informal employment, by SAE with a spatial component from Mexico's labour-force survey with census and administrative auxiliaries, used only where the sample was adequate [S, a 2018 study reported in search results, citation not resolved]; a nested-error model with geospatial auxiliaries improves municipal participation and unemployment estimates for Mexico (Merfeld et al. [S]); model-based labour-force estimates for sub-national areas generally use area-level or Rao–Yu models (§2.2). Below the district level the index should normally be refused.

#### 8.4 Footfall and Dwell Time

- **Sources.** Automatic counters; Wi-Fi probe requests, calibrated by manual counts, with biases from device ownership and MAC randomisation (Soundararaj, Cheshire and Longley 2020 [V]); a real-time census of a city from Wi-Fi (Kontokosta and Johnson 2017 [V]); mobile-phone data for dynamic population (Deville et al. 2014 [V]) and individual mobility (Calabrese et al. 2013 [V]); machine learning from street features trained on sensors, which over-predicted footfall after the pandemic (Asher, Oswald and Malleson 2025 [S]) and must be validated across cities before transfer.
- **Biases.** Phone ownership is unequal (Wesolowski et al. 2013 [V]); commercial mobility panels under-represent older and non-white visitors (Coston et al. 2021 [V], audited against administrative data). Dwell time depends on a stay-point definition (radius, minimum duration) that must be declared.
- **Design:** footfall between counters by models with spatial validation and AOA; panels re-weighted to known populations (raking) before use; refusal where no counter is within the AOA.

#### 8.5 Visitor Spending and Retail Sales

Card-transaction data aggregated by merchant location give spending at fine spatial and temporal resolution (Chetty et al. 2024 [V], the public database of the pandemic), with coverage depending on the issuer's market share and with the merchant's location standing for where money was spent, not who spent it; retail sales below the region come from sales-tax or value-added-tax records where released, from card data, or by allocation of regional totals on retail floor space (a dasymetric allocation, §1, declared as such).

#### 8.6 Municipal Revenue

Revenue is observed for the municipality in its accounts; comparisons across cities need standardisation for overlapping governments, as in fiscally standardised cities (Chernick, Langley and Reschovsky 2011 [V]). Sub-municipal revenue is an accounting allocation (property tax by assessed value, fees by service area), not an estimate, and should be declared as an allocation rule.

#### 8.7 Tree Canopy

Photo-interpretation of random points gives canopy share with binomial standard error $\sqrt{p(1-p)/n}$; about 1,000 points give roughly 1.4 points of standard error at 30% cover [S, USDA Forest Service guide and i-Tree reports; the formula reproduces their published figures]; US city canopy and impervious cover change by this method (Nowak and Greenfield 2012 [V]). Classification of aerial imagery and LiDAR gives wall-to-wall maps; global canopy-height models from Sentinel-2 and GEDI (Lang, Jetz, Schindler and Wegner 2023 [V]) and from very-high-resolution imagery trained on aerial LiDAR (Tolan et al. 2024 [V]) need local validation in cities, where street trees are small relative to pixels.

#### 8.8 Heat: UTCI and Mean Radiant Temperature

The Universal Thermal Climate Index (Jendritzky, de Dear and Havenith 2012 [V]; operational procedure by Bröde et al. 2012 [V]) needs air temperature, humidity, wind speed at 10 m and the mean radiant temperature $T_{mrt}$, the last dominant in sun. $T_{mrt}$ is modelled from 3D radiant fluxes in street canyons (SOLWEIG, Lindberg, Holmer and Thorsson 2008 [V]; ENVI-met) or measured by globe thermometers and six-directional radiometers (Thorsson et al. 2007 [V]). The operational polynomial is valid only within stated input ranges [M on the exact limits], which gives a natural refusal domain. Satellite land-surface temperature is not air temperature: crowdsourced air temperatures give urban heat islands of different magnitude and drivers from satellite ones (Venter, Chakraborty and Lee 2021 [V]). Local Climate Zones (Stewart and Oke 2012 [V]) are the usual strata for interpolating station temperatures. Reference: `pythermalcomfort` (Tartarini and Schiavon 2020 [V]) implements UTCI.

#### 8.9 Accessibility

Accessibility is computed, not estimated (Hansen 1959 [V]; Geurs and van Wee 2004 [V], review; the two-step floating catchment, Luo and Wang 2003 [V]); its uncertainty comes from the schedule versus realised service (Wessel, Allen and Farber 2017 [V], timetables reconstructed from vehicle locations), from departure-time variation (routing over a window, as in `r5r`, Pereira et al. 2021 [V]), from incomplete opportunity data, and from MAUP. An accessibility value should carry the range over departure times and the completeness of its opportunity layer.

#### 8.10 Social Vulnerability

The Social Vulnerability Index from principal components of census variables (Cutter, Boruff and Shirley 2003 [V]) and the CDC SVI from percentile ranks of ACS variables (Flanagan et al. 2011 [V]). Rankings are sensitive to construction choices (Tate 2012 [V], uncertainty and sensitivity analysis); the SoVI fails tests of internal consistency and construct validity (Spielman et al. 2020 [V]); composite indicators in general should be published with their uncertainty and sensitivity analysis (Saisana, Saltelli and Tarantola 2005 [V]); ACS margins of error propagate into every component. An index of this kind is a measurement model, belonging to the design's measurement module, and its value at a unit should carry the interval from both sampling and construction.

### 9. The Contract of an Estimator Implied by the Literature

Every estimator in §§1–8 can be stated in one form, which the studio can hold estimators to:

1. **Declared target:** the index, its definition, its support (unit and time), and whether it is extensive, intensive (with numerator and denominator) or distributional.
2. **Declared data and assumptions:** source supports, ancillary or proxy variables with their validation records, the model and its scale.
3. **Invariants:** volume preservation in space (§1), the aggregation constraint in time (§5), benchmarks to published totals (§2.4), calibration equations (§4). Each is an exact identity a check can hold after every step.
4. **Output:** the estimate, its MSE or interval with its kind (model-based, design-based, conformal, posterior), and its diagnostics: DI and AOA flag, CV, effective sample size, comparison with direct estimates.
5. **Validation record:** the scheme (re-aggregation, spatial or NNDM cross-validation, design-based simulation), its statistic, and the setting it was run in.
6. **Refusal:** a typed outcome with the failed ground, the value, the threshold and its source, and the default range for a declaration (§7.6).

This contract makes the hierarchy coherent: aggregation upward and allocation downward are the invariants of (3); the fast–slow coupling in time is the aggregation constraint; and a dynamical model's parameters inherit, as declared ranges, whatever the estimators could not supply.

### 10. Recommendations

Ranked by value to the design (estimate where possible, refuse where not, couple levels by invariants). Each line says where the method would go and what its checks could hold it to.

1. **A refusal-and-declaration outcome with office thresholds as declared schemes** (§7). *Where:* the measurement module (the typed outcome, the threshold schemes as data) and the studio (the declaration form with its default range). *Checks:* the ONS rule $n_{\min}=\mathrm{deff}/\mathrm{CV}^2$ reproduces 25 and 27; the NCHS proportion rules reproduce the published decision table; Fréchet bounds on 2×2 tables by closed form; each refusal ground triggered by a constructed case.
2. **A volume-preserving change-of-support kernel**: areal weighting, multi-class dasymetric with floor area by use as the ancillary layer (intelligent dasymetric mapping), and Tobler's pycnophylactic smoothing as the uninformed baseline (§1). *Where:* the urban domain (ancillary relations of buildings) on a general allocation routine in the statistics or measurement module. *Checks:* volume preservation to rounding for every source zone; exact recovery of a density that is piecewise constant on classes; non-negativity; agreement with PySAL `tobler` and R `pycno` on shared inputs; re-aggregation error on a known fine level.
3. **Fay–Herriot extensions**: spatial (SAR), spatio-temporal (Rao–Yu; Marhuenda–Molina–Morales), multivariate, covariates with error (Ybarra–Lohr), with Prasad–Rao and parametric-bootstrap MSE, and benchmarking (§2.2, §2.4). *Where:* the designs module beside the existing Fay–Herriot EBLUP. *Checks:* shrinkage factors and $g_1$ in closed form; R `sae` (`eblupFH`, `eblupSFH`, `eblupSTFH`, `mseFH`) on its bundled example data (Molina and Marhuenda 2015 [V]); bootstrap MSE converging to Prasad–Rao under the model in simulation.
4. **Unit-level empirical best for nonlinear indexes** (Molina–Rao EB under a transformed nested-error model, with parametric-bootstrap MSE; §2.3). *Where:* the designs module, feeding poverty, Gini and affordability by area. *Checks:* `emdi`'s EBP on its synthetic EU-SILC-like population and sample (Kreutzmann et al. 2019 [V]); design-based simulation from a synthetic census showing bias near zero and MSE coverage; EBLUP of a linear mean reproducing the BHF closed form.
5. **The area of applicability and prediction-task-matched cross-validation** (DI with its threshold; NNDM and kNNDM folds; §6.3–6.4). *Where:* the statistics module, applied to every learned estimator. *Checks:* DI computed by hand on a small table; R `CAST` (`aoa`, `nndm`, `knndm`) on its examples; a simulated covariate shift flagged exactly where constructed.
6. **Spatial microsimulation**: IPF with truncate-replicate-sample integerisation, GREGWT calibration with bounds, combinatorial optimisation, with resampling uncertainty (§4). *Where:* the social-science module beside the existing static microsimulation; its output is the micro-state of the agent models. *Checks:* IPF's margins met to tolerance and its fixed point equal to the minimum-KL table on small cases solved by convex optimisation; odds ratios of the seed preserved; calibration equations satisfied exactly; `mipfp` and `humanleague` reference outputs; non-convergent areas reported, as in Tanton et al. (2011) [V].
7. **BYM2 and Leroux GMRF models, with a Fay–Herriot sampling layer for survey estimates** (§6.1). *Where:* the statistics module (sparse precision, Laplace approximation or HMC, which the module has). *Checks:* the BYM2 scaling factor (geometric mean of ICAR marginal variances equal to one) on small graphs; R-INLA, `SUMMER` or the Stan implementation of Morris et al. (2019) [V] on the Scottish lip-cancer data, whose published posteriors are widely reproduced.
8. **Temporal disaggregation and benchmarking** (Chow–Lin, Fernández, Litterman, Denton; §5.2). *Where:* the dynamical-systems foundation as the coupling between time scales, in its state-space form (Proietti 2006 [V]). *Checks:* the aggregation constraint exactly; Denton's solution as the closed-form quadratic programme; R `tempdisagg` on its documented examples (Sax and Steiner 2013 [V]).
9. **State-space filtering with mixed frequency and benchmark constraints** (Mariano–Murasawa aggregation in the observation equation; Pfeffermann–Tiller; Bańbura–Modugno EM; §5.3). *Where:* the dynamical-systems foundation (declared law plus observation operator) with the statistics module's filters. *Checks:* the Kalman filter against the closed-form local-level model; `statsmodels`' mixed-frequency dynamic factor model on shared data; benchmarks met exactly.
10. **Conformal intervals restricted to the AOA** (split, conformalised quantile regression, localised and geographically weighted variants; §6.5). *Where:* the statistics module, wrapping any learner. *Checks:* finite-sample coverage between $1-\alpha$ and $1-\alpha+1/(n+1)$ verified by simulation under exchangeability; under-coverage reproduced under constructed spatial shift; `MAPIE` reference intervals.
11. **The housing-unit method and cohort-component estimation with the error of closure** (§5.4). *Where:* the urban domain (buildings and the occupancy chain) and demography. *Checks:* accounting identities exactly; an intercensal series that meets both censuses.
12. **A proxy registry with validation records**, and proxies used as small-area covariates rather than standalone predictors (§3). *Where:* the studio (registry, gatekeeping) and the designs module (Fay–Herriot with measurement-error correction). *Checks:* a registry entry without a validation at the target scale blocks use; Ybarra–Lohr's corrected estimator against simulation with noisy covariates.
13. **Area-to-point (Poisson) kriging and disaggregation regression** for rates and counts known on areas (§1.2). *Where:* the statistics module. *Checks:* the coherence property (averages of point predictions over a zone return its datum); R `disaggregation` (Nandi et al. 2023 [V]) on its examples.
14. **SPREE and generalised SPREE** to carry cross-classified small-area tables forward under new margins (§2.4). *Where:* the designs module, as the downward coupling of tables between censuses. *Checks:* SPREE equals IPF on the census table; association structure preserved; margins met.
15. **Hedonic and repeat-sales indexes with spatial random effects for rents and prices** (§8.1). *Where:* the urban domain (and the markets domain). *Checks:* the Bailey–Muth–Nourse estimator against a constructed panel with known index; repeat-rent index identities; refusal where transaction counts fail the precision threshold.
16. **UTCI with modelled mean radiant temperature, within the operational procedure's validity domain** (§8.8). *Where:* the architecture and urban domains' environment modules. *Checks:* `pythermalcomfort` values (Tartarini and Schiavon 2020 [V]); the published operational procedure's test values; refusal outside the polynomial's input ranges.
17. **Uncertainty and sensitivity of composite vulnerability indexes** (§8.10). *Where:* the measurement module (composite indicators exist; Sobol' indices exist in the statistics layer). *Checks:* the Saisana–Saltelli–Tarantola procedure on a small index; rank intervals under construction choices; propagation of ACS margins of error.
18. **Bottom-up population with posterior intervals where census totals are absent** (Leasure et al. 2020 [V]; §3.2). *Where:* the urban domain on the statistics module's hierarchical Bayes. *Checks:* the aggregate-then-predict-finer protocol; posterior predictive coverage on held-out enumeration areas.

### 11. Bibliography

- Aiken, E., Bellue, S., Karlan, D., Udry, C., Blumenstock, J. E. (2022). Machine learning and phone data can improve targeting of humanitarian aid. *Nature* 603(7903), 864–870. https://doi.org/10.1038/s41586-022-04484-9 **[V]**
- Aiken, E., Rolf, E., Blumenstock, J. (2023). Fairness and representation in satellite-based poverty maps: evidence of urban–rural disparities and their impacts on downstream policy. IJCAI 2023, AI for Social Good. arXiv:2305.01783 **[V]**
- Ambrose, B. W., Coulson, N. E., Yoshida, J. (2015). The repeat rent index. *Review of Economics and Statistics* 97(5), 939–950. https://doi.org/10.1162/REST_a_00500 **[V]**
- Angelopoulos, A. N., Bates, S. (2023). Conformal prediction: a gentle introduction. *Foundations and Trends in Machine Learning* 16(4), 494–591. https://doi.org/10.1561/2200000101 **[V]**
- Arcaute, E., Hatna, E., Ferguson, P., Youn, H., Johansson, A., Batty, M. (2015). Constructing cities, deconstructing scaling laws. *Journal of the Royal Society Interface* 12(102), 20140745. https://doi.org/10.1098/rsif.2014.0745 **[V]**
- Asher, M., Oswald, Y., Malleson, N. (2025). Understanding pedestrian dynamics using machine learning with real-time urban sensors. https://eprints.whiterose.ac.uk/id/eprint/221809/ **[S]**
- Australian Bureau of Statistics. Technical notes on data quality (archived releases, e.g. Household Expenditure Survey User Guide 2003–04). https://www.abs.gov.au/AUSSTATS/ABS@.NSF/Latestproducts/019D9A78E6E59900CA25718C001AB9B3 **[S]**
- Australian Bureau of Statistics. A Rao–Yu model for small area estimation of labour force statistics. https://www.abs.gov.au/statistics/research/rao-yu-model-small-area-estimation-labour-force-statistics **[S]**
- Bailey, M. J., Muth, R. F., Nourse, H. O. (1963). A regression method for real estate price index construction. *JASA* 58(304), 933–942. https://doi.org/10.1080/01621459.1963.10480679 **[V]**
- Bańbura, M., Giannone, D., Modugno, M., Reichlin, L. (2013). Now-casting and the real-time data flow. *Handbook of Economic Forecasting*, 195–237. https://doi.org/10.1016/B978-0-444-53683-9.00004-9 **[V]**
- Bańbura, M., Modugno, M. (2014). Maximum likelihood estimation of factor models on datasets with arbitrary pattern of missing data. *Journal of Applied Econometrics* 29(1), 133–160. https://doi.org/10.1002/jae.2306 **[V]**
- Banerjee, S., Gelfand, A. E., Finley, A. O., Sang, H. (2008). Gaussian predictive process models for large spatial data sets. *JRSS B* 70(4), 825–848. https://doi.org/10.1111/j.1467-9868.2008.00663.x **[V]**
- Barber, R. F., Candès, E. J., Ramdas, A., Tibshirani, R. J. (2023). Conformal prediction beyond exchangeability. *Annals of Statistics* 51(2), 816–845. https://doi.org/10.1214/23-AOS2276 **[V]**
- Battese, G. E., Harter, R. M., Fuller, W. A. (1988). An error-components model for prediction of county crop areas using survey and satellite data. *JASA* 83(401), 28–36. https://doi.org/10.1080/01621459.1988.10478561 **[V]**
- Beckman, R. J., Baggerly, K. A., McKay, M. D. (1996). Creating synthetic baseline populations. *Transportation Research Part A* 30(6), 415–429. https://doi.org/10.1016/0965-8564(96)00004-3 **[V]**
- Bell, W. R., Basel, W. W., Maples, J. J. (2016). An overview of the U.S. Census Bureau's Small Area Income and Poverty Estimates program. In *Analysis of Poverty Data by Small Area Estimation*, Wiley, 349–378. https://doi.org/10.1002/9781118814963.ch19 **[V]**
- Benavent, R., Morales, D. (2016). Multivariate Fay–Herriot models for small area estimation. *Computational Statistics & Data Analysis* 94, 372–390. https://doi.org/10.1016/j.csda.2015.07.013 **[V]**
- Besag, J., York, J., Mollié, A. (1991). Bayesian image restoration, with two applications in spatial statistics. *Annals of the Institute of Statistical Mathematics* 43(1), 1–20. https://doi.org/10.1007/BF00116466 **[V]**
- Biljecki, F., Arroyo Ohori, K., Ledoux, H., Peters, R., Stoter, J. (2016). Population estimation using a 3D city model: a multi-scale country-wide study in the Netherlands. *PLOS ONE* 11(6), e0156808. https://doi.org/10.1371/journal.pone.0156808 **[V]**
- Bluhm, R., Krause, M. (2022). Top lights: bright cities and their contribution to economic development. *Journal of Development Economics* 157, 102880. https://doi.org/10.1016/j.jdeveco.2022.102880 **[V]**
- Blumenstock, J., Cadamuro, G., On, R. (2015). Predicting poverty and wealth from mobile phone metadata. *Science* 350(6264), 1073–1076. https://doi.org/10.1126/science.aac4420 **[V]**
- Borysov, S. S., Rich, J., Pereira, F. C. (2019). How to generate micro-agents? A deep generative modeling approach to population synthesis. *Transportation Research Part C* 106, 73–97. https://doi.org/10.1016/j.trc.2019.07.006 **[V]**
- Bradley, J. R., Wikle, C. K., Holan, S. H. (2016). Bayesian spatial change of support for count-valued survey data with application to the American Community Survey. *JASA* 111(514), 472–487. https://doi.org/10.1080/01621459.2015.1117471 **[V]**
- Breiman, L. (2001). Random forests. *Machine Learning* 45(1), 5–32. https://doi.org/10.1023/A:1010933404324 **[V]**
- Bröde, P., Fiala, D., Błażejczyk, K., Holmér, I., Jendritzky, G., Kampmann, B., et al. (2012). Deriving the operational procedure for the Universal Thermal Climate Index (UTCI). *International Journal of Biometeorology* 56(3), 481–494. https://doi.org/10.1007/s00484-011-0454-1 **[V]**
- Burke, M., Driscoll, A., Lobell, D. B., Ermon, S. (2021). Using satellite imagery to understand and promote sustainable development. *Science* 371(6535), eabe8628. https://doi.org/10.1126/science.abe8628 **[V]**
- Calabrese, F., Diao, M., Di Lorenzo, G., Ferreira, J., Ratti, C. (2013). Understanding individual mobility patterns from urban sensing data: a mobile phone trace example. *Transportation Research Part C* 26, 301–313. https://doi.org/10.1016/j.trc.2012.09.009 **[V]**
- Case, K. E., Shiller, R. J. (1989). The efficiency of the market for single-family homes. *American Economic Review* 79(1), 125–137. **[M]**
- Chambers, R., Tzavidis, N. (2006). M-quantile models for small area estimation. *Biometrika* 93(2), 255–268. https://doi.org/10.1093/biomet/93.2.255 **[V]**
- Chen, T., Guestrin, C. (2016). XGBoost: a scalable tree boosting system. *Proceedings of KDD 2016*, 785–794. https://doi.org/10.1145/2939672.2939785 **[V]**
- Chen, X., Nordhaus, W. D. (2011). Using luminosity data as a proxy for economic statistics. *PNAS* 108(21), 8589–8594. https://doi.org/10.1073/pnas.1017031108 **[V]**
- Chernick, H., Langley, A., Reschovsky, A. (2011). The impact of the Great Recession and the housing crisis on the financing of America's largest cities. *Regional Science and Urban Economics* 41(4), 372–381. https://doi.org/10.1016/j.regsciurbeco.2011.04.002 **[V]**
- Chetty, R., Friedman, J. N., Stepner, M., Opportunity Insights Team (2024). The economic impacts of COVID-19: evidence from a new public database built using private sector data. *Quarterly Journal of Economics* 139(2), 829–889. https://doi.org/10.1093/qje/qjad048 **[V]**
- Chi, G., Fang, H., Chatterjee, S., Blumenstock, J. E. (2022). Microestimates of wealth for all low- and middle-income countries. *PNAS* 119(3), e2113658119. https://doi.org/10.1073/pnas.2113658119 **[V]**
- Choi, H., Varian, H. (2012). Predicting the present with Google Trends. *Economic Record* 88(s1), 2–9. https://doi.org/10.1111/j.1475-4932.2012.00809.x **[V]**
- Chow, G. C., Lin, A. (1971). Best linear unbiased interpolation, distribution, and extrapolation of time series by related series. *Review of Economics and Statistics* 53(4), 372. https://doi.org/10.2307/1928739 **[V]**
- Comber, A., Zeng, W. (2019). Spatial interpolation using areal features: a review of methods and opportunities using new forms of data with coded illustrations. *Geography Compass* 13(10), e12465. https://doi.org/10.1111/gec3.12465 **[V]**
- Corral, P., Molina, I., Nguyen, M. (2021). Pull your small area estimates up by the bootstraps. *Journal of Statistical Computation and Simulation* 91(16), 3304–3357. https://doi.org/10.1080/00949655.2021.1926460 **[V]** (Crossref lists the first author as Corral Rodas)
- Corral, P., Molina, I., Cojocaru, A., Segovia, S. (2022). *Guidelines to Small Area Estimation for Poverty Mapping*. World Bank. https://unstats.un.org/wiki/download/attachments/132284431/SAE_Guidelines.pdf **[S]**
- Coston, A., Guha, N., Ouyang, D., Lu, L., Chouldechova, A., Ho, D. E. (2021). Leveraging administrative data for bias audits: assessing disparate coverage with mobility data for COVID-19 policy. *FAccT 2021*, 173–184. https://doi.org/10.1145/3442188.3445881 **[V]**
- Cressie, N. (1993). *Statistics for Spatial Data*. Wiley. https://doi.org/10.1002/9781119115151 **[V]**
- Central Statistics Office, Ireland. Residential vacancy based on metered electricity consumption 2024: background notes and methodology. https://www-cloud.cso.ie/en/releasesandpublications/fp/fp-rvmec/residentialvacancybasedonmeteredelectricityconsumption2024/backgroundnotesandmethodology/ **[V]**
- Cutter, S. L., Boruff, B. J., Shirley, W. L. (2003). Social vulnerability to environmental hazards. *Social Science Quarterly* 84(2), 242–261. https://doi.org/10.1111/1540-6237.8402002 **[V]**
- Das, S., Chambers, R. (2017). Robust mean-squared error estimation for poverty estimates based on the method of Elbers, Lanjouw and Lanjouw. *JRSS A* 180(4), 1137–1161. https://doi.org/10.1111/rssa.12311 **[V]**
- Datta, A., Banerjee, S., Finley, A. O., Gelfand, A. E. (2016). Hierarchical nearest-neighbor Gaussian process models for large geostatistical datasets. *JASA* 111(514), 800–812. https://doi.org/10.1080/01621459.2015.1044091 **[V]**
- Datta, G. S., Ghosh, M., Steorts, R., Maples, J. (2011). Bayesian benchmarking with applications to small area estimation. *TEST* 20(3), 574–588. https://doi.org/10.1007/s11749-010-0218-y **[V]**
- Datta, G. S., Rao, J. N. K., Smith, D. D. (2005). On measuring the variability of small area estimators under a basic area level model. *Biometrika* 92(1), 183–196. https://doi.org/10.1093/biomet/92.1.183 **[V]**
- de Bruin, S., Brus, D. J., Heuvelink, G. B. M., van Ebbenhorst Tengbergen, T., Wadoux, A. M. J.-C. (2022). Dealing with clustered samples for assessing map accuracy by cross-validation. *Ecological Informatics* 69, 101665. https://doi.org/10.1016/j.ecoinf.2022.101665 **[V]**
- Deming, W. E., Stephan, F. F. (1940). On a least squares adjustment of a sampled frequency table when the expected marginal totals are known. *Annals of Mathematical Statistics* 11(4), 427–444. https://doi.org/10.1214/aoms/1177731829 **[V]**
- Denton, F. T. (1971). Adjustment of monthly or quarterly series to annual totals: an approach based on quadratic minimization. *JASA* 66(333), 99–102. https://doi.org/10.1080/01621459.1971.10482227 **[V]**
- Deville, J.-C., Särndal, C.-E. (1992). Calibration estimators in survey sampling. *JASA* 87(418), 376–382. https://doi.org/10.1080/01621459.1992.10475217 **[V]**
- Deville, P., Linard, C., Martin, S., Gilbert, M., Stevens, F. R., Gaughan, A. E., et al. (2014). Dynamic population mapping using mobile phone data. *PNAS* 111(45), 15888–15893. https://doi.org/10.1073/pnas.1408439111 **[V]**
- DHS Program. Geographic displacement of survey cluster coordinates (urban ≤ 2 km, rural ≤ 5 km, 1% of rural ≤ 10 km), as stated on the DHS user forum and in protocols copying it. https://userforum.dhsprogram.com/pdf.php?th=19& **[S]**
- Diggle, P. J., Tawn, J. A., Moyeed, R. A. (1998). Model-based geostatistics. *JRSS C* 47(3), 299–350. https://doi.org/10.1111/1467-9876.00113 **[V]**
- Do, V. H., Thomas-Agnan, C., Vanhems, A. (2015). Accuracy of areal interpolation methods for count data. *Spatial Statistics* 14, 412–438. https://doi.org/10.1016/j.spasta.2015.07.005 **[V]**
- Durbin, J., Koopman, S. J. (2012). *Time Series Analysis by State Space Methods*, 2nd ed. Oxford University Press. https://doi.org/10.1093/acprof:oso/9780199641178.001.0001 **[V]**
- Eicher, C. L., Brewer, C. A. (2001). Dasymetric mapping and areal interpolation: implementation and evaluation. *Cartography and Geographic Information Science* 28(2), 125–138. https://doi.org/10.1559/152304001782173727 **[V]**
- Elbers, C., Lanjouw, J. O., Lanjouw, P. (2003). Micro-level estimation of poverty and inequality. *Econometrica* 71(1), 355–364. https://doi.org/10.1111/1468-0262.00399 **[V]**
- Elvidge, C. D., Baugh, K., Zhizhin, M., Hsu, F. C., Ghosh, T. (2017). VIIRS night-time lights. *International Journal of Remote Sensing* 38(21), 5860–5879. https://doi.org/10.1080/01431161.2017.1342050 **[V]**
- Engstrom, R., Hersh, J., Newhouse, D. (2022). Poverty from space: using high resolution satellite imagery for estimating economic well-being. *World Bank Economic Review* 36(2), 382–412. https://doi.org/10.1093/wber/lhab015 **[V]**
- Eurostat. EU-SILC guidelines for publication (thresholds of 20 and 50 observations; item non-response of 20% and 50%). https://ec.europa.eu/eurostat/documents/203647/771732/Guidelines+for+publication **[S]**
- Eurostat (2013). *Handbook on Residential Property Prices Indices (RPPIs)*. Publications Office of the European Union. **[M]**
- Farooq, B., Bierlaire, M., Hurtubia, R., Flötteröd, G. (2013). Simulation based population synthesis. *Transportation Research Part B* 58, 243–263. https://doi.org/10.1016/j.trb.2013.09.012 **[V]**
- Fay, R. E., Herriot, R. A. (1979). Estimates of income for small places: an application of James–Stein procedures to census data. *JASA* 74(366a), 269–277. https://doi.org/10.1080/01621459.1979.10482505 **[V]**
- Fernández, R. B. (1981). A methodological note on the estimation of time series. *Review of Economics and Statistics* 63(3), 471. https://doi.org/10.2307/1924371 **[V]**
- Flanagan, B. E., Gregory, E. W., Hallisey, E. J., Heitgerd, J. L., Lewis, B. (2011). A social vulnerability index for disaster management. *Journal of Homeland Security and Emergency Management* 8(1). https://doi.org/10.2202/1547-7355.1792 **[V]**
- Flowerdew, R., Green, M., Kehris, E. (1991). Using areal interpolation methods in geographic information systems. *Papers in Regional Science* 70(3), 303–315. https://doi.org/10.1007/BF01434424 **[V]**
- Folch, D. C., Arribas-Bel, D., Koschinsky, J., Spielman, S. E. (2016). Spatial variation in the quality of American Community Survey estimates. *Demography* 53(5), 1535–1554. https://doi.org/10.1007/s13524-016-0499-1 **[V]**
- Fotheringham, A. S., Wong, D. W. S. (1991). The modifiable areal unit problem in multivariate statistical analysis. *Environment and Planning A* 23(7), 1025–1044. https://doi.org/10.1068/a231025 **[V]**
- Duncan, O. D., Davis, B. (1953). An alternative to ecological correlation. *American Sociological Review* 18(6), 665. https://doi.org/10.2307/2088122 **[V]**
- Gehlke, C. E., Biehl, K. (1934). Certain effects of grouping upon the size of the correlation coefficient in census tract material. *JASA* 29(185A), 169–170. https://doi.org/10.1080/01621459.1934.10506247 **[V]**
- Gelfand, A. E., Zhu, L., Carlin, B. P. (2001). On the change of support problem for spatio-temporal data. *Biostatistics* 2(1), 31–45. https://doi.org/10.1093/biostatistics/2.1.31 **[V]** (the record lists the first author only; co-authors from memory)
- Geurs, K. T., van Wee, B. (2004). Accessibility evaluation of land-use and transport strategies: review and research directions. *Journal of Transport Geography* 12(2), 127–140. https://doi.org/10.1016/j.jtrangeo.2003.10.005 **[V]**
- Ghysels, E., Sinko, A., Valkanov, R. (2007). MIDAS regressions: further results and new directions. *Econometric Reviews* 26(1), 53–90. https://doi.org/10.1080/07474930600972467 **[V]**
- Giannone, D., Reichlin, L., Small, D. (2008). Nowcasting: the real-time informational content of macroeconomic data. *Journal of Monetary Economics* 55(4), 665–676. https://doi.org/10.1016/j.jmoneco.2008.05.010 **[V]**
- Gibbs, I., Candès, E. (2021). Adaptive conformal inference under distribution shift. arXiv:2106.00170 **[V]**
- Gibson, J., Olivia, S., Boe-Gibson, G., Li, C. (2021). Which night lights data should we use in economics, and where? *Journal of Development Economics* 149, 102602. https://doi.org/10.1016/j.jdeveco.2020.102602 **[V]**
- González-Manteiga, W., Lombardía, M. J., Molina, I., Morales, D., Santamaría, L. (2008). Bootstrap mean squared error of a small-area EBLUP. *Journal of Statistical Computation and Simulation* 78(5), 443–462. https://doi.org/10.1080/00949650601141811 **[V]**
- Goovaerts, P. (2006). Geostatistical analysis of disease data: accounting for spatial support and population density in the isopleth mapping of cancer mortality risk using area-to-point Poisson kriging. *International Journal of Health Geographics* 5, 52. https://doi.org/10.1186/1476-072X-5-52 **[V]** (DOI, author, venue and year resolved; Crossref gave no title)
- Goodchild, M. F., Lam, N. S.-N. (1980). Areal interpolation: a variant of the traditional spatial problem. *Geo-Processing* 1, 297–312. **[M]**
- Gotway, C. A., Young, L. J. (2002). Combining incompatible spatial data. *JASA* 97(458), 632–648. https://doi.org/10.1198/016214502760047140 **[V]**
- Guan, L. (2023). Localized conformal prediction: a generalized inference framework for conformal prediction. *Biometrika* 110(1), 33–50. https://doi.org/10.1093/biomet/asac040 **[V]**
- Hansen, W. G. (1959). How accessibility shapes land use. *Journal of the American Institute of Planners* 25(2), 73–76. https://doi.org/10.1080/01944365908978307 **[V]**
- Harvey, A. C. (1989). *Forecasting, Structural Time Series Models and the Kalman Filter*. Cambridge University Press. **[M]**
- Head, A., Manguin, M., Tran, N., Blumenstock, J. E. (2017). Can human development be measured with satellite imagery? *ICTD '17*, 1–11. https://doi.org/10.1145/3136560.3136576 **[V]**
- Henderson, J. V., Storeygard, A., Weil, D. N. (2012). Measuring economic growth from outer space. *American Economic Review* 102(2), 994–1028. https://doi.org/10.1257/aer.102.2.994 **[V]**
- Hengl, T., Nussbaum, M., Wright, M. N., Heuvelink, G. B. M., Gräler, B. (2018). Random forest as a generic framework for predictive modeling of spatial and spatio-temporal variables. *PeerJ* 6, e5518. https://doi.org/10.7717/peerj.5518 **[V]**
- Hill, R. J. (2013). Hedonic price indexes for residential housing: a survey, evaluation and taxonomy. *Journal of Economic Surveys* 27(5), 879–914. https://doi.org/10.1111/j.1467-6419.2012.00731.x **[V]**
- Hodges, J. S., Reich, B. J. (2010). Adding spatially-correlated errors can mess up the fixed effect you love. *The American Statistician* 64(4), 325–334. https://doi.org/10.1198/tast.2010.10052 **[V]**
- HUD, *Cityscape* 24(2), chapter 4, on postal-service vacancy data against ACS vacancy. https://www.huduser.gov/PORTAL/periodicals/cityscpe/vol24num2/ch4.pdf **[S]**
- International Labour Organization. Guidelines concerning a statistical definition of informal employment (17th ICLS, 2003) and Resolution concerning statistics on the informal economy (21st ICLS, 2023). **[M]**
- Jean, N., Burke, M., Xie, M., Davis, W. M., Lobell, D. B., Ermon, S. (2016). Combining satellite imagery and machine learning to predict poverty. *Science* 353(6301), 790–794. https://doi.org/10.1126/science.aaf7894 **[V]**
- Jendritzky, G., de Dear, R., Havenith, G. (2012). UTCI—why another thermal index? *International Journal of Biometeorology* 56(3), 421–428. https://doi.org/10.1007/s00484-011-0513-7 **[V]**
- Jiang, H., Xie, Y. (2024). Spatial conformal inference through localized quantile regression. arXiv:2412.01098 **[V]**
- Jiang, J., Lahiri, P. (2006). Mixed model prediction and small area estimation. *TEST* 15(1), 1. https://doi.org/10.1007/BF02595419 **[V]** (first page only in the record)
- Jurjevich, J. R., Griffin, A. L., Spielman, S. E., Folch, D. C., Merrick, M., Nagle, N. N. (2018). Navigating statistical uncertainty: how urban and regional planners understand and work with American Community Survey (ACS) data for guiding policy. *Journal of the American Planning Association* 84(2), 112–126. https://doi.org/10.1080/01944363.2018.1440182 **[V]**
- Kalman, R. E. (1960). A new approach to linear filtering and prediction problems. *Journal of Basic Engineering* 82(1), 35–45. https://doi.org/10.1115/1.3662552 **[V]**
- King, G. (1997). *A Solution to the Ecological Inference Problem*. Princeton University Press. **[M]**
- King, K. (2011). An investigation of data release rules for medians and zero estimates for the ACS. US Census Bureau working paper. https://www.census.gov/library/working-papers/2011/acs/2011_King_02.html **[S]**
- Kondmann, L., Zhu, X. X. (2021). Under the radar: auditing fairness in ML for humanitarian mapping. arXiv:2108.02137 **[V]**
- Kontokosta, C. E., Johnson, N. (2017). Urban phenology: toward a real-time census of the city using Wi-Fi data. *Computers, Environment and Urban Systems* 64, 144–153. https://doi.org/10.1016/j.compenvurbsys.2017.01.011 **[V]**
- Kreutzmann, A.-K., Pannier, S., Rojas-Perilla, N., Schmid, T., Templ, M., Tzavidis, N. (2019). The R package emdi for estimating and mapping regionally disaggregated indicators. *Journal of Statistical Software* 91(7). https://doi.org/10.18637/jss.v091.i07 **[V]**
- Krennmair, P., Schmid, T. (2022). Flexible domain prediction using mixed effects random forests. *JRSS C* 71(5), 1865–1894. https://doi.org/10.1111/rssc.12600 **[V]**
- Kwan, M.-P. (2012). The uncertain geographic context problem. *Annals of the Association of American Geographers* 102(5), 958–968. https://doi.org/10.1080/00045608.2012.687349 **[V]**
- Kyriakidis, P. C. (2004). A geostatistical framework for area-to-point spatial interpolation. *Geographical Analysis* 36(3), 259–289. https://doi.org/10.1111/j.1538-4632.2004.tb01135.x **[V]**
- Lang, N., Jetz, W., Schindler, K., Wegner, J. D. (2023). A high-resolution canopy height model of the Earth. *Nature Ecology & Evolution* 7(11), 1778–1789. https://doi.org/10.1038/s41559-023-02206-6 **[V]**
- Langford, M. (2007). Rapid facilitation of dasymetric-based population interpolation by means of raster pixel maps. *Computers, Environment and Urban Systems* 31(1), 19–32. https://doi.org/10.1016/j.compenvurbsys.2005.07.005 **[V]**
- Lazer, D., Kennedy, R., King, G., Vespignani, A. (2014). The parable of Google Flu: traps in big data analysis. *Science* 343(6176), 1203–1205. https://doi.org/10.1126/science.1248506 **[V]**
- Leasure, D. R., Jochem, W. C., Weber, E. M., Seaman, V., Tatem, A. J. (2020). National population mapping from sparse survey data: a hierarchical Bayesian modeling framework to account for uncertainty. *PNAS* 117(39), 24173–24179. https://doi.org/10.1073/pnas.1913050117 **[V]**
- Lee, K., Braithwaite, J. (2022). High-resolution poverty maps in Sub-Saharan Africa. *World Development* 159, 106028. https://doi.org/10.1016/j.worlddev.2022.106028 **[V]**
- Lei, J., G'Sell, M., Rinaldo, A., Tibshirani, R. J., Wasserman, L. (2018). Distribution-free predictive inference for regression. *JASA* 113(523), 1094–1111. https://doi.org/10.1080/01621459.2017.1307116 **[V]**
- Leitão, J. C., Miotto, J. M., Gerlach, M., Altmann, E. G. (2016). Is this scaling nonlinear? *Royal Society Open Science* 3(7), 150649. https://doi.org/10.1098/rsos.150649 **[V]**
- Leroux, B. G., Lei, X., Breslow, N. (2000). Estimation of disease rates in small areas: a new mixed model for spatial dependence. In *Statistical Models in Epidemiology, the Environment, and Clinical Trials* (IMA Volumes in Mathematics and its Applications), Springer, 179–191. https://doi.org/10.1007/978-1-4612-1284-3_4 **[V]**
- Leyk, S., Gaughan, A. E., Adamo, S. B., de Sherbinin, A., Balk, D., Freire, S., et al. (2019). The spatial allocation of population: a review of large-scale gridded population data products and their fitness for use. *Earth System Science Data* 11(3), 1385–1409. https://doi.org/10.5194/essd-11-1385-2019 **[V]**
- Li, X., Zhou, Y., Zhao, M., Zhao, X. (2020). A harmonized global nighttime light dataset 1992–2018. *Scientific Data* 7, 168. https://doi.org/10.1038/s41597-020-0510-y **[V]**
- Lindberg, F., Holmer, B., Thorsson, S. (2008). SOLWEIG 1.0: modelling spatial variations of 3D radiant fluxes and mean radiant temperature in complex urban settings. *International Journal of Biometeorology* 52(7), 697–713. https://doi.org/10.1007/s00484-008-0162-7 **[V]**
- Lindgren, F., Rue, H., Lindström, J. (2011). An explicit link between Gaussian fields and Gaussian Markov random fields: the stochastic partial differential equation approach. *JRSS B* 73(4), 423–498. https://doi.org/10.1111/j.1467-9868.2011.00777.x **[V]**
- Linnenbrink, J., Milà, C., Ludwig, M., Meyer, H. (2024). kNNDM CV: k-fold nearest-neighbour distance matching cross-validation for map accuracy estimation. *Geoscientific Model Development* 17(15), 5897–5912. https://doi.org/10.5194/gmd-17-5897-2024 **[V]**
- Litterman, R. B. (1983). A random walk, Markov model for the distribution of time series. *Journal of Business & Economic Statistics* 1(2), 169–173. https://doi.org/10.1080/07350015.1983.10509336 **[V]**
- Lomax, N., Norman, P. (2016). Estimating population attribute values in a table: "get me started in" iterative proportional fitting. *The Professional Geographer* 68(3), 451–461. https://doi.org/10.1080/00330124.2015.1099449 **[V]**
- Lou, X., Luo, P., Meng, L. GeoConformal prediction: a model-agnostic framework of measuring the uncertainty of spatial prediction. arXiv:2412.08661 **[V]**; reported published in *Annals of the American Association of Geographers* 115(8), 2025 **[S]**
- Lovelace, R., Ballas, D. (2013). "Truncate, replicate, sample": a method for creating integer weights for spatial microsimulation. *Computers, Environment and Urban Systems* 41, 1–11. https://doi.org/10.1016/j.compenvurbsys.2013.03.004 **[V]**
- Lovelace, R., Birkin, M., Ballas, D., van Leeuwen, E. (2015). Evaluating the performance of iterative proportional fitting for spatial microsimulation: new tests for an established technique. *JASSS* 18(2), 21. https://doi.org/10.18564/jasss.2768 **[V]**
- Lovelace, R., Dumont, M. (2016). *Spatial Microsimulation with R*. CRC Press. **[M]**
- Ludwig, M., Moreno-Martinez, A., Hölzel, N., Pebesma, E., Meyer, H. (2023). Assessing and improving the transferability of current global spatial prediction models. *Global Ecology and Biogeography* 32(3), 356–368. https://doi.org/10.1111/geb.13635 **[V]**
- Luo, W., Wang, F. (2003). Measures of spatial accessibility to health care in a GIS environment: synthesis and a case study in the Chicago region. *Environment and Planning B* 30(6), 865–884. https://doi.org/10.1068/b29120 **[V]**
- Lwin, K., Murayama, Y. (2009). A GIS approach to estimation of building population for micro-spatial analysis. *Transactions in GIS* 13(4), 401–414. https://doi.org/10.1111/j.1467-9671.2009.01171.x **[V]**
- Mao, H., Martin, R., Reich, B. J. (2024). Valid model-free spatial prediction. *JASA* 119(546), 904–914. https://doi.org/10.1080/01621459.2022.2147531 **[V]**
- Marchetti, S., Giusti, C., Pratesi, M., Salvati, N., Giannotti, F., Pedreschi, D., et al. (2015). Small area model-based estimators using big data sources. *Journal of Official Statistics* 31(2), 263–281. https://doi.org/10.1515/jos-2015-0017 **[V]**
- Marhuenda, Y., Molina, I., Morales, D. (2013). Small area estimation with spatio-temporal Fay–Herriot models. *Computational Statistics & Data Analysis* 58, 308–325. https://doi.org/10.1016/j.csda.2012.09.002 **[V]**
- Mariano, R. S., Murasawa, Y. (2003). A new coincident index of business cycles based on monthly and quarterly series. *Journal of Applied Econometrics* 18(4), 427–443. https://doi.org/10.1002/jae.695 **[V]**
- Martínez, L. R. (2022). How much should we trust the dictator's GDP growth estimates? *Journal of Political Economy* 130(10), 2731–2769. https://doi.org/10.1086/720458 **[V]**
- Masaki, T., Newhouse, D., Silwal, A. R., Bedada, A., Engstrom, R. (2022). Small area estimation of non-monetary poverty with geospatial data. *Statistical Journal of the IAOS* 38(3), 1035–1051. https://doi.org/10.3233/SJI-210902 **[V]**
- Mennis, J. (2003). Generating surface models of population using dasymetric mapping. *The Professional Geographer* 55(1), 31–42. https://doi.org/10.1111/0033-0124.10042 **[V]**
- Mennis, J., Hultgren, T. (2006). Intelligent dasymetric mapping and its application to areal interpolation. *Cartography and Geographic Information Science* 33(3), 179–194. https://doi.org/10.1559/152304006779077309 **[V]**
- Mercer, L. D., Wakefield, J., Pantazis, A., Lutambi, A. M., Masanja, H., Clark, S. (2015). Space–time smoothing of complex survey data: small area estimation for child mortality. *Annals of Applied Statistics* 9(4), 1889–1905. https://doi.org/10.1214/15-AOAS872 **[V]**
- Merfeld, J. D., Newhouse, D., Weber, M., Lahiri, P. Municipal labour-force estimates for Mexico from a nested-error model with geospatial auxiliary data. IZA Discussion Paper 15390. https://docs.iza.org/dp15390.pdf **[S]** (authors and subject from a search summary; the title is not given here because it was not checked)
- Meyer, H., Pebesma, E. (2021). Predicting into unknown space? Estimating the area of applicability of spatial prediction models. *Methods in Ecology and Evolution* 12(9), 1620–1633. https://doi.org/10.1111/2041-210X.13650 **[V]**
- Meyer, H., Pebesma, E. (2022). Machine learning-based global maps of ecological variables and the challenge of assessing them. *Nature Communications* 13, 2208. https://doi.org/10.1038/s41467-022-29838-9 **[V]**
- Meyer, H., Reudenbach, C., Hengl, T., Katurji, M., Nauss, T. (2018). Improving performance of spatio-temporal machine learning models using forward feature selection and target-oriented validation. *Environmental Modelling & Software* 101, 1–9. https://doi.org/10.1016/j.envsoft.2017.12.001 **[V]**
- Milà, C., Mateu, J., Pebesma, E., Meyer, H. (2022). Nearest neighbour distance matching leave-one-out cross-validation for map validation. *Methods in Ecology and Evolution* 13(6), 1304–1316. https://doi.org/10.1111/2041-210X.13851 **[V]**
- Molina, I., Marhuenda, Y. (2015). sae: an R package for small area estimation. *The R Journal* 7(1), 81. https://doi.org/10.32614/RJ-2015-007 **[V]** (first page only in the record)
- Molina, I., Nandram, B., Rao, J. N. K. (2014). Small area estimation of general parameters with application to poverty indicators: a hierarchical Bayes approach. *Annals of Applied Statistics* 8(2), 852–885. https://doi.org/10.1214/13-AOAS702 **[V]**
- Molina, I., Rao, J. N. K. (2010). Small area estimation of poverty indicators. *Canadian Journal of Statistics* 38(3), 369–385. https://doi.org/10.1002/cjs.10051 **[V]**
- Morris, M., Wheeler-Martin, K., Simpson, D., Mooney, S. J., Gelman, A., DiMaggio, C. (2019). Bayesian hierarchical spatial models: implementing the Besag York Mollié model in Stan. *Spatial and Spatio-temporal Epidemiology* 31, 100301. https://doi.org/10.1016/j.sste.2019.100301 **[V]**
- Mullainathan, S., Spiess, J. (2017). Machine learning: an applied econometric approach. *Journal of Economic Perspectives* 31(2), 87–106. https://doi.org/10.1257/jep.31.2.87 **[V]**
- Nagle, N. N., Buttenfield, B. P., Leyk, S., Spielman, S. (2014). Dasymetric modeling and uncertainty. *Annals of the Association of American Geographers* 104(1), 80–95. https://doi.org/10.1080/00045608.2013.843439 **[V]**
- Nandi, A. K., Lucas, T. C. D., Arambepola, R., Gething, P., Weiss, D. J. (2023). disaggregation: an R package for Bayesian spatial disaggregation modeling. *Journal of Statistical Software* 106(11). https://doi.org/10.18637/jss.v106.i11 **[V]**
- National Center for Health Statistics (2017). *National Center for Health Statistics Data Presentation Standards for Proportions*. Vital and Health Statistics, Series 2, No. 175. https://stacks.cdc.gov/view/cdc/47786 **[V]**; criteria as stated on NCHS's NHANES tutorial page https://wwwn.cdc.gov/nchs/nhanes/tutorials/ReliabilityOfEstimates.aspx **[V]**
- Nguyen, M. C., Corral, P., Azevedo, J. P., Zhao, Q. (2018). sae: a Stata package for unit level small area estimation. World Bank Policy Research Working Paper 8630. https://doi.org/10.1596/1813-9450-8630 **[V]**
- Nowak, D. J., Greenfield, E. J. (2012). Tree and impervious cover change in U.S. cities. *Urban Forestry & Urban Greening* 11(1), 21–30. https://doi.org/10.1016/j.ufug.2011.11.005 **[V]**
- Nowok, B., Raab, G. M., Dibben, C. (2016). synthpop: bespoke creation of synthetic data in R. *Journal of Statistical Software* 74(11). https://doi.org/10.18637/jss.v074.i11 **[V]**
- Office for National Statistics (2020, revised 1 May). Measuring and reporting reliability of Labour Force Survey and Annual Population Survey estimates. https://www.ons.gov.uk/employmentandlabourmarket/peopleinwork/employmentandemployeetypes/methodologies/measuringandreportingreliabilityoflabourforcesurveyandannualpopulationsurveyestimates **[V]**
- Openshaw, S. (1984). *The Modifiable Areal Unit Problem*. CATMOG 38, Geo Books. **[M]**
- Pereira, R. H. M., Saraiva, M., Herszenhut, D., Braga, C. K. V., Conway, M. W. (2021). r5r: rapid realistic routing on multimodal transport networks with R5 in R. *Findings*. https://doi.org/10.32866/001c.21262 **[V]**
- Pesaresi, M., Schiavina, M., Politis, P., Freire, S., Krasnodębska, K., Uhl, J. H., et al. (2024). Advances on the Global Human Settlement Layer by joint assessment of Earth Observation and population survey data. *International Journal of Digital Earth* 17(1), 2390454. https://doi.org/10.1080/17538947.2024.2390454 **[V]**
- Pfeffermann, D. (2013). New important developments in small area estimation. *Statistical Science* 28(1), 40–68. https://doi.org/10.1214/12-STS395 **[V]**
- Pfeffermann, D., Tiller, R. (2006). Small-area estimation with state–space models subject to benchmark constraints. *JASA* 101(476), 1387–1397. https://doi.org/10.1198/016214506000000591 **[V]**
- Ploton, P., Mortier, F., Réjou-Méchain, M., Barbier, N., Picard, N., Rossi, V., et al. (2020). Spatial validation reveals poor predictive performance of large-scale ecological mapping models. *Nature Communications* 11, 4540. https://doi.org/10.1038/s41467-020-18321-y **[V]**
- Prasad, N. G. N., Rao, J. N. K. (1990). The estimation of the mean squared error of small-area estimators. *JASA* 85(409), 163–171. https://doi.org/10.1080/01621459.1990.10475320 **[V]**
- Pratesi, M., Salvati, N. (2008). Small area estimation: the EBLUP estimator based on spatially correlated random area effects. *Statistical Methods and Applications* 17(1), 113–141. https://doi.org/10.1007/s10260-007-0061-9 **[V]**
- Proietti, T. (2006). Temporal disaggregation by state space methods: dynamic regression methods revisited. *Econometrics Journal* 9(3), 357–372. https://doi.org/10.1111/j.1368-423X.2006.00189.x **[V]**
- Purcell, N. J., Kish, L. (1980). Postcensal estimates for local areas (or domains). *International Statistical Review* 48(1), 3. https://doi.org/10.2307/1402400 **[V]** (first page only in the record)
- Raftery, A. E., Li, N., Ševčíková, H., Gerland, P., Heilig, G. K. (2012). Bayesian probabilistic population projections for all countries. *PNAS* 109(35), 13915–13921. https://doi.org/10.1073/pnas.1211452109 **[V]**
- Rao, J. N. K., Molina, I. (2015). *Small Area Estimation*, 2nd ed. Wiley. https://doi.org/10.1002/9781118735855 **[V]**
- Rao, J. N. K., Yu, M. (1994). Small-area estimation by combining time-series and cross-sectional data. *Canadian Journal of Statistics* 22(4), 511–528. https://doi.org/10.2307/3315407 **[V]**
- Rasmussen, C. E., Williams, C. K. I. (2006). *Gaussian Processes for Machine Learning*. MIT Press. **[M]**
- Ratledge, N., Cadamuro, G., de la Cuesta, B., Stigler, M., Burke, M. (2022). Using machine learning to assess the livelihood impact of electricity access. *Nature* 611(7936), 491–495. https://doi.org/10.1038/s41586-022-05322-8 **[V]**
- Riebler, A., Sørbye, S. H., Simpson, D., Rue, H. (2016). An intuitive Bayesian spatial model for disease mapping that accounts for scaling. *Statistical Methods in Medical Research* 25(4), 1145–1165. https://doi.org/10.1177/0962280216660421 **[V]**
- Roberts, D. R., Bahn, V., Ciuti, S., Boyce, M. S., Elith, J., Guillera-Arroita, G., et al. (2017). Cross-validation strategies for data with temporal, spatial, hierarchical, or phylogenetic structure. *Ecography* 40(8), 913–929. https://doi.org/10.1111/ecog.02881 **[V]**
- Robinson, W. S. (1950). Ecological correlations and the behavior of individuals. *American Sociological Review* 15(3), 351. https://doi.org/10.2307/2087176 **[V]**
- Rolf, E., Proctor, J., Carleton, T., Bolliger, I., Shankar, V., Ishihara, M., et al. (2021). A generalizable and accessible approach to machine learning with global satellite imagery. *Nature Communications* 12, 4392. https://doi.org/10.1038/s41467-021-24638-z **[V]**
- Romano, Y., Patterson, E., Candès, E. J. (2019). Conformalized quantile regression. NeurIPS 2019. arXiv:1905.03222 **[V]**
- Rosen, S. (1974). Hedonic prices and implicit markets: product differentiation in pure competition. *Journal of Political Economy* 82(1), 34–55. https://doi.org/10.1086/260169 **[V]**
- Rue, H., Martino, S., Chopin, N. (2009). Approximate Bayesian inference for latent Gaussian models by using integrated nested Laplace approximations. *JRSS B* 71(2), 319–392. https://doi.org/10.1111/j.1467-9868.2008.00700.x **[V]**
- Saisana, M., Saltelli, A., Tarantola, S. (2005). Uncertainty and sensitivity analysis techniques as tools for the quality assessment of composite indicators. *JRSS A* 168(2), 307–323. https://doi.org/10.1111/j.1467-985X.2005.00350.x **[V]**
- Sax, C., Steiner, P. (2013). Temporal disaggregation of time series. *The R Journal* 5(2), 80. https://doi.org/10.32614/RJ-2013-028 **[V]** (first page only in the record)
- Schmid, T., Bruckschen, F., Salvati, N., Zbiranski, T. (2017). Constructing sociodemographic indicators for national statistical institutes by using mobile phone data: estimating literacy rates in Senegal. *JRSS A* 180(4), 1163–1190. https://doi.org/10.1111/rssa.12305 **[V]**
- Schumacher, F., Knoth, C., Ludwig, M., Meyer, H. (2025). Estimation of local training data point densities to support the assessment of spatial prediction uncertainty. *Geoscientific Model Development* 18(24), 10185–10202. https://doi.org/10.5194/gmd-18-10185-2025 **[V]**
- Simpson, D., Rue, H., Riebler, A., Martins, T. G., Sørbye, S. H. (2017). Penalising model component complexity: a principled, practical approach to constructing priors. *Statistical Science* 32(1), 1–28. https://doi.org/10.1214/16-STS576 **[V]**
- Smith, S. K., Tayman, J., Swanson, D. A. (2013). *A Practitioner's Guide to State and Local Population Projections*. Springer. https://doi.org/10.1007/978-94-007-7551-0 **[V]**
- Smythe, I. S., Blumenstock, J. E. (2022). Geographic microtargeting of social assistance with high-resolution poverty maps. *PNAS* 119(32), e2120025119. https://doi.org/10.1073/pnas.2120025119 **[V]**
- Soundararaj, B., Cheshire, J., Longley, P. (2020). Estimating real-time high-street footfall from Wi-Fi probe requests. *International Journal of Geographical Information Science* 34(2), 325–343. https://doi.org/10.1080/13658816.2019.1587616 **[V]**
- Spielman, S. E., Folch, D. C. (2015). Reducing uncertainty in the American Community Survey through data-driven regionalization. *PLOS ONE* 10(2), e0115626. https://doi.org/10.1371/journal.pone.0115626 **[V]**
- Spielman, S. E., Folch, D., Nagle, N. (2014). Patterns and causes of uncertainty in the American Community Survey. *Applied Geography* 46, 147–157. https://doi.org/10.1016/j.apgeog.2013.11.002 **[V]**
- Spielman, S. E., Tuccillo, J., Folch, D. C., Schweikert, A., Davies, R., Wood, N., et al. (2020). Evaluating social vulnerability indicators: criteria and their application to the Social Vulnerability Index. *Natural Hazards* 100(1), 417–436. https://doi.org/10.1007/s11069-019-03820-z **[V]**
- Statistics Canada. Quality indicators by coefficient of variation (archived table, 2015). https://www150.statcan.gc.ca/n1/pub/88-202-x/2015000/t066-eng.htm **[S]**; small-area estimation in the Labour Force Survey (technical paper 75-005-M2024003). https://www150.statcan.gc.ca/n1/pub/75-005-m/75-005-m2024003-eng.htm **[S]**
- Steorts, R. C., Schmid, T., Tzavidis, N. (2020). Smoothing and benchmarking for small area estimation. *International Statistical Review* 88(3), 580–598. https://doi.org/10.1111/insr.12373 **[V]**
- Stevens, F. R., Gaughan, A. E., Linard, C., Tatem, A. J. (2015). Disaggregating census data for population mapping using random forests with remotely-sensed and ancillary data. *PLOS ONE* 10(2), e0107042. https://doi.org/10.1371/journal.pone.0107042 **[V]**
- Stewart, I. D., Oke, T. R. (2012). Local Climate Zones for urban temperature studies. *Bulletin of the American Meteorological Society* 93(12), 1879–1900. https://doi.org/10.1175/BAMS-D-11-00019.1 **[V]**
- Sun, L., Erath, A. (2015). A Bayesian network approach for population synthesis. *Transportation Research Part C* 61, 49–62. https://doi.org/10.1016/j.trc.2015.10.010 **[V]**
- Tanton, R. (2014). A review of spatial microsimulation methods. *International Journal of Microsimulation* 7(1), 4–25. **[M]**
- Tanton, R., Vidyattama, Y., Nepal, B., McNamara, J. (2011). Small area estimation using a reweighting algorithm. *JRSS A* 174(4), 931–951. https://doi.org/10.1111/j.1467-985X.2011.00690.x **[V]**
- Tarozzi, A., Deaton, A. (2009). Using census and survey data to estimate poverty and inequality for small areas. *Review of Economics and Statistics* 91(4), 773–792. https://doi.org/10.1162/rest.91.4.773 **[V]**
- Tartarini, F., Schiavon, S. (2020). pythermalcomfort: a Python package for thermal comfort research. *SoftwareX* 12, 100578. https://doi.org/10.1016/j.softx.2020.100578 **[V]**
- Tate, E. (2012). Social vulnerability indices: a comparative assessment using uncertainty and sensitivity analysis. *Natural Hazards* 63(2), 325–347. https://doi.org/10.1007/s11069-012-0152-2 **[V]**
- Tatem, A. J. (2017). WorldPop, open data for spatial demography. *Scientific Data* 4, 170004. https://doi.org/10.1038/sdata.2017.4 **[V]**
- Templ, M., Meindl, B., Kowarik, A., Dupriez, O. (2017). Simulation of synthetic complex data: the R package simPop. *Journal of Statistical Software* 79(10). https://doi.org/10.18637/jss.v079.i10 **[V]**
- Thomson, D. R., Gaughan, A. E., Stevens, F. R., Yetman, G., Elias, P., Chen, R. (2021). Evaluating the accuracy of gridded population estimates in slums: a case study in Nigeria and Kenya. *Urban Science* 5(2), 48. https://doi.org/10.3390/urbansci5020048 **[V]**
- Thorsson, S., Lindberg, F., Eliasson, I., Holmer, B. (2007). Different methods for estimating the mean radiant temperature in an outdoor urban setting. *International Journal of Climatology* 27(14), 1983–1993. https://doi.org/10.1002/joc.1537 **[V]**
- Tibshirani, R. J., Barber, R. F., Candès, E. J., Ramdas, A. (2019). Conformal prediction under covariate shift. NeurIPS 2019. arXiv:1904.06019 **[V]**
- Tobler, W. R. (1979). Smooth pycnophylactic interpolation for geographical regions. *JASA* 74(367), 519–530. https://doi.org/10.1080/01621459.1979.10481647 **[V]**
- Tolan, J., Yang, H.-I., Nosarzewski, B., Couairon, G., Vo, H. V., Brandt, J., et al. (2024). Very high resolution canopy height maps from RGB imagery using self-supervised vision transformer and convolutional decoder trained on aerial lidar. *Remote Sensing of Environment* 300, 113888. https://doi.org/10.1016/j.rse.2023.113888 **[V]**
- Tuholske, C., Gaughan, A. E., Sorichetta, A., de Sherbinin, A., Bucherie, A., Hultquist, C., et al. (2021). Implications for tracking SDG indicator metrics with gridded population data. *Sustainability* 13(13), 7329. https://doi.org/10.3390/su13137329 **[V]**
- Tzavidis, N., Zhang, L.-C., Luna, A., Schmid, T., Rojas-Perilla, N. (2018). From start to finish: a framework for the production of small area official statistics. *JRSS A* 181(4), 927–979. https://doi.org/10.1111/rssa.12364 **[V]**
- US Census Bureau (2026). Updates to 2025 ACS 1-year data release rules (user note 2026-06). https://www.census.gov/programs-surveys/acs/technical-documentation/user-notes/2026-06.html **[V]**
- USDA Forest Service / i-Tree. *A Guide to Assessing Urban Forests* (NRS-INF-24-13), on point sampling of canopy cover and its standard error. https://www.itreetools.org/documents/1/guide_to_assessing_urban_forests_nrs_inf_24_13.pdf **[S]**
- Valavi, R., Elith, J., Lahoz-Monfort, J. J., Guillera-Arroita, G. (2019). blockCV: an R package for generating spatially or environmentally separated folds for k-fold cross-validation of species distribution models. *Methods in Ecology and Evolution* 10(2), 225–232. https://doi.org/10.1111/2041-210X.13107 **[V]**
- van der Weide, R., Blankespoor, B., Elbers, C., Lanjouw, P. (2024). How accurate is a poverty map based on remote sensing data? An application to Malawi. *Journal of Development Economics* 171, 103352. https://doi.org/10.1016/j.jdeveco.2024.103352 **[V]**
- Venter, Z. S., Chakraborty, T., Lee, X. (2021). Crowdsourced air temperatures contrast satellite measures of the urban heat island and its mechanisms. *Science Advances* 7(22), eabb9569. https://doi.org/10.1126/sciadv.abb9569 **[V]**
- Voas, D., Williamson, P. (2001). Evaluating goodness-of-fit measures for synthetic microdata. *Geographical and Environmental Modelling* 5(2), 177–200. https://doi.org/10.1080/13615930120086078 **[V]**
- Vovk, V., Gammerman, A., Shafer, G. (2005). *Algorithmic Learning in a Random World*. Springer. https://doi.org/10.1007/b106715 **[V]**
- Wadoux, A. M. J.-C., Heuvelink, G. B. M., de Bruin, S., Brus, D. J. (2021). Spatial cross-validation is not the right way to evaluate map accuracy. *Ecological Modelling* 457, 109692. https://doi.org/10.1016/j.ecolmodel.2021.109692 **[V]**
- Wakefield, J. (2004). Ecological inference for 2 × 2 tables (with discussion). *JRSS A* 167(3), 385–445. https://doi.org/10.1111/j.1467-985X.2004.02046.x **[V]**
- Wakefield, J. (2007). Disease mapping and spatial regression with count data. *Biostatistics* 8(2), 158–183. https://doi.org/10.1093/biostatistics/kxl008 **[V]**
- Wakefield, J., Okonek, T., Pedersen, J. (2020). Small area estimation for disease prevalence mapping. *International Statistical Review* 88(2), 398–418. https://doi.org/10.1111/insr.12400 **[V]**
- Wardrop, N. A., Jochem, W. C., Bird, T. J., Chamberlain, H. R., Clarke, D., Kerr, D., et al. (2018). Spatially disaggregated population estimates in the absence of national population and housing census data. *PNAS* 115(14), 3529–3537. https://doi.org/10.1073/pnas.1715305115 **[V]**
- Wesolowski, A., Eagle, N., Noor, A. M., Snow, R. W., Buckee, C. O. (2013). The impact of biases in mobile phone ownership on estimates of human mobility. *Journal of the Royal Society Interface* 10(81), 20120986. https://doi.org/10.1098/rsif.2012.0986 **[V]**
- Wessel, N., Allen, J., Farber, S. (2017). Constructing a routable retrospective transit timetable from a real-time vehicle location feed and GTFS. *Journal of Transport Geography* 62, 92–97. https://doi.org/10.1016/j.jtrangeo.2017.04.012 **[V]**
- Wheldon, M. C., Raftery, A. E., Clark, S. J., Gerland, P. (2013). Reconstructing past populations with uncertainty from fragmentary data. *JASA* 108(501), 96–110. https://doi.org/10.1080/01621459.2012.737729 **[V]**
- Whitworth, A., Carter, E., Ballas, D., Moon, G. (2017). Estimating uncertainty in spatial microsimulation approaches to small area estimation: a new approach to solving an old problem. *Computers, Environment and Urban Systems* 63, 50–57. https://doi.org/10.1016/j.compenvurbsys.2016.06.004 **[V]**
- Wikle, C. K., Zammit-Mangion, A., Cressie, N. (2019). *Spatio-Temporal Statistics with R*. CRC Press. https://doi.org/10.1201/9781351769723 **[V]**
- Williamson, P., Birkin, M., Rees, P. H. (1998). The estimation of population microdata by using data from small area statistics and samples of anonymised records. *Environment and Planning A* 30(5), 785–816. https://doi.org/10.1068/a300785 **[V]**
- Wright, J. K. (1936). A method of mapping densities of population: with Cape Cod as an example. *Geographical Review* 26(1), 103. https://doi.org/10.2307/209467 **[V]** (first page only in the record)
- Ybarra, L. M. R., Lohr, S. L. (2008). Small area estimation when auxiliary information is measured with error. *Biometrika* 95(4), 919–931. https://doi.org/10.1093/biomet/asn048 **[V]**
- Ye, X., Konduri, K., Pendyala, R. M., Sana, B., Waddell, P. (2009). A methodology to match distributions of both household and person attributes in the generation of synthetic populations. Transportation Research Board Annual Meeting. **[M]**
- Yeh, C., Perez, A., Driscoll, A., Azzari, G., Tang, Z., Lobell, D., et al. (2020). Using publicly available satellite imagery and deep learning to understand economic well-being in Africa. *Nature Communications* 11, 2583. https://doi.org/10.1038/s41467-020-16185-w **[V]**
- You, Y., Rao, J. N. K. (2002). A pseudo-empirical best linear unbiased prediction approach to small area estimation using survey weights. *Canadian Journal of Statistics* 30(3), 431–439. https://doi.org/10.2307/3316146 **[V]**
- Zhang, L.-C., Chambers, R. L. (2004). Small area estimates for cross-classifications. *JRSS B* 66(2), 479–496. https://doi.org/10.1111/j.1369-7412.2004.05266.x **[V]**

---

## Part III. Urban Indicator Frameworks, the Measurement of Qualitative Indexes, and Qualitative Quantities in Dynamical Models

*Survey L3 for the design of multi-scale dynamical models of cities whose states are indexes. Written 2026-11-18.*

### 0. Scope and Markers

This survey covers five things the design needs: (i) the indicator frameworks that cities, statistical offices and UN agencies use, and the spatial unit at which each indicator is defined; (ii) the canonical definitions of the indexes that recur in urban science and economics, with attention to which of them are additive across spatial units, which decompose exactly into within and between parts, and which must be recomputed from unit data (this decides what may flow up the hierarchy of cells → neighbourhoods → districts → city → region); (iii) how qualitative indexes (trust, perceived safety, place attachment, design quality, risk ratings) are measured, and how their measurement error and non-comparability across groups is modelled; (iv) how qualitative quantities enter dynamical models (fuzzy cognitive maps, causal loop diagrams and system dynamics, qualitative simulation, ordinal and latent state-space models, regime models, opinion and trust dynamics), and what such models can honestly claim; (v) how frameworks treat missing indicators and present uncertainty. Each bibliographic item carries a marker: **[V]** verified this session against a primary record (Crossref's API for DOIs; the arXiv API; or the issuing body's own page or PDF, read this session); **[S]** confirmed only from secondary sources (search summaries, mirrors, catalogues); **[M]** from memory, not verified. Volumes and pages are given only where the primary record gave them. Statements about the *content* of a verified item are my reading of it and are flagged as uncertain where I could not check the passage. Section numbers below are cited as §n.m.

### 1. Indicator Frameworks and the Units at Which They Are Defined

#### 1.1 The Spatial Unit Decides the Number

**Concepts.** Every urban indicator is a functional of data over a spatial unit, and the unit is part of the definition. Three families of unit are in use: administrative units (municipalities, LAU2, census tracts), morphological units derived from a population or built-up grid (the Degree of Urbanisation's urban centres), and functional units derived from flows (the EU–OECD Functional Urban Area, a city plus its commuting zone). The modifiable areal unit problem (MAUP) says that statistics computed on aggregates change with both the scale and the zoning of the aggregation (Fotheringham and Wong 1991 [V]).

**The Degree of Urbanisation** (DEGURBA), endorsed by the UN Statistical Commission in March 2020, classifies 1 km² population grid cells (Dijkstra et al. 2021 [V]; European Commission and Eurostat 2021 manual [V]; Eurostat's level-1 manual page [S]): an *urban centre* is a cluster of contiguous cells (four-point contiguity, gaps filled, edges smoothed) each with at least 1,500 inhabitants/km² and together at least 50,000 inhabitants; an *urban cluster* uses eight-point contiguity, at least 300 inhabitants/km² and at least 5,000 inhabitants; the rest is rural. Local units are then classed as cities, towns and semi-dense areas, or rural areas by where their population lives. DEGURBA is the denominator of SDG 11.3.1 and 11.7.1 (§1.4).

**The Functional Urban Area** (FUA; OECD 2012 [V]; Eurostat's archived definition page [S]): the *city* is one or more local units with at least half their residents in an urban centre; its *commuting zone* is the contiguous set of local units with at least 15% of employed residents working in the city; cities that send 15% of their workers to one another are merged. The OECD Metropolitan Database reports indicators for FUAs (its "metropolitan areas" historically being FUAs of 250,000 inhabitants or more [M]), and the Regional Database for territorial levels TL2 and TL3.

**Critique.** Urban scaling laws (Y ∝ N^β, Bettencourt et al. 2007 [V]; Bettencourt 2013 [V]) are the clearest case of an indicator whose value depends on the unit: Arcaute et al. (2015) [V] showed that β for England and Wales changes, and can cross 1, as the city definition (density threshold, commuting threshold) changes; Cottineau et al. (2017) [V] found the same for French cities across thousands of definitions; Louf and Barthelemy (2014) [V] showed that the emissions–size exponent flips with definition; Leitão et al. (2016) [V] showed that many reported nonlinearities are not distinguishable from linearity once the error model is chosen properly. For the design: an index's value is not a property of a place but of (place, unit system, year of boundaries), and the unit system must be part of the index's declared identity.

#### 1.2 ISO 37120, 37122 and 37123

**What they are.** ISO 37120:2018, *Sustainable cities and communities — Indicators for city services and quality of life*, defines 104 indicators across 19 themes (the World Council on City Data's page [V]; a 2014 first edition had 100 indicators in 17 themes [S]). Each theme has *core* indicators (required for conformance), *supporting* indicators (recommended) and *profile* indicators (context for peer comparison) [S]. ISO 37122:2019 (smart cities) and ISO 37123:2019 (resilient cities) extend the series on the same 19 themes (their indicator counts, about 80 and about 68, are from memory [M]); ISO 37124:2024 gives guidance on using the three together [S]. Certification is run by the World Council on City Data (WCCD), with levels by the number of indicators reported [S].

**Definitions and units.** The indicators are mostly ratios per 100,000 population or per capita (e.g., homicides per 100,000; km of transit per 100,000; % of population with authorised electricity), computed for the *administrative* city, with the boundary declared by the reporting city. The standards define numerators and denominators but neither a sampling design nor an uncertainty, and they leave the boundary to the city, so cross-city comparison inherits §1.1's problem. They are paywalled; a reimplementation must cite clause numbers from a purchased copy.

#### 1.3 UN-Habitat's Urban Monitoring Framework and City Prosperity Index

**The Global Urban Monitoring Framework** (UMF), endorsed by the UN Statistical Commission at its 53rd session in March 2022, is a matrix of five domains (society, economy, culture, environment, and governance and implementation) against four city objectives (safe and peaceful, inclusive, resilient, sustainable), with 72 indicators recommended as a starting set that cities may extend (UN-Habitat news release [V]; metadata volume [S]). It harmonises SDG 11, the New Urban Agenda and earlier tools (CPI, the Urban Indicators Programme) and is meant to be computed for cities delineated by DEGURBA or the functional area, though cities may adapt it.

**The City Prosperity Index** (CPI; UN-Habitat 2016 methodology and metadata [V], read this session) has six dimensions: productivity, infrastructure development, quality of life, equity and social inclusion, environmental sustainability, and urban governance and legislation, each with sub-dimensions and indicators, in three versions: *Basic* (commonly available indicators, for global comparison), *Extended* (more urban and spatial indicators, comparability not the aim) and *Contextual* (adding local policy indicators). Its standardisation maps each variable to [0, 100] by one of seven declared rules: none (already a bounded percentage), reversal (100 − X), min–max between benchmarks taken from world data (sometimes after ln x or x^(1/k)), its reverse, and three *objective* rules, distance below a minimum target, distance above a maximum target, and distance from a single optimum, e.g. population density with optimum X* = 15,000/km², scored 100(1 − |X − X*|/X*). Weights are nested and equal (dimensions equal in the index, sub-dimensions equal within dimensions, variables equal within sub-dimensions), following Alkire and Foster (2011) [V]; aggregation within dimensions is arithmetic in the 2016 metadata. A UN Statistics expert group page notes that no systematic review of the whole methodology had been made [S].

**For the design.** The CPI's seven standardisation rules are a ready, declared vocabulary of normalisations (benchmarks with their source and year; targets with their authority) that the composite-indicator module can hold as data rather than code.

#### 1.4 The Indicators of SDG 11 and Their Metadata

The metadata files below were downloaded from the UN Statistics Division this session (all last updated 2025-04-23) [V].

| Indicator | Definition (as in the metadata) | Inputs and unit | Pitfalls stated or evident |
|---|---|---|---|
| 11.1.1 | Proportion of urban population living in slums, informal settlements or inadequate housing. A slum household lacks one or more of five: improved water, improved sanitation, sufficient living area (not more than three people per habitable room), durable housing, secure tenure. | Household surveys and censuses (DHS, MICS, census microdata); urban population as nationally defined or by DEGURBA. | A union of deprivations: additive over households, so it aggregates as a ratio of counts; tenure is the weakest-measured; the threshold of three per room was fixed for MDG tracking after a range of three to four. |
| 11.2.1 | Proportion of population with convenient access to public transport: an officially recognised stop within 500 m *walking distance along the street network* of a low-capacity system (bus, BRT, tram) or 1 km of a high-capacity one (rail, metro, ferry); further criteria (accessible to all, frequent at peak, safe stops) are listed. | Stop inventory (GTFS), street network (OpenStreetMap as baseline), population grid; city by DEGURBA. | Proximity, not service: frequency and capacity are optional attributes; informal transport is hard to map. Brussel et al. (2019) [V] criticise it as access, not accessibility. |
| 11.3.1 | Ratio of land consumption rate to population growth rate, LCRPGR = LCR/PGR, with **LCR = (V_present − V_past)/V_past × 1/t** (V the built-up area) and **PGR = ln(Pop_{t+n}/Pop_t)/y**; two secondary indicators: built-up area per capita and total change in built-up area. | Built-up area from imagery (GHSL), population (census or grids) within the DEGURBA urban area at two dates. | The metadata itself warns: the ratio is undefined or sign-inverted when population growth is zero or negative; averaging the ratio over cities is ambiguous; the 2025 formula mixes a simple rate (land) with a logarithmic rate (population). Nicolau et al. (2019) [V] compare formulations; Melchiorri et al. (2019) [V] and Schiavina et al. (2022) [V] compute it globally on GHSL. |
| 11.6.2 | Annual mean levels of fine particulate matter (PM2.5 and PM10) in cities, population-weighted. | WHO's ambient air quality database (ground monitors in about 6,000 cities and localities) integrated with satellite retrievals, chemical transport models and population by a hierarchical model (DIMAQ). | National monitor means and WHO modelled estimates differ; the global figures are modelled with intervals. Population-weighting makes it a ratio of sums Σ p_i c_i / Σ p_i, additive over cells. |
| 11.7.1 | Average share of the built-up area of cities that is open space for public use for all (streets plus open public spaces), with a companion: share of population within 400 m walking distance (about 5 minutes) along the network of an open public space. | High-resolution imagery, OpenStreetMap, inventories, field verification; street area by sampling circles (10 ha samples at one or 0.5 circles per hectare by area). | Ownership and access are hard to observe from imagery ("potential open public space" until verified); the 400 m threshold assumes equal catchments. |

For a qualitative companion, **SDG 16.1.4** (proportion of population that feel safe walking alone around the area they live after dark) is a single survey item, and its metadata [V] lists its known limitations: it presumes that respondents go out, alone, in their neighbourhood, after dark; "neighbourhood" is undefined; crime is implicit; the sample should allow sub-national disaggregation. (See §3.4.)

#### 1.5 The OECD Databases and Eurostat's Urban Audit

**OECD.** The Metropolitan Database (FUAs, §1.1) and the Regional Database (TL2, TL3) carry population, GDP estimates downscaled from regions, labour, environment (PM2.5 exposure from satellite models, built-up area) and well-being measures; GDP for FUAs is *estimated* by allocating regional GDP, so it is a model output, not an observation (the CPI's metadata uses these city GDP estimates as benchmarks [V]).

**Eurostat's Urban Audit** (city statistics; ESMS metadata page [S]; methodological manual on city statistics 2017 [V] DOI checked): more than 130 variables (140 required in 2024) and over 50 derived indicators, collected for the administrative *city* (C) and its *FUA* (F); older handbooks also used sub-city districts [S]. Variables are supplied by national statistical institutes from registers, censuses and surveys; perception variables come from the separate Perception Survey on the Quality of Life in European Cities. Where surveys are not designed for cities, Eurostat's *Guidelines on small area estimation for city statistics and other functional geographies* (2019; KS-GQ-19-011) [S] give the estimation framework (tested on EU-SILC's AROPE for twenty cities and FUAs [S]).

#### 1.6 CITYkeys, the City Resilience Index and Resilience Scorecards

**CITYkeys** (an EU Horizon 2020 project, deliverable D1.4, 2016) [S]: indicators for smart city *projects* (about 92) and *cities* (about 73) on an extended triple bottom line of five themes (people, planet, prosperity, governance, propagation); normalisation and weighting for an index were left open. Huovila, Bosch and Airaksinen (2019) [V] compare seven recently published indicator standards for smart sustainable cities (413 indicators) by urban focus, sector and indicator type (input, process, output, outcome, impact), and find that they divide into standards suited to assessing smart-city implementation and standards focused on sustainability.

**The City Resilience Index** (Arup with the Rockefeller Foundation, 2014–2016) [V Arup's page; S for the rest]: four dimensions, 12 goals, 52 indicators assessed through 156 questions; each indicator is assessed twice, by a judgement against best-case and worst-case scenarios and by performance against a benchmark, and responses are aggregated and presented by the 12 goals, without a single number; it is a self-assessment tool not designed for comparison between cities. The UNDRR *Disaster Resilience Scorecard for Cities* is similar in kind: 47 questions scored 0–3 at its preliminary level, 117 criteria scored 0–5 at its detailed level [S]. Cutter (2016) [V] reviews the landscape of resilience indicator sets in the USA; the BRIC index (Cutter, Burton and Emrich 2010) [V] is a county-level composite of the same family. Kaika (2017) [V] is the standard critique of indicator-led resilience and smart-city agendas.

#### 1.7 What It Would Mean for the Design

- **An index is (definition, unit system, boundary vintage, data vintage).** The studio should carry DEGURBA classes, FUA membership and administrative codes as three parallel unit systems over the same cells, and refuse to compare values computed on different systems unless a declared crosswalk exists.
- **SDG 11 as reference implementations.** 11.2.1, 11.3.1 (with its secondary indicators and both rate conventions), 11.6.2 (population-weighted means) and 11.7.1 (with the 400 m companion) are small, fully specified functionals of grids, networks and imagery; implemented exactly to the metadata, they become checks (hand-computed toy cities; published GHSL values for 11.3.1).
- **Frameworks as data.** ISO, UMF, CPI, CITYkeys and CRI are best represented as declared catalogues (indicator id, numerator, denominator, unit, normalisation rule with its benchmark, weight, aggregation, required/optional), not as code; the composite-indicator machinery then runs over any catalogue (§2.13).
- **Qualitative frameworks have no single number by design** (CRI); a studio that insists on a composite would misrepresent them; it should show goal-level profiles with their scenario judgements.

### 2. Canonical Indexes and Their Algebra Across Spatial Units

#### 2.1 The Organising Principle: Carry Sufficient Statistics, Not Indexes

**Concepts.** For a hierarchy of units, an index I is *additive* if I(A ∪ B) = I(A) + I(B) (counts, totals, areas); a *ratio of additive quantities* (rates, shares, population-weighted means) aggregates exactly when numerator and denominator are carried; it is *additively decomposable* (Shorrocks 1980 [V]; Bourguignon 1979 [V]) if the index of a union equals a weighted sum of the parts' indexes plus a between-part term that depends only on the parts' summary statistics; otherwise it *must be recomputed* from the finest data. Among inequality measures, Shorrocks showed that the additively decomposable, scale-invariant ones are exactly the generalised entropy family (Theil's T and L among them, with population- or income-share weights); the Gini is not (its decomposition leaves an overlap residual). Among segregation measures, the information-theory index H and the mutual information index M decompose; the dissimilarity index D and the isolation index do not (Reardon and Firebaugh 2002 [V]; Mora and Ruiz-Castillo 2011 [V]). Among diversity measures, Shannon entropy (and Hill numbers through Jost's multiplicative partition) decompose into α and β components (Jost 2007 [V]); the Gini–Simpson index does not decompose independently.

**For the design.** The upward coupling between levels should move *sufficient statistics* (counts by group and category, sums and sums of squares, the distribution where a median or a Gini is wanted), never indexes, and each index should declare its algebra: `additive`, `ratio-of-sums`, `decomposable(within, between)` or `recompute`. A model at the district level that needs segregation then receives group counts and recomputes; a model that wants to know how much of the city's segregation is between districts reads the between term exactly.

#### 2.2 Segregation

**Massey and Denton's five dimensions** (Massey and Denton 1988 [V]): evenness (dissimilarity D = ½ Σ_i |a_i/A − b_i/B|, the Gini, Atkinson indices, entropy H), exposure (interaction ₓP_y and isolation ₓP_ₓ = Σ_i (x_i/X)(x_i/t_i), the correlation ratio), concentration (share of physical space occupied), centralisation (nearness to the centre), and clustering (contiguity of minority areas, spatial proximity), twenty indices in all; their factor analysis of US metropolitan areas found the five dimensions empirically distinct. Duncan and Duncan (1955) [V] is the classic methodological analysis of segregation indexes, including D's dependence on the units chosen.

**The information-theory index.** For groups m with city shares π_m, entropy E = −Σ_m π_m ln π_m; for unit i with population t_i and entropy E_i, Theil's H = Σ_i t_i (E − E_i)/(T E) (Theil and Finizza 1971 [V]). H ∈ [0, 1], handles many groups, and decomposes: for units nested in districts k, H = H_between-districts + Σ_k (T_k E_k/(T E)) H_within-k (Reardon and Firebaugh 2002 [V]). The mutual information index M = E − Σ_i (t_i/T) E_i is decomposable both by units and by groups with weights independent of entropies (Mora and Ruiz-Castillo 2011 [V]); Elbers (2023) [V] builds a method for comparing segregation across time and space on M's decomposition (R package `segregation`).

**Spatial versions.** Aspatial indexes ignore the arrangement of units (the "checkerboard problem") and change with zoning (MAUP). White (1983) [V] and Wong (1993) [V] proposed spatial adjustments; Reardon and O'Sullivan (2004) [V] define each person's *local environment* as a kernel-weighted population around their location (with a proximity function such as a biweight of bandwidth r), and spatial versions of H, D and exposure computed on these environments; the resulting *segregation profile* (index against r) separates macro- from micro-segregation. Applied to US metropolitan areas at radii of 500–4,000 m, macro-scale segregation was a large share of the total (Reardon et al. 2008 [V]; Lee et al. 2008 [V]); Wong (2004) [V] compared traditional and spatial measures across scales. Spatial indices need a population *surface*, which is itself an estimate (dasymetric mapping), so their uncertainty includes the surface's.

**Dynamics.** Segregation is the canonical urban index with a tipping law: Card, Mas and Rothstein (2008) [V] estimate tipping points in neighbourhoods' minority shares (typically 5–20%) beyond which white population falls discontinuously, a regime structure a dynamical model at the neighbourhood level should be able to represent (§4.5).

**Implementations.** PySAL's `segregation` (Cortes et al. 2020 [V]), with aspatial, spatial, multigroup and decomposition tools and inference by simulation; R's `segregation` (Elbers), `OasisR`, `seg`.

**For the design.** Group counts per cell are the sufficient statistic; H and M give exact within/between decompositions across the hierarchy; the spatial profile is a function of bandwidth, a natural multi-scale object; D and isolation are recompute-only.

#### 2.3 Concentration, Specialisation and Shift-Share

- **Herfindahl–Hirschman index** HHI = Σ_i s_i² (s_i shares of firms, sectors or partners); with N units HHI = 1/N + N·Var(s). Not decomposable across spatial units: recompute from shares. Ellison and Glaeser (1997) [V] correct geographic concentration for plant-size concentration: γ = [G − (1 − Σ_r x_r²) H] / [(1 − Σ_r x_r²)(1 − H)], with G the raw geographic concentration and H the plant HHI; γ is an estimator of spillover/natural-advantage intensity under a "dartboard" null.
- **Location quotient** LQ_{ir} = (e_{ir}/e_r)/(e_i/e). A ratio of ratios; recompute for any new aggregation from the counts e_{ir}. Isserman (1977) [V] reviews its use for export-base multipliers and its biases (cross-hauling, assumption of identical consumption).
- **Krugman's specialisation index** K_r = Σ_i |s_{ir} − s_i^ref|, twice a dissimilarity index between a region's sectoral structure and a reference (Krugman, *Geography and Trade*, 1991 [M]).
- **Shift-share** (Dunn 1960 [V]): Δe_{ir} = e_{ir} g + e_{ir}(g_i − g) + e_{ir}(g_{ir} − g_i), national growth, industry mix and regional (competitive) components. It is an accounting identity with no causal content; Esteban (2000) [V] gives a regression form. In econometrics the industry-mix term is the Bartik instrument, whose identification rests either on exogenous shares (Goldsmith-Pinkham, Sorkin and Swift 2020 [V]) or on many exogenous shocks (Borusyak, Hull and Jaravel 2022 [V]).

**For the design.** Employment counts by sector and unit are the sufficient statistic; the shift-share identity is exact and should be checked to rounding at every level; LQs at the cell level are noisy for small counts and need shrinkage or refusal below a count threshold.

#### 2.4 Diversity and Land-Use Mix

Shannon entropy H = −Σ_j p_j ln p_j, its normalised land-use-mix form LUM = H/ln k over k categories (Cervero and Kockelman 1997 [V]), Simpson's concentration λ = Σ p_j² (Simpson 1949 [V]) and Gini–Simpson 1 − λ, and Hill numbers ^qD = (Σ p_j^q)^{1/(1−q)} unifying them (Hill 1973 [V]). Song, Merlin and Rodriguez (2013) [V] compare eight land-use-mix measures and find them sensitive to the category set and the unit size, and weakly related to travel outcomes in some forms. LUM depends on k and on the choice of categories (the same street gets different values under different land-use classifications) and is not monotone in "mix" in the planning sense (perfect balance among irrelevant categories scores 1). Hill numbers with Jost's (2007) [V] multiplicative partition give exact α (within-unit), β (between-unit) and γ (total) diversity, so mix is decomposable across the hierarchy if counts (floor area by use) are carried; the infrastructure's tyche `classes` already holds Hill numbers and Jost's partition.

#### 2.5 Walkability and Urban Design Qualities

**Walkability index** (Frank et al. 2005 [V]; Frank et al. 2010 [V]): W = 2·z(intersection density) + z(net residential density) + z(retail floor-area ratio) + z(land-use-mix entropy), computed in network buffers (often 1 km) or block groups, with z-scores *relative to the study region*. Consequently W is not comparable across regions and must be recomputed when the region changes; it is an ordinal ranking device, validated against measured physical activity and travel in the NQLS and SMARTRAQ studies. Ewing and Cervero (2010) [V] meta-analyse the elasticities of travel with respect to the "D" variables (density, diversity, design, destination accessibility, distance to transit); most are small (|e| < 0.4). Walk Score, a commercial proximity score, was validated against GIS measures of amenities (Carr, Dunsiger and Marcus 2011 [V]; Duncan et al. 2011 [V]); the US EPA publishes a National Walkability Index on block groups (from the Smart Location Database) [M].

**Urban design qualities** (Ewing et al. 2006 [V]; Ewing and Handy 2009 [V]): an expert panel rated video clips of streets on nine qualities; five could be measured reliably and predicted from physical features: *imageability, enclosure, human scale, transparency, complexity*; the operational definitions are regressions of panel ratings on countable features (proportion of street wall, windows at street level, long sight lines, number of buildings with identifiers, …). Purciel et al. (2009) [V] created GIS versions and validated them against field ratings in New York; Yin and Wang (2016) [V] measured enclosure from street-view imagery with machine learning. These are *qualitative quantities defined by a calibrated measurement model*: a rating scale, raters, and a regression to observables, whose reliability (inter-rater ICC) was reported per quality.

**For the design.** Walkability is a recompute-only, region-relative index; better to carry its four components (each additive or ratio-of-sums) and form W at the reporting level. Ewing–Handy qualities are an example of the pattern the studio should support generally: an expert-rated latent quality, a rater model (§3.6), and a learned map from observables with its validation error.

#### 2.6 Accessibility

Hansen's (1959) [V] potential A_i = Σ_j O_j f(c_ij); cumulative opportunities within a threshold; the two-step floating catchment area (Luo and Wang 2003 [V]), which divides supply by the demand that reaches it; and utility-based logsums. Geurs and van Wee (2004) [V] classify measures by components (land use, transport, temporal, individual) and criteria (theoretical basis, interpretability, data). The infrastructure's nomos `measurement` and urban-science `access` already hold these. Algebra: additive over destinations, but the area value of an origin-based measure is a population-weighted mean, a ratio of sums; 2SFCA's catchment ratios are not decomposable and must be recomputed when units or networks change. SDG 11.2.1 and 11.7.1 are cumulative-opportunity measures with fixed thresholds.

#### 2.7 Housing Affordability

Ratio measures: price-to-income (the "median multiple"), rent-to-income with the 30% rule, and the 30/40 rule (households in the bottom 40% of the income distribution paying more than 30%). Hulchanski (1995) [V] distinguishes six uses of the expenditure-to-income ratio (description, administration, definition of need, prediction, selection, policy) and argues that only some are legitimate. Bramley (2012) [V] triangulates ratio, residual-income and need-based measures for England. Stone (2006) [V] argues for the *residual income* approach: a household is housing-burdened if income less housing cost falls below a normative non-housing budget, so affordability depends on household size and composition. Algebra: medians and shares of burdened households require household-level distributions (microdata or synthetic populations); a ratio of medians is not the median of ratios, and neither aggregates from area medians. For the design, affordability belongs at the household level of a microsimulation (nomos `microsimulation`) and is aggregated as a count of burdened households, a ratio of sums.

#### 2.8 Thermal Comfort

- **Mean radiant temperature** T_mrt: the uniform temperature of an enclosure exchanging the same radiation with a person as the real environment. The six-directional method: S_str = α_k Σ_i K_i F_i + ε_p Σ_i L_i F_i over shortwave K and longwave L fluxes from six directions with angular factors F_i (0.22 lateral, 0.06 up and down for a standing person), α_k ≈ 0.7, ε_p ≈ 0.97, and T_mrt = (S_str/(ε_p σ))^{1/4} − 273.15 (Thorsson et al. 2007 [V]; globe-thermometer alternative in ISO 7726). It depends on three-dimensional geometry and is the dominant term of outdoor heat stress by day; SOLWEIG (Lindberg, Holmer and Thorsson 2008 [V]) computes it on raster DSMs; RayMan (Matzarakis, Rutz and Mayer 2007 [V]) at points. Kántor and Unger (2011) [V] call it the most problematic variable of outdoor comfort assessment.
- **UTCI**: the air temperature of a reference environment (50% relative humidity with vapour pressure capped at 20 hPa, wind 0.5 m/s at 10 m, T_mrt = T_a) that produces the same dynamic physiological response of Fiala's multi-node model with an adaptive clothing model, for a person walking at 4 km/h. Its operational form is a sixth-order polynomial in T_a, T_mrt − T_a, wind and vapour pressure fitted to the model (Bröde et al. 2012 [V]); valid over roughly −50 to +50 °C air temperature. Ten stress categories, from extreme cold stress (< −40 °C) to extreme heat stress (> 46 °C), with "no thermal stress" between 9 and 26 °C (Jendritzky, de Dear and Havenith 2012 [V]; Błażejczyk et al. 2012 [V] compare it with other indices).
- **PET** (Höppe 1999 [V]): the air temperature at which, in a typical indoor setting (T_mrt = T_a, wind 0.1 m/s, vapour pressure 12 hPa), the heat budget of the body (MEMI model, light activity, 0.9 clo) balances with the same core and skin temperatures as outdoors.
- **WBGT** (outdoor) = 0.7 T_nwb + 0.2 T_g + 0.1 T_a (ISO 7243; Yaglou and Minard 1957 [M]); Liljegren et al. (2008) [V] compute it from standard meteorological data; Budd (2008) [V] reviews its history and its underestimation of strain where evaporation is restricted.

**Algebra.** All four are intensive and strongly nonlinear: the index of an averaged environment is not the average index. They must be computed at the finest resolution of the meteorological and radiative fields (cells, hours) and then summarised as exposure: population-hours above a category threshold, which is additive. **Implementations:** `pythermalcomfort` (Tartarini and Schiavon 2020 [V]), `thermofeel` (Brimicombe et al. 2022 [V]), SOLWEIG in UMEP, RayMan; Bröde's UTCI Fortran reference code [M].

#### 2.9 Vulnerability Indices and Their Robustness

**SoVI** (Cutter, Boruff and Shirley 2003 [V]): census variables for US counties (42 in the original, reduced in later versions) entered a principal components analysis with varimax rotation; components were given signs by judgement (does a high value raise or lower vulnerability?) and summed with equal weights; the index is mapped in standard deviations. CDC's SVI (Flanagan et al. 2011 [V]) instead ranks fifteen census variables into percentiles and sums them, a deductive design. Heat vulnerability indices follow SoVI: Reid et al. (2009) [V] reduced ten tract-level variables to four factors; Reid et al. (2012) [V] evaluated it in five states: higher index values went with higher hospitalisation and mortality on normal days as well as on hot days, with a stronger association on hot days only for some outcomes and states, so the index may mark general health vulnerability more than vulnerability to heat.

**Robustness critiques.** Schmidtlein et al. (2008) [V] showed sensitivity to scale and to PCA decisions; Tate (2012) [V] ran global uncertainty and sensitivity analysis over construction choices (variables, normalisation, weighting, aggregation, scale) for deductive, hierarchical and inductive designs and found the ranks of many units uncertain, the choice of design and of weighting being influential; Spielman et al. (2020) [V] found that SoVI fails internal consistency (a unit's rank changes with the geographic extent over which the PCA is run) and questioned its construct validity (my reading of Tate 2012 and Spielman et al. 2020; their abstracts could not be read this session); Rufat et al. (2019) [V] regressed Hurricane Sandy outcomes on four social vulnerability models, controlling for flood exposure: the indexes explained applications for housing assistance best and property loss poorly, and weighted and profile configurations showed higher construct validity than the prevailing indexes; Bakkensen et al. (2017) [V] found mixed predictive validity of resilience and vulnerability indices for disaster losses; Conlon et al. (2020) [V] reached similar conclusions for PCA-based heat vulnerability indices; Rufat et al. (2015) [V] review flood vulnerability measurement.

**For the design.** SoVI-type indices are *recompute-only and extent-relative* (the PCA depends on all units in the analysis), so they cannot be aggregated or downscaled and they change when the study extent changes. The design should prefer deductive indices with declared weights, attach a global UA/SA of construction choices (§2.13), and, wherever outcome data exist (heat deaths, damages, claims), report predictive validation; where none exist, label the index "unvalidated".

#### 2.10 Economic Complexity

Given a binary matrix M_cp (unit c has revealed comparative advantage RCA ≥ 1 in activity p, RCA being a location quotient), diversity k_c = Σ_p M_cp and ubiquity k_p = Σ_c M_cp; the *Economic Complexity Index* is the standardised eigenvector of M̃ = D⁻¹ M U⁻¹ Mᵀ associated with the second-largest eigenvalue, the fixed point of the "method of reflections" (Hidalgo et al. 2007 [V]; Hidalgo and Hausmann 2009 [V]; review Hidalgo 2021 [V]). Mealy, Farmer and Teytelboym (2019) [V] proved that the ECI is the Fiedler vector of a similarity graph, i.e. spectral clustering, so it orders units by similarity of their activity baskets and its interpretation as "complexity" is an added hypothesis. The *fitness–complexity* algorithm (Tacchella et al. 2012 [V]) is a nonlinear alternative (F_c = Σ_p M_cp Q_p, 1/Q_p = Σ_c M_cp/F_c, normalised each step); Mariani et al. (2015) [V] compare metrics on their ability to rank. For cities, complexity has been computed from industries, occupations and patents (Balland and Rigby 2017 [V]); complex activities concentrate disproportionately in large cities (Balland et al. 2020 [V]). **Algebra:** relative and recompute-only (it changes when the set of units or activities changes; the binarisation threshold is arbitrary). **Implementations:** Harvard Growth Lab's `ecomplexity` (Python), R `economiccomplexity` [M].

#### 2.11 Corruption Risk in Public Procurement

Fazekas, Tóth and King (2016) [V] define an *objective corruption risk index* from contract-level procurement records: elementary red flags (single bidder; call for tenders not published; non-open procedure; unusually short advertisement or decision periods; reliance on non-price evaluation criteria; contract modifications) are validated by their association with single bidding (the core outcome indicating restricted competition), then combined with weights derived from regressions into a [0, 1] index per contract, aggregated to buyers, suppliers, regions or years; Fazekas and Kocsis (2020) [V] extend it across European countries, and Fazekas and Tóth (2016) [V] use it to detect state capture. The index measures *risk*, not corruption; its validity rests on the regression weights' stability and on red flags not being gamed. **For the design:** a municipal index computed from open contract data (e.g., the Open Contracting Data Standard), additive at the contract level and aggregated as a contract-weighted mean (ratio of sums), with its validation regressions kept as evidence.

#### 2.12 Governance: The Worldwide Governance Indicators and Subnational Analogues

**The unobserved components model** (Kaufmann, Kraay and Mastruzzi 2010 working paper [V]; 2011 article [V]): each source k reports for country j a score y_jk = α_k + β_k (g_j + ε_jk), with latent governance g_j ~ N(0, 1) and source error ε_jk ~ N(0, σ_k²) independent across sources; the parameters are estimated and the published estimate is E[g_j | y_j] with standard error √Var[g_j | y_j], smaller when more and more precise sources cover a country; six dimensions (voice and accountability, political stability, government effectiveness, regulatory quality, rule of law, control of corruption); estimates are rescaled each year and so are relative. **Critiques:** Thomas (2010) [V] argues that construct validity was never shown; Kurtz and Schrank (2007) [V] that perception-based sources are contaminated by economic performance (halo); Langbein and Knack (2010) [V] that the six dimensions are not empirically distinct; and the independence of source errors is doubtful where sources copy each other (the same concern as tekmerion's "reports that share an origin counted once"). **Subnational:** the European Quality of Government Index (Charron, Dijkstra and Lapuente 2014 [V]) surveys tens of thousands of residents on quality, impartiality and corruption of public services in regions and combines them with national WGI scores.

**For the design.** The UCM is a one-factor measurement model with heteroscedastic, source-specific errors: exactly the right estimator for *institutional capacity* or *legitimacy* indexes assembled from heterogeneous sources (surveys, expert ratings, administrative signals), giving each district an estimate with a standard error, and it should declare which sources share an origin.

#### 2.13 Composite Indicators: Methodology, Uncertainty and Sensitivity

**The handbook** (OECD and JRC 2008 [V]) sets ten steps: theoretical framework; data selection; imputation of missing data; multivariate analysis; normalisation (ranking, z-scores, min–max, distance to a reference, categorical scales); weighting (equal, statistical (PCA, factor analysis), benefit of the doubt, participatory (budget allocation, AHP)) and aggregation (linear, geometric, non-compensatory multicriteria); uncertainty and sensitivity analysis; back to the data (deconstruction); links to other indicators; visualisation. Linear aggregation is fully compensatory (a deficit in one dimension is offset by a surplus in another); geometric aggregation is less so; non-compensatory Condorcet-type rankings avoid it (Munda and Nardo 2009 [V]). Benefit-of-the-doubt weights (Cherchye et al. 2007 [V]) let each unit be scored with its most favourable weights by a DEA linear programme. Saltelli (2007) [V] frames composites as lying "between analysis and advocacy".

**Uncertainty and sensitivity analysis** (Saisana, Saltelli and Tarantola 2005 [V]): treat each construction choice (inclusion of an indicator, imputation method, normalisation, weights drawn around their nominal values, aggregation rule) as a random input, propagate by Monte Carlo, report each unit's median rank and its 90% interval (uncertainty analysis), and apportion the variance of rank shifts to choices by Sobol' indices (sensitivity analysis; Saltelli et al. 2008 primer [V]). **Weights are not importance** (Paruolo, Saisana and Saltelli 2013 [V]): the influence of an indicator on the composite is measured by the first-order Sobol' index (Pearson's correlation ratio) S_i = V(E[Y|x_i])/V(Y), which depends on the indicators' variances and correlations as well as the nominal weights; Becker et al. (2017) [V] give an optimisation that finds weights achieving target importances. Greco et al. (2019) [V] review weighting, aggregation and robustness. **Implementation:** COINr (Becker et al. 2022 [V]), the JRC's R package, covers the whole pipeline including UA/SA; `Compind` (R) offers benefit-of-the-doubt and other methods [M].

#### 2.14 Summary of the Algebra Across Spatial Units

| Index | Additive or ratio-of-sums | Exactly decomposable (within + between) | Recompute from unit data | Sufficient statistic to carry upward |
|---|---|---|---|---|
| Counts, totals, built-up area, population | additive | yes (trivially) | no | the totals |
| Rates, shares, population-weighted means (11.2.1, 11.6.2, 11.7.1) | ratio of sums | yes | no | numerator and denominator |
| SDG 11.3.1 LCRPGR | no (ratio of rates) | no; averaging is ambiguous (metadata) | yes | built-up area and population at both dates |
| Theil T, L; generalised entropy (income) | no | yes (Shorrocks) | no if subgroup means, sizes, indexes carried | subgroup size, mean, index |
| Gini, Atkinson (non-GE), medians, quantiles | no | Gini: no (overlap term) | yes | the distribution (or a fine histogram) |
| Segregation H, M | no | yes (H by units; M by units and groups) | no if group counts carried | group counts per unit |
| Dissimilarity D, isolation, clustering | no | no | yes | group counts per unit (and geometry for spatial) |
| Spatial H̃, D̃ (kernel environments) | no | no | yes (needs population surface) | population surface by group |
| HHI, LQ, Krugman K, RCA | no | no | yes | counts by unit and sector |
| Shift-share components | additive (identity) | yes | no | counts by unit and sector at two dates |
| Shannon, Hill numbers (mix, diversity) | no | yes (Jost's α, β, γ) | no if category counts carried | category counts (floor area by use) |
| Walkability (Frank) | no (region-relative z-scores) | no | yes when the region changes | the four components |
| Accessibility (Hansen, cumulative) | additive over destinations; area value ratio-of-sums | partially | 2SFCA: yes | opportunities, demand, costs |
| Affordability (ratio, residual income) | burdened-household counts: ratio of sums | yes for counts | medians: yes | household-level distribution |
| UTCI, PET, WBGT, T_mrt | no (intensive, nonlinear) | no | yes, at the finest field resolution | exposure: population-hours by category |
| SoVI, PCA-based HVI | no | no | yes (extent-relative) | all input variables for all units |
| ECI, fitness | no | no | yes (relative to the set of units) | the unit × activity matrix |
| CRI (procurement) | contract-weighted mean | yes over contracts | no | contract-level flags and weights |
| WGI-type UCM estimates | no | no | yes (posterior from sources) | source scores with error variances |
| Composite indexes (linear) | linear in normalised indicators, but normalisation is set-relative | no | yes when normalisation is set-relative (min–max, z) | indicators; fixed external benchmarks make it local |

#### 2.15 What It Would Mean for the Design

- A declared **algebra tag** on every index, enforced by the hierarchy: the coupling "aggregate upward" may only sum additive quantities and form ratios of sums; decomposable indexes report within/between parts; recompute-only indexes request the sufficient statistics.
- **Set-relative normalisations** (z-scores, min–max over observed units, PCA) make an index change when units are added or removed; the studio should prefer fixed external benchmarks (as the CPI does) for anything that must be stable across levels and years, and mark set-relative indexes as such.
- **Small counts**: LQ, D, H and LUM are biased upward in small units (random allocation produces apparent segregation or specialisation); the cell level needs either a count threshold below which it refuses, or a model-based estimate with bias correction (randomisation nulls are available in PySAL's segregation module).

### 3. Measuring Qualitative Indexes

#### 3.1 Likert Responses Are Ordinal Readouts of Latent Quantities

**Concepts.** A Likert item (Likert 1932 [M]) records an ordered category; a scale sums or averages several items. The principled model treats each response as a thresholded readout of a latent continuous variable: Y = k iff τ_{k−1} < Y* ≤ τ_k, Y* = η + ε. With covariates this is the *cumulative link* (proportional-odds or ordered-probit) model, logit P(Y ≤ k) = τ_k − xᵀβ (McCullagh 1980 [V]); with several items loading on a common factor it is the *graded response model* of IRT, P(Y_j ≥ k | θ) = 1/(1 + exp(−a_j(θ − b_jk))) (Samejima 1969 [V]), equivalent to an ordinal factor model with probit or logit link.

**Methods and their failures.** Treating ordinal responses as metric (means, t-tests, OLS) produces false alarms, misses and even inversions of the sign of group differences when latent variances differ or responses pile up at an end (Liddell and Kruschke 2018 [V]); Bürkner and Vuorre (2019) [V] give a tutorial of cumulative, sequential and adjacent-category models (R `brms`, Bürkner 2017 [V]; `ordinal`, Christensen [M]). Factor analysis of ordinal items should use polychoric correlations (Olsson 1979 [V]) with a robust weighted least squares estimator; maximum likelihood on Pearson correlations is biased with few categories and skewed responses (Flora and Curran 2004 [V]). Coefficient α assumes equal loadings (tau-equivalence) and is a lower bound only under it; ω from a fitted factor model is preferred (Sijtsma 2009 [V]; McNeish 2018 [V]). **Implementations:** `lavaan` (Rosseel 2012 [V]), `mirt` (Chalmers 2012 [V]), `semTools`, `ordinal`, `brms`; nomos `psychometrics` already holds classical test theory and IRT by marginal ML; tyche holds factor analysis.

#### 3.2 Neighbourhood Indexes from Individual Responses: Ecometrics

Raudenbush and Sampson (1999) [V] defined *ecometrics*: the psychometrics of ecological settings. Items are nested in respondents nested in neighbourhoods; a three-level model separates item, person and neighbourhood variance, and the reliability of the neighbourhood mean is λ_j = τ/(τ + σ²/n_j (+ item-level terms)), increasing with the number of respondents per neighbourhood n_j and with the intra-neighbourhood correlation. The empirical-Bayes neighbourhood estimates shrink small-sample neighbourhoods toward the regression prediction, exactly the shrinkage of a Fay–Herriot model. Mujahid et al. (2007) [V] report ICCs and neighbourhood-level reliabilities for scales of aesthetic quality, walking environment, safety, violent crime and social cohesion in a multi-site study: reliabilities are adequate only with enough respondents per area, and neighbourhood ICCs are small, so neighbourhood differences are a small share of individual variation (the exact values are in the paper and were not checked this session).

**For the design.** A qualitative index at the neighbourhood level is a *small-area estimate of a latent mean*: it needs the measurement model (§3.1), the sampling design and a shrinkage model; the neighbourhood reliability is the quantity to report, and a threshold on it (say λ_j ≥ 0.7) is a principled rule for refusing to publish a neighbourhood value.

#### 3.3 Comparability Across Groups and Languages: Measurement Invariance

**Concepts.** Comparing latent means across groups (languages, cultures, age groups, waves) requires *measurement invariance*: configural (same structure), metric (equal loadings; comparisons of relations), scalar (equal intercepts or thresholds; comparisons of means), strict (equal residual variances) (Vandenberg and Lance 2000 [V]; Putnick and Bornstein 2016 [V]). For ordinal items identification is subtler: thresholds and loadings trade off, and Wu and Estabrook (2016) [V] give identification conditions for each level. Changes in fit indices are the usual criterion, e.g. a drop in CFI larger than about 0.01 signals non-invariance (Chen 2007 [V]), though such cutoffs are heuristics. With many groups, exact scalar invariance almost always fails; *alignment* (Asparouhov and Muthén 2014 [V]) estimates group means under approximate invariance by minimising a simplicity function on non-invariance. *Anchoring vignettes* (King et al. 2004 [V]) correct differential item functioning in response scales directly: respondents rate hypothetical vignettes as well as themselves, and their own response is recoded relative to their vignette ratings (R `anchors`).

**Evidence.** Generalised trust items in the European Social Survey reached only partial equivalence across countries (Reeskens and Hooghe 2008 [V]), so cross-country comparisons of trust means are fragile; the same caution applies to perceived safety and place attachment translated across languages in multilingual cities.

**For the design.** Every qualitative index should carry its invariance status across the groups over which it is compared (by language, by wave); if only metric invariance holds, the studio may compare correlations and dynamics but not levels; this is a refusal rule of the same kind as §3.2's reliability threshold.

#### 3.4 Validated Scales for Urban Constructs

- **Place attachment.** Williams and Vaske (2003) [V] validated a two-factor scale (place identity, place dependence); Hidalgo and Hernández (2001) [V] found attachment weakest at the neighbourhood scale compared with home and city, a result Lewicka (2010) [V] confirmed; Scannell and Gifford (2010) [V] organise the construct as person, process and place; Lewicka's (2011) [V] forty-year review finds length of residence the most consistent predictor, with social ties, age and home ownership.
- **Sense of community.** McMillan and Chavis (1986) [V]: membership, influence, integration and fulfilment of needs, shared emotional connection; the Brief Sense of Community Scale (eight items, four first-order factors and a second-order factor; Peterson, Speer and McMillan 2008 [V]); Buckner's (1988) [V] Neighborhood Cohesion Instrument (18 items).
- **Perceived safety and fear of crime.** The standard item "How safe do you feel walking alone in this area after dark?" (the basis of SDG 16.1.4, whose metadata [V] lists its limitations, §1.4) measures perceived risk more than fear and is vague about place and crime (Ferraro and LaGrange 1987 [V]); intensity questions overstate fear compared with questions on the frequency of worry, which show fear episodes to be rarer (Farrall et al. 1997 [V]; Gray, Jackson and Farrall 2008 [V]).
- **Social cohesion and collective efficacy.** Sampson, Raudenbush and Earls (1997) [V]: surveying 8,782 residents of 343 Chicago neighbourhood clusters, *social cohesion and trust* (five items: neighbours willing to help; close-knit; can be trusted; do not get along (reversed); do not share values (reversed)) and *informal social control* (five items on the likelihood that neighbours would intervene: children skipping school, graffiti, disrespect to an adult, a fight, the closing of a fire station) were so highly correlated at the neighbourhood level that they were combined into *collective efficacy*, which mediated much of the association of concentrated disadvantage and residential instability with violence.
- **Institutional trust.** The OECD *Guidelines on Measuring Trust* (2017) [V] recommend an 11-point (0–10) scale, core questions on interpersonal trust and on trust in institutions (parliament, police, civil service, courts, local government), and modules on trust drivers; the OECD's updated guidelines on trust and its drivers in public institutions (2026) [V; contents not read] add the drivers model of the OECD Trust Survey (responsiveness, reliability, integrity, openness, fairness).
- **Procedural justice and legitimacy.** Sunshine and Tyler (2003) [V] and Tyler (2003) [V]: perceived procedural fairness predicts legitimacy, which predicts cooperation and compliance, more than perceived effectiveness does; Reisig, Bratton and Gertz (2007) [V] found that the legitimacy scale mixes obligation to obey with trust and proposed refinements; Tankebe (2013) [V] distinguishes lawfulness, distributive fairness, procedural fairness and effectiveness as dimensions of legitimacy.

**For the design.** Each of these is a *latent index with a published item set*; the studio should hold the item sets with their sources and scale structures, fit the declared measurement model to any survey that used them, and never accept a single item as the index without saying so (SDG 16.1.4 is explicitly a single item).

#### 3.5 Small-Area Estimates of Attitudes

Surveys of attitudes are rarely designed for neighbourhoods. Two estimators exist. Area-level: Fay–Herriot on direct area estimates with their design variances (already in peira `small_areas`). Unit-level: **multilevel regression and poststratification** (MRP; Park, Gelman and Bafumi 2004 [V]): a multilevel model of individual responses on demographic cells and area-level predictors, whose predictions are weighted by census counts of each cell in each area. Lax and Phillips (2009) [V] found MRP more accurate than disaggregation for US states; Buttice and Highton (2013) [V] found its performance highly variable with conventional national surveys, depending on the strength of the area-level predictors (the same dependence as Fay–Herriot on its covariates). Eurostat's 2019 guidelines (§1.5) [S] apply the same logic to city statistics. **Checks:** on synthetic populations with known area means; the poststratification identity (estimates sum to the national direct estimate when the model is saturated).

**For the design.** MRP is the estimator for "where the data do not hold it" for any survey-based qualitative index, with the area-level predictors (deprivation, tenure, residential stability, crime) supplied by the observed quantitative indexes; its validation is out-of-sample at areas with large samples, and its refusal rule is the posterior standard deviation or a cross-validated error larger than a declared fraction of the between-area spread.

#### 3.6 Expert Rubrics and Their Reliability

Expert ratings (design quality, institutional capacity, a risk rating) are measurements with raters as instruments. **Agreement:** Krippendorff's α = 1 − D_o/D_e works for any number of raters, missing ratings and nominal, ordinal, interval or ratio metrics; values of at least 0.80 are taken as reliable and 0.667–0.80 as tentative (Hayes and Krippendorff 2007 [V]; Krippendorff, *Content Analysis* [M]); intraclass correlations (Shrout and Fleiss 1979 [V]) serve interval ratings; nomos already computes the agreement of coders. **Models:** the Varieties of Democracy project scores latent country-year traits from ordinal expert ratings with a Bayesian IRT model in which each coder has their own thresholds and reliability, producing posterior intervals and correcting systematic leniency (Marquardt and Pemstein 2018 [V]; Pemstein, Meserve and Melton 2010 [V] for combining regime-type measures). Anchoring vignettes (§3.3) serve raters as well as respondents.

**For the design.** A rubric score entered in the studio should record the rater, the rubric's anchors and its version; with two or more raters, α is computed and the index refused or marked tentative below threshold; with several raters across many units, a rater-specific ordinal IRT gives each unit a latent score with an interval.

#### 3.7 Systematic Social Observation and Street Audits

**SSO.** Sampson and Raudenbush (1999) [V] video-recorded about 23,000 street segments in 196 Chicago tracts and coded physical and social disorder with item response models (ecometrics, §3.2); observed disorder and crime were both explained by structural characteristics and collective efficacy, which weakened the broken-windows reading. **Audit instruments:** the Irvine–Minnesota Inventory (Day et al. 2006 [V]; reliability, Boarnet et al. 2006 [V]); PEDS, a segment-level pedestrian audit (Clifton, Livi Smith and Rodriguez 2007 [V]); MAPS, the Microscale Audit of Pedestrian Streetscapes, with route, segment, crossing and cul-de-sac sections (Millstein et al. 2013 [V]), whose scores explained physical activity beyond macro-scale walkability in four age groups (Cain et al. 2014 [V]). **Virtual audits:** audits on Google Street View agreed well with field audits for most items, less for small or temporary features (Rundle et al. 2011 [V]); Odgers et al. (2012) [V] found SSO by Street View reliable and cheap for children's neighbourhoods. Each instrument reports item-level inter-rater reliability (κ, ICC), so an audit score is a measurement with a known error.

#### 3.8 Street-View Imagery, Computer Vision and Perceived Qualities

Place Pulse collected pairwise judgements ("which place looks safer?") of street images; scores were derived from comparisons, revealing inequality of perceived safety within and across cities (Salesses, Schechtner and Hidalgo 2013 [V]); Streetscore trained a predictor of perceived safety (Naik et al. 2014 [V]); Place Pulse 2.0 extended it to over 110,000 images from 56 cities and six perceptual attributes with deep networks (Dubey et al. 2016 [V]); predicted changes in perceived safety over time were associated with the density of educated residents (Naik et al. 2017 [V]). Reviews: Biljecki and Ito (2021) [V], Kang et al. (2020) [V], Ito et al. (2024) [V]. **Critiques:** perceived safety from images diverges systematically from recorded crime ("perception bias", Zhang et al. 2021 [V]); perceptions vary with raters' demographics and personalities, and global models trained on pooled raters overestimate positive and underestimate negative perceptions compared with local respondents (Quintana et al. 2025 [V]); imagery dates differ from the periods modelled; models transfer imperfectly across cultures. A perceptual score from imagery is therefore an *estimate of what a particular rater population would say*, not of the construct measured by a resident survey.

**For the design.** Street-view perception models are proxies for a qualitative index, to be calibrated against the resident-survey index (§3.4–3.5) where both exist, with their rater population declared; where no survey exists, the studio should label them "perceived by remote raters" and not substitute them for the residents' index.

#### 3.9 Ordinal Typologies and Matrices

- **Arnstein's ladder** (Arnstein 1969 [V]): eight rungs in three groups (non-participation: manipulation, therapy; tokenism: informing, consultation, placation; citizen power: partnership, delegated power, citizen control). It is an ordinal scale of power, criticised for assuming that more control is always the goal and ignoring process (Tritter and McCallum 2006 [V]); the IAP2 spectrum is a practitioners' alternative [M].
- **Stakeholder salience** (Mitchell, Agle and Wood 1997 [V]): power, legitimacy and urgency, each present or absent, giving seven stakeholder classes whose salience rises with the number of attributes; Agle, Mitchell and Sonnenfeld (1999) [V] operationalised the attributes as rating scales. **Power–interest grids** (Ackermann and Eden 2011 [V]; Bryson 2004 [V]; Reed et al. 2009 [V] for a typology of stakeholder analysis methods) place stakeholders in a 2 × 2 of players, subjects, context setters and crowd.
- **Risk matrices.** A likelihood category × consequence category grid with colours. Cox (2008) [V] states three axioms (weak consistency, betweenness, consistent colouring) a matrix must meet to be coherent with quantitative risk, shows that for negatively correlated frequency and severity a matrix can be "worse than useless" (worse than random ranking), and notes range compression and ambiguity of category inputs; Thomas, Bratvold and Bickel (2014) [V] show that rankings depend on arbitrary scale design and that multiplying ordinal ranks is invalid; Duijm (2015) [V] recommends continuous colouring and against multiplying category numbers.

**For the design.** An ordinal typology is a quantity with an order and no metric: it may be compared, counted and modelled as an ordinal outcome, but not averaged or multiplied; a risk rating entered in the studio should be linted against Cox's axioms, and stored as its two category inputs with the category definitions, not as their product.

#### 3.10 Fuzzy-Set Calibration

Ragin's (2009) [V] *calibration* maps a raw measure to membership in a set by declared qualitative anchors: full membership, the crossover and full non-membership; the direct method maps the raw value's distance from the crossover to log-odds scaled so the anchors land at about 0.95, 0.5 and 0.05 (log-odds ±3), and applies the logistic function; the indirect method regresses researcher-assigned qualitative scores on the raw measure. Calibration is the opposite of normalisation: it encodes *substantive* thresholds ("a high-poverty neighbourhood") rather than the sample's spread. **Implementations:** R `QCA` (Duşa 2019 [V]), `SetMethods`; the infrastructure's zetesis `sets` holds the direct calibration.

#### 3.11 Qualitative Coding

Interviews, documents and observation are coded into categories by a codebook; reliability between coders is assessed (Krippendorff's α) where codes are to be counted, while reflexive thematic analysis (Braun and Clarke 2006 [V]) rejects coding reliability as a criterion; O'Connor and Joffe (2020) [V] give practical guidance on when intercoder reliability is appropriate. Coded frequencies can become indexes (share of passages expressing distrust per neighbourhood), with the passage as the unit and coding error modelled as misclassification. zetesis `coding` and tekmerion already hold codebooks, passages verified against documents and agreement.

#### 3.12 What It Would Mean for the Design

- **A qualitative index is a latent variable with a declared measurement model** (items or raters, link, thresholds), a *reliability* at the level reported, an *invariance status* across compared groups, and a *small-area estimator* (MRP or Fay–Herriot) for where it is not observed. Each of the three gives a refusal rule: low reliability, failed invariance for the comparison asked, or estimator error larger than a declared share of between-area spread.
- **The neighbourhood is the natural level** for survey-based indexes (ecometrics); cells are too small, districts too heterogeneous. Downward allocation of a qualitative index to cells should be refused unless a validated proxy model exists (street-view perception for perceived safety, calibrated against residents).
- **Ordinal outputs stay ordinal**: typologies, ladders and risk categories are stored with their category definitions and enter models as ordinal states or ordinal observations (§4.4), never as numbers to add.

### 4. Qualitative Quantities in Dynamical Models

#### 4.1 Fuzzy Cognitive Maps

**Definition.** A fuzzy cognitive map (Kosko 1986 [V]) is a signed, weighted digraph over concepts with activation A_i ∈ [0, 1] (or [−1, 1]); weights w_ji ∈ [−1, 1] express how much concept j increases or decreases concept i, elicited from people as linguistic terms ("strongly increases") mapped to numbers. Inference iterates A(t+1) = f(Wᵀ A(t)) or the variant with self-memory f(Wᵀ A(t) + A(t)), with f a sigmoid 1/(1 + e^{−λx}), tanh, or a threshold. A "scenario" clamps some concepts and reads where the others settle.

**What the dynamics are.** The iteration is a discrete map with no time unit: a step is not a year. Linear FCMs have a unique fixed point or none, and their behaviour is set by the spectrum of W (Knight, Lloyd and Penn 2014 [V]); sigmoid FCMs whose λ‖W‖ is small enough are contractions and converge to a *unique fixed point regardless of the initial activation* (Boutalis, Kottas and Christodoulou 2009 [V]; Nápoles et al. 2016 [V]), so the "scenario" result depends on W and the clamped concepts alone; Concepción et al. (2021) [V] show the feasible state space shrinks with every iteration. Larger λ admits limit cycles and chaos that are artefacts of the transfer function rather than of the system described. Carvalho (2013) [V] criticises the semantics of FCMs in the social sciences: weights mix incommensurable concepts, and the iteration lacks a temporal interpretation.

**Use in participatory modelling.** Özesmi and Özesmi (2004) [V] give the multi-step method: individual maps from stakeholders, condensed and aggregated (by adding adjacency matrices), with graph indices (density, centrality, transmitters, receivers) used to compare groups' mental models; Mental Modeler (Gray et al. 2013 [V]) is the common tool; Jetter and Kok (2014) [V] assess FCMs for futures studies, and Voinov et al. (2018) [V] place them among participatory modelling tools. Urban uses: Olazabal and Pascual (2016) [V] used FCMs to study urban resilience and low-carbon transformation with stakeholders; Pluchinotta, Esposito and Camarda (2019) [V] used them to support multi-agent decisions in urban policymaking. Learning weights from data (active Hebbian learning, Papageorgiou, Stylios and Groumpos 2004 [V]; evolutionary learning, Stach et al. 2005 [V]) estimates n² parameters from short series, so W is weakly identified. Reviews: Papageorgiou and Salmeron (2013) [V]; Felix et al. (2019) [V]. **Implementation:** `fcmpy` (Mkhitaryan et al. 2022 [V]).

**What they can claim.** An FCM elicited from people is a valid record of *what those people believe the causal structure is*, comparable across groups; its fixed points are statements about the consequences of that belief under an arbitrary squashing function. It cannot claim magnitudes, time paths or forecasts. The honest uses are structural: which concepts a press on one concept reaches and with which sign, whether different stakeholder groups' maps disagree on a link's sign, and how robust a sign is to the weights' fuzziness (§4.3's qualitative network models do this properly).

#### 4.2 Causal Loop Diagrams and System Dynamics

**From diagram to model.** A causal loop diagram records polarities of influence and the feedback loops they form (reinforcing or balancing); system dynamics turns it into stocks, flows and auxiliary functions, i.e. ordinary differential equations (Forrester's *Urban Dynamics*, 1969 [M]; Sterman, *Business Dynamics*, 2000 [M]). Richardson (1986) [V] lists the problems of CLDs: a link's polarity is ambiguous between a flow and a stock, and loop polarity alone does not determine behaviour, so behaviour cannot be read from a diagram; Lane (2008) [V] gives a critical history of diagramming in SD; Coyle (2000) [V] asked whether qualitative SD can suffice where quantification is dubious, and Homer and Oliva (2001) [V] answered that maps without simulation mislead about dynamics. *Urban Dynamics* modelled a city as stocks of housing, businesses and population classes; its policy conclusions were contested from the start (Alfeld 1995 [V] is a retrospective).

**Calibration and validation.** Barlas (1996) [V] distinguishes structure-oriented tests (direct structure tests; structure-oriented behaviour tests such as extreme conditions and sensitivity) from behaviour-pattern tests (matching the pattern of the record, not point values); Oliva (2003) [V] treats partial-model calibration, fitting sub-models to their own data, as a testing strategy that exposes structural errors. These are the standards a model whose stocks include "trust" or "legitimacy" should meet.

#### 4.3 Qualitative Simulation and Sign-Structured Models

**QSIM.** Kuipers (1986) [V]: a *qualitative differential equation* gives each variable a quantity space of ordered landmark values and constrains variables by qualitative relations: ADD, MULT, MINUS, DERIV, and monotone functions M⁺ and M⁻ (an unknown increasing or decreasing function). A qualitative state is each variable's position (at a landmark or between two) and direction (increasing, steady, decreasing); QSIM generates the tree of all qualitative behaviours consistent with the constraints. Its guarantee is **soundness**: every behaviour of every ODE consistent with the QDE is among the predicted behaviours; it is not complete: some predicted behaviours are spurious (chatter, behaviours no real ODE produces), which later filters reduce. Related formalisms: qualitative process theory with direct and indirect influences (Forbus 1984 [V]) and confluences (de Kleer and Brown 1984 [V]); Garp3 is a workbench used in ecology and teaching (Bredeweg et al. 2009 [V]); Kuipers's book *Qualitative Reasoning* (MIT Press, 1994) [M].

**Loop analysis and qualitative network models.** For a community matrix known only by signs, Levins's loop analysis (Puccia and Levins 1985 [M]) predicts the sign of each variable's equilibrium response to a sustained press from the adjoint of the matrix; where terms of opposite sign compete, the prediction is indeterminate, and Dambacher et al. (2002) [V] weight the predictions by the share of terms agreeing. Melbourne-Thomas et al. (2012) [V] simulate: sample magnitudes consistent with the signs, keep only stable matrices, and report the proportion of samples in which each response is positive (R `QPress`).

**For the design.** This is the honest home of qualitative causal knowledge: a declared signed structure ("procedural justice raises trust; trust raises reporting; reporting raises recorded crime"), with magnitudes unknown, yields either QSIM's set of possible behaviours (sound) or QPress's probability of each response's sign (under declared sampling of magnitudes). Both are certificates in the sense the infrastructure already uses (a verdict with its evidence), and both say "indeterminate" where they should.

#### 4.4 Ordinal and Latent State-Space Models

**Structure.** A latent continuous state x_t (trust in district j, perceived safety, legitimacy) follows a declared law, x_{t+1} = a + φ x_t + B u_t + η_t (or any kinesis declaration), and is observed only through ordinal responses, y_{itk} with P(y ≤ k) = Φ((τ_k − λ x_{j(i),t})/σ), or through survey marginals by group. With probit links, Albert and Chib's (1993) [V] data augmentation makes the latent responses Gaussian given the data, so forward filtering and backward sampling or HMC apply; with logit links a particle filter does.

**Precedents.** *Dynamic ideal points* with random-walk priors (Martin and Quinn 2002 [V]); *dynamic group-level IRT* estimating latent opinion of groups (states, demographic cells) by year from many survey items, each item with its own difficulty and discrimination, with a hierarchical model across groups and a dynamic linear model over time (Caughey and Warshaw 2015 [V]; R `dgo` [M]); *smooth country–year panels* of latent opinion from heterogeneous survey items with item-by-country biases (Claassen 2019 [V]); *pooling the polls* with house effects as a state-space model (Jackman 2005 [V]). Fariss (2014) [V] showed that a latent trend can be an artefact of a changing *standard* of measurement (coders becoming stricter over time), and modelled the standard explicitly: a dynamic analogue of measurement non-invariance.

**Identification.** The latent scale is identified only up to an affine map (location and unit), fixed by anchor items assumed invariant over time and groups. Quantities invariant under the affine map are robust: the persistence φ and the half-life ln 2/(−ln φ) of a linear law, signs of responses, the timing of a change, the ordering of groups under scalar invariance. Levels, absolute changes and comparisons with an outside quantity require the anchors to hold.

**For the design.** This is the right coupling between a qualitative index and a dynamical model: the model's state is the latent quantity, its observation equation is the measurement model of §3, survey waves at irregular times are observations, and the fast–slow structure of the hierarchy enters as different φ at different levels. The infrastructure has Gaussian state-space filters (tyche) and kinesis's declarations; the missing piece is the ordinal observation equation.

#### 4.5 Regimes, Hidden Markov Models and Neighbourhood Change

Hamilton's (1989) [V] Markov-switching model, with discrete regimes and regime-dependent dynamics, is the canonical regime model (tyche already holds HMMs, as used in the Economy Atlas's regimes). In urban research, neighbourhood trajectories are classified into types by census variables and their sequences of types modelled as Markov chains or clustered by sequence analysis (Delmelle 2016 [V]); spatial Markov chains condition a unit's transition probabilities on its neighbours' class (Rey 2001 [V]; PySAL `giddy`), a direct coupling between a unit and its surroundings. Tipping in neighbourhoods' ethnic composition (Card, Mas and Rothstein 2008 [V]) is a regime boundary estimated by a discontinuity. For the design: a neighbourhood's regime (stable, gentrifying, declining) is an ordinal or nominal latent state with an HMM readout from quantitative indexes; its transition matrix may depend on the district-level state (downward coupling) and its occupancy shares aggregate upward exactly (counts of neighbourhoods by regime).

#### 4.6 Opinion Dynamics, Trust and Legitimacy

**Opinion dynamics.** DeGroot's (1974) [V] averaging x(t+1) = W x(t) reaches consensus when W is irreducible and aperiodic; Friedkin and Johnsen (1990) [V] add susceptibility Λ and anchoring to initial opinions, x(t+1) = Λ W x(t) + (I − Λ) x(0), whose equilibrium x* = (I − ΛW)⁻¹(I − Λ) x(0) sustains disagreement in closed form; bounded-confidence models (Deffuant et al. 2000 [V]; Hegselmann and Krause 2002 [V]) give clusters whose number scales roughly as 1/(2ε) with the confidence bound ε. Reviews: Castellano, Fortunato and Loreto (2009) [V]; Flache et al. (2017) [V], who stress that most models are not empirically validated; Peralta, Kertész and Iñiguez (2025, from models to data) [V]. Friedkin et al. (2016) [V] fit belief-system dynamics under logic constraints to small-group experiments, one of the few direct validations.

**Trust.** Slovic's (1993) [V] *asymmetry principle*: trust is built slowly and destroyed quickly because negative events are more visible, more heavily weighted and more credible. Later experiments qualified it: the asymmetry depends on prior attitudes, people discounting information that contradicts their priors whichever its sign (White et al. 2003 [V]; Poortinga and Pidgeon 2004 [V]). A dynamical model should therefore estimate the asymmetry (separate gain and loss rates of trust in response to good and bad events, possibly depending on the current level), not build it in. **Legitimacy.** In Epstein's (2002) [V] agent-based model of civil violence, grievance = hardship × (1 − legitimacy), legitimacy being an exogenous parameter; endogenising it (legitimacy updated by procedural justice experienced in encounters, §3.4) is the natural extension and the one that needs data.

#### 4.7 Place Attachment Over Residence Time

Kasarda and Janowitz's (1974) [V] *systemic model*: length of residence, through local social ties, drives community attachment, while size and density matter little; Sampson (1988) [V] confirmed it in a multilevel model with community-level residential stability. A simple dynamical reading is relaxation toward a ceiling, dA/dt = (A* − A)/τ, for those who stay, with an exit hazard decreasing in A. Cross-sectional gradients of attachment on residence length then overstate within-person growth, because the less attached have left (selection); identifying τ needs panels or retrospective histories with movers. The hierarchy matters: attachment differs by scale (home, neighbourhood, city; §3.4), so a city-level attachment index is not the mean of neighbourhood indexes.

#### 4.8 Keeping Such Models Honest

Numerical models of open systems can be confirmed, never verified (Oreskes, Shrader-Frechette and Belitz 1994 [V]). Calibration should include a *model discrepancy* term, without which parameters absorb structural error (Kennedy and O'Hagan 2001 [V]); *history matching* rules out regions of parameter space whose implausibility (distance between simulation and record relative to all declared uncertainties) exceeds a cutoff, rather than seeking a best fit, and reports the non-implausible region (Vernon, Goldstein and Bower 2010 [V]); many parameter sets fit equally well (equifinality, Beven 2006 [V]); *pattern-oriented modelling* requires a model to reproduce several patterns at different scales at once (Grimm et al. 2005 [V]); profile likelihoods diagnose structural and practical non-identifiability (Raue et al. 2009 [V]); Saltelli et al. (2020) [V] give five rules for models that serve society (mind the assumptions, the hubris, the framing, the consequences, the unknowns).

#### 4.9 What It Would Mean for the Design

- **Three tiers of qualitative dynamics, by what they claim:** (a) *signed structure only* → QSIM behaviours or QPress sign probabilities (sound or with declared sampling); (b) *elicited weights* → FCM fixed points reported as consequences of stated beliefs, never as forecasts; (c) *measured latent states* → ordinal state-space models fitted to survey waves, reporting affine-invariant quantities (half-lives, signs, timing) with intervals.
- **Every qualitative state variable needs an observation equation** linking it to a measurement model (§3) or is flagged as "declared, not measured", in which case the model may run only with a declared range (the brief's refusal rule).
- **Calibration by history matching** suits these models better than point fitting: it returns the set of trust or legitimacy dynamics consistent with the records and their errors, which may be wide; the studio should show it.

### 5. Missing Indicators and the Presentation of Uncertainty

#### 5.1 Coverage Rules

Frameworks decide first whether a unit has enough data to be scored at all.

- **Global Innovation Index** (JRC statistical audit of the 2026 GII, read this session [V]): an economy is included only with at least 66% data availability in each of the two sub-indices (36 of 54 input and 17 of 25 output variables) and at least two of three sub-pillars computable in each pillar; the rule, adopted in 2016, sharply reduced the number of economies with very wide rank intervals.
- **SDG Index** (SDSN, 2023 methodology [S]): countries with data for at least 80% of indicators, or 75% if in previous editions; missing data generally not imputed "due to the lack of widely accepted statistical models for imputing country-level data"; the 2026 edition switched to a headline set and excludes countries missing more than two of its indicators [S].
- **WGI**: an estimate is produced for any country covered by at least one source, but its standard error grows as sources thin, so coverage is expressed as uncertainty rather than exclusion (Kaufmann, Kraay and Mastruzzi 2011 [V]).
- **City frameworks.** ISO 37120 makes core indicators mandatory for conformance and certification levels depend on how many indicators are reported [S]; the CPI separates a Basic version of commonly available indicators for comparison from an Extended version not meant for comparison [V]; the CRI avoids a single number altogether [V]. A World Bank study of global aggregates finds a trade-off: the lower the coverage threshold for publishing an aggregate, the less accurate it tends to be [S].

#### 5.2 Imputation in Composites

The OECD/JRC handbook (2008 [V]) and the JRC's ten-step guide (step 3 [S]) distinguish case deletion; single imputation (unconditional mean or median, hot deck, regression, expectation–maximisation); and multiple imputation, whose between-imputation variance enters the composite's uncertainty by Rubin's rules (R `mice`, van Buuren and Groothuis-Oudshoorn 2011 [V]; peira already holds Rubin's rules). The GII's practice is instructive: it does not impute; an aggregate with a missing component is computed from its observed components, which the audit calls a form of "shadow" imputation (the missing value implicitly takes the mean of the others), and the JRC tests EM and k-nearest-neighbour imputation as alternatives inside its uncertainty analysis [V]. The lesson generalises: *not imputing is itself an imputation*, and the choice belongs among the uncertain inputs of §2.13.

#### 5.3 Presenting Uncertainty

- **Rank intervals.** The JRC audit of the 2026 GII runs 5,000 Monte Carlo simulations over fifteen model combinations (weights perturbed around reference values, arithmetic vs. generalised (geometric) mean, three treatments of missing data) and reports each economy's median simulated rank with its 90% interval; all published ranks lie within their intervals, and economies with intervals wider than 20 positions are flagged for caution [V]. Weights are treated as scaling coefficients, and importance is checked ex post with the correlation ratio, identifying indicators that explain less than 9% of their sub-pillar's variance [V].
- **Standard errors and overlap.** WGI publishes standard errors and 90% intervals with each estimate and warns that most differences between neighbouring countries are not significant [V for the 2011 article; specific presentation from memory].
- **Modelled versus reported.** SDG 11.6.2's metadata distinguishes WHO's modelled estimates (with uncertainty intervals, integrating satellite and model data) from national monitor means and explains their differences [V]; SDG 11.3.1's metadata warns that the ratio cannot be interpreted without its two rates and that averaging it over cities is ambiguous [V].
- **Dashboards and categories.** The SDSN's dashboards colour each indicator by distance to a threshold; the CPI's documentation scores on 0–100 with benchmarks from world data [V]; such categorical displays hide the within-category uncertainty and should carry the underlying interval.
- **Small areas.** Eurostat's 2019 guidelines require small-area estimates for cities and FUAs to be published with mean squared error estimates [S]; peira's Fay–Herriot carries the EBLUP's second-order MSE.

#### 5.4 What It Would Mean for the Design

The brief's "refuse to estimate and make it a declared quantity" has three precedents to borrow: a **coverage rule** per index (the GII's 66% per sub-index; the SDSN's 80%), a **reliability or error rule** per estimate (neighbourhood reliability, §3.2; estimator RMSE relative to between-area spread, §3.5), and a **comparability rule** (invariance, §3.3; same unit system, §1.7). An index that fails any rule becomes a declared quantity with a range, entered by a person and marked as such in every downstream output; the composite's uncertainty analysis then treats declared ranges, imputations and construction choices alike as uncertain inputs, and reports intervals of levels and ranks.

### 6. Recommendations

Ranked by value to the design; each line says where it would go and what its checks could be held to.

1. **Algebra tags and sufficient statistics on every index** (§2.1, §2.14). *Where:* the urban domain's index catalogue and the studio's hierarchy coupling. *Checks:* exact within/between identities for Theil T/L, segregation H and M, Hill numbers (to rounding); refusal when a `recompute` index is asked to aggregate; shift-share identity at every level.
2. **Ordinal measurement models with invariance and reliability** (cumulative link, graded response, ordinal CFA on polychorics; configural/metric/scalar tests; alignment; anchoring vignettes) (§3.1–3.3). *Where:* a measurement module beside nomos `psychometrics` and tyche's factor analysis. *Checks:* `mirt`, `lavaan`/`semTools`, `ordinal` on public datasets; recovery of simulated thresholds and DIF.
3. **Ecometric neighbourhood reliability and shrinkage** (§3.2). *Where:* the same measurement module, sharing peira's Fay–Herriot. *Checks:* closed-form λ_j; equality of the EB estimate with the FH EBLUP in the balanced Gaussian case; published ICCs (Mujahid et al. 2007) as order-of-magnitude references.
4. **MRP for survey-based qualitative indexes in small areas** (§3.5). *Where:* peira (beside `small_areas`), with area predictors from the urban domain. *Checks:* synthetic populations with known area means; the saturated-model poststratification identity; agreement with `brms`/`rstanarm` fits.
5. **Ordinal-observation state-space models of latent qualitative states** (dynamic group-level IRT) (§4.4). *Where:* kinesis (declared law) with an ordinal observation equation in tyche's filters (data augmentation or particle filter). *Checks:* recovery of φ and half-lives on simulated waves; reduction to the Kalman filter as categories → ∞; invariance of reported half-lives under affine rescaling of the latent scale.
6. **Composite indicator uncertainty and sensitivity engine** (§2.13, §5.3): Monte Carlo over imputation, normalisation, weights, aggregation and declared ranges; median ranks with 90% intervals; Sobol' first-order and total indices; importance vs. nominal weights; Becker's weight optimisation. *Where:* nomos `measurement` (composite indicators) with tyche `uncertainty`. *Checks:* closed forms for linear composites of independent indicators (S_i = w_i² V_i / Σ_j w_j² V_j); COINr on its example data.
7. **Coverage, reliability and comparability rules as refusals** (§5.4). *Where:* the studio and the urban domain's index catalogue. *Checks:* constructed cases just above and below each threshold; that "no imputation" is reported as shadow imputation.
8. **Spatial segregation profiles** (Reardon–O'Sullivan kernel environments, H̃ over bandwidths) with randomisation nulls (§2.2). *Where:* tyche `spatial` or the urban domain. *Checks:* PySAL `segregation` and R `segregation` values on their bundled data; H̃ → aspatial H as the bandwidth → 0 on unit-sized cells.
9. **SDG 11 indicators exactly to their metadata** (11.2.1, 11.3.1 with secondary indicators and both rate conventions, 11.6.2, 11.7.1 with the 400 m companion) on DEGURBA urban centres (§1.4). *Where:* the urban domain. *Checks:* hand-computed toy cities; GHSL-published LCRPGR values for selected cities (Schiavina et al. 2022); the metadata's warnings reproduced as refusals (zero or negative population growth).
10. **DEGURBA and FUA delineation on grids and commuting tables** (§1.1). *Where:* geography or the urban domain, on physis's grids. *Checks:* GHS-SMOD's published classes for tiles; four- versus eight-contiguity cases by hand; Eurostat's FUA lists for a country.
11. **Rater models for expert rubrics** (Krippendorff's α gate; ordinal IRT with rater-specific thresholds) (§3.6). *Where:* the measurement module, using nomos's agreement of coders. *Checks:* the `krippendorff` Python package and R `irr`; recovery of simulated rater severity.
12. **Qualitative network models and qualitative simulation** (QPress-style sign probabilities under sampled magnitudes; QSIM behaviour trees with soundness) (§4.3). *Where:* kinesis. *Checks:* adjoint-sign predictions by closed form on small matrices; `QPress` outputs; Kuipers's textbook examples (the bathtub, the U-tube) giving the published behaviour trees.
13. **Fuzzy cognitive maps with honest diagnostics** (contraction test, fixed-point uniqueness, sensitivity of signs to weight fuzziness; group comparison of elicited maps) (§4.1). *Where:* kinesis (as maps) and the studio (elicitation). *Checks:* Knight et al.'s fixed-point results; `fcmpy` on its examples; uniqueness under λ‖W‖ < bound.
14. **Thermal comfort indices at cell–hour resolution with exposure aggregation** (T_mrt six-directional, UTCI polynomial, PET, WBGT by Liljegren) (§2.8). *Where:* meteorology (indices) and urban science (exposure). *Checks:* `pythermalcomfort` and `thermofeel`; the UTCI reference code's published test values; T_mrt = T_a in the isothermal enclosure.
15. **Unobserved-components aggregation of heterogeneous sources** with shared-origin correction, for governance, legitimacy and institutional capacity (§2.12). *Where:* tyche `latent` with tekmerion's shared-origin rule. *Checks:* closed-form Gaussian posterior and standard errors; reproduction of a published WGI-style toy example.
16. **History matching for qualitative and latent dynamics** (implausibility, non-implausible regions, model discrepancy) (§4.8). *Where:* kinesis's fitting tools beside least squares. *Checks:* the one-dimensional examples of Vernon et al. (2010); coverage of the true parameter in simulation.
17. **Economic complexity of urban activity** (ECI and fitness on unit × activity matrices) (§2.10). *Where:* nomos or economics. *Checks:* Harvard Growth Lab's `ecomplexity`; the equivalence of ECI with the Fiedler vector of the similarity graph (Mealy et al. 2019) as an exact identity.
18. **Trust and legitimacy dynamics with estimated asymmetry**, and Friedkin–Johnsen opinion dynamics (§4.6). *Where:* kinesis catalogue. *Checks:* the closed-form FJ equilibrium (I − ΛW)⁻¹(I − Λ)x(0); recovery of separate gain and loss rates in simulation.
19. **Procurement corruption-risk index from open contracting data** (§2.11). *Where:* the urban domain (municipal finance) with tekmerion for evidence. *Checks:* red-flag definitions on synthetic contracts; reproduction of single-bidding regressions on a public dataset (published coefficients as order-of-magnitude references).
20. **A risk-matrix linter** (Cox's axioms; storage of category inputs, not products) (§3.9). *Where:* the studio. *Checks:* Cox's (2008) counterexamples reproduced.

### 7. Bibliography

Markers: [V] verified this session (Crossref, arXiv, doi.org resolution to the publisher's record, or the issuing body's page read); [S] secondary sources only; [M] memory. Volume and pages as the primary record gave them.

#### 7.1 Frameworks, Standards and Official Metadata

- Arup and the Rockefeller Foundation. City Resilience Index. https://www.arup.com/insights/city-resilience-index/ [V]
- CITYkeys (2016). D1.4 Indicators for smart city projects and smart cities. Horizon 2020 project 646440, https://cordis.europa.eu/project/id/646440 [S]
- European Commission and Eurostat (2021). Applying the Degree of Urbanisation: a methodological manual to define cities, towns and rural areas for international comparisons. doi:10.2785/706535 [V, resolved to the Publications Office record KS-02-20-499]
- Eurostat (2017). Methodological manual on city statistics. doi:10.2785/708009 [V, resolved to KS-GQ-17-006]
- Eurostat (2019). Guidelines on small area estimation for city statistics and other functional geographies. KS-GQ-19-011, https://ec.europa.eu/eurostat/web/products-manuals-and-guidelines/-/ks-gq-19-011 [S]
- Eurostat. City statistics (Urban Audit), reference metadata. https://ec.europa.eu/eurostat/cache/metadata/en/urb_esms.htm [S]
- Eurostat. Applying the degree of urbanisation manual: methodology for level 1. https://ec.europa.eu/eurostat/statistics-explained/index.php/Applying_the_degree_of_urbanisation_manual_-_Methodology_for_applying_level_1_of_the_degree_of_urbanisation_classification [S]
- ISO 37120:2018. Sustainable cities and communities: indicators for city services and quality of life. https://www.iso.org/standard/68498.html [S]; World Council on City Data, About ISO 37120, https://www.dataforcities.org/iso-37120 [V]
- ISO 37122:2019 (smart cities), https://www.iso.org/standard/69050.html; ISO 37123:2019 (resilient cities), https://www.iso.org/standard/70428.html; ISO 37124:2024 (guidance), https://www.iso.org/standard/80290.html [S]
- JRC (2026). Appendix II: JRC statistical audit of the 2026 Global Innovation Index. WIPO, https://www.wipo.int/web-publications/global-innovation-index-2026/en/appendix-ii-joint-research-centre-jrc-statistical-audit-of-the-2026-global-innovation-index.html [V]
- JRC Competence Centre on Composite Indicators. 10-step guide, step 3: imputation of missing data. https://knowledge4policy.ec.europa.eu/composite-indicators/10-step-guide/step-3-imputation-missing-data_en [S]
- OECD (2012). Redefining "Urban": A New Way to Measure Metropolitan Areas. doi:10.1787/9789264174108-en [V]
- OECD (2017). OECD Guidelines on Measuring Trust. doi:10.1787/9789264278219-en [V]
- OECD (2026). Updated OECD Guidelines on Measuring Trust and Trust Drivers in Public Institutions. doi:10.1787/cdc4c319-en [V record; contents not read]
- OECD, European Union and JRC (2008). Handbook on Constructing Composite Indicators: Methodology and User Guide. doi:10.1787/9789264043466-en [V]
- SDSN. Sustainable Development Report, methodology. https://dashboards.sdgindex.org/chapters/methodology [S]
- UN-Habitat (2016). City Prosperity Index: methodology and metadata. https://unhabitat.org/sites/default/files/2019/02/CPI-METADATA.2016.pdf [V, read]
- UN-Habitat (2022). Global Urban Monitoring Framework endorsed by the UN Statistical Commission (news, 7 March 2022). https://unhabitat.org/news/07-mar-2022/global-urban-monitoring-framework-endorsed-by-the-un-statistical-commission [V]; metadata volume, https://unhabitat.org/sites/default/files/2022/08/the_global_urban_monitoring_framework_metadata.pdf [S]
- UN Statistics Division. SDG indicator metadata 11.1.1, 11.2.1, 11.3.1, 11.6.2, 11.7.1 and 16.1.4 (last updated 2025-04-23). https://unstats.un.org/sdgs/metadata/files/Metadata-11-03-01.pdf (and the corresponding files) [V, read]
- UNDRR. Disaster Resilience Scorecard for Cities. https://mcr2030.undrr.org/quick/13215 [S]

#### 7.2 Units, Scaling and Critiques of Indicators

- Arcaute E, Hatna E, Ferguson P, Youn H, Johansson A, Batty M (2015). Constructing cities, deconstructing scaling laws. J. R. Soc. Interface 12(102):20140745. doi:10.1098/rsif.2014.0745 [V]
- Bettencourt LMA, Lobo J, Helbing D, Kühnert C, West GB (2007). Growth, innovation, scaling, and the pace of life in cities. PNAS 104(17). doi:10.1073/pnas.0610172104 [V]
- Bettencourt LMA (2013). The origins of scaling in cities. Science 340(6139):1438–1441. doi:10.1126/science.1235823 [V]
- Brussel M, Zuidgeest M, Pfeffer K, van Maarseveen M (2019). Access or accessibility? A critique of the urban transport SDG indicator. ISPRS Int. J. Geo-Inf. 8(2):67. doi:10.3390/ijgi8020067 [V]
- Cottineau C, Hatna E, Arcaute E, Batty M (2017). Diverse cities or the systematic paradox of urban scaling laws. CEUS 63:80–94. doi:10.1016/j.compenvurbsys.2016.04.006 [V]
- Cutter SL (2016). The landscape of disaster resilience indicators in the USA. Natural Hazards 80(2):741–758. doi:10.1007/s11069-015-1993-2 [V]
- Cutter SL, Burton CG, Emrich CT (2010). Disaster resilience indicators for benchmarking baseline conditions. JHSEM 7(1). doi:10.2202/1547-7355.1732 [V]
- Dijkstra L, Florczyk AJ, Freire S, Kemper T, Melchiorri M, Pesaresi M, et al. (2021). Applying the Degree of Urbanisation to the globe. J. Urban Econ. 125:103312. doi:10.1016/j.jue.2020.103312 [V]
- Fotheringham AS, Wong DWS (1991). The modifiable areal unit problem in multivariate statistical analysis. Environ. Plan. A 23(7):1025–1044. doi:10.1068/a231025 [V]
- Huovila A, Bosch P, Airaksinen M (2019). Comparative analysis of standardized indicators for smart sustainable cities. Cities 89:141–153. doi:10.1016/j.cities.2019.01.029 [V]
- Kaika M (2017). "Don't call me resilient again!": the New Urban Agenda as immunology … Environment and Urbanization 29(1):89–102. doi:10.1177/0956247816684763 [V]
- Leitão JC, Miotto JM, Gerlach M, Altmann EG (2016). Is this scaling nonlinear? R. Soc. Open Sci. 3(7):150649. doi:10.1098/rsos.150649 [V]
- Louf R, Barthelemy M (2014). Scaling: lost in the smog. Environ. Plan. B 41(5):767–769. doi:10.1068/b4105c [V]
- Melchiorri M, Pesaresi M, Florczyk AJ, Corbane C, Kemper T (2019). Principles and applications of the Global Human Settlement Layer as baseline for the land use efficiency indicator, SDG 11.3.1. ISPRS Int. J. Geo-Inf. 8(2):96. doi:10.3390/ijgi8020096 [V]
- Nicolau R, David J, Caetano M, Pereira JMC (2019; online 2018). Ratio of land consumption rate to population growth rate: analysis of different formulations applied to mainland Portugal. ISPRS Int. J. Geo-Inf. 8(1):10. doi:10.3390/ijgi8010010 [V]
- Schiavina M, Melchiorri M, Freire S, Florio P, Ehrlich D, Tommasi P, et al. (2022). Land use efficiency of functional urban areas: global pattern and evolution of development trajectories. Habitat International 123:102543. doi:10.1016/j.habitatint.2022.102543 [V]

#### 7.3 Segregation, Concentration, Diversity, Inequality

- Borusyak K, Hull P, Jaravel X (2022). Quasi-experimental shift-share research designs. Rev. Econ. Stud. 89(1):181–213. doi:10.1093/restud/rdab030 [V]
- Bourguignon F (1979). Decomposable income inequality measures. Econometrica 47(4):901. doi:10.2307/1914138 [V]
- Card D, Mas A, Rothstein J (2008). Tipping and the dynamics of segregation. QJE 123(1):177–218. doi:10.1162/qjec.2008.123.1.177 [V]
- Cervero R, Kockelman K (1997). Travel demand and the 3Ds: density, diversity, and design. Transp. Res. D 2(3):199–219. doi:10.1016/S1361-9209(97)00009-6 [V]
- Cortes RX, Rey S, Knaap E, Wolf LJ (2020). An open-source framework for non-spatial and spatial segregation measures: the PySAL segregation module. J. Comput. Soc. Sci. 3(1):135–166. doi:10.1007/s42001-019-00059-3 [V]
- Duncan OD, Duncan B (1955). A methodological analysis of segregation indexes. Am. Sociol. Rev. 20(2):210. doi:10.2307/2088328 [V]
- Dunn ES (1960). A statistical and analytical technique for regional analysis. Papers in Regional Science 6(1):97–112. doi:10.1111/j.1435-5597.1960.tb01705.x [V]
- Elbers B (2023). A method for studying differences in segregation across time and space. Sociol. Methods Res. 52(1):5–42. doi:10.1177/0049124121986204 [V]
- Ellison G, Glaeser EL (1997). Geographic concentration in U.S. manufacturing industries: a dartboard approach. JPE 105(5):889–927. doi:10.1086/262098 [V]
- Esteban J (2000). Regional convergence in Europe and the industry mix: a shift-share analysis. RSUE 30(3):353–364. doi:10.1016/S0166-0462(00)00035-1 [V]
- Goldsmith-Pinkham P, Sorkin I, Swift H (2020). Bartik instruments: what, when, why, and how. AER 110(8):2586–2624. doi:10.1257/aer.20181047 [V]
- Hill MO (1973). Diversity and evenness: a unifying notation and its consequences. Ecology 54(2):427–432. doi:10.2307/1934352 [V]
- Isserman AM (1977). The location quotient approach to estimating regional economic impacts. JAIP 43(1):33–41. doi:10.1080/01944367708977758 [V]
- Jost L (2007). Partitioning diversity into independent alpha and beta components. Ecology 88(10):2427–2439. doi:10.1890/06-1736.1 [V]
- Krugman P (1991). Geography and Trade. MIT Press. [M]
- Lee BA, Reardon SF, Firebaugh G, Farrell CR, Matthews SA, O'Sullivan D (2008). Beyond the census tract: patterns and determinants of racial segregation at multiple geographic scales. ASR 73(5):766–791. doi:10.1177/000312240807300504 [V]
- Massey DS, Denton NA (1988). The dimensions of residential segregation. Social Forces 67(2):281. doi:10.2307/2579183 [V]
- Mora R, Ruiz-Castillo J (2011). Entropy-based segregation indices. Sociol. Methodol. 41(1):159–194. doi:10.1111/j.1467-9531.2011.01237.x [V]
- Reardon SF, Firebaugh G (2002). Measures of multigroup segregation. Sociol. Methodol. 32(1):33–67. doi:10.1111/1467-9531.00110 [V]
- Reardon SF, O'Sullivan D (2004). Measures of spatial segregation. Sociol. Methodol. 34(1):121–162. doi:10.1111/j.0081-1750.2004.00150.x [V]
- Reardon SF, Matthews SA, O'Sullivan D, Lee BA, Firebaugh G, Farrell CR, et al. (2008). The geographic scale of metropolitan racial segregation. Demography 45(3):489–514. doi:10.1353/dem.0.0019 [V]
- Shorrocks AF (1980). The class of additively decomposable inequality measures. Econometrica 48(3):613. doi:10.2307/1913126 [V]
- Simpson EH (1949). Measurement of diversity. Nature 163(4148):688. doi:10.1038/163688a0 [V]
- Song Y, Merlin L, Rodriguez D (2013). Comparing measures of urban land use mix. CEUS 42:1–13. doi:10.1016/j.compenvurbsys.2013.08.001 [V]
- Theil H, Finizza AJ (1971). A note on the measurement of racial integration of schools by means of informational concepts. J. Math. Sociol. 1(2):187–193. doi:10.1080/0022250X.1971.9989795 [V]
- White MJ (1983). The measurement of spatial segregation. AJS 88(5):1008–1018. doi:10.1086/227768 [V]
- Wong DWS (1993). Spatial indices of segregation. Urban Studies 30(3):559–572. doi:10.1080/00420989320080551 [V]
- Wong DWS (2004). Comparing traditional and spatial segregation measures: a spatial scale perspective. Urban Geography 25(1):66–82. doi:10.2747/0272-3638.25.1.66 [V]

#### 7.4 Walkability, Design, Accessibility, Affordability, Thermal Comfort

- Błażejczyk K, Epstein Y, Jendritzky G, Staiger H, Tinz B (2012). Comparison of UTCI to selected thermal indices. Int. J. Biometeorol. 56(3):515–535. doi:10.1007/s00484-011-0453-2 [V]
- Bramley G (2012). Affordability, poverty and housing need: triangulating measures and standards. J. Housing Built Environ. 27(2):133–151. doi:10.1007/s10901-011-9255-4 [V]
- Brimicombe C, Di Napoli C, Quintino T, Pappenberger F, Cornforth R, Cloke HL (2022). Thermofeel: a python thermal comfort indices library. SoftwareX 18:101005. doi:10.1016/j.softx.2022.101005 [V]
- Bröde P, Fiala D, Błażejczyk K, Holmér I, Jendritzky G, Kampmann B, et al. (2012). Deriving the operational procedure for the Universal Thermal Climate Index (UTCI). Int. J. Biometeorol. 56(3):481–494. doi:10.1007/s00484-011-0454-1 [V]
- Budd GM (2008). Wet-bulb globe temperature (WBGT): its history and its limitations. J. Sci. Med. Sport 11(1):20–32. doi:10.1016/j.jsams.2007.07.003 [V]
- Carr LJ, Dunsiger SI, Marcus BH (2011). Validation of Walk Score for estimating access to walkable amenities. BJSM 45(14):1144–1148. doi:10.1136/bjsm.2009.069609 [V]
- Duncan DT, Aldstadt J, Whalen J, Melly SJ, Gortmaker SL (2011). Validation of Walk Score for estimating neighborhood walkability. IJERPH 8(11):4160. doi:10.3390/ijerph8114160 [V]
- Ewing R, Cervero R (2010). Travel and the built environment: a meta-analysis. JAPA 76(3):265–294. doi:10.1080/01944361003766766 [V]
- Ewing R, Handy S (2009). Measuring the unmeasurable: urban design qualities related to walkability. J. Urban Design 14(1):65–84. doi:10.1080/13574800802451155 [V]
- Ewing R, Handy S, Brownson RC, Clemente O, Winston E (2006). Identifying and measuring urban design qualities related to walkability. J. Phys. Act. Health 3(s1):S223–S240. doi:10.1123/jpah.3.s1.s223 [V]
- Frank LD, Schmid TL, Sallis JF, Chapman J, Saelens BE (2005). Linking objectively measured physical activity with objectively measured urban form. AJPM 28(2):117–125. doi:10.1016/j.amepre.2004.11.001 [V]
- Frank LD, Sallis JF, Saelens BE, Leary L, Cain K, Conway TL, et al. (2010). The development of a walkability index. BJSM 44(13):924–933. doi:10.1136/bjsm.2009.058701 [V]
- Geurs KT, van Wee B (2004). Accessibility evaluation of land-use and transport strategies. J. Transp. Geogr. 12(2):127–140. doi:10.1016/j.jtrangeo.2003.10.005 [V]
- Hansen WG (1959). How accessibility shapes land use. JAIP 25(2):73–76. doi:10.1080/01944365908978307 [V]
- Höppe P (1999). The physiological equivalent temperature: a universal index for the biometeorological assessment of the thermal environment. Int. J. Biometeorol. 43(2):71–75. doi:10.1007/s004840050118 [V]
- Hulchanski JD (1995). The concept of housing affordability: six contemporary uses of the housing expenditure-to-income ratio. Housing Studies 10(4):471–491. doi:10.1080/02673039508720833 [V]
- Jendritzky G, de Dear R, Havenith G (2012). UTCI: why another thermal index? Int. J. Biometeorol. 56(3):421–428. doi:10.1007/s00484-011-0513-7 [V]
- Kántor N, Unger J (2011). The most problematic variable in the course of human-biometeorological comfort assessment: the mean radiant temperature. Open Geosciences (formerly Cent. Eur. J. Geosci.) 3(1):90. doi:10.2478/s13533-011-0010-x [V]
- Liljegren JC, Carhart RA, Lawday P, Tschopp S, Sharp R (2008). Modeling the wet bulb globe temperature using standard meteorological measurements. J. Occup. Environ. Hyg. 5(10):645–655. doi:10.1080/15459620802310770 [V]
- Lindberg F, Holmer B, Thorsson S (2008). SOLWEIG 1.0: modelling spatial variations of 3D radiant fluxes and mean radiant temperature in complex urban settings. Int. J. Biometeorol. 52(7):697–713. doi:10.1007/s00484-008-0162-7 [V]
- Luo W, Wang F (2003). Measures of spatial accessibility to health care in a GIS environment. Environ. Plan. B 30(6):865–884. doi:10.1068/b29120 [V]
- Matzarakis A, Rutz F, Mayer H (2007). Modelling radiation fluxes in simple and complex environments: application of the RayMan model. Int. J. Biometeorol. 51(4):323–334. doi:10.1007/s00484-006-0061-8 [V]
- Purciel M, Neckerman KM, Lovasi GS, Quinn JW, Weiss C, Bader MDM, et al. (2009). Creating and validating GIS measures of urban design for health research. J. Environ. Psychol. 29(4):457–466. doi:10.1016/j.jenvp.2009.03.004 [V]
- Stone ME (2006). What is housing affordability? The case for the residual income approach. Housing Policy Debate 17(1):151–184. doi:10.1080/10511482.2006.9521564 [V]
- Tartarini F, Schiavon S (2020). pythermalcomfort: a Python package for thermal comfort research. SoftwareX 12:100578. doi:10.1016/j.softx.2020.100578 [V]
- Thorsson S, Lindberg F, Eliasson I, Holmer B (2007). Different methods for estimating the mean radiant temperature in an outdoor urban setting. Int. J. Climatol. 27(14):1983–1993. doi:10.1002/joc.1537 [V]
- Yaglou CP, Minard D (1957). Control of heat casualties at military training centers. AMA Arch. Ind. Health 16. [M]
- Yin L, Wang Z (2016). Measuring visual enclosure for street walkability: using machine learning algorithms and Google Street View imagery. Applied Geography 76:147–153. doi:10.1016/j.apgeog.2016.09.024 [V]

#### 7.5 Vulnerability, Complexity, Corruption Risk, Governance, Composites

- Alkire S, Foster J (2011). Counting and multidimensional poverty measurement. J. Public Econ. 95(7–8):476–487. doi:10.1016/j.jpubeco.2010.11.006 [V]
- Bakkensen LA, Fox-Lent C, Read LK, Linkov I (2017). Validating resilience and vulnerability indices in the context of natural disasters. Risk Analysis 37(5):982–1004. doi:10.1111/risa.12677 [V]
- Balland PA, Rigby D (2017). The geography of complex knowledge. Economic Geography 93(1):1–23. doi:10.1080/00130095.2016.1205947 [V]
- Balland PA, Jara-Figueroa C, Petralia SG, Steijn MPA, Rigby DL, Hidalgo CA (2020). Complex economic activities concentrate in large cities. Nat. Hum. Behav. 4(3):248–254. doi:10.1038/s41562-019-0803-3 [V]
- Becker W, Saisana M, Paruolo P, Vandecasteele I (2017). Weights and importance in composite indicators: closing the gap. Ecological Indicators 80:12–22. doi:10.1016/j.ecolind.2017.03.056 [V]
- Becker W, Caperna G, Del Sorbo M, Norlén H, Papadimitriou E, Saisana M (2022). COINr: an R package for developing composite indicators. JOSS 7(78):4567. doi:10.21105/joss.04567 [V]
- Charron N, Dijkstra L, Lapuente V (2014). Regional governance matters: quality of government within European Union member states. Regional Studies 48(1):68–90. doi:10.1080/00343404.2013.770141 [V]
- Cherchye L, Moesen W, Rogge N, Van Puyenbroeck T (2007). An introduction to "benefit of the doubt" composite indicators. Soc. Indic. Res. 82(1):111–145. doi:10.1007/s11205-006-9029-7 [V]
- Conlon KC, Mallen E, Gronlund CJ, Berrocal VJ, Larsen L, O'Neill MS (2020). Mapping human vulnerability to extreme heat: a critical assessment of heat vulnerability indices created using principal components analysis. EHP 128(9):097001. doi:10.1289/EHP4030 [V]
- Cutter SL, Boruff BJ, Shirley WL (2003). Social vulnerability to environmental hazards. Soc. Sci. Q. 84(2):242–261. doi:10.1111/1540-6237.8402002 [V]
- Fazekas M, Tóth IJ, King LP (2016). An objective corruption risk index using public procurement data. Eur. J. Crim. Policy Res. 22(3):369–397. doi:10.1007/s10610-016-9308-z [V]
- Fazekas M, Tóth IJ (2016). From corruption to state capture. Polit. Res. Q. 69(2):320–334. doi:10.1177/1065912916639137 [V]
- Fazekas M, Kocsis G (2020). Uncovering high-level corruption: cross-national objective corruption risk indicators using public procurement data. BJPS 50(1):155–164. doi:10.1017/S0007123417000461 [V]
- Flanagan BE, Gregory EW, Hallisey EJ, Heitgerd JL, Lewis B (2011). A social vulnerability index for disaster management. JHSEM 8(1). doi:10.2202/1547-7355.1792 [V]
- Greco S, Ishizaka A, Tasiou M, Torrisi G (2019). On the methodological framework of composite indices. Soc. Indic. Res. 141(1):61–94. doi:10.1007/s11205-017-1832-9 [V]
- Hidalgo CA, Klinger B, Barabási AL, Hausmann R (2007). The product space conditions the development of nations. Science 317(5837):482–487. doi:10.1126/science.1144581 [V]
- Hidalgo CA, Hausmann R (2009). The building blocks of economic complexity. PNAS 106(26):10570–10575. doi:10.1073/pnas.0900943106 [V]
- Hidalgo CA (2021). Economic complexity theory and applications. Nat. Rev. Phys. 3(2):92–113. doi:10.1038/s42254-020-00275-1 [V]
- Kaufmann D, Kraay A, Mastruzzi M (2010). The Worldwide Governance Indicators: methodology and analytical issues. World Bank Policy Research Working Paper 5430. doi:10.1596/1813-9450-5430 [V]
- Kaufmann D, Kraay A, Mastruzzi M (2011). The Worldwide Governance Indicators: methodology and analytical issues. Hague J. Rule Law 3(2):220–246. doi:10.1017/S1876404511200046 [V]
- Kurtz MJ, Schrank A (2007). Growth and governance: models, measures, and mechanisms. J. Politics 69(2):538–554. doi:10.1111/j.1468-2508.2007.00549.x [V]
- Langbein L, Knack S (2010). The Worldwide Governance Indicators: six, one, or none? J. Dev. Stud. 46(2):350–370. doi:10.1080/00220380902952399 [V]
- Mariani MS, Vidmer A, Medo M, Zhang YC (2015). Measuring economic complexity of countries and products: which metric to use? Eur. Phys. J. B 88(11):293. doi:10.1140/epjb/e2015-60298-7 [V]
- Mealy P, Farmer JD, Teytelboym A (2019). Interpreting economic complexity. Sci. Adv. 5(1):eaau1705. doi:10.1126/sciadv.aau1705 [V]
- Munda G, Nardo M (2009). Noncompensatory/nonlinear composite indicators for ranking countries: a defensible setting. Applied Economics 41(12):1513–1523. doi:10.1080/00036840601019364 [V]
- Paruolo P, Saisana M, Saltelli A (2013). Ratings and rankings: voodoo or science? JRSS A 176(3):609. doi:10.1111/j.1467-985X.2012.01059.x [V]
- Reid CE, O'Neill MS, Gronlund CJ, Brines SJ, Brown DG, Diez-Roux AV, et al. (2009). Mapping community determinants of heat vulnerability. EHP 117(11):1730. doi:10.1289/ehp.0900683 [V]
- Reid CE, Mann JK, Alfasso R, English PB, King GC, Lincoln RA, et al. (2012). Evaluation of a heat vulnerability index on abnormally hot days. EHP 120(5):715–720. doi:10.1289/ehp.1103766 [V]
- Rufat S, Tate E, Burton CG, Maroof AS (2015). Social vulnerability to floods: review of case studies and implications for measurement. IJDRR 14:470. doi:10.1016/j.ijdrr.2015.09.013 [V]
- Rufat S, Tate E, Emrich CT, Antolini F (2019). How valid are social vulnerability models? Ann. Am. Assoc. Geogr. 109(4):1131–1153. doi:10.1080/24694452.2018.1535887 [V]
- Saisana M, Saltelli A, Tarantola S (2005). Uncertainty and sensitivity analysis techniques as tools for the quality assessment of composite indicators. JRSS A 168(2):307. doi:10.1111/j.1467-985X.2005.00350.x [V]
- Saltelli A (2007). Composite indicators between analysis and advocacy. Soc. Indic. Res. 81(1):65–77. doi:10.1007/s11205-006-0024-9 [V]
- Saltelli A, Ratto M, Andres T, Campolongo F, Cariboni J, Gatelli D, Saisana M, Tarantola S (2008; Crossref dates the record 2007). Global Sensitivity Analysis: The Primer. Wiley. doi:10.1002/9780470725184 [V]
- Schmidtlein MC, Deutsch RC, Piegorsch WW, Cutter SL (2008). A sensitivity analysis of the social vulnerability index. Risk Analysis 28(4):1099–1114. doi:10.1111/j.1539-6924.2008.01072.x [V]
- Spielman SE, Tuccillo J, Folch DC, Schweikert A, Davies R, Wood N, Tate E (2020). Evaluating social vulnerability indicators: criteria and their application to the Social Vulnerability Index. Natural Hazards 100(1):417–436. doi:10.1007/s11069-019-03820-z [V]
- Tacchella A, Cristelli M, Caldarelli G, Gabrielli A, Pietronero L (2012). A new metrics for countries' fitness and products' complexity. Sci. Rep. 2:723. doi:10.1038/srep00723 [V]
- Tate E (2012). Social vulnerability indices: a comparative assessment using uncertainty and sensitivity analysis. Natural Hazards 63(2):325–347. doi:10.1007/s11069-012-0152-2 [V]
- Thomas MA (2010). What do the Worldwide Governance Indicators measure? Eur. J. Dev. Res. 22(1):31–54. doi:10.1057/ejdr.2009.32 [V]

#### 7.6 Measurement of Qualitative Indexes

- Ackermann F, Eden C (2011). Strategic management of stakeholders: theory and practice. Long Range Planning 44(3):179–196. doi:10.1016/j.lrp.2010.08.001 [V]
- Agle BR, Mitchell RK, Sonnenfeld JA (1999). Who matters to CEOs? An investigation of stakeholder attributes and salience, corporate performance, and CEO values. Acad. Manage. J. 42(5):507–525. doi:10.5465/256973 [V]
- Arnstein SR (1969). A ladder of citizen participation. JAIP 35(4):216–224. doi:10.1080/01944366908977225 [V]
- Asparouhov T, Muthén B (2014). Multiple-group factor analysis alignment. Struct. Equ. Modeling 21(4):495–508. doi:10.1080/10705511.2014.919210 [V]
- Biljecki F, Ito K (2021). Street view imagery in urban analytics and GIS: a review. Landsc. Urban Plan. 215:104217. doi:10.1016/j.landurbplan.2021.104217 [V]
- Boarnet MG, Day K, Alfonzo M, Forsyth A, Oakes M (2006). The Irvine–Minnesota Inventory to measure built environments: reliability tests. AJPM 30(2):153. doi:10.1016/j.amepre.2005.09.018 [V]
- Braun V, Clarke V (2006). Using thematic analysis in psychology. Qual. Res. Psychol. 3(2):77–101. doi:10.1191/1478088706qp063oa [V]
- Bryson JM (2004). What to do when stakeholders matter. Public Management Review 6(1):21–53. doi:10.1080/14719030410001675722 [V]
- Buckner JC (1988). The development of an instrument to measure neighborhood cohesion. Am. J. Community Psychol. 16(6):771–791. doi:10.1007/BF00930892 [V]
- Bürkner PC (2017). brms: an R package for Bayesian multilevel models using Stan. J. Stat. Softw. 80(1). doi:10.18637/jss.v080.i01 [V]
- Bürkner PC, Vuorre M (2019). Ordinal regression models in psychology: a tutorial. AMPPS 2(1):77–101. doi:10.1177/2515245918823199 [V]
- Buttice MK, Highton B (2013). How does multilevel regression and poststratification perform with conventional national surveys? Political Analysis 21(4):449–467. doi:10.1093/pan/mpt017 [V]
- Cain KL, Millstein RA, Sallis JF, Conway TL, Gavand KA, Frank LD, et al. (2014). Contribution of streetscape audits to explanation of physical activity in four age groups based on the Microscale Audit of Pedestrian Streetscapes (MAPS). Soc. Sci. Med. 116:82–92. doi:10.1016/j.socscimed.2014.06.042 [V]
- Chalmers RP (2012). mirt: a multidimensional item response theory package for the R environment. J. Stat. Softw. 48(6). doi:10.18637/jss.v048.i06 [V]
- Chen FF (2007). Sensitivity of goodness of fit indexes to lack of measurement invariance. Struct. Equ. Modeling 14(3):464–504. doi:10.1080/10705510701301834 [V]
- Clifton KJ, Livi Smith AD, Rodriguez D (2007). The development and testing of an audit for the pedestrian environment. Landsc. Urban Plan. 80(1–2):95–110. doi:10.1016/j.landurbplan.2006.06.008 [V]
- Cox LA Jr (2008). What's wrong with risk matrices? Risk Analysis 28(2):497–512. doi:10.1111/j.1539-6924.2008.01030.x [V]
- Day K, Boarnet M, Alfonzo M, Forsyth A (2006). The Irvine–Minnesota Inventory to measure built environments: development. AJPM 30(2):144–152. doi:10.1016/j.amepre.2005.09.017 [V]
- Dubey A, Naik N, Parikh D, Raskar R, Hidalgo CA (2016). Deep learning the city: quantifying urban perception at a global scale. ECCV 2016, LNCS. doi:10.1007/978-3-319-46448-0_12 [V]
- Duijm NJ (2015). Recommendations on the use and design of risk matrices. Safety Science 76:21–31. doi:10.1016/j.ssci.2015.02.014 [V]
- Duşa A (2019). QCA with R: A Comprehensive Resource. Springer. doi:10.1007/978-3-319-75668-4 [V]
- Farrall S, Bannister J, Ditton J, Gilchrist E (1997). Questioning the measurement of the "fear of crime". Br. J. Criminol. 37(4):658–679. doi:10.1093/oxfordjournals.bjc.a014203 [V]
- Ferraro KF, LaGrange R (1987). The measurement of fear of crime. Sociol. Inquiry 57(1):70–97. doi:10.1111/j.1475-682X.1987.tb01181.x [V]
- Flora DB, Curran PJ (2004). An empirical evaluation of alternative methods of estimation for confirmatory factor analysis with ordinal data. Psychol. Methods 9(4):466–491. doi:10.1037/1082-989X.9.4.466 [V]
- Gray E, Jackson J, Farrall S (2008). Reassessing the fear of crime. Eur. J. Criminol. 5(3):363–380. doi:10.1177/1477370808090834 [V]
- Hayes AF, Krippendorff K (2007). Answering the call for a standard reliability measure for coding data. Commun. Methods Meas. 1(1):77–89. doi:10.1080/19312450709336664 [V]
- Hidalgo MC, Hernández B (2001). Place attachment: conceptual and empirical questions. J. Environ. Psychol. 21(3):273–281. doi:10.1006/jevp.2001.0221 [V]
- Ito K, Kang Y, Zhang Y, Zhang F, Biljecki F (2024). Understanding urban perception with visual data: a systematic review. Cities 152:105169. doi:10.1016/j.cities.2024.105169 [V]
- Kang Y, Zhang F, Gao S, Lin H, Liu Y (2020). A review of urban physical environment sensing using street view imagery in public health studies. Annals of GIS 26(3):261. doi:10.1080/19475683.2020.1791954 [V]
- King G, Murray CJL, Salomon JA, Tandon A (2004). Enhancing the validity and cross-cultural comparability of measurement in survey research. APSR 98(1):191–207. doi:10.1017/S000305540400108X [V]
- Krippendorff K. Content Analysis: An Introduction to Its Methodology. Sage (several editions). [M]
- Lax JR, Phillips JH (2009). How should we estimate public opinion in the states? AJPS 53(1):107–121. doi:10.1111/j.1540-5907.2008.00360.x [V]
- Lewicka M (2010). What makes neighborhood different from home and city? Effects of place scale on place attachment. J. Environ. Psychol. 30(1):35–51. doi:10.1016/j.jenvp.2009.05.004 [V]
- Lewicka M (2011). Place attachment: how far have we come in the last 40 years? J. Environ. Psychol. 31(3):207–230. doi:10.1016/j.jenvp.2010.10.001 [V]
- Liddell TM, Kruschke JK (2018). Analyzing ordinal data with metric models: what could possibly go wrong? J. Exp. Soc. Psychol. 79:328–348. doi:10.1016/j.jesp.2018.08.009 [V]
- Likert R (1932). A technique for the measurement of attitudes. Archives of Psychology 140. [M]
- McCullagh P (1980). Regression models for ordinal data. JRSS B 42(2):109–127. doi:10.1111/j.2517-6161.1980.tb01109.x [V]
- McMillan DW, Chavis DM (1986). Sense of community: a definition and theory. J. Community Psychol. 14(1):6–23. doi:10.1002/1520-6629(198601)14:1<6::AID-JCOP2290140103>3.0.CO;2-I [V]
- McNeish D (2018). Thanks coefficient alpha, we'll take it from here. Psychol. Methods 23(3):412–433. doi:10.1037/met0000144 [V]
- Millstein RA, Cain KL, Sallis JF, Conway TL, Geremia C, Frank LD, et al. (2013). Development, scoring, and reliability of the Microscale Audit of Pedestrian Streetscapes (MAPS). BMC Public Health 13:403. doi:10.1186/1471-2458-13-403 [V]
- Mitchell RK, Agle BR, Wood DJ (1997). Toward a theory of stakeholder identification and salience. Acad. Manage. Rev. 22(4):853. doi:10.5465/amr.1997.9711022105 [V]
- Mujahid MS, Diez Roux AV, Morenoff JD, Raghunathan T (2007). Assessing the measurement properties of neighborhood scales: from psychometrics to ecometrics. Am. J. Epidemiol. 165(8):858–867. doi:10.1093/aje/kwm040 [V]
- Naik N, Philipoom J, Raskar R, Hidalgo C (2014). Streetscore: predicting the perceived safety of one million streetscapes. CVPR Workshops, 793–799. doi:10.1109/CVPRW.2014.121 [V]
- Naik N, Kominers SD, Raskar R, Glaeser EL, Hidalgo CA (2017). Computer vision uncovers predictors of physical urban change. PNAS 114(29):7571–7576. doi:10.1073/pnas.1619003114 [V]
- O'Connor C, Joffe H (2020). Intercoder reliability in qualitative research: debates and practical guidelines. Int. J. Qual. Methods 19. doi:10.1177/1609406919899220 [V]
- Odgers CL, Caspi A, Bates CJ, Sampson RJ, Moffitt TE (2012). Systematic social observation of children's neighborhoods using Google Street View. J. Child Psychol. Psychiatry 53(10):1009–1017. doi:10.1111/j.1469-7610.2012.02565.x [V]
- Olsson U (1979). Maximum likelihood estimation of the polychoric correlation coefficient. Psychometrika 44(4):443–460. doi:10.1007/BF02296207 [V]
- Park DK, Gelman A, Bafumi J (2004). Bayesian multilevel estimation with poststratification: state-level estimates from national polls. Political Analysis 12(4):375–385. doi:10.1093/pan/mph024 [V]
- Pemstein D, Meserve SA, Melton J (2010). Democratic compromise: a latent variable analysis of ten measures of regime type. Political Analysis 18(4):426–449. doi:10.1093/pan/mpq020 [V]
- Marquardt KL, Pemstein D (2018). IRT models for expert-coded panel data. Political Analysis 26(4):431–456. doi:10.1017/pan.2018.28 [V]
- Peterson NA, Speer PW, McMillan DW (2008). Validation of a brief sense of community scale. J. Community Psychol. 36(1):61–73. doi:10.1002/jcop.20217 [V]
- Putnick DL, Bornstein MH (2016). Measurement invariance conventions and reporting. Dev. Rev. 41:71–90. doi:10.1016/j.dr.2016.06.004 [V]
- Quintana M, Gu Y, Liang X, Hou Y, Ito K, Zhu Y, Abdelrahman M, Biljecki F (2025). Global urban visual perception varies across demographics and personalities. Nature Cities 2(11):1092–1106. arXiv:2505.12758; doi:10.1038/s44284-025-00330-x [V]
- Ragin CC (2009). Measurement versus calibration: a set-theoretic approach. In The Oxford Handbook of Political Methodology, 174–198. doi:10.1093/oxfordhb/9780199286546.003.0008 [V]
- Raudenbush SW, Sampson RJ (1999). Ecometrics: toward a science of assessing ecological settings, with application to the systematic social observation of neighborhoods. Sociol. Methodol. 29(1):1–41. doi:10.1111/0081-1750.00059 [V]
- Reed MS, Graves A, Dandy N, Posthumus H, Hubacek K, Morris J, et al. (2009). Who's in and why? A typology of stakeholder analysis methods for natural resource management. J. Environ. Manage. 90(5):1933–1949. doi:10.1016/j.jenvman.2009.01.001 [V]
- Reeskens T, Hooghe M (2008; online 2007). Cross-cultural measurement equivalence of generalized trust: evidence from the European Social Survey (2002 and 2004). Soc. Indic. Res. 85(3):515–532. doi:10.1007/s11205-007-9100-z [V]
- Reisig MD, Bratton J, Gertz MG (2007). The construct validity and refinement of process-based policing measures. Crim. Justice Behav. 34(8):1005–1028. doi:10.1177/0093854807301275 [V]
- Rosseel Y (2012). lavaan: an R package for structural equation modeling. J. Stat. Softw. 48(2). doi:10.18637/jss.v048.i02 [V]
- Rundle AG, Bader MDM, Richards CA, Neckerman KM, Teitler JO (2011). Using Google Street View to audit neighborhood environments. AJPM 40(1):94–100. doi:10.1016/j.amepre.2010.09.034 [V]
- Salesses P, Schechtner K, Hidalgo CA (2013). The collaborative image of the city: mapping the inequality of urban perception. PLoS ONE 8(7):e68400. doi:10.1371/journal.pone.0068400 [V]
- Samejima F (1969). Estimation of latent ability using a response pattern of graded scores. Psychometrika 34(S1):1–97 (Psychometric Monograph 17). doi:10.1007/BF03372160 [V]
- Sampson RJ, Raudenbush SW (1999). Systematic social observation of public spaces: a new look at disorder in urban neighborhoods. AJS 105(3):603–651. doi:10.1086/210356 [V]
- Sampson RJ, Raudenbush SW, Earls F (1997). Neighborhoods and violent crime: a multilevel study of collective efficacy. Science 277(5328):918–924. doi:10.1126/science.277.5328.918 [V]
- Scannell L, Gifford R (2010). Defining place attachment: a tripartite organizing framework. J. Environ. Psychol. 30(1):1–10. doi:10.1016/j.jenvp.2009.09.006 [V]
- Shrout PE, Fleiss JL (1979). Intraclass correlations: uses in assessing rater reliability. Psychol. Bull. 86(2):420–428. doi:10.1037/0033-2909.86.2.420 [V]
- Sijtsma K (2009). On the use, the misuse, and the very limited usefulness of Cronbach's alpha. Psychometrika 74(1):107–120. doi:10.1007/s11336-008-9101-0 [V]
- Sunshine J, Tyler TR (2003). The role of procedural justice and legitimacy in shaping public support for policing. Law Soc. Rev. 37(3):513–547. doi:10.1111/1540-5893.3703002 [V]
- Tankebe J (2013). Viewing things differently: the dimensions of public perceptions of police legitimacy. Criminology 51(1):103–135. doi:10.1111/j.1745-9125.2012.00291.x [V]
- Thomas P, Bratvold RB, Bickel JE (2014). The risk of using risk matrices. SPE Econ. Manag. 6(2):56–66. doi:10.2118/166269-PA [V]
- Tritter JQ, McCallum A (2006). The snakes and ladders of user involvement: moving beyond Arnstein. Health Policy 76(2):156–168. doi:10.1016/j.healthpol.2005.05.008 [V]
- Tyler TR (2003). Procedural justice, legitimacy, and the effective rule of law. Crime and Justice 30:283–357. doi:10.1086/652233 [V]
- Vandenberg RJ, Lance CE (2000). A review and synthesis of the measurement invariance literature. Organ. Res. Methods 3(1):4–70. doi:10.1177/109442810031002 [V]
- Williams DR, Vaske JJ (2003). The measurement of place attachment: validity and generalizability of a psychometric approach. Forest Science 49(6):830–840. doi:10.1093/forestscience/49.6.830 [V]
- Wu H, Estabrook R (2016). Identification of confirmatory factor analysis models of different levels of invariance for ordered categorical outcomes. Psychometrika 81(4):1014–1045. doi:10.1007/s11336-016-9506-0 [V]
- Zhang F, Fan Z, Kang Y, Hu Y, Ratti C (2021). "Perception bias": deciphering a mismatch between urban crime and perception of safety. Landsc. Urban Plan. 207:104003. doi:10.1016/j.landurbplan.2020.104003 [V]

#### 7.7 Qualitative Quantities in Dynamical Models, and Keeping Models Honest

- Albert JH, Chib S (1993). Bayesian analysis of binary and polychotomous response data. JASA 88(422):669–679. doi:10.1080/01621459.1993.10476321 [V]
- Alfeld LE (1995). Urban dynamics: the first fifty years. Syst. Dyn. Rev. 11(3):199–217. doi:10.1002/sdr.4260110303 [V]
- Barlas Y (1996). Formal aspects of model validity and validation in system dynamics. Syst. Dyn. Rev. 12(3):183–210. doi:10.1002/(SICI)1099-1727(199623)12:3<183::AID-SDR103>3.0.CO;2-4 [V]
- Beven K (2006). A manifesto for the equifinality thesis. J. Hydrol. 320(1–2):18–36. doi:10.1016/j.jhydrol.2005.07.007 [V]
- Boutalis Y, Kottas TL, Christodoulou M (2009). Adaptive estimation of fuzzy cognitive maps with proven stability and parameter convergence. IEEE TFS 17(4):874–889. doi:10.1109/TFUZZ.2009.2017519 [V]
- Bredeweg B, Linnebank F, Bouwer A, Liem J (2009). Garp3: workbench for qualitative modelling and simulation. Ecol. Inform. 4(5–6):263–281. doi:10.1016/j.ecoinf.2009.09.009 [V]
- Carvalho JP (2013). On the semantics and the use of fuzzy cognitive maps and dynamic cognitive maps in social sciences. Fuzzy Sets Syst. 214:6–19. doi:10.1016/j.fss.2011.12.009 [V]
- Castellano C, Fortunato S, Loreto V (2009). Statistical physics of social dynamics. Rev. Mod. Phys. 81(2):591–646. doi:10.1103/RevModPhys.81.591 [V]
- Caughey D, Warshaw C (2015). Dynamic estimation of latent opinion using a hierarchical group-level IRT model. Political Analysis 23(2):197–211. doi:10.1093/pan/mpu021 [V]
- Claassen C (2019). Estimating smooth country–year panels of public opinion. Political Analysis 27(1):1–20. doi:10.1017/pan.2018.32 [V]
- Concepción L, Nápoles G, Falcon R, Vanhoof K, Bello R (2021). Unveiling the dynamic behavior of fuzzy cognitive maps. IEEE TFS 29(5):1252–1261. doi:10.1109/TFUZZ.2020.2973853 [V]
- Coyle G (2000). Qualitative and quantitative modelling in system dynamics: some research questions. Syst. Dyn. Rev. 16(3):225–244. doi:10.1002/1099-1727(200023)16:3<225::AID-SDR195>3.0.CO;2-D [V]
- Dambacher JM, Li HW, Rossignol PA (2002). Relevance of community structure in assessing indeterminacy of ecological predictions. Ecology 83(5):1372–1385. doi:10.1890/0012-9658(2002)083[1372:ROCSIA]2.0.CO;2 [V]
- de Kleer J, Brown JS (1984). A qualitative physics based on confluences. Artif. Intell. 24(1–3):7–83. doi:10.1016/0004-3702(84)90037-7 [V]
- Deffuant G, Neau D, Amblard F, Weisbuch G (2000). Mixing beliefs among interacting agents. Adv. Complex Syst. 3(1–4):87–98. doi:10.1142/S0219525900000078 [V]
- DeGroot MH (1974). Reaching a consensus. JASA 69(345):118–121. doi:10.1080/01621459.1974.10480137 [V]
- Delmelle EC (2016). Mapping the DNA of urban neighborhoods: clustering longitudinal sequences of neighborhood socioeconomic change. Ann. Am. Assoc. Geogr. 106(1):36–56. doi:10.1080/00045608.2015.1096188 [V]
- Epstein JM (2002). Modeling civil violence: an agent-based computational approach. PNAS 99(suppl 3):7243–7250. doi:10.1073/pnas.092080199 [V]
- Fariss CJ (2014). Respect for human rights has improved over time: modeling the changing standard of accountability. APSR 108(2):297–318. doi:10.1017/S0003055414000070 [V]
- Felix G, Nápoles G, Falcon R, Froelich W, Vanhoof K, Bello R (2019). A review on methods and software for fuzzy cognitive maps. Artif. Intell. Rev. 52(3):1707. doi:10.1007/s10462-017-9575-1 [V]
- Flache A, Mäs M, Feliciani T, Chattoe-Brown E, Deffuant G, Huet S, Lorenz J (2017). Models of social influence: towards the next frontiers. JASSS 20(4). doi:10.18564/jasss.3521 [V]
- Forbus KD (1984). Qualitative process theory. Artif. Intell. 24(1–3):85–168. doi:10.1016/0004-3702(84)90038-9 [V]
- Forrester JW (1969). Urban Dynamics. MIT Press. [M]
- Friedkin NE, Johnsen EC (1990). Social influence and opinions. J. Math. Sociol. 15(3–4):193–206. doi:10.1080/0022250X.1990.9990069 [V]
- Friedkin NE, Proskurnikov AV, Tempo R, Parsegov SE (2016). Network science on belief system dynamics under logic constraints. Science 354(6310):321–326. doi:10.1126/science.aag2624 [V]
- Gray SA, Gray S, Cox LJ, Henly-Shepard S (2013). Mental Modeler: a fuzzy-logic cognitive mapping modeling tool for adaptive environmental management. HICSS 46. doi:10.1109/HICSS.2013.399 [V]
- Grimm V, Revilla E, Berger U, Jeltsch F, Mooij WM, Railsback SF, et al. (2005). Pattern-oriented modeling of agent-based complex systems: lessons from ecology. Science 310(5750):987. doi:10.1126/science.1116681 [V]
- Hamilton JD (1989). A new approach to the economic analysis of nonstationary time series and the business cycle. Econometrica 57(2):357. doi:10.2307/1912559 [V]
- Hegselmann R, Krause U (2002). Opinion dynamics and bounded confidence: models, analysis and simulation. JASSS 5(3). https://www.jasss.org/5/3/2.html [V]
- Homer J, Oliva R (2001). Maps and models in system dynamics: a response to Coyle. Syst. Dyn. Rev. 17(4):347–355. doi:10.1002/sdr.224 [V]
- Jackman S (2005). Pooling the polls over an election campaign. Aust. J. Polit. Sci. 40(4):499–517. doi:10.1080/10361140500302472 [V]
- Jetter AJ, Kok K (2014). Fuzzy cognitive maps for futures studies: a methodological assessment of concepts and methods. Futures 61:45–57. doi:10.1016/j.futures.2014.05.002 [V]
- Kasarda JD, Janowitz M (1974). Community attachment in mass society. ASR 39(3):328. doi:10.2307/2094293 [V]
- Kennedy MC, O'Hagan A (2001). Bayesian calibration of computer models. JRSS B 63(3):425–464. doi:10.1111/1467-9868.00294 [V]
- Knight CJK, Lloyd DJB, Penn AS (2014). Linear and sigmoidal fuzzy cognitive maps: an analysis of fixed points. Appl. Soft Comput. 15:193–202. doi:10.1016/j.asoc.2013.10.030 [V]
- Kosko B (1986). Fuzzy cognitive maps. Int. J. Man-Mach. Stud. 24(1):65–75. doi:10.1016/S0020-7373(86)80040-2 [V]
- Kuipers B (1986). Qualitative simulation. Artif. Intell. 29(3):289–338. doi:10.1016/0004-3702(86)90073-1 [V]
- Kuipers B (1994). Qualitative Reasoning: Modeling and Simulation with Incomplete Knowledge. MIT Press. [M]
- Lane DC (2008). The emergence and use of diagramming in system dynamics: a critical account. Syst. Res. Behav. Sci. 25(1):3–23. doi:10.1002/sres.826 [V]
- Martin AD, Quinn KM (2002). Dynamic ideal point estimation via Markov chain Monte Carlo for the U.S. Supreme Court, 1953–1999. Political Analysis 10(2):134–153. doi:10.1093/pan/10.2.134 [V]
- Melbourne-Thomas J, Wotherspoon S, Raymond B, Constable A (2012). Comprehensive evaluation of model uncertainty in qualitative network analyses. Ecol. Monogr. 82(4):505–519. doi:10.1890/12-0207.1 [V]
- Mkhitaryan S, Giabbanelli P, Wozniak MK, Nápoles G, De Vries N, Crutzen R (2022). FCMpy: a python module for constructing and analyzing fuzzy cognitive maps. PeerJ Comput. Sci. 8:e1078. doi:10.7717/peerj-cs.1078 [V]
- Nápoles G, Papageorgiou E, Bello R, Vanhoof K (2016). On the convergence of sigmoid fuzzy cognitive maps. Inf. Sci. 349–350:154–171. doi:10.1016/j.ins.2016.02.040 [V]
- Olazabal M, Pascual U (2016). Use of fuzzy cognitive maps to study urban resilience and transformation. Environ. Innov. Soc. Transit. 18:18–40. doi:10.1016/j.eist.2015.06.006 [V]
- Oliva R (2003). Model calibration as a testing strategy for system dynamics models. EJOR 151(3):552–568. doi:10.1016/S0377-2217(02)00622-7 [V]
- Oreskes N, Shrader-Frechette K, Belitz K (1994). Verification, validation, and confirmation of numerical models in the earth sciences. Science 263(5147):641–646. doi:10.1126/science.263.5147.641 [V]
- Özesmi U, Özesmi SL (2004). Ecological models based on people's knowledge: a multi-step fuzzy cognitive mapping approach. Ecol. Model. 176(1–2):43–64. doi:10.1016/j.ecolmodel.2003.10.027 [V]
- Papageorgiou EI, Salmeron JL (2013). A review of fuzzy cognitive maps research during the last decade. IEEE TFS 21(1):66–79. doi:10.1109/TFUZZ.2012.2201727 [V]
- Papageorgiou EI, Stylios CD, Groumpos PP (2004). Active Hebbian learning algorithm to train fuzzy cognitive maps. Int. J. Approx. Reason. 37(3):219–249. doi:10.1016/j.ijar.2004.01.001 [V]
- Peralta AF, Kertész J, Iñiguez G (2025). Opinion dynamics in social networks: from models to data. In Handbook of Computational Social Science, 384–406. doi:10.4337/9781802207309.00042 [V]; preprint arXiv:2201.01322 [V]
- Pluchinotta I, Esposito D, Camarda D (2019). Fuzzy cognitive mapping to support multi-agent decisions in development of urban policymaking. Sustain. Cities Soc. 46:101402. doi:10.1016/j.scs.2018.12.030 [V]
- Poortinga W, Pidgeon NF (2004). Trust, the asymmetry principle, and the role of prior beliefs. Risk Analysis 24(6):1475–1486. doi:10.1111/j.0272-4332.2004.00543.x [V]
- Puccia CJ, Levins R (1985). Qualitative Modeling of Complex Systems. Harvard University Press. [M]
- Raue A, Kreutz C, Maiwald T, Bachmann J, Schilling M, Klingmüller U, Timmer J (2009). Structural and practical identifiability analysis of partially observed dynamical models by exploiting the profile likelihood. Bioinformatics 25(15):1923–1929. doi:10.1093/bioinformatics/btp358 [V]
- Rey SJ (2001). Spatial empirics for economic growth and convergence. Geogr. Anal. 33(3):195–214. doi:10.1111/j.1538-4632.2001.tb00444.x [V]
- Richardson GP (1986). Problems with causal-loop diagrams. Syst. Dyn. Rev. 2(2):158–170. doi:10.1002/sdr.4260020207 [V]
- Saltelli A, Bammer G, Bruno I, Charters E, Di Fiore M, Didier E, et al. (2020). Five ways to ensure that models serve society: a manifesto. Nature 582(7813):482–484. doi:10.1038/d41586-020-01812-9 [V]
- Sampson RJ (1988). Local friendship ties and community attachment in mass society: a multilevel systemic model. ASR 53(5):766. doi:10.2307/2095822 [V]
- Slovic P (1993). Perceived risk, trust, and democracy. Risk Analysis 13(6):675–682. doi:10.1111/j.1539-6924.1993.tb01329.x [V]
- Stach W, Kurgan L, Pedrycz W, Reformat M (2005). Genetic learning of fuzzy cognitive maps. Fuzzy Sets Syst. 153(3):371–401. doi:10.1016/j.fss.2005.01.009 [V]
- Sterman JD (2000). Business Dynamics: Systems Thinking and Modeling for a Complex World. Irwin/McGraw-Hill. [M]
- Vernon I, Goldstein M, Bower RG (2010). Galaxy formation: a Bayesian uncertainty analysis. Bayesian Analysis 5(4):619–669. doi:10.1214/10-BA524 [V]
- Voinov A, Jenni K, Gray S, Kolagani N, Glynn PD, Bommel P, et al. (2018). Tools and methods in participatory modeling: selecting the right tool for the job. Environ. Model. Softw. 109:232–255. doi:10.1016/j.envsoft.2018.08.028 [V]
- White MP, Pahl S, Buehner M, Haye A (2003). Trust in risky messages: the role of prior attitudes. Risk Analysis 23(4):717–726. doi:10.1111/1539-6924.00350 [V]
- van Buuren S, Groothuis-Oudshoorn K (2011). mice: multivariate imputation by chained equations in R. J. Stat. Softw. 45(3). doi:10.18637/jss.v045.i03 [V]

#### 7.8 Software Cited as Reference Implementations

PySAL `segregation` and `giddy`; R `segregation`, `OasisR`, `seg`; COINr and `Compind` (R); Harvard Growth Lab `ecomplexity` (Python) and R `economiccomplexity`; `pythermalcomfort`, `thermofeel`, SOLWEIG (UMEP), RayMan; R `ordinal`, `brms`, `mirt`, `lavaan`, `semTools`, `sirt` (alignment), `anchors`; Python `krippendorff`, R `irr`; R `QCA`, `SetMethods`; `fcmpy`, Mental Modeler; R `QPress`; QSIM and Garp3; R `dgo` (dynamic group-level IRT); R `mice`. Items above with a paper carry its marker; the remainder are [M] as software names, to be pinned to versions when adopted.

---


---

## Part IV. What Was Built From the Survey, and What Remains

### IV.1 The Principles Taken From It

Four principles run through the three parts, and the design follows them.

1. **Carry sufficient statistics up the levels, never indexes.** Counts, sums and distributions aggregate; indexes are computed from them at each level (Part III §2.1, §2.14). A ratio is the ratio of the sums at every level, a decomposable index splits exactly into within and between, and an index such as Gini's, the dissimilarity or the HHI is recomputed from the units beneath. Each index of the catalogue carries its rule.
2. **Allocate down under control totals, so that the levels agree by construction** (Part I §3.5, §5.5): proportional shares, logit shares, dasymetric weights. What is allocated sums to what the parent holds; what is aggregated is the sum of its parts.
3. **Estimate where an estimator has shown it can, and withhold elsewhere** (Part II §6.4, §7, §9): blocked cross-validation for skill, conformal intervals for spread, the area of applicability for extrapolation, and a declared fitness for use (the offices' thresholds) for publication; a withheld estimate is a quantity to declare.
4. **A parameter the record does not identify is declared, not reported** (Part I §6, recommendation 5): identifiability decides between "fitted" and "declared", and declared ranges are propagated, with the declarations ranked by what they move.

### IV.2 What Was Built

| Recommendation | Where it went | State |
|---|---|---|
| Algebra tags on every index (III.1) | the catalogue's `aggregation` column (sum, a weighted mean, a ratio of sums, decomposable, recompute, max, min, median, none) | 1,536 indexes tagged; drafted, unchecked |
| Fast–slow reduction (I.1) | kinesis `scales`: laws in equilibrium form, the quasi-steady state; the error of a fast flow falling as ε (checked) | written |
| Allocation and aggregation operators (I.3) | kinesis `scales`: sum, mean, weighted mean, minimum, maximum, reductions by any registered measure, parent, share, logit, z-scores among siblings, graph Laplacian, neighbours, the production-constrained interaction | written |
| Multirate stepping (I.6) | kinesis `scales`: laws at their own steps exchanging states at the start of each (Jacobi), first order in the slow step (checked), Runge–Kutta when one step is shared | written |
| Harris–Wilson structural dynamics (I.2) | expressible in kinesis `scales` (inflow and a flow on sizes; its equilibrium Σ W = Σ O/κ checked); not yet its Bayesian inference | in part |
| Identifiability as the gate (I.5) | kinesis `scales` `fit`: unknowns whose standard error passes half their value, or that move together, reported as not identified; the plug-in then declares them | written (from the linearised covariance, not the profile likelihood) |
| Ensembles over declarations and screening (III.6) | kinesis `scales` `ensemble` (Latin hypercube over declared ranges and estimated errors) and `screen` (Morris) | written |
| Perfect aggregation (I.13) | kinesis `scales` `perfect_aggregation` (Iwasa, Andreasen and Levin's condition) | written |
| Volume-preserving change of support (II.2) | tyche `spatial/support.hpp`: areal weighting, dasymetric, class densities by NNLS, pycnophylactic | written |
| Area of applicability and blocked cross-validation (II.5) | tyche `spatial/prediction.hpp` | written |
| Conformal intervals (II.10) | tyche `spatial/prediction.hpp`, from the blocked cross-validation's errors | written (coverage approximate, measured by a check) |
| A refusal-and-declaration outcome (II.1, III.7) | kinesis `scales` `missing` (what a model needs that it has not been given, refused with its name, level and count); tyche's withheld estimates with their reasons; the plug-in's declarations | written; the offices' thresholds as declared schemes not yet |
| Validation against persistence (I.8) | kinesis `scales` `fit`: skill on the years held aside against standing still; the plug-in's judgement at each level | in part (no figure of merit or CRPS yet) |

### IV.3 What Remains

In the order of the parts' rankings: the Bayesian inference of Harris–Wilson from sizes; reconciliation across the hierarchy (MinT) and temporal disaggregation (Denton, Chow–Lin); a Kalman filter on a tree; history matching and ABC; Fay–Herriot's extensions and unit-level empirical best; spatial microsimulation; BYM2; the offices' thresholds as declared schemes; ordinal measurement models with invariance, ecometrics and MRP for qualitative indexes; ordinal state-space models; segregation profiles and isolation and exposure indices; SDG 11's indicators to their metadata and DEGURBA; UTCI with mean radiant temperature; economic complexity; fuzzy cognitive maps and qualitative simulation; the composite-indicator uncertainty engine; constrained cellular automata and discrete development events (Part IV.4).

### IV.4 What the First Case Showed

On Melbourne's CLUE record (603 blocks, 13 areas, 2002–2024), two things the parts warn of appeared at once. First, a census withholds a great deal: 30% of all the floor space counted is in cells left blank for confidentiality, so the estimation of withheld cells is not a side issue but the first estimator a model needs (Part II §1). Second, growth comes in buildings: 70% of the homes and 90% of the office floor added in 2016–2024 went to the 5% of blocks that gained most, so a model of smooth shares, which every multi-level land-use model allocates by (Part I §3), cannot place it at the block (a correlation of 0.05 between the change forecast and the change recorded) though it places part of it among areas (0.35 for homes, 0.67 for offices). Development as discrete events with a size, the CLUE-S and constrained-CA tradition's ranked allocation with capacities (Part I §3.1, §3.3), is the next form to write. And a stock–flow cycle of vacancy and rents (Part I §4.1) is not identified by thirteen areas over fourteen years, so its rates are declared, as the design requires.

---

## Bibliography

Each part's bibliography follows that part, with its markers.
