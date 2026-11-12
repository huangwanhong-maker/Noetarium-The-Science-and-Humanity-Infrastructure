// Checks of a building drawn by hand (visualization/js/plan): that a storey stays a plane graph whatever is drawn or moved (walls
// cut where they meet, corners merged, walls made one again), that openings keep their place on the ground, that every operation
// leaves the document it was given untouched and a history restores it exactly, what a pointer snaps to, the plan's drawing and
// what lies under a point, the pieces a wall is cut into in the round; and what stands in the rooms: the catalogue, a piece's
// footprint and the floor it needs, where it snaps to, its operations, its symbol on the plan, and walking among the pieces and the
// walls; and the outside: the roof and the shell as the document keeps them, and the elevations. Called by run-tests.mjs with the
// suite's check and assert.

import { plan as P } from '../js/index.js';

const box = (x0, y0, x1, y1) => [[x0, y0], [x1, y0], [x1, y1], [x0, y1]];
const has = (s, p) => s.vertices.some((v) => P.dist(v, p) < 1e-9);
const joined = (s, p, q) => s.walls.some((w) => (P.dist(s.vertices[w.a], p) < 1e-9 && P.dist(s.vertices[w.b], q) < 1e-9) || (P.dist(s.vertices[w.a], q) < 1e-9 && P.dist(s.vertices[w.b], p) < 1e-9));
// A storey is a plane graph when no two walls cross, none passes through a vertex, none repeats and none has no length.
function planar(s) {
  for (let i = 0; i < s.walls.length; i++) {
    const [a, b] = P.wallEnds(s, i);
    if (P.dist(a, b) < 1e-6) return false;
    for (let v = 0; v < s.vertices.length; v++) {
      if (v === s.walls[i].a || v === s.walls[i].b) continue;
      const pr = P.project(s.vertices[v], a, b);
      if (pr.distance < 1e-6 && pr.t > 1e-6 && pr.t < 1 - 1e-6) return false;
    }
    for (let j = i + 1; j < s.walls.length; j++) {
      const [c, d] = P.wallEnds(s, j);
      if (P.crossing(a, b, c, d)) return false;
      if ((s.walls[i].a === s.walls[j].a && s.walls[i].b === s.walls[j].b) || (s.walls[i].a === s.walls[j].b && s.walls[i].b === s.walls[j].a)) return false;
    }
  }
  return true;
}
// The faces of a connected plane graph by Euler's formula: E − V + 2, the outside among them.
const faces = (s) => s.walls.length - s.vertices.length + 2;

