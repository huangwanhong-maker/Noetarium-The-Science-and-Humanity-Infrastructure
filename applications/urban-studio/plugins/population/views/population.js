// Population dynamics (ADR 0124): the field as the page. A place's people, cell by cell, painted on its grid year by year, with a
// ribbon of years beneath to step and play through. Four ways of reading it: as recorded; the laws learned of it, each judged on
// the years kept aside, its forecast drawn against the record and the currents it carries people by; the record's modes, each a
// map with its rate; and the years ahead under a law, where a new source of people may be placed by a click and the spread scaled.
// A cell opens where it stands, with its people over the years.

import { arrowsOf, changeOf, changeScaleOf, coloursOf, legendSvg, overlaySvg, paintField, scaleOf } from '/visualization/js/dynamics/fields.js';
import { complexPlane, lineChart } from '/visualization/js/dynamics/charts.js';

const enc = encodeURIComponent;
const P = {
  project: null, cases: [], study: {}, case: null, field: null, land: null, order: null, values: {}, scale: null,
  mode: 'recorded', year: null, showChange: false, currents: true,
  learned: null, chosen: null, showing: 'forecast',
  modes: null, mode_k: 1,
  run: null, law: 'drift', spread: 1, sources: [], placing: false, people: 2000, to: 2050,
  cell: null, playing: null, wrong: '', busy: '',
};
const base = (ctx) => `/api/projects/${enc(ctx.name)}/population`;
const night = () => document.documentElement.dataset.theme === 'night';
const pct = (x) => `${(100 * x).toFixed(1)}%`;
const fig = (x) => (x == null || !Number.isFinite(x) ? '–' : Math.abs(x) >= 1e6 ? `${(x / 1e6).toFixed(2)} million` : Math.round(x).toLocaleString('en-GB'));
const signed = (x) => (x == null || !Number.isFinite(x) ? '–' : `${x >= 0 ? '+' : '−'}${Math.abs(x).toFixed(2)}`);

export function view() {
  return { id: 'population', label: 'Population', draw, heard: (e, ctx) => { if (/population/.test(e.what) && document.querySelector('.tab.on')?.dataset.tab === 'population') { P.project = null; ctx.show('population'); } } };
}

// ---- the record --------------------------------------------------------------------------------------------------------------------

async function ensure(ctx) {
  if (P.project !== ctx.name) {
    Object.assign(P, { project: ctx.name, field: null, case: null, learned: null, modes: null, run: null, cell: null, sources: [], wrong: '' });
    const r = await ctx.api(base(ctx));
    P.cases = r.cases || [], P.study = r.study || {};
    const want = P.study.case || ctx.project?.population?.case || P.cases[0]?.case;
    if (want) await open(ctx, want);
    if (P.study.law) P.law = P.study.law;
    if (Array.isArray(P.study.sources)) P.sources = P.study.sources;
  }
}

async function open(ctx, c) {
  P.busy = 'Reading the place…';
  const f = await ctx.api(`${base(ctx)}/field?case=${enc(c)}`);
  const land = f.land.map((row) => row.map((v) => !!v));
  const order = [];
  land.forEach((row, i) => row.forEach((v, j) => { if (v) order.push([i, j]); }));
  const values = {};
  for (const e of f.epochs) values[e] = order.map(([i, j]) => f.values[String(e)][i][j] ?? 0);
  let most = 0;
  for (const e of f.epochs) for (const v of values[e]) most = Math.max(most, v);
  Object.assign(P, { case: c, field: f, land, order, values, scale: scaleOf([0, most], { log: true }), year: lastRecorded(f), learned: null, modes: null, run: null, cell: null, busy: '' });
}

const lastRecorded = (f) => Math.max(...f.epochs.filter((e) => !(f.projected || []).includes(e)));
const recorded = (y) => P.field.epochs.includes(y) && !(P.field.projected || []).includes(y);
const totalOf = (u) => u.reduce((s, v) => s + (v || 0), 0);

async function ask(ctx, op, body) {
  return ctx.api(`${base(ctx)}/${op}`, { method: 'POST', body: { case: P.case, ...body } });
}

async function keepStudy(ctx) {
  try { await ctx.api(base(ctx), { method: 'PUT', body: { study: { case: P.case, law: P.law, sources: P.sources } } }); } catch { /* a page that may only look keeps nothing */ }
}

