// Histos Harmonia: the page. A record of historical objects is read at a layer (the established alone, with the attested,
// with the proposed) and seen three ways that share one choice: the loom (time as the warp, strands side by side, the
// dependences as threads), the map (where things were, by a clock) and the cone of the object chosen (what its appearing
// rested on, and what rests on it). The object chosen has a sheet, whose tabs are the activities that plug-ins declare for
// its kind. The desk is where an archive's keeper works: what is said and by whom, the holdings, the sources, the log.

import { svgLoom, svgCone, glyphOf, yearText, ROLES, ROLE_ORDER, esc } from '/visualization/js/history/index.js';
import { createMap } from '/visualization/js/map/index.js';
import { desk } from './desk.js';

const $ = (id) => document.getElementById(id);
// A node's children replaced, leaving out what is not there: the DOM would write a missing child as the word "null".
const plainReplace = Element.prototype.replaceChildren;
Element.prototype.replaceChildren = function replaceChildren(...nodes) { return plainReplace.apply(this, nodes.filter((n) => n !== null && n !== undefined && n !== false)); };
export const el = (tag, attrs = {}, ...children) => {
  const n = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (v === null || v === undefined || v === false) continue;
    if (k === 'class') n.className = v;
    else if (k === 'html') n.innerHTML = v;
    else if (k.startsWith('on')) n.addEventListener(k.slice(2), v);
    else n.setAttribute(k, v === true ? '' : v);
  }
  for (const c of children.flat()) if (c !== null && c !== undefined && c !== false) n.append(c);
  return n;
};
export async function api(path, options = {}) {
  const r = await fetch(path, options);
  const type = r.headers.get('Content-Type') || '';
  const body = type.includes('json') ? await r.json() : await r.text();
  if (!r.ok) throw new Error(body?.message || `${r.status} ${r.statusText}`);
  return body;
}

const S = {
  records: [], record: null, data: null, by: new Map(), layer: 'established', view: 'loom', strands: 'discipline', selected: null, detail: null,
  year: null, from: 0, to: 1, plugins: [], modules: new Map(), activity: 'inquire', kinds: new Set(), find: '', writable: false, tour: null, stop: 0, map: null,
  countries: undefined, playing: null, placeShown: null,
};
const ORDER = { established: 0, attested: 1, proposed: 2 };
export const inLayer = (standing) => standing !== 'disputed' && ORDER[standing] <= ORDER[S.layer];
const isA = (o, kind) => o.kind === kind || (o.above || []).includes(kind);
const visible = () => S.data.objects.filter((o) => inLayer(o.standing));
const kindName = (o) => S.data.kinds.find((k) => k.id === o.kind)?.name || o.kind;
const approx = (o) => Boolean(o.when?.approximate);
// When an object was, as it is read: a year, or a span from one year to another.
export const whenText = (o) => (!o.when ? 'no date in the record' : o.when.text.includes('/') ? `${yearText(o.when.from, approx(o))} to ${yearText(o.when.to)}` : yearText(o.when.from, approx(o)));
export const state = S;

// ---- reading -------------------------------------------------------------------------------------------------------------------

async function boot() {
  const q = new URLSearchParams(location.search);
  // A link may say how the page opens: by night or by day, with an activity chosen, with every answer shown.
  if (q.get('theme') ? q.get('theme') === 'night' : localStorage.getItem('histos.night') === '1') document.documentElement.classList.add('night');
  if (q.get('activity')) S.activity = q.get('activity');
  if (q.get('answers')) sessionStorage.setItem('histos.open', q.get('answers') === 'shown' ? '1' : '0');
  const first = await api('/api/records');
  S.records = first.records;
  S.writable = first.writable;
  document.body.classList.toggle('writable', S.writable);
  if (!first.engine.built) return setup();
  S.plugins = (await api('/api/plugins')).plugins;
  $('record').replaceChildren(...S.records.map((r) => el('option', { value: r.id }, r.title)));
  S.layer = ORDER[q.get('layer')] !== undefined ? q.get('layer') : 'established';
  S.view = ['loom', 'map', 'cone', 'desk'].includes(q.get('view')) ? q.get('view') : 'loom';
  wire();
  await openRecord(q.get('record') && S.records.some((r) => r.id === q.get('record')) ? q.get('record') : S.records[0]?.id, q.get('object'));
  if (q.get('exhibit')) { document.body.classList.add('exhibit'); startTour(q.get('exhibit')); }
}

