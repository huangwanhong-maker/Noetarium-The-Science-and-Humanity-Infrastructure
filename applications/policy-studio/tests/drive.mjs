// The Policy Studio's page driven as an analyst would use it, in headless Chrome (not in the suite). Two questions: the thirteen
// real trials of the BCG vaccine read for an invented district, and the invented question of free school meals, whose theory has two
// paths. BCG: the theory drawn, with the gap every path passes through; a claim opened where it stands; the forest of the trials
// and the ladder of the grade; a trial chosen and the pool without it; a reader's assessment changed and the grade following; the
// crossing to the district through the strata; a premise stated; the options; the decision and what it turns on. School meals: the
// two paths and Wright's rule; the decision to study first with the curve of the study's worth; a claim's support factor named; the
// written form with a line that does not read; the page by night. Then a question of one's own from two sentences, a state added,
// and a claim's studies pasted as a table. With --model, a language model drafts a theory from a description (it asks the provider,
// with the key kept for this person). Pictures go to the folder given.
//
//   node applications/policy-studio/tests/drive.mjs [http://127.0.0.1:9190/] [folder for pictures] [--model]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const args = process.argv.slice(2).filter((a) => !a.startsWith('--'));
const MODEL = process.argv.includes('--model');
const BASE = args[0] || 'http://127.0.0.1:9190/';
const OUT = args[1] || join(tmpdir(), 'policy-studio-drive');
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
const bcg = (await post('api/projects', { title: 'BCG vaccination in the district' })).name;
const laid = await post(`api/projects/${bcg}/example`, { template: 'health/bcg-vaccination' });
const meals = (await post('api/projects', { title: 'Free school meals' })).name;
await post(`api/projects/${meals}/example`, { template: 'schooling/free-school-meals' });
const own = (await post('api/projects', { title: 'Cash for schooling' })).name;
const drafted = (await post('api/projects', { title: 'Home visits for newborns' })).name;
assert('both questions laid out', /BCG/.test(laid.question || ''), JSON.stringify(laid));

