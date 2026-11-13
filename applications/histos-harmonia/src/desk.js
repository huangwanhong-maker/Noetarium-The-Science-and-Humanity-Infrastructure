// The desk of Histos Harmonia: where an archive's keeper works on a record. Five sheets: what is said (every statement with
// how it stands, and what a source says of it added and confirmed), the holdings (materials kept by the hash of their
// content, with their rights), the sources, the log (what was done, only ever added to), and the record verified (the
// faults the reader found, kategoria's verdict on the record as a functor on its schema, and the record as a package).

let tab = 'said', filter = 'all', chosen = null;

export function desk(ctx) {
  const { S, bar, body, el } = ctx;
  const tabs = [['said', 'What is said'], ['holdings', 'Holdings'], ['sources', 'Sources'], ['log', 'Log'], ['verify', 'Verified']];
  bar.replaceChildren(...tabs.map(([id, name]) => el('button', { class: `key${tab === id ? ' on' : ''}`, type: 'button', onclick: () => { tab = id; desk(ctx); } }, name)),
    el('span', { class: 'say' }, S.records.find((r) => r.id === S.record)?.where === 'archive' ? 'Working on the archive\'s own copy of this record.' : 'The library\'s record: the first change makes the archive a working copy of it.'));
  const host = el('div', { class: 'desk' });
  body.replaceChildren(host);
  ({ said, holdings, sources, log, verify })[tab](ctx, host).catch((error) => host.replaceChildren(el('p', { class: 'msg' }, error.message)));
}

const chip = (el, standing) => el('span', { class: `chip ${standing}` }, standing);

// Every statement of the record: that an object was, a relation, an emergence, a premise.
function statements(S) {
  const name = (id) => S.by.get(id)?.name || id;
  const type = (id) => S.data.types.find((t) => t.id === id)?.name || id;
  const out = [];
  for (const o of S.data.objects) out.push({ row: 'object', id: o.id, object: o.id, text: `${o.name} was${o.when ? `, ${o.when.text}` : ''}`, standing: o.standing, said: o.said });
  for (const r of S.data.relations) out.push({ row: 'relation', id: r.id, object: r.from, text: `${name(r.from)} ${type(r.type)} ${name(r.to)}`, note: r.note, standing: r.standing, said: r.said });
  for (const e of S.data.emergences.filter((x) => x.written)) {
    out.push({ row: 'emergence', id: e.id, object: e.product, text: `${name(e.product)} appeared${e.when ? `, ${e.when.text}` : ''}${e.note ? ` (${e.note})` : ''}`, standing: e.standing, said: e.said });
    for (const p of e.premises) out.push({ row: 'premise', id: e.id, what: p.what, object: e.product, text: `${name(p.what)} was a ${p.role === 'against' ? 'thing argued against' : p.role} of the appearing of ${name(e.product)}`, note: p.note, standing: p.standing, said: [] });
  }
  return out;
}