async function setup() {
  const box = $('setup');
  box.hidden = false;
  const view = await api('/setup');
  box.replaceChildren(el('b', {}, 'The engine is not built yet. '), el('span', {}, view.about || ''), el('div', { class: 'row' },
    el('button', { class: 'key', type: 'button', onclick: async (e) => {
      e.target.disabled = true;
      e.target.textContent = 'Building… a few minutes';
      try { await api('/setup/run', { method: 'POST' }); } catch (error) { e.target.textContent = error.message; return; }
      const wait = setInterval(async () => { if ((await api('/api/records')).engine.built) { clearInterval(wait); location.reload(); } }, 4000);
    } }, 'Build it')));
}

async function openRecord(id, object = null) {
  if (!id) return;
  S.record = id;
  $('record').value = id;
  S.data = await api(`/api/record?record=${encodeURIComponent(id)}`);
  S.by = new Map(S.data.objects.map((o) => [o.id, o]));
  const years = S.data.objects.map((o) => o.appears).filter((y) => typeof y === 'number');
  S.from = Math.floor((Math.min(...years) - 20) / 50) * 50;
  S.to = Math.ceil((Math.max(...years) + 10) / 50) * 50;
  if (S.year === null || S.year < S.from || S.year > S.to) S.year = S.to;
  Object.assign($('year'), { min: S.from, max: S.to, step: 1, value: S.year });
  S.selected = object && S.by.has(object) ? object : null;
  S.detail = null;
  S.kinds.clear();
  refresh();
  if (S.selected) await select(S.selected, false);
}

async function reload() {
  const keep = S.selected;
  await openRecord(S.record, keep);
}

export async function select(id, show = true) {
  if (!S.by.has(id)) return;
  S.selected = id;
  S.detail = await api(`/api/object?record=${encodeURIComponent(S.record)}&id=${encodeURIComponent(id)}&layer=${S.layer}`);
  if (show && S.view === 'desk') S.view = 'loom';
  history.replaceState(null, '', `?record=${S.record}&object=${id}&view=${S.view}&layer=${S.layer}${document.body.classList.contains('exhibit') ? `&exhibit=${S.tour?.id || ''}` : ''}`);
  refresh();
}

// ---- the whole page, drawn again ------------------------------------------------------------------------------------------------

function refresh() {
  const c = S.data.statements;
  const upTo = { established: c.established, attested: c.established + c.attested, proposed: c.established + c.attested + c.proposed };
  for (const b of $('layers').querySelectorAll('[data-layer]')) {
    b.classList.toggle('on', b.dataset.layer === S.layer);
    b.querySelector('b').textContent = upTo[b.dataset.layer];
  }
  for (const b of $('views').querySelectorAll('[data-view]')) b.classList.toggle('on', b.dataset.view === S.view);
  const notice = $('notice');
  notice.hidden = upTo[S.layer] > 0 && !S.data.faults.length;
  if (S.data.faults.length) notice.replaceChildren(`This record has ${S.data.faults.length} fault${S.data.faults.length > 1 ? 's' : ''}; the rows that stand are shown. The desk lists them.`);
  else if (!notice.hidden) {
    const wider = S.layer === 'established' && c.attested ? 'attested' : 'proposed';
    notice.replaceChildren(el('span', {}, S.layer === 'established'
      ? `Nothing in this record is established yet: ${c.attested} statements are attested and ${c.proposed} are a language model's proposals, not yet confirmed against a source.`
      : 'Nothing in this record stands at this layer.'),
      el('button', { class: 'key', type: 'button', onclick: () => setLayer(wider) }, `Read with the ${wider}`));
  }
  finder();
  stage();
  sheet();
  clock();
}

async function setLayer(layer) {
  S.layer = layer;
  if (S.selected) return select(S.selected, false);
  refresh();
}

