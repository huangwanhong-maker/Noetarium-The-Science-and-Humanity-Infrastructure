// Cards: live views among the notes of a board (DOM). A card draws from what stands on the board (its
// tables by name, the quantities its notes define) and from what the work that hosts the board serves
// it (data, the running of modules). Three are every board's:
//
//   chart    a table drawn as bars, lines or points (the toolbox's charts, visualization/js/toolbox)
//   map      a table's places on the Earth: points by `lat` and `lon`, economies by `iso3`, over the
//            countries or, for a town, over a layer of the data library (the 2D GIS's projections)
//   module   a module run on the board: a plug-in's, or a few lines written on the card. It is given
//            the board's tables, quantities and notes and answers tables and notes (api/python/science/board.py)
//
// A plug-in adds cards of its own in the same form:
//
//   {label, about, size: {w, h}, height?: 'grow', props: {defaults},
//    form(ctx) → [{name, label, kind: 'text' | 'number' | 'integer' | 'boolean' | 'choice' | 'code', role?, of?, options?, group?, help?}],
//    signature(ctx) → what its drawing depends on beyond its own props, as text,
//    render(body, ctx)}
//
// with ctx = {id, props, out, state, tables, values, services, zoom(), set(props), setOut(out), setState(state), given(), say(words)}.
// What is marked data-live in a card answers the pointer itself; the card is moved by its heading.

import { h, s, clear } from './dom.js';
import { columnNames, findColumn } from './tables.js';
import { barChart, lineChart, scatterChart } from '../toolbox/charts.js';
import { planar } from '../map/projection.js';
import { colourMap } from '../../styles/palettes.js';
import { cssColour } from '../../styles/colour.js';

// A table's stamp: its name and which data it holds, so that a card is drawn again when they change.
const stamps = new WeakMap();
let stampCount = 0;
export function tableStamp(t) {
  if (!t) return 'none';
  if (t.error) return `!${t.error}`;
  if (!stamps.has(t.data)) stamps.set(t.data, ++stampCount);
  return `${t.name}#${stamps.get(t.data)}`;
}

const hint = (words) => h('div', { class: 'bd-hint' }, words);
function tableOf(ctx, body) {
  const name = ctx.props.table;
  if (!name) { body.append(hint(ctx.tables.size ? 'Choose a table in the panel.' : 'There is no table on the board yet: paste or drop one, or run a module that answers one.')); return null; }
  const t = ctx.tables.get(name);
  if (!t) { body.append(hint(`There is no table '${name}' on the board.`)); return null; }
  if (t.error) { body.append(h('div', { class: 'bd-hint wrong' }, t.error)); return null; }
  return t;
}
const numbers = (t, column) => t.data[column].map((v) => (typeof v === 'number' ? v : NaN));
const words = (t, column) => t.data[column].map((v) => (v === null || v === undefined ? '' : String(v)));
const listOf = (text) => String(text || '').split(',').map((x) => x.trim()).filter(Boolean);

// The toolbox's charts place their readout by the screen; on a board that is zoomed the readout is set
// by the board's own measure.
function steadyTips(body, ctx) {
  body.addEventListener('pointermove', () => {
    const k = ctx.zoom();
    for (const tip of body.querySelectorAll('.chart-tip.on')) {
      tip.style.left = `${(parseFloat(tip.style.left) - 14) / k + 14}px`;
      tip.style.top = `${(parseFloat(tip.style.top) - 8) / k + 8}px`;
    }
  });
}

// ---- a chart ----------------------------------------------------------------------------------------

