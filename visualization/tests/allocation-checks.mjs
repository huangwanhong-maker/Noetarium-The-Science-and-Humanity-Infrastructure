// Checks of the allocation drawings (visualization/js/allocation), ADR 0136. On what the API's allocation engine answers, abridged:
// amounts as they are read; the plots in the order reached, the largest kept and the rest gathered, the excluded last; the channel
// narrowing at each sluice, each sluice with its principle and what it gave, each plot with its amount, the excluded behind a gate
// with the reason, no two plots overlapping; the covering pairs of a dominance order (a chain's long pair left out) and its levels,
// an order that is not one refused; the Hasse diagram with the undominated at the top, the declared marked, no two slips
// overlapping, the principles each option is best under; the stakes widest first; the record with an entry for each act and every
// rule told by its word; a budget by group with a mark for each rule. Resources of many kinds allocated by evolution (ADR 0137):
// the field of the kept with a stone each, the first ringed, the least outcome highest where it is; an allocation on its recipients
// with every recipient drawn, the thread through the granted in order and the one not granted named so, a flag with its direction;
// which grants commute, crossed where not, with the declared marked; a type's laws as seals with their words; strengths, a model's
// marked as proposed. Called by run-tests.mjs.

import { allocation as A } from '../js/index.js';

const ROUND = {
  name: 'small grants', unit: '£', amount: 1200,
  entities: [{ id: 'north', name: 'North' }, { id: 'south', name: 'South' }, { id: 'east', name: 'East' }, { id: 'west', name: 'West' }],
  stages: [{ kind: 'eligibility', principle: 'residence' }, { kind: 'floor', principle: 'sufficiency' }, { kind: 'proportional', principle: 'equality per head' }],
};
const RESULT = {
  entities: [
    { id: 'north', name: 'North', amount: 500, by_stage: [0, 400, 100], excluded: null, reasons: [] },
    { id: 'south', name: 'South', amount: 400, by_stage: [0, 200, 200], excluded: null, reasons: [] },
    { id: 'east', name: 'East', amount: 300, by_stage: [0, 0, 300], excluded: null, reasons: [] },
    { id: 'west', name: 'West', amount: 0, by_stage: [0, 0, 0], excluded: 'excluded at stage 1 (eligibility): its population (3) is below 5', reasons: [] },
  ],
  stages: [{ kind: 'eligibility', principle: 'residence', given: 0, remaining: 1200 }, { kind: 'floor', principle: 'sufficiency', given: 600, remaining: 600 }, { kind: 'proportional', principle: 'equality per head', given: 600, remaining: 0 }],
  order: [{ entity: 'north', stage: 1, first: 400 }, { entity: 'south', stage: 1, first: 200 }, { entity: 'east', stage: 2, first: 300 }, { entity: 'west', stage: null, first: 0 }],
  left: 0, notes: [],
};

