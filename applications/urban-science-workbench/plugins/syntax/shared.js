// What the plug-in "Geometric syntax" shares between its layers and its tool: what is near a place (ways, footprints), colours by
// theme, small drawings of patterns, trees and graphs in SVG, and files to download.

const SVGNS = 'http://www.w3.org/2000/svg';
export const svg = (tag, attrs = {}, ...kids) => {
  const n = document.createElementNS(SVGNS, tag);
  for (const [k, v] of Object.entries(attrs)) if (v !== null && v !== undefined) n.setAttribute(k, v);
  for (const c of kids.flat()) if (c != null) n.append(c instanceof Node ? c : document.createTextNode(String(c)));
  return n;
};

// The ways near a point of the local frame, as [x0, y0, x1, y1, …] (m), each cut to the points within the radius.
export function waysNear(world, at, radius) {
  const out = [];
  for (const l of world.lines()) {
    if (l.rail) continue;
    let run = [];
    for (let i = 0; i < l.xyz.length; i += 3) {
      const x = l.xyz[i], y = l.xyz[i + 1];
      if (Math.hypot(x - at[0], y - at[1]) <= radius) run.push(Math.round(x * 10) / 10, Math.round(y * 10) / 10);
      else { if (run.length >= 4) out.push(run); run = []; }
    }
    if (run.length >= 4) out.push(run);
  }
  return out;
}

// What `near` finds once it finds anything, waiting for the world's tiles while they arrive (every half second, for up to ten
// seconds); null if nothing came, or if `alive` turned false meanwhile.
export async function whenNear(near, alive = () => true) {
  for (let k = 0; k < 20; k++) {
    const found = near();
    if (found.length) return found;
    if (!alive()) return null;
    await new Promise((r) => setTimeout(r, 500));
  }
  return null;
}

// The footprints of the buildings drawn near a point, as [x0, y0, x1, y1, …] rings (m): a city model's buildings (features with a
// footprint of [x, y] points) and the world's tiles (built solids, rings packed by their starts), which stand beside the model.
export function footprintsNear(gis, at, radius) {
  const out = [];
  for (const [, layer] of gis.layers) {
    if (layer.kind !== 'solids' || !layer.features?.length || !Array.isArray(layer.features[0].footprint)) continue;
    for (const f of layer.features) {
      const r = f.footprint;
      if (!r || r.length < 3) continue;
      let cx = 0, cy = 0;
      for (const p of r) cx += p[0], cy += p[1];
      if (Math.hypot(cx / r.length - at[0], cy / r.length - at[1]) > radius) continue;
      out.push(r.flatMap((p) => [Math.round(p[0] * 100) / 100, Math.round(p[1] * 100) / 100]));
    }
  }
  for (const [, layer] of gis.layers) {
    if (layer.kind !== 'built-solids' || !layer.built?.starts) continue;
    const { rings, starts } = layer.built;
    for (let k = 0; k < starts.length / 2; k++) {
      const s = starts[2 * k], n = starts[2 * k + 1];
      if (n < 3) continue;
      let cx = 0, cy = 0;
      for (let i = 0; i < n; i++) cx += rings[2 * (s + i)], cy += rings[2 * (s + i) + 1];
      cx /= n, cy /= n;
      if (Math.hypot(cx - at[0], cy - at[1]) > radius) continue;
      const ring = [];
      for (let i = 0; i < n; i++) ring.push(Math.round(rings[2 * (s + i)] * 100) / 100, Math.round(rings[2 * (s + i) + 1] * 100) / 100);
      out.push(ring);
    }
  }
  return out;
}

// Colours in the workbench's palette: the steps of a derivation from orchid to rose to gold, apart by night and by day.
export function palette(theme) {
  const night = theme === 'night';
  const stops = night ? [[0.62, 0.48, 0.86], [0.93, 0.56, 0.74], [0.95, 0.78, 0.48]] : [[0.43, 0.29, 0.64], [0.76, 0.28, 0.5], [0.66, 0.48, 0.16]];
  const ramp = (u) => {
    const x = Math.min(1, Math.max(0, u)) * (stops.length - 1), i = Math.min(stops.length - 2, Math.floor(x)), f = x - i;
    return stops[i].map((c, k) => c + (stops[i + 1][k] - c) * f);
  };
  return {
    ramp,
    plain: night ? [0.55, 0.5, 0.6] : [0.62, 0.58, 0.66],
    faint: night ? [0.3, 0.26, 0.36] : [0.82, 0.78, 0.84],
    accent: night ? [1, 0.75, 0.86] : [0.76, 0.28, 0.5],
    sample: night ? [0.98, 0.84, 0.6] : [0.62, 0.42, 0.12],
    css: (rgb) => `rgb(${rgb.map((c) => Math.round(c * 255)).join(',')})`,
  };
}

// A categorical colour for a letter, by its index (hues kept within the palette's warmth, apart from one another).
export function letterColour(k, theme) {
  const hues = [330, 280, 40, 200, 15, 160, 300, 60, 250, 100, 350, 220];
  const h = hues[k % hues.length], l = theme === 'night' ? 0.68 : 0.55, s = 0.5;
  const a = s * Math.min(l, 1 - l), f = (n) => { const q = (n + h / 30) % 12; return l - a * Math.max(-1, Math.min(q - 3, 9 - q, 1)); };
  return [f(0), f(8), f(4)];
}

