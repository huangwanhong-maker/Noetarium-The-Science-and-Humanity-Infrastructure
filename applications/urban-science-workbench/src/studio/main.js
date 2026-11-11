// The Studio (the design note of 2026-11-07): a page of its own beside Explore, holding one rendering project at a time. A project
// is a canvas, the figure it makes, of frames: the world (its own globe, camera and clock, and the layers drawn on it: data from the
// library, what its scripts draw, views captured in Explore), 2D maps following the world, charts, drawings and text. Its shared
// resources and its scripts, in JavaScript, Python and C++, are run by a controller in a schedule while the project plays; a script
// is given the world's range in space and time and the frames, and draws on the world or into any frame. A script made or saved
// while it plays is loaded (compiled, for C++) and joins the next tick.
//
// Explore is not touched: this page shares only the libraries beneath both and the server.

import { createScene } from './scene.js';
import { createCanvas } from './canvas.js';
import { createKernels } from './kernels.js';
import { createController } from './controller.js';
import { TEMPLATES } from './templates.js';
import { DATA, readRecord, marksOf } from './sources.js';
import { h } from '/src/dom.js';
import { LANGUAGES, EXTENSIONS, freshId, problemsOf, withCanvas, placeFrame } from '/visualization/scene/index.js';

const HEADERS = { 'Content-Type': 'application/json', 'X-Workbench': 'studio' };
async function post(path, body) {
  const r = await fetch(path, { method: 'POST', priority: 'high', headers: HEADERS, body: JSON.stringify(body) });
  const a = await r.json().catch(() => ({}));
  if (!r.ok) throw Object.assign(new Error(a.message || `the server answered ${r.status}`), { line: a.line ?? null, log: a.log || [] });
  return a;
}
async function get(path) {
  const r = await fetch(path, { priority: 'high' });
  const a = await r.json().catch(() => ({}));
  if (!r.ok) throw new Error(a.message || `the server answered ${r.status}`);
  return a;
}
const remembered = (k, d) => { try { return localStorage.getItem(k) ?? d; } catch { return d; } };
const remember = (k, v) => { try { localStorage.setItem(k, v); } catch { /* a private window */ } };

const STATUS = { loading: 'loading', compiling: 'compiling', ready: 'running', paused: 'paused', stopped: 'stopped', error: 'failed', refused: 'refused', stale: 'running the version before' };
// What a lamp shows: a refusal of the schedule, a version that did not load while the one before runs, or the system's state.
const shown = (s) => (s ? (s.pending || (s.refused ? 'refused' : s.stale ? 'stale' : s.status)) : 'stopped');
const KIND = { script: 'script', data: 'data', capture: 'capture' };
const FRAME = { world: 'world', map: 'map', chart: 'chart', drawing: 'drawing', text: 'text' };
const FRAME_ID = /^[A-Za-z0-9_-]{1,40}$/;
const $ = (id) => document.getElementById(id);
// replaceChildren writes null as text: what is absent is left out first.
const fill = (el, ...kids) => el.replaceChildren(...kids.flat().filter((x) => x !== null && x !== undefined && x !== false));
const utc = (ms) => new Date(ms).toISOString().replace('T', ' ').slice(0, 19) + ' UTC';

const S = {
  name: null, project: null,
  code: new Map(),        // file → its text as saved
  buffers: new Map(),     // script id → its text in the editor, if not yet saved
  selected: { kind: 'project' }, tab: 'code',
  lines: [], captures: [], projects: [], refusal: null, kernelState: null,
  records: new Map(),     // data layer id → its record
  scene: null, kernels: null, controller: null, canvas: null, world: null, dismissed: new Set(),
  editor: null, live: {}, deleteArmed: null,
};

// ---- the console ---------------------------------------------------------------------------------------------------------

let consoleDue = false;
function say(id, text, kind = 'out') {
  S.lines.push({ at: Date.now(), id, text: String(text), kind });
  if (S.lines.length > 600) S.lines.splice(0, S.lines.length - 600);
  if (!consoleDue) { consoleDue = true; requestAnimationFrame(() => { consoleDue = false; if (S.tab === 'console') renderConsole(); else markTab(); }); }
}

// ---- the project ---------------------------------------------------------------------------------------------------------

const P = () => S.project;
const scriptOf = (id) => P()?.scripts.find((s) => s.id === id);
const layerOf = (id) => P()?.layers.find((l) => l.id === id);
const resourceOf = (name) => P()?.resources.find((r) => r.name === name);
const systemOf = (id) => S.controller?.state.systems.get(id);
const frameOf = (id) => P()?.frames.find((f) => f.id === id);

function serialised() {
  const p = P();
  return { ...p, resources: p.resources.map(({ value, ...r }) => (r.saved === false || r.type === 'record' || value === undefined ? r : { ...r, value })) };
}
async function saveProject(quiet = false) {
  if (!S.name) return;
  try {
    const a = await post(`/projects/${encodeURIComponent(S.name)}/save`, { project: serialised() });
    P().meta.saved = a.saved;
    if (!quiet) say('project', `saved at ${a.saved}`, 'quiet');
    renderBar();
  } catch (error) { say('project', `not saved: ${error.message}`, 'error'); }
}

async function closeProject() {
  const C = S.controller;
  if (!C) return;
  C.hold();
  for (const id of [...C.state.systems.keys()]) await C.stop(id);
  for (const l of P().layers) S.scene.clear(l.id);
  S.controller = null;
  S.records.clear();
  overlay();
}

async function openProject(name) {
  await closeProject();
  const a = await get(`/projects/${encodeURIComponent(name)}`);
  const p = a.project;
  p.layers ||= [], p.resources ||= [], p.scripts ||= [], p.captures ||= [];
  withCanvas(p);
  for (const problem of problemsOf(p)) say('project', problem, 'error');
  S.name = a.name, S.project = p;
  S.code = new Map(Object.entries(a.scripts || {}));
  S.buffers.clear();
  S.selected = { kind: 'project' };
  remember('studio-project', a.name);
  document.title = `${p.meta.title} · Studio`;
  S.dismissed.clear();
  S.canvas.clearContents();
  S.canvas.clearHistories();
  S.canvas.build(S.world);
  S.scene.setCamera(p.scene.camera);
  S.scene.setTime(p.scene.clock.start_ms);
  S.controller = createController({
    project: P, kernels: S.kernels, scene: S.scene, onChange, say,
    range: () => S.scene.rangeOf(S.canvas.scale),
    frames: () => Object.fromEntries(P().frames.map((f) => [f.id, { kind: f.kind, x: f.x, y: f.y, w: f.w, h: f.h }])),
    onFrameMarks,
  });
  S.controller.seed();
  S.controller.setRate(p.scene.clock.rate || 1);
  renderAll();
  say('project', `opened ${p.meta.title}`, 'quiet');
  for (const l of p.layers) if (l.kind === 'data') readData(l);
  overlay();
  // Its scripts are loaded at once (the C++ compiled), one after another: those in JavaScript first, being quick.
  const order = [...p.scripts].sort((x, y) => (x.language === 'js' ? 0 : 1) - (y.language === 'js' ? 0 : 1));
  for (const s of order) await loadScript(s);
  kernelState();
}

