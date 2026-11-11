// Market Studio's page (ADR 0093): a project of market analysis worked on as general project software is. At the left the
// project (its slices of space and time, its data, its quantities, the tools, what they answered, the views, the boards); in
// the middle what is looked at (a map with a clock, a table, a chart, a board); at the right what is selected, with a form
// built from what it declares; beneath, what happened. Nothing here knows a market: the tools do, and they are declared by
// the server (predefined ones and the project's own scripts). What the page computes is the workspace's (visualization/js/
// workspace): which rows stand in a slice and at an instant, what a chart gathers, how a column is scaled.

import * as B from '/visualization/js/board/index.js';
import * as W from '/visualization/js/workspace/model.js';
import * as V from '/visualization/js/workspace/views.js';
import { createPlane } from '/visualization/js/map/plane.js';
import { colourMap } from '/visualization/styles/palettes.js';
import { cssColour } from '/visualization/styles/colour.js';
import { el, svg, fill, clear, download, csv } from '/visualization/js/toolbox/dom.js';
import { barChart, lineChart, scatterChart, formatNumber } from '/visualization/js/toolbox/charts.js';

const $ = (id) => document.getElementById(id);
const now = () => new Date().toISOString().replace(/\.\d+Z$/, 'Z');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const count = (n) => Number(n).toLocaleString('en-GB');
// A number in brief, as a legend wants it: 1.01 M, 19.8 k, 2003.
const brief = (v) => {
  if (typeof v !== 'number' || !Number.isFinite(v)) return '—';
  const a = Math.abs(v);
  for (const [from, unit, suffix] of [[1e12, 1e12, ' T'], [1e9, 1e9, ' B'], [1e6, 1e6, ' M'], [1e4, 1e3, ' k']]) if (a >= from) return `${+(v / unit).toPrecision(3)}${suffix}`;
  return Number.isInteger(v) ? String(v) : String(+v.toPrecision(3));
};
const MOST_SHOWN = 400, MOST_DRAWN = 30000, MOST_SENT = 1500;
const RAMPS = ['orchid', 'gold', 'sea', 'plum'];

const S = {
  name: null, project: null, tools: [], scripts: [], status: null,
  sel: null, tab: 'map', slice: null, table: null, chart: null, figure: null, board: null,
  forms: {}, job: null, fit: true, clock: { ticks: [], k: 0, period: 'year', timer: null },
};
const cache = new Map();
const shut = new Set();   // the heads of the explorer that are folded
try { for (const name of (localStorage.getItem('market-studio.shut') || '').split('|')) if (name) shut.add(name); } catch { /* private */ }

// ---- the server ---------------------------------------------------------------------------------------------------------------

async function api(path, { method = 'GET', body = undefined, run = false, raw = null } = {}) {
  const headers = {};
  if (body !== undefined || run) headers['Content-Type'] = 'application/json';
  if (run) headers['X-Studio'] = 'run';
  const r = await fetch(path, { method, headers, body: raw ?? (body !== undefined ? JSON.stringify(body) : run ? '{}' : undefined) });
  let answer = null;
  try { answer = await r.json(); } catch { /* no body */ }
  if (!r.ok) throw new Error(answer?.message || `${r.status} ${r.statusText}`);
  return answer;
}

function say(words) {
  const node = $('say');
  node.textContent = words;
  node.classList.add('shown');
  clearTimeout(say.timer);
  say.timer = setTimeout(() => node.classList.remove('shown'), 2600);
}
function log(words, cls = '') {
  const box = $('console');
  box.append(el('div', { class: cls }, el('time', {}, new Date().toTimeString().slice(0, 8)), words));
  while (box.children.length > 300) box.firstChild.remove();
  box.scrollTop = box.scrollHeight;
}
// What is said beside the tabs, by the pane in view only: a pane drawn late does not speak over another.
const note = (tab, words) => { if (S.tab === tab) $('tab-note').textContent = words; };
const fail = (error) => { log(error.message || String(error), 'wrong'); say(error.message || String(error)); };

// ---- keeping the project ----------------------------------------------------------------------------------------------------------

let keepTimer = null, keeping = Promise.resolve();
function state(words, wrong = false) { $('state').textContent = words; $('state').classList.toggle('wrong', wrong); }
function touch() {
  state('Changed');
  clearTimeout(keepTimer);
  keepTimer = setTimeout(flush, 700);
}
// The project kept now: before the server is asked anything that it reads from the project's file.
function flush() {
  clearTimeout(keepTimer);
  keepTimer = null;
  if (!S.name || $('state').textContent !== 'Changed') return keeping;
  const body = S.project;
  keeping = keeping.then(() => api(`/api/projects/${S.name}`, { method: 'PUT', body })).then(() => state('Kept'), (error) => { state('Not kept', true); log(`The project was not kept: ${error.message}`, 'wrong'); });
  return keeping;
}
// What changed is of a kind that makes tables anew (a source, a slice, a step).
function changed() { cache.clear(); touch(); }

// A table of the project by its name, within a slice; `limit` for its first rows only (its columns, a look at it).
async function tableOf(name, slice = null, limit = null) {
  const key = `${name}|${slice || ''}|${limit || ''}`;
  if (!cache.has(key)) {
    cache.set(key, flush().then(() => api(`/api/projects/${S.name}/table?name=${encodeURIComponent(name)}${slice ? `&slice=${slice}` : ''}${limit ? `&limit=${limit}` : ''}`)));
    cache.get(key).catch(() => cache.delete(key));
  }
  return cache.get(key);
}
const columnsOf = async (name) => (name && W.tablesOf(S.project).has(name) ? (await tableOf(name, null, 1)).columns : []);

// ---- opening ------------------------------------------------------------------------------------------------------------------

async function openProject(name) {
  await flush();
  const a = await api(`/api/projects/${name}`);
  const wrong = W.checkProject(a.project);
  if (wrong.length) throw new Error(`the project '${name}' cannot be opened: ${wrong.slice(0, 2).join('; ')}`);
  Object.assign(S, { name, project: a.project, scripts: a.scripts, sel: null, slice: a.project.slices[0]?.id || null, table: null, figure: null, forms: {}, fit: true,
    chart: a.project.views.find((v) => v.kind === 'chart')?.id || null, board: a.project.boards[0]?.id || null });
  cache.clear();
  try { localStorage.setItem('market-studio.project', name); } catch { /* private */ }
  $('title').value = a.project.title;
  document.title = `${a.project.title} · Market Studio`;
  state('Kept');
  await loadTools();
  if (board) { board.destroy(); board = null; boardKept = null; clear($('rail')); }
  $('dialog').hidden = true;
  drawAll();
  log(`Opened ${a.project.title}: ${a.project.sources.length} sources, ${a.project.slices.length} slices, ${a.project.steps.length} steps run.`, 'said');
}
async function loadTools() {
  S.tools = (await api(`/api/tools?project=${S.name}`)).tools;
  for (const t of S.tools) if (t.error) log(`${t.title || t.ref}: ${t.error}`, 'wrong');
}

async function projectsDialog() {
  const dialog = $('dialog');
  const [kept, templates] = await Promise.all([api('/api/projects'), api('/api/templates')]);
  const begin = async (template, title) => { try { const a = await api('/api/projects', { method: 'POST', body: { template, title } }); await openProject(a.name); } catch (error) { fail(error); } };
  const title = el('input', { type: 'text', placeholder: 'A title for an empty project', style: 'width: 60%' });
  fill(dialog,
    el('h2', {}, 'Projects'),
    el('p', {}, 'A project is a folder: its data as they are, slices of space and time, quantities, the tools that were run and what they answered, views and boards.'),
    kept.projects.length ? [el('h4', {}, 'Kept on this computer'), el('div', { class: 'choices' }, kept.projects.map((p) => el('button', { class: 'choice', type: 'button', onclick: () => openProject(p.name).catch(fail) },
      el('b', {}, p.title || p.name), el('small', {}, `${p.sources} sources · ${p.steps} steps · kept ${String(p.saved).slice(0, 10)}`))))] : null,
    el('h4', {}, 'Begin from'),
    el('div', { class: 'choices' }, templates.templates.map((t) => el('button', { class: 'choice', type: 'button', onclick: () => begin(t.id) }, el('b', {}, t.title), el('span', {}, t.about)))),
    el('h4', {}, 'Or begin empty'),
    el('div', { class: 'actions', style: 'border: none; padding-top: 2px' }, title, el('button', { class: 'key edged', type: 'button', onclick: () => begin(null, title.value.trim() || 'Untitled project') }, 'Begin')),
    S.project ? el('div', { class: 'actions' }, el('button', { class: 'key', type: 'button', onclick: () => { dialog.hidden = true; } }, 'Close')) : null);
  dialog.hidden = false;
}

// ---- selection ----------------------------------------------------------------------------------------------------------------

const same = (a, b) => !!a && !!b && a.kind === b.kind && a.id === b.id;
const sliceOf = (id) => S.project.slices.find((s) => s.id === id) || null;
const viewOf = (id) => S.project.views.find((v) => v.id === id) || null;

