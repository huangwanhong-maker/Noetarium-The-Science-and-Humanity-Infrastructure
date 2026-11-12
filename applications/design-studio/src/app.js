// The Design Studio: a building drawn by hand on any lot of any street (ADR 0095).
//
// The person draws; the studio tells what the drawing does. The document and every change to it are the visualization layer's
// (visualization/js/plan): a change is an operation, and the document before it is kept, so anything can be undone. After each
// change the server's engine reads the building on the architecture domain: its rooms and their areas, light, air and reach, the
// rules of the scheme chosen one by one, the brief statement by statement, quantities, cost and heat loss. Nothing is generated
// here: the tools on the left make what the hand asks for and nothing more.

import * as P from '/visualization/js/plan/index.js';
import { localFrameAt } from '/visualization/js/gis/frame.js';
import { subsolarPoint, sunAltAz } from '/domains/astronomy/js/index.js';

const $ = (id) => document.getElementById(id);
const esc = (s) => String(s ?? '').replace(/[&<>"]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
const KEEP = 'noetarium.design-studio.building';

// ---- what the page holds -------------------------------------------------------------------------------------------------------

function kept() {
  try { return P.opened(JSON.parse(localStorage.getItem(KEEP))); } catch { return P.exampleHouse(); }
}

const history = new P.History(kept());
const state = {
  reading: null, engine: null, storey: 0, tool: 'select', selection: null, ghost: null, view: null, mode: 'plan', panel: 'rooms',
  wall: 'inner', context: null, contextNote: '', schemes: [], kinds: [], elements: [], projects: [], places: null, found: [],
  show: { surroundings: true, roof: false, whole: false, lamps: true, cut: false }, sun: { day: 172, hour: 12 }, chain: null, lot: null, drag: null, solid: null,
  placing: null, eye: null, street: { reading: null, station: 0, buildings: [], note: '' },
};
const doc = () => history.doc;
const storey = () => doc().storeys[state.storey];
const R = () => (state.reading ? state.reading.storeys[state.storey] : null);

const TOOLS = [
  ['select', 'Select', 'V', 'M6 4l12 9-6 1-2 6z', 'Take hold of a corner, a wall, a door or a window and move it; choose a room to name it.'],
  ['room', 'Room', 'R', 'M4 6h16v12H4z', 'Drag a rectangle: its four walls are drawn, joined to those they meet.'],
  ['wall', 'Wall', 'W', 'M4 18L20 6M4 18v-3M20 6v3', 'Click where a wall starts and where it ends; it goes on from there. Esc ends it.'],
  ['door', 'Door', 'D', 'M5 20V5h9v15M14 20a9 9 0 0 0 6-8', 'Click a wall to open a door in it.'],
  ['window', 'Window', 'N', 'M4 9h16v6H4zM12 9v6', 'Click a wall to set a window in it.'],
  ['stair', 'Stair', 'S', 'M4 19h4v-4h4v-4h4V7h4', 'Click where a stair\'s first riser is; it rises to the storey above.'],
  ['furnish', 'Furnish', 'U', 'M6 11V9a3 3 0 0 1 3-3h6a3 3 0 0 1 3 3v2M4 11h16v6H4zM6 17v2.5M18 17v2.5', 'Choose a piece in the Interior panel, then click where it stands: against a wall it turns its back to it.'],
  ['-'],
  ['lot', 'Lot', 'O', 'M4 7l13-3 3 13-12 3z', 'Click the lot\'s corners in turn; click the first again to close it.'],
  ['erase', 'Erase', 'E', 'M5 17l8-10 6 5-6 8H8zM9 12l6 5', 'Click a wall, a door, a window or a stair to remove it.'],
];
const TABS = [['rooms', 'Rooms'], ['interior', 'Interior'], ['brief', 'Brief'], ['rules', 'Rules'], ['shell', 'Shell'], ['light', 'Light'], ['cost', 'Cost'], ['site', 'Site']];

// ---- words and numbers ---------------------------------------------------------------------------------------------------------

const m = (x) => (Number.isFinite(x) ? `${x.toFixed(2)} m` : '—');
const m2 = (x) => (Number.isFinite(x) ? `${x.toFixed(1)} m²` : '—');
const pct = (x) => (Number.isFinite(x) ? `${(100 * x).toFixed(1)}%` : '—');
const money = (x) => (Number.isFinite(x) ? Math.round(x).toLocaleString('en') : '—');
const inUnit = (unit) => (x) => (unit === 'share' ? pct(x) : unit === 'count' ? String(Math.round(x)) : unit === 'm²' ? m2(x) : m(x));
function asked(f) {
  const u = inUnit(f.unit);
  if (f.least !== null && f.most !== null) return `${u(f.least)} to ${u(f.most)}`;
  return f.least !== null ? `at least ${u(f.least)}` : `at most ${u(f.most)}`;
}
const cap = (s) => (s ? s[0].toUpperCase() + s.slice(1) : s);
const roomName = (room) => room.name || cap(room.kind) || 'A room without a name';
// An operation as the status line names it: by its title, in lower case.
const said = (name) => ((P.OPERATIONS[name] && P.OPERATIONS[name].title) || name).toLowerCase();

let saidTimer = 0;
function say(text, bad = false) {
  const el = $('said');
  el.textContent = text, el.className = bad ? 'bad' : '';
  clearTimeout(saidTimer);
  if (text) saidTimer = setTimeout(() => { el.textContent = ''; }, 7000);
}

// ---- changing the building -----------------------------------------------------------------------------------------------------

function changed() {
  try { localStorage.setItem(KEEP, JSON.stringify(doc())); } catch { /* a full store keeps nothing; the page still works */ }
  if (state.storey >= doc().storeys.length) state.storey = doc().storeys.length - 1;
  renderTop(), renderPlan();
  read().then(() => { if (state.mode === 'street') readStreet(); });
}

function change(name, args, keepSelection = false) {
  try {
    history.do(name, args);
  } catch (error) {
    say(cap(error.message) + '.', true);
    return false;
  }
  if (!keepSelection) state.selection = null;
  changed();
  return true;
}

// The Sun at the site on a day of the year at a local mean solar hour (the Sun is due north or south at 12).
function sunAt(day, hour) {
  const place = doc().site.place;
  if (!place) return null;
  const ms = Date.UTC(2026, 0, 1) + (day - 1) * 86400e3 + (hour - place.lon / 15) * 3600e3;
  return sunAltAz(place.lon, place.lat, subsolarPoint(ms));
}

// What stands and shines about the building, for the engine: the neighbours within 150 m of it as prisms, and the Sun every
// quarter of an hour of the day chosen. No part of the document: the place's.
function about() {
  const out = { buildings: [], suns: [] }, c = around(), pts = extent();
  if (c && pts.length) {
    let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
    for (const q of pts) x0 = Math.min(x0, q[0]), y0 = Math.min(y0, q[1]), x1 = Math.max(x1, q[0]), y1 = Math.max(y1, q[1]);
    for (const b of c.buildings) {
      if (b.ring.length < 3 || !(b.height > 0)) continue;
      const m = P.centroid(b.ring);
      if (m[0] > x0 - 150 && m[0] < x1 + 150 && m[1] > y0 - 150 && m[1] < y1 + 150) out.buildings.push({ ring: b.ring, height: b.height });
    }
  }
  if (doc().site.place) for (let h = 0.125; h < 24; h += 0.25) { const s = sunAt(state.sun.day, h); if (s.altitude > 0) out.suns.push([s.altitude, s.azimuth, 0.25]); }
  return out;
}

// The building read by the engine. One request is in flight at a time: a change made while one is, is read once that one is back,
// however many changes there were, so that a drawing made of many steps asks as often as the engine answers and no oftener.
let flight = null, again = false;
function read() {
  if (flight) { again = true; return flight; }
  state.busy = true;
  flight = (async () => {
    do {
      again = false;
      try {
        const reply = await fetch('/api/read', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ building: doc(), around: about() }) });
        const body = await reply.json();
        if (reply.ok) state.reading = body, state.engine = null;
        else state.reading = null, state.engine = body.message || 'the engine did not answer';
      } catch {
        state.reading = null, state.engine = 'the server did not answer';
      }
    } while (again);
    flight = null, state.busy = false;
    renderAll();
  })();
  return flight;
}

// The drawing grid: turned to the lot's longest edge, from its first corner.
function grid() {
  const lot = doc().site.lot;
  if (!lot) return { angle: 0, origin: [0, 0] };
  let best = 0, length = -1;
  lot.ring.forEach((p, k) => { const L = P.dist(p, lot.ring[(k + 1) % lot.ring.length]); if (L > length) length = L, best = k; });
  const a = lot.ring[best], b = lot.ring[(best + 1) % lot.ring.length];
  let angle = Math.atan2(b[1] - a[1], b[0] - a[0]) % (Math.PI / 2);
  if (angle > Math.PI / 4) angle -= Math.PI / 2;
  if (angle < -Math.PI / 4) angle += Math.PI / 2;
  return { angle, origin: lot.ring[0] };
}

// The surroundings as they are drawn: without the buildings that stand on the lot, which the building drawn replaces.
function around() {
  const lot = doc().site.lot, street = doc().street;
  if (!state.context) return state.context;
  const buildings = lot ? state.context.buildings.filter((b) => !P.inRing(P.centroid(b.ring), lot.ring)) : state.context.buildings;
  return { ...state.context, buildings: P.withHeights(buildings, street && street.heights) };
}

// ---- the plan ------------------------------------------------------------------------------------------------------------------

const planSize = () => { const r = $('plan').getBoundingClientRect(); return [Math.max(50, r.width), Math.max(50, r.height)]; };

// What the plan is fitted to: the building where there is one, with room about it; else the lot.
function extent() {
  const pts = [];
  for (const s of doc().storeys) pts.push(...s.vertices);
  if (!pts.length && doc().site.lot) pts.push(...doc().site.lot.ring);
  return pts;
}
const fitted = () => P.fit(extent(), planSize(), extent().length && doc().storeys.some((s) => s.vertices.length) ? 150 : 80);

function fitView() {
  state.view = fitted();
  renderPlan();
}

// Rules a room fails, as words beside its name.
function marks() {
  const rooms = R() && R().sound ? R().rooms : [], code = doc().code || {};
  return rooms.map((room) => {
    if (!room.habitable) return [];
    const out = [];
    if (code.light && Number.isFinite(code.light.least) && room.light_share < code.light.least - 1e-9) out.push('light');
    if (code.air && Number.isFinite(code.air.least) && room.air_share < code.air.least - 1e-9) out.push('air');
    return out;
  });
}

function renderPlan() {
  if (state.mode !== 'plan') return;
  const svg = $('plan'), size = planSize();
  if (!state.view) state.view = fitted();
  svg.setAttribute('viewBox', `0 0 ${size[0]} ${size[1]}`);
  svg.setAttribute('class', state.tool === 'select' ? 'select' : '');
  svg.innerHTML = P.planSvg({
    doc: doc(), reading: state.reading, storey: state.storey, view: state.view, size, selection: state.selection, ghost: state.ghost,
    context: around(), marks: state.view.scale >= 14 ? marks() : [], show: { vertices: state.tool === 'select' && state.view.scale >= 14, surroundings: state.show.surroundings },
    roofs: state.panel === 'shell' && state.reading && state.reading.shell && state.storey === doc().storeys.length - 1 ? state.reading.shell.roofs : null,
  });
}

const world = (e) => { const r = $('plan').getBoundingClientRect(); return P.toWorld(state.view, [r.width, r.height])([e.clientX - r.left, e.clientY - r.top]); };
const reach = () => 8 / state.view.scale;

function snapped(p, more = {}) {
  const g = grid(), lot = doc().site.lot;
  return P.snap(p, storey(), { reach: reach(), step: 0.05, angle: g.angle, origin: g.origin, extra: lot ? lot.ring : [], ...more });
}

// The wall under the pointer and where along it, for a door or a window.
function wallUnder(p) {
  const s = storey();
  let best = -1, d = Infinity, at = 0;
  for (let w = 0; w < s.walls.length; w++) {
    const [a, b] = P.wallEnds(s, w), pr = P.project(p, a, b);
    if (pr.distance < d && pr.distance <= Math.max(reach() * 1.6, s.walls[w].t)) d = pr.distance, best = w, at = pr.t * P.dist(a, b);
  }
  return best < 0 ? null : { wall: best, at: P.toGrid(at, 0.05) };
}

function openingFor(kind, under) {
  const s = storey(), side = R() && R().walls[under.wall];
  const base = kind === 'window' ? P.DEFAULTS.window : side && side.outer ? P.DEFAULTS.entrance : P.DEFAULTS.door;
  const L = P.wallLength(s, under.wall), width = Math.min(base.width, Math.max(0.4, L - 0.1));
  return { ...base, width, wall: under.wall, kind, at: Math.max(width / 2, Math.min(L - width / 2, under.at)) };
}

function rectangle(a, b) {
  const g = grid(), c = Math.cos(g.angle), s = Math.sin(g.angle), d = P.sub(b, a);
  const u = d[0] * c + d[1] * s, v = -d[0] * s + d[1] * c;
  return [a, P.add(a, [u * c, u * s]), b, P.add(a, [-v * s, v * c])];
}

function pointerDown(e) {
  const svg = $('plan');
  svg.setPointerCapture(e.pointerId);
  const p = world(e);
  if (e.button === 1 || e.button === 2 || e.altKey) { state.drag = { type: 'pan', x: e.clientX, y: e.clientY, cx: state.view.cx, cy: state.view.cy }; svg.classList.add('panning'); return; }
  if (e.button !== 0) return;
  const tool = state.tool;
  if (tool === 'select') {
    const h = P.hit(doc(), state.reading, state.storey, p, reach(), { lot: true });
    state.selection = h;
    if (h && h.type === 'piece' && state.panel !== 'interior') state.panel = 'interior', renderPanel();
    state.drag = h ? { type: h.type, index: h.index, start: p, x: e.clientX, y: e.clientY, moved: false }
      : { type: 'pan', x: e.clientX, y: e.clientY, cx: state.view.cx, cy: state.view.cy };
    if (!h) svg.classList.add('panning');
    renderPlan(), renderInspector();
  } else if (tool === 'room') {
    state.drag = { type: 'room', start: snapped(p).point };
  } else if (tool === 'wall') {
    const end = wallEnd(p);
    if (!state.chain) { state.chain = end; }
    else if (P.dist(state.chain, end) > 0.02) {
      const t = state.wall === 'outer' ? P.DEFAULTS.outer : state.wall === 'open' ? 0 : P.DEFAULTS.inner;
      if (change('addWall', { storey: state.storey, from: state.chain, to: end, t, kind: state.wall === 'open' ? 'open' : 'solid' })) state.chain = end;
    }
  } else if (tool === 'door' || tool === 'window') {
    const under = wallUnder(p);
    if (!under) return say('Click on a wall.');
    const { wall, kind, at, ...rest } = openingFor(tool, under);
    change('addOpening', { storey: state.storey, wall, kind, at, ...rest });
  } else if (tool === 'stair') {
    change('addStair', { from: state.storey, at: snapped(p).point, bearing: grid().angle * 180 / Math.PI + 90 });
  } else if (tool === 'furnish') {
    if (!state.placing) return say('Choose a piece in the Interior panel first.');
    const held = held_(state.placing, p);
    if (change('addPiece', { storey: state.storey, kind: held.kind, at: held.at, bearing: held.bearing })) {
      state.selection = { type: 'piece', index: doc().furniture.length - 1 };
      state.placing = null;
      setTool('select');
      renderAll();
    }
  } else if (tool === 'lot') {
    const q = snapped(p, { step: 0.1 }).point;
    if (!state.lot) state.lot = [];
    if (state.lot.length >= 3 && P.dist(q, state.lot[0]) <= reach() * 1.5) return closeLot();
    state.lot.push(q);
    state.ghost = { lot: state.lot, cursor: q };
    renderPlan();
  } else if (tool === 'erase') {
    const h = P.hit(doc(), state.reading, state.storey, p, reach(), { vertices: false });
    if (h && h.type === 'wall') change('removeWall', { storey: state.storey, wall: h.index });
    else if (h && h.type === 'opening') change('removeOpening', { storey: state.storey, opening: h.index });
    else if (h && h.type === 'stair') change('removeStair', { stair: h.index });
    else if (h && h.type === 'piece') change('removePiece', { piece: h.index });
    else if (h && h.type === 'room' && R().rooms[h.index].label !== null) change('removeLabel', { storey: state.storey, label: R().rooms[h.index].label });
  }
}

