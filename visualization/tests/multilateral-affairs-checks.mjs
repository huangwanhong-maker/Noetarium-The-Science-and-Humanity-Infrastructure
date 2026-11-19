// Checks of the affairs of an undertaking drawn (visualization/js/multilateral/affairs.js), ADR 0134, on answers in the form the API's
// engine gives them (science/affairs.hpp), for the invented records of api/tests/check_multilateral.py: a question under Article 27(3)
// with a permanent member abstaining, a negotiation of two issues in tenths with two drafts, a pair's events, two mandates, two review
// cycles, a lattice of two bodies and reciprocity over three windows. Each drawing well formed with no NaN; a seat for every member
// with each act told by a glyph and a word, the decisive ringed; a cell for every package of the field and the drafts' frames; the
// reports' ghost around bars where they count more; an arrow for an open-ended mandate and a cross for a review overdue; a thread for
// each repeat; a node for every group of the lattice with its regularities in words. Called by run-tests.mjs.

import { multilateral as M } from '../js/index.js';

const count = (text, part) => text.split(part).length - 1;

const PERMANENT = ['China', 'France', 'Russian Federation', 'United Kingdom', 'United States'];
const MEMBERS = [...PERMANENT, ...Array.from({ length: 10 }, (_, k) => `elected member ${k + 1}`)];
const RULE = { id: 'article-27-3', title: 'Article 27(3)', instrument: 'Charter of the United Nations, Article 27(3)', members: MEMBERS };
const ACTS = Object.fromEntries(MEMBERS.map((m) => [m, 'no']));
for (const m of [...PERMANENT.slice(0, 4), ...MEMBERS.slice(5, 10)]) ACTS[m] = 'yes';
ACTS['United States'] = 'abstain';
const OUTCOME = {
  question: 'q1', answer: 'adopted', reason: 'adopted under every admissible reading of the rule\'s terms',
  interpretations: [{ readings: { 'concurring vote': 'an affirmative vote' }, gives: 'not adopted', admissible: false, reason: 'not adopted' },
    { readings: { 'concurring vote': 'any vote but a negative one' }, gives: 'adopted', admissible: true, reason: 'adopted' }],
  set_aside: ['concurring vote=an affirmative vote'], pivots: [], unrecorded: [],
  decisive: [...PERMANENT.slice(0, 4), ...MEMBERS.slice(5, 10)].map((m) => ({ member: m, act: 'yes', flips: ['no', 'abstain', 'absent'] })).concat([{ member: 'United States', act: 'abstain', flips: ['no', 'absent'] }]),
  discrepancy: '',
  tally: { clause: 'all of', kind: 'all of', met: true, words: '2 of its 2 parts met; all are needed', parts: [
    { clause: 'nine', kind: 'threshold', met: true, words: '9 members yes, at least 9 needed: reached', parts: [] },
    { clause: 'permanent', kind: 'concurrence', met: true, words: 'every member whose concurrence it requires concurs (yes, abstain, not voting concur)', parts: [] }] },
};

const tenths = (id) => ({ id, text: id, options: Array.from({ length: 11 }, (_, k) => ({ id: String(k), text: `${k}/10` })) });
const NEGOTIATION = { name: 'two issues traded at different rates', issues: [tenths('X'), tenths('Y')],
  drafts: [{ id: 'second reading', open: { X: ['8'] } }, { id: "chair's text", open: { X: ['8'], Y: ['4', '5'] } }] };
const agreeable = [];
for (let x = 7; x <= 10; x++) for (let y = 0; y <= 3; y++) agreeable.push({ choice: { X: String(x), Y: String(y) }, words: `X: ${x}/10; Y: ${y}/10`, agreement: 'clear', parties: [] });
const ZONE = {
  problems: [], every_option: { draft: 'every option', packages: 121, clear: 16, arguable: 0, agreeable, settled: [], blocking: [] },
  drafts: [{ draft: 'second reading', when: '', zone: { packages: 11, clear: 4, arguable: 0 }, settled_now: ['X'], reopened: [], notes: [] },
    { draft: "chair's text", when: '', zone: { packages: 2, clear: 0, arguable: 0 }, settled_now: [], reopened: [], notes: ['the zone emptied: no package of this draft can be agreed', 'B accepts no package of this draft'] }],
  linkage: { alone: [{ issue: 'X', agreeable: 0, clear: 0 }, { issue: 'Y', agreeable: 0, clear: 0 }], linked: 16, linked_clear: 16, created: true },
  claimed: { between: ['A', 'B'], basis: 'the negotiators\' values at the midpoints of their intervals', nash: { point: [72, 58], package: 'X: 8/10; Y: 1/10' } },
};

const EVENTS = { series: [{ pair: ['north', 'south'], periods: [
  { period: '2026-01', events: { 'verbal cooperation': 2 }, reports: { 'verbal cooperation': 51 }, net: 2 },
  { period: '2026-02', events: { 'material conflict': 1, 'verbal conflict': 1 }, reports: { 'material conflict': 1, 'verbal conflict': 1 }, net: -2 }] }] };

const MANDATES = { mandates: [
  { id: 'm1', title: 'a programme on water', begins: '2020-01-01', ends: '2022-01-01', resourced: true, earlier: [], state: 'renewed', by: 'm2', why: 'renewed by m2', review: { last: '', due: '2021-01-01', overdue: false, never: true } },
  { id: 'm2', title: 'the programme widened', begins: '2022-01-01', ends: '', resourced: false, earlier: [{ link: 'renews', mandate: 'm1' }], state: 'in force', by: '', why: 'in force, open-ended', review: { last: '', due: '2023-01-01', overdue: true, never: true } }] };

