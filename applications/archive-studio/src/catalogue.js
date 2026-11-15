// The catalogue (ADR 0119): studies of Europe's social-science data archives, harvested from the CESSDA Data Catalogue by OAI-PMH into
// the data cache (data/products/social_studies.py), found by words and laid out as slips: each with its title, its makers, its
// period and countries, its archive, its universe and its keywords. A study taken into the project is written as linked data in
// DISCO, the DDI Alliance's vocabulary for the discovery of studies, into linked/studies.ttl, where the graph and SPARQL reach it.

export function catalogueView() {
  const state = { q: '', offset: 0, answer: null };
  return { id: 'catalogue', label: 'Catalogue', draw: (pane, ctx) => draw(pane, ctx, state) };
}

async function draw(pane, ctx, state) {
  const { el, fill } = ctx;
  pane.classList.add('catalogue');
  const search = el('input', { type: 'text', class: 'search', value: state.q, placeholder: 'Words: elections Austria, care, time use, 2019…', 'aria-label': 'Find studies' });
  const find = async () => { state.q = search.value.trim(); state.offset = 0; state.answer = null; ctx.show('catalogue'); };
  search.addEventListener('keydown', (e) => { if (e.key === 'Enter') find(); });
  if (!state.answer) state.answer = await ctx.api(`/api/catalogue?q=${encodeURIComponent(state.q)}&offset=${state.offset}&limit=30`);
  const a = state.answer;
  ctx.note(a.harvested ? `${a.total} studies${state.q ? ` with “${state.q}”` : ''}` : 'no catalogue harvested');
  const head = el('header', { class: 'cat-head' }, el('h2', {}, 'Studies of social-science data archives'), search, el('button', { type: 'button', class: 'key main', onclick: find }, 'Find'));
  if (!a.harvested) {
    return fill(pane, head, el('p', { class: 'note' }, a.message, ' The catalogue states no licence for its descriptions, so a harvest is kept in the data cache only.'));
  }
  const slips = a.studies.map((s) => el('article', { class: 'slip' },
    el('h3', {}, s.title),
    el('p', { class: 'who' }, [(s.authors || []).join(', '), s.from && s.until ? `${s.from} – ${s.until}` : s.released, (s.countries || []).join(' '), s.archive].filter(Boolean).join(' · ')),
    s.universe ? el('p', { class: 'universe' }, s.universe) : null,
    el('p', { class: 'words' }, (s.keywords || []).slice(0, 8).map((k) => el('span', { class: 'chip' }, k))),
    el('p', { class: 'acts' },
      s.holdings ? el('a', { class: 'key edged', href: s.holdings, target: '_blank', rel: 'noopener' }, 'At its archive') : null,
      ctx.project ? el('button', { type: 'button', class: 'key main', onclick: async (e) => {
        e.target.disabled = true;
        try {
          const r = await ctx.api(`/api/projects/${encodeURIComponent(ctx.name)}/catalogue`, { method: 'POST', body: { id: s.id } });
          ctx.log(`${s.title} taken into linked/studies.ttl (${r.statements} statements there).`);
          e.target.textContent = 'Taken';
        } catch (error) { ctx.say(error.message); e.target.disabled = false; }
      } }, 'Take into the project') : null)));
  const pages = el('nav', { class: 'pages' },
    state.offset > 0 ? el('button', { type: 'button', class: 'key', onclick: () => { state.offset = Math.max(0, state.offset - 30); state.answer = null; ctx.show('catalogue'); } }, 'Earlier') : null,
    el('span', { class: 'n' }, a.total ? `${state.offset + 1}–${Math.min(a.total, state.offset + 30)} of ${a.total}` : 'none found'),
    state.offset + 30 < a.total ? el('button', { type: 'button', class: 'key', onclick: () => { state.offset += 30; state.answer = null; ctx.show('catalogue'); } }, 'Later') : null);
  fill(pane, head, el('div', { class: 'slips' }, slips), pages);
}
