// The partnership (ADR 0132): its ledger read on the date. Above, whether it benefits every party, by the standard the record declares,
// said party by party and never as one number. Then its commitments, each with its screen (SMART unless the record names fewer
// criteria), its progress and its state; a commitment opened shows each criterion's reason, what was reported, and lets a report or an
// amendment be recorded, the commitment as first made never overwritten. Then each party's benefit by its own criteria, read under its
// own words with each reading's source; a party opened shows its values with what the evidence allows of them, and lets a criterion or a
// value be recorded. Beneath, what was given, by party, kind and unit, never summed across units; and the signs of failure the record
// shows, each a sign and not a finding.

import { svgCommitments, svgBenefits, figure } from '/visualization/js/multilateral/index.js';
import { S, ensure, keep, begun, name, names, P, SIGNS, statusOf, closeKey, form, again } from './common.js';

export function partnershipView() {
  return { id: 'partnership', label: 'Partnership', draw };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('partnership-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the partnership');
  const redraw = again(ctx, 'partnership');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin on the round: a partnership is read once it has members.'));
  if (S.wrong) return fill(pane, el('p', { class: 'note wrong' }, S.wrong));
  const r = S.record, p = r.partnership, a = P();
  if (!a) return fill(pane, el('p', { class: 'note' }, 'The record has no partnership yet: add its members below.'), membersForm(ctx, redraw));
  const nm = names();
  const st = r.standard || {};
  ctx.note(`${p.members.length} members · ${p.commitments.length} commitments · ${S.at ? `read on ${S.at}` : 'read as it stands'}`);

  // ---- mutual benefit, by the declared standard ---------------------------------------------------------------------------------
  const setStandard = (f, note) => keep(ctx, (x) => { x.standard = { ...(x.standard || {}) }; f(x.standard); }, 'declared the standard of benefit', note).then(redraw);
  const kind = el('select', { class: 'mp-pick', 'aria-label': 'What the standard asks', onchange: (e) => setStandard((s) => { s.kind = e.target.value; }, e.target.value) },
    [['standing', 'a standing'], ['probability', 'a probability'], ['both', 'a standing and a probability']].map(([v, w]) => el('option', { value: v, selected: (st.kind || 'standing') === v }, w)));
  const least = el('select', { class: 'mp-pick', 'aria-label': 'The least standing', onchange: (e) => setStandard((s) => { s.least = e.target.value; }, `at least ${e.target.value}`) },
    [['clear', 'clear'], ['arguable', 'at least arguable']].map(([v, w]) => el('option', { value: v, selected: (st.least || 'clear') === v }, w)));
  const thr = el('input', { type: 'number', min: 0, max: 1, step: 0.05, value: st.threshold ?? 0.5, class: 'mp-num', 'aria-label': 'The probability asked',
    onchange: (e) => setStandard((s) => { s.threshold = Number(e.target.value); }, `a probability above ${e.target.value}`) });
  const mv = a.mutual?.verdict || {};
  const ms = statusOf(mv);
  const head = el('header', { class: 'mp-head' },
    el('p', { class: 'sentence' }, el('i', {}, p.name || r.title), ' benefits every party, judged by ', kind, ...(st.kind !== 'probability' ? [' (', least, ')'] : []), ...(st.kind !== 'standing' ? [' above ', thr] : []), '?'),
    el('p', { class: `answer ${mv.status || ''}` }, el('b', {}, `${ms.glyph} ${mv.status === 'holds' ? 'It does' : mv.status === 'fails' ? 'It does not' : 'Not decided'}`),
      mv.reason ? ` · ${mv.reason.replace(/^(\S+):/, (m, id) => `${nm[id] || id}:`)}` : ''),
    el('p', { class: 'small' }, 'Each party is judged by its own criteria under its own reading; the partnership benefits every party only when each one\'s benefit holds. No sum is taken.'));

  // ---- commitments ------------------------------------------------------------------------------------------------------------------
  const chosenC = S.chosen?.kind === 'commitment' ? S.chosen.id : null;
  const cfig = el('div', { class: 'mp-fig' });
  cfig.innerHTML = svgCommitments(a, { names: nm, selected: chosenC, width: Math.max(620, Math.min(1000, pane.clientWidth - 48)) });
  cfig.addEventListener('click', (e) => {
    const row = e.target.closest('[data-commitment]');
    if (!row) return;
    S.chosen = chosenC === row.dataset.commitment ? null : { kind: 'commitment', id: row.dataset.commitment };
    redraw();
  });
  const ccard = chosenC ? commitmentCard(ctx, chosenC, redraw) : null;

  // ---- benefits, party by party -------------------------------------------------------------------------------------------------
  const chosenP = S.chosen?.kind === 'party' || S.chosen?.kind === 'participant' ? S.chosen.id : null;
  const bfig = el('div', { class: 'mp-fig' });
  bfig.innerHTML = svgBenefits(a, { names: nm, selected: chosenP, width: Math.max(620, Math.min(1000, pane.clientWidth - 48)) });
  bfig.addEventListener('click', (e) => {
    const party = e.target.closest('[data-party]');
    if (!party) return;
    S.chosen = chosenP === party.dataset.party ? null : { kind: 'party', id: party.dataset.party };
    redraw();
  });
  const pcard = chosenP && p.members.some((m) => m.party === chosenP) ? partyCard(ctx, chosenP, redraw) : null;

  // ---- contributions and signs ------------------------------------------------------------------------------------------------
  const given = el('table', { class: 'mp-table' }, el('thead', {}, el('tr', {}, ['party', 'kind', 'amount', 'unit'].map((h) => el('th', {}, h)))),
    el('tbody', {}, (a.contributions || []).map((g) => el('tr', {}, el('td', {}, nm[g.party] || g.party), el('td', {}, g.kind), el('td', { class: 'n' }, figure(g.amount)), el('td', {}, g.unit)))));
  const byMode = {};
  for (const s of a.signs || []) (byMode[s.mode] ||= []).push(s);
  const signs = el('section', { class: 'mp-signs' }, el('h4', { class: 'mp-sub' }, 'Signs of failure the record shows'),
    el('p', { class: 'small' }, 'Each is a sign read from the record by a declared threshold, not a finding: whether it predicts failure is a question for a comparison of cases.'),
    Object.keys(byMode).length ? Object.entries(byMode).map(([mode, list]) => el('div', { class: `sign ${mode.replace(/ /g, '-')}` }, el('b', {}, mode), el('span', { class: 'small' }, ` · ${SIGNS[mode] || ''}`),
      el('ul', {}, list.map((s) => el('li', {}, `${s.where.replace(/^commitment (\S+)$/, (m, id) => `the commitment “${(a.states || []).find((x) => x.commitment === id)?.text || id}”`)}: ${s.what.replace(/\b(\w[\w-]*) (gives|holds|set) /g, (m, id, verb) => `${nm[id] || id} ${verb} `)}`)))))
      : el('p', {}, 'None at this date.'));
  const notes = (p.notes || []).length ? el('section', {}, el('h4', { class: 'mp-sub' }, 'What the record leaves out'), p.notes.map((n) => el('p', { class: 'small' }, n))) : null;

  fill(pane, head,
    el('h4', { class: 'mp-sub' }, 'Commitments'), cfig, ccard,
    el('h4', { class: 'mp-sub' }, 'Each party\'s benefit'), bfig, pcard,
    el('div', { class: 'mp-two' }, el('section', {}, el('h4', { class: 'mp-sub' }, 'What was given'), given), signs),
    notes, el('details', { class: 'mp-add' }, el('summary', {}, 'Add a member, a goal or a commitment'), membersForm(ctx, redraw)));
}

