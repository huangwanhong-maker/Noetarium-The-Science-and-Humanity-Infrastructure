// What the views of the plug-in From evidence to action share (the design of 2026-11-19): the open project's engagement as the server
// keeps it (its trajectory, its view, the predefined methods, the transfers assessed) and the SHA-256 it was last read under, so that
// every write can say what it was made on; the examples; the role the person says they act in; and words for numbers.

export const enc = encodeURIComponent;

export const G = {
  project: null, state: null, sha: null, examples: null,
  branch: 'main', turn: null, sel: null, mode: null, drafts: {}, files: [], reason: '',
  revisit: null, diffA: '', diffB: '', said: '', saidKind: '', busy: '', wrong: '',
  arch: null, archFor: null, resting: null,
  pick: null, assessed: null, editing: '', editFor: null,   // the Transfer view's
};

// The roles a person may say they act in; what the rules empower each to do is the engagement's.
export const ROLES = [
  { id: 'analyst', label: 'Analyst' }, { id: 'lead', label: 'Lead' }, { id: 'resident', label: 'Resident' }, { id: 'guest', label: 'Guest' },
];
const ROLE_KEY = 'urban-studio.engagement.role';
export function role() {
  try { const r = localStorage.getItem(ROLE_KEY); if (ROLES.some((x) => x.id === r)) return r; } catch { /* private */ }
  return 'analyst';
}
export function setRole(r) { try { localStorage.setItem(ROLE_KEY, r); } catch { /* private */ } }

export const base = (ctx) => `/api/projects/${enc(ctx.name)}/engagement`;

// The engagement as the server answers it, taken in.
export function adopt(answer) {
  if (!answer) return;
  const s = G.state || {};
  G.state = {
    kept: answer.kept ?? (answer.trajectory ? true : s.kept),
    methods: answer.methods || s.methods || [],
    transfers: answer.transfers || s.transfers || [],
    trajectory: answer.trajectory || (answer.kept === false ? null : s.trajectory),
    view: answer.view || (answer.kept === false ? null : s.view),
  };
  if (answer.sha256 !== undefined) G.sha = answer.sha256;
}

// The engagement of the open project, read when it is not yet (or again when asked).
export async function read(ctx, again = false) {
  if (G.project !== ctx.name) {
    Object.assign(G, { project: ctx.name, state: null, sha: null, branch: 'main', turn: null, sel: null, mode: null, drafts: {}, files: [], revisit: null, said: '', busy: '', wrong: '', resting: null, assessed: null, pick: null, editing: '', editFor: null });
  }
  if (!G.state || again) {
    try { adopt(await ctx.api(base(ctx))); G.wrong = ''; } catch (error) { G.wrong = error.message; }
  }
  if (!G.examples) {
    try { G.examples = (await ctx.api(`${base(ctx)}/examples`)).examples || []; } catch { G.examples = []; }
  }
}

// A write to the engagement: JSON (or bytes, given `raw`), sent with the person's key. A 409 answers the engagement as it stands now:
// it is taken in and said, and the act is not made. Any other refusal is thrown with the server's reason.
export async function send(ctx, path, body, raw = null) {
  const r = await ctx.fetch(path, { method: 'POST', headers: raw ? {} : { 'Content-Type': 'application/json' }, body: raw ?? JSON.stringify(body) });
  let answer = null;
  try { answer = await r.json(); } catch { /* no body */ }
  if (r.status === 409) {
    adopt(answer);
    G.said = 'Someone acted on the engagement since you read it, so your act was not made: it is shown here as it stands now.';
    G.saidKind = 'conflict';
    return null;
  }
  if (!r.ok) throw new Error(answer?.message || `${r.status} ${r.statusText}`);
  adopt(answer);
  return answer;
}

