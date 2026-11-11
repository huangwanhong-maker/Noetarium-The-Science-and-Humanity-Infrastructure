// The operations of a board, and its history. A board's state is the fold of its operations; every
// operation has an inverse made as it is applied, so undo and redo are exact. Operations on different
// items commute, which is what merging two people's work will later need. Pure.
//
//   {t: 'add', id, item, place?, at?}     a new item, at an index of the order (the front when not given)
//   {t: 'remove', id}                     an item, which no link may still end on and no item be in
//   {t: 'set', id, patch}                 fields of an item's content; a field set to null is removed
//   {t: 'place', id, place}               some of x, y, w, h of an item's rectangle
//   {t: 'order', id, to}                  an item's index in the order, back to front
//   {t: 'title', title}                   the board's title
//   {t: 'layers', layers}                 the board's layers, back to front, replaced whole

import { isLink, members, linksOf, regionAt, makeId } from './model.js';

function step(draft, op) {
  const { items, place, order } = draft;
  switch (op.t) {
    case 'add': {
      if (!op.id || items[op.id]) throw new Error(`an item '${op.id}' cannot be added: ${op.id ? 'the board has one already' : 'it has no identifier'}`);
      items[op.id] = op.item;
      if (op.place) place[op.id] = { ...op.place };
      const at = op.at === undefined ? order.length : Math.max(0, Math.min(order.length, op.at));
      order.splice(at, 0, op.id);
      return { t: 'remove', id: op.id };
    }
    case 'remove': {
      const item = items[op.id];
      if (!item) throw new Error(`there is no item '${op.id}' to remove`);
      for (const id of order) {
        const other = items[id];
        if (id !== op.id && (other.from === op.id || other.to === op.id)) throw new Error(`'${op.id}' cannot be removed while the link '${id}' ends on it`);
        if (other.in === op.id) throw new Error(`'${op.id}' cannot be removed while '${id}' is in it`);
      }
      const inverse = { t: 'add', id: op.id, item, at: order.indexOf(op.id) };
      if (place[op.id]) inverse.place = place[op.id];
      delete items[op.id];
      delete place[op.id];
      order.splice(inverse.at, 1);
      return inverse;
    }
    case 'set': {
      const item = items[op.id];
      if (!item) throw new Error(`there is no item '${op.id}' to change`);
      const next = { ...item }, back = {};
      for (const [k, v] of Object.entries(op.patch)) {
        if (k === 'kind') throw new Error('the kind of an item does not change');
        back[k] = item[k] === undefined ? null : item[k];
        if (v === null || v === undefined) delete next[k];
        else next[k] = v;
      }
      items[op.id] = next;
      return { t: 'set', id: op.id, patch: back };
    }
    case 'place': {
      const old = place[op.id];
      if (!old) throw new Error(`'${op.id}' has no place to change`);
      const back = {};
      for (const k of Object.keys(op.place)) back[k] = old[k];
      place[op.id] = { ...old, ...op.place };
      return { t: 'place', id: op.id, place: back };
    }
    case 'order': {
      const from = order.indexOf(op.id);
      if (from < 0) throw new Error(`there is no item '${op.id}' to reorder`);
      order.splice(from, 1);
      order.splice(Math.max(0, Math.min(order.length, op.to)), 0, op.id);
      return { t: 'order', id: op.id, to: from };
    }
    case 'title': {
      const inverse = { t: 'title', title: draft.title };
      draft.title = String(op.title);
      return inverse;
    }
    case 'layers': {
      // A board that never had layers has none again when the first is undone (layers: null).
      const inverse = { t: 'layers', layers: draft.layers ?? null };
      const kept = new Set((op.layers || []).map((l) => l.id));
      for (const id of order) if (items[id].layer !== undefined && !kept.has(items[id].layer)) throw new Error(`the layer '${items[id].layer}' cannot be removed while '${id}' lies on it`);
      if (op.layers === null) delete draft.layers;
      else draft.layers = op.layers.map((l) => ({ ...l }));
      return inverse;
    }
    default:
      throw new Error(`a board has no operation '${op.t}'`);
  }
}

// The board after the operations, and the operations that undo them. The board given is not changed;
// if an operation is refused, nothing is applied.
export function apply(board, ops) {
  const draft = { ...board, items: { ...board.items }, place: { ...board.place }, order: [...board.order] };
  const inverse = [];
  for (const op of ops) inverse.unshift(step(draft, op));
  return { board: draft, inverse };
}

