// Theories of the law drawn (ADR 0133): doctrines (a reading of each open term and a ratio for each precedent whose court left its
// reason unstated) as the law domain's search finds them, judged by fit and by justification after Dworkin.
//
//   - the field: each doctrine kept placed by what it does not fit (the decided cases and binding readings it disagrees with,
//     weighted by their force, and the precedents its ratios make inconsistent) against what it is worth (the declared purposes it
//     serves), every weight the matter's; the score is the one less the other, so doctrines of equal score lie on a diagonal, and the
//     acceptable ones (within the tolerance of the best) above a line. The best doctrine of each kind there is, when every doctrine
//     was visited, is ringed, so that what the search has not reached shows;
//   - the case law: the decided cases along time, each by its force for the forum (binding, persuasive, left out) and whether a
//     doctrine agrees with it, and beyond the matter's date the decisions held back, veiled, with what the doctrine's test on them
//     found;
//   - a board of precedents: the factors of a case base by the side they favour, each precedent a row with its factors, the reason a
//     doctrine reads it as decided on ringed, the matter's own factors beneath, and the side the precedents then force.
//
// They take what the API's law engine answered (api/cpp/science/doctrine.hpp's `doctrines` and `assess_doctrine`) and ask no server;
// partsOf, fieldOf, caseLawOf and boardOf build the models and need no page, svgField, svgCaseLaw and svgBoard return markup, and
// doctrine.css gives it a look. A glyph and a word tell an outcome, a force and a verdict as well as a tone.

import { esc } from '../history/cone.js';

export const OUTCOMES = {
  holds: { glyph: '●', word: 'holds' },
  fails: { glyph: '○', word: 'fails' },
  turns: { glyph: '◐', word: 'turns on a fact left open' },
};
export const FORCES = { binding: { glyph: '■', word: 'binding' }, persuasive: { glyph: '□', word: 'persuasive' }, 'left out': { glyph: '⬚', word: 'left out' } };
export const VERDICTS = { predicted: { glyph: '✓', word: 'predicted' }, contradicted: { glyph: '✕', word: 'contradicted' }, open: { glyph: '·', word: 'left open' } };

const keyOf = (kind) => (typeof kind === 'string' ? kind : JSON.stringify(kind));
const count = (x) => (Array.isArray(x) ? x.length : Number(x) || 0);

// A doctrine's score in its two parts, with the matter's weights: what it does not fit and what it is worth (score = unfit − worth).
export function partsOf(weights, a) {
  const W = { authority: 10, cases: 1, coherence: 2, justification: 1, ...(weights || {}) };
  const authority = a.admissible === false ? W.authority : 0;
  const cases = W.cases * (Number(a.misfit) || 0);
  const coherence = W.coherence * count(a.conflicts);
  const unfit = authority + cases + coherence;
  const worth = W.justification * (Number(a.justification) || 0);
  return { authority, cases, coherence, unfit, worth, score: unfit - worth };
}

// The outcome of a question under an assessment ('holds', 'fails', 'turns'), from the engine's code (1 follows, 0 does not follow,
// 2 turns on the precedents).
const CODES = ['fails', 'holds', 'turns'];
export function outcomeOf(a, question) {
  const o = (a?.outcomes || []).find((x) => x.question === question) || (a?.outcomes || [])[0];
  return o ? CODES[o.code] || 'fails' : 'fails';
}

// The field: the doctrines kept and the exact best of each kind, on the plane of misfit and worth.
export function fieldOf(report, options = {}) {
  const P = report.problem || {};
  const W = P.weights || {};
  const question = options.question ?? (P.questions || [])[0];
  const point = (a, extra) => ({ ...extra, outcome: outcomeOf(a, question), ...partsOf(W, a) });
  const kept = (report.kept || []).map((d) => point(d.assessment, { serial: d.serial, kind: keyOf(d.kind), words: d.kind_words, text: d.text, tested: d.test?.outcome || null }));
  const exact = (report.exact?.best || []).filter((b) => b.assessment).map((b) => point(b.assessment, { kind: keyOf(b.kind), words: b.words, text: b.text }));
  for (const e of exact) e.found = kept.some((k) => k.kind === e.kind && Math.abs(k.score - e.score) < 1e-9);
  const all = [...kept, ...exact];
  const tolerance = Number.isFinite(P.tolerance) ? P.tolerance : 0.5;
  const best = all.length ? Math.min(...all.map((p) => p.score)) : 0;
  const xs = all.map((p) => p.unfit), ys = all.map((p) => p.worth);
  const pad = (lo, hi) => { const s = hi - lo || 1; return [lo - 0.08 * s, hi + 0.08 * s]; };
  return {
    question, kept, exact, best, tolerance, threshold: best + tolerance,
    x: pad(Math.min(0, ...xs), Math.max(1, ...xs)), y: pad(Math.min(0, ...ys), Math.max(1, ...ys)),
    acceptable: kept.filter((k) => k.score <= best + tolerance + 1e-12).map((k) => k.serial),
    enumerated: !!report.exact?.enumerated, size: report.exact?.size ?? P.size,
  };
}

