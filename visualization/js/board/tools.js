// The instruments beside a board (DOM): a rail from which items are added and the pen is taken up, a
// panel that shows and changes what is selected, and a template drawn small for a list to choose
// from. A work places them where it likes; each speaks to the board only through its interface.
//
// The panel of a card is built from what the card declares (its form: the parameters it needs, each
// with its kind and what it names), as an inspector is built from a component's fields: a plug-in
// says what it needs, and the board asks for it.

import { h, s, clear } from './dom.js';
import { RELATIONS, TONES, isLink, members } from './model.js';
import { tidy, tidyRegions, tree, byTime, align } from './arrange.js';
import { parseSchedule, resolveSchedule, parseDate, formatDay, spanText } from './schedule.js';
import { formatValue } from './quantities.js';
import { thumbnail } from './templates.js';
import { columnNames, findColumn, toDelimited } from './tables.js';

const icon = (d, extra = {}) => s('svg', { viewBox: '0 0 20 20', class: 'bd-icon', 'aria-hidden': 'true', ...extra }, s('path', { d }));
const ICONS = {
  hand: 'M5 3l9 7-4 .8 2.4 4.6-1.6.8L8.4 11.6 5.6 14z',
  pen: 'M3 17l1-4 9-9 3 3-9 9zM12 5l3 3',
  rubber: 'M8 16h8M4.5 12.5l7-7 4 4-6.5 6.5H6z',
  text: 'M4 5h12M10 5v11M7.5 16h5',
  region: 'M3 4h14v12H3zM3 7.5h14',
  shape: 'M3 10l7-6 7 6-7 6z',
  image: 'M3 4h14v12H3zM3 13l4-4 3 3 3-2 4 3',
  figure: 'M4 16V9m4 7V4m4 12v-5m4 5V7',
  plan: 'M3 5h8M6 10h9M9 15h8',
  chronicle: 'M2 10h16M5 10V6m5 4v4m5-4V6',
  table: 'M3 4h14v12H3zM3 8h14M3 12h14M8 4v12',
  chart: 'M3 17h14M5 14l4-5 3 3 4-7',
  map: 'M10 17s5-5 5-9a5 5 0 00-10 0c0 4 5 9 5 9zM10 6.5a1.6 1.6 0 100 3.2 1.6 1.6 0 000-3.2z',
  module: 'M7 5L3 10l4 5M13 5l4 5-4 5M11 4l-2 12',
  eye: 'M2 10s3-5 8-5 8 5 8 5-3 5-8 5-8-5-8-5zM10 7.6a2.4 2.4 0 100 4.8 2.4 2.4 0 000-4.8z',
  up: 'M10 16V5M5 9l5-5 5 5',
  down: 'M10 4v11M5 11l5 5 5-5',
  cross: 'M5 5l10 10M15 5L5 15',
  shut: 'M3 4l14 12M2 10s3-5 8-5c1.4 0 2.6.4 3.7.9M18 10s-3 5-8 5c-1.4 0-2.6-.4-3.7-.9',
  lock: 'M5 9h10v8H5zM7 9V6a3 3 0 016 0v3',
};
const GLYPHS = { text: 'T', image: '▧', region: '▭', shape: '◇', schedule: '≡', figure: '∑', ink: '∿', table: '▦', card: '◫', link: '→' };

// ---- the rail ---------------------------------------------------------------------------------------

const PEN_COLOURS = ['ink', 'rose', 'lilac', 'sky', 'sage', 'gold', 'clay'];

