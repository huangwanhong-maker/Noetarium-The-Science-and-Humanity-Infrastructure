// Checks of what is the Category Studio's own: its page (every element the script asks for, the files it loads, the guide),
// its recipe, its examples (each with a title and what it is about), and its server on a directory of its own: the examples
// listed, a script kept, read back and put away, a name that is no name refused, another page refused. Where the engine is
// built from the present sources, every example is carried out through the server and answered as it is known to be: so many
// statements hold, fail, are not decided or cannot be carried out; a derivation, a cone and a string diagram come with their
// views; the grammar is given; a script too long is refused; and a script after an error is still carried out.
// Run from the repository root:  node applications/category-studio/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

import { suite, ROOT } from '../../../development/tools/checks.mjs';

const { assert: say, skip, done } = suite();
const assert = (name, ok, detail = '') => say(name, ok, ok ? '' : detail);
const APP = resolve(ROOT, 'applications/category-studio');
const read = (path) => readFileSync(join(APP, path), 'utf8');

// ---- the page ---------------------------------------------------------------------------------------------------
{
  const page = read('index.html'), app = read('src/app.js'), guide = read('src/guide.js');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('category studio: the page\'s style sheets and script exist, the drawings\' style sheet among them', !missing.length && local.includes('/visualization/js/categories/categories.css'), missing.join(', '));
  const ids = [...new Set([...app.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]))];
  assert('category studio: every element the script asks for is on the page', ids.length > 15 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  const imports = [...app.matchAll(/from '(\/[^']+)'/g)].map((m) => m[1]);
  assert('category studio: what the script imports of the visualization layer exists', imports.length >= 1 && imports.every((p) => existsSync(join(ROOT, p))) && imports.some((p) => p.includes('/categories/')), imports.join(', '));
  assert('category studio: the page is set in serif type with a face of fixed width for scripts, its server names the port, and the page computes no category: it only draws',
    /Source\+Serif\+4/.test(page) && /IBM\+Plex\+Mono/.test(page) && /default=9070/.test(read('serve.py')) && !/compose\(|isLimit|adjoint\(/.test(app));
  const words = ['category', 'functor', 'transformation', 'sets', 'theory', 'model', 'diagram', 'check', 'find'];
  const grammar = readFileSync(join(ROOT, 'foundations/kategoria/script/include/kategoria/script/language.hpp'), 'utf8');
  const shown = (w) => guide.split('\n').some((line) => line.startsWith(`${w} `) || line.includes(`(\`${w} `));  // at the head of a line of an example
  assert('category studio: the guide shows each of the nine kinds of statement the grammar has', words.every((w) => shown(w) && grammar.includes(`"${w}"`)), words.filter((w) => !shown(w)).join(', '));
  const recipe = JSON.parse(read('recipe.json'));
  assert('category studio: its recipe has one step, the engine, done when its build is recorded', recipe.steps.length === 1 && recipe.steps[0].run[0] === 'applications/category-studio/engine.py' && /category-studio\/build\.json$/.test(recipe.steps[0].done));
}

// ---- the examples -----------------------------------------------------------------------------------------------
const EXPECTED = {
  '01-a-commutative-square': { holds: 9, fails: 2 },
  '02-groups-as-categories': { holds: 13, fails: 3 },
  '03-functors-and-transformations': { holds: 16 },
  '04-limits-in-a-lattice': { holds: 13, fails: 3 },
  '05-adjunctions': { holds: 14, fails: 2 },
  '06-data-on-a-schema': { holds: 11, fails: 1 },
  '07-yoneda-and-truth': { holds: 12 },
  '08-the-simplex-category': { holds: 5 },
  '09-monoids-and-string-diagrams': { holds: 10, fails: 3 },
  '10-bialgebras-and-frobenius': { holds: 2, fails: 2 },
  '11-what-is-refused': { holds: 7, fails: 5, undecided: 3, error: 2 },
};
{
  const files = readdirSync(join(APP, 'examples')).filter((f) => f.endsWith('.cat')).sort();
  assert('category studio: its examples are those the checks know, each beginning with a title and what it is about',
    files.join() === Object.keys(EXPECTED).map((k) => `${k}.cat`).join() && files.every((f) => /^# \S.*\n# \S/.test(read(`examples/${f}`))), files.join(', '));
}

// ---- the server -------------------------------------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
if (!python) skip('category studio: the server (no Python found to run it)');
else {
  const store = mkdtempSync(join(tmpdir(), 'category-studio-check-'));
  const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
  const server = spawn(python, [join(APP, 'serve.py'), '--port', String(port), '--scripts', store], { stdio: 'ignore' });
  const at = `http://127.0.0.1:${port}`;
  const json = { 'Content-Type': 'application/json' };
  const ask = async (path, options = {}) => { const r = await fetch(`${at}${path}`, options); return { code: r.status, body: await r.json().catch(() => null) }; };
  const runScript = (script) => ask('/api/run', { method: 'POST', headers: json, body: JSON.stringify({ script }) });
  try {
    let up = false, status = null;
    for (let k = 0; k < 100 && !up; k++) { await new Promise((r) => setTimeout(r, 150)); try { const r = await fetch(`${at}/api/status`); up = r.ok; status = await r.json(); } catch { /* not yet */ } }
    assert('category studio: the server comes up and says whether its engine is built, its limits, and that it keeps scripts here', up && typeof status.built === 'boolean' && status.limits.arrows === 3000 && status.keeps === true);
    const examples = (await ask('/api/examples')).body;
    assert('category studio: the examples are listed with their titles, what they are about and their text', examples.length === 11 && examples[0].title === 'A commutative square' && /generators and one equation/.test(examples[0].about) && /category Sq/.test(examples[0].text));
    const page = await fetch(`${at}/`), css = await fetch(`${at}/visualization/js/categories/categories.css`), outside = await fetch(`${at}/..%2f..%2fCLAUDE.md`);
    assert('category studio: the page and the visualization layer are served, and nothing outside them', page.ok && css.ok && !outside.ok);

    // Scripts kept.
    const text = 'category C = chain 3\ncheck C is thin\n';
    const kept = await ask('/api/scripts/a%20first%20script', { method: 'PUT', headers: json, body: JSON.stringify({ text }) });
    const list = (await ask('/api/scripts')).body, back = (await ask('/api/scripts/a%20first%20script')).body;
    assert('category studio: a script is kept under its name, listed and read back as it was', kept.code === 200 && list.length === 1 && list[0].name === 'a first script' && back.text === text && existsSync(join(store, 'a first script.cat')));
    const bad = await ask('/api/scripts/..%2Fescape', { method: 'PUT', headers: json, body: JSON.stringify({ text }) });
    const other = await ask('/api/scripts/another', { method: 'PUT', headers: { ...json, Origin: 'http://elsewhere.example' }, body: JSON.stringify({ text }) });
    assert('category studio: a name that is no name is refused, and so is another page', bad.code === 400 && other.code === 403 && (await ask('/api/scripts')).body.length === 1);
    const gone = await ask('/api/scripts/a%20first%20script', { method: 'DELETE' });
    assert('category studio: a script put away is no longer listed, and is not destroyed', gone.code === 200 && (await ask('/api/scripts')).body.length === 0 && readdirSync(join(store, 'put-away')).length === 1);

    if (!status.current) skip('category studio: the examples carried out through the server (the engine is not built from the present sources: python applications/category-studio/engine.py build)');
    else {
      const wrong = [];
      let views = 0;
      const answers = {};
      for (const e of examples) {
        const r = await runScript(e.text);
        const tally = {};
        (r.body?.outcomes || []).forEach((o) => { tally[o.status] = (tally[o.status] || 0) + 1; views += o.view ? 1 : 0; });
        answers[e.id] = r.body?.outcomes || [];
        if (r.code !== 200 || JSON.stringify(Object.entries(tally).sort()) !== JSON.stringify(Object.entries(EXPECTED[e.id]).sort())) wrong.push(`${e.id}: ${JSON.stringify(tally)}`);
      }
      assert('category studio: every example is answered as it is known to be: so many statements hold, fail, are not decided or cannot be carried out', wrong.length === 0, wrong.join('; '));
      assert('category studio: most answers come with something to draw', views > 100, String(views));
      const square = answers['01-a-commutative-square'], groups = answers['02-groups-as-categories'], strings = answers['09-monoids-and-string-diagrams'];
      assert('category studio: a square\'s equation comes with its derivation, its pullback with a cone on the category',
        square[1].view.type === 'derivation' && square[1].view.steps.length === 2 && square[2].view.type === 'cone' && square[2].view.category.objects[square[2].view.apex] === 'A' && square[2].line === 14);
      assert('category studio: (st)³ = 1 is proved from the three equations, step by step, and s ∘ t ≠ t ∘ s is answered with two normal forms',
        groups[1].status === 'holds' && groups[1].view.steps.length >= 4 && groups[1].view.steps.every((s, k, all) => k === all.length - 1 || /=/.test(s.by)) && groups[2].status === 'fails' && groups[2].evidence.length === 2);
      assert('category studio: a law of a theory comes with both sides as diagrams of wires and boxes', strings[0].view.type === 'theory' && strings[0].view.laws[0].left.boxes.length === 2 && strings[0].view.laws[0].right.wires.length === 5);
      const refused = answers['11-what-is-refused'];
      assert('category studio: what is refused says where: the line and column of a statement that ends too soon, the object with no universal property, what is not decided',
        refused.find((o) => o.status === 'error' && /line 16, column/.test(o.reason)) && refused.filter((o) => o.status === 'undecided').length === 3 && refused.at(-1).status === 'undecided');
      const grammar = await ask('/api/grammar');
      assert('category studio: the grammar is given as the engine reads it', grammar.code === 200 && /statement ::=/.test(grammar.body.text) && /cequal/.test(grammar.body.text));
      const long = await runScript(`# ${'x'.repeat(210000)}\n`);
      const none = await ask('/api/run', { method: 'POST', headers: json, body: JSON.stringify({ script: 7 }) });
      assert('category studio: a script too long, or no script at all, is refused', long.code === 413 && none.code === 400);
      const after = await runScript('category = chain 3\ncategory C = chain 3\nfind product 0 2 in C\n');
      assert('category studio: after a statement that is not understood the rest is still carried out', after.body.outcomes.map((o) => o.status).join() === 'error,holds,holds' && /0 with/.test(after.body.outcomes[2].claim));
      const large = await runScript('category S = symmetric 5\ncategory P = product S S\n');
      assert('category studio: a statement that would build more arrows than the server allows is refused with the count', large.body.outcomes[1].status === 'error' && /14400 arrows/.test(large.body.outcomes[1].reason));
    }
  } finally {
    server.kill();
    rmSync(store, { recursive: true, force: true });
  }
}

done();
