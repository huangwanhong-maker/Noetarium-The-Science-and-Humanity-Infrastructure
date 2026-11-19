// What Multilateral Phronimos's views share (ADR 0132): the open project's record, the date it is read on, and what the multilateral
// engine made of it on that date, kept once for every view and asked again whenever the record or the date changes; what is chosen,
// so that a party chosen on the round is the party opened in the ledger.

export const enc = encodeURIComponent;

export const S = {
  project: null,
  record: null, sha: null,
  at: '',               // the date read on ('' : every tie, whenever it held)
  answer: null,         // {undertaking, partnership}
  wrong: '',
  templates: [], rules: [],
  chosen: null,         // {kind: 'participant' | 'tie' | 'knot' | 'commitment' | 'party' | 'decision', id, …}
  layers: null,         // the layers drawn on the round (null: all)
  rule: 0,              // the rule drawn on the governance view
  question: 0,          // the question decided under a declared rule, drawn on governance (ADR 0134)
  pair: null,           // the pair whose events are drawn on the round (ADR 0134; null: the busiest)
};

export const base = (ctx) => `/api/projects/${enc(ctx.name)}`;

export async function ensure(ctx) {
  if (S.project === ctx.name && S.record) return;
  Object.assign(S, { project: ctx.name, record: null, sha: null, answer: null, chosen: null, layers: null, rule: 0, question: 0, pair: null, wrong: '' });
  await load(ctx);
}

export async function load(ctx) {
  const [r, t] = await Promise.all([ctx.api(`${base(ctx)}/record`), ctx.api('/api/templates')]);
  S.record = r.record, S.sha = r.sha256, S.templates = t.templates, S.rules = t.rules;
  S.at = S.record.at || '';
  await readAt(ctx);
}

export const begun = () => !!(S.record?.undertaking?.participants?.length || S.record?.partnership?.members?.length);

export async function readAt(ctx, at = S.at) {
  S.answer = null, S.wrong = '';
  if (!begun()) return;
  try {
    S.answer = await ctx.api(`${base(ctx)}/reading`, { method: 'POST', body: { at } });
    S.at = at;
  } catch (error) {
    S.wrong = error.message;
  }
}

// Keep the record after a change made to a copy of it, then read it again. A record changed meanwhile by someone else is read again
// and the change is not kept; the page says so.
export async function keep(ctx, change, what, note = '') {
  const r = JSON.parse(JSON.stringify(S.record));
  change(r);
  try {
    const a = await ctx.api(`${base(ctx)}/record`, { method: 'PUT', body: { record: r, base: S.sha, what, note } });
    S.record = r, S.sha = a.sha256;
  } catch (error) {
    ctx.say(`${error.message}: the record is read again.`);
    await load(ctx);
    return false;
  }
  await readAt(ctx);
  ctx.log(`${what[0].toUpperCase()}${what.slice(1)}${note ? `: ${note}` : ''}.`);
  return true;
}

// ---- Reading what the record holds and the engine answered ------------------------------------------------------------------------

export const name = (id) => S.record?.names?.[id] || S.record?.undertaking?.participants?.find((p) => p.id === id)?.name || id;
export const names = () => {
  const out = { ...(S.record?.names || {}) };
  for (const p of S.record?.undertaking?.participants || []) out[p.id] ??= p.name || p.id;
  return out;
};
export const U = () => S.answer?.undertaking || null;
export const P = () => S.answer?.partnership || null;
export const words = (s) => String(s ?? '').replace(/_/g, ' ');

// Every date the record holds, in order: ties begun and ended, decisions, designs of observation, commitments, reports, contributions.
export function dates() {
  const out = new Set();
  const add = (d) => { if (/^\d{4}-\d{2}-\d{2}$/.test(d || '')) out.add(d); };
  const u = S.record?.undertaking || {}, p = S.record?.partnership || {};
  for (const t of u.ties || []) add(t.since), add(t.until);
  for (const d of u.decisions || []) add(d.when);
  for (const o of u.observations || []) add(o.since), add(o.until);
  for (const c of p.commitments || []) add(c.made), add(c.due);
  for (const r of p.reports || []) add(r.when);
  for (const c of p.contributions || []) add(c.when);
  add(S.record?.at);
  return [...out].sort();
}

