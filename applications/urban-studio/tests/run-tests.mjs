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
import { createHash } from 'node:crypto';
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
      // A source is the data library's, or a small file the template brings beside it (templates/data/), named in its `files`.
      const brought = (ref) => /^project:data\//.test(ref) && (t.files || []).includes(ref.slice('project:data/'.length))
        && existsSync(join(APP, 'plugins', g, 'templates', 'data', ref.slice('project:data/'.length)));
      assert(`urban studio: the template ${g}/${file} is a sound project; every step of its plan is a tool that is there, on a slice that is there; its data are the library's or files it brings`,
        wrong.length === 0 && !!t.title && !!t.about && plan.length > 0 && plan.every((e) => [...toolFiles, ...pluginTools].includes(e.tool) && e.id && e.title && (e.slice === null || p.slices.some((s) => s.id === e.slice)))
        && p.sources.every((s) => /^data:/.test(s.ref) || brought(s.ref)) && p.views.every((v) => p.sources.some((s) => s.name === v.table)), wrong.join('; '));
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
    // The host states every plug-in engine's standing before it listens, resolving each engine's headers; with ten engines on many
    // foundations that takes about twenty seconds on Windows, so the server is given a minute.
    for (let k = 0; k < 400 && !up; k++) { await new Promise((r) => setTimeout(r, 150)); try { const r = await fetch(`${at}/api/status`); up = r.ok; status = await r.json(); } catch { /* not yet */ } }
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

      // A town's greenhouse gas inventory and its measures (ADR 0147): the template's invented town laid out with its two files, the
      // inventory read by sector and scope with BASIC+ held back by livestock not estimated, and the measures' curve whose Shapley values
      // sum to the joint abatement, the overlap of measures on one source said.
      if (status.engine?.engines?.materials?.built) {
        const te = await send('POST', '/api/projects', { template: 'materials.a-towns-emissions' });
        kept[te.body.name] = te.body.project;
        const notesOf = (r) => r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        const si = te.body.project.plan.find((x) => x.id === 'e-inventory');
        const ri = await run(te.body.name, { tool: si.tool, params: si.params, slice: si.slice, step: si.id });
        const cells = ri.state === 'done' ? await tableOf(te.body.name, 'inventory_cells') : null;
        assert('urban studio: a town\'s inventory by sector and scope: BASIC complete, BASIC+ held back by livestock not estimated, the key categories found',
          cells?.data && cells.data.status.includes('not estimated (NE)') && /BASIC is complete/.test(notesOf(ri)) && /BASIC\+ is incomplete: agriculture/.test(notesOf(ri)),
          ri.message || notesOf(ri).slice(0, 400));
        const sm = te.body.project.plan.find((x) => x.id === 'e-mitigation');
        const rm = await run(te.body.name, { tool: sm.tool, params: sm.params, slice: sm.slice, step: sm.id });
        const cv = rm.state === 'done' ? await tableOf(te.body.name, 'mitigation_curve') : null;
        const py = rm.state === 'done' ? await tableOf(te.body.name, 'mitigation_pathway') : null;
        assert('urban studio: the town\'s seven measures on a curve, each credited by its Shapley value, the overlap counted once; eleven years ahead',
          cv?.data && cv.data.measure.length === 7 && py?.data && py.data.year.length === 11 && /counting [\d,]+ t twice/.test(notesOf(rm)),
          rm.message || notesOf(rm).slice(0, 400));
        // A city's solid waste (ADR 0148): the invented city laid out with its five files, ninety households weighed, the composting
        // site below control, SDG 11.6.1 within its interval.
        const tw = await send('POST', '/api/projects', { template: 'materials.a-citys-waste' });
        kept[tw.body.name] = tw.body.project;
        const sw = tw.body.project.plan.find((x) => x.id === 'w-account');
        const rw = await run(tw.body.name, { tool: sw.tool, params: sw.params, slice: sw.slice, step: sw.id });
        const fw = rw.state === 'done' ? await tableOf(tw.body.name, 'waste_facilities') : null;
        const cw = rw.state === 'done' ? await tableOf(tw.body.name, 'waste_composition') : null;
        assert('urban studio: a city\'s solid waste: four facilities on the ladder (the composting site and the dumpsite below control), SDG 11.6.1 with its interval, the composition summing to one',
          fw?.data && fw.data.controlled.join() === 'yes,no,yes,no' && /SDG 11\.6\.1: [\d.]+% \(interval/.test(notesOf(rw)) && cw?.data && Math.abs(cw.data.share.reduce((a, b) => a + b, 0) - 1) < 1e-3,
          rw.message || notesOf(rw).slice(0, 400));
        // What the waste emits (ADR 0153): the landfill and the dumpsite in III.1.1 by first-order decay, their methane rising as deposits
        // accumulate, and the entries in the inventory tool's own columns.
        const wes = tw.body.project.plan.find((x) => x.id === 'w-emissions');
        const wer = await run(tw.body.name, { tool: wes.tool, params: wes.params, slice: wes.slice, step: wes.id });
        const wen = wer.state === 'done' ? await tableOf(tw.body.name, 'waste_emissions_entries') : null;
        const wsm = wer.state === 'done' ? await tableOf(tw.body.name, 'waste_site_methane') : null;
        const winv = wer.state === 'done' ? await tableOf(tw.body.name, 'waste_for_inventory') : null;
        const wlandfill = wsm?.data ? wsm.data['t CH4'].filter((_, i) => wsm.data.site[i] === 'landfill') : [];
        assert('urban studio: what the city\'s waste emits: its landfill and dumpsite in III.1.1, the landfill\'s methane rising as its deposits accumulate, the entries in the inventory\'s columns',
          wen?.data && wen.data.code.filter((c) => c === 'III.1.1').length === 2 && wlandfill.length > 2 && wlandfill.at(-1) > wlandfill[1]
          && winv?.data && winv.data.sector.every((x) => x === 'waste') && winv.data.flag.includes('biogenic'), wer.message || notesOf(wer).slice(0, 400));
      }

      // Vulnerability (ADR 0149): an invented survey's fragility fitted, updated and set against what was held before; and Freetown's
      // buildings under its river floods through every model of the JRC's library, the loss a range whose envelope is wider still.
      if (status.engine?.engines?.risk?.built) {
        const notesOf = (r) => r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        const tf = await send('POST', '/api/projects', { template: 'risk.damage-after-an-event' });
        kept[tf.body.name] = tf.body.project;
        const sf = tf.body.project.plan.find((x) => x.id === 'f-learn');
        const rf = await run(tf.body.name, { tool: sf.tool, params: sf.params, slice: sf.slice, step: sf.id });
        const cf = rf.state === 'done' ? await tableOf(tf.body.name, 'fragility_curves') : null;
        const bf = rf.state === 'done' ? await tableOf(tf.body.name, 'fragility_back_test') : null;
        // The survey was drawn from houses weaker than the curve held before: every fitted median lies below it, the update lies nearer
        // the fit than the prior does, and the earlier curve predicts too little damage at every state (z from 3.1 to 8.6).
        const D = cf?.data;
        const between = D && D.state.every((_, k) => D.median_fitted[k] < D.median_before[k] && D.median_updated[k] < D.median_before[k]
          && Math.abs(Math.log(D.median_updated[k] / D.median_fitted[k])) < Math.abs(Math.log(D.median_before[k] / D.median_fitted[k])));
        assert('urban studio: a fragility learnt from an event: four states fitted below the curve held before, the update nearer the fit than the prior, the earlier curve predicting too little damage',
          between && bf?.data && bf.data.z.every((z) => z > 2), rf.message || JSON.stringify(cf?.data || {}).slice(0, 400));
        if (existsSync(join(ROOT, 'data/cache/products/freetown-flood.json')) && existsSync(join(ROOT, 'data/cache/products/damage-functions.json'))) {
          const ft = await send('POST', '/api/projects', { template: 'risk.freetown' });
          kept[ft.body.name] = ft.body.project;
          const sd = ft.body.project.plan.find((x) => x.id === 's-damage');
          const rd = await run(ft.body.name, { tool: sd.tool, params: sd.params, slice: sd.slice, step: sd.id });
          const md = rd.state === 'done' ? await tableOf(ft.body.name, 'damage_models') : null;
          const aal = md?.data?.aal_usd || [];
          assert('urban studio: Freetown\'s buildings under its river floods through nine models of the JRC\'s library: a range of losses, never one number, the envelope wider',
            aal.length === 9 && Math.min(...aal) > 0 && Math.max(...aal) > Math.min(...aal) && /were each building free to take its own curve/.test(notesOf(rd)),
            rd.message || notesOf(rd).slice(0, 400));
        }
        // Well-being (ADR 0150): the invented city's poorest fifth bear more of the well-being losses than of the asset losses, and
        // the embankment for the waterfront, first by assets saved, is not first by well-being.
        const th = await send('POST', '/api/projects', { template: 'risk.households-in-a-flood' });
        kept[th.body.name] = th.body.project;
        const sh = th.body.project.plan.find((x) => x.id === 'w-wellbeing');
        const rh = await run(th.body.name, { tool: sh.tool, params: sh.params, slice: sh.slice, step: sh.id });
        const gq = rh.state === 'done' ? await tableOf(th.body.name, 'wellbeing_income_groups') : null;
        const mq = rh.state === 'done' ? await tableOf(th.body.name, 'wellbeing_measures') : null;
        const poorest = gq?.data ? gq.data.income_group.indexOf('poorest fifth') : -1;
        const dyke = mq?.data ? mq.data.measure.findIndex((m) => /embankment/.test(m)) : -1;
        assert('urban studio: households\' well-being in a flood: the poorest fifth bear more of the well-being losses than of the asset losses; the waterfront\'s embankment first by assets, not by well-being',
          poorest >= 0 && gq.data.share_of_wellbeing_losses[poorest] > gq.data.share_of_asset_losses[poorest] && dyke >= 0 && mq.data.rank_by_assets[dyke] === 1 && mq.data.rank_by_wellbeing[dyke] > 1,
          rh.message || notesOf(rh).slice(0, 400));
      }

      // Public transport and who can reach it (ADR 0151): Melbourne's centre under five readings of SDG 11.2.1, a tram read as of high
      // capacity never reaching fewer people, and the straight line never fewer than the streets.
      if (status.engine?.engines?.transit?.built && existsSync(join(ROOT, 'data/cache/products/transit/melbourne.json'))) {
        const notesOf = (r) => r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        const tt = await send('POST', '/api/projects', { template: 'transit.melbourne' });
        kept[tt.body.name] = tt.body.project;
        const st = tt.body.project.plan.find((x) => x.id === 't-access');
        const rt2 = await run(tt.body.name, { tool: st.tool, params: st.params, slice: st.slice, step: st.id });
        const tr = rt2.state === 'done' ? await tableOf(tt.body.name, 'transit_readings') : null;
        const sh = tr?.data?.share_within_reach || [];
        assert('urban studio: who lives within reach of public transport in Melbourne\'s centre: five readings, a high-capacity tram never reaching fewer, the straight line never fewer than the streets',
          sh.length === 5 && sh[2] >= sh[0] && sh[3] >= sh[1] && sh[4] >= sh[0] && sh.every((x) => x > 0.5 && x <= 1), rt2.message || notesOf(rt2).slice(0, 400));
        // Open space (ADR 0154): four readings of SDG 11.7.1, each wider set of land reaching no fewer people and covering no less of the
        // square, the open land drawn by its kinds.
        const so = tt.body.project.plan.find((x) => x.id === 't-openspace');
        const ro = so ? await run(tt.body.name, { tool: so.tool, params: so.params, slice: so.slice, step: so.id }) : { state: 'missing' };
        const orr = ro.state === 'done' ? await tableOf(tt.body.name, 'openspace_readings') : null;
        const ol = ro.state === 'done' ? await tableOf(tt.body.name, 'openspace_land') : null;
        const os = orr?.data?.share_within_reach || [], oa = orr?.data?.area_share || [];
        assert('urban studio: who lives within reach of open space in Melbourne\'s centre: four readings, each wider set of land reaching no fewer and covering no less, the land drawn by its kinds',
          os.length === 4 && os[1] >= os[0] && os[2] >= os[1] && oa[1] >= oa[0] && oa[2] >= oa[1] && oa[3] <= oa[0] && os.every((x) => x > 0 && x <= 1) &&
          ol?.rows > 100 && ol.data.kind.includes('park'), ro.message || notesOf(ro).slice(0, 400));
      }

      // Homes and who lacks them (ADR 0155): an invented city's survey under six readings of SDG 11.1.1; tenure counted, two to a room
      // and a hazardous site are wider readings and lower neither bound, walls and roof alone is narrower and raises neither (the JMP's
      // ladder is wider for toilets and narrower for water, so it is only read); the new estates below the riverside.
      if (status.engine?.engines?.shelter?.built) {
        const hb = await send('POST', '/api/projects', { template: 'shelter.a-citys-homes' });
        kept[hb.body.name] = hb.body.project;
        const hs = hb.body.project.plan.find((x) => x.id === 'h-shelter');
        const hr = await run(hb.body.name, { tool: hs.tool, params: hs.params, slice: hs.slice, step: hs.id });
        const ht = hr.state === 'done' ? await tableOf(hb.body.name, 'shelter_readings') : null;
        const hd = hr.state === 'done' ? await tableOf(hb.body.name, 'shelter_districts') : null;
        const lo = ht?.data?.slum_if_unknowns_not_deprived || [], up = ht?.data?.slum_if_unknowns_deprived || [];
        const at = (d) => hd?.data ? hd.data.slum_share[hd.data.district.indexOf(d)] : null;
        const notes = hr.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        assert('urban studio: homes and who lacks them: six readings of SDG 11.1.1, tenure, two to a room and a hazardous site never lowering a bound, walls and roof alone never raising one; the new estates below the riverside',
          lo.length === 6 && lo.every((x, k) => x > 0 && up[k] <= 1 && x <= up[k]) && [0, 3, 5].every((k) => k === 0 ? lo[1] >= lo[0] && up[1] >= up[0] : lo[k] >= lo[1] && up[k] >= up[1]) && lo[4] <= lo[1] && up[4] <= up[1] &&
          at('New Estates') < at('Riverside'), hr.message || notes.slice(0, 400));
      } else skip('urban studio: homes and who lacks them (the shelter engine is not built: python applications/urban-studio/engine.py build shelter)');

      // Visitors, how many and when (ADR 0156): six city-regions from Eurostat, each reading adding to the stock (platforms,
      // unregistered stays, same-day visitors), Venice's seasonality the strongest and its platform rentals carried from Veneto.
      if (status.engine?.engines?.visitors?.built && existsSync(join(ROOT, 'data/cache/products/visitors/european-cities-2024.json'))) {
        const vb = await send('POST', '/api/projects', { template: 'visitors.european-cities' });
        kept[vb.body.name] = vb.body.project;
        const vs = vb.body.project.plan.find((x) => x.id === 'v-stock');
        const vr = await run(vb.body.name, { tool: vs.tool, params: vs.params, slice: vs.slice, step: vs.id });
        const vt = vr.state === 'done' ? await tableOf(vb.body.name, 'visitors_readings') : null;
        const vm = vr.state === 'done' ? await tableOf(vb.body.name, 'visitors_measures') : null;
        const per = vt?.data?.mean_present_per_thousand || [];
        let rising = per.length === 24;
        for (let k = 0; k + 3 < per.length; k += 4) rising = rising && per[k] <= per[k + 1] && per[k + 1] <= per[k + 2] && per[k + 2] <= per[k + 3];
        const venice = vm?.data ? vm.data.place.findIndex((x) => /Venice/.test(x)) : -1;
        const vnotes = vr.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
        assert('urban studio: visitors in six European city-regions: each wider reading of "visitor" adds to the stock; Venice the most seasonal, its platform rentals carried from Veneto',
          rising && venice >= 0 && vm.data.gini[venice] === Math.max(...vm.data.gini) && /took its region's platform rentals/.test(vnotes), vr.message || vnotes.slice(0, 400));
      } else skip('urban studio: visitors (the visitors engine, or the data library\'s european-cities-2024.json, is not here: run the recipe)');

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

      // Evidence in space and time (ADR 0123): the plug-in's server side and views, an archive kept, a file taken in and handed over,
      // and, where its engine is built and its example is in the cache, the example laid out, appraised and its tools run.
      const evp = status?.plugins?.find((p) => p.name === 'evidence');
      assert('urban studio: the plug-in of evidence in space and time brings three tools, an engine, a server side and two views',
        evp && evp.tools === 3 && evp.engine === true && evp.routes === true && evp.views.join() === 'evidence.js,holdings.js' && evp.styles.join() === 'evidence.css' && evp.domains.includes('tekmerion'), JSON.stringify(evp));
      const vb = await send('POST', '/api/projects', { title: 'Evidence' });
      const vn = vb.body.name;
      kept[vn] = vb.body.project;
      const ev0 = await json(`/api/projects/${vn}/evidence`);
      assert('urban studio: a project keeps no archive of evidence until one is made, and is told which examples can be laid out', ev0.status === 200 && ev0.body.kept === false && ev0.body.archive.schema === 'noetarium.evidence/1' && Array.isArray(ev0.body.cases));
      const taken = await json(`/api/projects/${vn}/evidence/holdings?name=note.txt`, { method: 'POST', headers: { Origin: at }, body: 'rats seen at the back\n' });
      const want = createHash('sha256').update('rats seen at the back\n').digest('hex');
      assert('urban studio: a file taken in is kept with the SHA-256 of its bytes and whoever took it in as its first custodian', taken.status === 200 && taken.body.holding.digest === want && !!taken.body.holding.custodian, JSON.stringify(taken.body));
      const handed = await send('POST', `/api/projects/${vn}/evidence/transfer`, { holding: taken.body.holding.id, to: 'the analyst', note: 'for review' }, { Origin: at });
      assert('urban studio: a holding handed over is written into its chain, from its holder, with the digest observed then', handed.status === 200 && handed.body.holding.transfers[0].from === taken.body.holding.custodian && handed.body.holding.transfers[0].digest === want);
      const ev1 = await json(`/api/projects/${vn}/evidence`);
      const stale = await send('PUT', `/api/projects/${vn}/evidence`, { archive: { ...ev1.body.archive, title: 'Another' }, base: '0'.repeat(64) }, { Origin: at });
      const fresh = await send('PUT', `/api/projects/${vn}/evidence`, { archive: { ...ev1.body.archive, title: 'Rats' }, base: ev1.body.sha256 }, { Origin: at });
      const twice = await send('PUT', `/api/projects/${vn}/evidence`, { archive: { ...ev1.body.archive, items: [{ id: 'x' }, { id: 'x' }] } }, { Origin: at });
      assert('urban studio: an archive kept over a change made meanwhile is refused with the current one; one naming an item twice is refused',
        stale.status === 409 && stale.body.current.holdings.length === 1 && fresh.status === 200 && twice.status >= 400 && /more than once/.test(twice.body.message), `${stale.status} ${fresh.status} ${twice.status}`);
      const other = await send('POST', `/api/projects/${vn}/evidence/transfer`, { holding: taken.body.holding.id, to: 'someone' }, { Origin: 'http://elsewhere.example' });
      assert('urban studio: a change from another page is refused', other.status === 403);
      const bytes = await fetch(`${at}/api/projects/${vn}/evidence/holdings/note.txt`).then((r) => r.text());
      assert('urban studio: a holding\'s bytes are served as they were taken in', bytes === 'rats seen at the back\n');
      if (!status.engine?.engines?.evidence?.built) skip('urban studio: an archive of evidence appraised (the evidence engine is not built: run the recipe)');
      else {
        const ap = await send('POST', `/api/projects/${vn}/evidence/appraise`, {});
        assert('urban studio: an archive of one file and no items is appraised: the file intact, its custody unbroken through one handover',
          ap.status === 200 && ap.body.appraisal.keep.holdings[0].fixity === 'intact' && ap.body.appraisal.keep.holdings[0].custody.status === 'holds', JSON.stringify(ap.body).slice(0, 300));
        if (!existsSync(join(ROOT, 'data/cache/products/urban-evidence/nyc-rodents/archive.json'))) skip('urban studio: the example of evidence (its product is not in the cache: python data/products/urban_evidence.py)');
        else {
          const tb = await send('POST', '/api/projects', { template: 'evidence.rats-in-the-east-village' });
          const tn = tb.body.name;
          kept[tn] = tb.body.project;
          const laid = await send('POST', `/api/projects/${tn}/evidence/example`, { case: 'nyc-rodents' }, { Origin: at });
          const ex = (await send('POST', `/api/projects/${tn}/evidence/appraise`, {})).body.appraisal;
          const standing = (s) => ex.weigh.claims.filter((c) => c.standing === s).length;
          assert('urban studio: the example laid out: 1,566 items on 399 claims, both files intact; every claim weighed, some established and some refuted, a household\'s repeated complaints counted once',
            laid.status === 200 && laid.body.items === 1566 && ex.keep.verdict.status === 'holds' && ex.weigh.claims.length === 399 && standing('established') > 50 && standing('refuted') >= 1
              && ex.weigh.claims.some((c) => c.overstatement > 5), JSON.stringify([laid.body, standing('established'), standing('refuted')]));
          assert('urban studio: what to do: the looks bought within the budget, no other set gaining more', ex.decide.inspect.length > 10 && ex.decide.spent <= ex.decide.budget && ex.decide.exact, ex.decide.why);
          const placeOf = await json(`/api/projects/${tn}/evidence/place`);
          assert('urban studio: the place the archive names is served for the page: its ways and footprints', placeOf.status === 200 && placeOf.body.ways.length > 100 && placeOf.body.buildings.length > 100);
          const notesOf = (r) => r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
          const ca = await run(tn, { tool: 'evidence/calibrate.py', params: { table: 'calibration', reference: 'reference', found: 'found', reported: 'reported', confidence: 0.95 } });
          const ch = ca.state === 'done' ? await tableOf(tn, 'channel') : null;
          assert('urban studio: a resident\'s complaint counted against the inspectors, lot by lot: 26 of 213 lots found with rats and 22 of 1,217 passed, each with its intervals',
            ch && ch.data.reported.join() === '26,22' && ch.data.of.join() === '213,1217' && ch.data.wilson_low[0] < 26 / 213 && ch.data.exact_high[1] > 22 / 1217, ca.message || JSON.stringify(ch?.data));
          const wg = await run(tn, { tool: 'evidence/weigh.py', params: { archive: '' } });
          const cl = wg.state === 'done' ? await tableOf(tn, 'claims') : null;
          assert('urban studio: the claims weighed from the project\'s own archive, as a table placed on the map with the act for each', cl && cl.rows === 399 && cl.roles.lat === 'lat' && cl.data.act.includes('leave it'), wg.message || notesOf(wg));
          const fr = await run(tn, { tool: 'evidence/fared.py', params: { archive: '', table: 'outcome', claim: 'claim', looked: 'looked', found: 'found', source: 'inspector' } });
          const sc = fr.state === 'done' ? await tableOf(tn, 'fared_scores') : null;
          assert('urban studio: how the conclusions fared: the archive\'s predictions of the later inspections score better than the prior\'s alone',
            sc && sc.data.brier[0] < sc.data.brier[1] && sc.data.log_loss[0] < sc.data.log_loss[1], fr.message || notesOf(fr));
        }
      }

      // Population dynamics (ADR 0124): the plug-in's server side and view; and, where its engine is built and a place is in the cache, the
      // laws learned and judged, the modes, a run ahead with a source, and its tools.
      const pp = status?.plugins?.find((p) => p.name === 'population');
      assert('urban studio: the plug-in of population dynamics brings five tools, an engine, a server side and a view',
        pp && pp.tools === 5 && pp.engine === true && pp.routes === true && pp.views.join() === 'population.js' && pp.styles.join() === 'population.css' && pp.domains.includes('kinesis'), JSON.stringify(pp));
      const pb = await send('POST', '/api/projects', { title: 'Population' });
      const pn = pb.body.name;
      kept[pn] = pb.body.project;
      const p0 = await json(`/api/projects/${pn}/population`);
      assert('urban studio: a project\'s study of population starts empty, and is told the places the data library has made', p0.status === 200 && Array.isArray(p0.body.cases) && JSON.stringify(p0.body.study) === '{}');
      const bad = await send('POST', `/api/projects/${pn}/population/learn`, { case: '../evil' });
      assert('urban studio: a place that is not a name is refused', bad.status >= 400 && /not the name of a place/.test(bad.body.message || ''), JSON.stringify(bad.body));
      const elsewhere = await send('PUT', `/api/projects/${pn}/population`, { study: { case: 'x' } }, { Origin: 'http://elsewhere.example' });
      assert('urban studio: a study kept from another page is refused', elsewhere.status === 403);
      const keptStudy = await send('PUT', `/api/projects/${pn}/population`, { study: { case: 'melbourne', law: 'drift' } }, { Origin: at });
      assert('urban studio: a study kept from the work\'s own page is read back', keptStudy.status === 200 && (await json(`/api/projects/${pn}/population`)).body.study.law === 'drift');
      const place = p0.body.cases.find((c) => c.case === 'melbourne') || p0.body.cases[0];
      if (!status.engine?.engines?.population?.built) skip('urban studio: population dynamics asked (the population engine is not built: run the recipe)');
      else if (!place) skip('urban studio: population dynamics asked (no place is in the cache: python data/products/population_history.py)');
      else {
        const fieldOf = await json(`/api/projects/${pn}/population/field?case=${place.case}`);
        const cells = fieldOf.body.land.flat().filter((v) => v).length;
        assert('urban studio: a place\'s field is served, its land cells and its years', fieldOf.status === 200 && cells === place.land_cells && fieldOf.body.epochs[0] === 1975 && fieldOf.body.projected.includes(2030));
        const learned = await send('POST', `/api/projects/${pn}/population/learn`, { case: place.case });
        const by = Object.fromEntries((learned.body.models || []).map((m) => [m.name, m]));
        assert('urban studio: every law learned and judged on the two years kept aside, each with its skill and its skill at placing the change',
          learned.status === 200 && learned.body.held_out.join() === '2015,2020' && ['persistence', 'own-rates', 'growth', 'logistic', 'fisher', 'drift', 'crowding', 'settled', 'sparse', 'modes'].every((n) => by[n] && (by[n].held_out || by[n].error))
            && by.persistence.held_out.skill === 0 && Math.abs(by.growth.held_out.pattern_skill) < 1e-9, JSON.stringify(learned.body).slice(0, 400));
        assert('urban studio: no law is left with a negative spread: a diffusion found so is set aside, and said to be',
          Object.values(by).every((m) => !(m.law || []).some((t) => (t.term === 'diffusion' || t.term === 'crowding') && t.coefficient < 0)), '');
        assert('urban studio: the currents of each law at the last year fitted, one per cell', Object.values(learned.body.currents).every((c) => c.east.length === cells));
        const modes = await send('POST', `/api/projects/${pn}/population/modes`, { case: place.case, show: 2 });
        assert('urban studio: the record\'s modes, their rates and their maps', modes.status === 200 && modes.body.rank >= 2 && modes.body.modes[0].map.length === cells && modes.body.residual < 0.1, JSON.stringify(modes.body).slice(0, 300));
        const ahead = await send('POST', `/api/projects/${pn}/population/simulate`, { case: place.case, law: ['diffusion'], coefficients: [0.2], sources: [{ row: 40, col: 40, people_per_year: 1000 }], to: 2030 });
        assert('urban studio: a run that only moves people, with a new source of 1000 a year, gains exactly 10,000 people by 2030',
          ahead.status === 200 && Math.abs(ahead.body.totals[ahead.body.totals.length - 1] - ahead.body.totals[0] - 10000) < 1e-3 && ahead.body.against_projections.length === 2, JSON.stringify(ahead.body.totals));
        const pt = await send('POST', '/api/projects', { template: `population.${place.case}-1975-2050` });
        if (pt.status === 200) {
          const tn = pt.body.name;
          kept[tn] = pt.body.project;
          const lw = await run(tn, { tool: 'population/laws.py', params: { case: place.case, held_out: 2, weights: 'poisson' } });
          const lt = lw.state === 'done' ? await tableOf(tn, 'laws') : null;
          assert('urban studio: the laws as a table, each with its skills and what it says in words', lt && lt.rows >= 10 && lt.data.law.some((w) => /growth at/.test(w)), lw.message || JSON.stringify(lt?.data).slice(0, 300));
          const ah = await run(tn, { tool: 'population/ahead.py', params: { case: place.case, model: 'drift', to: 2040 } });
          const at_ = ah.state === 'done' ? await tableOf(tn, 'ahead') : null;
          assert('urban studio: the years ahead as a table placed on the map, a column a year, with its currents', at_ && at_.rows === cells && at_.data.p2040 && at_.data.east && at_.roles.lat === 'lat', ah.message || '');
        } else skip(`urban studio: the template of ${place.case} (none is written for it)`);
      }

      // Cities across scales (ADR 0128): the plug-in's server side and view, the catalogue of indexes; and, where its engine is built and
      // the city across scales is in the cache, every index at every level, a model that names what it still needs, and the template's
      // tools: a ratio of sums at every level, the census's withheld floor kept within its bounds, growth judged at each level, and
      // declarations ranked.
      const sp = status?.plugins?.find((p) => p.name === 'scales');
      assert('urban studio: the plug-in of cities across scales brings five tools, an engine, a server side and a view',
        sp && sp.tools === 5 && sp.engine === true && sp.routes === true && sp.views.join() === 'scales.js' && sp.styles.join() === 'scales.css' && sp.domains.includes('kinesis'), JSON.stringify(sp));
      const sb = await send('POST', '/api/projects', { title: 'Scales' });
      const sn = sb.body.name;
      kept[sn] = sb.body.project;
      const sc = await json(`/api/projects/${sn}/scales/catalogue`);
      assert('urban studio: the catalogue of indexes is read, quantitative and qualitative, each with how it is carried up the levels',
        sc.status === 200 && sc.body.indexes.length > 1000 && sc.body.counts.class.qualitative > 100 && sc.body.counts.aggregation['ratio-of-sums'] > 100, JSON.stringify(sc.body?.counts || sc.body).slice(0, 300));
      const scaleCase = (await json(`/api/projects/${sn}/scales`)).body?.cases?.[0];
      if (!status.engine?.engines?.scales?.built) skip('urban studio: cities across scales asked (the scales engine is not built: run the recipe)');
      else if (!scaleCase) skip('urban studio: cities across scales asked (no city across scales is in the cache: python data/products/city_scales.py)');
      else {
        const ix = await send('POST', `/api/projects/${sn}/scales/indexes`, { case: scaleCase.name });
        const vac = ix.body?.indexes?.[0];
        assert('urban studio: every index at every level and year, the first read at the block, the area and the city',
          ix.status === 200 && vac && ['block', 'area', 'city'].every((l) => vac.levels[l]?.length === ix.body.years.length), ix.body?.message || '');
        const ds = await send('POST', `/api/projects/${sn}/scales/describe`, { case: scaleCase.name, model: 'vacancy', undeclared: true });
        assert('urban studio: the vacancy cycle names the rents no record holds, at every area, to be declared',
          ds.status === 200 && ds.body.described.missing.some((m) => m.quantity === 'rho' && m.units === m.of), JSON.stringify(ds.body).slice(0, 300));
        const st = await send('POST', '/api/projects', { template: `scales.${scaleCase.name}-2002-2024` });
        if (st.status === 200) {
          const tn = st.body.name;
          kept[tn] = st.body.project;
          const ir = await run(tn, { tool: 'scales/indexes.py', params: { case: scaleCase.name, year: 2024 } });
          const it = ir.state === 'done' ? await tableOf(tn, 'indexes_city') : null;
          const ia = ir.state === 'done' ? await tableOf(tn, 'indexes_areas') : null;
          assert('urban studio: indexes across scales as tables for the blocks, the areas and the city over the years',
            it && ia && it.rows === ix.body.years.length && ia.rows === ix.body.levels[1].units.length, ir.message || '');
          const wr = await run(tn, { tool: 'scales/withheld.py', params: { case: scaleCase.name, year: 2024, use: 'homes' } });
          const wt = wr.state === 'done' ? await tableOf(tn, 'withheld_blocks') : null;
          const inside = wt && wt.data['homes, estimated (m²)'].every((v, i) => v == null || (v >= wt.data['homes, stated (m²)'][i] - 1e-6 && v <= wt.data['homes, at most (m²)'][i] + 1e-6));
          assert('urban studio: what the census withheld, each estimate within the census\'s own bounds', inside, wr.message || '');
          const gr = await run(tn, { tool: 'scales/growth.py', params: { case: scaleCase.name, use: 'homes', fit_to: 2016, most: 20 } });
          const gj = gr.state === 'done' ? await tableOf(tn, 'growth_judged') : null;
          assert('urban studio: where growth goes, judged on the years held aside at the areas and the blocks against standing still',
            gj && gj.rows === 2 && gj.data.level.join() === 'areas,blocks', gr.message || '');
          const vr = await run(tn, { tool: 'scales/vacancy.py', params: { case: scaleCase.name, fit_to: 2016, draws: 16 } });
          const vd = vr.state === 'done' ? await tableOf(tn, 'vacancy_declarations') : null;
          assert('urban studio: the vacancy cycle\'s declarations ranked by what they move, the rents among them', vd && vd.data.declared.includes('rho'), vr.message || '');
        } else skip('urban studio: the template of cities across scales (none is written for this city)');
      }

      // Lifelines and evacuation (ADR 0129): four tools, an engine, two templates; and, where its engine is built and Freetown's assessment
      // is in the cache, the template's financing of the losses: the strategies costed, the cheapest tower, a parametric cover refused.
      const rk = status?.plugins?.find((p) => p.name === 'risk');
      assert('urban studio: the plug-in of lifelines and evacuation brings seven tools and an engine, on the domains of risk',
        rk && rk.tools === 7 && rk.engine === true && rk.domains.includes('disaster-risk') && rk.domains.includes('risk-management'), JSON.stringify(rk));
      if (!status.engine?.engines?.risk?.built) skip('urban studio: the losses financed (the risk engine is not built: run the recipe)');
      else {
        const rt = await send('POST', '/api/projects', { template: 'risk.freetown' });
        if (rt.status === 200) {
          const tn = rt.body.name;
          kept[tn] = rt.body.project;
          assert('urban studio: the template of Freetown plans its six steps, every one a tool of the plug-in', rt.body.project.plan.length === 6 && rt.body.project.plan.every((st) => st.tool.startsWith('risk/')));
          const fr = await run(tn, { tool: 'risk/financing.py', params: { case: 'freetown' } });
          const fs = fr.state === 'done' ? await tableOf(tn, 'financing_strategies') : null;
          const tw = fr.state === 'done' ? await tableOf(tn, 'financing_tower') : null;
          assert('urban studio: Freetown\'s losses met by layers, each strategy cheaper than leaving everything unfunded and the cheapest tower found',
            fs && fs.rows === 4 && fs.data.expected_cost_a_year.slice(1).every((v) => v < fs.data.expected_cost_a_year[0]) && tw && tw.rows >= 1, fr.message || '');
        } else skip('urban studio: the template of Freetown (its assessment is not in the cache: python data/products/freetown_flood.py)');
      }

      // Allocation and justice (ADR 0136): four tools, an engine, a server side and a view; and, where its engine is built and the cases are
      // in the cache, England's fund divided and judged (proportional claims undominated, equal awards to districts dominated), its record
      // replayed with an appeal heard for everyone, Melbourne's care fund with a pivot between principles, and Warsaw's ballots counted again
      // (utilitarian greedy reproducing the city's selection, equal shares leaving fewer voters with nothing).
      const al = status?.plugins?.find((p) => p.name === 'allocation');
      assert('urban studio: the plug-in of allocation brings seven tools, an engine, a server side and a view, on the allocation domain',
        al && al.tools === 7 && al.engine === true && al.routes === true && al.views.join() === 'allocation.js' && al.styles.join() === 'allocation.css' && al.domains.includes('allocation'), JSON.stringify(al));
      if (!status.engine?.engines?.allocation?.built) skip('urban studio: allocation asked (the allocation engine is not built: run the recipe)');
      else {
        const ap = await send('POST', '/api/projects', { title: 'Allocation' });
        const an = ap.body.name;
        kept[an] = ap.body.project;
        const cases = (await json(`/api/projects/${an}/allocation`)).body?.cases || [];
        if (!cases.some((c) => c.name === 'england-iod2025')) skip('urban studio: allocation on England (the case is not in the cache: python data/products/allocation_cases.py)');
        else {
          const acts = [{ act: 'declare', when: '2026-11-20T09:00:00Z', who: 'the example' }, { act: 'allocate', when: '2026-11-20T10:00:00Z', who: 'the example' },
            { act: 'appeal', when: '2026-11-20T11:00:00Z', who: 'a council', entity: 'E06000001', kind: 'datum', key: 'need_claim', value: 2400000, reason: 'an invented appeal, for the check' },
            { act: 'review', when: '2026-11-20T12:00:00Z', who: 'the example', appeal: 0, upheld: true, reason: 'upheld' }, { act: 'allocate', when: '2026-11-20T13:00:00Z', who: 'the example' }];
          const c = await send('POST', `/api/projects/${an}/allocation/compute`, { case: 'england-iod2025', amount: 1e9, per_person: 80, rule: 'proportional', acts });
          const J = c.body?.judgement;
          const named = (part) => J?.options?.findIndex((o) => o.names.some((n) => n.includes(part)));
          assert('urban studio: England\'s fund divided to the pound, Birmingham reached first',
            c.status === 200 && Math.abs(c.body.result.entities.reduce((s, e) => s + e.amount, 0) + c.body.result.left - 1e9) < 1 && c.body.result.order[0].entity === 'E08000025', c.body?.message || '');
          assert('urban studio: under the case\'s principles proportional claims are undominated and equal awards to districts dominated',
            J && J.declared_undominated && J.options[named('equal_awards')]?.undominated === false, JSON.stringify(J?.said));
          const rules = Object.fromEntries((c.body?.procedure?.rules || []).map((r) => [r.name, r.status]));
          assert('urban studio: the record replayed, the appeal heard for everyone and the revised round applied: correctability and consistency hold',
            rules.correctability === 'holds' && rules.consistency === 'holds' && c.body.procedure.hearings[0].hearing.changed.length > 100, JSON.stringify(rules));
          const at_ = await send('POST', '/api/projects', { template: 'allocation.england-councils' });
          const tn = at_.body?.name;
          if (tn) {
            kept[tn] = at_.body.project;
            const tr = await run(tn, { tool: 'allocation/targeting.py', params: { deciles: 1, amount: 1e9 } });
            const tt = tr.state === 'done' ? await tableOf(tn, 'targeting') : null;
            assert('urban studio: a fund to the most deprived decile reaches well under all of the income-deprived, most of whom live elsewhere',
              tt && tt.data.value[0] > 0.3 && tt.data.value[0] < 0.7 && tt.data.value[2] > tt.data.value[1] / 2, tr.message || JSON.stringify(tt?.data));
          }
        }
        if (cases.some((c) => c.name === 'melbourne-areas')) {
          const m = await send('POST', `/api/projects/${an}/allocation/compute`, { case: 'melbourne-areas', amount: 2e6, per_person: 2000 });
          assert('urban studio: Melbourne\'s care fund: two areas with no one take no part, and the choice between its orders turns on which principle comes first',
            m.status === 200 && m.body.result.entities.filter((e) => e.excluded).length === 2 && m.body.judgement.pivots.some((p) => /turns on/.test(p.pivot)), JSON.stringify(m.body?.judgement?.said || m.body));
        } else skip('urban studio: allocation on Melbourne (the case is not in the cache)');
        // Resources of many kinds allocated by evolution (ADR 0137), on Melbourne's areas: every type lawful, every kept allocation lawful
        // with the duties all assigned, compared under principles, order found to matter, the record verified; a person's judgements read
        // as strengths; the tool's tables; a model refused from elsewhere than this computer is not tested here (the page asks it).
        if (cases.some((c) => c.name === 'melbourne-areas')) {
          const ev = await send('POST', `/api/projects/${an}/allocation/evolve`, { case: 'melbourne-areas', generations: 120, seed: 2 });
          const R = ev.body?.result, keptAllocations = R?.kept || [];
          assert('urban studio: Melbourne by evolution: every resource type lawful, every kept allocation lawful, the duties all assigned',
            ev.status === 200 && ev.body.composed.types.every((t) => t.lawful) && keptAllocations.length > 0 && keptAllocations.every((k) => k.lawful && k.amounts['maintenance duties'].reduce((a, b) => a + b, 0) === 4),
            ev.body?.message || JSON.stringify(keptAllocations.map((k) => k.amounts['maintenance duties'])));
          assert('urban studio: the kept compared under three principles, order found to matter with a witness, the record verified',
            R && R.comparison && R.comparison.principles.length === 3 && R.order.may_matter && R.order.witnesses.length > 0 && R.verified, JSON.stringify(R?.order || {}));
          const st = await send('POST', `/api/projects/${an}/allocation/strengths`, { items: 3, judgements: [{ first: 0, second: 1, first_preferred: true }, { first: 1, second: 2, first_preferred: true }, { first: 2, second: 0, first_preferred: false }], prior: 0.5 });
          assert("urban studio: a person's judgements read as strengths, the preferred strongest", st.status === 200 && st.body.strength[0] > st.body.strength[2], JSON.stringify(st.body));
          const er = await run(an, { tool: 'allocation/evolve.py', params: { case: 'melbourne-areas', generations: 60, seed: 1 } });
          const et = er.state === 'done' ? await tableOf(an, 'evolved_best') : null;
          assert('urban studio: the evolution tool answers the first allocation area by area, with the order of grants', et && et.data.area.length === 13 && et.data.granted.length === 13, er.message || '');
          // The economic evaluators at the studio (ADR 0141): habits calibrated to the data library's record, and a care fund allocated in an
          // economy of places, judged by the money re-spent in each area and by a declared value, the two kept apart.
          const cr = await run(an, { tool: 'allocation/calibrate.py', params: { case: 'melbourne-areas', short_below: 300 } });
          if (cr.state === 'done') {
            const ct = await tableOf(an, 'calibrated_habits');
            const spend = ct?.data?.value?.[ct.data.habit.indexOf('share of holdings spent a month')];
            assert("urban studio: an economy's habits calibrated to records: the accounts' shares read, the share spent a month fitted", ct && spend > 0 && spend < 1 && ct.data.how.some((h) => h.startsWith('fitted')), JSON.stringify(ct?.data || {}));
            // In a project of its own: a table is read by its name, and this project already holds the first case's.
            const xp = await send('POST', '/api/projects', { title: 'A care fund in an economy of places' });
            const xn = xp.body?.name;
            kept[xn] = xp.body?.project;
            const xr = await run(xn, { tool: 'allocation/evolve.py', params: { case: 'melbourne-economy', generations: 10, seed: 1 } });
            const xt = xr.state === 'done' ? await tableOf(xn, 'evolved_kept') : null;
            const cols = xt?.data ? Object.keys(xt.data) : [];
            assert('urban studio: a care fund in an economy of places: the kept read by the money re-spent and by the value to the people in need, kept apart',
              cols.some((c) => c.startsWith('re-spent')) && cols.includes('value to the people in need') && xt.data.lawful.every((v) => v === 'yes'), xr.message || JSON.stringify(xt || cols));
          } else if (/not in the data library's cache/.test(cr.message || '')) skip('urban studio: habits calibrated to records (python data/products/economy_habits.py)');
          else assert("urban studio: an economy's habits calibrated to records", false, cr.message || '');
        } else skip('urban studio: allocation by evolution on Melbourne (the case is not in the cache)');
        if (cases.some((c) => c.name === 'pabulib-Poland_Warszawa_2023')) {
          const b = await send('POST', `/api/projects/${an}/allocation/budget`, { case: 'pabulib-Poland_Warszawa_2023' });
          const by = Object.fromEntries((b.body?.answer?.rules || []).map((r) => [r.rule, r]));
          assert('urban studio: Warsaw\'s ballots counted again: utilitarian greedy reproduces the city\'s selection, equal shares leaves fewer voters with nothing',
            b.status === 200 && by.greedy && by.recorded && by.greedy.funded.slice().sort((x, y) => x - y).join() === by.recorded.funded.slice().sort((x, y) => x - y).join() &&
            by.equal_shares.share_with_nothing < by.recorded.share_with_nothing && by.recorded.groups.length === 18, b.body?.message || '');
          // A participatory budget as orders of its projects (ADR 0141): the rules' orders proposed, so every rule stands on or behind the front.
          const orr = await run(an, { tool: 'allocation/orders.py', params: { case: 'pabulib-Poland_Warszawa_2023', generations: 10, seed: 1 } });
          const ot = orr.state === 'done' ? await tableOf(an, 'orders_front') : null;
          const rows = ot?.data ? ot.data.selection.map((name, i) => ({ name, nothing: ot.data.share_with_nothing[i], met: ot.data.approvals_met[i] })) : [];
          const front = rows.filter((r) => r.name.startsWith('an order the search kept'));
          const behind = rows.slice(0, 3).every((r) => front.some((f) => f.nothing <= r.nothing + 1e-12 && f.met >= r.met));
          assert("urban studio: Warsaw's budget as orders of its projects: the city's count, greedy per cost and equal shares each on or behind the front of what orders reach",
            ot && rows.length >= 4 && front.length >= 1 && behind, orr.message || JSON.stringify(rows));
        } else skip('urban studio: allocation on Warsaw (the case is not in the cache)');
      }

      // Programmes and public finance (ADR 0144, ADR 0145, ADR 0146): the plug-in's parts; where its engine is built, an invented cohort's
      // scorecard scored part by part with its weaknesses, its programme read from needs to results, a city's finances and its debt capacity,
      // a European country's local government debt with a fan, and what a tram line could take back from the land it raises with a land
      // readjustment and its replotting.
      {
        const pr = status?.plugins?.find((p) => p.name === 'programmes');
        assert('urban studio: the plug-in of programmes and public finance brings six tools and an engine, on the policy and economics domains',
          pr && pr.tools === 6 && pr.engine === true && pr.domains.includes('policy') && pr.domains.includes('economics'), JSON.stringify(pr));
        if (!status.engine?.engines?.programmes?.built) skip('urban studio: programmes asked (the engine is not built: run the recipe)');
        else {
          const pp = await send('POST', '/api/projects', { title: 'Programmes' });
          const pn = pp.body?.name;
          kept[pn] = pp.body?.project;
          const sc = await run(pn, { tool: 'programmes/scorecard.py', params: { city: 'ardel', weak_at: '1' } });
          const parts = sc.state === 'done' ? await tableOf(pn, 'scorecard_parts') : null;
          const weak = sc.state === 'done' ? await tableOf(pn, 'scorecard_weaknesses') : null;
          assert('urban studio: a scorecard scored part by part (governance 2, knowing the risk 1, finance 0, preparedness 3), its three weaknesses with their evidence',
            parts?.data && parts.data.level.join() === '2,1,0,3' && weak?.data && weak.data.statement.join() === 'f2,r1,f1' && weak.data.evidence.every((e) => e.length > 0), sc.message || JSON.stringify(parts?.data));
          const pg_ = await run(pn, { tool: 'programmes/programme.py', params: { date: '2026-10-15' } });
          const claims = pg_.state === 'done' ? await tableOf(pn, 'programme_claims') : null;
          const acts = pg_.state === 'done' ? await tableOf(pn, 'programme_actions') : null;
          assert('urban studio: a programme read on a date: six actions in their states, two of three contribution claims substantiated',
            acts?.data && acts.data.state.length === 6 && claims?.data && claims.data.substantiated.filter((x) => x === 'yes').length === 2, pg_.message || JSON.stringify(claims?.data));
          const fi = await run(pn, { tool: 'programmes/finance.py', params: { floor: 1.5, rate: 0.05, tenor: 10 } });
          const ahead = fi.state === 'done' ? await tableOf(pn, 'finance_years_ahead') : null;
          assert("urban studio: a city's finances: its ratios judged and ten years ahead with the coverage of its debt service",
            ahead?.data && ahead.data.year.length === 10 && ahead.data.coverage.every((c) => c === null || c > 0), fi.message || '');
          const ld = await run(pn, { tool: 'programmes/local_debt.py', params: { geo: 'FR', years: 10, threshold: 0.1 } });
          if (ld.state === 'done') {
            const fan = await tableOf(pn, 'local_debt_fan');
            assert("urban studio: France's local government debt from Eurostat, and a fan of eleven years, its quantiles ordered",
              fan?.data && fan.data.year.length === 11 && fan.data.median.every((m, i) => fan.data['lower (10%)'][i] <= m && m <= fan.data['upper (90%)'][i]), JSON.stringify(fan?.data || {}));
          } else if (/not in the data library's cache/.test(ld.message || '')) skip('urban studio: local government debt (python data/products/local_government_finance.py)');
          else assert('urban studio: local government debt', false, ld.message || '');
          const ca = await run(pn, { tool: 'programmes/capture.py', params: { share: 0.3, but_for: 0.6, rate: 0.04 } });
          const insts = ca.state === 'done' ? await tableOf(pn, 'capture_instruments') : null;
          const plots = ca.state === 'done' ? await tableOf(pn, 'capture_replotting') : null;
          const settled = plots?.data ? plots.data.settlement.reduce((a, b) => a + b, 0) : NaN;
          assert('urban studio: what a tram line could take back: four instruments each with the cost it recovers, and a readjustment giving five owners five plots, settled to zero',
            insts?.data && insts.data.instrument.join() === 'levy,rights,increment,obligation' && insts.data['cost recovered'].every((x) => x > 0)
            && plots?.data && new Set(plots.data.plot).size === 5 && Math.abs(settled) < 0.5, ca.message || JSON.stringify(insts?.data || {}));
          // A project's financing plan (ADR 0152): the receipts lower the borrowing, and the city can afford more with them than without.
          const pj = await run(pn, { tool: 'programmes/project.py', params: { instruments: 'levy, rights, increment, obligation', valuation_year: 2026, years: 20, rate: 0.05, tenor: 20, floor: 1.5, pledge: true } });
          const af = pj.state === 'done' ? await tableOf(pn, 'project_affordable') : null;
          const scales = af?.data ? af.data['largest scale affordable'].map(Number) : [];
          assert('urban studio: can Ardel afford the tram line: less borrowed with the receipts, and a larger project affordable with them than without',
            af?.data && af.data.borrowed[0] < af.data.borrowed[1] && scales.length === 2 && scales[0] > scales[1] && scales[1] > 1, pj.message || JSON.stringify(af?.data || {}));
        }
      }

      // From evidence to action (the design of 2026-11-19): a tool turning any table into evidence, the project's archive keeping each run
      // and verifying against the journal, the ledger; and, where the engagement's engine is built, an engagement laid out from an example,
      // the ledger gathered and a turn run, a need read with its standing over the readings, a stale write refused, a proposal by a role not
      // empowered, the record verified, and a transfer by a lens with its support factor.
      const eg = status?.plugins?.find((p) => p.name === 'engagement');
      assert('urban studio: the plug-in of engagements brings a tool, an engine, a server side and two views, on kybernesis',
        eg && eg.tools === 1 && eg.engine === true && eg.routes === true && eg.views.join() === 'engagement.js,transfer.js' && eg.styles.join() === 'engagement.css' && eg.domains.includes('kybernesis'), JSON.stringify(eg));
      {
        const ep = await send('POST', '/api/projects', { title: 'An engagement' });
        const en = ep.body.name;
        kept[en] = ep.body.project;
        await json(`/api/projects/${en}/files?name=survey.csv`, { method: 'POST', body: 'quantity,value,low,high,place,readings\nrefuge_reach_share,0.93,0.9,0.96,freetown,warning=30 minutes\nevacuation_share_by_warning,0.7,0.62,0.78,freetown,leaving=as in 2011\nevacuation_share_by_warning,0.99,0.97,1,freetown,leaving=at once\n,1,,,,\n' });
        kept[en].sources.push({ id: 's1', name: 'survey', title: 'A survey', ref: 'project:data/survey.csv', roles: {}, where: [] });
        await send('PUT', `/api/projects/${en}`, kept[en]);
        const ev = await run(en, { tool: 'engagement/evidence_table.py', params: { table: 'survey', quantity: 'quantity', value: 'value', low: 'low', high: 'high', place: 'place', readings: 'readings', scale: 'absolute' } });
        const led = await json(`/api/projects/${en}/ledger`);
        const arc = await json(`/api/projects/${en}/archive`);
        assert('urban studio: a table turned into evidence: three items in the ledger, the empty row left out, the run kept in the archive under the digest its step names',
          ev.state === 'done' && led.body.items?.length === 3 && arc.body.runs?.length === 1 && ev.step?.run === arc.body.runs[0].digest, ev.message || JSON.stringify(led.body).slice(0, 300));
        const av = await json(`/api/projects/${en}/archive?verify=1`);
        assert('urban studio: the project\'s archive verifies against its journal', av.status === 200 && av.body.holds && av.body.runs === 1, JSON.stringify(av.body));
        if (!status.engine?.engines?.engagement?.built) skip('urban studio: an engagement steered (the engagement engine is not built: run the recipe)');
        else {
          const e0 = await json(`/api/projects/${en}/engagement`);
          const ex = await json(`/api/projects/${en}/engagement/examples`);
          const blocksWithMethods = new Set((e0.body.methods || []).map((m) => m.block));
          const estimators = (e0.body.methods || []).filter((m) => m.block === 'Estimation');
          assert('urban studio: a project keeps no engagement until one is begun, and is told the methods of seven blocks (four for Estimation, the default first) and the examples',
            e0.status === 200 && e0.body.kept === false && blocksWithMethods.size === 7 && estimators.length === 4 && estimators[0].default && ex.body.examples?.some((x) => x.name === 'freetown-from-miyako'),
            JSON.stringify(e0.body).slice(0, 300));
          const st = await send('POST', `/api/projects/${en}/engagement/start`, { example: 'freetown-from-miyako' }, own);
          const gat = await send('POST', `/api/projects/${en}/engagement/gather`, { turn: 0, base: st.body.sha256 }, own);
          const tu = await send('POST', `/api/projects/${en}/engagement/turn`, { turn: 0, base: gat.body.sha256 }, own);
          const blocks = tu.body?.view?.branches?.[0]?.turns?.[0]?.blocks || [];
          const evals = blocks.find((b) => b.block === 'Evaluation')?.values?.evaluations || [];
          const need = evals.find((f) => f.operator === 'need-out-in-time');
          const reach = evals.find((f) => f.operator === 'asset-reach');
          const miyako = evals.find((f) => f.operator === 'need-out-in-time-miyako');
          assert('urban studio: laid out from the example, the ledger gathered and the turn run: Freetown\'s need is arguable, turning on how leaving is read; its refuges within reach clear; Miyako, unmeasured, undecided',
            st.status === 200 && gat.status === 200 && tu.status === 200 && tu.body.progress.waiting.length === 0 && need?.standing === 'arguable' && need.turns_on.includes('leaving')
              && reach?.standing === 'clear' && miyako?.standing === 'undecided', JSON.stringify({ st: st.body.message, gat: gat.body.message, tu: tu.body.message, need, reach }).slice(0, 600));
          const late = await send('POST', `/api/projects/${en}/engagement/act`, { act: 'fill', block: 'Proposals', turn: 0, role: 'resident', outputs: [[]], base: st.body.sha256 }, own);
          assert('urban studio: an act on an engagement that changed since it was read is refused with the engagement as it stands', late.status === 409 && late.body.conflict && late.body.sha256 === tu.body.sha256, JSON.stringify(late.body).slice(0, 200));
          const guest = await send('POST', `/api/projects/${en}/engagement/act`, { act: 'fill', block: 'Policy', turn: 0, role: 'guest', outputs: [{ advice: 'nothing' }, {}], reason: 'a visitor', base: tu.body.sha256 }, own);
          assert('urban studio: a role not empowered enters a proposal, which awaits a review', guest.status === 200 && guest.body.outcome.undecided && /proposal/.test(guest.body.outcome.reason), JSON.stringify(guest.body.outcome));
          const vf = await json(`/api/projects/${en}/engagement/verify`);
          assert('urban studio: the engagement\'s record verifies', vf.status === 200 && vf.body.holds, JSON.stringify(vf.body));
          const tr = await send('POST', `/api/projects/${en}/engagement/transfer`, ex.body.examples.find((x) => x.name === 'freetown-from-miyako').transfers['freetown-from-miyako'], own);
          const leave = tr.body?.result?.factors?.find((f) => f.name === 'people leave when warned');
          assert('urban studio: leaving at once carried from Miyako to Freetown: arguable, probability in [0.5, 0.8], whether people leave when warned the support factor',
            tr.status === 200 && tr.body.result.standing === 'arguable' && Math.abs(tr.body.result.low - 0.5) < 1e-9 && Math.abs(tr.body.result.high - 0.8) < 1e-9 && leave?.sufficient, JSON.stringify(tr.body).slice(0, 500));
        }
      }

      // Needs (ADR 0125): the plug-in's server side and view; and, where its engine is built and the Melbourne dataset is in the cache, a
      // search run in steps and followed, its state kept, a statement read, tested and placed, and the tools.
      const np = status?.plugins?.find((p) => p.name === 'needs');
      assert('urban studio: the plug-in of needs brings three tools, an engine, a server side and a view',
        np && np.tools === 3 && np.engine === true && np.routes === true && np.views.join() === 'needs.js' && np.styles.join() === 'needs.css' && np.domains.includes('heuresis'), JSON.stringify(np));
      const nb = await send('POST', '/api/projects', { title: 'Needs' });
      const nn = nb.body.name;
      kept[nn] = nb.body.project;
      const n0 = await json(`/api/projects/${nn}/needs`);
      assert('urban studio: a project\'s search for needs starts with no report, told the datasets the data library has made', n0.status === 200 && n0.body.report === null && Array.isArray(n0.body.cases));
      const nbad = await send('POST', `/api/projects/${nn}/needs/read`, { case: '../evil', text: 'x' });
      assert('urban studio: a dataset that is not a name is refused', nbad.status >= 400 && /not the name of a dataset/.test(nbad.body.message || ''), JSON.stringify(nbad.body));
      const nel = await send('POST', `/api/projects/${nn}/needs/run`, { generations: 1 }, { Origin: 'http://elsewhere.example' });
      assert('urban studio: a search is not started by another site\'s page', nel.status === 403, JSON.stringify(nel.body));
      const melb = (n0.body.cases || []).find((c) => c.case === 'melbourne-assistance');
      if (!status.engine?.engines?.needs?.built) skip('urban studio: needs searched (the needs engine is not built: run the recipe)');
      else if (!melb) skip('urban studio: needs searched (the Melbourne dataset is not in the cache: python data/products/needs_tables.py)');
      else {
        const rd = await send('POST', `/api/projects/${nn}/needs/read`, { case: 'melbourne-assistance', text: 'need care for age >= 75 by comparative need_assist higher\nneed care for wealth < 1 by comparative need_assist higher' });
        assert('urban studio: statements read against the data, one refused with the reason', rd.status === 200 && rd.body[0].read && !rd.body[1].read && /not an attribute/.test(rd.body[1].why), JSON.stringify(rd.body).slice(0, 300));
        const ts = await send('POST', `/api/projects/${nn}/needs/test`, { case: 'melbourne-assistance', statements: ['need care for age >= 75 by comparative need_assist higher'] });
        const g = ts.body?.[0]?.evidence?.discovery;
        assert('urban studio: those 75 and over need help with self-care, mobility or communication far more often than others in the City of Melbourne', g && g.group_value > 5 * g.reference_value && g.z > 5, JSON.stringify(g));
        const st = await send('POST', `/api/projects/${nn}/needs/run`, { case: 'melbourne-assistance', generations: 4, step: 2 }, { Origin: at });
        assert('urban studio: a search started from the work\'s own page, in steps', st.status === 200 && st.body.started, JSON.stringify(st.body));
        let rs = null;
        for (let k = 0; k < 240; k++) {
          rs = (await json(`/api/projects/${nn}/needs/run`)).body;
          if (!rs.running) break;
          await new Promise((r) => setTimeout(r, 500));
        }
        const after = await json(`/api/projects/${nn}/needs`);
        const rep = after.body.report;
        assert('urban studio: the search followed to its end, its report kept', !rs.running && rep && rep.generation === 4 && rep.archive.length > 0, JSON.stringify(rs).slice(0, 300));
        assert('urban studio: each statement admitted tested once on the areas kept aside, its tests sealed in the journal', rep.confirmations.length === rep.admissions.length && rep.journal.length === 2 * rep.confirmations.length);
        const more = await send('POST', `/api/projects/${nn}/needs/run`, { case: 'melbourne-assistance', generations: 2, step: 2 }, { Origin: at });
        for (let k = 0; k < 240; k++) {
          rs = (await json(`/api/projects/${nn}/needs/run`)).body;
          if (!rs.running) break;
          await new Promise((r) => setTimeout(r, 500));
        }
        const cont = (await json(`/api/projects/${nn}/needs`)).body.report;
        assert('urban studio: a search continued from its kept state', more.status === 200 && cont.generation === 6 && cont.confirmations.length >= rep.confirmations.length, `${cont?.generation}`);
        const mp = await send('POST', `/api/projects/${nn}/needs/map`, { case: 'melbourne-assistance', statement: 'need care for age >= 75 by comparative need_assist higher' });
        assert('urban studio: a need area by area, each area with its part of the data', mp.status === 200 && mp.body.places.length > 300 && mp.body.places.every((p) => ['discovery', 'validation', 'confirmation'].includes(p.part)));
        // Interpretation (ADR 0126): the declared income lines and references, the rivals; provenance; a model asked only from here.
        const ip = await send('POST', `/api/projects/${nn}/needs/interpret`, { case: 'melbourne-assistance', statements: ['need income for age < 25 by comparative low_income higher'] });
        const it = ip.body?.[0]?.interpretation;
        assert('urban studio: a statement read under four income lines and two references, each interpretation with its statement and verdict',
          ip.status === 200 && it && it.interpretations.length === 8 && it.points.map((p) => p.name).join() === 'income line,reference' && ['clear', 'arguable', 'untenable'].includes(it.record), JSON.stringify(ip.body).slice(0, 300));
        assert('urban studio: and its rivals assessed', ip.body[0].rivals.length === 2 && ip.body[0].rivals.every((r) => typeof r.explains === 'string'), JSON.stringify(ip.body[0].rivals));
        const pv = await json(`/api/projects/${nn}/needs/provenance`);
        assert('urban studio: the search\'s lineage as PROV-JSON', pv.status === 200 && Object.keys(pv.body.entity).length === cont.lineage.length && pv.body.agent['needs:search'], JSON.stringify(pv.body).slice(0, 200));
        const im = await send('POST', `/api/projects/${nn}/needs/interpret-model`, { statement: 'need income for age < 25 by comparative low_income higher' }, { Origin: 'http://elsewhere.example' });
        assert('urban studio: a model is not asked to interpret from elsewhere', im.status === 403, JSON.stringify(im.body));
        const sa = await send('POST', `/api/projects/${nn}/needs/set-aside`, { kind: 'interpretation', index: 3 }, { Origin: at });
        assert('urban studio: setting aside a proposal that is not there is refused', sa.status >= 400, JSON.stringify(sa.body));
        const nt = await send('POST', '/api/projects', { template: 'needs.melbourne-needs' });
        if (nt.status === 200) {
          const tn = nt.body.name;
          kept[tn] = nt.body.project;
          const notesOf = (r) => r.step?.outputs?.filter((o) => o.kind === 'note').map((o) => o.text).join(' ') || '';
          const tt = await run(tn, { tool: 'needs/test.py', params: { case: 'melbourne-assistance', statements: 'need care for age >= 75 by comparative need_assist higher\nneed income for age < 25 by comparative low_income higher' } });
          const tb = tt.state === 'done' ? await tableOf(tn, 'tested') : null;
          assert('urban studio: statements tested as a table, each with its gap, its people and its standing', tb && tb.rows === 2 && tb.data.standing.every((x) => x === 'supported'), tt.message || notesOf(tt));
          const ar = await run(tn, { tool: 'needs/areas.py', params: { case: 'melbourne-assistance', statement: 'need care for age >= 75 by comparative need_assist higher' } });
          const at2 = ar.state === 'done' ? await tableOf(tn, 'need_by_area') : null;
          assert('urban studio: a need area by area as a table placed on the map', at2 && at2.rows > 300 && at2.roles.lat === 'lat', ar.message || '');
          const ev = await run(tn, { tool: 'needs/evolve.py', params: { case: 'melbourne-assistance', generations: 3 } });
          const ef = ev.state === 'done' ? await tableOf(tn, 'needs_found') : null;
          assert('urban studio: a search run apart by the tool, its needs kept as a table', ef && ef.rows > 0 && ef.data.statement.every((x) => /^need /.test(x)), ev.message || notesOf(ev));
        } else skip('urban studio: the template Needs in the City of Melbourne');
      }
    }
  } finally {
    server.kill();
    try { rmSync(store, { recursive: true, force: true }); } catch { /* a file still held */ }
  }
}

done();
