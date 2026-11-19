// The round (ADR 0132; the design of 2026-11-19, §4): the undertaking drawn as itself. The participants sit on a ring with no head of the
// table, grouped by kind; the relations are threads, a layer for each declared type, each switched on and off at its key; the date the
// record is read on is chosen above, from every date the record holds. Whatever is chosen opens where it stands, in a card beside the
// round: a party with its relational position at the date (what authorises it, what it relies on, what it may contest, the decisions it
// holds, is affected by or hears, its constraints), the diagnostics that name it, and its place in the partnership; a tie with its
// source; a change on the strands beneath with its date. Read from a chosen seat, every other pair is marked present, absent or not
// observed: what the record does not observe is drawn as a gap with a mark, never as nothing.

import { svgRound, svgStrands, kindOf, KINDS } from '/visualization/js/multilateral/index.js';
import { S, ensure, keep, readAt, begun, base, name, U, P, dates, DIAGNOSES, closeKey, form, words, again } from './common.js';

export function roundView() {
  return { id: 'round', label: 'Round', draw };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('round-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the round');
  const redraw = again(ctx, 'round');
  if (!begun()) return fill(pane, beginning(ctx, redraw));
  if (S.wrong) return fill(pane, el('p', { class: 'note wrong' }, S.wrong));
  const r = S.record, u = r.undertaking, ans = U();
  ctx.note(`${u.participants.length} participants · ${u.ties.length} ties · ${S.at ? `read on ${S.at}` : 'every tie, whenever it held'}`);

  // ---- the date and the layers -------------------------------------------------------------------------------------------------
  const all = dates();
  const at = el('select', { class: 'mp-pick', 'aria-label': 'The date the record is read on', onchange: async (e) => { await readAt(ctx, e.target.value); redraw(); } },
    el('option', { value: '', selected: !S.at }, 'every tie, whenever it held'), all.map((d) => el('option', { value: d, selected: d === S.at }, d)));
  const slide = all.length > 1 ? el('input', { type: 'range', min: 0, max: all.length - 1, step: 1, value: Math.max(0, all.indexOf(S.at)), class: 'mp-slide', 'aria-label': 'Move through the record\'s dates',
    onchange: async (e) => { await readAt(ctx, all[+e.target.value]); redraw(); } }) : null;
  const shown = new Set(S.layers || u.types.map((t) => t.name));
  const head = el('header', { class: 'mp-head' },
    el('p', { class: 'sentence' }, el('i', {}, u.name || r.title || 'The undertaking'), ', read on ', at, slide),
    el('p', { class: 'small' }, 'Choose a seat to read the round from it; a thread or a knot opens where it stands. Each key switches its layer.'));

  // ---- the round, and the card beside it -------------------------------------------------------------------------------------------
  const fig = el('div', { class: 'mp-fig round' });
  fig.innerHTML = svgRound(u, { at: S.at, layers: [...shown], selected: S.chosen?.kind === 'participant' ? S.chosen.id : null, answer: ans, width: Math.max(520, Math.min(860, pane.clientWidth - 380)) });
  fig.addEventListener('click', (e) => {
    const seat = e.target.closest('[data-participant]'), thread = e.target.closest('.mr-thread'), key = e.target.closest('.mr-key');
    if (key) {
      const t = key.dataset.type;
      shown.has(t) ? shown.delete(t) : shown.add(t);
      S.layers = [...shown];
      return redraw();
    }
    if (thread) S.chosen = { kind: 'tie', type: thread.dataset.type, from: thread.dataset.from, to: thread.dataset.to };
    else if (seat) S.chosen = S.chosen?.kind === 'participant' && S.chosen.id === seat.dataset.participant ? null : { kind: 'participant', id: seat.dataset.participant };
    else return;
    redraw();
  });
  const card = el('aside', { class: 'mp-card' }, cardFor(ctx, redraw));

  // ---- the strands beneath --------------------------------------------------------------------------------------------------------
  const strands = el('div', { class: 'mp-fig strands' });
  strands.innerHTML = svgStrands(ans, { at: S.at, partnership: P(), record: r.partnership, width: Math.max(640, pane.clientWidth - 48) });
  strands.addEventListener('click', async (e) => {
    const k = e.target.closest('.ms-knot');
    if (!k) return;
    S.chosen = { kind: 'knot', date: k.dataset.date, what: k.dataset.what, change: k.dataset.kind };
    await readAt(ctx, k.dataset.date);
    redraw();
  });

  fill(pane, head, el('div', { class: 'mp-stage' }, fig, card), el('h4', { class: 'mp-sub' }, 'What changed, and when'), strands);
}

// ---- the card --------------------------------------------------------------------------------------------------------------------

