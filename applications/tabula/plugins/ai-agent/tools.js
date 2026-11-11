// What an agent can do on a board: its tools, as definitions a model is given and as an executor that
// carries each call out. Every call becomes operations on the board's model (the board's own, with
// their inverses), so that what an agent did is undone like anything else; nothing is done to the page
// behind the model's back. Pure but for three things the page lends it: running a module, showing a
// part of the board, and the layer in the hand.
//
// What the agent may use of the plug-ins is decided by the person (capabilities): their templates,
// their cards, their modules. A plug-in that is not allowed is not told of, and a call that names it
// is refused. The agent is given no tool that writes code: it runs only the modules that are installed.
//
// The tools are the same whatever is allowed, and what a conversation was told at its beginning is
// not rewritten: a model's reasoning is kept only while what came before it stands as it was sent
// (and so is the provider's cache). When the person changes what the agent may use, the executor
// follows at once, and the model is told by a line beside the person's next words (capsChange).

const INKS = ['ink', 'rose', 'lilac', 'sky', 'sage', 'gold', 'clay'];
const FORMS = ['rect', 'pill', 'ellipse', 'diamond'];
const SIZES = ['title', 'heading', 'body', 'small'];
const short = (words, most = 240) => { const s = String(words ?? ''); return s.length > most ? `${s.slice(0, most - 1)}…` : s; };

// ---- what the agent may use ---------------------------------------------------------------------------

// From the plug-ins the person allows (null: all of them): their templates, cards and modules. The
// board's own cards (a chart, a map) are always there.
export function capabilities({ B, packs, modules = [], allowed = null, mayRun = true }) {
  const open = (id) => allowed === null || allowed.includes(id);
  const templates = [], cards = {};
  for (const [name, card] of Object.entries(B.CARDS)) if (name !== 'module') cards[name] = { label: card.label, about: card.about, form: card.form, props: card.props, size: card.size };
  for (const pack of packs) {
    if (!open(pack.id)) continue;
    for (const t of pack.templates || []) templates.push({ ref: `${pack.id}/${t.id}`, name: t.name, about: t.about || '', template: t });
    for (const [name, card] of Object.entries(pack.cards || {})) cards[name] = { label: card.label, about: card.about, form: card.form, props: card.props, size: card.size, plugin: pack.id };
  }
  return { templates, cards, modules: mayRun ? modules.filter((m) => !m.error && open(m.plugin)) : [], mayRun };
}

// The fields a card asks for, as a model reads them.
const fieldsOf = (card) => {
  let fields = [];
  try { fields = card.form ? card.form({ props: card.props || {}, tables: new Map(), services: {} }) : []; } catch { fields = []; }
  return fields.map((f) => `${f.name}${f.role ? ` (a ${f.role}${f.of ? ` of ${f.of}` : ''})` : f.kind === 'choice' ? ` (one of ${(f.options || []).map((o) => (Array.isArray(o) ? o[0] : o)).join(', ')})` : ` (${f.kind})`}`).join(', ');
};
const paramsOf = (m) => (m.params || []).map((q) => `${q.name}${q.role ? ` (a ${q.role}${q.of ? ` of ${q.of}` : ''})` : q.kind === 'choice' ? ` (one of ${(q.options || []).map((o) => (typeof o === 'object' ? o.value : o)).join(', ')})` : ` (${q.kind})`}, default ${JSON.stringify(q.default)}: ${q.label}`).join('; ');

// ---- what the model is told ---------------------------------------------------------------------------