function finder() {
  const all = visible();
  const tops = [...new Set(all.map((o) => (o.above?.length > 1 ? o.above[o.above.length - 2] : o.kind)))].sort();
  $('kinds').replaceChildren(...tops.map((k) => el('button', { class: `key${S.kinds.has(k) ? ' on' : ''}`, type: 'button', onclick: () => { S.kinds.has(k) ? S.kinds.delete(k) : S.kinds.add(k); finder(); } },
    `${glyphOf({ kind: k })} ${S.data.kinds.find((x) => x.id === k)?.name || k}`)));
  const want = S.find.trim().toLowerCase();
  const shown = all.filter((o) => (!S.kinds.size || [...S.kinds].some((k) => isA(o, k))) &&
    (!want || o.name.toLowerCase().includes(want) || Object.values(o.names || {}).some((n) => n.toLowerCase().includes(want))))
    .sort((a, b) => (a.appears ?? 1e9) - (b.appears ?? 1e9) || a.name.localeCompare(b.name));
  $('list').replaceChildren(...(shown.length ? shown.map((o) => el('li', { class: `s-${o.standing}${o.id === S.selected ? ' on' : ''}`, onclick: () => select(o.id) },
    el('span', { class: 'g' }, glyphOf(o)), el('span', { class: 'n' }, o.name), el('span', { class: 'y' }, yearText(o.appears)))) : [el('li', { class: 'none' }, 'Nothing at this layer.')]));
}

// ---- the stage: loom, map, cone, desk ------------------------------------------------------------------------------------------------

const CONDITIONS = ['Technology', 'Practice', 'Artifact', 'Institution', 'Polity'];
const STRANDS = {
  discipline: { name: 'discipline', of: (o) => (CONDITIONS.some((k) => isA(o, k)) ? 'conditions' : o.discipline || 'other'), order: ['music', 'philosophy', 'mathematics', 'conditions', 'other'],
    names: { conditions: 'What things were made with and done', other: 'Of no one discipline' } },
  tradition: { name: 'tradition', of: (o) => o.tradition || 'shared', order: ['greek', 'latin', 'arabic', 'indian', 'chinese', 'european', 'shared'], names: { shared: 'Of no one tradition' } },
  kind: { name: 'kind', of: (o) => (o.above?.length > 1 ? o.above[o.above.length - 2] : o.kind), order: [], names: {} },
};
const title = (s) => s.charAt(0).toUpperCase() + s.slice(1);

function threads() {
  const out = [];
  for (const e of S.data.emergences)
    for (const p of e.premises)
      if (inLayer(p.standing) && inLayer(S.by.get(p.what).standing) && inLayer(S.by.get(e.product).standing)) out.push({ from: p.what, to: e.product, role: p.role, standing: p.standing });
  return out;
}

function coneIds() {
  if (!S.detail) return null;
  return [S.selected, ...S.detail.past.map((e) => e.object), ...S.detail.future.map((e) => e.object)];
}

function legend() {
  return el('div', { class: 'legend' },
    ...ROLE_ORDER.map((r) => el('span', {}, el('i', { class: 'role', style: `border-color:${ROLES[r].hue}` }), ROLES[r].short)),
    el('span', {}, el('i', {}), 'established'), el('span', {}, el('i', { class: 'attested' }), 'attested'), el('span', {}, el('i', { class: 'proposed' }), 'proposed'));
}

