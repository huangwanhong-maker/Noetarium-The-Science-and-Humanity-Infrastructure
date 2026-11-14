// Histos Harmonia: the page. The record is drawn edge to edge, as a map (where things were, by a clock), a loom (time as
// the warp, strands side by side, dependences as threads) or the cone of one object. An object chosen opens where it
// stands: its relations are drawn from it, a card says what it is, and a ring of actions invites what can be done with
// it. An action enters a room, a page of its own for one object and one activity, which a plug-in may design. The thread
// of objects followed is kept, so that one can walk the web and come back. The desk is where an archive's keeper works.

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
  records: [], record: null, data: null, by: new Map(), layer: 'established', view: 'map', strands: 'discipline', selected: null, detail: null,
  year: null, from: 0, to: 1, plugins: [], modules: new Map(), kinds: new Set(), find: '', writable: false, tour: null, stop: 0, map: null,
  countries: undefined, playing: null, trail: [], room: null, fanAt: null, flown: null, shownOn: null, where: new Map(),
};
const ORDER = { established: 0, attested: 1, proposed: 2 };
export const inLayer = (standing) => standing !== 'disputed' && ORDER[standing] <= ORDER[S.layer];
const isA = (o, kind) => o.kind === kind || (o.above || []).includes(kind);
const visible = () => S.data.objects.filter((o) => inLayer(o.standing));
const kindName = (o) => S.data.kinds.find((k) => k.id === o.kind)?.name || o.kind;
const approx = (o) => Boolean(o.when?.approximate);
// When an object was, as it is read: a year, or a span from one year to another.
export const whenText = (o) => (!o.when ? 'no date in the record' : o.when.text.includes('/') ? `${yearText(o.when.from, approx(o))} to ${yearText(o.when.to)}` : yearText(o.when.from, approx(o)));
const placesOf = (id) => S.where.get(id) || [];
const placeNames = (o) => placesOf(o.id).filter((p) => p.place !== o.id).map((p) => `${S.by.get(p.place)?.name || p.place}${p.through ? ` (by ${S.by.get(p.through)?.name || p.through})` : ''}`);
export const state = S;

// ---- reading -------------------------------------------------------------------------------------------------------------------

async function boot() {
  const q = new URLSearchParams(location.search);
  // A link may say how the page opens: by night or by day, at a layer, on an object, in a room.
  if (q.get('theme') ? q.get('theme') === 'night' : localStorage.getItem('histos.night') === '1') document.documentElement.classList.add('night');
  if (q.get('answers')) sessionStorage.setItem('histos.open', q.get('answers') === 'shown' ? '1' : '0');
  const first = await api('/api/records');
  S.records = first.records;
  S.writable = first.writable;
  document.body.classList.toggle('writable', S.writable);
  if (!first.engine.built) return setup();
  S.plugins = (await api('/api/plugins')).plugins;
  $('record').replaceChildren(...S.records.map((r) => el('option', { value: r.id }, r.title)));
  $('record').hidden = S.records.length < 2;
  S.layer = ORDER[q.get('layer')] !== undefined ? q.get('layer') : 'established';
  S.view = ['loom', 'map', 'cone', 'desk'].includes(q.get('view')) ? q.get('view') : 'map';
  wire();
  await openRecord(q.get('record') && S.records.some((r) => r.id === q.get('record')) ? q.get('record') : S.records[0]?.id, q.get('object'));
  if (q.get('exhibit')) { document.body.classList.add('exhibit'); startTour(q.get('exhibit')); }
  else if (q.get('room') && S.selected) openRoom(q.get('room'), false);
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
  placeAll();
  const years = S.data.objects.map((o) => o.appears).filter((y) => typeof y === 'number');
  S.from = Math.floor((Math.min(...years) - 20) / 50) * 50;
  S.to = Math.ceil((Math.max(...years) + 10) / 50) * 50;
  if (S.year === null || S.year < S.from || S.year > S.to) S.year = S.to;
  Object.assign($('year'), { min: S.from, max: S.to, step: 1, value: S.year });
  S.selected = object && S.by.has(object) ? object : null;
  S.detail = null;
  S.kinds.clear();
  if (S.selected) await select(S.selected, { quiet: true });
  refresh();
}

// Where each object stands on the map. An object the record places stands there. One it does not place stands where what
// carries it stands, and is said to: an idea where the work that sets it out was written, a thing where its maker's works
// were, a person where their works were. A problem stands nowhere: it is asked wherever it is answered.
function placeAll() {
  S.where = new Map();
  for (const o of S.data.objects) if ((o.places || []).length) S.where.set(o.id, o.places.map((p) => ({ ...p })));
  const lend = (to, from) => { if (S.where.has(to) || !S.where.has(from)) return; S.where.set(to, S.where.get(from).filter((p) => !p.through).slice(0, 2).map((p) => ({ ...p, through: from }))); };
  const rel = (type) => S.data.relations.filter((r) => r.type === type && r.standing !== 'disputed');
  for (const r of rel('sets_out')) lend(r.to, r.from);
  for (const r of rel('made_by')) if (!S.where.has(r.to) && S.where.has(r.from)) S.where.set(r.to, S.where.get(r.from).filter((p) => !p.through).slice(0, 2).map((p) => ({ ...p, through: r.from })));
  for (const r of rel('made_by')) lend(r.from, r.to);
}

