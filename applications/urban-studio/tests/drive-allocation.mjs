// The plug-in Allocation and justice driven in headless Chrome (not in the suite): England's fund as a channel, a district opened in
// place, the rounds it could have been, its record with an appeal filed and upheld; Melbourne's pivot; Warsaw's ballots by
// neighbourhood; a picture of each.
//   node applications/urban-studio/tests/drive-allocation.mjs <base> <england project> <melbourne project> <warsaw project> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE, england, melbourne, warsaw, folder] = process.argv.slice(2);
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let failed = 0;
const assert = (name, ok, detail = '') => { if (!ok) failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };

async function tab(page) {
  await page.until(`document.querySelector('.tab[data-tab="allocation"]')`, 90);
  await sleep(800);
  await page.eval(`document.querySelector('.tab[data-tab="allocation"]').click()`);
}
const text = (page) => page.eval(`document.querySelector('#pane-allocation')?.innerText || ''`);

let page = await open(`${BASE}?project=${england}`, { port: 9397, dir: join(tmpdir(), `urban-allocation-${Date.now()}`), width: 1600, height: 1100 });
try {
  await tab(page);
  await page.until(`document.querySelector('#pane-allocation .al-channel svg')`, 180);
  await sleep(600);
  assert('England\'s fund is drawn as a channel with its two sluices', (await page.eval(`document.querySelectorAll('#pane-allocation .sluice').length`)) === 2);
  assert('the plots stand in the order reached, Birmingham first', (await page.eval(`document.querySelector('#pane-allocation .plot')?.dataset.entity`)) === 'E08000025');
  assert('the rounds it could have been are ordered, the declared one undominated', (await page.eval(`!!document.querySelector('#pane-allocation .option.undominated.declared')`)));
  await page.shot(join(folder, 'allocation-01-channel.jpg'));
  const chosen = await page.eval(`[...document.querySelectorAll('#pane-allocation .plot:not(.gathered)')][4]?.dataset.entity`);
  await page.eval(`document.querySelector('#pane-allocation .plot[data-entity="${chosen}"]')?.dispatchEvent(new MouseEvent('click', { bubbles: true }))`);
  await page.until(`document.querySelector('#pane-allocation .al-card')`, 60);
  assert('a district opens in place with its reasons and its stake', /receives/.test(await text(page)) && /Among the undominated options/.test(await text(page)), (await text(page)).slice(0, 300));
  await page.eval(`document.querySelector('#pane-allocation .al-card').scrollIntoView({ block: 'start' })`);
  await sleep(300);
  await page.shot(join(folder, 'allocation-02-district.jpg'));
  await page.eval(`document.querySelector('#pane-allocation .al-order').scrollIntoView({ block: 'start' })`);
  await sleep(300);
  await page.shot(join(folder, 'allocation-03-order.jpg'));
  // The record: published, consulted, applied; an appeal filed from the card and upheld; applied again.
  const settled = async () => { await sleep(300); await page.until(`!document.querySelector('#pane-allocation')?.dataset.busy`, 120); await sleep(300); };
  const press = async (label) => { await page.eval(`[...document.querySelectorAll('#pane-allocation .al-key')].find((b) => b.textContent.trim().startsWith(${JSON.stringify(label)}))?.click()`); await settled(); };
  await press('§ publish');
  for (let k = 0; k < 4; k++) await press('◎ consult');
  await press('→ apply');
  await page.eval(`document.querySelector('#pane-allocation .plot[data-entity="${chosen}"]')?.dispatchEvent(new MouseEvent('click', { bubbles: true }))`);
  await page.until(`document.querySelector('#pane-allocation .al-appeal')`, 60);
  await page.eval(`(() => { const f = document.querySelector('#pane-allocation .al-appeal'); f.elements.key.value = 'need_claim'; f.elements.value.value = '4000000'; f.elements.reason.value = 'an invented appeal, to show the record'; f.elements.who.value = 'the district\\'s clerk'; f.requestSubmit(); })()`);
  await settled();
  await press('uphold');
  await press('→ apply');
  await page.until(`document.querySelector('#pane-allocation .al-record svg')`, 90);
  const rules = await page.eval(`[...document.querySelectorAll('#pane-allocation .rule')].map((g) => g.dataset.rule + ':' + g.getAttribute('class').split(' ')[1]).join(' ')`);
  assert('the record holds the appeal heard for everyone, and correctability holds after the revision is applied', /correctability:holds/.test(rules) && /representativeness:holds/.test(rules) && /heard for everyone/.test(await text(page)), rules);
  await page.eval(`document.querySelector('#pane-allocation .al-procedure').scrollIntoView({ block: 'start' })`);
  await sleep(300);
  await page.shot(join(folder, 'allocation-04-record.jpg'));
} finally {
  await page.close();
}

page = await open(`${BASE}?project=${melbourne}`, { port: 9398, dir: join(tmpdir(), `urban-allocation-m-${Date.now()}`), width: 1600, height: 1100 });
try {
  await tab(page);
  await page.until(`document.querySelector('#pane-allocation .al-channel svg')`, 180);
  await sleep(600);
  assert('Melbourne\'s two areas with no one stand behind a closed gate', (await page.eval(`document.querySelectorAll('#pane-allocation .plot.excluded').length`)) === 2);
  assert('the pivot says the choice turns on which principle comes first', /turns on which of these principles/.test(await text(page)));
  await page.shot(join(folder, 'allocation-05-melbourne.jpg'));
} finally {
  await page.close();
}

page = await open(`${BASE}?project=${warsaw}`, { port: 9396, dir: join(tmpdir(), `urban-allocation-w-${Date.now()}`), width: 1600, height: 1100 });
try {
  await tab(page);
  await page.until(`document.querySelector('#pane-allocation .al-budget svg')`, 300);
  await sleep(600);
  assert('Warsaw\'s ballots are read by neighbourhood under every rule', (await page.eval(`document.querySelectorAll('#pane-allocation .al-budget .mark').length`)) === 18 * 4);
  await page.shot(join(folder, 'allocation-06-warsaw.jpg'));
} finally {
  await page.close();
}
console.log(failed ? `${failed} failed` : 'all passed');
process.exit(failed ? 1 : 0);
