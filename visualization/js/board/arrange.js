// Arrangements: functions from a board's content to a placement. Each returns a map from identifiers
// to the parts of a rectangle that change, and changes nothing itself (operations.js's placeOps makes
// them operations). Pure and deterministic: the same board gives the same placement.

import { isLink, members, RELATIONS } from './model.js';

// Items in reading order: by rows from the top, and from the left within a row.
function readingOrder(board, ids) {
  const list = ids.filter((id) => board.place[id]).map((id) => ({ id, ...board.place[id] }));
  list.sort((a, b) => a.y - b.y || a.x - b.x);
  const rows = [];
  for (const it of list) {
    const row = rows[rows.length - 1];
    if (row && it.y < row.y + row.h * 0.6) row.items.push(it);
    else rows.push({ y: it.y, h: it.h, items: [it] });
  }
  return rows.flatMap((row) => row.items.sort((a, b) => a.x - b.x));
}

// A grid of items in reading order from a corner, in columns of the widest item; each row is as high
// as its tallest.
function grid(list, { x, y, cols, gap }) {
  const out = {};
  const cw = Math.max(...list.map((it) => it.w));
  let top = y;
  for (let k = 0; k < list.length; k += cols) {
    const row = list.slice(k, k + cols);
    row.forEach((it, c) => { out[it.id] = { x: x + c * (cw + gap), y: top }; });
    top += Math.max(...row.map((it) => it.h)) + gap;
  }
  return { out, width: Math.min(cols, list.length) * (cw + gap) - gap, height: top - gap - y };
}

// The items given, set in a tidy grid where they are.
export function tidy(board, ids, { gap = 16, cols = null } = {}) {
  const list = readingOrder(board, ids.filter((id) => !isLink(board.items[id])));
  if (list.length < 2) return {};
  const x = Math.min(...list.map((it) => it.x)), y = Math.min(...list.map((it) => it.y));
  return grid(list, { x, y, cols: cols || Math.ceil(Math.sqrt(list.length * 1.5)), gap }).out;
}

// Every region's items set in a grid within it, under its title; a region too low for its items grows.
export function tidyRegions(board, { gap = 14, pad = 18, head = 46, only = null } = {}) {
  const out = {};
  for (const rid of board.order) {
    if (board.items[rid].kind !== 'region' || (only && !only.includes(rid))) continue;
    const inside = members(board, rid).filter((id) => !isLink(board.items[id]) && board.items[id].kind !== 'region');
    if (!inside.length) continue;
    const p = { ...board.place[rid], ...out[rid] };
    const list = readingOrder(board, inside);
    const cw = Math.max(...list.map((it) => it.w));
    const cols = Math.max(1, Math.floor((p.w - 2 * pad + gap) / (cw + gap)));
    const g = grid(list, { x: p.x + pad, y: p.y + head, cols, gap });
    Object.assign(out, g.out);
    const need = head + g.height + pad;
    if (need > p.h) out[rid] = { ...out[rid], h: need };
    if (g.width + 2 * pad > p.w) out[rid] = { ...out[rid], w: g.width + 2 * pad };
  }
  return out;
}