// A piece of a kind held at the pointer: against a wall within reach when it is one that stands against walls, else on the grid,
// turned to the grid.
function held_(kind, p, k = state.storey) {
  const piece = P.newPiece(kind, k, p, Math.round(grid().angle * 180 / Math.PI) + 90), c = P.entry(kind);
  const to = P.snapPiece(doc().storeys[k], piece, p, { reach: 0.45, against: Boolean(c && c.wall) });
  return { ...piece, at: to.at, bearing: to.bearing };
}

// A piece of the document moved by the pointer, keeping where it was taken hold of.
function moved_(piece, from, p) {
  const c = P.entry(piece.kind), q = [piece.at[0] + p[0] - from[0], piece.at[1] + p[1] - from[1]];
  const to = P.snapPiece(doc().storeys[piece.storey], piece, q, { reach: 0.3, against: Boolean(c && c.wall) });
  return { ...piece, at: to.at, bearing: to.bearing };
}

// Where a wall being drawn would end: a corner or a wall it snaps to, else the grid, squared to the grid's axes.
function wallEnd(p) {
  const s = snapped(p);
  if (!state.chain || s.kind === 'vertex' || s.kind === 'wall') return s.point;
  const g = grid();
  return snapped(P.squared(state.chain, s.point, g.angle)).point;
}

function pointerMove(e) {
  if (!state.view) return;
  const p = world(e), d = state.drag;
  $('where').textContent = `${p[0].toFixed(2)} E  ${p[1].toFixed(2)} N`;
  if (d && d.type === 'pan') {
    state.view = { ...state.view, cx: d.cx - (e.clientX - d.x) / state.view.scale, cy: d.cy + (e.clientY - d.y) / state.view.scale };
    return renderPlan();
  }
  const tool = state.tool, s = storey();
  if (tool === 'select' && d) {
    if (Math.hypot(e.clientX - d.x, e.clientY - d.y) > 3) d.moved = true;
    if (!d.moved) return;
    if (d.type === 'vertex') { d.to = snapped(p).point; state.ghost = { vertex: d.to }; }
    else if (d.type === 'wall' && s.walls[d.index]) {
      const [a, b] = P.wallEnds(s, d.index), L = P.dist(a, b) || 1, n = [-(b[1] - a[1]) / L, (b[0] - a[0]) / L];
      const far = P.toGrid(P.dot(P.sub(p, d.start), n), 0.05);
      d.by = P.scale(n, far);
      state.ghost = { wall: [P.add(a, d.by), P.add(b, d.by)] };
    } else if (d.type === 'opening' && s.openings[d.index]) {
      const o = s.openings[d.index], [a, b] = P.wallEnds(s, o.wall), L = P.dist(a, b);
      d.at = Math.max(o.width / 2, Math.min(L - o.width / 2, P.toGrid(P.project(p, a, b).t * L, 0.05)));
      state.ghost = { opening: { ...o, at: d.at } };
    } else if (d.type === 'stair') { d.to = snapped(p).point; state.ghost = { vertex: d.to }; }
    else if (d.type === 'piece' && doc().furniture[d.index]) { d.piece = moved_(doc().furniture[d.index], d.start, p); state.ghost = { piece: d.piece }; }
    else if (d.type === 'room' && R() && R().rooms[d.index] && R().rooms[d.index].label !== null) { d.to = p; state.ghost = { vertex: p }; }
    return renderPlan();
  }
  if (tool === 'room') state.ghost = d && d.type === 'room' ? { ring: rectangle(d.start, snapped(p).point), snap: snapped(p) } : { snap: snapped(p) };
  else if (tool === 'wall') { const end = wallEnd(p); state.ghost = state.chain ? { wall: [state.chain, end], snap: { point: end, kind: snapped(p).kind } } : { snap: snapped(p) }; }
  else if (tool === 'door' || tool === 'window') { const under = wallUnder(p); state.ghost = under ? { opening: openingFor(tool, under) } : null; }
  else if (tool === 'stair') state.ghost = { snap: snapped(p) };
  else if (tool === 'furnish') state.ghost = state.placing ? { piece: held_(state.placing, p) } : null;
  else if (tool === 'lot') { const q = snapped(p, { step: 0.1 }); state.ghost = { lot: state.lot || [], cursor: q.point, snap: q }; }
  else return;
  renderPlan();
}

function pointerUp(e) {
  const d = state.drag;
  state.drag = null;
  $('plan').classList.remove('panning');
  if (!d) return;
  const k = state.storey;
  if (d.type === 'room') {
    const ring = rectangle(d.start, snapped(world(e)).point);
    state.ghost = null;
    if (P.dist(ring[0], ring[1]) > 0.3 && P.dist(ring[1], ring[2]) > 0.3) change('addRoom', { storey: k, ring, t: P.DEFAULTS.outer });
    else renderPlan();
    return;
  }
  if (state.tool !== 'select' || !d.moved) return;
  state.ghost = null;
  if (d.type === 'vertex' && d.to) change('moveVertex', { storey: k, vertex: d.index, to: d.to });
  else if (d.type === 'wall' && d.by && Math.hypot(...d.by) > 1e-6) change('moveWall', { storey: k, wall: d.index, by: d.by });
  else if (d.type === 'opening' && Number.isFinite(d.at)) change('setOpening', { storey: k, opening: d.index, at: d.at }, true);
  else if (d.type === 'stair' && d.to) change('setStair', { stair: d.index, at: d.to }, true);
  else if (d.type === 'piece' && d.piece) change('setPiece', { piece: d.index, at: d.piece.at, bearing: d.piece.bearing }, true);
  else if (d.type === 'room' && d.to) change('setLabel', { storey: k, label: R().rooms[d.index].label, at: d.to }, true);
  else renderPlan();
}

function wheel(e) {
  e.preventDefault();
  const before = world(e), factor = Math.exp(-e.deltaY * 0.0014);
  const scale = Math.max(1.5, Math.min(400, state.view.scale * factor));
  state.view = { ...state.view, scale };
  const after = world(e);
  state.view.cx += before[0] - after[0], state.view.cy += before[1] - after[1];
  renderPlan();
}

// ---- the lot and its surroundings ----------------------------------------------------------------------------------------------

function facingStreet(ring) {
  const ways = state.context ? state.context.ways.filter((w) => !w.people && !w.rail) : [];
  return ring.map((p, e) => {
    const q = ring[(e + 1) % ring.length], mid = P.lerp(p, q, 0.5);
    return ways.some((w) => { for (let i = 0; i + 1 < w.line.length; i++) if (P.project(mid, w.line[i], w.line[i + 1]).distance <= w.width / 2 + 6) return true; return false; });
  });
}

function closeLot() {
  const ring = state.lot;
  state.lot = null, state.ghost = null;
  if (!ring || ring.length < 3) return renderPlan();
  if (change('setLot', { ring, street: facingStreet(ring) })) {
    setTool('select');
    state.panel = 'site';
    say(doc().site.lot.street.some(Boolean) ? 'The lot is marked; the edges beside a way are taken to face a street.' : 'The lot is marked. In the Site panel, say which edges face a street.');
  }
}

const tileOf = (lat, lon, z) => [Math.floor((lon + 180) / 360 * 2 ** z), Math.floor((1 - Math.asinh(Math.tan(lat * Math.PI / 180)) / Math.PI) / 2 * 2 ** z)];

// The buildings and ways within `radius` metres of the place, in the drawing's frame: metres east and north of it.
async function surroundings(radius = 320) {
  const place = doc().site.place;
  state.context = null, state.contextNote = '';
  if (!place) return;
  state.contextNote = 'Reading the surroundings…';
  renderPanel();
  const frame = localFrameAt(place.lat, place.lon), z = 14, tiles = new Map();
  for (const [dx, dy] of [[0, 0], [radius, 0], [-radius, 0], [0, radius], [0, -radius]]) {
    const g = frame.toGeodetic(dx, dy), t = tileOf(g.lat, g.lon, z);
    tiles.set(t.join('/'), t);
  }
  const context = { buildings: [], ways: [], credit: '' };
  const local = (flat, closed = false) => {
    const out = [];
    for (let i = 0; i + 1 < flat.length; i += 2) { const q = frame.toLocal(flat[i + 1], flat[i]); out.push([q[0], q[1]]); }
    // A ring that repeats its first corner at its end is given without the repeat.
    if (closed && out.length > 3 && P.dist(out[0], out[out.length - 1]) < 1e-6) out.pop();
    return out;
  };
  const near = (line) => line.some((p) => Math.hypot(p[0], p[1]) <= radius);
  try {
    for (const [x, y] of tiles.values()) {
      const reply = await fetch(`/world/city/${z}/${x}/${y}`);
      const tile = await reply.json();
      if (!reply.ok) throw new Error(tile.message || 'the surroundings could not be read');
      context.credit = tile.credit || context.credit;
      for (const b of tile.buildings) {
        const ring = local(b[3], true);
        if (ring.length >= 3 && near(ring)) context.buildings.push({ ring, height: b[0] > 0 ? b[0] : 6, presumed: !(b[0] > 0), name: b[5] || null });
      }
      for (const w of tile.ways) {
        const line = local(w[3]);
        if (line.length >= 2 && near(line)) context.ways.push({ line, width: w[1], name: w[2], kind: w[0], rail: w[4] === 1, people: ['footway', 'path', 'steps', 'pedestrian', 'cycleway', 'corridor'].includes(w[0]) });
      }
    }
    state.context = context;
    state.contextNote = `${context.buildings.length} buildings and ${context.ways.length} ways within ${radius} m. ${context.credit}`;
  } catch (error) {
    state.contextNote = `${cap(error.message)}. The lot can still be drawn without them.`;
  }
  renderPlan(), renderPanel(), renderSolid();
  read().then(() => { if (state.mode === 'street') readStreet(); });   // the neighbours are part of what the engine is asked
}

async function findPlace(text) {
  const pair = text.replace(/−/g, '-').match(/^\s*(-?\d+(?:\.\d+)?)\s*[,\s]\s*(-?\d+(?:\.\d+)?)\s*$/);
  if (pair) return goTo('', Number(pair[1]), Number(pair[2]));
  if (!state.places) {
    try { state.places = (await (await fetch('/data/products/places/earth.json')).json()).places; } catch { state.places = []; }
  }
  const want = text.trim().toLowerCase();
  state.found = want.length < 2 ? [] : state.places.filter((row) => row[0].toLowerCase().includes(want) && row[3] !== 'Country').slice(0, 7);
  if (!state.found.length) say(state.places.length ? 'No place of that name is in the gazetteer: give a latitude and a longitude, as -37.80, 144.97.' : 'The gazetteer is not here: give a latitude and a longitude, as -37.80, 144.97.');
  renderPanel();
}

function goTo(name, lat, lon) {
  state.found = [];
  if (change('setPlace', { name, lat, lon })) { surroundings().then(fitSite); }
}

function fitSite() {
  if (doc().site.lot || storey().vertices.length) return;
  state.view = { cx: 0, cy: 0, scale: 3.2 };
  renderPlan();
}

// ---- the top, the tools, the status --------------------------------------------------------------------------------------------

function setTool(tool) {
  state.tool = tool, state.chain = null, state.lot = null, state.ghost = null, state.drag = null;
  if (tool !== 'furnish') state.placing = null;
  else if (state.panel !== 'interior') state.panel = 'interior', renderPanel();
  renderTools(), renderPlan();
}

function renderTools() {
  $('tools').innerHTML = TOOLS.map(([id, name, key, d]) => (id === '-' ? '<div class="gapline"></div>'
    : `<button data-tool="${id}" class="${state.tool === id ? 'on' : ''}" title="${name} (${key})"><svg viewBox="0 0 24 24"><path d="${d}"/></svg>${name}</button>`)).join('');
  const tool = TOOLS.find((t) => t[0] === state.tool);
  $('hint').textContent = tool ? tool[4] : '';
  $('options').innerHTML = state.tool === 'wall'
    ? `<div class="seg">${[['inner', 'inner, 110'], ['outer', 'outer, 250'], ['open', 'open']].map(([id, name]) => `<button data-wall="${id}" class="${state.wall === id ? 'on' : ''}">${name}</button>`).join('')}</div>`
    : '';
}

function renderTop() {
  if (document.activeElement !== $('name')) $('name').value = doc().name || '';
  if (state.solid && walking()) renderMini();
  $('storeys').innerHTML = doc().storeys.map((s, k) => `<button data-storey="${k}" class="${k === state.storey ? 'on' : ''}">${esc(s.name || `Storey ${k + 1}`)}</button>`).join('')
    + '<button class="add" data-storey="add" title="Add a storey above, on the outer walls of the one below">+ storey</button>';
  $('undo').disabled = !history.past.length, $('redo').disabled = !history.future.length;
  $('undo').title = history.past.length ? `Undo: ${said(history.past[history.past.length - 1].name)} (Ctrl Z)` : 'Nothing to undo';
  for (const b of $('mode').children) b.classList.toggle('on', b.dataset.mode === state.mode);
}

function renderNotice() {
  const el = $('notice');
  if (!state.engine) { el.hidden = true; return; }
  el.hidden = false;
  el.innerHTML = `${esc(cap(state.engine))}. <button class="key" data-act="build">Build the engine</button>`;
}

// ---- the inspector: what is in hand --------------------------------------------------------------------------------------------

const field = (label, act, value, more = '') => `<label>${label}</label><input type="number" data-act="${act}" value="${value}" ${more}>`;

