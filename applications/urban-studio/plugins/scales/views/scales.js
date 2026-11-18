// Cities across scales (ADR 0128): the city as the page, at the scale chosen. Its blocks drawn as they lie, toned by an index read at
// the block, at the area each block is in, or for the whole city, with a ribbon of census years; the indexes the records give, each with
// how it is carried from one scale to the next; the models across scales drawn as their levels, the quantities on each, what carries a
// quantity between levels and what the model still needs declared; and the urban-science domain's catalogue of indexes, opened in place.

import { lineChart } from '/visualization/js/dynamics/charts.js';
import { coloursOf, legendSvg, scaleOf } from '/visualization/js/dynamics/fields.js';
import { modelDiagram, unitsMap } from '/visualization/js/dynamics/scales.js';

const enc = encodeURIComponent;
const S = { project: null, cases: [], study: {}, mode: 'indexes', data: null, index: 0, level: 'block', year: null, model: 'growth:homes',
  described: {}, catalogue: null, filter: '', open: null, wrong: '', busy: '' };
const base = (ctx) => `/api/projects/${enc(ctx.name)}/scales`;
const night = () => document.documentElement.dataset.theme === 'night';
const fmt = (x) => (x == null || !Number.isFinite(x) ? '–' : Math.abs(x) < 1 ? x.toFixed(3) : Math.abs(x) < 100 ? x.toFixed(2) : Math.round(x).toLocaleString('en-GB'));

export function view() {
  return { id: 'scales', label: 'Across scales', draw, heard: (e, ctx) => { if (/scales/.test(e.what) && document.querySelector('.tab.on')?.dataset.tab === 'scales') { S.project = null; ctx.show('scales'); } } };
}

async function ensure(ctx) {
  if (S.project === ctx.name) return;
  Object.assign(S, { project: ctx.name, data: null, described: {}, wrong: '' });
  const r = await ctx.api(base(ctx));
  S.cases = r.cases || [], S.study = r.study || {};
  Object.assign(S, { index: S.study.index ?? 0, level: S.study.level || 'block', model: S.study.model || 'growth:homes' });
  const c = S.study.case || ctx.project?.scales?.case || S.cases[0]?.name;
  if (c) {
    S.busy = 'Computing every index at every level…';
    S.data = await ctx.api(`${base(ctx)}/indexes`, { method: 'POST', body: { case: c } });
    S.year = S.study.year || S.data.years[S.data.years.length - 1];
    S.busy = '';
  }
}

