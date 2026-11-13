// The Dynamics Lab's page: a project is a script, its records and its studies.
//
// The script is read by the engine as it is typed, and what it declares is listed; a system is selected, and the
// tools that apply to it are offered (tools.js); a tool that is run becomes a study on the sheet, with what it was
// asked and what it rests on. A study made before the script changed is marked, and can be run again. The project
// is one document (noetarium.dynamics/1), kept on this computer under its name.

import { TOOLS, parseValues, parseWord, writeValues } from './tools.js';

const $ = (id) => document.getElementById(id);
const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
const GLYPH = { flow: '∿', map: '⋮', stochastic: '≈', rules: '→', lagrangian: '∫', field: '▦', quantum: 'ψ', composite: '⊕', derived: '⇒', empty: '·' };
const FORM = { flow: 'a flow', map: 'a map', stochastic: 'stochastic', rules: 'rules', lagrangian: 'an action', field: 'a field', quantum: 'quantum', composite: 'parts', derived: 'derived', empty: 'empty' };
const OP = { run: 'simulate' };

const project = { format: 'noetarium.dynamics/1', name: '', script: '', records: [], studies: [] };
const state = { systems: [], selected: null, record: null, tool: null, studies: [], read: 0, serial: 0, katex: null, three: null, limits: {} };

// ---- the server ----
async function call(path, options) {
  const r = await fetch(path, options);
  const body = await r.json().catch(() => ({ message: `the server answered ${r.status}` }));
  if (!r.ok) throw new Error(body.message ?? `the server answered ${r.status}`);
  return body;
}
const ask = (request) => call('/api/ask', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(request) });
const say = (text, wrong = false) => { $('state').textContent = text; $('state').classList.toggle('wrong', wrong); };

// ---- the script ----
function gutter() {
  const lines = $('script').value.split('\n');
  const begins = new Set(state.systems.map((s) => s.line)), wrong = new Set(state.systems.filter((s) => s.error).map((s) => s.error.line));
  if (state.trouble) wrong.add(state.trouble.line);
  $('gutter').innerHTML = lines.map((_, i) => `<div class="${wrong.has(i + 1) ? 'wrong' : begins.has(i + 1) ? 'begins' : ''}">${i + 1}</div>`).join('');
  $('gutter').scrollTop = $('script').scrollTop;
}

async function read() {
  const serial = ++state.read;
  project.script = $('script').value;
  try {
    const answer = await ask({ op: 'read', script: project.script });
    if (serial !== state.read) return;
    state.trouble = answer.error ?? null;
    if (!answer.error) state.systems = answer.systems;
    $('trouble').textContent = answer.error ? answer.error.message : '';
    const wrong = state.systems.filter((s) => s.error).length;
    $('tally').textContent = answer.error ? 'not read' : `${state.systems.length} system${state.systems.length === 1 ? '' : 's'}${wrong ? `, ${wrong} in error` : ''}`;
    if (!state.systems.some((s) => s.name === state.selected)) state.selected = state.systems[0]?.name ?? null;
    listSystems();
    select(state.selected);
    for (const study of state.studies) markStale(study);
  } catch (error) {
    if (serial === state.read) say(error.message, true);
  }
  gutter();
}

function listSystems() {
  $('systems').innerHTML = state.systems.map((s) => `<div class="system${s.name === state.selected && !state.record ? ' chosen' : ''}${s.error ? ' wrong' : ''}" role="option" data-name="${esc(s.name)}">
    <span class="glyph">${s.error ? '✕' : GLYPH[s.form] ?? '·'}</span><span class="name">${esc(s.name)}</span><span class="form">${s.error ? 'in error' : s.template ? 'a template' : FORM[s.form] ?? s.form}</span></div>`).join('');
}

const math = (latex, text) => (state.katex ? state.katex.renderToString(latex, { throwOnError: false, displayMode: false }) : `<span style="font-family: var(--mono); font-size: 12px">${esc(text ?? latex)}</span>`);

