// The affairs of an undertaking drawn (ADR 0134): markup that needs no page. Each drawing takes what the API's multilateral engine
// answers (science/affairs.hpp) and, where it needs them, the records it read; none asks a server.
//
//   svgOutcome      a question under a rule: the body as a hemicycle of seats, each act told by a glyph and a word in the key, the
//                   members decisive ex post ringed; beside it the verdict, the clauses met or not, and every reading of the rule's
//                   terms with what it gives, a reading set aside struck through and said so
//   svgZone         a negotiation's zone as a field of packages over its first two issues, each cell toned and marked by the
//                   agreement it can have (clear, arguable, untenable; over further issues, the best of them, with their number),
//                   every draft's brackets drawn as a frame closing on the field, the Nash point marked; the drafts in order beneath
//   svgEventSeries  a pair's events by period: cooperation rising and conflict falling from the axis, each class by its own ink and
//                   pattern, and, where the reports would count more, their count as a dashed ghost around the bar, so that a wire
//                   carried fifty times is seen to be one event
//   svgMandates     mandates as lanes in time, each bar its period in force (an arrow where it is open-ended), its links to earlier
//                   mandates as threads with their words, its review due as a diamond (a cross when overdue), its state in words
//   svgReviews      cycles as columns, each recommendation a row with its state's glyph and word, repeats as threads to the cycle after
//   svgLattice      the lattice of memberships, each group a node labelled by the institutions it introduces above and the parties
//                   below (Ganter and Wille's reduced labelling), covers as lines; the regularities beneath in words (options.labels
//                   names the institutions)
//   svgReciprocity  the pairwise and the group imbalance over windows of each length, side by side
//
// Every mark that a page may open carries data-… attributes; nothing is told by colour alone.

import { FONT, PALETTE, STANDINGS, esc, f1, fit, figure, textWidth } from './common.js';

const SIZE = { text: 10.5, small: 9.5, head: 12, big: 15 };

export const ACTS = {
  yes: { glyph: '●', word: 'yes or joined', ink: '#5E7F4E' },
  no: { glyph: '✕', word: 'no', ink: '#B8577A' },
  abstain: { glyph: '◐', word: 'abstained', ink: '#A0784A' },
  'not voting': { glyph: '◌', word: 'present, not voting', ink: '#6E5C78' },
  absent: { glyph: '·', word: 'absent', ink: '#B9AAC0' },
  objection: { glyph: '✖', word: 'objected formally', ink: '#B8577A' },
  reservation: { glyph: '◇', word: 'joined with a reservation', ink: '#A0784A' },
  'not recorded': { glyph: '?', word: 'not recorded', ink: '#6E5C78' },
};

const ANSWERS = {
  adopted: { glyph: '✓', ink: '#5E7F4E' }, 'not adopted': { glyph: '✕', ink: '#B8577A' },
  'turns on the readings': { glyph: '◐', ink: '#A0784A' }, 'turns on the members not recorded': { glyph: '?', ink: '#6E5C78' }, undecided: { glyph: '?', ink: '#6E5C78' },
};

const open = (cls, width, height, label, ground) =>
  `<svg xmlns="http://www.w3.org/2000/svg" class="${cls}" viewBox="0 0 ${f1(width)} ${f1(height)}" width="${f1(width)}" height="${f1(height)}" font-family="${esc(FONT)}" role="img" aria-label="${esc(label)}"><rect class="ma-ground" x="0" y="0" width="${f1(width)}" height="${f1(height)}" fill="${ground}"/>`;

const day = (d) => Date.UTC(+d.slice(0, 4), +d.slice(5, 7) - 1, +d.slice(8, 10)) / 86400000;

// ---- A question under a rule --------------------------------------------------------------------------------------------------

// The acts of the record that decided: the last stage the tally reached, or the one record.
function actsOf(question, answer) {
  if (!question) return {};
  if (question.acts) return question.acts;
  const recs = question.records || {};
  const stages = Object.keys(recs);
  if (answer?.tally?.kind === 'fallback' && answer.tally.parts?.length === 2 && stages.length > 1) return recs[stages[stages.length - 1]].acts || {};
  return (recs[''] || recs[stages[0]] || {}).acts || {};
}

// Seats of a hemicycle: rows from the inside out, each row's seats spread over the half circle, filled in order from the left.
export function hemicycle(n, cx, cy, inner, outer) {
  const rows = Math.max(1, Math.round(Math.sqrt(n / 4)));
  const radii = Array.from({ length: rows }, (_, r) => (rows === 1 ? (inner + outer) / 2 : inner + ((outer - inner) * r) / (rows - 1)));
  const total = radii.reduce((s, r) => s + r, 0);
  const counts = radii.map((r) => Math.max(1, Math.round((n * r) / total)));
  let diff = n - counts.reduce((s, c) => s + c, 0);
  for (let r = rows - 1; diff !== 0; r = (r + rows - 1) % rows) { counts[r] += Math.sign(diff); diff -= Math.sign(diff); }
  const seats = [];
  radii.forEach((r, k) => {
    for (let i = 0; i < counts[k]; i++) {
      const t = counts[k] === 1 ? 0.5 : i / (counts[k] - 1);
      const a = Math.PI * (1 - t);
      seats.push({ x: cx + r * Math.cos(a), y: cy - r * Math.sin(a), angle: a });
    }
  });
  seats.sort((p, q) => q.angle - p.angle || p.y - q.y);
  return seats;
}

