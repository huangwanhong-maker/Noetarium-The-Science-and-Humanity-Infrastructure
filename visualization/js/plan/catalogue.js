// What stands in a building's rooms: a catalogue of furniture and fixtures, each a rectangle on the floor with the space it needs
// before it, its symbol on a plan, and where it snaps to (ADR 0096).
//
// A PIECE of the document is {storey, kind, at: [x, y], bearing, width, depth, height, front, side, around, obstructs, finish,
// model}:
// its middle on the ground, the direction its front faces (degrees counter-clockwise from east), its size, the clear floor it
// needs before it, at each side and all round, whether it stands on the floor in a person's way, its finish, and, where it has
// one, a model of one's own ({id, name}: a glTF file a work keeps) drawn in the round in place of the catalogue's. Its local frame:
// lx to its right along its width, ly toward its front; the footprint is |lx| ≤ width/2, |ly| ≤ depth/2, its back at ly = −depth/2.
//
// The sizes and the spaces of the catalogue are a designer's defaults, not a standard's: a piece once placed carries its own, and
// a person may change them. What follows of a room that holds pieces (whether each is within it, apart from the others, usable and
// reached) is the architecture domain's `interior`.
//
// A piece's SYMBOL is a list of marks in its local frame, drawn by draw.js as an architect's plan shows it: a bed with its pillows,
// a table with its chairs, a basin with its bowl.

import { TOL, add, dist, project, scale, sub, toGrid } from './geometry.js';

// Finishes: cloths for what is upholstered or woven, by name. A piece keeps its finish's name; a renderer gives it its colour.
export const FINISHES = {
  linen: '#d8d0c2', oat: '#cbb89b', rose: '#d6a3ad', sage: '#a3b39a', lilac: '#b3a7d1', sky: '#a9bfd0', clay: '#c08a72', slate: '#6c7280',
};

export const GROUPS = [['living', 'Living'], ['dining', 'Dining'], ['kitchen', 'Kitchen'], ['bedroom', 'Bedroom'], ['bath', 'Bath and laundry'], ['other', 'Other'], ['garden', 'Garden and street']];

