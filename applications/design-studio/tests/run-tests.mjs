// Checks of the Design Studio: what is the studio's own. The science (rooms as faces, net areas, access, the rules' forms, the
// brief, quantities) is checked with the architecture domain, and the document and its operations with the visualization layer;
// these check that the schemes of rules that come with the studio are well formed and say what they do not find, that the page's
// ink can be read on every tone of its rooms by day and by night, and, through the server, that the engine answers the page as the
// page expects: the example house read, its numbers recomputed here from what the engine returns, a scheme found rule by rule, a
// storey that is not a plan refused with its faults, what is found of the pieces that stand in its rooms, projects and models of
// one's own kept and forgotten. Those that need Python or the engine skip, and say so, when they are missing.
// Run from the repository root:  node applications/design-studio/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { mkdtempSync, readdirSync, readFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite } from '../../../development/tools/checks.mjs';

const { assert, check, skip, done } = suite();
const APP = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const ROOT = resolve(APP, '../..');
const url = (rel) => { const p = join(ROOT, rel).replace(/\\/g, '/'); return /^[A-Za-z]:/.test(p) ? `file:///${p}` : `file://${p}`; };

const P = await import(url('visualization/js/plan/index.js'));
const { toLinear, hexRgb } = await import(url('visualization/styles/colour.js'));

// ---- the schemes of rules ------------------------------------------------------------------------------------------------------
const schemes = Object.fromEntries(readdirSync(join(APP, 'schemes')).filter((f) => f.endsWith('.json')).map((f) => [f.replace(/\.json$/, ''), JSON.parse(readFileSync(join(APP, 'schemes', f), 'utf8'))]));
{
  const limits = (o) => Object.values(o).flatMap((v) => (v && typeof v === 'object' && !Array.isArray(v) ? ('least' in v || 'most' in v ? [v] : limits(v)) : []));
  assert('a scheme has a name, its siting rules and its code of habitation; one that states rules says its version, when it was read and from where',
    Object.values(schemes).every((s) => s.name && typeof s.siting === 'object' && typeof s.code === 'object' && (Object.keys(s.siting).length + Object.keys(s.code).length === 0 || (s.version && /^\d{4}-\d{2}-\d{2}$/.test(s.read) && s.source))));
  assert('the scheme that states nothing states nothing', Object.keys(schemes.unstated.siting).length === 0 && Object.keys(schemes.unstated.code).length === 0);
  assert('every limit is a number, at least below at most, and cites its clause',
    Object.entries(schemes).filter(([id]) => id !== 'unstated').every(([, s]) => limits({ ...s.siting, ...s.code }).every((l) => ['least', 'most'].every((b) => !(b in l) || Number.isFinite(l[b])) && (!('least' in l && 'most' in l) || l.least <= l.most) && l.clause)));
  assert('a scheme of a jurisdiction says where it applies and lists what it does not find', Object.entries(schemes).filter(([id]) => id !== 'unstated').every(([, s]) => s.applies && Array.isArray(s.not_evaluated) && s.not_evaluated.length >= 5 && s.about.includes('read')));
  const v = schemes['victoria-nrz'];
  const s = (h) => v.siting.side_setback.steps.reduce((a, [above, to, slope]) => a + slope * Math.max(0, Math.min(h - above, (to ?? Infinity) - above)), v.siting.side_setback.least);
  check('Victoria\'s side setback, as the scheme states it, is 1 m to 3.6 m, 1.99 m at 6.9 m and 4.09 m at 9 m', Math.abs(s(3) - 1) + Math.abs(s(6.9) - 1.99) + Math.abs(s(9) - 4.09), 1e-12, 'm');
  assert('its two zones differ where the survey says: 9 m and two storeys at 60% against 11 m and three at 65%', v.siting.height.most === 9 && v.siting.storeys.most === 2 && v.siting.coverage.most === 0.6
    && schemes['victoria-grz'].siting.height.most === 11 && schemes['victoria-grz'].siting.storeys.most === 3 && schemes['victoria-grz'].siting.coverage.most === 0.65);
}