async function newProject() {
  const title = `A project of ${new Date().toISOString().slice(0, 10)}`;
  try {
    const a = await post('/projects', { title });
    await listProjects();
    await openProject(a.name);
  } catch (error) { say('project', `no project made: ${error.message}`, 'error'); }
}

async function listProjects() {
  try {
    const a = await get('/projects');
    S.projects = a.projects, S.refusal = a.refusal;
  } catch (error) { say('project', `the projects could not be listed: ${error.message}`, 'error'); }
}
async function listCaptures() {
  try { S.captures = (await get('/captures')).captures || []; } catch { S.captures = []; }
}
async function kernelState() {
  try { S.kernelState = await get('/kernels'); } catch { S.kernelState = null; }
  if (S.selected.kind === 'project') renderInspector();
}

// ---- scripts ---------------------------------------------------------------------------------------------------------------

async function loadScript(script) {
  if (!S.controller) return;
  await S.controller.load(script, S.code.get(script.file) ?? '');
}

async function newScript(language) {
  if (!S.name) return;
  const used = new Set(P().scripts.map((s) => s.id));
  const id = freshId({ js: 'sketch', python: 'study', cpp: 'kernel' }[language], used);
  const file = `${id}${EXTENSIONS[language]}`;
  const code = TEMPLATES[language](`A new system in ${LANGUAGES[language]}`);
  try { await post(`/projects/${encodeURIComponent(S.name)}/script`, { file, code }); } catch (error) { say(id, `not made: ${error.message}`, 'error'); return; }
  S.code.set(file, code);
  const script = { id, file, language, enabled: true, params: {} };
  P().scripts.push(script);
  P().layers.push({ id: `script:${id}`, title: id, kind: 'script', script: id, visible: true, opacity: 1 });
  S.selected = { kind: 'script', id };
  S.tab = 'code';
  await saveProject(true);
  renderAll();
  say(id, `made ${file}${S.controller.state.playing ? '; it joins the schedule once loaded, while the others run' : ''}`, 'quiet');
  await loadScript(script);
}

async function saveScript(id) {
  const script = scriptOf(id);
  if (!script) return;
  const text = S.buffers.get(id) ?? S.code.get(script.file) ?? '';
  try { await post(`/projects/${encodeURIComponent(S.name)}/script`, { file: script.file, code: text }); } catch (error) { say(id, `not saved: ${error.message}`, 'error'); return; }
  S.code.set(script.file, text);
  S.buffers.delete(id);
  editorHead();
  await loadScript(script);
}

async function deleteScript(id) {
  const script = scriptOf(id);
  if (!script) return;
  await S.controller.remove(id);
  try { await post(`/projects/${encodeURIComponent(S.name)}/script-delete`, { file: script.file }); } catch (error) { say(id, `its file was kept: ${error.message}`, 'error'); }
  P().scripts = P().scripts.filter((s) => s.id !== id);
  P().layers = P().layers.filter((l) => l.script !== id);
  S.code.delete(script.file);
  S.buffers.delete(id);
  if (S.selected.id === id) S.selected = { kind: 'project' };
  await saveProject(true);
  say(id, 'deleted', 'quiet');
  renderAll();
}

// ---- data layers and captures ---------------------------------------------------------------------------------------------

function addData(source) {
  const id = `data:${source}`;
  if (!layerOf(id)) {
    P().layers.push({ id, title: DATA[source].title, kind: 'data', source, visible: true, opacity: 1, style: { windowDays: 0 } });
    if (!resourceOf(source)) P().resources.push({ name: source, type: 'record', initial: null, saved: false, layer: id });
    readData(layerOf(id));
    saveProject(true);
  }
  S.selected = { kind: 'layer', id };
  renderAll();
}
async function readData(layer) {
  try {
    const record = await readRecord(layer.source);
    S.records.set(layer.id, record);
    S.controller?.setValue(layer.source, record);
    drawData(layer);
    say(layer.id, `read: ${record.title}`, 'quiet');
    if (S.selected.kind === 'layer' && S.selected.id === layer.id) renderInspector();
  } catch (error) { say(layer.id, error.message, 'error'); }
}
function drawData(layer) {
  const record = S.records.get(layer.id);
  if (!record || !S.controller) return;
  S.scene.draw(layer.id, marksOf(layer.source, record, { time_ms: S.controller.state.time_ms, windowDays: layer.style?.windowDays || 0 }), { visible: layer.visible !== false, opacity: layer.opacity ?? 1 });
}
const timeBound = (layer) => layer.kind === 'data' && (S.records.get(layer.id)?.kind === 'cells' || (layer.style?.windowDays || 0) > 0);

function takeView(capture) {
  if (capture.camera) S.scene.setCamera(capture.camera, 1.6);
  if (Number.isFinite(capture.time_ms) && S.controller) { S.controller.state.time_ms = capture.time_ms; S.scene.setTime(capture.time_ms); }
  say('capture', `took the view of ${capture.title || 'a capture'}`, 'quiet');
}
function layCapture(capture) {
  const used = new Set(P().layers.map((l) => l.id));
  const id = `capture:${freshId(capture.title || 'capture', used).slice(0, 40)}`;
  P().captures.push({ ...capture, id });
  P().layers.push({ id, title: capture.title || 'A capture', kind: 'capture', capture: id, visible: true, opacity: 0.7 });
  // The figures its layers gave (a front of alternatives, a curve) stand as charts of their own, each a frame in a free corner.
  const taken = new Set(P().frames.map((f) => f.id));
  for (const layer of capture.layers || [])
    for (const m of layer.figures || []) {
      const fid = freshId(String(m.title || 'figure').toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '').slice(0, 32) || 'figure', taken);
      taken.add(fid);
      P().frames.push({ id: fid, kind: 'chart', title: m.title || fid, ...placeFrame(P().canvas, P().frames, 460, 260), back: 'vellum', chart: { mark: m } });
    }
  S.canvas.build(S.world);
  takeView(capture);
  S.selected = { kind: 'layer', id };
  saveProject(true);
  renderAll();
  overlay();
}
// A capture's plug-in layers are not yet drawn again by the Studio: they stand as the capture's image, over the scene, in its view.
function overlay() {
  const img = $('overlay');
  const top = P() ? [...P().layers].reverse().find((l) => l.kind === 'capture' && l.visible !== false) : null;
  const cap = top && P().captures.find((c) => c.id === top.capture);
  if (!cap || !cap.preview) { img.hidden = true; return; }
  img.src = cap.preview;
  img.style.opacity = String(top.opacity ?? 0.7);
  img.hidden = false;
}

// ---- frames ---------------------------------------------------------------------------------------------------------------

