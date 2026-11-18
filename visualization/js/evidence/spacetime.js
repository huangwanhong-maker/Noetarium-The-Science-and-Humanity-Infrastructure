// Evidence in space and time drawn as a space-time cube (ADR 0127), after Hägerstrand's time geography: the place on the floor, time
// rising above it, each item of evidence where and when it speaks (a bar over its span, an ellipse for its radius), each person's
// possible whereabouts between the items that place them drawn as prisms (the intersection of two cones of their speed, kinesis's
// prisms), a theory's paths as threads through its events, and a plane at a chosen instant on which the slices show where each
// person could then have been. The same model drawn from above at one instant is the plan.
//
// It takes the matter (noetarium.matter/1, for its place) and what the API's theories engine answered (api/cpp/science/
// theories.hpp: the evidence in seconds after the matter's epoch, each person's fixes, the theories kept). sliceAt, lensOf,
// positionAt and projector mirror kinesis's prisms and need no page; cubeOf builds the model; svgCube and svgPlan return markup
// (spacetime.css gives it a look). The geometry is the engine's: Euclidean, straight paths, no obstacle.

import { esc } from '../history/cone.js';
import { INKS } from '../dynamics/charts.js';

export const inkOf = (k) => INKS[((k % INKS.length) + INKS.length) % INKS.length];

// A number of seconds after the epoch as a clock time on the epoch's day (the engine's epoch.seconds_of_day).
export function clockOf(epoch, t, withSeconds = true) {
  if (!Number.isFinite(t)) return '—';
  const sod = epoch?.given ? epoch.seconds_of_day : 0;
  let s = Math.round(sod + t);
  const day = Math.floor(s / 86400);
  s -= day * 86400;
  const pad = (v) => String(v).padStart(2, '0');
  const text = `${pad(Math.floor(s / 3600))}:${pad(Math.floor(s / 60) % 60)}${withSeconds ? `:${pad(s % 60)}` : ''}`;
  return day ? `${text} (${day > 0 ? '+' : ''}${day} d)` : text;
}

// The discs whose intersection holds every place a person can be at t between their fixes (kinesis::prisms::slice).
export function sliceAt(fixes, speed, t) {
  const f = [...fixes].filter((x) => Number.isFinite(x.t)).sort((a, b) => a.t - b.t);
  if (!f.length || !(speed > 0)) return [];
  if (t <= f[0].t) return [{ x: f[0].x, y: f[0].y, r: (f[0].radius || 0) + speed * (f[0].t - t) }];
  const last = f[f.length - 1];
  if (t >= last.t) return [{ x: last.x, y: last.y, r: (last.radius || 0) + speed * (t - last.t) }];
  let k = 0;
  while (k + 1 < f.length && f[k + 1].t < t) k++;
  return [{ x: f[k].x, y: f[k].y, r: (f[k].radius || 0) + speed * (t - f[k].t) }, { x: f[k + 1].x, y: f[k + 1].y, r: (f[k + 1].radius || 0) + speed * (f[k + 1].t - t) }];
}

// The intersection of discs as a polygon: points of each boundary kept where they lie within every other disc, in order of angle
// about their mean. Empty when the discs share no point; a disc within the others is its own circle.
export function lensOf(discs, n = 96) {
  if (!discs.length) return [];
  const inside = (p, skip) => discs.every((d, k) => k === skip || Math.hypot(p[0] - d.x, p[1] - d.y) <= d.r * (1 + 1e-9) + 1e-9);
  const pts = [];
  discs.forEach((d, k) => {
    for (let i = 0; i < n; i++) {
      const a = (2 * Math.PI * i) / n, p = [d.x + d.r * Math.cos(a), d.y + d.r * Math.sin(a)];
      if (inside(p, k)) pts.push(p);
    }
  });
  // The corners where two circles cross, so that a lens keeps its points.
  for (let i = 0; i < discs.length; i++)
    for (let j = i + 1; j < discs.length; j++) {
      const a = discs[i], b = discs[j], d = Math.hypot(b.x - a.x, b.y - a.y);
      if (!(d > 0) || d > a.r + b.r || d < Math.abs(a.r - b.r)) continue;
      const l = (a.r * a.r - b.r * b.r + d * d) / (2 * d), h = Math.sqrt(Math.max(0, a.r * a.r - l * l));
      const mx = a.x + (l * (b.x - a.x)) / d, my = a.y + (l * (b.y - a.y)) / d;
      for (const s of [1, -1]) {
        const p = [mx + (s * h * -(b.y - a.y)) / d, my + (s * h * (b.x - a.x)) / d];
        if (inside(p, -1)) pts.push(p);
      }
    }
  if (pts.length < 3) return pts.length ? pts : [];
  const cx = pts.reduce((s, p) => s + p[0], 0) / pts.length, cy = pts.reduce((s, p) => s + p[1], 0) / pts.length;
  return pts.sort((p, q) => Math.atan2(p[1] - cy, p[0] - cx) - Math.atan2(q[1] - cy, q[0] - cx));
}

