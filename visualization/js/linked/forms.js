// Forms from shapes (ADR 0120): a template of description (Dublin Core, Records in Contexts, CIDOC CRM, or one's own) is a SHACL
// node shape, and this module reads it as a form and writes what is filled in back as statements. The template is data, not code:
// a vocabulary is added by writing a shape, and the same shape checks what the form wrote (symploke's SHACL).
//
//   - shapes(quads): the node shapes a graph declares, each with its label, comment and target class.
//   - form(quads, shape): its fields, in sh:order: each a property shape's path (an IRI, or a sequence path written as an RDF list,
//     SHACL §2.3.1), its name and description, what its values are (sh:datatype, sh:nodeKind, sh:class, sh:in), its least and most
//     values, its group, and, for a sequence path, the classes of the nodes it passes through (as:through, the studio's own term),
//     so that a CIDOC CRM path such as P108i_was_produced_by / P14_carried_out_by is filled in as one field.
//   - values(quads, node, field): what the graph says at the end of the field's path from a node.
//   - write(quads, node, field, words, fresh): the change that makes the field say `words`: the old values at the end of the path
//     removed, the new added, and the nodes the path passes through made (blank, typed by as:through) where none is there yet.
// Statements are [s, p, o] of terms {kind, value, datatype?, language?}, as the API's science.linked gives them.

import { key } from './model.js';

const SH = 'http://www.w3.org/ns/shacl#';
const RDF = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
const RDFS = 'http://www.w3.org/2000/01/rdf-schema#';
const XSD = 'http://www.w3.org/2001/XMLSchema#';
export const AS = 'urn:archive-studio:vocabulary#';

const objects = (quads, s, p) => quads.filter((q) => key(q[0]) === key(s) && q[1].value === p).map((q) => q[2]);
const one = (quads, s, p) => objects(quads, s, p)[0];
const text = (t) => (t ? t.value : '');

function list(quads, head) {
  const out = [];
  let node = head;
  for (let guard = 0; node && !(node.kind === 'iri' && node.value === `${RDF}nil`) && guard < 64; guard++) {
    const first = one(quads, node, `${RDF}first`);
    if (!first) break;
    out.push(first);
    node = one(quads, node, `${RDF}rest`);
  }
  return out;
}

export function shapes(quads) {
  return quads.filter((q) => q[1].value === `${RDF}type` && q[2].value === `${SH}NodeShape`).map((q) => q[0])
    .filter((s, i, a) => a.findIndex((t) => key(t) === key(s)) === i)
    .map((s) => ({ node: s, label: text(one(quads, s, `${RDFS}label`) || one(quads, s, `${SH}name`)) || s.value, comment: text(one(quads, s, `${RDFS}comment`)),
      target: one(quads, s, `${SH}targetClass`)?.value || null, order: Number(text(one(quads, s, `${SH}order`))) || 0 }))
    .sort((a, b) => a.order - b.order || a.label.localeCompare(b.label));
}

export function form(quads, shape) {
  const s = typeof shape === 'string' ? { kind: 'iri', value: shape } : shape;
  const fields = objects(quads, s, `${SH}property`).map((p, k) => {
    const pathTerm = one(quads, p, `${SH}path`);
    const path = !pathTerm ? [] : pathTerm.kind === 'iri' ? [pathTerm.value] : list(quads, pathTerm).map((t) => t.value);
    const through = list(quads, one(quads, p, `${AS}through`)).map((t) => t.value);
    const nodeKind = one(quads, p, `${SH}nodeKind`)?.value || null;
    const cls = one(quads, p, `${SH}class`)?.value || null;
    const allowed = one(quads, p, `${SH}in`);
    return {
      key: `f${k}`, path, through, name: text(one(quads, p, `${SH}name`)) || (path.at(-1) || '').split(/[#/]/).pop(),
      description: text(one(quads, p, `${SH}description`)), datatype: one(quads, p, `${SH}datatype`)?.value || null, nodeKind, class: cls,
      in: allowed ? list(quads, allowed) : null, min: Number(text(one(quads, p, `${SH}minCount`))) || 0, max: text(one(quads, p, `${SH}maxCount`)) ? Number(text(one(quads, p, `${SH}maxCount`))) : null,
      order: Number(text(one(quads, p, `${SH}order`))) || k, group: text(one(quads, one(quads, p, `${SH}group`) || { kind: 'iri', value: '' }, `${RDFS}label`)) || '',
      multiline: text(one(quads, p, `${AS}multiline`)) === 'true',
      kind: nodeKind === `${SH}IRI` || nodeKind === `${SH}BlankNodeOrIRI` || cls ? 'iri' : 'literal',
    };
  }).filter((f) => f.path.length).sort((a, b) => a.order - b.order);
  const info = shapes(quads).find((x) => key(x.node) === key(s));
  return { shape: s, label: info?.label || s.value, comment: info?.comment || '', target: info?.target || null, fields };
}

// The nodes reached from `node` along the first `steps` of the path.
function reach(quads, node, path, steps) {
  let at = [node];
  for (let i = 0; i < steps; i++) at = at.flatMap((n) => objects(quads, n, path[i]));
  return at;
}

export function values(quads, node, field) {
  return reach(quads, node, field.path, field.path.length);
}

// A word as the term the field's values are.
export function term(field, word) {
  const w = String(word).trim();
  if (field.in) { const t = field.in.find((x) => x.value === w || x.value.split(/[#/]/).pop() === w); if (t) return t; }
  if (field.kind === 'iri') return { kind: 'iri', value: w };
  return { kind: 'literal', value: w, datatype: field.datatype || `${XSD}string` };
}

export function write(quads, node, field, words, fresh) {
  const added = [], removed = [];
  const path = field.path;
  let at = node;
  for (let i = 0; i < path.length - 1; i++) {
    const next = objects(quads, at, path[i])[0] || added.find((q) => key(q[0]) === key(at) && q[1].value === path[i])?.[2];
    if (next) { at = next; continue; }
    const b = { kind: 'blank', value: fresh() };
    added.push([at, { kind: 'iri', value: path[i] }, b]);
    if (field.through[i]) added.push([b, { kind: 'iri', value: `${RDF}type` }, { kind: 'iri', value: field.through[i] }]);
    at = b;
  }
  const last = { kind: 'iri', value: path.at(-1) };
  const now = objects(quads, at, last.value);
  const wanted = words.map((w) => term(field, w)).filter((t) => t.value);
  for (const t of now) if (!wanted.some((w) => key(w) === key(t))) removed.push([at, last, t]);
  for (const t of wanted) if (!now.some((w) => key(w) === key(t))) added.push([at, last, t]);
  return { added, removed };
}
