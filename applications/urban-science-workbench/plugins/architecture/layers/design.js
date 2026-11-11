// What could be built (ADR 0089): pick a block anywhere, and the engine searches what could stand on it under rules one declares.
// Each lot (a footprint as built: parcels are not in the open data) takes a podium, an upper part on a share of its upper envelope and
// their storeys; one passage may be opened through the block. heuresis's NSGA-III keeps the designs no other betters in all of six
// measures, each the domain's own: floor, winter sun on the streets, winter sun on the neighbours' faces, the heat a winter day needs
// per m² of floor (architecture), how the streets about the block stand in the whole and how much floor lies within a five-minute
// walk of the doors (urban science). The chosen design stands on the map in place of the block's buildings, with the hours of sun on
// the ground about it; the front is drawn on the card, and given to the Studio as figures when the view is captured.

import { colourMap } from '/visualization/styles/index.js';
import { subsolarPoint, sunAltAz } from '/domains/astronomy/js/index.js';

export const STREETS = new Set(['motorway', 'trunk', 'primary', 'secondary', 'tertiary', 'residential', 'living_street', 'unclassified', 'pedestrian', 'road',
  'motorway_link', 'trunk_link', 'primary_link', 'secondary_link', 'tertiary_link']);
export const NOT_WALKED = new Set(['motorway', 'motorway_link']);
const STOREY = 3.2;
// The rules, declared: none is any city's planning scheme.
export const RULES = {
  wall: { name: 'a street wall and towers', rules: { max_height: 100, plot_ratio: 12, street_setback: 0, side_setback: 0, street_wall: 24, upper_setback: 10, storey: STOREY, min_frontage: 0.6 },
    says: 'a street wall to 24 m on the street line, towers set back 10 m from the street above it, to 100 m; a plot ratio of 12; three fifths of the street line walled' },
  mid: { name: 'mid-rise', rules: { max_height: 32, plot_ratio: 4.5, street_setback: 0, side_setback: 0, street_wall: 16, upper_setback: 5, storey: STOREY, min_frontage: 0.8 },
    says: 'a street wall to 16 m, upper floors set back 5 m above it, to 32 m; a plot ratio of 4.5; four fifths of the street line walled' },
  apart: { name: 'towers apart', rules: { max_height: 150, plot_ratio: 8, street_setback: 6, side_setback: 6, street_wall: 9.6, upper_setback: 4, storey: STOREY, min_frontage: 0 },
    says: 'set back 6 m from the street and from neighbours, a low base to 9.6 m, towers to 150 m stepped 4 m further; a plot ratio of 8; no street wall asked' },
};
const AIR = { day: 8, night: 4 };  // °C on the winter day: declared, the same everywhere
// Each measure: its key, its name, its unit, how it is written, and whether more is better (1) or worse (−1).
const thousands = (v) => `${(v / 1000).toLocaleString('en', { maximumFractionDigits: v < 1e5 ? 1 : 0 })}k`;
const MEASURES = [
  ['floor', 'floor', 'm²', thousands, 1],
  ['streetSun', 'winter sun on the streets', 'm²·h', thousands, 1],
  ['facadeSun', "winter sun on the neighbours' faces", 'm²·h', thousands, 1],
  ['heat', 'heat a winter day needs', 'Wh/m²', (v) => v.toFixed(0), -1],
  ['configuration', 'integration of the streets about it', 'NAIN', (v) => v.toFixed(3), 1],
  ['access', 'floor within five minutes on foot', 'm²', thousands, 1],
];
const PICKS = [['balanced', 'balanced'], ['floor', 'floor'], ['streetSun', 'street sun'], ['facadeSun', "neighbours' sun"], ['heat', 'least heat'], ['access', 'walk'], ['built', 'as built']];

function centroidOf(ring) {
  let a = 0, cx = 0, cy = 0;
  for (let i = 0, n = ring.length; i < n; i++) {
    const [x0, y0] = ring[i], [x1, y1] = ring[(i + 1) % n];
    const c = x0 * y1 - x1 * y0;
    a += c, cx += (x0 + x1) * c, cy += (y0 + y1) * c;
  }
  return Math.abs(a) < 1e-9 ? ring[0] : [cx / (3 * a), cy / (3 * a)];
}

