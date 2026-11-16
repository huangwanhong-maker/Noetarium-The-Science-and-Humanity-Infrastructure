// The plug-in Evidence in space and time driven as an analyst would use it, in headless Chrome (not in the suite). A project begun
// from the template Rats in the East Village; the archive laid out; the place drawn with its claims and items; a claim opened where
// it stands, the one whose reports most repeat one registration; an item opened from it; the mode "what to do" with the places worth
// looking at first; a close look; what is kept, each holding with its chain of custody; a holding handed over; the 311 file altered
// on disk, so that every complaint stops counting and the claims are weighed again on the inspections alone, then mended; a claim
// placed by a click and its terms set; the plan of the template run (the channel counted, the claims weighed, how they fared); the
// page by night. Pictures go to the folder given; the store is the server's (its --store), named so that a file can be altered.
//
//   node applications/urban-studio/tests/drive-evidence.mjs <http://127.0.0.1:9095/> <the server's store> [folder for pictures]

import { open } from './cdp.mjs';
import { copyFileSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE = 'http://127.0.0.1:9095/', STORE, OUT = join(tmpdir(), 'urban-evidence-drive')] = process.argv.slice(2);
if (!STORE) { console.error('usage: node drive-evidence.mjs <server> <its store> [pictures]'); process.exit(2); }
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
const made = await post('api/projects', { title: 'Rats in the East Village', template: 'evidence.rats-in-the-east-village' });
const name = made.name;
const laid = await post(`api/projects/${name}/evidence/example`, { case: 'nyc-rodents' });
assert('the project begun from the template, and the archive laid out with its two files taken in', !!name && laid.items > 1000 && laid.holdings === 2, JSON.stringify(laid));

