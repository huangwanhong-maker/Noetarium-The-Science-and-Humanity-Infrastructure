// A judgement drawn (ADR 0136), as markup that needs no page: the options a round could have been, ordered by dominance under the
// declared principles together, and what is at stake for each entity.
//
// Dominance under several principles is a partial order, so the options are drawn as its Hasse diagram: the undominated at the top,
// each option at the length of the longest chain of options above it, with a line for each covering pair only. Options on one level
// are not ranked against each other; the pivot between two undominated options says which principles favour which. Beside each
// option, the principles under which it is best. Nothing is summed.
//
// It takes what the API's allocation engine answers to `judge` (api/cpp/science/allocation.hpp).

import { esc, textWidth } from '../history/cone.js';
import { amount } from './channel.js';

// The covering pairs (a above b with nothing between) of a strict order given as dominating pairs [a, b].
export function covering(n, pairs) {
  const above = Array.from({ length: n }, () => new Set());
  for (const [a, b] of pairs) above[b].add(a);
  const cover = [];
  for (const [a, b] of pairs) {
    let between = false;
    for (const c of above[b]) if (c !== a && above[c].has(a)) { between = true; break; }
    if (!between) cover.push([a, b]);
  }
  return cover;
}

// Each option's level: the length of the longest chain of options above it (the undominated at 0).
export function levels(n, pairs) {
  const above = Array.from({ length: n }, () => []);
  for (const [a, b] of pairs) above[b].push(a);
  const memo = new Array(n).fill(-1);
  const depth = (k, seen = new Set()) => {
    if (memo[k] >= 0) return memo[k];
    if (seen.has(k)) throw new Error('the dominance given is not an order');
    seen.add(k);
    memo[k] = above[k].length ? 1 + Math.max(...above[k].map((a) => depth(a, seen))) : 0;
    seen.delete(k);
    return memo[k];
  };
  return Array.from({ length: n }, (_, k) => depth(k));
}

function label(option, k) {
  const first = option.names[0] || '';
  return k === 0 ? 'as declared' : first.replace(/^rules: /, '').replace(/^order: /, '');
}

// What a slip says beneath its name: the principles it is best under, in lines of at most `width` px.
function bestLines(judgement, option, k, width) {
  const best = judgement.principles.filter((_, j) => (judgement.best_under[j] || []).includes(k));
  if (!best.length) return [option.undominated ? 'undominated' : 'dominated'];
  const words = `best under ${best.join(', ')}`.split(' ');
  const lines = [];
  let line = '';
  for (const w of words) {
    const next = line ? `${line} ${w}` : w;
    if (line && textWidth(next, 10.5) > width) { lines.push(line); line = w; } else line = next;
  }
  if (line) lines.push(line);
  return lines;
}

export function layoutOrder(judgement, options = {}) {
  const n = judgement.options.length;
  const pairs = judgement.dominance || [];
  const lv = levels(n, pairs);
  const rows = [];
  lv.forEach((l, k) => { (rows[l] ||= []).push(k); });
  const rowGap = 46, pad = 16, maxW = options.maxW ?? 320;
  const widths = judgement.options.map((o, k) => Math.min(maxW, Math.max(130, textWidth(label(o, k), 12) * 1.1 + 30)));
  const lines = judgement.options.map((o, k) => bestLines(judgement, o, k, widths[k] - 20));
  const boxH = 30 + 14 * Math.max(...lines.map((l) => l.length));
  const rowW = rows.map((r) => r.reduce((s, k) => s + widths[k] + 18, -18));
  const width = Math.max(420, ...rowW) + 2 * pad;
  const at = new Map();
  rows.forEach((r, l) => {
    let x = (width - rowW[l]) / 2;
    for (const k of r) {
      at.set(k, { x, y: pad + 40 + l * (boxH + rowGap), w: widths[k], h: boxH, level: l, lines: lines[k] });
      x += widths[k] + 18;
    }
  });
  return { at, rows, cover: covering(n, pairs), width, height: pad + 40 + rows.length * (boxH + rowGap) - rowGap + pad + 10 };
}

