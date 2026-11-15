// Linked data drawn (ADR 0119): a graph as SVG markup, Turtle coloured as HTML, and Web Annotations as statements.
//
//   - svgGraph(model, positions, options): edges as hairlines with their predicates, arrowheads at the object; IRIs as rings, blank
//     nodes as small hollow dots, a node's classes as a slip under its name; what is selected, what a query or a shape marks, and
//     what was entailed (dashed) told apart by class names a stylesheet colours. Every name escaped.
//   - htmlTurtle(text): Turtle (or SPARQL) as HTML spans by token (comments, IRIs, prefixed names, literals, numbers, keywords,
//     variables, punctuation), the text itself unchanged, for an editor's overlay.
//   - annotation(...): the statements of a W3C Web Annotation (Web Annotation Data Model, W3C Recommendation 2017, §3–4): a body
//     (text, or a tag that is an IRI), a target (a holding) with a selector, the media fragment of a region of an image (xywh) or
//     a span of time (t), or a passage of text by its quotation and position.

import { key, brief, COMMON_PREFIXES } from './model.js';

const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');

export function svgGraph(model, positions, { width = 900, height = 600, selected = null, marked = new Set(), inferred = new Set(), faint = new Set(), title = 'A graph of linked data' } = {}) {
  const out = [`<svg class="l-graph" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" role="img" aria-label="${esc(title)}">`,
    '<defs><marker id="l-head" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse"><path d="M0,1 L9,5 L0,9" class="l-head"/></marker></defs>'];
  const r = (n) => (n.kind === 'blank' ? 4 : 6 + Math.min(6, Math.sqrt(n.degree)));
  const byKey = new Map(model.nodes.map((n) => [n.key, n]));
  model.edges.forEach((e, i) => {
    const a = positions.get(e.from), b = positions.get(e.to);
    if (!a || !b) return;
    const na = byKey.get(e.from), nb = byKey.get(e.to);
    const ex = b.x - a.x, ey = b.y - a.y, d = Math.max(Math.hypot(ex, ey), 1);
    const x1 = a.x + (ex / d) * r(na), y1 = a.y + (ey / d) * r(na), x2 = b.x - (ex / d) * (r(nb) + 2), y2 = b.y - (ey / d) * (r(nb) + 2);
    const id = `e${i}`;
    const cls = ['l-edge', inferred.has(id) ? 'inferred' : '', marked.has(id) ? 'marked' : '', faint.has(e.from) || faint.has(e.to) ? 'faint' : ''].filter(Boolean).join(' ');
    out.push(`<g class="${cls}" data-edge="${i}"><line x1="${x1.toFixed(1)}" y1="${y1.toFixed(1)}" x2="${x2.toFixed(1)}" y2="${y2.toFixed(1)}" marker-end="url(#l-head)"/>` +
      `<text x="${((a.x + b.x) / 2).toFixed(1)}" y="${((a.y + b.y) / 2 - 3).toFixed(1)}" text-anchor="middle">${esc(e.label)}</text></g>`);
  });
  for (const n of model.nodes) {
    const p = positions.get(n.key);
    if (!p) continue;
    const cls = ['l-node', n.kind, n.key === selected ? 'selected' : '', marked.has(n.key) ? 'marked' : '', faint.has(n.key) ? 'faint' : ''].filter(Boolean).join(' ');
    const kinds = n.classes.length ? `<text class="l-kind" x="${p.x.toFixed(1)}" y="${(p.y + r(n) + 22).toFixed(1)}" text-anchor="middle">${esc(n.classes.map((c) => brief(c.value)).join(' · '))}</text>` : '';
    out.push(`<g class="${cls}" data-node="${esc(n.key)}" tabindex="0"><circle cx="${p.x.toFixed(1)}" cy="${p.y.toFixed(1)}" r="${r(n).toFixed(1)}"/>` +
      (n.kind === 'blank' ? '' : `<text class="l-name" x="${p.x.toFixed(1)}" y="${(p.y + r(n) + 11).toFixed(1)}" text-anchor="middle">${esc(n.label)}</text>`) + kinds + '</g>');
  }
  out.push('</svg>');
  return out.join('');
}

