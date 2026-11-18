// Space and time (ADR 0127): the matter's evidence drawn where and when it speaks, as a cube whose floor is the place and whose
// height is time (the visualization layer's evidence/spacetime.js). Each person's possible whereabouts between the items that place
// them rise as prisms; a theory chosen draws its paths and events; a plane at an instant shows where each person could then have
// been. The cube turns under the hand; seen from above it is the plan at the instant. An item opens where it stands. Beside it, the
// question every alibi asks, read under the readings of its open terms: could this person have been at the scene at the time?

import { cubeOf, svgCube, svgPlan, clockOf, inkOf } from '/visualization/js/evidence/index.js';
import { S, T, ensure, loadTheories, runTheories, chosenTheory, nameOf, classOf } from './common.js';

export function spacetimeView() {
  return { id: 'spacetime', label: 'Space and time', draw };
}

const MARK = { clear: '✓', arguable: '◐', untenable: '✕' };

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('stv-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('space and time');
  if (!S.matter?.located?.length)
    return fill(pane, el('div', { class: 'stv-empty' }, el('h2', {}, 'Where and when'), el('p', {}, 'This matter places no evidence in space and time. A matter that does (its people, the kinds of event, and each item where and when it speaks) is drawn here as a cube, with the theories of the case the search finds. The example ', el('b', {}, 'The fire at the boathouse'), ' shows one.')));
  try { await loadTheories(ctx); } catch (error) { return fill(pane, el('p', { class: 'note wrong' }, error.message)); }
  const R = T.report;
  if (!R) return fill(pane, el('p', { class: 'note' }, 'The evidence could not be laid out.'));
  const theory = chosenTheory();
  let model = cubeOf(S.matter, R, { theory, zoom: T.zoom });
  if (T.at === null || T.at < model.t0 || T.at > model.t1) T.at = (R.evidence.rooms?.[0] ? (R.evidence.rooms[0].from + R.evidence.rooms[0].until) / 2 : (model.t0 + model.t1) / 2);
  const epoch = R.evidence.epoch;
  ctx.note(`${(R.evidence.located || []).length} items, ${(R.evidence.agents || []).length} people; ${(R.kept || []).length} theories kept${theory ? `; drawn: #${theory.serial}, ${theory.central}` : ''}`);

  const stage = el('div', { class: 'stv-stage' });
  const card = el('div', { class: 'stv-card', hidden: true });
  const render = () => {
    model = cubeOf(S.matter, R, { theory, zoom: T.zoom });
    stage.innerHTML = T.above ? svgPlan(model, { at: T.at, window: 240 }) : svgCube(model, { azimuth: T.azimuth, elevation: T.elevation, at: T.at, selected: T.item });
    clock.textContent = clockOf(epoch, T.at);
  };
  // Turning the cube by hand.
  let drag = null;
  stage.addEventListener('pointerdown', (e) => {
    if (e.target.closest('.st-item')) return;
    drag = { x: e.clientX, y: e.clientY, az: T.azimuth, el: T.elevation };
    stage.setPointerCapture(e.pointerId);
    stage.classList.add('turning');
  });
  stage.addEventListener('pointermove', (e) => {
    if (!drag || T.above) return;
    T.azimuth = drag.az + (e.clientX - drag.x) / 220;
    T.elevation = Math.max(0.05, Math.min(1.5, drag.el + (e.clientY - drag.y) / 260));
    render();
  });
  const stop = () => { drag = null; stage.classList.remove('turning'); };
  stage.addEventListener('pointerup', stop);
  // Closer and farther under the wheel, about the scene.
  stage.addEventListener('wheel', (e) => {
    e.preventDefault();
    T.zoom = Math.min(12, Math.max(1, (T.zoom || 1) * (e.deltaY < 0 ? 1.18 : 1 / 1.18)));
    render();
  }, { passive: false });
  stage.addEventListener('pointercancel', stop);
  // An item opens where it stands.
  stage.addEventListener('click', (e) => {
    const g = e.target.closest('.st-item');
    if (!g) { card.hidden = true; T.item = null; return; }
    T.item = g.dataset.item;
    openItem(T.item, e);
    render();
  });
  const openItem = (id, e) => {
    const i = (R.evidence.located || []).find((x) => x.id === id), def = (S.matter.located || []).find((x) => x.id === id);
    if (!i) return;
    const reading = theory?.readings?.find((r) => r.item === id);
    const rect = pane.getBoundingClientRect();
    card.replaceChildren(
      el('button', { type: 'button', class: 'close', 'aria-label': 'Close', onclick: () => { card.hidden = true; T.item = null; render(); } }, '×'),
      el('p', { class: 'stv-kind' }, `${{ sighting: 'places', event: 'reports', absence: 'reports no', interval: 'times' }[i.says] || i.says} · ${i.modality || 'an item'}`),
      el('h4', {}, i.text || i.id),
      el('p', { class: 'small' }, `${i.source || 'a source'} · ${i.from_clock}${i.until > i.from ? ` to ${i.until_clock}` : ''} · within ${i.radius} m`),
      el('p', {}, el('span', { class: 'stv-cred', style: `--c:${Math.round(100 * i.credibility)}%` }), ` credibility ${i.credibility}: the chance that what it says is so, its source considered`),
      i.held_back ? el('p', { class: 'stv-held' }, 'Held back from the search, to test what it found: the search never saw it, and each theory was tested on it once, the test sealed before it was read.') : null,
      reading ? el('p', { class: `stv-stance ${reading.believed ? 'believed' : 'doubted'}` }, `In theory #${theory.serial}: ${reading.believed ? `believed (${reading.status}${reading.clock ? `, at ${reading.clock}` : ''})` : 'doubted'}${reading.note ? ` — ${reading.note}` : ''}`) : null,
      def?.document ? el('button', { type: 'button', class: 'key edged', onclick: () => { S.doc = def.document; S.fact = null; ctx.show('evidence'); } }, `Read it: ${def.document}`) : null);
    card.hidden = false;
    const x = Math.min(rect.width - 330, Math.max(12, e.clientX - rect.left + 14)), y = Math.min(rect.height - 220, Math.max(56, e.clientY - rect.top - 20));
    card.style.left = `${x}px`, card.style.top = `${y}px`;
  };

  // The theories as chips: the evidence alone, then each kept, most plausible first.
  const chips = el('nav', { class: 'stv-theories', 'aria-label': 'The theory drawn' },
    el('button', { type: 'button', class: `stv-chip${theory ? '' : ' on'}`, onclick: () => { T.chosen = null; ctx.show('spacetime'); } }, 'the evidence alone'),
    (R.kept || []).map((t) => el('button', { type: 'button', class: `stv-chip${theory?.serial === t.serial ? ' on' : ''}`, title: t.reason,
      onclick: () => { T.chosen = t.serial; ctx.show('spacetime'); } },
      el('span', { class: 'stv-serial' }, `#${t.serial}`), ` ${t.central}`, el('span', { class: 'stv-weight' }, ` ${(100 * t.plausibility).toFixed(0)}%`),
      t.test ? el('span', { class: `stv-test ${t.test.outcome.replace(/\s+/g, '-')}`, title: `tested on what was held back: ${t.test.reason}` }, t.test.outcome === 'admitted' ? ' ✓' : ' ✕') : null)),
    el('button', { type: 'button', class: 'key edged stv-go', onclick: () => ctx.show('theories') }, 'The search →'));

  // The ribbon of time.
  const clock = el('span', { class: 'stv-clock' });
  const slider = el('input', { type: 'range', class: 'stv-range', min: String(model.t0), max: String(model.t1), step: '5', value: String(T.at), 'aria-label': 'The instant of the plane',
    oninput: (e) => { T.at = Number(e.target.value); render(); } });
  let timer = null;
  const play = el('button', { type: 'button', class: 'key edged', onclick: () => {
    if (timer) { clearInterval(timer); timer = null; play.textContent = 'Play'; return; }
    play.textContent = 'Pause';
    timer = setInterval(() => {
      if (!pane.isConnected) { clearInterval(timer); return; }
      T.at += (model.t1 - model.t0) / 240;
      if (T.at > model.t1) T.at = model.t0;
      slider.value = String(T.at);
      render();
    }, 60);
  } }, 'Play');
  const ribbon = el('div', { class: 'stv-ribbon' }, play, el('span', { class: 'small' }, clockOf(epoch, model.t0, false)), slider, el('span', { class: 'small' }, clockOf(epoch, model.t1, false)), clock,
    el('button', { type: 'button', class: 'key edged', title: 'Closer about the scene (or turn the wheel over the drawing)', onclick: () => { T.zoom = (T.zoom || 1) > 1.5 ? 1 : 3; render(); } }, 'Closer / the whole place'),
    el('button', { type: 'button', class: `key edged${T.above ? ' on' : ''}`, onclick: () => { T.above = !T.above; ctx.show('spacetime'); } }, T.above ? 'In the round' : 'From above'));

  // Who is drawn, in their inks; and the shapes.
  const legend = el('div', { class: 'stv-legend' },
    (R.evidence.agents || []).map((a, k) => {
      const def = (S.matter.agents || []).find((x) => x.id === a.id);
      return el('p', { class: 'stv-person', title: def?.basis || '' }, el('span', { class: 'stv-ink', style: `background:${inkOf(k)}` }), el('b', {}, a.name), el('span', { class: 'small' }, ` at most ${a.speed} m/s`));
    }),
    el('p', { class: 'small stv-shapes' }, '● places someone · ◆ reports an event · ⊘ reports an absence · a bar is the span of time, a dashed ring the error of place · rings rising are where a person could have been'),
    model.outside.length ? el('p', { class: 'small' }, `Outside the window drawn: ${model.outside.join(', ')}.`) : null);

  // Could they have been there? Under every reading of "at the scene" and "at the time".
  const presence = el('aside', { class: 'stv-presence' }, el('h3', {}, 'Could they have been there?'),
    (R.presence || []).map((p) => el('div', { class: `stv-who ${p.class}` },
      el('p', {}, el('span', { class: `cls ${p.class}` }, `${MARK[p.class] || '·'} ${classOf(p.class).word}`), ' ', el('b', {}, p.name)),
      el('p', { class: 'small' }, sentence(p)),
      el('table', { class: 'stv-readings' }, el('tbody', {}, p.each.map((x) => el('tr', { class: x.holds ? 'yes' : 'no', title: x.reason },
        el('td', {}, x.holds ? '✓' : '✕'), el('td', {}, x.place), el('td', {}, x.time),
        el('td', { class: 'small' }, x.windows.length ? x.windows.map((w) => `${w.from_clock.slice(0, 8)}–${w.until_clock.slice(0, 8)}`).join(', ') : `short by ${Number(x.least).toFixed(0)} m`))))))),
    el('p', { class: 'small' }, 'Read off each person\'s sightings under a bound on their speed: an alibi so found is certain; a possibility is under the widening of sightings known only within a span.'));

  fill(pane, el('div', { class: 'stv-frame' }, chips, stage, card, legend, presence, ribbon,
    T.busy ? el('p', { class: 'stv-busy' }, 'The engine is working…') : null));
  render();
}

function sentence(p) {
  if (p.class === 'clear') return `under every reading of the scene and the time, a path at ${p.name}'s speed reaches it.`;
  if (p.class === 'untenable') return `under no reading: no path at their speed reaches the scene in time. An alibi, certified.`;
  const turn = (p.pivots || []).filter((v) => v.flips.length).map((v) => `${v.point} (read as ${v.flips.join(' or ')})`).join('; ');
  return `under ${Math.round(100 * p.share)}% of the readings${turn ? `; it turns on ${turn}` : ''}.`;
}
