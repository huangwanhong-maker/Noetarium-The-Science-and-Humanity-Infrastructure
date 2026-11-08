# The Configuration of Melbourne's Streets, Against Its Walkers

*Status: draft (2026-11-08). Research.* The first study of urban science's configuration measures ([ADR 0084](../../../development/decisions/0084-the-configuration-of-streets.md); `domains/urban-science/syntax/include/urban/syntax/configuration.hpp`, on morphe's centralities). It asks how much of where Melbourne's walkers are counted the shape of its street network explains, and compares the answer with the walkers' model of the district-economy flow study.

## 1. The Question and Why

Space Syntax holds that a street network's configuration, how each street stands in the whole, shapes movement before any destination is considered: Hillier's "natural movement" (Hillier, Penn, Hanson, Grajewski and Xu 1993, *Environ. Plann. B* 20, 29–66). Its measures are centralities of the segment map:
- **integration** is closeness: how near a segment lies to all others;
- **choice** is betweenness: how often a segment lies on the least paths between all others.

Their normalised forms are NAIN and NACH (Hillier, Yang and Turner 2012, *J. Space Syntax* 3, 155–193), at an angular, metric or topological cost and within a radius. In London such measures have correlated with counts at gates at r ≈ 0.6–0.8.

Melbourne counts its walkers by the hour at about ninety places. The walkers' model of the flow study, which predicts from destinations and street widths, fits those counts hour by hour with a gross R² of 0.72. The two together allow a test with a ground truth.

## 2. The Method

- **The networks.** The data library's city model of Melbourne's centre (OpenStreetMap, ODbL; 3.9 × 4.6 km), in its exact local frame (`prep.py`). Three networks are read:
  - **streets** (the primary network): the street centre lines, footways, paths, steps, cycleways, motorways and busways left out. 7,941 ways become 8,649 junctions and 11,304 segments.
  - **walking**: the streets with the footways, paths and steps: 34,951 segments.
  - **roads**: the streets without service ways and tracks, which in Melbourne are mostly laneways: 5,660 segments.
  Each becomes urban science's street graph (points within 1 m one, bends over 20° kept) and its segment map.
- **The measures.** For each counter, the configuration of its nearest segment (median distance 7 m): NACH, NAIN and log(choice + 1).
  - On the streets: at every cost (angular, metric, topological) and radius (400, 800, 1,200, 2,000 m and n).
  - On the walking network: angular at 800 m and n.
  - On the roads: angular at 400, 800, 1,200 m and n.
  `configuration.cpp` computes them all; the whole network at radius n takes 5 s on 8 threads.
- **The counts.** The City's Pedestrian Counting System, 30 October 2024 to 29 October 2026 (CC BY 4.0). Each counter's mean daily count is taken over the days with all 24 hours counted, both directions summed, for the 87 counters with at least 60 such days. The response is its logarithm.
- **What draws walkers.** The City's Census of Land Use and Employment (2024): the non-residential floor space of the blocks whose centres lie within 250 m of each counter.
- **The statistics** (`analyse.py`). For each measure: Spearman's ρ, and the least-squares line's R², and its R² with each counter left out (from the hat matrix).
  - **Primary tests.** Four were declared before any result was seen: on the streets, angular NACH and NAIN at 800 m and at n. Each has a permutation p-value for ρ (20,000 permutations), adjusted by Holm's method; everything else is exploratory.
  - **Edge effects.** At each radius, the counters at least that far from the network's edge are analysed apart.
- **Against the walkers.** On the flow study's 16 counters, the daily totals of its observed mean week are set against two predictions, each made with that counter left out:
  - its own leave-one-out predictions;
  - the best primary measure's line.

## 3. What Was Found

1. **The shape of the streets explains almost none of where walkers are counted.** None of the four primary tests survives correction:

   | Angular, on the streets | ρ | R² | R², left out | p | p, Holm |
   |---|---|---|---|---|---|
   | NACH at 800 m | −0.05 | 0.01 | −0.04 | 0.68 | 1.00 |
   | NAIN at 800 m | 0.22 | 0.02 | −0.03 | 0.037 | 0.15 |
   | NACH at n | 0.07 | 0.00 | −0.05 | 0.55 | 1.00 |
   | NAIN at n | 0.09 | 0.00 | −0.05 | 0.41 | 1.00 |

   - **Exploratory measures.** Of the 59 other pairs of measure and network, none has a positive R² with each counter left out except among the counters away from the edge. The largest ρ is 0.32 (topological integration at 1,200 m), and 0.39 among the 60 counters at least 800 m from the edge (topological integration at 800 m, R² left out 0.02).
   - **Choice relates to nothing.** At every cost and radius, choice lies between ρ −0.19 and 0.20.
   - **Local integration is weakly positive.** Integration at 800–1,200 m is weakly but consistently positive.
