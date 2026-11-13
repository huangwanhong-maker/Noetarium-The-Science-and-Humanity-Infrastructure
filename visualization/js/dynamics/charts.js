// Drawings of dynamical systems, as markup that needs no page (ADR 0103): each function returns an SVG as text,
// so that a work puts it where it likes, a check reads it without a browser, and a figure can be saved as it is.
//
//   lineChart     series against a common axis, or one component against another (a phase portrait); points as
//                 dots for the iterates of a map or the points of a diagram; marks for events and levels
//   heatMap       a function on a grid: a magnitude (a landscape) or a label (basins), with its legend
//   complexPlane  eigenvalues and rates in the plane, with the axis or the circle that divides stable from not
//   braidDiagram  a braid word as strands that cross, the strand that passes over drawn whole
//   formsGraph    the forms a rewriting system reaches, by length, with the steps between them
//
// Colour. A magnitude takes one of the visualization layer's sequential maps, and a label one of its ten
// categorical hues in the order that keeps neighbours apart in every vision (styles/palettes.js): those are
// pastels, grounds for a cell of a map. A line is an ink, and takes one of the ten INKS below. Every series is
// also named in a legend, since hue alone does not carry identity. Ink, hairlines and ground come from the page's
// CSS variables (--d-ink, --d-faint, --d-line, --d-ground), so that a drawing follows the page by day and by night.

import { cssColour } from '../../styles/colour.js';
import { colourMap } from '../../styles/palettes.js';

const HUES = colourMap('categorical-apart').stops;
// Ten inks for lines: rose, sky, olive, ochre, indigo, sage, umber, lilac, teal, violet. Each holds a contrast of
// 3 against the paper and against the night (WCAG's floor for a graphic), so one set serves both. They are in the
// order in which each is farthest, in its worst vision (normal, protan, deutan, tritan by Machado's matrices),
// from those before it: the first three differ pairwise by at least 0.11 in OKLab in every vision, the first four
// by 0.08, all ten by 0.039 (the checks measure it). Found by a greedy search over OKLCh, the rose given.
export const INKS = ['#b8577a', '#1f99c7', '#617209', '#c18434', '#5469b4', '#6e9a65', '#8c654f', '#917ca2', '#1c8580', '#7f5f96'];
export const hue = (i, ink = true) => (ink ? INKS : HUES)[((i % 10) + 10) % 10];

const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
const fmt = (v) => {
  if (!Number.isFinite(v)) return '';
  const a = Math.abs(v);
  if (a !== 0 && (a < 1e-3 || a >= 1e5)) return v.toExponential(1).replace('e+', 'e');
  return String(Number(v.toPrecision(4)));
};

// About n round values that span [lo, hi].
export function ticks(lo, hi, n = 5) {
  if (!(hi > lo)) return [lo];
  const raw = (hi - lo) / n, power = 10 ** Math.floor(Math.log10(raw));
  const step = [1, 2, 5, 10].map((m) => m * power).find((s) => s >= raw) ?? raw;
  const out = [];
  for (let v = Math.ceil(lo / step - 1e-9) * step; v <= hi + 1e-9 * step; v += step) out.push(Math.abs(v) < step * 1e-9 ? 0 : v);
  return out;
}

const extent = (arrays) => {
  let lo = Infinity, hi = -Infinity;
  for (const a of arrays) for (const v of a) if (Number.isFinite(v)) { if (v < lo) lo = v; if (v > hi) hi = v; }
  if (!Number.isFinite(lo)) return [0, 1];
  if (lo === hi) { const pad = Math.abs(lo) > 0 ? Math.abs(lo) * 0.1 : 1; return [lo - pad, hi + pad]; }
  const pad = (hi - lo) * 0.04;
  return [lo - pad, hi + pad];
};

