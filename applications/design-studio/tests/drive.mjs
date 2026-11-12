// The Design Studio driven as a person would: by the mouse and the keys, from an empty building to a house of two storeys on a lot
// with a scheme of rules, furnished and walked through, and to a real place. Each step asserts what the page then holds. Not in
// the suite: it needs Chrome, and the last step the network.
// With the server running (python applications/design-studio/serve.py):
//   node applications/design-studio/tests/drive.mjs <a directory for its pictures and Chrome's profile> [no-place]
import { mkdirSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';

import { open } from './cdp.mjs';

if (!process.argv[2]) { console.error('usage: node applications/design-studio/tests/drive.mjs <directory> [no-place]'); process.exit(2); }
const OUT = resolve(process.argv[2]), PLACE = process.argv[3] !== 'no-place';
const page = await open('http://127.0.0.1:9050/', { dir: `${OUT}/chrome-profile`, port: 9351 });
let passed = 0, failed = 0;
const assert = (name, ok, detail = '') => { ok ? passed++ : failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${detail ? `: ${detail}` : ''}`); };
const S = async (x, y) => page.eval(`(() => { const r = document.getElementById('plan').getBoundingClientRect(); const q = studio.plan.toScreen(studio.state.view, [r.width, r.height])([${x}, ${y}]); return [q[0] + r.left, q[1] + r.top]; })()`);
const click = async (x, y) => { const q = await S(x, y); await page.click(q[0], q[1]); await page.wait(250); };
const drag = async (x0, y0, x1, y1) => { const a = await S(x0, y0), b = await S(x1, y1); await page.drag(a[0], a[1], b[0], b[1]); await page.wait(300); };
const settled = async () => { await page.wait(120); await page.until('studio.state.busy === false', 20); await page.wait(80); };
const doc = (expr) => page.eval(`(() => { const d = studio.history.doc, r = studio.state.reading; return ${expr}; })()`);
const dom = (selector) => page.eval(`document.querySelector(${JSON.stringify(selector)}).click()`);

try {
  await page.until('window.studio && window.studio.state.busy === false', 30);
  // What an earlier run kept in the profile is forgotten: the page opens on its example.
  await page.eval('localStorage.clear(); location.reload()');
  await page.wait(600);
  await page.until('window.studio && window.studio.state.busy === false && window.studio.state.reading !== null', 30);
  assert('the page opens on the example house, read by the engine: seven rooms', await doc('r.storeys[0].rooms.length') === 7);

  // A new building, and a lot set down.
  await dom('#file'); await page.wait(300); await dom('[data-file="new"]'); await settled();
  assert('a new building has no wall and no lot', await doc('d.storeys[0].walls.length === 0 && d.site.lot === null'));
  await dom('[data-tab="site"]'); await page.wait(150); await dom('[data-act="box-lot"]'); await settled();
  const lot = await doc('d.site.lot.ring');
  assert('a lot of 15 × 30 m is set down', Math.abs((lot[1][0] - lot[0][0]) - 15) < 1e-9 && Math.abs((lot[2][1] - lot[1][1]) - 30) < 1e-9 && await doc('Math.abs(r.siting.lot_area - 450) < 1e-9'));
  const [x0, y0] = lot[0];
  const X = (x) => x0 + x, Y = (y) => y0 + y;
  // The south edge faces a street: by the Site panel's table.
  await dom('[data-street="0"]'); await settled();
  assert('its south edge is said to face a street', await doc('d.site.lot.street[0] === true && d.site.lot.street[1] === false'));

  // A room dragged with the Room tool.
  await page.key('r'); await drag(X(2), Y(6), X(13), Y(16)); await settled();
  assert('a rectangle dragged with the Room tool is four walls and one room of 110 m²', await doc('d.storeys[0].walls.length === 4 && r.storeys[0].rooms.length === 1 && Math.abs(r.storeys[0].rooms[0].area - 110) < 1e-6'), JSON.stringify(await doc('[d.storeys[0].walls.length, r.storeys[0].rooms.map((x) => x.area)]')));
  await page.shot(`${OUT}/1-room.png`);

  // A wall across it.
  await page.key('w'); await click(X(2), Y(11)); await click(X(13), Y(11)); await page.key('Escape'); await settled();
  assert('a wall drawn across it splits the outer walls and makes two rooms of 55 m²', await doc('d.storeys[0].walls.length === 7 && r.storeys[0].rooms.length === 2 && r.storeys[0].rooms.every((x) => Math.abs(x.area - 55) < 1e-6)'), JSON.stringify(await doc('[d.storeys[0].walls.length, r.storeys[0].rooms.map((x) => x.area)]')));

  // Doors and a window.
  await page.key('d'); await click(X(6), Y(11)); await settled();
  assert('a click on the inner wall with the Door tool opens a door of 0.82 m', await doc('d.storeys[0].openings.length === 1 && d.storeys[0].openings[0].kind === "door" && d.storeys[0].openings[0].width === 0.82'));
  await click(X(2), Y(8)); await settled();
  assert('on an outer wall the door is an entrance of 0.92 m, and both rooms are reached', await doc('d.storeys[0].openings.length === 2 && d.storeys[0].openings[1].width === 0.92 && r.storeys[0].rooms.every((x) => x.depth !== null)'));
  await page.key('n'); await click(X(7), Y(6)); await settled();
  assert('the Window tool sets a window whose glass lights its room', await doc('d.storeys[0].openings.length === 3 && d.storeys[0].openings[2].kind === "window" && r.storeys[0].rooms.some((x) => x.glazed_area > 1.7)'));

  // A room named.
  await page.key('v'); await click(X(5), Y(8.6)); await page.wait(200);
  assert('a click in a room with the Select tool takes it in hand', await page.eval('studio.state.selection && studio.state.selection.type === "room"'));
  await dom('#inspector [data-v="living"]'); await settled();
  assert('choosing a use names the room: a label is set in it, and the domain reads it as a living room', await doc('d.storeys[0].labels.length === 1 && d.storeys[0].labels[0].kind === "living" && r.storeys[0].rooms.filter((x) => x.kind === "living" && x.habitable).length === 1'));

  // Undo and redo.
  await page.key('z', { modifiers: 2 }); await page.key('z', { modifiers: 2 }); await settled();
  assert('two undos take back the name and the window', await doc('d.storeys[0].labels.length === 0 && d.storeys[0].openings.length === 2'));
  await page.key('y', { modifiers: 2 }); await page.key('y', { modifiers: 2 }); await settled();
  assert('two redos bring them back', await doc('d.storeys[0].labels.length === 1 && d.storeys[0].openings.length === 3'));

  // A wall moved by hand, its door with it.
  await page.key('v'); await drag(X(9), Y(11), X(9), Y(12)); await settled();
  assert('the inner wall dragged 1 m north stands at 12 m, its door still on it, the rooms 66 and 44 m²',
    await doc(`d.storeys[0].vertices.some((v) => Math.abs(v[1] - ${Y(12)}) < 1e-6) && !d.storeys[0].vertices.some((v) => Math.abs(v[1] - ${Y(11)}) < 1e-6) && d.storeys[0].openings.length === 3 && r.storeys[0].rooms.map((x) => Math.round(x.area)).sort().join() === "44,66"`),
    JSON.stringify(await doc('[d.storeys[0].vertices, r.storeys[0].rooms.map((x) => x.area)]')));
  await page.shot(`${OUT}/2-drawn.png`);

  // A scheme of rules.
  await dom('[data-tab="rules"]'); await page.wait(150);
  await page.eval(`(() => { const s = document.querySelector('[data-act="scheme"]'); s.value = 'victoria-nrz'; s.dispatchEvent(new Event('change', { bubbles: true })); })()`);
  await settled();
  const siting = await doc('r.siting.findings.map((f) => [f.rule, f.ok, f.actual])');
  assert('a scheme chosen states its rules, and each is found of the building: the street setback 5.875 m against 4 m, coverage 110 + 42 × 0.125 over 450',
    siting.some((f) => f[0] === 'street-setback' && f[1] && Math.abs(f[2] - 5.875) < 1e-9) && siting.some((f) => f[0] === 'coverage' && Math.abs(f[2] - (110 + 42 * 0.125) / 450) < 1e-9), JSON.stringify(siting));
  assert('the living room is found short of nothing or something, rule by rule', await doc('r.habitability.findings.filter((f) => f.rule === "light").length === 1 && r.habitability.findings.filter((f) => f.rule === "ceiling").length === 2'));
  await page.shot(`${OUT}/3-rules.png`);

  // A storey above, and a stair to it.
  await dom('[data-storey="add"]'); await settled();
  assert('a storey above begins on the outer walls of the one below: four walls, one room', await doc('d.storeys.length === 2 && d.storeys[1].walls.length === 4 && r.storeys[1].rooms.length === 1'), JSON.stringify(await doc('d.storeys[1].walls.length')));
  await dom('[data-storey="0"]'); await page.wait(200);
  await page.key('s'); await click(X(11), Y(7)); await settled();
  assert('a stair placed on the ground joins the room above: it is reached', await doc('d.stairs.length === 1 && r.stairs[0].head !== null && r.storeys[1].rooms[0].depth !== null'));
  assert('with the scheme, the stair\'s riser of 3 m over 17 is found within its range', await doc('r.habitability.findings.some((f) => f.rule === "riser" && f.ok && Math.abs(f.actual - 3 / 17) < 1e-12)'));
  const two = await doc('r.siting.findings.filter((f) => f.rule === "side-setback").map((f) => [f.subject, f.ok, f.least])');
  assert('the second storey, 6 m high, is asked 1.72 m from the sides and has 1.875 m', two.length === 2 && Math.abs(two[1][2] - 1.72) < 1e-9 && two[1][1], JSON.stringify(two));

  // In the round.
  await dom('[data-mode="solid"]'); await page.wait(3500);
  assert('the building stands in the round', await page.eval('!!document.querySelector("#solid canvas") && studio.state.solid !== null'));
  await page.shot(`${OUT}/4-solid.png`);
  await dom('[data-mode="plan"]'); await page.wait(300);

  // ---- what stands in the rooms ----
  await page.key('u'); await page.wait(200);
  assert('the Furnish tool opens the catalogue', await page.eval('studio.state.tool === "furnish" && studio.state.panel === "interior" && document.querySelectorAll("#panel [data-kind]").length >= 30'));
  await dom('#panel [data-kind="sofa-3"]'); await page.wait(150);
  await click(X(7), Y(6.5)); await settled();
  const sofa = await doc('d.furniture[0]');
  assert('a sofa chosen and set down near the south wall stands flush against its face, its back to it, and is in hand',
    await doc('d.furniture.length === 1') && Math.abs(sofa.at[1] - (Y(6) + 0.125 + 0.475)) < 1e-9 && Math.abs(sofa.at[0] - X(7)) < 1e-9 && sofa.bearing === 90
    && await page.eval('studio.state.selection.type === "piece" && studio.state.tool === "select"'), JSON.stringify(sofa));
  assert('the domain finds it within its room, its floor free, and reached from the door', await doc('r.interior.pieces.length === 1 && r.interior.pieces[0].ok && r.interior.pieces[0].judged'));
  await drag(X(7), Y(6.6), X(9), Y(9)); await settled();
  assert('dragged to the middle of the room it stands on the grid, turned as it was', await doc(`Math.abs(d.furniture[0].at[0] - ${X(9)}) < 0.11 && Math.abs(d.furniture[0].at[1] - ${Y(9)}) < 0.11 && d.furniture[0].bearing === 90`), JSON.stringify(await doc('d.furniture[0]')));
  await page.key(']'); await settled();
  assert('the key ] turns the piece in hand 15° to the right', await doc('d.furniture[0].bearing === 75'));
  await page.eval(`studio.change('addPiece', { storey: 0, kind: 'coffee-table', at: [${X(9)}, ${Y(9)}], bearing: 0 })`); await settled();
  assert('a table set on the sofa is found: each overlaps the other, and the Interior tab counts two', await doc('r.interior.pieces.every((f) => f.overlaps.length === 1 && !f.ok)') && await page.eval('document.querySelector(\'[data-tab="interior"] .count\').textContent === "2"'));
  await page.key('z', { modifiers: 2 }); await settled();
  await page.shot(`${OUT}/6-furnished.png`);

  // ---- the walk ----
  await dom('[data-mode="walk"]');
  await page.until('studio.state.solid !== null && studio.state.eye !== null', 60);
  await page.wait(5000);
  // One picture drawn as a moving eye's is, so that what it needs is ready before the walk is timed (without a graphics card the
  // first such picture takes a second or two).
  await page.eval('(() => { const v = studio.state.solid; v.renderer.render(v.scene, v.camera); v.renderer.getContext().finish(); })()');
  const eye0 = await page.eval('studio.state.eye');
  assert('walking begins just inside the door from the outside, looking in, with a small plan that marks the eye',
    Math.abs(eye0.at[0] - (X(2) + 0.125 + 0.7)) < 0.05 && Math.abs(eye0.at[1] - Y(8)) < 0.3 && Math.abs(eye0.heading) < 1 && await page.eval('!document.getElementById("minimap").hidden && document.querySelector("#mini .eye") !== null'), JSON.stringify(eye0));
  await page.send('Input.dispatchKeyEvent', { type: 'keyDown', key: 'w' }); await page.wait(1600); await page.send('Input.dispatchKeyEvent', { type: 'keyUp', key: 'w' }); await page.wait(500);
  const eye1 = await page.eval('studio.state.eye');
  assert('holding W walks forward, at about a person\'s pace', eye1.at[0] - eye0.at[0] > 0.25 && eye1.at[0] - eye0.at[0] < 3.2 && Math.abs(eye1.at[1] - eye0.at[1]) < 0.05, JSON.stringify([eye0.at, eye1.at]));
  await page.eval('studio.state.solid.stand(studio.state.eye.at, 90)'); await page.wait(200);
  await page.send('Input.dispatchKeyEvent', { type: 'keyDown', key: 'w' }); await page.wait(5000); await page.send('Input.dispatchKeyEvent', { type: 'keyUp', key: 'w' }); await page.wait(400);
  const eye2 = await page.eval('studio.state.eye');
  assert('walking north at the wall between the rooms one is stopped by it, the radius of a body from its face', eye2.at[1] <= Y(12) - 0.055 - 0.2 + 1e-6 && eye2.at[1] > Y(12) - 0.7, JSON.stringify(eye2.at));
  {
    const r = await page.eval('(() => { const b = document.getElementById("mini").getBoundingClientRect(); return [b.left, b.top, b.width, b.height]; })()');
    await page.click(r[0] + r[2] * 0.62, r[1] + r[3] * 0.62); await page.wait(400);
    const eye3 = await page.eval('studio.state.eye');
    assert('a click on the small plan puts the eye there', Math.hypot(eye3.at[0] - eye2.at[0], eye3.at[1] - eye2.at[1]) > 1.5, JSON.stringify(eye3.at));
  }
  // A piece taken in hand from within, and moved over the floor.
  {
    const s = await doc('d.furniture[0]');
    await page.eval(`(() => { const v = studio.state.solid; v.stand([${s.at[0] - 3.2}, ${s.at[1]}], 0); v.eye.pitch = -14; v.look(); })()`); await page.wait(1200);
    const at = await page.eval(`(() => { const v = studio.state.solid, T = v.THREE, q = new T.Vector3(${s.at[0]}, 0.45, ${-s.at[1]}).project(v.camera), b = v.renderer.domElement.getBoundingClientRect(); return [b.left + (q.x + 1) / 2 * b.width, b.top + (1 - q.y) / 2 * b.height]; })()`);
    await page.click(at[0], at[1]); await page.wait(500);
    assert('a click on the sofa, seen from within, takes it in hand', await page.eval('studio.state.selection && studio.state.selection.type === "piece" && studio.state.selection.index === 0'), JSON.stringify(at));
    await page.drag(at[0], at[1], at[0] + 170, at[1] + 30, 8); await settled(); await page.wait(400);
    const after = await doc('d.furniture[0]');
    assert('dragged, it moves over the floor and stays in hand', Math.hypot(after.at[0] - s.at[0], after.at[1] - s.at[1]) > 0.3 && await page.eval('studio.state.selection && studio.state.selection.type === "piece"'), JSON.stringify([s.at, after.at]));
    await page.shot(`${OUT}/7-walk.png`);
  }
  // A model of one's own: a box of 2 × 0.5 × 1 m written here as a binary glTF file, given to the piece in hand.
  {
    const pos = [], idx = [];
    for (const z of [-0.25, 0.25]) for (const y of [0, 1]) for (const x of [-1, 1]) pos.push(x, y, z);
    for (const f of [[0, 1, 3, 2], [4, 6, 7, 5], [0, 4, 5, 1], [2, 3, 7, 6], [0, 2, 6, 4], [1, 5, 7, 3]]) idx.push(f[0], f[2], f[1], f[0], f[3], f[2]);
    const bin = Buffer.concat([Buffer.from(new Float32Array(pos).buffer), Buffer.from(new Uint16Array(idx).buffer)]);
    const json = Buffer.from(JSON.stringify({ asset: { version: '2.0' }, scene: 0, scenes: [{ nodes: [0] }], nodes: [{ mesh: 0 }], meshes: [{ primitives: [{ attributes: { POSITION: 0 }, indices: 1 }] }],
      buffers: [{ byteLength: bin.length }], bufferViews: [{ buffer: 0, byteOffset: 0, byteLength: 96 }, { buffer: 0, byteOffset: 96, byteLength: 72 }],
      accessors: [{ bufferView: 0, componentType: 5126, count: 8, type: 'VEC3', min: [-1, 0, -0.25], max: [1, 1, 0.25] }, { bufferView: 1, componentType: 5123, count: 36, type: 'SCALAR' }] }));
    const pad = (b, fill) => Buffer.concat([b, Buffer.alloc((4 - b.length % 4) % 4, fill)]), J = pad(json, 0x20), B = pad(bin, 0);
    const head = Buffer.alloc(12), hj = Buffer.alloc(8), hb = Buffer.alloc(8);
    head.writeUInt32LE(0x46546c67, 0); head.writeUInt32LE(2, 4); head.writeUInt32LE(12 + 8 + J.length + 8 + B.length, 8);
    hj.writeUInt32LE(J.length, 0); hj.writeUInt32LE(0x4e4f534a, 4); hb.writeUInt32LE(B.length, 0); hb.writeUInt32LE(0x004e4942, 4);
    mkdirSync(OUT, { recursive: true });
    const file = resolve(OUT, 'a-box.glb');
    writeFileSync(file, Buffer.concat([head, hj, J, hb, B]));
    const root = await page.send('DOM.getDocument'), node = await page.send('DOM.querySelector', { nodeId: root.root.nodeId, selector: '#pickmodel' });
    await page.send('DOM.setFileInputFiles', { nodeId: node.nodeId, files: [file] });
    await page.until('studio.history.doc.furniture[0].model !== undefined', 30).catch(() => {});
    await settled(); await page.wait(1500);
    const own = await doc('d.furniture[0]');
    assert('a binary glTF file given to the piece in hand is kept by the server, and the piece takes its size: 2 × 0.5 × 1 m',
      own.model && /^[0-9a-f]{64}$/.test(own.model.id) && own.model.name === 'a-box.glb' && own.width === 2 && own.depth === 0.5 && own.height === 1, JSON.stringify(own));
    assert('in the round the piece is drawn by that file\'s mesh of eight corners', await page.eval(`(() => { let n = 0; studio.state.solid.pieces[0].traverse((o) => { if (o.isMesh && o.geometry.attributes.position.count === 8) n++; }); return n === 1; })()`));
  }
  await dom('[data-mode="plan"]'); await page.wait(300);

  // ---- the outside (ADR 0097) ----
  await dom('[data-mode="plan"]'); await page.wait(300);
  await dom('[data-tab="shell"]'); await page.wait(200);
  assert('a building drawn from nothing has a flat roof with a parapet, read on its upper storey', await doc('d.roof.form === "flat" && r.shell.roofs.length === 1 && r.shell.roofs[0].form === "flat" && r.shell.roofs[0].storey === 1 && r.shell.roofs[0].walls.length === 4'));
  await dom('[data-act="roof-form"][data-v="hip"]'); await settled();
  assert('made a hip in the Shell panel, its faces rise from every eave at the pitch: its area is its plan\'s over the cosine', await doc('d.roof.form === "hip" && d.roof.pitch === 27.5 && r.shell.roofs[0].form === "hip" && Math.abs(r.shell.roofs[0].area - r.shell.roofs[0].plan_area / Math.cos(27.5 * Math.PI / 180)) < 1e-6 && r.shell.roofs[0].walls.length === 0'));
  assert('the plan shows the roof over the storey in hand: its edge and a ridge', await page.eval('(() => { studio.state.storey = 1; studio.renderAll(); return !!document.querySelector("#plan .roofedge") && !!document.querySelector("#plan .roofline.ridge"); })()'));
  await dom('[data-act="roof-form"][data-v="gable"]'); await settled();
  assert('made a gable, the two ends of its long way stand as gables and their walls are carried up', await doc('d.roof.form === "gable" && d.roof.gables.length === 2 && r.shell.roofs[0].gable.filter(Boolean).length === 2 && r.shell.roofs[0].walls.length === 2 && r.shell.roofs[0].refused === 0'));
  assert('the panel lists its four eaves, two of them gables', await page.eval('document.querySelectorAll("#panel [data-gable]").length === 4 && document.querySelectorAll("#panel [data-gable].on").length === 2'));
  await dom('#panel [data-gable].on'); await settled();
  assert('a gable clicked becomes an eave again: one gable is left', await doc('d.roof.gables.length === 1 && r.shell.roofs[0].gable.filter(Boolean).length === 1 && r.shell.roofs[0].walls.length === 1'));
  await page.key('z', { modifiers: 2 }); await settled();
  assert('and undo brings it back', await doc('d.roof.gables.length === 2'));
  await dom('[data-act="clad"][data-v="brick"]'); await settled();
  await dom('[data-act="tone"][data-v="clay"]'); await settled();
  await dom('[data-act="bound"][data-v="hedge"]'); await settled();
  assert('the walls are clad in brick of a clay tone, and a hedge bounds the lot where it faces no street', await doc('d.shell.cladding === "brick" && d.shell.tone === "clay" && d.site.boundary.kind === "hedge"'));
  assert('its faces are listed by the way they look', await page.eval('document.querySelectorAll("#panel table tbody tr").length') === 4);
  await dom('[data-mode="elevations"]'); await page.wait(500);
  assert('the four elevations are drawn on one sheet: walls with their openings, the roof, the levels', await page.eval('document.querySelectorAll("#elev .el-side").length === 4 && document.querySelectorAll("#elev .el-wall").length >= 8 && document.querySelectorAll("#elev .el-roof").length >= 2 && document.querySelectorAll("#elev .el-glass").length >= 1 && document.querySelector("#elev").textContent.includes("+6.00")'));
  await page.shot(`${OUT}/6-elevations.png`);
  await dom('[data-mode="solid"]'); await page.wait(3500);
  assert('in the round the roof stands on the building, in its cover', await page.eval('(() => { let n = 0; studio.state.solid.model.traverse((o) => { if (o.userData && o.userData.roof) n++; }); return n; })()') === 1);
  await page.shot(`${OUT}/7-outside.png`);
  await dom('[data-mode="plan"]'); await page.wait(300);
  await dom('[data-tab="light"]'); await page.wait(200);
  assert('with no place the Light panel gives each room\'s glass, sky and daylight factor, and says the Sun is not given', await page.eval('document.querySelectorAll("#panel table tbody tr").length >= 1 && /No place is given/.test(document.getElementById("panel").textContent)') && await doc('r.shell.lit === false && r.shell.rooms.some((x) => x.daylight_factor > 0)'),
    JSON.stringify(await page.eval('[document.querySelectorAll("#panel table tbody tr").length, document.getElementById("panel").textContent.slice(0, 160), studio.state.panel]')));

  // A real place: its surroundings.
  if (PLACE) {
    await page.eval('studio.goTo("Carlton", -37.80035, 144.96735)');
    await page.until('studio.state.context !== null || /could not|The lot can still/.test(studio.state.contextNote)', 120).catch(() => {});
    const n = await page.eval('studio.state.context ? [studio.state.context.buildings.length, studio.state.context.ways.length] : null');
    if (n) assert('a place given, its buildings and ways are read from the world\'s map', n[0] > 20 && n[1] > 5, JSON.stringify(n));
    else console.log('skip  the surroundings: ' + await page.eval('studio.state.contextNote'));
    await page.eval('studio.fitView()'); await page.wait(300);
    await page.eval('studio.state.view = { ...studio.state.view, scale: 5 }; studio.setTool("select")'); await page.wait(300);
    await page.shot(`${OUT}/5-place.png`);
    // ---- the light of that place, and its street (ADR 0097, ADR 0098) ----
    if (n) {
      await settled(); await page.wait(400); await settled();
      assert('with a place, the Sun is its Sun: the rooms\' hours of sun on the day are found among the neighbours', await doc('r.shell.lit === true && r.shell.neighbours > 5 && r.shell.rooms.every((x) => x.sun_hours >= 0) && r.heat.day_gain !== null && r.heat.day_loss > 0'));
      await dom('[data-mode="street"]'); await page.wait(600);
      const ways = await page.eval('document.querySelectorAll("#panel [data-way]").length');
      assert('the Street view offers the ways about the site by their names', ways >= 1, String(ways));
      await dom('#panel [data-way]');
      await page.until('studio.state.street.reading !== null', 30);
      await page.wait(300);
      const st = await page.eval('(() => { const r = studio.state.street.reading; return [r.stations.length, r.length, r.rises.length, r.lit, document.querySelectorAll("#street .sx-building").length, document.querySelectorAll("#street .ux-face").length]; })()');
      assert('a way taken in hand is read along its length: stations every 5 m, the faces on either side, the sun, the buildings that stand on it', st[0] >= 8 && Math.abs(st[1] / st[0] - 5) < 1.5 && st[2] >= 2 && st[3] === true && st[5] > 8, JSON.stringify(st));
      await dom('[data-act="strips-presume"]'); await settled(); await page.until('studio.state.street.reading && studio.state.street.reading.strips.length > 0', 20);
      await page.wait(300);
      assert('a division presumed for its width is set out, and drawn in the section strip by strip', await page.eval('(() => { const d = studio.history.doc, r = studio.state.street.reading; return d.street.strips.length >= 1 && r.strips.length === d.street.strips.length && document.querySelectorAll("#street .sx-strip").length === r.strips.length && Math.abs(r.total - d.street.strips.reduce((s, x) => s + x.width, 0)) < 1e-9; })()'));
      await dom('[data-strip-add="verge"]'); await settled(); await page.until('studio.state.street.reading && studio.state.street.reading.strips.some((s) => s.kind === "verge")', 20);
      assert('a verge added by hand is a strip more, and the green share is no longer nothing', await page.eval('studio.state.street.reading.shares.green > 0'));
      const before = await page.eval('studio.state.street.station');
      { const box = await page.eval('(() => { const r = document.getElementById("street").getBoundingClientRect(); return [r.left, r.top, r.width, r.height]; })()'); await page.click(box[0] + box[2] * 0.7, box[1] + box[3] - 56 - 75); await page.wait(300); }
      assert('a click on the unrolled street moves the station in hand', await page.eval('studio.state.street.station') !== before);
      const raised = await page.eval('(() => { const el = document.querySelector("#panel [data-rise]"); if (!el) return null; el.value = "30"; el.dispatchEvent(new Event("change", { bubbles: true })); return Number(el.dataset.rise); })()');
      await settled();
      await page.until('studio.state.street.reading && studio.state.street.reading.rises.some((x) => Math.abs(x.height - 30) < 1e-9)', 20).catch(() => {});
      assert('a building beside it given another height stands so in the design, and the street is read again with it', raised !== null && await page.eval('studio.history.doc.street.heights.length === 1 && studio.history.doc.street.heights[0].height === 30 && studio.state.street.reading.rises.some((x) => Math.abs(x.height - 30) < 1e-9)'),
        JSON.stringify(await page.eval('[studio.history.doc.street.heights, studio.state.street.reading.rises.map((x) => x.height)]')));
      await page.shot(`${OUT}/8-street.png`);
      await dom('[data-mode="solid"]'); await page.wait(4000);
      await page.shot(`${OUT}/9-street-round.png`);
      await dom('[data-mode="plan"]'); await page.wait(300);
    }
  }
} catch (error) {
  failed++;
  console.log('FAIL  the drive stopped: ' + error.message);
  await page.shot(`${OUT}/stopped.png`).catch(() => {});
} finally {
  assert('the page threw nothing', page.errors.length === 0 && page.logs.length === 0, JSON.stringify([page.errors, page.logs]));
  console.log(`\n${passed} of ${passed + failed} assertions passed.`);
  await page.close();
}