async function said(ctx, host) {
  const { S, el, api, select, reload } = ctx;
  const all = statements(S);
  const counts = all.reduce((c, s) => ({ ...c, [s.standing]: (c[s.standing] || 0) + 1 }), {});
  const shown = all.filter((s) => filter === 'all' || s.standing === filter).filter((s) => !S.selected || filter !== 'object' || s.object === S.selected);
  const mine = S.selected ? shown.filter((s) => s.object === S.selected || s.what === S.selected) : shown;
  const list = S.selected && mine.length ? mine : shown;
  const key = (s) => `${s.row}:${s.id}:${s.what || ''}`;
  const picked = list.find((s) => key(s) === chosen) || null;
  const holdings = (await api('/api/holdings')).holdings.filter((h) => h.type.startsWith('text/') || /\.(txt|md|xml|tei|html)$/.test(h.name));
  const msg = el('div', { class: 'msg' });
  const field = (label, node) => el('label', {}, label, node);
  const by = el('select', {}, ...S.data.sources.filter((s) => s.kind !== 'model').map((s) => el('option', { value: s.id }, `${s.id} · ${s.cite.slice(0, 70)}`)));
  const grade = el('select', {}, el('option', { value: 'B' }, 'B · a text states it (quote the passage)'), el('option', { value: 'A' }, 'A · a structured source records it'));
  const quote = el('textarea', { placeholder: 'The passage, in the source\'s own words' });
  const where = el('input', { type: 'text', placeholder: 'page, folio, section, identifier' });
  const holding = el('select', {}, el('option', { value: '' }, 'not looked for in a holding'), ...holdings.map((h) => el('option', { value: h.sha256 }, `${h.name} · ${h.sha256.slice(0, 10)}`)));
  const documentary = el('input', { type: 'checkbox' }), disputes = el('input', { type: 'checkbox' });
  const who = el('input', { type: 'text', placeholder: 'your name', value: localStorage.getItem('histos.keeper') || '' });
  const add = async () => {
    try {
      localStorage.setItem('histos.keeper', who.value);
      await api('/api/assert', { method: 'POST', body: JSON.stringify({ record: S.record, row: picked.row, id: picked.id, what: picked.what, by: by.value, grade: grade.value, quote: quote.value,
        where: where.value, holding: holding.value || undefined, documentary: documentary.checked, disputes: disputes.checked, confirmed: who.value }) });
      await reload();
    } catch (error) { msg.textContent = error.message; }
  };
  host.replaceChildren(
    el('div', { class: 'row' }, el('span', { class: 'caption' }, 'Standing'), ...['all', 'established', 'attested', 'proposed', 'disputed'].map((f) => el('button', { class: `key${filter === f ? ' on' : ''}`, type: 'button', onclick: () => { filter = f; desk(ctx); } },
      f, f === 'all' ? null : el('b', {}, counts[f] || 0))), el('span', { class: 'say' }, S.selected && mine.length ? `Those about ${S.by.get(S.selected).name}; choose nothing in the list to see all.` : `${list.length} statements`)),
    picked ? el('div', { class: 'form' }, el('h3', {}, picked.text), picked.note ? el('p', { class: 'silent' }, picked.note) : null,
      ...(picked.said.length ? picked.said.map((a) => el('div', { class: 'said' }, `${a.grade} · `, el('span', { class: 'by' }, a.by), a.disputes ? ' disputes it' : '', a.quote ? el('span', {}, ': ', el('q', {}, a.quote)) : '', a.where ? ` (${a.where})` : '',
        a.documentary ? ' · documentary' : '', a.confirmed ? ` · confirmed by ${a.confirmed}` : ' · not confirmed')) : [el('p', { class: 'silent' }, 'Nothing is said of it yet by a source of its own.')]),
      S.writable && picked.row !== 'premise' || S.writable ? el('div', {}, el('h2', { class: 'caption', style: 'margin-top:12px' }, 'Add what a source says'),
        S.data.sources.some((s) => s.kind !== 'model') ? null : el('p', { class: 'msg' }, 'The record names no source but a model: add a source first.'),
        el('div', { class: 'row' }, field('Source', by), field('Grade', grade)), quote,
        el('div', { class: 'row' }, field('Where', where), field('Look for the passage in', holding)),
        el('div', { class: 'row' }, field('', documentary), 'the source is the thing itself speaking', field('', disputes), 'the source denies the statement'),
        el('div', { class: 'row' }, field('Confirmed against the source by', who), el('button', { class: 'key', type: 'button', onclick: add }, 'Add'), msg),
        el('p', { class: 'silent' }, 'A statement is established by a confirmed documentary assertion, or by confirmed assertions of two different sources, when none disputes it. A passage looked for in a holding and not found adds nothing.')) : null) : null,
    el('table', {}, el('tr', {}, el('th', {}, 'Statement'), el('th', {}, 'Stands'), el('th', {}, 'Said by')),
      ...list.slice(0, 400).map((s) => el('tr', { class: `choose${key(s) === chosen ? ' on' : ''}`, onclick: () => { chosen = key(s); desk(ctx); } },
        el('td', {}, s.text), el('td', {}, chip(el, s.standing)), el('td', {}, [...new Set(s.said.map((a) => `${a.by} (${a.grade}${a.confirmed ? ', confirmed' : ''})`))].join('; '))))));
  void select;
}

