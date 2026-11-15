// What the Law Studio's views share (ADR 0121): the open project's matter, the scheme it is read under, its documents, and what the
// law engine made of them, kept once for every view and asked again whenever the matter changes.

export const enc = encodeURIComponent;

export const S = {
  project: null,
  matter: null, sha: null,
  scheme: null, schemeId: '', check: null, unfetched: [],
  evaluation: null, evaluating: null, wrong: '',
  documents: [], proposals: [], schemes: [], templates: [],
  chosen: null, chosenKey: null, form: null, actual: false, cand: null, issue: null, doc: null, fact: null, provision: null, term: null,
};

export const base = (ctx) => `/api/projects/${enc(ctx.name)}`;

export async function ensure(ctx) {
  if (S.project === ctx.name && S.matter) return;
  Object.assign(S, { project: ctx.name, matter: null, evaluation: null, scheme: null, schemeId: '', chosen: null, cand: null, issue: null, doc: null, fact: null, provision: null, term: null, wrong: '' });
  await load(ctx);
}

export async function load(ctx) {
  const [m, d, p, sc] = await Promise.all([
    ctx.api(`${base(ctx)}/matter`), ctx.api(`${base(ctx)}/documents`), ctx.api(`${base(ctx)}/proposals`), ctx.api('/api/schemes'),
  ]);
  S.matter = m.matter, S.sha = m.sha256, S.documents = d.documents, S.proposals = p.proposals, S.schemes = sc.schemes, S.templates = sc.templates;
  await loadScheme(ctx);
  await evaluate(ctx);
}

export async function loadScheme(ctx) {
  const id = S.matter?.scheme || '';
  if (!id) { S.scheme = null, S.schemeId = '', S.check = null, S.unfetched = []; return; }
  if (S.schemeId === id && S.scheme) return;
  const a = await ctx.api(`/api/schemes/${id.split('/').map(enc).join('/')}`);
  S.scheme = a.scheme, S.schemeId = id, S.check = a.check, S.unfetched = a.unfetched;
}

export async function evaluate(ctx) {
  if (!S.matter?.scheme || !(S.matter.issues || []).length) { S.evaluation = null; return; }
  try {
    const a = await ctx.api(`${base(ctx)}/evaluate`, { method: 'POST', body: {} });
    S.evaluation = a.evaluation, S.wrong = '';
  } catch (error) {
    S.evaluation = null, S.wrong = error.message;
  }
}

// Keep the matter after a change made to a copy of it, then ask the engine again. A matter changed meanwhile by someone else is read
// again and the change is not kept; the page says so.
export async function keep(ctx, change, what, note = '') {
  const m = JSON.parse(JSON.stringify(S.matter));
  change(m);
  try {
    const a = await ctx.api(`${base(ctx)}/matter`, { method: 'PUT', body: { matter: m, base: S.sha, what, note } });
    S.matter = m, S.sha = a.sha256;
  } catch (error) {
    ctx.say(`${error.message}: the matter is read again.`);
    await load(ctx);
    return false;
  }
  await loadScheme(ctx);
  await evaluate(ctx);
  ctx.log(`${what[0].toUpperCase()}${what.slice(1)}${note ? `: ${note}` : ''}.`);
  return true;
}

export const gloss = (id) => (S.scheme?.vocabulary || []).find((v) => v.id === id)?.gloss || (S.matter?.facts || []).find((f) => f.id === id)?.text || '';
export const factOf = (id) => (S.matter?.facts || []).find((f) => f.id === id) || null;
export const evaluatedFact = (id) => (S.evaluation?.facts || []).find((f) => f.id === id) || null;
export const answerOf = (issue) => (S.evaluation?.issues || []).find((a) => a.issue === issue) || null;

export const CLASS = {
  clear: { glyph: '✓', word: 'clear' }, arguable: { glyph: '◐', word: 'arguable' }, untenable: { glyph: '✕', word: 'untenable' },
  'without an admissible interpretation': { glyph: '∅', word: 'no admissible reading' },
};
export const classOf = (c) => CLASS[c] || { glyph: '·', word: c || 'not asked' };
export const STANDINGS = ['alleged', 'admitted', 'proved', 'found', 'noticed', 'presumed', 'deemed', 'not proved', 'rebutted'];
export const GLYPH = { final: '◆', found: '◆', admitted: '■', noticed: '■', deemed: '■', proved: '●', presumed: '◒', alleged: '○', 'not proved': '✕', rebutted: '✕' };

// A provision of the scheme, with its instrument.
export function provisionOf(id) {
  for (const ins of S.scheme?.instruments || []) for (const p of ins.provisions || []) if (p.id === id) return { ...p, instrument: ins };
  return null;
}

// The rules, exceptions and counts-as links that cite a provision.
export function citing(id) {
  const out = [];
  for (const kind of ['rules', 'exceptions', 'counts']) for (const x of S.scheme?.[kind] || []) if ((x.cites || []).includes(id)) out.push({ kind, ...x });
  return out;
}

// A class word with its glyph, as an element.
export const verdictMark = (el, c) => el('span', { class: `cls ${String(c || '').split(' ')[0]}` }, `${classOf(c).glyph} ${classOf(c).word}`);
