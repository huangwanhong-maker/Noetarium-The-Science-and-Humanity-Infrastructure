# architecture

*Status: draft. Field, height 0.* Buildings as objects of a science: their form, the spaces they enclose and join, the frontage at which a building meets its street, and the rules that bound them. Founded as a domain of its own at the user's word (2026-10-31, [ADR 0060](../../development/decisions/0060-streets-as-open-systems.md) §2.8); its first modules, `space` and `environment`, written for the formalism of landscapes ([ADR 0062](../../development/decisions/0062-landscapes-as-cellular-configurations.md), [ADR 0063](../../development/decisions/0063-the-district-as-a-complex-to-work-on.md)), C++ from the start: a building's rooms, and a district's buildings as one complex of cells. Its `rules`, `form` and `frontage`, with sun and shade on streets and faces in `environment`, came for the generative design loop ([ADR 0089](../../development/decisions/0089-what-could-be-built.md)): the envelope a lot's rules allow, a massing within it, its street wall, and the light and heat it takes.

## 1. Purpose

A building is a thing in the world with models of its own. Its massing stands in the city's light and casts its shadows. Its plan is a complex of rooms, courts and passages, which a person reaches by paths and sees across. Its frontage is the interface through which a holding meets the street. Its form answers to rules: heights, setbacks, plot ratios. No other domain holds these.

Urban science, the domain of streets and cities as open systems, stands on this one ([ADR 0067](../../development/decisions/0067-event-spacetime.md)): its buildings and their shared walls are among the volumes and interfaces of a city's event spacetime. This domain depends on nothing but the foundations. The Urban Science Workbench's plug-in *Architecture* draws it.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `form` | Buildings as solids and voids: footprints, heights and their bases, massing, setbacks, the envelope; the solids of a city model and what can be measured of them (volume, floor area, surface, compactness) | draft: a massing from a few parameters, a podium on the ground envelope and an upper part on a share of the upper envelope moved in evenly, its storeys clipped to the height limit and the plot ratio, so that it complies by construction; floor area, coverage, volume, surface, compactness; its prisms for the Sun (`form.hpp`, ADR 0089); 14 checks |
| `space` | Plans and districts as cell complexes (morphe's topology): rooms, courts and blocks as cells, their adjacency, the paths through them, visibility and reach (Hillier and Hanson 1984, *The Social Logic of Space*), computed on the complex | draft: a building as a box of rooms fitted to its footprint, a regular complex (a ball), what each room carries; doors and stairs opened (the same cells read otherwise), rooms merged and parted (new cells, the heat kept); the rooms reached from the entrance (`rooms.hpp`); a district's buildings as one regular 3-complex, each building a 3-cell, a wall two buildings share one 2-cell, corners pooled and T-junctions inserted, the buildings oriented outward, terraces as its components through walls (`massing.hpp`, [ADR 0063](../../development/decisions/0063-the-district-as-a-complex-to-work-on.md)) |
| `frontage` | The frontage as an interface between a holding and the street: openings, depths and thresholds measured at the holding, its physical component kept apart from its symbolic one | draft: the street wall, the share of a lot's street line its podium stands on, its height and its active length (`frontage.hpp`, ADR 0089); 7 checks. Openings, depths, thresholds and the symbolic component planned |
| `rules` | The rules that bound form: heights, setbacks, plot ratios, the envelopes they allow, and whether a building lies within them | draft: a lot and the rules a scheme states (height, plot ratio, setbacks, a street wall and the step above it, the street line to be walled), the envelopes they allow on physis's inset, whether a massing complies with each failure named, a lot cut by a passage (`rules.hpp`, ADR 0089); 13 checks |
| `environment` | A building in its environment: sun and shadow on its faces and the street, daylight, and later its heat and air (physis's continuum) | draft: the heat of its rooms as the Hodge Laplacian Δ₃ with the fabric in its stars, the Sun's sol-air gains through glass and opaque fabric, the building's thermal modes, each hour propagated exactly (`heat.hpp`); the heat of a district's buildings through their party walls and to the air and the ground, Δ₃ on the massing, by Crank–Nicolson (`massing_heat.hpp`); sun and shadow on the ground and on faces, a threshold on each building's height so that a design changing heights is judged by comparisons, and party walls covered (`sun.hpp`, ADR 0089); a day's balance of an envelope, losses less sol-air gains (`balance.hpp`); 9 checks. Daylight from the sky planned |
| `syntax` | The footprints and outlines that syntaxis's grammars read (ADR 0083): a district's footprints as one 2-complex in which shared walls share vertices (`footprints`), and their rings as a geometric graph (`outlines`) | draft: 4 checks |

## 3. Dependencies

| Source | What architecture uses from it |
|---|---|
| morphe | `topology`: regular complexes, cubical grids, their rewriting and homology; `formal`: footprints of rewrites |
| physis (`computational_geometry`) | rings clipped and moved inward edge by edge, the entry of a ray (decision 0025), plane faces |
| physis | `discretisation` (Hodge Laplacians with stars); later `geometry` (solids, frames) and `continuum` (air) |
| calx | linear algebra, matrix exponentials |

The Sun's altitude and azimuth are given to `environment` by its caller (the workbench takes them from astronomy), so the domain stays at height 0; civil engineering, when it is founded, will hold structures, and architecture will meet it there.

## 4. Used By

The Urban Science Workbench: its plug-in *Landscape Dynamics* fills a building picked on the map with rooms and follows its heat through the day (the analysis "A building's rooms"); its plug-in *Architecture* designs what could be built on a block anywhere, through the API's composition (`api/cpp/science/design.hpp`, ADR 0089), which the study `research/experiments/2026-11-09-what-could-be-built` also runs. Urban science, when it declares architecture.

## 5. Open Questions

- Where the line between architecture and civil engineering falls: form and space here, structures and their loads there.
- Parcels: where a cadastre is open, lots should be parcels rather than footprints (ADR 0089 §3).
- Which records of buildings come first: the city model's footprints and heights (OpenStreetMap; measured heights from the City of Melbourne's outlines), or plans of single buildings.
