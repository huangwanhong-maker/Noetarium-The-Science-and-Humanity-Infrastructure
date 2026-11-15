// Linked data drawn (ADR 0119): the public interface of visualization/js/linked.
//
// symploke answers with terms and statements; these functions name them briefly, gather them into a graph, lay it out and
// return markup, colour Turtle and SPARQL, and write and read W3C Web Annotations, for any work's page. linked.css beside this
// file gives every mark a default look, which a work's own stylesheet may change.

export { RDF_TYPE, COMMON_PREFIXES, NAME_PREDICATES, key, names, label, brief, graphModel, neighbourhood, layoutGraph } from './model.js';
export { svgGraph, htmlTurtle, annotation, annotations } from './draw.js';
export { shapes, form, values as fieldValues, write as writeField, term as fieldTerm, AS } from './forms.js';
