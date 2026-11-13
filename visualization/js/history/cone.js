// The cone of a historical object (ADR 0110): what its appearing rested on, to the left, and what rests on it, to the
// right, with the object between them.
//
// To the left the premises stand in columns by the number of steps from the object; the first column is sorted by
// the role each premise takes (the problem answered, the conditions, the sources, what is argued against), and each
// group is named. A premise inherited from what the object specialises or exemplifies is drawn fainter, with the
// name of the object it comes through, and never as the object's own. Above the object stand the objects it stands
// under. A link is drawn as it stands: established a full line, attested dashed, proposed dotted; the standing is
// also a word in the slip's title, so that it is never told by the line alone.
//
// layoutCone is pure arithmetic; svgCone returns markup. Nothing here needs a page (history.css gives the marks a
// default look).

import { MAPS } from '../../styles/palettes.js';

export const HUES = MAPS['categorical-apart'].stops;
// The roles of a premise, each with its words and a hue of the validated ten (always beside its words).
export const ROLES = {
  problem: { name: 'the problem it answers', short: 'problem', hue: HUES[4] },
  condition: { name: 'what made it possible', short: 'condition', hue: HUES[6] },
  source: { name: 'what it drew on', short: 'source', hue: HUES[3] },
  against: { name: 'what it argued against', short: 'against', hue: HUES[2] },
};
export const ROLE_ORDER = ['problem', 'condition', 'source', 'against'];

// A glyph for each kind of the core; an object takes the glyph of its own kind or of the nearest above it.
export const GLYPHS = {
  Object: '·', Event: '◆', Period: '▬', Place: '◎', Agent: '●', Person: '●', Collective: '◐', Institution: '◐', Polity: '◐', School: '◐',
  Work: '■', Artifact: '◇', Idea: '○', Concept: '○', Theory: '△', Problem: '?', Tradition: '□', Genre: '□', Technology: '▣', Practice: '◌',
};
export function glyphOf(object) {
  for (const k of [object.kind, ...(object.above || [])]) if (GLYPHS[k]) return GLYPHS[k];
  return GLYPHS.Object;
}

// A year as it is read: "433 BC", "1584", with "about" when the date is approximate.
export function yearText(year, approximate = false) {
  if (year === null || year === undefined || Number.isNaN(year)) return '';
  const y = Math.floor(year);
  return `${approximate ? 'about ' : ''}${y <= 0 ? `${1 - y} BC` : `${y}`}`;
}