// The links read as a forest and set out as a tree growing to the right (a mind map, an issue tree):
// a parent beside the middle of its children. Each tree keeps its root where it is. Items no link
// touches are left alone; a cycle is cut where it closes.
export function tree(board, { hgap = 56, vgap = 14, ids = null } = {}) {
  const nodes = (ids || board.order).filter((id) => board.place[id] && !isLink(board.items[id]));
  const allowed = new Set(nodes);
  const children = new Map(nodes.map((id) => [id, []])), parent = new Map();
  for (const id of board.order) {
    const link = board.items[id];
    if (!isLink(link) || !allowed.has(link.from) || !allowed.has(link.to)) continue;
    const up = RELATIONS[link.rel]?.parent === 'to' ? link.to : link.from, down = up === link.from ? link.to : link.from;
    if (parent.has(down) || down === up) continue;
    // A link that would close a cycle is not a branch.
    let at = up, loop = false;
    while (at !== undefined) { if (at === down) { loop = true; break; } at = parent.get(at); }
    if (loop) continue;
    parent.set(down, up);
    children.get(up).push(down);
  }
  const out = {};
  const roots = nodes.filter((id) => !parent.has(id) && children.get(id).length);
  for (const root of roots) {
    // Children keep the order they have from top to bottom.
    const sorted = (id) => children.get(id).slice().sort((a, b) => board.place[a].y - board.place[b].y || board.place[a].x - board.place[b].x);
    const pos = {};
    let cursor = 0;
    const lay = (id, x) => {
      const p = board.place[id], kids = sorted(id);
      if (!kids.length) {
        pos[id] = { x, y: cursor };
        cursor += p.h + vgap;
        return;
      }
      const before = cursor;
      for (const kid of kids) lay(kid, x + p.w + hgap);
      const first = pos[kids[0]], last = pos[kids[kids.length - 1]];
      const mid = (first.y + last.y + board.place[kids[kids.length - 1]].h) / 2;
      pos[id] = { x, y: mid - p.h / 2 };
      // A parent taller than its children pushes what follows down.
      if (pos[id].y < before) {
        const shift = before - pos[id].y;
        const move = (n) => { pos[n].y += shift; for (const k of children.get(n)) move(k); };
        move(id);
        cursor += shift;
      }
      cursor = Math.max(cursor, pos[id].y + p.h + vgap);
    };
    lay(root, 0);
    const dx = board.place[root].x - pos[root].x, dy = board.place[root].y - pos[root].y;
    for (const [id, p] of Object.entries(pos)) out[id] = { x: Math.round(p.x + dx), y: Math.round(p.y + dy) };
  }
  return out;
}

// Items that say when (item.when, a day number through schedule.js's parseDate, given as `dayOf`), set
// in a row in the order of time; those that do not say are left alone.
export function byTime(board, ids, dayOf, { gap = 20 } = {}) {
  const dated = ids.filter((id) => board.place[id] && board.items[id].when).map((id) => ({ id, day: dayOf(board.items[id].when), ...board.place[id] }))
    .filter((it) => Number.isFinite(it.day));
  if (dated.length < 2) return {};
  const x0 = Math.min(...dated.map((it) => it.x)), y0 = Math.min(...dated.map((it) => it.y));
  dated.sort((a, b) => a.day - b.day || a.x - b.x);
  const out = {};
  let x = x0;
  for (const it of dated) { out[it.id] = { x, y: y0 }; x += it.w + gap; }
  return out;
}

// Edges or middles brought into line, and items spread evenly: the small arrangements of the hand.
export function align(board, ids, how) {
  const list = ids.filter((id) => board.place[id]).map((id) => ({ id, ...board.place[id] }));
  if (list.length < 2) return {};
  const out = {};
  const x0 = Math.min(...list.map((p) => p.x)), x1 = Math.max(...list.map((p) => p.x + p.w));
  const y0 = Math.min(...list.map((p) => p.y)), y1 = Math.max(...list.map((p) => p.y + p.h));
  for (const p of list) {
    if (how === 'left') out[p.id] = { x: x0 };
    else if (how === 'right') out[p.id] = { x: x1 - p.w };
    else if (how === 'centre') out[p.id] = { x: (x0 + x1) / 2 - p.w / 2 };
    else if (how === 'top') out[p.id] = { y: y0 };
    else if (how === 'bottom') out[p.id] = { y: y1 - p.h };
    else if (how === 'middle') out[p.id] = { y: (y0 + y1) / 2 - p.h / 2 };
  }
  if (how === 'across' || how === 'down') {
    const key = how === 'across' ? ['x', 'w'] : ['y', 'h'];
    list.sort((a, b) => a[key[0]] - b[key[0]]);
    const total = list.reduce((s, p) => s + p[key[1]], 0);
    const span = (how === 'across' ? x1 - x0 : y1 - y0) - total;
    const gap = span / (list.length - 1);
    let at = how === 'across' ? x0 : y0;
    for (const p of list) { out[p.id] = { [key[0]]: at }; at += p[key[1]] + gap; }
  }
  return out;
}
