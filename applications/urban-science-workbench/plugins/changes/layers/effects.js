// What a change did, and around it (ADR 0087): any case of changes and counts the data library holds (data/products/street_changes.py),
// through the engine on urban science's interventions and peira's interference. The changes come with the clock, those standing
// strong and those to come faint; each sensor is coloured by its own change across its entry against the sensors not yet reached; the
// card gives the effects by ring of distance along the ways, in the two-way regression and against the sensors not yet reached, the
// event study of each ring, and what the record can tell: how many sensors saw their change, and how many there were to compare with.

import { addEstimates, addFootprints, percent, effectColour } from '/visualization/js/gis/effects.js';

const SCALE = 0.5;  // the colour's end, in logarithms: −39% to +65%
const css = (rgb) => `rgb(${rgb.map((v) => Math.round(255 * v)).join(',')})`;

export async function layer(ctx) {
  const { gis, frame, earth, dom: { h } } = ctx;
  const sitesId = ctx.layerId(':sensors'), marksId = ctx.layerId(':changes');
  let cases = {}, chosen = null, R = null, bounds = null, now = null, dead = false, token = 0, clock = ctx.clock;
  const status = h('div', { class: 'ld-quiet' }, 'Finding the records of changes…');
  const head = h('div', { class: 'ld-quiet' });
  const table = h('div', { class: 'gs-readout' });
  const plot = h('div', {});
  const tells = h('div', { class: 'ld-quiet' });
  const choices = h('div', { class: 'keys-tight' });
  const ringKeys = h('div', { class: 'keys-tight' });

  const periodOf = (c) => {
    const y = +c.date.slice(0, 4), m = +c.date.slice(5, 7);
    return R && R.period === 'year' ? y : y * 12 + (m - 1);
  };
  const label = (t) => (R && R.period === 'year' ? String(t) : `${Math.floor(t / 12)}-${String((t % 12) + 1).padStart(2, '0')}`);
  const unit = () => (R && R.period === 'year' ? 'years' : 'months');

  function draw() {
    if (!R) return;
    now = periodOf(clock);
    const sensors = R.sensors.map((s) => ({ s, p: frame.toLocal(s.lat, s.lon) }));
    addEstimates(gis, sitesId, sensors, { position: (f) => [f.p[0], f.p[1]], value: (f) => (f.s.own == null ? NaN : f.s.own), scale: SCALE, theme: ctx.theme,
      radius: Math.max(6, earth.view.range * 0.004), height: Math.max(18, earth.view.range * 0.012) });
    const marks = R.footprints.filter((f) => f.reaches);
    const lines = marks.length && marks[0].ll.length > 2;
    addFootprints(gis, marksId, marks, lines
      ? { line: (f) => { const out = []; for (let i = 0; i < f.ll.length; i += 2) { const q = frame.toLocal(f.ll[i + 1], f.ll[i]); out.push([q[0], q[1]]); } return out; },
          begin: (f) => f.begin, now, fresh: R.period === 'year' ? 1 : 12, width: Math.max(3, earth.view.range * 0.0016), theme: ctx.theme }
      : { point: (f) => { const q = frame.toLocal(f.ll[1], f.ll[0]); return [q[0], q[1]]; }, begin: (f) => f.begin, now, fresh: 1, theme: ctx.theme });
    const standing = marks.filter((f) => f.begin <= now).length;
    status.textContent = `${label(now)}: ${standing} of the ${marks.length} changes near a sensor stand; the sensors coloured by their own change across their entry.`;
  }

  function card() {
    const n = (v) => Math.round(v).toLocaleString('en');
    const fmt = (e, lo, hi) => (Number.isFinite(e) ? `${percent(e)} (${percent(lo)} to ${percent(hi)})` : '—');
    const rows = [h('div', {}, h('span', {}, 'ring, along the ways'), h('b', {}, 'two-way regression · against those not yet reached'))];
    let inner = 0;
    R.rings.forEach((g) => {
      const tw = g.two_way, st = g.staggered;
      const stText = Number.isFinite(st.simple) ? `${percent(st.simple)} ± ${percent(st.se ?? NaN).replace(/^[+−]/, '')} (${st.units} sensors)` : 'not estimable';
      rows.push(h('div', {}, h('span', {}, `${n(inner)}–${n(g.bound)} m: ${g.entering} entering`), h('b', {}, `${fmt(tw.effect, tw.lower, tw.upper)} · ${stText}`)));
      inner = g.bound;
    });
    rows.push(h('div', {}, h('span', {}, `beyond ${n(inner)} m`), h('b', {}, `${R.never} never reached`)));
    table.replaceChildren(...rows);
    // The event study of each ring, against those not yet reached.
    const tones = ctx.theme === 'night' ? [[0.98, 0.72, 0.62], [0.72, 0.84, 0.86], [0.85, 0.78, 0.95]] : [[0.66, 0.28, 0.22], [0.18, 0.42, 0.45], [0.42, 0.3, 0.58]];
    const lim = R.period === 'year' ? 6 : 36;
    const series = [];
    R.rings.forEach((g, k) => {
      const pts = g.staggered.by_exposure.filter(([e, v]) => Math.abs(e) <= lim && v != null);
      if (pts.length < 2) return;
      const x = pts.map((p) => p[0]), y = pts.map((p) => p[1]), lo = pts.map((p) => p[1] - 2 * (p[2] ?? 0)), hi = pts.map((p) => p[1] + 2 * (p[2] ?? 0));
      series.push({ name: `band ${k}`, x, y: lo, fill: hi, color: css(tones[k % 3]), opacity: 0.16 });
      series.push({ name: `within ${Math.round(g.bound)} m`, x, y, color: css(tones[k % 3]), width: 1.8 });
    });
    plot.replaceChildren(series.length ? ctx.chart.lines(series, { height: 150, width: 340, xlabel: `${unit()} from entering the ring`, ylabel: 'change (log)', marks: [{ x: 0, label: 'entry' }] }) : h('div', { class: 'ld-quiet' }, 'No ring has an event study: no sensors were left to compare with.'));
    const saw = R.sensors.filter((s) => s.own != null).length, entered = R.sensors.filter((s) => s.onset != null).length;
    tells.textContent = `What the record can tell: ${saw} of the ${entered} sensors reached have a change of their own (counting before their change came, with sensors not yet reached to compare with), and ${R.never} were never reached. ` +
      'A sensor that began counting after its change says nothing of what the change did; the effects rest on the others and on those not yet reached.';
    head.textContent = `${R.city}: ${R.question}, ${label(R.periods[0])} to ${label(R.periods[R.periods.length - 1])}. ${R.outcome}; ${R.contacts.toLocaleString('en')} contacts within ${Math.round(R.bounds[R.bounds.length - 1])} m along the ways.`;
  }

  async function evaluate() {
    if (!chosen) return;
    const me = ++token;
    status.textContent = `Reading ${cases[chosen].city}'s record along its ways… (the first time takes a while)`;
    try {
      const r = await ctx.ask('evaluate', { case: chosen, bounds });
      if (me !== token || dead) return;
      R = r;
    } catch (e) { status.textContent = String(e.message || e); return; }
    card();
    draw();
  }

  function choose(name) {
    chosen = name;
    const c = cases[name];
    bounds = c.bounds.slice();
    const sets = [['as declared', c.bounds], ['narrower', c.bounds.map((b) => Math.round(b / 2))], ['wider', c.bounds.map((b) => Math.round(b * 1.5))]];
    ringKeys.replaceChildren(...sets.map(([name2, b], i) => h('button', { class: i === 0 ? 'on' : '', title: b.map((v) => `${v} m`).join(', '), onclick: (e) => {
      for (const x of e.currentTarget.parentElement.children) x.classList.toggle('on', x === e.currentTarget);
      bounds = b.slice();
      evaluate();
    } }, `${name2}: ${b.join(', ')} m`)));
    for (const x of choices.children) x.classList.toggle('on', x.dataset.case === name);
    evaluate();
  }

  try {
    cases = await ctx.ask('cases', {});
  } catch (e) { status.textContent = `No record of changes: ${e.message || e}`; }
  const near = () => {
    const v = earth.view, lat = (v.lat * 180) / Math.PI, lon = (v.lon * 180) / Math.PI;
    let best = null, bd = Infinity;
    for (const [k, c] of Object.entries(cases)) {
      const d = Math.hypot(c.origin.lat - lat, (c.origin.lon - lon) * Math.cos((lat * Math.PI) / 180)) * 111;
      if (d < bd) bd = d, best = k;
    }
    return bd < 60 ? best : null;
  };
  choices.replaceChildren(...Object.entries(cases).map(([k, c]) => h('button', { 'data-case': k, onclick: () => {
    earth.setView({ lon: (c.origin.lon * Math.PI) / 180, lat: (c.origin.lat * Math.PI) / 180, height: 0, range: k.startsWith('newyork') ? 26000 : 7000, tilt: 0.35, heading: 0 });
    choose(k);
  } }, c.city)));
  const here = near();
  if (here) choose(here);
  else status.textContent = 'No record of changes here: choose a city.';

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'What a change did, and around it'),
    h('div', { class: 'gs-row' }, h('span', {}, 'record'), choices),
    h('div', { class: 'gs-row' }, h('span', {}, 'rings'), ringKeys),
    head, table, plot,
    h('span', { class: 'ramp', style: `background:linear-gradient(90deg, ${Array.from({ length: 11 }, (_, i) => css(effectColour((i / 5 - 1) * SCALE, SCALE))).join(',')})` }),
    h('div', { class: 'ramp-ends' }, h('span', {}, percent(-SCALE)), h('span', {}, 'a sensor\'s own change'), h('span', {}, percent(SCALE))),
    tells, status,
    h('div', { class: 'ld-quiet' }, 'Estimators: peira\'s interference (rings after Butts 2021; Callaway and Sant\'Anna 2021 against those not yet reached). Records: City of Melbourne (CC BY 4.0); NYC DOT on NYC Open Data; ways, Overture Maps (ODbL).'));
  ctx.onDispose(() => { dead = true; });
  return {
    legend,
    setTime(c) { clock = c; if (R) draw(); },
    describe(hit) {
      if (!R || hit.layer !== sitesId) return null;
      const s = hit.feature.s;
      const year = (t) => (R.period === 'year' ? t : t / 12);  // a month's index as a decimal year
      const xs = s.series.map((p) => year(p[0])), ys = s.series.map((p) => Math.exp(p[1]));
      const marks = s.onset != null ? [{ x: year(s.onset), label: 'reached' }] : [];
      const chart = xs.length > 1 ? ctx.chart.lines([{ name: 'counted', x: xs, y: ys, width: 1.6 }], { height: 140, width: 340, xlabel: 'year', ylabel: R.outcome.replace(' (logarithm)', ''), marks }) : null;
      const ring = s.entry < R.bounds.length ? `entered the ring within ${Math.round(R.bounds[s.entry])} m in ${label(s.onset)}` : 'never reached';
      return { kind: 'sensor', title: s.name, lines: [ring, s.own != null ? `its own change across its entry, against those not yet reached: ${percent(s.own)}` : 'no change of its own to tell: it was not counting before, or none were left to compare with'],
        extra: chart ? h('div', {}, chart) : undefined };
    },
    dispose() { dead = true; gis.remove(sitesId); gis.remove(marksId); },
  };
}