async function reload() {
  const room = S.room;
  await openRecord(S.record, S.selected);
  if (room) openRoom(room, false);
}

// An object chosen: what the record says of it is asked, it joins the thread followed, and the page turns to it.
export async function select(id, { quiet = false } = {}) {
  if (!S.by.has(id)) return;
  S.selected = id;
  S.fanAt = null;
  S.detail = await api(`/api/object?record=${encodeURIComponent(S.record)}&id=${encodeURIComponent(id)}&layer=${S.layer}`);
  S.trail = [...S.trail.filter((t) => t !== id), id].slice(-7);
  if (S.view === 'desk' && !quiet) S.view = 'map';
  address();
  if (!quiet) refresh();
}

function clear() {
  S.selected = null;
  S.detail = null;
  S.fanAt = null;
  address();
  refresh();
}

function address() {
  const q = new URLSearchParams({ record: S.record, view: S.view, layer: S.layer });
  if (S.selected) q.set('object', S.selected);
  if (S.room) q.set('room', S.room);
  if (document.body.classList.contains('exhibit') && S.tour) q.set('exhibit', S.tour.id);
  history.replaceState(history.state, '', `?${q}`);
}

// ---- the whole page, drawn again ------------------------------------------------------------------------------------------------

function refresh() {
  const c = S.data.statements;
  const upTo = { established: c.established, attested: c.established + c.attested, proposed: c.established + c.attested + c.proposed };
  for (const b of $('layers').querySelectorAll('[data-layer]')) {
    b.classList.toggle('on', ORDER[b.dataset.layer] <= ORDER[S.layer]);
    b.classList.toggle('at', b.dataset.layer === S.layer);
    b.querySelector('b').textContent = upTo[b.dataset.layer];
  }
  for (const b of $('views').querySelectorAll('[data-view]')) b.classList.toggle('on', b.dataset.view === S.view);
  document.body.dataset.view = S.view;
  const notice = $('notice');
  notice.hidden = S.view === 'desk' || (upTo[S.layer] > 0 && !S.data.faults.length);
  if (!notice.hidden) {
    const wider = S.layer === 'established' && c.attested ? 'attested' : 'proposed';
    notice.replaceChildren(...(S.data.faults.length && upTo[S.layer] > 0
      ? [el('p', {}, `This record has ${S.data.faults.length} fault${S.data.faults.length > 1 ? 's' : ''}; the rows that stand are shown, and the desk lists the rest.`)]
      : [el('h2', {}, S.layer === 'established' ? 'Nothing here is established yet' : 'Nothing stands at this layer'),
        el('p', {}, S.layer === 'established'
          ? `Of this record's statements, ${c.attested} are attested and ${c.proposed} are a language model's proposals, written from memory and not yet held against a source. They can be read as proposals; they are not history confirmed.`
          : 'The record holds nothing at this depth of reading.'),
        el('button', { class: 'key strong', type: 'button', onclick: () => setLayer(wider) }, `Read the ${wider} too`)]));
  }
  stage();
  card();
  trail();
  clock();
  queueMicrotask(petals);
}

async function setLayer(layer) {
  S.layer = layer;
  if (S.selected) return select(S.selected);
  address();
  refresh();
}

// ---- the stage: map, loom, cone, desk --------------------------------------------------------------------------------------------

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

// What is tied to the object chosen, each with how: before it (in a role) or after it, at one step or further off.
function ties() {
  const out = new Map();
  if (!S.detail) return out;
  for (const e of S.detail.past) out.set(e.object, { ...e, side: 'past' });
  for (const e of S.detail.future) out.set(e.object, { ...e, side: 'future' });
  return out;
}
const coneIds = () => (S.detail ? [S.selected, ...ties().keys()] : null);

