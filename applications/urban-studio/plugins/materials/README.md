# Materials and Flows

*Status: draft (2026-11-13; [ADR 0106](../../../../development/decisions/0106-industrial-ecology-founded.md)).* The Urban Studio's third plug-in: the material conditions of a city and an economy. What a city's buildings weigh and of what they are made; the flows that would renew them; any stock over time; measured flows made to balance; a product's life cycle; what an economy's final demand causes. Its mathematics is the [industrial-ecology domain](../../../../domains/industrial-ecology/README.md)'s, through the API's `science/materials.hpp`.

## 1. Tools

| Tool | What it answers | On |
|---|---|---|
| *What the buildings hold* | Each building's mass of eight materials, by group and in all, with bounds that widen over what is not recorded of it (structure, function) | any table of buildings with floor areas; RASMI's intensities by region |
| *What would renew the stock* | Floor area demolished and built each year to keep a path, and the materials leaving and entering, under a declared lifetime law | the same table, with years of construction |
| *A stock over time* | The stock, inflow and outflow of any goods in use, from what enters each year or from the stock's path | any series by year |
| *Flows that balance* | Measured flows reconciled so that every process balances; unknown flows found; a χ² test of whether they can balance; the flow most likely in error | any table of flows (from, to, value, standard deviation) |
| *A life cycle* | A product system's inventory and impacts, each process's share, the coefficients that matter most, a Monte Carlo spread, the supply chains | a table of exchanges and a table of characterisation factors |
| *What final demand causes* | Each product's and each kind of final use's pressure along its supply chains, and the chains that carry most | an input–output table and a table of pressures by industry |

## 2. Templates

- **The material stock of Melbourne's centre.** The Hoddle Grid's 1,524 buildings in CLUE's 2024 census, joined to their OpenStreetMap footprints, weighed by RASMI's intensities for Australia and New Zealand, by block and by use, then renewed to 2050 under a Weibull lifetime of mean 80 years (presumed).
- **What the European Union's final demand emits.** The EU27's domestic input–output table for 2022 with its air emissions accounts: greenhouse gases, acidifying gases and fine particulates, by product, by kind of final use and along supply chains.

## 3. What Is Presumed

- **Material intensities** are RASMI's: the 5th, 50th and 95th percentiles of its ranges are taken as the lower, central and upper bounds. RASMI states no period of construction, so a building's age does not change its intensity.
- **Structure** is not recorded in CLUE, so every building's bounds span RASMI's four structures. A building recorded as unoccupied has no known function, and its bounds span all three functions.
- **Floor area** is the footprint's area times CLUE's storeys. A footprint holding several of CLUE's buildings is shared equally among them.
- **Lifetimes** are declared in each run, never defaulted silently. The template's Weibull law of mean 80 years and shape 3 is an assumption, not fitted to Melbourne's records.
- **A single-region input–output table** treats imports as coming from outside. What they embody abroad needs a multi-regional table and is not counted.

## 4. Data

`data/products/materials_and_flows.py` writes the tables from RASMI (CC BY 4.0), CLUE (CC BY 4.0), OpenStreetMap through the city model (ODbL 1.0), and Eurostat's `naio_10_cp1750` and `env_ac_ainah_r2` (CC BY 4.0). The Studio's recipe fetches them all.
