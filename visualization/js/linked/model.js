// Linked data as something to draw (ADR 0119): terms named briefly, a graph's nodes and edges, a neighbourhood, a layout.
//
// Terms come as the API's science.linked gives them: {kind: 'iri' | 'blank' | 'literal', value, datatype?, language?}; a statement
// is [s, p, o] or [s, p, o, g]. Nothing here asks a server: a work hands these functions what symploke answered.
//
//   - label(term, prefixes, names): a node by its name where the graph gives one (names(quads): dcterms:title, rdfs:label,
//     skos:prefLabel, foaf:name, schema:name); else an IRI as prefix:local where a prefix fits, else its last segment, decoded;
//     a blank node as _:label; a literal as its text with its language or datatype in brief.
//   - graphModel(quads, options): the nodes (IRIs and blank nodes; literals kept on their subjects as attributes, unless
//     literals: 'nodes') and the edges between them, each node with its classes (objects of rdf:type, drawn as its kind and
//     not as edges unless types: 'edges'), its degree and its literals.
//   - neighbourhood(model, key, depth): the nodes within depth steps of one, whichever way the edges run.
//   - layoutGraph(model, options): a force-directed layout (Fruchterman and Reingold 1991, Software: Practice and Experience
//     21, 1129–1164) with a weak pull to the centre, from a seeded start, so that the same graph is always drawn the same; laid
//     out freely and then fitted into the frame.

export const RDF_TYPE = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#type';
const XSD = 'http://www.w3.org/2001/XMLSchema#';
const LANG_STRING = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#langString';

export const COMMON_PREFIXES = {
  rdf: 'http://www.w3.org/1999/02/22-rdf-syntax-ns#', rdfs: 'http://www.w3.org/2000/01/rdf-schema#', xsd: XSD, owl: 'http://www.w3.org/2002/07/owl#',
  sh: 'http://www.w3.org/ns/shacl#', dcterms: 'http://purl.org/dc/terms/', dcmitype: 'http://purl.org/dc/dcmitype/', skos: 'http://www.w3.org/2004/02/skos/core#',
  prov: 'http://www.w3.org/ns/prov#', foaf: 'http://xmlns.com/foaf/0.1/', schema: 'https://schema.org/', oa: 'http://www.w3.org/ns/oa#',
  disco: 'http://rdf-vocabulary.ddialliance.org/discovery#', rico: 'https://www.ica.org/standards/RiC/ontology#', crm: 'http://www.cidoc-crm.org/cidoc-crm/',
};

export const key = (t) => (t ? `${t.kind}|${t.value}|${t.datatype || ''}|${t.language || ''}` : '');

export const NAME_PREDICATES = ['http://purl.org/dc/terms/title', 'http://www.w3.org/2000/01/rdf-schema#label', 'http://www.w3.org/2004/02/skos/core#prefLabel',
  'http://xmlns.com/foaf/0.1/name', 'https://schema.org/name', 'http://purl.org/dc/elements/1.1/title'];

// The name each node is given in the statements, by the first naming predicate that names it (English preferred, then untagged).
export function names(quads) {
  const out = new Map(), rank = new Map();
  for (const [s, p, o] of quads) {
    const r = NAME_PREDICATES.indexOf(p.value);
    if (r < 0 || o.kind !== 'literal') continue;
    const score = r * 3 + (o.language === 'en' ? 0 : o.language ? 2 : 1), k = key(s);
    if (!rank.has(k) || score < rank.get(k)) rank.set(k, score), out.set(k, o.value);
  }
  return out;
}

export function label(t, prefixes = COMMON_PREFIXES, named = null) {
  if (!t) return '';
  if (named && named.has(key(t))) {
    const n = named.get(key(t));
    return n.length > 48 ? `${n.slice(0, 45)}…` : n;
  }
  if (t.kind === 'blank') return `_:${t.value}`;
  if (t.kind === 'literal') {
    const text = t.value.length > 60 ? `${t.value.slice(0, 57)}…` : t.value;
    if (t.language) return `“${text}”@${t.language}`;
    if (t.datatype && t.datatype !== `${XSD}string` && t.datatype !== LANG_STRING) return `“${text}” ${brief(t.datatype, prefixes)}`;
    return `“${text}”`;
  }
  return brief(t.value, prefixes);
}

