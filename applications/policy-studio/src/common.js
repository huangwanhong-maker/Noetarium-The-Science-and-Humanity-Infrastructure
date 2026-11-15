// What the Policy Studio's views share (ADR 0122): the open project's question, the theory as its written form reads, and what the
// policy engine made of the question, kept once for every view and asked again whenever the question changes.

import { figure } from '/visualization/js/policy/index.js';

export const enc = encodeURIComponent;

export const S = {
  project: null,
  question: null, sha: null,
  read: null,           // the theory as its written form reads: {ok, errors, theory}
  appraisal: null, wrong: '',
  schemes: [], templates: [], drafts: [],
  chosen: null,         // on the theory: "n:<id>" or "l:<index>"
  written: false,       // the theory shown in its written form
  body: null, study: null, option: null,
};

export const base = (ctx) => `/api/projects/${enc(ctx.name)}`;

export async function ensure(ctx) {
  if (S.project === ctx.name && S.question) return;
  Object.assign(S, { project: ctx.name, question: null, read: null, appraisal: null, chosen: null, written: false, body: null, study: null, option: null, wrong: '' });
  await load(ctx);
}

export async function load(ctx) {
  const [q, sc, d] = await Promise.all([ctx.api(`${base(ctx)}/question`), ctx.api('/api/schemes'), ctx.api(`${base(ctx)}/drafts`)]);
  S.question = q.question, S.sha = q.sha256, S.schemes = sc.schemes, S.templates = sc.templates, S.drafts = d.drafts;
  await appraise(ctx);
}

export const begun = () => !!String(S.question?.theory || '').trim();

export async function appraise(ctx) {
  S.read = null, S.appraisal = null, S.wrong = '';
  if (!begun()) return;
  try {
    const [r, a] = await Promise.all([ctx.api('/api/theory', { method: 'POST', body: { text: S.question.theory } }), ctx.api(`${base(ctx)}/appraise`, { method: 'POST', body: {} })]);
    S.read = r, S.appraisal = a.appraisal;
  } catch (error) {
    S.wrong = error.message;
  }
}

// Keep the question after a change made to a copy of it, then ask the engine again. A question changed meanwhile by someone else
// is read again and the change is not kept; the page says so.
export async function keep(ctx, change, what, note = '') {
  const q = JSON.parse(JSON.stringify(S.question));
  change(q);
  try {
    const a = await ctx.api(`${base(ctx)}/question`, { method: 'PUT', body: { question: q, base: S.sha, what, note } });
    S.question = q, S.sha = a.sha256;
  } catch (error) {
    ctx.say(`${error.message}: the question is read again.`);
    await load(ctx);
    return false;
  }
  await appraise(ctx);
  ctx.log(`${what[0].toUpperCase()}${what.slice(1)}${note ? `: ${note}` : ''}.`);
  return true;
}

// ---- Reading what the engine answered -------------------------------------------------------------------------------------------

export const theory = () => (S.appraisal?.theory?.ok ? S.appraisal.theory : null);
export const levels = () => S.appraisal?.scheme?.levels || theory()?.levels || [];
export const nodeText = (id) => theory()?.nodes.find((n) => n.id === id)?.text || id;
export const bodyAnswer = (id) => (S.appraisal?.bodies || []).find((b) => b.id === id) || null;
export const bodyAsked = (id) => (S.question?.bodies || []).find((b) => b.id === id) || null;
export const bodyOn = (link) => (link ? theory()?.bodies?.[`${link.from} -> ${link.to}`] || null : null);
export const isRatio = (measure) => measure === 'log_risk_ratio' || measure === 'log_odds_ratio';
export const MEASURES = {
  as_given: 'an effect and its variance, as given', log_risk_ratio: 'a risk ratio, from 2 × 2 counts', log_odds_ratio: 'an odds ratio, from 2 × 2 counts', risk_difference: 'a risk difference, from 2 × 2 counts',
};
export const words = (s) => String(s ?? '').replace(/_/g, ' ');

// An effect as it is read: a logarithm of a ratio is told as the ratio.
export const effect = (v, ratio) => figure(ratio ? Math.exp(v) : v);
export const interval = (e, lo, hi, ratio) => `${effect(e, ratio)} [${effect(lo, ratio)}, ${effect(hi, ratio)}]`;
export const money = (v) => (Number.isFinite(v) ? Math.round(v).toLocaleString('en-GB') : '—');
export const percent = (v) => (Number.isFinite(v) ? `${figure(100 * v)}%` : '—');

// A verdict's status as a glyph and a word; never a colour alone.
export const STATUS = { holds: { glyph: '✓', word: 'shown' }, fails: { glyph: '✕', word: 'not so' }, undecided: { glyph: '◐', word: 'not yet shown' } };
export const statusOf = (v) => STATUS[v?.status] || { glyph: '·', word: v?.status || '' };

// A number field that keeps on change.
export function numberIn(el, value, onchange, attrs = {}) {
  return el('input', { type: 'number', value: Number.isFinite(value) ? value : '', step: 'any', ...attrs, onchange: (e) => { const v = e.target.value === '' ? null : Number(e.target.value); if (v === null || Number.isFinite(v)) onchange(v); } });
}

export const closeKey = (el, onclick) => el('button', { type: 'button', class: 'close', 'aria-label': 'Close', onclick: (e) => { e.stopPropagation(); onclick(); } }, '×');

// The designs a scheme starts, read from its text (the engine answers them too, once the scheme reads).
export const designs = () => Object.keys(S.appraisal?.scheme?.start || {});
