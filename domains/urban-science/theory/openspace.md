# Open Space: Its Share of a Place and Who Lives within Reach of It

*Status: draft (ADR 0154). The module `urban::openspace`; its checks are `openspace/tests/check_openspace.cpp`.*

## 1. The Concept

SDG 11.7.1 is the average share of the built-up area of cities that is open space for public use for all. UN-Habitat's metadata (last updated 2025-04-23) defines its terms:

- **Public space** is all places of public use, accessible by all, and comprises open public space and streets, the two measured apart.
- **Open public space** is any open piece of land with no buildings that is accessible to the public without charge. By typology it can include green public areas, riparian reserves, parks and urban forests, playgrounds, squares and plazas, waterfronts, sports fields, community gardens, parklets and pocket parks. Recreation that charges a fee, such as a golf course, is removed.
- **Potential open public space** is what a base map or imagery shows of open land before it is validated as free and open to all. What a computation from Overture's base theme counts is potential open public space in this sense.
- **The city** is delineated by the Degree of Urbanisation, and the built-up area of the city is the denominator.
- **Access.** The metadata also estimates the share of people within 400 m walking distance of open public space, along the street network (a service area of 400 m about each space), assuming every street walkable and people spread over the buildings.

**Open terms.** Each has more than one reading, and the module computes the shares under every declared one:

- which land counts: parks and plazas only; pedestrian areas and sports grounds as well, which are often open to the public; every green and open land, gardens and cemeteries among them;
- how large a space must be: the metadata names none. WHO Europe's brief for action asks for public green space of at least 0.5–1 ha within 300 m in a straight line, and the test of that indicator by Annerstedt van den Bosch et al. (2016) took 1 ha and 300 m to the space's boundary as its defaults;
- the distance, and whether it is walked along the streets or measured in a straight line;
- the area the share is of.

None of these shares is the indicator's value until a reading is chosen. This is the treatment of open terms elsewhere in the infrastructure (tekmerion's `construal`; transit's readings of SDG 11.2.1).

## 2. The Share of Land

A space is a polygon with holes in metres of a local frame, with a kind given by the caller's reading of the land. Land cover lies beneath land use, and the base theme's polygons overlap. A park drawn over grass would be counted twice by summing areas.

**Definition.** For a set $S$ of counted spaces and a cell side $c$, the raster union area is $A_c(S) = c^2 \cdot \#\{\text{cells whose centre lies in some } s \in S\}$. A centre lies in a space when it lies in the outer ring and in none of its holes (even–odd). Given an analysis box $B$, only centres in $B$ count, and the share is $A_c(S)/|B|$.

*Remark.* Let $U = \bigcup S \cap B$. A cell counted wrongly, in or out, meets the boundary of $U$, which lies within the spaces' and the box's boundaries. The cells meeting a boundary of total length $P$ number of the order of $P/c$, so $|A_c(S) - |U||$ is of the order of $cP$, and vanishes for boundaries along the cells' edges: on the checks, axis-aligned squares on a 1 m raster are counted exactly.

**Monotone in the readings.** If a reading's kinds contain another's and its least area is no larger, its counted spaces contain the other's, so its union and its share are no smaller. The suite's check of Melbourne relies on this.

## 3. Reach

A place lies within reach under a reading when it lies inside a counted space, or when its distance to the nearest point of a counted space's boundary is within the reading's distance.

- **Along the streets.** Each space's outer ring is sampled every 25 m (`edge_points`); the walking network is urban science's access network densified to 20 m (transit's `walking`). One search from every boundary point at once gives every place's distance (access's `nearest_supply`, on morphe's Dijkstra). A place and a boundary point join the network at its nearest joinable point.
- **In a straight line.** The least Euclidean distance to a boundary point.

**Error of the sampling.** A boundary point lies within 12.5 m of every point of the ring between it and the next, so a place's true distance to the boundary in a straight line is at most 12.5 m less than the one computed. Along the streets the join adds its own walk at each end.

**Proposition 1.** For one place and one reading, the straight-line distance is at most the network distance (as transit's Proposition 2, with boundary points in place of stops). So under the same kinds and distance the share in a straight line is at least the share along the streets.

**The join through a grid.** Joining every place and every boundary point to the network reads the network's points for each of them; in Melbourne's centre, with some 20,000 boundary points, that cost 22 s a reading. `access::Joiner` buckets the joinable points in a grid of 100 m and searches rings of buckets outward, stopping when no bucket beyond can hold a nearer point (a point $r+1$ rings out lies at least $r$ cells away). It keeps the lowest index among points equally near, as `join` does, and so returns the same point and gap; a check holds it to `join` on 2,000 places at five bucket sides. The reading now takes about a second.

## 4. What Is Not Here

- the Degree of Urbanisation's delineation of the city and its built-up area; the analysis area is a square about the centre, with ways and land gathered half a kilometre beyond it so that a space across its edge serves the people inside;
- validation of which spaces are free and open to all, which the metadata asks for on the ground;
- the land allocated to streets, the other half of public space;
- disaggregation by sex, age and disability;
- the quality, safety and size classes of spaces (metropolitan, district, neighbourhood, pocket, linear);
- polygons cut by a tile's edge, which come in their parts: a large park so cut may fall below a least area, which lowers WHO Europe's reading.

## References

- UN-Habitat. SDG indicator metadata, 11.7.1, last updated 2025-04-23. https://unstats.un.org/sdgs/metadata/
- WHO Regional Office for Europe (2016). *Urban green spaces and health: a review of evidence.* Copenhagen.
- WHO Regional Office for Europe (2017). *Urban green spaces: a brief for action.* Copenhagen.
- Annerstedt van den Bosch, M., Mudu, P., Uscila, V., et al. (2016). Development of an urban green space indicator and the public health rationale. *Scandinavian Journal of Public Health* 44(2), 159–167.
- Overture Maps Foundation. Base theme: land use and land cover. https://docs.overturemaps.org/
