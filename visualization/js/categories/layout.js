// The layout of a category as a diagram: its objects in layers, its arrows between them.
//
// A category is drawn by arrows that generate it (kategoria's view says which), since every other arrow is a composite
// of those and would only repeat them. The objects are set in layers from left to right, each arrow running forward
// where it can: an arrow that would close a cycle is found by a depth-first search and left out of the layering (it is
// still drawn, running back). Within a layer the objects are ordered by the mean place of their neighbours, swept a few
// times, which removes most crossings of small diagrams (Sugiyama, Tagawa and Toda 1981, their first three steps).
// Parallel arrows are bowed apart; an arrow from an object to itself is a petal.
//
// Pure arithmetic: no page is needed. Lengths are in the units of the drawing (CSS pixels at scale 1).

const WIDTH_OF = (text, size = 14) => [...String(text)].length * size * 0.56;

// The layers of the objects: layer[v] is the longest way to v along forward arrows.
export function layers(count, edges) {
  const out = Array.from({ length: count }, () => []);
  edges.forEach((e, k) => { if (e.dom !== e.cod) out[e.dom].push(k); });
  const state = new Array(count).fill(0), back = new Set();
  const visit = (v) => {
    state[v] = 1;
    for (const k of out[v]) {
      const w = edges[k].cod;
      if (state[w] === 1) back.add(k);
      else if (state[w] === 0) visit(w);
    }
    state[v] = 2;
  };
  for (let v = 0; v < count; v++) if (state[v] === 0) visit(v);
  const layer = new Array(count).fill(0);
  for (let pass = 0; pass < count; pass++) {
    let changed = false;
    edges.forEach((e, k) => {
      if (e.dom === e.cod || back.has(k)) return;
      if (layer[e.cod] < layer[e.dom] + 1) { layer[e.cod] = layer[e.dom] + 1; changed = true; }
    });
    if (!changed) break;
  }
  // An object nothing leads to stands just before the first it leads to, not at the far left.
  for (let v = 0; v < count; v++) {
    const into = edges.some((e, k) => e.cod === v && e.dom !== v && !back.has(k));
    const next = edges.filter((e, k) => e.dom === v && e.cod !== v && !back.has(k)).map((e) => layer[e.cod]);
    if (!into && next.length) layer[v] = Math.max(0, Math.min(...next) - 1);
  }
  return { layer, back };
}

// The order of the objects within their layers, by the mean place of their neighbours.
export function ordered(layer, edges) {
  const depth = Math.max(0, ...layer) + 1;
  const rows = Array.from({ length: depth }, () => []);
  layer.forEach((l, v) => rows[l].push(v));
  const place = new Array(layer.length).fill(0);
  const settle = () => rows.forEach((row) => row.forEach((v, i) => { place[v] = i - (row.length - 1) / 2; }));
  settle();
  const near = layer.map(() => []);
  edges.forEach((e) => { if (e.dom !== e.cod) { near[e.dom].push(e.cod); near[e.cod].push(e.dom); } });
  for (let sweep = 0; sweep < 6; sweep++) {
    const order = sweep % 2 === 0 ? rows.map((_, i) => i) : rows.map((_, i) => depth - 1 - i);
    for (const l of order) {
      const mean = (v) => {
        const others = near[v].filter((w) => layer[w] !== l);
        return others.length ? others.reduce((s, w) => s + place[w], 0) / others.length : place[v];
      };
      const keyed = rows[l].map((v, i) => ({ v, key: mean(v), i }));
      keyed.sort((a, b) => a.key - b.key || a.i - b.i);
      rows[l] = keyed.map((x) => x.v);
      settle();
    }
  }
  return rows;
}

const quadratic = (a, c, b, t) => ({ x: (1 - t) ** 2 * a.x + 2 * (1 - t) * t * c.x + t * t * b.x, y: (1 - t) ** 2 * a.y + 2 * (1 - t) * t * c.y + t * t * b.y });

// Where the line from the centre of an ellipse towards a point leaves it.
const leave = (node, towards, pad = 4) => {
  const dx = towards.x - node.x, dy = towards.y - node.y, d = Math.hypot(dx, dy) || 1;
  const r = 1 / Math.hypot(dx / d / (node.rx + pad), dy / d / (node.ry + pad));
  return { x: node.x + (dx / d) * r, y: node.y + (dy / d) * r };
};

