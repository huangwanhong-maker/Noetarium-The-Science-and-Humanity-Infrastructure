// The plug-in Cities across scales driven in headless Chrome (not in the suite): its view at each scale, a model drawn with what it
// still needs, and the catalogue opened in place; a picture of each.
//   node applications/urban-studio/tests/drive-scales.mjs <base> <project> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE, project, folder] = process.argv.slice(2);
const page = await open(`${BASE}?project=${project}`, { port: 9399, dir: join(tmpdir(), `urban-scales-${Date.now()}`), width: 1600, height: 1060 });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let failed = 0;
const assert = (name, ok, detail = '') => { if (!ok) failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
const text = () => page.eval(`document.querySelector('#pane-scales')?.innerText || ''`);
const click = (sel, label) => page.eval(`[...document.querySelectorAll('#pane-scales ${sel}')].find((b) => b.textContent.trim() === ${JSON.stringify(label)})?.click()`);
try {
  await page.until(`document.querySelector('.tab[data-tab="scales"]')`, 60);
  await sleep(800);
  await page.eval(`document.querySelector('.tab[data-tab="scales"]').click()`);
  await page.until(`document.querySelector('#pane-scales .ms-unit')`, 60);
  assert('the city\'s blocks are drawn, one outline each', (await page.eval(`document.querySelectorAll('#pane-scales .ms-unit').length`)) === 603);
  await page.shot(join(folder, 'scales-blocks.jpg'));
  await click('.ms-level-b', 'area');
  await sleep(1200);
  assert('read at the area, the blocks carry their area\'s value and the areas are named', (await text()).includes('Docklands') && (await page.eval(`document.querySelectorAll('#pane-scales .ms-group').length`)) === 13);
  await click('.ms-index', 'land-use mix');
  await sleep(1200);
  assert('an index chosen says how it is carried up', (await text()).includes('Carried up the levels as'));
  await page.shot(join(folder, 'scales-areas-mix.jpg'));
  await click('.ms-index', 'dissimilarity of homes and offices');
  await sleep(1200);
  assert('an index among the blocks beneath cannot be read at a block', (await page.eval(`[...document.querySelectorAll('#pane-scales .ms-level-b')].find((b) => b.textContent === 'block')?.disabled`)) === true);
  await click('.ms-mode', 'Models');
  await page.until(`document.querySelector('#pane-scales .ms-diagram')`, 60);
  assert('a model is drawn as its levels', (await page.eval(`document.querySelectorAll('#pane-scales .ms-band').length`)) === 3);
  await page.shot(join(folder, 'scales-model-growth.jpg'));
  await click('.ms-model', 'The vacancy cycle');
  await page.until(`document.querySelector('#pane-scales .ms-need')`, 60);
  assert('the vacancy cycle rings the rents no record holds, to be declared', (await text()).includes('rho at each area (13 of 13)'));
  await page.shot(join(folder, 'scales-model-vacancy.jpg'));
  await click('.ms-mode', 'The catalogue');
  await page.until(`document.querySelector('#pane-scales .ms-row')`, 60);
  assert('the catalogue lists its indexes with their counts', (await text()).includes('1,536 indexes'));
  await page.eval(`(() => { const i = document.querySelector('#pane-scales .ms-search'); i.value = 'trust'; i.dispatchEvent(new Event('input')); })()`);
  await sleep(900);
  await page.eval(`document.querySelector('#pane-scales .ms-row-b')?.click()`);
  await sleep(600);
  assert('an index opens in place with its definition and source', (await text()).includes('Source:'));
  await page.shot(join(folder, 'scales-catalogue.jpg'));
} finally {
  await page.close();
}
console.log(failed ? `${failed} failed` : 'all passed');
process.exit(failed ? 1 : 0);