function tallyRows(t, depth, out) {
  if (!t) return;
  out.push({ depth, met: t.met, head: t.clause, words: t.words });
  for (const p of t.parts || []) tallyRows(p, depth + 1, out);
}

export function svgOutcome(answer, options = {}) {
  const rule = options.rule || {}, names = options.names || {};
  const width = options.width || 900, ground = options.ground || PALETTE.paper;
  const acts = actsOf(options.question, answer);
  const members = rule.members || Object.keys(acts);
  const decisive = new Set((answer?.decisive || []).map((d) => d.member));
  const left = Math.min(400, width * 0.44);
  const small = members.length <= 24;  // a small body: each seat named
  const cx = left / 2, cy = 196, outer = small ? Math.min(left / 2 - 70, 120) : left / 2 - 18, inner = Math.max(40, outer * 0.45);
  const seats = hemicycle(members.length, cx, cy, inner, outer);
  const seatR = small ? Math.max(6, Math.min(13, (Math.PI * outer) / Math.max(4, members.length) / 2.2))
    : Math.max(3, Math.min(10, (Math.PI * outer) / Math.max(8, members.length / Math.max(1, Math.round(Math.sqrt(members.length / 4)))) / 2.4));
  const rows = [];
  tallyRows(answer?.tally, 0, rows);
  const interp = answer?.interpretations || [];
  const rightX = left + 24, rightW = width - rightX - 16;
  const height = Math.max(300, 92 + rows.length * 17 + 30 + interp.length * 17 + 60);
  const out = [open('ma-outcome', width, height, 'A question decided under a rule', ground)];
  // the body
  const counts = {};
  members.forEach((m, i) => {
    const a = acts[m] || 'not recorded';
    counts[a] = (counts[a] || 0) + 1;
    const s = seats[i], look = ACTS[a] || ACTS['not recorded'];
    out.push(`<g class="ma-seat" data-member="${esc(m)}" data-act="${esc(a)}"><title>${esc(`${names[m] || m}: ${look.word}${decisive.has(m) ? ', decisive' : ''}`)}</title>`);
    if (decisive.has(m)) out.push(`<circle cx="${f1(s.x)}" cy="${f1(s.y)}" r="${f1(seatR + 2.6)}" fill="none" stroke="${PALETTE.orchid}" stroke-width="1.4"/>`);
    out.push(`<text x="${f1(s.x)}" y="${f1(s.y + seatR * 0.55)}" text-anchor="middle" font-size="${f1(seatR * 1.9)}" fill="${look.ink}">${look.glyph}</text>`);
    if (small) {
      const d = seatR + 7, lx = s.x + Math.cos(s.angle) * d, ly = s.y - Math.sin(s.angle) * d + 3.5;
      const anchor = s.angle > Math.PI * 0.58 ? 'end' : s.angle < Math.PI * 0.42 ? 'start' : 'middle';
      out.push(`<text x="${f1(lx)}" y="${f1(ly)}" text-anchor="${anchor}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(fit(names[m] || m, 112, SIZE.small))}</text>`);
    }
    out.push('</g>');
  });
  let ky = cy + 26;
  for (const [a, n] of Object.entries(counts)) {
    const look = ACTS[a] || ACTS['not recorded'];
    out.push(`<text x="16" y="${f1(ky)}" font-size="${SIZE.small}" fill="${look.ink}">${look.glyph}</text><text x="30" y="${f1(ky)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(`${n} ${look.word}`)}</text>`);
    ky += 14;
  }
  if (decisive.size) {
    out.push(`<circle cx="21" cy="${f1(ky - 3.5)}" r="5" fill="none" stroke="${PALETTE.orchid}" stroke-width="1.4"/><text x="30" y="${f1(ky)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(`${decisive.size} decisive: each, acting otherwise alone, would have changed it`)}</text>`);
    ky += 14;
  }
  // the verdict
  const look = ANSWERS[answer?.answer] || ANSWERS.undecided;
  out.push(`<text x="${f1(rightX)}" y="34" font-size="${SIZE.big}" fill="${look.ink}">${look.glyph} ${esc(answer?.answer || 'undecided')}</text>`);
  out.push(`<text x="${f1(rightX)}" y="54" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(answer?.reason || '', rightW, SIZE.small))}</text>`);
  if (rule.instrument || rule.title) out.push(`<text x="${f1(rightX)}" y="70" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">${esc(fit(rule.instrument || rule.title, rightW, SIZE.small))}</text>`);
  let y = 94;
  out.push(`<text x="${f1(rightX)}" y="${f1(y)}" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">the clauses, under the stated readings</text>`);
  y += 16;
  if (!rows.length) out.push(`<text x="${f1(rightX)}" y="${f1(y)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc('no tally: some members are not recorded')}</text>`), (y += 16);
  for (const r of rows) {
    const x = rightX + r.depth * 14;
    out.push(`<g class="ma-clause" data-met="${r.met}"><text x="${f1(x)}" y="${f1(y)}" font-size="${SIZE.text}" fill="${r.met ? '#5E7F4E' : '#B8577A'}">${r.met ? '✓' : '✕'}</text>`);
    const head = `${r.head}: `;
    out.push(`<text x="${f1(x + 14)}" y="${f1(y)}" font-size="${SIZE.text}" fill="${PALETTE.ink}"><tspan font-weight="600">${esc(head)}</tspan>${esc(fit(r.words, rightW - (x - rightX) - 16 - textWidth(head, SIZE.text), SIZE.text))}</text></g>`);
    y += 17;
  }
  y += 10;
  out.push(`<text x="${f1(rightX)}" y="${f1(y)}" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">every reading of its open terms</text>`);
  y += 16;
  for (const i of interp) {
    const words = Object.entries(i.readings || {}).map(([t, r]) => `${t}: ${r}`).join('; ') || 'no open term';
    const g = ANSWERS[i.gives] || ANSWERS.undecided;
    const tail = i.admissible ? i.gives : `${i.gives} (set aside)`;
    out.push(`<g class="ma-reading" data-admissible="${i.admissible}"><text x="${f1(rightX)}" y="${f1(y)}" font-size="${SIZE.text}" fill="${g.ink}" opacity="${i.admissible ? 1 : 0.6}">${g.glyph}</text>`);
    out.push(`<text x="${f1(rightX + 14)}" y="${f1(y)}" font-size="${SIZE.text}" fill="${PALETTE.ink}" opacity="${i.admissible ? 1 : 0.6}"${i.admissible ? '' : ' text-decoration="line-through"'}>${esc(fit(words, rightW - 30 - textWidth(tail, SIZE.text), SIZE.text))}</text>`);
    out.push(`<text x="${f1(width - 16)}" y="${f1(y)}" text-anchor="end" font-size="${SIZE.text}" fill="${g.ink}">${esc(tail)}</text></g>`);
    y += 17;
  }
  if (answer?.discrepancy) out.push(`<text x="${f1(rightX)}" y="${f1(y + 8)}" font-size="${SIZE.text}" fill="${PALETTE.rose}">${esc(fit(`✕ ${answer.discrepancy}`, rightW, SIZE.text))}</text>`);
  out.push('</svg>');
  return out.join('');
}