export function systemPrompt({ caps, types, today = '' }) {
  const lines = [
    'You are the agent of Tabula, a board on which a person sets out their thinking and ties it to evidence. You act on the board through your tools, at the person\'s request. Everything you do is an operation the person can undo, and they see the board change as you work.',
    '',
    '# The board',
    'Positions are in the board\'s units (a pixel at true size), x to the right and y downward. A note is 200 wide and about 132 high; leave 20 or more between items. Leave x and y out and items are laid out for you in a free part of the board, or within the region you name; give them when the arrangement itself says something (a diagram, a matrix, a line of time).',
    `Notes are of kinds, and the kind says how a statement stands: ${types.map((t) => `${t.id} (${t.label}${t.about ? `: ${t.about}` : ''})`).join('; ')}.`,
    'A link says how two items stand to each other: arrow (no claim), supports, contradicts, causes ("leads to"), part-of, follows ("then").',
    'A line of a note such as  share = 3% [2%, 5%]  defines a quantity with the range it is believed to lie in; a figure\'s formula uses the names (TAM = buyers * need * spend) with + - * / ( ) min( ) max( ), and the ranges are carried through. Numbers take the suffixes k, M, B and %.',
    'A plan or a chronicle is written as lines, one entry each: "Name : 2026-11-16, 10d", "Name : after Other, 2w, 40%", "Name : after A & B, milestone", "# A group"; for a chronicle the date comes first: "1687-07-05 : Principia", "480 BC : Salamis".',
    'A table is rows and columns under a name; a chart, a map or a module draws it by that name. A map needs columns lat and lon, or iso3.',
    '',
    '# How to work',
    'Read the board before you change what is on it, and when the request refers to something there. Then act: prefer a few well-placed items to many, gather what belongs together in a region, and join what bears on what.',
    'Say what is known and what is assumed. A fact needs a source: write one, or make the note an assumption or a question. Numbers you supply are placeholders unless the person gave them or a module returned them: write them as assumptions with a range, and say so.',
    'When you have finished, say in a sentence or two what you did and what the person should check. Do not describe each tool call.',
    'The names of a plan\'s entries hold no comma, colon, & or +: those divide a line.',
    'A message may end with lines in square brackets that the page added: what the person has selected on the board ("this" and "these" are those items), or a change to what you may use, which holds from then on.',
  ];
  if (today) lines.push(`Today is ${today}.`);
  lines.push('', '# What you may use', 'The person decides which templates, cards and modules are yours to use. A call for one that is not is refused.');
  lines.push(caps.templates.length ? 'Templates you may lay on the board (lay_template):' : 'No template is yours to lay.', ...caps.templates.map(templateLine));
  lines.push('Cards you may add, live views of a table (add_card):', ...Object.entries(caps.cards).map(cardLine));
  if (caps.modules.length) lines.push('Modules you may run (run_module); they get data and compute, and each answers tables and notes:', ...caps.modules.map(moduleLine),
    'A module reads the board: the tables on it and the quantities its notes define. If it needs a quantity that no note defines, add the assumption note first.');
  else lines.push(caps.mayRun ? 'No module is yours to run.' : 'You may not run modules: the person has turned that off.');
  return lines.join('\n');
}

const templateLine = (t) => `- ${t.ref}: ${t.name}. ${t.about}`;
const cardLine = ([name, c]) => `- ${name}: ${c.label}. ${c.about || ''} Props: ${fieldsOf(c) || 'none'}.`;
const moduleLine = (m) => `- ${m.ref}: ${m.title}. ${m.description || ''} Params: ${paramsOf(m) || 'none'}.`;

// What changed in what the agent may use, as lines for the model to read beside the person's next
// words; empty when nothing did. What a conversation was told before is not rewritten.
export function capsChange(before, after) {
  const gone = [], come = [];
  const diff = (was, is, name, line, what) => {
    const lost = was.filter((x) => !is.some((y) => name(y) === name(x))), won = is.filter((x) => !was.some((y) => name(y) === name(x)));
    if (lost.length) gone.push(`${what}: ${lost.map(name).join(', ')}`);
    if (won.length) come.push(`${what}:`, ...won.map(line));
  };
  diff(before.templates, after.templates, (t) => t.ref, templateLine, 'templates');
  diff(Object.entries(before.cards), Object.entries(after.cards), ([name]) => name, cardLine, 'cards');
  if (before.mayRun && !after.mayRun) gone.push('modules: you may no longer run any');
  else diff(before.modules, after.modules, (m) => m.ref, moduleLine, 'modules');
  if (!gone.length && !come.length) return '';
  return `[The person changed what you may use.${gone.length ? `\nNo longer yours: ${gone.join('; ')}.` : ''}${come.length ? `\nNow yours as well, ${come.join('\n')}` : ''}]`;
}

const obj = (properties, required = []) => ({ type: 'object', properties, required });
const str = (description, extra = {}) => ({ type: 'string', description, ...extra });
const num = (description) => ({ type: 'number', description });
const list = (items, description) => ({ type: 'array', items, description });

