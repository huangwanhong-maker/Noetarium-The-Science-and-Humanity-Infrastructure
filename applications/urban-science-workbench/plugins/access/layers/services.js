// Access to services, and who is left without (ADR 0086): pick a place, and its people on the 1 km cells of GHS-POP within reach, the
// buildings of a kind (hospitals and clinics, schools, shops) with their floor space as what they offer, and the ways at a mode's speeds
// go to the engine. Each cell is coloured by its access (the two-step floating catchment: floor space shared among the people who can
// reach it, per person), the cells with nothing within the limit stand apart, and the card gives who is left without, how unequal access
// is, and the Rural Access Index where people live in the countryside. Facilities are what Overture and OpenStreetMap say a building is:
// where a hospital is mapped as a plain building, it is missed, and the card's figures are of what is mapped.

import { colourMap } from '/visualization/styles/index.js';
import { MODES } from './reach.js';
import { tileBox, tileOf } from '/visualization/js/gis/world.js';

const SERVICES = {
  health: { name: 'health', kinds: ['hospital', 'clinic', 'doctors', 'medical', 'healthcare', 'dentist'], unit: 'm² of care' },
  schools: { name: 'schools', kinds: ['school', 'kindergarten', 'college', 'university', 'education'], unit: 'm² of teaching' },
  shops: { name: 'shops', kinds: ['retail', 'supermarket', 'shop', 'marketplace'], unit: 'm² of shops' },
};
const MINUTES = [10, 15, 30];
const ALL_SEASON = new Set(['motorway', 'motorway_link', 'trunk', 'trunk_link', 'primary', 'primary_link', 'secondary', 'secondary_link', 'tertiary', 'tertiary_link']);

// The buildings of some kinds within a box [x0, y0, x1, y1], with their centroids and floor space (footprint × storeys of 3.2 m).
function facilities(gis, box, kinds) {
  const want = new Set(kinds), out = [];
  const add = (ring, height, kind, name, roof) => {
    if (!want.has(String(kind || '').toLowerCase())) return;
    let a = 0, cx = 0, cy = 0;
    for (let i = 0, n = ring.length; i < n; i++) {
      const [x0, y0] = ring[i], [x1, y1] = ring[(i + 1) % n];
      const c = x0 * y1 - x1 * y0;
      a += c, cx += (x0 + x1) * c, cy += (y0 + y1) * c;
    }
    if (Math.abs(a) < 1e-9) return;
    cx /= 3 * a, cy /= 3 * a;
    if (cx < box[0] || cx > box[2] || cy < box[1] || cy > box[3]) return;
    const storeys = Math.max(1, Math.round((height || 3.2) / 3.2));
    out.push({ x: cx, y: cy, capacity: (Math.abs(a) / 2) * storeys, kind, name, roof: roof ?? height ?? 0 });
  };
  for (const [, layer] of gis.layers) {
    if (layer.kind === 'solids' && layer.features?.length && Array.isArray(layer.features[0].footprint))
      for (const f of layer.features) add(f.footprint, f.height_m, f.kind, f.name, f.height_m);
    if (layer.kind === 'built-solids' && layer.built?.starts) {
      const { rings, starts, features, boxes } = layer.built;
      for (let k = 0; k < starts.length / 2; k++) {
        const s = starts[2 * k], n = starts[2 * k + 1], f = features?.[k] || [];
        const ring = [];
        for (let i = 0; i < n; i++) ring.push([rings[2 * (s + i)], rings[2 * (s + i) + 1]]);
        add(ring, f[0], f[3], f[2], boxes ? boxes[5 * k + 4] : f[0]);
      }
    }
  }
  return out;
}