// ---- A negotiation's zone ------------------------------------------------------------------------------------------------------

const RANK = { clear: 3, arguable: 2, untenable: 1 };
const DRAFT_INKS = ['#5469b4', '#1c8580', '#c18434', '#7f5f96', '#617209', '#8c654f'];

export function svgZone(answer, negotiation, options = {}) {
  const width = options.width || 900, ground = options.ground || PALETTE.paper;
  const issues = negotiation?.issues || [];
  if (issues.length < 2) return `${open('ma-zone', width, 60, 'A negotiation’s zone', ground)}<text x="16" y="34" font-size="${SIZE.text}" fill="${PALETTE.muted}">A field of packages needs two issues.</text></svg>`;
  const [X, Y] = issues;
  const cells = new Map();
  for (const p of answer?.every_option?.agreeable || []) {
    const key = `${p.choice[X.id]}|${p.choice[Y.id]}`;
    const c = cells.get(key) || { best: 'untenable', n: 0 };
    if (RANK[p.agreement] > RANK[c.best]) c.best = p.agreement;
    c.n++;
    cells.set(key, c);
  }
  const rest = issues.slice(2).reduce((s, is) => s * is.options.length, 1);
  const drafts = answer?.drafts || [];
  const leftX = 24 + Math.min(140, Math.max(...Y.options.map((o) => textWidth(o.text || o.id, SIZE.small)))) + 10;
  const fieldW = Math.min(width - leftX - 220, 600), cellW = fieldW / X.options.length, cellH = Math.min(cellW, 30);
  const top = 64, fieldH = cellH * Y.options.length;
  const lines = (d) => Math.max(1, (d.notes || []).length + ((d.settled_now?.length || d.reopened?.length) && !(d.notes || []).length ? 1 : 0));
  const height = top + fieldH + 30 + drafts.reduce((h, d) => h + 18 + 13 * lines(d), 0) + 30;
  const out = [open('ma-zone', width, height, 'A negotiation’s zone of agreement, draft by draft', ground)];
  out.push(`<text x="16" y="24" font-size="${SIZE.head}" fill="${PALETTE.ink}">${esc(fit(negotiation.name || 'A negotiation', width - 32, SIZE.head))}</text>`);
  out.push(`<text x="16" y="42" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">${esc(fit(`across: ${X.text || X.id} · down: ${Y.text || Y.id}`, width - 32, SIZE.small))}</text>`);
  X.options.forEach((o, i) => out.push(`<text x="${f1(leftX + (i + 0.5) * cellW)}" y="${f1(top - 6)}" text-anchor="middle" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(o.text || o.id, cellW - 2, SIZE.small))}</text>`));
  Y.options.forEach((o, j) => out.push(`<text x="${f1(leftX - 6)}" y="${f1(top + (j + 0.5) * cellH + 3)}" text-anchor="end" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(o.text || o.id, leftX - 30, SIZE.small))}</text>`));
  Y.options.forEach((oy, j) => X.options.forEach((ox, i) => {
    const c = cells.get(`${ox.id}|${oy.id}`);
    const st = c ? c.best : 'untenable', look = STANDINGS[st];
    const x = leftX + i * cellW, y = top + j * cellH;
    const fill = st === 'clear' ? '#5E7F4E' : st === 'arguable' ? '#A0784A' : PALETTE.paper;
    out.push(`<g class="ma-package" data-x="${esc(ox.id)}" data-y="${esc(oy.id)}" data-agreement="${st}"><title>${esc(`${X.id}: ${ox.text || ox.id}; ${Y.id}: ${oy.text || oy.id}: ${look.word}${rest > 1 && c ? ` (${c.n} of ${rest} over the other issues)` : ''}`)}</title>`);
    out.push(`<rect x="${f1(x + 0.5)}" y="${f1(y + 0.5)}" width="${f1(cellW - 1)}" height="${f1(cellH - 1)}" fill="${fill}" fill-opacity="${st === 'untenable' ? 1 : st === 'clear' ? 0.55 : 0.32}" stroke="${PALETTE.line}" stroke-width="0.6"/>`);
    if (cellW >= 14) out.push(`<text x="${f1(x + cellW / 2)}" y="${f1(y + cellH / 2 + 3.5)}" text-anchor="middle" font-size="${SIZE.small}" fill="${st === 'untenable' ? PALETTE.faint : PALETTE.ink}">${look.glyph}</text>`);
    out.push('</g>');
  }));
  // the drafts' brackets, outermost first
  const boxOf = (brackets) => {
    const xi = X.options.map((o, i) => (!brackets?.[X.id] || brackets[X.id].includes(o.id) ? i : -1)).filter((i) => i >= 0);
    const yi = Y.options.map((o, j) => (!brackets?.[Y.id] || brackets[Y.id].includes(o.id) ? j : -1)).filter((j) => j >= 0);
    return { x0: Math.min(...xi), x1: Math.max(...xi), y0: Math.min(...yi), y1: Math.max(...yi) };
  };
  (negotiation.drafts || []).forEach((d, k) => {
    const b = boxOf(d.open), ink = k === negotiation.drafts.length - 1 ? PALETTE.orchid : DRAFT_INKS[k % DRAFT_INKS.length];
    const inset = 1.5 + k * 1.2;
    out.push(`<g class="ma-draft" data-draft="${esc(d.id)}"><rect x="${f1(leftX + b.x0 * cellW + inset)}" y="${f1(top + b.y0 * cellH + inset)}" width="${f1((b.x1 - b.x0 + 1) * cellW - 2 * inset)}" height="${f1((b.y1 - b.y0 + 1) * cellH - 2 * inset)}" fill="none" stroke="${ink}" stroke-width="1.6"${k === negotiation.drafts.length - 1 ? '' : ' stroke-dasharray="5 2"'}/></g>`);
  });
  // the Nash point
  const nash = answer?.claimed?.nash?.package;
  if (nash) {
    const pick = (id) => (nash.split('; ').find((s) => s.startsWith(`${id}: `)) || '').slice(id.length + 2);
    const xi = X.options.findIndex((o) => (o.text || o.id) === pick(X.id)), yi = Y.options.findIndex((o) => (o.text || o.id) === pick(Y.id));
    if (xi >= 0 && yi >= 0) out.push(`<text class="ma-nash" x="${f1(leftX + (xi + 0.5) * cellW)}" y="${f1(top + yi * cellH + 10)}" text-anchor="middle" font-size="${SIZE.text}" fill="${PALETTE.orchid}">✦</text>`);
  }
  // the key
  const kx = leftX + fieldW + 24;
  let ky = top + 8;
  for (const st of ['clear', 'arguable', 'untenable']) {
    out.push(`<text x="${f1(kx)}" y="${f1(ky)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${STANDINGS[st].glyph} ${esc(`agreement ${STANDINGS[st].word}`)}</text>`);
    ky += 15;
  }
  if (nash) out.push(`<text x="${f1(kx)}" y="${f1(ky)}" font-size="${SIZE.small}" fill="${PALETTE.orchid}">✦ <tspan fill="${PALETTE.ink}">${esc('Nash, on the declared basis')}</tspan></text>`), (ky += 15);
  if (answer?.linkage) {
    ky += 6;
    const l = answer.linkage;
    out.push(`<text x="${f1(kx)}" y="${f1(ky)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(l.created ? 'linkage creates this zone:' : 'linkage:', width - kx - 12, SIZE.small))}</text>`);
    for (const a of l.alone) out.push(`<text x="${f1(kx)}" y="${f1((ky += 14))}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(fit(`${a.issue} alone: ${a.agreeable} agreeable`, width - kx - 12, SIZE.small))}</text>`);
    out.push(`<text x="${f1(kx)}" y="${f1((ky += 14))}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(`together: ${l.linked} agreeable`)}</text>`);
  }
  if (rest > 1) out.push(`<text x="${f1(kx)}" y="${f1((ky += 20))}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(`each cell the best over ${rest} packages of the other issues`, width - kx - 12, SIZE.small))}</text>`);
  // the drafts in order
  let y = top + fieldH + 30;
  drafts.forEach((d, k) => {
    const ink = k === drafts.length - 1 ? PALETTE.orchid : DRAFT_INKS[k % DRAFT_INKS.length];
    const z = d.zone || {};
    const moves = [d.settled_now?.length ? `settles ${d.settled_now.join(', ')}` : '', d.reopened?.length ? `reopens ${d.reopened.join(', ')}` : ''].filter(Boolean).join('; ');
    out.push(`<g class="ma-step" data-draft="${esc(d.draft)}"><line x1="16" x2="40" y1="${f1(y - 4)}" y2="${f1(y - 4)}" stroke="${ink}" stroke-width="1.6"${k === drafts.length - 1 ? '' : ' stroke-dasharray="5 2"'}/>`);
    out.push(`<text x="48" y="${f1(y)}" font-size="${SIZE.text}" fill="${PALETTE.ink}"><tspan font-weight="600">${esc(d.draft)}</tspan>${esc(`${d.when ? `, ${d.when}` : ''}: ${z.clear || 0} clear and ${z.arguable || 0} arguable of ${figure(z.packages || 0)} packages${moves ? `; it ${moves}` : ''}`)}</text>`);
    let yy = y;
    for (const note of d.notes || []) out.push(`<text x="48" y="${f1((yy += 13))}" font-size="${SIZE.small}" fill="${PALETTE.rose}">${esc(fit(note, width - 64, SIZE.small))}</text>`);
    out.push('</g>');
    y += 18 + 13 * lines(d);
  });
  out.push('</svg>');
  return out.join('');
}

