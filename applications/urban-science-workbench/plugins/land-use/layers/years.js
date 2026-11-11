// A change, and the years after it (ADR 0094): pick a place, and its people on GHS-POP's 1 km cells as households, the floor of
// its workplaces as jobs, and its classified roads at their speeds and capacities go to the engine. Changes are then placed on the
// map (a road closed or widened, more dwellings allowed in a cell, jobs added in one) and the years are run as things stand and with
// the changes (urban science's luti, through the API's composition): each year dwellings follow last year's rents, households
// choose where to live by a market in which each place is valued for its access to jobs, and the journeys to work settle on the
// congested roads. The cells take the colour of what the changes do to their households, the roads of their load in the peak hour,
// and the years play on the map. Workplaces are what Overture and OpenStreetMap say a building is used for; where a place maps no
// uses, the jobs are taken where people live, and the card says so.

import { colourMap } from '/visualization/styles/index.js';
import { tileOf } from '/visualization/js/gis/world.js';

const KMH = (v) => v / 3.6;
// Roads: free-flow speed and capacity a direction in vehicles an hour, by class (declared, of the order of the Highway Capacity
// Manual's per lane times a class's lanes). The strategic network of land-use models: local streets are reached by walking to it.
export const ROADS = {
  motorway: [KMH(90), 4000], motorway_link: [KMH(50), 1500], trunk: [KMH(70), 3000], trunk_link: [KMH(40), 1200],
  primary: [KMH(50), 1800], primary_link: [KMH(35), 900], secondary: [KMH(40), 1200], secondary_link: [KMH(30), 700],
  tertiary: [KMH(35), 900], tertiary_link: [KMH(25), 500],
};
// Buildings whose use makes them workplaces, and the floor a job takes (declared).
export const WORK = new Set(['commercial', 'office', 'retail', 'industrial', 'warehouse', 'supermarket', 'shop', 'hospital', 'clinic', 'school',
  'university', 'college', 'kindergarten', 'civic', 'government', 'public', 'hotel', 'manufacture', 'factory', 'service', 'transportation', 'education', 'medical']);
export const FLOOR_PER_JOB = 40;   // m²
export const PERSONS = 2.5;        // a household
const CHANGES = {
  widen: { name: 'widen a road', on: 'road' },
  close: { name: 'close a road', on: 'road' },
  build: { name: 'allow more dwellings', on: 'cell' },
  jobs: { name: 'add jobs', on: 'cell' },
};

// Each workplace's floor (footprint × storeys of 3.2 m) given to the nearest cell within a kilometre.
function workplaces(gis, cells) {
  const jobs = new Array(cells.length).fill(0);
  let mapped = 0;
  const bucket = new Map(), key = (x, y) => `${Math.floor(x / 1000)},${Math.floor(y / 1000)}`;
  cells.forEach((c, i) => { const k = key(c[0], c[1]); if (!bucket.has(k)) bucket.set(k, []); bucket.get(k).push(i); });
  const add = (ring, height, kind) => {
    if (!WORK.has(String(kind || '').toLowerCase())) return;
    let a = 0, cx = 0, cy = 0;
    for (let i = 0, n = ring.length; i < n; i++) {
      const [x0, y0] = ring[i], [x1, y1] = ring[(i + 1) % n];
      const c = x0 * y1 - x1 * y0;
      a += c, cx += (x0 + x1) * c, cy += (y0 + y1) * c;
    }
    if (Math.abs(a) < 1e-9) return;
    cx /= 3 * a, cy /= 3 * a;
    let best = -1, d = 1e3;
    for (let di = -1; di <= 1; di++) for (let dj = -1; dj <= 1; dj++)
      for (const i of bucket.get(`${Math.floor(cx / 1000) + di},${Math.floor(cy / 1000) + dj}`) || []) {
        const e = Math.hypot(cells[i][0] - cx, cells[i][1] - cy);
        if (e < d) d = e, best = i;
      }
    if (best < 0) return;
    const floor = (Math.abs(a) / 2) * Math.max(1, Math.round((height || 3.2) / 3.2));
    jobs[best] += floor / FLOOR_PER_JOB;
    mapped++;
  };
  for (const [, layer] of gis.layers) {
    if (layer.kind === 'solids' && layer.features?.length && Array.isArray(layer.features[0].footprint))
      for (const f of layer.features) add(f.footprint, f.height_m, f.kind);
    if (layer.kind === 'built-solids' && layer.built?.starts) {
      const { rings, starts, features } = layer.built;
      for (let k = 0; k < starts.length / 2; k++) {
        const s = starts[2 * k], n = starts[2 * k + 1], f = features?.[k] || [];
        const ring = [];
        for (let i = 0; i < n; i++) ring.push([rings[2 * (s + i)], rings[2 * (s + i) + 1]]);
        add(ring, f[0], f[3]);
      }
    }
  }
  return { jobs, mapped };
}

