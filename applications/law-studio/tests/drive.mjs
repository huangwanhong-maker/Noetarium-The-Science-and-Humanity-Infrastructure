// The Law Studio's page driven as counsel would use it, in headless Chrome (not in the suite). Two matters on a server started with
// --people: the invented bylaw of the park, small enough to be a drawing, and the invented transfer to the United States under the
// GDPR's Chapter V, large enough to be an outline. The park: the matter as its argument; a fact opened beside itself; the warden's
// log read with its passages marked, a proposal accepted; the camera's note with its broken custody; a passage marked by hand; the
// readings with a foreign court's holding and the parties' arguments; the canons ranked, and the answer changing; the plan of proof.
// The transfer: the outline; a rule opened in place with the words of its clause; the law clause by clause in the Official Journal's
// words; the Court of Justice's readings; the plan; a fact's standing set and the answer following; the matter by night. With
// --model, a language model reads one document and proposes readings (it asks the provider, with the key kept for this person).
// Pictures go to the folder given.
//
//   node applications/law-studio/tests/drive.mjs [http://127.0.0.1:9170/] [folder for pictures] [--model]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const args = process.argv.slice(2).filter((a) => !a.startsWith('--'));
const MODEL = process.argv.includes('--model');
const BASE = args[0] || 'http://127.0.0.1:9170/';
const OUT = args[1] || join(tmpdir(), 'law-studio-drive');
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
const park = (await post('api/projects', { title: 'Council of Avalon v Rider' })).name;
await post(`api/projects/${park}/example`, { template: 'park/the-bicycle' });
const transfer = (await post('api/projects', { title: 'Whelan v Halden Care' })).name;
const laid = await post(`api/projects/${transfer}/example`, { template: 'privacy/a-transfer-to-the-united-states' });
assert('both matters laid out', laid.documents?.length === 4, JSON.stringify(laid));

