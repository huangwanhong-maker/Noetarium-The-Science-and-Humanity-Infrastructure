// Plane geometry for drawing a building's plan: points, segments, what a pointer snaps to. Metres; x east, y north.

export const TOL = 1e-4;   // points nearer than a tenth of a millimetre are one point

export const dist = (a, b) => Math.hypot(b[0] - a[0], b[1] - a[1]);
export const add = (a, b) => [a[0] + b[0], a[1] + b[1]];
export const sub = (a, b) => [a[0] - b[0], a[1] - b[1]];
export const scale = (a, k) => [a[0] * k, a[1] * k];
export const lerp = (a, b, t) => [a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t];
export const cross = (a, b) => a[0] * b[1] - a[1] * b[0];
export const dot = (a, b) => a[0] * b[0] + a[1] * b[1];

// Where p falls on the segment a–b: t in [0, 1] of the nearest point, that point, and the distance to it.
export function project(p, a, b) {
  const e = sub(b, a), L2 = dot(e, e);
  const t = L2 > 0 ? Math.max(0, Math.min(1, dot(sub(p, a), e) / L2)) : 0;
  const q = lerp(a, b, t);
  return { t, point: q, distance: dist(p, q) };
}

// The point at which the open segments a–b and c–d cross, interior to both; null when they do not.
export function crossing(a, b, c, d) {
  const r = sub(b, a), s = sub(d, c), den = cross(r, s);
  const Lr = Math.hypot(r[0], r[1]), Ls = Math.hypot(s[0], s[1]);
  if (Math.abs(den) <= 1e-12 * Lr * Ls) return null;
  const w = sub(c, a), t = cross(w, s) / den, u = cross(w, r) / den;
  if (t * Lr <= TOL || (1 - t) * Lr <= TOL || u * Ls <= TOL || (1 - u) * Ls <= TOL) return null;
  return lerp(a, b, t);
}

export function ringArea(ring) {
  let twice = 0;
  for (let k = 0; k < ring.length; k++) { const p = ring[k], q = ring[(k + 1) % ring.length]; twice += p[0] * q[1] - q[0] * p[1]; }
  return twice / 2;
}

export function inRing(p, ring) {
  let inside = false;
  for (let i = 0, j = ring.length - 1; i < ring.length; j = i++) {
    const a = ring[i], b = ring[j];
    if ((a[1] > p[1]) !== (b[1] > p[1]) && p[0] < a[0] + (b[0] - a[0]) * (p[1] - a[1]) / (b[1] - a[1])) inside = !inside;
  }
  return inside;
}

export function centroid(ring) {
  let a = 0, x = 0, y = 0;
  for (let k = 0; k < ring.length; k++) {
    const p = ring[k], q = ring[(k + 1) % ring.length], c = p[0] * q[1] - q[0] * p[1];
    a += c, x += (p[0] + q[0]) * c, y += (p[1] + q[1]) * c;
  }
  if (Math.abs(a) < 1e-12) return ring.length ? ring[0] : [0, 0];
  return [x / (3 * a), y / (3 * a)];
}

// A point well inside a ring for a label: its centroid when that lies inside, else the middle of the widest horizontal chord through
// the centroid's height.
export function interiorPoint(ring) {
  const c = centroid(ring);
  if (inRing(c, ring)) return c;
  const xs = [];
  for (let k = 0; k < ring.length; k++) {
    const p = ring[k], q = ring[(k + 1) % ring.length];
    if ((p[1] > c[1]) !== (q[1] > c[1])) xs.push(p[0] + (q[0] - p[0]) * (c[1] - p[1]) / (q[1] - p[1]));
  }
  xs.sort((u, v) => u - v);
  let best = c, width = -1;
  for (let k = 0; k + 1 < xs.length; k += 2) if (xs[k + 1] - xs[k] > width) width = xs[k + 1] - xs[k], best = [(xs[k] + xs[k + 1]) / 2, c[1]];
  return best;
}

// A length rounded to a grid step.
export const toGrid = (x, step) => Math.round(x / step) * step;

// What a pointer at p snaps to among a storey's vertices and walls, a lot's corners and a grid turned by `angle` (radians) about
// `origin`: {point, kind: 'vertex' | 'wall' | 'grid' | 'free', vertex?, wall?}. `reach` is the snapping distance in metres.
export function snap(p, storey, { reach = 0.2, step = 0.05, angle = 0, origin = [0, 0], extra = [] } = {}) {
  let best = null;
  const V = storey ? storey.vertices : [];
  for (let i = 0; i < V.length; i++) { const d = dist(p, V[i]); if (d <= reach && (!best || d < best.d)) best = { d, point: V[i].slice(), kind: 'vertex', vertex: i }; }
  for (const q of extra) { const d = dist(p, q); if (d <= reach && (!best || d < best.d)) best = { d, point: q.slice(), kind: 'vertex' }; }
  if (best) return best;
  const c = Math.cos(angle), s = Math.sin(angle);
  const local = (q) => { const v = sub(q, origin); return [v[0] * c + v[1] * s, -v[0] * s + v[1] * c]; };
  const world = (q) => add(origin, [q[0] * c - q[1] * s, q[0] * s + q[1] * c]);
  if (storey) {
    for (let w = 0; w < storey.walls.length; w++) {
      const a = V[storey.walls[w].a], b = V[storey.walls[w].b], pr = project(p, a, b);
      if (pr.distance <= reach && (!best || pr.distance < best.d)) {
        // Along the wall, to the grid's step from its first end.
        const L = dist(a, b), along = Math.max(0, Math.min(L, toGrid(pr.t * L, step)));
        best = { d: pr.distance, point: lerp(a, b, L > 0 ? along / L : 0), kind: 'wall', wall: w };
      }
    }
    if (best) return best;
  }
  const g = local(p);
  return { d: 0, point: world([toGrid(g[0], step), toGrid(g[1], step)]), kind: step > 0 ? 'grid' : 'free' };
}

// The point q moved so that a → q runs along the grid's axes (within `degrees` of one), keeping its length along that axis.
export function squared(a, q, angle = 0, degrees = 8) {
  const v = sub(q, a), L = Math.hypot(v[0], v[1]);
  if (L === 0) return q;
  const bearing = Math.atan2(v[1], v[0]) - angle, quarter = Math.PI / 2, k = Math.round(bearing / quarter);
  if (Math.abs(bearing - k * quarter) > degrees * Math.PI / 180) return q;
  const dir = [Math.cos(angle + k * quarter), Math.sin(angle + k * quarter)];
  return add(a, scale(dir, dot(v, dir)));
}
