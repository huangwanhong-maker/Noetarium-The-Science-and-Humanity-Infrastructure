// The loom of a record of historical objects (ADR 0110): time as the warp, running left to right; strands side by
// side, one for each lane a work chooses (a discipline, a tradition, a kind); and the dependences as the weft, threads
// from what came before to what rested on it, most visible where they cross from one strand to another.
//
// History is dense where it is recent. The warp is therefore not ruled evenly: a year stands part by its date and
// part by its rank among the dates shown (warp), so that a crowded century opens and an empty one closes, and the
// ticks say what the scale has done. An object with no date is not on the loom, and is counted.
//
// layoutLoom is pure arithmetic; svgLoom returns markup (history.css gives the marks a default look).

import { ROLES, esc, glyphOf, textWidth, yearText } from './cone.js';

// A scale of time from years to [0, 1]: `evenness` of it by rank among the given years, the rest by the date itself.
export function warp(years, from, to, evenness = 0.5) {
  const sorted = [...new Set(years.filter((y) => y >= from && y <= to))].sort((a, b) => a - b);
  const knots = [from, ...sorted.filter((y) => y > from && y < to), to];
  const byRank = (year) => {
    if (knots.length < 3) return (year - from) / (to - from || 1);
    let i = 0;
    while (i < knots.length - 2 && knots[i + 1] < year) i++;
    const a = knots[i], b = knots[i + 1];
    return (i + (b > a ? (year - a) / (b - a) : 0)) / (knots.length - 1);
  };
  return (year) => {
    const y = Math.max(from, Math.min(to, year));
    return (1 - evenness) * ((y - from) / (to - from || 1)) + evenness * byRank(y);
  };
}

// Years to mark between two dates: at most `most`, on a step of 1, 2 or 5 times a power of ten.
export function ticks(from, to, most = 12) {
  const span = to - from;
  let step = 1;
  for (const s of [1, 2, 5, 10, 20, 25, 50, 100, 200, 250, 500, 1000, 2000]) { step = s; if (span / s <= most) break; }
  const out = [];
  for (let y = Math.ceil(from / step) * step; y <= to; y += step) out.push(y);
  return out;
}

const ROW = 24, PAD = 14, LEFT = 132;

// Where everything stands. view: { from, to, lanes: [{id, name}], objects: [{id, name, kind, year, lane, …}],
// threads: [{from, to, role, standing}] }; options: { width (1200), evenness (0.5) }.
export function layoutLoom(view, options = {}) {
  const width = options.width || 1200, right = 24, inner = width - LEFT - right;
  const dated = view.objects.filter((o) => typeof o.year === 'number' && o.year >= view.from && o.year <= view.to && view.lanes.some((l) => l.id === o.lane));
  const scale = warp(dated.map((o) => o.year), view.from, view.to, options.evenness ?? 0.5);
  const x = (year) => LEFT + scale(year) * inner;
  let y = 34;
  const lanes = [], marks = [];
  for (const lane of view.lanes) {
    const mine = dated.filter((o) => o.lane === lane.id).sort((a, b) => a.year - b.year);
    const rows = [];  // the right edge taken in each row
    for (const o of mine) {
      const w = 16 + textWidth(o.name, 12.5) + 8, ox = x(o.year);
      let row = rows.findIndex((edge) => edge + 6 <= ox);
      if (row < 0) { row = rows.length; rows.push(0); }
      rows[row] = ox + w;
      marks.push({ ...o, x: ox, y: y + PAD + row * ROW + ROW / 2, w, row });
    }
    const height = PAD * 2 + Math.max(1, rows.length) * ROW;
    lanes.push({ ...lane, y, height, count: mine.length });
    y += height;
  }
  const at = new Map(marks.map((m) => [m.id, m]));
  const threads = [];
  for (const t of view.threads || []) {
    const a = at.get(t.from), b = at.get(t.to);
    if (a && b) threads.push({ ...t, a, b, across: a.lane !== b.lane });
  }
  const axis = ticks(view.from, view.to).map((year) => ({ year, x: x(year) }));
  return { width, height: y + 30, lanes, marks, threads, axis, x, undated: view.objects.length - dated.length };
}