// A chart or a drawing a script draws into a frame; a frame it names that is not on the canvas is made, in a free corner, unless it
// was taken away in this session.
function onFrameMarks(by, marks) {
  for (const m of marks) {
    if (!FRAME_ID.test(String(m.frame || ''))) { say(by, `a ${m.kind} names no frame, or one that is not a name ("${m.frame}")`, 'error'); continue; }
    if (!frameOf(m.frame)) {
      if (S.dismissed.has(m.frame)) continue;
      const kind = m.kind === 'chart' ? 'chart' : 'drawing';
      const spot = placeFrame(P().canvas, P().frames, kind === 'chart' ? 440 : 220, kind === 'chart' ? 230 : 220);
      P().frames.push({ id: m.frame, kind, title: m.title || m.frame, ...spot, back: 'vellum', ...(kind === 'chart' ? { chart: {} } : {}) });
      S.canvas.build(S.world);
      say(by, `made the frame "${m.frame}" for its ${m.kind}; move it with arrange`, 'quiet');
      saveProject(true);
      renderTree();
    } else if (frameOf(m.frame).kind !== (m.kind === 'chart' ? 'chart' : 'drawing')) continue;
    S.canvas.draw(m.frame, m, by);
  }
}

function addFrame(kind) {
  const used = new Set(P().frames.map((f) => f.id));
  const id = freshId(kind, used);
  const size = { map: [380, 240], chart: [460, 240], drawing: [220, 220], text: [720, 80] }[kind];
  const spot = placeFrame(P().canvas, P().frames, ...size);
  const f = { id, kind, title: { map: 'A map', chart: 'A chart', drawing: 'A drawing', text: 'Text' }[kind], ...spot, back: kind === 'text' ? 'none' : 'vellum' };
  if (kind === 'map') f.map = { follows: 'world', projection: 'equal-earth' };
  if (kind === 'chart') f.chart = {};
  if (kind === 'text') f.text = { text: 'A caption', size: 22, font: 'serif', align: 'left' };
  P().frames.push(f);
  S.canvas.build(S.world);
  choose('frame', id);
  saveProject(true);
}

function setArrange(on) {
  S.canvas.setArrange(on);
  renderBar();
}

async function exportImage() {
  try {
    const url = await S.canvas.image(2);
    const a = h('a', { href: url, download: `${(P()?.meta.title || 'figure').replace(/[^\w-]+/g, '-').toLowerCase()}-${new Date().toISOString().slice(0, 19).replace(/:/g, '')}.png` });
    document.body.append(a);
    a.click();
    a.remove();
    say('export', `the canvas as one image, at twice its size (${P().canvas.width * 2} × ${P().canvas.height * 2})`, 'quiet');
  } catch (error) { say('export', `not exported: ${error.message}`, 'error'); }
}

// ---- what the controller says ---------------------------------------------------------------------------------------------

let scheduleDue = 0;
function onChange(kind) {
  if (kind === 'tick') {
    renderReadout();
    updateLive();
    const st = S.controller.state;
    for (const name of S.canvas.followed()) S.canvas.record(name, st.tick, st.values.get(name));
    const now = performance.now();
    if (S.tab === 'schedule' && now - scheduleDue > 300) { scheduleDue = now; renderSchedule(); }
    return;
  }
  if (kind === 'transport') { if (S.controller && S.controller.state.tick === 0) S.canvas.clearHistories(); renderBar(); renderReadout(); return; }
  if (kind === 'value') { updateLive(); return; }
  renderTree();
  if (!['layer', 'frame', 'canvas'].includes(S.selected.kind)) renderInspector();
  editorHead();
  if (S.tab === 'schedule') renderSchedule();
}

// ---- the head of the page: the project and the transport -------------------------------------------------------------------

function key(label, onclick, { on = false, title, cls = '' } = {}) {
  return h('button', { class: `${on ? 'on' : ''} ${cls}`.trim(), title, onclick }, label);
}

function renderBar() {
  const p = P(), C = S.controller;
  const title = h('h1', { class: 'st-title', title: 'the project: rename it in the inspector' }, p ? p.meta.title : '—');
  title.addEventListener('click', () => { S.selected = { kind: 'project' }; renderAll(); });
  const projects = h('select', { 'aria-label': 'open a project', onchange: (e) => { if (e.target.value) openProject(e.target.value); } },
    S.projects.map((q) => h('option', { value: q.name, selected: q.name === S.name ? true : undefined }, q.title)));
  $('project').replaceChildren(title,
    h('div', { class: 'keys-tight' }, key('new', newProject, { title: 'a new project, with three example scripts that share a resource' }), key('save', () => saveProject(), { title: 'save the project and its scripts\' settings' }),
      key('arrange', () => setArrange(!S.canvas.arranging), { on: S.canvas?.arranging, title: 'move and resize the frames on the canvas; the world is held still meanwhile' }),
      key('export', exportImage, { title: 'the whole canvas as one PNG, at twice its size' })),
    h('div', { class: 'keys-tight', title: 'day or night, as in Explore' }, ['day', 'night'].map((t) => key(t, () => setTheme(t), { on: document.documentElement.dataset.theme === t }))),
    projects);
  if (!C) { $('transport').replaceChildren(); return; }
  const rates = [0.25, 1, 4, 60, 3600];
  $('transport').replaceChildren(
    h('div', { class: 'keys-tight st-play' },
      key(C.state.playing ? 'hold' : 'play', () => (C.state.playing ? C.hold() : C.play()), { on: C.state.playing, title: 'play or hold the project (space)' }),
      key('step', () => C.step(), { title: 'one tick' }),
      key('reset', () => { C.reset(); for (const l of P().layers) if (l.kind === 'data' && S.records.has(l.id)) S.controller.setValue(l.source, S.records.get(l.id)); renderReadout(); }, { title: 'back to the clock\'s start; the resources to their first values' })),
    h('div', { class: 'keys-tight', title: 'how fast the scene\'s clock runs' }, rates.map((r) => key(r < 1 ? `×${r}` : `×${r}`, () => { C.setRate(r); P().scene.clock.rate = r; }, { on: C.state.rate === r }))));
}

function renderReadout() {
  const C = S.controller;
  if (!C) { $('readout').replaceChildren(); return; }
  const st = C.state;
  $('readout').replaceChildren(
    h('span', { title: 'the scene\'s clock' }, utc(st.time_ms)),
    h('span', { title: 'ticks run' }, `tick ${st.tick}`),
    h('span', { title: 'the last tick\'s time, all its systems' }, `${st.tickMs.toFixed(1)} ms`),
    h('span', { class: st.lag ? 'warn' : '', title: 'ticks dropped because the last had not finished: the kernels set the pace' }, `lag ${st.lag}`));
}

// ---- the project tree ----------------------------------------------------------------------------------------------------

function lamp(s) {
  const status = shown(s);
  return h('i', { class: `lamp ${status}`, title: s?.refused || STATUS[status] || status });
}
function row(label, { selected, onclick, tag, before, after, title } = {}) {
  return h('div', { class: `st-row${selected ? ' chosen' : ''}`, title, onclick }, before || null, h('span', { class: 'st-label' }, label), tag ? h('em', {}, tag) : null, after || null);
}
function choose(kind, id) { S.selected = { kind, id }; if (kind === 'script') S.tab = 'code'; S.canvas?.select?.(kind === 'frame' ? id : null); renderAll(); }