function stage() {
  const bar = $('stage-bar'), body = $('stage');
  body.className = `stage-body ${S.view}`;
  $('clock').hidden = S.view === 'desk' || S.view === 'cone';
  if (S.view !== 'map') { S.map = null; S.placeShown = null; }
  if (S.view !== 'loom') S.shownOn = null;
  if (S.view === 'loom') {
    const strand = STRANDS[S.strands];
    const objects = visible().filter((o) => !isA(o, 'Place')).map((o) => ({ id: o.id, name: o.name, kind: o.kind, above: o.above, year: o.appears, lane: strand.of(o), standing: o.standing, approximate: approx(o) }));
    const present = [...new Set(objects.map((o) => o.lane))];
    const lanes = [...strand.order.filter((l) => present.includes(l)), ...present.filter((l) => !strand.order.includes(l)).sort()].map((id) => ({ id, name: strand.names[id] || S.data.kinds.find((k) => k.id === id)?.name && title(S.data.kinds.find((k) => k.id === id).name) || title(id) }));
    bar.replaceChildren(el('span', { class: 'caption' }, 'Strands by'), ...Object.keys(STRANDS).map((k) => el('button', { class: `key${S.strands === k ? ' on' : ''}`, type: 'button', onclick: () => { S.strands = k; stage(); } }, STRANDS[k].name)),
      tourPick(), legend());
    body.innerHTML = svgLoom({ from: S.from, to: S.to, lanes, objects, threads: threads() }, { width: Math.max(body.clientWidth - 24, 1300), selected: S.selected, lit: coneIds(), now: S.year, evenness: 0.62 });
    if (S.selected !== S.shownOn) body.querySelector('.h-mark.selected')?.scrollIntoView({ block: 'center', inline: 'center' });
    S.shownOn = S.selected;
  } else if (S.view === 'cone') {
    if (!S.selected || !S.detail) {
      bar.replaceChildren(el('span', { class: 'say' }, 'Choose an object, on the loom, on the map or in the list, to see what its appearing rested on.'));
      body.replaceChildren();
      return;
    }
    const entry = (e) => { const o = S.by.get(e.object); return { ...o, kindName: kindName(o), year: o.appears, approximate: approx(o), steps: e.steps, role: e.role, standing: e.steps === 1 ? e.standing : o.standing, inherited: e.inherited }; };
    const center = S.by.get(S.selected);
    const names = Object.fromEntries(S.data.objects.map((o) => [o.id, o.name]));
    bar.replaceChildren(el('span', { class: 'say' }, `What the appearing of ${center.name} rested on, to the left, and what rests on it, to the right.`), legend());
    body.innerHTML = svgCone({ center: { ...center, kindName: kindName(center), year: center.appears, approximate: approx(center) }, past: S.detail.past.map(entry), future: S.detail.future.map(entry),
      links: S.detail.links, lineage: S.detail.lineage.map((a) => ({ id: a.object, name: names[a.object] })) }, { names });
    body.querySelector('.h-slip.center')?.scrollIntoView({ block: 'center', inline: 'center' });
  } else if (S.view === 'map') {
    drawMap(bar, body);
  } else {
    desk({ S, bar, body, api, el, select, reload, inLayer });
  }
}

// The places of the record on the sphere's map, by the clock: an object is drawn where it was, once it has appeared.
async function drawMap(bar, body) {
  if (S.countries === undefined) {
    try { S.countries = await api('/data/natural-earth-50m/ne_50m_admin_0_countries.geojson'); } catch { S.countries = null; }
    if (typeof S.countries === 'string') S.countries = null;
  }
  if (S.view !== 'map') return;
  bar.replaceChildren(el('span', { class: 'say' }, `Where things were by ${yearText(S.year)}. Present coasts are drawn, with no names and no borders of the time; an object stands where the record places it.`), tourPick(), legend());
  if (!S.map) {
    body.replaceChildren(el('div', { html: '<svg id="map" xmlns="http://www.w3.org/2000/svg"></svg>', style: 'width:100%;height:100%' }));
    S.map = createMap(body.querySelector('#map'), { countries: S.countries, nameOf: () => '', onSettle: () => overlay(body) });
  }
  overlay(body);
}

function overlay(body) {
  const M = S.map;
  if (!M) return;
  const px = M.px(), NS = 'http://www.w3.org/2000/svg';
  const make = (tag, attrs, text) => { const n = document.createElementNS(NS, tag); for (const [k, v] of Object.entries(attrs)) n.setAttribute(k, v); if (text) n.textContent = text; return n; };
  const lit = new Set(coneIds() || []);
  const here = new Map();  // place id → the objects there by the clock
  for (const o of visible()) {
    if (isA(o, 'Place') || typeof o.appears !== 'number' || o.appears > S.year) continue;
    for (const p of o.places || []) {
      if (!here.has(p.place)) here.set(p.place, { ...p, objects: [] });
      here.get(p.place).objects.push(o);
    }
  }
  const placeOf = (id) => (S.by.get(id)?.places || [])[0];
  const marks = [];
  if (S.detail) {
    for (const l of S.detail.links) {
      if (!lit.has(l.what) || !lit.has(l.product)) continue;
      const a = placeOf(l.what), b = placeOf(l.product);
      if (!a || !b || a.place === b.place || (S.by.get(l.product).appears ?? 1e9) > S.year) continue;
      marks.push(make('path', { class: `arc s-${l.standing}`, d: M.planar.greatCircle({ latitude: a.lat, longitude: a.lon }, { latitude: b.lat, longitude: b.lon }), 'stroke-width': 1.6 * px, style: `stroke:${ROLES[l.role]?.hue || ''}` }));
    }
  }
  for (const [id, p] of here) {
    const [x, y] = M.planar.project(p.lon, p.lat);
    const on = p.objects.some((o) => lit.has(o.id));
    const spot = make('circle', { class: `spot${on ? ' lit' : ''}`, cx: x, cy: y, r: (4 + Math.sqrt(p.objects.length) * 2.2) * px, 'stroke-width': px, 'data-place': id });
    spot.addEventListener('click', (e) => { e.stopPropagation(); S.placeShown = id; overlay(body); });
    marks.push(spot);
    // Names are written for what is lit, for places of several objects, and for all once the map is drawn near.
    if (on || p.objects.length > 2 || M.view.k >= 2.4) marks.push(make('text', { class: 'spot-name', x: x + 9 * px, y: y + 4 * px, 'font-size': 12 * px, 'stroke-width': 3 * px }, `${S.by.get(id)?.name || id}${p.objects.length > 1 ? ` · ${p.objects.length}` : ''}`));
  }
  M.over.replaceChildren(...marks);
  body.querySelector('.map-list')?.remove();
  const shown = S.placeShown && here.get(S.placeShown);
  if (shown) body.append(el('div', { class: 'map-list' }, el('h3', {}, S.by.get(S.placeShown).name), ...shown.objects.sort((a, b) => a.appears - b.appears).map((o) => el('div', { onclick: () => select(o.id) }, `${glyphOf(o)} ${o.name}, ${yearText(o.appears)}`))));
}

