// The model of a board (schema noetarium.board/1): what is thought, and apart from it how it lies.
//
// A board is (V, R, β; λ):
//   V  items, each of a kind with its content                                  board.items[id]
//   R  typed relations between items, themselves items of the kind `link`      {kind: 'link', from, to, rel}
//      and membership of an item in a region                                   item.in
//   β  bindings of an item to things outside the board                         item.when, item.source …
//      and data set on the board: tables (the kind `table`), and what a card's module answered (item.out)
//   λ  the placement: a rectangle for every item but the links, and an order   board.place[id], board.order
//      and layers, back to front, above the ground: an item says its layer     board.layers, item.layer
//      (a layer can be hidden, held or made faint, with all that lies on it)
//
// Content never refers to placement, so the same content can be laid out again by an arrangement
// (arrange.js). Everything here is pure, and a board is plain JSON.

export const SCHEMA = 'noetarium.board/1';

export const KINDS = ['note', 'text', 'image', 'region', 'shape', 'link', 'schedule', 'figure', 'ink', 'table', 'card'];

// The relations a link may state. `parent` says which end is the parent when links are read as a tree.
export const RELATIONS = {
  arrow: { label: '', parent: 'from' },
  supports: { label: 'supports', parent: 'to' },
  contradicts: { label: 'contradicts', parent: 'to' },
  causes: { label: 'leads to', parent: 'from' },
  'part-of': { label: 'part of', parent: 'to' },
  follows: { label: 'then', parent: 'from' },
};

// The kinds of note every board has. A plug-in may add its own (a competitor, a segment, a quotation).
// A tone names a pair of colours of the page's style sheet; the glyph and the label say the kind without colour.
export const NOTE_TYPES = [
  { id: 'idea', label: 'Idea', glyph: '✦', tone: 'apricot' },
  { id: 'fact', label: 'Fact', glyph: '●', tone: 'sage', about: 'Something known, with its source.' },
  { id: 'assumption', label: 'Assumption', glyph: '≈', tone: 'lilac', about: 'Something taken to be so. It may carry a quantity with its range.' },
  { id: 'question', label: 'Question', glyph: '?', tone: 'sky' },
  { id: 'insight', label: 'Insight', glyph: '❋', tone: 'rose' },
  { id: 'decision', label: 'Decision', glyph: '◆', tone: 'gold' },
  { id: 'risk', label: 'Risk', glyph: '▲', tone: 'clay' },
  { id: 'plain', label: 'Note', glyph: '·', tone: 'paper' },
];

export const TONES = ['apricot', 'sage', 'lilac', 'sky', 'rose', 'gold', 'clay', 'paper'];

export const DEFAULT_SIZE = {
  note: { w: 200, h: 132 }, text: { w: 320, h: 44 }, image: { w: 320, h: 220 }, region: { w: 520, h: 360 },
  shape: { w: 168, h: 92 }, schedule: { w: 780, h: 260 }, figure: { w: 290, h: 150 }, card: { w: 460, h: 340 },
  ink: { w: 40, h: 40 }, table: { w: 440, h: 200 },
};

export const MIN_SIZE = { w: 48, h: 32 };

// An identifier that two boards made apart will not share, so that boards can later be merged.
export function makeId(random = Math.random) {
  let s = '';
  while (s.length < 9) s += Math.floor(random() * 36).toString(36);
  return `i${s}`;
}

export function emptyBoard({ id = makeId(), title = 'Untitled board', now = null } = {}) {
  return { schema: SCHEMA, id, title, items: {}, place: {}, order: [], meta: now ? { created: now, modified: now } : {} };
}

// A new item's content, with the defaults of its kind.
export function newItem(kind, props = {}) {
  const base = {
    note: { kind, type: 'idea', text: '' },
    text: { kind, text: '', size: 'body' },
    image: { kind, src: '', caption: '' },
    region: { kind, title: '' },
    shape: { kind, form: 'rect', text: '' },
    link: { kind, from: null, to: null, rel: 'arrow' },
    schedule: { kind, title: 'Plan', mode: 'plan', source: '' },
    figure: { kind, title: '', formula: '' },
    // A stroke of the pen: points [x, y, pressure] within its rectangle, as drawn at the size `size`.
    ink: { kind, points: [], size: [1, 1], colour: 'ink', width: 3 },
    table: { kind, name: 'table', title: '', source: '', rows: 0, columns: [], data: {} },
    card: { kind, card: '', props: {} },
  }[kind];
  if (!base) throw new Error(`a board has no items of the kind '${kind}'`);
  return { ...base, ...props, kind };
}

const finite = (v) => typeof v === 'number' && Number.isFinite(v);