// The frame of a chart: its axes, ticks and labels, and the scales from values to the page.
function frame({ width, height, xRange, yRange, xLabel, yLabel, legend = 0, equal = false }) {
  const m = { l: 54, r: 12 + legend, t: 10, b: 38 };
  let [x0, x1] = xRange, [y0, y1] = yRange;
  const w = width - m.l - m.r, h = height - m.t - m.b;
  if (equal) {  // one unit of x is one unit of y on the page
    const scale = Math.max((x1 - x0) / w, (y1 - y0) / h), cx = (x0 + x1) / 2, cy = (y0 + y1) / 2;
    x0 = cx - scale * w / 2, x1 = cx + scale * w / 2, y0 = cy - scale * h / 2, y1 = cy + scale * h / 2;
  }
  const sx = (v) => m.l + (v - x0) / (x1 - x0) * w, sy = (v) => m.t + h - (v - y0) / (y1 - y0) * h;
  let g = `<rect class="d-plot" x="${m.l}" y="${m.t}" width="${w}" height="${h}"/>`;
  for (const v of ticks(x0, x1, Math.max(3, Math.round(w / 90)))) {
    g += `<path class="d-grid" d="M${sx(v).toFixed(1)} ${m.t}v${h}"/><text class="d-tick" x="${sx(v).toFixed(1)}" y="${m.t + h + 14}" text-anchor="middle">${fmt(v)}</text>`;
  }
  for (const v of ticks(y0, y1, Math.max(3, Math.round(h / 50)))) {
    g += `<path class="d-grid" d="M${m.l} ${sy(v).toFixed(1)}h${w}"/><text class="d-tick" x="${m.l - 6}" y="${(sy(v) + 3.5).toFixed(1)}" text-anchor="end">${fmt(v)}</text>`;
  }
  if (xLabel) g += `<text class="d-label" x="${m.l + w / 2}" y="${height - 6}" text-anchor="middle">${esc(xLabel)}</text>`;
  if (yLabel) g += `<text class="d-label" transform="translate(13 ${m.t + h / 2}) rotate(-90)" text-anchor="middle">${esc(yLabel)}</text>`;
  return { g, sx, sy, m, w, h, x0, x1, y0, y1 };
}

// A drawing fills the width it is given up to the width it was made for, and is not drawn larger: its type keeps its size.
const open = (width, height, what) => `<svg class="d-chart" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${height}" style="max-width:${width}px" role="img" aria-label="${esc(what)}">`;

function legendOf(names, x, y) {
  let g = '';
  names.forEach((n, i) => {
    g += `<path d="M${x} ${y + 16 * i}h14" stroke="${n.colour}" stroke-width="2.2" fill="none"/><text class="d-legend" x="${x + 19}" y="${y + 16 * i + 3.5}">${esc(n.name)}</text>`;
  });
  return g;
}

// Series: [{name, x, y, colour?, dots?}]. marks: [{x?, y?, label?}] verticals and horizontals. At most `most`
// points of a series are drawn, evenly thinned, which a caption should say when it matters.
export function lineChart({ series, width = 640, height = 300, xLabel = '', yLabel = '', marks = [], dots = false, equal = false, most = 6000, xRange, yRange, what = 'a chart' }) {
  const legend = series.length > 1 ? 16 + 7 * Math.max(...series.map((s) => String(s.name ?? '').length)) : 0;
  const f = frame({ width, height, xRange: xRange ?? extent(series.map((s) => s.x)), yRange: yRange ?? extent(series.map((s) => s.y)), xLabel, yLabel, legend, equal });
  let g = open(width, height, what) + f.g + `<clipPath id="d-clip-${width}-${height}"><rect x="${f.m.l}" y="${f.m.t}" width="${f.w}" height="${f.h}"/></clipPath><g clip-path="url(#d-clip-${width}-${height})">`;
  for (const mark of marks) {
    if (mark.x !== undefined) g += `<path class="d-mark" d="M${f.sx(mark.x).toFixed(1)} ${f.m.t}v${f.h}"/>`;
    if (mark.y !== undefined) g += `<path class="d-mark" d="M${f.m.l} ${f.sy(mark.y).toFixed(1)}h${f.w}"/>`;
  }
  series.forEach((s, i) => {
    const colour = s.colour ?? hue(i), n = Math.min(s.x.length, s.y.length), stride = Math.max(1, Math.ceil(n / most));
    let d = '';
    const asDots = s.dots ?? dots;
    for (let k = 0; k < n; k += stride) {
      const X = f.sx(s.x[k]), Y = f.sy(s.y[k]);
      if (!Number.isFinite(X) || !Number.isFinite(Y)) continue;
      d += asDots ? `M${X.toFixed(1)} ${Y.toFixed(1)}h0` : `${d ? 'L' : 'M'}${X.toFixed(1)} ${Y.toFixed(1)}`;
    }
    g += asDots ? `<path d="${d}" stroke="${colour}" stroke-width="${s.size ?? 1.6}" stroke-linecap="round" fill="none" opacity="${s.opacity ?? 0.85}"/>`
                : `<path d="${d}" stroke="${colour}" stroke-width="${s.width ?? 1.3}" stroke-linejoin="round" fill="none"${s.dash ? ` stroke-dasharray="${s.dash}"` : ''}/>`;
  });
  g += '</g>';
  for (const mark of marks) if (mark.label && mark.y !== undefined) g += `<text class="d-tick" x="${f.m.l + f.w - 4}" y="${(f.sy(mark.y) - 4).toFixed(1)}" text-anchor="end">${esc(mark.label)}</text>`;
  if (legend) g += legendOf(series.map((s, i) => ({ name: s.name, colour: s.colour ?? hue(i) })), f.m.l + f.w + 10, f.m.t + 8);
  return g + '</svg>';
}