function renderTree() {
  const p = P();
  if (!p) { $('tree').replaceChildren(); return; }
  const sel = S.selected;
  const visibility = (l) => {
    const box = h('input', { type: 'checkbox', checked: l.visible !== false ? true : undefined, 'aria-label': `show ${l.title}`, onclick: (e) => e.stopPropagation() });
    box.addEventListener('change', () => setVisible(l, box.checked));
    return box;
  };
  const dataKeys = h('div', { class: 'st-add' }, h('span', {}, 'add data'),
    h('select', { 'aria-label': 'add a record of the data library', onchange: (e) => { if (e.target.value) addData(e.target.value); e.target.value = ''; } },
      h('option', { value: '' }, 'a record of the library…'), Object.values(DATA).map((d) => h('option', { value: d.id }, d.title))));
  const scriptKeys = h('div', { class: 'st-add' }, h('span', {}, 'new script'),
    h('div', { class: 'keys-tight' }, Object.entries(LANGUAGES).map(([k, name]) => key(name, () => newScript(k), { title: `a new system in ${name}; made while the project plays, it joins the schedule` }))));
  const usedBy = (name) => [...(S.controller?.state.systems.values() || [])].filter((s) => s.describe && [...s.describe.reads, ...s.describe.writes, ...s.describe.previous].includes(name)).length;
  const frameKeys = h('div', { class: 'st-add' }, h('span', {}, 'add frame'),
    h('div', { class: 'keys-tight' }, ['map', 'chart', 'drawing', 'text'].map((k) => key(k, () => addFrame(k), { title: `a ${k} frame on the canvas` }))));
  $('tree').replaceChildren(
    h('section', {}, h('h2', {}, 'Canvas'),
      row(`${p.canvas.width} × ${p.canvas.height}`, { selected: sel.kind === 'canvas', onclick: () => choose('canvas'), tag: 'canvas' }),
      [...p.frames].reverse().map((f) => row(f.title || f.id, { selected: sel.kind === 'frame' && sel.id === f.id, onclick: () => choose('frame', f.id), tag: FRAME[f.kind] })),
      frameKeys),
    h('section', {}, h('h2', {}, 'Layers of the world'),
      [...p.layers].reverse().map((l) => row(l.title, {
        selected: sel.kind === 'layer' && sel.id === l.id, onclick: () => choose('layer', l.id), tag: KIND[l.kind],
        before: visibility(l), after: l.kind === 'script' ? lamp(systemOf(l.script)) : null,
      })),
      dataKeys),
    h('section', {}, h('h2', {}, 'Scripts'),
      p.scripts.map((s) => row(s.id, {
        selected: sel.kind === 'script' && sel.id === s.id, onclick: () => choose('script', s.id), tag: { js: 'js', python: 'py', cpp: 'c++' }[s.language],
        before: lamp(systemOf(s.id)), title: systemOf(s.id)?.describe?.title,
      })),
      scriptKeys),
    h('section', {}, h('h2', {}, 'Resources'),
      p.resources.length ? p.resources.map((r) => row(r.name, { selected: sel.kind === 'resource' && sel.id === r.name, onclick: () => choose('resource', r.name), tag: `${r.type}${r.merge ? `, ${r.merge}` : ''}`, title: `${usedBy(r.name)} systems use it` }))
        : h('p', { class: 'faint' }, 'none yet: a script declares what it shares')),
    h('section', {}, h('h2', {}, 'Captures from Explore'),
      S.captures.length ? S.captures.slice(0, 12).map((c, k) => row(c.title || `capture ${k + 1}`, { selected: sel.kind === 'capture' && sel.id === k, onclick: () => choose('capture', k), tag: (c.at || '').slice(5, 16).replace('T', ' ') }))
        : h('p', { class: 'faint' }, 'none: press capture beside Explore\'s view keys'),
      h('div', { class: 'st-add' }, h('div', { class: 'keys-tight' }, key('look again', async () => { await listCaptures(); renderTree(); })))));
}

function setVisible(l, v) {
  l.visible = v;
  if (l.kind === 'capture') overlay();
  else if (l.kind === 'data' && v && !S.scene.layers.has(l.id)) drawData(l);
  else S.scene.setVisible(l.id, v);
  saveProject(true);
}

// ---- the inspector -------------------------------------------------------------------------------------------------------

const field = (label, value) => h('div', { class: 'st-field' }, h('span', {}, label), value instanceof Node ? value : h('b', {}, String(value ?? '—')));
const names = (list) => (list && list.length ? list.join(', ') : '—');

function summary(v) {
  if (v == null) return '—';
  if (typeof v === 'number') return Number.isInteger(v) ? String(v) : v.toPrecision(5);
  if (typeof v === 'string') return v.length > 60 ? `${v.slice(0, 60)}…` : v;
  if (Array.isArray(v)) return `${v.length} values`;
  if (v.kind && v.title) return `${v.kind}: ${v.title}`;
  if (Array.isArray(v.lon)) return `${v.lon.length} points`;
  return `${Object.keys(v).length} fields`;
}

function renderInspector() {
  const el = $('inspector');
  S.live = {};
  const sel = S.selected, p = P();
  if (!p) { el.replaceChildren(); return; }
  if (sel.kind === 'script' && scriptOf(sel.id)) return fill(el, inspectScript(scriptOf(sel.id)));
  if (sel.kind === 'layer' && layerOf(sel.id)) return fill(el, inspectLayer(layerOf(sel.id)));
  if (sel.kind === 'resource' && resourceOf(sel.id)) return fill(el, inspectResource(resourceOf(sel.id)));
  if (sel.kind === 'capture' && S.captures[sel.id]) return fill(el, inspectCapture(S.captures[sel.id]));
  if (sel.kind === 'frame' && frameOf(sel.id)) return fill(el, inspectFrame(frameOf(sel.id)));
  if (sel.kind === 'canvas') return fill(el, inspectCanvas(p));
  fill(el, inspectProject(p));
}

