// The evidence (ADR 0121): the matter's documents as a reading room. At the left the documents, each kept with the digest of its
// bytes, an exhibit with its chain of custody as archeion follows it; in the middle the document's text, the passages that bear on
// the facts marked where they stand; at the right the facts, each with the passages for and against it. A passage is put forward by
// hand (select the words) or proposed by a language model, which is never the source: its passage is looked for in the document, and
// counts only when a person has read it and accepted it.

import { locate, segments } from '/visualization/js/evidence/index.js';
import { S, base, enc, ensure, keep, load, gloss, evaluatedFact } from './common.js';

export function evidenceView() {
  const state = { texts: new Map(), pick: null, busy: false };
  return { id: 'evidence', label: 'Evidence', draw: (pane, ctx) => draw(pane, ctx, state) };
}

async function textOf(ctx, state, name) {
  if (!state.texts.has(name)) {
    try { state.texts.set(name, (await ctx.api(`${base(ctx)}/documents/${enc(name)}`)).text); } catch { state.texts.set(name, null); }
  }
  return state.texts.get(name);
}

const STATUS = { accepted: { glyph: '●', word: 'accepted' }, proposed: { glyph: '◌', word: 'proposed, not yet read' }, rejected: { glyph: '✕', word: 'rejected' } };

async function draw(pane, ctx, state) {
  const { el, fill } = ctx;
  pane.classList.add('evidence-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the evidence');
  const m = S.matter, redraw = () => ctx.show('evidence');
  const docs = S.documents;
  const cands = (m.candidates || []);
  // Come for a fact, the document opened is one that speaks of it.
  if (!S.doc || !docs.some((d) => d.name === S.doc)) S.doc = cands.find((c) => c.proposition === S.fact && c.status !== 'rejected' && docs.some((d) => d.name === c.document))?.document || docs[0]?.name || null;
  const waiting = cands.filter((c) => c.status === 'proposed').length;
  ctx.note(`${docs.length} ${docs.length === 1 ? 'document' : 'documents'}, ${cands.filter((c) => (c.status || 'accepted') === 'accepted').length} passages accepted${waiting ? `, ${waiting} waiting to be read` : ''}`);
  const authenticity = new Map((S.evaluation?.evidence?.authenticity || []).map((a) => [a.exhibit, a.verdict]));
  const exhibitOf = (name) => (m.exhibits || []).find((x) => x.document === name) || null;

  // ---- the documents ----
  const file = el('input', { type: 'file', multiple: true, hidden: true, onchange: async (e) => {
    for (const f of e.target.files) {
      try {
        const a = await ctx.api(`${base(ctx)}/documents?name=${enc(f.name)}`, { method: 'POST', raw: f });
        ctx.log(`${a.name} kept: SHA-256 ${a.sha256.slice(0, 16)}…${a.text ? '' : ' No text could be read from it: a scan must be transcribed first.'}`);
        S.doc = a.name;
      } catch (error) { ctx.say(error.message); }
    }
    S.documents = (await ctx.api(`${base(ctx)}/documents`)).documents;
    redraw();
  } });
  const shelf = el('nav', { class: 'shelf', 'aria-label': 'The documents' },
    el('h3', {}, 'Documents'),
    docs.map((d) => {
      const x = exhibitOf(d.name), v = x ? authenticity.get(x.id) : null;
      const n = cands.filter((c) => c.document === d.name && c.status !== 'rejected').length;
      return el('button', { type: 'button', class: `doc${S.doc === d.name ? ' on' : ''}`, onclick: () => { S.doc = d.name; state.pick = null; redraw(); } },
        el('b', {}, d.name.replace(/\.[a-z0-9]+$/i, '')),
        el('span', { class: 'small' }, `${d.sha256 ? `sha256 ${d.sha256.slice(0, 10)}…` : 'no digest'}${n ? ` · ${n} ${n === 1 ? 'passage' : 'passages'}` : ''}${d.text ? '' : ' · no text'}`),
        x ? el('span', { class: `custody ${v?.status || ''}` }, v ? (v.status === 'holds' ? '✓ exhibit, authentic' : '✕ exhibit, not shown authentic') : 'an exhibit') : null);
    }),
    el('button', { type: 'button', class: 'key edged', onclick: () => file.click() }, 'Add documents'), file);

  // ---- the document read ----
  const text = S.doc ? await textOf(ctx, state, S.doc) : null;
  const here = cands.filter((c) => c.document === S.doc && c.status !== 'rejected');
  const marks = [];
  if (text) for (const c of here) { const at = locate(text, c.passage, c.at ?? null); if (at) marks.push({ id: c.id, start: at[0], end: at[1] }); }
  const byId = new Map(cands.map((c) => [c.id, c]));
  const page = el('article', { class: 'page', tabindex: 0 });
  if (text) {
    for (const s of segments(text, marks)) {
      if (!s.marks.length) { page.append(s.text); continue; }
      const cs = s.marks.map((id) => byId.get(id));
      const on = cs.some((c) => c.id === S.cand || c.proposition === S.fact);
      page.append(el('mark', { class: `${cs.some((c) => (c.status || 'accepted') === 'accepted') ? 'accepted' : 'proposed'}${cs.some((c) => c.negated) ? ' denies' : ''}${on ? ' on' : ''}`, 'data-candidate': cs[0].id,
        title: cs.map((c) => `${c.negated ? 'denies' : 'says'} ${c.proposition} (${STATUS[c.status || 'accepted'].word})`).join('; '),
        onclick: () => { S.cand = cs[0].id; S.fact = cs[0].proposition; redraw(); } }, s.text));
    }
    page.addEventListener('mouseup', () => {
      const sel = window.getSelection();
      const words = sel && !sel.isCollapsed && page.contains(sel.anchorNode) && page.contains(sel.focusNode) ? sel.toString().trim() : '';
      if (words.length >= 3 && words !== state.pick) { state.pick = words; redraw(); }
    });
  } else if (S.doc) page.append(el('p', { class: 'note' }, 'No text could be read from this document. A scan or a picture must be transcribed before its passages can be marked.'));
  else page.append(el('p', { class: 'note' }, 'Add the matter\'s documents: letters, logs, contracts, policies, records. Each is kept as it is, with the SHA-256 of its bytes.'));

  // A passage in hand: say what it is evidence of.
  const facts = m.facts || [];
  let pickBar = null;
  if (state.pick && text && locate(text, state.pick)) {
    const fact = el('select', { 'aria-label': 'The fact' }, facts.map((f) => el('option', { value: f.id, selected: f.id === S.fact }, f.id)));
    const sense = el('select', { 'aria-label': 'Whether it says or denies' }, el('option', { value: 'says' }, 'says'), el('option', { value: 'denies' }, 'denies'));
    const source = el('input', { type: 'text', placeholder: 'who says it', value: here[0]?.source || '' });
    pickBar = el('div', { class: 'pick' }, el('q', {}, state.pick.length > 140 ? `${state.pick.slice(0, 139)}…` : state.pick), el('span', {}, sense, ' ', fact, ', by ', source),
      el('button', { type: 'button', class: 'key main', disabled: !facts.length, onclick: async () => {
        const c = { id: `${S.doc}:${Date.now().toString(36)}`, proposition: fact.value, negated: sense.value === 'denies', source: source.value.trim() || S.doc, document: S.doc, passage: state.pick,
          grade: 'B', status: 'accepted', by: ctx.me?.name || 'counsel' };
        state.pick = null;
        await keep(ctx, (x) => { (x.candidates ||= []).push(c); }, 'marked a passage as evidence', `${c.negated ? 'against' : 'for'} ${c.proposition}, in ${S.doc}`);
        S.fact = c.proposition, S.cand = c.id;
        redraw();
      } }, 'Mark as evidence'),
      el('button', { type: 'button', class: 'key', onclick: () => { state.pick = null; redraw(); } }, 'Leave it'));
  }
  const ask = el('button', { type: 'button', class: 'key edged', disabled: !text || state.busy || !facts.length, title: 'A language model reads the document against the matter\'s facts. Each passage it proposes is looked for in the text; none counts until you accept it.',
    onclick: async () => {
      state.busy = true; redraw();
      ctx.log(`A model is reading ${S.doc} against ${facts.length} facts…`);
      try {
        const a = await ctx.api(`${base(ctx)}/recognise`, { method: 'POST', body: { document: S.doc }, run: true });
        ctx.log(`${a.by} proposed ${a.added.length} ${a.added.length === 1 ? 'passage' : 'passages'} in ${S.doc}, each found in its text${a.refused.length ? `; ${a.refused.length} it gave ${a.refused.length === 1 ? 'was' : 'were'} not in the document and ${a.refused.length === 1 ? 'was' : 'were'} dropped` : ''}. None counts until it is accepted.`);
        await load(ctx);
      } catch (error) { ctx.log(`The model could not read it: ${error.message}`, 'wrong'); ctx.say(error.message); }
      state.busy = false; redraw();
    } }, state.busy ? 'A model is reading…' : 'Have a model read it');
  const x = S.doc ? exhibitOf(S.doc) : null;
  const custody = x ? custodyOf(ctx, x, authenticity.get(x.id)) : S.doc ? offer(ctx, S.doc, docs.find((d) => d.name === S.doc), redraw) : null;
  const reading = el('section', { class: 'reading' },
    el('header', { class: 'reading-head' }, el('h2', {}, S.doc ? S.doc.replace(/\.[a-z0-9]+$/i, '') : 'The reading room'), el('span', { class: 'spacer' }), S.doc ? ask : null),
    pickBar, page, custody);

  // ---- the facts and their passages ----
  const review = async (c, status) => {
    try { await ctx.api(`${base(ctx)}/review`, { method: 'POST', body: { candidate: c.id, status } }); await load(ctx); ctx.log(`${c.id} ${status}: ${c.negated ? 'against' : 'for'} ${c.proposition}.`); } catch (error) { ctx.say(error.message); }
    redraw();
  };
  const recognised = new Map((S.evaluation?.evidence?.recognised || []).map((r) => [r.proposition, r]));
  const ledger = el('aside', { class: 'ledger', 'aria-label': 'The facts and their evidence' },
    el('h3', {}, 'Facts'),
    facts.length ? null : el('p', { class: 'small' }, 'The matter has no fact yet: enter them on the matter\'s page.'),
    facts.map((f) => {
      const e = evaluatedFact(f.id), r = recognised.get(f.id);
      const own = cands.filter((c) => c.proposition === f.id);
      return el('section', { class: `fact${S.fact === f.id ? ' on' : ''}` },
        el('button', { type: 'button', class: 'fact-head', onclick: () => { S.fact = S.fact === f.id ? null : f.id; redraw(); } },
          el('b', {}, f.id), el('span', { class: 'small' }, f.text || gloss(f.id)),
          el('span', { class: 'stand' }, `${e?.used || f.standing || 'alleged'}${e?.evidence ? ` · evidence ${e.evidence}` : ''}${r ? ` · ${r.standing}` : ''}`)),
        own.map((c) => {
          const st = STATUS[c.status || 'accepted'];
          return el('div', { class: `cand ${c.status || 'accepted'}${c.negated ? ' denies' : ''}${S.cand === c.id ? ' on' : ''}`, onclick: () => { S.cand = c.id; S.fact = f.id; if (c.document) S.doc = c.document; redraw(); } },
            el('p', { class: 'passage' }, el('span', { class: 'sense' }, c.negated ? 'denies' : 'says'), ' ', el('q', {}, c.passage || '(no passage)')),
            el('p', { class: 'small' }, `${c.source || 'no source'}${c.document ? `, in ${c.document.replace(/\.[a-z0-9]+$/i, '')}` : ''} · ${st.glyph} ${st.word}${c.by ? ` · put forward by ${c.by}` : ''}`),
            c.note ? el('p', { class: 'small' }, c.note) : null,
            el('p', { class: 'keys' },
              c.status === 'proposed' ? [el('button', { type: 'button', class: 'key main', onclick: (ev) => { ev.stopPropagation(); review(c, 'accepted'); } }, 'Accept'), el('button', { type: 'button', class: 'key', onclick: (ev) => { ev.stopPropagation(); review(c, 'rejected'); } }, 'Reject')]
                : el('button', { type: 'button', class: 'key', onclick: (ev) => { ev.stopPropagation(); review(c, 'proposed'); } }, 'Reopen')));
        }),
        r?.reason && own.length ? el('p', { class: 'small why' }, r.reason) : null);
    }));
  fill(pane, shelf, reading, ledger);
  pane.querySelector('mark.on')?.scrollIntoView({ block: 'center' });
}

