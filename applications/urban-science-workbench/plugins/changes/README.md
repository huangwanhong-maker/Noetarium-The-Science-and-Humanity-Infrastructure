# What a Change Did

*What a change to the streets did to what is counted there, and near it.* Status: **draft**. Activity: research. [ADR 0087](../../../../development/decisions/0087-what-a-change-did-and-around-it.md).

## 1. Layer

**What a change did, and around it** (anywhere a record of changes is held). The cities whose records the data library holds are listed on the card; the one near the view opens itself.
- **On the map.**
  - The changes come with the clock: those standing strong, those begun within the last period brightest, those still to come faint.
  - Each sensor is coloured by its own change across its entry against the sensors not yet reached, on the visualization layer's diverging map (−39% to +65%).
- **On the card.**
  - The effects by ring of distance along the ways, under the rings declared for the case, or half or one and a half times them:
    - in the two-way regression, with its 95% interval;
    - against the sensors not yet reached, with its bootstrap standard error;
    - the event study of each ring.
  - What the record can tell: how many sensors were counting before their change came, and how many were never reached.
- **A sensor picked** shows its series, the period it was reached and its own change.

The records today:
- **Melbourne:** what a major building completed (2010–2019) does to the walkers counted near it.
- **New York:** what a protected lane installed (2013–2026) does to the riders counted on it and near it.

A city is added in `data/products/street_changes.py`, with no change here.

## 2. Engine

`engine/main.cpp`, on urban science's `interventions` and `access` and peira's `interference`:
- `cases`: the cases built, read from `street-changes/index.json` under the products;
- `evaluate`: one case under a set of rings.

A case's network is built once and kept, so a second set of rings answers in seconds.

## 3. Limits

- **The assumptions of the estimators.** Parallel trends, and an exposure mapping that holds: nothing reaches beyond the outer ring. The event studies' placebos test the first in part. The second is not testable; finer rings show where effects fall to zero.
- **What the records are.** A sensor that began counting after its change says nothing of what the change did. Where few sensors are left to compare with, the estimates against those not yet reached say "not estimable" rather than guess.
- **Distances.** They run along the ways of the world's tiles, stitched within 8 m, to the footprints sampled every 25 m.
- **The intervention's date is the record's.** A building's year of completion; a lane's date of installation.
