// Checks of what is the Urban Studio's own: its page (every element the shared studio asks for, the files it loads), its recipe,
// its tools and plug-ins (each declared, its schemes with their sources, its templates sound and every step of their plans a tool
// that is there), and its server on a store of its own, on the API's host for project-based works: the plug-ins listed with what
// they bring, a project begun, a small forest kept and read, and, where the ecology engine is built, every tool of the ecology
// plug-in run on it with its answers held to what can be counted by hand (composition against Santamour's limits, kinds of each
// rank, the standing trees now, a population of one stage, the share of homes below a cover, occupancy recovered from simulated
// visits, the current of a corridor), a placed table written for the Workbench in the line of time's form, a run refused to
// another page. Where the example's products are in the cache, the template's first step is run on Melbourne's 82,064 trees.
// Where the provision engine is built, its tools are run on small tables answered by hand (days in four parts, two circles of three
// people, a sample with a known law of the day, committed minutes against a declared line), and, where its product is in the cache,
// the first steps of Time and care in Europe. Where the materials engine is built, its tools are run likewise (a stock under a fixed
// lifetime, one process's flows reconciled, Heijungs and Suh's life cycle, a two-industry economy's footprint, a building weighed by the
// envelope of RASMI's structures), and, where its product is in the cache, the first steps of its two templates.
// Run from the repository root:  node applications/urban-studio/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';
import { workspace as W } from '../../../visualization/js/index.js';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(name, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/urban-studio');
const read = (path) => readFileSync(join(APP, path), 'utf8');

// ---- the page --------------------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js'), shell = readFileSync(join(ROOT, 'visualization/js/workspace/studio.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('urban studio: the page\'s style sheets and script exist, the board\'s among them', !missing.length && local.includes('/visualization/js/board/board.css'), missing.join(', '));
  const ids = [...new Set([...shell.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => !['progress', 'progress-text'].includes(i));
  assert('urban studio: every element the shared studio asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  assert('urban studio: the page starts the visualization layer\'s studio, names itself and the Workbench, and knows no subject: its plug-ins do',
    /startStudio\(/.test(app) && /name: 'urban-studio'/.test(app) && /workbench/.test(app) && !/forest\.py|allometry|carbon/i.test(app));
  assert('urban studio: serif type, and the server names its port and stands on the API\'s host',
    /Source\+Serif\+4/.test(page) && /port=9090/.test(read('serve.py')) && /studio_host/.test(read('serve.py')));
  assert('urban studio: the shared studio is general: it names no work, and starts from what it is given', !/Market Studio|Urban Studio'|market-studio/.test(shell.replace(/^\/\/.*$/gm, '')) && /export function startStudio\(C\)/.test(shell));
}

// ---- the recipe, the tools, the plug-ins and their templates ----------------------------------------------------------
const groups = readdirSync(join(APP, 'tools'), { withFileTypes: true }).filter((d) => d.isDirectory()).map((d) => d.name);
const toolFiles = groups.flatMap((g) => readdirSync(join(APP, 'tools', g)).filter((f) => f.endsWith('.py') && !f.startsWith('_')).map((f) => `${g}/${f}`));
const plugins = readdirSync(join(APP, 'plugins'), { withFileTypes: true }).filter((d) => d.isDirectory() && existsSync(join(APP, 'plugins', d.name, 'plugin.json'))).map((d) => d.name);
const pluginTools = plugins.flatMap((g) => readdirSync(join(APP, 'plugins', g, 'tools')).filter((f) => f.endsWith('.py') && !f.startsWith('_')).map((f) => `${g}/${f}`));
{
  const recipe = JSON.parse(read('recipe.json'));
  const missing = recipe.steps.filter((s) => { const [script, arg] = s.run; return !existsSync(join(ROOT, script)) || (script.endsWith('fetch.py') && !existsSync(join(ROOT, 'data/registry', `${arg}.json`))); });
  assert('urban studio: the recipe: every step a script and, for fetching, a manifest that exist; the engines first', missing.length === 0 && recipe.steps[0].id === 'engines', missing.map((s) => s.id).join(', '));
  assert('urban studio: the work\'s own tools stand in a group described, each a module that declares itself',
    groups.includes('tables') && toolFiles.every((f) => /def create\(api\)/.test(read(`tools/${f}`))) && JSON.parse(read('tools/tables/group.json')).title);
  assert('urban studio: a plug-in says what it is, the domains it stands on, and brings tools under its own name that collide with no group of the work',
    plugins.includes('ecology') && plugins.every((g) => { const m = JSON.parse(read(`plugins/${g}/plugin.json`)); return m.title && m.about && Array.isArray(m.domains) && !groups.includes(g); })
    && pluginTools.filter((f) => f.startsWith('ecology/')).length >= 10 && pluginTools.every((f) => /def create\(api\)/.test(read(`plugins/${f.replace('/', '/tools/')}`))), `${pluginTools.length} tools`);
  const theory = JSON.parse(read('plugins/ecology/plugin.json')).theory;
  assert('urban studio: the ecology plug-in points to the theory note, brings an engine on the API\'s composition, and its tools name no proposition',
    existsSync(join(ROOT, theory)) && /science\/ecology\.hpp/.test(read('plugins/ecology/engine/main.cpp'))
    && pluginTools.filter((f) => f.startsWith('ecology/')).every((f) => !/Proposition|Theorem \d|Corollary/.test(read(`plugins/${f.replace('/', '/tools/')}`))));
  const schemes = readdirSync(join(APP, 'plugins/ecology/schemes')).filter((f) => f.endsWith('.json')).map((f) => JSON.parse(read(`plugins/ecology/schemes/${f}`)));
  const allo = schemes.find((s) => s.schema === 'noetarium.allometry/1'), lim = schemes.find((s) => s.schema === 'noetarium.limits/1');
  const groupsNamed = new Set((allo?.groups || []).map((g) => g.name));
  assert('urban studio: the declared schemes name their sources; every genus, species and family of the allometry names a group of it, and its fallback too',
    allo && lim && schemes.every((s) => s.source && s.source.length > 40 && s.name) && [allo.genera, allo.species, allo.families].every((m) => Object.values(m).every((g) => groupsNamed.has(g)))
    && groupsNamed.has(allo.fallback) && allo.groups.every((g) => Number.isFinite(g.aboveground.b0) && Number.isFinite(g.aboveground.b1)) && lim.species < lim.genus && lim.genus < lim.family);
  for (const g of plugins)
    for (const file of readdirSync(join(APP, 'plugins', g, 'templates')).filter((f) => f.endsWith('.json'))) {
      const t = JSON.parse(read(`plugins/${g}/templates/${file}`)), p = { ...W.emptyProject(), ...t.project, steps: [] };
      const wrong = W.checkProject(p), plan = t.project.plan || [];
      assert(`urban studio: the template ${g}/${file} is a sound project; every step of its plan is a tool that is there, on a slice that is there; its data are the library's`,
        wrong.length === 0 && !!t.title && !!t.about && plan.length > 0 && plan.every((e) => [...toolFiles, ...pluginTools].includes(e.tool) && e.id && e.title && (e.slice === null || p.slices.some((s) => s.id === e.slice)))
        && p.sources.every((s) => /^data:/.test(s.ref)) && p.views.every((v) => p.sources.some((s) => s.name === v.table)), wrong.join('; '));
    }
}

// ---- the server ---------------------------------------------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
const numpy = python && spawnSync(python, ['-c', 'import numpy'], { encoding: 'utf8' }).status === 0;
if (!python || !numpy) skip('urban studio: the server and its tools (no Python with numpy found to run them)');
else {
  const store = mkdtempSync(join(tmpdir(), 'urban-studio-check-'));
  const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
  const server = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store], { stdio: 'ignore' });
  const at = `http://127.0.0.1:${port}`;
  try {
    let up = false, status = null;
    for (let k = 0; k < 120 && !up; k++) { await new Promise((r) => setTimeout(r, 150)); try { const r = await fetch(`${at}/api/status`); up = r.ok; status = await r.json(); } catch { /* not yet */ } }
    const eco = status?.plugins?.find((p) => p.name === 'ecology');
    assert('urban studio: the server answers on the API\'s host, names its work, and lists its plug-ins with what each brings',
      up && status.work?.name === 'urban-studio' && typeof status.engine?.current === 'boolean' && eco && eco.tools >= 10 && eco.engine === true && eco.schemes.length >= 2 && eco.domains.includes('ecology'));
    if (up) {
      const json = async (path, options) => { const r = await fetch(`${at}${path}`, options); return { status: r.status, body: await r.json().catch(() => undefined) }; };
      const send = (method, path, body, headers = {}) => json(path, { method, headers: { 'Content-Type': 'application/json', ...headers }, body: typeof body === 'string' ? body : JSON.stringify(body) });
      const own = { Origin: at, 'X-Studio': 'run' };
      const run = async (project, body, headers = own) => {
        const started = await send('POST', `/api/projects/${project}/run`, body, headers);
        if (started.status !== 200) return { state: 'refused', status: started.status, message: started.body?.message };
        for (let k = 0; k < 4000; k++) {
          await new Promise((r) => setTimeout(r, 120));
          const j = (await json(`/api/job/${started.body.job}`)).body;
          if (j.state === 'running') continue;
          // What a step answered is the project's once the step is kept in it, as the page keeps it.
          if (j.state === 'done' && kept[project]) { kept[project].steps.push(j.step); await send('PUT', `/api/projects/${project}`, kept[project]); }
          return j;
        }
        return { state: 'late' };
      };
      const kept = {};
      const tableOf = async (project, name) => (await json(`/api/projects/${project}/table?name=${name}`)).body;
      const tools = (await json('/api/tools')).body.tools;
      assert('urban studio: every tool is described with its parameters, none failing to load', tools.length >= 14 && tools.every((t) => !t.error && t.title && Array.isArray(t.params)), tools.filter((t) => t.error).map((t) => `${t.ref}: ${t.error}`).join(' | '));
      const begun = await send('POST', '/api/projects', { title: "A check's forest" });
      const name = begun.body?.name;
      assert('urban studio: a project is begun under a name of its own (an apostrophe joins its word), and is sound', begun.status === 200 && name === 'a-checks-forest' && W.checkProject(begun.body.project).length === 0, name);

      // A small forest: twelve trees in two areas. Species: A three, B three, C two, D two (genus G1 holds A and B), and two unnamed.
      const rows = [['A', 'G1', 'F1', 30, 'north'], ['A', 'G1', 'F1', 40, 'north'], ['A', 'G1', 'F1', 12, 'south'], ['B', 'G1', 'F1', 22, 'north'], ['B', 'G1', 'F1', 50, 'south'],
        ['B', 'G1', 'F1', 35, 'south'], ['C', 'G2', 'F1', 18, 'north'], ['C', 'G2', 'F1', 27, 'south'], ['D', 'G3', 'F2', 60, 'north'], ['D', 'G3', 'F2', 900, 'south'], ['', 'G3', 'F2', '', 'north'], ['', 'G2', 'F1', 15, 'south']];
      const csv = ['id,lat,lon,species,genus,family,dbh_cm,area,life_lower,life_upper', ...rows.map((r, i) => [i + 1, -37.81 + i * 0.001, 144.96 + (i % 4) * 0.001, `Sp ${r[0]}`.replace(/^Sp $/, ''), r[1], r[2], r[3], r[4], 10, 20].join(','))].join('\n');
      const keptFile = await json(`/api/projects/${name}/files?name=trees.csv`, { method: 'POST', body: csv });
      const project = begun.body.project;
      kept[name] = project;
      project.sources.push({ id: 's1', name: 'trees', title: 'Trees', ref: keptFile.body.ref, roles: { lat: 'lat', lon: 'lon', name: 'species' }, where: [] });
      await send('PUT', `/api/projects/${name}`, project);
      const trees = await tableOf(name, 'trees');
      assert('urban studio: the forest is kept and read as a table of twelve trees', keptFile.status === 200 && trees?.rows === 12);
      const engine = status.engine?.engines?.ecology?.built;
      if (!engine) skip('urban studio: the ecology plug-in\'s tools (its engine is not built: run the recipe)');
      else {
        const base = { table: 'trees', species: 'species', genus: 'genus', family: 'family', dbh: 'dbh_cm', area: 'area', largest: 250 };
        const f = await run(name, { tool: 'ecology/forest.py', params: { ...base, age: '', limits: 'limits-santamour-1990.json', allometry: 'allometry-jenkins-2003.json', beyond: 'extrapolate' } });
        const comp = f.state === 'done' ? await tableOf(name, 'composition') : null;
        const rowOf = (rank, n) => comp ? comp.data.trees[comp.data.rank.findIndex((r, i) => r === rank && comp.data.name[i] === n)] : null;
        const note = f.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        assert('urban studio: the forest tool counts species, genera and families by hand (A 3, G1 6, F1 9) and finds every rank above Santamour\'s limits',
          f.state === 'done' && rowOf('species', 'Sp A') === 3 && rowOf('genus', 'G1') === 6 && rowOf('family', 'F1') === 9 && /Species: fails/.test(note) && /Genus: fails/.test(note) && /Family: fails/.test(note), f.message || note.slice(0, 300));
        assert('urban studio: a diameter beyond the plausible is set aside and said to be, and the trees without a diameter hold no estimate',
          /1 diameters above 250 cm were taken for errors/.test(note) && /2 trees have no diameter/.test(note), note.slice(-400));
        const d = await run(name, { tool: 'ecology/diversity.py', params: { table: 'trees', species: 'species', genus: 'genus', family: 'family', area: 'area', rank: 'genus', least: 5 } });
        const dv = d.state === 'done' ? await tableOf(name, 'diversity_by_area') : null;
        const kinds = dv ? Object.fromEntries(dv.data.area.map((a, i) => [a, dv.data.kinds[i]])) : {};
        assert('urban studio: the diversity tool counts the kinds of genus in each area by hand (north 3, south 3)', d.state === 'done' && kinds.north === 3 && kinds.south === 3, d.message);
        const fu = await run(name, { tool: 'ecology/future.py', params: { ...base, age: '', life_lower: 'life_lower', life_upper: 'life_upper', allometry: 'allometry-jenkins-2003.json', beyond: 'extrapolate', years: 25, hazard: 0.03, planting: 0, mix: '', planted_dbh: 5, planted_hazard: 0.04, increment: 0.5 } });
        const fut = fu.state === 'done' ? await tableOf(name, 'future') : null;
        assert('urban studio: the future tool: ten trees with a plausible diameter stand now, half of them at fifteen years (lives uniform in [10, 20]), none at twenty-five',
          fut && fut.data.standing[0] === 10 && Math.abs(fut.data.standing[15] - 5) < 1e-9 && fut.data.standing[25] === 0, fu.message);
        const po = await send('PUT', `/api/projects/${name}/scripts/none.py`, { code: '' }, own);   // a script kept by the page's own request
        await json(`/api/projects/${name}/files?name=stages.csv`, { method: 'POST', body: 'stage,survival,growth,now\nall,0.6,0,100' });
        project.sources.push({ id: 's2', name: 'one', title: 'One stage', ref: 'project:data/stages.csv', roles: {}, where: [] });
        await send('PUT', `/api/projects/${name}`, project);
        const pp = await run(name, { tool: 'ecology/population.py', params: { table: 'one', stage: 'stage', survival: 'survival', growth: 'growth', fecundity: '', now: 'now', input: 20, years: 200 } });
        const pa = pp.state === 'done' ? await tableOf(name, 'population_analysis') : null;
        assert('urban studio: a population of one stage kept at 0.6 a year with 20 arriving holds 20/(1 − 0.6) = 50, a member living 2.5 years',
          po.status === 200 && pa && Math.abs(pa.data.sustained[0] - 50) < 0.1 && Math.abs(pa.data.years_alive[0] - 2.5) < 1e-9, pp.message);
        await json(`/api/projects/${name}/files?name=areas.csv`, { method: 'POST', body: 'area,cover,homes\na,0.05,100\nb,0.25,300\nc,0.5,600' });
        project.sources.push({ id: 's3', name: 'areas', title: 'Areas', ref: 'project:data/areas.csv', roles: {}, where: [] });
        await send('PUT', `/api/projects/${name}`, project);
        const cn = await run(name, { tool: 'ecology/canopy.py', params: { table: 'areas', cover: 'cover', weight: 'homes', name: 'area', thresholds: '0.1, 0.3' } });
        const ce = cn.state === 'done' ? await tableOf(name, 'canopy_exposure') : null;
        assert('urban studio: the canopy tool: of a thousand homes, a tenth under 10% and four tenths under 30%', ce && Math.abs(ce.data.share_of_homes_below[0] - 0.1) < 1e-12 && Math.abs(ce.data.share_of_homes_below[1] - 0.4) < 1e-12, cn.message);
        // Occupancy: 150 sites, 4 visits, ψ = 0.6, p = 0.4, drawn by a fixed generator.
        let seed = 7;
        const u = () => { seed = (seed * 1103515245 + 12345) % 2147483648; return seed / 2147483648; };
        const visits = ['site,v1,v2,v3,v4'];
        for (let s = 0; s < 150; s++) { const z = u() < 0.6; visits.push([s + 1, ...[0, 1, 2, 3].map(() => (z && u() < 0.4 ? 1 : 0))].join(',')); }
        await json(`/api/projects/${name}/files?name=visits.csv`, { method: 'POST', body: visits.join('\n') });
        project.sources.push({ id: 's4', name: 'visits', title: 'Visits', ref: 'project:data/visits.csv', roles: {}, where: [] });
        await send('PUT', `/api/projects/${name}`, project);
        const oc = await run(name, { tool: 'ecology/occupancy.py', params: { table: 'visits', visits: 'v1, v2, v3, v4', covariates: '', site: 'site', lat: '', lon: '' } });
        const om = oc.state === 'done' ? await tableOf(name, 'occupancy_model') : null;
        const logistic = (x) => 1 / (1 + Math.exp(-x));
        assert('urban studio: occupancy from simulated visits recovers ψ = 0.6 and p = 0.4 within their errors',
          om && Math.abs(logistic(om.data.logit_coefficient[0]) - 0.6) < 0.12 && Math.abs(logistic(om.data.logit_coefficient[1]) - 0.4) < 0.08, oc.message || JSON.stringify(om?.data));
        // Connectivity on a small grid: two dense patches joined by a strip of canopy across open ground.
        const cells = ['lat,lon,cover'];
        for (let j = 0; j < 12; j++) for (let i = 0; i < 20; i++) {
          const patch = (i < 4 || i > 15) && j > 3 && j < 9, strip = j === 6;
          cells.push([(-37.8 + j * 0.000226).toFixed(6), (144.9 + i * 0.000283).toFixed(6), patch ? 0.9 : strip ? 0.5 : 0.05].join(','));
        }
        await json(`/api/projects/${name}/files?name=canopy.csv`, { method: 'POST', body: cells.join('\n') });
        project.sources.push({ id: 's5', name: 'canopy', title: 'Canopy', ref: 'project:data/canopy.csv', roles: { lat: 'lat', lon: 'lon' }, where: [] });
        await send('PUT', `/api/projects/${name}`, project);
        const co = await run(name, { tool: 'ecology/connectivity.py', params: { table: 'canopy', lat: 'lat', lon: 'lon', cover: 'cover', patch_cover: 0.6, patches: 2, r_canopy: 1, r_open: 40, median: 200 } });
        const cur = co.state === 'done' ? await tableOf(name, 'connectivity_current') : null;
        const onStrip = cur ? cur.data.current.filter((_, k) => Math.abs(cur.data.lat[k] - (-37.8 + 6 * 0.000226)) < 1e-5).length : 0;
        assert('urban studio: connectivity finds the two patches and carries their current along the strip between them', co.state === 'done' && onStrip >= 10, co.message || `${onStrip} cells`);
        const ex = await send('POST', `/api/projects/${name}/export`, { table: 'connectivity_current' });
        const record = ex.status === 200 ? await (await fetch(`${at}${ex.body.url}`)).json() : null;
        if (ex.status === 200) rmSync(join(ROOT, 'data/cache/urban-studio/exports', ex.body.file), { force: true });   // the check leaves no record behind
        assert('urban studio: a placed table is written for the Workbench as events of the line of time, one place a row',
          record?.kind === 'events' && record.lat.length === cur?.rows && record.time.length === record.lat.length && Array.isArray(record.values.current), ex.body?.message);
        const refused = await run(name, { tool: 'ecology/forest.py', params: base }, { Origin: 'http://elsewhere.example', 'X-Studio': 'run' });
        assert('urban studio: a run asked by another page is refused', refused.state === 'refused' && refused.status === 403);
        // Melbourne, where the product is in the cache: the template's first step.
        if (!existsSync(join(ROOT, 'data/cache/products/urban-forest/melbourne-trees.csv'))) skip('urban studio: Melbourne\'s forest (its product is not in the cache: run the recipe)');
        else {
          const m = await send('POST', '/api/projects', { template: 'ecology.melbourne-urban-forest' });
          kept[m.body.name] = m.body.project;
          const step = m.body.project.plan.find((e) => e.id === 'p-forest');
          const r = await run(m.body.name, { tool: step.tool, params: step.params, slice: step.slice, step: step.id });
          const text = r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
          assert('urban studio: Melbourne\'s template: 82,064 trees, and Eucalyptus camaldulensis above a tenth of them', r.state === 'done' && /82,064 trees/.test(text) && /Eucalyptus camaldulensis at 10\.\d%/.test(text), r.message || text.slice(0, 200));
        }
      }
      // Time, care and provision: small tables answered by hand (days in four parts, two circles of three people, a sample with a known law,
      // a few people's committed minutes), and the template's first steps on Eurostat's data where its product is in the cache.
      const prov = status?.plugins?.find((p) => p.name === 'provision');
      assert('urban studio: the plug-in of time, care and provision brings at least eight tools, an engine and its template, on sociology and social provision',
        prov && prov.tools >= 8 && prov.engine === true && prov.templates >= 1 && prov.domains.includes('sociology') && prov.domains.includes('social-provision'), JSON.stringify(prov));
      if (!status.engine?.engines?.provision?.built) skip('urban studio: the provision plug-in\'s tools (its engine is not built: run the recipe)');
      else {
        const pb = await send('POST', '/api/projects', { title: 'Days and ties' });
        const pn = pb.body.name, pj = pb.body.project;
        kept[pn] = pj;
        const add = async (file, lines, source) => {
          const f = await json(`/api/projects/${pn}/files?name=${file}`, { method: 'POST', body: lines.join('\n') });
          pj.sources.push({ id: `s-${source}`, name: source, title: source, ref: f.body.ref, roles: {}, where: [] });
          await send('PUT', `/api/projects/${pn}`, pj);
        };
        const clr = (x) => { const l = x.map(Math.log), m = l.reduce((a, b) => a + b, 0) / l.length; return l.map((v) => v - m); };
        const day = { North: { F: [540, 200, 260, 440], M: [530, 300, 150, 460] }, South: { F: [560, 150, 300, 430], M: [550, 320, 120, 450] },
          East: { F: [600, 180, 280, 380], M: [590, 260, 170, 420] } };
        await add('days.csv', ['name,wave,sex,age,sleep,paid,unpaid,free', ...Object.entries(day).flatMap(([p, d]) => ['F', 'M'].map((x) => [p, 2010, x, 'TOTAL', ...d[x]].join(',')))], 'days');
        const td = await run(pn, { tool: 'provision/the_day.py', params: { table: 'days', parts: 'sleep, paid, unpaid, free', place: 'name', sex: 'sex', wave_column: 'wave', wave: '2010',
          age_column: 'age', age: 'TOTAL', unpaid: 'unpaid', paid: 'paid' } });
        const bs = td.state === 'done' ? await tableOf(pn, 'between_sexes') : null, dd = td.state === 'done' ? await tableOf(pn, 'the_day') : null;
        const zf = clr(day.North.F), zm = clr(day.North.M), gap = zf.map((v, k) => v - zm[k]);
        const north = bs ? bs.data.place.indexOf('North') : -1, nf = dd ? dd.data.day.indexOf('North, women') : -1;
        assert('urban studio: the day: women\'s and men\'s days in the North lie at Aitchison\'s distance by hand, apart most in the part of the largest log-ratio',
          north >= 0 && Math.abs(bs.data.distance[north] - Math.hypot(...gap)) < 1e-4 && bs.data.most_apart[north] === ['sleep', 'paid', 'unpaid', 'free'][gap.reduce((b, v, k) => (Math.abs(v) > Math.abs(gap[b]) ? k : b), 0)],
          td.message || JSON.stringify(bs?.data));
        assert('urban studio: the day: the balance of unpaid against paid work, √(1/2) ln(260/200), for the North\'s women',
          nf >= 0 && Math.abs(dd.data.unpaid_against_paid[nf] - Math.SQRT1_2 * Math.log(260 / 200)) < 1e-4, JSON.stringify(dd?.data?.unpaid_against_paid));
        const uw = await run(pn, { tool: 'provision/unpaid_work.py', params: { table: 'days', place: 'name', care: 'unpaid', paid: 'paid', wave: '2010', age: 'TOTAL', wage: 10, currency: 'EUR' } });
        const ut = uw.state === 'done' ? await tableOf(pn, 'unpaid_work') : null;
        const ui = ut ? ut.data.place.indexOf('North') : -1;
        assert('urban studio: unpaid work: the North\'s women carry 260/(260 + 150) of it, worth 260 × 365/60 × 10 a year',
          ui >= 0 && Math.abs(ut.data.women_share[ui] - 260 / 410) < 1e-4 && Math.abs(ut.data.value_woman_a_year[ui] - 260 * 365 / 6) < 1, uw.message);
        await add('ties.csv', ['from,to', 'a,b', 'b,c', 'c,a', 'c,d', 'd,e', 'e,f', 'f,d'], 'ties');
        await add('people.csv', ['id,group', 'a,X', 'b,X', 'c,X', 'd,Y', 'e,Y', 'f,Y'], 'people');
        const rl = await run(pn, { tool: 'provision/relations.py', params: { table: 'ties', source: 'from', target: 'to', directed: 'no', people: 'people', person: 'id', group: 'group', permutations: 199, draws: 100 } });
        const be = rl.state === 'done' ? await tableOf(pn, 'belonging') : null, pt = rl.state === 'done' ? await tableOf(pn, 'people_ties') : null;
        const rtext = rl.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        assert('urban studio: ties: each circle keeps six of its seven ends of ties within, and c, the bridge, reaches all five others within two ties',
          be && Math.abs(be.data.ties_within[0] - 6 / 7) < 1e-12 && pt && pt.data.reach_in_two[pt.data.person.indexOf('c')] === 5 && /E–I index is -0\.71/.test(rtext), rl.message || rtext.slice(0, 300));
        // A sample whose outcome is a known law of the day, y = 3 + b·clr(x) with Σ b = 0: the substitution table recovers it.
        let sd = 11;
        const ur = () => { sd = (sd * 1103515245 + 12345) % 2147483648; return sd / 2147483648; };
        const b = [0.8, -0.5, 0.2, -0.5], sample = ['sleep,paid,unpaid,free,wellbeing'], rowsOf = [];
        for (let i = 0; i < 60; i++) {
          const x = [60 + 540 * ur(), 60 + 540 * ur(), 60 + 540 * ur(), 60 + 540 * ur()];
          rowsOf.push(x);
          sample.push([...x.map((v) => v.toFixed(6)), (3 + clr(x).reduce((a, v, k) => a + b[k] * v, 0)).toFixed(12)].join(','));
        }
        await add('sample.csv', sample, 'sample');
        const ra = await run(pn, { tool: 'provision/reallocation.py', params: { table: 'sample', parts: 'sleep, paid, unpaid, free', outcome: 'wellbeing', covariates: '', minutes: 30, resolution: 10 } });
        const rt = ra.state === 'done' ? await tableOf(pn, 'reallocations') : null;
        const g = [0, 1, 2, 3].map((k) => Math.exp(rowsOf.reduce((a, x) => a + Math.log(x[k] / x.reduce((p, q) => p + q, 0)), 0) / rowsOf.length));
        const x0 = g.map((v) => (1440 * v) / g.reduce((p, q) => p + q, 0)), x1 = x0.map((v, k) => v + (k === 0 ? 30 : k === 1 ? -30 : 0));
        const truth = clr(x1).reduce((a, v, k) => a + b[k] * (v - clr(x0)[k]), 0);
        const k01 = rt ? rt.data.to.findIndex((t, i) => t === 'sleep' && rt.data.from[i] === 'paid') : -1;
        assert('urban studio: time moved: thirty minutes to sleep from paid work change the outcome as the known law says', k01 >= 0 && Math.abs(rt.data.change[k01] - truth) < 1e-6,
          ra.message || `${rt?.data?.change?.[k01]} against ${truth}`);
        await add('committed.csv', ['committed', 300, 400, 500, 600, 900, 1000], 'committed');
        const tp = await run(pn, { tool: 'provision/time_poverty.py', params: { table: 'committed', committed: 'committed', weight: '', group: '', multiple: 1.5, line: 700 } });
        const tt = tp.state === 'done' ? await tableOf(pn, 'time_poverty') : null;
        assert('urban studio: time poverty against a declared line of 700 minutes: two of six, 250 minutes above it on average', tt && Math.abs(tt.data.rate[0] - 1 / 3) < 1e-4 && Math.abs(tt.data.gap_minutes[0] - 250) < 0.05, tp.message);
        if (!existsSync(join(ROOT, 'data/cache/products/time-and-care/days.csv'))) skip('urban studio: Time and care in Europe (its product is not in the cache: run the recipe)');
        else {
          const e = await send('POST', '/api/projects', { template: 'provision.time-and-care-in-europe' });
          kept[e.body.name] = e.body.project;
          const said = async (id) => {
            const step = e.body.project.plan.find((x) => x.id === id);
            const r = await run(e.body.name, { tool: step.tool, params: step.params, slice: step.slice, step: step.id });
            return { r, text: r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '' };
          };
          const a = await said('p-day');
          assert('urban studio: Time and care in Europe: eighteen places\' days, women\'s and men\'s apart most by household and family care',
            a.r.state === 'done' && /Across 18 places/.test(a.text) && /separates them most is household and family care/.test(a.text), a.r.message || a.text.slice(0, 300));
          const h = await said('p-healthy');
          assert('urban studio: Time and care in Europe: Sullivan\'s method holds Eurostat\'s healthy life years at 65 to a median of a tenth of a year',
            h.r.state === 'done' && /median of 0\.(0\d|1[0-5]) years/.test(h.text), h.r.message || h.text.slice(0, 300));
          // Groups of places (the EU, the euro area) are never added to their members, and a prevalence in percent is read as such: the members'
          // need comes within a tenth of the EU's own (a unit guessed value by value, or an aggregate added in, would put it far above).
          const c = await said('p-care');
          const m = /from ([\d,]+) in 2024/.exec(c.text), eu = /EU27_2020 goes from ([\d,]+)/.exec(c.text);
          const num = (x) => Number(x.replace(/,/g, ''));
          assert('urban studio: Time and care in Europe: the countries\' need for care comes within a tenth of the EU27\'s own, groups never added to their members',
            c.r.state === 'done' && m && eu && Math.abs(num(m[1]) / num(eu[1]) - 1) < 0.1, c.r.message || c.text.slice(0, 400));
        }
      }

      // Materials and flows: a stock under a fixed lifetime, one process's flows reconciled by hand, Heijungs and Suh's life cycle, a
      // two-industry economy's footprint by hand, a building weighed by the envelope of RASMI's structures; and, where the product is in
      // the cache, the templates' first steps.
      const mat = status?.plugins?.find((p) => p.name === 'materials');
      assert('urban studio: the plug-in of materials and flows brings at least six tools, an engine and two templates, on industrial ecology',
        mat && mat.tools >= 6 && mat.engine === true && mat.templates >= 2 && mat.domains.includes('industrial-ecology'), JSON.stringify(mat));
      if (!status.engine?.engines?.materials?.built) skip('urban studio: the materials plug-in\'s tools (its engine is not built: run the recipe)');
      else {
        const mb = await send('POST', '/api/projects', { title: 'Stocks and flows' });
        const mn = mb.body.name, mj = mb.body.project;
        kept[mn] = mj;
        const add = async (file, lines, source) => {
          const f = await json(`/api/projects/${mn}/files?name=${file}`, { method: 'POST', body: lines.join('\n') });
          mj.sources.push({ id: `s-${source}`, name: source, title: source, ref: f.body.ref, roles: {}, where: [] });
          await send('PUT', `/api/projects/${mn}`, mj);
        };
        const notes = (r) => r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        await add('series.csv', ['year,inflow', ...Array.from({ length: 30 }, (_, k) => `${2000 + k},5`)], 'series');
        const so = await run(mn, { tool: 'materials/stock_over_time.py', params: { table: 'series', year: 'year', values: 'inflow', mode: 'inflow', law: 'fixed', mean: 10, spread: 1 } });
        const st = so.state === 'done' ? await tableOf(mn, 'stock_model') : null;
        assert('urban studio: a stock over time: five a year under a fixed lifetime of ten holds 50, and five leave a year from the eleventh',
          st && Math.abs(st.data.stock[29] - 50) < 1e-9 && Math.abs(st.data.outflow[9]) < 1e-9 && Math.abs(st.data.outflow[10] - 5) < 1e-9, so.message);
        await add('flows.csv', ['flow,from,to,value,sd', 'ore,,mill,100,5', 'metal,mill,,60,3', 'slag,mill,,35,3'], 'flows');
        const bf = await run(mn, { tool: 'materials/balanced_flows.py', params: { table: 'flows', name: 'flow', from_: 'from', to: 'to', value: 'value', sd: 'sd' } });
        const rc = bf.state === 'done' ? await tableOf(mn, 'reconciled') : null;
        assert('urban studio: flows that balance: one process\'s three measurements reconciled by hand (ore 100 − 25·5/43), χ² = 25/43',
          rc && Math.abs(rc.data.reconciled[0] - (100 - 125 / 43)) < 1e-9 && /χ² = 0\.58 on 1 degrees/.test(notes(bf)), bf.message || notes(bf));
        await add('exchanges.csv', ['process,flow,kind,amount', 'power plant,electricity (kWh),product,10', 'power plant,fuel (L),product,-2', 'power plant,CO2 (kg),elementary,1',
          'refinery,fuel (L),product,100', 'refinery,electricity (kWh),product,-50', 'refinery,CO2 (kg),elementary,10'], 'exchanges');
        await add('factors.csv', ['category,flow,factor,unit', 'climate change,CO2 (kg),1,kg CO2-eq'], 'factors');
        const lc = await run(mn, { tool: 'materials/life_cycle.py', params: { table: 'exchanges', process: 'process', flow: 'flow', kind: 'kind', amount: 'amount', factors: 'factors',
          demand: 'electricity (kWh)=1000', gsd: 1 } });
        const im = lc.state === 'done' ? await tableOf(mn, 'impacts') : null;
        assert('urban studio: a life cycle: Heijungs and Suh\'s 1000 kWh emit 1200/9 kg of CO₂', im && Math.abs(im.data.impact[0] - 1200 / 9) < 1e-9, lc.message);
        await add('io.csv', ['industry,label,energy,goods,households,exports', 'energy,Energy,10,40,30,20', 'goods,Goods,20,30,100,50'], 'io');
        await add('air.csv', ['industry,label,CO2', 'energy,Energy,300', 'goods,Goods,100', 'households,Households,25'], 'air');
        const fp = await run(mn, { tool: 'materials/footprint.py', params: { table: 'io', industry: 'industry', final: 'households, exports', pressures: 'air', pressure: 'CO2',
          direct: 'households', unit: 'EUR' } });
        const fu = fp.state === 'done' ? await tableOf(mn, 'footprint_by_use') : null;
        const M = [2.65 / 0.725, 1.05 / 0.725];
        assert('urban studio: what final demand causes: by hand, households 30 M₁ + 100 M₂ + 25 of their own, exports 20 M₁ + 50 M₂, together all 425',
          fu && Math.abs(fu.data.caused[0] - (30 * M[0] + 100 * M[1] + 25)) < 1e-9 && Math.abs(fu.data.caused[1] - (20 * M[0] + 50 * M[1])) < 1e-9, fp.message);
        const schemeFile = join(ROOT, 'data/cache/products/materials-and-flows/rasmi-scheme.json');
        if (!existsSync(schemeFile)) skip('urban studio: buildings weighed by RASMI (its product is not in the cache: run the recipe)');
        else {
          const sch = JSON.parse(readFileSync(schemeFile, 'utf8'));
          const rm = sch.rows.filter((r) => r.region === 'OECD_AUNZ' && r.use === 'RM');
          const concrete = 1000 * rm.reduce((a, r) => a + r.central[0], 0) / rm.length, lowest = 1000 * Math.min(...rm.map((r) => r.lower[0]));
          await add('buildings.csv', ['id,block,function,structure,floor_area_m2,year,latitude,longitude', 'a,1,RM,,1000,1990,-37.81,144.96', 'b,1,RM,C,500,2000,-37.811,144.961',
            'c,2,XX,,300,2010,-37.812,144.962'], 'buildings');
          const bs = await run(mn, { tool: 'materials/building_stock.py', params: { table: 'buildings', floor_area: 'floor_area_m2', function: 'function', structure: 'structure', group: 'block',
            lat: 'latitude', lon: 'longitude', intensities: 'RASMI 20230905 (5th, 50th and 95th percentiles)', region: 'OECD_AUNZ' } });
          const bm = bs.state === 'done' ? await tableOf(mn, 'building_materials') : null;
          assert('urban studio: a building whose structure is unknown takes the envelope of RASMI\'s four: its concrete the mean of their medians, its lower bound the least',
            bm && Math.abs(bm.data.concrete_t[0] - concrete / 1000) < 0.06 && bm.data.total_lower_t[0] <= bm.data.total_t[0] && /1 building of .* that no row of the scheme matches is counted and left out/.test(notes(bs)),
            bs.message || `${bm?.data?.concrete_t?.[0]} against ${concrete / 1000} (lowest ${lowest / 1000}) ${notes(bs).slice(0, 300)}`);
          if (!existsSync(join(ROOT, 'data/cache/products/materials-and-flows/melbourne-buildings.csv'))) skip('urban studio: the templates of materials and flows (their products are not in the cache)');
          else {
            const e = await send('POST', '/api/projects', { template: 'materials.material-stock-of-melbourne' });
            kept[e.body.name] = e.body.project;
            const step = e.body.project.plan.find((x) => x.id === 'p-stock');
            const r = await run(e.body.name, { tool: step.tool, params: step.params, slice: step.slice, step: step.id });
            assert('urban studio: the material stock of Melbourne\'s centre: well over a thousand buildings weighed, with bounds, block by block',
              r.state === 'done' && /^([\d,]+) buildings of/.test(notes(r)) && Number(/^([\d,]+) buildings/.exec(notes(r))[1].replace(/,/g, '')) > 1000, r.message || notes(r).slice(0, 300));
            const u = await send('POST', '/api/projects', { template: 'materials.what-the-eu-emits' });
            kept[u.body.name] = u.body.project;
            const g = u.body.project.plan.find((x) => x.id === 'p-ghg');
            const gr = await run(u.body.name, { tool: g.tool, params: g.params, slice: g.slice, step: g.id });
            const air = readFileSync(join(ROOT, 'data/cache/products/materials-and-flows/eu27-air-2022.csv'), 'utf8').trim().split('\n').slice(1);
            const ghg = air.reduce((a, line) => a + Number(line.split(',').slice(-6)[0] || 0), 0);
            const bu = gr.state === 'done' ? await tableOf(u.body.name, 'footprint_by_use') : null;
            // An industry with no output (extraterritorial organisations) is left out with what it records, and the note says how much.
            const left = Number((/their recorded pressure, ([-\d.e+]+), is left out/.exec(notes(gr)) || [0, 0])[1]);
            assert('urban studio: what the EU emits: final demand causes, in one economy, exactly what its industries and households emit (Σ by use + what is left out = Σ of the accounts)',
              bu && Math.abs((bu.data.caused.reduce((a, b) => a + b, 0) + left) / ghg - 1) < 1e-9, gr.message || `${bu?.data?.caused?.reduce((a, b) => a + b, 0)} + ${left} against ${ghg}`);
          }
        }
      }

      // A water network: EPANET's Net1 from the data library, its nine junctions all served, and a pipe that cannot be lost.
      if (status.engine?.engines?.materials?.built && existsSync(join(ROOT, 'data/cache/epanet-examples/all/Net1.inp'))) {
        const wb = await send('POST', '/api/projects', { title: 'Water' });
        kept[wb.body.name] = wb.body.project;
        const wi = await run(wb.body.name, { tool: 'materials/water_network.py', params: { network: 'data:epanet-examples/all/Net1.inp', analysis: 'an instant', hour: 0,
          pressure_driven: 'demand-driven', required: 20, minimum: 0 } });
        const wn = wi.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        assert('urban studio: a water network: Net1\'s nine junctions draw all they ask, at pressures EPANET finds (77.9 to 89.7 m)',
          wi.state === 'done' && /9 junctions draw ([\d.,]+) of the \1 L\/s/.test(wn) && /pressures from 77\.9 to 89\.7 m/.test(wn), wi.message || wn.slice(0, 300));
      } else skip('urban studio: a water network (the materials engine, or EPANET\'s examples, are not here: run the recipe)');

      // Appraisal and futures: alternatives screened and ranked by hand, a stream's present value by hand, a panel's rounds, a field
      // counted by hand, and two coders' agreement against statsmodels.
      const apr = status?.plugins?.find((p) => p.name === 'appraisal');
      assert('urban studio: the plug-in of appraisal and futures brings four tools and an engine, on nomos', apr && apr.tools >= 4 && apr.engine === true && apr.domains.includes('nomos'), JSON.stringify(apr));
      if (!status.engine?.engines?.appraisal?.built) skip('urban studio: the appraisal plug-in\'s tools (its engine is not built: run the recipe)');
      else {
        const ab = await send('POST', '/api/projects', { title: 'Choices' });
        const an = ab.body.name, aj = ab.body.project;
        kept[an] = aj;
        const add = async (file, lines, source) => {
          const f = await json(`/api/projects/${an}/files?name=${file}`, { method: 'POST', body: lines.join('\n') });
          aj.sources.push({ id: `s-${source}`, name: source, title: source, ref: f.body.ref, roles: {}, where: [] });
          await send('PUT', `/api/projects/${an}`, aj);
        };
        const notes = (r) => r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        await add('alternatives.csv', ['alternative,cost,benefit,risk', 'A,100,7,2', 'B,150,9,1', 'C,120,5,3'], 'alternatives');
        const cp = await run(an, { tool: 'appraisal/compare.py', params: { table: 'alternatives', name: 'alternative', criteria: 'cost:min:0.5, benefit:max:0.3, risk:min:0.2',
          limits: 'risk<=2.5', method: 'weighted sum' } });
        const rk = cp.state === 'done' ? await tableOf(an, 'ranking') : null;
        assert('urban studio: alternatives compared: C set aside by the limit on risk, and of A and B by hand A first (1 against 0 after min–max)',
          rk && rk.data.alternative.length === 2 && rk.data.rank[rk.data.alternative.indexOf('A')] === 1 && /Set aside by the limits, whatever else they offer: C/.test(notes(cp)), cp.message || notes(cp));
        await add('streams.csv', ['year,benefit,cost', '0,0,100', '1,30,0', '2,30,0', '3,30,0', '4,30,0', '5,30,0'], 'streams');
        const cb = await run(an, { tool: 'appraisal/costs_and_benefits.py', params: { table: 'streams', year: 'year', benefit: 'benefit', cost: 'cost', rate: 5, bands: '', currency: 'EUR' } });
        const pv = [1, 2, 3, 4, 5].reduce((s, t) => s + 30 / 1.05 ** t, 0) - 100;
        assert('urban studio: costs and benefits: NPV = Σ 30/1.05ᵗ − 100 by hand, paying back in year 4', cb.state === 'done' && new RegExp(`net present value of ${Math.round(pv)} EUR`).test(notes(cb)) && /pays back in year 4/.test(notes(cb)),
          cb.message || notes(cb));
        await add('panel.csv', ['round,estimate', '1,10', '1,20', '1,30', '1,40', '1,100', '2,25', '2,28', '2,30', '2,32', '2,40', '3,29', '3,30', '3,30', '3,31', '3,32'], 'panel');
        const dl = await run(an, { tool: 'appraisal/futures.py', params: { method: 'Delphi rounds', table: 'panel', group: 'round', value: 'estimate', exclusions: '', consensus: 0.25, stability: 0.05 } });
        // Round 2: an IQR of 4 on a median of 30 (0.13 ≤ 0.25), and the median unchanged from round 1's 30.
        assert('urban studio: Delphi: the panel agrees and settles by round 2, on 30', dl.state === 'done' && /agrees and has settled by round 2, on a median of 30/.test(notes(dl)), dl.message || notes(dl));
        await add('field.csv', ['parameter,value', 'energy,grid', 'energy,local', 'energy,none', 'ownership,public', 'ownership,private', 'provision,universal', 'provision,means-tested'], 'field');
        await add('exclusions.csv', ['parameter_a,value_a,parameter_b,value_b', 'energy,none,provision,universal', 'ownership,private,provision,universal'], 'exclusions');
        const mf = await run(an, { tool: 'appraisal/futures.py', params: { method: 'morphological field', table: 'field', group: 'parameter', value: 'value', exclusions: 'exclusions', consensus: 0.25, stability: 0.05 } });
        assert('urban studio: a morphological field: 8 of 12 configurations left by two exclusions, by hand', mf.state === 'done' && /Of 12 configurations of 3 parameters, 8 contain/.test(notes(mf)), mf.message || notes(mf));
        const a1 = [0, 1, 2, 1, 0, 2, 2, 1, 0, 1, 2, 0, 1, 1, 2, 0, 0, 2, 1, 1], b1 = [0, 1, 2, 2, 0, 1, 2, 1, 1, 1, 2, 0, 0, 1, 2, 0, 1, 2, 1, 2];
        await add('codes.csv', ['unit,coder,code', ...a1.map((c, u) => `u${u},A,c${c}`), ...b1.map((c, u) => `u${u},B,c${c}`)], 'codes');
        const ag = await run(an, { tool: 'appraisal/agreement.py', params: { table: 'codes', unit: 'unit', coder: 'coder', code: 'code', metric: 'nominal' } });
        const at = ag.state === 'done' ? await tableOf(an, 'agreement') : null;
        const kappa = at ? at.data.value[at.data.coefficient.indexOf("Cohen's κ")] : null;
        assert('urban studio: two coders\' Cohen\'s κ, 0.5454545, as statsmodels has it', kappa !== null && Math.abs(kappa - 0.5454545454545454) < 1e-12, ag.message || JSON.stringify(at?.data));
      }
    }
  } finally {
    server.kill();
    try { rmSync(store, { recursive: true, force: true }); } catch { /* a file still held */ }
  }
}

done();
