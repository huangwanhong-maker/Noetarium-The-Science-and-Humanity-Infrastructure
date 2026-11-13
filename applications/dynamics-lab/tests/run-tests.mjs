// Checks of what is the Dynamics Lab's own: its page (every element the script asks for, the files it loads), its tools
// (each an operation the server allows and the engine answers), its examples (each with a title, what it is about, and
// studies that name a tool), its recipe, and its server on a directory of its own: the examples listed, a project kept,
// read back and put away, a name that is no name refused, another page refused, an operation that is none refused.
// Where the engine is built from the present sources, every example is read and every study of it is carried out
// through the server, and answered as it is known to be: Lorenz's equilibria and the sum of its exponents, the logistic
// map's ln 2, a ball's first bounce, the pendulum's equation derived from its action, the wells of a landscape, a
// grammar's words and the histories of one, a driven atom's stationary state, braids with their entropies and what a
// record certifies of them, systems joined, and a law recovered from a record; a cascade of turbulence with its exact
// balances, parts joined by an action, a fluid with its chemistry and a species, and behaviours of different kinds
// together, each held to what is known of it in closed form; and what is refused says why.
// Run from the repository root:  node applications/dynamics-lab/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, check, skip, done } = suite();
const assert = (name, ok, detail = '') => say(name, Boolean(ok), ok ? '' : detail);
const APP = resolve(ROOT, 'applications/dynamics-lab');
const read = (path) => readFileSync(join(APP, path), 'utf8');
const parseWord = (text) => [...String(text).matchAll(/s\s*(\d+)\s*(\^\s*-\s*1)?/g)].map((m) => (m[2] ? -1 : 1) * Number(m[1]));

// ---- the page and its tools ---------------------------------------------------------------------------------------
const page = read('index.html'), app = read('src/app.js'), tools = read('src/tools.js'), serve = read('serve.py');
const toolIds = [...tools.matchAll(/^\s{4}id: '(\w+)'/gm)].map((m) => m[1]);
const allowed = [...serve.match(/OPS = \{([^}]*)\}/s)[1].matchAll(/"(\w+)"/g)].map((m) => m[1]);
{
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]);
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('dynamics lab: the page\'s style sheets and script exist, the drawings\' style sheet among them', !missing.length && local.includes('/visualization/js/dynamics/dynamics.css'), missing.join(', '));
  const ids = [...new Set([...app.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))].filter((i) => i !== 'go' && !i.startsWith('f-'));  // the form's own are made with it
  assert('dynamics lab: every element the script asks for is on the page', ids.length > 20 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  const imports = [...`${app}\n${tools}`.matchAll(/from '(\/[^']+)'/g)].map((m) => m[1]);
  assert('dynamics lab: what the scripts import of the visualization layer exists', imports.length >= 1 && imports.every((p) => existsSync(join(ROOT, p))) && imports.some((p) => p.includes('/dynamics/')), imports.join(', '));
  assert('dynamics lab: the page is set in serif type with a face of fixed width for scripts, three.js is pinned, and its server names the port',
    /Source\+Serif\+4/.test(page) && /IBM\+Plex\+Mono/.test(page) && /three@0\.170\.0/.test(page) && /default=9110/.test(serve));
  const api = readFileSync(join(ROOT, 'api/cpp/science/dynamics.hpp'), 'utf8');
  assert('dynamics lab: each of its nineteen tools is an operation the server allows, and each operation allowed is one the engine answers',
    toolIds.length === 19 && new Set(toolIds).size === 19 && toolIds.every((t) => allowed.includes(t === 'run' ? 'simulate' : t)) && allowed.every((op) => api.includes(`op == "${op}"`)),
    `${toolIds.length} tools; ${toolIds.filter((t) => !allowed.includes(t === 'run' ? 'simulate' : t)).join(', ')}; ${allowed.filter((op) => !api.includes(`op == "${op}"`)).join(', ')}`);
  const recipe = JSON.parse(read('recipe.json'));
  assert('dynamics lab: its recipe has one step, the engine, done when its build is recorded', recipe.steps.length === 1 && recipe.steps[0].run[0] === 'applications/dynamics-lab/engine.py' && /dynamics-lab\/build\.json$/.test(recipe.steps[0].done));
}

