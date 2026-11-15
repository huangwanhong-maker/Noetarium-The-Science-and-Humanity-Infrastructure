// A matter drawn as its argument (ADR 0121): each question at the top, the rules that give it beneath, and at the bottom, on one
// ground, the facts and the open terms it all rests on.
//
// The graph is read off the scheme (noetarium.scheme/1) backwards from each issue's question, as the law domain reads it: a rule
// gives its consequence and asks for its elements; an exception defeats a rule; a counts-as link makes a fact count as what a rule
// asks for under a reading of an open term. What the engine said of the matter (api/cpp/science/law.hpp's evaluate) is laid on it:
// each question's label (clear, arguable, untenable), each fact's standing as it entered the case and what its evidence gives,
// each term's readings fixed or set aside, the holdings that read it, and the steps an argument for the question took.
//
// matterGraph and layoutMatter are pure; svgMatter returns markup (evidence.css gives it a look). A standing is always a word and a
// glyph as well as a tone, never a tone alone.

import { esc, textWidth } from '../history/cone.js';

export const CLASSES = {
  clear: { glyph: '✓', word: 'clear' },
  arguable: { glyph: '◐', word: 'arguable' },
  untenable: { glyph: '✕', word: 'untenable' },
  'without an admissible interpretation': { glyph: '∅', word: 'no admissible reading' },
};

// The standings of a fact, each with a glyph; and the place a fact holds in the plan of proof.
export const STANDINGS = {
  final: '◆', found: '◆', admitted: '■', noticed: '■', deemed: '■', proved: '●', presumed: '◒', alleged: '○', 'not proved': '✕', rebutted: '✕',
};
export const PLACES = { given: 'given', open: 'open', decided: 'decided against', absent: 'not alleged' };

const atomOf = (l) => (String(l).startsWith('¬') ? String(l).slice(1) : String(l));
const negated = (l) => String(l).startsWith('¬');
const readingOf = (e) => {
  const m = /^\[([^=\]]+)=([^\]]+)\]$/.exec(String(e));
  return m ? { term: m[1], reading: m[2] } : null;
};

function provisionsOf(scheme, id) {
  const out = [];
  for (const kind of ['rules', 'exceptions', 'counts'])
    for (const x of scheme[kind] || [])
      if (x.id === id)
        for (const p of x.cites || []) {
          let label = p, instrument = '';
          for (const ins of scheme.instruments || [])
            for (const q of ins.provisions || []) if (q.id === p) label = q.label || p, instrument = ins.citation || ins.title || ins.id;
          out.push({ id: p, label, instrument });
        }
  return out;
}

