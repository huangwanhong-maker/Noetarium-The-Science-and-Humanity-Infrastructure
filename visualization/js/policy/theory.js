// A theory of change drawn (ADR 0122): from the intervention at the left to the outcome at the right, each causal claim a link that
// says how far it is shown.
//
// It takes the theory as the API's policy engine answers it (api/cpp/science/policy.hpp: nodes; links with their assumption, support
// factors, evidence, standing and the body of evidence set on them) and the assessment of the question (the strongest path, the link
// that limits it, the links every path passes through and the gaps among them). A link is drawn by its standing: a full line when it
// is shown, with its grade in words and as pips of the scale; dashed when it is not shown; the strongest path heavier; the weakest
// link of that path and a gap on every path each named in words. A standing is never told by the line alone.
//
// layoutTheory is pure arithmetic; svgTheory returns markup (policy.css gives it a default look).

import { esc, textWidth } from '../history/cone.js';

// A text broken into lines no wider than a width.
export function wrapText(text, width, size = 13) {
  const words = String(text ?? '').split(/\s+/).filter(Boolean), lines = [];
  let line = '';
  for (const w of words) {
    const t = line ? `${line} ${w}` : w;
    if (line && textWidth(t, size) > width) { lines.push(line); line = w; } else line = t;
  }
  if (line) lines.push(line);
  return lines;
}

// The pips of a grade on a scale of n levels: filled up to the level, hollow above.
export function pips(rank, n) {
  if (rank === null || rank === undefined || rank < 0) return '○'.repeat(n);
  return '●'.repeat(rank + 1) + '○'.repeat(Math.max(0, n - rank - 1));
}