// ---- A pair's events --------------------------------------------------------------------------------------------------------------

const CLASSES = [
  ['material cooperation', '#5E7F4E', 1, ''], ['verbal cooperation', '#5E7F4E', 0.45, ''],
  ['verbal conflict', '#B8577A', 0.45, ''], ['material conflict', '#B8577A', 1, ''],
];

export function svgEventSeries(answer, options = {}) {
  const width = options.width || 900, ground = options.ground || PALETTE.paper, names = options.names || {};
  const s = (answer?.series || [])[options.pair || 0];
  const periods = s?.periods || [];
  const up = (p, key) => CLASSES.filter(([c]) => c.includes('cooperation')).reduce((t, [c]) => t + ((p[key] || {})[c] || 0), 0);
  const down = (p, key) => CLASSES.filter(([c]) => c.includes('conflict')).reduce((t, [c]) => t + ((p[key] || {})[c] || 0), 0);
  const most = Math.max(1, ...periods.map((p) => Math.max(up(p, 'reports'), down(p, 'reports'))));
  const height = 304, mid = 140, half = 96, left = 50, right = 200;
  const step = (width - left - right) / Math.max(1, periods.length), barW = Math.max(8, Math.min(64, step * 0.6));
  const out = [open('ma-events', width, height, 'A pair’s events by period, each event counted once', ground)];
  const pair = s ? s.pair.map((x) => names[x] || x).join(' and ') : 'no pair';
  out.push(`<text x="16" y="22" font-size="${SIZE.head}" fill="${PALETTE.ink}">${esc(fit(`Between ${pair}`, width - 32, SIZE.head))}</text>`);
  out.push(`<line x1="${left}" x2="${f1(width - right)}" y1="${mid}" y2="${mid}" stroke="${PALETTE.faint}"/>`);
  out.push(`<text x="${left - 6}" y="${mid - half + 8}" text-anchor="end" font-size="${SIZE.small}" fill="${PALETTE.muted}">${figure(most)}</text><text x="${left - 6}" y="${mid + half}" text-anchor="end" font-size="${SIZE.small}" fill="${PALETTE.muted}">${figure(most)}</text>`);
  periods.forEach((p, i) => {
    const x = left + i * step + (step - barW) / 2;
    out.push(`<g class="ma-period" data-period="${esc(p.period)}"><title>${esc(`${p.period}: net ${p.net}`)}</title>`);
    // the reports' count, a ghost
    const ru = up(p, 'reports') > up(p, 'events') ? (up(p, 'reports') / most) * half : 0, rd = down(p, 'reports') > down(p, 'events') ? (down(p, 'reports') / most) * half : 0;
    if (ru > 0) out.push(`<rect x="${f1(x - 2)}" y="${f1(mid - ru)}" width="${f1(barW + 4)}" height="${f1(ru)}" fill="none" stroke="${PALETTE.muted}" stroke-dasharray="2 2" stroke-width="0.8"/>`,
      `<text x="${f1(x + barW / 2)}" y="${f1(mid - ru - 4)}" text-anchor="middle" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(`${up(p, 'reports')} reports`)}</text>`);
    if (rd > 0) out.push(`<rect x="${f1(x - 2)}" y="${mid}" width="${f1(barW + 4)}" height="${f1(rd)}" fill="none" stroke="${PALETTE.muted}" stroke-dasharray="2 2" stroke-width="0.8"/>`,
      `<text x="${f1(x + barW / 2)}" y="${f1(mid + rd + 12)}" text-anchor="middle" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(`${down(p, 'reports')} reports`)}</text>`);
    let yu = mid, yd = mid;
    for (const [c, ink, op] of CLASSES) {
      const n = (p.events || {})[c] || 0;
      if (!n) continue;
      const h = (n / most) * half;
      if (c.includes('cooperation')) yu -= h, out.push(`<rect x="${f1(x)}" y="${f1(yu)}" width="${f1(barW)}" height="${f1(h)}" fill="${ink}" fill-opacity="${op}" data-class="${c}"/>`);
      else out.push(`<rect x="${f1(x)}" y="${f1(yd)}" width="${f1(barW)}" height="${f1(h)}" fill="${ink}" fill-opacity="${op}" data-class="${c}"/>`), (yd += h);
    }
    out.push(`<text x="${f1(x + barW / 2)}" y="${height - 10}" text-anchor="middle" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(p.period)}</text>`);
    out.push(`<text x="${f1(x + barW / 2)}" y="${height - 24}" text-anchor="middle" font-size="${SIZE.small}" fill="${p.net >= 0 ? '#5E7F4E' : '#B8577A'}">${p.net > 0 ? '+' : ''}${p.net}</text></g>`);
  });
  let ky = 46;
  const kx = width - right + 18;
  out.push(`<text x="${kx}" y="${ky}" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">each event counted once</text>`);
  for (const [c, ink, op] of CLASSES) {
    ky += 16;
    out.push(`<rect x="${kx}" y="${ky - 9}" width="12" height="10" fill="${ink}" fill-opacity="${op}"/><text x="${kx + 18}" y="${ky}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(c)}</text>`);
  }
  ky += 16;
  out.push(`<rect x="${kx - 1}" y="${ky - 10}" width="14" height="12" fill="none" stroke="${PALETTE.muted}" stroke-dasharray="2 2" stroke-width="0.8"/><text x="${kx + 18}" y="${ky}" font-size="${SIZE.small}" fill="${PALETTE.ink}">as the reports count</text>`);
  out.push(`<text x="${kx}" y="${ky + 20}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc('cooperation above, conflict below;')}</text><text x="${kx}" y="${ky + 33}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc('the figure is the net')}</text>`);
  out.push('</svg>');
  return out.join('');
}

