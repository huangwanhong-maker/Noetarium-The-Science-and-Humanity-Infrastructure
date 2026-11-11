// A project's workspace: what a general, project-based work keeps and how its parts refer to one another (ADR 0093). Pure.
//
// A project is plain data, kept as a folder (project.json, data/, scripts/, results/, boards/):
//
//   sources      the person's data, left as they are, each a table read from a file, with its columns bound to ROLES (which column
//                is the latitude, the time, the amount, …) and, if wanted, a filter (rows where a column is, or is among, values)
//   slices       regions of the Earth and intervals of time, each with a period: what an analysis is asked about
//   quantities   data and assumptions by name, each with the range it is believed to lie in
//   steps        tools that were run: which tool, on which slice, with which parameters, when, and what it answered (tables by
//                name, notes, figures). A step's tables live in results/<step>.json; the step is how they are made again
//   views        layers of the map and charts, each drawing a table (a source's or a step's) by its name
//   boards       boards of the visualization layer, where findings are written about
//
// Tables are named: a source by its `name`, a step's table by the name the tool gave it. A tool is given tables by name, so the
// names are the project's wiring. One name belongs to one table: a step run again replaces what it answered before.

export const SCHEMA = 'noetarium.workspace/1';

// What a column can be bound to. A table is drawn on the map when it has a place (lat and lon, or cell with the table's places),
// and moves with the clock when it has a time.
export const ROLES = [
  { id: 'lat', label: 'Latitude', kind: 'number', about: 'degrees north, WGS 84' },
  { id: 'lon', label: 'Longitude', kind: 'number', about: 'degrees east, WGS 84' },
  { id: 'cell', label: 'Cell', kind: 'any', about: 'the id of an area whose outline the table carries' },
  { id: 'time', label: 'Time', kind: 'any', about: 'an instant or the beginning of a period: a year, or a date as 2024-03-01' },
  { id: 'until', label: 'Until', kind: 'any', about: 'the end, for what lasts' },
  { id: 'key', label: 'Identity', kind: 'any', about: 'what makes two rows the same thing at different times' },
  { id: 'name', label: 'Name', kind: 'any', about: 'what a row is called' },
  { id: 'amount', label: 'Amount', kind: 'number', about: 'a quantity a row carries: sales, people, floor space' },
  { id: 'category', label: 'Kind', kind: 'any', about: 'a class the rows fall into' },
];
export const PERIODS = { year: 365.2425 * 86400e3, quarter: 91.310625 * 86400e3, month: 30.436875 * 86400e3, week: 7 * 86400e3, day: 86400e3, hour: 3600e3 };
export const NAME = /^[A-Za-z_][A-Za-z0-9_]{0,47}$/;

const ID = () => `w${Math.random().toString(36).slice(2, 10)}`;
export const makeId = (random = Math.random) => `w${random().toString(36).slice(2, 10).padEnd(8, '0')}`;

export function emptyProject({ title = 'Untitled project', now = null } = {}) {
  return { schema: SCHEMA, title, meta: now ? { created: now, saved: now } : {}, frame: null, sources: [], slices: [], quantities: [], steps: [], views: [], boards: [] };
}

// A name for a table from words, not taken by another: letters, digits and _.
export function tableName(words, taken = new Set()) {
  let base = String(words || 'table').normalize('NFKD').replace(/[^\w]+/g, '_').replace(/^_+|_+$/g, '').replace(/^(\d)/, '_$1').slice(0, 40) || 'table';
  if (!NAME.test(base)) base = 'table';
  let name = base;
  for (let k = 2; taken.has(name); k++) name = `${base}_${k}`;
  return name;
}

// ---- time ---------------------------------------------------------------------------------------------------------------

