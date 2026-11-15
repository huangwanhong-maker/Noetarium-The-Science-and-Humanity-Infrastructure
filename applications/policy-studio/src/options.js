// The options (ADR 0122): what each would be worth here. An option is a ledger: the effect a unit of the population gains each
// year (the effect expected here, or one stated), what a unit of the outcome is worth, whom it reaches, and what it costs year by
// year. Its net present value is uncertain as its effect is, and is drawn as an interval; among several, the one of greatest
// expected value is chosen, with how often each would in fact be best and what it would be worth to know which. Every worth, cost
// and rate is a declaration of whoever asks.

import { svgIntervals, figure, readStream, writeStream } from '/visualization/js/policy/index.js';
import { S, ensure, keep, begun, money, percent, statusOf, numberIn } from './common.js';

export function optionsView() {
  return { id: 'options', label: 'Options', draw };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('options-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the options');
  const q = S.question, redraw = () => ctx.show('options');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin with the theory: an option takes the effect expected here.'));
  if (S.wrong) return fill(pane, el('p', { class: 'note wrong' }, S.wrong));
  const change = async (f, what, note) => { await keep(ctx, f, what, note); redraw(); };
  const o = S.appraisal?.options || {}, unit = S.appraisal?.unit, units = q.units || {}, many = units.units || `${units.unit || 'unit'}s`;
  const appraised = (name) => (o.appraised || []).find((a) => a.name === name);
  const best = (name) => { const k = (o.appraised || []).findIndex((a) => a.name === name); return k >= 0 ? o.best?.[k] : undefined; };
  const setOption = (k, f, what, note) => change((x) => { f(x.options[k]); }, what, note ?? x_name(q, k));
  ctx.note(o.chosen?.length ? `the greatest expected value: ${o.chosen.join(', ')}` : o.error || 'no comparison yet');

  const rate = typeof q.discount === 'number' ? q.discount : null;
  const terms = el('p', { class: 'sentence small-sentence' }, 'Costs and benefits are discounted at ',
    rate === null ? el('span', {}, 'a schedule of rates by bands of years') : numberIn(el, +(rate * 100).toFixed(4), (v) => change((x) => { x.discount = (v ?? 0) / 100; }, 'changed the discount rate', `${v ?? 0}% a year`), { class: 'q-num', min: 0, max: 30, 'aria-label': 'The discount rate, per cent a year' }),
    rate === null ? '' : '% a year. ', 'An option that takes the effect expected here reads it ',
    el('select', { class: 'q-pick', 'aria-label': 'How the effect here becomes the outcome\'s units', onchange: (e) => change((x) => { x.scale = { ...(x.scale || {}), kind: e.target.value }; }, 'changed how the effect here is read', e.target.value) },
      el('option', { value: 'as_given', selected: (q.scale?.kind || 'as_given') === 'as_given' }, 'as it stands'), el('option', { value: 'risk_ratio', selected: q.scale?.kind === 'risk_ratio' }, 'as a risk ratio on a baseline risk')),
    q.scale?.kind === 'risk_ratio' ? [' of ', numberIn(el, q.scale.baseline, (v) => change((x) => { x.scale.baseline = v ?? 0; }, 'changed the baseline risk', `${v ?? 0} a unit and year`), { class: 'q-num wide', min: 0, 'aria-label': 'The baseline risk, a unit and year' }), ' a unit and year'] : null, '.',
    unit ? el('span', { class: 'small' }, ` That is ${figure(unit.effect)} ± ${figure(unit.se)} ${units.outcome || 'of the outcome'} a ${units.unit || 'unit'} and year.`) : null);

  const cards = (q.options || []).map((x, k) => {
    const a = appraised(x.name), here = x.effect === 'here', p = best(x.name), chosen = (o.chosen || []).includes(x.name);
    const costs = el('input', { type: 'text', class: 'stream', value: writeStream(x.cost || []), 'aria-label': `The costs of ${x.name}, year by year`, onchange: (e) => {
      try { const v = readStream(e.target.value); setOption(k, (y) => { y.cost = v; }, 'restated an option\'s costs', `${x.name}: ${v.length} years`); } catch (error) { ctx.say(error.message); }
    } });
    const row = (label, control, under) => el('div', { class: 'ledger-row' }, el('span', { class: 'label' }, label), el('div', {}, control, under ? el('span', { class: 'small' }, under) : null));
    return el('article', { class: `option-card${chosen ? ' chosen' : ''}` },
      el('header', {}, el('input', { class: 'option-name', type: 'text', value: x.name || '', 'aria-label': 'The option\'s name', onchange: (e) => { const name = e.target.value.trim(); if (name) change((z) => { if (z.decision?.option === z.options[k].name) z.decision.option = name; z.options[k].name = name; }, 'renamed an option', name); } }),
        chosen ? el('span', { class: 'tag' }, '✓ the greatest expected value') : null),
      row('Its effect', el('span', { class: 'inline' },
        el('select', { 'aria-label': `Where the effect of ${x.name} comes from`, onchange: (e) => setOption(k, (y) => { if (e.target.value === 'here') { y.effect = 'here'; delete y.se; } else { y.effect = unit?.effect ?? 0; y.se = unit?.se ?? 0; } }, 'changed where an option\'s effect comes from', `${x.name}: ${e.target.value}`) },
          el('option', { value: 'here', selected: here }, 'the effect expected here'), el('option', { value: 'stated', selected: !here }, 'stated')),
        here ? null : [numberIn(el, x.effect, (v) => setOption(k, (y) => { y.effect = v ?? 0; }, 'stated an option\'s effect', `${x.name}: ${v ?? 0}`), { 'aria-label': `The effect of ${x.name}` }), ' ± ',
          numberIn(el, x.se, (v) => setOption(k, (y) => { y.se = v ?? 0; }, 'stated an effect\'s standard error', `${x.name}: ${v ?? 0}`), { min: 0, 'aria-label': `The standard error of the effect of ${x.name}` })]),
        `${units.outcome || 'the outcome'}, a ${units.unit || 'unit'} and year${here && unit ? `: ${figure(unit.effect)} ± ${figure(unit.se)}` : ''}`),
      row('A unit of the outcome is worth', numberIn(el, x.value, (v) => setOption(k, (y) => { y.value = v ?? 0; }, 'restated the worth of a unit', `${x.name}: ${v ?? 0}`), { 'aria-label': `The worth of a unit of the outcome, for ${x.name}` }), units.money || ''),
      row('It reaches', numberIn(el, x.population, (v) => setOption(k, (y) => { y.population = v ?? 0; }, 'restated whom an option reaches', `${x.name}: ${v ?? 0}`), { min: 0, 'aria-label': `The units ${x.name} reaches each year` }), `${many} each year, with benefits from year ${x.from ?? 1}`),
      row('It costs, year by year', costs, `from year 0; ${(x.cost || []).length} years in all, which is its horizon`),
      a ? el('p', { class: 'npv' }, 'Net present value ', el('b', {}, money(a.mean)), a.sd ? ` ± ${money(a.sd)}` : '', el('span', { class: 'small' }, a.costs || a.benefits ? ` — benefits ${money(a.benefits)} against costs ${money(a.costs)}${Number.isFinite(a.ratio) ? `, a ratio of ${figure(a.ratio)}` : ''}.` : ' — nothing is spent and nothing gained.'),
        Number.isFinite(a.switching) ? el('span', { class: 'small' }, ` It breaks even when a unit is worth ${figure(a.switching)}.`) : null,
        Number.isFinite(p) ? el('span', { class: 'small' }, ` Best on ${percent(p)} of the draws.`) : null) : null,
      el('div', { class: 'keys' },
        q.decision?.option === x.name ? el('span', { class: 'small' }, 'The decision is taken on this option.') : el('button', { type: 'button', class: 'key', onclick: () => change((z) => { (z.decision ||= {}).option = x.name; }, 'took the decision on another option', x.name) }, 'Decide on this one'),
        el('button', { type: 'button', class: 'key', onclick: () => change((z) => { z.options.splice(k, 1); if (z.decision?.option === x.name) delete z.decision.option; }, 'removed an option', x.name) }, 'Remove')));
  });
  const add = (statusQuo) => change((x) => {
    const names = new Set((x.options || []).map((y) => y.name)), years = (x.options || [])[0]?.cost?.length || 6, value = (x.options || [])[0]?.value ?? 1;
    let name = statusQuo ? 'The status quo' : 'An option';
    for (let k = 2; names.has(name); k++) name = `${statusQuo ? 'The status quo' : 'An option'} ${k}`;
    (x.options ||= []).push(statusQuo ? { name, effect: 0, se: 0, value, population: 0, cost: Array(years).fill(0), from: 1 } : { name, effect: 'here', value, population: 1000, cost: Array(years).fill(0), from: 1 });
    if (!statusQuo && !x.decision?.option) (x.decision ||= {}).option = name;
  }, 'added an option', statusQuo ? 'the status quo: no cost and no effect' : 'its ledger is to be filled');

  const fig = el('div', { class: 'fig' });
  fig.innerHTML = svgIntervals((o.appraised || []).map((a) => ({ label: a.name.length > 34 ? `${a.name.slice(0, 33)}…` : a.name, value: a.mean, lower: a.sd ? a.mean - 1.96 * a.sd : undefined, upper: a.sd ? a.mean + 1.96 * a.sd : undefined,
    kind: (o.chosen || []).includes(a.name) ? 'chosen' : '' })), { show: money, label: 'the options\' net present values', plotW: 300, numW: 250 });
  const st = o.verdict ? statusOf(o.verdict) : null;
  fill(pane, el('header', { class: 'q-head' }, el('h2', {}, 'What each option would be worth here'), terms),
    o.error ? el('p', { class: 'note wrong' }, o.error.replace(/^policy::options: /, '')) : null,
    el('div', { class: 'ledgers' }, cards,
      el('div', { class: 'option-card add' }, el('button', { type: 'button', class: 'key edged', onclick: () => add(false) }, '+ an option'),
        (q.options || []).some((x) => x.effect === 0 && !(x.cost || []).some((c) => c)) ? null : el('button', { type: 'button', class: 'key', onclick: () => add(true) }, '+ the status quo'))),
    o.verdict ? el('section', { class: 'compare' }, el('p', { class: `answer ${o.verdict.status}` }, el('b', {}, `${st.glyph} ${o.chosen.join(', ')}`), ' has the greatest expected net present value.',
      el('span', { class: 'small' }, ` Knowing beforehand which option is in fact best would be worth ${money(o.evpi)} (± ${money(o.evpi_error)}, from ${o.draws.toLocaleString('en-GB')} draws of the effects).`)), fig,
    el('p', { class: 'small' }, 'Each interval is the net present value at the effect\'s 95% bounds: it is normal because the benefits are linear in the effect.')) : o.note ? el('p', { class: 'note' }, `${o.note[0].toUpperCase()}${o.note.slice(1)}.`) : null);
}

const x_name = (q, k) => q.options?.[k]?.name || '';