export function allocationChecks({ assert }) {
  assert('allocation: amounts as they are read, the unit before a symbol and after a word', A.amount(1200, '£') === '£1,200' && A.amount(2.5e6, 'PLN') === '2.50 m PLN' && A.amount(NaN) === '—', `${A.amount(1200, '£')} ${A.amount(2.5e6, 'PLN')}`);

  const P = A.plots(ROUND, RESULT, 2);
  assert('allocation: the plots in the order reached, the largest two kept, the third gathered, the excluded last',
    P[0].id === 'north' && P[1].id === 'south' && P[2].gathered && P[2].amount === 300 && P[2].name === 'and 1 others' && P[3].id === 'west' && P[3].excluded, JSON.stringify(P.map((p) => p.id)));

  const L = A.layoutChannel(ROUND, RESULT);
  assert('allocation: the channel narrows at each sluice and never widens', L.widths.every((w, k) => k === 0 || w <= L.widths[k - 1] + 1e-9) && L.widths[0] > L.widths[L.widths.length - 1], L.widths.join());
  const xs = L.P.map((_, i) => L.plotX(i));
  assert('allocation: no two plots overlap', xs.every((x, i) => i === 0 || x >= xs[i - 1] + L.plotW));
  const svg = A.svgChannel(ROUND, RESULT, { selected: 'south' });
  assert('allocation: each sluice says its principle and what it gave', /for sufficiency/.test(svg) && /gave £600/.test(svg) && /for equality per head/.test(svg));
  assert('allocation: each plot says its amount, and the chosen one is marked', /£500/.test(svg) && /£400/.test(svg) && /£300/.test(svg) && /class="plot chosen" data-entity="south"/.test(svg));
  assert('allocation: the excluded plot stands behind a closed gate, with its reason', /class="plot excluded" data-entity="west"/.test(svg) && /class="closed"/.test(svg) && /below 5/.test(svg));
  assert('allocation: each plot is filled by stage, in that stage\'s ink', (svg.match(/class="fill"/g) || []).length === 5 && svg.includes(A.STAGE_INKS[1]) && svg.includes(A.STAGE_INKS[2]));

  // a > b > c with a > c given too; d alone.
  const pairs = [[0, 1], [1, 2], [0, 2]];
  const cov = A.covering(4, pairs);
  assert('allocation: the covering pairs of a chain leave its long pair out', cov.length === 2 && !cov.some(([a, b]) => a === 0 && b === 2), JSON.stringify(cov));
  assert('allocation: each option\'s level is the longest chain above it', A.levels(4, pairs).join() === '0,1,2,0');
  let refused = false;
  try { A.levels(2, [[0, 1], [1, 0]]); } catch { refused = true; }
  assert('allocation: a dominance that is not an order is refused', refused);

  const J = {
    principles: ['leximin', 'headcount below 54.8'],
    options: [
      { names: ['as declared'], undominated: false, values: [0, 0.5] },
      { names: ['rules: stage 1 by equal_awards'], undominated: true, values: [10, 1] },
      { names: ['rules: stage 1 by talmud', 'rules: stage 1 by adjusted_proportional'], undominated: true, values: [5, 0.5] },
      { names: ['rules: stage 1 by proportional'], undominated: false, values: [5.45, 1] },
    ],
    best_under: [[1], [0, 2]], dominance: [[2, 0], [1, 3]], undominated: [1, 2], declared_undominated: false,
    stakes: [{ entity: 'north', least: 5, most: 10, declared: 0, best: 1 }, { entity: 'south', least: 50, most: 55, declared: 60, best: 2 }, { entity: 'east', least: 1, most: 1, declared: 1, best: 1 }],
  };
  const O = A.layoutOrder(J);
  assert('allocation: the undominated stand at the top of the Hasse diagram, the dominated below them', O.at.get(1).level === 0 && O.at.get(2).level === 0 && O.at.get(0).level === 1 && O.at.get(3).level === 1);
  const slips = [...O.at.values()];
  assert('allocation: no two slips of the diagram overlap', slips.every((a, i) => slips.every((b, j) => i === j || a.x + a.w <= b.x || b.x + b.w <= a.x || a.y + a.h <= b.y || b.y + b.h <= a.y)));
  const os = A.svgOrder(J);
  assert('allocation: the declared round marked, each option with the principles it is best under, a line for each covering pair',
    /class="option dominated declared"/.test(os) && /best\s+under\s+leximin/.test(os.replace(/<[^>]+>/g, ' ')) && /best\s+under\s+headcount\s+below\s+54\.8/.test(os.replace(/<[^>]+>/g, ' ')) && (os.match(/class="covers"/g) || []).length === 2);
  const tall = Math.max(...[...O.at.values()].map((p) => p.lines.length));
  assert('allocation: a slip\'s words are wrapped to its width, and every slip is as tall as the most lines need', [...O.at.values()].every((p) => p.h === 30 + 14 * tall));
  const ss = A.svgStakes(J, ROUND);
  assert('allocation: the stakes widest first, each with its least, most and the declared amount in words',
    ss.indexOf('data-entity="north"') < ss.indexOf('data-entity="south"') && ss.indexOf('data-entity="south"') < ss.indexOf('data-entity="east"') && /£5 – £10 \(£0\)/.test(ss), ss.slice(0, 200));

  const PR = {
    head: 'abcdef0123456789',
    journal: [{ when: '2026-11-02', who: 'the council', what: 'declare', about: 'x', note: 'the round published', seal: 'aaa' }, { when: '2026-11-10', who: 'the council', what: 'allocate', about: 'x', note: 'y', seal: 'bbb' },
      { when: '2026-11-11', who: 'a clerk', what: 'appeal', about: 'north', note: 'datum population: a new estate', seal: 'ccc' }],
    rules: [{ name: 'consistency', framework: 'Leventhal', status: 'holds', claim: 'c', reason: 'r' }, { name: 'correctability', framework: 'both', status: 'fails', claim: 'c', reason: 'appeal 1 not heard' },
      { name: 'ethicality', framework: 'Leventhal', status: 'undecided', claim: 'c', reason: 'a person\'s judgement' }],
  };
  const rs = A.svgRecord(PR);
  assert('allocation: the record has an entry for each act, with its glyph', (rs.match(/class="entry /g) || []).length === 3 && rs.includes(A.ACT_GLYPHS.appeal));
  assert('allocation: each rule told by its word as well as its seal: holds, fails, left to people',
    /class="rule holds"/.test(rs) && /class="rule fails"/.test(rs) && /left to people/.test(rs) && /path class="seal fails"/.test(rs) && /circle class="seal open"/.test(rs));

  const B = { rules: [
    { rule: 'recorded', groups: [{ group: 'Bemowo', voters: 100, share_with_nothing: 0.2 }, { group: 'Wola', voters: 50, share_with_nothing: 0.1 }] },
    { rule: 'equal_shares', groups: [{ group: 'Bemowo', voters: 100, share_with_nothing: 0.05 }, { group: 'Wola', voters: 50, share_with_nothing: 0.02 }] }] };
  const bs = A.svgBudget(B);
  assert('allocation: a budget by group: a row for each group, a mark for each rule, the rules named', (bs.match(/class="mark"/g) || []).length === 4 && /equal shares/.test(bs) && /recorded/.test(bs) && bs.indexOf('Bemowo') < bs.indexOf('Wola'));

  // ---- resources of many kinds allocated by evolution (ADR 0137) ----
  {
  const EP = {
    name: 'three areas', recipients: [{ id: 'a', name: 'Area A' }, { id: 'b', name: 'Area B' }, { id: 'c', name: 'Area C' }],
    resources: [{ name: 'money', carrier: 'unit_claims' }, { name: 'grant', carrier: 'claims' }, { name: 'crews', carrier: 'units' }, { name: 'site', carrier: 'items' }],
    attributes: [{ name: 'canopy direction', space: 'circle' }],
  };
  const kept = (total, units, lawful) => ({ metrics: [{ name: 'value', role: 'objective', value: -total, standing: 'computed', units }], lawful,
    amounts: { money: [0, 3, 2], grant: [10, 40, 0], crews: [0, 2, 0] }, owners: { site: { 'river site': 'a' } }, attributes: { 'canopy direction': [[0, -1], [1, 0], [0, 1]] },
    granted: ['a', 'b'], unmet: ['c'] });
  const RES = { kept: [kept(5.3, [1, 3, 1.3], true), kept(5.1, [0.5, 3, 1.6], true), kept(4.0, [1.2, 1.4, 1.4], true)],
    order: { states: 40, commute: [[false, false, true], [false, false, true], [true, true, false]], declared: [[false, false, true], [false, false, false], [true, false, false]] } };
  const F = A.layoutField(RES, { metric: 'value' });
  const fs = A.svgField(RES, { metric: 'value', selected: 1 });
  assert('allocation: the field of the kept: a stone for each, the first ringed, the chosen marked, the least outcome highest where it is',
    (fs.match(/class="stone/g) || []).length === 3 && (fs.match(/class="ring"/g) || []).length === 1 && /stone lawful chosen/.test(fs) && F.pts[2].y < F.pts[1].y && F.pts[0].x > F.pts[2].x,
    `${F.pts.map((p) => `${p.x.toFixed(0)},${p.y.toFixed(0)}`).join(' ')}`);
  const as = A.svgAllocation(EP, RES.kept[0], { places: { a: [-37.80, 144.96], b: [-37.82, 144.95], c: [-37.79, 144.98] } });
  assert('allocation: an allocation on its recipients: each drawn, the thread through the granted in order, the one not granted named so',
    (as.match(/class="recipient/g) || []).length === 3 && /class="thread"/.test(as) && (as.match(/not granted/g) || []).length === 1 && /class="stop"[^>]*>1</.test(as) && /class="stop"[^>]*>2</.test(as));
  assert('allocation: a site as a flag with its direction, crews as pips, what divides as discs',
    (as.match(/class="flag"/g) || []).length === 1 && (as.match(/class="direction"/g) || []).length === 1 && (as.match(/class="pip/g) || []).length === 2 + 3 + 2 && (as.match(/class="disc/g) || []).length === 3);
  const ring = A.placeRecipients(EP, { width: 400, height: 400 });
  assert('allocation: recipients without places stand on a ring', Object.keys(ring).length === 3 && Math.abs(Math.hypot(ring.a[0] - 200, ring.a[1] - 200) - Math.hypot(ring.b[0] - 200, ring.b[1] - 200)) < 1e-9);
  const cs = A.svgCommutation(RES, ['Area A', 'Area B', 'Area C']);
  assert('allocation: which grants commute: every pair a cell, crossed where a test found they do not, the declared marked',
    (cs.match(/class="pair/g) || []).length === 6 && (cs.match(/class="cross"/g) || []).length === 2 && (cs.match(/class="declared"/g) || []).length === 2);
  const ss = A.svgSeals([{ type: 'money', lawful: true, holds: 2, undecided: 1, fails: 0, entries: [
    { law: 'combination is associative', claim: 'c1', status: 'holds', method: 'by construction', reason: 'r' },
    { law: 'money is conserved', claim: 'c2', status: 'holds', method: 'on the generating processes', reason: 'r' },
    { law: 'convertibility', claim: 'c3', status: 'undecided', method: 'by bounded search', reason: 'the budget ran out' }] }]);
  assert('allocation: a type\'s laws as seals, a row for each type, the undecided written out with its word and how it was established',
    (ss.match(/circle class="seal holds"/g) || []).length === 2 && (ss.match(/circle class="seal open"/g) || []).length === 2 && /undecided, by bounded search/.test(ss) && /lawful, 2 holding/.test(ss));
  const whole = A.svgSeals([{ type: 'crews', lawful: true, holds: 2, undecided: 0, fails: 0, entries: [
    { law: 'l1', claim: 'c1', status: 'holds', method: 'by construction', reason: 'r' }, { law: 'l2', claim: 'c2', status: 'holds', method: 'by construction', reason: 'r' }] }]);
  assert('allocation: a type whose laws all hold is one row of seals and how they were established', /every law holds: 2 by construction/.test(whole) && (whole.match(/class="law-text"/g) || []).length === 0);
  const crowd = A.spread({ a: [100, 100], b: [101, 100], c: [100, 101] }, { width: 400, height: 400, pad: 20, gap: 60 });
  assert('allocation: crowded places are spread apart, inside the frame', Math.hypot(crowd.a[0] - crowd.b[0], crowd.a[1] - crowd.b[1]) >= 59 && Math.hypot(crowd.a[0] - crowd.c[0], crowd.a[1] - crowd.c[1]) >= 59 && Object.values(crowd).every(([x, y]) => x >= 20 && y >= 20));
  const st = A.svgStrengths([{ label: 'the people of the project', strength: [1.4, 0.8, 0.9] }, { label: 'a model', strength: [1, 1.1, 0.9], grade: 'C' }]);
  assert('allocation: strengths from judgements, a model\'s marked as proposed and its grade said',
    (st.match(/class="strength"/g) || []).length === 6 && (st.match(/bar proposed/g) || []).length === 3 && /grade C/.test(st) && /never summed into the search/.test(st));
  assert('allocation: names are escaped in the evolution drawings', !/<script/.test(A.svgAllocation({ ...EP, recipients: [{ id: 'a', name: '<script>' }, EP.recipients[1], EP.recipients[2]] }, RES.kept[0])));
  }
}
