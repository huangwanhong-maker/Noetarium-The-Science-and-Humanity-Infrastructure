// Agora: a market on the map of a city. The page draws the city's streets, its blocks (who could buy) and its places of business
// (what is offered) on the city model's plane, asks the engine for the market under the assumptions set at the left, and answers
// four questions on the map: who sells and how much of the spend buys; whose custom an outlet's is and where it would go; what a
// new outlet would sell, from whom, and where to open; how large the market is. A finding is sent to a board (the visualization
// layer's, as Tabula's), where it is written about.
//
// The kernel of the market is PRESUMED (Huff's model on minutes walked): every number at the left is an assumption, and says so.
//
//   ?view=market|outlet|open|size    begin with a question
//   ?theme=night                     by night

import * as B from '/visualization/js/board/index.js';
import { createPlane } from '/visualization/js/map/plane.js';
import { colourMap } from '/visualization/styles/palettes.js';
import { cssColour } from '/visualization/styles/colour.js';

const NS = 'http://www.w3.org/2000/svg';
const $ = (id) => document.getElementById(id);
const el = (tag, attrs = {}, ...kids) => {
  const node = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (v === null || v === undefined || v === false) continue;
    if (k.startsWith('on')) node.addEventListener(k.slice(2), v);
    else node.setAttribute(k, v === true ? '' : v);
  }
  for (const kid of kids.flat(Infinity)) if (kid !== null && kid !== undefined && kid !== false) node.append(kid instanceof Node ? kid : document.createTextNode(String(kid)));
  return node;
};
const svgEl = (tag, attrs = {}) => { const n = document.createElementNS(NS, tag); for (const [k, v] of Object.entries(attrs)) if (v !== null && v !== undefined) n.setAttribute(k, v); return n; };
const query = new URLSearchParams(location.search);

// ---- what the page has to say, and the theme ---------------------------------------------------------------------------
let saying = null;
function say(words) {
  const line = $('say');
  line.textContent = words;
  line.classList.add('shown');
  clearTimeout(saying);
  saying = setTimeout(() => line.classList.remove('shown'), 3400);
}
function setTheme(theme) {
  document.documentElement.dataset.theme = theme;
  $('theme').textContent = theme === 'night' ? 'Day' : 'Night';
  try { localStorage.setItem('agora.theme', theme); } catch { /* kept for the visit */ }
}
let remembered = null;
try { remembered = localStorage.getItem('agora.theme'); } catch { /* none */ }
setTheme(query.get('theme') || remembered || 'day');
$('theme').addEventListener('click', () => { setTheme(document.documentElement.dataset.theme === 'night' ? 'day' : 'night'); paint(); });

async function ask(op, body = {}) {
  let response;
  try {
    response = await fetch(`/api/${op}`, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) });
  } catch { throw new Error('the server did not answer'); }
  const answer = await response.json().catch(() => null);
  if (!response.ok) throw new Error(answer?.message || `the server answered ${response.status}`);
  return answer;
}
const getJson = async (path) => { const r = await fetch(path); if (!r.ok) throw new Error(`the data library has no ${path}`); return r.json(); };

// ---- numbers as they are read --------------------------------------------------------------------------------------------
const money = (v, unit = state.product?.currency || '') => {
  const a = Math.abs(v);
  for (const [from, by, suffix] of [[1e9, 1e9, ' B'], [1e6, 1e6, ' M'], [1e4, 1e3, ' k']]) if (a >= from) return `${unit} ${String(Number((v / by).toPrecision(3)))}${suffix}`.trim();
  return `${unit} ${Math.round(v).toLocaleString('en-GB')}`.trim();
};
const count = (v) => Math.round(v).toLocaleString('en-GB');
const percent = (v, digits = 0) => `${(100 * v).toFixed(digits)}%`;
const minutes = (v) => `${v.toFixed(1)} min`;

// ---- the state ---------------------------------------------------------------------------------------------------------------
const DEFAULTS = { kind: '4511', residents_per_dwelling: 2, workers_per_100m2: 5, spend_resident: 30, spend_worker: 25, weeks: 48, decay: 'power', beta: 2, limit: 15, outside: 5 };
const RANGES = { residents_per_dwelling: [1.8, 2.3], workers_per_100m2: [4, 6.7], spend_resident: [20, 45], spend_worker: [15, 40], weeks: [46, 50] };
const state = {
  city: 'melbourne', product: null, params: { ...DEFAULTS }, ranges: structuredClone(RANGES), market: null, view: ['market', 'outlet', 'open', 'size'].includes(query.get('view')) ? query.get('view') : 'market',
  layer: 'unserved', offer: null, offerAnswer: null, seller: -1, entry: null, sites: null, size: null, busy: 0,
};
let plane = null, blockPaths = [], placeDots = [];

const marketBody = () => ({ city: state.city, market: state.params });

// ---- the map ---------------------------------------------------------------------------------------------------------------
const STREETS = new Set(['primary', 'secondary', 'tertiary', 'trunk', 'residential', 'unclassified', 'pedestrian', 'living_street', 'primary_link', 'secondary_link', 'tertiary_link', 'trunk_link']);

function drawGround(model) {
  let streets = '', lanes = '';
  for (const road of model.roads) {
    if (road.kind === 'motorway' || road.kind === 'motorway_link' || road.line.length < 2) continue;
    const d = plane.path(road.line);
    if (STREETS.has(road.kind)) streets += d;
    else lanes += d;
  }
  plane.layer('lanes').append(svgEl('path', { d: lanes }));
  plane.layer('blocks');
  plane.layer('streets').append(svgEl('path', { d: streets }));
  const blocks = plane.layer('blocks');
  blockPaths = state.product.blocks.map((b, i) => { const p = svgEl('path', { d: plane.path(b.ring, true), 'data-block': i }); blocks.append(p); return p; });
  plane.layer('places');
  plane.layer('marks');
}

