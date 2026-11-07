// A rendering project of the Studio (the design note of 2026-11-07, §4): its manifest, made and checked here, without a page or a
// renderer, so that it is tested in Node and any application may open one.
//
//   project   { schema, meta, canvas, frames, scene, layers, resources, scripts }
//   canvas    { width, height (pixels of the figure), background: 'theme' | 'paper' | 'night' | '#rrggbb' }
//   frame     { id, kind: 'world' | 'map' | 'chart' | 'drawing' | 'text', title, x, y, w, h (the canvas's pixels), back: 'none' | 'vellum' | 'paper',
//               map: { follows: 'world', projection }, chart: { resource?, history? }, text: { text, size, font: 'serif' | 'label', align } }
//   scene     { camera: {lon, lat, height, range, tilt, heading} (degrees, metres), clock: {start_ms, rate, tick_hz}, theme, base }
//   layer     { id, title, kind: 'data' | 'script', source?, script?, visible, opacity, style }
//   resource  { name, type: 'number' | 'text' | 'numbers' | 'points' | 'record', merge?, initial, saved }
//   script    { id, file, language: 'js' | 'python' | 'cpp', enabled }
//
// A project is a folder: project.json beside its scripts (scripts/<file>). The schema's name carries its version.
//
// The project is a canvas: the figure it makes. Its frames lie on it, each with its own place and size: the world (the 3D scene, its
// camera and what its layers draw), 2D maps following the world, statistical charts, drawings of a script's own, and text. The world's
// layers are what is drawn in the world frame; a script draws on the world or into any frame.

export const SCHEMA = 'noetarium.project/1';
export const LANGUAGES = { js: 'JavaScript', python: 'Python', cpp: 'C++' };
export const EXTENSIONS = { js: '.js', python: '.py', cpp: '.cpp' };
export const RESOURCE_TYPES = ['number', 'text', 'numbers', 'points', 'record'];
export const FRAME_KINDS = ['world', 'map', 'chart', 'drawing', 'text'];
export const CANVAS = { width: 1600, height: 1000, background: 'theme' };

export const slug = (title) => String(title || 'project').toLowerCase().normalize('NFKD').replace(/[^\w\s-]/g, '').trim().replace(/[\s_]+/g, '-').replace(/-+/g, '-').slice(0, 48) || 'project';

export function newProject(title = 'A new project', now = Date.now()) {
  return {
    schema: SCHEMA,
    meta: { title, created: new Date(now).toISOString(), saved: null },
    scene: {
      camera: { lon: 144.9631, lat: -37.8136, height: 0, range: 2.2e7, tilt: 0, heading: 0 },
      clock: { start_ms: now, rate: 1, tick_hz: 10 },
      theme: 'night',
      base: 'globe',
    },
    canvas: { ...CANVAS },
    frames: [{ id: 'world', kind: 'world', title: 'The world', x: 0, y: 0, w: CANVAS.width, h: CANVAS.height, back: 'none' }],
    layers: [],
    resources: [],
    scripts: [],
  };
}

const finite = (x) => typeof x === 'number' && Number.isFinite(x);

