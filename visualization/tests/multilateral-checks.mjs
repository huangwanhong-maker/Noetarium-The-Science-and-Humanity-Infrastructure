// Checks of multilateral practice drawn (visualization/js/multilateral), ADR 0132. On the invented undertaking the API's check reads
// (api/tests/check_multilateral.py), whose numbers the domain computed: a tie's status read as the domain reads it (3 delegations present,
// 9 pairs absent within the register, 18 not observed), so that the page and the engine agree; the round well formed, a seat for every
// participant, a thread for every tie in force of a layer drawn, a layer left out leaving its threads out, a chosen seat's marks one for
// every other seat and layer with each status as the record says in either direction, the participant without whom authority falls
// apart ringed twice, nothing written NaN; the strands with a knot for every dated change and the date read marked; the ledger's states
// and standings each told by a glyph and a word, a criterion not observed said so, a commitment with no indicator said so; a rule's
// members alike drawn once with their number, vetoes in words; ideal points with no two names meeting. Called by run-tests.mjs.

import { multilateral as M } from '../js/index.js';

const count = (text, part) => text.split(part).length - 1;

const UNDERTAKING = {
  name: 'an invented programme of two cities',
  types: [{ name: 'authorises', meaning: 'gives authority', authority: true }, { name: 'relies', meaning: 'relies on claims', evidential: true },
    { name: 'challenges', meaning: 'may contest', challenge: true }],
  participants: [{ id: 'fund', name: 'the fund', kind: 'organisation', constituted: true }, { id: 'sec', name: 'the secretariat', kind: 'organisation' },
    { id: 'north', name: 'the northern office', kind: 'city' }, { id: 'south', name: 'the southern office', kind: 'city' },
    { id: 'model', name: 'a model', kind: 'AI system' }, { id: 'panel', name: 'the panel', kind: 'forum', constituted: true }],
  ties: [{ type: 'authorises', from: 'fund', to: 'sec', since: '2025-01-01' }, { type: 'authorises', from: 'sec', to: 'north', since: '2025-02-01' },
    { type: 'authorises', from: 'sec', to: 'south', since: '2025-02-01' }, { type: 'relies', from: 'sec', to: 'model', since: '2025-03-01' },
    { type: 'challenges', from: 'north', to: 'panel', since: '2025-01-01' }],
  observations: [{ type: 'authorises', design: 'the register of delegations', since: '2025-01-01', until: '2026-01-01', covered: ['fund', 'sec', 'north', 'south'] }],
};
// What the engine answered for it on 2025-07-01 (api/tests/check_multilateral.py), in part.
const ANSWER = {
  layers: [{ type: 'authorises', present: 3, absent: 9, not_observed: 18 }, { type: 'relies', present: 1, absent: 0, not_observed: 29 }, { type: 'challenges', present: 1, absent: 0, not_observed: 29 }],
  diagnoses: [{ kind: 'succession', where: 'participant sec', what: '' }, { kind: 'succession', where: 'tie fund authorises sec', what: '' }],
  timeline: [{ date: '2025-01-01', kind: 'tie begins', what: 'fund authorises sec', participants: ['fund', 'sec'] },
    { date: '2025-01-01', kind: 'observed from', what: 'the register of delegations (authorises)', participants: [] },
    { date: '2025-02-01', kind: 'tie begins', what: 'sec authorises north', participants: [] },
    { date: '2025-06-01', kind: 'decision', what: 'd1: tariffs set', participants: ['sec'] },
    { date: '2026-01-01', kind: 'observed until', what: 'the register of delegations (authorises)', participants: [] }],
};
const PARTNERSHIP_ANSWER = {
  states: [{ commitment: 'c1', text: 'a plant', by: 'a', made: '2024-02-01', due: '2025-12-31', indicator: 'people served', unit: 'people', target: 1000, latest: 1200, progress: 1.2, reached_on: '2025-06-30', state: 'honoured', reports: 1 },
    { commitment: 'c2', text: 'a joint programme', by: 'b', made: '2024-02-01', due: '2026-12-31', indicator: '', target: null, latest: null, progress: null, reached_on: '', state: 'unreported', reports: 0 }],
  screens: [{ commitment: 'c1', checks: ['specific', 'measurable', 'achievable', 'resource-based', 'time-bound'].map((n) => ({ name: n, verdict: { status: n === 'achievable' ? 'undecided' : 'holds', reason: '' } })) }],
  benefits: [{ party: 'a', standing: 'clear', measure: { kind: 'probability', value: 1 }, met: ['water'], unmet: [], open: [], unobserved: [], interests: [{ dimension: 'water', threshold: 50 }] },
    { party: 'b', standing: 'arguable', measure: { kind: 'interval', lower: 0.1, upper: 0.16 }, met: [], unmet: [], open: ['water'], unobserved: [], interests: [{ dimension: 'water', threshold: 75 }] },
    { party: 'c', standing: 'undecided', measure: { kind: 'none' }, met: [], unmet: [], open: [], unobserved: ['standing'], interests: [{ dimension: 'standing', threshold: 1 }] }],
  values: { a: [{ dimension: 'water', value: 70, lower: 60, upper: 80 }], b: [{ dimension: 'water', value: 70, lower: 60, upper: 80 }], c: [] },
};