// The part of a rectangle where worth ≥ unfit − threshold (the acceptable doctrines' half-plane), as a polygon in data coordinates.
export function acceptableRegion(model) {
  const [x0, x1] = model.x, [y0, y1] = model.y, T = model.threshold;
  const inside = ([x, y]) => y >= x - T - 1e-12;
  const corners = [[x0, y0], [x1, y0], [x1, y1], [x0, y1]];
  const out = [];
  for (let k = 0; k < 4; k++) {
    const a = corners[k], b = corners[(k + 1) % 4];
    if (inside(a)) out.push(a);
    if (inside(a) !== inside(b)) {
      // Where the edge a→b meets y = x − T.
      const fa = a[1] - a[0] + T, fb = b[1] - b[0] + T, t = fa / (fa - fb);
      out.push([a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1])]);
    }
  }
  return out;
}

const fmt = (v, d = 2) => (Number.isFinite(v) ? (Math.abs(v - Math.round(v)) < 1e-9 ? String(Math.round(v)) : v.toFixed(d)) : '—');

function ticks(lo, hi, most = 6) {
  const span = hi - lo, raw = span / most, mag = 10 ** Math.floor(Math.log10(raw || 1));
  const step = [1, 2, 2.5, 5, 10].map((m) => m * mag).find((s) => span / s <= most) || mag * 10;
  const out = [];
  for (let v = Math.ceil(lo / step) * step; v <= hi + 1e-9; v += step) out.push(Math.abs(v) < 1e-12 ? 0 : v);
  return out;
}

const glyphOf = (outcome, x, y, r) => {
  if (outcome === 'turns') return `<circle class="df-ring" cx="${x}" cy="${y}" r="${r}"/><path class="df-half" d="M${x},${y - r}A${r},${r} 0 0 0 ${x},${y + r}Z"/>`;
  return `<circle class="${outcome === 'holds' ? 'df-full' : 'df-ring'}" cx="${x}" cy="${y}" r="${r}"/>`;
};

