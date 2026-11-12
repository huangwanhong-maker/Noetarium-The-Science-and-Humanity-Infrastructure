# 0096. What Stands in the Rooms, and the Building Walked Through

- **Status:** Accepted, provisionally (2026-11-12). Extends [ADR 0095](0095-the-design-studio.md): its third stage is begun (pieces and the floor they need; their connections remain for the fourth), and a way of seeing is added.
- **Date:** 2026-11-12
- **Decided by:**
  - **The request.** The user, with a picture of a furnished room seen at eye height and a small plan in its corner: "Whether we may even can dive into the design in first-person view. Like the image, and even can configure furnitures. Of course, I think they need 3D models, we may provide some example models."
  - **The manner.** The user: "make it high quality and delicate and elegant."
  - **The design.** Claude, here.

## 1. Context

ADR 0095 built a studio in which a person draws a building and is told what the drawing does. Its rooms were empty, and it could be seen only from above. Its third stage planned fixtures and furniture "as parametric objects with use-zones and service connections".

The survey behind that record bears on this one:

- furniture has been laid out under published clearances, with a person as a disc on the floor (Merrell et al. 2011): that is prior work, and the form is taken from it;
- no open library of fixtures with dimensions was found, and the large sets of furnished rooms may not be used;
- models in the public domain exist, but they are fixed meshes in a style of their own, and a piece made wider would not be a wider piece.

## 2. Decision

### 2.1 A Piece

A piece is a rectangle on the floor: its middle, the way its front faces, a width, a depth and a height; the clear floor it needs before it, at each side, or all round; and whether it stands in a person's way (a rug, a picture and a lamp hung from the ceiling do not). The document keeps pieces beside its walls (`furniture`), and three operations change them: set one down, change one, take one away. They are operations like any other, undone like any other.

A **catalogue** of 36 kinds (`visualization/js/plan/catalogue.js`) gives each its size, the floor it needs, its symbol on a plan and whether it stands against walls. A piece once set down carries its own size and floor, which a person may change. The catalogue's values are a designer's defaults and say so: none is a standard's.

### 2.2 What Is Found of a Piece

A new module of the architecture domain, `interior` (30 checks worked by hand), reads the pieces in the rooms `plan` read:

- **within:** its footprint lies in its room, inside the walls' faces;
- **apart:** it overlaps no other piece that obstructs (two convex footprints overlap when one clipped by the other's edges keeps an area; physis's `clip_left`);
- **usable:** the floor it needs lies in the room and no piece stands in it. Two pieces' floors may overlap: one stands before a sofa and before a table in turn;
- **reached:** a person can come from a door of the room to the floor it needs. A person is a disc of a declared width (0.6 m unless stated). The room's free floor is sampled on a grid of 5 cm; where the disc's middle can stand is that set eroded by the disc's radius, and a piece is reached when its floor, grown by the radius, meets the component a door opens onto. The erosion and the components are eikon's (its exact distance transform), so the architecture domain now declares eikon.

Of each door it finds whether a piece stands where its leaf swings, and whether a person can stand behind it. A door swings into the room on its wall's left, or into the building, unless drawn otherwise; the plan's opening gains `swing`, and the page the end it hangs at.

*Caveat.* Reaching is exact to about one cell: a gap within 5 cm of the passage may be found either way.

### 2.3 Example Models, Drawn Here

Each kind of the catalogue has a model in the round (`models.js`), built from a few eased solids **to the size its piece has**, so that a sofa made wider is a wider sofa. Their textures (boards, tiles, a woven ground, a painted picture) are drawn on a canvas from seeded sequences (`materials.js`). Nothing is a photograph or a borrowed asset; the pictures can be made again.

A piece may instead carry **a model of one's own**: a binary glTF file, kept by the studio's server by the hash of its content, read once to learn its size, and fitted to the piece. The piece takes the file's size; the domain still sees a rectangle.

### 2.4 The Building from Within

`visualization/js/plan/view.js` replaces the first view. Walls stand with two faces, each in the stuff of what it faces; windows sit in frames with their panes; doors hang on their hinges, open; skirtings run between the doors; floors are laid by the use of their rooms; ceilings carry their lights and each room a lamp. The Sun is the place's Sun, with a sky above; the rooms' lamps are lit inside and by night. Corners are shaded when the eye is at rest and not while it moves.

Two ways of seeing:

- **In the round,** the roof off; the walls may be cut at 1.2 m, as a model's, so that what stands against the near walls is seen.
- **Walked through,** the eye at 1.6 m. W A S D or the arrows walk, dragging looks about, a click on a floor goes there. One is stopped by the walls, less their doors, and by the pieces (`walk.js`, pure and checked): walking slantwise into a wall one goes along it, and straight into it one stops. A small plan in the corner shows the storey, the eye and the field it sees; a click on it puts the eye there.

A piece is taken in hand from within as on the plan: a click selects it, a drag moves it over the floor, against a wall it turns its back to it. A piece chosen in the catalogue is set down by a click on a floor.

### 2.5 On the Plan

Each piece is drawn by its symbol, as an architect's plan shows it. The piece in hand shows the floor it needs; a piece that cannot be used is drawn in rose with that floor. A panel, *Interior*, holds what is found of each piece in sentences, the width a person needs, the catalogue, and the share of each room's floor left free. The example house comes furnished: 29 pieces, each found well placed.

## 3. Alternatives Considered

- **Borrowed models.** A kit in the public domain would have given more kinds at once. Its pieces are fixed meshes: they do not follow a piece's size, their style is not the studio's, and a collection's terms can forbid gathering it by script. A person's own file is the way in for such models.
- **A photograph's realism.** The picture the user sent is rendered by tracing light. What is drawn here is lit directly, with shaded corners: quick enough to walk in on an ordinary machine, and consistent with the design by construction. Tracing light is left for later.
- **The pointer locked to the page.** It walks better and takes the pointer away, with a prompt to give it back. Dragging to look keeps the page a page.
- **Reaching by exact geometry.** The free floor less the pieces grown by a radius is a polygon with arcs; its components are exact and laborious. The grid on eikon reuses a foundation and states its error.
- **Clearances as a code's rules.** They belong in a scheme, as ADR 0095's limits do, each with its clause. The catalogue's floors are defaults until such a scheme is written.

## 4. Consequences

- **Written.**
  - The architecture domain's `interior`: 30 checks (239 in the domain; it passes alone).
  - `visualization/js/plan`: the catalogue, the models, the materials, the walk and the view; 84 checks in all.
  - The API's `building.hpp` reads the pieces and answers what is found of each.
  - The Design Studio: a tool *Furnish*, the panel *Interior*, the view *Walk through*, a model of one's own. 55 checks; the page driven by mouse and keys in headless Chrome (37 assertions, not in the suite).
  - Thirteen pictures in the gallery's §29.
- **Not built.**
  - The connections of fixtures to water, drains, power and air (ADR 0095's stage 4).
  - Clearances stated by a code, as a scheme.
  - A piece under another (a stool under a counter stands in its floor and is found to).
  - Stairs climbed, and the void they need above.
  - A way sought round what stands in the way: a walk ends at it.
  - The finishes of walls and floors chosen by hand; cupboards hung on walls.
  - Light traced.
- **Known limits.** A piece is a rectangle. Its floor is its catalogue's. Reaching is judged within a room, from its own doors, not from the building's entrance. Each is stated where it is computed.
