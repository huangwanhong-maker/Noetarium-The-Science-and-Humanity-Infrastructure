// Checks of the board (visualization/js/board): its model and operations (every operation undone
// exactly, operations on different items commuting, refusals that change nothing), its geometry, its
// arrangements, plans and chronicles (the calendar against JavaScript's own, float and the critical
// chain against a case worked by hand), quantities (interval arithmetic enclosing every value its
// inputs allow, by sampling), templates, tables (a sheet read into columns, gathered by name, given to
// modules), the pen (a stroke kept in fewer points within a tolerance, its outline by its pressure),
// layers (an item's standing by its own word or its layer's; a layer removed leaving its items on the ground),
// and its style sheet (the variables a picture carries, and
// the contrast of ink on every tone by day and by night).
// Called by run-tests.mjs with the suite's check and assert.

import { readFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { seeded } from '../../development/tools/checks.mjs';
import { board as B } from '../js/index.js';

const HERE = dirname(fileURLToPath(import.meta.url));
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);

// A board of notes in regions with links among them, made by operations from a seed.
function sample(seed, n = 14) {
  const rnd = seeded(seed), id = () => B.makeId(rnd);
  let board = B.emptyBoard({ id: 'sample', title: 'A sample' });
  const ops = [];
  const regions = [id(), id()];
  regions.forEach((rid, k) => ops.push({ t: 'add', id: rid, item: B.newItem('region', { title: `Region ${k}` }), place: { x: k * 700, y: 0, w: 600, h: 500 } }));
  const notes = Array.from({ length: n }, id);
  notes.forEach((nid, k) => ops.push({ t: 'add', id: nid, item: B.newItem('note', { type: B.NOTE_TYPES[k % 7].id, text: `note ${k}`, in: regions[k % 2] }),
    place: { x: (k % 2) * 700 + 20 + Math.floor(rnd() * 360), y: 50 + Math.floor(rnd() * 300), w: 200, h: 132 } }));
  for (let k = 0; k + 1 < n; k += 2) ops.push({ t: 'add', id: id(), item: B.newItem('link', { from: notes[k], to: notes[k + 1], rel: Object.keys(B.RELATIONS)[k % 6] }) });
  board = B.apply(board, ops).board;
  return { board, regions, notes, rnd, id };
}

