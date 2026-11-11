// The board of the visualization layer: a ground on which thought is set out and tied to evidence —
// notes of several kinds, words, images, strokes of a pen, regions, shapes, links that say how two
// items stand to each other, plans and chronicles, figures computed from the quantities the notes
// carry, tables, and cards that draw them (a chart, a map) or run a module on them. It is of no
// subject: a work embeds it, and a subject comes to it as a plug-in of templates, kinds of note,
// cards and modules (the design is development/ideas/2026-11-11-markets-and-a-board-for-ideas.md).
//
// What is thought is kept apart from how it lies (model.js), every change is an operation with its
// inverse (operations.js), and an arrangement is a function from the one to the other (arrange.js).
//
// - model.js       the schema noetarium.board/1, its kinds, relations and kinds of note; its soundness (pure)
// - operations.js  operations, their inverses and a board's history; removal, copies, membership (pure)
// - geometry.js    rectangles, the paths of links, snapping, the view (pure)
// - arrange.js     arrangements: grids, regions, trees, the order of time, alignment (pure)
// - schedule.js    plans and chronicles written as lines: dates, resolution, float, their layouts (pure)
// - quantities.js  definitions on notes and figures, as one system in interval arithmetic (pure)
// - templates.js   templates as data, laid on a board; those every board has (pure)
// - tables.js      tables as the data library has them: read from delimited text, gathered by name, given to modules (pure)
// - cards.js       the cards every board has: a chart, a map, a module; what a card is given (DOM)
// - kinds.js       how each kind of item is drawn (DOM)
// - view.js        the board on the page and the hand's work on it (DOM)
// - tools.js       the rail, the panel, a template in small (DOM)
// - export.js      the board as a picture (DOM)
// - board.css      its style sheet, every colour a variable
//
// The pure parts are checked in Node (visualization/tests/board-checks.mjs).

export * from './model.js';
export * from './operations.js';
export * from './geometry.js';
export * from './arrange.js';
export * from './schedule.js';
export * from './quantities.js';
export * from './templates.js';
export * from './tables.js';
export { CARDS, tableStamp, runModule } from './cards.js';
export { h, s, clear } from './dom.js';
export { drawPlan, drawChronicle } from './kinds.js';
export { createBoard } from './view.js';
export { createToolbar, createInspector, templatePreview } from './tools.js';
export { boardSvg, boardPng, TOKENS } from './export.js';