// The place of a history at t: straight steps between its points (resting before the first and after the last).
export function positionAt(points, t) {
  const a = [...points].sort((p, q) => p.t - q.t);
  if (!a.length) return null;
  if (t <= a[0].t) return [a[0].x, a[0].y];
  if (t >= a[a.length - 1].t) return [a[a.length - 1].x, a[a.length - 1].y];
  let k = 0;
  while (k + 1 < a.length && a[k + 1].t < t) k++;
  const s = a[k + 1].t > a[k].t ? (t - a[k].t) / (a[k + 1].t - a[k].t) : 0;
  return [a[k].x + s * (a[k + 1].x - a[k].x), a[k].y + s * (a[k + 1].y - a[k].y)];
}

// A projection of space and time to the page: the plane turned by `azimuth` about the vertical, tilted by `elevation` (π/2 from
// above, 0 from the side), time rising at `rise` units of length a second from t0. Returns page coordinates, y down.
export function projector({ azimuth = -0.6, elevation = 0.5, rise = 1, t0 = 0, cx = 0, cy = 0 } = {}) {
  const ca = Math.cos(azimuth), sa = Math.sin(azimuth), se = Math.sin(elevation), ce = Math.cos(elevation);
  return (x, y, t = t0) => {
    const X = (x - cx) * ca - (y - cy) * sa, Y = (x - cx) * sa + (y - cy) * ca, Z = (t - t0) * rise;
    return [X, -Y * se - Z * ce];
  };
}

// Round steps of clock time for ticks: the first of 1, 2, 5, 10, 15, 30, 60 minutes giving at most `most` ticks.
export function timeTicksOf(t0, t1, epoch, most = 8) {
  const out = [];
  if (!(t1 > t0)) return out;
  const sod = epoch?.given ? epoch.seconds_of_day : 0;
  const step = [60, 120, 300, 600, 900, 1800, 3600, 7200].find((s) => (t1 - t0) / s <= most) || 7200;
  for (let a = Math.ceil((sod + t0) / step) * step; a <= sod + t1; a += step) out.push({ t: a - sod, words: clockOf(epoch, a - sod, false) });
  return out;
}

// ---- The model ------------------------------------------------------------------------------------------------------------------