function select(sel) {
  S.sel = sel;
  const p = S.project;
  if (sel?.kind === 'slice') { setSlice(sel.id); return; }
  if (sel?.kind === 'source') { S.table = p.sources.find((s) => s.id === sel.id)?.name || null; S.figure = null; show('table'); }
  if (sel?.kind === 'table') { S.table = sel.id; show('table'); }
  if (sel?.kind === 'view') {
    const v = viewOf(sel.id);
    if (v?.kind === 'chart') { S.chart = v.id; S.figure = null; show('chart'); }
    if (v?.kind === 'layer') { if (!v.on) { v.on = true; touch(); } show('map'); }
  }
  if (sel?.kind === 'board') { S.board = sel.id; show('board'); }
  drawExplorer();
  drawInspector();
}
function setSlice(id) {
  S.slice = id;
  S.fit = true;
  drawAll();
}
function show(tab) {
  S.tab = tab;
  for (const t of document.querySelectorAll('.tab')) t.classList.toggle('on', t.dataset.tab === tab);
  for (const name of ['map', 'table', 'chart', 'board']) $(`pane-${name}`).hidden = name !== tab;
  ({ map: drawMap, table: drawTable, chart: drawChart, board: openBoard })[tab]().catch(fail);
}
function drawAll() {
  const p = S.project;
  fill($('slice'), el('option', { value: '' }, 'the whole'), p.slices.map((s) => el('option', { value: s.id, selected: s.id === S.slice }, s.name)));
  drawExplorer();
  drawInspector();
  show(S.tab);
}

// ---- the explorer -------------------------------------------------------------------------------------------------------------

function drawExplorer() {
  const p = S.project, box = $('explorer');
  if (!p) { fill(box, el('p', { class: 'none' }, 'No project is open.')); return; }
  const stale = W.staleSteps(p, S.tools);
  const fold = (name) => { if (shut.has(name)) shut.delete(name); else shut.add(name); try { localStorage.setItem('market-studio.shut', [...shut].join('|')); } catch { /* private */ } drawExplorer(); };
  const open = (name, ...parts) => (shut.has(name) ? null : parts);
  const head = (title, add, about) => el('h3', { class: shut.has(title) ? 'shut' : '' }, el('span', { role: 'button', tabindex: 0, title: shut.has(title) ? 'Unfold' : 'Fold', onclick: () => fold(title), onkeydown: (e) => { if (e.key === 'Enter') fold(title); } }, title),
    add ? el('button', { class: 'key', type: 'button', title: about, 'aria-label': about, onclick: add }, '+') : null);
  const item = (sel, words, note = null, { cls = '', glyph = null, before = null, title = null } = {}) => el('div', { class: `item ${same(S.sel, sel) ? 'on' : ''} ${cls}`, role: 'button', tabindex: 0, title: title || words,
    onclick: () => select(sel), onkeydown: (e) => { if (e.key === 'Enter') select(sel); } }, before, glyph ? el('span', { class: 'glyph' }, glyph) : null, el('span', {}, words), note ? el('i', {}, note) : null);
  const none = (words) => el('p', { class: 'none' }, words);
  const when = (s) => { if (!s.time) return ''; const a = s.time.from ?? '', b = s.time.until ?? ''; return a === b ? String(a) : `${a}–${b}`; };

  const groups = new Map();
  for (const t of S.tools) { if (!groups.has(t.group)) groups.set(t.group, { title: t.group_title || t.title || t.group, tools: [] }); if (t.id || t.error) groups.get(t.group).tools.push(t); }
  const planned = (p.plan || []).filter((e) => !p.steps.some((s) => s.id === e.id));
  const toggle = (v) => el('input', { type: 'checkbox', checked: !!v.on, 'aria-label': `Draw ${v.title}`, onclick: (e) => { e.stopPropagation(); v.on = e.target.checked; touch(); drawMap().catch(fail); } });

  fill(box,
    head('Slices', addSlice, 'A new slice of space and time'),
    open('Slices', item({ kind: 'slice', id: null }, 'The whole', null, { cls: S.slice === null ? 'on' : '', glyph: '◇' }),
      p.slices.map((s) => item({ kind: 'slice', id: s.id }, s.name, when(s), { cls: S.slice === s.id ? 'on' : '', glyph: s.region ? '▣' : '◷' }))),
    head('Data', addDataDialog, 'Add a file of data'),
    open('Data', p.sources.length ? p.sources.map((s) => item({ kind: 'source', id: s.id }, s.title || s.name, s.name)) : none('Drop a file here, or add one.')),
    head('Quantities', addQuantity, 'A new quantity'),
    open('Quantities', p.quantities.length ? p.quantities.map((q) => item({ kind: 'quantity', id: q.name }, q.name, `${brief(q.mid)}${q.low !== q.mid || q.high !== q.mid ? ` [${brief(q.low)}, ${brief(q.high)}]` : ''}`)) : none('Data and assumptions by name, with their ranges.')),
    head('Tools', newScript, 'Write a tool of the project\'s own'),
    open('Tools', [...groups.values()].map((g) => [el('h4', { class: shut.has(`tools:${g.title}`) ? 'shut' : '', role: 'button', tabindex: 0, onclick: () => fold(`tools:${g.title}`) }, g.title),
      open(`tools:${g.title}`, g.tools.map((t) => item({ kind: 'tool', id: t.ref }, t.title || t.ref, null, { cls: 'sub', title: t.description || t.error || '' })))])),
    head('Results', planned.length || stale.size ? () => runPlan().catch(fail) : null, 'Run what is planned and what is stale'),
    open('Results', p.steps.map((s) => [
      item({ kind: 'step', id: s.id }, s.title, sliceOf(s.slice)?.name || '', { cls: stale.has(s.id) ? 'stale' : '', glyph: stale.has(s.id) ? '↻' : '✓', title: stale.has(s.id) ? 'What it was given changed since it ran' : s.title }),
      (s.outputs || []).filter((o) => o.kind === 'table').map((o) => item({ kind: 'table', id: o.name }, o.name, count(o.rows), { cls: 'sub', title: o.title })),
    ]),
    planned.map((e) => item({ kind: 'plan', id: e.id }, e.title, 'to run', { glyph: '·' })),
    !p.steps.length && !planned.length ? none('Run a tool: what it answers stands here.') : null),
    head('Views'),
    open('Views', p.views.length ? p.views.map((v) => item({ kind: 'view', id: v.id }, v.title || v.table, v.kind === 'chart' ? 'chart' : v.layer, { before: v.kind === 'layer' ? toggle(v) : null, glyph: v.kind === 'chart' ? '▤' : null })) : none('Layers of the map and charts.')),
    head('Boards', addBoard, 'A new board'),
    open('Boards', p.boards.map((b) => item({ kind: 'board', id: b.id }, b.title))));
}

// ---- the inspector: forms from what is declared ----------------------------------------------------------------------------------

const row = (label, control, help = null, tall = false) => el('label', { class: `row${tall ? ' tall' : ''}` }, el('span', {}, label), control, help ? el('small', {}, help) : null);
const textIn = (value, on, attrs = {}) => el('input', { type: 'text', value: value ?? '', spellcheck: 'false', onchange: (e) => on(e.target.value.trim()), ...attrs });
const numberIn = (value, on, attrs = {}) => el('input', { type: 'number', value: value ?? '', step: 'any', onchange: (e) => on(e.target.value === '' ? null : Number(e.target.value)), ...attrs });
const choiceIn = (value, options, on) => el('select', { onchange: (e) => on(e.target.value) }, options.map((o) => { const [v, l] = Array.isArray(o) ? o : [o, o]; return el('option', { value: v, selected: String(v) === String(value ?? '') }, l); }));
const actions = (...keys) => el('div', { class: 'actions' }, keys);
const key = (words, act, cls = 'edged', attrs = {}) => el('button', { class: `key ${cls}`, type: 'button', onclick: () => Promise.resolve(act()).catch(fail), ...attrs }, words);
const columnChoice = (columns, value, on, { numbers = false, none = '(none)' } = {}) => choiceIn(value, [['', none], ...columns.filter((c) => !numbers || c.kind === 'number').map((c) => [c.name, c.name])], on);

let inspecting = 0;
async function drawInspector() {
  const box = $('inspector'), p = S.project, sel = S.sel, token = ++inspecting;
  if (!p) { clear(box); return; }
  let parts;
  try {
    parts = await ({ slice: inspectSlice, source: inspectSource, quantity: inspectQuantity, tool: inspectTool, step: inspectStep, plan: inspectPlan, table: inspectTable, view: inspectView, board: inspectBoard, row: inspectRow })[sel?.kind || '']?.(sel)
      ?? inspectProject();
  } catch (error) { parts = [el('h3', {}, 'Not read'), el('p', { class: 'wrong' }, error.message)]; }
  if (token === inspecting) fill(box, parts);
}

function inspectProject() {
  const p = S.project, e = S.status?.engine;
  return [el('h3', {}, 'The project'),
    el('p', {}, `${p.sources.length} sources, ${p.slices.length} slices, ${p.quantities.length} quantities, ${p.steps.length} steps run, ${p.views.length} views, ${p.boards.length} boards.`),
    el('p', { class: 'tip' }, 'Add your data and say what its columns are. Cut slices of space and time. Run a tool on a slice: what it answers is a table, drawn as a layer of the map or as a chart, and sent to a board to be written about. A tool is a script with declared parameters; write your own when the predefined ones do not answer.'),
    e && !e.current ? [el('p', { class: 'wrong' }, 'The engine of the markets domain is not built: the market\'s tools cannot answer.'), actions(key('Build it', buildEngine, 'main'))] : null,
    S.status?.tools ? el('p', { class: 'wrong' }, S.status.tools) : null];
}

