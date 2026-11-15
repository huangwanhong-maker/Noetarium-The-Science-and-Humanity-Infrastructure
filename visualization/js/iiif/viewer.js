// A deep-zoom viewer of a IIIF image service (ADR 0120): the image drawn on a canvas from the tiles of the level the view needs, the
// coarsest level beneath it while they come; panned by dragging, zoomed by the wheel about the pointer, by keys and by double-click;
// regions laid over it in the image's own pixels, and a region drawn by hand when asked. Runs in a browser; its arithmetic is
// pyramid.js's, checked in Node.
//
//   const v = deepZoom(container, info, { marks, onRegion, drawing, label, address })   address(url): where a tile is fetched (a key added)
//   v.setMarks([{x, y, w, h, title, cls}]), v.draw(true|false), v.home(), v.zoom(by), v.destroy()

import { levels, factorFor, tiles, View } from './pyramid.js';

export function deepZoom(container, info, { marks = [], onRegion = null, drawing = false, label = 'An image', address = (u) => u } = {}) {
  const canvas = document.createElement('canvas');
  canvas.className = 'iiif-canvas';
  canvas.setAttribute('role', 'img');
  canvas.setAttribute('aria-label', label);
  canvas.tabIndex = 0;
  const keys = document.createElement('div');
  keys.className = 'iiif-keys';
  for (const [word, act] of [['+', () => zoomBy(1.4)], ['−', () => zoomBy(1 / 1.4)], ['⌂', () => home()]]) {
    const b = document.createElement('button');
    b.type = 'button';
    b.className = 'key';
    b.textContent = word;
    b.title = { '+': 'Closer', '−': 'Farther', '⌂': 'The whole image' }[word];
    b.addEventListener('click', act);
    keys.append(b);
  }
  container.append(canvas, keys);
  const cache = new Map();   // url → Image, or a promise of one
  const coarsest = levels(info).at(-1);
  let view = null, dirty = true, frame = 0, drag = null, live = true, current = marks, draw = drawing, drawn = null;
  const ratio = () => window.devicePixelRatio || 1;

  function resize() {
    const r = container.getBoundingClientRect();
    const w = Math.max(10, r.width), h = Math.max(10, r.height);
    canvas.width = Math.round(w * ratio());
    canvas.height = Math.round(h * ratio());
    canvas.style.width = `${w}px`;
    canvas.style.height = `${h}px`;
    if (!view) view = new View(w, h, info);
    else Object.assign(view, { width: w, height: h });
    dirty = true;
  }

  function image(u) {
    const got = cache.get(u);
    if (got instanceof HTMLImageElement) return got.complete && got.naturalWidth ? got : null;
    if (!got) {
      const img = new Image();
      img.decoding = 'async';
      img.onload = () => { dirty = true; };
      img.onerror = () => { cache.set(u, 'failed'); };
      img.src = address(u);
      cache.set(u, img);
    }
    return null;
  }

  function paint() {
    frame = requestAnimationFrame(paint);
    if (!dirty || !view || !live) return;
    dirty = false;
    const g = canvas.getContext('2d');
    g.setTransform(ratio(), 0, 0, ratio(), 0, 0);
    g.clearRect(0, 0, view.width, view.height);
    const seen = view.visible();
    if (seen.w <= 0 || seen.h <= 0) return;
    const want = factorFor(info, view.scale);
    let waiting = false;
    for (const f of [coarsest.factor, want].filter((x, i, a) => a.indexOf(x) === i)) {
      for (const t of tiles(info, f, f === coarsest.factor ? { x: 0, y: 0, w: info.width, h: info.height } : seen)) {
        const img = image(t.url);
        if (!img) { waiting = true; continue; }
        const [sx, sy] = view.toScreen(t.region[0], t.region[1]);
        g.drawImage(img, sx, sy, t.region[2] * view.scale + 0.5, t.region[3] * view.scale + 0.5);
      }
    }
    g.lineWidth = 1.5;
    for (const m of current) {
      const [x0, y0] = view.toScreen(m.x, m.y);
      g.strokeStyle = m.cls === 'chosen' ? '#7D5A9E' : '#B9607E';
      g.fillStyle = m.cls === 'chosen' ? 'rgba(125,90,158,0.14)' : 'rgba(185,96,126,0.12)';
      g.fillRect(x0, y0, m.w * view.scale, m.h * view.scale);
      g.strokeRect(x0, y0, m.w * view.scale, m.h * view.scale);
    }
    if (drawn) {
      const [x0, y0] = view.toScreen(drawn.x, drawn.y);
      g.setLineDash([5, 4]);
      g.strokeStyle = '#7D5A9E';
      g.strokeRect(x0, y0, drawn.w * view.scale, drawn.h * view.scale);
      g.setLineDash([]);
    }
    if (waiting) dirty = true;
  }

  const point = (e) => { const r = canvas.getBoundingClientRect(); return [e.clientX - r.left, e.clientY - r.top]; };
  function zoomBy(by, at = null) { view.zoom(by, ...(at || [view.width / 2, view.height / 2])); dirty = true; }
  function home() { view.fit(); dirty = true; }
  canvas.addEventListener('wheel', (e) => { e.preventDefault(); zoomBy(e.deltaY < 0 ? 1.18 : 1 / 1.18, point(e)); }, { passive: false });
  canvas.addEventListener('dblclick', (e) => zoomBy(e.shiftKey ? 1 / 2 : 2, point(e)));
  canvas.addEventListener('keydown', (e) => {
    if (e.key === '+' || e.key === '=') zoomBy(1.4);
    else if (e.key === '-') zoomBy(1 / 1.4);
    else if (e.key === '0') home();
    else if (e.key.startsWith('Arrow')) { view.pan(...({ ArrowLeft: [60, 0], ArrowRight: [-60, 0], ArrowUp: [0, 60], ArrowDown: [0, -60] })[e.key]); dirty = true; }
    else return;
    e.preventDefault();
  });
  canvas.addEventListener('pointerdown', (e) => {
    canvas.setPointerCapture(e.pointerId);
    const [sx, sy] = point(e);
    drag = draw ? { from: view.toImage(sx, sy) } : { sx, sy };
  });
  canvas.addEventListener('pointermove', (e) => {
    if (!drag) return;
    const [sx, sy] = point(e);
    if (!draw) { view.pan(sx - drag.sx, sy - drag.sy); drag.sx = sx; drag.sy = sy; dirty = true; return; }
    const [x, y] = view.toImage(sx, sy), [x0, y0] = drag.from;
    drawn = { x: Math.max(0, Math.min(x, x0)), y: Math.max(0, Math.min(y, y0)), w: Math.abs(x - x0), h: Math.abs(y - y0) };
    dirty = true;
  });
  canvas.addEventListener('pointerup', () => {
    const was = drawn;
    drag = null;
    drawn = null;
    dirty = true;
    if (draw && was && was.w >= 4 && was.h >= 4 && onRegion) onRegion({ x: Math.round(was.x), y: Math.round(was.y), w: Math.round(Math.min(was.w, info.width - was.x)), h: Math.round(Math.min(was.h, info.height - was.y)) });
  });
  const ro = new ResizeObserver(resize);
  ro.observe(container);
  resize();
  frame = requestAnimationFrame(paint);
  return {
    get view() { return view; },
    setMarks(m) { current = m || []; dirty = true; },
    draw(on) { draw = !!on; canvas.classList.toggle('drawing', draw); },
    home,
    zoom: zoomBy,
    // How many tiles of the level in view are drawn: for checks that drive the page.
    loaded() { const f = factorFor(info, view.scale); const ts = tiles(info, f, view.visible()); return { factor: f, tiles: ts.length, drawn: ts.filter((t) => image(t.url)).length }; },
    destroy() { live = false; cancelAnimationFrame(frame); ro.disconnect(); canvas.remove(); keys.remove(); },
  };
}
