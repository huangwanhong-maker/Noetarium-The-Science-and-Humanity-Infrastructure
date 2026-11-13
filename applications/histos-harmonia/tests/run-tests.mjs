// Checks of what is Histos Harmonia's own: its page (every element the script asks for, the files it loads), its plug-ins
// (each declared activity has its function, each tour's stops name objects of its record), its recipe, and its server on an
// archive and a library of its own. Where the engine is built from the present sources, the server is asked as the page
// asks it, on a small record made for the checks: the record whole, an object inquired into, the frontier of problems,
// kategoria's verdict, a tuning, an argument map; and the desk's work is done and undone: a source added, a holding taken
// in and kept by its hash, a passage looked for and not found (nothing added), found (the statement established), what is
// written for an activity kept and inherited, a holding altered and found so, a dark holding withheld, a package made,
// another page refused. The domain's own record is then read whole.
// Run from the repository root:  node applications/histos-harmonia/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync, writeFileSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(`histos harmonia: ${name}`, Boolean(ok), ok ? '' : String(detail));
const APP = resolve(ROOT, 'applications/histos-harmonia');
const read = (path) => readFileSync(join(APP, path), 'utf8');

// ---- the page and its plug-ins --------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('the page\'s style sheets and script exist, the drawings\' style sheet among them', !missing.length && local.includes('/visualization/js/history/history.css'), missing.join(', '));
  const ids = [...new Set([...app.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))];
  assert('every element the script asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  const imports = [...app.matchAll(/from '(\/[^']+)'/g)].map((m) => m[1]);
  assert('what the script imports of the visualization layer exists', imports.length >= 2 && imports.every((p) => existsSync(join(ROOT, p))));
  assert('the page is set in serif type, its server names the port, and the page computes no history: it asks', /Source\+Serif\+4/.test(page) && /default=9130/.test(read('serve.py')) && !/topological|rests_on\(|standing_of/.test(app));
  const recipe = JSON.parse(read('recipe.json'));
  assert('its recipe has one step, the engine, done when its build is recorded', recipe.steps.length === 1 && recipe.steps[0].run[0] === 'applications/histos-harmonia/engine.py' && /build\.json$/.test(recipe.steps[0].done));

  const record = readFileSync(join(ROOT, 'domains/history/records/tuning-and-temperament.jsonl'), 'utf8').split('\n').filter(Boolean).map((l) => JSON.parse(l));
  const objects = new Set(record.filter((r) => r.row === 'object').map((r) => r.id));
  const plugins = readdirSync(join(APP, 'plugins'));
  let declared = 0, activities = [];
  for (const name of plugins) {
    const p = JSON.parse(read(`plugins/${name}/plugin.json`));
    const mod = await import(`file://${join(APP, 'plugins', name, p.module).replace(/\\/g, '/').replace(/ /g, '%20')}`);
    declared += p.activities.length;
    assert(`the plug-in ${name} gives a function for each activity it declares, and says what each is about`, p.activities.every((a) => typeof mod.render[a.id] === 'function' && a.name && a.about && a.kinds.length), p.activities.map((a) => a.id).join(', '));
    activities = activities.concat(p.activities.map((a) => a.id));
    for (const t of p.tours || []) assert(`the path “${t.name}” stops at objects of its record, each with a sentence`, t.record === 'tuning-and-temperament' && t.stops.length >= 5 && t.stops.every((s) => objects.has(s.object) && s.say.length > 20), t.stops.filter((s) => !objects.has(s.object)).map((s) => s.object).join(', '));
    for (const k of p.kinds || []) assert(`the kind ${k.id} that ${name} adds is declared in the record it serves`, record.some((r) => r.row === 'kind' && r.id === k.id && r.plugin === name && r.under.join() === k.under.join()));
  }
  assert('three plug-ins declare six activities, each named once', plugins.length === 3 && declared === 6 && new Set(activities).size === 6, activities.join(', '));
  assert('a tour\'s activities are declared', JSON.parse(read('plugins/core/plugin.json')).tours.every((t) => t.stops.every((s) => !s.activity || activities.includes(s.activity))));
  const tuned = record.filter((r) => r.sound).map((r) => r.sound.tuning);
  const api = readFileSync(join(ROOT, 'api/cpp/science/sounding.hpp'), 'utf8');
  assert('every tuning the record asks to be sounded is one the API names', tuned.length >= 8 && tuned.every((t) => api.includes(`"${t}"`) || /^(equal|meantone)-/.test(t)), tuned.join(', '));
}

// ---- the server -----------------------------------------------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
const built = python && JSON.parse(spawnSync(python, [join(APP, 'engine.py'), 'status'], { encoding: 'utf8' }).stdout || '{}').current;
if (!python) skip('histos harmonia: the server (no Python found to run it)');
else if (!built) skip('histos harmonia: the server (its engine is not built from the present sources: python applications/histos-harmonia/engine.py build)');
else {
  const archive = mkdtempSync(join(tmpdir(), 'histos-check-'));
  const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
  const start = (library) => spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--archive', archive, ...(library ? ['--library', library] : [])], { stdio: 'ignore' });
  const at = `http://127.0.0.1:${port}`;
  const ask = async (path, options = {}) => { const r = await fetch(`${at}${path}`, options); return { code: r.status, body: await r.json().catch(() => null) }; };
  const post = (path, body) => ask(path, { method: 'POST', body: JSON.stringify(body) });
  const up = async () => { for (let k = 0; k < 100; k++) { await new Promise((r) => setTimeout(r, 150)); try { if ((await fetch(`${at}/api/records`)).ok) return true; } catch { /* not yet */ } } return false; };
  let server = start(join(APP, 'tests/fixtures'));
  try {
    assert('the server comes up', await up());
    const list = (await ask('/api/records')).body;
    assert('it lists the library\'s record, says its engine is built and that it takes changes here', list.records.length === 1 && list.records[0].id === 'small' && list.records[0].where === 'library' && list.engine.built && list.writable);
    const page = await fetch(`${at}/`), css = await fetch(`${at}/visualization/js/history/history.css`), outside = await fetch(`${at}/..%2f..%2fCLAUDE.md`);
    assert('the page and the visualization layer are served, and nothing outside them', page.ok && css.ok && !outside.ok);

    const R = (await ask('/api/record?record=small')).body;
    assert('the record whole: its objects with their kinds above, years and places, its relations, no fault', R.objects.length === 8 && R.relations.length === 7 && R.faults.length === 0 &&
      R.objects.find((o) => o.id === 'east').appears === 1584 && R.objects.find((o) => o.id === 'treatise').places[0].place === 'town' && R.objects.find((o) => o.id === 'closing').above.join() === 'Idea,Object', JSON.stringify(R.faults));
    assert('every statement is a model\'s proposal: none established', R.statements.established === 0 && R.statements.proposed === 15, JSON.stringify(R.statements));
    const empty = (await ask('/api/object?record=small&id=east')).body, east = (await ask('/api/object?record=small&id=east&layer=proposed')).body;
    assert('at the established layer the record is silent on the eastern answer', empty.past.length === 0 && empty.inquiry.silent.length === 4);
    assert('with the proposed: its problem, why that arose, and the western answer as an independent comparison case', east.inquiry.problems[0].object === 'closing' && east.inquiry.arose[0].object === 'old' &&
      east.comparisons.length === 1 && east.comparisons[0].object === 'west' && east.comparisons[0].independent && east.past.length === 3, JSON.stringify(east.comparisons));
    const f1500 = (await ask('/api/frontier?record=small&date=1500&layer=proposed')).body, f1600 = (await ask('/api/frontier?record=small&date=1600&layer=proposed')).body;
    assert('the problem is open in 1500 with two answers to come, and answered by 1600', f1500.problems[0].open && f1500.problems[0].later.length === 2 && !f1600.problems[0].open && f1600.problems[0].answered.length === 2);
    const verdict = (await ask('/api/verify?record=small')).body;
    assert('kategoria finds the record a functor on its schema', verdict.status === 'holds' && verdict.objects === 8, verdict.reason);
    const gathered = (await post('/api/gather', { record: 'small', classes: ['tempering', 'closing'], layer: 'proposed' })).body;
    assert('dependences gathered between what stands under two objects', gathered.derived.length === 1 && gathered.derived[0].from === 'closing' && gathered.derived[0].to === 'tempering' && gathered.derived[0].count === 1, JSON.stringify(gathered));
    const mean = (await ask('/api/tuning?system=meantone-1/4&first_hz=256')).body, equal = (await ask('/api/tuning?system=equal-12&first_hz=256')).body;
    assert('quarter-comma meantone: eight pure major thirds and a wolf 35.68 cents wide', mean.major_thirds.filter((d) => d.pure).length === 8 && Math.abs(mean.wolf.departure - 35.682) < 0.01, mean.wolf.departure);
    assert('twelve equal semitones: every fifth 1.955 cents narrow, the octave at twice the first degree', equal.fifths.every((d) => Math.abs(d.departure + 1.955) < 1e-3) && Math.abs(equal.tones[12].hz - 512) < 1e-9);
    assert('a tuning with no pitch stated is refused', (await ask('/api/tuning?system=equal-12')).code === 400 || (await ask('/api/tuning?system=nonesuch&first_hz=256')).code === 422);
    const map = (await post('/api/map', { text: 'exclusive question q: Which?\nposition a -> q: This.\nposition b -> q: That.\ncon x -> a: Not this.' })).body;
    assert('an argument map evaluated: the position argued against falls, its rival stands, the question is closed', map.evaluated && map.positions[0].standing.status === 'rejected' && map.positions[1].standing.status === 'accepted' && map.open_questions.length === 0, JSON.stringify(map.positions));
    assert('a map that is not well formed is shown with its faults and not evaluated', (await post('/api/map', { text: 'position a -> nowhere: This.' })).body.evaluated === false);
    const plugins = (await ask('/api/plugins')).body.plugins;
    assert('the plug-ins are described to the page', plugins.length === 3 && plugins.find((p) => p.id === 'core').tours.length === 1);

    // The desk.
    const passage = 'An account of the western answer, in which every semitone is alike.';
    const text = `THE TREATISE\n\n${passage}\n   Printed in a town.\n`;
    const sha = createHash('sha256').update(text).digest('hex');
    const taken = await ask(`/api/holdings?name=treatise.txt&objects=treatise&rights=${encodeURIComponent('Public domain')}&access=public&note=a%20transcription`, { method: 'POST', body: text });
    assert('a holding is taken in and named by the hash of its content', taken.code === 200 && taken.body.sha256 === sha && existsSync(join(archive, 'holdings', sha.slice(0, 2), sha)), JSON.stringify(taken.body));
    assert('a holding with no statement of rights is refused', (await ask('/api/holdings?name=x.txt&access=public', { method: 'POST', body: 'x' })).code === 400);
    assert('it is served to the page, and listed for its object', (await (await fetch(`${at}/api/holding/${sha}`)).text()) === text && (await ask('/api/holdings?object=treatise')).body.holdings.length === 1);
    const claim = { record: 'small', row: 'relation', id: 'r5', by: 'title', grade: 'B', where: 'title page', holding: sha, documentary: true, confirmed: 'a keeper' };
    const wrong = await post('/api/assert', { ...claim, quote: 'in which every semitone differs' });
    assert('a passage not found in the holding adds nothing', wrong.code === 400 && /not found/.test(wrong.body.message) && !existsSync(join(archive, 'records', 'small.jsonl')), wrong.body?.message);
    assert('grade B with no passage, a model\'s grade, and a source the record does not name are refused',
      (await post('/api/assert', { ...claim, quote: '' })).code === 400 && (await post('/api/assert', { ...claim, grade: 'C', quote: passage })).code === 400 && (await post('/api/assert', { ...claim, by: 'nobody', quote: passage })).code === 400);
    const right = await post('/api/assert', { ...claim, quote: 'an account of the  western answer,\nin which every semitone is alike' });
    assert('the passage found, whatever its spacing and capitals: the assertion is added, to the archive\'s own copy', right.code === 200 && /passage found/.test(right.body.added.confirmed) && existsSync(join(archive, 'records', 'small.jsonl')), JSON.stringify(right.body));
    const after = (await ask('/api/record?record=small')).body;
    assert('a confirmed documentary assertion establishes the statement, and only it', after.relations.find((r) => r.id === 'r5').standing === 'established' && after.statements.established === 1 && after.statements.proposed === 14, JSON.stringify(after.statements));
    assert('the library\'s record is as it was', !readFileSync(join(APP, 'tests/fixtures/small.jsonl'), 'utf8').includes('semitone') && (await ask('/api/records')).body.records[0].where === 'archive');
    const added = await post('/api/source', { record: 'small', id: 'book2020', kind: 'reference', cite: 'A book, 2020' });
    assert('a source is added once', added.code === 200 && (await post('/api/source', { record: 'small', id: 'book2020', kind: 'reference', cite: 'A book, 2020' })).code === 400 && (await ask('/api/record?record=small')).body.sources.length === 3);
    await post('/api/assert', { record: 'small', row: 'object', id: 'west', by: 'book2020', grade: 'A', confirmed: 'a keeper' });
    const layer = (await ask('/api/record?record=small')).body;
    assert('one confirmed source attests and does not establish', layer.objects.find((o) => o.id === 'west').standing === 'attested');

    // What is written for an activity.
    const put = (object, doc) => ask(`/api/content?record=small&object=${object}&activity=inquire`, { method: 'PUT', body: JSON.stringify(doc) });
    assert('nothing is written yet', (await ask('/api/content?record=small&object=west&activity=inquire')).body.content === null);
    await put('tempering', { kind: 'document', text: 'Of every tempering.' });
    const inherited = (await ask('/api/content?record=small&object=west&activity=inquire')).body;
    assert('what is written for an object serves what stands under it, and says through what', inherited.content.text === 'Of every tempering.' && inherited.inherited === 'tempering');
    await put('west', { kind: 'program', source: 'return { panes: [] };' });
    const own = (await ask('/api/content?record=small&object=west&activity=inquire')).body;
    assert('until it has its own; and what is neither a document nor a program is refused', own.content.kind === 'program' && own.inherited === null && (await put('west', { kind: 'spell' })).code === 400);

    // Fixity, access, the package, the log, another page.
    const sound = (await ask('/api/fixity', { method: 'POST' })).body;
    writeFileSync(join(archive, 'holdings', sha.slice(0, 2), sha), `${text} `);
    const altered = (await ask('/api/fixity', { method: 'POST' })).body;
    assert('every holding is checked against its hash, and one altered is found', sound.checked === 1 && sound.altered.length === 0 && altered.altered.join() === sha);
    await post('/api/holding', { sha256: sha, access: 'dark' });
    assert('a dark holding is described and not shown', (await fetch(`${at}/api/holding/${sha}`)).status === 403 && (await ask('/api/holdings')).body.holdings[0].access === 'dark');
    const bag = await fetch(`${at}/api/export?record=small`);
    const zip = Buffer.from(await bag.arrayBuffer());
    assert('the record is given as a package, a zip with a manifest and without the dark holding', bag.ok && zip.subarray(0, 2).toString() === 'PK' && zip.includes('manifest-sha256.txt') && zip.includes('bagit.txt') && !zip.includes('treatise.txt'));
    const log = (await ask('/api/log')).body.log.map((e) => e.event);
    assert('what was done is in the log, in order', ['ingested', 'copied', 'asserted', 'source added', 'content kept', 'fixity', 'described', 'exported'].every((e) => log.includes(e)) && log.indexOf('ingested') < log.indexOf('asserted'), log.join(', '));
    const other = await ask('/api/source', { method: 'POST', headers: { Origin: 'http://elsewhere.example' }, body: JSON.stringify({ record: 'small', id: 'x', kind: 'reference', cite: 'X' }) });
    assert('another page changes nothing', other.code === 403);
  } finally {
    server.kill();
  }

  // The domain's own record, through the server.
  await new Promise((r) => setTimeout(r, 400));
  server = start(null);
  try {
    if (await up()) {
      const T = (await ask('/api/record?record=tuning-and-temperament')).body;
      const all = T.statements.established + T.statements.attested + T.statements.proposed;
      assert(`the record of tuning and temperament reads whole: ${T.objects.length} objects, ${T.relations.length} relations, ${all} statements, no fault`, T.objects.length >= 100 && T.faults.length === 0 && all > 200, JSON.stringify(T.faults.slice(0, 2)));
      assert('all of it is a model\'s proposal, and says so: nothing is established before a keeper confirms it', T.statements.established === 0 && T.statements.proposed === all);
      const zhu = (await ask('/api/object?record=tuning-and-temperament&id=equal-zhu&layer=proposed')).body;
      assert('Zhu Zaiyu\'s equal temperament answers the circle that does not close, and Stevin\'s is its independent comparison case',
        zhu.inquiry.problems.some((p) => p.object === 'p-circle') && zhu.comparisons.some((c) => c.object === 'equal-stevin' && c.independent), JSON.stringify(zhu.comparisons));
      const circle = (await ask('/api/object?record=tuning-and-temperament&id=pythagorean&layer=proposed')).body;
      assert('the problem of the circle arises twice, so it does not rest on the Pythagorean tuning alone', !circle.rests_on_it.includes('p-circle') && circle.rests_on_it.includes('meantone'));
      assert('kategoria finds it a functor on its schema', (await ask('/api/verify?record=tuning-and-temperament')).body.status === 'holds');
      const dispute = T.objects.find((o) => o.id === 'dialogo').map;
      const M = (await post('/api/map', { text: dispute })).body;
      assert('the dispute written for Galilei\'s Dialogo is well formed and evaluated', M.evaluated && M.positions.length === 2 && M.arguments.length === 2, JSON.stringify(M.faults));
      for (const o of T.objects.filter((x) => x.sound)) {
        const t = await ask(`/api/tuning?system=${encodeURIComponent(o.sound.tuning)}&first_hz=256`);
        if (t.code !== 200) assert(`the tuning of ${o.id} is sounded`, false, t.body?.message);
      }
      assert('every tuning the record asks for is computed', true);
    } else assert('the server comes up on the domain\'s records', false);
  } finally {
    server.kill();
    rmSync(archive, { recursive: true, force: true });
  }
}

done();