// ---- the clock, and the questions of a year --------------------------------------------------------------------------------------

let asking = null;
function clock() {
  $('year').value = S.year;
  $('year-text').textContent = yearText(S.year);
  clearTimeout(asking);
  asking = setTimeout(async () => {
    const y = Math.floor(S.year), date = `${y < 0 ? '-' : ''}${String(Math.abs(y)).padStart(4, '0')}`;
    let f;
    try { f = await api(`/api/frontier?record=${encodeURIComponent(S.record)}&date=${date}&layer=${S.layer}`); } catch { return; }
    const chips = f.problems.sort((a, b) => Number(b.open) - Number(a.open)).map((q) => el('span', { class: `q${q.open ? ' open' : ''}`, title: q.open ? 'Posed, and not yet answered' : 'Answered by then', onclick: () => select(q.problem) },
      `? ${S.by.get(q.problem).name}`, el('small', {}, q.open ? (q.later.length ? `open · ${q.later.length} answer${q.later.length > 1 ? 's' : ''} to come` : 'open') : `${q.answered.length} answer${q.answered.length > 1 ? 's' : ''}`)));
    $('questions').replaceChildren(el('span', { class: 'caption' }, `The questions of ${yearText(S.year)}`), ...(chips.length ? chips : [el('span', { class: 'say' }, 'The record names none posed by then, at this layer.')]));
  }, 160);
}

// ---- the sheet of the object chosen: its activities ---------------------------------------------------------------------------------

function activitiesFor(o) {
  const out = [];
  for (const p of S.plugins)
    for (const a of p.activities || [])
      if ((a.kinds || ['Object']).some((k) => isA(o, k)) && (!a.needs || o[a.needs] !== undefined) && (!a.desk || S.writable)) out.push({ ...a, plugin: p });
  return out;
}

async function moduleOf(plugin) {
  if (!S.modules.has(plugin.id)) S.modules.set(plugin.id, await import(`/plugins/${plugin.id}/${plugin.module}`));
  return S.modules.get(plugin.id);
}

// A program written for an activity on an object: run apart from the page, in a worker, and shown in a frame that can do nothing.
function runProgram(source, input) {
  return new Promise((resolve) => {
    const code = `self.onmessage = async (e) => { try { const f = new Function('ctx', ${JSON.stringify(source)}); postMessage({ ok: true, out: await f(e.data) }); } catch (error) { postMessage({ ok: false, error: String(error) }); } };`;
    const worker = new Worker(URL.createObjectURL(new Blob([code], { type: 'text/javascript' })));
    const stop = setTimeout(() => { worker.terminate(); resolve({ ok: false, error: 'the program did not answer within five seconds' }); }, 5000);
    worker.onmessage = (e) => { clearTimeout(stop); worker.terminate(); resolve(e.data); };
    worker.postMessage(input);
  });
}