async function holdings(ctx, host) {
  const { S, el, api } = ctx;
  const rows = (await api('/api/holdings')).holdings;
  const msg = el('div', { class: 'msg' });
  const rights = el('input', { type: 'text', placeholder: 'e.g. Public domain; CC BY 4.0; © the museum, all rights reserved', style: 'min-width:340px' });
  const access = el('select', {}, ...['restricted', 'public', 'dark'].map((a) => el('option', { value: a }, a)));
  const note = el('input', { type: 'text', placeholder: 'what it is: a scan of folio 3r, a recording, a transcription' , style: 'min-width:300px' });
  const objects = el('input', { type: 'text', value: S.selected || '', placeholder: 'ids of the objects it is of, separated by commas' });
  const take = async (files) => {
    for (const f of files) {
      try {
        const q = new URLSearchParams({ name: f.name.replace(/[^A-Za-z0-9._-]/g, '-'), objects: objects.value.replace(/\s/g, ''), rights: rights.value, access: access.value, note: note.value });
        await api(`/api/holdings?${q}`, { method: 'POST', body: f });
      } catch (error) { msg.textContent = `${f.name}: ${error.message}`; return; }
    }
    desk(ctx);
  };
  const drop = el('div', { class: 'drop', ondragover: (e) => { e.preventDefault(); drop.classList.add('over'); }, ondragleave: () => drop.classList.remove('over'), ondrop: (e) => { e.preventDefault(); take(e.dataTransfer.files); } },
    'Drop files here, or ', el('input', { type: 'file', multiple: true, onchange: (e) => take(e.target.files) }));
  const size = (n) => (n > 1048576 ? `${(n / 1048576).toFixed(1)} MB` : `${Math.ceil(n / 1024)} kB`);
  host.replaceChildren(
    el('div', { class: 'form' }, el('h3', {}, 'Take in materials'),
      el('p', { class: 'silent' }, 'A file is kept under the SHA-256 of its content and never altered; what it is, whose it is and who may see it are written beside it. The same content taken in twice is one holding.'),
      el('div', { class: 'row' }, el('label', {}, 'Of the objects', objects), el('label', {}, 'Access', access)), el('div', { class: 'row' }, el('label', {}, 'Rights', rights)), el('div', { class: 'row' }, el('label', {}, 'Note', note)), drop, msg),
    el('div', { class: 'row' }, el('button', { class: 'key', type: 'button', onclick: async (e) => {
      const r = await api('/api/fixity', { method: 'POST' });
      e.target.after(el('span', { class: `msg${r.altered.length + r.missing.length ? '' : ' ok'}` }, ` ${r.checked} checked: ${r.altered.length} altered, ${r.missing.length} missing (${r.at})`));
    } }, 'Check every holding against its hash')),
    el('table', {}, el('tr', {}, el('th', {}, 'Holding'), el('th', {}, 'Of'), el('th', {}, 'Rights'), el('th', {}, 'Access'), el('th', {}, 'Taken in'), el('th', {}, 'SHA-256')),
      ...(rows.length ? rows.map((h) => el('tr', {}, el('td', {}, h.access === 'dark' ? h.name : el('a', { href: `/api/holding/${h.sha256}`, target: '_blank' }, h.name), el('div', { class: 'silent' }, `${h.type} · ${size(h.bytes)}${h.note ? ` · ${h.note}` : ''}`)),
        el('td', {}, (h.objects || []).map((id) => S.by.get(id)?.name || id).join(', ')), el('td', {}, h.rights), el('td', {}, h.access), el('td', {}, (h.ingested || '').slice(0, 10)), el('td', { class: 'mono' }, h.sha256.slice(0, 16)))) :
        [el('tr', {}, el('td', { colspan: 6, class: 'silent' }, 'The archive holds no materials yet.'))])));
}