function renderInspector() {
  const el = $('inspector'), sel = state.selection, s = storey(), reading = R();
  const k = state.storey;
  if (sel && sel.type === 'wall' && s.walls[sel.index]) {
    const w = s.walls[sel.index], side = reading && reading.walls[sel.index];
    const name = (r) => (r === null || r === undefined ? 'the outside' : roomName(reading.rooms[r]));
    el.innerHTML = `<h2>A wall of ${m(P.wallLength(s, sel.index))}</h2>
      <div class="row">${field('thickness, m', 'wall-t', w.t, 'step="0.01" min="0"')}
        <div class="seg"><button data-act="wall-kind" data-v="solid" class="${w.kind !== 'open' ? 'on' : ''}">solid</button><button data-act="wall-kind" data-v="open" class="${w.kind === 'open' ? 'on' : ''}">open</button></div></div>
      <p class="facts">${side && reading.sound ? (side.divides_nothing ? `It divides nothing: ${name(side.left)} is on both its sides.` : `${side.outer ? 'An outer wall' : 'A wall'} between ${name(side.left)} and ${name(side.right)}.`) : ''}</p>
      <button class="plain" data-act="remove">Remove it</button>`;
  } else if (sel && sel.type === 'opening' && s.openings[sel.index]) {
    const o = s.openings[sel.index];
    el.innerHTML = `<h2>A ${o.kind} of ${m(o.width)}</h2>
      <div class="row">${field('width', 'o-width', o.width, 'step="0.01" min="0.3"')}${field('height', 'o-height', o.height, 'step="0.01" min="0.3"')}${field('sill', 'o-sill', o.sill, 'step="0.05" min="0"')}</div>
      <div class="row">${field('glass, %', 'o-glazed', Math.round(o.glazed * 100), 'step="5" min="0" max="100"')}${field('opens, %', 'o-openable', Math.round(o.openable * 100), 'step="5" min="0" max="100"')}</div>
      ${o.kind === 'door' ? '<p class="facts"><button class="plain" data-act="o-swing">Swing to the other side</button> · <button class="plain" data-act="o-hinge">Hang at the other end</button></p>' : ''}
      <button class="plain" data-act="remove">Remove it</button>`;
  } else if (sel && sel.type === 'piece' && doc().furniture[sel.index]) {
    const piece = doc().furniture[sel.index], c = P.entry(piece.kind), found = state.reading && state.reading.interior ? state.reading.interior.pieces[sel.index] : null;
    el.innerHTML = `<h2>${esc(piece.name || (c ? c.title : piece.kind))}</h2>
      <div class="row">${field('wide', 'p-width', piece.width, 'step="0.05" min="0.1"')}${field('deep', 'p-depth', piece.depth, 'step="0.05" min="0.02"')}${field('high', 'p-height', piece.height, 'step="0.05" min="0.01"')}</div>
      <div class="row"><label>clear floor: before</label><input type="number" data-act="p-front" value="${piece.front}" step="0.05" min="0"><label>sides</label><input type="number" data-act="p-side" value="${piece.side}" step="0.05" min="0"></div>
      <div class="row"><label>or all round</label><input type="number" data-act="p-around" value="${piece.around}" step="0.05" min="0"><label>m</label></div>
      ${piece.finish ? `<div class="chips swatches">${Object.entries(P.FINISHES).map(([name, colour]) => `<button data-act="p-finish" data-v="${name}" class="${piece.finish === name ? 'on' : ''}" title="${name}"><i style="background:${colour}"></i>${name}</button>`).join('')}</div>` : ''}
      <p class="facts"><button class="plain" data-act="p-turn" data-v="90">Turn it left</button> · <button class="plain" data-act="p-turn" data-v="-90">right</button> · <button class="plain" data-act="p-turn" data-v="15">a little</button>
      ${found ? `<br>${pieceFound(sel.index).map((line) => esc(line)).join('<br>')}` : ''}</p>
      <p class="facts">${piece.model ? `Drawn in the round by a model of your own, ${esc(piece.model.name || 'a file')}. <button class="plain quiet" data-act="p-nomodel">Use the catalogue's again</button>`
        : '<button class="plain quiet" data-act="p-model">Give it a model of your own…</button>'}</p>
      <button class="plain" data-act="remove">Take it away</button>`;
  } else if (sel && sel.type === 'room' && reading && reading.sound && reading.rooms[sel.index]) {
    const room = reading.rooms[sel.index];
    const through = room.through.map((t) => roomName(state.reading.storeys[t.storey].rooms[t.room])).join(', ');
    el.innerHTML = `<h2>${esc(roomName(room))}</h2>
      <div class="row"><label>name</label><input type="text" class="grow" data-act="room-name" value="${esc(room.name)}" placeholder="its name"></div>
      <div class="chips">${state.kinds.map((u) => `<button data-act="room-kind" data-v="${u.kind}" class="${room.kind === u.kind ? 'on' : ''}">${u.kind}</button>`).join('')}</div>
      <p class="facts">${m2(room.net_area)} inside its walls${room.net_exact ? '' : ' (to the first order)'}; ${m2(room.area)} to their centrelines${Number.isFinite(room.width) ? `; it holds a circle of ${m(room.width)}` : ''}.<br>
      ${room.indoor ? `Glass ${pct(room.light_share)} of its floor, openable ${pct(room.air_share)}.<br>` : ''}
      ${room.depth === null ? 'It is not reached from the outside.' : `${room.depth} ${room.depth === 1 ? 'step' : 'steps'} from the outside${through ? `, only through ${esc(through)}` : ''}.`}</p>`;
  } else if (sel && sel.type === 'stair' && doc().stairs[sel.index]) {
    const st = doc().stairs[sel.index], rise = doc().storeys[st.from].height / st.risers;
    el.innerHTML = `<h2>A stair of ${st.risers} risers</h2>
      <div class="row">${field('risers', 'st-risers', st.risers, 'step="1" min="2"')}${field('going, m', 'st-going', st.going, 'step="0.005" min="0.1"')}${field('width, m', 'st-width', st.width, 'step="0.05" min="0.5"')}</div>
      <div class="row">${field('turned, °', 'st-bearing', Math.round(st.bearing), 'step="15"')}</div>
      <p class="facts">Each riser ${(rise * 1000).toFixed(0)} mm; twice the riser plus the going ${((2 * rise + st.going) * 1000).toFixed(0)} mm; it runs ${m((st.risers - 1) * st.going)}.</p>
      <button class="plain" data-act="remove">Remove it</button>`;
  } else if (sel && sel.type === 'edge' && doc().site.lot) {
    const lot = doc().site.lot, a = lot.ring[sel.index], b = lot.ring[(sel.index + 1) % lot.ring.length];
    el.innerHTML = `<h2>An edge of the lot, ${m(P.dist(a, b))}</h2>
      <div class="chips"><button data-act="edge-street" data-v="1" class="${lot.street[sel.index] ? 'on' : ''}">faces a street</button><button data-act="edge-street" data-v="0" class="${lot.street[sel.index] ? '' : 'on'}">a side or the rear</button></div>`;
  } else if (sel && sel.type === 'vertex' && s.vertices[sel.index]) {
    const v = s.vertices[sel.index];
    el.innerHTML = `<h2>A corner</h2><p class="facts">${v[0].toFixed(2)} m east, ${v[1].toFixed(2)} m north. Drag it to move the walls that meet there.</p>`;
  } else {
    el.innerHTML = `<h2>${esc(s.name || `Storey ${k + 1}`)}</h2>
      <div class="row"><label>name</label><input type="text" class="grow" data-act="s-name" value="${esc(s.name)}"></div>
      <div class="row">${field('floor to floor, m', 's-height', s.height, 'step="0.05" min="2"')}${field('ceiling, m', 's-ceiling', s.ceiling, 'step="0.05" min="2"')}</div>
      <p class="facts">${reading && reading.sound ? `${reading.rooms.length} ${reading.rooms.length === 1 ? 'room' : 'rooms'}, ${m2(reading.internal_area)} inside the walls.` : ''}
      ${doc().storeys.length > 1 ? '<button class="plain quiet" data-act="remove-storey">Remove this storey</button>' : ''}</p>`;
  }
}

function inspectorAct(act, value, el) {
  const sel = state.selection, k = state.storey, num = Number(value);
  const keep = (name, args) => change(name, args, true);
  if (act === 'wall-t') keep('setWall', { storey: k, wall: sel.index, t: num });
  else if (act === 'wall-kind') keep('setWall', { storey: k, wall: sel.index, kind: el.dataset.v, ...(el.dataset.v === 'open' ? { t: 0 } : storey().walls[sel.index].t === 0 ? { t: P.DEFAULTS.inner } : {}) });
  else if (act === 'o-width') keep('setOpening', { storey: k, opening: sel.index, width: num });
  else if (act === 'o-height') keep('setOpening', { storey: k, opening: sel.index, height: num });
  else if (act === 'o-sill') keep('setOpening', { storey: k, opening: sel.index, sill: num });
  else if (act === 'o-glazed') keep('setOpening', { storey: k, opening: sel.index, glazed: num / 100 });
  else if (act === 'o-openable') keep('setOpening', { storey: k, opening: sel.index, openable: num / 100 });
  else if (act === 'o-swing') { const o = storey().openings[sel.index], side = R() && R().walls[o.wall], now = o.swing || (side && side.outer && !(side.left !== null && R().rooms[side.left].indoor) ? -1 : 1); keep('setOpening', { storey: k, opening: sel.index, swing: -now }); }
  else if (act === 'o-hinge') keep('setOpening', { storey: k, opening: sel.index, hinge: storey().openings[sel.index].hinge === 'b' ? 'a' : 'b' });
  else if (act.startsWith('p-')) {
    const key = act.slice(2), piece = doc().furniture[sel.index];
    if (key === 'model') $('pickmodel').click();
    else if (key === 'nomodel') keep('setPiece', { piece: sel.index, model: undefined });
    else if (key === 'turn') keep('setPiece', { piece: sel.index, bearing: ((piece.bearing + Number(el.dataset.v) + 540) % 360) - 180 });
    else if (key === 'finish') keep('setPiece', { piece: sel.index, finish: el.dataset.v });
    else keep('setPiece', { piece: sel.index, [key]: num });
  }
  else if (act === 'room-name' || act === 'room-kind') {
    const room = R().rooms[sel.index], what = act === 'room-name' ? { name: value } : { kind: el.dataset.v };
    if (room.label === null) keep('setLabel', { storey: k, at: P.interiorPoint(room.ring), kind: room.kind, name: room.name, ...what });
    else keep('setLabel', { storey: k, label: room.label, ...what });
  } else if (act === 'st-risers') keep('setStair', { stair: sel.index, risers: Math.round(num) });
  else if (act === 'st-going') keep('setStair', { stair: sel.index, going: num });
  else if (act === 'st-width') keep('setStair', { stair: sel.index, width: num });
  else if (act === 'st-bearing') keep('setStair', { stair: sel.index, bearing: num });
  else if (act === 'edge-street') keep('setStreet', { edge: sel.index, street: el.dataset.v === '1' });
  else if (act === 's-name') keep('setStorey', { storey: k, name: value });
  else if (act === 's-height') keep('setStorey', { storey: k, height: num });
  else if (act === 's-ceiling') keep('setStorey', { storey: k, ceiling: num });
  else if (act === 'remove-storey') { if (change('removeStorey', { storey: k })) state.storey = Math.max(0, k - 1), renderAll(); }
  else if (act === 'remove') removeSelection();
}

function removeSelection() {
  const sel = state.selection, k = state.storey;
  if (!sel) return;
  if (sel.type === 'wall') change('removeWall', { storey: k, wall: sel.index });
  else if (sel.type === 'opening') change('removeOpening', { storey: k, opening: sel.index });
  else if (sel.type === 'stair') change('removeStair', { stair: sel.index });
  else if (sel.type === 'piece') change('removePiece', { piece: sel.index });
  else if (sel.type === 'room' && R() && R().rooms[sel.index] && R().rooms[sel.index].label !== null) change('removeLabel', { storey: k, label: R().rooms[sel.index].label });
}

// ---- what stands in the rooms ----------------------------------------------------------------------------------------------------

const pieceTitle = (i) => { const piece = doc().furniture[i], c = P.entry(piece.kind); return piece.name || (c ? c.title : piece.kind); };

// What is found of a piece, in sentences; one sentence when all is well.
function pieceFound(i) {
  const I = state.reading && state.reading.interior, f = I && I.pieces[i];
  if (!f) return [];
  const names = (list) => list.map((j) => pieceTitle(j).toLowerCase()).join(', '), out = [];
  if (f.room === null) return ['It stands in no room.'];
  if (!f.within) out.push('It does not lie within its room: it crosses a wall.');
  if (f.overlaps.length) out.push(`It overlaps the ${names(f.overlaps)}.`);
  if (!f.space_within) out.push('The clear floor it needs runs into a wall.');
  if (f.blocked_by.length) out.push(`The ${names(f.blocked_by)} ${f.blocked_by.length > 1 ? 'stand' : 'stands'} in the clear floor it needs.`);
  if (f.judged && !f.reached) out.push(`It cannot be reached from the room's door: less than ${m(I.passage)} is left to pass.`);
  return out.length ? out : [f.judged ? 'It stands well: within its room, its clear floor free, and reached from the door.' : 'It stands well.'];
}

// A piece of the catalogue as a small drawing: its symbol, fitted to a square.
function icon(c) {
  const piece = P.newPiece(c.kind, 0, [0, 0], 90), size = 46, k = (size - 8) / Math.max(piece.width, piece.depth + (c.kind === 'desk' ? 0.5 : 0), 0.6);
  const body = P.symbol(piece).map((mark) => `<path class="pm ${mark.cls}" d="M${mark.points.map((q) => `${(size / 2 + q[0] * k).toFixed(1)} ${(size / 2 + q[1] * k - (c.kind === 'desk' ? 0.25 * k : 0)).toFixed(1)}`).join('L')}${mark.closed ? 'Z' : ''}"/>`).join('');
  return `<svg viewBox="0 0 ${size} ${size}" class="piece">${body}</svg>`;
}

function panelInterior() {
  const I = state.reading && state.reading.interior, pieces = doc().furniture || [];
  let out = '';
  if (I) {
    const bad = pieces.map((_, i) => i).filter((i) => I.pieces[i] && !I.pieces[i].ok), doors = I.doors.filter((d) => d.swing_blocked_by.length || !d.entered);
    out += '<h3>What stands in the rooms</h3>';
    out += pieces.length ? `<p>${pieces.length} ${pieces.length === 1 ? 'piece' : 'pieces'}${bad.length ? `, of which ${bad.length} cannot be used as ${bad.length === 1 ? 'it stands' : 'they stand'}` : ': each within its room, usable and reached'}.</p>`
      : '<p class="note">Nothing stands in the rooms yet. Choose a piece below, then click where it goes.</p>';
    out += bad.map((i) => `<div class="finding bad pick" data-piece="${i}"><span class="glyph">✗</span><span class="what"><b>${esc(pieceTitle(i))}</b>${roomOf(i)}</span>${pieceFound(i).map((line) => `<span class="remark">${esc(line)}</span>`).join('')}</div>`).join('');
    out += doors.map((d) => {
      const o = doc().storeys[d.storey].openings[d.opening], S = state.reading.storeys[d.storey], side = S.walls[o.wall], room = side.left !== null ? side.left : side.right;
      return `<div class="finding bad"><span class="glyph">✗</span><span class="what"><b>A door</b> · ${esc(room !== null ? roomName(S.rooms[room]) : 'outside')}</span>
        ${d.swing_blocked_by.length ? `<span class="remark">The ${esc(d.swing_blocked_by.map((j) => pieceTitle(j).toLowerCase()).join(', '))} ${d.swing_blocked_by.length > 1 ? 'stand' : 'stands'} where its leaf swings.</span>` : ''}
        ${d.entered ? '' : '<span class="remark">No one can stand behind it: the floor there is taken.</span>'}</div>`;
    }).join('');
    out += `<div class="row"><label class="grow">A person needs, to pass</label><input type="number" data-act="passage" value="${(doc().interior && doc().interior.passage) || 0.6}" min="0.3" max="1.5" step="0.05"><label>m</label></div>`;
  } else out += '<p class="note">The building is not read yet.</p>';
  out += `<h3>The catalogue</h3><p class="note small">${state.placing ? `Click on the plan where the ${esc(P.entry(state.placing).title.toLowerCase())} goes; Esc lets it go.` : 'Choose a piece, then click where it stands. Against a wall it turns its back to it.'}</p>`;
  for (const [id, title] of P.GROUPS) {
    out += `<div class="group">${esc(title)}</div><div class="catalogue">${P.CATALOGUE.filter((c) => c.group === id).map((c) => `<button data-kind="${c.kind}" class="${state.placing === c.kind ? 'on' : ''}" title="${esc(c.title)}, ${c.width} × ${c.depth} m">${icon(c)}<span>${esc(c.title)}</span></button>`).join('')}</div>`;
  }
  if (I && I.rooms.length) {
    out += `<h3>Floor left free</h3><table><tbody>${I.rooms.filter((r) => state.reading.storeys[r.storey].rooms[r.room].indoor).map((r) => `<tr><td>${esc(roomName(state.reading.storeys[r.storey].rooms[r.room]))}</td><td>${pct(r.free_share)}</td></tr>`).join('')}</tbody></table>`;
  }
  out += '<p class="note small">The sizes and the clear floor each piece needs are the catalogue\'s, not a standard\'s: change them on the piece. A piece is reached when a person of the width above can come from a door of its room to the floor it needs, found on a grid of 5 cm.</p>';
  return out;
}