// Layers from the nodes nothing leads to, each node one layer after the latest node that leads to it; a claim that passes over a
// layer is given a place in it, so that it is drawn around what stands there and not behind it; within a layer, the order that
// keeps each thing near what it is joined to. options.room: the width there is, to which the nodes' width is fitted.
export function layoutTheory(theory, assessment = null, options = {}) {
  const gapX = options.gapX ?? 132, gapY = options.gapY ?? 26, pad = options.pad ?? 22;
  const ids = theory.nodes.map((n) => n.id);
  const index = new Map(ids.map((id, i) => [id, i]));
  const links = theory.links.map((l, i) => ({ ...l, i })).filter((l) => index.has(l.from) && index.has(l.to) && l.from !== l.to);
  const layer = new Array(ids.length).fill(0);
  for (let pass = 0; pass < ids.length; pass++) {
    let moved = false;
    for (const l of links) {
      const a = index.get(l.from), b = index.get(l.to);
      if (layer[b] < layer[a] + 1) { layer[b] = layer[a] + 1; moved = true; }
    }
    if (!moved) break;
  }
  const count = Math.max(1, ...layer.map((x) => x + 1));
  let nodeW = options.nodeW ?? 196;
  if (options.room) nodeW = Math.max(150, Math.min(250, Math.floor((options.room - 2 * pad - (count - 1) * gapX) / count)));

  // The things to place: the nodes, then a waypoint in each layer a claim passes over.
  const items = ids.map((id, i) => ({ node: i, layer: layer[i] }));
  const chains = links.map((l) => {
    const a = index.get(l.from), b = index.get(l.to), chain = [a];
    for (let L = layer[a] + 1; L < layer[b]; L++) { items.push({ way: l.i, layer: L }); chain.push(items.length - 1); }
    chain.push(b);
    return { link: l.i, chain };
  });
  const before = items.map(() => []), after = items.map(() => []);
  for (const c of chains) for (let k = 0; k + 1 < c.chain.length; k++) { after[c.chain[k]].push(c.chain[k + 1]); before[c.chain[k + 1]].push(c.chain[k]); }
  const columns = Array.from({ length: count }, () => []);
  items.forEach((it, i) => columns[it.layer].push(i));
  const place = new Map();
  const reindex = () => columns.forEach((col) => col.forEach((i, k) => place.set(i, k)));
  reindex();
  const near = (i, side) => {
    const xs = (side === 'in' ? before[i] : after[i]).map((j) => place.get(j));
    return xs.length ? xs.reduce((a, b) => a + b, 0) / xs.length : place.get(i);
  };
  for (let sweep = 0; sweep < 6; sweep++) {
    const forward = sweep % 2 === 0;
    for (const c of forward ? columns.keys() : [...columns.keys()].reverse()) {
      columns[c].sort((a, b) => near(a, forward ? 'in' : 'out') - near(b, forward ? 'in' : 'out') || a - b);
      reindex();
    }
  }
  const lines = theory.nodes.map((n) => {
    const all = wrapText(n.text || n.id, nodeW - 26, 13);
    if (all.length <= 4) return all;
    let last = all[3].replace(/[\s,;:.]+$/, '');
    while (last.includes(' ') && textWidth(`${last}…`, 13) > nodeW - 26) last = last.slice(0, last.lastIndexOf(' '));
    return [...all.slice(0, 3), `${last}…`];
  });
  const height = (i) => (items[i].node === undefined ? 38 : 40 + 17 * Math.max(1, lines[items[i].node].length));
  const colH = columns.map((col) => col.reduce((s, i) => s + height(i), 0) + gapY * Math.max(0, col.length - 1));
  const H = Math.max(options.minHeight ?? 150, ...colH) + 2 * pad;
  const spot = items.map(() => null);
  columns.forEach((col, c) => {
    let y = pad + (H - 2 * pad - colH[c]) / 2;
    for (const i of col) {
      spot[i] = { x: pad + c * (nodeW + gapX), y, w: nodeW, h: height(i), layer: c };
      y += height(i) + gapY;
    }
  });
  const at = new Map(ids.map((id, i) => [id, { ...spot[i], lines: lines[i] }]));
  const strongest = new Set((assessment?.strongest?.links) || []);
  const necessary = new Set(assessment?.necessary || []), gaps = new Set(assessment?.gaps || []);
  const f = (v) => v.toFixed(1);
  const drawn = chains.map(({ link, chain }) => {
    const l = theory.links[link], a = spot[chain[0]], b = spot[chain.at(-1)];
    const x1 = a.x + a.w, y1 = a.y + a.h / 2, x2 = b.x, y2 = b.y + b.h / 2;
    let px = x1, py = y1, path = `M${f(x1)},${f(y1)}`, mx = (x1 + x2) / 2, my = (y1 + y2) / 2, straight = true;
    const curve = (ex, ey) => { const c = (ex - px) / 2; path += ` C${f(px + c)},${f(py)} ${f(ex - c)},${f(ey)} ${f(ex)},${f(ey)}`; px = ex; py = ey; };
    chain.slice(1, -1).forEach((v, k) => {
      const s = spot[v], y = s.y + s.h / 2;
      curve(s.x, y);
      path += ` L${f(s.x + s.w)},${f(y)}`;
      px = s.x + s.w;
      if (k === 0) { mx = s.x + s.w / 2; my = y; straight = false; }
    });
    curve(x2, y2);
    return { index: link, path, x1, y1, x2, y2, mx, my, straight, shown: l.rank >= 0, strongest: strongest.has(link), necessary: necessary.has(link), gap: gaps.has(link),
      limiting: assessment?.strongest?.limiting === link && strongest.size > 0 };
  });
  // Two badges at one height in one gap are set apart.
  const byGap = new Map();
  for (const d of drawn.filter((z) => z.straight)) { const key = Math.round(d.mx); if (!byGap.has(key)) byGap.set(key, []); byGap.get(key).push(d); }
  for (const group of byGap.values()) {
    group.sort((p, q) => p.my - q.my);
    for (let k = 1; k < group.length; k++) { const need = group[k].limiting || group[k].gap || group[k - 1].limiting || group[k - 1].gap ? 40 : 27; if (group[k].my - group[k - 1].my < need) group[k].my = group[k - 1].my + need; }
  }
  return { at, links: drawn, width: pad * 2 + count * nodeW + Math.max(0, count - 1) * gapX, height: H, levels: theory.levels || [], nodeW };
}

