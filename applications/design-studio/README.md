# Design Studio

*Status: draft. Serves use and learning. The first stage of five, most of the second, part of the third, and the first step to the street ([ADR 0095](../../development/decisions/0095-the-design-studio.md), [ADR 0096](../../development/decisions/0096-what-stands-in-the-rooms.md), [ADR 0097](../../development/decisions/0097-the-shell-of-a-drawn-building.md), [ADR 0098](../../development/decisions/0098-the-street-a-building-stands-on.md)); its name and its colours are provisional.*

A building drawn by hand on any lot of any street, furnished, roofed and clad, seen in plan, in elevation and in the round, walked through and round at a person's height, and read with the street it stands on. The person designs. The studio tells what the drawing does, after every change: its rooms measured, their light and air, how each is reached, whether what stands in them can be used, the rules it meets one by one, the brief it answers, what it is made of and what that would cost; its roof's area and height, the sun, the sky and the daylight at its glass among its neighbours; and of its street the width between the faces, the sun on the footpaths, and how high each building beside it may rise.

## 1. Purpose

The room planners in use draw a house and compute nothing about it; the professional tools compute, but on massing at the scale of a site. This studio computes one building, inside and out, as a person draws it, and says what it presumes.

- **Nothing is generated.** The tools make what the hand asks for and nothing more. Assistants (alternatives for a storey, a search, an agent) are the last stage of five, and will use the same operations as the hand.
- **It advises and does not forbid.** A rule failed is a finding, with what was asked and what was found. Only what is not a building is refused: walls that cross are shown in rose, and an operation that cannot be carried out changes nothing and says why.
- **Any street.** A place is a latitude and a longitude; its buildings and ways are read from the world's map. Rules are a scheme a person chooses or states; a jurisdiction's scheme is a file.
- **Nothing is borrowed.** Every piece of furniture is built here to the size its piece has, and every texture is drawn from a seeded sequence. A model of one's own can take a piece's place.

## 2. Running It

```
python applications/design-studio/serve.py --open
```

It serves http://127.0.0.1:9050/. The engine is compiled the first time by the page's notice, or by

```
python data/acquisition/recipe.py applications/design-studio/recipe.json
```

with g++ 11 or later, natively or in WSL (about two minutes). Nothing else is needed. The surroundings of a place are fetched from Overture Maps' archives when a place is first given, and kept in `data/cache/world-city/`.

## 3. What One Does

### 3.1 On the Plan

Five ways of seeing: **Plan**, **Elevations**, **In the round**, **Walk through**, **Street**.

| With | One |
|---|---|
| **Room** (R) | drags a rectangle: four walls, joined to those they meet |
| **Wall** (W) | clicks a wall's start and its end; it goes on from there. Inner, outer or open (a boundary with nothing built on it) |
| **Door**, **Window** (D, N) | clicks a wall. On an outer wall a door is an entrance. A door can be drawn to swing to the other side and to hang at the other end |
| **Stair** (S) | clicks its first riser; it rises to the storey above |
| **Furnish** (U) | chooses a piece in the catalogue, then clicks where it stands. Against a wall it turns its back to it. Trees, a hedge, a bench and their kin stand outdoors |
| **Select** (V) | moves a corner, a wall, a door, a window or a piece by dragging it; chooses a room to name it and give it a use. `[` and `]` turn the piece in hand |
| **Lot** (O) | clicks the lot's corners in turn |
| **Erase** (E) | clicks what to remove |

Ctrl Z and Ctrl Y undo and redo every change. F fits the view. The wheel zooms; dragging empty ground moves the plan.

### 3.2 The Panels

- **Rooms:** each room's area inside its walls, its width, its glass and openable area as shares of its floor, and how many doors it is from the outside.
- **Interior:** what is found of each piece that cannot be used as it stands, in sentences; the width a person needs to pass; the catalogue of 44 kinds, eight of them for the garden; the share of each room's floor left free.
- **Brief:** the spaces asked for and the relations between uses, each statement met or not, with the reason.
- **Shell:** the roof's form (flat, shed, gable, hip), its pitch, its eaves and its cover, and which eaves stand as gables; what the outer walls are clad in, and its tone; how the lot is bounded; the roof's area and height; each face with the way it looks, its wall and its glass. While it is open the plan shows the roof over it.
- **Light:** each room's glass, the angle of sky it sees, its average daylight factor and, with a place, the hours its best window is in the Sun on a day chosen; a clear day's gain through the glass beside the fabric's loss at two declared temperatures; what is presumed.
- **Rules:** a scheme of rules chosen, its findings one by one with their clauses, limits of one's own, and what the scheme does not find.
- **Cost:** quantities, rates as ranges, the cost as a range, the heat the fabric loses per kelvin, where the glass faces.
- **Site:** the place, its surroundings, the lot and which of its edges face a street.

