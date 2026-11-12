// A building as a person draws it: the document `noetarium.building/1` and the operations that change it (ADR 0095).
//
// THE DOCUMENT holds only what is drawn: a site (a place, a lot and which of its edges face a street), storeys (vertices, walls
// between them, openings on the walls, labels as points), stairs, what stands in the rooms (pieces of furniture and fixtures, each a
// rectangle on the floor with the space it needs: catalogue.js; ADR 0096), and what is declared (a brief, a scheme of siting rules,
// a code of habitation, rates, a fabric). Rooms are not in it: they follow from the walls, and the architecture domain reads them.
//
// THE OPERATIONS. Each is a pure function of a document and its arguments, answering a new document or throwing with the reason; none
// touches the document it was given, so the document before is the operation's inverse and a history is a list of documents. A
// hand, a generator and an agent all change a building through these and nothing else (OPERATIONS).
//
// A STOREY STAYS A PLANE GRAPH. A wall drawn across others is cut where it meets them and they where it meets them; a wall ending on
// another splits it; a vertex moved onto another is merged with it. After a move the storey is drawn again from its walls as
// segments (replan), so no operation can leave walls crossing; walls that run straight on through a vertex nothing else uses are
// one wall again. An opening keeps its place on the ground where its wall is stretched or cut, and goes with its wall where the
// wall is moved; labels are points, so they keep their rooms.
//
// Lengths in metres; x east, y north; angles in degrees, counter-clockwise from east.

import { entry, newPiece } from './catalogue.js';
import { TOL, add, crossing, dist, lerp, project, sub } from './geometry.js';
import { DEFAULT_ROOF, DEFAULT_SHELL, roofOf, shellOf } from './exterior.js';

export const SCHEMA = 'noetarium.building/1';

// Rates no source stands behind: a person's own are to replace them, and the page says so.
export const PRESUMED_RATES = {
  currency: '',
  presumed: true,
  items: {
    outer_wall: [250, 600], inner_wall: [90, 220], window: [500, 1500], outer_door: [800, 2500], inner_door: [300, 900],
    ground_slab: [150, 400], upper_floor: [180, 450], roof: [200, 500], stair: [5000, 20000], fit_out: [600, 1800], wet_room: [800, 2500],
  },
};

export const DEFAULT_FABRIC = { wall: 0.5, window: 2.8, door: 2.5, roof: 0.25, ground: 0.6 };

export const DEFAULT_BRIEF = {
  name: 'A household of four',
  needs: [
    { kind: 'living', count: 1, area: 16 }, { kind: 'kitchen', count: 1, area: 8 },
    { kind: 'bedroom', count: 3, area: 9 }, { kind: 'bathroom', count: 1, area: 3.5 },
  ],
  relations: [
    { a: 'bedroom', b: 'bedroom', how: 'not through' }, { a: 'bedroom', b: 'bathroom', how: 'not through' },
    { a: 'bathroom', b: 'kitchen', how: 'apart' },
  ],
};

export const DEFAULTS = {
  outer: 0.25, inner: 0.11,                                   // wall thicknesses
  door: { width: 0.82, height: 2.04, sill: 0, glazed: 0, openable: 1 },
  entrance: { width: 0.92, height: 2.04, sill: 0, glazed: 0, openable: 1 },
  window: { width: 1.5, height: 1.2, sill: 0.9, glazed: 1, openable: 0.5 },
  stair: { width: 1.0, going: 0.25, risers: 17, bearing: 90 },
};

export function newStorey(name = 'Ground') {
  return { name, height: 3.0, ceiling: 2.7, vertices: [], walls: [], openings: [], labels: [] };
}

export function newBuilding(name = 'A house') {
  return {
    schema: SCHEMA, name,
    site: { place: null, lot: null, scheme: 'unstated' },
    storeys: [newStorey()], stairs: [], furniture: [],
    // The width a person needs to pass between what stands in a room: presumed, and a person's to state.
    interior: { passage: 0.6 },
    brief: structuredClone(DEFAULT_BRIEF), siting: {}, code: {},
    rates: structuredClone(PRESUMED_RATES), fabric: { ...DEFAULT_FABRIC },
    // The outside (exterior.js): a roof of a form, what the walls are clad in. A new building's roof is flat until a person says.
    roof: structuredClone(DEFAULT_ROOF), shell: { ...DEFAULT_SHELL },
  };
}

// ---- reading a storey ----------------------------------------------------------------------------------------------------------

export const wallEnds = (s, w) => [s.vertices[s.walls[w].a], s.vertices[s.walls[w].b]];
export const wallLength = (s, w) => dist(...wallEnds(s, w));