export const esc = (s) => String(s ?? '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
// The width a text takes, near enough for a layout: Latin letters at about half their size, wide scripts at their size.
export function textWidth(text, size = 13) {
  let w = 0;
  for (const ch of String(text ?? '')) {
    const c = ch.codePointAt(0);
    w += c > 0x2e80 ? size : c > 0x0530 ? size * 0.62 : /[iljtf.,;:'| ]/.test(ch) ? size * 0.3 : /[mwMW]/.test(ch) ? size * 0.82 : size * 0.52;
  }
  return w;
}
function clip(text, width, size = 13) {
  if (textWidth(text, size) <= width) return String(text);
  let out = '';
  for (const ch of String(text)) {
    if (textWidth(`${out}${ch}…`, size) > width) break;
    out += ch;
  }
  return `${out.trimEnd()}…`;
}

const SLIP = { w: 196, h: 40, gap: 9, column: 250, head: 22 };

// Where everything stands. view: { center, past: [entry], future: [entry], links: [{what, product, role, standing}],
// lineage: [object] }, an entry an object with { steps, role, inherited }. options.depth: the columns kept a side (3).
export function layoutCone(view, options = {}) {
  const depth = options.depth || 3;
  const column = (steps) => Math.min(steps, depth);
  const sideColumns = (entries, past) => {
    const columns = Array.from({ length: depth }, () => []);
    for (const e of entries) columns[column(e.steps) - 1].push(e);
    if (past) {
      const rank = (e) => (e.inherited ? 10 : 0) + Math.max(0, ROLE_ORDER.indexOf(e.role));
      columns[0].sort((a, b) => rank(a) - rank(b) || (a.year ?? 1e9) - (b.year ?? 1e9));
    }
    for (const c of columns.slice(past ? 1 : 0)) c.sort((a, b) => (a.year ?? 1e9) - (b.year ?? 1e9));
    return columns.filter((c, i) => i === 0 || c.length);
  };
  const left = sideColumns(view.past || [], true), right = sideColumns(view.future || [], false);
  const used = (cols) => cols.filter((c) => c.length).length;
  const nl = used(left) || 1, nr = used(right) || 1;
  const pad = 26, top = 30 + (view.lineage?.length ? 34 : 0);
  // Heights: the first column of the past carries a heading before each group.
  const groups = [];
  let y = 0, last = null;
  const firstRows = left[0].map((e) => {
    const key = e.inherited ? `inherited:${e.inherited}` : e.role;
    if (key !== last) { groups.push({ key, role: e.role, inherited: e.inherited || null, y }); y += SLIP.head; last = key; }
    const row = { e, y };
    y += SLIP.h + SLIP.gap;
    return row;
  });
  const heightOf = (c) => c.length * (SLIP.h + SLIP.gap);
  const tallest = Math.max(y, ...left.slice(1).map(heightOf), ...right.map(heightOf), SLIP.h + 30);
  const width = pad * 2 + (nl + nr) * SLIP.column + SLIP.w + 20, height = top + tallest + pad;
  const cx = pad + nl * SLIP.column, cy = top + tallest / 2 - SLIP.h / 2;
  const nodes = [{ ...view.center, x: cx, y: cy, w: SLIP.w + 20, h: SLIP.h + 8, center: true }];
  firstRows.forEach(({ e, y: ry }) => nodes.push({ ...e, x: cx - SLIP.column, y: top + (tallest - y) / 2 + ry, w: SLIP.w, h: SLIP.h, past: true }));
  left.slice(1).forEach((c, i) => c.forEach((e, k) => nodes.push({ ...e, x: cx - SLIP.column * (i + 2), y: top + (tallest - heightOf(c)) / 2 + k * (SLIP.h + SLIP.gap), w: SLIP.w, h: SLIP.h, past: true })));
  right.forEach((c, i) => c.forEach((e, k) => nodes.push({ ...e, x: cx + SLIP.w + 20 + SLIP.column * (i + 1) - SLIP.w, y: top + (tallest - heightOf(c)) / 2 + k * (SLIP.h + SLIP.gap), w: SLIP.w, h: SLIP.h, past: false })));
  const at = new Map(nodes.map((n) => [n.id, n]));
  const edges = [];
  for (const l of view.links || []) {
    const a = at.get(l.what), b = at.get(l.product);
    if (a && b && a !== b) edges.push({ ...l, a, b });
  }
  // A premise inherited through an ancestor that is not drawn is tied to the object itself, as inherited.
  for (const n of nodes) {
    if (!n.inherited || n.steps !== 1) continue;
    if (!edges.some((e) => e.a === n)) edges.push({ what: n.id, product: view.center.id, role: n.role, standing: n.standing, inherited: true, a: n, b: nodes[0] });
  }
  const headings = groups.map((g) => ({ ...g, x: cx - SLIP.column, y: top + (tallest - y) / 2 + g.y + 14 }));
  return { width, height, nodes, edges, headings, top, cx, cy };
}

function slip(n) {
  const cls = ['h-slip', n.center ? 'center' : '', n.inherited ? 'inherited' : '', n.standing ? `s-${n.standing}` : '', n.role ? `r-${n.role}` : ''].filter(Boolean).join(' ');
  const hue = !n.center && n.role && ROLES[n.role] ? ROLES[n.role].hue : null;
  const year = yearText(n.year, n.approximate), size = n.center ? 15 : 13;
  const name = clip(n.name, n.w - 40, size);
  const title = `${n.name}${year ? `, ${year}` : ''}${n.kindName ? ` · ${n.kindName}` : ''}${n.role && ROLES[n.role] ? ` · ${ROLES[n.role].short}` : ''}${n.standing ? ` · ${n.standing}` : ''}`;
  return `<g class="${cls}" data-object="${esc(n.id)}" transform="translate(${n.x.toFixed(1)} ${n.y.toFixed(1)})"><title>${esc(title)}</title>` +
    `<rect width="${n.w}" height="${n.h}" rx="2"/>` + (hue ? `<rect class="h-role" width="4" height="${n.h}" fill="${hue}"/>` : '') +
    `<text class="h-glyph" x="16" y="${(n.h / 2 + 5).toFixed(1)}" text-anchor="middle">${esc(glyphOf(n))}</text>` +
    `<text class="h-name" x="30" y="${(year || n.kindName ? n.h / 2 - 2 : n.h / 2 + 5).toFixed(1)}" font-size="${size}">${esc(name)}</text>` +
    (year || n.kindName ? `<text class="h-sub" x="30" y="${(n.h / 2 + 13).toFixed(1)}">${esc([n.kindName, year].filter(Boolean).join(' · '))}</text>` : '') + '</g>';
}

// The cone as SVG markup. options.depth as layoutCone's; options.names: id → name, for what a premise is inherited through.
export function svgCone(view, options = {}) {
  const L = layoutCone(view, options);
  const parts = [`<svg xmlns="http://www.w3.org/2000/svg" class="h-cone" viewBox="0 0 ${L.width.toFixed(0)} ${L.height.toFixed(0)}" width="${L.width.toFixed(0)}" height="${L.height.toFixed(0)}" role="img" aria-label="${esc(`The cone of ${view.center.name}`)}">`];
  for (const e of L.edges) {
    const x1 = e.a.x + e.a.w, y1 = e.a.y + e.a.h / 2, x2 = e.b.x, y2 = e.b.y + e.b.h / 2, bend = Math.max(30, (x2 - x1) * 0.45);
    const hue = ROLES[e.role]?.hue;
    parts.push(`<path class="h-link s-${esc(e.standing || 'proposed')}${e.inherited ? ' inherited' : ''}" data-role="${esc(e.role || '')}"${hue ? ` style="stroke:${hue}"` : ''} d="M${x1.toFixed(1)} ${y1.toFixed(1)}C${(x1 + bend).toFixed(1)} ${y1.toFixed(1)} ${(x2 - bend).toFixed(1)} ${y2.toFixed(1)} ${x2.toFixed(1)} ${y2.toFixed(1)}"/>`);
  }
  for (const h of L.headings) {
    const through = h.inherited ? `inherited through ${options.names?.[h.inherited] || h.inherited}` : ROLES[h.role]?.name || h.role;
    parts.push(`<text class="h-heading${h.inherited ? ' inherited' : ''}" x="${h.x.toFixed(1)}" y="${h.y.toFixed(1)}">${esc(through)}</text>`);
  }
  if (view.lineage?.length) {
    const text = `stands under ${view.lineage.map((a) => a.name).join(' · ')}`;
    parts.push(`<text class="h-lineage" x="${(L.cx + (196 + 20) / 2).toFixed(1)}" y="${(L.cy - 14).toFixed(1)}" text-anchor="middle">${esc(clip(text, 420, 12))}</text>`);
  }
  const center = L.nodes[0];
  if (!(view.past || []).length) parts.push(`<text class="h-silent" x="${(center.x - 24).toFixed(1)}" y="${(center.y + center.h / 2 + 4).toFixed(1)}" text-anchor="end">the record names nothing it rested on</text>`);
  if (!(view.future || []).length) parts.push(`<text class="h-silent" x="${(center.x + center.w + 24).toFixed(1)}" y="${(center.y + center.h / 2 + 4).toFixed(1)}">the record names nothing that rests on it</text>`);
  for (const n of L.nodes.slice(1)) parts.push(slip(n));
  parts.push(slip(center), '</svg>');
  return parts.join('');
}