function inspectProject(p) {
  const title = h('input', { type: 'text', value: p.meta.title, 'aria-label': 'the project\'s title' });
  title.addEventListener('change', () => { p.meta.title = title.value.trim() || p.meta.title; document.title = `${p.meta.title} · Studio`; renderBar(); saveProject(true); });
  const hz = h('input', { type: 'number', min: '1', max: '60', step: '1', value: String(p.scene.clock.tick_hz) });
  hz.addEventListener('change', () => { const v = Math.min(60, Math.max(1, Number(hz.value) || 10)); p.scene.clock.tick_hz = v; saveProject(true); });
  const k = S.kernelState;
  const kern = (name, s) => h('div', { class: 'st-field' }, h('span', {}, name), h('b', {}, s ? `${s.alive ? 'running' : 'not started'}${s.restarted ? `, started again ${s.restarted}×` : ''}${s.toolchain ? `, ${s.toolchain}` : ''}` : '—'));
  const problems = problemsOf(p);
  return [
    h('h2', {}, 'The project'),
    field('title', title),
    field('folder', S.name),
    field('saved', p.meta.saved || 'not yet'),
    h('h3', {}, 'Scene'),
    field('camera', `${p.scene.camera.lat.toFixed(3)}°, ${p.scene.camera.lon.toFixed(3)}°, ${(p.scene.camera.range / 1000).toFixed(0)} km`),
    h('div', { class: 'keys-tight st-keys' },
      key('keep this view', () => { p.scene.camera = S.scene.camera(); saveProject(true); renderInspector(); }, { title: 'the scene\'s camera, kept with the project' }),
      key('back to it', () => S.scene.setCamera(p.scene.camera, 1.2))),
    field('clock starts', utc(p.scene.clock.start_ms)),
    h('div', { class: 'keys-tight st-keys' }, key('start it here', () => { p.scene.clock.start_ms = Math.round(S.controller.state.time_ms); saveProject(true); renderInspector(); }, { title: 'the clock\'s start at its present instant' })),
    field('ticks a second', hz),
    h('h3', {}, 'Kernels'),
    field('JavaScript', 'the page\'s worker'),
    kern('Python', k?.python), kern('C++', k?.cpp),
    S.refusal ? h('p', { class: 'st-note bad' }, S.refusal) : null,
    h('div', { class: 'keys-tight st-keys' }, key('look again', kernelState)),
    problems.length ? h('div', {}, h('h3', {}, 'What is wrong'), problems.map((x) => h('p', { class: 'st-note bad' }, x))) : null,
  ];
}

function inspectScript(script) {
  const s = systemOf(script.id), d = s?.describe;
  const status = s ? (s.refused ? 'refused' : s.status) : 'stopped';
  const seen = shown(s);
  let armed = S.deleteArmed === script.id;
  const del = key(armed ? 'delete? again' : 'delete', () => {
    if (S.deleteArmed === script.id) { S.deleteArmed = null; deleteScript(script.id); return; }
    S.deleteArmed = script.id; renderInspector();
    setTimeout(() => { if (S.deleteArmed === script.id) { S.deleteArmed = null; renderInspector(); } }, 3000);
  }, { cls: armed ? 'danger' : '', title: 'stop it, take it from the schedule and delete its file (press twice)' });
  const C = S.controller;
  const keys = h('div', { class: 'keys-tight st-keys' },
    status === 'ready' ? key('pause', () => C.pause(script.id), { title: 'it stays loaded, its state kept, and is skipped by the schedule' }) : null,
    status === 'paused' ? key('resume', () => C.resume(script.id)) : null,
    ['stopped', 'error', 'refused'].includes(status) ? key('start', () => loadScript(script), { title: 'load it (compile it, for C++) and put it in the schedule' }) : null,
    ['ready', 'paused', 'refused'].includes(status) ? key('stop', () => C.stop(script.id), { title: 'unload it; its state is lost' }) : null,
    key('reload', () => saveScript(script.id), { title: 'save the editor\'s text and load it again, its state kept where save() and load() allow' }),
    del);
  const params = (d?.params || []).map((q) => {
    const v = script.params?.[q.name] ?? q.default;
    const input = q.kind === 'boolean' ? h('input', { type: 'checkbox', checked: v ? true : undefined })
      : h('input', { type: q.kind === 'number' ? 'number' : 'text', value: String(v), min: q.min, max: q.max, step: q.step || 'any' });
    input.addEventListener('change', () => {
      script.params ||= {};
      script.params[q.name] = q.kind === 'boolean' ? input.checked : q.kind === 'number' ? Number(input.value) : input.value;
      saveProject(true);
    });
    return field(q.label || q.name, input);
  });
  const timing = h('b', {}, '—');
  S.live.timing = () => { const x = systemOf(script.id); timing.textContent = x ? `${x.ms.toFixed(1)} ms, ${x.runs} runs${x.compileS ? `; compiled in ${x.compileS} s` : ''}` : '—'; };
  S.live.timing();
  return [
    h('h2', {}, d?.title || script.id),
    h('div', { class: 'st-status' }, lamp(s), h('span', {}, STATUS[seen] || seen), s?.carried && !s.stale ? h('em', {}, 'state kept') : null),
    s?.error ? h('p', { class: 'st-note bad' }, s.error + (s.line ? ` (line ${s.line})` : '')) : null,
    s?.refused ? h('p', { class: 'st-note bad' }, s.refused) : null,
    keys,
    field('file', `${script.file} · ${LANGUAGES[script.language]}`),
    field('phase', d ? (d.phase === 'tick' ? `tick, ${P().scene.clock.tick_hz} a second` : 'update, each frame') : '—'),
    field('reads', names(d?.reads)),
    field('writes', names(d?.writes)),
    field('previous', names(d?.previous)),
    d?.after?.length ? field('after', names(d.after)) : null,
    field('time', timing),
    params.length ? h('h3', {}, 'Parameters') : null,
    ...params,
  ];
}

function inspectLayer(l) {
  const title = h('input', { type: 'text', value: l.title });
  title.addEventListener('change', () => { l.title = title.value.trim() || l.title; renderTree(); saveProject(true); });
  const opacity = h('input', { type: 'range', min: '0', max: '1', step: '0.02', value: String(l.opacity ?? 1) });
  opacity.addEventListener('input', () => { l.opacity = Number(opacity.value); if (l.kind === 'capture') overlay(); else S.scene.setOpacity(l.id, l.opacity); });
  opacity.addEventListener('change', () => saveProject(true));
  const out = [h('h2', {}, l.title), field('title', title), field('kind', KIND[l.kind]), field('opacity', opacity)];
  if (l.kind === 'data') {
    const d = DATA[l.source], r = S.records.get(l.id);
    out.push(field('source', d?.title), field('credit', d?.credit), field('read as', r ? `${r.kind}${r.lat ? `, ${r.lat.length} events` : r.tracks ? `, ${r.tracks.length} tracks` : r.cells ? `, ${r.cells.length} cells` : ''}` : 'reading…'),
      field('resource', h('b', {}, `${l.source}: scripts may read it`)));
    if (r?.kind === 'events') {
      const w = h('input', { type: 'number', min: '0', step: '1', value: String(l.style?.windowDays || 0) });
      w.addEventListener('change', () => { l.style = { ...(l.style || {}), windowDays: Math.max(0, Number(w.value) || 0) }; drawData(l); saveProject(true); });
      out.push(field('days before the clock (0: all)', w));
    }
    let armed = false;
    out.push(h('div', { class: 'keys-tight st-keys' }, key('remove', (e) => {
      if (!armed) { armed = true; e.currentTarget.textContent = 'remove? again'; e.currentTarget.classList.add('danger'); return; }
      S.scene.clear(l.id); P().layers = P().layers.filter((x) => x !== l); P().resources = P().resources.filter((x) => x.layer !== l.id); S.records.delete(l.id);
      S.selected = { kind: 'project' }; saveProject(true); renderAll();
    }, { title: 'take the layer from the project (press twice)' })));
  } else if (l.kind === 'script') {
    out.push(field('drawn by', l.script), h('div', { class: 'keys-tight st-keys' }, key('the script', () => choose('script', l.script))));
  } else if (l.kind === 'capture') {
    const cap = P().captures.find((c) => c.id === l.capture);
    out.push(...captureFields(cap || {}),
      h('p', { class: 'st-note' }, 'Explore\'s plug-ins are not yet drawn again by the Studio: what they drew stands here as the capture\'s image, true only from its view.'),
      h('div', { class: 'keys-tight st-keys' }, key('its view', () => cap && takeView(cap)), key('remove', () => { P().layers = P().layers.filter((x) => x !== l); P().captures = P().captures.filter((c) => c.id !== l.capture); S.selected = { kind: 'project' }; overlay(); saveProject(true); renderAll(); })));
  }
  return out;
}

