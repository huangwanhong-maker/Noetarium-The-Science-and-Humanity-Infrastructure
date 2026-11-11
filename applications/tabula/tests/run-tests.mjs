// Checks of what is Tabula's own: its plug-ins (each a pack of sound templates whose schedules are
// read and whose quantities have values, kinds of note that do not collide), its page (the colours of
// its charts apart in every vision), and its server (boards kept, listed, opened and put aside without
// being destroyed; images and files of data kept by their content; what is not a board, not an image
// or not within the store refused; the modules of the plug-ins described; a module or a script run only
// for the server's own page, on what stands on a board; the market's modules on the data library's own
// data, where they are in the cache); and the agent plug-in (agent-checks.mjs: what it may use, its
// tools on a board, its conversation with a scripted model; check_agent_server.py: its key kept and
// never given back; and here, that a plug-in's server side answers only this work's own page, and
// keeps what is the person's outside everything the server serves). The board itself is checked with
// the visualization layer; the hand's work on the page by driving a browser (the README says how).
// Run from the repository root:  node applications/tabula/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readdirSync, readFileSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

import { suite, ROOT, seeded } from '../../../development/tools/checks.mjs';
import { board as B, styles } from '../../../visualization/js/index.js';
import { agentChecks } from './agent-checks.mjs';

const { assert, check, skip, done, lines } = suite();
const APP = resolve(ROOT, 'applications/tabula');

