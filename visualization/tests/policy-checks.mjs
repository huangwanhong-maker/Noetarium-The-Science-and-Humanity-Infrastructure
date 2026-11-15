// Checks of the policy drawings (visualization/js/policy), ADR 0122. On a theory of change as the API's policy engine answers one,
// with two paths and a claim that passes over a layer: the layout in layers from the intervention to the outcome, no two states
// overlapping, the long claim drawn around what stands in the layer it passes and not behind it, the width fitted to the room; the
// markup with every claim's standing told by words and pips as well as by its line, the strongest path heavier, its weakest link
// and a gap on every path named; a forest plot whose logarithms of ratios are told as ratios; numbers as they are read; the worth
// of a study over its size with its best size marked; the decision over the worth of a unit, switching where costs meet benefits;
// and a theory as a document: written back into the domain's form, changed by operations that leave the old one as it was, with a
// stream of yearly amounts read as a person writes it. Called by run-tests.mjs.

import { policy as P } from '../js/index.js';

// As the engine answers (api/cpp/science/policy.hpp), abridged: two paths, and attendance -> learning passing over a layer.
const LEVELS = ['very_low', 'low', 'moderate', 'high'];
const link = (from, to, rank, extra = {}) => ({ from, to, assumption: `${from} brings ${to}`, support: [], sign: 1, evidence: { given: rank >= 0, effect: 0.2, lower: 0.1, upper: 0.3, grade: LEVELS[Math.max(0, rank)] },
  rank, level: rank >= 0 ? LEVELS[rank] : null, why: rank >= 0 ? `${from} -> ${to} is shown at ${LEVELS[rank]}` : `${from} -> ${to} has no evidence`, ...extra });
const THEORY = {
  name: 'meals', levels: LEVELS,
  nodes: [
    { id: 'meals', kind: 'intervention', text: 'A free meal for every pupil, served before the first lesson' },
    { id: 'attendance', kind: 'outcome', text: 'Pupils attend more days' },
    { id: 'nutrition', kind: 'outcome', text: 'Pupils are better nourished' },
    { id: 'attention', kind: 'outcome', text: 'Pupils attend to their lessons' },
    { id: 'learning', kind: 'impact', text: 'Pupils learn more' },
  ],
  links: [link('meals', 'attendance', 3), link('attendance', 'learning', 2, { support: ['teachers are present'] }), link('meals', 'nutrition', 2), link('nutrition', 'attention', -1), link('attention', 'learning', 1, { sign: -1 })],
};
const ASSESSMENT = { paths: 2, strongest: { links: [0, 1], weakest: 'moderate', limiting: 1, support: ['teachers are present'] }, necessary: [], gaps: [] };
const inside = (x, y, r) => x > r.x && x < r.x + r.w && y > r.y && y < r.y + r.h;

