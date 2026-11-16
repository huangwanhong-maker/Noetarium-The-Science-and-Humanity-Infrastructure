// Evidence and the law drawn (ADR 0121), of no work: a matter as its argument from each question down to its facts and open terms
// (matter.js), the same unfolded as an outline for a matter too large to draw (outline.js), the plan of proof and the readings of
// its terms in words (plan.js), passages marked in a document's text (passages.js); and evidence in space and time, the place as the
// page with its claims and items and a strip of time (located.js, ADR 0123). They take what the API's law engine answers and ask no server; evidence.css is their default look.

export { CLASSES, STANDINGS, PLACES, matterGraph, nodeSize, layoutMatter, svgMatter } from './matter.js';
export { STEP, waysOf, stepText, readingsTable, labelEnvironments, restsOn } from './plan.js';
export { charAt, byteAt, locate, segments } from './passages.js';
export { matterTree, unfoldedAtFirst, rows } from './outline.js';
export { frameOf, boxOf, spanOf, intervalOf, decibans, toneOf, hueOf, STANDINGS as LOCATED_STANDINGS, sceneOf, basePaths, svgPlace, scaleBar, timeTicks, svgStrip, ratioWords } from './located.js';