// kind, title, group, width, depth, height, then what it needs and how it stands.
const piece = (kind, title, group, width, depth, height, more = {}) => ({ kind, title, group, width, depth, height, front: 0, side: 0, around: 0, obstructs: true, wall: false, ...more });
export const CATALOGUE = [
  piece('sofa-3', 'Sofa, three seats', 'living', 2.2, 0.95, 0.82, { front: 0.5, wall: true, finish: 'linen' }),
  piece('sofa-2', 'Sofa, two seats', 'living', 1.6, 0.92, 0.82, { front: 0.5, wall: true, finish: 'linen' }),
  piece('armchair', 'Armchair', 'living', 0.86, 0.86, 0.8, { front: 0.5, finish: 'rose' }),
  piece('coffee-table', 'Coffee table', 'living', 1.1, 0.6, 0.38),
  piece('side-table', 'Side table', 'living', 0.45, 0.45, 0.5),
  piece('media-unit', 'Media unit', 'living', 1.8, 0.42, 0.5, { front: 0.5, wall: true }),
  piece('bookshelf', 'Bookshelf', 'living', 0.9, 0.32, 2.0, { front: 0.6, wall: true }),
  piece('rug', 'Rug', 'living', 2.4, 1.7, 0.012, { obstructs: false, finish: 'oat' }),
  piece('floor-lamp', 'Floor lamp', 'living', 0.36, 0.36, 1.6),
  piece('plant', 'Plant', 'living', 0.5, 0.5, 1.4),
  piece('dining-6', 'Table for six', 'dining', 1.8, 1.7, 0.76, { around: 0.35, finish: 'sage' }),
  piece('dining-4', 'Table for four', 'dining', 1.2, 1.6, 0.76, { around: 0.35, finish: 'sage' }),
  piece('chair', 'Chair', 'dining', 0.45, 0.5, 0.85, { finish: 'sage' }),
  piece('stool', 'Stool', 'dining', 0.38, 0.38, 0.7),
  piece('counter', 'Kitchen counter', 'kitchen', 1.8, 0.62, 0.9, { front: 0.9, wall: true }),
  piece('counter-sink', 'Counter with a sink', 'kitchen', 1.2, 0.62, 0.9, { front: 0.9, wall: true }),
  piece('counter-hob', 'Counter with a hob', 'kitchen', 0.9, 0.62, 0.9, { front: 0.9, wall: true }),
  piece('island', 'Kitchen island', 'kitchen', 2.0, 0.95, 0.9, { around: 0.8 }),
  piece('fridge', 'Refrigerator', 'kitchen', 0.7, 0.7, 1.85, { front: 0.9, wall: true }),
  piece('oven-tower', 'Oven tower', 'kitchen', 0.6, 0.62, 2.1, { front: 0.9, wall: true }),
  piece('bed-double', 'Double bed', 'bedroom', 1.6, 2.1, 0.5, { front: 0.6, side: 0.5, wall: true, finish: 'linen' }),
  piece('bed-single', 'Single bed', 'bedroom', 0.95, 2.05, 0.5, { front: 0.5, wall: true, finish: 'sky' }),
  piece('bedside', 'Bedside table', 'bedroom', 0.45, 0.4, 0.5, { wall: true }),
  piece('wardrobe', 'Wardrobe', 'bedroom', 1.2, 0.6, 2.1, { front: 0.8, wall: true }),
  piece('desk', 'Desk and chair', 'bedroom', 1.3, 0.65, 0.75, { front: 0.8, wall: true, finish: 'lilac' }),
  piece('dresser', 'Chest of drawers', 'bedroom', 1.0, 0.48, 0.8, { front: 0.7, wall: true }),
  piece('toilet', 'Toilet', 'bath', 0.38, 0.7, 0.8, { front: 0.6, side: 0.2, wall: true }),
  piece('basin', 'Basin', 'bath', 0.75, 0.48, 0.86, { front: 0.6, wall: true }),
  piece('bath', 'Bath', 'bath', 1.7, 0.75, 0.56, { front: 0.6, wall: true }),
  piece('shower', 'Shower', 'bath', 0.9, 0.9, 2.0, { front: 0.6, wall: true }),
  piece('washer', 'Washing machine', 'bath', 0.6, 0.6, 0.85, { front: 0.8, wall: true }),
  piece('console', 'Console', 'other', 1.0, 0.35, 0.8, { front: 0.4, wall: true }),
  piece('picture', 'Picture', 'other', 0.9, 0.04, 0.6, { obstructs: false, wall: true, lift: 1.5 }),
  piece('pendant', 'Pendant lamp', 'other', 0.38, 0.38, 0.3, { obstructs: false, hung: true }),
  piece('car', 'Car', 'other', 1.8, 4.5, 1.45, { around: 0.5 }),
  piece('own', 'A model of one\'s own', 'other', 1.0, 1.0, 1.0),
  // What stands outdoors (ADR 0097): judged by no room. A tree's width is its crown's; a person passes beneath it.
  piece('tree', 'Tree', 'garden', 5.0, 5.0, 7.5, { outdoor: true, obstructs: false }),
  piece('tree-small', 'Small tree', 'garden', 2.8, 2.8, 4.2, { outdoor: true, obstructs: false }),
  piece('shrub', 'Shrub', 'garden', 1.1, 1.1, 0.95, { outdoor: true }),
  piece('hedge', 'Hedge', 'garden', 2.0, 0.6, 1.2, { outdoor: true }),
  piece('planter', 'Planter', 'garden', 1.4, 0.45, 0.75, { outdoor: true }),
  piece('bench', 'Bench', 'garden', 1.6, 0.55, 0.82, { outdoor: true, front: 0.5 }),
  piece('outdoor-table', 'Garden table for four', 'garden', 1.2, 1.6, 0.74, { outdoor: true, around: 0.35, finish: 'clay' }),
  piece('lamp-post', 'Lamp post', 'garden', 0.3, 0.3, 3.6, { outdoor: true }),
];
export const entry = (kind) => CATALOGUE.find((c) => c.kind === kind) || null;

// A piece of a kind as the document keeps it, at a place and a bearing.
export function newPiece(kind, storey, at, bearing = 90) {
  const c = entry(kind);
  if (!c) throw new Error(`the catalogue has no ${kind}`);
  const p = { storey, kind, at: [at[0], at[1]], bearing, width: c.width, depth: c.depth, height: c.height, front: c.front, side: c.side, around: c.around, obstructs: c.obstructs };
  if (c.finish) p.finish = c.finish;
  if (c.outdoor) p.outdoor = true;
  return p;
}

// ---- a piece on the ground -----------------------------------------------------------------------------------------------------