const chart = {
  label: 'Chart', about: 'A table drawn as bars, lines or points.', size: { w: 540, h: 360 },
  props: { table: '', form: 'bar', x: '', y: '', label: '', most: 20, title: '' },
  form: () => [
    { name: 'table', label: 'Table', kind: 'text', role: 'table' },
    { name: 'form', label: 'Drawn as', kind: 'choice', options: [['bar', 'Bars'], ['line', 'Lines'], ['scatter', 'Points']] },
    { name: 'x', label: 'Across', kind: 'text', role: 'column', of: 'table' },
    { name: 'y', label: 'Up', kind: 'text', role: 'columns', of: 'table', help: 'one column, or several with commas between' },
    { name: 'label', label: 'Named by', kind: 'text', role: 'column', of: 'table' },
    { name: 'most', label: 'At most', kind: 'integer', min: 2, max: 60, help: 'bars: the first rows of the table' },
    { name: 'title', label: 'Title', kind: 'text' },
  ],
  signature: (ctx) => tableStamp(ctx.tables.get(ctx.props.table)),
  render(body, ctx) {
    const t = tableOf(ctx, body);
    if (!t) return;
    const p = ctx.props, numeric = columnNames(t, 'number'), text = columnNames(t, 'text');
    const ys = listOf(p.y).filter((c) => t.data[c]);
    const title = p.title || t.title || t.name;
    let figure;
    if (p.form === 'scatter') {
      const x = t.data[p.x] ? p.x : numeric[0], y = ys[0] || numeric.find((c) => c !== x);
      if (!x || !y) { body.append(hint('Points need two columns of numbers.')); return; }
      const label = t.data[p.label] ? p.label : text[0];
      figure = scatterChart({ title, x: numbers(t, x), y: numbers(t, y), xlabel: x, ylabel: y, labels: label ? words(t, label) : null });
    } else if (p.form === 'line') {
      const x = t.data[p.x] && numeric.includes(p.x) ? p.x : null;
      const series = (ys.length ? ys : numeric.filter((c) => c !== x).slice(0, 1)).filter((c) => numeric.includes(c));
      if (!series.length) { body.append(hint('Lines need a column of numbers.')); return; }
      const xs = x ? numbers(t, x) : t.data[series[0]].map((_, i) => i + 1);
      const order = xs.map((_, i) => i).filter((i) => Number.isFinite(xs[i])).sort((a, b) => xs[a] - xs[b]);
      figure = lineChart({ title, x: order.map((i) => xs[i]), series: series.map((c) => ({ name: c, y: order.map((i) => numbers(t, c)[i]) })), xlabel: x || 'row', ylabel: series.length === 1 ? series[0] : '' });
    } else {
      const x = t.data[p.x] ? p.x : text[0] || columnNames(t)[0];
      const series = (ys.length ? ys : numeric.filter((c) => c !== x).slice(0, 1)).filter((c) => numeric.includes(c));
      if (!x || !series.length) { body.append(hint('Bars need a column of names and a column of numbers.')); return; }
      const n = Math.min(t.rows, Math.max(2, Number(p.most) || 20));
      figure = barChart({ title: n < t.rows ? `${title} (the first ${n} of ${t.rows})` : title, labels: words(t, x).slice(0, n), series: series.map((c) => ({ name: c, values: numbers(t, c).slice(0, n) })) });
    }
    body.append(figure);
    steadyTips(body, ctx);
  },
};

// ---- a map ------------------------------------------------------------------------------------------

const COUNTRIES = 'natural-earth-50m/ne_50m_admin_0_countries.geojson';
const DISPUTED = 'natural-earth-50m/ne_50m_admin_0_breakaway_disputed_areas.geojson';
const codeOf = (p) => ['ISO_A3', 'ISO_A3_EH', 'ADM0_A3'].map((k) => p[k]).find((v) => v && v !== '-99' && v.length === 3) || null;
const f2 = (v) => v.toFixed(2);

