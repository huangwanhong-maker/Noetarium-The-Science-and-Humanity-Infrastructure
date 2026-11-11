// Checks of what is Market Studio's own: its page (every element the script asks for, the files it loads), its recipe, its
// predefined tools and its template (sound as a project, every step of its plan a tool that is there), and its server on a
// store of its own: a project begun, a file of data kept as it is and read as a table, its rows within a slice the same by
// the server's rule and by the page's, a filter, the tools described with their parameters, a run refused to another page,
// tools run on a slice and their answers kept as tables (counts and sums by hand, openings and closings by hand, places in
// areas), names held apart, a formula that is not arithmetic refused, a script of the project's own kept, run and its figure
// read back, a board kept, and files outside the store refused. Where the engine is built and the example's data are in the
// cache, the template's plan is run on Melbourne's cafés: the market conserves what buyers bring, the size is the product
// of its quantities, survival falls, and the sites' gains do not rise.
// Run from the repository root:  node applications/market-studio/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';
import { board as B, workspace as W } from '../../../visualization/js/index.js';

const { assert: say, check, skip, done } = suite();
const assert = (name, ok, detail = '') => say(name, ok, ok ? '' : detail);   // what was found is said only where it is wrong
const APP = resolve(ROOT, 'applications/market-studio');
const read = (path) => readFileSync(join(APP, path), 'utf8');