// The tools, the same for every conversation: which templates, cards and modules may be named is said
// in words (the system prompt, and a change since), not in the schemas.
export function toolDefinitions() {
  return [
    { name: 'read_board', description: 'Read what is on the board: every item with its id, kind, words and place, the links, the layers, the quantities its notes define with their values, and the tables with their columns. Call it before changing existing items.', input_schema: obj({}) },
    { name: 'add_items', description: 'Add notes, text, regions, shapes, figures, plans and chronicles. Give each a key to refer to it later (in links, as a region for other items). Returns the id of each.',
      input_schema: obj({ items: list(obj({
        key: str('a name of your own for this item, to refer to it in later calls'),
        kind: str('what it is', { enum: ['note', 'text', 'region', 'shape', 'figure', 'schedule'] }),
        type: str('note: its kind (idea, fact, assumption, question, insight, decision, risk, plain, or a plug-in\'s)'),
        text: str('note, text, shape: its words. A note may hold lines that define quantities'),
        title: str('region, figure, schedule: its title'),
        source: str('note: where a fact comes from. schedule: its entries, a line each'),
        when: str('note: a date, as 2026-11-16, 2026-11 or 2026'),
        formula: str('figure: its formulas, a line each, as  name = expression'),
        mode: str('schedule: how it is drawn', { enum: ['plan', 'chronicle'] }),
        form: str('shape: its form', { enum: FORMS }),
        size: str('text: its size', { enum: SIZES }),
        tone: str('note, region, shape: a colour other than its kind\'s', { enum: ['apricot', 'sage', 'lilac', 'sky', 'rose', 'gold', 'clay', 'paper'] }),
        in: str('the region it lies in: a region\'s id or key. Left out with x and y, it is placed within that region'),
        x: num('its left edge; leave out to have it placed'), y: num('its top edge'), w: num('its width'), h: num('its height'),
      }, ['kind']), 'the items, regions before what lies in them') }, ['items']) },
    { name: 'connect', description: 'Join items with links that say how they stand to each other.',
      input_schema: obj({ links: list(obj({ from: str('an id or key'), to: str('an id or key'), rel: str('what the link says', { enum: ['arrow', 'supports', 'contradicts', 'causes', 'part-of', 'follows'] }), label: str('words on the link, instead of the relation\'s own') }, ['from', 'to'])) }, ['links']) },
    { name: 'update_items', description: 'Change items that are on the board: their words, kind, colour, place, size or layer; or what a link says. Only the fields given are changed; the fields are those of add_items.',
      input_schema: obj({ changes: list(obj({ id: str('an id or key'), type: str('a note\'s kind'), text: str(''), title: str(''), source: str(''), when: str(''), formula: str(''), mode: str(''), form: str(''), size: str(''), tone: str(''),
        rel: str('a link: what it says'), label: str('a link: its words; "" for the relation\'s own'),
        props: { type: 'object', description: 'a card: props to change' }, x: num(''), y: num(''), w: num(''), h: num(''), layer: str('a layer\'s id, or "" for the ground') }, ['id'])) }, ['changes']) },
    { name: 'remove_items', description: 'Take items off the board. Links on them go with them; what lay in a removed region stays.', input_schema: obj({ ids: list(str('an id or key')) }, ['ids']) },
    { name: 'draw', description: 'Draw with the pen: each stroke a line through points, for an underline, a circle about something, an arrow, a sketch. A closed stroke returns to its first point.',
      input_schema: obj({ strokes: list(obj({ points: list({ type: 'array', items: { type: 'number' }, description: '[x, y]' }, 'the points the stroke passes through, in order; many points for a curve'),
        colour: str('the ink', { enum: INKS }), width: num('2 fine, 4 medium, 9 broad'), marker: { type: 'boolean', description: 'a broad clear marker, to go over words' }, closed: { type: 'boolean', description: 'return to the first point' } }, ['points'])) }, ['strokes']) },
    { name: 'add_table', description: 'Lay a table on the board from delimited text (commas, semicolons or tabs between, the first row the names of the columns). Charts, maps and modules draw it by its name.',
      input_schema: obj({ name: str('letters, digits and _'), csv: str('the rows'), title: str('what it is'), source: str('where the data come from') }, ['name', 'csv']) },
    { name: 'read_table', description: 'Read rows of a table on the board.', input_schema: obj({ name: str('the table\'s name'), rows: num('how many rows (at most 60)'), from: num('the first row to read, from 0') }, ['name']) },
    { name: 'arrange', description: 'Set items out again: in a tidy grid, as a tree by the links among them, each region\'s items within it, or the notes that say when in the order of time.',
      input_schema: obj({ how: str('', { enum: ['grid', 'tree', 'regions', 'time'] }), ids: list(str('an id or key'), 'the items; all of them when left out') }, ['how']) },
    { name: 'layers', description: 'Work with layers: add one (new items are then laid on it), choose the one new items are laid on, move items to one, hide, hold or make one faint, or remove one (what lay on it is left on the ground).',
      input_schema: obj({ action: str('', { enum: ['add', 'use', 'move', 'set', 'remove'] }), name: str('add: its name'), layer: str('the layer\'s id; "" for the ground'), ids: list(str('an id or key'), 'move: the items'),
        hidden: { type: 'boolean' }, locked: { type: 'boolean' }, opacity: num('set: 1 clear, less for faint') }, ['action']) },
    { name: 'show', description: 'Bring items into the person\'s view (all of the board when none are named).', input_schema: obj({ ids: list(str('an id or key')) }) },
    { name: 'lay_template', description: 'Lay a template on the board, beside what is there: one of those you were told you may lay. Returns the ids of its items, which you may then change.',
      input_schema: obj({ template: str('which, as <plug-in>/<template>') }, ['template']) },
    { name: 'add_card', description: 'Add a card: a live view of a table (a chart, a map, or a plug-in\'s), one of those you were told you may add, with the props it was said to take.',
      input_schema: obj({ card: str('which, by its name'), props: { type: 'object', description: 'its props, such as {"table": "potential", "value": "value"}' }, key: str('a name of your own for it'),
        x: num(''), y: num(''), w: num(''), h: num('') }, ['card']) },
    { name: 'run_module', description: 'Run a module on the board: one of those you were told you may run. It is laid on the board as a card (or the card given is run again), and answers notes and tables, which other cards can then draw by name. It may take some seconds.',
      input_schema: obj({ module: str('which, as <plug-in>/<file>'), params: { type: 'object', description: 'its parameters by name; those left out take their defaults' }, card_id: str('run this module card again instead of adding one') }, ['module']) },
  ];
}

