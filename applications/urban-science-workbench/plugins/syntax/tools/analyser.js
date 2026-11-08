// The syntax analyser: a grammar learned from a structure, its derivation played step by step, and a moved, turned and scaled copy of
// the structure parsed by it, step by step; and a triangle rewritten by Pachner's, the midpoint's and Sierpinski's moves, whose
// derivation is recovered from its final triangles alone. Every result is saved as JSON.

import { download, palette, patternSvg, stepColour, svg, treeSvg } from '../shared.js';

const EXAMPLES = [
  { key: 'bridged', name: 'bridged triangles', request: { example: 'bridged', depth: 4, maxOrder: 3, tolerance: 0.06 },
    note: 'Three copies of the whole, shrunk to its corners and joined by edges: patterns of three nodes find a rule for each depth, and the probabilistic grammar made from them is recursive.' },
  { key: 'sierpinski', name: 'Sierpinski', request: { example: 'sierpinski', depth: 4, maxOrder: 5, tolerance: 0.08 },
    note: "The gasket's triangles share their corners. Node replacement replaces only disjoint occurrences, so a few nodes remain: the gasket's own grammar replaces hyperedges (planned)." },
  { key: 'grid', name: 'a grid', request: { example: 'grid', rows: 8, columns: 8, maxOrder: 5, tolerance: 0.08 },
    note: 'A square grid: its cells and their joins repeat everywhere.' },
];

