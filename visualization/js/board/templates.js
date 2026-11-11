// Templates: boards' beginnings as data. A template is a list of items with their rectangles (from
// its own corner) and the links among them; laying one on a board is a list of operations. A plug-in
// brings templates of its own in the same form (a SWOT, a canvas, a plan). Pure.
//
//   {id, name, about, items: [{key, kind, x, y, w, h, in?: key, …content}], links: [{from: key, to: key, rel?, label?}]}

import { DEFAULT_SIZE, makeId, newItem } from './model.js';

// ---- the helpers a template is written with ---------------------------------------------------------

export const region = (key, title, x, y, w, h, extra = {}) => ({ key, kind: 'region', title, x, y, w, h, ...extra });
export const note = (type, text, x, y, extra = {}) => ({ kind: 'note', type, text, x, y, ...DEFAULT_SIZE.note, ...extra });
export const text = (words, x, y, extra = {}) => ({ kind: 'text', text: words, x, y, w: 420, h: 44, size: 'title', ...extra });

// Regions in a grid of columns and rows within a rectangle: cells are [key, title, column, row, columns
// spanned, rows spanned, extra].
export function sheet({ x = 0, y = 0, w, h, cols, rows, gap = 0 }, cells) {
  const cw = (w - gap * (cols - 1)) / cols, rh = (h - gap * (rows - 1)) / rows;
  return cells.map(([key, title, c, r, cs = 1, rs = 1, extra = {}]) =>
    region(key, title, x + c * (cw + gap), y + r * (rh + gap), cs * cw + (cs - 1) * gap, rs * rh + (rs - 1) * gap, extra));
}

// ---- laying a template on a board -------------------------------------------------------------------

export function templateBounds(template) {
  const boxes = template.items.filter((it) => it.kind !== 'link');
  const x0 = Math.min(...boxes.map((it) => it.x)), y0 = Math.min(...boxes.map((it) => it.y));
  const x1 = Math.max(...boxes.map((it) => it.x + it.w)), y1 = Math.max(...boxes.map((it) => it.y + it.h));
  return { x: x0, y: y0, w: x1 - x0, h: y1 - y0 };
}

// The operations that lay a template with its corner at `at`: regions at the back, in their order,
// then the rest, then the links. Returns them with the identifiers given to the template's keys.
export function instantiate(template, { at = { x: 0, y: 0 }, id = makeId } = {}) {
  const b = templateBounds(template);
  const ids = new Map();
  template.items.forEach((it, k) => ids.set(it.key ?? `#${k}`, id()));
  const ops = [];
  const add = (it, k) => {
    const { key, x, y, w, h, in: within, kind, ...content } = it;
    const item = newItem(kind, content);
    if (within !== undefined) {
      if (!ids.has(within)) throw new Error(`the template '${template.id}' puts an item in '${within}', which it does not have`);
      item.in = ids.get(within);
    }
    ops.push({ t: 'add', id: ids.get(key ?? `#${k}`), item, place: { x: Math.round(x - b.x + at.x), y: Math.round(y - b.y + at.y), w: Math.round(w), h: Math.round(h) } });
  };
  template.items.forEach((it, k) => { if (it.kind === 'region') add(it, k); });
  template.items.forEach((it, k) => { if (it.kind !== 'region') add(it, k); });
  for (const link of template.links || []) {
    if (!ids.has(link.from) || !ids.has(link.to)) throw new Error(`the template '${template.id}' links '${link.from}' to '${link.to}', and does not have both`);
    const { from, to, ...rest } = link;
    ops.push({ t: 'add', id: id(), item: newItem('link', { ...rest, from: ids.get(from), to: ids.get(to) }) });
  }
  return { ops, ids, size: { w: b.w, h: b.h } };
}

// A template in small, for a list to choose from: its rectangles in a box of its own proportions.
export function thumbnail(template) {
  const b = templateBounds(template);
  return {
    w: b.w, h: b.h,
    rects: template.items.filter((it) => it.kind !== 'link').map((it) => ({ x: it.x - b.x, y: it.y - b.y, w: it.w, h: it.h, kind: it.kind, type: it.type, tone: it.tone })),
  };
}

// ---- the templates every board has ------------------------------------------------------------------

const ring = (n, cx, cy, rx, ry) => Array.from({ length: n }, (_, k) => {
  const a = -Math.PI / 2 + (2 * Math.PI * k) / n;
  return { x: cx + rx * Math.cos(a) - 100, y: cy + ry * Math.sin(a) - 66 };
});

const brainstorm = (() => {
  const spots = ring(6, 600, 380, 430, 270);
  return {
    id: 'brainstorm', name: 'Brainstorm', about: 'A question in the middle and ideas about it. Tab on a note adds another joined to it.',
    items: [
      note('question', 'What are we trying to find out?', 480, 300, { key: 'q', w: 240, h: 160 }),
      ...spots.map((p, k) => note('idea', '', p.x, p.y, { key: `i${k}` })),
    ],
    links: spots.map((_, k) => ({ from: 'q', to: `i${k}` })),
  };
})();