// A rule's pattern, drawn small: its nodes in its canonical frame, terminals as dots and nonterminals as rings.
export function patternSvg(rule, theme, size = 54) {
  const P = palette(theme);
  const pts = rule.nodes.map((n) => n.at);
  const xs = pts.map((p) => p[0]), ys = pts.map((p) => -p[1]);
  const minX = Math.min(...xs), maxX = Math.max(...xs), minY = Math.min(...ys), maxY = Math.max(...ys);
  const span = Math.max(maxX - minX, maxY - minY) || 1, pad = 0.18 * span;
  const root = svg('svg', { viewBox: `${minX - pad} ${minY - pad} ${span + 2 * pad} ${span + 2 * pad}`, width: size, height: size, class: 'gs-pattern' });
  const sw = span / 30;
  for (const [a, b] of rule.edges) root.append(svg('line', { x1: xs[a], y1: ys[a], x2: xs[b], y2: ys[b], stroke: P.css(P.plain), 'stroke-width': sw }));
  rule.nodes.forEach((n, k) => root.append(svg('circle', { cx: xs[k], cy: ys[k], r: n.symbol ? sw * 2.6 : sw * 1.8, fill: n.symbol ? 'none' : P.css(P.accent), stroke: n.symbol ? P.css(P.ramp(0.5)) : 'none', 'stroke-width': sw })));
  return root;
}

// A derivation tree as rows by level, its roots at the top: each instance a dot coloured by its rule, joined to its children. A
// forest too wide to draw leaf by leaf is drawn by its nonterminals alone, the input's nodes folded beneath the rules' instances.
export function treeSvg(tree, theme, width = 560, height = 220, maxLeaves = 700) {
  const P = palette(theme);
  const I = tree.instances;
  const leaves = I.filter((n) => !n.children.length).length;
  const folded = leaves > maxLeaves;
  const kids = (i) => (folded ? I[i].children.filter((c) => I[c].terminal == null) : I[i].children);
  const roots = folded ? tree.roots.filter((r) => I[r].terminal == null) : tree.roots;
  const bare = tree.roots.length - roots.length;
  // Depth of each node from the roots; x by the order of leaves under it.
  const depth = new Array(I.length).fill(0), x = new Array(I.length).fill(0);
  let maxDepth = 0, next = 0;
  const visit = (i, d) => {
    depth[i] = d, maxDepth = Math.max(maxDepth, d);
    const ks = kids(i);
    if (!ks.length) { x[i] = next++; return; }
    for (const c of ks) visit(c, d + 1);
    x[i] = ks.reduce((s, c) => s + x[c], 0) / ks.length;
  };
  for (const r of roots) visit(r, 0);
  if (!roots.length || next > maxLeaves) {
    const note = svg('svg', { viewBox: `0 0 ${width} 22`, width: '100%', class: 'gs-tree' });
    note.append(svg('text', { x: 4, y: 15, class: 'gs-note' }, `a forest of ${leaves.toLocaleString('en')} leaves, too wide to draw: saved whole`));
    return note;
  }
  const foot = folded ? 16 : 0;
  const root = svg('svg', { viewBox: `0 0 ${width} ${height + foot}`, width: '100%', class: 'gs-tree' });
  const sx = (v) => 10 + (v / Math.max(1, next - 1)) * (width - 20), sy = (d) => 14 + (d / Math.max(1, maxDepth)) * (height - 28);
  const levels = Math.max(1, ...I.map((n) => n.level || 0));
  const reached = new Set();
  const draw = (i) => {
    reached.add(i);
    for (const c of kids(i)) {
      root.append(svg('line', { x1: sx(x[i]), y1: sy(depth[i]), x2: sx(x[c]), y2: sy(depth[c]), stroke: P.css(P.faint), 'stroke-width': 0.7 }));
      draw(c);
    }
  };
  for (const r of roots) draw(r);
  for (const i of reached) {
    const n = I[i];
    const terminal = n.terminal != null;
    root.append(svg('circle', { cx: sx(x[i]), cy: sy(depth[i]), r: terminal ? 1.4 : folded ? 2.2 : 3, fill: terminal ? P.css(P.plain) : P.css(P.ramp((n.level || 0) / levels)) }));
  }
  if (folded)
    root.append(svg('text', { x: 4, y: height + foot - 3, class: 'gs-note' },
      `the rules' ${reached.size.toLocaleString('en')} instances, the input's ${leaves.toLocaleString('en')} nodes folded beneath them${bare ? ` (${bare.toLocaleString('en')} never reduced)` : ''}`));
  return root;
}

export function download(name, data) {
  const blob = new Blob([JSON.stringify(data, null, 1)], { type: 'application/json' });
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = name;
  document.body.append(a);
  a.click();
  setTimeout(() => { URL.revokeObjectURL(a.href); a.remove(); }, 1000);
}

// The renderer reads flats' and ribbons' colours as linear light (its points convert from sRGB themselves): the palette's sRGB
// colours, as the legend shows them, taken to linear (IEC 61966-2-1), so that the map and the legend agree.
export const linear = (rgb) => rgb.map((c) => (c <= 0.04045 ? c / 12.92 : ((c + 0.055) / 1.055) ** 2.4));

// Each step's colour: by its place among the steps.
export const stepColour = (P, k, n) => P.ramp(n > 1 ? k / (n - 1) : 0.5);