// ---- the plug-ins -------------------------------------------------------------------------------------
const names = readdirSync(join(APP, 'plugins')).filter((n) => existsSync(join(APP, 'plugins', n, 'plugin.js')));
const packs = [];
for (const name of names) packs.push((await import(pathToFileURL(join(APP, 'plugins', name, 'plugin.js')).href)).default(B));
const allCards = { ...B.CARDS, ...Object.assign({}, ...packs.map((p) => p.cards || {})) };
{
  assert('tabula: market analysis is a plug-in, and not the only one', names.includes('market-analysis') && names.length >= 2, names.join(', '));
  const withModules = readdirSync(join(APP, 'plugins')).filter((n) => existsSync(join(APP, 'plugins', n, 'modules')));
  assert('tabula: plug-ins bring modules, market analysis among them', withModules.includes('market-analysis') && withModules.includes('general'), withModules.join(', '));
  assert('tabula: a plug-in is named as its directory is, and brings templates, cards, panels or several of them', packs.every((p, k) => p.id === names[k]) && packs.every((p) => p.name && Array.isArray(p.templates) && (p.templates.length || Object.keys(p.cards || {}).length || (p.panels || []).length)));
  const panels = packs.flatMap((p) => p.panels || []);
  assert('tabula: a panel a plug-in brings has a name of its own, a label for its key in the bar, and is made when it is opened',
    new Set(panels.map((q) => q.id)).size === panels.length && panels.length >= 1 && panels.every((q) => /^[a-z][a-z0-9-]*$/.test(q.id) && q.label && typeof q.mount === 'function'), panels.map((q) => q.id).join(', '));
  const cardNames = packs.flatMap((p) => Object.keys(p.cards || {}));
  assert('tabula: the cards the plug-ins bring are named apart from the board\'s and from each other, each saying what it is and what it asks for',
    new Set([...Object.keys(B.CARDS), ...cardNames]).size === Object.keys(B.CARDS).length + cardNames.length && cardNames.length >= 3
    && packs.every((p) => Object.values(p.cards || {}).every((c) => c.label && c.about && c.size && typeof c.render === 'function' && Array.isArray(c.form({ props: c.props, tables: new Map(), services: {} })))), cardNames.join(', '));
  const types = [...B.NOTE_TYPES, ...packs.flatMap((p) => p.noteTypes || [])];
  assert('tabula: the kinds of note of the plug-ins collide neither with the board\'s nor with each other, and each takes a tone the board has',
    new Set(types.map((t) => t.id)).size === types.length && types.every((t) => B.TONES.includes(t.tone) && t.glyph && t.label));
  const known = new Set(types.map((t) => t.id));
  const rnd = seeded(7), id = () => B.makeId(rnd);
  const wrong = [];
  let count = 0;
  for (const pack of [B.GENERAL, ...packs]) {
    if (new Set(pack.templates.map((t) => t.id)).size !== pack.templates.length) wrong.push(`${pack.id}: two templates share a name`);
    for (const template of pack.templates) {
      count++;
      const where = `${pack.id}/${template.id}`;
      let board;
      try { board = B.apply(B.emptyBoard(), B.instantiate(template, { at: { x: 0, y: 0 }, id }).ops).board; } catch (error) { wrong.push(`${where}: ${error.message}`); continue; }
      const problems = B.checkBoard(board);
      if (problems.length) wrong.push(`${where}: ${problems[0]}`);
      if (B.membershipOps(board, board.order).length) wrong.push(`${where}: an item's region is not the one it lies in`);
      if (!template.name || !template.about) wrong.push(`${where}: no name or no word of what it is for`);
      for (const i of board.order) {
        const item = board.items[i];
        if (item.kind === 'note' && !known.has(item.type)) wrong.push(`${where}: a note of the unknown kind '${item.type}'`);
        if (item.kind === 'card' && !allCards[item.card]) wrong.push(`${where}: a card '${item.card}' that no plug-in brings`);
        if (item.kind === 'schedule') {
          const parsed = B.parseSchedule(item.source), r = B.resolveSchedule(parsed.tasks);
          if (parsed.errors.length || r.errors.length || r.warnings.length || !r.tasks.length) wrong.push(`${where}: its schedule is not read: ${[...parsed.errors, ...r.errors, ...r.warnings].map((e) => e.message).join('; ')}`);
        }
      }
      for (const [name, v] of B.evaluate(B.collect(board))) if (v.error) wrong.push(`${where}: '${name}' has no value: ${v.error}`);
      // No two notes of a template lie on each other.
      const notes = board.order.filter((i) => board.items[i].kind === 'note');
      if (notes.some((a, k) => notes.slice(k + 1).some((b) => B.intersects(board.place[a], board.place[b])))) wrong.push(`${where}: two notes lie on each other`);
    }
  }
  assert(`tabula: the ${count} templates of the board and the plug-ins each lay a sound board: memberships as they lie, kinds known, schedules read, quantities valued, no note on another`, !wrong.length, wrong.join('; '));

  // The size of a market, from the template's placeholders: three measures, each within the last.
  const sizing = packs.find((p) => p.id === 'market-analysis').templates.find((t) => t.id === 'sizing');
  const board = B.apply(B.emptyBoard(), B.instantiate(sizing, { id }).ops).board;
  const v = B.evaluate(B.collect(board)), tam = v.get('TAM'), sam = v.get('SAM'), som = v.get('SOM');
  check('tabula: all who could buy are the buyers with the need times what one spends', tam.mid - 120000 * 0.3 * 240, 1e-6, 'a year');
  check('tabula: those who can be won are a part of a part of them', som.mid - 120000 * 0.3 * 240 * 0.5 * 0.08, 1e-6, 'a year');
  assert('tabula: the three measures are nested in their likely values and in both bounds, and each knows the assumptions it rests on',
    som.mid < sam.mid && sam.mid < tam.mid && som.lo < sam.lo && sam.lo < tam.lo && som.hi < sam.hi && sam.hi < tam.hi
    && som.rests.length === 5 && tam.rests.length === 3 && B.formatValue(som) === '346 k a year (50.4 k – 1.29 M)', B.formatValue(som));
  // Change one assumption by an operation: the figures follow.
  const need = board.order.find((i) => (board.items[i].text || '').includes('need ='));
  const changed = B.apply(board, [{ t: 'set', id: need, patch: { text: 'need = 60% [50%, 70%]' } }]).board;
  check('tabula: an assumption doubled on its note doubles what rests on it', B.evaluate(B.collect(changed)).get('SOM').mid / som.mid - 2, 1e-12);
}

