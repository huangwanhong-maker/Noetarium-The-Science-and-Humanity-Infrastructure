# 0089. What Could Be Built

- **Status:** Accepted, provisionally (2026-11-09).
- **Date:** 2026-11-09
- **Decided by:**
  - **The order.** Stage 5 of the urban priorities (`development/ideas/2026-11-08-urban-science-priorities.md`): "The generative design loop. Massing and frontage rules, sun and shade on streets and façades, heuresis's Pareto alternatives in the Studio", "'What could be designed', scored by configuration and access as well as energy".
  - **The go-ahead.** The user: "If finished, then commit and push. And continue the next feature."
  - **The condition, from stage 3.** That what is built be reusable: "We don't want it just for a city, and a specific visualization task".
  - **The design.** Claude.

## 1. Context

The architecture domain held buildings as complexes of rooms and their heat (ADR 0062, 0063), and its README planned `form`, `frontage` and `rules`. The workbench's plug-in *Architecture* was planned. heuresis searched designs (ADR 0073), and its case study of a district (`research/experiments/2026-11-05-designing-a-district`) wrote a provisional model of shadows on the ground for itself, and kept each parcel's footprint as built: nothing in the infrastructure said what a lot may hold.

A design loop needs three things the infrastructure lacked:
- the envelope rules allow on a lot;
- a massing within it, with its measures;
- the sun and shade a massing casts on streets and on the faces of buildings.

It also needs one composition that any work can call on any block, not a city's script.

## 2. Decision

Layers, each general where it stands.

### 2.1 Clipping, Insets and Rays in physis

physis's decision 0025, in `computational_geometry`'s planar geometry:
- `clip_left`, a ring clipped by a half-plane (Sutherland–Hodgman);
- `inset`, a ring moved inward by a distance for each edge. The mitred offset is kept where it is a simple ring at least the distances from every edge, checked; otherwise the half-planes' intersection with the ring, exact for a convex ring and conservative for one with reflex vertices;
- `ray_entry`, where a ray first crosses a ring;
- `segment_distance`.

They have 10 new checks.

### 2.2 The Architecture Domain's `rules`, `form` and `frontage`