const MAPS = { brings: colourMap('gold'), unserved: colourMap('sea'), trade: colourMap('orchid') };
const tint = (map, u) => cssColour(map.colour(Math.min(1, Math.max(0, u))));

// The blocks by the layer in view, the outlets by what they sell.
function paint() {
  if (!state.market || !plane) return;
  const m = state.market, blocks = state.product.blocks, legend = $('legend');
  let values = null, map = null, words = '', hi = 0;
  const area = (i) => Math.max(1, blocks[i].area);
  if (state.view === 'outlet' && state.offerAnswer) {
    values = state.offerAnswer.trade.map((v, i) => v / area(i)); map = MAPS.trade; words = 'The custom of this outlet, by block, per hectare';
  } else if (state.view === 'open' && state.entry) {
    values = state.entry.trade.map((v, i) => v / area(i)); map = MAPS.trade; words = 'The custom the new outlet would draw, by block, per hectare';
  } else if (state.layer === 'brings') {
    values = m.block.brings.map((v, i) => v / area(i)); map = MAPS.brings; words = 'What each block brings a year, per hectare';
  } else {
    values = m.block.brings.map((v, i) => (v > 0 ? m.block.unserved[i] / v : null)); map = MAPS.unserved; words = 'The part of each block\'s spend that buys nothing';
  }
  const known = values.filter((v) => v !== null && Number.isFinite(v));
  hi = state.layer === 'unserved' && map === MAPS.unserved ? 1 : Math.max(...known, 1e-9);
  const core = new Map((state.view === 'outlet' && state.offerAnswer ? state.offerAnswer.core.blocks : []).map(([b, part]) => [b, part]));
  blockPaths.forEach((p, i) => {
    const v = values[i];
    p.style.fill = v === null || !(v > 0) ? '' : tint(map, Math.sqrt(v / hi));
    p.classList.toggle('core', (core.get(i) || 0) > 0.5);
  });
  // Outlets: their radius by what they sell, redrawn with the zoom so that they keep their size on the screen.
  const most = Math.max(...m.offer.demand, 1), px = plane.px(), own = state.seller;
  placeDots.forEach((dot, o) => {
    dot.setAttribute('r', ((2 + 5.5 * Math.sqrt(m.offer.demand[o] / most)) * px).toFixed(2));
    dot.setAttribute('class', o === state.offer && state.view === 'outlet' ? 'picked' : own >= 0 && m.offer.seller[o] === own ? 'own' : state.view === 'outlet' && state.offerAnswer ? 'dim' : '');
  });
  for (const mark of plane.layer('marks').querySelectorAll('[data-r]')) mark.setAttribute('r', (Number(mark.dataset.r) * px).toFixed(2));
  for (const mark of plane.layer('marks').querySelectorAll('text')) mark.setAttribute('font-size', (12 * px).toFixed(2));
  legend.replaceChildren(...[
    state.view === 'market' ? el('div', { class: 'layers' },
      el('button', { class: `key${state.layer === 'unserved' ? ' on' : ''}`, type: 'button', onclick: () => { state.layer = 'unserved'; paint(); } }, 'Buys nothing'),
      el('button', { class: `key${state.layer === 'brings' ? ' on' : ''}`, type: 'button', onclick: () => { state.layer = 'brings'; paint(); } }, 'What it brings')) : null,
    el('div', {}, words),
    el('div', {}, map === MAPS.unserved ? '0%' : '0', el('i', { class: 'ramp', style: `background:${map.gradient()}` }), map === MAPS.unserved ? '100%' : `${money(hi)} a hectare`),
    el('small', {}, map === MAPS.unserved ? 'The colour runs with the square root of the share. Outlets beyond the map are not counted: its edges read as less served than they are.'
      : 'The colour runs with the square root. Dots: outlets, by what they sell.')].filter(Boolean));
}

function drawPlaces() {
  const layer = plane.layer('places'), m = state.market, places = state.product.places;
  layer.replaceChildren();
  placeDots = m.offer.place.map((p, o) => {
    const [x, y] = plane.at(places.x[p], places.y[p]);
    const dot = svgEl('circle', { cx: x, cy: y, 'data-offer': o });
    dot.append(svgEl('title', {}));
    dot.firstChild.textContent = places.name[p];
    layer.append(dot);
    return dot;
  });
}

function clearMarks() { plane.layer('marks').replaceChildren(); }

