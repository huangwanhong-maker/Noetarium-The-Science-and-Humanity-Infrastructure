// Tables on a board: data set beside the thought it serves. A table is the data library's: named
// columns of equal length, each of numbers (null where missing) or of text,
//
//     {name, title, source, rows, columns: [{name, kind: 'number' | 'text', label?}], data: {column: [values]}}
//
// It comes to a board pasted or dropped as delimited text, or as the answer of a module; whatever made
// it, a chart, a map or a module draws it by its name. Pure.

import { parseDefinition } from './quantities.js';

const NAME = /^[A-Za-z_][A-Za-z0-9_]*$/;

// A name a table can go by, from any words: letters, digits and _, not beginning with a digit.
export function tableName(words, taken = new Set()) {
  let base = String(words || '').trim().toLowerCase().replace(/\.[a-z0-9]+$/, '').replace(/[^a-z0-9_]+/g, '_').replace(/^_+|_+$/g, '');
  if (!base) base = 'table';
  if (/^[0-9]/.test(base)) base = `t_${base}`;
  let name = base;
  for (let k = 2; taken.has(name); k++) name = `${base}_${k}`;
  return name;
}

// Delimited text split into rows of fields: the delimiter is the tab, the semicolon or the comma that
// divides its first lines most evenly; a field in double quotes may hold the delimiter, line breaks and
// doubled quotes (RFC 4180).
export function splitDelimited(text) {
  const raw = String(text || '');
  // A byte order mark at the head of a file is not part of its first name.
  const body = (raw.charCodeAt(0) === 0xfeff ? raw.slice(1) : raw).replace(/\r\n?/g, '\n');
  // The first lines as they are meant: a line break within quotes does not end a line, and a
  // delimiter within quotes does not divide one.
  const head = [];
  for (let i = 0, line = '', quoted = false; i <= body.length && head.length < 8; i++) {
    const c = body[i];
    if (c === '"') quoted = !quoted;
    if (c === undefined || (c === '\n' && !quoted)) { if (line.trim()) head.push(line); line = ''; } else line += quoted && c !== '"' ? ' ' : c;
  }
  let delimiter = ',', best = 0;
  for (const d of ['\t', ';', ',']) {
    const counts = head.map((l) => l.split(d).length - 1);
    const even = counts.length && counts[0] > 0 && counts.every((c) => c === counts[0]);
    const score = even ? counts[0] + 1000 : Math.min(...counts);
    if (score > best) { best = score; delimiter = d; }
  }
  const rows = [];
  let row = [], field = '', quoted = false, started = false;
  for (let i = 0; i < body.length; i++) {
    const c = body[i];
    if (quoted) {
      if (c === '"') { if (body[i + 1] === '"') { field += '"'; i++; } else quoted = false; } else field += c;
    } else if (c === '"' && !started) { quoted = true; started = true; }
    else if (c === delimiter) { row.push(field); field = ''; started = false; }
    else if (c === '\n') { row.push(field); rows.push(row); row = []; field = ''; started = false; }
    else { field += c; started = true; }
  }
  if (field !== '' || row.length) { row.push(field); rows.push(row); }
  return { delimiter, rows: rows.filter((r) => r.some((f) => f.trim() !== '')) };
}

// Whether text reads as a table rather than as prose: several lines, each divided alike.
export function looksTabular(text) {
  const lines = String(text || '').replace(/\r\n?/g, '\n').split('\n').filter((l) => l.trim());
  if (lines.length < 2) return false;
  const { rows } = splitDelimited(lines.slice(0, 12).join('\n'));
  return rows.length >= 2 && rows[0].length >= 2 && rows.every((r) => r.length === rows[0].length);
}

const asNumber = (s) => {
  const t = s.trim().replace(/^(-?)\$/, '$1');
  if (t === '') return null;
  const plain = /^-?\d{1,3}(,\d{3})+(\.\d+)?$/.test(t) ? t.replace(/,/g, '') : t;
  if (/%$/.test(plain)) { const v = Number(plain.slice(0, -1)); return Number.isFinite(v) ? v / 100 : NaN; }
  const v = Number(plain);
  return Number.isFinite(v) ? v : NaN;
};