// A scale of colour for a column: of numbers, the orchid map from its least to its most (by
// logarithms when they are positive and span more than two decades); of text, a hue for each kind in
// the order it first appears.
function colouring(t, column) {
  if (!column || !t.data[column]) return null;
  const kind = t.columns.find((c) => c.name === column)?.kind;
  if (kind === 'number') {
    const values = t.data[column].filter((v) => typeof v === 'number');
    if (!values.length) return null;
    const lo = Math.min(...values), hi = Math.max(...values);
    const log = lo > 0 && hi / lo > 100;
    const map = colourMap('orchid');
    const u = (v) => (hi > lo ? (log ? Math.log(v / lo) / Math.log(hi / lo) : (v - lo) / (hi - lo)) : 0.5);
    return { kind: 'number', lo, hi, log, of: (v) => (typeof v === 'number' ? cssColour(map.colour(u(v))) : null), gradient: map.gradient() };
  }
  const kinds = [...new Set(t.data[column].filter((v) => v !== null && v !== ''))];
  const map = colourMap('categorical-apart');
  const index = new Map(kinds.map((k, i) => [k, i]));
  return { kind: 'text', kinds, of: (v) => (index.has(v) ? cssColour(map.colour(index.get(v))) : null) };
}

const short = (v) => {
  if (typeof v !== 'number' || !Number.isFinite(v)) return v === null || v === undefined ? '—' : String(v);
  const a = Math.abs(v);
  for (const [from, unit, suffix] of [[1e12, 1e12, ' T'], [1e9, 1e9, ' B'], [1e6, 1e6, ' M'], [1e4, 1e3, ' k']]) if (a >= from) return `${(v / unit).toPrecision(3).replace(/\.?0+$/, '')}${suffix}`;
  return String(+v.toPrecision(3));
};