The piece, the door, the wall or the room in hand is shown above the panels with what can be changed of it: a piece's size, the clear floor it needs, its cloth.

### 3.3 Elevations

The four sides of the building on one sheet at one scale, drawn from the plan as an architect draws them: the walls that face the eye with their windows and doors, the roof's faces with their courses, the gables, the levels, a person of 1.7 m, and behind it the outlines of what stands beside. A tree before the building is dashed.

### 3.4 In the Round

The building with its walls cut at their openings, their cut tops inked as on the plan, among its neighbours, under the place's Sun on a day and at an hour chosen, in its cladding. The roof is off unless asked for, and then stands in its cover with its soffit and fascia; **cut low** stands the walls only to 1.2 m, as a model's, so that what is against the near walls is seen. A click takes a piece in hand; a drag moves it over the floor.

### 3.5 Walking Through

The eye at 1.6 m, inside. **W A S D** or the arrows walk (Shift to hurry), **Q** and **E** turn, dragging looks about, and a click on a floor goes there. One is stopped by the walls, less their doors, and by what stands in the way. A small plan in the corner shows the storey, where one stands and the field one sees; a click on it puts the eye there. A piece is taken in hand, moved and set down from within as on the plan. The rooms' lamps are lit inside, and by night. One goes out by a door and round the house, through the garden and along the street: the roof is on, and the minimap says *Outside*.

*Caveat.* No way is sought round a piece: a walk toward it ends at it. Stairs are not climbed; the storey's tab takes one up.

### 3.6 A Model of One's Own

A piece in hand can be given a binary glTF file (`.glb`, in metres, its front toward +Z). The server keeps the file by the hash of its content; the piece takes the file's size and is drawn by it in the round. The domain still sees a rectangle of that size.

### 3.7 The Street

With a place given, **Street** offers the ways about the site by their names. One taken in hand is read along 240 m of it (a map cuts a street at its crossings: the ways of its name that carry on are joined).

- **Its section** at a station: the buildings on either side at their heights (one too tall for the page is cut off and its height written), the strips at their levels with who uses them, the width between the faces, the sky the centreline sees, and the Sun at the hour chosen: the ray that grazes the roof, the ground and the face it lights.
- **The whole unrolled** beneath: the faces by their distance from the centreline, darker the higher, the building drawn in rose, each footpath's hours of sun. A click moves the station.
- **Its strips** set out by hand from the left: footpaths, frontage, a shared surface, cycle tracks, verges with trees, a median, bus lanes, a tramway, parking, lanes; whether they fit at the narrowest; each group's share. A map does not say how a street is divided: a division presumed for its width can be begun from.
- **What is asked of it:** limits of one's own (a footpath's width, a lane's, the share for people on foot, the enclosure, the hours of sun on a footpath), each found. None is any authority's.
- **How high each may rise:** of every building that stands on it, its height, the sun it takes from the footpaths as it stands, and the greatest height at which every footpath point that has its hours keeps them. A building may be given another height: the design's, not the map's. It then stands so in the round and shades the building drawn.

In the round and walked through, the strips lie along the way with trees on the verges.

**File** keeps a building on this computer's server, saves it as a file (`noetarium.building/1`), and opens one.

## 4. What It Stands On

| Layer | What the studio uses |
|---|---|
| The architecture domain | `plan` (rooms as the faces of the walls, net areas, widths, light, air, the access graph; habitation; quantities, cost, heat loss), `programme` (the brief), `interior` (what stands in the rooms), `rules` (siting), `shell` (the footprint, the roof, the faces, the glass), `environment` (the sun and the sky at a pane, the daylight factor, how high a building may rise) |
| The urban-science domain | `section`: a street between the faces that stand on it |
| physis, morphe, eikon | the faces of a plane graph, the inset of a ring and the clipping of one ring by another, the roof of equal slope over a polygon (its straight skeleton); shortest paths; the exact distance transform and components of a grid |
| The API | `science/building.hpp` and `science/street.hpp`, the compositions any work can call; the studio's engine is a program around them |
| The visualization layer | `js/plan`: the document, its operations, the catalogue and its models, the plan as a drawing, the elevations, the building in the round and from within, the street's section and its length unrolled; `js/gis/frame.js`, the exact local frame |
| The data library | `world_city.py`, any place's buildings and ways (Overture Maps, ODbL); the gazetteer of place names |
| The astronomy domain | the Sun's altitude and azimuth |

