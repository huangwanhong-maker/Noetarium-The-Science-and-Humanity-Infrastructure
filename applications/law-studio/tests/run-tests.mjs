// Checks of the Law Studio (ADR 0121). The page starts the shared studio with its five views. Each plug-in's schemes and templates
// hold together: every passage a template's matter cites stands in its document, every exhibit names a document that is there, and a
// scheme of real law carries no clause's words in the repository (they are fetched from the official text). The server, on a store
// of its own and with several people: the park's matter laid out and read by the engine (the offence arguable on how "vehicle" is
// read; the camera's note not authentic; a foreign court persuading; a holding after the question's date left out, and binding once
// the date passes it, so that the offence becomes untenable); a fact's standing changed and the plan of proof following; a matter
// kept over another's change refused with the current one; a document added with its digest; a model asked only from the work's own
// page; no key ever given to a page; a reader who may not keep. What a model proposes is held with a stand-in (check_models.py).
// Where the official texts are in the cache, the privacy scheme is whole and its example matter is read: three readings fixed by
// the Court of Justice, the prohibition arguable, its plan of four ways and three.
// Run from the repository root:  node applications/law-studio/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(`law studio: ${name}`, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/law-studio');
const read = (path) => readFileSync(join(APP, path), 'utf8');
const python = process.env.PYTHON || (process.platform === 'win32' ? 'python' : 'python3');
const squeeze = (s) => s.replace(/\s+/g, ' ').trim();