// ---- drawing -------------------------------------------------------------------------------------------------------------------------

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('pd-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  try { await ensure(ctx); } catch (error) { P.wrong = error.message; }
  ctx.here('the population');
  const redraw = () => draw(pane, ctx);
  if (!P.field) return fill(pane, begin(ctx, redraw));
  const head = header(ctx, redraw);
  const stage = el('div', { class: 'pd-stage' });
  const left = el('div', { class: 'pd-left' }), right = el('div', { class: 'pd-right' });
  stage.append(left, right);
  fill(pane, head, stage);
  try {
    if (P.mode === 'laws' && !P.learned) { say(ctx, pane, 'Learning the laws and judging them on the years kept aside…'); P.learned = await ask(ctx, 'learn', {}); P.chosen ||= P.learned.best?.name || 'drift'; }
    if (P.mode === 'modes' && !P.modes) { say(ctx, pane, 'Finding the record\'s modes…'); P.modes = await ask(ctx, 'modes', { show: 6 }); }
    if (P.mode === 'ahead' && !P.run) { say(ctx, pane, 'Learning the law on every year recorded and running it on…'); await runAhead(ctx); }
    P.wrong = '';
  } catch (error) { P.wrong = error.message; }
  left.querySelector('.pd-busy')?.remove();
  fill(pane, head, stage);
  const shown = frame();
  left.append(mapOf(ctx, shown, redraw), ribbon(ctx, redraw));
  right.append(side(ctx, shown, redraw));
  if (P.wrong) right.prepend(el('p', { class: 'wrong' }, P.wrong));
}

function say(ctx, pane, words) {
  const s = pane.querySelector('.pd-busy') || ctx.el('p', { class: 'pd-busy note' });
  s.textContent = words;
  pane.querySelector('.pd-left')?.prepend(s);
}

function header(ctx, redraw) {
  const { el } = ctx;
  const mode = (m, words) => el('button', { type: 'button', class: `pd-mode${P.mode === m ? ' on' : ''}`, 'aria-pressed': P.mode === m ? 'true' : 'false', onclick: () => { stop(); P.mode = m; P.cell = null; if (m === 'recorded' || m === 'modes') P.year = lastRecorded(P.field); redraw(); } }, words);
  const places = el('div', { class: 'pd-places' }, P.cases.map((c) => el('button', { type: 'button', class: `pd-place${c.case === P.case ? ' on' : ''}`, onclick: async () => { stop(); await open(ctx, c.case); await keepStudy(ctx); redraw(); } }, c.title || c.case)),
    el('details', { class: 'pd-new' }, el('summary', {}, '+ a place'), newPlace(ctx, redraw)));
  const f = P.field;
  return el('div', { class: 'pd-head' },
    el('div', { class: 'pd-title-row' }, el('h2', { class: 'pd-title' }, f.title), places),
    el('div', { class: 'pd-title-row' },
      el('div', { class: 'pd-modes' }, mode('recorded', 'As recorded'), mode('laws', 'Laws learned'), mode('modes', 'Modes'), mode('ahead', 'The years ahead')),
      el('span', { class: 'small' }, `${P.order.length.toLocaleString('en-GB')} cells of 1 km² · GHS-POP R2023A, the European Commission's JRC, CC BY 4.0 · 2025 and 2030 are its projections`)));
}

function newPlace(ctx, redraw) {
  const { el } = ctx;
  const name = el('input', { type: 'text', placeholder: 'a name', size: 12 }), lat = el('input', { type: 'text', placeholder: 'latitude', size: 9 }),
    lon = el('input', { type: 'text', placeholder: 'longitude', size: 9 }), half = el('input', { type: 'number', value: 30, min: 5, max: 60, step: 1, style: 'width:4.5em' });
  const go = el('button', { type: 'button', class: 'key', onclick: async () => {
    go.disabled = true; ctx.say('The data library is fetching fifty-five years of a place, about a minute…');
    try {
      const r = await ctx.api(`${base(ctx)}/place`, { method: 'POST', body: { name: name.value, lat: Number(lat.value), lon: Number(lon.value), half_km: Number(half.value) } });
      P.cases = r.cases; await open(ctx, r.case); await keepStudy(ctx); ctx.log(`Made the place ${r.case}.`); redraw();
    } catch (error) { ctx.say(error.message); go.disabled = false; }
  } }, 'Make it');
  return el('div', { class: 'pd-new-form' }, name, lat, lon, el('span', { class: 'small' }, 'half-width, km'), half, go);
}

