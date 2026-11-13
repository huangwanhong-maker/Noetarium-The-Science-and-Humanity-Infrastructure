// The views of a workspace: what a layer of the map and a chart make of a table. Pure: they answer numbers and labels,
// which the page draws. The predefined kinds are meant to cover what is asked most often, as a spreadsheet's charts do;
// what they do not cover is a tool's to compute and answer as a table, or a figure of its own.
//
//   charts   bars · stacked bars · lines · points · a histogram · shares (a pie's numbers, drawn as one bar)
//   layers   points (alike, or sized and coloured by a column) · areas coloured by a column · lines
//
// A chart may gather the rows first: by a column (and a second, for series), taking the count, the sum, the mean, the least
// or the greatest of another.

export const CHARTS = [
  { id: 'bar', label: 'Bars', about: 'an amount for each category' },
  { id: 'stacked', label: 'Stacked bars', about: 'an amount for each category, split by a second' },
  { id: 'line', label: 'Lines', about: 'an amount along a column of numbers or times' },
  { id: 'scatter', label: 'Points', about: 'one column of numbers against another' },
  { id: 'histogram', label: 'Histogram', about: 'how often a column takes its values' },
  { id: 'share', label: 'Shares', about: 'each category\'s part of the whole' },
];
export const GATHER = ['count', 'sum', 'mean', 'least', 'greatest'];
export const LAYERS = [
  { id: 'points', label: 'Points' }, { id: 'areas', label: 'Areas' }, { id: 'lines', label: 'Lines' },
];

const isNumber = (v) => typeof v === 'number' && Number.isFinite(v);

// Rows gathered by the values of a column (and of a second): {keys, series: [{name, values}]}, the keys in the order they first
// appear unless they are numbers, which are sorted; `how` is one of GATHER, of the column `of` (the count needs none).
export function gather(table, { by, split = null, of = null, how = 'count', rows = null }) {
  if (!table.data[by]) throw new Error(`the table has no column '${by}'`);
  if (how !== 'count' && !table.data[of]) throw new Error(`the ${how} is of a column: choose one`);
  const index = rows || Array.from({ length: table.rows }, (_, i) => i);
  const keys = [], seen = new Map(), parts = [], partsSeen = new Map();
  const SEP = String.fromCharCode(31), cell = (k, s) => `${k}${SEP}${s}`;
  const acc = new Map();
  for (const i of index) {
    const key = table.data[by][i], part = split ? table.data[split][i] : '';
    if (key === null || key === undefined || key === '') continue;
    if (!seen.has(key)) { seen.set(key, keys.length); keys.push(key); }
    if (!partsSeen.has(part)) { partsSeen.set(part, parts.length); parts.push(part); }
    const v = how === 'count' ? 1 : table.data[of][i];
    if (how !== 'count' && !isNumber(v)) continue;
    const a = acc.get(cell(key, part)) || { n: 0, sum: 0, least: Infinity, greatest: -Infinity };
    a.n++; a.sum += v; a.least = Math.min(a.least, v); a.greatest = Math.max(a.greatest, v);
    acc.set(cell(key, part), a);
  }
  if (keys.every(isNumber)) keys.sort((a, b) => a - b);
  const value = (a) => (!a ? (how === 'count' || how === 'sum' ? 0 : null) : how === 'count' ? a.n : how === 'sum' ? a.sum : how === 'mean' ? a.sum / a.n : how === 'least' ? a.least : a.greatest);
  return { keys, series: parts.map((part) => ({ name: split ? String(part ?? '') : how === 'count' ? 'count' : how === 'sum' ? of : `${how} of ${of}`, values: keys.map((k) => value(acc.get(cell(k, part)))) })) };
}

// A histogram of a column's numbers: `bins` equal intervals from its least to its greatest value.
export function histogram(values, bins = 12) {
  const v = values.filter(isNumber);
  if (!v.length) return { edges: [], counts: [] };
  const lo = Math.min(...v), hi = Math.max(...v), n = hi > lo ? Math.max(1, Math.min(200, Math.floor(bins))) : 1, w = (hi - lo) / n || 1;
  const counts = new Array(n).fill(0);
  for (const x of v) counts[Math.min(n - 1, Math.floor((x - lo) / w))]++;
  return { edges: Array.from({ length: n + 1 }, (_, k) => lo + k * w), counts };
}

const short = (v) => (isNumber(v) ? String(+v.toPrecision(4)) : String(v ?? ''));

