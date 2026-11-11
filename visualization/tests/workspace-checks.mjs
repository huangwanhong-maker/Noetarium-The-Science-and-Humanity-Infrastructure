// Checks of the workspace (visualization/js/workspace): instants and periods against JavaScript's own calendar, the rows of a
// table within a slice and at an instant (a row of a year standing for that calendar year), the roles guessed for a table's
// columns, a project's soundness, staleness carried from a table to every step that stands on it, the project's plane, and
// the views: what a chart gathers (counts and sums against brute force), a histogram, shares, the scale of a column, and the
// view a table suggests. Called by run-tests.mjs with the suite's check and assert.

import { seeded } from '../../development/tools/checks.mjs';
import { workspace as W } from '../js/index.js';

const table = (columns, more = {}) => ({ name: 't', title: 'T', rows: Object.values(columns)[0].length,
  columns: Object.keys(columns).map((c) => ({ name: c, kind: columns[c].every((v) => v === null || typeof v === 'number') ? 'number' : 'text' })), data: columns, ...more });

export function workspaceChecks({ assert, check }) {
  // ---- instants and periods ----
  {
    const cases = [[2018, Date.UTC(2018, 0, 1)], ['2018', Date.UTC(2018, 0, 1)], ['2024-03', Date.UTC(2024, 2, 1)], ['2024-03-09', Date.UTC(2024, 2, 9)], ['2024-03-09T06:30', Date.UTC(2024, 2, 9, 6, 30)],
      ['2024-03-09 06:30:15Z', Date.UTC(2024, 2, 9, 6, 30, 15)], ['2024-03-09T06:30+10:00', Date.UTC(2024, 2, 8, 20, 30)], [1710000000000, 1710000000000]];
    assert('workspace: an instant is read from a year, a month, a date, a date and a time with or without a zone, or milliseconds', cases.every(([v, t]) => W.instant(v) === t), cases.filter(([v, t]) => W.instant(v) !== t).map(([v]) => v).join(', '));
    assert('workspace: what is no instant is none: empty, a word, a small number, a thirteenth month', [null, '', 'soon', 17, '2024-13-01', NaN].every((v) => W.instant(v) === null));
    const t = Date.UTC(2023, 10, 20);
    assert('workspace: a period ends with its calendar: the year, the quarter and the month on their first days, a day a day later',
      W.periodEnd(t, 'year') === Date.UTC(2024, 0, 1) && W.periodEnd(Date.UTC(2023, 9, 1), 'quarter') === Date.UTC(2024, 0, 1) && W.periodEnd(t, 'month') === Date.UTC(2023, 11, 1) && W.periodEnd(t, 'day') === t + 86400e3);
    const years = W.ticks(Date.UTC(2002, 0, 1), Date.UTC(2025, 0, 1), 'year'), months = W.ticks(Date.UTC(2023, 10, 1), Date.UTC(2024, 2, 1), 'month');
    assert('workspace: a clock stops once a period: 23 years from 2002 to 2024, four months from November to February',
      years.length === 23 && W.formatInstant(years[22], 'year') === '2024' && months.map((m) => W.formatInstant(m, 'month')).join(' ') === '2023-11 2023-12 2024-01 2024-02');
    const slice = W.newSlice({ from: 2019, until: 2021 }), i = W.interval(slice);
    assert('workspace: a slice written 2019 to 2021 holds three calendar years, its last year within it', i.from === Date.UTC(2019, 0, 1) && i.until === Date.UTC(2022, 0, 1) && W.interval(W.newSlice()) === null);
  }

  // ---- the rows of a table within a slice, and at an instant ----
  {
    const t = table({ year: [2017, 2018, 2019, 2020, 2021, 2022], lat: [-37.81, -37.81, -37.81, -37.9, -37.81, -37.81], lon: [144.96, 144.96, 144.96, 144.96, 145.4, 144.96] }, { roles: { time: 'year', lat: 'lat', lon: 'lon' } });
    const region = { west: 144.9, east: 145.0, south: -37.85, north: -37.78 };
    const inTime = W.rowsIn(t, W.newSlice({ from: 2019, until: 2021 })), inBoth = W.rowsIn(t, W.newSlice({ from: 2019, until: 2021, region })), inPlace = W.rowsIn(t, W.newSlice({ region }));
    assert('workspace: a slice keeps the rows of its years (2018 is not in 2019–2021), of its region, and of both', inTime.join() === '2,3,4' && inBoth.join() === '2' && inPlace.join() === '0,1,2,5', `${inTime} | ${inBoth} | ${inPlace}`);
    assert('workspace: no slice keeps every row; a table without a time is whole at every instant', W.rowsIn(t, null).length === 6 && W.rowsAt(table({ a: [1, 2] }), 0).length === 2);
    assert('workspace: at an instant stand the rows whose period holds it: mid-2020 is the row of 2020', W.rowsAt(t, Date.UTC(2020, 6, 1)).join() === '3' && W.rowsAt(t, Date.UTC(2030, 0, 1)).length === 0);
    const lives = table({ from: [2010, 2012, 2015], until: [2013, null, 2016] }, { roles: { time: 'from', until: 'until' } });
    assert('workspace: what lasts stands from its beginning to its end, and without an end from then on', W.rowsAt(lives, Date.UTC(2012, 5, 1)).join() === '0,1' && W.rowsAt(lives, Date.UTC(2013, 5, 1)).join() === '1' && W.rowsAt(lives, Date.UTC(2040, 0, 1)).join() === '1');
    const cells = table({ block: ['a', 'b'], n: [1, 2] }, { roles: { cell: 'block' }, places: { kind: 'cells', column: 'block', rings: { a: [[0, 0], [1, 0], [1, 1], [0, 1]], b: [[5, 5], [6, 5], [6, 6], [5, 6]] } } });
    assert('workspace: an area is in a region when its middle is', W.rowsIn(cells, W.newSlice({ region: { west: -1, east: 2, south: -1, north: 2 } })).join() === '0' && W.centres(cells.places.rings).get('b').join() === '5.5,5.5');
    assert('workspace: a table with some of its rows keeps its columns and its roles', W.takeRows(t, [1, 4]).rows === 2 && W.takeRows(t, [1, 4]).data.year.join() === '2018,2021' && W.takeRows(t, [1]).roles.time === 'year');
  }

  // ---- roles ----
  {
    const t = table({ census_year: [2002, 2003], trading_name: ['A', 'B'], latitude: [-37.8, -37.9], longitude: [144.9, 145.0], sales: [10, 20], industry: ['x', 'y'], block_id: [1, 2] });
    const r = W.guessRoles(t);
    assert('workspace: a table\'s columns are guessed by their names: latitude, longitude, the time, the name, the amount, the kind', r.lat === 'latitude' && r.lon === 'longitude' && r.time === 'census_year' && r.name === 'trading_name' && r.amount === 'sales' && r.category === 'industry', JSON.stringify(r));
    const odd = W.guessRoles(table({ lat: [1000, 2000], lon: [1, 2], date: ['soon', 'later'], block: ['a', 'b'] }));
    assert('workspace: what is not degrees is no place, what is no instant no time, and a cell without outlines no cell', !odd.lat && !odd.lon && !odd.time && !odd.cell, JSON.stringify(odd));
    assert('workspace: every role is described, and a number is asked of the roles that need one', W.ROLES.length === 9 && W.ROLES.every((x) => x.label && x.about) && W.ROLES.filter((x) => x.kind === 'number').map((x) => x.id).join() === 'lat,lon,amount');
  }

  // ---- a project ----
  {
    const p = W.emptyProject({ title: 'A check', now: '2026-11-11T00:00:00Z' });
    p.sources.push({ id: 's1', name: 'cafes', title: 'Cafés', ref: 'project:data/cafes.csv', roles: { lat: 'lat' }, changed: '2026-11-11T01:00:00Z' });
    p.slices.push({ ...W.newSlice({ name: 'Grid', from: 2024, until: 2024 }), id: 'sl1' });
    p.quantities.push({ name: 'weeks', low: 46, mid: 48, high: 50 });
    p.steps.push({ id: 'a', tool: 'time/turnover.py', slice: 'sl1', params: { table: 'cafes' }, ran: '2026-11-11T02:00:00Z', outputs: [{ kind: 'table', name: 'lives', rows: 3 }] },
      { id: 'b', tool: 'time/survival.py', slice: null, params: { table: 'lives' }, ran: '2026-11-11T03:00:00Z', outputs: [{ kind: 'table', name: 'survival', rows: 9 }, { kind: 'note', text: 'n' }] });
    p.views.push({ id: 'v1', kind: 'chart', title: 'S', table: 'survival' });
    p.boards.push({ id: 'b1', title: 'Board' });
    assert('workspace: a project of sources, a slice, a quantity, steps, a view and a board is sound; an empty one too', W.checkProject(p).length === 0 && W.checkProject(W.emptyProject()).length === 0, W.checkProject(p).join('; '));
    assert('workspace: the tables of a project by name: a source\'s, and what its steps answered', [...W.tablesOf(p).keys()].join() === 'cafes,lives,survival' && W.tablesOf(p).get('lives').from === 'step');
    const wrong = W.checkProject({ ...p, sources: [...p.sources, { id: 's2', name: 'lives', ref: 'x' }], quantities: [{ name: 'weeks', low: 50, mid: 48, high: 46 }], slices: [{ id: 'sl1', name: 'Bad', time: { from: 2024, until: 2020 }, period: 'year' }], views: [{ id: 'v1', kind: 'dial', title: 'D' }] });
    assert('workspace: what is wrong with a project is named: two tables of one name, a quantity out of order, a slice that ends before it begins, a view of no kind', ['two tables are named', 'low ≤ mid ≤ high', 'ends before it begins', 'neither a layer nor a chart'].every((w) => wrong.some((x) => x.includes(w))), wrong.join('; '));
    assert('workspace: what is not a project is said to be none', W.checkProject({ schema: 'other/1' }).length === 1 && W.checkProject(null).length === 1);
    const tools = [{ ref: 'time/turnover.py', params: [{ name: 'table', role: 'table' }] }, { ref: 'time/survival.py', params: [{ name: 'table', role: 'table' }, { name: 'cohorts' }] }];
    assert('workspace: steps that ran after what they were given are not stale', W.staleSteps(p, tools).size === 0);
    p.sources[0].changed = '2026-11-11T02:30:00Z';
    assert('workspace: a source changed after a step ran makes it stale, and every step that stands on its answer', [...W.staleSteps(p, tools)].sort().join() === 'a,b');
    p.sources[0].changed = '2026-11-11T01:00:00Z';
    p.slices[0].changed = '2026-11-11T02:30:00Z';
    assert('workspace: so does its slice changed', [...W.staleSteps(p, tools)].sort().join() === 'a,b');
    p.slices[0].changed = null;
    p.meta.quantities = '2026-11-11T02:30:00Z';
    assert('workspace: a quantity changed makes stale the steps that ran before it, and those on their answers', [...W.staleSteps(p, tools)].sort().join() === 'a,b');
    const taken = new Set(['cafes', 'cafes_2']);
    assert('workspace: a table\'s name is letters, digits and _, and its own', W.tableName('Cafés & bars 2024.csv', new Set()) === 'Cafe_s_bars_2024_csv' && W.tableName('cafes', taken) === 'cafes_3' && W.tableName('2024') === '_2024' && W.NAME.test(W.tableName('  ')));
  }

  // ---- the plane ----
  {
    const frame = { lat: -37.8136, lon: 144.9631 }, pl = W.plane(frame), rnd = seeded(5);
    let worst = 0;
    for (let k = 0; k < 200; k++) { const lon = frame.lon + (rnd() - 0.5) * 0.2, lat = frame.lat + (rnd() - 0.5) * 0.2, [x, y] = pl.to(lon, lat), [a, b] = pl.from(x, y); worst = Math.max(worst, Math.abs(a - lon), Math.abs(b - lat)); }
    check('workspace: a project\'s plane and back', worst, 1e-12, 'degrees');
    const [x, y] = pl.to(frame.lon + 0.01, frame.lat + 0.01);
    assert('workspace: a hundredth of a degree is 879 m east and 1106 m north at Melbourne', Math.abs(x - 879.4) < 0.5 && Math.abs(y - 1105.7) < 0.5, `${x.toFixed(1)}, ${y.toFixed(1)}`);
  }

  // ---- views ----
  {
    const rnd = seeded(11), n = 500, kinds = ['café', 'bar', 'shop'], areas = ['north', 'south'];
    const t = table({ kind: Array.from({ length: n }, () => kinds[Math.floor(rnd() * 3)]), area: Array.from({ length: n }, () => areas[Math.floor(rnd() * 2)]), year: Array.from({ length: n }, () => 2020 + Math.floor(rnd() * 4)),
      sales: Array.from({ length: n }, () => (rnd() < 0.05 ? null : Math.round(rnd() * 1000))) }, { roles: { category: 'kind', amount: 'sales', time: 'year' } });
    const g = W.gather(t, { by: 'kind' }), brute = kinds.map((k) => t.data.kind.filter((x) => x === k).length);
    assert('workspace: rows gathered by a column are counted as by hand', g.keys.every((k, i) => g.series[0].values[i] === brute[kinds.indexOf(k)]) && g.series[0].values.reduce((a, b) => a + b, 0) === n);
    const s = W.gather(t, { by: 'year', split: 'area', of: 'sales', how: 'sum' });
    const by = (y, a) => t.data.sales.reduce((acc, v, i) => acc + (t.data.year[i] === y && t.data.area[i] === a && v !== null ? v : 0), 0);
    assert('workspace: sums by a column and a second: years in their order, a series for each area, each sum as by hand',
      s.keys.join() === '2020,2021,2022,2023' && s.series.length === 2 && s.series.every((r) => s.keys.every((y, i) => r.values[i] === by(y, r.name))));
    const m = W.gather(t, { by: 'kind', of: 'sales', how: 'mean' }), lo = W.gather(t, { by: 'kind', of: 'sales', how: 'least' }), hi = W.gather(t, { by: 'kind', of: 'sales', how: 'greatest' });
    assert('workspace: the mean lies between the least and the greatest', m.series[0].values.every((v, i) => lo.series[0].values[i] <= v && v <= hi.series[0].values[i]));
    let refused = 0;
    for (const bad of [{ by: 'nothing' }, { by: 'kind', how: 'sum' }]) { try { W.gather(t, bad); } catch { refused++; } }
    assert('workspace: a column that is not there, and a sum of nothing, are refused', refused === 2);
    const h = W.histogram(t.data.sales, 10);
    assert('workspace: a histogram counts every number once, in bins of equal width', h.counts.reduce((a, b) => a + b, 0) === t.data.sales.filter((v) => v !== null).length && h.edges.length === 11 && Math.abs((h.edges[1] - h.edges[0]) - (h.edges[10] - h.edges[9])) < 1e-9
      && W.histogram([3, 3, 3]).counts.join() === '3' && W.histogram([]).counts.length === 0);
    const bars = W.chartOf(t, { chart: 'bar', x: 'kind', y: 'sales', how: 'sum' }), share = W.chartOf(t, { chart: 'share', x: 'kind' }), lines = W.chartOf(t, { chart: 'line', x: 'year', y: 'sales', how: 'mean', split: 'area' });
    const stacked = W.chartOf(t, { chart: 'stacked', x: 'year', split: 'kind' }), points = W.chartOf(t, { chart: 'scatter', x: 'year', y: 'sales' }), hist = W.chartOf(t, { chart: 'histogram', y: 'sales', bins: 8 });
    assert('workspace: bars stand the greatest first; shares add up to a hundred', bars.kind === 'bars' && bars.series[0].values.every((v, i, a) => i === 0 || a[i - 1] >= v) && Math.abs(share.series[0].values.reduce((a, b) => a + b, 0) - 100) < 1e-9);
    assert('workspace: lines run along the years with a series for each area; stacked bars keep the years in order, a series for each kind',
      lines.kind === 'lines' && lines.x.join() === '2020,2021,2022,2023' && lines.series.length === 2 && stacked.stacked === true && stacked.labels.join() === '2020,2021,2022,2023' && stacked.series.length === 3);
    assert('workspace: points keep the rows that have both numbers; a histogram has the bins asked for', points.kind === 'scatter' && points.x.length === t.data.sales.filter((v) => v !== null).length && hist.labels.length === 8);
    assert('workspace: a chart that cannot be drawn says what it needs', !!W.chartOf(t, { chart: 'histogram', y: 'kind' }).note && !!W.chartOf(t, { chart: 'bar', x: 'kind', y: 'area', how: 'sum' }).note && !!W.chartOf(table({ a: ['x'] }), { chart: 'scatter' }).note);
    const many = table({ k: Array.from({ length: 60 }, (_, i) => `k${i}`), v: Array.from({ length: 60 }, (_, i) => i) });
    assert('workspace: of many bars the first are drawn, and the title says how many', W.chartOf(many, { chart: 'bar', x: 'k', y: 'v', how: 'sum', most: 10 }).labels.length === 10 && /first 10 of 60/.test(W.chartOf(many, { chart: 'bar', x: 'k', y: 'v', how: 'sum', most: 10 }).title));
    const some = W.chartOf(t, { chart: 'bar', x: 'kind' }, [0, 1, 2, 3, 4]);
    assert('workspace: a chart of some rows counts those rows only', some.series[0].values.reduce((a, b) => a + b, 0) === 5);

    const lin = W.scaleOf([0, 5, 10]), log = W.scaleOf([1, 10, 1000]), root = W.scaleOf([0, 25, 100], { root: true }), text = W.scaleOf(['a', 'b', 'a', null]);
    assert('workspace: a column is scaled from its least to its greatest: evenly, by logarithms over more than two decades, by square roots when asked',
      lin.u(5) === 0.5 && !lin.log && log.log && Math.abs(log.u(10) - 1 / 3) < 1e-12 && root.u(25) === 0.5 && lin.u(null) === null && W.scaleOf([4, 4]).u(4) === 0.5);
    assert('workspace: kinds take an index each, in the order they first appear', text.kind === 'text' && text.kinds.join() === 'a,b' && text.u('b') === 1 && text.u('z') === null);
    const placed = table({ lat: [1], lon: [2], sales: [3] }, { roles: { lat: 'lat', lon: 'lon', amount: 'sales' } }), cells = table({ block: ['a'], n: [1] }, { roles: { cell: 'block' }, places: { kind: 'cells', column: 'block', rings: { a: [[0, 0], [1, 0], [1, 1]] } } });
    assert('workspace: a table is drawn as points where it has a latitude and a longitude, as areas where it has cells with outlines, as lines where it has them, else not on the map',
      W.layerKind(placed) === 'points' && W.layerKind(cells) === 'areas' && W.layerKind({ ...table({ a: [] }), lines: [{ line: [[0, 0], [1, 1]] }] }) === 'lines' && W.layerKind(t) === null);
    const dv = W.defaultView(placed), dc = W.defaultView(t), dn = W.defaultView(table({ who: ['a', 'b'] }));
    assert('workspace: a table suggests its view: a layer coloured and sized by its amount, a line along its time, else bars of a count',
      dv.kind === 'layer' && dv.colour === 'sales' && dv.size === 'sales' && dc.kind === 'chart' && dc.chart === 'line' && dc.x === 'year' && dc.y === 'sales' && dn.chart === 'bar' && dn.how === 'count');
    assert('workspace: the predefined charts and layers are described', W.CHARTS.length === 6 && W.CHARTS.every((c) => c.label && c.about) && W.LAYERS.map((l) => l.id).join() === 'points,areas,lines' && W.GATHER.length === 5);
  }
}
