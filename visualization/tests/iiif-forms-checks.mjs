// Checks of IIIF tiles (visualization/js/iiif/pyramid.js) and of forms from shapes (visualization/js/linked/forms.js), ADR 0120.
// Tiles: the levels of a 6000 × 4000 image cover it exactly at every scale factor; the tiles a view needs are those that meet it and
// no others, each request in the Image API's form; the factor chosen never stretches a tile more than twofold; a zoom about a point
// keeps that point where it is. Forms: a shape read as fields in order, a sequence path with the classes it passes through; a field
// filled in writes the statements that make it say so and reads back; the nodes a path passes through made once and shared.
// Called by run-tests.mjs.

import { iiif as I, linked as L } from '../js/index.js';

const iri = (v) => ({ kind: 'iri', value: v });
const lit = (v, d = 'http://www.w3.org/2001/XMLSchema#string') => ({ kind: 'literal', value: v, datatype: d });
const bl = (v) => ({ kind: 'blank', value: v });
const SH = 'http://www.w3.org/ns/shacl#', RDF = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#', CRM = 'http://www.cidoc-crm.org/cidoc-crm/', AS = 'urn:archive-studio:vocabulary#';

export function iiifFormsChecks({ assert, check }) {
  const info = { id: 'https://example.org/iiif/plan', width: 6000, height: 4000, tiles: [{ width: 512, scaleFactors: [1, 2, 4, 8, 16] }] };
  const levels = I.levels(info);
  let covered = true;
  for (const l of levels) {
    const ts = I.tiles(info, l.factor, { x: 0, y: 0, w: 6000, h: 4000 });
    const area = ts.reduce((s, t) => s + t.region[2] * t.region[3], 0);
    covered = covered && area === 6000 * 4000 && ts.length === l.columns * l.rows && ts.every((t) => t.width <= 512 && t.height <= 512);
  }
  assert('iiif: at every scale factor the tiles cover the image exactly, none larger than 512 pixels', covered && levels.at(-1).columns === 1 && levels.at(-1).rows === 1);
  const view = { x: 1000, y: 900, w: 600, h: 300 };
  const ts = I.tiles(info, 1, view);
  const meets = (t) => t.region[0] < view.x + view.w && t.region[0] + t.region[2] > view.x && t.region[1] < view.y + view.h && t.region[1] + t.region[3] > view.y;
  assert('iiif: the tiles a view needs are exactly those that meet it (three columns by two rows)', ts.length === 6 && ts.every(meets) && ts[0].url === 'https://example.org/iiif/plan/512,512,512,512/512,/0/default.jpg');
  const last = I.tiles(info, 4, { x: 5000, y: 3000, w: 1000, h: 1000 }).at(-1);
  assert('iiif: a tile at the image\'s edge is cut to it, and asked at its scaled width', last.region[0] + last.region[2] === 6000 && last.region[1] + last.region[3] === 4000 && last.width === Math.ceil(last.region[2] / 4));
  let stretch = true;
  for (const scale of [0.02, 0.05, 0.1, 0.2, 0.33, 0.5, 0.9, 1, 2, 4]) {
    const f = I.factorFor(info, scale);
    stretch = stretch && (scale * f <= 2.0001 || f === 1) && (1 / f >= scale * 0.999 || f === 1);
  }
  assert('iiif: the factor chosen shows at least the screen\'s resolution and never stretches a tile more than twofold', stretch);
  const v = new I.View(800, 600, info);
  const before = v.toImage(200, 150);
  v.zoom(2.5, 200, 150);
  const after = v.toImage(200, 150);
  check('iiif: a zoom about a point keeps that point where it is (pixels of the image)', Math.hypot(before[0] - after[0], before[1] - after[1]), 1e-9);
  const seen = new I.View(800, 600, info).visible();
  assert('iiif: fitted, the whole image is in view', seen.x === 0 && seen.y === 0 && Math.abs(seen.w - 6000) < 1e-6 && Math.abs(seen.h - 4000) < 1e-6);

  // A small CIDOC CRM template: a title, and who made it by a path through the production.
  const s = iri('urn:t:shape'), p1 = bl('p1'), p2 = bl('p2'), l1 = bl('l1'), l2 = bl('l2'), t1 = bl('t1');
  const shape = [
    [s, iri(`${RDF}type`), iri(`${SH}NodeShape`)], [s, iri('http://www.w3.org/2000/01/rdf-schema#label'), lit('CIDOC CRM')], [s, iri(`${SH}targetClass`), iri(`${CRM}E22_Human-Made_Object`)],
    [s, iri(`${SH}property`), p1], [p1, iri(`${SH}path`), iri(`${CRM}P3_has_note`)], [p1, iri(`${SH}name`), lit('Note')], [p1, iri(`${SH}order`), lit('2', 'http://www.w3.org/2001/XMLSchema#integer')],
    [s, iri(`${SH}property`), p2], [p2, iri(`${SH}path`), l1], [p2, iri(`${SH}name`), lit('Made by')], [p2, iri(`${SH}nodeKind`), iri(`${SH}IRI`)], [p2, iri(`${SH}order`), lit('1', 'http://www.w3.org/2001/XMLSchema#integer')],
    [l1, iri(`${RDF}first`), iri(`${CRM}P108i_was_produced_by`)], [l1, iri(`${RDF}rest`), l2], [l2, iri(`${RDF}first`), iri(`${CRM}P14_carried_out_by`)], [l2, iri(`${RDF}rest`), iri(`${RDF}nil`)],
    [p2, iri(`${AS}through`), t1], [t1, iri(`${RDF}first`), iri(`${CRM}E12_Production`)], [t1, iri(`${RDF}rest`), iri(`${RDF}nil`)],
  ];
  const f = L.form(shape, 'urn:t:shape');
  assert('forms: a shape read as its fields in order, a sequence path with the class it passes through', f.label === 'CIDOC CRM' && f.fields.length === 2 && f.fields[0].name === 'Made by'
    && f.fields[0].path.length === 2 && f.fields[0].through[0] === `${CRM}E12_Production` && f.fields[0].kind === 'iri' && f.fields[1].kind === 'literal' && L.shapes(shape).length === 1);
  const node = iri('urn:t:plan');
  let n = 0;
  const fresh = () => `made${n++}`;
  const w1 = L.writeField([], node, f.fields[0], ['urn:t:brandt'], fresh);
  const g1 = w1.added;
  assert('forms: filling a path makes the production it passes through, typed, and the maker at its end', g1.length === 3 && g1.some((q) => q[2].value === `${CRM}E12_Production`)
    && L.fieldValues(g1, node, f.fields[0]).map((t) => t.value).join() === 'urn:t:brandt');
  const w2 = L.writeField(g1, node, f.fields[0], ['urn:t:brandt', 'urn:t:okafor'], fresh);
  const g2 = [...g1, ...w2.added];
  assert('forms: a second maker shares the production already there, and nothing is removed', w2.added.length === 1 && w2.removed.length === 0 && L.fieldValues(g2, node, f.fields[0]).length === 2);
  const w3 = L.writeField(g2, node, f.fields[0], ['urn:t:okafor'], fresh);
  assert('forms: a maker taken out is one statement removed', w3.removed.length === 1 && w3.removed[0][2].value === 'urn:t:brandt' && w3.added.length === 0);
  const w4 = L.writeField([], node, f.fields[1], ['Drawn in pencil'], fresh);
  assert('forms: a literal field writes one statement of its datatype', w4.added.length === 1 && w4.added[0][2].kind === 'literal' && w4.added[0][1].value === `${CRM}P3_has_note`);
}