function inspectSlice(sel) {
  const s = sliceOf(sel.id);
  if (!s) return [el('h3', {}, 'The whole'), el('p', { class: 'tip' }, 'Everything, everywhere and at every time. A slice keeps the rows within a region and an interval.')];
  const set = (patch) => { Object.assign(s, patch, { changed: now() }); S.fit = !!patch.region; changed(); drawAll(); };
  const time = (which) => textIn(s.time?.[which], (v) => { const t = { from: s.time?.from ?? null, until: s.time?.until ?? null, [which]: v === '' ? null : /^\d{4}$/.test(v) ? Number(v) : v }; set({ time: t.from === null && t.until === null ? null : t }); }, { placeholder: 'a year, or 2024-03-01' });
  const r = s.region, side = (k) => numberIn(r?.[k], (v) => set({ region: { ...(s.region || viewRegion()), [k]: v } }), { 'aria-label': k, title: k });
  return [el('h3', {}, 'A slice'),
    row('Name', textIn(s.name, (v) => set({ name: v || s.name }))),
    row('From', time('from')), row('Until', time('until'), 'its last period is within the slice'),
    row('Period', choiceIn(s.period, Object.keys(W.PERIODS), (v) => set({ period: v })), 'the step of its clock, and what a row of one time stands for'),
    row('Region', el('div', { class: 'four' }, side('west'), side('east'), side('south'), side('north')), r ? 'degrees: west, east, south, north' : 'none: everywhere', true),
    actions(key('The map\'s view', () => set({ region: viewRegion() })), r ? key('Everywhere', () => set({ region: null })) : null,
      key('Remove', () => { S.project.slices = S.project.slices.filter((x) => x !== s); S.slice = null; S.sel = null; changed(); drawAll(); }, ''))];
}

async function inspectSource(sel) {
  const s = S.project.sources.find((x) => x.id === sel.id);
  if (!s) return null;
  const set = (patch) => { Object.assign(s, patch, { changed: now() }); changed(); drawAll(); };
  const remove = actions(key('Remove', () => { S.project.sources = S.project.sources.filter((x) => x !== s); S.project.views = S.project.views.filter((v) => v.table !== s.name); S.sel = null; S.table = null; changed(); drawAll(); }, ''));
  let t, whole;
  try { t = await tableOf(s.name, null, 1); whole = await tableOf(s.name); } catch (error) {
    // A source that cannot be read still has its panel: its file can be seen, its filter mended, and it can be removed.
    return [el('h3', {}, 'Data'), row('Named', el('b', {}, s.name)), row('File', el('span', { title: s.ref }, s.ref)), el('p', { class: 'wrong' }, `Not read: ${error.message}`),
      (s.where || []).length ? actions(key('Drop its filter', () => set({ where: [] }))) : null, remove];
  }
  const filter = el('textarea', { rows: 3, spellcheck: 'false', placeholder: 'kind = Cafes and Restaurants\nyear >= 2010', onchange: (e) => { try { set({ where: parseFilter(e.target.value) }); } catch (error) { fail(error); } } }, (s.where || []).map(writeCondition).join('\n'));
  return [el('h3', {}, 'Data'),
    row('Title', textIn(s.title, (v) => { s.title = v; touch(); drawExplorer(); })),
    row('Named', el('b', {}, s.name), 'tools are given it by this name'),
    row('File', el('span', { title: s.ref }, s.ref)),
    el('p', { class: 'tip' }, `${count(whole.rows)} rows of ${t.columns.length} columns${whole.cut ? ` (the first ${count(whole.cut.sent)} of ${count(whole.cut.rows)} sent)` : ''}${whole.lines ? `, ${count(whole.lines.length)} lines` : ''}. The file is left as it is: say what its columns are.`),
    el('h4', {}, 'Its columns as'),
    W.ROLES.map((role) => row(role.label, columnChoice(t.columns, s.roles?.[role.id], (v) => { const roles = { ...(s.roles || {}) }; if (v) roles[role.id] = v; else delete roles[role.id]; set({ roles }); }, { numbers: role.kind === 'number' }), role.about)),
    el('h4', {}, 'Keep only rows where'),
    row('', filter, 'a line a condition: column, then = != > >= < <= or in, then a value (for in, values with commas between)', true),
    tableActions(s.name, null), remove];
}
const writeCondition = (c) => `${c.column} ${c.op} ${Array.isArray(c.value) ? c.value.join(', ') : c.value}`;
function parseFilter(text) {
  return text.split('\n').map((l) => l.trim()).filter(Boolean).map((line) => {
    const m = /^(.+?)\s*(!=|>=|<=|=|>|<|\bin\b)\s*(.+)$/.exec(line);
    if (!m) throw new Error(`not a condition: ${line}`);
    return { column: m[1].trim(), op: m[2], value: m[2] === 'in' ? m[3].split(',').map((v) => v.trim()).filter(Boolean) : m[3].trim() };
  });
}

function inspectQuantity(sel) {
  const p = S.project, q = p.quantities.find((x) => x.name === sel.id);
  if (!q) return null;
  const set = (patch) => {
    const next = { ...q, ...patch };
    if (!(next.low <= next.mid && next.mid <= next.high)) { if ('mid' in patch) { next.low = Math.min(next.low, next.mid); next.high = Math.max(next.high, next.mid); } else { fail(new Error('a quantity is low ≤ likely ≤ high')); drawInspector(); return; } }
    Object.assign(q, next);
    p.meta = { ...(p.meta || {}), quantities: now() };
    touch(); drawExplorer(); drawInspector();
  };
  return [el('h3', {}, 'A quantity'),
    row('Named', el('b', {}, q.name), 'tools and formulas are given it by this name'),
    row('Likely', numberIn(q.mid, (v) => v !== null && set({ mid: v }))),
    row('At least', numberIn(q.low, (v) => v !== null && set({ low: v }))), row('At most', numberIn(q.high, (v) => v !== null && set({ high: v }))),
    row('Unit', textIn(q.unit, (v) => { q.unit = v; touch(); })),
    row('About', textIn(q.about, (v) => { q.about = v; touch(); }), 'where it comes from: a datum, or an assumption'),
    actions(key('Remove', () => { p.quantities = p.quantities.filter((x) => x !== q); p.meta = { ...(p.meta || {}), quantities: now() }; S.sel = null; touch(); drawExplorer(); drawInspector(); }, ''))];
}

// The form of a tool, from the parameters it declares: a table among the project's, a column among that table's, a quantity
// among the project's, a choice among its options, a number within its bounds.
async function inspectTool(sel) {
  const p = S.project, tool = S.tools.find((t) => t.ref === sel.id);
  if (!tool) return null;
  if (tool.error) return [el('h3', {}, tool.title || tool.ref), el('p', { class: 'wrong' }, tool.error), tool.group === 'project' ? actions(key('Edit the script', () => editScript(tool.ref.split('/')[1]))) : null];
  const last = [...p.steps].reverse().find((s) => s.tool === tool.ref);
  const form = S.forms[tool.ref] ||= { ...Object.fromEntries(tool.params.map((q) => [q.name, q.default])), ...(last?.params || {}) };
  const names = [...W.tablesOf(p).keys()];
  const fields = [];
  for (const q of tool.params) {
    const set = (v) => { form[q.name] = v; if (tool.params.some((o) => o.of === q.name)) drawInspector(); };
    let control;
    if (q.role === 'table') control = choiceIn(form[q.name], [...(names.includes(form[q.name]) ? [] : [[form[q.name] ?? '', form[q.name] ? `${form[q.name]} (not there yet)` : '(choose)']]), ...names], set);
    else if (q.role === 'column') control = columnChoice(await columnsOf(form[q.of]).catch(() => []), form[q.name], set, { none: '(left out)' });
    else if (q.role === 'columns') control = textIn(form[q.name], set, { placeholder: (await columnsOf(form[q.of]).catch(() => [])).map((c) => c.name).slice(0, 4).join(', ') });
    else if (q.role === 'quantity') control = choiceIn(form[q.name], [...(p.quantities.some((x) => x.name === form[q.name]) ? [] : [[form[q.name] ?? '', `${form[q.name] || '(choose)'}${form[q.name] ? ' (not defined)' : ''}`]]), ...p.quantities.map((x) => x.name)], set);
    else if (q.kind === 'choice') control = choiceIn(form[q.name], q.options.map((o) => (typeof o === 'object' ? [o.value, o.label ?? o.value] : [o, o])), set);
    else if (q.kind === 'boolean') control = el('input', { type: 'checkbox', checked: !!form[q.name], onchange: (e) => set(e.target.checked) });
    else if (q.kind === 'number' || q.kind === 'integer') control = numberIn(form[q.name], (v) => set(v ?? q.default), { min: q.min, max: q.max, step: q.step ?? (q.kind === 'integer' ? 1 : 'any') });
    else if (String(q.default ?? '').includes('\n')) control = el('textarea', { rows: 5, spellcheck: 'false', onchange: (e) => set(e.target.value) }, form[q.name] ?? '');
    else control = textIn(form[q.name], set);
    fields.push(row(q.label, control, q.help, control.tagName === 'TEXTAREA'));
  }
  form.$slice ??= S.slice || '';
  const running = S.job?.tool === tool.ref;
  return [el('h3', {}, tool.title),
    el('p', {}, tool.description),
    fields,
    row('On', choiceIn(form.$slice, [['', 'the whole'], ...p.slices.map((s) => [s.id, s.name])], (v) => { form.$slice = v; }), 'the slice whose rows the tool is given'),
    running ? [el('div', { class: 'progress' }, el('b', { id: 'progress', style: `width:${Math.round(100 * S.job.progress)}%` })), el('p', { class: 'tip', id: 'progress-text' }, S.job.text || 'Running…'), actions(key('Stop', () => api(`/api/job/${S.job.id}/cancel`, { method: 'POST', run: true })))]
      : actions(key('Run', () => runTool(tool, paramsOf(tool, form), form.$slice || null), 'main', { disabled: !!S.job }), tool.group === 'project' ? key('Edit the script', () => editScript(tool.ref.split('/')[1])) : null)];
}
const paramsOf = (tool, form) => Object.fromEntries(tool.params.map((q) => [q.name, form[q.name] ?? q.default]));

