// An undertaking's trajectory drawn as strands beneath the round (ADR 0132; the design of 2026-11-19, §4): markup that needs no page.
//
// Time runs from left to right over the dates the records hold. Each strand is a kind of change, and each change a knot on it, told by
// a glyph and a word as well as by its place:
//
//   relations      a tie begun (●), ended (○) or recorded absent (×)
//   observation    a design of observation begun (▸) or ended (◂): what the record can say is absent, and when
//   decisions      a decision (◆), ringed when it closed provisionally
//   commitments    a commitment made (□), due (◇), honoured (✓) or found overdue at its date (✕)
//   contributions  what a party gave, one strand for each unit, each knot's area in proportion to its amount
//
// Knots on one date stand one above another. A line marks the date the page reads (options.at). Every knot carries data-kind,
// data-date and data-what, so that a page opens it where it stands; classes (ms-…) let a page restyle them.
//
// svgStrands(answer, options) takes the engine's answer to `undertaking` (its timeline), and options.partnership the engine's answer
// to `partnership` with options.record the partnership's record (for contributions and their dates).
//
// options: { at, width, partnership, record, ground, title: true }

import { FONT, PALETTE, esc, f1, fit, figure, STATES } from './common.js';

const SIZE = { strand: 10.5, tick: 9, small: 9 };
const STRAND_INK = { relations: '#5469b4', observation: '#1c8580', decisions: '#8E4FA0', commitments: '#c18434', contributions: '#5E7F4E' };

const KNOTS = {
  'tie begins': { strand: 'relations', glyph: '●', word: 'a tie begins' },
  'tie ends': { strand: 'relations', glyph: '○', word: 'a tie ends' },
  'recorded absent': { strand: 'relations', glyph: '×', word: 'recorded absent' },
  'observed from': { strand: 'observation', glyph: '▸', word: 'observed from' },
  'observed until': { strand: 'observation', glyph: '◂', word: 'observed until' },
  decision: { strand: 'decisions', glyph: '◆', word: 'a decision' },
  made: { strand: 'commitments', glyph: '□', word: 'a commitment made' },
  due: { strand: 'commitments', glyph: '◇', word: 'a commitment due' },
  honoured: { strand: 'commitments', glyph: '✓', word: 'honoured' },
  overdue: { strand: 'commitments', glyph: '✕', word: 'overdue at its date' },
  contribution: { strand: 'contributions', glyph: '◉', word: 'a contribution' },
};

const day = (d) => Date.UTC(+d.slice(0, 4), +d.slice(5, 7) - 1, +d.slice(8, 10)) / 86400000;

// The knots, from the engine's answers and the partnership's record.
export function knotsOf(answer, options = {}) {
  const knots = [];
  for (const e of answer?.timeline || []) if (KNOTS[e.kind] && /^\d{4}-\d{2}-\d{2}/.test(e.date)) knots.push({ kind: e.kind, date: e.date.slice(0, 10), what: e.what, participants: e.participants || [] });
  for (const s of options.partnership?.states || []) {
    if (s.made) knots.push({ kind: 'made', date: s.made, what: s.text || s.commitment, participants: [s.by] });
    if (s.reached_on) knots.push({ kind: 'honoured', date: s.reached_on, what: s.text || s.commitment, participants: [s.by] });
    else if (s.due) knots.push({ kind: s.state === 'overdue' ? 'overdue' : 'due', date: s.due, what: s.text || s.commitment, participants: [s.by] });
  }
  for (const c of options.record?.contributions || []) {
    if (/^\d{4}-\d{2}-\d{2}/.test(c.when || '')) knots.push({ kind: 'contribution', date: c.when.slice(0, 10), what: `${figure(c.amount)} ${c.unit || ''}`.trim(), participants: [c.party], amount: c.amount, unit: c.unit || '' });
  }
  return knots;
}

