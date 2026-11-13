# 0109. Ecosystem Services on a Grid

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user asked to implement the methods of the handbook *Material and Ecological Conditions of Life*. Its §6.3 names InVEST's models of ecosystem services, with the caution that "an ecological-service model … should not become the sole representation of the condition, integrity, or moral significance of ecological systems".
  - **The design.** Claude, here, in the ecology domain's planned module `ecosystems` (ADR 0101).

## 1. Context

The ecology domain held taxonomy, biodiversity, populations, the urban forest and connectivity. Carbon was computed tree by tree, by allometry. It had no service of a landscape: carbon by land cover, the quality of habitat under threats, the water a basin yields, the soil it loses.

## 2. Decision

`ecology/ecosystems/services.hpp` holds InVEST's four models that compute in closed form (Sharp et al. 2020) on a grid of land cover:

- **carbon storage:** four pools per class;
- **habitat quality:** threats decaying linearly or exponentially with distance, weighted and met by each class's sensitivity, the quality falling by the half-saturation curve;
- **annual water yield:** Budyko's curve in Fu's form, ω from the available water and Z;
- **soil loss:** RUSLE with McCool's slope length and steepness.

Every coefficient is the caller's to declare, with its source; the domain holds none.

## 3. Alternatives Considered

- **Wrapping InVEST.** InVEST is open (Apache 2.0) and in Python. Rejected for the domain's C++, which these closed forms need no library for. Works in Python may call InVEST directly.
- **Routing water and sediment over the terrain** (InVEST's seasonal water yield and sediment delivery). Deferred: they need flow directions over a digital elevation model, which belong with geography's terrain.

## 4. Consequences

- **Checks.** Seventeen, by hand:
  - carbon;
  - habitat degradation at given distances, none beyond the linear reach, the exponential at 5% of its reach;
  - Budyko's curve at its limits and at PET = P;
  - a cell's yield, and a sealed cell's;
  - RUSLE's L and S.
- **Not built:**
  - pollination;
  - urban cooling;
  - sediment delivery and routing;
  - ecosystem accounts (SEEA EA);
  - a tool of the Urban Studio on a grid of land cover.
