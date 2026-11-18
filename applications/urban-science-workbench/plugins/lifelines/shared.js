// What the lifelines' layers share: the ways in view with a mode's speeds and their widths, people placed on the buildings of their
// cells, the buildings mapped as care, and a place's hazard maps from the data library.
//
// Speeds and widths are declarations: a road's free-flow speed by its class (as the plug-in Reach and use declares them), a width by
// class where a way gives none, and walking at the speed a work chooses. People come from GHS-POP's 1 km cells and are shared among
// the buildings within each cell by floor space (a dasymetric declaration: where people sleep is not mapped), then gathered on cells of
// 50 m so that a request stays small.

import { tileOf } from '/visualization/js/gis/world.js';

const KMH = (v) => v / 3.6;
export const DRIVE = { motorway: KMH(90), motorway_link: KMH(50), trunk: KMH(70), trunk_link: KMH(40), primary: KMH(50), primary_link: KMH(35), secondary: KMH(40),
  secondary_link: KMH(30), tertiary: KMH(35), tertiary_link: KMH(25), residential: KMH(25), unclassified: KMH(25), living_street: KMH(10), service: KMH(15) };
export const WIDTH = { motorway: 14, motorway_link: 7, trunk: 12, trunk_link: 7, primary: 10, primary_link: 7, secondary: 9, secondary_link: 6, tertiary: 8, tertiary_link: 6,
  residential: 6, unclassified: 6, living_street: 5, service: 4, pedestrian: 6, footway: 2.5, path: 2, steps: 2, cycleway: 2.5, track: 3 };
const NO_FOOT = new Set(['motorway', 'motorway_link', 'trunk', 'trunk_link']);
export const CARE = ['hospital', 'clinic', 'doctors', 'medical', 'healthcare'];

// The ways the world holds, as [speed, width, x0, y0, …] for the engine: by road or on foot (at `walk` m/s).
export function waysFor(world, mode, walk = 1) {
  const out = [];
  for (const l of world.lines()) {
    if (l.rail) continue;
    const speed = mode === 'drive' ? DRIVE[l.cls] || 0 : NO_FOOT.has(l.cls) ? 0 : walk;
    if (!(speed > 0)) continue;
    const width = Number.isFinite(l.width) && l.width > 0 ? l.width : WIDTH[l.cls] || 5;
    const row = [Math.round(speed * 1000) / 1000, Math.round(width * 10) / 10];
    for (let i = 0; i < l.xyz.length; i += 3) row.push(Math.round(l.xyz[i] * 10) / 10, Math.round(l.xyz[i + 1] * 10) / 10);
    if (row.length >= 6) out.push(row);
  }
  return out;
}

// Every building the map holds: its centroid, floor space (footprint × storeys of 3.2 m), height, kind and name.
export function buildings(gis) {
  const out = [];
  const add = (ring, height, kind, name) => {
    let a = 0, cx = 0, cy = 0;
    for (let i = 0, n = ring.length; i < n; i++) {
      const [x0, y0] = ring[i], [x1, y1] = ring[(i + 1) % n];
      const c = x0 * y1 - x1 * y0;
      a += c, cx += (x0 + x1) * c, cy += (y0 + y1) * c;
    }
    if (Math.abs(a) < 1e-9) return;
    const storeys = Math.max(1, Math.round((height || 3.2) / 3.2));
    out.push({ x: cx / (3 * a), y: cy / (3 * a), footprint: Math.abs(a) / 2, floor: (Math.abs(a) / 2) * storeys, height: height || 0, kind: String(kind || '').toLowerCase(), name });
  };
  for (const [, layer] of gis.layers) {
    if (layer.kind === 'solids' && layer.features?.length && Array.isArray(layer.features[0].footprint))
      for (const f of layer.features) add(f.footprint, f.height_m, f.kind, f.name);
    if (layer.kind === 'built-solids' && layer.built?.starts) {
      const { rings, starts, features } = layer.built;
      for (let k = 0; k < starts.length / 2; k++) {
        const s = starts[2 * k], n = starts[2 * k + 1], f = features?.[k] || [];
        const ring = [];
        for (let i = 0; i < n; i++) ring.push([rings[2 * (s + i)], rings[2 * (s + i) + 1]]);
        add(ring, f[0], f[3], f[2]);
      }
    }
  }
  return out;
}

// The tiles whose ways have come, and how far from a place they reach (km).
export function loadedTiles(world, lat, lon) {
  const loaded = new Set();
  let km = 0;
  for (const [k, t] of world.tiles) {
    if (!t.lines) continue;
    loaded.add(k);
    const z = 14, n = 2 ** z;
    const lon0 = (t.x / n) * 360 - 180, lon1 = ((t.x + 1) / n) * 360 - 180;
    const lat0 = (Math.atan(Math.sinh(Math.PI * (1 - (2 * (t.y + 1)) / n))) * 180) / Math.PI, lat1 = (Math.atan(Math.sinh(Math.PI * (1 - (2 * t.y) / n))) * 180) / Math.PI;
    km = Math.max(km, Math.abs(lat1 - lat) * 111.32, Math.abs(lat0 - lat) * 111.32, Math.abs(lon1 - lon) * 111.32 * Math.cos((lat * Math.PI) / 180), Math.abs(lon0 - lon) * 111.32 * Math.cos((lat * Math.PI) / 180));
  }
  return { loaded, km };
}