// ---- what the model reads of the board ----------------------------------------------------------------

export function describeBoard(B, board, tables, values) {
  const items = [], links = [];
  let strokes = 0;
  for (const id of board.order) {
    const it = board.items[id], p = board.place[id];
    if (it.kind === 'link') { links.push({ id, from: it.from, to: it.to, rel: it.rel, ...(it.label ? { label: it.label } : {}) }); continue; }
    if (it.kind === 'ink') { strokes++; continue; }
    const row = { id, kind: it.kind, x: Math.round(p.x), y: Math.round(p.y), w: Math.round(p.w), h: Math.round(p.h) };
    for (const k of ['type', 'title', 'mode', 'form', 'size', 'tone', 'when', 'in', 'layer', 'card', 'name']) if (it[k] !== undefined && it[k] !== '') row[k] = it[k];
    if (it.text) row.text = short(it.text);
    if (it.kind === 'note' && it.source) row.source = short(it.source, 80);
    if (it.kind === 'schedule') row.source = short(it.source, 500);
    if (it.formula) row.formula = short(it.formula, 300);
    if (it.caption) row.caption = short(it.caption, 80);
    if (it.kind === 'card') { row.props = it.props || {}; if (it.out) row.ran = it.out.error ? `failed: ${short(it.out.error, 160)}` : `answered ${(it.out.tables || []).map((t) => t.name).join(', ') || 'no tables'}`; }
    if (it.kind === 'table') { row.rows = it.rows; row.columns = it.columns.map((c) => c.name); }
    if (it.hidden) row.hidden = true;
    if (it.locked) row.locked = true;
    items.push(row);
  }
  const quantities = {};
  for (const [name, v] of values) quantities[name] = v.error ? { error: v.error } : { value: v.mid, low: v.lo, high: v.hi, ...(v.unit ? { unit: v.unit } : {}) };
  const tabled = [];
  for (const [name, t] of tables) tabled.push(t.error ? { name, error: t.error } : { name, rows: t.rows, columns: t.columns.map((c) => `${c.name}${c.kind === 'number' ? '' : ' (text)'}`), ...(t.title ? { title: short(t.title, 80) } : {}) });
  const box = B.bounds(board);
  return { title: board.title, items, links, ...(strokes ? { strokes_of_the_pen: strokes } : {}), layers: (board.layers || []).map((l) => ({ ...l })), quantities, tables: tabled,
    extent: box ? { x: Math.round(box.x), y: Math.round(box.y), w: Math.round(box.w), h: Math.round(box.h) } : null };
}

// ---- carrying a call out ------------------------------------------------------------------------------