const map = {
  label: 'Map', about: 'A table\'s places on the Earth: points by lat and lon, economies by iso3.', size: { w: 620, h: 420 },
  props: { table: '', value: '', size: '', label: '', geo: '', title: '' },
  form: () => [
    { name: 'table', label: 'Table', kind: 'text', role: 'table', help: 'with columns lat and lon, or iso3' },
    { name: 'value', label: 'Coloured by', kind: 'text', role: 'column', of: 'table' },
    { name: 'size', label: 'Sized by', kind: 'text', role: 'column', of: 'table', help: 'points only' },
    { name: 'label', label: 'Named by', kind: 'text', role: 'column', of: 'table' },
    { name: 'geo', label: 'Over', kind: 'text', help: 'a GeoJSON of the data library drawn beneath, as dataset/…/file.geojson' },
    { name: 'title', label: 'Title', kind: 'text' },
  ],
  signature: (ctx) => tableStamp(ctx.tables.get(ctx.props.table)),
  render(body, ctx) {
    const t = tableOf(ctx, body);
    if (!t) return;
    const p = ctx.props;
    const lat = findColumn(t, 'lat', 'latitude'), lon = findColumn(t, 'lon', 'lng', 'long', 'longitude'), code = findColumn(t, 'iso3', 'iso_a3', 'code');
    if (!(lat && lon) && !code) { body.append(hint(`The table '${t.name}' has neither lat and lon nor iso3: a map does not know where its rows are.`)); return; }
    if (!ctx.services.data) { body.append(hint('This work serves no data: a map cannot be drawn here.')); return; }
    const points = Boolean(lat && lon);
    const label = t.data[p.label] ? p.label : findColumn(t, 'name', 'title', 'label');
    const colour = colouring(t, t.data[p.value] ? p.value : null);
    const W = Math.max(200, ctx.width - 26), H = Math.max(140, ctx.height - 96);
    const svg = s('svg', { class: 'bd-map', viewBox: `0 0 ${W} ${H}`, width: W, height: H, 'data-live': true });
    const readout = h('div', { class: 'bd-map-readout' }, p.title || t.title || t.name);
    body.append(svg, readout);
    svg.append(s('text', { class: 'bd-map-wait', x: W / 2, y: H / 2, 'text-anchor': 'middle' }, 'Reading the map…'));

    // Where the rows are, and how far they reach.
    const rows = [];
    if (points) {
      const la = numbers(t, lat), lo = numbers(t, lon);
      for (let i = 0; i < t.rows && rows.length < 20000; i++) if (Number.isFinite(la[i]) && Number.isFinite(lo[i]) && Math.abs(la[i]) <= 90 && Math.abs(lo[i]) <= 180) rows.push(i);
    }
    const la = points ? numbers(t, lat) : [], lo = points ? numbers(t, lon) : [];
    const span = points && rows.length ? { s: Math.min(...rows.map((i) => la[i])), n: Math.max(...rows.map((i) => la[i])), w: Math.min(...rows.map((i) => lo[i])), e: Math.max(...rows.map((i) => lo[i])) } : null;
    // A town or a district is drawn on its own plane, east and north scaled alike at its middle; anything
    // wider on Equal Earth, over the countries.
    const local = Boolean(span) && span.n - span.s < 4 && span.e - span.w < 4;

    (async () => {
      let countries = null, disputed = null, geo = null, failure = '';
      try {
        if (!local) [countries, disputed] = await Promise.all([ctx.services.data(COUNTRIES), ctx.services.data(DISPUTED).catch(() => null)]);
        if (p.geo) geo = await ctx.services.data(p.geo);
      } catch (error) { failure = error.message; }
      if (!svg.isConnected) return;
      clear(svg);
      if (failure && !countries && !local) { svg.append(s('text', { class: 'bd-map-wait', x: W / 2, y: H / 2, 'text-anchor': 'middle' }, `The map could not be read: ${failure}`)); return; }

      let project, outline = '';
      if (local) {
        const lat0 = (span.s + span.n) / 2, lon0 = (span.w + span.e) / 2, c = Math.cos((lat0 * Math.PI) / 180);
        project = (x, y) => [(x - lon0) * c * 1000, -(y - lat0) * 1000];
      } else {
        const P = planar('equal-earth', 100);
        project = P.project;
        outline = P.outline();
      }
      const ring = (r) => `${r.map(([x, y], i) => { const [a, b] = project(x, y); return `${i ? 'L' : 'M'}${f2(a)},${f2(b)}`; }).join('')}Z`;
      const line = (r) => r.map(([x, y], i) => { const [a, b] = project(x, y); return `${i ? 'L' : 'M'}${f2(a)},${f2(b)}`; }).join('');
      const shape = (g) => {
        if (!g) return '';
        if (g.type === 'Polygon') return g.coordinates.map(ring).join('');
        if (g.type === 'MultiPolygon') return g.coordinates.map((poly) => poly.map(ring).join('')).join('');
        if (g.type === 'LineString') return line(g.coordinates);
        if (g.type === 'MultiLineString') return g.coordinates.map(line).join('');
        return '';
      };

      const world = s('g', {});
      svg.append(world);
      if (!local) world.append(s('path', { class: 'sphere', d: outline }));
      // Economies by their codes: the value of each, and its row.
      const byCode = new Map();
      if (!points) t.data[code].forEach((c, i) => { if (c) byCode.set(String(c).toUpperCase(), i); });
      const told = (i) => `${label ? t.data[label][i] ?? '' : ''}${colour && p.value ? `${label ? ' · ' : ''}${p.value} ${short(t.data[p.value][i])}` : ''}`;
      const say = (words) => { readout.textContent = words || p.title || t.title || t.name; };
      for (const f of countries?.features || []) {
        const i = byCode.get(codeOf(f.properties));
        const fill = !points && i !== undefined && colour ? colour.of(t.data[p.value][i]) : null;
        const path = s('path', { class: fill ? 'country valued' : 'country', d: shape(f.geometry), style: fill ? `fill:${fill}` : null });
        if (i !== undefined && !points) { path.addEventListener('pointerenter', () => say(told(i))); path.addEventListener('pointerleave', () => say('')); }
        world.append(path);
      }
      // An area in dispute is drawn in no claimant's colour, hatched, and without a name (ADR 0069).
      if (disputed?.features?.length) {
        const id = `bd-hatch-${ctx.id}`;
        svg.append(s('defs', {}, s('pattern', { id, patternUnits: 'userSpaceOnUse', width: 5, height: 5, patternTransform: 'rotate(45)' }, s('rect', { class: 'hatch-ground', width: 5, height: 5 }), s('path', { class: 'hatch-line', d: 'M0 0V5' }))));
        for (const f of disputed.features) world.append(s('path', { class: 'disputed', d: shape(f.geometry), fill: `url(#${id})` }));
      }
      for (const f of geo?.features || (geo?.type === 'Feature' ? [geo] : [])) world.append(s('path', { class: 'layer', d: shape(f.geometry) }));

      // Points: their size by the square root of a column, so that areas compare.
      const dots = [];
      if (points) {
        const sized = t.data[p.size] ? numbers(t, p.size) : null, most = sized ? Math.max(...rows.map((i) => sized[i]).filter(Number.isFinite), 0) : 0;
        for (const i of rows) {
          const [x, y] = project(lo[i], la[i]);
          const radius = sized && most > 0 && Number.isFinite(sized[i]) ? 2.2 + 8.5 * Math.sqrt(Math.max(0, sized[i]) / most) : 3.4;
          const dot = s('circle', { class: 'point', cx: f2(x), cy: f2(y), style: colour ? `fill:${colour.of(t.data[p.value][i]) || 'var(--bd-faint)'}` : null });
          dot.addEventListener('pointerenter', () => say(told(i)));
          dot.addEventListener('pointerleave', () => say(''));
          dots.push([dot, radius]);
          world.append(dot);
        }
      }

      // The view: what is drawn, fitted; then the wheel and the hand.
      let box;
      if (points && rows.length) {
        const xy = rows.map((i) => project(lo[i], la[i]));
        box = { x0: Math.min(...xy.map((q) => q[0])), x1: Math.max(...xy.map((q) => q[0])), y0: Math.min(...xy.map((q) => q[1])), y1: Math.max(...xy.map((q) => q[1])) };
        if (!local && (box.x1 - box.x0 > 260 || box.y1 - box.y0 > 130)) box = null;
      }
      if (!box) { const b = world.getBBox(); box = { x0: b.x, x1: b.x + b.width, y0: b.y, y1: b.y + b.height }; }
      const bw = Math.max(box.x1 - box.x0, 1e-6), bh = Math.max(box.y1 - box.y0, 1e-6);
      const k0 = Math.min((W * 0.86) / bw, (H * 0.86) / bh);
      const home = { k: k0, x: W / 2 - ((box.x0 + box.x1) / 2) * k0, y: H / 2 - ((box.y0 + box.y1) / 2) * k0 };
      let view = { ...home };
      const apply = () => {
        world.setAttribute('transform', `translate(${f2(view.x)},${f2(view.y)}) scale(${view.k})`);
        for (const [dot, radius] of dots) dot.setAttribute('r', radius / view.k);
        svg.style.setProperty('--bd-map-k', String(1 / view.k));
      };
      apply();
      const at = (e) => { const r = svg.getBoundingClientRect(); return { x: ((e.clientX - r.left) * W) / r.width, y: ((e.clientY - r.top) * H) / r.height }; };
      svg.addEventListener('wheel', (e) => {
        // The wheel over a map is the map's: the board beneath does not zoom.
        e.preventDefault();
        e.stopPropagation();
        const q = at(e), k = Math.max(home.k * 0.6, Math.min(home.k * 4000, view.k * Math.exp(-e.deltaY * 0.0016)));
        view = { k, x: q.x - (q.x - view.x) * (k / view.k), y: q.y - (q.y - view.y) * (k / view.k) };
        apply();
      }, { passive: false });
      let drag = null;
      svg.addEventListener('pointerdown', (e) => { if (e.button === 0) { drag = { q: at(e), view }; svg.setPointerCapture(e.pointerId); } });
      svg.addEventListener('pointermove', (e) => { if (drag) { const q = at(e); view = { k: view.k, x: drag.view.x + q.x - drag.q.x, y: drag.view.y + q.y - drag.q.y }; apply(); } });
      svg.addEventListener('pointerup', () => { drag = null; });
      svg.addEventListener('dblclick', () => { view = { ...home }; apply(); });

      // The key: what the colours say.
      if (colour) {
        const key = h('div', { class: 'bd-map-key' }, h('span', { class: 'bd-map-key-name' }, p.value));
        if (colour.kind === 'number') key.append(h('span', {}, short(colour.lo)), h('i', { class: 'ramp', style: `background:${colour.gradient}` }), h('span', {}, short(colour.hi)), ...(colour.log ? [h('span', { class: 'bd-map-key-note' }, 'by logarithms')] : []));   // a node's append would write a null as the word
        else {
          for (const k of colour.kinds.slice(0, 10)) key.append(h('span', { class: 'kind' }, h('i', { style: `background:${colour.of(k)}` }), k));
          if (colour.kinds.length > 10) key.append(h('span', { class: 'bd-map-key-note' }, `and ${colour.kinds.length - 10} more, their colours used again`));
        }
        body.append(key);
      }
      const drawn = points ? `${rows.length.toLocaleString('en-GB')} places${rows.length < t.rows ? ` of ${t.rows.toLocaleString('en-GB')} rows` : ''}` : `${[...byCode.keys()].length} economies`;
      body.append(h('div', { class: 'bd-map-note' }, drawn, t.source ? ` · ${t.source}` : '', failure ? ` · the layer could not be read: ${failure}` : ''));
    })();
  },
};