function begin(ctx, redraw) {
  const { el } = ctx;
  return el('div', { class: 'pd-begin' }, el('h2', { class: 'pd-title' }, 'A place\'s people, as a field that moves and grows'),
    el('p', {}, 'The data library has made no place yet. Make one from its latitude and longitude, or run python data/products/population_history.py for the examples.'),
    newPlace(ctx, redraw), P.wrong ? el('p', { class: 'wrong' }, P.wrong) : '');
}

// The field shown: what the mode and the year ask for, with its colours, its legend and its arrows.
function frame() {
  const f = P.field, y = P.year;
  if (P.mode === 'laws' && P.learned) {
    const m = P.learned.models.find((x) => x.name === P.chosen);
    const held = P.learned.held_out;
    const k = held.indexOf(y);
    const best = P.learned.best;
    const fc = k >= 0 && best && best.name === P.chosen ? best.frames[k] : null;
    if (k >= 0 && P.showing !== 'recorded' && fc) {
      if (P.showing === 'error') {
        const err = changeOf(P.values[y], fc);
        const s = changeScaleOf(err);
        return { values: err, colours: coloursOf(err, s, { diverging: true }), legend: { scale: s, diverging: true, label: `the forecast less the record, ${y}, people per km²` }, arrows: null, what: `the forecast's error in ${y}` };
      }
      return { values: fc, colours: coloursOf(fc, P.scale, { reverse: night() }), legend: { scale: P.scale, label: `people per km², the forecast for ${y}` }, arrows: null, what: `the forecast for ${y}` };
    }
    const cur = P.currents && P.learned.currents?.[P.chosen];
    const u = P.values[recordedOrNear(y)];
    return { values: u, colours: coloursOf(u, P.scale, { reverse: night() }), legend: { scale: P.scale, label: `people per km², ${recordedOrNear(y)}` }, arrows: cur ? arrowsOf({ rows: f.grid.rows, cols: f.grid.cols, land: P.land, east: cur.east, north: cur.north, block: 4 }) : null, what: `the record in ${y}${m ? `, with the currents of ${m.title}` : ''}` };
  }
  if (P.mode === 'modes' && P.modes) {
    const md = P.modes.modes[P.mode_k - 1];
    if (md?.map) {
      const s = changeScaleOf(md.map);
      return { values: md.map, colours: coloursOf(md.map, s, { diverging: true }), legend: { scale: s, diverging: true, label: `mode ${P.mode_k} as it stood in ${P.modes.from}, people per km²` }, arrows: null, what: `mode ${P.mode_k}` };
    }
  }
  if (P.mode === 'ahead' && P.run) {
    const k = P.run.years.indexOf(y);
    if (k >= 0) {
      const u = P.run.frames[k];
      const cur = P.currents ? P.run.currents[k] : null;
      return { values: u, colours: coloursOf(u, P.scale, { reverse: night() }), legend: { scale: P.scale, label: `people per km², ${y}, under the law` }, arrows: cur ? arrowsOf({ rows: f.grid.rows, cols: f.grid.cols, land: P.land, east: cur.east, north: cur.north, block: 4 }) : null, what: `the field in ${y} under the law` };
    }
  }
  const yy = P.values[y] ? y : lastRecorded(f);
  if (P.showChange) {
    const i = f.epochs.indexOf(yy);
    const prev = f.epochs[Math.max(0, i - 1)];
    const ch = changeOf(P.values[prev], P.values[yy]);
    const s = changeScaleOf(ch);
    return { values: ch, colours: coloursOf(ch, s, { diverging: true }), legend: { scale: s, diverging: true, label: `people per km² gained or lost, ${prev} to ${yy}` }, arrows: null, what: `the change from ${prev} to ${yy}` };
  }
  return { values: P.values[yy], colours: coloursOf(P.values[yy], P.scale, { reverse: night() }), legend: { scale: P.scale, label: `people per km², ${yy}${(f.projected || []).includes(yy) ? ' (the JRC\'s projection)' : ''}` }, arrows: null, what: `the people of ${f.title.split(':')[0]} in ${yy}` };
}

