// The arrangement (ADR 0120): the archive as its finding aid, a tree from the fonds through series and files to items, each unit with
// its reference code, its title, its dates and its creator (what it does not state, inherited from above and shown so), and the
// holdings it contains. A keeper arranges it in place: units added beneath others, moved within their siblings' order, given their
// levels, codes and dates, holdings placed into them. Every fault archeion finds (respect des fonds, original order, levels, dates
// within dates, codes) is listed and marked on its unit. The arrangement is kept as Records in Contexts (linked/arrangement.ttl),
// written and read as EAD3, and a unit or the whole archive packed as a BagIt bag for transfer. Beneath, the project's journal: every
// change, who made it and when, each entry sealed to the one before, and archeion's verdict on it.

const LEVELS = ['fonds', 'subfonds', 'series', 'subseries', 'file', 'item'];
const enc = encodeURIComponent;

export function arrangementView() {
  const state = { units: null, faults: [], unarranged: [], chosen: null, dirty: false, journal: null, verdict: null };
  return {
    id: 'arrangement', label: 'Arrangement', draw: (pane, ctx) => draw(pane, ctx, state),
    heard: (e, ctx) => { if (!state.dirty && /arranged|finding aid|holding|journal/.test(e.what)) { state.units = null; if (document.querySelector('.tab.on')?.dataset.tab === 'arrangement') ctx.show('arrangement'); } },
  };
}

async function load(ctx, state) {
  const a = await ctx.api(`/api/projects/${enc(ctx.name)}/arrangement`);
  Object.assign(state, { units: a.units, faults: a.faults, unarranged: a.unarranged, dirty: false });
}

const children = (units, id) => units.filter((u) => (u.parent || '') === (id || '')).sort((a, b) => a.order - b.order);