// ---- the page -------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('market studio: the page\'s style sheets and script exist, the board\'s style sheet among them', !missing.length && local.includes('/visualization/js/board/board.css'), missing.join(', '));
  const ids = [...new Set([...app.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => !['progress', 'progress-text'].includes(i));
  assert('market studio: every element the script asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  const imports = [...app.matchAll(/from '(\/[^']+)'/g)].map((m) => m[1]);
  assert('market studio: what the script imports of the visualization layer exists, the workspace and the board among it',
    imports.length >= 7 && imports.every((p) => existsSync(join(ROOT, p))) && imports.some((p) => p.includes('/workspace/')) && imports.some((p) => p.includes('/board/')), imports.filter((p) => !existsSync(join(ROOT, p))).join(', '));
  assert('market studio: the page is set in serif type, its server names the port, and the page names no market: the tools do',
    /Source\+Serif\+4/.test(page) && /default=9030/.test(read('serve.py')) && !/huff|hhi|herfindahl/i.test(app));
}

// ---- the recipe, the tools and the template ------------------------------------------------------------------
const groups = readdirSync(join(APP, 'tools'), { withFileTypes: true }).filter((d) => d.isDirectory() && !d.name.startsWith('_')).map((d) => d.name);
const toolFiles = groups.flatMap((g) => readdirSync(join(APP, 'tools', g)).filter((f) => f.endsWith('.py') && !f.startsWith('_')).map((f) => `${g}/${f}`));
{
  const recipe = JSON.parse(read('recipe.json'));
  const missing = recipe.steps.filter((s) => { const [script, arg] = s.run; return !existsSync(join(ROOT, script)) || (script.endsWith('fetch.py') && !existsSync(join(ROOT, 'data/registry', `${arg}.json`))); });
  assert('market studio: the recipe: every step a script and, for fetching, a manifest that exist; the engine first', missing.length === 0 && recipe.steps[0].id === 'engine', missing.map((s) => s.id).join(', '));
  assert('market studio: the predefined tools stand in groups, each described, each tool a module that declares itself',
    groups.length >= 3 && toolFiles.length >= 14 && groups.every((g) => JSON.parse(read(`tools/${g}/group.json`)).title) && toolFiles.every((f) => /def create\(api\)/.test(read(`tools/${f}`)) && /params = /.test(read(`tools/${f}`))), `${groups.length} groups, ${toolFiles.length} tools`);
  for (const file of readdirSync(join(APP, 'templates')).filter((f) => f.endsWith('.json'))) {
    const t = JSON.parse(read(`templates/${file}`)), p = { ...W.emptyProject(), ...t.project, steps: [] };
    const wrong = W.checkProject(p), plan = t.project.plan || [];
    assert(`market studio: the template ${file} is a sound project, and every step of its plan is a tool that is there, on a slice that is there`,
      wrong.length === 0 && !!t.title && !!t.about && plan.length > 0 && plan.every((e) => toolFiles.includes(e.tool) && (e.slice === null || p.slices.some((s) => s.id === e.slice)) && e.id && e.title), wrong.join('; '));
    assert(`market studio: the template ${file} names its data in the data library, and every view draws a table that is there or that its plan will answer`,
      p.sources.every((s) => /^data:/.test(s.ref)) && p.views.every((v) => p.sources.some((s) => s.name === v.table)));
  }
}

// ---- the server -----------------------------------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
const numpy = python && spawnSync(python, ['-c', 'import numpy'], { encoding: 'utf8' }).status === 0;
if (!python || !numpy) skip('market studio: the server and its tools (no Python with numpy found to run them)');
else {
  const store = mkdtempSync(join(tmpdir(), 'market-studio-check-'));
  const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
  const server = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store], { stdio: 'ignore' });
  const at = `http://127.0.0.1:${port}`;
  try {
    let up = false, status = null;
    for (let k = 0; k < 100 && !up; k++) { await new Promise((r) => setTimeout(r, 150)); try { const r = await fetch(`${at}/api/status`); up = r.ok; status = await r.json(); } catch { /* not yet */ } }
    assert('market studio: the server answers, and says whether its engine is built and whether tools can be run', up && typeof status?.engine?.current === 'boolean' && status.tools === null && status.writable === true);
    if (up) {
      const json = async (path, options) => { const r = await fetch(`${at}${path}`, options); return { status: r.status, body: await r.json().catch(() => undefined) }; };
      const send = (method, path, body, headers = {}) => json(path, { method, headers: { 'Content-Type': 'application/json', ...headers }, body: typeof body === 'string' ? body : JSON.stringify(body) });
      const own = { Origin: at, 'X-Studio': 'run' };
      const run = async (project, body, headers = own) => {
        const started = await send('POST', `/api/projects/${project}/run`, body, headers);
        if (started.status !== 200) return { state: 'refused', status: started.status, message: started.body?.message };
        for (let k = 0; k < 2400; k++) { await new Promise((r) => setTimeout(r, 120)); const j = (await json(`/api/job/${started.body.job}`)).body; if (j.state !== 'running') return j; }
        return { state: 'late' };
      };
      const tableOf = async (project, name, slice = null) => (await json(`/api/projects/${project}/table?name=${name}${slice ? `&slice=${slice}` : ''}`)).body;

      // A project begun empty; a file of data kept as it is; read as a table.
      const templates = (await json('/api/templates')).body.templates;
      const begun = await send('POST', '/api/projects', { title: 'A check — 市場' });
      const name = begun.body?.name;
      assert('market studio: the templates are listed; a project is begun empty under a name of its own, and is sound', templates.length >= 1 && templates.every((t) => t.title && t.about) && begun.status === 200 && /^[a-z0-9-]+$/.test(name) && W.checkProject(begun.body.project).length === 0);
      const csv = ['year;name;lat;lon;sales;kind', '2021;Alpha;-37.810;144.960;100;cafe', '2021;Beta;-37.812;144.962;200;cafe', '2021;Gamma;-37.820;144.970;50;bar', '2022;Alpha;-37.810;144.960;120;cafe', '2022;Beta;-37.812;144.962;210;cafe',
        '2022;Delta;-37.811;144.961;80;bar', '2023;Alpha;-37.810;144.960;130;cafe', '2023;Delta;-37.811;144.961;90;bar', '2023;Epsilon;-37.900;145.200;10;cafe'].join('\n');
      const squares = { type: 'FeatureCollection', features: [['A', 144.955, -37.815, 144.965, -37.805], ['B', 144.966, -37.825, 144.975, -37.815]].map(([id, w, s, e, n]) => ({ type: 'Feature', id, properties: { label: `Square ${id}` },
        geometry: { type: 'Polygon', coordinates: [[[w, s], [e, s], [e, n], [w, n], [w, s]]] } })) };
      const kept = await json(`/api/projects/${name}/files?name=outlets.csv`, { method: 'POST', body: csv });
      const keptAreas = await json(`/api/projects/${name}/files?name=squares.geojson`, { method: 'POST', body: JSON.stringify(squares) });
      const notData = await json(`/api/projects/${name}/files?name=run.exe`, { method: 'POST', body: 'x' });
      assert('market studio: a file of data is kept in the project as it is; what is not a file of data is refused', kept.status === 200 && kept.body.ref === 'project:data/outlets.csv' && keptAreas.status === 200 && notData.status === 400);
      const head = (await send('POST', `/api/projects/${name}/read`, { source: { ref: kept.body.ref, name: 'outlets' } })).body;
      const roles = W.guessRoles(head);
      assert('market studio: the file is read as a table: nine rows, its numbers as numbers, its words as text; its columns\' roles are guessed from their names',
        head.rows === 9 && head.columns.map((c) => `${c.name}:${c.kind}`).join() === 'year:number,name:text,lat:number,lon:number,sales:number,kind:text' && roles.lat === 'lat' && roles.lon === 'lon' && roles.time === 'year' && roles.name === 'name' && roles.amount === 'sales' && roles.category === 'kind');
      const areasHead = (await send('POST', `/api/projects/${name}/read`, { source: { ref: keptAreas.body.ref, name: 'squares' } })).body;
      assert('market studio: areas of GeoJSON are read as rows with a cell each and the table\'s outlines', areasHead.rows === 2 && areasHead.roles.cell === 'cell' && Object.keys(areasHead.places.rings).join() === 'A,B' && W.layerKind(areasHead) === 'areas');

      // The project with its sources, slices and a quantity; its rows within a slice by the server's rule and by the page's.
      const project = begun.body.project;
      project.sources.push({ id: 's1', name: 'outlets', title: 'Outlets', ref: kept.body.ref, roles, where: [] }, { id: 's2', name: 'squares', title: 'Squares', ref: keptAreas.body.ref, roles: { cell: 'cell' }, where: [] },
        { id: 's3', name: 'bars', title: 'Bars', ref: kept.body.ref, roles, where: [{ column: 'kind', op: '=', value: 'bar' }, { column: 'sales', op: '>=', value: '80' }] });
      const region = { west: 144.95, east: 144.98, south: -37.83, north: -37.80 };
      project.slices.push({ id: 'centre', name: 'Centre, 2022–2023', region, time: { from: 2022, until: 2023 }, period: 'year' }, { id: 'years', name: 'All years', region: null, time: { from: 2021, until: 2023 }, period: 'year' },
        { id: 'y2021', name: '2021', region: null, time: { from: 2021, until: 2021 }, period: 'year' });
      project.quantities.push({ name: 'margin', low: 0.1, mid: 0.2, high: 0.3, unit: '', about: 'assumed' });
      const put = await send('PUT', `/api/projects/${name}`, project), notProject = await send('PUT', `/api/projects/${name}`, { schema: 'other/1' }), elsewhere = await send('PUT', `/api/projects/${name}`, project, { Origin: 'http://elsewhere.example' });
      const back = (await json(`/api/projects/${name}`)).body;
      assert('market studio: the project is kept and read back; what is not a project, and a project from another page, are refused', put.status === 200 && back.project.sources.length === 3 && back.project.title === 'A check — 市場' && notProject.status === 400 && elsewhere.status === 403);
      const whole = await tableOf(name, 'outlets'), within = await tableOf(name, 'outlets', 'centre');
      const byPage = W.rowsIn({ ...whole, roles }, project.slices[0]);
      assert('market studio: the rows of a table within a slice are the same by the server\'s rule and by the page\'s: five of the nine', within.rows === 5 && byPage.length === 5 && byPage.map((i) => `${whole.data.year[i]} ${whole.data.name[i]}`).join() === within.data.year.map((y, i) => `${y} ${within.data.name[i]}`).join());
      const bars = await tableOf(name, 'bars');
      assert('market studio: a source\'s filter keeps the rows where every condition holds, as the file is read', bars.rows === 2 && bars.data.name.join() === 'Delta,Delta' && (await tableOf(name, 'nothing')).message.includes('no table'));
      const outside = [await send('POST', `/api/projects/${name}/read`, { source: { ref: 'project:../../serve.py', name: 'x' } }), await send('POST', `/api/projects/${name}/read`, { source: { ref: 'data:../../README.md', name: 'x' } }),
        await send('POST', `/api/projects/${name}/read`, { source: { ref: 'file:///etc/passwd', name: 'x' } }), await json('/api/projects/..%2F..%2Fserve')];
      assert('market studio: a source outside the project and the data library, and a project outside the store, are refused', outside.every((r) => r.status === 400 || r.status === 404), outside.map((r) => r.status).join(' '));

      // The tools, described; a run asked by another page, or not as the page asks, refused.
      const tools = (await json(`/api/tools?project=${name}`)).body.tools;
      assert('market studio: every predefined tool is described with its title, what it answers and its parameters, in its group',
        tools.length === toolFiles.length && tools.every((t) => !t.error && t.title && t.description && Array.isArray(t.params) && t.group_title), tools.filter((t) => t.error).map((t) => `${t.ref}: ${t.error}`).join(' | '));
      assert('market studio: a parameter that names a table, a column of it or a quantity says so, for the form to offer what the project has',
        tools.every((t) => t.params.every((q) => !q.role || ['table', 'column', 'columns', 'quantity', 'file'].includes(q.role))) && tools.some((t) => t.params.some((q) => q.role === 'column' && q.of)) && tools.some((t) => t.params.some((q) => q.role === 'quantity')));
      const gather = { tool: 'tables/gather.py', params: { table: 'outlets', by: 'kind', how: 'sum', of: 'sales', name: 'by_kind' }, slice: 'centre', step: 'g1' };
      const refused = [await run(name, gather, { Origin: 'http://elsewhere.example', 'X-Studio': 'run' }), await run(name, gather, { Origin: at }), await run(name, gather, { 'X-Studio': 'run' })];
      assert('market studio: a tool is run only from this work\'s own page: another page, a request without its mark, and one without an origin are refused', refused.every((r) => r.state === 'refused' && r.status === 403));
      assert('market studio: a tool that is not there, in a project or out of the tools, is not run', (await run(name, { tool: '../serve.py' })).status === 404 && (await run(name, { tool: 'project/none.py' })).status === 404);

      // Tools run on a slice; their answers kept as the project's tables.
      const g = await run(name, gather);
      assert('market studio: rows gathered on a slice are those of the slice: cafés 460 and bars 170 in the centre in 2022–2023, as by hand',
        g.state === 'done' && g.step.id === 'g1' && g.step.slice === 'centre' && g.step.outputs[0].name === 'by_kind' && g.step.outputs[0].show.kind === 'chart', g.message);
      const steps = [g.step];
      const keep = async () => { await send('PUT', `/api/projects/${name}`, { ...project, steps }); };
      await keep();
      const kinds = await tableOf(name, 'by_kind');
      assert('market studio: the sums are those by hand', kinds.rows === 2 && Object.fromEntries(kinds.data.kind.map((k, i) => [k, kinds.data.sum_of_sales[i]])).cafe === 460 && Object.fromEntries(kinds.data.kind.map((k, i) => [k, kinds.data.sum_of_sales[i]])).bar === 170, JSON.stringify(kinds.data));
      const again = await run(name, { ...gather, slice: 'years', step: 'g2' });
      assert('market studio: one name belongs to one table: the same tool run as another step, on another slice, answers a table named after its slice', again.state === 'done' && again.step.outputs[0].name === 'by_kind_all_years', again.step?.outputs?.[0]?.name || again.message);
      const rerun = await run(name, gather);
      assert('market studio: a step run again keeps its name and replaces what it answered', rerun.state === 'done' && rerun.step.id === 'g1' && rerun.step.outputs[0].name === 'by_kind');
      steps.push(again.step);

      const t = await run(name, { tool: 'time/turnover.py', params: { table: 'outlets', identity: 'name' }, slice: 'years', step: 't1' });
      steps.push(t.step); await keep();
      const turnover = t.state === 'done' ? await tableOf(name, 'turnover') : null, lives = t.state === 'done' ? await tableOf(name, 'lives') : null;
      assert('market studio: a record of who was there read as lives: three there in each year; Delta and Epsilon opened, Gamma and Beta closed, as by hand',
        !!turnover && turnover.data.there.join() === '3,3,3' && turnover.data.opened.join() === ',1,1' && turnover.data.closed.join() === ',1,1' && lives.rows === 5 && lives.roles.time === 'from' && lives.roles.until === 'until', t.message || JSON.stringify(turnover?.data));
      const life = lives ? Object.fromEntries(lives.data.name.map((n, i) => [n, [lives.data.from[i], lives.data.until[i]]])) : {};
      assert('market studio: a life ends the period after it is last seen, and has no end while it is still there', String(life.Gamma) === '2021,2022' && String(life.Beta) === '2021,2023' && life.Alpha?.[1] === null && W.rowsAt({ ...lives, roles: lives.roles }, Date.UTC(2022, 6, 1)).length === 3);
      const one = await run(name, { tool: 'time/turnover.py', params: { table: 'outlets', identity: 'name' }, slice: 'y2021', step: 't2' });
      assert('market studio: a tool that cannot answer says why, and the server lives: a record of one period has no openings', one.state === 'failed' && /^a record of one period/.test(one.message), one.message);

      const d = await run(name, { tool: 'tables/density.py', params: { table: 'outlets', side: 500, name: 'squares_of_500' }, slice: 'y2021', step: 'd1' });
      const a = await run(name, { tool: 'tables/areas.py', params: { places: 'outlets', areas: 'squares', of: 'sales', name: 'in_squares' }, slice: 'y2021', step: 'a1' });
      const c = await run(name, { tool: 'time/change.py', params: { table: 'outlets', by: 'kind', of: 'sales' }, slice: 'years', step: 'c1' });
      const q = await run(name, { tool: 'tables/compute.py', params: { table: 'outlets', column: 'profit', formula: 'sales * margin', name: 'with_profit' }, slice: 'y2021', step: 'q1' });
      const de = await run(name, { tool: 'tables/describe.py', params: { table: 'outlets' }, slice: null, step: 'de1' });
      steps.push(d.step, a.step, c.step, q.step, de.step); await keep();
      const density = d.state === 'done' ? await tableOf(name, 'squares_of_500') : null;
      assert('market studio: places gathered into squares are all counted once, and the squares are areas of the map', !!density && density.data.count.reduce((x, y) => x + y, 0) === 3 && W.layerKind(density) === 'areas' && d.step.outputs[0].show.layer === 'areas', d.message);
      const inSquares = a.state === 'done' ? await tableOf(name, 'in_squares') : null;
      assert('market studio: places counted in areas: Alpha and Beta in square A with 300 of sales, Gamma in B with 50, as by hand',
        !!inSquares && inSquares.data.cell.join() === 'A,B' && inSquares.data.count.join() === '2,1' && inSquares.data.sum_of_sales.join() === '300,50' && inSquares.data.hectares.every((h) => h > 50 && h < 150), a.message || JSON.stringify(inSquares?.data));
      const change = c.state === 'done' ? await tableOf(name, 'change') : null, row = change ? Object.fromEntries(change.data.kind.map((k, i) => [k, [change.data.at_2021[i], change.data.at_2023[i], change.data.change[i]]])) : {};
      assert('market studio: change between the first and the last period: cafés from 300 to 140, bars from 50 to 90', String(row.cafe) === '300,140,-160' && String(row.bar) === '50,90,40', c.message || JSON.stringify(change?.data));
      const profit = q.state === 'done' ? await tableOf(name, 'with_profit') : null;
      assert('market studio: a new column from a formula of columns and the project\'s quantities, the table keeping its place', !!profit && profit.data.profit.map((v) => Math.round(v)).join() === '20,40,10' && profit.roles.lat === 'lat' && profit.roles.amount === 'profit', q.message);
      const evil = await run(name, { tool: 'tables/compute.py', params: { table: 'outlets', column: 'x', formula: '__import__("os").system("echo")', name: 'evil' }, slice: null, step: 'q2' });
      const dunder = await run(name, { tool: 'tables/compute.py', params: { table: 'outlets', column: 'x', formula: 'sales.__class__', name: 'evil' }, slice: null, step: 'q3' });
      assert('market studio: a formula that is not arithmetic is refused, not run, and the reason is said in a line, the trace kept beside it',
        evil.state === 'failed' && dunder.state === 'failed' && /^a formula may call/.test(evil.message) && /^a formula is arithmetic/.test(dunder.message) && !/Traceback/.test(evil.message) && /Traceback/.test(evil.detail), `${evil.message} | ${dunder.message}`);
      const described = de.state === 'done' ? await tableOf(name, 'described') : null;
      assert('market studio: a table described: six columns, nine filled values each, five names', !!described && described.rows === 6 && described.data.filled.every((n) => n === 9) && described.data.distinct[described.data.column.indexOf('name')] === 5, de.message);

      // A script of the project's own: kept only from the page, described, run on a slice, its figure read back.
      const script = ['"""A tool of the check\'s own."""', '', 'def create(api):', '    class Mine(api.BoardAnalysis):', '        id = "mine"', '        title = "Mine"', '        description = "The rows it is given."',
        '        params = [api.param("table", "Table", "outlets", kind="text", role="table"), api.param("times", "Times", 2, kind="integer", minimum=1, maximum=9)]', '',
        '        def run(self, board, params, context):', '            t = board.table(params["table"])',
        '            svg = \'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 10 10"><circle cx="5" cy="5" r="4"/></svg>\'',
        '            return [api.table("mine", {"rows": [t.rows * params["times"]], "lat": [t.column(t.roles["lat"])[0]], "lon": [t.column(t.roles["lon"])[0]]}, roles={"lat": "lat", "lon": "lon"}, show={"kind": "layer", "layer": "points"}),',
        '                    api.note(f"{t.rows} rows in {board.view[\'slice\']}, {board.view[\'period\']} by {board.view[\'period\']}; margin {board.quantity(\'margin\').mid}"), api.figure(svg, "A figure")]', '    return Mine()', ''].join('\n');
      const keptScript = await send('PUT', `/api/projects/${name}/scripts/mine.py`, { code: script }, own), foreign = await send('PUT', `/api/projects/${name}/scripts/theirs.py`, { code: script }, { Origin: 'http://elsewhere.example', 'X-Studio': 'run' });
      const badName = await send('PUT', `/api/projects/${name}/scripts/..%2Fmine.py`, { code: script }, own);
      const mineTool = (await json(`/api/tools?project=${name}`)).body.tools.find((x) => x.ref === 'project/mine.py');
      assert('market studio: a script is kept only from this work\'s own page, under a plain name; kept, it is a tool of the project with its parameters', keptScript.status === 200 && foreign.status === 403 && badName.status !== 200 && mineTool?.title === 'Mine' && mineTool.params.length === 2 && (await json(`/api/projects/${name}/scripts/mine.py`)).body.code === script);
      const m = await run(name, { tool: 'project/mine.py', params: { times: 3 }, slice: 'centre', step: 'm1' });
      steps.push(m.step); await keep();
      const figures = m.state === 'done' ? (await json(`/api/projects/${name}/result?step=m1`)).body.figures : [];
      assert('market studio: the script is given its table within the slice, the slice itself and the project\'s quantities, and its parameters as their kinds',
        m.state === 'done' && m.step.outputs.find((o) => o.kind === 'note').text === '5 rows in Centre, 2022–2023, year by year; margin 0.2' && (await tableOf(name, 'mine')).data.rows[0] === 15 && m.step.outputs[0].show.layer === 'points', m.message);
      assert('market studio: a figure a script answers is kept with its step and read back', figures.length === 1 && figures[0].format === 'svg' && figures[0].data.includes('<circle') && figures[0].k === 2 && (await json(`/api/projects/${name}/result?step=none`)).status === 404);
      const final = (await json(`/api/projects/${name}`)).body.project;
      assert('market studio: the project with its steps is sound, its tables those of its sources and of its steps', W.checkProject(final).length === 0 && ['outlets', 'by_kind', 'by_kind_all_years', 'lives', 'turnover', 'in_squares', 'mine'].every((n) => W.tablesOf(final).has(n)), W.checkProject(final).join('; '));

      // A board of the project.
      const board = B.apply(B.emptyBoard({ id: 'b1', title: 'Findings' }), [{ t: 'add', id: 'n1', item: B.newItem('note', { text: 'kept — 市場' }), place: { x: 0, y: 0, w: 200, h: 132 } }]).board;
      const none = await json(`/api/projects/${name}/boards/b1`), stored = await send('PUT', `/api/projects/${name}/boards/b1`, board), notBoard = await send('PUT', `/api/projects/${name}/boards/b1`, { schema: 'something/1' });
      assert('market studio: a board of the project not yet kept is none, and no error; kept, it is read back as it was; what is not a board is refused',
        none.status === 200 && none.body === null && stored.status === 200 && JSON.stringify((await json(`/api/projects/${name}/boards/b1`)).body) === JSON.stringify(board) && notBoard.status === 400);

      // The template's plan on Melbourne's cafés, where the engine is built and the data are in the cache.
      const template = JSON.parse(read('templates/melbourne-cafes.json'));
      const have = template.needs.every((f) => existsSync(join(ROOT, 'data/cache', f)));
      if (!status.engine.current || !have) skip(`market studio: the template's plan on Melbourne's cafés (${!have ? 'its data are not in the cache' : 'the engine is not built from the present sources'}: run the recipe)`);
      else {
        const mel = (await send('POST', '/api/projects', { template: 'melbourne-cafes' })).body, p = mel.project, done2 = [];
        const plan = Object.fromEntries(p.plan.map((e) => [e.id, e]));
        const step = async (id) => { const e = plan[id], r = await run(mel.name, { tool: e.tool, params: e.params, slice: e.slice, step: e.id, title: e.title }); if (r.state === 'done') { done2.push(r.step); await send('PUT', `/api/projects/${mel.name}`, { ...p, steps: done2 }); } return r; };
        const cafes = await tableOf(mel.name, 'cafes', 'sl-city-2024');
        assert('market studio: Melbourne: the source\'s filter and the slice of 2024 keep the 1,677 cafés and restaurants of that census', cafes.rows === 1677 && cafes.roles.time === 'census_year', String(cafes.rows));
        const buyers = await step('p-buyers'), market = await step('p-market-docklands');
        assert('market studio: Melbourne: buyers are made from the census\'s dwellings and offices by the project\'s quantities', buyers.state === 'done' && buyers.step.outputs[0].name === 'buyers' && buyers.step.outputs[0].rows === 1206, buyers.message);
        if (market.state !== 'done') assert('market studio: Melbourne: the market of Docklands is computed', false, market.message);
        else {
          const outlets = await tableOf(mel.name, 'outlets'), unserved = await tableOf(mel.name, 'unserved'), sellers = await tableOf(mel.name, 'sellers');
          const sum = (v) => v.reduce((x, y) => x + (y || 0), 0);
          check('market studio: Melbourne: what the buyers of Docklands bring is what its outlets sell and what buys nothing', (sum(outlets.data.sales) + sum(unserved.data.unserved)) / sum(unserved.data.brings) - 1, 1e-4);
          assert('market studio: Melbourne: the sellers\' sales are the outlets\', and every outlet lies in the slice\'s region', Math.abs(sum(sellers.data.sales) / sum(outlets.data.sales) - 1) < 1e-4 && outlets.rows > 50 && outlets.data.lon.every((x) => x >= 144.932 && x <= 144.9515));
        }
        const turnover = await step('p-turnover'), survival = await step('p-survival'), concentration = await step('p-concentration'), size = await step('p-size'), sites = await step('p-sites');
        const custom = await step('p-custom');
        const cu = custom.state === 'done' ? await tableOf(mel.name, 'custom') : null, rv = cu ? await tableOf(mel.name, 'rivals') : null;
        assert('market studio: Melbourne: the custom of one outlet place by place, each place once, the part of it in the core between none and all; where it would go adds up to no more than all of it',
          !!cu && cu.rows > 0 && new Set(cu.data.cell).size === cu.rows && cu.data.custom.every((v) => v > 0) && cu.data.in_core.every((v) => v >= 0 && v <= 1) && rv.data.part.reduce((x, y) => x + y, 0) <= 100.001, custom.message);
        const tv = turnover.state === 'done' ? await tableOf(mel.name, 'turnover') : null;
        assert('market studio: Melbourne: openings and closings over 23 years; in each year those there are those before, and opened, less closed', !!tv && tv.rows === 23 && tv.data.there.every((n, i) => i === 0 || n === tv.data.there[i - 1] + tv.data.opened[i] - tv.data.closed[i]), turnover.message);
        const sv = survival.state === 'done' ? await tableOf(mel.name, 'survival') : null;
        assert('market studio: Melbourne: the share still there begins at a hundred and never rises, in every group', !!sv && sv.rows > 20 && sv.data.still_there.every((v, i) => (sv.data.after[i] === 0 ? v === 100 : v <= sv.data.still_there[i - 1] + 1e-9)), survival.message);
        const cn = concentration.state === 'done' ? await tableOf(mel.name, 'concentration') : null;
        assert('market studio: Melbourne: concentration year by year: an index between none and one, as many equal sellers as its inverse', !!cn && cn.rows === 23 && cn.data.hhi.every((h, i) => h > 0 && h < 1 && Math.abs(cn.data.equal_sellers[i] - 1 / h) < 0.06 / h), concentration.message);
        const sz = size.state === 'done' ? await tableOf(mel.name, 'size') : null, quantity = Object.fromEntries(p.quantities.map((x) => [x.name, x]));
        const by = (k) => quantity.dwellings[k] * quantity.residents_per_dwelling[k] * quantity.spend_resident[k] * quantity.weeks[k] + quantity.office_100m2[k] * quantity.workers_per_100m2[k] * quantity.spend_worker[k] * quantity.weeks[k];
        const last = sz ? sz.rows - 1 : 0;
        assert('market studio: Melbourne: the size is the sums and products of the project\'s quantities, its bounds those at their ends', !!sz && Math.abs(sz.data.mid[last] / by('mid') - 1) < 1e-12 && Math.abs(sz.data.low[last] / by('low') - 1) < 1e-12 && Math.abs(sz.data.high[last] / by('high') - 1) < 1e-12, size.message);
        const st = sites.state === 'done' ? await tableOf(mel.name, 'sites') : null;
        assert('market studio: Melbourne: three sites chosen in Docklands, each gaining no more than the one before; what each would sell is drawn from rivals and from those who bought nothing',
          !!st && st.rows === 3 && st.data.gain.every((v, i) => v > 0 && (i === 0 || v <= st.data.gain[i - 1] + 1)) && st.data.sells_alone.every((v, i) => Math.abs(v - st.data.from_rivals[i] - st.data.from_those_who_bought_nothing[i]) <= 2), sites.message);
      }
    }
  } finally {
    server.kill();
    await new Promise((r) => setTimeout(r, 200));
    rmSync(store, { recursive: true, force: true });
  }
}

done();