// An instant in milliseconds UTC from what a column holds: a number between 1000 and 3000 is a year; a larger number is
// milliseconds; text is a year, a year and a month, a date, or a date and a time (ISO 8601; no zone is UTC). null when it is none.
export function instant(value) {
  if (value === null || value === undefined || value === '') return null;
  if (typeof value === 'number') {
    if (!Number.isFinite(value)) return null;
    if (value >= 1000 && value < 3000) return Date.UTC(Math.floor(value), 0, 1) + (value - Math.floor(value)) * PERIODS.year;
    return Math.abs(value) >= 1e11 ? value : null;
  }
  const s = String(value).trim();
  const m = /^(\d{4})(?:-(\d{1,2})(?:-(\d{1,2})(?:[T ](\d{1,2}):(\d{2})(?::(\d{2}(?:\.\d+)?))?\s*(Z|[+-]\d{2}:?\d{2})?)?)?)?$/.exec(s);
  if (!m) return null;
  const [y, mo, d, h, mi, sec] = [m[1], m[2] || 1, m[3] || 1, m[4] || 0, m[5] || 0, m[6] || 0].map(Number);
  if (mo < 1 || mo > 12 || d < 1 || d > 31 || h > 23 || mi > 59) return null;
  let t = Date.UTC(y, mo - 1, d, h, mi, Math.floor(sec), Math.round((sec % 1) * 1000));
  if (m[7] && m[7] !== 'Z') { const sign = m[7][0] === '-' ? 1 : -1, hh = Number(m[7].slice(1, 3)), mm = Number(m[7].slice(-2)); t += sign * (hh * 60 + mm) * 60e3; }
  return t;
}

// The period [from, until) a row stands for: its time, and its until or the end of the period that holds its time (a row of the
// year 2018 stands for that calendar year).
export function span(table, row, period = 'year') {
  const roles = table.roles || {};
  if (!roles.time) return null;
  const from = instant(table.data[roles.time][row]);
  if (from === null) return null;
  const until = roles.until ? instant(table.data[roles.until][row]) : null;
  return { from, until: until === null ? (roles.until ? Infinity : periodEnd(from, period)) : until };
}

// The rows that stand at an instant: all of them when the table has no time.
export function rowsAt(table, t, period = 'year') {
  const out = [];
  if (!table.roles?.time) { for (let i = 0; i < table.rows; i++) out.push(i); return out; }
  for (let i = 0; i < table.rows; i++) { const s = span(table, i, period); if (s && s.from <= t && t < s.until) out.push(i); }
  return out;
}

// The instants a clock stops at over an interval, a period apart, from the interval's beginning (calendar years and months
// on their first days).
export function ticks(from, until, period = 'year') {
  const out = [];
  if (!(until > from)) return out;
  if (period === 'year' || period === 'month' || period === 'quarter') {
    const step = { year: 12, quarter: 3, month: 1 }[period], d = new Date(from);
    let y = d.getUTCFullYear(), m = period === 'year' ? 0 : Math.floor(d.getUTCMonth() / step) * step;
    for (let t = Date.UTC(y, m, 1); t < until && out.length < 5000; m += step, t = Date.UTC(y, m, 1)) if (t >= from - 1) out.push(t);
    return out;
  }
  for (let t = from; t < until && out.length < 5000; t += PERIODS[period] || PERIODS.day) out.push(t);
  return out;
}

export const formatInstant = (t, period = 'year') => {
  const iso = new Date(t).toISOString();
  return period === 'year' ? iso.slice(0, 4) : period === 'month' || period === 'quarter' ? iso.slice(0, 7) : period === 'hour' ? `${iso.slice(0, 10)} ${iso.slice(11, 16)}` : iso.slice(0, 10);
};

// ---- slices -----------------------------------------------------------------------------------------------------------------

export function newSlice({ name = 'All', region = null, from = null, until = null, period = 'year' } = {}) {
  return { id: ID(), name, region, time: from === null && until === null ? null : { from, until }, period };
}

// A slice's interval in milliseconds: [from, until), until taken to the end of its last period when it is written as a year or a date.
export function interval(slice) {
  if (!slice?.time) return null;
  const from = instant(slice.time.from), last = instant(slice.time.until);
  if (from === null && last === null) return null;
  const end = last === null ? Infinity : periodEnd(last, slice.period);
  return { from: from === null ? -Infinity : from, until: end };
}
export function periodEnd(t, period) {
  const d = new Date(t);
  if (period === 'year') return Date.UTC(d.getUTCFullYear() + 1, 0, 1);
  if (period === 'month') return Date.UTC(d.getUTCFullYear(), d.getUTCMonth() + 1, 1);
  if (period === 'quarter') return Date.UTC(d.getUTCFullYear(), d.getUTCMonth() + 3, 1);
  return t + (PERIODS[period] || PERIODS.day);
}
export const inRegion = (region, lon, lat) => !region || (lon >= region.west && lon <= region.east && lat >= region.south && lat <= region.north);

