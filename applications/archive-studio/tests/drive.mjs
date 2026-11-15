// The Archive Studio's page driven as people would use it, in headless Chrome (not in the suite), against a server started with
// --people: the keeper at this computer and a contributor by the key the keeper gave. A project made and the example archive laid out;
// the wall; the register's facsimile read in deep zoom from the server's IIIF service; the plan described in CIDOC CRM, its maker
// filled along the path through its production; the table's chain of custody with its break; the minutes' coded passages; the
// arrangement with archeion's fault, and the journal verified; a JSON-LD file in the desk; a codebook holding its file to it, a fault
// and the frequencies; two variables harmonised; a QCA run on the rooms compared; a contributor who comes in by a link, seen by the
// keeper; the arrangement by night. Pictures go to the folder given.
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
const origin = BASE.replace(/\/$/, '');
const post = async (path, body) => (await fetch(`${BASE}${path}`, { method: 'POST', headers: { 'Content-Type': 'application/json', Origin: origin }, body: JSON.stringify(body) })).json();

for (let k = 0; k < 120; k++) {   // a server just started may not listen yet
  try { if ((await fetch(`${BASE}api/status`)).ok) break; } catch { /* not yet */ }
  await new Promise((r) => setTimeout(r, 500));
}
const { name } = await post('api/projects', { title: 'Papers of the reading room' });
const laid = await post(`api/projects/${name}/example`, {});
assert('the example archive laid out: eight holdings, ten files of linked data, two codebooks', laid.holdings === 8 && laid.files === 10 && laid.codebooks === 2, JSON.stringify(laid));
// A server driven before may know J. Okafor already: the person is added afresh, so that a key is given.
await post(`api/people/${encodeURIComponent('J. Okafor')}/remove`, {});
const person = await post('api/people', { name: 'J. Okafor', role: 'contributor' });