// ---- the page's colours --------------------------------------------------------------------------------------------------------
{
  const css = readFileSync(join(APP, 'styles.css'), 'utf8');
  const block = (selector) => Object.fromEntries([...css.slice(css.indexOf(`${selector} {`)).split('}')[0].matchAll(/--([a-z-]+):\s*(#[0-9a-f]{6})/g)].map((m) => [m[1], m[2]]));
  const luminance = (hex) => { const [r, g, b] = hexRgb(hex).map(toLinear); return 0.2126 * r + 0.7152 * g + 0.0722 * b; };
  const contrast = (a, b) => { const [hi, lo] = [luminance(a), luminance(b)].sort((x, y) => y - x); return (hi + 0.05) / (lo + 0.05); };
  for (const [name, selector] of [['day', ':root'], ['night', ':root.night']]) {
    const c = block(selector), tones = Object.entries(c).filter(([k]) => k.startsWith('u-') || k === 'paper' || k === 'sheet');
    const least = Math.min(...tones.map(([, hex]) => contrast(c.ink, hex)));
    assert(`by ${name}, the ink holds a contrast of 7 on the paper, the sheets and every tone of a room: ${least.toFixed(1)} at least`, tones.length === 11 && least >= 7);
    assert(`by ${name}, what fails is told from what passes by a glyph and a word, and its rose still reads on a sheet (4.5)`, contrast(c.fail, c.sheet) >= 4.5 && readFileSync(join(APP, 'src/app.js'), 'utf8').includes("f.ok ? '✓' : '✗'"));
  }
  const classes = new Set([...css.matchAll(/\.room\.use-([a-z]+)/g)].map((m) => m[1]));
  const kinds = ['living', 'dining', 'family', 'kitchen', 'bedroom', 'study', 'bathroom', 'ensuite', 'toilet', 'laundry', 'hall', 'corridor', 'entry', 'landing', 'store', 'pantry', 'wardrobe', 'garage'];
  assert('every indoor use has a tone on the plan', kinds.every((k) => classes.has(k)));
  const marks = new Set(P.CATALOGUE.flatMap((c) => P.symbol(P.newPiece(c.kind, 0, [0, 0])).flatMap((m) => m.cls.split(' ').filter(Boolean))));
  assert('every mark of a piece\'s symbol has its style on the plan', [...marks].every((cls) => css.includes(`.pm.${cls}`)), [...marks].filter((cls) => !css.includes(`.pm.${cls}`)).join(' '));
}

// ---- through the server --------------------------------------------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
if (!python) skip('the design studio: the server and the engine (no Python found to run them)');
else {
  const port = 9057, store = mkdtempSync(join(tmpdir(), 'design-studio-'));
  const server = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store], { stdio: 'ignore' });
  const base = `http://127.0.0.1:${port}`;
  const get = async (path) => { const r = await fetch(base + path); return { status: r.status, body: await r.json().catch(() => null) }; };
  const send = async (method, path, body) => { const r = await fetch(base + path, { method, headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) }); return { status: r.status, body: await r.json().catch(() => null) }; };
  try {
    let status = null;
    for (let i = 0; i < 50 && !status; i++) { await new Promise((r) => setTimeout(r, 200)); try { status = (await get('/api/status')).body; } catch { /* not up yet */ } }
    assert('the server starts and says whether its engine is built', status !== null && typeof status.engine.current === 'boolean');
    const page = await fetch(base + '/'), script = await fetch(base + '/src/app.js'), shared = await fetch(base + '/visualization/js/plan/index.js');
    assert('it serves the page, its script and the visualization layer beside it', page.status === 200 && (await page.text()).includes('<title>Design Studio</title>') && script.status === 200 && shared.status === 200);
    const listed = (await get('/api/schemes')).body.schemes;
    assert('it lists the schemes, each with its file\'s name as its id', listed.length === Object.keys(schemes).length && listed.every((s) => schemes[s.id] && s.name === schemes[s.id].name));
    assert('surroundings are read at level 14 and no other', (await get('/world/city/13/1/1')).status === 400);

    // Projects kept and forgotten.
    const house = P.exampleHouse();
    const kept = await send('PUT', '/api/projects/a-house', house);
    const again = await get('/api/projects/a-house'), list = await get('/api/projects');
    assert('a building kept is listed by its name and read back as it was', kept.status === 200 && JSON.stringify(again.body) === JSON.stringify(house) && list.body.projects.length === 1 && list.body.projects[0].name === house.name);
    assert('what is not a building is not kept, and a name that is no name is not found', (await send('PUT', '/api/projects/x', { schema: 'other/1' })).status === 400 && (await get('/api/projects/No_Such')).status === 404);
    assert('a building forgotten is gone', (await send('DELETE', '/api/projects/a-house')).status === 200 && (await get('/api/projects/a-house')).status === 404);

    // A model of one's own, kept by the hash of its content.
    {
      const { createHash } = await import('node:crypto');
      const glb = new Uint8Array([0x67, 0x6c, 0x54, 0x46, 2, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0x4a, 0x53, 0x4f, 0x4e]);
      const put = await fetch(base + '/api/models', { method: 'PUT', body: glb }), kept = await put.json();
      const back = await fetch(base + `/api/models/${kept.id}`);
      assert('a binary glTF file is kept by the hash of its content and given back byte for byte', put.status === 200 && kept.id === createHash('sha256').update(glb).digest('hex')
        && back.headers.get('content-type') === 'model/gltf-binary' && Buffer.from(await back.arrayBuffer()).equals(Buffer.from(glb)));
      const not = await fetch(base + '/api/models', { method: 'PUT', body: new TextEncoder().encode('<html>not a model</html>') });
      assert('what is not a binary glTF file is not kept, and a hash that names nothing is not found', not.status === 400 && (await fetch(base + `/api/models/${'0'.repeat(64)}`)).status === 404);
    }

    if (!status.engine.current) skip('the design studio: what the engine reads (the engine is not built from the present sources: run the recipe)');
    else {
      const vocabulary = (await get('/api/vocabulary')).body;
      assert('the engine names the uses of rooms and the elements a rate may be declared for; the rates that come with a new building are for those elements',
        vocabulary.kinds.length === 24 && vocabulary.elements.length === 11 && vocabulary.elements.every((e) => P.PRESUMED_RATES.items[e.id]) && vocabulary.schema === P.SCHEMA);
      const read = async (doc) => send('POST', '/api/read', doc);
      const r = (await read(house)).body, rooms = r.storeys[0].rooms, named = (n) => rooms.find((x) => x.name === n);
      assert('the example house is read: sound, seven rooms, each named and reached', r.sound && rooms.length === 7 && rooms.every((x) => x.name && x.depth !== null) && named('Hall').depth === 1 && named('Living').depth === 2);
      check('its rooms tile 11 × 10 m: 110 m² to the centrelines', r.storeys[0].floor_area - 110, 1e-9, 'm²');
      check('the internal area is the sum of the rooms\' net areas', r.storeys[0].internal_area - rooms.reduce((a, x) => a + x.net_area, 0), 1e-9, 'm²');
      check('the living room inside its walls is (6 − 0.125) × (5 − 0.125 − 0.055): the open wall to the kitchen takes nothing', named('Living').net_area - 5.875 * 4.82, 1e-9, 'm²');
      check('the hall holds a circle of 1.2 − 0.11 = 1.09 m', named('Hall').width - 1.09, 1e-6, 'm');
      assert('the third bedroom, with 0.6 m² of glass on 9.7 m² of floor, has the least light of the bedrooms', named('Bedroom 3').light_share < 0.07 && named('Bedroom 1').light_share > 0.15 && named('Bedroom 2').light_share > 0.15);
      assert('the kitchen is joined to the living room by the open wall, without a door', r.access.edges.some(([u, v]) => (u === named('Living').vertex && v === named('Kitchen').vertex) || (v === named('Living').vertex && u === named('Kitchen').vertex)));
      assert('the brief of a household of four is answered, statement by statement', r.brief.length === 8 && r.brief.every((s) => s.ok));
      // Cost and heat, recomputed here from the quantities returned.
      const q = r.quantities, rate = P.PRESUMED_RATES.items;
      const amount = { outer_wall: q.outer_wall_net, inner_wall: q.inner_wall_net, window: q.window_area, outer_door: q.outer_doors, inner_door: q.inner_doors, ground_slab: q.ground_slab, upper_floor: q.upper_floor,
        roof: q.roof, stair: q.stairs, fit_out: q.internal_area, wet_room: q.wet_area };
      const low = Object.keys(rate).reduce((a, id) => a + amount[id] * rate[id][0], 0), high = Object.keys(rate).reduce((a, id) => a + amount[id] * rate[id][1], 0);
      check('the cost is each quantity at its low rate and at its high, summed', Math.abs(r.cost.low - low) + Math.abs(r.cost.high - high), 1e-6);
      const f = P.DEFAULT_FABRIC;
      check('the fabric\'s heat loss is Σ U A', r.heat.total - (f.wall * q.outer_wall_net + f.window * q.window_area + f.door * q.outer_door_area + f.roof * q.roof_plan + f.ground * q.ground_slab), 1e-9, 'W/K');
      check('the outer walls are 42 m at 3 m with the two gables carried up to the roof, less the entrance and seven windows', q.outer_wall_net - (126 + r.shell.roofs[0].wall_area - 0.92 * 2.04 - q.window_area), 1e-9, 'm²');
      assert('with no scheme chosen, only whether it stands within its lot is found', r.siting.findings.length === 1 && r.siting.findings[0].rule === 'lot' && r.siting.findings[0].ok && r.habitability.findings.length === 0);

      // A scheme chosen: its rules found one by one.
      let ruled = P.apply(P.apply(house, 'declare', { path: ['siting'], value: schemes['victoria-nrz'].siting }), 'declare', { path: ['code'], value: schemes['victoria-nrz'].code });
      const v = (await read(ruled)).body, finding = (list, rule, subject = '') => list.find((x) => x.rule === rule && x.subject.includes(subject));
      check('under Victoria\'s scheme the house stands 6 − 0.125 m from the street', finding(v.siting.findings, 'street-setback').actual - 5.875, 1e-9, 'm');
      check('... 2 − 0.125 m from its sides, against 1 m', Math.abs(finding(v.siting.findings, 'side-setback').actual - 1.875) + Math.abs(finding(v.siting.findings, 'side-setback').least - 1), 1e-9, 'm');
      check('... and covers 110 m² of floor and 42 × 0.125 of wall, of 450', finding(v.siting.findings, 'coverage').actual - 115.25 / 450, 1e-12);
      assert('every siting rule is met, each citing its clause', v.siting.findings.length === 8 && v.siting.findings.every((x) => x.ok) && v.siting.findings.filter((x) => x.clause).length === 7);
      const light = v.habitability.findings.filter((x) => x.rule === 'light'), failing = v.habitability.findings.filter((x) => !x.ok);
      assert('light is found of the five habitable rooms; the third bedroom and the kitchen fail it', light.length === 5 && light.filter((x) => !x.ok).map((x) => x.subject).sort().join() === 'Bedroom 3,Kitchen');
      assert('what fails is light and air in those two rooms, and nothing else', failing.length === 4 && failing.every((x) => ['light', 'air'].includes(x.rule) && ['Bedroom 3', 'Kitchen'].includes(x.subject)));
      assert('the hall\'s clear width of 1.09 m meets 1.0 m, and the kitchen takes the 2.1 m stated for kitchens', finding(v.habitability.findings, 'passage', 'Hall').ok && finding(v.habitability.findings, 'ceiling', 'Kitchen').least === 2.1);
      // A window widened by the hand's own operation mends it.
      const s = ruled.storeys[0], small = s.openings.findIndex((o) => o.kind === 'window' && o.width === 0.6 && o.height === 1.0);
      ruled = P.apply(ruled, 'setOpening', { storey: 0, opening: small, width: 1.5, height: 1.5, sill: 0.6 });
      const mended = (await read(ruled)).body.habitability.findings.filter((x) => !x.ok);
      assert('the third bedroom\'s window widened to 1.5 × 1.5 m, only the kitchen still fails', mended.length === 2 && mended.every((x) => x.subject === 'Kitchen'));
      // A second storey 6 m high is asked more of.
      const outer = v.storeys[0].walls.map((w, i) => (w.outer ? i : -1)).filter((i) => i >= 0);
      const two = (await read(P.apply(ruled, 'addStorey', { copy: 'outer', outer }))).body;
      check('a storey above, 6 m high, is asked 1.72 m from the sides', finding(two.siting.findings, 'side-setback', 'storey 2').least - 1.72, 1e-9, 'm');
      assert('... and the building is then two storeys of 6 m, within the 9 m and two the zone allows', finding(two.siting.findings, 'height').actual === 6 && finding(two.siting.findings, 'storeys').ok && Math.abs(two.quantities.upper_floor - 110) < 1e-9);

      // The shell (ADR 0097): the roof, the faces, the light at the glass.
      {
        const S = r.shell, roof = S.roofs[0], t = Math.tan(27.5 * Math.PI / 180), eaves = 11.25 + 0.9, deep = 10.25 + 0.9;
        assert('the example\'s roof is a gable on its one storey, from a plate at 3 m, its west and east eaves standing as gables', S.roofs.length === 1 && roof.form === 'gable' && roof.storey === 0 && roof.plate === 3 && roof.exact && roof.gable.filter(Boolean).length === 2 && roof.refused === 0);
        check('its eaves are 0.45 m beyond walls 11.25 × 10.25 m at their outer faces: 12.15 × 11.15 in plan', roof.plan_area - eaves * deep, 1e-6, 'm²');
        check('... every face at 27.5°: the roof is that over the cosine', roof.area - eaves * deep / Math.cos(27.5 * Math.PI / 180), 1e-6, 'm²');
        check('... the ridge at 3 + tan 27.5° × 5.125 m', roof.top - (3 + t * 5.125), 1e-9, 'm');
        check('... and it runs the whole 12.15 m', roof.lines.filter((l) => l.kind === 0).reduce((s, l) => s + Math.hypot(l.b[0] - l.a[0], l.b[1] - l.a[1]), 0) - eaves, 1e-6, 'm');
        check('... beneath each gable the wall rises in a triangle of base 10.25 m', Math.abs(roof.walls[0].area - 0.5 * 10.25 * 5.125 * t) + Math.abs(roof.walls[1].area - 0.5 * 10.25 * 5.125 * t) + (roof.walls.length - 2), 1e-6, 'm²');
        check('the roof is counted as it is drawn', q.roof - roof.area, 1e-9, 'm²');
        const face = (wind) => S.facades.find((f) => f.wind === wind);
        check('its south face is 11 × 3 m with 7.2 m² of glass; its west 10 × 3 m with a gable above', Math.abs(face('south').wall - 33) + Math.abs(face('south').glass - 7.2) + Math.abs(face('west').wall - (30 + roof.walls[0].area)), 1e-9, 'm²');
        assert('with no Sun given no hours are counted, and every habitable room has a daylight factor', !S.lit && S.solar_gain === null && S.rooms.length === 7 && S.rooms.filter((x) => x.panes > 0).every((x) => x.daylight_factor > 0 && x.sky > 0 && x.sky <= 90 + 1e-9));
        const living = S.rooms.find((x) => r.storeys[0].rooms[x.room].name === 'Living'), third = S.rooms.find((x) => r.storeys[0].rooms[x.room].name === 'Bedroom 3');
        assert('the living room, with 7.2 m² of glass in two walls, is the best daylit; the third bedroom, with 0.6 m², the least of the bedrooms', S.rooms.every((x) => x.daylight_factor <= living.daylight_factor + 1e-12) && third.daylight_factor < 1 && Math.abs(living.glass - (5.04 + 2.16)) < 1e-9);
        // The Sun due north at 40° for an hour, as on a southern winter's noon: the bedrooms' north windows have it, the south ones none.
        const lit = (await send('POST', '/api/read', { building: house, around: { buildings: [], suns: [[40, 0, 1]] } })).body.shell;
        const hours = (name) => lit.rooms.find((x) => r.storeys[0].rooms[x.room].name === name).sun_hours;
        assert('given the Sun due north at 40° for an hour, the bedrooms that look north have the hour and the kitchen, which looks south, none', lit.lit && hours('Bedroom 1') === 1 && hours('Bedroom 2') === 1 && hours('Kitchen') === 0 && lit.solar_gain > 0);
        // A neighbour 12 m high standing 4 m north of the bedrooms' wall takes that sun and most of their sky.
        const shaded = (await send('POST', '/api/read', { building: house, around: { buildings: [{ ring: [[0, 20.125], [15, 20.125], [15, 30], [0, 30]], height: 12 }], suns: [[40, 0, 1]] } })).body.shell;
        const skyOf = (S_, name) => S_.rooms.find((x) => r.storeys[0].rooms[x.room].name === name).sky;
        assert('a neighbour of 12 m standing 4 m north of them takes that hour, and most of their sky', shaded.neighbours === 1 && shaded.rooms.find((x) => r.storeys[0].rooms[x.room].name === 'Bedroom 1').sun_hours === 0 && skyOf(shaded, 'Bedroom 1') < 0.45 * skyOf(lit, 'Bedroom 1'));
        const flat = (await read(P.apply(house, 'declare', { path: ['roof'], value: { form: 'flat', parapet: 0.4 } }))).body;
        check('made flat, the roof is its footprint with a parapet of 0.4 m about it', Math.abs(flat.shell.roofs[0].area - 11.25 * 10.25) + Math.abs(flat.shell.roofs[0].wall_area - 0.4 * 2 * (11.25 + 10.25)) + Math.abs(flat.shell.roofs[0].top - 3.4), 1e-9);
        const hip = (await read(P.apply(house, 'declare', { path: ['roof'], value: { form: 'hip', pitch: 27.5, eaves: 0.45 } }))).body.shell.roofs[0];
        check('made a hip, the same area, no wall carried up, a ridge of 12.15 − 11.15 = 1 m', Math.abs(hip.area - roof.area) + hip.walls.length + Math.abs(hip.lines.filter((l) => l.kind === 0).reduce((s, l) => s + Math.hypot(l.b[0] - l.a[0], l.b[1] - l.a[1]), 0) - 1), 1e-6);
        const bad = await read(P.apply(house, 'declare', { path: ['roof'], value: { form: 'dome', pitch: 30 } }));
        assert('a roof of no known form is refused with the reason', bad.status === 400 && bad.body.message === 'a roof is flat, a shed, a gable or a hip');
      }
      // The street (ADR 0098): a way 100 m long with a building on each side, read through the server.
      {
        const street = { line: [[0, 0], [100, 0]], strips: [{ kind: 'footpath', width: 3 }, { kind: 'lane', width: 3.5 }, { kind: 'lane', width: 3.5 }, { kind: 'footpath', width: 3 }], need: 1, step: 10,
          scheme: { width: { footpath: { least: 2 }, lane: { most: 3.3, clause: 'a' } }, people: { least: 0.4 } } };
        const around = { buildings: [{ ring: [[0, 10], [100, 10], [100, 30], [0, 30]], height: 20 }, { ring: [[0, -22], [60, -22], [60, -8], [0, -8]], height: 7 }], suns: [[30, 180, 1], [25, 180, 1]] };
        const got = await send('POST', '/api/street', { street, around }), s = got.body;
        assert('a street is read through the server: ten stations, the south side open beyond 60 m', got.status === 200 && s.stations.length === 10 && s.stations[8].faces[1].distance === null && s.stations[2].faces[1].building === 1);
        check('18 m between its faces, enclosed 13.5 / 18, 13 m of strips that fit', Math.abs(s.width_least - 18) + Math.abs(s.enclosure - 13.5 / 18) + Math.abs(s.total - 13) + (s.fits ? 0 : 1), 1e-12);
        assert('what is asked is found one by one: the lanes too wide, cited by their clause; the footpaths and the share for people met', s.findings.length === 6 && s.findings.filter((f) => !f.ok).length === 2 && s.findings.filter((f) => !f.ok).every((f) => f.rule === 'width.lane' && f.clause === 'a'));
        check('the south building, 7 m, may rise to 13 tan 30° and keep the hour of the north footpath, 13 m from it; the north one has no limit', Math.abs(s.rises[1].limit - 13 * Math.tan(Math.PI / 6)) + (s.rises[0].limit === null ? 0 : 1), 1e-5, 'm');
        const refused = await send('POST', '/api/street', { street: { line: [[0, 0]] }, around });
        assert('a street of one point is refused with the reason', refused.status === 400 && /two points/.test(refused.body.message));
      }
      // What stands in the rooms.
      {
        const I = r.interior;
        assert('of the example\'s 29 pieces each is found within its room, apart from the others, its floor free, and reached; no door is in a piece\'s way; the six in its garden are judged by no room',
          I.pieces.length === 35 && I.pieces.every((f, i) => f.ok && (house.furniture[i].outdoor ? f.outdoor && f.room === null : f.room !== null)) && I.doors.length === 7 && I.doors.every((d) => d.swing_blocked_by.length === 0 && d.entered) && I.passage === 0.6);
        assert('reaching is judged of the pieces that obstruct, in rooms with a door: 24 of them', I.pieces.filter((f) => f.judged).length === 24 && I.pieces.every((f, i) => f.judged === (house.furniture[i].obstructs !== false && !house.furniture[i].outdoor)));
        const hall = I.rooms.find((x) => r.storeys[0].rooms[x.room].name === 'Hall'), bath = I.rooms.find((x) => r.storeys[0].rooms[x.room].name === 'Bath');
        check('the console takes 0.35 m² of the hall\'s 11.72: 97% is left free, to the grid', hall.free_share - (1 - 0.35 / named('Hall').net_area), 0.004);
        assert('the bath\'s three fixtures leave about two thirds of its floor', bath.free_share > 0.62 && bath.free_share < 0.8);
        // The wardrobe of the first bedroom moved against the bed's foot: each then stands in the floor the other needs.
        const wardrobe = house.furniture.findIndex((f) => f.kind === 'wardrobe'), bed = house.furniture.findIndex((f) => f.kind === 'bed-double');
        const moved = (await read(P.apply(house, 'setPiece', { piece: wardrobe, at: [4.9, 14.55], bearing: 180 }))).body.interior;
        assert('the wardrobe moved to the bed\'s foot stands in the floor the bed needs, and the bed in the wardrobe\'s; nothing else is disturbed',
          moved.pieces[bed].blocked_by.join() === String(wardrobe) && moved.pieces[wardrobe].blocked_by.join() === String(bed) && moved.pieces.filter((f) => !f.ok).length === 2 && moved.pieces[wardrobe].within);
        const through = (await read(P.apply(house, 'setPiece', { piece: wardrobe, at: [5.2, 12.555] }))).body.interior.pieces[wardrobe];
        assert('pushed 0.45 m through the wall it is not within its room', !through.within && !through.ok);
        // A chair set behind the first bedroom's door.
        const chaired = P.apply(house, 'addPiece', { storey: 0, kind: 'chair', at: [3.7, 12.7], bearing: 90 });
        const cd = (await read(chaired)).body.interior, blocked = cd.doors.filter((d) => d.swing_blocked_by.length);
        assert('a chair behind a bedroom\'s door stands where its leaf swings: that door alone is found, with the chair', blocked.length === 1 && chaired.furniture[blocked[0].swing_blocked_by[0]].kind === 'chair');
        // A person who needs more room: fewer pieces are reached, never more; no one 1.4 m wide stands in a hall of 1.09 m.
        const console_ = house.furniture.findIndex((f) => f.kind === 'console'), reachedAt = [];
        let last = null;
        for (const passage of [0.6, 0.9, 1.4]) { last = (await read(P.apply(house, 'declare', { path: ['interior', 'passage'], value: passage }))).body.interior; reachedAt.push(last.pieces.filter((f) => f.judged && f.reached).length); }
        assert('for a person who needs more room fewer pieces are reached, never more; at 1.4 m the console in the hall of 1.09 m is not', last.passage === 1.4 && reachedAt[0] === 24 && reachedAt[1] <= reachedAt[0] && reachedAt[2] <= reachedAt[1] && !last.pieces[console_].reached,
          reachedAt.join(' '));
        const refused = await read({ ...house, furniture: [{ ...house.furniture[0], storey: 3 }] });
        assert('a piece on a storey the building does not have is refused with the reason', refused.status === 400 && refused.body.message === 'a piece stands on a storey of the building');
      }

      // What is not a plan is refused, and what is not a building is not read.
      const crossed = structuredClone(house);
      crossed.storeys[0].walls.push({ a: 0, b: 2, t: 0.1, kind: 'solid' });
      const bad = (await read(crossed)).body;
      assert('walls that cross are not a plan: the storey is not sound, its faults are named, and the brief is not read', bad.sound === false && bad.storeys[0].rooms.length === 0 && bad.storeys[0].faults.some((x) => x.includes('cross')) && bad.storeys[0].faulty_walls.length >= 2 && bad.brief.length === 0);
      const broken = structuredClone(house);
      broken.storeys[0].walls[0].a = 'north';
      const refused = await read(broken);
      assert('a wall that ends at no index is refused with the reason, and the engine goes on answering', refused.status === 400 && refused.body.message === 'a wall\'s end is an index' && (await read(house)).status === 200);
    }
  } finally {
    server.kill();
    rmSync(store, { recursive: true, force: true });
  }
}

done();