// An IRI in brief: the longest namespace that fits, else what follows the last # or /.
export function brief(iri, prefixes = COMMON_PREFIXES) {
  let best = null;
  for (const [p, ns] of Object.entries(prefixes || {})) if (iri.startsWith(ns) && iri.length > ns.length && (!best || ns.length > best[1].length)) best = [p, ns];
  const decode = (x) => { try { return decodeURIComponent(x); } catch { return x; } };
  if (best) return `${best[0]}:${decode(iri.slice(best[1].length))}`;
  const cut = Math.max(iri.lastIndexOf('#'), iri.lastIndexOf('/'), iri.lastIndexOf(':'));
  return cut >= 0 && cut < iri.length - 1 ? decode(iri.slice(cut + 1)) : iri;
}

export function graphModel(quads, { literals = 'attributes', types = 'kinds', prefixes = COMMON_PREFIXES } = {}) {
  const nodes = new Map(), edges = [], named = names(quads);
  const node = (t) => {
    const k = key(t);
    if (!nodes.has(k)) nodes.set(k, { key: k, term: t, label: label(t, prefixes, named), kind: t.kind, classes: [], literals: [], degree: 0 });
    return nodes.get(k);
  };
  for (const [s, p, o, g] of quads) {
    const a = node(s);
    if (o.kind === 'literal' && literals !== 'nodes') {
      a.literals.push({ predicate: p, value: o, graph: g || null });
      continue;
    }
    if (p.value === RDF_TYPE && types !== 'edges' && o.kind === 'iri') {
      a.classes.push(o);
      continue;
    }
    const b = node(o);
    edges.push({ from: a.key, to: b.key, predicate: p, label: label(p, prefixes), graph: g || null });
    a.degree++;
    b.degree++;
  }
  return { nodes: [...nodes.values()], edges };
}

export function neighbourhood(model, k, depth = 1) {
  const near = new Set([k]);
  let frontier = [k];
  for (let d = 0; d < depth; d++) {
    const next = [];
    for (const e of model.edges) {
      if (frontier.includes(e.from) && !near.has(e.to)) near.add(e.to), next.push(e.to);
      if (frontier.includes(e.to) && !near.has(e.from)) near.add(e.from), next.push(e.from);
    }
    frontier = next;
  }
  return { nodes: model.nodes.filter((n) => near.has(n.key)), edges: model.edges.filter((e) => near.has(e.from) && near.has(e.to)) };
}

