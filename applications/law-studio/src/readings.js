// The readings (ADR 0121): interpretation, as the matter has it. Each open term the questions turn on is set out with its readings
// side by side; under each stand the courts that read it so, weighed for the forum (binding, persuasive, or left out, each with its
// reason), the parties' arguments by their canons, and what a language model proposes, which fixes nothing. A reading is fixed when
// an argument for it is accepted over the others, set aside when every argument for it is defeated, and open otherwise; the answer
// to each question follows from that, and is shown above.

import { readingsTable } from '/visualization/js/evidence/index.js';
import { S, base, ensure, keep, load, gloss, answerOf, verdictMark } from './common.js';

const CANONS = ['text', 'system', 'purpose', 'history', 'precedent'];

export function readingsView() {
  const state = { busy: false };
  return { id: 'readings', label: 'Readings', draw: (pane, ctx) => draw(pane, ctx, state) };
}

async function draw(pane, ctx, state) {
  const { el, fill } = ctx;
  pane.classList.add('readings-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the readings');
  const m = S.matter, redraw = () => ctx.show('readings');
  const change = async (f, what, note) => { await keep(ctx, f, what, note); redraw(); };
  if (!S.evaluation) return fill(pane, el('p', { class: 'note' }, S.wrong || 'Ask a question on the matter\'s page: the open terms it turns on are read here.'));
  const table = readingsTable(S.evaluation, m, S.proposals);
  ctx.note(`${table.length} open ${table.length === 1 ? 'term' : 'terms'}: ${(S.evaluation.admissibility?.fixed || []).length} readings fixed, ${(S.evaluation.admissibility?.set_aside || []).length} set aside`);

  // The forum: whose courts bind it, and the date the question is asked as of.
  const orders = [...new Set((S.scheme.courts || []).map((c) => c.order))];
  const bound = new Set(m.forum?.bound_by || []);
  const forum = el('section', { class: 'forum' },
    el('label', {}, 'Before ', el('input', { type: 'text', value: m.forum?.name || '', placeholder: 'the forum', onchange: (e) => change((x) => { (x.forum ||= {}).name = e.target.value.trim(); }, 'named the forum', e.target.value.trim()) })),
    el('span', {}, 'bound by the courts of ', orders.map((o) => el('label', { class: 'tick' }, el('input', { type: 'checkbox', checked: bound.has(o), onchange: (e) => change((x) => {
      const f = (x.forum ||= {}); const set = new Set(f.bound_by || []); if (e.target.checked) set.add(o); else set.delete(o); f.bound_by = [...set];
    }, 'changed whose courts bind the forum', `${o}: ${e.target.checked ? 'bound' : 'not bound'}`) }), o))),
    el('label', {}, 'as of ', el('input', { type: 'text', size: 10, value: m.date || '', placeholder: 'YYYY-MM-DD', onchange: (e) => change((x) => { x.date = e.target.value.trim(); }, 'set the date of the question', e.target.value.trim()) })),
    el('span', { class: 'spacer' }),
    el('button', { type: 'button', class: 'key edged', disabled: state.busy || !table.length, title: 'A language model proposes a reading of each open term, with its reasons. A proposal fixes nothing: it stands beside the arguments until counsel makes one of it.',
      onclick: async () => {
        state.busy = true; redraw();
        ctx.log('A model is reading the open terms…');
        try {
          const a = await ctx.api(`${base(ctx)}/readings`, { method: 'POST', body: {}, run: true });
          ctx.log(`${a.proposal.by} proposes: ${Object.entries(a.proposal.proposed).map(([k, v]) => `${k} as ${v || 'none'}`).join('; ')}. A proposal of grade C: it fixes nothing.`);
          await load(ctx);
        } catch (error) { ctx.log(`The model could not read them: ${error.message}`, 'wrong'); ctx.say(error.message); }
        state.busy = false; redraw();
      } }, state.busy ? 'A model is reading…' : 'Ask a model for readings'));
  const answers = el('p', { class: 'answers' }, (m.issues || []).map((i) => { const a = answerOf(i.id); return el('span', { class: 'answer', title: a?.verdict?.reason || '' }, gloss(i.question) || i.question, ' ', a ? verdictMark(el, a.class) : null); }));

  const terms = table.map((t) => {
    const argue = (reading) => {
      const canon = el('select', { 'aria-label': 'The canon' }, CANONS.map((c) => el('option', { value: c }, `from the ${c}`)));
      const party = el('input', { type: 'text', placeholder: 'whose argument', size: 12 }), text = el('input', { type: 'text', placeholder: 'the argument', class: 'wide' });
      return el('details', { class: 'argue' }, el('summary', {}, 'Argue for it'),
        el('p', {}, canon, ' ', party, ' ', text, ' ', el('button', { type: 'button', class: 'key main', onclick: () => {
          if (!text.value.trim()) return ctx.say('Say what the argument is.');
          change((x) => { (x.arguments ||= []).push({ id: `a-${Date.now().toString(36)}`, party: party.value.trim(), term: t.term, reading, canon: canon.value, text: text.value.trim() }); }, 'made an interpretive argument', `${t.term} as ${reading}, from the ${canon.value}`);
        } }, 'Argue')));
    };
    return el('section', { class: `term${S.term === t.term ? ' on' : ''}`, id: `term-${t.term}` },
      el('h3', {}, t.term), t.gloss ? el('p', { class: 'small' }, t.gloss) : null,
      el('div', { class: 'columns' }, t.readings.map((r) => el('div', { class: `reading-col${r.fixed ? ' fixed' : ''}${r.aside ? ' aside' : ''}` },
        el('h4', {}, `${r.fixed ? '◆' : r.aside ? '✕' : '◇'} ${r.name}`, el('span', { class: 'state' }, r.fixed ? 'fixed' : r.aside ? 'set aside' : 'open')),
        r.gloss ? el('p', { class: 'gloss' }, r.gloss) : null,
        r.holdings.map((h) => {
          const def = (S.scheme.holdings || []).find((x) => x.id === h.holding);
          return el('div', { class: `said holding ${h.force}` }, el('p', { class: 'who' }, el('b', {}, h.name || h.holding), ` · ${h.citation}`, el('span', { class: 'force' }, h.force)),
            def?.passage ? el('q', {}, def.passage) : null, el('p', { class: 'small' }, h.reason), def?.note ? el('p', { class: 'note-line' }, def.note) : null);
        }),
        r.arguments.map((a) => el('div', { class: 'said argument' }, el('p', { class: 'who' }, el('b', {}, a.party || 'a party'), ` · from the ${a.canon}`,
          el('button', { type: 'button', class: 'close', 'aria-label': 'Withdraw the argument', onclick: () => change((x) => { x.arguments = x.arguments.filter((y) => y.id !== a.id); }, 'withdrew an interpretive argument', a.id) }, '×')), el('p', {}, a.text))),
        r.proposals.map((p) => el('div', { class: 'said proposal' }, el('p', { class: 'who' }, el('b', {}, p.by), el('span', { class: 'force' }, 'a proposal')), el('p', {}, p.rationale),
          el('button', { type: 'button', class: 'key', title: 'Make the model\'s reason an argument of your own: it then counts, as yours.', onclick: () => change((x) => {
            (x.arguments ||= []).push({ id: `a-${Date.now().toString(36)}`, party: ctx.me?.name || 'counsel', term: t.term, reading: r.name, canon: 'purpose', text: p.rationale });
          }, 'adopted a model\'s proposal as an argument', `${t.term} as ${r.name}`) }, 'Adopt as an argument'))),
        argue(r.name)))),
      t.left_out.length ? el('p', { class: 'small left-out' }, 'Left out: ', t.left_out.map((h, k) => [k ? '; ' : '', el('b', {}, h.name || h.holding), ` (${h.reason})`])) : null);
  });

  // The ranking of the canons, which the matter states.
  const canons = [...new Set((m.arguments || []).map((a) => a.canon))];
  const order = m.canon_order || [];
  const weaker = el('select', { 'aria-label': 'The weaker canon' }, canons.map((c) => el('option', { value: c }, c)));
  const stronger = el('select', { 'aria-label': 'The stronger canon' }, canons.map((c) => el('option', { value: c }, c)).reverse());
  const ranking = canons.length > 1 ? el('section', { class: 'ranking' }, el('h3', {}, 'The ranking of the canons'),
    el('p', { class: 'small' }, 'Which kind of argument prevails over which is a legal tradition\'s to say, so it is stated here, not assumed. Unranked, arguments for different readings stand against each other and the term stays open.'),
    order.length ? el('ul', {}, order.map(([w, s], k) => el('li', {}, `an argument from the ${s} prevails over one from the ${w} `,
      el('button', { type: 'button', class: 'close', 'aria-label': 'Remove', onclick: () => change((x) => { x.canon_order.splice(k, 1); }, 'removed a ranking of canons', `${s} over ${w}`) }, '×')))) : null,
    el('p', {}, 'Let an argument from the ', stronger, ' prevail over one from the ', weaker, ' ',
      el('button', { type: 'button', class: 'key edged', onclick: () => { if (stronger.value !== weaker.value) change((x) => { (x.canon_order ||= []).push([weaker.value, stronger.value]); }, 'ranked the canons', `${stronger.value} over ${weaker.value}`); } }, 'Rank'))) : null;
  fill(pane, el('header', { class: 'wall-head' }, el('h2', {}, 'How the open terms are read')), forum, answers,
    table.length ? terms : el('p', { class: 'note' }, 'The questions turn on no open term: nothing here needs reading.'), ranking);
  if (S.term) document.getElementById(`term-${S.term}`)?.scrollIntoView({ block: 'nearest' });
}
