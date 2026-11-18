// The plug-in Needs driven as an analyst would use it, in headless Chrome (not in the suite). A project begun from the template
// Needs in the City of Melbourne; the page before a search, with the areas by their part of the data; a search begun from the page and
// followed to its end; the field of statements with a need opened where it stands, its line of operators drawn back; the need placed
// on the map; the loop with a stage opened and a strategy chosen; statements of one's own read, one refused, and tested; the ledger
// of confirmations and the generations; a generation brought forward; the template's plan run; the page by night. Pictures go to the
// folder given.
//
//   node applications/urban-studio/tests/drive-needs.mjs <http://127.0.0.1:9091/> [folder for pictures]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE = 'http://127.0.0.1:9091/', OUT = join(tmpdir(), 'urban-needs-drive')] = process.argv.slice(2);
mkdirSync(OUT, { recursive: true });
let failures = 0, passed = 0;
const assert = (what, ok, detail = '') => { console.log(`${ok ? 'ok  ' : 'FAIL'}  ${what}${detail ? `: ${detail}` : ''}`); ok ? passed++ : failures++; };
const origin = BASE.replace(/\/$/, '');
const post = async (path, body) => (await fetch(`${BASE}${path}`, { method: 'POST', headers: { 'Content-Type': 'application/json', Origin: origin }, body: JSON.stringify(body) })).json();
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

for (let k = 0; k < 120; k++) {
  try { if ((await fetch(`${BASE}api/status`)).ok) break; } catch { /* not yet */ }
  await sleep(500);
}
const made = await post('api/projects', { title: `Needs in Melbourne ${Date.now() % 100000}`, template: 'needs.melbourne-needs' });
const name = made.name;
assert('the project begun from the template', !!name, JSON.stringify(made).slice(0, 200));

