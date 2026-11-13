// The Dynamics Lab driven in headless Chrome: each example is opened, its studies are waited for, and what the page
// shows is asserted; pictures are taken for the gallery. Not in the suite: it needs Chrome and a built engine.
//
//   node applications/dynamics-lab/tests/drive.mjs [--shots <a directory>] [--only <an example's id>]
//   node applications/dynamics-lab/tests/drive.mjs --gallery <a directory>
//
// With --gallery nothing is asserted: the gallery's pictures are taken, each of an example with one of its studies at
// the head of the sheet and one of its systems selected, the last by night.
//
// It starts the Lab's server on a port of its own and stops it at the end.
import { spawn } from 'node:child_process';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { open } from './cdp.mjs';

const here = dirname(fileURLToPath(import.meta.url)), root = resolve(here, '../../..');
const arg = (name) => { const i = process.argv.indexOf(name); return i > 0 ? process.argv[i + 1] : null; };
const shots = arg('--shots'), only = arg('--only'), gallery = arg('--gallery');
const PORT = 9096, CHROME_PORT = 9397;
if (shots) mkdirSync(shots, { recursive: true });
if (gallery) mkdirSync(gallery, { recursive: true });

// The gallery's pictures: an example, the study at the head of the sheet, the system selected, and the picture's name.
const GALLERY = [
  { example: '01-lorenz', study: 0, name: '01-a-flow-that-is-chaotic' },
  { example: '01-lorenz', study: 2, name: '02-equilibria-and-a-diagram' },
  { example: '02-maps', study: 0, name: '03-a-map-on-its-way-to-chaos' },
  { example: '02-maps', study: 2, name: '04-the-lift-to-an-operator' },
  { example: '03-events', study: 0, system: 'ball', name: '05-events' },
  { example: '04-actions', study: 0, system: 'double_pendulum', name: '06-a-law-given-by-its-action' },
  { example: '05-noise-and-landscapes', study: 1, name: '07-landscapes' },
  { example: '05-noise-and-landscapes', study: 3, system: 'gradient', name: '08-basins-and-slowing-down' },
  { example: '06-grammars', study: 0, name: '09-grammars-as-dynamical-systems' },
  { example: '07-fields-without-a-background', study: -1, system: 'boulatov', name: '10-read-and-not-yet-run' },
  { example: '08-a-diffusion-model', study: 0, name: '11-a-random-model' },
  { example: '09-open-quantum-systems', study: 0, name: '12-open-quantum-systems' },
  { example: '10-braids', study: 1, system: 'stirrers', name: '13-braids-of-trajectories' },
  { example: '10-braids', study: 2, system: 'turning', name: '14-what-a-record-certifies' },
  { example: '11-systems-of-systems', study: 1, system: 'synchronised', name: '15-systems-of-systems' },
  { example: '12-from-a-record-to-a-model', study: 0, name: '16-from-a-record-to-a-model' },
  { example: '12-from-a-record-to-a-model', study: 2, name: '17-matched-against-the-catalogue' },
  { example: '10-braids', study: 0, system: 'vortices', night: true, name: '18-by-night' },
  { example: '13-turbulence', study: 0, name: '19-a-cascade-seen-shell-by-shell' },
  { example: '13-turbulence', study: 1, name: '20-the-spectrum-of-turbulence' },
  { example: '14-joined-by-an-action', study: 0, system: 'joined', name: '21-joined-by-an-action' },
  { example: '15-a-sea-with-life-in-it', study: 1, system: 'sea', name: '22-a-sea-with-life-in-it' },
  { example: '15-a-sea-with-life-in-it', study: 3, system: 'barren', name: '23-with-fish-and-without' },
  { example: '16-behaviours-together', study: 1, system: 'duet', name: '24-who-listens-who-leads' },
  { example: '16-behaviours-together', study: 4, system: 'trio', name: '25-a-third-that-answers' },
];