export async function layer(ctx) {
  const { gis, frame, world, earth, dom: { h } } = ctx;
  const sitesId = ctx.layerId(':sites');
  let draped = false;
  const map = colourMap(ctx.theme === 'night' ? 'plum' : 'orchid').colour;
  // Nothing within reach is hatched, in a rose red by night and a deep wine by day: apart from the ramp's plum and cream, and from the
  // palette's rose in which the imagery is graded.
  const without = ctx.theme === 'night' ? [0.95, 0.36, 0.46] : [0.45, 0.08, 0.2];
  let service = 'health', mode = 'foot', minutes = 15, centre = null, latlon = null, dead = false, token = 0;
  const status = h('div', { class: 'ld-quiet' }, 'Pick a place: who can reach what.');
  const readout = h('div', { class: 'gs-readout' });
  // The key: access against the region's floor space per person, from none to twice it; and the colour of nothing within reach.
  const css = (rgb) => `rgb(${rgb.map((v) => Math.round(255 * v)).join(',')})`;
  const shade = (u) => map(ctx.theme === 'night' ? 0.15 + 0.85 * u : 0.1 + 0.9 * u);
  const key = h('div', {},
    h('span', { class: 'ramp', style: `background:linear-gradient(90deg, ${Array.from({ length: 11 }, (_, i) => css(shade(i / 10))).join(',')})` }),
    h('div', { class: 'ramp-ends' }, h('span', {}, 'little'), h('span', {}, "the region's mean"), h('span', {}, 'twice it')),
    h('div', { class: 'ld-quiet', style: 'display:flex;align-items:center;gap:6px;margin-top:3px' },
      h('i', { style: `display:inline-block;width:14px;height:9px;background:repeating-linear-gradient(135deg, ${css(without)} 0 1.5px, transparent 1.5px 4px)` }), 'nothing within reach, hatched'));
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));

  async function compute() {
    if (!centre) return;
    const me = ++token;
    const M = MODES[mode], S = SERVICES[service];
    // The ways loaded about the view bound everything: people beyond them would be joined to the network far away and read as without,
    // so only the cells in a tile whose ways have come are kept (to 25 km from the place), and the people of the others are counted
    // as left out.
    const lines = world.lines();
    const loaded = new Set();
    let reachKm = 0;
    for (const [k, t] of world.tiles) {
      if (!t.lines) continue;
      loaded.add(k);
      const [s0, w0, n0, e0] = tileBox(t.x, t.y);
      const dLat = Math.max(Math.abs(n0 - latlon[0]), Math.abs(s0 - latlon[0])), dLon = Math.max(Math.abs(e0 - latlon[1]), Math.abs(w0 - latlon[1]));
      reachKm = Math.max(reachKm, dLat * 111.32, dLon * 111.32 * Math.cos((latlon[0] * Math.PI) / 180));
    }
    if (!lines.length || !loaded.size) { status.textContent = 'The ways here are still coming: try again in a moment.'; return; }
    const box = [Infinity, Infinity, -Infinity, -Infinity];
    for (const l of lines)
      for (let i = 0; i < l.xyz.length; i += 3) {
        const x = l.xyz[i], y = l.xyz[i + 1];
        if (x < box[0]) box[0] = x;
        if (y < box[1]) box[1] = y;
        if (x > box[2]) box[2] = x;
        if (y > box[3]) box[3] = y;
      }
    const km = Math.min(25, Math.max(1.5, reachKm));
    status.textContent = 'gathering the people and the ways…';
    let pop;
    try {
      const r = await fetch(`/world/population?lat=${latlon[0]}&lon=${latlon[1]}&km=${km.toFixed(2)}`);
      pop = await r.json();
      if (!r.ok) throw new Error(pop.message || r.status);
    } catch (e) { status.textContent = `No population here: ${e.message || e}`; return; }
    if (me !== token || dead) return;
    const demand = [], where = [];
    let beyond = 0;
    for (const [la, lo, people, cls] of pop.cells) {
      if (!loaded.has(tileOf(la, lo).join('/'))) { beyond += people; continue; }
      const p = frame.toLocal(la, lo);
      demand.push([Math.round(p[0]), Math.round(p[1]), people, cls]);
      where.push([la, lo]);
    }
    if (!demand.length) { status.textContent = 'No one lives within the ways drawn here.'; return; }
    const supply = facilities(gis, box, S.kinds);
    const ways = [], roads = [];
    for (const l of lines) {
      const row = [M.speed(l.cls, l.rail)], xy = [];
      for (let i = 0; i < l.xyz.length; i += 3) xy.push(Math.round(l.xyz[i]), Math.round(l.xyz[i + 1]));
      ways.push(row.concat(xy));
      if (ALL_SEASON.has(l.cls)) roads.push(xy);
    }
    status.textContent = `${demand.length.toLocaleString('en')} cells of people, ${supply.length.toLocaleString('en')} buildings of ${S.name}…`;
    let a;
    try {
      a = await ctx.ask('services', { ways, demand, supply: supply.map((f) => [Math.round(f.x), Math.round(f.y), Math.round(f.capacity)]), limit: minutes * 60, walk: M.walk, roads, decay: 'gaussian' });
    } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    // The cells, draped on the ground as GHS-POP's own grid (rows from the south): 0 no one or left out, 1 nothing within reach (drawn
    // as hatching by the Earth's shader), 3…255 access from none to twice the region's floor space per person. The ramp is read
    // between its texels, so 2 is left unused and texel 0 takes the hatching's colour: no value is blended with another meaning.
    const [dx, dy] = pop.cell_degrees;
    let s0 = Infinity, w0 = Infinity, n0 = -Infinity, e0 = -Infinity;
    for (const [la, lo] of pop.cells) s0 = Math.min(s0, la), n0 = Math.max(n0, la), w0 = Math.min(w0, lo), e0 = Math.max(e0, lo);
    const width = Math.round((e0 - w0) / dx) + 1, height = Math.round((n0 - s0) / dy) + 1, bytes = new Uint8Array(width * height);
    const regional = a.summary.regional || 1;
    where.forEach(([la, lo], i) => {
      const A = a.cells[i] ? a.cells[i][0] : 0;
      bytes[Math.round((la - s0) / dy) * width + Math.round((lo - w0) / dx)] = A > 0 ? 3 + Math.round(252 * Math.min(1, A / (2 * regional))) : 1;
    });
    earth.setDrape({ data: bytes, width, height, box: [s0 - dy / 2, w0 - dx / 2, n0 + dy / 2, e0 + dx / 2], nearest: true, opacity: 1, floor: 0.5 / 255, hatch: 1,
      ramp: (u) => { const q = Math.round(255 * u); return q <= 1 ? [...without, 0.75] : [...shade(Math.max(0, q - 3) / 252), 0.6]; } });
    draped = true;
    if (supply.length) {
      const top = Math.max(...supply.map((f) => f.capacity));
      gis.addPoints(sitesId, supply, { position: (f) => [f.x, f.y], radius: (f) => 6 + 28 * Math.sqrt(f.capacity / top), base: (f) => f.roof, height: () => 24,
        color: () => (ctx.theme === 'night' ? [1, 0.84, 0.6] : [0.62, 0.42, 0.12]), pick: (f) => `${f.name || f.kind}: ${Math.round(f.capacity).toLocaleString('en')} m² of floor` });
    } else gis.remove(sitesId);
    const s = a.summary, pct = (v) => `${Math.round(100 * v)}%`, n = (v) => Math.round(v).toLocaleString('en');
    const rows = [h('div', {}, h('span', {}, 'people within the ways drawn'), h('b', {}, `${n(demand.reduce((t, c) => t + c[2], 0))}${beyond > 0 ? `; ${n(beyond)} beyond, left out` : ''}`))];
    if (supply.length && s.people) {
      rows.push(
        h('div', {}, h('span', {}, `within ${minutes} min ${M.name}`), h('b', {}, `${pct(1 - s.shareWithout)} of them`)),
        h('div', {}, h('span', {}, 'nothing within reach'), h('b', {}, `${n(s.without)} people (${pct(s.shareWithout)})`)),
        h('div', {}, h('span', {}, 'below half the mean'), h('b', {}, `${n(s.shortOf)} people (${pct(s.shareShort)})`)),
        h('div', {}, h('span', {}, 'access, person-weighted'), h('b', {}, `${s.meanAccess.toFixed(2)} ${S.unit} a person; Gini ${s.gini.toFixed(2)}`)));
    } else rows.push(h('div', {}, h('span', {}, `buildings of ${S.name}`), h('b', {}, 'none mapped here')));
    if (a.rai && a.rai.index != null) rows.push(h('div', {}, h('span', {}, 'Rural Access Index'), h('b', {}, `${pct(a.rai.index)} of ${n(a.rai.rural)} rural people within 2 km of a main road`)));
    readout.replaceChildren(...rows);
    status.textContent = `Cells by their ${S.name} within ${minutes} minutes ${M.name}, shared among all who can reach it (the two-step floating catchment); hatched, nothing within reach. ${supply.length} buildings mapped as ${S.name}.`;
  }

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'Access to services, and who is left without'),
    h('div', { class: 'gs-row' }, h('span', {}, 'service'), keys(Object.entries(SERVICES).map(([k, v]) => [k, v.name]), service, (v) => { service = v; compute(); })),
    h('div', { class: 'gs-row' }, h('span', {}, 'how'), keys(Object.entries(MODES).map(([k, v]) => [k, v.name]), mode, (v) => { mode = v; compute(); })),
    h('div', { class: 'gs-row' }, h('span', {}, 'within'), keys(MINUTES.map((m) => [m, `${m} min`]), minutes, (v) => { minutes = v; compute(); })),
    readout, key, status,
    h('div', { class: 'ld-quiet' }, 'People: GHS-POP and GHS-SMOD R2023A (EC JRC, CC BY 4.0). Buildings and ways: Overture Maps, OpenStreetMap. The measures after Luo and Wang (2003), Hansen (1959) and the World Bank\'s Rural Access Index.'));
  ctx.onDispose(() => { dead = true; });
  return {
    legend,
    pickAt(lat, lon) {
      const p = frame.toLocal(lat, lon);
      centre = [p[0], p[1]];
      latlon = [lat, lon];
      compute();
      return { kind: 'place', title: 'Who can reach what', lines: [SERVICES[service].name], at: { lat, lon } };
    },
    dispose() { dead = true; if (draped) earth.setDrape(null); gis.remove(sitesId); },
  };
}