- **`rules`.** A lot (a ring, and which of its edges face a street) and the rules a planning scheme states, declared by the caller:
  - a height limit and a plot ratio;
  - setbacks from the street and from the other edges;
  - a street wall, above which the building steps back by an upper setback;
  - the share of the street line to be walled, and the storey.

  It holds the envelopes they allow (physis's inset), whether a massing complies (each failure named), and a lot cut by a passage, its edges along the passage facing it as a street.
- **`form`.** A massing from a few parameters:
  - a podium on the ground envelope;
  - an upper part on a share of the upper envelope, moved in evenly so that it stays within it whatever the shape;
  - their storeys, clipped to the height limit and taken off while the floor exceeds the plot ratio.

  A massing so made complies by construction. Its measures are floor area, coverage, volume, surface and compactness, and it gives its prisms for the Sun.
- **`frontage`.** The street wall: the share of a lot's street line the podium stands on, its height, and its active length. Its symbolic part stays planned.

### 2.3 Sun and Shade, and the Day's Heat, in `environment`

- **`sun.hpp`.** Buildings as prisms on flat ground; receivers on the ground and on faces.
  - **The threshold.** A receiver at height z is shaded at an instant by a prism of height h exactly when h > z + t_in tan α, with t_in the ray's first entry into the footprint. Thresholds are computed once, and a design that changes heights is evaluated by comparisons.
  - **Faces.** A face takes the Sun only in front of it. A receiver against another building (a party wall) is covered while that building stands above it, again a threshold.
  - **Its origin.** Promoted from the district study's provisional model, and widened to faces.
- **`balance.hpp`.** The heat a massing needs over a day. Loss through its open walls and roofs, less the sol-air gains on its sunlit open walls and roofs, with the domain's fabric and Meinel's clear sky, clipped at zero at each instant. It is coarser than `massing_heat.hpp` (no storage), and quick enough for thousands of designs.

There are 43 new checks in the domain, held to closed forms: envelopes known by hand, shadow lengths, a face's edge at atan(1.2), thresholds against a direct test of every ray, a box's balance with a party wall and with the Sun.

### 2.4 One Composition in the API

`api/cpp/science/design.hpp` (`science::design::Designer`), after the toolbox's place (ADR 0080), so that no work depends on another:
- **The block.** The least bounded face of the streets' plane graph about a point (physis's plane faces).
- **The lots.** Its buildings, each a footprint as built. Parcels are not in the open data. An edge faces a street where a ray from it reaches the block's edge before another lot.
- **Passages.** One across the block's middle and one along it, 6 m wide; the lots they cross are cut.
- **Variants.** The block as built, without a passage, and with each passage. Each variant's receivers and thresholds are computed once: the open ground about the block, the neighbours' faces across the streets, its own walls.
- **The measures of a design**, each from its domain:
  - floor (`form`);
  - winter sun on the streets and on the neighbours' faces (`sun.hpp`);
  - the heat a winter day needs per m² of floor (`balance.hpp`);
  - the mean angular integration within 300 m at radius 800 m (urban science's configuration, ADR 0084);
  - the floor of the buildings about it within a five-minute walk of each lot's door, weighted by the lot's floor (urban science's access, morphe's shortest paths).
- **The constraints.** Keep a share of the floor standing now (half by default), and wall the street line the rules ask.
- **The search.** heuresis's NSGA-III on the six measures, each relative to the block as built; every massing on the front is checked against the rules, and any failure named.

### 2.5 The Workbench's Plug-in *Architecture* and the Studio

The plug-in, planned since ADR 0060, gains its first view, *what could be built* (lens Fabric), anywhere:
- **Choosing.** A block picked; three declared rule sets; the floor kept; how long to search.
- **The card.** A table of the six measures against the block as built, the front (floor against winter sun on the streets, by passage), and the designs best in each measure or balanced.
- **The map.** The design chosen stands in place of the block's buildings, which a new hollow of the visualization layer (`setHollow`) hides from the drawing and from the shadows. The hours of sun on the open ground stand about it.
- **The Studio.** A layer may now give figures to a view captured in Explore, and the Studio lays them as chart frames: the front's five trade-offs arrive in the Studio with the view.

The plug-in has 15 checks on a grid of streets whose block is known by hand.

## 3. Alternatives Considered

- **A straight skeleton for insets.** Deferred: the mitred offset is checked and exact where edges do not vanish, and the fallback is conservative, so every envelope lies within the rules.
- **Scaling the upper part about the envelope's centroid.** Rejected after the first runs: on an L it leaves the envelope (48 of 2,003 massings failed the rules on a real block). Moving it in evenly keeps it within.
- **The heat of the massing with its storage (`massing_heat.hpp`) inside the search.** Deferred: it needs the massing's complex for each design. The balance compares envelopes, and the massing's heat can be run on the front.
- **Parcels from a cadastre.** Deferred: not open in most places. Footprints stand in, and the study says what that changes.
- **Uses on each floor.** Deferred: access then counts floor, not homes and jobs.

## 4. Consequences

- **What could be built on three blocks of Melbourne** (`research/experiments/2026-11-09-what-could-be-built`, on the data library's city model):
  - **Less floor.** In the Hoddle Grid, a street wall to 24 m with towers set back 10 m holds at most 96–98% of the floor standing on Collins Street's block, and 78% on Bourke Street's. Mid-rise's plot ratio of 4.5 cannot keep half.
  - **More sun.** Winter sun on the streets rises by up to 38–54%, and by 10–16% while four fifths of the floor are kept.
  - **No warmer.** No design is warmer per m² than what stands: its buildings fill their lots and share their walls.
  - **A passage.** It moves integration within 300 m by at most 0.2%. Most designs open the one across the block.
  - **Carlton.** The rules can double the floor of its row houses.
- **Every massing on every front lies within its rules.** Over the study's 14 runs with a front that is 35,093 massings, each checked.
- **Next.**
  - Parcels where cadastres are open.
  - Uses by floor, so that access counts homes and jobs, and the preferences of ADR 0088 can score a design.
  - The massing's heat with storage for the front.
  - Diffuse light and shaded roofs.
  - Stage 6 of the priorities, land use and transport interacting.
