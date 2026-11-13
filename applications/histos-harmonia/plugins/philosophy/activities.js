// Philosophy's activity: a dispute followed. An object that carries an argument map (questions, the positions that answer
// them, arguments for and against) has it read and evaluated by the philosophy domain; the page lays out what the engine
// answers. The standing of a position is the grounded semantics': what is defended from what nothing attacks is accepted,
// what an accepted argument attacks is rejected, and a tie is left undecided.

const WORDS = { accepted: 'stands', rejected: 'falls', undecided: 'is left undecided' };

export const render = {
  async dispute(ctx) {
    const { el, object: o, api } = ctx;
    const M = await api('/api/map', { method: 'POST', body: JSON.stringify({ text: o.map }) });
    const host = el('div', { class: 'amap' });
    if (M.errors.length || M.faults.length) {
      host.append(el('p', { class: 'msg' }, 'The map written for this object is not well formed, and is shown without its standings.'),
        ...M.errors.map((e) => el('div', { class: 'said' }, `line ${e.line}: ${e.why}`)), ...M.faults.map((f) => el('div', { class: 'said' }, `${f.id}: ${f.why}`)));
    }
    const standing = (s) => (s ? el('span', { class: 'st' }, ` — ${WORDS[s.status]}${s.deciding.length ? `, by ${s.deciding.join(', ')}` : ''}`) : null);
    const text = Object.fromEntries([...M.positions, ...M.arguments].map((x) => [x.id, x.text]));
    const bearing = (target) => M.arguments.filter((a) => a.bears.some((b) => b.target === target)).map((a) => {
      const polarity = a.bears.find((b) => b.target === target).polarity;
      return el('div', { class: `argument ${polarity}` }, el('b', {}, polarity === 'pro' ? 'For: ' : 'Against: '), a.text || text[a.id] || a.id, standing(a.standing),
        a.premises.length ? el('div', { class: 'st' }, `From: ${a.premises.join(' · ')}${a.conclusion ? ` — therefore: ${a.conclusion}` : ''}`) : null, ...bearing(a.id));
    });
    for (const q of M.questions) {
      host.append(el('div', { class: 'question' }, `? ${q.text}`),
        el('div', { class: 'st' }, q.exclusive ? 'Its positions exclude one another. ' : '', M.evaluated ? (q.open ? 'As argued here, the question is open: no position stands.' : `As argued here, what stands: ${q.accepted.map((p) => `“${text[p]}”`).join('; ')}`) : ''),
        ...M.positions.filter((p) => p.answers === q.id).map((p) => el('div', { class: 'position' }, p.text, standing(p.standing), ...bearing(p.id))));
    }
    host.append(el('p', { class: 'silent' }, 'A map is an outline of a dispute by whoever wrote it, and its verdict is of the map: add an argument, and what stands may change. This one is part of the record’s draft by a language model, unconfirmed.'));
    return host;
  },
};
