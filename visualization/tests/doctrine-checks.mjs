// Checks of theories of the law drawn (visualization/js/evidence/doctrine.js), ADR 0133. On a report shaped as the law engine's
// `doctrines` answers it, for an invented park bylaw with its case law: a doctrine's score parted into misfit and worth that sum back
// to it; the field with the acceptable doctrines within the tolerance of the best, the half-plane they lie in, each kind's exact best
// ringed and marked found when the search matches it; the case law in order of date with the decision held back beyond the matter's
// date and its test's verdict; the board of precedents with the factors by side, the reasons a doctrine reads, the matter's side
// forced; and the markup, each mark a glyph and a word, names escaped. Called by evidence-checks.mjs.

import { evidence as E } from '../js/index.js';

const W = { authority: 10, cases: 1, coherence: 2, justification: 1, persuasive: 0.3 };
const FACTORS = [
  { id: 'life_at_risk', side: 'emergency', text: "someone's life was at risk" },
  { id: 'rushing_to_care', side: 'emergency', text: 'hurrying to care' },
  { id: 'alternatives_available', side: 'no_emergency', text: 'another way was at hand' },
  { id: 'no_injury', side: 'no_emergency', text: 'no one was hurt' },
  { id: 'child_injured', side: 'emergency', text: 'a child was hurt' },
];
const PROBLEM = {
  questions: ['offence'], date: '2026-06-01', weights: W, tolerance: 0.5, size: 162,
  decided: [
    { id: 'lindqvist', name: 'Council v Lindqvist', court: 'avalon-high', date: '2018-09-03', question: 'prohibited', held: false, force: 'binding', weight: 1, facts: ['bicycle', 'in_park'] },
    { id: 'okafor', name: 'Council v Okafor', court: 'avalon-high', date: '2015-04-20', question: 'prohibited', held: true, force: 'binding', weight: 1, facts: ['motor_assisted', 'in_park'] },
    { id: 'tandem', name: 'Borough v <Tandem> Club', court: 'erewhon', date: '2020-06-11', question: 'prohibited', held: true, force: 'persuasive', weight: 0.3, facts: ['bicycle', 'in_park'] },
  ],
  held_back: [{ id: 'moreau', name: 'Council v Moreau', date: '2027-03-15', court: 'avalon-supreme' }, { id: 'emergency/jogger', name: 'jogger' }],
  bases: [{
    fact: 'emergency', sides: ['emergency', 'no_emergency'], factors: FACTORS, situation: ['rushing_to_care', 'child_injured', 'alternatives_available'],
    cases: [
      { name: 'doctor', facts: ['life_at_risk', 'rushing_to_care', 'alternatives_available'], outcome: 'emergency', stated: false, reason: [], for_outcome: ['life_at_risk', 'rushing_to_care'] },
      { name: 'commuter', facts: ['rushing_to_care', 'alternatives_available', 'no_injury'], outcome: 'no_emergency', stated: false, reason: [], for_outcome: ['alternatives_available', 'no_injury'] },
    ],
  }],
};
const assessed = (o) => ({
  admissible: true, conflicts: [], cases: [], bases: [], summary: '', ...o,
});
const NARROW = {
  serial: 3, kind: [0, 0], kind_words: 'offence does not follow; emergency held by the precedents', text: 'reading vehicle = narrow\nratio emergency/doctor: rushing_to_care\nratio emergency/commuter: alternatives_available\n',
  ratios: [{ fact: 'emergency', case: 'doctor', factors: ['rushing_to_care'], chosen: true }, { fact: 'emergency', case: 'commuter', factors: ['alternatives_available'], chosen: true }],
  assessment: assessed({
    score: 0.3 - 1.4, misfit: 0.3, justification: 1.4,
    cases: [{ id: 'okafor', force: 'binding', weight: 1, follows: true, agrees: true }, { id: 'lindqvist', force: 'binding', weight: 1, follows: false, agrees: true }, { id: 'tandem', force: 'persuasive', weight: 0.3, follows: false, agrees: false }],
    outcomes: [{ question: 'offence', outcome: 'does not follow', code: 0 }],
    bases: [{ fact: 'emergency', forced: 'emergency', held: true, why: 'forced by doctor' }],
  }),
  test: { outcome: 'upheld', cases: [{ id: 'moreau', verdict: 'predicted', reason: 'under its readings prohibited does not follow; the court held it did not' }, { id: 'emergency/jogger', verdict: 'open', reason: 'its ratios force neither side' }] },
};
const BROAD = {
  serial: 7, kind: [2, 2], kind_words: 'offence turns on the precedents; emergency left open by the precedents', text: 'reading vehicle = broad\nratio emergency/doctor: life_at_risk\nratio emergency/commuter: no_injury\n',
  ratios: [{ fact: 'emergency', case: 'doctor', factors: ['life_at_risk'], chosen: true }, { fact: 'emergency', case: 'commuter', factors: ['no_injury'], chosen: true }],
  assessment: assessed({
    score: 1 + 2 - 1, misfit: 1, justification: 1, conflicts: ['emergency: doctor and commuter'],
    cases: [{ id: 'okafor', force: 'binding', weight: 1, agrees: true }, { id: 'lindqvist', force: 'binding', weight: 1, agrees: false }, { id: 'tandem', force: 'persuasive', weight: 0.3, agrees: true }],
    outcomes: [{ question: 'offence', outcome: 'turns on the precedents', code: 2 }],
    bases: [{ fact: 'emergency', forced: '', held: null, why: '' }],
  }),
};
const REPORT = {
  problem: PROBLEM, kept: [NARROW, BROAD],
  exact: { enumerated: true, size: 162, best: [
    { kind: [0, 0], words: NARROW.kind_words, score: NARROW.assessment.score, text: NARROW.text, assessment: NARROW.assessment },
    { kind: [2, 2], words: BROAD.kind_words, score: 0.6, text: 'reading vehicle = broad\n', assessment: assessed({ score: 0.6, misfit: 1, justification: 0.4, outcomes: [{ question: 'offence', code: 2 }] }) },
    { kind: [1, 1], words: 'offence follows; emergency denied by the precedents', score: 0.2, text: 'reading vehicle = broad\n', assessment: assessed({ score: 0.2, misfit: 1.2, justification: 1, outcomes: [{ question: 'offence', code: 1 }] }) },
  ] },
};

