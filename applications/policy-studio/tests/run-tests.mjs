// Checks of the Policy Studio (ADR 0122). The page starts the shared studio with its five views. Each plug-in's templates and
// schemes hold together: a template's bodies are set on claims its theory states, its place takes its effect from a body it has,
// its decision is on an option it states, and it says what in it is invented; the BCG template's counts are the published ones,
// row by row against peira's fixture. The server, on a store of its own and with several people: the BCG question laid out and
// worked by the engine (the claim from vaccination to cases shown at low on thirteen trials; the question not yet shown, for the
// claim before it has no evidence; the effect carried to the district through the strata; adopt, turning on the worth of a case);
// a reader's assessment changed and the grade following; evidence written on the first claim and the question then shown; a
// question kept over another's change refused with the current one; a theory read and a pasted table read without a project; a
// model asked only from the work's own page; no key ever given to a page; a reader who may not keep. The invented question of school
// meals: two paths, and the decision to study first. What a model drafts is held with a stand-in (check_draft.py).
// Run from the repository root:  node applications/policy-studio/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(`policy studio: ${name}`, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/policy-studio');
const read = (path) => readFileSync(join(APP, path), 'utf8');
const python = process.env.PYTHON || (process.platform === 'win32' ? 'python' : 'python3');

// ---- the page ------------------------------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js'), shell = readFileSync(join(ROOT, 'visualization/js/workspace/studio.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('the page\'s style sheets and script exist, the policy drawings\' among them', !missing.length && local.includes('/visualization/js/policy/policy.css'), missing.join(', '));
  const ids = [...new Set([...shell.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => !['progress', 'progress-text', 'to-workbench'].includes(i));
  assert('every element the shared studio asks for is on the page', ids.length > 12 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  assert('the page starts the shared studio with five views of its own, the theory first',
    /startStudio\(/.test(app) && /views: \[theoryView\(\), evidenceView\(\), hereView\(\), optionsView\(\), decisionView\(\)\]/.test(app) && /first: 'theory'/.test(app));
  assert('the server names its port, stands on the API\'s host, gives it its own routes and keeps its engine running',
    /port=9190/.test(read('serve.py')) && /routes=questions\.routes/.test(read('serve.py')) && /ENGINE\.keep\(/.test(read('serve.py')));
  const views = ['src/app.js', 'src/common.js', 'src/theory.js', 'src/evidence.js', 'src/here.js', 'src/options.js', 'src/decision.js'];
  const sources = [...views, 'questions.py', 'serve.py', 'engine.py'].map(read).join('\n');
  assert('no key is written in the work: it is kept in the person\'s settings folder', !/sk-ant-/.test(sources) && /settings_folder/.test(read('questions.py')));
  assert('the page never reads the theory\'s written form: it writes it back, and the domain\'s reader reads it', views.every((v) => !/split\('->'\)|match\(\/\^node /.test(read(v))) && /\/api\/theory/.test(read('src/common.js')) && /writeTheory/.test(read('src/theory.js')));
  const css = read('views.css');
  const used = [...new Set(views.flatMap((v) => [...read(v).matchAll(/class: '([a-z0-9 -]+)'/g)].flatMap((m) => m[1].split(' '))))].filter((c) => c.length > 2);
  const unstyled = used.filter((c) => !new RegExp(`\\.${c}\\b`).test(css) && !new RegExp(`\\.${c}\\b`).test(read('styles.css')));
  assert('every class a view gives an element is styled by the work', unstyled.length <= 6, unstyled.join(', '));
}

// ---- the plug-ins --------------------------------------------------------------------------------------------------------------
const plugins = readdirSync(join(APP, 'plugins')).filter((d) => existsSync(join(APP, 'plugins', d, 'plugin.json')));
{
  assert('two plug-ins: public health on real trials, and schooling as an invented example', plugins.includes('health') && plugins.includes('schooling'), plugins.join(', '));
  for (const g of plugins) {
    const manifest = JSON.parse(read(`plugins/${g}/plugin.json`));
    assert(`the plug-in ${g} says what it is about and what it stands on`, !!manifest.title && manifest.about.length > 80 && manifest.domains.includes('policy') && existsSync(join(ROOT, manifest.theory)));
    for (const f of readdirSync(join(APP, 'plugins', g, 'schemes')).filter((x) => x.endsWith('.json'))) {
      const s = JSON.parse(read(`plugins/${g}/schemes/${f}`));
      const factors = [...s.text.matchAll(/^(?:down|up) (\S+) \d+$/gm)].map((m) => m[1]);
      assert(`the scheme ${g}/${f} declares its levels, where designs start and its factors; every rule reads a factor it declares; and it says it is an example`,
        /^scheme \S+\nlevels (\S+ )+\S+\n(start \S+ \S+\n)+/.test(s.text) && factors.length >= 3 && s.rules.every((r) => factors.includes(r.factor) && r.values.length) && /example/i.test(s.title) && /not GRADE's own text|invented/.test(s.about), s.title);
    }
    const dir = join(APP, 'plugins', g, 'templates');
    for (const t of existsSync(dir) ? readdirSync(dir) : []) {
      const q = JSON.parse(read(`plugins/${g}/templates/${t}/question.json`));
      const claims = new Set([...q.theory.matchAll(/^link (\S+) -> (\S+):/gm)].map((m) => `${m[1]} -> ${m[2]}`)), states = new Set([...q.theory.matchAll(/^node (\S+) /gm)].map((m) => m[1]));
      const loose = q.bodies.filter((b) => b.link.from && !claims.has(`${b.link.from} -> ${b.link.to}`)).map((b) => b.id);
      assert(`the template ${g}/${t}: its bodies are set on claims its theory states, its place takes the effect from a body it has, its decision is on an option it states, and it asks from and about states of the theory`,
        q.schema === 'noetarium.policy/1' && !loose.length && q.bodies.some((b) => b.id === q.place.body) && q.options.some((o) => o.name === q.decision.option) && states.has(q.intervention) && states.has(q.outcome)
          && q.theory.split('\n')[1] === q.scheme.split('\n')[1], loose.join(', '));
      assert(`the template ${g}/${t} says what in it is invented`, /invented/.test(q.about) && /invented/i.test(q.place.name) && (g !== 'schooling' || (/invented/i.test(q.title) && q.bodies.every((b) => /Invented/.test(b.source) && b.studies.every((s) => /invented/.test(s.label))))));
    }
  }
  // The BCG trials are the published ones: each row against peira's fixture (Colditz et al. 1994, as dat.bcg of metadat).
  const fixture = readFileSync(join(ROOT, 'foundations/peira/synthesis/tests/fixtures/bcg.csv'), 'utf8').split('\n').filter((l) => /^\d/.test(l)).map((l) => l.split(',').map(Number));
  const bcg = JSON.parse(read('plugins/health/templates/bcg-vaccination/question.json'));
  const same = fixture.length === 13 && bcg.bodies[0].studies.length === 13 && fixture.every(([trial, year, tpos, tneg, cpos, cneg, ablat], k) => {
    const s = bcg.bodies[0].studies[k];
    return s.id === String(trial) && s.year === year && s.treated_events === tpos && s.treated_others === tneg && s.control_events === cpos && s.control_others === cneg && s.latitude === ablat && s.stratum === (ablat < 30 ? 'under 30°' : '30° or more');
  });
  assert('the BCG template\'s thirteen trials are the published ones, count for count, each in the stratum of its latitude; it names its source and says it is no advice', same && /Colditz/.test(bcg.bodies[0].source) && /None of this is advice/.test(bcg.about));
}

// ---- what a model drafts, with a stand-in ------------------------------------------------------------------------------------------
{
  const r = spawnSync(python, [join(APP, 'tests/check_draft.py')], { cwd: ROOT, encoding: 'utf8', timeout: 600000, env: { ...process.env, PYTHONIOENCODING: 'utf-8' } });
  const lines = (r.stdout || '').split('\n').filter((l) => l.startsWith('{')).map((l) => JSON.parse(l));
  if (!lines.length) assert('check_draft.py ran', false, (r.stderr || 'no answer').trim().split('\n').slice(-3).join(' | '));
  for (const x of lines) { if (x.skip) skip(`policy studio: what a model drafts (${x.name})`); else assert(x.name, x.ok, x.detail); }
}

// ---- the server ----------------------------------------------------------------------------------------------------------------
const free = () => new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const { port } = s.address(); s.close(() => r(port)); }); });
const store = mkdtempSync(join(tmpdir(), 'policy-studio-'));
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
  assert('the server starts with several people, on a store of its own, and lists its plug-ins', !!status && status.people === true && status.work?.name === 'policy-studio' && status.plugins?.length === 2, stderr.slice(-300));
  const listed = (await call('GET', '/api/schemes')).body;
  assert('it lists the grading schemes and the templates its plug-ins bring', listed.schemes.some((s) => s.id === 'health/after-grade') && listed.schemes.some((s) => s.id === 'schooling/three-levels') && listed.templates.length === 2);
  const keys = await call('GET', '/api/models');
  assert('it says whether a key is kept for a provider and how it ends, never the key, and where it is kept without naming the account', keys.status === 200 && typeof keys.body.providers.anthropic.kept === 'boolean' && !/sk-ant-/.test(JSON.stringify(keys.body)) && !/Users/.test(keys.body.kept_in));
  const name = (await call('POST', '/api/projects', { title: 'BCG vaccination in the district' })).body?.name;
  const P = `/api/projects/${name}`;
  const blank = (await call('GET', `${P}/question`)).body;
  assert('a new project has a question with nothing in it yet, kept nowhere', blank.question.schema === 'noetarium.policy/1' && blank.question.theory === '' && blank.sha256 === null);
  if (!status?.engine?.built) skip('policy studio: the question worked by the engine (the engine is not built: python applications/policy-studio/engine.py build)');
  else {
    const unbegun = await call('POST', `${P}/appraise`, {});
    assert('a question with no theory is not appraised, and is told why', unbegun.status >= 400 && /no theory of change/.test(unbegun.body.message), JSON.stringify(unbegun.body));
    const laid = await call('POST', `${P}/example`, { template: 'health/bcg-vaccination' });
    assert('the BCG question laid out', laid.status === 200 && /BCG/.test(laid.body.question), JSON.stringify(laid.body));
    const a = (await call('POST', `${P}/appraise`, {})).body.appraisal;
    const b = a.bodies[0];
    assert('the thirteen trials pool to a risk ratio of 0.489 [0.344, 0.696], graded low; their claim is shown at low',
      Math.abs(Math.exp(b.pooled.estimate) - 0.4894) < 5e-4 && Math.abs(Math.exp(b.pooled.lower) - 0.3441) < 5e-4 && Math.abs(Math.exp(b.pooled.upper) - 0.6962) < 5e-4 && b.grade === 'low' && a.theory.links[1].level === 'low', JSON.stringify(b.pooled));
    assert('the question is not yet shown: whether the programme reaches the newborn has no evidence, and every path passes through it',
      a.assessment.verdict.status === 'undecided' && a.assessment.gaps.join() === '0' && a.theory.links[0].rank === -1, JSON.stringify(a.assessment.verdict));
    assert('the effect is carried to the district through the five trials below 30° of latitude: a ratio of 0.688, not 0.489; two support factors are not known there',
      a.here.transported && Math.abs(Math.exp(a.here.effect) - 0.6878) < 5e-4 && a.here.strata[0].k === 5 && a.here.unknown.length === 2 && a.here.verdict.status === 'undecided', JSON.stringify([a.here.effect, a.here.unknown]));
    assert('the decision is to adopt, turning on the worth of a case averted; no study of the sizes considered is worth more than deciding now',
      a.decision.chosen.join() === 'adopt' && a.decision.robust === false && a.decision.study < a.decision.adopt && a.options.chosen.join() === 'Vaccinate every newborn', JSON.stringify(a.decision.verdict));
    // The reader withdraws a step, and the grade follows.
    const got = (await call('GET', `${P}/question`)).body;
    const eased = JSON.parse(JSON.stringify(got.question));
    eased.bodies[0].assessed = {};
    const kept = await call('PUT', `${P}/question`, { question: eased, base: got.sha256, what: 'assessed a factor' });
    const a2 = (await call('POST', `${P}/appraise`, {})).body.appraisal;
    assert('with the reader\'s step for risk of bias withdrawn, the trials are graded moderate and their claim shown at moderate', kept.status === 200 && a2.bodies[0].grade === 'moderate' && a2.theory.links[1].level === 'moderate', JSON.stringify([kept.body, a2.bodies[0].grade]));
    const stale = await call('PUT', `${P}/question`, { question: { ...eased, title: 'Another title' }, base: got.sha256 });
    assert('a question kept over a change made meanwhile is refused, with the current one', stale.status === 409 && Object.keys(stale.body.current.bodies[0].assessed).length === 0, JSON.stringify(stale.body).slice(0, 200));
    // Evidence written on the first claim, without its studies: the question is then shown.
    const now = (await call('GET', `${P}/question`)).body;
    const written = { ...now.question, theory: `${now.question.theory}evidence programme -> vaccinated: 0.85 [0.8, 0.9] moderate\n` };
    await call('PUT', `${P}/question`, { question: written, base: now.sha256, what: 'rewrote the theory' });
    const a3 = (await call('POST', `${P}/appraise`, {})).body.appraisal;
    assert('evidence written on the first claim at moderate, the question is shown at moderate: a chain is as well shown as its weakest claim',
      a3.assessment.verdict.status === 'holds' && a3.assessment.strongest.weakest === 'moderate' && a3.assessment.gaps.length === 0, JSON.stringify(a3.assessment.verdict));
    const asked = (await call('POST', `${P}/appraise`, { question: { ...written, least: 'high' } })).body.appraisal;
    const unchanged = (await call('GET', `${P}/question`)).body.question.least;
    assert('a question may be appraised as it would be, without being kept: asked for high, it is not shown, and the question kept still asks for moderate', asked.assessment.verdict.status === 'undecided' && unchanged === 'moderate');
    const journal = (await call('GET', `${P}/journal`)).body;
    assert('every change is in the project\'s sealed journal, with who made it', (journal.entries || []).some((e) => e.what === 'laid out a template' && e.who === 'the keeper') && (journal.entries || []).some((e) => e.what === 'assessed a factor'), JSON.stringify(journal).slice(0, 300));

    const theory = await call('POST', '/api/theory', { text: 'theory t\nlevels low high\nnode a intervention: A\nnode b outcome: B\nlink a -> b: a brings b\nnode c\n' });
    assert('a theory is read without a project, and a line that does not read is named', theory.status === 200 && theory.body.ok === false && /^line 6/.test(theory.body.errors[0]), JSON.stringify(theory.body));
    const table = await call('POST', '/api/studies', { csv: 'label,effect,se\nNorth,0.09,0.05\nSouth,0.04,0.06\n' });
    const wrong = await call('POST', '/api/studies', { csv: 'label,effect,se\nNorth,much,0.05\n' });
    assert('a pasted table is read as studies, and one that does not read is refused with its line', table.body.studies.length === 2 && Math.abs(table.body.studies[0].variance - 0.0025) < 1e-12 && wrong.status >= 400 && /line 2: effect is a number/.test(wrong.body.message), JSON.stringify(wrong.body));

    const undrafted = await call('POST', `${P}/draft`, { description: 'Free meals at school, so that pupils attend and learn more.' }, { origin: 'http://elsewhere.example' });
    assert('a model is asked only from the work\'s own page, as a script is run: a request from elsewhere is refused', undrafted.status === 403, `${undrafted.status}`);
    const reader = (await call('POST', '/api/people', { name: 'R. Lind', role: 'reader' })).body;
    const sees = await call('GET', `${P}/question`, undefined, { key: reader.key });
    const keeps = await call('PUT', `${P}/question`, { question: written }, { key: reader.key });
    const computes = await call('POST', `${P}/appraise`, {}, { key: reader.key });
    assert('a reader sees the question and may have it appraised, and may not keep a change', sees.status === 200 && computes.status === 200 && keeps.status === 403, `${sees.status} ${computes.status} ${keeps.status}`);
    const other = await call('PUT', `${P}/question`, { question: written }, { origin: 'http://elsewhere.example' });
    assert('a write from another page is refused', other.status === 403);
    const odd = await call('PUT', `${P}/question`, { question: { schema: 'something/else' } });
    assert('what is not a question is not kept', odd.status >= 400 && /noetarium\.policy\/1/.test(odd.body.message), JSON.stringify(odd.body));

    // The invented question: two paths, and a study worth running first.
    const n2 = (await call('POST', '/api/projects', { title: 'Free school meals' })).body.name;
    await call('POST', `/api/projects/${n2}/example`, { template: 'schooling/free-school-meals' });
    const m = (await call('POST', `/api/projects/${n2}/appraise`, {})).body.appraisal;
    assert('school meals: of two paths the one through attendance is the strongest, shown at moderate at its weakest claim; the other is held to low',
      m.assessment.paths === 2 && m.assessment.strongest.links.join() === '0,1' && m.assessment.strongest.weakest === 'moderate' && m.assessment.verdict.status === 'holds' && m.theory.links[3].level === 'low', JSON.stringify(m.assessment.strongest));
    const implied = m.bodies[0].pooled.estimate * m.bodies[1].pooled.estimate + m.bodies[2].pooled.estimate * m.bodies[3].pooled.estimate * m.bodies[4].pooled.estimate;
    assert('the two paths imply an effect on learning that is the sum of the products of the pooled effects along each (Wright\'s rule), near what the trials of the whole found',
      Math.abs(m.assessment.implied.effect - implied) < 1e-12 && Math.abs(implied - m.here.there.effect) < 0.01 && m.here.body === 'meals-trials', `${implied} ${m.here.there.effect}`);
    const d = m.decision, top = Math.max(...d.curve.map((p) => p.worth));
    assert('the decision is to study first: adopting now is worth less than nothing, and a study of some eighteen thousand pupils is worth more than rejecting; no size on the curve is worth more than the one chosen',
      d.chosen.join() === 'study first' && d.adopt < 0 && d.study > 0 && d.n > 15000 && d.n < 22000 && d.evsi < d.evpi && top <= d.study + 1e-6, JSON.stringify([d.adopt, d.study, d.n, d.evsi, d.evpi]));
    // Graded by the plainer scheme, the same studies stand on other levels.
    const q2 = (await call('GET', `/api/projects/${n2}/question`)).body.question;
    const plain = listed.schemes.find((s) => s.id === 'schooling/three-levels');
    const regraded = { ...q2, scheme: plain.text, rules: plain.rules, least: 'fair', theory: q2.theory.replace(/^levels .*$/m, 'levels weak fair strong'), bodies: q2.bodies.map((x) => ({ ...x, assessed: {} })) };
    const m2 = (await call('POST', `/api/projects/${n2}/appraise`, { question: regraded })).body.appraisal;
    assert('graded by the plainer scheme the same studies stand on its levels: the trials strong, the observational bodies weak, and the question is no longer shown at fair',
      m2.scheme.levels.join() === 'weak,fair,strong' && m2.bodies[0].grade === 'strong' && m2.bodies[1].grade === 'weak' && m2.assessment.verdict.status === 'undecided', JSON.stringify([m2.bodies.map((x) => x.grade), m2.assessment.verdict.status]));
  }
} finally {
  child.kill();
  try { rmSync(store, { recursive: true, force: true }); } catch { /* a file still held */ }
}
done();