// values[iy * xs.length + ix] on the grid of centres xs × ys. kind 'sequential' draws a magnitude from pale (low)
// to deep (high), or the reverse with `deepLow`; 'categorical' draws labels, −1 as the ground, with `names`.
export function heatMap({ xs, ys, values, kind = 'sequential', map = 'orchid', deepLow = false, names = [], width = 420, height = 380, xLabel = '', yLabel = '', points = [], what = 'a map' }) {
  // The legend is as wide as its longest name asks, for labels; for a magnitude, as its two values.
  const legend = kind === 'categorical' ? Math.max(74, 34 + 6.4 * Math.max(0, ...names.map((n) => String(n).length))) : 74;
  const dx = xs.length > 1 ? xs[1] - xs[0] : 1, dy = ys.length > 1 ? ys[1] - ys[0] : 1;
  const f = frame({ width, height, xRange: [xs[0] - dx / 2, xs[xs.length - 1] + dx / 2], yRange: [ys[0] - dy / 2, ys[ys.length - 1] + dy / 2], xLabel, yLabel, legend });
  let lo = Infinity, hi = -Infinity;
  for (const v of values) if (Number.isFinite(v)) { if (v < lo) lo = v; if (v > hi) hi = v; }
  const ramp = colourMap(map).colour;
  const colour = (v) => {
    if (kind === 'categorical') return v < 0 ? 'none' : hue(v, false);
    const u = hi > lo ? (v - lo) / (hi - lo) : 0.5;
    return cssColour(ramp(deepLow ? 1 - u : u));
  };
  const cw = f.w / xs.length, ch = f.h / ys.length;
  let g = open(width, height, what) + f.g;
  for (let iy = 0; iy < ys.length; iy++) {
    for (let ix = 0; ix < xs.length; ix++) {
      const v = values[iy * xs.length + ix];
      if (!Number.isFinite(v) || (kind === 'categorical' && v < 0)) continue;
      g += `<rect x="${(f.m.l + ix * cw).toFixed(2)}" y="${(f.m.t + f.h - (iy + 1) * ch).toFixed(2)}" width="${(cw + 0.4).toFixed(2)}" height="${(ch + 0.4).toFixed(2)}" fill="${colour(v)}"/>`;
    }
  }
  for (const p of points) g += `<circle class="d-point" cx="${f.sx(p.x).toFixed(1)}" cy="${f.sy(p.y).toFixed(1)}" r="4"/>${p.label ? `<text class="d-tick" x="${(f.sx(p.x) + 7).toFixed(1)}" y="${(f.sy(p.y) + 3.5).toFixed(1)}">${esc(p.label)}</text>` : ''}`;
  const lx = f.m.l + f.w + 12;
  if (kind === 'categorical') {
    names.forEach((n, i) => { g += `<rect x="${lx}" y="${f.m.t + 4 + 17 * i}" width="11" height="11" fill="${hue(i, false)}"/><text class="d-legend" x="${lx + 16}" y="${f.m.t + 13.5 + 17 * i}">${esc(n)}</text>`; });
  } else {
    const steps = 40, bar = Math.min(150, f.h - 20);
    for (let k = 0; k < steps; k++) g += `<rect x="${lx}" y="${(f.m.t + 6 + bar - (k + 1) * bar / steps).toFixed(1)}" width="11" height="${(bar / steps + 0.5).toFixed(1)}" fill="${cssColour(ramp(deepLow ? 1 - (k + 0.5) / steps : (k + 0.5) / steps))}"/>`;
    g += `<text class="d-tick" x="${lx + 15}" y="${f.m.t + 12}">${fmt(hi)}</text><text class="d-tick" x="${lx + 15}" y="${f.m.t + 8 + bar}">${fmt(lo)}</text>`;
  }
  return g + '</svg>';
}

