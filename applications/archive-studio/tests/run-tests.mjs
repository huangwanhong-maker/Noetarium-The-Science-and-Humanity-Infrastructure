// Checks of what is the Archive Studio's own (ADR 0119): its page (every element the shared studio asks for, the files it loads, its
// three views declared), and its server on a store of its own: a project begun and the example archive laid out; every holding kept
// with the SHA-256 of its bytes, computed here apart; the files of linked data listed and read; a file that does not read refused
// with its line and column, and one that reads kept; an edit and its undo returning the description to what it was; and, where the
// engine of linked data is built, the summary, a query, the shapes' two failures planted in the example, entailment and
// isomorphism, each held to what the example states; the catalogue answering whether or not it is harvested; and a write from
// another page refused. Run from the repository root:  node applications/archive-studio/tests/run-tests.mjs

import { spawn } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(name, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/archive-studio');
const read = (path) => readFileSync(join(APP, path), 'utf8');

// ---- the page ------------------------------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js'), shell = readFileSync(join(ROOT, 'visualization/js/workspace/studio.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('archive studio: the page\'s style sheets and script exist, the linked-data drawings\' among them', !missing.length && local.includes('/visualization/js/linked/linked.css'), missing.join(', '));
  const ids = [...new Set([...shell.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => !['progress', 'progress-text', 'to-workbench'].includes(i));
  assert('archive studio: every element the shared studio asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  assert('archive studio: the page starts the shared studio with three views of its own, the archive first',
    /startStudio\(/.test(app) && /views: \[archiveView\(\), graphView\(\), catalogueView\(\)\]/.test(app) && /first: 'archive'/.test(app));
  assert('archive studio: the shared studio takes views of a work\'s own and still names no work', /const OWN = new Map\(\(C\.views/.test(shell) && !/Archive Studio|archive-studio/.test(shell.replace(/^\/\/.*$/gm, '')));
  assert('archive studio: the server names its port, stands on the API\'s host and gives it its own routes',
    /port=9150/.test(read('serve.py')) && /routes=archive\.routes/.test(read('serve.py')) && /def routes\(handler, method/.test(read('archive.py')));
}

// ---- the server ----------------------------------------------------------------------------------------------------------------
const free = () => new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const { port } = s.address(); s.close(() => r(port)); }); });
const python = process.env.PYTHON || (process.platform === 'win32' ? 'python' : 'python3');
const store = mkdtempSync(join(tmpdir(), 'archive-studio-'));
const port = await free();
const BASE = `http://127.0.0.1:${port}`;
const child = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store], { stdio: ['ignore', 'ignore', 'pipe'] });
let stderr = '';
child.stderr.on('data', (d) => { stderr += d; });
const call = async (method, path, body, origin = BASE) => {
  const r = await fetch(BASE + path, { method, headers: { 'Content-Type': 'application/json', Origin: origin }, body: body === undefined ? undefined : JSON.stringify(body) });
  let json = null;
  try { json = await r.json(); } catch { /* none */ }
  return { status: r.status, body: json };
};
try {
  let status = null;
  for (let i = 0; i < 60 && !status; i++) {
    await new Promise((r) => setTimeout(r, 250));
    try { status = (await call('GET', '/api/status')).body; } catch { /* not up */ }
  }
  assert('archive studio: the server starts on a store of its own', !!status, stderr.slice(-300));
  const made = await call('POST', '/api/projects', { title: 'Papers of the reading room' });
  const name = made.body?.name;
  assert('archive studio: a project begun', made.status === 200 && !!name, JSON.stringify(made.body));
  const engine = status?.engine?.current;
  const example = await call('POST', `/api/projects/${name}/example`, {});
  if (!engine) {
    skip('archive studio: the example, every route of linked data and the catalogue (the engine of linked data is not built: run the recipe)');
  } else {
    assert('archive studio: the example archive laid out: four holdings, five files of linked data', example.status === 200 && example.body.holdings === 4 && example.body.files === 5, JSON.stringify(example.body));
    const hs = (await call('GET', `/api/projects/${name}/holdings`)).body.holdings;
    const sound = hs.find((h) => h.name.endsWith('(synthetic).wav'));
    let digests = true;
    for (const h of hs) {
      const bytes = Buffer.from(await (await fetch(`${BASE}/api/projects/${name}/holdings/${encodeURIComponent(h.name)}`)).arrayBuffer());
      digests = digests && createHash('sha256').update(bytes).digest('hex') === h.sha256 && bytes.length === h.bytes;
    }
    assert('archive studio: every holding is kept with the SHA-256 of its bytes, computed here apart', hs.length === 4 && digests && !!sound && sound.type === 'audio/wav');
    const files = (await call('GET', `/api/projects/${name}/linked`)).body.files.map((f) => f.file);
    assert('archive studio: its linked data: annotations, holdings, people, shapes and a vocabulary', files.join(' ') === 'annotations.ttl holdings.ttl people.ttl shapes.ttl vocabulary.ttl', files.join(' '));
    const refused = await call('PUT', `/api/projects/${name}/linked/notes.ttl`, { text: '@prefix ex: <http://e/> .\nex:a ex:b nope:c .' });
    assert('archive studio: a file that does not read is refused, with its line and column', refused.status === 400 && refused.body.line === 2 && refused.body.column === 17, JSON.stringify(refused.body));
    const kept = await call('PUT', `/api/projects/${name}/linked/notes.ttl`, { text: '@prefix ex: <http://e/> .\nex:a ex:b "c"@en .' });
    assert('archive studio: one that reads is kept, its statements counted', kept.status === 200 && kept.body.statements === 1);
    const summary = (await call('POST', `/api/projects/${name}/ask`, { op: 'summary' })).body;
    const minutes = hs.find((h) => h.name.startsWith('Minutes'));
    const edit = (await call('POST', `/api/projects/${name}/linked/holdings.ttl/edit`, { added: [[{ kind: 'iri', value: minutes.iri }, { kind: 'iri', value: 'http://purl.org/dc/terms/rights' }, { kind: 'literal', value: 'open', datatype: 'http://www.w3.org/2001/XMLSchema#string' }]] })).body;
    const back = (await call('POST', `/api/projects/${name}/linked/holdings.ttl/edit`, edit.undo)).body;
    const after = (await call('POST', `/api/projects/${name}/ask`, { op: 'summary' })).body;
    assert('archive studio: an edit adds one statement, and its undo returns the archive to what it was', edit.undo.removed.length === 1 && back.undo.added.length === 1 && after.statements === summary.statements, `${summary.statements} → ${after.statements}`);
    const q = (await call('POST', `/api/projects/${name}/ask`, { op: 'query', query: 'PREFIX dcterms: <http://purl.org/dc/terms/> SELECT ?t WHERE { ?h dcterms:identifier ?i ; dcterms:title ?t } ORDER BY ?t' })).body;
    assert('archive studio: a query finds the four holdings by their titles, in order', q.rows.length === 4 && q.rows[0][0].value.startsWith('A sounding'), JSON.stringify(q.rows).slice(0, 200));
    const v = (await call('POST', `/api/projects/${name}/ask`, { op: 'validate', shapes: 'shapes.ttl' })).body;
    const why = v.results?.map((r) => r.message).sort();
    assert('archive studio: the shapes find the two faults the example plants: a date in words, a table with no maker',
      v.conforms === false && v.results.length === 2 && why[0] === 'a date is written YYYY, YYYY-MM or YYYY-MM-DD' && why[1] === 'a holding names who made it', JSON.stringify(why));
    const e = (await call('POST', `/api/projects/${name}/ask`, { op: 'entail' })).body;
    const typed = e.derived?.filter((d) => d.statement[1].value.endsWith('#type') && d.statement[2].value.endsWith(':Record'));
    assert('archive studio: entailment by the vocabulary makes every holding a record, by rdfs9, with no inconsistency', typed?.length === 4 && typed.every((d) => d.rule === 'rdfs9') && e.inconsistencies.length === 0, JSON.stringify(e).slice(0, 300));
    const same = (await call('POST', `/api/projects/${name}/ask`, { op: 'compare', files: ['people.ttl', 'notes.ttl'], a: [], b: [] })).body;
    assert('archive studio: isomorphism is asked of the engine and answered as a verdict', ['holds', 'fails', 'undecided'].includes(same.status), JSON.stringify(same));
    const cat = (await call('GET', '/api/catalogue?q=elections&limit=3')).body;
    if (cat.harvested) {
      const taken = await call('POST', `/api/projects/${name}/catalogue`, { id: cat.studies[0].id });
      assert('archive studio: a harvested study taken into the project as linked data in DISCO', taken.status === 200 && taken.body.kept === 'studies.ttl' && taken.body.statements > 3);
    } else {
      assert('archive studio: with no catalogue harvested, the catalogue says how to harvest it', /social_studies\.py/.test(cat.message || ''));
    }
  }
  const other = await call('PUT', `/api/projects/${name}/linked/x.ttl`, { text: '' }, 'http://elsewhere.example');
  assert('archive studio: a write from another page is refused', other.status === 403, JSON.stringify(other.body));
} finally {
  child.kill();
  try { rmSync(store, { recursive: true, force: true }); } catch { /* a file still held */ }
}
done();
