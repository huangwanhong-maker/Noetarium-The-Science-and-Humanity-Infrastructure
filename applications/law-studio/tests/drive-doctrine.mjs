// The Law Studio's theories of the law driven in headless Chrome (ADR 0133; not in the suite). The invented dash through the park:
// the law of the matter read as doctrines; the search run; the field of doctrines with the offence's standing over the acceptable
// ones, each kind's best ringed; a doctrine opened beside it with its readings, ratios, score and test; the case law along time as
// it reads it, a case opened, the decision held back veiled with its verdict; the precedents on the emergency as a board, the reasons
// read ringed and the side forced; another kind's doctrine chosen, the board changing; a doctrine of one's own assessed and entered;
// the ledger of tests; by night. With --model, a language model proposes doctrines in its four roles. Pictures go to the folder given.
//
//   node applications/law-studio/tests/drive-doctrine.mjs [http://127.0.0.1:9170/] [folder for pictures] [--model]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const args = process.argv.slice(2).filter((a) => !a.startsWith('--'));
const MODEL = process.argv.includes('--model');
const BASE = args[0] || 'http://127.0.0.1:9170/';
const OUT = args[1] || join(tmpdir(), 'law-studio-doctrine');
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
const name = (await post('api/projects', { title: 'Council of Avalon v Dashwood' })).name;
const laid = await post(`api/projects/${name}/example`, { template: 'park/the-dash' });
assert('the dash through the park laid out with its interview', laid.documents?.length === 1, JSON.stringify(laid).slice(0, 200));