function captureFields(c) {
  return [
    c.preview ? h('img', { class: 'st-preview', src: c.preview, alt: `the view captured: ${c.title || ''}` }) : null,
    field('place', c.title), field('at', c.time_ms ? utc(c.time_ms) : '—'),
    field('camera', c.camera ? `${c.camera.lat.toFixed(3)}°, ${c.camera.lon.toFixed(3)}°, ${(c.camera.range / 1000).toFixed(1)} km` : '—'),
    field('layers', (c.layers || []).map((x) => x.title || x.id).join(', ') || 'the base alone'),
  ];
}
function number(value, onchange, o = {}) {
  const input = h('input', { type: 'number', step: o.step || '1', min: o.min, max: o.max, value: String(value) });
  input.addEventListener('change', () => onchange(Number(input.value)));
  return input;
}

function inspectCanvas(p) {
  const cv = p.canvas;
  const resize = (w, hgt) => {
    const sx = w / cv.width, sy = hgt / cv.height;
    // The frames keep their places in proportion.
    for (const f of p.frames) { f.x = Math.round(f.x * sx), f.y = Math.round(f.y * sy), f.w = Math.round(f.w * sx), f.h = Math.round(f.h * sy); }
    cv.width = w, cv.height = hgt;
    S.canvas.build(S.world); for (const f of p.frames) S.canvas.redraw(f.id);
    saveProject(true); renderAll();
  };
  const presets = [['16 : 10', 1600, 1000], ['A4 landscape, 150 dpi', 1754, 1240], ['square', 1200, 1200], ['slide 16 : 9', 1920, 1080], ['column, 300 dpi', 1050, 900]];
  return [
    h('h2', {}, 'The canvas'),
    h('p', { class: 'st-note' }, 'The figure the project makes. Its frames lie on it in its pixels; it is exported at any multiple of its size.'),
    field('width', number(cv.width, (v) => resize(Math.max(64, Math.min(20000, v)), cv.height), { min: 64, max: 20000 })),
    field('height', number(cv.height, (v) => resize(cv.width, Math.max(64, Math.min(20000, v))), { min: 64, max: 20000 })),
    h('div', { class: 'keys-tight st-keys' }, presets.map(([name, w, hgt]) => key(name, () => resize(w, hgt), { on: cv.width === w && cv.height === hgt }))),
    field('background', h('div', { class: 'keys-tight' }, [['theme', 'the theme\'s'], ['paper', 'paper'], ['night', 'night']].map(([k, name]) => key(name, () => { cv.background = k; S.canvas.fit(); saveProject(true); renderInspector(); }, { on: (cv.background || 'theme') === k })))),
  ];
}

function inspectFrame(f) {
  const save = () => saveProject(true);
  const moved = () => { S.canvas.build(S.world); S.canvas.redraw(f.id); save(); };
  const title = h('input', { type: 'text', value: f.title || '' });
  title.addEventListener('change', () => { f.title = title.value.trim() || f.title; S.canvas.build(S.world); renderTree(); save(); });
  const k = P().frames.indexOf(f);
  const order = h('div', { class: 'keys-tight st-keys' },
    key('to the front', () => { P().frames.splice(k, 1); P().frames.push(f); moved(); renderAll(); }),
    key('to the back', () => { P().frames.splice(k, 1); P().frames.splice(1, 0, f); moved(); renderAll(); }, { title: 'just above the world' }));
  const out = [
    h('h2', {}, f.title || f.id),
    field('id', h('b', {}, f.id)),
    f.kind !== 'world' ? field('title', title) : null,
    field('kind', FRAME[f.kind]),
    field('x', number(f.x, (v) => { f.x = v; moved(); })), field('y', number(f.y, (v) => { f.y = v; moved(); })),
    field('width', number(f.w, (v) => { f.w = Math.max(20, v); moved(); })), field('height', number(f.h, (v) => { f.h = Math.max(20, v); moved(); })),
    field('ground', h('div', { class: 'keys-tight' }, [['none', 'none'], ['vellum', 'vellum'], ['paper', 'paper']].map(([b, name]) => key(name, () => { f.back = b; S.canvas.fit(); save(); renderInspector(); }, { on: (f.back || 'none') === b })))),
    f.kind !== 'world' ? order : null,
  ];
  if (f.kind === 'world') out.push(h('p', { class: 'st-note' }, 'The 3D world: the layers of the world are drawn in it, and scripts are given its range in space and time.'));
  if (f.kind === 'map') {
    out.push(h('p', { class: 'st-note' }, 'A 2D map following the world: its footprint drawn in rose, the world\'s marks as dots. Places are not named on it; areas in dispute are hatched in no country\'s colour.'),
      field('projection', h('div', { class: 'keys-tight' }, [['equal-earth', 'Equal Earth'], ['equirectangular', 'equirectangular']].map(([pj, name]) => key(name, () => {
        f.map = { ...(f.map || {}), projection: pj };
        const i = P().frames.indexOf(f); P().frames.splice(i, 1); S.canvas.build(S.world); P().frames.splice(i, 0, f); S.canvas.build(S.world);
        save(); renderInspector();
      }, { on: (f.map?.projection || 'equal-earth') === pj })))),
      field('follows', h('div', { class: 'keys-tight' }, [['world', 'the world'], ['none', 'nothing']].map(([w, name]) => key(name, () => { f.map = { ...(f.map || {}), follows: w }; save(); renderInspector(); }, { on: (f.map?.follows || 'world') === w })))));
  }
  if (f.kind === 'chart') {
    const numbers = P().resources.filter((r) => r.type === 'number').map((r) => r.name);
    const pick = h('select', { 'aria-label': 'the resource the chart follows' }, h('option', { value: '' }, 'what a script draws'), numbers.map((n) => h('option', { value: n, selected: f.chart?.resource === n ? true : undefined }, `${n}, tick by tick`)));
    pick.addEventListener('change', () => { f.chart = { ...(f.chart || {}), resource: pick.value || undefined }; S.canvas.redraw(f.id); save(); });
    const by = S.canvas.contentOf(f.id)?.by;
    out.push(field('shows', pick), h('p', { class: 'st-note' }, by && !f.chart?.resource ? `Drawn by ${by}.` : `A script draws into it with chart("${f.id}", …) in any language.`));
  }
  if (f.kind === 'drawing') {
    const by = S.canvas.contentOf(f.id)?.by;
    out.push(h('p', { class: 'st-note' }, `${by ? `Drawn by ${by}. ` : ''}A script draws into it with drawing("${f.id}", items), the items in the frame's own pixels (${f.w} × ${f.h}).`));
  }
  if (f.kind === 'text') {
    const t = f.text || (f.text = { text: '', size: 22, font: 'serif', align: 'left' });
    const area = h('textarea', { class: 'st-textin', rows: '3' });
    area.value = t.text || '';
    area.addEventListener('input', () => { t.text = area.value; S.canvas.fit(); });
    area.addEventListener('change', save);
    out.push(field('text', area), field('size', number(t.size || 22, (v) => { t.size = Math.max(6, v); S.canvas.fit(); save(); })),
      field('type', h('div', { class: 'keys-tight' }, [['serif', 'serif'], ['label', 'Plex'], ['mono', 'mono']].map(([ft, name]) => key(name, () => { t.font = ft; S.canvas.fit(); save(); renderInspector(); }, { on: (t.font || 'serif') === ft })))),
      field('align', h('div', { class: 'keys-tight' }, ['left', 'center', 'right'].map((al) => key(al, () => { t.align = al; S.canvas.fit(); save(); renderInspector(); }, { on: (t.align || 'left') === al })))));
  }
  if (f.kind !== 'world') {
    let armed = false;
    out.push(h('div', { class: 'keys-tight st-keys' }, key('remove', (e) => {
      if (!armed) { armed = true; e.currentTarget.textContent = 'remove? again'; e.currentTarget.classList.add('danger'); return; }
      P().frames = P().frames.filter((x) => x !== f);
      S.dismissed.add(f.id);
      S.canvas.build(S.world);
      S.selected = { kind: 'canvas' };
      save(); renderAll();
    }, { title: 'take the frame from the canvas (press twice); a script drawing into it does not bring it back until it is loaded again' })));
  }
  return out;
}

