// The core's activities: what can be done with any historical object. Each is a function of a context (the object, what the
// engine answered of it, the record, and the page's helpers) that returns a node for the object's sheet.

const answer = (ctx, a, extra = '') => {
  const o = ctx.by.get(a.object);
  return ctx.el('div', { class: `ans${a.inherited ? ' inherited' : ''}`, onclick: () => ctx.select(o.id) },
    ctx.el('span', { class: 'g' }, ctx.glyphOf(o)), ctx.el('span', { class: 'n' }, o.name, o.appears !== undefined ? `, ${ctx.yearText(o.appears)}` : ''),
    ctx.el('span', { class: `chip ${a.standing}` }, a.standing),
    (a.note || a.inherited || extra) ? ctx.el('span', { class: 'note' }, [a.inherited ? `inherited through ${ctx.by.get(a.inherited)?.name || a.inherited}` : '', extra, a.note].filter(Boolean).join(' · ')) : null);
};
const silent = (ctx, words) => ctx.el('p', { class: 'silent' }, words);

// A question asked before its answer is shown: the reader thinks first.
function asked(ctx, question, hint, body, count) {
  const d = ctx.el('details', { class: 'ask' }, ctx.el('summary', { class: 'qtext' }, question, ctx.el('span', { class: 'n', style: 'font-weight:400;color:var(--faint)' }, count === 0 ? '  · the record is silent' : `  · ${count} in the record`)),
    hint ? ctx.el('p', { class: 'silent' }, hint) : null, ...body);
  d.open = sessionStorage.getItem('histos.open') === '1';
  return d;
}