function inspectPlan(sel) {
  const e = (S.project.plan || []).find((x) => x.id === sel.id), tool = e && S.tools.find((t) => t.ref === e.tool);
  if (!e) return null;
  return [el('h3', {}, 'Planned'), el('p', {}, e.title), el('p', { class: 'tip' }, `${tool?.title || e.tool}${sliceOf(e.slice) ? `, on ${sliceOf(e.slice).name}` : ''}. ${tool?.description || ''}`),
    actions(key('Run it', () => runPlanned(e), 'main', { disabled: !!S.job }), key('Run all that is planned', runPlan, 'edged', { disabled: !!S.job }))];
}

async function inspectStep(sel) {
  const p = S.project, s = p.steps.find((x) => x.id === sel.id), tool = s && S.tools.find((t) => t.ref === s.tool);
  if (!s) return null;
  const stale = W.staleSteps(p, S.tools).has(s.id);
  const figures = (s.outputs || []).map((o, k) => [o, k]).filter(([o]) => o.kind === 'figure');
  const given = Object.entries(s.params || {}).filter(([, v]) => v !== '' && v !== null && !String(v).includes('\n')).map(([k, v]) => `${k}: ${v}`).join(' · ');
  return [el('h3', {}, 'A result'),
    row('Title', textIn(s.title, (v) => { s.title = v || s.title; touch(); drawExplorer(); })),
    el('p', { class: 'tip' }, `${tool?.title || s.tool}${sliceOf(s.slice) ? `, on ${sliceOf(s.slice).name}` : ', on the whole'}; run ${String(s.ran).replace('T', ' ').replace('Z', '')} in ${s.seconds} s.`),
    stale ? el('p', { class: 'wrong' }, 'What it was given changed since it ran: its answer is of an earlier state.') : null,
    given ? el('p', { class: 'tip' }, given) : null,
    (s.outputs || []).filter((o) => o.kind === 'note').map((o) => el('div', { class: 'note-text' }, o.text)),
    (s.outputs || []).filter((o) => o.kind === 'table').length ? el('h4', {}, 'Its tables') : null,
    (s.outputs || []).filter((o) => o.kind === 'table').map((o) => el('div', { class: 'item', role: 'button', tabindex: 0, onclick: () => select({ kind: 'table', id: o.name }) }, el('span', { title: o.title }, o.title || o.name), el('i', {}, count(o.rows)))),
    figures.length ? [el('h4', {}, 'Its figures'), figures.map(([o, k]) => el('div', { class: 'item', role: 'button', tabindex: 0, onclick: () => { S.figure = { step: s.id, k }; show('chart'); } }, el('span', {}, o.title || 'A figure')))] : null,
    actions(key('Run again', () => runTool(tool, s.params, s.slice, { step: s.id, title: s.title }), 'main', { disabled: !!S.job || !tool }), key('To the board', () => sendStep(s)),
      key('Its form', () => { S.forms[s.tool] = { ...s.params, $slice: s.slice || '' }; select({ kind: 'tool', id: s.tool }); }),
      key('Remove', () => { const names = new Set((s.outputs || []).map((o) => o.name)); p.steps = p.steps.filter((x) => x !== s); p.views = p.views.filter((v) => !names.has(v.table)); S.sel = null; changed(); drawAll(); }, ''))];
}

// What can be made of a table: a layer where it has a place, a chart, a board's table, a file.
function tableActions(name, slice) {
  const p = S.project;
  const view = async (kind) => {
    const t = await tableOf(name, null, 200), d = kind === 'layer' ? { ...V.defaultView(t), on: true } : { kind: 'chart', ...(V.defaultView(t).kind === 'chart' ? V.defaultView(t) : { chart: 'bar', x: t.roles?.category || t.roles?.name || t.columns[0]?.name, y: t.roles?.amount || null, how: t.roles?.amount ? 'sum' : 'count' }) };
    if (d.kind !== kind) throw new Error(`the table '${name}' has no place: bind its latitude and longitude, or its cell`);
    const v = { id: W.makeId(), title: t.title || name, table: name, ...d };
    p.views.push(v);
    touch();
    select({ kind: 'view', id: v.id });
  };
  return actions(key('On the map', () => view('layer')), key('As a chart', () => view('chart')), key('To the board', () => sendTable(name)),
    key('Save as CSV', async () => { const t = await tableOf(name, slice ?? S.slice); download(`${name}.csv`, csv(t.columns.map((c) => c.name), Array.from({ length: t.rows }, (_, i) => t.columns.map((c) => t.data[c.name][i])))); }));
}
async function inspectTable(sel) {
  const where = W.tablesOf(S.project).get(sel.id);
  if (!where) return null;
  const t = await tableOf(sel.id, null, 1), all = await tableOf(sel.id);
  return [el('h3', {}, 'A table'), el('p', {}, where.title), row('Named', el('b', {}, sel.id)),
    el('p', { class: 'tip' }, `${count(all.rows)} rows of ${t.columns.length} columns. ${Object.entries(t.roles || {}).map(([r, c]) => `${W.ROLES.find((x) => x.id === r)?.label || r}: ${c}`).join(' · ')}`),
    tableActions(sel.id, null)];
}

async function inspectView(sel) {
  const p = S.project, v = viewOf(sel.id);
  if (!v) return null;
  const columns = await columnsOf(v.table);
  const set = (patch) => { Object.assign(v, patch); touch(); drawExplorer(); (v.kind === 'chart' ? drawChart() : drawMap()).catch(fail); };
  const col = (k, opts = {}) => columnChoice(columns, v[k], (c) => set({ [k]: c || null }), opts);
  const common = [row('Title', textIn(v.title, (t) => set({ title: t }))), row('Table', el('div', { class: 'item', role: 'button', tabindex: 0, onclick: () => select({ kind: 'table', id: v.table }) }, el('span', {}, v.table)))];
  const end = actions(key('To the board', () => sendTable(v.table, v)), key('Remove', () => { p.views = p.views.filter((x) => x !== v); S.sel = null; if (S.chart === v.id) S.chart = null; touch(); drawAll(); }, ''));
  if (v.kind === 'layer') return [el('h3', {}, 'A layer of the map'), common,
    row('Drawn as', el('b', {}, V.LAYERS.find((l) => l.id === v.layer)?.label || v.layer)),
    v.layer === 'lines' ? null : [row('Coloured by', col('colour')), v.layer === 'points' ? row('Sized by', col('size', { numbers: true })) : null,
      row('Colours', choiceIn(v.map || 'orchid', RAMPS, (m) => set({ map: m })), 'for numbers; kinds take hues held apart'), row('Named by', col('label'), 'what a place picked is called')],
    el('p', { class: 'tip' }, 'A layer whose table has a time moves with the clock.'), end];
  return [el('h3', {}, 'A chart'), common,
    row('Drawn as', choiceIn(v.chart, V.CHARTS.map((c) => [c.id, c.label]), (c) => set({ chart: c })), V.CHARTS.find((c) => c.id === v.chart)?.about),
    row('Across', col('x')), row('Up', col('y', { numbers: true, none: '(the count)' })),
    v.chart === 'scatter' || v.chart === 'histogram' ? null : [row('Take', choiceIn(v.how || (v.y ? 'sum' : 'count'), V.GATHER, (h) => set({ how: h })), 'of the rows that share a value across'), row('Split by', col('split'), 'a series for each value')],
    v.chart === 'histogram' ? row('Bins', numberIn(v.bins || 12, (n) => set({ bins: n || 12 }), { min: 1, max: 200, step: 1 })) : null,
    ['bar', 'stacked', 'share'].includes(v.chart) ? row('At most', numberIn(v.most || 24, (n) => set({ most: n || 24 }), { min: 2, max: 200, step: 1 }), 'the greatest first') : null,
    end];
}

