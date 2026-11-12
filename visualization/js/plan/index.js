// A building drawn by hand (ADR 0095, ADR 0096): the document a person, a generator or an agent changes, the operations that change
// it, what stands in its rooms, its plan as a drawing, and the building in the round and walked through. Of no work: the Design
// Studio is the first to use it; any work that lets a building be drawn can.
//
// - geometry.js   points and segments on the ground; what a pointer snaps to (pure)
// - model.js      the document noetarium.building/1; the operations, each a pure function with the document before as its inverse;
//                 a storey kept a plane graph whatever is drawn; a history; a furnished example (pure)
// - catalogue.js  furniture and fixtures: each a rectangle on the floor with the clear floor it needs, its symbol on a plan, and
//                 where it snaps to (pure)
// - exterior.js   the outside as the document keeps it: the roof's form, cladding, covers, tones, the lot's boundary (pure)
// - elevation.js  a building's elevations as drawings, from the document and what the domain read of its shell (pure)
// - street.js     a street: the way in hand, its strips, its section drawn square across, the whole unrolled (pure; ADR 0098)
// - draw.js       a storey's plan as SVG, with its pieces, the lot, its surroundings and what the domain read; what lies under a
//                 pointer (pure)
// - walk.js       what stops a person on a storey, the step from one place toward another, where one comes in (pure)
// - solid.js      the pieces a wall is cut into at its openings; the direction to the Sun (pure)
// - materials.js  textures drawn here from seeded sequences, materials by name, the solids pieces are built from (three.js given)
// - models.js     the catalogue's pieces in the round, each built to its piece's size (three.js given)
// - view.js       the building in the round and from within: walls with their two faces, windows, doors on their hinges, floors by
//                 use, ceilings and lamps, the pieces, the Sun and the sky; walking, and taking a piece in hand (three.js given)
//
// What follows from a drawing (its rooms, their areas, light, access, the rules it meets, its cost; whether each piece can be used
// and reached) is the architecture domain's, asked through the API (api/cpp/science/building.hpp). Checked in Node
// (visualization/tests/plan-checks.mjs).

export * from './geometry.js';
export * from './model.js';
export * from './catalogue.js';
export * from './exterior.js';
export * from './elevation.js';
export * from './street.js';
export * from './draw.js';
export * from './walk.js';
export { sunDirection, wallPieces } from './solid.js';
export { makeKit, sequence } from './materials.js';
export { MODELLED, buildPiece } from './models.js';
export { BODY, BuildingView, EYE, lightPlaces } from './view.js';
