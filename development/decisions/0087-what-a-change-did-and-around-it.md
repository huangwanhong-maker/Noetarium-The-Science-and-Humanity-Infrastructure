# 0087. What a Change Did, and Around It

- **Status:** Accepted, provisionally (2026-11-09).
- **Date:** 2026-11-09
- **Decided by:**
  - **The order.** Stage 3 of the urban priorities (`development/ideas/2026-11-08-urban-science-priorities.md`): the causal evaluation of street changes.
  - **The condition.** The user asked that what is built be reusable: "We don't want it just for a city, and a specific visualization task."
  - **The layers.** Claude proposed five, a foundation's module among them, with a second case of another kind to prove reuse.
  - **The go-ahead.** The user: "Good! Begin."
  - **The design and the choice of New York's lanes as the second case.** Claude.

## 1. Context

A change to a street acts where it is made and beyond it. A lane draws riders, perhaps from the parallel streets; a building brings walkers through the streets about it. The spillover is an effect, and it contaminates the comparison. peira's quasi-experiments assumed no interference. The workbench had no way to show what a change did. Built for one city and one picture, an evaluation would serve neither the next city nor another kind of change.

## 2. Decision

Five layers, each general where it stands.

### 2.1 Interference in peira

A module `interference` (peira's decision 0006):
- exposure mappings by rings of distance in any metric, with onsets and entry rings;
- the two-way regression on rings (Butts 2021);
- staggered adoption by ring against units not yet reached, through quasi's estimator, which gains `reached`;
- clean donors for synthetic control;
- Athey, Eckles and Imbens's exact test of no spillovers.

It has 14 checks, against designs whose answers are known exactly.

### 2.2 Interventions in Urban Science

A module `interventions`:
- the network distances from sensors to the footprints of changes, as the mapping's contacts;
- each sensor's own change across its entry;
- `evaluate`, the whole chain in one call.

Nothing in it names a city. urban science's `access` gains `stitch_to_ways`, which joins loose ends to the nearest point of another way. The world's tiles simplify their lines and drop the vertices where side streets met them; Melbourne's network was in 7,875 pieces before it.

### 2.3 Records as Cases in the Data Library

`data/products/street_changes.py`: a case is a recipe of sensors with outcomes by period, interventions with footprints and periods, and the ways about the sensors from the world's tiles. Two cases are written:
- **Melbourne:** its major developments, 2010–2019, and its walkers;
- **New York:** its protected lanes, 2013–2026, and its riders, with `data/registry/nyc-bicycle-counts.json`.

A city is added by adding a case.

### 2.4 Marks of Effects in the Visualization Layer

`visualization/js/gis/effects.js`, for any work that shows what something did where:
- estimates at places, on the validated diverging map, paler where within two standard errors;
- footprints that stand from a period, strong once standing, faint while to come.

### 2.5 A Plug-in Anywhere a Record Is Held

The workbench's *What a change did* (lens Change) works on any case:
- the changes come with the clock, and each sensor is coloured by its own change;
- its card gives the effects by ring under the declared rings, narrower or wider, by both estimators, with their event studies;
- the card says what the record can tell: how many sensors were counting before their change, and how many were never reached.

## 3. Alternatives Considered

- **A Melbourne study alone.** Rejected at the user's word: it would be one city and one picture.
- **Distance over the ground.** Rejected: a spillover travels along the ways one could take, not across a river.
- **The city model's network for Melbourne.** Not taken: the world's tiles serve every city alike, and once stitched to ways they agree with it.
- **Randomisation inference for these changes.** Not applicable: none was randomised. The exact test is there for designs that are.

## 4. Consequences

- **What the two records show** (`research/experiments/2026-11-09-street-changes`):
  - **Melbourne.** No effect of a major building on the walkers near it is detectable: −13% (−36% to +17%) within 200 m, −2% (−13% to +11%) at 200–600 m.
  - **New York.** The riders on a new protected lane may double (+100%, −5% to +320%, in the regression), but the record cannot establish it.
  - **Why.** In Melbourne, a building farther away reaches almost every counter first. In New York, 23 of 30 counters on new lanes began counting after them, and its dense network leaves almost no sensor unreached.
  - **The lesson, as a requirement.** A record that can tell what a change did needs sensors counting before the change, and a pool not yet reached when it comes. The plug-in shows both.
- **Next.**
  - The access plug-in's reach and services stitched to ways too: their figures in ADR 0086 were computed with loose ends joined to points only.
  - Effects by the kind of change.
  - Seasonality in monthly outcomes.
  - Cases of other sensors: air monitors about a low-emission zone, footfall about a station.
  - Stage 4, demand estimation, which can use the same records.
