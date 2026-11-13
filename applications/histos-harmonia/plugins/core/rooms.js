// The core's rooms: pages of their own for inquiring into an object and for presenting it. A room is a function of the
// same context as an activity's pane, and returns the whole page beneath the room's bar.

// An object as a card: what it is, when, in what role it stands here and how the statement stands.
function rcard(ctx, a, i = 0) {
  const o = ctx.by.get(a.object), role = ctx.ROLES[a.role];
  return ctx.el('button', { type: 'button', class: `rcard${a.inherited ? ' inherited' : ''}`, style: `${role ? `border-left-color:${role.hue};` : ''}animation-delay:${Math.min(i, 8) * 50}ms`, onclick: () => ctx.go(o.id) },
    ctx.el('span', { class: 'n' }, `${ctx.glyphOf(o)} ${o.name}`),
    ctx.el('span', { class: 'm' }, `${ctx.kindName(o)} · ${ctx.whenText(o)}`),
    ctx.el('span', { class: 'note' }, a.label || role ? ctx.el('span', { class: 'chip' }, a.label || role.short) : null, a.standing ? ctx.el('span', { class: `chip ${a.standing}` }, a.standing) : null,
      a.inherited ? `inherited through ${ctx.by.get(a.inherited)?.name || a.inherited}. ` : '', a.note || ''));
}

