// Walking out of a hazard's reach (ADR 0129): the people within the zone a map gives (Miyako's assumed tsunami, Freetown's 1-in-100-year
// river flood) walk on the ways in view to the designated evacuation sites and to the ground the water does not reach. The engine
// answers who can walk to safety within the warning, the most any routing could bring to safety by every minute (a bound, for
// everyone leaving at once and for the people's departures), and the crowd as it moves, routed to the nearest refuge with room, which
// the map plays with the clock from the moment the layer was asked; and, among the tall buildings in view, which would serve best as
// refuges for those who cannot get out in time.
//
// Declarations, each in the card: walking speed (1.0 m/s, the Fire and Disaster Management Agency's planning value; 0.62 m/s, the mean
// MLIT measured in 2011; 0.5 m/s, its value for those who walk with difficulty), the warning, the departures (the shares of the 2011
// survey by the Cabinet Office, FDMA and JMA, the times when each group left this work's own), 1.3 people a second per metre of width,
// 5.4 per m² at a standstill, and a tall building's refuge: its floors from the third up, a person a square metre.

import { colourMap } from '/visualization/styles/index.js';
import { buildings, frameOf, groundOf, hazards, peopleAbout, waysFor } from '../shared.js';

// The 2011 survey's shares (N = 870): 57% left as soon as the shaking stopped, 31% after doing something first, 11% only as the tsunami
// neared; the times are a declaration (from 2 min, as the FDMA's manual assumes, to 45 min).
const DEPARTURE = { time: [0, 120, 600, 1500, 2400, 2700], share: [0, 0, 0.57, 0.88, 0.99, 1],
  source: 'shares of the 2011 survey by the Cabinet Office, FDMA and JMA (57% at once, 31% after doing something, 11% as the tsunami neared); the times are this work\'s declaration' };
const AT_ONCE = { time: [0, 1e-6], share: [0, 1], source: 'everyone at once' };