// The ends of an opening on the ground, and its middle.
export function openingSpan(s, o) {
  const [a, b] = wallEnds(s, o.wall), L = dist(a, b) || 1;
  return { from: lerp(a, b, (o.at - o.width / 2) / L), to: lerp(a, b, (o.at + o.width / 2) / L), middle: lerp(a, b, o.at / L) };
}

// The height of a storey's floor above the ground.
export function elevation(doc, k) {
  let z = 0;
  for (let i = 0; i < k; i++) z += doc.storeys[i].height;
  return z;
}

// ---- keeping a storey a plane graph --------------------------------------------------------------------------------------------

function splitWall(s, w, p) {
  const wall = s.walls[w], cut = dist(s.vertices[wall.a], p);
  s.vertices.push([p[0], p[1]]);
  const m = s.vertices.length - 1;
  s.walls.push({ ...wall, a: m, b: wall.b });
  wall.b = m;
  const second = s.walls.length - 1;
  for (const o of s.openings) if (o.wall === w && o.at > cut) o.wall = second, o.at -= cut;
  return m;
}

// The vertex at p: one already there, or a new one, splitting the wall it falls on.
function vertexAt(s, p) {
  for (let i = 0; i < s.vertices.length; i++) if (dist(s.vertices[i], p) <= TOL) return i;
  for (let w = 0; w < s.walls.length; w++) {
    const [a, b] = wallEnds(s, w), pr = project(p, a, b), L = dist(a, b);
    if (pr.distance <= TOL && pr.t * L > TOL && (1 - pr.t) * L > TOL) return splitWall(s, w, pr.point);
  }
  s.vertices.push([p[0], p[1]]);
  return s.vertices.length - 1;
}

const joined = (s, u, v) => s.walls.some((w) => (w.a === u && w.b === v) || (w.a === v && w.b === u));

// A segment drawn into a storey: cut where it meets walls and vertices, they where it meets them. Answers the walls it made.
function insertSegment(s, pa, pb, props) {
  if (dist(pa, pb) <= TOL) return [];
  const a = vertexAt(s, pa), b = vertexAt(s, pb);
  if (a === b) return [];
  const A = s.vertices[a], B = s.vertices[b];
  const before = s.walls.length;
  for (let w = 0; w < before; w++) {
    const wall = s.walls[w];
    if (wall.a === a || wall.b === a || wall.a === b || wall.b === b) continue;
    const x = crossing(A, B, s.vertices[wall.a], s.vertices[wall.b]);
    if (x) splitWall(s, w, x);
  }
  const L = dist(A, B), on = [];
  for (let v = 0; v < s.vertices.length; v++) {
    if (v === a || v === b) continue;
    const pr = project(s.vertices[v], A, B);
    if (pr.distance <= TOL && pr.t * L > TOL && (1 - pr.t) * L > TOL) on.push([pr.t, v]);
  }
  on.sort((p, q) => p[0] - q[0]);
  const chain = [a, ...on.map((x) => x[1]), b], made = [];
  for (let k = 0; k + 1 < chain.length; k++) {
    if (joined(s, chain[k], chain[k + 1])) continue;
    s.walls.push({ a: chain[k], b: chain[k + 1], t: props.t, kind: props.kind || 'solid' });
    made.push(s.walls.length - 1);
  }
  return made;
}

// Walls that run straight on through a vertex nothing else uses become one wall.
function straighten(s) {
  for (let again = true; again;) {
    again = false;
    const degree = new Array(s.vertices.length).fill(0);
    for (const w of s.walls) degree[w.a]++, degree[w.b]++;
    for (let v = 0; v < s.vertices.length && !again; v++) {
      if (degree[v] !== 2) continue;
      const at = s.walls.map((w, i) => i).filter((i) => s.walls[i].a === v || s.walls[i].b === v);
      const [i, j] = at, wi = s.walls[i], wj = s.walls[j];
      if (wi.t !== wj.t || (wi.kind || 'solid') !== (wj.kind || 'solid')) continue;
      const x = wi.a === v ? wi.b : wi.a, y = wj.a === v ? wj.b : wj.a;
      if (x === y) continue;
      const P = s.vertices[v], u = sub(s.vertices[x], P), q = sub(s.vertices[y], P);
      if (Math.abs(u[0] * q[1] - u[1] * q[0]) > 1e-9 * Math.hypot(...u) * Math.hypot(...q) || u[0] * q[0] + u[1] * q[1] >= 0) continue;
      s.walls[i] = { ...wi, a: x, b: y };
      s.walls.splice(j, 1);
      again = true;
    }
  }
  // Vertices no wall uses are dropped.
  const used = new Array(s.vertices.length).fill(false);
  for (const w of s.walls) used[w.a] = used[w.b] = true;
  const index = [];
  let n = 0;
  for (let v = 0; v < s.vertices.length; v++) index.push(used[v] ? n++ : -1);
  s.vertices = s.vertices.filter((_, v) => used[v]);
  for (const w of s.walls) w.a = index[w.a], w.b = index[w.b];
}