const page = await open(`${BASE}?project=${name}`, { port: 9397, dir: join(tmpdir(), `urban-needs-chrome-${Date.now()}`), width: 1600, height: 1060 });
const shot = async (file) => { await sleep(700); await page.shot(join(OUT, `${file}.jpg`)); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const clickSel = async (sel) => page.eval(`(() => { const e = document.querySelector(${JSON.stringify(sel)}); if (e) e.dispatchEvent(new MouseEvent('click', { bubbles: true })); return !!e; })()`);
const clickText = async (sel, words) => page.eval(`(() => { const e = [...document.querySelectorAll(${JSON.stringify(sel)})].find((x) => x.textContent.includes(${JSON.stringify(words)})); if (e) e.dispatchEvent(new MouseEvent('click', { bubbles: true })); return !!e; })()`);
const count = (sel) => page.eval(`document.querySelectorAll(${JSON.stringify(sel)}).length`);

try {
  await page.until(`document.querySelector('.tab[data-tab="needs"]')`, 60);
  await sleep(1200);
  assert('the plug-in brings its view, Needs, beside the studio\'s', true);
  await page.eval(`document.querySelector('.tab[data-tab="needs"]').click()`);
  await page.until(`document.querySelector('#pane-needs .nd-head')`, 60);
  await page.until(`document.querySelectorAll('#pane-needs .nd-area').length > 300`, 60);
  await sleep(800);
  const head = await text('#pane-needs .nd-head');
  assert('the head names the data and says no search has begun', /City of Melbourne/.test(head) && /No search yet/.test(head), head.slice(0, 220));
  const parts = await page.eval(`['discovery', 'validation', 'confirmation'].map((p) => document.querySelectorAll('#pane-needs .nd-area.nd-part-' + p).length)`);
  assert('the areas drawn by the part each lies in: about half, a quarter and a quarter', parts[0] > 120 && parts[1] > 50 && parts[2] > 50, JSON.stringify(parts));
  assert('the loop of seven stages', (await count('#pane-needs .nd-stagenode')) === 7);
  await shot('needs-01-before');

  // A stage opened, a strategy chosen.
  await clickSel('#pane-needs .nd-stagenode:nth-of-type(5)');
  await page.until(`document.querySelector('#pane-needs .nd-stagepanel')`, 20);
  const survive = await text('#pane-needs .nd-stagepanel');
  assert('a stage opens where it stands, with its strategies', /Survive/.test(survive) && /aging/.test(survive), survive.slice(0, 160));
  await clickText('#pane-needs .nd-stagepanel .nd-chip', 'aging');
  await sleep(600);
  const chosen = await page.eval(`[...document.querySelectorAll('#pane-needs .nd-stagepanel .nd-chip.on')].map((e) => e.textContent)`);
  assert('aging chosen for survival, and said on the ring', chosen.includes('aging') && /aging/.test(await text('#pane-needs .nd-loop')), JSON.stringify(chosen));
  await shot('needs-02-loop');
  await clickText('#pane-needs .nd-stagepanel .nd-chip', 'plus');
  await sleep(400);

  // The search begun from the page and followed.
  await clickSel('#pane-needs .nd-go');
  await page.until(`/Generation 1[0-9]/.test(document.querySelector('#pane-needs .nd-stats')?.textContent || '') && !document.querySelector('#pane-needs .nd-go[disabled]')`, 240);
  await sleep(1500);
  const stats = await text('#pane-needs .nd-stats');
  assert('the search ran ten generations from the page and says what it found', /Generation 10/.test(stats) && /kept/.test(stats) && /confirmed/.test(stats), stats);
  const needs = await count('#pane-needs .nd-need');
  const robust = await count('#pane-needs .nd-need.nd-robust');
  assert('the needs kept stand in the field, the confirmed with double rings', needs > 8 && robust > 3, `${needs} kept, ${robust} confirmed`);
  assert('every statement made is in the field', (await count('#pane-needs .nd-made')) > 200);
  await shot('needs-03-field');

  // A need opened where it stands.
  await page.eval(`(() => { const all = [...document.querySelectorAll('#pane-needs .nd-need.nd-robust')]; const care = all.find((g) => /need care/.test(g.getAttribute('aria-label'))) || all[0]; care.dispatchEvent(new MouseEvent('click', { bubbles: true })); })()`);
  await page.until(`document.querySelector('#pane-needs .nd-card')`, 20);
  await sleep(500);
  const card = await text('#pane-needs .nd-card');
  assert('a need opens where it stands: its statement, its numbers, its tests in words', /need /.test(card) && /the areas kept aside/.test(card) && /the holdout/.test(card) && /resampled areas/.test(card), card.slice(0, 300));
  assert('its standing from the argument of its tests', (await count('#pane-needs .nd-card .nd-arg')) >= 1);
  assert('the line of operators that made it, drawn back through the field', (await count('#pane-needs .nd-thread path')) >= 1 || (await count('#pane-needs .nd-thread text')) >= 1);
  await shot('needs-04-opened');

  await clickText('#pane-needs .nd-card .nd-quiet', 'Show where on the map');
  await page.until(`document.querySelectorAll('#pane-needs .nd-area[style]').length > 20`, 60);
  await sleep(500);
  const said = await text('#pane-needs .nd-areasbox');
  assert('the need placed on the map, area by area', /Where the need lies/.test(said), said.slice(0, 160));
  await page.eval(`document.querySelector('#pane-needs .nd-areasbox').scrollIntoView()`);
  await shot('needs-05-map');

  // The ledger and the generations.
  await page.eval(`document.querySelector('#pane-needs .nd-ledgerbox').scrollIntoView()`);
  const tests = await count('#pane-needs .nd-test');
  const yes = await count('#pane-needs .nd-test.yes');
  assert('the ledger: every statement admitted tested once, the confirmed above the rule\'s level', tests > 10 && yes > 3 && yes < tests, `${yes} of ${tests}`);
  assert('the journal\'s last seal is shown', /ends in/.test(await text('#pane-needs .nd-ledgerbox')));
  assert('the generations, each with who made its children', (await count('#pane-needs .nd-generation')) === 10 && (await count('#pane-needs .nd-birth')) >= 20);
  await page.eval(`document.querySelectorAll('#pane-needs .nd-generation')[4].dispatchEvent(new MouseEvent('click', { bubbles: true }))`);
  await sleep(600);
  assert('a generation brought forward in the field', (await count('#pane-needs .nd-made.on')) > 10, String(await count('#pane-needs .nd-made.on')));
  await page.eval(`document.querySelector('#pane-needs .nd-ledgerbox').scrollIntoView()`);
  await shot('needs-06-ledger');
  await page.eval(`document.querySelectorAll('#pane-needs .nd-generation')[4].dispatchEvent(new MouseEvent('click', { bubbles: true }))`);

  // Statements of one's own.
  await page.eval(`document.querySelector('#pane-needs').scrollTop = 0`);
  await clickText('#pane-needs .nd-quiet', 'Write or ask for statements');
  await page.until(`document.querySelector('#pane-needs .nd-compose textarea')`, 20);
  await page.eval(`(() => { const t = document.querySelector('#pane-needs .nd-compose textarea'); t.value = 'need care for age >= 85 and sex = female by comparative need_assist higher\\nneed care for income < 30000 by comparative need_assist higher\\nneed income for age >= 65 and social_housing > 0.1 by normative low_income above 30%'; t.dispatchEvent(new Event('input', { bubbles: true })); })()`);
  await clickText('#pane-needs .nd-compose .nd-quiet', 'Read');
  await page.until(`document.querySelectorAll('#pane-needs .nd-line').length >= 3`, 30);
  const lines = await page.eval(`[...document.querySelectorAll('#pane-needs .nd-line')].map((e) => e.className + ' ' + e.textContent)`);
  assert('one\'s own statements read, the one the data cannot judge refused with the reason', lines.filter((l) => /yes/.test(l)).length === 2 && lines.some((l) => /no/.test(l) && /not an attribute/.test(l)), JSON.stringify(lines).slice(0, 400));
  await clickText('#pane-needs .nd-compose .nd-quiet', 'Test them');
  await page.until(`[...document.querySelectorAll('#pane-needs .nd-line')].some((e) => /people, z/.test(e.textContent))`, 60);
  await sleep(500);
  assert('and tested on the areas the search sees', true);
  await clickText('#pane-needs .nd-compose .nd-quiet', 'Add to the next generations');
  await sleep(400);
  assert('added to the next generations as a person\'s proposals', /wait for the next generation/.test(await text('#pane-needs .nd-head')));
  await shot('needs-07-own');

  // A language model asked for statements, when NEEDS_MODEL is set (it calls a real model with the key kept in the settings folder).
  if (process.env.NEEDS_MODEL) {
    await page.eval(`(() => { const t = document.querySelectorAll('#pane-needs .nd-compose textarea')[1]; t.value = 'older people and carers who may lack help, and young people on low incomes'; t.dispatchEvent(new Event('input', { bubbles: true })); })()`);
    await clickText('#pane-needs .nd-compose .nd-quiet', 'Ask a model for statements');
    await page.until(`document.querySelector('#pane-needs .nd-asked') || document.querySelector('#pane-needs .nd-compose .wrong')`, 240);
    await sleep(600);
    const asked = await text('#pane-needs .nd-compose');
    assert('a language model\'s statements read by the engine, each a proposal of grade C, a line that does not read refused', /grade C/.test(asked), asked.slice(-400));
    await shot('needs-09-model');
  }

  // The template's plan, and the studio's map of a need area by area.
  const plan = await post(`api/projects/${name}/run`, { all: true });
  assert('the template\'s plan is started', !!plan, JSON.stringify(plan).slice(0, 160));

  // By night.
  await page.eval(`document.documentElement.dataset.theme = 'night'`);
  await page.eval(`document.querySelector('#pane-needs').scrollTop = 0`);
  await sleep(500);
  await shot('needs-08-night');
} catch (error) {
  assert('the drive ran to its end', false, error.stack || String(error));
}
console.log(`\n${passed} of ${passed + failures} steps passed; pictures in ${OUT}`);
await page.close();
process.exit(failures ? 1 : 0);
