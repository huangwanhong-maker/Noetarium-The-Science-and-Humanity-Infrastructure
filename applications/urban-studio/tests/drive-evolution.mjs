// Resources of many kinds allocated by evolution (ADR 0137), driven in headless Chrome (not in the suite): Melbourne's areas, the field
// of the allocations kept, one opened on its areas with the thread of grants, which grants commute, the types' laws as seals, the
// search's record; a person's pairwise judgements read as strengths; with --model, a model's judgements beside them (grade C).
//   node applications/urban-studio/tests/drive-evolution.mjs <base> <project made from allocation.melbourne-evolve> <folder for pictures> [--model]
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE, project, folder, flag] = process.argv.slice(2);
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let failed = 0;
const assert = (name, ok, detail = '') => { if (!ok) failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
const q = (sel) => `document.querySelector('#pane-allocation ${sel}')`;
const count = (page, sel) => page.eval(`document.querySelectorAll('#pane-allocation ${sel}').length`);

const page = await open(`${BASE}?project=${project}`, { port: 9399, dir: join(tmpdir(), `urban-evolution-${Date.now()}`), width: 1600, height: 1150 });
try {
  await page.until(`document.querySelector('.tab[data-tab="allocation"]')`, 90);
  await sleep(800);
  await page.eval(`document.querySelector('.tab[data-tab="allocation"]').click()`);
  await page.until(`${q('.al-evo-field svg')}`, 300);
  await sleep(600);
  const stones = await count(page, '.al-evo-field .stone');
  assert('the field of the allocations kept: a stone for each, the first ringed', stones >= 2 && (await count(page, '.al-evo-field .ring')) === 1, `${stones}`);
  assert('the first allocation drawn on Melbourne\'s thirteen areas, with the thread of grants', (await count(page, '.al-evo-allocation .recipient')) === 13 && !!(await page.eval(`!!${q('.al-evo-allocation .thread')}`)));
  assert('every kept allocation is lawful, and the page says what kategoria refused', /found unlawful by kategoria and refused/.test(await page.eval(`${q('.al-notes')}?.innerText || ''`)));
  await page.shot(join(folder, 'evolution-01-field-and-allocation.jpg'));
  // Another allocation opened from the field.
  await page.eval(`document.querySelectorAll('#pane-allocation .al-evo-field .stone')[1]?.dispatchEvent(new MouseEvent('click', { bubbles: true }))`);
  await sleep(800);
  assert('a stone opens its allocation', /Allocation 2:/.test(await page.eval(`${q('.al-two .small')}?.innerText || ''`)));
  await page.eval(`${q('.al-evo-commute')}?.scrollIntoView({ block: 'start' })`);
  await sleep(400);
  assert('which grants commute: every pair a cell, some crossed', (await count(page, '.al-evo-commute .pair')) === 13 * 12 && (await count(page, '.al-evo-commute .cross')) > 0);
  assert('the resource types\' laws as seals, every type lawful', (await count(page, '.al-evo-seals .law')) >= 4 * 5 && /lawful, /.test(await page.eval(`${q('.al-evo-seals')}?.textContent || ''`)));
  await page.shot(join(folder, 'evolution-02-commutation-and-seals.jpg'));
  // A person's judgements.
  const press = async (label) => {
    await page.eval(`[...document.querySelectorAll('#pane-allocation .al-key')].find((b) => b.textContent.trim().startsWith(${JSON.stringify(label)}))?.click()`);
    await sleep(400);
    await page.until(`!document.querySelector('#pane-allocation')?.dataset.busy`, 300);
    await sleep(400);
  };
  for (const k of [0, 1, 2, 3, 4, 5]) await press(k % 3 === 2 ? 'the right serves better' : '← the left serves better');
  await page.eval(`${q('.al-compare')}?.scrollIntoView({ block: 'start' })`);
  await sleep(500);
  assert('a person\'s judgements read as strengths beside the search', (await count(page, '.al-evo-strengths .strength')) >= 2, await page.eval(`${q('.al-compare')}?.innerText.slice(0, 400) || ''`));
  await page.shot(join(folder, 'evolution-03-judgements.jpg'));
  if (flag === '--model') {
    await press('ask a model to compare');
    await page.until(`document.querySelectorAll('#pane-allocation .al-evo-strengths .bar.proposed').length > 0 || !!document.querySelector('#pane-allocation .model-word, #pane-allocation .al-compare .wrong')`, 900);
    await sleep(500);
    const proposed = await count(page, '.al-evo-strengths .bar.proposed');
    const word = await page.eval(`document.querySelector('#pane-allocation .model-word')?.textContent || ''`);
    assert('a model\'s judgements beside the people\'s, of grade C, or its declining said so', proposed >= 2 || /tell no pair apart/.test(word), await page.eval(`${q('.al-compare')}?.innerText.slice(0, 500) || ''`));
    await page.eval(`${q('.al-compare')}?.scrollIntoView({ block: 'start' })`);
    await sleep(400);
    await page.shot(join(folder, 'evolution-04-a-model-beside.jpg'));
  }
  await page.eval(`${q('.al-record')}?.scrollIntoView({ block: 'start' })`);
  await sleep(400);
  assert('the search\'s record, sealed and verified', /verified/.test(await page.eval(`[...document.querySelectorAll('#pane-allocation h3')].map((h) => h.textContent).join(' ')`)) && (await count(page, '.al-record .entry')) >= 8);
  await page.shot(join(folder, 'evolution-05-record.jpg'));
} finally {
  await page.close();
}
console.log(failed ? `${failed} failed` : 'all passed');
process.exit(failed ? 1 : 0);