// The graph of one issue (or of all, when none is named): nodes and edges, with what the engine said laid on them.
export function matterGraph(scheme, matter = {}, evaluation = null, issueId = null) {
  const nodes = new Map(), edges = [];
  const gloss = new Map((scheme.vocabulary || []).map((v) => [v.id, v.gloss || '']));
  const facts = new Map((matter.facts || []).map((f) => [f.id, f]));
  const evaluated = new Map(((evaluation && evaluation.facts) || []).map((f) => [f.id, f]));
  const issues = (matter.issues || []).filter((i) => !issueId || i.id === issueId);
  const answers = new Map(((evaluation && evaluation.issues) || []).map((a) => [a.issue, a]));
  const terms = new Map((scheme.terms || []).map((t) => [t.name, t]));
  const fixed = new Set((evaluation && evaluation.admissibility && evaluation.admissibility.fixed) || []);
  const aside = new Set((evaluation && evaluation.admissibility && evaluation.admissibility.set_aside) || []);
  const assessed = ((evaluation && evaluation.authority && evaluation.authority.assessed) || []);
  const steps = new Map();   // the rules, exceptions and links whose elements stand in the engine's argument, and whether each holds
  const places = new Map();
  for (const a of answers.values()) {
    for (const s of (a.rests_on && a.rests_on.steps) || []) steps.set(s.id, s.holds !== false);
    for (const k of Object.keys(PLACES)) for (const f of (a.plan && a.plan[k]) || []) places.set(f, k);
  }
  const add = (id, node) => { if (!nodes.has(id)) nodes.set(id, { id, ...node }); return nodes.get(id); };
  const edge = (from, to, kind, extra = {}) => { if (!edges.some((e) => e.from === from && e.to === to && e.kind === kind)) edges.push({ from, to, kind, ...extra }); };
  const atomNode = (a) => {
    const f = facts.get(a), e = evaluated.get(a);
    return add(`a:${a}`, {
      kind: 'atom', atom: a, text: (f && f.text) || gloss.get(a) || a,
      declared: f ? f.standing || 'alleged' : null, used: e ? e.used : null, evidence: e ? e.evidence || null : null,
      meets: e && e.meets ? e.meets.status : null, place: places.get(a) || null,
    });
  };
  const termNode = (t) => {
    const def = terms.get(t) || { name: t, readings: [] };
    // A reading is set aside when every argument for it is defeated, or when another reading of its term is fixed.
    const names = (def.readings || []).map((r) => (typeof r === 'string' ? r : r.name));
    const settled = names.some((x) => fixed.has(`[${t}=${x}]`));
    const readings = (def.readings || []).map((r) => {
      const name = typeof r === 'string' ? r : r.name;
      const w = `[${t}=${name}]`;
      return { name, gloss: typeof r === 'string' ? '' : r.gloss || '', fixed: fixed.has(w), aside: aside.has(w) || (settled && !fixed.has(w)) };
    });
    return add(`t:${t}`, { kind: 'term', term: t, text: def.gloss || t, readings, holdings: assessed.filter((h) => h.term === t) });
  };
  const seen = new Set();
  const visit = (a) => {
    if (seen.has(a)) return;
    seen.add(a);
    const node = atomNode(a);
    let given = false;
    for (const r of scheme.rules || []) {
      if (atomOf(r.consequence) !== a) continue;
      given = true;
      const rn = add(`r:${r.id}`, { kind: 'rule', rule: r.id, text: r.id, provisions: provisionsOf(scheme, r.id), applied: steps.has(r.id), holds: steps.get(r.id) === true, denies: negated(r.consequence) });
      edge(rn.id, node.id, 'gives', { negated: negated(r.consequence) });
      for (const e of r.elements || []) {
        const rd = readingOf(e);
        if (rd) { edge(termNode(rd.term).id, rn.id, 'reading', { reading: rd.reading }); continue; }
        edge(`a:${atomOf(e)}`, rn.id, 'asks', { negated: negated(e) });
        visit(atomOf(e));
      }
      for (const x of scheme.exceptions || []) {
        if (x.rule !== r.id) continue;
        const xn = add(`x:${x.id}`, { kind: 'exception', rule: x.id, text: x.id, provisions: provisionsOf(scheme, x.id), applied: steps.has(x.id) });
        edge(xn.id, rn.id, 'defeats');
        for (const e of x.elements || []) {
          const rd = readingOf(e);
          if (rd) { edge(termNode(rd.term).id, xn.id, 'reading', { reading: rd.reading }); continue; }
          edge(`a:${atomOf(e)}`, xn.id, 'asks', { negated: negated(e) });
          visit(atomOf(e));
        }
      }
    }
    for (const k of scheme.counts || []) {
      if (atomOf(k.as) !== a) continue;
      given = true;
      const kn = add(`k:${k.id}`, { kind: 'counts', rule: k.id, text: `counts as ${a}`, provisions: provisionsOf(scheme, k.id), applied: steps.has(k.id), holds: steps.get(k.id) === true, reading: k.reading, term: k.term });
      edge(kn.id, node.id, 'gives');
      edge(termNode(k.term).id, kn.id, 'reading', { reading: k.reading });
      edge(`a:${atomOf(k.fact)}`, kn.id, 'asks');
      visit(atomOf(k.fact));
    }
    node.role = given ? 'consequence' : 'fact';
  };
  for (const i of issues) {
    visit(atomOf(i.question));
    const n = nodes.get(`a:${atomOf(i.question)}`);
    const a = answers.get(i.id);
    n.role = 'issue';
    n.issue = i.id;
    n.burden = i.burden || '';
    n.cls = a ? a.class : null;
    n.reason = a && a.verdict ? a.verdict.reason : '';
    n.label = a ? a.label || [] : [];
  }
  // Edges whose ends both stand.
  return { nodes: [...nodes.values()], edges: edges.filter((e) => nodes.has(e.from) && nodes.has(e.to)) };
}

