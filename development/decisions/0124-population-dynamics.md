# 0124. Population Dynamics: a Place's People Learned as a Fluid's Laws, in the Urban Studio

- **Status:** Accepted, provisionally (2026-11-16). The plug-in's name and colour are provisional.
- **Date:** 2026-11-16
- **Decided by:**
  - **The request.** "Please utilize dynamical system library to learn population dynamics with various models, like fluid dynamics, and simulate, visualization, in the urban studio. Similarly, as a plug-in/script may be ok."
  - **The design.** Claude, here, on kinesis (ADR 0103) in the Urban Studio (ADR 0102), with the plug-in contract of ADR 0123.

## 1. Context

kinesis learns laws from records: templates filled, sparse discovery, candidates ranked, dynamic modes. Each of these needs one trajectory of a small system sampled often. A place's people is the opposite: thousands of cells known at a dozen times. GHS-POP gives every fifth year since 1975 on a 1 km grid. Its laws, if it has any, couple each cell to its neighbours: spread, drift and growth, as in a fluid. The library could not declare such a law, nor learn one from a dozen snapshots (its decision 0004 deferred indexed families; its theory note listed a discretised fluid as not built).

## 2. Decision

**The foundation first: kinesis's `balance`** (its decision 0005).

- **Laws.** Balance laws of a density on a graph of cells, in finite volumes. Diffusion, crowding (the porous-medium flux) and drift up a covariate are conservative by construction. Growth may be linear, logistic or Gompertz's, or made in proportion to a covariate. Every law is linear in its coefficients.
- **Runs** by Runge–Kutta within Gershgorin's bound.
- **Learning from a few snapshots.** The integral form by the trapezoid, then shooting by Levenberg–Marquardt. A negative diffusion is ill-posed, so it is set aside and said to be.
- **Judgement** on snapshots kept aside, as a whole and as a pattern (each forecast's total set to the record's), against carrying the last snapshot forward.
- **Baselines:** a rate per cell, and the record's dynamic modes by the method of snapshots.

**The data library's `population_history.py`.** GHS-POP R2023A's 1 km Mollweide tiles of every epoch, 1975–2030, for any place (a few MB each), are carried onto a 1 km grid in the place's UTM zone with nobody lost. Mollweide keeps area and not shape: at Melbourne its axes meet at 134° on the ground, so a neighbour on its grid is not a neighbour at a distance. Its sphere is WGS 84's semi-major axis, held by lining a tile up with GHS-POP's WGS 84 grid. Melbourne and Lagos are the first cases, and any place is made from its latitude and longitude.

**The API composes them** in `science/population.hpp`: `growth` (the whole as one cell: exponential, logistic, Gompertz's), `learn` (every law fitted, judged, with its currents), `modes`, and `simulate` (a law run ahead, with sources and scaled terms, against the field's own projections). `science.population` asks it from scripts.

**The visualization layer's `dynamics/fields.js`** draws a field on a grid:

- tones on the validated maps, on a logarithmic scale;
- change on a diverging scale;
- currents as arrows summed over blocks, their lengths by the square root;
- legends, and a canvas painter.

**The plug-in, *Population dynamics*,** in the Urban Studio:

- **its view**, *Population*: the field as the page, a ribbon of years, and four readings (as recorded; laws learned; modes; the years ahead, with a source placed by a click);
- **five tools** of the board's contract;
- **two templates**, Melbourne and Lagos;
- **its server side.**

## 3. Alternatives Considered

- **Indexed families in kinesis's grammar.** The general answer, and a change to every tool (kinesis decision 0005). The structured class of balance laws needs none of it.
- **The plug-in fitting its laws in Python.** Rejected: learning laws from records is kinesis's, and a general method goes into its foundation first.
- **Mollweide's grid as it is.** Its shear makes a diffusion anisotropic by a factor of 1.4 and turns its axes 44° at Melbourne.
- **The global 30″ grid of each epoch.** 384 MB an epoch; the tiles cost a hundredth of it.
- **Census tracts as the cells.** The operators take any graph, and a work may give one. GHS-POP covers every place at one resolution and one method, and is the first.

## 4. Consequences

- **Checks:**
  - kinesis 464 (31 new);
  - the API's population checks against fields made by a separate integrator (18);
  - the visualization layer's `fields` (18 of its 718);
  - the data library's population history (Mollweide, the tiling, conservation, and the alignment with GHS-POP's own grid, 0.92 in logarithms);
  - the Urban Studio's suite, with the plug-in's (86 in all).

  The page was driven in headless Chrome (17 steps, not in the suite); twelve pictures are in the gallery's §44, and the record is `research/experiments/2026-11-16-population-dynamics/`.
- **What the first records show** (GHS-POP's estimates, which are themselves disaggregated from censuses into built-up land):
  - **No diffusion.** At 1 km neither city holds one: it is found negative and set aside, or too small to move anyone. A city's density is pinned by its buildings, and a spread that would move people a kilometre a year would flatten Melbourne's centre within a year.
  - **Melbourne.** Fitted on 1975–2010 and judged on 2015 and 2020, the laws of the field barely beat carrying 2010 forward, and miss the acceleration of growth after 2005 (the total off by 16%). The record's modes place the change best (pattern skill +0.87).
  - **Lagos.** Growth slowed (3.4% a year to 1.8%), and every law overshoots the total. A drift along the settled landscape places the change slightly better than growing everywhere alike (pattern skill +0.09). For the whole, Gompertz's law with a ceiling near 20 million is off by 0.6% on the years kept aside.
- **Not built:**
  - zones of any shape from data (the operators take them);
  - covariates from other layers (roads, land use, access) to drift up;
  - coefficients that vary in space;
  - births, deaths and migration as terms of their own (demography's);
  - an implicit step for stiff laws;
  - spatial bootstrap errors.
- **Provisional:** the plug-in's name and colour.
