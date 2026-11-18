// The Law Studio's theories of the case driven in headless Chrome (ADR 0127; not in the suite). The invented fire at the boathouse:
// the evidence as a cube, turned, with each person's prisms; the plane at an instant with where each could then have been; the plan
// from above; an item opened where it stands (the till roll held back); whether each person could have been at the scene; the search
// run; the theory that Vane lit the fire drawn through its events, and its test on the till roll; the field of theories with the
// question over them; a theory opened as its story; the best alternative; a theory of one's own assessed and entered; the ledger of
// tests; by night. With --model, a language model proposes theories in its four roles. Pictures go to the folder given.
//
//   node applications/law-studio/tests/drive-theories.mjs [http://127.0.0.1:9170/] [folder for pictures] [--model]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const args = process.argv.slice(2).filter((a) => !a.startsWith('--'));
const MODEL = process.argv.includes('--model');
const BASE = args[0] || 'http://127.0.0.1:9170/';
const OUT = args[1] || join(tmpdir(), 'law-studio-theories');
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
const name = (await post('api/projects', { title: 'The fire at the boathouse' })).name;
const laid = await post(`api/projects/${name}/example`, { template: 'incident/the-boathouse' });
assert('the boathouse laid out with its twelve documents', laid.documents?.length === 12, JSON.stringify(laid).slice(0, 200));

