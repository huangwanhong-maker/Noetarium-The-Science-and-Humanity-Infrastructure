// The computational graph of an architecture laid out (kybernesis, ADR 0130): pure arithmetic, no page.
//
// The stages of one turn are columns from left to right, each tinted by its register. Within a stage the blocks stand in
// sub-columns by their depth along the wires that are not carried to the next turn, so that every such wire runs from left to
// right; a block that nothing feeds stands just before what it feeds. Across all the columns this is one layered drawing
// (Sugiyama's): a wire that passes over a sub-column is given a place in it, shared by the wires of one port, so that it runs
// around what stands there and not behind it. The order within a sub-column is found by barycentres and adjacent exchanges that
// lessen the crossings, under three rules: what enters from outside (a boundary) and a wire that passes over the whole of a
// stage stand at the top; a block whose wires return beneath stands below the other blocks. The heights are found by isotonic
// regression (pool-adjacent-violators) of each item towards its neighbours under the order and the least separations, the
// longest chain of blocks held straight, a passing wire held level, a boundary following what it feeds without pulling it.
// A wire carried to the next turn, and a wire that runs back against the stages (a fault), returns leftwards beneath the
// columns, each on a lane of its own, nested so that none crosses another at its ends; it leaves and meets a block at its foot,
// or at its side when another block stands beneath it; a block's wire to itself is a small petal at its lower corner.
//
// layoutGraph returns a scene: columns, boxes, wires, dots and texts, every text with its extent, so that checks can see
// that nothing overlaps; draw.js writes it as SVG.

export const FONT = `'Source Serif 4', Georgia, serif`;
export const PALETTE = {
  ink: '#2E2233', muted: '#6E5C78', moss: '#557A5C', sand: '#A0784A', orchid: '#9C5590', lilac: '#7864B0', rose: '#B8577F',
  line: '#CDBFD3', paper: '#FFFFFF',
};
// The registers by name; one not named here takes the next of the spare tints in the order the script declares them.
export const REGISTERS = {
  Ontological: { tint: '#F1EDF6', ink: PALETTE.moss },
  Relational: { tint: '#FAF2F7', ink: PALETTE.orchid },
  Manifestational: { tint: '#F7F2EA', ink: PALETTE.sand },
};
const SPARE = [{ tint: '#EEF3EF', ink: PALETTE.moss }, { tint: '#F2EFF8', ink: PALETTE.lilac }, { tint: '#F9F0F3', ink: PALETTE.rose }];

// Methods by family, verdicts by status: each a glyph and a word, never a colour alone.
export const FAMILIES = { formal: '◇', statistical: '○', declared: '□', proposal: '△' };
export const VERDICTS = {
  holds: { mark: '✓', word: 'admitted', fill: PALETTE.moss },
  fails: { mark: '✗', word: 'refused', fill: PALETTE.rose },
  undecided: { mark: '…', word: 'undecided', fill: PALETTE.muted },
};

const SIZES = { name: 13, small: 10.5, method: 10, wire: 9.5, stage: 12.5, register: 10, key: 10, footer: 10.5 };

// The width a text takes in the serif, near enough for a layout: 0.56 of its size a letter (0.6 bold), a glyph or a mark about
// nine tenths of a square.
export function textWidth(text, size = 13, bold = false) {
  let w = 0;
  for (const ch of String(text ?? '')) w += ch.codePointAt(0) >= 0x2010 && ch !== '·' ? size * 0.9 : size * (bold ? 0.6 : 0.56);
  return w;
}

// Isotonic regression of u (non-decreasing) with weights w, by pooling adjacent violators.
function isotonic(u, w) {
  const pools = [];
  for (let i = 0; i < u.length; i++) {
    pools.push({ s: w[i] * u[i], w: w[i], n: 1 });
    while (pools.length > 1) {
      const b = pools[pools.length - 1], a = pools[pools.length - 2];
      if (a.s / a.w <= b.s / b.w) break;
      a.s += b.s; a.w += b.w; a.n += b.n; pools.pop();
    }
  }
  const z = [];
  for (const p of pools) for (let k = 0; k < p.n; k++) z.push(p.s / p.w);
  return z;
}

// A report's methods and checks, by block.
function methodsOf(report) {
  const by = new Map();
  for (const m of (report && report.methods) || []) {
    if (!by.has(m.block)) by.set(m.block, []);
    by.get(m.block).push(m);
  }
  return by;
}