// Its right and its front, as unit vectors.
export function pieceAxes(p) {
  const b = p.bearing * Math.PI / 180, c = Math.cos(b), s = Math.sin(b);
  return { u: [s, -c], f: [c, s] };
}
export function pieceAt(p, lx, ly) {
  const { u, f } = pieceAxes(p);
  return [p.at[0] + lx * u[0] + ly * f[0], p.at[1] + lx * u[1] + ly * f[1]];
}
export function pieceLocal(p, q) {
  const { u, f } = pieceAxes(p), d = sub(q, p.at);
  return [d[0] * u[0] + d[1] * u[1], d[0] * f[0] + d[1] * f[1]];
}
const box = (p, x0, y0, x1, y1) => [pieceAt(p, x0, y0), pieceAt(p, x1, y0), pieceAt(p, x1, y1), pieceAt(p, x0, y1)];
export const pieceRing = (p) => box(p, -p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2);
// The clear floor it needs: before it, at each side, all round.
export function pieceSpaces(p) {
  const w = p.width / 2, d = p.depth / 2, out = [];
  if (p.around > 0) {
    const a = p.around;
    out.push(box(p, -w - a, d, w + a, d + a), box(p, -w - a, -d - a, w + a, -d), box(p, w, -d, w + a, d), box(p, -w - a, -d, -w, d));
    return out;
  }
  if (p.front > 0) out.push(box(p, -w, d, w, d + p.front));
  if (p.side > 0) out.push(box(p, w, -d, w + p.side, d), box(p, -w - p.side, -d, -w, d));
  return out;
}
export const inPiece = (p, q, grow = 0) => { const [lx, ly] = pieceLocal(p, q); return Math.abs(lx) <= p.width / 2 + grow && Math.abs(ly) <= p.depth / 2 + grow; };

// Where a piece held at q goes: against the face of a wall within reach, its back to it, when it is a piece that stands against
// walls; else on the grid, turned as it was. Answers {at, bearing, wall}.
export function snapPiece(storey, p, q, { reach = 0.35, step = 0.05, against = true } = {}) {
  let best = null;
  if (against && storey) {
    for (let w = 0; w < storey.walls.length; w++) {
      const wall = storey.walls[w];
      if (wall.kind === 'open') continue;
      const a = storey.vertices[wall.a], b = storey.vertices[wall.b], L = dist(a, b);
      if (L <= TOL) continue;
      const pr = project(q, a, b), d = [(b[0] - a[0]) / L, (b[1] - a[1]) / L];
      // The side of the wall the pointer is on, and how far the piece's back would be from that face.
      const side = (q[0] - a[0]) * -d[1] + (q[1] - a[1]) * d[0] >= 0 ? 1 : -1, n = [-d[1] * side, d[0] * side];
      const gap = pr.distance - wall.t / 2 - p.depth / 2;
      if (gap > reach || pr.t * L < -1e-9 || pr.t * L > L + 1e-9) continue;
      if (!best || Math.abs(gap) < best.gap) {
        const along = Math.max(Math.min(p.width / 2, L / 2), Math.min(L - Math.min(p.width / 2, L / 2), toGrid(pr.t * L, step)));
        best = { gap: Math.abs(gap), wall: w, at: add(add(a, scale(d, along)), scale(n, wall.t / 2 + p.depth / 2)), bearing: Math.atan2(n[1], n[0]) * 180 / Math.PI };
      }
    }
  }
  if (best) return { at: best.at, bearing: Math.round(best.bearing * 1e6) / 1e6, wall: best.wall };
  return { at: [toGrid(q[0], step), toGrid(q[1], step)], bearing: p.bearing, wall: null };
}

// ---- symbols -------------------------------------------------------------------------------------------------------------------