// points: [[re, im], …]. With `circle` the unit circle is drawn (a map's eigenvalues: stable inside it); else the
// imaginary axis (a flow's, a generator's: stable to its left).
export function complexPlane({ points, circle = false, width = 360, height = 300, what = 'eigenvalues in the plane', xLabel = 'real part', yLabel = 'imaginary part' }) {
  const re = points.map((p) => p[0]), im = points.map((p) => p[1]);
  const f = frame({ width, height, xRange: extent([re, circle ? [-1, 1] : [0]]), yRange: extent([im, circle ? [-1, 1] : [0]]), xLabel, yLabel, equal: circle });
  let g = open(width, height, what) + f.g;
  if (circle) g += `<ellipse class="d-mark" cx="${f.sx(0).toFixed(1)}" cy="${f.sy(0).toFixed(1)}" rx="${Math.abs(f.sx(1) - f.sx(0)).toFixed(1)}" ry="${Math.abs(f.sy(1) - f.sy(0)).toFixed(1)}" fill="none"/>`;
  else g += `<path class="d-mark" d="M${f.sx(0).toFixed(1)} ${f.m.t}v${f.h}"/>`;
  for (const [a, b] of points) {
    const stable = circle ? Math.hypot(a, b) < 1 - 1e-9 : a < -1e-9;
    g += `<circle class="d-eigen${stable ? '' : ' d-not-stable'}" cx="${f.sx(a).toFixed(1)}" cy="${f.sy(b).toFixed(1)}" r="3.4"/>`;
  }
  return g + '</svg>';
}

// A braid word on `strands` strands, read from the left: letter +i is the counterclockwise half-exchange of the
// strands at positions i and i + 1, the left one passing below; drawn with time to the right, the strand that
// passes over whole and the one beneath it broken. At most `most` letters are drawn.
export function braidDiagram({ strands, word, width = 640, height, most = 60, what = 'a braid' }) {
  const letters = word.slice(0, most), n = strands;
  const row = 22, step = Math.max(14, Math.min(40, (width - 60) / Math.max(letters.length, 1)));
  const H = height ?? 26 + row * (n - 1) + 22, W = Math.min(width, 50 + step * Math.max(letters.length, 1) + 20);
  const y = (p) => 14 + row * p, x = (k) => 30 + step * k;
  // at[p] is the strand at position p; each strand keeps its hue through the crossings. The diagram is drawn a
  // letter at a time, the strand beneath first, then a gap of the ground's colour along the strand above, then
  // that strand: so the one above is whole and the one beneath is broken where they cross.
  const at = Array.from({ length: n }, (_, i) => i);
  const line = (d, s) => `<path d="${d}" stroke="${hue(s)}" stroke-width="2.4" fill="none" stroke-linecap="round"/>`;
  let g = open(W, H, what);
  for (let p = 0; p < n; p++) g += line(`M${x(0) - 14} ${y(p)}L${x(0)} ${y(p)}`, at[p]);
  letters.forEach((letter, k) => {
    const i = Math.abs(letter) - 1, positive = letter > 0;
    const curve = (from, to) => `M${x(k).toFixed(1)} ${y(from)}C${(x(k) + step / 2).toFixed(1)} ${y(from)} ${(x(k) + step / 2).toFixed(1)} ${y(to)} ${x(k + 1).toFixed(1)} ${y(to)}`;
    for (let p = 0; p < n; p++) if (p !== i && p !== i + 1) g += line(`M${x(k).toFixed(1)} ${y(p)}L${x(k + 1).toFixed(1)} ${y(p)}`, at[p]);
    const left = { s: at[i], d: curve(i, i + 1) }, right = { s: at[i + 1], d: curve(i + 1, i) };
    // In a positive crossing the left strand passes below.
    const beneath = positive ? left : right, above = positive ? right : left;
    g += line(beneath.d, beneath.s) + `<path class="d-gap" d="${above.d}" stroke-width="7.5" fill="none"/>` + line(above.d, above.s);
    [at[i], at[i + 1]] = [at[i + 1], at[i]];
  });
  for (let p = 0; p < n; p++) g += line(`M${x(letters.length).toFixed(1)} ${y(p)}L${(x(letters.length) + 14).toFixed(1)} ${y(p)}`, at[p]);
  letters.forEach((letter, k) => { g += `<text class="d-tick" x="${(x(k) + step / 2).toFixed(1)}" y="${H - 5}" text-anchor="middle">${letter > 0 ? '' : '−'}${Math.abs(letter)}</text>`; });
  if (word.length > most) g += `<text class="d-tick" x="${W - 4}" y="${H - 5}" text-anchor="end">… ${word.length - most} more</text>`;
  return g + '</svg>';
}