const page = await open(`${BASE}?project=${name}`, { port: 9393, dir: join(tmpdir(), `law-studio-theories-${Date.now()}`), width: 1500, height: 940 });
const shot = async (file) => { await sleep(500); await page.shot(join(OUT, `${file}.jpg`)); };
const tab = async (id, ready, wait = 90) => { await page.eval(`document.querySelector('.tab[data-tab="${id}"]').click()`); await page.until(`document.querySelector('#pane-${id} ${ready}')`, wait); await sleep(300); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const click = (js) => page.eval(`(() => { const x = ${js}; if (!x) return false; x.dispatchEvent(new MouseEvent('click', { bubbles: true })); return true; })()`);
try {
  await page.until(`document.querySelector('.tab[data-tab="spacetime"]')`, 60);
  await tab('spacetime', 'svg.spacetime', 120);
  assert('the evidence drawn as a cube: the place on the floor, every person\'s prisms rising, the items where and when they speak',
    await page.eval(`document.querySelectorAll('#pane-spacetime .st-prism').length >= 2 && document.querySelectorAll('#pane-spacetime .st-item').length >= 9 && !!document.querySelector('#pane-spacetime .st-building')`));
  assert('whether each could have been at the scene: Vane clearly, Ashby arguably', /Corrie Vane/.test(await text('.stv-presence')) && await page.eval(`!!document.querySelector('.stv-who.clear') && !!document.querySelector('.stv-who.arguable')`), await text('.stv-presence'));
  await shot('law-theories-01-cube');
  // Turned by hand.
  await page.eval(`(() => { const s = document.querySelector('.stv-stage'); const r = s.getBoundingClientRect(); const at = (t, x, y) => s.dispatchEvent(new PointerEvent(t, { bubbles: true, clientX: x, clientY: y, pointerId: 1 })); at('pointerdown', r.left + 300, r.top + 300); at('pointermove', r.left + 420, r.top + 250); at('pointerup', r.left + 420, r.top + 250); })()`);
  await shot('law-theories-02-turned');
  // The plane at 21:44:30.
  await page.eval(`(() => { const s = document.querySelector('.stv-range'); s.value = String(14 * 60 + 30); s.dispatchEvent(new Event('input', { bubbles: true })); })()`);
  await page.until(`/21:44:30/.test(document.querySelector('.stv-clock')?.textContent || '')`, 10);
  assert('the plane at 21:44:30, with where each person could then have been', await page.eval(`document.querySelectorAll('#pane-spacetime .st-slice').length >= 2`));
  await shot('law-theories-03-plane');
  await click(`[...document.querySelectorAll('.stv-ribbon .key')].find((b) => b.textContent === 'From above')`);
  await page.until(`document.querySelector('#pane-spacetime svg.spacetime.plan')`, 20);
  assert('from above, the plan at the instant', true);
  await shot('law-theories-04-above');
  await click(`[...document.querySelectorAll('.stv-ribbon .key')].find((b) => b.textContent === 'In the round')`);
  await page.until(`document.querySelector('#pane-spacetime svg.spacetime:not(.plan)')`, 20);
  // The search run.
  await tab('theories', '.th-standings', 60);
  await click(`[...document.querySelectorAll('#pane-theories .key')].find((b) => b.textContent === 'Run ten generations')`);
  await page.until(`document.querySelectorAll('#pane-theories .th-seg').length >= 3 && !document.querySelector('#pane-theories .key[disabled]')`, 180);
  await click(`[...document.querySelectorAll('#pane-theories .key')].find((b) => b.textContent === 'Run ten generations')`);
  await page.until(`!document.querySelector('#pane-theories .key[disabled]') && /generation 2\\d/.test(document.querySelector('#pane-theories .wall-head')?.textContent || '')`, 180);
  const standing = await text('.th-standing');
  assert('the field of theories, and damage by fire clear under none of them: the alternatives not excluded', await page.eval(`document.querySelectorAll('.th-seg').length >= 3`) && /not excluded/.test(standing), standing.slice(0, 300));
  await shot('law-theories-05-field');
  // Vane's theory opened as its story.
  await click(`[...document.querySelectorAll('.th-seg')].find((b) => b.textContent.includes('Corrie Vane'))`);
  await page.until(`/Corrie Vane/.test(document.querySelector('.th-story h3')?.textContent || '')`, 20);
  assert('Vane\'s theory as its story, minute by minute, its test on the till roll held back: it must doubt it', /must doubt late-till/.test(await text('.th-test')) && await page.eval(`document.querySelectorAll('.th-line').length >= 5`), await text('.th-test'));
  await page.eval(`document.querySelector('.th-story').scrollIntoView({ block: 'start' })`);
  await shot('law-theories-06-story');
  // Drawn in space and time.
  await click(`[...document.querySelectorAll('.th-story .key')].find((b) => b.textContent.includes('space and time'))`);
  await page.until(`document.querySelector('#pane-spacetime .st-path') && document.querySelector('#pane-spacetime .st-event')`, 30);
  await page.eval(`(() => { const s = document.querySelector('.stv-range'); s.value = String(15 * 60 + 20); s.dispatchEvent(new Event('input', { bubbles: true })); })()`);
  assert('drawn through its events: Vane\'s path from the inn to the door and home', /ignite by Corrie Vane/.test(await text('#pane-spacetime .st-event')));
  await shot('law-theories-07-vane-drawn');
  // The till roll held back, opened where it stands.
  await click(`document.querySelector('#pane-spacetime .st-item[data-item="late-till"]')`);
  await page.until(`!document.querySelector('.stv-card').hidden`, 10);
  assert('an item opened where it stands: the till roll, held back from the search', /Held back/.test(await text('.stv-card')));
  await shot('law-theories-08-held-back');
  await click(`document.querySelector('.stv-card .close')`);
  // The best alternative.
  await click(`[...document.querySelectorAll('.stv-chip')].find((b) => b.textContent.includes('grey coat'))`);
  await page.until(`[...document.querySelectorAll('.stv-chip.on')].some((b) => b.textContent.includes('grey coat'))`, 20);
  assert('the grey coat\'s theory drawn: its test admitted', /✓/.test(await text('.stv-chip.on')));
  await shot('law-theories-09-alternative');
  // A theory of one's own.
  await tab('theories', '.th-own', 30);
  await page.eval(`(() => { const t = document.querySelector('.th-write'); t.value = 'event ignite actor=ashby at -3 2 when 21:43:30\\ndoubt hale'; })()`);
  await click(`[...document.querySelectorAll('.th-own .key')].find((b) => b.textContent === 'Assess it')`);
  await page.until(`/coherent/.test(document.querySelector('.th-assessed')?.textContent || '')`, 30);
  assert('a theory of one\'s own assessed: the caretaker\'s hand, doubting Mr Hale, coherent', /✓ coherent/.test(await text('.th-assessed')), await text('.th-assessed'));
  await page.eval(`document.querySelector('.th-own').scrollIntoView({ block: 'center' })`);
  await shot('law-theories-10-own');
  await page.eval(`document.querySelector('.th-ledger').scrollIntoView({ block: 'center' })`);
  assert('the ledger of tests on what was held back, each registered before it was written', await page.eval(`document.querySelectorAll('.th-tests li').length >= 3`));
  await shot('law-theories-11-ledger');
  if (MODEL) {
    await page.eval(`document.querySelector('#pane-theories').scrollTop = 0`);
    await click(`[...document.querySelectorAll('#pane-theories .key')].find((b) => b.textContent === 'Ask a model')`);
    await page.until(`!document.querySelector('#pane-theories .key[disabled]') && /proposed/.test(document.querySelector('#console')?.textContent || '')`, 300);
    assert('a model proposed theories in its roles, each read by the engine (grade C)', /proposed \d+ theories/.test(await text('#console')), (await text('#console')).slice(-300));
    await shot('law-theories-12-model');
  }
  await tab('spacetime', 'svg.spacetime', 30);
  await click(`document.getElementById('theme')`);
  await shot('law-theories-13-night');
  assert('no error was thrown in the page', !(page.errors || []).length, JSON.stringify(page.errors || []));
} catch (error) {
  failures++;
  console.log(`FAIL  the drive stopped: ${error.message}`);
  await page.shot(join(OUT, 'law-theories-fail.jpg'));
  console.log('pane:', (await page.eval(`document.querySelector('.pane:not([hidden])')?.textContent.slice(0, 600)`)));
  console.log('console:', (await text('#console')).slice(-500));
}
await page.close();
console.log(`${passed} of ${passed + failures} steps passed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
