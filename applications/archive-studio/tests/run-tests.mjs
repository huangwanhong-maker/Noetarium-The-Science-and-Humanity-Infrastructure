// Checks of what is the Archive Studio's own (ADR 0119, ADR 0120): its page (every element the shared studio asks for, the files it
// loads, its five views declared) and its server, started with several people on a store of its own. A project begun and the example
// archive laid out; every holding kept with the SHA-256 of its bytes, computed here apart; files of linked data refused with their line
// and column, or kept; an edit and its undo; a query, the shapes' two planted faults, entailment, isomorphism. Then the second stage:
// the arrangement with archeion's one planted fault and every reference code; the chains of custody, one broken where it is planted;
// the journal verified, and a copy with one entry changed refused at that entry; a codebook holding its file to it (the planted 7
// found), two versions compared, two variables harmonised; the coded passages in bytes that are their quotes; an image as a IIIF
// service, a tile of it at its size, the archive as a manifest; the finding aid in EAD3 read back; a BagIt package; the linked data as
// JSON-LD; a JSON-LD document whose context cannot be fetched refused naming it; people with keys and roles (a contributor keeps, a
// reader does not, a keeper alone arranges, a key no one has is refused); two people's changes to one file merged by statements; and
// what one does told to the others as an event. The tools are held to answers known by construction (tools_check.py).
// Run from the repository root:  node applications/archive-studio/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(name, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/archive-studio');
const read = (path) => readFileSync(join(APP, path), 'utf8');
const python = process.env.PYTHON || (process.platform === 'win32' ? 'python' : 'python3');

