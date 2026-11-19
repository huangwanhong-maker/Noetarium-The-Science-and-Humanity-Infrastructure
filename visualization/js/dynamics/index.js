// Drawings of dynamical systems (ADR 0103): charts as markup that needs no page, and the same systems in the round; fields
// on a grid of cells with their currents (ADR 0124); systems across scales, their levels and couplings (ADR 0128); and what
// kinesis's detectors of critical transitions said of a record (ADR 0135).
export { INKS, braidDiagram, complexPlane, formsGraph, heatMap, hue, lineChart, ticks } from './charts.js';
export { path3d, strands3d, surface3d } from './space.js';
export { arrowsOf, cellsOf, changeOf, changeScaleOf, coloursOf, fieldSvg, legendSvg, overlaySvg, paintField, scaleOf } from './fields.js';
export { modelDiagram, unitsMap } from './scales.js';
export { eigenPath, findingChart, gridMap, operating, verdictMark, wellsChart, whereBars } from './transitions.js';
