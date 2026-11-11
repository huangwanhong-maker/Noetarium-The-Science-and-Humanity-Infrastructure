// A project's workspace (ADR 0093): what a general, project-based work keeps and what its views make of a table. Of no
// subject: a work brings its tools; the workspace knows only data bound to roles, slices of space and time, steps and views.
//
// - model.js   the schema noetarium.workspace/1: sources and their roles, slices, quantities, steps and their staleness, views,
//              boards; instants, periods and the rows of a table within a slice and at an instant; a project's plane (pure)
// - views.js   what a chart gathers and draws of a table (bars, stacked bars, lines, points, a histogram, shares); how a column
//              is scaled for a layer; the view a table suggests (pure)
//
// Checked in Node (visualization/tests/workspace-checks.mjs).

export * from './model.js';
export * from './views.js';
