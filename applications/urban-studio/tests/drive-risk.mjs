// The plug-in Lifelines and evacuation driven as an analyst would use it, in headless Chrome (not in the suite): a project begun from
// the template of Miyako, its plan run from the page (the tsunami walked out of three ways, the roads under the largest flood), the
// map and a chart; then Freetown's template, its plan run, the roads placed on the map, the losses financed, the walk out of the
// flood's zone charted. A picture of each.
//   node applications/urban-studio/tests/drive-risk.mjs <base> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { mkdirSync } from 'node:fs';

const [BASE = 'http://127.0.0.1:9090/', OUT = join(tmpdir(), 'urban-risk-drive')] = process.argv.slice(2);
mkdirSync(OUT, { recursive: true });
const origin = BASE.replace(/\/$/, '');
const post = async (path, body) => (await fetch(`${BASE}${path}`, { method: 'POST', headers: { 'Content-Type': 'application/json', Origin: origin }, body: JSON.stringify(body) })).json();
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let passed = 0, failures = 0;
const assert = (name, ok, detail = '') => { ok ? passed++ : failures++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
for (let i = 0; i < 60; i++) {
  try { if ((await fetch(`${BASE}api/status`)).ok) break; } catch { /* not yet */ }
  await sleep(1000);
}

async function project(title, template, shots) {
  if (process.env.ONLY && !template.includes(process.env.ONLY)) return;
  const made = await post('api/projects', { title, template });
  const name = made.name;
  assert(`the project begun from the template ${template}`, !!name, JSON.stringify(made).slice(0, 200));
  const page = await open(`${BASE}?project=${name}`, { port: 9398, dir: join(tmpdir(), `urban-risk-chrome-${Date.now()}`), width: 1560, height: 1000 });
  try {
    await page.until(`window.studio && window.studio.S.project && Array.isArray(window.studio.S.project.steps) && window.studio.S.tools?.length > 0`, 120);
    await sleep(1500);
    await page.eval(`window.studio.runPlan()`);
    const n = await page.eval(`window.studio.S.project.plan.length`);
    await page.until(`window.studio.S.project.steps.filter((s) => s.ran || s.error).length >= ${n}`, 3000);
    const steps = await page.eval(`window.studio.S.project.steps.map((s) => s.title + ': ' + (s.error || 'ran'))`);
    assert(`${title}: the template's plan runs`, steps.length >= n && steps.every((s) => /ran$/.test(s)), JSON.stringify(steps));
    for (const [what, file] of shots) {
      await page.eval(what);
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

// A step chosen, and a view of it: the map with only the layers of some tables on, its chart, or its table.
const step = (title, view, tables = []) => `(() => {
  const S = window.studio.S, s = S.project.steps.find((x) => x.title === ${JSON.stringify(title)});
  if (!s) return false;
  window.studio.select({ kind: 'step', id: s.id });
  const keep = ${JSON.stringify(tables)};
  if (keep.length) for (const v of S.project.views) if (v.kind === 'layer') v.on = keep.includes(v.table);
  window.studio.show(${JSON.stringify(view)});
  return true;
})()`;
await project('Miyako: walking out of the tsunami', 'risk.miyako', [
  [`window.studio.show('map')`, 'risk-studio-01-miyako-map'],
  [step('Walking out at 1.0 m/s, leaving as in 2011', 'chart'), 'risk-studio-02-miyako-curves'],
]);
await project('Freetown: roads, floods and losses', 'risk.freetown', [
  [step('Which roads matter most to reaching care', 'map', ['roads']), 'risk-studio-03-freetown-roads'],
  [step('How the city might meet its flood losses', 'table'), 'risk-studio-04-freetown-financing'],
  [step('Walking out of the 1-in-100-year river flood, leaving as in 2011', 'chart'), 'risk-studio-05-freetown-curves'],
]);
console.log(`\n${passed} passed, ${failures} failed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