function inspectBoard(sel) {
  const p = S.project, b = p.boards.find((x) => x.id === sel.id);
  if (!b) return null;
  return [el('h3', {}, 'A board'), row('Title', textIn(b.title, (v) => { b.title = v || b.title; board?.title(b.title); touch(); drawExplorer(); })),
    el('p', { class: 'tip' }, 'Where findings are written about: notes, links, a pen, plans, quantities with their ranges, tables and the cards that draw them. Send a table, a view or a result here from its own panel.'),
    p.boards.length > 1 ? actions(key('Remove', () => { p.boards = p.boards.filter((x) => x !== b); S.board = p.boards[0].id; S.sel = null; touch(); drawAll(); }, '')) : null];
}

function inspectRow(sel) {
  const t = sel.table, i = sel.row;
  return [el('h3', {}, 'A place picked'), el('p', {}, sel.title),
    t.columns.map((c) => row(c.name, el('span', {}, typeof t.data[c.name][i] === 'number' ? formatNumber(t.data[c.name][i]) : String(t.data[c.name][i] ?? '—'))))];
}

// ---- adding ---------------------------------------------------------------------------------------------------------------------

function addSlice() {
  const p = S.project, s = W.newSlice({ name: `Slice ${p.slices.length + 1}`, region: plane && p.frame ? viewRegion() : null });
  s.changed = now();
  p.slices.push(s);
  S.slice = s.id;
  S.sel = { kind: 'slice', id: s.id };
  changed(); drawAll();
}
function addQuantity() {
  const p = S.project, name = W.tableName('quantity', new Set(p.quantities.map((q) => q.name)));
  const dialog = $('dialog'), named = el('input', { type: 'text', value: name, spellcheck: 'false' });
  fill(dialog, el('h2', {}, 'A quantity'), el('p', {}, 'A datum or an assumption by name: letters, digits and _. Its range is set in its panel.'),
    el('div', { class: 'actions', style: 'border: none' }, named, key('Add', () => {
      const n = named.value.trim();
      if (!W.NAME.test(n) || p.quantities.some((q) => q.name === n)) throw new Error('a quantity needs a name of its own, of letters, digits and _');
      p.quantities.push({ name: n, mid: 1, low: 1, high: 1, unit: '', about: '' });
      p.meta = { ...(p.meta || {}), quantities: now() };
      dialog.hidden = true; touch(); select({ kind: 'quantity', id: n });
    }, 'main'), key('Close', () => { dialog.hidden = true; }, '')));
  dialog.hidden = false;
  named.select();
}
function addBoard() {
  const p = S.project, b = { id: W.makeId(), title: `Board ${p.boards.length + 1}` };
  p.boards.push(b);
  touch();
  select({ kind: 'board', id: b.id });
}

async function addSource(ref, title) {
  const p = S.project;
  const head = await api(`/api/projects/${S.name}/read`, { method: 'POST', body: { source: { ref, name: 'table' }, limit: 300 } });
  const source = { id: W.makeId(), name: W.tableName(title.replace(/\.[a-z]+$/i, ''), new Set(W.tablesOf(p).keys())), title, ref, roles: { ...(head.roles || {}), ...W.guessRoles(head) }, where: [], changed: now() };
  p.sources.push(source);
  if (head.lines?.length) p.views.push({ id: W.makeId(), kind: 'layer', layer: 'lines', title, table: source.name, on: true });
  changed();
  log(`Added ${title}: ${head.cut ? count(head.cut.rows) : count(head.rows)} rows; its columns were guessed as ${Object.entries(source.roles).map(([r, c]) => `${r} ${c}`).join(', ') || 'nothing'}: confirm them.`, 'said');
  select({ kind: 'source', id: source.id });
}
async function addFiles(files) {
  for (const f of files) {
    try {
      const kept = await api(`/api/projects/${S.name}/files?name=${encodeURIComponent(f.name)}`, { method: 'POST', raw: f });
      await addSource(kept.ref, f.name);
    } catch (error) { fail(new Error(`${f.name}: ${error.message}`)); }
  }
}
function addDataDialog() {
  const dialog = $('dialog');
  const file = el('input', { type: 'file', multiple: true, accept: '.csv,.tsv,.txt,.json,.geojson', onchange: () => { dialog.hidden = true; addFiles([...file.files]); } });
  const ref = el('input', { type: 'text', placeholder: 'products/tables/melbourne-blocks.json', spellcheck: 'false', style: 'width: 70%' });
  fill(dialog, el('h2', {}, 'Add data'),
    el('p', {}, 'A file is kept in the project as it is: delimited text, a table or a list of rows in JSON, or GeoJSON. Nothing is asked of its columns\' names: you say afterwards which is the latitude, the time, the amount.'),
    el('div', { class: 'actions', style: 'border: none' }, file),
    el('h4', {}, 'Or a file of the data library'),
    el('div', { class: 'actions', style: 'border: none; padding-top: 2px' }, ref, key('Add', async () => { const r = ref.value.trim(); if (!r) return; dialog.hidden = true; await addSource(`data:${r}`, r.split('/').pop()); }, 'main')),
    actions(key('Close', () => { dialog.hidden = true; }, '')));
  dialog.hidden = false;
}

// ---- running --------------------------------------------------------------------------------------------------------------------

async function runTool(tool, params, slice, { step = null, title = null } = {}) {
  if (S.job) throw new Error('a tool is running: wait for it, or stop it');
  await flush();
  const a = await api(`/api/projects/${S.name}/run`, { method: 'POST', run: true, body: { tool: tool.ref, params, slice, step, title } });
  S.job = { id: a.job, tool: tool.ref, progress: 0, text: '' };
  log(`Running ${title || tool.title}${sliceOf(slice) ? ` on ${sliceOf(slice).name}` : ''}…`);
  drawInspector();
  try {
    for (;;) {
      await sleep(300);
      const j = await api(`/api/job/${a.job}`);
      Object.assign(S.job, { progress: j.progress || 0, text: j.text || '' });
      if ($('progress')) { $('progress').style.width = `${Math.round(100 * S.job.progress)}%`; $('progress-text').textContent = S.job.text || 'Running…'; }
      if (j.state === 'done') { S.job = null; return integrate(j.step); }
      if (j.state !== 'running') { if (j.detail && tool.group === 'project') log(j.detail, 'wrong'); throw new Error(`${title || tool.title}: ${j.message || j.state}`); }
    }
  } finally { S.job = null; drawInspector(); }
}

// A step that ran, taken into the project: it replaces what it answered before; each of its tables gets the view its tool
// suggested, unless one draws it already.
function integrate(step) {
  const p = S.project, k = p.steps.findIndex((s) => s.id === step.id);
  if (k >= 0) p.steps[k] = step; else p.steps.push(step);
  let first = null;
  const drawn = new Set();
  for (const o of step.outputs || []) {
    if (o.kind === 'note') log(o.text, 'said');
    if (o.kind !== 'table') continue;
    let v = p.views.find((x) => x.table === o.name);
    if (!v && o.show) { v = { id: W.makeId(), title: o.title || o.name, table: o.name, ...o.show }; p.views.push(v); }
    // What the step answered is what the map shows: its areas and its points, one of each, over the lines; the rest is put by.
    if (v?.kind === 'layer') {
      if (!drawn.size) for (const x of p.views) if (x.kind === 'layer' && x.layer !== 'lines') x.on = false;
      v.on = !drawn.has(v.layer);
      drawn.add(v.layer);
    }
    first ||= v;
  }
  log(`${step.title}: ${(step.outputs || []).filter((o) => o.kind === 'table').map((o) => `${o.name} (${count(o.rows)})`).join(', ') || 'no table'} in ${step.seconds} s.`);
  changed();
  S.sel = { kind: 'step', id: step.id };
  if (first?.kind === 'chart') { S.chart = first.id; S.figure = null; }
  drawExplorer(); drawInspector();
  show(first?.kind === 'chart' ? 'chart' : first?.kind === 'layer' ? 'map' : S.tab);
  return step;
}

async function runPlanned(e) {
  const tool = S.tools.find((t) => t.ref === e.tool);
  if (!tool || tool.error) throw new Error(`the plan names a tool that is not there: ${e.tool}`);
  return runTool(tool, { ...Object.fromEntries(tool.params.map((q) => [q.name, q.default])), ...(e.params || {}) }, e.slice || null, { step: e.id, title: e.title });
}
// What is planned and not yet run, in its order; then what is stale, in the order it was run.
async function runPlan() {
  const p = S.project;
  for (const e of p.plan || []) if (!p.steps.some((s) => s.id === e.id)) await runPlanned(e);
  for (;;) {
    const stale = W.staleSteps(p, S.tools), s = p.steps.find((x) => stale.has(x.id)), tool = s && S.tools.find((t) => t.ref === s.tool);
    if (!s || !tool) break;
    await runTool(tool, s.params, s.slice, { step: s.id, title: s.title });
  }
  say('Everything planned has run.');
}

// ---- scripts of the project's own -------------------------------------------------------------------------------------------------