// env: {B, board(), commit(ops, label), tables(), values(), caps, types, run(cardId) → the card's answer, fit(ids), layer(), setLayer(id)}
export function createExecutor(env) {
  const { B } = env;
  const refs = new Map();   // the keys the agent gave → ids
  const fail = (words) => { throw new Error(words); };
  const idOf = (name) => {
    const board = env.board();
    if (board.items[name]) return name;
    if (refs.has(name) && board.items[refs.get(name)]) return refs.get(name);
    return fail(`there is no item '${name}' on the board`);
  };
  // Operations checked against the board before they are given to it: a refusal is the model's to read.
  const commit = (ops, label) => { if (ops.length) { B.apply(env.board(), ops); env.commit(ops, label); } };
  const onLayer = (item) => (env.layer() ? { ...item, layer: env.layer() } : item);
  const finite = (v) => typeof v === 'number' && Number.isFinite(v);
  // A free part of the board: to the right of what is there.
  const freeCorner = () => { const b = B.bounds(env.board()); return b ? { x: Math.round(b.x + b.w + 120), y: Math.round(b.y) } : { x: 0, y: 0 }; };
  const noteHeight = (text) => Math.max(B.DEFAULT_SIZE.note.h, 64 + 21 * String(text || '').split('\n').reduce((n, line) => n + Math.max(1, Math.ceil(line.length / 25)), 0));

  const FIELDS = {
    note: ['type', 'text', 'source', 'when', 'tone'], text: ['text', 'size'], region: ['title', 'tone'], shape: ['form', 'text', 'tone'],
    figure: ['title', 'formula'], schedule: ['title', 'mode', 'source'], table: ['title', 'source'], image: ['caption'], card: [], ink: [],
  };
  function content(kind, spec, adding = false) {
    const out = {};
    for (const k of FIELDS[kind]) if (spec[k] !== undefined) out[k] = String(spec[k]);
    if (kind === 'note' && out.type && !env.types.some((t) => t.id === out.type)) fail(`'${out.type}' is not a kind of note (there is: ${env.types.map((t) => t.id).join(', ')})`);
    if (out.form && !FORMS.includes(out.form)) fail(`'${out.form}' is not a form`);
    if (out.size && !SIZES.includes(out.size)) fail(`'${out.size}' is not a size of text`);
    if (out.mode && !['plan', 'chronicle'].includes(out.mode)) fail(`'${out.mode}' is not a way to draw a schedule`);
    if (out.when && !B.parseDate(out.when)) fail(`'${out.when}' is not a date: write 2026-11-16, 2026-11 or 2026`);
    // A plan or a chronicle that cannot be read is not laid: the model is told which lines, and writes them again.
    if (kind === 'schedule' && (adding || out.source !== undefined)) {
      const parsed = B.parseSchedule(out.source ?? ''), resolved = B.resolveSchedule(parsed.tasks);
      const wrong = [...parsed.errors, ...resolved.errors].map((e) => `line ${e.line ?? '?'}: ${e.message}`);
      if (wrong.length) fail(`the schedule cannot be read. ${wrong.slice(0, 6).join('; ')}. Each entry is a line "Name : 2026-11-16, 10d" or "Name : after Another, 2w" (several: "after A & B"); a name holds no comma, colon, & or +`);
      if (!resolved.tasks.length) fail('the schedule has no entries: its lines go in `source`, one entry a line');
    }
    return out;
  }
  // What is wrong with the quantities that the items given define: for the model to mend.
  const troubles = (ids) => {
    const mine = new Set(ids), out = [];
    for (const [name, v] of env.values()) if (v.error && mine.has(v.item)) out.push(`'${name}': ${v.error}`);
    return out.length ? { problems: out } : {};
  };

  const TOOLS = {
    read_board() { return describeBoard(B, env.board(), env.tables(), env.values()); },

    add_items({ items }) {
      if (!Array.isArray(items) || !items.length) fail('no items are given');
      const board = env.board(), made = [], ops = [];
      const corner = freeCorner();
      let flowX = corner.x, flowY = corner.y, rowH = 0, inRow = 0;
      const local = new Map();      // keys of this call → {id, place}
      const filled = new Map();     // region id → how many items it has been given a slot for
      const standing = new Map();   // regions that were on the board → their places as this call leaves them (copies: a board is not written on)
      const regionOf = (name) => {
        if (local.has(name)) return local.get(name);
        const id = idOf(name);
        if (board.items[id].kind !== 'region') fail(`'${name}' is not a region`);
        if (!standing.has(id)) standing.set(id, { id, place: { ...board.place[id] } });
        return standing.get(id);
      };
      const ordered = [...items.filter((s) => s.kind === 'region'), ...items.filter((s) => s.kind !== 'region')];
      for (const spec of ordered) {
        if (!['note', 'text', 'region', 'shape', 'figure', 'schedule'].includes(spec.kind)) fail(`add_items adds notes, text, regions, shapes, figures and schedules, not '${spec.kind}' (use add_table, add_card or lay_template)`);
        const item = B.newItem(spec.kind, content(spec.kind, spec, true));
        const size = { ...B.DEFAULT_SIZE[spec.kind] };
        if (spec.kind === 'note') size.h = noteHeight(spec.text);
        // Words set large are given the width they need on one line; a paragraph, the height of its lines.
        if (spec.kind === 'text') {
          const words = String(spec.text || ''), per = { title: 16, heading: 12 }[item.size];
          if (per) size.w = Math.max(320, Math.min(900, per * words.length));
          else size.h = Math.max(size.h, 22 * words.split('\n').reduce((n, line) => n + Math.max(1, Math.ceil(line.length / 44)), 0) + 12);
        }
        if (finite(spec.w)) size.w = Math.max(B.MIN_SIZE.w, spec.w);
        if (finite(spec.h)) size.h = Math.max(B.MIN_SIZE.h, spec.h);
        // A plan or a chronicle is as high as its entries need (the page gives it that height anyway): the model is told where it ends.
        if (spec.kind === 'schedule') {
          const resolved = B.resolveSchedule(B.parseSchedule(item.source).tasks);
          const laid = item.mode === 'chronicle' ? B.layoutChronicle(resolved, { width: size.w - 20 }) : B.layoutPlan(resolved, { width: size.w - 20 });
          size.h = Math.max(size.h, Math.ceil(laid.height + 48));
        }
        let place;
        if (finite(spec.x) && finite(spec.y)) place = { x: Math.round(spec.x), y: Math.round(spec.y), ...size };
        else if (spec.in !== undefined && spec.in !== '') {
          // A slot within the region, after what it already holds.
          const region = regionOf(spec.in), r = region.place;
          const taken = filled.has(region.id) ? filled.get(region.id) : B.members(board, region.id).length;
          const cols = Math.max(1, Math.floor((r.w - 36 + 14) / (size.w + 14)));
          place = { x: Math.round(r.x + 18 + (taken % cols) * (size.w + 14)), y: Math.round(r.y + 46 + Math.floor(taken / cols) * (size.h + 14)), ...size };
          filled.set(region.id, taken + 1);
          // A region too low for what it is given grows.
          const need = place.y + size.h + 18 - r.y;
          if (need > r.h) r.h = need;
        } else {
          if (inRow >= (spec.kind === 'region' ? 2 : 4) || (inRow && spec.kind !== 'note')) { flowX = corner.x; flowY += rowH + 24; rowH = 0; inRow = 0; }
          place = { x: flowX, y: flowY, ...size };
          flowX += size.w + 24; rowH = Math.max(rowH, size.h); inRow++;
        }
        const id = B.makeId();
        if (spec.key) { refs.set(String(spec.key), id); local.set(String(spec.key), { id, place }); }
        ops.push({ t: 'add', id, item: onLayer(item), place, ...(spec.kind === 'region' ? { at: 0 } : {}) });
        made.push({ ...(spec.key ? { key: String(spec.key) } : {}), id, kind: spec.kind, ...place });
      }
      // Regions were given places that may have grown since.
      for (const op of ops) if (op.t === 'add' && op.item.kind === 'region') { const l = [...local.values()].find((v) => v.id === op.id); if (l) op.place = { ...op.place, h: l.place.h }; }
      for (const { id, place } of standing.values()) if (place.h !== board.place[id].h) ops.push({ t: 'place', id, place: { h: place.h } });
      const after = B.apply(board, ops).board;
      commit([...ops, ...B.membershipOps(after, made.map((m) => m.id))], `add ${made.length} ${made.length === 1 ? 'item' : 'items'}`);
      return { added: made, ...troubles(made.map((m) => m.id)) };
    },

    connect({ links }) {
      if (!Array.isArray(links) || !links.length) fail('no links are given');
      const board = env.board(), ops = [], made = [];
      for (const l of links) {
        const from = idOf(l.from), to = idOf(l.to), rel = l.rel || 'arrow';
        if (!B.RELATIONS[rel]) fail(`'${rel}' is not something a link says (there is: ${Object.keys(B.RELATIONS).join(', ')})`);
        if (from === to) fail('a link joins two items, not one to itself');
        if (board.order.some((i) => board.items[i].kind === 'link' && board.items[i].from === from && board.items[i].to === to) || made.some((m) => m.from === from && m.to === to)) continue;
        const id = B.makeId();
        ops.push({ t: 'add', id, item: B.newItem('link', { from, to, rel, ...(l.label ? { label: String(l.label) } : {}) }) });
        made.push({ id, from, to, rel });
      }
      commit(ops, `join ${made.length}`);
      return { joined: made };
    },

    update_items({ changes }) {
      if (!Array.isArray(changes) || !changes.length) fail('no changes are given');
      const board = env.board(), ops = [], moved = [];
      for (const c of changes) {
        const id = idOf(c.id), item = board.items[id];
        if (item.kind === 'link') {
          const patch = {};
          if (c.rel) { if (!B.RELATIONS[c.rel]) fail(`'${c.rel}' is not something a link says (there is: ${Object.keys(B.RELATIONS).join(', ')})`); patch.rel = c.rel; }
          if (c.label !== undefined) patch.label = c.label || null;
          if (Object.keys(patch).length) ops.push({ t: 'set', id, patch });
          continue;
        }
        const patch = content(item.kind, c);
        if (item.kind === 'card' && c.props && typeof c.props === 'object') patch.props = { ...(item.props || {}), ...c.props };
        if (c.layer !== undefined) { if (c.layer !== '' && !(board.layers || []).some((l) => l.id === c.layer)) fail(`there is no layer '${c.layer}'`); patch.layer = c.layer === '' ? null : c.layer; }
        if (Object.keys(patch).length) ops.push({ t: 'set', id, patch });
        const place = {};
        for (const k of ['x', 'y', 'w', 'h']) if (finite(c[k])) place[k] = k === 'w' ? Math.max(B.MIN_SIZE.w, c[k]) : k === 'h' ? Math.max(B.MIN_SIZE.h, c[k]) : Math.round(c[k]);
        if (Object.keys(place).length) { ops.push({ t: 'place', id, place }); moved.push(id); }
      }
      const after = B.apply(board, ops).board;
      commit([...ops, ...B.membershipOps(after, moved.some((i) => board.items[i].kind === 'region') ? after.order : moved)], `change ${changes.length}`);
      return { changed: changes.length, ...troubles(ops.map((o) => o.id)) };
    },

    remove_items({ ids }) {
      if (!Array.isArray(ids) || !ids.length) fail('no items are named');
      const gone = ids.map(idOf);
      commit(B.removeOps(env.board(), gone), `remove ${gone.length}`);
      return { removed: gone.length };
    },

    draw({ strokes }) {
      if (!Array.isArray(strokes) || !strokes.length) fail('no strokes are given');
      const ops = [], made = [], r1 = (v) => Math.round(v * 10) / 10;
      for (const st of strokes) {
        let pts = (st.points || []).filter((q) => Array.isArray(q) && finite(q[0]) && finite(q[1])).map((q) => [q[0], q[1], 0.5]);
        if (pts.length < 2) fail('a stroke passes through at least two points, each [x, y]');
        if (st.closed) pts = [...pts, pts[0]];
        const colour = INKS.includes(st.colour) ? st.colour : 'ink', width = finite(st.width) ? Math.max(1, Math.min(24, st.width)) : (st.marker ? 16 : 3);
        const pad = width * 0.9 + 1, x0 = Math.min(...pts.map((q) => q[0])) - pad, y0 = Math.min(...pts.map((q) => q[1])) - pad;
        const w = Math.max(...pts.map((q) => q[0])) + pad - x0, h = Math.max(...pts.map((q) => q[1])) + pad - y0;
        const id = B.makeId();
        ops.push({ t: 'add', id, item: onLayer(B.newItem('ink', { points: pts.map(([x, y, p]) => [r1(x - x0), r1(y - y0), p]), size: [r1(w), r1(h)], colour, width, ...(st.marker ? { marker: true } : {}) })), place: { x: r1(x0), y: r1(y0), w: r1(w), h: r1(h) } });
        made.push(id);
      }
      const after = B.apply(env.board(), ops).board;
      commit([...ops, ...B.membershipOps(after, made)], `draw ${made.length}`);
      return { drawn: made };
    },

    add_table({ name, csv, title = '', source = '' }) {
      const table = B.parseDelimited(String(csv || ''), { name: B.tableName(name, new Set(env.tables().keys())), title: String(title), source: String(source) });
      const size = { w: Math.min(660, 150 + 88 * Math.min(table.columns.length, 7)), h: 200 }, at = freeCorner(), id = B.makeId();
      commit([{ t: 'add', id, item: onLayer(B.newItem('table', table)), place: { ...at, ...size } }], 'add a table');
      return { id, name: table.name, rows: table.rows, columns: table.columns.map((c) => `${c.name}${c.kind === 'number' ? '' : ' (text)'}`) };
    },

    read_table({ name, rows = 12, from = 0 }) {
      const t = env.tables().get(name);
      if (!t) fail(`there is no table '${name}' on the board (there is: ${[...env.tables().keys()].join(', ') || 'none'})`);
      if (t.error) fail(t.error);
      const a = Math.max(0, Math.floor(from) || 0), n = Math.max(1, Math.min(60, Math.floor(rows) || 12)), names = t.columns.map((c) => c.name);
      return { name, rows: t.rows, columns: names, from: a, values: Array.from({ length: Math.max(0, Math.min(n, t.rows - a)) }, (_, k) => names.map((c) => t.data[c][a + k])) };
    },

    arrange({ how, ids }) {
      const board = env.board(), chosen = Array.isArray(ids) && ids.length ? ids.map(idOf) : board.order.filter((i) => board.place[i]);
      const placements = how === 'grid' ? B.tidy(board, chosen) : how === 'tree' ? B.tree(board, { ids: chosen }) : how === 'regions' ? B.tidyRegions(board) : how === 'time' ? B.byTime(board, chosen, (w) => B.parseDate(w)?.day)
        : fail(`'${how}' is not an arrangement (there is: grid, tree, regions, time)`);
      const ops = B.placeOps(board, placements), after = B.apply(board, ops).board;
      commit([...ops, ...B.membershipOps(after, ops.some((o) => board.items[o.id].kind === 'region') ? after.order : ops.map((o) => o.id))], `arrange as ${how}`);
      return { moved: ops.length };
    },

    layers({ action, name, layer, ids, hidden, locked, opacity }) {
      const board = env.board(), layers = board.layers || [];
      const known = (l) => (layers.some((x) => x.id === l) ? l : fail(`there is no layer '${l}' (there is: ${layers.map((x) => `${x.id} "${x.name}"`).join(', ') || 'none'})`));
      if (action === 'add') {
        const made = { id: B.makeId(), name: String(name || `Layer ${layers.length + 1}`) };
        commit([{ t: 'layers', layers: [...layers, made] }], 'add a layer');
        env.setLayer(made.id);
        return { layer: made.id, name: made.name, note: 'new items are now laid on it' };
      }
      if (action === 'use') { env.setLayer(layer ? known(layer) : null); return { new_items_on: layer || 'the ground' }; }
      if (action === 'move') { const to = layer ? known(layer) : null; const moved = (ids || []).map(idOf).filter((i) => board.items[i].kind !== 'link'); commit(moved.map((i) => ({ t: 'set', id: i, patch: { layer: to } })), 'move to a layer'); return { moved: moved.length }; }
      if (action === 'set') {
        known(layer);
        commit([{ t: 'layers', layers: layers.map((l) => (l.id !== layer ? l : Object.fromEntries(Object.entries({ ...l, ...(hidden !== undefined ? { hidden } : {}), ...(locked !== undefined ? { locked } : {}), ...(finite(opacity) ? { opacity: Math.max(0.1, Math.min(1, opacity)) } : {}) })
          .filter(([k, v]) => !(v === false || (k === 'opacity' && v === 1)))))) }], 'change a layer');
        return { layer };
      }
      if (action === 'remove') { commit(B.removeLayerOps(board, known(layer)), 'remove a layer'); if (env.layer() === layer) env.setLayer(null); return { removed: layer }; }
      return fail(`'${action}' is not something done with layers (there is: add, use, move, set, remove)`);
    },

    show({ ids } = {}) { env.fit(Array.isArray(ids) && ids.length ? ids.map(idOf) : null); return { shown: true }; },

    lay_template({ template }) {
      const found = env.caps.templates.find((t) => t.ref === template);
      if (!found) fail(`'${template}' is not a template you may lay (there is: ${env.caps.templates.map((t) => t.ref).join(', ') || 'none'})`);
      const r = B.instantiate(found.template, { at: freeCorner() });
      if (env.layer()) for (const op of r.ops) if (op.item.kind !== 'link') op.item = { ...op.item, layer: env.layer() };
      commit(r.ops, `lay out '${found.name}'`);
      const board = env.board(), laid = [];
      for (const [key, id] of r.ids) { const it = board.items[id]; if (it && it.kind !== 'link') laid.push({ ...(key.startsWith('#') ? {} : { key }), id, kind: it.kind, ...(it.title ? { title: it.title } : {}), ...(it.text ? { text: short(it.text, 80) } : {}), ...(it.card ? { card: it.card, props: it.props } : {}) }); }
      return { laid };
    },

    add_card({ card, props = {}, key, x, y, w, h }) {
      const def = env.caps.cards[card];
      if (!def) fail(`'${card}' is not a card you may add (there is: ${Object.keys(env.caps.cards).join(', ') || 'none'})`);
      const size = { ...(def.size || B.DEFAULT_SIZE.card) };
      if (finite(w)) size.w = Math.max(160, w);
      if (finite(h)) size.h = Math.max(120, h);
      const at = finite(x) && finite(y) ? { x: Math.round(x), y: Math.round(y) } : freeCorner(), id = B.makeId();
      if (key) refs.set(String(key), id);
      commit([{ t: 'add', id, item: onLayer(B.newItem('card', { card, props: typeof props === 'object' && props ? props : {} })), place: { ...at, ...size } }], `add a ${def.label || card}`);
      const named = props?.table, table = named ? env.tables().get(named) : null;
      return { id, card, ...(named && !table ? { note: `there is no table '${named}' on the board yet: the card will draw it once a module answers it` } : {}) };
    },

    async run_module({ module, params = {}, card_id }) {
      const found = env.caps.modules.find((m) => m.ref === module);
      if (!env.caps.mayRun) fail('you may not run modules: the person has turned that off in the settings');
      if (!found) fail(`'${module}' is not a module you may run (there is: ${env.caps.modules.map((m) => m.ref).join(', ') || 'none'})`);
      const given = typeof params === 'object' && params ? params : {};
      const unknown = Object.keys(given).filter((k) => !(found.params || []).some((q) => q.name === k));
      if (unknown.length) fail(`${module} has no parameter ${unknown.join(', ')} (it has: ${(found.params || []).map((q) => q.name).join(', ') || 'none'})`);
      let id;
      if (card_id) {
        id = idOf(card_id);
        const item = env.board().items[id];
        if (item.kind !== 'card' || item.card !== 'module' || item.props?.module !== module) fail(`'${card_id}' is not a card of the module ${module}`);
        if (Object.keys(given).length) commit([{ t: 'set', id, patch: { props: { ...item.props, params: { ...(item.props.params || {}), ...given } } } }], 'set a module\'s parameters');
      } else {
        id = B.makeId();
        commit([{ t: 'add', id, item: onLayer(B.newItem('card', { card: 'module', props: { module, params: given } })), place: { ...freeCorner(), w: 400, h: 220 } }], `add the module '${found.title}'`);
      }
      const out = await env.run(id);
      if (!out) fail('the module could not be run here');
      if (out.error && !out.ran) fail(`the module failed: ${short(out.error.split('\n').pop(), 400)}`);
      const sample = (t) => { const names = t.columns.map((c) => c.name); return { name: t.name, rows: t.rows, columns: names, first: Array.from({ length: Math.min(3, t.rows) }, (_, k) => names.map((c) => t.data[c][k])) }; };
      return { card_id: id, ...(out.error ? { failed: short(out.error.split('\n').pop(), 400) } : {}), notes: (out.notes || []).map((n) => short(n, 600)), tables: (out.tables || []).map(sample), figures: (out.figures || []).length };
    },
  };

  return {
    refs,
    // The answer of a call, for the model to read; a call that cannot be carried out throws with the reason.
    async execute(name, input) {
      const tool = TOOLS[name];
      if (!tool) throw new Error(`there is no tool '${name}'`);
      if (!input || typeof input !== 'object') throw new Error('a tool is given an object');
      return tool(input);
    },
  };
}