// A storey's walls as segments on the ground, each with its openings. `was` is the storey before a move: an opening then carries
// the place on the ground where it stood, which it keeps if its wall, stretched, still passes there.
function segmentsOf(s, was = null) {
  return s.walls.map((w, i) => ({
    a: s.vertices[w.a].slice(), b: s.vertices[w.b].slice(), t: w.t, kind: w.kind || 'solid',
    openings: s.openings.filter((o) => o.wall === i).map((o) => (was ? { ...o, ground: openingSpan(was, o).middle } : { ...o })),
  }));
}

// A storey drawn again from segments: a plane graph whatever they are. An opening goes to the wall its middle now lies on, and is
// dropped where no wall is left under it or it no longer fits.
function replan(s, segments) {
  const out = { ...s, vertices: [], walls: [], openings: [] };
  const places = [];
  for (const g of segments) {
    const L = dist(g.a, g.b);
    if (L <= TOL) continue;
    insertSegment(out, g.a, g.b, g);
    for (const { ground, ...o } of g.openings || []) {
      if (L < o.width) continue;
      // Where it stood, if the wall still passes there; else as far along the wall as it was.
      const stays = ground ? project(ground, g.a, g.b) : null;
      const at = Math.max(o.width / 2, Math.min(L - o.width / 2, stays && stays.distance <= TOL * 10 ? stays.t * L : o.at));
      places.push({ o, point: lerp(g.a, g.b, at / L) });
    }
  }
  straighten(out);
  for (const { o, point } of places) {
    for (let w = 0; w < out.walls.length; w++) {
      const [a, b] = wallEnds(out, w), pr = project(point, a, b), L = dist(a, b);
      if (pr.distance > TOL * 10 || L < o.width) continue;
      const { wall: _old, ...rest } = o;
      out.openings.push({ ...rest, wall: w, at: Math.max(o.width / 2, Math.min(L - o.width / 2, pr.t * L)) });
      break;
    }
  }
  return out;
}

// ---- the operations ------------------------------------------------------------------------------------------------------------

function storeyOf(doc, k) {
  if (!Number.isInteger(k) || k < 0 || k >= doc.storeys.length) throw new Error('no such storey');
  return doc.storeys[k];
}
function wallOf(s, w) {
  if (!Number.isInteger(w) || w < 0 || w >= s.walls.length) throw new Error('no such wall');
  return s.walls[w];
}
function checkPiece(p) {
  for (const key of ['width', 'depth', 'height']) positive(p[key], `a piece's ${key}`);
  for (const key of ['front', 'side', 'around']) if (!(Number.isFinite(p[key]) && p[key] >= 0)) throw new Error(`the clear floor a piece needs is not negative: ${key}`);
  if (!Number.isFinite(p.bearing)) throw new Error('a piece\'s bearing is in degrees');
}
const isPoint = (p) => Array.isArray(p) && p.length >= 2 && Number.isFinite(p[0]) && Number.isFinite(p[1]);
function point(p, what) {
  if (!isPoint(p)) throw new Error(`${what} is a point [x, y]`);
  return [p[0], p[1]];
}
function positive(x, what) {
  if (!(Number.isFinite(x) && x > 0)) throw new Error(`${what} is a positive number`);
  return x;
}

