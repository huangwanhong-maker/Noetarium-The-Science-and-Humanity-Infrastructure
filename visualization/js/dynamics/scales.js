// Systems across scales drawn (ADR 0128), as markup that needs no page.
//
// modelDiagram(described)  a model as its levels, the coarsest at the top: each level a band with its quantities, a glyph by role
//                          (a state a filled disc, a driver a triangle, a fixed value a square, a derived quantity a diamond), the
//                          laws of the states listed beneath; what carries a quantity between levels drawn as an arrow between the bands (a sum
//                          or a reduction up, a parent's value down), what works among siblings or neighbours marked within its band;
//                          and what the model still needs ringed and said to be declared.
// unitsMap(units)          units drawn on a plane from their outlines in longitude and latitude, toned by a value, a unit without one
//                          hatched; the units of each group (the area above) carrying its name at their middle.
//
// The words are the model's own: names, operators and expressions as the API's science/scales describes them.

const esc = (s) => String(s ?? '').replace(/[&<>"]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' })[c]);
const UP = new Set(['sum', 'mean', 'wmean', 'minimum', 'maximum', 'reduce']);
const AMONG = new Set(['share', 'logit', 'zscore']);
const NEAR = new Set(['laplacian', 'neighbours', 'inflow']);

function glyph(role, x, y, r = 6) {
  if (role === 'state') return `<circle class="ms-g ms-state" cx="${x}" cy="${y}" r="${r}"/>`;
  if (role === 'driver') return `<path class="ms-g ms-driver" d="M${x - r},${y - r} L${x + r},${y} L${x - r},${y + r} Z"/>`;
  if (role === 'fixed') return `<rect class="ms-g ms-fixed" x="${x - r * 0.85}" y="${y - r * 0.85}" width="${r * 1.7}" height="${r * 1.7}"/>`;
  return `<path class="ms-g ms-derived" d="M${x},${y - r} L${x + r},${y} L${x},${y + r} L${x - r},${y} Z"/>`;
}

// described: {levels: [{name, units}], quantities: [{name, level, role, form?, expression?}], derived: [{name, level, op, of, reads}],
//             missing: [{quantity, level, what}]}
export function modelDiagram(described, { width = 720, band = 120, what = 'a model across scales' } = {}) {
  const levels = [...(described.levels || [])].reverse();  // the coarsest at the top
  const at = {};
  const items = [];
  levels.forEach((lv, k) => {
    const mine = [
      ...(described.quantities || []).filter((q) => q.level === lv.name).map((q) => ({ ...q, kind: 'quantity' })),
      ...(described.derived || []).filter((d) => d.level === lv.name).map((d) => ({ ...d, role: 'derived', kind: 'derived' })),
    ];
    const gap = (width - 150) / Math.max(1, mine.length);
    mine.forEach((m, i) => {
      const x = 150 + gap * (i + 0.5), y = k * band + band * 0.42;
      at[m.name] = { x, y, level: k };
      items.push({ ...m, x, y });
    });
  });
  const need = new Set((described.missing || []).map((m) => m.quantity));
  const laws = (described.quantities || []).filter((q) => q.form);
  const height = levels.length * band + 14 + laws.length * 18;
  const parts = [];
  levels.forEach((lv, k) => {
    parts.push(`<rect class="ms-band${k % 2 ? ' ms-odd' : ''}" x="0" y="${k * band}" width="${width}" height="${band}"/>`);
    parts.push(`<text class="ms-level" x="12" y="${k * band + 22}">${esc(lv.name)}</text>`);
    parts.push(`<text class="ms-count" x="12" y="${k * band + 40}">${esc(lv.units)} unit${lv.units === 1 ? '' : 's'}</text>`);
  });
  const arrows = [];
  for (const d of described.derived || []) {
    const to = at[d.name];
    if (!to) continue;
    const from = (d.of || '').match(/[A-Za-z_][A-Za-z0-9_]*/g)?.map((n) => at[n]).filter((p) => p && p.level !== to.level) || [];
    for (const f of from.slice(0, 3)) {
      const cls = UP.has(d.op) ? 'ms-up' : d.op === 'parent' ? 'ms-down' : 'ms-across';
      arrows.push(`<path class="ms-arrow ${cls}" d="M${f.x},${f.y + (f.y < to.y ? 8 : -8)} C${f.x},${(f.y + to.y) / 2} ${to.x},${(f.y + to.y) / 2} ${to.x},${to.y + (f.y < to.y ? -9 : 9)}" marker-end="url(#ms-head)"><title>${esc(d.op)} of ${esc(d.of)}</title></path>`);
    }
  }
  for (const m of items) {
    const ring = need.has(m.name) ? `<circle class="ms-need" cx="${m.x}" cy="${m.y}" r="11"><title>to be declared: ${esc(m.name)}</title></circle>` : '';
    const op = m.kind === 'derived' ? `<text class="ms-op${AMONG.has(m.op) ? ' ms-among' : NEAR.has(m.op) ? ' ms-near' : ''}" x="${m.x}" y="${m.y + 34}">${esc(m.op)}</text>` : '';
    const law = m.form ? `<text class="ms-op" x="${m.x}" y="${m.y + 34}">${esc(m.form)}</text>` : '';
    parts.push(`<g class="ms-item" data-name="${esc(m.name)}">${ring}${glyph(m.role, m.x, m.y)}<text class="ms-name" x="${m.x}" y="${m.y + 20}">${esc(m.name)}</text>${op}${law}</g>`);
  }
  laws.forEach((q, i) => {
    const sign = q.form === 'map' ? ' ← ' : q.form === 'flow' ? '′ = ' : ' = ';
    parts.push(`<text class="ms-law" x="12" y="${levels.length * band + 22 + i * 18}">${esc(q.name)}${sign}${esc(q.expression || '')}${q.form === 'equilibrium' ? '   (at rest)' : ''}</text>`);
  });
  return `<svg class="ms-diagram" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" role="img" aria-label="${esc(what)}">`
    + '<defs><marker id="ms-head" viewBox="0 0 8 8" refX="7" refY="4" markerWidth="7" markerHeight="7" orient="auto-start-reverse"><path d="M0,0 L8,4 L0,8 Z" class="ms-head"/></marker>'
    + '<pattern id="ms-hatch" width="5" height="5" patternUnits="userSpaceOnUse" patternTransform="rotate(45)"><line x1="0" y1="0" x2="0" y2="5" class="ms-hatch"/></pattern></defs>'
    + parts.slice(0, levels.length * 3).join('') + arrows.join('') + parts.slice(levels.length * 3).join('') + '</svg>';
}

// units: {rings: {id: [[lon, lat], ...]}, ids: [ids], colours: [css per id], groups?: [name per id], width?, what?}
export function unitsMap({ rings, ids, colours, groups = null, titles = null, width = 640, what = 'units on a plane' }) {
  let w = Infinity, e = -Infinity, s = Infinity, n = -Infinity;
  for (const id of ids) for (const [x, y] of rings[id] || []) { w = Math.min(w, x); e = Math.max(e, x); s = Math.min(s, y); n = Math.max(n, y); }
  if (!Number.isFinite(w)) return `<svg class="ms-map" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} 10" role="img" aria-label="${esc(what)}"></svg>`;
  const k = Math.cos(((s + n) / 2) * Math.PI / 180);
  const sx = (width - 8) / Math.max((e - w) * k, 1e-12);
  const height = Math.round((n - s) * sx + 8);
  const px = (x) => 4 + (x - w) * k * sx, py = (y) => 4 + (n - y) * sx;
  const paths = [], middles = {};
  ids.forEach((id, i) => {
    const ring = rings[id];
    if (!ring || ring.length < 3) return;
    const d = ring.map(([x, y], j) => `${j ? 'L' : 'M'}${px(x).toFixed(1)},${py(y).toFixed(1)}`).join('') + 'Z';
    const fill = colours[i] && colours[i] !== '#00000000' ? colours[i] : 'url(#ms-hatch)';
    paths.push(`<path class="ms-unit" data-id="${esc(id)}" d="${d}" style="fill:${fill}"><title>${esc(titles ? titles[i] : id)}</title></path>`);
    if (groups) {
      const g = (middles[groups[i]] ||= { x: 0, y: 0, n: 0 });
      g.x += px(ring[0][0]), g.y += py(ring[0][1]), g.n += 1;
    }
  });
  const names = groups ? Object.entries(middles).map(([g, m]) => `<text class="ms-group" x="${(m.x / m.n).toFixed(1)}" y="${(m.y / m.n).toFixed(1)}">${esc(g)}</text>`) : [];
  return `<svg class="ms-map" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" role="img" aria-label="${esc(what)}">`
    + '<defs><pattern id="ms-hatch" width="5" height="5" patternUnits="userSpaceOnUse" patternTransform="rotate(45)"><line x1="0" y1="0" x2="0" y2="5" class="ms-hatch"/></pattern></defs>'
    + paths.join('') + names.join('') + '</svg>';
}
