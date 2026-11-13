// Histos Harmonia driven as a person would use it, in headless Chrome over the DevTools protocol: an object found by its
// name in its own script, its ties drawn on the map, the ring of actions and the card, a place opened as a fan, the thread
// followed, a room entered from the ring, a tuning's circle touched, an inquiry answered and walked on from, the loom, the
// cone, the clock, and a path walked. It needs the work's server running on the domain's own record, and is not part of
// the suite.
// Run from the repository root:  node applications/histos-harmonia/tests/drive.mjs [--at http://127.0.0.1:9130] [--chrome <path>]

import { spawn } from 'node:child_process';
import { existsSync, mkdtempSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const arg = (name, fallback) => { const i = process.argv.indexOf(`--${name}`); return i > 0 ? process.argv[i + 1] : fallback; };
const AT = arg('at', 'http://127.0.0.1:9130');
const CHROME = arg('chrome', ['C:/Program Files/Google/Chrome/Application/chrome.exe', '/usr/bin/google-chrome', '/usr/bin/chromium'].find((p) => existsSync(p)));
if (!CHROME) { console.error('No Chrome found: name one with --chrome.'); process.exit(2); }

let passed = 0, failed = 0;
const say = (name, ok, detail = '') => { ok ? passed++ : failed++; console.log(`${ok ? 'ok  ' : 'FAIL'}  ${name}${!ok && detail !== '' ? `: ${typeof detail === 'string' ? detail : JSON.stringify(detail)}` : ''}`); };
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
const profile = mkdtempSync(join(tmpdir(), 'histos-drive-'));
const chrome = spawn(CHROME, ['--headless=new', '--disable-gpu', `--remote-debugging-port=${port}`, `--user-data-dir=${profile}`, '--window-size=1600,1000', '--autoplay-policy=no-user-gesture-required', 'about:blank'], { stdio: 'ignore' });
let socket;
try {
  let target = null;
  for (let k = 0; k < 80 && !target; k++) { await sleep(150); try { target = (await (await fetch(`http://127.0.0.1:${port}/json/list`)).json()).find((t) => t.type === 'page'); } catch { /* not yet */ } }
  socket = new WebSocket(target.webSocketDebuggerUrl);
  await new Promise((r) => socket.addEventListener('open', r, { once: true }));
  const waiting = new Map(), errors = [];
  let serial = 0;
  socket.addEventListener('message', (e) => {
    const m = JSON.parse(e.data);
    if (m.id && waiting.has(m.id)) { waiting.get(m.id)(m); waiting.delete(m.id); }
    else if (m.method === 'Runtime.exceptionThrown') errors.push(m.params.exceptionDetails.exception?.description || m.params.exceptionDetails.text);
    else if (m.method === 'Runtime.consoleAPICalled' && m.params.type === 'error') errors.push(m.params.args.map((a) => a.value || a.description).join(' '));
  });
  const send = (method, params = {}) => new Promise((r) => { const id = ++serial; waiting.set(id, r); socket.send(JSON.stringify({ id, method, params })); });
  // An expression evaluated in the page; its value, or the error it threw as { threw }.
  let HELP = '';
  const page = async (expression) => {
    const m = await send('Runtime.evaluate', { expression: `(async () => { ${expression} })()`, awaitPromise: true, returnByValue: true });
    return m.result?.exceptionDetails ? { threw: m.result.exceptionDetails.exception?.description || m.result.exceptionDetails.text } : m.result?.result?.value;
  };
  const until = async (condition, ms = 20000) => { for (let t = 0; t < ms; t += 150) { if (await page(`${HELP} return Boolean(${condition});`) === true) return true; await sleep(150); } return false; };
  const open = async (query) => { await send('Page.navigate', { url: `${AT}/${query}` }); await sleep(400); };
  const HELP_TEXT = `const $$ = (s) => [...document.querySelectorAll(s)]; const shown = (s) => { const n = document.querySelector(s); return Boolean(n) && !n.hidden && n.getClientRects().length > 0; };
    const find = async (text) => { window.dispatchEvent(new KeyboardEvent('keydown', { key: '/' })); const f = document.getElementById('find'); f.value = text; f.dispatchEvent(new Event('input')); f.dispatchEvent(new KeyboardEvent('keydown', { key: 'Enter' })); };`;
  const ask = (body) => page(`${HELP_TEXT} ${body}`);
  HELP = HELP_TEXT;
  await send('Runtime.enable');
  await send('Page.enable');

  // The page as it first opens: nothing is established, and it says so.
  await open('?theme=day');
  say('the page opens on the map and says that nothing is established yet', await until(`document.querySelector('#notice h2')?.textContent.includes('Nothing here is established')`), await ask(`return document.body.innerText.slice(0, 200);`));
  await ask(`document.querySelector('#notice button').click();`);
  say('one key reads the proposals: the places appear, and nothing is chosen', await until(`document.querySelectorAll('#map .spot').length > 10`) && await ask(`return !shown('#card') && !shown('#petals') && document.querySelector('[data-layer=proposed]').classList.contains('at');`));

  // An object found by its name in its own script; its ties drawn; the ring and the card.
  await ask(`await find('新法');`);
  say('an object is found by its name in its own script', await until(`document.querySelector('#card h1')?.textContent.includes("Zhu Zaiyu's equal temperament")`), await ask(`return document.querySelector('#card h1')?.textContent;`));
  await sleep(900);
  const map = await ask(`return { self: $$('#map .spot.self').length, direct: $$('#map .arc.direct').length, echo: $$('#map .arc.echo').length / 2, tags: $$('#map .arc-tag').map((t) => t.textContent), dim: $$('#map .spot.dim').length,
    petals: $$('#petals .petal').map((b) => b.textContent), acts: $$('#card .act b').map((b) => b.textContent), ties: $$('#card .tie .role').map((b) => b.textContent), zoom: document.querySelector('#map .world').getAttribute('transform') };`);
  say('its place is marked, its ties are drawn as threads and said in words, and the map has drawn near', map.self === 1 && map.direct >= 1 && map.ties.includes('made possible by') && map.dim > 3 && !/scale\(1\.0000\)/.test(map.zoom), map);
  say('those who answered the same problem independently are joined to it by a thread of their own', map.echo >= 2 && map.tags.includes('the same problem, independently') && map.ties.some((t) => t.includes('independently')), map);
  say('a ring of actions opens on it, what music brings first, and the card offers them in words', map.petals.length >= 3 && map.petals[0] === 'Practise' && map.acts[0] === 'Hear it and measure it' && map.acts.includes('Inquire into it'), map);

  // A place of several objects opens as a fan; choosing from it walks on, and the thread followed is kept.
  await ask(`document.querySelector('#map .spot[data-place=venice]').dispatchEvent(new MouseEvent('click', { bubbles: true }));`);
  say('a place of several objects opens as a fan of them', await until(`shown('#fan') && document.querySelectorAll('#fan button').length >= 3`), await ask(`return document.querySelector('#fan')?.innerText;`));
  const before = await ask(`return document.querySelector('#card h1').textContent;`);
  await ask(`document.querySelector('#fan button:not(.on)').click();`);
  say('an object chosen from the fan is walked to, and the fan closes', await until(`document.querySelector('#card h1').textContent !== ${JSON.stringify(before)} && !shown('#fan')`));
  await ask(`const a = document.querySelector('#card .tie a'); if (a) a.click();`);
  say('a name on the card is a way on, and the thread followed lists where one has been', await until(`document.querySelectorAll('#trail button').length >= 3`), await ask(`return $$('#trail button').map((b) => b.textContent);`));
  await ask(`document.querySelector('#trail button').click();`);
  say('the thread leads back', await until(`document.querySelector('#card h1').textContent === ${JSON.stringify(before)}`));

  // A room entered from the ring: a tuning's own page.
  await ask(`await find('Quarter');`);
  await until(`document.querySelector('#card h1')?.textContent.includes('Quarter-comma')`);
  await sleep(700);
  await ask(`document.querySelector('#petals .petal.first').click();`);
  say('the ring\'s first petal enters the tuning\'s own page', await until(`shown('#room') && document.querySelectorAll('#room .wheel .seg').length === 12`), await ask(`return document.querySelector('#room')?.innerText.slice(0, 300);`));
  const wheel = await ask(`return { thirds: $$('#room .wheel .third').length, notes: $$('#room .wheel .note').length, strings: $$('#room .string').length, wolf: $$('#room .wheel .wolf').map((t) => t.textContent), petals: shown('#petals'), activity: document.getElementById('room').dataset.activity };`);
  say('its circle has twelve fifths, twelve thirds and twelve notes, the wolf is named, and its string has twelve lengths', wheel.thirds === 12 && wheel.notes === 12 && wheel.strings === 12 && /the wolf \+35\.68/.test(wheel.wolf[0] || '') && wheel.activity === 'practise' && !wheel.petals, wheel);
  await ask(`document.querySelector('#room .wheel .wolf').previousElementSibling.dispatchEvent(new MouseEvent('click', { bubbles: true }));`);
  say('a fifth touched is sounded and read: its departure, and how fast it beats', await until(`/\\+35\\.68 cents/.test(document.querySelector('#room .wheel .centre.big').textContent) && /beats \\d/.test(document.querySelector('#room .wheel').textContent)`), await ask(`return document.querySelector('#room .wheel').textContent.slice(-160);`));
  await ask(`$$('#room .wheel .third')[0].dispatchEvent(new MouseEvent('click', { bubbles: true }));`);
  say('a pure third does not beat', await until(`/does not beat/.test(document.querySelector('#room .wheel').textContent)`));

  // The inquiry: a thought written, the record's answer seen, and a card walked on from.
  await ask(`$$('#room .room-acts button').find((b) => b.textContent === 'Inquire').click();`);
  say('another activity of the same object is one key away, within the room', await until(`document.querySelector('#room .inq-step h2')?.textContent.includes('What problem does Quarter-comma meantone answer')`));
  await ask(`const t = document.querySelector('#room .guess textarea'); t.value = 'Harsh thirds.'; t.dispatchEvent(new Event('input')); $$('#room .inq-step button').find((b) => b.textContent.includes('See what the record says')).click();`);
  say('one writes what one thinks, then sees what the record says, each answer a card', await until(`document.querySelectorAll('#room .rcard').length >= 1 && document.querySelector('#room .yours')?.textContent.includes('Harsh thirds')`), await ask(`return document.querySelector('#room .inq-step')?.innerText.slice(0, 300);`));
  const card = await ask(`const c = document.querySelector('#room .rcard'); const name = c.querySelector('.n').textContent; c.click(); return name;`);
  say('a card leads on to that object, still in the room and still inquiring', await until(`shown('#room') && document.querySelector('#room .room-of b').textContent !== 'Quarter-comma meantone' && document.querySelector('#room .inq-step')`), card);
  await ask(`window.dispatchEvent(new KeyboardEvent('keydown', { key: 'Escape' }));`);
  say('Escape leaves the room, and the object walked to is the one chosen on the map', await until(`!shown('#room') && shown('#card') && document.querySelector('#card h1').textContent !== 'Quarter-comma meantone'`), await ask(`return [shown('#room'), document.querySelector('#card h1')?.textContent];`));

  // The loom, the cone, the clock.
  await ask(`await find('Quarter');`);
  await until(`document.querySelector('#card h1')?.textContent.includes('Quarter-comma')`);
  await ask(`document.querySelector('[data-view=loom]').click();`);
  say('the same choice on the loom, with its ring', await until(`document.querySelector('.h-loom .h-mark.selected') && document.querySelectorAll('#petals .petal').length >= 1`), await ask(`return { marks: $$('.h-mark').length, petals: $$('#petals .petal').length };`));
  await ask(`document.querySelector('[data-view=cone]').click();`);
  say('and as its cone', await until(`document.querySelector('.h-cone .h-slip.center')`));
  await ask(`document.querySelector('[data-view=map]').click();`);
  await until(`document.querySelectorAll('#map .spot').length > 10`);
  const all = await ask(`return $$('#map .spot').length;`);
  await ask(`const y = document.getElementById('year'); y.value = -100; y.dispatchEvent(new Event('input'));`);
  say('the clock turned back to 101 BC leaves fewer places, and shows the questions of that year', await until(`document.getElementById('year-text').textContent === '101 BC' && document.querySelectorAll('#map .spot').length < ${all} && document.querySelectorAll('#questions .q').length >= 1`), await ask(`return { spots: $$('#map .spot').length, year: document.getElementById('year-text').textContent, questions: $$('#questions .q').map((q) => q.textContent) };`));
  await ask(`window.dispatchEvent(new KeyboardEvent('keydown', { key: 'Escape' }));`);
  say('Escape lets the object go: no card, no ring', await until(`!shown('#card') && !shown('#petals')`));

  // A path walked.
  await open('?theme=day&exhibit=the-circle');
  say('a path opens at its first stop, with a key to enter its room', await until(`shown('#tour') && document.querySelector('#tour .at')?.textContent.startsWith('1 of 9') && document.querySelector('#card h1')?.textContent === 'The monochord'`), await ask(`return document.querySelector('#tour')?.innerText;`));
  await ask(`window.dispatchEvent(new KeyboardEvent('keydown', { key: 'ArrowRight' }));`);
  say('the arrow goes on to the next stop', await until(`document.querySelector('#tour .at')?.textContent.startsWith('2 of 9') && document.querySelector('#card h1')?.textContent === 'Pythagorean tuning'`));
  await ask(`$$('#tour button').find((b) => b.textContent === 'Enter').click();`);
  say('and Enter opens the stop\'s room', await until(`shown('#room') && document.getElementById('room').dataset.activity === 'inquire'`));

  say('nothing was thrown and nothing was written to the console as an error', errors.length === 0, errors.slice(0, 3));
} catch (error) {
  say('the drive ran to its end', false, String(error?.stack || error));
} finally {
  try { socket?.close(); } catch { /* closed */ }
  chrome.kill();
  await sleep(300);
  try { rmSync(profile, { recursive: true, force: true }); } catch { /* Chrome still holds it; the system's temporary folder is cleared in time */ }
}
console.log(`\n${passed} of ${passed + failed} assertions passed.`);
process.exit(failed ? 1 : 0);
