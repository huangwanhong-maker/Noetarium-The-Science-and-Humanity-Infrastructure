// A field on a grid of cells, drawn (ADR 0124): its values as tones, its change on a diverging map, its currents as
// arrows, marks on cells; of no work. A field is a value per kept cell of a grid, the cells in row-major order of a mask
// (rows from the north), as kinesis's fields and the API's population composition answer. Everything here but
// paintField is pure and returns numbers or SVG text, so that it is checked in Node; paintField paints a canvas.
//
// Tones: a magnitude on a sequential map (orchid by default: pale for few, deep violet for many; `reverse` for a dark
// ground), on a logarithmic scale where the values span orders of
// magnitude (people per km² from one to fifteen thousand), its ticks at 1, 3, 10, 30 …; a change on the diverging
// lavender-rust map, symmetric about zero, lavender for less and rust for more. Currents: the vector at each cell summed
// over blocks of b × b cells, drawn from the block's middle with a length proportional to the square root of its size,
// so that small currents stay visible beside large ones (the legend says so).

import { cssColour } from '../../styles/colour.js';
import { colourMap } from '../../styles/palettes.js';

const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
const fmt = (v) => (Math.abs(v) >= 1000 ? `${Math.round(v / 100) / 10}k` : Math.abs(v) >= 10 ? `${Math.round(v)}` : `${Math.round(v * 10) / 10}`);

// The kept cells of a mask, in the order of a field's values.
export function cellsOf(rows, cols, land) {
  const out = [];
  for (let i = 0; i < rows; i++) for (let j = 0; j < cols; j++) if (!land || land[i][j]) out.push({ row: i, col: j });
  return out;
}

// A scale for a field's values: at(v) in [0, 1], with its ticks. log: on log10(1 + v).
export function scaleOf(values, { log = true, lo, hi } = {}) {
  let a = lo ?? Infinity, b = hi ?? -Infinity;
  if (lo === undefined || hi === undefined)
    for (const v of values) if (Number.isFinite(v)) { if (lo === undefined) a = Math.min(a, v); if (hi === undefined) b = Math.max(b, v); }
  if (!Number.isFinite(a)) a = 0;
  if (!Number.isFinite(b) || b <= a) b = a + 1;
  const f = log ? (v) => Math.log10(1 + Math.max(0, v)) : (v) => v;
  const fa = f(Math.max(a, 0)), fb = f(b);
  const at = (v) => (Number.isFinite(v) ? Math.min(1, Math.max(0, (f(v) - fa) / (fb - fa || 1))) : NaN);
  const ticks = [];
  if (log) {
    for (let d = 1; d <= b * 1.0001; d *= 10) for (const m of [1, 3]) if (d * m >= a && d * m <= b * 1.0001) ticks.push(d * m);
    if (a <= 0) ticks.unshift(0);
  } else {
    const step = 10 ** Math.floor(Math.log10((b - a) / 4 || 1));
    for (let v = Math.ceil(a / step) * step; v <= b + 1e-9; v += step) ticks.push(Math.round(v / step) * step);
  }
  return { lo: a, hi: b, log, at, ticks };
}

// A symmetric scale for a change: at(v) in [−1, 1], on a signed square root so that small changes show.
export function changeScaleOf(values, { most } = {}) {
  let m = most ?? 0;
  if (most === undefined) for (const v of values) if (Number.isFinite(v)) m = Math.max(m, Math.abs(v));
  if (!(m > 0)) m = 1;
  const at = (v) => (Number.isFinite(v) ? Math.sign(v) * Math.sqrt(Math.min(1, Math.abs(v) / m)) : NaN);
  return { most: m, at, ticks: [-m, -m / 4, 0, m / 4, m] };
}