// The layout of a category's view: { width, height, nodes, edges }.
//   view     kategoria's: { objects: [names], arrows: [{ name, dom, cod, kind }], generators?: [arrow] }
//   options  { arrows?: the arrows to draw (default: the generators, or every arrow that is no identity where none are given),
//              gapX?, gapY?, size? }
export function layoutCategory(view, options = {}) {
  const size = options.size || 14;
  const drawn = options.arrows || (view.generators && view.generators.length ? view.generators : view.arrows.map((a, i) => i).filter((i) => view.arrows[i].kind !== 'identity'));
  const edges = drawn.map((i) => ({ arrow: i, dom: view.arrows[i].dom, cod: view.arrows[i].cod, name: view.arrows[i].name }));
  const count = view.objects.length;
  const { layer } = layers(count, edges);
  const rows = ordered(layer, edges);
  const nodes = view.objects.map((name) => ({ label: name, rx: Math.max(13, WIDTH_OF(name, size) / 2 + 7), ry: 13, x: 0, y: 0 }));
  const widest = rows.map((row) => Math.max(0, ...row.map((v) => nodes[v].rx)));
  // The room an arrow's name needs between two layers.
  const room = rows.map(() => 0);
  edges.forEach((e) => { if (e.dom !== e.cod) { const l = Math.min(layer[e.dom], layer[e.cod]); room[l] = Math.max(room[l], WIDTH_OF(e.name, size * 0.86)); } });
  const loops = nodes.map(() => []);
  edges.forEach((e, k) => { if (e.dom === e.cod) loops[e.dom].push(k); });
  const petal = 46 + Math.max(0, ...edges.filter((e) => e.dom === e.cod).map((e) => WIDTH_OF(e.name, size * 0.86)));
  const gapY = options.gapY || 64 + (loops.some((l) => l.length) ? 34 : 0);
  let x = 0;
  const xs = rows.map((row, l) => {
    const here = x + widest[l];
    x = here + widest[l] + Math.max(options.gapX || 74, room[l] + 46);
    return here;
  });
  const tallest = Math.max(1, ...rows.map((row) => row.length));
  rows.forEach((row, l) => row.forEach((v, i) => { nodes[v].x = xs[l]; nodes[v].y = (i - (row.length - 1) / 2) * gapY + ((tallest - 1) / 2) * gapY; }));

  // Arrows between two objects, bowed apart where several run between the same two.
  const groups = new Map();
  edges.forEach((e, k) => {
    if (e.dom === e.cod) return;
    const key = e.dom < e.cod ? `${e.dom}:${e.cod}` : `${e.cod}:${e.dom}`;
    if (!groups.has(key)) groups.set(key, []);
    groups.get(key).push(k);
  });
  const out = edges.map((e) => ({ arrow: e.arrow, name: e.name, loop: e.dom === e.cod }));
  for (const members of groups.values()) {
    members.forEach((k, j) => {
      const e = edges[k], a = nodes[e.dom], b = nodes[e.cod];
      const dx = b.x - a.x, dy = b.y - a.y, d = Math.hypot(dx, dy) || 1;
      // The normal is taken for the pair in a fixed order, so that arrows running opposite ways bow apart.
      const flip = e.dom < e.cod ? 1 : -1;
      const nx = (-dy / d) * flip, ny = (dx / d) * flip;
      let bow = (j - (members.length - 1) / 2) * 30;
      const span = Math.abs(layer[e.cod] - layer[e.dom]);
      if (members.length === 1 && span > 1 && rows.slice(Math.min(layer[e.dom], layer[e.cod]) + 1, Math.max(layer[e.dom], layer[e.cod])).some((row) => row.some((v) => Math.abs(nodes[v].y - (a.y + b.y) / 2) < 30)))
        bow = 34 * (a.y + b.y <= (tallest - 1) * gapY ? -1 : 1) * flip;  // round an object that stands between
      if (members.length === 1 && span === 0) bow = 0;
      const c = { x: (a.x + b.x) / 2 + nx * bow * 2, y: (a.y + b.y) / 2 + ny * bow * 2 };
      const from = leave(a, c), to = leave(b, c, 7);
      const mid = quadratic(from, c, to, 0.5);
      const side = bow >= 0 ? 1 : -1;
      const angle = Math.atan2(to.y - c.y, to.x - c.x);
      out[k] = { ...out[k], d: `M${from.x.toFixed(1)} ${from.y.toFixed(1)} Q${c.x.toFixed(1)} ${c.y.toFixed(1)} ${to.x.toFixed(1)} ${to.y.toFixed(1)}`, from, to, control: c,
                 head: { x: to.x, y: to.y, angle }, label: { x: mid.x + nx * side * 11, y: mid.y + ny * side * 11 } };
    });
  }
  // Petals: arrows from an object to itself, spread on the side away from the diagram's middle.
  const middle = { x: nodes.reduce((s, n) => s + n.x, 0) / Math.max(1, count), y: nodes.reduce((s, n) => s + n.y, 0) / Math.max(1, count) };
  loops.forEach((members, v) => {
    if (!members.length) return;
    const n = nodes[v];
    const alone = count === 1;
    const away = alone ? -Math.PI / 2 : Math.atan2(n.y - middle.y || -1, n.x - middle.x);
    const spread = alone ? (2 * Math.PI) / members.length : Math.min(Math.PI / 3, (Math.PI * 1.1) / members.length);
    members.forEach((k, j) => {
      const theta = away + (j - (alone ? 0 : (members.length - 1) / 2)) * spread;
      const open = Math.min(0.42, spread * 0.36);
      const reach = 44;
      const p = (angle, r) => ({ x: n.x + Math.cos(angle) * r, y: n.y + Math.sin(angle) * r });
      const from = leave(n, p(theta - open, 100)), to = leave(n, p(theta + open, 100), 7);
      const c1 = p(theta - open * 1.5, n.rx + reach), c2 = p(theta + open * 1.5, n.rx + reach);
      const tip = p(theta, n.rx + reach * 0.82);
      out[k] = { ...out[k], d: `M${from.x.toFixed(1)} ${from.y.toFixed(1)} C${c1.x.toFixed(1)} ${c1.y.toFixed(1)} ${c2.x.toFixed(1)} ${c2.y.toFixed(1)} ${to.x.toFixed(1)} ${to.y.toFixed(1)}`, from, to,
                 head: { x: to.x, y: to.y, angle: Math.atan2(to.y - c2.y, to.x - c2.x) }, label: { x: tip.x + Math.cos(theta) * 10, y: tip.y + Math.sin(theta) * 10 }, towards: theta };
    });
  });
  // The frame: everything drawn, with a margin; then moved so that it begins at the margin.
  let minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity;
  const take = (px, py, w = 0, h = 0) => { minX = Math.min(minX, px - w); maxX = Math.max(maxX, px + w); minY = Math.min(minY, py - h); maxY = Math.max(maxY, py + h); };
  nodes.forEach((n) => take(n.x, n.y, n.rx, n.ry));
  out.forEach((e) => { if (!e.d) return; take(e.label.x, e.label.y, WIDTH_OF(e.name, size * 0.86) / 2 + 2, 9); if (e.control) take((e.from.x + 2 * e.control.x + e.to.x) / 4, (e.from.y + 2 * e.control.y + e.to.y) / 4); });
  if (!count) { minX = minY = 0; maxX = maxY = 0; }
  const margin = 16, shiftX = margin - minX, shiftY = margin - minY;
  nodes.forEach((n) => { n.x += shiftX; n.y += shiftY; });
  const moved = (d) => d.replace(/(-?\d+\.?\d*) (-?\d+\.?\d*)/g, (_, a, b) => `${(Number(a) + shiftX).toFixed(1)} ${(Number(b) + shiftY).toFixed(1)}`);
  out.forEach((e) => {
    if (!e.d) return;
    e.d = moved(e.d);
    for (const p of [e.from, e.to, e.head, e.label, e.control]) if (p) { p.x += shiftX; p.y += shiftY; }
  });
  void petal;
  return { width: Math.max(60, maxX - minX + 2 * margin), height: Math.max(40, maxY - minY + 2 * margin), nodes, edges: out, layer, rows };
}

export const textWidth = WIDTH_OF;
