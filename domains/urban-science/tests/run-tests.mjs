// Checks of the urban-science domain: its C++ (the street system, its terrain, hours, mixing and visits; the
// rhythm of a record on the calendars' lattice; the response to forcing; the affinity of uses, catalysis and
// spacing; the crowd as a kinematic wave, person-time, transit and circulation; the classification), built with
// CMake and run on calx's verification.
// Run from the repository root:  node domains/urban-science/tests/run-tests.mjs

import { readFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'urban-science' });

// The catalogue of indexes (indicators/, ADR 0128): data, read here for its form.
{
  const rows = (file) => readFileSync(resolve(domain, 'indicators', file), 'utf8').replace(/\n+$/, '').split('\n').map((l) => l.split('\t'));
  const [head, ...cat] = rows('catalogue.tsv');
  const col = Object.fromEntries(head.map((h, i) => [h, i]));
  assert('indicators: every row of the catalogue has its twenty columns', head.length === 20 && cat.every((r) => r.length === 20), cat.find((r) => r.length !== 20)?.slice(0, 2).join());
  const ids = new Set(cat.map((r) => r[col.id]));
  assert('indicators: every index is one row', ids.size === cat.length, `${cat.length - ids.size} repeated`);
  const kinds = { quantitative: ['count', 'amount', 'flow', 'rate', 'ratio', 'density', 'price', 'duration', 'distribution', 'coefficient', 'composite', 'network', 'spatial-statistic', 'physical', 'valuation'],
    qualitative: ['ordinal', 'categorical', 'binary', 'fuzzy', 'latent', 'coded'] };
  const badKind = cat.find((r) => !(kinds[r[col.class]] || []).includes(r[col.kind]));
  assert('indicators: every index is quantitative or qualitative, of a kind of its class', !badKind, badKind?.slice(0, 4).join());
  const aggs = ['sum', 'wmean', 'ratio-of-sums', 'decomposable', 'recompute', 'max', 'min', 'median', 'none'];
  const badAgg = cat.find((r) => !aggs.includes(r[col.aggregation].split(':')[0]));
  assert('indicators: every index says how it is carried up the levels, in the vocabulary', !badAgg, badAgg?.slice(0, 6).join());
  const badEst = cat.find((r) => !['high', 'medium', 'low', 'none'].includes(r[col.estimability]) || !['state', 'flow', 'parameter', 'driver', 'output', 'observation'].includes(r[col.role]));
  assert('indicators: every index says how estimable it is and its role in a model', !badEst, badEst?.slice(0, 2).join());
  const quals = cat.filter((r) => r[col.class] === 'qualitative').length;
  assert('indicators: 1,536 indexes, 170 of them qualitative, 73 computed by the infrastructure', cat.length === 1536 && quals === 170 && cat.filter((r) => r[col.computed_by]).length === 73, `${cat.length} ${quals}`);
  const [, ...kw] = rows('keywords.tsv');
  const keywords = new Set(kw.map((r) => `${r[1]}|${r[2]}|${r[0]}`));
  const lost = kw.find((r) => r[3] !== 'excluded' && !ids.has(r[4]));
  assert('indicators: every keyword of the atlas that names an index names one in the catalogue', !lost, lost?.join(' | '));
  const reasons = ['method', 'concept with no standard measure', 'policy instrument', 'document or instrument', 'actor or organisation', 'place'];
  const unexplained = kw.find((r) => r[3] === 'excluded' && !reasons.includes(r[4]));
  assert('indicators: every keyword that names no index says why', !unexplained, unexplained?.join(' | '));
  assert('indicators: every one of the atlas\'s 4,591 keywords is accounted for', keywords.size === 4591 || kw.filter((r, i) => i === 0 || r[0] !== kw[i - 1][0] || r[2] !== kw[i - 1][2]).length === 4591, String(keywords.size));
}

done();