// ---- the examples -------------------------------------------------------------------------------------------------
// For each: the forms of the systems it declares, in order, and the number of its studies.
const EXPECTED = {
  '01-lorenz': { forms: 'flow', studies: 4 },
  '02-maps': { forms: 'map map', studies: 4 },
  '03-events': { forms: 'flow flow', studies: 2 },
  '04-actions': { forms: 'lagrangian lagrangian', studies: 5 },
  '05-noise-and-landscapes': { forms: 'stochastic flow flow stochastic', studies: 5 },
  '06-grammars': { forms: 'rules rules rules', studies: 3 },
  '07-fields-without-a-background': { forms: 'field derived field', studies: 0 },
  '08-a-diffusion-model': { forms: 'stochastic derived stochastic', studies: 3 },
  '09-open-quantum-systems': { forms: 'quantum quantum', studies: 4 },
  '10-braids': { forms: 'flow flow flow', studies: 5 },
  '11-systems-of-systems': { forms: 'flow flow composite map flow composite stochastic flow composite', studies: 5 },
  '12-from-a-record-to-a-model': { forms: 'flow flow flow flow', studies: 6 },
  '13-turbulence': { forms: 'flow', studies: 4 },
  '14-joined-by-an-action': { forms: 'lagrangian composite flow composite', studies: 4 },
  '15-a-sea-with-life-in-it': { forms: 'flow flow flow composite composite map composite', studies: 8 },
  '16-behaviours-together': { forms: 'flow composite flow composite', studies: 7 },
};
const studiesOf = (text) => [...text.matchAll(/^#!\s*(\{.*\})\s*$/gm)].map((m) => JSON.parse(m[1]));
{
  const files = readdirSync(join(APP, 'examples')).filter((f) => f.endsWith('.dyn')).sort();
  assert('dynamics lab: its examples are those the checks know, each beginning with a title and what it is about',
    files.join() === Object.keys(EXPECTED).map((k) => `${k}.dyn`).join() && files.every((f) => /^# \S.*\n# \S/.test(read(`examples/${f}`))), files.join(', '));
  const wrong = files.filter((f) => { const s = studiesOf(read(`examples/${f}`)); return s.length !== EXPECTED[f.slice(0, -4)]?.studies || s.some((x) => !toolIds.includes(x.tool)); });
  assert('dynamics lab: each example opens with the studies the checks know, each naming one of the tools', wrong.length === 0, wrong.join(', '));
}

// ---- the server ---------------------------------------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
if (!python) skip('dynamics lab: the server (no Python found to run it)');
else {
  const store = mkdtempSync(join(tmpdir(), 'dynamics-lab-check-'));
  const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
  const server = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--projects', store], { stdio: 'ignore' });
  const at = `http://127.0.0.1:${port}`;
  const json = { 'Content-Type': 'application/json' };
  const ask = async (path, options = {}) => { const r = await fetch(`${at}${path}`, options); return { code: r.status, body: await r.json().catch(() => null) }; };
  const engine = (request) => ask('/api/ask', { method: 'POST', headers: json, body: JSON.stringify(request) });
  try {
    let up = false, status = null;
    for (let k = 0; k < 100 && !up; k++) { await new Promise((r) => setTimeout(r, 150)); try { const r = await fetch(`${at}/api/status`); up = r.ok; status = await r.json(); } catch { /* not yet */ } }
    assert('dynamics lab: the server comes up and says whether its engine is built, its limits, and that it keeps projects here', up && typeof status.built === 'boolean' && status.limits.bytes === 8000000 && status.limits.seconds === 120 && status.keeps === true);
    const examples = (await ask('/api/examples')).body;
    assert('dynamics lab: the examples are listed with their titles, what they are about and their text', examples.length === 16 && examples[0].title.startsWith('Lorenz') && /Three equations/.test(examples[0].about) && /system lorenz/.test(examples[0].text));
    const home = await fetch(`${at}/`), css = await fetch(`${at}/visualization/js/dynamics/dynamics.css`), outside = await fetch(`${at}/..%2f..%2fCLAUDE.md`);
    assert('dynamics lab: the page and the visualization layer are served, and nothing outside them', home.ok && css.ok && !outside.ok);

    // Projects kept.
    const document = { format: 'noetarium.dynamics/1', script: 'system s {\n  state x : real\n  change { x\' = -x }\n}\n', records: [], studies: [{ tool: 'run', request: { system: 's', initial: { x: 1 } } }] };
    const kept = await ask('/api/projects/a%20first%20project', { method: 'PUT', headers: json, body: JSON.stringify(document) });
    const list = (await ask('/api/projects')).body, back = (await ask('/api/projects/a%20first%20project')).body;
    assert('dynamics lab: a project is kept under its name, listed and read back as it was', kept.code === 200 && list.length === 1 && list[0].name === 'a first project' && JSON.stringify(back) === JSON.stringify(document) && existsSync(join(store, 'a first project.json')));
    const bad = await ask('/api/projects/..%2Fescape', { method: 'PUT', headers: json, body: JSON.stringify(document) });
    const other = await ask('/api/projects/another', { method: 'PUT', headers: { ...json, Origin: 'http://elsewhere.example' }, body: JSON.stringify(document) });
    const formless = await ask('/api/projects/formless', { method: 'PUT', headers: json, body: JSON.stringify({ script: '' }) });
    assert('dynamics lab: a name that is no name is refused, and so are another page and a document of no format', bad.code === 400 && other.code === 403 && formless.code === 400 && (await ask('/api/projects')).body.length === 1);
    const gone = await ask('/api/projects/a%20first%20project', { method: 'DELETE' });
    assert('dynamics lab: a project put away is no longer listed, and is not destroyed', gone.code === 200 && (await ask('/api/projects')).body.length === 0 && readdirSync(join(store, 'put-away')).length === 1);
    const noop = await engine({ op: 'shell', script: '' }), nothing = await ask('/api/ask', { method: 'POST', headers: json, body: '[1, 2]' });
    assert('dynamics lab: an operation that is none of the engine\'s is refused, and so is a request that is no object', noop.code === 400 && /no operation named/.test(noop.body.message) && nothing.code === 400);

    if (!status.current) skip('dynamics lab: the examples carried out through the server (the engine is not built from the present sources: python applications/dynamics-lab/engine.py build)');
    else {
      // Every example read, and every study of it carried out.
      const answers = {}, forms = {}, refused = [];
      for (const e of examples) {
        const script = e.text.replace(/^#!.*\n/gm, '');
        const r = await engine({ op: 'read', script });
        forms[e.id] = r.body?.error ? `error: ${r.body.error.message}` : (r.body?.systems ?? []).map((s) => (s.error ? `${s.form}!` : s.form)).join(' ');
        answers[e.id] = [];
        for (const { tool, ...request } of studiesOf(e.text)) {
          if (typeof request.word === 'string') request.word = parseWord(request.word);
          const a = await engine({ ...request, op: tool === 'run' ? 'simulate' : tool, script });
          if (a.code !== 200) refused.push(`${e.id} ${tool}: ${a.body?.message}`);
          answers[e.id].push(a.body ?? {});
        }
      }
      const unlike = Object.keys(EXPECTED).filter((id) => forms[id] !== EXPECTED[id].forms);
      assert('dynamics lab: every example is read, its systems of the forms they are known to be, none in error', unlike.length === 0, unlike.map((id) => `${id}: ${forms[id]}`).join('; '));
      assert('dynamics lab: every study of every example is answered', refused.length === 0, refused.join('; '));

      // A flow that is chaotic.
      const [run, exponents, equilibria, diagram] = answers['01-lorenz'];
      assert('dynamics lab: Lorenz is run by Runge-Kutta from the state given, a sample at each step', run.t.length === 8001 && run.x[2][0] === 20 && /Runge-Kutta/.test(run.method));
      check('dynamics lab: Lorenz\'s exponents sum to -(sigma + 1 + beta), the divergence of the field', exponents.sum + 41 / 3, 1e-4);
      check('dynamics lab: the largest against 0.9056, in the errors of its finite time', (exponents.exponents[0] - 0.9056) / exponents.error[0], 4);
      const pair = equilibria.equilibria.filter((q) => Math.abs(q.x[2] - 27) < 1e-6);
      check('dynamics lab: its equilibria are the origin and the pair at (+-sqrt 72, +-sqrt 72, 27)', Math.abs(equilibria.equilibria.length - 3) + pair.reduce((s, q) => s + Math.abs(Math.abs(q.x[0]) - Math.sqrt(72)) + Math.abs(q.x[0] - q.x[1]), 0) + Math.abs(pair.length - 2), 1e-8);
      assert('dynamics lab: all three are saddles, and the diagram is of the maxima of z on the attractor', equilibria.equilibria.every((q) => q.kind === 'saddle' && !q.stable) && diagram.observable === 'z' && diagram.x.length === diagram.y.length && diagram.x.length > 1000);

      // Maps.
      const [mapDiagram, ln2, transfer, henon] = answers['02-maps'];
      check('dynamics lab: the logistic map at r = 4 has the exponent ln 2', ln2.exponents[0] - Math.LN2, 1e-3);
      check('dynamics lab: its transfer operator on 200 cells keeps every path and has the leading eigenvalue 1', Math.abs(transfer.eigenvalues[0][0] - 1) + Math.abs(transfer.eigenvalues[0][1]) + transfer.lost, 1e-9);
      assert('dynamics lab: a map is iterated, a sample an iteration; its diagram has 120 iterates at each of 400 values', henon.method === 'iterated' && henon.t.length === 6001 && henon.t[1] === 1 && mapDiagram.x.length === 48000);

      // Events.
      const [neuron, ball] = answers['03-events'];
      check('dynamics lab: a ball dropped from 1 first meets the ground at sqrt(2 / g)', ball.events[0].t - Math.sqrt(2 / 9.81), 1e-6);
      assert('dynamics lab: the neuron spikes and is reset; the ball\'s speed is a component of its own', neuron.events.length >= 3 && ball.events.length >= 10 && ball.names.join() === "h,h'");

      // Systems given by an action.
      const [pendulum, , , , chaos] = answers['04-actions'];
      assert('dynamics lab: the pendulum\'s equation is derived from its action', pendulum.text.includes("q'' = -g * sin(q) / l"), pendulum.text);
      check('dynamics lab: the double pendulum keeps volume: its exponents sum to zero, and come in pairs of opposite sign, within the error of a finite time',
        Math.abs(chaos.sum) + Math.abs(chaos.exponents[0] + chaos.exponents[3]) + Math.abs(chaos.exponents[1] + chaos.exponents[2]), 0.06);

      // Noise, landscapes and slowing down.
      const [, twoWells, well, basins, warnings] = answers['05-noise-and-landscapes'];
      check('dynamics lab: the stationary landscape of the two wells has its minima at -1 and 1 and its barrier at 0', Math.abs(twoWells.exact.minima[0] + 1) + Math.abs(twoWells.exact.minima[1] - 1) + Math.abs(twoWells.exact.maxima[0]), 1e-3);
      check('dynamics lab: and so has the potential of the well without noise', Math.abs(well.exact.minima[0] + 1) + Math.abs(well.exact.minima[1] - 1) + Math.abs(well.exact.maxima[0]), 1e-3);
      const nodes = basins.kind.map((k, i) => (k === 'stable node' ? i : -1)).filter((i) => i >= 0);
      assert('dynamics lab: the basins\' two greatest ends are the stable nodes at (-1, -1) and (1, 1), nearly half the grid each; the ends that are not at rest are said to be moving',
        nodes.join() === '0,1' && nodes.every((i) => Math.abs(Math.abs(basins.attractors[i][0]) - 1) < 1e-6 && Math.abs(basins.share[i] - 0.5) < 0.02) && basins.kind.slice(2).every((k) => k === 'moving') && basins.speed.slice(2).every((v) => v > 1e-6),
        JSON.stringify({ kind: basins.kind, share: basins.share }));
      assert('dynamics lab: toward the fold the variance and the autocorrelation rise, each beyond what red noise gives', warnings.tau_variance > 0.5 && warnings.tau_autocorrelation > 0.5 && warnings.p_variance < 0.05 && warnings.p_autocorrelation < 0.05);

      // Grammars.
      const [anbn, sums, anbncn] = answers['06-grammars'];
      assert('dynamics lab: the words of a^n b^n up to ten letters are its five, each with one history', anbn.words.map((w) => w.word).join('|') === 'a b|a a b b|a a a b b b|a a a a b b b b|a a a a a b b b b b' && anbn.words.every((w) => w.histories === 1) && anbn.context_free);
      assert('dynamics lab: a sum of three has two histories, which the word does not tell apart; a^n b^n c^n is not context-free',
        sums.words.find((w) => w.word === 'x plus x plus x')?.histories === 2 && !anbncn.context_free && anbncn.words.map((w) => w.length).join() === '3,6,9', JSON.stringify(sums.words.map((w) => [w.word, w.histories])));

      // A diffusion model, and what is not built.
      const [forward, , heavy] = answers['08-a-diffusion-model'];
      assert('dynamics lab: a system with noise is stepped by Euler-Maruyama, and a map with draws is iterated with them', forward.method === 'Euler-Maruyama' && heavy.method === 'a map with draws' && heavy.t.length === 2001);

      // Open quantum systems.
      const [atom, stationary, , cavity] = answers['09-open-quantum-systems'];
      check('dynamics lab: a driven atom that decays settles at the excited population Omega^2 / (gamma^2 + 2 Omega^2) = 9/19', stationary.stationary[0] - 9 / 19, 1e-10);
      check('dynamics lab: its run from the excited level ends there, having lost its purity', Math.abs(atom.x[0].at(-1) - 9 / 19) + Math.abs(atom.purity.at(-1) - stationary.purity), 1e-3);
      assert('dynamics lab: each has one stationary state; the cavity\'s is the vacuum, which is pure', stationary.stationary_dimension === 1 && cavity.stationary_dimension === 1 && Math.abs(cavity.purity - 1) < 1e-9 && cavity.dimension === 16);

      // Braids.
      const [vortices, stirrers, fine, coarse, word] = answers['10-braids'];
      const golden = Math.log((3 + Math.sqrt(5)) / 2);
      check('dynamics lab: three stirrers on a figure of eight braid with the entropy 3 log((3 + sqrt 5) / 2) a turn, which Burau\'s bound meets', Math.abs(stirrers.braid.entropy.estimate - 3 * golden) + Math.abs(stirrers.braid.entropy.lower - 3 * golden), 1e-9);
      assert('dynamics lab: their braid is pure, (s2^-1 s1)^3, and certified under the speed assumed; the vortices\' braid has no entropy',
        stirrers.braid.text === 's2^-1 s1 s2^-1 s1 s2^-1 s1' && stirrers.braid.pure && stirrers.certificate.certified && stirrers.certificate.margin > 0 && vortices.certificate.certified && vortices.braid.entropy.estimate === 0, stirrers.braid.text);
      assert('dynamics lab: one turn sampled sixteen times is certified to wind once; sampled four times it is not, and 57 windings are admitted',
        fine.certificate.certified && fine.winding.values.join() === '1' && fine.winding.certified && !coarse.certificate.certified && coarse.certificate.margin < 0 && coarse.winding.values.length === 57 && !coarse.winding.certified);
      check('dynamics lab: the word (s1 s2^-1)^2 has the entropy 2 log((3 + sqrt 5) / 2)', word.entropy.estimate - 2 * golden, 1e-9);
      assert('dynamics lab: and the Alexander polynomial 1 - 3t + t^2 of its closure, the figure-eight knot', word.alexander === '1 - 3t + t^2' && word.permutation.join() === '1,2,0');

      // Systems of systems.
      const [merged, synchronised, together, sampled, filtered] = answers['11-systems-of-systems'];
      assert('dynamics lab: two flows joined are merged into one declaration, which is given; parts of different forms are co-simulated',
        /system synchronised/.test(merged.text) && /merged into one flow/.test(synchronised.method) && /co-simulated/.test(sampled.method) && /co-simulated/.test(filtered.method) && sampled.names.join() === 'clock.n,tank.x');
      check('dynamics lab: the follower comes to the drive: their y differ at the end by less than a thousandth', synchronised.x[synchronised.names.indexOf('drive_y')].at(-1) - synchronised.x[synchronised.names.indexOf('follower_y')].at(-1), 1e-3);
      check('dynamics lab: the merged system\'s exponents sum to the divergences of its two parts, -41/3 - 11/3', together.sum + 52 / 3, 1e-4);

      // From a record to a model.
      const [discovered, fitted, matched, catalogued, forecast, operator] = answers['12-from-a-record-to-a-model'];
      assert('dynamics lab: Lorenz\'s seven terms are recovered from a record of it', discovered.terms === 7 && /x' = /.test(discovered.text) && discovered.residual < 1e-2, `${discovered.terms} terms`);
      check('dynamics lab: a template\'s three numbers are fitted to 10, 28 and 8/3', Math.abs(fitted.numbers.s - 10) + Math.abs(fitted.numbers.r - 28) + Math.abs(fitted.numbers.b - 8 / 3), 5e-3);
      assert('dynamics lab: of three candidates the template that is Lorenz\'s ranks first, by its error on what was held out', matched.ranking.map((c) => c.name).join() === 'template,linear,rossler' && matched.ranking[0].one_step < matched.ranking[1].one_step / 100);
      assert('dynamics lab: against the catalogue with nothing proposed, the model of Lorenz is first of its six of three components, each tried in six orders, and its numbers are found',
        catalogued.tried === 36 && catalogued.proposed === 0 && catalogued.ranking.length === 6 && catalogued.ranking[0].name === 'lorenz' && Math.abs(catalogued.ranking[0].numbers.rho - 28) < 1e-2 && Math.abs(catalogued.ranking[0].numbers.sigma - 10) < 1e-2,
        JSON.stringify(catalogued.ranking?.map((c) => c.name)));
      assert('dynamics lab: a forecaster with no law follows the held-out record for a while, and a linear operator is fitted with its residual', forecast.horizon >= 20 && forecast.truth.length === 3 && operator.eigenvalues.length === 3 && operator.residual > 0);

      // Turbulence: a cascade, with what is exact of it.
      const [cascade, spectrum, thicker, fastest] = answers['13-turbulence'];
      const of = (a, name) => a.mean[a.names.indexOf(name)], flat = (a, name) => a.second[a.names.indexOf(name)];
      assert('dynamics lab: the shell model runs on its fourteen shells, twenty-eight components', cascade.names.length === 28 && cascade.t.length === 2001);
      check('dynamics lab: in the steady state of the cascade the power put in is the power dissipated, to three hundredths', (of(spectrum, 'input') - of(spectrum, 'dissipation')) / of(spectrum, 'input'), 0.03);
      check('dynamics lab: and the flux of energy is the same through two shells of the cascade, to two hundredths', (of(spectrum, 'flux3') - of(spectrum, 'flux6')) / of(spectrum, 'input'), 0.02);
      assert('dynamics lab: the energy of the shells falls as a power of the wavenumber over eight octaves, steeper than Kolmogorov\'s 2/3, and the flatness grows down the cascade',
        spectrum.scaling.exponent < -2 / 3 && spectrum.scaling.exponent > -0.9 && spectrum.scaling.error < 0.05 && flat(spectrum, 'e9') > 1.5 * flat(spectrum, 'e4'), JSON.stringify([spectrum.scaling.exponent, flat(spectrum, 'e4'), flat(spectrum, 'e9')]));
      assert('dynamics lab: with the viscosity more than three times as great the cascade ends sooner and the power dissipated changes by less than a third; the three largest exponents are positive',
        of(thicker, 'e12') < of(spectrum, 'e12') / 3 && Math.abs(of(thicker, 'dissipation') / of(spectrum, 'dissipation') - 1) < 0.33 && fastest.exponents.length === 3 && fastest.exponents.every((v) => v > 0.5));

      // Joined by an action.
      const [whole, beats, shares, wired] = answers['14-joined-by-an-action'];
      assert('dynamics lab: two pendulums joined by an action are one system, each pulled by the other, with its energy declared kept',
        whole.text.includes("a_q'' = -k * (a_q - b_q) - 9.81 * sin(a_q)") && whole.text.includes("b_q'' = k * (a_q - b_q) - 9.81 * sin(b_q)") && /conserves/.test(whole.text) && /joined by an action/.test(beats.method), whole.text);
      const column = (a, name) => a.x[a.names.indexOf(name)];
      const start = 9.81 * (1 - Math.cos(0.4)) + 0.6 / 2 * 0.16;
      check('dynamics lab: the energies of the two and of the spring between them sum, on average, to the energy the whole began with', of(shares, 'energy_a') + of(shares, 'energy_b') + of(shares, 'in_the_spring') - start, 1e-7);
      assert('dynamics lab: the energy passes from the one that was moved to the other and back: each holds near half of it on average', Math.abs(of(shares, 'energy_a') - of(shares, 'energy_b')) < 0.1 && Math.max(...column(beats, 'b_q').map(Math.abs)) > 0.3);
      const alone = column(wired, 'a_q').map((q, k) => column(wired, "a_q'")[k] ** 2 / 2 - 9.81 * Math.cos(q));
      const pulled = column(wired, 'b_q').map((q, k) => column(wired, "b_q'")[k] ** 2 / 2 - 9.81 * Math.cos(q));
      check('dynamics lab: joined by a wire one way, the first keeps its own energy to itself', Math.max(...alone) - Math.min(...alone), 1e-7);
      assert('dynamics lab: and the second is driven: its energy is not kept', Math.max(...pulled) - Math.min(...pulled) > 0.1);

      // A sea with life in it.
      const [sea, , warming, living, barren, generations, calm, stirred] = answers['15-a-sea-with-life-in-it'];
      assert('dynamics lab: a fluid, its chemistry and a species are merged into one flow of six components', /fluid_x' = /.test(sea.text) && /water_P' = /.test(sea.text) && /life_F' = /.test(sea.text));
      check('dynamics lab: with fish the plankton stands at death / gain, the level at which the fish neither grow nor decline', of(living, 'plankton') - 0.1 / 0.24, 1e-3);
      check('dynamics lab: the heating of the fluid is its own and what the plankton adds', of(living, 'heating') - (22 + 8 * of(living, 'plankton')), 1e-9);
      assert('dynamics lab: without fish there is more than three times the plankton, more heating and more overturning', of(barren, 'plankton') > 3 * of(living, 'plankton') && of(barren, 'heating') > of(living, 'heating') + 5 && of(barren, 'overturning') > 1.2 * of(living, 'overturning'),
        JSON.stringify([of(barren, 'plankton'), of(barren, 'heating'), of(barren, 'overturning')]));
      const spread = (from, to) => { const ys = warming.y.filter((_, k) => warming.x[k] >= from && warming.x[k] <= to); return Math.max(...ys) - Math.min(...ys.filter((v) => v > 0)); };
      assert('dynamics lab: as the plankton warms the water more, the fluid passes from steady overturning to chaos: below a warming of 5 one level, above 8 a band', spread(0, 5) < 3 && spread(8, 12) > 8, `${spread(0, 5)}, ${spread(8, 12)}`);
      assert('dynamics lab: with the species living by generations, a map among flows, the three are advanced side by side', /co-simulated/.test(generations.method) && generations.names.join() === 'fluid.x,fluid.y,fluid.z,water.N,water.P,life.F' && generations.x[5].at(-1) > 0);

      const alive = (a) => a.equilibria.find((q) => q.x[4] > 0.3 && q.x[5] > 0.1);
      assert('dynamics lab: the steady state of the sea with life in it is stable at a warming of 6 and not at 8: it is lost where the heating passes the threshold of the fluid alone, though the sweep shows chaos only later',
        alive(calm)?.stable === true && alive(stirred)?.stable === false && Math.abs(alive(calm).x[4] - 0.1 / 0.24) < 1e-6 && 22 + 6 * alive(calm).x[4] < 24.74 && 22 + 8 * alive(stirred).x[4] > 24.74,
        JSON.stringify([alive(calm)?.kind, alive(stirred)?.kind]));

      // Behaviours together.
      const [, unequal, equal, apart, , inStep, drifting] = answers['16-behaviours-together'];
      check('dynamics lab: two callers that lock call at one pace, the mean of their own weighted by how much the other listens: 1.25', Math.abs(of(unequal, 'pace_of_a') - 1.25) + Math.abs(of(unequal, 'pace_of_b') - 1.25) + Math.abs(of(unequal, 'lag') - 0.25), 1e-8);
      check('dynamics lab: listening equally they call at the plain mean, 1.15', Math.abs(of(equal, 'pace_of_a') - 1.15) + Math.abs(of(equal, 'pace_of_b') - 1.15), 1e-8);
      assert('dynamics lab: listening too little they do not lock: each keeps a pace nearer its own', of(apart, 'pace_of_b') - of(apart, 'pace_of_a') > 0.2);
      check('dynamics lab: with a third that answers and hurries both alike, the pace they keep is 1.25 and the hurry times its answering', Math.abs(of(inStep, 'pace_of_a') - 1.25 - 0.05 * of(inStep, 'answering')) + Math.abs(of(inStep, 'pace_of_b') - of(inStep, 'pace_of_a')), 1e-6);
      assert('dynamics lab: it answers more when the two are in step than when they drift, and when they drift it answers in bursts', of(inStep, 'answering') > of(drifting, 'answering') && flat(drifting, 'answering') > flat(inStep, 'answering') && of(drifting, 'pace_of_b') - of(drifting, 'pace_of_a') > 0.2);

      // What is refused, and where a script goes wrong.
      const lorenz = examples[0].text.replace(/^#!.*\n/gm, '');
      const catalogue = (await ask('/api/catalogue')).body.models;
      assert('dynamics lab: the catalogue is given, each model with its field, its source, its form and a declaration of its own name',
        catalogue.length === 30 && catalogue.every((m) => m.title && m.field && m.source && m.form && m.text.includes(`system ${m.id} {`)) && new Set(catalogue.map((m) => m.form)).size >= 6, String(catalogue.length));
      const grammar = await ask('/api/grammar');
      assert('dynamics lab: the grammar is given as the engine reads it', grammar.code === 200 && /script\s+::=/.test(grammar.body.text) && /system\s+::=/.test(grammar.body.text));
      const mistaken = lorenz.replace("x' = sigma * (y - x)", "x' = sigma (y - x)"), line = mistaken.split('\n').findIndex((l) => l.includes("x' = sigma (y - x)")) + 1;
      const wrong = await engine({ op: 'read', script: mistaken });
      assert('dynamics lab: a script that is wrong is said where: the line and the column of the product that lacks its sign, and what may follow',
        wrong.code === 200 && line > 0 && wrong.body.error.line === line && wrong.body.error.column === mistaken.split('\n')[line - 1].indexOf('(') + 1 && /what may follow/.test(wrong.body.error.message), JSON.stringify(wrong.body.error));
      const half = await engine({ op: 'read', script: `${lorenz}system broken {\n  state x : real\n  change { x' = y }\n}\n` });
      assert('dynamics lab: a system that is wrong is said to be, and the others are still read', half.body.systems.length === 2 && !half.body.systems[0].error && /'y' is not declared/.test(half.body.systems[1].error.message));
      const landscape = await engine({ op: 'landscape', script: lorenz, system: 'lorenz' }), long = await engine({ op: 'simulate', script: lorenz, system: 'lorenz', t1: 1e9 }), nobody = await engine({ op: 'simulate', script: lorenz, system: 'nobody' });
      assert('dynamics lab: what cannot be answered is refused with the reason: a landscape of a flow that is no gradient, a run too long, a system that is not declared',
        landscape.code === 422 && /not a gradient/.test(landscape.body.message) && long.code === 422 && /more than 400000 samples/.test(long.body.message) && nobody.code === 422 && /no system named 'nobody'/.test(nobody.body.message));
      const big = await engine({ op: 'read', script: `# ${'x'.repeat(8100000)}\n` });
      assert('dynamics lab: a request longer than the server takes is refused', big.code === 413);
    }
  } finally {
    server.kill();
    rmSync(store, { recursive: true, force: true });
  }
}

done();
