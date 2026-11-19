// Multilateral Phronimos's page driven as a person would use it, in headless Chrome (not in the suite). Two records: the invented Arda
// Basin Partnership, which shows every part of the work, and the Migration Multi-Partner Trust Fund as its publisher records it in IATI.
// The basin: the round first, read on its date, with the commission and the secretariat ringed as the participants without whom
// authority falls apart; a seat chosen, the round read from it (every other pair marked present, absent or not observed) and its card
// with its position and what the record shows of it; a layer switched off; a knot on the strands opened, the round read on its date.
// The partnership: mutual benefit failing at Porthaven, party by party; a commitment opened with its screen; the fishers' benefit
// untenable; the signs. Governance: the model's decision faulted three ways; the steering committee's double majority, in which the
// Upland Republic's 25 votes give it no more power than the chair's 5; the Security Council's rule. Negotiation: the assembly's members
// on a line; Nash and Kalai–Smorodinsky; the issues divided. The Migration MPTF: its round, the fund ringed; its commitments with no
// indicator. The page by night. Pictures go to the folder given.
//
// The affairs (ADR 0134), all invented: beneath the round, what passed between the Upland Republic and the Delta Union, a wire carried
// twelve times counted once; on governance, the first tranche turning on whether the Bank concurred by abstaining, the mandates in time
// and the review cycles; the negotiation opening on the dry-season talks' zone, draft by draft; the institutions' lattice and reciprocity.
//
//   node applications/multilateral-phronimos/tests/drive.mjs [http://127.0.0.1:9210/] [folder for pictures]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const args = process.argv.slice(2).filter((a) => !a.startsWith('--'));
const BASE = args[0] || 'http://127.0.0.1:9210/';
const OUT = args[1] || join(tmpdir(), 'multilateral-phronimos-drive');
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
const basin = (await post('api/projects', { title: 'The Arda Basin Partnership' })).name;
await post(`api/projects/${basin}/example`, { template: 'charter/basin-partnership' });
const fund = (await post('api/projects', { title: 'The Migration MPTF' })).name;
const laidFund = await post(`api/projects/${fund}/example`, { template: 'migration/migration-fund' });