const affinity = {
  id: 'affinity', name: 'Affinity map', about: 'Notes gathered into themes. Write first, then drag what belongs together into a theme and name it.',
  items: [
    text('What we heard', 0, 0),
    ...sheet({ x: 0, y: 64, w: 1500, h: 520, cols: 3, rows: 1, gap: 24 }, [
      ['a', 'Theme', 0, 0, 1, 1, { hint: 'Drag notes that belong together here, then name the theme.' }],
      ['b', 'Theme', 1, 0, 1, 1, { hint: 'A second theme.' }],
      ['c', 'Theme', 2, 0, 1, 1, { hint: 'A third.' }],
    ]),
    region('pile', 'Unsorted', 0, 612, 1500, 220, { hint: 'New notes start here.' }),
    note('plain', '', 18, 660, { in: 'pile' }), note('plain', '', 232, 660, { in: 'pile' }), note('plain', '', 446, 660, { in: 'pile' }),
  ],
};

const twoByTwo = {
  id: 'two-by-two', name: 'Two by two', about: 'Two questions asked of every note at once: here, what it is worth and what it costs.',
  items: [
    text('Worth against effort', 0, 0),
    ...sheet({ x: 0, y: 64, w: 1100, h: 760, cols: 2, rows: 2, gap: 0 }, [
      ['q1', 'Worth much · little effort', 0, 0, 1, 1, { tone: 'sage', hint: 'Do these first.' }],
      ['q2', 'Worth much · much effort', 1, 0, 1, 1, { tone: 'gold', hint: 'Plan these.' }],
      ['q3', 'Worth little · little effort', 0, 1, 1, 1, { tone: 'sky', hint: 'Fit these in.' }],
      ['q4', 'Worth little · much effort', 1, 1, 1, 1, { tone: 'clay', hint: 'Leave these.' }],
    ]),
  ],
};

const kanban = {
  id: 'kanban', name: 'To do, doing, done', about: 'Work in three columns, moved across as it advances.',
  items: sheet({ x: 0, y: 0, w: 1140, h: 640, cols: 3, rows: 1, gap: 20 }, [
    ['todo', 'To do', 0, 0], ['doing', 'Doing', 1, 0, 1, 1, { tone: 'gold' }], ['done', 'Done', 2, 0, 1, 1, { tone: 'sage' }],
  ]),
};

const issueTree = {
  id: 'issue-tree', name: 'Issue tree', about: 'A question divided into parts that do not overlap and leave nothing out. Arrange → tree sets it out again.',
  items: [
    note('question', 'The question', 0, 250, { key: 'root' }),
    note('question', 'First part', 300, 60, { key: 'a' }), note('question', 'Second part', 300, 250, { key: 'b' }), note('question', 'Third part', 300, 440, { key: 'c' }),
    note('plain', '', 600, 0, { key: 'a1' }), note('plain', '', 600, 146, { key: 'a2' }),
    note('plain', '', 600, 292, { key: 'b1' }), note('plain', '', 600, 438, { key: 'c1' }),
  ],
  links: [['root', 'a'], ['root', 'b'], ['root', 'c'], ['a', 'a1'], ['a', 'a2'], ['b', 'b1'], ['c', 'c1']].map(([from, to]) => ({ from, to })),
};

const plan = {
  id: 'plan', name: 'Plan', about: 'Tasks on a line of time, each after what it waits for. Written as lines of text in the panel.',
  items: [{
    kind: 'schedule', mode: 'plan', title: 'Plan', x: 0, y: 0, w: 820, h: 250,
    source: ['# Prepare', 'Frame the question : 2026-11-16, 4d', 'Gather what is known : after Frame the question, 2w, 30%',
      '# Work', 'First pass : after Gather what is known, 3w', 'Review : after First pass, milestone', 'Second pass : after Review, 2w'].join('\n'),
  }],
};

const chronicle = {
  id: 'chronicle', name: 'Chronicle', about: 'Events on a line of time: a date, a colon and what happened.',
  items: [{
    kind: 'schedule', mode: 'chronicle', title: 'A chronicle', x: 0, y: 0, w: 900, h: 200,
    source: ['1543 : De revolutionibus', '1609 : Astronomia nova', '1610-01 : The moons of Jupiter seen', '1632..1633 : The Dialogue and the trial',
      '1687-07-05 : Principia'].join('\n'),
  }],
};

export const GENERAL = {
  id: 'general', name: 'General', about: 'Beginnings for any subject.',
  templates: [brainstorm, affinity, twoByTwo, issueTree, kanban, plan, chronicle],
};