// The loom as SVG markup. options.selected: an object's id; options.lit: ids to keep bright (a cone), the rest standing
// back; options.now: a year at which a line is drawn (the clock).
export function svgLoom(view, options = {}) {
  const L = layoutLoom(view, options);
  const lit = options.lit ? new Set(options.lit) : null;
  const parts = [`<svg xmlns="http://www.w3.org/2000/svg" class="h-loom${lit ? ' quiet' : ''}" viewBox="0 0 ${L.width} ${L.height.toFixed(0)}" width="${L.width}" height="${L.height.toFixed(0)}" role="img" aria-label="The loom of the record">`];
  for (const t of L.axis) {
    parts.push(`<line class="h-tick" x1="${t.x.toFixed(1)}" y1="26" x2="${t.x.toFixed(1)}" y2="${(L.height - 26).toFixed(1)}"/>` +
      `<text class="h-year" x="${t.x.toFixed(1)}" y="18" text-anchor="middle">${esc(yearText(t.year))}</text>`);
  }
  L.lanes.forEach((lane, i) => {
    parts.push(`<g class="h-lane${i % 2 ? ' odd' : ''}" data-lane="${esc(lane.id)}"><rect x="0" y="${lane.y.toFixed(1)}" width="${L.width}" height="${lane.height.toFixed(1)}"/>` +
      `<text class="h-lane-name" x="14" y="${(lane.y + 22).toFixed(1)}">${esc(lane.name)}</text><text class="h-lane-count" x="14" y="${(lane.y + 38).toFixed(1)}">${lane.count}</text></g>`);
  });
  for (const t of L.threads) {
    const x1 = t.a.x + 6, y1 = t.a.y, x2 = t.b.x - 2, y2 = t.b.y, mid = (x1 + x2) / 2, lift = t.across ? 0 : -Math.min(26, Math.abs(x2 - x1) * 0.2 + 8);
    const on = !lit || (lit.has(t.from) && lit.has(t.to));
    const hue = ROLES[t.role]?.hue;
    parts.push(`<path class="h-thread s-${esc(t.standing || 'proposed')}${t.across ? ' across' : ''}${on ? '' : ' back'}" data-from="${esc(t.from)}" data-to="${esc(t.to)}"${hue ? ` style="stroke:${hue}"` : ''} ` +
      `d="M${x1.toFixed(1)} ${y1.toFixed(1)}C${mid.toFixed(1)} ${(y1 + lift).toFixed(1)} ${mid.toFixed(1)} ${(y2 + lift).toFixed(1)} ${x2.toFixed(1)} ${y2.toFixed(1)}"/>`);
  }
  for (const m of L.marks) {
    const cls = ['h-mark', m.id === options.selected ? 'selected' : '', lit && !lit.has(m.id) ? 'back' : '', m.standing ? `s-${m.standing}` : ''].filter(Boolean).join(' ');
    parts.push(`<g class="${cls}" data-object="${esc(m.id)}" transform="translate(${m.x.toFixed(1)} ${m.y.toFixed(1)})"><title>${esc(`${m.name}, ${yearText(m.year, m.approximate)}${m.standing ? ` · ${m.standing}` : ''}`)}</title>` +
      `<rect class="h-hit" x="-4" y="-10" width="${(m.w + 2).toFixed(1)}" height="20" rx="2"/><text class="h-glyph" x="2" y="4" text-anchor="middle">${esc(glyphOf(m))}</text><text class="h-name" x="12" y="4">${esc(m.name)}</text></g>`);
  }
  if (typeof options.now === 'number' && options.now >= view.from && options.now <= view.to) {
    const nx = L.x(options.now);
    parts.push(`<line class="h-now" x1="${nx.toFixed(1)}" y1="24" x2="${nx.toFixed(1)}" y2="${(L.height - 22).toFixed(1)}"/><text class="h-now-year" x="${nx.toFixed(1)}" y="${(L.height - 8).toFixed(1)}" text-anchor="middle">${esc(yearText(options.now))}</text>`);
  }
  if (L.undated) parts.push(`<text class="h-silent" x="${L.width - 24}" y="${(L.height - 8).toFixed(1)}" text-anchor="end">${L.undated} without a date, or outside these years or strands, not on the loom</text>`);
  parts.push('</svg>');
  return parts.join('');
}
