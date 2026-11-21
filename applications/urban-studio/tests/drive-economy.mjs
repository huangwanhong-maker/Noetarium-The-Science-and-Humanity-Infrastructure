// A care fund allocated in an economy of places (ADR 0141), driven in headless Chrome (not in the suite): Melbourne's areas, the
// economy's habits calibrated to records and what they leave unidentified, the field of the allocations kept by the money re-spent in
// their areas, one opened on its areas with the thread of grants, the value to the people in need beside it, and which grants commute.
//   node applications/urban-studio/tests/drive-economy.mjs <base> <project made from allocation.melbourne-economy> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE, project, folder] = process.argv.slice(2);
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let failed = 0;
const assert = (name, ok, detail = '') => { if (!ok) failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
const q = (sel) => `document.querySelector('#pane-allocation ${sel}')`;
const count = (page, sel) => page.eval(`document.querySelectorAll('#pane-allocation ${sel}').length`);

const page = await open(`${BASE}?project=${project}`, { port: 9398, dir: join(tmpdir(), `urban-economy-${Date.now()}`), width: 1600, height: 1150 });
try {
  await page.until(`document.querySelector('.tab[data-tab="allocation"]')`, 90);
  await sleep(800);
  await page.eval(`document.querySelector('.tab[data-tab="allocation"]').click()`);
  await page.until(`${q('.al-evo-field svg')}`, 900);
  await sleep(600);
  const notes = await page.eval(`[...document.querySelectorAll('#pane-allocation .al-notes')].map((n) => n.innerText).join(' | ')`);
  assert('the economy\'s habits, calibrated: read from the accounts, the share spent fitted to a velocity, what is unidentified said',
    /calibrated/.test(notes) && /matched in \d+ runs to a velocity/.test(notes) && /Not identified by the records/.test(notes), notes.slice(0, 400));
  const words = await page.eval(`${q('.al-two .small')}?.innerText || ''`);
  assert('the first allocation read by the money re-spent in its areas and, apart, by the value to the people in need',
    /re-spent in their own areas/.test(words) && /value to the people in need/.test(words) && /LM3 of the grants/.test(words), words);
  assert('the first allocation drawn on Melbourne\'s thirteen areas, with the thread of grants', (await count(page, '.al-evo-allocation .recipient')) === 13 && !!(await page.eval(`!!${q('.al-evo-allocation .thread')}`)));
  await page.shot(join(folder, 'allocation-economy-01-field-and-allocation.jpg'));
  await page.eval(`${q('.al-evo-commute')}?.scrollIntoView({ block: 'start' })`);
  await sleep(400);
  assert('which grants commute in the economy: every pair a cell, some crossed', (await count(page, '.al-evo-commute .pair')) === 13 * 12 && (await count(page, '.al-evo-commute .cross')) > 0);
  await page.shot(join(folder, 'allocation-economy-02-commutation.jpg'));
} catch (e) {
  assert('the page answered', false, String(e?.message || e));
} finally {
  await page.close();
}
console.log(failed ? `${failed} failed` : 'all passed');
process.exit(failed ? 1 : 0);