const page = await open(`${BASE}?project=${name}`, { port: 9395, dir: join(tmpdir(), `urban-evidence-chrome-${Date.now()}`), width: 1560, height: 1000 });
const shot = async (file) => { await sleep(500); await page.shot(join(OUT, `${file}.jpg`)); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const tab = async (id, ready) => { await page.eval(`document.querySelector('.tab[data-tab="${id}"]').click()`); await page.until(`document.querySelector('#pane-${id} ${ready}')`, 60); await sleep(400); };
const click = async (x, y) => {
  await page.send('Input.dispatchMouseEvent', { type: 'mouseMoved', x, y });
  await page.send('Input.dispatchMouseEvent', { type: 'mousePressed', x, y, button: 'left', clickCount: 1 });
  await page.send('Input.dispatchMouseEvent', { type: 'mouseReleased', x, y, button: 'left', clickCount: 1 });
};
// A point of a claim's disc that no item covers, on the screen.
const claimPoint = (id) => page.eval(`(() => {
  const c = document.querySelector('[data-claim="${id}"]'); if (!c) return null;
  const r = c.getBoundingClientRect(), cx = r.left + r.width / 2, cy = r.top + r.height / 2;
  for (const f of [0, 0.35, 0.55, 0.7]) for (let a = 0; a < 12; a++) {
    const x = cx + Math.cos(a * Math.PI / 6) * f * r.width / 2, y = cy + Math.sin(a * Math.PI / 6) * f * r.height / 2;
    const hit = document.elementFromPoint(x, y);
    if (hit && hit.closest('[data-claim]')?.dataset.claim === '${id}' && !hit.closest('[data-item]')) return [x, y];
  }
  return [cx, cy];
})()`);
try {
  await page.until(`document.querySelector('.tab[data-tab="evidence"]')`, 60);
  await sleep(1500);
  console.log(`(the explorer holds ${await page.eval(`document.getElementById('explorer').textContent.length`)} characters; the inspector ${await page.eval(`document.getElementById('inspector').textContent.length`)})`);
  assert('the plug-in brings two views of its own, Evidence and Holdings, beside the studio\'s', await page.eval(`!!document.querySelector('.tab[data-tab="holdings"]')`));
  await tab('evidence', 'svg.place .lo-claim');
  await page.until(`document.querySelectorAll('#pane-evidence .ev-base path.lo-way').length > 10`, 30);
  const counts = await page.eval(`[document.querySelectorAll('#pane-evidence svg.place .lo-claim').length, document.querySelectorAll('#pane-evidence svg.place .lo-item').length, document.querySelectorAll('#pane-evidence .ev-base path').length]`);
  assert('the place is the page: its streets and footprints, 399 claims and 1,566 items where they were', counts[0] === 399 && counts[1] === 1566 && counts[2] > 10, JSON.stringify(counts));
  const said = await text('#pane-evidence .ev-said');
  assert('the head says the archive is intact and how the claims stand', /Every holding is what it was/.test(said) && /established/.test(said) && /refuted/.test(said), said);
  assert('the strip lays the items along time, a row a kind', await page.eval(`document.querySelectorAll('#pane-evidence svg.strip .lo-kind').length === 2 && document.querySelectorAll('#pane-evidence svg.strip [data-item]').length === 1566`));
  await shot('evidence-01-place');

  // The claim whose reports most repeat one registration.
  const worst = await page.eval(`fetch('/api/projects/${name}/evidence/appraise', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: '{}' }).then((r) => r.json()).then((a) => a.appraisal.weigh.claims.filter((c) => c.overstatement).sort((p, q) => q.overstatement - p.overstatement)[0].id)`);
  const [x, y] = await claimPoint(worst);
  await click(x, y);
  await page.until(`document.querySelector('#pane-evidence .ev-card h3')`, 20);
  const card = await text('#pane-evidence .ev-card');
  assert(`a claim opens where it stands (${worst}): its standing, the weight of its evidence, what bears on it, and that repeated reports count once`, /dB/.test(card) && /What bears on it/.test(card) && /repeat a registration counted once/.test(card) && /What to do/.test(card), card.slice(0, 300));
  await shot('evidence-02-claim');
  await page.eval(`document.querySelector('#pane-evidence .ev-card .ev-item-row').click()`);
  await page.until(`/Who says it/.test(document.querySelector('#pane-evidence .ev-card')?.textContent || '')`, 20);
  const item = await text('#pane-evidence .ev-card');
  assert('an item opens from it: who says it with their channel, what it rests on and its custody, the claims it bears on', /What it rests on/.test(item) && /intact/.test(item) && /It bears on/.test(item), item.slice(0, 300));
  await shot('evidence-03-item');

  await page.eval(`[...document.querySelectorAll('#pane-evidence .ev-mode')].find((b) => /What to do/.test(b.textContent)).click()`);
  await page.until(`document.querySelector('#pane-evidence .ev-sheet') && document.querySelectorAll('#pane-evidence svg.place .lo-claim.inspect').length > 0`, 30);
  const sheet = await text('#pane-evidence .ev-sheet');
  const acts = await page.eval(`[...document.querySelectorAll('#pane-evidence .ev-acts input[type="text"]')].map((i) => i.value).join(', ')`);
  assert('what to do: the acts with their worths, a survey and its budget, and the places worth surveying first ringed on the map', /worth looking at first/.test(sheet) && /treat the area/.test(acts) && await page.eval(`document.querySelectorAll('#pane-evidence svg.place .lo-claim.inspect').length`) >= 20, sheet.slice(0, 300));
  await page.eval(`(() => { const c = [...document.querySelectorAll('#pane-evidence .ev-card .ev-close')][0]; if (c) c.click(); })()`);
  await sleep(300);
  await shot('evidence-04-decide');
  // A close look, by the wheel.
  const m = await page.eval(`(() => { const r = document.querySelector('#pane-evidence .ev-map').getBoundingClientRect(); return [r.left + r.width * 0.55, r.top + r.height * 0.45]; })()`);
  for (let k = 0; k < 2; k++) await page.send('Input.dispatchMouseEvent', { type: 'mouseWheel', x: m[0], y: m[1], deltaX: 0, deltaY: -260 });
  await sleep(600);
  // A place worth surveying first, among those in view, nearest the middle.
  const decided = await page.eval(`(() => {
    const r = document.querySelector('#pane-evidence .ev-map').getBoundingClientRect(), mx = r.left + r.width / 2, my = r.top + r.height / 2;
    const seen = [...document.querySelectorAll('#pane-evidence svg.place .lo-claim.inspect')].map((c) => { const b = c.getBoundingClientRect(); return { id: c.dataset.claim, x: b.left + b.width / 2, y: b.top + b.height / 2 }; })
      .filter((c) => c.x > r.left + 40 && c.x < r.right - 40 && c.y > r.top + 40 && c.y < r.bottom - 40).sort((p, q) => Math.hypot(p.x - mx, p.y - my) - Math.hypot(q.x - mx, q.y - my));
    return seen[0]?.id || null;
  })()`);
  const near = decided ? await claimPoint(decided) : null;
  if (near) { await click(near[0], near[1]); await page.until(`document.querySelector('#pane-evidence .ev-card h3')`, 20); }
  const looked = await text('#pane-evidence .ev-card');
  assert('close in, a place worth surveying first says what the survey is worth there and what would be done after it', /Worth looking at first/.test(looked) && /if it found the thing|whatever it found/.test(looked), looked.slice(0, 260));
  await shot('evidence-05-close');

  // What is kept.
  await tab('holdings', '.ev-holding');
  const held = await text('#pane-holdings');
  assert('what is kept: the two files, each intact, with its chain of custody', /Intact/.test(held) && (await page.eval(`document.querySelectorAll('#pane-holdings .ev-holding').length`)) === 2 && /the data library's cache/.test(held), held.slice(0, 200));
  await page.eval(`(() => { const h = document.querySelector('#pane-holdings .ev-holding'); const [to, note] = h.querySelectorAll('.ev-hand input'); to.value = 'the analyst'; note.value = 'for the quarter\\'s review'; [...h.querySelectorAll('.ev-hand .key')].pop().click(); })()`);
  await page.until(`/the analyst/.test(document.querySelector('#pane-holdings .ev-chain')?.textContent || '')`, 30);
  assert('a holding handed over: the handover is written into its chain with the digest observed then, and the chain holds', /the same bytes/.test(await text('#pane-holdings .ev-chain')) && !(await page.eval(`!!document.querySelector('#pane-holdings .ev-holding.unsound')`)));
  await shot('evidence-06-holdings');

  // The 311 file altered on disk.
  const file = join(STORE, 'projects', name, 'evidence', 'holdings', 'complaints-2024.json');
  const kept = readFileSync(file);
  copyFileSync(file, `${file}.kept`);
  writeFileSync(file, Buffer.concat([kept, Buffer.from('\n')]));
  await page.eval(`document.querySelector('.tab[data-tab="evidence"]').click()`);
  await sleep(200);
  await tab('holdings', '.ev-holding.unsound');
  const broken = await text('#pane-holdings');
  assert('a byte added to the 311 file on disk: its digest is not the one handed over, and the page says it cannot be relied on', /altered/.test(broken) && /Not all of it can be relied on/.test(broken), broken.slice(0, 300));
  await shot('evidence-07-altered');
  await tab('evidence', 'svg.place .lo-item.left');
  const left = await page.eval(`document.querySelectorAll('#pane-evidence svg.place .lo-item.left').length`), complaints = laid.items - (await page.eval(`[...document.querySelectorAll('#pane-evidence svg.strip .lo-kind')].map((k) => k.textContent)`)).filter((k) => /inspection/.test(k)).map((k) => Number(k.replace(/\D+/g, '')))[0];
  assert('on the place every complaint stops counting, drawn as a cross, and the claims are weighed on the inspections alone', left === complaints && left > 100, `${left} items do not count, of ${complaints} complaints`);
  await shot('evidence-08-weighed-again');
  writeFileSync(file, kept);
  await tab('holdings', '.ev-holding');
  await page.eval(`document.querySelector('.tab[data-tab="evidence"]').click()`);
  await sleep(300);

  // A claim placed by a click.
  await page.eval(`[...document.querySelectorAll('#pane-evidence .ev-mode')].find((b) => /What the evidence/.test(b.textContent)).click()`);
  await sleep(400);
  await page.eval(`[...document.querySelectorAll('#pane-evidence .ev-head .key')].find((b) => /a claim/.test(b.textContent)).click()`);
  await page.until(`document.querySelector('#pane-evidence .ev-map.placing')`, 10);
  const spot = await page.eval(`(() => { const r = document.querySelector('#pane-evidence .ev-map').getBoundingClientRect(); return [r.left + r.width * 0.42, r.top + r.height * 0.5]; })()`);
  await click(spot[0], spot[1]);
  await page.until(`document.querySelector('#pane-evidence .ev-form')`, 30);
  await page.eval(`(() => { const f = document.querySelector('#pane-evidence .ev-form'); f.querySelector('textarea').value = 'Rats are active along this block, May 2024'; f.querySelector('[data-key="radius"]').value = '90'; f.querySelector('[data-key="from"]').value = '2024-05'; f.querySelector('[data-key="until"]').value = '2024-05'; })()`);
  await shot('evidence-09-place-a-claim');
  await page.eval(`[...document.querySelectorAll('#pane-evidence .ev-form .key')].find((b) => /Keep/.test(b.textContent)).click()`);
  await page.until(`/along this block/.test(document.querySelector('#pane-evidence .ev-card h3')?.textContent || '')`, 30);
  const placed = await text('#pane-evidence .ev-card');
  assert('a claim placed by a click takes the archive\'s terms, and with its own set is weighed at once by the items of its place and month', /dB|no evidence/.test(placed) && /Within 90 m/.test(placed) && !/refuted — from 50% to 0\.0%/.test(placed), placed.slice(0, 260));
  await shot('evidence-10-placed');

  // The template's plan.
  await page.eval(`window.studio.runPlan()`);
  await page.until(`window.studio.S.project.steps.filter((s) => s.ran).length >= 3`, 300);
  const steps = await page.eval(`window.studio.S.project.steps.map((s) => s.title + ': ' + (s.error || 'ran'))`);
  assert('the template\'s plan runs: the channel counted, the claims weighed and decided, how they fared', steps.length >= 3 && steps.every((s) => /ran$/.test(s)), JSON.stringify(steps));
  const fared = await page.eval(`JSON.stringify(window.studio.S.project.steps.find((s) => s.id === 'p-fared').outputs.filter((o) => o.kind === 'note'))`);
  assert('how they fared is scored against the prior alone', /better than the prior alone|no better than the prior alone/.test(fared), fared.slice(0, 200));
  await page.eval(`window.studio.show('map')`);
  await sleep(1500);
  await shot('evidence-11-studio-map');

  await page.eval(`document.getElementById('theme').click()`);
  await tab('evidence', 'svg.place .lo-claim');
  await shot('evidence-12-night');
  await page.eval(`document.getElementById('theme').click()`);
  assert('the page threw nothing', page.errors.length === 0, page.errors.join(' | '));
  if (page.logs.length) console.log(`(console: ${page.logs.slice(0, 5).join(' | ')})`);
} catch (error) {
  failures++;
  console.log(`FAIL  ${error.message}`);
  try { await page.shot(join(OUT, 'evidence-failure.jpg')); } catch { /* no picture */ }
  if (page.errors.length) console.log(`(page errors: ${page.errors.join(' | ')})`);
} finally {
  await page.close();
}
console.log(`\n${passed} passed, ${failures} failed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