// ---- what is assumed ---------------------------------------------------------------------------------------------------------
let recompute = null;
function assumptions() {
  const p = state.params;
  const field = (name, label, step, wide = false) => el('label', { class: `row${wide ? ' wide' : ''}` }, el('span', {}, label),
    el('input', { type: 'number', step, min: '0', value: p[name], 'data-param': name, onchange: (e) => { const v = Number(e.target.value); if (v >= 0 && Number.isFinite(v)) { p[name] = v; later(); } } }));
  $('assume').replaceChildren(
    el('h3', {}, 'Who could buy'),
    field('residents_per_dwelling', 'Residents a dwelling', '0.1'),
    field('workers_per_100m2', 'Workers per 100 m² of office', '0.5'),
    el('p', { class: 'tip' }, `The census of ${state.product.year} counts dwellings and floor space, not people.`),
    el('h3', {}, 'What each brings'),
    field('spend_resident', 'A resident, a week', '1'),
    field('spend_worker', 'A worker, a week', '1'),
    field('weeks', 'Weeks of a year', '1'),
    el('h3', {}, 'How they choose'),
    el('label', { class: 'row wide' }, el('span', {}, 'Nearer is better'),
      el('select', { 'data-param': 'decay', onchange: (e) => { p.decay = e.target.value; p.beta = p.decay === 'power' ? 2 : 0.25; assumptions(); later(); } },
        el('option', { value: 'power', selected: p.decay === 'power' }, 'as a power'), el('option', { value: 'exponential', selected: p.decay === 'exponential' }, 'exponentially'))),
    field('beta', p.decay === 'power' ? 'Its exponent, β' : 'Its rate a minute, β', '0.05'),
    field('limit', 'Reach, minutes on foot', '1'),
    field('outside', 'Not buying, as an outlet this many minutes away', '1'),
    el('p', { class: 'tip' }, 'Huff\'s model on minutes walked along the streets, with the walk within a block added. Every outlet is taken to attract alike: the census gives none a size.'),
    el('p', { class: 'tip' }, 'The market here is presumed, not observed: every number above is an assumption to replace with your own.'));
}
function later() { clearTimeout(recompute); recompute = setTimeout(compute, 350); }

// ---- the market ---------------------------------------------------------------------------------------------------------------
function busy(on) {
  state.busy += on ? 1 : -1;
  $('state').textContent = state.busy > 0 ? 'Reckoning…' : '';
  $('state').classList.remove('wrong');
}
function failed(error) { $('state').textContent = error.message; $('state').classList.add('wrong'); }

async function compute() {
  busy(true);
  try {
    state.market = await ask('market', marketBody());
    state.offer = null; state.offerAnswer = null; state.entry = null; state.sites = null; state.size = null;
    if (state.seller >= state.market.sellers) state.seller = -1;
    clearMarks();
    drawPlaces();
    busy(false);
    await show(state.view);
  } catch (error) { state.busy = 0; failed(error); }
}

const sellersByDemand = () => state.market.seller.demand.map((d, s) => s).sort((a, b) => state.market.seller.demand[b] - state.market.seller.demand[a]);
const placeOf = (offer) => state.market.offer.place[offer];
const nameOf = (offer) => state.product.places.name[placeOf(offer)] || '(unnamed)';
const kindName = () => state.product.kinds.find((k) => k.code === state.params.kind)?.name || state.params.kind;

function answerMarket() {
  const m = state.market, order = sellersByDemand();
  return [
    el('h3', {}, kindName()),
    el('div', { class: 'figures' },
      el('div', { class: 'figure' }, el('b', {}, money(m.total)), el('span', {}, 'all who could buy bring, a year')),
      el('div', { class: 'figure' }, el('b', {}, percent(m.penetration)), el('span', {}, 'of it buys; the rest has nothing near enough')),
      el('div', { class: 'figure' }, el('b', {}, count(m.offers)), el('span', {}, `outlets of ${count(m.sellers)} sellers`)),
      el('div', { class: 'figure' }, el('b', {}, minutes(m.minutes)), el('span', {}, 'walked, on average, by the spend that buys'))),
    el('p', { class: 'tip' }, `${count(m.buyers.residents)} residents and ${count(m.buyers.workers)} workers, by the two assumptions at the left.`,
      m.buyers.withheld_dwellings + m.buyers.withheld_office ? ` The census states no dwellings for ${m.buyers.withheld_dwellings} blocks and no offices for ${m.buyers.withheld_office} (none there, or withheld); they count as none.` : ''),
    el('h3', {}, 'Who sells'),
    el('div', { class: 'figures' },
      el('div', { class: 'figure' }, el('b', {}, m.structure.hhi.toFixed(4)), el('span', {}, 'Herfindahl–Hirschman index')),
      el('div', { class: 'figure' }, el('b', {}, m.structure.equivalent.toFixed(0)), el('span', {}, 'equal sellers would be as concentrated')),
      el('div', { class: 'figure' }, el('b', {}, percent(m.structure.cr4, 1)), el('span', {}, 'held by the four largest'))),
    el('div', { class: 'list' }, order.slice(0, 12).map((s) => el('button', { type: 'button', class: s === state.seller ? 'on' : '', 'data-seller': s,
      onclick: () => { state.seller = state.seller === s ? -1 : s; draw(); } },
      el('span', {}, m.seller.name[s]), el('i', {}, `${percent(m.seller.demand[s] / m.bought, 1)} · ${m.seller.outlets[s]}`),
      el('span', { class: 'bar' }, el('b', { style: `width:${(100 * m.seller.demand[s] / m.seller.demand[order[0]]).toFixed(1)}%` }))))),
    el('p', { class: 'tip' }, 'A seller is a trading name; its share of what is bought, and its outlets. Press one to see its outlets; press an outlet on the map to ask whose custom it is.'),
    el('div', { class: 'actions' }, el('button', { class: 'key edged', type: 'button', 'data-send': 'market', onclick: sendMarket }, 'Send to the board')),
  ];
}

// ---- one outlet ---------------------------------------------------------------------------------------------------------------
async function pickOffer(offer) {
  state.offer = offer; state.view = 'outlet';
  busy(true);
  try {
    state.offerAnswer = await ask('offer', { ...marketBody(), offer });
    busy(false);
  } catch (error) { state.busy = 0; state.offerAnswer = null; failed(error); }
  draw();
}

