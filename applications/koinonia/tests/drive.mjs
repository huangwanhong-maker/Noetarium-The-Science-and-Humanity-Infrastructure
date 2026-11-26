// Koinonia's page driven as a person would use it, in headless Chrome (not in the suite): a society opened, its rounds drawn with
// seats, agents and commitments, an agent's card, a community's card, an entry's card, the page by night. Pictures go to the folder.
//
//   node applications/koinonia/tests/drive.mjs [http://127.0.0.1:9230/] [society] [folder for pictures]

import { open } from './cdp.mjs';
import { mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const args = process.argv.slice(2);
const BASE = args[0] || 'http://127.0.0.1:9230/';
const NAME = args[1] || 'first-two';
const OUT = args[2] || join(tmpdir(), 'koinonia-drive');
mkdirSync(OUT, { recursive: true });
let failures = 0;
const assert = (what, ok, detail = '') => { console.log(`${ok ? 'ok  ' : 'FAIL'}  ${what}${detail ? `: ${detail}` : ''}`); if (!ok) failures++; };
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

const page = await open(`${BASE}`, { port: 9397, dir: join(tmpdir(), `koinonia-chrome-${Date.now()}`), width: 1560, height: 980 });
const shot = async (name) => { await sleep(600); await page.shot(join(OUT, `${name}.jpg`)); };
const click = (js) => page.eval(`(() => { const x = ${js}; if (!x) return false; const r = x.getBoundingClientRect(); x.dispatchEvent(new MouseEvent('click', { bubbles: true, clientX: r.x + r.width / 2, clientY: r.y + r.height / 2 })); return true; })()`);
try {
  await page.eval(`localStorage.setItem('koinonia.society', ${JSON.stringify(NAME)}); location.reload()`);
  await page.until(`document.querySelectorAll('#rounds .seat').length > 4`, 60);
  const rounds = await page.eval(`document.querySelectorAll('#rounds .ring:not(.inner)').length`);
  assert('the society opens as its rounds, one ring a community', rounds >= 2, String(rounds));
  assert('agents are drawn beside the seats of those who lent them', await page.eval(`document.querySelectorAll('#rounds .seat.operative').length >= 4`));
  assert('the work is woven beneath, a cell an entry', await page.eval(`document.querySelectorAll('#weave .cell').length >= 40`));
  await shot('koinonia-01-society');
  await click(`[...document.querySelectorAll('#rounds .seat.operative')].find((s) => /model checker/.test(s.textContent))`);
  await page.until(`!document.querySelector('#card').hidden`, 10);
  assert('an agent opened in place: its sponsor, mandate and record', /lent by/.test(await page.eval(`document.querySelector('#card').textContent`)));
  await shot('koinonia-02-agent');
  await click(`document.querySelector('#focus-head h2')`);
  await page.until(`/sealed/.test(document.querySelector('#card').textContent)`, 10);
  await shot('koinonia-03-community');
  await click(`document.querySelector('#weave .cell.fault') || document.querySelector('#weave .cell.holds')`);
  await page.until(`/hierarchy/.test(document.querySelector('#card').textContent)`, 10);
  assert('an entry opened: its declaration and the checks of it', /by /.test(await page.eval(`document.querySelector('#card').textContent`)));
  await shot('koinonia-04-entry');
  await page.eval(`document.querySelector('.tab[data-tab="journal"]').click()`);
  await page.until(`document.querySelectorAll('#journal .entry').length > 10`, 20);
  await page.eval(`(() => { const x = document.querySelector('#journal #idle'); x.checked = true; x.dispatchEvent(new Event('change')); })()`);
  await page.until(`[...document.querySelectorAll('#journal .what')].some((x) => x.textContent === 'wake')`, 20);
  assert('the journal shows every participant\'s recorded behaviour, wakes, decisions and acts among it', await page.eval(`['wake', 'decide', 'act'].every((w) => [...document.querySelectorAll('#journal .what')].some((x) => x.textContent === w))`));
  await shot('koinonia-06-journal');
  await page.eval(`document.getElementById('take-part').click()`);
  await page.until(`!document.querySelector('#sheet').hidden && document.querySelectorAll('#sheet .place').length > 0`, 20);
  assert('a person takes part from the page: their places and the acts open to them', await page.eval(`/you hold/.test(document.querySelector('#sheet').textContent)`));
  await shot('koinonia-07-take-part');
  await page.eval(`document.querySelector('#sheet #cancel').click()`);
  await page.eval(`document.getElementById('theme').click()`);
  await shot('koinonia-05-night');
} catch (error) {
  assert('the drive ran to its end', false, error.message);
} finally {
  await page.close?.();
}
console.log(failures ? `${failures} failed` : 'all passed');
process.exit(failures ? 1 : 0);