// The size a node is drawn at.
export function nodeSize(n) {
  if (n.kind === 'term') {
    const w = Math.max(textWidth(n.term, 13) + 40, ...n.readings.map((r) => textWidth(r.name, 12) + 44), 120);
    return { w: Math.min(w, 240), h: 34 + 18 * n.readings.length };
  }
  if (n.kind === 'counts') return { w: Math.min(Math.max(textWidth(n.text, 11) + 24, 96), 200), h: 30 };
  if (n.kind === 'rule' || n.kind === 'exception') {
    const label = n.provisions.length ? n.provisions.map((p) => p.label).join(', ') : 'no provision cited';
    return { w: Math.min(Math.max(textWidth(n.text, 13) + 28, Math.min(textWidth(label, 11) + 28, 190), 124), 230), h: 48 };
  }
  if (n.role === 'issue') return { w: Math.min(Math.max(textWidth(n.text, 16) + 36, 260), 460), h: 64 };
  return { w: Math.min(Math.max(textWidth(n.text, 11) * 0.55 + 30, textWidth(n.atom, 13) + 28, 128), n.role === 'fact' ? 176 : 230), h: n.role === 'fact' ? 62 : 40 };
}

// Layers from the questions down: each node one layer beneath the lowest node it bears on, so that a fact stands under the rules
// that ask for it (options.ground puts every fact and term on the last layer instead, which reads well for a small matter and runs
// wide for a large one). Within a layer, the order that brings each node near the nodes it is joined to (barycentres, swept down and
// up), then positions without overlap.
export function layoutMatter(graph, options = {}) {
  const gap = options.gap ?? 16, rowGap = options.rowGap ?? 44, pad = options.pad ?? 20;
  const ids = graph.nodes.map((n) => n.id);
  const byId = new Map(graph.nodes.map((n) => [n.id, n]));
  const up = new Map(ids.map((i) => [i, []])), down = new Map(ids.map((i) => [i, []]));
  for (const e of graph.edges) up.get(e.from).push(e.to), down.get(e.to).push(e.from);
  const layer = new Map();
  for (const n of graph.nodes) if (n.role === 'issue') layer.set(n.id, 0);
  for (let pass = 0; pass < ids.length + 1; pass++) {
    let moved = false;
    for (const e of graph.edges) {
      if (!layer.has(e.to)) continue;
      const want = layer.get(e.to) + 1;
      if (!layer.has(e.from) || layer.get(e.from) < want) layer.set(e.from, want), moved = true;
    }
    if (!moved) break;
  }
  for (const n of graph.nodes) if (!layer.has(n.id)) layer.set(n.id, 0);
  const deepest = Math.max(0, ...[...layer.values()]);
  if (options.ground) for (const n of graph.nodes) if (n.kind === 'term' || n.role === 'fact') layer.set(n.id, Math.max(deepest, 1));
  const rows = [];
  for (const n of graph.nodes) (rows[layer.get(n.id)] ||= []).push(n.id);
  for (let r = 0; r < rows.length; r++) rows[r] ||= [];
  const order = new Map();
  const index = () => rows.forEach((row) => row.forEach((id, k) => order.set(id, k)));
  index();
  const bary = (id, near) => {
    const xs = near.get(id).filter((x) => order.has(x)).map((x) => order.get(x));
    return xs.length ? xs.reduce((a, b) => a + b, 0) / xs.length : order.get(id);
  };
  for (let sweep = 0; sweep < 6; sweep++) {
    const downward = sweep % 2 === 0;
    const range = downward ? [...rows.keys()].slice(1) : [...rows.keys()].reverse().slice(1);
    for (const r of range) {
      rows[r].sort((a, b) => bary(a, downward ? up : down) - bary(b, downward ? up : down) || a.localeCompare(b));
      index();
    }
  }
  const sizes = new Map(graph.nodes.map((n) => [n.id, nodeSize(n)]));
  const widths = rows.map((row) => row.reduce((s, id) => s + sizes.get(id).w, 0) + gap * Math.max(0, row.length - 1));
  const width = Math.max(320, ...widths) + 2 * pad;
  const at = new Map();
  let y = pad;
  rows.forEach((row, r) => {
    const h = Math.max(0, ...row.map((id) => sizes.get(id).h));
    let x = pad + (width - 2 * pad - widths[r]) / 2;
    for (const id of row) {
      const s = sizes.get(id);
      at.set(id, { x, y: y + (h - s.h) / 2, w: s.w, h: s.h, layer: r });
      x += s.w + gap;
    }
    y += h + rowGap;
  });
  return { at, width, height: y - rowGap + pad, rows: rows.length, byId };
}

