// The canvas of a project: the figure it makes (the design note of 2026-11-07, §9, brought to the centre at the user's word). Frames
// lie on it, each with its own place and size in the canvas's pixels: the world (the scene's globe, one renderer), 2D maps that follow
// the world (the visualization layer's 2D GIS, its naming policy: no names drawn, areas in dispute hatched in no country's colour),
// statistical charts (the workbench's charts), drawings of a script's own, and text. The canvas is shown fitted to the viewport; it is
// exported at any multiple of its size, every frame drawn again at that size.

import { h, css } from '/src/dom.js';
import * as chart from '/src/draw/chart.js';
import { createMap, styledClone, embeddedFonts } from '/visualization/js/map/index.js';

const SVGNS = 'http://www.w3.org/2000/svg';
const sv = (tag, attrs = {}, ...kids) => {
  const n = document.createElementNS(SVGNS, tag);
  for (const [k, v] of Object.entries(attrs)) if (v !== null && v !== undefined) n.setAttribute(k, v);
  for (const c of kids.flat()) if (c != null) n.append(c instanceof Node ? c : document.createTextNode(String(c)));
  return n;
};
const COUNTRIES = '/data/natural-earth-50m/ne_50m_admin_0_countries.geojson';
const DISPUTED = '/data/natural-earth-50m/ne_50m_admin_0_breakaway_disputed_areas.geojson';
const FONTS = { serif: 'var(--serif)', label: 'var(--label)', mono: 'var(--mono)', text: 'var(--text)' };
// A colour in a drawing: a token of the page (accent, minor, ink, ink-2, ink-faint, paper, series-1…) or a colour as written.
const paint = (c) => (!c || c === 'none' ? 'none' : /^[a-z][a-z0-9-]*$/.test(c) ? `var(--${c})` : c);

let geography = null;
function countries() {
  if (!geography) geography = Promise.all([fetch(COUNTRIES).then((r) => (r.ok ? r.json() : null)).catch(() => null), fetch(DISPUTED).then((r) => (r.ok ? r.json() : null)).catch(() => null)]);
  return geography;
}

