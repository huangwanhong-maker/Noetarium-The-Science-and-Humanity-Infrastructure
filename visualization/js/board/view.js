// A board on the page (DOM): its items drawn where they lie, and the hand's work on them. Everything
// the hand does becomes operations on the model (operations.js), so that it can be undone, saved and
// later merged; nothing is kept only in the page.
//
//   drag the ground           move over the board          wheel, or pinch       zoom about the pointer
//   Shift + drag the ground   select what a rectangle touches
//   click, Shift + click      select                       drag                  move, snapping to the others (Alt: freely)
//   double-click              write in an item; on the ground, a new note
//   drag a side's dot         join to another item; let go on the ground for a new note joined to this one
//   Tab                       a new note joined to the one selected
//   Delete · Ctrl Z · Ctrl Shift Z · Ctrl D · Ctrl A · Ctrl C X V · arrows · [ ] · 1 (see all) · 0 (true size)
//   P                         the pen: a mouse or a stylus draws (a stylus by its pressure), fingers still move the board;
//   E · V                     the rubber, which takes strokes away; the hand again

import { h, s, clear } from './dom.js';
import { NOTE_TYPES, DEFAULT_SIZE, MIN_SIZE, RELATIONS, emptyBoard, newItem, makeId, isLink, members, bounds, checkBoard, standing } from './model.js';
import { apply, createHistory, removeOps, duplicateOps, membershipOps, placeOps, removeLayerOps } from './operations.js';
import { intersects, normalised, linkPath, snap, toScreen, toBoard, zoomAbout, fitView, simplify, strokeOutline } from './geometry.js';
import { collect, evaluate } from './quantities.js';
import { instantiate } from './templates.js';
import { draw, signature, heightMode, release } from './kinds.js';
import { collectTables, boardForModules, parseDelimited, looksTabular, tableName } from './tables.js';
import { CARDS, runModule } from './cards.js';

const CLIP = 'application/x-noetarium-board';