// The buildings drawn about a point, with their heights: a city model's and the world's tiles'.
export function buildingsNear(gis, at, radius) {
  const out = [];
  const add = (ring, height) => {
    if (!ring || ring.length < 3) return;
    const c = centroidOf(ring);
    if (Math.hypot(c[0] - at[0], c[1] - at[1]) > radius) return;
    out.push([Math.round(10 * (height > 0 ? height : 3 * STOREY)) / 10, ...ring.flatMap((p) => [Math.round(p[0] * 10) / 10, Math.round(p[1] * 10) / 10])]);
  };
  for (const [, layer] of gis.layers) {
    if (layer.kind === 'solids' && layer.features?.length && Array.isArray(layer.features[0].footprint))
      for (const f of layer.features) add(f.footprint, f.height_m);
    if (layer.kind === 'built-solids' && layer.built?.starts) {
      const { rings, starts, features, boxes } = layer.built;
      for (let k = 0; k < starts.length / 2; k++) {
        const s = starts[2 * k], n = starts[2 * k + 1], f = features?.[k] || [];
        const ring = [];
        for (let i = 0; i < n; i++) ring.push([rings[2 * (s + i)], rings[2 * (s + i) + 1]]);
        add(ring, f[0] || (boxes ? boxes[5 * k + 4] : 0));
      }
    }
  }
  return out;
}

// The ways about a point within a radius, cut where they leave it: those of some classes, or all but those of others.
export function waysNear(world, at, radius, keep) {
  const out = [];
  for (const l of world.lines()) {
    if (l.rail || !keep(l.cls)) continue;
    let run = [];
    for (let i = 0; i < l.xyz.length; i += 3) {
      const x = l.xyz[i], y = l.xyz[i + 1];
      if (Math.hypot(x - at[0], y - at[1]) <= radius) run.push(Math.round(x * 10) / 10, Math.round(y * 10) / 10);
      else { if (run.length >= 4) out.push(run); run = []; }
    }
    if (run.length >= 4) out.push(run);
  }
  return out;
}

// The winter solstice of the place's hemisphere, its Sun at each hour from 10 to 14, local solar time.
export function winterDay(lat, lon, year) {
  const [m, d] = lat < 0 ? [5, 21] : [11, 21];
  const suns = [];
  for (const hour of [10, 11, 12, 13, 14]) {
    const ms = Date.UTC(year, m, d) + (hour - lon / 15) * 3600e3;
    const a = sunAltAz(lon, lat, subsolarPoint(ms));
    suns.push([(a.altitude * Math.PI) / 180, (a.azimuth * Math.PI) / 180, 1, AIR.day]);
  }
  return { date: `${year}-${String(m + 1).padStart(2, '0')}-${d}`, suns };
}

const ring = (flat) => { const r = []; for (let i = 0; i + 1 < flat.length; i += 2) r.push([flat[i], flat[i + 1]]); return r; };