// An exhibit's chain of custody, handover by handover, with archeion's verdict.
function custodyOf(ctx, x, verdict) {
  const { el } = ctx;
  const c = x.chain || {};
  return el('section', { class: 'custody-chain' },
    el('p', { class: 'exhibit-line' }, `Exhibit ${x.id}, offered to show `, el('code', {}, x.reports), `, produced by ${x.source}`),
    el('ol', {}, el('li', {}, el('time', {}, c.since || ''), ` taken in by ${c.custodian || '—'}`),
      (c.transfers || []).map((t) => el('li', {}, el('time', {}, t.when), ` ${t.from} → ${t.to}`, t.note ? el('span', { class: 'small' }, ` · ${t.note}`) : null))),
    verdict ? el('p', { class: `verdict ${verdict.status}` }, verdict.status === 'holds' ? '✓ ' : '✕ ', verdict.reason, verdict.status === 'holds' ? '' : '. What it shows is undercut by this finding until the break is explained.') : null);
}

// A document offered as an exhibit: what it is offered to show, who produced it, who took it in.
function offer(ctx, name, doc, redraw) {
  const { el } = ctx;
  const facts = S.matter.facts || [];
  if (!facts.length || !doc?.sha256) return null;
  const fact = el('select', {}, facts.map((f) => el('option', { value: f.id, selected: f.id === S.fact }, f.id)));
  const source = el('input', { type: 'text', placeholder: 'who produced it' }), keeper = el('input', { type: 'text', placeholder: 'who took it in' });
  const since = el('input', { type: 'text', placeholder: 'YYYY-MM-DD', value: new Date().toISOString().slice(0, 10), size: 10 });
  return el('details', { class: 'offer' }, el('summary', {}, 'Offer it as an exhibit'),
    el('p', {}, 'to show ', fact, ', produced by ', source, ', taken in by ', keeper, ' on ', since, ' ',
      el('button', { type: 'button', class: 'key main', onclick: async () => {
        const id = `x-${(S.matter.exhibits || []).length + 1}`;
        await keep(ctx, (m) => { (m.exhibits ||= []).push({ id, reports: fact.value, source: source.value.trim() || name, kind: 'document', text: name, document: name,
          identity: { what: name }, chain: { digest: doc.sha256, custodian: keeper.value.trim() || ctx.me?.name || 'counsel', since: since.value.trim(), transfers: [] } }); }, 'offered an exhibit', `${name}, to show ${fact.value}`);
        redraw();
      } }, 'Offer')));
}