// ---- Mandates in time -------------------------------------------------------------------------------------------------------------

const MANDATE_STATES = {
  'in force': { glyph: '●', ink: '#5E7F4E' }, 'not yet in force': { glyph: '○', ink: '#6E5C78' }, expired: { glyph: '◌', ink: '#6E5C78' },
  renewed: { glyph: '↻', ink: '#5469b4' }, superseded: { glyph: '⇥', ink: '#A0784A' }, consolidated: { glyph: '⊕', ink: '#A0784A' }, terminated: { glyph: '✕', ink: '#B8577A' },
};

export function svgMandates(answer, options = {}) {
  const width = options.width || 900, ground = options.ground || PALETTE.paper, at = options.at || '';
  // Lanes by lineage: each root by its beginning, then what continues it, depth first, so that a renewal lies beside what it renews.
  const all = [...(answer?.mandates || [])].sort((a, b) => (a.begins < b.begins ? -1 : a.begins > b.begins ? 1 : 0));
  const ms = [], placed = new Set();
  const visit = (m) => {
    if (placed.has(m.id)) return;
    placed.add(m.id), ms.push(m);
    for (const n of all) if ((n.earlier || []).some((e) => e.mandate === m.id)) visit(n);
  };
  for (const m of all) if (!(m.earlier || []).some((e) => all.some((n) => n.id === e.mandate))) visit(m);
  for (const m of all) visit(m);
  const dates = ms.flatMap((m) => [m.begins, m.ends, m.review?.due]).concat(at).filter(Boolean);
  const lo = Math.min(...dates.map(day)), hi = Math.max(...dates.map(day)) + 120;
  const left = 24 + Math.min(240, Math.max(80, ...ms.map((m) => textWidth(m.title || m.id, SIZE.text)))), right = 190, laneH = 34, top = 40;
  const X = (d) => left + ((day(d) - lo) / Math.max(1, hi - lo)) * (width - left - right);
  const height = top + ms.length * laneH + 40;
  const row = new Map(ms.map((m, i) => [m.id, i]));
  const out = [open('ma-mandates', width, height, 'Mandates in time, with their links and reviews', ground)];
  for (let y = new Date(lo * 86400000).getUTCFullYear(); y <= new Date(hi * 86400000).getUTCFullYear(); y++) {
    const x = X(`${y}-01-01`);
    if (x < left || x > width - right) continue;
    out.push(`<line x1="${f1(x)}" x2="${f1(x)}" y1="${top - 8}" y2="${f1(height - 30)}" stroke="${PALETTE.line}" stroke-width="0.6"/><text x="${f1(x + 3)}" y="${top - 12}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${y}</text>`);
  }
  if (at) out.push(`<line x1="${f1(X(at))}" x2="${f1(X(at))}" y1="${top - 8}" y2="${f1(height - 30)}" stroke="${PALETTE.orchid}" stroke-width="1"/><text x="${f1(X(at) + 3)}" y="${f1(height - 18)}" font-size="${SIZE.small}" fill="${PALETTE.orchid}">${esc(at)}</text>`);
  ms.forEach((m, i) => {
    const y = top + i * laneH + 14;
    const st = MANDATE_STATES[m.state] || MANDATE_STATES.expired;
    const x0 = X(m.begins), x1 = m.ends ? X(m.ends) : width - right - 4;
    out.push(`<g class="ma-mandate" data-mandate="${esc(m.id)}" data-state="${esc(m.state)}"><title>${esc(`${m.id}${m.title ? `: ${m.title}` : ''}: ${m.why}`)}</title>`);
    out.push(`<text x="16" y="${f1(y + 4)}" font-size="${SIZE.text}" fill="${PALETTE.ink}">${esc(fit(m.title || m.id, left - 24, SIZE.text))}</text>`);
    out.push(`<rect x="${f1(x0)}" y="${f1(y - 5)}" width="${f1(Math.max(2, x1 - x0))}" height="10" fill="${m.resourced ? PALETTE.laurel : PALETTE.paper}" fill-opacity="${m.resourced ? 0.5 : 1}" stroke="${PALETTE.laurel}" stroke-width="1"${m.resourced ? '' : ' stroke-dasharray="3 2"'} rx="1.5"/>`);
    if (!m.ends) out.push(`<text x="${f1(x1 + 2)}" y="${f1(y + 4)}" font-size="${SIZE.text}" fill="${PALETTE.laurel}">→</text>`);
    if (m.review?.due && m.state === 'in force') {
      const xd = X(m.review.due);
      out.push(`<text x="${f1(xd)}" y="${f1(y - 8)}" text-anchor="middle" font-size="${SIZE.small}" fill="${m.review.overdue ? PALETTE.rose : PALETTE.muted}">${m.review.overdue ? '✕' : '◇'}</text>`);
    }
    out.push(`<text x="${f1(width - right + 12)}" y="${f1(y + 4)}" font-size="${SIZE.small}" fill="${st.ink}">${st.glyph} ${esc(fit(m.state + (m.by ? ` by ${m.by}` : ''), right - 30, SIZE.small))}</text>`);
    for (const e of m.earlier || []) {
      const j = row.get(e.mandate);
      if (j === undefined) continue;
      const yj = top + j * laneH + 14;
      out.push(`<path d="M ${f1(x0)} ${f1(yj + 5)} C ${f1(x0 - 18)} ${f1(yj + 14)}, ${f1(x0 - 18)} ${f1(y - 14)}, ${f1(x0)} ${f1(y - 5)}" fill="none" stroke="${PALETTE.muted}" stroke-width="0.9"/>`);
      out.push(`<text x="${f1(x0 - 20)}" y="${f1((yj + y) / 2 + 3)}" text-anchor="end" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">${esc(e.link)}</text>`);
    }
    out.push('</g>');
  });
  out.push(`<text x="16" y="${f1(height - 8)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc('a bar filled when resourced, dashed when not; ◇ the next review of a mandate in force, ✕ one overdue; → open-ended')}</text>`);
  out.push('</svg>');
  return out.join('');
}