// Everything wrong with a board, in words; empty when it is sound.
export function checkBoard(board) {
  const problems = [];
  if (!board || typeof board !== 'object') return ['a board is a JSON object'];
  if (board.schema !== SCHEMA) problems.push(`its schema is '${board.schema}', not '${SCHEMA}'`);
  const { items = {}, place = {}, order = [] } = board;
  const seen = new Set();
  for (const id of order) {
    if (seen.has(id)) problems.push(`'${id}' is twice in the order`);
    seen.add(id);
    if (!items[id]) problems.push(`'${id}' is in the order and is not an item`);
  }
  for (const [id, item] of Object.entries(items)) {
    if (!seen.has(id)) problems.push(`'${id}' is not in the order`);
    if (!KINDS.includes(item.kind)) { problems.push(`'${id}' is of an unknown kind '${item.kind}'`); continue; }
    if (item.kind === 'link') {
      if (place[id]) problems.push(`the link '${id}' has a place`);
      for (const end of ['from', 'to']) {
        const other = items[item[end]];
        if (!other) problems.push(`the link '${id}' has no item at its end '${end}'`);
        else if (other.kind === 'link') problems.push(`the link '${id}' ends on a link`);
      }
      if (item.from === item.to) problems.push(`the link '${id}' joins an item to itself`);
      if (!RELATIONS[item.rel]) problems.push(`the link '${id}' states an unknown relation '${item.rel}'`);
    } else {
      const p = place[id];
      if (!p || ![p.x, p.y, p.w, p.h].every(finite) || p.w <= 0 || p.h <= 0) problems.push(`'${id}' has no rectangle to lie in`);
    }
    if (item.in !== undefined) {
      if (item.kind === 'link') problems.push(`the link '${id}' is in a region`);
      else if (items[item.in]?.kind !== 'region') problems.push(`'${id}' is in '${item.in}', which is not a region`);
      else if (item.in === id) problems.push(`'${id}' is in itself`);
    }
  }
  for (const id of Object.keys(place)) if (!items[id]) problems.push(`a place is kept for '${id}', which is not an item`);
  const layers = board.layers || [];
  if (!Array.isArray(layers) || new Set(layers.map((l) => l?.id)).size !== layers.length || layers.some((l) => !l || !l.id || typeof l.name !== 'string')) problems.push('its layers are not a list of layers, each with an id of its own and a name');
  else for (const [id, item] of Object.entries(items)) if (item.layer !== undefined && !layers.some((l) => l.id === item.layer)) problems.push(`'${id}' lies on the layer '${item.layer}', which the board has not`);
  return problems;
}

// ---- queries ----------------------------------------------------------------------------------------

export const isLink = (item) => item?.kind === 'link';

// How an item stands: on which layer (its index from the back; −1 on the ground beneath them all),
// and whether it is hidden, held where it is, or faint, by its own word or its layer's.
export function standing(board, id) {
  const item = board.items[id] || {};
  const index = item.layer === undefined ? -1 : (board.layers || []).findIndex((l) => l.id === item.layer);
  const layer = index >= 0 ? board.layers[index] : null;
  return { layer: index, hidden: Boolean(item.hidden || layer?.hidden), locked: Boolean(item.locked || layer?.locked), opacity: layer?.opacity ?? 1 };
}

// The items of a region, and with `deep` those of the regions within it.
export function members(board, regionId, deep = false) {
  const out = [];
  const visit = (rid, guard) => {
    for (const id of board.order) {
      if (board.items[id].in !== rid || guard.has(id)) continue;
      guard.add(id);
      out.push(id);
      if (deep && board.items[id].kind === 'region') visit(id, guard);
    }
  };
  visit(regionId, new Set([regionId]));
  return out;
}

export function linksOf(board, ids) {
  const set = new Set(Array.isArray(ids) ? ids : [ids]);
  return board.order.filter((id) => isLink(board.items[id]) && (set.has(board.items[id].from) || set.has(board.items[id].to)));
}

// The rectangle that holds the given items (all of them when none are named); null for none.
export function bounds(board, ids = null) {
  let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
  for (const id of ids || Object.keys(board.place)) {
    const p = board.place[id];
    if (!p) continue;
    x0 = Math.min(x0, p.x); y0 = Math.min(y0, p.y); x1 = Math.max(x1, p.x + p.w); y1 = Math.max(y1, p.y + p.h);
  }
  return x0 === Infinity ? null : { x: x0, y: y0, w: x1 - x0, h: y1 - y0 };
}

// The region an item lying at `rect` belongs to: the front-most region holding its centre, itself and
// those within it left out. Null when it lies in none.
export function regionAt(board, rect, id = null) {
  const cx = rect.x + rect.w / 2, cy = rect.y + rect.h / 2;
  const inside = id && board.items[id]?.kind === 'region' ? new Set([id, ...members(board, id, true)]) : new Set([id]);
  let found = null, area = Infinity;
  for (const rid of board.order) {
    if (board.items[rid].kind !== 'region' || inside.has(rid)) continue;
    const p = board.place[rid];
    if (cx < p.x || cx > p.x + p.w || cy < p.y || cy > p.y + p.h) continue;
    // Of nested regions, the smallest holds the item.
    if (p.w * p.h <= area) { found = rid; area = p.w * p.h; }
  }
  return found;
}
