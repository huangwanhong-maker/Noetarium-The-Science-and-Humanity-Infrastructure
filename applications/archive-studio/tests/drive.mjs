// The Archive Studio's page driven as a person would use it, in headless Chrome (not in the suite): a project made and the example
// archive laid out; the wall of holdings; the plan opened in its reading room with its annotated region; the minutes with their
// passage; the record's fixity verified in the page; the graph with a node opened; a SPARQL query whose bindings are marked; the
// shapes' failures; entailment drawn dashed; the text of a file refused at its line; the catalogue. Pictures go to the folder given.
//
//   node applications/archive-studio/tests/drive.mjs [http://127.0.0.1:9150/] [folder for pictures]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const BASE = process.argv[2] || 'http://127.0.0.1:9150/';
const OUT = process.argv[3] || join(tmpdir(), 'archive-studio-drive');
mkdirSync(OUT, { recursive: true });
let failures = 0, passed = 0;
const assert = (what, ok, detail = '') => { console.log(`${ok ? 'ok  ' : 'FAIL'}  ${what}${detail ? `: ${detail}` : ''}`); ok ? passed++ : failures++; };

const make = await fetch(`${BASE}api/projects`, { method: 'POST', headers: { 'Content-Type': 'application/json', Origin: BASE.replace(/\/$/, '') }, body: JSON.stringify({ title: 'Papers of the reading room' }) });
const { name } = await make.json();
await fetch(`${BASE}api/projects/${name}/example`, { method: 'POST', headers: { 'Content-Type': 'application/json', Origin: BASE.replace(/\/$/, '') }, body: '{}' });