export async function render(root, ctx) {
  const { dom: { h } } = ctx;
  const P = palette(ctx.theme);
  let example = EXAMPLES[0], learned = null, parsed = null, playing = null, dead = false;
  const canvas = h('div', { class: 'gs-canvas' });
  const slider = h('input', { type: 'range', min: 0, max: 0, step: 1, value: 0, 'aria-label': 'the step', oninput: () => { stop(); show(current(), +slider.value); } });
  const label = h('span', { class: 'mono' }, '');
  const note = h('p', { class: 'muted' }, example.note);
  const readout = h('div', { class: 'gs-readout' });
  const rules = h('div', { class: 'gs-rules' });
  const treeBox = h('div', { class: 'gs-treebox' });
  const playKey = h('button', { onclick: () => (playing ? stop() : play(current())) }, 'play');
  let view = 'learned';
  const current = () => (view === 'parsed' && parsed ? parsed : learned);

  // The graph and its steps drawn in an SVG: edges swallowed by earlier steps coloured by their step, this step's lit, the rest plain.
  function show(result, s) {
    if (!result) return;
    const g = result.graph, n = result.steps.length;
    slider.max = n;
    slider.value = s;
    label.textContent = s < n ? `step ${s + 1} of ${n}: rule ${result.steps[s].symbol}, ${result.steps[s].occurrences.length} replaced` : `all ${n} steps done`;
    const xs = g.nodes.map((p) => p[0]), ys = g.nodes.map((p) => -p[1]);
    const minX = Math.min(...xs), maxX = Math.max(...xs), minY = Math.min(...ys), maxY = Math.max(...ys);
    const span = Math.max(maxX - minX, maxY - minY) || 1, pad = span * 0.05, sw = span / 260;
    const root = svg('svg', { viewBox: `${minX - pad} ${minY - pad} ${maxX - minX + 2 * pad} ${maxY - minY + 2 * pad}`, width: '100%', class: 'gs-graph' });
    const when = result.when;
    g.edges.forEach(([u, v], e) => {
      const w = when[e];
      const c = w < 0 || w > s ? P.plain : w === s ? P.accent : stepColour(P, w, n);
      root.append(svg('line', { x1: xs[u], y1: ys[u], x2: xs[v], y2: ys[v], stroke: P.css(c), 'stroke-width': w === s ? sw * 2.2 : sw, 'stroke-linecap': 'round' }));
    });
    result.steps.forEach((st, k) => {
      if (k > s) return;
      for (const at of st.made) root.append(svg('circle', { cx: at[0], cy: -at[1], r: sw * (2 + k * 0.8), fill: 'none', stroke: P.css(stepColour(P, k, n)), 'stroke-width': sw * 0.8 }));
    });
    canvas.replaceChildren(root);
  }
  const stop = () => { if (playing) { clearInterval(playing); playing = null; playKey.textContent = 'play'; } };
  function play(result) {
    if (!result) return;
    stop();
    let s = 0;
    playKey.textContent = 'hold';
    show(result, 0);
    playing = setInterval(() => { s++; if (s > result.steps.length || dead) { stop(); return; } show(result, s); }, 800);
  }

  function edgeSteps(result) {
    const g = result.graph, group = new Int32Array(g.nodes.length).fill(-1), when = new Int32Array(g.edges.length).fill(-1);
    result.steps.forEach((s, k) => {
      s.occurrences.forEach((occ, o) => { for (const v of occ) group[v] = o; });
      g.edges.forEach(([u, v], e) => { if (when[e] < 0 && group[u] >= 0 && group[u] === group[v]) when[e] = k; });
      group.fill(-1);
    });
    return when;
  }

  async function learn() {
    stop();
    readout.replaceChildren(h('div', {}, h('span', {}, 'learning'), h('b', {}, '…')));
    let a;
    try { a = await ctx.ask('learn', example.request); } catch (e) { readout.textContent = String(e.message || e); return; }
    if (dead) return;
    a.when = edgeSteps(a);
    learned = a, parsed = null, view = 'learned';
    const G = a.grammar;
    readout.replaceChildren(
      h('div', {}, h('span', {}, 'nodes, edges'), h('b', {}, `${a.graph.nodes.length}, ${a.graph.edges.length}`)),
      h('div', {}, h('span', {}, 'rules'), h('b', {}, `${G.rules.length}, used ${G.rules.map((r) => r.uses).join(', ')} times`)),
      h('div', {}, h('span', {}, 'axiom'), h('b', {}, `${G.axiom.nodes.length} nodes`)),
      h('div', {}, h('span', {}, 'description'), h('b', {}, `${a.description} of ${a.size}`)),
      h('div', {}, h('span', {}, 'replay'), h('b', {}, a.replay.edges ? `exact (positions within ${a.replay.worst.toExponential(1)})` : 'not exact')));
    rules.replaceChildren(...G.rules.map((r) => h('div', { class: 'gs-rule' }, patternSvg(r, ctx.theme), h('span', {}, `${r.symbol}`), h('b', {}, `×${r.uses}`))));
    treeBox.replaceChildren(h('div', { class: 'gs-sub' }, 'The derivation tree'), treeSvg(a.tree, ctx.theme, 560, 190));
    play(a);
  }

  async function parseCopy() {
    if (!learned) return;
    stop();
    // A copy turned by 37°, scaled by 2.3 and moved: parsing is invariant under similarity.
    const t = [2.3 * Math.cos(0.65), 2.3 * Math.sin(0.65), 5, -3];
    let a;
    try { a = await ctx.ask('parse', { ...example.request, transform: t }); } catch (e) { readout.textContent = String(e.message || e); return; }
    if (dead) return;
    a.when = edgeSteps(a);
    parsed = a, view = 'parsed';
    readout.replaceChildren(
      h('div', {}, h('span', {}, 'the copy'), h('b', {}, 'turned 37°, scaled 2.3, moved')),
      h('div', {}, h('span', {}, 'covered'), h('b', {}, `${Math.round(a.covered * 100)}%`)),
      h('div', {}, h('span', {}, 'derived'), h('b', {}, a.derived ? 'yes: what remains is the axiom' : `no: ${a.remaining} remain`)),
      h('div', {}, h('span', {}, 'rules applied'), h('b', {}, a.uses.map((u, k) => `${k + 1}×${u}`).join('  '))));
    treeBox.replaceChildren(h('div', { class: 'gs-sub' }, 'The parse forest'), treeSvg(a.tree, ctx.theme, 560, 190));
    play(a);
  }

  // ---- triangles rewritten ------------------------------------------------------------------------------------------------
  const triCanvas = h('div', { class: 'gs-canvas' });
  const triReadout = h('div', { class: 'gs-readout' });
  const triSlider = h('input', { type: 'range', min: 0, max: 0, step: 1, value: 0, 'aria-label': 'the pass of the parse', oninput: () => drawParse(+triSlider.value) });
  const triLabel = h('span', { class: 'mono' }, '');
  let rewritten = null;
  const MOVE = ['Pachner 1–3', 'midpoint', 'Sierpinski'];
  function drawParse(pass) {
    if (!rewritten) return;
    const R = rewritten, Pz = R.parse;
    triSlider.value = pass;
    triLabel.textContent = pass === 0 ? 'the final triangles, coloured by the move that made them' : `pass ${pass} of ${Pz.steps.length}: the parents found`;
    const root = svg('svg', { viewBox: '-0.04 -0.92 1.08 0.96', width: '100%', class: 'gs-graph' });
    const poly = (t, attrs) => svg('polygon', { points: t.map((p) => `${p[0]},${-p[1]}`).join(' '), ...attrs });
    // Leaves coloured by their parent's move in the generating tree.
    const parentMove = new Map();
    R.tree.forEach((n) => { for (const c of n.children) parentMove.set(c, n.move); });
    R.tree.forEach((n, k) => {
      if (n.move >= 0) return;
      const m = parentMove.get(k);
      root.append(poly(n.corners, { fill: P.css(m === undefined ? P.plain : stepColour(P, m, 3)), 'fill-opacity': 0.55, stroke: P.css(P.faint), 'stroke-width': 0.002 }));
    });
    for (let k = 0; k < pass; k++)
      for (const id of Pz.steps[k]) root.append(poly(Pz.nodes[id].corners, { fill: 'none', stroke: P.css(k === pass - 1 ? P.accent : stepColour(P, Pz.nodes[id].move, 3)), 'stroke-width': k === pass - 1 ? 0.006 : 0.003 }));
    triCanvas.replaceChildren(root);
  }
  async function rewrite() {
    let a;
    try { a = await ctx.ask('replace', { weights: [0.6, 1, 1, 1], depth: 4, seed: 1 + Math.floor(Math.random() * 1e6) }); } catch (e) { triReadout.textContent = String(e.message || e); return; }
    if (dead) return;
    rewritten = a;
    triSlider.max = a.parse.steps.length;
    triReadout.replaceChildren(
      h('div', {}, h('span', {}, 'generated'), h('b', {}, `${a.leaves.length} triangles by ${a.moves.map((m, k) => `${m} ${MOVE[k]}`).join(', ')}`)),
      h('div', {}, h('span', {}, 'recovered'), h('b', {}, `${a.parse.moves.map((m, k) => `${m} ${MOVE[k]}`).join(', ')}`)),
      h('div', {}, h('span', {}, 'derived'), h('b', {}, a.parse.derived ? 'yes: one triangle remains, the start' : 'no')));
    drawParse(0);
    let pass = 0;
    const timer = setInterval(() => { pass++; if (pass > a.parse.steps.length || dead || rewritten !== a) { clearInterval(timer); return; } drawParse(pass); }, 900);
  }

  const exampleKeys = h('div', { class: 'keys-tight' }, EXAMPLES.map((E) => h('button', { class: E === example ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    example = E;
    note.textContent = E.note;
    learn();
  } }, E.name)));
  root.append(
    h('h1', {}, 'The syntax analyser'),
    h('p', { class: 'muted' }, 'A grammar is learned from a structure by compression: what repeats, up to rotation, scale and position, becomes a rule. Played back, each step lights what it replaces. Then a turned, scaled and moved copy is parsed by the grammar: the same rules find the same structure, wherever and however large it is.'),
    exampleKeys, note,
    canvas, h('label', { class: 'view-slider gs-slider' }, slider, label),
    h('div', { class: 'keys-tight gs-keys' }, playKey, h('button', { onclick: learn }, 'learn again'), h('button', { onclick: parseCopy }, 'parse a turned copy')),
    readout, rules, treeBox,
    h('div', { class: 'keys-tight gs-keys' },
      h('button', { onclick: () => learned && download(`grammar-${example.key}.json`, learned.grammar) }, 'save the grammar'),
      h('button', { onclick: () => learned && download(`derivation-${example.key}.json`, { graph: learned.graph, steps: learned.steps, tree: learned.tree }) }, 'save the derivation'),
      h('button', { onclick: () => parsed && download(`parse-${example.key}.json`, { graph: parsed.graph, steps: parsed.steps, tree: parsed.tree, covered: parsed.covered, derived: parsed.derived }) }, 'save the parse')),
    h('h2', {}, 'Triangles rewritten'),
    h('p', { class: 'muted' }, 'A triangle rewrites into three by Pachner’s 1–3 move, four by the midpoint, or three corners by Sierpinski’s, each with a probability, to a depth: a probabilistic context-free grammar whose words are 2-simplices. Its derivation is then recovered from the final triangles alone, bottom up.'),
    triCanvas, h('label', { class: 'view-slider gs-slider' }, triSlider, triLabel),
    h('div', { class: 'keys-tight gs-keys' }, h('button', { onclick: rewrite }, 'rewrite a triangle'),
      h('button', { onclick: () => rewritten && download('triangles.json', rewritten) }, 'save the derivation and its parse')),
    triReadout);
  ctx.onDispose(() => { dead = true; stop(); });
  learn();
  rewrite();
}