const recordedOrNear = (y) => (P.values[y] ? y : lastRecorded(P.field));

function mapOf(ctx, shown, redraw) {
  const { el } = ctx;
  const f = P.field, rows = f.grid.rows, cols = f.grid.cols;
  const width = Math.min(640, Math.max(360, (document.querySelector('.pd-left')?.clientWidth || 600)));
  const cell = Math.max(3, Math.floor(width / cols));
  const W = cols * cell, H = rows * cell;
  const canvas = el('canvas', { class: 'pd-canvas', width: W * (window.devicePixelRatio || 1), height: H * (window.devicePixelRatio || 1), style: `width:${W}px;height:${H}px`, role: 'img', 'aria-label': shown.what });
  const g = canvas.getContext('2d');
  g.scale(window.devicePixelRatio || 1, window.devicePixelRatio || 1);
  paintField(g, { rows, cols, land: P.land, colours: shown.colours, cell, ground: getComputedStyle(document.documentElement).getPropertyValue('--pd-sea').trim() || (night() ? '#182027' : '#e1e8ea') });
  const marks = [];
  if (P.mode === 'ahead') for (const s of P.sources) marks.push({ row: s.row, col: s.col, r: 2.6, label: `${fig(s.people_per_year)} a year` });
  if (P.cell) marks.push({ row: P.cell[0], col: P.cell[1], r: 1.6, kind: 'cell' });
  const over = el('div', { class: 'pd-over' });
  over.innerHTML = overlaySvg({ rows, cols, cell, arrows: shown.arrows?.arrows || [], marks, what: shown.what });
  const box = el('div', { class: `pd-map${P.placing ? ' placing' : ''}`, style: `width:${W}px;height:${H}px` }, canvas, over);
  box.addEventListener('click', (e) => {
    const r = box.getBoundingClientRect();
    const j = Math.floor((e.clientX - r.left) / cell), i = Math.floor((e.clientY - r.top) / cell);
    if (i < 0 || j < 0 || i >= rows || j >= cols || !P.land[i][j]) return;
    if (P.placing) {
      P.sources.push({ row: i, col: j, people_per_year: Number(P.people) || 0 });
      P.placing = false, P.run = null;
      keepStudy(ctx);
      return redraw();
    }
    P.cell = P.cell && P.cell[0] === i && P.cell[1] === j ? null : [i, j];
    redraw();
  });
  const legend = el('div', { class: 'pd-legend' });
  legend.innerHTML = legendSvg({ scale: shown.legend.scale, diverging: shown.legend.diverging, reverse: !shown.legend.diverging && night(), label: shown.legend.label, width: Math.min(W, 340) });
  const under = el('div', { class: 'pd-under' }, legend,
    el('div', { class: 'pd-scalebar' }, el('span', { class: 'pd-bar', style: `width:${10 * cell}px` }), el('span', { class: 'small' }, '10 km')),
    shown.arrows ? el('span', { class: 'small pd-arrows-said' }, `arrows: the currents of people, summed over 4 km squares, longest ${fig(shown.arrows.largest)} a year; lengths by the square root`) : '');
  const card = P.cell ? cellCard(ctx, redraw) : '';
  return el('div', { class: 'pd-mapwrap' }, box, card, under);
}