// The diagnostics' kinds as the monograph names its failures (Table 10.2), each with what it asks of the record.
export const DIAGNOSES = {
  evidence: { title: 'Evidential gaps', asks: 'a decision with the evidence it rests on' },
  authorisation: { title: 'Authorisation', asks: 'a decision made by someone who holds authority (a recommendation is not a decision)' },
  participation: { title: 'Participation', asks: 'a venue where a decision may be contested, and a route to it for everyone it affects' },
  'AI dependency': { title: 'AI dependency', asks: 'evidence beyond artificial systems, each source counted once' },
  succession: { title: 'Succession', asks: 'relations of authority that do not hang on one participant or one tie' },
  concentration: { title: 'Concentration', asks: 'reliance spread across sources' },
  unobserved: { title: 'What is not observed', asks: 'a design of observation, so that an absence can be known' },
};
export const SIGNS = {
  'paper partnership': 'registered, and nothing contributed or reported for a long while',
  drift: 'contributions serving no declared goal',
  'resource fragility': 'one party giving most of a kind of support, or a commitment with nothing secured',
  capture: 'the goals set by one party while others set none',
  'collaborative inertia': 'reports that do not move toward their targets',
  'accountability gap': 'a commitment past its date with nothing reported, or with nothing to measure it by',
  'succession loss': 'a commitment held by a party that has left, taken over by no one',
};

// A verdict's status as a glyph and a word; never a colour alone.
export const STATUS = { holds: { glyph: '✓', word: 'holds' }, fails: { glyph: '✕', word: 'fails' }, undecided: { glyph: '◐', word: 'undecided' } };
export const statusOf = (v) => STATUS[v?.status] || { glyph: '·', word: v?.status || '' };

export const closeKey = (el, onclick) => el('button', { type: 'button', class: 'close', 'aria-label': 'Close', onclick: (e) => { e.stopPropagation(); onclick(); } }, '×');

// A small form: fields declared, a key that keeps.
export function form(el, fields, label, onsubmit) {
  const inputs = {};
  const rows = fields.map((f) => {
    let input;
    if (f.options) input = el('select', { 'aria-label': f.label }, f.options.map(([v, w]) => el('option', { value: v, selected: v === f.value }, w)));
    else if (f.many) input = el('select', { multiple: true, size: Math.min(5, f.many.length), 'aria-label': f.label }, f.many.map(([v, w]) => el('option', { value: v }, w)));
    else input = el('input', { type: f.type || 'text', value: f.value ?? '', placeholder: f.placeholder || '', step: f.type === 'number' ? 'any' : undefined, 'aria-label': f.label });
    inputs[f.key] = input;
    return el('label', { class: 'f-row' }, el('span', {}, f.label), input);
  });
  const go = el('button', { type: 'button', class: 'key main', onclick: () => {
    const v = {};
    for (const f of fields) {
      const i = inputs[f.key];
      v[f.key] = f.many ? [...i.selectedOptions].map((o) => o.value) : f.type === 'number' ? (i.value === '' ? null : Number(i.value)) : f.type === 'checkbox' ? i.checked : i.value.trim();
    }
    onsubmit(v);
  } }, label);
  return el('div', { class: 'f-form' }, rows, el('div', { class: 'keys' }, go));
}

export const today = () => new Date().toISOString().slice(0, 10);

// Draw a view again after something it awaited, only while it is still the view in front: a person who moved to another view meanwhile
// is not pulled back.
export function again(ctx, id) {
  return () => { if (document.querySelector('.tab.on')?.dataset.tab === id) ctx.show(id); };
}
