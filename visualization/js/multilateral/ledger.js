// A partnership's ledger drawn (ADR 0132): its commitments and the benefit each party finds, as markup that needs no page.
//
// svgCommitments(answer, options): each commitment a row, as the engine's answer to `partnership` gives it:
//   - its words and its holder;
//   - its screen, a small mark for each criterion (S, M, A, R, T for SMART), told by a glyph (✓ holds, ✕ fails, ◐ undecided) beneath
//     its letter, never by a colour alone;
//   - its progress from baseline to target as a bar, with the target as a tick, when the record can say it; "no indicator" in words
//     when it cannot;
//   - its state as a glyph and a word: honoured, honoured late, in progress, overdue, unreported.
//
// svgBenefits(answer, options): each party a column, never one number for all. A party's head says its standing (clear, arguable,
// untenable, undecided) by a glyph and a word, and the measure in words (the probability, or its interval, that every criterion is met).
// Beneath, each of its criteria: the dimension, the threshold as a tick, and the interval the evidence allows of the value as a bar
// with its estimate as a dot; a criterion not observed is a broken outline with "not observed", never an empty bar.
//
// Every row and column carries data-commitment or data-party; classes (ml-…) let a page restyle them.

import { FONT, PALETTE, STANDINGS, STATES, VERDICT, esc, f1, fit, figure } from './common.js';

const SIZE = { text: 11, small: 9.5, letter: 9, head: 11.5 };
const LETTERS = { specific: 'S', measurable: 'M', achievable: 'A', 'resource-based': 'R', 'time-bound': 'T' };

export function svgCommitments(answer, options = {}) {
  const states = answer?.states || [];
  const screens = Object.fromEntries((answer?.screens || []).map((s) => [s.commitment, s]));
  const names = options.names || {};
  const width = options.width || 860;
  const ground = options.ground || PALETTE.paper;
  const rowH = 38, top = 30;
  const colText = 16, colScreen = width * 0.46, colBar = colScreen + 112, colState = width - 132;
  const height = top + rowH * Math.max(1, states.length) + 14;
  const out = [];
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="ml-commitments" viewBox="0 0 ${f1(width)} ${f1(height)}" width="${f1(width)}" height="${f1(height)}" font-family="${esc(FONT)}" role="img" aria-label="The partnership's commitments, each with its screen, its progress and its state">`);
  out.push(`<rect class="ml-ground" x="0" y="0" width="${f1(width)}" height="${f1(height)}" fill="${ground}"/>`);
  const heads = [[colText, 'start', 'commitment, and who holds it'], [colScreen, 'start', 'screen'], [colBar, 'start', 'progress to its target'], [colState + 8, 'start', 'state']];
  for (const [x, anchor, t] of heads) out.push(`<text class="ml-head" x="${f1(x)}" y="18" text-anchor="${anchor}" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">${esc(t)}</text>`);
  if (!states.length) out.push(`<text x="${colText}" y="${top + 20}" font-size="${SIZE.text}" fill="${PALETTE.muted}" font-style="italic">No commitment is recorded.</text>`);
  states.forEach((s, i) => {
    const y = top + i * rowH;
    const st = STATES[s.state] || STATES.unreported;
    out.push(`<g class="ml-row${options.selected === s.commitment ? ' chosen' : ''}" data-commitment="${esc(s.commitment)}">`);
    if (i) out.push(`<path d="M${colText},${f1(y - 4)} H${f1(width - 16)}" stroke="${PALETTE.line}" stroke-width="0.6"/>`);
    if (options.selected === s.commitment) out.push(`<rect x="4" y="${f1(y - 3)}" width="${f1(width - 8)}" height="${rowH - 2}" fill="none" stroke="${PALETTE.orchid}" stroke-width="1.2" rx="2"/>`);
    out.push(`<text x="${colText}" y="${f1(y + 13)}" font-size="${SIZE.text}" fill="${PALETTE.ink}">${esc(fit(s.text || s.commitment, colScreen - colText - 16, SIZE.text))}</text>`);
    out.push(`<text x="${colText}" y="${f1(y + 27)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(`${names[s.by] || s.by || '—'}${s.due ? ` · due ${s.due}` : ''}${s.amended ? ` · amended ${s.amended}×` : ''}`, colScreen - colText - 16, SIZE.small))}</text>`);
    // the screen
    const sc = screens[s.commitment];
    (sc?.checks || []).forEach((c, k) => {
      const x = colScreen + 10 + k * 19;
      const v = VERDICT[c.verdict.status] || VERDICT.undecided;
      const ink = c.verdict.status === 'holds' ? PALETTE.laurel : c.verdict.status === 'fails' ? PALETTE.rose : PALETTE.muted;
      out.push(`<g class="ml-criterion ${esc(c.verdict.status)}" data-criterion="${esc(c.name)}"><title>${esc(c.name)}: ${esc(v.word)}. ${esc(c.verdict.reason)}</title>`
        + `<text x="${f1(x)}" y="${f1(y + 12)}" text-anchor="middle" font-size="${SIZE.letter}" fill="${PALETTE.muted}">${esc(LETTERS[c.name] || c.name[0].toUpperCase())}</text>`
        + `<text x="${f1(x)}" y="${f1(y + 26)}" text-anchor="middle" font-size="11" fill="${ink}">${esc(v.glyph)}</text></g>`);
    });
    // the progress
    const bw = colState - colBar - 22;
    if (Number.isFinite(s.progress)) {
      const p = Math.max(0, Math.min(1.25, s.progress));
      out.push(`<rect class="ml-track" x="${f1(colBar)}" y="${f1(y + 9)}" width="${f1(bw)}" height="7" fill="none" stroke="${PALETTE.line}" stroke-width="0.8" rx="1"/>`);
      out.push(`<rect class="ml-bar" x="${f1(colBar)}" y="${f1(y + 9)}" width="${f1((bw * p) / 1.25)}" height="7" fill="${st.ink}" fill-opacity="0.55" rx="1"/>`);
      const tx = colBar + bw / 1.25;
      out.push(`<path class="ml-target" d="M${f1(tx)},${f1(y + 5)} V${f1(y + 20)}" stroke="${PALETTE.ink}" stroke-width="1"/>`);
      out.push(`<text x="${f1(colBar)}" y="${f1(y + 30)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(`${figure(s.latest)} of ${figure(s.target)}${s.unit ? ` ${s.unit}` : ''} · ${Math.round(100 * s.progress)}%`, bw, SIZE.small))}</text>`);
    } else {
      const why = !s.indicator ? 'no indicator: no one can tell' : !Number.isFinite(s.target) ? 'no target stated' : s.reports ? 'no baseline to measure from' : 'nothing reported yet';
      out.push(`<rect class="ml-track none" x="${f1(colBar)}" y="${f1(y + 9)}" width="${f1(bw)}" height="7" fill="none" stroke="${PALETTE.faint}" stroke-width="0.8" stroke-dasharray="2 2" rx="1"/>`);
      out.push(`<text x="${f1(colBar)}" y="${f1(y + 30)}" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">${esc(fit(why, bw, SIZE.small))}</text>`);
    }
    // the state
    out.push(`<text class="ml-state" x="${f1(colState + 8)}" y="${f1(y + 18)}" font-size="${SIZE.text}" fill="${st.ink}">${esc(`${st.glyph} ${st.word}`)}</text>`);
    out.push('</g>');
  });
  out.push('</svg>');
  return out.join('');
}