export function multilateralChecks({ assert }) {
  // ---- status, as the domain reads it --------------------------------------------------------------------------------------------
  const c = M.layerCounts(UNDERTAKING, 'authorises', '2025-07-01');
  assert('multilateral: the delegations read as the domain reads them: 3 present, 9 absent within the register, 18 not observed (the engine\'s own numbers)',
    c.present === 3 && c.absent === 9 && c['not observed'] === 18, JSON.stringify(c));
  assert('multilateral: outside the register a pair is not observed, never absent', M.status(UNDERTAKING, 'authorises', 'fund', 'panel', '2025-07-01') === 'not observed'
    && M.status(UNDERTAKING, 'authorises', 'north', 'south', '2025-07-01') === 'absent' && M.status(UNDERTAKING, 'authorises', 'sec', 'north', '2025-01-15') === 'absent');

  // ---- the round -------------------------------------------------------------------------------------------------------------------
  const svg = M.svgRound(UNDERTAKING, { at: '2025-07-01', answer: ANSWER });
  assert('multilateral: the round is well formed, with no NaN', M.balanced(svg) && !svg.includes('NaN'));
  assert('multilateral: a seat for every participant, by its kind\'s glyph', count(svg, 'class="mr-seat') === 6 && svg.includes('⬡') && svg.includes('⌂'));
  assert('multilateral: a thread for every tie in force (5 on 1 July 2025)', count(svg, 'class="mr-thread') === 5, count(svg, 'class="mr-thread'));
  const early = M.svgRound(UNDERTAKING, { at: '2025-01-15' });
  assert('multilateral: read on 15 January, before the delegations and the reliance, two threads', count(early, 'class="mr-thread') === 2, count(early, 'class="mr-thread'));
  const onlyAuth = M.svgRound(UNDERTAKING, { at: '2025-07-01', layers: ['authorises'] });
  assert('multilateral: a layer left out leaves its threads out', count(onlyAuth, 'class="mr-thread') === 3 && count(onlyAuth, 'class="mr-key off"') === 2);
  assert('multilateral: the participant without whom authority falls apart is ringed twice, the only link drawn doubled',
    count(svg, 'class="mr-cut"') === 2 && count(svg, 'class="mr-only"') === 1);
  const chosen = M.svgRound(UNDERTAKING, { at: '2025-07-01', selected: 'north', answer: ANSWER });
  const marks = count(chosen, 'class="mr-pair');
  assert('multilateral: read from the northern office, one mark for every other seat and layer (5 × 3)', marks === 15, marks);
  const present = count(chosen, 'data-status="present"'), absent = count(chosen, 'data-status="absent"'), unobserved = count(chosen, 'data-status="not observed"');
  // north–sec authorises (present), north–panel challenges (present); north–fund, north–south authorises absent; the rest not observed
  assert('multilateral: its marks say what the record says in either direction: 2 present, 2 absent, 11 not observed',
    present === 2 && absent === 2 && unobserved === 11, `${present} ${absent} ${unobserved}`);
  assert('multilateral: a pair not observed is a broken ring, never nothing, and the key says so', count(chosen, 'class="mr-pair unobserved"') === 11 && chosen.includes('stroke-dasharray="1.1 1.1"') && chosen.includes('not observed: a gap, never nothing'));
  assert('multilateral: the key gives each layer\'s counts from the engine', svg.includes('3 present, 9 absent, 18 not observed'));

  // ---- the strands ------------------------------------------------------------------------------------------------------------
  const st = M.svgStrands(ANSWER, { at: '2025-07-01', partnership: PARTNERSHIP_ANSWER, record: { contributions: [{ party: 'c', when: '2024-03-01', amount: 1e6, unit: 'EUR' }] } });
  const knots = count(st, 'class="ms-knot');
  // 5 from the timeline, 2 made, 1 honoured, 1 due, 1 contribution
  assert('multilateral: a knot for every dated change: the timeline\'s, each commitment made and honoured or due, each contribution', knots === 10 && M.balanced(st) && !st.includes('NaN'), knots);
  assert('multilateral: the strands are named, the contributions by their unit, and the date read marked',
    st.includes('>relations<') && st.includes('>decisions<') && st.includes('>contributions, EUR<') && st.includes('read on 2025-07-01'));
  assert('multilateral: a record with nothing dated says so', M.svgStrands({ timeline: [] }).includes('No change is dated'));

  // ---- the ledger ---------------------------------------------------------------------------------------------------------------
  const cm = M.svgCommitments(PARTNERSHIP_ANSWER, { names: { a: 'City A', b: 'City B' } });
  assert('multilateral: each state told by a glyph and a word', cm.includes('✓ honoured') && cm.includes('? unreported') && M.balanced(cm) && !cm.includes('NaN'));
  assert('multilateral: the screen\'s letters, achievable undecided', count(cm, 'class="ml-criterion holds"') === 4 && count(cm, 'class="ml-criterion undecided"') === 1 && cm.includes('>S<') && cm.includes('>T<'));
  assert('multilateral: a commitment with no indicator says that no one can tell', cm.includes('no indicator: no one can tell'));
  assert('multilateral: progress to the target in words', cm.includes('1,200 of 1,000 people · 120%') || cm.includes('1200 of 1000'), cm.match(/of 1[,0]*000[^<]*/)?.[0]);
  const bn = M.svgBenefits(PARTNERSHIP_ANSWER, {});
  assert('multilateral: each party\'s standing by a glyph and a word, never one number for all', bn.includes('● clear') && bn.includes('◐ arguable') && bn.includes('? undecided') && count(bn, 'class="ml-party') === 3);
  assert('multilateral: the measure in words, an interval where the evidence leaves one', bn.includes('P(all met) = 1') && bn.includes('P(all met) in [0.1, 0.16]'));
  assert('multilateral: a criterion not observed is said so, not drawn empty', bn.includes('>not observed<') && count(bn, 'class="ml-interest not-observed"') === 1);

  // ---- power and the line -----------------------------------------------------------------------------------------------------
  const ga = { route: 'generating functions', power_to_act: 0.0001, power: Array.from({ length: 193 }, (_, i) => ({ member: `m${i}`, penrose: 0.05, banzhaf: 1 / 193, shapley_shubik: 1 / 193 })) };
  const pg = M.svgPower(ga);
  assert('multilateral: 193 equal members are one row, with how many they are', count(pg, 'class="mp-member"') === 1 && pg.includes('and 192 alike') && pg.includes('never a measure of influence'));
  const sc = { power: [{ member: 'P1', penrose: 0.05, banzhaf: 0.167, shapley_shubik: 0.196 }, { member: 'P2', penrose: 0.05, banzhaf: 0.167, shapley_shubik: 0.196 }, { member: 'E1', penrose: 0.005, banzhaf: 0.0165, shapley_shubik: 0.00186 }],
    properties: { vetoes: ['P1', 'P2'], dummies: [] } };
  const ps = M.svgPower(sc);
  assert('multilateral: vetoes said in words, members alike drawn once', ps.includes('>veto<') && count(ps, 'class="mp-member"') === 2 && M.balanced(ps));
  const pts = Array.from({ length: 40 }, (_, i) => ({ member: `member ${i}`, x: Math.sin(i * 1.7) * 1.5, lower: Math.sin(i * 1.7) * 1.5 - 0.2, upper: Math.sin(i * 1.7) * 1.5 + 0.2, votes: 100 }));
  const ln = M.svgLine({ points: pts, anchor: 'member 3', classified: 0.9, apre: 0.7 }, { width: 860 });
  const labels = [...ln.matchAll(/<text x="([\d.]+)" y="([\d.]+)" text-anchor="middle" font-size="9.5" fill="[^"]+">([^<]+)<\/text>/g)].map((m) => ({ x: +m[1], y: +m[2], w: m[3].length * 5.2 }));
  let meet = 0;
  for (let i = 0; i < labels.length; i++) for (let j = i + 1; j < labels.length; j++) {
    const a = labels[i], b = labels[j];
    if (Math.abs(a.y - b.y) < 1 && Math.abs(a.x - b.x) < (a.w + b.w) / 2 - 8) meet++;
  }
  const bg = M.svgBargaining({ nash: [2, 1], kalai_smorodinsky: [2, 1], egalitarian: [4 / 3, 4 / 3], ideal: [4, 2], disagreement: [0, 0], frontier: [[0, 2], [4, 0]] }, { parties: ['A', 'B'] });
  assert('multilateral: a bargaining problem drawn with its frontier, its two points and three solutions, each named', M.balanced(bg) && count(bg, 'class="mp-solution"') === 5
    && bg.includes('>Nash<') && bg.includes('>Kalai–Smorodinsky<') && bg.includes('>egalitarian<') && bg.includes('class="mp-frontier"') && !bg.includes('NaN'));
  assert('multilateral: 40 members on a line, every name placed, no two meeting in a row', labels.length === 40 && meet === 0 && ln.includes('class="mp-point anchor"'), `${labels.length} names, ${meet} meeting`);
}
