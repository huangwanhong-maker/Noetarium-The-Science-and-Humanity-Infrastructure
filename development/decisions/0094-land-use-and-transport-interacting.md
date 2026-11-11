# 0094. Land Use and Transport Interacting

- **Status:** Accepted, provisionally (2026-11-12).
- **Date:** 2026-11-12
- **Decided by:**
  - **The order.** Stage 6 of the urban priorities (`development/ideas/2026-11-08-urban-science-priorities.md`): "Land use and transport interacting. Housing, movement and access fed back over years", in urban science, after stages 2 and 4.
  - **The go-ahead.** The user: "If finished, then commit and push. And continue the next feature", and "Continuer".
  - **The condition, from stage 3.** That what is built be reusable: "We don't want it just for a city, and a specific visualization task".
  - **The design.** Claude.

## 1. Context

The infrastructure held the parts of a land-use and transport model, but not the model:
- **Households choosing dwellings** in urban science's `housing`: a logit market with unique clearing rents and Berry's inversion (ADR 0075).
- **Networks of ways and the times on them** in `access` (ADR 0078, 0086).
- **The measures of accessibility** in nomos (ADR 0086).
- **Choices estimated and preferences priced** (ADR 0088).

Nothing said where travellers go between places, nor what congestion does to the way they take. Nothing fed travel back to where people live, or rents back to what is built, year after year. Land-use and transport models compose these at their three speeds (Wegener 2004): construction slow, location intermediate, travel fast.

## 2. Decision

Layers, each general where it stands.

### 2.1 Spatial Interaction and Congestion in nomos

nomos's decision 0010:
- **A new module, `interaction`.**
  - Wilson's production-, attraction- and doubly constrained models, with Furness's balancing.
  - Hyman's calibration to a mean cost or to an observed table. It is the Poisson maximum likelihood estimate, which nomos's own PPML also gives.
  - The logsum accessibility, and the common part of commuters.
  - Lowry's metropolis and Garin's closed form.
  - Evans's combined equilibrium of distribution and assignment, by partial linearisation with its exact lower bound.
- **`games` gains routing games with congestion.** Arcs of the Bureau of Public Roads, Wardrop's equilibrium and the system optimum by Frank–Wolfe on Beckmann's potential, the relative gap, skims, and the price of anarchy, on morphe's Dijkstra.

The checks hold them to statsmodels' Poisson GLM, to scipy's path-based solution of a grid of streets, to Pigou's and Braess's networks in closed form, and to Roughgarden and Tardos's 4/3.

### 2.2 The Urban-Science Module `luti`

`luti/model.hpp` composes the region year by year ([the theory note](../../domains/urban-science/theory/land-use-and-transport.md)):
1. **Development.** Dwellings follow last year's rents within the capacity the rules allow.
2. **Location.** Households choose zones by `housing`'s market, each zone valued for its own amenity plus λ times last year's accessibility; the rents clear.
3. **Jobs.** Service jobs follow households (Lowry).
4. **Travel.** The journeys to work settle in Evans's equilibrium on a congested network, or by gravity on a fixed table.
5. **Accessibility.** The logsum of the jobs, which next year's location reads.

`reveal` takes a base year from observed households and rents. Scenarios change jobs, capacities, own values, roads and households. The theory note proves:
- Wilson's theorem;
- that calibration is maximum likelihood;
- the logsum's derivatives;
- Evans's equilibrium and the bound the code uses;
- capitalisation (Δ ln r = λ ΔA/α, exactly, for one class at a held stock);
- Braess's paradox in land;
- that a revealed base year is a fixed point;
- the form of the stationary states.

### 2.3 One Composition in the API

`api/cpp/science/luti.hpp` (`science::luti::run`) takes a place's roads and zones (cells of households and jobs) and does four things:
- contracts the roads to a network of chains between junctions, with capacities in the peak hour;
- joins the zones to it as `access`'s catchments do;
- reveals the base year from what stands;
- runs the years twice, as things stand and with a scenario.

A year that repeats the one before, with no event to come, is copied, so only the years a change moves are solved. Like the toolbox (ADR 0080) and the designer (ADR 0089), it stands in the API so that no work depends on another.

### 2.4 Journeys to Work in the Data Library

`data/products/commuting.py` makes cases of journeys to work by year. Its first case is New York City's five boroughs: LODES 8 (US Census Bureau, public domain), 2002–2023, summed from blocks to the 2,327 census tracts, each tract's point weighted by its blocks' jobs and workers. A region is added as a case.

### 2.5 The Workbench's Plug-in *Land Use and Transport*

A layer anywhere, in the lens Change: *a change, and the years after it*.
- **A place.** Its people on GHS-POP's cells become households, the floor of its workplaces becomes jobs, and its classified roads come with their classes' speeds and capacities.
- **Changes on the map.** Widen or close a road, allow more dwellings in a cell, add jobs.
- **The years**, as things stand and with the changes:
  - the cells coloured by what the changes do to their households;
  - the roads coloured by their load in the peak hour;
  - the years played on the map;
  - the card's chart given to the Studio as a figure.

### 2.6 What New York's Record Shows

`research/experiments/2026-11-12-land-use-and-transport`: New York City's journeys to work between 2,327 tracts, 2002–2023, at straight distances.
- **The decay has weakened, and distance explains little.** β fell from 0.146 per km (2002) to 0.127 (2021–2023). The model places 0.56–0.60 of the flows where they are, against 0.50–0.54 for one that ignores distance.
- **The decay carries over.** 2010's β forecasts 2019's flows as well as 2019's own (0.598 against 0.597), and better than 2010's table carried forward (0.595) or unchanged (0.515). Over the four years of the pandemic the carried table does better (0.602 against 0.583).
- **Where access rose, resident workers came less.** Within a borough, a kilometre more access to jobs went with 17.1% fewer resident workers (± 3.5, the spatial error model).
- **The composed model does not find where they went.** With its speed of building set so that the total is right (+23.9%), its tracts correlate −0.04 with the record, and uniform growth does better. In New York 2010–2019, building did not follow access through rents. Nothing in the record says why: rents, rezonings and who moved are not in it.

## 3. Alternatives Considered

- **A microsimulation of households and firms (UrbanSim's manner).** Deferred: `housing`'s and `movement`'s agents are its pieces. A deterministic composition first gives exact theorems to hold a microsimulation to.
- **The spatial interaction models in urban science.** Rejected: trade, migration and retail use the same models (nomos's decision 0010).
- **Conjugate Frank–Wolfe as the default.** Tried, and set aside: it stalled on the checks' grid.
- **Every street in the page's network.** Rejected: strategic land-use models assign to the classified network, with zones joined to it. Evans's iteration on every street of a city would take minutes a year.
- **Rents from a record.** Not open in most places. The base year is revealed with every rent at what building pays, so that nothing is built until a change moves access.

## 4. Consequences

- **Checks:**
  - nomos: `check_nomos_interaction` (27) and `check_nomos_congestion` (23);
  - urban science: `check_urban_luti` (21);
  - the workbench: `check_land_use.py` (12).
- **Converging slowly.** Frank–Wolfe and Evans's iteration converge sublinearly; the page asks for a gap of 10⁻³. Path-based methods are the open question in both nomos READMEs.
- **Next:**
  - firms choosing where to locate;
  - more than one mode;
  - households that age, through demography's projections;
  - the agents of `housing` and `movement` in place of steps 2 and 4.