// What is wrong with a project, in words; an empty list if nothing is.
export function problemsOf(p) {
  const out = [];
  if (!p || typeof p !== 'object') return ['a project is an object'];
  if (p.schema !== SCHEMA) out.push(`its schema is ${JSON.stringify(p.schema)}, not ${SCHEMA}`);
  const c = p.scene?.camera;
  if (!c || !['lon', 'lat', 'range'].every((k) => finite(c[k]))) out.push('its scene has no camera with a longitude, latitude and range');
  else if (Math.abs(c.lat) > 90) out.push('its camera\'s latitude is beyond the poles');
  if (!finite(p.scene?.clock?.tick_hz) || p.scene.clock.tick_hz <= 0 || p.scene.clock.tick_hz > 120) out.push('its clock\'s tick is not between 0 and 120 a second');
  const ids = new Set();
  for (const s of p.scripts || []) {
    if (!s.id || ids.has(s.id)) out.push(`a script without an id, or with one used twice (${s.id})`);
    ids.add(s.id);
    if (!LANGUAGES[s.language]) out.push(`script ${s.id}: no language "${s.language}"`);
    if (!/^[A-Za-z0-9][A-Za-z0-9_-]{0,63}\.(js|py|cpp)$/.test(s.file || '')) out.push(`script ${s.id}: its file "${s.file}" is not a name the Studio keeps`);
    else if (EXTENSIONS[s.language] && !s.file.endsWith(EXTENSIONS[s.language])) out.push(`script ${s.id}: its file does not end ${EXTENSIONS[s.language]}`);
  }
  const names = new Set();
  for (const r of p.resources || []) {
    if (!r.name || names.has(r.name)) out.push(`a resource without a name, or with one used twice (${r.name})`);
    names.add(r.name);
    if (!RESOURCE_TYPES.includes(r.type)) out.push(`resource ${r.name}: no type "${r.type}"`);
  }
  const cv = p.canvas;
  if (!cv || !finite(cv.width) || !finite(cv.height) || cv.width < 64 || cv.height < 64 || cv.width > 20000 || cv.height > 20000) out.push('its canvas has no width and height between 64 and 20,000 pixels');
  const frames = new Set();
  for (const f of p.frames || []) {
    if (!f.id || frames.has(f.id)) out.push(`a frame without an id, or with one used twice (${f.id})`);
    frames.add(f.id);
    if (!FRAME_KINDS.includes(f.kind)) out.push(`frame ${f.id}: no kind "${f.kind}"`);
    if (!['x', 'y', 'w', 'h'].every((k) => finite(f[k])) || f.w <= 0 || f.h <= 0) out.push(`frame ${f.id}: no place and size`);
    else if (cv && (f.x >= cv.width || f.y >= cv.height || f.x + f.w <= 0 || f.y + f.h <= 0)) out.push(`frame ${f.id}: off the canvas`);
  }
  if ((p.frames || []).filter((f) => f.kind === 'world').length !== 1) out.push('its canvas has not one world frame');
  const layers = new Set();
  for (const l of p.layers || []) {
    if (!l.id || layers.has(l.id)) out.push(`a layer without an id, or with one used twice (${l.id})`);
    layers.add(l.id);
    if (l.kind === 'script' && !ids.has(l.script)) out.push(`layer ${l.id}: its script ${l.script} is not in the project`);
  }
  return out;
}

// A fresh id: a word and a number not yet used among those given.
export function freshId(stem, used) {
  const base = slug(stem).replace(/-/g, '_') || 'item';
  let k = 1;
  while (used.has(`${base}${k === 1 ? '' : `_${k}`}`)) k++;
  return `${base}${k === 1 ? '' : `_${k}`}`;
}

// A project of an earlier shape given a canvas: the world filling it, as the scene was the whole project.
export function withCanvas(p) {
  if (!p.canvas) p.canvas = { ...CANVAS };
  if (!Array.isArray(p.frames) || !p.frames.some((f) => f.kind === 'world')) p.frames = [{ id: 'world', kind: 'world', title: 'The world', x: 0, y: 0, w: p.canvas.width, h: p.canvas.height, back: 'none' }, ...(p.frames || [])];
  return p;
}

// A place for a new frame of w by h: the corner of the canvas where it covers least of the other frames (the world aside), inset by a margin.
export function placeFrame(canvas, frames, w, h, margin = 32) {
  const W = canvas.width, H = canvas.height;
  const spots = [[W - w - margin, H - h - margin], [margin, H - h - margin], [W - w - margin, margin], [margin, margin], [(W - w) / 2, H - h - margin]];
  const others = frames.filter((f) => f.kind !== 'world');
  const overlap = (x, y) => others.reduce((a, f) => a + Math.max(0, Math.min(x + w, f.x + f.w) - Math.max(x, f.x)) * Math.max(0, Math.min(y + h, f.y + f.h) - Math.max(y, f.y)), 0);
  let best = spots[0], least = Infinity;
  for (const [x, y] of spots) { const o = overlap(x, y); if (o < least) { least = o; best = [x, y]; } }
  return { x: Math.round(Math.max(0, best[0])), y: Math.round(Math.max(0, best[1])), w: Math.min(w, W), h: Math.min(h, H) };
}
