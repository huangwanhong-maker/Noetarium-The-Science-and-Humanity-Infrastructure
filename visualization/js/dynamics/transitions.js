// Critical transitions drawn (ADR 0135), as markup that needs no page: what kinesis's detectors said of a record, in the words
// and shapes of their answer (api/cpp/science/transitions.hpp).
//
//   verdictMark(verdict)    a verdict told by a glyph and a word as well as a tone: signal a filled disc, no signal an open
//                           ring, undecidable a half-filled disc
//   findingChart(finding)   an indicator through its windows, the first and the last window ringed and joined by the change
//                           the test is of, with the verdict beside it
//   eigenPath(kind)         the dominant eigenvalue's path in the plane against the unit circle, faint where it began and full
//                           where it ends, with the three places a local loss of stability goes: +1, −1 and the circle
//   operating(findings)     each test's operating characteristics at the record's length: its detection rate against the
//                           declared alternative as a bar, its false-alarm rate as a tick, the area under its ROC curve, its
//                           significance, its verdict
//   gridMap(finding)        the grid of windows and bandwidths, each cell toned by how significant the change is there and
//                           crossed where it is significant the other way
//   whereBars(where)        where the leading mode shows over the units (filled) and what excites it (outlined)
//   wellsChart(wells)       the landscape −ln p of the last window and its fitted polynomial, the wells marked
//
// A verdict, a reversal and a mode's place are each told by a shape and a word, never by colour alone.

import { escapeText as esc, extent, formatNumber as fmt, frame, lineChart, openDrawing as open } from './charts.js';

const VERDICTS = { signal: 'signal', 'no signal': 'no signal', undecidable: 'undecidable' };

function glyph(verdict, x, y, r = 5.5) {
  if (verdict === 'signal') return `<circle class="tr-glyph tr-signal" cx="${x}" cy="${y}" r="${r}"/>`;
  if (verdict === 'no signal') return `<circle class="tr-glyph tr-none" cx="${x}" cy="${y}" r="${r}"/>`;
  return `<circle class="tr-glyph tr-none" cx="${x}" cy="${y}" r="${r}"/><path class="tr-glyph tr-signal" d="M${x} ${y - r}A${r} ${r} 0 0 0 ${x} ${y + r}Z"/>`;
}

export function verdictMark(verdict, { width = 120 } = {}) {
  const v = VERDICTS[verdict] ?? 'undecidable';
  return `${open(width, 18, `verdict: ${v}`)}${glyph(v, 8, 9)}<text class="tr-word" x="19" y="13">${esc(v)}</text></svg>`;
}

// finding: {indicator, at, series, change, positive, verdict, p, detection}
export function findingChart(finding, { width = 420, height = 210 } = {}) {
  const at = finding.at ?? [], series = (finding.series ?? []).map((v) => (v === null ? NaN : v));
  const pts = at.map((t, i) => [t, series[i]]).filter((p) => Number.isFinite(p[1]));
  if (pts.length < 2) return `${open(width, 40, `${finding.indicator}: too few windows`)}<text class="d-label" x="8" y="24">${esc(finding.indicator)}: too few windows to draw</text></svg>`;
  const f = frame({ width, height: height - 22, xRange: extent([pts.map((p) => p[0])]), yRange: extent([pts.map((p) => p[1])]), xLabel: 'the end of the window', yLabel: finding.indicator });
  let g = open(width, height, `${finding.indicator} through its windows: ${finding.verdict}`).replace(`viewBox="0 0 ${width} ${height}"`, `viewBox="0 -22 ${width} ${height}"`);
  g += f.g;
  let d = '';
  for (const [x, y] of pts) d += `${d ? 'L' : 'M'}${f.sx(x).toFixed(1)} ${f.sy(y).toFixed(1)}`;
  g += `<path class="tr-series" d="${d}"/>`;
  const [a, b] = [pts[0], pts[pts.length - 1]];
  g += `<path class="tr-change" d="M${f.sx(a[0]).toFixed(1)} ${f.sy(a[1]).toFixed(1)}L${f.sx(b[0]).toFixed(1)} ${f.sy(b[1]).toFixed(1)}"/>`;
  g += `<circle class="tr-end" cx="${f.sx(a[0]).toFixed(1)}" cy="${f.sy(a[1]).toFixed(1)}" r="4"/><circle class="tr-end" cx="${f.sx(b[0]).toFixed(1)}" cy="${f.sy(b[1]).toFixed(1)}" r="4"/>`;
  const v = VERDICTS[finding.verdict] ?? 'undecidable';
  const change = `${finding.positive ? 'log ratio' : 'change'} ${fmt(finding.change)}, p = ${fmt(finding.p)}`;
  g += `${glyph(v, 60, -11)}<text class="tr-word" x="71" y="-7">${esc(v)}</text><text class="d-tick" x="${width - 12}" y="-7" text-anchor="end">${esc(change)}</text>`;
  return g + '</svg>';
}