function distanceToLine(x, y, xy) {
  let best = Infinity;
  for (let i = 0; i + 3 < xy.length; i += 2) {
    const ax = xy[i], ay = xy[i + 1], bx = xy[i + 2], by = xy[i + 3], dx = bx - ax, dy = by - ay, L = dx * dx + dy * dy;
    const t = L > 0 ? Math.max(0, Math.min(1, ((x - ax) * dx + (y - ay) * dy) / L)) : 0;
    best = Math.min(best, Math.hypot(x - ax - t * dx, y - ay - t * dy));
  }
  return best;
}

export async function layer(ctx) {
  const { gis, frame, world, earth, dom: { h } } = ctx;
  const roadsId = ctx.layerId(':roads'), marksId = ctx.layerId(':marks');
  const signed = colourMap('sea-brick'), loadMap = colourMap('gold').colour;
  let place = null, changes = [], tool = 'widen', years = 10, jobsAdded = 10000, result = null, shown = 0, playing = null, dead = false, token = 0, draped = false, picking = false;
  const status = h('div', { class: 'ld-quiet' }, 'Pick a place: its people, workplaces and roads.');
  const readout = h('div', { class: 'gs-readout' });
  const chart = h('div', {});
  const listed = h('div', { class: 'ld-quiet' });
  const css = (rgb) => `rgb(${rgb.map((v) => Math.round(255 * v)).join(',')})`;
  const n = (v) => Math.round(v).toLocaleString('en');
  const pct = (v, d = 1) => `${v >= 0 ? '+' : '−'}${Math.abs(100 * v).toFixed(d)}%`;
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));
  const yearSlider = h('input', { type: 'range', min: 0, max: years, value: 0, step: 1, style: 'width:100%', oninput: (e) => show(Number(e.target.value)) });
  const yearLabel = h('b', {}, 'year 0');
  const keyEnds = h('div', { class: 'ramp-ends' }, h('span', {}, 'fewer households'), h('span', {}, 'as things stand'), h('span', {}, 'more'));
  const playKey = h('button', { onclick: () => play() }, 'play');

  async function gather(lat, lon) {
    const me = ++token;
    const lines = world.lines().filter((l) => !l.rail && ROADS[l.cls]);
    const loaded = new Set();
    for (const [k, t] of world.tiles) if (t.lines) loaded.add(k);
    if (!lines.length || !loaded.size) { status.textContent = 'The roads here are still coming: try again in a moment.'; return; }
    status.textContent = 'gathering the people, the workplaces and the roads…';
    let pop;
    try {
      const r = await fetch(`/world/population?lat=${lat}&lon=${lon}&km=6`);
      pop = await r.json();
      if (!r.ok) throw new Error(pop.message || r.status);
    } catch (e) { status.textContent = `No population here: ${e.message || e}`; return; }
    if (me !== token || dead) return;
    const cells = [], where = [];
    for (const [la, lo, people] of pop.cells) {
      if (!loaded.has(tileOf(la, lo).join('/')) || !(people > 0)) continue;
      const p = frame.toLocal(la, lo);
      cells.push([Math.round(p[0]), Math.round(p[1]), people / PERSONS]);
      where.push([la, lo]);
    }
    if (cells.length < 2) { status.textContent = 'Too few people live within the roads drawn here.'; return; }
    const { jobs, mapped } = workplaces(gis, cells);
    const ways = lines.map((l) => {
      const xy = [];
      for (let i = 0; i < l.xyz.length; i += 3) xy.push(Math.round(l.xyz[i]), Math.round(l.xyz[i + 1]));
      return { cls: l.cls, xy, speed: ROADS[l.cls][0], capacity: ROADS[l.cls][1] };
    });
    place = { lat, lon, cells, where, jobs, mapped, ways, degrees: pop.cell_degrees };
    changes = [];
    result = null;
    listChanges();
    status.textContent = `${cells.length} cells, ${n(cells.reduce((t, c) => t + c[2], 0))} households, ${mapped ? `${n(jobs.reduce((t, j) => t + j, 0))} jobs on ${n(mapped)} workplaces` : 'no workplaces mapped (jobs taken where people live)'}, ${ways.length} roads. Now place a change on the map, and run.`;
    picking = true;
    drawMarks();
  }

  function listChanges() {
    listed.replaceChildren(...(changes.length ? changes.map((c, k) => h('div', {}, `${k + 1}. ${CHANGES[c.kind].name}: ${c.label}`)) : [h('div', {}, 'No change placed yet.')]));
  }

  function addChange(x, y) {
    const C = CHANGES[tool];
    if (C.on === 'road') {
      let best = -1, d = 120;
      place.ways.forEach((w, k) => { const e = distanceToLine(x, y, w.xy); if (e < d) d = e, best = k; });
      if (best < 0) { status.textContent = 'No classified road within 120 m of that point.'; return; }
      changes = changes.filter((c) => !(CHANGES[c.kind].on === 'road' && c.index === best));
      changes.push({ kind: tool, index: best, label: `a ${place.ways[best].cls.replace('_', ' ')} road`, at: [x, y] });
    } else {
      let best = -1, d = 800;
      place.cells.forEach((c, k) => { const e = Math.hypot(c[0] - x, c[1] - y); if (e < d) d = e, best = k; });
      if (best < 0) { status.textContent = 'No cell of people within 800 m of that point.'; return; }
      changes = changes.filter((c) => !(c.kind === tool && c.index === best));
      changes.push({ kind: tool, index: best, amount: tool === 'jobs' ? jobsAdded : 2,
        label: `${tool === 'jobs' ? `${n(jobsAdded)} jobs in ` : ''}the cell of ${n(place.cells[best][2])} households`, at: [place.cells[best][0], place.cells[best][1]] });
    }
    listChanges();
    drawMarks();
    status.textContent = 'Place another change, or run the years.';
  }

  async function run() {
    if (!place) { status.textContent = 'Pick a place first.'; return; }
    const me = ++token;
    const scenario = { year: 1, closed: [], widened: [], built: [], jobs: [] };
    for (const c of changes) {
      if (c.kind === 'close') scenario.closed.push(c.index);
      if (c.kind === 'widen') scenario.widened.push([c.index, 2]);
      if (c.kind === 'build') scenario.built.push([c.index, 2]);
      if (c.kind === 'jobs') scenario.jobs.push([c.index, c.amount]);
    }
    const zones = place.cells.map((c, i) => [c[0], c[1], c[2], place.mapped ? place.jobs[i] : 0]);
    status.textContent = `running ${years} years as things stand and with ${changes.length || 'no'} change${changes.length === 1 ? '' : 's'}…`;
    let a;
    try {
      a = await ctx.ask('run', { ways: place.ways.map((w) => [w.speed, w.capacity, ...w.xy]), zones, years, scenario, parameters: {} });
    } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    result = a;
    yearSlider.max = years;
    show(years);
    const reached = Math.max(...a.changed.map((y) => y.gap), ...a.plain.map((y) => y.gap));
    status.textContent = `${years} years in ${a.seconds.toFixed(1)} s; the network of ${n(a.network.chains)} chains between ${n(a.network.nodes)} junctions; the journeys' equilibrium each year to a gap of ${reached.toExponential(0)} or less. The cells: what the changes do to their households; the roads: their load in the peak hour.`;
    drawChart();
  }

  function show(y) {
    if (!result || !place) return;
    shown = y;
    yearSlider.value = y;
    yearLabel.textContent = `year ${y}`;
    const P = result.plain[y], C = result.changed[y];
    // The scale: the largest change of a cell's households over all the years, however small, so that the years compare; the key
    // gives its ends.
    let top = 1e-6;
    for (let t = 0; t < result.changed.length; t++)
      for (let i = 0; i < place.cells.length; i++) top = Math.max(top, Math.abs(result.changed[t].zones[i][0] / result.plain[t].zones[i][0] - 1));
    const [dx, dy] = place.degrees;
    let s0 = Infinity, w0 = Infinity, n0 = -Infinity, e0 = -Infinity;
    for (const [la, lo] of place.where) s0 = Math.min(s0, la), n0 = Math.max(n0, la), w0 = Math.min(w0, lo), e0 = Math.max(e0, lo);
    const width = Math.round((e0 - w0) / dx) + 1, height = Math.round((n0 - s0) / dy) + 1, bytes = new Uint8Array(width * height);
    // A cell the changes leave within 3% of the scale's end stays clear, so that the cells they move stand out from the imagery.
    place.where.forEach(([la, lo], i) => {
      const d = C.zones[i][0] / P.zones[i][0] - 1;
      if (Math.abs(d) < 0.03 * top) return;
      bytes[Math.round((la - s0) / dy) * width + Math.round((lo - w0) / dx)] = 3 + Math.round(126 + 126 * Math.max(-1, Math.min(1, d / top)));
    });
    earth.setDrape({ data: bytes, width, height, box: [s0 - dy / 2, w0 - dx / 2, n0 + dy / 2, e0 + dx / 2], nearest: true, opacity: 1, floor: 2.5 / 255,
      ramp: (u) => { const q = Math.round(255 * u); return q < 3 ? [0, 0, 0, 0] : [...signed.colour((q - 3) / 252), 0.8]; } });
    draped = true;
    keyEnds.replaceChildren(h('span', {}, `${pct(-top, top < 0.01 ? 2 : 1)} households`), h('span', {}, 'as things stand'), h('span', {}, `${pct(top, top < 0.01 ? 2 : 1)}`));
    // The roads by their load in the peak hour, with the scenario's: broad enough to be seen at a city's scale.
    const load = new Map(C.loads.map(([k, v]) => [k, v]));
    const shownRoads = [];
    place.ways.forEach((w, k) => { if (load.has(k)) shownRoads.push({ k, v: load.get(k), w }); });
    const closed = new Set(changes.filter((c) => c.kind === 'close').map((c) => c.index));
    const widened = new Set(changes.filter((c) => c.kind === 'widen').map((c) => c.index));
    const night = ctx.theme === 'night';
    gis.addRibbons(roadsId, shownRoads, {
      line: (f) => { const l = []; for (let i = 0; i < f.w.xy.length; i += 2) l.push([f.w.xy[i], f.w.xy[i + 1]]); return l; },
      width: (f) => (widened.has(f.k) && y >= 1 ? 90 : 25 + 55 * Math.min(1, f.v)),
      color: (f) => (closed.has(f.k) && y >= 1 ? (night ? [0.95, 0.36, 0.46] : [0.45, 0.08, 0.2]) : loadMap(Math.min(1, f.v / 1.2))), z: 0.8, order: 3 });
    // The card.
    const sum = (Y) => Y.zones.reduce((t, z) => t + z[0], 0);
    const gains = place.cells.map((c, i) => [i, C.zones[i][0] - P.zones[i][0]]).sort((a, b) => b[1] - a[1]);
    const rows = [
      h('div', {}, h('span', {}, 'households living here'), h('b', {}, `${n(sum(C))} with the changes, ${n(sum(P))} as things stand (${pct(sum(C) / sum(P) - 1, 2)})`)),
      h('div', {}, h('span', {}, 'the mean journey to work'), h('b', {}, `${C.commute.toFixed(1)} min, against ${P.commute.toFixed(1)}`)),
      h('div', {}, h('span', {}, 'the cell gaining most'), h('b', {}, `${gains[0][1] >= 0 ? '+' : ''}${n(gains[0][1])} households; its rent ${C.zones[gains[0][0]][1].toFixed(2)} (100 in year 0)`)),
      h('div', {}, h('span', {}, 'the cell losing most'), h('b', {}, `${n(gains[gains.length - 1][1])} households`)),
    ];
    readout.replaceChildren(...rows);
  }

  function drawChart() {
    if (!result) return;
    const W = 260, H = 76, T = result.changed.length - 1;
    const series = [
      { name: 'households living here', v: result.changed.map((c, t) => c.located / result.plain[t].located - 1), colour: night() ? '#e7a7b9' : '#8e3b78' },
      { name: 'the mean journey to work', v: result.changed.map((c, t) => c.commute / result.plain[t].commute - 1), colour: night() ? '#e9c36a' : '#965208' },
    ];
    let lo = 0, hi = 0;
    for (const s of series) for (const v of s.v) lo = Math.min(lo, v), hi = Math.max(hi, v);
    if (hi - lo < 1e-6) hi = lo + 1e-6;
    const X = (t) => 8 + ((W - 16) * t) / Math.max(1, T), Y = (v) => H - 14 - ((H - 24) * (v - lo)) / (hi - lo);
    const svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" style="display:block">
      <line x1="8" x2="${W - 8}" y1="${Y(0)}" y2="${Y(0)}" stroke="currentColor" stroke-opacity="0.35" stroke-width="0.6"/>
      ${series.map((s) => `<polyline fill="none" stroke="${s.colour}" stroke-width="1.6" points="${s.v.map((v, t) => `${X(t).toFixed(1)},${Y(v).toFixed(1)}`).join(' ')}"/>`).join('')}
      <text x="8" y="${H - 2}" font-size="9" fill="currentColor" opacity="0.7">year 0</text><text x="${W - 8}" y="${H - 2}" font-size="9" text-anchor="end" fill="currentColor" opacity="0.7">year ${T}</text>
    </svg>`;
    const box = h('div', {});
    box.innerHTML = svg;
    chart.replaceChildren(box, h('div', { class: 'ld-quiet' }, ...series.map((s) => h('span', { style: `color:${s.colour};margin-right:10px` }, `— ${s.name}`)), ' with the changes, against things as they stand'));
  }
  const night = () => ctx.theme === 'night';

  function drawMarks() {
    if (!place) { gis.remove(marksId); return; }
    const marks = changes.filter((c) => CHANGES[c.kind].on === 'cell');
    if (!marks.length) { gis.remove(marksId); return; }
    gis.addPoints(marksId, marks, { position: (c) => c.at, radius: () => 60, base: () => 0, height: () => 40,
      color: (c) => (c.kind === 'jobs' ? (night() ? [1, 0.84, 0.6] : [0.62, 0.42, 0.12]) : (night() ? [0.8, 0.7, 1] : [0.37, 0.23, 0.55])), pick: (c) => `${CHANGES[c.kind].name}: ${c.label}` });
  }

  function play() {
    if (!result) return;
    if (playing) { clearInterval(playing); playing = null; playKey.textContent = 'play'; return; }
    playKey.textContent = 'pause';
    let y = shown >= years ? 0 : shown;
    playing = setInterval(() => {
      if (dead) { clearInterval(playing); return; }
      show(y);
      y = y >= years ? 0 : y + 1;
    }, 900);
  }

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'A change, and the years after it'),
    h('div', { class: 'gs-row' }, h('span', {}, 'change'), keys(Object.entries(CHANGES).map(([k, v]) => [k, v.name]), tool, (v) => { tool = v; })),
    listed,
    h('div', { class: 'gs-row' }, h('span', {}, 'jobs added'), keys([[2000, '2,000'], [10000, '10,000'], [40000, '40,000']], jobsAdded, (v) => { jobsAdded = v; })),
    h('div', { class: 'gs-row' }, h('span', {}, 'years'), keys([[10, '10'], [20, '20']], years, (v) => { years = v; })),
    h('div', { class: 'keys-tight' }, h('button', { onclick: () => run() }, 'run the years'), h('button', { onclick: () => { changes = []; listChanges(); drawMarks(); } }, 'clear the changes'),
      h('button', { onclick: () => { picking = false; place = null; result = null; status.textContent = 'Pick a place: its people, workplaces and roads.'; } }, 'another place')),
    h('div', { class: 'gs-row' }, yearLabel, playKey), yearSlider,
    readout, chart,
    h('div', {},
      h('span', { class: 'ramp', style: `background:${signed.gradient()}` }),
      keyEnds),
    status,
    h('div', { class: 'ld-quiet' }, `Declared: ${PERSONS} people a household, ${FLOOR_PER_JOB} m² of floor a job, a tenth of the workers on the roads in the peak hour, roads' speeds and capacities by class, the rules allowing half again the dwellings that stand, building after last year's rents. A widened road doubles its capacity; more dwellings doubles a cell's; jobs are added as chosen.`),
    h('div', { class: 'ld-quiet' }, 'People: GHS-POP R2023A (EC JRC, CC BY 4.0). Buildings and roads: Overture Maps, OpenStreetMap. The model after Lowry (1964), Wilson (1970), Evans (1976) and Wegener (2004), written here (urban science\'s luti, housing and access; nomos\'s interaction and routing games).'));
  ctx.onDispose(() => { dead = true; if (playing) clearInterval(playing); });
  return {
    legend,
    pickAt(lat, lon) {
      const p = frame.toLocal(lat, lon);
      if (!place || !picking) {
        gather(lat, lon);
        return { kind: 'place', title: 'A change, and the years after it', lines: ['gathering the place'], at: { lat, lon } };
      }
      addChange(p[0], p[1]);
      return { kind: 'place', title: CHANGES[tool].name, lines: [changes.length ? changes[changes.length - 1].label : ''], at: { lat, lon } };
    },
    figures() {
      if (!result) return [];
      const x = result.changed.map((c) => c.year);
      return [{ chart: 'line', title: 'What the changes do, year by year', xlabel: 'year', ylabel: '% against things as they stand',
        series: [{ name: 'households living here', x, y: result.changed.map((c, t) => 100 * (c.located / result.plain[t].located - 1)) },
          { name: 'the mean journey to work', x, y: result.changed.map((c, t) => 100 * (c.commute / result.plain[t].commute - 1)) }] }];
    },
    dispose() { dead = true; if (playing) clearInterval(playing); if (draped) earth.setDrape(null); gis.remove(roadsId); gis.remove(marksId); },
  };
}