function cellCard(ctx, redraw) {
  const { el } = ctx;
  const [i, j] = P.cell;
  const k = P.order.findIndex(([a, b]) => a === i && b === j);
  const f = P.field;
  const last = lastRecorded(f), proj = [last, ...(f.projected || [])];
  const series = [{ name: 'recorded', x: f.epochs.filter((e) => recorded(e)), y: f.epochs.filter((e) => recorded(e)).map((e) => P.values[e][k]), width: 2 },
    { name: 'the JRC\'s projection', x: proj, y: proj.map((e) => P.values[e][k]), width: 1.6, dash: '4 3' }];
  if (P.mode === 'ahead' && P.run) series.push({ name: 'under the law', x: P.run.years, y: P.run.frames.map((u) => u[k]), width: 2 });
  if (P.mode === 'laws' && P.learned?.best?.name === P.chosen) series.push({ name: 'forecast', x: P.learned.held_out, y: P.learned.best.frames.map((u) => u[k]), dots: true, size: 6 });
  const mid = (n) => (n - 1) / 2;
  const east = (j - mid(f.grid.cols)) * f.grid.dx / 1000, north = (mid(f.grid.rows) - i) * f.grid.dx / 1000;
  const chart = el('div', { class: 'pd-cell-chart' });
  chart.innerHTML = lineChart({ series, width: 300, height: 150, xLabel: 'year', yLabel: 'people per km²', what: 'the people of one km² over the years' });
  return el('aside', { class: 'pd-card' }, el('button', { type: 'button', class: 'pd-close', 'aria-label': 'Close', onclick: (e) => { e.stopPropagation(); P.cell = null; redraw(); } }, '×'),
    el('p', { class: 'pd-kicker' }, `A km², ${Math.abs(east).toFixed(0)} km ${east >= 0 ? 'east' : 'west'} and ${Math.abs(north).toFixed(0)} km ${north >= 0 ? 'north' : 'south'} of the middle`),
    el('p', {}, `${fig(P.values[f.epochs[0]][k])} people in ${f.epochs[0]}; ${fig(P.values[lastRecorded(f)][k])} in ${lastRecorded(f)}.`), chart);
}

// The years: recorded (filled), projected by the JRC (hollow), and those a forecast or a run gives (in the accent); a play key.
function ribbon(ctx, redraw) {
  const { el } = ctx;
  const f = P.field;
  let years = [...f.epochs];
  if (P.mode === 'ahead' && P.run) years = [...new Set([...f.epochs.filter((e) => recorded(e)), ...P.run.years])].sort((a, b) => a - b);
  if (P.mode === 'modes') years = f.epochs.filter((e) => recorded(e));
  const kind = (y) => (P.mode === 'ahead' && P.run && P.run.years.includes(y) && y !== P.run.years[0] ? 'run' : P.mode === 'laws' && P.learned?.held_out.includes(y) ? 'kept' : recorded(y) ? 'recorded' : 'projected');
  const keys = years.map((y) => el('button', { type: 'button', class: `pd-year pd-${kind(y)}${y === P.year ? ' on' : ''}`, title: `${y}`, onclick: () => { stop(); P.year = y; redraw(); } }, el('span', { class: 'pd-dot' }), el('span', { class: 'pd-y' }, String(y))));
  const play = el('button', { type: 'button', class: 'key edged pd-play', onclick: () => {
    if (P.playing) return (stop(), redraw());
    let k = Math.max(0, years.indexOf(P.year));
    if (k >= years.length - 1) k = -1;
    P.playing = setInterval(() => {
      k++;
      if (k >= years.length) return (stop(), redraw());
      P.year = years[k];
      redraw();
    }, 650);
    redraw();
  } }, P.playing ? 'Stop' : 'Play the years');
  const toggles = [];
  if (P.mode === 'recorded') toggles.push(el('label', { class: 'pd-toggle' }, el('input', { type: 'checkbox', checked: P.showChange, onchange: (e) => { P.showChange = e.target.checked; redraw(); } }), ' the change since the year before'));
  if (P.mode === 'laws' || P.mode === 'ahead') toggles.push(el('label', { class: 'pd-toggle' }, el('input', { type: 'checkbox', checked: P.currents, onchange: (e) => { P.currents = e.target.checked; redraw(); } }), ' currents'));
  const legend = el('span', { class: 'pd-ribbon-key small' }, el('span', { class: 'pd-dot pd-recorded' }), ' recorded ', el('span', { class: 'pd-dot pd-projected' }), ' the JRC\'s projection ',
    P.mode === 'laws' ? [el('span', { class: 'pd-dot pd-kept' }), ' kept aside '] : '', P.mode === 'ahead' ? [el('span', { class: 'pd-dot pd-run' }), ' under the law'] : '');
  return el('div', { class: 'pd-ribbon' }, el('div', { class: 'pd-years' }, keys), el('div', { class: 'pd-ribbon-row' }, play, toggles, legend));
}

function stop() { if (P.playing) clearInterval(P.playing); P.playing = null; }

// ---- the side, by mode ---------------------------------------------------------------------------------------------------------------

function side(ctx, shown, redraw) {
  if (P.mode === 'laws') return lawsSide(ctx, redraw);
  if (P.mode === 'modes') return modesSide(ctx, redraw);
  if (P.mode === 'ahead') return aheadSide(ctx, redraw);
  return recordedSide(ctx);
}