function commitmentCard(ctx, id, redraw) {
  const { el } = ctx;
  const p = S.record.partnership, a = P();
  const ci = p.commitments.findIndex((c) => c.id === id);
  const c = p.commitments[ci], s = (a.states || []).find((x) => x.commitment === id) || {}, sc = (a.screens || []).find((x) => x.commitment === id);
  if (!c) return null;
  const reports = (p.reports || []).filter((r) => r.commitment === id).sort((x, y) => x.when.localeCompare(y.when));
  return el('section', { class: 'mp-card wide' },
    el('header', {}, el('h3', {}, el('span', {}, c.text || id), closeKey(el, () => { S.chosen = null; redraw(); })),
      el('p', { class: 'small' }, `held by ${name(s.by || c.by)} · made ${c.made || '—'} · due ${s.due || '—'} · ${s.state}`)),
    el('div', { class: 'mp-two' },
      el('section', {}, el('h4', {}, 'Its screen'), (sc?.checks || []).map((k) => el('p', { class: `crit ${k.verdict.status}` }, el('b', {}, `${statusOf(k.verdict).glyph} ${k.name}. `), k.verdict.reason))),
      el('section', {}, el('h4', {}, 'What was reported'),
        reports.length ? reports.map((r) => el('p', {}, `${r.when}: ${figure(r.value)}${c.indicator?.unit ? ` ${c.indicator.unit}` : ''}${r.source ? ` (${r.source})` : ''}`)) : el('p', { class: 'small' }, 'Nothing is reported: the commitment is unreported, not failed.'),
        (c.amendments || []).length ? [el('h4', {}, 'Amendments'), c.amendments.map((m) => el('p', { class: 'small' }, `${m.when}: ${[Number.isFinite(m.target) ? `target ${figure(m.target)}` : '', m.due ? `due ${m.due}` : '', m.transferred_to ? `to ${name(m.transferred_to)}` : ''].filter(Boolean).join(', ')}${m.reason ? ` · ${m.reason}` : ''}`))] : null)),
    el('div', { class: 'mp-two' },
      el('details', { class: 'mp-add' }, el('summary', {}, 'Record a report'),
        form(el, [{ key: 'when', label: 'at', type: 'date', value: S.at }, { key: 'value', label: `value${c.indicator?.unit ? ` (${c.indicator.unit})` : ''}`, type: 'number' }, { key: 'source', label: 'source' }], 'Record it', async (v) => {
          if (!v.when || v.value === null) return ctx.say('Give the date and the value reported.');
          await keep(ctx, (x) => x.partnership.reports.push({ commitment: id, when: v.when, by: c.by, source: v.source, value: v.value }), 'recorded a report', `${c.text || id}: ${v.value} on ${v.when}`);
          redraw();
        })),
      el('details', { class: 'mp-add' }, el('summary', {}, 'Record an amendment'),
        el('p', { class: 'small' }, 'The commitment as first made is kept; an amendment counts from its date and never reaches back.'),
        form(el, [{ key: 'when', label: 'on', type: 'date', value: S.at }, { key: 'due', label: 'a new date', type: 'date' }, { key: 'target', label: 'a new target', type: 'number' },
          { key: 'to', label: 'taken over by', options: [['', 'no one'], ...p.members.map((m) => [m.party, name(m.party)])] }, { key: 'reason', label: 'why' }], 'Record it', async (v) => {
          if (!v.when) return ctx.say('Give the date of the amendment.');
          await keep(ctx, (x) => { const k = x.partnership.commitments[ci]; (k.amendments ||= []).push({ when: v.when, by: S.record.partnership.commitments[ci].by, reason: v.reason, target: v.target, due: v.due, transferred_to: v.to }); },
            'amended a commitment', `${c.text || id}${v.reason ? `: ${v.reason}` : ''}`);
          redraw();
        }))));
}