export function planChecks({ assert, check }) {
  // ---- drawing ----
  {
    let doc = P.newBuilding();
    doc = P.apply(doc, 'addRoom', { storey: 0, ring: box(0, 0, 4, 4), t: 0.25 });
    const s0 = doc.storeys[0];
    assert('plan: a room drawn is four corners and four walls', s0.vertices.length === 4 && s0.walls.length === 4 && planar(s0));
    const cut = P.apply(doc, 'addWall', { storey: 0, from: [2, -1], to: [2, 5], t: 0.11 }).storeys[0];
    assert('plan: a wall drawn across a room is cut where it meets the walls, and they where it meets them: 8 corners, 9 walls, two rooms',
      cut.vertices.length === 8 && cut.walls.length === 9 && planar(cut) && faces(cut) === 3 && has(cut, [2, 0]) && has(cut, [2, 4]) && joined(cut, [2, 0], [2, 4]) && joined(cut, [2, -1], [2, 0]));
    let x = P.apply(doc, 'addWall', { storey: 0, from: [0, 0], to: [4, 4], t: 0.11 });
    x = P.apply(x, 'addWall', { storey: 0, from: [0, 4], to: [4, 0], t: 0.11 });
    assert('plan: two diagonals meet at a corner made for them: four walls from the middle, four rooms',
      has(x.storeys[0], [2, 2]) && x.storeys[0].walls.length === 8 && planar(x.storeys[0]) && faces(x.storeys[0]) === 5);
    const twice = P.apply(doc, 'addWall', { storey: 0, from: [0, 0], to: [4, 0], t: 0.25 });
    assert('plan: a wall drawn over one that stands adds nothing', twice.storeys[0].walls.length === 4);
    const over = P.apply(doc, 'addWall', { storey: 0, from: [-1, 0], to: [6, 0], t: 0.25 }).storeys[0];
    assert('plan: a wall drawn along one and beyond it adds only what is beyond', over.walls.length === 6 && joined(over, [-1, 0], [0, 0]) && joined(over, [4, 0], [6, 0]) && planar(over));
    const beside = P.apply(doc, 'addRoom', { storey: 0, ring: box(4, 1, 7, 3), t: 0.25 }).storeys[0];
    assert('plan: a room drawn against another shares its wall, which is split at the new corners', beside.walls.length === 9 && beside.vertices.length === 8 && planar(beside) && faces(beside) === 3 && joined(beside, [4, 1], [4, 3]));
    assert('plan: an operation leaves the document it was given as it was', doc.storeys[0].walls.length === 4 && JSON.stringify(doc) === JSON.stringify(P.apply(P.newBuilding(), 'addRoom', { storey: 0, ring: box(0, 0, 4, 4), t: 0.25 })));
    let refused = '';
    try { P.apply(doc, 'addWall', { storey: 0, from: [1, 1], to: [1, 1] }); } catch (error) { refused = error.message; }
    assert('plan: a wall without a length is refused, with the reason', refused === 'a wall has a length');
    try { P.apply(doc, 'paint', {}); refused = ''; } catch (error) { refused = error.message; }
    assert('plan: there is no operation but those declared', refused === 'no such operation: paint' && Object.values(P.OPERATIONS).every((op) => op.title && op.params && typeof op.apply === 'function'));
  }

  // ---- moving and removing ----
  {
    let doc = P.apply(P.newBuilding(), 'addRoom', { storey: 0, ring: box(0, 0, 10, 8), t: 0.2 });
    doc = P.apply(doc, 'addWall', { storey: 0, from: [6, 0], to: [6, 8], t: 0.1 });
    const s = () => doc.storeys[0];
    const inner = () => s().walls.findIndex((w) => w.t === 0.1);
    doc = P.apply(doc, 'addOpening', { storey: 0, wall: inner(), kind: 'door', at: 3 });
    const where = (d) => P.openingSpan(d.storeys[0], d.storeys[0].openings[0]).middle;
    const before = where(doc);
    const moved = P.apply(doc, 'moveWall', { storey: 0, wall: inner(), by: [-2, 0] });
    assert('plan: a wall moved carries its ends along the walls they stand on, and the storey stays a plane graph',
      has(moved.storeys[0], [4, 0]) && has(moved.storeys[0], [4, 8]) && !has(moved.storeys[0], [6, 0]) && planar(moved.storeys[0]) && moved.storeys[0].walls.length === 7 && faces(moved.storeys[0]) === 3);
    check('plan: its door moves with it, 2 m west, and keeps its place along it', P.dist(where(moved), [before[0] - 2, before[1]]), 1e-9, 'm');
    let stretched = P.apply(doc, 'addOpening', { storey: 0, wall: s().walls.findIndex((w, i) => { const [a, b] = P.wallEnds(s(), i); return a[1] === 0 && b[1] === 0 && Math.min(a[0], b[0]) === 6; }), kind: 'window', at: 2, width: 1.2 });
    const stood = P.openingSpan(stretched.storeys[0], stretched.storeys[0].openings[1]).middle;
    stretched = P.apply(stretched, 'moveWall', { storey: 0, wall: inner(), by: [-1, 0] });
    check('plan: a window on a wall that the moved wall stretches or shortens stays where it stood on the ground', P.dist(P.openingSpan(stretched.storeys[0], stretched.storeys[0].openings.find((o) => o.kind === 'window')).middle, stood), 1e-12, 'm');
    const far = P.apply(doc, 'moveWall', { storey: 0, wall: inner(), by: [4, 0] }).storeys[0];
    assert('plan: a wall moved onto another becomes that wall: four walls again, one room', far.walls.length === 4 && far.vertices.length === 4 && planar(far));
    const gone = P.apply(doc, 'removeWall', { storey: 0, wall: inner() }).storeys[0];
    assert('plan: with the dividing wall removed, the walls it split are whole again and its door is gone', gone.walls.length === 4 && gone.vertices.length === 4 && gone.openings.length === 0);
    const corner = s().vertices.findIndex((v) => P.dist(v, [10, 8]) < 1e-9);
    const pulled = P.apply(doc, 'moveVertex', { storey: 0, vertex: corner, to: [12, 10] }).storeys[0];
    assert('plan: a corner moved takes its walls with it', has(pulled, [12, 10]) && !has(pulled, [10, 8]) && planar(pulled) && pulled.walls.length === 7);
    const merged = P.apply(doc, 'moveVertex', { storey: 0, vertex: s().vertices.findIndex((v) => P.dist(v, [6, 8]) < 1e-9), to: [10, 8] }).storeys[0];
    assert('plan: a corner moved onto another is merged with it: no wall without a length, none twice', planar(merged) && has(merged, [10, 8]) && !has(merged, [6, 8]));
    // A wall moved across another is cut by it.
    let cross = P.apply(P.newBuilding(), 'addWall', { storey: 0, from: [0, 0], to: [4, 0], t: 0.1 });
    cross = P.apply(cross, 'addWall', { storey: 0, from: [2, 1], to: [2, 3], t: 0.1 });
    const through = P.apply(cross, 'moveWall', { storey: 0, wall: 1, by: [0, -2] }).storeys[0];
    assert('plan: a wall moved across another is cut by it, and cuts it', planar(through) && has(through, [2, 0]) && through.walls.length === 4);
    // Splitting keeps an opening where it stands.
    let split = P.apply(P.newBuilding(), 'addWall', { storey: 0, from: [0, 0], to: [10, 0], t: 0.2 });
    split = P.apply(split, 'addOpening', { storey: 0, wall: 0, kind: 'window', at: 7, width: 1.2 });
    const kept = where(split);
    split = P.apply(split, 'addWall', { storey: 0, from: [4, 0], to: [4, 3], t: 0.1 });
    check('plan: a wall split by another keeps its window where it stood, now on the further part', P.dist(where(split), kept), 1e-12, 'm');
    assert('plan: ... 3 m along that part', Math.abs(split.storeys[0].openings[0].at - 3) < 1e-12 && split.storeys[0].walls.length === 3);
  }

  // ---- openings, storeys, stairs, the lot, what is declared ----
  {
    let doc = P.apply(P.newBuilding(), 'addRoom', { storey: 0, ring: box(0, 0, 4, 1), t: 0.2 });
    const short = doc.storeys[0].walls.findIndex((w, i) => Math.abs(P.wallLength(doc.storeys[0], i) - 1) < 1e-9);
    let said = '';
    try { P.apply(doc, 'addOpening', { storey: 0, wall: short, kind: 'window', at: 0.5, width: 1.5 }); } catch (error) { said = error.message; }
    assert('plan: an opening wider than its wall is refused, with both lengths', said === 'the wall is 1.00 m long: too short for an opening of 1.50 m');
    const long = doc.storeys[0].walls.findIndex((w, i) => Math.abs(P.wallLength(doc.storeys[0], i) - 4) < 1e-9);
    doc = P.apply(doc, 'addOpening', { storey: 0, wall: long, kind: 'door', at: 0.1 });
    assert('plan: an opening near a wall\'s end is set wholly on the wall', Math.abs(doc.storeys[0].openings[0].at - 0.41) < 1e-12);
    try { P.apply(doc, 'addOpening', { storey: 0, wall: long, kind: 'door', at: 0.9 }); said = ''; } catch (error) { said = error.message; }
    assert('plan: two openings do not stand in one place', said === 'another opening stands there');
    try { P.apply(doc, 'addStair', { from: 0, at: [1, 0.5] }); said = ''; } catch (error) { said = error.message; }
    assert('plan: a stair is refused where there is no storey above', said.startsWith('a stair rises to a storey above'));
    const two = P.apply(doc, 'addStorey', { copy: 'all' });
    assert('plan: a storey above begins on the walls of the one below, at its heights', two.storeys.length === 2 && two.storeys[1].walls.length === 4 && two.storeys[1].height === two.storeys[0].height && P.elevation(two, 1) === 3);
    const stair = P.apply(two, 'addStair', { from: 0, at: [1, 0.5], bearing: 0 });
    const plan = P.stairPlan(stair.stairs[0]);
    check('plan: a stair of 17 risers and a going of 0.25 m runs 4 m, east when its bearing is 0', P.dist(plan.head, [5, 0.5]), 1e-12, 'm');
    assert('plan: removing the storey a stair rises to removes the stair', P.apply(stair, 'removeStorey', { storey: 1 }).stairs.length === 0);
    const lot = P.apply(doc, 'setLot', { ring: box(-2, -2, 13, 28), street: [true] });
    assert('plan: a lot keeps for each edge whether it faces a street', lot.site.lot.street.join() === 'true,false,false,false' && P.apply(lot, 'setStreet', { edge: 2, street: true }).site.lot.street[2] === true);
    const own = P.apply(P.apply(doc, 'declare', { path: ['siting', 'height', 'most'], value: 9 }), 'declare', { path: ['brief', 'needs', 0, 'count'], value: 2 });
    assert('plan: a limit and a need are declared where they belong, and withdrawn again', own.siting.height.most === 9 && own.brief.needs[0].count === 2
      && P.apply(own, 'declare', { path: ['siting', 'height', 'most'], value: undefined }).siting.height.most === undefined);
    try { P.apply(doc, 'declare', { path: ['storeys', 0, 'walls'], value: [] }); said = ''; } catch (error) { said = error.message; }
    assert('plan: nothing drawn can be declared away', said === 'nothing is declared at storeys.0.walls');
  }

  // ---- a history ----
  {
    const h = new P.History(P.newBuilding());
    const empty = JSON.stringify(h.doc);
    h.do('addRoom', { storey: 0, ring: box(0, 0, 5, 4), t: 0.25 });
    const one = JSON.stringify(h.doc);
    h.do('addWall', { storey: 0, from: [2, 0], to: [2, 4], t: 0.11 });
    const two = JSON.stringify(h.doc);
    let threw = false;
    try { h.do('addOpening', { storey: 0, wall: 99 }); } catch { threw = true; }
    assert('plan: an operation that fails changes nothing, and is not in the history', threw && JSON.stringify(h.doc) === two && h.past.length === 2);
    assert('plan: undo names what it took back and restores the document exactly', h.undo() === 'addWall' && JSON.stringify(h.doc) === one && h.undo() === 'addRoom' && JSON.stringify(h.doc) === empty && h.undo() === null);
    assert('plan: redo brings it back exactly', h.redo() === 'addRoom' && h.redo() === 'addWall' && JSON.stringify(h.doc) === two && h.redo() === null);
    h.undo();
    h.group('Two walls', [['addWall', { storey: 0, from: [0, 2], to: [5, 2], t: 0.11 }], ['addWall', { storey: 0, from: [3, 0], to: [3, 4], t: 0.11 }]]);
    assert('plan: several operations as one step are undone together', h.undo() === 'Two walls' && JSON.stringify(h.doc) === one);
    let all = true;
    try { h.group('One that fails', [['addWall', { storey: 0, from: [0, 3], to: [5, 3] }], ['removeWall', { storey: 0, wall: 99 }]]); all = false; } catch { /* refused */ }
    assert('plan: a step of which one operation fails is not taken at all', all && JSON.stringify(h.doc) === one);
  }

  // ---- the example, and a document opened ----
  {
    const doc = P.exampleHouse(), s = doc.storeys[0];
    assert('plan: the example house is a plane graph of seven rooms, by Euler\'s formula, with 29 pieces in them and six in its garden', planar(s) && faces(s) === 8 && s.labels.length === 7 && s.openings.length === 14 && doc.furniture.length === 35 && doc.furniture.filter((f) => f.outdoor).length === 6);
    assert('plan: every opening of it lies wholly on its wall', s.openings.every((o) => o.at - o.width / 2 >= -1e-9 && o.at + o.width / 2 <= P.wallLength(s, o.wall) + 1e-9));
    const back = P.opened(JSON.parse(JSON.stringify(doc)));
    assert('plan: a document written and read back is the same document', JSON.stringify(back) === JSON.stringify(doc));
    let said = '';
    try { P.opened({ schema: 'something/1' }); } catch (error) { said = error.message; }
    assert('plan: a file of another schema is refused', said === 'not a building of the schema noetarium.building/1');
    assert('plan: the rates that come with a new building say they are presumed', P.newBuilding().rates.presumed === true && Object.keys(P.PRESUMED_RATES.items).length === 11);
  }

  // ---- snapping ----
  {
    const s = P.apply(P.newBuilding(), 'addRoom', { storey: 0, ring: box(0, 0, 4, 4), t: 0.25 }).storeys[0];
    const v = P.snap([3.93, 4.06], s, { reach: 0.2 }), w = P.snap([2.02, 0.07], s, { reach: 0.2 }), g = P.snap([1.52, 1.98], s, { reach: 0.2, step: 0.05 });
    assert('plan: a pointer snaps to a corner before a wall, to a wall before the grid', v.kind === 'vertex' && P.dist(v.point, [4, 4]) < 1e-12 && w.kind === 'wall' && P.dist(w.point, [2, 0]) < 1e-9 && g.kind === 'grid' && P.dist(g.point, [1.5, 2]) < 1e-9);
    const turned = P.snap([0.72, 0.7], null, { step: 1, angle: Math.PI / 4 });
    check('plan: on a grid of 1 m turned 45°, the nearest crossing to (0.72, 0.70) is one step along its first axis', P.dist(turned.point, [Math.SQRT1_2, Math.SQRT1_2]), 1e-12, 'm');
    check('plan: a wall drawn within 8° of an axis is squared to it, keeping its length along it', P.dist(P.squared([0, 0], [5, 0.4]), [5, 0]), 1e-12, 'm');
    assert('plan: ... and one drawn at 30° is left as it is', P.dist(P.squared([0, 0], [5, 2.9]), [5, 2.9]) === 0);
    check('plan: a point well inside an L is found for its label, though its centroid may not be', P.inRing(P.interiorPoint([[0, 0], [6, 0], [6, 1], [1, 1], [1, 6], [0, 6]]), [[0, 0], [6, 0], [6, 1], [1, 1], [1, 6], [0, 6]]) ? 0 : 1, 0);
  }

  // ---- the drawing, and what lies under a point ----
  {
    const doc = P.exampleHouse(), size = [900, 700], view = P.fit(doc.storeys[0].vertices, size, 80);
    const S = P.toScreen(view, size), W = P.toWorld(view, size), q = W(S([3.3, 7.7]));
    check('plan: the ground to the page and back is the identity', P.dist(q, [3.3, 7.7]), 1e-12, 'm');
    const corners = doc.storeys[0].vertices.map(S);
    assert('plan: a view fitted to the house holds every corner within its margin', corners.every((c) => c[0] >= 79.9 && c[0] <= size[0] - 79.9 && c[1] >= 79.9 && c[1] <= size[1] - 79.9));
    const svg = P.planSvg({ doc, reading: null, storey: 0, view, size, selection: { type: 'wall', index: 0 }, ghost: null, context: null });
    assert('plan: the drawing has a line for every wall, a mark for every opening, the lot and its street, north and a scale; one wall is in hand',
      (svg.match(/class="wall/g) || []).length === doc.storeys[0].walls.length && (svg.match(/class="opening /g) || []).length === 14 && svg.includes('class="lot"') && svg.includes('>street<')
      && svg.includes('class="north"') && / m<\/text>/.test(svg) && (svg.match(/class="wall[^"]* sel"/g) || []).length === 1);
    assert('plan: without a reading no room is drawn, and nothing is said of what is not known', !svg.includes('class="room'));
    const wall = P.hit(doc, null, 0, [5, 16.05], 0.1), door = P.hit(doc, null, 0, [6.5, 11.02], 0.1), corner = P.hit(doc, null, 0, [2.03, 6.02], 0.1), none = P.hit(doc, null, 0, [9, 7], 0.1);
    const table = P.hit(doc, null, 0, [4.1, 8.5], 0.1), rug = P.hit(doc, null, 0, [5, 8], 0.1);
    assert('plan: under a point on a table that stands on a rug lies the table, the smaller; beside it, the rug; with pieces left out, neither',
      table.type === 'piece' && doc.furniture[table.index].kind === 'coffee-table' && rug.type === 'piece' && doc.furniture[rug.index].kind === 'rug' && P.hit(doc, null, 0, [5, 8], 0.1, { pieces: false }) === null);
    assert('plan: under a point lie a corner, then an opening, then a wall, in that order; in a room, without a reading, nothing',
      corner.type === 'vertex' && door.type === 'opening' && doc.storeys[0].openings[door.index].kind === 'door' && wall.type === 'wall' && none === null);
    assert('plan: an edge of the lot is found when it is asked for', P.hit(doc, null, 0, [7, 0.05], 0.1, { lot: true }).type === 'edge' && P.hit(doc, null, 0, [7, 0.05], 0.1) === null);
    const sofa = doc.furniture.findIndex((f) => f.kind === 'sofa-3');
    const drawn = P.planSvg({ doc, reading: null, storey: 0, view, size, selection: { type: 'piece', index: sofa }, ghost: null, context: null, eye: { at: [5, 9], heading: 0, storey: 0 } });
    assert('plan: the drawing has a symbol for every piece; the one in hand shows the floor it needs; the eye of one who walks is marked on its storey only',
      (drawn.match(/class="piece/g) || []).length === 35 && (drawn.match(/class="piece[^"]* sel"/g) || []).length === 1 && (drawn.match(/class="space"/g) || []).length === 1 && drawn.includes('class="eye"')
      && !P.planSvg({ doc, reading: null, storey: 0, view, size, selection: null, ghost: null, context: null, eye: { at: [5, 9], heading: 0, storey: 1 } }).includes('class="eye"')
      && !P.planSvg({ doc, reading: null, storey: 0, view, size, selection: null, ghost: null, context: null, show: { furniture: false } }).includes('class="piece'));
  }

  // ---- what stands in the rooms ----
  {
    const kinds = P.CATALOGUE.map((c) => c.kind);
    assert('plan: the catalogue names each kind once, in a group that is listed, with a size', new Set(kinds).size === kinds.length && P.CATALOGUE.every((c) => P.GROUPS.some(([g]) => g === c.group) && c.width > 0 && c.depth > 0 && c.height > 0 && c.title));
    const symbols = P.CATALOGUE.map((c) => [c, P.symbol(P.newPiece(c.kind, 0, [0, 0]))]);
    assert('plan: every kind has a symbol of finite marks, none reaching more than its chair beyond its footprint',
      symbols.every(([c, marks]) => marks.length > 0 && marks.every((mk) => mk.points.every((q) => Number.isFinite(q[0]) && Number.isFinite(q[1]) && Math.abs(q[0]) <= c.width / 2 + 1e-9 && Math.abs(q[1]) <= c.depth / 2 + 0.55))));
    assert('plan: every kind but a model of one\'s own is built in the round', kinds.filter((k) => k !== 'own').every((k) => P.MODELLED.includes(k)) && !P.MODELLED.includes('own'));
    assert('plan: a cloth\'s finish is one of the eight named', P.CATALOGUE.every((c) => !c.finish || P.FINISHES[c.finish]) && Object.keys(P.FINISHES).length === 8);
    // A bed 1.6 × 2.0 facing south at (2.5, 2.1), as the domain's check has it.
    const bed = { ...P.newPiece('bed-double', 0, [2.5, 2.1], -90), width: 1.6, depth: 2.0, front: 0.6, side: 0.5 };
    const span = (ring, a, hi) => ring.reduce((v, q) => (hi ? Math.max(v, q[a]) : Math.min(v, q[a])), ring[0][a]);
    const ring = P.pieceRing(bed), spaces = P.pieceSpaces(bed);
    check('plan: a bed facing south stands on [1.7, 3.3] × [1.1, 3.1], counter-clockwise', Math.abs(span(ring, 0, false) - 1.7) + Math.abs(span(ring, 0, true) - 3.3) + Math.abs(span(ring, 1, false) - 1.1) + Math.abs(span(ring, 1, true) - 3.1) + Math.abs(P.ringArea(ring) - 3.2), 1e-12);
    check('plan: it needs its foot\'s 0.6 m on [0.5, 1.1] and its two sides\' 0.5 m', (spaces.length === 3 ? 0 : 1) + Math.abs(span(spaces[0], 1, false) - 0.5) + Math.abs(span(spaces[0], 1, true) - 1.1) + Math.abs(span(spaces[1], 0, false) - 1.2) + Math.abs(span(spaces[2], 0, true) - 3.8), 1e-12);
    const island = P.newPiece('island', 0, [0, 0], 0), about = P.pieceSpaces(island);
    check('plan: the floor all round an island is the ring between it and itself grown by 0.8 m', about.reduce((a, r) => a + Math.abs(P.ringArea(r)), 0) - ((2.0 + 1.6) * (0.95 + 1.6) - 2.0 * 0.95), 1e-12, 'm²');
    assert('plan: a point is in a piece by its own frame, however it is turned', P.inPiece(bed, [1.75, 3.05]) && !P.inPiece(bed, [1.65, 2]) && P.inPiece(bed, [1.65, 2], 0.1));
    const back = P.pieceLocal(bed, P.pieceAt(bed, 0.3, -0.4));
    check('plan: a piece\'s frame to the ground and back is the identity', Math.hypot(back[0] - 0.3, back[1] + 0.4), 1e-12, 'm');

    // Snapping: against a wall, its back to it.
    const room = P.apply(P.newBuilding(), 'addRoom', { storey: 0, ring: box(0, 0, 5, 4), t: 0.25 }), s = room.storeys[0];
    const sofa = P.newPiece('sofa-3', 0, [0, 0], 0), near = P.snapPiece(s, sofa, [2.52, 0.5]), middle = P.snapPiece(s, sofa, [2.52, 2.0]), free = P.snapPiece(s, sofa, [2.52, 0.5], { against: false });
    check('plan: a sofa held near the south wall goes flush against its face, its back to it, facing north', Math.hypot(near.at[0] - 2.5, near.at[1] - (0.125 + 0.475)) + Math.abs(near.bearing - 90), 1e-9);
    assert('plan: ... held in the middle of the room it stays on the grid as it was turned; a piece that does not stand against walls does so anywhere', middle.wall === null && middle.bearing === 0 && P.dist(middle.at, [2.5, 2]) < 1e-9 && free.wall === null);
    const east = P.snapPiece(s, sofa, [4.3, 2.0]);
    check('plan: near the east wall it faces west', Math.hypot(east.at[0] - (5 - 0.125 - 0.475), east.at[1] - 2) + Math.abs(Math.abs(east.bearing) - 180), 1e-9);

    // The operations.
    let doc = P.apply(room, 'addPiece', { storey: 0, kind: 'bed-double', at: [1.2, 2], bearing: 0 });
    assert('plan: a piece set down takes the catalogue\'s size, the floor it needs and its cloth', doc.furniture.length === 1 && doc.furniture[0].width === 1.6 && doc.furniture[0].front === 0.6 && doc.furniture[0].side === 0.5 && doc.furniture[0].finish === 'linen' && room.furniture.length === 0);
    let said = '';
    try { P.apply(room, 'addPiece', { storey: 0, kind: 'throne', at: [1, 1] }); } catch (error) { said = error.message; }
    assert('plan: a kind the catalogue does not have is refused', said === 'the catalogue has no throne');
    try { P.apply(doc, 'setPiece', { piece: 0, width: 0 }); said = ''; } catch (error) { said = error.message; }
    assert('plan: a piece keeps a width; the floor it needs is not negative', said === 'a piece\'s width is a positive number' && (() => { try { P.apply(doc, 'setPiece', { piece: 0, front: -1 }); return false; } catch { return true; } })());
    doc = P.apply(doc, 'setPiece', { piece: 0, at: [3, 2], bearing: 180, finish: 'rose' });
    assert('plan: a piece moved, turned and given another cloth is changed, and taken away again', doc.furniture[0].at[0] === 3 && doc.furniture[0].bearing === 180 && doc.furniture[0].finish === 'rose' && P.apply(doc, 'removePiece', { piece: 0 }).furniture.length === 0);
    const two = P.apply(P.apply(doc, 'addStorey', { copy: 'all' }), 'addPiece', { storey: 1, kind: 'desk', at: [2, 2] });
    const less = P.apply(two, 'removeStorey', { storey: 0 });
    assert('plan: with a storey removed, its pieces go with it and those above come down one', two.furniture.length === 2 && less.furniture.length === 1 && less.furniture[0].kind === 'desk' && less.furniture[0].storey === 0);
    const h = new P.History(room);
    h.do('addPiece', { storey: 0, kind: 'plant', at: [1, 1] });
    assert('plan: setting a piece down is undone like any other change', h.undo() === 'addPiece' && h.doc.furniture.length === 0 && h.redo() === 'addPiece' && h.doc.furniture.length === 1);
    const swung = P.apply(P.apply(room, 'addOpening', { storey: 0, wall: 0, kind: 'door', at: 2 }), 'setOpening', { storey: 0, opening: 0, swing: -1, hinge: 'b' });
    assert('plan: a door keeps the side it is drawn to swing to and the end it hangs at', swung.storeys[0].openings[0].swing === -1 && swung.storeys[0].openings[0].hinge === 'b');
  }

  // ---- walking ----
  {
    const doc = P.exampleHouse(), obs = P.obstacles(doc, 0);
    assert('plan: what stops a person in the example house: the walls\' solid stretches between the doors, and the 27 pieces that obstruct (a person passes beneath a tree)',
      obs.pieces.length === 27 && obs.walls.length > doc.storeys[0].walls.length - 1 && obs.rooms === null);
    assert('plan: one can stand in the hall, not in a wall, not in the sofa, not within the body\'s radius of a wall', P.clear(obs, [7, 11.6]) && !P.clear(obs, [7, 11.02]) && !P.clear(obs, [2.6, 8.5]) && !P.clear(obs, [7, 11.2]) && P.clear(obs, [7, 11.3]));
    const along = P.walk(obs, [3, 11.6], [12, 11.6]), stopped = P.walk(obs, [9, 11.6], [9, 14]), through = P.walk(obs, [10, 11.6], [10, 13.2]);
    check('plan: a walk along the hall goes its length', P.dist(along, [12, 11.6]), 1e-9, 'm');
    assert('plan: a walk into a wall stops at it, the body\'s radius from its face', Math.abs(stopped[0] - 9) < 0.3 && stopped[1] < 12.2 - 0.055 - 0.2 + 1e-9 && stopped[1] > 11.8);
    check('plan: a walk through a door\'s opening passes it', P.dist(through, [10, 13.2]), 1e-9, 'm');
    const around = P.walk(obs, [5.5, 8.5], [3.2, 8.5]);
    assert('plan: a walk toward the sofa ends at the table that stands before it', around[0] > 3.075 + 0.1 && around[0] < 5.5 && !P.inPiece(doc.furniture.find((f) => f.kind === 'coffee-table'), around));
    assert('plan: the nearest clear place to one inside a piece is beside it; a clear place is itself', P.clear(obs, P.nearestClear(obs, [2.6, 8.5])) && P.dist(P.nearestClear(obs, [7, 11.6]), [7, 11.6]) === 0);
    // Where one comes in: a room 5 × 4 with a door in its south wall, read as the domain would.
    let room = P.apply(P.newBuilding(), 'addRoom', { storey: 0, ring: box(0, 0, 5, 4), t: 0.25 });
    room = P.apply(room, 'addOpening', { storey: 0, wall: 0, kind: 'door', at: 2 });
    const reading = { storeys: [{ sound: true, rooms: [{ indoor: true, area: 20, ring: box(0, 0, 5, 4) }], walls: room.storeys[0].walls.map(() => ({ left: 0, right: null, outer: true })) }] };
    const way = P.entrance(room, 0, reading);
    check('plan: one comes in 0.7 m inside the door from the outside, looking in', Math.hypot(way.at[0] - 2, way.at[1] - (0.125 + 0.7)) + Math.abs(way.heading - 90), 1e-9);
    const upper = P.obstacles(P.apply(room, 'addStorey', { copy: 'all' }), 1, { storeys: [reading.storeys[0], reading.storeys[0]] });
    assert('plan: above the ground the floor ends at the rooms: one stands inside one, not beyond its walls', P.clear(upper, [2, 2]) && !P.clear(upper, [7, 2]));
    const places = P.lightPlaces(box(0, 0, 4, 3)), one = P.lightPlaces(box(0, 0, 1, 1));
    assert('plan: a room of 4 × 3 m has four ceiling lights, each half a metre clear of its edge; a small room has one', places.length === 4 && places.every((q) => q[0] > 0.5 && q[0] < 3.5 && q[1] > 0.5 && q[1] < 2.5) && one.length === 1);
  }

  // ---- the outside (ADR 0097): the roof and the shell as the document keeps them, and the elevations ----
  {
    const fresh = P.newBuilding();
    assert('plan: a new building\'s roof is flat, with no pitch and no eaves, under a membrane; its walls are rendered in chalk; its lot is open',
      P.roofOf(fresh).form === 'flat' && P.roofOf(fresh).pitch === 0 && P.roofOf(fresh).eaves === 0 && P.roofOf(fresh).cover === 'membrane' && P.shellOf(fresh).cladding === 'render' && P.shellOf(fresh).tone === 'chalk' && P.boundaryOf(fresh).kind === 'none');
    assert('plan: a roof that states a pitch and no form is a hip (a document from before); gables are kept only by a gable',
      P.roofOf({ roof: { pitch: 20 } }).form === 'hip' && P.roofOf({ roof: { form: 'hip', pitch: 20, gables: [[1, 2]] } }).gables.length === 0 && P.roofOf({ roof: { form: 'gable', pitch: 20, gables: [[1, 2], 'x'] } }).gables.length === 1);
    assert('plan: what is not a form, a cover, a cladding or a tone is given its default; a pitch is held between 1° and 60°',
      P.roofOf({ roof: { form: 'dome', pitch: 30 } }).form === 'flat' && P.roofOf({ roof: { form: 'hip', pitch: 80, cover: 'gold' } }).pitch === 60 && P.roofOf({ roof: { form: 'hip', pitch: 80, cover: 'gold' } }).cover === 'tiles' && P.shellOf({ shell: { cladding: 'glass', tone: 'neon' } }).cladding === 'render');
    const ends = P.defaultGables(box(2, 6, 13, 16)), tall = P.defaultGables(box(0, 0, 4, 9));
    assert('plan: a roof first made a gable takes the two ends of its long way', ends.length === 2 && ends[0][0] === 2 && ends[1][0] === 13 && ends[0][1] === 11 && tall[0][1] === 0 && tall[1][1] === 9 && P.defaultGables([]).length === 0);
    assert('plan: a bearing is named by the nearest of eight winds', P.windOf(-90) === 'south' && P.windOf(0) === 'east' && P.windOf(100) === 'north' && P.windOf(180) === 'west' && P.windOf(-180) === 'west' && P.windOf(-140) === 'south-west');
    const ex = P.exampleHouse();
    assert('plan: the example house has a gabled roof of tiles, weatherboards, a hedge about its garden, and is undone step by step to a bare one',
      P.roofOf(ex).form === 'gable' && P.roofOf(ex).gables.length === 2 && P.shellOf(ex).cladding === 'boards' && P.boundaryOf(ex).kind === 'hedge' && P.boundaryOf(ex).height === 1.1);
    let declared = P.apply(fresh, 'declare', { path: ['shell'], value: { cladding: 'brick', tone: 'clay' } });
    declared = P.apply(declared, 'declare', { path: ['site', 'boundary'], value: { kind: 'fence', height: 1.5 } });
    let refused = false;
    try { P.apply(fresh, 'declare', { path: ['site', 'lot'], value: null }); } catch { refused = true; }
    assert('plan: the shell and the lot\'s boundary are declared like any value; the lot itself is not', P.shellOf(declared).cladding === 'brick' && P.boundaryOf(declared).kind === 'fence' && refused && P.shellOf(fresh).cladding === 'render');

    // A room 6 × 4 m with walls 0.25 thick, 3 m high, a window in its south wall and a door in its west; over it a gabled roof at
    // 30° with eaves of 0.5 whose ridge runs east and west, as the domain would read it.
    let doc = P.apply(P.newBuilding(), 'addRoom', { storey: 0, ring: box(0, 0, 6, 4), t: 0.25 });
    const s = () => doc.storeys[0], on = (q) => { let best = -1, d = Infinity; s().walls.forEach((w, i) => { const pr = P.project(q, ...P.wallEnds(s(), i)); if (pr.distance < d) d = pr.distance, best = i; }); return { wall: best, at: P.dist(P.wallEnds(s(), best)[0], q) }; };
    doc = P.apply(doc, 'addOpening', { storey: 0, kind: 'window', ...on([2, 0]), width: 1.5, height: 1.2, sill: 0.9 });
    doc = P.apply(doc, 'addOpening', { storey: 0, kind: 'door', ...on([0, 2]), width: 0.9, height: 2.04 });
    doc = P.apply(doc, 'declare', { path: ['roof'], value: { form: 'gable', pitch: 30, eaves: 0.5, cover: 'tiles', gables: [[0, 2], [6, 2]] } });
    const inside = (i) => { const [a, b] = P.wallEnds(s(), i), m = [(a[0] + b[0]) / 2, (a[1] + b[1]) / 2], n = [-(b[1] - a[1]), b[0] - a[0]], L = Math.hypot(n[0], n[1]); return P.inRing([m[0] + n[0] / L * 0.5, m[1] + n[1] / L * 0.5], box(0, 0, 6, 4)); };
    const t30 = Math.tan(Math.PI / 6), ze = 3 - 0.5 * t30, zr = 3 + 2.125 * t30, lo = -0.625, hx = 6.625, hy = 4.625;
    const reading = {
      storeys: [{ sound: true, rooms: [{ indoor: true, kind: 'living', ring: box(0, 0, 6, 4), holes: [] }], walls: s().walls.map((w, i) => (inside(i) ? { left: 0, right: null, outer: true } : { left: null, right: 0, outer: true })) }],
      shell: { roofs: [{ storey: 0, part: 0, form: 'gable', pitch: 30, eaves: 0.5, plate: 3, top: zr, cover: 'tiles',
        pieces: [{ edge: 0, ring: [[lo, lo, ze], [hx, lo, ze], [hx, 2, zr], [lo, 2, zr]] }, { edge: 2, ring: [[hx, hy, ze], [lo, hy, ze], [lo, 2, zr], [hx, 2, zr]] }],
        lines: [{ a: [lo, 2, zr], b: [hx, 2, zr], kind: 0 }],
        outline: [[lo, lo, ze], [hx, lo, ze], [hx, 2, zr], [hx, hy, ze], [lo, hy, ze], [lo, 2, zr]],
        walls: [{ a: [6.125, -0.125], b: [6.125, 4.125], t: 0.25, top: [[0, 3], [2.125, zr], [4.25, 3]] }, { a: [-0.125, 4.125], b: [-0.125, -0.125], t: 0.25, top: [[0, 3], [2.125, zr], [4.25, 3]] }] }] },
    };
    const sides = P.elevationBearings(doc, reading);
    assert('plan: a building has four elevations, square to its longest outer wall, read from the south round by the east', sides.length === 4 && sides.map((x) => x.name).join(' ') === 'south east north west' && sides[0].bearing === -90);
    const south = P.elevationData(doc, reading, -90), walls = south.faces.filter((f) => f.kind === 'wall'), roofFaces = south.faces.filter((f) => f.kind === 'roof');
    check('plan: from the south one wall faces the eye, 6.25 m across its outer face and 3 m high', walls.length === 1 ? Math.abs(Math.max(...walls[0].points.map((q) => q[0])) - Math.min(...walls[0].points.map((q) => q[0])) - 6.25) + Math.abs(Math.max(...walls[0].points.map((q) => q[1])) - 3) : 1, 1e-12, 'm');
    assert('plan: ... with its window drawn where it stands, 1.25 to 2.75 m along, 0.9 to 2.1 m up', walls.length === 1 && walls[0].openings.length === 1 && Math.abs(walls[0].openings[0].x0 - 1.25) < 1e-9 && Math.abs(walls[0].openings[0].x1 - 2.75) < 1e-9 && Math.abs(walls[0].openings[0].z0 - 0.9) < 1e-9 && Math.abs(walls[0].openings[0].z1 - 2.1) < 1e-9);
    assert('plan: ... and the roof\'s south face, which leans toward the eye; the north face is hidden behind the ridge', roofFaces.length === 1 && roofFaces[0].edge === 0);
    check('plan: ... the drawing is as wide as the eaves and as high as the ridge', Math.abs(south.x1 - south.x0 - 7.25) + Math.abs(south.top - zr), 1e-12, 'm');
    assert('plan: ... the faces are given from the farthest to the nearest, so that the nearer hides the farther', south.faces.every((f, i) => i === 0 || f.depth <= south.faces[i - 1].depth));
    assert('plan: ... its levels are the ground, the wall plate at +3.00 and the ridge', south.levels.length === 3 && south.levels[0].name === 'ground' && south.levels[1].z === 3 && south.levels[1].name === 'wall plate' && south.levels[2].name === 'ridge');
    const west = P.elevationData(doc, reading, 180), gable = west.faces.find((f) => f.upper);
    assert('plan: from the west the door\'s wall faces the eye, with the gable carried up to the ridge above it, and no face of the roof',
      west.faces.filter((f) => f.kind === 'wall' && !f.upper).length === 1 && west.faces.find((f) => f.kind === 'wall' && !f.upper).openings[0].kind === 'door' && gable && Math.abs(Math.max(...gable.points.map((q) => q[1])) - zr) < 1e-12 && !west.faces.some((f) => f.kind === 'roof'));
    assert('plan: ... and the verge of the roof, climbing to the ridge and down again', west.faces.filter((f) => f.kind === 'fascia').length === 2);
    const north = P.elevationData(doc, reading, 90);
    assert('plan: from the north, east is on the left: nothing of the south wall\'s window is seen', north.faces.filter((f) => f.kind === 'wall').every((f) => f.openings.length === 0) && north.faces.some((f) => f.kind === 'roof' && f.edge === 2));
    const drawn = P.elevationSvg(south, { box: [0, 0, 800, 500] });
    assert('plan: an elevation is drawn with its wall, its glass, its roof and its courses, the ground, the levels and its name',
      drawn.scale > 0 && ['el-wall', 'el-glass', 'el-roof', 'el-course', 'el-ground', 'el-level', 'el-figure', 'South elevation', '+3.00', '±0.00'].every((w) => drawn.svg.includes(w)));
    const sheet = P.elevationSheet(doc, reading, { size: [1000, 700] });
    assert('plan: the four are set on one sheet at one scale', (sheet.svg.match(/class="el-side"/g) || []).length === 4 && sheet.scale > 0 && sheet.sides.length === 4);
    assert('plan: with no reading there is nothing to draw, and the sheet says so', P.elevationSheet(P.newBuilding(), null).svg.includes('nothing drawn yet'));
    const view = { cx: 3, cy: 2, scale: 40 }, roofPlan = P.planSvg({ doc, reading: null, storey: 0, view, size: [600, 400], selection: null, ghost: null, context: null, roofs: reading.shell.roofs });
    assert('plan: the plan shows the roof over it when asked: its edge and its ridge', roofPlan.includes('class="roofedge"') && roofPlan.includes('roofline ridge') && !P.planSvg({ doc, reading: null, storey: 0, view, size: [600, 400], selection: null, ghost: null, context: null }).includes('roofline'));
    const garden = P.CATALOGUE.filter((c) => c.outdoor);
    assert('plan: eight kinds stand outdoors, and a piece of such a kind says so; a person passes beneath a tree', garden.length === 8 && garden.every((c) => c.group === 'garden') && P.newPiece('tree', 0, [0, 0]).outdoor === true && P.newPiece('tree', 0, [0, 0]).obstructs === false && !('outdoor' in P.newPiece('chair', 0, [0, 0])));
  }

  // ---- the street (ADR 0098): the way in hand, its strips, its drawings ----
  {
    // A street cut at two crossings into three ways, the middle one walked backwards; a way of another name beside it; a rail.
    const ways = [{ name: 'High Street', line: [[0, 0], [50, 0]] }, { name: 'High Street', line: [[120, 0], [50, 0]] }, { name: 'High Street', line: [[120, 0], [200, 10]] },
      { name: 'Low Street', line: [[50, 0], [50, 80]] }, { name: 'High Street', line: [[0, 0], [-40, 0]], rail: true }, { name: '', line: [[200, 10], [260, 10]] }];
    const whole = P.joinWays(ways, 1);
    check('a street cut at its crossings is joined again from any of its ways, each walked the way that carries on', Math.abs(whole.line.reduce((s, q, i) => (i ? s + P.dist(q, whole.line[i - 1]) : 0), 0) - (50 + 70 + Math.hypot(80, 10))) + Math.abs(whole.before - Math.hypot(80, 10)), 1e-9, 'm');
    assert('... in one line from end to end, without the way of another name, the rail, or the way with no name', whole.line.length === 4 && P.dist(whole.line[0], [200, 10]) < 1e-9 && P.dist(whole.line[3], [0, 0]) < 1e-9 && P.joinWays(ways, 5).line.length === 2);
    const near = P.nearestWay(ways, [60, 6]);
    assert('the way nearest a point is found, a rail never', near.index === 1 && Math.abs(near.distance - 6) < 1e-9 && P.nearestWay(ways, [-20, 1]).index === 0 && P.nearestWay(ways, [500, 500]) === null);
    const part = P.stretch([[0, 0], [100, 0], [100, 100]], 90, 30);
    check('a stretch of 30 m either way of a place along a line keeps the corner between', Math.abs(part.length - 3) + P.dist(part[0], [60, 0]) + P.dist(part[1], [100, 0]) + P.dist(part[2], [100, 20]), 1e-9, 'm');
    for (const w of [5, 9, 13, 18, 30]) {
      const strips = P.presumedStrips(w), total = strips.reduce((s, x) => s + x.width, 0);
      assert(`a division presumed for ${w} m is no wider than it, of kinds that are strips, with a footpath at each face where there is room`, total <= w + 1e-9 && strips.every((s) => P.STRIPS[s.kind] && s.width > 0) && (w < 7.5 || (strips[0].kind === 'footpath' && strips[strips.length - 1].kind === 'footpath')));
    }
    const placed = P.placeStrips([{ kind: 'footpath', width: 3 }, { kind: 'lane', width: 7 }, { kind: 'footpath', width: 3 }]);
    check('strips are set out about the centreline from the left', Math.abs(placed[0].from + 6.5) + Math.abs(placed[1].from + 3.5) + Math.abs(placed[2].to - 6.5), 1e-12, 'm');
    const changed = P.withHeights([{ ring: box(0, 0, 10, 10), height: 6, presumed: true }, { ring: box(20, 0, 30, 10), height: 9 }], [{ at: [5, 5], height: 24 }]);
    assert('a building given another height is named by a point within it; the others stand as they did', changed[0].height === 24 && changed[0].changed && !changed[0].presumed && changed[1].height === 9 && !changed[1].changed);
    const left = P.offsetLine([[0, 0], [10, 0], [10, 10]], 2);
    check('a line moved 2 m to its left keeps its corner mitred', P.dist(left[0], [0, 2]) + P.dist(left[1], [8, 2]) + P.dist(left[2], [8, 10]), 1e-9, 'm');
    // The Sun in the section: due south at 45° over a street running east, it stands square on the right; from the south-east the
    // profile angle is steeper.
    const station = { left: [0, 1] };
    const square = P.profileSun(station, { altitude: 45, azimuth: 180 }), slant = P.profileSun(station, { altitude: 45, azimuth: 135 });
    check('the Sun due south over a street running east stands on its right at its own altitude; from the south-east, at atan(√2)', Math.abs(square.angle - Math.PI / 4) + Math.abs(slant.angle - Math.atan(Math.SQRT2)) + (square.side === -1 && slant.side === -1 ? 0 : 1), 1e-12, 'rad');
    assert('the Sun down, or along the street, lights no side of the section', P.profileSun(station, { altitude: -3, azimuth: 180 }) === null && P.profileSun(station, { altitude: 30, azimuth: 90 }).side === 0);
    // A reading as the engine gives it, of two stations, drawn.
    const reading = { length: 20, total: 13, lit: true, need: 2, step: 10, strips: [{ kind: 'footpath', from: -6.5, to: -3.5 }, { kind: 'lane', from: -3.5, to: 3.5 }, { kind: 'footpath', from: 3.5, to: 6.5 }],
      stations: [{ at: 5, point: [5, 0], left: [0, 1], faces: [{ distance: 10, height: 20, building: 0 }, { distance: 8, height: 8, building: 1 }], width: 18, enclosure: 14 / 18, sky: 0.58, sun: [1, 0], foot: [[5, 5], [5, -5]] },
        { at: 15, point: [15, 0], left: [0, 1], faces: [{ distance: 10, height: 20, building: 0 }, { distance: null, height: 0, building: null }], width: null, enclosure: 0, sky: 0.72, sun: [2, 2], foot: [[15, 5], [15, -5]] }] };
    const section = P.sectionSvg(reading, 0, { box: [0, 0, 900, 420], sun: { altitude: 40, azimuth: 180 }, own: 1, heading: 'High Street' });
    assert('a section is drawn with a building on either side, its strips and who uses them, the width between the faces, the sky, and the Sun\'s ray',
      (section.svg.match(/class="sx-building/g) || []).length === 2 && section.svg.includes('sx-building own') && (section.svg.match(/class="sx-strip"/g) || []).length === 3 && ['sx-figure', 'sx-vehicle', 'sx-sky', 'sx-ray', 'sx-lit', '18.0 m between the faces', 'High Street'].every((w) => section.svg.includes(w)));
    const open = P.sectionSvg(reading, 1, { box: [0, 0, 900, 420] });
    assert('where a side is open the section says so, and draws one building', (open.svg.match(/class="sx-building/g) || []).length === 1 && open.svg.includes('open to the right here') && !open.svg.includes('sx-ray'));
    const map = P.unrolledSvg(reading, 1, { box: [0, 0, 900, 150], need: 2 });
    assert('the street unrolled shows its faces, its strips, each footpath\'s sun and the station in hand; a place across it is a distance along it',
      (map.svg.match(/class="ux-face/g) || []).length === 3 && (map.svg.match(/class="ux-sun/g) || []).length === 4 && (map.svg.match(/class="ux-sun has"/g) || []).length === 2 && (map.svg.match(/class="ux-here"/g) || []).length === 1 && map.at(0) === 0 && map.at(5000) === 20);
    assert('with no street there is nothing to draw, and the section says so', P.sectionSvg(null, 0).svg.includes('No street is in hand') && P.unrolledSvg(null, 0).svg === '');
  }

  // ---- in the round ----
  {
    const pieces = P.wallPieces(6, 3, [{ at: 1.5, width: 1, height: 2.04, sill: 0, kind: 'door' }, { at: 4, width: 1.2, height: 1.2, sill: 0.9, kind: 'window' }]);
    const solid = pieces.filter((p) => !p.glass), area = solid.reduce((a, p) => a + (p.s1 - p.s0) * (p.z1 - p.z0), 0);
    check('plan: a wall cut at a door and a window keeps its area less theirs', area - (18 - 2.04 - 1.44), 1e-12, 'm²');
    assert('plan: ... in six pieces and a pane: three whole lengths, one over the door, one under and one over the window', solid.length === 6 && pieces.filter((p) => p.glass).length === 1 && pieces[0].s0 === 0 && pieces[pieces.length - 1].s1 === 6);
    const up = P.sunDirection(90, 0), north = P.sunDirection(0, 0), east = P.sunDirection(0, 90);
    check('plan: the Sun overhead is up; on the horizon to the north it is at −Z, to the east at +X', Math.hypot(up[0], up[1] - 1, up[2]) + Math.hypot(north[0], north[1], north[2] + 1) + Math.hypot(east[0] - 1, east[1], east[2]), 1e-12);
  }
}
