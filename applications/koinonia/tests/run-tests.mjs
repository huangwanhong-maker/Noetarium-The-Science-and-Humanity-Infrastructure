// Checks of Koinonia (ADR 0169): the page's files and what it asks of the server; the server's port and its rule that changes come only
// from this computer; and the society itself (check_koinonia.py): two communities working a catalogue through their chronicles, the
// spending guard, and three communities whose wants run in a circle under five arrangements of circulation. No model is asked.
// Run from the repository root:  node applications/koinonia/tests/run-tests.mjs

import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(`koinonia: ${name}`, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/koinonia');
const read = (path) => readFileSync(join(APP, path), 'utf8');
const python = process.env.PYTHON || (process.platform === 'win32' ? 'python' : 'python3');

{
  const page = read('index.html'), app = read('src/app.js'), serve = read('serve.py');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  assert('the page\'s style sheet and script exist', local.length >= 2 && local.every((p) => existsSync(join(APP, p))), local.join(', '));
  const ids = [...new Set([...app.matchAll(/\$\("([a-z-]+)"\)/g)].map((m) => m[1]))];
  assert('every element the page\'s script asks for is on the page', ids.length > 10 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  assert('the server names its port, 9230, and takes changes only from this computer', /default=9230/.test(serve) && /changes are taken only from this computer/.test(serve));
  assert('the server keeps no key: it reads the kept key and the ledger from the person\'s settings folder', /kept_key\("koinonia"\)/.test(serve) && /Ledger\.kept\(\)/.test(serve) && !/sk-ant/.test(serve + app));
  assert('model agents ask a stand-in until the person switches Claude on', /self\.ask_with\(False\)/.test(serve));
  assert('a community\'s card shows what it holds and its norms of access, and a person reaches its holdings from the page under those norms',
    /holdingsHtml\(c\)/.test(app) && /Its norms of access/.test(app) && /data-act="deposit"/.test(app) && /PERSON_KINDS = agents\.PAGE_KINDS/.test(serve));
}

const run = spawnSync(python, [join(APP, 'tests', 'check_koinonia.py')],
  { cwd: ROOT, encoding: 'utf8', timeout: 1800000, maxBuffer: 1 << 24, env: { ...process.env, PYTHONIOENCODING: 'utf-8' } });
let told = 0;
for (const line of (run.stdout || '').split('\n')) {
  if (!line.startsWith('{')) continue;
  let rec;
  try { rec = JSON.parse(line); } catch { continue; }
  told++;
  if (rec.kind === 'assert') say(rec.name, rec.ok, rec.detail || '');
  else if (rec.kind === 'skip') skip(rec.name);
}
assert('check_koinonia.py ran to the end', run.status === 0 && told > 0, (run.stderr || '').trim().split('\n').slice(-4).join(' | '));
// the case of a book written by communities (the design record of 2026-11-26), dry: check_book.py
const book = spawnSync(python, [join(APP, 'tests', 'check_book.py')],
  { cwd: ROOT, encoding: 'utf8', timeout: 1800000, maxBuffer: 1 << 24, env: { ...process.env, PYTHONIOENCODING: 'utf-8' } });
let toldBook = 0;
for (const line of (book.stdout || '').split('\n')) {
  if (!line.startsWith('{')) continue;
  let rec;
  try { rec = JSON.parse(line); } catch { continue; }
  toldBook++;
  if (rec.kind === 'assert') say(rec.name, rec.ok, rec.detail || '');
  else if (rec.kind === 'skip') skip(rec.name);
}
assert('check_book.py ran to the end', book.status === 0 && toldBook > 0, (book.stderr || '').trim().split('\n').slice(-4).join(' | '));
// the node's gateway and the archive of conduct (Phase 10 of the design of 2026-11-27): check_gateway.py and check_archive.py; the
// completion re-founded at a node, on a copy of the first attempt (Phase 12): check_completion.py
for (const script of ['check_gateway.py', 'check_archive.py', 'check_completion.py', 'check_book_passports.py',
  'check_dispatch_waits.py', 'check_book_dashboard.py']) {
  const r = spawnSync(python, [join(APP, 'tests', script)],
    { cwd: ROOT, encoding: 'utf8', timeout: 1800000, maxBuffer: 1 << 24, env: { ...process.env, PYTHONIOENCODING: 'utf-8' } });
  let n = 0;
  for (const line of (r.stdout || '').split('\n')) {
    if (!line.startsWith('{')) continue;
    let rec;
    try { rec = JSON.parse(line); } catch { continue; }
    n++;
    if (rec.kind === 'assert') say(rec.name, rec.ok, rec.detail || '');
    else if (rec.kind === 'skip') skip(rec.name);
  }
  assert(`${script} ran to the end`, r.status === 0 && n > 0, (r.stderr || '').trim().split('\n').slice(-4).join(' | '));
}
{
  const serve = read('serve.py'), guide = existsSync(join(APP, 'ONBOARDING.md')) ? read('ONBOARDING.md') : '';
  assert('the server takes credentialed acts from other addresses only when started with --open-to-agents, off by default',
    /add_argument\("--open-to-agents", action="store_true"/.test(serve) && /open_to_agents=args\.open_to_agents/.test(serve));
  assert('ONBOARDING.md holds the one prompt for any agent and the routes of the gateway', guide.includes('You act for `<principal>`') && guide.includes('/record/mine'));
}
done();