async function authored(o, activity) {
  const got = await api(`/api/content?record=${encodeURIComponent(S.record)}&object=${encodeURIComponent(o.id)}&activity=${activity.id}`);
  const doc = got.content;
  if (!doc) return { doc: null, node: null };
  const box = el('div', { class: 'authored' });
  if (got.inherited) box.append(el('div', { class: 'from' }, `Written for ${S.by.get(got.inherited)?.name || got.inherited}, which this stands under.`));
  if (doc.kind === 'document') {
    for (const block of String(doc.text || '').split(/\n\s*\n/)) {
      const m = block.match(/^\?\s*(.+?)\s*::\s*([\s\S]+)$/);
      if (m) {
        const answer = el('div', { class: 'reveal', hidden: true }, m[2]);
        box.append(el('div', { class: 'ask' }, el('div', { class: 'qtext' }, m[1]), el('button', { class: 'key quiet', type: 'button', onclick: (e) => { answer.hidden = false; e.target.remove(); } }, 'Think first, then show'), answer));
      } else if (block.trim()) box.append(el('p', {}, block.trim()));
    }
  } else {
    const result = await runProgram(doc.source || '', { object: o, detail: S.detail, layer: S.layer, record: { id: S.data.id, title: S.data.title } });
    if (!result.ok) box.append(el('p', { class: 'msg' }, `The program written here failed: ${result.error}`));
    else for (const pane of result.out?.panes || []) box.append(el('h2', {}, pane.title || ''), el('iframe', { sandbox: '', srcdoc: `<body style="font:15px Georgia,serif;margin:10px;color:#3b2c3f">${pane.html || ''}</body>` }));
  }
  return { doc, node: box };
}

function writer(o, activity, doc) {
  if (!S.writable) return null;
  const kind = el('select', {}, el('option', { value: 'document' }, 'a document'), el('option', { value: 'program' }, 'a program'));
  kind.value = doc?.kind || 'document';
  const text = el('textarea', { spellcheck: 'false' });
  text.value = doc ? (doc.kind === 'program' ? doc.source : doc.text) || '' : '';
  const msg = el('div', { class: 'msg' });
  return el('details', { class: 'write' }, el('summary', {}, `Write for “${activity.name}” on this object`),
    el('p', { class: 'silent' }, 'A document is paragraphs; a paragraph written “? a question :: its answer” is asked before it is shown. A program is the body of a function of ctx (the object, its inquiry, its cones) that returns { panes: [{ title, html }] }; it runs apart from the page. What is written here also serves the objects that stand under this one, until they have their own.'),
    el('div', { class: 'row' }, el('label', {}, 'As', kind)), text,
    el('div', { class: 'row' }, el('button', { class: 'key', type: 'button', onclick: async () => {
      const body = kind.value === 'program' ? { kind: 'program', source: text.value } : { kind: 'document', text: text.value };
      try {
        await api(`/api/content?record=${encodeURIComponent(S.record)}&object=${encodeURIComponent(o.id)}&activity=${activity.id}`, { method: 'PUT', body: JSON.stringify(body) });
        sheet();
      } catch (error) { msg.textContent = error.message; }
    } }, 'Keep'), msg));
}

let drawing = 0;
async function sheet() {
  const host = $('sheet');
  const o = S.selected && S.by.get(S.selected);
  if (!o || !S.detail) {
    host.replaceChildren(el('h1', {}, S.data.title), el('p', {}, S.data.about), el('p', { class: 'empty' }, 'Choose an object to inquire into it.'));
    return;
  }
  const mine = ++drawing;
  const acts = activitiesFor(o);
  const activity = acts.find((a) => a.id === S.activity) || acts[0];
  const names = Object.entries(o.names || {}).map(([, n]) => n).join(' · ');
  const pane = el('div', { class: 'pane' }, el('p', { class: 'silent' }, '…'));
  host.replaceChildren(
    el('h1', {}, `${glyphOf(o)} ${o.name}`), names ? el('div', { class: 'own' }, names) : null,
    el('div', { class: 'meta' }, el('span', {}, kindName(o)), el('span', {}, whenText(o)),
      ...(o.places || []).filter((p) => p.place !== o.id).map((p) => el('span', {}, `at ${S.by.get(p.place)?.name || p.place}`)), el('span', { class: `chip ${o.standing}`, title: 'How the record\'s word on this object stands' }, o.standing)),
    el('div', { class: 'tabs' }, ...acts.map((a) => el('button', { class: `key${a.id === activity.id ? ' on' : ''}`, type: 'button', title: a.about || '', onclick: () => { S.activity = a.id; sheet(); } }, a.name))),
    pane);
  try {
    const [mod, written] = await Promise.all([moduleOf(activity.plugin), authored(o, activity)]);
    if (mine !== drawing) return;
    const ctx = { S, object: o, detail: S.detail, record: S.data, by: S.by, layer: S.layer, api, el, select, reload, inLayer, kindName, whenText, glyphOf, yearText, esc, ROLES, writable: S.writable, activity };
    const made = await mod.render[activity.id](ctx);
    if (mine !== drawing) return;
    pane.replaceChildren(...[written.node, made, writer(o, activity, written.node && !written.inherited ? written.doc : null)].filter(Boolean));
  } catch (error) {
    pane.replaceChildren(el('p', { class: 'msg' }, `This activity could not be shown: ${error.message}`));
  }
}