function roomOf(i) {
  const f = state.reading.interior.pieces[i], piece = doc().furniture[i];
  return f.room === null ? '' : ` · ${esc(roomName(state.reading.storeys[piece.storey].rooms[f.room]))}`;
}

// ---- the panels ----------------------------------------------------------------------------------------------------------------

function findingRow(f) {
  const u = inUnit(f.unit);
  return `<div class="finding${f.ok ? '' : ' bad'}"><span class="glyph">${f.ok ? '✓' : '✗'}</span><span class="what"><b>${esc(f.title)}</b> · ${esc(f.subject)}${f.clause ? `<span class="clause">${esc(f.clause)}</span>` : ''}</span>
    <span class="numbers">asked ${asked(f)} · found <em>${u(f.actual)}</em></span>${f.note ? `<span class="remark">${esc(f.note)}</span>` : ''}</div>`;
}

function failures() {
  const r = state.reading;
  if (!r) return { rules: 0, brief: 0, interior: 0 };
  const interior = r.interior ? r.interior.pieces.filter((f) => !f.ok).length + r.interior.doors.filter((d) => d.swing_blocked_by.length || !d.entered).length : 0;
  return { rules: r.habitability.findings.filter((f) => !f.ok).length + r.siting.findings.filter((f) => !f.ok).length, brief: r.brief.filter((s) => !s.ok).length, interior };
}

function renderTabs() {
  const bad = failures();
  $('tabs').innerHTML = TABS.map(([id, name]) => {
    const n = id === 'rules' ? bad.rules : id === 'brief' ? bad.brief : id === 'interior' ? bad.interior : 0;
    return `<button data-tab="${id}" class="${state.panel === id ? 'on' : ''}">${name}${n ? `<span class="count">${n}</span>` : ''}</button>`;
  }).join('');
}

function panelRooms() {
  const r = state.reading;
  if (!r) return '<p class="note">The building is not read yet.</p>';
  const code = doc().code || {}, least = (l) => (l && Number.isFinite(l.least) ? l.least : null);
  const light = least(code.light), air = least(code.air);
  let out = '';
  r.storeys.forEach((S, k) => {
    out += `<h3>${esc(doc().storeys[k].name || `Storey ${k + 1}`)}</h3>`;
    if (!S.sound) { out += `<ul class="plainlist">${S.faults.map((f) => `<li>${esc(cap(f))}.</li>`).join('')}</ul><p class="note">The walls in rose are not a plan yet: undo, or move them apart.</p>`; return; }
    if (!S.rooms.length) { out += '<p class="note">No room is enclosed yet. Drag a rectangle with the Room tool, or close a ring of walls.</p>'; return; }
    out += `<table><thead><tr><th>room</th><th>inside</th><th>width</th><th>glass</th><th>opens</th><th>steps</th></tr></thead><tbody>${S.rooms.map((room, i) => `
      <tr class="pick${state.selection && state.selection.type === 'room' && state.selection.index === i && k === state.storey ? ' on' : ''}" data-room="${k}:${i}">
      <td>${esc(roomName(room))}<br><small>${esc(room.kind || 'no use given')}</small></td><td>${m2(room.net_area)}</td><td>${Number.isFinite(room.width) ? room.width.toFixed(2) : '—'}</td>
      <td class="${room.habitable && light !== null && room.light_share < light - 1e-9 ? 'bad' : ''}">${room.indoor ? pct(room.light_share) : '—'}</td>
      <td class="${room.habitable && air !== null && room.air_share < air - 1e-9 ? 'bad' : ''}">${room.indoor ? pct(room.air_share) : '—'}</td>
      <td class="${room.depth === null ? 'bad' : ''}">${room.depth === null ? 'none' : room.depth}</td></tr>`).join('')}</tbody>
      <tfoot><tr><td>the storey</td><td>${m2(S.internal_area)}</td><td colspan="4"><small>${m2(S.floor_area)} to the walls' centrelines</small></td></tr></tfoot></table>`;
    if (S.remarks.length) out += `<ul class="plainlist">${S.remarks.map((f) => `<li>${esc(cap(f))}.</li>`).join('')}</ul>`;
  });
  if (r.remarks.length) out += `<ul class="plainlist">${r.remarks.map((f) => `<li>${esc(cap(f))}.</li>`).join('')}</ul>`;
  out += `<p class="note small">Inside: within the walls' faces. Width: the widest circle the room holds; for a rectangle, its lesser side. Glass and opens: the glazed and the openable area on its outer walls, as shares of its floor. Steps: doors passed from the outside.</p>`;
  return out;
}

function panelBrief() {
  const r = state.reading, brief = doc().brief || { needs: [], relations: [] };
  const kinds = state.kinds.map((u) => u.kind), options = (v) => kinds.map((k) => `<option${k === v ? ' selected' : ''}>${k}</option>`).join('');
  let out = `<div class="row"><input type="text" class="grow" data-brief="name" value="${esc(brief.name || '')}" placeholder="whose brief this is"></div>`;
  out += '<h3>What the drawing answers</h3>';
  out += r && r.sound ? r.brief.map((s) => `<div class="finding${s.ok ? '' : ' bad'}"><span class="glyph">${s.ok ? '✓' : '✗'}</span><span class="what">${esc(s.text)}</span>${s.note ? `<span class="remark">${esc(s.note)}</span>` : ''}</div>`).join('')
    : '<p class="note">The brief is read once the walls are a plan.</p>';
  out += '<h3>Spaces asked for</h3>';
  out += (brief.needs || []).map((n, i) => `<div class="row tight"><input type="number" data-brief="needs:${i}:count" value="${n.count}" min="1" step="1" style="width:40px">
    <select data-brief="needs:${i}:kind">${options(n.kind)}</select><label>of at least</label><input type="number" data-brief="needs:${i}:area" value="${n.area}" min="0" step="0.5" style="width:52px"><label>m²</label>
    <button class="cross" title="Ask for it no longer" data-brief-remove="needs:${i}">×</button></div>`).join('');
  out += '<button class="plain" data-brief-add="needs">Ask for another space</button>';
  out += '<h3>Relations between uses</h3>';
  out += (brief.relations || []).map((n, i) => `<div class="row tight"><select data-brief="relations:${i}:a">${options(n.a)}</select>
    <select data-brief="relations:${i}:how">${['opens to', 'adjoins', 'apart', 'not through'].map((h) => `<option${h === n.how ? ' selected' : ''}>${h}</option>`).join('')}</select>
    <select data-brief="relations:${i}:b">${options(n.b)}</select><button class="cross" title="State it no longer" data-brief-remove="relations:${i}">×</button></div>`).join('');
  out += '<button class="plain" data-brief-add="relations">State another relation</button>';
  out += '<p class="note small">A relation is read of every room of the first use: “bedroom not through bedroom” says no bedroom is reached only by passing through another.</p>';
  return out;
}

const OWN = [
  ['siting', 'height', 'most', 'Height, at most', 'm', 1], ['siting', 'storeys', 'most', 'Storeys, at most', '', 1], ['siting', 'street_setback', 'least', 'From the street, at least', 'm', 1],
  ['siting', 'side_setback', 'least', 'From the sides and rear, at least', 'm', 1], ['siting', 'coverage', 'most', 'Site coverage, at most', '%', 100],
  ['code', 'ceiling_habitable', 'least', 'Ceilings of habitable rooms, at least', 'm', 1], ['code', 'light', 'least', 'Glass, of the floor, at least', '%', 100],
  ['code', 'air', 'least', 'Openable, of the floor, at least', '%', 100], ['code', 'door', 'least', 'Doors\' clear width, at least', 'm', 1],
];

function panelRules() {
  const r = state.reading, scheme = state.schemes.find((s) => s.id === doc().site.scheme);
  let out = `<div class="row"><select data-act="scheme" class="grow">${state.schemes.map((s) => `<option value="${s.id}"${s.id === doc().site.scheme ? ' selected' : ''}>${esc(s.name)}</option>`).join('')}</select></div>`;
  if (scheme && scheme.version) out += `<p class="note small">${esc(scheme.version)}; read ${esc(scheme.read)}.</p>`;
  if (scheme && scheme.about) out += `<p class="note small">${esc(scheme.about)}</p>`;
  if (r) {
    out += '<h3>The building on its lot</h3>';
    if (!doc().site.lot) out += '<p class="note">No lot is marked: mark one with the Lot tool, and its rules can be found.</p>';
    else out += r.siting.findings.length ? r.siting.findings.map(findingRow).join('') : '<p class="note">Nothing is enclosed yet.</p>';
    out += '<h3>Rooms, doors and stairs</h3>';
    out += r.habitability.findings.length ? r.habitability.findings.map(findingRow).join('') : '<p class="note">The scheme states no rule of habitation.</p>';
  }
  out += '<h3>Limits of your own</h3>';
  out += OWN.map(([root, key, bound, label, unit, k]) => {
    const v = doc()[root] && doc()[root][key] ? doc()[root][key][bound] : undefined;
    return `<div class="row"><label class="grow">${label}</label><input type="number" data-own="${root}:${key}:${bound}:${k}" value="${Number.isFinite(v) ? +(v * k).toFixed(4) : ''}" step="${k === 100 ? 1 : 0.1}" min="0" placeholder="not stated"><label>${unit}</label></div>`;
  }).join('');
  out += '<p class="note small">A limit left empty is not stated, and nothing is found against it.</p>';
  if (scheme && scheme.applies) out += `<h3>Where it applies</h3><p class="note">${esc(scheme.applies)}</p>`;
  if (scheme && scheme.not_evaluated && scheme.not_evaluated.length) out += `<h3>What is not found here</h3><ul class="plainlist">${scheme.not_evaluated.map((t) => `<li>${esc(t)}</li>`).join('')}</ul>`;
  return out;
}

const ELEMENTS = {
  outer_wall: 'Outer walls, net of openings', inner_wall: 'Inner walls, net of doors', window: 'Windows', outer_door: 'Doors to the outside', inner_door: 'Doors between rooms',
  ground_slab: 'Ground slab', upper_floor: 'Upper floors', roof: 'Roof', stair: 'Stairs', fit_out: 'Fit-out, by floor inside the walls', wet_room: 'Wet rooms, more',
};

function panelCost() {
  const r = state.reading;
  if (!r) return '<p class="note">The building is not read yet.</p>';
  const q = r.quantities, c = r.cost, rates = doc().rates || { items: {} }, lines = new Map(c.items.map((i) => [i.id, i])), unit = rates.currency ? ` ${esc(rates.currency)}` : '';
  let out = `<h3>What it would cost</h3><p class="total">${money(c.low)} to ${money(c.high)}${unit}</p>`;
  out += rates.presumed ? '<p class="note small">These rates are presumed: no open source of building rates was found, and none stands behind them. Replace them with your own; the cost is the range between each quantity at its low rate and at its high.</p>'
    : '<p class="note small">Each quantity at its low rate and at its high: the range is exact for the rates declared.</p>';
  out += `<div class="row"><label>currency</label><input type="text" data-rate="currency" value="${esc(rates.currency || '')}" placeholder="its name" style="width:90px"></div>`;
  out += `<table><thead><tr><th>element</th><th>quantity</th><th>low</th><th>high</th></tr></thead><tbody>${state.elements.map((e) => {
    const rate = rates.items[e.id] || ['', ''], line = lines.get(e.id);
    const quantity = { outer_wall: q.outer_wall_net, inner_wall: q.inner_wall_net, window: q.window_area, outer_door: q.outer_doors, inner_door: q.inner_doors, ground_slab: q.ground_slab, upper_floor: q.upper_floor, roof: q.roof, stair: q.stairs, fit_out: q.internal_area, wet_room: q.wet_area }[e.id];
    return `<tr><td>${ELEMENTS[e.id] || e.id}<br><small>${line ? `${money(line.low)} to ${money(line.high)}` : 'no rate'}</small></td><td>${e.unit === 'each' ? quantity : quantity.toFixed(1)}<br><small>${e.unit}</small></td>
      <td><input type="number" data-rate="${e.id}:0" value="${rate[0]}" min="0" step="10"></td><td><input type="number" data-rate="${e.id}:1" value="${rate[1]}" min="0" step="10"></td></tr>`;
  }).join('')}</tbody></table>`;
  const f = doc().fabric || {}, h = r.heat;
  out += `<h3>The heat its fabric loses</h3><p class="total">${h.total.toFixed(0)} W/K <small class="sub">${h.per_floor.toFixed(2)} W/K per m² of floor</small></p>
    <table><thead><tr><th>element</th><th>area</th><th>U, W/(m² K)</th><th>U A</th></tr></thead><tbody>${[['wall', 'Outer walls', q.outer_wall_net], ['window', 'Windows', q.window_area], ['door', 'Outer doors', q.outer_door_area], ['roof', 'Roof, in plan', q.roof_plan], ['ground', 'Ground slab', q.ground_slab]]
    .map(([id, name, area]) => `<tr><td>${name}</td><td>${area.toFixed(1)}</td><td><input type="number" data-fabric="${id}" value="${f[id] ?? ''}" min="0" step="0.05"></td><td>${h[id].toFixed(1)}</td></tr>`).join('')}</tbody></table>
    <p class="note small">The steady loss per kelvin between inside and outside, by transmission alone: thermal bridges and the air exchanged are left out. It compares designs; it does not predict a bill.</p>`;
  const sides = ['North', 'East', 'South', 'West'], most = Math.max(1e-9, ...q.glass_by_side);
  out += `<h3>Where the glass faces</h3><div class="sides">${sides.map((name, i) => `<span>${name}</span><span class="bar"><i style="width:${(100 * q.glass_by_side[i] / most).toFixed(0)}%"></i></span><span>${q.glass_by_side[i].toFixed(1)} m²</span>`).join('')}</div>
    <p class="note small">${q.outer_wall > 0 ? `Glass is ${pct(q.glass_area / q.outer_wall)} of the outer walls.` : ''}</p>`;
  out += `<p class="note small">The roof is counted as it is drawn: ${m2(q.roof)} of it over ${m2(q.roof_plan)} in plan. Its form, pitch and eaves are under <b>Shell</b>.</p>`;
  return out;
}

// ---- the street (ADR 0098) -----------------------------------------------------------------------------------------------------

// The buildings a street is read among: the neighbours as the design has them, and the building drawn, storey by storey.
function streetBuildings() {
  const out = [], c = around(), r = state.reading;
  if (c) for (const b of c.buildings) if (b.ring.length >= 3 && b.height > 0) out.push({ ring: b.ring, height: b.height, presumed: !!b.presumed, changed: !!b.changed, name: b.name || '' });
  if (r && r.shell) r.shell.footprints.forEach((parts, k) => { for (const f of parts) if (!f.court && r.storeys[k]) out.push({ ring: f.ring, height: r.storeys[k].top, own: true }); });
  return out;
}