// ---- a module ---------------------------------------------------------------------------------------

const describe = (ctx) => (ctx.props.module ? (ctx.services.modules || []).find((m) => m.ref === ctx.props.module) : null);

export async function runModule(ctx) {
  if (!ctx.services.run) { ctx.say('This work cannot run modules.'); return; }
  if (ctx.state?.running) return;
  const p = ctx.props, before = ctx.out || {};
  ctx.setState({ running: true, progress: 0, text: '' });
  const started = Date.now();
  try {
    const results = await ctx.services.run({ module: p.module || null, code: p.module ? null : p.code, params: p.params || {}, board: ctx.given() },
      (progress, text) => ctx.setState({ running: true, progress, text }));
    ctx.setState(null);
    ctx.setOut({ ran: new Date().toISOString().replace(/\.\d+Z$/, 'Z'), seconds: Math.round((Date.now() - started) / 100) / 10,
      tables: results.filter((r) => r.kind === 'table').map(({ kind, ...t }) => t), notes: results.filter((r) => r.kind === 'note').map((r) => String(r.text)),
      figures: results.filter((r) => r.kind === 'figure').map(({ kind, ...f }) => f) });
  } catch (error) {
    ctx.setState(null);
    // What an earlier run answered stays; the failure is said beside it.
    ctx.setOut({ ...before, error: error.message, failed: new Date().toISOString().replace(/\.\d+Z$/, 'Z') });
  }
}