// Who may do what at which block: the engagement's rules (block or *, act, role or *).
export function empowered(rules, who, block, act) {
  return (rules || []).some((r) => (r.block === '*' || r.block === block) && r.act === act && (r.role === '*' || r.role === who));
}
export function empoweredRoles(rules, block, act) {
  return [...new Set((rules || []).filter((r) => (r.block === '*' || r.block === block) && r.act === act).map((r) => (r.role === '*' ? 'everyone' : r.role)))];
}

// Words for numbers: a share as a percentage, anything else to three figures.
export const fig = (x, d = 3) => (x === null || x === undefined || !Number.isFinite(Number(x)) ? '—' : (+Number(x).toPrecision(d)).toLocaleString('en-GB', { maximumSignificantDigits: d }));
export const pct = (p) => (p === null || p === undefined || !Number.isFinite(Number(p)) ? '—' : `${(100 * Number(p)).toFixed(Number(p) > 0.995 || Number(p) < 0.005 ? 0 : 1)}%`);
export const amount = (x, unit) => (unit === 'share' ? pct(x) : `${fig(x)}${unit ? ` ${unit}` : ''}`);
export function interval(e, unit) {
  if (!e) return '—';
  const lo = e.low ?? e.value, hi = e.high ?? e.value;
  return Math.abs(Number(hi) - Number(lo)) < 1e-12 ? amount(lo, unit) : `${amount(lo, unit)} to ${amount(hi, unit)}`;
}
const MONTHS = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
export function when(t) {
  const d = new Date(String(t || ''));
  if (Number.isNaN(d.getTime())) return String(t || '');
  return `${d.getUTCDate()} ${MONTHS[d.getUTCMonth()]} ${d.getUTCFullYear()}, ${String(d.getUTCHours()).padStart(2, '0')}:${String(d.getUTCMinutes()).padStart(2, '0')} UTC`;
}

// A file handed to the reader: text made in the page, saved under a name.
export function save(name, text, type = 'application/json') {
  const url = URL.createObjectURL(new Blob([text], { type: `${type};charset=utf-8` }));
  const a = document.createElement('a');
  a.href = url;
  a.download = name;
  document.body.append(a);
  a.click();
  a.remove();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}

// A small scale of an interval: the estimate as a band and a dot, a standard as a tick, a share always on 0 to 1.
export function scaleMarkup({ value, low, high, standard = null, unit = '', width = 220 }) {
  const lo = Number(low ?? value), hi = Number(high ?? value), v = Number(value ?? lo);
  const nums = [lo, hi, v, standard].filter((x) => x !== null && x !== undefined && Number.isFinite(Number(x))).map(Number);
  let a = unit === 'share' ? 0 : Math.min(...nums), b = unit === 'share' ? 1 : Math.max(...nums);
  if (!(b > a)) { a -= 1; b += 1; }
  const pad = unit === 'share' ? 0 : (b - a) * 0.08;
  a -= pad; b += pad;
  const W = width, H = 22, x = (t) => 6 + ((t - a) / (b - a)) * (W - 12);
  const parts = [`<line x1="6" y1="12" x2="${W - 6}" y2="12" class="eg-axis"/>`];
  if (unit === 'share') for (const t of [0, 0.5, 1]) parts.push(`<line x1="${x(t).toFixed(1)}" y1="9" x2="${x(t).toFixed(1)}" y2="15" class="eg-axis"/>`);
  parts.push(`<rect x="${x(lo).toFixed(1)}" y="8" width="${Math.max(2.5, x(hi) - x(lo)).toFixed(1)}" height="8" rx="2" class="eg-band"/>`);
  if (Number.isFinite(v)) parts.push(`<circle cx="${x(v).toFixed(1)}" cy="12" r="2.6" class="eg-dot"/>`);
  if (standard !== null && standard !== undefined && Number.isFinite(Number(standard))) parts.push(`<line x1="${x(Number(standard)).toFixed(1)}" y1="3" x2="${x(Number(standard)).toFixed(1)}" y2="21" class="eg-standard"/>`);
  return `<svg class="eg-scale" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" aria-hidden="true">${parts.join('')}</svg>`;
}
