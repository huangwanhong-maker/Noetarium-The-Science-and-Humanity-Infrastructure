// What is kept (ADR 0123): the archive's holdings, each a file as it was taken in, with the SHA-256 of its bytes then and now and
// its chain of custody drawn handover by handover, as archeion follows it; a file dropped here is taken in; a holding is handed over
// by writing who takes it. Beneath, the sources: each a kind of report with its channel, declared or counted.

import { E, base, ensure, within, load, keep, keptHolding, pct, fig, when, closeKey } from './_shared.js';

export function view() {
  return {
    id: 'holdings', label: 'Holdings', draw,
    heard: (e, ctx) => { if (/evidence|holding|example|handed|source|archive/.test(e.what)) { E.archive = null; if (document.querySelector('.tab.on')?.dataset.tab === 'holdings') ctx.show('holdings'); } },
  };
}

const size = (n) => (n >= 1 << 20 ? `${(n / (1 << 20)).toFixed(1)} MB` : n >= 1024 ? `${Math.round(n / 1024)} kB` : `${n} B`);

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('ev-view', 'ev-holdings');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the holdings');
  const redraw = () => within(ctx, 'holdings');
  const k = E.appraisal?.keep;
  const holdings = E.archive?.holdings || [];
  const counts = new Map();
  for (const i of E.archive?.items || []) if (i.holding) counts.set(i.holding, (counts.get(i.holding) || 0) + 1);

  const input = el('input', { type: 'file', multiple: true, hidden: true, onchange: (e) => takeIn(ctx, [...e.target.files]) });
  const drop = el('div', { class: 'ev-drop', tabindex: 0, role: 'button', onclick: () => input.click(), onkeydown: (e) => { if (e.key === 'Enter') input.click(); } },
    el('b', {}, 'Take a file in'), el('span', {}, 'Drop it here, or choose it. It is kept as it is, with the SHA-256 of its bytes and you as its first custodian.'), input);
  drop.addEventListener('dragover', (e) => { e.preventDefault(); e.stopPropagation(); drop.classList.add('over'); });
  drop.addEventListener('dragleave', () => drop.classList.remove('over'));
  drop.addEventListener('drop', (e) => { e.preventDefault(); e.stopPropagation(); drop.classList.remove('over'); takeIn(ctx, [...e.dataTransfer.files]); });

  const rows = holdings.map((h) => holdingRow(ctx, h, keptHolding(h.id), counts.get(h.id) || 0, redraw));
  fill(pane,
    el('header', { class: 'ev-head' }, el('h2', {}, 'What is kept'),
      k ? el('p', { class: `ev-said ${k.verdict.status}` }, el('b', {}, k.verdict.status === 'holds' ? '✓ Intact. ' : k.verdict.status === 'fails' ? '✕ Not all of it can be relied on. ' : '◐ Not all of it was observed. '), `${k.verdict.reason[0].toUpperCase()}${k.verdict.reason.slice(1)}. ${k.sound_items} of ${k.items_in_all} items rest on what is sound.`) : null),
    drop,
    rows.length ? el('ol', { class: 'ev-register' }, rows) : el('p', { class: 'note' }, 'Nothing is kept yet.'),
    sources(ctx, redraw));
  if (E.holding) pane.querySelector(`[data-holding="${CSS.escape(E.holding)}"]`)?.scrollIntoView({ block: 'center' });
}

async function takeIn(ctx, files) {
  for (const file of files) {
    try {
      const r = await ctx.fetch(`${base(ctx)}/holdings?name=${encodeURIComponent(file.name)}`, { method: 'POST', body: file });
      const a = await r.json();
      if (!r.ok) throw new Error(a.message || r.statusText);
      ctx.log(`Took ${a.holding.name} in: SHA-256 ${a.holding.digest.slice(0, 16)}…, held by ${a.holding.custodian}.`);
      E.holding = a.holding.id;
    } catch (error) { ctx.say(`${file.name}: ${error.message}`); }
  }
  await load(ctx);
  within(ctx, 'holdings');
}