export function svgBenefits(answer, options = {}) {
  const parties = answer?.benefits || [];
  const values = answer?.values || {};
  const names = options.names || {};
  const ground = options.ground || PALETTE.paper;
  const colW = options.column || 210, gap = 14;
  const rows = Math.max(1, ...parties.map((p) => (p.interests || []).length));
  const width = options.width || Math.max(420, 16 + parties.length * (colW + gap));
  const perRow = Math.max(1, Math.floor((width - 16) / (colW + gap)));
  const blockH = 64 + rows * 44;
  const lines = Math.max(1, Math.ceil(parties.length / perRow));
  const height = 24 + lines * (blockH + 12);
  const out = [];
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="ml-benefits" viewBox="0 0 ${f1(width)} ${f1(height)}" width="${f1(width)}" height="${f1(height)}" font-family="${esc(FONT)}" role="img" aria-label="Each party's benefit by its own criteria, never one number for all">`);
  out.push(`<rect class="ml-ground" x="0" y="0" width="${f1(width)}" height="${f1(height)}" fill="${ground}"/>`);
  out.push(`<text x="16" y="16" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">Each party by its own criteria, read under its own words</text>`);
  if (!parties.length) out.push(`<text x="16" y="44" font-size="${SIZE.text}" fill="${PALETTE.muted}" font-style="italic">No party is a member.</text>`);
  parties.forEach((p, i) => {
    const x = 16 + (i % perRow) * (colW + gap), y = 28 + Math.floor(i / perRow) * (blockH + 12);
    const sd = STANDINGS[p.standing] || STANDINGS.undecided;
    const chosen = options.selected === p.party;
    out.push(`<g class="ml-party${chosen ? ' chosen' : ''}" data-party="${esc(p.party)}">`);
    out.push(`<rect x="${f1(x)}" y="${f1(y)}" width="${colW}" height="${blockH}" fill="none" stroke="${chosen ? PALETTE.orchid : PALETTE.line}" stroke-width="${chosen ? 1.4 : 0.8}" rx="3"/>`);
    out.push(`<text x="${f1(x + 10)}" y="${f1(y + 18)}" font-size="${SIZE.head}" fill="${PALETTE.ink}">${esc(fit(names[p.party] || p.party, colW - 20, SIZE.head))}</text>`);
    out.push(`<text class="ml-standing ${esc(p.standing)}" x="${f1(x + 10)}" y="${f1(y + 35)}" font-size="${SIZE.text}" fill="${sd.ink}">${esc(`${sd.glyph} ${sd.word}`)}</text>`);
    const m = p.measure || {};
    const words = m.kind === 'probability' ? `P(all met) = ${figure(m.value)}` : m.kind === 'interval' ? `P(all met) in [${figure(m.lower)}, ${figure(m.upper)}]` : 'no probability';
    out.push(`<text x="${f1(x + 10)}" y="${f1(y + 50)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(words, colW - 20, SIZE.small))}</text>`);
    const vs = Object.fromEntries((values[p.party] || []).map((v) => [v.dimension, v]));
    const interests = p.interests || [];
    if (!interests.length) out.push(`<text x="${f1(x + 10)}" y="${f1(y + 76)}" font-size="${SIZE.small}" fill="${PALETTE.muted}" font-style="italic">${esc(fit('no criterion of its benefit declared', colW - 20, SIZE.small))}</text>`);
    interests.forEach((it, k) => {
      const yy = y + 66 + k * 44;
      const v = vs[it.dimension];
      const status = p.met.includes(it.dimension) ? 'met' : p.unmet.includes(it.dimension) ? 'unmet' : p.open.includes(it.dimension) ? 'open' : 'not observed';
      const glyph = { met: '●', unmet: '○', open: '◐', 'not observed': '?' }[status];
      out.push(`<g class="ml-interest ${esc(status.replace(' ', '-'))}" data-dimension="${esc(it.dimension)}"><title>${esc(it.dimension)}: asks ${esc(figure(it.threshold))}${it.reading ? ` (${esc(it.reading)}` : ''}${it.source ? `, by ${esc(it.source)})` : it.reading ? ')' : ''}; ${esc(status)}</title>`);
      out.push(`<text x="${f1(x + 10)}" y="${f1(yy)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(fit(`${glyph} ${it.dimension}: asks ${figure(it.threshold)}`, colW - 20, SIZE.small))}</text>`);
      const bx = x + 12, bw = colW - 24, by = yy + 9;
      if (!v || !Number.isFinite(v.lower ?? v.value)) {
        out.push(`<rect x="${f1(bx)}" y="${f1(by)}" width="${f1(bw)}" height="8" fill="none" stroke="${PALETTE.muted}" stroke-width="0.7" stroke-dasharray="2 2" rx="1"/>`);
        out.push(`<text x="${f1(bx + bw / 2)}" y="${f1(by + 7)}" text-anchor="middle" font-size="7.5" fill="${PALETTE.muted}" font-style="italic">not observed</text>`);
      } else {
        const lo = Math.min(v.lower ?? v.value, it.threshold), hi = Math.max(v.upper ?? v.value, it.threshold);
        const pad = (hi - lo || Math.abs(hi) || 1) * 0.2;
        const X = (z) => bx + ((z - (lo - pad)) / (hi - lo + 2 * pad)) * bw;
        out.push(`<path d="M${f1(bx)},${f1(by + 4)} H${f1(bx + bw)}" stroke="${PALETTE.line}" stroke-width="0.7"/>`);
        const ink = status === 'met' ? PALETTE.laurel : status === 'unmet' ? PALETTE.rose : PALETTE.sand;
        out.push(`<rect class="ml-allowed" x="${f1(X(v.lower ?? v.value))}" y="${f1(by)}" width="${f1(Math.max(1.5, X(v.upper ?? v.value) - X(v.lower ?? v.value)))}" height="8" fill="${ink}" fill-opacity="0.35" rx="1"/>`);
        if (Number.isFinite(v.value)) out.push(`<circle cx="${f1(X(v.value))}" cy="${f1(by + 4)}" r="2.3" fill="${ink}"/>`);
        out.push(`<path class="ml-threshold" d="M${f1(X(it.threshold))},${f1(by - 3)} V${f1(by + 11)}" stroke="${PALETTE.ink}" stroke-width="1.1"/>`);
      }
      out.push('</g>');
    });
    out.push('</g>');
  });
  out.push('</svg>');
  return out.join('');
}