function answerOutlet() {
  const a = state.offerAnswer;
  if (!a) return [el('h3', {}, 'Whose custom'), el('p', {}, 'Press an outlet on the map.'), el('p', { class: 'tip' }, 'Its trade area is drawn on the blocks, a density and not a ring; the blocks outlined hold most of its custom in the fewest buyers.')];
  const places = state.product.places, p = placeOf(a.offer);
  return [
    el('h3', {}, places.name[p] || '(unnamed)'),
    el('p', { class: 'tip' }, places.address[p]),
    el('div', { class: 'figures' },
      el('div', { class: 'figure' }, el('b', {}, money(a.demand)), el('span', {}, 'it would sell a year')),
      el('div', { class: 'figure' }, el('b', {}, percent(a.share, 2)), el('span', {}, 'of what is bought')),
      el('div', { class: 'figure' }, el('b', {}, minutes(a.reach.half)), el('span', {}, 'holds half its custom')),
      el('div', { class: 'figure' }, el('b', {}, minutes(a.reach.most)), el('span', {}, 'holds four fifths'))),
    el('p', { class: 'tip' }, `The blocks outlined hold ${percent(a.core.share)} of its custom in the fewest buyers: ${count(a.core.buyers)} of them. Its custom walks ${minutes(a.reach.mean)} on average.`),
    el('h3', {}, 'If it closed'),
    el('p', {}, `${percent(a.diversion.outside, 1)} of its custom would buy nothing`, a.diversion.own > 0 ? `, ${percent(a.diversion.own, 1)} would stay with its seller's other outlets` : '', '. The rest would go to:'),
    el('div', { class: 'list' }, a.diversion.to.map((d) => el('button', { type: 'button', onclick: () => pickOffer(d.offer) },
      el('span', {}, nameOf(d.offer)), el('i', {}, percent(d.part, 1)), el('small', {}, `shares ${percent(d.overlap)} of its buyers`)))),
    el('p', { class: 'tip' }, 'Diversion follows who shares buyers with whom, not who is largest.'),
    el('div', { class: 'actions' }, el('button', { class: 'key edged', type: 'button', 'data-send': 'outlet', onclick: sendOutlet }, 'Send to the board')),
  ];
}

function markOutlet() {
  clearMarks();
  const a = state.offerAnswer;
  if (!a) return;
  const places = state.product.places, marks = plane.layer('marks'), from = placeOf(a.offer);
  const most = Math.max(...a.diversion.to.map((d) => d.part), 1e-9);
  for (const d of a.diversion.to) {
    const to = placeOf(d.offer);
    const [x1, y1] = plane.at(places.x[from], places.y[from]), [x2, y2] = plane.at(places.x[to], places.y[to]);
    marks.append(svgEl('line', { x1, y1, x2, y2, 'stroke-width': (0.8 + 4 * d.part / most).toFixed(2) }));
  }
}

// ---- a new outlet, and where to open ------------------------------------------------------------------------------------
async function enterAt(point) {
  busy(true);
  try {
    state.entry = { ...(await ask('enter', { ...marketBody(), at: point, seller: state.seller })), at: point };
    state.sites = null;
    busy(false);
  } catch (error) { state.busy = 0; state.entry = null; failed(error); }
  draw();
}
async function chooseSites(k, from) {
  busy(true);
  try {
    state.sites = { ...(await ask('sites', { ...marketBody(), k, from, seller: state.seller })), from, k };
    state.entry = null;
    busy(false);
  } catch (error) { state.busy = 0; state.sites = null; failed(error); }
  draw();
}

function answerOpen() {
  const m = state.market, order = sellersByDemand(), e = state.entry, s = state.sites;
  const who = state.seller >= 0 ? m.seller.name[state.seller] : 'a newcomer';
  const out = [
    el('h3', {}, 'Where to open'),
    el('label', { class: 'row wide' }, el('span', {}, 'For'),
      el('select', { 'data-param': 'seller', onchange: (ev) => { state.seller = Number(ev.target.value); state.entry = null; state.sites = null; draw(); } },
        el('option', { value: -1, selected: state.seller < 0 }, 'a newcomer'), order.slice(0, 30).map((i) => el('option', { value: i, selected: i === state.seller }, `${m.seller.name[i]} (${m.seller.outlets[i]})`)))),
    el('p', {}, 'Press a point on the map for an outlet there, or let the sites be chosen:'),
    el('div', { class: 'actions' },
      el('button', { class: 'key edged', type: 'button', 'data-sites': 'vacant', onclick: () => chooseSites(3, 'vacant') }, 'Three of the vacant spaces'),
      el('button', { class: 'key edged', type: 'button', 'data-sites': 'blocks', onclick: () => chooseSites(3, 'blocks') }, 'Three of the blocks')),
  ];
  if (e) out.push(
    el('h3', {}, 'An outlet here'),
    el('div', { class: 'figures' },
      el('div', { class: 'figure' }, el('b', {}, money(e.demand)), el('span', {}, 'it would sell a year')),
      el('div', { class: 'figure' }, el('b', {}, money(e.gain)), el('span', {}, `${who} would gain`))),
    el('div', { class: 'list' },
      el('div', { class: 'item' }, el('span', {}, 'from rivals'), el('i', {}, `${money(e.from_rivals)} · ${percent(e.from_rivals / e.demand)}`)),
      el('div', { class: 'item' }, el('span', {}, 'from those who bought nothing'), el('i', {}, `${money(e.from_outside)} · ${percent(e.from_outside / e.demand)}`)),
      state.seller >= 0 ? el('div', { class: 'item' }, el('span', {}, 'from its own outlets'), el('i', {}, `${money(e.cannibalised)} · ${percent(e.cannibalised / e.demand)}`)) : null),
    el('p', { class: 'tip' }, 'It would draw most from:'),
    el('div', { class: 'list' }, e.from.slice(0, 6).map((d) => el('div', { class: 'item' }, el('span', {}, nameOf(d.offer)), el('i', {}, money(d.amount))))),
    el('div', { class: 'actions' }, el('button', { class: 'key edged', type: 'button', 'data-send': 'entry', onclick: sendEntry }, 'Send to the board')));
  if (s) out.push(
    el('h3', {}, `Three sites for ${who}`),
    el('div', { class: 'list' }, s.chosen.map((c, i) => el('button', { type: 'button', onclick: () => plane.centreOn(c.x, c.y, Math.max(plane.view.k, 0.9)) }, el('span', {}, `Site ${i + 1}`), el('i', {}, `+ ${money(c.gain)}`)))),
    el('p', {}, `${who === 'a newcomer' ? 'A newcomer' : who} would sell ${money(s.after)} a year, from ${money(s.before)} now.`),
    el('p', { class: 'tip' }, `Chosen one at a time among ${count(s.candidates)} ${s.from === 'vacant' ? 'vacant spaces of the census' : 'blocks'}, each the site of greatest gain. That gains at least ${percent(s.bound)} of what the best three would, and usually all of it; rivals are taken not to respond.`),
    el('div', { class: 'actions' }, el('button', { class: 'key edged', type: 'button', 'data-send': 'sites', onclick: sendSites }, 'Send to the board')));
  return out;
}