// Marks in the local frame: {points, closed, cls} polygons and polylines; circles as polygons, so that a turn keeps them true.
function pen() {
  const marks = [];
  const arc = (cx, cy, rx, ry, a0, a1, n) => { const pts = []; for (let i = 0; i <= n; i++) { const a = a0 + (a1 - a0) * i / n; pts.push([cx + rx * Math.cos(a), cy + ry * Math.sin(a)]); } return pts; };
  const api = {
    marks,
    rect(x0, y0, x1, y1, cls = '', r = 0) {
      const lo = [Math.min(x0, x1), Math.min(y0, y1)], hi = [Math.max(x0, x1), Math.max(y0, y1)];
      r = Math.min(r, (hi[0] - lo[0]) / 2, (hi[1] - lo[1]) / 2);
      const pts = r > 0 ? [...arc(hi[0] - r, hi[1] - r, r, r, 0, Math.PI / 2, 4), ...arc(lo[0] + r, hi[1] - r, r, r, Math.PI / 2, Math.PI, 4), ...arc(lo[0] + r, lo[1] + r, r, r, Math.PI, 1.5 * Math.PI, 4), ...arc(hi[0] - r, lo[1] + r, r, r, 1.5 * Math.PI, 2 * Math.PI, 4)]
        : [[lo[0], lo[1]], [hi[0], lo[1]], [hi[0], hi[1]], [lo[0], hi[1]]];
      marks.push({ points: pts, closed: true, cls });
      return api;
    },
    ellipse(cx, cy, rx, ry, cls = '') { marks.push({ points: arc(cx, cy, rx, ry, 0, 2 * Math.PI, 28).slice(0, -1), closed: true, cls }); return api; },
    circle(cx, cy, r, cls = '') { return api.ellipse(cx, cy, r, r, cls); },
    line(x0, y0, x1, y1, cls = '') { marks.push({ points: [[x0, y0], [x1, y1]], closed: false, cls }); return api; },
    path(points, cls = '', closed = false) { marks.push({ points, closed, cls }); return api; },
  };
  return api;
}

const sofa = (p, seats) => {
  const w = p.width / 2, d = p.depth / 2, arm = 0.17, back = 0.2, s = pen();
  s.rect(-w, -d, w, d, 'body', 0.07).rect(-w, -d, w, -d + back, 'part', 0.05).rect(-w, -d + back, -w + arm, d, 'part', 0.05).rect(w - arm, -d + back, w, d, 'part', 0.05);
  const span = (2 * w - 2 * arm) / seats;
  for (let i = 0; i < seats; i++) s.rect(-w + arm + i * span + 0.015, -d + back + 0.015, -w + arm + (i + 1) * span - 0.015, d - 0.02, 'soft', 0.05);
  return s.marks;
};
const diningSet = (p, perSide) => {
  const w = p.width / 2, table = 0.45, s = pen(), seat = 0.42;
  for (const side of [-1, 1]) for (let i = 0; i < perSide; i++) {
    const cx = -w + (2 * w) * (i + 0.5) / perSide, y0 = side * (table - 0.12), y1 = side * (table - 0.12 + seat);
    s.rect(cx - seat / 2, y0, cx + seat / 2, y1, 'soft', 0.06).line(cx - seat / 2 + 0.03, side * (table - 0.12 + seat - 0.05), cx + seat / 2 - 0.03, side * (table - 0.12 + seat - 0.05), 'thin');
  }
  s.rect(-w, -table, w, table, 'body', 0.04);
  return s.marks;
};
const bed = (p, pillows) => {
  const w = p.width / 2, d = p.depth / 2, s = pen();
  s.rect(-w, -d, w, d, 'body', 0.04).rect(-w, -d, w, -d + 0.07, 'part');
  const span = (2 * w - 0.16) / pillows;
  for (let i = 0; i < pillows; i++) s.rect(-w + 0.08 + i * span + 0.03, -d + 0.14, -w + 0.08 + (i + 1) * span - 0.03, -d + 0.52, 'soft', 0.08);
  s.rect(-w + 0.03, -d + 0.66, w - 0.03, d - 0.03, 'soft', 0.05).line(-w + 0.03, -d + 0.86, w - 0.03, -d + 0.86, 'thin');
  return s.marks;
};
const counter = (p, what) => {
  const w = p.width / 2, d = p.depth / 2, s = pen();
  s.rect(-w, -d, w, d, 'body').line(-w, d - 0.03, w, d - 0.03, 'thin');
  if (what === 'sink') s.rect(-0.3, -0.19, 0.12, 0.17, 'soft', 0.05).circle(-0.09, -0.01, 0.022, 'thin').rect(0.16, -0.19, 0.42, 0.17, 'thin', 0.03).circle(-0.09, -0.25, 0.025, 'part');
  if (what === 'hob') { s.rect(-0.3, -0.24, 0.3, 0.2, 'part', 0.02); for (const [x, y, r] of [[-0.15, -0.12, 0.09], [0.15, -0.12, 0.075], [-0.15, 0.09, 0.075], [0.15, 0.09, 0.09]]) s.circle(x, y, r, 'thin'); }
  return s.marks;
};