let askingStreet = 0;
async function readStreet() {
  const street = doc().street, mine = ++askingStreet;
  if (!street || !street.line || street.line.length < 2) { state.street.reading = null; return renderAll(); }
  const all = streetBuildings(), line = street.line;
  // Those within 150 m of the way are enough: nothing farther shades its footpaths at the instants a day is read by.
  const near = all.filter((b) => P.nearestOnLine(line, P.centroid(b.ring)).distance < 150);
  try {
    const reply = await fetch('/api/street', { method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ street: { line, strips: street.strips || [], scheme: street.scheme || {}, need: street.need ?? 2, step: 5 }, around: { buildings: near.map((b) => ({ ring: b.ring, height: b.height })), suns: about().suns } }) });
    const body = await reply.json();
    if (mine !== askingStreet) return;
    if (reply.ok) state.street.reading = body, state.street.buildings = near, state.street.note = '';
    else state.street.reading = null, state.street.note = body.message || 'the engine did not answer';
  } catch {
    if (mine !== askingStreet) return;
    state.street.reading = null, state.street.note = 'the server did not answer';
  }
  if (state.street.reading) state.street.station = Math.max(0, Math.min(state.street.reading.stations.length - 1, state.street.station));
  renderAll();
}

// The ways a street can be taken from: the nearest few to the lot (or to the building, or to the place), each once by its name.
function waysNear() {
  const c = state.context;
  if (!c) return [];
  const lot = doc().site.lot, pts = extent(), at = lot ? P.centroid(lot.ring) : pts.length ? P.centroid(pts) : [0, 0], seen = new Set(), out = [];
  const all = c.ways.map((w, index) => ({ w, index, near: P.nearestOnLine(w.line, at) })).filter((x) => !x.w.rail && !x.w.people && x.near.distance < 160).sort((p, q) => p.near.distance - q.near.distance);
  for (const x of all) {
    const key = x.w.name || `#${x.index}`;
    if (seen.has(key)) continue;
    seen.add(key);
    out.push(x);
    if (out.length >= 6) break;
  }
  return out;
}

function takeWay(index) {
  const w = state.context && state.context.ways[index];
  if (!w) return;
  const lot = doc().site.lot, pts = extent(), at = lot ? P.centroid(lot.ring) : pts.length ? P.centroid(pts) : [0, 0];
  // A map cuts a street at its crossings: the ways of its name that carry on from it are joined, and 240 m about the site kept.
  const whole = P.joinWays(state.context.ways, index).line;
  const line = P.stretch(whole, P.nearestOnLine(whole, at).along, 120).map((q) => [Math.round(q[0] * 100) / 100, Math.round(q[1] * 100) / 100]);
  state.street.station = 0;
  if (change('declare', { path: ['street'], value: { name: w.name || '', line, strips: [], need: 2, heights: [], scheme: {} } }, true)) say(`${w.name || 'The way'} is in hand: its section is read along ${Math.round(line.reduce((s, q, i) => (i ? s + P.dist(q, line[i - 1]) : 0), 0))} m.`);
}

const setStreet = (more) => change('declare', { path: ['street'], value: { ...doc().street, ...more } }, true);
const compass = (v) => P.windOf(Math.atan2(v[1], v[0]) * 180 / Math.PI);

function renderStreet() {
  if (state.mode !== 'street') return;
  const svg = $('street'), box = svg.getBoundingClientRect(), w = Math.max(300, box.width), h = Math.max(260, box.height), r = state.street.reading, street = doc().street;
  svg.setAttribute('viewBox', `0 0 ${w} ${h}`);
  if (!street || !r) {
    svg.innerHTML = `<text class="sx-title" x="${w / 2}" y="${h / 2 - 8}" text-anchor="middle">${street ? esc(cap(state.street.note || 'Reading the street…')) : 'No street is in hand.'}</text><text class="sx-note" x="${w / 2}" y="${h / 2 + 14}" text-anchor="middle">${street ? '' : 'Choose one of the ways about the site, at the right.'}</text>`;
    return;
  }
  const i = state.street.station, st = r.stations[i];
  const sun = sunNow(), heading = `${street.name || 'The street'} · ${st.at.toFixed(0)} m along, looking ${compass([st.left[1], -st.left[0]])}`;
  // The section above, the street unrolled beneath it, and room below for the Sun's bar.
  const low = 56, strip = 150, owns = new Set(state.street.buildings.map((b, k) => (b.own ? k : -1)).filter((k) => k >= 0));
  const section = P.sectionSvg(r, i, { box: [0, 0, w, h - strip - low - 8], sun: sun.fixed ? null : sun, own: st.faces.map((f) => f.building).find((b) => owns.has(b)) ?? null, heading });
  const map = P.unrolledSvg(r, i, { box: [0, h - strip - low, w, strip], need: r.need, own: owns });
  state.street.at = map.at, state.street.mapTop = h - strip - low, state.street.mapBottom = h - low;
  svg.innerHTML = section.svg + `<line class="sx-rule" x1="16" y1="${h - strip - low - 6}" x2="${w - 16}" y2="${h - strip - low - 6}"/>` + map.svg;
}

function panelStreet() {
  const street = doc().street, r = state.street.reading;
  if (!street) {
    if (!doc().site.place) return '<h3>The street</h3><p class="note">Give the site a place (under <b>Site</b>, in the plan), and the ways about it can be taken in hand: a street\'s section, its sun and what its buildings may rise to are read from what stands there.</p>';
    const ways = waysNear();
    return `<h3>The street</h3><p class="note">Take a way in hand. Its section is read along 240 m of it, about the site.</p>${ways.length ? `<div class="found">${ways.map((x) => `<button data-way="${x.index}">${esc(x.w.name || 'A way without a name')} <small class="sub">${esc(x.w.kind || '')} · ${x.near.distance.toFixed(0)} m away</small></button>`).join('')}</div>` : `<p class="note small">${state.context ? 'No way for vehicles lies within 160 m.' : esc(state.contextNote || 'The surroundings are not read yet.')}</p>`}`;
  }
  let out = `<h3>${esc(street.name || 'The street')}</h3>`;
  if (!r) return out + `<p class="note">${esc(cap(state.street.note || 'Reading the street…'))}</p><button class="plain quiet" data-act="street-clear">Take another way</button>`;
  const side = (s) => r.sides[s], num = (x, d = 1) => (x === null || !Number.isFinite(x) ? '–' : x.toFixed(d));
  out += `<p>${r.length.toFixed(0)} m of it, read every ${r.step} m. ${r.width_least === null ? 'No stretch has faces on both sides.' : `Between the faces it is ${num(r.width_least)} m at its narrowest, ${num(r.width_median)} m at the median.`}</p>
    <p class="total">${r.enclosure.toFixed(2)} <small class="sub">height over width</small> · ${(100 * r.sky).toFixed(0)}% <small class="sub">of the sky seen from its middle</small></p>
    <table><thead><tr><th>side</th><th>line</th><th>built</th><th>aligned</th><th>height</th>${r.lit ? '<th>sun</th>' : ''}</tr></thead><tbody>${['left', 'right'].map((name, s) => `<tr><td>${name}</td><td>${num(side(s).line)} m</td><td>${pct(side(s).built)}</td><td>${pct(side(s).aligned)}</td><td>${num(side(s).height)} m</td>${r.lit ? `<td>${side(s).sun.toFixed(1)} h</td>` : ''}</tr>`).join('')}</tbody></table>
    <p class="note small">Left and right are as one walks it. <b>Line:</b> the median distance of a side's faces from the centreline. <b>Built:</b> the share of its length with a face within 40 m; <b>aligned:</b> with one within 2 m of the line.${r.lit ? ' <b>Sun:</b> the mean hours on that side\'s footpath on the day chosen.' : ''}</p>
    <div class="row"><button class="plain" data-act="station-back">◂ back</button><label class="grow" style="text-align:center">station ${state.street.station + 1} of ${r.stations.length}</label><button class="plain" data-act="station-on">on ▸</button></div>`;
  // Its section.
  const strips = street.strips || [];
  out += '<h3>Its section, from the left</h3>';
  if (!strips.length) out += `<p class="note">A map does not say how a street is divided. Set its strips out by hand, or begin from a division presumed for its width.</p><button class="plain" data-act="strips-presume">Begin from a presumed division${r.width_least !== null ? ` of ${num(r.width_least)} m` : ''}</button>`;
  else {
    out += `<table class="strips"><tbody>${strips.map((s, i) => `<tr><td><select data-strip="${i}:kind">${Object.entries(P.STRIPS).map(([id, d]) => `<option value="${id}"${id === s.kind ? ' selected' : ''}>${d.title}</option>`).join('')}</select></td>
      <td><input type="number" data-strip="${i}:width" value="${s.width}" min="0.3" max="40" step="0.1"> m</td><td><button class="plain quiet" data-strip-move="${i}:-1" title="Toward the left">◂</button><button class="plain quiet" data-strip-move="${i}:1" title="Toward the right">▸</button><button class="plain quiet" data-strip-remove="${i}" title="Take it away">✕</button></td></tr>`).join('')}</tbody></table>
      <p class="${r.fits ? '' : 'bad'}">${r.fits ? '✓' : '✗'} ${r.total.toFixed(1)} m of strips${r.width_least !== null ? ` in ${num(r.width_least)} m between the faces at the narrowest` : ''}.</p>
      <div class="sides">${P.STRIP_GROUPS.map(([id, name]) => `<span>${name}</span><span class="bar"><i class="g-${id}" style="width:${(100 * r.shares[id]).toFixed(0)}%"></i></span><span>${pct(r.shares[id])}</span>`).join('')}</div>`;
  }
  out += `<div class="label">add</div><div class="chips">${Object.entries(P.STRIPS).map(([id, d]) => `<button data-strip-add="${id}">${d.title.toLowerCase()}</button>`).join('')}</div>`;
  // What is asked.
  const scheme = street.scheme || {}, least = (l) => (l && Number.isFinite(l.least) ? l.least : ''), most = (l) => (l && Number.isFinite(l.most) ? l.most : '');
  out += `<h3>What is asked of it</h3>
    <div class="row"><label class="grow">a footpath at least</label><input type="number" data-ask="width.footpath.least" value="${least(scheme.width && scheme.width.footpath)}" min="0" step="0.1" placeholder="not stated"><label>m</label></div>
    <div class="row"><label class="grow">a traffic lane at most</label><input type="number" data-ask="width.lane.most" value="${most(scheme.width && scheme.width.lane)}" min="0" step="0.1" placeholder="not stated"><label>m</label></div>
    <div class="row"><label class="grow">for people on foot at least</label><input type="number" data-ask="people.least:100" value="${least(scheme.people) === '' ? '' : +(least(scheme.people) * 100).toFixed(2)}" min="0" max="100" step="1" placeholder="not stated"><label>%</label></div>
    <div class="row"><label class="grow">height over width at most</label><input type="number" data-ask="enclosure.most" value="${most(scheme.enclosure)}" min="0" step="0.1" placeholder="not stated"><label></label></div>
    <div class="row"><label class="grow">sun on a footpath at least</label><input type="number" data-ask="sun.least" value="${least(scheme.sun)}" min="0" step="0.5" placeholder="not stated"><label>h</label></div>
    ${r.findings.length ? r.findings.map(findingRow).join('') : '<p class="note small">Nothing is asked yet. A limit left empty is not stated, and nothing is found against it; none here is any authority\'s.</p>'}`;
  // The sun, and how high each may rise.
  out += '<h3>Sun on its footpaths</h3>';
  if (!r.lit) out += '<p class="note small">The Sun is the place\'s Sun on a day: give the site a place.</p>';
  else {
    out += `<div class="row"><label>on</label><select data-light="day">${DAYS.map(([d, n]) => `<option value="${d}"${d === state.sun.day ? ' selected' : ''}>${n}</option>`).join('')}</select><label>keeping</label><input type="number" data-street="need" value="${r.need}" min="0" max="12" step="0.5"><label>h</label></div>
      <p>${['left', 'right'].map((name, s) => `The ${name} footpath has ${side(s).sun.toFixed(1)} h on average, and ${pct(side(s).sunny)} of its length has ${r.need} h.`).join(' ')}</p>
      <h3>How high each may rise</h3>`;
    // Each building that stands on the street, in the order it is passed; the building drawn once, at its full height.
    const tall = Math.max(0, ...state.street.buildings.filter((b) => b.own).map((b) => b.height));
    const rows = r.rises.map((x) => {
      const b = state.street.buildings[x.building] || {}, where = r.stations.find((st) => st.faces.some((f) => f.building === x.building));
      return { x, b, at: where ? where.at : Infinity, on: where ? (where.faces[0].building === x.building ? 'left' : 'right') : '' };
    }).filter((row) => !row.b.own || row.b.height === tall).sort((p, q) => p.at - q.at);
    out += `<table class="rises"><thead><tr><th>stands</th><th>now</th><th>takes</th><th>may rise to</th><th>set</th></tr></thead><tbody>${rows.map(({ x, b, at, on }) => `<tr${b.own ? ' class="own"' : ''}><td>${b.own ? '<b>the building drawn</b>' : esc(b.name || `${on}, ${Number.isFinite(at) ? at.toFixed(0) : '?'} m along`)}${b.presumed ? ' <small class="sub">presumed</small>' : ''}${b.changed ? ' <small class="sub">set here</small>' : ''}</td>
      <td>${x.height.toFixed(1)} m</td><td>${x.takes >= 0.005 ? `${x.takes.toFixed(2)} h` : '–'}</td><td>${x.limit === null ? '–' : `${x.limit.toFixed(1)} m`}</td><td>${b.own ? '' : `<input type="number" data-rise="${x.building}" value="${x.height.toFixed(1)}" min="0" max="300" step="1">`}</td></tr>`).join('')}</tbody></table>
      <p class="note small">The greatest height at which every point of a footpath that now has its ${r.need} h keeps them, the others as they stand: one building at a time. Two raised together may take what neither takes alone. <b>Takes:</b> the sun the footpaths' points would have, on average, were it gone. A dash under <b>may rise to</b>: it shades no footpath that has its hours, at any height (a building raised past its limit has none: what it shaded has lost them). <b>Presumed:</b> the map gives no height, and 6 m is taken. A height set here is this design\'s, not the map\'s; it shades the building drawn as well.${(street.heights || []).length ? ` <button class="plain quiet" data-act="heights-clear">Put back the map's heights</button>` : ''}</p>`;
  }
  out += '<p><button class="plain quiet" data-act="street-clear">Take another way</button></p>';
  return out;
}

// ---- the shell: the roof, the walls' cladding, the garden's edge, the faces ---------------------------------------------------

const WINDS = { east: 0, 'north-east': 45, north: 90, 'north-west': 135, west: 180, 'south-west': -135, south: -90, 'south-east': -45 };
const swatch = (act, items, now, tone) => `<div class="chips swatches">${items.map(([id, title, colour]) => `<button data-act="${act}" data-v="${id}" class="${now === id ? 'on' : ''}">${colour ? `<i style="background:${colour}"></i>` : ''}${esc(title)}</button>`).join('')}</div>`;

// The eaves of the roofs the engine read: each edge of a footprint, named by the way it looks.
function eaves() {
  const r = state.reading, out = [];
  if (!r || !r.shell) return out;
  for (const roof of r.shell.roofs) {
    const part = (r.shell.footprints[roof.storey] || []).filter((f) => !f.court)[roof.part];
    if (!part) continue;
    part.ring.forEach((a, k) => {
      const b = part.ring[(k + 1) % part.ring.length], L = P.dist(a, b);
      out.push({ middle: [(a[0] + b[0]) / 2, (a[1] + b[1]) / 2], length: L, wind: P.windOf(Math.atan2(-(b[0] - a[0]), b[1] - a[1]) * 180 / Math.PI), gable: !!roof.gable[k] });
    });
  }
  return out;
}