const page = await open(`${BASE}?project=${name}`, { port: 9394, dir: join(tmpdir(), `law-studio-doctrine-${Date.now()}`), width: 1500, height: 940 });
const shot = async (file) => { await sleep(500); await page.shot(join(OUT, `${file}.jpg`)); };
const tab = async (id, ready, wait = 90) => { await page.eval(`document.querySelector('.tab[data-tab="${id}"]').click()`); await page.until(`document.querySelector('#pane-${id} ${ready}')`, wait); await sleep(300); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const click = (js) => page.eval(`(() => { const x = ${js}; if (!x) return false; x.dispatchEvent(new MouseEvent('click', { bubbles: true })); return true; })()`);
const scroll = (sel, block = 'start') => page.eval(`document.querySelector(${JSON.stringify(sel)})?.scrollIntoView({ block: '${block}' })`);
try {
  await page.until(`document.querySelector('.tab[data-tab="doctrine"]')`, 60);
  await tab('doctrine', 'svg.doctrine-field', 120);
  assert('the law of the matter read as doctrines before any search: the field drawn, the decided cases along time, the precedents as a board',
    await page.eval(`!!document.querySelector('#pane-doctrine svg.doctrine-caselaw') && !!document.querySelector('#pane-doctrine svg.doctrine-board')`));
  await click(`[...document.querySelectorAll('#pane-doctrine .key')].find((b) => b.textContent === 'Run ten generations')`);
  await page.until(`!document.querySelector('#pane-doctrine .key[disabled]') && /generation 1\\d/.test(document.querySelector('#pane-doctrine .wall-head')?.textContent || '')`, 240);
  const standing = await text('#pane-doctrine .th-standing');
  assert('the offence has a standing over the acceptable doctrines', /over the acceptable doctrines/.test(standing), standing.slice(0, 300));
  assert('the field: every kind\'s best found by the search, ringed solid', await page.eval(`document.querySelectorAll('#pane-doctrine .df-dot').length >= 3 && document.querySelectorAll('#pane-doctrine .df-exact').length === document.querySelectorAll('#pane-doctrine .df-exact.found').length`),
    await page.eval(`document.querySelectorAll('#pane-doctrine .df-exact').length + ' kinds, ' + document.querySelectorAll('#pane-doctrine .df-exact.found').length + ' found'`));
  await shot('law-doctrine-01-field');
  assert('the best doctrine opened beside the field, with its readings, its score term by term and its test', /Its readings/.test(await text('.dc-card')) && /score/.test(await text('.dc-score')) && /upheld|contradicted/.test(await text('.dc-test')), await text('.dc-card'));
  await scroll('.dc-card', 'center');
  await shot('law-doctrine-02-card');
  // The case law, and a case opened.
  await scroll('.dc-caselaw', 'center');
  await click(`document.querySelector('.dl-case[data-case="lindqvist"]')`);
  await page.until(`/Lindqvist/.test(document.querySelector('.dc-case')?.textContent || '')`, 20);
  assert('a decided case opened: Lindqvist, binding, its passage', /binding/.test(await text('.dc-case')) && /walking pace/.test(await text('.dc-case')), await text('.dc-case'));
  await scroll('.dc-caselaw', 'center');
  await shot('law-doctrine-03-caselaw');
  await click(`document.querySelector('.dl-case[data-case="moreau"]')`);
  await page.until(`/held back/.test(document.querySelector('.dc-case')?.textContent || '')`, 20);
  assert('the decision held back opened: given after the matter, never read by the search, and the doctrine\'s verdict on it', /never read it/.test(await text('.dc-case')), await text('.dc-case'));
  await scroll('.dc-caselaw', 'center');
  await shot('law-doctrine-04-held-back');
  // The board.
  await scroll('.dc-board', 'center');
  assert('the precedents on the emergency as a board: a reason ringed in each precedent read, and the matter\'s side', await page.eval(`document.querySelectorAll('.dc-board .db-reason').length >= 4`) && /force/.test(await text('.dc-board')), await text('.dc-board p'));
  await shot('law-doctrine-05-board');
  // Another kind's doctrine chosen: one under which the precedents hold the emergency.
  const other = await page.eval(`(() => { const on = document.querySelector('.df-dot.on'); const d = [...document.querySelectorAll('.df-dot')].find((g) => g !== on && /emergency held by the precedents/.test(g.getAttribute('aria-label'))); return d ? d.dataset.serial : null; })()`);
  if (other) {
    await click(`document.querySelector('.df-dot[data-serial="${other}"]')`);
    await page.until(`/#${other}\\b/.test(document.querySelector('.dc-card h3')?.textContent || '')`, 20);
    assert('another kind\'s doctrine chosen, the precedents holding the emergency under it: its card says so, and its board shows the side forced',
      /held by the precedents/.test(await text('.dc-card header')) && /forced: emergency/.test(await page.eval(`document.querySelector('.dc-board svg')?.textContent || ''`)), await text('.dc-card header'));
    await page.eval(`document.querySelector('#pane-doctrine').scrollTop = 0`);
    await shot('law-doctrine-06-other');
    await scroll('.dc-board', 'center');
    await shot('law-doctrine-07-other-board');
  }
  // A doctrine of one's own.
  await scroll('.th-own', 'center');
  await page.eval(`(() => { const t = document.querySelector('#pane-doctrine .th-write'); t.value = 'reading vehicle = broad\\nratio emergency/parent: child_injured'; })()`);
  await click(`[...document.querySelectorAll('#pane-doctrine .th-own .key')].find((b) => b.textContent === 'Assess it')`);
  await page.until(`/score/.test(document.querySelector('#pane-doctrine .th-assessed')?.textContent || '')`, 30);
  assert('a doctrine of one\'s own assessed, its kind and score said', /score/.test(await text('#pane-doctrine .th-assessed')), await text('#pane-doctrine .th-assessed'));
  await shot('law-doctrine-08-own');
  await scroll('#pane-doctrine .th-ledger', 'center');
  assert('the ledger of tests on what was held back, each registered before it was written', await page.eval(`document.querySelectorAll('#pane-doctrine .th-tests li').length >= 3`));
  await shot('law-doctrine-09-ledger');
  if (MODEL) {
    await page.eval(`document.querySelector('#pane-doctrine').scrollTop = 0`);
    await click(`[...document.querySelectorAll('#pane-doctrine .key')].find((b) => b.textContent === 'Ask a model')`);
    await page.until(`!document.querySelector('#pane-doctrine .key[disabled]') && /proposed \\d+ doctrines/.test(document.querySelector('#console')?.textContent || '')`, 300);
    assert('a model proposed doctrines in its roles, each read by the engine (grade C)', /proposed \d+ doctrines/.test(await text('#console')), (await text('#console')).slice(-300));
    await shot('law-doctrine-10-model');
  }
  await page.eval(`document.querySelector('#pane-doctrine').scrollTop = 0`);
  await click(`document.getElementById('theme')`);
  await shot('law-doctrine-11-night');
  assert('no error was thrown in the page', !(page.errors || []).length, JSON.stringify(page.errors || []));
} catch (error) {
  failures++;
  console.log(`FAIL  the drive stopped: ${error.message}`);
  await page.shot(join(OUT, 'law-doctrine-fail.jpg'));
  console.log('pane:', (await page.eval(`document.querySelector('.pane:not([hidden])')?.textContent.slice(0, 600)`)));
  console.log('console:', (await text('#console')).slice(-500));
}
await page.close();
console.log(`\n${passed} passed, ${failures} failed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