const SYMBOLS = {
  'sofa-3': (p) => sofa(p, 3),
  'sofa-2': (p) => sofa(p, 2),
  armchair: (p) => sofa(p, 1),
  'coffee-table': (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.05).rect(-p.width / 2 + 0.06, -p.depth / 2 + 0.06, p.width / 2 - 0.06, p.depth / 2 - 0.06, 'thin', 0.03).marks,
  'side-table': (p) => pen().circle(0, 0, p.width / 2, 'body').circle(0, 0, p.width / 2 - 0.05, 'thin').marks,
  'media-unit': (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body').rect(-0.62, -p.depth / 2 + 0.1, 0.62, -p.depth / 2 + 0.15, 'part').line(-p.width / 6, -p.depth / 2, -p.width / 6, p.depth / 2, 'thin').line(p.width / 6, -p.depth / 2, p.width / 6, p.depth / 2, 'thin').marks,
  bookshelf: (p) => { const s = pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body'); for (let i = 1; i < 3; i++) s.line(-p.width / 2 + i * p.width / 3, -p.depth / 2, -p.width / 2 + i * p.width / 3, p.depth / 2, 'thin'); return s.marks; },
  rug: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'rug', 0.03).rect(-p.width / 2 + 0.12, -p.depth / 2 + 0.12, p.width / 2 - 0.12, p.depth / 2 - 0.12, 'thin dash', 0.02).marks,
  'floor-lamp': (p) => pen().circle(0, 0, p.width / 2, 'body').circle(0, 0, 0.05, 'part').marks,
  plant: (p) => { const s = pen().circle(0, 0, p.width / 2, 'leaf'); for (let i = 0; i < 7; i++) { const a = i * 2 * Math.PI / 7 + 0.3; s.line(0, 0, 0.21 * Math.cos(a), 0.21 * Math.sin(a), 'thin'); } return s.circle(0, 0, 0.11, 'thin').marks; },
  'dining-6': (p) => diningSet(p, 3),
  'dining-4': (p) => diningSet(p, 2),
  chair: (p) => pen().rect(-p.width / 2, -p.depth / 2 + 0.04, p.width / 2, p.depth / 2, 'soft', 0.07).rect(-p.width / 2, -p.depth / 2, p.width / 2, -p.depth / 2 + 0.07, 'part', 0.03).marks,
  stool: (p) => pen().circle(0, 0, p.width / 2, 'soft').marks,
  counter: (p) => counter(p),
  'counter-sink': (p) => counter(p, 'sink'),
  'counter-hob': (p) => counter(p, 'hob'),
  island: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.02).rect(-p.width / 2 + 0.04, -p.depth / 2 + 0.04, p.width / 2 - 0.04, p.depth / 2 - 0.04, 'thin').marks,
  fridge: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.02).line(-p.width / 2, p.depth / 2 - 0.07, p.width / 2, p.depth / 2 - 0.07, 'thin').line(p.width / 2 - 0.08, p.depth / 2 - 0.03, p.width / 2 - 0.2, p.depth / 2 - 0.03, 'part').marks,
  'oven-tower': (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body').rect(-p.width / 2 + 0.06, p.depth / 2 - 0.1, p.width / 2 - 0.06, p.depth / 2 - 0.04, 'part').marks,
  'bed-double': (p) => bed(p, 2),
  'bed-single': (p) => bed(p, 1),
  bedside: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.02).circle(0, -0.02, 0.09, 'thin').marks,
  wardrobe: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body').line(-p.width / 2 + 0.05, -0.02, p.width / 2 - 0.05, -0.02, 'thin dash').line(0, p.depth / 2 - 0.05, 0, p.depth / 2, 'thin').line(-p.width / 2, p.depth / 2 - 0.04, p.width / 2, p.depth / 2 - 0.04, 'thin').marks,
  desk: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.02).rect(-0.22, p.depth / 2 + 0.08, 0.22, p.depth / 2 + 0.5, 'soft', 0.08).rect(-0.2, p.depth / 2 + 0.45, 0.2, p.depth / 2 + 0.52, 'part', 0.03).marks,
  dresser: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body').line(0, -p.depth / 2, 0, p.depth / 2, 'thin').line(-p.width / 2, p.depth / 2 - 0.04, p.width / 2, p.depth / 2 - 0.04, 'thin').marks,
  toilet: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, -p.depth / 2 + 0.19, 'body', 0.03).ellipse(0, 0.08, p.width / 2 - 0.01, 0.26, 'body').ellipse(0, 0.1, p.width / 2 - 0.07, 0.17, 'thin').marks,
  basin: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.02).ellipse(0, 0.02, 0.22, 0.15, 'soft').circle(0, -0.17, 0.022, 'part').circle(0, 0.02, 0.018, 'thin').marks,
  bath: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.04).rect(-p.width / 2 + 0.07, -p.depth / 2 + 0.07, p.width / 2 - 0.07, p.depth / 2 - 0.07, 'soft', 0.22).circle(p.width / 2 - 0.3, 0, 0.025, 'thin').circle(p.width / 2 - 0.05, 0, 0.02, 'part').marks,
  shower: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'soft').line(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'thin').line(-p.width / 2, p.depth / 2, p.width / 2, -p.depth / 2, 'thin').circle(0, 0, 0.04, 'thin').line(-p.width / 2, p.depth / 2, p.width / 2, p.depth / 2, 'part').marks,
  washer: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.03).circle(0, 0.02, 0.2, 'thin').circle(0, 0.02, 0.13, 'thin').marks,
  console: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.02).marks,
  picture: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'part').marks,
  pendant: (p) => pen().circle(0, 0, p.width / 2, 'thin dash').circle(0, 0, 0.04, 'part').marks,
  own: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body').line(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'thin').line(-p.width / 2, p.depth / 2, p.width / 2, -p.depth / 2, 'thin').marks,
  car: (p) => { const w = p.width / 2, d = p.depth / 2; return pen().rect(-w, -d, w, d, 'body', 0.35).rect(-w + 0.14, -d + 0.9, w - 0.14, d - 1.35, 'thin', 0.15).line(-w + 0.14, d - 1.35, w - 0.14, d - 1.35, 'part').marks; },
};