function stage() {
  const view = $('view'), tools = $('tools');
  view.className = `view ${S.view}`;
  if (S.view !== 'map') { S.map = null; S.flown = null; $('fan').hidden = true; }
  if (S.view !== 'loom') S.shownOn = null;
  $('legend').replaceChildren(...ROLE_ORDER.map((r) => el('span', {}, el('i', { style: `border-color:${ROLES[r].hue}` }), ROLES[r].short)),
    el('span', {}, el('i', { class: 'plain' }), 'established'), el('span', {}, el('i', { class: 'dashed' }), 'attested'), el('span', {}, el('i', { class: 'dotted' }), 'proposed'));
  if (S.view === 'loom') {
    const strand = STRANDS[S.strands];
    const objects = visible().filter((o) => !isA(o, 'Place')).map((o) => ({ id: o.id, name: o.name, kind: o.kind, above: o.above, year: o.appears, lane: strand.of(o), standing: o.standing, approximate: approx(o) }));
    const present = [...new Set(objects.map((o) => o.lane))];
    const lanes = [...strand.order.filter((l) => present.includes(l)), ...present.filter((l) => !strand.order.includes(l)).sort()]
      .map((id) => ({ id, name: strand.names[id] || title(S.data.kinds.find((k) => k.id === id)?.name || id) }));
    tools.replaceChildren(el('span', { class: 'caption' }, 'Strands by'), ...Object.keys(STRANDS).map((k) => el('button', { class: `key${S.strands === k ? ' on' : ''}`, type: 'button', onclick: () => { S.strands = k; stage(); queueMicrotask(petals); } }, STRANDS[k].name)), tourPick());
    view.innerHTML = svgLoom({ from: S.from, to: S.to, lanes, objects, threads: threads() }, { width: Math.max(view.clientWidth - 24, 1300), selected: S.selected, lit: coneIds(), now: S.year, evenness: 0.62 });
    if (S.selected !== S.shownOn && view.querySelector('.h-mark.selected')) { view.querySelector('.h-mark.selected').scrollIntoView({ block: 'center', inline: 'center' }); view.scrollLeft += 185; }
    S.shownOn = S.selected;
  } else if (S.view === 'cone') {
    tools.replaceChildren(tourPick());
    if (!S.selected || !S.detail) {
      view.replaceChildren(el('div', { class: 'hint' }, el('h2', {}, 'The cone of an object'), el('p', {}, 'Choose an object, on the map or the loom or by its name, to see what its appearing rested on and what rests on it.'),
        el('button', { class: 'key strong', type: 'button', onclick: openPalette }, 'Find an object')));
      return;
    }
    const entry = (e) => { const o = S.by.get(e.object); return { ...o, kindName: kindName(o), year: o.appears, approximate: approx(o), steps: e.steps, role: e.role, standing: e.steps === 1 ? e.standing : o.standing, inherited: e.inherited }; };
    const center = S.by.get(S.selected);
    const names = Object.fromEntries(S.data.objects.map((o) => [o.id, o.name]));
    view.innerHTML = svgCone({ center: { ...center, kindName: kindName(center), year: center.appears, approximate: approx(center) }, past: S.detail.past.map(entry), future: S.detail.future.map(entry),
      links: S.detail.links, lineage: S.detail.lineage.map((a) => ({ id: a.object, name: names[a.object] })) }, { names });
    if (view.querySelector('.h-slip.center')) { view.querySelector('.h-slip.center').scrollIntoView({ block: 'center', inline: 'center' }); view.scrollLeft += 185; }
  } else if (S.view === 'map') {
    tools.replaceChildren(tourPick());
    drawMap(view);
  } else {
    tools.replaceChildren();
    const bar = el('div', { class: 'desk-bar' }), body = el('div', { class: 'desk-body' });
    view.replaceChildren(bar, body);
    desk({ S, bar, body, api, el, select, reload, inLayer });
  }
}

// The places of the record on the sphere's map, by the clock. An object stands where the record places it, once it has
// appeared. When one is chosen, what it rests on and what rests on it are drawn to it as threads between places, flowing
// the way dependence runs, and the map draws near to them.
async function drawMap(view) {
  if (S.countries === undefined) {
    try { S.countries = await api('/data/natural-earth-50m/ne_50m_admin_0_countries.geojson'); } catch { S.countries = null; }
    if (typeof S.countries === 'string') S.countries = null;
  }
  if (S.view !== 'map') return;
  if (!S.map) {
    view.replaceChildren(el('div', { html: '<svg id="map" xmlns="http://www.w3.org/2000/svg"></svg>', class: 'map-host' }));
    S.map = createMap(view.querySelector('#map'), { countries: S.countries, nameOf: () => '', maxZoom: 14, onSettle: () => { overlay(); petals(); }, onPick: () => { if (S.fanAt) { S.fanAt = null; overlay(); } } });
  }
  overlay();
  if (S.selected && S.flown !== S.selected) {
    S.flown = S.selected;
    const T = ties();
    const points = [S.selected, ...[...T].filter(([, t]) => t.steps === 1).map(([id]) => id), ...S.detail.comparisons.filter((c) => c.independent).slice(0, 3).map((c) => c.object)].flatMap((id) => placesOf(id).slice(0, 1)).map((p) => S.map.planar.project(p.lon, p.lat));
    if (points.length) {
      S.map.fit(points, { margin: 0.42, max: 5.5, least: [70, 50] });
      // The card stands at the right: what is framed is set to the left of it.
      const v = S.map.view, aside = 190 * S.map.px() * v.k;
      S.map.setView({ k: v.k, x: v.x - aside, y: v.y + 30 * S.map.px() * v.k }, true);
    }
  }
  queueMicrotask(petals);
}

const placeOf = (id) => placesOf(id)[0];