const page = await open(`${BASE}?project=${name}`, { port: 9361, dir: join(tmpdir(), `archive-studio-chrome-${Date.now()}`), width: 1500, height: 940 });
try {
  await page.until(`document.querySelectorAll('.holding').length >= 4`, 40);
  assert('the archive opens first, with its four holdings on the wall', await page.eval(`document.querySelector('.tab.on')?.dataset.tab === 'archive' && document.querySelectorAll('.holding').length === 4`));
  await page.wait(1500);
  await page.shot(join(OUT, 'archive-01-wall.jpg'));
  // The plan, in its reading room.
  await page.eval(`[...document.querySelectorAll('.holding')].find((b) => b.title.startsWith('Plan')).click()`);
  await page.until(`document.querySelector('.room .viewer.image img')?.complete && document.querySelectorAll('.marks-layer rect').length > 0`, 30);
  assert('the plan opens in its reading room, its annotated region drawn on it', true);
  await page.eval(`[...document.querySelectorAll('.record button')].find((b) => b.textContent.startsWith('Verify')).click()`);
  await page.until(`document.querySelector('.fixity .verdict')`, 20);
  assert('the bytes, digested again in the page, are the ones kept', await page.eval(`document.querySelector('.fixity .verdict').classList.contains('holds')`));
  await page.shot(join(OUT, 'archive-02-plan.jpg'));
  // The minutes, with their annotated passage.
  await page.eval(`[...document.querySelectorAll('.ring button')].find((b) => b.textContent.startsWith('Back')).click()`);
  await page.until(`document.querySelectorAll('.holding').length === 4`);
  await page.eval(`[...document.querySelectorAll('.holding')].find((b) => b.title.startsWith('Minutes')).click()`);
  await page.until(`document.querySelector('.viewer .page mark')`, 20);
  assert('the minutes open with their annotated passage marked', await page.eval(`document.querySelector('.viewer .page mark').textContent.includes('parish registers')`));
  await page.eval(`[...document.querySelectorAll('.ring button')].find((b) => b.textContent === 'Describe').click()`);
  await page.until(`!document.querySelector('.record .field').disabled`);
  await page.shot(join(OUT, 'archive-03-minutes.jpg'));
  // The sound.
  await page.eval(`[...document.querySelectorAll('.ring button')].find((b) => b.textContent.startsWith('Back')).click()`);
  await page.until(`document.querySelectorAll('.holding').length === 4`);
  await page.eval(`[...document.querySelectorAll('.holding')].find((b) => b.title.startsWith('A sounding')).click()`);
  await page.until(`document.querySelector('.viewer .wave') && document.querySelector('.timeline .segment')`, 30);
  assert('the sound opens with the shape of its waves and its annotated span on the timeline', true);
  await page.shot(join(OUT, 'archive-04-sound.jpg'));
  // The graph.
  await page.eval(`document.querySelector('.tab[data-tab="graph"]').click()`);
  await page.until(`document.querySelectorAll('.l-node').length > 10`, 30);
  assert('the graph draws the project\'s linked data', true, `${await page.eval(`document.querySelectorAll('.l-node').length`)} nodes`);
  await page.eval(`[...document.querySelectorAll('.l-node')].find((g) => g.textContent.includes('Papers of the reading room'))?.dispatchEvent(new MouseEvent('click', { bubbles: true }))`);
  await page.until(`document.querySelector('.node-card')`, 20);
  assert('a node opens where it stands, with what is said of it', await page.eval(`document.querySelector('.node-card .said li') !== null`));
  await page.shot(join(OUT, 'archive-05-graph.jpg'));
  await page.eval(`document.querySelector('.node-card .x').click()`);
  await page.until(`!document.querySelector('.node-card')`);
  await page.eval(`[...document.querySelectorAll('.files .key')].find((b) => b.textContent === 'Query').click()`);
  await page.until(`document.querySelector('.leaf.query')`);
  await page.eval(`[...document.querySelectorAll('.leaf.query .key')].find((b) => b.textContent === 'Run').click()`);
  await page.until(`document.querySelector('.answers tr td')`, 30);
  assert('a SPARQL query answers in a table, and its bindings are marked on the graph', await page.eval(`document.querySelectorAll('.l-node.marked').length > 0`));
  await page.shot(join(OUT, 'archive-06-query.jpg'));
  await page.eval(`[...document.querySelectorAll('.files .key')].find((b) => b.textContent === 'Shapes').click()`);
  await page.until(`[...document.querySelectorAll('.leaf .key')].some((b) => b.textContent === 'Check')`);
  await page.eval(`[...document.querySelectorAll('.leaf .key')].find((b) => b.textContent === 'Check').click()`);
  await page.until(`document.querySelectorAll('.answers tr').length >= 3`, 30);
  assert('the shapes find the two holdings that fail them', await page.eval(`document.querySelectorAll('.answers tr').length === 3`));
  await page.shot(join(OUT, 'archive-07-shapes.jpg'));
  await page.eval(`[...document.querySelectorAll('.files .key')].find((b) => b.textContent === 'Entail').click()`);
  await page.until(`[...document.querySelectorAll('.leaf .key')].some((b) => b.textContent === 'Entail')`);
  await page.eval(`[...document.querySelectorAll('.leaf .key')].find((b) => b.textContent === 'Entail').click()`);
  await page.until(`document.querySelectorAll('.l-edge.inferred').length > 0 || document.querySelectorAll('.answers tr').length > 1`, 30);
  assert('entailment lists its conclusions with their rules', await page.eval(`document.querySelectorAll('.answers tr').length > 1`));
  await page.eval(`[...document.querySelectorAll('.files .key')].find((b) => b.textContent === 'Text').click()`);
  await page.until(`document.querySelector('.leaf.text .code-box .code')`);
  await page.eval(`(() => { const a = document.querySelector('.leaf.text .code-box .code'); a.value = a.value + '\\nex:broken ex:p nope:x .'; a.dispatchEvent(new Event('input')); })()`);
  await page.eval(`[...document.querySelectorAll('.leaf .key')].find((b) => b.textContent === 'Keep').click()`);
  await page.until(`document.querySelector('.leaf.text .wrong')`, 20);
  assert('a file that does not read is refused, with its line and column', await page.eval(`/line \\d+, column \\d+/i.test(document.querySelector('.leaf.text .wrong').textContent)`));
  await page.shot(join(OUT, 'archive-08-text.jpg'));
  // The catalogue.
  await page.eval(`document.querySelector('.tab[data-tab="catalogue"]').click()`);
  await page.until(`document.querySelector('.slip') || document.querySelector('.pane.catalogue .note')`, 30);
  assert('the catalogue shows studies, or says how to harvest them', true, await page.eval(`document.querySelectorAll('.slip').length + ' slips'`));
  await page.shot(join(OUT, 'archive-09-catalogue.jpg'));
  // By night.
  await page.eval(`document.querySelector('.tab[data-tab="archive"]').click()`);
  await page.until(`document.querySelector('.room') || document.querySelectorAll('.holding').length === 4`);
  assert('returning to the archive finds the reading room it was left in', await page.eval(`document.querySelector('.room') !== null`));
  await page.eval(`[...document.querySelectorAll('.ring button')].find((b) => b.textContent.startsWith('Back')).click()`);
  await page.until(`document.querySelectorAll('.holding').length === 4`);
  await page.eval(`document.getElementById('theme').click()`);
  await page.wait(800);
  await page.shot(join(OUT, 'archive-10-night.jpg'));
  assert('one view at a time: every other pane is hidden from sight', await page.eval(`[...document.querySelectorAll('#stage .pane')].filter((p) => getComputedStyle(p).display !== 'none').length === 1`));
  assert('no error was thrown on the page', page.errors.length === 0, page.errors.join(' | '));
} catch (error) {
  failures++;
  console.log(`FAIL  ${error.message}`);
  await page.shot(join(OUT, 'failure.jpg')).catch(() => {});
} finally {
  await page.close();
}
console.log(`\n${passed} of ${passed + failures} steps passed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