function partyCard(ctx, id, redraw) {
  const { el } = ctx;
  const p = S.record.partnership, a = P();
  const b = (a.benefits || []).find((x) => x.party === id) || {};
  const vs = (a.values || {})[id] || [];
  return el('section', { class: 'mp-card wide' },
    el('header', {}, el('h3', {}, el('span', {}, name(id)), closeKey(el, () => { S.chosen = null; redraw(); })),
      el('p', { class: 'small' }, `${(p.members.find((m) => m.party === id) || {}).role || 'a member'} · its benefit ${b.standing || 'undecided'} · ${b.measure?.words || ''}`)),
    el('div', { class: 'mp-two' },
      el('section', {}, el('h4', {}, 'Its criteria, in its own words'),
        (b.interests || []).length ? b.interests.map((i) => el('p', {}, el('b', {}, `${i.dimension}: `), `asks ${figure(i.threshold)}`, i.reading ? `, read as “${i.reading}”` : '', i.source ? el('span', { class: 'small' }, ` · ${i.source}`) : null))
          : el('p', { class: 'small' }, 'It has declared no criterion of its benefit, so whether it benefits is not decided.'),
        b.unobserved?.length ? el('p', { class: 'small' }, `Not observed: ${b.unobserved.join(', ')}. A value not observed is not low: the benefit is undecided until it is.`) : null),
      el('section', {}, el('h4', {}, 'What the evidence allows of its values'),
        vs.length ? vs.map((v) => el('p', {}, el('b', {}, `${v.dimension}: `), `${figure(v.value)}${Number.isFinite(v.lower) && v.lower !== v.upper ? ` [${figure(v.lower)}, ${figure(v.upper)}]` : ''}${Number.isFinite(v.error) ? ` ± ${figure(v.error)}` : ''}`,
          v.reading ? `, ${v.reading}` : '', v.source ? el('span', { class: 'small' }, ` · ${v.source}`) : null)) : el('p', { class: 'small' }, 'No value is recorded for it.'))),
    el('div', { class: 'mp-two' },
      el('details', { class: 'mp-add' }, el('summary', {}, 'Declare a criterion of its benefit'),
        form(el, [{ key: 'dimension', label: 'on', options: (p.dimensions || []).map((d) => [d.name, `${d.name} (${d.scale})`]) }, { key: 'threshold', label: 'asks', type: 'number' },
          { key: 'reading', label: 'read as', placeholder: 'its own words' }, { key: 'source', label: 'whose words', placeholder: 'a resolution, a strategy, a statement' }], 'Declare', async (v) => {
          if (!v.dimension || v.threshold === null) return ctx.say('Choose a dimension and what it asks.');
          await keep(ctx, (x) => x.partnership.interests.push({ party: id, dimension: v.dimension, reading: v.reading, source: v.source, threshold: v.threshold }), 'declared a criterion of benefit', `${name(id)}, ${v.dimension}`);
          redraw();
        })),
      el('details', { class: 'mp-add' }, el('summary', {}, 'Record a value'),
        form(el, [{ key: 'dimension', label: 'on', options: (p.dimensions || []).map((d) => [d.name, d.name]) }, { key: 'value', label: 'value', type: 'number' },
          { key: 'lower', label: 'at least', type: 'number' }, { key: 'upper', label: 'at most', type: 'number' }, { key: 'error', label: 'standard error', type: 'number' },
          { key: 'when', label: 'at', type: 'date', value: S.at }, { key: 'reading', label: 'read as' }, { key: 'source', label: 'source' }], 'Record it', async (v) => {
          if (!v.dimension || v.value === null) return ctx.say('Choose a dimension and give the value.');
          await keep(ctx, (x) => x.partnership.values.push({ party: id, dimension: v.dimension, when: v.when, reading: v.reading, source: v.source, value: v.value, lower: v.lower, upper: v.upper, error: v.error, grade: 'A' }),
            'recorded a value', `${name(id)}, ${v.dimension}: ${v.value}`);
          redraw();
        }))));
}

