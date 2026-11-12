// A building's plan as a drawing: one storey of the document, with what the domain read of it, as SVG; and what lies under a pointer.
//
// The drawing is a string of SVG, a function of its state and nothing else, so that it can be made and checked without a page.
// Its marks are those of an architect's plan: walls filled to their thickness, doors with their swing, windows as panes in the wall,
// stairs with their treads and an arrow up, what stands in the rooms by its symbol, each room named with its area; north is up,
// and a bar gives the scale. The lot, the streets it faces, the ground its rules leave and its surroundings lie beneath. Colours
// come from the page's style by class.
//
// state = {doc, reading, storey, view: {cx, cy, scale}, size: [w, h], selection, ghost, context, marks, show}
//   view      the ground point at the middle of the drawing, and pixels per metre
//   selection {type: 'wall' | 'vertex' | 'opening' | 'room' | 'stair' | 'edge' | 'piece', index}
//   ghost     what a tool is about to make: {wall: [from, to]}, {ring}, {opening: {wall, at, width, kind}}, {lot: [points…], cursor},
//             {snap: {point, kind}}, {vertex: point}
//   marks     per room of the storey, short words set beside its name (rules it fails)
//   roofs     the roofs the domain read over this storey (reading.shell.roofs), drawn as a roof plan; none for no roof
//   show      {grid, surroundings, below, vertices; furniture: false leaves the pieces out; names: false the rooms' names;
//             furnishings: false north and the scale bar}
//   eye       {at, heading, storey}: where one who walks through the building stands and looks, marked when it is on this storey

import { inPiece, pieceAt, pieceSpaces, symbol } from './catalogue.js';
import { dist, inRing, interiorPoint, lerp, project, ringArea } from './geometry.js';
import { openingSpan, wallEnds } from './model.js';