// A small generator of numbers in [0, 1) from a seed (mulberry32), so that layouts repeat.
function stream(seed) {
  let a = seed >>> 0;
  return () => {
    a = (a + 0x6d2b79f5) >>> 0;
    let t = a;
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

export function layoutGraph(model, { width = 900, height = 600, iterations = 450, seed = 7, margin = 40, fixed = {}, separation = [110, 34] } = {}) {
  const n = model.nodes.length, pos = new Map();
  if (n === 0) return pos;
  const random = stream(seed), index = new Map(model.nodes.map((v, i) => [v.key, i]));
  const x = new Float64Array(n), y = new Float64Array(n);
  for (let i = 0; i < n; i++) {
    const f = fixed[model.nodes[i].key];
    x[i] = f ? f.x : margin + random() * (width - 2 * margin);
    y[i] = f ? f.y : margin + random() * (height - 2 * margin);
  }
  // A pull towards the centre keeps a sparse graph off the frame's edges (gravity, as Fruchterman and Reingold's variants add).
  const area = (width - 2 * margin) * (height - 2 * margin), k = 0.95 * Math.sqrt(area / n), cx = width / 2, cy = height / 2, gravity = 0.05;
  const links = model.edges.map((e) => [index.get(e.from), index.get(e.to)]).filter(([a, b]) => a !== b);
  let temperature = Math.min(width, height) / 8;
  const dx = new Float64Array(n), dy = new Float64Array(n);
  for (let it = 0; it < iterations; it++) {
    dx.fill(0), dy.fill(0);
    for (let i = 0; i < n; i++)
      for (let j = i + 1; j < n; j++) {
        let ex = x[i] - x[j], ey = y[i] - y[j];
        let d = Math.hypot(ex, ey);
        if (d > 3 * k) continue;  // repulsion only within 3k, as in Fruchterman and Reingold's grid variant: components then stay near
        if (d < 0.01) ex = (random() - 0.5) * 0.1, ey = (random() - 0.5) * 0.1, d = 0.05;
        const f = (k * k) / d;
        dx[i] += (ex / d) * f, dy[i] += (ey / d) * f;
        dx[j] -= (ex / d) * f, dy[j] -= (ey / d) * f;
      }
    for (const [a, b] of links) {
      const ex = x[a] - x[b], ey = y[a] - y[b], d = Math.max(Math.hypot(ex, ey), 0.01), f = (d * d) / k;
      dx[a] -= (ex / d) * f, dy[a] -= (ey / d) * f;
      dx[b] += (ex / d) * f, dy[b] += (ey / d) * f;
    }
    for (let i = 0; i < n; i++) {
      if (fixed[model.nodes[i].key]) continue;
      dx[i] -= (x[i] - cx) * gravity;
      dy[i] -= (y[i] - cy) * gravity * (width / height);
      const d = Math.max(Math.hypot(dx[i], dy[i]), 0.01), step = Math.min(d, temperature);
      x[i] += (dx[i] / d) * step;
      y[i] += (dy[i] / d) * step;
    }
    temperature *= 0.988;
  }
  // Laid out freely, then fitted into the frame (a margin wider at the sides, where names run), so that nothing sticks to an edge.
  const free = model.nodes.map((v, i) => !fixed[v.key] ? i : -1).filter((i) => i >= 0);
  if (free.length) {
    const mx = Math.max(margin, 70), xs = free.map((i) => x[i]), ys = free.map((i) => y[i]);
    const [x0, x1, y0, y1] = [Math.min(...xs), Math.max(...xs), Math.min(...ys), Math.max(...ys)];
    for (const i of free) {
      x[i] = x1 > x0 ? mx + ((x[i] - x0) / (x1 - x0)) * (width - 2 * mx) : width / 2;
      y[i] = y1 > y0 ? margin + ((y[i] - y0) / (y1 - y0)) * (height - 2 * margin) : height / 2;
    }
  }
  // Names are wider than they are tall: two nodes nearer than an ellipse as wide as half their names together (and `separation`
  // at least) are moved apart, half each, for a few rounds.
  const [sx0, sy] = separation, mx2 = Math.max(margin, 70);
  const half = model.nodes.map((v) => (v.kind === 'blank' ? 12 : Math.min(150, 3.3 * (v.label || '').length)));
  for (let round = 0; round < 120; round++) {
    let moved = false;
    for (let i = 0; i < n; i++)
      for (let j = i + 1; j < n; j++) {
        const sx = Math.max(sx0, half[i] + half[j] + 14);
        const ex = (x[j] - x[i]) / sx, ey = (y[j] - y[i]) / sy, d = Math.hypot(ex, ey);
        if (d >= 1) continue;
        moved = true;
        const push = (1 - d) / 2 + 0.01, ux = d > 1e-6 ? ex / d : 1, uy = d > 1e-6 ? ey / d : 0;
        if (!fixed[model.nodes[i].key]) x[i] -= ux * push * sx, y[i] -= uy * push * sy;
        if (!fixed[model.nodes[j].key]) x[j] += ux * push * sx, y[j] += uy * push * sy;
      }
    for (const i of free) x[i] = Math.min(width - mx2, Math.max(mx2, x[i])), y[i] = Math.min(height - margin, Math.max(margin, y[i]));
    if (!moved) break;
  }
  for (let i = 0; i < n; i++) pos.set(model.nodes[i].key, { x: x[i], y: y[i] });
  return pos;
}