export async function layer(ctx) {
  const { gis, frame, world, dom: { h } } = ctx;
  const solidsId = ctx.layerId(':massing'), sunId = ctx.layerId(':sun'), passageId = ctx.layerId(':passage'), blockId = ctx.layerId(':block');
  const night = ctx.theme === 'night';
  const map = colourMap(night ? 'plum' : 'orchid').colour;
  const css = (rgb) => `rgb(${rgb.map((v) => Math.round(255 * v)).join(',')})`;
  const tint = { podium: night ? [0.93, 0.84, 0.88] : [0.96, 0.9, 0.91], upper: night ? [0.93, 0.56, 0.74] : [0.85, 0.5, 0.66], built: night ? [0.72, 0.66, 0.78] : [0.78, 0.74, 0.8] };
  const variantColour = night ? ['#b892dd', '#ee8fbd', '#e9c27a'] : ['#7d54ab', '#c2477f', '#a07a2e'];
  let preset = 'wall', generations = 60, keep = 0.5, centre = null, latlon = null, R = null, chosen = 'balanced', index = -1, token = 0, dead = false, hollowed = false;
  const status = h('div', { class: 'ld-quiet' }, 'Pick a block: what could be built there, under rules you choose.');
  const readout = h('div', { class: 'ad-readout' });
  const front = h('div', { class: 'ad-front' });
  const picks = h('div', { class: 'keys-tight' });
  const note = h('div', { class: 'ld-quiet' });
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));
  const says = h('div', { class: 'ld-quiet' }, RULES[preset].says);
  const accent = night ? '#ee8fbd' : '#c2477f';
  const style = h('style', {}, `div.ad-table{display:grid;grid-template-columns:1fr auto auto auto;column-gap:10px;row-gap:2px;margin:6px 0 4px;font-size:12px;align-items:baseline}
.ad-table .ad-head{font-size:10.5px;letter-spacing:.04em;text-transform:uppercase;opacity:.7;text-align:right}
.ad-table .ad-num{font-variant-numeric:tabular-nums;text-align:right;font-weight:600}
.ad-table .ad-built{font-weight:400;opacity:.72}
.ad-table .ad-better{color:${accent}}
.ad-table .ad-unit{font-size:10.5px;opacity:.6}
.ad-front svg{display:block;margin-top:4px}`);

  // A design's six measures, each scaled over the front to [0, 1] with 0 the best; the balanced design is the one whose worst is least.
  function scaled() {
    const D = R.designs, out = D.map(() => []);
    for (const [key, , , , sign] of MEASURES) {
      const v = D.map((d) => d.measures[key]), lo = Math.min(...v), hi = Math.max(...v);
      D.forEach((d, i) => out[i].push(hi > lo ? (sign > 0 ? hi - d.measures[key] : d.measures[key] - lo) / (hi - lo) : 0));
    }
    return out;
  }
  function choose(pick) {
    chosen = pick;
    if (!R || !R.designs.length || pick === 'built') { index = -1; return; }
    const S = scaled();
    if (pick === 'balanced') index = S.reduce((b, s, i) => (Math.max(...s) < Math.max(...S[b]) ? i : b), 0);
    else {
      const k = MEASURES.findIndex((m) => m[0] === pick);
      index = S.reduce((b, s, i) => (s[k] < S[b][k] || (s[k] === S[b][k] && Math.max(...s) < Math.max(...S[b])) ? i : b), 0);
    }
  }

  // The block's buildings hidden from the city, so that the design stands alone: a mask of the lots at half a metre.
  function hollow(on) {
    if (!on || !R) { if (hollowed) gis.setHollow?.(null); hollowed = false; return; }
    const lots = R.lots.map((l) => ring(l.ring));
    let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
    for (const r of lots) for (const [x, y] of r) x0 = Math.min(x0, x), y0 = Math.min(y0, y), x1 = Math.max(x1, x), y1 = Math.max(y1, y);
    x0 -= 1, y0 -= 1, x1 += 1, y1 += 1;
    const step = 0.5, width = Math.min(2048, Math.ceil((x1 - x0) / step)), height = Math.min(2048, Math.ceil((y1 - y0) / step));
    const c = document.createElement('canvas');
    c.width = width, c.height = height;
    const g = c.getContext('2d');
    g.fillStyle = '#fff';
    g.strokeStyle = '#fff';
    g.lineWidth = 2;
    for (const r of lots) {
      g.beginPath();
      r.forEach(([x, y], i) => { const px = ((x - x0) / (x1 - x0)) * width, py = ((y - y0) / (y1 - y0)) * height; if (i) g.lineTo(px, py); else g.moveTo(px, py); });
      g.closePath();
      g.fill();
      g.stroke();
    }
    const rgba = g.getImageData(0, 0, width, height).data, data = new Uint8Array(width * height);
    for (let i = 0; i < data.length; i++) data[i] = rgba[4 * i] > 127 ? 255 : 0;  // rows from y0 (south): the canvas's row 0 drawn at y0
    gis.setHollow?.({ box: [x0, y0, x1, y1], data, width, height });
    hollowed = true;
  }

  async function draw() {
    if (!R) return;
    const me = token;
    const block = ring(R.block);
    gis.addRibbons(blockId, [{ line: [...block, block[0]] }], { line: (f) => f.line, width: () => 1.2, color: () => (night ? [0.93, 0.56, 0.74] : [0.76, 0.28, 0.5]), z: 0.5, order: 4 });
    const features = [];
    if (index < 0) {
      hollow(false);
      gis.remove(solidsId);
      gis.remove(passageId);
    } else {
      const d = R.designs[index], v = R.variants[d.variant];
      d.parts.forEach(([podium, level, top], j) => {
        const piece = v.pieces[j];
        if (podium > 0 && piece.ground) features.push({ ring: ring(piece.ground), base: 0, top: podium, kind: 'podium' });
        if (level >= 0 && piece.upper[level]) features.push({ ring: ring(piece.upper[level]), base: podium, top, kind: 'upper' });
      });
      hollow(true);
      gis.addSolids(solidsId, features, { ring: (f) => f.ring, base: (f) => f.base, height: (f) => f.top, color: (f) => tint[f.kind] });
      if (d.variant > 0) {
        const [ax, ay, bx, by] = R.passages[d.variant - 1];
        gis.addRibbons(passageId, [{ line: [[ax, ay], [bx, by]] }], { line: (f) => f.line, width: () => 6, color: () => (night ? [0.91, 0.76, 0.48] : [0.66, 0.48, 0.16]), z: 0.4, order: 3 });
      } else gis.remove(passageId);
    }
    // The hours of sun on the open ground about the block.
    try {
      const s = await ctx.ask('sunmap', { design: index });
      if (me !== token || dead) return;
      const half = s.spacing / 2;
      gis.addPrisms(sunId, s.points, { ring: ([x, y]) => [[x - half, y - half], [x + half, y - half], [x + half, y + half], [x - half, y + half]], height: () => 0.6, z: 0.7, shrink: 0.9, order: 4,
        color: ([, , hours]) => map((night ? 0.15 : 0.1) + 0.85 * (hours / s.hours)), opacity: 0.85 });
    } catch { /* the map of sun is a nicety */ }
    card();
  }

  function card() {
    const d = index >= 0 ? R.designs[index] : null, B = R.asBuilt;
    // A table: each measure, this design's value (the better of the two in the accent), the block as built, and the unit.
    const cell = (text, cls = '') => h('span', { class: cls }, text);
    const cells = [cell(''), cell(d ? 'design' : '', 'ad-head'), cell('as built', 'ad-head'), cell('')];
    const better = (key, sign) => d && sign * (d.measures[key] - B[key]) > 1e-9;
    for (const [key, name, unit, fmt, sign] of MEASURES)
      cells.push(cell(name, 'ad-name'), cell(d ? fmt(d.measures[key]) : '', better(key, sign) ? 'ad-num ad-better' : 'ad-num'), cell(fmt(B[key]), 'ad-num ad-built'), cell(unit, 'ad-unit'));
    const pr = (m) => `${m.plotRatio.toFixed(1)}`, ht = (m) => `${Math.round(m.height)}`, wall = (m) => `${Math.round(100 * m.frontage)}%`;
    cells.push(cell('plot ratio', 'ad-name'), cell(d ? pr(d.measures) : '', 'ad-num'), cell(pr(B), 'ad-num ad-built'), cell(''));
    cells.push(cell('height', 'ad-name'), cell(d ? ht(d.measures) : '', 'ad-num'), cell(ht(B), 'ad-num ad-built'), cell('m', 'ad-unit'));
    cells.push(cell('street line walled', 'ad-name'), cell(d ? wall(d.measures) : '', 'ad-num'), cell(wall(B), 'ad-num ad-built'), cell(''));
    readout.replaceChildren(h('div', { class: 'ad-table' }, cells),
      d ? h('div', { class: 'ld-quiet' }, d.variant > 0 ? `With ${R.variants[d.variant].name}, 6 m wide.` : 'No passage.') : h('div', { class: 'ld-quiet' }, 'The block as it stands.'));
    drawFront();
    note.textContent = `${R.designs.length} designs no other betters in all six measures, from ${R.evaluations.toLocaleString('en')} evaluated (NSGA-III, ${R.population} for ${R.generations} generations, ${R.seconds.searching.toFixed(1)} s on ${R.threads} threads). ` +
      `${R.lots.length} lots on ${Math.round(R.lotArea).toLocaleString('en')} m²; ${R.complying} of ${R.pieces} massings on the front checked against the rules, ${R.complying === R.pieces ? 'all within them' : 'some not'}. Every design keeps at least ${keep === 0.5 ? 'half' : 'a tenth of'} the floor standing now${R.minFrontage > 0 ? ` and walls ${Math.round(100 * R.minFrontage)}% of the street line` : ''}. ` +
      `The Sun at 10:00–14:00 solar time on the winter solstice; the air ${AIR.day} °C by day and ${AIR.night} °C by night, declared. Lots are the footprints as built. Shading is of prisms on flat ground; heat a balance of the envelope with the domain's fabric, no storage or ventilation.`;
  }

  // The front: floor against winter sun on the streets, each design a dot coloured by its passage; a dot chosen is drawn.
  function drawFront() {
    const W = 300, H = 170, L = 40, B = 26, T = 8, Rr = 8, D = R.designs;
    const xs = D.map((d) => d.measures.floor).concat(R.asBuilt.floor), ys = D.map((d) => d.measures.streetSun).concat(R.asBuilt.streetSun);
    const [x0, x1] = [Math.min(...xs), Math.max(...xs)], [y0, y1] = [Math.min(...ys), Math.max(...ys)];
    const X = (x) => L + ((x - x0) / ((x1 - x0) || 1)) * (W - L - Rr), Y = (y) => H - B - ((y - y0) / ((y1 - y0) || 1)) * (H - B - T);
    const NS = 'http://www.w3.org/2000/svg', el = (t, a, ...k) => { const n = document.createElementNS(NS, t); for (const [q, v] of Object.entries(a)) n.setAttribute(q, v); n.append(...k); return n; };
    const svg = el('svg', { viewBox: `0 0 ${W} ${H}`, width: '100%', class: 'ad-plot' });
    const ink = night ? '#d9c8dc' : '#4a3b52';
    svg.append(el('line', { x1: L, y1: H - B, x2: W - Rr, y2: H - B, stroke: ink, 'stroke-width': 0.6 }), el('line', { x1: L, y1: T, x2: L, y2: H - B, stroke: ink, 'stroke-width': 0.6 }));
    svg.append(el('text', { x: (L + W) / 2, y: H - 6, 'text-anchor': 'middle', fill: ink, 'font-size': 9 }, 'floor (m²)'));
    svg.append(el('text', { x: 10, y: (T + H - B) / 2, 'text-anchor': 'middle', fill: ink, 'font-size': 9, transform: `rotate(-90 10 ${(T + H - B) / 2})` }, 'winter sun on the streets'));
    D.forEach((d, i) => {
      const c = el('circle', { cx: X(d.measures.floor), cy: Y(d.measures.streetSun), r: i === index ? 5 : 3, fill: variantColour[d.variant] || ink, 'fill-opacity': i === index ? 1 : 0.7,
        stroke: i === index ? ink : 'none', 'stroke-width': 1.2, style: 'cursor:pointer' });
      c.addEventListener('click', () => { index = i; chosen = 'one'; for (const b of picks.querySelectorAll('button')) b.classList.remove('on'); token++; draw(); });
      svg.append(c);
    });
    const bx = X(R.asBuilt.floor), by = Y(R.asBuilt.streetSun);
    svg.append(el('rect', { x: bx - 4, y: by - 4, width: 8, height: 8, fill: 'none', stroke: ink, 'stroke-width': 1.2 }), el('text', { x: bx + 7, y: by + 3, fill: ink, 'font-size': 9 }, 'as built'));
    const legend = h('div', { class: 'ld-quiet' }, R.variants.map((v, k) => h('span', { style: `margin-right:10px;white-space:nowrap` }, h('i', { style: `display:inline-block;width:8px;height:8px;border-radius:50%;background:${variantColour[k]};margin-right:4px` }), v.name)));
    front.replaceChildren(svg, legend);
  }

  async function design() {
    if (!centre) return;
    const me = ++token;
    const streets = waysNear(world, centre, 700, (c) => STREETS.has(c));
    const ways = waysNear(world, centre, 1100, (c) => !NOT_WALKED.has(c));
    const buildings = buildingsNear(gis, centre, 600);
    if (!streets.length) { status.textContent = 'The streets here are still coming: try again in a moment.'; return; }
    const day = winterDay(latlon[0], latlon[1], new Date(ctx.clock?.instant || Date.now()).getUTCFullYear());
    status.textContent = `Finding the block, its ${buildings.length.toLocaleString('en')} buildings about it, and searching what could stand there…`;
    try {
      const r = await ctx.ask('design', { at: centre, streets, ways, buildings, rules: RULES[preset].rules, suns: day.suns, night: { air: AIR.night, hours: 24 - day.suns.length }, generations, least_floor: keep });
      if (me !== token || dead) return;
      R = r;
      R.day = day;
    } catch (e) { status.textContent = String(e.message || e); return; }
    choose(chosen === 'one' ? 'balanced' : chosen);
    picks.replaceChildren(keys(PICKS, chosen, (v) => { choose(v); token++; draw(); }));
    const n = (v) => Math.round(v).toLocaleString('en');
    if (!R.designs.length) {
      // Nothing feasible: say which limit binds.
      const binds = R.plotRatioFloor >= 0 && R.plotRatioFloor < R.leastFloor
        ? `its plot ratio allows at most ${n(R.plotRatioFloor)} m² on these lots, less than the ${n(R.leastFloor)} m² asked (${keep === 0.5 ? 'half' : 'a tenth of'} the floor standing now)`
        : `no design found keeps ${keep === 0.5 ? 'half' : 'a tenth of'} the floor standing now (${n(R.leastFloor)} m²)${R.minFrontage > 0 ? ` with ${Math.round(100 * R.minFrontage)}% of the street line walled` : ''}`;
      status.textContent = `Nothing under "${RULES[preset].name}" can stand here: ${binds}. The block as it stands is shown.`;
    } else status.textContent = `The block's designs under "${RULES[preset].name}". Choose one by what it does best, or on the front.`;
    await draw();
  }

  const legend = h('div', { class: 'card gs-card' }, style, h('b', {}, 'What could be built'),
    h('div', { class: 'gs-row' }, h('span', {}, 'rules'), keys(Object.entries(RULES).map(([k, v]) => [k, v.name]), preset, (v) => { preset = v; says.textContent = RULES[v].says; design(); })),
    says,
    h('div', { class: 'gs-row' }, h('span', {}, 'keep'), keys([[0.5, 'half the floor now'], [0.1, 'a tenth']], keep, (v) => { keep = v; design(); })),
    h('div', { class: 'gs-row' }, h('span', {}, 'search'), keys([[30, 'quick'], [60, 'thorough'], [150, 'long']], generations, (v) => { generations = v; design(); })),
    status, picks, readout, front, note,
    h('div', { class: 'ld-quiet' }, h('button', { class: 'ld-link', onclick: () => { if (R?.day) ctx.clock?.set?.({ date: R.day.date, hour: 12 }); } }, 'set the clock to the winter day at noon')),
    h('div', { class: 'ld-quiet' }, 'Streets and buildings: Overture Maps, OpenStreetMap. The Sun: the astronomy domain. The measures: the architecture domain (rules, form, frontage, sun and shade, the day\'s heat) and urban science (configuration after Hillier and Hanson; reach on the ways). The search: heuresis\'s NSGA-III (Deb and Jain 2014).'));
  ctx.onDispose(() => { dead = true; });
  return {
    legend,
    pickAt(lat, lon) {
      const p = frame.toLocal(lat, lon);
      centre = [p[0], p[1]];
      latlon = [lat, lon];
      design();
      return { kind: 'place', title: 'What could be built', lines: [RULES[preset].name], at: { lat, lon } };
    },
    // The front for the Studio, when the view is captured: floor against each other measure.
    figures() {
      if (!R || !R.designs.length) return [];
      const D = R.designs, x = D.map((d) => d.measures.floor);
      return MEASURES.slice(1).map(([key, name, unit]) => ({ chart: 'scatter', title: `Floor and ${name}`, x, y: D.map((d) => d.measures[key]), xlabel: 'floor (m²)', ylabel: `${name} (${unit})` }));
    },
    dispose() {
      dead = true;
      hollow(false);
      for (const id of [solidsId, sunId, passageId, blockId]) gis.remove(id);
    },
  };
}
