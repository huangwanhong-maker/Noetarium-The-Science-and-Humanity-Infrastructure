// How each kind of item is drawn (DOM). An item's element is filled from its content alone and from
// what the board computes for it (the values of its quantities, whether a region is empty); its place
// is the view's concern. An element marked data-edit holds text that is edited where it stands.

import { h, s, clear } from './dom.js';
import { parseSchedule, resolveSchedule, layoutPlan, layoutChronicle, formatDay, parseDate, spanText } from './schedule.js';
import { formatNumber, formatValue } from './quantities.js';
import { strokeOutline } from './geometry.js';

// How an item's height follows its content: 'grow' (never lower than its content), 'exact' (as high as
// its content), or not at all. A card says for itself.
const HEIGHT = { note: 'grow', text: 'grow', shape: 'grow', figure: 'grow', schedule: 'exact', table: 'exact' };
export const heightMode = (ctx, item) => (item.kind === 'card' ? ctx.cards[item.card]?.height || null : HEIGHT[item.kind] || null);

// What an item's drawing depends on beyond its own content.
export function signature(ctx, id, item) {
  if (item.kind === 'region') return ctx.hasMembers(id) ? 'held' : 'empty';
  if (item.kind === 'note' || item.kind === 'figure') {
    return (ctx.definitions.get(id) || []).map((d) => { const v = ctx.values.get(d.name); return v?.item === id ? formatValue(v) : `!${d.name}`; }).join('|')
      + (item.kind === 'note' ? `/${ctx.types.get(item.type)?.label || ''}` : '');
  }
  if (item.kind === 'schedule') return `${ctx.today ?? ''}/${ctx.widthOf(id)}`;
  if (item.kind === 'card') { const card = ctx.cards[item.card]; return card ? `${ctx.widthOf(id)}x${ctx.heightOf(id)}/${card.signature ? card.signature(ctx.card(id, item)) : ''}` : 'none'; }
  return '';
}

// What a card left to be released when it is drawn again or taken away: a scene, a clock.
export function release(el) {
  if (!el._release) return;
  try { el._release(); } catch { /* it is gone either way */ }
  el._release = null;
}

export function draw(ctx, el, id, item) {
  release(el);
  clear(el);
  el.className = `bd-item bd-${item.kind}`;
  DRAW[item.kind](ctx, el, id, item);
}

const tone = (el, name) => { if (name) el.classList.add(`tone-${name}`); };

// A range drawn as a bar with its likely value marked: where the value lies between its bounds.
function rangeBar(v) {
  const at = v.hi > v.lo ? ((v.mid - v.lo) / (v.hi - v.lo)) * 100 : 50;
  return h('div', { class: 'bd-range', title: 'the range the value is believed to lie in' },
    h('span', { class: 'bd-range-end' }, formatNumber(v.lo, v)), h('i', {}, h('b', { style: `left:${at.toFixed(1)}%` })), h('span', { class: 'bd-range-end' }, formatNumber(v.hi, v)));
}

