// Closer pictures of the plug-in Lifelines and evacuation (not in the suite): Freetown's road links of greatest risk under its floods,
// and of greatest criticality, each framed by the camera.
//   node applications/urban-science-workbench/tests/drive-lifelines-close.mjs <base> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE = 'http://127.0.0.1:8970/', folder = '.'] = process.argv.slice(2);
const page = await open(BASE, { port: 9413, dir: join(tmpdir(), `wb-lifelines-close-${Date.now()}`), width: 1600, height: 1000 });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let failed = 0;
const assert = (name, ok, detail = '') => { if (!ok) failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
const W = 'window.workbench';
// Frame the links whose value (risk or criticality) is greatest.
const frameTop = (key, n) => page.eval(`(() => {
  for (const [, l] of ${W}.state.gis.layers) {
    if (l.kind !== 'ribbons' || !l.features?.length || l.features[0].${key} === undefined) continue;
    const top = [...l.features].sort((a, b) => b.${key} - a.${key}).slice(0, ${n});
    let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
    for (const f of top) for (const [x, y] of f.line) { x0 = Math.min(x0, x); y0 = Math.min(y0, y); x1 = Math.max(x1, x); y1 = Math.max(y1, y); }
    const pad = 300;
    ${W}.state.gis.fit({ x0: x0 - pad, y0: y0 - pad, x1: x1 + pad, y1: y1 + pad }, { tilt: 50, heading: 15, range: 0.9, seconds: 0.1 });
    return top.length;
  }
  return 0;
})()`);
try {
  await page.until(`window.workbench && window.workbench.state.plugins?.length > 0`, 120);
  await page.eval(`(async () => { const a = ${W}.state.areas.find((x) => x.id === 'freetown'); await ${W}.openArea(a, { view: 'jump' }); })()`);
  await page.until(`${W}.state.area?.id === 'freetown' && !${W}.state.ui.opening`, 120);
  await page.until(`${W}.state.world.lines().length > 200`, 180);
  await sleep(6000);
  await page.eval(`(async () => { const p = ${W}.state.plugins.find((x) => x.name === 'lifelines'); await ${W}.toggleLayer(p, p.layers.find((x) => x.id === 'criticality'), true); })()`);
  await page.until(`${W}.state.layers.get('lifelines/criticality')?.handle`, 120);
  await page.eval(`${W}.state.layers.get('lifelines/criticality').handle.pickAt(8.4757, -13.2332)`);
  await page.until(`document.getElementById('legend').innerText.includes('expected loss')`, 900);
  assert('the links of greatest risk are framed', (await frameTop('risk', 1)) > 0);
  await sleep(9000);
  await page.shot(join(folder, 'risk-lifelines-05-freetown-risk-close.jpg'));
  await page.eval(`[...document.querySelectorAll('#legend button')].find((b) => b.textContent.trim() === 'what losing it adds')?.click()`);
  await sleep(1500);
  assert('the links of greatest criticality are framed', (await frameTop('crit', 1)) > 0);
  await sleep(9000);
  await page.shot(join(folder, 'risk-lifelines-06-freetown-criticality-close.jpg'));
  assert('no exception on the page', page.errors.length === 0, page.errors.join(' | '));
} catch (e) {
  failed++;
  console.log(`FAIL  ${e.message}`);
} finally {
  await page.close();
}
console.log(failed ? `${failed} failed` : 'all passed');
process.exit(failed ? 1 : 0);