const page = await open(`${BASE}?project=${basin}`, { port: 9395, dir: join(tmpdir(), `multilateral-chrome-${Date.now()}`), width: 1600, height: 1000 });
const shot = async (name) => { await sleep(500); await page.shot(join(OUT, `${name}.jpg`)); };
const tab = async (name, ready) => { await page.eval(`document.querySelector('.tab[data-tab="${name}"]').click()`); await page.until(`document.querySelector('#pane-${name} ${ready}')`, 60); await sleep(400); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const click = (js) => page.eval(`(() => { const x = ${js}; if (!x) return false; x.dispatchEvent(new MouseEvent('click', { bubbles: true })); return true; })()`);
const setValue = (js, value) => page.eval(`(() => { const x = ${js}; if (!x) return false; x.value = ${JSON.stringify(value)}; x.dispatchEvent(new Event('change', { bubbles: true })); return true; })()`);
try {
  // ---- the round ----
  await page.until(`document.querySelector('#pane-round svg.mr-round')`, 90);
  assert('the round opens first, twelve seats on a ring, read on the record\'s date', await page.eval(`document.querySelectorAll('#pane-round .mr-seat').length === 12`) && /2025-12-31/.test(await page.eval(`document.querySelector('#pane-round .mp-pick').value`)));
  assert('the commission and the secretariat are ringed: without either the relations of authority fall apart', await page.eval(`document.querySelectorAll('#pane-round .mr-cut').length === 4`));
  assert('the strands beneath hold the record\'s changes', await page.eval(`document.querySelectorAll('#pane-round .ms-knot').length >= 20`), String(await page.eval(`document.querySelectorAll('#pane-round .ms-knot').length`)));
  await shot('multilateral-01-round');
  await click(`document.querySelector('#pane-round .mr-seat[data-participant="fishers"]')`);
  await page.until(`/fishers/.test(document.querySelector('#pane-round .mp-card h3')?.textContent || '')`, 20);
  const card = await text('#pane-round .mp-card');
  assert('a seat chosen opens its card: the fishers hold no authority, are affected by the first tranche, and have no route to its venue', /holds no authority recorded/.test(card) && /affected by/.test(card) && /no route of challenge/.test(card), card.slice(0, 400));
  const marks = await page.eval(`[...document.querySelectorAll('#pane-round .mr-pair')].map((m) => m.dataset.status)`);
  assert('read from their seat, every other pair marked for each of five layers, absent or not observed: the outreach ended in March, the commission\'s silence is recorded absent',
    marks.length === 11 * 5 && marks.includes('absent') && marks.includes('not observed') && !marks.includes('present'), `${marks.length} marks`);
  await shot('multilateral-02-seat');
  await click(`document.querySelector('#pane-round .mr-key[data-type="relies"]')`);
  await page.until(`document.querySelector('#pane-round .mr-key.off[data-type="relies"]')`, 20);
  assert('a layer switched off at its key: no thread of reliance is drawn', await page.eval(`![...document.querySelectorAll('#pane-round .mr-thread')].some((t) => t.dataset.type === 'relies')`));
  await click(`document.querySelector('#pane-round .mr-key[data-type="relies"]')`);
  await page.until(`!document.querySelector('#pane-round .mr-key.off')`, 20);
  await click(`[...document.querySelectorAll('#pane-round .ms-knot')].find((k) => k.dataset.kind === 'decision' && k.dataset.date === '2024-09-02')`);
  await page.until(`document.querySelector('#pane-round .mp-pick').value === '2024-09-02'`, 30);
  assert('a knot opened: the decision of 2 September 2024, and the round read on its date', /dredging/.test(await text('#pane-round .mp-card')));
  await shot('multilateral-03-knot');
  await setValue(`document.querySelector('#pane-round .mp-pick')`, '2025-12-31');
  await page.until(`[...document.querySelectorAll('#pane-round .mr-centre')].some((t) => t.textContent === 'on 2025-12-31')`, 30);
  await page.until(`document.querySelector('#pane-round svg.ma-events')`, 30);
  await page.eval(`document.querySelector('#pane-round svg.ma-events').scrollIntoView({ block: 'center' })`);
  const ev = await text('#pane-round svg.ma-events');
  assert('beneath the round, what passed between the Upland Republic and the Delta Union, month by month, the twelve-fold wire counted once',
    /Between/.test(ev) && await page.eval(`document.querySelectorAll('#pane-round .ma-period').length >= 5`) && /overstate/.test(await text('#pane-round')), ev.slice(0, 160));
  await shot('multilateral-15-events');

  // ---- the partnership ----
  await tab('partnership', 'svg.ml-commitments');
  const answer = await text('#pane-partnership .answer');
  assert('the partnership does not benefit every party, and fails at Porthaven, whose 75 bathing days the evidence leaves open (P = 0.16)', /does not/.test(answer) && /Porthaven/.test(answer) && /0\.1587/.test(answer), answer);
  assert('four commitments, each with its state in a glyph and a word', await page.eval(`document.querySelectorAll('#pane-partnership .ml-row').length === 4`) && /honoured/.test(await text('#pane-partnership svg.ml-commitments')) && /overdue/.test(await text('#pane-partnership svg.ml-commitments')));
  await shot('multilateral-04-partnership');
  await click(`document.querySelector('#pane-partnership .ml-row[data-commitment="c-dredge"]')`);
  await page.until(`document.querySelector('#pane-partnership .mp-card.wide')`, 20);
  const cc = await text('#pane-partnership .mp-card.wide');
  assert('the dredging opened: not measurable, no resource secured, and nothing reported', /measurable/.test(cc) && /names no indicator|no indicator/.test(cc) && /none secured|names no resources/.test(cc) && /unreported, not failed/.test(cc), cc.slice(0, 300));
  await page.eval(`document.querySelector('#pane-partnership .mp-card.wide').scrollIntoView({ block: 'center' })`);
  await shot('multilateral-05-commitment');
  await click(`document.querySelector('#pane-partnership .ml-party[data-party="fishers"]')`);
  await page.until(`/a catch to live on/.test(document.querySelector('#pane-partnership .mp-card.wide')?.textContent || '')`, 20);
  assert('the fishers\' benefit is untenable: 500 tonnes asked, at most 450 allowed by the evidence', /untenable/.test(await text('#pane-partnership .mp-card.wide')));
  await page.eval(`document.querySelector('#pane-partnership svg.ml-benefits').scrollIntoView({ block: 'start' })`);
  await shot('multilateral-06-benefits');
  await page.eval(`document.querySelector('#pane-partnership .mp-signs').scrollIntoView({ block: 'start' })`);
  const signs = await text('#pane-partnership .mp-signs');
  assert('the signs: resource fragility, an accountability gap, succession loss at Fordwick, inertia in the monitoring',
    ['resource fragility', 'accountability gap', 'succession loss', 'collaborative inertia'].every((m) => signs.includes(m)), signs.slice(0, 300));
  await shot('multilateral-07-signs');

  // ---- governance ----
  await tab('governance', '.mp-decision');
  const d2 = await page.eval(`document.querySelector('#pane-governance .mp-decision[data-decision="d2"]').textContent`);
  assert('the model\'s decision is faulted: no authority, no venue, resting on itself twice', /holding no authority/.test(d2) && /names no venue/.test(d2) && /more than once/.test(d2), d2.slice(0, 300));
  await page.until(`document.querySelector('#pane-governance svg.mp-power')`, 60);
  const power = await text('#pane-governance svg.mp-power');
  assert('the steering committee\'s double majority: the bank holds 0.4, and the Upland Republic\'s 25 votes give it no more than the chair\'s 5 (0.15 each, drawn as one row of four alike)', /and 3 alike/.test(power) && /0\.4/.test(power), power.slice(0, 200));
  await shot('multilateral-08-governance');
  await click(`[...document.querySelectorAll('#pane-governance .chip')].find((c) => /Security Council/.test(c.textContent))`);
  await page.until(`/veto/.test(document.querySelector('#pane-governance svg.mp-power')?.textContent || '')`, 60);
  assert('the Security Council by Article 27(3): five vetoes, ten elected members alike', /and 4 alike/.test(await text('#pane-governance svg.mp-power')) && /and 9 alike/.test(await text('#pane-governance svg.mp-power')));
  await page.eval(`document.querySelector('#pane-governance .mp-rule').scrollIntoView({ block: 'start' })`);
  await shot('multilateral-09-council');
  await page.until(`document.querySelector('#pane-governance svg.ma-outcome')`, 30);
  await page.eval(`document.querySelector('#pane-governance svg.ma-outcome').scrollIntoView({ block: 'center' })`);
  const q = await text('#pane-governance svg.ma-outcome');
  assert('the first tranche turns on whether the Bank concurred by abstaining: five seats, the readings with what each gives', /turns on the readings/.test(q)
    && await page.eval(`document.querySelectorAll('#pane-governance .ma-seat').length === 5`) && /no vote against/.test(q) && /recorded as adopted/.test(await text('#pane-governance')), q.slice(0, 200));
  await shot('multilateral-16-question');
  await page.eval(`document.querySelector('#pane-governance svg.ma-mandates').scrollIntoView({ block: 'center' })`);
  assert('the mandates in time: the monitoring renewed, the flood warning asked by the fishers, who hold no authority',
    await page.eval(`document.querySelectorAll('#pane-governance .ma-mandate').length === 3`) && /fishers gave it holding no authority/.test(await text('#pane-governance')));
  await shot('multilateral-17-mandates');
  await page.eval(`document.querySelector('#pane-governance svg.ma-reviews').scrollIntoView({ block: 'center' })`);
  assert('two review cycles: the releases claimed and recommended again, the abstraction contested', await page.eval(`document.querySelectorAll('#pane-governance .ma-recommendation').length === 7`)
    && /contested/.test(await text('#pane-governance svg.ma-reviews')) && await page.eval(`document.querySelectorAll('#pane-governance .ma-repeat').length === 1`));
  await shot('multilateral-18-reviews');

  // ---- negotiation ----
  await tab('negotiation', 'svg.mp-bargaining');
  await page.until(`document.querySelector('#pane-negotiation svg.ma-zone')`, 30);
  const zone = await text('#pane-negotiation svg.ma-zone');
  assert('the dry-season talks open the negotiation: twenty-five packages, one arguable, four drafts, the zone emptied at the second chair\'s draft',
    await page.eval(`document.querySelectorAll('#pane-negotiation .ma-package').length === 25 && document.querySelectorAll('#pane-negotiation .ma-package[data-agreement="arguable"]').length === 1 && document.querySelectorAll('#pane-negotiation .ma-draft').length === 4`)
      && /emptied/.test(zone), zone.slice(0, 200));
  await shot('multilateral-19-talks');
  await page.until(`document.querySelector('#pane-negotiation svg.mp-line')`, 90);
  assert('the assembly\'s fourteen members on a line, the anchor marked', await page.eval(`document.querySelectorAll('#pane-negotiation .mp-point').length === 14 && !!document.querySelector('#pane-negotiation .mp-point.anchor')`));
  assert('a bargain solved: Nash, Kalai–Smorodinsky and the egalitarian solution named', /Nash/.test(await text('#pane-negotiation .mp-bargain')) && /Kalai–Smorodinsky/.test(await text('#pane-negotiation .mp-bargain')));
  await page.until(`/Each holds/.test(document.querySelector('#pane-negotiation .mp-divide .said')?.textContent || '')`, 30);
  assert('the issues divided: equal points, one issue split', /Each holds/.test(await text('#pane-negotiation .mp-divide')) && await page.eval(`document.querySelectorAll('#pane-negotiation .sh.split').length === 1`));
  await shot('multilateral-10-negotiation');
  await page.eval(`document.querySelector('#pane-negotiation .mp-divide').scrollIntoView({ block: 'end' })`);
  await shot('multilateral-11-division');

  // ---- institutions ----
  await tab('institutions', 'svg.ma-lattice');
  assert('the institutions: a lattice of groups, the regularities in words, the complex on water, reciprocity over three windows',
    await page.eval(`document.querySelectorAll('#pane-institutions .ma-group').length >= 4 && document.querySelectorAll('#pane-institutions .ma-window').length === 3`)
      && /every party belongs to the Arda Basin Partnership/.test(await text('#pane-institutions svg.ma-lattice')) && /overlap/.test(await text('#pane-institutions')));
  await shot('multilateral-20-institutions');

  // ---- the Migration MPTF ----
  if (laidFund.title) {
    await page.eval(`location.href = ${JSON.stringify(`${BASE}?project=${fund}`)}`);
    await page.until(`document.querySelector('#pane-round svg.mr-round')`, 90);
    await sleep(800);
    assert('the Migration MPTF: 56 seats, the fund ringed as the one participant without whom its authority falls apart', await page.eval(`document.querySelectorAll('#pane-round .mr-seat').length === 56 && document.querySelectorAll('#pane-round .mr-cut').length === 2`));
    await shot('multilateral-12-migration');
    await tab('partnership', 'svg.ml-commitments');
    assert('its fifteen joint programmes, none with an indicator in the publication', await page.eval(`document.querySelectorAll('#pane-partnership .ml-row').length === 15`) && /no indicator/.test(await text('#pane-partnership svg.ml-commitments')));
    await shot('multilateral-13-migration-commitments');
  }

  // ---- by night ----
  await click(`document.getElementById('theme')`);
  await tab('round', 'svg.mr-round');
  await shot('multilateral-14-night');
  assert('no error on the page', page.errors.length === 0, page.errors.join(' | '));
} catch (error) {
  assert('the drive ran to the end', false, error.message);
  await shot('multilateral-failure').catch(() => {});
}
await page.close();
console.log(`${passed} passed, ${failures} failed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