const page = await open(`${BASE}?project=${bcg}`, { port: 9393, dir: join(tmpdir(), `policy-studio-chrome-${Date.now()}`), width: 1560, height: 980 });
const shot = async (name) => { await sleep(450); await page.shot(join(OUT, `${name}.jpg`)); };
const tab = async (name, ready) => { await page.eval(`document.querySelector('.tab[data-tab="${name}"]').click()`); await page.until(`document.querySelector('#pane-${name} ${ready}')`, 40); await sleep(300); };
const text = (sel) => page.eval(`document.querySelector(${JSON.stringify(sel)})?.textContent || ''`);
const click = (js) => page.eval(`(() => { const x = ${js}; if (!x) return false; x.dispatchEvent(new MouseEvent('click', { bubbles: true })); return true; })()`);
const setValue = (js, value) => page.eval(`(() => { const x = ${js}; if (!x) return false; x.value = ${JSON.stringify(value)}; x.dispatchEvent(new Event('change', { bubbles: true })); return true; })()`);
try {
  // ---- BCG: real trials, an invented district ----
  await page.until(`document.querySelector('#pane-theory svg.theory')`, 60);
  assert('the theory opens first, drawn from the programme to the cases, and the question is not yet shown: the trials say nothing of whether the programme reaches the newborn',
    /Not yet shown/.test(await text('#pane-theory .answer')) && await page.eval(`document.querySelectorAll('svg.theory [data-node]').length === 4 && document.querySelectorAll('svg.theory [data-link]').length === 3`), await text('#pane-theory .answer'));
  assert('the gap that every path passes through is named on the drawing and beneath it', /a gap on every path|the weakest link/.test(await text('svg.theory')) && /would help most/.test(await text('#pane-theory .help')));
  await shot('policy-01-theory');
  await click(`document.querySelector('svg.theory [data-link="1"]')`);
  await page.until(`document.querySelector('#pane-theory .card h3')`, 10);
  assert('a claim opens where it stands: shown at low, what it rests on, what it needs in the place, and its thirteen studies', /shown at low/.test(await text('#pane-theory .card')) && /13 studies pooled/.test(await text('#pane-theory .card')) && /potency/.test(await text('#pane-theory .card')), await text('#pane-theory .card'));
  await shot('policy-02-claim');
  await click(`[...document.querySelectorAll('#pane-theory .card .key')].find((b) => b.textContent === 'Open its evidence')`);
  await page.until(`document.querySelector('#pane-evidence svg.forest')`, 40);
  const said = await text('#pane-evidence .said');
  assert('the thirteen trials as a forest, pooled to the published value: a risk ratio of 0.489 [0.344, 0.696]', /0\.489 \[0\.344, 0\.696\]/.test(said) && await page.eval(`document.querySelectorAll('svg.forest .row.study').length === 13`), said);
  assert('the ladder of the grade: it starts high, inconsistency lowers it two steps, the large effect raises it one, the reader\'s risk of bias lowers it one, to low',
    /its grade/.test(await text('#pane-evidence .ladder li.final')) && /low/.test(await text('#pane-evidence .ladder li.final')) && await page.eval(`document.querySelectorAll('#pane-evidence ol.steps li.moves').length === 3`), await text('#pane-evidence ol.steps'));
  await shot('policy-03-evidence');
  await click(`document.querySelector('svg.forest [data-study="8"]')`);
  await page.until(`document.querySelector('#pane-evidence .study-card')`, 10);
  assert('a trial chosen opens beneath the forest, with the pool without it', /Without it the others pool to/.test(await text('#pane-evidence .study-card')) && await page.eval(`document.querySelector('.study-name').value.includes('Madras')`));
  await page.eval(`document.querySelector('#pane-evidence .study-card').scrollIntoView({ block: 'center' })`);
  await shot('policy-04-study');
  await setValue(`[...document.querySelectorAll('#pane-evidence .assess select')].find((s) => s.getAttribute('aria-label') === 'Steps for risk of bias')`, '0');
  await page.until(`/moderate/.test(document.querySelector('#pane-evidence .ladder li.final')?.textContent || '')`, 30);
  assert('with the reader\'s step for risk of bias withdrawn, the grade rises to moderate', true);
  await setValue(`[...document.querySelectorAll('#pane-evidence .assess select')].find((s) => s.getAttribute('aria-label') === 'Steps for risk of bias')`, '1');
  await page.until(`/low/.test(document.querySelector('#pane-evidence .ladder li.final')?.textContent || '')`, 30);

  await tab('here', '.crossing');
  const here = await text('#pane-here .crossing');
  assert('the crossing: the pooled ratio of 0.489 there becomes 0.688 here, carried through the trials below 30° of latitude', /0\.489/.test(here) && /0\.688/.test(here) && /carried through the strata/.test(here), here.slice(0, 300));
  assert('the argument is not complete: two support factors are not known here', /not complete/.test(await text('#pane-here .answer')) && await page.eval(`document.querySelectorAll('#pane-here .plank.unknown').length === 2`));
  await shot('policy-05-here');
  await setValue(`[...document.querySelectorAll('#pane-here .plank select')].find((s) => s.getAttribute('aria-label').includes('births are reached'))`, 'stated');
  await page.until(`document.querySelectorAll('#pane-here .plank.unknown').length === 1`, 30);
  assert('a premise stated for the district lays its plank; one is still missing', /not complete/.test(await text('#pane-here .answer')));

  await tab('options', '.option-card');
  const opts = await text('#pane-options');
  assert('the options as ledgers: vaccinating has the greatest expected net present value, and how often each would be best is said', /Vaccinate every newborn has the greatest expected/.test(opts.replace(/✓ /g, '')) && /Best on/.test(opts) && /breaks even when a unit is worth/.test(opts), opts.slice(0, 200));
  await shot('policy-06-options');

  await tab('decision', '.courses');
  const dec = await text('#pane-decision');
  assert('the decision: adopt, though the grade is low; no study of the sizes considered is worth running first', /Adopt has the greatest expected worth/.test(dec.replace(/✓ /g, '')) && /No study of the sizes considered is worth running first/.test(dec), dec.slice(0, 300));
  assert('it turns on the worth of a case averted, within the bounds declared', /turns on the worth of a unit/.test(dec) && await page.eval(`!!document.querySelector('#pane-decision svg.worth-line')`));
  await shot('policy-07-decision');

  // ---- Free school meals: invented, two paths ----
  await page.send('Page.navigate', { url: `${BASE}?project=${meals}` });
  await page.until(`document.querySelector('#pane-theory svg.theory') && document.querySelectorAll('svg.theory [data-node]').length === 5`, 60);
  const ans = await text('#pane-theory .answer');
  assert('two paths lead from the meal to learning; the one through attendance is the strongest, shown at moderate or better', /It is shown/.test(ans) && /2 paths lead/.test(ans) && await page.eval(`document.querySelectorAll('svg.theory .link.strongest').length === 2`), ans);
  assert('every claim states a linear effect, so the paths imply an effect on learning (Wright\'s rule), near the trials\' own 0.069', /0\.0721/.test(await text('#pane-theory .implied')), await text('#pane-theory .implied'));
  await shot('policy-08-two-paths');
  await click(`document.querySelector('svg.theory [data-link="3"]')`);
  await page.until(`document.querySelector('#pane-theory .card h3')`, 10);
  await setValue(`document.querySelector('#pane-theory .card input[aria-label="A support factor to add"]')`, 'lessons are taught in a language the child follows');
  await page.until(`/language the child follows/.test(document.querySelector('#pane-theory .card .factors')?.textContent || '')`, 30);
  assert('a support factor named on a claim is written into the theory and read back by the domain\'s reader', true);
  await shot('policy-09-support');
  await click(`document.querySelector('#pane-theory .card .close')`);

  await tab('decision', '.courses');
  const d2 = await text('#pane-decision');
  assert('the decision is to study first: 18,504 pupils, and the curve of the study\'s worth over its size is drawn', /Study first has the greatest expected worth/.test(d2.replace(/✓ /g, '')) && /18,504/.test(d2) && await page.eval(`!!document.querySelector('#pane-decision svg.study path.worth')`), d2.slice(0, 300));
  await shot('policy-10-study-first');

  await tab('evidence', 'svg.forest');
  assert('six bodies of evidence, one for each claim and one for the whole intervention', await page.eval(`document.querySelectorAll('#pane-evidence .body-slip:not(.add)').length === 6`));
  await shot('policy-11-bodies');

  // The written form, with a line that does not read.
  await tab('theory', 'svg.theory');
  await click(`[...document.querySelectorAll('#pane-theory .legend .key')].find((b) => b.textContent === 'The written form')`);
  await page.until(`document.querySelector('#pane-theory textarea.written')`, 10);
  await page.eval(`(() => { const a = document.querySelector('textarea.written'); a.value += 'link learning -> meals: learning feeds back\\nnode what\\n'; a.dispatchEvent(new Event('input', { bubbles: true })); })()`);
  await page.until(`document.querySelectorAll('#pane-theory .reads li.wrong').length >= 1`, 20);
  assert('in its written form, a line that does not read and a cycle are each named by the domain\'s reader, and nothing is kept', await page.eval(`document.querySelectorAll('#pane-theory .reads li.wrong').length >= 1`), await text('#pane-theory .reads'));
  await shot('policy-12-written');
  await click(`[...document.querySelectorAll('#pane-theory .written-form .key')].find((b) => b.textContent === 'Leave it as it was')`);
  await page.until(`document.querySelector('#pane-theory svg.theory')`, 20);

  // By night.
  await page.eval(`document.getElementById('theme').click()`);
  await sleep(300);
  await shot('policy-13-night');
  await tab('here', '.crossing');
  await shot('policy-14-here-night');
  await page.eval(`document.getElementById('theme').click()`);

  // ---- A question of one's own, from nothing ----
  await page.send('Page.navigate', { url: `${BASE}?project=${own}` });
  await page.until(`document.querySelector('#pane-theory .begin')`, 60);
  assert('a project with no question opens on how to begin: the examples, a question of one\'s own, and a draft that may be asked of a model', await page.eval(`document.querySelectorAll('#pane-theory .begin .choice').length === 2 && !!document.querySelector('#pane-theory .own-row input') && !!document.querySelector('#pane-theory textarea.describe')`));
  await shot('policy-15-begin');
  await page.eval(`(() => { const [a, b] = document.querySelectorAll('#pane-theory .own-row input[type="text"]'); a.value = 'Cash paid to families each month a child is in school'; b.value = 'More children finish primary school'; })()`);
  await click(`[...document.querySelectorAll('#pane-theory .own .key')].find((b) => b.textContent === 'Begin')`);
  await page.until(`document.querySelectorAll('#pane-theory svg.theory [data-node]').length === 2`, 40);
  assert('begun from two sentences: two states and the claim between them, which has no evidence, so nothing is shown yet', /Not yet shown/.test(await text('#pane-theory .answer')) && /no evidence/.test(await text('#pane-theory svg.theory')));
  // A state between, added from the first one's card; then evidence for the claim, pasted as a table.
  await click(`document.querySelector('svg.theory [data-node="cash_paid"]')`);
  await page.until(`document.querySelector('#pane-theory .card input[aria-label="A new state this leads to"]')`, 10);
  await setValue(`document.querySelector('#pane-theory .card input[aria-label="A new state this leads to"]')`, 'Children enrol and attend');
  await page.until(`document.querySelectorAll('#pane-theory svg.theory [data-node]').length === 3`, 30);
  assert('a state added from another\'s card stands in the theory with the claim that leads to it', await page.eval(`document.querySelectorAll('svg.theory [data-link]').length === 2`));
  await click(`document.querySelector('#pane-theory .card .close')`);
  await click(`document.querySelector('svg.theory [data-link="0"]')`);
  await page.until(`[...document.querySelectorAll('#pane-theory .card .key')].some((b) => b.textContent === '+ a body of evidence')`, 10);
  await click(`[...document.querySelectorAll('#pane-theory .card .key')].find((b) => b.textContent === '+ a body of evidence')`);
  await page.until(`document.querySelector('#pane-evidence textarea.csv')`, 40);
  await page.eval(`(() => { const a = document.querySelector('#pane-evidence textarea.csv'); a.value = 'id,label,year,stratum,effect,se\\n1,First trial (invented),2012,,0.021,0.012\\n2,Second trial (invented),2015,,0.034,0.009\\n3,Third trial (invented),2019,,0.027,0.011\\n'; })()`);
  await click(`[...document.querySelectorAll('#pane-evidence .studies .key')].find((b) => b.textContent === 'Read the table')`);
  await page.until(`document.querySelectorAll('#pane-evidence svg.forest .row.study').length === 3`, 40);
  assert('three studies pasted as a table are pooled and graded on the claim they were set on', /3 studies pooled/.test(await text('#pane-evidence .said')) && /its grade/.test(await text('#pane-evidence .ladder li.final')), await text('#pane-evidence .said'));
  await shot('policy-16-pasted');
  await tab('theory', 'svg.theory');
  assert('the claim is now shown on the drawing, at the grade its studies were given; the other still has no evidence', await page.eval(`document.querySelectorAll('svg.theory .link.shown').length === 1 && document.querySelectorAll('svg.theory .link.unshown').length === 1`));
  await shot('policy-17-own');

  if (MODEL) {
    // A language model drafts a theory from a description (it asks the provider, with the key kept for this person).
    await page.send('Page.navigate', { url: `${BASE}?project=${drafted}` });
    await page.until(`document.querySelector('#pane-theory textarea.describe')`, 60);
    await page.eval(`document.querySelector('#pane-theory textarea.describe').value = 'Community health workers visit every household with a newborn in the first week, to check for danger signs and refer sick babies to a clinic, so that fewer newborns die.'`);
    await click(`[...document.querySelectorAll('#pane-theory .draft-box .key')].find((b) => b.textContent === 'Ask for a draft')`);
    await page.until(`document.querySelector('#pane-theory .draft svg.theory')`, 180);
    const draft = await text('#pane-theory .draft');
    assert('a model drafts a theory from a description: it is drawn as a proposal, every claim without evidence, and nothing is kept until it is taken', /A proposal/.test(draft) && await page.eval(`document.querySelectorAll('#pane-theory .draft svg.theory .link.shown').length === 0 && document.querySelectorAll('#pane-theory .draft svg.theory [data-node]').length >= 4`), draft.slice(0, 200));
    await shot('policy-18-draft');
    await click(`[...document.querySelectorAll('#pane-theory .draft .key')].find((b) => b.textContent === 'Take this draft as the theory')`);
    await page.until(`document.querySelector('#pane-theory .ground svg.theory') && /Not yet shown/.test(document.querySelector('#pane-theory .answer')?.textContent || '')`, 60);
    assert('taken, the draft is the theory: the question is asked from its intervention to its impact, and is not yet shown', true);
    await shot('policy-19-draft-taken');
  }

  assert('the page threw nothing', page.errors.length === 0, page.errors.join(' | '));
  if (page.logs.length) console.log(`(console: ${page.logs.slice(0, 5).join(' | ')})`);
} catch (error) {
  failures++;
  console.log(`FAIL  ${error.message}`);
  try { await page.shot(join(OUT, 'policy-failure.jpg')); } catch { /* no picture */ }
  if (page.errors.length) console.log(`(page errors: ${page.errors.join(' | ')})`);
} finally {
  await page.close();
}
console.log(`\n${passed} passed, ${failures} failed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