function overlay() {
  const M = S.map;
  if (!M) return;
  const px = M.px(), NS = 'http://www.w3.org/2000/svg';
  const make = (tag, attrs, text) => { const n = document.createElementNS(NS, tag); for (const [k, v] of Object.entries(attrs)) n.setAttribute(k, v); if (text) n.textContent = text; return n; };
  const T = ties(), chosen = S.selected && S.by.get(S.selected);
  const span = S.to - S.from;
  const here = new Map();  // place id → the objects there by the clock
  for (const o of visible()) {
    if (isA(o, 'Place') || typeof o.appears !== 'number' || o.appears > S.year) continue;
    for (const p of placesOf(o.id)) {
      if (!here.has(p.place)) here.set(p.place, { ...p, objects: [] });
      here.get(p.place).objects.push(o);
    }
  }
  const marks = [], tags = [], twins = [];  // tags: the word written at the middle of a thread, saying what the tie is
  // The threads: one for each tie between two places, by role, drawn from what came before to what rests on it.
  if (chosen) {
    const drawn = new Set();
    for (const l of S.detail.links) {
      const direct = l.what === S.selected || l.product === S.selected;
      const a = placeOf(l.what), b = placeOf(l.product);
      if (!a || !b || a.place === b.place || (S.by.get(l.product).appears ?? 1e9) > S.year || (S.by.get(l.what).appears ?? 1e9) > S.year) continue;
      if (!direct && !(T.has(l.what) && T.has(l.product))) continue;
      const key = `${a.place}>${b.place}:${l.role}:${direct ? 1 : 0}`;
      if (drawn.has(key)) continue;
      drawn.add(key);
      const d = M.planar.greatCircle({ latitude: a.lat, longitude: a.lon }, { latitude: b.lat, longitude: b.lon }, 0.16);
      const hue = ROLES[l.role]?.hue || '';
      if (direct) marks.push(make('path', { class: 'arc glow', d, 'stroke-width': 7 * px, style: `stroke:${hue}` }));
      const thread = make('path', { class: `arc s-${l.standing}${direct ? ' direct' : ' beyond'}`, d, 'stroke-width': (direct ? 2 : 1.1) * px, 'stroke-dasharray': `${(direct ? 9 : 3) * px} ${(direct ? 7 : 6) * px}`, style: `stroke:${hue};--dash:${-16 * px}px` });
      marks.push(thread);
      if (direct) tags.push([thread, l.product === S.selected ? (ROLES[l.role]?.short || l.role) : 'then came']);
    }
  }
  // The echoes: who else answered the same problem with no path of dependence between them. A double thread, at rest.
  const echoes = new Set();
  if (chosen) {
    for (const c of S.detail.comparisons) {
      const a = placeOf(S.selected), b = placeOf(c.object);
      if (!c.independent || !a || !b || a.place === b.place || (S.by.get(c.object).appears ?? 1e9) > S.year) continue;
      echoes.add(c.object);
      const d = M.planar.greatCircle({ latitude: a.lat, longitude: a.lon }, { latitude: b.lat, longitude: b.lon }, 0.3);
      const twin = make('path', { class: 'arc echo', d, 'stroke-width': 4.2 * px });
      marks.push(twin, make('path', { class: 'arc echo inner', d, 'stroke-width': 2 * px }));
      twins.push(twin);
    }
  }
  const taken = [];  // the boxes of the names written, so that no name is written over another
  const rank = (p) => (chosen && p.objects.some((o) => o.id === S.selected) ? 0 : p.objects.some((o) => T.has(o.id) && T.get(o.id).steps === 1) ? 1 : p.objects.some((o) => echoes.has(o.id)) ? 2 : p.objects.some((o) => T.has(o.id)) ? 3 : 4);
  for (const [id, p] of [...here].sort((a, b) => rank(a[1]) - rank(b[1]) || b[1].objects.length - a[1].objects.length)) {
    const [x, y] = M.planar.project(p.lon, p.lat);
    const self = chosen && p.objects.some((o) => o.id === S.selected);
    const echo = p.objects.filter((o) => echoes.has(o.id));
    const tied = [...p.objects.filter((o) => T.has(o.id)), ...echo];
    const born = S.playing && p.objects.some((o) => o.appears > S.year - span / 45);
    const cls = `spot${self ? ' self' : tied.length ? ' lit' : chosen ? ' dim' : ''}${born ? ' born' : ''}`;
    const r = (3.6 + Math.sqrt(p.objects.length) * 2.1) * px;
    const g = make('g', { class: cls, 'data-place': id, transform: `translate(${x} ${y})` });
    g.append(make('title', {}, `${S.by.get(id)?.name || id}: ${p.objects.slice(0, 6).map((o) => o.name).join('; ')}${p.objects.length > 6 ? `; and ${p.objects.length - 6} more` : ''}`),
      make('circle', { class: 'halo', r: r + 5 * px, 'stroke-width': px }), make('circle', { class: 'knot', r, 'stroke-width': 1.2 * px }));
    g.addEventListener('pointerdown', (e) => e.stopPropagation());
    g.addEventListener('click', (e) => {
      e.stopPropagation();
      if (p.objects.length === 1) select(p.objects[0].id);
      else { S.fanAt = S.fanAt === id ? null : id; overlay(); }
    });
    marks.push(g);
    // Names: the place of what is chosen, the places tied to it with what is there, and every place once the map is near.
    const names = self ? S.by.get(id)?.name : tied.length ? `${tied.slice(0, 2).map((o) => o.name).join(' · ')}${tied.length > 2 ? ` +${tied.length - 2}` : ''}` : (!chosen && (p.objects.length > 2 || M.view.k >= 2.4)) || M.view.k >= 3.2 ? S.by.get(id)?.name : null;
    const size = (self ? 14 : 12.5) * px, under = tied.length && !self ? `${S.by.get(id)?.name || ''}${echo.length ? ' · the same problem, independently' : ''}` : '';
    const box = names && [x + r, y - size, x + r + 5 * px + Math.max(names.length, under.length * 0.8) * size * 0.5, y + (under ? 20 : 6) * px];
    if (names && (self || !taken.some((b) => box[0] < b[2] && b[0] < box[2] && box[1] < b[3] && b[1] < box[3]))) {
      taken.push(box);
      marks.push(make('text', { class: `spot-name${self ? ' self' : tied.length ? ' lit' : ''}`, x: x + r + 5 * px, y: y + 4 * px, 'font-size': size, 'stroke-width': 3.4 * px }, names));
      if (under) marks.push(make('text', { class: 'spot-place', x: x + r + 5 * px, y: y + 17 * px, 'font-size': 10.5 * px, 'stroke-width': 3 * px }, under));
    }
  }
  M.over.replaceChildren(...marks);
  // Of the echoes the longest is the one that says what they are.
  const longest = twins.sort((a, b) => b.getTotalLength() - a.getTotalLength())[0];
  if (longest) tags.push([longest, 'the same problem, independently']);
  for (const [thread, word] of tags) {
    const length = thread.getTotalLength?.() || 0;
    if (length < 52 * px) continue;
    const at = thread.getPointAtLength(length / 2);
    M.over.append(make('text', { class: 'arc-tag', x: at.x, y: at.y - 5 * px, 'font-size': 11 * px, 'stroke-width': 3.2 * px }, word));
  }
  // A place of several objects, opened: its objects in a fan beside it, those tied to what is chosen first.
  const fan = $('fan'), at = S.fanAt && here.get(S.fanAt), spot = at && M.over.querySelector(`[data-place="${S.fanAt}"]`);
  fan.hidden = !at || !spot;
  if (!fan.hidden) {
    const box = spot.getBoundingClientRect(), stageBox = $('stage').getBoundingClientRect();
    fan.style.left = `${Math.max(12, Math.min(box.right - stageBox.left + 10, stageBox.width - 330))}px`;
    fan.style.top = `${Math.max(60, Math.min(box.top - stageBox.top - 20, stageBox.height - 80 - at.objects.length * 30))}px`;
    const order = [...at.objects].sort((a, b) => Number(T.has(b.id)) - Number(T.has(a.id)) || a.appears - b.appears);
    fan.replaceChildren(el('h3', {}, `${S.by.get(S.fanAt).name}`, el('small', {}, ` · ${at.objects.length} by ${yearText(S.year)}`)),
      ...order.map((o) => el('button', { type: 'button', class: o.id === S.selected ? 'on' : '', onclick: () => select(o.id) }, el('span', { class: 'g' }, glyphOf(o)), el('span', { class: 'n' }, o.name), el('span', { class: 'y' }, yearText(o.appears)),
        T.has(o.id) ? el('i', { style: `background:${ROLES[T.get(o.id).role]?.hue || 'var(--accent-2)'}`, title: T.get(o.id).side === 'past' ? (ROLES[T.get(o.id).role]?.short || 'before it') : 'rests on it' }) : null)));
  }
}