// The rows of a table within a slice: by their place where the table has one, by their time where it has one.
export function rowsIn(table, slice) {
  const roles = table.roles || {}, when = interval(slice), region = slice?.region || null, period = slice?.period || 'year';
  const centre = table.places?.rings ? centres(table.places.rings) : null;
  const out = [];
  for (let i = 0; i < table.rows; i++) {
    if (region) {
      if (roles.lat && roles.lon) { if (!inRegion(region, table.data[roles.lon][i], table.data[roles.lat][i])) continue; }
      else if (roles.cell && centre) { const c = centre.get(String(table.data[roles.cell][i])); if (!c || !inRegion(region, c[0], c[1])) continue; }
    }
    if (when && roles.time) { const s = span(table, i, period); if (!s || s.until <= when.from || s.from >= when.until) continue; }
    out.push(i);
  }
  return out;
}

// The middle of each ring (the mean of its vertices): where an area is, for a region's test and for a label.
export function centres(rings) {
  const out = new Map();
  for (const [id, ring] of Object.entries(rings)) {
    let x = 0, y = 0;
    for (const p of ring) { x += p[0]; y += p[1]; }
    if (ring.length) out.set(String(id), [x / ring.length, y / ring.length]);
  }
  return out;
}

// A table with only some of its rows.
export function takeRows(table, rows) {
  const data = {};
  for (const c of table.columns) data[c.name] = rows.map((i) => table.data[c.name][i]);
  return { ...table, rows: rows.length, data };
}

// ---- roles ------------------------------------------------------------------------------------------------------------------

const GUESS = {
  lat: /^(lat|latitude|y_?coord|lat_?dd)$/i, lon: /^(lon|lng|long|longitude|x_?coord|lon_?dd)$/i,
  time: /^(time|date|datetime|timestamp|year|census_year|period|month|day|opened|open_date|from|start|start_date)$/i,
  until: /^(until|to|end|end_date|closed|close_date)$/i, key: /^(id|key|uid|code|property_id|outlet_id|store_id)$/i,
  name: /^(name|title|label|trading_name|store|outlet|brand)$/i, amount: /^(amount|value|sales|revenue|count|total|people|population|spend|visits)$/i,
  category: /^(category|kind|type|class|group|industry|segment|sector)$/i, cell: /^(cell|block|block_id|area|zone|region|iso3|h3)$/i,
};

// The roles a table's columns most likely play, by their names and by what they hold: for the person to confirm or change.
export function guessRoles(table) {
  const roles = {};
  const numeric = (c) => c.kind === 'number';
  for (const role of ROLES) {
    const need = role.kind === 'number';
    const found = table.columns.find((c) => GUESS[role.id].test(c.name) && (!need || numeric(c)) && !Object.values(roles).includes(c.name));
    if (found) roles[role.id] = found.name;
  }
  // A latitude without a longitude, or values that are not degrees, place nothing.
  const ok = (col, most) => { const v = table.data[col].filter((x) => typeof x === 'number'); return v.length > 0 && v.every((x) => Math.abs(x) <= most); };
  if (!(roles.lat && roles.lon && ok(roles.lat, 90) && ok(roles.lon, 180))) { delete roles.lat; delete roles.lon; }
  if (roles.time && !table.data[roles.time].some((v) => instant(v) !== null)) delete roles.time;
  if (roles.until && !roles.time) delete roles.until;
  if (roles.cell && !table.places) delete roles.cell;
  return roles;
}

// ---- soundness ---------------------------------------------------------------------------------------------------------------