function markOpen() {
  clearMarks();
  const marks = plane.layer('marks');
  if (state.entry) { const [x, y] = plane.at(...state.entry.at); marks.append(svgEl('circle', { class: 'site', cx: x, cy: y, 'data-r': 9 })); }
  for (const [i, c] of (state.sites?.chosen || []).entries()) {
    const [x, y] = plane.at(c.x, c.y);
    marks.append(svgEl('circle', { class: 'site', cx: x, cy: y, 'data-r': 13 }));
    const label = svgEl('text', { x, y, dx: 0, dy: 0, 'text-anchor': 'middle', 'dominant-baseline': 'central' });
    label.textContent = String(i + 1);
    marks.append(label);
  }
}

// ---- how large ---------------------------------------------------------------------------------------------------------------
const LABELS = { dwellings: 'Dwellings', residents_per_dwelling: 'Residents a dwelling', spend_resident: 'A resident\'s spend, a week', office_100m2: 'Offices, 100 m²',
  workers_per_100m2: 'Workers per 100 m²', spend_worker: 'A worker\'s spend, a week', weeks: 'Weeks of a year' };

async function sizeUp() {
  // A range holds its middle: an assumption moved at the left carries its range with it.
  for (const [name, r] of Object.entries(state.ranges)) { r[0] = Math.min(r[0], state.params[name]); r[1] = Math.max(r[1], state.params[name]); }
  busy(true);
  try {
    state.size = await ask('size', { ...marketBody(), ranges: state.ranges, seller: state.seller });
    busy(false);
  } catch (error) { state.busy = 0; state.size = null; failed(error); }
}

function answerSize() {
  const z = state.size;
  if (!z) return [el('h3', {}, 'How large'), el('p', {}, 'Reckoning…')];
  const leaves = z.nodes.filter((n) => n.kind === 'leaf');
  const W = 286, x0 = 4, x1 = W - 4, lo = z.all.low, hi = z.all.high, X = (v) => x0 + (x1 - x0) * (hi > lo ? (v - lo) / (hi - lo) : 0.5);
  const tornado = svgEl('svg', { class: 'tornado', viewBox: `0 0 ${W} ${z.swings.length * 32 + 6}`, width: W, height: z.swings.length * 32 + 6 });
  tornado.append(svgEl('line', { x1: X(z.all.mid), y1: 0, x2: X(z.all.mid), y2: z.swings.length * 32 + 6 }));
  z.swings.forEach((w, i) => {
    const y = i * 32 + 2, label = svgEl('text', { x: x0, y: y + 11 });
    label.textContent = `${LABELS[w.leaf] || w.leaf}: ${money(w.low, '')} – ${money(w.high, '')}`;
    tornado.append(label, svgEl('rect', { x: X(w.low), y: y + 17, width: Math.max(1, X(w.high) - X(w.low)), height: 8, rx: 1 }));
  });
  const out = [
    el('h3', {}, 'All who could buy'),
    el('div', { class: 'figures' },
      el('div', { class: 'figure' }, el('b', {}, money(z.all.mid)), el('span', {}, 'a year, at the middle of every assumption')),
      el('div', { class: 'figure' }, el('b', {}, `${money(z.all.low, '')} – ${money(z.all.high, '')}`), el('span', {}, 'at the low and the high of all of them'))),
    el('h3', {}, 'What it rests on'),
    ...leaves.map((n) => (n.assumed
      ? el('label', { class: 'range' }, el('span', {}, `${LABELS[n.name] || n.name}: ${n.mid}`),
        el('input', { type: 'number', step: 'any', value: n.low, 'aria-label': `${n.name}, at least`, onchange: (e) => setRange(n.name, 0, Number(e.target.value)) }),
        el('input', { type: 'number', step: 'any', value: n.high, 'aria-label': `${n.name}, at most`, onchange: (e) => setRange(n.name, 1, Number(e.target.value)) }))
      : el('div', { class: 'range' }, el('span', {}, `${LABELS[n.name] || n.name}: ${count(n.mid)}`), el('span', { style: 'grid-column: 2 / 4; text-align: right' }, n.source)))),
    el('p', { class: 'tip' }, 'Each assumption with the least and the most it is believed to be. The bounds above are exact: the size rises with every one of them.'),
    el('h3', {}, 'What could change it most'),
    tornado,
    el('p', { class: 'tip' }, 'The size with one assumption at its ends and the others at their middles; the line is the middle of all.'),
  ];
  if (z.seller) {
    const name = state.market.seller.name[z.seller.index];
    out.push(el('h3', {}, name),
      el('div', { class: 'list' },
        el('div', { class: 'item' }, el('span', {}, 'All who could buy'), el('i', {}, money(z.all.mid))),
        el('div', { class: 'item' }, el('span', {}, 'Those within reach of its outlets'), el('i', {}, `${money(z.seller.served)} · ${percent(z.seller.served / z.all.mid)}`)),
        el('div', { class: 'item' }, el('span', {}, 'What it sells'), el('i', {}, `${money(z.seller.sells)} · ${percent(z.seller.sells / z.all.mid, 2)}`))),
      el('p', { class: 'tip' }, 'The total, the serviceable and the obtained market of the seller chosen, each within the one before.'));
  } else out.push(el('p', { class: 'tip' }, 'Choose a seller under The market to see what lies within its reach and what it sells.'));
  out.push(el('div', { class: 'actions' }, el('button', { class: 'key edged', type: 'button', 'data-send': 'size', onclick: sendSize }, 'Send to the board')));
  return out;
}
async function setRange(name, end, value) {
  if (!(value >= 0)) return;
  const r = state.ranges[name] || [state.params[name], state.params[name]];
  r[end] = value;
  if (r[0] > state.params[name] || r[1] < state.params[name]) { say('A range holds its middle: the least is at most the assumption at the left, the most at least it.'); draw(); return; }
  state.ranges[name] = r;
  await sizeUp();
  draw();
}