const page = await open(`${BASE}?project=${name}`, { port: 9361, dir: join(tmpdir(), `archive-studio-chrome-${Date.now()}`), width: 1500, height: 940 });
let other = null;
const openHolding = async (title) => {
  await page.eval(`(() => { const b = [...document.querySelectorAll('.ring button')].find((x) => x.textContent.startsWith('Back')); if (b) b.click(); })()`);
  await page.until(`document.querySelectorAll('.holding').length === 8`, 30);
  await page.eval(`[...document.querySelectorAll('.holding')].find((b) => b.title.startsWith(${JSON.stringify(title)})).click()`);
};
try {
  await page.until(`document.querySelectorAll('.holding').length >= 8`, 60);
  assert('the archive opens first, with its eight holdings on the wall', await page.eval(`document.querySelector('.tab.on')?.dataset.tab === 'archive'`));
  assert('the keeper is told who they are', await page.eval(`document.getElementById('here')?.textContent.includes('M. Ferrand')`));
  // The facsimile, in deep zoom.
  await openHolding('Register of readers');
  await page.until(`window.archiveZoom && window.archiveZoom.loaded().drawn === window.archiveZoom.loaded().tiles`, 60);
  const whole = await page.eval(`JSON.stringify(window.archiveZoom.loaded())`);
  await page.eval(`(() => { const z = window.archiveZoom, v = z.view; z.zoom(5.5, v.toScreen(4100, 2800)); })()`);
  await page.until(`window.archiveZoom.loaded().drawn === window.archiveZoom.loaded().tiles && window.archiveZoom.loaded().factor <= 2`, 60);
  const near = JSON.parse(await page.eval(`JSON.stringify(window.archiveZoom.loaded())`));
  assert('the facsimile is read in tiles from the server\'s IIIF service, finer tiles as it comes closer', near.factor < JSON.parse(whole).factor && near.drawn > 0, `${whole} → ${JSON.stringify(near)}`);
  await page.wait(500);
  await page.shot(join(OUT, 'archive-11-deep-zoom.jpg'));
  // The plan, described in CIDOC CRM.
  await openHolding('Plan');
  await page.until(`document.querySelectorAll('.marks-layer rect').length > 0`, 30);
  assert('the plan opens with its annotated region drawn on it', true);
  await page.eval(`[...document.querySelectorAll('.ring button')].find((b) => b.textContent === 'Describe').click()`);
  await page.until(`document.querySelector('.form-pick')`);
  await page.eval(`(() => { const s = document.querySelector('.form-pick'); s.value = [...s.options].find((o) => o.textContent === 'CIDOC CRM').value; s.dispatchEvent(new Event('change')); })()`);
  await page.until(`document.querySelector('.form-about')?.textContent.includes('production')`, 30);
  await page.until(`[...document.querySelectorAll('.record .row')].some((x) => x.querySelector('.word')?.textContent === 'Made by' && !x.querySelector('.field').disabled)`, 30);
  await page.eval(`(() => { const r = [...document.querySelectorAll('.record .row')].find((x) => x.querySelector('.word')?.textContent === 'Made by'); const i = r.querySelector('.field'); i.value = 'urn:archive:${name}:brandt'; i.dispatchEvent(new Event('change')); })()`);
  // Kept, and the record drawn again from what is kept: the form is there, and its field reads the maker back.
  await page.until(`document.getElementById('console').textContent.includes('The description of') && document.querySelector('.form-about') && [...document.querySelectorAll('.record .row')].find((x) => x.querySelector('.word')?.textContent === 'Made by')?.querySelector('.field').value.includes('brandt')`, 60);
  const crm = await (await fetch(`${BASE}api/projects/${name}/ask`, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ op: 'query', files: ['holdings.ttl'], query: 'PREFIX crm: <http://www.cidoc-crm.org/cidoc-crm/> SELECT ?who WHERE { ?h crm:P108i_was_produced_by ?p . ?p a crm:E12_Production ; crm:P14_carried_out_by ?who }' }) })).json();
  assert('the plan\'s maker kept along the path through its production, which the form made and typed', crm.rows?.length === 1 && crm.rows[0][0].value.endsWith(':brandt'), JSON.stringify(crm).slice(0, 200));
  await page.until(`[...document.querySelectorAll('.record .row')].some((x) => x.querySelector('.word')?.textContent === 'Note' && !x.querySelector('.field').disabled)`, 30);
  await page.wait(300);
  await page.eval(`(() => { const r = [...document.querySelectorAll('.record .row')].find((x) => x.querySelector('.word')?.textContent === 'Note'); const i = r.querySelector('.field'); i.value = 'Drawn for the example, in pencil and ink.'; i.dispatchEvent(new Event('change')); })()`);
  await page.until(`(document.getElementById('console').textContent.match(/The description of/g) || []).length >= 2 && document.querySelector('.form-about') && [...document.querySelectorAll('.record .row')].find((x) => x.querySelector('.word')?.textContent === 'Note')?.querySelector('.field').value.includes('pencil')`, 60);
  await page.shot(join(OUT, 'archive-12-crm-form.jpg'));
  await page.eval(`(() => { const s = document.querySelector('.form-pick'); s.value = 'dc'; s.dispatchEvent(new Event('change')); })()`);
  await page.until(`!document.querySelector('.form-about') && document.querySelector('.record .fields')`, 30);
  await page.eval(`[...document.querySelectorAll('.ring button')].find((b) => b.textContent === 'Describe').click()`);
  await page.until(`!document.querySelector('.ring button.on')`, 30);
  // The table's custody, broken.
  await openHolding('Readers of the reading room');
  await page.until(`document.querySelector('.custody .chain li.broken')`, 30);
  assert('the table\'s chain of custody is broken where it is handed over by someone who never held it, and it is not shown authentic', await page.eval(`document.querySelectorAll('.custody .verdict.fails').length === 2`));
  await page.eval(`document.querySelector('.custody').scrollIntoView({ block: 'center' })`);
  await page.wait(300);
  await page.shot(join(OUT, 'archive-13-custody.jpg'));
  // The minutes, coded by two people.
  await openHolding('Minutes');
  await page.until(`document.querySelectorAll('.viewer .page mark.coded').length >= 5`, 30);
  await page.eval(`[...document.querySelectorAll('.ring button')].find((b) => b.textContent === 'Code').click()`);
  await page.until(`document.querySelector('.coded h4')?.textContent.includes('by')`, 30);
  assert('the minutes carry twelve coded passages, by L. Brandt and J. Okafor', await page.eval(`/12 by L. Brandt and J. Okafor/.test(document.querySelector('.coded h4').textContent)`));
  await page.eval(`document.querySelector('.coded').scrollIntoView({ block: 'center' })`);
  await page.wait(300);
  await page.shot(join(OUT, 'archive-14-coding.jpg'));
  // The arrangement, and the journal.
  await page.eval(`document.querySelector('.tab[data-tab="arrangement"]').click()`);
  await page.until(`document.querySelectorAll('.tree .unit').length === 8`, 30);
  assert('the finding aid: eight units, the file that ends after its series marked as archeion finds it', await page.eval(`document.querySelectorAll('.tree .unit.faulty').length === 1 && document.querySelector('.faults').textContent.includes('1935')`));
  await page.eval(`[...document.querySelectorAll('.tree .line')].find((l) => l.textContent.includes('The register of readers')).click()`);
  await page.until(`document.querySelector('.unit-card h3')?.textContent.includes('register')`);
  await page.shot(join(OUT, 'archive-15-arrangement.jpg'));
  await page.eval(`[...document.querySelectorAll('.journal button')].find((b) => b.textContent.startsWith('Verify')).click()`);
  await page.until(`document.querySelector('.journal .verdict')`, 30);
  assert('the journal verified by archeion: every entry sealed to the one before', await page.eval(`document.querySelector('.journal .verdict').classList.contains('holds')`));
  await page.eval(`document.querySelector('.journal').scrollIntoView({ block: 'start' })`);
  await page.wait(300);
  await page.shot(join(OUT, 'archive-16-journal.jpg'));
  // A JSON-LD file in the desk.
  await page.eval(`document.querySelector('.tab[data-tab="graph"]').click()`);
  await page.until(`document.querySelectorAll('.l-node').length > 10`, 60);
  await page.eval(`[...document.querySelectorAll('.files .key')].find((b) => b.textContent === 'Text').click()`);
  await page.until(`document.querySelector('.leaf.text select')`);
  await page.eval(`(() => { const s = document.querySelector('.leaf.text select'); s.value = 'visit.jsonld'; s.dispatchEvent(new Event('change')); })()`);
  await page.until(`document.querySelector('.leaf.text .code')?.value.includes('@context')`, 30);
  assert('a file of JSON-LD among the linked data, read and drawn with the rest', await page.eval(`[...document.querySelectorAll('.files .chip')].some((c) => c.textContent === 'visit.jsonld')`));
  await page.shot(join(OUT, 'archive-17-jsonld.jpg'));
  // The datasets.
  await page.eval(`document.querySelector('.tab[data-tab="datasets"]').click()`);
  await page.until(`document.querySelectorAll('.variables tr').length > 5`, 30);
  await page.eval(`[...document.querySelectorAll('.hold .key')][0].click()`);
  await page.until(`document.querySelector('.hold .verdict')`, 30);
  await page.eval(`[...document.querySelectorAll('.variables tr')].find((r) => r.textContent.startsWith('satisfied')).click()`);
  await page.until(`document.querySelector('.freq figcaption')?.textContent.startsWith('Satisfied')`, 30);
  assert('the survey held to its codebook: the 7 no code declares is found, and the frequencies drawn with the missing apart', await page.eval(`document.querySelector('.hold .faults')?.textContent.includes('7 is not a code') && document.querySelectorAll('.freq .bar.missing').length >= 1`));
  await page.eval(`document.querySelector('.hold').scrollIntoView({ block: 'start' })`);
  await page.wait(300);
  await page.shot(join(OUT, 'archive-18-codebook.jpg'));
  await page.eval(`[...document.querySelectorAll('.variables tr')].find((r) => r.textContent.startsWith('purpose')).click()`);
  await page.until(`document.querySelector('.harmonise .grid th')?.textContent.startsWith('purpose') && [...document.querySelectorAll('.harmonise .grid th')].some((h) => h.textContent.startsWith('5 teaching'))`, 30);
  // Study may also mean teaching, and family history means genealogy: two ticks beside those the labels already match.
  await page.eval(`(() => { const rows = [...document.querySelectorAll('.harmonise .grid tr')], head = [...rows[0].querySelectorAll('th')];
    const tick = (a, b) => { const r = rows.find((x) => x.querySelector('th')?.textContent.startsWith(a)); const c = head.findIndex((h) => h.textContent.startsWith(b)); const i = r.children[c].querySelector('input'); if (!i.checked) i.click(); };
    tick('1 ', '5 '); tick('2 ', '2 '); })()`);
  await page.eval(`[...document.querySelectorAll('.harmonise .key')].find((b) => b.textContent === 'The common target').click()`);
  await page.until(`document.querySelectorAll('.harmonise .classes li').length >= 4`, 30);
  assert('purpose and reason harmonised: study and teaching go together, four codes in common', await page.eval(`document.querySelectorAll('.harmonise .classes li').length === 4 && document.querySelector('.harmonise .classes li').textContent.includes('teaching')`));
  await page.eval(`document.querySelector('.harmonise').scrollIntoView({ block: 'start' })`);
  await page.wait(300);
  await page.shot(join(OUT, 'archive-19-harmonise.jpg'));
  // A QCA run on the rooms compared.
  await page.eval(`document.querySelector('.tab[data-tab="archive"]').click()`);
  await openHolding('Reading rooms compared');
  await page.until(`[...document.querySelectorAll('.record button')].some((b) => b.textContent.startsWith('Use as a table'))`, 30);
  const sources = await page.eval(`window.studio.S.project.sources.length`);
  await page.eval(`[...document.querySelectorAll('.record button')].find((b) => b.textContent.startsWith('Use as a table')).click()`);
  await page.until(`window.studio.S.project.sources.length > ${sources}`, 30);
  await page.eval(`(async () => { const S = window.studio.S; const t = S.tools.find((x) => x.id === 'qca'); const table = S.project.sources.at(-1).name;
    await window.studio.runTool(t, { table, case: 'room', conditions: 'evenings, catalogue, heating, staffed', outcome: 'well_used', consistency: 0.8, pri: 0.5, frequency: 1, expect: '+ + + +', negate: 'no' }, null); })()`);
  await page.until(`!window.studio.S.job && window.studio.S.project.steps.length > 0`, 120);
  await page.eval(`window.studio.select({ kind: 'table', id: 'solutions' })`);
  await page.until(`document.querySelector('#pane-table table') && /raw_coverage/i.test(document.querySelector('#pane-table').textContent)`, 30);
  assert('QCA finds the two paths the rooms were invented with: catalogue and staff, or evenings and heating', await page.eval(`/catalogue\\*staffed/.test(document.querySelector('#pane-table').textContent) && /evenings\\*heating/.test(document.querySelector('#pane-table').textContent)`));
  await page.shot(join(OUT, 'archive-20-qca.jpg'));
  // A second person, by the key the keeper gave.
  other = await open(`${BASE}?key=${encodeURIComponent(person.key)}&project=${name}`, { port: 9362, dir: join(tmpdir(), `archive-studio-chrome-b-${Date.now()}`), width: 1200, height: 800 });
  await other.until(`document.querySelectorAll('.holding').length >= 8`, 60);
  assert('the contributor comes in by the link, and is told who they are', await other.eval(`document.getElementById('here')?.textContent.includes('J. Okafor')`));
  await page.eval(`document.querySelector('.tab[data-tab="arrangement"]').click()`);
  await page.until(`document.querySelector('#here .other')`, 40);
  assert('the keeper sees the contributor here', await page.eval(`document.querySelector('#here .other').title.startsWith('J. Okafor')`));
  await other.eval(`window.studio.S.project.title = 'Papers of the reading room'; null`);
  await page.eval(`window.studio.peopleDialog()`);
  await page.until(`document.querySelector('#dialog .people li')`, 20);
  await page.shot(join(OUT, 'archive-21-people.jpg'));
  await page.eval(`document.getElementById('dialog').hidden = true`);
  await page.eval(`document.getElementById('theme').click()`);
  await page.wait(800);
  await page.shot(join(OUT, 'archive-22-night.jpg'));
  assert('one view at a time: every other pane is hidden from sight', await page.eval(`[...document.querySelectorAll('#stage .pane')].filter((p) => getComputedStyle(p).display !== 'none').length === 1`));
  assert('no error was thrown on either page', page.errors.length === 0 && other.errors.length === 0, [...page.errors, ...other.errors].join(' | '));
} catch (error) {
  failures++;
  console.log(`FAIL  ${error.message}`);
  await page.shot(join(OUT, 'failure.jpg')).catch(() => {});
  if (page.errors.length) console.log(page.errors.join('\n'));
} finally {
  await page.close();
  if (other) await other.close();
}
console.log(`\n${passed} of ${passed + failures} steps passed; pictures in ${OUT}`);
process.exit(failures ? 1 : 0);