// ---- the agent ----------------------------------------------------------------------------------------
await agentChecks({ assert, check, B, packs: packs.filter((p) => p.id !== 'ai-agent'), dir: join(APP, 'plugins', 'ai-agent') });

// ---- the page -----------------------------------------------------------------------------------------
{
  const page = readFileSync(join(APP, 'index.html'), 'utf8'), app = readFileSync(join(APP, 'src/app.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('tabula: the page\'s style sheets and script exist, the board\'s style sheet among them', !missing.length && local.includes('/visualization/js/board/board.css'), missing.join(', '));
  const ids = [...app.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]);
  assert('tabula: every element the script asks for is on the page', ids.length > 10 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  // The colours of its charts, by day and by night: neighbours apart in normal vision and under each deficiency.
  const sheet = readFileSync(join(APP, 'styles.css'), 'utf8');
  const series = (text) => [1, 2, 3, 4, 5, 6].map((n) => new RegExp(`--tb-series-${n}:\\s*(#[0-9a-f]{6})`).exec(text)?.[1]);
  const day = series(sheet.slice(0, sheet.indexOf("[data-theme='night']"))), night = series(sheet.slice(sheet.indexOf("[data-theme='night']")));
  for (const [name, colours] of [['day', day], ['night', night]]) {
    const apart = colours.every(Boolean) ? styles.separation(colours.map(styles.hexRgb)) : {};
    const least = Math.min(...Object.values(apart), Infinity);
    check(`tabula: the six colours of its charts by ${name} stay apart from their neighbours in every vision (shortfall of 0.05)`, colours.every(Boolean) ? Math.max(0, 0.05 - least) : 1, 0);
  }
  assert('tabula: the page is set in serif type and names the port in its server', /Source\+Serif\+4/.test(page) && /default=9010/.test(readFileSync(join(APP, 'serve.py'), 'utf8')));
}

// ---- the server ---------------------------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
if (!python) skip('tabula: the server (no Python found to run it)');
else {
  const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
  // The agent's side of the server, with a provider that asks no one.
  lines(spawnSync(python, [join(APP, 'tests', 'check_agent_server.py'), join(APP, 'plugins', 'ai-agent', 'server.py')], { encoding: 'utf8' }).stdout || 'FAIL  tabula: the checks of the agent\'s server side did not run');
  // The server keeps the person's settings in a folder of these checks, never in the person's own.
  const store = mkdtempSync(join(tmpdir(), 'tabula-')), settings = mkdtempSync(join(tmpdir(), 'tabula-settings-'));
  const server = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store, '--settings', settings], { stdio: 'ignore' });
  const at = `http://127.0.0.1:${port}`;
  try {
    let up = false;
    for (let k = 0; k < 60 && !up; k++) { await new Promise((r) => setTimeout(r, 150)); try { up = (await fetch(`${at}/api/boards`)).ok; } catch { /* not yet */ } }
    assert('tabula: the server answers', up);
    if (up) {
      const json = async (path, options) => { const r = await fetch(`${at}${path}`, options); return { status: r.status, body: await r.json().catch(() => null) }; };
      const listed = (await json('/api/plugins')).body.plugins.map((p) => p.id);
      assert('tabula: the server lists the plug-ins that are there', JSON.stringify(listed) === JSON.stringify(names.slice().sort()), listed.join(', '));
      const rnd = seeded(9);
      const template = packs[0].templates[0];
      const board = { ...B.apply(B.emptyBoard({ id: 'a-board_1', title: 'Kept — 市場' }), B.instantiate(template, { id: () => B.makeId(rnd) }).ops).board, meta: { modified: '2026-11-11T00:00:00Z' } };
      const put = (id, body) => json(`/api/boards/${id}`, { method: 'PUT', headers: { 'Content-Type': 'application/json' }, body: typeof body === 'string' ? body : JSON.stringify(body) });
      const kept = await put('a-board_1', board), list = (await json('/api/boards')).body, back = (await json('/api/boards/a-board_1')).body;
      assert('tabula: a board kept is listed with its title and opened as it was kept', kept.status === 200 && list.boards.length === 1 && list.boards[0].title === 'Kept — 市場' && list.boards[0].items === board.order.length
        && JSON.stringify(back) === JSON.stringify(board) && !B.checkBoard(back).length);
      const refusals = [await put('other', board), await put('a-board_1', '{"schema":"something else"}'), await put('a-board_1', 'not json'), await put('bad%20id', board), await json('/api/boards/none')];
      assert('tabula: a board under another name, what is not a board, what is not JSON, a bad name and a board that is not there are refused', refusals.map((r) => r.status).join() === '400,400,400,404,404' && refusals.every((r) => r.body?.message), refusals.map((r) => r.status).join());
      // A one-pixel PNG, kept by its content.
      const png = Buffer.from('iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8BQDwAEhQGAhKmMIQAAAABJRU5ErkJggg==', 'base64');
      const image = await json('/api/files', { method: 'POST', headers: { 'Content-Type': 'image/png' }, body: png }), again = await json('/api/files', { method: 'POST', headers: { 'Content-Type': 'image/png' }, body: png });
      const got = image.body?.src ? await fetch(`${at}${image.body.src}`) : null;
      assert('tabula: an image is kept by its content, once, and served as it was', image.status === 200 && again.body.src === image.body.src && got?.ok && got.headers.get('content-type') === 'image/png' && Buffer.from(await got.arrayBuffer()).equals(png)
        && readdirSync(join(store, 'files')).length === 1);
      const svg = await json('/api/files', { method: 'POST', headers: { 'Content-Type': 'image/svg+xml' }, body: '<svg xmlns="http://www.w3.org/2000/svg"/>' });
      const outside = await Promise.all(['/files/../boards/a-board_1.json', '/files/%2e%2e/boards/a-board_1.json', '/..%2f..%2fCLAUDE.md', '/files/serve.py'].map((p) => fetch(`${at}${p}`).then((r) => r.status)));
      assert('tabula: an image that could carry a script is refused, and nothing outside the store or the page is served', svg.status === 415 && outside.every((s) => s === 404), `${svg.status} ${outside.join()}`);

      // ---- modules: described, run only for the server's own page, on what stands on a board ----------------
      const described = (await json('/api/modules')).body.modules;
      const refs = described.map((m) => m.ref);
      assert('tabula: the server describes the modules of the plug-ins, each with what it is and the parameters it needs',
        described.length >= 7 && described.every((m) => !m.error && m.title && m.description && Array.isArray(m.params)) && refs.includes('market-analysis/segments.py') && refs.includes('general/figure_from_file.py'),
        described.filter((m) => m.error).map((m) => `${m.ref}: ${m.error}`).join('; '));
      const roles = Object.fromEntries(described.flatMap((m) => m.params.filter((q) => q.role).map((q) => [`${m.ref}:${q.name}`, q.role])));
      assert('tabula: their parameters say what they name: a table, columns of it, a quantity of the board, a file',
        roles['market-analysis/segments.py:table'] === 'table' && roles['market-analysis/segments.py:columns'] === 'columns' && roles['market-analysis/potential.py:adoption'] === 'quantity' && roles['general/figure_from_file.py:file'] === 'file');
      const asked = [B.GENERAL, ...packs].flatMap((p) => p.templates.flatMap((t) => t.items.filter((i) => i.kind === 'card' && i.props?.module).map((i) => i.props.module)));
      assert('tabula: every module a template lays on the board is one the server has', asked.length >= 4 && asked.every((r) => refs.includes(r)), asked.filter((r) => !refs.includes(r)).join(', '));

      const own = { 'Content-Type': 'application/json', 'X-Tabula': 'run', Origin: at };
      const start = (request, headers = own) => json('/api/run', { method: 'POST', headers, body: JSON.stringify(request) });
      const finish = async (request) => {
        const begun = await start(request);
        if (begun.status !== 200) return { state: 'refused', message: begun.body?.message };
        for (let k = 0; k < 600; k++) {
          await new Promise((r) => setTimeout(r, 100));
          const j = (await json(`/api/job/${begun.body.job}`)).body;
          if (j.state !== 'running') return j;
        }
        return { state: 'never ended' };
      };
      const code = 'def run(board, api):\n    t = board.table("towns")\n    return [api.table("dear", {"city": t.text("city"), "twice": t.column("rent") * 2 * float(board.quantity("scale"))}), api.note(f"{t.rows} rows")]\n';
      const given = { tables: { towns: { columns: [{ name: 'city', kind: 'text' }, { name: 'rent', kind: 'number' }], data: { city: ['A', 'B'], rent: [10, null] } } }, quantities: { scale: { lo: 1, mid: 1.5, hi: 2, unit: '' } }, notes: [] };
      const ran = await finish({ code, board: given });
      assert('tabula: a script written on a board is run on its tables and quantities, and answers a table and a note',
        ran.state === 'done' && ran.results?.[0]?.name === 'dear' && JSON.stringify(ran.results[0].data.twice) === '[30,null]' && ran.results[1].text === '2 rows', ran.message || JSON.stringify(ran.results));
      const failed = await finish({ code: 'def run(board, api):\n    return [api.table("x", {"a": [1], "b": [1, 2]})]\n', board: given });
      assert('tabula: a script that fails is told to the page with its reason, and the server lives', failed.state === 'failed' && /differ in length/.test(failed.message) && (await json('/api/boards')).status === 200, failed.message);
      const refusals2 = await Promise.all([start({ code }, { 'Content-Type': 'application/json', Origin: at }), start({ code }, { ...own, Origin: 'http://elsewhere.example' }),
        start({ code }, { 'Content-Type': 'text/plain', 'X-Tabula': 'run', Origin: at }), start({ code }, { 'Content-Type': 'application/json', 'X-Tabula': 'run' }), start({ module: 'market-analysis/../../serve.py' }), start({})]);
      assert('tabula: a run is refused to what is not this work\'s own page (no mark, another origin, not JSON, no origin), and for a module that is not one',
        refusals2.map((r) => r.status).join() === '403,403,403,403,404,400' && refusals2.every((r) => r.body?.message), refusals2.map((r) => r.status).join());
      const foreign = await json('/api/boards/a-board_1', { method: 'PUT', headers: { 'Content-Type': 'application/json', Origin: 'http://elsewhere.example' }, body: JSON.stringify(board) });
      assert('tabula: a page of another origin keeps nothing here', foreign.status === 403);

      // A file of data kept, named to a module by a parameter, read and given to the board.
      const keptFile = await json('/api/files?name=sales.csv', { method: 'POST', body: 'year;sales\n2020;10\n2021;14\n2022;13\n' });
      const notData = await json('/api/files?name=run.exe', { method: 'POST', body: 'MZ' });
      const served = keptFile.body?.ref ? await fetch(`${at}/files/${keptFile.body.ref.slice(5)}`).then((r) => r.status) : 0;
      assert('tabula: a file of data is kept by its content for the modules, not served back, and only as rows and columns or JSON', keptFile.status === 200 && /^kept:[0-9a-f]{32}\.csv$/.test(keptFile.body.ref) && notData.status === 415 && served === 404, `${keptFile.status} ${notData.status} ${served}`);
      const drawn = await finish({ module: 'general/figure_from_file.py', params: { file: keptFile.body.ref, x: 'year', y: 'sales' }, board: {} });
      const table = drawn.results?.find((r) => r.kind === 'table'), figure = drawn.results?.find((r) => r.kind === 'figure');
      assert('tabula: a module is given the file its parameter names, reads it, and answers its table' + (figure ? ' and a figure it drew' : ' (no matplotlib here: no figure)'),
        drawn.state === 'done' && table?.name === 'from_file' && JSON.stringify(table.data.sales) === '[10,14,13]' && (!figure || (figure.format === 'svg' && figure.data.includes('<svg'))), drawn.message || '');

      // The market's modules, on the data library's own data where they are there.
      const economies = await finish({ module: 'market-analysis/economies.py', params: { year: 2023 }, board: {} });
      if (economies.state !== 'done' || !economies.results?.some((r) => r.kind === 'table')) skip(`tabula: the market's modules on real data (${(economies.message || 'no economies').split('\n').pop().slice(0, 120)})`);
      else {
        const eco = economies.results.find((r) => r.kind === 'table');
        const onBoard = { tables: { economies: eco }, quantities: { adoption: { lo: 0.01, mid: 0.02, hi: 0.04, unit: '' }, wallet: { lo: 0.003, mid: 0.005, hi: 0.01, unit: '' } }, notes: [] };
        const worth = await finish({ module: 'market-analysis/potential.py', params: {}, board: onBoard }), pot = worth.results?.find((r) => r.kind === 'table');
        const row = eco.data.iso3.indexOf(pot?.data.iso3[0]);
        assert('tabula: the economies of a year come from the data library, each with its people and its income, aggregates left out',
          eco.rows > 150 && eco.data.iso3.every((c) => /^[A-Z]{3}$/.test(c)) && !eco.data.region.includes('Aggregates') && eco.data.population.every((v) => v > 0) && /World Bank/.test(eco.source), `${eco.rows} rows`);
        check('tabula: what a market could be worth is the people in towns times the share who would buy times income times the share spent (relative)',
          pot.data.value[0] / (eco.data.urban_population[row] * 0.02 * eco.data.gdp_per_capita[row] * 0.005) - 1, 1e-12);
        assert('tabula: its range is that of the two assumptions, the economies in order of their worth, their shares summing to all',
          pot.data.value.every((v, k) => pot.data.value_low[k] <= v && v <= pot.data.value_high[k] && (k === 0 || v <= pot.data.value[k - 1])) && Math.abs(pot.data.value_high[0] / pot.data.value_low[0] - (0.04 * 0.01) / (0.01 * 0.003)) < 1e-9
          && Math.abs(pot.data.share.reduce((a, v) => a + v, 0) - 100) < 1e-9);
        const noNote = await finish({ module: 'market-analysis/potential.py', params: {}, board: { tables: { economies: eco } } });
        assert('tabula: without the assumptions on the board the module says which note to write', noNote.state === 'failed' && /adoption/.test(noNote.message) && /on a note/.test(noNote.message), (noNote.message || '').split('\n').pop());
        const kinds = await finish({ module: 'market-analysis/segments.py', params: { k: 4, seed: 1 }, board: onBoard }), again = await finish({ module: 'market-analysis/segments.py', params: { k: 4, seed: 1 }, board: onBoard });
        const seg = kinds.results?.find((r) => r.name === 'segments'), profile = kinds.results?.find((r) => r.name === 'segments_profile');
        assert('tabula: k-means gathers every economy into one of k segments, numbered from the least income, the same on the same seed',
          seg?.rows === eco.rows && new Set(seg.data.segment).size === 4 && profile.data.rows.reduce((a, v) => a + v, 0) === eco.rows
          && profile.data.median_gdp_per_capita.every((v, k) => k === 0 || v >= profile.data.median_gdp_per_capita[k - 1])
          && JSON.stringify(again.results.find((r) => r.name === 'segments').data.segment) === JSON.stringify(seg.data.segment), kinds.message || '');
      }

      // ---- the plug-ins of a run and of a history ----------------------------------------------------------
      const beliefs = { quantities: { innovation: { lo: 0.01, mid: 0.03, hi: 0.05, unit: '' }, imitation: { lo: 0.25, mid: 0.38, hi: 0.5, unit: '' } } };
      const mixed = await finish({ module: 'agents/diffusion.py', params: { reach: 'everyone', side: 100, runs: 10, steps: 30, seed: 3 }, board: beliefs });
      const curve = mixed.results?.find((r) => r.name === 'spread');
      // The mean field of the model, in discrete time: F(t+1) = F(t) + (1 − F(t)) (p + q F(t)).
      const field = [0];
      for (let k = 0; k < 30; k++) field.push(field[k] + (1 - field[k]) * (0.03 + 0.38 * field[k]));
      check('tabula: agents who look to everyone follow Bass\'s curve, the mean of ten runs of ten thousand within 0.01 of it', curve ? Math.max(0, Math.max(...curve.data.mean.map((v, k) => Math.abs(v - field[k]))) - 0.01) : 1, 0);
      check('tabula: the curve the module answers is that recursion', curve ? Math.max(...curve.data.bass.map((v, k) => Math.abs(v - field[k]))) : 1, 1e-12);
      assert('tabula: the curves at the low and high ends of the two assumptions enclose it, and the runs enclose their mean',
        Boolean(curve) && curve.data.bass.every((v, k) => curve.data.bass_low[k] <= v + 1e-12 && v <= curve.data.bass_high[k] + 1e-12) && curve.data.mean.every((v, k) => curve.data.low[k] <= v + 1e-12 && v <= curve.data.high[k] + 1e-12), mixed.message || '');
      const near = await finish({ module: 'agents/diffusion.py', params: { side: 30, runs: 5, steps: 25, seed: 3 }, board: beliefs }), nearAgain = await finish({ module: 'agents/diffusion.py', params: { side: 30, runs: 5, steps: 25, seed: 3 }, board: beliefs });
      const buyers = near.results?.find((r) => r.name === 'spread_buyers'), first = near.results?.find((r) => r.name === 'spread');
      assert('tabula: among neighbours, each buyer of the first run adopts at a step, the share at each step is the buyers who had adopted by it, and a seed gives the run again',
        buyers?.rows === 900 && first.data.share.every((v, k) => Math.abs(v - buyers.data.adopted_at.filter((w) => w !== null && w <= k).length / 900) < 1e-12)
        && first.data.share.every((v, k) => k === 0 || v >= first.data.share[k - 1]) && JSON.stringify(nearAgain.results.find((r) => r.name === 'spread_buyers').data) === JSON.stringify(buyers.data), near.message || '');
      const notChance = await finish({ module: 'agents/diffusion.py', params: {}, board: { quantities: { innovation: { lo: 0.5, mid: 1.5, hi: 2, unit: '' }, imitation: beliefs.quantities.imitation } } });
      assert('tabula: a chance that is not within 0 and 1 is refused, with the note it comes from', notChance.state === 'failed' && /innovation/.test(notChance.message) && /within 0 and 1/.test(notChance.message), (notChance.message || '').split('\n').pop());

      const canon = await finish({ module: 'history/canon.py', params: { canon: 'science-1543-1727' }, board: {} }), athens = await finish({ module: 'history/canon.py', params: { canon: 'athens-470-322bc', holding: 'socrates' }, board: {} });
      const happened = canon.results?.find((r) => r.name === 'events'), old = athens.results?.find((r) => r.name === 'events');
      assert('tabula: the occasions of a canon of the history domain are read, in the order of time, each with a date the board\'s line of time can read',
        happened?.rows === 100 && happened.data.year.every((v, k) => k === 0 || v >= happened.data.year[k - 1]) && happened.data.when.every((w) => B.parseDate(w)) && happened.data.until.every((w) => w === null || B.parseDate(w))
        && happened.data.year[0] >= 1543 && happened.data.year[99] <= 1728 && /not yet checked/.test(canon.results.find((r) => r.kind === 'note').text), canon.message || '');
      assert('tabula: years before 1 are read as the codebook writes them, and a canon is searched by what its occasions hold',
        old?.rows > 3 && old.rows < 75 && old.data.year.every((v) => v < -320 && v > -471) && old.data.when.every((w) => B.civil(B.parseDate(w).day).y < 0) && old.data.who.concat(old.data.label).some((w) => /Socrates/.test(w)), athens.message || `${old?.rows}`);
      // A plug-in's server side: the agent's settings, asked as this work's own page asks.
      const page = { 'X-Tabula': 'run' }, posting = { 'Content-Type': 'application/json', 'X-Tabula': 'run', Origin: at };
      const ask = (path, headers, body) => json(`/api/plugin/${path}`, body === undefined ? { headers } : { method: 'POST', headers, body: JSON.stringify(body) });
      const secret = 'sk-test-not-a-real-key-4Zq9';
      const [bare, elsewhere, asText, noOrigin, read] = await Promise.all([ask('ai-agent/settings', {}), ask('ai-agent/settings', { ...posting, Origin: 'http://elsewhere.example' }, { key: secret }),
        ask('ai-agent/settings', { ...posting, 'Content-Type': 'text/plain' }, { key: secret }), ask('ai-agent/settings', { 'Content-Type': 'application/json', 'X-Tabula': 'run' }, { key: secret }), ask('ai-agent/settings', page)]);
      assert('tabula: a plug-in\'s server side answers only this work\'s own page: a request without the page\'s mark, from another page, or not as the page sends it is refused, and nothing is kept',
        bare.status === 403 && elsewhere.status === 403 && asText.status === 403 && noOrigin.status === 403 && read.status === 200 && read.body.keys.anthropic.kept === false && readdirSync(settings).length === 0, `${bare.status} ${elsewhere.status} ${asText.status} ${noOrigin.status} ${read.status}`);
      const keptKey = await ask('ai-agent/settings', posting, { key: secret, allowed: ['general', 'research'], modules: false }), readAgain = await ask('ai-agent/settings', page);
      const everywhere = (dir) => (existsSync(dir) ? readdirSync(dir, { recursive: true, withFileTypes: true }).filter((e) => e.isFile()).map((e) => join(e.parentPath ?? e.path, e.name)) : []);
      const holding = (dir) => everywhere(dir).filter((f) => readFileSync(f).includes(secret));
      assert('tabula: a key given to the agent is kept in the folder of the person\'s settings and nowhere the server serves; the page is told that one is kept and how it ends, never the key',
        keptKey.status === 200 && !JSON.stringify(keptKey.body).includes(secret) && !JSON.stringify(readAgain.body).includes(secret) && readAgain.body.keys.anthropic.kept === true && readAgain.body.keys.anthropic.ends === '…4Zq9'
        && holding(settings).length === 1 && holding(store).length === 0 && JSON.stringify(readAgain.body.allowed) === '["general","research"]' && readAgain.body.modules === false, `${holding(settings).length} ${holding(store).length}`);
      const [noSide, noPlugin, noPath, noStep, unknown, source] = await Promise.all([ask('research/settings', page), ask('nobody/settings', page), json('/api/plugin/..%2F..%2Fserve/x', { headers: page }),
        ask('ai-agent/messages', posting, { system: 'S' }), ask('ai-agent/keys', page), fetch(`${at}/plugins/ai-agent/server.py`).then((r) => r.text())]);
      assert('tabula: a plug-in without a server side, a plug-in that is not there and a path that is none are answered as such; a step of the agent without a conversation is refused before anyone is asked',
        noSide.status === 404 && /no server side/.test(noSide.body.message) && noPlugin.status === 404 && noPath.status === 404 && noStep.status === 400 && unknown.status === 404 && !source.includes(secret), `${noSide.status} ${noPlugin.status} ${noPath.status} ${noStep.status} ${unknown.status}`);

      const aside = await json('/api/boards/a-board_1', { method: 'DELETE' }), after = (await json('/api/boards')).body;
      assert('tabula: a board put aside is no longer listed, and is not destroyed', aside.status === 200 && after.boards.length === 0 && readdirSync(join(store, 'trash')).length === 1
        && JSON.stringify(JSON.parse(readFileSync(join(store, 'trash', readdirSync(join(store, 'trash'))[0]), 'utf8'))) === JSON.stringify(board));
    }
  } finally {
    server.kill();
    await new Promise((r) => setTimeout(r, 200));
    for (const dir of [store, settings]) try { rmSync(dir, { recursive: true, force: true }); } catch { /* the folder is the system's to clear */ }
  }
}

done();