// The forms a rewriting system reaches: {forms: [{text, final, terminal, histories}], edges: [[from, to, rule]]},
// laid out by the length of the form, the start at the top.
export function formsGraph({ forms, edges, width = 640, what = 'the forms a system of rules reaches' }) {
  const depth = new Array(forms.length).fill(-1);
  depth[0] = 0;
  for (let pass = 0; pass < forms.length; pass++) {
    let moved = false;
    for (const [a, b] of edges) if (depth[a] >= 0 && depth[b] < depth[a] + 1 && depth[b] < 0) { depth[b] = depth[a] + 1; moved = true; }
    if (!moved) break;
  }
  const rows = [];
  forms.forEach((_, i) => { const d = Math.max(0, depth[i]); (rows[d] ??= []).push(i); });
  const rowH = 46, H = 24 + rowH * rows.length;
  // A row is set from its forms' own widths, centred, so that no two names run into each other; of a row too long
  // for the page the first forms are drawn and the rest are counted.
  const label = (i) => `${forms[i].text}${forms[i].final && forms[i].terminal && forms[i].histories > 1 ? ` ×${forms[i].histories}` : ''}`;
  const wide = (text) => 7.3 * text.length, gap = 22;
  const place = new Map(), more = [];
  rows.forEach((row, d) => {
    let kept = row, used = row.reduce((s, i) => s + wide(label(i)), 0) + gap * (row.length - 1);
    if (used > width - 24) {
      const tail = wide('… 99 more') + gap;
      kept = [], used = 0;
      for (const i of row) {
        const next = used + (kept.length ? gap : 0) + wide(label(i));
        if (next + tail > width - 24) break;
        kept.push(i), used = next;
      }
      more.push({ count: row.length - kept.length, y: 20 + rowH * d });
    }
    let x = (width - used - (kept.length < row.length ? wide('… 99 more') + gap : 0)) / 2;
    for (const i of kept) { const w = wide(label(i)); place.set(i, { x: x + w / 2, y: 20 + rowH * d }); x += w + gap; }
    if (kept.length < row.length) more[more.length - 1].x = x;
  });
  let g = open(width, H, what);
  for (const m of more) g += `<text class="d-tick" x="${m.x.toFixed(1)}" y="${m.y + 4}">… ${m.count} more</text>`;
  for (const [a, b] of edges) {
    const p = place.get(a), q = place.get(b);
    if (p && q) g += `<path class="d-edge" d="M${p.x.toFixed(1)} ${p.y + 9}L${q.x.toFixed(1)} ${q.y - 11}"/>`;
  }
  forms.forEach((form, i) => {
    const p = place.get(i);
    if (!p) return;
    const cls = form.final && form.terminal ? 'd-word' : form.final ? 'd-dead' : 'd-form';
    g += `<text class="${cls}" x="${p.x.toFixed(1)}" y="${p.y + 4}" text-anchor="middle">${esc(label(i))}</text>`;
  });
  return g + '</svg>';
}