2. **Not a restricted range.** The counters stand on busier-than-average streets (their mean percentile of choice at radius n is 0.72), but their configuration varies as widely as the network's: a standard deviation of NACH of 0.42 among them against 0.44 in the network.
3. **Laneways and footways do not change it.** Without the laneways, every measure stays within ρ ±0.26 and below zero left out. With the footways, ρ is at most 0.20.
4. **Floor space explains a little more, and not much.** The non-residential floor space within 250 m gives ρ 0.24, R² 0.08, and 0.04 with each counter left out. With integration beside it, R² is 0.08, and 0.01 left out.
5. **The walkers' model does no better between places.** On the flow study's 16 counters and their daily totals:
   - its predictions with each counter left out have R² −0.80 (−0.04 in-sample), ρ 0.54;
   - configuration's line has R² 0.00 left out (0.18 in-sample), ρ 0.49.

   Its gross R² of 0.72 measures how well it follows the hours of the day and week, not the differences between places.
6. **What the counts follow.** The counters most above the line are Southbank Promenade, QV on Swanston Street, Flinders Lane at Swanston Street, Town Hall and Bourke Street Mall: each at a station, a promenade or a mall. Those most below it are Westwood Place (a laneway) and three on Spencer Street. A counter's daily volume follows what stands at it (stations, malls, the river) and where it was placed, more than how its street stands in the network or how much is built around it.

## 4. What It Means, and What It Does Not

- **What the measures don't do here.** Melbourne's counters were placed where the City wished to watch walkers: busy places in a regular grid. Among them, configuration does not rank volumes.
- **What isn't refuted.** This does not refute natural movement. Hillier's tests sampled gates across whole areas, quiet streets among them, and counted every footpath alike; Melbourne's counters each watch one footpath at a chosen place.
- **The walkers' model.** It keeps its fit in time, and gains a measured limit in space: between places it predicts no better than a constant.
- **What would answer the question.** Gates sampled across the network, or the Census's journeys to work assigned to the streets. Stations, entrances and malls should be modelled as attractors on the network itself, not as floor space in a disc.

## 5. Figures

In `assets/gallery/` (the gallery's §21): `config-01-choice-n.jpg` and `config-02-choice-800.jpg` (the map of angular choice, the counters by their counts), `config-03-by-radius.jpg`, `config-04-against-walkers.jpg`, `config-05-configuration-and-attraction.jpg`.

## 6. Reproducing It

From the repository's root:

```
python research/experiments/2026-11-08-street-configuration/prep.py
cmake -S research/experiments/2026-11-08-street-configuration -B ~/.cache/science-build/exp-configuration -DCMAKE_BUILD_TYPE=Release
cmake --build ~/.cache/science-build/exp-configuration -j2
~/.cache/science-build/exp-configuration/configuration research/experiments/2026-11-08-street-configuration/results/inputs.json research/experiments/2026-11-08-street-configuration/results --threads 8
python research/experiments/2026-11-08-street-configuration/analyse.py
```

`results/inputs.json` and `results/maps.json` carry the ways (ODbL) and are not committed; `prep.py` and `configuration` make them again. `counts.json`, `measures.json`, `results.json` and `run.log` are committed.

## 7. Limits

- **The segment map.** It is built from the city model's ways clipped to its box, so paths that would leave the box are cut. That is an edge effect, which the analysis at each radius shows apart.
- **Matching.** A counter is matched to its nearest segment, which at a corner may be the cross street. The median distance is 7 m, but a tenth lie more than 34 m away (promenades and park paths, absent from the street centre lines).
- **The counts.** A counter watches one footpath, so the measure of a street with two footpaths is set against one of them.
- **Declared parameters.** Floor space is read in a disc of a declared radius, and the topological cost's threshold of 30° is declared.