function holdingRow(ctx, h, kh, items, redraw) {
  const { el } = ctx;
  const steps = [{ who: h.custodian || '—', when: h.since, digest: h.digest, note: 'taken in' }, ...(h.transfers || []).map((t) => ({ who: t.to, when: t.when, digest: t.digest, from: t.from, note: t.note }))];
  const broken = new Map((kh?.breaks || []).map((b) => [b.at, b.what]));
  const fixity = kh?.fixity || 'not observed';
  const to = el('input', { type: 'text', placeholder: 'who takes it', 'aria-label': `Who takes ${h.name}` });
  const note = el('input', { type: 'text', placeholder: 'why, or how (optional)', 'aria-label': 'A note on the handover' });
  const hand = async () => {
    if (!to.value.trim()) return ctx.say('Say who takes it.');
    try {
      await ctx.api(`${base(ctx)}/transfer`, { method: 'POST', body: { holding: h.id, to: to.value.trim(), note: note.value.trim() } });
      ctx.log(`${h.name} handed over to ${to.value.trim()}.`);
      await load(ctx);
    } catch (error) { ctx.say(error.message); }
    redraw();
  };
  return el('li', { class: `ev-holding${kh && !kh.sound ? ' unsound' : ''}${E.holding === h.id ? ' on' : ''}`, 'data-holding': h.id },
    el('div', { class: 'ev-holding-head' },
      el('a', { class: 'ev-holding-name', href: ctx.keyed(`${base(ctx)}/holdings/${encodeURIComponent(h.name || '')}`), target: '_blank', rel: 'noopener' }, h.name || h.id),
      el('span', { class: 'small' }, [h.bytes ? size(h.bytes) : null, `${items} ${items === 1 ? 'item rests' : 'items rest'} on it`].filter(Boolean).join(' · ')),
      el('span', { class: `ev-fixity ${fixity === 'intact' ? 'sound' : fixity === 'not observed' ? '' : 'unsound'}` }, fixity === 'intact' ? '✓ intact' : fixity === 'altered' ? '✕ altered' : fixity === 'missing' ? '✕ missing' : fixity === 'no digest' ? '✕ no digest' : '◌ not observed')),
    h.attributes?.title ? el('p', { class: 'ev-holding-title' }, h.attributes.title, h.attributes.creator ? el('span', { class: 'small' }, ` — ${h.attributes.creator}`) : null) : null,
    el('p', { class: 'ev-digest' }, el('code', { title: h.digest }, `SHA-256 ${String(h.digest || '').slice(0, 24)}…`), kh?.digest && kh.digest !== h.digest ? el('code', { title: kh.digest }, ` → ${kh.digest.slice(0, 24)}… by its last handover`) : null),
    el('ol', { class: 'ev-chain', 'aria-label': 'Its chain of custody' }, steps.map((s, n) => el('li', { class: broken.has(n) ? 'broken' : '' },
      el('span', { class: 'node' }), el('b', {}, s.who), el('time', {}, when(s.when)),
      n ? el('span', { class: 'small' }, s.digest === steps[n - 1].digest ? 'the same bytes' : 'its digest changed') : el('span', { class: 'small' }, 'taken in'),
      s.note && n ? el('span', { class: 'small' }, ` — ${s.note}`) : null, broken.has(n) ? el('span', { class: 'wrong' }, ` ✕ ${broken.get(n)}`) : null))),
    kh ? el('p', { class: `ev-verdict ${kh.sound ? 'sound' : 'unsound'}` }, kh.sound ? `✓ ${kh.authentic?.reason || kh.custody.reason}.` : `✕ ${kh.why}.`) : null,
    el('p', { class: 'ev-hand' }, 'Hand it over: ', to, ' ', note, ' ', el('button', { type: 'button', class: 'key edged', onclick: hand }, 'Hand over')));
}

function sources(ctx, redraw) {
  const { el } = ctx;
  const ss = E.archive?.sources || [];
  const used = new Map();
  for (const i of E.archive?.items || []) used.set(i.source, (used.get(i.source) || 0) + 1);
  const num = (v, onchange, label) => el('input', { type: 'number', step: 'any', min: 0, max: 1, value: v ?? '', 'aria-label': label, onchange: (e) => { const x = Number(e.target.value); if (e.target.value !== '' && x >= 0 && x <= 1) onchange(x); } });
  const set = (id, f, what) => keep(ctx, (a) => f(a.sources.find((s) => s.id === id)), 'changed a source\'s channel', what).then(redraw);
  const add = () => {
    const ids = new Set(ss.map((s) => s.id));
    let n = 1;
    while (ids.has(`source-${n}`)) n++;
    keep(ctx, (a) => { (a.sources ||= []).push({ id: `source-${n}`, name: 'A kind of report', hit: 0.7, false_alarm: 0.1 }); }, 'declared a source', `source-${n}`).then(redraw);
  };
  return el('section', { class: 'ev-sources' }, el('h3', {}, 'Who says what: the sources'),
    el('p', { class: 'small' }, 'A source\'s channel is how often it reports the thing where it is, and where it is not. The force of a report follows from both: a report is the hit rate over the false-alarm rate as likely where the thing is. Count a channel with the tool “A kind of report\'s channel, counted”, or declare it and say so.'),
    el('table', { class: 'ev-acts' }, el('thead', {}, el('tr', {}, ['Source', 'Reports where it is', 'Where it is not', 'A report weighs', 'Items', 'Said of it'].map((h) => el('th', {}, h)))),
      el('tbody', {}, ss.map((s) => el('tr', {},
        el('td', {}, el('input', { type: 'text', value: s.name || s.id, 'aria-label': `The name of ${s.id}`, onchange: (e) => set(s.id, (x) => { x.name = e.target.value.trim(); }, `${s.id} renamed`) }), el('small', {}, ` ${s.id}`)),
        el('td', {}, s.hit !== undefined ? num(s.hit, (v) => set(s.id, (x) => { x.hit = v; }, `${s.id}: hit ${v}`), `How often ${s.id} reports the thing where it is`) : 'by its attributes'),
        el('td', {}, s.false_alarm !== undefined ? num(s.false_alarm, (v) => set(s.id, (x) => { x.false_alarm = v; }, `${s.id}: false alarm ${v}`), `How often ${s.id} reports it where it is not`) : ''),
        el('td', { class: 'num' }, s.hit !== undefined && s.false_alarm > 0 ? `×${fig(s.hit / s.false_alarm, 3)}` : '—'),
        el('td', { class: 'num' }, `${used.get(s.id) || 0}`),
        el('td', { class: 'small' }, s.note || ''))))),
    el('button', { type: 'button', class: 'key edged', onclick: add }, '+ a source'));
}
