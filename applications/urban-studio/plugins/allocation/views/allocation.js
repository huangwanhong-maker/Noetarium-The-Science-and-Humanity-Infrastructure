// Allocation and justice (ADR 0136): the round as the page. The resource runs along a channel whose sluices are the declared stages,
// each letting water off to the plots it serves; the plots stand in the order the round reached them, and a plot opens where it stands
// with what each stage gave it and why, what is at stake for it among the undominated options, and the appeal its people may file.
// Above the channel, the case, the sum, the rule and the stages, each with the principle it serves and who declared it, and a model's
// proposed stages to take or set aside. Beneath it, the rounds the declared one could have been, ordered by dominance under the case's
// principles (never summed), with the pivots; the record of the round, sealed, with the rules of procedural justice as seals; and, for
// a participatory budget, the voters left with nothing in each neighbourhood under each rule.

import { svgChannel, svgOrder, svgStakes, svgRecord, svgBudget, amount } from '/visualization/js/allocation/index.js';

const enc = encodeURIComponent;
const S = { project: null, cases: [], study: {}, data: null, budget: null, open: null, proposals: null, articulated: null, wrong: '', busy: '', model: null };
const base = (ctx) => `/api/projects/${enc(ctx.name)}/allocation`;
const ROUNDS = ['england-iod2025', 'melbourne-areas'];
const DEFAULTS = { 'england-iod2025': { amount: 1e9, per_person: 80, rule: 'proportional' }, 'melbourne-areas': { amount: 2e6, per_person: 2000, rule: 'proportional' } };
const RULES = ['proportional', 'equal_awards', 'equal_losses', 'talmud', 'piniles', 'adjusted_proportional'];
const RULE_WORDS = { proportional: 'proportional', equal_awards: 'equal awards', equal_losses: 'equal losses', talmud: 'the Talmud rule', piniles: 'Piniles', adjusted_proportional: 'adjusted proportional' };
const now = () => new Date().toISOString().replace(/\.\d+Z$/, 'Z');

export function view() {
  return {
    id: 'allocation', label: 'Allocation', draw,
    heard: (e, ctx) => { if (/allocation/.test(e.what) && document.querySelector('.tab.on')?.dataset.tab === 'allocation') { S.project = null; ctx.show('allocation'); } },
  };
}

function settings() {
  const c = S.study.case || 'england-iod2025';
  return { case: c, ...(DEFAULTS[c] || {}), ...(S.study.settings || {}) };
}

async function ensure(ctx) {
  if (S.project === ctx.name) return;
  Object.assign(S, { project: ctx.name, data: null, budget: null, open: null, proposals: null, articulated: null, wrong: '' });
  const r = await ctx.api(base(ctx));
  S.cases = r.cases || [];
  S.study = r.study || {};
  S.model = r.model || null;
  // A project begun from a template names its case in its plan's steps (and in its own `allocation`, when kept).
  const planned = [...(ctx.project?.steps || []), ...(ctx.project?.plan || [])].find((s) => String(s.tool || '').startsWith('allocation/') && s.params?.case)?.params.case;
  if (!S.study.case) S.study.case = ctx.project?.allocation?.case || planned || 'england-iod2025';
  await compute(ctx);
}

async function compute(ctx) {
  const st = settings();
  try {
    if (ROUNDS.includes(st.case)) {
      S.busy = 'The round is run, judged against its alternatives, and its record replayed…';
      S.data = await ctx.api(`${base(ctx)}/compute`, { method: 'POST', body: { ...st, stages: S.study.stages || undefined, acts: S.study.acts || [] } });
      S.budget = null;
    } else {
      S.busy = 'The ballots are counted under every rule…';
      S.budget = await ctx.api(`${base(ctx)}/budget`, { method: 'POST', body: { case: st.case } });
      S.data = null;
    }
    S.wrong = '';
  } catch (e) {
    S.wrong = String(e.message || e);
  }
  S.busy = '';
}

async function keep(ctx) {
  try { await ctx.api(base(ctx), { method: 'PUT', body: { study: S.study } }); } catch { /* a page that may only look keeps nothing */ }
}

