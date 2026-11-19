// Governance (ADR 0132): how the undertaking decides, and where its arrangements fail by its own record. Each decision is a record with
// its authority, the evidence it rests on, those it affects and the venues where it may be contested, closed provisionally or finally
// (the monograph's Appendix D asks for no less); the diagnostics that name it stand on it. Then the undertaking's other diagnostics,
// grouped as the monograph's Table 10.2 names its failures, each with what it asks of the record. Then the rules its bodies decide by,
// each with its text, its source and the reading under which it is computed, and its a priori power member by member: a measure of
// the rule, never of influence.

import { svgPower } from '/visualization/js/multilateral/index.js';
import { S, ensure, keep, begun, base, name, U, DIAGNOSES, form, words, again } from './common.js';

export function governanceView() {
  return { id: 'governance', label: 'Governance', draw };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('governance-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('governance');
  const redraw = again(ctx, 'governance');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin on the round: decisions are taken among participants.'));
  if (S.wrong) return fill(pane, el('p', { class: 'note wrong' }, S.wrong));
  const r = S.record, u = r.undertaking, ans = U() || {};
  const diag = ans.diagnoses || [];
  ctx.note(`${u.decisions.length} decisions · ${diag.length} diagnostics · ${(r.rules || []).length} rules`);

  // ---- decisions --------------------------------------------------------------------------------------------------------------
  const onDecision = (d) => diag.filter((x) => x.where.startsWith(`decision ${d.id}`));
  const decisions = el('section', { class: 'mp-decisions' },
    u.decisions.length ? [...u.decisions].sort((a, b) => (a.when || '').localeCompare(b.when || '')).map((d) => {
      const own = onDecision(d);
      const future = S.at && d.when && d.when > S.at;
      return el('article', { class: `mp-decision${own.length ? ' faulted' : ''}${future ? ' later' : ''}${S.chosen?.kind === 'decision' && S.chosen.id === d.id ? ' chosen' : ''}`, 'data-decision': d.id },
        el('header', {}, el('b', {}, d.label || d.id), el('span', { class: 'small' }, ` · ${d.when || 'undated'} · ${d.closure === 'final' ? 'closed finally' : 'closed provisionally'}${future ? ' · after the date read' : ''}`)),
        el('dl', {},
          el('dt', {}, 'decided by'), el('dd', {}, (d.authorities || []).map(name).join(', ') || '—'),
          el('dt', {}, 'resting on'), el('dd', {}, (d.evidence || []).map(name).join(', ') || 'nothing named'),
          el('dt', {}, 'affecting'), el('dd', {}, (d.affected || []).map(name).join(', ') || '—'),
          el('dt', {}, 'contested at'), el('dd', {}, (d.venues || []).map(name).join(', ') || 'no venue named'),
          d.rule ? [el('dt', {}, 'by the rule'), el('dd', {}, d.rule)] : null,
          d.reason ? [el('dt', {}, 'because'), el('dd', {}, d.reason)] : null),
        own.length ? el('div', { class: 'mp-diag' }, own.map((x) => el('p', { class: `d ${x.kind.replace(/ /g, '-')}` }, el('b', {}, `${DIAGNOSES[x.kind]?.title || x.kind}. `), x.what)))
          : future ? null : el('p', { class: 'small ok' }, '✓ No diagnostic names it.'));
    }) : el('p', { class: 'note' }, 'No decision is recorded. A decision is recorded with who decided it, what it rests on, whom it affects and where it may be contested.'));
  const parts = u.participants.map((p) => [p.id, p.name || p.id]);
  const record = el('details', { class: 'mp-add' }, el('summary', {}, 'Record a decision'),
    form(el, [{ key: 'label', label: 'what was decided' }, { key: 'when', label: 'on', type: 'date', value: S.at },
      { key: 'authorities', label: 'decided by', many: parts }, { key: 'evidence', label: 'resting on', many: parts },
      { key: 'affected', label: 'affecting', many: parts }, { key: 'venues', label: 'contested at', many: parts },
      { key: 'rule', label: 'by the rule' }, { key: 'closure', label: 'closed', options: [['provisional', 'provisionally'], ['final', 'finally']] }, { key: 'reason', label: 'because' }], 'Record it', async (v) => {
      if (!v.label || !v.when) return ctx.say('Say what was decided, and when.');
      const id = `d${u.decisions.length + 1}`;
      await keep(ctx, (x) => x.undertaking.decisions.push({ id, label: v.label, when: v.when, rule: v.rule, closure: v.closure, authorities: v.authorities, evidence: v.evidence, affected: v.affected, venues: v.venues, reason: v.reason }),
        'recorded a decision', v.label);
      redraw();
    }));

  // ---- the other diagnostics -----------------------------------------------------------------------------------------------------
  const rest = diag.filter((x) => !x.where.startsWith('decision '));
  const groups = {};
  for (const x of rest) (groups[x.kind] ||= []).push(x);
  const diagnostics = el('section', { class: 'mp-groups' },
    Object.keys(DIAGNOSES).filter((k) => groups[k]).map((k) => el('div', { class: `group ${k.replace(/ /g, '-')}` },
      el('h4', {}, DIAGNOSES[k].title), el('p', { class: 'small' }, `The record would hold ${DIAGNOSES[k].asks}.`),
      el('ul', {}, groups[k].map((x) => el('li', {}, x.where.startsWith('type ') ? [el('b', {}, `${words(x.where.slice(5))}: `), x.what] : x.what))))),
    !rest.length ? el('p', { class: 'small ok' }, '✓ Beyond the decisions, nothing.') : null);

  // ---- the rules ---------------------------------------------------------------------------------------------------------------
  const rules = r.rules || [];
  const chips = el('div', { class: 'mp-chips' }, rules.map((x, i) => el('button', { type: 'button', class: `chip${i === S.rule ? ' on' : ''}`, onclick: () => { S.rule = i; redraw(); } }, x.title)));
  const ruleBox = el('section', { class: 'mp-rule' });
  if (rules.length) {
    const x = rules[Math.min(S.rule, rules.length - 1)];
    ruleBox.append(el('p', { class: 'said' }, el('b', {}, x.title), x.source ? ` · ${x.source}` : ''), x.text ? el('blockquote', {}, x.text) : null, x.reading ? el('p', { class: 'small' }, `Read as: ${x.reading}`) : null);
    const figBox = el('div', { class: 'mp-fig' }, el('p', { class: 'small' }, 'Computing its power…'));
    ruleBox.append(figBox);
    ctx.api(`${base(ctx)}/rule`, { method: 'POST', body: { index: Math.min(S.rule, rules.length - 1) } }).then((a) => {
      figBox.innerHTML = svgPower(a.answer, { width: Math.max(560, Math.min(900, pane.clientWidth - 64)) });
      const pr = a.answer.properties;
      if (pr) figBox.append(el('p', { class: 'small' }, [pr.proper ? 'Proper: no two disjoint coalitions both win.' : 'Not proper: two disjoint coalitions can both win.', pr.strong ? ' Strong: of every coalition and its complement one wins.' : ' Not strong: a coalition and its complement can both lose, so the body can fail to act.', ` ${pr.minimal_winning} minimal winning coalitions.`].join('')));
    }).catch((e) => { figBox.textContent = e.message; });
  }
  const own = S.rules.filter((x) => !rules.some((y) => y.title === x.title));
  const addRule = own.length ? el('details', { class: 'mp-add' }, el('summary', {}, 'Add a rule the plug-ins bring'),
    el('div', { class: 'mp-chips' }, own.map((x) => el('button', { type: 'button', class: 'chip', onclick: async () => {
      await keep(ctx, (y) => { (y.rules ||= []).push(x); }, 'added a rule', x.title);
      S.rule = (S.record.rules || []).length - 1;
      redraw();
    } }, x.title)))) : null;
  const ownRule = el('details', { class: 'mp-add' }, el('summary', {}, 'Declare a weighted rule of one\'s own'),
    form(el, [{ key: 'title', label: 'the body' }, { key: 'members', label: 'members', placeholder: 'names, separated by commas' }, { key: 'weights', label: 'their votes', placeholder: 'numbers, in the same order' },
      { key: 'quota', label: 'needed to pass', type: 'number' }, { key: 'at_least', label: 'and at least … members', type: 'number' }, { key: 'source', label: 'source' }], 'Declare', async (v) => {
      const members = v.members.split(',').map((s) => s.trim()).filter(Boolean), weights = v.weights.split(',').map((s) => Number(s.trim()));
      if (!v.title || members.length < 2 || weights.length !== members.length || weights.some((w) => !Number.isFinite(w)) || !Number.isFinite(v.quota)) return ctx.say('Name the body, its members, one number of votes for each, and the votes needed.');
      const weighted = { weights, quota: v.quota };
      const rule = Number.isFinite(v.at_least) && v.at_least > 0 ? { all_of: [{ at_least: v.at_least }, weighted] } : weighted;
      await keep(ctx, (y) => { (y.rules ||= []).push({ title: v.title, source: v.source, text: '', reading: 'As declared.', members, rule }); }, 'declared a rule', v.title);
      S.rule = (S.record.rules || []).length - 1;
      redraw();
    }));

  fill(pane,
    el('header', { class: 'mp-head' }, el('p', { class: 'sentence' }, 'How ', el('i', {}, u.name || r.title || 'the undertaking'), ' decides, and where its own record says it fails'),
      el('p', { class: 'small' }, 'Every diagnostic is a property of the record under its declared types: evidence about the arrangement, not a verdict on it.')),
    el('div', { class: 'mp-two wide-left' }, el('section', {}, el('h4', { class: 'mp-sub' }, 'Decisions'), decisions, record), el('section', {}, el('h4', { class: 'mp-sub' }, 'The arrangement'), diagnostics)),
    el('h4', { class: 'mp-sub' }, 'The rules its bodies decide by'), rules.length ? chips : el('p', { class: 'small' }, 'No rule is declared.'), ruleBox, addRule, ownRule);
}

