// The decision (ADR 0122): adopt, reject, or study first. The three courses stand side by side, each with what it is worth in
// expectation, the greatest marked. Beneath them, what the decision turns on: the study worth running before deciding (its size, its
// cost, what it is expected to gain, drawn over every size considered), and the worth of a unit of the outcome, over whose bounds
// the decision holds or switches. The rule is expected worth, which looks at no threshold of significance: an effect whose interval
// barely leaves zero may be adopted, and a well-shown one may still be worth studying, when the stakes say so.

import { svgStudy, svgWorth, worthLine, figure } from '/visualization/js/policy/index.js';
import { S, ensure, keep, begun, money, statusOf, numberIn } from './common.js';

export function decisionView() {
  return { id: 'decision', label: 'Decision', draw };
}

const COURSE = {
  adopt: { title: 'Adopt', under: 'decide now, for it' },
  reject: { title: 'Reject', under: 'decide now, against it' },
  'study first': { title: 'Study first', under: 'run the study, then decide' },
};

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('decision-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the decision');
  const q = S.question, redraw = () => ctx.show('decision');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin with the theory: the decision is taken on an option appraised here.'));
  if (S.wrong) return fill(pane, el('p', { class: 'note wrong' }, S.wrong));
  const change = async (f, what, note) => { await keep(ctx, f, what, note); redraw(); };
  const d = S.appraisal?.decision, dec = q.decision || {}, units = q.units || {}, many = units.units || `${units.unit || 'unit'}s`;
  const setDecision = (f, what, note) => change((x) => { x.decision ||= {}; f(x.decision); }, what, note);
  const optionPick = el('select', { class: 'q-pick', 'aria-label': 'The option decided on', onchange: (e) => setDecision((y) => { y.option = e.target.value; }, 'took the decision on another option', e.target.value) },
    (q.options || []).map((o) => el('option', { value: o.name, selected: o.name === (d?.option || dec.option) }, o.name)));
  const head = el('header', { class: 'q-head' }, el('p', { class: 'sentence' }, 'Should ', optionPick, ' be adopted here, rejected, or studied first?'));
  if (!d || d.error) return fill(pane, head, el('p', { class: 'note wrong' }, (d?.error || 'No option is stated to decide on.').replace(/^policy::[a-z]+: /, '')), el('p', { class: 'small' }, 'State the options first: the decision is taken on one of them against doing nothing.'));
  const st = statusOf(d.verdict), chosen = new Set(d.chosen || []);
  ctx.note(`${d.option}: ${[...chosen].join(' or ')}`);

  const worths = [['adopt', d.adopt], ['reject', d.reject], ...(Number.isFinite(d.study) ? [['study first', d.study]] : [])];
  const courses = el('div', { class: 'courses' }, worths.map(([c, w]) => el('section', { class: `course${chosen.has(c) ? ' chosen' : ''}` },
    el('h3', {}, COURSE[c].title), el('p', { class: 'under' }, c === 'study first' ? `study ${d.n.toLocaleString('en-GB')} ${d.n === 1 ? units.unit || 'unit' : many}, then decide` : COURSE[c].under),
    el('p', { class: 'big' }, money(w)), el('p', { class: 'small' }, `expected worth${units.money ? `, ${units.money}` : ''}`),
    chosen.has(c) ? el('p', { class: 'tag' }, '✓ the greatest expected worth') : null)),
    Number.isFinite(d.study) ? null : el('section', { class: 'course none' }, el('h3', {}, 'Study first'), el('p', { class: 'under' }, 'no study is described'), el('p', { class: 'small' }, 'Describe a study beneath, and its worth is set beside deciding now.')));

  const a = d.appraised;
  const why = el('p', { class: 'said' }, `At the effect expected here (${figure(d.effect)} ± ${figure(d.se)} a ${units.unit || 'unit'} and year), its net present value is `, el('b', {}, money(a.mean)), ` ± ${money(a.sd)}. `,
    `Knowing the effect before deciding would be worth at most ${money(d.evpi)}: no study, however large, can be worth more.`);

  // The study that could be run.
  const s = dec.study || null;
  const field = (label, key, attrs, under) => el('label', {}, el('span', {}, label), numberIn(el, s?.[key], (v) => setDecision((y) => { (y.study ||= { unit_variance: 1, fixed_cost: 0, unit_cost: 0, most: 100000, delay: 1 })[key] = v ?? 0; }, 'restated the study that could be run', `${label}: ${v ?? 0}`), attrs), under ? el('span', { class: 'small' }, under) : null);
  const curve = el('div', { class: 'fig' });
  if (s) curve.innerHTML = svgStudy(d, { width: Math.max(420, Math.min(680, pane.clientWidth - 60)), units: many });
  const best = (d.curve || []).find((p) => p.n === d.n);
  const study = el('section', { class: 'study-box' }, el('h4', {}, 'The study that could be run first'),
    s ? [el('div', { class: 'fields' },
      field('variance, a unit studied', 'unit_variance', { min: 0 }, 'of the effect\'s estimate; a study of n units has this over n'),
      field('fixed cost', 'fixed_cost', { min: 0 }), field('cost, a unit', 'unit_cost', { min: 0 }),
      field('the largest considered', 'most', { min: 1, step: 1 }, many),
      field('the worth kept while waiting', 'delay', { min: 0, max: 1, step: 0.01 }, 'a factor: 1 when waiting costs nothing')),
      curve,
      el('p', { class: 'said' }, chosen.has('study first')
        ? [`A study of ${d.n.toLocaleString('en-GB')} ${many} is worth running: it costs ${money(best?.cost)}, and what it would teach is expected to improve the decision by ${money(d.evsi)}. `, `Studying first and then deciding is worth ${money(d.study)}, against ${money(Math.max(d.adopt, d.reject))} for deciding now.`]
        : [d.n > 1 ? `No study of the sizes considered is worth running first: the best, of ${d.n.toLocaleString('en-GB')} ${many}, leaves ${money(d.study)}, against ${money(Math.max(d.adopt, d.reject))} for deciding now. `
          : `No study of the sizes considered is worth running first: even the smallest leaves ${money(d.study)}, against ${money(Math.max(d.adopt, d.reject))} for deciding now. `,
          d.evsi > 0 ? `What it would teach is worth ${money(d.evsi)}, less than it costs in money and in waiting.` : 'What so small a study would teach could not change the decision, and waiting for it costs.']),
      el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key', onclick: () => setDecision((y) => { delete y.study; }, 'set the study aside', '') }, 'Consider no study'))]
      : [el('p', { class: 'small' }, 'No study is described: the choice is between adopting and rejecting now.'),
        el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key edged', onclick: () => setDecision((y) => { y.study = { unit_variance: 1, fixed_cost: 0, unit_cost: 1, most: 100000, delay: 1 }; }, 'described a study that could be run', 'its numbers are to be stated') }, 'Describe a study'))]);

  // The worth of a unit, and whether the decision holds over its bounds.
  const option = (q.options || []).find((o) => o.name === d.option) || {};
  const bounds = Array.isArray(dec.worth) && dec.worth.length === 2 ? dec.worth : null;
  const line = el('div', { class: 'fig' });
  line.innerHTML = svgWorth(a, option.value, bounds, { width: Math.max(420, Math.min(680, pane.clientWidth - 60)), label: `the worth of a unit of the outcome${units.money ? ` (${units.money})` : ''}` });
  const w = Number.isFinite(option.value) && option.value ? worthLine(a, option.value, bounds) : null;
  const setBound = (k, v) => setDecision((y) => { const b = Array.isArray(y.worth) && y.worth.length === 2 ? [...y.worth] : [option.value * 0.5, option.value * 1.5]; b[k] = v ?? b[k]; y.worth = [Math.min(b[0], b[1]), Math.max(b[0], b[1])]; }, 'restated the bounds on a unit\'s worth', `${k ? 'at most' : 'at least'} ${v}`);
  const worth = el('section', { class: 'worth-box' }, el('h4', {}, 'The worth of a unit of the outcome'),
    el('p', { class: 'sentence small-sentence' }, `A unit is taken to be worth ${figure(option.value)}; it is known only to lie between `,
      numberIn(el, bounds?.[0], (v) => setBound(0, v), { class: 'q-num wide', 'aria-label': 'The least a unit may be worth', placeholder: '—' }), ' and ',
      numberIn(el, bounds?.[1], (v) => setBound(1, v), { class: 'q-num wide', 'aria-label': 'The most a unit may be worth', placeholder: '—' }), '.'),
    line,
    el('p', { class: 'said' }, !bounds ? 'State bounds, and the decision is tested over them.'
      : d.robust ? ['The decision between adopting and rejecting ', el('b', {}, 'holds over the whole of the bounds'), `: the net present value keeps its sign from ${figure(bounds[0])} to ${figure(bounds[1])}.`]
        : ['The decision ', el('b', {}, 'turns on the worth of a unit'), w?.switching ? `: below ${figure(w.switching)} it is to reject, above it to adopt, and the bounds hold both. ` : ': the bounds hold both adopting and rejecting. ',
          `Judged by the worst case over the bounds (Γ-maximin), the choice is to ${d.maximin}.`]));

  fill(pane, head, courses,
    el('p', { class: `answer ${d.verdict.status}` }, el('b', {}, `${st.glyph} ${[...chosen].map((c) => COURSE[c]?.title || c).join(' or ')}`), ' has the greatest expected worth.'),
    why, study, worth,
    el('p', { class: 'small foot' }, 'The decision follows from what was declared: the theory and its evidence, the place, the costs, the rates and the worths. Change one, and it is worked again. It decides nothing for anyone.'));
}
