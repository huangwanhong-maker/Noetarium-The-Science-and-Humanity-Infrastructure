// The geometry of a board: rectangles, the paths of links between them, and the lines a moved item
// snaps to. Pure; lengths are in the board's own units (a pixel at zoom 1).

export const centre = (r) => ({ x: r.x + r.w / 2, y: r.y + r.h / 2 });
export const contains = (r, p) => p.x >= r.x && p.x <= r.x + r.w && p.y >= r.y && p.y <= r.y + r.h;
export const intersects = (a, b) => a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h;
export const normalised = (a, b) => ({ x: Math.min(a.x, b.x), y: Math.min(a.y, b.y), w: Math.abs(a.x - b.x), h: Math.abs(a.y - b.y) });

// The middle of a side of a rectangle, and the direction out of it.
const SIDES = {
  e: (r) => ({ x: r.x + r.w, y: r.y + r.h / 2, nx: 1, ny: 0 }),
  w: (r) => ({ x: r.x, y: r.y + r.h / 2, nx: -1, ny: 0 }),
  s: (r) => ({ x: r.x + r.w / 2, y: r.y + r.h, nx: 0, ny: 1 }),
  n: (r) => ({ x: r.x + r.w / 2, y: r.y, nx: 0, ny: -1 }),
};
export const side = (r, which) => SIDES[which](r);

// The sides two rectangles are joined by: across the wider of the gaps between them, so that a link
// leaves an item toward the other.
export function facing(a, b) {
  const ca = centre(a), cb = centre(b);
  const gapX = Math.max(b.x - (a.x + a.w), a.x - (b.x + b.w)), gapY = Math.max(b.y - (a.y + a.h), a.y - (b.y + b.h));
  const horizontal = gapX > 0 || gapY > 0 ? gapX >= gapY : Math.abs(cb.x - ca.x) >= Math.abs(cb.y - ca.y);
  if (horizontal) return cb.x >= ca.x ? ['e', 'w'] : ['w', 'e'];
  return cb.y >= ca.y ? ['s', 'n'] : ['n', 's'];
}

const cubicAt = (p0, p1, p2, p3, t) => {
  const u = 1 - t;
  return u * u * u * p0 + 3 * u * u * t * p1 + 3 * u * t * t * p2 + t * t * t * p3;
};

// The path of a link from one rectangle to another (or to a point, a rectangle of no size): a cubic
// leaving and arriving square to the sides. `head` is the arrowhead's triangle at the far end, `mid`
// where a label goes.
export function linkPath(a, b, { head = 9, gap = 3 } = {}) {
  const point = b.w === 0 && b.h === 0;
  const [sa, sb] = facing(a, point ? { x: b.x, y: b.y, w: 0, h: 0 } : b);
  const p0 = side(a, sa), p3 = point ? { x: b.x, y: b.y, nx: -p0.nx, ny: -p0.ny } : side(b, sb);
  const start = { x: p0.x + p0.nx * gap, y: p0.y + p0.ny * gap };
  const end = { x: p3.x + p3.nx * gap, y: p3.y + p3.ny * gap };
  const reach = Math.max(24, 0.42 * Math.hypot(end.x - start.x, end.y - start.y));
  const c1 = { x: start.x + p0.nx * reach, y: start.y + p0.ny * reach };
  const c2 = { x: end.x + p3.nx * reach, y: end.y + p3.ny * reach };
  // The line stops at the base of the arrowhead.
  const base = { x: end.x + p3.nx * head * 0.9, y: end.y + p3.ny * head * 0.9 };
  const d = `M${r(start.x)} ${r(start.y)}C${r(c1.x)} ${r(c1.y)} ${r(c2.x)} ${r(c2.y)} ${r(base.x)} ${r(base.y)}`;
  const tx = -p3.ny, ty = p3.nx, half = head * 0.42;
  const tip = end, back = { x: end.x + p3.nx * head, y: end.y + p3.ny * head };
  const arrow = `M${r(tip.x)} ${r(tip.y)}L${r(back.x + tx * half)} ${r(back.y + ty * half)}L${r(back.x - tx * half)} ${r(back.y - ty * half)}Z`;
  const mid = { x: cubicAt(start.x, c1.x, c2.x, end.x, 0.5), y: cubicAt(start.y, c1.y, c2.y, end.y, 0.5) };
  return { d, arrow, start, end, mid, sides: [sa, sb] };
}
const r = (v) => Math.round(v * 10) / 10;

// The shift that brings a moving rectangle's edges or middle onto those of the others, when within
// the threshold, and the guide lines to draw: {dx, dy, guides: [{x} | {y}, with the extent to draw]}.
export function snap(moving, others, threshold = 6) {
  const xs = [moving.x, moving.x + moving.w / 2, moving.x + moving.w];
  const ys = [moving.y, moving.y + moving.h / 2, moving.y + moving.h];
  let bestX = null, bestY = null;
  for (const o of others) {
    const ox = [o.x, o.x + o.w / 2, o.x + o.w], oy = [o.y, o.y + o.h / 2, o.y + o.h];
    for (let i = 0; i < 3; i++) for (let j = 0; j < 3; j++) {
      // Edges snap to edges and middles to middles.
      if ((i === 1) !== (j === 1)) continue;
      const dx = ox[j] - xs[i], dy = oy[j] - ys[i];
      if (Math.abs(dx) <= threshold && (!bestX || Math.abs(dx) < Math.abs(bestX.d))) bestX = { d: dx, at: ox[j], o };
      if (Math.abs(dy) <= threshold && (!bestY || Math.abs(dy) < Math.abs(bestY.d))) bestY = { d: dy, at: oy[j], o };
    }
  }
  const dx = bestX ? bestX.d : 0, dy = bestY ? bestY.d : 0;
  const moved = { x: moving.x + dx, y: moving.y + dy, w: moving.w, h: moving.h };
  const guides = [];
  if (bestX) guides.push({ x: bestX.at, from: Math.min(moved.y, bestX.o.y), to: Math.max(moved.y + moved.h, bestX.o.y + bestX.o.h) });
  if (bestY) guides.push({ y: bestY.at, from: Math.min(moved.x, bestY.o.x), to: Math.max(moved.x + moved.w, bestY.o.x + bestY.o.w) });
  return { dx, dy, guides };
}