async function draw(pane, ctx, state) {
  const { el, fill } = ctx;
  pane.classList.add('arrangement');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  if (!state.units) await load(ctx, state);
  const base = `/api/projects/${enc(ctx.name)}`;
  const keeper = !ctx.people || ctx.me?.role === 'keeper';
  ctx.here('the arrangement');
  const units = state.units;
  ctx.note(`${units.length} units, ${state.faults.length} ${state.faults.length === 1 ? 'fault' : 'faults'}${state.dirty ? ', changed and not kept' : ''}`);
  const faultsOf = (id) => state.faults.filter((f) => f.unit === id);
  const ead = el('input', { type: 'file', accept: '.xml', hidden: true, onchange: async (e) => {
    const f = e.target.files[0];
    if (!f) return;
    try {
      const a = await ctx.api(`${base}/arrangement/ead`, { method: 'POST', raw: f });
      Object.assign(state, { units: a.units, faults: a.faults, unarranged: a.unarranged, dirty: false });
      ctx.log(`The finding aid ${f.name} read into the arrangement: ${a.units.length} units.`);
    } catch (error) { ctx.say(error.message); }
    ctx.show('arrangement');
  } });
  const keep = async () => {
    try {
      const a = await ctx.api(`${base}/arrangement`, { method: 'PUT', body: { units: state.units.map(({ id, parent, level, code, title, creator, from, to, order, holdings }) => ({ id, parent, level, code, title, creator, from, to, order, holdings: holdings.map((h) => h.name) })) } });
      Object.assign(state, { units: a.units, faults: a.faults, unarranged: a.unarranged, dirty: false });
      ctx.log(`The arrangement kept in Records in Contexts: ${a.units.length} units, ${a.faults.length} faults found by archeion.`);
    } catch (error) { ctx.say(error.message); }
    ctx.show('arrangement');
  };
  const pack = async (unit) => {
    ctx.say('Packing…');
    const r = await ctx.fetch(`${base}/package`, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ unit }) });
    if (!r.ok) return ctx.say((await r.json().catch(() => ({}))).message || 'the package could not be made');
    const name = (r.headers.get('Content-Disposition') || '').match(/filename="([^"]+)"/)?.[1] || 'bag.zip';
    const a = el('a', { href: URL.createObjectURL(await r.blob()), download: name });
    a.click();
    ctx.log(`A BagIt bag made of ${unit ? `the unit ${unit}` : 'the archive'}: ${name}, its holdings under their reference codes, with the finding aid, the linked data and the journal.`);
  };
  const head = el('header', { class: 'wall-head' }, el('h2', {}, 'The finding aid'),
    el('span', { class: 'spacer' }),
    state.dirty && keeper ? el('button', { type: 'button', class: 'key main', onclick: keep }, 'Keep the arrangement') : null,
    state.dirty ? el('button', { type: 'button', class: 'key', onclick: () => { state.units = null; ctx.show('arrangement'); } }, 'Leave the changes') : null,
    units.length ? el('a', { class: 'key', href: ctx.keyed(`${base}/arrangement.xml`), download: `${ctx.name}-finding-aid.xml` }, 'EAD3') : null,
    keeper ? el('button', { type: 'button', class: 'key', onclick: () => ead.click() }, 'Read a finding aid') : null, ead,
    el('button', { type: 'button', class: 'key', onclick: () => pack(null).catch((e) => ctx.say(e.message)) }, 'Package for transfer'));
  const faults = state.faults.length ? el('ul', { class: 'faults' }, state.faults.map((f) => el('li', { role: 'button', tabindex: 0, onclick: () => { state.chosen = f.unit; ctx.show('arrangement'); } },
    el('span', { class: 'mark' }, '✗'), ' ', el('b', {}, units.find((u) => u.id === f.unit)?.title || f.unit), ': ', f.what)))
    : units.length ? el('p', { class: 'verdict holds' }, '✓ archeion finds no fault: one provenance in each fonds, the original order kept, dates within dates, codes apart.') : null;
  const tree = el('ol', { class: 'tree' });
  const row = (u, depth) => {
    const own = faultsOf(u.id);
    const dates = [u.from || u.described?.from, u.to || u.described?.to];
    const inherited = !u.from && u.described?.from;
    const li = el('li', { class: `unit level-${u.level}${state.chosen === u.id ? ' on' : ''}${own.length ? ' faulty' : ''}`, style: { '--depth': depth } },
      el('div', { class: 'line', role: 'button', tabindex: 0, onclick: () => { state.chosen = u.id; ctx.show('arrangement'); } },
        el('span', { class: 'level' }, u.level || '—'),
        el('code', { class: 'ref' }, u.reference || u.code),
        el('span', { class: 'title' }, u.title || u.id),
        el('span', { class: `dates${inherited ? ' inherited' : ''}` }, dates[0] ? (dates[0] === dates[1] ? dates[0] : `${dates[0]}–${dates[1] || ''}`) : ''),
        u.creator || u.described?.creator ? el('span', { class: `creator${u.creator ? '' : ' inherited'}` }, u.creator || u.described.creator) : null,
        own.length ? el('span', { class: 'mark', title: own.map((f) => f.what).join('; ') }, '✗') : null),
      u.holdings.length ? el('div', { class: 'held' }, u.holdings.map((h) => el('span', { class: 'chip', title: h.sha256 ? `SHA-256 ${h.sha256}` : '' }, h.name))) : null);
    tree.append(li);
    for (const c of children(units, u.id)) row(c, depth + 1);
  };
  for (const r of children(units, '')) row(r, 0);
  const chosen = units.find((u) => u.id === state.chosen);
  const side = el('aside', { class: 'unit-card' }, chosen ? unitCard(ctx, state, chosen, keeper, pack) : el('p', { class: 'hint' }, units.length ? 'Choose a unit to see it, and, as a keeper, to arrange it.' : 'The archive has no arrangement yet. Begin it with a fonds.'),
    !units.length && keeper ? el('button', { type: 'button', class: 'key main', onclick: () => { state.units.push({ id: 'fonds', parent: '', level: 'fonds', code: 'F1', title: ctx.project.title || 'The fonds', creator: '', from: '', to: '', order: 0, holdings: [] }); state.chosen = 'fonds'; state.dirty = true; ctx.show('arrangement'); } }, 'Begin with a fonds') : null,
    state.unarranged.length ? el('section', {}, el('h4', {}, `Not yet arranged · ${state.unarranged.length}`),
      el('div', { class: 'held' }, state.unarranged.map((n) => el('span', { class: 'chip' }, n, chosen && keeper ? el('button', { type: 'button', class: 'x', title: `Place it in ${chosen.title || chosen.id}`, onclick: () => {
        chosen.holdings.push({ name: n });
        state.unarranged = state.unarranged.filter((x) => x !== n);
        state.dirty = true;
        ctx.show('arrangement');
      } }, '+') : null)))) : null);
  const journal = el('section', { class: 'journal' });
  fill(pane, head, faults, el('div', { class: 'arranged' }, tree, side), journal);
  await journalOf(ctx, state, journal);
}

