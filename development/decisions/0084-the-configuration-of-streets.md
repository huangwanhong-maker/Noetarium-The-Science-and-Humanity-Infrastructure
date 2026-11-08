# 0084. The Configuration of Streets: Space Syntax's Measures, Against Melbourne's Walkers

- **Status:** Accepted, provisionally (2026-11-08).
- **Date:** 2026-11-08
- **Decided by:**
  - **The request.** The user asked for the next of the urban-science priorities (`development/ideas/2026-11-08-urban-science-priorities.md`, stage 1). Asked whether it merely "induce[s] the street syntax, and produce[s] some representations", they were answered that it is a measurement of how each street stands in the whole, testable against counts and linked to the grammars of ADR 0083. They then said "proceed".
  - **The design and the study.** Claude.

## 1. Context

Space Syntax (Hillier and Hanson 1984; Hillier 1996) measures a street network's configuration by centralities of its segment map:
- **integration**, closeness;
- **choice**, betweenness.

Both are taken at an angular, metric or topological cost, within a radius, and normalised as NAIN and NACH (Hillier, Yang and Turner 2012). Its theory of natural movement (Hillier et al. 1993) holds that configuration shapes movement before destinations do.

The infrastructure held no centrality at all, and its walkers' model of Melbourne's centre had been judged by an R² of 0.72 on hourly counts. Despite their names, the measures are not a grammar. Geometric syntax (ADR 0083) says what a network is made of; configuration says how each part stands in the whole.

## 2. Decision

### 2.1 morphe: Centralities

Closeness and betweenness by Brandes's algorithm, within a radius, on graphs whose arcs carry a cost and a length (morphe's decision 0012). Least paths are least in cost, then in length, which keeps their counts finite where costs are zero (a street running straight on costs nothing under an angular cost). Checked against networkx 3.4.2 and against brute force.

### 2.2 urban science: The Configuration of Streets

The module `syntax` gains `configuration.hpp`:
- the segment map of the street graph, joined where segments meet, with the angle turned and the metres between centres;
- the three costs, the cost of a right angle being 1;
- node count, total and mean depth, choice, NAIN and NACH for every segment, the sources spread over calx's thread pool with the same result on any number of threads;
- the segment nearest a point.

### 2.3 The Study

`research/experiments/2026-11-08-street-configuration` sets 63 measures (three networks, costs and radii) against the mean daily counts of Melbourne's 87 counters (2024–2026). Four primary tests were declared before the results were seen.

What it found:
- **Configuration explains almost none of the spatial variation.** No primary test survives Holm's correction (the best, angular NAIN at 800 m: ρ 0.22, R² 0.02), and no measure has a positive R² with each counter left out.
- **The usual explanations don't hold.** The result is not a restricted range, and laneways and footways do not change it.
- **Floor space adds little.** The floor space within 250 m explains a little more (R² 0.08).
- **The walkers' model does no better between places.** On its own 16 counters, its leave-one-out R² of the daily totals is −0.80, against configuration's 0.00. Its 0.72 measures the hours, not the places.
- **What the counts follow.** What stands at a counter: stations, malls, the river.

### 2.4 The Workbench

*Geometric syntax* gains a layer, *how each street stands in the whole*, for any place:
- its streets within 1.5 km read as a whole, coloured by the rank of their choice or integration, at the three costs and four radii;
- in Melbourne, the City's counters on the map at their mean daily count (read by the engine from the data library's street-counts), with ρ and R² for the measures shown.

The grammars meet configuration in two places:
- **Rules.** Each rule of a learned grammar carries the mean choice of the streets it joins, and those above the median are marked: the rules of the through streets.
- **Samples.** A sampled network is compared with the streets by its integration and choice as distributions. In Melbourne's centre, a context-sensitive sample keeps the city's choice (distributions 10% apart) but not its integration (47% apart): the grammar reproduces local shapes and builds a less connected whole.

## 3. Alternatives Considered

- **Axial maps.** Rejected: the fewest longest lines are ill-defined and hard to compute; segment maps from centre lines are their accepted successor (Turner 2007).
- **Comparing with the hourly R² of the walkers' model.** Rejected as unfair: configuration predicts places, not hours. Both were set against the same daily totals.
- **Choosing the best of many measures without saying so.** Rejected: the primary tests were declared, the rest are reported as exploratory, and R² is reported with each counter left out.

## 4. Consequences

- **The next urban priorities gain a baseline.** For stage 2 (accessibility) and stage 4 (demand), configuration alone ranks Melbourne's counted places no better than a constant. Attractors on the network itself (stations, entrances) come first.
- **The grammars gain a second check beyond local shape.** Configuration of the whole now scores sampled networks: a test that the PCSG's samples do not yet pass for integration.
- **The gallery's §21** holds the study's figures and the layer.