// Each value's colour: a magnitude on a sequential map, or a change (diverging = true) on a diverging one.
export function coloursOf(values, scale, { map, diverging = false, reverse = false, none = '#00000000' } = {}) {
  const m = colourMap(map || (diverging ? 'lavender-rust' : 'orchid'));
  return values.map((v) => {
    const u = scale.at(v);
    if (!Number.isFinite(u)) return none;
    return cssColour(diverging ? m.at(u) : m.colour(reverse ? 1 - u : u));
  });
}

// The change from one field to another, cell by cell.
export const changeOf = (a, b) => b.map((v, i) => v - a[i]);

// Arrows of a field's currents, summed over blocks of b × b cells: each {x, y, dx, dy, size} in cells (x east, y south),
// its length up to `reach` cells, proportional to √size over √(the largest size).
export function arrowsOf({ rows, cols, land, east, north, block = 3, reach, least = 0 }) {
  const cells = cellsOf(rows, cols, land);
  const nb = Math.ceil(rows / block), mb = Math.ceil(cols / block);
  const sx = new Float64Array(nb * mb), sy = new Float64Array(nb * mb), count = new Int32Array(nb * mb);
  cells.forEach((c, k) => {
    const b = Math.floor(c.row / block) * mb + Math.floor(c.col / block);
    sx[b] += east[k] || 0, sy[b] += north[k] || 0, count[b]++;
  });
  let top = 0;
  for (let b = 0; b < sx.length; b++) top = Math.max(top, Math.hypot(sx[b], sy[b]));
  const r = reach ?? block * 0.95;
  const out = [];
  for (let b = 0; b < sx.length; b++) {
    const size = Math.hypot(sx[b], sy[b]);
    if (!count[b] || size <= least || top === 0) continue;
    const len = r * Math.sqrt(size / top);
    const bi = Math.floor(b / mb), bj = b % mb;
    out.push({ x: (bj + 0.5) * block, y: (bi + 0.5) * block, dx: (sx[b] / size) * len, dy: (-sy[b] / size) * len, size });
  }
  return { arrows: out, largest: top, block };
}

function arrowSvg(a, cell, cls) {
  const x0 = a.x * cell - (a.dx * cell) / 2, y0 = a.y * cell - (a.dy * cell) / 2;
  const x1 = x0 + a.dx * cell, y1 = y0 + a.dy * cell;
  const L = Math.hypot(x1 - x0, y1 - y0);
  if (L < 0.5) return '';
  const ux = (x1 - x0) / L, uy = (y1 - y0) / L, h = Math.min(5, L * 0.45);
  const hx1 = x1 - ux * h - uy * h * 0.55, hy1 = y1 - uy * h + ux * h * 0.55, hx2 = x1 - ux * h + uy * h * 0.55, hy2 = y1 - uy * h - ux * h * 0.55;
  return `<path class="${cls}" d="M${x0.toFixed(1)} ${y0.toFixed(1)}L${x1.toFixed(1)} ${y1.toFixed(1)}M${hx1.toFixed(1)} ${hy1.toFixed(1)}L${x1.toFixed(1)} ${y1.toFixed(1)}L${hx2.toFixed(1)} ${hy2.toFixed(1)}"/>`;
}

// The arrows and marks alone, as an SVG layer of `cols × cell` by `rows × cell` (to lie over a painted canvas).
export function overlaySvg({ rows, cols, cell = 6, arrows = [], marks = [], what = 'currents of a field' }) {
  const w = cols * cell, h = rows * cell;
  const parts = arrows.map((a) => arrowSvg(a, cell, 'fd-arrow'));
  for (const m of marks) {
    const cx = (m.col + 0.5) * cell, cy = (m.row + 0.5) * cell;
    parts.push(`<circle class="fd-mark${m.kind ? ` fd-mark-${esc(m.kind)}` : ''}" cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="${(m.r ?? 2.2) * cell / 2}"/>`);
    if (m.label) parts.push(`<text class="fd-mark-label" x="${(cx + cell * 1.6).toFixed(1)}" y="${(cy + 4).toFixed(1)}">${esc(m.label)}</text>`);
  }
  return `<svg class="fd-overlay" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} ${h}" width="${w}" height="${h}" role="img" aria-label="${esc(what)}">${parts.join('')}</svg>`;
}