export function boardChecks({ assert, check }) {
  // ---- the model and its operations ---------------------------------------------------------------
  {
    const { board, regions, notes, rnd, id } = sample(11);
    assert('board: an empty board and a board made by operations are sound', !B.checkBoard(B.emptyBoard()).length && !B.checkBoard(board).length, B.checkBoard(board).join('; '));

    // A random run of operations of every kind, undone by its inverses.
    const ops = [];
    for (let k = 0; k < 60; k++) {
      const pick = notes[Math.floor(rnd() * notes.length)];
      const kind = Math.floor(rnd() * 5);
      if (kind === 0) ops.push({ t: 'set', id: pick, patch: { text: `changed ${k}`, source: rnd() < 0.5 ? 'a source' : null } });
      else if (kind === 1) ops.push({ t: 'place', id: pick, place: { x: Math.floor(rnd() * 900), h: 132 + Math.floor(rnd() * 80) } });
      else if (kind === 2) ops.push({ t: 'order', id: pick, to: Math.floor(rnd() * board.order.length) });
      else if (kind === 3) ops.push({ t: 'add', id: id(), item: B.newItem('text', { text: 'words' }), place: { x: 0, y: 0, w: 100, h: 40 }, at: Math.floor(rnd() * 5) });
      else ops.push({ t: 'title', title: `Title ${k}` });
    }
    const forward = B.apply(board, ops);
    assert('board: a run of operations leaves a sound board, and its inverses restore the first exactly',
      !B.checkBoard(forward.board).length && same(B.apply(forward.board, forward.inverse).board, board));
    assert('board: applying operations does not change the board given', !B.checkBoard(board).length && board.title === 'A sample');

    // Operations on different items commute.
    let commute = true;
    for (let k = 0; k < 40 && commute; k++) {
      const a = notes[Math.floor(rnd() * notes.length)], b = notes.find((n) => n !== a);
      const p = { t: 'set', id: a, patch: { text: `a${k}` } }, q = rnd() < 0.5 ? { t: 'place', id: b, place: { x: k } } : { t: 'set', id: b, patch: { type: 'risk' } };
      commute = same(B.apply(board, [p, q]).board, B.apply(board, [q, p]).board);
    }
    assert('board: operations on different items commute', commute);

    // Refusals change nothing.
    let refused = '';
    try { B.apply(board, [{ t: 'set', id: notes[0], patch: { text: 'x' } }, { t: 'remove', id: notes[0] }]); } catch (error) { refused = error.message; }
    assert('board: an item a link ends on is not removed, and the refusal says why', /link/.test(refused) && board.items[notes[0]].text === 'note 0', refused);
    let unknown = '';
    try { B.apply(board, [{ t: 'fold', id: notes[0] }]); } catch (error) { unknown = error.message; }
    assert('board: an unknown operation is refused', /no operation/.test(unknown));

    // Removal composed: links first, and what a removed region held stays on the board.
    const gone = B.apply(board, B.removeOps(board, [regions[0], notes[1]])).board;
    assert('board: removing a region and a note removes the links on the note and leaves the region\'s other items in no region',
      !B.checkBoard(gone).length && !gone.items[regions[0]] && !gone.items[notes[1]] && gone.items[notes[0]] && gone.items[notes[0]].in === undefined
      && !gone.order.some((i) => gone.items[i].from === notes[1] || gone.items[i].to === notes[1]), B.checkBoard(gone).join('; '));

    // A history.
    const history = B.createHistory(board);
    history.commit([{ t: 'set', id: notes[2], patch: { text: 'first' } }], 'write');
    history.amend([{ t: 'place', id: notes[2], place: { h: 200 } }]);
    history.commit([{ t: 'title', title: 'Renamed' }], 'rename');
    const final = history.board;
    history.undo(); history.undo();
    const back = same(history.board, board);
    history.redo(); history.redo();
    assert('board: a history undone is the first board, done again the last; what is amended is undone with its step', back && same(history.board, final) && !history.canRedo && history.length === 2);

    // Copies.
    const dup = B.duplicateOps(board, [regions[1]], { id });
    const copied = B.apply(board, dup.ops).board;
    const inside = B.members(board, regions[1]).length, newRegion = dup.ids.get(regions[1]);
    const links = (b, set) => b.order.filter((i) => B.isLink(b.items[i]) && set.has(b.items[i].from) && set.has(b.items[i].to)).length;
    assert('board: a region is copied with what it holds, and a link exactly when both its ends are',
      !B.checkBoard(copied).length && B.members(copied, newRegion).length === inside
      && links(copied, new Set(B.members(copied, newRegion))) === links(board, new Set(B.members(board, regions[1]))));

    // Membership follows the place.
    const moved = B.apply(board, [{ t: 'place', id: notes[0], place: { x: 2000, y: 2000 } }, { t: 'place', id: notes[1], place: { x: 40, y: 60 } }]).board;
    const after = B.apply(moved, B.membershipOps(moved, [notes[0], notes[1]])).board;
    assert('board: an item belongs to the region its centre lies in, or to none', after.items[notes[0]].in === undefined && after.items[notes[1]].in === regions[0]);
    const nested = B.apply(board, [{ t: 'add', id: 'inner', item: B.newItem('region', { title: 'inner' }), place: { x: 10, y: 40, w: 300, h: 300 } }]).board;
    assert('board: of nested regions the smallest holds an item', B.regionAt(nested, { x: 60, y: 90, w: 100, h: 100 }) === 'inner' && B.regionAt(nested, { x: 60, y: 90, w: 100, h: 100 }, 'inner') === regions[0]);

    const broken = { ...board, items: { ...board.items, bad: B.newItem('link', { from: notes[0], to: 'nowhere' }) }, order: [...board.order, 'bad'] };
    const { [notes[3]]: _lost, ...fewer } = board.place;
    assert('board: a link to nothing and an item without a rectangle are found', B.checkBoard(broken).some((p) => /no item at its end/.test(p)) && B.checkBoard({ ...board, place: fewer }).some((p) => /no rectangle/.test(p)));
  }

  // ---- geometry -------------------------------------------------------------------------------------
  {
    const a = { x: 0, y: 0, w: 200, h: 100 }, right = { x: 500, y: 20, w: 200, h: 100 }, below = { x: 40, y: 400, w: 200, h: 100 };
    const p = B.linkPath(a, right, { gap: 0 }), q = B.linkPath(a, below, { gap: 0 });
    assert('board: a link leaves the side that faces the other item and arrives on the side that faces it',
      same(p.sides, ['e', 'w']) && p.start.x === 200 && p.start.y === 50 && p.end.x === 500 && p.end.y === 70 && same(q.sides, ['s', 'n']) && q.end.y === 400);
    assert('board: its arrowhead has its tip at the end', p.arrow.startsWith('M500 70'));
    const s1 = B.snap({ x: 503, y: 300, w: 100, h: 60 }, [right], 6), s2 = B.snap({ x: 520, y: 300, w: 100, h: 60 }, [right], 6);
    assert('board: a moved item snaps to an edge within the threshold, and not beyond it', s1.dx === -3 && s1.guides.some((g) => g.x === 500) && s2.dx === 0 && s2.guides.length === 0);
    const view = { x: 37, y: -12, k: 0.8 }, at = { x: 300, y: 200 };
    const z = B.zoomAbout(view, 1.7, at), w0 = B.toBoard(view, at), w1 = B.toBoard(z, at);
    check('board: zooming about a point leaves the point where it is', Math.hypot(w0.x - w1.x, w0.y - w1.y), 1e-9, 'units');
    const fit = B.fitView({ x: -300, y: 100, w: 2400, h: 900 }, { w: 1200, h: 800 });
    const c0 = B.toScreen(fit, { x: -300, y: 100 }), c1 = B.toScreen(fit, { x: 2100, y: 1000 });
    const clear = B.fitView({ x: -300, y: 100, w: 2400, h: 900 }, { w: 1200, h: 800 }, { inset: { left: 150, right: 290 } });
    const e0 = B.toScreen(clear, { x: -300, y: 100 }), e1 = B.toScreen(clear, { x: 2100, y: 1000 });
    assert('board: a view fitted to a rectangle shows it whole, clear of what stands over the edges', c0.x >= 0 && c0.y >= 0 && c1.x <= 1200 && c1.y <= 800 && e0.x >= 150 && e1.x <= 910 && e0.y >= 0 && e1.y <= 800);
  }

  // ---- arrangements ---------------------------------------------------------------------------------
  {
    const { board, regions, notes } = sample(23, 16);
    const overlap = (b, ids) => ids.some((i, k) => ids.slice(k + 1).some((j) => B.intersects(b.place[i], b.place[j])));
    const tidied = B.apply(board, B.placeOps(board, B.tidy(board, notes))).board;
    assert('board: a tidy grid has no item on another, and tidying twice changes nothing more', !overlap(tidied, notes) && !B.placeOps(tidied, B.tidy(tidied, notes)).length);
    const inRegions = B.apply(board, B.placeOps(board, B.tidyRegions(board))).board;
    const within = (b, id, rid) => { const p = b.place[id], r = b.place[rid]; return p.x >= r.x && p.y >= r.y && p.x + p.w <= r.x + r.w && p.y + p.h <= r.y + r.h; };
    assert('board: regions tidied hold their items within them, growing where they must', !overlap(inRegions, notes) && notes.every((n) => within(inRegions, n, inRegions.items[n].in))
      && regions.some((r) => inRegions.place[r].h > board.place[r].h));

    // A tree: one root, three children, one of them with two of its own; and a link that closes a cycle.
    let t = B.emptyBoard();
    const ids = ['root', 'a', 'b', 'c', 'b1', 'b2'];
    t = B.apply(t, [...ids.map((id, k) => ({ t: 'add', id, item: B.newItem('note', { text: id }), place: { x: 40 * k, y: 90 * k, w: 200, h: 100 } })),
      ...[['root', 'a'], ['root', 'b'], ['root', 'c'], ['b', 'b1'], ['b', 'b2'], ['b2', 'root']].map(([from, to], k) => ({ t: 'add', id: `l${k}`, item: B.newItem('link', { from, to }) }))]).board;
    const laid = B.apply(t, B.placeOps(t, B.tree(t))).board, P = laid.place;
    const mid = (id) => P[id].y + P[id].h / 2;
    assert('board: a tree grows to the right, each parent beside the middle of its children, its root where it was, a cycle cut',
      same([P.root.x, P.root.y], [0, 0]) && P.a.x === P.b.x && P.b.x === P.c.x && P.a.x > P.root.x + P.root.w && P.b1.x > P.b.x + P.b.w
      && Math.abs(mid('b') - (mid('b1') + mid('b2')) / 2) <= 1 && Math.abs(mid('root') - (mid('a') + mid('c')) / 2) <= 1 && !overlap(laid, ids));
    const partOf = B.apply(B.emptyBoard(), [{ t: 'add', id: 'whole', item: B.newItem('note'), place: { x: 0, y: 0, w: 200, h: 100 } }, { t: 'add', id: 'part', item: B.newItem('note'), place: { x: 0, y: 300, w: 200, h: 100 } },
      { t: 'add', id: 'l', item: B.newItem('link', { from: 'part', to: 'whole', rel: 'part-of' }) }]).board;
    assert('board: what is part of another is its child in the tree', B.tree(partOf).part.x > 200);

    const dated = B.apply(board, notes.slice(0, 4).map((id, k) => ({ t: 'set', id, patch: { when: ['2026-03', '1999', '2026-03-02', '480 BC'][k] } }))).board;
    const timed = B.byTime(dated, notes, (w) => B.parseDate(w)?.day);
    const xs = [3, 1, 0, 2].map((k) => timed[notes[k]].x);
    assert('board: notes that say when are set in the order of time, the others left', xs.every((x, k) => k === 0 || x > xs[k - 1]) && Object.keys(timed).length === 4);
    const spread = B.apply(board, B.placeOps(board, B.align(board, notes.slice(0, 5), 'across'))).board;
    const sorted = notes.slice(0, 5).map((n) => spread.place[n]).sort((p, q) => p.x - q.x);
    const gaps = sorted.slice(1).map((p, k) => p.x - (sorted[k].x + sorted[k].w));
    check('board: items spread across have equal gaps', Math.max(...gaps) - Math.min(...gaps), 1e-9, 'units');
  }

  // ---- plans and chronicles -------------------------------------------------------------------------
  {
    let worst = 0, round = true;
    for (let y = 1583; y <= 2400; y += 7) for (const [m, d] of [[1, 1], [2, 28], [3, 1], [7, 14], [12, 31]]) {
      const day = B.dayNumber(y, m, d);
      worst = Math.max(worst, Math.abs(day - Date.UTC(y, m - 1, d) / 86400000));
      const c = B.civil(day);
      round = round && c.y === y && c.m === m && c.d === d;
    }
    for (let day = -900000; day <= 900000; day += 9973) { const c = B.civil(day); round = round && B.dayNumber(c.y, c.m, c.d) === day; }
    check('board: the day number of a date agrees with JavaScript\'s own calendar, 1583 to 2400', worst, 0, 'days');
    assert('board: a day number and its date return to each other, before year 1 too', round && B.dayNumber(1970, 1, 1) === 0 && B.dayNumber(2000, 3, 1) === 11017);
    const bc = B.parseDate('480 BC'), month = B.parseDate('2026-02');
    assert('board: dates are read to the day, the month or the year; 480 BC is the year −479; 30 February is no date',
      B.parseDate('2026-11-16').day === B.dayNumber(2026, 11, 16) && month.next - month.day === 28 && month.precision === 'month' && B.civil(bc.day).y === -479
      && B.parseDate('-0479').day === bc.day && B.parseDate('2026-02-30') === null && B.parseDate('soon') === null && B.formatDay(bc.day, 'year') === '480 BC');

    const text = ['# Research', 'Desk : 2026-11-16, 5d', 'Calls : 2026-11-16, 2d', 'Synthesis : after Desk & Calls, 1w, 50%', 'Report : after Synthesis, milestone',
      'Lost : after Nothing, 2d', 'Loop A : after Loop B, 1d', 'Loop B : after Loop A, 1d', 'Floating : 3d', 'nonsense', 'Odd : 2026-11-16, soon'].join('\n');
    const parsed = B.parseSchedule(text), r = B.resolveSchedule(parsed.tasks);
    const by = Object.fromEntries(r.tasks.map((t) => [t.name, t]));
    const d0 = B.dayNumber(2026, 11, 16);
    assert('board: an entry after others starts when the last of them ends', by.Synthesis.start === d0 + 5 && by.Synthesis.end === d0 + 12 && by.Report.start === d0 + 12 && by.Report.milestone && by.Synthesis.progress === 0.5 && by.Desk.group === 'Research');
    assert('board: the float of an entry is what it could slip; the chain without float is critical', by.Calls.float === 3 && !by.Calls.critical && by.Desk.critical && by.Synthesis.critical && by.Report.critical && by.Desk.float === 0);
    const lines = [...parsed.errors, ...r.errors].map((e) => e.line).sort((a, b) => a - b);
    assert('board: what cannot be read or placed is refused line by line, each with its reason (no colon, an unknown word, an unknown entry, a cycle, no start)',
      same(lines, [6, 7, 8, 9, 10, 11]) && r.errors.some((e) => /not an entry/.test(e.message)) && r.errors.some((e) => /comes after itself/.test(e.message)) && r.errors.some((e) => /no start/.test(e.message)), JSON.stringify(lines));
    const early = B.resolveSchedule(B.parseSchedule('A : 2026-01-10, 10d\nB : 2026-01-12, after A, 2d').tasks);
    assert('board: an entry dated before what it comes after has ended is kept and warned of', early.tasks.length === 2 && early.warnings.length === 1);

    const plan = B.layoutPlan(r, { width: 800, today: d0 + 3 });
    const inPlot = plan.rows.every((row) => row.x >= plan.plot.x - 0.01 && row.x + row.w <= plan.plot.x + plan.plot.w + 0.01);
    assert('board: a plan\'s bars lie within its plot in the order written, later starts further right, a join for every dependence',
      inPlot && plan.rows.length === 4 && plan.rows.every((row, k) => k === 0 || row.y > plan.rows[k - 1].y) && plan.rows[2].x > plan.rows[0].x && plan.joins.length === 3
      && plan.joins.filter((j) => j.critical).length === 2 && plan.today > plan.plot.x && plan.groups.length === 1 && plan.ticks.length >= 2 && plan.ticks.every((t) => t.x >= plan.plot.x - 0.01));

    const events = Array.from({ length: 24 }, (_, k) => `${1500 + ((k * 37) % 200)} : Event number ${k}`).join('\n') + '\n1632..1660 : A long period\n1789-07-14 : A day';
    const chronicle = B.layoutChronicle(B.resolveSchedule(B.parseSchedule(events).tasks), { width: 900 });
    const spans = chronicle.events.map((e) => ({ lane: e.lane, a: e.anchor === 'start' ? e.x - 6 : e.labelX - e.task.name.length * 6.5 - 14, b: e.anchor === 'start' ? e.labelX + e.task.name.length * 6.5 : e.x + e.w + 6 }));
    const clash = spans.some((p, k) => spans.slice(k + 1).some((q) => q.lane === p.lane && p.a < q.b && q.a < p.b));
    assert('board: a chronicle places every event, in lanes where no name runs into another', chronicle.events.length === 26 && !clash && chronicle.height > 60);
    for (const [a, b, most] of [[0, 9, 12], [0, 80, 10], [0, 700, 10], [0, 20000, 8], [-900000, 300000, 9]]) {
      const t = B.ticks(a, b, most);
      assert(`board: ${b - a} days are marked with at most ${most + 1} ticks, all within them`, t.ticks.length >= 1 && t.ticks.length <= most + 1 && t.ticks.every((k) => k.day >= a && k.day <= b), `${t.ticks.length} of ${t.step.unit}`);
    }
  }

  // ---- quantities -----------------------------------------------------------------------------------
  {
    const d = B.parseDefinition('population = 5.2M [4.8M, 5.6M] people'), range = B.parseDefinition('share = 3%..5%'), list = B.parseDefinition('spend = 1,200 [30, 5,500] AUD a year');
    assert('board: a definition is read with its range and its unit; a..b takes the middle; a comma separates thousands only before three digits',
      d.expression.mid === 5.2e6 && d.expression.lo === 4.8e6 && d.unit === 'people' && d.literal && Math.abs(range.expression.mid - 0.04) < 1e-12 && range.percent
      && list.expression.mid === 1200 && list.expression.lo === 30 && list.expression.hi === 5500 && list.unit === 'AUD a year');
    assert('board: prose is not a definition, and a definition that cannot be read says why',
      B.parseDefinition('We think the share is small') === null && B.parseDefinition('x == 3') === null && /not within its range/.test(B.parseDefinition('x = 9 [1, 2]').error) && /ends where/.test(B.parseDefinition('x = 3 *').error));

    let board = B.emptyBoard();
    const add = (id, kind, props) => ({ t: 'add', id, item: B.newItem(kind, props), place: { x: 0, y: 0, w: 200, h: 120 } });
    board = B.apply(board, [
      add('n1', 'note', { type: 'fact', text: 'People in the region\npopulation = 5.2M [4.8M, 5.6M] people' }),
      add('n2', 'note', { type: 'assumption', text: 'share = 3%..5%' }),
      add('n3', 'note', { type: 'assumption', text: 'spend = 40 [30, 55] AUD' }),
      add('f1', 'figure', { formula: 'TAM = population * share * spend AUD\nSAM = TAM * min(reach, 1)\nreach = 60%' }),
      add('f2', 'figure', { formula: 'bad = missing + 1\ntwice = 1\ntwice = 2\nloop = loop + 1\nratio = 1 / (share - 4%)' }),
    ]).board;
    const v = B.evaluate(B.collect(board)), tam = v.get('TAM'), sam = v.get('SAM');
    check('board: the middle of a product is the product of the middles', tam.mid - 5.2e6 * 0.04 * 40, 1e-6, 'AUD');
    check('board: its low bound is the product of the low bounds (of positive values)', tam.lo - 4.8e6 * 0.03 * 30, 1e-6, 'AUD');
    check('board: its high bound is the product of the high bounds', tam.hi - 5.6e6 * 0.05 * 55, 1e-6, 'AUD');
    assert('board: a figure knows the values written down that it rests on, through other figures too, and which item makes it',
      same(tam.rests.slice().sort(), ['population', 'share', 'spend']) && same(sam.rests.slice().sort(), ['population', 'reach', 'share', 'spend']) && tam.item === 'f1' && v.get('population').item === 'n1' && tam.unit === 'AUD');
    assert('board: a name not defined, a name defined twice, a definition that needs itself and a division by a range that holds zero are each refused with their reason',
      /not defined/.test(v.get('bad').error) && /more than once/.test(v.get('twice').error) && /needs itself/.test(v.get('loop').error) && /holds zero/.test(v.get('ratio').error));

    // Enclosure: whatever values the inputs take within their ranges, the result lies within its range.
    const rnd = seeded(5);
    let escaped = 0, widest = 0;
    for (let k = 0; k < 200; k++) {
      const lo = [1, 2, 3].map(() => rnd() * 8 - 2), hi = lo.map((l) => l + rnd() * 3);
      const defs = ['a', 'b', 'c'].map((n, i) => B.parseDefinition(`${n} = ${((lo[i] + hi[i]) / 2).toFixed(6)} [${lo[i].toFixed(6)}, ${hi[i].toFixed(6)}]`))
        .concat(B.parseDefinition('y = a * b - (c + a) / (max(b, 0) + 1) + max(a, c) - min(b, 2)'));
      const y = B.evaluate(defs).get('y');
      if (y.error) { escaped += 40; continue; }
      for (let s = 0; s < 40; s++) {
        const [a, b, c] = lo.map((l, i) => Number(l.toFixed(6)) + rnd() * (Number(hi[i].toFixed(6)) - Number(l.toFixed(6))));
        const exact = a * b - (c + a) / (Math.max(b, 0) + 1) + Math.max(a, c) - Math.min(b, 2);
        if (exact < y.lo - 1e-9 || exact > y.hi + 1e-9) escaped++;
      }
      widest = Math.max(widest, y.hi - y.lo);
    }
    check('board: of 8,000 values sampled within the inputs\' ranges, none falls outside the result\'s range', escaped, 0, 'values');
    assert('board: numbers are written for reading', B.formatNumber(8320000) === '8.32 M' && B.formatNumber(0.04, { percent: true }) === '4%' && B.formatNumber(4500) === '4,500' && B.formatNumber(120000) === '120 k' && B.formatNumber(345600) === '346 k'
      && B.formatNumber(-12.5) === '−12.5' && B.formatNumber(15.4e9) === '15.4 B' && B.formatValue(v.get('population')) === '5.2 M people (4.8 M – 5.6 M)' && B.formatValue(v.get('reach')) === '60%',
    `${B.formatNumber(8320000)} ${B.formatNumber(4500)} ${B.formatNumber(-12.5)} ${B.formatValue(v.get('population'))}`);
  }

  // ---- templates ------------------------------------------------------------------------------------
  {
    const rnd = seeded(3), id = () => B.makeId(rnd);
    const wrong = [];
    for (const template of B.GENERAL.templates) {
      const r = B.instantiate(template, { at: { x: 100, y: -40 }, id });
      const board = B.apply(B.emptyBoard(), r.ops).board, box = B.bounds(board);
      const problems = B.checkBoard(board);
      if (problems.length) wrong.push(`${template.id}: ${problems[0]}`);
      if (box.x !== 100 || box.y !== -40) wrong.push(`${template.id}: its corner is at ${box.x}, ${box.y}`);
      if (B.membershipOps(board, board.order).length) wrong.push(`${template.id}: an item's region is not the one it lies in`);
      const firstNote = board.order.findIndex((i) => board.items[i].kind !== 'region'), lastRegion = board.order.map((i) => board.items[i].kind).lastIndexOf('region');
      if (firstNote >= 0 && lastRegion > firstNote) wrong.push(`${template.id}: a region lies in front of an item`);
      if (B.thumbnail(template).rects.length !== template.items.length) wrong.push(`${template.id}: its small drawing leaves items out`);
      for (const i of board.order) {
        if (board.items[i].kind !== 'schedule') continue;
        const parsed = B.parseSchedule(board.items[i].source), res = B.resolveSchedule(parsed.tasks);
        if (parsed.errors.length || res.errors.length || !res.tasks.length) wrong.push(`${template.id}: its schedule is not read`);
      }
    }
    assert(`board: the ${B.GENERAL.templates.length} general templates each lay a sound board at the corner given, regions behind, memberships as they lie, schedules read`, !wrong.length, wrong.join('; '));
    let refused = '';
    try { B.instantiate({ id: 'bad', items: [B.note('idea', '', 0, 0, { in: 'nowhere' })] }); } catch (error) { refused = error.message; }
    assert('board: a template that puts an item in a region it does not have is refused', /does not have/.test(refused));
  }

  // ---- tables ---------------------------------------------------------------------------------------
  {
    const sheet = 'city\tshops\trent\tshare\tnote\nA\t12\t3,400\t40%\t"said ""no"""\nB\t\t5,100\t7.5%\t"two\nlines"\nC\t7\t2,250\t\tplain';
    const t = B.parseDelimited(sheet, { name: 'towns' });
    assert('board: a sheet is read into columns: numbers with their thousands and their shares, an empty field null, quoted fields whole',
      t.rows === 3 && t.columns.map((c) => c.kind).join() === 'text,number,number,number,text' && same(t.data.shops, [12, null, 7]) && same(t.data.rent, [3400, 5100, 2250])
      && Math.abs(t.data.share[1] - 0.075) < 1e-12 && t.data.share[2] === null && t.data.note[0] === 'said "no"' && t.data.note[1] === 'two\nlines' && !B.checkTable(t).length, JSON.stringify(t.data));
    const semi = B.parseDelimited(String.fromCharCode(0xfeff) + 'year;sales\n2020;10,5\n2021;14', { name: 'x' }), comma = B.parseDelimited('a,b\n1,"x, y"\n2,z', { name: 'y' });
    assert('board: the delimiter is the one that divides the lines alike (a semicolon, a comma, a tab), and a byte order mark is no part of a name',
      semi.columns[0].name === 'year' && same(semi.data.year, [2020, 2021]) && semi.columns[1].kind === 'text' && same(comma.data.b, ['x, y', 'z']) && B.splitDelimited(sheet).delimiter === '\t');
    let refused = '';
    try { B.parseDelimited('a,b\n1,2,3', { name: 'z' }); } catch (error) { refused = error.message; }
    assert('board: a row with more fields than names is refused, a short one filled', /row 2 has 3 fields/.test(refused) && same(B.parseDelimited('a,b\n1\n2,3', { name: 'z' }).data.b, [null, 3]));
    assert('board: rows and columns are told from prose', B.looksTabular(sheet) && B.looksTabular('a,b\n1,2') && !B.looksTabular('We met them on Tuesday, and again on Friday.\nThey said no.') && !B.looksTabular('one line only'));
    assert('board: a table is named in letters, digits and _, apart from the others', B.tableName('Sales 2024 (final).csv') === 'sales_2024_final' && B.tableName('2024') === 't_2024' && B.tableName('towns', new Set(['towns', 'towns_2'])) === 'towns_3' && B.tableName('') === 'table');
    const back = B.parseDelimited(B.toDelimited(t, '\t'), { name: 'towns' });
    assert('board: a table written out for a sheet and read again holds the same', same(back.data.shops, t.data.shops) && same(back.data.note, t.data.note) && same(back.data.rent, t.data.rent));
    assert('board: a table whose columns differ in length, or whose numbers are not numbers, is found', B.checkTable({ name: 'x', columns: [{ name: 'a', kind: 'number' }, { name: 'b', kind: 'text' }], data: { a: [1, 'two'], b: ['x'] } }).length === 2
      && B.checkTable({ name: '2x', columns: [], data: {} }).length === 1);

    // The tables of a board by name: those laid on it and those its modules answered.
    const add = (id, item) => ({ t: 'add', id, item, place: { x: 0, y: 0, w: 300, h: 200 } });
    const answered = { name: 'potential', title: 'worth', columns: [{ name: 'iso3', kind: 'text' }, { name: 'value', kind: 'number' }], data: { iso3: ['AAA'], value: [1] }, rows: 1 };
    let board = B.apply(B.emptyBoard(), [add('t1', B.newItem('table', t)), add('m1', B.newItem('card', { card: 'module', props: { module: 'x/y.py' }, out: { tables: [answered] } })),
      add('n1', B.newItem('note', { type: 'assumption', text: 'What we believe\nadoption = 2% [1%, 4%]', source: 'a guess' })), add('s1', B.newItem('ink', { points: [[1, 1, 0.5], [9, 9, 0.5]], size: [10, 10] }))]).board;
    const tables = B.collectTables(board);
    assert('board: a board with a table, a stroke of the pen and a card is sound', !B.checkBoard(board).length && B.KINDS.includes('ink') && B.KINDS.includes('table'), B.checkBoard(board).join('; '));
    assert('board: its tables are gathered by name, each with the item that gives it', tables.get('towns').item === 't1' && tables.get('potential').item === 'm1' && tables.get('potential').rows === 1 && tables.size === 2);
    const given = B.boardForModules(board, tables, B.evaluate(B.collect(board)));
    assert('board: a module is given the tables, the quantities with their ranges, and the notes\' words without the lines that only define',
      same(Object.keys(given.tables), ['towns', 'potential']) && same(given.quantities.adoption, { lo: 0.01, mid: 0.02, hi: 0.04, unit: '' }) && given.notes[0].text === 'What we believe' && given.notes[0].source === 'a guess');
    board = B.apply(board, [add('t2', B.newItem('table', { ...t, name: 'potential' }))]).board;
    const twice = B.collectTables(board);
    assert('board: a name two items give is refused for both, and no module is given it', /more than one table/.test(twice.get('potential').error) && !('potential' in B.boardForModules(board, twice, new Map()).tables));
  }

  // ---- the pen --------------------------------------------------------------------------------------
  {
    const rnd = seeded(17);
    const line = Array.from({ length: 200 }, (_, k) => [k * 2, 30 * Math.sin(k / 25) + (rnd() - 0.5) * 0.3, 0.5]);
    const few = B.simplify(line, 0.5);
    // How far each point left out lies from the line that is kept.
    let worst = 0;
    for (const q of line) {
      let near = Infinity;
      for (let k = 1; k < few.length; k++) {
        const [ax, ay] = few[k - 1], [bx, by] = few[k], dx = bx - ax, dy = by - ay;
        const u = Math.max(0, Math.min(1, ((q[0] - ax) * dx + (q[1] - ay) * dy) / (dx * dx + dy * dy || 1)));
        near = Math.min(near, Math.hypot(q[0] - ax - u * dx, q[1] - ay - u * dy));
      }
      worst = Math.max(worst, near);
    }
    assert('board: a stroke is kept in fewer points, its ends among them', few.length < line.length / 3 && few[0] === line[0] && few[few.length - 1] === line[line.length - 1] && few.every((q) => q.length === 3), `${few.length} of ${line.length}`);
    check('board: no point left out lies further from the stroke kept than the tolerance (excess)', Math.max(0, worst - 0.5), 1e-9, 'units');
    const outline = B.strokeOutline([[0, 0, 0.5], [50, 0, 0.5], [100, 0, 0.5]], 6);
    // The points a path passes through or is drawn toward: the pairs of M, L and Q, and where an arc ends.
    const pointsOf = (d) => [...d.matchAll(/([MLQA])([^MLQAZ]*)/g)].flatMap(([, c, rest]) => {
      const n = rest.trim().split(/[ ,]+/).map(Number);
      return c === 'A' ? [[n[5], n[6]]] : Array.from({ length: n.length / 2 }, (_, k) => [n[2 * k], n[2 * k + 1]]);
    });
    const ys = pointsOf(outline).map((q) => q[1]);
    assert('board: the outline of a stroke is closed, and as wide as the pen at a pressure of one half', outline.startsWith('M') && outline.endsWith('Z') && Math.max(...ys) === 3 && Math.min(...ys) === -3, outline);
    const light = B.strokeOutline([[0, 0, 0.1], [100, 0, 0.1]], 6), heavy = B.strokeOutline([[0, 0, 0.9], [100, 0, 0.9]], 6);
    const reach = (d) => Math.max(...pointsOf(d).map((q) => Math.abs(q[1])));
    assert('board: a pen pressed harder draws wider; a single point is a dot', reach(heavy) > 2 * reach(light) && /a\d/.test(B.strokeOutline([[5, 5, 0.5]], 4)) && B.strokeOutline([], 4) === '');
  }

  // ---- layers ---------------------------------------------------------------------------------------
  {
    const { board, notes, regions } = sample(31, 6);
    const layers = [{ id: 'back', name: 'Sketches' }, { id: 'front', name: 'Findings', hidden: true, opacity: 0.6 }];
    const laid = B.apply(board, [{ t: 'layers', layers }, { t: 'set', id: notes[0], patch: { layer: 'front' } }, { t: 'set', id: notes[1], patch: { layer: 'back', locked: true } }]);
    const on = laid.board;
    assert('board: a board with layers and items on them is sound, and the operation undone gives the board without them', !B.checkBoard(on).length && same(B.apply(on, laid.inverse).board, board), B.checkBoard(on).join('; '));
    assert('board: an item stands on its layer or on the ground, hidden, held or faint by its own word or its layer\'s',
      same(B.standing(on, notes[0]), { layer: 1, hidden: true, locked: false, opacity: 0.6 }) && same(B.standing(on, notes[1]), { layer: 0, hidden: false, locked: true, opacity: 1 })
      && same(B.standing(on, notes[2]), { layer: -1, hidden: false, locked: false, opacity: 1 }) && B.standing(on, regions[0]).layer === -1);
    let refused = '';
    try { B.apply(on, [{ t: 'layers', layers: [layers[0]] }]); } catch (error) { refused = error.message; }
    assert('board: a layer is not removed while an item lies on it, and nothing is changed by the refusal', /lies on it/.test(refused) && on.layers.length === 2);
    const gone = B.apply(on, B.removeLayerOps(on, 'front')).board;
    assert('board: a layer removed by its operations leaves what lay on it on the ground', !B.checkBoard(gone).length && gone.layers.length === 1 && gone.items[notes[0]].layer === undefined && gone.items[notes[1]].layer === 'back');
    const stray = { ...on, items: { ...on.items, [notes[2]]: { ...on.items[notes[2]], layer: 'nowhere' } } };
    assert('board: an item on a layer the board has not, and two layers of one id, are found', B.checkBoard(stray).some((p) => /which the board has not/.test(p)) && B.checkBoard({ ...on, layers: [layers[0], layers[0]] }).some((p) => /layers/.test(p)));
  }

  // ---- the style sheet ------------------------------------------------------------------------------
  {
    const css = readFileSync(resolve(HERE, '../js/board/board.css'), 'utf8');
    const block = (selector) => { const at = css.indexOf(`${selector} {`); return css.slice(at, css.indexOf('\n}', at)); };
    const vars = (text) => Object.fromEntries([...text.matchAll(/(--bd-[a-z0-9-]+):\s*([^;]+);/g)].map((m) => [m[1], m[2].trim()]));
    const day = vars(block(':root')), night = vars(block("[data-theme='night']"));
    const names = Object.keys(day).filter((n) => n !== '--bd-ik').sort();
    assert('board: a picture carries every variable the style sheet defines', same(names, [...new Set(B.TOKENS)].sort()), names.filter((n) => !B.TOKENS.includes(n)).concat(B.TOKENS.filter((n) => !names.includes(n))).join(' '));
    const colours = names.filter((n) => !/serif|mono/.test(n));
    assert('board: night gives every colour that day gives', colours.every((n) => night[n]), colours.filter((n) => !night[n]).join(' '));
    const used = new Set([...css.matchAll(/var\((--bd-[a-z0-9-]+)/g)].map((m) => m[1]));
    assert('board: every variable used is defined', [...used].every((n) => day[n]), [...used].filter((n) => !day[n]).join(' '));

    // WCAG contrast of the ink of words (at least 7) and of a kind's label (at least 4.5) on every tone.
    const lum = (hex) => { const c = [1, 3, 5].map((k) => parseInt(hex.slice(k, k + 2), 16) / 255).map((u) => (u <= 0.04045 ? u / 12.92 : ((u + 0.055) / 1.055) ** 2.4)); return 0.2126 * c[0] + 0.7152 * c[1] + 0.0722 * c[2]; };
    const contrast = (a, b) => { const [hi, lo] = [lum(a), lum(b)].sort((p, q) => q - p); return (hi + 0.05) / (lo + 0.05); };
    let leastInk = Infinity, leastLabel = Infinity, where = '';
    for (const [name, set] of [['day', day], ['night', night]]) for (const tone of ['apricot', 'sage', 'lilac', 'sky', 'rose', 'gold', 'clay']) {
      const ink = contrast(set['--bd-ink'], set[`--bd-${tone}`]), label = contrast(set[`--bd-${tone}-ink`], set[`--bd-${tone}`]);
      if (label < leastLabel) where = `${tone} by ${name}`;
      leastInk = Math.min(leastInk, ink); leastLabel = Math.min(leastLabel, label);
    }
    for (const [name, set] of [['day', day], ['night', night]]) { const c = contrast(set['--bd-accent-ink'], set['--bd-card']); if (c < leastLabel) { leastLabel = c; where = `the accent's ink by ${name}`; } }
    check('board: words on every tone of note, by day and by night, have a contrast of at least 7 (shortfall)', Math.max(0, 7 - leastInk), 0, '');
    check(`board: the label of every kind has a contrast of at least 4.5 on its tone (shortfall; least on ${where})`, Math.max(0, 4.5 - leastLabel), 0, '');
  }
}