const page = await open(`${BASE}?project=${park}`, { port: 9391, dir: join(tmpdir(), `law-studio-chrome-${Date.now()}`), width: 1500, height: 940 });
const shot = async (name) => { await sleep(450); await page.shot(join(OUT, `${name}.jpg`)); };
const tab = async (name, ready) => { await page.eval(`document.querySelector('.tab[data-tab="${name}"]').click()`); await page.until(`document.querySelector('#pane-${name} ${ready}')`, 40); await sleep(300); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const click = (js) => page.eval(`(() => { const x = ${js}; if (!x) return false; x.dispatchEvent(new MouseEvent('click', { bubbles: true })); return true; })()`);
try {
  // ---- the park: a matter small enough to draw ----
  await page.until(`document.querySelector('#pane-matter svg.matter')`, 60);
  assert('the matter opens first, drawn as its argument, the offence arguable', (await text('#pane-matter .question')).includes('arguable') && await page.eval(`document.querySelectorAll('svg.matter [data-node]').length >= 10`));
  await shot('law-01-matter');
  await click(`document.querySelector('[data-node="a:motor_assisted"]')`);
  await page.until(`document.querySelector('.card h3')?.textContent === 'motor_assisted'`, 10);
  assert('a fact opens beside itself: its standing, what its evidence gives, where it stands in the plan', /overruled/.test(await text('.card')) && /It is open/.test(await text('.card')));
  await shot('law-02-fact');
  await click(`[...document.querySelectorAll('.card .key')].find((b) => b.textContent.startsWith('Its evidence'))`);
  await page.until(`document.querySelector('#pane-evidence .page mark')`, 40);
  // The warden's log, with the proposal waiting.
  await click(`[...document.querySelectorAll('.doc')].find((b) => b.textContent.includes('warden'))`);
  await page.until(`document.querySelector('.reading-head h2')?.textContent.includes('warden') && document.querySelectorAll('.page mark').length >= 2`, 20);
  assert('the log is read with its passages marked where they stand: one accepted, one proposed and waiting', await page.eval(`document.querySelectorAll('.page mark.accepted').length >= 1 && document.querySelectorAll('.page mark.proposed').length === 1`));
  await shot('law-03-evidence');
  await click(`[...document.querySelectorAll('.cand.proposed .key')].find((b) => b.textContent === 'Accept')`);
  await page.until(`document.querySelectorAll('.cand.proposed').length === 0`, 30);
  assert('accepted, the warden\'s passage stands against the rider\'s denial: the motor\'s evidence is defensible, not justified', /evidence defensible/.test(await page.eval(`[...document.querySelectorAll('.ledger .fact')].find((f) => f.textContent.startsWith('motor_assisted')).textContent`)));
  await click(`[...document.querySelectorAll('.doc')].find((b) => b.textContent.includes('Gate camera'))`);
  await page.until(`document.querySelector('.custody-chain .verdict.fails')`, 20);
  assert('the camera\'s note is an exhibit whose custody breaks: handed on by an office that never held it', /handed over by the enforcement office/.test(await text('.custody-chain .verdict')));
  await shot('law-04-custody');
  // A passage marked by hand in the rider's letter.
  await click(`[...document.querySelectorAll('.doc')].find((b) => b.textContent.includes('Letter'))`);
  await page.until(`document.querySelector('.reading-head h2')?.textContent.includes('Letter')`, 20);
  await page.eval(`(() => { const p = document.querySelector('.page'); const w = document.createTreeWalker(p, NodeFilter.SHOW_TEXT); let n; const want = 'late'; while ((n = w.nextNode())) { const i = n.data.indexOf('ordinary bicycle with seven gears'); if (i >= 0) { const r = document.createRange(); r.setStart(n, i); r.setEnd(n, i + 33); const s = getSelection(); s.removeAllRanges(); s.addRange(r); p.dispatchEvent(new MouseEvent('mouseup', { bubbles: true })); return true; } } return false; })()`);
  await page.until(`document.querySelector('.pick')`, 10);
  await page.eval(`(() => { const [sense, fact] = document.querySelectorAll('.pick select'); sense.value = 'denies'; fact.value = 'motor_assisted'; document.querySelector('.pick input').value = 'Mr T. Rider'; })()`);
  await shot('law-05-mark');
  await click(`[...document.querySelectorAll('.pick .key')].find((b) => b.textContent === 'Mark as evidence')`);
  await page.until(`!document.querySelector('.pick') && [...document.querySelectorAll('.cand')].some((c) => c.textContent.includes('seven gears'))`, 30);
  assert('words selected in a document are marked as evidence against a fact, by hand', true);
  // The readings.
  await tab('readings', '.term');
  assert('the open term with its two readings: a foreign court persuading for the broad one, the home court\'s later holding left out', /persuasive/.test(await text('#pane-readings')) && /Left out/.test(await text('#pane-readings')) && /after the date of the question/.test(await text('#pane-readings')));
  await shot('law-06-readings');
  await page.eval(`(() => { const [strong, weak] = document.querySelectorAll('.ranking select'); strong.value = 'purpose'; weak.value = 'text'; })()`);
  await click(`[...document.querySelectorAll('.ranking .key')].find((b) => b.textContent === 'Rank')`);
  await page.until(`document.querySelector('.ranking li')`, 30);
  const after = await text('#pane-readings .answers');
  assert('with purpose ranked over text the council\'s textual argument no longer defeats the rider\'s; the foreign court still stands for the broad reading, and the offence stays arguable', /arguable/.test(after), after);
  await tab('plan', '.issue-plan');
  assert('the plan of proof: two ways to establish the offence and two to resist it, the emergency to be excluded in every way', await page.eval(`document.querySelectorAll('.lane').length === 2 && document.querySelectorAll('.lane:first-child .ways li').length === 2`) && /Whatever way is taken/.test(await text('.needed')));
  await shot('law-07-plan');

  // ---- the transfer: a matter large enough to be an outline ----
  await page.eval(`location.href = ${JSON.stringify(`${BASE}?project=${transfer}`)}`);
  await page.until(`document.querySelector('#pane-matter .otree .orow')`, 90);
  assert('the transfer opens as an outline, the prohibition arguable, its branches in play unfolded', (await text('#pane-matter .orow.r-issue')).includes('arguable') && await page.eval(`document.querySelectorAll('.orow').length > 12`));
  await shot('law-08-outline');
  await page.eval(`(() => { const r = [...document.querySelectorAll('.orow.k-exception')].find((x) => x.textContent.includes('by safeguards')); r.scrollIntoView({ block: 'start' }); r.dispatchEvent(new MouseEvent('click', { bubbles: true })); })()`);
  await page.until(`document.querySelector('.card.inline blockquote.clause')`, 20);
  assert('an exception opens where it stands, with the words of Article 46(1) from the Official Journal', /only if the controller or processor has provided appropriate safeguards/.test(await text('.card.inline blockquote.clause')));
  await page.eval(`document.querySelector('.card.inline').scrollIntoView({ block: 'center' })`);
  await shot('law-09-clause');
  await click(`document.querySelector('.card.inline blockquote.clause .key')`);
  await page.until(`document.querySelector('#pane-law .provision.on')`, 40);
  assert('from there, the clause in the law: its article, and beside it the rules that state it', (await text('#pane-law .provision.on')).includes('by_safeguards'));
  await shot('law-10-law');
  await tab('readings', '.term');
  assert('the Court of Justice\'s readings bind: identifiability relative (Breyer, EDPS v SRB), safeguards essentially equivalent (Schrems II)', await page.eval(`document.querySelectorAll('.said.holding.binding').length === 3 && document.querySelectorAll('.reading-col.fixed').length === 3`));
  await shot('law-11-readings');
  await page.eval(`document.getElementById('term-chapter_v_view').scrollIntoView({ block: 'start' })`);
  await shot('law-12-open-term');
  await tab('plan', '.issue-plan');
  assert('the plan: four ways to establish the prohibition, three to resist it, and the derogations left out until alleged', await page.eval(`document.querySelectorAll('.lane:first-child .ways li').length === 4 && document.querySelectorAll('.lane:last-child .ways li').length === 3`) && /explicit_consent/.test(await text('#pane-plan .absent')));
  await shot('law-13-plan');
  // Counsel establishes that the measures are effective: the prohibition no longer follows.
  await page.eval(`(() => { const s = document.querySelector('select[aria-label="The standing of supplementary_measures_effective"]'); s.value = 'found'; s.dispatchEvent(new Event('change', { bubbles: true })); })()`);
  await page.until(`/untenable/.test(document.querySelector('.issue-plan header')?.textContent || '')`, 40);
  assert('with the supplementary measures found effective the safeguards hold, and the prohibition is untenable', true);
  await tab('matter', '.otree');
  await shot('law-14-untenable');
  if (MODEL) {
    await tab('evidence', '.page');
    await click(`[...document.querySelectorAll('.doc')].find((b) => b.textContent.includes('Complaint'))`);
    await page.until(`document.querySelector('.reading-head h2')?.textContent.includes('Complaint')`, 20);
    const before = await page.eval(`document.querySelectorAll('.cand').length`);
    await click(`[...document.querySelectorAll('.reading-head .key')].find((b) => b.textContent.includes('model'))`);
    await page.until(`!document.querySelector('.reading-head .key[disabled]') && document.querySelectorAll('.cand').length > ${before}`, 180);
    assert('a model read the complaint: its passages, each found in the text, wait as proposals', await page.eval(`document.querySelectorAll('.cand.proposed').length >= 1`), await text('#console'));
    await shot('law-15-model');
    await tab('readings', '.term');
    await click(`[...document.querySelectorAll('.forum .key')].find((b) => b.textContent.includes('model'))`);
    await page.until(`document.querySelector('.said.proposal')`, 180);
    await page.eval(`document.querySelector('.said.proposal').scrollIntoView({ block: 'center' })`);
    assert('a model proposed readings of the open terms, each kept as a proposal that fixes nothing', await page.eval(`document.querySelectorAll('.said.proposal').length >= 2`));
    await shot('law-16-proposals');
  }
  await tab('matter', '.otree');
  await click(`document.getElementById('theme')`);
  await shot('law-17-night');
  assert('no error was thrown in the page', !(page.errors || []).length, JSON.stringify(page.errors || []));
} catch (error) {
  failures++;
  console.log(`FAIL  the drive stopped: ${error.message}`);
  await page.shot(join(OUT, 'law-fail.jpg'));
  console.log('pane:', (await page.eval(`document.querySelector('.pane:not([hidden])')?.textContent.slice(0, 500)`)));
  console.log('console:', (await text('#console')).slice(-500));
}
await page.close();
console.log(`${passed} of ${passed + failures} steps passed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