async function keep(ctx) {
  try { await ctx.api(base(ctx), { method: 'PUT', body: { study: { case: S.data?.case, index: S.index, level: S.level, year: S.year, model: S.model } } }); } catch { /* a page that may only look keeps nothing */ }
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('ms-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  try { await ensure(ctx); } catch (error) { S.wrong = error.message; }
  ctx.here('the city across scales');
  const redraw = () => draw(pane, ctx);
  const mode = (m, words) => el('button', { type: 'button', class: `ms-mode${S.mode === m ? ' on' : ''}`, 'aria-pressed': String(S.mode === m), onclick: () => { S.mode = m; S.open = null; redraw(); } }, words);
  const head = el('header', { class: 'ms-head' },
    el('div', { class: 'ms-title-row' }, el('h2', { class: 'ms-title' }, S.data ? S.data.title : 'Cities across scales'),
      el('span', { class: 'ms-modes', role: 'group' }, mode('indexes', 'Indexes'), mode('models', 'Models'), mode('catalogue', 'The catalogue'))),
    S.wrong ? el('p', { class: 'wrong' }, S.wrong) : null, S.busy ? el('p', { class: 'note' }, S.busy) : null);
  if (!S.data && S.mode !== 'catalogue')
    return fill(pane, head, el('p', { class: 'note' }, 'No city across scales is in the data library yet: python data/products/city_scales.py'));
  const body = S.mode === 'indexes' ? indexesView(ctx, redraw) : S.mode === 'models' ? await modelsView(ctx, redraw) : await catalogueView(ctx, redraw);
  fill(pane, head, body);
}

// ---- indexes, at the scale chosen ------------------------------------------------------------------------------------------------

function indexesView(ctx, redraw) {
  const { el } = ctx;
  const D = S.data, ix = D.indexes[S.index], k = D.years.indexOf(S.year);
  const [fine, area, city] = D.levels;
  const level = ix.levels[S.level] ? S.level : Object.keys(ix.levels)[0];
  const areaIndex = Object.fromEntries(area.units.map((a, i) => [a, i]));
  const valueOf = (b) => (level === fine.name ? ix.levels[level][k][b] : level === area.name ? ix.levels[level][k][areaIndex[fine.parent[b]]] : ix.levels[level][k][0]);
  const values = fine.units.map((_, b) => valueOf(b));
  const all = Object.values(ix.levels).flatMap((rows) => rows.flat()).filter(Number.isFinite);
  const scale = scaleOf(level === fine.name ? values : all, { log: false });
  const colours = coloursOf(values, scale, { reverse: night() });
  const map = el('div', { class: 'ms-map-box' });
  map.innerHTML = unitsMap({ rings: D.rings, ids: fine.units, colours, groups: level === fine.name ? null : fine.parent,
    titles: fine.units.map((u, b) => `Block ${u}, ${fine.parent[b]}: ${fmt(values[b])}`), width: 620, what: `${ix.label} in ${S.year}, by ${level}` });
  const legend = el('div', { class: 'ms-legend' });
  legend.innerHTML = legendSvg({ scale, reverse: night(), label: `${ix.label}, ${S.year}, read at ${level === fine.name ? 'each block' : level === area.name ? 'each block\'s area' : 'the whole city'}`, width: 340 });
  const levelButton = (lv) => el('button', { type: 'button', class: `ms-level-b${level === lv ? ' on' : ''}`, disabled: !ix.levels[lv], onclick: () => { S.level = lv; keep(ctx); redraw(); } }, lv);
  const ribbon = el('div', { class: 'ms-ribbon' }, D.years.map((y) => el('button', { type: 'button', class: `ms-year${y === S.year ? ' on' : ''}`, onclick: () => { S.year = y; keep(ctx); redraw(); } }, String(y).slice(2))));
  const list = el('ol', { class: 'ms-indexes' }, D.indexes.map((x, i) => el('li', {}, el('button', { type: 'button', class: `ms-index${i === S.index ? ' on' : ''}`, onclick: () => { S.index = i; if (!x.levels[S.level]) S.level = Object.keys(x.levels)[0]; keep(ctx); redraw(); } }, x.label))));
  const series = [{ name: 'the city', x: D.years, y: (ix.levels[city.name] || []).map((r) => r[0]), width: 2.2 }];
  area.units.forEach((a, i) => { if (ix.levels[area.name]) series.push({ name: a, x: D.years, y: ix.levels[area.name].map((r) => r[i]), width: 0.8 }); });
  const chart = el('div', { class: 'ms-chart' });
  chart.innerHTML = lineChart({ series, width: 380, height: 220, xLabel: 'census year', yLabel: ix.label, what: `${ix.label}: the city and its areas over the years` });
  const card = el('aside', { class: 'ms-card' },
    el('p', { class: 'ms-kicker' }, ix.catalogue ? `In the catalogue as ${ix.catalogue}` : 'Not named by the atlas'),
    el('p', {}, ix.words), el('p', { class: 'small' }, `Carried up the levels as: ${ix.aggregation}.`),
    el('p', { class: 'ms-city' }, `The city, ${S.year}: ${fmt(ix.levels[city.name]?.[k]?.[0])}`), chart);
  return el('div', { class: 'ms-stage' },
    el('div', { class: 'ms-left' }, el('div', { class: 'ms-levels', role: 'group', 'aria-label': 'the scale read' }, D.levels.map((lv) => levelButton(lv.name))), map, legend, ribbon,
      el('p', { class: 'small' }, `${D.credit}. Floor space the census withheld enters as its estimate.`)),
    el('div', { class: 'ms-right' }, list, card));
}

// ---- models across scales ----------------------------------------------------------------------------------------------------------

async function modelsView(ctx, redraw) {
  const { el } = ctx;
  const [model, use] = S.model.split(':');
  const key = S.model;
  if (!S.described[key]) {
    try {
      S.described[key] = await ctx.api(`${base(ctx)}/describe`, { method: 'POST', body: { case: S.data.case, model, use, undeclared: model === 'vacancy' } });
    } catch (error) { return el('p', { class: 'wrong' }, error.message); }
  }
  const d = S.described[key].described;
  const pick = (m, words) => el('button', { type: 'button', class: `ms-model${S.model === m ? ' on' : ''}`, onclick: () => { S.model = m; keep(ctx); redraw(); } }, words);
  const diagram = el('div', { class: 'ms-diagram-box' });
  diagram.innerHTML = modelDiagram(d, { width: 760, what: `the model ${model}` });
  const missing = d.missing.length
    ? el('div', { class: 'ms-missing' }, el('h3', {}, 'What the records do not hold, to be declared'),
      el('ul', {}, d.missing.map((m) => el('li', {}, `${m.quantity}${m.level ? ` at each ${m.level} (${m.units} of ${m.of})` : ''}: ${m.what}`))),
      el('p', { class: 'small' }, 'A run refuses until each is given from a record, estimated, or declared with a range; the tool The vacancy cycle declares the rents with the range its form asks for.'))
    : el('p', { class: 'small' }, 'Every quantity this model needs is given by the records or estimated.');
  const params = el('table', { class: 'ms-params' }, el('tr', {}, el('th', {}, 'unknown'), el('th', {}, 'start'), el('th', {}, 'bounds'), el('th', {}, 'what it says')),
    d.parameters.map((p) => el('tr', {}, el('td', {}, p.name), el('td', {}, fmt(p.value)), el('td', {}, `${fmt(p.lower)} to ${fmt(p.upper)}`), el('td', {}, p.unit))));
  return el('div', { class: 'ms-models' },
    el('div', { class: 'ms-choose' }, pick('growth:homes', 'Where homes go'), pick('growth:office', 'Where offices go'), pick('vacancy:', 'The vacancy cycle')),
    diagram,
    el('p', { class: 'small' }, 'A disc is a state advanced by its law, a triangle a driver given over time, a square a fixed value, a diamond a quantity derived by an operator; arrows carry a quantity up (a sum, a reduction) or down (a parent\'s value) the levels; an operator in italics works among siblings or neighbours.'),
    missing, el('h3', {}, 'Its unknowns'), params,
    el('p', { class: 'small' }, 'The tools Where growth goes and The vacancy cycle fit the unknowns to the records on the years before those held aside, say which the records do not identify, judge the model on the years held aside at every level, and run it over draws of what is declared.'));
}

// ---- the catalogue ---------------------------------------------------------------------------------------------------------------------

async function catalogueView(ctx, redraw) {
  const { el } = ctx;
  if (!S.catalogue) {
    try { S.catalogue = await ctx.api(`${base(ctx)}/catalogue`); } catch (error) { return el('p', { class: 'wrong' }, error.message); }
  }
  const C = S.catalogue;
  const count = (k) => el('p', { class: 'ms-count-row' }, el('span', { class: 'ms-count-k' }, k), Object.entries(C.counts[k]).sort((a, b) => b[1] - a[1]).map(([v, n]) => el('span', { class: 'ms-chip' }, `${v || '—'} ${n}`)));
  const want = S.filter.toLowerCase();
  const rows = C.indexes.filter((r) => !want || `${r.id} ${r.name} ${r.section} ${r.kind}`.toLowerCase().includes(want)).slice(0, 200);
  const search = el('input', { type: 'search', class: 'ms-search', placeholder: 'Look for an index…', value: S.filter, oninput: (e) => { S.filter = e.target.value; clearTimeout(S.t); S.t = setTimeout(redraw, 250); } });
  return el('div', { class: 'ms-catalogue' },
    el('p', {}, `${C.indexes.length.toLocaleString('en-GB')} indexes, ${C.counts.computed} of them computed by the infrastructure already; ${C.status}.`),
    count('class'), count('aggregation'), count('estimability'), count('role'), search,
    el('ul', { class: 'ms-rows' }, rows.map((r) => el('li', { class: `ms-row${S.open === r.id ? ' open' : ''}` },
      el('button', { type: 'button', class: 'ms-row-b', onclick: () => { S.open = S.open === r.id ? null : r.id; redraw(); } },
        el('span', { class: 'ms-row-name' }, r.name), el('span', { class: 'ms-row-meta' }, `${r.class}, ${r.kind}; carried by ${r.aggregation || '—'}; estimable ${r.estimability || '—'}`)),
      S.open === r.id ? el('div', { class: 'ms-row-more' }, el('p', {}, r.definition), el('p', { class: 'small' }, `Source: ${r.source || '—'}. In the atlas under ${r.section}.`),
        el('p', { class: 'small' }, `Where not observed: ${r.estimable_from || '—'}.`), r.law ? el('p', { class: 'small' }, `In a model: ${r.law}`) : null,
        el('p', { class: 'small' }, r.computed_by ? `Computed by ${r.computed_by}.` : 'Not computed by the infrastructure yet.')) : null))),
    rows.length === 200 ? el('p', { class: 'small' }, 'The first 200 shown; narrow the search for the rest.') : null);
}