// ---- what opens on an object: the ring of actions, and the card ----------------------------------------------------------------------

function activitiesFor(o) {
  const out = [];
  for (const p of S.plugins)
    for (const a of p.activities || [])
      if ((a.kinds || ['Object']).some((k) => isA(o, k)) && (!a.needs || o[a.needs] !== undefined)) out.push({ ...a, plugin: p });
  // What a discipline brings for this kind of object comes first: it is the most its own.
  const own = (a) => Number((a.kinds || ['Object'])[0] !== 'Object' || Boolean(a.needs));
  return out.sort((a, b) => own(b) - own(a));
}

// The ring: the activities of the object chosen, set round it where it stands, on whichever drawing shows it.
function petals() {
  const host = $('petals'), o = S.selected && S.by.get(S.selected);
  const target = !o || S.room || S.view === 'desk' ? null
    : S.view === 'map' ? $('view').querySelector('.spot.self .knot') : S.view === 'loom' ? $('view').querySelector('.h-mark.selected .h-hit') : $('view').querySelector('.h-slip.center rect');
  const stageBox = $('stage').getBoundingClientRect(), box = target?.getBoundingClientRect();
  if (!box || box.bottom < stageBox.top + 50 || box.top > stageBox.bottom - 70 || box.right < stageBox.left || box.left > stageBox.right) { host.hidden = true; return; }
  const acts = activitiesFor(o).slice(0, 6);
  const cx = box.left + box.width / 2 - stageBox.left, cy = box.top + box.height / 2 - stageBox.top;
  const radius = Math.max(box.height / 2 + 50, 58), below = cy < 210;
  const spread = Math.min(190, 42 * acts.length), start = (below ? 90 : -90) - spread / 2;
  const key = `${S.selected}:${S.view}`;
  const fresh = host.dataset.key !== key;
  host.dataset.key = key;
  host.hidden = false;
  const cardBox = $('card').hidden ? null : $('card').getBoundingClientRect();
  host.replaceChildren(...acts.map((a, i) => {
    const angle = (start + (acts.length === 1 ? spread / 2 : (spread * i) / (acts.length - 1))) * Math.PI / 180;
    const x = cx + Math.cos(angle) * (radius * 2.1 + box.width / 2), y = cy + Math.sin(angle) * radius * 1.15;
    // A petal that would lie on the card is left to the card, which offers the same.
    if (cardBox && x + stageBox.left > cardBox.left - 50 && y + stageBox.top < cardBox.bottom + 16) return null;
    return el('button', { type: 'button', class: `petal${fresh ? ' fresh' : ''}${i === 0 ? ' first' : ''}`, style: `left:${x.toFixed(0)}px;top:${y.toFixed(0)}px;animation-delay:${i * 45}ms`, title: a.about || '', onclick: () => openRoom(a.id) }, a.name);
  }));
}

