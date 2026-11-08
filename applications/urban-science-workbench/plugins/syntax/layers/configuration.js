// How each street stands in the whole (ADR 0084): pick a place, and its streets within 1.5 km become a segment map whose every
// segment is coloured by its choice (how often it lies on the least paths between all others; NACH) or its integration (how near it
// lies to all others; NAIN), at an angular, metric or topological cost and a radius (urban-science's configuration, on morphe's
// centralities). Where the City of Melbourne counts walkers, its counters stand on the map at their mean daily count, and the card
// says how far the streets' configuration goes with it.

import { colourMap } from '/visualization/styles/index.js';
import { linear, palette, waysNear, whenNear } from '../shared.js';

const READ = 1500;
const MEASURES = [['nach', 'choice'], ['integration', 'integration']];
const COSTS = [['angular', 'angular'], ['metric', 'metric'], ['topological', 'turns']];
const RADII = [[400, '400 m'], [800, '800 m'], [1200, '1.2 km'], [0, 'n']];

export async function layer(ctx) {
  const { gis, frame, world, dom: { h } } = ctx;
  const id = ctx.layerId(':segments'), countersId = ctx.layerId(':counters');
  const P = palette(ctx.theme);
  const map = colourMap(ctx.theme === 'night' ? 'plum' : 'orchid').colour;
  const colour = (u) => map(ctx.theme === 'night' ? 0.12 + 0.88 * u : 0.08 + 0.92 * u);
  let measure = 'nach', cost = 'angular', radius = 800, centre = null, answer = null, token = 0, dead = false;
  let sensors = null;
  const status = h('div', { class: 'ld-quiet' }, 'Pick a place: its streets are read as a whole.');
  const readout = h('div', { class: 'gs-readout' });
  const against = h('div', { class: 'gs-readout' });
  const ramp = h('div', { class: 'gs-ramp' });
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));

  // The City's counters, from the data library, in this area's frame: Melbourne's only.
  async function counters() {
    if (sensors) return sensors;
    try {
      const r = await fetch('/data/products/street-sensors.json');
      const j = r.ok ? await r.json() : { sensors: [] };
      sensors = (j.sensors || []).filter((s) => Number.isFinite(s.lat)).map((s) => {
        const p = frame.toLocal(s.lat, s.lon);
        return { id: s.id, name: s.name, x: Math.round(p[0] * 10) / 10, y: Math.round(p[1] * 10) / 10 };
      });
    } catch { sensors = []; }
    return sensors;
  }

  function draw() {
    if (!answer) return;
    // Each street by its rank among those read (the measures crowd near their tops, so a linear scale would light nearly all).
    const values = answer[measure];
    const order = values.map((v, k) => k).sort((a, b) => values[a] - values[b]);
    const rank = new Float64Array(values.length);
    order.forEach((k, i) => { rank[k] = order.length > 1 ? i / (order.length - 1) : 1; });
    const u = (v, k) => rank[k];
    const width = Math.max(1.6, ctx.earth.view.range * 0.0025);
    const features = answer.segments.map((s, k) => ({ k, line: [[s[0], s[1]], [s[2], s[3]]] })).sort((a, b) => values[a.k] - values[b.k]);
    gis.addRibbons(id, features, { line: (f) => f.line, width: (f) => width * (0.35 + 1.1 * u(values[f.k], f.k) ** 2), color: (f) => linear(colour(u(values[f.k], f.k))), z: 0.45, order: 3 });
    ramp.replaceChildren(h('span', {}, 'lowest'), h('i', { style: `background:linear-gradient(90deg, ${[0, 0.25, 0.5, 0.75, 1].map((t) => P.css(colour(t))).join(', ')})` }), h('span', {}, 'highest, by rank'));
    const shown = answer.counters || [];
    if (shown.length) {
      const top = Math.max(...shown.map((c) => c.meanDaily));
      gis.addPoints(countersId, shown, { position: (c) => [c.x, c.y], radius: (c) => width * (1.2 + 4 * Math.sqrt(c.meanDaily / top)), height: () => width * 1.5,
        color: () => P.accent, pick: (c) => `counter: ${Math.round(c.meanDaily).toLocaleString('en')} walkers a day` });
    } else gis.remove(countersId);
  }

  async function compute() {
    if (!centre) return;
    const me = ++token;
    status.textContent = 'gathering the streets here…';
    const ways = await whenNear(() => waysNear(world, centre, READ), () => me === token && !dead);
    if (me !== token || dead) return;
    if (!ways) { status.textContent = 'No ways here: pick another place.'; return; }
    status.textContent = `reading ${ways.length.toLocaleString('en')} ways as a whole…`;
    const request = { ways, cost, radius, counters: await counters() };
    let a;
    try { a = await ctx.ask('configuration', request); } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    answer = a;
    draw();
    readout.replaceChildren(
      h('div', {}, h('span', {}, 'segments'), h('b', {}, `${a.segments.length.toLocaleString('en')} from ${a.junctions.toLocaleString('en')} junctions`)),
      h('div', {}, h('span', {}, 'cost, radius'), h('b', {}, `${COSTS.find(([v]) => v === cost)[1]}, ${RADII.find(([v]) => v === radius)[1]}`)),
      h('div', {}, h('span', {}, 'computed'), h('b', {}, `in ${a.seconds.toFixed(1)} s on ${a.threads} threads`)));
    if (a.against) {
      const g = a.against;
      against.replaceChildren(
        h('div', {}, h('span', {}, 'counters here'), h('b', {}, `${g.n}, at their mean daily walkers`)),
        h('div', {}, h('span', {}, 'with choice'), h('b', {}, `ρ ${g.rhoNach.toFixed(2)}, R² ${g.r2Nach.toFixed(2)}`)),
        h('div', {}, h('span', {}, 'with integration'), h('b', {}, `ρ ${g.rhoIntegration.toFixed(2)}, R² ${g.r2Integration.toFixed(2)}`)));
    } else against.replaceChildren();
    say();
  }

  function say() {
    if (!answer) return;
    const name = MEASURES.find(([v]) => v === measure)[1];
    status.textContent = `Each street coloured by the rank of its ${name} among those read, its width growing with it; the counters in rose. Read within ${READ / 1000} km: near that edge the values are lower than the city's.`;
  }

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'How each street stands in the whole'),
    h('div', { class: 'gs-row' }, h('span', {}, 'measure'), keys(MEASURES, measure, (v) => { measure = v; draw(); say(); })),
    h('div', { class: 'gs-row' }, h('span', {}, 'cost'), keys(COSTS, cost, (v) => { cost = v; compute(); })),
    h('div', { class: 'gs-row' }, h('span', {}, 'radius'), keys(RADII, radius, (v) => { radius = v; compute(); })),
    ramp, readout, against, status,
    h('div', { class: 'ld-quiet' }, 'Ways: Overture Maps, OpenStreetMap. Counters: City of Melbourne (CC BY 4.0). Space Syntax after Hillier and Hanson; NACH and NAIN after Hillier, Yang and Turner (2012).'));
  ctx.onDispose(() => { dead = true; });
  return {
    legend,
    pickAt(lat, lon) {
      const p = frame.toLocal(lat, lon);
      centre = [Math.round(p[0] * 10) / 10, Math.round(p[1] * 10) / 10];
      compute();
      return { kind: 'place', title: 'Read as a whole', lines: [`the streets within ${READ / 1000} km`], at: { lat, lon } };
    },
    dispose() { dead = true; gis.remove(id); gis.remove(countersId); },
  };
}