const DRAW = {
  note(ctx, el, id, item) {
    const type = ctx.types.get(item.type) || ctx.types.get('plain');
    tone(el, item.tone || type.tone);
    el.append(h('div', { class: 'bd-kind' }, h('i', {}, type.glyph), type.label));
    el.append(h('div', { class: 'bd-words', 'data-edit': 'text', 'data-placeholder': 'Write…' }, item.text || ''));
    // The quantities its lines define, with their values.
    const defs = ctx.definitions.get(id) || [];
    if (defs.length) {
      el.append(h('div', { class: 'bd-chips' }, defs.map((d) => {
        const v = ctx.values.get(d.name);
        const mine = v && v.item === id;
        if (!mine || v.error) return h('div', { class: 'bd-chip wrong', title: v?.error || `'${d.name}' is defined more than once` }, h('b', {}, d.name), ' ', mine ? v.error : 'defined elsewhere too');
        return h('div', { class: 'bd-chip', title: 'a quantity other items can use by its name' }, h('b', {}, d.name), ' ', formatValue(v));
      })));
    }
    const when = item.when ? parseDate(item.when) : null;
    const foot = [item.source && h('span', { class: 'bd-source', title: 'its source' }, item.source), item.when && h('span', { class: when ? 'bd-when' : 'bd-when wrong' }, when ? formatDay(when.day, when.precision) : `${item.when}?`)].filter(Boolean);
    if (foot.length) el.append(h('div', { class: 'bd-foot' }, foot));
  },

  text(ctx, el, id, item) {
    el.classList.add(`size-${item.size || 'body'}`);
    el.append(h('div', { class: 'bd-words', 'data-edit': 'text', 'data-placeholder': 'Text' }, item.text || ''));
  },

  image(ctx, el, id, item) {
    el.append(item.src ? h('img', { src: item.src, alt: item.caption || '', draggable: 'false' }) : h('div', { class: 'bd-missing' }, 'no image'));
    if (item.caption) el.append(h('div', { class: 'bd-caption' }, item.caption));
  },

  region(ctx, el, id, item) {
    tone(el, item.tone);
    el.append(h('div', { class: 'bd-region-title', 'data-edit': 'title', 'data-placeholder': 'Name this region' }, item.title || ''));
    if (item.hint && !ctx.hasMembers(id)) el.append(h('div', { class: 'bd-hint' }, item.hint));
  },

  shape(ctx, el, id, item) {
    tone(el, item.tone || 'paper');
    const form = item.form || 'rect';
    const outline = {
      rect: s('rect', { x: 1, y: 1, width: 98, height: 98 }),
      pill: s('rect', { x: 1, y: 1, width: 98, height: 98, rx: 49, ry: 49 }),
      ellipse: s('ellipse', { cx: 50, cy: 50, rx: 49, ry: 49 }),
      diamond: s('path', { d: 'M50 1L99 50L50 99L1 50Z' }),
    }[form] || s('rect', { x: 1, y: 1, width: 98, height: 98 });
    outline.setAttribute('vector-effect', 'non-scaling-stroke');
    el.classList.add(`form-${form}`);
    el.append(s('svg', { class: 'bd-outline', viewBox: '0 0 100 100', preserveAspectRatio: 'none' }, outline),
      h('div', { class: 'bd-words', 'data-edit': 'text', 'data-placeholder': '' }, item.text || ''));
  },

  figure(ctx, el, id, item) {
    if (item.title) el.append(h('div', { class: 'bd-kind' }, item.title));
    const defs = ctx.definitions.get(id) || [];
    if (!defs.length) el.append(h('div', { class: 'bd-hint' }, 'A figure computes from the quantities on the board. Write a formula: TAM = population * spend'));
    for (const d of defs) {
      const v = ctx.values.get(d.name);
      const mine = v && v.item === id;
      if (!mine || v.error) {
        el.append(h('div', { class: 'bd-result wrong' }, h('span', { class: 'bd-name' }, d.name), h('span', { class: 'bd-reason' }, mine ? v.error : 'defined elsewhere too')));
        continue;
      }
      const rests = v.rests.filter((n) => n !== d.name);
      el.append(h('div', { class: 'bd-result' },
        h('span', { class: 'bd-name' }, d.name),
        h('span', { class: 'bd-value' }, formatNumber(v.mid, v), v.unit ? h('small', {}, ` ${v.unit}`) : null),
        v.hi > v.lo ? rangeBar(v) : null,
        rests.length ? h('span', { class: 'bd-rests', title: 'the values written on the board that this comes from' }, `rests on ${rests.join(', ')}`) : null));
    }
    el.append(h('div', { class: 'bd-formula', 'data-edit': 'formula', 'data-placeholder': 'name = expression' }, item.formula || ''));
  },

  schedule(ctx, el, id, item) {
    const parsed = parseSchedule(item.source), resolved = resolveSchedule(parsed.tasks);
    const problems = [...parsed.errors, ...resolved.errors];
    const head = h('div', { class: 'bd-schedule-head' }, h('span', { class: 'bd-schedule-title', 'data-edit': 'title', 'data-placeholder': 'Title' }, item.title || ''));
    // The span is written no finer than its entries are: years for a chronicle of years.
    const fine = resolved.tasks.map((t) => t.precision || 'day');
    const precision = fine.every((p) => p === 'year') ? 'year' : fine.some((p) => p === 'day') ? 'day' : 'month';
    if (resolved.span) head.append(h('span', { class: 'bd-schedule-span' }, `${formatDay(resolved.span.start, precision)} – ${formatDay(Math.max(resolved.span.start, resolved.span.end - 1), precision)}`));
    if (problems.length) head.append(h('span', { class: 'bd-schedule-wrong', title: problems.map((p) => `line ${p.line}: ${p.message}`).join('\n') }, `${problems.length} ${problems.length === 1 ? 'line' : 'lines'} not read`));
    el.append(head);
    if (!resolved.tasks.length) { el.append(h('div', { class: 'bd-hint' }, 'Write its entries in the panel, a line each: Name : 2026-11-16, 10d')); return; }
    const width = Math.max(240, (ctx.widthOf(id) || 760) - 2);
    el.append(item.mode === 'chronicle' ? drawChronicle(layoutChronicle(resolved, { width })) : drawPlan(layoutPlan(resolved, { width, today: ctx.today })));
  },

  // A stroke of the pen, drawn at the size it was made and stretched with its rectangle.
  ink(ctx, el, id, item) {
    const [w, hgt] = item.size || [1, 1];
    el.classList.add(`ink-${item.colour || 'ink'}`);
    if (item.marker) el.classList.add('marker');
    el.append(s('svg', { viewBox: `0 0 ${w} ${hgt}`, preserveAspectRatio: 'none' }, s('path', { d: strokeOutline(item.points || [], item.width || 3) })));
  },

  // A table: its name, by which other items draw it, and its first rows.
  table(ctx, el, id, item) {
    const twice = ctx.tables.get(item.name)?.error;
    el.append(h('div', { class: 'bd-table-head' }, h('span', { class: 'bd-name', title: 'the name other items draw this table by' }, item.name),
      h('span', { class: 'bd-table-title', 'data-edit': 'title', 'data-placeholder': 'What it is' }, item.title || ''),
      h('span', { class: 'bd-table-size' }, `${item.rows.toLocaleString('en-GB')} × ${item.columns.length}`)));
    if (twice) el.append(h('div', { class: 'bd-hint wrong' }, twice));
    const columns = item.columns.slice(0, 7), rows = Math.min(item.rows, 6);
    const cell = (c, v) => h('td', { class: c.kind === 'number' ? 'n' : null }, v === null || v === undefined ? '' : c.kind === 'number' ? formatNumber(v) : String(v).length > 26 ? `${String(v).slice(0, 25)}…` : String(v));
    el.append(h('table', { class: 'bd-rows' },
      h('thead', {}, h('tr', {}, columns.map((c) => h('th', { class: c.kind === 'number' ? 'n' : null, title: c.label || null }, c.name)), item.columns.length > columns.length ? h('th', { class: 'more' }, `+${item.columns.length - columns.length}`) : null)),
      h('tbody', {}, Array.from({ length: rows }, (_, r) => h('tr', {}, columns.map((c) => cell(c, item.data[c.name]?.[r])))))));
    const foot = [item.rows > rows ? `and ${(item.rows - rows).toLocaleString('en-GB')} more rows` : null, item.source || null].filter(Boolean);
    if (foot.length) el.append(h('div', { class: 'bd-table-foot' }, foot.join(' · ')));
  },

  card(ctx, el, id, item) {
    const card = ctx.cards[item.card];
    if (!card) { el.append(h('div', { class: 'bd-kind' }, item.card || 'card'), h('div', { class: 'bd-hint' }, `The card '${item.card}' is not installed in this work, so it cannot be shown here. Its content is kept.`)); return; }
    el.classList.add(`card-${item.card}`);
    el.append(h('div', { class: 'bd-kind bd-card-head' }, card.label || item.card));
    const body = h('div', { class: 'bd-card-body' });
    el.append(body);
    try {
      const done = card.render(body, ctx.card(id, item));
      if (typeof done === 'function') el._release = done;
    } catch (error) {
      body.append(h('div', { class: 'bd-hint wrong' }, `This card could not be drawn: ${error.message}`));
    }
  },
};

