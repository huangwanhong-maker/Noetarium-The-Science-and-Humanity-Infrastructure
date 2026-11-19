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

// ---- Theories of the case (ADR 0127) ----
// The search's last report (the engine's answer without its state), kept for the views that draw it; the theory chosen, the instant
// of the plane and the way the cube is turned are the page's own.
export const T = { project: null, report: null, begun: false, proposals: [], chosen: null, at: null, azimuth: -0.62, elevation: 0.48, zoom: 2.2, above: false, busy: false, item: null };

export async function loadTheories(ctx, force = false) {
  if (T.project === ctx.name && T.report && !force) return;
  const a = await ctx.api(`${base(ctx)}/theories`);
  Object.assign(T, { project: ctx.name, report: a.report, begun: a.begun, proposals: a.proposals || [] });
  if (!T.report && S.matter?.located?.length) await runTheories(ctx, { generations: 0 }, 'laid out the evidence in space and time');
}

export async function runTheories(ctx, body, words) {
  T.busy = true;
  try {
    const a = await ctx.api(`${base(ctx)}/theories/${body.reset ? 'reset' : body.model ? 'model' : body.text ? 'propose' : 'run'}`, { method: 'POST', body, run: !!body.model });
    if (body.reset) { T.report = null, T.begun = false, T.chosen = null; await loadTheories(ctx, true); }
    else if (a.read === false) return a;
    else T.report = a.report || T.report, T.begun = true;
    if (words) ctx.log(`${words[0].toUpperCase()}${words.slice(1)}.`);
    return a;
  } finally {
    T.busy = false;
  }
}

// The theory chosen, or the most plausible kept, or none.
export const chosenTheory = () => (T.report?.kept || []).find((t) => t.serial === T.chosen) || null;
export const personOf = (id) => (T.report?.evidence?.agents || []).find((a) => a.id === id) || null;
export const nameOf = (id) => personOf(id)?.name || id;

// ---- Theories of the law (ADR 0133) ----
// The search's last report for doctrines, kept for the view that draws it; the doctrine chosen and the case looked at are the
// page's own.
export const D = { project: null, report: null, begun: false, proposals: [], chosen: null, item: null, busy: false };

// Whether the matter's scheme gives the law anything to read as doctrines: decided cases or precedents by factors.
export const hasCaseLaw = () => !!((S.scheme?.decided || []).length || (S.scheme?.precedents || []).length);

export async function loadDoctrines(ctx, force = false) {
  if (D.project === ctx.name && D.report && !force) return;
  const a = await ctx.api(`${base(ctx)}/doctrines`);
  Object.assign(D, { project: ctx.name, report: a.report, begun: a.begun, proposals: a.proposals || [], chosen: D.project === ctx.name ? D.chosen : null });
  if (!D.report && hasCaseLaw()) await runDoctrines(ctx, { generations: 0 }, 'set out the law of the matter as doctrines');
}

export async function runDoctrines(ctx, body, words) {
  D.busy = true;
  try {
    const a = await ctx.api(`${base(ctx)}/doctrines/${body.reset ? 'reset' : body.model ? 'model' : body.text ? 'propose' : 'run'}`, { method: 'POST', body, run: !!body.model });
    if (body.reset) { D.report = null, D.begun = false, D.chosen = null; await loadDoctrines(ctx, true); }
    else if (a.read === false) return a;
    else D.report = a.report || D.report, D.begun = true;
    if (words) ctx.log(`${words[0].toUpperCase()}${words.slice(1)}.`);
    return a;
  } finally {
    D.busy = false;
  }
}

// The doctrine chosen, or none.
export const chosenDoctrine = () => (D.report?.kept || []).find((d) => d.serial === D.chosen) || null;
