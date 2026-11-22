# 0154. Open Space and Who Can Reach It: SDG 11.7.1 under Its Readings

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue design and implementation while they slept. ADR 0151 left room for SDG 11.7.1, open public space within 400 m, which the survey *Evidence, methods and actions for steering cities* ranks next after access to public transport.

## 1. Context

ADR 0151 gave urban science `transit`, who lives within reach of public transport under every reading of SDG 11.2.1's open terms. Its sibling indicator, SDG 11.7.1, asks what share of a city's built-up area is open space for public use, free of charge, and UN-Habitat's metadata (last updated 2025-04-23) adds the share of people within 400 m of it along the street network. The infrastructure read no open land and computed neither share.

The indicator has open terms. Which land counts is a reading (the metadata lists parks, plazas, sports fields, community gardens, waterfronts and more, and removes recreation that charges a fee); it names no least size, where WHO Europe asks 0.5–1 ha within 300 m in a straight line; and land taken from a base map is, in the metadata's words, potential open public space until it is validated on the ground.

Overture's base theme maps land use and land cover anywhere, each polygon with its subtype and class. The polygons overlap: a park lies over grass.

## 2. Decision

1. **The data library's `world_city.open_spaces`** reads the base theme's `land_use` and `land` layers at its finest level for a box, each polygon with its layer, subtype, class and name, kept by tile.
2. **Urban science's `openspace`** (theory note `domains/urban-science/theory/openspace.md`, 12 checks):
   - **spaces** as polygons with holes and a kind the caller gives;
   - **the share of land** as the area of the counted spaces' union on a raster of declared cell, within an analysis box, so that overlapping land use and land cover are counted once;
   - **reach under a reading** (kinds, least area, distance, along the streets or in a straight line): a place inside a counted space, or within the distance of a point of its boundary sampled every 25 m, on transit's walking network.
3. **Urban science's `access` gains `Joiner`,** the nearest joinable point of a network found through a grid of buckets, the same point and gap as `join` (a check holds it to `join` on 2,000 places at five bucket sides). `nearest_supply` and `pairs_within` use it. A reading in Melbourne's centre, with some 20,000 boundary points, fell from 22 s to about one.
4. **The API's `science/openspace.hpp`** (`openspace.access`), answered by the transit plug-in's engine, and in `science.transit` the declared table `SPACE_KINDS`, which reads the base theme's subtypes and classes as kinds (park, plaza, pedestrian area, sports, green, garden, cemetery, golf course), and `open_land`.
5. **The Urban Studio's plug-in becomes *Public transport, open space and who can reach them*,** with the tool *Open space and who can reach it (SDG 11.7.1)*: four readings (parks and plazas; with pedestrian areas and sports grounds; with every green and open land, gardens and cemeteries; WHO Europe's parks of a declared least size within 300 m in a straight line), each kind of land alone, where people live and how far they walk, and the open land drawn by its kinds. Melbourne's template gains the step.

## 3. Alternatives Considered

- **Summing the polygons' areas.** Land use lies over land cover, and a park over grass would be counted twice. The union is counted on a raster, whose error is of the order of the cell times the perimeter.
- **The exact union by polygon clipping.** Exact, but with many polygons from tiles, some self-touching, it is fragile and needs a library the infrastructure does not hold. A raster of 5 m is well inside what the base map knows of a boundary.
- **One reading.** Each reading moves the share by many points in Melbourne's centre (78% to 95% within reach). Every declared reading is reported, and none is called the indicator's value.
- **The Degree of Urbanisation's city.** The analysis area is a square about the centre, with ways and land gathered half a kilometre beyond it. The city's delineation would need GHS-SMOD and its built-up area; it is left for a later change.
- **A plug-in of its own.** The tool shares the transit plug-in's engine, network and people; both indicators are about who can reach what on foot.

## 4. Consequences

- **What Melbourne's centre shows** (a square of 5 km about the Town Hall, 220,211 people):
  - within 400 m along the streets: 78.3% of people of parks and plazas, 89.0% with pedestrian areas and sports grounds, 94.8% with every green and open land;
  - open land covers 11.0%, 13.3% and 17.6% of the square under the same readings;
  - WHO Europe's parks within 300 m in a straight line: 62.7% for parks of 0.5 ha, 47.4% for parks of 1 ha.
- **Found while building:** joining a point to the network read every point of the network, which made a reading cost 22 s. The grid join is exact and makes it about one second.
- **Not built:** the Degree of Urbanisation's city; validation of spaces on the ground; the land allocated to streets; disaggregation by sex, age and disability; size classes of spaces; polygons cut by a tile's edge merged again (a large park so cut may fall below a least area, lowering WHO Europe's reading).
- **Provisional:** the kinds' table; the readings; the plug-in's new name.
