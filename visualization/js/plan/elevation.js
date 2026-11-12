// A drawn building's elevations (ADR 0097): each side seen square on from far away, as an architect draws it: the outer walls that
// face the eye with their windows and doors, the roof's faces, the eaves and the gables, the ground, the levels, a person for
// scale, and behind it the outlines of what stands beside. Pure: from the document and what the domain read of its shell, to SVG.
//
// THE PROJECTION. An elevation is named by the way its face looks: a bearing b in degrees counter-clockwise from east (−90 for the
// face that looks south). The eye looks along v = −(cos b, sin b); its right is r = (v_y, −v_x). A point (x, y, z) is drawn at
// (p · r, z), and p · v is its depth. Faces are drawn from the farthest to the nearest, each filled, so that the nearer hides the
// farther: exact for a building whose faces do not pass through one another, which a plan's walls and a roof's faces do not.
// A wall is drawn when its outer side faces the eye; a roof's face when it leans toward the eye.
//
// What is left out: what stands before the building (a neighbour or a tree nearer the eye would hide it; trees are drawn in
// outline only), shadows, and the thickness of the roof beyond its fascia.

import { CLADDINGS, ROOF_COVERS, roofOf, shellOf, windOf } from './exterior.js';
import { dist, ringArea } from './geometry.js';
import { elevation, wallEnds } from './model.js';

