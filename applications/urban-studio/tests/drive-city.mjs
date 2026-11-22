// The work on cities' development of 22 November 2026 driven in headless Chrome (not in the suite), for the gallery's pictures: projects
// begun from the templates of Programmes and public finance (scorecards, a programme, a city's finances, France's local debt, land value
// capture), Materials and flows (a town's inventory and measures, a city's waste), Lifelines and evacuation (a fragility learnt from an
// event, Freetown's buildings under every damage function, households' well-being), Public transport, open space and who can reach
// them (Melbourne) and Homes and who lacks them (an invented city); each planned step run from the page, then its table, chart or
// layers shown with the step's note beside. A picture of each.
//   node applications/urban-studio/tests/drive-city.mjs <base> <folder for pictures>
// ONLY=<part of a template's name> drives one project.
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { mkdirSync } from 'node:fs';

const [BASE = 'http://127.0.0.1:9090/', OUT = join(tmpdir(), 'urban-city-drive')] = process.argv.slice(2);
mkdirSync(OUT, { recursive: true });
const origin = BASE.replace(/\/$/, '');
const post = async (path, body) => (await fetch(`${BASE}${path}`, { method: 'POST', headers: { 'Content-Type': 'application/json', Origin: origin }, body: JSON.stringify(body) })).json();
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const js = (v) => JSON.stringify(v);
let passed = 0, failures = 0;
const assert = (name, ok, detail = '') => { ok ? passed++ : failures++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
for (let i = 0; i < 60; i++) {
  try { if ((await fetch(`${BASE}api/status`)).ok) break; } catch { /* not yet */ }
  await sleep(1000);
}

// The planned steps named, run one after another from the page; every step when none is named.
const runSteps = (ids) => `(async () => {
  const S = window.studio.S, want = ${js(ids)};
  for (const e of S.project.plan) {
    if (want.length && !want.includes(e.id)) continue;
    const tool = S.tools.find((t) => t.ref === e.tool);
    await window.studio.runTool(tool, { ...Object.fromEntries(tool.params.map((q) => [q.name, q.default])), ...(e.params || {}) }, e.slice || null, { step: e.id, title: e.title });
  }
  return true;
})()`;
// A step selected (its note at the right), and one of its tables shown as a table, a chart or the map's layers.
const view = (stepId, kind, table, layers = []) => `(() => {
  const S = window.studio.S, s = S.project.steps.find((x) => x.id === ${js(stepId)});
  if (!s) return false;
  window.studio.select({ kind: 'step', id: s.id });
  if (${js(kind)} === 'table') { S.table = ${js(table)}; window.studio.show('table'); return true; }
  if (${js(kind)} === 'chart') {
    const v = S.project.views.find((x) => x.kind === 'chart' && x.table === ${js(table)});
    if (!v) return false;
    S.chart = v.id; S.figure = null; window.studio.show('chart'); return true;
  }
  const keep = ${js(layers)};
  for (const v of S.project.views) if (v.kind === 'layer') v.on = keep.includes(v.table);
  window.studio.show('map');
  return true;
})()`;

async function project(title, template, ids, shots, port) {
  if (process.env.ONLY && !template.includes(process.env.ONLY)) return;
  const made = await post('api/projects', { title, template });
  const name = made.name;
  assert(`the project begun from the template ${template}`, !!name, js(made).slice(0, 200));
  const page = await open(`${BASE}?project=${name}`, { port, dir: join(tmpdir(), `urban-city-chrome-${port}-${Date.now()}`), width: 1600, height: 960 });
  try {
    await page.until(`window.studio && window.studio.S.project && Array.isArray(window.studio.S.project.steps) && window.studio.S.tools?.length > 0`, 120);
    await sleep(1500);
    await page.eval(runSteps(ids));
    const want = ids.length || (await page.eval(`window.studio.S.project.plan.length`));
    await page.until(`window.studio.S.project.steps.filter((s) => s.ran || s.error).length >= ${want}`, 3000);
    const steps = await page.eval(`window.studio.S.project.steps.map((s) => s.title + ': ' + (s.error || 'ran'))`);
    assert(`${title}: the steps run`, steps.length >= want && steps.every((s) => /ran$/.test(s)), js(steps));
    for (const [what, file] of shots) {
      const ok = await page.eval(what);
      assert(`${title}: the view for ${file} is there`, ok === true);
      await sleep(3500);
      await page.shot(join(OUT, `${file}.jpg`));
    }
    assert(`${title}: the page threw nothing`, page.errors.length === 0, page.errors.join(' | '));
  } catch (error) {
    assert(`${title}: the drive ran to its end`, false, error.stack || String(error));
    try { await page.shot(join(OUT, `${template}-failed.jpg`)); } catch { /* no page */ }
  } finally {
    await page.close();
  }
}

await project('An invented cohort of three cities', 'programmes.cohort', [], [
  [view('s-scorecard', 'chart', 'scorecard_profile'), 'city-programmes-01-scorecard'],
  [view('s-programme', 'table', 'programme_claims'), 'city-programmes-02-claims'],
  [view('s-finance', 'chart', 'finance_years_ahead'), 'city-programmes-03-coverage'],
  [view('s-local-debt', 'chart', 'local_debt_paths'), 'city-programmes-04-france-debt'],
  [view('s-capture', 'table', 'capture_instruments'), 'city-programmes-05-capture'],
  [view('s-capture', 'table', 'capture_replotting'), 'city-programmes-06-replotting'],
], 9411);
await project('A town\'s inventory and its measures', 'materials.a-towns-emissions', [], [
  [view('e-inventory', 'table', 'inventory_cells'), 'city-emissions-01-inventory'],
  [view('e-mitigation', 'chart', 'mitigation_credits'), 'city-emissions-02-credits'],
  [view('e-mitigation', 'chart', 'mitigation_paths'), 'city-emissions-03-pathway'],
], 9412);
await project('A city\'s solid waste', 'materials.a-citys-waste', [], [
  [view('w-account', 'table', 'waste_facilities'), 'city-waste-01-facilities'],
  [view('w-account', 'table', 'waste_account'), 'city-waste-02-account'],
], 9413);
await project('A fragility learnt from an event', 'risk.damage-after-an-event', [], [
  [view('f-learn', 'chart', 'fragility_bands'), 'city-fragility-01-curves'],
  [view('f-learn', 'table', 'fragility_back_test'), 'city-fragility-02-back-test'],
], 9414);
await project('Freetown: what the floods would cost its buildings', 'risk.freetown', ['s-damage'], [
  [view('s-damage', 'map', null, ['damage_cells']), 'city-damage-01-freetown-map'],
  [view('s-damage', 'chart', 'damage_models'), 'city-damage-02-models'],
], 9415);
await project('Households in a flood', 'risk.households-in-a-flood', [], [
  [view('w-wellbeing', 'chart', 'wellbeing_income_shares'), 'city-wellbeing-01-income-groups'],
  [view('w-wellbeing', 'chart', 'wellbeing_measures_avoided'), 'city-wellbeing-02-measures'],
  [view('w-wellbeing', 'table', 'wellbeing_districts'), 'city-wellbeing-03-districts'],
], 9416);
await project('Melbourne: who lives within reach of public transport and of open space', 'transit.melbourne', [], [
  [view('t-access', 'chart', 'transit_readings'), 'city-transit-01-readings'],
  [view('t-access', 'map', null, ['transit_people']), 'city-transit-02-people'],
  [view('t-access', 'map', null, ['transit_stops']), 'city-transit-03-stops'],
  [view('t-openspace', 'chart', 'openspace_readings'), 'city-openspace-01-readings'],
  [view('t-openspace', 'chart', 'openspace_kinds'), 'city-openspace-02-kinds'],
  [view('t-openspace', 'map', null, ['openspace_land']), 'city-openspace-03-land'],
  [view('t-openspace', 'map', null, ['openspace_people']), 'city-openspace-04-people'],
], 9417);
await project('A city\'s homes and who lacks them', 'shelter.a-citys-homes', [], [
  [view('h-shelter', 'chart', 'shelter_readings'), 'city-shelter-01-readings'],
  [view('h-shelter', 'chart', 'shelter_components'), 'city-shelter-02-components'],
  [view('h-shelter', 'chart', 'shelter_districts'), 'city-shelter-03-districts'],
  [view('h-shelter', 'chart', 'shelter_deprivations'), 'city-shelter-04-deprivations'],
], 9418);
await project('Visitors in six European city-regions, 2024', 'visitors.european-cities', [], [
  [view('v-stock', 'chart', 'visitors_months'), 'city-visitors-01-months'],
  [view('v-stock', 'chart', 'visitors_readings'), 'city-visitors-02-readings'],
  [view('v-stock', 'chart', 'visitors_origin'), 'city-visitors-03-origin'],
  [view('v-stock', 'table', 'visitors_measures'), 'city-visitors-04-measures'],
  [view('v-spending', 'chart', 'tourism_where'), 'city-tourism-01-where'],
  [view('v-spending', 'chart', 'tourism_products'), 'city-tourism-02-products'],
  [view('v-city', 'chart', 'tourism_where_region'), 'city-regional-01-where'],
  [view('v-levy', 'chart', 'levy_curve'), 'city-tourism-03-levy'],
  [view('v-shock', 'chart', 'shock_sectors'), 'city-recovery-01-sectors'],
  [view('v-shock', 'chart', 'shock_paths'), 'city-recovery-02-paths'],
  [view('v-two', 'chart', 'shock_sectors_two'), 'city-regional-02-two'],
], 9419);
console.log(`\n${passed} passed, ${failures} failed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