// What is drawn: the window of time and the extent of the plane, the place's geometry, the items, each person with their fixes and
// speed, and the theory chosen (one of reply.kept), in the matter's metres and seconds after its epoch.
export function cubeOf(matter, reply, options = {}) {
  const ev = reply?.evidence || { agents: [], located: [], rooms: [], epoch: { given: false } };
  const rooms = ev.rooms || [];
  const items = (ev.located || []).filter((i) => i.says !== 'interval');
  // The window: about the rooms where events may be, or the items' span.
  let t0, t1;
  if (Number.isFinite(options.from) && Number.isFinite(options.until)) [t0, t1] = [options.from, options.until];
  else if (rooms.length) [t0, t1] = [Math.min(...rooms.map((r) => r.from)) - 900, Math.max(...rooms.map((r) => r.until)) + 600];
  else if (items.length) [t0, t1] = [Math.min(...items.map((i) => i.from)), Math.max(...items.map((i) => i.until))];
  else [t0, t1] = [0, 3600];
  if (!(t1 > t0)) t1 = t0 + 600;
  const inWindow = (i) => i.until >= t0 && i.from <= t1;
  // The extent: the scene, the rooms and the centres of the items within the window, padded.
  const scene = matter?.place?.scene || { x: 0, y: 0, name: 'the scene' };
  const pts = [[scene.x, scene.y], ...rooms.map((r) => [r.x, r.y]), ...items.filter(inWindow).map((i) => [i.x, i.y])];
  for (const a of ev.agents || []) for (const f of a.fixes || []) if (f.t >= t0 - 1800 && f.t <= t1 + 1800 && f.radius < 300) pts.push([f.x, f.y]);
  let west = Math.min(...pts.map((p) => p[0])), east = Math.max(...pts.map((p) => p[0])), south = Math.min(...pts.map((p) => p[1])), north = Math.max(...pts.map((p) => p[1]));
  const size0 = Math.max(200, east - west, north - south), pad = size0 * 0.12;
  west -= pad, east += pad, south -= pad, north += pad;
  let cx = (west + east) / 2, cy = (south + north) / 2, half = Math.max(east - west, north - south) / 2;
  // Closer, about the scene: the extent shrinks by the zoom and its centre moves toward the scene.
  const zoom = Math.max(1, Number(options.zoom) || 1);
  if (zoom > 1) {
    const f = 1 - 1 / zoom;
    cx += (scene.x - cx) * f, cy += (scene.y - cy) * f, half /= zoom;
  }
  west = cx - half, east = cx + half, south = cy - half, north = cy + half;
  const theory = options.theory ?? null;
  const readings = new Map((theory?.readings || []).map((r) => [r.item, r]));
  const agents = (ev.agents || []).map((a, k) => ({ ...a, ink: inkOf(k), index: k }));
  const inkFor = new Map(agents.map((a) => [a.id, a.ink]));
  return {
    t0, t1, west, east, south, north, cx, cy, half, zoom, scene, epoch: ev.epoch,
    place: matter?.place || {},
    rooms,
    agents,
    items: items.map((i) => ({ ...i, inWindow: inWindow(i), ink: i.agent ? inkFor.get(i.agent) || '#8f8494' : null, reading: readings.get(i.id) || null })),
    outside: items.filter((i) => !inWindow(i)).map((i) => i.id),
    theory: theory ? { ...theory, trajectories: (theory.trajectories || []).map((tr) => ({ ...tr, ink: inkFor.get(tr.agent) || '#8f8494' })) } : null,
  };
}

// ---- Drawing --------------------------------------------------------------------------------------------------------------------

const f1 = (v) => (Math.round(v * 10) / 10).toFixed(1);

function pathOf(points, close = false) {
  if (!points.length) return '';
  return points.map((p, k) => `${k ? 'L' : 'M'}${f1(p[0])},${f1(p[1])}`).join('') + (close ? 'Z' : '');
}

// The convex hull of points of the page (Andrew's monotone chain), counter-clockwise.
function hull(points) {
  const p = [...points].sort((a, b) => a[0] - b[0] || a[1] - b[1]);
  if (p.length < 3) return p;
  const cross = (o, a, b) => (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0]);
  const lower = [], upper = [];
  for (const q of p) { while (lower.length >= 2 && cross(lower[lower.length - 2], lower[lower.length - 1], q) <= 0) lower.pop(); lower.push(q); }
  for (const q of [...p].reverse()) { while (upper.length >= 2 && cross(upper[upper.length - 2], upper[upper.length - 1], q) <= 0) upper.pop(); upper.push(q); }
  return [...lower.slice(0, -1), ...upper.slice(0, -1)];
}

// The polygon of a ring of the plane clipped to a disc about the extent's centre (so that a cone of a large fix stays on the page).
function clipped(model, discs) {
  return lensOf([...discs, { x: model.cx, y: model.cy, r: model.half * Math.SQRT2 }], 72);
}

// The prisms of a person: between consecutive fixes, slices at `m` instants, each a ring at its height.
export function prismRings(model, agent, m = 7) {
  const f = [...(agent.fixes || [])].sort((a, b) => a.t - b.t);
  const rings = [];
  for (let k = 0; k + 1 < f.length; k++) {
    const a = Math.max(f[k].t, model.t0), b = Math.min(f[k + 1].t, model.t1);
    if (!(b > a)) continue;
    if ((f[k].radius || 0) > model.half || (f[k + 1].radius || 0) > model.half) continue;  // a fix wider than the plane says little here
    for (let s = 0; s <= m; s++) {
      const t = a + ((b - a) * s) / m;
      const ring = clipped(model, sliceAt(f, agent.speed, t));
      if (ring.length > 2) rings.push({ t, ring });
    }
  }
  return rings;
}