// The field as markup: misfit to the right, worth upward, the acceptable half-plane shaded, the line of the best score, each
// doctrine kept a mark by the question's outcome under it (spread where several share a place), each kind's exact best ringed.
export function svgField(model, options = {}) {
  const width = options.width || 560, height = options.height || 360, chosen = options.chosen ?? null;
  const L = 58, R = 18, T = 18, B = 48;
  const sx = (v) => L + ((v - model.x[0]) / (model.x[1] - model.x[0])) * (width - L - R);
  const sy = (v) => height - B - ((v - model.y[0]) / (model.y[1] - model.y[0])) * (height - T - B);
  const parts = [];
  parts.push(`<rect class="df-plot" x="${L}" y="${T}" width="${width - L - R}" height="${height - T - B}"/>`);
  const region = acceptableRegion(model);
  if (region.length > 2) parts.push(`<path class="df-acceptable" d="${region.map((p, k) => `${k ? 'L' : 'M'}${sx(p[0]).toFixed(1)},${sy(p[1]).toFixed(1)}`).join('')}Z"/>`);
  for (const v of ticks(...model.x)) parts.push(`<line class="df-grid" x1="${sx(v)}" x2="${sx(v)}" y1="${T}" y2="${height - B}"/><text class="df-tick" x="${sx(v)}" y="${height - B + 15}" text-anchor="middle">${fmt(v)}</text>`);
  for (const v of ticks(...model.y)) parts.push(`<line class="df-grid" y1="${sy(v)}" y2="${sy(v)}" x1="${L}" x2="${width - R}"/><text class="df-tick" x="${L - 6}" y="${sy(v) + 4}" text-anchor="end">${fmt(v)}</text>`);
  // A line of equal score, y = x − s, clipped to the plot.
  const diagonal = (s, cls, label) => {
    const [x0, x1] = model.x, [y0, y1] = model.y;
    const ends = [];
    for (const [x, y] of [[x0, x0 - s], [x1, x1 - s], [y0 + s, y0], [y1 + s, y1]]) if (x >= x0 - 1e-9 && x <= x1 + 1e-9 && y >= y0 - 1e-9 && y <= y1 + 1e-9) ends.push([x, y]);
    if (ends.length < 2) return;
    ends.sort((a, b) => a[0] - b[0]);
    const [a, b] = [ends[0], ends[ends.length - 1]];
    parts.push(`<line class="${cls}" x1="${sx(a[0]).toFixed(1)}" y1="${sy(a[1]).toFixed(1)}" x2="${sx(b[0]).toFixed(1)}" y2="${sy(b[1]).toFixed(1)}"/>`);
    if (label) parts.push(`<text class="df-line-label" x="${(sx(b[0]) - 4).toFixed(1)}" y="${(sy(b[1]) + 13).toFixed(1)}" text-anchor="end">${esc(label)}</text>`);
  };
  diagonal(model.best, 'df-best', `Best score ${fmt(model.best)}`);
  diagonal(model.threshold, 'df-threshold', `Tolerance ${fmt(model.tolerance)}`);
  // The exact best of each kind, ringed (dashed where the search has not yet reached it).
  for (const e of model.exact) parts.push(`<circle class="df-exact${e.found ? ' found' : ''}" cx="${sx(e.unfit).toFixed(1)}" cy="${sy(e.worth).toFixed(1)}" r="10"><title>${esc(`best of its kind there is: ${e.words}; score ${fmt(e.score)}${e.found ? ' (found by the search)' : ' (not yet found)'}`)}</title></circle>`);
  // The doctrines kept, spread where several share a place.
  const groups = new Map();
  for (const k of model.kept) {
    const g = `${k.unfit.toFixed(6)},${k.worth.toFixed(6)}`;
    if (!groups.has(g)) groups.set(g, []);
    groups.get(g).push(k);
  }
  for (const members of groups.values())
    members.forEach((k, i) => {
      const n = members.length, a = (2 * Math.PI * i) / n - Math.PI / 2, d = n > 1 ? 7 + 1.2 * n : 0;
      const x = (sx(k.unfit) + d * Math.cos(a)).toFixed(1), y = (sy(k.worth) + d * Math.sin(a)).toFixed(1);
      parts.push(`<g class="df-dot o-${k.outcome}${chosen === k.serial ? ' on' : ''}${model.acceptable.includes(k.serial) ? ' acceptable' : ''}" data-serial="${k.serial}" tabindex="0" role="button" aria-label="${esc(`doctrine #${k.serial}: ${k.words}; ${model.question} ${OUTCOMES[k.outcome]?.word || k.outcome}; score ${fmt(k.score)}`)}">`
        + `${chosen === k.serial ? `<circle class="df-halo" cx="${x}" cy="${y}" r="10"/>` : ''}${glyphOf(k.outcome, Number(x), Number(y), 5.5)}<title>${esc(`#${k.serial}: ${k.words}\nmisfit ${fmt(k.unfit)}, worth ${fmt(k.worth)}, score ${fmt(k.score)}`)}</title></g>`);
    });
  parts.push(`<text class="df-axis" x="${(L + width - R) / 2}" y="${height - 10}" text-anchor="middle">Misfit (weighted by force)</text>`);
  parts.push(`<text class="df-axis" transform="translate(14 ${(T + height - B) / 2}) rotate(-90)" text-anchor="middle">Worth (declared purposes)</text>`);
  return `<svg class="doctrine-field" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" role="img" aria-label="${esc(`the doctrines kept, by misfit and worth; ${model.kept.length} kept, ${model.acceptable.length} acceptable`)}">${parts.join('')}</svg>`;
}

// The case law under a doctrine: the decided cases in order of date with their force and whether the doctrine agrees, and the
// decisions held back with what the doctrine's test found of each.
export function caseLawOf(report, doctrine) {
  const P = report.problem || {};
  const fits = new Map((doctrine?.assessment?.cases || []).map((c) => [c.id, c]));
  const tests = new Map((doctrine?.test?.cases || []).map((c) => [c.id, c]));
  const rows = (P.decided || []).map((d) => ({
    id: d.id, name: d.name || d.id, court: d.court, date: d.date, question: d.question, held: d.held, force: d.force, weight: d.weight, why: d.why || '',
    passage: d.passage || '', facts: d.facts || [], agrees: fits.has(d.id) ? !!fits.get(d.id).agrees : null, reason: fits.get(d.id)?.reason || '', held_back: false,
  }));
  for (const h of P.held_back || [])
    if (!String(h.id).includes('/'))
      rows.push({ id: h.id, name: h.name || h.id, court: h.court, date: h.date, held_back: true, verdict: tests.get(h.id)?.verdict || null, reason: tests.get(h.id)?.reason || '' });
  rows.sort((a, b) => String(a.date || '').localeCompare(String(b.date || '')) || String(a.id).localeCompare(String(b.id)));
  const agree = rows.filter((r) => r.agrees === true).length, disagree = rows.filter((r) => r.agrees === false).length;
  return { date: P.date || '', rows, agree, disagree };
}

const yearOf = (date) => {
  const m = /^(\d{4})(?:-(\d{2}))?(?:-(\d{2}))?/.exec(String(date || ''));
  return m ? Number(m[1]) + ((Number(m[2] || 1) - 1) * 30.44 + (Number(m[3] || 1) - 1)) / 365.25 : NaN;
};

// The case law as markup: a line of years, each case a stone (filled when binding, a ring when persuasive, dotted when left out)
// with its agreement above it, the matter's date as a rule, and the decisions held back veiled beyond it.
export function svgCaseLaw(model, options = {}) {
  const width = options.width || 640, height = options.height || 150, chosen = options.chosen ?? null;
  const L = 24, R = 24, axis = 92;
  const years = [...model.rows.map((r) => yearOf(r.date)), yearOf(model.date)].filter(Number.isFinite);
  const lo = years.length ? Math.min(...years) : 2000, hi = years.length ? Math.max(...years) : 2001;
  const y0 = Math.floor(lo) - 1, y1 = Math.max(Math.ceil(hi) + 1, y0 + 3);
  const sx = (y) => L + ((y - y0) / (y1 - y0)) * (width - L - R);
  const parts = [`<line class="dl-axis" x1="${L}" x2="${width - R}" y1="${axis}" y2="${axis}"/>`];
  const step = Math.max(1, Math.ceil((y1 - y0) / 10));
  for (let y = Math.ceil(y0 / step) * step; y <= y1; y += step) parts.push(`<line class="dl-tick" x1="${sx(y)}" x2="${sx(y)}" y1="${axis}" y2="${axis + 4}"/><text class="dl-year" x="${sx(y)}" y="${axis + 16}" text-anchor="middle">${y}</text>`);
  const md = yearOf(model.date);
  if (Number.isFinite(md)) {
    parts.push(`<rect class="dl-veil" x="${sx(md).toFixed(1)}" y="8" width="${(width - R - sx(md)).toFixed(1)}" height="${axis + 22}"/>`);
    parts.push(`<line class="dl-matter" x1="${sx(md).toFixed(1)}" x2="${sx(md).toFixed(1)}" y1="8" y2="${axis + 22}"/><text class="dl-matter-label" x="${(sx(md) + 4).toFixed(1)}" y="18">The matter, ${esc(model.date)}</text>`);
  }
  // Stones of one year are lifted so that they do not overlap.
  const placed = [];
  model.rows.forEach((r) => {
    const yr = yearOf(r.date);
    if (!Number.isFinite(yr)) return;
    const x = sx(yr);
    let lift = 0;
    while (placed.some((p) => Math.abs(p.x - x) < 15 && p.lift === lift)) lift++;
    placed.push({ x, lift });
    const cy = axis - 14 - 26 * lift, rad = r.held_back ? 6 : 4 + 4 * Math.min(1, Number(r.weight) || 0);
    const cls = r.held_back ? 'held-back' : `f-${String(r.force || 'left out').replace(/\s+/g, '-')}`;
    const mark = r.held_back ? (r.verdict ? VERDICTS[r.verdict]?.glyph || '·' : '?') : r.agrees === true ? '✓' : r.agrees === false ? '✕' : '';
    const markCls = r.held_back ? `v-${r.verdict || 'untested'}` : r.agrees === true ? 'agrees' : r.agrees === false ? 'disagrees' : '';
    const words = r.held_back
      ? `${r.name} (${r.date}), held back${r.verdict ? `: ${VERDICTS[r.verdict]?.word || r.verdict} — ${r.reason}` : ': not yet tested'}`
      : `${r.name} (${r.court}, ${r.date}), ${FORCES[r.force]?.word || r.force}: ${r.question} ${r.held ? 'held' : 'not held'}${r.agrees === null ? '' : r.agrees ? '; the doctrine agrees' : '; the doctrine disagrees'}${r.reason ? ` — ${r.reason}` : ''}`;
    parts.push(`<g class="dl-case ${cls}${chosen === r.id ? ' on' : ''}" data-case="${esc(r.id)}" tabindex="0" role="button" aria-label="${esc(words)}">`
      + `<line class="dl-stem" x1="${x.toFixed(1)}" x2="${x.toFixed(1)}" y1="${cy + rad}" y2="${axis}"/>`
      + `<circle class="dl-stone" cx="${x.toFixed(1)}" cy="${cy}" r="${rad.toFixed(1)}"/>`
      + (mark ? `<text class="dl-mark ${markCls}" x="${(x + rad + 3).toFixed(1)}" y="${cy + 4}">${mark}</text>` : '')
      + `<text class="dl-name" x="${x.toFixed(1)}" y="${axis + 30 + 11 * (placed.length % 2)}" text-anchor="middle">${esc(r.name.replace(/^(Council|Borough) v /, ''))}</text>`
      + `<title>${esc(words)}</title></g>`);
  });
  return `<svg class="doctrine-caselaw" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" role="img" aria-label="${esc(`the decided cases along time: ${model.agree} agree with the doctrine, ${model.disagree} do not`)}">${parts.join('')}</svg>`;
}

// A board of a case base under a doctrine: the factors by side, each precedent's factors and the reason read, the matter's factors,
// the side forced, and the precedents held back with the test's verdict.
export function boardOf(report, doctrine, k = 0) {
  const P = report.problem || {}, B = (P.bases || [])[k];
  if (!B) return null;
  const sides = B.sides || ['holds', 'fails'];
  const factors = [...(B.factors || [])].sort((a, b) => (a.side === sides[0] ? 0 : 1) - (b.side === sides[0] ? 0 : 1) || String(a.id).localeCompare(String(b.id)));
  const ratio = new Map((doctrine?.ratios || []).filter((r) => r.fact === B.fact).map((r) => [r.case, r]));
  const rows = (B.cases || []).map((c) => ({
    name: c.name, outcome: c.outcome, facts: [...(c.facts || [])], stated: !!c.stated,
    reason: ratio.has(c.name) ? [...ratio.get(c.name).factors] : [...(c.reason || [])], chosen: ratio.has(c.name) ? !!ratio.get(c.name).chosen : false,
  }));
  const forced = (doctrine?.assessment?.bases || [])[k] || null;
  const tests = new Map((doctrine?.test?.cases || []).map((c) => [c.id, c]));
  const held = (P.held_back || []).filter((h) => String(h.id).startsWith(`${B.fact}/`)).map((h) => ({ name: h.name, verdict: tests.get(h.id)?.verdict || null, reason: tests.get(h.id)?.reason || '' }));
  return { fact: B.fact, sides, factors, rows, matter: { facts: [...(B.situation || [])], forced: forced?.forced || '', why: forced?.why || '' }, held };
}

// The board as markup: a column for each factor (those for the first side, then those for the second), a row for each precedent and
// one for the matter; a pip where a case has a factor, ringed where the doctrine reads it as the case's reason; the side decided at
// the row's end; the side the precedents force for the matter, or that they leave it open.
export function svgBoard(board, options = {}) {
  const col = options.column || 34, row = options.row || 26, left = options.left || 112, right = options.right || 150, head = options.head || 124;
  const n = board.factors.length, width = left + n * col + right;
  const rows = [...board.rows.map((r) => ({ ...r, kind: 'case' })), ...board.held.map((h) => ({ ...h, kind: 'held' })), { kind: 'matter', name: 'The matter', facts: board.matter.facts }];
  const height = head + rows.length * row + 10;
  const parts = [];
  const split = board.factors.findIndex((f) => f.side !== board.sides[0]);
  const first = split < 0 ? n : split;
  // The two sides' headers.
  const bracket = (from, to, words, cls) => {
    if (to <= from) return;
    const x0 = left + from * col + 3, x1 = left + to * col - 3;
    parts.push(`<path class="db-bracket ${cls}" d="M${x0},${head - 4}V${head - 8}H${x1}V${head - 4}"/><text class="db-side ${cls}" x="${(x0 + x1) / 2}" y="${head - 12}" text-anchor="middle">${esc(words)}</text>`);
  };
  board.factors.forEach((f, i) => {
    const x = left + i * col + col / 2;
    parts.push(`<text class="db-factor ${i < first ? 's0' : 's1'}" transform="translate(${x + 4} ${head - 26}) rotate(-38)">${esc(f.id.replace(/_/g, ' '))}<title>${esc(`${f.id}: ${f.text || ''} (favours ${f.side})`)}</title></text>`);
    parts.push(`<line class="db-col" x1="${x}" x2="${x}" y1="${head}" y2="${height - 8}"/>`);
  });
  bracket(0, first, `For ${board.sides[0].replace(/_/g, ' ')}`, 's0');
  bracket(first, n, `For ${board.sides[1].replace(/_/g, ' ')}`, 's1');
  rows.forEach((r, j) => {
    const y = head + j * row + row / 2;
    parts.push(`<g class="db-row ${r.kind}${r.kind === 'matter' ? '' : ''}">`);
    if (r.kind === 'matter') parts.push(`<line class="db-rule" x1="8" x2="${width - 8}" y1="${y - row / 2}" y2="${y - row / 2}"/>`);
    parts.push(`<text class="db-name" x="${left - 10}" y="${y + 4}" text-anchor="end">${esc(r.name)}</text>`);
    if (r.kind === 'held') {
      parts.push(`<rect class="db-veil" x="${left}" y="${y - row / 2 + 3}" width="${n * col}" height="${row - 6}"/>`);
      const v = r.verdict ? VERDICTS[r.verdict] : null;
      parts.push(`<text class="db-end v-${r.verdict || 'untested'}" x="${left + n * col + 10}" y="${y + 4}">${esc(v ? `${v.glyph} ${v.word}` : 'held back')}</text>`);
    } else {
      const has = new Set(r.facts), why = new Set(r.reason || []);
      board.factors.forEach((f, i) => {
        if (!has.has(f.id)) return;
        const x = left + i * col + col / 2;
        parts.push(`<circle class="db-pip ${i < first ? 's0' : 's1'}${why.has(f.id) ? ' reason' : ''}" cx="${x}" cy="${y}" r="${why.has(f.id) ? 6 : 3.5}"/>`);
        if (why.has(f.id)) parts.push(`<circle class="db-reason" cx="${x}" cy="${y}" r="9.5"/>`);
      });
      if (r.kind === 'case') {
        const s0 = r.outcome === board.sides[0];
        parts.push(`<text class="db-end ${s0 ? 's0' : 's1'}" x="${left + n * col + 10}" y="${y + 4}">${esc(`${s0 ? '◀' : '▶'} ${String(r.outcome).replace(/_/g, ' ')}${r.stated ? ' (stated)' : r.chosen ? '' : ''}`)}</text>`);
      } else {
        const f = board.matter.forced;
        parts.push(`<text class="db-end forced ${f ? (f === board.sides[0] ? 's0' : 's1') : 'open'}" x="${left + n * col + 10}" y="${y + 4}">${esc(f ? `${f === board.sides[0] ? '◀' : '▶'} forced: ${f.replace(/_/g, ' ')}` : '◆ left open')}</text>`);
      }
    }
    parts.push('</g>');
  });
  return `<svg class="doctrine-board" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" role="img" aria-label="${esc(`the precedents on ${board.fact} by their factors, the reasons a doctrine reads them as decided on, and the matter's factors`)}">${parts.join('')}</svg>`;
}

// The score of a doctrine set out term by term, in words, with the matter's weights.
export function scoreTerms(weights, a) {
  const p = partsOf(weights, a);
  const W = { authority: 10, cases: 1, coherence: 2, justification: 1, ...(weights || {}) };
  return [
    { term: 'against a binding reading', weight: W.authority, value: a.admissible === false ? 1 : 0, sum: p.authority },
    { term: 'decided cases it disagrees with, by force', weight: W.cases, value: Number(a.misfit) || 0, sum: p.cases },
    { term: 'precedents its ratios make inconsistent', weight: W.coherence, value: count(a.conflicts), sum: p.coherence },
    { term: 'the declared purposes it serves', weight: -W.justification, value: Number(a.justification) || 0, sum: -p.worth },
  ];
}