export const OPERATIONS = {
  // ---- walls ----
  addWall: {
    title: 'Draw a wall',
    params: { storey: 'index', from: 'point', to: 'point', t: 'thickness (m)', kind: 'solid | open' },
    apply(doc, { storey, from, to, t = DEFAULTS.inner, kind = 'solid' }) {
      const s = storeyOf(doc, storey);
      const a = point(from, 'a wall\'s start'), b = point(to, 'a wall\'s end');
      if (dist(a, b) <= TOL) throw new Error('a wall has a length');
      if (!(t >= 0)) throw new Error('a wall\'s thickness is not negative');
      insertSegment(s, a, b, { t, kind });
      return doc;
    },
  },
  addRoom: {
    title: 'Draw a room',
    params: { storey: 'index', ring: 'points', t: 'thickness (m)' },
    apply(doc, { storey, ring, t = DEFAULTS.outer }) {
      const s = storeyOf(doc, storey);
      if (!Array.isArray(ring) || ring.length < 3) throw new Error('a room has three corners at least');
      const r = ring.map((p) => point(p, 'a corner'));
      for (let k = 0; k < r.length; k++) insertSegment(s, r[k], r[(k + 1) % r.length], { t, kind: 'solid' });
      return doc;
    },
  },
  removeWall: {
    title: 'Remove a wall',
    params: { storey: 'index', wall: 'index' },
    apply(doc, { storey, wall }) {
      const s = storeyOf(doc, storey);
      wallOf(s, wall);
      doc.storeys[storey] = replan(s, segmentsOf(s).filter((_, i) => i !== wall));
      return doc;
    },
  },
  setWall: {
    title: 'Change a wall',
    params: { storey: 'index', wall: 'index', t: 'thickness (m)', kind: 'solid | open' },
    apply(doc, { storey, wall, t, kind }) {
      const w = wallOf(storeyOf(doc, storey), wall);
      if (t !== undefined) { if (!(t >= 0)) throw new Error('a wall\'s thickness is not negative'); w.t = t; }
      if (kind !== undefined) { if (kind !== 'solid' && kind !== 'open') throw new Error('a wall is solid or open'); w.kind = kind; }
      return doc;
    },
  },
  moveVertex: {
    title: 'Move a corner',
    params: { storey: 'index', vertex: 'index', to: 'point' },
    apply(doc, { storey, vertex, to }) {
      const s = storeyOf(doc, storey), p = point(to, 'where a corner goes');
      if (!Number.isInteger(vertex) || vertex < 0 || vertex >= s.vertices.length) throw new Error('no such corner');
      const moved = structuredClone(s);
      moved.vertices[vertex] = p;
      doc.storeys[storey] = replan(s, segmentsOf(moved, s));
      return doc;
    },
  },
  moveWall: {
    title: 'Move a wall',
    params: { storey: 'index', wall: 'index', by: 'vector' },
    apply(doc, { storey, wall, by }) {
      const s = storeyOf(doc, storey), w = wallOf(s, wall), d = point(by, 'how far a wall moves');
      const moved = structuredClone(s);
      for (const v of new Set([w.a, w.b])) moved.vertices[v] = add(s.vertices[v], d);
      doc.storeys[storey] = replan(s, segmentsOf(moved, s));
      return doc;
    },
  },
  // ---- openings ----
  addOpening: {
    title: 'Open a door or a window',
    params: { storey: 'index', wall: 'index', kind: 'door | window', at: 'metres from the wall\'s first end', width: 'm', height: 'm', sill: 'm', glazed: 'share', openable: 'share' },
    apply(doc, { storey, wall, kind = 'door', at, ...rest }) {
      const s = storeyOf(doc, storey);
      wallOf(s, wall);
      if (kind !== 'door' && kind !== 'window') throw new Error('an opening is a door or a window');
      const o = { wall, kind, ...(kind === 'window' ? DEFAULTS.window : DEFAULTS.door), ...rest };
      const L = wallLength(s, wall);
      positive(o.width, 'an opening\'s width'), positive(o.height, 'an opening\'s height');
      if (o.width > L - 2 * TOL) throw new Error(`the wall is ${L.toFixed(2)} m long: too short for an opening of ${o.width.toFixed(2)} m`);
      o.at = Math.max(o.width / 2, Math.min(L - o.width / 2, Number.isFinite(at) ? at : L / 2));
      if (s.openings.some((q) => q.wall === wall && Math.abs(q.at - o.at) < (q.width + o.width) / 2 - TOL)) throw new Error('another opening stands there');
      s.openings.push(o);
      return doc;
    },
  },
  setOpening: {
    title: 'Change a door or a window',
    params: { storey: 'index', opening: 'index', at: 'm', width: 'm', height: 'm', sill: 'm', glazed: 'share', openable: 'share', swing: '1: to the wall\'s left; −1: to its right', hinge: 'a | b: the end its leaf hangs at' },
    apply(doc, { storey, opening, ...change }) {
      const s = storeyOf(doc, storey), o = s.openings[opening];
      if (!o) throw new Error('no such opening');
      const next = { ...o, ...change };
      const L = wallLength(s, o.wall);
      positive(next.width, 'an opening\'s width'), positive(next.height, 'an opening\'s height');
      if (next.width > L - 2 * TOL) throw new Error(`the wall is ${L.toFixed(2)} m long: too short for an opening of ${next.width.toFixed(2)} m`);
      next.at = Math.max(next.width / 2, Math.min(L - next.width / 2, next.at));
      if (s.openings.some((q, i) => i !== opening && q.wall === o.wall && Math.abs(q.at - next.at) < (q.width + next.width) / 2 - TOL)) throw new Error('another opening stands there');
      s.openings[opening] = next;
      return doc;
    },
  },
  removeOpening: {
    title: 'Close a door or a window',
    params: { storey: 'index', opening: 'index' },
    apply(doc, { storey, opening }) {
      const s = storeyOf(doc, storey);
      if (!s.openings[opening]) throw new Error('no such opening');
      s.openings.splice(opening, 1);
      return doc;
    },
  },
  // ---- rooms' names ----
  setLabel: {
    title: 'Name a room',
    params: { storey: 'index', label: 'index, or none for a new one', at: 'point', kind: 'a use', name: 'text' },
    apply(doc, { storey, label, at, kind, name }) {
      const s = storeyOf(doc, storey);
      if (label === undefined || label === null) {
        s.labels.push({ at: point(at, 'a label\'s place'), kind: kind || '', name: name || '' });
        return doc;
      }
      const l = s.labels[label];
      if (!l) throw new Error('no such label');
      if (at !== undefined) l.at = point(at, 'a label\'s place');
      if (kind !== undefined) l.kind = kind;
      if (name !== undefined) l.name = name;
      return doc;
    },
  },
  removeLabel: {
    title: 'Remove a room\'s name',
    params: { storey: 'index', label: 'index' },
    apply(doc, { storey, label }) {
      const s = storeyOf(doc, storey);
      if (!s.labels[label]) throw new Error('no such label');
      s.labels.splice(label, 1);
      return doc;
    },
  },
  // ---- storeys and stairs ----
  addStorey: {
    title: 'Add a storey above',
    params: { copy: 'outer | all | none: which walls of the storey below to begin from', name: 'text' },
    apply(doc, { copy = 'outer', name, outer = [] } = {}) {
      const below = doc.storeys[doc.storeys.length - 1];
      const s = newStorey(name || `Storey ${doc.storeys.length + 1}`);
      s.height = below.height, s.ceiling = below.ceiling;
      // `outer` lists the walls of the storey below that are outer walls, as the domain read them; without it every wall is copied.
      const keep = copy === 'none' ? [] : copy === 'outer' && outer.length ? outer : below.walls.map((_, i) => i);
      for (const i of keep) if (below.walls[i]) insertSegment(s, below.vertices[below.walls[i].a], below.vertices[below.walls[i].b], below.walls[i]);
      straighten(s);
      doc.storeys.push(s);
      return doc;
    },
  },
  removeStorey: {
    title: 'Remove a storey',
    params: { storey: 'index' },
    apply(doc, { storey }) {
      storeyOf(doc, storey);
      if (doc.storeys.length === 1) throw new Error('a building has one storey at least');
      doc.storeys.splice(storey, 1);
      doc.stairs = doc.stairs.filter((st) => st.from !== storey && st.from + 1 !== storey).map((st) => (st.from > storey ? { ...st, from: st.from - 1 } : st));
      doc.furniture = (doc.furniture || []).filter((f) => f.storey !== storey).map((f) => (f.storey > storey ? { ...f, storey: f.storey - 1 } : f));
      return doc;
    },
  },
  setStorey: {
    title: 'Change a storey',
    params: { storey: 'index', name: 'text', height: 'floor to floor (m)', ceiling: 'floor to ceiling (m)' },
    apply(doc, { storey, name, height, ceiling }) {
      const s = storeyOf(doc, storey);
      if (name !== undefined) s.name = String(name);
      if (height !== undefined) s.height = positive(height, 'a storey\'s height');
      if (ceiling !== undefined) s.ceiling = positive(ceiling, 'a ceiling\'s height');
      if (s.ceiling > s.height) throw new Error('a ceiling is no higher than its storey');
      return doc;
    },
  },
  addStair: {
    title: 'Place a stair',
    params: { from: 'the storey it rises from', at: 'point: the middle of its first riser', bearing: 'degrees from east', width: 'm', going: 'm', risers: 'count' },
    apply(doc, { from, at, ...rest }) {
      storeyOf(doc, from);
      if (from + 1 >= doc.storeys.length) throw new Error('a stair rises to a storey above: add one first');
      const st = { from, at: point(at, 'a stair\'s foot'), ...DEFAULTS.stair, ...rest };
      positive(st.width, 'a stair\'s width'), positive(st.going, 'a stair\'s going');
      if (!(Number.isInteger(st.risers) && st.risers >= 2)) throw new Error('a stair has two risers at least');
      doc.stairs.push(st);
      return doc;
    },
  },
  setStair: {
    title: 'Change a stair',
    params: { stair: 'index', at: 'point', bearing: 'degrees', width: 'm', going: 'm', risers: 'count' },
    apply(doc, { stair, ...change }) {
      const st = doc.stairs[stair];
      if (!st) throw new Error('no such stair');
      const next = { ...st, ...change };
      if (change.at !== undefined) next.at = point(change.at, 'a stair\'s foot');
      positive(next.width, 'a stair\'s width'), positive(next.going, 'a stair\'s going');
      if (!(Number.isInteger(next.risers) && next.risers >= 2)) throw new Error('a stair has two risers at least');
      doc.stairs[stair] = next;
      return doc;
    },
  },
  removeStair: {
    title: 'Remove a stair',
    params: { stair: 'index' },
    apply(doc, { stair }) {
      if (!doc.stairs[stair]) throw new Error('no such stair');
      doc.stairs.splice(stair, 1);
      return doc;
    },
  },
  // ---- what stands in the rooms ----
  addPiece: {
    title: 'Set down a piece',
    params: { storey: 'index', kind: 'a kind of the catalogue', at: 'point: its middle', bearing: 'degrees from east: the way its front faces', width: 'm', depth: 'm', height: 'm', front: 'clear floor before it (m)',
      side: 'clear floor at each side (m)', around: 'clear floor all round (m)', finish: 'a cloth by name', name: 'text' },
    apply(doc, { storey, kind, at, bearing = 90, ...rest }) {
      storeyOf(doc, storey);
      if (!entry(kind)) throw new Error(`the catalogue has no ${kind}`);
      const piece = { ...newPiece(kind, storey, point(at, 'a piece\'s place'), bearing), ...rest };
      checkPiece(piece);
      if (!Array.isArray(doc.furniture)) doc.furniture = [];
      doc.furniture.push(piece);
      return doc;
    },
  },
  setPiece: {
    title: 'Change a piece',
    params: { piece: 'index', at: 'point', bearing: 'degrees', width: 'm', depth: 'm', height: 'm', front: 'm', side: 'm', around: 'm', finish: 'a cloth by name', name: 'text', storey: 'index' },
    apply(doc, { piece, ...change }) {
      const now = (doc.furniture || [])[piece];
      if (!now) throw new Error('no such piece');
      const next = { ...now, ...change };
      if (change.at !== undefined) next.at = point(change.at, 'a piece\'s place');
      storeyOf(doc, next.storey);
      checkPiece(next);
      doc.furniture[piece] = next;
      return doc;
    },
  },
  removePiece: {
    title: 'Take a piece away',
    params: { piece: 'index' },
    apply(doc, { piece }) {
      if (!(doc.furniture || [])[piece]) throw new Error('no such piece');
      doc.furniture.splice(piece, 1);
      return doc;
    },
  },
  // ---- the site ----
  setLot: {
    title: 'Mark the lot',
    params: { ring: 'points, or none to clear it', street: 'for each edge, whether it faces a street' },
    apply(doc, { ring, street }) {
      if (!ring) { doc.site.lot = null; return doc; }
      if (!Array.isArray(ring) || ring.length < 3) throw new Error('a lot has three corners at least');
      const r = ring.map((p) => point(p, 'a corner of the lot'));
      doc.site.lot = { ring: r, street: r.map((_, k) => Boolean(street && street[k])) };
      return doc;
    },
  },
  setStreet: {
    title: 'Say whether an edge of the lot faces a street',
    params: { edge: 'index', street: 'true | false' },
    apply(doc, { edge, street }) {
      const lot = doc.site.lot;
      if (!lot || !Number.isInteger(edge) || edge < 0 || edge >= lot.ring.length) throw new Error('no such edge of the lot');
      lot.street[edge] = Boolean(street);
      return doc;
    },
  },
  setPlace: {
    title: 'Say where the site is',
    params: { name: 'text', lat: 'degrees north (WGS 84)', lon: 'degrees east (WGS 84)' },
    apply(doc, { name, lat, lon }) {
      if (!(Number.isFinite(lat) && Math.abs(lat) <= 90 && Number.isFinite(lon) && Math.abs(lon) <= 180)) throw new Error('a place is a latitude and a longitude in degrees');
      doc.site.place = { name: name || '', lat, lon };
      return doc;
    },
  },
  // ---- what is declared ----
  declare: {
    title: 'Declare a value',
    params: { path: 'where: name | brief… | siting… | code… | rates… | fabric… | roof… | shell… | light… | street… | interior… | site.scheme | site.boundary…', value: 'the value; none to withdraw it' },
    apply(doc, { path, value }) {
      if (!Array.isArray(path) || !path.length) throw new Error('a path names what is declared');
      const roots = ['name', 'brief', 'siting', 'code', 'rates', 'fabric', 'roof', 'shell', 'light', 'street', 'interior', 'site'];
      if (!roots.includes(path[0]) || (path[0] === 'site' && path[1] !== 'scheme' && path[1] !== 'boundary')) throw new Error(`nothing is declared at ${path.join('.')}`);
      let at = doc;
      for (let k = 0; k + 1 < path.length; k++) {
        if (at[path[k]] === undefined || at[path[k]] === null || typeof at[path[k]] !== 'object') at[path[k]] = typeof path[k + 1] === 'number' ? [] : {};
        at = at[path[k]];
      }
      const last = path[path.length - 1];
      if (value === undefined) { if (Array.isArray(at)) at.splice(last, 1); else delete at[last]; }
      else at[last] = value;
      return doc;
    },
  },
};

