// A scene of the Studio: the project's own renderer (the design note of 2026-11-07, §2, §6), a globe of the visualization layer
// instanced again on the Studio's page, with its own camera, Sun and loop; and the marks the project's layers and scripts declare,
// drawn on it: points, columns and lines on WGS 84, in degrees and metres. Nothing here touches the workbench's main view.

import * as THREE from 'three';
import * as G from '/visualization/js/gis/index.js';
import { cartesianDeg } from '/visualization/js/globe/index.js';
import { colourMap } from '/visualization/styles/index.js';
import { subsolarPoint } from '/domains/astronomy/js/index.js';
import { look } from '/src/shell/look.js';

const DEG = Math.PI / 180;
const hex = (s) => { const x = String(s).replace('#', ''); const v = x.length === 3 ? x.split('').map((c) => c + c).join('') : x; return [0, 2, 4].map((i) => parseInt(v.slice(i, i + 2), 16) / 255); };

// A round dot for points, drawn once.
let dotTexture = null;
function dot() {
  if (dotTexture) return dotTexture;
  const c = document.createElement('canvas');
  c.width = c.height = 64;
  const g = c.getContext('2d');
  const grad = g.createRadialGradient(32, 32, 0, 32, 32, 30);
  grad.addColorStop(0, 'rgba(255,255,255,1)');
  grad.addColorStop(0.62, 'rgba(255,255,255,0.95)');
  grad.addColorStop(1, 'rgba(255,255,255,0)');
  g.fillStyle = grad;
  g.beginPath();
  g.arc(32, 32, 30, 0, Math.PI * 2);
  g.fill();
  dotTexture = new THREE.CanvasTexture(c);
  return dotTexture;
}

// Each mark's values as colours: a constant, or a value through a validated map over a domain.
function colours(mark, n) {
  if (!Array.isArray(mark.value) || !mark.value.length) {
    const c = hex(mark.colour || '#EE8FBD');
    return Array.from({ length: n }, () => c);
  }
  const vs = mark.value.filter(Number.isFinite);
  const [lo, hi] = mark.domain || [Math.min(...vs), Math.max(...vs)];
  let map;
  try { map = colourMap(mark.map || 'plum').colour; } catch { map = colourMap('plum').colour; }
  return mark.value.map((v) => (Number.isFinite(v) ? map(hi > lo ? Math.min(1, Math.max(0, (v - lo) / (hi - lo))) : 0.7) : [0.5, 0.45, 0.5]));
}