const TOKENS = [
  ['comment', /^#[^\n]*/],
  ['literal', /^("""[\s\S]*?"""|'''[\s\S]*?'''|"(?:[^"\\\n]|\\.)*"|'(?:[^'\\\n]|\\.)*')(@[A-Za-z]+(?:-[A-Za-z0-9]+)*)?/],
  ['iri', /^<[^>\s]*>/],
  ['variable', /^[?$][A-Za-z_][A-Za-z0-9_]*/],
  ['keyword', /^(@prefix|@base|PREFIX|BASE|SELECT|CONSTRUCT|ASK|WHERE|FILTER|OPTIONAL|UNION|MINUS|BIND|VALUES|GRAPH|GROUP\s+BY|ORDER\s+BY|HAVING|LIMIT|OFFSET|DISTINCT|AS|a|true|false)(?![A-Za-z0-9_:-])/i],
  ['number', /^[+-]?(\d+\.?\d*([eE][+-]?\d+)?|\.\d+([eE][+-]?\d+)?)/],
  ['blank', /^_:[A-Za-z0-9_][A-Za-z0-9_.-]*/],
  ['prefixed', /^[A-Za-z][A-Za-z0-9_.-]*:[A-Za-z0-9_:%.\\-]*|^:[A-Za-z0-9_:%.-]*/],
  ['punct', /^[.;,\[\]{}()^*+|/!=<>&-]/],
];

export function htmlTurtle(text) {
  let out = '', i = 0;
  while (i < text.length) {
    const rest = text.slice(i);
    const space = /^\s+/.exec(rest);
    if (space) {
      out += esc(space[0]);
      i += space[0].length;
      continue;
    }
    let matched = false;
    for (const [cls, re] of TOKENS) {
      const m = re.exec(rest);
      if (m && m[0].length) {
        out += `<span class="t-${cls}">${esc(m[0])}</span>`;
        i += m[0].length;
        matched = true;
        break;
      }
    }
    if (!matched) out += esc(text[i++]);
  }
  return out;
}

// ---- Web Annotation -------------------------------------------------------------------------------------------------------------

const OA = 'http://www.w3.org/ns/oa#', RDF = COMMON_PREFIXES.rdf, DCTERMS = COMMON_PREFIXES.dcterms, XSD = COMMON_PREFIXES.xsd;
const iri = (value) => ({ kind: 'iri', value });
const blank = (value) => ({ kind: 'blank', value });
const lit = (value, datatype = `${XSD}string`) => ({ kind: 'literal', value: String(value), datatype });

// selector: {kind: 'region', x, y, w, h} (pixels of the image) | {kind: 'time', start, end} (seconds) |
//           {kind: 'text', exact, prefix?, suffix?, start?, end?}; body: {text} | {tag: IRI}; motivation: an oa: motivation.
export function annotation({ id, target, selector = null, body, motivation = 'commenting', creator = null, created = null }) {
  const a = iri(id), q = [];
  const add = (s, p, o) => q.push([s, iri(p), o]);
  add(a, `${RDF}type`, iri(`${OA}Annotation`));
  add(a, `${OA}motivatedBy`, iri(`${OA}${motivation}`));
  if (created) add(a, `${DCTERMS}created`, lit(created, `${XSD}dateTime`));
  if (creator) add(a, `${DCTERMS}creator`, lit(creator));
  if (body?.tag) add(a, `${OA}hasBody`, iri(body.tag));
  else if (body?.text != null) {
    const b = blank(`${id.replace(/[^A-Za-z0-9]/g, '')}body`);
    add(a, `${OA}hasBody`, b);
    add(b, `${RDF}type`, iri(`${OA}TextualBody`));
    add(b, `${RDF}value`, lit(body.text));
  }
  if (!selector) {
    add(a, `${OA}hasTarget`, iri(target));
    return q;
  }
  const t = blank(`${id.replace(/[^A-Za-z0-9]/g, '')}target`), s = blank(`${id.replace(/[^A-Za-z0-9]/g, '')}selector`);
  add(a, `${OA}hasTarget`, t);
  add(t, `${OA}hasSource`, iri(target));
  add(t, `${OA}hasSelector`, s);
  if (selector.kind === 'region' || selector.kind === 'time') {
    add(s, `${RDF}type`, iri(`${OA}FragmentSelector`));
    add(s, `${DCTERMS}conformsTo`, iri('http://www.w3.org/TR/media-frags/'));
    const value = selector.kind === 'region'
      ? `xywh=${[selector.x, selector.y, selector.w, selector.h].map((v) => Math.round(v)).join(',')}`
      : `t=${(+selector.start).toFixed(2)},${(+selector.end).toFixed(2)}`;
    add(s, `${RDF}value`, lit(value));
  } else if (selector.kind === 'text') {
    add(s, `${RDF}type`, iri(`${OA}TextQuoteSelector`));
    add(s, `${OA}exact`, lit(selector.exact));
    if (selector.prefix) add(s, `${OA}prefix`, lit(selector.prefix));
    if (selector.suffix) add(s, `${OA}suffix`, lit(selector.suffix));
    if (Number.isInteger(selector.start) && Number.isInteger(selector.end)) {
      const p = blank(`${id.replace(/[^A-Za-z0-9]/g, '')}position`);
      add(s, `${OA}refinedBy`, p);
      add(p, `${RDF}type`, iri(`${OA}TextPositionSelector`));
      add(p, `${OA}start`, lit(selector.start, `${XSD}nonNegativeInteger`));
      add(p, `${OA}end`, lit(selector.end, `${XSD}nonNegativeInteger`));
    }
  }
  return q;
}

// The annotations of a set of statements, read back: [{id, target, selector, body, motivation}].
export function annotations(quads) {
  const by = new Map();
  for (const [s, p, o] of quads) {
    const k = key(s);
    if (!by.has(k)) by.set(k, []);
    by.get(k).push([p.value, o]);
  }
  const get = (t, p) => (by.get(key(t)) || []).find(([q]) => q === p)?.[1];
  const out = [];
  for (const [s, p, o] of quads) {
    if (p.value !== `${RDF}type` || o.value !== `${OA}Annotation`) continue;
    const target = get(s, `${OA}hasTarget`), bodyTerm = get(s, `${OA}hasBody`);
    let source = target, selector = null;
    if (target && target.kind === 'blank') {
      source = get(target, `${OA}hasSource`);
      const sel = get(target, `${OA}hasSelector`);
      if (sel) {
        const value = get(sel, `${RDF}value`)?.value, exact = get(sel, `${OA}exact`)?.value;
        if (value?.startsWith('xywh=')) { const [x, y, w, h] = value.slice(5).split(',').map(Number); selector = { kind: 'region', x, y, w, h }; }
        else if (value?.startsWith('t=')) { const [start, end] = value.slice(2).split(',').map(Number); selector = { kind: 'time', start, end }; }
        else if (exact != null) selector = { kind: 'text', exact, prefix: get(sel, `${OA}prefix`)?.value, suffix: get(sel, `${OA}suffix`)?.value };
      }
    }
    const body = bodyTerm?.kind === 'iri' ? { tag: bodyTerm.value } : bodyTerm ? { text: get(bodyTerm, `${RDF}value`)?.value ?? '' } : null;
    out.push({ id: s.value, target: source?.value ?? null, selector, body, motivation: (get(s, `${OA}motivatedBy`)?.value || '').replace(OA, ''), creator: get(s, `${DCTERMS}creator`)?.value || null });
  }
  return out;
}
