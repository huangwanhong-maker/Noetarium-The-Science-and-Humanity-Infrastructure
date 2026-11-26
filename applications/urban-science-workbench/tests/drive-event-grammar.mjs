// The grammar of Event Spacetime driven in headless Chrome (not in the suite; ADR 0168): the Hoddle Grid's record drawn as its
// complex, the tab "Its grammar" learned and read, and the events coloured by what each cost the grammar when it came; a picture of
// each.
//   node applications/urban-science-workbench/tests/drive-event-grammar.mjs <base> <folder for pictures>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE = 'http://127.0.0.1:8970/', folder = '.'] = process.argv.slice(2);
const page = await open(BASE, { port: 9413, dir: join(tmpdir(), `wb-grammar-${Date.now()}`), width: 1600, height: 1000 });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let failed = 0;
const assert = (name, ok, detail = '') => { if (!ok) failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${ok ? '' : `: ${detail}`}`); };
const W = 'window.workbench';
const text = () => page.eval('document.body.innerText');
try {
  await page.until(`window.workbench && window.workbench.state.plugins?.length > 0`, 120);
  await page.eval(`(async () => { const a = ${W}.state.areas.find((x) => x.id === 'melbourne'); await ${W}.openArea(a, { view: 'jump' }); })()`);
  await page.until(`${W}.state.area?.id === 'melbourne' && !${W}.state.ui.opening`, 180);
  await sleep(6000);
  // The record as a 2-complex, drawn.
  await page.eval(`(async () => { const p = ${W}.state.plugins.find((x) => x.name === 'event-spacetime'); const l = p.layers.find((x) => x.id === 'foam'); await ${W}.toggleLayer(p, l, true); })()`);
  await page.until(`${W}.state.layers.get('event-spacetime/foam')?.handle`, 300);
  await page.until(`(document.getElementById('legend')?.innerText || '').includes('events so far')`, 600);
  // The tool, and its tab "Its grammar".
  await page.eval(`(async () => { const p = ${W}.state.plugins.find((x) => x.name === 'event-spacetime'); await ${W}.openTool(p, p.tools[0]); })()`);
  await page.until(`[...document.querySelectorAll('.ld-tab')].some((b) => b.dataset.name === 'Its grammar')`, 600);
  await page.eval(`[...document.querySelectorAll('.ld-tab')].find((b) => b.dataset.name === 'Its grammar').click()`);
  await page.until(`document.body.innerText.includes('What the grammar foresaw')`, 900);
  const t = await text();
  assert('the grammar learned: what it foresaw, the five grammars compared, how χ is generated, the lives',
    t.includes('undominated') && t.includes('How χ is generated') && t.includes('How a new building is attached') && t.includes('The lives of the volumes'), t.slice(0, 600));
  assert('χ counted from the stars, −26,671', t.includes('−26,671') || t.includes('-26,671'), '');
  await sleep(2500);
  await page.shot(join(folder, 'event-grammar-01-foresaw.jpg'));
  // A heading brought to the top of the panel's scroll, clear of its tabs.
  const scrollTo = (heading) => page.eval(`(() => {
    const h = [...document.querySelectorAll('h3')].find((x) => x.textContent === ${JSON.stringify(heading)});
    h.scrollIntoView({ block: 'start' });
    let p = h.parentElement;
    while (p && !(p.scrollHeight > p.clientHeight + 4 && getComputedStyle(p).overflowY !== 'visible')) p = p.parentElement;
    if (p) p.scrollTop -= 130;
  })()`);
  await scrollTo('How χ is generated');
  await sleep(1200);
  await page.shot(join(folder, 'event-grammar-02-chi-and-births.jpg'));
  // The lives of the street spaces and the laneways opened.
  await page.eval(`for (const d of document.querySelectorAll('details')) { const s = d.querySelector('summary'); if (s && /^(street|laneway): /.test(s.textContent)) d.open = true; }`);
  await scrollTo('The lives of the volumes');
  await sleep(1200);
  const lives = await text();
  assert('the lives of the street spaces: a run shared by the segments of one street', lives.includes('all of Swanston Street') || lives.includes('all of Bourke Street'), '');
  await page.shot(join(folder, 'event-grammar-03-lives.jpg'));
  // The events coloured by what they cost when they came.
  await page.eval(`[...document.querySelectorAll('label.ld-switch')].find((l) => l.textContent.includes('what they cost when they came')).querySelector('input').click()`);
  await page.until(`(document.getElementById('legend')?.innerText || '').includes('the bits it cost when it came')`, 120);
  assert('the layer keyed by what each event cost the grammar of the events before it', (await page.eval(`document.getElementById('legend').innerText`)).includes('12+'));
  await page.eval(`${W}.closePanel && ${W}.closePanel()`);
  await sleep(5000);
  await page.shot(join(folder, 'event-grammar-04-cost.jpg'));
  assert('no exception on the page', page.errors.length === 0, page.errors.join(' | '));
} catch (e) {
  failed++;
  console.log(`FAIL  ${e.message}`);
  try { await page.shot(join(folder, 'event-grammar-failure.jpg')); } catch { /* gone */ }
} finally {
  await page.close();
}
console.log(failed ? `${failed} failed` : 'all passed');
process.exit(failed ? 1 : 0);