function cardFor(ctx, redraw) {
  const c = S.chosen;
  if (c?.kind === 'participant') return participantCard(ctx, c.id, redraw);
  if (c?.kind === 'tie') return tieCard(ctx, c, redraw);
  if (c?.kind === 'knot') return knotCard(ctx, c, redraw);
  return summaryCard(ctx, redraw);
}

const list = (el, label, ids, after = '') => (ids?.length ? el('div', { class: 'mp-list' }, el('span', {}, label), el('p', {}, ids.map((i) => name(i)).join(', ') + after)) : null);

function participantCard(ctx, id, redraw) {
  const { el } = ctx;
  const u = S.record.undertaking, p = u.participants.find((x) => x.id === id);
  if (!p) return summaryCard(ctx, redraw);
  const pos = (U()?.positions || []).find((x) => x.participant === id) || {};
  const k = kindOf(p.kind);
  const diag = (U()?.diagnoses || []).filter((d) => d.where === `participant ${id}` || d.where.includes(` ${id} `) || d.what.includes(name(id)));
  const member = (S.record.partnership?.members || []).find((m) => m.party === id);
  const benefit = (P()?.benefits || []).find((b) => b.party === id);
  const commitments = (P()?.states || []).filter((s) => s.by === id);
  const given = (P()?.contributions || []).filter((g) => g.party === id);
  const typeLists = u.types.flatMap((t) => [list(el, `${words(t.name)} →`, pos.out?.[t.name]), list(el, `← ${words(t.name)}`, pos.in?.[t.name])]).filter(Boolean);
  const authority = pos.constituted ? 'holds authority by its own constitution' : pos.formal_authority ? `authorised by ${(pos.authorised_by || []).map(name).join(', ')}` : 'holds no authority recorded';
  const others = u.participants.filter((x) => x.id !== id);
  return [
    el('header', {}, el('h3', {}, el('span', {}, `${k.glyph} ${name(id)}`), closeKey(el, () => { S.chosen = null; redraw(); })),
      el('p', { class: 'small' }, `${k.word} · ${authority}${S.at ? ` · on ${S.at}` : ''}`)),
    typeLists.length ? el('section', {}, el('h4', {}, 'Its ties'), typeLists) : el('p', { class: 'small' }, 'No tie of it is recorded at this date.'),
    list(el, 'decides', pos.decides), list(el, 'affected by', pos.affected_by), list(el, 'a venue of', pos.venue_of), list(el, 'holds the constraints', pos.constraints),
    diag.length ? el('section', { class: 'mp-diag' }, el('h4', {}, 'What the record shows of it'),
      diag.map((d) => el('p', { class: `d ${d.kind.replace(/ /g, '-')}` }, el('b', {}, `${DIAGNOSES[d.kind]?.title || d.kind}. `), d.what))) : null,
    member || benefit || commitments.length ? el('section', {}, el('h4', {}, 'In the partnership'),
      member ? el('p', {}, `${member.role || 'a member'}${member.since ? ` since ${member.since}` : ''}${member.until ? `, until ${member.until}` : ''}`) : null,
      benefit ? el('p', {}, el('b', {}, `Its benefit: ${benefit.standing}`), ` · ${benefit.measure?.words || ''}`) : null,
      commitments.length ? el('p', {}, `It holds ${commitments.length} commitment${commitments.length > 1 ? 's' : ''}: ${commitments.map((s) => `${s.text || s.commitment} (${s.state})`).join('; ')}`) : null,
      given.length ? el('p', {}, `It gave ${given.map((g) => `${Math.round(g.amount).toLocaleString('en-GB')} ${g.unit} (${g.kind})`).join('; ')}`) : null,
      el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key edged', onclick: () => { S.chosen = { kind: 'party', id }; ctx.show('partnership'); } }, 'Open its ledger'))) : null,
    el('details', { class: 'mp-add' }, el('summary', {}, 'Record a tie of it'),
      form(el, [
        { key: 'type', label: 'type', options: u.types.map((t) => [t.name, words(t.name)]) },
        { key: 'dir', label: 'direction', options: [['out', `from ${name(id)}`], ['in', `to ${name(id)}`]] },
        { key: 'other', label: 'with', options: others.map((x) => [x.id, x.name || x.id]) },
        { key: 'since', label: 'since', type: 'date', value: S.at },
        { key: 'until', label: 'until', type: 'date' },
        { key: 'source', label: 'source', placeholder: 'where the record says so' },
        { key: 'absent', label: 'recorded absent', type: 'checkbox' },
      ], 'Record it', async (v) => {
        if (!v.type || !v.other) return ctx.say('Choose a type and the other party.');
        const [from, to] = v.dir === 'out' ? [id, v.other] : [v.other, id];
        await keep(ctx, (x) => x.undertaking.ties.push({ type: v.type, from, to, since: v.since, until: v.until, absent: !!v.absent, source: v.source }),
          v.absent ? 'recorded a tie absent' : 'recorded a tie', `${name(from)} ${v.type} ${name(to)}`);
        redraw();
      })),
  ];
}

function tieCard(ctx, c, redraw) {
  const { el } = ctx;
  const u = S.record.undertaking;
  const ties = u.ties.map((t, i) => ({ t, i })).filter(({ t }) => t.type === c.type && t.from === c.from && t.to === c.to);
  const type = u.types.find((t) => t.name === c.type) || {};
  const only = (U()?.diagnoses || []).find((d) => d.kind === 'succession' && d.where === `tie ${c.from} ${c.type} ${c.to}`);
  return [
    el('header', {}, el('h3', {}, el('span', {}, `${name(c.from)} ${words(c.type)} ${name(c.to)}`), closeKey(el, () => { S.chosen = null; redraw(); })),
      el('p', { class: 'small' }, `${type.meaning || ''}${type.authority ? ' · carries authority' : ''}${type.evidential ? ' · evidential' : ''}${type.challenge ? ' · a route of challenge' : ''}`)),
    ties.map(({ t, i }) => el('section', { class: 'mp-tie' },
      el('p', {}, `${t.absent ? 'Recorded absent' : 'Holds'}${t.since ? ` from ${t.since}` : ''}${t.until ? ` until ${t.until}` : ''}.`),
      t.source ? el('p', { class: 'small' }, `Source: ${t.source}`) : null,
      !t.until ? el('details', { class: 'mp-add' }, el('summary', {}, 'Record that it ends'), form(el, [{ key: 'until', label: 'on', type: 'date', value: S.at }], 'Record the end', async (v) => {
        if (!v.until) return ctx.say('Give the date it ends.');
        await keep(ctx, (x) => { x.undertaking.ties[i].until = v.until; }, 'recorded the end of a tie', `${name(t.from)} ${t.type} ${name(t.to)}, ${v.until}`);
        redraw();
      })) : null)),
    only ? el('p', { class: 'd succession' }, el('b', {}, 'Succession. '), only.what) : null,
  ];
}

function knotCard(ctx, c, redraw) {
  const { el } = ctx;
  return [
    el('header', {}, el('h3', {}, el('span', {}, c.date), closeKey(el, () => { S.chosen = null; redraw(); })), el('p', { class: 'small' }, words(c.change))),
    el('p', {}, c.what),
    el('p', { class: 'small' }, `The round is read on ${c.date}: what held then is drawn, what had ended or had not begun is not.`),
  ];
}

function summaryCard(ctx, redraw) {
  const { el } = ctx;
  const r = S.record, u = r.undertaking;
  const byKind = {};
  for (const p of u.participants) byKind[p.kind || 'participant'] = (byKind[p.kind || 'participant'] || 0) + 1;
  const diag = {};
  for (const d of U()?.diagnoses || []) diag[d.kind] = (diag[d.kind] || 0) + 1;
  return [
    el('header', {}, el('h3', {}, el('span', {}, r.title || u.name || 'The undertaking')), r.about ? el('p', { class: 'small' }, r.about) : null),
    el('section', {}, el('h4', {}, 'Who takes part'), el('p', {}, Object.entries(byKind).map(([k, n]) => `${kindOf(k).glyph} ${n} ${kindOf(k).word}${n > 1 ? 's' : ''}`).join(' · '))),
    el('section', {}, el('h4', {}, 'What the record knows of each layer'),
      (U()?.layers || []).map((l) => el('p', { class: 'small' }, el('b', {}, words(l.type)), ` · ${l.present} present, ${l.absent} absent, ${l.not_observed} not observed (${Math.round(100 * (l.observed || 0))}% of pairs observed)`))),
    Object.keys(diag).length ? el('section', {}, el('h4', {}, 'What the record shows'),
      Object.entries(diag).map(([k, n]) => el('p', { class: `d ${k.replace(/ /g, '-')}` }, el('b', {}, `${DIAGNOSES[k]?.title || k}: `), `${n}`)),
      el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key edged', onclick: () => ctx.show('governance') }, 'Read them in Governance'))) : null,
    (r.sources || []).length ? el('section', {}, el('h4', {}, 'Where it comes from'),
      r.sources.map((s) => el('p', { class: 'small' }, s.url ? el('a', { href: s.url, target: '_blank', rel: 'noopener' }, s.title) : s.title, s.licence ? ` · ${s.licence}` : '', s.read ? ` · read ${s.read.slice(0, 10)}` : ''))) : null,
    (r.undertaking.notes || []).length ? el('section', {}, el('h4', {}, 'What it leaves out'), r.undertaking.notes.map((n) => el('p', { class: 'small' }, n))) : null,
    el('details', { class: 'mp-add' }, el('summary', {}, 'Add a participant'),
      form(el, [{ key: 'name', label: 'name' }, { key: 'kind', label: 'kind', options: Object.keys(KINDS).map((k) => [k, KINDS[k].word]) },
        { key: 'constituted', label: 'holds authority by its own constitution', type: 'checkbox' }], 'Add', async (v) => {
        if (!v.name) return ctx.say('Give the participant a name.');
        const id = v.name.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '').slice(0, 40) || `p${u.participants.length + 1}`;
        if (u.participants.some((p) => p.id === id)) return ctx.say(`${v.name} is already a participant.`);
        await keep(ctx, (x) => { x.undertaking.participants.push({ id, name: v.name, kind: v.kind, constituted: !!v.constituted }); x.names = { ...(x.names || {}), [id]: v.name }; }, 'added a participant', v.name);
        S.chosen = { kind: 'participant', id };
        redraw();
      })),
    el('details', { class: 'mp-add' }, el('summary', {}, 'Declare a type of relation'),
      form(el, [{ key: 'name', label: 'name' }, { key: 'meaning', label: 'meaning' }, { key: 'authority', label: 'carries authority', type: 'checkbox' },
        { key: 'evidential', label: 'one relies on the other\'s claims', type: 'checkbox' }, { key: 'challenge', label: 'one may contest the other\'s decisions', type: 'checkbox' }], 'Declare', async (v) => {
        if (!v.name) return ctx.say('Give the type a name.');
        await keep(ctx, (x) => x.undertaking.types.push({ name: v.name, meaning: v.meaning, authority: !!v.authority, evidential: !!v.evidential, challenge: !!v.challenge }), 'declared a type of relation', v.name);
        redraw();
      })),
    el('details', { class: 'mp-add' }, el('summary', {}, 'Declare a design of observation'),
      el('p', { class: 'small' }, 'Within a design, a pair with no tie recorded is absent; outside every design it is not observed.'),
      form(el, [{ key: 'type', label: 'of the type', options: u.types.map((t) => [t.name, words(t.name)]) }, { key: 'design', label: 'the design', placeholder: 'a register kept complete' },
        { key: 'since', label: 'since', type: 'date' }, { key: 'until', label: 'until', type: 'date' },
        { key: 'covered', label: 'covering', many: u.participants.map((p) => [p.id, p.name || p.id]) }], 'Declare', async (v) => {
        if (!v.design || !v.covered.length) return ctx.say('Name the design and the participants it covers.');
        await keep(ctx, (x) => x.undertaking.observations.push({ type: v.type, design: v.design, since: v.since, until: v.until, covered: v.covered }), 'declared a design of observation', v.design);
        redraw();
      })),
  ];
}

// ---- an empty project ------------------------------------------------------------------------------------------------------------

function beginning(ctx, redraw) {
  const { el } = ctx;
  return el('section', { class: 'mp-begin' },
    el('h2', {}, 'An undertaking among many parties'),
    el('p', {}, 'Lay out a template, or begin with the first participant. Every tie, decision, commitment and value is recorded with its source, and what the record does not observe is said to be.'),
    el('div', { class: 'choices' }, S.templates.map((t) => el('button', { type: 'button', class: 'choice', disabled: !t.ready, onclick: async () => {
      try {
        await ctx.api(`${base(ctx)}/example`, { method: 'POST', body: { template: t.id } });
        S.project = null;
        ctx.log(`Laid out ${t.title}.`);
        redraw();
      } catch (error) { ctx.say(error.message); }
    } }, el('b', {}, t.title), el('span', {}, t.about), !t.ready ? el('small', {}, `Not made yet: ${t.make}`) : null))),
    el('details', { class: 'mp-add' }, el('summary', {}, 'Begin with a participant'),
      form(el, [{ key: 'name', label: 'name' }, { key: 'kind', label: 'kind', options: Object.keys(KINDS).map((k) => [k, KINDS[k].word]) }], 'Begin', async (v) => {
        if (!v.name) return ctx.say('Give the participant a name.');
        const id = v.name.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '').slice(0, 40) || 'p1';
        await keep(ctx, (x) => {
          x.undertaking.participants.push({ id, name: v.name, kind: v.kind, constituted: v.kind === 'state' });
          x.names = { [id]: v.name };
          if (!x.undertaking.types.length) x.undertaking.types.push({ name: 'authorises', meaning: 'gives another authority to act', authority: true }, { name: 'relies', meaning: 'relies on another\'s claims', evidential: true }, { name: 'challenges', meaning: 'may contest another\'s decisions', challenge: true }, { name: 'informs', meaning: 'tells another' });
        }, 'began the undertaking', v.name);
        redraw();
      })));
}