// ---- Review cycles ----------------------------------------------------------------------------------------------------------------

function reviewLook(r) {
  if (r.response === 'no response') return { glyph: '?', word: 'unanswered', ink: '#6E5C78' };
  if (r.response === 'noted') return { glyph: '–', word: 'noted', ink: '#6E5C78' };
  if (r.response === 'rejected') return { glyph: '✕', word: 'rejected', ink: '#B8577A' };
  if (!r.reported) return { glyph: '○', word: 'supported, unreported', ink: '#5469b4' };
  if (r.standing === 'contested') return { glyph: '⚡', word: 'contested', ink: '#B8577A' };
  if (r.done === 'implemented') return r.standing === 'evidenced' ? { glyph: '●', word: 'implemented, evidenced', ink: '#5E7F4E' } : { glyph: '◐', word: 'implemented, claimed', ink: '#A0784A' };
  if (r.done === 'in part') return { glyph: '◔', word: 'in part', ink: '#A0784A' };
  return { glyph: '✕', word: 'not implemented', ink: '#B8577A' };
}

export function svgReviews(answer, options = {}) {
  const width = options.width || 900, ground = options.ground || PALETTE.paper, names = options.names || {};
  const recs = answer?.recommendations || [];
  const cycles = [...new Set(recs.map((r) => r.cycle))];
  const colW = (width - 32) / Math.max(1, cycles.length), rowH = 18, top = 44;
  const byCycle = cycles.map((c) => recs.filter((r) => r.cycle === c));
  const height = top + Math.max(1, ...byCycle.map((v) => v.length)) * rowH + 30;
  const pos = new Map();
  const out = [open('ma-reviews', width, height, 'Review cycles: each recommendation and its state', ground)];
  byCycle.forEach((v, k) => {
    const x = 16 + k * colW;
    out.push(`<text x="${f1(x)}" y="26" font-size="${SIZE.head}" fill="${PALETTE.ink}">${esc(`cycle ${cycles[k]}`)}</text>`);
    v.forEach((r, i) => {
      const y = top + i * rowH + 10, look = reviewLook(r);
      pos.set(r.id, { x, y });
      out.push(`<g class="ma-recommendation" data-recommendation="${esc(r.id)}"><title>${esc(`${r.id}: ${r.text || r.theme} (${r.words})`)}</title>`);
      out.push(`<text x="${f1(x)}" y="${f1(y)}" font-size="${SIZE.text}" fill="${look.ink}">${look.glyph}</text>`);
      const theme = `${r.theme || r.text}: `;
      out.push(`<text x="${f1(x + 14)}" y="${f1(y)}" font-size="${SIZE.text}" fill="${PALETTE.ink}"><tspan font-weight="600">${esc(theme)}</tspan>${esc(fit(`${names[r.from] || r.from} → ${names[r.to] || r.to}`, colW - 130 - textWidth(theme, SIZE.text), SIZE.text))}</text>`);
      out.push(`<text x="${f1(x + colW - 18)}" y="${f1(y)}" text-anchor="end" font-size="${SIZE.small}" fill="${look.ink}">${esc(look.word)}</text></g>`);
    });
  });
  for (const rp of answer?.repeats || []) {
    const a = pos.get(rp.earlier), b = pos.get(rp.later);
    if (!a || !b) continue;
    out.push(`<path class="ma-repeat" data-later="${esc(rp.later)}" d="M ${f1(a.x + colW - 14)} ${f1(a.y - 4)} C ${f1(a.x + colW + 4)} ${f1(a.y - 4)}, ${f1(b.x - 14)} ${f1(b.y - 4)}, ${f1(b.x - 4)} ${f1(b.y - 4)}" fill="none" stroke="${PALETTE.orchid}" stroke-width="1"${rp.declared ? '' : ' stroke-dasharray="3 2"'}/>`);
  }
  out.push(`<text x="16" y="${f1(height - 8)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc('a thread joins a recommendation to its repetition in a later cycle, solid when declared and dashed when matched')}</text>`);
  out.push('</svg>');
  return out.join('');
}