export function svgStrands(answer, options = {}) {
  const knots = knotsOf(answer, options);
  const ground = options.ground || PALETTE.paper;
  const width = options.width || 900;
  const left = 132, right = 24;
  const units = [...new Set(knots.filter((k) => k.kind === 'contribution').map((k) => k.unit))];
  const strands = ['relations', 'observation', 'decisions', 'commitments', ...units.map((u) => `contributions:${u}`)].filter((s) => s.startsWith('contributions:') || knots.some((k) => KNOTS[k.kind].strand === s));
  const strandOf = (k) => (k.kind === 'contribution' ? `contributions:${k.unit}` : KNOTS[k.kind].strand);
  const dates = knots.map((k) => day(k.date)).concat(options.at ? [day(options.at)] : []);
  const out = [];
  if (!knots.length) {
    const h = 60;
    out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="ms-strands" viewBox="0 0 ${width} ${h}" width="${width}" height="${h}" font-family="${esc(FONT)}" role="img" aria-label="No change is dated in the record">`);
    out.push(`<rect x="0" y="0" width="${width}" height="${h}" fill="${ground}"/><text x="${width / 2}" y="34" text-anchor="middle" font-size="11" fill="${PALETTE.muted}" font-style="italic">No change is dated in the record.</text></svg>`);
    return out.join('');
  }
  let t0 = Math.min(...dates), t1 = Math.max(...dates);
  if (t1 - t0 < 60) t0 -= 30, t1 += 30;
  const pad = (t1 - t0) * 0.03;
  t0 -= pad, t1 += pad;
  const X = (d) => left + ((day(d) - t0) / (t1 - t0)) * (width - left - right);
  // rows: each strand is as tall as its deepest stack of knots on one date
  const depth = {};
  const stack = {};
  for (const k of knots) {
    const key = `${strandOf(k)}|${k.date}`;
    k.level = stack[key] = (stack[key] ?? -1) + 1;
    depth[strandOf(k)] = Math.max(depth[strandOf(k)] || 0, k.level + 1);
  }
  const rowTop = {};
  let y = options.title === false ? 18 : 34;
  for (const s of strands) {
    rowTop[s] = y;
    y += 26 + 11 * Math.max(0, Math.min(6, (depth[s] || 1) - 1));
  }
  const height = y + 30;
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="ms-strands" viewBox="0 0 ${f1(width)} ${f1(height)}" width="${f1(width)}" height="${f1(height)}" font-family="${esc(FONT)}" role="img" aria-label="The undertaking's trajectory, its changes as knots on strands over time">`);
  out.push(`<rect class="ms-ground" x="0" y="0" width="${f1(width)}" height="${f1(height)}" fill="${ground}"/>`);
  if (options.title !== false) out.push(`<text class="ms-title" x="${left}" y="18" font-size="11" fill="${PALETTE.muted}" font-style="italic">What changed, and when</text>`);
  // years
  const y0 = new Date(t0 * 86400000).getUTCFullYear(), y1 = new Date(t1 * 86400000).getUTCFullYear();
  const step = Math.max(1, Math.ceil((y1 - y0 + 1) / 12));
  for (let yr = y0; yr <= y1 + 1; yr += step) {
    const d = `${yr}-01-01`;
    if (day(d) < t0 || day(d) > t1) continue;
    const x = X(d);
    out.push(`<path class="ms-year" d="M${f1(x)},${f1(rowTop[strands[0]] - 8)} V${f1(height - 22)}" stroke="${PALETTE.line}" stroke-width="0.6"/><text class="ms-tick" x="${f1(x)}" y="${f1(height - 10)}" text-anchor="middle" font-size="${SIZE.tick}" fill="${PALETTE.muted}">${yr}</text>`);
  }
  // strands
  const largest = {};
  for (const k of knots) if (k.kind === 'contribution') largest[k.unit] = Math.max(largest[k.unit] || 0, Math.abs(k.amount || 0));
  for (const s of strands) {
    const yy = rowTop[s] + 6;
    const unit = s.startsWith('contributions:') ? s.slice(14) : null;
    const name = unit !== null ? `contributions${unit ? `, ${unit}` : ''}` : s;
    const ink = STRAND_INK[unit !== null ? 'contributions' : s];
    out.push(`<text class="ms-strand-name" x="${left - 12}" y="${f1(yy + 3.5)}" text-anchor="end" font-size="${SIZE.strand}" fill="${PALETTE.ink}">${esc(name)}</text>`);
    out.push(`<path class="ms-strand" d="M${left},${f1(yy)} H${f1(width - right)}" stroke="${ink}" stroke-width="1" stroke-opacity="0.45"/>`);
  }
  // knots
  for (const k of knots) {
    const s = strandOf(k);
    const ink = STRAND_INK[KNOTS[k.kind].strand];
    const x = X(k.date), yy = rowTop[s] + 6 + 11 * Math.min(6, k.level);
    const title = `<title>${esc(k.date)}: ${esc(KNOTS[k.kind].word)}, ${esc(k.what)}</title>`;
    const data = `data-kind="${esc(k.kind)}" data-date="${esc(k.date)}" data-what="${esc(k.what)}"`;
    if (k.kind === 'contribution') {
      const r = 2 + 7 * Math.sqrt(Math.abs(k.amount || 0) / (largest[k.unit] || 1));
      out.push(`<g class="ms-knot contribution" ${data}>${title}<circle cx="${f1(x)}" cy="${f1(yy)}" r="${f1(r)}" fill="${ink}" fill-opacity="0.22" stroke="${ink}" stroke-width="0.9"/></g>`);
    } else {
      const g = KNOTS[k.kind].glyph;
      const stateInk = k.kind === 'overdue' ? STATES.overdue.ink : k.kind === 'honoured' ? STATES.honoured.ink : ink;
      out.push(`<g class="ms-knot ${esc(k.kind.replace(/ /g, '-'))}" ${data}>${title}<circle cx="${f1(x)}" cy="${f1(yy)}" r="5.2" fill="${ground}" stroke="none"/><text x="${f1(x)}" y="${f1(yy + 3.4)}" text-anchor="middle" font-size="10" fill="${stateInk}">${esc(g)}</text></g>`);
    }
  }
  if (options.at) {
    const x = X(options.at);
    out.push(`<path class="ms-at" d="M${f1(x)},${f1(rowTop[strands[0]] - 10)} V${f1(height - 22)}" stroke="${PALETTE.orchid}" stroke-width="1.2" stroke-dasharray="3 2"/><text class="ms-at-label" x="${f1(x + 4)}" y="${f1(rowTop[strands[0]] - 12)}" font-size="${SIZE.small}" fill="${PALETTE.orchid}">read on ${esc(options.at)}</text>`);
  }
  out.push('</svg>');
  return out.join('');
}

export { KNOTS };