// A board with its past and its future. `commit` applies operations as one step; `amend` adds to the
// last step, so that what follows from an edit (a note grown to hold its text) is undone with it.
export function createHistory(initial, { limit = 400 } = {}) {
  let now = initial;
  let past = [], future = [];
  return {
    get board() { return now; },
    get canUndo() { return past.length > 0; },
    get canRedo() { return future.length > 0; },
    get length() { return past.length; },
    commit(ops, label = '') {
      if (!ops.length) return now;
      const r = apply(now, ops);
      now = r.board;
      past.push({ ops, inverse: r.inverse, label });
      if (past.length > limit) past.shift();
      future = [];
      return now;
    },
    amend(ops) {
      if (!ops.length) return now;
      const last = past[past.length - 1];
      if (!last) return this.commit(ops);
      const r = apply(now, ops);
      now = r.board;
      last.ops = [...last.ops, ...ops];
      last.inverse = [...r.inverse, ...last.inverse];
      return now;
    },
    undo() {
      const stepBack = past.pop();
      if (!stepBack) return null;
      now = apply(now, stepBack.inverse).board;
      future.push(stepBack);
      return stepBack.label || 'a change';
    },
    redo() {
      const stepOn = future.pop();
      if (!stepOn) return null;
      now = apply(now, stepOn.ops).board;
      past.push(stepOn);
      return stepOn.label || 'a change';
    },
    reset(board) { now = board; past = []; future = []; },
  };
}

// ---- operations composed --------------------------------------------------------------------------

// Removing items: the links that end on them go first, and what was in a removed region is left on
// the board, in the region that held it.
export function removeOps(board, ids) {
  const gone = new Set(ids.filter((id) => board.items[id]));
  for (const id of linksOf(board, [...gone])) gone.add(id);
  const ops = [];
  for (const id of board.order) {
    const item = board.items[id];
    if (!gone.has(id) && item.in !== undefined && gone.has(item.in)) {
      let up = board.items[item.in].in;
      while (up !== undefined && gone.has(up)) up = board.items[up].in;
      ops.push({ t: 'set', id, patch: { in: up === undefined ? null : up } });
    }
  }
  const links = [...gone].filter((id) => isLink(board.items[id]));
  const rest = [...gone].filter((id) => !isLink(board.items[id]));
  // Items within removed regions before the regions.
  const depth = (id) => { let d = 0, at = board.items[id].in; while (at !== undefined && d < 64) { d++; at = board.items[at]?.in; } return d; };
  rest.sort((a, b) => depth(b) - depth(a));
  for (const id of [...links, ...rest]) ops.push({ t: 'remove', id });
  return ops;
}

// Removing a layer: what lay on it is left on the ground, then the layer goes.
export function removeLayerOps(board, layerId) {
  const ops = board.order.filter((id) => board.items[id].layer === layerId).map((id) => ({ t: 'set', id, patch: { layer: null } }));
  return [...ops, { t: 'layers', layers: (board.layers || []).filter((l) => l.id !== layerId) }];
}

// Copies of items, moved by an offset, with the links among them. Returns the operations and the new
// identifiers by the old.
export function duplicateOps(board, ids, { dx = 24, dy = 24, id = makeId } = {}) {
  const chosen = new Set(ids);
  for (const rid of ids) if (board.items[rid]?.kind === 'region') for (const m of members(board, rid, true)) chosen.add(m);
  const fresh = new Map();
  for (const old of chosen) if (board.items[old]) fresh.set(old, id());
  const ops = [];
  for (const old of board.order) {
    if (!fresh.has(old)) continue;
    const item = board.items[old];
    if (isLink(item)) continue;
    const copy = { ...item };
    if (copy.in !== undefined && fresh.has(copy.in)) copy.in = fresh.get(copy.in);
    const p = board.place[old];
    ops.push({ t: 'add', id: fresh.get(old), item: copy, place: { ...p, x: p.x + dx, y: p.y + dy } });
  }
  // A link is copied exactly when both its ends are.
  for (const old of board.order) {
    const item = board.items[old];
    if (!isLink(item)) continue;
    if (!fresh.has(item.from) || !fresh.has(item.to)) { fresh.delete(old); continue; }
    if (!fresh.has(old)) fresh.set(old, id());
    ops.push({ t: 'add', id: fresh.get(old), item: { ...item, from: fresh.get(item.from), to: fresh.get(item.to) } });
  }
  return { ops, ids: fresh };
}

// The membership of items after they have moved: each belongs to the region its centre lies in.
export function membershipOps(board, ids) {
  const ops = [];
  for (const id of ids) {
    const item = board.items[id];
    if (!item || isLink(item)) continue;
    const region = regionAt(board, board.place[id], id);
    if ((item.in ?? null) !== region) ops.push({ t: 'set', id, patch: { in: region } });
  }
  return ops;
}

// Placements (a map from identifiers to some of x, y, w, h) as operations, leaving out what does not change.
export function placeOps(board, placements) {
  const ops = [];
  for (const [id, p] of Object.entries(placements)) {
    const old = board.place[id];
    if (!old) continue;
    const change = {};
    for (const k of ['x', 'y', 'w', 'h']) if (p[k] !== undefined && p[k] !== old[k]) change[k] = p[k];
    if (Object.keys(change).length) ops.push({ t: 'place', id, place: change });
  }
  return ops;
}
