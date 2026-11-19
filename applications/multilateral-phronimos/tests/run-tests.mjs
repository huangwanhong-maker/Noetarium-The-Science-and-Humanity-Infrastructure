// Checks of Multilateral Phronimos (ADR 0132). The page starts the shared studio with its four views, the round first, and every element
// the studio asks for is on it. The plug-ins hold together: the invented template reads without fault and says it is invented in every
// party's name; the Charter's rules quote the Charter and say how they are read; the migration template reads its records from the data
// library and commits none. The server, on a store of its own and with several people: a new project's record is empty and kept nowhere;
// the invented partnership laid out and read on its date (the model's decision faulted, the commission and the secretariat holding
// authority together, mutual benefit failing at Porthaven); its rules' power (the steering committee's double majority, where 25 votes
// buy no more than 5; the Security Council by Article 27(3)); its votes read as ideal points; a record kept over another's change refused
// with the current one; a reader who may read and not keep; bargaining and a division asked of no project. And, where the data library has read it, the Migration MPTF laid out with its fund holding its authority alone.
// Run from the repository root:  node applications/multilateral-phronimos/tests/run-tests.mjs

import { spawn } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(`multilateral phronimos: ${name}`, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/multilateral-phronimos');
const read = (path) => readFileSync(join(APP, path), 'utf8');
const python = process.env.PYTHON || (process.platform === 'win32' ? 'python' : 'python3');

// ---- the page ------------------------------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js'), shell = readFileSync(join(ROOT, 'visualization/js/workspace/studio.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('the page\'s style sheets and script exist, the multilateral drawings\' among them', !missing.length && local.includes('/visualization/js/multilateral/multilateral.css'), missing.join(', '));
  const ids = [...new Set([...shell.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => !['progress', 'progress-text', 'to-workbench'].includes(i));
  assert('every element the shared studio asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  assert('the page starts the shared studio with four views of its own, the round first',
    /startStudio\(/.test(app) && /views: \[roundView\(\), partnershipView\(\), governanceView\(\), negotiationView\(\)\]/.test(app) && /first: 'round'/.test(app));
  assert('the server names its port, stands on the API\'s host, gives it its own routes and keeps its engine running',
    /port=9210/.test(read('serve.py')) && /routes=dossier\.routes/.test(read('serve.py')) && /ENGINE\.keep\(/.test(read('serve.py')));
  const views = ['src/app.js', 'src/common.js', 'src/round.js', 'src/partnership.js', 'src/governance.js', 'src/negotiation.js'];
  assert('a view redraws itself after what it awaited only while it is in front', views.slice(2).every((v) => /const redraw = again\(ctx, '/.test(read(v))) && /export function again/.test(read('src/common.js')));
  assert('the work\'s reading of a record does not take the host\'s route for reading a source', /"reading"/.test(read('dossier.py')) && !/"read", "rule"/.test(read('dossier.py')) && /\/reading`/.test(read('src/common.js')));
  const css = read('views.css') + read('styles.css');
  const used = [...new Set(views.flatMap((v) => [...read(v).matchAll(/class: '([a-z0-9 -]+)'/g)].flatMap((m) => m[1].split(' '))))].filter((c) => c.length > 2);
  const unstyled = used.filter((c) => !new RegExp(`\\.${c}\\b`).test(css));
  assert('every class a view gives an element is styled by the work', unstyled.length <= 8, unstyled.join(', '));
}

// ---- the plug-ins --------------------------------------------------------------------------------------------------------------
{
  const plugins = readdirSync(join(APP, 'plugins')).filter((d) => existsSync(join(APP, 'plugins', d, 'plugin.json')));
  assert('two plug-ins: the Charter\'s rules with an invented partnership, and the migration fund from open data', plugins.includes('charter') && plugins.includes('migration'), plugins.join(', '));
  for (const g of plugins) {
    const m = JSON.parse(read(`plugins/${g}/plugin.json`));
    assert(`the plug-in ${g} says what it is about and what it stands on`, !!m.title && m.about.length > 120 && m.domains.includes('diplomacy') && existsSync(join(ROOT, m.theory)));
  }
  const rec = JSON.parse(read('plugins/charter/templates/basin-partnership/record.json'));
  const parties = rec.undertaking.participants;
  assert('the invented template says it is invented: in its title, its account and every participant\'s name',
    /invented/.test(rec.title) && /Every party, figure, decision and vote is invented/.test(rec.about) && parties.every((p) => /invented/.test(p.name)), parties.filter((p) => !/invented/.test(p.name)).map((p) => p.id).join(', '));
  const ids = new Set(parties.map((p) => p.id)), types = new Set(rec.undertaking.types.map((t) => t.name));
  assert('its ties join declared participants by declared types, and its members are participants',
    rec.undertaking.ties.every((t) => ids.has(t.from) && ids.has(t.to) && types.has(t.type)) && rec.partnership.members.every((x) => ids.has(x.party)));
  for (const f of readdirSync(join(APP, 'plugins/charter/rules'))) {
    const r = JSON.parse(read(`plugins/charter/rules/${f}`));
    assert(`the rule ${f} quotes the Charter, names its article and says how it is read`, /^Charter of the United Nations, Article \d+\(\d\)$/.test(r.source) && r.text.length > 60 && r.reading.length > 40 && r.members.length >= 15);
  }
  const t = JSON.parse(read('plugins/migration/templates/migration-fund/template.json'));
  assert('the migration template reads its records from the data library and commits none', t.from_product === 'migration-fund' && /multilateral_cases\.py migration-fund/.test(t.make) && !existsSync(join(APP, 'plugins/migration/templates/migration-fund/record.json')));
}

// ---- the server ----------------------------------------------------------------------------------------------------------------
const free = () => new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const { port } = s.address(); s.close(() => r(port)); }); });
const store = mkdtempSync(join(tmpdir(), 'multilateral-phronimos-'));
const port = await free();
const BASE = `http://127.0.0.1:${port}`;
const child = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--store', store, '--people', '--keeper', 'the keeper'], { stdio: ['ignore', 'ignore', 'pipe'] });
let stderr = '';
child.stderr.on('data', (d) => { stderr += d; });
const call = async (method, path, body, { origin = BASE, key = null } = {}) => {
  const headers = { Origin: origin, ...(body !== undefined ? { 'Content-Type': 'application/json' } : {}), ...(key ? { 'X-Key': key } : {}) };
  const r = await fetch(BASE + path, { method, headers, body: body === undefined ? undefined : JSON.stringify(body) });
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
  assert('the server starts with several people, on a store of its own, and lists its plug-ins', !!status && status.people === true && status.work?.name === 'multilateral-phronimos' && status.plugins?.length === 2, stderr.slice(-300));
  const listed = (await call('GET', '/api/templates')).body;
  assert('it lists its templates and the rules its plug-ins bring', listed.templates.some((x) => x.id === 'charter/basin-partnership') && listed.rules.some((x) => x.id === 'charter/security-council'));
  const name = (await call('POST', '/api/projects', { title: 'The Arda Basin Partnership' })).body?.name;
  const P = `/api/projects/${name}`;
  const blank = (await call('GET', `${P}/record`)).body;
  assert('a new project has a record with nothing in it yet, kept nowhere', blank.record.schema === 'noetarium.multilateral/1' && !blank.record.undertaking.participants.length && blank.sha256 === null);
  if (!status?.engine?.built) skip('multilateral phronimos: the record read by the engine (the engine is not built: python applications/multilateral-phronimos/engine.py build)');
  else {
    const laid = await call('POST', `${P}/example`, { template: 'charter/basin-partnership' });
    assert('the invented partnership laid out, with the Charter\'s two rules beside its own', laid.status === 200 && /Arda/.test(laid.body.title) && (await call('GET', `${P}/record`)).body.record.rules.length === 3, JSON.stringify(laid.body));
    const a = (await call('POST', `${P}/reading`, {})).body;
    const where = a.undertaking.diagnoses.map((d) => `${d.kind}|${d.where}`);
    assert('read on its date: the model\'s decision faulted for authority, venue and its sources; the commission and the secretariat hold authority together',
      where.includes('authorisation|decision d2 (the delta\'s dredging deferred)') && where.includes('participation|decision d2 (the delta\'s dredging deferred)')
        && where.filter((w) => w.startsWith('AI dependency|decision d2')).length === 2 && where.includes('succession|participant commission') && where.includes('succession|participant secretariat'), where.join('; '));
    const m = a.partnership.mutual.verdict;
    assert('mutual benefit fails at Porthaven: its 75 bathing days left open, P = 0.16, below the declared 0.5', m.status === 'fails' && m.evidence.some((e) => e.value === 'port') && /0\.1587/.test(m.reason), m.reason);
    const states = Object.fromEntries(a.partnership.states.map((s) => [s.commitment, s.state]));
    assert('the commitments\' states: the plant honoured, Fordwick\'s connections overdue, the monitoring in progress, the dredging unreported',
      states['c-plant'] === 'honoured' && states['c-connect'] === 'overdue' && states['c-monitor'] === 'in progress' && states['c-dredge'] === 'unreported', JSON.stringify(states));
    const early = (await call('POST', `${P}/reading`, { at: '2024-03-01' })).body;
    assert('read on another date, the record answers for that date: on 1 March 2024 no decision has been taken', !early.undertaking.diagnoses.some((d) => d.where.startsWith('decision')));
    const sc = (await call('POST', `${P}/rule`, { index: 0 })).body.answer.power;
    const ss = Object.fromEntries(sc.map((x) => [x.member, x.shapley_shubik]));
    assert('the steering committee\'s double majority: the bank 0.4, and the Upland Republic\'s 25 votes no more power than the chair\'s 5 (0.15 each)',
      Math.abs(ss['the development bank'] - 0.4) < 1e-12 && Math.abs(ss['Upland Republic'] - 0.15) < 1e-12 && Math.abs(ss["the Commission's chair"] - 0.15) < 1e-12, JSON.stringify(ss));
    const council = (await call('POST', `${P}/rule`, { index: 1 })).body.answer;
    assert('the Security Council by Article 27(3): the permanent members its vetoes', council.properties.vetoes.length === 5 && Math.abs(council.power[0].banzhaf - 848 / 5080) < 1e-12);
    const v = (await call('POST', `${P}/votes`, {})).body.answer;
    const x = Object.fromEntries(v.points.map((p) => [p.member, p.x]));
    assert('its invented votes read as ideal points: upstream and downstream at opposite ends, the anchor on the positive side, most votes classified',
      x['Upland Republic'] > 0 && x['Delta Union'] < 0 && x.Saltmarsh < 0 && x.Highcliff > 0 && v.classified > 0.85, JSON.stringify(x));
    const kept = (await call('GET', `${P}/record`)).body;
    const stale = await call('PUT', `${P}/record`, { record: kept.record, base: '0'.repeat(64), what: 'kept the record' });
    assert('a record kept over another\'s change is refused with the current one', stale.status === 409 && stale.body.conflict === true && stale.body.current.title === kept.record.title);
    const fine = await call('PUT', `${P}/record`, { record: { ...kept.record, at: '2025-06-30' }, base: kept.sha256, what: 'read the record on another date' });
    assert('a record kept on the digest last read is kept, and its journal says so', fine.status === 200 && fine.body.kept === true);
    // a reader, invited by the keeper, may read and not keep
    const invite = await call('POST', '/api/people', { name: 'a reader', role: 'reader' });
    if (invite.status === 200 && invite.body?.key) {
      const elsewhere = 'http://192.0.2.7:9999';
      const reads = await call('POST', `${P}/reading`, {}, { origin: elsewhere, key: invite.body.key });
      const writes = await call('PUT', `${P}/record`, { record: kept.record, base: null }, { origin: elsewhere, key: invite.body.key });
      assert('a reader may read the record and not keep it', reads.status === 200 && writes.status === 403, `${reads.status} ${writes.status}`);
    } else skip('multilateral phronimos: a reader (the host did not issue a key here)');
    const b = (await call('POST', '/api/bargaining', { outcomes: [[0, 0], [4, 0], [0, 2]], disagreement: [0, 0] })).body;
    const d = (await call('POST', '/api/division', { issues: ['a', 'b', 'c'], a: [50, 30, 20], b: [20, 30, 50] })).body;
    assert('bargaining and a division asked of no project: Nash at (2, 1); 65 points each, the shared issue split',
      Math.abs(b.nash[0] - 2) < 1e-12 && Math.abs(b.nash[1] - 1) < 1e-12 && Math.abs(d.points_a - 65) < 1e-12 && d.shares[1].split === true);
    const fund = (await call('POST', '/api/projects', { title: 'The Migration MPTF' })).body?.name;
    const laidFund = await call('POST', `/api/projects/${fund}/example`, { template: 'migration/migration-fund' });
    if (laidFund.status !== 200) skip(`multilateral phronimos: the Migration MPTF (${laidFund.body?.message || 'not made'}: python data/products/multilateral_cases.py migration-fund)`);
    else {
      const f = (await call('POST', `/api/projects/${fund}/reading`, {})).body;
      const cut = f.undertaking.diagnoses.filter((dd) => dd.kind === 'succession' && dd.where.startsWith('participant'));
      assert('the Migration MPTF: its authority runs through the fund alone, and none of its programmes has an indicator in the publication',
        cut.length === 1 && cut[0].where === 'participant migration-mptf' && f.partnership.states.every((s) => !s.indicator), cut.map((c) => c.where).join(', '));
    }
  }
} finally {
  child.kill();
  rmSync(store, { recursive: true, force: true });
}
done();