const SCRIPT = `"""A tool of this project's own: what it answers, in a line."""

import numpy as np


def create(api):
    class Mine(api.BoardAnalysis):
        id = "mine"
        title = "My tool"
        description = "The mean of a column, for each value of another."
        params = [api.param("table", "Table", "", kind="text", role="table"),
                  api.param("by", "By", "", kind="text", role="column", of="table"),
                  api.param("of", "Mean of", "", kind="text", role="column", of="table")]

        def run(self, board, params, context):
            t = board.table(params["table"])          # its rows are those of the slice the tool was run on
            keys, values = t.text(params["by"]), t.column(params["of"])
            groups = {}
            for k, v in zip(keys, values):
                if np.isfinite(v):
                    groups.setdefault(k, []).append(v)
            names = sorted(groups)
            return [
                api.table("means", {params["by"]: names, "mean": [float(np.mean(groups[k])) for k in names]},
                          title=f"The mean of {params['of']} by {params['by']}",
                          show={"kind": "chart", "chart": "bar", "x": params["by"], "y": "mean", "how": "sum"}),
                api.note(f"{len(names)} groups of {t.rows} rows in {board.view.get('slice') or 'the whole'}."),
            ]
    return Mine()
`;
function newScript() {
  const dialog = $('dialog'), named = el('input', { type: 'text', value: 'mine', spellcheck: 'false' });
  fill(dialog, el('h2', {}, 'A tool of your own'),
    el('p', {}, 'A script in Python that declares its parameters and answers tables, notes and figures. It is given the tables its parameters name, within the slice it is run on, and the project\'s quantities; it may use numpy, the data library, and the markets domain through science.markets. It runs with your authority on this computer.'),
    el('div', { class: 'actions', style: 'border: none' }, named, el('span', {}, '.py'), key('Write it', async () => {
      const file = `${named.value.trim()}.py`;
      if (!/^[A-Za-z0-9][A-Za-z0-9_-]{0,63}\.py$/.test(file)) throw new Error('a script is named by letters, digits, _ and -');
      if (!S.scripts.includes(file)) { await api(`/api/projects/${S.name}/scripts/${file}`, { method: 'PUT', run: true, body: { code: SCRIPT.replaceAll('"mine"', `"${file.slice(0, -3)}"`) } }); S.scripts.push(file); }
      await editScript(file);
    }, 'main'), key('Close', () => { dialog.hidden = true; }, '')));
  dialog.hidden = false;
}
async function editScript(file) {
  const dialog = $('dialog'), kept = await api(`/api/projects/${S.name}/scripts/${file}`);
  const code = el('textarea', { rows: 26, spellcheck: 'false' }, kept.code), said = el('p', { class: 'tip' }, 'Kept in the project\'s scripts. The contract is the board\'s (api/python/science/board.py): a tool written here runs on a board of Tabula, and the other way round.');
  code.addEventListener('keydown', (e) => { if (e.key === 'Tab') { e.preventDefault(); code.setRangeText('    ', code.selectionStart, code.selectionEnd, 'end'); } });
  fill(dialog, el('h2', {}, file), code, said, actions(key('Keep', async () => {
    await api(`/api/projects/${S.name}/scripts/${file}`, { method: 'PUT', run: true, body: { code: code.value } });
    await loadTools();
    const tool = S.tools.find((t) => t.ref === `project/${file}`);
    said.textContent = tool?.error ? `Kept, but it cannot be read as a tool: ${tool.error}` : 'Kept: its form is at the right.';
    said.className = tool?.error ? 'wrong' : 'tip';
    delete S.forms[`project/${file}`];
    S.sel = { kind: 'tool', id: `project/${file}` };
    drawExplorer(); drawInspector();
  }, 'main'), key('Close', () => { dialog.hidden = true; }, '')));
  dialog.hidden = false;
}

async function buildEngine() {
  await api('/setup/run', { method: 'POST' });
  log('Building the engine and fetching the example\'s data: this takes some minutes the first time.');
  for (;;) {
    await sleep(4000);
    const view = await api('/setup').catch(() => null), status = await api('/api/status').catch(() => null);
    if (status) S.status = status;
    if (view?.state !== 'running') { log(status?.engine.current ? 'The engine is built.' : `The recipe ended: ${view?.message || view?.state || 'see the server'}.`, status?.engine.current ? 'said' : 'wrong'); drawInspector(); return; }
  }
}

// ---- the map --------------------------------------------------------------------------------------------------------------------

let plane = null, drawing = 0, shown = [], drawnFor = null;
// By day the pale end of a map is the ground's and stands for the least; by night the deep end is, so the map is turned
// and the pale end stands for the most. The legend's ramp turns with it.
const night = () => document.documentElement.dataset.theme === 'night';
const tint = (map, u) => { const v = Math.min(1, Math.max(0, u)); return cssColour(map.colour(night() ? 0.92 - 0.8 * v : 0.1 + 0.9 * v)); };
const HUES = colourMap('categorical-apart');

// The region the map shows, in degrees.
function viewRegion() {
  const r = $('map').getBoundingClientRect(), v = plane.view, to = W.plane(S.project.frame);
  const [west, north] = to.from((0 - v.x) / v.k, -(0 - v.y) / v.k), [east, south] = to.from((r.width - v.x) / v.k, -(r.height - v.y) / v.k);
  const round = (x) => Math.round(x * 1e5) / 1e5;
  return { west: round(west), south: round(south), east: round(east), north: round(north) };
}

