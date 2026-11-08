// The grammar of the streets: pick a place, and the street network about it is read as a geometric graph (urban-science's syntax), a
// grammar is learned from it by compression (syntaxis), and its derivation is played back on the map step by step — each step's
// occurrences lit as they are replaced, what earlier steps swallowed coloured by when. The grammar can then parse another place (the
// syntax analysis), or be made probabilistic or context-sensitive and sampled into streets that were never drawn. Grammars, derivation
// trees and parses are saved as JSON.

import { download, linear, palette, patternSvg, stepColour, treeSvg, waysNear, whenNear } from '../shared.js';

const RADII = [[250, '250 m'], [500, '500 m'], [1000, '1 km']];
const TOLERANCES = [[0.06, 'fine'], [0.1, 'middling'], [0.16, 'loose']];

export async function layer(ctx) {
  const { gis, frame, earth, world, dom: { h } } = ctx;
  const id = ctx.layerId(':streets'), made = ctx.layerId(':made'), parseId = ctx.layerId(':parse'), sampleId = ctx.layerId(':sample');
  const P = palette(ctx.theme);
  let radius = 500, tolerance = 0.1, centre = null, learned = null, parsed = null, step = 0, playing = null, dead = false, armed = false, token = 0;

  // Short at first, so that the shell keeps it in the card (it folds long prose away); it then says what is happening.
  const status = h('div', { class: 'ld-quiet' }, 'Pick a place: a grammar is learned from its streets.');
  const readout = h('div', { class: 'gs-readout' });
  const rules = h('div', { class: 'gs-rules' });
  const tree = h('div', { class: 'gs-treebox' });
  const parseBox = h('div', { class: 'gs-readout' });
  const sampleBox = h('div', { class: 'gs-readout' });
  const slider = h('input', { type: 'range', min: 0, max: 0, step: 1, value: 0, 'aria-label': 'the step of the derivation', oninput: () => { stop(); show(+slider.value); } });
  const stepLabel = h('span', { class: 'mono' }, '');
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));
  const playKey = h('button', { onclick: () => (playing ? stop() : play()) }, 'play');
  const parseKey = h('button', { title: 'the next place picked is parsed by this grammar', onclick: () => { armed = !armed; parseKey.classList.toggle('on', armed); status.textContent = armed ? 'Pick another place: it is parsed by the grammar learned here.' : status.textContent; } }, 'parse another place');

  const width = () => Math.max(2, earth.view.range * 0.003);
  const z = () => 0.4;  // just above the ground and the ways

  // When each edge was swallowed, and each node's step: from the steps' occurrences (sets of input nodes).
  function edgeSteps(result) {
    const g = result.graph, n = g.nodes.length;
    const group = new Int32Array(n).fill(-1);
    const when = new Int32Array(g.edges.length).fill(-1);
    result.steps.forEach((s, k) => {
      s.occurrences.forEach((occ, o) => { for (const v of occ) group[v] = o; });
      g.edges.forEach(([u, v], e) => { if (when[e] < 0 && group[u] >= 0 && group[u] === group[v]) when[e] = k; });
      group.fill(-1);
    });
    return when;
  }

  function drawGraph(drawId, result, colourOf, opacity = 1) {
    const g = result.graph;
    const features = g.edges.map(([u, v], e) => ({ e, line: [[g.nodes[u][0], g.nodes[u][1]], [g.nodes[v][0], g.nodes[v][1]]] }));
    gis.addRibbons(drawId, features, { line: (f) => f.line, width: () => width(), color: (f) => linear(colourOf(f.e)), z: z(), opacity, order: 3 });
  }

  function show(s) {
    if (!learned) return;
    step = s;
    const n = learned.steps.length;
    slider.value = s;
    stepLabel.textContent = s < n ? `step ${s + 1} of ${n}: rule ${learned.steps[s].symbol}, ${learned.steps[s].occurrences.length} replaced` : `all ${n} steps: what remains is the axiom`;
    const when = learned.when;
    // Steps done in their colours, the step playing lit, the rest dim: never reduced (left in the axiom), or not yet.
    const dim = P.plain.map((x, i) => x * 0.5 + P.faint[i] * 0.5);
    drawGraph(id, learned, (e) => {
      const w = when[e];
      if (w < 0 || w > s) return dim;
      if (w === s) return P.accent;
      return stepColour(P, w, n);
    });
    const points = [];
    learned.steps.forEach((st, k) => { if (k <= s) st.made.forEach((at) => points.push({ at, k })); });
    gis.addPoints(made, points, { position: (f) => f.at, radius: () => width() * 1.8, height: (f) => width() * (3 + 2 * f.k), color: (f) => stepColour(P, f.k, n), pick: false });
  }
  const stop = () => { if (playing) { clearInterval(playing); playing = null; playKey.textContent = 'play'; } };
  function play() {
    if (!learned) return;
    stop();
    let s = 0;
    playKey.textContent = 'hold';
    show(0);
    playing = setInterval(() => {
      s++;
      if (s > learned.steps.length || dead) { stop(); return; }
      show(s);
    }, 900);
  }

  function describe(result) {
    const G = result.grammar;
    readout.replaceChildren(
      h('div', {}, h('span', {}, 'junctions, segments'), h('b', {}, `${result.graph.nodes.length.toLocaleString('en')}, ${result.graph.edges.length.toLocaleString('en')}`)),
      h('div', {}, h('span', {}, 'rules'), h('b', {}, String(G.rules.length))),
      h('div', {}, h('span', {}, 'description'), h('b', {}, `${result.description.toLocaleString('en')} of ${result.size.toLocaleString('en')} (${Math.round((100 * result.description) / result.size)}%)`)),
      h('div', {}, h('span', {}, 'axiom'), h('b', {}, `${G.axiom.nodes.length} nodes`)),
      h('div', {}, h('span', {}, 'replay'), h('b', {}, result.replay.edges ? 'every segment, exactly' : 'not exact')));
    // The rules small, the first 21 drawn (all are in the grammar saved).
    rules.replaceChildren(...G.rules.slice(0, 21).map((r) => h('div', { class: 'gs-rule', title: `rule ${r.symbol}: ${r.nodes.length} nodes, ${r.edges.length} edges, used ${r.uses} times` },
      patternSvg(r, ctx.theme, 34), h('span', {}, `${r.symbol} ×${r.uses}`))),
      ...(G.rules.length > 21 ? [h('div', { class: 'gs-rule more' }, h('span', {}, `+${G.rules.length - 21}`))] : []));
    tree.replaceChildren(treeSvg(result.tree, ctx.theme, 520, 170));
  }

  async function learn() {
    if (!centre) return;
    const me = ++token;
    status.textContent = 'gathering the streets here…';
    const ways = await whenNear(() => waysNear(world, centre, radius), () => me === token && !dead);
    if (me !== token || dead) return;
    if (!ways) { status.textContent = 'No ways here: pick another place.'; return; }
    status.textContent = `reading ${ways.length.toLocaleString('en')} ways and learning their grammar…`;
    let a;
    try { a = await ctx.ask('learn', { ways, tolerance, maxOrder: 5 }); } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    a.when = edgeSteps(a);
    learned = a;
    parsed = null;
    gis.remove(parseId), gis.remove(sampleId);
    parseBox.replaceChildren(), sampleBox.replaceChildren();
    slider.max = a.steps.length;
    describe(a);
    status.textContent = `learned in ${a.seconds.toFixed(1)} s; each step lights what it replaces, and leaves a column where it stood.`;
    play();
  }

  async function parseAt(at) {
    status.textContent = 'gathering the streets there…';
    const ways = await whenNear(() => waysNear(world, at, radius), () => !dead);
    if (dead) return;
    if (!ways) { status.textContent = 'No ways there: pick another place to parse.'; return; }
    status.textContent = 'parsing this place by the grammar learned there…';
    let a;
    try { a = await ctx.ask('parse', { ways }); } catch (e) { status.textContent = String(e.message || e); return; }
    if (dead) return;
    a.when = edgeSteps(a);
    parsed = a;
    const n = Math.max(1, a.steps.length);
    drawGraph(parseId, a, (e) => (a.when[e] < 0 ? P.plain : stepColour(P, a.when[e], n)));
    parseBox.replaceChildren(
      h('div', {}, h('span', {}, 'covered'), h('b', {}, `${Math.round(a.covered * 100)}% of its junctions`)),
      h('div', {}, h('span', {}, 'derived'), h('b', {}, a.derived ? 'yes: what remains is the axiom' : `no: ${a.remaining} nodes remain`)),
      h('div', {}, h('span', {}, 'rules applied'), h('b', {}, a.uses.map((u, k) => `${k + 1}×${u}`).join('  '))),
      h('div', { class: 'keys-tight' }, h('button', { onclick: () => download('parse.json', { grammar: learned.grammar, graph: a.graph, steps: a.steps, tree: a.tree, covered: a.covered, derived: a.derived }) }, 'save the parse')));
    status.textContent = 'The other place, parsed: its segments coloured by the step that absorbed them, uncovered ones plain.';
    armed = false;
    parseKey.classList.remove('on');
  }

  async function sample(kind) {
    if (!learned) return;
    let a;
    try { a = await ctx.ask('sample', { kind, seed: 1 + Math.floor(Math.random() * 1e6) }); } catch (e) { sampleBox.textContent = String(e.message || e); return; }
    if (dead) return;
    gis.addRibbons(sampleId, a.graph.edges.map(([u, v]) => ({ line: [[a.graph.nodes[u][0], a.graph.nodes[u][1]], [a.graph.nodes[v][0], a.graph.nodes[v][1]]] })),
      { line: (f) => f.line, width: () => width() * 1.15, color: () => linear(P.sample), z: z() + 0.3, order: 4 });
    sampleBox.replaceChildren(
      h('div', {}, h('span', {}, kind === 'pcsg' ? 'context-sensitive' : 'probabilistic'), h('b', {}, `${a.sample.nodes} junctions, ${a.sample.edges} segments`)),
      h('div', {}, h('span', {}, 'categories'), h('b', {}, `${a.categories.length} from ${learned.grammar.rules.length} rules`)),
      h('div', {}, h('span', {}, 'degrees apart'), h('b', {}, `${(a.degreeDistance * 100).toFixed(0)}% from the streets'`)),
      h('div', {}, h('span', {}, 'crossings'), h('b', {}, `${a.sample.crossings}${kind === 'pcsg' ? `; ${a.refused} productions refused for crossing` : ''}`)));
  }

  const legend = h('div', { class: 'card gs-card' },
    h('b', {}, 'The grammar of the streets'),
    h('div', { class: 'gs-row' }, h('span', {}, 'within'), keys(RADII, radius, (v) => { radius = v; learn(); })),
    h('div', { class: 'gs-row' }, h('span', {}, 'tolerance'), keys(TOLERANCES, tolerance, (v) => { tolerance = v; learn(); })),
    h('label', { class: 'view-slider gs-slider' }, slider, stepLabel), h('div', { class: 'keys-tight' }, playKey),
    readout, rules, tree,
    h('div', { class: 'keys-tight gs-keys' },
      h('button', { onclick: () => learned && download('grammar.json', learned.grammar) }, 'save the grammar'),
      h('button', { onclick: () => learned && download('derivation.json', { graph: learned.graph, steps: learned.steps, tree: learned.tree }) }, 'save the derivation')),
    h('div', { class: 'keys-tight gs-keys' }, parseKey,
      h('button', { title: 'a network drawn from the grammar made probabilistic', onclick: () => sample('pcfg') }, 'sample (PCFG)'),
      h('button', { title: 'drawn with its context: the parent, and no crossing what is drawn', onclick: () => sample('pcsg') }, 'sample (PCSG)')),
    parseBox, sampleBox, status,
    h('div', { class: 'ld-quiet' }, 'Ways: Overture Maps, OpenStreetMap. Grammar induction after Fiser et al. (2016); syntaxis.'));
  ctx.onDispose(() => { dead = true; stop(); });
  return {
    legend,
    pickAt(lat, lon) {
      const p = frame.toLocal(lat, lon);
      const at = [Math.round(p[0] * 10) / 10, Math.round(p[1] * 10) / 10];
      if (armed && learned) { parseAt(at); return { kind: 'place', title: 'Parsed here', lines: ['by the grammar learned before'], at: { lat, lon } }; }
      centre = at;
      learn();
      return { kind: 'place', title: 'Learned here', lines: [`the streets within ${radius} m`], at: { lat, lon } };
    },
    dispose() { dead = true; stop(); for (const k of [id, made, parseId, sampleId]) gis.remove(k); },
  };
}