## 5. Schemes of Rules

A scheme is a file in [`schemes/`](schemes/): siting rules and a code of habitation as limits, each with its clause, and with the scheme's version, the date it was read, its source, where it applies and a list of what it does not find.

| Scheme | What it states |
|---|---|
| `unstated` | nothing: the default |
| `victoria-nrz`, `victoria-grz` | Victoria's standards for one dwelling on a lot (clause 54 at amendment VC322, 2 November 2026) in two zones, with the National Construction Code 2022's provisions for a house |

*Caveat.* The Victorian values were read by a survey on 2026-11-11 and not checked clause by clause against the published text. The scheme is amended often, and a schedule to a zone or an overlay can change a standard. Read the clauses before relying on a finding.

## 6. What Is Presumed, and What Is Left Out

- **Rates are presumed.** No open source of building rates was found; those that come with a new building have no source behind them, and the page says so until one is declared.
- **The clear floor a piece needs is its catalogue's,** a designer's default and not a standard's; so is the 0.6 m a person needs to pass. Both are a person's to change.
- **Whether a piece is reached is found on a grid of 5 cm,** within its room and from that room's doors: a gap within one cell of the passage may be found either way.
- **A neighbour with no height in the data stands 6 m,** and the street's table says *presumed* beside it.
- **The daylight factor is an estimate** (BRE's formula for a room lit from the side), on two declared values: the light the glass passes (0.68) and the light the room's surfaces reflect (0.5). The standard that read it as a limit is withdrawn; nothing is asked of it here.
- **The Sun's energy through the glass is the direct beam on a clear day:** the sky's diffuse light is left out, so it is a bound from below. The day's loss is the fabric's at two temperatures one states.
- **A building shades as its walls do:** the roof above the wall plate is not among what a ray is tested against.
- **The sky a street sees is its section's:** that of a canyon of that section running on without end.
- **A building's limit is its own:** two raised together may take what neither takes alone.
- **Approximations are noted on their findings:** a net area to the first order where a wall stands free in a room; coverage that leaves out the corners' squares and is exact only when the storeys nest; uncovered land counted as garden.
- **Not found:** the rules that need the neighbours (overlooking, overshadowing, daylight to their windows); light traced from the sky; the heat of a day hour by hour; clearances stated by a code; a street's traffic, noise and air.
- **Not drawn:** curved walls; stairs that turn, and the void they need above; dormers, chimneys, a roof over a court, an upper storey's own roof where it is set back (it is flat); a gable on an eave that is not on the footprint's hull (it is made a hip, and the panel says so); what a fixture is connected to (stage 4); light traced as a photograph's is; several buildings drawn on one street.

## 7. Checks

```
node applications/design-studio/tests/run-tests.mjs
```

75 checks: the schemes are well formed; the ink holds a contrast of 7 on every tone by day and by night; every mark of a piece's symbol has its style; through the server, the example house is read and its cost and heat loss recomputed from the quantities returned, a scheme is found rule by rule, a storey that is not a plan is refused with its faults, the example's 29 pieces are each found well placed and a piece moved badly is found, a model of one's own is kept by its hash, projects are kept and forgotten; the example's roof is read (its area, its ridge, its gables), its faces, each room's daylight, the hours of sun given the Sun and a neighbour; a street is read through the server, its findings one by one and what a building beside it may rise to.

`tests/drive.mjs` drives the page by mouse and keys in headless Chrome (56 assertions, not in the suite), from an empty building to a furnished house of two storeys, roofed and clad, drawn in elevation, walked through, on a real street taken in hand:

```
node applications/design-studio/tests/drive.mjs <a directory for its pictures>
```

with the server running. `tests/timing.mjs` times one picture of the building in the round and walking, for finding what slows the view.