// Delimited text as a table: its first row names the columns; a column is of numbers when every field
// that is not empty is one (thousands separated by commas, a share written with %), else of text. An
// empty field is null. Rows short of the header are filled with nulls; fields beyond it are refused.
export function parseDelimited(text, { name = 'table', title = '', source = '' } = {}) {
  const { rows } = splitDelimited(text);
  if (rows.length < 2) throw new Error('a table has a row of names and at least one row beneath it');
  const header = rows[0].map((h, k) => h.trim() || `column_${k + 1}`);
  const seen = new Set();
  const names = header.map((h) => { let n = h; for (let k = 2; seen.has(n); k++) n = `${h}_${k}`; seen.add(n); return n; });
  const long = rows.findIndex((r, k) => k > 0 && r.length > names.length);
  if (long > 0) throw new Error(`row ${long + 1} has ${rows[long].length} fields under ${names.length} names`);
  const columns = [], data = {};
  names.forEach((n, c) => {
    const fields = rows.slice(1).map((r) => (r[c] ?? '').trim());
    const numbers = fields.map(asNumber);
    const numeric = numbers.every((v) => v === null || !Number.isNaN(v)) && numbers.some((v) => v !== null);
    columns.push({ name: n, kind: numeric ? 'number' : 'text' });
    data[n] = numeric ? numbers : fields.map((f) => (f === '' ? null : f));
  });
  return { name, title, source, rows: rows.length - 1, columns, data };
}

// Everything wrong with a table, in words; empty when it is sound.
export function checkTable(t) {
  const problems = [];
  if (!t || typeof t !== 'object') return ['a table is an object'];
  if (!NAME.test(t.name || '')) problems.push(`'${t.name}' is not a name: letters, digits and _, not beginning with a digit`);
  if (!Array.isArray(t.columns) || !t.data || typeof t.data !== 'object') return [...problems, 'a table has columns and data'];
  const lengths = new Set();
  for (const c of t.columns) {
    const values = t.data[c.name];
    if (!Array.isArray(values)) { problems.push(`the column '${c.name}' has no values`); continue; }
    lengths.add(values.length);
    if (c.kind === 'number' && values.some((v) => v !== null && !(typeof v === 'number' && Number.isFinite(v)))) problems.push(`the column '${c.name}' is of numbers and holds what is not one`);
  }
  if (lengths.size > 1) problems.push('its columns differ in length');
  return problems;
}

export const columnNames = (t, kind = null) => (t?.columns || []).filter((c) => !kind || c.kind === kind).map((c) => c.name);
// The first column that goes by one of the names given, whatever its case.
export const findColumn = (t, ...names) => (t?.columns || []).find((c) => names.includes(c.name.toLowerCase()))?.name || null;

// A table as delimited text, to take elsewhere.
export function toDelimited(t, delimiter = ',') {
  const quote = (v) => { const s = v === null || v === undefined ? '' : String(v); return /["\n]/.test(s) || s.includes(delimiter) ? `"${s.replace(/"/g, '""')}"` : s; };
  const names = columnNames(t);
  const lines = [names.map(quote).join(delimiter)];
  for (let r = 0; r < t.rows; r++) lines.push(names.map((n) => quote(t.data[n][r])).join(delimiter));
  return lines.join('\n');
}

// Every table of a board by its name: those laid on it (items of the kind `table`) and those its
// modules have answered (the `out` of a card). A name two items give is refused for both, as a
// quantity defined twice is. Returns a map from each name to the table (with `item`, who gives it) or
// to {error, item}.
export function collectTables(board) {
  const out = new Map(), twice = new Set();
  const give = (table, item) => {
    if (!table?.name) return;
    if (out.has(table.name)) twice.add(table.name);
    else out.set(table.name, { ...table, item });
  };
  for (const id of board.order) {
    const item = board.items[id];
    if (item.kind === 'table') give(item, id);
    else if (item.kind === 'card') for (const t of item.out?.tables || []) give(t, id);
  }
  for (const name of twice) out.set(name, { name, error: `'${name}' is the name of more than one table on the board`, item: out.get(name).item });
  return out;
}

// What a module is given of a board: its tables by name, its quantities, its notes.
export function boardForModules(board, tables, values) {
  const given = { tables: {}, quantities: {}, notes: [] };
  for (const [name, t] of tables) if (!t.error) given.tables[name] = { title: t.title || '', source: t.source || '', columns: t.columns, data: t.data };
  for (const [name, v] of values) if (!v.error) given.quantities[name] = { lo: v.lo, mid: v.mid, hi: v.hi, unit: v.unit || '' };
  for (const id of board.order) {
    const item = board.items[id];
    // The words of a note without the lines that only define quantities.
    if (item.kind === 'note') given.notes.push({ type: item.type, text: String(item.text || '').split('\n').filter((l) => !parseDefinition(l)).join('\n'), source: item.source || '' });
  }
  return given;
}