// The theory as markup. options.selected names what is drawn as chosen ("n:<id>" or "l:<index>").
export function svgTheory(theory, assessment = null, layout = layoutTheory(theory, assessment), options = {}) {
  const parts = [];
  const n = (theory.levels || []).length;
  for (const d of layout.links) {
    const l = theory.links[d.index];
    const cls = ['link', d.shown ? 'shown' : 'unshown', d.strongest ? 'strongest' : '', d.gap ? 'gap' : ''].filter(Boolean).join(' ');
    parts.push(`<path class="${cls}" d="${d.path}"/>`);
    parts.push(`<path class="head ${d.shown ? 'shown' : 'unshown'}" d="M${(d.x2 - 7).toFixed(1)},${(d.y2 - 4).toFixed(1)} L${d.x2.toFixed(1)},${d.y2.toFixed(1)} L${(d.x2 - 7).toFixed(1)},${(d.y2 + 4).toFixed(1)}"/>`);
    const word = d.shown ? (l.level || '').replace(/_/g, ' ') : l.evidence?.given ? 'not shown' : 'no evidence';
    const note = d.limiting ? 'the weakest link' : d.gap ? 'a gap on every path' : '';
    const w = Math.max(textWidth(`${pips(d.shown ? l.rank : -1, n)} ${word}`, 11.5), textWidth(note, 10.5)) + 18, h = note ? 34 : 21;
    const sel = options.selected === `l:${d.index}` ? ' chosen' : '';
    parts.push(`<g class="badge ${d.shown ? 'shown' : 'unshown'}${d.limiting ? ' limiting' : ''}${d.gap ? ' gap' : ''}${sel}" data-link="${d.index}" transform="translate(${(d.mx - w / 2).toFixed(1)},${(d.my - h / 2).toFixed(1)})">`
      + `<title>${esc(l.why || '')}</title><rect width="${w.toFixed(1)}" height="${h}" rx="2"/>`
      + `<text class="grade" x="${(w / 2).toFixed(1)}" y="14.5" text-anchor="middle"><tspan class="pips">${pips(d.shown ? l.rank : -1, n)}</tspan> ${esc(word)}</text>`
      + (note ? `<text class="note" x="${(w / 2).toFixed(1)}" y="28" text-anchor="middle">${esc(note)}</text>` : '') + '</g>');
  }
  for (const node of theory.nodes) {
    const p = layout.at.get(node.id);
    if (!p) continue;
    const sel = options.selected === `n:${node.id}` ? ' chosen' : '';
    const asked = node.id === options.intervention ? ' from' : node.id === options.outcome ? ' to' : '';
    const text = p.lines.map((line, k) => `<text class="title" x="13" y="${34 + 17 * k}">${esc(line)}</text>`).join('');
    parts.push(`<g class="node ${esc(String(node.kind || '').replace(/[^a-z]/gi, ''))}${asked}${sel}" data-node="${esc(node.id)}" transform="translate(${p.x.toFixed(1)},${p.y.toFixed(1)})">`
      + `<title>${esc(node.text || node.id)}</title><rect class="slip" width="${p.w}" height="${p.h}" rx="2"/>`
      + `<text class="kicker" x="13" y="17">${esc(String(node.kind || '').toUpperCase())}</text>${text}</g>`);
  }
  return `<svg class="theory" viewBox="0 0 ${layout.width.toFixed(0)} ${layout.height.toFixed(0)}" width="${layout.width.toFixed(0)}" height="${layout.height.toFixed(0)}" role="img" aria-label="the theory of change">${parts.join('')}</svg>`;
}
