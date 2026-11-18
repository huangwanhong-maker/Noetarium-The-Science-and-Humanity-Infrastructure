// Which roads matter most to reaching care, under a place's floods (ADR 0129): pick a place, and the ways in view (by road), its people
// (GHS-POP shared among buildings), the buildings mapped as care and, where the data library holds them, the hazard's maps go to the
// engine. Each road link is coloured by what the floods cost through it (its risk: what protecting it would save) or by what losing it
// would add (its criticality), in person-minutes added to one trip to the nearest care by everyone, a person cut off counted at two
// hours, expected over the year's largest flood (each map standing for the floods of its band of return periods, nothing below the
// first). How long a flood stands is not in the maps, so this is no total over a year. Where no map is held, a dry year alone.

import { colourMap } from '/visualization/styles/index.js';
import { CARE, buildings, frameOf, groundOf, hazards, peopleAbout, waysFor } from '../shared.js';

export async function layer(ctx) {
  const { gis, frame, area, dom: { h } } = ctx;
  const id = ctx.layerId(':links'), care = ctx.layerId(':care');
  const ground = groundOf(ctx);
  const map = colourMap(ctx.theme === 'night' ? 'plum' : 'orchid').colour;
  let measure = 'risk', result = null, latlon = null, dead = false, token = 0, threshold = 30;
  const status = h('div', { class: 'ld-quiet' }, 'Pick a place: which roads matter most to reaching care.');
  const readout = h('div', { class: 'gs-readout' });
  const table = h('div', {});
  const css = (rgb) => `rgb(${rgb.map((v) => Math.round(255 * v)).join(',')})`;
  const shade = (u) => map(ctx.theme === 'night' ? 0.12 + 0.88 * u : 0.08 + 0.92 * u);
  const key = h('div', {}, h('span', { class: 'ramp', style: `background:linear-gradient(90deg, ${Array.from({ length: 11 }, (_, i) => css(shade(i / 10))).join(',')})` }),
    h('div', { class: 'ramp-ends' }, h('span', {}, 'nothing'), h('span', {}, 'the most'), h('span', {}, '')));
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));
  const n0 = (v) => Math.round(v).toLocaleString('en');

  function draw() {
    if (!result) return;
    const values = result[measure];
    const top = Math.max(1e-9, ...values);
    const feats = result.links.map((xy, k) => {
      const line = [];
      for (let i = 0; i < xy.length; i += 2) line.push([xy[i], xy[i + 1]]);
      return { k, line, v: values[k], crit: result.criticality[k], risk: result.risk[k], depth: result.depth[k] ?? 0 };
    });
    gis.addRibbons(id, feats, {
      line: (f) => f.line, width: (f) => 2.5 + 9 * Math.sqrt(Math.max(0, f.v) / top), z: 1.2, height: ground, order: 4,
      color: (f) => (f.v > 0 ? shade(Math.sqrt(f.v / top)) : ctx.theme === 'night' ? [0.32, 0.27, 0.36] : [0.82, 0.78, 0.8]),
      pick: (f) => `${measure === 'risk' ? 'what the floods cost through it' : 'what losing it would add'}: ${n0(f.v)} person-minutes a trip${f.depth > 0 ? `\ndeepest water on it: ${f.depth.toFixed(2)} m` : ''}`,
    });
  }

  async function compute() {
    if (!latlon) return;
    const me = ++token;
    status.textContent = 'gathering the ways, the people and the buildings of care…';
    let P, H;
    try {
      [P, H] = await Promise.all([peopleAbout(ctx, latlon[0], latlon[1]), hazards(area?.id).catch(() => null)]);
    } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    const ways = waysFor(ctx.world, 'drive');
    const services = buildings(gis).filter((b) => CARE.includes(b.kind)).map((b) => [Math.round(b.x), Math.round(b.y), Math.round(b.floor), b.name || b.kind]);
    if (!services.length) { status.textContent = 'No building here is mapped as care: nothing to reach.'; return; }
    status.textContent = `${n0(P.total)} people, ${services.length} buildings of care, ${ways.length.toLocaleString('en')} ways${H ? `, ${H.name}` : ''}: ranking every road link…`;
    let a;
    try {
      a = await ctx.ask('criticality', { ways, frame: frameOf(frame), people: P.people, services, measure: { threshold: threshold * 60, cap: 7200 },
        hazard: H ? { maps: H.maps } : undefined, curve: 'vehicles', top: 12 });
    } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    result = a;
    draw();
    gis.addPoints(care, services, { position: (s) => [s[0], s[1]], radius: () => 9, height: () => 30, base: () => 0,
      color: () => (ctx.theme === 'night' ? [1, 0.84, 0.6] : [0.62, 0.42, 0.12]), pick: (s) => `${s[3]}: care` });
    const rows = [h('div', {}, h('span', {}, 'people about the place'), h('b', {}, `${n0(P.total)}${P.beyond > 0 ? `; ${n0(P.beyond)} beyond the ways drawn, left out` : ''}`)),
      h('div', {}, h('span', {}, 'road links, junctions'), h('b', {}, `${a.links.length.toLocaleString('en')}, ${a.junctions.toLocaleString('en')}`)),
      h('div', {}, h('span', {}, 'expected loss'), h('b', {}, `${n0(a.expected)} person-minutes added to a trip to care, ${H && H.maps.some((m) => m.return_period > 1) ? "over the year's largest flood" : H ? "under the map's flood (no return period)" : 'in a dry year'}`))];
    readout.replaceChildren(...rows);
    table.replaceChildren(h('table', { class: 'sources' }, h('thead', {}, h('tr', {}, h('th', {}, 'scenario'), h('th', {}, 'weight'), h('th', {}, 'closed'), h('th', {}, 'cut off'), h('th', {}, `beyond ${threshold} min`))),
      h('tbody', {}, a.scenarios.map((s) => h('tr', {}, h('th', {}, s.name), h('td', {}, s.weight.toFixed(4)), h('td', {}, String(s.closed)), h('td', {}, n0(s.isolated)), h('td', {}, n0(s.beyond)))))));
    status.textContent = `Each road link coloured by ${measure === 'risk' ? 'what the floods cost through it' : 'what losing it would add'}: person-minutes added to one trip to care by everyone (a person cut off counted at two hours), expected over the year's largest flood. Cars stop in 30 cm of water (Pregnolato et al. 2017).${H ? '' : ' No hazard map is held here: a dry year alone.'}`;
  }

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'Which roads matter most to reaching care'),
    h('div', { class: 'gs-row' }, h('span', {}, 'colour by'), keys([['risk', 'what the floods cost'], ['criticality', 'what losing it adds']], measure, (v) => { measure = v; draw(); })),
    h('div', { class: 'gs-row' }, h('span', {}, 'beyond reach'), keys([[15, '15 min'], [30, '30 min'], [60, '60 min']], threshold, (v) => { threshold = v; compute(); })),
    readout, key, table, status,
    h('div', { class: 'ld-quiet' }, 'Roads and buildings: Overture Maps, OpenStreetMap. People: GHS-POP R2023A (EC JRC, CC BY 4.0). Floods: the data library\'s maps of this place. The method: risk management\'s criticality on urban risk management\'s lifelines (after the World Bank\'s Lifelines, 2019).'));
  ctx.onDispose(() => { dead = true; });
  return {
    legend,
    pickAt(lat, lon) {
      latlon = [lat, lon];
      compute();
      return { kind: 'place', title: 'Which roads matter most', lines: ['to reaching care'], at: { lat, lon } };
    },
    dispose() { dead = true; gis.remove(id); gis.remove(care); },
  };
}