const count = (s, re) => (s.match(re) || []).length;

export function doctrineChecks({ assert }) {
  const ok = [NARROW, BROAD].every((d) => Math.abs(E.partsOf(W, d.assessment).score - d.assessment.score) < 1e-12);
  assert('doctrine: a doctrine\'s score parts into what it does not fit (against authority, decided cases by force, inconsistent precedents) and what it is worth, which give the score back', ok,
    JSON.stringify([NARROW, BROAD].map((d) => [E.partsOf(W, d.assessment).score, d.assessment.score])));
  assert('doctrine: the score set out term by term sums to the score', Math.abs(E.scoreTerms(W, BROAD.assessment).reduce((s, t) => s + t.sum, 0) - BROAD.assessment.score) < 1e-12);
  assert('doctrine: an outcome is read from the engine\'s code: does not follow, follows, turns on the precedents',
    E.outcomeOf(NARROW.assessment, 'offence') === 'fails' && E.outcomeOf(BROAD.assessment, 'offence') === 'turns' && E.outcomeOf(REPORT.exact.best[2].assessment, 'offence') === 'holds');

  const F = E.fieldOf(REPORT);
  assert('doctrine: on the field, the best score is the least over the doctrines kept and the best of each kind there is; only those within the tolerance are acceptable',
    Math.abs(F.best - -1.1) < 1e-12 && Math.abs(F.threshold - -0.6) < 1e-12 && JSON.stringify(F.acceptable) === '[3]', JSON.stringify([F.best, F.threshold, F.acceptable]));
  assert('doctrine: each kind\'s exact best is marked found exactly when the search keeps a doctrine of that kind as good',
    F.exact.map((e) => e.found).join() === 'true,false,false', F.exact.map((e) => `${e.kind}:${e.found}`).join(' '));
  const region = E.acceptableRegion(F);
  const best = F.kept.find((k) => k.serial === 3), worst = F.kept.find((k) => k.serial === 7);
  const within = (p, poly) => { let c = false; for (let i = 0, j = poly.length - 1; i < poly.length; j = i++) { const [xi, yi] = poly[i], [xj, yj] = poly[j]; if ((yi > p[1]) !== (yj > p[1]) && p[0] < ((xj - xi) * (p[1] - yi)) / (yj - yi) + xi) c = !c; } return c; };
  assert('doctrine: the acceptable half-plane holds the best doctrine and not one beyond the tolerance, its corners on or above the line worth = misfit − threshold',
    region.length >= 3 && region.every(([x, y]) => y >= x - F.threshold - 1e-9) && within([best.unfit, best.worth], region) && !within([worst.unfit, worst.worth], region), JSON.stringify(region));
  const svg = E.svgField(F, { chosen: 7 });
  assert('doctrine: the field\'s markup has a mark for each doctrine kept, by outcome (a ring where it does not follow, a half disc where it turns), and a ring for each kind\'s best, the found one solid',
    count(svg, /class="df-dot /g) === 2 && svg.includes('o-fails acceptable') && svg.includes('df-half') && count(svg, /class="df-exact/g) === 3 && count(svg, /df-exact found/g) === 1 && count(svg, /df-halo/g) === 1);
  assert('doctrine: the field is named by its axes in nouns', svg.includes('Misfit (weighted by force)') && svg.includes('Worth (declared purposes)') && svg.includes('Tolerance 0.50'));

  const C = E.caseLawOf(REPORT, NARROW);
  assert('doctrine: the case law runs in order of date, the decision held back after the matter last, with its test\'s verdict',
    C.rows.map((r) => r.id).join() === 'okafor,lindqvist,tandem,moreau' && C.rows[3].held_back && C.rows[3].verdict === 'predicted' && C.agree === 2 && C.disagree === 1, JSON.stringify(C.rows.map((r) => [r.id, r.agrees, r.verdict])));
  const caselaw = E.svgCaseLaw(C, { chosen: 'tandem' });
  assert('doctrine: the case law\'s markup has a stone for each case by its force, the matter\'s date, the held-back decision veiled with its verdict, and names escaped',
    count(caselaw, /class="dl-case /g) === 4 && caselaw.includes('f-binding') && caselaw.includes('f-persuasive') && caselaw.includes('held-back') && caselaw.includes('The matter, 2026-06-01')
      && caselaw.includes('dl-mark v-predicted') && caselaw.includes('&lt;Tandem&gt;') && !caselaw.includes('<Tandem>') && caselaw.includes('dl-case f-persuasive on'));
  const years = [...caselaw.matchAll(/class="dl-year"[^>]*>(\d{4})</g)].map((m) => Number(m[1]));
  assert('doctrine: the line of years runs from a year before the first case to a year after the last decision', years.length > 2 && years[0] >= 2014 && years[0] <= 2015 && years[years.length - 1] >= 2027 && years[years.length - 1] <= 2028, years.join());

  const B = E.boardOf(REPORT, NARROW, 0);
  assert('doctrine: the board lists the factors for the first side before those for the second, and reads each precedent\'s reason from the doctrine',
    B.factors.map((f) => f.side).join() === 'emergency,emergency,emergency,no_emergency,no_emergency' && B.rows[0].reason.join() === 'rushing_to_care' && B.rows[1].reason.join() === 'alternatives_available' && B.rows.every((r) => r.chosen));
  assert('doctrine: under the doctrine the precedents force the matter\'s side, and the precedent held back carries its test\'s verdict',
    B.matter.forced === 'emergency' && B.matter.why === 'forced by doctor' && B.held.length === 1 && B.held[0].name === 'jogger' && B.held[0].verdict === 'open');
  const board = E.svgBoard(B);
  const pips = B.rows.reduce((s, r) => s + r.facts.length, 0) + B.matter.facts.length;
  assert('doctrine: the board\'s markup has a pip for each factor a case or the matter has, a ring for each reason read, the forced side and the veiled precedent held back',
    count(board, /class="db-pip /g) === pips && count(board, /class="db-reason"/g) === 2 && board.includes('forced: emergency') && board.includes('db-veil') && board.includes('· left open'), `${count(board, /class="db-pip /g)} pips of ${pips}`);
  const open = E.svgBoard(E.boardOf(REPORT, BROAD, 0));
  assert('doctrine: where the precedents leave the matter open, the board says so in a word', open.includes('◆ left open') && !open.includes('forced:'));
}