function panelShell() {
  const r = state.reading, roof = P.roofOf(doc()), shell = P.shellOf(doc()), bound = P.boundaryOf(doc()), read = r && r.shell ? r.shell : null;
  let out = '<h3>The roof</h3>';
  out += swatch('roof-form', P.ROOF_FORMS, roof.form);
  if (roof.form === 'flat') out += `<div class="row"><label>parapet</label><input type="number" data-roof="parapet" value="${roof.parapet}" min="0" max="1.5" step="0.05"><label>m above the roof</label></div>`;
  else {
    out += `<div class="row"><label>pitch</label><input type="number" data-roof="pitch" value="${roof.pitch}" min="1" max="60" step="2.5"><label>°</label><label>eaves</label><input type="number" data-roof="eaves" value="${roof.eaves}" min="0" max="1.5" step="0.05"><label>m beyond the walls</label></div>`;
    out += '<div class="label">covered with</div>' + swatch('roof-cover', Object.entries(P.ROOF_COVERS).filter(([id]) => id !== 'membrane').map(([id, c]) => [id, c.title, c.tone]), roof.cover);
  }
  if (roof.form === 'shed') out += '<div class="label">the water runs to the</div>' + swatch('roof-fall', ['north', 'east', 'south', 'west'].map((w) => [String(WINDS[w]), w]), String(Math.round(roof.fall / 90) * 90 === -180 ? 180 : Math.round(roof.fall / 90) * 90));
  if (roof.form === 'gable') {
    const all = eaves();
    out += `<div class="label">its eaves; choose which stand as gables</div>${all.length ? `<div class="chips">${all.map((e, i) => `<button data-gable="${i}" class="${e.gable ? 'on' : ''}">${e.wind} · ${e.length.toFixed(1)} m${e.gable ? ' · gable' : ''}</button>`).join('')}</div>` : '<p class="note small">Draw outer walls that close, and their eaves are listed.</p>'}`;
  }
  if (read && read.roofs.length) {
    const area = read.roofs.reduce((s, x) => s + x.area, 0), plan = read.roofs.reduce((s, x) => s + x.plan_area, 0), walls = read.roofs.reduce((s, x) => s + x.wall_area, 0), top = Math.max(...read.roofs.map((x) => x.top)), plate = read.roofs[0].plate;
    out += `<p class="total">${m2(area)} <small class="sub">of roof over ${m2(plan)} in plan</small></p><p>${roof.form === 'flat' ? 'Its parapet' : 'Its highest point'} stands ${m(top)} above the ground, ${m(top - plate)} above the walls${walls > 0.005 ? `; ${m2(walls)} of wall is carried up to it` : ''}.</p>`;
    for (const x of read.roofs) if (x.note) out += `<p class="note small">${esc(cap(x.note))}.</p>`;
    if (roof.form === 'hip' || roof.form === 'gable') out += '<p class="note small">Planes of one pitch rising from every eave: the footprint\'s straight skeleton. Every face has the pitch, so the area is the plan\'s over its cosine, exactly.</p>';
  } else out += '<p class="note small">A roof stands on outer walls that close.</p>';
  out += '<h3>The walls outside</h3>' + swatch('clad', Object.entries(P.CLADDINGS).map(([id, c]) => [id, c.title]), shell.cladding)
    + swatch('tone', Object.entries(P.WALL_TONES).map(([id, colour]) => [id, id, colour]), shell.tone);
  out += '<h3>The lot\'s edge</h3>' + swatch('bound', Object.entries(P.BOUNDARIES).map(([id, b]) => [id, b.title]), bound.kind);
  if (bound.kind !== 'none') out += `<div class="row"><label>height</label><input type="number" data-bound="height" value="${bound.height}" min="0.3" max="3" step="0.1"><label>m, on the edges that face no street</label></div>`;
  if (!doc().site.lot) out += '<p class="note small">It stands where a lot is marked.</p>';
  if (read && read.facades.length) {
    out += `<h3>Its faces</h3><table><thead><tr><th>looks</th><th>wall</th><th>glass</th><th>share</th><th></th></tr></thead><tbody>${read.facades.map((f) => `<tr><td>${f.wind}</td><td>${f.wall.toFixed(1)} m²</td><td>${f.glass.toFixed(1)} m²</td><td>${pct(f.share)}</td><td><small>${f.windows} window${f.windows === 1 ? '' : 's'}${f.doors ? `, ${f.doors} door${f.doors === 1 ? '' : 's'}` : ''}</small></td></tr>`).join('')}</tbody></table>
      <p class="note small">Each outer wall with the way it looks, to the nearest of eight winds; its area is its length by its storey's height, with what is carried up to the roof. <b>Elevations</b>, above, draws them.</p>`;
  }
  return out;
}

// ---- light: the sun, the sky and the daylight at the glass ----------------------------------------------------------------------

function panelLight() {
  const r = state.reading, sh = r && r.shell, light = { transmittance: 0.68, reflectance: 0.5, gain: 0.6, indoor: 20, outdoor: 8, ...(doc().light || {}) };
  if (!sh) return '<p class="note">The building is not read yet.</p>';
  const place = doc().site.place, name = (x) => { const room = r.storeys[x.storey].rooms[x.room]; return room.name || room.kind || 'a room'; };
  let out = '<h3>Daylight and sun, room by room</h3>';
  out += place ? `<div class="row"><label>on</label><select data-light="day">${DAYS.map(([d, n]) => `<option value="${d}"${d === state.sun.day ? ' selected' : ''}>${n}</option>`).join('')}</select><label>at ${esc(place.name || 'the place')}, among ${sh.neighbours} neighbour${sh.neighbours === 1 ? '' : 's'}</label></div>`
    : '<p class="note small">No place is given, so the Sun is not: give the site a place, and each room\'s hours of sun on a day are found. The daylight factor needs no Sun.</p>';
  const rooms = sh.rooms.filter((x) => r.storeys[x.storey].rooms[x.room].habitable || x.panes > 0);
  out += `<table><thead><tr><th>room</th><th>glass</th><th>sky</th><th>daylight</th>${sh.lit ? '<th>sun</th>' : ''}</tr></thead><tbody>${rooms.map((x) => `<tr class="pick" data-room="${x.storey}:${x.room}"><td>${esc(name(x))}</td><td>${x.glass.toFixed(1)} m²</td><td>${x.panes ? `${x.sky.toFixed(0)}°` : '–'}</td><td>${x.daylight_factor.toFixed(1)}%</td>${sh.lit ? `<td>${x.sun_hours.toFixed(1)} h</td>` : ''}</tr>`).join('')}</tbody></table>`;
  out += `<p class="note small"><b>Sky:</b> the angle of sky a room's glass sees, square to the wall, between what stands before it and the eave above it. <b>Daylight:</b> the average daylight factor, the light on a table as a share of that outdoors under an overcast sky (BRE's estimate; BS 8206-2 took 2% as a room that looks daylit and 5% as one that needs no lamps by day, and is withdrawn for a standard that asks otherwise). ${sh.lit ? '<b>Sun:</b> the hours its best window\'s middle is in the Sun on a clear day.' : ''}</p>`;
  if (sh.lit) {
    const gain = r.heat.day_gain, loss = r.heat.day_loss, most = Math.max(gain, loss, 1e-9);
    out += `<h3>A clear day's balance</h3><div class="sides"><span>Sun in</span><span class="bar"><i class="warm" style="width:${(100 * gain / most).toFixed(0)}%"></i></span><span>${gain.toFixed(1)} kWh</span><span>Lost</span><span class="bar"><i class="cool" style="width:${(100 * Math.max(0, loss) / most).toFixed(0)}%"></i></span><span>${loss.toFixed(1)} kWh</span></div>
      <div class="row"><label>held at</label><input type="number" data-light="indoor" value="${light.indoor}" step="1"><label>°C inside, with</label><input type="number" data-light="outdoor" value="${light.outdoor}" step="1"><label>°C outside</label></div>
      <p class="note small">In: the Sun's direct beam through the glass over the day, on a clear day (the sky's diffuse light is left out, so it is a bound from below). Lost: the fabric's ${r.heat.total.toFixed(0)} W/K over 24 hours at these temperatures, which are yours to state. A comparison between designs, not a forecast.</p>`;
  }
  out += `<h3>What is presumed</h3>
    <div class="row"><label class="grow">light the glass passes</label><input type="number" data-light="transmittance" value="${light.transmittance}" min="0.1" max="1" step="0.01"></div>
    <div class="row"><label class="grow">light the rooms' surfaces reflect</label><input type="number" data-light="reflectance" value="${light.reflectance}" min="0" max="0.9" step="0.05"></div>
    <div class="row"><label class="grow">the Sun's heat the glass passes</label><input type="number" data-light="gain" value="${light.gain}" min="0" max="1" step="0.05"></div>
    <p class="note small">Left out, and so to be read with care: the roof above the walls as an obstruction, light reflected from the ground and from what stands opposite, trees, the ground's relief. A neighbour with no height in the data stands 6 m.</p>`;
  return out;
}

function panelSite() {
  const site = doc().site, lot = site.lot, r = state.reading;
  let out = '<h3>The place</h3>';
  out += `<div class="row"><input type="text" class="grow" id="place" placeholder="a town, or a latitude and longitude" value=""><button class="key" data-act="find">Find</button></div>`;
  if (state.found.length) out += `<div class="found">${state.found.map((row, i) => `<button data-found="${i}">${esc(row[0])} <small class="sub">${esc(row[4] || row[3] || '')}</small></button>`).join('')}</div>`;
  out += site.place ? `<p>${esc(site.place.name || 'A place')} · ${site.place.lat.toFixed(5)}°, ${site.place.lon.toFixed(5)}° <small class="sub">WGS 84</small></p><p class="note small">The drawing is in metres east and north of this point. ${esc(state.contextNote)}</p>
    <div class="row"><label><input type="checkbox" data-act="surroundings" ${state.show.surroundings ? 'checked' : ''}> draw the surroundings</label></div>`
    : '<p class="note">No place is given: the building stands on a plane with north up. Give one, and its streets and neighbours are read from the world\'s map, and the Sun is its Sun.</p>';
  out += '<h3>The lot</h3>';
  if (lot) {
    const area = r && r.siting.lot_area ? r.siting.lot_area : Math.abs(P.ringArea(lot.ring));
    out += `<p>${m2(area)}${r && Number.isFinite(r.siting.covered) ? ` · the building covers ${pct(r.siting.covered / area)}` : ''}</p>
      <table><thead><tr><th>edge</th><th>length</th><th>faces</th></tr></thead><tbody>${lot.ring.map((p, e) => `<tr class="pick" data-edge="${e}"><td>${e + 1}</td><td>${m(P.dist(p, lot.ring[(e + 1) % lot.ring.length]))}</td>
      <td><button class="plain${lot.street[e] ? '' : ' quiet'}" data-street="${e}">${lot.street[e] ? 'a street' : 'a side or the rear'}</button></td></tr>`).join('')}</tbody></table>
      <p class="note small">Click what an edge faces to change it. The dotted ring inside the lot is the ground its setbacks leave.</p>
      <button class="plain" data-act="draw-lot">Mark it again</button> · <button class="plain quiet" data-act="clear-lot">Clear it</button>`;
  } else {
    out += `<p class="note">No lot is marked. Mark its corners on the drawing, or set down a rectangle to begin from.</p>
      <button class="plain" data-act="draw-lot">Mark the lot's corners</button>
      <div class="row"><label>or a lot of</label><input type="number" id="lot-w" value="15" min="3" step="0.5"><label>×</label><input type="number" id="lot-d" value="30" min="3" step="0.5"><label>m</label><button class="key" data-act="box-lot">Set it down</button></div>`;
  }
  return out;
}

function renderPanel() {
  const streets = state.mode === 'street';
  $('tabs').toggleAttribute('hidden', streets), $('inspector').toggleAttribute('hidden', streets);
  if (streets) { const panel = $('panel'), top = panel.scrollTop; panel.innerHTML = panelStreet(); panel.scrollTop = top; return; }
  renderTabs();
  const panel = $('panel'), top = panel.scrollTop;
  panel.innerHTML = { rooms: panelRooms, interior: panelInterior, brief: panelBrief, rules: panelRules, shell: panelShell, light: panelLight, cost: panelCost, site: panelSite }[state.panel]();
  panel.scrollTop = top;
}

function panelChange(el) {
  const d = el.dataset, num = Number(el.value);
  if (d.brief) {
    const [list, i, key] = d.brief.split(':');
    if (list === 'name') return change('declare', { path: ['brief', 'name'], value: el.value }, true);
    const value = key === 'count' ? Math.max(1, Math.round(num)) : key === 'area' ? Math.max(0, num) : el.value;
    return change('declare', { path: ['brief', list, Number(i), key], value }, true);
  }
  if (d.own) {
    const [root, key, bound, k] = d.own.split(':');
    return change('declare', { path: [root, key, bound], value: el.value === '' ? undefined : num / Number(k) }, true);
  }
  if (d.rate) {
    if (d.rate === 'currency') return change('declare', { path: ['rates', 'currency'], value: el.value }, true);
    const [id, end] = d.rate.split(':'), now = (doc().rates.items[id] || [0, 0]).slice();
    now[Number(end)] = Math.max(0, num);
    if (now[1] < now[0]) now[1 - Number(end)] = now[Number(end)];
    return history.group('Declare a rate', [['declare', { path: ['rates', 'items', id], value: now }], ['declare', { path: ['rates', 'presumed'], value: false }]]) && changed();
  }
  if (d.fabric) return change('declare', { path: ['fabric', d.fabric], value: Math.max(0, num) }, true);
  if (d.strip) {
    const [i, key] = d.strip.split(':'), strips = (doc().street.strips || []).map((s) => ({ ...s }));
    strips[Number(i)][key] = key === 'width' ? Math.max(0.3, Math.min(40, num)) : el.value;
    return setStreet({ strips });
  }
  if (d.ask) {
    const [path, k] = d.ask.split(':'), keys = path.split('.'), scheme = structuredClone(doc().street.scheme || {});
    let at = scheme;
    for (const key of keys.slice(0, -1)) at = at[key] = at[key] || {};
    if (el.value === '') delete at[keys[keys.length - 1]]; else at[keys[keys.length - 1]] = num / Number(k || 1);
    return setStreet({ scheme });
  }
  if (d.street === 'need') return setStreet({ need: Math.max(0, Math.min(12, num)) });
  if (d.rise) {
    const b = state.street.buildings[Number(d.rise)];
    if (!b || b.own) return null;
    const at = P.interiorPoint(b.ring).map((v) => Math.round(v * 100) / 100), heights = (doc().street.heights || []).filter((x) => !P.inRing(x.at, b.ring));
    return setStreet({ heights: [...heights, { at, height: Math.max(0, Math.min(300, num)) }] });
  }
  if (d.roof) return setRoof({ [d.roof]: d.roof === 'pitch' ? Math.max(1, Math.min(60, num)) : Math.max(0, Math.min(1.5, num)) });
  if (d.bound) return change('declare', { path: ['site', 'boundary'], value: { ...P.boundaryOf(doc()), height: Math.max(0.3, Math.min(3, num)) } }, true);
  if (d.light === 'day') { state.sun.day = num; renderSunbar(); return read().then(() => { if (state.mode === 'street') readStreet(); }); }
  if (d.light) return change('declare', { path: ['light', d.light], value: num }, true);
  if (d.act === 'passage') return change('declare', { path: ['interior', 'passage'], value: Math.max(0.3, Math.min(1.5, num)) }, true);
  if (d.act === 'scheme') {
    const scheme = state.schemes.find((s) => s.id === el.value);
    if (!scheme) return null;
    history.group('Choose a scheme of rules', [['declare', { path: ['site', 'scheme'], value: scheme.id }], ['declare', { path: ['siting'], value: structuredClone(scheme.siting || {}) }],
      ['declare', { path: ['code'], value: structuredClone(scheme.code || {}) }]]);
    return changed();
  }
  if (d.act === 'surroundings') { state.show.surroundings = el.checked; renderPlan(), renderSolid(); }
  return null;
}