// A whole field as SVG: each kept cell a square in its tone, the rest the ground, and arrows and marks above.
export function fieldSvg({ rows, cols, land, colours, cell = 6, arrows = [], marks = [], what = 'a field on a grid' }) {
  const w = cols * cell, h = rows * cell;
  const cells = cellsOf(rows, cols, land);
  if (colours.length !== cells.length) throw new Error(`fieldSvg: ${colours.length} colours for ${cells.length} cells`);
  const rects = cells.map((c, k) => `<rect x="${c.col * cell}" y="${c.row * cell}" width="${cell}" height="${cell}" fill="${colours[k]}"/>`);
  const over = arrows.map((a) => arrowSvg(a, cell, 'fd-arrow'));
  for (const m of marks) over.push(`<circle class="fd-mark" cx="${((m.col + 0.5) * cell).toFixed(1)}" cy="${((m.row + 0.5) * cell).toFixed(1)}" r="${(m.r ?? 2.2) * cell / 2}"/>`);
  return `<svg class="fd-field" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} ${h}" width="${w}" height="${h}" role="img" aria-label="${esc(what)}">`
    + `<rect class="fd-ground" width="${w}" height="${h}"/><g shape-rendering="crispEdges">${rects.join('')}</g>${over.join('')}</svg>`;
}

// Paints a field on a canvas: each kept cell a square in its colour, the rest left as the ground (browser only).
export function paintField(ctx, { rows, cols, land, colours, cell = 6, ground = null }) {
  if (ground) { ctx.fillStyle = ground; ctx.fillRect(0, 0, cols * cell, rows * cell); } else ctx.clearRect(0, 0, cols * cell, rows * cell);
  const cells = cellsOf(rows, cols, land);
  cells.forEach((c, k) => {
    ctx.fillStyle = colours[k];
    ctx.fillRect(c.col * cell, c.row * cell, cell + 0.5, cell + 0.5);
  });
}

// A legend of a scale, as SVG: the ramp with its ticks and a label.
export function legendSvg({ scale, map, diverging = false, reverse = false, width = 260, label = '', what = 'the legend of a field' }) {
  const m = colourMap(map || (diverging ? 'lavender-rust' : 'orchid'));
  const h = 34, bar = 9, n = 48;
  const stops = [];
  for (let k = 0; k <= n; k++) {
    const u = k / n;
    stops.push(`<stop offset="${(u * 100).toFixed(1)}%" stop-color="${cssColour(diverging ? m.at(2 * u - 1) : m.colour(reverse ? 1 - u : u))}"/>`);
  }
  const id = `fdg${Math.random().toString(36).slice(2, 8)}`;
  const pad = 16;  // room for the end labels
  const pos = (v) => (diverging ? ((scale.at(v) + 1) / 2) : scale.at(v)) * (width - 2 * pad) + pad;
  const ticks = (scale.ticks || []).map((v) => `<line class="fd-tick" x1="${pos(v).toFixed(1)}" x2="${pos(v).toFixed(1)}" y1="${bar + 12}" y2="${bar + 15}"/><text class="fd-tick-label" x="${pos(v).toFixed(1)}" y="${h}" text-anchor="middle">${diverging && v > 0 ? '+' : ''}${fmt(v)}</text>`);
  return `<svg class="fd-legend" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${h + 2}" width="${width}" height="${h + 2}" role="img" aria-label="${esc(what)}">`
    + `<defs><linearGradient id="${id}">${stops.join('')}</linearGradient></defs>`
    + `<text class="fd-legend-label" x="${pad}" y="9">${esc(label)}</text>`
    + `<rect x="${pad}" y="12" width="${width - 2 * pad}" height="${bar}" fill="url(#${id})"/>${ticks.join('')}</svg>`;
}
