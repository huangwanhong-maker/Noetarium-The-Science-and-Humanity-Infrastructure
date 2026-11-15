// Evidence-based policy drawn (ADR 0122), of no work: a theory of change with each link's standing (theory.js); the charts of an
// appraisal: a forest plot, intervals compared, the worth of a study over its size, the decision over the worth of a unit
// (charts.js); and a theory as a document, written back into the domain's form and changed by operations (written.js). They take
// what the API's policy engine answers and ask no server; policy.css is their default look.

export { wrapText, pips, layoutTheory, svgTheory } from './theory.js';
export { figure, ticks, forestRows, svgForest, svgIntervals, svgStudy, worthLine, svgWorth, courses } from './charts.js';
export { writeTheory, freshId, addNode, setNode, removeNode, addLink, setLink, removeLink, setLevels, readStream, writeStream } from './written.js';