function card() {
  const host = $('card'), o = S.selected && S.by.get(S.selected);
  host.hidden = !o || !S.detail || S.view === 'desk';
  if (host.hidden) return;
  const Q = S.detail.inquiry, names = Object.values(o.names || {}).join(' · ');
  const line = (label, answers, hue) => (answers.length ? el('div', { class: 'tie' }, el('span', { class: 'role', style: hue ? `border-color:${hue}` : '' }, label),
    el('span', { class: 'who' }, ...answers.slice(0, 3).flatMap((a, i) => [i ? ', ' : '', el('a', { href: '#', onclick: (e) => { e.preventDefault(); select(a.object); } }, S.by.get(a.object).name)]), answers.length > 3 ? ` and ${answers.length - 3} more` : '')) : null);
  const acts = activitiesFor(o);
  host.replaceChildren(
    el('button', { class: 'close', type: 'button', title: 'Let it go (Esc)', onclick: clear }, '×'),
    el('div', { class: 'kind' }, `${glyphOf(o)} ${kindName(o)}`, el('span', { class: `chip ${o.standing}`, title: 'How the record\'s word on this object stands' }, o.standing)),
    el('h1', {}, o.name), names ? el('div', { class: 'own' }, names) : null,
    el('div', { class: 'meta' }, whenText(o), placeNames(o).length ? ` · ${placeNames(o).join(', ')}` : ''),
    o.summary ? el('p', { class: 'summary' }, o.summary) : null,
    el('div', { class: 'ties' },
      line('answers', Q.problems, ROLES.problem.hue), line('made possible by', Q.answerable.filter((a) => a.role === 'condition'), ROLES.condition.hue),
      line('draws on', Q.answerable.filter((a) => a.role === 'source'), ROLES.source.hue), line('argues against', Q.against, ROLES.against.hue), line('then came', Q.after, null),
      line('answered also, independently, by', S.detail.comparisons.filter((c) => c.independent), null),
      S.detail.past.length + S.detail.future.length === 0 ? el('p', { class: 'silent' }, 'The record ties nothing to it at this depth of reading.') : null),
    el('div', { class: 'acts' }, ...acts.map((a, i) => el('button', { class: `act${i === 0 ? ' first' : ''}`, type: 'button', title: a.about || '', onclick: () => openRoom(a.id) }, el('b', {}, a.invite || a.name), el('span', {}, '→')))),
    el('div', { class: 'see' }, 'See it ', ...['map', 'loom', 'cone'].filter((v) => v !== S.view).map((v) => el('button', { class: 'plain', type: 'button', onclick: () => { S.view = v; address(); refresh(); } }, `on the ${v}`))));
}

function trail() {
  const host = $('trail');
  const ids = S.trail.filter((id) => S.by.has(id));
  host.hidden = ids.length < 2 || S.view === 'desk';
  if (!host.hidden) host.replaceChildren(el('span', { class: 'caption' }, 'The thread you have followed'), ...ids.flatMap((id, i) => [i ? el('i', {}, '›') : null,
    el('button', { type: 'button', class: id === S.selected ? 'on' : '', onclick: () => select(id) }, S.by.get(id).name)]));
}

// ---- the clock, and the questions of a year --------------------------------------------------------------------------------------

let asking = null;
function clock() {
  $('clock').hidden = S.view === 'desk' || S.view === 'cone';
  $('year').value = S.year;
  $('year-text').textContent = yearText(S.year);
  $('year').style.setProperty('--p', `${(100 * (S.year - S.from) / (S.to - S.from || 1)).toFixed(2)}%`);
  clearTimeout(asking);
  asking = setTimeout(async () => {
    const y = Math.floor(S.year), date = `${y < 0 ? '-' : ''}${String(Math.abs(y)).padStart(4, '0')}`;
    let f;
    try { f = await api(`/api/frontier?record=${encodeURIComponent(S.record)}&date=${date}&layer=${S.layer}`); } catch { return; }
    const chips = f.problems.sort((a, b) => Number(b.open) - Number(a.open)).map((q) => el('button', { type: 'button', class: `q${q.open ? ' open' : ''}${q.problem === S.selected ? ' on' : ''}`, title: q.open ? 'Posed, and not yet answered' : 'Answered by then', onclick: () => select(q.problem) },
      `${S.by.get(q.problem).name}`, el('small', {}, q.open ? 'open' : `${q.answered.length} answer${q.answered.length > 1 ? 's' : ''}`)));
    $('questions').replaceChildren(...(chips.length ? [el('span', { class: 'caption' }, `The questions of ${yearText(S.year)}`), ...chips] : []));
  }, 160);
}

// ---- finding an object by its name ---------------------------------------------------------------------------------------------------

function openPalette() {
  $('palette').hidden = false;
  $('find').value = S.find = '';
  finder();
  $('find').focus();
}
function closePalette() { $('palette').hidden = true; }

