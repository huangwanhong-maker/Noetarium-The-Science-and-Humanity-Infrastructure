# 0095. The Design Studio: A Building Drawn by Hand on Any Lot

- **Status:** Accepted, provisionally (2026-11-12). The first of five stages (§2.9); the work's name and colour are provisional until the user chooses them.
- **Date:** 2026-11-12
- **Decided by:**
  - **The question.** The user: "Architecture and Street Spatial Design. We will from a single architecture (both outdoor appearance and indoor appearance and equipment design), then move to street architectures/spatial design. I think this topics is really challenging. [...] I think currently already have some easy-to-use design software, what still the problem, and our innovation".
  - **The order.** The user: "I think consider single architecture first is better. For street one, I think we can utilize GIS, and may be need solve some optimization problems." And: "for street building, geography, and economics etc reality is one we need pursue."
  - **The generality.** The user, of a first site in one city: "it is wierd. I think it should be a general software, and can choose any street."
  - **The manner.** The user: "I don't think we should always autogenerate design. But in fact a design studio, can even allow users to design. Of course, can like market AI agent, automatically use tools and plug-in provided to design. but it just assistant".
  - **The word.** The user: "proceed. and begin implementation, try your best."
  - **The design.** Claude, in the conversation, in the idea [*A design studio for buildings*](../ideas/2026-11-12-a-design-studio-for-buildings.md) and here, after a survey of the software, the literature and the open data ([*Design of a single building*](../../research/literature/reports/Design%20of%20a%20single%20building.md)).

## 1. Context

### 1.1 What the Survey Found

- **The tools in use divide in two.** Consumer room planners draw a house and compute nothing about it: no daylight, no heat, no rule. Professional tools compute, but on massing at the scale of a site, or for one building type.
- **No tool surveyed covers plan, exterior, interior, equipment and performance together.** None publishes which provisions of a code it checks or reports them rule by rule; none states the uncertainty of a result; every building-design product is closed.
- **Assistants are arriving in them.** An agent that acts on a design is not by itself new.
- **The literature is older than the tools suggest.** All small rectangular plans meeting given adjacencies were enumerated and then dimensioned in 1976; a façade grammar has been learned and parsed; furniture has been laid out under published clearances with suggestions offered beside a person's own arrangement; rules have been checked by machine since 1966. These must not be claimed.
- **Real rules are formulas, and many are relations to the neighbours.** Victoria's standards for one dwelling on a lot are numerical, were rewritten in 2025 and amended three times by November 2026, and depend on the lot's size.
- **Costs are the weak side of reality.** No open source of building rates was found, and the two databases of embodied carbon forbid this use.

### 1.2 What the Infrastructure Held

The architecture domain held a lot's rules and the envelope they allow, a massing within it, sun and shade, a building as a box of rooms on a grid, and a district's buildings as one complex (ADR 0062, 0063, 0089). It held no building as a person draws one: walls where they are put, rooms of any shape, doors and windows, a brief. The foundations held what such a building needs: the faces of a plane graph and the inset of a ring (physis), shortest paths (morphe). The data library reads any place's buildings and ways from the world's tiles (ADR 0078).

### 1.3 Three Turns of the User

The design first proposed was a chain that generated a building from a brief, on a first site in Melbourne. The user turned it three times, and each turn is kept:

- one building first, the street later, where reality (geography, economics) is what to pursue;
- general software: any street, a place only a case of data;
- a studio in which a person designs; generation and an agent are assistants.

## 2. Decision

### 2.1 The Work

`applications/design-studio`, the *Design Studio*, on port 9050: a building drawn by hand on any lot of any street, in plan and in the round, with what the drawing does told as it is drawn. Its name is the user's phrase and is provisional; its colours (warm paper, plum ink, a peony rose on what is chosen, a lilac for what passes) are provisional.

### 2.2 The Person Designs, and the Studio Tells

- **The hand.** Eight tools: select and move, a room, a wall, a door, a window, a stair, the lot, erase. They make what is asked and nothing more.
- **What the drawing does,** after every change: each room's area inside its walls, its width, its glass and openable area as shares of its floor, and how many doors it is from the outside; the rules of the scheme chosen, one by one; the brief, statement by statement; quantities, their cost as a range, and the heat the fabric loses.
- **It advises and does not forbid.** A rule failed is a finding with what was asked and what was found. The only refusals are of what is not a building: walls that cross are a fault the page shows, and an operation that cannot be carried out changes nothing and says why.
- **Assistants come last** (§2.9), and will use the same operations as the hand.

