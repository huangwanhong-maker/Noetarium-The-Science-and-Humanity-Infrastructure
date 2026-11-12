// String diagrams: the wires and boxes of a term of a monoidal theory, laid out and drawn.
//
// A diagram is kategoria's (kategoria/monoidal): wires, each with a type; boxes, each with wires in and out and a depth
// (one more than the deepest box that feeds it); and the diagram's own wires in and out, in order. It is read from left
// to right. The boxes of one depth stand in one column. Down the columns runs an ordered list of the wires that are
// "live" there: a box takes its wires in out of the list and puts its wires out in their place, and a wire that only
// passes keeps its place. A box with no wire in (a unit, a state) is placed beside the wire its output will meet. The
// crossings that remain are those the term has: a symmetry is two wires that cross.
//
// Pure arithmetic and markup: no page is needed.

import { textWidth } from './layout.js';

const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');

// The layout: { width, height, boxes: [{ name, x, y, w, h }], wires: [{ type, points: [{x, y}], d }], inputs, outputs }.
export function layoutStrings(diagram, options = {}) {
  const slot = options.slot || 26, gap = options.gap || 74, size = options.size || 13;
  const columns = [];
  diagram.boxes.forEach((b, i) => { (columns[b.depth] = columns[b.depth] || []).push(i); });
  for (let d = 0; d < columns.length; d++) columns[d] = columns[d] || [];
  const consumer = new Map();  // wire → { box, port }
  diagram.boxes.forEach((b, i) => b.in.forEach((w, p) => consumer.set(w, { box: i, port: p })));
  const outputAt = new Map(diagram.outputs.map((w, k) => [w, k]));
  const widths = columns.map((col) => Math.max(26, ...col.map((i) => textWidth(diagram.boxes[i].name, size) + 18)));
  const xs = [];
  let x = 34;
  columns.forEach((col, d) => { x += gap / 2 + widths[d] / 2; xs.push(x); x += widths[d] / 2 + gap / 2; });
  const xIn = 12, xOut = Math.max(x + 22, 120);
  const points = new Map(), ended = [];
  const boundary = (count, k) => (k - (count - 1) / 2) * slot;
  let live = [...diagram.inputs];
  diagram.inputs.forEach((w, k) => points.set(w, [{ x: xIn, y: boundary(diagram.inputs.length, k) }]));
  const boxes = diagram.boxes.map((b) => ({ name: b.name, x: 0, y: 0, w: 0, h: 0 }));
  columns.forEach((col, d) => {
    const at = new Map(live.map((w, k) => [w, k]));
    const key = (i) => {
      const b = diagram.boxes[i];
      const ins = b.in.filter((w) => at.has(w)).map((w) => at.get(w));
      if (ins.length) return ins.reduce((s, v) => s + v, 0) / ins.length;
      // No wire in: beside the live wire that its output will meet at one box, on the side its port says; else by where it leaves.
      for (const w of b.out) {
        const c = consumer.get(w);
        if (c) {
          const mates = diagram.boxes[c.box].in.map((v, p) => ({ v, p })).filter(({ v }) => at.has(v));
          if (mates.length) {
            const nearest = mates.reduce((best, m) => (Math.abs(m.p - c.port) < Math.abs(best.p - c.port) ? m : best));
            return at.get(nearest.v) + (c.port < nearest.p ? -0.5 : 0.5);
          }
        } else if (outputAt.has(w)) {
          return (outputAt.get(w) / Math.max(1, diagram.outputs.length - 1 || 1)) * Math.max(0, live.length - 1) + (outputAt.get(w) === 0 ? -0.5 : 0.5);
        }
      }
      return live.length;
    };
    const taken = new Set(col.flatMap((i) => diagram.boxes[i].in));
    const items = [...live.filter((w) => !taken.has(w)).map((w) => ({ wire: w, key: at.get(w) })), ...col.map((i, order) => ({ box: i, key: key(i), order }))];
    items.sort((a, b) => a.key - b.key || (a.order ?? 0) - (b.order ?? 0));
    const height = items.reduce((s, it) => s + (it.box === undefined ? 1 : Math.max(1, diagram.boxes[it.box].in.length, diagram.boxes[it.box].out.length)), 0);
    let row = -height / 2;
    const next = [];
    for (const it of items) {
      if (it.box === undefined) {
        points.get(it.wire).push({ x: xs[d], y: (row + 0.5) * slot });
        next.push(it.wire);
        row += 1;
        continue;
      }
      const b = diagram.boxes[it.box], rows = Math.max(1, b.in.length, b.out.length);
      const cy = (row + rows / 2) * slot, w = textWidth(b.name, size) + 18, h = Math.max(22, rows * slot - 8);
      boxes[it.box] = { name: b.name, x: xs[d], y: cy, w, h };
      b.in.forEach((wire, p) => {
        points.get(wire).push({ x: xs[d] - w / 2, y: cy + boundary(b.in.length, p) * Math.min(1, (h - 8) / Math.max(1, (b.in.length - 1) * slot || 1)) });
        ended.push(wire);
      });
      b.out.forEach((wire, p) => {
        points.set(wire, [{ x: xs[d] + w / 2, y: cy + boundary(b.out.length, p) * Math.min(1, (h - 8) / Math.max(1, (b.out.length - 1) * slot || 1)) }]);
        next.push(wire);
      });
      row += rows;
    }
    live = next;
  });
  diagram.outputs.forEach((w, k) => points.get(w).push({ x: xOut, y: boundary(diagram.outputs.length, k) }));
  let minY = Infinity, maxY = -Infinity;
  for (const list of points.values()) for (const p of list) { minY = Math.min(minY, p.y); maxY = Math.max(maxY, p.y); }
  boxes.forEach((b) => { minY = Math.min(minY, b.y - b.h / 2); maxY = Math.max(maxY, b.y + b.h / 2); });
  if (!Number.isFinite(minY)) { minY = -slot / 2; maxY = slot / 2; }
  const shift = 16 - minY;
  boxes.forEach((b) => { b.y += shift; });
  const wires = diagram.wires.map((type, w) => {
    const list = (points.get(w) || []).map((p) => ({ x: p.x, y: p.y + shift }));
    let d = '';
    list.forEach((p, k) => {
      if (k === 0) { d += `M${p.x.toFixed(1)} ${p.y.toFixed(1)}`; return; }
      const q = list[k - 1], bend = (p.x - q.x) * 0.5;
      d += ` C${(q.x + bend).toFixed(1)} ${q.y.toFixed(1)} ${(p.x - bend).toFixed(1)} ${p.y.toFixed(1)} ${p.x.toFixed(1)} ${p.y.toFixed(1)}`;
    });
    return { type, points: list, d };
  });
  return { width: xOut + 12, height: maxY - minY + 32, boxes, wires, inputs: diagram.inputs, outputs: diagram.outputs };
}