function inspectCapture(c) {
  return [h('h2', {}, c.title || 'A capture'), ...captureFields(c),
    h('div', { class: 'keys-tight st-keys' }, key('take its view', () => takeView(c), { title: 'the scene\'s camera and clock set to the capture\'s' }), key('lay it as a layer', () => layCapture(c), { title: 'a layer of the project: its view, and its image over the scene' }))];
}

function inspectResource(r) {
  const systems = [...(S.controller?.state.systems.values() || [])].filter((s) => s.describe);
  const by = (k) => systems.filter((s) => (s.describe[k] || []).includes(r.name)).map((s) => s.id);
  const value = h('b', {}, '—');
  S.live.value = () => { value.textContent = summary(S.controller?.state.values.get(r.name)); };
  S.live.value();
  const out = [h('h2', {}, r.name), field('type', r.type), field('merge', r.merge || 'one writer'), field('declared by', r.declaredBy || (r.layer ? `the layer ${r.layer}` : 'the project')),
    field('written by', names(by('writes'))), field('read by', names(by('reads'))), field('read as previous by', names(by('previous'))), field('now', value)];
  if (r.type === 'number') {
    const input = h('input', { type: 'number', step: 'any', value: String(S.controller?.state.values.get(r.name) ?? r.initial ?? 0) });
    input.addEventListener('change', () => { S.controller.setValue(r.name, Number(input.value)); saveProject(true); });
    out.push(field('set it', input), h('p', { class: 'st-note' }, 'A value set here holds until a system writes it.'));
  }
  return out;
}

function updateLive() { for (const f of Object.values(S.live)) f(); }

// ---- the dock: code, console, schedule ----------------------------------------------------------------------------------

function markTab() {
  const t = document.querySelector('#dock-tabs [data-tab="console"]');
  if (t && S.tab !== 'console' && S.lines.at(-1)?.kind === 'error') t.classList.add('alert');
}
function renderDock() {
  const tabs = [['code', 'code'], ['console', 'console'], ['schedule', 'schedule']];
  $('dock-tabs').replaceChildren(...tabs.map(([k, name]) => {
    const b = h('button', { class: S.tab === k ? 'on' : '', onclick: () => { S.tab = k; renderDock(); } }, name);
    b.dataset.tab = k;
    return b;
  }));
  S.editor = null;
  if (S.tab === 'code') renderEditor();
  else if (S.tab === 'console') renderConsole();
  else renderSchedule();
}

function renderEditor() {
  const body = $('dock-body');
  const script = S.selected.kind === 'script' ? scriptOf(S.selected.id) : null;
  if (!script) {
    body.replaceChildren(h('p', { class: 'st-empty' }, 'Choose a script, or make one: in JavaScript it runs in the page\'s worker, in Python in the server\'s kernel, in C++ compiled alone and swapped in. Saved while the project plays (Ctrl S), it is loaded again and joins the next tick.'));
    return;
  }
  const area = h('textarea', { class: 'st-code', spellcheck: 'false', wrap: 'off', 'aria-label': `the code of ${script.file}` });
  area.value = S.buffers.get(script.id) ?? S.code.get(script.file) ?? '';
  const gutter = h('pre', { class: 'st-gutter', 'aria-hidden': 'true' });
  const marker = h('div', { class: 'st-errline', hidden: true });
  const head = h('div', { class: 'st-edhead' });
  const numbers = () => { gutter.textContent = Array.from({ length: area.value.split('\n').length }, (_, i) => i + 1).join('\n') + '\n'; };
  const place = () => {
    gutter.scrollTop = area.scrollTop;
    const s = systemOf(script.id), line = s?.error ? s.line : null;
    if (!line) { marker.hidden = true; return; }
    const lh = parseFloat(getComputedStyle(area).lineHeight) || 18, pad = parseFloat(getComputedStyle(area).paddingTop) || 0;
    marker.hidden = false;
    marker.style.top = `${pad + (line - 1) * lh - area.scrollTop}px`;
    marker.style.height = `${lh}px`;
  };
  area.addEventListener('input', () => { S.buffers.set(script.id, area.value); numbers(); editorHead(); });
  area.addEventListener('scroll', place);
  area.addEventListener('keydown', (e) => {
    if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 's') { e.preventDefault(); saveScript(script.id); return; }
    if (e.key === 'Tab' && !e.shiftKey) {
      e.preventDefault();
      const a = area.selectionStart, b = area.selectionEnd;
      area.setRangeText('  ', a, b, 'end');
      S.buffers.set(script.id, area.value);
      numbers();
      editorHead();
    }
  });
  numbers();
  body.replaceChildren(head, h('div', { class: 'st-editor' }, gutter, h('div', { class: 'st-codewrap' }, marker, area)));
  S.editor = { id: script.id, head, place, area };
  editorHead();
  requestAnimationFrame(place);
}