// ---- the page ------------------------------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js'), shell = readFileSync(join(ROOT, 'visualization/js/workspace/studio.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('the page\'s style sheets and script exist, the evidence drawings\' among them', !missing.length && local.includes('/visualization/js/evidence/evidence.css'), missing.join(', '));
  const ids = [...new Set([...shell.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => !['progress', 'progress-text', 'to-workbench'].includes(i));
  assert('every element the shared studio asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  assert('the page starts the shared studio with five views of its own, the matter first',
    /startStudio\(/.test(app) && /views: \[matterView\(\), evidenceView\(\), lawView\(\), readingsView\(\), planView\(\)\]/.test(app) && /first: 'matter'/.test(app));
  assert('the server names its port, stands on the API\'s host, gives it its own routes and keeps its engine running',
    /port=9170/.test(read('serve.py')) && /routes=matters\.routes/.test(read('serve.py')) && /ENGINE\.keep\(/.test(read('serve.py')));
  const sources = ['src/app.js', 'src/common.js', 'src/matter.js', 'src/evidence.js', 'src/law.js', 'src/readings.js', 'src/plan.js', 'matters.py', 'serve.py', 'engine.py'].map(read).join('\n');
  assert('no key is written in the work: it is kept in the person\'s settings folder', !/sk-ant-/.test(sources) && /settings_folder/.test(readFileSync(join(ROOT, 'api/python/science/models.py'), 'utf8')));
}

// ---- the plug-ins --------------------------------------------------------------------------------------------------------------
const plugins = readdirSync(join(APP, 'plugins')).filter((d) => existsSync(join(APP, 'plugins', d, 'plugin.json')));
{
  assert('two plug-ins: an invented bylaw as an example, and international privacy law', plugins.includes('park') && plugins.includes('privacy'), plugins.join(', '));
  for (const g of plugins) {
    const schemes = readdirSync(join(APP, 'plugins', g, 'schemes')).filter((f) => f.endsWith('.json')).map((f) => JSON.parse(read(`plugins/${g}/schemes/${f}`)));
    for (const s of schemes) {
      const provisions = new Set(s.instruments.flatMap((i) => i.provisions.map((p) => p.id)));
      const cited = [...s.rules, ...s.exceptions, ...s.counts];
      const facts = new Set(s.vocabulary.map((v) => v.id)), terms = new Map(s.terms.map((t) => [t.name, t.readings.map((r) => r.name || r)]));
      assert(`the scheme ${g}/${s.id} names itself and its date; every rule, exception and link is cited to provisions it lists; every holding reads a declared term in a way it admits`,
        s.schema === 'noetarium.scheme/1' && !!s.title && !!s.as_of && cited.every((x) => (x.cites || []).length && x.cites.every((c) => provisions.has(c)))
          && s.holdings.every((h) => terms.has(h.term) && terms.get(h.term).includes(h.reading) && s.courts.some((c) => c.id === h.court))
          && s.counts.every((k) => terms.get(k.term)?.includes(k.reading) && facts.has(k.as)),
        cited.filter((x) => !(x.cites || []).length || !x.cites.every((c) => provisions.has(c))).map((x) => x.id).join(', '));
    }
    const dir = join(APP, 'plugins', g, 'templates');
    for (const t of existsSync(dir) ? readdirSync(dir) : []) {
      const m = JSON.parse(read(`plugins/${g}/templates/${t}/matter.json`));
      const docs = new Map(readdirSync(join(dir, t, 'documents')).map((f) => [f, squeeze(readFileSync(join(dir, t, 'documents', f), 'utf8'))]));
      const lost = m.candidates.filter((c) => !docs.get(c.document)?.includes(squeeze(c.passage))).map((c) => c.id);
      assert(`the template ${g}/${t}: every passage its matter cites stands in its document, every exhibit names a document that is there, and every document says it is invented`,
        m.schema === 'noetarium.matter/1' && !lost.length && m.exhibits.every((x) => docs.has(x.document)) && [...docs.values()].every((d) => /invented/.test(d)) && /invented/i.test(m.title), lost.join(', '));
    }
  }
  const transfers = JSON.parse(read('plugins/privacy/schemes/transfers.json'));
  const eu = transfers.instruments.filter((i) => /eur-lex/.test(i.url || ''));
  assert('the privacy scheme carries no word of the Union\'s official texts: each clause is named, and read from the text fetched', eu.length >= 6 && eu.every((i) => i.provisions.every((p) => !p.text)));
  assert('it says where it stands: its date, what is under appeal, what is not in force, what it does not encode, and that it is not legal advice',
    /C-703\/25 P/.test(transfers.about) && /not in force/.test(transfers.about) && /Not encoded yet/.test(transfers.about) && /not legal advice/.test(transfers.about));
}

// ---- what a model proposes, with a stand-in -----------------------------------------------------------------------------------------
{
  const r = spawnSync(python, [join(APP, 'tests/check_models.py')], { cwd: ROOT, encoding: 'utf8', timeout: 600000, env: { ...process.env, PYTHONIOENCODING: 'utf-8' } });
  const lines = (r.stdout || '').split('\n').filter((l) => l.startsWith('{')).map((l) => JSON.parse(l));
  if (!lines.length) assert('check_models.py ran', false, (r.stderr || 'no answer').trim().split('\n').slice(-3).join(' | '));
  for (const x of lines) { if (x.skip) skip(`law studio: what a model proposes (${x.name})`); else assert(x.name, x.ok, x.detail); }
}

// ---- the server ----------------------------------------------------------------------------------------------------------------
const free = () => new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const { port } = s.address(); s.close(() => r(port)); }); });
const store = mkdtempSync(join(tmpdir(), 'law-studio-'));
const port = await free();
const BASE = `http://127.0.0.1:${port}`;
const child = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store, '--people', '--keeper', 'the keeper'], { stdio: ['ignore', 'ignore', 'pipe'] });
let stderr = '';
child.stderr.on('data', (d) => { stderr += d; });
const call = async (method, path, body, { origin = BASE, key = null, raw = null, run = false } = {}) => {
  const headers = { Origin: origin, ...(body !== undefined ? { 'Content-Type': 'application/json' } : {}), ...(key ? { 'X-Key': key } : {}), ...(run ? { 'X-Studio': 'run' } : {}) };
  const r = await fetch(BASE + path, { method, headers, body: raw ?? (body === undefined ? undefined : JSON.stringify(body)) });
  let json = null;
  try { json = await r.json(); } catch { /* none */ }
  return { status: r.status, body: json };
};
try {
  let status = null;
  for (let i = 0; i < 180 && !status; i++) {
    await new Promise((r) => setTimeout(r, 500));
    try { status = (await call('GET', '/api/status')).body; } catch { /* not up */ }
  }
  assert('the server starts with several people, on a store of its own, and lists its plug-ins', !!status && status.people === true && status.work?.name === 'law-studio' && status.plugins?.length === 2, stderr.slice(-300));
  const listed = (await call('GET', '/api/schemes')).body;
  assert('it lists the schemes and templates its plug-ins bring', listed.schemes.some((s) => s.id === 'park/park-bylaw') && listed.schemes.some((s) => s.id === 'privacy/transfers') && listed.templates.length === 2);
  const keys = await call('GET', '/api/models');
  assert('it says whether a key is kept for a provider and how it ends, never the key, and where it is kept without naming the account', keys.status === 200 && typeof keys.body.providers.anthropic.kept === 'boolean' && !/sk-ant-/.test(JSON.stringify(keys.body)) && !/Users/.test(keys.body.kept_in));
  const name = (await call('POST', '/api/projects', { title: 'Council of Avalon v Rider' })).body?.name;
  if (!status?.engine?.built) skip('law studio: the matter read by the engine (the engine is not built: python applications/law-studio/engine.py build)');
  else {
    const P = `/api/projects/${name}`;
    const laid = await call('POST', `${P}/example`, { template: 'park/the-bicycle' });
    assert('the park\'s matter laid out: three documents kept', laid.status === 200 && laid.body.documents.length === 3, JSON.stringify(laid.body));
    const scheme = (await call('GET', '/api/schemes/park/park-bylaw')).body;
    assert('its scheme is whole, and needs no fetching: an invented bylaw carries its own words', scheme.check.faults.length === 0 && scheme.unfetched.length === 0 && scheme.check.counts.rules === 2, JSON.stringify(scheme.check));
    const ev = (await call('POST', `${P}/evaluate`, {})).body.evaluation;
    const issue = ev.issues[0], held = Object.fromEntries(ev.authority.assessed.map((h) => [h.holding, h]));
    assert('the offence is arguable: it follows only if "vehicle" is read broadly, and rests on sections 1(1) and 2',
      issue.class === 'arguable' && issue.label.join() === '[vehicle=broad]' && issue.rests_on.steps.some((s) => s.id === 'offence_rule' && s.provisions[0].label === 'Section 2'), JSON.stringify(issue.label));
    assert('the foreign court persuades and does not bind; the home court\'s holding of 2027 is left out, the question being asked as of 2026',
      held.skater.force === 'persuasive' && held.pram.force === 'left out' && /after the date of the question/.test(held.pram.reason), JSON.stringify(ev.authority.assessed.map((h) => [h.holding, h.force])));
    const auth = Object.fromEntries(ev.evidence.authenticity.map((a) => [a.exhibit, a.verdict]));
    assert('archeion follows custody: the warden\'s log is authentic; the camera\'s note, handed on by an office that never held it, is not',
      auth['x-log'].status === 'holds' && auth['x-still'].status === 'fails' && /handed over by the enforcement office/.test(auth['x-still'].reason), JSON.stringify(auth));
    assert('the plan of proof: two ways to establish the offence, two to resist it, and in every way the emergency shown not to hold',
      issue.plan.establish.length === 2 && issue.plan.resist.length === 2 && issue.plan.must_disprove.join() === 'emergency' && issue.plan.open.join() === 'emergency,motor_assisted', JSON.stringify(issue.plan));
    // The date passes the home court's holding: it binds, the narrow reading is fixed, and a bicycle without a motor is no vehicle.
    const got = (await call('GET', `${P}/matter`)).body;
    const later = { ...got.matter, date: '2027-03-01' };
    const kept = await call('PUT', `${P}/matter`, { matter: later, base: got.sha256, what: 'set the date of the question' });
    const ev2 = (await call('POST', `${P}/evaluate`, {})).body.evaluation;
    assert('asked as of March 2027 the Supreme Court\'s holding binds, the narrow reading is fixed and the offence is untenable on the bicycle alone',
      kept.status === 200 && ev2.authority.assessed.find((h) => h.holding === 'pram').force === 'binding' && ev2.admissibility.fixed.join() === '[vehicle=narrow]' && ev2.issues[0].class === 'untenable',
      JSON.stringify([kept.body, ev2.admissibility, ev2.issues[0].class]));
    const stale = await call('PUT', `${P}/matter`, { matter: { ...later, title: 'Another title' }, base: got.sha256 });
    assert('a matter kept over a change made meanwhile is refused, with the current one', stale.status === 409 && stale.body.current.date === '2027-03-01', JSON.stringify(stale.body).slice(0, 200));
    // The motor found: the offence follows under the narrow reading too.
    const now = (await call('GET', `${P}/matter`)).body;
    const found = { ...now.matter, facts: now.matter.facts.map((f) => (f.id === 'motor_assisted' ? { ...f, standing: 'found' } : f)) };
    await call('PUT', `${P}/matter`, { matter: found, base: now.sha256, what: 'set a fact\'s standing' });
    const ev3 = (await call('POST', `${P}/evaluate`, {})).body.evaluation;
    assert('with the motor found, the offence is clear whatever the reading, and only the emergency is left to resist it',
      ev3.issues[0].class === 'clear' && ev3.issues[0].plan.resist.length === 1 && ev3.issues[0].plan.resist[0].prove.join() === 'emergency', JSON.stringify(ev3.issues[0].plan));
    const doc = await call('POST', `${P}/documents?name=${encodeURIComponent('A note.txt')}`, undefined, { raw: 'The bandstand was quiet.\n' });
    const text = (await call('GET', `${P}/documents/${encodeURIComponent('A note.txt')}`)).body;
    assert('a document added is kept with the SHA-256 of its bytes, and its text read', doc.status === 200 && doc.body.sha256 === text.sha256 && doc.body.sha256.length === 64 && text.text.includes('bandstand'), JSON.stringify(doc.body));
    const unasked = await call('POST', `${P}/recognise`, { document: 'A note.txt' });
    const unread = await call('POST', `${P}/readings`, {});
    assert('a model is asked only from the work\'s own page, as a script is run: a bare request is refused', unasked.status === 403 && unread.status === 403, `${unasked.status} ${unread.status}`);
    const journal = (await call('GET', `${P}/journal`)).body;
    assert('every change is in the project\'s sealed journal, with who made it', (journal.entries || []).some((e) => e.what === 'laid out a template' && e.who === 'the keeper') && (journal.entries || []).some((e) => e.what === 'kept a document'), JSON.stringify(journal).slice(0, 300));
    const reader = (await call('POST', '/api/people', { name: 'R. Lind', role: 'reader' })).body;
    const sees = await call('GET', `${P}/matter`, undefined, { key: reader.key });
    const keeps = await call('PUT', `${P}/matter`, { matter: found }, { key: reader.key });
    const computes = await call('POST', `${P}/evaluate`, {}, { key: reader.key });
    assert('a reader sees the matter and may have it evaluated, and may not keep a change', sees.status === 200 && computes.status === 200 && keeps.status === 403, `${sees.status} ${computes.status} ${keeps.status}`);
    const other = await call('PUT', `${P}/matter`, { matter: found }, { origin: 'http://elsewhere.example' });
    assert('a write from another page is refused', other.status === 403);

    // International privacy law, where the official texts are in the cache.
    const privacy = (await call('GET', '/api/schemes/privacy/transfers')).body;
    if (privacy.unfetched.length) skip(`law studio: the privacy scheme against its official texts (not fetched: ${privacy.unfetched.join(', ')}; python data/products/legal_texts.py)`);
    else {
      assert('the privacy scheme is whole against the official texts: every quotation stands in its clause, every holding\'s passage in its judgment',
        privacy.check.faults.length === 0 && privacy.check.counts.rules === 16 && privacy.check.counts.holdings === 4, JSON.stringify(privacy.check.faults));
      const art46 = privacy.scheme.instruments[0].provisions.find((p) => p.id === 'gdpr/art-46/2/c');
      assert('a clause is read in the Official Journal\'s words, in its place under its article and paragraph',
        art46.label === 'Article 46(2)(c)' && /^standard data protection clauses adopted by the Commission/.test(art46.text) && art46.parent === 'gdpr/art-46/2' && privacy.scheme.instruments[0].provisions.some((p) => p.id === 'gdpr/art-46'));
      const n2 = (await call('POST', '/api/projects', { title: 'Whelan v Halden Care' })).body.name;
      await call('POST', `/api/projects/${n2}/example`, { template: 'privacy/a-transfer-to-the-united-states' });
      const e = (await call('POST', `/api/projects/${n2}/evaluate`, {})).body.evaluation;
      const hs = Object.fromEntries(e.authority.assessed.map((h) => [h.holding, h.force]));
      assert('the Court of Justice\'s readings bind the Irish authority and fix two terms; the Board\'s guidance persuades and, unopposed, fixes a third',
        hs['breyer-49'] === 'binding' && hs['srb-86'] === 'binding' && hs['schrems-ii-105'] === 'binding' && hs['edpb-2-2018'] === 'persuasive'
          && e.admissibility.fixed.join() === '[identifiable=relative],[appropriate_safeguards=essentially_equivalent],[occasional=required]', JSON.stringify(e.admissibility));
      const facts = Object.fromEntries(e.facts.map((f) => [f.id, f]));
      assert('the clauses are proved by the agreement, an authentic exhibit; whether the recipient can identify the patients is disputed and stays alleged',
        facts.sccs_concluded.used === 'proved' && facts.recipient_can_identify.used === 'alleged' && e.evidence.recognised.find((x) => x.proposition === 'recipient_can_identify').standing === 'disputed'
          && e.evidence.authenticity.find((a) => a.exhibit === 'x-letter').verdict.status === 'fails');
      const i = e.issues[0];
      assert('whether the transfer is forbidden is arguable: under two of the four admissible interpretations, turning on whose side "personal data" is judged from',
        i.class === 'arguable' && i.admissible === 4 && i.admissible_derivable === 2 && i.label.some((l) => l.includes('[chapter_v_view=exporter]')), JSON.stringify([i.admissible, i.admissible_derivable, i.label]));
      assert('its plan: four ways to establish it, three to resist it (effective measures; no access in practice, if practice counts; not personal for the recipient, if the recipient\'s side is the test)',
        i.plan.establish.length === 4 && i.plan.resist.length === 3 && i.plan.resist.some((w) => w.prove.join() === 'supplementary_measures_effective' && !w.readings.length)
          && i.plan.resist.some((w) => w.prove.join() === 'no_access_in_practice' && w.readings.join() === '[laws_and_practices=practice_counts]')
          && i.plan.resist.some((w) => w.disprove.join() === 'recipient_can_identify' && w.readings.join() === '[chapter_v_view=recipient]'), JSON.stringify(i.plan.resist));
      assert('the derogations rest on facts no one alleges, and are left out of the plan until someone does', i.plan.absent.includes('explicit_consent') && i.plan.absent.includes('necessary_for_legal_claims'));
    }
  }
} finally {
  child.kill();
  try { rmSync(store, { recursive: true, force: true }); } catch { /* a file still held */ }
}
done();