export function createBoard(host, options = {}) {
  // insets: what stands over the stage's edges (a rail, a panel), as {left, right, top, bottom} or a function giving it.
  // services: what the work serves the board's cards: {data(path) → a file of the data library, run(request, progress) → a module's results, modules: [described]}.
  const opt = { board: null, noteTypes: [], cards: {}, storeImage: null, today: null, insets: null, services: {}, ...options };
  const cards = { ...CARDS, ...opt.cards };
  const types = new Map([...NOTE_TYPES, ...opt.noteTypes].map((t) => [t.id, t]));
  const history = createHistory(opt.board || emptyBoard());
  const listeners = { change: new Set(), select: new Set(), view: new Set(), say: new Set(), tool: new Set() };
  const emit = (name, ...args) => { for (const fn of listeners[name]) fn(...args); };

  host.classList.add('bd-host');
  host.tabIndex = 0;
  const world = h('div', { class: 'bd-world' });
  const linksSvg = s('svg', { class: 'bd-links' });
  const handles = h('div', { class: 'bd-handles' });
  const inkSvg = s('svg', { class: 'bd-links bd-ink-layer' });
  const overlay = s('svg', { class: 'bd-overlay' });
  const stage = h('div', { class: 'bd-stage' }, world, overlay);
  world.append(linksSvg, inkSvg, handles);
  host.append(stage);

  let view = { x: 0, y: 0, k: 1 };
  const selection = new Set();
  const recs = new Map();        // id → {el, item, place, sig}
  const live = new Map();        // id → the rectangle an item has while the hand moves it
  let values = new Map(), definitions = new Map(), held = new Set(), tables = new Map();
  const transient = new Map();   // id → what a card is doing now (a module running), which is not the board's to keep
  let tool = { name: 'select', colour: 'ink', width: 3, marker: false };
  let onLayer = null;            // the layer new items are laid on; null for the ground
  let editing = null, gesture = null, space = false;
  let pointer = { x: 0, y: 0 };  // the last place of the pointer, on the stage
  const touches = new Map();

  const board = () => history.board;
  const rectOf = (id) => live.get(id) || board().place[id];
  const screenOf = (e) => { const r = stage.getBoundingClientRect(); return { x: e.clientX - r.left, y: e.clientY - r.top }; };
  const stageSize = () => ({ w: stage.clientWidth || 800, h: stage.clientHeight || 600 });
  const inset = () => (typeof opt.insets === 'function' ? opt.insets() : opt.insets) || {};
  // The middle of the stage that nothing stands over.
  const clearCentre = () => { const z = stageSize(), i = inset(); return { x: ((i.left || 0) + z.w - (i.right || 0)) / 2, y: ((i.top || 0) + z.h - (i.bottom || 0)) / 2 }; };
  const viewCentre = () => toBoard(view, clearCentre());

  // What a card is given when it is drawn, and when its panel is made.
  function cardCtx(id, item) {
    const def = cards[item.card] || {}, p = rectOf(id) || { w: 0, h: 0 };
    return {
      id, props: { ...(def.props || {}), ...(item.props || {}) }, out: item.out || null, state: transient.get(id) || null,
      width: p.w, height: p.h, tables, values, services: opt.services,
      zoom: () => view.k,
      set: (props, label = 'change the card') => commit([{ t: 'set', id, patch: { props: { ...(board().items[id].props || {}), ...props } } }], label),
      setOut: (out) => { if (board().items[id]) commit([{ t: 'set', id, patch: { out } }], 'run a module'); },
      setState: (state) => {
        if (state) transient.set(id, state); else transient.delete(id);
        if (board().items[id]) { refresh(); fitHeights(); }
      },
      given: () => boardForModules(board(), tables, values),
      say: (words) => emit('say', words),
      // Another item laid to the right of this card: a table it answered, drawn.
      addBeside: (kind, props) => {
        const here = board().place[id], size = (kind === 'card' && cards[props.card]?.size) || DEFAULT_SIZE[kind];
        return add(kind, props, { at: { x: here.x + here.w + 48 + size.w / 2, y: here.y + size.h / 2 } });
      },
    };
  }

  const ctx = {
    types, cards, get values() { return values; }, get definitions() { return definitions; }, get tables() { return tables; }, get today() { return opt.today; },
    hasMembers: (id) => held.has(id),
    widthOf: (id) => rectOf(id)?.w,
    heightOf: (id) => rectOf(id)?.h,
    card: cardCtx,
  };

  // ---- drawing ------------------------------------------------------------------------------------

  function position(el, item, p) {
    el.style.left = `${p.x}px`;
    el.style.top = `${p.y}px`;
    el.style.width = `${p.w}px`;
    const mode = heightMode(ctx, item);
    el.style.height = mode ? '' : `${p.h}px`;
    el.style.minHeight = mode === 'grow' ? `${p.h}px` : '';
  }

  function refresh() {
    const b = board();
    const defs = collect(b);
    values = evaluate(defs);
    tables = collectTables(b);
    definitions = new Map();
    for (const d of defs) definitions.set(d.item, [...(definitions.get(d.item) || []), d]);
    held = new Set(b.order.map((id) => b.items[id].in).filter((r) => r !== undefined));
    for (const id of [...selection]) if (!b.items[id]) selection.delete(id);
    for (const [id, rec] of recs) if (!b.items[id] || isLink(b.items[id])) { release(rec.el); rec.el.remove(); recs.delete(id); }
    if (onLayer && !(b.layers || []).some((l) => l.id === onLayer)) onLayer = null;
    // Links lie between the regions and the rest of the layer in front.
    linksSvg.style.zIndex = String((b.layers || []).length * 100000 + 1500);
    b.order.forEach((id, index) => {
      const item = b.items[id];
      if (isLink(item)) return;
      let rec = recs.get(id);
      if (!rec) {
        rec = { el: h('div', { class: 'bd-item', 'data-id': id }), item: null, place: null, sig: null };
        recs.set(id, rec);
        world.append(rec.el);
      }
      const place = b.place[id];
      if (rec.place !== place) { position(rec.el, item, place); rec.place = place; }
      const sig = signature(ctx, id, item);
      if ((rec.item !== item || rec.sig !== sig) && editing?.id !== id) {
        draw(ctx, rec.el, id, item);
        rec.item = item;
        rec.sig = sig;
      }
      const stands = standing(b, id);
      rec.el.style.zIndex = (stands.layer + 1) * 100000 + (item.kind === 'region' ? 10 : 2000) + index;
      rec.el.style.opacity = stands.opacity < 1 ? String(stands.opacity) : '';
      rec.el.classList.toggle('sel', selection.has(id));
      rec.el.classList.toggle('is-hidden', stands.hidden);
      rec.el.classList.toggle('is-held', stands.locked);
    });
    drawLinks();
    drawHandles();
  }

  // Items whose height follows their content are measured, and the model told: what is drawn and what
  // is kept agree. The change joins the step that caused it.
  function fitHeights() {
    const b = board(), ops = [];
    for (const [id, rec] of recs) {
      const mode = b.items[id] ? heightMode(ctx, b.items[id]) : null;
      if (!mode || editing?.id === id) continue;
      const hgt = Math.ceil(rec.el.offsetHeight), p = b.place[id];
      if (!hgt) continue;
      if (mode === 'grow' ? hgt > p.h + 0.5 : Math.abs(hgt - p.h) > 0.5) ops.push({ t: 'place', id, place: { h: hgt } });
    }
    if (!ops.length) return false;
    if (history.length) history.amend(ops);
    else history.reset(apply(b, ops).board);
    refresh();
    return true;
  }

  function changed(why = '') {
    refresh();
    fitHeights();
    emit('change', board(), why);
  }

  function drawLinks() {
    const b = board();
    clear(linksSvg);
    for (const id of b.order) {
      const item = b.items[id];
      if (!isLink(item)) continue;
      const a = rectOf(item.from), c = rectOf(item.to);
      if (!a || !c || standing(b, item.from).hidden || standing(b, item.to).hidden) continue;
      const path = linkPath(a, c);
      const label = item.label ?? RELATIONS[item.rel]?.label ?? '';
      const g = s('g', { class: `bd-link rel-${item.rel}${selection.has(id) ? ' sel' : ''}`, 'data-id': id },
        s('path', { class: 'hit', d: path.d }), s('path', { class: 'line', d: path.d }), s('path', { class: 'head', d: path.arrow }));
      if (label) g.append(s('text', { x: path.mid.x.toFixed(1), y: (path.mid.y + 4).toFixed(1), 'text-anchor': 'middle' }, label));
      linksSvg.append(g);
    }
    if (gesture?.type === 'connect' && gesture.to) {
      const path = linkPath(rectOf(gesture.from), gesture.over ? rectOf(gesture.over) : { ...gesture.to, w: 0, h: 0 });
      linksSvg.append(s('g', { class: 'bd-link drawing' }, s('path', { class: 'line', d: path.d }), s('path', { class: 'head', d: path.arrow })));
    }
  }

  function drawHandles() {
    clear(handles);
    if (selection.size !== 1 || editing) return;
    const [id] = selection;
    const item = board().items[id], p = rectOf(id);
    if (!item || isLink(item) || !p || standing(board(), id).hidden) return;
    // The frame takes the height the element has, which may be more than its place while it is written in.
    const height = Math.max(p.h, recs.get(id)?.el.offsetHeight || 0);
    const frame = h('div', { class: 'bd-frame', style: `left:${p.x}px;top:${p.y}px;width:${p.w}px;height:${height}px` });
    // What is held where it is has no grips to be resized by.
    if (!standing(board(), id).locked) for (const c of ['nw', 'ne', 'sw', 'se']) frame.append(h('i', { class: `bd-grip ${c}`, 'data-handle': c }));
    for (const c of ['n', 'e', 's', 'w']) frame.append(h('i', { class: `bd-dot ${c}`, 'data-connect': c, title: 'drag to join to another item' }));
    handles.append(frame);
  }

  function drawGuides(guides = [], marquee = null) {
    clear(overlay);
    for (const g of guides) {
      const a = toScreen(view, g.x !== undefined ? { x: g.x, y: g.from } : { x: g.from, y: g.y });
      const c = toScreen(view, g.x !== undefined ? { x: g.x, y: g.to } : { x: g.to, y: g.y });
      overlay.append(s('path', { class: 'bd-guide', d: `M${a.x.toFixed(1)} ${a.y.toFixed(1)}L${c.x.toFixed(1)} ${c.y.toFixed(1)}` }));
    }
    if (marquee) overlay.append(s('rect', { class: 'bd-marquee', x: marquee.x, y: marquee.y, width: marquee.w, height: marquee.h }));
  }

  function applyView() {
    world.style.transform = `translate(${view.x}px, ${view.y}px) scale(${view.k})`;
    host.style.setProperty('--bd-ik', String(1 / view.k));
    // The ground's dots keep a readable spacing: coarser when the board is far.
    const unit = 24 * view.k * (view.k < 0.45 ? 4 : 1);
    stage.style.backgroundSize = `${unit}px ${unit}px`;
    stage.style.backgroundPosition = `${view.x}px ${view.y}px`;
    emit('view', view);
  }

  function setView(next) { view = next; applyView(); }

  // ---- changes ------------------------------------------------------------------------------------

  function commit(ops, label = '') {
    if (!ops.length) return;
    try {
      history.commit(ops, label);
    } catch (error) {
      emit('say', error.message);
      return;
    }
    changed(label);
  }

  function select(ids, { add = false } = {}) {
    if (!add) selection.clear();
    for (const id of ids) if (board().items[id]) selection.add(id);
    for (const [id, rec] of recs) rec.el.classList.toggle('sel', selection.has(id));
    drawLinks();
    drawHandles();
    emit('select', [...selection]);
  }

  // A place for something new of a size: about a point, moved aside while it would lie on another item.
  function placeAbout(at, size) {
    const p = { x: Math.round(at.x - size.w / 2), y: Math.round(at.y - size.h / 2), ...size };
    const b = board();
    for (let n = 0; n < 12; n++) {
      const hit = b.order.some((id) => b.place[id] && b.items[id].kind !== 'region' && Math.abs(b.place[id].x - p.x) < 8 && Math.abs(b.place[id].y - p.y) < 8);
      if (!hit) break;
      p.x += 22; p.y += 22;
    }
    return p;
  }

  function add(kind, props = {}, { at = null, edit = false, size = null } = {}) {
    const id = makeId();
    const place = placeAbout(at || viewCentre(), size || (kind === 'card' && cards[props.card]?.size) || DEFAULT_SIZE[kind]);
    const ops = [{ t: 'add', id, item: newItem(kind, onLayer ? { ...props, layer: onLayer } : props), place }];
    const after = apply(board(), ops).board;
    commit([...ops, ...membershipOps(after, [id])], `add a ${kind}`);
    select([id]);
    if (edit) startEdit(id);
    return id;
  }

  function link(from, to, props = {}) {
    const b = board();
    if (from === to || !b.items[from] || !b.items[to] || isLink(b.items[from]) || isLink(b.items[to])) return null;
    const existing = b.order.find((id) => isLink(b.items[id]) && b.items[id].from === from && b.items[id].to === to);
    if (existing) return existing;
    const id = makeId();
    commit([{ t: 'add', id, item: newItem('link', { from, to, ...props }) }], 'join');
    return id;
  }

  function remove(ids = [...selection]) {
    if (!ids.length) return;
    commit(removeOps(board(), ids), 'remove');
    select([]);
  }

  function duplicate(ids = [...selection]) {
    if (!ids.length) return;
    const r = duplicateOps(board(), ids);
    commit(r.ops, 'duplicate');
    select([...r.ids.values()]);
  }

  // Placements made by an arrangement or by the keys, with the memberships that follow from them.
  function place(placements, label = 'arrange') {
    const ops = placeOps(board(), placements);
    if (!ops.length) return;
    const after = apply(board(), ops).board;
    const moved = ops.map((o) => o.id);
    const regions = moved.some((id) => board().items[id].kind === 'region');
    commit([...ops, ...membershipOps(after, regions ? after.order : moved)], label);
  }

  function reorder(ids, front) {
    const b = board();
    const ops = ids.filter((id) => b.items[id]).map((id) => ({ t: 'order', id, to: front ? b.order.length - 1 : 0 }));
    commit(front ? ops : ops.reverse(), front ? 'bring to the front' : 'send to the back');
  }

  // A template laid on the board: at its corner when the board is empty, else to the right of what
  // is there. The view goes to it.
  function insert(template) {
    const all = bounds(board());
    const at = all ? { x: Math.round(all.x + all.w + 140), y: Math.round(all.y) } : { x: 0, y: 0 };
    const r = instantiate(template, { at });
    if (onLayer) for (const op of r.ops) if (!isLink(op.item)) op.item = { ...op.item, layer: onLayer };
    commit(r.ops, `lay out '${template.name}'`);
    const ids = [...r.ids.values()].filter((id) => board().place[id]);
    select([]);
    setView(fitView(bounds(board(), ids), stageSize(), { inset: inset() }));
    return ids;
  }

  // ---- writing in an item -------------------------------------------------------------------------

  // What has been written in an element, as text: its characters as they are (innerText would give
  // them as they are drawn, in capitals where the style sets capitals), a line break for every <br>
  // and before every block after the first.
  function written(node) {
    let out = '';
    const walk = (n) => {
      for (const kid of n.childNodes) {
        if (kid.nodeType === 3) out += kid.data;
        else if (kid.nodeName === 'BR') out += '\n';
        else if (kid.nodeType === 1) {
          if (/^(DIV|P)$/.test(kid.nodeName) && out && !out.endsWith('\n')) out += '\n';
          walk(kid);
        }
      }
    };
    walk(node);
    return out;
  }

  function startEdit(id, field = null) {
    const rec = recs.get(id);
    if (!rec || editing) return;
    const node = field ? rec.el.querySelector(`[data-edit="${field}"]`) : rec.el.querySelector('[data-edit]');
    if (!node) return;
    const name = node.dataset.edit;
    editing = { id, field: name, node, before: board().items[id][name] || '' };
    node.textContent = editing.before;
    try { node.contentEditable = 'plaintext-only'; } catch { node.contentEditable = 'true'; }
    if (node.contentEditable !== 'plaintext-only') node.contentEditable = 'true';
    rec.el.classList.add('writing');
    node.focus({ preventScroll: true });
    const range = document.createRange();
    range.selectNodeContents(node);
    if (editing.before) range.collapse(false);
    const sel = window.getSelection();
    sel.removeAllRanges();
    sel.addRange(range);
    node.addEventListener('keydown', editKey);
    node.addEventListener('blur', editBlur);
    node.addEventListener('input', editInput);
    drawHandles();
  }
  const editKey = (e) => {
    e.stopPropagation();
    if (e.key === 'Escape' || (e.key === 'Enter' && (e.ctrlKey || e.metaKey))) { e.preventDefault(); endEdit(); host.focus({ preventScroll: true }); }
  };
  const editBlur = () => endEdit();
  const editInput = () => {
    // Links follow a note as it grows under the hand.
    const rec = recs.get(editing.id), p = board().place[editing.id];
    live.set(editing.id, { ...p, h: Math.max(p.h, rec.el.offsetHeight) });
    drawLinks();
  };
  function endEdit() {
    if (!editing) return;
    const { id, field, node, before } = editing;
    node.removeEventListener('keydown', editKey);
    node.removeEventListener('blur', editBlur);
    node.removeEventListener('input', editInput);
    const text = written(node).replace(/\xa0/g, ' ').replace(/\r/g, '').replace(/\n+$/, '');
    node.contentEditable = 'false';
    editing = null;
    live.delete(id);
    const rec = recs.get(id);
    if (rec) { rec.el.classList.remove('writing'); rec.item = null; }
    if (text !== before && board().items[id]) commit([{ t: 'set', id, patch: { [field]: text } }], 'write');
    else { refresh(); fitHeights(); }
  }

  // ---- the hand -----------------------------------------------------------------------------------

  // The items that move with those selected: what is in a selected region goes with it.
  function moving() {
    const b = board(), ids = new Set();
    for (const id of selection) {
      if (!b.place[id] || standing(b, id).locked) continue;
      ids.add(id);
      if (b.items[id].kind === 'region') for (const m of members(b, id, true)) if (!standing(b, m).locked) ids.add(m);
    }
    return [...ids];
  }

  function itemAt(e) {
    const node = e.target.closest?.('[data-id]');
    return node && stage.contains(node) ? node.dataset.id : null;
  }

  function down(e) {
    if (editing) {
      if (editing.node.contains(e.target)) return;
      endEdit();
    }
    if (e.target.closest?.('[data-live]')) return;
    const at = screenOf(e);
    pointer = at;
    host.focus({ preventScroll: true });
    if (e.pointerType === 'touch') {
      touches.set(e.pointerId, at);
      if (touches.size === 2) {
        cancelGesture();
        const [a, c] = [...touches.values()];
        gesture = { type: 'pinch', d: Math.hypot(a.x - c.x, a.y - c.y), mid: { x: (a.x + c.x) / 2, y: (a.y + c.y) / 2 }, view };
        return;
      }
    }
    stage.setPointerCapture?.(e.pointerId);
    if (e.button === 1 || e.button === 2 || (e.button === 0 && space)) { gesture = { type: 'pan', at, view, moved: false }; return; }
    if (e.button !== 0) return;
    if (tool.name !== 'select') {
      // Fingers still move the board while a pen or a mouse draws on it.
      if (e.pointerType === 'touch') { gesture = { type: 'pan', at, view, moved: false }; return; }
      if (tool.name === 'rubber' || (e.pointerType === 'pen' && (e.buttons & 32))) { gesture = { type: 'rub', at, ids: new Set() }; rubAt(e); return; }
      const q = toBoard(view, at), pen = e.pointerType === 'pen';
      const path = s('path', { class: `bd-ink-live ink-${tool.colour}${tool.marker ? ' marker' : ''}` });
      inkSvg.append(path);
      gesture = { type: 'ink', at, pen, path, points: [[q.x, q.y, pen ? e.pressure || 0.5 : 0.5]] };
      path.setAttribute('d', strokeOutline(gesture.points, tool.width));
      return;
    }
    const grip = e.target.closest?.('[data-handle]'), dot = e.target.closest?.('[data-connect]');
    if (grip) {
      const [id] = selection;
      gesture = { type: 'resize', id, corner: grip.dataset.handle, at, origin: { ...board().place[id] }, moved: false };
      return;
    }
    if (dot) {
      const [id] = selection;
      gesture = { type: 'connect', from: id, at, to: null, over: null };
      return;
    }
    let id = itemAt(e);
    // What is held where it is lets the hand through to the ground, unless it is already selected.
    if (id && standing(board(), id).locked && !selection.has(id)) id = null;
    if (!id) {
      gesture = e.shiftKey ? { type: 'marquee', at, rect: null } : { type: 'pan', at, view, moved: false, clear: true };
      return;
    }
    if (e.shiftKey) {
      if (selection.has(id)) { selection.delete(id); select([...selection]); return; }
      select([id], { add: true });
    } else if (!selection.has(id)) select([id]);
    if (isLink(board().items[id])) return;
    const ids = moving();
    gesture = { type: 'move', at, ids, origin: new Map(ids.map((m) => [m, board().place[m]])), box: bounds(board(), ids), clicked: id, shift: e.shiftKey, moved: false };
  }

  function move(e) {
    const at = screenOf(e);
    pointer = at;
    if (e.pointerType === 'touch' && touches.has(e.pointerId)) touches.set(e.pointerId, at);
    if (!gesture) return;
    const dx = at.x - (gesture.at?.x ?? 0), dy = at.y - (gesture.at?.y ?? 0);
    const far = Math.hypot(dx, dy) > 4;
    switch (gesture.type) {
      case 'pinch': {
        const [a, c] = [...touches.values()];
        if (!a || !c) return;
        const d = Math.hypot(a.x - c.x, a.y - c.y), mid = { x: (a.x + c.x) / 2, y: (a.y + c.y) / 2 };
        const z = zoomAbout(gesture.view, d / Math.max(1, gesture.d), gesture.mid);
        setView({ k: z.k, x: z.x + mid.x - gesture.mid.x, y: z.y + mid.y - gesture.mid.y });
        return;
      }
      case 'pan':
        if (!gesture.moved && !far) return;
        gesture.moved = true;
        stage.classList.add('panning');
        setView({ k: view.k, x: gesture.view.x + dx, y: gesture.view.y + dy });
        return;
      case 'move': {
        if (!gesture.moved && !far) return;
        gesture.moved = true;
        let wx = dx / view.k, wy = dy / view.k, guides = [];
        if (!e.altKey && gesture.box) {
          const set = new Set(gesture.ids), b = board();
          const others = b.order.filter((id) => b.place[id] && !set.has(id)).map((id) => b.place[id]);
          const r = snap({ ...gesture.box, x: gesture.box.x + wx, y: gesture.box.y + wy }, others, 6 / view.k);
          wx += r.dx; wy += r.dy; guides = r.guides;
        }
        for (const id of gesture.ids) {
          const o = gesture.origin.get(id), p = { ...o, x: Math.round(o.x + wx), y: Math.round(o.y + wy) };
          live.set(id, p);
          const el = recs.get(id)?.el;
          if (el) { el.style.left = `${p.x}px`; el.style.top = `${p.y}px`; }
        }
        stage.classList.add('moving');
        drawLinks(); drawHandles(); drawGuides(guides);
        return;
      }
      case 'resize': {
        if (!gesture.moved && !far) return;
        gesture.moved = true;
        const o = gesture.origin, c = gesture.corner, item = board().items[gesture.id];
        let w = o.w + (c.includes('e') ? dx : -dx) / view.k, hgt = o.h + (c.includes('s') ? dy : -dy) / view.k;
        w = Math.max(MIN_SIZE.w, w); hgt = Math.max(MIN_SIZE.h, hgt);
        // An image keeps its proportions; so does anything while Shift is held.
        if (item.kind === 'image' ? !e.shiftKey : e.shiftKey) { const k = Math.max(w / o.w, hgt / o.h); w = o.w * k; hgt = o.h * k; }
        const p = { x: Math.round(c.includes('w') ? o.x + o.w - w : o.x), y: Math.round(c.includes('n') ? o.y + o.h - hgt : o.y), w: Math.round(w), h: Math.round(hgt) };
        live.set(gesture.id, p);
        const el = recs.get(gesture.id)?.el;
        if (el) position(el, item, p);
        drawLinks(); drawHandles();
        return;
      }
      case 'ink': {
        // Every sample the pen gave since the last frame, so that a quick stroke keeps its curve.
        const samples = e.getCoalescedEvents?.() || [];
        for (const ev of samples.length ? samples : [e]) {
          const q = toBoard(view, screenOf(ev)), last = gesture.points[gesture.points.length - 1];
          if (Math.hypot(q.x - last[0], q.y - last[1]) * view.k < 0.7) continue;
          gesture.points.push([q.x, q.y, gesture.pen ? ev.pressure || 0.5 : 0.5]);
        }
        gesture.path.setAttribute('d', strokeOutline(gesture.points, tool.width));
        return;
      }
      case 'rub':
        rubAt(e);
        return;
      case 'marquee':
        gesture.rect = normalised(gesture.at, at);
        drawGuides([], gesture.rect);
        return;
      case 'connect': {
        gesture.to = toBoard(view, at);
        const under = document.elementFromPoint(e.clientX, e.clientY)?.closest?.('.bd-item[data-id]');
        const over = under && stage.contains(under) && under.dataset.id !== gesture.from ? under.dataset.id : null;
        if (over !== gesture.over) {
          if (gesture.over) recs.get(gesture.over)?.el.classList.remove('target');
          if (over) recs.get(over)?.el.classList.add('target');
          gesture.over = over;
        }
        drawLinks();
        return;
      }
      default:
    }
  }

  function up(e) {
    touches.delete(e.pointerId);
    const g = gesture;
    gesture = null;
    stage.classList.remove('panning', 'moving');
    if (!g) return;
    switch (g.type) {
      case 'pan':
        if (!g.moved && g.clear) select([]);
        return;
      case 'move': {
        drawGuides();
        if (!g.moved) {
          if (!g.shift && selection.size > 1) select([g.clicked]);
          return;
        }
        const placements = Object.fromEntries(g.ids.map((id) => [id, live.get(id)]).filter(([, p]) => p));
        live.clear();
        place(placements, 'move');
        return;
      }
      case 'resize': {
        const p = live.get(g.id);
        live.clear();
        if (g.moved && p) place({ [g.id]: p }, 'resize');
        else refresh();
        return;
      }
      case 'marquee': {
        drawGuides();
        if (!g.rect) return;
        const a = toBoard(view, g.rect), c = toBoard(view, { x: g.rect.x + g.rect.w, y: g.rect.y + g.rect.h });
        const box = { x: a.x, y: a.y, w: c.x - a.x, h: c.y - a.y }, b = board();
        select(b.order.filter((id) => { const st = standing(b, id); return b.place[id] && !st.hidden && !st.locked && intersects(b.place[id], box); }), { add: true });
        return;
      }
      case 'ink': {
        g.path.remove();
        const pts = simplify(g.points, 0.45 / view.k), pad = tool.width * 0.9 + 1, r1 = (v) => Math.round(v * 10) / 10;
        const x0 = Math.min(...pts.map((q) => q[0])) - pad, y0 = Math.min(...pts.map((q) => q[1])) - pad;
        const w = Math.max(...pts.map((q) => q[0])) + pad - x0, hgt = Math.max(...pts.map((q) => q[1])) + pad - y0;
        const item = newItem('ink', { points: pts.map(([x, y, pr]) => [r1(x - x0), r1(y - y0), Math.round(pr * 100) / 100]), size: [r1(w), r1(hgt)], colour: tool.colour, width: tool.width, ...(tool.marker ? { marker: true } : {}), ...(onLayer ? { layer: onLayer } : {}) });
        const id = makeId(), ops = [{ t: 'add', id, item, place: { x: r1(x0), y: r1(y0), w: r1(w), h: r1(hgt) } }];
        commit([...ops, ...membershipOps(apply(board(), ops).board, [id])], 'draw');
        return;
      }
      case 'rub':
        if (g.ids.size) commit(removeOps(board(), [...g.ids]), 'rub out');
        return;
      case 'connect': {
        if (g.over) recs.get(g.over)?.el.classList.remove('target');
        if (g.over) { const id = link(g.from, g.over); drawLinks(); if (id) select([id]); return; }
        if (!g.to || Math.hypot(screenOf(e).x - g.at.x, screenOf(e).y - g.at.y) < 24) { drawLinks(); return; }
        const from = board().items[g.from];
        const id = add('note', { type: from.kind === 'note' ? from.type : 'idea' }, { at: g.to });
        link(g.from, id);
        select([id]);
        startEdit(id);
        return;
      }
      default:
    }
  }

  // The strokes under the rubber are marked, and taken away together when it is lifted.
  function rubAt(e) {
    const under = document.elementFromPoint(e.clientX, e.clientY)?.closest?.('.bd-ink[data-id]');
    if (!under || !stage.contains(under)) return;
    gesture.ids.add(under.dataset.id);
    under.classList.add('rubbed');
  }

  function setTool(next = {}) {
    tool = { ...tool, ...next };
    stage.dataset.tool = tool.name;
    if (tool.name !== 'select') select([]);
    emit('tool', tool);
  }

  function cancelGesture() {
    if (!gesture) return;
    gesture.path?.remove();
    gesture = null;
    live.clear();
    stage.classList.remove('panning', 'moving');
    drawGuides();
    for (const rec of recs.values()) { rec.place = null; rec.el.classList.remove('target', 'rubbed'); }
    refresh();
  }

  function dbl(e) {
    // The pointer is captured by the stage while it is down, so the event's own target is not to be
    // trusted: what lies under the pointer is asked for.
    const under = document.elementFromPoint(e.clientX, e.clientY);
    if (editing || !under || !stage.contains(under) || under.closest('[data-live],[data-handle],[data-connect]')) return;
    const id = under.closest('[data-id]')?.dataset.id;
    if (!id) { add('note', {}, { at: toBoard(view, screenOf(e)), edit: true }); return; }
    if (isLink(board().items[id])) return;
    select([id]);
    startEdit(id, under.closest('[data-edit]')?.dataset.edit || null);
  }

  function wheel(e) {
    e.preventDefault();
    const at = screenOf(e);
    // A pinch on a trackpad arrives as a wheel with Ctrl; a mouse's wheel in whole notches; two fingers in small steps.
    if (e.ctrlKey || e.metaKey) setView(zoomAbout(view, Math.exp(-e.deltaY * 0.01), at));
    else if (e.deltaMode === 1 || (e.deltaX === 0 && Math.abs(e.deltaY) >= 50)) setView(zoomAbout(view, Math.exp(-(e.deltaMode === 1 ? e.deltaY * 33 : e.deltaY) * 0.0017), at));
    else setView({ k: view.k, x: view.x - e.deltaX, y: view.y - e.deltaY });
  }

  // A new note joined to the one selected, beside it and below those it is already joined to.
  function branch() {
    if (selection.size !== 1) return;
    const [from] = selection, b = board(), item = b.items[from];
    if (!item || isLink(item)) return;
    const p = b.place[from], size = DEFAULT_SIZE.note;
    const kids = b.order.filter((id) => isLink(b.items[id]) && b.items[id].from === from).map((id) => b.place[b.items[id].to]).filter(Boolean);
    const y = kids.length ? Math.max(...kids.map((k) => k.y + k.h)) + 14 : p.y;
    const id = add('note', { type: item.kind === 'note' ? item.type : 'idea' }, { at: { x: p.x + p.w + 56 + size.w / 2, y: y + size.h / 2 } });
    link(from, id);
    select([id]);
    startEdit(id);
  }

  function key(e) {
    if (editing) return;
    if (e.target !== host) return;
    const mod = e.ctrlKey || e.metaKey, k = e.key;
    const done = () => { e.preventDefault(); e.stopPropagation(); };
    if (k === ' ') { space = true; stage.classList.add('hand'); return done(); }
    if (mod && (k === 'z' || k === 'Z')) { e.shiftKey ? redo() : undo(); return done(); }
    if (mod && (k === 'y' || k === 'Y')) { redo(); return done(); }
    if (mod && (k === 'a' || k === 'A')) { select(board().order.filter((id) => { const st = standing(board(), id); return board().place[id] && !st.hidden && !st.locked; })); return done(); }
    if (mod && (k === 'd' || k === 'D')) { duplicate(); return done(); }
    if (mod) return undefined;
    if (k === 'Delete' || k === 'Backspace') { remove(); return done(); }
    if (k === 'Escape') { cancelGesture(); if (tool.name !== 'select') setTool({ name: 'select' }); select([]); return done(); }
    if (k === 'p' || k === 'P') { setTool({ name: tool.name === 'pen' ? 'select' : 'pen' }); return done(); }
    if (k === 'e' || k === 'E') { setTool({ name: tool.name === 'rubber' ? 'select' : 'rubber' }); return done(); }
    if (k === 'v' || k === 'V') { setTool({ name: 'select' }); return done(); }
    if (k === 'Tab') { branch(); return done(); }
    if ((k === 'Enter' || k === 'F2') && selection.size === 1) { startEdit([...selection][0]); return done(); }
    if (k.startsWith('Arrow') && selection.size) {
      const step = e.shiftKey ? 10 : 1, d = { ArrowLeft: [-step, 0], ArrowRight: [step, 0], ArrowUp: [0, -step], ArrowDown: [0, step] }[k];
      const b = board();
      place(Object.fromEntries(moving().map((id) => [id, { x: b.place[id].x + d[0], y: b.place[id].y + d[1] }])), 'nudge');
      return done();
    }
    const here = pointer.x || pointer.y ? toBoard(view, pointer) : viewCentre();
    if (k === 'n' || k === 'N') { add('note', {}, { at: here, edit: true }); return done(); }
    if (k === 't' || k === 'T') { add('text', {}, { at: here, edit: true }); return done(); }
    if (k === 'r' || k === 'R') { add('region', { title: '' }, { at: here, edit: true }); return done(); }
    if (k === '1') { fit(); return done(); }
    if (k === '0') { zoom(1 / view.k); return done(); }
    if (k === '+' || k === '=') { zoom(1.25); return done(); }
    if (k === '-') { zoom(0.8); return done(); }
    if (k === ']') { reorder([...selection], true); return done(); }
    if (k === '[') { reorder([...selection], false); return done(); }
    return undefined;
  }
  const keyUp = (e) => { if (e.key === ' ') { space = false; stage.classList.remove('hand'); } };

  // ---- the clipboard, and files dropped ---------------------------------------------------------------

  function copy(e) {
    if (editing || !selection.size) return;
    const b = board(), ids = moving(), set = new Set(ids);
    for (const id of b.order) if (isLink(b.items[id]) && set.has(b.items[id].from) && set.has(b.items[id].to)) set.add(id);
    const part = { schema: b.schema, items: {}, place: {}, order: b.order.filter((id) => set.has(id)) };
    for (const id of part.order) {
      // What is copied without its region is in none.
      const { in: within, ...rest } = b.items[id];
      part.items[id] = within !== undefined && set.has(within) ? b.items[id] : rest;
      if (b.place[id]) part.place[id] = b.place[id];
    }
    e.clipboardData.setData(CLIP, JSON.stringify(part));
    e.clipboardData.setData('text/plain', part.order.map((id) => part.items[id].text || part.items[id].title || '').filter(Boolean).join('\n\n'));
    e.preventDefault();
  }
  function cut(e) { if (editing || !selection.size) return; copy(e); remove(); }

  function pasteItems(part, at) {
    const box = bounds(part), fresh = new Map(part.order.map((id) => [id, makeId()]));
    if (!box) return;
    const ops = [];
    const dx = at.x - (box.x + box.w / 2), dy = at.y - (box.y + box.h / 2);
    for (const old of part.order) {
      const item = { ...part.items[old] };
      if (isLink(item)) continue;
      // What is pasted lies on the layer in the hand.
      delete item.layer;
      if (onLayer) item.layer = onLayer;
      if (item.in !== undefined) { if (fresh.has(item.in)) item.in = fresh.get(item.in); else delete item.in; }
      const p = part.place[old];
      ops.push({ t: 'add', id: fresh.get(old), item, place: { ...p, x: Math.round(p.x + dx), y: Math.round(p.y + dy) } });
    }
    for (const old of part.order) {
      const item = part.items[old];
      if (isLink(item) && fresh.has(item.from) && fresh.has(item.to)) ops.push({ t: 'add', id: fresh.get(old), item: { ...item, from: fresh.get(item.from), to: fresh.get(item.to) } });
    }
    const after = apply(board(), ops).board;
    const ids = ops.filter((o) => o.place).map((o) => o.id);
    commit([...ops, ...membershipOps(after, ids)], 'paste');
    select(ids);
  }

  async function addImage(file, at) {
    let src;
    try {
      src = opt.storeImage ? await opt.storeImage(file) : await new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = () => resolve(reader.result);
        reader.onerror = () => reject(new Error('the image could not be read'));
        reader.readAsDataURL(file);
      });
    } catch (error) { emit('say', error.message); return null; }
    const size = await new Promise((resolve) => {
      const img = new Image();
      img.onload = () => { const k = Math.min(1, 420 / img.naturalWidth); resolve({ w: Math.round(img.naturalWidth * k), h: Math.round(img.naturalHeight * k) }); };
      img.onerror = () => resolve(DEFAULT_SIZE.image);
      img.src = src;
    });
    return add('image', { src, caption: '' }, { at, size });
  }

  function paste(e) {
    if (editing) return;
    const data = e.clipboardData, at = toBoard(view, pointer);
    const own = data.getData(CLIP);
    if (own) {
      try { const part = JSON.parse(own); if (!checkBoard({ ...part, schema: board().schema }).length) { pasteItems(part, at); e.preventDefault(); return; } } catch { /* not ours after all */ }
    }
    const image = [...data.files].find((f) => f.type.startsWith('image/'));
    if (image) { addImage(image, at); e.preventDefault(); return; }
    const words = data.getData('text/plain').trim();
    if (!words) return;
    e.preventDefault();
    // Rows and columns copied from a sheet become a table; anything else a note.
    if (looksTabular(words)) { try { addTable(words, { at }); return; } catch { /* prose after all */ } }
    add('note', { type: 'plain', text: words }, { at });
  }

  // Delimited text laid on the board as a table, under a name no other table has.
  function addTable(text, { name = 'table', title = '', source = '', at = null } = {}) {
    const table = parseDelimited(text, { name: tableName(name, new Set(tables.keys())), title, source });
    const size = { w: Math.min(660, 150 + 88 * Math.min(table.columns.length, 7)), h: 200 };
    return add('table', table, { at, size });
  }

  function drop(e) {
    const all = [...(e.dataTransfer?.files || [])];
    const images = all.filter((f) => f.type.startsWith('image/')), sheets = all.filter((f) => /[.](csv|tsv|txt)$/i.test(f.name) || f.type === 'text/csv');
    if (!images.length && !sheets.length) return;
    e.preventDefault();
    const at = toBoard(view, screenOf(e));
    images.forEach((f, k) => addImage(f, { x: at.x + 30 * k, y: at.y + 30 * k }));
    sheets.forEach(async (f, k) => {
      try { addTable(await f.text(), { name: f.name, title: f.name, at: { x: at.x + 30 * k, y: at.y + 30 * k } }); } catch (error) { emit('say', `${f.name} is not a table: ${error.message}.`); }
    });
  }

  // ---- the view -----------------------------------------------------------------------------------

  function fit(ids = null) { setView(fitView(bounds(board(), ids && ids.length ? ids : null), stageSize(), { inset: inset() })); }
  function zoom(factor) { setView(zoomAbout(view, factor, clearCentre())); }
  function undo() { const what = history.undo(); if (what) { changed('undo'); select([...selection]); emit('say', `Undone: ${what}`); } }
  function redo() { const what = history.redo(); if (what) { changed('redo'); select([...selection]); emit('say', `Done again: ${what}`); } }

  function load(next, { keepView = false } = {}) {
    const problems = checkBoard(next);
    if (problems.length) throw new Error(`this is not a board that can be opened: ${problems.slice(0, 3).join('; ')}`);
    if (editing) endEdit();
    history.reset(next);
    selection.clear();
    for (const rec of recs.values()) { release(rec.el); rec.el.remove(); }
    recs.clear();
    onLayer = null;
    refresh();
    fitHeights();
    if (!keepView) fit();
    emit('select', []);
  }

  stage.addEventListener('pointerdown', down);
  stage.addEventListener('pointermove', move);
  stage.addEventListener('pointerup', up);
  stage.addEventListener('pointercancel', (e) => { touches.delete(e.pointerId); cancelGesture(); });
  stage.addEventListener('dblclick', dbl);
  stage.addEventListener('wheel', wheel, { passive: false });
  stage.addEventListener('contextmenu', (e) => e.preventDefault());
  host.addEventListener('keydown', key);
  host.addEventListener('keyup', keyUp);
  host.addEventListener('blur', () => { space = false; stage.classList.remove('hand'); });
  host.addEventListener('copy', copy);
  host.addEventListener('cut', cut);
  host.addEventListener('paste', paste);
  host.addEventListener('dragover', (e) => { if ([...(e.dataTransfer?.types || [])].includes('Files')) e.preventDefault(); });
  host.addEventListener('drop', drop);
  const resized = new ResizeObserver(() => applyView());
  resized.observe(stage);
  // Type that arrives late changes what notes measure.
  document.fonts?.ready.then(() => { if (fitHeights()) emit('change', board(), 'type'); });

  refresh();
  fitHeights();
  applyView();

  return {
    get board() { return board(); },
    get view() { return view; },
    get selection() { return [...selection]; },
    get canUndo() { return history.canUndo; },
    get canRedo() { return history.canRedo; },
    get values() { return values; },
    get types() { return [...types.values()]; },
    get tables() { return tables; },
    get cards() { return cards; },
    get services() { return opt.services; },
    get tool() { return tool; },
    setTool, addTable,
    say(words) { emit('say', words); },
    // Layers, back to front, above the ground; and the one new items are laid on (null: the ground).
    get layers() { return board().layers || []; },
    get layer() { return onLayer; },
    setLayer(id) { onLayer = id && (board().layers || []).some((l) => l.id === id) ? id : null; emit('select', [...selection]); },
    setLayers(layers, label = 'change the layers') { commit([{ t: 'layers', layers }], label); },
    addLayer(name) {
      const layer = { id: makeId(), name: name || `Layer ${(board().layers || []).length + 1}` };
      commit([{ t: 'layers', layers: [...(board().layers || []), layer] }], 'add a layer');
      onLayer = layer.id;
      emit('select', [...selection]);
      return layer.id;
    },
    removeLayer(id) { commit(removeLayerOps(board(), id), 'remove a layer'); },
    toLayer(ids, layer) { commit(ids.filter((id) => board().items[id] && !isLink(board().items[id])).map((id) => ({ t: 'set', id, patch: { layer: layer || null } })), 'move to a layer'); },
    card(id) { const item = board().items[id]; return item?.kind === 'card' ? cardCtx(id, item) : null; },
    run(id) { const c = this.card(id); return c ? runModule(c) : null; },
    on(name, fn) { listeners[name].add(fn); return () => listeners[name].delete(fn); },
    load, commit, select, add, link, remove, duplicate, place, reorder, insert, startEdit, undo, redo, fit, zoom, setView, addImage,
    set(id, patch, label = 'change') { commit([{ t: 'set', id, patch }], label); },
    title(title) { if (title !== board().title) commit([{ t: 'title', title }], 'rename the board'); },
    centre: viewCentre,
    elements: { host, stage, world },
    focus() { host.focus({ preventScroll: true }); },
    destroy() { for (const rec of recs.values()) release(rec.el); resized.disconnect(); stage.remove(); host.classList.remove('bd-host'); },
  };
}