### 2.3 One Set of Operations

The document `noetarium.building/1` holds only what is drawn: a site, storeys (vertices, walls, openings on walls, labels as points), stairs, and what is declared (a brief, a scheme of siting rules, a code of habitation, rates, a fabric). Rooms are not in it; they follow from the walls.

- **Every change is an operation** (`visualization/js/plan/model.js`, `OPERATIONS`): a pure function of a document and its arguments, with a title and declared parameters, answering a new document or throwing with the reason. The document before is the inverse, so a history is a list of documents.
- **A storey stays a plane graph whatever is drawn.** A wall drawn across others is cut where it meets them; a wall ending on another splits it; after a move the storey is drawn again from its walls as segments; walls that run straight on through a vertex nothing else uses are one wall again. Openings keep their place on the ground, and labels are points, so both survive such changes.
- **Of no work.** The document, its operations, the plan as a drawing and the building in the round are in the visualization layer (`visualization/js/plan`), as the board is, for any work that lets a building be drawn.

### 2.4 The Domain: A Building as It Is Drawn

Three additions to the architecture domain, C++ and header-only; their theory note is [`a-drawn-building.md`](../../domains/architecture/theory/a-drawn-building.md).

- **`plan`** (`plan.hpp`): the rooms are the bounded faces of the storey's plane graph (physis's `plane_faces`); what stands free in a room is a hole of it; each wall has the rooms on its two sides, and is an outer wall when one side is indoors and the other is not. A room's net area is physis's inset of its ring by half each wall's thickness, exact while the mitred offset is and otherwise given to the first order and said to be. Its width is the largest circle its net ring holds under that inset. The access graph joins spaces through doors, open walls and stairs; depth, the spaces a room is reached only through, and relative asymmetry (Hillier and Hanson 1984) follow on morphe's shortest paths.
- **`plan`** (`habitability.hpp`, `quantities.hpp`): a code of habitation as limits a scheme states (ceilings, light, air, doors, passages, stairs, a kind's area and width), found of each room, door and stair; quantities taken from the drawing; cost as the interval between each quantity at its low rate and at its high; the fabric's transmission coefficient $H = \sum U A$.
- **`programme`**: a brief as a labelled graph of spaces and relations between uses, and whether a building answers it, statement by statement.
- **`rules`** (`siting.hpp`, `finding.hpp`): a dwelling's siting as a scheme states it: height, storeys, the street setback, a side setback that grows with a wall's height piecewise linearly, walls on a boundary with the length a boundary may carry, coverage, open space, and garden by the lot's size.

A limit not stated is not evaluated, and nothing in the domain is any jurisdiction's. A finding is one rule met or not by one subject, with what was asked, what was found, the clause and a note where it rests on an approximation.

### 2.5 The Composition

`api/cpp/science/building.hpp` reads a document and answers everything that follows, in JSON, for any work; the studio's engine is a program around it (`applications/design-studio/engine/main.cpp`), built into `data/cache/engines/design-studio/` and kept alive by the server. It reads no file: a request holds the whole building.

### 2.6 Any Street

- **A place** is a latitude and a longitude, typed or found by name in the data library's gazetteer. The drawing is in metres east and north of it, in the exact local frame of the visualization layer's GIS.
- **The surroundings** are the buildings and ways within 320 m, read from Overture Maps' world archives by the data library (`world_city.py`) through the studio's server, and kept once asked. Buildings that stand on the lot are left out, as the building drawn replaces them.
- **The lot** is marked by hand, corner by corner, or set down as a rectangle. An edge beside a way is taken to face a street, and the person can say otherwise. No cadastre is asked for: none is open for the whole world.
- **The Sun** is the place's Sun, on a day and at a local solar hour chosen, from the astronomy domain.
- **Without a place** the building stands on a plane with north up, and everything but the surroundings and the Sun works.

### 2.7 Rules as Declared Schemes

A scheme is a file (`applications/design-studio/schemes/*.json`): a set of siting rules and a code of habitation, with its version, the date it was read, its source, where it applies, and a list of what it does not find. Three come with the studio: one that states nothing (the default), and Victoria's standards for one dwelling on a lot in two zones, at amendment VC322 with the National Construction Code 2022. They are the first case, not the software's boundary; a person can state limits of their own in the page. The Victorian values were read by the survey and not checked clause by clause, and the scheme says so.

### 2.8 What Is Presumed Is Said to Be

- **Rates.** A new building carries rates that no source stands behind, marked presumed; the page says so until a rate is declared.
- **Neighbours' heights.** A neighbour with no height in the data stands 6 m, marked presumed in the data the page holds.
- **Approximations.** A net area to the first order, a coverage exact only when the storeys nest, uncovered land counted as garden: each is a note on its finding.

### 2.9 Five Stages

| Stage | What is built | State |
|---|---|---|
| 1. The studio and the plan | a building drawn by hand on any lot; rooms, light, air, access, rules, brief, quantities, cost and heat loss as it is drawn | this record |
| 2. The shell | walls and roof with real constructions; daylight from the sky; heat through a day with the real windows (after ISO 52016-1's hourly method, held to the BESTEST cases, EnergyPlus and Radiance); the façade as a grammar; materials; the rules that need the neighbours | planned |
| 3. The interior | parametric fixtures and furniture with their use-zones and connections, under published clearances | begun: pieces, the clear floor they need, example models, and the building walked through ([ADR 0096](0096-what-stands-in-the-rooms.md)); connections and a code's clearances remain |
| 4. Equipment | water, drainage, power, heating and air as networks routed and sized as the plan changes | planned |
| 5. The assistants | alternatives for a storey by enumeration, held to the known counts of rectangulations; search over the whole (heuresis); an agent given the studio's operations, as Tabula's (ADR 0091) | planned |

The street follows the building: buildings composed at their frontages on the Urban Science Workbench's GIS, judged by what the domains already hold of walkers, sun, access, prices and markets.

## 3. Alternatives Considered

- **Generation as the pipeline.** A brief in, a building out, as first proposed. Rejected by the user: a person designs, and tools are tools. It also would have rested the work's claim on enumeration, which is from 1976.
- **A first site in one city.** Rejected by the user. The first case is data: a scheme, and whatever a place's records add.
- **A plug-in of the Urban Science Workbench.** The workbench is a map; a building's interior is drawn at another scale, with tools of its own. The workbench's *Architecture* remains for blocks, and the street stage will meet it.
- **Rooms stored as polygons.** Room planners store rooms and let walls follow. Then a wall shared by two rooms is two things that must be kept equal, and a wall that divides nothing cannot be drawn. Walls are stored and rooms read as faces, so adjacency, outer walls and holes are theorems of the drawing, not bookkeeping.
- **Evaluation in the page.** It would have put the domain's substance in JavaScript, against ADR 0003. The page keeps a storey a plane graph, which is editing; what follows from the drawing is the domain's, asked of the engine after each change (some tens of milliseconds).
- **A rectangular dual from the brief's graph.** The theory is contested for graphs with cut vertices, and a brief's graph must first be completed to a triangulation. The assistant of stage 5 will enumerate rectangulations and keep those that answer the brief.

## 4. Consequences

- **Written.**
  - The architecture domain's `plan` and `programme`, and `siting` in `rules`: 129 checks worked by hand (209 in the domain; it passes alone).
  - `visualization/js/plan`: 52 checks.
  - The API's `building.hpp`.
  - The Design Studio: 43 checks; the page driven by mouse and keys in headless Chrome (`tests/drive.mjs`, 23 assertions, not in the suite), from an empty building to a house of two storeys under a scheme, on a real street.
  - Seven pictures in the gallery's §29.
- **Shown.** A house drawn on a lot traced in Kyoto stands among its 1,848 neighbours under that place's Sun, with no preparation; under Victoria's scheme the example house meets every siting rule and fails light and air in two rooms, each named.
- **Not built.** Stages 2 to 5. Of this stage: stairs that turn and the void they need above; curved walls; a roof's form; the rules that need the neighbours (overlooking, overshadowing, daylight to their windows), which wait for stage 2; a cadastre's parcels where one is open; several people on one building.
- **Asked of the foundations.** An outward offset of a ring in physis, for a floor measured to the outer faces of its walls and a coverage exact at the corners: until then coverage leaves out the corners' squares, and says so. Rectangulations in morphe, for stage 5.
- **Known limits.** The width of a room with reflex corners is its widest arm's, a bound from below on the largest circle. A wall's distance from a boundary is taken at its ends. The Victorian scheme is a survey's reading. Each is stated where it is computed.
