# 0086. Access to Services, and Who Is Left Without

- **Status:** Accepted, provisionally (2026-11-08).
- **Date:** 2026-11-08
- **Decided by:**
  - **The order.** Stage 2 of the urban priorities (`development/ideas/2026-11-08-urban-science-priorities.md`): "Accessibility and unmet need, anywhere. Gravity and cumulative opportunities; two-step floating catchments; the Rural Access Index".
  - **The go-ahead.** The user: "please implement the next priority one".
  - **The design.** Claude.

## 1. Context

The workbench could say how far one can go from a place picked (ADR 0078), but not who can reach what they need. Planners, health services and development banks ask for the second. A clinic's capacity is shared among everyone who can reach it, so counting what lies within reach of each place overstates access where many people share one clinic. The two-step floating catchment shares supply out, and gives each place its share per person. The same question in the countryside is the World Bank's Rural Access Index (SDG 9.1.1): who lives within 2 km of an all-season road.

## 2. Decision

### 2.1 The Measures in nomos

nomos's `measurement` gains `accessibility.hpp` (its decision 0008), on a sparse list of the pairs within reach and their costs, however the caller found them:
- decays of a cost (binary, exponential, Gaussian within a limit);
- cumulative opportunities and gravity;
- the two-step floating catchment with a decay in both steps, which conserves supply: Σ_i P_i A_i is the supply some demand reaches;
- unmet need (the people with nothing within reach, those below a declared share of the mean) and the person-weighted Gini coefficient of access;
- the Rural Access Index.

They name no object in nature, and hospitals, schools and labour markets in the countryside need them as much as streets do.

### 2.2 The Network in Urban Science

urban science's `access` gains `catchment.hpp`: places of demand and of supply joined to a place's network of ways (ADR 0078) at their nearest points, the pairs within a time by Dijkstra's search from each supply bounded by the limit (morphe), the time to the nearest supply by one search from all, and the distance to a road.

A place may join only a connected part of the network holding at least 1% of its points, and always the largest. A way closed to a mode (a footway to a car, a motorway to a walker) leaves its stubs, stitched at their loose ends, as islands of a few points. A place joined to one reaches nothing. Before this rule, a quarter of inner Melbourne's people appeared to have no care within 15 minutes by road, some of them 70 m from a hospital.

### 2.3 The Layer in the Workbench

The plug-in *Reach and use* gains a layer, *access to services, and who is left without*, anywhere:
- **People.** GHS-POP R2023A's cells of 30 seconds of arc, about 1 km, with GHS-SMOD's degree of urbanisation, read about a point by the server (`/world/population`, `data/acquisition/services.py`).
- **Services.** The buildings Overture and OpenStreetMap call care, schools or shops, each offering its floor space (footprint × storeys).
- **Ways.** Those loaded about the view, at a mode's declared speeds.
- **Window.** Only the cells within the world's tiles whose ways have come are measured, and the people of the others are counted as left out, never as without.
- **Drawn.** Each cell by its access, draped on the ground as GHS-POP's own grid, so that it follows the hills; the cells with nothing within the limit hatched; the services by their floor space, on their roofs. A card gives the share within reach, the people with nothing, those below half the mean, the Gini coefficient of access, and the Rural Access Index where cells are rural.
- **Hatching.** The visualization layer's drape gains one value drawn as hatching by the Earth's shader, in stripes fixed to the screen, so that it stays crisp at every distance and stands apart from the palette's rose.

### 2.4 What It Shows

On 8–9 November 2026, with the buildings and ways of Overture's release 2026-10-21.0:

| Place, service, mode | People in the window | Services mapped | Within reach | Access a person | Gini |
|---|---|---|---|---|---|
| Melbourne's centre, care, 15 min on foot | 651,312 | 168 | 55% | 1.42 m² | 0.87 |
| the same, shops, 15 min on foot | 651,312 | 854 | 83% | 2.45 m² | 0.74 |
| the same, care, 15 min by road | 651,312 | 168 | 100% | 1.47 m² | 0.13 |
| Freetown, care, 15 min on foot | 1,717,915 | 49 | 37% | 0.04 m² | 0.88 |
| Dhulikhel, care, 30 min by road | 83,454 | 16 | 99% | 0.19 m² | 0.20 |

The Rural Access Index is 99% of Dhulikhel's 19,018 rural people. Freetown's mapped care is a thirty-fifth of Melbourne's per person on foot, and its east is out of reach of all of it. The measures are exact for the data given. What they show is first the map's coverage: neither city maps its general practices as buildings of care.

## 3. Alternatives Considered

- **The measures in urban science.** Rejected: nothing in them is a street (§2.1).
- **Places of interest as services.** Overture's places name more services than its buildings do, but carry no size. Floor space is a declared proxy for capacity, and a building's kind is what was mapped; both are stated on the page.
- **The Rural Access Index on mapped surfaces.** Rejected for now: few roads carry a surface or a season. Motorways to tertiary roads are taken as all-season, and this is stated.

## 4. Consequences

- nomos gains 8 checks; urban science's `access` gains 7: the catchment on a network by hand, and an island that may not be joined.
- **Limits, stated on the page:** a service is what is mapped; floor space is not capacity; the window ends where the ways loaded end, and people beyond it are left out and counted; a cell is one place, at its centre.
- **Next:** registries of facilities as supply where the map is thin (healthsites.io; the WHO's list of public health facilities in sub-Saharan Africa, Maina et al. 2019); access over the hours (opening times, as Street Oceanography holds them), public transport, and competition among services (the inverted catchment); stage 3, the causal evaluation of street changes, on peira's `quasi`.