async function sources(ctx, host) {
  const { S, el, api, reload } = ctx;
  const msg = el('div', { class: 'msg' });
  const id = el('input', { type: 'text', placeholder: 'a short id: barbour1951' }), cite = el('input', { type: 'text', placeholder: 'the source, cited in full', style: 'min-width:460px' });
  const kind = el('select', {}, ...['reference', 'primary', 'structured'].map((k) => el('option', { value: k }, k)));
  const url = el('input', { type: 'text', placeholder: 'where it can be read, if anywhere' });
  const used = (s) => S.data.objects.reduce((n, o) => n + o.said.filter((a) => a.by === s).length, 0) + S.data.relations.reduce((n, r) => n + r.said.filter((a) => a.by === s).length, 0);
  host.replaceChildren(
    S.writable ? el('div', { class: 'form' }, el('h3', {}, 'Add a source'), el('div', { class: 'row' }, el('label', {}, 'Id', id), el('label', {}, 'Kind', kind), el('label', {}, 'Address', url)), el('div', { class: 'row' }, el('label', {}, 'Cited as', cite)),
      el('div', { class: 'row' }, el('button', { class: 'key', type: 'button', onclick: async () => {
        try { await api('/api/source', { method: 'POST', body: JSON.stringify({ record: S.record, id: id.value, kind: kind.value, cite: cite.value, url: url.value }) }); await reload(); } catch (error) { msg.textContent = error.message; }
      } }, 'Add'), msg)) : null,
    el('table', {}, el('tr', {}, el('th', {}, 'Id'), el('th', {}, 'Kind'), el('th', {}, 'Source'), el('th', {}, 'Assertions')),
      ...S.data.sources.map((s) => el('tr', {}, el('td', { class: 'mono' }, s.id), el('td', {}, s.kind), el('td', {}, s.cite, s.object ? el('div', { class: 'silent' }, `also an object of the record: ${S.by.get(s.object)?.name || s.object}`) : null), el('td', {}, used(s.id))))));
}

async function log(ctx, host) {
  const { el, api } = ctx;
  const rows = (await api('/api/log')).log.reverse();
  host.replaceChildren(el('p', { class: 'silent' }, 'What was done in this archive, newest first. The log is only ever added to.'),
    el('table', {}, el('tr', {}, el('th', {}, 'When'), el('th', {}, 'What'), el('th', {}, 'Of')),
      ...(rows.length ? rows.map((r) => { const { at, event, ...rest } = r; return el('tr', {}, el('td', { class: 'mono' }, at), el('td', {}, event), el('td', { class: 'mono' }, JSON.stringify(rest))); }) :
        [el('tr', {}, el('td', { colspan: 3, class: 'silent' }, 'Nothing has been done here yet.'))])));
}

async function verify(ctx, host) {
  const { S, el, api } = ctx;
  host.replaceChildren(el('p', { class: 'silent' }, 'Asking kategoria…'));
  const v = await api(`/api/verify?record=${encodeURIComponent(S.record)}`);
  host.replaceChildren(
    el('div', { class: `verdict${v.status === 'holds' ? '' : ' fails'}` }, el('b', {}, v.status === 'holds' ? 'It holds: ' : `It ${v.status}: `), `${v.claim.replace('TheRecord', 'the record')}. `, el('span', { class: 'silent' }, v.reason),
      el('div', { class: 'silent' }, `${v.objects} objects, ${v.relations} relations and ${v.emergences} emergences, written as sets and functions on the schema's category Record and checked by kategoria, whose code shares nothing with the reader's.`)),
    el('p', {}, el('a', { href: '/domains/history/schema/figures/record.svg', target: '_blank' }, 'The schema\'s diagram'), ' · ', el('a', { href: '/domains/history/schema/historical-objects.cat', target: '_blank' }, 'its script'), ' · ',
      el('a', { href: '/domains/history/schema/figures/README.md', target: '_blank' }, 'kategoria\'s answers on the schema itself')),
    el('h2', { class: 'caption' }, `Faults the reader found: ${S.data.faults.length}`),
    S.data.faults.length ? el('table', {}, el('tr', {}, el('th', {}, 'Line'), el('th', {}, 'Row'), el('th', {}, 'What is wrong')), ...S.data.faults.map((f) => el('tr', {}, el('td', {}, f.line || ''), el('td', { class: 'mono' }, f.row), el('td', {}, f.what)))) : el('p', { class: 'silent' }, 'None: every row stands.'),
    el('h2', { class: 'caption', style: 'margin-top:16px' }, 'The record as a package'),
    el('p', {}, el('a', { href: `/api/export?record=${encodeURIComponent(S.record)}` }, 'Take the record with the holdings of its objects'), ' as one bag: the data, a manifest of SHA-256 and what the bag is. Holdings that are dark stay behind.'));
}
