// The plug-in From evidence to action driven as an analyst, a resident and a lead would use it, in headless Chrome (not in the suite): a
// project begun from the template Freetown learning from Miyako, its plan run from the page (four walks out of the hazards, each run
// answering evidence to the project's ledger); the engagement laid out from the example, the ledger gathered and the turn run; the
// findings, the belief and the policy opened in place; the belief amended and the turn run again; a resident's proposal at the decision,
// accepted by a lead; a branch under another standard, its turn entered and run; the record verified, replayed and two turns compared;
// an act on an engagement changed meanwhile, refused with the engagement as it stands; the practice's transfer assessed and drawn; the
// page by night. A picture of each.
//   node applications/urban-studio/tests/drive-engagement.mjs <base> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { mkdirSync } from 'node:fs';

const [BASE = 'http://127.0.0.1:9090/', OUT = join(tmpdir(), 'urban-engagement-drive')] = process.argv.slice(2);
mkdirSync(OUT, { recursive: true });
const origin = BASE.replace(/\/$/, '');
const post = async (path, body) => (await fetch(`${BASE}${path}`, { method: 'POST', headers: { 'Content-Type': 'application/json', Origin: origin }, body: JSON.stringify(body) })).json();
const get = async (path) => (await fetch(`${BASE}${path}`)).json();
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let passed = 0, failures = 0;
const assert = (name, ok, detail = '') => { ok ? passed++ : failures++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
for (let i = 0; i < 60; i++) {
  try { if ((await fetch(`${BASE}api/status`)).ok) break; } catch { /* not yet */ }
  await sleep(1000);
}

const made = await post('api/projects', { title: 'Freetown learning from Miyako', template: 'engagement.freetown-from-miyako' });
const name = made.name;
assert('the project is begun from the template engagement.freetown-from-miyako', !!name, JSON.stringify(made).slice(0, 200));
const page = await open(`${BASE}?project=${name}`, { port: 9401, dir: join(tmpdir(), `urban-engagement-chrome-${Date.now()}`), width: 1560, height: 1080 });
const P = '#pane-engagement';
const js = (v) => JSON.stringify(v);
// A key pressed by its words, within a pane.
const press = (text, scope = P) => page.eval(`(() => { const b = [...document.querySelectorAll(${js(scope)} + ' button')].find((x) => x.textContent.trim().startsWith(${js(text)}) && !x.disabled); if (!b) return false; b.click(); return true; })()`);
const cell = (block, turn, branch = 'main') => `${P} .kt-cell[data-block="${block}"][data-turn="${turn}"][data-branch="${branch}"]`;
const open_ = (block, turn, branch = 'main') => page.eval(`(() => { const g = document.querySelector(${js(cell(block, turn, branch))}); if (!g) return false; g.dispatchEvent(new MouseEvent('click', { bubbles: true })); return true; })()`);
const stateOf = (block, turn, branch = 'main') => page.eval(`document.querySelector(${js(cell(block, turn, branch))})?.dataset.state || ''`);
const classOf = (block, turn, branch = 'main') => page.eval(`document.querySelector(${js(cell(block, turn, branch))})?.getAttribute('class') || ''`);
const text = (selector) => page.eval(`[...document.querySelectorAll(${js(selector)})].map((x) => x.textContent).join(' | ')`);
const idle = () => page.until(`!document.querySelector('${P} .eg-busy')`, 180);
const role = (r) => page.eval(`(() => { const s = document.querySelector('${P} .eg-role select'); s.value = ${js(r)}; s.dispatchEvent(new Event('change')); return true; })()`);
const reason = (words) => page.eval(`(() => { const i = document.querySelector('${P} .eg-here > .eg-input'); i.value = ${js(words)}; i.dispatchEvent(new Event('input', { bubbles: true })); return true; })()`);
// The k-th value in the editor changed by a function of its JSON.
const edit = (k, fn) => page.eval(`(() => { const t = document.querySelectorAll('${P} .eg-editor textarea')[${k}]; t.value = JSON.stringify((${fn})(JSON.parse(t.value)), null, 1); t.dispatchEvent(new Event('input', { bubbles: true })); return true; })()`);
const shot = async (file, where = null) => {
  // Only the pane scrolls, so that the studio around it stays where it is.
  if (where) await page.eval(`(() => { const e = document.querySelector(${js(where)}), pane = e?.closest('.pane'); if (!e || !pane) return false; pane.scrollTop += e.getBoundingClientRect().top - pane.getBoundingClientRect().top - 8; return true; })()`);
  await sleep(900);
  await page.shot(join(OUT, `${file}.jpg`));
};

try {
  await page.until(`window.studio && window.studio.S.project && Array.isArray(window.studio.S.project.steps) && window.studio.S.tools?.length > 0`, 120);
  await sleep(1500);
  // ---- the plan: the walks out of the hazards, each answering evidence ----
  await page.eval(`window.studio.runPlan()`);
  const n = await page.eval(`window.studio.S.project.plan.length`);
  await page.until(`window.studio.S.project.steps.filter((s) => s.ran || s.error).length >= ${n}`, 3000);
  const steps = await page.eval(`window.studio.S.project.steps.map((s) => s.title + ': ' + (s.error || 'ran'))`);
  assert('the template\'s plan runs: four walks out of the hazards', steps.length >= n && steps.every((s) => /ran$/.test(s)), js(steps));

  // ---- before anything is kept ----
  await page.eval(`window.studio.show('engagement')`);
  await page.until(`document.querySelector('${P} .eg-begin')`, 60);
  assert('the view offers to start from the example or to open an empty engagement', /Start from this example/.test(await text(`${P} .eg-begin`)) && /Open an empty engagement/.test(await text(`${P} .eg-begin`)));
  await shot('engagement-01-begin');

  // ---- the example laid out, the ledger gathered, the turn run ----
  await press('Start from this example');
  await page.until(`document.querySelector('${P} svg.kt-trajectory')`, 180);
  await idle();
  assert('the example lays out the constitution (entered), the practice (observed) and the proposals (entered) at turn 0',
    (await stateOf('Constitution', 0)) === 'entered' && (await stateOf('Practice', 0)) === 'observed' && (await stateOf('Proposals', 0)) === 'entered', `${await stateOf('Constitution', 0)} ${await stateOf('Practice', 0)}`);
  assert('the world is drawn and never in the record', (await stateOf('System', 0)) === 'world' && (await stateOf('Realisation', 0)) === 'world');
  await press('Gather the ledger');
  await idle();
  assert('the ledger is gathered: Collection observed at turn 0', (await stateOf('Collection', 0)) === 'observed', await text(`${P} .eg-said`));
  await press('Run the turn');
  await idle();
  const computed = await page.eval(`document.querySelectorAll('${P} .kt-cell[data-turn="0"][data-branch="main"][data-state="computed"]').length`);
  assert('the turn is run: the seven blocks the predefined methods fill are computed', computed === 7, `${computed} computed; ${await text(`${P} .eg-said`)}`);
  assert('the record is sealed, and the next turn is offered', /sealed/.test(await text(`${P} .eg-seal`)) && /turn 1 · next/.test(await text(`${P} .eg-chip`)));
  await shot('engagement-02-turn');

  // ---- the findings, the belief and the policy opened in place ----
  await open_('Evaluation', 0);
  await page.until(`document.querySelectorAll('${P} .eg-card .eg-finding').length === 3`, 20);
  const need = await text(`${P} .eg-finding[data-operator="need-out-in-time"]`), asset = await text(`${P} .eg-finding[data-operator="asset-reach"]`);
  assert('Evaluation opens beside the lattice with its findings: the need of getting out in time arguable, turning on "leaving"; reaching a refuge clear',
    /◐ arguable/.test(need) && /Turns on leaving/.test(need) && /✓ holds/.test(need) && /✕ fails/.test(need) && /✓ clear/.test(asset) && /declared for the example/.test(need), need.slice(0, 300));
  assert('the opened cell is ringed in the lattice', await page.eval(`!!document.querySelector('${P} .kt-ring')`));
  await shot('engagement-03-evaluation', `${P} .eg-stage`);
  await open_('Estimation', 0);
  await page.until(`document.querySelectorAll('${P} .eg-card .eg-quantity').length === 4`, 20);
  assert('Estimation opens with each quantity of each place, its intervals drawn on a scale', await page.eval(`document.querySelectorAll('${P} .eg-card .eg-scale').length >= 6`) && /evacuation_share_by_warning @ freetown/.test(await text(`${P} .eg-card`)));
  await shot('engagement-04-estimation', `${P} .eg-stage`);
  await open_('PolicyMaking', 0);
  await page.until(`document.querySelector('${P} .eg-card .eg-practice')`, 20);
  const pol = await text(`${P} .eg-card`);
  assert('PolicyMaking opens with the needs taken up, the practice that answers them and what is open', /Needs taken up/.test(pol) && /Leaving at once/.test(pol) && /Assess its transfer/.test(pol) && /the term leaving/.test(pol), pol.slice(0, 300));
  await shot('engagement-05-policy', `${P} .eg-stage`);

  // ---- an amendment, and the turn run again from it ----
  await role('analyst');
  await open_('Estimation', 0);
  await page.until(`document.querySelector('${P} .eg-card .eg-quantity')`, 20);
  await press('Amend');
  await page.until(`document.querySelector('${P} .eg-editor textarea')`, 10);
  await edit(0, `(v) => { const q = v['evacuation_share_by_warning @ freetown']; const k = Object.keys(q.by_readings).find((s) => s.includes('as in 2011')); Object.assign(q.by_readings[k], { low: 0.85, high: 0.96 }); return v; }`);
  await reason('the routes may be slower than the model assumes: the interval is widened');
  await press('Amend', `${P} .eg-editor`);
  await idle();
  assert('the belief is amended, and what rested on it is marked stale', (await stateOf('Estimation', 0)) === 'amended' && /kt-mark-stale/.test(await classOf('Evaluation', 0)), `${await stateOf('Estimation', 0)}; ${await text(`${P} .eg-said`)}`);
  await shot('engagement-06-amended');
  await press('Run again from it');
  await idle();
  await open_('Evaluation', 0);
  await page.until(`document.querySelector('${P} .eg-finding[data-operator="need-out-in-time"]')`, 20);
  assert('run again from turn 0, Evaluation rests on the amendment: under "as in 2011" the need is now open', !/kt-mark-stale/.test(await classOf('Evaluation', 0)) && /◐ open/.test(await text(`${P} .eg-finding[data-operator="need-out-in-time"]`)));

  // ---- a resident's proposal, and a lead's review ----
  await role('resident');
  await open_('Decision', 0);
  await page.until(`document.querySelector('${P} .eg-card')`, 20);
  assert('a resident is told that a fill at the decision enters as a proposal', /as a proposal: the rules empower lead/.test(await text(`${P} .eg-card-acts`)));
  await press('Fill');
  await page.until(`document.querySelector('${P} .eg-editor textarea')`, 10);
  await edit(0, `(v) => ({ advice: 'warn by the neighbourhood wardens, and walk to the schools on high ground' })`);
  await reason('residents say the wardens are heard before any siren');
  await press('Enter', `${P} .eg-editor`);
  await idle();
  assert('the resident\'s fill stands as a proposal awaiting a review', /kt-mark-proposal/.test(await classOf('Decision', 0)) && /a proposal/.test(await text(`${P} .eg-said`)), await text(`${P} .eg-said`));
  await shot('engagement-07-proposal', `${P} .eg-stage`);
  await role('lead');
  await page.until(`document.querySelector('${P} .eg-proposal')`, 10);
  await reason('the wardens are how Freetown is warned');
  await press('✓ Accept', `${P} .eg-proposal`);
  await idle();
  assert('a lead accepts it: the decision is accepted, the proposal no longer waits', (await stateOf('Decision', 0)) === 'accepted' && !/kt-mark-proposal/.test(await classOf('Decision', 0)), await text(`${P} .eg-said`));
  await shot('engagement-08-accepted', `${P} .eg-stage`);

  // ---- a branch under another standard ----
  await role('analyst');
  await press('turn 1 · next');
  await press('Take a branch');
  await page.until(`document.querySelector('${P} .eg-form input')`, 10);
  await page.eval(`(() => { const [a, b] = document.querySelectorAll('${P} .eg-form input'); a.value = 'standard-85'; b.value = 'another standard: 85% safe by the warning'; return true; })()`);
  await press('Take it');
  await idle();
  assert('a branch is taken at turn 1 and drawn as a line parting from main', await page.eval(`!!document.querySelector('${P} .kt-lane[data-branch="standard-85"][data-parent="main"]')`) && (await stateOf('Constitution', 0, 'standard-85')) === 'entered'
    && /kt-mark-inherited/.test(await classOf('Constitution', 0, 'standard-85')), await text(`${P} .eg-said`));
  await open_('Constitution', 1, 'standard-85');
  await press('Fill');
  await page.until(`document.querySelector('${P} .eg-editor textarea')`, 10);
  assert('an empty cell\'s editor is prefilled from the block\'s last value on its branch', /Prefilled from turn 0/.test(await text(`${P} .eg-editor`)));
  await edit(2, `(v) => v.map((s) => (s.id === 'target-safe' ? { ...s, value: 0.85, held_by: 'the analyst, for comparison' } : s))`);
  await reason('another standard, for comparison');
  await press('Enter', `${P} .eg-editor`);
  await idle();
  for (const [block, act] of [['Practice', 'Observe'], ['Proposals', 'Fill']]) {
    await open_(block, 1, 'standard-85');
    await press(act);
    await page.until(`document.querySelector('${P} .eg-editor textarea')`, 10);
    await reason('as at turn 0');
    await press(act === 'Observe' ? 'Enter as observed' : 'Enter', `${P} .eg-editor`);
    await idle();
  }
  await press('Gather the ledger');
  await idle();
  await press('Run the turn');
  await idle();
  const onBranch = await page.eval(`document.querySelectorAll('${P} .kt-cell[data-turn="1"][data-branch="standard-85"][data-state="computed"]').length`);
  await open_('Evaluation', 1, 'standard-85');
  await page.until(`document.querySelector('${P} .eg-finding[data-operator="need-out-in-time"]')`, 20);
  assert('the branch\'s turn is entered and run: under a standard of 85% the need of getting out in time is untenable', onBranch === 7 && /✕ untenable/.test(await text(`${P} .eg-finding[data-operator="need-out-in-time"]`)),
    `${onBranch} computed; ${await text(`${P} .eg-said`)}`);
  await shot('engagement-09-branch', `${P} .eg-stage`);

  // ---- revisiting ----
  await press('Verify');
  await page.until(`document.querySelector('${P} .eg-answer .eg-verdict')`, 60);
  assert('the record verifies, its head shown', /the record verifies/.test(await text(`${P} .eg-answer`)) && await page.eval(`(document.querySelector('${P} .eg-head-digest')?.textContent || '').length === 64`));
  await press('Replay');
  await page.until(`/replays/.test(document.querySelector('${P} .eg-answer')?.textContent || '')`, 60);
  assert('every computed value replays', /every computed value replays/.test(await text(`${P} .eg-answer .eg-verdict`)), await text(`${P} .eg-answer .eg-verdict`));
  await page.eval(`(() => { const [a, b] = document.querySelectorAll('${P} .eg-diff select'); a.value = 'main:0'; a.dispatchEvent(new Event('change')); b.value = 'standard-85:1'; b.dispatchEvent(new Event('change')); return true; })()`);
  await press('Compare');
  await page.until(`/differ/.test(document.querySelector('${P} .eg-answer')?.textContent || '')`, 60);
  assert('the difference between main at turn 0 and the branch at turn 1 names each value that differs', /Constitution\.standards/.test(await text(`${P} .eg-answer`)) && /Evaluation\.evaluations/.test(await text(`${P} .eg-answer`)));
  await shot('engagement-10-revisiting');

  // ---- an act on an engagement changed meanwhile ----
  // The page hears another's act and reads the engagement again; to act on what it read before, it stops listening first.
  await page.eval(`(() => { window.studio.room.name = null; return true; })()`);
  await sleep(500);
  const now = await get(`api/projects/${name}/engagement`);
  const behind = await post(`api/projects/${name}/engagement/gather`, { turn: 1, branch: 'standard-85', role: 'analyst', base: now.sha256 });
  assert('meanwhile someone else gathers the ledger again', !!behind.sha256 && behind.sha256 !== now.sha256);
  await press('Gather the ledger');
  await idle();
  assert('the page\'s act, made on what it read before, is refused and the engagement shown as it stands', /acted on the engagement since you read it/.test(await text(`${P} .eg-said.conflict`)), await text(`${P} .eg-said`));
  await shot('engagement-11-conflict');

  // ---- the practice's transfer, from the policy (read on the branch from main) ----
  await open_('PolicyMaking', 0, 'standard-85');
  await page.until(`document.querySelector('${P} .eg-card .eg-practice')`, 20);
  await press('Assess its transfer');
  await page.until(`document.querySelector('#pane-transfer .eg-chip.on')`, 30);
  await press('Assess', '#pane-transfer');
  await page.until(`document.querySelector('#pane-transfer svg.kx-transfer')`, 120);
  await sleep(500);
  const planks = await page.eval(`[...document.querySelectorAll('#pane-transfer .kx-plank')].map((g) => g.dataset.factor + '=' + g.dataset.standing)`);
  assert('the transfer is drawn: Miyako facing Freetown, three planks, refuges laid, the warning and leaving when warned half laid',
    planks.length === 3 && planks.includes('refuges within reach=clear') && planks.includes('people leave when warned=arguable') && /Miyako/.test(await text('#pane-transfer svg')) && /Freetown/.test(await text('#pane-transfer svg')), js(planks));
  assert('it will work here: arguable, and the assessment is kept among the transfers', /◐ arguable/.test(await text('#pane-transfer .kx-standing')) && await page.eval(`document.querySelectorAll('#pane-transfer .eg-kept-row').length >= 1`));
  await shot('engagement-12-transfer');

  // ---- by night ----
  await page.eval(`document.getElementById('theme').click()`);
  await page.eval(`window.studio.show('engagement')`);
  await page.until(`document.querySelector('${P} svg.kt-trajectory')`, 30);
  await press('main');
  await page.until(`document.querySelector(${js(cell('Evaluation', 0))})`, 10);
  await open_('Evaluation', 0);
  await page.until(`document.querySelector('${P} .eg-card .eg-finding')`, 20);
  await shot('engagement-13-night', `${P} .eg-stage`);
  await page.eval(`window.studio.show('transfer')`);
  await page.until(`document.querySelector('#pane-transfer svg.kx-transfer')`, 30);
  await shot('engagement-14-transfer-night');
  await page.eval(`document.getElementById('theme').click()`);

  assert('the page threw nothing', page.errors.length === 0, page.errors.join(' | '));
} catch (error) {
  assert('the drive ran to its end', false, error.stack || String(error));
  try { await page.shot(join(OUT, 'engagement-failed.jpg')); } catch { /* no page */ }
} finally {
  await page.close();
}
console.log(`\n${passed} passed, ${failures} failed; pictures in ${OUT}; the project ${name}`);
process.exit(failures ? 1 : 0);