// ---- The lattice of memberships ---------------------------------------------------------------------------------------------------

export function svgLattice(answer, options = {}) {
  const width = options.width || 900, ground = options.ground || PALETTE.paper, names = options.names || {};
  const label = (id) => options.labels?.[id] || id;  // an institution's name
  const groups = answer?.lattice?.groups || [];
  const covers = answer?.lattice?.covers || [];
  const levels = [...new Set(groups.map((g) => g.parties.length))].sort((a, b) => b - a);
  const levelOf = (g) => levels.indexOf(g.parties.length);
  const byLevel = levels.map(() => []);
  groups.forEach((g, i) => byLevel[levelOf(g)].push(i));
  const regs = answer?.regularities || [];
  const levelH = 74, top = 40;
  const height = top + Math.max(1, levels.length) * levelH + 24 + regs.length * 15 + 10;
  const at = new Map();
  byLevel.forEach((v, l) => v.forEach((i, k) => at.set(i, { x: ((k + 1) * width) / (v.length + 1), y: top + l * levelH + 20 })));
  const upper = (i) => covers.filter(([lo]) => lo === i).map(([, up]) => up);
  const lower = (i) => covers.filter(([, up]) => up === i).map(([lo]) => lo);
  const out = [open('ma-lattice', width, height, 'The lattice of memberships', ground)];
  for (const [a, b] of covers) {
    const p = at.get(a), q = at.get(b);
    out.push(`<line x1="${f1(p.x)}" y1="${f1(p.y)}" x2="${f1(q.x)}" y2="${f1(q.y)}" stroke="${PALETTE.faint}" stroke-width="1"/>`);
  }
  groups.forEach((g, i) => {
    const p = at.get(i);
    const intro = g.institutions.filter((x) => !upper(i).some((u) => groups[u].institutions.includes(x)));
    const own = g.parties.filter((x) => !lower(i).some((d) => groups[d].parties.includes(x)));
    out.push(`<g class="ma-group" data-group="${i}"><title>${esc(`${g.parties.map((x) => names[x] || x).join(', ') || 'no party'}: ${g.institutions.map(label).join(', ') || 'no institution in common'}`)}</title>`);
    out.push(`<circle cx="${f1(p.x)}" cy="${f1(p.y)}" r="5" fill="${intro.length ? PALETTE.laurel : PALETTE.paper}" stroke="${PALETTE.laurel}" stroke-width="1.2"/>`);
    if (intro.length) out.push(`<text x="${f1(p.x)}" y="${f1(p.y - 10)}" text-anchor="middle" font-size="${SIZE.text}" fill="${PALETTE.ink}" stroke="${ground}" stroke-width="3" paint-order="stroke" font-weight="600">${esc(fit(intro.map(label).join(', '), 220, SIZE.text))}</text>`);
    if (own.length) out.push(`<text x="${f1(p.x)}" y="${f1(p.y + 18)}" text-anchor="middle" font-size="${SIZE.small}" fill="${PALETTE.muted}" stroke="${ground}" stroke-width="3" paint-order="stroke">${esc(fit(own.map((x) => names[x] || x).join(', '), 200, SIZE.small))}</text>`);
    out.push('</g>');
  });
  let y = top + levels.length * levelH + 18;
  regs.forEach((r) => {
    const words = r.premise.length ? `every member of ${r.premise.map(label).join(' and ')} belongs to ${r.conclusion.map(label).join(' and ')}` : `every party belongs to ${r.conclusion.map(label).join(' and ')}`;
    out.push(`<text x="16" y="${f1(y)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(fit(`· ${words}`, width - 32, SIZE.small))}</text>`);
    y += 15;
  });
  out.push('</svg>');
  return out.join('');
}