// ---- the pen ---------------------------------------------------------------------------------------

// A polyline with fewer points, none of those left out further than `tolerance` from it (Ramer,
// Douglas and Peucker). Points are [x, y, …]; what follows x and y (a pressure) is kept.
export function simplify(points, tolerance = 0.5) {
  if (points.length < 3) return points.slice();
  const keep = new Uint8Array(points.length);
  keep[0] = keep[points.length - 1] = 1;
  const stack = [[0, points.length - 1]];
  while (stack.length) {
    const [a, b] = stack.pop();
    const [ax, ay] = points[a], [bx, by] = points[b];
    const dx = bx - ax, dy = by - ay, len = Math.hypot(dx, dy);
    let far = -1, most = tolerance;
    for (let i = a + 1; i < b; i++) {
      const [px, py] = points[i];
      const d = len ? Math.abs((px - ax) * dy - (py - ay) * dx) / len : Math.hypot(px - ax, py - ay);
      if (d > most) { most = d; far = i; }
    }
    if (far > 0) { keep[far] = 1; stack.push([a, far], [far, b]); }
  }
  return points.filter((_, i) => keep[i]);
}

// The outline of a stroke of the pen as a closed path: the line through the points, as wide at each
// as the pen pressed there (width × (0.35 + 1.3 pressure); a pressure of ½ is the width itself),
// rounded at both ends and smoothed through the middles of its sides. A single point is a dot.
export function strokeOutline(points, width = 3) {
  const half = (p) => (width / 2) * (0.35 + 1.3 * (p[2] ?? 0.5));
  if (!points.length) return '';
  if (points.length === 1 || points.every((p) => p[0] === points[0][0] && p[1] === points[0][1])) {
    const [x, y] = points[0], rad = half(points[0]);
    return `M${r(x - rad)} ${r(y)}a${r(rad)} ${r(rad)} 0 1 0 ${r(2 * rad)} 0a${r(rad)} ${r(rad)} 0 1 0 ${r(-2 * rad)} 0Z`;
  }
  const left = [], right = [];
  for (let i = 0; i < points.length; i++) {
    const a = points[Math.max(0, i - 1)], b = points[Math.min(points.length - 1, i + 1)];
    let dx = b[0] - a[0], dy = b[1] - a[1];
    const len = Math.hypot(dx, dy) || 1;
    dx /= len; dy /= len;
    const h = half(points[i]);
    left.push([points[i][0] - dy * h, points[i][1] + dx * h]);
    right.push([points[i][0] + dy * h, points[i][1] - dx * h]);
  }
  // A side as a curve through the middles of its segments, its corners the control points.
  const side = (pts) => {
    let d = '';
    for (let i = 1; i < pts.length - 1; i++) d += `Q${r(pts[i][0])} ${r(pts[i][1])} ${r((pts[i][0] + pts[i + 1][0]) / 2)} ${r((pts[i][1] + pts[i + 1][1]) / 2)}`;
    const end = pts[pts.length - 1];
    return `${d}L${r(end[0])} ${r(end[1])}`;
  };
  const n = points.length - 1, back = right.slice().reverse();
  const cap = (rad, to) => `A${r(rad)} ${r(rad)} 0 0 1 ${r(to[0])} ${r(to[1])}`;
  return `M${r(left[0][0])} ${r(left[0][1])}${side(left)}${cap(half(points[n]), right[n])}${side(back)}${cap(half(points[0]), left[0])}Z`;
}

// A view of the board: a point (wx, wy) of the board is drawn at (x + k wx, y + k wy) of the screen.
export const toScreen = (view, p) => ({ x: view.x + view.k * p.x, y: view.y + view.k * p.y });
export const toBoard = (view, p) => ({ x: (p.x - view.x) / view.k, y: (p.y - view.y) / view.k });

// The view zoomed by a factor about a point of the screen, which stays where it is.
export function zoomAbout(view, factor, at, { min = 0.08, max = 4 } = {}) {
  const k = Math.max(min, Math.min(max, view.k * factor));
  const f = k / view.k;
  return { k, x: at.x - (at.x - view.x) * f, y: at.y - (at.y - view.y) * f };
}

// The view that shows a rectangle of the board whole in a screen of a size, with a margin, clear of
// what stands over the screen's edges (inset: {left, right, top, bottom}, the widths covered).
export function fitView(rect, screen, { margin = 56, max = 1.25, min = 0.08, inset = {} } = {}) {
  const left = inset.left || 0, top = inset.top || 0;
  const w = Math.max(80, screen.w - left - (inset.right || 0)), h = Math.max(80, screen.h - top - (inset.bottom || 0));
  if (!rect || rect.w <= 0 || rect.h <= 0) return { k: 1, x: left + w / 2 - (rect ? rect.x : 0), y: top + h / 2 - (rect ? rect.y : 0) };
  const k = Math.max(min, Math.min(max, (w - 2 * margin) / rect.w, (h - 2 * margin) / rect.h));
  return { k, x: left + (w - rect.w * k) / 2 - rect.x * k, y: top + (h - rect.h * k) / 2 - rect.y * k };
}