export function policyChecks({ assert }) {
  // ---- the layout ----
  const L = P.layoutTheory(THEORY, ASSESSMENT);
  const at = (id) => L.at.get(id);
  assert('policy: the theory is laid out in layers from the intervention to the outcome: each state after the latest that leads to it',
    at('meals').layer === 0 && at('attendance').layer === 1 && at('nutrition').layer === 1 && at('attention').layer === 2 && at('learning').layer === 3
      && THEORY.links.every((l) => at(l.from).x + at(l.from).w < at(l.to).x), JSON.stringify([...L.at].map(([k, v]) => [k, v.layer])));
  const boxes = [...L.at.values()];
  assert('policy: no two states overlap, and each stands within the drawing',
    boxes.every((a, i) => boxes.every((b, j) => i === j || a.x + a.w <= b.x || b.x + b.w <= a.x || a.y + a.h <= b.y || b.y + b.h <= a.y)) && boxes.every((b) => b.x >= 0 && b.y >= 0 && b.x + b.w <= L.width && b.y + b.h <= L.height));
  const long = L.links.find((d) => d.index === 1);
  assert('policy: a claim that passes over a layer is given a place in it: its badge stands there, clear of every state, and its line runs level across it',
    !long.straight && long.mx > at('attention').x && long.mx < at('attention').x + at('attention').w && !boxes.some((b) => inside(long.mx, long.my, b)) && / L[\d.]+,[\d.]+ C/.test(long.path), JSON.stringify([long.mx, long.my, at('attention')]));
  assert('policy: every claim\'s line leaves the right of its cause and arrives at the left of its effect',
    L.links.every((d) => { const l = THEORY.links[d.index], a = at(l.from), b = at(l.to); return Math.abs(d.x1 - (a.x + a.w)) < 0.01 && Math.abs(d.x2 - b.x) < 0.01 && d.path.startsWith(`M${d.x1.toFixed(1)},${d.y1.toFixed(1)}`) && d.path.endsWith(`${d.x2.toFixed(1)},${d.y2.toFixed(1)}`); }));
  const badges = L.links.filter((d) => d.straight);
  assert('policy: two badges in one gap are set apart', badges.every((a, i) => badges.every((b, j) => i >= j || Math.round(a.mx) !== Math.round(b.mx) || Math.abs(a.my - b.my) >= 27)));
  assert('policy: the strongest path, its weakest link and what a path needs are marked on the claims', L.links.filter((d) => d.strongest).map((d) => d.index).join() === '0,1' && L.links.find((d) => d.limiting).index === 1 && L.links.filter((d) => !d.shown).map((d) => d.index).join() === '3');
  const fitted = P.layoutTheory(THEORY, ASSESSMENT, { room: 860 }), wide = P.layoutTheory(THEORY, ASSESSMENT, { room: 2400 });
  assert('policy: the states\' width is fitted to the room there is, within bounds: narrower in a narrow room, never wider than reads well',
    fitted.nodeW >= 150 && fitted.nodeW < L.nodeW && fitted.width <= L.width && wide.nodeW === 250, `${fitted.nodeW} ${L.nodeW} ${wide.nodeW}`);
  const lone = P.layoutTheory({ levels: LEVELS, nodes: [{ id: 'a', kind: 'intervention', text: 'A' }], links: [] });
  assert('policy: a theory of one state and no claim is laid out too', lone.at.get('a').layer === 0 && lone.links.length === 0 && lone.height > 0);

  // ---- the markup ----
  const svg = P.svgTheory(THEORY, ASSESSMENT, L, { selected: 'l:3', intervention: 'meals', outcome: 'learning' });
  assert('policy: a claim\'s standing is told by words and pips as well as by its line: the grade where it is shown, "no evidence" where it has none',
    /class="link shown strongest"/.test(svg) && /class="link unshown"/.test(svg) && svg.includes('●●●● </tspan> high') === false && /●●●●<\/tspan> high/.test(svg) && /○○○○<\/tspan> no evidence/.test(svg) && /●●○○<\/tspan> low/.test(svg));
  assert('policy: the weakest link of the strongest path is named in words on its badge', /class="badge shown limiting"[^>]*data-link="1"/.test(svg) && />the weakest link</.test(svg));
  assert('policy: what is chosen is marked, and the states asked from and about are told apart', /class="badge unshown chosen" data-link="3"/.test(svg) && /class="node intervention from" data-node="meals"/.test(svg) && /class="node impact to" data-node="learning"/.test(svg));
  const gap = P.svgTheory(THEORY, { ...ASSESSMENT, strongest: { links: [0, 1], weakest: 'moderate', limiting: 1 }, gaps: [3] });
  assert('policy: a gap that every path passes through is named on its badge', /class="badge unshown gap" data-link="3"/.test(gap) && />a gap on every path</.test(gap));
  assert('policy: the pips of a grade fill up to its level, and none when it is not shown', P.pips(1, 4) === '●●○○' && P.pips(3, 4) === '●●●●' && P.pips(-1, 4) === '○○○○' && P.pips(null, 3) === '○○○');
  assert('policy: a state\'s words are broken into lines that fit it', P.wrapText('A free meal for every pupil, served before the first lesson', 120).length >= 3 && P.wrapText('', 100).length === 0);
  const cut = P.layoutTheory({ levels: LEVELS, nodes: [{ id: 'a', kind: 'intervention', text: 'Community health workers visit every household with a newborn during the first week of life, to check for danger signs' }], links: [] }, null, { nodeW: 150 });
  assert('policy: words too long for a state are cut at four lines, and the cut is told by an ellipsis', cut.at.get('a').lines.length === 4 && cut.at.get('a').lines[3].endsWith('…'));
  assert('policy: text is escaped in the markup', !P.svgTheory({ ...THEORY, nodes: [{ id: 'meals', kind: 'x', text: 'a <b> & c' }, ...THEORY.nodes.slice(1)] }, ASSESSMENT).includes('<b>'));

  // ---- numbers as they are read ----
  assert('policy: a number is told in three figures, small ones written out and large ones grouped',
    P.figure(0.4893) === '0.489' && P.figure(0.000468321) === '0.000468' && P.figure(1516367.5) === '1,516,368' && P.figure(2102.1) === '2100' && P.figure(-0.7145) === '-0.715' && P.figure(NaN) === '—' && P.figure(0) === '0' && P.figure(3e-9) === '3.00e-9');
  const tk = P.ticks(-1.2, 0.6, 6);
  assert('policy: ticks are round values within the range, zero among them when it is', tk.includes(0) && tk.every((v) => v >= -1.2 - 1e-9 && v <= 0.6 + 1e-9) && tk.length <= 8 && tk.length >= 3, tk.join());

  // ---- the forest ----
  const BODY = {
    studies: [{ id: '1', label: 'North', year: 1950, stratum: 'a', effect: -0.9, lower: -2.0, upper: 0.2, weight: 0.2 }, { id: '2', label: 'South', year: 1960, stratum: 'b', effect: -0.2, lower: -0.5, upper: 0.1, weight: 0.8 }],
    pooled: { k: 2, estimate: -0.4, lower: -0.9, upper: 0.1, model: 'random effects (REML)' }, prediction: { lower: -1.6, upper: 0.8 },
  };
  const rows = P.forestRows(BODY);
  assert('policy: a forest has a row for each study, then the pooled effect and the interval predicted for a new setting', rows.map((r) => r.kind).join() === 'study,study,pooled,prediction' && rows[3].lower === -1.6);
  const forest = P.svgForest(BODY, { ratio: true, selected: '2' });
  assert('policy: logarithms of ratios are told as ratios, each number beside its mark: exp(−0.9) = 0.407 with its interval',
    forest.includes('0.407 [0.135, 1.22]') && forest.includes('0.67 [0.407, 1.11]') && /class="row study chosen" data-study="2"/.test(forest) && /<path class="diamond"/.test(forest) && /class="predicted"/.test(forest) && />ratio \[interval\]</.test(forest));
  const plain = P.svgForest(BODY);
  assert('policy: effects as they are given are told as they are, and a body with no study draws nothing', plain.includes('-0.9 [-2, 0.2]') && />effect \[interval\]</.test(plain) && P.svgForest({ studies: [] }) === '');
  const heavier = (s) => Number(s.match(/class="box"[^>]*width="([\d.]+)"/)[1]);
  assert('policy: a study\'s box grows with its weight', heavier(forest.split('data-study="2"')[1]) > heavier(forest.split('data-study="1"')[1].split('data-study="2"')[0]));

  // ---- intervals compared ----
  const iv = P.svgIntervals([{ label: 'There', value: -0.7, lower: -1.1, upper: -0.4 }, { label: 'Here', value: -0.4, lower: -0.7, upper: 0, kind: 'here' }, { label: 'None', value: NaN }], { show: (v) => P.figure(Math.exp(v)) });
  assert('policy: intervals are set against each other on one axis with a zero line, each with its number; what has no value is left out',
    (iv.match(/class="row/g) || []).length === 2 && /class="row here"/.test(iv) && /class="grid zero"/.test(iv) && iv.includes('0.497 [0.333, 0.67]'));

  // ---- the study, and the worth of a unit ----
  const curve = [1, 10, 100, 1000, 10000].map((n) => ({ n, worth: -40000 + (n === 1000 ? 120000 : n === 100 ? 20000 : 0), evsi: 0, cost: 40000 + 3 * n }));
  const study = P.svgStudy({ curve, adopt: -600000, reject: 0, study: 80000, n: 1000, chosen: ['study first'] }, { units: 'pupils' });
  assert('policy: the worth of studying first is drawn over the study\'s size, the best size marked and named, with deciding now across it',
    /<path class="worth"/.test(study) && /1,000 pupils: 80,000/.test(study) && /deciding now: 0/.test(study) && />pupils studied</.test(study));
  const none = P.svgStudy({ curve, adopt: 100, reject: 0, study: -39900, n: 1, chosen: ['adopt'] });
  assert('policy: where no study is worth more than its smallest, no size is marked as best; and a curve of one point draws nothing', !/class="best"/.test(none) && P.svgStudy({ curve: curve.slice(0, 1), adopt: 0, reject: 0 }) === '');
  const A = { benefits: 3200000, costs: 1680000, mean: 1520000 };
  const w = P.worthLine(A, 4000, [1500, 12000]);
  assert('policy: the net present value is linear in the worth of a unit: it switches where the costs meet the benefits, 1,680,000 ÷ 800 = 2,100',
    Math.abs(w.switching - 2100) < 1e-9 && Math.abs(w.at(4000) - A.mean) < 1e-6 && Math.abs(w.at(2100)) < 1e-6 && w.lo === 1500 && w.hi === 12000);
  const ws = P.svgWorth(A, 4000, [1500, 12000]);
  assert('policy: the switching worth and the worth stated are marked on the line', /it switches at 2100/.test(ws) && /as stated, 4000/.test(ws) && P.svgWorth(A, 0, null) === '');
  const cs = P.courses({ adopt: 5, reject: 0, study: 9, n: 40, chosen: ['study first'] });
  assert('policy: the three courses with their worths, the chosen marked; two where no study is described',
    cs.map((c) => `${c.course}:${c.worth}:${c.chosen}`).join() === 'adopt:5:false,reject:0:false,study first:9:true' && P.courses({ adopt: 1, reject: 0, study: NaN, chosen: ['adopt'] }).length === 2);

  // ---- a theory as a document ----
  const T = { name: 'Free school meals', levels: LEVELS, nodes: THEORY.nodes.slice(0, 2), links: [{ from: 'meals', to: 'attendance', assumption: ' families  send children ', support: ['the meal is served early', ' '], sign: -1, evidence: { given: true, effect: 0.12, lower: 0.05, upper: 0.19, grade: 'moderate' } }] };
  const text = P.writeTheory(T);
  assert('policy: a theory is written back in the domain\'s form, a line each, with its support factors, its evidence and the side it expects',
    text === 'theory free_school_meals\nlevels very_low low moderate high\nnode meals intervention: A free meal for every pupil, served before the first lesson\nnode attendance outcome: Pupils attend more days\n'
      + 'link meals -> attendance: families send children\nsupport meals -> attendance: the meal is served early\nevidence meals -> attendance: 0.12 [0.05, 0.19] moderate\nsign meals -> attendance: -\n', text);
  const added = P.addNode(T, { text: 'Pupils attend more days', kind: 'outcome', after: 'attendance' });
  assert('policy: a state added takes an identifier no other has, with the claim that leads to it; the theory it was added to is as it was',
    added.id === 'pupils_attend' && added.theory.nodes.length === 3 && added.theory.links.at(-1).from === 'attendance' && T.nodes.length === 2 && P.freshId({ nodes: [{ id: 'state' }, { id: 'state_2' }] }, '…') === 'state_3');
  const removed = P.removeNode(added.theory, 'attendance');
  assert('policy: a state removed takes the claims at it with it', removed.nodes.map((n) => n.id).join() === 'meals,pupils_attend' && removed.links.length === 0);
  assert('policy: a claim is stated once, and never from a state to itself', P.addLink(T, 'meals', 'attendance').links.length === 1 && P.addLink(T, 'meals', 'meals').links.length === 1 && P.addLink(added.theory, 'meals', added.id).links.length === 3);
  assert('policy: a claim and a state are changed in a copy', P.setLink(T, 0, { sign: 1 }).links[0].sign === 1 && T.links[0].sign === -1 && P.setNode(T, 'meals', { text: 'Meals', id: 'x' }).nodes[0].id === 'meals' && P.removeLink(T, 0).links.length === 0);
  const moved = P.setLevels(T, ['weak', 'fair', 'strong']);
  assert('policy: graded on other levels, evidence written in a level the new scale lacks is dropped, and said', moved.dropped.join() === 'meals -> attendance' && !moved.theory.links[0].evidence.given && T.links[0].evidence.given && P.setLevels(T, LEVELS).dropped.length === 0);
  assert('policy: a stream of yearly amounts is read as a person writes it and written back with its runs gathered',
    P.readStream('400000, 150000 × 10').length === 11 && P.readStream('1; 2 x 2\n3*1').join() === '1,2,2,3' && P.writeStream([400000, 150000, 150000, 0]) === '400000, 150000 × 2, 0' && P.writeStream(P.readStream('0 × 6')) === '0 × 6' && P.readStream('').length === 0);
  let refused = '';
  try { P.readStream('12, many'); } catch (error) { refused = error.message; }
  assert('policy: what is not an amount is refused, and named', /“many” is not an amount/.test(refused));
}