function totalsChart(ctx, extra = [], marks = []) {
  const f = P.field;
  const rec = f.epochs.filter((e) => recorded(e)), proj = f.projected || [];
  const joined = [rec[rec.length - 1], ...proj];
  const series = [{ name: 'recorded', x: rec, y: rec.map((e) => totalOf(P.values[e])), width: 2 }, { name: 'the JRC\'s projection', x: joined, y: joined.map((e) => totalOf(P.values[e])), width: 1.6, dash: '4 3' }, ...extra];
  const box = ctx.el('div', { class: 'pd-chart' });
  box.innerHTML = lineChart({ series, width: 380, height: 220, xLabel: 'year', yLabel: 'people', marks, what: 'everyone in the square over the years' });
  return box;
}

function recordedSide(ctx) {
  const { el } = ctx;
  const f = P.field, first = f.epochs[0], last = lastRecorded(f);
  const t0 = totalOf(P.values[first]), t1 = totalOf(P.values[last]);
  const rate = Math.log(t1 / t0) / (last - first);
  return el('div', { class: 'pd-side' },
    el('h3', { class: 'pd-side-title' }, 'Everyone in the square'), totalsChart(ctx),
    el('p', {}, `${fig(t0)} people in ${first} and ${fig(t1)} in ${last}: ${(t1 / t0).toFixed(2)} times as many, ${pct(rate)} a year on average. The JRC projects ${fig(totalOf(P.values[f.epochs[f.epochs.length - 1]]))} for ${f.epochs[f.epochs.length - 1]}.`),
    el('p', { class: 'note' }, 'Each year is the JRC\'s disaggregation of the censuses then available into the built-up surface, carried onto a 1 km grid with no person lost. Where the censuses were coarse, so is what a cell says. Choose a km² to see its years; play the years to see the place fill.'));
}

function lawsSide(ctx, redraw) {
  const { el } = ctx;
  const L = P.learned;
  if (!L) return el('div', { class: 'pd-side' });
  const ranked = L.models.filter((m) => m.held_out).sort((a, b) => b.held_out.pattern_skill - a.held_out.pattern_skill);
  const bar = (v, cls) => el('span', { class: `pd-skill ${cls}` }, el('span', { class: `pd-fill${v < 0 ? ' neg' : ''}`, style: `width:${Math.min(100, Math.abs(v) * 100).toFixed(1)}%` }));
  const rows = ranked.map((m) => el('li', { class: `pd-law${m.name === P.chosen ? ' on' : ''}` },
    el('button', { type: 'button', class: 'pd-law-key', onclick: () => { P.chosen = m.name; redraw(); } },
      el('span', { class: 'pd-law-title' }, m.title),
      el('span', { class: 'pd-law-bars' }, el('span', { class: 'small' }, 'where'), bar(m.held_out.pattern_skill, 'pattern'), el('span', { class: 'pd-num' }, signed(m.held_out.pattern_skill)),
        el('span', { class: 'small' }, 'how many'), bar(m.held_out.skill, 'total'), el('span', { class: 'pd-num' }, signed(m.held_out.skill)))),
    m.name === P.chosen ? lawCard(ctx, m, redraw) : ''));
  const failed = L.models.filter((m) => m.error).map((m) => el('li', { class: 'small wrong' }, `${m.title}: ${m.error}`));
  return el('div', { class: 'pd-side' },
    el('h3', { class: 'pd-side-title' }, `Fitted on ${L.fitted_on[0]}–${L.fitted_on[L.fitted_on.length - 1]}, judged on ${L.held_out.join(' and ')}`),
    el('p', { class: 'small' }, 'Each law\'s skill against carrying the last field forward (above zero, better): where the people went, once each forecast\'s total is set to the record\'s; and how many there were, as forecast.'),
    el('ol', { class: 'pd-laws' }, rows, failed));
}