async function change(ctx, pane, f) {
  f();
  S.open = null;
  pane.dataset.busy = '1';
  await keep(ctx);
  await compute(ctx);
  await draw(pane, ctx);
  delete pane.dataset.busy;
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('al-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  try { await ensure(ctx); } catch (e) { return fill(pane, el('p', { class: 'wrong' }, String(e.message || e))); }
  ctx.here('allocation');
  const st = settings();
  fill(pane, head(ctx, pane, st), S.busy ? el('p', { class: 'note' }, S.busy) : null, S.wrong ? el('p', { class: 'wrong' }, S.wrong) : null,
    S.data ? roundView(ctx, pane) : null, S.budget ? budgetView(ctx) : null);
}

// ---- the head: case, sum, rule, stages --------------------------------------------------------------------------------------------

function head(ctx, pane, st) {
  const { el } = ctx;
  const caseKeys = el('div', { class: 'al-choose' }, S.cases.map((c) => el('button', { class: `al-case${c.name === st.case ? ' on' : ''}`, title: c.credit,
    onclick: () => change(ctx, pane, () => { S.study = { case: c.name }; }) }, c.title)));
  const out = [el('div', { class: 'al-title-row' }, el('h2', { class: 'al-title' }, 'Who first, what and how much'), el('span', { class: 'small' }, 'every amount, rule and parameter is declared; nothing here says what is just')), caseKeys];
  if (!ROUNDS.includes(st.case)) return el('div', { class: 'al-head' }, out);
  const num = (key, label, step) => el('label', { class: 'al-field' }, label, el('input', { type: 'number', value: st[key], step, onchange: (e) => change(ctx, pane, () => { S.study.settings = { ...(S.study.settings || {}), [key]: +e.target.value }; }) }));
  const rule = el('label', { class: 'al-field' }, 'claims rationed by',
    el('select', { onchange: (e) => change(ctx, pane, () => { S.study.settings = { ...(S.study.settings || {}), rule: e.target.value }; }) },
      RULES.map((r) => el('option', { value: r, selected: r === st.rule ? '' : null }, RULE_WORDS[r]))));
  out.push(el('div', { class: 'al-fields' }, num('amount', 'the sum', 1e5), num('per_person', 'declared a person in need', 10), st.case === 'england-iod2025' ? rule : null));
  const R = S.data?.round;
  if (R) {
    out.push(el('ol', { class: 'al-stages' }, R.stages.map((s, k) => el('li', { class: 'al-stage' },
      el('b', {}, `${k + 1}. ${s.kind}`), ` for ${s.principle || 'no principle named'}`, s.rule && ['floor', 'priority', 'claims'].includes(s.kind) ? `, by ${RULE_WORDS[s.rule] || s.rule}` : '',
      s.share !== undefined && s.share < 1 ? `, taking ${Math.round(s.share * 100)}% of what remains` : '', el('span', { class: 'small' }, ` · declared by ${s.source?.who || 'no one named'}`),
      S.study.stages ? el('button', { class: 'al-mini', title: 'remove this stage', onclick: () => change(ctx, pane, () => { S.study.stages = R.stages.filter((_, j) => j !== k); }) }, '×') : null))));
    out.push(proposalsView(ctx, pane, R));
  }
  return el('div', { class: 'al-head' }, out);
}

function proposalsView(ctx, pane, R) {
  const { el } = ctx;
  const box = el('div', { class: 'al-proposals' });
  const ask = el('form', { class: 'al-ask', onsubmit: async (e) => {
    e.preventDefault();
    const situation = e.target.elements.situation.value;
    S.proposals = { busy: true };
    draw(pane, ctx);
    try { S.proposals = await ctx.api(`${base(ctx)}/propose`, { method: 'POST', body: { ...settings(), situation } }); } catch (err) { S.proposals = { refused: String(err.message || err) }; }
    draw(pane, ctx);
  } }, el('input', { name: 'situation', placeholder: 'What is allocated, to whom, and why it is scarce: a model may propose stages (it proposes no numbers)', class: 'al-situation' }),
  el('button', { type: 'submit', class: 'al-key', disabled: S.model && !S.model.kept ? '' : null, title: S.model && !S.model.kept ? 'no key is kept for a model at this computer' : '' }, 'ask for stages'));
  box.append(ask);
  const P = S.proposals;
  if (P?.busy) box.append(el('p', { class: 'note' }, 'A model is asked…'));
  else if (P?.refused) box.append(el('p', { class: 'wrong' }, P.refused));
  else if (P?.stages) {
    box.append(el('p', { class: 'small' }, `Proposed by ${P.by}, grade C: nothing here is in the round until a person takes it.`));
    for (const s of P.stages) box.append(el('div', { class: 'al-proposal' }, el('b', {}, s.kind), ` for ${s.principle}, reading ${s.attribute}, by ${RULE_WORDS[s.rule] || s.rule}: `, el('i', {}, s.source.why), ' ',
      el('button', { class: 'al-key', onclick: () => change(ctx, pane, () => { const { grade, status, ...stage } = s; S.study.stages = [...(S.study.stages || R.stages), { ...stage, source: { ...stage.source, who: `${stage.source.who}, taken by ${ctx.me?.name || 'a person'}` } }]; S.proposals = null; }) }, 'take into the round')));
    if (P.refused?.length) box.append(el('p', { class: 'small' }, `Refused: ${P.refused.join('; ')}.`));
  }
  return box;
}

// ---- the round --------------------------------------------------------------------------------------------------------------------

function roundView(ctx, pane) {
  const { el } = ctx;
  const D = S.data, R = D.round, res = D.result;
  const channel = el('div', { class: 'al-channel' });
  channel.innerHTML = svgChannel(R, res, { selected: S.open, most: 40, width: Math.max(720, (pane.clientWidth || 1000) - 48) });
  channel.addEventListener('click', (e) => {
    const g = e.target.closest('[data-entity]');
    if (!g || g.classList.contains('gathered')) return;
    S.open = S.open === g.dataset.entity ? null : g.dataset.entity;
    draw(pane, ctx);
  });
  const parts = [el('p', { class: 'al-notes' }, res.notes.join(' ')), channel];
  if (S.open) parts.push(card(ctx, pane, S.open));
  const J = D.judgement;
  if (J && !J.refused) {
    const order = el('div', { class: 'al-order' });
    order.innerHTML = svgOrder(J);
    const stakes = el('div', { class: 'al-stakes' });
    if (J.undominated.length > 1) stakes.innerHTML = svgStakes(J, R, { most: 14 });
    else stakes.append(el('p', { class: 'note' }, 'Only one option is undominated under these principles, so nothing is at stake among the undominated: every other rule or order is worse under at least one principle and better under none.'));
    const cap = (t) => t.charAt(0).toUpperCase() + t.slice(1);
    parts.push(el('h3', {}, 'The rounds it could have been'), el('p', { class: 'small' }, `${J.said.map(cap).join('. ')}. Read per ${D.reading.per.replace(/_/g, ' ')}.`),
      el('div', { class: 'al-two' }, order, stakes), ...J.pivots.map((p) => el('p', { class: 'al-pivot' }, cap(p.pivot))));
  } else if (J?.refused) parts.push(el('p', { class: 'wrong' }, `The judgement was refused: ${J.refused}`));
  parts.push(recordView(ctx, pane));
  return el('div', { class: 'al-round' }, parts);
}

function card(ctx, pane, id) {
  const { el } = ctx;
  const D = S.data, R = D.round, e = D.result.entities.find((x) => x.id === id), ent = R.entities.find((x) => x.id === id);
  const stake = D.judgement?.stakes?.find((s) => s.entity === id);
  const rank = D.result.order.findIndex((o) => o.entity === id) + 1;
  const at = Object.entries(ent.at).map(([k, v]) => el('span', { class: 'al-at' }, `${k} ${typeof v === 'number' ? (Math.abs(v) < 1 ? v.toFixed(3) : Math.round(v).toLocaleString('en-GB')) : v}`));
  const appeal = el('form', { class: 'al-appeal', onsubmit: (ev) => {
    ev.preventDefault();
    const f = ev.target.elements;
    const acts = S.study.acts || [];
    acts.push({ act: 'appeal', when: now(), who: f.who.value || 'a person', entity: id, kind: 'datum', key: f.key.value, value: +f.value.value, reason: f.reason.value });
    change(ctx, pane, () => { S.study.acts = acts; });
  } }, el('b', {}, 'File an appeal on a datum: '), el('select', { name: 'key' }, Object.keys(ent.at).map((k) => el('option', { value: k }, k))),
  el('input', { name: 'value', type: 'number', step: 'any', placeholder: 'the value claimed', required: '' }), el('input', { name: 'reason', placeholder: 'why', required: '' }),
  el('input', { name: 'who', placeholder: 'who appeals' }), el('button', { type: 'submit', class: 'al-key' }, 'file'));
  return el('div', { class: 'al-card' },
    el('h3', {}, `${ent.name}`), el('p', { class: 'small' }, rank ? `reached ${rank}${rank === 1 ? 'st' : rank === 2 ? 'nd' : rank === 3 ? 'rd' : 'th'}` : ''),
    el('p', {}, e.excluded ? e.excluded : `receives ${amount(e.amount, R.unit)}`), el('ul', { class: 'al-reasons' }, e.reasons.map((r) => el('li', {}, r))),
    stake ? el('p', { class: 'small' }, `Among the undominated options it would receive from ${amount(stake.least, R.unit)} to ${amount(stake.most, R.unit)}.`) : null,
    el('div', { class: 'al-ats' }, at), appeal);
}


function recordView(ctx, pane) {
  const { el } = ctx;
  const acts = S.study.acts || [];
  const P = S.data.procedure;
  const add = (act) => change(ctx, pane, () => { S.study.acts = [...(S.study.acts || []), { when: now(), who: ctx.me?.name || 'the clerk', ...act }]; });
  const groups = [...new Set(S.data.round.entities.map((e) => e.group).filter(Boolean))];
  const keys = el('div', { class: 'al-acts' },
    el('button', { class: 'al-key', onclick: () => add({ act: 'declare' }) }, '§ publish the round'),
    groups.length ? el('button', { class: 'al-key', onclick: () => add({ act: 'consult', group: groups.find((g) => !acts.some((a) => a.act === 'consult' && a.group === g)) || groups[0], note: 'consulted on the round' }) }, '◎ consult a group') : null,
    el('button', { class: 'al-key', onclick: () => add({ act: 'allocate' }) }, '→ apply the round'),
    ...acts.map((a, k) => [a, k]).filter(([a]) => a.act === 'appeal').map(([a], j) => (acts.some((b) => b.act === 'review' && b.appeal === j) ? null :
      el('span', { class: 'al-review' }, `appeal ${j + 1} (${a.entity}: ${a.key} ${a.value}): `,
        el('button', { class: 'al-key', onclick: () => add({ act: 'review', appeal: j, upheld: true, reason: 'upheld on review' }) }, 'uphold'),
        el('button', { class: 'al-key', onclick: () => add({ act: 'review', appeal: j, upheld: false, reason: 'dismissed on review' }) }, 'dismiss')))),
    acts.length ? el('button', { class: 'al-mini', onclick: () => change(ctx, pane, () => { S.study.acts = []; }) }, 'begin the record again') : null);
  const out = [el('h3', {}, 'The record of the round'), keys];
  if (P?.refused) out.push(el('p', { class: 'wrong' }, P.refused));
  else if (P) {
    const rec = el('div', { class: 'al-record' });
    rec.innerHTML = svgRecord(P, { width: 980 });
    out.push(rec);
    for (const h of P.hearings || []) out.push(el('p', { class: 'small' }, `Appeal ${h.appeal + 1}, heard for everyone: ${h.hearing.said}.`));
  } else out.push(el('p', { class: 'note' }, 'Nothing is on the record yet: publish the round, consult, apply it; an appeal is filed from a plot.'));
  return el('div', { class: 'al-procedure' }, out);
}

// ---- a participatory budget -------------------------------------------------------------------------------------------------------

function budgetView(ctx) {
  const { el } = ctx;
  const B = S.budget;
  const chart = el('div', { class: 'al-budget' });
  chart.innerHTML = svgBudget(B.answer, { plotW: 420 });
  const rows = B.answer.rules.map((r) => el('tr', {}, el('td', {}, r.rule.replace(/_/g, ' ')), el('td', {}, String(r.funded.length)), el('td', {}, amount(r.spent, B.currency)),
    el('td', {}, `${(100 * r.share_with_nothing).toFixed(1)}%`), el('td', {}, r.mean_funded.toFixed(2))));
  return el('div', { class: 'al-round' }, el('p', { class: 'al-notes' }, B.said),
    el('table', { class: 'al-table' }, el('thead', {}, el('tr', {}, ['rule', 'projects', 'spent', 'voters with nothing', 'projects a voter'].map((h) => el('th', {}, h)))), el('tbody', {}, rows)),
    chart);
}
