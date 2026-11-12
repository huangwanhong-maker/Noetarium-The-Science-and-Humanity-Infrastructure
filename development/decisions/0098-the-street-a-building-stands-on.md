# 0098. The Street a Building Stands On

- **Status:** Accepted, provisionally (2026-11-12). The first step from one building to the street, which [ADR 0095](0095-the-design-studio.md) left for after its stages.
- **Date:** 2026-11-12
- **Decided by:**
  - **The topic.** The user, at its opening: "from a single architecture … then move to street architectures/spatial design"; "for street one, I think we can utilize GIS, and may be need solve some optimization problems"; "for street building, geography, and economics etc reality is one we need pursue."
  - **Its conditions.** The user: "it should be a general software, and can choose any street"; "a design studio, can even allow users to design … [an assistant] just assistant."
  - **The word to go on.** The user: "You may proceed all tasks remain, including outdoor appearance design, street and urban architectures design, GIS and optimization algorithms, earth science algorithms, etc can reused if needed."
  - **The design.** Claude, here.

## 1. Context

A building in the Design Studio stands on a lot of a real place, among the buildings and ways read from the world's map (ADR 0095), and its windows' sun is found against those neighbours ([ADR 0097](0097-the-shell-of-a-drawn-building.md)). The street it stands on was a grey band.

What the infrastructure already holds for a street:

- the Sun and shadow on the ground as thresholds on heights (architecture's `sun.hpp`, ADR 0089), with which a design that changes only heights is evaluated by comparisons;
- the generative loop for a whole block, in the Urban Science Workbench (ADR 0089): massings searched under rules by NSGA-III. It designs a block's envelope, not a street's ground;
- the configuration of streets (ADR 0084) and who walks them (ADR 0075), at the scale of a city.

What exists elsewhere: section editors for streets (Streetmix is the best known), in which strips are laid out across a width. That a section be laid out by hand is prior practice and is not claimed. No survey was made for this record of what such tools compute of a place; the survey behind ADR 0095 was of buildings.

## 2. Decision

### 2.1 A Street Is What Is Measured Between Its Faces

A new module of the urban-science domain, `section` (29 checks worked by hand; theory note `domains/urban-science/theory/a-streets-section.md`). A street is a centreline with the buildings about it as prisms. At stations along it, the first face met on either side gives the width between faces, the enclosure (mean height over width) and the sky the centreline sees (the view factor of an endless canyon of that section: Oke 1981; Johnson and Watson 1984). Each side has a line, the share of its length that is built, and the share aligned to the line. Nothing is said of a reserve or of who owns the ground: only of what stands there.

### 2.2 The Strips

The street is divided by hand into strips from the left: footpaths, frontage, a shared surface, cycle tracks, verges with trees, a median, bus lanes, a tramway, parking, traffic lanes. The division fits when it is no wider than the street at its narrowest. Each strip belongs to a group, and the groups' shares are given. A scheme states limits (a strip's width by its kind, the enclosure, the share for people on foot, the hours of sun on a footpath) and each is found, one by one, as a building's rules are. None is any authority's until a scheme states it.

A map does not say how a street is divided. A division presumed from the width can be begun from, and is said to be presumed.

### 2.3 Sun on the Footpaths, and How High Each Building May Rise

On each side at each station a point of the footpath has its hours of sun on the day chosen, against every building. And each building that stands on the street is given a limit: the greatest height at which every footpath point that now has the hours asked keeps them, the others as they stand. It is exact, by sorting the thresholds (architecture's `solar.hpp`, 12 checks): Knowles's solar envelope (1981) for one building and the footpaths.

*Caveat.* It is one building's limit. Two raised together may take what neither takes alone; a design of several heights is checked whole, and a search over them is the assistant's work, not built here.

### 2.4 In the Studio

A fifth way of seeing, **Street**:

- **a way taken in hand** from the ways about the site, by its name: 240 m of it about the lot;
- **its section** drawn square across at a station: the buildings at their heights, the strips at their levels with what uses them, the width between the faces, the sky's angle, and the Sun at the hour chosen: the ray that grazes the roof, the ground it lights, the face it lights;
- **the whole unrolled** beneath: the faces on either side by their distance, darker the higher, the strips, each footpath's hours of sun, the station in hand;
- **a panel**: the street's measures, its strips edited (a kind, a width, moved, taken away), what is asked and found, the sun, and of each building its height and its limit. A building may be given another height: the design's, not the map's. That height then stands in the round, shades the footpaths, and shades the windows of the building drawn.

The building drawn is among the street's buildings, storey by storey. In the round and walked through, the strips lie along the way as ribbons at their levels, with trees on the verges.

The street is a part of the building's document (`street`: the line, the strips, the hours to keep, the heights given, what is asked), changed by the same operation as any declared value and undone like it.

### 2.5 In the Other Layers

- The API's `science/street.hpp`: the composition any work can call; the studio's engine answers it beside a building.
- `visualization/js/plan/street.js`: the way in hand, the strips, the section and the unrolled street as drawings (pure).

## 3. Alternatives Considered

- **A work of its own for streets.** The user's order is one building, then its street: the studio's document already holds the place, and a building's sun and a street's sun are one computation. A work for a district remains the Urban Science Workbench.
- **A block's generative loop, here.** ADR 0089's search designs envelopes for lots that are not drawn. The studio's person draws; an assistant that proposes heights along the street (heuresis, on the same thresholds) is the stage after.
- **The street's width from its class.** The map gives a way a class and sometimes a width. What is wanted of a section is the width between the faces, which the buildings give.
- **The sky by a hemisphere of rays.** Truer at one point, and no longer a property of the section. The canyon's formula compares sections and is exact for what it says.

## 4. Consequences

- **Written.**
  - urban-science `section` (29 checks) and its theory note; architecture `environment`'s `solar.hpp` (12).
  - The API's `street.hpp`; the engine and the server answer `/api/street`.
  - `visualization/js/plan/street.js`; the strips in the round.
  - The Design Studio's Street.
- **Not built.**
  - Several buildings drawn on one street; lots along it set out by hand.
  - A search over the heights along a street under what is asked (heuresis), and over the strips.
  - The street's rules that a jurisdiction states (a scheme as a file, as a building's are).
  - What a street does: who walks it (urban science's `movement`), what its frontages hold (`frontage`, markets), its place in the whole (`syntax`). The measures are there; the studio does not ask them yet.
  - Trees and awnings as shade; the ground's relief; junctions.
- **Known limits.** The sky is the section's. A footpath point stands where the strips put it, or a metre from the face. Heights the map does not give are 6 m and say so.
