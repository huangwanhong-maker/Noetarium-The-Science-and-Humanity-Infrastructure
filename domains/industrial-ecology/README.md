# industrial-ecology

*Status: draft (2026-11-13; [ADR 0106](../../development/decisions/0106-industrial-ecology-founded.md)). Field, height 1, on economics; C++ from the start.* The material and energy stocks and flows of a society, its social metabolism: what is in use, what enters and leaves it, what each process takes and returns, and what final demand causes along its supply chains. Founded at the user's request to implement the handbook *Material and Ecological Conditions of Life*, whose §5 (resource, environmental and economic accounting) is the domain's scope. Its theory note is [material-and-energy-accounts.md](theory/material-and-energy-accounts.md).

## 1. Purpose

- **Mass is conserved.** Every account here balances by construction, and says by how much it departs from balance, which should be rounding: a stock changes by what enters less what leaves; a process returns what it takes, less what it stores.
- **Presence is not access.** A stock of buildings, a flow of water, a footprint of carbon describe a society's material conditions; who can use them is the business of the domains above it (social provision, urban science).
- **What is known is kept apart from what is presumed.** Lifetimes, material intensities, allocation factors and characterisation factors are declared, with their sources and bounds. An unknown attribute of a building widens its bounds; it is never guessed.
- **Measurements are reconciled, not forced.** Flows measured with errors are made to balance by weighted least squares, and a test says when they cannot be, and where.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `stocks` | Lifetime laws (fixed, exponential, normal, Weibull, lognormal, gamma, a table, peira's Kaplan–Meier); dynamic stock models driven by inflows or by stocks (Müller 2006), with an initial stock by age and the correction of negative inflows by early retirement, reported; the materials a stock holds and moves, with exact bounds | draft |
| `flows` | Material flow analysis: systems of processes, flows and stocks and their balances; transfer coefficients; a substance carried by goods; the reconciliation of measured flows with unknowns eliminated, the χ² test and each measurement's normalised adjustment (STAN's method) | draft |
| `lca` | Life cycle assessment by the matrix method (Heijungs and Suh 2002): multifunctional processes partitioned or expanded; scaling, inventory, impacts and contributions; exact elasticities; Monte Carlo on lognormal coefficients; structural paths | draft |
| `eeio` | Environmentally extended input–output on economics' Leontief model: intensities and multipliers; production- and consumption-based accounts by region, what trade embodies, the origin–destination matrix of every stressor; footprints by product; characterisation; supply chains | draft (held to pymrio) |
| `accounts` | Physical supply–use tables and their three identities (SEEA-CF); economy-wide material flow indicators: DMI, DMC, PTB, NAS, RMC | draft |
| `buildings` | The material stock of buildings: a declared scheme of intensities by region, use, structure and period, with bounds; a building's intensity as the envelope over what is unknown of it; stocks by group; cohorts by age for a dynamic stock model | draft |
| `energy` | Energy balances (supply, transformation, final use) and exergy | planned |
| `circularity` | Recycling rates, the circularity of a system, secondary materials and their quality | planned |

## 3. Dependencies

| Source | What industrial ecology uses from it |
|---|---|
| economics | `production`: input–output tables and the Leontief model, on which `eeio` stands |
| calx | `linalg` (solve, QR with its Q applied, the leading walks of a Neumann series, calx decision 0017), `random`, verification |
| tyche | `probability` (normal, Weibull, lognormal, gamma, exponential laws; the χ² law of the reconciliation test) |
| peira | `survival` (Kaplan–Meier, a lifetime law from observed lives) |
| morphe, physis, nomos | declared; not yet used |

## 4. Used By

- **The API's `science/materials.hpp`** composes it for any work's engine.
- **The Urban Studio's plug-in *Materials and flows*** is its first work: a city's buildings as a material stock, the flows that renew it, measured flows reconciled, a product's life cycle, and an economy's footprint.

## 5. Open Questions

- **Large tables.** The `eeio` module solves the Leontief model densely, which serves national tables (64 products) and small multi-regional ones (a few thousand sectors); EXIOBASE's 9,800 sectors need calx's sparse solvers behind the same interface.
- **Inventories.** LCA's databases (ecoinvent) are licensed; the domain holds the method, and a system is a work's to give. An open background (USEEIO, the Federal LCA Commons) is a candidate for the data library.
- **Lifetimes from records.** A building's lifetime is a survival law with censoring, which peira estimates from records of demolition; no open record of demolitions is in the data library yet.
- **Coupling stocks to flows.** A dynamic stock model's inflows are a flow system's imports; the two are one model in ODYM's design, and are joined here by a work.

## 6. References

- Brunner, P. H. and Rechberger, H. (2016), *Handbook of Material Flow Analysis*, 2nd ed., CRC Press.
- Cencic, O. and Rechberger, H. (2008), Material flow analysis with software STAN, *Journal of Environmental Engineering and Management* 18, 3–7.
- Fishman, T. et al. (2024), A comprehensive set of global residential and non-residential building material intensities (RASMI), *Scientific Data* 11, 418.
- Heijungs, R. and Suh, S. (2002), *The Computational Structure of Life Cycle Assessment*, Kluwer.
- Miller, R. E. and Blair, P. D. (2009), *Input–Output Analysis*, 2nd ed., Cambridge University Press.
- Müller, D. B. (2006), Stock dynamics for forecasting material flows, *Ecological Economics* 59, 142–156.
- Pauliuk, S. and Heeren, N. (2020), ODYM, an open software framework for studying dynamic material systems, *Journal of Industrial Ecology* 24, 446–458.
- United Nations et al. (2014), *System of Environmental-Economic Accounting 2012: Central Framework*.
- The theory note's §8 lists the rest.