// kind: {path: [[re, im]], approach, described}
export function eigenPath(kind, { width = 320, height = 300 } = {}) {
  const path = (kind?.path ?? []).filter((p) => Number.isFinite(p[0]) && Number.isFinite(p[1]));
  const f = frame({ width, height, xRange: [-1.25, 1.25], yRange: [-1.25, 1.25], xLabel: 'real part', yLabel: 'imaginary part', equal: true });
  let g = open(width, height, `the dominant eigenvalue's path: ${kind?.described ?? 'none'}`) + f.g;
  const cx = f.sx(0), cy = f.sy(0), rx = Math.abs(f.sx(1) - cx), ry = Math.abs(f.sy(1) - cy);
  g += `<ellipse class="tr-circle" cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" rx="${rx.toFixed(1)}" ry="${ry.toFixed(1)}"/>`;
  // Each place's name on its own line: −1 above the axis, +1 below it, the circle's at its top right, so that none overlaps.
  const place = (x, y, label, anchor, cls, dy) => `<circle class="tr-place${cls === kind?.approach ? ' tr-reached' : ''}" cx="${f.sx(x).toFixed(1)}" cy="${f.sy(y).toFixed(1)}" r="3"/><text class="tr-place-label" x="${(f.sx(x) + (anchor === 'end' ? -6 : anchor === 'start' ? 6 : 0)).toFixed(1)}" y="${(f.sy(y) + dy).toFixed(1)}" text-anchor="${anchor}">${esc(label)}</text>`;
  g += place(1, 0, '+1: fold · transcritical · pitchfork', 'end', 'stationary', 15) + place(-1, 0, '−1: period-doubling', 'start', 'flip', -7) +
       place(Math.cos(Math.PI / 3), Math.sin(Math.PI / 3), 'the circle: Hopf', 'start', 'oscillatory', -8);
  path.forEach(([re, im], i) => {
    const o = path.length > 1 ? 0.2 + 0.8 * i / (path.length - 1) : 1;
    g += `<circle class="tr-step" cx="${f.sx(re).toFixed(1)}" cy="${f.sy(im).toFixed(1)}" r="${i === path.length - 1 ? 4.2 : 2.4}" opacity="${o.toFixed(2)}"/>`;
    if (Math.abs(im) > 1e-9) g += `<circle class="tr-step tr-conjugate" cx="${f.sx(re).toFixed(1)}" cy="${f.sy(-im).toFixed(1)}" r="1.8" opacity="${(o * 0.5).toFixed(2)}"/>`;
  });
  return g + '</svg>';
}

// findings: [{channel, indicator, verdict, p, false_alarm, detection, auc}]
export function operating(findings, { width = 760, row = 22, alpha = 0.05 } = {}) {
  const head = 34, left = 310, bar = 170, gap = 18;
  const height = head + row * findings.length + 8;
  let g = open(width, height, "each test's operating characteristics at the record's length");
  const x0 = left + 108;
  g += `<text class="tr-head" x="8" y="14">indicator</text><text class="tr-head" x="${left}" y="14">verdict</text>`;
  g += `<text class="tr-head" x="${x0}" y="14">detects the alternative</text><text class="tr-head" x="${x0 + bar + gap}" y="14">ROC area</text><text class="tr-head" x="${x0 + bar + gap + 80}" y="14">p</text>`;
  g += `<text class="d-tick" x="${x0}" y="28">0</text><text class="d-tick" x="${x0 + bar}" y="28" text-anchor="end">1</text>`;
  findings.forEach((f, i) => {
    const y = head + row * i + row / 2;
    const v = VERDICTS[f.verdict] ?? 'undecidable';
    const label = (f.channel && f.channel !== 'the record' ? `${f.channel} · ` : '') + f.indicator;
    g += `<text class="tr-name" x="8" y="${y + 4}">${esc(label.length > 50 ? `${label.slice(0, 49)}…` : label)}</text>`;
    g += `${glyph(v, left + 6, y)}<text class="tr-word" x="${left + 16}" y="${y + 4}">${esc(v)}</text>`;
    g += `<rect class="tr-track" x="${x0}" y="${y - 5}" width="${bar}" height="10"/><rect class="tr-bar" x="${x0}" y="${y - 5}" width="${(bar * Math.max(0, Math.min(1, f.detection ?? 0))).toFixed(1)}" height="10"/>`;
    g += `<path class="tr-alarm" d="M${(x0 + bar * Math.max(0, Math.min(1, f.false_alarm ?? alpha))).toFixed(1)} ${y - 8}v16"><title>false alarms ${fmt(f.false_alarm)}</title></path>`;
    const ax = x0 + bar + gap, aw = 64;
    g += `<rect class="tr-track" x="${ax}" y="${y - 1}" width="${aw}" height="2"/><circle class="tr-auc" cx="${(ax + aw * Math.max(0, Math.min(1, ((f.auc ?? 0.5) - 0.5) / 0.5))).toFixed(1)}" cy="${y}" r="3.6"/>`;
    g += `<text class="tr-name" x="${ax + 80}" y="${y + 4}">${esc(fmt(f.p))}</text>`;
  });
  return g + '</svg>';
}

