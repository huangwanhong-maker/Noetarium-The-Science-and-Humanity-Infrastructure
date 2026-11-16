// What the views of evidence in space and time share (ADR 0123): the open project's archive of evidence, what the engine made of
// it, and the place it is about, kept once for both views and asked again whenever the archive changes.

export const enc = encodeURIComponent;

export const E = {
  project: null,
  archive: null, sha: null, kept: false, cases: [],
  appraisal: null, wrong: '',
  place: null, placeFor: null,
  selected: null,        // "c:<claim>" or "i:<item>"
  mode: 'weigh',         // what the claims are toned by: "weigh" (their evidence) or "decide" (what to do)
  vb: null,              // the map's view box [x, y, w, h], in metres
  placing: false,        // the next click on the map places a claim
  holding: null,         // the holding opened on the Holdings view
};

export const base = (ctx) => `/api/projects/${enc(ctx.name)}/evidence`;

// The archive of the open project, read when it is not yet; and, when a view is shown afresh (not redrawn from within), appraised
// again, so that a file changed since it was last used is found changed: holdings are checked whenever they are used.
export async function ensure(ctx) {
  const inner = E.inner;
  E.inner = false;
  if (E.project !== ctx.name || !E.archive) {
    Object.assign(E, { project: ctx.name, archive: null, appraisal: null, place: null, placeFor: null, selected: null, vb: null, placing: false, holding: null, wrong: '' });
    await load(ctx);
  } else if (!inner) {
    await appraise(ctx);
  }
}

// A view drawn again from within itself (a click, a change kept): what it shows was appraised a moment ago.
export function within(ctx, view) {
  E.inner = true;
  ctx.show(view);
}

export async function load(ctx) {
  const a = await ctx.api(base(ctx));
  Object.assign(E, { archive: a.archive, sha: a.sha256, kept: a.kept, cases: a.cases || [] });
  await appraise(ctx);
}

export async function appraise(ctx) {
  if (!E.kept) { E.appraisal = null; return; }
  try {
    E.appraisal = (await ctx.api(`${base(ctx)}/appraise`, { method: 'POST', body: {} })).appraisal;
    E.wrong = '';
  } catch (error) {
    E.appraisal = null, E.wrong = error.message;
  }
}

// The place, asked once for an archive's box.
export async function place(ctx) {
  const key = JSON.stringify([E.archive?.basemap || '', E.archive?.frame?.box || null, (E.archive?.items || []).length, (E.archive?.claims || []).length]);
  if (E.placeFor === key) return E.place;
  try { E.place = await ctx.api(`${base(ctx)}/place`); } catch (error) { E.place = null; ctx.log(`The place could not be drawn: ${error.message}`, 'wrong'); }
  E.placeFor = key;
  return E.place;
}

// Keep the archive after a change made to a copy of it, then ask the engine again. An archive changed meanwhile by someone else is
// read again and the change is not kept; the page says so.
export async function keep(ctx, change, what, note = '') {
  const a = JSON.parse(JSON.stringify(E.archive));
  change(a);
  try {
    const r = await ctx.api(base(ctx), { method: 'PUT', body: { archive: a, base: E.sha, what, note } });
    Object.assign(E, { archive: a, sha: r.sha256, kept: true });
  } catch (error) {
    ctx.say(`${error.message}: the archive is read again.`);
    await load(ctx);
    return false;
  }
  await appraise(ctx);
  ctx.log(`${what[0].toUpperCase()}${what.slice(1)}${note ? `: ${note}` : ''}.`);
  return true;
}

export const weighed = (id) => (E.appraisal?.weigh?.claims || []).find((c) => c.id === id) || null;
export const decided = (id) => (E.appraisal?.decide?.claims || []).find((c) => c.claim === id) || null;
export const itemOf = (id) => (E.archive?.items || []).find((i) => i.id === id) || null;
export const claimOf = (id) => (E.archive?.claims || []).find((c) => c.id === id) || null;
export const holdingOf = (id) => (E.archive?.holdings || []).find((h) => h.id === id) || null;
export const keptHolding = (id) => (E.appraisal?.keep?.holdings || []).find((h) => h.id === id) || null;
export const keptItem = (id) => (E.appraisal?.keep?.items || []).find((i) => i.id === id) || null;
export const sourceOf = (id) => (E.archive?.sources || []).find((s) => s.id === id) || null;

export const pct = (p, digits = 0) => (p === null || p === undefined || !Number.isFinite(p) ? '—' : `${(100 * p).toFixed(p > 0.995 || p < 0.005 ? Math.max(1, digits) : digits)}%`);
export const fig = (x, d = 3) => (x === null || x === undefined || !Number.isFinite(x) ? '—' : (+x.toPrecision(d)).toLocaleString('en-GB', { maximumSignificantDigits: d }));
export const metres = (m) => (m === null || m === undefined || !Number.isFinite(m) ? '—' : m >= 1000 ? `${(m / 1000).toFixed(m >= 10000 ? 0 : 1)} km` : `${Math.round(m)} m`);
const MONTHS = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
export const when = (t) => {
  if (t === null || t === undefined || t === '') return '';
  const coarse = String(t).match(/^(\d{4})(?:-(\d{2}))?$/);   // a year, or a month, is said as one
  if (coarse) return coarse[2] ? `${MONTHS[Number(coarse[2]) - 1]} ${coarse[1]}` : coarse[1];
  const d = new Date(typeof t === 'number' ? t * 1000 : Date.parse(/z|[+-]\d{2}:?\d{2}$/i.test(String(t)) ? t : `${String(t).replace(' ', 'T')}Z`));
  if (Number.isNaN(d.getTime())) return String(t);
  const day = d.toLocaleDateString('en-GB', { day: 'numeric', month: 'short', year: 'numeric', timeZone: 'UTC' });
  return d.getUTCHours() || d.getUTCMinutes() ? `${day}, ${d.toLocaleTimeString('en-GB', { hour: '2-digit', minute: '2-digit', timeZone: 'UTC' })}` : day;
};
export const closeKey = (el, onclick) => el('button', { type: 'button', class: 'ev-close', 'aria-label': 'Close', onclick: (e) => { e.stopPropagation(); onclick(); } }, '×');