// ---- the views ---------------------------------------------------------------------------------------------------------------
function draw() {
  for (const key of document.querySelectorAll('.views .key')) key.classList.toggle('on', key.dataset.view === state.view);
  if (!state.market) return;
  $('answer').replaceChildren(...({ market: answerMarket, outlet: answerOutlet, open: answerOpen, size: answerSize }[state.view])());
  if (state.view === 'outlet') markOutlet();
  else if (state.view === 'open') markOpen();
  else clearMarks();
  paint();
}
async function show(view) {
  state.view = view;
  if (view === 'size') { draw(); await sizeUp(); }
  draw();
}
for (const key of document.querySelectorAll('.views .key')) key.addEventListener('click', () => show(key.dataset.view));

// ---- the board ---------------------------------------------------------------------------------------------------------------
let board = null, keepTimer = null;
const boardId = () => `agora-${state.city}`;
async function openBoard() {
  $('board-sheet').hidden = false;
  $('open-board').classList.add('on');
  if (board) { board.focus(); return board; }
  let kept = null;
  try { const r = await fetch(`/api/boards/${boardId()}`); if (r.ok) kept = await r.json(); } catch { /* none kept */ }
  board = B.createBoard($('board'), {
    board: kept || B.emptyBoard({ id: boardId(), title: `Agora: ${state.product.name}`, now: new Date().toISOString().replace(/\.\d+Z$/, 'Z') }),
    services: { data: (path) => getJson(`/data/${path}`) }, today: Math.floor(Date.now() / 86400000),
    insets: () => ({ left: 96 }),
  });
  B.createToolbar(board, $('rail'));
  board.on('say', say);
  board.on('change', () => {
    $('board-state').textContent = 'Changed';
    clearTimeout(keepTimer);
    keepTimer = setTimeout(async () => {
      try {
        const r = await fetch(`/api/boards/${boardId()}`, { method: 'PUT', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(board.board) });
        $('board-state').textContent = r.ok ? 'Kept' : 'Not kept';
      } catch { $('board-state').textContent = 'Not kept'; }
    }, 900);
  });
  if (kept) board.fit();
  return board;
}
$('open-board').addEventListener('click', () => { if ($('board-sheet').hidden) openBoard(); else { $('board-sheet').hidden = true; $('open-board').classList.remove('on'); } });
$('close-board').addEventListener('click', () => { $('board-sheet').hidden = true; $('open-board').classList.remove('on'); });

// A finding laid on the board to the right of what is there: items in a column, each beneath the one before.
async function lay(items) {
  const b = await openBoard(), box = B.bounds(b.board);
  let x = box ? box.x + box.w + 90 : 0, y = box ? box.y : 0;
  const ids = [];
  const taken = new Set(b.tables.keys());
  for (const item of items) {
    let props = item.props;
    if (item.kind === 'table') { const name = B.tableName(props.name, taken); taken.add(name); item.rename?.(name); props = { ...props, name }; }
    const size = item.size || (item.kind === 'card' ? { w: 520, h: 380 } : B.DEFAULT_SIZE[item.kind]);
    ids.push(b.add(item.kind, typeof props === 'function' ? props() : props, { at: { x: x + size.w / 2, y: y + size.h / 2 }, size }));
    y += size.h + 24;
  }
  b.select([]);
  b.fit(ids);
  say('Sent to the board.');
}
const table = (name, title, columns, source = `Agora, on ${state.product.credit}`) => ({ name, title, source, rows: Object.values(columns)[0].length,
  columns: Object.keys(columns).map((c) => ({ name: c, kind: typeof columns[c].find((v) => v !== null) === 'number' ? 'number' : 'text' })), data: columns });