function axis(layout, height) {
  const g = s('g', { class: 'bd-axis' });
  for (const t of layout.ticks) {
    g.append(s('path', { class: t.major ? 'bd-tick major' : 'bd-tick', d: `M${t.x.toFixed(1)} 22V${height - 6}` }),
      s('text', { x: (t.x + 4).toFixed(1), y: 15 }, t.label));
  }
  g.append(s('path', { class: 'bd-axis-line', d: `M${layout.plot.x} 22H${layout.plot.x + layout.plot.w}` }));
  return g;
}

export function drawPlan(layout) {
  const svg = s('svg', { class: 'bd-chart', width: layout.width, height: layout.height, viewBox: `0 0 ${layout.width} ${layout.height}` });
  svg.append(axis(layout, layout.height));
  for (const g of layout.groups) svg.append(s('text', { class: 'bd-group', x: 4, y: g.y + 15 }, g.name));
  for (const j of layout.joins) svg.append(s('path', { class: j.critical ? 'bd-join critical' : 'bd-join', d: j.d }));
  for (const r of layout.rows) {
    const t = r.task, cls = `${t.critical ? ' critical' : ''}`;
    const title = `${t.name}: ${formatDay(t.start)}${t.end > t.start ? ` – ${formatDay(t.end - 1)}, ${spanText(t.end - t.start)}` : ''}${t.float ? `; could slip ${spanText(t.float)}` : t.critical ? '; critical' : ''}`;
    svg.append(s('text', { class: 'bd-row-name', x: t.group ? 14 : 4, y: r.mid + 4 }, t.name));
    if (t.milestone) {
      const m = 6.5;
      svg.append(s('path', { class: `bd-milestone${cls}`, d: `M${r.x.toFixed(1)} ${r.mid - m}l${m} ${m}l${-m} ${m}l${-m} ${-m}Z` }, s('title', {}, title)));
    } else {
      svg.append(s('rect', { class: `bd-bar${cls}`, x: r.x.toFixed(1), y: r.y + 6, width: r.w.toFixed(1), height: 14, rx: 2 }, s('title', {}, title)));
      if (t.progress) svg.append(s('rect', { class: 'bd-progress', x: r.x.toFixed(1), y: r.y + 6, width: (r.w * t.progress).toFixed(1), height: 14, rx: 2 }));
    }
  }
  if (layout.today !== null && layout.today !== undefined) svg.append(s('path', { class: 'bd-today', d: `M${layout.today.toFixed(1)} 18V${layout.height - 4}` }, s('title', {}, 'today')));
  return svg;
}

export function drawChronicle(layout) {
  const svg = s('svg', { class: 'bd-chart', width: layout.width, height: layout.height, viewBox: `0 0 ${layout.width} ${layout.height}` });
  svg.append(axis(layout, layout.height));
  for (const e of layout.events) {
    const t = e.task;
    const when = t.end > t.start + 1 ? `${formatDay(t.start, t.precision)} – ${formatDay(t.end - 1, t.precision)}` : formatDay(t.start, t.precision);
    svg.append(s('path', { class: 'bd-drop', d: `M${e.x.toFixed(1)} 22V${e.mid}` }));
    if (e.w > 3) svg.append(s('rect', { class: 'bd-bar', x: e.x.toFixed(1), y: e.mid - 4, width: e.w.toFixed(1), height: 8, rx: 2 }, s('title', {}, `${t.name}: ${when}`)));
    else svg.append(s('circle', { class: 'bd-event', cx: e.x.toFixed(1), cy: e.mid, r: 4.2 }, s('title', {}, `${t.name}: ${when}`)));
    svg.append(s('text', { class: 'bd-row-name', x: e.labelX.toFixed(1), y: e.mid + 4, 'text-anchor': e.anchor }, t.name));
  }
  return svg;
}