function ellipse(project, x, y, r, t, n = 28) {
  const pts = [];
  for (let i = 0; i < n; i++) {
    const a = (2 * Math.PI * i) / n;
    pts.push(project(x + r * Math.cos(a), y + r * Math.sin(a), t));
  }
  return pathOf(pts, true);
}

// The cube as markup. options: azimuth, elevation (radians), at (an instant for the plane, or null), selected (an item's id),
// prisms (true), threads (true: the evidence's own threads between fixes), labels (true).
export function svgCube(model, options = {}) {
  const { t0, t1, west, east, south, north, epoch } = model;
  const rise = (2 * model.half * (options.height ?? 0.95)) / (t1 - t0);
  const project = projector({ azimuth: options.azimuth ?? -0.6, elevation: options.elevation ?? 0.5, rise, t0, cx: model.cx, cy: model.cy });
  const at = Number.isFinite(options.at) ? Math.min(t1, Math.max(t0, options.at)) : null;
  const P = (x, y, t) => project(x, y, t);
  const corners = [[west, south], [east, south], [east, north], [west, north]];
  const parts = [];
  const box = { x0: Infinity, x1: -Infinity, y0: Infinity, y1: -Infinity };
  for (const [x, y] of corners) for (const t of [t0, t1]) {
    const [sx, sy] = P(x, y, t);
    box.x0 = Math.min(box.x0, sx), box.x1 = Math.max(box.x1, sx), box.y0 = Math.min(box.y0, sy), box.y1 = Math.max(box.y1, sy);
  }
  const floorPath = pathOf(corners.map(([x, y]) => P(x, y, t0)), true);
  const tag = Math.abs(Math.round(model.cx + model.cy * 7 + t0 + 1000 * (model.zoom || 1)));
  const clipId = `st-floor-${tag}`, boxId = `st-box-${tag}`;
  const silhouette = hull(corners.flatMap(([x, y]) => [P(x, y, t0), P(x, y, t1)]));
  parts.push(`<defs><clipPath id="${clipId}"><path d="${floorPath}"/></clipPath><clipPath id="${boxId}"><path d="${pathOf(silhouette, true)}"/></clipPath></defs>`);
  // The floor: the ground, water, buildings, ways, the scene.
  parts.push(`<path class="st-floor" d="${floorPath}"/>`);
  parts.push(`<g class="st-place" clip-path="url(#${clipId})">`);
  for (const w of model.place.water || []) parts.push(`<path class="st-water" d="${pathOf(w.points.map(([x, y]) => P(x, y, t0)), true)}"><title>${esc(w.name || 'water')}</title></path>`);
  for (const w of model.place.ways || []) parts.push(`<path class="st-way ${esc(w.kind || 'way')}" d="${pathOf(w.points.map(([x, y]) => P(x, y, t0)))}"><title>${esc(w.name || '')}</title></path>`);
  for (const b of model.place.buildings || []) parts.push(`<path class="st-building" d="${pathOf(b.points.map(([x, y]) => P(x, y, t0)), true)}"><title>${esc(b.name || '')}</title></path>`);
  parts.push('</g>');
  // The frame of the cube: the vertical edges, the ceiling, and the clock on the edge farthest back.
  const back = corners.reduce((best, c) => (P(c[0], c[1], t0)[1] < P(best[0], best[1], t0)[1] ? c : best), corners[0]);
  for (const [x, y] of corners) {
    const [a, b] = [P(x, y, t0), P(x, y, t1)];
    parts.push(`<line class="st-edge${x === back[0] && y === back[1] ? ' back' : ''}" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/>`);
  }
  parts.push(`<path class="st-ceiling" d="${pathOf(corners.map(([x, y]) => P(x, y, t1)), true)}"/>`);
  const left = corners.reduce((best, c) => (P(c[0], c[1], t0)[0] < P(best[0], best[1], t0)[0] ? c : best), corners[0]);
  for (const tk of timeTicksOf(t0, t1, epoch)) {
    const [a, b] = [P(left[0], left[1], tk.t), P(left[0] + (east - west) * 0.03, left[1], tk.t)];
    parts.push(`<line class="st-tick" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/><text class="st-clock" x="${f1(a[0] - 6)}" y="${f1(a[1] + 3)}" text-anchor="end">${esc(tk.words)}</text>`);
    box.x0 = Math.min(box.x0, a[0] - 52);
  }
  // The scene on the floor, and the rooms where events may be as cylinders.
  const sc = P(model.scene.x, model.scene.y, t0);
  parts.push(`<circle class="st-scene" cx="${f1(sc[0])}" cy="${f1(sc[1])}" r="3"><title>${esc(model.scene.name || 'the scene')}</title></circle>`);
  for (const r of model.rooms) {
    const a = Math.max(t0, r.from), b = Math.min(t1, r.until);
    if (!(b > a)) continue;
    const lo = ellipse(P, r.x, r.y, Math.max(r.radius, model.half * 0.012), a), hi = ellipse(P, r.x, r.y, Math.max(r.radius, model.half * 0.012), b);
    const [p0, p1] = [P(r.x, r.y, a), P(r.x, r.y, b)];
    parts.push(`<g class="st-room"><path d="${lo}"/><path d="${hi}"/><line x1="${f1(p0[0])}" y1="${f1(p0[1])}" x2="${f1(p1[0])}" y2="${f1(p1[1])}"/><title>${esc(`where and when ${r.kind} may be: within ${r.radius} m, ${r.from_clock || clockOf(epoch, r.from)} to ${r.until_clock || clockOf(epoch, r.until)}`)}</title></g>`);
  }
  // What stands in the cube is cut at its faces.
  parts.push(`<g class="st-within" clip-path="url(#${boxId})">`);
  // The prisms, faint, a person at a time.
  if (options.prisms !== false)
    for (const a of model.agents) {
      const rings = prismRings(model, a);
      if (!rings.length) continue;
      parts.push(`<g class="st-prism" data-agent="${esc(a.id)}" style="stroke:${a.ink}">${rings.map(({ t, ring }) => `<path d="${pathOf(ring.map(([x, y]) => P(x, y, t)), true)}"/>`).join('')}<title>${esc(`where ${a.name} could have been between the items that place them, at ${a.speed} m/s`)}</title></g>`);
    }
  // The evidence's own threads: each person's fixes joined, dotted.
  if (options.threads !== false)
    for (const a of model.agents) {
      const f = (a.fixes || []).filter((x) => x.t >= t0 && x.t <= t1).sort((p, q) => p.t - q.t);
      if (f.length > 1) parts.push(`<path class="st-thread" style="stroke:${a.ink}" d="${pathOf(f.map((x) => P(x.x, x.y, x.t)))}"/>`);
    }
  // The items: a bar over the span, an ellipse for the radius at the middle, a glyph by what it says.
  const onPlane = (i) => Math.abs(i.x - model.cx) <= model.half * 1.02 && Math.abs(i.y - model.cy) <= model.half * 1.02;
  const sorted = model.items.filter((i) => i.inWindow && onPlane(i)).sort((p, q) => P(q.x, q.y, (q.from + q.until) / 2)[1] - P(p.x, p.y, (p.from + p.until) / 2)[1]);
  parts.push('<g class="st-items">');
  for (const i of sorted) {
    const a = Math.max(t0, i.from), b = Math.min(t1, i.until), mid = (a + b) / 2;
    const [p, q, m] = [P(i.x, i.y, a), P(i.x, i.y, b), P(i.x, i.y, mid)];
    const doubted = i.reading && !i.reading.believed;
    const cls = `st-item ${esc(i.says)}${i.held_back ? ' held' : ''}${doubted ? ' doubted' : ''}${options.selected === i.id ? ' chosen' : ''}`;
    const ink = i.ink || 'var(--st-accent)';
    const title = `${i.id}: ${i.text || i.says}${i.agent ? ` (${i.agent})` : ''}, ${i.from_clock}${i.until > i.from ? ` to ${i.until_clock}` : ''}, within ${i.radius} m; credibility ${i.credibility}${i.held_back ? ' — held back from the search' : ''}${doubted ? ' — doubted by this theory' : ''}`;
    const g = [];
    if (i.radius > 0 && i.radius < model.half * 1.5) g.push(`<path class="st-error" d="${ellipse(P, i.x, i.y, i.radius, mid)}" style="stroke:${ink}"/>`);
    if (b > a) g.push(`<line class="st-span" x1="${f1(p[0])}" y1="${f1(p[1])}" x2="${f1(q[0])}" y2="${f1(q[1])}" style="stroke:${ink}"/>`);
    const s = 5;
    if (i.says === 'sighting') g.push(`<circle class="st-glyph" cx="${f1(m[0])}" cy="${f1(m[1])}" r="${s}" style="fill:${ink}"/>`);
    else if (i.says === 'event') g.push(`<path class="st-glyph" d="M${f1(m[0])},${f1(m[1] - s - 1)}L${f1(m[0] + s + 1)},${f1(m[1])}L${f1(m[0])},${f1(m[1] + s + 1)}L${f1(m[0] - s - 1)},${f1(m[1])}Z"/>`);
    else g.push(`<circle class="st-glyph ring" cx="${f1(m[0])}" cy="${f1(m[1])}" r="${s}"/><line class="st-glyph slash" x1="${f1(m[0] - s)}" y1="${f1(m[1] + s)}" x2="${f1(m[0] + s)}" y2="${f1(m[1] - s)}"/>`);
    if (doubted) g.push(`<path class="st-cross" d="M${f1(m[0] - 8)},${f1(m[1] - 8)}L${f1(m[0] + 8)},${f1(m[1] + 8)}M${f1(m[0] - 8)},${f1(m[1] + 8)}L${f1(m[0] + 8)},${f1(m[1] - 8)}"/>`);
    if (options.labels !== false) g.push(`<text class="st-label" x="${f1(m[0] + 8)}" y="${f1(m[1] - 6)}">${esc(i.id)}</text>`);
    parts.push(`<g class="${cls}" data-item="${esc(i.id)}"><title>${esc(title)}</title>${g.join('')}</g>`);
  }
  parts.push('</g>');
  // The theory chosen: its paths, bold, and its events.
  if (model.theory) {
    parts.push('<g class="st-theory">');
    for (const tr of model.theory.trajectories) {
      const w = [...(tr.witness || [])].sort((p, q) => p.t - q.t);
      if (!w.length) continue;
      const pts = [];
      const within = w.filter((x) => x.t > t0 && x.t < t1);
      const start = positionAt(w, t0), end = positionAt(w, t1);
      pts.push(P(start[0], start[1], t0));
      for (const x of within) pts.push(P(x.x, x.y, x.t));
      pts.push(P(end[0], end[1], t1));
      parts.push(`<path class="st-path${tr.status === 'consistent' ? '' : ' broken'}" data-agent="${esc(tr.agent)}" style="stroke:${tr.ink}" d="${pathOf(pts)}"><title>${esc(`${tr.agent}'s path in this theory: ${tr.reason || tr.status}`)}</title></path>`);
    }
    for (const e of model.theory.events || []) {
      if (e.t < t0 || e.t > t1) continue;
      const m = P(e.x, e.y, e.t), who = Object.values(e.roles || {}).filter(Boolean).map((id) => model.agents.find((a) => a.id === id)?.name || id).join(', ');
      parts.push(`<g class="st-event"><path d="M${f1(m[0])},${f1(m[1] - 8)}L${f1(m[0] + 8)},${f1(m[1])}L${f1(m[0])},${f1(m[1] + 8)}L${f1(m[0] - 8)},${f1(m[1])}Z"/><text x="${f1(m[0] + 12)}" y="${f1(m[1] + 4)}">${esc(`${e.kind}${who ? ` by ${who}` : ''}, ${e.clock || clockOf(epoch, e.t)}`)}</text></g>`);
    }
    parts.push('</g>');
  }
  // The plane at an instant, and on it where each person could then have been (within the cube; its clock outside).
  if (at === null) parts.push('</g>');
  if (at !== null) {
    parts.push(`<g class="st-plane"><path class="st-sheet" d="${pathOf(corners.map(([x, y]) => P(x, y, at)), true)}"/>`);
    for (const a of model.agents) {
      const ring = clipped(model, sliceAt(a.fixes || [], a.speed, at));
      if (ring.length > 2) parts.push(`<path class="st-slice" data-agent="${esc(a.id)}" style="stroke:${a.ink};fill:${a.ink}" d="${pathOf(ring.map(([x, y]) => P(x, y, at)), true)}"><title>${esc(`where ${a.name} could have been at ${clockOf(epoch, at)}`)}</title></path>`);
    }
    if (model.theory)
      for (const tr of model.theory.trajectories) {
        const p = positionAt(tr.witness || [], at);
        if (!p) continue;
        const m = P(p[0], p[1], at);
        parts.push(`<circle class="st-here" cx="${f1(m[0])}" cy="${f1(m[1])}" r="4.5" style="fill:${tr.ink}"><title>${esc(`${tr.agent} in this theory at ${clockOf(epoch, at)}`)}</title></circle>`);
      }
    const [lx, ly] = P(left[0], left[1], at);
    parts.push(`</g></g><text class="st-at" x="${f1(lx - 6)}" y="${f1(ly - 6)}" text-anchor="end">${esc(clockOf(epoch, at))}</text>`);
  }
  const padX = (box.x1 - box.x0) * 0.04, padY = (box.y1 - box.y0) * 0.05;
  return `<svg class="spacetime" viewBox="${f1(box.x0 - padX)} ${f1(box.y0 - padY)} ${f1(box.x1 - box.x0 + 2 * padX + 120)} ${f1(box.y1 - box.y0 + 2 * padY)}" preserveAspectRatio="xMidYMid meet" role="img" aria-label="the evidence in space and time: the place on the floor, time rising">${parts.join('')}</svg>`;
}

// The plan at an instant, from above: the place, where each person could then have been (their slices), the items speaking within
// `window` seconds of it, and the theory's people where it puts them.
export function svgPlan(model, options = {}) {
  const at = Number.isFinite(options.at) ? options.at : (model.t0 + model.t1) / 2, window = options.window ?? 300;
  const { west, east, south, north, epoch } = model;
  const P = (x, y) => [x, -y];
  const parts = [];
  parts.push(`<rect class="st-floor" x="${f1(west)}" y="${f1(-north)}" width="${f1(east - west)}" height="${f1(north - south)}"/>`);
  for (const w of model.place.water || []) parts.push(`<path class="st-water" d="${pathOf(w.points.map(([x, y]) => P(x, y)), true)}"/>`);
  for (const w of model.place.ways || []) parts.push(`<path class="st-way ${esc(w.kind || 'way')}" d="${pathOf(w.points.map(([x, y]) => P(x, y)))}"/>`);
  for (const b of model.place.buildings || []) parts.push(`<path class="st-building" d="${pathOf(b.points.map(([x, y]) => P(x, y)), true)}"><title>${esc(b.name || '')}</title></path>`);
  for (const a of model.agents) {
    const ring = clipped(model, sliceAt(a.fixes || [], a.speed, at));
    if (ring.length > 2) parts.push(`<path class="st-slice" data-agent="${esc(a.id)}" style="stroke:${a.ink};fill:${a.ink}" d="${pathOf(ring.map(([x, y]) => P(x, y)), true)}"><title>${esc(`where ${a.name} could have been at ${clockOf(epoch, at)}`)}</title></path>`);
  }
  for (const i of model.items) {
    if (i.until < at - window || i.from > at + window) continue;
    const [x, y] = P(i.x, i.y);
    parts.push(`<g class="st-item ${esc(i.says)}${i.held_back ? ' held' : ''}" data-item="${esc(i.id)}"><circle class="st-error" cx="${f1(x)}" cy="${f1(y)}" r="${f1(Math.min(i.radius, model.half * 2))}" style="stroke:${i.ink || 'var(--st-accent)'}"/><circle class="st-glyph" cx="${f1(x)}" cy="${f1(y)}" r="${f1(model.half * 0.012)}" style="fill:${i.ink || 'var(--st-accent)'}"/><title>${esc(`${i.id}: ${i.text || i.says}, ${i.from_clock}`)}</title></g>`);
  }
  if (model.theory)
    for (const tr of model.theory.trajectories) {
      const p = positionAt(tr.witness || [], at);
      if (p) parts.push(`<circle class="st-here" cx="${f1(p[0])}" cy="${f1(-p[1])}" r="${f1(model.half * 0.018)}" style="fill:${tr.ink}"/>`);
    }
  const s = P(model.scene.x, model.scene.y);
  parts.push(`<circle class="st-scene" cx="${f1(s[0])}" cy="${f1(s[1])}" r="${f1(model.half * 0.01)}"/>`);
  return `<svg class="spacetime plan" viewBox="${f1(west)} ${f1(-north)} ${f1(east - west)} ${f1(north - south)}" preserveAspectRatio="xMidYMid meet" role="img" aria-label="${esc(`where each person could have been at ${clockOf(epoch, at)}`)}">${parts.join('')}</svg>`;
}