export function createToolbar(api, host) {
  host.classList.add('bd-rail');
  const tool = (label, about, node, act, extra = {}) => h('button', { type: 'button', class: 'bd-tool', title: about, 'aria-label': label, 'data-label': label, onclick: () => { act(); }, ...extra }, node);
  const image = h('input', { type: 'file', accept: 'image/*', multiple: true, hidden: true, onchange: () => { for (const f of image.files) api.addImage(f, api.centre()); image.value = ''; } });
  const sheet = h('input', { type: 'file', accept: '.csv,.tsv,.txt,text/csv', hidden: true, onchange: async () => {
    for (const f of sheet.files) { try { api.addTable(await f.text(), { name: f.name, title: f.name }); } catch (error) { api.say(`${f.name} is not a table: ${error.message}.`); } }
    sheet.value = '';
  } });

  // The pen's colours and widths, shown while it is in the hand.
  const strip = h('div', { class: 'bd-pen-strip', role: 'group', 'aria-label': 'The pen' });
  function drawStrip() {
    clear(strip);
    const t = api.tool;
    strip.hidden = t.name !== 'pen';
    if (strip.hidden) return;
    for (const c of PEN_COLOURS) strip.append(h('button', { type: 'button', class: `bd-pen-colour ink-${c}${t.colour === c ? ' on' : ''}`, title: c, 'aria-label': `Colour: ${c}`, 'aria-pressed': String(t.colour === c), onclick: () => api.setTool({ colour: c }) }));
    strip.append(h('span', { class: 'bd-strip-rule' }));
    for (const [w, name] of [[2, 'fine'], [4, 'medium'], [9, 'broad']]) {
      strip.append(h('button', { type: 'button', class: `bd-pen-width${t.width === w && !t.marker ? ' on' : ''}`, title: name, 'aria-label': `Width: ${name}`, onclick: () => api.setTool({ width: w, marker: false }) }, h('i', { style: `height:${Math.max(1.5, w * 0.8)}px` })));
    }
    strip.append(h('button', { type: 'button', class: `bd-pen-width marker${t.marker ? ' on' : ''}`, title: 'a marker: broad and clear, to go over words', 'aria-label': 'Marker', onclick: () => api.setTool({ width: 16, marker: true }) }, h('i', { style: 'height:9px' })));
  }
  const hands = {
    select: tool('Hand', 'Select and move (V)', icon(ICONS.hand), () => api.setTool({ name: 'select' })),
    pen: tool('Pen', 'Draw with a mouse or a stylus; fingers still move the board (P)', icon(ICONS.pen), () => api.setTool({ name: api.tool.name === 'pen' ? 'select' : 'pen' })),
    rubber: tool('Rubber', 'Take strokes away (E)', icon(ICONS.rubber), () => api.setTool({ name: api.tool.name === 'rubber' ? 'select' : 'rubber' })),
  };
  const reflect = () => { for (const [name, key] of Object.entries(hands)) { key.classList.toggle('on', api.tool.name === name); key.setAttribute('aria-pressed', String(api.tool.name === name)); } drawStrip(); };
  api.on('tool', reflect);

  host.append(
    h('div', { class: 'bd-rail-group', role: 'group', 'aria-label': 'The hand' }, hands.select, hands.pen, hands.rubber, strip),
    h('div', { class: 'bd-rail-group slips', role: 'group', 'aria-label': 'Notes' }, api.types.map((t) =>
      h('button', { type: 'button', class: `bd-tool bd-slip tone-${t.tone}`, title: `${t.label}${t.about ? `: ${t.about}` : ''}`, 'aria-label': `Add: ${t.label}`, 'data-label': t.label,
        onclick: () => api.add('note', { type: t.id }, { edit: true }) }, h('i', {}, t.glyph)))),
    h('div', { class: 'bd-rail-group', role: 'group', 'aria-label': 'Words and pictures' },
      tool('Text', 'Words on the board itself: a title, a heading, a remark (T)', icon(ICONS.text), () => api.add('text', {}, { edit: true })),
      tool('Region', 'A named part of the board; what lies in it moves with it (R)', icon(ICONS.region), () => api.add('region', {}, { edit: true })),
      tool('Shape', 'A box, an ellipse or a diamond, for a diagram', icon(ICONS.shape), () => api.add('shape', {}, { edit: true })),
      tool('Image', 'A picture from this computer; one can also be dropped or pasted', icon(ICONS.image), () => image.click()), image),
    h('div', { class: 'bd-rail-group', role: 'group', 'aria-label': 'Reckoning and time' },
      tool('Figure', 'A number computed from the quantities written on the notes', icon(ICONS.figure), () => api.add('figure', {}, { edit: true })),
      tool('Plan', 'Tasks on a line of time, a Gantt chart', icon(ICONS.plan), () => api.add('schedule', { mode: 'plan', title: 'Plan', source: 'First task : 2026-11-16, 5d\nSecond task : after First task, 1w' })),
      tool('Chronicle', 'Events on a line of time', icon(ICONS.chronicle), () => api.add('schedule', { mode: 'chronicle', title: 'Chronicle', source: '2026-01 : An event\n2026-06..2026-09 : A period' }, { size: { w: 780, h: 160 } }))),
    h('div', { class: 'bd-rail-group', role: 'group', 'aria-label': 'Data' },
      tool('Table', 'A table from a file of rows and columns; one can also be dropped, or pasted from a sheet', icon(ICONS.table), () => sheet.click()), sheet,
      tool('Chart', 'A table drawn as bars, lines or points', icon(ICONS.chart), () => api.add('card', { card: 'chart', props: { table: [...api.tables.keys()][0] || '' } })),
      tool('Map', 'A table\'s places on the Earth', icon(ICONS.map), () => api.add('card', { card: 'map', props: { table: [...api.tables].find(([, t]) => !t.error && (findColumn(t, 'lat', 'latitude') || findColumn(t, 'iso3')))?.[0] || '' } })),
      tool('Script', 'A few lines of Python run on the board\'s tables, quantities and notes', icon(ICONS.module), () => api.add('card', { card: 'module', props: { code: 'def run(board, api):\n    return [api.note(f"{len(board.tables)} tables, {len(board.quantities)} quantities")]' } }))));
  reflect();
  return host;
}