function unitCard(ctx, state, u, keeper, pack) {
  const { el } = ctx;
  const field = (key, word, kind = 'text') => {
    const input = kind === 'level' ? el('select', { class: 'field', disabled: !keeper }, LEVELS.concat(LEVELS.includes(u.level) ? [] : [u.level]).map((l) => el('option', { value: l, selected: u.level === l }, l)))
      : el('input', { type: 'text', class: 'field', value: u[key] || '', disabled: !keeper, placeholder: key === 'creator' ? (u.described?.creator ? `as above: ${u.described.creator}` : '') : key === 'from' || key === 'to' ? (u.described?.[key] ? `as above: ${u.described[key]}` : 'YYYY, YYYY-MM or YYYY-MM-DD') : '' });
    input.addEventListener('change', () => { u[key] = input.value.trim(); state.dirty = true; ctx.show('arrangement'); });
    return el('label', { class: 'row' }, el('span', { class: 'word' }, word), input);
  };
  const siblings = children(state.units, u.parent);
  const move = (by) => {
    const k = siblings.indexOf(u), j = k + by;
    if (j < 0 || j >= siblings.length) return;
    [siblings[k].order, siblings[j].order] = [siblings[j].order, siblings[k].order];
    if (siblings[k].order === siblings[j].order) siblings[j].order += by;
    state.dirty = true;
    ctx.show('arrangement');
  };
  const add = () => {
    const kids = children(state.units, u.id);
    const level = LEVELS[Math.min(LEVELS.length - 1, LEVELS.indexOf(u.level) + (u.level === 'fonds' ? 2 : 1))] || 'item';
    let id = `${u.id}-${kids.length + 1}`;
    while (state.units.some((x) => x.id === id)) id += 'a';
    state.units.push({ id, parent: u.id, level, code: String(kids.length + 1), title: `A new ${level}`, creator: '', from: '', to: '', order: kids.length, holdings: [] });
    state.chosen = id;
    state.dirty = true;
    ctx.show('arrangement');
  };
  const remove = () => {
    if (children(state.units, u.id).length) return ctx.say('A unit with units beneath it is not removed; move or remove those first.');
    state.unarranged.push(...u.holdings.map((h) => h.name));
    state.units = state.units.filter((x) => x !== u);
    state.chosen = u.parent || null;
    state.dirty = true;
    ctx.show('arrangement');
  };
  return [
    el('h3', {}, u.title || u.id), el('p', { class: 'kindline' }, `${u.level} · ${u.reference || u.code}`),
    field('level', 'Level', 'level'), field('code', 'Code'), field('title', 'Title'), field('creator', 'Creator'), field('from', 'From'), field('to', 'To'),
    el('section', {}, el('h4', {}, `Holds · ${u.holdings.length}`), el('div', { class: 'held' }, u.holdings.map((h) => el('span', { class: 'chip' }, h.name,
      keeper ? el('button', { type: 'button', class: 'x', title: 'Take it out of this unit', onclick: () => { u.holdings = u.holdings.filter((x) => x !== h); state.unarranged.push(h.name); state.dirty = true; ctx.show('arrangement'); } }, '×') : null)))),
    keeper ? el('div', { class: 'actions' },
      el('button', { type: 'button', class: 'key', onclick: add }, 'A unit beneath'),
      el('button', { type: 'button', class: 'key', onclick: () => move(-1), title: 'Earlier in the original order' }, '↑'),
      el('button', { type: 'button', class: 'key', onclick: () => move(1), title: 'Later in the original order' }, '↓'),
      el('button', { type: 'button', class: 'key', onclick: remove }, 'Remove')) : null,
    el('button', { type: 'button', class: 'key edged', onclick: () => pack(u.id).catch((e) => ctx.say(e.message)) }, 'Package this unit'),
  ];
}

async function journalOf(ctx, state, box) {
  const { el, fill } = ctx;
  const base = `/api/projects/${enc(ctx.name)}`;
  const j = await ctx.api(`${base}/journal`).catch(() => ({ entries: [] }));
  const verify = el('button', { type: 'button', class: 'key edged', onclick: async () => {
    try { state.verdict = (await ctx.api(`${base}/journal/verify`)).verdict; } catch (error) { state.verdict = { status: 'fails', reason: error.message }; }
    ctx.show('arrangement');
  } }, 'Verify the journal');
  fill(box, el('header', {}, el('h4', {}, `The journal · ${j.entries.length} entries`), verify,
    state.verdict ? el('span', { class: `verdict ${state.verdict.status}` }, state.verdict.status === 'holds' ? '✓ ' : '✗ ', state.verdict.reason) : null),
    el('ol', { class: 'entries', reversed: true }, j.entries.slice(-40).reverse().map((e) => el('li', {}, el('time', { title: e.when }, e.when.slice(0, 16).replace('T', ' ')), ' ',
      el('b', {}, e.who), ' ', e.what, e.about ? el('span', { class: 'about' }, ` — ${e.about}`) : null, el('code', { class: 'seal', title: `sealed ${e.seal}` }, e.seal.slice(0, 8))))));
}
