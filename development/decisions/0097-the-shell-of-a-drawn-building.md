# 0097. The Shell of a Drawn Building: Its Roof, Its Faces, and the Light at Its Glass

- **Status:** Accepted, provisionally (2026-11-12). The second stage of [ADR 0095](0095-the-design-studio.md), without its day of heat hour by hour and its façade as a grammar, which remain.
- **Date:** 2026-11-12
- **Decided by:**
  - **The request.** The user: "You may proceed all tasks remain, including outdoor appearance design, street and urban architectures design, GIS and optimization algorithms, earth science algorithms, etc can reused if needed"; and, of the manner, "make it delicate, elegant, and funtional".
  - **The design.** Claude, here.

## 1. Context

After ADR 0095 and [ADR 0096](0096-what-stands-in-the-rooms.md) a building could be drawn, furnished and walked through. It had no outside: its roof was a pitch in a table of quantities and a flat lid in the round; its walls were one stuff; it could be seen from above and from within, never from the street. Nothing was found of the light its windows take, though the neighbours that take it were already on the page.

Three things the survey behind ADR 0095 found bear on this record:

- the room planners draw a roof and compute nothing of it, and none draws an elevation that follows from the plan;
- daylight in practice is either a rule of thumb (a share of glass to floor, which `plan` already finds) or a simulation that needs a weather file and traced light; between them stands the average daylight factor, an estimate with a formula;
- a roof of equal pitch over any footprint is the footprint's straight skeleton, which the infrastructure did not have: physis's `inset` said so ("a straight skeleton … is not written").

## 2. Decision

### 2.1 Roofs as Straight Skeletons (physis)

physis gains the roof of equal slope over a polygon (its decision 0026): the straight skeleton read as a height, computed as the lower envelope of slabs over the motorcycle graph (Eppstein and Erickson 1999; Cheng and Vigneron 2007), not by simulating the wavefront's events, which coincide wherever a plan is rectilinear or symmetric. It answers the height at a point, the faces as convex pieces, and the ridges, hips and valleys. An edge may be marked a gable: it does not rise. A gable is accepted on an edge of the polygon's convex hull between two convex corners, and otherwise refused and reported.

### 2.2 The Shell (architecture)

A new module of the architecture domain, `shell` (55 checks worked by hand), reads a drawn building's outside from what `plan` read:

- **the footprint** of each storey: its outer walls chained into rings, with the indoors on the left, and moved out to the walls' outer faces. A ring that runs clockwise is a court;
- **the roof** on the top storey's footprint, from the wall plate: flat with a parapet, a shed falling toward a bearing, a hip, or a gable. The eaves' ring is the footprint moved out by the overhang, and the roof passes through the top of the walls' outer faces. Every sloping face has the pitch, so the roof's area is its plan's over the cosine, exactly. A gable is asked for by a point on the ground; the wall beneath it is carried up to the roof, and so are a shed's walls and a flat roof's parapet;
- **the faces**: each outer wall with the way it looks, gathered by the eight winds: wall, glass, share;
- **the glass**: every glazed opening as a pane, with the eave above it as an overhang.

### 2.3 Sun, Sky and Daylight at the Glass (architecture's `environment`)

`panes.hpp` (17 checks) reads each pane against the prisms about it, the neighbours and the building's own storeys:

- its **hours of sun** on a day, by the share of the pane lit at each instant, and the hours its middle is lit; a ray is cut by a prism (the thresholds of ADR 0089's `sun.hpp`) or by the eave above the pane, in closed form;
- its **direct energy** on a clear day (Meinel and Meinel's beam, which the module already used), a bound from below;
- its **angle of visible sky**, between what stands before it and the eave above it;
- a room's **average daylight factor** from its panes' glass and angles, the area of its surfaces, and two declared values (Crisp and Littlefair 1984; BRE Digest 309).

The Sun's instants are the place's, given by the work from the astronomy domain; the domain reads no place. A day's balance is set beside it: what the Sun gives through the glass against what the fabric loses in 24 hours at two declared temperatures.

### 2.4 The Document and What Stands Outdoors

The document's roof becomes `{form, pitch, eaves, cover, gables, fall, parapet}`; it gains a `shell` (a cladding and a tone), a boundary for the lot (a hedge, a fence, a wall, on the edges that face no street) and `light` (the declared values). All are declared like any value and undone like any change. A new building's roof is flat until a person says otherwise: nothing is generated.

The catalogue gains eight kinds that stand outdoors (trees, a shrub, a hedge, a planter, a bench, a garden table, a lamp post), judged by no room; a person passes beneath a tree.

### 2.5 The Outside Drawn

- **In the round and walked through** (`view.js`, `materials.js`): the outer walls in their cladding (render, brick, weatherboards, battens, stone, in nine tones), mapped in metres so that courses run on round the building; the roof's faces in their cover (tiles, slate, standing seam, shingles), with a soffit and a fascia; gables; the lot's boundary; trees built from seeded sequences. One walks out of the door and round the house.
- **Elevations** (`elevation.js`, pure, 22 checks): each side square on, as an architect draws it: the walls that face the eye with their windows and doors, the roof's faces with their courses, the verge and the gable, the levels, a person for scale, and behind it the outlines of what stands beside. Faces are drawn from the farthest to the nearest. Four on one sheet at one scale.
- **On the plan**: the roof over a storey as a roof plan, its edge and its ridges, hips and valleys, while the shell is looked at.

### 2.6 The Page

Two panels and a view join the Design Studio: **Shell** (the roof's form, pitch, eaves, cover and gables; cladding and tone; the lot's edge; the faces), **Light** (each room's glass, sky, daylight factor and hours of sun on a day chosen; a clear day's balance; what is presumed) and **Elevations**. The engine is asked with what stands and shines about the building: the neighbours within 150 m and the Sun every quarter of an hour of the day.

## 3. Alternatives Considered

- **Simulating the wavefront.** The textbook algorithm for a straight skeleton processes edge and split events in order. Houses are rectilinear and symmetric: their events coincide, and the cases multiply. The lower envelope is a minimum over slabs at each point, indifferent to coincidence, and its checks are closed forms.
- **A borrowed skeleton.** CGAL's straight skeletons are under the GPL; the libraries own their interfaces and take no copyleft (CLAUDE.md §7).
- **Roof planes drawn by hand.** Free, and nothing would follow: no area, no gable wall, no eave over a window. The four forms cover a house; a hand-drawn roof can come later as a fifth.
- **Daylight traced.** EN 17037 asks for illuminance over a share of the year, from a climate file and traced light. That is a simulation to hold to Radiance, planned under ADR 0095's second stage; the factor is what can be said now, and says what it is.
- **The roof as an obstruction.** A ray is tested against prisms to each storey's top, not against the roof above the plate: a ridge can shade a neighbour's window a little more than its walls do. Stated on the page.

## 4. Consequences

- **Written.**
  - physis: `roof.hpp` and its checks (its decision 0026).
  - architecture: `shell` (55 checks), `environment`'s `panes.hpp` (17) and `solar.hpp` (12; used by ADR 0098); the domain declares nothing new. Its theory note gains §9 to §11.
  - The API's `building.hpp` reads the shell; pieces that stand outdoors are judged by no room; the roof and the walls carried up to it enter the quantities.
  - `visualization/js/plan`: `exterior.js`, `elevation.js`, claddings and covers, the roof and the garden in the round.
  - The Design Studio: Shell, Light, Elevations.
- **Changed.** The example house has a gabled roof of clay tiles, weatherboards, a hedge and three trees. Its outer walls are counted with their gables.
- **Not built.**
  - Dormers, chimneys, gutters; a mansard, a gambrel, a butterfly; a roof over a court; an upper storey's own roof where it is set back (it is flat).
  - A cladding by wall, a base course, shutters; the façade as a grammar (syntaxis).
  - Limits on daylight and sun in a scheme (the findings are given; nothing is asked of them yet).
  - A day's heat hour by hour, held to a standard's cases; light traced.
  - Shadows cast in the elevations; sections.
- **Known limits.** Each is stated where it is computed: the daylight factor is an estimate; the energy leaves out the sky's diffuse light; a gable stands only on the hull; a neighbour with no height in the data stands 6 m.