function finder() {
  const all = visible();
  const tops = [...new Set(all.map((o) => (o.above?.length > 1 ? o.above[o.above.length - 2] : o.kind)))].sort();
  $('kinds').replaceChildren(...tops.map((k) => el('button', { class: `key${S.kinds.has(k) ? ' on' : ''}`, type: 'button', onclick: () => { S.kinds.has(k) ? S.kinds.delete(k) : S.kinds.add(k); finder(); } },
    `${glyphOf({ kind: k })} ${S.data.kinds.find((x) => x.id === k)?.name || k}`)));
  const want = S.find.trim().toLowerCase();
  const shown = all.filter((o) => (!S.kinds.size || [...S.kinds].some((k) => isA(o, k))) &&
    (!want || o.name.toLowerCase().includes(want) || Object.values(o.names || {}).some((n) => n.toLowerCase().includes(want))))
    .sort((a, b) => (a.appears ?? 1e9) - (b.appears ?? 1e9) || a.name.localeCompare(b.name));
  $('list').replaceChildren(...(shown.length ? shown.map((o) => el('li', { 'data-id': o.id, onclick: () => { closePalette(); select(o.id); } },
    el('span', { class: 'g' }, glyphOf(o)), el('span', { class: 'n' }, o.name, Object.values(o.names || {})[0] ? el('small', {}, ` ${Object.values(o.names)[0]}`) : null), el('span', { class: 'k' }, kindName(o)), el('span', { class: 'y' }, yearText(o.appears))))
    : [el('li', { class: 'none' }, all.length ? 'No object of that name.' : 'Nothing stands at this depth of reading: read the attested or the proposed too.')]));
}

// ---- rooms: a page of its own for one object and one activity ----------------------------------------------------------------------------

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
        box.append(el('div', { class: 'ask' }, el('div', { class: 'qtext' }, m[1]), el('button', { class: 'key', type: 'button', onclick: (e) => { answer.hidden = false; e.target.remove(); } }, 'Think first, then show'), answer));
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
        openRoom(activity.id, false);
      } catch (error) { msg.textContent = error.message; }
    } }, 'Keep'), msg));
}

let entering = 0;
// Enter the room of an activity for the object chosen. A plug-in may give the room a page of its own design (its module's
// `room`); otherwise what it renders for the activity is set on a page of the common design.
async function openRoom(id, push = true) {
  const o = S.selected && S.by.get(S.selected);
  if (!o || !S.detail) return;
  const acts = activitiesFor(o);
  const activity = acts.find((a) => a.id === id) || acts.find((a) => a.id === 'inquire') || acts[0];
  const mine = ++entering, host = $('room');
  const was = S.room;
  S.room = activity.id;
  if (push && !was) history.pushState({ room: true }, '');
  address();
  $('petals').hidden = true;
  host.hidden = false;
  host.dataset.activity = activity.id;
  host.classList.toggle('entering', !was);
  const body = el('div', { class: 'room-body' }, el('p', { class: 'silent wait' }, '…'));
  const names = Object.values(o.names || {}).join(' · ');
  host.replaceChildren(
    el('header', { class: 'room-top' },
      el('button', { class: 'back', type: 'button', onclick: closeRoom }, `← Back to the ${S.view === 'desk' ? 'map' : S.view}`),
      el('div', { class: 'room-of' }, el('span', { class: 'g' }, glyphOf(o)), el('b', {}, o.name), names ? el('span', { class: 'own' }, names) : null, el('span', { class: 'when' }, `${kindName(o)} · ${whenText(o)}`), el('span', { class: `chip ${o.standing}` }, o.standing)),
      el('nav', { class: 'room-acts' }, ...acts.map((a) => el('button', { type: 'button', class: a.id === activity.id ? 'on' : '', title: a.about || '', onclick: () => openRoom(a.id, false) }, a.name)))),
    body);
  host.scrollTop = 0;
  try {
    const [mod, written] = await Promise.all([moduleOf(activity.plugin), authored(o, activity)]);
    if (mine !== entering) return;
    // In a room, to choose another object is to go on to it and stay in the room, with the same activity where it has one.
    const go = async (other) => { await select(other, { quiet: true }); refresh(); openRoom(activity.id, false); };
    const ctx = { S, object: o, detail: S.detail, record: S.data, by: S.by, layer: S.layer, api, el, select: go, go, reload, inLayer, kindName, whenText, placeNames, glyphOf, yearText, esc, ROLES, ROLE_ORDER,
      svgCone, writable: S.writable, activity, openRoom: (a) => openRoom(a, false), closeRoom };
    const made = mod.room?.[activity.id] ? await mod.room[activity.id](ctx) : el('div', { class: 'room-plain pane' }, await mod.render[activity.id](ctx));
    if (mine !== entering) return;
    body.replaceChildren(written.node ? el('div', { class: 'room-plain' }, written.node) : null, made, el('div', { class: 'room-plain' }, writer(o, activity, written.node && !written.inherited ? written.doc : null)));
  } catch (error) {
    body.replaceChildren(el('p', { class: 'msg' }, `This room could not be opened: ${error.message}`));
  }
}

function closeRoom() {
  if (!S.room) return;
  S.room = null;
  entering++;
  $('room').hidden = true;
  $('room').replaceChildren();
  if (history.state?.room) history.back();
  address();
  refresh();
}

