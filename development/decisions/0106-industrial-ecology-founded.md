# 0106. Industrial Ecology Founded: Material and Energy Accounts

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user shared a handbook, *Material and Ecological Conditions of Life: A Comprehensive Research Toolkit for Studying People's Lives in the Future*, and asked whether most of it was implemented, dynamical systems and agent-based models included. Claude answered that about a third was. Accounting was the largest gap: material flow analysis, LCA, environmentally extended input–output and physical accounts. The user then asked: "implement them, as possible, if some new domain(s), foundation(s) is/are needed, just tell me."
  - **The decision.** Claude told the user a new domain was needed and founded it. The handbook's accounting is a science with its own objects (goods in use, processes, product systems, sectors' pressures), and README §4 did not list it. The idea of 2026-11-13, *The material stock of a city*, had already recommended a domain of its own and is promoted here.
  - **The design.** Claude, here.

## 1. Context

The handbook's §5 calls resource, environmental and economic accounting "the most direct bridge between physical conditions and political economy". Its roadmap puts "one reproducible resource account" second, after the research architecture. The infrastructure had parts of it in other places:

- economics' Leontief model;
- the ecology domain's carbon of trees;
- architecture's heat losses.

It had no stocks of goods in use, no balance of processes, no product system and no pressure attributed to final demand.

## 2. Decision

### 2.1 The Foundations First

- **tyche:** the exponential, Weibull and lognormal laws, the lifetimes of goods in use ([tyche decision 0020](https://github.com/huangwanhong-maker/tyche/blob/main/development/decisions/0020-lifetime-laws.md)).
- **calx:** the leading walks of a Neumann series ([calx decision 0017](https://github.com/huangwanhong-maker/calx/blob/main/development/decisions/0017-the-leading-walks-of-a-neumann-series.md)). For Z ≥ 0 with ρ(Z) < 1, every walk of eᵀ(I − Z)⁻¹y above a cut is found, with no omission, by an exact bound on what each branch holds. This is the structural path analysis that LCA and input–output both need, and it names no object in nature.

### 2.2 The Domain

**industrial-ecology** is a field at height 1 on economics, C++ from the start. Its theory note is `domains/industrial-ecology/theory/material-and-energy-accounts.md`.

- `stocks`:
  - lifetime laws, including peira's Kaplan–Meier;
  - dynamic stock models driven by inflows or by stocks (Müller 2006), with an initial stock by age and early retirement when a stock falls faster than its cohorts leave;
  - the materials a stock holds, with exact bounds.
- `flows`:
  - systems of processes and their balances;
  - transfer coefficients;
  - substances carried by goods;
  - STAN's reconciliation: unknowns eliminated by projection, the χ² test, normalised adjustments, observability.
- `lca`:
  - the matrix method (Heijungs and Suh 2002);
  - partition and substitution;
  - exact elasticities;
  - Monte Carlo;
  - structural paths.
- `eeio`:
  - multipliers;
  - production-based and consumption-based accounts by region;
  - what trade embodies, and the origin–destination matrix of each stressor;
  - footprints by product;
  - characterisation;
  - supply chains.
- `accounts`: physical supply–use tables and their identities; economy-wide material flow indicators.
- `buildings`: material intensities by region, use, structure and period, with bounds. A building's intensity is the envelope over what is unknown of it, so an unknown attribute widens the bounds and is never guessed.
- `energy` and `circularity`: planned.

### 2.3 The API, the Data and the Work

- **The API:** `science/materials.hpp` (seven operations) and `science.materials`.
- **The data library:**
  - RASMI's material intensities (Fishman et al. 2024, CC BY 4.0);
  - Eurostat's industry-by-industry input–output tables and its air emissions accounts (CC BY 4.0);
  - `data/products/materials_and_flows.py`, which joins CLUE's 2024 buildings to the city model's OpenStreetMap footprints for their floor areas.
- **The Urban Studio's plug-in *Materials and flows*:**
  - six tools: what the buildings hold, what would renew the stock, a stock over time, flows that balance, a life cycle, what final demand causes;
  - two templates on open data: Melbourne's centre, and the EU27's emissions.

## 3. Alternatives Considered

- **Modules of architecture and urban science.** Rejected, as the idea argued: it would scatter accounting that belongs together across the domains of buildings and streets, and LCA and input–output would have no home.
- **Inside economics.** Rejected: economics holds value. Industrial ecology holds mass and energy, which balance whether or not anything is priced; it uses economics' Leontief model and does not belong to it.
- **The name `social-metabolism`.** The handbook's word, and the domain's object. The field that studies it is called industrial ecology (Graedel and Allenby; the Journal of Industrial Ecology), and the domain takes the field's name, as the others do.
- **Wrapping openLCA, Brightway or pymrio.** Their implementations sit outside the infrastructure's interfaces, and their methods are each a few matrix equations. The domain implements the methods and holds them to the reference libraries: its EEIO reproduces pymrio's accounts to 10⁻¹⁰.

## 4. Consequences

- **Checks.** calx's walks (9 checks), tyche's lifetime laws (12), the domain's 104, and the Urban Studio's suite more.
- **Shown.**
  - Every account of the domain balances to rounding.
  - The EU27's final demand causes, in one economy, exactly what its industries and households emit.
  - A building whose structure is not recorded takes the envelope of RASMI's four structures.
- **Not built:**
  - energy balances and exergy, and circularity;
  - multi-regional tables at EXIOBASE's size, which need sparse solvers;
  - open LCA inventories (USEEIO, the Federal LCA Commons);
  - lifetimes of buildings estimated from records of demolition.
- **Provisional:** the bounds of RASMI taken as its 5th and 95th percentiles; the template's lifetime of buildings (Weibull, mean 80 years, shape 3), presumed.