// People within the loaded tiles about a place, shared among buildings by floor space, gathered on 50 m cells: [[x, y, n]].
export async function peopleAbout(ctx, lat, lon, kmMax = 12) {
  const { world, frame, gis } = ctx;
  const { loaded, km } = loadedTiles(world, lat, lon);
  if (!loaded.size) throw new Error('The ways here are still coming: try again in a moment.');
  const r = await fetch(`/world/population?lat=${lat}&lon=${lon}&km=${Math.min(kmMax, Math.max(1.5, km)).toFixed(2)}`);
  const pop = await r.json();
  if (!r.ok) throw new Error(pop.message || `no population here (${r.status})`);
  const [dx, dy] = pop.cell_degrees;
  const cellOf = (la, lo) => `${Math.round(la / dy)}/${Math.round(lo / dx)}`;
  const people = new Map();
  let beyond = 0;
  for (const [la, lo, n] of pop.cells) {
    if (!loaded.has(tileOf(la, lo).join('/'))) { beyond += n; continue; }
    people.set(cellOf(la, lo), { la, lo, n, floor: 0 });
  }
  const bs = buildings(gis);
  for (const b of bs) {
    const g = frame.toGeodetic(b.x, b.y);
    b.cell = cellOf(g.lat, g.lon);
    const c = people.get(b.cell);
    if (c) c.floor += b.floor;
  }
  const bins = new Map();
  const put = (x, y, n) => {
    const k = `${Math.round(x / 50)}/${Math.round(y / 50)}`;
    const v = bins.get(k) || [0, 0, 0];
    v[0] += x * n, v[1] += y * n, v[2] += n;
    bins.set(k, v);
  };
  for (const b of bs) {
    const c = people.get(b.cell);
    if (c && c.floor > 0) put(b.x, b.y, (c.n * b.floor) / c.floor);
  }
  for (const c of people.values())
    if (!(c.floor > 0) && c.n > 0) {
      const p = frame.toLocal(c.la, c.lo);
      put(p[0], p[1], c.n);
    }
  const out = [];
  for (const [x, y, n] of bins.values()) if (n > 0.05) out.push([Math.round(x / n), Math.round(y / n), Math.round(n * 10) / 10]);
  return { people: out, beyond, total: out.reduce((t, p) => t + p[2], 0), buildings: bs };
}

// The frame's centre, for the engine to read maps of longitude and latitude.
export const frameOf = (frame) => ({ lat: frame.origin.lat, lon: frame.origin.lon });

// A place's hazard maps, from the data library's products: Freetown's floods; Miyako's tsunami and flood.
const kept = {};
export async function hazards(area) {
  if (area === 'freetown') {
    kept.freetown ??= fetch('/data/products/freetown-flood.json').then((r) => { if (!r.ok) throw new Error('Freetown\'s maps are not built: python data/products/freetown_flood.py'); return r.json(); });
    const P = await kept.freetown;
    const maps = P.maps.filter((m) => m.peril === 'FLRF').map((m) => ({ return_period: m.return_period, path: m.file, box: m.box, width: m.width, height: m.height, unit: P.depth_code.metres_per_unit }));
    return { name: 'river floods (JBA, the Freetown Flood Risk Assessment)', maps, zone: maps.find((m) => m.return_period === 100), zoneName: 'the 1-in-100-year river flood', shelters: [], credit: P.credit, events: P.events };
  }
  if (area === 'miyako') {
    kept.miyako ??= fetch('/data/products/japan-hazards/03202.json').then((r) => { if (!r.ok) throw new Error('Miyako\'s maps are not built: python data/products/japan_hazards.py'); return r.json(); });
    const P = await kept.miyako;
    const map = (peril) => { const m = P.maps.find((q) => q.peril === peril); return { return_period: 0, path: m.file, box: m.box, width: m.width, height: m.height, unit: m.unit }; };
    return { name: "the largest assumed river flood (MLIT's 想定最大規模)", maps: [map('flood')], zone: map('tsunami'), zoneName: 'the assumed tsunami (津波浸水想定)',
      shelters: P.sites.filter((s) => s.hazards.includes('tsunami')), credit: P.credit };
  }
  return null;
}

// The ground's height in the local frame beneath a point (m), from the Earth's terrain, kept on cells of 10 m: what lines drawn on
// the map stand on, so that they are not buried under a city's hills.
export function groundOf(ctx) {
  const kept = new Map();
  const DEG = Math.PI / 180;
  return (x, y) => {
    const k = `${Math.round(x / 10)}/${Math.round(y / 10)}`;
    if (kept.has(k)) return kept.get(k);
    const g = ctx.frame.toGeodetic(x, y);
    const h = ctx.earth?.tree?.heightAt ? ctx.earth.tree.heightAt(g.lon * DEG, g.lat * DEG) || 0 : 0;
    const z = ctx.frame.toLocal(g.lat, g.lon, h)[2];
    kept.set(k, z);
    return z;
  };
}