// The roof as the document keeps it, changed in what is given. A roof first made a gable takes the two ends of its long way.
function setRoof(more) {
  const next = { ...P.roofOf(doc()), ...more };
  if (more.form && more.form !== 'flat' && !(doc().roof && doc().roof.pitch > 0)) next.pitch = 27.5;
  if (more.form === 'gable' && !(next.gables && next.gables.length)) {
    const prints = state.reading && state.reading.shell ? state.reading.shell.footprints[doc().storeys.length - 1] || [] : [], part = prints.find((f) => !f.court);
    next.gables = part ? P.defaultGables(part.ring) : [];
  }
  if (more.form === 'flat') next.pitch = 0;
  if (more.form && more.form !== 'flat' && next.cover === 'membrane') next.cover = 'tiles';
  if (more.form) state.show.roof = true;
  return change('declare', { path: ['roof'], value: next }, true);
}

function panelClick(el) {
  const d = el.dataset;
  if (d.way) return takeWay(Number(d.way));
  if (d.act === 'street-clear') { state.street.reading = null; return change('declare', { path: ['street'], value: undefined }, true); }
  if (d.act === 'strips-presume') return setStreet({ strips: P.presumedStrips(state.street.reading ? state.street.reading.width_least ?? 16 : 16) });
  if (d.act === 'heights-clear') return setStreet({ heights: [] });
  if (d.act === 'station-back' || d.act === 'station-on') { const n = state.street.reading ? state.street.reading.stations.length : 1; state.street.station = Math.max(0, Math.min(n - 1, state.street.station + (d.act === 'station-on' ? 1 : -1))); renderStreet(); return renderPanel(); }
  if (d.stripAdd) {
    // A new strip goes beside the centreline, where the carriageway is; a footpath goes to the nearer face.
    const strips = (doc().street.strips || []).map((s) => ({ ...s })), fresh = { kind: d.stripAdd, width: P.STRIPS[d.stripAdd].width };
    if (d.stripAdd === 'footpath' || d.stripAdd === 'frontage') strips.push(fresh); else strips.splice(Math.ceil(strips.length / 2), 0, fresh);
    return setStreet({ strips });
  }
  if (d.stripRemove) return setStreet({ strips: (doc().street.strips || []).filter((s, i) => i !== Number(d.stripRemove)) });
  if (d.stripMove) {
    const [i, by] = d.stripMove.split(':').map(Number), strips = (doc().street.strips || []).map((s) => ({ ...s })), j = i + by;
    if (j < 0 || j >= strips.length) return null;
    [strips[i], strips[j]] = [strips[j], strips[i]];
    return setStreet({ strips });
  }
  if (d.act === 'roof-form') return setRoof({ form: d.v });
  if (d.act === 'roof-cover') return setRoof({ cover: d.v });
  if (d.act === 'roof-fall') return setRoof({ fall: Number(d.v) });
  if (d.gable) {
    const all = eaves(), e = all[Number(d.gable)];
    if (!e) return null;
    return setRoof({ gables: all.filter((x, i) => (i === Number(d.gable) ? !x.gable : x.gable)).map((x) => x.middle.map((v) => Math.round(v * 1000) / 1000)) });
  }
  if (d.act === 'clad') return change('declare', { path: ['shell'], value: { ...P.shellOf(doc()), cladding: d.v } }, true);
  if (d.act === 'tone') return change('declare', { path: ['shell'], value: { ...P.shellOf(doc()), tone: d.v } }, true);
  if (d.act === 'bound') return change('declare', { path: ['site', 'boundary'], value: { kind: d.v, height: P.BOUNDARIES[d.v].height } }, true);
  if (d.room) { const [k, i] = d.room.split(':').map(Number); state.storey = k, state.selection = { type: 'room', index: i }; return renderAll(); }
  if (d.kind) { const again = state.placing === d.kind; if (state.mode === 'plan') setTool('furnish'); state.placing = again ? null : d.kind; renderPanel(); return say(again ? '' : `Click where the ${P.entry(d.kind).title.toLowerCase()} goes.`); }
  if (d.piece) { state.storey = doc().furniture[Number(d.piece)].storey; state.selection = { type: 'piece', index: Number(d.piece) }; return renderAll(); }
  if (d.briefAdd) {
    const list = (doc().brief && doc().brief[d.briefAdd]) || [];
    return change('declare', { path: ['brief', d.briefAdd, list.length], value: d.briefAdd === 'needs' ? { kind: 'bedroom', count: 1, area: 9 } : { a: 'bedroom', b: 'hall', how: 'opens to' } }, true);
  }
  if (d.briefRemove) { const [list, i] = d.briefRemove.split(':'); return change('declare', { path: ['brief', list, Number(i)], value: undefined }, true); }
  if (d.street) return change('setStreet', { edge: Number(d.street), street: !doc().site.lot.street[Number(d.street)] }, true);
  if (d.edge) { state.selection = { type: 'edge', index: Number(d.edge) }; return renderPlan(), renderInspector(); }
  if (d.found) { const row = state.found[Number(d.found)]; return goTo(row[0], row[2], row[1]); }
  if (d.act === 'find') return findPlace($('place').value);
  if (d.act === 'draw-lot') { setTool('lot'); return say('Click the lot\'s corners in turn; click the first again, or press Enter, to close it.'); }
  if (d.act === 'clear-lot') return change('setLot', { ring: null });
  if (d.act === 'box-lot') {
    const w = Number($('lot-w').value), dep = Number($('lot-d').value), c = state.view ? [state.view.cx, state.view.cy] : [0, 0];
    const x0 = P.toGrid(c[0] - w / 2, 0.5), y0 = P.toGrid(c[1] - dep / 2, 0.5);
    if (change('setLot', { ring: [[x0, y0], [x0 + w, y0], [x0 + w, y0 + dep], [x0, y0 + dep]], street: [false, false, false, false] })) { fitView(); say('The lot is set down. Say which of its edges face a street.'); }
  }
  return null;
}

// ---- the building in the round -------------------------------------------------------------------------------------------------

function sunNow() {
  // Without a place the light is a mid-afternoon's, from the south-west.
  return sunAt(state.sun.day, state.sun.hour) || { altitude: 40, azimuth: 218, fixed: true };
}

const DAYS = [[80, '20 March'], [172, '21 June'], [266, '23 September'], [355, '21 December']];
const walking = () => state.mode === 'walk';

function renderSunbar() {
  const el = $('sunbar'), sun = sunNow();
  el.hidden = drawn() && state.mode !== 'street';
  if (el.hidden) return;
  const streets = state.mode === 'street';
  const hour = `${String(Math.floor(state.sun.hour)).padStart(2, '0')}:${String(Math.round((state.sun.hour % 1) * 60)).padStart(2, '0')}`;
  el.innerHTML = (walking() ? '<span class="how">W A S D or the arrows to walk · drag to look · click a floor to go there</span>' : '')
    + (sun.fixed ? (walking() ? '' : '<span>Give the site a place, and the Sun is its Sun.</span>')
      : `<label>day <select data-sun="day">${DAYS.map(([d, name]) => `<option value="${d}"${d === state.sun.day ? ' selected' : ''}>${name}</option>`).join('')}</select></label>
       <label>solar time <input type="range" data-sun="hour" min="4" max="20" step="0.25" value="${state.sun.hour}"><b>${hour}</b></label>
       <span>${sun.altitude > 0 ? `Sun ${sun.altitude.toFixed(0)}° high, bearing ${sun.azimuth.toFixed(0)}°` : 'the Sun is down'}</span>`)
    + (streets ? '' : `<label><input type="checkbox" data-sun="lamps" ${state.show.lamps ? 'checked' : ''}> lamps</label>`)
    + (walking() || streets ? '' : `<label title="The walls stand only to 1.2 m, as a model's, so that what is against them is seen"><input type="checkbox" data-sun="cut" ${state.show.cut ? 'checked' : ''}> cut low</label>
       <label><input type="checkbox" data-sun="roof" ${state.show.roof ? 'checked' : ''}> roof</label>
       <label><input type="checkbox" data-sun="whole" ${state.show.whole ? 'checked' : ''}> every storey</label><button class="plain" data-sun="frame">see it whole</button>`);
}

// three.js and what the view can use of its additions; an addition that does not load is done without.
async function deps3d() {
  const THREE = await import('three'), { OrbitControls } = await import('three/addons/controls/OrbitControls.js');
  const maybe = async (path, name) => { try { return (await import(path))[name]; } catch { return null; } };
  return {
    THREE, OrbitControls,
    RoundedBoxGeometry: await maybe('three/addons/geometries/RoundedBoxGeometry.js', 'RoundedBoxGeometry'),
    RoomEnvironment: await maybe('three/addons/environments/RoomEnvironment.js', 'RoomEnvironment'),
    EffectComposer: await maybe('three/addons/postprocessing/EffectComposer.js', 'EffectComposer'),
    RenderPass: await maybe('three/addons/postprocessing/RenderPass.js', 'RenderPass'),
    GTAOPass: await maybe('three/addons/postprocessing/GTAOPass.js', 'GTAOPass'),
    GLTFLoader: await maybe('three/addons/loaders/GLTFLoader.js', 'GLTFLoader'),
    OutputPass: await maybe('three/addons/postprocessing/OutputPass.js', 'OutputPass'),
  };
}

// A model of one's own for the piece in hand: the file is kept by the server, read here once to learn its size, and the piece
// takes that size. The file is a binary glTF (.glb), in metres, its front toward +Z.
async function ownModel(file) {
  const sel = state.selection;
  if (!sel || sel.type !== 'piece' || !doc().furniture[sel.index]) return;
  try {
    const bytes = await file.arrayBuffer();
    const reply = await fetch('/api/models', { method: 'PUT', headers: { 'Content-Type': 'model/gltf-binary' }, body: bytes });
    const kept = await reply.json();
    if (!reply.ok) return say(cap(kept.message) + '.', true);
    const deps = await deps3d();
    if (!deps.GLTFLoader) return say('The model is kept, but its reader could not be loaded.', true);
    const scene = await new Promise((resolve, reject) => new deps.GLTFLoader().parse(bytes, '', (g) => resolve(g.scene), reject));
    const size = new deps.THREE.Box3().setFromObject(scene).getSize(new deps.THREE.Vector3()), fit = (v) => Math.max(0.05, Math.min(12, Math.round(v * 100) / 100));
    if (change('setPiece', { piece: sel.index, model: { id: kept.id, name: file.name }, width: fit(size.x), depth: fit(size.z), height: fit(size.y) }, true)) say(`The piece is now drawn by ${file.name}, ${fit(size.x)} × ${fit(size.z)} × ${fit(size.y)} m.`);
  } catch (error) {
    say(`${file.name} could not be read as a binary glTF file.`, true);
  }
}

let making = null;
async function renderSolid() {
  if (drawn()) return;
  if (!state.solid) {
    if (!making) making = deps3d().then((deps) => {
      state.solid = new P.BuildingView(deps, $('solid'), { night: document.documentElement.classList.contains('night') });
      state.solid.onEye = (eye) => { state.eye = { at: eye.at.slice(), heading: eye.heading, storey: eye.storey }; renderMini(); };
      wire3d(state.solid);
    }).catch(() => { making = null; say('The building in the round needs three.js, which could not be loaded.', true); });
    await making;
    if (!state.solid) return;
  }
  const v = state.solid, sel = state.selection;
  if (v.mode !== (walking() ? 'walk' : 'round')) v.setMode(walking() ? 'walk' : 'round');
  v.resize();
  v.update(doc(), state.reading, around(), {
    roof: walking() || state.show.roof, ceilings: walking(), surroundings: state.show.surroundings, lights: state.show.lamps,
    street: doc().street && doc().street.line && (doc().street.strips || []).length ? { line: doc().street.line, strips: P.placeStrips(doc().street.strips) } : null,
    upTo: walking() || state.show.whole ? Infinity : state.storey, selected: sel && sel.type === 'piece' ? sel.index : null, cut: state.show.cut && !walking() ? 1.2 : null,
  });
  const sun = sunNow();
  v.setSun(sun.altitude, sun.azimuth);
  if (walking() && !state.entered) {
    state.entered = true;
    const room = sel && sel.type === 'room' && R() && R().sound ? R().rooms[sel.index] : null;
    if (room) v.stand(P.interiorPoint(room.net_ring && room.net_ring.length ? room.net_ring : room.ring), 90, state.storey);
    else v.enter(state.storey);
  } else if (walking() && v.eye.storey !== state.storey) v.stand(v.eye.at, undefined, state.storey);
}

// The pointer on the building: turning about it or looking about within it, taking a piece in hand, moving it over the floor,
// setting one down, going to a place.
function wire3d(v) {
  const el = v.renderer.domElement;
  let drag = null;
  el.addEventListener('pointerdown', (e) => {
    if (e.button !== 0) return;
    const hit = v.pick(e.clientX, e.clientY), sel = state.selection;
    drag = { x: e.clientX, y: e.clientY, px: e.clientX, py: e.clientY, moved: false, hit };
    if (hit && hit.piece !== undefined && sel && sel.type === 'piece' && sel.index === hit.piece) {
      const piece = doc().furniture[hit.piece], from = v.onFloor(e.clientX, e.clientY, piece.storey);
      if (from) drag.piece = hit.piece, drag.from = from, v.orbit.enabled = false;
    }
    if (walking() || drag.piece !== undefined) el.setPointerCapture(e.pointerId);
  });
  el.addEventListener('pointermove', (e) => {
    if (!drag) return;
    if (Math.hypot(e.clientX - drag.x, e.clientY - drag.y) > 4) drag.moved = true;
    if (!drag.moved) return;
    if (drag.piece !== undefined) {
      const piece = doc().furniture[drag.piece], q = v.onFloor(e.clientX, e.clientY, piece.storey);
      if (q) { drag.to = moved_(piece, drag.from, q); v.preview(drag.piece, drag.to); }
    } else if (walking()) v.turn((e.clientX - drag.px) * 0.2, (e.clientY - drag.py) * 0.16);
    drag.px = e.clientX, drag.py = e.clientY;
  });
  el.addEventListener('pointerup', (e) => {
    const d = drag;
    drag = null;
    v.orbit.enabled = v.mode === 'round';
    if (!d) return;
    if (d.piece !== undefined && d.moved) { if (d.to) change('setPiece', { piece: d.piece, at: d.to.at, bearing: d.to.bearing }, true); return; }
    if (d.moved) return;
    const hit = d.hit;
    if (state.placing && hit && hit.floor) {
      const held = held_(state.placing, hit.floor, hit.storey);
      if (change('addPiece', { storey: hit.storey, kind: held.kind, at: held.at, bearing: held.bearing })) { state.selection = { type: 'piece', index: doc().furniture.length - 1 }; state.placing = null; renderAll(); }
    } else if (hit && hit.piece !== undefined) {
      state.selection = { type: 'piece', index: hit.piece };
      if (state.panel !== 'interior') state.panel = 'interior';
      v.select(hit.piece), renderInspector(), renderPanel();
    } else {
      if (state.selection) { state.selection = null; v.select(null), renderInspector(), renderPanel(); }
      if (walking() && hit && hit.floor && hit.storey === v.eye.storey) v.goTo(hit.floor);
    }
    void e;
  });
}

// The small plan beside the walk: the storey one is on, and where one stands and looks.
let miniDue = false;
function renderMini() {
  if (miniDue) return;
  miniDue = true;
  requestAnimationFrame(() => {
    miniDue = false;
    const box = $('minimap');
    box.hidden = !walking() || !state.eye;
    if (box.hidden) return;
    const svg = $('mini'), size = [260, 200], k = state.eye.storey, s = doc().storeys[k];
    if (!s) return;
    state.miniView = P.fit(s.vertices, size, 12);
    svg.setAttribute('viewBox', `0 0 ${size[0]} ${size[1]}`);
    svg.innerHTML = P.planSvg({ doc: doc(), reading: state.reading, storey: k, view: state.miniView, size, selection: null, ghost: null, context: null, marks: [], eye: state.eye,
      show: { grid: false, vertices: false, names: false, furnishings: false } });
    const S = state.reading && state.reading.storeys[k], room = S && S.sound ? S.rooms.find((r) => P.inRing(state.eye.at, r.ring)) : null;
    $('minicap').textContent = room ? `${roomName(room)} · ${m2(room.net_area)}` : 'Outside';
  });
}