// ---- the page ------------------------------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js'), shell = readFileSync(join(ROOT, 'visualization/js/workspace/studio.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('archive studio: the page\'s style sheets and script exist, the linked-data drawings\' and the deep-zoom viewer\'s among them', !missing.length && local.includes('/visualization/js/linked/linked.css') && local.includes('/visualization/js/iiif/iiif.css'), missing.join(', '));
  const ids = [...new Set([...shell.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => !['progress', 'progress-text', 'to-workbench', 'here'].includes(i));
  assert('archive studio: every element the shared studio asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  assert('archive studio: the page starts the shared studio with five views of its own, the archive first',
    /startStudio\(/.test(app) && /views: \[archiveView\(\), arrangementView\(\), graphView\(\), datasetsView\(\), catalogueView\(\)\]/.test(app) && /first: 'archive'/.test(app));
  assert('archive studio: the shared studio takes views of a work\'s own, sends a person\'s key, and still names no work', /const OWN = new Map\(\(C\.views/.test(shell) && /'X-Key'/.test(shell) && !/Archive Studio|archive-studio/.test(shell.replace(/^\/\/.*$/gm, '')));
  assert('archive studio: the server names its port, stands on the API\'s host and gives it its own routes',
    /port=9150/.test(read('serve.py')) && /routes=archive\.routes/.test(read('serve.py')) && /def routes\(handler, method/.test(read('archive.py')));
}

// ---- the tools -----------------------------------------------------------------------------------------------------------------
{
  const r = spawnSync(python, [join(APP, 'tests/tools_check.py')], { cwd: ROOT, encoding: 'utf8', timeout: 600000 });
  const lines = (r.stdout || '').split('\n').filter((l) => l.startsWith('{'));
  if (!lines.length) skip(`archive studio: the tools (${(r.stderr || 'no answer').trim().split('\n').at(-1)})`);
  for (const l of lines) { const x = JSON.parse(l); assert(`archive studio ${x.name}`, x.ok, x.detail); }
}

// ---- the server ----------------------------------------------------------------------------------------------------------------
const free = () => new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const { port } = s.address(); s.close(() => r(port)); }); });
const store = mkdtempSync(join(tmpdir(), 'archive-studio-'));
const port = await free();
const BASE = `http://127.0.0.1:${port}`;
const child = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store, '--people', '--keeper', 'the keeper'], { stdio: ['ignore', 'ignore', 'pipe'] });
let stderr = '';
child.stderr.on('data', (d) => { stderr += d; });
const call = async (method, path, body, { origin = BASE, key = null, raw = null } = {}) => {
  const headers = { Origin: origin, ...(body !== undefined ? { 'Content-Type': 'application/json' } : {}), ...(key ? { 'X-Key': key } : {}) };
  const r = await fetch(BASE + path, { method, headers, body: raw ?? (body === undefined ? undefined : JSON.stringify(body)) });
  const type = r.headers.get('Content-Type') || '';
  let json = null, bytes = null;
  if (type.includes('json')) { try { json = await r.json(); } catch { /* none */ } } else bytes = Buffer.from(await r.arrayBuffer());
  return { status: r.status, body: json, bytes, type };
};
const q = (name, query, files) => call('POST', `/api/projects/${name}/ask`, { op: 'query', query, ...(files ? { files } : {}) });
try {
  let status = null;
  for (let i = 0; i < 180 && !status; i++) {   // the server first resolves its engines' sources, which may take some seconds
    await new Promise((r) => setTimeout(r, 500));
    try { status = (await call('GET', '/api/status')).body; } catch { /* not up */ }
  }
  assert('archive studio: the server starts with several people, on a store of its own', !!status && status.people === true && status.person?.name === 'the keeper', stderr.slice(-300));
  const made = await call('POST', '/api/projects', { title: 'Papers of the reading room' });
  const name = made.body?.name;
  assert('archive studio: a project begun', made.status === 200 && !!name, JSON.stringify(made.body));
  const engines = status?.engine?.engines || {};
  const built = ['linked', 'archive', 'inquiry'].every((e) => engines[e]?.built);
  if (!built) {
    skip('archive studio: the example, linked data, archives and the catalogue (the engines are not built: run python applications/archive-studio/engine.py build)');
  } else {
    const example = await call('POST', `/api/projects/${name}/example`, {});
    assert('archive studio: the example archive laid out: eight holdings, ten files of linked data, two codebooks', example.status === 200 && example.body.holdings === 8 && example.body.files === 10 && example.body.codebooks === 2, JSON.stringify(example.body));
    const hs = (await call('GET', `/api/projects/${name}/holdings`)).body.holdings;
    let digests = true;
    for (const h of hs) {
      const bytes = Buffer.from(await (await fetch(`${BASE}/api/projects/${name}/holdings/${encodeURIComponent(h.name)}`)).arrayBuffer());
      digests = digests && createHash('sha256').update(bytes).digest('hex') === h.sha256 && bytes.length === h.bytes;
    }
    assert('archive studio: every holding is kept with the SHA-256 of its bytes, computed here apart', hs.length === 8 && digests);
    const files = (await call('GET', `/api/projects/${name}/linked`)).body.files.map((f) => f.file);
    assert('archive studio: its linked data, ten files: annotations, arrangement, codebooks, codes, custody, holdings, people, shapes, a visit in JSON-LD, a vocabulary',
      files.join(' ') === 'annotations.ttl arrangement.ttl codebooks.ttl codes.ttl custody.ttl holdings.ttl people.ttl shapes.ttl visit.jsonld vocabulary.ttl', files.join(' '));
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
    const titles = (await q(name, 'PREFIX dcterms: <http://purl.org/dc/terms/> SELECT DISTINCT ?h WHERE { ?h dcterms:identifier ?i ; dcterms:title ?t }')).body;
    assert('archive studio: a query finds the eight holdings by their titles', titles.rows.length === 8, JSON.stringify(titles).slice(0, 200));
    const v = (await call('POST', `/api/projects/${name}/ask`, { op: 'validate', shapes: 'shapes.ttl', files: ['holdings.ttl', 'people.ttl', 'shapes.ttl'] })).body;
    const why = v.results?.map((r) => r.message).sort();
    assert('archive studio: the shapes find the two faults the example plants: a date in words, a table with no maker',
      v.conforms === false && v.results.length === 2 && why[0] === 'a date is written YYYY, YYYY-MM or YYYY-MM-DD' && why[1] === 'a holding names who made it', JSON.stringify(why));
    const e = (await call('POST', `/api/projects/${name}/ask`, { op: 'entail', files: ['holdings.ttl', 'vocabulary.ttl'] })).body;
    const typed = e.derived?.filter((d) => d.statement[1].value.endsWith('#type') && d.statement[2].value.endsWith(':Record'));
    assert('archive studio: entailment by the vocabulary makes the described holdings records, by rdfs9, with no inconsistency', typed?.length >= 4 && typed.every((d) => d.rule === 'rdfs9') && e.inconsistencies.length === 0, JSON.stringify(e).slice(0, 300));
    const same = (await call('POST', `/api/projects/${name}/ask`, { op: 'compare', files: ['people.ttl', 'notes.ttl'], a: [], b: [] })).body;
    assert('archive studio: isomorphism is asked of the engine and answered as a verdict', ['holds', 'fails', 'undecided'].includes(same.status), JSON.stringify(same));

    // ---- the arrangement, custody and the journal ----
    const arr = (await call('GET', `/api/projects/${name}/arrangement`)).body;
    assert('archive studio: the arrangement in RiC-O: eight units, every holding placed, the codes joined into references',
      arr.units.length === 8 && arr.unarranged.length === 0 && arr.units.find((u) => u.id === 'register').reference === 'RR/2/2', JSON.stringify(arr.units.map((u) => u.reference)));
    assert('archive studio: archeion finds the one fault the example plants: a file that ends after its series', arr.faults.length === 1 && arr.faults[0].unit === 'register' && arr.faults[0].what.includes('1935'), JSON.stringify(arr.faults));
    const rico = (await q(name, 'PREFIX rico: <https://www.ica.org/standards/RiC/ontology#> SELECT ?u WHERE { ?u rico:hasRecordSetType <https://www.ica.org/standards/RiC/vocabularies/recordSetTypes#Series> }', ['arrangement.ttl'])).body;
    assert('archive studio: the series are RiC record sets of the type Series', rico.rows.length === 4, JSON.stringify(rico.rows));
    const cus = (await call('GET', `/api/projects/${name}/custody`)).body.chains;
    const broken = cus.filter((c) => c.verdict?.status === 'fails');
    assert('archive studio: every chain of custody followed by archeion; the one planted break found, where someone hands over what they never held',
      cus.length === 8 && broken.length === 1 && broken[0].holding.startsWith('Readers') && broken[0].breaks[0].at === 1 && broken[0].authentic.status === 'fails' && cus.filter((c) => c.authentic?.status === 'holds').length === 7, JSON.stringify(broken));
    const j = (await call('GET', `/api/projects/${name}/journal/verify`)).body;
    assert('archive studio: the journal of every change verified by archeion, each entry sealed to the one before', j.verdict.status === 'holds' && j.entries > 30, JSON.stringify(j));
    const path = join(store, 'projects', name, 'journal.jsonl');
    const lines = readFileSync(path, 'utf8').split('\n');
    const changed = JSON.parse(lines[4]);
    changed.who = 'someone else';
    writeFileSync(path, [...lines.slice(0, 4), JSON.stringify(changed), ...lines.slice(5)].join('\n'));
    const tampered = (await call('GET', `/api/projects/${name}/journal/verify`)).body;
    assert('archive studio: a journal with one entry changed is refused at that entry', tampered.verdict.status === 'fails' && tampered.verdict.reason.startsWith('entry 5 '), JSON.stringify(tampered.verdict));
    writeFileSync(path, lines.join('\n'));

    // ---- datasets ----
    const conform = (await call('POST', `/api/projects/${name}/codebooks/survey-1932/conform`, { holding: 'Survey of readers, 1932 (illustrative).csv' })).body;
    const sat = conform.frequencies?.find((f) => f.variable === 'satisfied');
    assert('archive studio: the 1932 survey held to its codebook: the planted 7 found, and the declared 9 counted as missing', conform.faults.length === 1 && conform.faults[0].includes('7 is not a code') && sat?.missing['9'] === 1, JSON.stringify(conform.faults));
    const cmp = (await call('POST', `/api/projects/${name}/codebooks/compare`, { earlier: 'survey-1932', later: 'survey-1934' })).body;
    assert('archive studio: the two versions compared: purpose removed, which breaks comparison, and reason added', !cmp.compatible && cmp.changes.some((c) => c.variable === 'purpose' && c.breaking) && cmp.changes.some((c) => c.variable === 'reason' && !c.breaking), JSON.stringify(cmp.changes));
    const common = (await call('POST', `/api/projects/${name}/codebooks/common`, { a: { codebook: 'survey-1932', variable: 'purpose' }, b: { codebook: 'survey-1934', variable: 'reason' }, corresponds: [['1', '1'], ['1', '5'], ['2', '2'], ['3', '3'], ['4', '4']] })).body;
    assert('archive studio: purpose and reason harmonised: four target codes, study and teaching in one', common.classes.length === 4 && common.classes[0].join() === 'a:1,b:1,b:5', JSON.stringify(common.classes));
    const ddi = await call('GET', `/api/projects/${name}/codebooks/survey-1932.xml`);
    assert('archive studio: a codebook written in DDI-Codebook 2.5', ddi.status === 200 && ddi.bytes.toString('utf8').includes('ddi:codebook:2_5') && ddi.bytes.toString('utf8').includes('satisfied'));

    // ---- coded passages ----
    const cod = (await call('GET', `/api/projects/${name}/codings`)).body;
    const text = Buffer.from(readFileSync(join(APP, 'examples/reading-room/holdings/Minutes of the reading-room committee.txt')));
    const quoted = cod.codings.document.every((d, i) => text.subarray(cod.codings.begin[i], cod.codings.end[i]).toString('utf8') === cod.codings.quote[i]);
    assert('archive studio: twelve coded passages by two coders, each span in bytes exactly its quote', cod.codings.document.length === 12 && quoted && new Set(cod.codings.coder).size === 2, JSON.stringify(cod.codings).slice(0, 200));

    // ---- IIIF, EAD, BagIt, JSON-LD ----
    const img = encodeURIComponent('Register of readers, 1931 (synthetic facsimile).jpg');
    const info = await call('GET', `/iiif/${name}/${img}/info.json`);
    const tile = await call('GET', `/iiif/${name}/${img}/512,512,512,512/256,/0/default.png`);
    const png = tile.bytes;
    assert('archive studio: an image is a IIIF Image API 3.0 service, and a tile of it comes at the size asked', info.status === 200 && info.body.width === 4800 && info.body.tiles[0].scaleFactors.at(-1) === 16
      && png.readUInt32BE(16) === 256 && png.readUInt32BE(20) === 256, `${info.status} ${png?.readUInt32BE(16)}×${png?.readUInt32BE(20)}`);
    const man = (await call('GET', `/iiif/${name}/manifest.json`)).body;
    assert('archive studio: the archive as a IIIF Presentation 3.0 manifest: its images with their services, its sound, the annotations on them',
      man?.type === 'Manifest' && man.items.some((c) => c.items[0].items[0].body.service) && man.items.some((c) => c.items[0].items[0].body.type === 'Sound') && man.items.some((c) => c.annotations), JSON.stringify(man).slice(0, 200));
    const ead = await call('GET', `/api/projects/${name}/arrangement.xml`);
    const back2 = spawnSync(python, ['-c', 'import sys; sys.path.insert(0, "data/formats"); import ead, json; r = ead.read(sys.stdin.buffer.read()); print(json.dumps([len(r["units"]), r["version"], sum(len(u["holdings"]) for u in r["units"])]))'],
      { cwd: ROOT, input: ead.bytes, encoding: 'utf8' });
    assert('archive studio: the finding aid in EAD3, read back with its eight units and its eight holdings', ead.status === 200 && back2.stdout.trim() === '[8, "EAD3", 8]', back2.stdout + back2.stderr);
    const pack = await call('POST', `/api/projects/${name}/package`, { unit: 'plans' });
    assert('archive studio: a BagIt package of a unit, as a zip', pack.status === 200 && pack.type === 'application/zip' && pack.bytes.subarray(0, 2).toString() === 'PK' && pack.bytes.toString('latin1').includes('manifest-sha256.txt'), String(pack.status));
    const jsonld = await call('GET', `/api/projects/${name}/export.jsonld`);
    const doc = jsonld.body;
    assert('archive studio: the linked data as one JSON-LD document, compacted against its prefixes', jsonld.status === 200 && doc['@context'].rico === 'https://www.ica.org/standards/RiC/ontology#' && doc['@graph'].length > 50, String(jsonld.status));
    const unfetched = await call('PUT', `/api/projects/${name}/linked/elsewhere.jsonld`, { text: '{"@context": "http://example.invalid/ctx.jsonld", "@id": "http://e/x", "name": "y"}' });
    assert('archive studio: a JSON-LD document whose remote context cannot be fetched is refused, naming the context', unfetched.status === 400 && unfetched.body.missing === 'http://example.invalid/ctx.jsonld', JSON.stringify(unfetched.body));

    // ---- several people ----
    const okafor = (await call('POST', '/api/people', { name: 'J. Okafor', role: 'contributor' })).body;
    const reader = (await call('POST', '/api/people', { name: 'A. Reader', role: 'reader' })).body;
    const who = (await call('GET', '/api/whoami', undefined, { key: okafor.key })).body;
    assert('archive studio: a person added by the keeper is known by the key given once', who.name === 'J. Okafor' && who.role === 'contributor' && okafor.key.length > 20);
    const since = (await call('GET', `/api/projects/${name}/events?after=0&wait=0`, undefined, { key: okafor.key })).body.seq;
    const t = (await call('GET', `/api/projects/${name}/linked/people.ttl`, undefined, { key: okafor.key })).body;
    const mine = await call('PUT', `/api/projects/${name}/linked/people.ttl`, { text: `${t.text}\nex:ferrand foaf:nick "Mme F." .\n`, base: t.sha256 }, { key: okafor.key });
    assert('archive studio: a contributor keeps a change by their key', mine.status === 200, JSON.stringify(mine.body));
    const theirs = await call('PUT', `/api/projects/${name}/linked/people.ttl`, { text: `${t.text}\nex:brandt foaf:nick "Lou" .\n`, base: t.sha256, base_text: t.text });
    const nicks = (await q(name, 'PREFIX foaf: <http://xmlns.com/foaf/0.1/> SELECT ?n WHERE { ?p foaf:nick ?n } ORDER BY ?n', ['people.ttl'])).body.rows.map((r) => r[0].value);
    assert('archive studio: two people\'s changes to one file merged by statements, neither lost', theirs.body.merged === true && nicks.join() === 'Lou,Mme F.', JSON.stringify([theirs.body, nicks]));
    const ev = (await call('GET', `/api/projects/${name}/events?after=${since}&wait=0`, undefined, { key: reader.key })).body;
    assert('archive studio: what each did told to the others as events, with who did it', ev.events.some((x) => x.who === 'J. Okafor' && x.what === 'kept a file of linked data') && ev.events.some((x) => x.who === 'the keeper' && x.what.startsWith('merged')), JSON.stringify(ev.events.map((x) => [x.who, x.what])));
    const noRead = await call('PUT', `/api/projects/${name}/linked/notes.ttl`, { text: '@prefix ex: <http://e/> .' }, { key: reader.key });
    const noArrange = await call('PUT', `/api/projects/${name}/arrangement`, { units: [] }, { key: okafor.key });
    const nobody = await call('GET', `/api/projects/${name}/holdings`, undefined, { key: 'not-a-key' });
    assert('archive studio: a reader may not keep, a contributor may not arrange, and a key no one has is refused', noRead.status === 403 && noArrange.status === 403 && nobody.status === 403, `${noRead.status} ${noArrange.status} ${nobody.status}`);
    const cat = (await call('GET', '/api/catalogue?q=elections&limit=3')).body;
    if (cat.harvested) {
      const taken = await call('POST', `/api/projects/${name}/catalogue`, { id: cat.studies[0].id });
      assert('archive studio: a harvested study taken into the project as linked data in DISCO', taken.status === 200 && taken.body.kept === 'studies.ttl' && taken.body.statements > 3);
    } else {
      assert('archive studio: with no catalogue harvested, the catalogue says how to harvest it', /social_studies\.py/.test(cat.message || ''));
    }
  }
  const other = await call('PUT', `/api/projects/${name}/linked/x.ttl`, { text: '' }, { origin: 'http://elsewhere.example' });
  assert('archive studio: a write from another page is refused', other.status === 403, JSON.stringify(other.body));
} finally {
  child.kill();
  try { rmSync(store, { recursive: true, force: true }); } catch { /* a file still held */ }
}
done();