export async function layer(ctx) {
  const { gis, frame, earth, area, chart, clock, dom: { h } } = ctx;
  const edgesId = ctx.layerId(':crowd'), sitesId = ctx.layerId(':sites'), lateId = ctx.layerId(':late'), towersId = ctx.layerId(':towers');
  const H = await hazards(area?.id);
  if (!H || !H.zone) throw new Error('No hazard zone is held for this place.');
  const dense = colourMap(ctx.theme === 'night' ? 'plum' : 'orchid').colour;
  let speed = 1.0, warning = 30, departure = 'survey', result = null, dead = false, token = 0, hour0 = clock.hour, shown = -1, playing = null, feats = [], layerHandle = null;
  const status = h('div', { class: 'ld-quiet' }, 'Press “evacuate”: the people in the zone walk to safety.');
  const readout = h('div', { class: 'gs-readout' }), plot = h('div', {}), plan = h('div', { class: 'gs-readout' });
  const n0 = (v) => Math.round(v).toLocaleString('en'), mm = (s) => (Number.isFinite(s) ? `${Math.round(s / 60)} min` : 'beyond the hour');
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));

  // The zone on the ground: the map's depth, drawn faintly.
  const bytes = await fetch(`/data/products/${H.zone.path}`).then((r) => r.arrayBuffer()).then((b) => new Uint8Array(b));
  const sea = colourMap('sea').colour;
  earth.setDrape({ data: bytes, width: H.zone.width, height: H.zone.height, box: H.zone.box, opacity: 0.55, floor: 0.002,
    ramp: (u) => { const d = u * 255 * H.zone.unit; if (d <= 0.02) return [0, 0, 0, 0]; const c = sea(ctx.theme === 'night' ? 0.75 - 0.5 * Math.min(1, d / 8) : 0.25 + 0.5 * Math.min(1, d / 8)); return [c[0], c[1], c[2], 0.5]; } });

  function paint(k) {
    if (!result || !layerHandle || k === shown) return;
    const fr = result.frames[Math.max(0, Math.min(k, result.frames.length - 1))];
    if (!fr) return;
    shown = k;
    const f = fr.density;
    const col = layerHandle.object.geometry.attributes.color;
    for (const [i, feat] of feats.entries()) {
      const [start, count] = layerHandle.ranges[i];
      const d = f[feat.e] || 0;
      // The crowd from a quarter of a person a square metre to two and more, bright against the ground; the empty ways faint.
      const u = Math.min(1, Math.sqrt(Math.max(0, d) / 2));
      const rgb = d > 0.02 ? dense(ctx.theme === 'night' ? 0.45 + 0.55 * u : 0.3 + 0.7 * u) : ctx.theme === 'night' ? [0.22, 0.18, 0.26] : [0.88, 0.85, 0.87];
      for (let v = start; v < start + count; v++) col.setXYZ(v, rgb[0], rgb[1], rgb[2]);
    }
    col.needsUpdate = true;
    clockLine.textContent = `${Math.round(fr.t / 60)} min after the warning`;
  }
  const clockLine = h('div', { class: 'ld-line' }, '');
  const frameAt = (seconds) => {
    if (!result) return 0;
    let k = 0;
    while (k + 1 < result.frames.length && result.frames[k + 1].t <= seconds) k++;
    return k;
  };
  const stop = () => { if (playing) cancelAnimationFrame(playing); playing = null; };
  function play() {
    stop();
    const t0 = performance.now(), seconds = 10, mine = result;
    const step = (now) => {
      if (result !== mine) return;  // a new evacuation replaced the one being played
      const u = Math.min(1, (now - t0) / (seconds * 1000));
      paint(Math.round(u * (result.frames.length - 1)));
      playing = u < 1 && !dead ? requestAnimationFrame(step) : null;
    };
    playing = requestAnimationFrame(step);
  }

  async function compute() {
    const me = ++token;
    stop();
    status.textContent = 'gathering the ways and the people…';
    let P;
    try { P = await peopleAbout(ctx, frame.origin.lat, frame.origin.lon); } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    // Only what lies within the zone's box and a kilometre about it: the people who must leave, and the ground they walk to.
    const [s0, w0, n0b, e0] = H.zone.box, sw = frame.toLocal(s0, w0), ne = frame.toLocal(n0b, e0), pad = 1000;
    const box = [Math.min(sw[0], ne[0]) - pad, Math.min(sw[1], ne[1]) - pad, Math.max(sw[0], ne[0]) + pad, Math.max(sw[1], ne[1]) + pad];
    const inBox = (x, y) => x >= box[0] && x <= box[2] && y >= box[1] && y <= box[3];
    const ways = waysFor(ctx.world, 'walk', speed).filter((r) => { for (let i = 2; i < r.length; i += 2) if (inBox(r[i], r[i + 1])) return true; return false; });
    P.people = P.people.filter((p) => inBox(p[0], p[1]));
    P.total = P.people.reduce((t, p) => t + p[2], 0);
    const shelters = H.shelters.map((s) => { const p = frame.toLocal(s.lat, s.lon); return [Math.round(p[0]), Math.round(p[1]), 0, s.name]; });
    const candidates = P.buildings.filter((b) => b.height >= 12 && inBox(b.x, b.y)).map((b) => [Math.round(b.x), Math.round(b.y), Math.round(b.footprint * Math.max(0, Math.round(b.height / 3.2) - 2)), b.name || `a building of ${Math.round(b.height)} m`])
      .filter((c) => c[2] > 0).slice(0, 60);
    status.textContent = `${n0(P.total)} people about, ${shelters.length} designated refuges, ${ways.length.toLocaleString('en')} ways: walking…`;
    let a;
    try {
      a = await ctx.ask('evacuation', { ways, frame: frameOf(frame), people: P.people, shelters, hazard: { maps: [H.zone] }, safe: 'outside', only_in_zone: true,
        walking: { speed }, departure: departure === 'survey' ? DEPARTURE : AT_ONCE, warning: warning * 60, horizon: Math.max(3600, warning * 120), step: 15, read: 30, frames: 60,
        candidates, k: 3 });
    } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    result = a;
    hour0 = clock.hour;
    shown = -1;
    feats = a.edges.map((e, i) => { const xy = a.links[e.link], line = []; for (let j = 0; j < xy.length; j += 2) line.push([xy[j], xy[j + 1]]); return { e: i, line, width: e.width, peak: a.peak[i] }; });
    layerHandle = gis.addRibbons(edgesId, feats, { line: (f) => f.line, width: (f) => Math.max(3, Math.min(14, 1.4 * f.width)), z: 1.2, height: groundOf(ctx), order: 4, color: () => [0.8, 0.78, 0.8],
      pick: (f) => `the crowd's peak here: ${f.peak.toFixed(2)} people per m²` });
    paint(0);
    gis.addPoints(sitesId, a.shelters, { position: (s) => [s.x, s.y], radius: () => 8, height: () => 26, base: () => 0,
      color: () => (ctx.theme === 'night' ? [0.62, 0.9, 0.72] : [0.16, 0.5, 0.32]), pick: (s) => `${s.name}: a designated refuge` });
    gis.addPoints(lateId, a.late, { position: (p) => [p[0], p[1]], radius: (p) => 4 + 2 * Math.sqrt(p[2]), height: () => 10, base: () => 0,
      color: () => (ctx.theme === 'night' ? [0.95, 0.4, 0.5] : [0.62, 0.12, 0.25]), pick: (p) => `${n0(p[2])} people who cannot walk to safety within ${warning} min${p[3] ? ` (${Math.round(p[3] / 60)} min)` : ''}` });
    const B = a.bound, S = a.simulation;
    readout.replaceChildren(
      h('div', {}, h('span', {}, `people within ${H.zoneName || 'the zone'}`), h('b', {}, n0(a.people))),
      h('div', {}, h('span', {}, `who can walk to safety within ${warning} min`), h('b', {}, `${n0(a.within)} (${Math.round((100 * a.within) / Math.max(1, a.people))}%)`)),
      h('div', {}, h('span', {}, 'at most safe by then, everyone leaving at once'), h('b', {}, n0(Math.min(a.people, B.by_warning_at_once)))),
      h('div', {}, h('span', {}, 'at most safe by then, leaving when they do'), h('b', {}, n0(Math.min(a.people, B.by_warning)))),
      h('div', {}, h('span', {}, 'safe by then in the simulation'), h('b', {}, n0(S.by_warning))),
      h('div', {}, h('span', {}, `${Math.round(100 * S.cleared)}% safe in the simulation, all by the bound`), h('b', {}, `${mm(S.clearance)}; ${mm(B.clearance)}`)));
    const t = a.curve.t.map((x) => x / 60);
    const series = [
      { name: 'leaving, as they do', x: t, y: a.curve.departed, color: chart.category(3), dash: '2 3' },
      { name: 'the bound, everyone at once', x: t, y: a.curve.bound_at_once, color: chart.category(0), dash: '5 3' },
      { name: 'the bound, leaving as they do', x: t, y: a.curve.bound, color: chart.category(1) },
      { name: 'the crowd, simulated', x: t, y: a.curve.simulated, color: chart.category(2), width: 2.2 },
    ];
    plot.replaceChildren(chart.lines(series, { xlabel: 'minutes after the warning', ylabel: 'people safe', width: 400, height: 210, marks: [{ x: warning, label: 'the warning\'s time' }] }), chart.legend(series));
    if (a.plan && a.plan.chosen.length) {
      const chosen = a.plan.chosen.map((c) => candidates[c]);
      gis.addPoints(towersId, chosen, { position: (c) => [c[0], c[1]], radius: () => 12, height: () => 60, base: () => 0,
        color: () => (ctx.theme === 'night' ? [1, 0.84, 0.6] : [0.7, 0.5, 0.15]), pick: (c) => `${c[3]}: a refuge for ${n0(c[2])}, chosen` });
      plan.replaceChildren(h('div', {}, h('span', {}, `${chosen.length} tall buildings as refuges`), h('b', {}, `${n0(a.plan.existing)} → ${n0(a.plan.value.at(-1))} housed within ${warning} min`)),
        h('div', {}, h('span', {}, 'no three could do more than'), h('b', {}, n0(a.plan.bound))));
    } else { gis.remove(towersId); plan.replaceChildren(); }
    status.textContent = `The crowd on the ways, people per m², from the warning; red, those who cannot walk out in time. Walking at ${speed} m/s; departures: ${(departure === 'survey' ? DEPARTURE : AT_ONCE).source}.`;
    play();
  }

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'Walking out of the hazard\'s reach'),
    h('div', { class: 'gs-row' }, h('span', {}, 'walking'), keys([[1.0, '1.0 m/s'], [0.62, '0.62 m/s'], [0.5, '0.5 m/s']], speed, (v) => { speed = v; })),
    h('div', { class: 'gs-row' }, h('span', {}, 'warning'), keys([[15, '15 min'], [30, '30 min'], [45, '45 min']], warning, (v) => { warning = v; })),
    h('div', { class: 'gs-row' }, h('span', {}, 'leaving'), keys([['survey', 'as in 2011'], ['once', 'at once']], departure, (v) => { departure = v; })),
    h('div', { class: 'keys-tight' }, h('button', { onclick: () => compute() }, 'evacuate'), h('button', { onclick: () => result && play() }, 'play again')),
    clockLine, readout, plot, plan, status,
    h('div', { class: 'ld-quiet' }, 'An illustration under the declarations shown, not an official evacuation plan: where a hazard is real, follow the city’s hazard map and its instructions.'),
    h('div', { class: 'ld-quiet' }, `${H.credit} Ways: Overture Maps, OpenStreetMap. People: GHS-POP R2023A (EC JRC, CC BY 4.0), shared among buildings by floor space. The bound by flows over time (morphe), the crowd by kinematic waves (physis), the refuges chosen greedily with a bound (calx): disaster risk's evacuation.`));
  ctx.onDispose(() => { dead = true; stop(); });
  return {
    legend,
    setTime(c) { if (result) paint(frameAt(Math.max(0, (c.hour - hour0) * 3600))); },
    pickAt() { compute(); return { kind: 'place', title: 'Walking out of the hazard\'s reach', lines: [H.zoneName || ''] }; },
    dispose() { dead = true; stop(); earth.setDrape(null); for (const k of [edgesId, sitesId, lateId, towersId]) gis.remove(k); },
  };
}
