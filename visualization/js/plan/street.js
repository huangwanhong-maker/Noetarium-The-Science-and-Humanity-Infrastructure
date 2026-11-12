// A street as a work shows it (ADR 0098): the way taken in hand from a place's ways, the strips it is divided into, its section
// drawn square across at a station, and the whole of it unrolled along its length. Pure: what follows of a street (the faces on
// either side, the width between, its enclosure and sky, the sun on its footpaths, how high each building may rise) is the
// urban-science domain's `section`, asked through the API (api/cpp/science/street.hpp); here are the document's side and the
// drawings of that answer.
//
// A STREET of the document is {name, line: [[x, y], …], strips: [{kind, width}], need, heights: [{at: [x, y], height}],
// scheme}: the centreline in the order it is walked; its strips from the left, looking along it; the hours of sun a footpath is
// to keep; buildings given another height than the map's, each named by a point within it; and what a scheme asks.

import { dist, inRing } from './geometry.js';

// The kinds of strip: its name, a usual width, the group it belongs to, its tone on a drawing, how far its surface stands above
// the carriageway.
export const STRIPS = {
  footpath: { title: 'Footpath', width: 2.5, group: 'people', tone: '#e6dcd3', rise: 0.15 },
  frontage: { title: 'Frontage', width: 1.5, group: 'people', tone: '#eadfd2', rise: 0.15 },
  shared: { title: 'Shared surface', width: 5.0, group: 'people', tone: '#e3d6c8', rise: 0.02 },
  cycle: { title: 'Cycle track', width: 1.8, group: 'cycles', tone: '#d9c3cf', rise: 0.1 },
  verge: { title: 'Verge with trees', width: 1.6, group: 'green', tone: '#cfdcc2', rise: 0.15 },
  median: { title: 'Median', width: 2.0, group: 'green', tone: '#cfdcc2', rise: 0.15 },
  bus: { title: 'Bus lane', width: 3.3, group: 'transit', tone: '#cdbfc3', rise: 0 },
  tram: { title: 'Tramway', width: 3.2, group: 'transit', tone: '#c9c1c6', rise: 0 },
  parking: { title: 'Parking', width: 2.3, group: 'vehicles', tone: '#cfc9c8', rise: 0 },
  lane: { title: 'Traffic lane', width: 3.2, group: 'vehicles', tone: '#c2bdbf', rise: 0 },
};
export const STRIP_GROUPS = [['people', 'People on foot'], ['cycles', 'Cycles'], ['green', 'Green'], ['transit', 'Transit'], ['vehicles', 'Vehicles']];