// ---- a path walked: an exhibit's tour ------------------------------------------------------------------------------------------------

const tours = () => S.plugins.flatMap((p) => (p.tours || []).filter((t) => t.record === S.record));
function tourPick() {
  const all = tours();
  if (!all.length || S.tour) return null;
  return el('select', { class: 'walk', 'aria-label': 'A path through the record', onchange: (e) => e.target.value && startTour(e.target.value) }, el('option', { value: '' }, 'Walk a path…'), ...all.map((t) => el('option', { value: t.id }, t.name)));
}
function startTour(id) {
  S.tour = tours().find((t) => t.id === id) || tours()[0] || null;
  S.stop = 0;
  if (S.tour) { if (S.tour.layer === 'proposed') S.layer = 'proposed'; showStop(); }
}
function endTour() { S.tour = null; $('tour').hidden = true; refresh(); }
async function showStop() {
  const t = S.tour, stop = t.stops[S.stop];
  $('tour').hidden = false;
  $('tour').replaceChildren(
    el('button', { class: 'key', type: 'button', disabled: S.stop === 0, onclick: () => { S.stop--; showStop(); } }, '←'),
    el('div', { class: 'say' }, stop.say),
    stop.activity ? el('button', { class: 'key strong', type: 'button', onclick: () => openRoom(stop.activity) }, 'Enter') : null,
    el('span', { class: 'at' }, `${S.stop + 1} of ${t.stops.length} · ${t.name}`),
    el('button', { class: 'key', type: 'button', disabled: S.stop === t.stops.length - 1, onclick: () => { S.stop++; showStop(); } }, '→'),
    el('button', { class: 'plain', type: 'button', onclick: endTour }, 'End'));
  const o = S.by.get(stop.object);
  if (o && typeof o.appears === 'number') S.year = Math.min(S.to, Math.ceil(o.appears) + 1);
  if (stop.view) S.view = stop.view;
  if (S.room) { S.room = null; $('room').hidden = true; }
  await select(stop.object);
}

// ---- the keys -------------------------------------------------------------------------------------------------------------------

function wire() {
  $('record').addEventListener('change', (e) => { S.year = null; S.trail = []; openRecord(e.target.value); });
  $('views').addEventListener('click', (e) => { const b = e.target.closest('[data-view]'); if (b) { S.view = b.dataset.view; address(); refresh(); } });
  $('layers').addEventListener('click', (e) => { const b = e.target.closest('[data-layer]'); if (b) setLayer(b.dataset.layer); });
  $('find-open').addEventListener('click', openPalette);
  $('find').addEventListener('input', (e) => { S.find = e.target.value; finder(); });
  $('find').addEventListener('keydown', (e) => { if (e.key === 'Enter') { const first = $('list').querySelector('li[data-id]'); if (first) { closePalette(); select(first.dataset.id); } } });
  $('palette').addEventListener('click', (e) => { if (e.target === $('palette')) closePalette(); });
  $('view').addEventListener('click', (e) => { const g = e.target.closest('[data-object]'); if (g) select(g.dataset.object); });
  $('view').addEventListener('scroll', () => petals(), { passive: true });
  $('year').addEventListener('input', (e) => { S.year = Number(e.target.value); if (S.view === 'map') overlay(); else if (S.view === 'loom') stage(); clock(); petals(); });
  $('play').addEventListener('click', (e) => {
    if (S.playing) { clearInterval(S.playing); S.playing = null; e.target.textContent = '▷'; if (S.view === 'map') overlay(); return; }
    if (S.year >= S.to) S.year = S.from;
    e.target.textContent = '❚❚';
    S.playing = setInterval(() => {
      S.year = Math.min(S.to, S.year + Math.max(1, Math.round((S.to - S.from) / 420)));
      $('year').value = S.year;
      $('year').dispatchEvent(new Event('input'));
      if (S.year >= S.to) $('play').click();
    }, 60);
  });
  $('night').addEventListener('click', () => { const on = document.documentElement.classList.toggle('night'); localStorage.setItem('histos.night', on ? '1' : '0'); });
  window.addEventListener('resize', () => { if (S.view === 'loom') stage(); petals(); });
  window.addEventListener('popstate', () => { if (S.room) { S.room = null; entering++; $('room').hidden = true; $('room').replaceChildren(); address(); refresh(); } });
  window.addEventListener('keydown', (e) => {
    const typing = /^(INPUT|TEXTAREA|SELECT)$/.test(document.activeElement?.tagName || '');
    if (e.key === 'Escape') { if (!$('palette').hidden) closePalette(); else if (S.room) closeRoom(); else if (S.fanAt) { S.fanAt = null; overlay(); } else if (S.selected) clear(); }
    else if (!typing && (e.key === '/' || (e.key === 'k' && (e.ctrlKey || e.metaKey)))) { e.preventDefault(); openPalette(); }
    else if (!typing && S.tour && e.key === 'ArrowRight' && S.stop < S.tour.stops.length - 1) { S.stop++; showStop(); }
    else if (!typing && S.tour && e.key === 'ArrowLeft' && S.stop > 0) { S.stop--; showStop(); }
  });
}

boot().catch((error) => { document.body.append(el('div', { class: 'setup' }, `The page could not start: ${error.message}`)); });
