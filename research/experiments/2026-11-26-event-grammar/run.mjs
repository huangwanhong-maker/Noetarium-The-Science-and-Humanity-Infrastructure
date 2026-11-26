// The grammar of Melbourne's event spacetime (ADR 0168): the district the Urban Science Workbench's Event Spacetime builds (the
// workbench's src/district.js on the data library's city model, as the page sends it), its record of events, and the grammar of the
// record's complex, asked of the plug-in's engine as recorded, by events' types and by their kinds.
//
//   node research/experiments/2026-11-26-event-grammar/run.mjs            (asks the engine, writes results.json, prints a summary)
//   node research/experiments/2026-11-26-event-grammar/run.mjs --summary  (prints the summary of results.json)
//
// Needs the data library's city-model-melbourne.json and city-events-melbourne.json and the built engine
// (python applications/urban-science-workbench/engine.py build event-spacetime). Writes results.json beside this file, trimmed as
// `trim` says.

import { readFileSync, writeFileSync, mkdtempSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath, pathToFileURL } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));
const ROOT = join(HERE, '..', '..', '..');
const APP = join(ROOT, 'applications', 'urban-science-workbench');
const { districtOf, boxOf, networkOf, footprintsIn } = await import(pathToFileURL(join(APP, 'src', 'district.js')).href);
const { localFrameAt } = await import(pathToFileURL(join(ROOT, 'visualization', 'js', 'gis', 'frame.js')).href);

// What results.json keeps of a grammar: each event's rule and surprisal left out; the readings with states share the record's rules
// and its lives, so the rules are kept with the record and the lives with the changes; each life's run kept to its first twelve events.
function trim(g) {
  delete g.record.perEvent, delete g.both.perEvent;
  for (const k of ['record', 'uses', 'regimes', 'both']) delete g[k].lives;
  for (const k of ['uses', 'regimes', 'both']) delete g[k].rules, delete g[k].novel;
  for (const L of Object.values(g.changes.lives))
    for (const x of L.top) if (x.expansion.length > 12) x.expansion = [...x.expansion.slice(0, 12), `… ${x.expansion.length - 12} more`];
}

let out;
if (process.argv.includes('--summary')) {
  out = JSON.parse(readFileSync(join(HERE, 'results.json'), 'utf8'));
} else {
  const area = JSON.parse(readFileSync(join(APP, 'areas.json'), 'utf8')).areas.find((a) => a.id === 'melbourne');
  const model = JSON.parse(readFileSync(join(ROOT, 'data', 'cache', 'products', 'city-model-melbourne.json'), 'utf8'));
  const frame = localFrameAt(model.origin.lat, model.origin.lon);
  const box = boxOf(frame, districtOf(area));
  const request = { area: 'melbourne', ...networkOf(model, box), buildings: footprintsIn(model, box) };
  const dir = mkdtempSync(join(tmpdir(), 'event-grammar-'));
  writeFileSync(join(dir, 'request.json'), JSON.stringify(request));

  const driver = [
    'import json, sys', `sys.path.insert(0, ${JSON.stringify(APP)})`, 'import engine',
    'D = json.load(open(sys.argv[1], encoding="utf-8"))', 'w = engine.Worker("event-spacetime")',
    'def ask(op, **kw):', '    a = w.ask({"op": op, **kw})', '    if not a.get("ok"): raise SystemExit(op + ": " + str(a.get("error")))', '    return a["result"]',
    'out = {}', 'rec = ask("record", **D)',
    'out["record"] = {"network": rec["network"], "topology": rec["topology"], "kinds": rec["kinds"], "dropped": rec["dropped"], "window": rec["window"]}',
    'out["grammar"] = ask("grammar", split=2014)', 'out["detailed"] = ask("grammar", split=2014, detailed=True)',
    'w.stop()', 'print(json.dumps(out))',
  ].join('\n');
  const r = spawnSync('python', ['-c', driver, join(dir, 'request.json')], { cwd: ROOT, encoding: 'utf8', maxBuffer: 1 << 30, env: { ...process.env, PYTHONIOENCODING: 'utf-8' } });
  if (r.status !== 0) {
    console.error((r.stderr || r.stdout || '').slice(-2000));
    process.exit(1);
  }
  out = JSON.parse(r.stdout);
  for (const g of [out.grammar, out.detailed]) trim(g);
  writeFileSync(join(HERE, 'results.json'), JSON.stringify(out, null, 1));
}

const show = (name, G) => {
  const R = G.record, C = G.changes;
  console.log(`\n== ${name}: the record (${R.events} events) and its changes (${C.events})`);
  for (const [label, X] of [['record', R], ['with uses', G.uses], ['with regimes in force', G.regimes], ['with both', G.both], ['changes', C]]) {
    const E = X.entropies, P = X.prequential;
    console.log(`${label}: χ ${X.cells.euler} = Σκ ${X.cells.local}; ${E.rules} rules, ${E.matches} matches, ${E.contexts} contexts; ` +
      `H(R) ${E.rule}, H(R|M) ${E.ruleGivenMatch}, H(R|C) ${E.ruleGivenContext}; prequential ${P.total.join(' / ')} bits (${P.refits} refits); ` +
      `per event before (${P.eras[0].events}) ${P.eras[0].bitsPerEvent.join(' / ')}, after (${P.eras[1].events}) ${P.eras[1].bitsPerEvent.join(' / ')}; ` +
      `${P.newRulesAfter} rules new after the split; depth ${X.steps.depth}; inexact keys ${X.inexactKeys}; final γ ${X.grammar.gamma}, α ${X.grammar.alpha1}, ${X.grammar.alpha2}`);
  }
  console.log('the record by year (bits per event at levels 0 / 1 / 2, then level 2 with both states):');
  for (const [k, y] of R.prequential.years.entries())
    console.log(`  ${y.year}\t${y.events}\t${y.newRules} new\t${y.bits.map((b) => (b / y.events).toFixed(3)).join(' / ')}\t${(G.both.prequential.years[k].bits[2] / y.events).toFixed(3)}`);
  console.log('candidates', JSON.stringify(G.candidates), 'above', JSON.stringify(G.above), 'refinement', JSON.stringify(G.refinement));
  console.log('rules of the record (count, κ, share of χ):');
  for (const x of R.rules.slice(0, 25))
    console.log(`  ${x.count}\tκ ${x.kappa}\tχ ${x.cells.euler}\t${x.description}${x.rate ? `\t${x.rate.perYear}/yr of ${x.rate.consumes}` : ''}`);
  console.log("the window's ends:", JSON.stringify(R.boundary));
  console.log('births:');
  for (const x of R.rules.filter((q) => q.kind === 'birth')) console.log(`  ${x.count}\tκ ${x.kappa}\t${x.description}`);
  for (const [kind, L] of Object.entries(C.lives)) {
    console.log(`lives of the ${kind} couplings without the observations: ${L.words} words, ${L.inputLength} letters → ${L.size}, ${L.rules} rules`);
    for (const x of L.top.slice(0, 6))
      console.log(`  ${x.uses} uses × ${x.length} by ${x.lives} lives of ${x.distinctNames} names, ${x.commonestLives} of them ${x.commonest} (${x.names.slice(0, 4).join('; ')}): ${x.expansion.slice(0, 8).join(' → ')}${x.expansion.length > 8 ? ' → …' : ''}`);
  }
  console.log('novel after the split:', R.novel.length, 'rules,', R.novel.reduce((a, x) => a + x.count, 0), 'events');
};
show('by types', out.grammar);
show('by kinds', out.detailed);