// What a chart draws of a table: {kind: 'bars' | 'lines' | 'scatter', …} in the shapes of the toolbox's charts, or {note} saying
// what it still needs. `view` is {chart, x, y, split, how, bins, most}.
export function chartOf(table, view, rows = null) {
  const kind = view.chart || 'bar', numeric = table.columns.filter((c) => c.kind === 'number').map((c) => c.name), all = table.columns.map((c) => c.name);
  const title = view.title || table.title || table.name;
  const x = table.data[view.x] ? view.x : null, y = table.data[view.y] ? view.y : null;
  const index = rows || Array.from({ length: table.rows }, (_, i) => i);
  if (kind === 'histogram') {
    const col = y || x || numeric[0];
    if (!col || !numeric.includes(col)) return { note: 'A histogram is of a column of numbers: choose one.' };
    const h = histogram(index.map((i) => table.data[col][i]), view.bins || 12);
    return { kind: 'bars', title, labels: h.counts.map((_, k) => `${short(h.edges[k])}–${short(h.edges[k + 1])}`), series: [{ name: col, values: h.counts }], xlabel: col };
  }
  if (kind === 'scatter') {
    const a = x && numeric.includes(x) ? x : numeric[0], b = y && numeric.includes(y) ? y : numeric.find((c) => c !== a);
    if (!a || !b) return { note: 'Points need two columns of numbers.' };
    const keep = index.filter((i) => isNumber(table.data[a][i]) && isNumber(table.data[b][i]));
    const label = table.roles?.name || all.find((c) => !numeric.includes(c));
    // A split colours the points by a column's values, each named in a legend.
    const split = view.split && table.data[view.split] ? view.split : null;
    return { kind: 'scatter', title, x: keep.map((i) => table.data[a][i]), y: keep.map((i) => table.data[b][i]), xlabel: a, ylabel: b, labels: label ? keep.map((i) => String(table.data[label][i] ?? '')) : null,
      groups: split ? keep.map((i) => String(table.data[split][i] ?? '')) : null };
  }
  const by = x || table.roles?.category || table.roles?.name || all.find((c) => !numeric.includes(c)) || all[0];
  if (!by) return { note: 'Choose the column the chart runs along.' };
  const how = view.how || (y ? 'sum' : 'count');
  if (how !== 'count' && !(y && numeric.includes(y))) return { note: `The ${how} is of a column of numbers: choose one.` };
  const g = gather(table, { by, split: kind === 'stacked' || view.split ? view.split || null : null, of: y, how, rows: index });
  if (kind === 'line') {
    const xs = g.keys.every(isNumber) ? g.keys : g.keys.map((_, k) => k + 1);
    return { kind: 'lines', title, x: xs, series: g.series.map((s) => ({ name: s.name, y: s.values })), xlabel: by, ylabel: how === 'count' ? 'count' : how === 'sum' ? y : `${how} of ${y}`, ticks: g.keys.every(isNumber) ? null : g.keys.map(String) };
  }
  // Bars: the greatest first unless the categories are numbers (years stay in their order); at most `most` of them.
  let order = g.keys.map((_, k) => k);
  if (!g.keys.every(isNumber)) order.sort((p, q) => g.series.reduce((s, r) => s + (r.values[q] || 0), 0) - g.series.reduce((s, r) => s + (r.values[p] || 0), 0));
  const most = Math.max(2, view.most || 24), shown = order.slice(0, most);
  let series = g.series.map((s) => ({ name: s.name, values: shown.map((k) => s.values[k] ?? 0) }));
  if (kind === 'share') {
    const total = g.series[0].values.reduce((a, v) => a + (v || 0), 0) || 1;
    series = [{ name: 'share, %', values: shown.map((k) => (100 * (g.series[0].values[k] || 0)) / total) }];
  }
  return { kind: 'bars', title: order.length > most ? `${title} (the first ${most} of ${order.length})` : title, labels: shown.map((k) => String(g.keys[k])), series, stacked: kind === 'stacked' };
}

// A scale of a column for a layer: of numbers, from its least to its greatest (by logarithms when positive over more than two
// decades, by square roots otherwise when `root`); of text, an index for each kind in the order it first appears.
export function scaleOf(values, { root = false } = {}) {
  const numbers = values.filter(isNumber);
  if (numbers.length && numbers.length >= values.filter((v) => v !== null && v !== undefined && v !== '').length) {
    const lo = Math.min(...numbers), hi = Math.max(...numbers), log = lo > 0 && hi / lo > 100;
    const u = (v) => (!isNumber(v) ? null : hi > lo ? (log ? Math.log(v / lo) / Math.log(hi / lo) : root ? Math.sqrt((v - Math.min(0, lo)) / (hi - Math.min(0, lo))) : (v - lo) / (hi - lo)) : 0.5);
    return { kind: 'number', lo, hi, log, root: root && !log, u };
  }
  const kinds = [...new Set(values.filter((v) => v !== null && v !== undefined && v !== ''))].slice(0, 400), index = new Map(kinds.map((k, i) => [k, i]));
  return { kind: 'text', kinds, u: (v) => (index.has(v) ? index.get(v) : null) };
}

// The kind of layer a table can be drawn as, from its roles: points, areas, or none.
export function layerKind(table) {
  const r = table.roles || {};
  if (table.lines?.length) return 'lines';
  if (r.lat && r.lon) return 'points';
  if (r.cell && table.places?.rings) return 'areas';
  return null;
}

// A view of a table when nothing was said: a layer where it has a place, else a chart of what its columns suggest.
export function defaultView(table) {
  const kind = layerKind(table), numeric = table.columns.filter((c) => c.kind === 'number').map((c) => c.name), r = table.roles || {};
  const quiet = new Set([r.lat, r.lon, r.time, r.until, r.cell, r.key].filter(Boolean));
  const value = r.amount || numeric.find((c) => !quiet.has(c)) || null;
  if (kind === 'points' || kind === 'areas') return { kind: 'layer', layer: kind, colour: value, size: kind === 'points' ? value : null };
  if (kind === 'lines') return { kind: 'layer', layer: 'lines' };
  if (r.time && value) return { kind: 'chart', chart: 'line', x: r.time, y: value, how: 'sum' };
  const by = r.category || r.name || table.columns.find((c) => c.kind !== 'number')?.name;
  return { kind: 'chart', chart: 'bar', x: by || table.columns[0]?.name, y: value, how: value ? 'sum' : 'count' };
}