export const render = {
  inquire(ctx) {
    const { el, detail, object: o } = ctx;
    const Q = detail.inquiry;
    const roleWord = { condition: 'a condition', source: 'a source', problem: 'a problem behind it', against: 'argued against' };
    const byProblem = new Map();
    for (const a of Q.arose) { if (!byProblem.has(a.of)) byProblem.set(a.of, []); byProblem.get(a.of).push(a); }
    const everyone = el('button', { class: 'key quiet', type: 'button', onclick: () => { const on = sessionStorage.getItem('histos.open') !== '1'; sessionStorage.setItem('histos.open', on ? '1' : '0'); ctx.reload(); } },
      sessionStorage.getItem('histos.open') === '1' ? 'Ask me first' : 'Show every answer');
    return el('div', {},
      el('p', {}, `Four questions are asked of every object. Think what the answer to each might be for ${o.name}; then open it and see what the record says, and how each statement stands.`, ' ', everyone),
      asked(ctx, '1. What problem does it answer?', 'A thing is made, said or done in answer to something. What was asked, here?', Q.problems.length ? Q.problems.map((a) => answer(ctx, a)) : [silent(ctx, 'The record names no problem it answers, at this layer.')], Q.problems.length),
      asked(ctx, '2. Why did that problem arise then and there?', 'A problem is not always there to be asked. What had to exist before it could trouble anyone?',
        Q.problems.length ? [...byProblem].flatMap(([p, list]) => [el('h2', {}, `Behind “${ctx.by.get(p).name}”`), ...list.map((a) => answer(ctx, a, roleWord[a.role]))]).concat(Q.arose.length ? [] : [silent(ctx, 'The record does not say what its problems rested on.')])
          : [silent(ctx, 'With no problem named, there is nothing to ask this of.')], Q.arose.length),
      asked(ctx, '3. What made it answerable?', 'With what tools, on what earlier work, against what? An answer needs its means as well as its question.',
        [...(Q.answerable.length ? Q.answerable.map((a) => answer(ctx, a, roleWord[a.role])) : [silent(ctx, 'The record names nothing that made it possible.')]),
          ...(Q.against.length ? [el('h2', {}, 'What it argued against'), ...Q.against.map((a) => answer(ctx, a))] : [])], Q.answerable.length + Q.against.length),
      asked(ctx, '4. To whom was it worth answering, and what did it make possible?', 'Someone made it, kept it, or belonged to it; and others built on it.',
        [...(Q.for_whom.length ? [el('h2', {}, 'Who made it, held it, belonged to it'), ...Q.for_whom.map((a) => answer(ctx, a))] : []),
          ...(Q.after.length ? [el('h2', {}, 'What rested on it next'), ...Q.after.map((a) => answer(ctx, a, roleWord[a.role] ? `as ${roleWord[a.role]}` : ''))] : []),
          ...(Q.for_whom.length + Q.after.length ? [] : [silent(ctx, 'The record names neither.')])], Q.for_whom.length + Q.after.length),
      detail.comparisons.length ? asked(ctx, 'Who else answered the same problem?', 'The same problem answered elsewhere, with or without knowledge of this answer: the cases to compare.',
        detail.comparisons.map((c) => answer(ctx, { object: c.object, standing: ctx.by.get(c.object).standing }, `${c.independent ? 'independently: the record knows no path of dependence between them' : 'not independently: one rests on the other'} · on ${c.problems.map((p) => `“${ctx.by.get(p).name}”`).join(', ')}`)), detail.comparisons.length) : null,
      detail.rests_on_it.length ? asked(ctx, 'What, in this record, could not have appeared without it?', 'Everything all of whose premises pass through this object. It is a property of the record, not a claim about what history would have done.',
        detail.rests_on_it.map((id) => answer(ctx, { object: id, standing: ctx.by.get(id).standing })), detail.rests_on_it.length) : null,
      Q.silent.length ? el('p', { class: 'silent' }, `Where the record is silent at this layer: ${Q.silent.join('; ')}. A silence of the record is not an absence in history.`) : null);
  },

  present(ctx) {
    const { el, object: o, detail } = ctx;
    const under = detail.under.map((id) => ctx.by.get(id));
    return el('div', {},
      o.summary ? el('p', {}, o.summary) : silent(ctx, 'The record gives no account of it yet.'),
      detail.lineage.length ? el('div', {}, el('h2', {}, 'It stands under'), ...detail.lineage.map((a) => answer(ctx, { object: a.object, standing: ctx.by.get(a.object).standing }, a.steps === 1 ? a.by : `${a.steps} steps above`))) : null,
      under.length ? el('div', {}, el('h2', {}, 'Under it stand'), ...under.map((u) => answer(ctx, { object: u.id, standing: u.standing }))) : null,
      el('h2', {}, 'In the record'),
      el('p', {}, `${/^[aeiou]/.test(ctx.kindName(o)) ? 'An' : 'A'} ${ctx.kindName(o)}`, (o.above || []).length > 1 ? `, which is a kind of ${o.above.slice(0, -1).map((k) => ctx.record.kinds.find((x) => x.id === k)?.name || k).join(', a kind of ')}` : '', '. ',
        o.when ? `Its date: ${ctx.whenText(o)} (written “${o.when.text}” in the record). ` : 'It has no date. ', o.tradition ? `Tradition: ${o.tradition}. ` : '', o.discipline ? `Discipline: ${o.discipline}.` : ''),
      el('p', { class: 'silent' }, `The record's word that it was stands as ${o.standing}: ${o.standing === 'proposed' ? 'a language model proposes it from memory, and no one has confirmed it against a source' : o.standing === 'attested' ? 'a source states it, short of what establishes' : o.standing === 'established' ? 'confirmed against its sources, and disputed by none' : 'a source disputes it'}.`));
  },

  trace(ctx) {
    const { el, detail } = ctx;
    const group = (entries, words) => {
      const steps = [...new Set(entries.map((e) => e.steps))].sort((a, b) => a - b);
      return steps.flatMap((s) => [el('h2', {}, words(s)), ...entries.filter((e) => e.steps === s).map((e) => answer(ctx, { object: e.object, standing: s === 1 ? e.standing : ctx.by.get(e.object).standing, inherited: e.inherited }, e.role && ctx.ROLES[e.role] ? ctx.ROLES[e.role].short : ''))]);
    };
    return el('div', {},
      el('p', {}, el('button', { class: 'key', type: 'button', onclick: () => { ctx.S.view = 'cone'; ctx.select(ctx.object.id, false); } }, 'Draw its cone'), `  ${detail.past.length} before it, ${detail.future.length} after.`),
      ...(detail.past.length ? group(detail.past, (s) => (s === 1 ? 'What its appearing rested on' : `${s} steps back`)) : [silent(ctx, 'The record names nothing it rested on, at this layer.')]),
      ...(detail.future.length ? group(detail.future, (s) => (s === 1 ? 'What rested on it' : `${s} steps on`)) : [silent(ctx, 'The record names nothing that rests on it, at this layer.')]));
  },

  async examine(ctx) {
    const { el, object: o, record, api } = ctx;
    const cite = (id) => record.sources.find((s) => s.id === id)?.cite || id;
    const saying = (a) => el('div', { class: 'said' }, `${a.grade} · `, el('span', { class: 'by' }, cite(a.by)), a.disputes ? ' disputes it' : '', a.quote ? el('span', {}, ': ', el('q', {}, a.quote)) : '', a.where ? ` (${a.where})` : '',
      a.confirmed ? ` · confirmed by ${a.confirmed}` : a.grade === 'C' ? ' · a model\'s word, from memory' : ' · not yet confirmed');
    const type = (id) => record.types.find((t) => t.id === id);
    const mine = record.relations.filter((r) => r.from === o.id || r.to === o.id);
    const held = (await api(`/api/holdings?object=${encodeURIComponent(o.id)}`)).holdings;
    const asSource = record.sources.filter((s) => s.object === o.id);
    return el('div', {},
      el('h2', {}, 'That it was'), ...(o.said.length ? o.said.map(saying) : [silent(ctx, 'No source is named.')]),
      asSource.length ? el('p', {}, 'It is also a source of this record: ', asSource.map((s) => s.cite).join('; '), '.') : null,
      el('h2', {}, `Its relations `, el('span', { class: 'n' }, `· ${mine.length}`)),
      ...mine.map((r) => { const t = type(r.type), other = ctx.by.get(r.from === o.id ? r.to : r.from);
        return el('div', { class: 'ans', onclick: () => ctx.select(other.id) }, el('span', { class: 'g' }, ctx.glyphOf(other)), el('span', { class: 'n' }, `${r.from === o.id ? t.name : t.inverse} ${other.name}`), el('span', { class: `chip ${r.standing}` }, r.standing),
          el('span', { class: 'note' }, [r.note, ...r.said.map((a) => `${a.grade} · ${cite(a.by).slice(0, 60)}${a.confirmed ? ' · confirmed' : ''}`)].filter(Boolean).join(' — '))); }),
      el('h2', {}, 'What the archive holds of it ', el('span', { class: 'n' }, `· ${held.length}`)),
      ...(held.length ? held.map((h) => el('div', { class: 'said' }, h.access === 'dark' ? el('span', {}, h.name) : el('a', { href: `/api/holding/${h.sha256}`, target: '_blank' }, h.name), ` · ${h.type} · ${h.rights} · ${h.access}`, h.note ? ` · ${h.note}` : '',
        h.access !== 'dark' && h.type.startsWith('image/') ? el('div', {}, el('img', { src: `/api/holding/${h.sha256}`, alt: h.note || h.name, style: 'max-width:100%;margin-top:6px;border:1px solid var(--line)' })) : null))
        : [silent(ctx, 'Nothing yet. Materials are taken in at the desk.')]));
  },
};
