// The architecture of kybernesis drawn (ADR 0130): the public interface of visualization/js/steering.
//
// kybernesis declares the loop from evidence to action as data, in a line-based architecture script; these functions read the
// script and draw its computational graph (the stages of one turn as columns, the blocks in sub-columns by depth, the wires
// between them, those carried to the next turn returning beneath), with the foundation's report of checks and methods when it
// is given. An engagement's trajectory is drawn as a lattice of turns and blocks with its branches, and a transfer as two systems
// facing each other with a plank for each support factor (the design of 2026-11-19). Markup that needs no page.

export { parseArchitecture } from './parse.js';
export { FONT, PALETTE, REGISTERS, FAMILIES, VERDICTS, textWidth, layoutGraph } from './layout.js';
export { svgGraph } from './draw.js';
export { ENTRY_STATES, entryOf, layoutTrajectory, svgTrajectory } from './trajectory.js';
export { STANDINGS, READING_STATUS, readingLines, factorRole, layoutTransfer, svgTransfer } from './transfer.js';