const esc = (s) => String(s).replace(/[&<>"]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
const f1 = (v) => (Math.round(v * 10) / 10).toFixed(1);
const level = (z) => (Math.abs(z) < 0.005 ? '±0.00' : `${z > 0 ? '+' : '−'}${Math.abs(z).toFixed(2)}`);

// The bearings of a building's four elevations: the way its longest outer wall looks, and the three others square to it; for a
// building with no outer wall yet, the four winds. Each is given with its name.
export function elevationBearings(doc, reading) {
  let best = null;
  doc.storeys.forEach((s, ks) => {
    const R = reading && reading.storeys[ks];
    if (!R || !R.sound) return;
    s.walls.forEach((w, i) => {
      const side = R.walls[i];
      if (!side || !side.outer || w.kind === 'open') return;
      const [a, b] = wallEnds(s, i), L = dist(a, b), indoorLeft = side.left !== null && side.left !== undefined && R.rooms[side.left] && R.rooms[side.left].indoor;
      if (!best || L > best.L + 1e-9) best = { L, bearing: Math.atan2(b[1] - a[1], b[0] - a[0]) * 180 / Math.PI + (indoorLeft ? -90 : 90) };
    });
  });
  const b0 = best ? best.bearing : -90, out = [];
  for (let k = 0; k < 4; k++) {
    let b = b0 + 90 * k;
    b = ((b + 180) % 360 + 360) % 360 - 180;
    out.push({ bearing: Math.round(b * 1e6) / 1e6, name: windOf(b) });
  }
  // The south first, then round by the east: as a sheet of elevations is read.
  const order = (n) => ['south', 'south-east', 'east', 'north-east', 'north', 'north-west', 'west', 'south-west'].indexOf(n);
  return out.sort((p, q) => order(p.name) - order(q.name));
}

// What an elevation shows, in metres: faces [{kind, depth, points: [[X, z]…], …}] from far to near, the extent, the levels.
export function elevationData(doc, reading, bearing, { context = null, reach = 28 } = {}) {
  const b = bearing * Math.PI / 180, v = [-Math.cos(b), -Math.sin(b)], r = [v[1], -v[0]];
  const X = (p) => p[0] * r[0] + p[1] * r[1], D = (p) => p[0] * v[0] + p[1] * v[1];
  const faces = [], levels = [{ z: 0, name: 'ground' }], shell = shellOf(doc), roof = roofOf(doc);
  let x0 = Infinity, x1 = -Infinity, top = 0, near = Infinity, far = -Infinity;
  const grow = (pts, depth) => { for (const [x, z] of pts) x0 = Math.min(x0, x), x1 = Math.max(x1, x), top = Math.max(top, z); near = Math.min(near, depth), far = Math.max(far, depth); };

  doc.storeys.forEach((s, ks) => {
    const R = reading && reading.storeys[ks], z = elevation(doc, ks);
    if (ks > 0) levels.push({ z, name: (s.name || `storey ${ks + 1}`).toLowerCase() });
    if (!R || !R.sound) return;
    const indoor = (room) => room !== null && room !== undefined && R.rooms[room] && R.rooms[room].indoor;
    s.walls.forEach((w, i) => {
      const side = R.walls[i];
      if (!side || !side.outer || w.kind === 'open') return;
      const [a, bb] = wallEnds(s, i), L = dist(a, bb);
      if (!(L > 0)) return;
      const dir = [(bb[0] - a[0]) / L, (bb[1] - a[1]) / L], out = indoor(side.left) ? [dir[1], -dir[0]] : [-dir[1], dir[0]];
      const facing = -(out[0] * v[0] + out[1] * v[1]);
      if (facing < 0.02) return;
      const t = Math.max(0.04, w.t), at = (along) => [a[0] + dir[0] * along + out[0] * t / 2, a[1] + dir[1] * along + out[1] * t / 2];
      const p0 = at(-t / 2), p1 = at(L + t / 2), depth = (D(p0) + D(p1)) / 2;
      const points = [[X(p0), z], [X(p1), z], [X(p1), z + s.height], [X(p0), z + s.height]];
      const openings = s.openings.filter((o) => o.wall === i).map((o) => {
        const sill = Math.max(0, o.sill || 0), head = Math.min(s.height, sill + o.height), q0 = X(at(o.at - o.width / 2)), q1 = X(at(o.at + o.width / 2));
        return { kind: o.kind, x0: Math.min(q0, q1), x1: Math.max(q0, q1), z0: z + sill, z1: z + head, lights: Math.max(1, Math.round(o.width / 0.9)), glazed: o.glazed || 0, entrance: o.kind === 'door' };
      }).filter((o) => o.z1 - o.z0 > 0.1);
      faces.push({ kind: 'wall', depth, points, facing, openings });
      grow(points, depth);
    });
  });

  // Walls of one storey that stand in one plane and meet are one face: a wall cut by the wall that ends on it is drawn whole.
  for (let merged = true; merged;) {
    merged = false;
    for (let i = 0; i < faces.length && !merged; i++) for (let j = i + 1; j < faces.length && !merged; j++) {
      const a = faces[i], b = faces[j];
      if (a.kind !== 'wall' || b.kind !== 'wall' || Math.abs(a.depth - b.depth) > 0.02 || Math.abs(a.facing - b.facing) > 1e-6 || Math.abs(a.points[0][1] - b.points[0][1]) > 1e-9 || Math.abs(a.points[2][1] - b.points[2][1]) > 1e-9) continue;
      const ax = a.points.map((q) => q[0]), bx = b.points.map((q) => q[0]), a0 = Math.min(...ax), a1 = Math.max(...ax), b0 = Math.min(...bx), b1 = Math.max(...bx);
      if (b0 > a1 + 0.02 || a0 > b1 + 0.02) continue;
      const lo = Math.min(a0, b0), hi = Math.max(a1, b1), z0 = a.points[0][1], z1 = a.points[2][1];
      faces[i] = { ...a, points: [[lo, z0], [hi, z0], [hi, z1], [lo, z1]], openings: [...a.openings, ...b.openings] };
      faces.splice(j, 1);
      merged = true;
    }
  }

  const roofs = (reading && reading.shell && reading.shell.roofs) || [];
  for (const rf of roofs) {
    for (const piece of rf.pieces) {
      const q = piece.ring;
      if (q.length < 3) continue;
      let n = null;
      for (let i = 1; i + 1 < q.length && !n; i++) {
        const u = [q[i][0] - q[0][0], q[i][1] - q[0][1], q[i][2] - q[0][2]], w = [q[i + 1][0] - q[0][0], q[i + 1][1] - q[0][1], q[i + 1][2] - q[0][2]];
        const c = [u[1] * w[2] - u[2] * w[1], u[2] * w[0] - u[0] * w[2], u[0] * w[1] - u[1] * w[0]];
        if (Math.abs(c[2]) > 1e-9) n = c[2] > 0 ? c : [-c[0], -c[1], -c[2]];
      }
      if (!n) continue;
      const lean = -(n[0] * v[0] + n[1] * v[1]) / Math.hypot(n[0], n[1], n[2]);
      if (lean < 0.01) continue;
      const points = q.map((p) => [X(p), p[2]]), depth = q.reduce((sum, p) => sum + D(p), 0) / q.length;
      faces.push({ kind: 'roof', depth, points, facing: lean, edge: piece.edge, rise: Math.hypot(n[0], n[1]) / Math.hypot(n[0], n[1], n[2]) });
      grow(points, depth);
    }
    // The fascia, where the roof's edge faces the eye.
    const outline = rf.outline || [], turn = outline.length > 2 && ringArea(outline.map((p) => [p[0], p[1]])) < 0 ? -1 : 1;
    for (let i = 0; i < outline.length && rf.form !== 'flat'; i++) {
      const p = outline[i], q = outline[(i + 1) % outline.length], L = dist(p, q);
      if (!(L > 1e-6)) continue;
      const out = [turn * (q[1] - p[1]) / L, -turn * (q[0] - p[0]) / L];
      if (-(out[0] * v[0] + out[1] * v[1]) < 0.02) continue;
      const points = [[X(p), p[2]], [X(q), q[2]], [X(q), q[2] - 0.17], [X(p), p[2] - 0.17]];
      faces.push({ kind: 'fascia', depth: (D(p) + D(q)) / 2 - 0.02, points });
      grow(points, (D(p) + D(q)) / 2);
    }
    // The walls carried up to the roof.
    for (const w of rf.walls || []) {
      const L = dist(w.a, w.b);
      if (!(L > 0.01) || !w.top || w.top.length < 2) continue;
      const dir = [(w.b[0] - w.a[0]) / L, (w.b[1] - w.a[1]) / L], out = [dir[1], -dir[0]], facing = -(out[0] * v[0] + out[1] * v[1]);
      if (facing < 0.02) continue;
      const at = (s) => [w.a[0] + dir[0] * s, w.a[1] + dir[1] * s];
      const points = [[X(w.a), rf.plate], [X(w.b), rf.plate], ...w.top.slice().reverse().map(([s, z]) => [X(at(s)), Math.max(z, rf.plate)])];
      faces.push({ kind: 'wall', depth: (D(w.a) + D(w.b)) / 2, points, facing, openings: [], upper: true });
      grow(points, (D(w.a) + D(w.b)) / 2);
    }
    levels.push({ z: rf.plate, name: 'wall plate' }, { z: rf.top, name: rf.form === 'flat' ? 'parapet' : 'ridge' });
    top = Math.max(top, rf.top);
  }
  if (!roofs.length && top > 0) levels.push({ z: top, name: 'top' });
  faces.sort((p, q) => q.depth - p.depth);

  // What stands beside and behind: the neighbours as outlines, and the garden's trees.
  const behind = [], trees = [];
  if (Number.isFinite(x0)) {
    if (context) for (const nb of context.buildings || []) {
      if (!nb.ring || nb.ring.length < 3 || !(nb.height > 0)) continue;
      const xs = nb.ring.map(X), depth = nb.ring.reduce((sum, p) => sum + D(p), 0) / nb.ring.length, lo = Math.min(...xs), hi = Math.max(...xs);
      if (depth < near - 0.5 || hi < x0 - reach || lo > x1 + reach || depth > far + 4 * reach) continue;
      behind.push({ x0: lo, x1: hi, height: nb.height, depth, presumed: !!nb.presumed });
    }
    behind.sort((p, q) => q.depth - p.depth);
    for (const p of doc.furniture || []) {
      if (!p.outdoor || !['tree', 'tree-small', 'shrub'].includes(p.kind)) continue;
      const x = X(p.at);
      if (x < x0 - reach || x > x1 + reach) continue;
      trees.push({ x, radius: p.width / 2, height: p.height, before: D(p.at) < near, shrub: p.kind === 'shrub' });
    }
  }
  const seen = new Map();
  for (const l of levels) { const key = l.z.toFixed(2); if (!seen.has(key) || l.name === 'ridge') seen.set(key, l); }
  return { bearing, name: windOf(bearing), faces, behind, trees, x0, x1, top, levels: [...seen.values()].sort((p, q) => p.z - q.z), cladding: shell.cladding, cover: roof.cover, pitch: roof.pitch,
    empty: !Number.isFinite(x0) };
}

// An elevation as SVG within a box [x, y, w, h] of the page, at `scale` pixels to the metre (or the largest that fits). `id` keeps
// its patterns apart from another drawing's on the same page. Answers {svg, scale}.
export function elevationSvg(data, { box = [0, 0, 800, 500], scale = null, id = 'el', title = true, figure = true, levels = true } = {}) {
  const [bx, by, bw, bh] = box, margin = { left: levels ? 86 : 24, right: 30, top: 26, bottom: title ? 58 : 26 };
  if (data.empty) return { svg: `<text class="el-title" x="${bx + bw / 2}" y="${by + bh / 2}" text-anchor="middle">${esc(data.name)} elevation: nothing drawn yet</text>`, scale: 1 };
  const wide = data.x1 - data.x0 + 3.2, tall = data.top + 0.8;
  const k = scale || Math.min((bw - margin.left - margin.right) / wide, (bh - margin.top - margin.bottom) / tall);
  const ground = by + bh - margin.bottom, mid = bx + margin.left + (bw - margin.left - margin.right) / 2, cx = (data.x0 + data.x1) / 2;
  const sx = (x) => mid + (x - cx) * k, sy = (z) => ground - z * k, pt = ([x, z]) => `${sx(x).toFixed(1)},${sy(z).toFixed(1)}`;
  const left = bx + margin.left - 14, right = bx + bw - margin.right + 6;
  const out = [`<defs>`];
  // Courses: lines level with the ground, so that every face's courses meet at the corners.
  const lines = (name, step, cls) => {
    let px = step * k, every = 1;
    while (px * every < 3.2) every++;
    px *= every;
    out.push(`<pattern id="${id}-${name}" patternUnits="userSpaceOnUse" x="0" y="${(ground % px).toFixed(2)}" width="40" height="${px.toFixed(2)}"><line class="${cls}" x1="0" y1="0.5" x2="40" y2="0.5"/></pattern>`);
  };
  const course = CLADDINGS[data.cladding] ? CLADDINGS[data.cladding].course : [0, 0], cover = ROOF_COVERS[data.cover] ? ROOF_COVERS[data.cover].course : [0, 0];
  if (course[1] > 0) lines('clad', course[1], 'el-course');
  out.push(`</defs>`);
  // Beside and behind.
  for (const nb of data.behind) out.push(`<rect class="el-behind${nb.presumed ? ' presumed' : ''}" x="${sx(nb.x0).toFixed(1)}" y="${sy(nb.height).toFixed(1)}" width="${((nb.x1 - nb.x0) * k).toFixed(1)}" height="${(nb.height * k).toFixed(1)}"/>`);
  const tree = (t) => {
    const crown = t.shrub ? t.height * 0.55 : Math.max(t.radius, (t.height - Math.max(1.2, t.height * 0.36)) / 2), cy = t.height - crown;
    return `<g class="el-tree${t.before ? ' before' : ''}">${t.shrub ? '' : `<line x1="${sx(t.x).toFixed(1)}" y1="${sy(0).toFixed(1)}" x2="${sx(t.x).toFixed(1)}" y2="${sy(cy).toFixed(1)}"/>`}<ellipse cx="${sx(t.x).toFixed(1)}" cy="${sy(cy).toFixed(1)}" rx="${(t.radius * k).toFixed(1)}" ry="${(crown * k).toFixed(1)}"/></g>`;
  };
  for (const t of data.trees) if (!t.before) out.push(tree(t));
  // The faces, from far to near.
  for (const f of data.faces) {
    const d = f.points.map(pt).join(' ');
    if (f.kind === 'wall') {
      out.push(`<polygon class="el-wall" points="${d}"/>`);
      if (f.facing < 0.999) out.push(`<polygon class="el-turned" points="${d}" style="fill-opacity:${(0.1 * (1 - f.facing)).toFixed(3)}"/>`);
      if (course[1] > 0) out.push(`<polygon class="el-hatch" points="${d}" fill="url(#${id}-clad)"/>`);
      if (course[0] > 0 && course[1] === 0) {
        // Battens: lines that stand, closer as the wall turns away.
        const xs = f.points.map((p) => p[0]), lo = Math.min(...xs), hi = Math.max(...xs);
        let step = course[0] * f.facing;
        while (step * k < 3.2) step *= 2;
        const clip = `${id}-c${out.length}`;
        out.push(`<clipPath id="${clip}"><polygon points="${d}"/></clipPath><g clip-path="url(#${clip})">`);
        for (let x = lo + step; x < hi; x += step) out.push(`<line class="el-course" x1="${sx(x).toFixed(1)}" y1="${sy(0).toFixed(1)}" x2="${sx(x).toFixed(1)}" y2="${sy(data.top).toFixed(1)}"/>`);
        out.push(`</g>`);
      }
      for (const o of f.openings) {
        const x = sx(o.x0), y = sy(o.z1), w = (o.x1 - o.x0) * k, h = (o.z1 - o.z0) * k;
        if (w < 0.6) continue;
        if (o.kind === 'window') {
          out.push(`<rect class="el-glass" x="${x.toFixed(1)}" y="${y.toFixed(1)}" width="${w.toFixed(1)}" height="${h.toFixed(1)}"/>`);
          for (let j = 1; j < o.lights; j++) out.push(`<line class="el-frame" x1="${(x + j * w / o.lights).toFixed(1)}" y1="${y.toFixed(1)}" x2="${(x + j * w / o.lights).toFixed(1)}" y2="${(y + h).toFixed(1)}"/>`);
          out.push(`<rect class="el-frame" x="${x.toFixed(1)}" y="${y.toFixed(1)}" width="${w.toFixed(1)}" height="${h.toFixed(1)}"/>`);
          out.push(`<line class="el-sill" x1="${(x - 0.05 * k).toFixed(1)}" y1="${(y + h).toFixed(1)}" x2="${(x + w + 0.05 * k).toFixed(1)}" y2="${(y + h).toFixed(1)}"/>`);
        } else {
          out.push(`<rect class="el-door" x="${x.toFixed(1)}" y="${y.toFixed(1)}" width="${w.toFixed(1)}" height="${h.toFixed(1)}"/>`);
          if (o.glazed > 0) out.push(`<rect class="el-glass" x="${(x + w * 0.2).toFixed(1)}" y="${(y + h * 0.1).toFixed(1)}" width="${(w * 0.6).toFixed(1)}" height="${(h * 0.5 * Math.min(1, o.glazed + 0.2)).toFixed(1)}"/>`);
          out.push(`<circle class="el-knob" cx="${(x + w * 0.84).toFixed(1)}" cy="${(y + h * 0.52).toFixed(1)}" r="${Math.max(1, 0.03 * k).toFixed(1)}"/>`);
        }
      }
    } else if (f.kind === 'roof') {
      out.push(`<polygon class="el-roof" points="${d}"/>`);
      if (cover[1] > 0 && f.rise > 0.02) {
        // The cover's courses: level lines, as far apart as a course rises.
        let step = cover[1] * f.rise;
        while (step * k < 3.2) step *= 2;
        const zs = f.points.map((p) => p[1]), lo = Math.min(...zs), hi = Math.max(...zs), xs = f.points.map((p) => p[0]), clip = `${id}-c${out.length}`;
        out.push(`<clipPath id="${clip}"><polygon points="${d}"/></clipPath><g clip-path="url(#${clip})">`);
        for (let z = lo + step; z < hi; z += step) out.push(`<line class="el-course roof" x1="${sx(Math.min(...xs)).toFixed(1)}" y1="${sy(z).toFixed(1)}" x2="${sx(Math.max(...xs)).toFixed(1)}" y2="${sy(z).toFixed(1)}"/>`);
        out.push(`</g>`);
      }
      out.push(`<polygon class="el-roof-line" points="${d}"/>`);
    } else out.push(`<polygon class="el-fascia" points="${d}"/>`);
  }
  for (const t of data.trees) if (t.before) out.push(tree(t));
  // The ground, the levels, a person, the name.
  out.push(`<line class="el-ground" x1="${(bx + 8).toFixed(1)}" y1="${ground.toFixed(1)}" x2="${(bx + bw - 8).toFixed(1)}" y2="${ground.toFixed(1)}"/>`);
  if (levels) {
    let last = Infinity;
    for (const l of data.levels) {
      const y = sy(l.z);
      out.push(`<line class="el-level" x1="${left.toFixed(1)}" y1="${y.toFixed(1)}" x2="${right.toFixed(1)}" y2="${y.toFixed(1)}"/>`);
      if (last - y < 11) continue;
      last = y;
      out.push(`<text class="el-mark" x="${(left - 4).toFixed(1)}" y="${(y - 3).toFixed(1)}" text-anchor="end">${level(l.z)}</text><text class="el-note" x="${(left - 4).toFixed(1)}" y="${(y + 9).toFixed(1)}" text-anchor="end">${esc(l.name)}</text>`);
    }
  }
  if (figure) {
    // A person of 1.7 m beside the building.
    const fx = sx(data.x1) + 1.0 * k, h = 1.7 * k, head = 0.11 * k;
    out.push(`<g class="el-figure"><circle cx="${fx.toFixed(1)}" cy="${(ground - h + head).toFixed(1)}" r="${head.toFixed(1)}"/><path d="M ${(fx - 0.2 * k).toFixed(1)} ${(ground - h + 2.5 * head + 0.02 * k).toFixed(1)} Q ${fx.toFixed(1)} ${(ground - h + 2.1 * head).toFixed(1)} ${(fx + 0.2 * k).toFixed(1)} ${(ground - h + 2.5 * head + 0.02 * k).toFixed(1)} L ${(fx + 0.1 * k).toFixed(1)} ${ground.toFixed(1)} L ${(fx - 0.1 * k).toFixed(1)} ${ground.toFixed(1)} Z"/></g>`);
  }
  if (title) {
    const bar = [1, 2, 5, 10].find((m) => m * k >= 60) || 20, tx = bx + margin.left;
    out.push(`<text class="el-title" x="${tx.toFixed(1)}" y="${(ground + 30).toFixed(1)}">${esc(data.name.charAt(0).toUpperCase() + data.name.slice(1))} elevation</text>`);
    out.push(`<text class="el-note" x="${tx.toFixed(1)}" y="${(ground + 45).toFixed(1)}">${f1(data.x1 - data.x0)} m wide, ${f1(data.top)} m high</text>`);
    const ex = bx + bw - margin.right - bar * k;
    out.push(`<path class="el-scale" d="M ${ex.toFixed(1)} ${(ground + 24).toFixed(1)} v 5 h ${(bar * k).toFixed(1)} v -5"/><text class="el-note" x="${(ex + bar * k / 2).toFixed(1)}" y="${(ground + 43).toFixed(1)}" text-anchor="middle">${bar} m</text>`);
  }
  return { svg: out.join(''), scale: k };
}

// The four elevations on one sheet of [w, h] pixels, two by two, all at one scale: the least of what each would take.
export function elevationSheet(doc, reading, { size = [1000, 700], context = null, gap = 12 } = {}) {
  const sides = elevationBearings(doc, reading).map((s) => elevationData(doc, reading, s.bearing, { context }));
  const w = (size[0] - gap) / 2, h = (size[1] - gap) / 2, boxes = sides.map((_, i) => [(i % 2) * (w + gap), Math.floor(i / 2) * (h + gap), w, h]);
  const scale = Math.min(...sides.map((d, i) => elevationSvg(d, { box: boxes[i], id: `e${i}` }).scale));
  return { sides, scale, svg: sides.map((d, i) => `<g class="el-side" data-side="${i}">${elevationSvg(d, { box: boxes[i], scale: d.empty ? null : scale, id: `e${i}` }).svg}</g>`).join('') };
}