// A document changed by an operation; the document given is left as it was.
export function apply(doc, name, args = {}) {
  const op = OPERATIONS[name];
  if (!op) throw new Error(`no such operation: ${name}`);
  return op.apply(structuredClone(doc), args);
}

// A document and the documents before it: every operation can be undone, and a failed one changes nothing.
export class History {
  constructor(doc, limit = 200) {
    this.doc = doc, this.past = [], this.future = [], this.limit = limit;
  }
  do(name, args) {
    const next = apply(this.doc, name, args);
    this.past.push({ name, doc: this.doc });
    if (this.past.length > this.limit) this.past.shift();
    this.future = [];
    this.doc = next;
    return next;
  }
  // Several operations as one step: all of them, or none.
  group(title, steps) {
    let next = this.doc;
    for (const [name, args] of steps) next = apply(next, name, args);
    this.past.push({ name: title, doc: this.doc });
    this.future = [];
    this.doc = next;
    return next;
  }
  undo() {
    const last = this.past.pop();
    if (!last) return null;
    this.future.push({ name: last.name, doc: this.doc });
    this.doc = last.doc;
    return last.name;
  }
  redo() {
    const next = this.future.pop();
    if (!next) return null;
    this.past.push({ name: next.name, doc: this.doc });
    this.doc = next.doc;
    return next.name;
  }
  reset(doc) {
    this.doc = doc, this.past = [], this.future = [];
  }
}

