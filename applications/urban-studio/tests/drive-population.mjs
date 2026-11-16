// The plug-in Population dynamics driven as an analyst would use it, in headless Chrome (not in the suite). A project begun from the
// template Melbourne's people; the field as recorded, and its change; a km² opened; the laws learned and judged, one with its
// currents, the best with its forecast's error; the record's modes; the years ahead under a law of spread and drift with a new source
// placed by a click; Lagos, as recorded and under its laws; the template's plan run and the studio's map; the page by night. Pictures go
// to the folder given.
//
//   node applications/urban-studio/tests/drive-population.mjs <http://127.0.0.1:9096/> [folder for pictures]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE = 'http://127.0.0.1:9096/', OUT = join(tmpdir(), 'urban-population-drive')] = process.argv.slice(2);
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
const made = await post('api/projects', { title: 'Melbourne\'s people', template: 'population.melbourne-1975-2050' });
const name = made.name;
assert('the project begun from the template', !!name, JSON.stringify(made).slice(0, 200));

const page = await open(`${BASE}?project=${name}`, { port: 9396, dir: join(tmpdir(), `urban-population-chrome-${Date.now()}`), width: 1560, height: 1000 });
const shot = async (file) => { await sleep(600); await page.shot(join(OUT, `${file}.jpg`)); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const clickSel = async (sel) => page.eval(`(() => { const e = document.querySelector(${JSON.stringify(sel)}); if (e) e.click(); return !!e; })()`);
const clickText = async (sel, words) => page.eval(`(() => { const e = [...document.querySelectorAll(${JSON.stringify(sel)})].find((x) => x.textContent.includes(${JSON.stringify(words)})); if (e) e.click(); return !!e; })()`);
const click = async (x, y) => {
  await page.send('Input.dispatchMouseEvent', { type: 'mouseMoved', x, y });
  await page.send('Input.dispatchMouseEvent', { type: 'mousePressed', x, y, button: 'left', clickCount: 1 });
  await page.send('Input.dispatchMouseEvent', { type: 'mouseReleased', x, y, button: 'left', clickCount: 1 });
};
// A point of the map at a share of its width and height.
const mapPoint = (fx, fy) => page.eval(`(() => { const r = document.querySelector('#pane-population .pd-map').getBoundingClientRect(); return [r.left + ${fx} * r.width, r.top + ${fy} * r.height]; })()`);
const settled = () => page.until(`document.querySelector('#pane-population canvas.pd-canvas') && !document.querySelector('#pane-population .pd-busy')`, 120);

try {
  await page.until(`document.querySelector('.tab[data-tab="population"]')`, 60);
  await sleep(1200);
  assert('the plug-in brings its view, Population, beside the studio\'s', true);
  await page.eval(`document.querySelector('.tab[data-tab="population"]').click()`);
  await settled();
  const head = await text('#pane-population .pd-head');
  assert('the field as the page: Melbourne, its cells of 1 km², its sources said', /Melbourne/.test(head) && /cells of 1 km²/.test(head) && /JRC/.test(head), head.slice(0, 200));
  const years = await page.eval(`document.querySelectorAll('#pane-population .pd-year').length`);
  assert('a ribbon of twelve years, 1975 to 2030', years === 12, String(years));
  const side = await text('#pane-population .pd-side');
  assert('everyone in the square over the years, said in words', /people in 1975/.test(side) && /times as many/.test(side), side.slice(0, 200));
  await shot('population-01-recorded');

  await page.eval(`document.querySelector('#pane-population .pd-toggle input').click()`);
  await settled();
  const legend = await text('#pane-population .pd-legend');
  assert('the change since the year before, on a diverging map', /gained or lost, 2015 to 2020/.test(legend), legend);
  await shot('population-02-change');
  await page.eval(`document.querySelector('#pane-population .pd-toggle input').click()`);
  await settled();

  const [cx, cy] = await mapPoint(0.62, 0.45);
  await click(cx, cy);
  await page.until(`document.querySelector('#pane-population .pd-card svg')`, 20);
  const card = await text('#pane-population .pd-card');
  assert('a km² opens where it stands, with its people over the years', /A km²/.test(card) && /people in 1975/.test(card), card.slice(0, 160));
  await shot('population-03-a-cell');
  await clickSel('#pane-population .pd-close');

  await clickText('#pane-population .pd-mode', 'Laws learned');
  await page.until(`document.querySelectorAll('#pane-population .pd-law').length >= 10`, 180);
  await settled();
  const laws = await page.eval(`[...document.querySelectorAll('#pane-population .pd-law-title')].map((e) => e.textContent)`);
  assert('the laws, learned and judged, ranked by where they place the change', laws.length >= 10 && laws.includes('Growth, spread and drift toward the centre'), laws.join(' | '));
  const first = laws[0];
  assert('the record\'s modes place Melbourne\'s change best', /modes/.test(first), first);
  await clickSel('#pane-population .pd-showing .pd-mode:nth-child(3)');
  await settled();
  const errLegend = await text('#pane-population .pd-legend');
  assert('the best law\'s forecast for 2020 against the record, as its error', /the forecast less the record, 2020/.test(errLegend), errLegend);
  await shot('population-04-laws-error');
  await clickText('#pane-population .pd-law-title', 'Growth, spread and drift toward the centre');
  await settled();
  await page.eval(`(() => { const y = [...document.querySelectorAll('#pane-population .pd-year')].find((e) => e.textContent.includes('2010')); y && y.click(); })()`);
  await settled();
  const arrows = await page.eval(`document.querySelectorAll('#pane-population .pd-over .fd-arrow').length`);
  assert('a law\'s currents drawn as arrows over the record', arrows > 20, String(arrows));
  const lawCard = await text('#pane-population .pd-law.on .pd-law-card');
  assert('its card says what it found in words, and what it set aside', /growth at/.test(lawCard) && (/set aside|drift/.test(lawCard)), lawCard.slice(0, 300));
  await shot('population-05-law-currents');

  await clickText('#pane-population .pd-mode', 'Modes');
  await page.until(`document.querySelector('#pane-population .pd-side svg')`, 120);
  await clickText('#pane-population .pd-law-title', 'Mode 2');
  await settled();
  const modeLegend = await text('#pane-population .pd-legend');
  assert('the record\'s modes: their eigenvalues about the circle, and the second mode\'s map', /mode 2 as it stood/.test(modeLegend), modeLegend);
  await shot('population-06-modes');

  await clickText('#pane-population .pd-mode', 'The years ahead');
  await page.until(`document.querySelector('#pane-population .pd-side h3')`, 180);
  await settled();
  await clickText('#pane-population .pd-side .key', 'a new source of people');
  await settled();
  const [sx, sy] = await mapPoint(0.80, 0.30);
  await click(sx, sy);
  await page.until(`document.querySelector('#pane-population .pd-over .fd-mark') && document.querySelector('#pane-population .pd-side h3')`, 180);
  await settled();
  await page.eval(`(() => { const y = [...document.querySelectorAll('#pane-population .pd-year')].find((e) => e.textContent.includes('2050')); y && y.click(); })()`);
  await settled();
  const ahead = await text('#pane-population .pd-side');
  assert('the years ahead under spread and drift, a new source placed by a click, the total against the JRC\'s projections', /2050/.test(ahead) && /the JRC projects/.test(ahead) && /a year/.test(await text('#pane-population .pd-over')), ahead.slice(0, 300));
  await shot('population-07-ahead');

  await clickText('#pane-population .pd-place', 'Lagos');
  await page.until(`/Lagos/.test(document.querySelector('#pane-population .pd-title')?.textContent || '')`, 60);
  await settled();
  await shot('population-08-lagos-ahead');
  await clickText('#pane-population .pd-mode', 'As recorded');
  await settled();
  await page.eval(`document.querySelector('#pane-population .pd-toggle input').click()`);
  await settled();
  await shot('population-09-lagos-change');
  await page.eval(`document.querySelector('#pane-population .pd-toggle input').click()`);
  await clickText('#pane-population .pd-mode', 'Laws learned');
  await page.until(`document.querySelectorAll('#pane-population .pd-law').length >= 10`, 180);
  await settled();
  await clickText('#pane-population .pd-law-title', 'settled landscape');
  await settled();
  const lagosFirst = await page.eval(`document.querySelector('#pane-population .pd-law-title')?.textContent`);
  assert('in Lagos a law places the change better than growing everywhere alike', !/Growth everywhere at one rate|Each place/.test(lagosFirst), lagosFirst);
  await shot('population-10-lagos-laws');

  await clickText('#pane-population .pd-place', 'Melbourne');
  await settled();
  await page.eval(`window.studio.runPlan()`);
  await page.until(`window.studio.S.project.steps.filter((s) => s.ran).length >= 5`, 600);
  const steps = await page.eval(`window.studio.S.project.steps.map((s) => s.title + ': ' + (s.error || 'ran'))`);
  assert('the template\'s plan runs: the place, the whole, the laws, the modes, the years ahead', steps.length >= 5 && steps.every((s) => /ran$/.test(s)), JSON.stringify(steps));
  await page.eval(`window.studio.show('map')`);
  await sleep(2500);
  await shot('population-11-studio-map');

  await page.eval(`document.getElementById('theme').click()`);
  await page.eval(`document.querySelector('.tab[data-tab="population"]').click()`);
  await clickText('#pane-population .pd-mode', 'The years ahead');
  await page.until(`document.querySelector('#pane-population .pd-side h3')`, 180);
  await settled();
  await shot('population-12-night');
  await page.eval(`document.getElementById('theme').click()`);
  assert('the page threw nothing', page.errors.length === 0, page.errors.join(' | '));
} catch (error) {
  assert('the drive ran to its end', false, error.stack || String(error));
  try { await shot('population-failed'); } catch { /* no page */ }
} finally {
  await page.close();
}
console.log(`\n${passed} passed, ${failures} failed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