async function drawMap() {
  const p = S.project, token = ++drawing, slice = sliceOf(S.slice);
  if (!p) return;
  if (!plane) plane = createPlane($('map'), { onPick: pick, onSettle: resize });
  const views = p.views.filter((v) => v.kind === 'layer' && v.on && W.tablesOf(p).has(v.table));
  const tables = await Promise.all(views.map((v) => tableOf(v.table, S.slice).catch((error) => { log(`${v.title}: ${error.message}`, 'wrong'); return null; })));
  if (token !== drawing) return;
  // The frame: the project's, or the middle of the first table that has a place.
  if (!p.frame) {
    for (const t of tables) {
      if (!t) continue;
      const at = t.lines?.length ? t.lines[0].line[0] : t.roles?.lat && t.rows ? [t.data[t.roles.lon].find(Number.isFinite), t.data[t.roles.lat].find(Number.isFinite)] : t.places?.rings ? Object.values(t.places.rings)[0]?.[0] : null;
      if (at && Number.isFinite(at[0])) { p.frame = { lon: at[0], lat: at[1] }; touch(); break; }
    }
  }
  const ground = plane.layer('ground');
  clear(ground);
  shown = [];
  if (!p.frame) { fill($('legend')); $('clock').hidden = true; note('map', views.length ? '' : 'Nothing is drawn yet: put a table with a place on the map.'); return; }
  const to = W.plane(p.frame).to, period = slice?.period || 'year';
  // The clock: the slice's interval, or what the tables with a time span.
  let from = Infinity, until = -Infinity;
  const when = W.interval(slice);
  for (const t of tables) if (t?.roles?.time) for (let i = 0; i < t.rows; i++) { const s = W.span(t, i, period); if (s) { from = Math.min(from, s.from); until = Math.max(until, Number.isFinite(s.until) ? s.until : s.from + 1); } }
  if (when && Number.isFinite(when.from)) from = Math.max(from, when.from);
  if (when && Number.isFinite(when.until)) until = Math.min(until, when.until);
  const ticks = from < until ? W.ticks(from, until, period) : [];
  const clock = S.clock;
  if (ticks.length !== clock.ticks.length || ticks[0] !== clock.ticks[0] || clock.period !== period) Object.assign(clock, { ticks, k: ticks.length - 1, period });
  const t = clock.ticks.length ? clock.ticks[Math.min(clock.k, clock.ticks.length - 1)] : null;
  drawClock();

  const box = { x0: Infinity, y0: Infinity, x1: -Infinity, y1: -Infinity };
  const grow = (x, y) => { box.x0 = Math.min(box.x0, x); box.x1 = Math.max(box.x1, x); box.y0 = Math.min(box.y0, y); box.y1 = Math.max(box.y1, y); };
  const legend = [];
  const order = { areas: 0, lines: 1, points: 2 };
  const layers = views.map((v, k) => ({ v, t: tables[k] })).filter((l) => l.t).sort((a, b) => order[a.v.layer] - order[b.v.layer]);
  let drawn = 0;
  for (const { v, t: table } of layers) {
    const g = svg('g', { class: `ms-${v.layer}` });
    ground.append(g);
    if (v.layer === 'lines') {
      let d = '';
      for (const l of table.lines || []) { const pts = l.line.map(([lon, lat]) => to(lon, lat)); for (const [x, y] of pts) grow(x, y); d += plane.path(pts); }
      g.append(svg('path', { d }));
      legend.push(el('div', { class: 'entry' }, v.title));
      continue;
    }
    const rows = table.roles?.time && t !== null ? W.rowsAt(table, t, period) : Array.from({ length: table.rows }, (_, i) => i);
    let unplaced = 0;   // rows that say no place: counted, and said
    const colours = v.colour && table.data[v.colour] ? V.scaleOf(table.data[v.colour], { root: true }) : null, map = colourMap(RAMPS.includes(v.map) ? v.map : 'orchid');
    const fillOf = (i) => { if (!colours) return null; const u = colours.u(table.data[v.colour][i]); return u === null ? null : colours.kind === 'number' ? tint(map, u) : cssColour(HUES.colour(u)); };
    if (v.layer === 'areas') {
      const rings = table.places?.rings || {}, cell = table.roles?.cell;
      for (const i of rows) {
        const ring = rings[String(table.data[cell][i])];
        if (!ring) continue;
        const pts = ring.map(([lon, lat]) => to(lon, lat));
        for (const [x, y] of pts) grow(x, y);
        const path = svg('path', { d: plane.path(pts, true), 'data-row': i });
        const c = fillOf(i);
        if (c) path.style.fill = c;
        path.$layer = { v, table };
        g.append(path);
      }
    } else {
      const lat = table.data[table.roles.lat], lon = table.data[table.roles.lon];
      const sizes = v.size && table.data[v.size] ? V.scaleOf(table.data[v.size], { root: true }) : null;
      const some = rows.length > MOST_DRAWN ? rows.slice(0, MOST_DRAWN) : rows;
      if (some.length < rows.length) log(`${v.title}: the first ${count(MOST_DRAWN)} of ${count(rows.length)} places are drawn.`);
      for (const i of some) {
        if (!Number.isFinite(lat[i]) || !Number.isFinite(lon[i])) { unplaced++; continue; }
        const [x, y] = to(lon[i], lat[i]);
        grow(x, y);
        const u = sizes ? sizes.u(table.data[v.size][i]) : null;
        const dot = svg('circle', { cx: x.toFixed(1), cy: (-y).toFixed(1), 'data-row': i, 'data-r': (sizes ? 1.6 + 6.4 * (u ?? 0) : 3).toFixed(2) });
        const c = fillOf(i);
        if (c) dot.style.fill = c;
        dot.$layer = { v, table };
        g.append(dot);
      }
    }
    drawn += rows.length;
    shown.push({ v, table, rows });
    legend.push(el('div', { class: 'entry' }, el('b', {}, v.title), ` · ${count(rows.length)}${unplaced ? ` (${count(unplaced)} with no place)` : ''}`,
      colours?.kind === 'number' ? el('div', {}, v.colour, ' ', brief(colours.lo), el('i', { class: 'ramp', style: `background:${map.gradient(night() ? { direction: 'to left' } : undefined)}` }), brief(colours.hi), colours.log ? ' (by logarithms)' : colours.root ? ' (by square roots)' : '')
        : colours ? el('div', {}, colours.kinds.slice(0, 8).map((k, n) => [el('i', { class: 'swatch', style: `background:${cssColour(HUES.colour(n))}` }), String(k)]), colours.kinds.length > 8 ? ` and ${colours.kinds.length - 8} more` : '') : null,
      v.size && v.layer === 'points' ? el('div', {}, `sized by ${v.size}`) : null));
  }
  if (slice?.region) {
    const r = slice.region, a = to(r.west, r.south), b = to(r.east, r.north);
    ground.append(svg('path', { class: 'ms-region', d: plane.path([a, [b[0], a[1]], b, [a[0], b[1]]], true) }));
    if (S.fit) { box.x0 = a[0]; box.y0 = a[1]; box.x1 = b[0]; box.y1 = b[1]; }
  }
  fill($('legend'), legend);
  note('map', `${slice ? slice.name : 'The whole'}${t !== null ? ` · ${W.formatInstant(t, period)}` : ''}${drawn ? ` · ${count(drawn)} places` : ''}`);
  drawnFor = `${S.slice || ''}|${layers.map((l) => l.v.table).sort().join()}`;
  if (S.fit && Number.isFinite(box.x0)) { plane.fit(box, 36); S.fit = false; }
  resize();
}
// Marks that keep their size on the screen whatever the zoom.
function resize() {
  if (!plane) return;
  const px = plane.px();
  for (const dot of plane.layer('ground').querySelectorAll('circle')) dot.setAttribute('r', (Number(dot.dataset.r) * px).toFixed(2));
}
function pick(_event, _place, target) {
  for (const old of plane.layer('ground').querySelectorAll('.picked')) old.classList.remove('picked');
  const layer = target?.$layer;
  if (!layer) return;
  target.classList.add('picked');
  const i = Number(target.dataset.row), name = layer.v.label || layer.table.roles?.name || layer.table.roles?.cell;
  S.sel = { kind: 'row', id: `${layer.v.id}:${i}`, table: layer.table, row: i, title: `${layer.v.title}${name ? `: ${layer.table.data[name][i]}` : ''}` };
  drawExplorer(); drawInspector();
}
// The clock is built when its steps change and only moved otherwise: a handle being dragged is not replaced under the hand.
function drawClock() {
  const box = $('clock'), c = S.clock;
  box.hidden = c.ticks.length < 2;
  if (box.hidden) { clearInterval(c.timer); c.timer = null; c.built = null; return; }
  const built = `${c.ticks.length}|${c.ticks[0]}|${c.period}`;
  if (c.built !== built) {
    c.built = built;
    const label = el('b', {}), range = el('input', { type: 'range', min: 0, max: c.ticks.length - 1, step: 1, 'aria-label': 'The clock', oninput: (e) => { c.k = Number(e.target.value); drawMap().catch(fail); } });
    const play = el('button', { class: 'key edged', type: 'button', onclick: () => {
      if (c.timer) { clearInterval(c.timer); c.timer = null; drawClock(); return; }
      if (c.k >= c.ticks.length - 1) c.k = 0;
      c.timer = setInterval(() => { if (c.k >= c.ticks.length - 1) { clearInterval(c.timer); c.timer = null; drawClock(); return; } c.k++; drawMap().catch(fail); }, 700);
      drawMap().catch(fail);
    } });
    fill(box, play, range, label);
  }
  const [play, range, label] = box.children;
  play.textContent = c.timer ? 'Pause' : 'Play';
  if (Number(range.value) !== c.k) range.value = c.k;
  label.textContent = W.formatInstant(c.ticks[c.k], c.period);
}

// ---- the table and the chart ------------------------------------------------------------------------------------------------------

async function drawTable() {
  const box = $('pane-table'), p = S.project, names = W.tablesOf(p);
  if (!S.table || !names.has(S.table)) S.table = names.keys().next().value || null;
  if (!S.table) { fill(box, el('p', { class: 'note' }, 'The project has no table yet: add a file of data.')); note('table', ''); return; }
  const name = S.table, t = await tableOf(name, S.slice);
  if (name !== S.table) return;
  const bound = Object.fromEntries(Object.entries(t.roles || {}).map(([r, c]) => [c, W.ROLES.find((x) => x.id === r)?.label || r]));
  const n = Math.min(t.rows, MOST_SHOWN);
  note('table', `${names.get(name).title} · ${count(t.rows)} rows${sliceOf(S.slice) ? ` in ${sliceOf(S.slice).name}` : ''}${t.rows > n ? ` · the first ${n} shown` : ''}`);
  fill(box, el('table', { class: 'ms-table' },
    el('thead', {}, el('tr', {}, t.columns.map((c) => el('th', {}, c.name, el('small', {}, bound[c.name] || ' '))))),
    el('tbody', {}, Array.from({ length: n }, (_, i) => el('tr', {}, t.columns.map((c) => { const v = t.data[c.name][i]; return el('td', { class: typeof v === 'number' ? 'n' : '' }, v === null || v === undefined ? '' : typeof v === 'number' ? (Number.isInteger(v) && Math.abs(v) < 1e15 && /year|id|code|block/i.test(c.name) ? String(v) : formatNumber(v)) : String(v)); }))))),
    t.lines?.length && !t.rows ? el('p', { class: 'note' }, `${count(t.lines.length)} lines, drawn on the map.`) : null);
}

async function drawChart() {
  const box = $('pane-chart'), p = S.project;
  if (S.figure) {
    const a = await api(`/api/projects/${S.name}/result?step=${S.figure.step}`), f = a.figures.find((x) => x.k === S.figure.k);
    note('chart', f?.title || 'A figure');
    fill(box, f ? el('div', { class: 'ms-chart' }, el('img', { alt: f.title || 'A figure', style: 'max-width: 100%', src: f.format === 'png' ? `data:image/png;base64,${f.data}` : `data:image/svg+xml;charset=utf-8,${encodeURIComponent(f.data)}` })) : el('p', { class: 'note' }, 'The figure is not there any more.'));
    return;
  }
  const v = viewOf(S.chart) || p.views.find((x) => x.kind === 'chart');
  if (!v || !W.tablesOf(p).has(v.table)) { fill(box, el('p', { class: 'note' }, 'No chart yet: choose a table and make a chart of it, or run a tool that answers one.')); note('chart', ''); return; }
  S.chart = v.id;
  const t = await tableOf(v.table, S.slice);
  if (S.chart !== v.id) return;
  const c = V.chartOf(t, v);
  note('chart', `${v.title || v.table}${sliceOf(S.slice) ? ` · ${sliceOf(S.slice).name}` : ''} · ${count(t.rows)} rows`);
  fill(box, el('div', { class: 'ms-chart' }, c.note ? el('p', { class: 'note' }, c.note) : c.kind === 'bars' ? barChart(c) : c.kind === 'lines' ? lineChart(c) : scatterChart(c)));
}

