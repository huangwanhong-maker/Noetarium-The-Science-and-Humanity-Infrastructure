// A decision rule's a priori power, and ideal points on a line (ADR 0132): markup that needs no page.
//
// svgPower(answer, options) takes the engine's answer to `rule`. Each member is a row with its three measures side by side, each a bar
// on its own scale beside its figure: Penrose's (the probability of being decisive), Banzhaf's (the swings' share) and Shapley and
// Shubik's (the share of orders in which it is pivotal). A veto and a dummy are said in words. Members who stand alike (equal on every
// measure) are drawn once, with how many they are, so that a body of 193 equal members is one row. A rule's power is a measure of the
// rule, never of influence: the footer says so.
//
// svgLine(answer, options) takes the engine's answer to `ideal_points`: the members on one line, each a dot with its interval when the
// bootstrap was asked, names set above and below in as many rows as they need so that no two meet; the anchor marked. The line is a
// summary of how members divided, under one dimension, not a measure of what they want: the footer says so.

import { FONT, PALETTE, esc, f1, fit, figure, textWidth } from './common.js';

const SIZE = { text: 10.5, small: 9.5, head: 9.5 };

export function svgPower(answer, options = {}) {
  const names = options.names || {};
  const ground = options.ground || PALETTE.paper;
  const width = options.width || 760;
  const vetoes = new Set(answer?.properties?.vetoes || []), dummies = new Set(answer?.properties?.dummies || []);
  // members alike on every measure drawn once
  const groups = [];
  for (const p of answer?.power || []) {
    const key = [p.penrose, p.banzhaf, p.shapley_shubik].map((v) => (Number.isFinite(v) ? v.toPrecision(12) : '-')).join('|') + (vetoes.has(p.member) ? 'v' : '') + (dummies.has(p.member) ? 'd' : '');
    const g = groups.find((x) => x.key === key);
    if (g) g.members.push(p.member); else groups.push({ key, members: [p.member], p });
  }
  const measures = [['penrose', 'Penrose: decisive'], ['banzhaf', 'Banzhaf: share of swings'], ['shapley_shubik', 'Shapley–Shubik: pivotal']];
  const most = Object.fromEntries(measures.map(([k]) => [k, Math.max(1e-300, ...(answer?.power || []).map((p) => p[k] || 0))]));
  const nameW = Math.min(240, width * 0.3), colW = (width - nameW - 40) / 3;
  const rowH = 22, top = 34;
  const height = top + rowH * Math.max(1, groups.length) + 44;
  const out = [];
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="mp-power" viewBox="0 0 ${f1(width)} ${f1(height)}" width="${f1(width)}" height="${f1(height)}" font-family="${esc(FONT)}" role="img" aria-label="A decision rule's a priori power, member by member">`);
  out.push(`<rect x="0" y="0" width="${f1(width)}" height="${f1(height)}" fill="${ground}"/>`);
  measures.forEach(([, label], k) => out.push(`<text x="${f1(nameW + 24 + k * colW)}" y="20" font-size="${SIZE.head}" fill="${PALETTE.muted}" font-style="italic">${esc(label)}</text>`));
  groups.forEach((g, i) => {
    const y = top + i * rowH;
    const who = g.members.length === 1 ? names[g.members[0]] || g.members[0] : `${names[g.members[0]] || g.members[0]} and ${g.members.length - 1} alike`;
    const tags = [vetoes.has(g.members[0]) ? 'veto' : '', dummies.has(g.members[0]) ? 'dummy' : ''].filter(Boolean).join(', ');
    out.push(`<g class="mp-member" data-member="${esc(g.members[0])}" data-count="${g.members.length}"><title>${esc(g.members.map((m) => names[m] || m).join(', '))}</title>`);
    out.push(`<text x="16" y="${f1(y + 13)}" font-size="${SIZE.text}" fill="${PALETTE.ink}">${esc(fit(who, nameW - (tags ? textWidth(tags, SIZE.small) + 14 : 4), SIZE.text))}</text>`);
    if (tags) out.push(`<text x="${f1(nameW + 4)}" y="${f1(y + 13)}" text-anchor="end" font-size="${SIZE.small}" fill="${vetoes.has(g.members[0]) ? PALETTE.orchid : PALETTE.muted}" font-style="italic">${esc(tags)}</text>`);
    measures.forEach(([k], j) => {
      const x = nameW + 24 + j * colW, w = colW - 64;
      const v = g.p[k] || 0;
      out.push(`<rect x="${f1(x)}" y="${f1(y + 5)}" width="${f1(Math.max(0.5, (w * v) / most[k]))}" height="9" fill="${PALETTE.laurel}" fill-opacity="${dummies.has(g.members[0]) ? 0.15 : 0.55}" rx="1"/>`);
      out.push(`<text x="${f1(x + w + 6)}" y="${f1(y + 13)}" font-size="${SIZE.small}" fill="${PALETTE.ink}">${esc(figure(v))}</text>`);
    });
    out.push('</g>');
  });
  const act = Number.isFinite(answer?.power_to_act) ? `The rule lets the body act on ${figure(100 * answer.power_to_act)}% of the coalitions that could form (Coleman). ` : '';
  out.push(`<text x="16" y="${f1(height - 24)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(`${act}${answer?.route ? `Computed by ${answer.route}.` : ''}`, width - 32, SIZE.small))}</text>`);
  out.push(`<text x="16" y="${f1(height - 10)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit('Power here is the rule’s alone, before any preference is known: never a measure of influence.', width - 32, SIZE.small))}</text>`);
  out.push('</svg>');
  return out.join('');
}