function card(s) {
  if (!s) return '<p class="note">The script declares no system yet.</p>';
  let html = `<h2>${esc(s.name)}</h2><div class="kind">${s.template ? `a template with the unknowns ${s.unknowns.map((u) => `?${esc(u)}`).join(', ')}` : esc(describe(s))}</div>`;
  if (s.lagrangian) html += `<div class="equation">${math(`L = ${s.lagrangian}`)}</div>`;
  if (s.action) html += `<div class="equation">${math(`S = ${s.action}`)}</div>`;
  for (const e of s.equations ?? []) {
    const left = e.left.replace(/^d(\w+)$/, 'd$1').replace(/'+$/, (p) => (s.discrete ? '_{n+1}' : '^{' + '\\prime'.repeat(p.length) + '}'));
    html += `<div class="equation">${math(`${left.replace(/_(?!\{)/g, '\\_')} = ${e.latex}`, `${e.left} = ${e.text ?? ''}`)}</div>`;
  }
  for (const r of s.rules ?? []) html += `<div class="equation"><span style="font-family: var(--mono); font-size: 12px">${esc(r.name)} : ${esc(r.left)} → ${esc(r.right)}${s.weights === 'probability' ? `  (${Number(r.probability.toPrecision(3))})` : ''}</span></div>`;
  for (const e of s.events ?? []) html += `<div class="note">an event when ${esc(e)}</div>`;
  if (s.conserved?.length) html += `<div class="note">declared conserved: ${s.conserved.map((c) => math(c)).join('; ')}</div>`;
  if (s.parts) html += `<div class="note">parts: ${s.parts.map((p) => `${esc(p.name)}, ${esc(FORM[p.form] ?? p.form)}`).join('; ')}. ${s.joined ? 'They are joined by an action: the whole is one system given by the sum of their actions and the interaction.' : s.merges ? 'They merge into one flow.' : 'Of different forms: advanced side by side.'}</div>`;
  if (s.note) html += `<div class="note">${esc(s.note)}</div>`;
  if (s.error) html += `<div class="note wrong">${esc(s.error.message)}</div>`;
  return html;
}

function describe(s) {
  if (s.form === 'quantum') return `an open quantum system of dimension ${s.dimension}, with ${s.jumps} jump${s.jumps === 1 ? '' : 's'}`;
  if (s.form === 'rules') return `rules on words, ${s.context_free ? 'context-free' : 'with context'}; weights: ${s.weights}`;
  if (s.form === 'stochastic') return `${s.discrete ? 'a map with draws' : `a stochastic differential, read in ${s.calculus === 'ito' ? "Itô's" : "Stratonovich's"} calculus`}; ${s.states.length} component${s.states.length === 1 ? '' : 's'}`;
  if (s.form === 'field') return `a field weighted by its action, ${s.background ? 'on a declared background' : 'with no background declared'}`;
  if (s.form === 'derived') return `derived: ${s.derivation} ${s.source}`;
  const what = { flow: 'a flow', map: 'a map', lagrangian: 'given by an action', composite: 'made of parts' }[s.form] ?? s.form;
  return `${what}${s.states ? `; ${s.states.length} component${s.states.length === 1 ? '' : 's'}: ${s.states.join(', ')}` : ''}`;
}

function select(name, record = null) {
  state.selected = name, state.record = record;
  const s = state.systems.find((x) => x.name === name);
  $('card').innerHTML = record ? `<h2>${esc(record.name)}</h2><div class="kind">a record: ${record.t.length.toLocaleString()} samples of ${record.names.join(', ')}</div>` : card(s);
  const offered = record ? TOOLS.filter((t) => t.record) : [...TOOLS.filter((t) => s && t.on(s)), ...TOOLS.filter((t) => t.free || t.record)];
  $('tools').innerHTML = offered.map((t) => `<button class="key small edged" type="button" data-tool="${t.id}" title="${esc(t.about)}">${esc(t.title)}</button>`).join('') || '<span class="note">No tool applies to this yet.</span>';
  $('form').innerHTML = '';
  state.tool = null;
  listSystems();
  listRecords();
}

// ---- a tool's form ----
function openTool(id) {
  const tool = TOOLS.find((t) => t.id === id), s = state.systems.find((x) => x.name === state.selected);
  state.tool = tool;
  for (const b of $('tools').querySelectorAll('button')) b.classList.toggle('on', b.dataset.tool === id);
  const ctx = { systems: state.systems };
  const fields = tool.fields(s ?? { states: [], params: [] }, ctx);
  const input = (f) => {
    const id = `f-${f.key.replace('.', '-')}`, attr = `id="${id}" data-key="${f.key}" data-kind="${f.kind}"`;
    if (f.kind === 'check') return `<label class="check"><input type="checkbox" ${attr}${f.default ? ' checked' : ''}> ${esc(f.label)}</label>`;
    let control;
    if (f.kind === 'component') control = `<select ${attr}>${(s?.states ?? []).map((n) => `<option${n === f.default ? ' selected' : ''}>${esc(n)}</option>`).join('')}</select>`;
    else if (f.kind === 'parameter') control = `<select ${attr}>${(s?.params ?? []).map((p) => `<option${p.name === f.default ? ' selected' : ''}>${esc(p.name)}</option>`).join('')}</select>`;
    else if (f.kind === 'choice') control = `<select ${attr}>${f.options.map((o) => `<option${o === f.default ? ' selected' : ''}>${esc(o)}</option>`).join('')}</select>`;
    else if (f.kind === 'record') {
      const runnable = state.systems.filter((x) => x.states && !x.template && !x.error && x.form !== 'quantum' && x.form !== 'rules');
      const chosen = state.record ? `r:${state.record.id}` : `s:${state.selected}`;
      control = `<select ${attr}>${project.records.map((r) => `<option value="r:${r.id}"${chosen === `r:${r.id}` ? ' selected' : ''}>${esc(r.name)}</option>`).join('')}${runnable.map((x) => `<option value="s:${esc(x.name)}"${chosen === `s:${x.name}` ? ' selected' : ''}>a run of ${esc(x.name)}</option>`).join('')}</select>`;
    } else control = `<input type="text" ${attr} value="${esc(f.kind === 'values' ? writeValues(f.default) : f.default ?? '')}"${f.pairs ? ' data-pairs="1"' : ''}${f.list ? ' data-list="1"' : ''}${f.names ? ' data-names="1"' : ''}${f.word ? ' data-word="1"' : ''}>`;
    return `<label${f.only ? ` data-only="${f.only}"` : ''}>${esc(f.label)}${control}</label>`;
  };
  $('form').innerHTML = `<p class="about">${esc(tool.about)}</p>${fields.map(input).join('')}<div class="row"><button id="go" class="key main" type="button">${esc(tool.title)}</button></div>`;
  const onlyRuns = () => { const r = $('f-record'); for (const l of $('form').querySelectorAll('[data-only="run"]')) l.hidden = r && !r.value.startsWith('s:'); };
  $('f-record')?.addEventListener('change', onlyRuns);
  onlyRuns();
  $('go').addEventListener('click', () => addStudy(tool.id, collect(tool)));
}

function collect(tool) {
  const request = tool.free || tool.record ? {} : { system: state.selected };
  const put = (key, value) => {
    if (!key.includes('.')) return void (request[key] = value);
    const [head, tail] = key.split('.');
    if (request[head]?.simulate) request[head].simulate[tail] = value;
  };
  for (const el of $('form').querySelectorAll('[data-key]')) {
    const { key, kind } = el.dataset, raw = el.type === 'checkbox' ? el.checked : el.value.trim();
    if (kind === 'check') { if (raw) put(key, true); continue; }
    if (kind === 'record') { request.record = raw.startsWith('r:') ? { id: raw.slice(2) } : { simulate: { system: raw.slice(2) } }; continue; }
    if (raw === '') continue;
    if (kind === 'number') { if (Number.isFinite(Number(raw))) put(key, Number(raw)); continue; }
    if (kind === 'values') { const v = parseValues(raw); if (Object.keys(v).length) put(key, v); continue; }
    if (el.dataset.pairs) { put(key, raw.split(';').map((p) => p.trim().split(/\s+/)).filter((p) => p.length === 2)); continue; }
    if (el.dataset.word) { put(key, parseWord(raw)); continue; }
    if (el.dataset.list) { put(key, raw.split(',').map((x) => x.trim()).filter(Boolean).map((x) => (el.dataset.names ? x : Number(x)))); continue; }
    put(key, raw);
  }
  return request;
}

// ---- studies ----
const hash = (text) => { let h = 2166136261; for (let i = 0; i < text.length; i++) h = Math.imul(h ^ text.charCodeAt(i), 16777619); return h >>> 0; };

function addStudy(toolId, request, quiet = false) {
  const study = { id: ++state.serial, tool: toolId, request, status: 'asking', answer: null, dispose: null };
  state.studies.push(study);
  const el = document.createElement('article');
  el.className = 'study', el.id = `study-${study.id}`;
  $('sheet').querySelector('.empty')?.remove();
  $('sheet').append(el);
  if (!quiet) el.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
  return runStudy(study);
}

async function runStudy(study) {
  const tool = TOOLS.find((t) => t.id === study.tool), el = $(`study-${study.id}`);
  study.status = 'asking', study.hash = hash(project.script);
  paint(study);
  try {
    const request = { ...study.request, op: OP[study.tool] ?? study.tool, script: project.script };
    if (typeof request.word === 'string') request.word = parseWord(request.word);
    if (request.record?.id) {
      const r = project.records.find((x) => x.id === request.record.id);
      if (!r) throw new Error('the record this study was made on is no longer in the project');
      request.record = { names: r.names, t: r.t, x: r.x };
    }
    study.answer = await ask(request);
    study.status = 'done';
  } catch (error) {
    study.status = 'refused', study.error = error.message;
  }
  paint(study);
  if (study.status === 'done') {
    const shown = tool.render(study.answer, study, { append });
    const host = el.querySelector('[data-round]');
    if (host && shown.round) {
      try {
        const three = await loadThree();
        study.dispose?.();
        study.dispose = shown.round(host, { ...three, ink: getComputedStyle(document.body).getPropertyValue('--ink-2').trim() }).dispose;
      } catch { host.innerHTML = '<p class="note" style="padding: 12px">The view in the round needs three.js, which could not be loaded.</p>'; }
    }
  }
}

function paint(study) {
  const tool = TOOLS.find((t) => t.id === study.tool), el = $(`study-${study.id}`), r = study.request;
  const of = r.system ?? r.template ?? (r.record?.simulate ? `a run of ${r.record.simulate.system}` : r.record?.id ? project.records.find((x) => x.id === r.record.id)?.name : '') ?? '';
  let body = '<p class="said">Asking the engine…</p>', actions = [];
  if (study.status === 'refused') body = `<p class="said refused">${esc(study.error)}</p>`;
  if (study.status === 'done') { const shown = tool.render(study.answer, study, { append }); body = shown.html; actions = shown.actions ?? []; }
  el.innerHTML = `<header><h3>${esc(tool.title)}</h3><span class="of">${esc(of)}</span><span class="tag" hidden></span><span class="spacer"></span>
    ${actions.map((a, i) => `<button class="key small edged" type="button" data-action="${i}">${esc(a.label)}</button>`).join('')}
    <button class="key small" type="button" data-again>Again</button><button class="key small" type="button" data-remove>Remove</button></header><div class="body">${body}</div>`;
  el.querySelector('[data-again]').addEventListener('click', () => runStudy(study));
  el.querySelector('[data-remove]').addEventListener('click', () => { study.dispose?.(); state.studies = state.studies.filter((s) => s !== study); el.remove(); if (!state.studies.length) blank(); });
  actions.forEach((a, i) => el.querySelector(`[data-action="${i}"]`).addEventListener('click', a.run));
  markStale(study);
}

function markStale(study) {
  const tag = $(`study-${study.id}`)?.querySelector('.tag');
  if (!tag) return;
  const stale = study.status !== 'asking' && study.hash !== hash(project.script);
  tag.hidden = !stale, tag.textContent = stale ? 'made before the script changed' : '';
}

function blank() {
  $('sheet').innerHTML = '<p class="empty">Select a system at the left and a tool at the right. Each tool that is run becomes a study here, with what it was asked and what it rests on.</p>';
}

function append(text) {
  $('script').value = `${$('script').value.replace(/\s*$/, '')}\n\n${text.trim()}\n`;
  read();
  say('Added to the script.');
}

async function loadThree() {
  state.three ??= Promise.all([import('three'), import('three/addons/controls/OrbitControls.js')]).then(([THREE, controls]) => ({ THREE, OrbitControls: controls.OrbitControls }));
  return state.three;
}

// ---- projects, examples, records ----
function clearStudies() {
  for (const s of state.studies) s.dispose?.();
  state.studies = [];
  blank();
}

async function openDocument(doc) {
  clearStudies();
  project.name = doc.name ?? '', project.records = doc.records ?? [];
  $('name').value = project.name;
  $('script').value = doc.script ?? '';
  await read();
  for (const s of doc.studies ?? []) await addStudy(s.tool, s.request, true);
  $('sheet').scrollTop = 0;
}

function openExample(example) {
  const studies = [...example.text.matchAll(/^#!\s*(\{.*\})\s*$/gm)].map((m) => { const { tool, ...request } = JSON.parse(m[1]); return { tool, request }; });
  return openDocument({ name: '', script: example.text.replace(/^#!.*\n/gm, ''), studies });
}

async function keep() {
  const name = $('name').value.trim();
  if (!name) return say('Give the project a name to keep it.', true);
  try {
    await call(`/api/projects/${encodeURIComponent(name)}`, { method: 'PUT', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ ...project, name, script: $('script').value, studies: state.studies.map((s) => ({ tool: s.tool, request: s.request })) }) });
    project.name = name;
    say(`Kept as “${name}”.`);
    listKept(name);
  } catch (error) { say(error.message, true); }
}

async function listKept(chosen) {
  const kept = await call('/api/projects').catch(() => []);
  $('kept-pick').hidden = !kept.length;
  $('kept').innerHTML = `<option value="">…</option>${kept.map((k) => `<option${k.name === chosen ? ' selected' : ''}>${esc(k.name)}</option>`).join('')}`;
}

function listRecords() {
  $('records').innerHTML = project.records.map((r) => `<div class="record${state.record?.id === r.id ? ' chosen' : ''}" data-id="${r.id}"><span>${esc(r.name)}</span><span class="size">${r.t.length.toLocaleString()} × ${r.names.length}</span></div>`).join('')
    || '<p class="note" style="padding: 2px 8px">None yet. A record is a table: a column of times, then one for each component. A run of a system serves as one too.</p>';
}

// A table of numbers as text: commas, tabs or spaces; a first line of names if it is not numbers.
function bring(file, text) {
  const rows = text.split(/\r?\n/).map((l) => l.trim()).filter((l) => l && !l.startsWith('#')).map((l) => l.split(/[,\t;]|\s+/).map((c) => c.trim()).filter((c) => c !== ''));
  if (rows.length < 3) throw new Error('a record needs at least three lines');
  const named = rows[0].some((c) => !Number.isFinite(Number(c)));
  const head = named ? rows.shift() : rows[0].map((_, i) => (i ? `x${i}` : 't'));
  const data = rows.map((r) => r.map(Number)).filter((r) => r.length === head.length && r.every(Number.isFinite));
  if (data.length < 3 || head.length < 2) throw new Error('a record is a column of times and at least one column of values, in rows of one length');
  const record = { id: `r${Date.now().toString(36)}`, name: file.name.replace(/\.[^.]+$/, ''), names: head.slice(1), t: data.map((r) => r[0]), x: data.map((r) => r.slice(1)) };
  project.records.push(record);
  select(state.selected, record);
  say(`Brought ${record.name}: ${record.t.length.toLocaleString()} samples.`);
}

// ---- beginning ----
async function begin() {
  blank();
  document.documentElement.dataset.theme = localStorage.getItem('dynamics-lab-theme') ?? 'day';
  $('theme').textContent = document.documentElement.dataset.theme === 'night' ? 'Day' : 'Night';
  $('theme').addEventListener('click', () => {
    const night = document.documentElement.dataset.theme !== 'night';
    document.documentElement.dataset.theme = night ? 'night' : 'day';
    localStorage.setItem('dynamics-lab-theme', night ? 'night' : 'day');
    $('theme').textContent = night ? 'Day' : 'Night';
  });
  import('https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.mjs').then((m) => { state.katex = m.default; select(state.selected, state.record); }).catch(() => {});

  let timer = 0;
  $('script').addEventListener('input', () => { gutter(); clearTimeout(timer); timer = setTimeout(read, 350); });
  $('script').addEventListener('scroll', () => { $('gutter').scrollTop = $('script').scrollTop; });
  $('systems').addEventListener('click', (e) => { const el = e.target.closest('.system'); if (el) select(el.dataset.name); });
  $('tools').addEventListener('click', (e) => { const b = e.target.closest('[data-tool]'); if (b) openTool(b.dataset.tool); });
  $('records').addEventListener('click', (e) => { const el = e.target.closest('.record'); if (el) select(state.selected, project.records.find((r) => r.id === el.dataset.id)); });
  $('bring').addEventListener('change', async (e) => { const f = e.target.files[0]; if (!f) return; try { bring(f, await f.text()); } catch (error) { say(error.message, true); } e.target.value = ''; });
  $('keep').addEventListener('click', keep);
  $('kept').addEventListener('change', async (e) => { if (e.target.value) openDocument(await call(`/api/projects/${encodeURIComponent(e.target.value)}`)).catch((error) => say(error.message, true)); });
  $('guide-open').addEventListener('click', async () => {
    const g = await call('/api/grammar').catch((error) => ({ text: error.message }));
    $('guide-body').innerHTML = `<p class="note">A script declares systems. A system gives its state, and then its change, its structure, its constraints and what is told apart; or it is derived from another, or made of parts. The examples show each form. This is the grammar the engine reads:</p><pre>${esc(g.text)}</pre>`;
    $('guide').showModal();
  });
  $('guide-close').addEventListener('click', () => $('guide').close());

  const status = await call('/api/status').catch((error) => ({ built: false, message: error.message }));
  state.limits = status.limits ?? {};
  $('keep').hidden = status.keeps === false;
  if (!status.built) return setup();
  const examples = await call('/api/examples');
  $('examples').innerHTML = examples.map((x) => `<option value="${x.id}" title="${esc(x.about)}">${esc(x.title)}</option>`).join('');
  $('examples').addEventListener('change', (e) => openExample(examples.find((x) => x.id === e.target.value)));
  // The catalogue's models, by field: one chosen is added to the script as it is declared there.
  const { models } = await call('/api/catalogue').catch(() => ({ models: [] }));
  const fields = [...new Set(models.map((m) => m.field))];
  $('models').innerHTML = `<option value="">add one…</option>${fields.map((f) => `<optgroup label="${esc(f)}">${models.filter((m) => m.field === f).map((m) => `<option value="${m.id}" title="${esc(`${m.about} (${m.source})`)}">${esc(m.title)}</option>`).join('')}</optgroup>`).join('')}`;
  $('models').addEventListener('change', (e) => {
    const model = models.find((m) => m.id === e.target.value);
    e.target.value = '';
    if (!model) return;
    if (state.systems.some((s) => s.name === model.id)) { $('state').textContent = `The script already declares ${model.id}.`; return; }
    append(`# ${model.title} (${model.source}). ${model.about}
${model.text.trimEnd()}`);
  });
  listKept();
  const wanted = new URLSearchParams(location.search).get('example');
  const first = examples.find((x) => x.id === wanted) ?? examples[0];
  if (first) { $('examples').value = first.id; await openExample(first); }
  document.body.dataset.ready = '1';
}

async function setup() {
  const view = await call('/setup').catch((error) => ({ about: error.message }));
  $('setup-about').textContent = view.about ?? '';
  $('setup-state').textContent = view.writable === false ? view.refusal : '';
  $('setup-run').disabled = view.writable === false;
  $('setup-close').addEventListener('click', () => $('setup').close());
  $('setup-run').addEventListener('click', async () => {
    $('setup-run').disabled = true;
    $('setup-state').textContent = 'Building: about a minute and a half…';
    await call('/setup/run', { method: 'POST' }).catch((error) => { $('setup-state').textContent = error.message; });
    const wait = setInterval(async () => {
      const s = await call('/api/status').catch(() => ({}));
      if (s.built) { clearInterval(wait); location.reload(); }
    }, 3000);
  });
  $('setup').showModal();
}

begin();
