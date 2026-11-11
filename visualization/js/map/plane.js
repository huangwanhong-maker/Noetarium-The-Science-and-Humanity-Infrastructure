// A map of a town on its own plane: metres east and north of an origin (a city model's frame), drawn in an SVG whose world is
// moved and scaled as a whole. The map of the sphere (widget.js) draws countries and regions; a district of a few kilometres is a
// few pixels there. This is its counterpart at the scale of streets: the same manner of use (a world to draw into, a size of one
// pixel, fitting, picking), without a projection, since the frame is already a plane.
//
//   const plane = createPlane(svg, {onPick, onSettle, onMove});
//   plane.layer('streets')            a <g> of the world to draw into, in the order the layers are first asked for
//   plane.path(points)                'M x,y L …' of points [[x, y], …] in the frame (north up: y is turned for the screen)
//   plane.at(x, y)                    the frame's point as the world's coordinates [x, −y]
//   plane.px()                        the frame's metres in one pixel, for marks that keep their size on the screen
//   plane.fit(box, margin?)           show the box {x0, y0, x1, y1} of the frame
//   plane.place(event)                the frame's point under a pointer
//
// Pure of what is drawn: a work draws its layers and draws them again on `onSettle` if they depend on the zoom.

const NS = 'http://www.w3.org/2000/svg';

// The view that shows a box of the frame in a screen of a size: the scale (pixels a metre) and the offset of the frame's origin.
export function fitBox(box, width, height, margin = 24, inset = {}) {
  const left = margin + (inset.left || 0), right = margin + (inset.right || 0), top = margin + (inset.top || 0), bottom = margin + (inset.bottom || 0);
  const w = Math.max(1e-9, box.x1 - box.x0), h = Math.max(1e-9, box.y1 - box.y0);
  const k = Math.max(1e-9, Math.min((width - left - right) / w, (height - top - bottom) / h));
  return { k, x: left + (width - left - right) / 2 - k * (box.x0 + box.x1) / 2, y: top + (height - top - bottom) / 2 + k * (box.y0 + box.y1) / 2 };
}

// A view zoomed by a factor about a point of the screen, within bounds.
export function zoomView(view, factor, at, { min = 0.02, max = 40 } = {}) {
  const k = Math.min(max, Math.max(min, view.k * factor));
  return { k, x: at.x - (at.x - view.x) * (k / view.k), y: at.y - (at.y - view.y) * (k / view.k) };
}

export function createPlane(svg, { onPick = null, onSettle = null, onMove = null, inset = () => ({}) } = {}) {
  const world = document.createElementNS(NS, 'g');
  svg.append(world);
  const layers = new Map();
  let view = { k: 1, x: 0, y: 0 }, home = null, settle = null;
  const size = () => { const r = svg.getBoundingClientRect(); return { w: r.width || 800, h: r.height || 600, left: r.left, top: r.top }; };
  const apply = () => {
    world.setAttribute('transform', `translate(${view.x.toFixed(2)},${view.y.toFixed(2)}) scale(${view.k})`);
    svg.style.setProperty('--plane-px', String(1 / view.k));
    clearTimeout(settle);
    settle = setTimeout(() => onSettle?.(view), 90);
  };
  const local = (e) => { const s = size(); return { x: e.clientX - s.left, y: e.clientY - s.top }; };
  const place = (e) => { const p = local(e); return [(p.x - view.x) / view.k, -(p.y - view.y) / view.k]; };

  svg.addEventListener('wheel', (e) => {
    e.preventDefault();
    view = zoomView(view, Math.exp(-e.deltaY * 0.0016), local(e));
    apply();
  }, { passive: false });
  let drag = null;
  svg.addEventListener('pointerdown', (e) => {
    if (e.button !== 0) return;
    drag = { at: local(e), view, moved: false, target: e.target };
    svg.setPointerCapture(e.pointerId);
  });
  svg.addEventListener('pointermove', (e) => {
    if (!drag) { onMove?.(e, place(e)); return; }
    const p = local(e);
    if (Math.hypot(p.x - drag.at.x, p.y - drag.at.y) > 3) drag.moved = true;
    if (drag.moved) { view = { k: view.k, x: drag.view.x + p.x - drag.at.x, y: drag.view.y + p.y - drag.at.y }; apply(); }
  });
  svg.addEventListener('pointerup', (e) => {
    const was = drag;
    drag = null;
    if (was && !was.moved) onPick?.(e, place(e), was.target);
  });
  svg.addEventListener('pointercancel', () => { drag = null; });
  svg.addEventListener('dblclick', () => { if (home) { view = { ...home }; apply(); } });

  return {
    world,
    get view() { return view; },
    layer(name) {
      if (!layers.has(name)) { const g = document.createElementNS(NS, 'g'); g.setAttribute('class', `plane-${name}`); world.append(g); layers.set(name, g); }
      return layers.get(name);
    },
    at: (x, y) => [x, -y],
    path: (points, closed = false) => `${points.map(([x, y], i) => `${i ? 'L' : 'M'}${x.toFixed(1)},${(-y).toFixed(1)}`).join('')}${closed ? 'Z' : ''}`,
    px: () => 1 / view.k,
    place,
    fit(box, margin = 24) { const s = size(); view = fitBox(box, s.w, s.h, margin, inset()); home = { ...view }; apply(); },
    zoomBy(factor) { const s = size(); view = zoomView(view, factor, { x: s.w / 2, y: s.h / 2 }); apply(); },
    centreOn(x, y, k = view.k) { const s = size(), i = inset(); view = { k, x: (i.left || 0) + (s.w - (i.left || 0) - (i.right || 0)) / 2 - k * x, y: s.h / 2 + k * y }; apply(); },
  };
}