let passed = 0, failed = 0;
const check = (name, ok, detail = '') => { ok ? passed++ : failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${detail ? `: ${detail}` : ''}`); };

const server = spawn('python', [join(root, 'applications/dynamics-lab/serve.py'), '--port', String(PORT)], { stdio: 'ignore' });
// A drive that has not ended in ten minutes will not: it is stopped, with its server, and said to have failed.
setTimeout(() => { console.log('FAIL  the drive ended within ten minutes'); server.kill(); process.exit(1); }, 600000).unref();
const base = `http://127.0.0.1:${PORT}`;
for (let i = 0; i < 50; i++) { try { await fetch(`${base}/api/status`); break; } catch { await new Promise((r) => setTimeout(r, 200)); } }

// For each example: the studies it opens with, the forms its systems must have, and what its sheet must say.
const EXAMPLES = [
  { id: '01-lorenz', studies: 4, says: ['Kaplan and Yorke', 'saddle', 'Runge-Kutta'], shot: 'a-flow-that-is-chaotic' },
  { id: '02-maps', studies: 4, says: ['0.69', 'iterates on the attractor'], shot: 'maps-and-their-diagram' },
  { id: '03-events', studies: 2, says: ['events'], shot: 'events' },
  { id: '04-actions', studies: 5, says: ["q'' = -g * sin(q) / l", 'Derived from the terms'], shot: 'systems-given-by-an-action' },
  { id: '05-noise-and-landscapes', studies: 5, says: ['Wells at', 'red-noise', 'share of the grid'], shot: 'noise-landscapes-and-slowing-down' },
  { id: '06-grammars', studies: 3, says: ['a a b b', 'more than one history', 'a a b b c c'], shot: 'grammars-as-dynamical-systems' },
  { id: '07-fields-without-a-background', studies: 0, says: [], shot: 'fields-without-a-background' },
  { id: '08-a-diffusion-model', studies: 3, says: ['Euler-Maruyama', 'a map with draws'], shot: 'a-diffusion-model' },
  { id: '09-open-quantum-systems', studies: 4, says: ['the only one', 'exact propagator'], shot: 'open-quantum-systems' },
  { id: '10-braids', studies: 5, says: ['Certified', 'Not certified', 'Alexander polynomial', '1 - 3t + t^2'], shot: 'braids-of-trajectories' },
  { id: '11-systems-of-systems', studies: 5, says: ['merged into one flow', 'co-simulated'], shot: 'systems-of-systems' },
  { id: '12-from-a-record-to-a-model', studies: 6, says: ['7</b> terms', 'least squares', 'rank', "the catalogue's models of this shape"], shot: 'from-a-record-to-a-model' },
  { id: '13-turbulence', studies: 4, says: ['the power law has the exponent', 'flatness', 'largest of the system'], shot: 'turbulence' },
  { id: '14-joined-by-an-action', studies: 4, says: ['joined by an action', 'conserves', 'in_the_spring'], shot: 'joined-by-an-action' },
  { id: '15-a-sea-with-life-in-it', studies: 8, says: ['merged into one flow', 'co-simulated', 'overturning', 'stable focus'], shot: 'a-sea-with-life-in-it' },
  { id: '16-behaviours-together', studies: 7, says: ['pace_of_a', 'answering'], shot: 'behaviours-together' },
].filter((e) => !only || e.id === only);

const page = await open(`${base}/?example=${EXAMPLES[0].id}`, { port: CHROME_PORT, dir: join(tmpdir(), 'dynamics-lab-chrome'), width: 1560, height: 1000 });
try {
  // A run that was stopped may have left its theme in the browser's storage: every example is then opened by day.
  await page.until('document.body.dataset.ready === "1"', 40);
  await page.eval(`localStorage.removeItem('dynamics-lab-theme')`);
  if (gallery) {
    for (const picture of GALLERY) {
      await page.send('Page.navigate', { url: `${base}/?example=${picture.example}` });
      await page.until('document.body.dataset.ready === "1"', 40);
      await page.until(`!document.querySelector('#sheet')?.textContent.includes('Asking the engine')`, 240);
      if (picture.night) await page.eval(`document.documentElement.dataset.theme === 'night' || document.getElementById('theme').click()`);
      if (picture.system) await page.eval(`document.querySelector('.system[data-name="${picture.system}"]')?.click()`);
      if (picture.study >= 0) await page.eval(`(() => { const sheet = document.querySelector('#sheet'), study = document.querySelectorAll('.study')[${picture.study}]; if (study) sheet.scrollTo(0, study.offsetTop - sheet.offsetTop - 10); })()`);
      await page.wait(picture.night ? 2500 : 1800);  // the views in the round
      await page.shot(join(gallery, `dynamics-lab-${picture.name}.jpg`));
      console.log(`taken  dynamics-lab-${picture.name}.jpg`);
      if (picture.night) await page.eval(`localStorage.removeItem('dynamics-lab-theme')`);
    }
    await page.close();
    server.kill();
    process.exit(0);
  }
  for (const example of EXAMPLES) {
    const number = example.id.slice(0, 2);  // a picture keeps its example's number, whichever are driven
    await page.send('Page.navigate', { url: `${base}/?example=${example.id}` });
    await page.until('document.body.dataset.ready === "1"', 40);
    await page.until(`!document.querySelector('#sheet')?.textContent.includes('Asking the engine')`, 240);
    await page.wait(1500);  // the views in the round
    const seen = await page.eval(`({
      studies: document.querySelectorAll('.study').length,
      refused: [...document.querySelectorAll('.study .refused')].map((e) => e.textContent),
      systems: [...document.querySelectorAll('.system')].map((e) => e.textContent.trim().replace(/\\s+/g, ' ')),
      wrong: document.querySelectorAll('.system.wrong').length,
      trouble: document.querySelector('#trouble').textContent,
      text: document.querySelector('#sheet').innerHTML,
      charts: document.querySelectorAll('.study svg.d-chart').length,
      rounds: document.querySelectorAll('.study .round canvas').length,
      math: document.querySelectorAll('#card .katex').length,
    })`);
    check(`${example.id}: the script is read, with no system in error`, seen.trouble === '' && seen.wrong === 0 && seen.systems.length > 0, `${seen.systems.length} systems; ${seen.trouble}`);
    check(`${example.id}: its ${example.studies} studies are answered, none refused`, seen.studies === example.studies && seen.refused.length === 0, `${seen.studies} studies; ${seen.refused.join(' | ')}`);
    for (const phrase of example.says) check(`${example.id}: the sheet says “${phrase}”`, seen.text.includes(phrase));
    if (example.studies) check(`${example.id}: its figures are drawn`, seen.charts + seen.rounds > 0, `${seen.charts} charts, ${seen.rounds} in the round`);
    check(`${example.id}: the page raised no error`, page.errors.length === 0, page.errors.join(' | '));
    page.errors.length = 0;
    if (shots) {
      await page.shot(join(shots, `dynamics-lab-${number}-${example.shot}.jpg`));
      // The rest of the sheet, a screen at a time, for the studies below the first.
      const tall = await page.eval(`document.querySelector('#sheet').scrollHeight > document.querySelector('#sheet').clientHeight + 200`);
      if (tall) {
        await page.eval(`document.querySelector('#sheet').scrollTo(0, document.querySelector('#sheet').clientHeight - 60)`);
        await page.wait(600);
        await page.shot(join(shots, `dynamics-lab-${number}-${example.shot}-further.jpg`));
      }
    }
  }

  // A tool run by hand: select a system, open a tool, run it, and see a study added.
  if (!only) {
    await page.send('Page.navigate', { url: `${base}/?example=01-lorenz` });
    await page.until('document.body.dataset.ready === "1"', 40);
    await page.until(`!document.querySelector('#sheet')?.textContent.includes('Asking the engine')`, 240);
    const before = await page.eval(`document.querySelectorAll('.study').length`);
    await page.eval(`document.querySelector('[data-tool="basins"]').click()`);
    await page.eval(`(() => { const set = (id, v) => { document.getElementById(id).value = v; }; set('f-x0', -20); set('f-x1', 20); set('f-y0', -20); set('f-y1', 20); set('f-nx', 12); set('f-ny', 12); set('f-time', 5); document.getElementById('go').click(); })()`);
    await page.until(`document.querySelectorAll('.study').length === ${before + 1} && !document.querySelector('#sheet').textContent.includes('Asking the engine')`, 120);
    check('a tool opened from the bench and run adds a study', true);
    // A model of the catalogue chosen is added to the script as it is declared there, and read.
    const declared = await page.eval(`document.querySelectorAll('.system').length`);
    await page.eval(`(() => { const m = document.getElementById('models'); m.value = 'van_der_pol'; m.dispatchEvent(new Event('change')); })()`);
    await page.until(`document.querySelectorAll('.system').length === ${declared + 1}`, 20);
    check('a model of the catalogue chosen is added to the script with its source, and read', await page.eval(`document.getElementById('script').value.includes('system van_der_pol {') && document.getElementById('script').value.includes('van der Pol 1926') && document.querySelector('.system[data-name="van_der_pol"]') !== null`));
    // The script changed: the studies are marked.
    await page.eval(`(() => { const s = document.getElementById('script'); s.value = s.value.replace('rho = 28', 'rho = 30'); s.dispatchEvent(new Event('input')); })()`);
    await page.until(`[...document.querySelectorAll('.study .tag')].some((t) => !t.hidden)`, 20);
    check('studies made before the script changed are marked', true);
    // A script that is wrong is said where it is wrong, and what was declared stays listed.
    await page.eval(`(() => { const s = document.getElementById('script'); s.value = s.value.replace("x' = sigma * (y - x)", "x' = sigma (y - x)"); s.dispatchEvent(new Event('input')); })()`);
    await page.until(`document.querySelector('#trouble').textContent.includes('line')`, 20);
    check('a script that is wrong is said where it goes wrong', (await page.eval(`document.querySelector('#gutter .wrong') !== null && document.querySelectorAll('.system').length`)) > 0);
    // Night.
    await page.eval(`document.getElementById('theme').click()`);
    check('the page turns to night', (await page.eval(`document.documentElement.dataset.theme`)) === 'night');
    if (shots) {
      await page.send('Page.navigate', { url: `${base}/?example=10-braids` });
      await page.until('document.body.dataset.ready === "1"', 40);
      await page.until(`!document.querySelector('#sheet')?.textContent.includes('Asking the engine')`, 240);
      await page.wait(1500);
      await page.shot(join(shots, 'dynamics-lab-13-by-night.jpg'));
    }
    await page.eval(`localStorage.removeItem('dynamics-lab-theme')`);
    check('the page raised no error while it was driven', page.errors.length === 0, page.errors.join(' | '));
  }
} catch (error) {
  check('the page was driven to the end', false, error.message);
} finally {
  await page.close();
  server.kill();
}
console.log(`\n${passed} of ${passed + failed} checks passed.`);
process.exit(failed ? 1 : 0);