// The four elevations on one sheet, at one scale.
function renderElevations() {
  if (state.mode !== 'elevations') return;
  const svg = $('elev'), box = svg.getBoundingClientRect(), size = [Math.max(200, box.width - 36), Math.max(200, box.height - 36)];
  svg.setAttribute('viewBox', `-18 -18 ${size[0] + 36} ${size[1] + 36}`);
  svg.innerHTML = P.elevationSheet(doc(), state.reading, { size, context: state.show.surroundings ? around() : null }).svg;
}

const drawn = () => state.mode === 'plan' || state.mode === 'elevations' || state.mode === 'street';
function setMode(mode) {
  state.mode = mode;
  $('plan').toggleAttribute('hidden', mode !== 'plan'), $('elev').toggleAttribute('hidden', mode !== 'elevations'), $('street').toggleAttribute('hidden', mode !== 'street');
  $('solid').hidden = drawn(), $('options').hidden = mode !== 'plan';
  $('minimap').hidden = mode !== 'walk';
  if (state.solid) for (const key of [...state.solid.keys]) state.solid.press(key, false);
  renderTop(), renderSunbar();
  renderPanel();
  if (mode === 'plan') renderPlan(); else if (mode === 'elevations') renderElevations(); else if (mode === 'street') { renderStreet(); readStreet(); } else renderSolid().then(renderMini);
}

// ---- files ---------------------------------------------------------------------------------------------------------------------

const slug = (name) => (name || 'a-house').toLowerCase().normalize('NFKD').replace(/[^a-z0-9]+/g, '-').replace(/^-+|-+$/g, '').slice(0, 60) || 'a-house';

function begin(next, note) {
  history.reset(next);
  state.storey = 0, state.selection = null, state.view = null, state.context = null, state.contextNote = '', state.entered = false;
  if (state.solid) state.solid.framed = false;
  changed();
  if (doc().site.place) surroundings();
  say(note);
}

async function renderFile() {
  const el = $('filemenu');
  try { state.projects = (await (await fetch('/api/projects')).json()).projects || []; } catch { state.projects = []; }
  el.innerHTML = `<button data-file="new">A new building</button><button data-file="example">The example house</button><div class="rule"></div>
    <button data-file="keep">Keep it here</button><button data-file="export">Save a copy as a file</button><button data-file="import">Open a file…</button>
    ${state.projects.length ? `<div class="rule"></div><div class="label">Kept here</div>${state.projects.map((p) => `<button data-open="${p.id}">${esc(p.name)} <small class="sub">${p.saved.slice(0, 10)}</small></button>`).join('')}` : ''}`;
}

async function fileAct(el) {
  const d = el.dataset;
  $('filemenu').hidden = true;
  if (d.file === 'new') begin(P.newBuilding(), 'A new building: mark a lot, or begin with a room.');
  else if (d.file === 'example') begin(P.exampleHouse(), 'The example house.');
  else if (d.file === 'export') {
    const a = document.createElement('a');
    a.href = URL.createObjectURL(new Blob([JSON.stringify(doc(), null, 1)], { type: 'application/json' }));
    a.download = `${slug(doc().name)}.building.json`;
    a.click();
    URL.revokeObjectURL(a.href);
  } else if (d.file === 'import') $('pick').click();
  else if (d.file === 'keep') {
    try {
      const reply = await fetch(`/api/projects/${slug(doc().name)}`, { method: 'PUT', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(doc()) });
      const body = await reply.json();
      say(reply.ok ? `Kept as “${doc().name}”.` : cap(body.message) + '.', !reply.ok);
    } catch { say('The server did not answer.', true); }
  } else if (d.open) {
    try { begin(P.opened(await (await fetch(`/api/projects/${d.open}`)).json()), 'Opened.'); } catch (error) { say(cap(error.message) + '.', true); }
  }
}

// ---- everything, and the wiring ------------------------------------------------------------------------------------------------

function renderAll() {
  const sel = state.selection, s = storey();
  if (sel && ((sel.type === 'wall' && !s.walls[sel.index]) || (sel.type === 'opening' && !s.openings[sel.index]) || (sel.type === 'vertex' && !s.vertices[sel.index])
    || (sel.type === 'room' && !(R() && R().sound && R().rooms[sel.index])) || (sel.type === 'stair' && !doc().stairs[sel.index]) || (sel.type === 'piece' && !(doc().furniture || [])[sel.index]))) state.selection = null;
  renderTop(), renderNotice(), renderInspector(), renderPanel(), renderSunbar();
  if (state.mode === 'plan') renderPlan(); else if (state.mode === 'elevations') renderElevations(); else if (state.mode === 'street') renderStreet(); else renderSolid();
}

async function build() {
  say('Building the engine; under a minute.');
  try { await fetch('/setup/run', { method: 'POST' }); } catch { return say('The server did not answer.', true); }
  for (let i = 0; i < 90; i++) {
    await new Promise((done) => setTimeout(done, 2000));
    try { if ((await (await fetch('/api/status')).json()).engine.current) { say('The engine is built.'); await vocabulary(); return read(); } } catch { /* the server is busy building */ }
  }
  say('The engine was not built: see the server\'s output.', true);
}

async function vocabulary() {
  try {
    const v = await (await fetch('/api/vocabulary')).json();
    if (v.kinds) state.kinds = v.kinds, state.elements = v.elements;
  } catch { /* without the engine there are no uses to offer; the notice says why */ }
}

function keys(e) {
  if (e.target instanceof HTMLInputElement || e.target instanceof HTMLSelectElement) { if (e.key === 'Enter') e.target.blur(); return; }
  const key = e.key.toLowerCase();
  if (walking() && state.solid && !e.ctrlKey && !e.metaKey && WALK_KEYS.has(key)) { e.preventDefault(); return state.solid.press(key, true); }
  if ((key === '[' || key === ']') && state.selection && state.selection.type === 'piece') {
    const piece = doc().furniture[state.selection.index];
    return change('setPiece', { piece: state.selection.index, bearing: ((piece.bearing + (key === '[' ? 15 : -15) + 540) % 360) - 180 }, true);
  }
  if ((e.ctrlKey || e.metaKey) && key === 'z') { e.preventDefault(); const name = e.shiftKey ? history.redo() : history.undo(); if (name) { state.selection = null; changed(); say(`${e.shiftKey ? 'Done again' : 'Undone'}: ${said(name)}.`); } return; }
  if ((e.ctrlKey || e.metaKey) && key === 'y') { e.preventDefault(); const name = history.redo(); if (name) { state.selection = null; changed(); say(`Done again: ${said(name)}.`); } return; }
  if (e.ctrlKey || e.metaKey) return;
  if (key === 'escape' && state.placing) { state.placing = null; state.ghost = null; renderPlan(); return renderPanel(); }
  if (key === 'escape') { if (state.chain || state.lot || state.ghost) { state.chain = null, state.lot = null, state.ghost = null; renderPlan(); } else if (state.selection) { state.selection = null; renderPlan(), renderInspector(); } else setTool('select'); return; }
  if (key === 'enter' && state.tool === 'lot') return closeLot();
  if (key === 'delete' || key === 'backspace') { e.preventDefault(); return removeSelection(); }
  if (walking()) return;
  if (key === 'f') return state.mode === 'plan' ? fitView() : state.solid && state.solid.frame();
  const tool = TOOLS.find((t) => t[2] && t[2].toLowerCase() === key);
  if (tool) { if (state.mode !== 'plan') setMode('plan'); setTool(tool[0]); }
}

const WALK_KEYS = new Set(['w', 'a', 's', 'd', 'q', 'e', 'arrowup', 'arrowdown', 'arrowleft', 'arrowright', 'shift']);

async function start() {
  if (localStorage.getItem('noetarium.design-studio.night') === '1') document.documentElement.classList.add('night');
  $('theme').textContent = document.documentElement.classList.contains('night') ? 'Day' : 'Night';
  $('tabs').addEventListener('click', (e) => { const b = e.target.closest('[data-tab]'); if (b) { state.panel = b.dataset.tab; renderPanel(); $('panel').scrollTop = 0; renderPlan(); } });
  $('tools').addEventListener('click', (e) => { const b = e.target.closest('[data-tool]'); if (b) { if (state.mode !== 'plan') setMode('plan'); setTool(b.dataset.tool); } });
  $('options').addEventListener('click', (e) => { const b = e.target.closest('[data-wall]'); if (b) { state.wall = b.dataset.wall; renderTools(); } });
  $('mode').addEventListener('click', (e) => { const b = e.target.closest('[data-mode]'); if (b) setMode(b.dataset.mode); });
  $('storeys').addEventListener('click', (e) => {
    const b = e.target.closest('[data-storey]');
    if (!b) return;
    if (b.dataset.storey === 'add') {
      const top = state.reading && state.reading.storeys[doc().storeys.length - 1];
      const outer = top && top.sound ? top.walls.map((w, i) => (w.outer ? i : -1)).filter((i) => i >= 0) : [];
      if (change('addStorey', { copy: 'outer', outer })) { state.storey = doc().storeys.length - 1; renderAll(); say('A storey above, on the outer walls of the one below.'); }
    } else { state.storey = Number(b.dataset.storey), state.selection = null, state.chain = null; renderAll(); renderMini(); }
  });
  $('name').addEventListener('change', (e) => change('declare', { path: ['name'], value: e.target.value }, true));
  $('undo').addEventListener('click', () => { const name = history.undo(); if (name) { state.selection = null; changed(); say(`Undone: ${said(name)}.`); } });
  $('redo').addEventListener('click', () => { const name = history.redo(); if (name) { state.selection = null; changed(); say(`Done again: ${said(name)}.`); } });
  $('file').addEventListener('click', async () => { const el = $('filemenu'); if (el.hidden) await renderFile(); el.hidden = !el.hidden; $('file').setAttribute('aria-expanded', String(!el.hidden)); });
  $('filemenu').addEventListener('click', (e) => { const b = e.target.closest('button'); if (b) fileAct(b); });
  document.addEventListener('pointerdown', (e) => { if (!e.target.closest('.menu')) $('filemenu').hidden = true; });
  $('pick').addEventListener('change', async (e) => {
    const file = e.target.files[0];
    e.target.value = '';
    if (!file) return;
    try { begin(P.opened(JSON.parse(await file.text())), `Opened ${file.name}.`); } catch (error) { say(`${file.name} is ${error.message}.`, true); }
  });
  $('pickmodel').addEventListener('change', (e) => { const file = e.target.files[0]; e.target.value = ''; if (file) ownModel(file); });
  $('theme').addEventListener('click', () => {
    const night = document.documentElement.classList.toggle('night');
    localStorage.setItem('noetarium.design-studio.night', night ? '1' : '0');
    $('theme').textContent = night ? 'Day' : 'Night';
    if (state.solid) state.solid.setTheme(night), renderSolid();
    renderMini();
  });
  $('notice').addEventListener('click', (e) => { if (e.target.closest('[data-act="build"]')) build(); });
  const svg = $('plan');
  svg.addEventListener('pointerdown', pointerDown);
  svg.addEventListener('pointermove', pointerMove);
  svg.addEventListener('pointerup', pointerUp);
  svg.addEventListener('pointerleave', () => { if (!state.drag && state.ghost && !state.chain && !state.lot) { state.ghost = null; renderPlan(); } });
  svg.addEventListener('wheel', wheel, { passive: false });
  svg.addEventListener('contextmenu', (e) => e.preventDefault());
  svg.addEventListener('dblclick', () => { if (state.tool === 'wall') { state.chain = null, state.ghost = null; renderPlan(); } else if (state.tool === 'lot') closeLot(); });
  $('inspector').addEventListener('change', (e) => { const el = e.target.closest('[data-act]'); if (el) inspectorAct(el.dataset.act, el.value, el); });
  $('inspector').addEventListener('click', (e) => { const el = e.target.closest('button[data-act]'); if (el) inspectorAct(el.dataset.act, el.value, el); });
  $('panel').addEventListener('change', (e) => panelChange(e.target));
  $('panel').addEventListener('click', (e) => { const el = e.target.closest('button, tr.pick, .finding.pick'); if (el) panelClick(el); });
  $('panel').addEventListener('keydown', (e) => { if (e.key === 'Enter' && e.target.id === 'place') findPlace(e.target.value); });
  $('sunbar').addEventListener('input', (e) => {
    const d = e.target.dataset;
    if (d.sun === 'hour') state.sun.hour = Number(e.target.value);
    else if (d.sun === 'day') state.sun.day = Number(e.target.value);
    else if (d.sun === 'roof') state.show.roof = e.target.checked;
    else if (d.sun === 'whole') state.show.whole = e.target.checked;
    else if (d.sun === 'lamps') state.show.lamps = e.target.checked;
    else if (d.sun === 'cut') state.show.cut = e.target.checked;
    else return;
    if (d.sun === 'day') read().then(() => { if (state.mode === 'street') readStreet(); });
    if (state.mode === 'street') { renderSunbar(); renderStreet(); const again = $('sunbar').querySelector(`[data-sun="${d.sun}"]`); if (again) again.focus(); return; }
    if (d.sun === 'hour' || d.sun === 'day') { const sun = sunNow(); state.solid && state.solid.setSun(sun.altitude, sun.azimuth); renderSunbar(); const again = $('sunbar').querySelector(`[data-sun="${d.sun}"]`); if (again) again.focus(); }
    else renderSolid();
  });
  $('sunbar').addEventListener('click', (e) => { if (e.target.dataset.sun === 'frame' && state.solid) state.solid.frame(); });
  window.addEventListener('keydown', keys);
  window.addEventListener('keyup', (e) => { if (state.solid && WALK_KEYS.has(e.key.toLowerCase())) state.solid.press(e.key.toLowerCase(), false); });
  window.addEventListener('blur', () => { if (state.solid) for (const key of [...state.solid.keys]) state.solid.press(key, false); });
  $('mini').addEventListener('click', (e) => {
    if (!state.solid || !state.miniView) return;
    const r = $('mini').getBoundingClientRect();
    state.solid.stand(P.toWorld(state.miniView, [260, 200])([(e.clientX - r.left) * 260 / r.width, (e.clientY - r.top) * 200 / r.height]));
  });
  window.addEventListener('resize', () => { if (state.mode === 'plan') renderPlan(); else if (state.mode === 'elevations') renderElevations(); else if (state.mode === 'street') renderStreet(); else state.solid && state.solid.resize(); });
  $('street').addEventListener('pointerdown', (e) => {
    const r = state.street.reading, box = $('street').getBoundingClientRect();
    const y = (e.clientY - box.top) / box.height * (state.street.mapBottom + 56);
    if (!r || !state.street.at || y < state.street.mapTop || y > state.street.mapBottom) return;
    state.street.station = Math.max(0, Math.min(r.stations.length - 1, Math.floor(state.street.at(e.clientX - box.left) / r.length * r.stations.length)));
    renderStreet(), renderPanel();
  });

  renderTools(), renderTop(), renderInspector(), renderTabs();
  try { state.schemes = (await (await fetch('/api/schemes')).json()).schemes || []; } catch { state.schemes = []; }
  await vocabulary();
  fitView();
  if (doc().site.place) surroundings();
  await read();
}

// What a script driving the page may hold: the document, the reading, and the same operations the hand uses.
window.studio = { state, history, change, read, setTool, setMode, fitView, goTo, facingStreet, renderAll, plan: P };

start();