// finding.grid: [{window, bandwidth, p, p_reverse, change}]
export function gridMap(finding, { cell = 40, alpha = 0.05 } = {}) {
  const grid = finding?.grid ?? [];
  if (!grid.length) return `${open(260, 30, 'no grid of windows and bandwidths')}<text class="d-label" x="8" y="20">no grid of windows and bandwidths was asked for</text></svg>`;
  const ws = [...new Set(grid.map((c) => c.window))].sort((a, b) => a - b), bs = [...new Set(grid.map((c) => c.bandwidth))].sort((a, b) => a - b);
  const left = 92, top = 30, width = left + cell * ws.length + 12, height = top + cell * bs.length + 34;
  let g = open(width, height, `${finding.indicator}: the change's significance over windows and bandwidths`);
  g += `<text class="tr-head" x="${left}" y="14">the window, as a share of the record</text>`;
  ws.forEach((w, i) => { g += `<text class="d-tick" x="${left + cell * i + cell / 2}" y="${top - 4}" text-anchor="middle">${esc(fmt(w))}</text>`; });
  bs.forEach((b, j) => { g += `<text class="d-tick" x="${left - 6}" y="${top + cell * j + cell / 2 + 4}" text-anchor="end">smoother ${esc(fmt(b))}</text>`; });
  for (const c of grid) {
    const i = ws.indexOf(c.window), j = bs.indexOf(c.bandwidth);
    const x = left + cell * i, y = top + cell * j;
    const strength = c.p < alpha ? Math.min(1, Math.log10(alpha / c.p) / 1.5 + 0.35) : 0;
    g += `<rect class="tr-cell${c.p < alpha ? ' tr-significant' : ''}" x="${x + 1}" y="${y + 1}" width="${cell - 2}" height="${cell - 2}"${strength ? ` style="fill-opacity:${strength.toFixed(2)}"` : ''}><title>p = ${fmt(c.p)}</title></rect>`;
    if (c.p_reverse < alpha) g += `<path class="tr-reverse" d="M${x + 8} ${y + 8}L${x + cell - 8} ${y + cell - 8}M${x + cell - 8} ${y + 8}L${x + 8} ${y + cell - 8}"/>`;
  }
  g += `<text class="tr-name" x="${left}" y="${height - 8}">filled: significant in the indicator's direction · crossed: significant the other way</text>`;
  return g + '</svg>';
}

// where: {units, loading, excitation, sentence}
export function whereBars(where, { width = 640, height = 220 } = {}) {
  const n = where?.units?.length ?? 0;
  if (!n) return `${open(240, 30, 'no place')}<text class="d-label" x="8" y="20">no leading mode was read</text></svg>`;
  const most = Math.max(...where.loading, ...where.excitation, 1e-12);
  const f = frame({ width, height, xRange: [-0.5, n - 0.5], yRange: [0, most * 1.08], xLabel: '', yLabel: 'share of the mode' });
  const slot = f.w / n, bw = Math.min(26, slot * 0.6);
  let g = open(width, height, `where the leading mode shows: ${where.sentence ?? ''}`) + f.g;
  where.units.forEach((u, i) => {
    const x = f.sx(i);
    const yl = f.sy(where.loading[i]), ye = f.sy(where.excitation[i]);
    g += `<rect class="tr-load" x="${(x - bw / 2).toFixed(1)}" y="${yl.toFixed(1)}" width="${bw.toFixed(1)}" height="${(f.sy(0) - yl).toFixed(1)}"><title>${esc(u)}: shows ${fmt(where.loading[i])}</title></rect>`;
    g += `<rect class="tr-excite" x="${(x - bw / 2 - 3).toFixed(1)}" y="${ye.toFixed(1)}" width="${(bw + 6).toFixed(1)}" height="${(f.sy(0) - ye).toFixed(1)}"><title>${esc(u)}: excited ${fmt(where.excitation[i])}</title></rect>`;
    if (n <= 24) g += `<text class="d-tick" x="${x.toFixed(1)}" y="${(f.m.t + f.h + 14).toFixed(1)}" text-anchor="middle">${esc(String(u).length > 9 ? `${String(u).slice(0, 8)}…` : u)}</text>`;
  });
  return g + '</svg>';
}

// wells: {grid, landscape, fitted, minima, wells, switches}
export function wellsChart(wells, { width = 420, height = 240 } = {}) {
  if (!wells?.grid?.length) return `${open(240, 30, 'no landscape')}<text class="d-label" x="8" y="20">no landscape was read</text></svg>`;
  return lineChart({ series: [{ name: '−ln p', x: wells.grid, y: wells.landscape, dots: true }, { name: 'fitted', x: wells.grid, y: wells.fitted }],
    marks: (wells.minima ?? []).map((m) => ({ x: m })), xLabel: 'value in the last window', yLabel: '−ln p', width, height,
    what: `the landscape of the last window: ${wells.wells} well${wells.wells === 1 ? '' : 's'}, ${wells.switches} switches` });
}