function editorHead() {
  const E = S.editor;
  if (!E) return;
  const script = scriptOf(E.id), s = systemOf(E.id);
  if (!script) return;
  const dirty = S.buffers.has(E.id) && S.buffers.get(E.id) !== S.code.get(script.file);
  const status = shown(s);
  fill(E.head,
    h('b', {}, script.file), h('em', {}, LANGUAGES[script.language]), dirty ? h('span', { class: 'st-dirty', title: 'not saved' }, 'edited') : null,
    lamp(s), h('span', { class: 'st-state' }, `${STATUS[status] || status}${s?.error ? `: ${s.error.split('\n')[0].slice(0, 140)}${s.line ? ` (line ${s.line})` : ''}` : ''}`),
    h('div', { class: 'keys-tight' },
      key(script.language === 'cpp' ? 'save, compile and run' : 'save and run', () => saveScript(E.id), { title: 'Ctrl S: save it and load it again, its state kept; it joins the next tick' }),
      dirty ? key('revert', () => { S.buffers.delete(E.id); renderEditor(); }) : null));
  E.place();
}

function renderConsole() {
  const t = document.querySelector('#dock-tabs [data-tab="console"]');
  if (t) t.classList.remove('alert');
  const body = $('dock-body');
  const list = h('div', { class: 'st-console' }, S.lines.slice(-300).map((l) => h('div', { class: `st-line ${l.kind}` },
    h('time', {}, new Date(l.at).toTimeString().slice(0, 8)), h('b', {}, l.id), h('span', {}, l.text))));
  body.replaceChildren(h('div', { class: 'st-edhead' }, h('b', {}, 'What the scripts print, and what the controller says'), h('div', { class: 'keys-tight' }, key('clear', () => { S.lines = []; renderConsole(); }))), list);
  list.scrollTop = list.scrollHeight;
}

function renderSchedule() {
  const body = $('dock-body'), C = S.controller;
  if (!C) return body.replaceChildren();
  const pl = C.state.plan;
  const phase = (name, label) => {
    const ids = pl.phases[name] || [];
    return h('section', { class: 'st-phase' }, h('h3', {}, label),
      ids.length ? h('ol', {}, ids.map((id) => {
        const s = systemOf(id), d = s?.describe || {};
        return h('li', {}, lamp(s), h('b', {}, id), h('em', {}, { js: 'js', python: 'py', cpp: 'c++' }[s?.language] || ''),
          h('span', { class: 'flow' }, `${names(d.reads)} → ${names(d.writes)}${d.previous?.length ? `  · previous ${names(d.previous)}` : ''}`),
          h('span', { class: 'ms' }, s && s.runs ? `${s.ms.toFixed(1)} ms` : ''));
      })) : h('p', { class: 'faint' }, 'no system'));
  };
  body.replaceChildren(h('div', { class: 'st-schedule' },
    phase('tick', `Each tick, ${P().scene.clock.tick_hz} a second, in this order`),
    phase('update', 'Each frame'),
    pl.refusals.length ? h('section', { class: 'st-phase' }, h('h3', {}, 'Refused'), pl.refusals.map((r) => h('p', { class: 'st-note bad' }, r.message))) : null,
    h('p', { class: 'st-note' }, 'A writer runs before those that read what it writes; a resource read as previous is the value at the end of the last tick, through which a loop may close. A resource has one writer, unless it declares a merge (sum, min, max, union).')));
}

function renderAll() { renderBar(); renderReadout(); renderTree(); renderInspector(); renderDock(); }

// ---- the page ------------------------------------------------------------------------------------------------------------

function theme(set) {
  const t = set || (remembered('urban-theme', 'night') === 'day' ? 'day' : 'night');
  document.documentElement.dataset.theme = t;
  remember('urban-theme', t);
  return t;
}
// Day or night: the page, the globe's look and every frame drawn again in the theme's colours (shared with Explore).
function setTheme(t) {
  theme(t);
  S.scene.setTheme(t);
  if (P()) { S.canvas.build(S.world); for (const f of P().frames) S.canvas.redraw(f.id); }
  renderBar();
}

async function main() {
  const t = theme();
  // The world's element: the globe and a capture's image over it. It stands in the viewport until a project's canvas takes it.
  const sceneEl = h('div', { id: 'scene', class: 'map' });
  S.world = h('div', { class: 'st-world' }, sceneEl, h('img', { id: 'overlay', class: 'st-overlay', alt: '', hidden: true }));
  $('viewport').append(S.world);
  S.scene = createScene(sceneEl, { theme: t });
  S.canvas = createCanvas({
    host: $('viewport'), project: P, scene: S.scene,
    onSelect: (id) => { if (id && (S.selected.kind !== 'frame' || S.selected.id !== id)) { S.selected = { kind: 'frame', id }; renderTree(); renderInspector(); } },
    onMoved: (f) => { saveProject(true); if (S.selected.kind === 'frame' && S.selected.id === f.id) renderInspector(); },
  });
  S.kernels = createKernels({ project: () => S.name });
  let lastData = 0, lastMaps = 0, lastCharts = 0;
  const frame = (now) => {
    S.scene.earth.frame(now);
    if (S.controller) {
      S.controller.frame(now);
      if (S.controller.state.playing && now - lastData > 250) { lastData = now; for (const l of P().layers) if (timeBound(l)) drawData(l); }
      // The maps follow the world a few times a second, the charts of resources likewise.
      if (now - lastMaps > 300 && P().frames.some((f) => f.kind === 'map')) {
        lastMaps = now;
        const list = [...S.scene.layers].filter(([, L]) => L.visible !== false).map(([id, L]) => ({ id, marks: L.marks || [] }));
        S.canvas.drawMaps(S.scene.rangeOf(S.canvas.scale), { list, version: `${S.controller.state.tick}:${list.map((L) => `${L.id}${L.marks.length}`).join()}` });
      }
      if (now - lastCharts > 250) { lastCharts = now; S.canvas.drawFollowers(); }
    }
    requestAnimationFrame(frame);
  };
  requestAnimationFrame(frame);
  document.addEventListener('keydown', (e) => {
    if (e.target.closest('input, textarea, select') || !S.controller) return;
    if (e.code === 'Space') { e.preventDefault(); S.controller.state.playing ? S.controller.hold() : S.controller.play(); }
  });
  await Promise.all([listProjects(), listCaptures()]);
  renderAll();
  kernelState();
  if (S.refusal) say('studio', `${S.refusal}: the scripts in JavaScript run, those in Python and C++ do not, and nothing is saved`, 'error');
  let name = remembered('studio-project', null);
  if (!S.projects.some((q) => q.name === name)) name = S.projects[0]?.name;
  if (name) await openProject(name);
  else if (!S.refusal) await newProject();
}

// A handle for checking the page from outside (headless Chrome), as Explore's window.workbench.
window.studio = { S, newScript, saveScript, deleteScript, addData, layCapture, takeView, openProject, saveProject, addFrame, setArrange, exportImage };

main().catch((error) => { console.error(error); say('studio', `the Studio could not start: ${error.message}`, 'error'); renderDock(); });
