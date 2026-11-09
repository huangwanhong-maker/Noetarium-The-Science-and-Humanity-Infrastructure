# What Changes to the Streets Did, in Two Cities

*Status: draft (2026-11-09). Research.* The first study of urban science's interventions on peira's interference ([ADR 0087](../../../development/decisions/0087-what-a-change-did-and-around-it.md); `domains/urban-science/interventions`, `foundations/peira/interference`).

## 1. The Question and Why

A change to a street acts where it is made and beyond it:
- a protected lane draws riders onto itself, and perhaps from the streets one could ride instead;
- a large building brings people to its door, and perhaps through the streets about it.

The effect on the street itself and the spillover around it are both wanted. The spillover also contaminates the usual comparison, since the sensors near a change are no longer untouched. The study asks, of two records the data library holds:
- **Melbourne:** what a major building completed did to the walkers counted near it;
- **New York:** what a protected cycle lane did to the riders counted on it and near it.

It also asks a second question, of the method rather than the cities: whether one code path, written for no city, carries both. Its answer is yes. Both cases run through `data/products/street_changes.py`, the domain's `evaluate`, this study's `changes.cpp` and the workbench's plug-in *What a change did*, unchanged.

## 2. The Method

- **The records** (`data/products/street_changes.py`; each case a recipe of sensors with their outcome by period, interventions with their footprints and periods, and the ways about the sensors):
  - **Melbourne:**
    - *sensors:* the City's pedestrian counters, their monthly means taken to years over the counted months, 2009–2019; 35 sensors with three such years (CC BY 4.0). The pandemic's years are left out: they changed the centre's walking too unevenly between places for parallel trends.
    - *interventions:* the Development Activity Monitor's 87 developments completed 2010–2019 with at least 20,000 m² of non-residential floor space, or 300 dwellings, 300 student beds or 200 hotel rooms (a declared threshold), each a point standing from its year of completion.
  - **New York:**
    - *sensors:* NYC DOT's bicycle counts, 15-minute counts summed by the portal into months; 56 sensors with 24 months at least 90% counted, January 2013 to June 2026.
    - *interventions:* the Bike Routes' 6,368 segments of class I (protected lanes and paths) installed within the panel, each a line standing from its month of installation. For an upgraded segment, the installation date is read as the protected lane's: an assumption.
  - **Ways:** Overture Maps' world tiles about the sensors (ODbL): every way for walkers but motorways, and for riders also no footways, sidewalks or steps.
- **Distances along the ways** (`urban-science/access`, `interventions`):
  - the ways become a network whose times are metres, its loose ends stitched within 8 m to points and to ways (`stitch_to_ways`, written for this study: the tiles simplify their lines and drop the vertices where side streets met them);
  - each footprint is sampled every 25 m;
  - a search bounded by the outer ring from each footprint gives every sensor's distance.
