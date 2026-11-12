// Walking through a drawn building (ADR 0096): what stops a person on a storey, whether a place is clear, the step from one place
// toward another, and where one comes in. Pure: the view that shows the walk asks these and moves its eye.
//
// A person is a disc of a radius on the floor. What stops it: the solid walls, each to half its thickness, less their doors (a
// door is walked through; a window is not); and the pieces that obstruct. On an upper storey the floor ends at the rooms, so the
// disc's middle must stay in one.

import { dist, inRing, project, sub } from './geometry.js';
import { inPiece } from './catalogue.js';
import { openingSpan, wallEnds } from './model.js';

// What stops a person on storey k: the walls' solid stretches as {a, b, half}, the obstructing pieces, and, above the ground, the
// rooms' rings one must stay within (from the reading, when there is one).
export function obstacles(doc, k, reading = null) {
  const s = doc.storeys[k], walls = [];
  s.walls.forEach((w, i) => {
    if (w.kind === 'open') return;
    const [a, b] = wallEnds(s, i), L = dist(a, b);
    if (!(L > 0)) return;
    const doors = s.openings.filter((o) => o.wall === i && o.kind === 'door').map((o) => [Math.max(0, o.at - o.width / 2), Math.min(L, o.at + o.width / 2)]).sort((p, q) => p[0] - q[0]);
    let from = 0;
    const at = (x) => [a[0] + (b[0] - a[0]) * x / L, a[1] + (b[1] - a[1]) * x / L];
    for (const [lo, hi] of doors) {
      if (lo > from) walls.push({ a: at(from), b: at(lo), half: w.t / 2 });
      from = Math.max(from, hi);
    }
    if (from < L) walls.push({ a: at(from), b: at(L), half: w.t / 2 });
  });
  const pieces = (doc.furniture || []).filter((p) => p.storey === k && p.obstructs !== false);
  const R = reading && reading.storeys[k];
  const rooms = k > 0 && R && R.sound ? R.rooms.filter((r) => r.indoor).map((r) => r.ring) : null;
  return { walls, pieces, rooms };
}

// Whether a person of a radius can stand at p.
export function clear(obs, p, radius = 0.2) {
  for (const w of obs.walls) if (project(p, w.a, w.b).distance < w.half + radius) return false;
  for (const piece of obs.pieces) if (inPiece(piece, p, radius * 0.6)) return false;
  if (obs.rooms && !obs.rooms.some((ring) => inRing(p, ring))) return false;
  return true;
}

// Where a person at `from` gets to on the way to `to`: straight on while the way is clear, and along what stops them where a turn
// of up to 75° still brings them nearer (by a fifth of a step at least), so that one who walks slantwise into a wall goes along it
// and one who walks straight into it stops. No way round is sought: a piece in the way is where the walk ends. Steps of `stride`
// metres.
export function walk(obs, from, to, radius = 0.2, stride = 0.04) {
  let p = [from[0], from[1]];
  const total = dist(from, to);
  if (!(total > 0)) return p;
  const n = Math.ceil(total / stride), d = sub(to, from), step = total / n, u = [d[0] / total, d[1] / total];
  for (let i = 0; i < n; i++) {
    let moved = false;
    for (const turn of [0, 25, -25, 50, -50, 75, -75]) {
      const a = turn * Math.PI / 180, c = Math.cos(a), s = Math.sin(a), len = step * c;
      const q = [p[0] + (u[0] * c - u[1] * s) * len, p[1] + (u[0] * s + u[1] * c) * len];
      if ((turn === 0 || dist(q, to) <= dist(p, to) - 0.2 * step) && clear(obs, q, radius)) { p = q; moved = true; break; }
    }
    if (!moved) break;
  }
  return p;
}

// The nearest clear place to p within `reach` metres, on rings about it; p itself when it is clear; null when none is.
export function nearestClear(obs, p, radius = 0.2, reach = 1.5) {
  if (clear(obs, p, radius)) return [p[0], p[1]];
  for (let r = 0.1; r <= reach; r += 0.1) {
    for (let i = 0; i < 16; i++) {
      const a = i * Math.PI / 8, q = [p[0] + r * Math.cos(a), p[1] + r * Math.sin(a)];
      if (clear(obs, q, radius)) return q;
    }
  }
  return null;
}

// Where one comes into storey k, and which way one then looks (degrees counter-clockwise from east): just inside its first door
// from the outside, looking in; without one, the middle of its largest room, looking north.
export function entrance(doc, k, reading) {
  const s = doc.storeys[k], R = reading && reading.storeys[k];
  if (R && R.sound) {
    for (const o of s.openings) {
      const side = R.walls[o.wall];
      if (o.kind !== 'door' || !side || !side.outer || (side.left !== null && side.right !== null)) continue;
      const [a, b] = wallEnds(s, o.wall), L = dist(a, b) || 1, into = side.left !== null ? 1 : -1;
      const n = [-(b[1] - a[1]) / L * into, (b[0] - a[0]) / L * into], m = openingSpan(s, o).middle, off = s.walls[o.wall].t / 2 + 0.7;
      return { at: [m[0] + n[0] * off, m[1] + n[1] * off], heading: Math.atan2(n[1], n[0]) * 180 / Math.PI };
    }
    let best = null;
    for (const room of R.rooms) if (room.indoor && (!best || room.area > best.area)) best = room;
    if (best) {
      let x = 0, y = 0;
      for (const p of best.ring) x += p[0] / best.ring.length, y += p[1] / best.ring.length;
      return { at: [x, y], heading: 90 };
    }
  }
  return { at: s.vertices.length ? s.vertices[0].slice() : [0, 0], heading: 90 };
}