const module = {
  label: 'Module', about: 'A module run on the board: it is given the board\'s tables, quantities and notes, and answers tables and notes.', size: { w: 400, h: 200 }, height: 'grow',
  props: { module: '', code: '', title: '', params: {} },
  form(ctx) {
    const d = describe(ctx);
    if (ctx.props.module) return (d?.params || []).map((q) => ({ ...q, group: 'params', options: q.options?.map((o) => (typeof o === 'object' ? [o.value, o.label ?? o.value] : [o, o])) }));
    return [{ name: 'title', label: 'Title', kind: 'text' }, { name: 'code', label: 'Python', kind: 'code', help: 'def run(board, api): return [api.table(name, {column: values}), api.note(words)]' }];
  },
  signature: (ctx) => `${ctx.state ? `${ctx.state.progress}:${ctx.state.text}` : ''}/${ctx.out?.ran || ''}/${ctx.out?.failed || ''}/${describe(ctx) ? 'known' : 'unknown'}/${ctx.services.run ? 1 : 0}`,
  render(body, ctx) {
    const p = ctx.props, d = describe(ctx), out = ctx.out || {};
    body.append(h('div', { class: 'bd-module-title' }, p.title || d?.title || (p.module ? p.module : 'A script')));
    if (p.module && !d) body.append(h('div', { class: 'bd-hint wrong' }, `The module '${p.module}' is not installed in this work. What it answered is kept.`));
    if (d?.description) body.append(h('div', { class: 'bd-module-about' }, d.description));
    const given = (d?.params || []).map((q) => `${q.label || q.name}: ${p.params?.[q.name] ?? q.default}`);
    if (given.length) body.append(h('div', { class: 'bd-module-params' }, given.join(' · ')));
    if (!p.module) body.append(h('pre', { class: 'bd-module-code' }, (p.code || '').trim() || 'def run(board, api):\n    return [api.note("…")]'));
    const running = ctx.state?.running;
    const key = h('button', { type: 'button', class: 'bd-run', 'data-live': true, disabled: running || (p.module && !d) || !ctx.services.run, onclick: () => runModule(ctx) }, running ? 'Running…' : out.ran ? 'Run again' : 'Run');
    const state = running ? `${Math.round((ctx.state.progress || 0) * 100)}%${ctx.state.text ? ` · ${ctx.state.text}` : ''}`
      : out.ran ? `ran ${out.ran.replace('T', ' ').replace('Z', ' UTC')}, ${out.seconds} s` : ctx.services.run ? 'not run yet' : 'this work cannot run modules';
    body.append(h('div', { class: 'bd-module-run' }, key, h('span', { class: 'bd-module-state' }, state),
      running ? h('i', { class: 'bd-progress-bar' }, h('b', { style: `width:${Math.round((ctx.state.progress || 0) * 100)}%` })) : null));
    if (out.error) body.append(h('pre', { class: 'bd-module-error' }, out.error));
    for (const n of out.notes || []) body.append(h('div', { class: 'bd-module-note' }, n));
    // A figure the module drew: shown as a picture (an SVG as a picture runs no script).
    for (const f of out.figures || []) {
      const src = f.format === 'svg' ? `data:image/svg+xml;charset=utf-8,${encodeURIComponent(f.data)}` : `data:image/png;base64,${f.data}`;
      body.append(h('figure', { class: 'bd-module-figure' }, h('img', { src, alt: f.title || 'a figure the module drew', draggable: 'false' }), f.title ? h('figcaption', {}, f.title) : null));
    }
    // The tables it answered: a press on one lays it on the board, as a map where its rows have places, else as a chart.
    if (out.tables?.length) {
      body.append(h('div', { class: 'bd-module-tables' }, out.tables.map((t) => {
        const placed = findColumn(t, 'lat', 'latitude') || findColumn(t, 'iso3');
        const value = columnNames(t, 'number').find((c) => !['lat', 'lon', 'latitude', 'longitude', 'year'].includes(c.toLowerCase())) || '';
        return h('button', { type: 'button', class: 'bd-table-chip', 'data-live': true, title: `${t.title || t.name}: draw it beside this card, as a ${placed ? 'map' : 'chart'}`,
          onclick: () => ctx.addBeside('card', placed ? { card: 'map', props: { table: t.name, value } } : { card: 'chart', props: { table: t.name } }) },
        h('b', {}, t.name), ` ${t.rows.toLocaleString('en-GB')} × ${t.columns.length}`, h('i', {}, placed ? ' map →' : ' chart →'));
      })));
    }
  },
};

export const CARDS = { chart, map, module };