// ---- the boards -----------------------------------------------------------------------------------------------------------------

let board = null, boardKept = null, boardTimer = null;
async function openBoard() {
  const p = S.project;
  if (!p.boards.length) { p.boards.push({ id: W.makeId(), title: 'Board' }); touch(); drawExplorer(); }
  const entry = p.boards.find((b) => b.id === S.board) || p.boards[0];
  S.board = entry.id;
  note('board', entry.title);
  if (board && boardKept === `${S.name}/${entry.id}`) { board.focus(); return board; }
  await flush();
  const kept = await api(`/api/projects/${S.name}/boards/${entry.id}`).catch(() => null);
  const next = kept || B.emptyBoard({ id: entry.id, title: entry.title, now: now() });
  if (!board) {
    board = B.createBoard($('board'), { board: next, services: { data: (path) => api(`/data/${path}`) }, today: Math.floor(Date.now() / 86400000), insets: () => ({ left: 96 }) });
    B.createToolbar(board, $('rail'));
    board.on('say', say);
    board.on('change', () => {
      const where = boardKept, body = board.board;
      clearTimeout(boardTimer);
      boardTimer = setTimeout(() => api(`/api/projects/${where}`.replace(/\/([^/]+)$/, '/boards/$1'), { method: 'PUT', body }).catch((error) => log(`The board was not kept: ${error.message}`, 'wrong')), 800);
    });
  } else board.load(next);
  boardKept = `${S.name}/${entry.id}`;
  if (kept) board.fit();
  return board;
}

// Items laid on the board in view to the right of what stands there, each beneath the one before.
async function lay(items) {
  S.tab = 'board';
  show('board');
  const b = await openBoard(), box = B.bounds(b.board), taken = new Set(b.tables.keys()), ids = [];
  // Three columns: the notes; the tables; and beside each table the card that draws it.
  const x0 = box ? box.x + box.w + 90 : 0, y0 = box ? box.y : 0, noted = items.some((i) => i.kind === 'note');
  const left = { note: x0, table: x0 + (noted ? 350 : 0), card: x0 + (noted ? 350 : 0) + 544 }, down = { note: y0, table: y0, card: y0 };
  for (const item of items) {
    let props = item.props;
    if (item.kind === 'table') { const name = B.tableName(props.name, taken); taken.add(name); item.rename?.(name); props = { ...props, name }; down.table = down.card = Math.max(down.table, down.card); }
    const size = item.size || (item.kind === 'card' ? { w: 520, h: 380 } : B.DEFAULT_SIZE[item.kind]);
    ids.push(b.add(item.kind, typeof props === 'function' ? props() : props, { at: { x: left[item.kind] + size.w / 2, y: down[item.kind] + size.h / 2 }, size }));
    down[item.kind] += size.h + 24;
  }
  b.select([]);
  b.fit(ids);
  say('Sent to the board.');
}
const boardTable = (name, title, columns) => ({ name, title, source: `Market Studio: ${S.project.title}${sliceOf(S.slice) ? `, ${sliceOf(S.slice).name}` : ''}`, rows: Object.values(columns)[0]?.length || 0,
  columns: Object.keys(columns).map((c) => ({ name: c, kind: columns[c].some((v) => v !== null && typeof v !== 'number') || !columns[c].some((v) => v !== null) ? 'text' : 'number' })), data: columns });

// A table of the project as items of a board: itself (its first rows, if it is long) and the card that draws it as its view does;
// a chart that gathers is sent as what it gathered, so that the board's chart shows the same.
async function tableItems(name, view = null) {
  const t = await tableOf(name, S.slice), v = view || S.project.views.find((x) => x.table === name) || V.defaultView(t);
  let named = name;
  const rename = (n) => { named = n; };
  if (v.kind === 'chart') {
    const c = V.chartOf(t, v);
    if (c.note) throw new Error(c.note);
    const columns = c.kind === 'scatter' ? { [c.xlabel]: c.x, [c.ylabel]: c.y } : c.kind === 'lines' ? { [c.xlabel]: c.ticks || c.x, ...Object.fromEntries(c.series.map((s) => [W.tableName(s.name), s.y])) }
      : { [v.x || 'category']: c.labels, ...Object.fromEntries(c.series.map((s) => [W.tableName(s.name), s.values])) };
    const keys = Object.keys(columns);
    return [{ kind: 'table', props: boardTable(name, c.title, columns), rename, size: { w: 520, h: 200 } },
      { kind: 'card', props: () => ({ card: 'chart', props: { table: named, form: c.kind === 'bars' ? 'bar' : c.kind === 'lines' ? 'line' : 'scatter', x: keys[0], y: keys.slice(1).join(', '), most: v.most || 24, title: c.title } }) }];
  }
  const rows = Array.from({ length: Math.min(t.rows, MOST_SENT) }, (_, i) => i), columns = Object.fromEntries(t.columns.map((c) => [c.name, rows.map((i) => t.data[c.name][i])]));
  if (!(t.roles?.lat && t.roles?.lon) && t.roles?.cell && t.places?.rings) { const at = W.centres(t.places.rings); columns.lon = rows.map((i) => at.get(String(t.data[t.roles.cell][i]))?.[0] ?? null); columns.lat = rows.map((i) => at.get(String(t.data[t.roles.cell][i]))?.[1] ?? null); }
  const placed = columns.lat && columns.lon;
  if (t.rows > MOST_SENT) log(`${name}: the first ${count(MOST_SENT)} of ${count(t.rows)} rows went to the board.`);
  return [{ kind: 'table', props: boardTable(name, t.title || name, columns), rename, size: { w: 520, h: 200 } },
    ...(placed ? [{ kind: 'card', props: () => ({ card: 'map', props: { table: named, value: v.colour || '', size: v.size || '', label: v.label || t.roles?.name || '', title: v.title || t.title || name } }) }] : [])];
}
async function sendTable(name, view = null) { await lay(await tableItems(name, view)); }
async function sendStep(step) {
  const items = [];
  for (const o of step.outputs || []) {
    if (o.kind === 'note') {
      // What was found, and apart from it what was presumed to find it.
      const cut = o.text.search(/Presumed, not observed|The census states neither/);
      const parts = cut > 0 ? [['insight', o.text.slice(0, cut).trim()], ['assumption', o.text.slice(cut).trim()]] : [[/presumed|assum/i.test(o.text) ? 'assumption' : 'insight', o.text]];
      for (const [type, text] of parts) items.push({ kind: 'note', props: { type, text }, size: { w: 320, h: Math.min(420, 96 + Math.ceil(text.length / 44) * 19) } });
    }
    if (o.kind === 'table') items.push(...await tableItems(o.name));
  }
  await lay(items);
}

// ---- the page -------------------------------------------------------------------------------------------------------------------

for (const tab of document.querySelectorAll('.tab')) tab.addEventListener('click', () => show(tab.dataset.tab));
$('slice').addEventListener('change', (e) => { S.sel = { kind: 'slice', id: e.target.value || null }; setSlice(e.target.value || null); });
$('title').addEventListener('change', (e) => { if (S.project) { S.project.title = e.target.value.trim() || S.project.title; document.title = `${S.project.title} · Market Studio`; touch(); } });
$('open-projects').addEventListener('click', () => projectsDialog().catch(fail));
$('theme').addEventListener('click', () => {
  const night = document.documentElement.dataset.theme !== 'night';
  document.documentElement.dataset.theme = night ? 'night' : 'day';
  $('theme').textContent = night ? 'Day' : 'Night';
  try { localStorage.setItem('market-studio.theme', night ? 'night' : 'day'); } catch { /* private */ }
  if (S.project && S.tab !== 'board') show(S.tab);
});
window.addEventListener('dragover', (e) => { if (S.project && e.dataTransfer?.types.includes('Files')) e.preventDefault(); });
window.addEventListener('drop', (e) => { if (S.project && e.dataTransfer?.files.length) { e.preventDefault(); addFiles([...e.dataTransfer.files]); } });
window.addEventListener('beforeunload', () => { flush(); });
window.addEventListener('keydown', (e) => { if (e.key === 'Escape' && S.project) $('dialog').hidden = true; });

(async () => {
  const asked = new URLSearchParams(location.search).get('theme');
  try { if ((asked || localStorage.getItem('market-studio.theme')) === 'night') $('theme').click(); } catch { /* private */ }
  S.status = await api('/api/status');
  if (S.status.tools) log(S.status.tools, 'wrong');
  if (!S.status.engine.current) log('The engine of the markets domain is not built from the present sources: the market\'s tools cannot answer until it is (the project\'s panel builds it).', 'wrong');
  let last = null;
  try { last = new URLSearchParams(location.search).get('project') || localStorage.getItem('market-studio.project'); } catch { /* private */ }
  const kept = (await api('/api/projects')).projects;
  if (last && kept.some((p) => p.name === last)) await openProject(last);
  else await projectsDialog();
})().catch(fail);

// For checks that drive the page.
window.studio = { S, select, show, runTool, runPlan, openProject, tableOf, flush, get plane() { return plane; }, get board() { return board; }, get drawnFor() { return drawnFor; } };