export function svgOrder(judgement, options = {}) {
  const L = layoutOrder(judgement, options);
  const parts = [`<text class="head" x="16" y="18">The options, ordered by dominance</text>`, `<text class="sub" x="16" y="34">under ${esc(judgement.principles.join('; '))}</text>`];
  for (const [a, b] of L.cover) {
    const A = L.at.get(a), B = L.at.get(b);
    parts.push(`<line class="covers" x1="${(A.x + A.w / 2).toFixed(1)}" y1="${A.y + A.h}" x2="${(B.x + B.w / 2).toFixed(1)}" y2="${B.y}"/>`);
  }
  judgement.options.forEach((o, k) => {
    const p = L.at.get(k);
    const cls = `option${o.undominated ? ' undominated' : ' dominated'}${k === 0 ? ' declared' : ''}${options.selected === k ? ' chosen' : ''}`;
    parts.push(`<g class="${cls}" data-option="${k}"><rect class="slip" x="${p.x.toFixed(1)}" y="${p.y}" width="${p.w.toFixed(1)}" height="${p.h}" rx="2"/>` +
      `<text class="name" x="${(p.x + 10).toFixed(1)}" y="${p.y + 18}">${esc(label(o, k))}</text>` +
      p.lines.map((t, i) => `<text class="best" x="${(p.x + 10).toFixed(1)}" y="${p.y + 34 + 14 * i}">${esc(t)}</text>`).join('') +
      `<title>${esc(o.names.join('; '))}</title></g>`);
  });
  return `<svg class="allocation-order" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${L.width} ${L.height}" width="${L.width}" height="${L.height}" role="img" aria-label="the options ordered by dominance">${parts.join('')}</svg>`;
}

// What is at stake for each entity among the undominated options: a bar from the least to the most it would receive, the declared
// round's amount marked; the entities with the widest stakes first.
export function svgStakes(judgement, round, options = {}) {
  const names = new Map(round.entities.map((e) => [e.id, e.name || e.id]));
  const rows = [...judgement.stakes].sort((a, b) => (b.most - b.least) - (a.most - a.least)).slice(0, options.most ?? 20);
  if (!rows.length) return '';
  const unit = round.unit || '';
  const labelW = Math.min(200, Math.max(90, ...rows.map((r) => textWidth(names.get(r.entity), 12) + 12))), plotW = options.plotW ?? 320, rowH = 22, top = 28;
  const hi = Math.max(1e-12, ...rows.map((r) => Math.max(r.most, r.declared)));
  const x = (v) => labelW + (plotW * v) / hi;
  const valueW = Math.max(150, ...rows.map((r) => textWidth(`${amount(r.least, unit)} – ${amount(r.most, unit)} (${amount(r.declared, unit)})`, 11.5) + 16));
  const W = labelW + plotW + valueW, H = top + rows.length * rowH + 12;
  const parts = [`<text class="head" x="0" y="14">At stake among the undominated</text><text class="head" x="${W}" y="14" text-anchor="end">least – most (declared)</text>`];
  rows.forEach((r, k) => {
    const y = top + k * rowH + rowH / 2;
    parts.push(`<g class="stake" data-entity="${esc(r.entity)}"><text class="name" x="${labelW - 8}" y="${y + 4}" text-anchor="end">${esc(names.get(r.entity))}</text>` +
      `<line class="range" x1="${x(r.least).toFixed(1)}" y1="${y}" x2="${x(r.most).toFixed(1)}" y2="${y}"/>` +
      `<circle class="end" cx="${x(r.least).toFixed(1)}" cy="${y}" r="3"/><circle class="end" cx="${x(r.most).toFixed(1)}" cy="${y}" r="3"/>` +
      `<path class="declared" d="M${x(r.declared).toFixed(1)},${y - 6} l4,6 l-4,6 l-4,-6 Z"/>` +
      `<text class="value" x="${W}" y="${y + 4}" text-anchor="end">${esc(amount(r.least, unit))} – ${esc(amount(r.most, unit))} (${esc(amount(r.declared, unit))})</text></g>`);
  });
  return `<svg class="allocation-stakes" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="what is at stake for each entity">${parts.join('')}</svg>`;
}