const esc = (s) => String(s).replace(/[&<>"]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
const f1 = (x) => x.toFixed(1);

export const toScreen = (view, size) => (p) => [size[0] / 2 + (p[0] - view.cx) * view.scale, size[1] / 2 - (p[1] - view.cy) * view.scale];
export const toWorld = (view, size) => (q) => [view.cx + (q[0] - size[0] / 2) / view.scale, view.cy - (q[1] - size[1] / 2) / view.scale];

// The view that holds points within a size, with a margin in pixels.
export function fit(points, size, margin = 70) {
  if (!points.length) return { cx: 0, cy: 0, scale: 40 };
  let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
  for (const p of points) x0 = Math.min(x0, p[0]), y0 = Math.min(y0, p[1]), x1 = Math.max(x1, p[0]), y1 = Math.max(y1, p[1]);
  const w = Math.max(x1 - x0, 1), h = Math.max(y1 - y0, 1);
  const scale = Math.max(2, Math.min(160, Math.min((size[0] - 2 * margin) / w, (size[1] - 2 * margin) / h)));
  return { cx: (x0 + x1) / 2, cy: (y0 + y1) / 2, scale };
}

// A stair's plan from the document: its ring, its treads, and the arrow up.
export function stairPlan(st) {
  const b = (st.bearing ?? 90) * Math.PI / 180, u = [Math.cos(b), Math.sin(b)], n = [-u[1] * st.width / 2, u[0] * st.width / 2];
  const run = Math.max(0, st.risers - 1) * st.going, head = [st.at[0] + run * u[0], st.at[1] + run * u[1]];
  const ring = [[st.at[0] - n[0], st.at[1] - n[1]], [head[0] - n[0], head[1] - n[1]], [head[0] + n[0], head[1] + n[1]], [st.at[0] + n[0], st.at[1] + n[1]]];
  const treads = [];
  for (let k = 1; k < st.risers - 1; k++) {
    const c = [st.at[0] + k * st.going * u[0], st.at[1] + k * st.going * u[1]];
    treads.push([[c[0] - n[0], c[1] - n[1]], [c[0] + n[0], c[1] + n[1]]]);
  }
  return { ring, treads, foot: st.at, head, run };
}

const path = (S, ring) => 'M' + ring.map((p) => S(p).map(f1).join(' ')).join('L') + 'Z';

function niceLength(metres) {
  const steps = [0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500];
  return steps.reduce((best, s) => (s <= metres ? s : best), steps[0]);
}

export function planSvg(state) {
  const { doc, reading, storey: k, view, size, selection, ghost, context, marks = [], show = {}, eye = null, roofs = null } = state;
  const S = toScreen(view, size), s = doc.storeys[k], R = reading && reading.storeys[k];
  const px = view.scale, out = [];
  const sel = (type, index) => (selection && selection.type === type && selection.index === index ? ' sel' : '');

  // The grid: every metre, every fifth a little stronger.
  if (show.grid !== false && px >= 9) {
    const W = toWorld(view, size), [x0, y1] = W([0, 0]), [x1, y0] = W(size);
    const lines = [];
    for (let x = Math.ceil(x0); x <= x1; x++) lines.push(`<line class="grid${x % 5 === 0 ? ' five' : ''}" x1="${f1(S([x, 0])[0])}" y1="0" x2="${f1(S([x, 0])[0])}" y2="${size[1]}"/>`);
    for (let y = Math.ceil(y0); y <= y1; y++) lines.push(`<line class="grid${y % 5 === 0 ? ' five' : ''}" x1="0" y1="${f1(S([0, y])[1])}" x2="${size[0]}" y2="${f1(S([0, y])[1])}"/>`);
    if (lines.length < 600) out.push(`<g>${lines.join('')}</g>`);
  }

  // The surroundings: ways, then buildings.
  if (context && show.surroundings !== false) {
    const ways = context.ways.map((w) => `<polyline class="way${w.people ? ' foot' : ''}" stroke-width="${f1(Math.max(1, w.width * px))}" points="${w.line.map((p) => S(p).map(f1).join(',')).join(' ')}"/>`);
    const names = [];
    for (const w of context.ways) {
      if (!w.name || w.people || px < 3) continue;
      const mid = w.line[Math.floor(w.line.length / 2)], q = S(mid);
      if (q[0] > 40 && q[0] < size[0] - 40 && q[1] > 20 && q[1] < size[1] - 20 && !names.some((n) => n.name === w.name)) names.push({ name: w.name, q });
    }
    out.push(`<g>${ways.join('')}</g>`);
    out.push(`<g>${context.buildings.map((b) => `<path class="neighbour" d="${path(S, b.ring)}"/>`).join('')}</g>`);
    out.push(`<g>${names.map((n) => `<text class="wayname" x="${f1(n.q[0])}" y="${f1(n.q[1])}">${esc(n.name)}</text>`).join('')}</g>`);
  }

  // The lot, the edges that face a street, and the ground its rules leave.
  const lot = doc.site.lot;
  if (lot) {
    out.push(`<path class="lot" d="${path(S, lot.ring)}"/>`);
    const envelope = reading && reading.siting && reading.siting.envelope;
    if (envelope && envelope.length >= 3) out.push(`<path class="envelope" d="${path(S, envelope)}"/>`);
    lot.ring.forEach((p, e) => {
      const q = lot.ring[(e + 1) % lot.ring.length], a = S(p), b = S(q);
      out.push(`<line class="lotedge${lot.street[e] ? ' street' : ''}${sel('edge', e)}" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/>`);
      if (lot.street[e] && dist(a, b) > 70) {
        let angle = Math.atan2(b[1] - a[1], b[0] - a[0]) * 180 / Math.PI;
        if (angle > 90 || angle < -90) angle += 180;
        // The word sits outside the lot: on the side away from its middle.
        const mid = [(a[0] + b[0]) / 2, (a[1] + b[1]) / 2], c = S(interiorPoint(lot.ring));
        const n = [-(b[1] - a[1]), b[0] - a[0]], L = Math.hypot(n[0], n[1]) || 1, side = (c[0] - mid[0]) * n[0] + (c[1] - mid[1]) * n[1] > 0 ? -1 : 1;
        out.push(`<text class="streetword" transform="translate(${f1(mid[0] + side * n[0] / L * 13)} ${f1(mid[1] + side * n[1] / L * 13)}) rotate(${f1(angle)})">street</text>`);
      }
    });
  }

  // The storey below, faintly, under an upper storey.
  if (k > 0 && show.below !== false) {
    const below = doc.storeys[k - 1];
    out.push(`<g>${below.walls.map((w, i) => { const [a, b] = wallEnds(below, i).map(S); return `<line class="below" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/>`; }).join('')}</g>`);
  }

  // The rooms.
  if (R && R.sound) {
    out.push(`<g>${R.rooms.map((room, r) => `<path class="room use-${esc(room.kind || 'none')}${room.indoor ? '' : ' outdoor'}${sel('room', r)}" fill-rule="evenodd" d="${path(S, room.ring)}${room.holes.map((h) => path(S, h)).join('')}"/>`).join('')}</g>`);
  }

  // Stairs: rising from this storey, and arriving from the one below.
  doc.stairs.forEach((st, i) => {
    if (st.from !== k && st.from + 1 !== k) return;
    const plan = stairPlan(st), arriving = st.from + 1 === k;
    const a = S(plan.foot), b = S(plan.head), ang = Math.atan2(b[1] - a[1], b[0] - a[0]);
    const head = `M${f1(b[0] - 9 * Math.cos(ang - 0.4))} ${f1(b[1] - 9 * Math.sin(ang - 0.4))}L${f1(b[0])} ${f1(b[1])}L${f1(b[0] - 9 * Math.cos(ang + 0.4))} ${f1(b[1] - 9 * Math.sin(ang + 0.4))}`;
    out.push(`<g class="stair${arriving ? ' arriving' : ''}${sel('stair', i)}"><path d="${path(S, plan.ring)}"/>${plan.treads.map(([p, q]) => { const u = S(p), v = S(q); return `<line x1="${f1(u[0])}" y1="${f1(u[1])}" x2="${f1(v[0])}" y2="${f1(v[1])}"/>`; }).join('')}<path class="arrow" d="M${f1(a[0])} ${f1(a[1])}L${f1(b[0])} ${f1(b[1])}${head}"/></g>`);
  });

  // What stands in the rooms, each piece by its symbol; the space it needs when it is in hand or cannot be used.
  if (show.furniture !== false) {
    const found = reading && reading.interior ? reading.interior.pieces : [], pieces = [], spaces = [];
    (doc.furniture || []).forEach((piece, i) => {
      if (piece.storey !== k) return;
      const chosen = selection && selection.type === 'piece' && selection.index === i, bad = found[i] && !found[i].ok;
      const body = symbol(piece).map((m) => {
        const pts = m.points.map((q) => S(pieceAt(piece, q[0], q[1])).map(f1).join(' ')).join('L');
        return `<path class="pm ${m.cls}" d="M${pts}${m.closed ? 'Z' : ''}"/>`;
      }).join('');
      pieces.push(`<g class="piece${piece.obstructs === false ? ' light' : ''}${chosen ? ' sel' : ''}${bad ? ' bad' : ''}">${body}</g>`);
      if (chosen || bad) for (const ring of pieceSpaces(piece)) spaces.push(`<path class="space${bad ? ' bad' : ''}" d="${path(S, ring)}"/>`);
    });
    out.push(`<g>${spaces.join('')}</g><g>${pieces.join('')}</g>`);
    if (ghost && ghost.piece) {
      const body = symbol(ghost.piece).map((m) => `<path class="pm ${m.cls}" d="M${m.points.map((q) => S(pieceAt(ghost.piece, q[0], q[1])).map(f1).join(' ')).join('L')}${m.closed ? 'Z' : ''}"/>`).join('');
      out.push(`<g class="piece ghostpiece">${body}</g>${pieceSpaces(ghost.piece).map((ring) => `<path class="space" d="${path(S, ring)}"/>`).join('')}`);
    }
  }

  // The walls, filled to their thickness; then their openings.
  const faulty = new Set(R ? R.faulty_walls : []);
  out.push(`<g>${s.walls.map((w, i) => {
    const [a, b] = wallEnds(s, i).map(S), open = w.kind === 'open', outer = R && R.walls[i] && R.walls[i].outer;
    return `<line class="wall${open ? ' open' : ''}${outer ? ' outer' : ''}${faulty.has(i) ? ' faulty' : ''}${sel('wall', i)}" stroke-width="${open ? 1 : f1(Math.max(1.6, w.t * px))}" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/>`;
  }).join('')}</g>`);
  const openings = [];
  s.openings.forEach((o, i) => {
    if (!s.walls[o.wall]) return;
    const span = openingSpan(s, o), a = S(span.from), b = S(span.to), t = Math.max(1.6, s.walls[o.wall].t * px);
    const d = [b[0] - a[0], b[1] - a[1]], L = Math.hypot(d[0], d[1]) || 1, n = [-d[1] / L, d[0] / L];
    let mark = `<line class="gap" stroke-width="${f1(t + 1.5)}" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/>`;
    if (o.kind === 'window') {
      for (const off of [-t / 2, 0, t / 2]) mark += `<line class="pane" x1="${f1(a[0] + n[0] * off)}" y1="${f1(a[1] + n[1] * off)}" x2="${f1(b[0] + n[0] * off)}" y2="${f1(b[1] + n[1] * off)}"/>`;
    } else {
      // The leaf swings into the room on the wall's left, or into the building where the wall is an outer wall.
      // unless it is drawn to swing the other way; it hangs at the wall's first end unless drawn at the other.
      const side = R && R.walls[o.wall] ? R.walls[o.wall] : null;
      const intoRight = o.swing ? o.swing < 0 : side && side.outer && !(side.left !== null && R.rooms[side.left] && R.rooms[side.left].indoor);
      const m = intoRight ? [-n[0], -n[1]] : n, pts = [], far = o.hinge === 'b', h = far ? b : a, along = far ? [-d[0] / L, -d[1] / L] : [d[0] / L, d[1] / L];
      // On the screen y runs down, so the wall's left is at −n.
      for (let j = 0; j <= 8; j++) { const th = j / 8 * Math.PI / 2; pts.push([h[0] + L * (Math.sin(th) * along[0] - Math.cos(th) * m[0]), h[1] + L * (Math.sin(th) * along[1] - Math.cos(th) * m[1])]); }
      mark += `<path class="leaf" d="M${f1(h[0])} ${f1(h[1])}L${pts.map((p) => p.map(f1).join(' ')).join('L')}"/>`;
    }
    openings.push(`<g class="opening ${o.kind}${sel('opening', i)}">${mark}</g>`);
  });
  out.push(`<g>${openings.join('')}</g>`);

  // The roof over this storey, as a roof plan shows it: its edge, and its ridges, hips and valleys (ADR 0097).
  if (roofs) for (const rf of roofs) {
    const edge = (rf.outline || []).map((p) => S(p).map(f1).join(',')).join(' ');
    out.push(`<g class="roofplan">${edge ? `<polygon class="roofedge" points="${edge}"/>` : ''}${(rf.lines || []).map((l) => { const a = S(l.a), b = S(l.b); return `<line class="roofline ${['ridge', 'hip', 'valley'][l.kind] || ''}" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/>`; }).join('')}</g>`);
  }

  // The vertices, when one can take hold of them.
  if (show.vertices) out.push(`<g>${s.vertices.map((p, i) => { const q = S(p); return `<circle class="vertex${sel('vertex', i)}" cx="${f1(q[0])}" cy="${f1(q[1])}" r="2.6"/>`; }).join('')}</g>`);

  // The rooms' names and areas.
  if (R && R.sound && show.names !== false) {
    const labels = [];
    R.rooms.forEach((room, r) => {
      const at = room.label !== null && s.labels[room.label] ? s.labels[room.label].at : interiorPoint(room.ring);
      const q = S(at), named = room.name || room.kind, words = named || 'name this room';
      // The name is set smaller where the room is narrow on the page, and the area left out where even that does not fit.
      let x0 = Infinity, x1 = -Infinity;
      for (const c of room.ring) x0 = Math.min(x0, c[0]), x1 = Math.max(x1, c[0]);
      const wide = (x1 - x0) * px - 8;
      if (wide < 26 || px * Math.sqrt(Math.abs(room.area)) < 30) return;
      const small = words.length * 6.5 > wide, tiny = words.length * 5.3 > wide;
      const area = wide > 44 && !tiny ? `<tspan class="roomarea" x="${f1(q[0])}" dy="${small ? 13 : 15}">${f1(room.net_area)} m²</tspan>` : '';
      const failing = marks[r] && marks[r].length ? `<tspan class="failing" x="${f1(q[0])}" dy="${area ? 14 : 13}">✗ ${esc(marks[r].join(' · '))}</tspan>` : '';
      labels.push(`<text class="roomname${named ? '' : ' unnamed'}${small ? ' small' : ''}${tiny ? ' tiny' : ''}" x="${f1(q[0])}" y="${f1(q[1] - 3)}">${esc(words)}${area}${failing}</text>`);
    });
    out.push(`<g>${labels.join('')}</g>`);
  }

  // The length of the wall in hand.
  if (selection && selection.type === 'wall' && s.walls[selection.index]) {
    const [a, b] = wallEnds(s, selection.index), q = S(lerp(a, b, 0.5));
    out.push(`<text class="dimension" x="${f1(q[0])}" y="${f1(q[1] - 9)}">${dist(a, b).toFixed(2)} m</text>`);
  }

  // What a tool is about to make.
  if (ghost) {
    if (ghost.wall) {
      const [a, b] = ghost.wall.map(S), q = [(a[0] + b[0]) / 2, (a[1] + b[1]) / 2];
      out.push(`<line class="ghost" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/><text class="dimension" x="${f1(q[0])}" y="${f1(q[1] - 9)}">${dist(ghost.wall[0], ghost.wall[1]).toFixed(2)} m</text>`);
    }
    if (ghost.ring) {
      const r = ghost.ring;
      out.push(`<path class="ghost" d="${path(S, r)}"/>`);
      if (r.length === 4) {
        const a = S(lerp(r[0], r[1], 0.5)), b = S(lerp(r[1], r[2], 0.5));
        out.push(`<text class="dimension" x="${f1(a[0])}" y="${f1(a[1] + 16)}">${dist(r[0], r[1]).toFixed(2)} m</text><text class="dimension" x="${f1(b[0] + 30)}" y="${f1(b[1])}">${dist(r[1], r[2]).toFixed(2)} m</text>`);
      }
    }
    if (ghost.opening && s.walls[ghost.opening.wall]) {
      const span = openingSpan(s, ghost.opening), a = S(span.from), b = S(span.to);
      out.push(`<line class="ghost opening" stroke-width="${f1(Math.max(3, s.walls[ghost.opening.wall].t * px + 2))}" x1="${f1(a[0])}" y1="${f1(a[1])}" x2="${f1(b[0])}" y2="${f1(b[1])}"/>`);
    }
    if (ghost.lot && ghost.lot.length) {
      const pts = [...ghost.lot, ...(ghost.cursor ? [ghost.cursor] : [])].map(S);
      out.push(`<polyline class="ghost lotghost" points="${pts.map((p) => p.map(f1).join(',')).join(' ')}"/>${ghost.lot.map((p) => { const q = S(p); return `<circle class="ghostpoint" cx="${f1(q[0])}" cy="${f1(q[1])}" r="3.5"/>`; }).join('')}`);
    }
    if (ghost.vertex) { const q = S(ghost.vertex); out.push(`<circle class="ghostpoint" cx="${f1(q[0])}" cy="${f1(q[1])}" r="4.5"/>`); }
    if (ghost.snap) {
      const q = S(ghost.snap.point), r = ghost.snap.kind === 'vertex' ? 6 : 4;
      out.push(ghost.snap.kind === 'grid' || ghost.snap.kind === 'free'
        ? `<path class="snap" d="M${f1(q[0] - 5)} ${f1(q[1])}H${f1(q[0] + 5)}M${f1(q[0])} ${f1(q[1] - 5)}V${f1(q[1] + 5)}"/>`
        : `<circle class="snap ${ghost.snap.kind}" cx="${f1(q[0])}" cy="${f1(q[1])}" r="${r}"/>`);
    }
  }

  // The eye of one who walks through the building: where it stands, and the field it sees.
  if (eye && eye.storey === k) {
    const q = S(eye.at), h = eye.heading * Math.PI / 180, r = Math.max(18, 2.6 * px), half = 33 * Math.PI / 180;
    const ray = (a) => [q[0] + r * Math.cos(a), q[1] - r * Math.sin(a)];
    const fan = [];
    for (let j = 0; j <= 8; j++) fan.push(ray(h - half + 2 * half * j / 8));
    out.push(`<path class="sight" d="M${f1(q[0])} ${f1(q[1])}L${fan.map((p) => p.map(f1).join(' ')).join('L')}Z"/><circle class="eye" cx="${f1(q[0])}" cy="${f1(q[1])}" r="4"/>`);
  }

  // North, and the scale.
  if (show.furnishings === false) return out.join('');
  const bar = niceLength(110 / px), x0 = 18, y0 = size[1] - 18;
  out.push(`<g class="furniture"><path d="M${x0} ${y0 - 5}V${y0}H${f1(x0 + bar * px)}V${y0 - 5}"/><text x="${f1(x0 + bar * px + 7)}" y="${y0}">${bar} m</text>`
    + `<path class="north" d="M${size[0] - 30} 52V22M${size[0] - 30} 22l-5 10M${size[0] - 30} 22l5 10"/><text class="northword" x="${size[0] - 30}" y="66">N</text></g>`);
  return out.join('');
}

// What lies under a ground point p, the nearest kind first: a corner, an opening, a wall, a piece, a stair, an edge of the lot, a
// room. `reach` is how near counts, in metres. Of pieces that lie over one another (a table on a rug), the smallest is taken.
export function hit(doc, reading, k, p, reach, { vertices = true, lot = false, pieces = true } = {}) {
  const s = doc.storeys[k];
  if (vertices) {
    let best = -1, d = reach;
    s.vertices.forEach((v, i) => { const e = dist(p, v); if (e <= d) d = e, best = i; });
    if (best >= 0) return { type: 'vertex', index: best };
  }
  for (let i = 0; i < s.openings.length; i++) {
    const o = s.openings[i];
    if (!s.walls[o.wall]) continue;
    const span = openingSpan(s, o), pr = project(p, span.from, span.to);
    if (pr.distance <= Math.max(reach, s.walls[o.wall].t / 2)) return { type: 'opening', index: i };
  }
  {
    let best = -1, d = Infinity;
    for (let i = 0; i < s.walls.length; i++) {
      const pr = project(p, ...wallEnds(s, i));
      if (pr.distance <= Math.max(reach, s.walls[i].t / 2) && pr.distance < d) d = pr.distance, best = i;
    }
    if (best >= 0) return { type: 'wall', index: best };
  }
  if (pieces) {
    let best = -1, area = Infinity;
    (doc.furniture || []).forEach((piece, i) => { const a = piece.width * piece.depth; if (piece.storey === k && a < area && inPiece(piece, p, piece.width < 0.12 || piece.depth < 0.12 ? reach : 0)) area = a, best = i; });
    if (best >= 0) return { type: 'piece', index: best };
  }
  for (let i = 0; i < doc.stairs.length; i++) {
    const st = doc.stairs[i];
    if ((st.from === k || st.from + 1 === k) && inRing(p, stairPlan(st).ring)) return { type: 'stair', index: i };
  }
  if (lot && doc.site.lot) {
    const ring = doc.site.lot.ring;
    for (let e = 0; e < ring.length; e++) if (project(p, ring[e], ring[(e + 1) % ring.length]).distance <= reach) return { type: 'edge', index: e };
  }
  const R = reading && reading.storeys[k];
  if (R && R.sound) {
    let best = -1, area = Infinity;
    R.rooms.forEach((room, r) => { const a = Math.abs(ringArea(room.ring)); if (a < area && inRing(p, room.ring)) area = a, best = r; });
    if (best >= 0) return { type: 'room', index: best };
  }
  return null;
}