function membersForm(ctx, redraw) {
  const { el } = ctx;
  const u = S.record.undertaking, p = S.record.partnership;
  return el('div', { class: 'mp-two' },
    form(el, [{ key: 'party', label: 'a member', options: u.participants.filter((x) => !p.members.some((m) => m.party === x.id)).map((x) => [x.id, x.name || x.id]) },
      { key: 'role', label: 'its role' }, { key: 'since', label: 'since', type: 'date' }], 'Add the member', async (v) => {
      if (!v.party) return ctx.say('Every participant is already a member.');
      await keep(ctx, (x) => x.partnership.members.push({ party: v.party, role: v.role, since: v.since, until: '' }), 'added a member', name(v.party));
      redraw();
    }),
    form(el, [{ key: 'text', label: 'a commitment' }, { key: 'by', label: 'held by', options: p.members.map((m) => [m.party, name(m.party)]) },
      { key: 'goal', label: 'serving', options: (p.goals || []).map((g) => [g.id, g.text]) }, { key: 'due', label: 'due', type: 'date' },
      { key: 'indicator', label: 'measured by' }, { key: 'unit', label: 'in' }, { key: 'baseline', label: 'from', type: 'number' }, { key: 'target', label: 'to', type: 'number' }], 'Record the commitment', async (v) => {
      if (!v.text || !v.by) return ctx.say('Say what is committed and who holds it.');
      const id = `c${p.commitments.length + 1}`;
      await keep(ctx, (x) => x.partnership.commitments.push({ id, by: v.by, text: v.text, goal: v.goal, made: S.at || '', due: v.due, roles: {},
        indicator: { name: v.indicator, unit: v.unit, baseline: v.baseline, target: v.target, higher_is_better: true }, resources: [], amendments: [] }), 'recorded a commitment', v.text);
      redraw();
    }));
}