// The diagram as SVG markup. Classes: k-wire, k-box, k-box-name, k-end, k-type.
export function svgStrings(diagram, options = {}) {
  const L = layoutStrings(diagram, options);
  const several = new Set(diagram.wires).size > 1;
  const parts = [`<svg xmlns="http://www.w3.org/2000/svg" class="k-strings" viewBox="0 0 ${L.width.toFixed(0)} ${L.height.toFixed(0)}" width="${L.width.toFixed(0)}" height="${L.height.toFixed(0)}" role="img" aria-label="${esc(diagram.words || 'a string diagram')}">`];
  L.wires.forEach((w) => { if (w.d) parts.push(`<path class="k-wire" d="${w.d}"/>`); });
  const end = (wire, first) => {
    const list = L.wires[wire].points, p = first ? list[0] : list[list.length - 1];
    parts.push(`<circle class="k-end" cx="${p.x.toFixed(1)}" cy="${p.y.toFixed(1)}" r="2.6"/>`);
    if (several) parts.push(`<text class="k-type" x="${(p.x + (first ? 6 : -6)).toFixed(1)}" y="${(p.y - 6).toFixed(1)}" text-anchor="${first ? 'start' : 'end'}">${esc(L.wires[wire].type)}</text>`);
  };
  L.inputs.forEach((w) => end(w, true));
  L.outputs.forEach((w) => end(w, false));
  L.boxes.forEach((b) => {
    parts.push(`<rect class="k-box" x="${(b.x - b.w / 2).toFixed(1)}" y="${(b.y - b.h / 2).toFixed(1)}" width="${b.w.toFixed(1)}" height="${b.h.toFixed(1)}" rx="3"/>`);
    parts.push(`<text class="k-box-name" x="${b.x.toFixed(1)}" y="${(b.y + 4.5).toFixed(1)}" text-anchor="middle">${esc(b.name)}</text>`);
  });
  parts.push('</svg>');
  return parts.join('');
}

// Two diagrams side by side with a sign between: an equation, or its failure.
export function htmlEquation(left, right, sign = '=') {
  if (!left) return '';
  if (!right) return `<div class="k-equation">${svgStrings(left)}</div>`;
  return `<div class="k-equation">${svgStrings(left)}<span class="k-sign">${esc(sign)}</span>${svgStrings(right)}</div>`;
}
