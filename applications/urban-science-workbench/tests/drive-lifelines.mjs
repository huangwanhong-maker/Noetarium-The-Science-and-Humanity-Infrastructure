// The plug-in Lifelines and evacuation driven in headless Chrome (not in the suite): Freetown's roads ranked under its floods, the
// tool on financing its losses, and Miyako's people walking out of the assumed tsunami's reach; a picture of each.
//   node applications/urban-science-workbench/tests/drive-lifelines.mjs <base> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE = 'http://127.0.0.1:8970/', folder = '.'] = process.argv.slice(2);
const page = await open(BASE, { port: 9411, dir: join(tmpdir(), `wb-lifelines-${Date.now()}`), width: 1600, height: 1000 });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let failed = 0;
const assert = (name, ok, detail = '') => { if (!ok) failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
const legend = () => page.eval(`document.getElementById('legend')?.innerText || ''`);
const W = 'window.workbench';
async function area(id) {
  await page.eval(`(async () => { const a = ${W}.state.areas.find((x) => x.id === ${JSON.stringify(id)}); await ${W}.openArea(a, { view: 'jump' }); })()`);
  await page.until(`${W}.state.area?.id === ${JSON.stringify(id)} && !${W}.state.ui.opening`, 120);
  // Wait for the world's ways about the view.
  await page.until(`${W}.state.world.lines().length > 200`, 180);
  await sleep(6000);
}
async function layer(id, on = true) {
  await page.eval(`(async () => { const p = ${W}.state.plugins.find((x) => x.name === 'lifelines'); const l = p.layers.find((x) => x.id === ${JSON.stringify(id)}); await ${W}.toggleLayer(p, l, ${on}); })()`);
  if (on) await page.until(`${W}.state.layers.get('lifelines/${id}')?.handle`, 120);
}
try {
  await page.until(`window.workbench && window.workbench.state.plugins?.length > 0`, 120);
  // ---- Freetown: the roads under its floods -----------------------------------------------------------------------------------
  if (process.env.ONLY !== 'miyako') {
  await area('freetown');
  await layer('criticality');
  await page.eval(`${W}.state.layers.get('lifelines/criticality').handle.pickAt(8.4757, -13.2332)`);
  await page.until(`document.getElementById('legend').innerText.includes('expected loss')`, 900);
  const t = await legend();
  assert('Freetown: the road links are ranked under its six river floods', t.includes('100-year') && t.includes('over the year\'s largest flood'), t.slice(0, 400));
  await sleep(2500);
  await page.shot(join(folder, 'risk-lifelines-01-freetown-roads.jpg'));
  // ---- Freetown: financing the losses -----------------------------------------------------------------------------------------
  await page.eval(`(async () => { const p = ${W}.state.plugins.find((x) => x.name === 'lifelines'); await ${W}.openTool(p, p.tools[0]); })()`);
  await page.until(`document.body.innerText.includes('The cheapest tower')`, 300);
  assert('Freetown: strategies of financing compared, the cheapest tower found, a parametric cover refused', (await page.eval(`document.body.innerText`)).includes('none is invented'));
  await sleep(1200);
  await page.shot(join(folder, 'risk-lifelines-02-financing.jpg'));
  await page.eval(`${W}.closePanel && ${W}.closePanel()`);
  await layer('criticality', false);
  }
  // ---- Miyako: walking out of the tsunami's reach -----------------------------------------------------------------------------
  await area('miyako');
  await layer('evacuation');
  await page.eval(`[...document.querySelectorAll('#legend button')].find((b) => b.textContent.trim() === 'evacuate')?.click()`);
  await page.until(`document.getElementById('legend').innerText.includes('safe by then in the simulation')`, 1800);
  const m = await legend();
  assert('Miyako: the people of the tsunami zone walk out, bounded and simulated', m.includes('who can walk to safety within') && m.includes('at most safe by then'), m.slice(0, 500));
  await sleep(11000);  // the crowd played to its end
  await page.shot(join(folder, 'risk-lifelines-03-miyako-evacuation.jpg'));
  // Ten minutes after the warning, framed closer on the centre by the bay, where the crowd moves.
  await page.eval(`${W}.state.gis.fit({ x0: -900, y0: -800, x1: 900, y1: 800 }, { tilt: 48, heading: 350, range: 0.9, seconds: 0.1 })`);
  await sleep(6000);
  await page.eval(`${W}.state.layers.get('lifelines/evacuation').handle.setTime({ hour: ${W}.state.clock.hour + 10 / 60 })`);
  await sleep(1500);
  await page.shot(join(folder, 'risk-lifelines-04-miyako-ten-minutes.jpg'));
  assert('no exception on the page', page.errors.length === 0, page.errors.join(' | '));
} catch (e) {
  failed++;
  console.log(`FAIL  ${e.message}`);
  try { await page.shot(join(folder, 'risk-lifelines-failure.jpg')); } catch { /* gone */ }
} finally {
  await page.close();
}
console.log(failed ? `${failed} failed` : 'all passed');
process.exit(failed ? 1 : 0);