// A text cut to a width, with an ellipsis where it is cut.
const fit = (text, width, size) => {
  let t = String(text);
  if (textWidth(t, size) <= width) return t;
  while (t.length > 1 && textWidth(`${t}…`, size) > width) t = t.slice(0, -1);
  return `${t.trimEnd()}…`;
};

// The matter as markup. options.selected names a node drawn as chosen.
export function svgMatter(graph, layout = layoutMatter(graph), options = {}) {
  const { at } = layout;
  const parts = [];
  const curve = (a, b) => {
    const x1 = a.x + a.w / 2, y1 = a.y, x2 = b.x + b.w / 2, y2 = b.y + b.h;
    const m = (y1 + y2) / 2;
    return `M${x1.toFixed(1)},${y1.toFixed(1)} C${x1.toFixed(1)},${m.toFixed(1)} ${x2.toFixed(1)},${m.toFixed(1)} ${x2.toFixed(1)},${y2.toFixed(1)}`;
  };
  for (const e of graph.edges) {
    const a = at.get(e.from), b = at.get(e.to);
    if (!a || !b) continue;
    const cls = ['link', e.kind, e.negated ? 'negated' : ''].filter(Boolean).join(' ');
    parts.push(`<path class="${cls}" d="${curve(a, b)}"><title>${esc(e.kind === 'defeats' ? 'an exception: when it applies, the rule does not' : e.kind === 'reading' ? `under the reading ${e.reading}` : e.negated ? 'asked not to hold' : e.kind === 'gives' ? 'gives' : 'asked for')}</title></path>`);
    if (e.kind === 'reading' || e.negated) {
      const mx = (a.x + a.w / 2 + b.x + b.w / 2) / 2, my = (a.y + b.y + b.h) / 2;
      const word = e.kind === 'reading' ? e.reading : 'not';
      parts.push(`<g class="tag ${e.kind}"><rect x="${(mx - textWidth(word, 10.5) / 2 - 5).toFixed(1)}" y="${(my - 8).toFixed(1)}" width="${(textWidth(word, 10.5) + 10).toFixed(1)}" height="15" rx="1"/><text x="${mx.toFixed(1)}" y="${(my + 3.5).toFixed(1)}">${esc(word)}</text></g>`);
    }
  }
  for (const n of graph.nodes) {
    const p = at.get(n.id);
    if (!p) continue;
    const sel = options.selected === n.id ? ' chosen' : '';
    const g = [];
    if (n.role === 'issue') {
      const c = CLASSES[n.cls] || null;
      g.push(`<rect class="slip" width="${p.w}" height="${p.h}" rx="2"/>`);
      const title = fit(n.text, p.w - 28, 16);
      g.push(`<text class="kicker" x="14" y="18">THE QUESTION${n.burden ? ` · BURDEN: ${esc(n.burden.toUpperCase())}` : ''}</text>`);
      g.push(`<text class="title" x="14" y="40">${esc(title)}</text>`);
      if (c) g.push(`<text class="class ${esc(n.cls.split(' ')[0])}" x="${p.w - 12}" y="18" text-anchor="end">${c.glyph} ${esc(c.word)}</text>`);
      g.push(`<text class="small" x="14" y="56">${esc(n.atom)}</text>`);
      parts.push(`<g class="node issue${n.cls ? ` ${n.cls.split(' ')[0]}` : ''}${sel}" data-node="${esc(n.id)}" transform="translate(${p.x.toFixed(1)},${p.y.toFixed(1)})"><title>${esc(n.reason || n.text)}</title>${g.join('')}</g>`);
      continue;
    }
    if (n.kind === 'rule' || n.kind === 'exception') {
      const label = n.provisions.length ? n.provisions.map((q) => `${q.instrument ? `${q.instrument} ` : ''}${q.label}`).join(', ') : 'no provision cited';
      g.push(`<rect class="slip" width="${p.w}" height="${p.h}" rx="1"/>`);
      g.push(`<text class="kicker" x="12" y="17">${n.kind === 'exception' ? 'EXCEPTION' : 'RULE'}${n.applied ? ' · APPLIES' : ''}</text>`);
      g.push(`<text class="title" x="12" y="33">${esc(n.text)}</text>`);
      g.push(`<text class="cite" x="12" y="44">${esc(fit(label, p.w - 22, 11))}</text>`);
      parts.push(`<g class="node ${n.kind}${n.applied ? ' applied' : ''}${sel}" data-node="${esc(n.id)}" transform="translate(${p.x.toFixed(1)},${p.y.toFixed(1)})"><title>${esc(`${n.text}: ${label}`)}</title>${g.join('')}</g>`);
      continue;
    }
    if (n.kind === 'counts') {
      g.push(`<rect class="slip" width="${p.w}" height="${p.h}" rx="12"/>`);
      g.push(`<text class="title" x="${p.w / 2}" y="19" text-anchor="middle">${esc(n.text)}</text>`);
      parts.push(`<g class="node counts${n.applied ? ' applied' : ''}${sel}" data-node="${esc(n.id)}" transform="translate(${p.x.toFixed(1)},${p.y.toFixed(1)})"><title>${esc(`${n.rule}: under ${n.term} read ${n.reading}`)}</title>${g.join('')}</g>`);
      continue;
    }
    if (n.kind === 'term') {
      g.push(`<rect class="slip" width="${p.w}" height="${p.h}" rx="1"/>`);
      g.push(`<text class="kicker" x="12" y="16">OPEN TERM</text>`);
      g.push(`<text class="title" x="12" y="30">${esc(n.term)}</text>`);
      n.readings.forEach((r, k) => {
        const mark = r.fixed ? '◆' : r.aside ? '✕' : '◇';
        const word = r.fixed ? ' fixed' : r.aside ? ' set aside' : '';
        g.push(`<text class="reading${r.fixed ? ' fixed' : ''}${r.aside ? ' aside' : ''}" x="14" y="${48 + 18 * k}">${mark} ${esc(r.name)}<tspan class="small">${esc(word)}</tspan></text>`);
      });
      const binding = n.holdings.filter((h) => h.force === 'binding').length, persuasive = n.holdings.filter((h) => h.force === 'persuasive').length;
      if (binding + persuasive) g.push(`<text class="small" x="${p.w - 10}" y="16" text-anchor="end">${binding ? `${binding} binding` : ''}${binding && persuasive ? ' · ' : ''}${persuasive ? `${persuasive} persuasive` : ''}</text>`);
      parts.push(`<g class="node term${sel}" data-node="${esc(n.id)}" transform="translate(${p.x.toFixed(1)},${p.y.toFixed(1)})"><title>${esc(n.text)}</title>${g.join('')}</g>`);
      continue;
    }
    // A fact, or a consequence that rules give.
    const standing = n.used || n.declared;
    const glyph = n.role === 'fact' ? (n.place === 'open' && (!standing || standing === 'alleged') ? '◌' : STANDINGS[standing] || '○') : '';
    const status = n.role === 'fact' ? (n.declared === null ? 'not in the record' : n.used && n.used !== n.declared ? `${n.declared} → ${n.used}` : standing || '') : 'given by a rule';
    g.push(`<rect class="slip" width="${p.w}" height="${p.h}" rx="1"/>`);
    g.push(`<text class="kicker" x="12" y="16">${n.role === 'fact' ? `${glyph} ${esc(status.toUpperCase())}` : 'CONSEQUENCE'}</text>`);
    g.push(`<text class="title" x="12" y="${n.role === 'fact' ? 31 : 30}">${esc(fit(n.atom, p.w - 20, 13))}</text>`);
    if (n.role === 'fact') {
      if (n.text && n.text !== n.atom) g.push(`<text class="small" x="12" y="45">${esc(fit(n.text, p.w - 22, 11))}</text>`);
      if (n.evidence) g.push(`<text class="evidence ${esc(n.evidence)}" x="12" y="${p.h - 7}">evidence ${esc(n.evidence)}${n.place ? ` · ${esc(PLACES[n.place])}` : ''}</text>`);
      else if (n.place) g.push(`<text class="evidence" x="12" y="${p.h - 7}">${esc(PLACES[n.place])}</text>`);
    }
    parts.push(`<g class="node atom ${n.role}${n.place ? ` ${n.place}` : ''}${sel}" data-node="${esc(n.id)}" transform="translate(${p.x.toFixed(1)},${p.y.toFixed(1)})"><title>${esc(n.text)}</title>${g.join('')}</g>`);
  }
  return `<svg class="matter" viewBox="0 0 ${layout.width.toFixed(0)} ${layout.height.toFixed(0)}" width="${layout.width.toFixed(0)}" height="${layout.height.toFixed(0)}" role="img" aria-label="the matter as its argument">${parts.join('')}</svg>`;
}
