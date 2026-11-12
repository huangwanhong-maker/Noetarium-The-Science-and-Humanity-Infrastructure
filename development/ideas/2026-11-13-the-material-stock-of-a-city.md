---
title: "The Material Stock of a City: Its Buildings' Mass and the Flows That Renew It"
status: seed
date: 2026-11-13
related: [ADR 0101 (ecology founded; the material stock deferred, §3), ADR 0102 (the Urban Studio, whose second plug-in this would be), the user's handbook of methods (§5, resource, environmental and economic accounting), architecture's `form` and `rules`, the Urban Science Workbench's buildings of any place (ADR 0078), peira's `survival` and `cohorts`]
---

# The Material Stock of a City: Its Buildings' Mass and the Flows That Renew It

*Written at the end of the night the ecology domain and the Urban Studio were built. The user had said: "forest is ok, building material stock can also do if have time". It was not built, because it asks a decision of placement first, which is the user's (CLAUDE.md §6.5).*

## 1. The Question

What does a city weigh, of what is it made, and what must flow through it each year to keep it? A building's mass is its floor area times a material intensity (kg of concrete, steel, timber, brick, glass per m²), which depends on its use, its structure and the period it was built in. A city's stock is the sum, and its flows are the stock's renewal: what is built in (inflow) and demolished out (outflow), each year.

## 2. What Is Already Here

- **Every building of any place:** footprints and heights from Overture Maps (ADR 0078), heights measured, from floors or estimated and said to be; Melbourne's CLUE adds use, floor space and construction year block by block.
- **Material intensities:** the RASMI database (Fishman et al. 2024, *Scientific Data*; open, its licence to be checked) gives intensities by region, use and structure for seven materials, with their ranges, which suits a declared scheme with bounds (as Market Studio's sizes carry ranges).
- **Lifetimes:** peira's `survival` holds the laws; a building's lifetime is a survival law, and a dynamic stock model is a renewal process over cohorts.

## 3. The Mathematics

For a stock $S(t)$ with inflows $I(t)$ and a lifetime law with survival $\bar F$:

$$S(t) = \sum_{c \le t} I(c)\,\bar F(t - c), \qquad O(t) = \sum_{c \le t} I(c)\,\bigl(\bar F(t - c - 1) - \bar F(t - c)\bigr), \qquad S(t) - S(t-1) = I(t) - O(t),$$

the inflow-driven model; the stock-driven one solves for $I(t)$ given a path of $S(t)$, a triangular system (Müller 2006, *Ecological Economics* 59: 142–156; Pauliuk's ODYM). Mass is conserved by construction. The bounds of the intensities carry to the stock exactly (sums of products of non-negative quantities, as Market Studio's sizes).

## 4. Where It Would Go

Three placements, for the user to choose:

1. **A domain of its own:** social metabolism, or industrial ecology. It would hold stocks and flows of materials and energy of a society, MFA, LCA's technology matrix and EEIO (the last on economics' Leontief model). README §4 does not list it yet; it would stand on economics and architecture.
2. **Modules of existing domains:** buildings' material stock in architecture (`materials`), and a city's in urban science. This is quicker, but it would scatter accounting that belongs together.
3. **The general part in a foundation:** the dynamic stock model, as cohort accounting with lifetime laws (peira's `cohorts` and `survival`, or a module of physis's `continuum` as a balance law), whichever of 1 or 2 is chosen for the rest.

My recommendation: (1) with (3). Material and energy accounting is a science of its own, with its own objects (goods, processes, sectors), and the handbook gives it a whole section.

## 5. A First Slice

- the dynamic stock model with its checks (conservation, closed forms for exponential lifetimes, the stock-driven inverse);
- a building's mass by a declared scheme of intensities with their ranges;
- Melbourne's centre by its CLUE blocks (floor space, use, construction year);
- any place's buildings by Overture's footprints and heights;
- a plug-in of the Urban Studio, *Materials and flows*: a city's stock by material and by block, its flows under a scenario of renewal, drawn on the map and sent to the Workbench.