export function svgReciprocity(answer, options = {}) {
  const width = options.width || 560, ground = options.ground || PALETTE.paper;
  const rows = answer?.reciprocity || [];
  const height = 46 + rows.length * 34 + 30, left = 120, barW = (width - left - 80);
  const out = [open('ma-reciprocity', width, height, 'Reciprocity specific and diffuse over windows', ground)];
  out.push(`<text x="16" y="24" font-size="${SIZE.head}" fill="${PALETTE.ink}">Imbalance over windows</text>`);
  rows.forEach((r, i) => {
    const y = 46 + i * 34;
    out.push(`<g class="ma-window" data-days="${r.days}"><text x="16" y="${f1(y + 12)}" font-size="${SIZE.text}" fill="${PALETTE.ink}">${esc(`${r.days} days`)}</text>`);
    out.push(`<rect x="${left}" y="${f1(y)}" width="${f1(Math.max(0.5, barW * (r.pairwise || 0)))}" height="10" fill="${PALETTE.rose}" fill-opacity="0.6"/><text x="${f1(left + barW + 6)}" y="${f1(y + 9)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(`pairs ${figure(r.pairwise)}`)}</text>`);
    out.push(`<rect x="${left}" y="${f1(y + 13)}" width="${f1(Math.max(0.5, barW * (r.group || 0)))}" height="10" fill="${PALETTE.laurel}" fill-opacity="0.6"/><text x="${f1(left + barW + 6)}" y="${f1(y + 22)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(`group ${figure(r.group)}`)}</text></g>`);
  });
  out.push(`<text x="16" y="${f1(height - 10)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit('0 when balanced: pairs (specific reciprocity), the group (diffuse); the group never exceeds the pairs', width - 32, SIZE.small))}</text>`);
  out.push('</svg>');
  return out.join('');
}