// ---- a path walked: an exhibit's tour ------------------------------------------------------------------------------------------------

const tours = () => S.plugins.flatMap((p) => (p.tours || []).filter((t) => t.record === S.record));
function tourPick() {
  const all = tours();
  if (!all.length) return null;
  const pick = el('select', { 'aria-label': 'A path through the record', onchange: (e) => (e.target.value ? startTour(e.target.value) : endTour()) }, el('option', { value: '' }, 'Walk a path…'), ...all.map((t) => el('option', { value: t.id }, t.name)));
  pick.value = S.tour?.id || '';
  return pick;
}
function startTour(id) {
  S.tour = tours().find((t) => t.id === id) || tours()[0] || null;
  S.stop = 0;
  if (S.tour) { if (S.layer !== 'proposed' && S.tour.layer === 'proposed') S.layer = 'proposed'; showStop(); }
}
function endTour() { S.tour = null; $('tour').hidden = true; refresh(); }
async function showStop() {
  const t = S.tour, stop = t.stops[S.stop];
  $('tour').hidden = false;
  $('tour').replaceChildren(
    el('button', { class: 'key', type: 'button', disabled: S.stop === 0, onclick: () => { S.stop--; showStop(); } }, '←'),
    el('div', { class: 'say' }, stop.say), el('span', { class: 'at' }, `${S.stop + 1} of ${t.stops.length} · ${t.name}`),
    el('button', { class: 'key', type: 'button', disabled: S.stop === t.stops.length - 1, onclick: () => { S.stop++; showStop(); } }, '→'),
    el('button', { class: 'key quiet', type: 'button', onclick: endTour }, 'End'));
  const o = S.by.get(stop.object);
  if (o && typeof o.appears === 'number') S.year = Math.min(S.to, Math.ceil(o.appears) + 1);
  if (stop.view) S.view = stop.view;
  if (stop.activity) S.activity = stop.activity;
  await select(stop.object, false);
}

// ---- the keys -------------------------------------------------------------------------------------------------------------------

function wire() {
  $('record').addEventListener('change', (e) => { S.year = null; openRecord(e.target.value); });
  $('views').addEventListener('click', (e) => { const b = e.target.closest('[data-view]'); if (b) { S.view = b.dataset.view; refresh(); } });
  $('layers').addEventListener('click', (e) => { const b = e.target.closest('[data-layer]'); if (b) setLayer(b.dataset.layer); });
  $('find').addEventListener('input', (e) => { S.find = e.target.value; finder(); });
  $('stage').addEventListener('click', (e) => { const g = e.target.closest('[data-object]'); if (g) select(g.dataset.object); });
  $('year').addEventListener('input', (e) => { S.year = Number(e.target.value); if (S.view === 'map') overlay($('stage')); else if (S.view === 'loom') stage(); clock(); });
  $('play').addEventListener('click', (e) => {
    if (S.playing) { clearInterval(S.playing); S.playing = null; e.target.textContent = '▷'; return; }
    if (S.year >= S.to) S.year = S.from;
    e.target.textContent = '❚❚';
    S.playing = setInterval(() => {
      S.year = Math.min(S.to, S.year + Math.max(1, Math.round((S.to - S.from) / 400)));
      $('year').value = S.year;
      $('year').dispatchEvent(new Event('input'));
      if (S.year >= S.to) $('play').click();
    }, 60);
  });
  $('night').addEventListener('click', () => { const on = document.documentElement.classList.toggle('night'); localStorage.setItem('histos.night', on ? '1' : '0'); });
  window.addEventListener('resize', () => { if (S.view === 'loom') stage(); });
}

boot().catch((error) => { document.body.append(el('div', { class: 'setup' }, `The page could not start: ${error.message}`)); });