function lawCard(ctx, m, redraw) {
  const { el } = ctx;
  const L = P.learned;
  const h = m.held_out;
  const isBest = L.best?.name === m.name;
  const words = [];
  if (m.reading) words.push(readingWords(m.reading));
  if (m.name === 'persistence') words.push('Nothing changes: the yardstick.');
  if (m.name === 'own-rates') words.push(`A rate of its own for each of ${P.order.length.toLocaleString('en-GB')} cells, from the first year to the last fitted.`);
  if (m.name === 'modes') words.push(`The record's ${m.rank} dynamic modes, carried on.`);
  const show = (s, w) => el('button', { type: 'button', class: `pd-mode${P.showing === s ? ' on' : ''}`, onclick: (e) => { e.stopPropagation(); P.showing = s; if (!L.held_out.includes(P.year)) P.year = L.held_out[L.held_out.length - 1]; redraw(); } }, w);
  return el('div', { class: 'pd-law-card' },
    el('p', {}, words.join(' ')),
    el('p', { class: 'small' }, `On ${L.held_out.join(' and ')}: the total off by ${pct(h.total_error)}; the change it forecast against the change recorded, correlated ${h.change_correlation.toFixed(2)}; ${m.parameters} numbers fitted.${m.clipped ? ` Its run set ${fig(m.clipped)} people's worth of negative densities to zero.` : ''}`),
    (m.dropped || []).map((d) => el('p', { class: 'small' }, d)),
    m.law ? el('table', { class: 'pd-terms' }, el('tbody', {}, m.law.filter((t) => t.coefficient !== 0).map((t) => el('tr', {}, el('td', { class: 'pd-sym' }, t.symbol), el('td', { class: 'pd-num' }, Number(t.coefficient).toPrecision(4)), el('td', { class: 'pd-num small' }, `± ${Number(t.se).toPrecision(2)}`))))) : '',
    isBest ? el('div', { class: 'pd-modes pd-showing' }, show('forecast', 'its forecast'), show('recorded', 'the record'), show('error', 'the error')) : el('p', { class: 'small' }, 'The map shows the record with this law\'s currents; the forecast is drawn for the law that places the people best by its skill.'));
}

export function readingWords(r) {
  const parts = [];
  if (r.growth_rate_a_year != null) {
    if (r.form === 'Gompertz') parts.push(`Gompertz's growth at ${pct(r.growth_rate_a_year)} a year${r.ceiling_per_km2 ? ` toward ${fig(r.ceiling_per_km2)} people per km²` : ''}`);
    else if (r.form === 'logistic') parts.push(`growth at ${pct(r.growth_rate_a_year)} a year${r.ceiling_per_km2 ? ` toward a ceiling of ${fig(r.ceiling_per_km2)} people per km²` : ', accelerating, with no ceiling'}`);
    else parts.push(`growth at ${pct(r.growth_rate_a_year)} a year everywhere`);
  }
  if (r.diffusivity_km2_a_year != null) parts.push(`spread at ${Number(r.diffusivity_km2_a_year).toPrecision(3)} km² a year`);
  if (r.crowding_km2_a_year_per_density != null && r.crowding_km2_a_year_per_density !== 0) parts.push(`crowding at ${Number(r.crowding_km2_a_year_per_density).toPrecision(3)} km² a year for each person per km²`);
  if (r.speed_toward_centre_km_a_year != null) parts.push(`a drift ${r.speed_toward_centre_km_a_year > 0 ? 'toward' : 'away from'} the centre at ${(Math.abs(r.speed_toward_centre_km_a_year) * 1000).toFixed(1)} m a year`);
  if (r.front_speed_km_a_year != null) parts.push(`a front of settlement advancing ${(r.front_speed_km_a_year * 1000).toFixed(0)} m a year`);
  return parts.length ? `${parts.join('; ')}.` : '';
}

function modesSide(ctx, redraw) {
  const { el } = ctx;
  const M = P.modes;
  if (!M) return el('div', { class: 'pd-side' });
  const plane = el('div', { class: 'pd-chart' });
  plane.innerHTML = complexPlane({ points: M.modes.map((m) => m.eigenvalue), circle: true, width: 300, height: 240, what: 'the modes\' eigenvalues about the unit circle' });
  const list = M.modes.map((m, k) => el('li', { class: `pd-law${P.mode_k === k + 1 ? ' on' : ''}` }, el('button', { type: 'button', class: 'pd-law-key', disabled: !m.map, onclick: () => { P.mode_k = k + 1; redraw(); } },
    el('span', { class: 'pd-law-title' }, `Mode ${k + 1}`),
    el('span', { class: 'small' }, `${m.rate_a_year >= 0 ? 'grows' : 'fades'} ${pct(Math.abs(m.rate_a_year))} a year${m.period_years ? `, turns every ${m.period_years.toFixed(0)} years` : ''}; carries ${fig(m.size)}`))));
  return el('div', { class: 'pd-side' },
    el('h3', { class: 'pd-side-title' }, `${M.rank} modes of ${M.singular_values.length + 1} years ${M.step} apart`),
    plane, el('ol', { class: 'pd-laws' }, list),
    el('p', { class: 'small' }, `The modes carry each year to the next within ${pct(M.residual)}. A mode inside the circle fades, one outside grows; a pair off the axis turns. The first is most of the place; the others are where it changed apart from it, lavender where they take away and rust where they add.`));
}

async function runAhead(ctx) {
  const body = { model: P.law, to: P.to, every: 5, sources: P.sources };
  if (P.spread !== 1) body.scale = { diffusion: P.spread, crowding: P.spread };
  P.run = await ask(ctx, 'simulate', body);
  if (!P.run.years.includes(P.year)) P.year = P.run.years[P.run.years.length - 1];
}

function aheadSide(ctx, redraw) {
  const { el } = ctx;
  const R = P.run;
  const laws = [['growth', 'growth everywhere'], ['logistic', 'logistic growth'], ['fisher', 'growth and spread'], ['drift', 'spread and drift'], ['crowding', 'crowding and drift'], ['settled', 'a drift along the settled landscape'], ['sparse', 'a law found among every term']];
  const choose = el('div', { class: 'pd-choose' }, laws.map(([k, w]) => el('button', { type: 'button', class: `pd-place${P.law === k ? ' on' : ''}`, onclick: async () => { P.law = k; P.run = null; await keepStudy(ctx); redraw(); } }, w)));
  const spreads = el('div', { class: 'pd-choose' }, el('span', { class: 'small' }, 'its spread ×'), [0.5, 1, 2, 4].map((s) => el('button', { type: 'button', class: `pd-place${P.spread === s ? ' on' : ''}`, onclick: () => { P.spread = s; P.run = null; redraw(); } }, String(s))));
  const people = el('input', { type: 'number', value: P.people, min: 0, step: 100, style: 'width:6em', onchange: (e) => { P.people = Number(e.target.value) || 0; } });
  const place = el('div', { class: 'pd-choose' }, el('button', { type: 'button', class: `key${P.placing ? ' main' : ' edged'}`, onclick: () => { P.placing = !P.placing; redraw(); } }, P.placing ? 'Click a km² on the map…' : '+ a new source of people'), people, el('span', { class: 'small' }, 'a year'),
    P.sources.length ? el('button', { type: 'button', class: 'key edged', onclick: () => { P.sources = []; P.run = null; keepStudy(ctx); redraw(); } }, `remove ${P.sources.length === 1 ? 'it' : `all ${P.sources.length}`}`) : '');
  if (!R) return el('div', { class: 'pd-side' }, choose, spreads, place);
  const extra = [{ name: 'under the law', x: R.years, y: R.totals, width: 2 }];
  const ag = (R.against_projections || []).map((a) => `for ${a.year} the JRC projects ${fig(a.projected_total)} and the law runs to ${fig(a.run_total)}`);
  return el('div', { class: 'pd-side' },
    el('h3', { class: 'pd-side-title' }, `The years ahead under ${laws.find(([k]) => k === P.law)?.[1] || P.law}`), choose, spreads, place,
    totalsChart(ctx, extra),
    el('p', {}, `Learned on ${R.fitted?.on?.[0] ?? ''}–${R.fitted?.on?.[1] ?? ''}: ${readingWords(R.reading)} From ${fig(R.totals[0])} in ${R.years[0]} to ${fig(R.totals[R.totals.length - 1])} in ${R.years[R.years.length - 1]}${ag.length ? `; ${ag.join('; ')}` : ''}.`),
    R.clipped > 0 ? el('p', { class: 'small' }, `The run set ${fig(R.clipped)} people's worth of negative densities to zero: the law was asked beyond what it describes.`) : '',
    el('p', { class: 'note' }, 'A run is what the law does if nothing it does not hold changes: no new road, no rezoning, no change in who comes. It is a consequence of the law, not a forecast of the place.'));
}
