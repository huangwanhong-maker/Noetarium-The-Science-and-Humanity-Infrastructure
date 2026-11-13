// Historical objects drawn (ADR 0110): the public interface of visualization/js/history.
//
// - cone.js: the cone of an object: what its appearing rested on, by steps and by role, with what it inherits marked
//   so, and what rests on it; the glyphs of the kinds, the words and hues of the roles, a year as it is read;
// - loom.js: the loom of a record: time as the warp, ruled part by date and part by rank so that crowded years open;
//   strands side by side; dependences as threads between them.
//
// Every function returns markup and needs no page; history.css beside this file gives every mark a default look, which
// a work's own stylesheet may change. The map of a record is the layer's 2D GIS (../map), on which a work draws.

export { HUES, ROLES, ROLE_ORDER, GLYPHS, glyphOf, yearText, textWidth, esc, layoutCone, svgCone } from './cone.js';
export { warp, ticks, layoutLoom, svgLoom } from './loom.js';