// ---- the panel --------------------------------------------------------------------------------------

const KIND_NAMES = { note: 'Note', text: 'Text', image: 'Image', region: 'Region', shape: 'Shape', link: 'Link', schedule: 'Plan or chronicle', figure: 'Figure', ink: 'A stroke of the pen', table: 'Table', card: 'Card' };

export function createInspector(api, host) {
  host.classList.add('bd-panel');
  let live = null;          // redraws the part of the panel that follows the board while a field is being typed in
  let timer = null;

  const row = (label, ...control) => h('label', { class: 'bd-row' }, h('span', { class: 'bd-row-label' }, label), ...control);
  const chips = (options, current, pick) => h('div', { class: 'bd-chips-row', role: 'group' }, options.map(([value, label, cls = '', title = null]) =>
    h('button', { type: 'button', class: `bd-pick ${cls}${value === current ? ' on' : ''}`, title, 'aria-pressed': String(value === current), onclick: () => pick(value) }, label)));
  const input = (value, change, placeholder = '') => h('input', { type: 'text', value: value ?? '', placeholder, spellcheck: 'false', onchange: (e) => change(e.target.value.trim()) });
  const area = (value, change, rows, placeholder) => {
    const node = h('textarea', { rows, spellcheck: 'false', placeholder }, value || '');
    const send = () => { clearTimeout(timer); change(node.value); };
    node.addEventListener('input', () => { clearTimeout(timer); timer = setTimeout(send, 320); });
    node.addEventListener('blur', send);
    return node;
  };
  const pick = (options, current, change) => h('select', { onchange: (e) => change(e.target.value) },
    options.map(([value, label]) => h('option', { value, selected: String(value) === String(current ?? '') }, label)));
  const button = (label, act, title = null) => h('button', { type: 'button', class: 'bd-act', title, onclick: act }, label);
  const tones = (id, item) => row('Colour', chips([[null, '—', '', 'the colour of its kind'], ...TONES.map((t) => [t, '', `bd-tone tone-${t}`, t])], item.tone ?? null, (tone) => api.set(id, { tone })));

  function actions(ids) {
    return h('div', { class: 'bd-actions' },
      button('Duplicate', () => api.duplicate(ids), 'Ctrl D'), button('To front', () => api.reorder(ids, true), ']'), button('To back', () => api.reorder(ids, false), '['),
      button('Remove', () => api.remove(ids), 'Delete'));
  }

  // One input for one declared parameter: by its kind, and by what it names.
  function field(f, value, change, values) {
    const tables = api.tables;
    if (f.role === 'table') return pick([['', '—'], ...[...tables.keys()].map((n) => [n, n])], value, change);
    if (f.role === 'column') {
      const t = tables.get(values[f.of || 'table']);
      return pick([['', '—'], ...columnNames(t && !t.error ? t : null).map((n) => [n, n])], value, change);
    }
    if (f.role === 'quantity') return pick([['', '—'], ...[...api.values].filter(([, v]) => !v.error).map(([n]) => [n, n])], value, change);
    if (f.role === 'file') {
      const text = input(value, change, 'data:dataset/file.csv, or a path');
      const file = h('input', { type: 'file', hidden: true, onchange: async () => {
        const [chosen] = file.files;
        if (!chosen) return;
        try { change(await api.services.keep(chosen)); } catch (error) { api.say(`The file was not kept: ${error.message}.`); }
      } });
      return h('span', { class: 'bd-file' }, text, api.services.keep ? button('Choose…', () => file.click(), 'a file of this computer, kept beside the boards') : null, file);
    }
    if (f.kind === 'choice') return pick((f.options || []).map((o) => (Array.isArray(o) ? o : [o, o])), value, change);
    if (f.kind === 'boolean') return h('input', { type: 'checkbox', checked: value === true || value === 'true', onchange: (e) => change(e.target.checked) });
    if (f.kind === 'number' || f.kind === 'integer') return h('input', { type: 'number', value: value ?? '', min: f.min ?? null, max: f.max ?? null, step: f.step ?? (f.kind === 'integer' ? 1 : 'any'), onchange: (e) => change(e.target.value === '' ? null : Number(e.target.value)) });
    if (f.kind === 'code') return area(value, change, 9, f.help || '');
    return input(value, change, f.role === 'columns' ? 'columns, with commas between' : '');
  }

  function cardPanel(id, item) {
    const def = api.cards[item.card], ctx = api.card(id);
    if (!def || !ctx) return [h('p', { class: 'bd-tip' }, `The card '${item.card}' is not installed in this work.`)];
    const fields = def.form ? def.form(ctx) : [];
    const out = [];
    if (def.about) out.push(h('p', { class: 'bd-tip' }, def.about));
    for (const f of fields) {
      const grouped = f.group === 'params';
      const values = grouped ? { ...Object.fromEntries(fields.map((g) => [g.name, g.default])), ...(ctx.props.params || {}) } : ctx.props;
      const change = (v) => {
        const now = api.card(id);
        if (!now) return;
        if (grouped) now.set({ params: { ...(now.props.params || {}), [f.name]: v } }, 'set a parameter');
        else now.set({ [f.name]: v }, 'set a parameter');
      };
      const control = field(f, values[f.name] ?? f.default ?? '', change, values);
      const columns = f.role === 'columns' ? columnNames(api.tables.get(values[f.of || 'table'])) : null;
      out.push(h(f.kind === 'code' ? 'div' : 'label', { class: f.kind === 'code' ? 'bd-row tall' : 'bd-row' }, h('span', { class: 'bd-row-label' }, f.label || f.name), control));
      if (columns?.length) out.push(h('p', { class: 'bd-tip small' }, `It has: ${columns.join(', ')}.`));
      else if (f.help && f.kind !== 'code') out.push(h('p', { class: 'bd-tip small' }, f.help));
    }
    if (item.card === 'module') {
      live = () => { const now = api.card(id); return now?.state?.running ? [h('p', { class: 'bd-tip' }, `Running… ${Math.round((now.state.progress || 0) * 100)}%`)] : now?.out?.error ? [h('p', { class: 'bd-wrong' }, 'It failed; the card says why.')] : []; };
      out.push(h('div', { class: 'bd-actions' }, button(item.out?.ran ? 'Run again' : 'Run', () => api.run(id), 'the module is given the board\'s tables, quantities and notes')));
    }
    return out;
  }

  const KINDS = {
    note(id, item) {
      const when = () => (item.when && !parseDate(item.when) ? h('p', { class: 'bd-wrong' }, 'Not a date: write 2026-11-16, 2026-11 or 2026.') : null);
      return [
        row('Kind', chips(api.types.map((t) => [t.id, [h('i', {}, t.glyph), ` ${t.label}`], `tone-${t.tone}`, t.about || null]), item.type, (type) => api.set(id, { type }))),
        row('Source', input(item.source, (source) => api.set(id, { source: source || null }), 'where this comes from')),
        row('When', input(item.when, (v) => api.set(id, { when: v || null }), '2026-11-16')),
        when(),
        tones(id, item),
        h('p', { class: 'bd-tip small' }, 'A line such as  share = 3% [2%, 5%]  makes a quantity that figures and modules can use by its name.'),
      ];
    },
    text(id, item) {
      return [row('Size', chips([['title', 'Title'], ['heading', 'Heading'], ['body', 'Body'], ['small', 'Small']], item.size || 'body', (size) => api.set(id, { size })))];
    },
    image(id, item) { return [row('Caption', input(item.caption, (caption) => api.set(id, { caption })))]; },
    region(id, item) {
      return [row('Name', input(item.title, (title) => api.set(id, { title }))), tones(id, item),
        h('div', { class: 'bd-actions' }, button('Tidy what is in it', () => api.place(tidyRegions(api.board, { only: [id] }), 'tidy')))];
    },
    shape(id, item) {
      return [row('Form', chips([['rect', 'Box'], ['pill', 'Pill'], ['ellipse', 'Ellipse'], ['diamond', 'Diamond']], item.form || 'rect', (form) => api.set(id, { form }))), tones(id, item)];
    },
    ink(id, item) {
      return [
        row('Colour', chips(PEN_COLOURS.map((c) => [c, '', `bd-pen-colour ink-${c}`, c]), item.colour || 'ink', (colour) => api.set(id, { colour }))),
        row('Width', chips([[2, 'Fine'], [4, 'Medium'], [9, 'Broad'], [16, 'Marker']], item.width, (width) => api.set(id, { width, marker: width === 16 ? true : null }))),
      ];
    },
    table(id, item) {
      const rename = (name) => {
        if (!/^[A-Za-z_][A-Za-z0-9_]*$/.test(name)) return api.say('A table\'s name is letters, digits and _, and does not begin with a digit.');
        if (name !== item.name && api.tables.has(name)) return api.say(`Another table is already named '${name}'.`);
        return api.set(id, { name }, 'name the table');
      };
      const copy = async () => { try { await navigator.clipboard.writeText(toDelimited(item, '\t')); api.say('The table is copied, to paste into a sheet.'); } catch { api.say('The table could not be copied here.'); } };
      return [
        row('Name', input(item.name, rename)), h('p', { class: 'bd-tip small' }, 'Charts, maps and modules draw the table by this name.'),
        row('Title', input(item.title, (title) => api.set(id, { title }))),
        row('Source', input(item.source, (source) => api.set(id, { source }), 'where the data come from')),
        h('p', { class: 'bd-tip small' }, `${item.rows.toLocaleString('en-GB')} rows. ${item.columns.map((c) => `${c.name}${c.kind === 'number' ? '' : ' (text)'}`).join(', ')}.`),
        h('div', { class: 'bd-actions' }, button('Copy for a sheet', copy),
          button('Chart it', () => api.add('card', { card: 'chart', props: { table: item.name } })),
          findColumn(item, 'lat', 'latitude', 'iso3') ? button('Map it', () => api.add('card', { card: 'map', props: { table: item.name } })) : null),
      ];
    },
    link(id, item) {
      return [
        row('Says', chips(Object.entries(RELATIONS).map(([rel, r]) => [rel, r.label || 'arrow', `rel-${rel}`]), item.rel, (rel) => api.set(id, { rel, label: null }))),
        row('Words', input(item.label ?? RELATIONS[item.rel].label, (label) => api.set(id, { label: label === RELATIONS[item.rel].label ? null : label }), 'on the link')),
        h('div', { class: 'bd-actions' }, button('Turn round', () => api.set(id, { from: item.to, to: item.from }, 'turn the link round'))),
      ];
    },
    schedule(id, item) {
      live = () => {
        const now = api.board.items[id];
        if (!now) return null;
        const parsed = parseSchedule(now.source), r = resolveSchedule(parsed.tasks);
        const problems = [...parsed.errors, ...r.errors], out = [];
        if (r.span) {
          const critical = r.tasks.filter((t) => t.critical).map((t) => t.name);
          out.push(h('p', { class: 'bd-tip' }, `${r.tasks.length} ${r.tasks.length === 1 ? 'entry' : 'entries'}, ${formatDay(r.span.start)} to ${formatDay(Math.max(r.span.start, r.span.end - 1))} (${spanText(r.span.end - r.span.start)}).`,
            critical.length ? ` What cannot slip: ${critical.join(' → ')}.` : ''));
        }
        for (const p of problems) out.push(h('p', { class: 'bd-wrong' }, `Line ${p.line}: ${p.message}.`));
        for (const p of r.warnings) out.push(h('p', { class: 'bd-warn' }, `Line ${p.line}: ${p.message}.`));
        return out;
      };
      return [
        row('Drawn as', chips([['plan', 'Plan (Gantt)'], ['chronicle', 'Chronicle']], item.mode, (mode) => api.set(id, { mode }))),
        row('Title', input(item.title, (title) => api.set(id, { title }))),
        h('div', { class: 'bd-row tall' }, h('span', { class: 'bd-row-label' }, 'Entries'),
          area(item.source, (source) => { if (source !== api.board.items[id]?.source) api.set(id, { source }, 'write the schedule'); }, 11, 'Name : 2026-11-16, 10d')),
        h('details', { class: 'bd-help' }, h('summary', {}, 'How entries are written'), h('pre', {},
          '# A group\nName : 2026-11-16, 10d\nName : after Other, 2w, 40%\nName : 2026-12-01..2026-12-19\nName : after A & B, milestone\n1789-07-14 : An event\n480 BC : An event')),
      ];
    },
    figure(id, item) {
      live = () => {
        const out = [];
        for (const [name, v] of api.values) if (v.item === id && !v.error) out.push(h('p', { class: 'bd-tip' }, `${name} = ${formatValue(v)}`));
        return out;
      };
      return [
        row('Title', input(item.title, (title) => api.set(id, { title: title || null }))),
        h('div', { class: 'bd-row tall' }, h('span', { class: 'bd-row-label' }, 'Formulas'),
          area(item.formula, (formula) => { if (formula !== api.board.items[id]?.formula) api.set(id, { formula }, 'write the formula'); }, 5, 'TAM = population * spend')),
        h('p', { class: 'bd-tip small' }, 'A formula uses the names that notes and other figures define, with + − * / ( ) min( ) max( ). Ranges are carried through: the result holds every value its inputs allow.'),
      ];
    },
    card: cardPanel,
  };

  function many(ids) {
    const b = api.board, boxes = ids.filter((id) => b.place[id]);
    const notes = boxes.filter((id) => b.items[id].kind === 'note');
    const out = [h('h3', {}, `${ids.length} items`)];
    if (notes.length === boxes.length && notes.length) {
      out.push(row('Kind', chips(api.types.map((t) => [t.id, [h('i', {}, t.glyph), ` ${t.label}`], `tone-${t.tone}`]), null,
        (type) => api.commit(notes.map((id) => ({ t: 'set', id, patch: { type } })), 'change their kind'))));
    }
    if (boxes.length > 1) {
      const put = (how) => () => api.place(align(api.board, boxes, how), 'align');
      out.push(row('Align', h('div', { class: 'bd-chips-row' }, button('Left', put('left')), button('Centre', put('centre')), button('Right', put('right')),
        button('Top', put('top')), button('Middle', put('middle')), button('Bottom', put('bottom')))));
      out.push(row('Spread', h('div', { class: 'bd-chips-row' }, button('Across', put('across')), button('Down', put('down')))));
      out.push(row('Arrange', h('div', { class: 'bd-chips-row' },
        button('Tidy', () => api.place(tidy(api.board, boxes), 'tidy'), 'into a grid, in reading order'),
        button('As a tree', () => api.place(tree(api.board, { ids: boxes }), 'arrange as a tree'), 'by the links among them'),
        button('By time', () => api.place(byTime(api.board, boxes, (w) => parseDate(w)?.day), 'arrange by time'), 'the notes that say when, in order'))));
    }
    out.push(layerRow(boxes));
    out.push(actions(ids));
    return out;
  }

  // The layers of the board, the front first, above the ground. The one marked is the one new items
  // are laid on; a layer is named where it stands, hidden, held, made faint, moved and removed.
  function layersPanel() {
    const b = api.board, layers = api.layers;
    const count = (id) => b.order.filter((i) => (b.items[i].layer ?? null) === id && !isLink(b.items[i])).length;
    const change = (id, patch, label) => api.setLayers(layers.map((l) => (l.id === id ? Object.fromEntries(Object.entries({ ...l, ...patch }).filter(([, v]) => v !== null && v !== false)) : l)), label);
    const move = (k, by) => { const next = layers.slice(); const [l] = next.splice(k, 1); next.splice(k + by, 0, l); api.setLayers(next, 'reorder the layers'); };
    const flag = (on, d, title, act) => h('button', { type: 'button', class: `bd-flag${on ? ' on' : ''}`, title, 'aria-pressed': String(Boolean(on)), onclick: act }, icon(d));
    const rows = layers.map((l, k) => h('div', { class: `bd-layer${api.layer === l.id ? ' on' : ''}${l.hidden ? ' hidden' : ''}`, 'data-layer': l.id },
      h('button', { type: 'button', class: 'bd-layer-mark', title: 'lay new items on this layer', 'aria-pressed': String(api.layer === l.id), onclick: () => api.setLayer(l.id) }),
      h('input', { type: 'text', value: l.name, spellcheck: 'false', 'aria-label': 'The layer\'s name', onchange: (e) => change(l.id, { name: e.target.value.trim() || l.name }, 'name the layer') }),
      h('span', { class: 'bd-layer-count', title: 'items on it' }, count(l.id)),
      flag(l.hidden, l.hidden ? ICONS.shut : ICONS.eye, l.hidden ? 'show the layer' : 'hide the layer', () => change(l.id, { hidden: l.hidden ? null : true }, 'hide or show a layer')),
      flag(l.locked, ICONS.lock, l.locked ? 'let the layer move' : 'hold the layer where it is', () => change(l.id, { locked: l.locked ? null : true }, 'hold or free a layer')),
      h('button', { type: 'button', class: `bd-flag faint${(l.opacity ?? 1) < 1 ? ' on' : ''}`, title: 'how clearly it is drawn: 100, 60 or 30 in a hundred',
        onclick: () => change(l.id, { opacity: (l.opacity ?? 1) > 0.9 ? 0.6 : (l.opacity ?? 1) > 0.5 ? 0.3 : null }, 'make a layer faint or clear') }, `${Math.round((l.opacity ?? 1) * 100)}`),
      flag(false, ICONS.up, 'bring the layer forward', () => { if (k < layers.length - 1) move(k, 1); }),
      flag(false, ICONS.down, 'send the layer back', () => { if (k > 0) move(k, -1); }),
      flag(false, ICONS.cross, 'remove the layer; what lies on it is left on the ground', () => api.removeLayer(l.id)))).reverse();
    return h('div', { class: 'bd-layers' }, h('h4', {}, 'Layers'), rows,
      h('div', { class: `bd-layer${api.layer === null ? ' on' : ''}` }, h('button', { type: 'button', class: 'bd-layer-mark', title: 'lay new items on the ground', 'aria-pressed': String(api.layer === null), onclick: () => api.setLayer(null) }),
        h('span', { class: 'bd-layer-ground' }, 'The ground'), h('span', { class: 'bd-layer-count' }, count(null))),
      h('div', { class: 'bd-chips-row', style: 'margin-top:5px' }, button('New layer', () => api.addLayer(), 'a layer in front of the others; new items are then laid on it')));
  }
  // The layers an item, or several, may be moved to.
  const layerRow = (ids) => (api.layers.length ? row('Layer', pick([['', 'The ground'], ...api.layers.map((l) => [l.id, l.name])],
    ids.every((i) => api.board.items[i]?.layer === api.board.items[ids[0]]?.layer) ? api.board.items[ids[0]]?.layer ?? '' : '', (layer) => api.toLayer(ids, layer || null))) : null);

  // The contents of the board, as its regions hold them: each item by its kind and its first words.
  // A row selects its item and brings it into view; an item can be hidden, or held where it is.
  function contents() {
    const b = api.board;
    const say = (id) => {
      const item = b.items[id];
      const described = item.kind === 'card' && item.props?.module ? (api.services.modules || []).find((m) => m.ref === item.props.module)?.title : null;
      const words = item.kind === 'note' ? item.text : item.kind === 'card' ? item.props?.title || described || api.cards[item.card]?.label || item.card
        : item.kind === 'table' ? item.name : item.kind === 'ink' ? 'a stroke' : item.title || item.text || item.caption || item.formula || '';
      return String(words || '').split('\n')[0].slice(0, 40) || KIND_NAMES[item.kind];
    };
    const glyph = (item) => (item.kind === 'note' ? api.types.find((t) => t.id === item.type)?.glyph || '·' : GLYPHS[item.kind] || '·');
    const line = (id, depth) => {
      const item = b.items[id];
      const flag = (name, d, title) => h('button', { type: 'button', class: `bd-flag${item[name] ? ' on' : ''}`, title, 'aria-pressed': String(Boolean(item[name])),
        onclick: (e) => { e.stopPropagation(); api.set(id, { [name]: item[name] ? null : true }, title); } }, icon(d));
      return h('div', { class: `bd-content${item.hidden ? ' hidden' : ''}`, style: `padding-left:${4 + depth * 12}px` },
        h('button', { type: 'button', class: 'bd-content-name', onclick: () => { api.select([id]); if (!item.hidden) api.fit([id]); } }, h('i', {}, glyph(item)), say(id)),
        flag('hidden', item.hidden ? ICONS.shut : ICONS.eye, item.hidden ? 'show it' : 'hide it'), flag('locked', ICONS.lock, item.locked ? 'let it move' : 'hold it where it is'));
    };
    const rows = [];
    const walk = (id, depth) => {
      rows.push(line(id, depth));
      if (b.items[id].kind === 'region') for (const m of members(b, id).slice().reverse()) if (b.items[m].kind !== 'ink') walk(m, depth + 1);
    };
    // The front of the board first, as it is seen. Strokes of the pen are counted, not listed.
    const top = b.order.filter((id) => !isLink(b.items[id]) && b.items[id].in === undefined).reverse();
    const strokes = b.order.filter((id) => b.items[id].kind === 'ink');
    if (!api.layers.length) { for (const id of top) if (b.items[id].kind !== 'ink') walk(id, 0); }
    else {
      for (const l of [...api.layers.slice().reverse(), { id: undefined, name: 'The ground' }]) {
        const here = top.filter((id) => b.items[id].layer === l.id && b.items[id].kind !== 'ink');
        if (!here.length) continue;
        rows.push(h('h5', {}, l.name));
        for (const id of here) walk(id, 0);
      }
    }
    if (!rows.length && !strokes.length) return null;
    return h('details', { class: 'bd-contents', open: rows.length <= 24 }, h('summary', {}, 'Contents'), rows,
      strokes.length ? h('p', { class: 'bd-tip small' }, `and ${strokes.length} ${strokes.length === 1 ? 'stroke' : 'strokes'} of the pen.`) : null);
  }

  function nothing() {
    const b = api.board;
    const count = (kind) => b.order.filter((id) => b.items[id].kind === kind).length;
    const words = [['note', 'note', 'notes'], ['region', 'region', 'regions'], ['link', 'link', 'links'], ['text', 'text', 'texts'], ['shape', 'shape', 'shapes'], ['image', 'image', 'images'],
      ['figure', 'figure', 'figures'], ['schedule', 'plan or chronicle', 'plans and chronicles'], ['table', 'table', 'tables'], ['card', 'card', 'cards'], ['ink', 'stroke', 'strokes']];
    const parts = words.map(([k, one, more]) => [count(k), one, more]).filter(([n]) => n).map(([n, one, more]) => `${n} ${n === 1 ? one : more}`);
    const quantities = [...api.values].filter(([, v]) => !v.error);
    const tables = [...api.tables].filter(([, t]) => !t.error);
    return [
      h('h3', {}, 'The board'),
      h('p', { class: 'bd-tip' }, parts.length ? `${parts.join(' · ')}.` : 'Nothing yet. Double-click the ground for a note, or lay out a template.'),
      row('Arrange', h('div', { class: 'bd-chips-row' },
        button('Tidy regions', () => api.place(tidyRegions(api.board), 'tidy the regions'), 'each region\'s items in a grid within it'),
        button('As a tree', () => api.place(tree(api.board), 'arrange as a tree'), 'the links read as a tree, growing to the right'))),
      layersPanel(),
      contents(),
      quantities.length ? h('div', { class: 'bd-quantities' }, h('h4', {}, 'Quantities'), quantities.map(([name, v]) =>
        h('button', { type: 'button', class: 'bd-quantity', title: 'show where it is written', onclick: () => { api.select([v.item]); api.fit([v.item]); } },
          h('b', {}, name), h('span', {}, formatValue(v))))) : null,
      tables.length ? h('div', { class: 'bd-quantities' }, h('h4', {}, 'Tables'), tables.map(([name, t]) =>
        h('button', { type: 'button', class: 'bd-quantity', title: 'show what gives it', onclick: () => { api.select([t.item]); api.fit([t.item]); } },
          h('b', {}, name), h('span', {}, `${t.rows.toLocaleString('en-GB')} × ${t.columns.length}`)))) : null,
      h('details', { class: 'bd-help' }, h('summary', {}, 'The hand'), h('pre', {},
        'drag the ground      move\nwheel · pinch        zoom\nShift + drag         select an area\ndouble-click         write · new note\ndrag a side\'s dot    join\nTab                  a joined note\nN · T · R            note · text · region\nP · E · V            pen · rubber · hand\n1 · 0                see all · true size\nCtrl Z · Ctrl D      undo · duplicate')),
    ];
  }

  function render() {
    clearTimeout(timer);
    live = null;
    clear(host);
    const ids = api.selection, b = api.board;
    let content;
    if (!ids.length) content = nothing();
    else if (ids.length > 1) content = many(ids);
    else {
      const [id] = ids, item = b.items[id];
      if (!item) return;
      const name = item.kind === 'card' ? api.cards[item.card]?.label || 'Card' : KIND_NAMES[item.kind];
      content = [h('h3', {}, name), ...KINDS[item.kind](id, item), isLink(item) ? null : layerRow([id]), isLink(item) ? h('div', { class: 'bd-actions' }, button('Remove', () => api.remove([id]), 'Delete')) : actions([id])];
    }
    host.append(...content.filter(Boolean));
    const spot = h('div', { class: 'bd-live' });
    host.append(spot);
    if (live) spot.append(...(live() || []));
  }

  // A field that loses the focus to a press on one of the panel's keys changes the board before the
  // key is let go: the panel is drawn again only after the press is over, or the key would be gone.
  let pressing = false, pending = false;
  const pressed = () => { pressing = true; };
  const released = () => {
    if (!pressing) return;
    pressing = false;
    setTimeout(() => { if (pending) { pending = false; render(); } }, 0);
  };
  host.addEventListener('pointerdown', pressed, true);
  window.addEventListener('pointerup', released, true);

  const offSelect = api.on('select', render);
  const offChange = api.on('change', () => {
    if (pressing) { pending = true; return undefined; }
    // While a field of the panel is typed in, only what follows the board is drawn again.
    const typing = host.contains(document.activeElement) && /^(INPUT|TEXTAREA|SELECT)$/.test(document.activeElement.tagName) && document.activeElement.type !== 'checkbox';
    if (!typing) return render();
    const spot = host.querySelector('.bd-live');
    if (spot && live) { clear(spot); spot.append(...(live() || [])); }
    return undefined;
  });
  render();
  return { render, destroy() { offSelect(); offChange(); host.removeEventListener('pointerdown', pressed, true); window.removeEventListener('pointerup', released, true); clear(host); } };
}

// ---- a template in small ----------------------------------------------------------------------------

export function templatePreview(template, types = []) {
  const t = thumbnail(template), toneOf = new Map(types.map((x) => [x.id, x.tone]));
  const k = Math.min(132 / t.w, 84 / t.h);
  const svg = s('svg', { class: 'bd-thumb', viewBox: `0 0 ${(t.w * k).toFixed(1)} ${(t.h * k).toFixed(1)}`, width: (t.w * k).toFixed(1), height: (t.h * k).toFixed(1), 'aria-hidden': 'true' });
  for (const r of t.rects) {
    const cls = r.kind === 'region' ? `region tone-${r.tone || 'none'}` : r.kind === 'note' ? `note tone-${toneOf.get(r.type) || 'paper'}` : r.kind;
    svg.append(s('rect', { class: cls, x: (r.x * k).toFixed(1), y: (r.y * k).toFixed(1), width: Math.max(1.5, r.w * k).toFixed(1), height: Math.max(1.5, r.h * k).toFixed(1) }));
  }
  return svg;
}