// A crown seen from above: an outline of lobes about a trunk, as plans draw a tree.
const crown = (p, lobes) => {
  const r = p.width / 2, s = pen(), pts = [];
  for (let i = 0; i < lobes * 8; i++) { const a = i * 2 * Math.PI / (lobes * 8); const k = 1 - 0.07 * (1 + Math.cos(a * lobes)); pts.push([r * k * Math.cos(a), r * k * Math.sin(a)]); }
  s.path(pts, 'crown', true).circle(0, 0, Math.min(0.14, r * 0.12), 'part');
  for (let i = 0; i < lobes; i++) { const a = (i + 0.5) * 2 * Math.PI / lobes; s.line(0.2 * r * Math.cos(a), 0.2 * r * Math.sin(a), 0.62 * r * Math.cos(a), 0.62 * r * Math.sin(a), 'thin'); }
  return s.marks;
};
Object.assign(SYMBOLS, {
  tree: (p) => crown(p, 9),
  'tree-small': (p) => crown(p, 7),
  shrub: (p) => crown(p, 5),
  hedge: (p) => pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'crown', Math.min(0.25, p.depth / 2)).marks,
  planter: (p) => { const s = pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.03); const n = Math.max(1, Math.round(p.width / 0.45)); for (let i = 0; i < n; i++) s.circle(-p.width / 2 + (i + 0.5) * p.width / n, 0, Math.min(0.17, p.depth / 2 - 0.04), 'leaf'); return s.marks; },
  bench: (p) => { const s = pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body', 0.03).rect(-p.width / 2, -p.depth / 2, p.width / 2, -p.depth / 2 + 0.09, 'part', 0.03); for (let i = 1; i < 4; i++) s.line(-p.width / 2 + 0.04, -p.depth / 2 + 0.09 + i * (p.depth - 0.09) / 4, p.width / 2 - 0.04, -p.depth / 2 + 0.09 + i * (p.depth - 0.09) / 4, 'thin'); return s.marks; },
  'outdoor-table': (p) => diningSet(p, 2),
  'lamp-post': (p) => pen().circle(0, 0, p.width / 2, 'body').circle(0, 0, 0.05, 'part').line(-p.width / 2, 0, p.width / 2, 0, 'thin').line(0, -p.depth / 2, 0, p.depth / 2, 'thin').marks,
});

// The marks of a piece's symbol in its local frame; a kind the catalogue does not draw is its rectangle.
export function symbol(p) {
  const draw = SYMBOLS[p.kind];
  return draw ? draw(p) : pen().rect(-p.width / 2, -p.depth / 2, p.width / 2, p.depth / 2, 'body').marks;
}