const esc = (s) => String(s).replace(/[&<>"]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
const f1 = (v) => v.toFixed(1);

// ---- the way in hand -----------------------------------------------------------------------------------------------------------

const lengthOf = (line) => { let L = 0; for (let i = 0; i + 1 < line.length; i++) L += dist(line[i], line[i + 1]); return L; };
function pointAt(line, s) {
  for (let i = 0; i + 1 < line.length; i++) {
    const L = dist(line[i], line[i + 1]);
    if (s <= L || i + 2 === line.length) { const t = L > 0 ? Math.max(0, Math.min(1, s / L)) : 0; return [line[i][0] + (line[i + 1][0] - line[i][0]) * t, line[i][1] + (line[i + 1][1] - line[i][1]) * t]; }
    s -= L;
  }
  return line[line.length - 1];
}
// The point of a line nearest p: {distance, along}.
export function nearestOnLine(line, p) {
  let best = { distance: Infinity, along: 0 }, run = 0;
  for (let i = 0; i + 1 < line.length; i++) {
    const a = line[i], b = line[i + 1], L = dist(a, b);
    const t = L > 0 ? Math.max(0, Math.min(1, ((p[0] - a[0]) * (b[0] - a[0]) + (p[1] - a[1]) * (b[1] - a[1])) / (L * L))) : 0;
    const d = dist(p, [a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t]);
    if (d < best.distance) best = { distance: d, along: run + t * L };
    run += L;
  }
  return best;
}

// The way of a place's ways nearest a point, among those a street can be (not a rail, not a path through a building): its index
// and how far it is; null when none lies within `reach`.
export function nearestWay(ways, p, reach = 30) {
  let best = null;
  (ways || []).forEach((w, index) => {
    if (w.rail || !w.line || w.line.length < 2) return;
    const near = nearestOnLine(w.line, p);
    if (near.distance <= reach && (!best || near.distance < best.distance)) best = { index, distance: near.distance, along: near.along };
  });
  return best;
}

// A street as it runs on: a map cuts a street into ways at its crossings. From one way, the ways of the same name that begin
// where it ends are joined to it, end to end, each walked the way that carries on, until `most` metres are joined or none carries
// on. A way without a name is itself alone. Answers the line, and how far along it the first way begins.
export function joinWays(ways, index, { most = 600, reach = 1.5 } = {}) {
  const first = ways[index], used = new Set([index]);
  let line = first.line.map((q) => q.slice()), before = 0;
  if (!first.name) return { line, before };
  for (const end of [1, 0]) {
    for (;;) {
      if (lengthOf(line) >= most) break;
      const tip = end ? line[line.length - 1] : line[0];
      let next = null;
      ways.forEach((w, i) => {
        if (used.has(i) || w.rail || w.name !== first.name || !w.line || w.line.length < 2) return;
        const a = dist(w.line[0], tip), b = dist(w.line[w.line.length - 1], tip);
        if (Math.min(a, b) <= reach && (!next || Math.min(a, b) < next.gap)) next = { i, gap: Math.min(a, b), flip: b < a };
      });
      if (!next) break;
      used.add(next.i);
      // Walked so that it leaves the tip: its first point is the tip's when it is joined after, its last when before.
      let more = ways[next.i].line.map((q) => q.slice());
      if (end ? next.flip : !next.flip) more.reverse();
      if (end) line = line.concat(more.slice(1));
      else { before += lengthOf(more); line = more.slice(0, -1).concat(line); }
    }
  }
  return { line, before };
}

// The stretch of a line within `half` metres either way of the place `along` it, as a line of its own, walked the same way.
export function stretch(line, along, half = 120) {
  const total = lengthOf(line), from = Math.max(0, along - half), to = Math.min(total, along + half), out = [pointAt(line, from)];
  let run = 0;
  for (let i = 0; i + 1 < line.length; i++) {
    run += dist(line[i], line[i + 1]);
    if (run > from + 1e-9 && run < to - 1e-9) out.push(line[i + 1].slice());
  }
  out.push(pointAt(line, to));
  return out.filter((q, i) => i === 0 || dist(q, out[i - 1]) > 1e-6);
}

// A division of a width into strips, to begin from: footpaths at the faces, parking where there is room, and lanes between.
// Presumed: a map does not say how a street is divided.
export function presumedStrips(width) {
  const w = Number.isFinite(width) ? Math.max(4, width) : 16, strip = (kind, x) => ({ kind, width: Math.round(x * 20) / 20 });
  if (w < 7.5) return [strip('shared', Math.min(w, 6))];
  if (w < 11) return [strip('footpath', (w - 5.6) / 2), strip('lane', 5.6), strip('footpath', (w - 5.6) / 2)];
  if (w < 15.5) return [strip('footpath', (w - 6.4) / 2), strip('lane', 3.2), strip('lane', 3.2), strip('footpath', (w - 6.4) / 2)];
  if (w < 21) return [strip('footpath', (w - 11) / 2), strip('parking', 2.3), strip('lane', 3.2), strip('lane', 3.2), strip('parking', 2.3), strip('footpath', (w - 11) / 2)];
  const path = Math.min(4.5, (w - 14.2) / 2);
  return [strip('footpath', path), strip('verge', 1.6), strip('parking', 2.3), strip('lane', 3.2), strip('lane', 3.2), strip('parking', 2.3), strip('verge', 1.6), strip('footpath', path)];
}

// The strips set out about the centreline, from the left: [{kind, from, to}], offsets negative to the left.
export function placeStrips(strips) {
  const total = (strips || []).reduce((s, x) => s + x.width, 0), out = [];
  let at = -total / 2;
  for (const s of strips || []) { out.push({ kind: s.kind, from: at, to: at + s.width }); at += s.width; }
  return out;
}

// A place's buildings with the heights a street's design gives them: each change names a building by a point within it.
export function withHeights(buildings, changes) {
  if (!changes || !changes.length) return buildings;
  return buildings.map((b) => { const c = changes.find((x) => inRing(x.at, b.ring)); return c ? { ...b, height: c.height, presumed: false, changed: true } : b; });
}

// A line moved to its left by `off` (to its right when negative), its corners mitred.
export function offsetLine(line, off) {
  const n = line.length, out = [];
  for (let i = 0; i < n; i++) {
    const a = line[Math.max(0, i - 1)], b = line[i], c = line[Math.min(n - 1, i + 1)];
    const d1 = i > 0 ? [(b[0] - a[0]) / (dist(a, b) || 1), (b[1] - a[1]) / (dist(a, b) || 1)] : null, d2 = i < n - 1 ? [(c[0] - b[0]) / (dist(b, c) || 1), (c[1] - b[1]) / (dist(b, c) || 1)] : null;
    const u = d1 || d2, v = d2 || d1, m = [-(u[1] + v[1]), u[0] + v[0]], L = Math.hypot(m[0], m[1]) || 1, cos = Math.max(0.35, (u[0] * v[0] + u[1] * v[1] + 1) / 2);
    const k = off / Math.sqrt(cos);
    out.push([b[0] + m[0] / L * k, b[1] + m[1] / L * k]);
  }
  return out;
}

// ---- the section, drawn square across --------------------------------------------------------------------------------------------

// The Sun as the section sees it: which side it stands on (+1 the left), and its angle above the ground in the section's plane
// (the profile angle: tan p = tan altitude / |cos of the angle between the Sun's bearing and the street's cross direction|).
export function profileSun(station, sun) {
  if (!sun || !(sun.altitude > 0)) return null;
  const a = sun.azimuth * Math.PI / 180, u = [Math.sin(a), Math.cos(a)], c = u[0] * station.left[0] + u[1] * station.left[1];
  if (Math.abs(c) < 0.02) return { side: 0, angle: Math.PI / 2 };
  return { side: c > 0 ? 1 : -1, angle: Math.atan2(Math.tan(sun.altitude * Math.PI / 180), Math.abs(c)) };
}

const figure = (x, ground, k, h = 1.7) => { const head = 0.11 * k, top = ground - h * k; return `<g class="sx-figure"><circle cx="${f1(x)}" cy="${f1(top + head)}" r="${f1(head)}"/><path d="M ${f1(x - 0.2 * k)} ${f1(top + 2.6 * head)} Q ${f1(x)} ${f1(top + 2.1 * head)} ${f1(x + 0.2 * k)} ${f1(top + 2.6 * head)} L ${f1(x + 0.1 * k)} ${f1(ground)} L ${f1(x - 0.1 * k)} ${f1(ground)} Z"/></g>`; };
const tree = (x, ground, k, h = 6.5, r = 2.2) => `<g class="sx-tree"><line x1="${f1(x)}" y1="${f1(ground)}" x2="${f1(x)}" y2="${f1(ground - (h - r) * k)}"/><ellipse cx="${f1(x)}" cy="${f1(ground - (h - r * 0.9) * k)}" rx="${f1(r * k)}" ry="${f1(r * 0.95 * k)}"/></g>`;
const box = (x, ground, k, w, h, cls, lift = 0.25) => `<rect class="${cls}" x="${f1(x - w * k / 2)}" y="${f1(ground - (h + lift) * k)}" width="${f1(w * k)}" height="${f1(h * k)}" rx="${f1(0.18 * k)}"/><circle class="sx-wheel" cx="${f1(x - w * k * 0.32)}" cy="${f1(ground - 0.3 * k)}" r="${f1(0.3 * k)}"/><circle class="sx-wheel" cx="${f1(x + w * k * 0.32)}" cy="${f1(ground - 0.3 * k)}" r="${f1(0.3 * k)}"/>`;

// A street's section at a station, within a box [x, y, w, h] of the page. `reading` is the engine's answer; `sun` the Sun now
// ({altitude, azimuth} in degrees), or none. Answers {svg, scale}.
export function sectionSvg(reading, index, { box: frame = [0, 0, 900, 420], sun = null, own = null, heading = '' } = {}) {
  const [bx, by, bw, bh] = frame, st = reading && reading.stations[index];
  if (!st) return { svg: `<text class="sx-title" x="${bx + bw / 2}" y="${by + bh / 2}" text-anchor="middle">No street is in hand.</text>`, scale: 1 };
  const strips = reading.strips || [], half = reading.total / 2;
  const dl = st.faces[0].distance, dr = st.faces[1].distance, hl = st.faces[0].height, hr = st.faces[1].height;
  const leftEdge = -(Math.max(dl ?? 0, half) + 5), rightEdge = Math.max(dr ?? 0, half) + 5, whole = Math.max(hl, hr, 7) + 3.5;
  const pad = { left: 16, right: 16, top: 30, bottom: 92 }, marks = [];
  // The street's width sets the scale; a building too tall for the page at that scale is cut off above, and its height written.
  // Twelve metres of height are always shown.
  const across = (bw - pad.left - pad.right) / (rightEdge - leftEdge), up = bh - pad.top - pad.bottom;
  const k = Math.min(across, up / Math.min(whole, 12)), tall = Math.min(whole, up / k);
  const ground = by + bh - pad.bottom, mid = bx + pad.left + (bw - pad.left - pad.right) / 2 - ((leftEdge + rightEdge) / 2) * k;
  const sx = (x) => mid + x * k, sy = (z) => ground - z * k, out = [];
  const x0 = sx(leftEdge), x1 = sx(rightEdge), clip = `sx-clip-${Math.round(bx)}-${Math.round(by)}`;
  out.push(`<clipPath id="${clip}"><rect x="${f1(bx)}" y="${f1(sy(tall))}" width="${f1(bw)}" height="${f1(tall * k + 4)}"/></clipPath><g clip-path="url(#${clip})">`);

  // The Sun's light on the ground and on the faces, where it falls at this hour.
  const light = profileSun(st, sun);
  if (light && light.side !== 0) {
    // Mirrored so that the Sun stands at the low side: X = side × offset. The near face casts a shadow of its height over tan p.
    const side = light.side, tan = Math.tan(light.angle), near = side > 0 ? [dl, hl] : [dr, hr], away = side > 0 ? [dr, hr] : [dl, hl];
    const low = side > 0 ? leftEdge : -rightEdge, nearX = near[0] === null ? null : -near[0], farX = away[0] === null ? (side > 0 ? rightEdge : -leftEdge) : away[0];
    const edge = nearX === null ? low : nearX + near[1] / tan, litFrom = Math.max(edge, nearX ?? low), back = (X) => sx(side * X);
    if (litFrom < farX) out.push(`<rect class="sx-lit" x="${f1(Math.min(back(litFrom), back(farX)))}" y="${f1(ground - 3)}" width="${f1(Math.abs(back(farX) - back(litFrom)))}" height="6"/>`);
    // On the far face: lit above where the shadow climbs it.
    if (away[0] !== null) {
      const climbs = nearX === null ? 0 : Math.max(0, near[1] - (farX - nearX) * tan);
      if (climbs < away[1]) out.push(`<line class="sx-litface" x1="${f1(back(farX))}" y1="${f1(sy(climbs))}" x2="${f1(back(farX))}" y2="${f1(sy(away[1]))}"/>`);
    }
    // The ray that grazes the near roof's edge, to where it lands.
    if (nearX !== null) {
      const landX = Math.min(edge, farX), landZ = near[1] - (landX - nearX) * tan;
      out.push(`<line class="sx-ray" x1="${f1(back(nearX) - side * 70 * Math.cos(light.angle))}" y1="${f1(sy(near[1]) - 70 * Math.sin(light.angle))}" x2="${f1(back(landX))}" y2="${f1(sy(landZ))}"/>`);
    }
  }
  out.push('</g>');
  // The buildings on either side.
  const building = (side, d, h, own_) => {
    if (d === null) return;
    const x = side < 0 ? x0 : sx(d), w = side < 0 ? sx(-d) - x0 : x1 - sx(d);
    const shown = Math.min(h, tall), over = h > tall + 1e-9;
    out.push(`<rect class="sx-building${own_ ? ' own' : ''}${over ? ' cut' : ''}" x="${f1(x)}" y="${f1(sy(shown))}" width="${f1(Math.max(0, w))}" height="${f1(shown * k)}"/>`);
    if (over) out.push(`<line class="sx-cut" x1="${f1(x)}" y1="${f1(sy(shown))}" x2="${f1(x + Math.max(0, w))}" y2="${f1(sy(shown))}"/>`);
    for (let z = 3; z < shown - 0.5; z += 3) out.push(`<line class="sx-floor" x1="${f1(x)}" y1="${f1(sy(z))}" x2="${f1(x + Math.max(0, w))}" y2="${f1(sy(z))}"/>`);
    marks.push(`<text class="sx-mark over" x="${f1(side < 0 ? sx(-d) - 6 : sx(d) + 6)}" y="${f1(sy(shown) + 14)}" text-anchor="${side < 0 ? 'end' : 'start'}">${f1(h)} m${over ? ', cut off here' : ''}</text>`);
  };
  building(-1, dl, hl, own !== null && st.faces[0].building === own);
  building(1, dr, hr, own !== null && st.faces[1].building === own);
  // The sky the centreline sees.
  if (dl !== null || dr !== null) {
    const c = [sx(0), ground], spoke = (side, d, h) => (d === null ? '' : `<line class="sx-sky" x1="${f1(c[0])}" y1="${f1(c[1])}" x2="${f1(sx(side * d))}" y2="${f1(sy(h))}"/>`);
    out.push(`<g clip-path="url(#${clip})">` + spoke(-1, dl, hl) + spoke(1, dr, hr) + '</g>' + `<text class="sx-note" x="${f1(c[0])}" y="${f1(sy(Math.min(tall - 1.2, Math.max(hl, hr, 6) + 1.6)))}" text-anchor="middle">sees ${(st.sky * 100).toFixed(0)}% of the sky</text>`);
  }
  // The strips: each a slab at its level, with what uses it standing on it.
  out.push(`<line class="sx-ground" x1="${f1(x0)}" y1="${f1(ground)}" x2="${f1(x1)}" y2="${f1(ground)}"/>`);
  strips.forEach((s, i) => {
    const def = STRIPS[s.kind] || STRIPS.lane, a = sx(s.from), b = sx(s.to), top = sy(def.rise), m = (a + b) / 2, w = s.to - s.from;
    out.push(`<rect class="sx-strip" data-strip="${i}" x="${f1(a)}" y="${f1(top)}" width="${f1(b - a)}" height="${f1(ground - top + 0.32 * k)}" style="fill:${def.tone}"/>`);
    if (s.kind === 'footpath' || s.kind === 'frontage' || s.kind === 'shared') out.push(figure(m - (w > 2.4 ? 0.45 * k : 0), top, k) + (w > 2.4 ? figure(m + 0.5 * k, top, k, 1.62) : ''));
    else if (s.kind === 'verge' || s.kind === 'median') out.push(tree(m, top, k, 6.5, Math.min(2.4, Math.max(1.3, w * 1.1))));
    else if (s.kind === 'cycle') out.push(figure(m, top - 0.25 * k, k, 1.6) + `<circle class="sx-wheel" cx="${f1(m)}" cy="${f1(top - 0.33 * k)}" r="${f1(0.33 * k)}"/>`);
    else if (s.kind === 'bus') out.push(box(m, top, k, Math.min(2.55, w - 0.3), 2.9, 'sx-vehicle'));
    else if (s.kind === 'tram') out.push(box(m, top, k, Math.min(2.65, w - 0.2), 3.2, 'sx-vehicle', 0.2));
    else out.push(box(m, top, k, Math.min(1.8, w - 0.3), 1.2, 'sx-vehicle'));
    out.push(`<line class="sx-tick" x1="${f1(a)}" y1="${f1(ground + 0.32 * k)}" x2="${f1(a)}" y2="${f1(ground + 0.32 * k + 24)}"/>`);
    if (b - a > 22) out.push(`<text class="sx-mark" x="${f1(m)}" y="${f1(ground + 0.32 * k + 15)}" text-anchor="middle">${f1(w)}</text>`);
    if (b - a > 46) out.push(`<text class="sx-note" x="${f1(m)}" y="${f1(ground + 0.32 * k + 28)}" text-anchor="middle">${esc((def.title || s.kind).toLowerCase().split(' ')[0])}</text>`);
  });
  if (strips.length) out.push(`<line class="sx-tick" x1="${f1(sx(half))}" y1="${f1(ground + 0.32 * k)}" x2="${f1(sx(half))}" y2="${f1(ground + 0.32 * k + 24)}"/>`);
  // The width between the faces, and the centreline.
  out.push(`<line class="sx-centre" x1="${f1(sx(0))}" y1="${f1(ground + 0.32 * k + 30)}" x2="${f1(sx(0))}" y2="${f1(ground - 0.8 * k)}"/>`);
  if (dl !== null && dr !== null) {
    const y = ground + 0.32 * k + 46;
    out.push(`<path class="sx-dimension" d="M ${f1(sx(-dl))} ${f1(y - 4)} v 8 M ${f1(sx(-dl))} ${f1(y)} H ${f1(sx(dr))} M ${f1(sx(dr))} ${f1(y - 4)} v 8"/><text class="sx-mark" x="${f1((sx(-dl) + sx(dr)) / 2)}" y="${f1(y + 15)}" text-anchor="middle">${f1(dl + dr)} m between the faces · height over width ${st.enclosure.toFixed(2)}</text>`);
  } else out.push(`<text class="sx-note" x="${f1(sx(0))}" y="${f1(ground + 0.32 * k + 61)}" text-anchor="middle">${dl === null && dr === null ? 'open on both sides here' : `open to the ${dl === null ? 'left' : 'right'} here`}</text>`);
  out.push(marks.join('') + `<text class="sx-title" x="${f1(bx + pad.left)}" y="${f1(by + 18)}">${esc(heading)}</text>`);
  return { svg: out.join(''), scale: k };
}

// ---- the street unrolled along its length ----------------------------------------------------------------------------------------

// The whole street on one strip of the page: its length across, the offset from its centreline up and down (not to one scale);
// the faces on either side by their distance, darker the higher; the strips' edges; each footpath's hours of sun; the station in
// hand; the faces of the buildings in `own` (a set of indices: the building drawn) in the accent. Answers {svg, at: (x) => metres
// along}.
export function unrolledSvg(reading, index, { box: frame = [0, 0, 900, 150], need = 2, own = null } = {}) {
  const [bx, by, bw, bh] = frame;
  if (!reading || !reading.stations.length) return { svg: '', at: () => 0 };
  const pad = { left: 46, right: 16, top: 12, bottom: 22 }, L = reading.length, far = Math.max(8, ...reading.stations.flatMap((s) => s.faces.map((f) => f.distance ?? 0))) + 7;
  const kx = (bw - pad.left - pad.right) / L, ky = (bh - pad.top - pad.bottom) / (2 * far), cy = by + pad.top + (bh - pad.top - pad.bottom) / 2;
  const sx = (s) => bx + pad.left + s * kx, sy = (off) => cy - off * ky, out = [], step = L / reading.stations.length, top = Math.max(1, ...reading.stations.flatMap((s) => s.faces.map((f) => f.height)));
  out.push(`<rect class="ux-ground" x="${f1(sx(0))}" y="${f1(sy(far))}" width="${f1(L * kx)}" height="${f1(2 * far * ky)}"/>`);
  for (const s of reading.strips || []) {
    const def = STRIPS[s.kind] || STRIPS.lane;
    out.push(`<rect x="${f1(sx(0))}" y="${f1(sy(-s.from))}" width="${f1(L * kx)}" height="${f1((s.to - s.from) * ky)}" style="fill:${def.tone}" class="ux-strip"/>`);
  }
  reading.stations.forEach((st, i) => {
    const a = sx(st.at - step / 2), w = step * kx + 0.5;
    st.faces.forEach((f, side) => {
      if (f.distance === null) return;
      const y = side === 0 ? sy(far) : sy(-f.distance), h = (far - f.distance) * ky;
      out.push(`<rect class="ux-face${own && own.has(f.building) ? ' own' : ''}" x="${f1(a)}" y="${f1(y)}" width="${f1(w)}" height="${f1(h)}" style="fill-opacity:${(0.1 + 0.34 * f.height / top).toFixed(2)}"/>`);
    });
    if (reading.lit) st.sun.forEach((hours, side) => {
      const off = (st.foot[side][0] - st.point[0]) * st.left[0] + (st.foot[side][1] - st.point[1]) * st.left[1];
      out.push(`<rect class="ux-sun${hours >= need - 1e-9 ? ' has' : ''}" x="${f1(a + 0.5)}" y="${f1(sy(off) - 2.5)}" width="${f1(Math.max(1, w - 1))}" height="5" style="fill-opacity:${Math.min(1, 0.12 + hours / 8).toFixed(2)}"/>`);
    });
    if (i === index) out.push(`<rect class="ux-here" x="${f1(a)}" y="${f1(sy(far) - 4)}" width="${f1(w)}" height="${f1(2 * far * ky + 8)}"/>`);
  });
  out.push(`<line class="ux-centre" x1="${f1(sx(0))}" y1="${f1(cy)}" x2="${f1(sx(L))}" y2="${f1(cy)}"/>`);
  const every = [10, 20, 50, 100, 200].find((m) => m * kx >= 46) || 500;
  for (let s = 0; s <= L + 1e-6; s += every) out.push(`<line class="ux-tick" x1="${f1(sx(s))}" y1="${f1(sy(-far))}" x2="${f1(sx(s))}" y2="${f1(sy(-far) + 4)}"/><text class="sx-note" x="${f1(sx(s))}" y="${f1(sy(-far) + 15)}" text-anchor="middle">${s} m</text>`);
  out.push(`<text class="sx-note" x="${f1(bx + pad.left - 6)}" y="${f1(sy(far * 0.62))}" text-anchor="end">left</text><text class="sx-note" x="${f1(bx + pad.left - 6)}" y="${f1(sy(-far * 0.62) + 4)}" text-anchor="end">right</text>`);
  return { svg: out.join(''), at: (x) => Math.max(0, Math.min(L, (x - bx - pad.left) / kx)), box: [sx(0), sy(far), L * kx, 2 * far * ky] };
}