export function createCanvas({ host, project, scene, onSelect = () => {}, onMoved = () => {} }) {
  const plate = h('div', { class: 'st-plate' });
  const ticks = sv('svg', { class: 'st-ticks', 'aria-hidden': 'true', preserveAspectRatio: 'none', viewBox: '0 0 100 100' }, sv('path', { d: 'M0 3V0h3M97 0h3v3M100 97v3h-3M3 100H0v-3' }));
  host.append(plate);
  const els = new Map();          // frame id → { el, body, kind, map?, content? }
  const contents = new Map();     // frame id → the last mark drawn into it (a chart or a drawing), and by whom
  let scale = 1, arranging = false, selected = null;
  const P = () => project();

  // ---- layout -------------------------------------------------------------------------------------------------------------
  function fit() {
    const cv = P()?.canvas;
    if (!cv) return;
    const r = host.getBoundingClientRect(), pad = 18;
    scale = Math.max(0.05, Math.min((r.width - 2 * pad) / cv.width, (r.height - 2 * pad) / cv.height));
    plate.style.width = `${cv.width * scale}px`;
    plate.style.height = `${cv.height * scale}px`;
    plate.style.left = `${(r.width - cv.width * scale) / 2}px`;
    plate.style.top = `${(r.height - cv.height * scale) / 2}px`;
    plate.dataset.background = cv.background || 'theme';
    for (const f of P().frames) place(f);
  }
  function place(f) {
    const e = els.get(f.id);
    if (!e) return;
    Object.assign(e.el.style, { left: `${f.x * scale}px`, top: `${f.y * scale}px`, width: `${f.w * scale}px`, height: `${f.h * scale}px` });
    e.el.dataset.back = f.back || 'none';
    if (f.kind === 'text') drawText(f);
  }

  // The frames made again from the project: the world's element is kept (its globe lives in it), the others made anew where their kind
  // or content needs.
  function build(worldEl) {
    const keep = new Set(P().frames.map((f) => f.id));
    for (const [id, e] of els) if (!keep.has(id)) { e.el.remove(); els.delete(id); contents.delete(id); }
    P().frames.forEach((f, k) => {
      let e = els.get(f.id);
      if (e && e.kind !== f.kind) { e.el.remove(); els.delete(f.id); e = null; }
      if (!e) {
        const body = f.kind === 'world' ? worldEl : h('div', { class: 'st-body' });
        const el = h('div', { class: `st-frame ${f.kind}` }, body, h('div', { class: 'st-cover' }, h('span', { class: 'st-tag' }, f.title || f.id), h('i', { class: 'st-grip' })));
        el.dataset.id = f.id;
        e = { el, body, kind: f.kind };
        els.set(f.id, e);
        wire(e);
        if (f.kind === 'map') makeMap(f, e);
      }
      e.el.querySelector('.st-tag').textContent = f.title || f.id;
      e.el.style.zIndex = String(k + 1);
      e.el.classList.toggle('chosen', selected === f.id);
      plate.append(e.el);
      if (f.kind === 'chart' && f.chart?.resource) drawChart(f, null);
      else if (contents.has(f.id)) draw(f.id, contents.get(f.id).mark);
      else if (f.kind === 'chart' || f.kind === 'drawing') empty(f, e);
    });
    plate.append(ticks);
    fit();
  }

  function empty(f, e) {
    if (e.body.dataset.empty) return;
    e.body.replaceChildren(h('p', { class: 'st-hint' }, f.kind === 'chart' ? `a chart: a script draws it with chart("${f.id}", …), or it follows a resource` : `a drawing: a script draws it with drawing("${f.id}", …)`));
    e.body.dataset.empty = '1';
  }

  // ---- arranging: frames moved and resized on the canvas, in its pixels, to a grid of 4 --------------------------------------------
  function wire(e) {
    const cover = e.el.querySelector('.st-cover');
    let drag = null;
    cover.addEventListener('pointerdown', (ev) => {
      if (!arranging) return;
      ev.preventDefault();
      const f = P().frames.find((q) => q.id === e.el.dataset.id);
      if (!f) return;
      select(f.id);
      drag = { f, resize: ev.target.classList.contains('st-grip'), x0: ev.clientX, y0: ev.clientY, f0: { ...f } };
      cover.setPointerCapture(ev.pointerId);
    });
    cover.addEventListener('pointermove', (ev) => {
      if (!drag) return;
      const cv = P().canvas, snap = (v) => Math.round(v / 4) * 4;
      const dx = (ev.clientX - drag.x0) / scale, dy = (ev.clientY - drag.y0) / scale, f = drag.f, o = drag.f0;
      if (drag.resize) {
        f.w = Math.max(40, Math.min(cv.width - o.x, snap(o.w + dx)));
        f.h = Math.max(30, Math.min(cv.height - o.y, snap(o.h + dy)));
      } else {
        f.x = Math.max(0, Math.min(cv.width - f.w, snap(o.x + dx)));
        f.y = Math.max(0, Math.min(cv.height - f.h, snap(o.y + dy)));
      }
      place(f);
    });
    const end = () => {
      if (!drag) return;
      const f = drag.f, changed = ['x', 'y', 'w', 'h'].some((k) => f[k] !== drag.f0[k]);
      drag = null;
      if (changed) { redraw(f.id); onMoved(f); }
    };
    cover.addEventListener('pointerup', end);
    cover.addEventListener('pointercancel', end);
    // Out of arranging, a frame other than the world and a map is chosen by a click.
    e.el.addEventListener('click', () => { if (!arranging && !['world', 'map'].includes(e.kind)) select(e.el.dataset.id); });
  }
  function setArrange(on) { arranging = on; plate.classList.toggle('arranging', on); }
  function select(id) {
    selected = id;
    for (const [k, e] of els) e.el.classList.toggle('chosen', k === id);
    onSelect(id);
  }

  // ---- what frames hold ---------------------------------------------------------------------------------------------------
  // A mark of a script (a chart or a drawing) into its frame, drawn at the frame's size in the canvas's pixels.
  function draw(frameId, mark, by) {
    const f = P().frames.find((q) => q.id === frameId);
    if (!f) return false;
    contents.set(frameId, { mark, by: by ?? contents.get(frameId)?.by });
    if (f.kind === 'chart') drawChart(f, mark);
    else if (f.kind === 'drawing') drawDrawing(f, mark);
    return true;
  }
  function redraw(id) {
    const f = P().frames.find((q) => q.id === id);
    if (!f) return;
    if (f.kind === 'map') { const e = els.get(id); if (e?.map) e.lastKey = null; }
    else if (contents.has(id)) draw(id, contents.get(id).mark);
    else if (f.kind === 'chart' && f.chart?.resource) drawChart(f, null);
  }

  // A resource's values over the ticks, kept for the charts that follow it.
  const histories = new Map();
  function record(name, tick, value) {
    if (!Number.isFinite(value)) return;
    if (!histories.has(name)) histories.set(name, { x: [], y: [] });
    const s = histories.get(name);
    s.x.push(tick), s.y.push(value);
    if (s.x.length > 600) s.x.splice(0, s.x.length - 600), s.y.splice(0, s.y.length - 600);
  }
  const followed = () => [...new Set(P().frames.filter((f) => f.kind === 'chart' && f.chart?.resource).map((f) => f.chart.resource))];
  function drawFollowers() { for (const f of P().frames) if (f.kind === 'chart' && f.chart?.resource) drawChart(f, null); }
  function clearHistories() { histories.clear(); }

  function drawChart(f, mark) {
    const e = els.get(f.id);
    if (!e) return;
    let m = mark;
    if (!m && f.chart?.resource) {
      const s = histories.get(f.chart.resource) || { x: [], y: [] };
      m = { chart: 'line', x: s.x.slice(-(f.chart.history || 300)), y: s.y.slice(-(f.chart.history || 300)), title: f.title || f.chart.resource, xlabel: 'tick', ylabel: f.chart.resource };
    }
    if (!m) return;
    const head = 30, W = f.w, H = Math.max(60, f.h - head);
    let svgEl;
    try {
      const series = m.series || (m.x && m.y ? [{ name: m.title, x: m.x, y: m.y }] : []);
      const has = series.some((s) => (s.x || []).length && (s.y || []).some(Number.isFinite));
      if (!has) svgEl = sv('svg', { viewBox: `0 0 ${W} ${H}` }, sv('text', { x: W / 2, y: H / 2, 'text-anchor': 'middle', class: 'st-nodata' }, 'no values yet'));
      else if (m.chart === 'bars') svgEl = chart.bars(series[0].x.map((x, i) => ({ x: i, y: series[0].y[i], label: `${fmtNum(x)}: ${fmtNum(series[0].y[i])}` })), { width: W, height: H, xlabel: m.xlabel, ylabel: m.ylabel, labels: series[0].x.map(fmtNum) });
      else if (m.chart === 'scatter') svgEl = chart.scatter(series[0].x.map((x, i) => ({ x, y: series[0].y[i] })), { width: W, height: H, xlabel: m.xlabel, ylabel: m.ylabel });
      else svgEl = chart.lines(series.map((s, k) => ({ ...s, color: css(`--series-${(k % 5) + 1}`) })), { width: W, height: H, xlabel: m.xlabel, ylabel: m.ylabel });
    } catch (error) {
      svgEl = sv('svg', { viewBox: `0 0 ${W} ${H}` }, sv('text', { x: 8, y: 20, class: 'st-nodata' }, `not drawn: ${error.message}`));
    }
    svgEl.removeAttribute('style');
    svgEl.setAttribute('width', '100%');
    svgEl.setAttribute('height', '100%');
    svgEl.setAttribute('preserveAspectRatio', 'none');
    const title = h('div', { class: 'st-charttitle' }, m.title || f.title || '');
    title.style.fontSize = `${15 * scale}px`;
    title.style.height = `${head * scale}px`;
    const box = h('div', { class: 'st-chartbox' }, svgEl);
    box.style.height = `calc(100% - ${head * scale}px)`;
    delete e.body.dataset.empty;
    e.body.replaceChildren(title, box);
    e.svg = svgEl, e.title = m.title || f.title || '', e.head = head;
  }
  const fmtNum = (v) => (Number.isFinite(v) ? (Math.abs(v) >= 100 ? v.toFixed(0) : Math.abs(v) >= 10 ? v.toFixed(1) : v.toFixed(2)) : '');

  function drawDrawing(f, mark) {
    const e = els.get(f.id);
    if (!e) return;
    const items = (mark.items || []).slice(0, 5000).map((it) => {
      const st = `fill:${paint(it.fill ?? (it.type === 'text' ? 'ink' : 'none'))};stroke:${paint(it.stroke ?? (it.type === 'text' ? 'none' : 'ink'))};stroke-width:${it.width ?? 1}${it.opacity != null ? `;opacity:${it.opacity}` : ''}`;
      if (it.type === 'line') return sv('line', { x1: it.x1, y1: it.y1, x2: it.x2, y2: it.y2, style: st });
      if (it.type === 'circle') return sv('circle', { cx: it.cx, cy: it.cy, r: it.r, style: st });
      if (it.type === 'rect') return sv('rect', { x: it.x, y: it.y, width: it.w, height: it.h, style: st });
      if (it.type === 'path') return sv('path', { d: it.d, style: st });
      if (it.type === 'text') return sv('text', { x: it.x, y: it.y, 'text-anchor': it.anchor || 'start', style: `${st};font-family:${FONTS[it.font] || FONTS.label};font-size:${it.size || 12}px${it.italic ? ';font-style:italic' : ''}` }, String(it.text ?? ''));
      return null;
    }).filter(Boolean);
    const svgEl = sv('svg', { viewBox: `0 0 ${f.w} ${f.h}`, width: '100%', height: '100%', preserveAspectRatio: 'none' }, items);
    delete e.body.dataset.empty;
    e.body.replaceChildren(svgEl);
    e.svg = svgEl, e.title = '', e.head = 0;
  }

  function drawText(f) {
    const e = els.get(f.id);
    if (!e) return;
    const t = f.text || {};
    const p = e.body.querySelector('.st-text') || e.body.appendChild(h('div', { class: 'st-text' }));
    p.textContent = t.text || '';
    p.style.fontFamily = FONTS[t.font] || FONTS.serif;
    p.style.fontSize = `${(t.size || 24) * scale}px`;
    p.style.fontStyle = (t.font || 'serif') === 'serif' ? 'italic' : 'normal';
    p.style.fontWeight = (t.font || 'serif') === 'serif' ? '300' : '400';
    p.style.textAlign = t.align || 'left';
  }

  // ---- maps that follow the world ---------------------------------------------------------------------------------------
  async function makeMap(f, e) {
    const svgEl = sv('svg', { class: 'st-map', id: `map-${f.id}`, width: '100%', height: '100%' });
    e.body.replaceChildren(svgEl);
    const [geo, disputed] = await countries();
    e.map = createMap(svgEl, { projection: f.map?.projection || 'equal-earth', countries: geo, disputed, nameOf: () => '', maxZoom: 60 });
    e.svg = svgEl, e.title = '', e.head = 0, e.lastKey = null;
  }

  // The world's footprint and what its layers draw, on each map; the map is fitted to the footprint while it follows the world.
  function drawMaps(view, layers) {
    for (const f of P().frames) {
      if (f.kind !== 'map') continue;
      const e = els.get(f.id);
      if (!e?.map) continue;
      const key = JSON.stringify([view?.bbox, view?.footprint?.length, layers.version, f.w, f.h]);
      if (key === e.lastKey) continue;
      e.lastKey = key;
      const M = e.map, Pj = M.planar, over = M.over, px = M.px();
      over.replaceChildren();
      const pts = [];
      for (const L of layers.list) {
        for (const m of L.marks) {
          if (m.kind === 'points' || m.kind === 'columns') {
            const n = Math.min(m.lon.length, 4000);
            for (let k = 0; k < n; k++) pts.push(Pj.project(m.lon[k], m.lat[k]));
          } else if (m.kind === 'lines') {
            const d = (m.paths || []).slice(0, 2000).map((path) => path.map(([lon, lat], i) => { const [x, y] = Pj.project(lon, lat); return `${i ? 'L' : 'M'}${x.toFixed(1)},${y.toFixed(1)}`; }).join('')).join('');
            over.append(sv('path', { d, class: 'st-mapline' }));
          }
        }
      }
      if (pts.length) over.append(sv('path', { class: 'st-mapdot', d: pts.map(([x, y]) => `M${x.toFixed(1)},${y.toFixed(1)}h0.01`).join(''), style: `stroke-width:${2.4 * px}px` }));
      const ring = view?.footprint?.length >= 3 ? view.footprint : view?.bbox ? [[view.bbox[0], view.bbox[1]], [view.bbox[2], view.bbox[1]], [view.bbox[2], view.bbox[3]], [view.bbox[0], view.bbox[3]]] : null;
      if (ring) {
        const xy = ring.map(([lon, lat]) => Pj.project(lon, lat));
        over.append(sv('path', { class: 'st-footprint', d: xy.map(([x, y], i) => `${i ? 'L' : 'M'}${x.toFixed(1)},${y.toFixed(1)}`).join('') + 'Z', style: `stroke-width:${1.4 * px}px` }));
        if ((f.map?.follows || 'world') === 'world') {
          const xs = xy.map((q) => q[0]), ys = xy.map((q) => q[1]);
          const wide = Math.max(...xs) - Math.min(...xs), tall = Math.max(...ys) - Math.min(...ys);
          // The footprint fitted with room about it: a tenth of the map's width at least, so that a street is seen in its region.
          const room = Math.max(wide, tall, Pj.W * 0.12);
          const cx = (Math.max(...xs) + Math.min(...xs)) / 2, cy = (Math.max(...ys) + Math.min(...ys)) / 2;
          M.fit([[cx - room, cy - room * 0.62], [cx + room, cy + room * 0.62]], { margin: 0.9 });
        }
      }
    }
  }

  // ---- the canvas as one image ------------------------------------------------------------------------------------------
  async function image(factor = 2) {
    const cv = P().canvas, out = document.createElement('canvas');
    out.width = Math.round(cv.width * factor), out.height = Math.round(cv.height * factor);
    const g = out.getContext('2d');
    g.scale(factor, factor);
    const bg = cv.background === 'paper' ? css('--paper-light') || '#FAF4FA' : cv.background === 'night' ? '#150D1B' : css('--paper');
    g.fillStyle = /^#|^rgb/.test(cv.background || '') ? cv.background : bg;
    g.fillRect(0, 0, cv.width, cv.height);
    let fonts = '';
    try { fonts = await embeddedFonts(); } catch { fonts = ''; }
    for (const f of P().frames) {
      const e = els.get(f.id);
      if (!e) continue;
      g.save();
      g.beginPath();
      g.rect(f.x, f.y, f.w, f.h);
      g.clip();
      if (f.back === 'vellum' || f.back === 'paper') { g.fillStyle = css(f.back === 'vellum' ? '--vellum-strong' : '--paper'); g.fillRect(f.x, f.y, f.w, f.h); }
      if (f.kind === 'world') {
        const r = scene.earth.renderer, old = r.getPixelRatio(), shown = e.el.getBoundingClientRect().width || 1;
        r.setPixelRatio((f.w * factor) / shown);
        scene.earth.resize();
        scene.earth.frame(performance.now());
        g.drawImage(scene.earth.canvas, f.x, f.y, f.w, f.h);
        r.setPixelRatio(old);
        scene.earth.resize();
        const over = e.el.querySelector('.st-overlay');
        if (over && !over.hidden && over.complete) {
          g.globalAlpha = Number(over.style.opacity || 1);
          const s = Math.max(f.w / over.naturalWidth, f.h / over.naturalHeight);
          g.drawImage(over, f.x + (f.w - over.naturalWidth * s) / 2, f.y + (f.h - over.naturalHeight * s) / 2, over.naturalWidth * s, over.naturalHeight * s);
          g.globalAlpha = 1;
        }
      } else if (f.kind === 'text') {
        const t = f.text || {}, size = t.size || 24, font = t.font || 'serif';
        g.fillStyle = css('--ink');
        g.font = `${font === 'serif' ? 'italic 300' : '400'} ${size}px ${font === 'serif' ? '"Source Serif 4", Georgia, serif' : font === 'mono' ? '"IBM Plex Mono", monospace' : '"IBM Plex Sans Condensed", sans-serif'}`;
        g.textBaseline = 'top';
        g.textAlign = t.align === 'center' ? 'center' : t.align === 'right' ? 'right' : 'left';
        const x = t.align === 'center' ? f.x + f.w / 2 : t.align === 'right' ? f.x + f.w : f.x;
        let line = '', y = f.y;
        for (const word of String(t.text || '').split(/\s+/)) {
          const next = line ? `${line} ${word}` : word;
          if (g.measureText(next).width > f.w && line) { g.fillText(line, x, y); line = word; y += size * 1.22; } else line = next;
        }
        if (line) g.fillText(line, x, y);
      } else if (e.svg) {
        if (e.title) {
          g.fillStyle = css('--ink');
          g.font = `italic 300 15px "Source Serif 4", Georgia, serif`;
          g.textBaseline = 'middle';
          g.fillText(e.title, f.x + 10, f.y + e.head / 2);
        }
        const copy = styledClone(e.svg);
        const top = f.y + (e.head || 0), height = f.h - (e.head || 0);
        copy.setAttribute('xmlns', SVGNS);
        copy.setAttribute('width', String(f.w * factor));
        copy.setAttribute('height', String(height * factor));
        if (fonts) copy.insertBefore(sv('style', {}, fonts), copy.firstChild);
        const url = `data:image/svg+xml;charset=utf-8,${encodeURIComponent(new XMLSerializer().serializeToString(copy))}`;
        const img = new Image();
        await new Promise((ok) => { img.onload = ok; img.onerror = ok; img.src = url; });
        if (img.naturalWidth) g.drawImage(img, f.x, top, f.w, height);
      }
      if (f.back === 'vellum') { g.strokeStyle = css('--hair-2'); g.lineWidth = 1; g.strokeRect(f.x + 0.5, f.y + 0.5, f.w - 1, f.h - 1); }
      g.restore();
    }
    return out.toDataURL('image/png');
  }

  new ResizeObserver(() => fit()).observe(host);
  return {
    plate, build, fit, draw, redraw, setArrange, select, drawMaps, image, record, followed, drawFollowers, clearHistories,
    get scale() { return scale; }, get arranging() { return arranging; },
    contentOf: (id) => contents.get(id),
    element: (id) => els.get(id)?.el,
    clearContents() { contents.clear(); for (const [, e] of els) if (e.kind === 'chart' || e.kind === 'drawing') { delete e.body.dataset.empty; e.svg = null; } },
  };
}