export function svgLine(answer, options = {}) {
  const names = options.names || {};
  const ground = options.ground || PALETTE.paper;
  const width = options.width || 860;
  const pts = [...(answer?.points || [])].filter((p) => Number.isFinite(p.x)).sort((a, b) => a.x - b.x);
  const lo = Math.min(-2, ...pts.map((p) => (Number.isFinite(p.lower) ? p.lower : p.x))), hi = Math.max(2, ...pts.map((p) => (Number.isFinite(p.upper) ? p.upper : p.x)));
  const left = 24, right = 24;
  const X = (v) => left + ((v - lo) / (hi - lo)) * (width - left - right);
  // names in rows above and below the line, each taking the first row where it meets no other
  const rowsAbove = [], rowsBelow = [];
  const placed = pts.map((p, i) => {
    const text = names[p.member] || p.member;
    const w = textWidth(text, SIZE.small) + 8;
    const x0 = X(p.x) - w / 2, x1 = X(p.x) + w / 2;
    const rows = i % 2 ? rowsBelow : rowsAbove;
    let r = 0;
    while (rows[r] !== undefined && rows[r] > x0) r++;
    rows[r] = x1;
    return { p, text, row: r, above: !(i % 2) };
  });
  const nAbove = rowsAbove.length, nBelow = rowsBelow.length;
  const axisY = 30 + 13 * nAbove + 14;
  const height = axisY + 22 + 13 * nBelow + 34;
  const out = [];
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="mp-line" viewBox="0 0 ${f1(width)} ${f1(height)}" width="${f1(width)}" height="${f1(height)}" font-family="${esc(FONT)}" role="img" aria-label="Members' ideal points on one line, with their intervals">`);
  out.push(`<rect x="0" y="0" width="${f1(width)}" height="${f1(height)}" fill="${ground}"/>`);
  out.push(`<path d="M${left},${f1(axisY)} H${f1(width - right)}" stroke="${PALETTE.line}" stroke-width="0.8"/>`);
  for (let v = Math.ceil(lo); v <= Math.floor(hi); v++) out.push(`<path d="M${f1(X(v))},${f1(axisY - 3)} V${f1(axisY + 3)}" stroke="${PALETTE.faint}"/><text x="${f1(X(v))}" y="${f1(axisY + 14)}" text-anchor="middle" font-size="8" fill="${PALETTE.muted}">${v}</text>`);
  for (const q of placed) {
    const { p } = q;
    const x = X(p.x), anchor = p.member === answer?.anchor;
    const ly = q.above ? axisY - 14 - 13 * q.row : axisY + 26 + 13 * q.row;
    out.push(`<g class="mp-point${anchor ? ' anchor' : ''}${options.selected === p.member ? ' chosen' : ''}" data-member="${esc(p.member)}"><title>${esc(q.text)}: ${esc(figure(p.x))}${Number.isFinite(p.lower) ? ` [${esc(figure(p.lower))}, ${esc(figure(p.upper))}]` : ''}, ${p.votes} votes</title>`);
    if (Number.isFinite(p.lower)) out.push(`<path d="M${f1(X(p.lower))},${f1(axisY)} H${f1(X(p.upper))}" stroke="${PALETTE.orchid}" stroke-width="3" stroke-opacity="0.25"/>`);
    out.push(`<path d="M${f1(x)},${f1(axisY)} V${f1(q.above ? ly + 2 : ly - 9)}" stroke="${PALETTE.faint}" stroke-width="0.5"/>`);
    out.push(`<circle cx="${f1(x)}" cy="${f1(axisY)}" r="${anchor ? 3.4 : 2.6}" fill="${anchor ? PALETTE.orchid : PALETTE.ink}"/>`);
    out.push(`<text x="${f1(x)}" y="${f1(ly)}" text-anchor="middle" font-size="${SIZE.small}" fill="${options.selected === p.member ? PALETTE.orchid : PALETTE.ink}">${esc(q.text)}</text></g>`);
  }
  const fitted = Number.isFinite(answer?.classified) ? `The line classifies ${figure(100 * answer.classified)}% of the votes; APRE ${figure(answer.apre)}. ` : '';
  out.push(`<text x="${left}" y="${f1(height - 10)}" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(fit(`${fitted}Votes record public positions, not preferences: the line summarises how members divided, under one dimension.`, width - 48, SIZE.small))}</text>`);
  out.push('</svg>');
  return out.join('');
}

// svgBargaining(answer, options) takes the engine's answer to `bargaining`: the Pareto frontier at or above the disagreement point, the
// disagreement point and the ideal point, and the three solutions, each a mark and a name (Nash ◆, Kalai–Smorodinsky ▲, egalitarian ●),
// with the segment from the disagreement point to the ideal point on which Kalai and Smorodinsky's lies. options.parties names the axes.
export function svgBargaining(answer, options = {}) {
  const ground = options.ground || PALETTE.paper;
  const width = options.width || 420, height = options.height || 360;
  const [nameA, nameB] = options.parties || ['party A', 'party B'];
  const pts = [...(answer?.frontier || []), answer?.disagreement, answer?.ideal, answer?.nash].filter((p) => Array.isArray(p) && p.every(Number.isFinite));
  const xs = pts.map((p) => p[0]), ys = pts.map((p) => p[1]);
  const x0 = Math.min(0, ...xs), x1 = Math.max(...xs, x0 + 1), y0 = Math.min(0, ...ys), y1 = Math.max(...ys, y0 + 1);
  const left = 48, bottom = 40, top = 16, right = 16;
  const X = (v) => left + ((v - x0) / (x1 - x0)) * (width - left - right) * 0.92;
  const Y = (v) => height - bottom - ((v - y0) / (y1 - y0)) * (height - top - bottom) * 0.92;
  const out = [];
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="mp-bargaining" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" font-family="${esc(FONT)}" role="img" aria-label="A bargaining problem: its frontier and three solutions">`);
  out.push(`<rect x="0" y="0" width="${width}" height="${height}" fill="${ground}"/>`);
  out.push(`<path d="M${left},${top} V${height - bottom} H${width - right}" fill="none" stroke="${PALETTE.line}" stroke-width="0.8"/>`);
  out.push(`<text x="${(left + width - right) / 2}" y="${height - 10}" text-anchor="middle" font-size="${SIZE.small}" fill="${PALETTE.muted}">${esc(nameA)}'s utility →</text>`);
  out.push(`<text x="14" y="${(top + height - bottom) / 2}" text-anchor="middle" font-size="${SIZE.small}" fill="${PALETTE.muted}" transform="rotate(-90 14 ${(top + height - bottom) / 2})">${esc(nameB)}'s utility →</text>`);
  const f = answer?.frontier || [];
  if (f.length) out.push(`<path class="mp-frontier" d="${f.map((p, i) => `${i ? 'L' : 'M'}${f1(X(p[0]))},${f1(Y(p[1]))}`).join(' ')}" fill="none" stroke="${PALETTE.laurel}" stroke-width="1.8"/>`);
  const d = answer?.disagreement, I = answer?.ideal;
  if (d && I) out.push(`<path d="M${f1(X(d[0]))},${f1(Y(d[1]))} L${f1(X(I[0]))},${f1(Y(I[1]))}" stroke="${PALETTE.faint}" stroke-width="0.8" stroke-dasharray="3 3"/>`);
  // the marks, and their names set to the right, moved down where two would meet (solutions often coincide)
  const marks = [[d, '○', 'disagreement', PALETTE.muted], [I, '✶', 'ideal', PALETTE.muted], [answer?.nash, '◆', 'Nash', PALETTE.orchid],
    [answer?.kalai_smorodinsky, '▲', 'Kalai–Smorodinsky', PALETTE.ink], [answer?.egalitarian, '●', 'egalitarian', PALETTE.laurel]].filter(([p]) => Array.isArray(p) && p.every(Number.isFinite));
  const placed = [];
  for (const [p, glyph, word, ink] of marks) {
    const x = X(p[0]), y = Y(p[1]);
    let ly = y + 4;
    while (placed.some((q) => Math.abs(q.x - x) < 90 && Math.abs(q.ly - ly) < 12)) ly += 12;
    placed.push({ x, ly });
    const together = marks.filter(([q]) => q !== p && Math.hypot(X(q[0]) - x, Y(q[1]) - y) < 3).map(([, , w]) => w);
    out.push(`<g class="mp-solution" data-solution="${esc(word)}"><title>${esc(word)}: (${figure(p[0])}, ${figure(p[1])})${together.length ? `, with ${esc(together.join(' and '))}` : ''}</title>`
      + `<text x="${f1(x)}" y="${f1(y + 4)}" text-anchor="middle" font-size="12" fill="${ink}">${glyph}</text>`
      + (ly !== y + 4 ? `<path d="M${f1(x + 5)},${f1(y)} L${f1(x + 8)},${f1(ly - 3.5)}" stroke="${PALETTE.faint}" stroke-width="0.6"/>` : '')
      + `<text x="${f1(x + 10)}" y="${f1(ly)}" font-size="${SIZE.small}" fill="${ink}">${esc(word)}</text></g>`);
  }
  out.push('</svg>');
  return out.join('');
}
