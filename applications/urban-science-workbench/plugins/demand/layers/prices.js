// What a home's price pays for (ADR 0088): any case of homes sold the data library holds (data/products/home_prices.py), through the
// engine on urban science's demand and tyche's spatial regression. Each home on the map is coloured by how much dearer or cheaper it
// sold than the model predicts; the card gives what the subway, parks, a protected lane and the centre add to a price under least
// squares and under the spatial models, which the Lagrange multiplier tests choose between, in proportions and in money at the median.

import { addEstimates, percent, effectColour } from '/visualization/js/gis/effects.js';

const SCALE = 0.5;
const css = (rgb) => `rgb(${rgb.map((v) => Math.round(255 * v)).join(',')})`;
const AMENITIES = [[7, 'the subway, 10% farther', 'distance'], [8, 'a park, 10% farther', 'distance'], [9, 'a protected lane within 300 m', 'indicator'], [10, 'the centre, 10% farther', 'distance']];

export async function layer(ctx) {
  const { gis, frame, earth, dom: { h } } = ctx;
  const homesId = ctx.layerId(':homes');
  let cases = {}, chosen = null, R = null, sample = 1200, token = 0, dead = false;
  const status = h('div', { class: 'ld-quiet' }, 'Finding the records of homes sold…');
  const head = h('div', { class: 'ld-quiet' });
  const table = h('div', { class: 'gs-readout' });
  const tests = h('div', { class: 'ld-quiet' });
  const choices = h('div', { class: 'keys-tight' });
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));
  const money = (v) => `$${Math.round(v).toLocaleString('en')}`;

  // An amenity's effect: a distance in logarithms reads as the change for 10% farther; an indicator as its premium.
  const effect = (b, kind) => (kind === 'distance' ? percent(b * Math.log(1.1), 1) : percent(b, 1));

  function draw() {
    if (!R) return;
    const homes = R.homes_drawn.map((r) => ({ r, p: frame.toLocal(r[0], r[1]) }));
    addEstimates(gis, homesId, homes, { position: (f) => [f.p[0], f.p[1]], value: (f) => f.r[2], scale: SCALE, theme: ctx.theme,
      radius: Math.max(5, earth.view.range * 0.0025), height: Math.max(10, earth.view.range * 0.006) });
  }

  function card() {
    const rows = [h('div', {}, h('span', {}, 'a price changes by'), h('b', {}, `least squares · ${R.chosen === 'least squares' ? 'chosen' : `${R.chosen}, chosen (with spillover)`} · ${R.chosen === 'error' ? 'lag' : 'error'}`))];
    const other = R.chosen === 'error' ? R.lag : R.error;
    const pick = R.chosen === 'lag' ? R.lag : R.chosen === 'error' ? R.error : R.ols;
    for (const [i, name, kind] of AMENITIES) {
      const o = R.ols.beta[i], c = pick.beta[i], x = other.beta[i];
      const total = R.chosen === 'lag' ? ` (${effect(R.impacts.total[i], kind)})` : '';
      rows.push(h('div', {}, h('span', {}, name), h('b', {}, `${effect(o, kind)} · ${effect(c, kind)}${total} · ${effect(x, kind)}`)));
    }
    const lane = pick.beta[9];
    rows.push(h('div', {}, h('span', {}, 'a protected lane, at the median price'), h('b', {}, `${money(Math.expm1(lane) * R.median_price)} (least squares ${money(Math.expm1(R.ols.beta[9]) * R.median_price)})`)));
    table.replaceChildren(...rows);
    const t = R.tests;
    tests.textContent = `Moran's I of the least-squares residuals ${R.moran.I.toFixed(2)}; LM lag ${t.lm_lag.toFixed(0)}, LM error ${t.lm_error.toFixed(0)}, robust ${t.robust_lag.toFixed(0)} and ${t.robust_error.toFixed(0)}: ` +
      `the rule chooses ${R.chosen === 'least squares' ? 'least squares' : `the ${R.chosen} model`} (ρ ${R.lag.rho.toFixed(2)}, λ ${R.error.lambda.toFixed(2)}). ` +
      'What least squares credits to an amenity, the spatial models give partly to the neighbourhood it lies in: an amenity built where homes already cost more looks dearer than it is.';
    head.textContent = `${R.city}, ${R.year}: ${R.question}. ${R.homes.toLocaleString('en')} of ${R.of.toLocaleString('en')} homes sold, at their ${R.neighbours} nearest neighbours; the median price ${money(R.median_price)}.`;
  }

  async function evaluate() {
    if (!chosen) return;
    const me = ++token;
    status.textContent = `Fitting the models to ${sample.toLocaleString('en')} homes… (about ${sample > 1500 ? 'a minute' : 'twenty seconds'})`;
    try {
      const r = await ctx.ask('hedonic', { case: chosen, sample });
      if (me !== token || dead) return;
      R = r;
    } catch (e) { status.textContent = String(e.message || e); return; }
    card();
    draw();
    status.textContent = 'Each home coloured by its residual in the chosen model: rose, dearer than its rooms, land and place predict; sea glass, cheaper.';
  }

  try { cases = await ctx.ask('cases', {}); } catch (e) { status.textContent = `No record of homes: ${e.message || e}`; }
  const near = () => {
    const v = earth.view, lat = (v.lat * 180) / Math.PI, lon = (v.lon * 180) / Math.PI;
    let best = null, bd = Infinity;
    for (const [k, c] of Object.entries(cases)) {
      const d = Math.hypot(c.origin.lat - lat, (c.origin.lon - lon) * Math.cos((lat * Math.PI) / 180)) * 111;
      if (d < bd) bd = d, best = k;
    }
    return bd < 60 ? best : null;
  };
  choices.replaceChildren(...Object.entries(cases).map(([k, c]) => h('button', { onclick: () => {
    earth.setView({ lon: (c.origin.lon * Math.PI) / 180, lat: (c.origin.lat * Math.PI) / 180, height: 0, range: 16000, tilt: 0.3, heading: 0 });
    chosen = k;
    evaluate();
  } }, c.city)));
  chosen = near();
  if (chosen) evaluate();
  else status.textContent = 'No record of homes sold here: choose a city.';

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'What a home\'s price pays for'),
    h('div', { class: 'gs-row' }, h('span', {}, 'record'), choices),
    h('div', { class: 'gs-row' }, h('span', {}, 'homes'), keys([[1200, '1,200'], [2000, '2,000']], sample, (v) => { sample = v; evaluate(); })),
    head, table, tests,
    h('span', { class: 'ramp', style: `background:linear-gradient(90deg, ${Array.from({ length: 11 }, (_, i) => css(effectColour((i / 5 - 1) * SCALE, SCALE))).join(',')})` }),
    h('div', { class: 'ramp-ends' }, h('span', {}, percent(-SCALE)), h('span', {}, 'sold against the model'), h('span', {}, percent(SCALE))),
    status,
    h('div', { class: 'ld-quiet' }, 'Models: tyche\'s spatial regression (Anselin 1988; LeSage and Pace 2009). Records: NYC Department of Finance, MTA, NYC Parks, NYC DOT (NYC Open Data).'));
  ctx.onDispose(() => { dead = true; });
  return {
    legend,
    describe(hit) {
      if (!R || hit.layer !== homesId) return null;
      const r = hit.feature.r;
      return { kind: 'home', title: `A home sold for ${money(Math.exp(r[3]))}`, lines: [`${percent(r[2], 0)} against what its rooms, land and place predict in the chosen model`] };
    },
    dispose() { dead = true; gis.remove(homesId); },
  };
}