const assumed = () => `Presumed, not observed: Huff's model (${state.params.decay}, β ${state.params.beta}) on minutes walked, reach ${state.params.limit} min; ${state.params.residents_per_dwelling} residents a dwelling, ${state.params.workers_per_100m2} workers per 100 m² of office; a resident brings ${state.params.spend_resident} a week, a worker ${state.params.spend_worker}, over ${state.params.weeks} weeks.`;
const lonLat = (x, y) => { const o = state.product.origin, c = Math.cos((o.lat * Math.PI) / 180); return [o.lon + x / (111320 * c), o.lat + y / 110574]; };

function sendMarket() {
  const m = state.market, order = sellersByDemand().slice(0, 40), blocks = state.product.blocks;
  const sellers = table('sellers', `${kindName()}: sellers by what they sell a year`, { seller: order.map((s) => m.seller.name[s]), outlets: order.map((s) => m.seller.outlets[s]), sales: order.map((s) => m.seller.demand[s]),
    share: order.map((s) => Math.round(1e4 * m.seller.demand[s] / m.bought) / 100) });
  const unserved = table('unserved', `${kindName()}: the spend of each block that buys nothing, a year`, { block: blocks.map((b) => b.id), lat: blocks.map((b) => b.lat), lon: blocks.map((b) => b.lon), brings: m.block.brings, unserved: m.block.unserved });
  let sellersName = 'sellers', unservedName = 'unserved';
  lay([
    { kind: 'note', props: { type: 'insight', text: `${kindName()} in ${state.product.name}, ${state.product.year}: ${count(m.offers)} outlets of ${count(m.sellers)} sellers. All who could buy bring ${money(m.total)} a year; ${percent(m.penetration)} of it buys. The four largest sellers hold ${percent(m.structure.cr4, 1)}; the index of concentration is ${m.structure.hhi.toFixed(4)}.` }, size: { w: 300, h: 190 } },
    { kind: 'note', props: { type: 'assumption', text: assumed() }, size: { w: 300, h: 210 } },
    { kind: 'table', props: sellers, rename: (n) => { sellersName = n; }, size: { w: 520, h: 200 } },
    { kind: 'card', props: () => ({ card: 'chart', props: { table: sellersName, form: 'bar', x: 'seller', y: 'sales', most: 15, title: 'Sellers by what they sell a year' } }) },
    { kind: 'table', props: unserved, rename: (n) => { unservedName = n; }, size: { w: 520, h: 200 } },
    { kind: 'card', props: () => ({ card: 'map', props: { table: unservedName, value: 'unserved', size: 'unserved', label: 'block', title: 'Spend that buys nothing, by block' } }) },
  ]);
}
function sendOutlet() {
  const a = state.offerAnswer, blocks = state.product.blocks, keep = blocks.map((b, i) => i).filter((i) => a.trade[i] > 0);
  let name = 'custom';
  lay([
    { kind: 'note', props: { type: 'insight', text: `${nameOf(a.offer)}: ${money(a.demand)} a year, ${percent(a.share, 2)} of what is bought. Half its custom lies within ${minutes(a.reach.half)}, four fifths within ${minutes(a.reach.most)}. If it closed, ${percent(a.diversion.outside, 1)} of its custom would buy nothing; most of the rest would go to ${a.diversion.to.slice(0, 3).map((d) => `${nameOf(d.offer)} (${percent(d.part, 1)})`).join(', ')}.` }, size: { w: 300, h: 230 } },
    { kind: 'note', props: { type: 'assumption', text: assumed() }, size: { w: 300, h: 210 } },
    { kind: 'table', props: table('custom', `The custom of ${nameOf(a.offer)}, by block, a year`, { block: keep.map((i) => blocks[i].id), lat: keep.map((i) => blocks[i].lat), lon: keep.map((i) => blocks[i].lon), custom: keep.map((i) => a.trade[i]) }), rename: (n) => { name = n; }, size: { w: 520, h: 200 } },
    { kind: 'card', props: () => ({ card: 'map', props: { table: name, value: 'custom', size: 'custom', label: 'block', title: `Whose custom: ${nameOf(a.offer)}` } }) },
  ]);
}
function sendEntry() {
  const e = state.entry, who = state.seller >= 0 ? state.market.seller.name[state.seller] : 'a newcomer', [lon, lat] = lonLat(...e.at);
  lay([
    { kind: 'note', props: { type: 'insight', text: `An outlet of ${who} at ${lat.toFixed(5)}, ${lon.toFixed(5)} would sell ${money(e.demand)} a year: ${percent(e.from_rivals / e.demand)} from rivals, ${percent(e.from_outside / e.demand)} from those who bought nothing${state.seller >= 0 ? `, ${percent(e.cannibalised / e.demand)} from its own outlets` : ''}. ${who === 'a newcomer' ? 'It' : who} would gain ${money(e.gain)}.` }, size: { w: 300, h: 200 } },
    { kind: 'note', props: { type: 'assumption', text: `${assumed()} Rivals are taken not to respond.` }, size: { w: 300, h: 230 } },
  ]);
}
function sendSites() {
  const s = state.sites, who = state.seller >= 0 ? state.market.seller.name[state.seller] : 'a newcomer', at = s.chosen.map((c) => lonLat(c.x, c.y));
  let name = 'sites';
  lay([
    { kind: 'note', props: { type: 'insight', text: `Three sites for ${who}, chosen among ${count(s.candidates)} ${s.from === 'vacant' ? 'vacant spaces' : 'blocks'}: sales of ${money(s.after)} a year, from ${money(s.before)} now. The gains: ${s.chosen.map((c) => money(c.gain)).join(', ')}.` }, size: { w: 300, h: 170 } },
    { kind: 'note', props: { type: 'assumption', text: `${assumed()} Rivals are taken not to respond; the sites are chosen one at a time, each of greatest gain.` }, size: { w: 300, h: 250 } },
    { kind: 'table', props: table('sites', `Sites for ${who}, in the order chosen`, { site: s.chosen.map((c, i) => `Site ${i + 1}`), lat: at.map((p) => p[1]), lon: at.map((p) => p[0]), gain: s.chosen.map((c) => Math.round(c.gain)) }), rename: (n) => { name = n; }, size: { w: 440, h: 170 } },
    { kind: 'card', props: () => ({ card: 'map', props: { table: name, value: 'gain', size: 'gain', label: 'site', title: `Where to open: ${who}` } }) },
  ]);
}
function sendSize() {
  // The assumptions as quantities on notes, and a figure that computes the size from them: the board reckons it again by itself.
  const z = state.size, leaf = (n) => z.nodes.find((x) => x.name === n), line = (n) => { const x = leaf(n); return x.assumed && x.high > x.low ? `${n} = ${x.mid} [${x.low}, ${x.high}]` : `${n} = ${x.mid}`; };
  lay([
    { kind: 'note', props: { type: 'fact', text: `${line('dwellings')}\n${line('office_100m2')}`, source: `CLUE ${state.product.year}, City of Melbourne` }, size: { w: 260, h: 190 } },
    { kind: 'note', props: { type: 'assumption', text: ['residents_per_dwelling', 'spend_resident', 'workers_per_100m2', 'spend_worker', 'weeks'].map(line).join('\n') }, size: { w: 300, h: 330 } },
    { kind: 'figure', props: { title: `${kindName()}: all who could buy, a year`, formula: 'residents = dwellings * residents_per_dwelling * spend_resident * weeks\nworkers = office_100m2 * workers_per_100m2 * spend_worker * weeks\nTAM = residents + workers' }, size: { w: 340, h: 260 } },
  ]);
}