const REVIEWS = {
  recommendations: [{ id: 'r1', cycle: 'c1', from: 'south', to: 'north', theme: 'schools', response: 'supported', reported: true, done: 'implemented', standing: 'claimed', words: 'supported, implemented (claimed)' },
    { id: 'r3', cycle: 'c1', from: 'west', to: 'north', theme: 'press', response: 'noted', reported: false, done: '', standing: 'not reported', words: 'noted' },
    { id: 'r2', cycle: 'c2', from: 'south', to: 'north', theme: 'schools', response: 'no response', reported: false, done: '', standing: 'not reported', words: 'unanswered' }],
  repeats: [{ later: 'r2', earlier: 'r1', declared: false }],
};

const LATTICE = {
  lattice: { groups: [{ parties: ['a', 'b', 'c'], institutions: ['REG'] }, { parties: ['a', 'b'], institutions: ['REG', 'SUB'] }, { parties: [], institutions: ['REG', 'SUB', 'CLUB'] }],
    covers: [[1, 0], [2, 1]], top: 0, bottom: 2 },
  regularities: [{ premise: [], conclusion: ['REG'] }, { premise: ['REG', 'CLUB'], conclusion: ['SUB'] }],
  reciprocity: [{ days: 30, pairwise: 1, group: 0 }, { days: 91, pairwise: 1, group: 0 }, { days: 365, pairwise: 0.4, group: 0 }],
};

export function multilateralAffairsChecks({ assert }) {
  const sane = (svg) => M.balanced(svg) && !svg.includes('NaN') && !svg.includes('undefined');

  const o = M.svgOutcome(OUTCOME, { rule: RULE, question: { id: 'q1', acts: ACTS } });
  assert('multilateral affairs: the question is well formed', sane(o));
  assert('multilateral affairs: a seat for every member of the Council, each act told by a glyph and counted in words',
    count(o, 'class="ma-seat"') === 15 && o.includes('9 yes or joined') && o.includes('5 no') && o.includes('1 abstained') && o.includes('◐'), count(o, 'class="ma-seat"'));
  assert('multilateral affairs: the ten decisive members ringed, and said so', count(o, 'stroke="#8E4FA0" stroke-width="1.4"/>') === 11 && o.includes('10 decisive'), count(o, 'stroke="#8E4FA0" stroke-width="1.4"/>'));
  assert('multilateral affairs: the verdict, both clauses met, and the reading set aside struck through and said so',
    o.includes('✓ adopted') && count(o, 'class="ma-clause" data-met="true"') === 3 && o.includes('text-decoration="line-through"') && o.includes('(set aside)'));
  const body = M.hemicycle(193, 200, 200, 60, 180);
  assert('multilateral affairs: a hemicycle seats all 193 members of the Assembly, none outside its half circle', body.length === 193 && body.every((s) => s.y <= 200.0001));

  const z = M.svgZone(ZONE, NEGOTIATION);
  assert('multilateral affairs: the zone is well formed, a cell for every one of the 121 packages', sane(z) && count(z, 'class="ma-package"') === 121);
  assert('multilateral affairs: sixteen cells agreed clearly, told by a glyph', count(z, 'data-agreement="clear"') === 16 && count(z, 'data-agreement="untenable"') === 105);
  assert('multilateral affairs: a frame for each draft, the latest solid, and the Nash point marked', count(z, 'class="ma-draft"') === 2 && z.includes('class="ma-nash"'));
  assert('multilateral affairs: the drafts in order, the emptied zone said so', count(z, 'class="ma-step"') === 2 && z.includes('the zone emptied') && z.includes('linkage creates this zone'));

  const e = M.svgEventSeries(EVENTS);
  assert('multilateral affairs: a pair\'s events well formed, one period a month, the net beside each', sane(e) && count(e, 'class="ma-period"') === 2 && e.includes('+2') && e.includes('-2'));
  assert('multilateral affairs: where the reports count fifty-one and the events two, the ghost is drawn; where they agree, none (the key has its own)', count(e, 'stroke-dasharray="2 2" stroke-width="0.8"/>') === 2,
    count(e, 'stroke-dasharray="2 2" stroke-width="0.8"/>'));

  const m = M.svgMandates(MANDATES, { at: '2024-06-01' });
  assert('multilateral affairs: mandates well formed, a lane each, the renewal threaded and named', sane(m) && count(m, 'class="ma-mandate"') === 2 && m.includes('>renews<'));
  assert('multilateral affairs: an open-ended mandate ends in an arrow, an unresourced one is dashed, a review overdue a cross',
    m.includes('>→<') && m.includes('stroke-dasharray="3 2" rx="1.5"') && m.includes('>✕<'));

  const r = M.svgReviews(REVIEWS);
  assert('multilateral affairs: review cycles well formed, a column each, a row for each recommendation with its state in words',
    sane(r) && count(r, 'class="ma-recommendation"') === 3 && r.includes('implemented, claimed') && r.includes('unanswered') && r.includes('>noted<'));
  assert('multilateral affairs: the repeat matched across cycles is a dashed thread', count(r, 'class="ma-repeat"') === 1 && r.includes('stroke-dasharray="3 2"'));

  const l = M.svgLattice(LATTICE);
  assert('multilateral affairs: the lattice well formed, a node for every group, each institution introduced once',
    sane(l) && count(l, 'class="ma-group"') === 3 && count(l, 'font-weight="600">REG<') === 1 && count(l, 'font-weight="600">SUB<') === 1);
  assert('multilateral affairs: the regularities in words', l.includes('every party belongs to REG') && l.includes('every member of REG and CLUB belongs to SUB'));
  const q = M.svgReciprocity(LATTICE);
  assert('multilateral affairs: reciprocity drawn over each window, the pairs and the group side by side', sane(q) && count(q, 'class="ma-window"') === 3 && q.includes('pairs 1') && q.includes('group 0'));
}