// A document read from a file or a store: its schema checked, what is missing given its default.
export function opened(raw) {
  if (!raw || raw.schema !== SCHEMA) throw new Error(`not a building of the schema ${SCHEMA}`);
  const doc = { ...newBuilding(raw.name || 'A house'), ...structuredClone(raw) };
  doc.site = { place: null, lot: null, scheme: 'unstated', ...(raw.site || {}) };
  if (!Array.isArray(doc.storeys) || !doc.storeys.length) doc.storeys = [newStorey()];
  doc.storeys = doc.storeys.map((s, k) => ({ ...newStorey(k ? `Storey ${k + 1}` : 'Ground'), ...s }));
  if (!Array.isArray(doc.stairs)) doc.stairs = [];
  if (!Array.isArray(doc.furniture)) doc.furniture = [];
  doc.interior = { passage: 0.6, ...(raw.interior || {}) };
  doc.roof = roofOf(doc), doc.shell = shellOf(doc);
  return doc;
}

// ---- an example ----------------------------------------------------------------------------------------------------------------

// A house of three bedrooms on a lot of 15 × 30 m whose south edge faces a street: drawn by the operations, as a person would.
export function exampleHouse() {
  let doc = newBuilding('A house of three bedrooms');
  const room = (x0, y0, x1, y1) => [[x0, y0], [x1, y0], [x1, y1], [x0, y1]];
  const run = (name, args) => { doc = apply(doc, name, args); };
  run('setLot', { ring: room(0, 0, 15, 30), street: [true, false, false, false] });
  run('addRoom', { storey: 0, ring: room(2, 6, 13, 16), t: DEFAULTS.outer });
  for (const [from, to] of [[[2, 11], [13, 11]], [[2, 12.2], [13, 12.2]], [[5.4, 12.2], [5.4, 16]], [[8.6, 12.2], [8.6, 16]], [[11.4, 12.2], [11.4, 16]]])
    run('addWall', { storey: 0, from, to, t: DEFAULTS.inner });
  run('addWall', { storey: 0, from: [8, 6], to: [8, 11], t: 0, kind: 'open' });
  for (const [at, kind, name] of [[[5, 8.5], 'living', 'Living'], [[10.5, 8.5], 'kitchen', 'Kitchen'], [[7, 11.6], 'hall', 'Hall'], [[3.7, 14], 'bedroom', 'Bedroom 1'],
    [[7, 14], 'bedroom', 'Bedroom 2'], [[10, 14], 'bedroom', 'Bedroom 3'], [[12.2, 14], 'bathroom', 'Bath']])
    run('setLabel', { storey: 0, at, kind, name });
  const s = () => doc.storeys[0];
  // The wall under a point, for placing openings as a person does, by where they are.
  const wallAt = (p) => {
    let best = -1, d = Infinity;
    for (let w = 0; w < s().walls.length; w++) { const pr = project(p, ...wallEnds(s(), w)); if (pr.distance < d) d = pr.distance, best = w; }
    return best;
  };
  const open = (p, kind, props = {}) => {
    const w = wallAt(p), [a] = wallEnds(s(), w);
    run('addOpening', { storey: 0, wall: w, kind, at: dist(a, p), ...props });
  };
  open([2, 11.6], 'door', DEFAULTS.entrance);                        // the entrance, at the hall's west end
  open([6.5, 11], 'door'); open([10, 11], 'door');                   // hall to living, hall to kitchen
  open([3.7, 12.2], 'door'); open([7, 12.2], 'door'); open([10, 12.2], 'door'); open([12.2, 12.2], 'door');
  open([5, 6], 'window', { width: 2.4, height: 2.1, sill: 0.1 });    // the living room, to the street
  open([2, 8.5], 'window', { width: 1.8, height: 1.2 });
  open([10.5, 6], 'window', { width: 1.8, height: 1.2 });            // the kitchen
  open([3.7, 16], 'window', { width: 1.5, height: 1.5, sill: 0.6 }); open([7, 16], 'window', { width: 1.5, height: 1.5, sill: 0.6 });
  open([10, 16], 'window', { width: 0.6, height: 1.0, sill: 1.1 });  // a bedroom with too little glass
  open([13, 14], 'window', { width: 0.6, height: 0.6, sill: 1.5 });  // the bath
  // The doors from the hall to the living room and the kitchen swing into those rooms, not into the hall.
  for (const [i, o] of s().openings.entries()) if (o.kind === 'door' && Math.abs(openingSpan(s(), o).middle[1] - 11) < 1e-6) run('setOpening', { storey: 0, opening: i, swing: -1 });
  // What stands in the rooms: each piece set down by its middle and the way it faces (0 east, 90 north, 180 west, −90 south).
  const set = (kind, x, y, bearing, more = {}) => run('addPiece', { storey: 0, kind, at: [x, y], bearing, ...more });
  set('sofa-3', 2.6, 8.5, 0); set('rug', 4.3, 8.5, 0); set('coffee-table', 4.1, 8.5, 0); set('armchair', 4.3, 6.65, 90);
  set('media-unit', 4.2, 10.735, -90); set('floor-lamp', 2.45, 10.2, 0); set('plant', 2.5, 6.55, 0); set('plant', 7.55, 10.5, 0);
  set('dining-4', 6.75, 7.9, 0); set('pendant', 6.75, 7.9, 0); set('picture', 7.0, 6.145, 90);
  set('fridge', 12.525, 6.475, 180); set('counter-sink', 12.565, 7.425, 180); set('counter-hob', 12.565, 8.475, 180); set('counter', 12.565, 9.825, 180);
  set('island', 10.2, 8.3, 180); set('pendant', 10.2, 7.8, 0); set('pendant', 10.2, 8.8, 0);
  set('console', 5.1, 11.97, -90);
  set('bed-double', 3.175, 14.55, 0); set('wardrobe', 4.745, 12.555, 90);
  set('bed-double', 7.495, 14.55, 180, { finish: 'rose' }); set('desk', 5.78, 13.05, 0);
  set('bed-single', 9.13, 14.85, -90); set('bedside', 9.83, 15.675, -90); set('wardrobe', 11.045, 14.5, 180);
  set('toilet', 11.805, 13.6, 0); set('basin', 11.695, 14.5, 0); set('shower', 12.425, 15.425, -90);
  // The outside: a gabled roof of clay tiles whose ridge runs east and west, weatherboards, a hedge about the garden, two trees.
  run('declare', { path: ['roof'], value: { ...DEFAULT_ROOF, form: 'gable', pitch: 27.5, eaves: 0.45, cover: 'tiles', gables: [[2, 11], [13, 11]] } });
  run('declare', { path: ['shell'], value: { cladding: 'boards', tone: 'chalk' } });
  run('declare', { path: ['site', 'boundary'], value: { kind: 'hedge', height: 1.1 } });
  set('tree', 4.2, 23.5, 0); set('tree-small', 11.6, 21.2, 0); set('shrub', 3.2, 4.6, 0); set('shrub', 4.5, 4.9, 0); set('bench', 9.5, 19.0, -90); set('tree-small', 12.3, 2.8, 0);
  return doc;
}