// ---- the beginning ---------------------------------------------------------------------------------------------------------------
async function setup(status) {
  const sheet = $('setup');
  sheet.hidden = false;
  const view = await getJson('/setup').catch(() => null);
  const refresh = async () => { const s = await getJson('/api/status').catch(() => null); if (s?.engine.current && s.cities.length) location.reload(); else setTimeout(() => setup(s || status), 3000); };
  sheet.replaceChildren(
    el('h2', {}, 'Agora is not yet set up here'),
    el('p', {}, !status.engine.current ? 'Its engine is not built from the present sources. ' : '', !status.cities.length ? 'The data library holds no market\'s places yet. ' : '', 'The recipe builds the engine with g++ and fetches the City of Melbourne\'s census and the city model:'),
    el('ol', {}, (view?.steps || []).map((s) => el('li', { class: s.done ? 'done' : '' }, `${s.title}${s.done ? ' — done' : ''}`))),
    view?.state === 'running' ? el('p', { class: 'tip' }, 'It is running…') : el('div', { class: 'actions' }, el('button', { class: 'key edged', type: 'button', onclick: async () => { await fetch('/setup/run', { method: 'POST' }); setup(status); } }, 'Run the recipe')),
    el('p', { class: 'tip' }, 'Or, in a terminal: python data/acquisition/recipe.py applications/agora/recipe.json'));
  if (view?.state === 'running') setTimeout(refresh, 3000);
}

async function begin() {
  let status;
  try { status = await getJson('/api/status'); } catch { $('state').textContent = 'The server did not answer.'; return; }
  if (!status.engine.current || !status.cities.length) { setup(status); return; }
  state.city = status.cities[0].name;
  state.product = await getJson(`/data/products/${status.cities[0].file}`);
  // A block's area, in hectares, for densities.
  for (const b of state.product.blocks) { let a = 0; b.ring.forEach(([x, y], i) => { const [u, v] = b.ring[(i + 1) % b.ring.length]; a += x * v - u * y; }); b.area = Math.abs(a) / 2 / 1e4; }
  const model = await getJson(`/data/products/${state.product.model}`);
  $('city').textContent = `${state.product.name}, ${state.product.year}`;
  const kinds = state.product.kinds.filter((k) => k.code !== '0000');
  $('kind').replaceChildren(...kinds.map((k) => el('option', { value: k.code, selected: k.code === state.params.kind }, `${k.name} (${k.count})`)));
  $('kind').addEventListener('change', (e) => { state.params.kind = e.target.value; state.seller = -1; compute(); });
  plane = createPlane($('map'), {
    inset: () => ({ left: 276, right: 342 }),
    onSettle: () => paint(),
    onPick: (e, point, target) => {
      const dot = target.closest?.('[data-offer]');
      if (dot) { pickOffer(Number(dot.dataset.offer)); return; }
      if (state.view === 'open') enterAt(point);
    },
  });
  drawGround(model);
  const xs = state.product.blocks.flatMap((b) => b.ring.map((p) => p[0])), ys = state.product.blocks.flatMap((b) => b.ring.map((p) => p[1]));
  plane.fit({ x0: Math.min(...xs), x1: Math.max(...xs), y0: Math.min(...ys), y1: Math.max(...ys) }, 18);
  assumptions();
  await compute();
}
await begin();

// For the checks that drive the page, and for the curious.
window.agora = { state, ask, show, pickOffer, enterAt, chooseSites, openBoard, get plane() { return plane; }, get board() { return board; }, B };