// Everything wrong with a project, in words; empty when it is sound.
export function checkProject(p) {
  const wrong = [];
  if (!p || p.schema !== SCHEMA) return [`not a project of the schema ${SCHEMA}`];
  for (const k of ['sources', 'slices', 'quantities', 'steps', 'views', 'boards']) if (!Array.isArray(p[k])) wrong.push(`a project has ${k}`);
  if (wrong.length) return wrong;
  const names = new Map(), ids = new Set();
  const take = (id, what) => { if (!id || ids.has(id)) wrong.push(`${what} without an id of its own`); ids.add(id); };
  for (const s of p.sources) {
    take(s.id, 'a source');
    if (!NAME.test(s.name || '')) wrong.push(`the source '${s.title || s.id}' has no name of letters, digits and _`);
    else if (names.has(s.name)) wrong.push(`two tables are named '${s.name}'`);
    else names.set(s.name, `source:${s.id}`);
    if (!s.ref) wrong.push(`the source '${s.name}' names no file`);
    for (const [role, column] of Object.entries(s.roles || {})) if (!ROLES.some((r) => r.id === role) || typeof column !== 'string') wrong.push(`the source '${s.name}' binds '${role}', which is no role`);
  }
  for (const s of p.slices) {
    take(s.id, 'a slice');
    const r = s.region;
    if (r && !(r.west < r.east && r.south < r.north && r.south >= -90 && r.north <= 90)) wrong.push(`the slice '${s.name}' has a region that is none`);
    const i = interval(s);
    if (s.time && (!i || !(i.until > i.from))) wrong.push(`the slice '${s.name}' has a time that ends before it begins`);
    if (!PERIODS[s.period]) wrong.push(`the slice '${s.name}' has no period`);
  }
  const quantities = new Set();
  for (const q of p.quantities) {
    if (!NAME.test(q.name || '') || quantities.has(q.name)) wrong.push(`a quantity needs a name of its own: '${q.name}'`);
    quantities.add(q.name);
    if (!(q.low <= q.mid && q.mid <= q.high)) wrong.push(`the quantity '${q.name}' is not low ≤ mid ≤ high`);
  }
  for (const s of p.steps) {
    take(s.id, 'a step');
    if (s.slice && !p.slices.some((x) => x.id === s.slice)) wrong.push(`the step '${s.title || s.tool}' is of a slice that is not there`);
    for (const o of s.outputs || []) if (o.kind === 'table') {
      if (names.has(o.name) && names.get(o.name) !== `step:${s.id}`) wrong.push(`two tables are named '${o.name}'`);
      names.set(o.name, `step:${s.id}`);
    }
  }
  for (const v of p.views) {
    take(v.id, 'a view');
    if (!['layer', 'chart'].includes(v.kind)) wrong.push(`the view '${v.title}' is neither a layer nor a chart`);
    if (!v.table) wrong.push(`the view '${v.title}' draws no table`);
  }
  for (const b of p.boards) take(b.id, 'a board');
  return wrong;
}

// Every table of a project by its name: where it comes from.
export function tablesOf(p) {
  const out = new Map();
  for (const s of p.sources) out.set(s.name, { from: 'source', id: s.id, title: s.title || s.name, changed: s.changed || null });
  for (const s of p.steps) for (const o of s.outputs || []) if (o.kind === 'table') out.set(o.name, { from: 'step', id: s.id, title: o.title || o.name, changed: s.ran || null, rows: o.rows });
  return out;
}

// The tables a step was given: those its parameters name (a tool says which of its parameters name tables).
export function inputsOf(step, tool) {
  const out = [];
  for (const q of tool?.params || []) if (q.role === 'table' && step.params?.[q.name]) out.push(step.params[q.name]);
  return out;
}

// A step is stale when a table it was given, a quantity or its slice changed after it ran: its answer is of an earlier state.
export function staleSteps(p, tools) {
  const tables = tablesOf(p), stale = new Set();
  const later = (stamp, ran) => stamp && ran && stamp > ran;
  let grew = true;
  while (grew) {
    grew = false;
    for (const s of p.steps) {
      if (stale.has(s.id) || !s.ran) continue;
      const tool = tools.find((t) => t.ref === s.tool);
      const inputs = inputsOf(s, tool).map((n) => tables.get(n)).filter(Boolean);
      const slice = p.slices.find((x) => x.id === s.slice);
      if (inputs.some((t) => later(t.changed, s.ran) || (t.from === 'step' && stale.has(t.id))) || later(slice?.changed, s.ran) || later(p.meta?.quantities, s.ran)) { stale.add(s.id); grew = true; }
    }
  }
  return stale;
}

// ---- the plane of a project ---------------------------------------------------------------------------------------------------

// Degrees to metres east and north of a project's frame, and back: a plane that is true at the frame's point (equirectangular
// about it). Within a city its error is below a part in ten thousand; across a continent it stretches east and west, and says so.
export function plane(frame) {
  const k = Math.cos((frame.lat * Math.PI) / 180), E = 111319.49 * k, N = 110574;
  return { to: (lon, lat) => [(lon - frame.lon) * E, (lat - frame.lat) * N], from: (x, y) => [frame.lon + x / E, frame.lat + y / N] };
}