export function createScene(container, { theme = 'night' } = {}) {
  const earth = G.createEarth(THREE, container, {
    eventElement: container, preserveDrawingBuffer: true,
    terrainUrl: (z, x, y) => `${location.origin}/tiles/terrain/earth/${z}/${x}/${y}`,
  });
  const setTheme = (t) => { const L = look(t); earth.setLook({ ...L, haze: L.sky(0).horizon, tintAmount: L.tintAmount }); };
  setTheme(theme);
  const layers = new Map();   // id → { marks, visible, opacity, drawn: [{kind, key, …}] }

  const heightOf = (lon, lat, h) => (earth.tree.heightAt?.(lon * DEG, lat * DEG) || 0) + (Number.isFinite(h) ? h : 0);

  function drawPoints(key, m, style) {
    const n = Math.min(m.lon.length, m.lat.length);
    if (!n) return null;
    const centre = cartesianDeg(earth.fig, m.lon[0], m.lat[0], 0);
    const pos = new Float32Array(3 * n), col = new Float32Array(3 * n);
    const cs = colours(m, n);
    for (let k = 0; k < n; k++) {
      const h = Array.isArray(m.height) ? m.height[k] : m.height;
      const p = cartesianDeg(earth.fig, m.lon[k], m.lat[k], heightOf(m.lon[k], m.lat[k], h) + 30);
      pos[3 * k] = p[0] - centre[0], pos[3 * k + 1] = p[1] - centre[1], pos[3 * k + 2] = p[2] - centre[2];
      col.set(cs[k], 3 * k);
    }
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute('position', new THREE.BufferAttribute(pos, 3));
    geometry.setAttribute('color', new THREE.BufferAttribute(col, 3));
    geometry.computeBoundingSphere();
    const material = new THREE.PointsMaterial({ size: (m.size || 6) * Math.min(2, devicePixelRatio || 1), sizeAttenuation: false, vertexColors: true, map: dot(),
      transparent: true, alphaTest: 0.05, depthWrite: false, opacity: style.opacity ?? 1 });
    const object = new THREE.Points(geometry, material);
    object.renderOrder = 6;
    earth.addObject(key, object, centre);
    return { kind: 'object', key, object };
  }

  function drawLines(key, m, style) {
    const segs = [];
    for (const path of m.paths || []) for (let k = 1; k < path.length; k++) segs.push(path[k - 1], path[k]);
    if (!segs.length) return null;
    const centre = cartesianDeg(earth.fig, segs[0][0], segs[0][1], 0);
    const pos = new Float32Array(3 * segs.length);
    segs.forEach(([lon, lat], k) => {
      const p = cartesianDeg(earth.fig, lon, lat, heightOf(lon, lat, m.height) + 40);
      pos[3 * k] = p[0] - centre[0], pos[3 * k + 1] = p[1] - centre[1], pos[3 * k + 2] = p[2] - centre[2];
    });
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute('position', new THREE.BufferAttribute(pos, 3));
    geometry.computeBoundingSphere();
    const object = new THREE.LineSegments(geometry, new THREE.LineBasicMaterial({ color: new THREE.Color(...hex(m.colour || '#EE8FBD')), transparent: true, opacity: style.opacity ?? 0.9, depthWrite: false }));
    object.renderOrder = 6;
    earth.addObject(key, object, centre);
    return { kind: 'object', key, object };
  }

  // Columns: hexagons of the mark's radius about each point, standing to their value. A set whose places have not moved is only
  // raised and coloured again, which costs little each tick.
  function drawColumns(key, m, style, old) {
    const n = Math.min(m.lon.length, m.lat.length, (m.value || []).length);
    if (!n) return null;
    const placeKey = `${n}|${m.radius}|${m.lon[0]}|${m.lat[0]}|${m.lon[n - 1]}|${m.lat[n - 1]}|${m.lon.reduce((a, b) => a + b, 0).toFixed(6)}`;
    let entry = old && old.kind === 'columns' && old.placeKey === placeKey ? old : null;
    if (!entry) {
      if (old && old.kind === 'columns') earth.removeColumns(old.key);
      const rings = [];
      for (let k = 0; k < n; k++) {
        const c = Math.cos(m.lat[k] * DEG), r = m.radius || 60;
        rings.push({ ring: Array.from({ length: 6 }, (_, i) => { const a = (i * Math.PI) / 3 + Math.PI / 6; return [m.lon[k] + (r * Math.cos(a)) / (111320 * c), m.lat[k] + (r * Math.sin(a)) / 110574]; }) });
      }
      const set = earth.addColumns(key, rings, { shrink: 0.94, opacity: style.opacity ?? 0.95, grow: false });
      entry = { kind: 'columns', key, set, placeKey };
    }
    const vs = m.value.slice(0, n), finite = vs.filter(Number.isFinite);
    const hi = (m.domain && m.domain[1]) || Math.max(1e-9, ...finite);
    const cs = colours({ ...m, value: vs }, n);
    entry.set.prisms.set(vs.map((v) => (Number.isFinite(v) ? (m.top || 300) * (0.04 + 0.96 * Math.max(0, v) / hi) : -1)), cs);
    return entry;
  }

  function removeDrawn(d) {
    if (!d) return;
    if (d.kind === 'object') {
      earth.removeObject(d.key);
      d.object.geometry.dispose();
      d.object.material.dispose();
    } else if (d.kind === 'columns') earth.removeColumns(d.key);
  }

  // A layer's marks drawn again: what is drawn of each mark is kept by its index, so that columns are raised in place.
  function draw(id, marks, style = {}) {
    const L0 = layers.get(id) || { visible: true, opacity: 1, drawn: [] };
    const L1 = { ...L0, ...style, marks, drawn: [] };
    layers.set(id, L1);
    if (!L1.visible) { L0.drawn.forEach(removeDrawn); return; }
    const used = new Set();
    marks.forEach((m, k) => {
      const key = `${id}#${k}`;
      const old = L0.drawn[k];
      let d = null;
      try {
        if (m.kind === 'columns') d = drawColumns(key, m, L1, old);
        else {
          if (old) removeDrawn(old);
          if (m.kind === 'points') d = drawPoints(key, m, L1);
          else if (m.kind === 'lines') d = drawLines(key, m, L1);
        }
      } catch (error) { console.warn('a mark could not be drawn', error); }
      if (d) { L1.drawn[k] = d; used.add(k); }
    });
    L0.drawn.forEach((d, k) => { if (!used.has(k) && d) removeDrawn(d); });
  }

  function clear(id) {
    const L0 = layers.get(id);
    if (!L0) return;
    L0.drawn.forEach(removeDrawn);
    layers.delete(id);
  }

  function setVisible(id, visible) {
    const L0 = layers.get(id);
    if (!L0) return;
    L0.visible = visible;
    if (!visible) { L0.drawn.forEach(removeDrawn); L0.drawn = []; } else draw(id, L0.marks || [], {});
  }
  function setOpacity(id, opacity) {
    const L0 = layers.get(id);
    if (!L0) return;
    L0.opacity = opacity;
    for (const d of L0.drawn) if (d?.kind === 'object') d.object.material.opacity = opacity;
    if (L0.drawn.some((d) => d?.kind === 'columns')) { L0.drawn.filter((d) => d?.kind === 'columns').forEach(removeDrawn); L0.drawn = []; draw(id, L0.marks || [], {}); }
  }

  function setCamera(c, seconds = 0) {
    const v = { lon: c.lon * DEG, lat: c.lat * DEG, height: c.height || 0, range: c.range, tilt: (c.tilt || 0) * DEG, heading: (c.heading || 0) * DEG };
    if (seconds > 0) earth.flyTo(v, seconds); else earth.setView(v);
  }
  function camera() {
    const v = earth.view;
    return { lon: +(v.lon / DEG).toFixed(6), lat: +(v.lat / DEG).toFixed(6), height: Math.round(v.height || 0), range: Math.round(v.range), tilt: +((v.tilt || 0) / DEG).toFixed(2), heading: +((v.heading || 0) / DEG).toFixed(2) };
  }
  function setTime(ms) {
    const s = subsolarPoint(ms);
    earth.setSubsolar(s.lat, s.lon);
  }

  // The world frame's range in space, as a script is given it: the camera (degrees, metres), the footprint (the ground seen along the
  // frame's edges, clockwise from its top left, when all of it is ground), the box of the ground seen (west, south, east, north; east
  // may exceed 180 across the antimeridian), and the metres of ground to a pixel of the canvas at the point looked at.
  function rangeOf(canvasScale = 1) {
    const c = camera(), r = earth.canvas.getBoundingClientRect();
    const out = { ...c, footprint: null, bbox: null, metres_per_px: null };
    if (!r.width || !r.height) return out;
    const at = (u, v) => earth.groundAt(r.left + u * r.width, r.top + v * r.height);
    const n = 6, border = [];
    for (let k = 0; k < n; k++) border.push([k / n, 0]);
    for (let k = 0; k < n; k++) border.push([1, k / n]);
    for (let k = 0; k < n; k++) border.push([1 - k / n, 1]);
    for (let k = 0; k < n; k++) border.push([0, 1 - k / n]);
    const ring = [];
    let whole = true;
    for (const [u, v] of border) { const g = at(u, v); if (g) ring.push([g.lon, g.lat]); else whole = false; }
    const seen = [...ring];
    for (let i = 0; i <= n; i++) for (let j = 0; j <= n; j++) { const g = at(i / n, j / n); if (g) seen.push([g.lon, g.lat]); }
    if (seen.length) {
      const lons = seen.map((q) => q[0]), lats = seen.map((q) => q[1]);
      let w = Math.min(...lons), e = Math.max(...lons);
      // Across the antimeridian the box is the shorter way round.
      if (e - w > 180) { const east = lons.map((x) => (x < 0 ? x + 360 : x)); w = Math.min(...east), e = Math.max(...east); }
      out.bbox = [+w.toFixed(5), +Math.min(...lats).toFixed(5), +e.toFixed(5), +Math.max(...lats).toFixed(5)];
    }
    if (whole) out.footprint = ring.map(([lon, lat]) => [+lon.toFixed(5), +lat.toFixed(5)]);
    const mpp = earth.groundMetresPerPixel?.();
    if (Number.isFinite(mpp)) out.metres_per_px = mpp * canvasScale;
    return out;
  }

  // The scene as an image, at a multiple of the screen's resolution.
  function image(scale = 2) {
    const r = earth.renderer, old = r.getPixelRatio();
    r.setPixelRatio(old * scale);
    earth.resize();
    earth.frame(performance.now());
    const url = earth.canvas.toDataURL('image/png');
    r.setPixelRatio(old);
    earth.resize();
    return url;
  }

  window.addEventListener('resize', () => earth.resize());
  return { earth, draw, clear, setVisible, setOpacity, setCamera, camera, rangeOf, setTime, setTheme, image, layers };
}
