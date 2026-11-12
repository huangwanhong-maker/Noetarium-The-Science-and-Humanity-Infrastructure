// Categories drawn (ADR 0100): the public interface of visualization/js/categories.
//
// Kategoria answers a script with views; these functions lay them out and return markup, for any work's page.
// categories.css beside this file gives every mark a default look, which a work's own stylesheet may change.

export { layers, ordered, layoutCategory, textWidth } from './layout.js';
export { layoutStrings, svgStrings, htmlEquation } from './strings.js';
export { DRAWN, GLYPH, HUES, svgCategory, htmlCategory, htmlFunctor, htmlTransformation, htmlAdjunction, htmlCone, svgSets, htmlSets, htmlDerivation, htmlTable, htmlComposition,
         htmlTheory, htmlLaws, render } from './draw.js';