- **The exposure mapping** (peira `interference`). A sensor's ring in a period is set by the nearest change standing by then, and a sensor beyond the outer ring is unexposed. The rings were declared before any result was seen:
  - **Melbourne:** 200 and 600 m (a block, and eight minutes' walk);
  - **New York:** 100, 600 and 1,500 m (the lane's own street, the parallel avenues, the district).
  
  Other sets are reported beside them, every one that was run.
- **The estimators.** The outcome is the logarithm of the mean daily count, so effects read as proportions.
  - **Rings in a two-way regression.** Sensor and period effects, an indicator for each ring, errors clustered by sensor (Butts 2021).
  - **Rings with timing that varies.** Callaway and Sant'Anna's group-time effects of entering each ring, against the sensors not yet reached by any change, with errors from 499 bootstrap draws of sensors.
  - **Each sensor's own change.** Across its entry, against those not yet reached, for the map.
- **What the record can tell.** For every ring: how many sensors entered it, how many of them were counting before their change came, and how many sensors were left to compare with.

`changes.cpp` runs a case in 0.5 s (Melbourne) to 48 s (New York's 6,368 footprints).

## 3. What Was Found

1. **Neither record can say much, and each fails for its own reason.**

   | | Sensors | Entering the inner ring | Of them counting before | Entering the outer rings | Of them counting before | Never reached |
   |---|---|---|---|---|---|---|
   | Melbourne (200, 600 m) | 35 | 1 | 1 | 27 | 13 | 7 |
   | New York (100, 600, 1,500 m) | 56 | 4 | 2 | 8 + 43 | 0 + 12 | 1 |

   - **Melbourne.** Nearly every counter is first reached by a building 200–600 m away, so the direct ring is almost empty.
   - **New York.** The city counts its lanes once they are built: of the 30 sensors whose first lane within 400 m came within 100 m, 23 began counting after it. Its protected network is so dense that within 1.5 km all but one sensor is reached, and no sensor is left to compare with for the inner rings.
2. **Melbourne: no effect of a major building is detectable.**
   - Under the declared rings:
     - the two-way regression gives −13% (95% interval −36% to +17%) within 200 m and −2% (−13% to +11%) at 200–600 m;
     - against those not yet reached, +11% ± 12% at 200–600 m.
   - The direct estimate rests on one or two sensors, and changes sign with the ring's bound: +24% (+15% to +34%) when the ring is 150 m and holds two sensors.
3. **New York: the riders on a new protected lane may double, but the record cannot establish it.**
   - **The two-way regression** gives +100% (−5% to +320%) within 100 m under the declared rings, and +96% to +98% under every narrower set. It is consistent, but rests on the few sensors that saw their lane come, and on effects the same everywhere.
   - **Against those not yet reached** the estimates are unstable: +990% under rings of 100 and 400 m, from about seven sensors against one or two comparisons. The event study's placebos before entry lie between −0.1 and −0.2. The rise comes eleven months after the recorded installation, which may be when the lanes opened, or noise.
   - **The spillover** within 400 m is positive in the regression (+40%, +5% to +87%) and negative or nil in wider rings. No consistent sign can be read.
4. **The general lesson, as a requirement.** To identify what a change did and what it spilled, a record needs:
   - sensors counting before the changes come;
   - a pool of sensors not yet reached when each change comes;
   - rings narrower than the spacing of the changes.
   
   The plug-in shows the first two for any case, beside its estimates, so that a reader sees what the record can tell before reading what it says.

## 4. What It Means, and What It Does Not

- **What was built is general.** Distances along any city's ways, rings of exposure, both estimators, the diagnostics and the map serve any record of changes and counts. A third city or another kind of sensor (air monitors about a low-emission zone, footfall about a station) is a new case in the data library, with no new code.
- **The null results are about the records, not about buildings or lanes.**
  - **Melbourne:** the counters stand where large developments are everywhere.
  - **New York:** the counters were placed on lanes after they were built, by design: they measure lanes, not the change a lane makes.
  - **Better records:** a city that wants to know what its changes do can count before it changes, beside the change and a little away from it.
- **Not tested here.**
  - Effects that vary by the kind of change (offices against homes; a protected lane against an upgrade).
  - The seasonality of riding, beyond what the period effects absorb.
  - Parallel trends beyond the placebo periods.
  - The randomisation test of no spillovers, which needs a randomised design: none of these changes was randomised.

## 5. Running It

```
python data/products/street_changes.py melbourne-developments newyork-lanes
cmake -S research/experiments/2026-11-09-street-changes -B ~/.cache/science-build/exp-changes -DCMAKE_BUILD_TYPE=Release
cmake --build ~/.cache/science-build/exp-changes -j2
~/.cache/science-build/exp-changes/changes data/cache/products/street-changes/<case>.json research/experiments/2026-11-09-street-changes/results/<city>-<rings>.json --bounds <rings> --replications 499 --window <2 | 12>
python research/experiments/2026-11-09-street-changes/analyse.py
```

`results/` holds the runs (committed) and `summary.json`. The cases' files carry ways (ODbL) and stay in the data cache. The figures are the gallery's §23.