export function layoutGraph(architecture, report = null, options = {}) {
  const A = architecture;
  const S = { ...SIZES, ...(options.sizes || {}) };
  const ground = options.ground || PALETTE.paper;
  const registers = new Map();
  let spare = 0;
  for (const r of A.registers || []) registers.set(r, REGISTERS[r] || SPARE[spare++ % SPARE.length]);
  const registerOf = (name) => registers.get(name) || { tint: '#F4F1F5', ink: PALETTE.muted };
  const stages = (A.stages || []).map((s, i) => ({ ...s, index: i }));
  const stageIndex = new Map(stages.map((s) => [s.name, s.index]));
  const blocks = (A.blocks || []).filter((b) => stageIndex.has(b.stage));
  const blockIndex = new Map(blocks.map((b, i) => [b.name, i]));
  const methods = methodsOf(report);

  // ---- wires: forward, or returning (carried to the next turn, or a fault that runs back) ----
  const wires = [];
  for (const w of A.wires || []) {
    const a = blockIndex.get(w.from), b = blockIndex.get(w.to);
    if (a === undefined || b === undefined) continue;
    const out = blocks[a].outputs.find((p) => p.port === w.out), inn = blocks[b].inputs.find((p) => p.port === w.in);
    if (!out || !inn) continue;
    const sa = stageIndex.get(blocks[a].stage), sb = stageIndex.get(blocks[b].stage);
    const kind = w.next ? 'next' : (sa > sb || a === b) ? 'backward' : 'forward';
    wires.push({ ...w, a, b, type: out.type, kind });
  }
  // Within a stage, a cycle among forward wires is broken where a depth-first walk finds it running back.
  {
    const succ = blocks.map(() => []);
    wires.forEach((w, k) => { if (w.kind === 'forward' && blocks[w.a].stage === blocks[w.b].stage) succ[w.a].push(k); });
    const state = blocks.map(() => 0);
    const visit = (v) => {
      state[v] = 1;
      for (const k of succ[v]) {
        const u = wires[k].b;
        if (state[u] === 1) wires[k].kind = 'backward';
        else if (state[u] === 0) visit(u);
      }
      state[v] = 2;
    };
    blocks.forEach((_, v) => { if (!state[v]) visit(v); });
  }
  const forward = wires.filter((w) => w.kind === 'forward');
  const returning = wires.filter((w) => w.kind !== 'forward');

  // ---- depth within each stage, and the layers ----
  const depth = blocks.map(() => 0);
  for (let pass = 0; pass <= blocks.length; pass++) {
    let moved = false;
    for (const w of forward) if (blocks[w.a].stage === blocks[w.b].stage && depth[w.b] < depth[w.a] + 1) { depth[w.b] = depth[w.a] + 1; moved = true; }
    if (!moved) break;
  }
  const fed = new Set(forward.map((w) => w.b));
  const stageDepth = stages.map((s) => Math.max(0, ...blocks.map((b, i) => (b.stage === s.name ? depth[i] : 0))));
  blocks.forEach((b, i) => {
    if (fed.has(i)) return;
    const own = forward.filter((w) => w.a === i && blocks[w.b].stage === b.stage).map((w) => depth[w.b]);
    const later = forward.some((w) => w.a === i);
    if (own.length) depth[i] = Math.max(0, Math.min(...own) - 1);
    else if (later) depth[i] = stageDepth[stageIndex.get(b.stage)];
  });
  const offset = [];
  let total = 0;
  stages.forEach((s, k) => { offset[k] = total; total += stageDepth[k] + 1; });
  const layerStage = [];
  stages.forEach((s, k) => { for (let d = 0; d <= stageDepth[k]; d++) layerStage.push(k); });
  const layerOf = blocks.map((b, i) => offset[stageIndex.get(b.stage)] + depth[i]);

  // ---- the boxes' contents and sizes ----
  const returningAt = new Set(returning.flatMap((w) => [w.a, w.b]));
  const boxes = blocks.map((b, i) => {
    const referred = b.laws.includes('referred');
    const rows = [{ text: b.name, size: S.name, bold: true, cls: 'kb-name' }, { text: `yields ${b.yields}`, size: S.small, cls: 'kb-yields' }];
    if (b.laws.length) rows.push({ text: b.laws.join(', '), size: S.small, italic: true, cls: 'kb-laws' });
    const ms = (methods.get(b.name) || []).map((m) => {
      const v = VERDICTS[m.status] || VERDICTS.undecided;
      const left = `${FAMILIES[m.family] || '·'} ${m.method}`, right = `${v.mark} ${v.word}`;
      return { ...m, left, right, verdict: v, width: textWidth(left, S.method) + 14 + textWidth(right, S.method) };
    });
    const w = Math.max(104, ...rows.map((r) => textWidth(r.text, r.size, r.bold) + 22), ...ms.map((m) => m.width + 18));
    return { block: b, index: i, name: b.name, boundary: b.boundary, referred, rows, methods: ms, w, layer: layerOf[i], bottomWires: returningAt.has(i) };
  });

  // ---- the items of each layer: blocks, and a place for each port's wires in each layer they pass over ----
  const nodes = boxes.map((bx, i) => ({ id: `b:${bx.name}`, block: i, layer: bx.layer, group: bx.boundary ? 0 : bx.bottomWires ? 2 : 1 }));
  const passing = new Map();
  const chains = forward.map((w) => {
    const chain = [w.a];
    for (let L = layerOf[w.a] + 1; L < layerOf[w.b]; L++) {
      const key = `${w.from}.${w.out}@${L}`;
      if (!passing.has(key)) {
        // A wire that passes over the whole of this layer's stage may run above the stage's boundaries.
        const k = layerStage[L], over = stageIndex.get(blocks[w.a].stage) < k && stageIndex.get(blocks[w.b].stage) > k;
        passing.set(key, nodes.length);
        nodes.push({ id: `d:${key}`, block: null, layer: L, group: over ? 0 : 1, port: `${w.from}.${w.out}` });
      } else if (!(stageIndex.get(blocks[w.a].stage) < layerStage[L] && stageIndex.get(blocks[w.b].stage) > layerStage[L])) nodes[passing.get(key)].group = 1;
      chain.push(passing.get(key));
    }
    chain.push(w.b);
    return chain;
  });
  const layerCount = Math.max(total, 1);
  const layers = Array.from({ length: layerCount }, () => []);
  nodes.forEach((n, v) => layers[n.layer].push(v));
  const segs = new Map(); // "u>v" → { u, v, wires: [k] }
  chains.forEach((chain, k) => {
    for (let j = 1; j < chain.length; j++) {
      const key = `${chain[j - 1]}>${chain[j]}`;
      if (!segs.has(key)) segs.set(key, { u: chain[j - 1], v: chain[j], wires: [] });
      segs.get(key).wires.push(k);
    }
  });
  const preds = nodes.map(() => []), succs = nodes.map(() => []);
  for (const s of segs.values()) { preds[s.v].push(s.u); succs[s.u].push(s.v); }

  // ---- the order within each layer ----
  const pos = new Array(nodes.length).fill(0);
  const place = () => layers.forEach((list) => list.forEach((v, k) => { pos[v] = k; }));
  place();
  const crossings = (L) => {
    const es = [];
    for (const v of layers[L + 1] || []) for (const u of preds[v]) es.push([pos[u], pos[v]]);
    let c = 0;
    for (let i = 0; i < es.length; i++) for (let j = i + 1; j < es.length; j++) if ((es[i][0] - es[j][0]) * (es[i][1] - es[j][1]) < 0) c++;
    return c;
  };
  const allCrossings = () => { let c = 0; for (let L = 0; L + 1 < layers.length; L++) c += crossings(L); return c; };
  const sortLayer = (L, toward) => {
    const bary = new Map();
    for (const v of layers[L]) {
      const ns = toward[v];
      bary.set(v, ns.length ? ns.reduce((s, u) => s + pos[u], 0) / ns.length : pos[v]);
    }
    // By barycentre, at the top what enters from outside (boundaries) and the wires that pass over the whole of a stage; beneath
    // them the rest, the blocks whose wires return beneath standing below the other blocks (a passing wire may stand anywhere
    // among them), so that no block stands in the way of a returning wire.
    const sorted = [...layers[L]].sort((x, y) => nodes[x].group - nodes[y].group || bary.get(x) - bary.get(y) || pos[x] - pos[y]);
    const top = sorted.filter((v) => nodes[v].group === 0);
    const rest = [...layers[L]].filter((v) => nodes[v].group !== 0).sort((x, y) => bary.get(x) - bary.get(y) || pos[x] - pos[y]);
    const slots = rest.map((v, k) => (nodes[v].block !== null ? k : -1)).filter((k) => k >= 0);
    const ranked = rest.filter((v) => nodes[v].block !== null).sort((x, y) => nodes[x].group - nodes[y].group || bary.get(x) - bary.get(y) || pos[x] - pos[y]);
    slots.forEach((k, j) => { rest[k] = ranked[j]; });
    layers[L] = [...top, ...rest];
    layers[L].forEach((v, k) => { pos[v] = k; });
  };
  const transpose = () => {
    let better = true, rounds = 0;
    while (better && rounds++ < 12) {
      better = false;
      for (let L = 0; L < layers.length; L++) {
        for (let k = 0; k + 1 < layers[L].length; k++) {
          const x = layers[L][k], y = layers[L][k + 1];
          const gx = nodes[x].group, gy = nodes[y].group;
          if (gx !== gy && (gx === 0 || gy === 0 || (nodes[x].block !== null && nodes[y].block !== null))) continue;
          const before = (L > 0 ? crossings(L - 1) : 0) + crossings(L);
          layers[L][k] = y; layers[L][k + 1] = x; pos[x] = k + 1; pos[y] = k;
          const after = (L > 0 ? crossings(L - 1) : 0) + crossings(L);
          if (after < before) better = true;
          else { layers[L][k] = x; layers[L][k + 1] = y; pos[x] = k; pos[y] = k + 1; }
        }
      }
    }
  };
  let best = layers.map((l) => [...l]), bestCrossings = Infinity;
  for (let sweep = 0; sweep < 16; sweep++) {
    if (sweep % 2 === 0) for (let L = 1; L < layers.length; L++) sortLayer(L, preds);
    else for (let L = layers.length - 2; L >= 0; L--) sortLayer(L, succs);
    transpose();
    const c = allCrossings();
    if (c < bestCrossings) { bestCrossings = c; best = layers.map((l) => [...l]); }
  }
  best.forEach((l, L) => { layers[L] = l; });
  place();

  // ---- ports on the boxes' sides, in the order of what they meet ----
  const SPACE_IN = 10, SPACE_OUT = 13;
  boxes.forEach((bx, i) => {
    const b = bx.block;
    const inKey = (p) => {
      const ks = forward.map((w, k) => ({ w, k })).filter(({ w }) => w.b === i && w.in === p.port).map(({ k }) => pos[chains[k][chains[k].length - 2]]);
      return ks.length ? ks[0] : Infinity;
    };
    const outKey = (p) => {
      const ks = forward.map((w, k) => ({ w, k })).filter(({ w }) => w.a === i && w.out === p.port).map(({ k }) => pos[chains[k][1]]);
      return ks.length ? ks.reduce((s, x) => s + x, 0) / ks.length : Infinity;
    };
    const sided = (list, key, carried) => list.map((p, k) => ({ ...p, key: key(p), k }))
      .filter((p) => p.key !== Infinity || !carried(p))
      .sort((x, y) => x.key - y.key || x.k - y.k);
    // A block with another beneath it in its layer meets its returning wires at its sides, not at its foot.
    const list = layers[bx.layer];
    bx.blocked = list.slice(list.indexOf(i) + 1).some((v) => nodes[v].block !== null);
    const inReturning = (p) => returning.some((w) => w.b === i && w.in === p.port && (w.a === i || !bx.blocked));
    const outReturning = (p) => returning.some((w) => w.a === i && w.out === p.port && (w.b === i || !bx.blocked));
    bx.left = sided(b.inputs, inKey, inReturning);
    bx.right = sided(b.outputs, outKey, outReturning);
    bx.right.forEach((p) => { p.labelled = forward.some((w) => w.a === i && w.out === p.port); p.wired = returning.some((w) => w.a === i && w.out === p.port) || p.labelled; });
    bx.left.forEach((p) => { p.wired = forward.some((w) => w.b === i && w.in === p.port) || returning.some((w) => w.b === i && w.in === p.port); });
    const content = bx.rows.length === 3 ? 52 : 40;
    const need = Math.max((bx.left.length - 1) * SPACE_IN + 20, (bx.right.length - 1) * SPACE_OUT + 22);
    bx.body = Math.max(content, need);
    bx.contentTop = (bx.body - content) / 2;
    bx.h = bx.body + (bx.methods.length ? bx.methods.length * 14 + 7 : 0);
    const spread = (list, space) => list.forEach((p, k) => { p.dy = bx.body / 2 + (k - (list.length - 1) / 2) * space; });
    spread(bx.left, SPACE_IN);
    spread(bx.right, SPACE_OUT);
  });
  const height = (v) => (nodes[v].block === null ? 0 : boxes[nodes[v].block].h);
  // Where a forward wire meets an item, relative to the item's centre.
  const outAt = (v, wire) => (nodes[v].block === null ? 0 : (() => { const bx = boxes[nodes[v].block]; return bx.right.find((p) => p.port === wire.out).dy - bx.h / 2; })());
  const inAt = (v, wire) => (nodes[v].block === null ? 0 : (() => { const bx = boxes[nodes[v].block]; return bx.left.find((p) => p.port === wire.in).dy - bx.h / 2; })());
  // The spine: the longest chain of blocks along forward wires, drawn straight. What enters from outside follows what it feeds
  // and does not pull it.
  const spine = new Set();
  {
    const longest = blocks.map(() => 1), via = blocks.map(() => -1);
    const byLayer = blocks.map((_, i) => i).sort((p, q) => layerOf[p] - layerOf[q] || p - q);
    for (const v of byLayer) forward.forEach((w, k) => { if (w.a === v && longest[v] + 1 > longest[w.b]) { longest[w.b] = longest[v] + 1; via[w.b] = k; } });
    let end = -1;
    blocks.forEach((_, i) => { if (end < 0 || longest[i] > longest[end]) end = i; });
    while (end >= 0 && via[end] >= 0) { spine.add(via[end]); end = forward[via[end]].a; }
  }
  const links = []; // { u, v, du, dv, wIn (u's pull on v), wOut (v's pull on u) }
  for (const s of segs.values()) {
    const wire = forward[s.wires[0]];
    const du = outAt(s.u, wire), dv = inAt(s.v, wire);
    let weight = nodes[s.u].block === null && nodes[s.v].block === null ? 200 : 2;
    if (s.wires.some((k) => spine.has(k))) weight = 1000;
    const fromOutside = nodes[s.u].block !== null && boxes[nodes[s.u].block].boundary;
    links.push({ u: s.u, v: s.v, du, dv, wIn: fromOutside ? 0.2 : weight, wOut: weight });
  }
  const linksIn = nodes.map(() => []), linksOut = nodes.map(() => []);
  links.forEach((l) => { linksIn[l.v].push(l); linksOut[l.u].push(l); });

  // ---- heights: each item towards its neighbours, under the order and the least separations ----
  const gapOf = (x, y) => {
    const bx = nodes[x].block !== null, by = nodes[y].block !== null;
    return bx && by ? 28 : bx || by ? 15 : 11;
  };
  const Y = new Array(nodes.length).fill(0);
  const seps = layers.map((list) => list.slice(1).map((v, k) => height(list[k]) / 2 + height(v) / 2 + gapOf(list[k], v)));
  layers.forEach((list, L) => { let y = 0; list.forEach((v, k) => { if (k) y += seps[L][k - 1]; Y[v] = y; }); const mid = y / 2; list.forEach((v) => { Y[v] -= mid; }); });
  const settle = (L, useIn, useOut) => {
    const list = layers[L];
    if (!list.length) return;
    const t = [], w = [];
    for (const v of list) {
      let s = 0, n = 0;
      if (useIn) for (const l of linksIn[v]) { s += l.wIn * (Y[l.u] + l.du - l.dv); n += l.wIn; }
      if (useOut) for (const l of linksOut[v]) { s += l.wOut * (Y[l.v] + l.dv - l.du); n += l.wOut; }
      if (n) { t.push(s / n); w.push(n); } else { t.push(Y[v]); w.push(0.05); }
    }
    const c = [0];
    for (let k = 1; k < list.length; k++) c.push(c[k - 1] + seps[L][k - 1]);
    const z = isotonic(t.map((x, k) => x - c[k]), w);
    list.forEach((v, k) => { Y[v] = z[k] + c[k]; });
  };
  // Sweeps from one side first, to set the items out; then from both sides, alternately left to right and back, until they rest.
  for (let round = 0; round < 6; round++) {
    if (round % 2 === 0) for (let L = 1; L < layers.length; L++) settle(L, true, false);
    else for (let L = layers.length - 2; L >= 0; L--) settle(L, false, true);
  }
  for (let round = 0; round < 120; round++) {
    const before = Y.slice();
    if (round % 2 === 0) for (let L = 0; L < layers.length; L++) settle(L, true, true);
    else for (let L = layers.length - 1; L >= 0; L--) settle(L, true, true);
    if (Y.every((y, v) => Math.abs(y - before[v]) < 0.01)) break;
  }

  // A passing wire that wavers by a few points is drawn level: each run of its places within 14 points of one another is set to
  // one height, the nearest to their mean that keeps every place clear of its neighbours.
  {
    const room = (v) => {
      const list = layers[nodes[v].layer], k = list.indexOf(v), L = nodes[v].layer;
      const lo = k > 0 ? Y[list[k - 1]] + seps[L][k - 1] : -Infinity, hi = k + 1 < list.length ? Y[list[k + 1]] - seps[L][k] : Infinity;
      return [lo, hi];
    };
    const byPort = new Map();
    nodes.forEach((n, v) => { if (n.block === null) { if (!byPort.has(n.port)) byPort.set(n.port, []); byPort.get(n.port).push(v); } });
    for (const list of byPort.values()) {
      list.sort((p, q) => nodes[p].layer - nodes[q].layer);
      let start = 0;
      for (let k = 1; k <= list.length; k++) {
        const ys = list.slice(start, k + 1).map((v) => Y[v]);
        if (k < list.length && nodes[list[k]].layer === nodes[list[k - 1]].layer + 1 && Math.max(...ys) - Math.min(...ys) < 14) continue;
        const span = list.slice(start, k);
        if (span.length > 1) {
          let lo = -Infinity, hi = Infinity;
          for (const v of span) { const [a, b] = room(v); lo = Math.max(lo, a); hi = Math.min(hi, b); }
          const mean = span.reduce((t, v) => t + Y[v], 0) / span.length;
          if (lo <= hi) span.forEach((v) => { Y[v] = Math.min(hi, Math.max(lo, mean)); });
        }
        start = k;
      }
    }
  }

  // ---- the frame: headers above, columns of layers, lanes beneath ----
  const HEAD = 58, TOP = 8;
  let minTop = Infinity, maxBottom = -Infinity;
  nodes.forEach((n, v) => { minTop = Math.min(minTop, Y[v] - height(v) / 2); maxBottom = Math.max(maxBottom, Y[v] + height(v) / 2); });
  if (!Number.isFinite(minTop)) { minTop = 0; maxBottom = 40; }
  const shiftY = TOP + HEAD + 12 - minTop;
  nodes.forEach((n, v) => { Y[v] += shiftY; });
  maxBottom += shiftY;

  // Widths: a layer is as wide as its widest box; the gap after it holds its ports' labels and the bend of a wire.
  const labelOf = (type) => textWidth(type, S.wire) + 8;
  const layerW = layers.map((list) => Math.max(18, ...list.filter((v) => nodes[v].block !== null).map((v) => boxes[nodes[v].block].w)));
  const overhang = layers.map((list, L) => Math.max(0, ...list.filter((v) => nodes[v].block !== null).flatMap((v) => {
    const bx = boxes[nodes[v].block];
    return bx.right.filter((p) => p.labelled).map((p) => labelOf(p.type) - (layerW[L] - bx.w) / 2);
  })));
  const BEND = 38, PAD = 16, COLGAP = 6, ENTRY = 26;
  const layerX = [], columns = [];
  let x = 10;
  stages.forEach((s, k) => {
    const mine = layers.map((_, L) => L).filter((L) => layerStage[L] === k);
    const left = x;
    x += k === 0 ? PAD : ENTRY;
    mine.forEach((L, j) => {
      layerX[L] = x;
      x += layerW[L];
      x += j < mine.length - 1 ? Math.max(48, overhang[L] + BEND) : Math.max(PAD, overhang[L] + 10);
    });
    const header = Math.max(textWidth(s.name, S.stage, true), textWidth(s.register || '', S.register)) + 20;
    if (x - left < header) x = left + header;
    columns.push({ stage: s.name, register: s.register, left, right: x, ...registerOf(s.register) });
    x += COLGAP;
  });
  const frameRight = x - COLGAP;

  // ---- boxes placed ----
  boxes.forEach((bx, i) => {
    const v = i; // a block's item is its index
    bx.x = layerX[bx.layer] + (layerW[bx.layer] - bx.w) / 2;
    bx.y = Y[v] - bx.h / 2;
  });
  const tintAt = (px) => (columns.find((c) => px >= c.left && px <= c.right) || { tint: ground }).tint;

  const texts = [];
  const text = (t) => { const w = textWidth(t.text, t.size, t.bold); const x0 = t.anchor === 'middle' ? t.x - w / 2 : t.anchor === 'end' ? t.x - w : t.x; texts.push({ ...t, w, box: { x: x0, y: t.y - t.size * 0.78, w, h: t.size * 1.0 } }); };

  // Headers.
  columns.forEach((c) => {
    text({ x: c.left + 10, y: TOP + 22, text: c.stage, size: S.stage, bold: true, fill: c.ink, cls: 'kb-stage', owner: `stage:${c.stage}` });
    text({ x: c.left + 10, y: TOP + 38, text: c.register || '', size: S.register, italic: true, fill: PALETTE.muted, cls: 'kb-register', owner: `stage:${c.stage}` });
  });

  // Box texts.
  boxes.forEach((bx) => {
    const cx = bx.x + bx.w / 2;
    const base = [17, 30, 43];
    bx.rows.forEach((r, k) => text({ x: cx, y: bx.y + bx.contentTop + base[k], text: r.text, size: r.size, bold: r.bold, italic: r.italic, anchor: 'middle',
      fill: r.bold ? PALETTE.ink : PALETTE.muted, cls: r.cls, owner: `block:${bx.name}` }));
    bx.methods.forEach((m, k) => {
      const y = bx.y + bx.body + 15 + k * 14;
      text({ x: bx.x + 9, y, text: m.left, size: S.method, fill: PALETTE.ink, cls: 'kb-method', owner: `block:${bx.name}` });
      text({ x: bx.x + bx.w - 9, y, text: m.right, size: S.method, anchor: 'end', fill: m.verdict.fill, italic: true, cls: `kb-verdict kb-${m.status}`, owner: `block:${bx.name}` });
    });
  });

  // ---- forward wires ----
  const f1 = (v) => v.toFixed(1);
  const bend = (p, q) => {
    if (Math.abs(p.y - q.y) < 0.5) return ` L${f1(q.x)} ${f1(q.y)}`;
    const dx = (q.x - p.x) / 2;
    return ` C${f1(p.x + dx)} ${f1(p.y)} ${f1(q.x - dx)} ${f1(q.y)} ${f1(q.x)} ${f1(q.y)}`;
  };
  const drawn = [];
  const dots = [];
  const labelled = new Set();
  forward.forEach((w, k) => {
    const chain = chains[k];
    const src = boxes[w.a], tgt = boxes[w.b];
    const out = src.right.find((p) => p.port === w.out), inn = tgt.left.find((p) => p.port === w.in);
    const start = { x: src.x + src.w, y: src.y + out.dy };
    const points = [start, { x: start.x + labelOf(w.type), y: start.y }];
    for (let j = 1; j < chain.length - 1; j++) {
      const L = nodes[chain[j]].layer;
      points.push({ x: layerX[L], y: Y[chain[j]], straight: true }, { x: layerX[L] + layerW[L], y: Y[chain[j]] });
    }
    const end = { x: tgt.x, y: tgt.y + inn.dy };
    points.push({ x: end.x - 5.5, y: end.y });
    let d = `M${f1(points[0].x)} ${f1(points[0].y)}`;
    for (let j = 1; j < points.length; j++) d += points[j].straight || j === 1 ? ` L${f1(points[j].x)} ${f1(points[j].y)}` : bend(points[j - 1], points[j]);
    drawn.push({ ...w, d, dashed: false, head: { x: end.x, y: end.y, dir: 'right' }, points });
    const key = `${w.from}.${w.out}`;
    if (!labelled.has(key)) {
      labelled.add(key);
      text({ x: start.x + 4, y: start.y - 3.5, text: w.type, size: S.wire, italic: false, fill: PALETTE.muted, halo: tintAt(start.x + 4), cls: 'kb-type', owner: `wire:${key}` });
      dots.push({ x: start.x, y: start.y, filled: true });
    }
  });
  // Ports with no wire: a hollow dot.
  boxes.forEach((bx) => {
    bx.left.filter((p) => !p.wired).forEach((p) => dots.push({ x: bx.x, y: bx.y + p.dy, filled: false }));
    bx.right.filter((p) => !p.wired).forEach((p) => dots.push({ x: bx.x + bx.w, y: bx.y + p.dy, filled: false }));
  });

  // ---- returning wires: lanes beneath, nested; a wire from a block to itself a small loop at its foot ----
  const LANE = 16, R = 6;
  const selfLoops = returning.filter((w) => w.a === w.b);
  const loopsAt = new Set(selfLoops.map((w) => w.a));
  const laneTop = maxBottom + 24;
  const spans = returning.filter((w) => w.a !== w.b).map((w, k) => {
    const src = boxes[w.a], tgt = boxes[w.b];
    const sideOut = src.blocked, sideIn = tgt.blocked;
    const lo = sideIn ? layerX[tgt.layer] - 24 : tgt.x, hi = sideOut ? layerX[src.layer] + layerW[src.layer] + overhang[src.layer] + 24 : src.x + src.w;
    return { k, w, lo, hi, sideOut, sideIn };
  }).sort((p, q) => (p.hi - p.lo) - (q.hi - q.lo) || p.k - q.k);
  const lanes = [];
  for (const s of spans) {
    let lane = 0;
    while (lanes[lane] && lanes[lane].some((o) => !(s.hi + 10 < o.lo || o.hi + 10 < s.lo))) lane++;
    (lanes[lane] = lanes[lane] || []).push(s);
    s.lane = lane;
  }
  // The ends on each box's foot: wires out at the right, the outermost lane nearest the corner; wires in at the left, likewise.
  // At a side, in the gap beside the layer: the outermost lane farthest from the box.
  const exits = new Map(), entries = new Map();
  for (const s of spans) {
    const ko = `${s.sideOut ? 'side' : 'foot'}:${s.sideOut ? boxes[s.w.a].layer : s.w.a}`, ki = `${s.sideIn ? 'side' : 'foot'}:${s.sideIn ? boxes[s.w.b].layer : s.w.b}`;
    if (!exits.has(ko)) exits.set(ko, []);
    if (!entries.has(ki)) entries.set(ki, []);
    exits.get(ko).push(s); entries.get(ki).push(s);
  }
  for (const [key, list] of exits) {
    const [mode, at] = key.split(':'), n = Number(at);
    if (mode === 'foot') list.sort((p, q) => q.lane - p.lane).forEach((s, j) => { s.x0 = boxes[n].x + boxes[n].w - 12 - (loopsAt.has(n) ? 30 : 0) - j * 9; });
    else list.sort((p, q) => p.lane - q.lane).forEach((s, j) => { s.x0 = layerX[n] + layerW[n] + overhang[n] + 10 + j * 6; });
  }
  for (const [key, list] of entries) {
    const [mode, at] = key.split(':'), n = Number(at);
    if (mode === 'foot') list.sort((p, q) => q.lane - p.lane).forEach((s, j) => { s.x1 = boxes[n].x + 12 + j * 9; });
    else list.sort((p, q) => p.lane - q.lane).forEach((s, j) => { s.x1 = layerX[n] - 10 - j * 6; });
  }
  const wordOf = (w) => (w.kind === 'next' ? `next · ${w.type}` : `${w.type} · backward`);
  const inkOf = (w) => (w.kind === 'next' ? PALETTE.lilac : PALETTE.rose);
  const clsOf = (w) => (w.kind === 'next' ? 'kb-next-label' : 'kb-backward-label');
  let laneBottom = maxBottom;
  for (const w of selfLoops) {
    // A petal at the box's lower right corner: out of its right side, back in at its foot.
    const bx = boxes[w.a], xr = bx.x + bx.w, yb = bx.y + bx.h;
    const d = `M${f1(xr)} ${f1(yb - 9)} C${f1(xr + 20)} ${f1(yb - 9)} ${f1(xr - 14)} ${f1(yb + 22)} ${f1(xr - 14)} ${f1(yb + 5.5)}`;
    drawn.push({ ...w, d, dashed: true, lane: -1, loop: true, head: { x: xr - 14, y: yb, dir: 'up' } });
    dots.push({ x: xr, y: yb - 9, filled: true });
    const right = exits.has(w.a);
    text({ x: right ? xr + 11 : xr - 22, y: yb + 13, text: wordOf(w), size: S.wire, italic: true, anchor: right ? 'start' : 'end', fill: inkOf(w), halo: tintAt(xr),
      cls: clsOf(w), owner: `wire:${w.from}.${w.out}->${w.to}.${w.in}` });
    laneBottom = Math.max(laneBottom, yb + 16);
  }
  for (const s of spans) {
    const src = boxes[s.w.a], tgt = boxes[s.w.b];
    const y = laneTop + s.lane * LANE;
    laneBottom = Math.max(laneBottom, y);
    let d, head;
    if (s.sideOut) {
      const py = src.y + src.right.find((p) => p.port === s.w.out).dy;
      d = `M${f1(src.x + src.w)} ${f1(py)} H${f1(s.x0 - R)} Q${f1(s.x0)} ${f1(py)} ${f1(s.x0)} ${f1(py + R)}`;
      dots.push({ x: src.x + src.w, y: py, filled: true });
    } else {
      d = `M${f1(s.x0)} ${f1(src.y + src.h)}`;
      dots.push({ x: s.x0, y: src.y + src.h, filled: true });
    }
    d += ` V${f1(y - R)} Q${f1(s.x0)} ${f1(y)} ${f1(s.x0 - R)} ${f1(y)} H${f1(s.x1 + R)} Q${f1(s.x1)} ${f1(y)} ${f1(s.x1)} ${f1(y - R)}`;
    if (s.sideIn) {
      const py = tgt.y + tgt.left.find((p) => p.port === s.w.in).dy;
      d += ` V${f1(py + R)} Q${f1(s.x1)} ${f1(py)} ${f1(s.x1 + R)} ${f1(py)} H${f1(tgt.x - 5.5)}`;
      head = { x: tgt.x, y: py, dir: 'right' };
    } else {
      d += ` V${f1(tgt.y + tgt.h + 5.5)}`;
      head = { x: s.x1, y: tgt.y + tgt.h, dir: 'up' };
    }
    drawn.push({ ...s.w, d, dashed: true, lane: s.lane, head });
    const mid = (s.x0 + s.x1) / 2;
    text({ x: mid, y: y + 3.4, text: wordOf(s.w), size: S.wire, italic: true, anchor: 'middle', fill: inkOf(s.w), halo: tintAt(mid),
      cls: clsOf(s.w), owner: `wire:${s.w.from}.${s.w.out}->${s.w.to}.${s.w.in}` });
  }
  const columnBottom = laneBottom + 18;
  columns.forEach((c) => { c.top = TOP; c.bottom = columnBottom; });

  // ---- the key, and with a report the checks ----
  const key = [];
  let ky = columnBottom + 26, kx = 14;
  const keyRight = Math.max(frameRight, 560);
  const item = (width) => { if (kx + width > keyRight && kx > 14) { kx = 14; ky += 20; } const at = { x: kx, y: ky }; kx += width + 18; return at; };
  const keyText = (x0, y0, t, extra = {}) => text({ x: x0, y: y0 + 3.5, text: t, size: S.key, fill: PALETTE.muted, cls: 'kb-key-text', owner: 'key', ...extra });
  {
    const tw = textWidth('wire', S.key);
    const at = item(34 + tw);
    key.push({ kind: 'wire', x: at.x, y: at.y }); keyText(at.x + 34, at.y, 'wire');
  }
  {
    const label = 'next: carried to the following turn';
    const at = item(34 + textWidth(label, S.key));
    key.push({ kind: 'next', x: at.x, y: at.y }); keyText(at.x + 34, at.y, label);
  }
  {
    const label = 'boundary: given from outside the loop';
    const at = item(30 + textWidth(label, S.key));
    key.push({ kind: 'boundary', x: at.x, y: at.y }); keyText(at.x + 30, at.y, label);
  }
  {
    const label = 'referred to, not established';
    const at = item(30 + textWidth(label, S.key));
    key.push({ kind: 'referred', x: at.x, y: at.y }); keyText(at.x + 30, at.y, label);
  }
  for (const [name, r] of registers) {
    const at = item(20 + textWidth(name, S.key));
    key.push({ kind: 'register', x: at.x, y: at.y, tint: r.tint, ink: r.ink }); keyText(at.x + 20, at.y, name);
  }
  if (report) {
    for (const [family, glyph] of Object.entries(FAMILIES)) {
      const t = `${glyph} ${family}`;
      const at = item(textWidth(t, S.key));
      keyText(at.x, at.y, t, { fill: PALETTE.ink });
    }
    for (const v of Object.values(VERDICTS)) {
      const t = `${v.mark} ${v.word}`;
      const at = item(textWidth(t, S.key));
      keyText(at.x, at.y, t, { fill: v.fill, italic: true });
    }
  }
  let bottom = ky + 14;
  const footer = [];
  if (report && (report.checks || []).length) {
    const checks = report.checks;
    const count = (s) => checks.filter((c) => c.status === s).length;
    ky = bottom + 14;
    const summary = `checks: ${count('holds')} holding · ${count('fails')} failing · ${count('undecided')} undecided`;
    text({ x: 14, y: ky + 4, text: summary, size: S.footer, fill: PALETTE.ink, cls: 'kb-checks', owner: 'checks' });
    for (const c of checks.filter((c) => c.status !== 'holds')) {
      ky += 16;
      const v = VERDICTS[c.status] || VERDICTS.undecided;
      text({ x: 24, y: ky + 4, text: `${v.mark} ${c.kind ? `${c.kind} · ` : ''}${c.check}`, size: S.footer, fill: v.fill, cls: `kb-check kb-${c.status}`, owner: 'checks' });
      footer.push(c);
    }
    bottom = ky + 14;
  }
  // The architecture's name at the right of the key's first row, or beneath all when the row has no room for it.
  if (options.title !== false && A.name) {
    const t = `${A.name} · version ${A.version}`, right = Math.max(frameRight, keyRight);
    const firstRow = texts.filter((x) => x.owner === 'key' && Math.abs(x.y - (columnBottom + 26 + 3.5)) < 1);
    const room = right - Math.max(0, ...firstRow.map((x) => x.box.x + x.box.w)) > textWidth(t, S.register) + 30;
    text({ x: right, y: room ? columnBottom + 26 + 3.5 : bottom + 6, text: t, size: S.register, anchor: 'end', fill: PALETTE.muted, italic: true, cls: 'kb-title', owner: 'title' });
    if (!room) bottom += 14;
  }
  const width = Math.max(frameRight, keyRight, ...texts.map((t) => t.box.x + t.box.w)) + 10;
  return {
    width, height: bottom + 8, ground, columns, boxes, wires: drawn, dots, texts, key, footer, lanes: lanes.length,
    layers: layers.length, crossings: bestCrossings, errors: A.errors || [],
  };
}