export const room = {
  // The inquiry, one question at a time: the reader writes what they think, then sees what the record says and how it stands.
  inquire(ctx) {
    const { el, detail, object: o } = ctx;
    const Q = detail.inquiry, key = `histos.inquiry.${ctx.record.id}.${o.id}`;
    const kept = JSON.parse(sessionStorage.getItem(key) || '{"at":0,"seen":[],"guess":{}}');
    const keep = () => sessionStorage.setItem(key, JSON.stringify(kept));
    const behind = (a) => ({ ...a, label: `behind “${ctx.by.get(a.of).name}”` });
    const steps = [
      { id: 'problem', q: `What problem does ${o.name} answer?`, hint: 'A thing is made, said or done in answer to something. What was being asked?', answers: Q.problems, none: 'The record names no problem it answers, at this depth of reading.' },
      { id: 'arose', q: 'Why did that problem arise then and there?', hint: 'A problem is not always there to be asked. What had to exist before it could trouble anyone?', answers: Q.arose.map(behind), none: Q.problems.length ? 'The record does not say what its problems rested on.' : 'With no problem named, there is nothing to ask this of.' },
      { id: 'means', q: 'What made it answerable?', hint: 'With what tools, on whose earlier work, against what? An answer needs its means as well as its question.', answers: [...Q.answerable, ...Q.against], none: 'The record names nothing that made it possible.' },
      { id: 'worth', q: 'To whom was it worth answering, and what did it make possible?', hint: 'Someone made it, kept it or belonged to it; and others built on it.',
        answers: [...Q.for_whom.map((a) => ({ ...a, label: a.role.replace('_', ' ') })), ...Q.after.map((a) => ({ ...a, label: 'then came', role: '' }))], none: 'The record names neither.' },
      ...(detail.comparisons.length ? [{ id: 'others', q: 'Who else answered the same problem?', hint: 'The same problem answered elsewhere, with or without knowledge of this answer. These are the cases to compare.',
        answers: detail.comparisons.map((c) => ({ object: c.object, label: c.independent ? 'independently' : 'not independently', standing: ctx.by.get(c.object).standing,
          note: `${c.independent ? 'The record knows no path of dependence between them.' : 'One rests on the other.'} On ${c.problems.map((p) => `“${ctx.by.get(p).name}”`).join(', ')}.` })) }] : []),
      ...(detail.rests_on_it.length ? [{ id: 'rests', q: 'What, in this record, could not have appeared without it?', hint: 'Everything all of whose premises pass through this object. It is a property of the record, not a claim about what history would have done.',
        answers: detail.rests_on_it.map((id) => ({ object: id, standing: ctx.by.get(id).standing })) }] : []),
    ];
    // A link may ask that every answer be shown (answers=shown), as for a picture of the page.
    if (sessionStorage.getItem('histos.open') === '1') kept.seen = steps.map((x) => x.id);
    const host = el('div', { class: 'inq' });
    const draw = () => {
      const at = Math.min(kept.at, steps.length), last = at === steps.length;
      const rail = el('nav', { class: 'inq-rail' }, el('div', { class: 'caption', style: 'margin-bottom:10px' }, 'An inquiry'),
        ...steps.map((s, i) => el('button', { type: 'button', class: `${i === at ? 'on' : ''}${kept.seen.includes(s.id) ? ' done' : ''}`, onclick: () => { kept.at = i; keep(); draw(); } }, s.q.replace(o.name, 'it'))),
        el('button', { type: 'button', class: last ? 'on' : '', onclick: () => { kept.at = steps.length; keep(); draw(); } }, 'What you have found'));
      let main;
      if (last) {
        main = el('div', { class: 'inq-step' }, el('div', { class: 'num' }, 'In the end'), el('h2', {}, `What you have found of ${o.name}`),
          ...steps.flatMap((s) => [el('h3', { style: 'margin-top:20px;font-weight:600' }, s.q),
            kept.guess[s.id] ? el('div', { class: 'yours' }, `You thought: ${kept.guess[s.id]}`) : null,
            kept.seen.includes(s.id) ? (s.answers.length ? el('p', {}, 'The record says: ', s.answers.map((a) => ctx.by.get(a.object).name).join('; '), '.') : el('p', { class: 'silent' }, s.none)) : el('p', { class: 'silent' }, 'Not yet looked at.')]),
          Q.silent.length ? el('p', { class: 'silent', style: 'margin-top:22px' }, `Where the record is silent: ${Q.silent.join('; ')}. A silence of the record is not an absence in history: it is a place to look further.`) : null,
          el('div', { class: 'inq-nav' }, el('button', { class: 'key', type: 'button', onclick: () => ctx.openRoom('trace') }, 'Trace everything it rested on'), el('button', { class: 'key', type: 'button', onclick: () => ctx.openRoom('examine') }, 'Examine what is said, and by whom')));
      } else {
        const s = steps[at], seen = kept.seen.includes(s.id);
        const guess = el('textarea', { placeholder: 'A sentence or two. It stays on this computer, for you.' });
        guess.value = kept.guess[s.id] || '';
        guess.addEventListener('input', () => { kept.guess[s.id] = guess.value; keep(); });
        const reveal = () => { kept.seen.push(s.id); keep(); draw(); };
        main = el('div', { class: 'inq-step' }, el('div', { class: 'num' }, `Question ${at + 1} of ${steps.length}`), el('h2', {}, s.q), el('p', { class: 'hintline' }, s.hint),
          seen ? null : el('div', { class: 'guess' }, el('label', {}, 'What do you think?'), guess),
          seen ? null : el('button', { class: 'key strong', type: 'button', onclick: reveal }, 'See what the record says'),
          seen && kept.guess[s.id] ? el('div', { class: 'yours' }, `You thought: ${kept.guess[s.id]}`) : null,
          seen ? (s.answers.length ? el('div', { class: 'rcards', style: 'margin-top:18px' }, ...s.answers.map((a, i) => rcard(ctx, a, i))) : el('p', { class: 'silent', style: 'margin-top:18px;font-size:17px' }, s.none)) : null,
          seen ? el('p', { class: 'silent', style: 'margin-top:14px;font-size:13px' }, 'Each card says how its statement stands. Choose one to go on to it, and inquire there.') : null,
          el('div', { class: 'inq-nav' }, at > 0 ? el('button', { class: 'key', type: 'button', onclick: () => { kept.at = at - 1; keep(); draw(); } }, '← Before') : null,
            el('button', { class: seen ? 'key strong' : 'key', type: 'button', onclick: () => { kept.at = at + 1; keep(); draw(); } }, at + 1 < steps.length ? 'The next question →' : 'What you have found →')));
      }
      host.replaceChildren(rail, main);
    };
    draw();
    return host;
  },

  // The object presented, as a museum would: what it is, in large type, and where it stands among others.
  async present(ctx) {
    const { el, object: o, detail, api } = ctx;
    const names = Object.values(o.names || {}).join(' · ');
    const held = (await api(`/api/holdings?object=${encodeURIComponent(o.id)}`)).holdings.filter((h) => h.access !== 'dark');
    const of = (ids) => ids.map((id, i) => rcard(ctx, { object: id, standing: ctx.by.get(id).standing }, i));
    const Q = detail.inquiry;
    return el('div', {},
      el('div', { class: 'hero' }, el('div', { class: 'caption' }, `${ctx.glyphOf(o)} ${ctx.kindName(o)}`), el('h1', { class: 'display' }, o.name), names ? el('div', { class: 'own' }, names) : null,
        el('div', { class: 'meta' }, ctx.whenText(o), ctx.placeNames(o).length ? ` · ${ctx.placeNames(o).join(', ')}` : '', o.tradition ? ` · ${o.tradition}` : ''),
        o.summary ? el('p', { class: 'lead' }, o.summary) : el('p', { class: 'lead silent' }, 'The record gives no account of it yet.')),
      held.some((h) => h.type.startsWith('image/')) ? el('div', { class: 'columns' }, ...held.filter((h) => h.type.startsWith('image/')).map((h) => el('figure', { style: 'margin:0' },
        el('img', { src: `/api/holding/${h.sha256}`, alt: h.note || h.name, style: 'max-width:100%;border:1px solid var(--line)' }), el('figcaption', { class: 'silent', style: 'font-size:13px' }, `${h.note || h.name} · ${h.rights}`)))) : null,
      el('div', { class: 'columns' },
        Q.problems.length ? el('div', {}, el('h2', {}, 'It answers'), el('div', { class: 'rcards' }, ...Q.problems.map((a, i) => rcard(ctx, a, i)))) : null,
        Q.answerable.length ? el('div', {}, el('h2', {}, 'It rested on'), el('div', { class: 'rcards' }, ...Q.answerable.map((a, i) => rcard(ctx, a, i)))) : null,
        Q.after.length ? el('div', {}, el('h2', {}, 'Then came'), el('div', { class: 'rcards' }, ...Q.after.map((a, i) => rcard(ctx, { ...a, role: '' }, i)))) : null,
        detail.lineage.length ? el('div', {}, el('h2', {}, 'It stands under'), el('div', { class: 'rcards' }, ...of(detail.lineage.map((a) => a.object)))) : null,
        detail.under.length ? el('div', {}, el('h2', {}, 'Under it stand'), el('div', { class: 'rcards' }, ...of(detail.under))) : null),
      el('div', { class: 'columns' }, el('p', { class: 'silent' }, `How the record's word on it stands: ${o.standing}. ${o.standing === 'proposed' ? 'A language model proposes it from memory, and no one has yet held it against a source.' : ''}`,
        el('br'), el('button', { class: 'key strong', type: 'button', style: 'margin-top:14px', onclick: () => ctx.openRoom('inquire') }, 'Inquire into it →'))));
  },
};
