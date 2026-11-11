// Checks of the agent plug-in's pure parts (plugins/ai-agent/tools.js and agent.js): what the agent may
// use follows what the person allows, while its tools and what a conversation was told stay as they
// were (a change is told in new words); every tool call is one step of the board's history, leaves the
// board sound and is undone exactly; a call that cannot be carried out changes nothing and says why;
// and the conversation stays in a form that can go on whatever happens (a call that fails, a stop, an
// answer cut short, a refusal, a provider that cannot be reached). No model is asked: `send` is scripted.
// Called by run-tests.mjs with the suite's assert and check.

import { pathToFileURL } from 'node:url';
import { join } from 'node:path';

const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);

export async function agentChecks({ assert, check, B, packs, dir }) {
  const { capabilities, toolDefinitions, systemPrompt, capsChange, createExecutor, describeBoard } = await import(pathToFileURL(join(dir, 'tools.js')).href);
  const { createAgent } = await import(pathToFileURL(join(dir, 'agent.js')).href);

  const all = [B.GENERAL, ...packs];
  const types = [...B.NOTE_TYPES, ...packs.flatMap((p) => p.noteTypes || [])];
  const modules = [
    { ref: 'market-analysis/potential.py', plugin: 'market-analysis', title: 'Potential', description: 'What a market could be worth.', params: [{ name: 'year', kind: 'integer', default: 2023, label: 'Year' }, { name: 'by', kind: 'choice', options: ['gdp', 'people'], default: 'gdp', label: 'By' }] },
    { ref: 'agents/diffusion.py', plugin: 'agents', title: 'Diffusion', description: '', params: [] },
    { ref: 'agents/broken.py', plugin: 'agents', error: 'it cannot be read' },
  ];

  // A board without a page: its history, and what the page would lend.
  function harness(caps, { answer = null } = {}) {
    const history = B.createHistory(B.emptyBoard({ id: 'b', title: 'A board' }));
    let layer = null;
    const shown = [], ran = [];
    const env = {
      B, caps, types,
      board: () => history.board,
      commit: (ops, label) => history.commit(ops, label),
      tables: () => B.collectTables(history.board),
      values: () => B.evaluate(B.collect(history.board)),
      run: async (id) => {
        ran.push(id);
        if (answer) history.commit([{ t: 'set', id, patch: { out: answer(history.board.items[id]) } }], 'a module\'s answer');
        return history.board.items[id].out;
      },
      fit: (ids) => shown.push(ids),
      layer: () => layer,
      setLayer: (id) => { layer = id; },
    };
    return { env, history, shown, ran, exec: createExecutor(env), layer: () => layer };
  }
  const refused = async (exec, name, input) => { try { await exec.execute(name, input); return ''; } catch (error) { return error.message; } };
  const overlapping = (board, ids) => ids.some((a, k) => ids.slice(k + 1).some((b) => B.intersects(board.place[a], board.place[b])));

  // ---- what the agent may use -------------------------------------------------------------------------
  const everything = capabilities({ B, packs: all, modules, allowed: null, mayRun: true });
  const some = capabilities({ B, packs: all, modules, allowed: ['general', 'agents'], mayRun: true });
  const none = capabilities({ B, packs: all, modules, allowed: [], mayRun: true });
  const noRuns = capabilities({ B, packs: all, modules, allowed: null, mayRun: false });
  {
    const count = all.reduce((n, p) => n + (p.templates || []).length, 0);
    assert('agent: allowed everything, it may lay every template, add every card and run every module that can be read',
      everything.templates.length === count && everything.modules.length === 2 && all.every((p) => Object.keys(p.cards || {}).every((c) => everything.cards[c])) && !everything.cards.module);
    assert('agent: allowed some plug-ins, it is offered theirs alone; the board\'s own cards are always there',
      some.templates.every((t) => /^(general|agents)\//.test(t.ref)) && some.templates.length > 0 && some.modules.map((m) => m.ref).join() === 'agents/diffusion.py'
      && Boolean(some.cards.chart && some.cards.map) && Object.values(some.cards).every((c) => !c.plugin || c.plugin === 'agents') && Object.keys(everything.cards).length > Object.keys(some.cards).length);
    const tools = toolDefinitions();
    assert('agent: the tools are the same whatever is allowed, so that a conversation is not rewritten when the settings change; none names a plug-in',
      same(tools, toolDefinitions()) && ['lay_template', 'add_card', 'run_module', 'add_items', 'draw'].every((n) => tools.some((t) => t.name === n)) && !/market-analysis|diffusion|city3d/.test(JSON.stringify(tools)));
    assert('agent: every tool has a name of its own, says what it does and declares its input',
      new Set(tools.map((t) => t.name)).size === tools.length && tools.every((t) => /^[a-z_]{3,}$/.test(t.name) && t.description.length > 30 && t.input_schema.type === 'object' && Array.isArray(t.input_schema.required)));
    assert('agent: it is given no tool that writes or runs code of its own', !tools.some((t) => /code|script|python|eval/i.test(t.name)) && !JSON.stringify(tools).includes('"code"'));
    const told = systemPrompt({ caps: some, types }), again = systemPrompt({ caps: capabilities({ B, packs: all, modules, allowed: ['general', 'agents'], mayRun: true }), types });
    assert('agent: what it is told is the same for the same choices (so that it is read from the cache), names what is allowed and nothing that is not',
      told === again && told.includes('agents/diffusion.py') && !told.includes('market-analysis/') && types.every((t) => told.includes(t.id)) && systemPrompt({ caps: noRuns, types }).includes('may not run modules')
      && systemPrompt({ caps: none, types }).includes('No template is yours') && systemPrompt({ caps: everything, types, today: '2026-11-11' }).includes('Today is 2026-11-11'));
    // A change of what it may use, as the model is told it beside the person's next words.
    const narrowed = capsChange(everything, some), widened = capsChange(some, everything), off = capsChange(everything, noRuns);
    assert('agent: a change of what it may use is told in words: what is no longer its to use by name, what is newly so as it would have been told at the beginning; no change, no words',
      capsChange(everything, everything) === '' && capsChange(some, capabilities({ B, packs: all, modules, allowed: ['general', 'agents'], mayRun: true })) === ''
      && /No longer yours/.test(narrowed) && narrowed.includes('market-analysis/potential.py') && !/Now yours/.test(narrowed) && !narrowed.includes('agents/diffusion.py')
      && /Now yours as well/.test(widened) && widened.includes('market-analysis/potential.py: Potential.') && widened.includes('Params: year (integer)') && !/No longer/.test(widened)
      && /may no longer run any/.test(off) && !off.includes('potential.py') && /^\[.*\]$/s.test(widened), narrowed.slice(0, 200));
  }

  // ---- the tools on a board ---------------------------------------------------------------------------
  {
    const h = harness(everything), { exec, history } = h;
    const sound = () => B.checkBoard(history.board);
    const empty = await exec.execute('read_board', {});
    assert('agent: an empty board is read as one', same(empty.items, []) && empty.extent === null && history.length === 0);

    const added = await exec.execute('add_items', { items: [
      { key: 'a', kind: 'note', type: 'fact', text: 'Cafés in the centre: 1,677', source: 'CLUE 2023' },
      { key: 'b', kind: 'note', type: 'assumption', text: 'share = 3% [2%, 5%]' },
      { key: 'c', kind: 'note', type: 'question', text: 'Who buys in the morning?' },
      { key: 'd', kind: 'note', text: 'A long idea, '.repeat(30) },
      { key: 'e', kind: 'note', type: 'risk', text: 'Rents rise' },
      { key: 't', kind: 'text', size: 'title', text: 'The market for a café' },
      { key: 'f', kind: 'figure', title: 'Sales', formula: 'buyers = 40k [30k, 55k]\nsales = buyers * share' },
    ] });
    const ids = added.added.map((x) => x.id), board1 = history.board;
    assert('agent: items added without places are laid where they lie on nothing, each with the id it is known by, in one step of the history',
      added.added.length === 7 && sound().length === 0 && history.length === 1 && !overlapping(board1, ids) && ids.every((i) => board1.items[i]) && board1.items[exec.refs.get('b')].type === 'assumption', sound()[0] || '');
    assert('agent: a note with much to say is given the height for it', board1.place[exec.refs.get('d')].h > board1.place[exec.refs.get('a')].h);
    const read = await exec.execute('read_board', {});
    check('agent: the quantities the notes define are read with their values and ranges', Math.abs(read.quantities.sales.value - 1200) + Math.abs(read.quantities.sales.low - 600) + Math.abs(read.quantities.sales.high - 2750), 1e-9);

    const regioned = await exec.execute('add_items', { items: [
      { kind: 'note', text: 'one', in: 'r' }, { kind: 'note', text: 'two', in: 'r' }, { kind: 'note', text: 'three', in: 'r' }, { kind: 'note', text: 'four', in: 'r' }, { kind: 'note', text: 'five', in: 'r' },
      { key: 'r', kind: 'region', title: 'Themes', w: 480, h: 200 },
    ] });
    const r = exec.refs.get('r'), within = regioned.added.filter((x) => x.kind === 'note').map((x) => x.id), board2 = history.board;
    const inside = (id) => { const p = board2.place[id], q = board2.place[r]; return p.x >= q.x && p.y >= q.y && p.x + p.w <= q.x + q.w && p.y + p.h <= q.y + q.h; };
    assert('agent: items placed within a region lie in it, belong to it, and it grows to hold them; the region lies behind them',
      sound().length === 0 && within.every((i) => board2.items[i].in === r && inside(i)) && !overlapping(board2, within) && board2.place[r].h > 200 && board2.order.indexOf(r) < Math.min(...within.map((i) => board2.order.indexOf(i))), sound()[0] || '');
    assert('agent: what was there is not laid upon', !overlapping(board2, [...ids, r]));

    const joined = await exec.execute('connect', { links: [{ from: 'a', to: 'b', rel: 'supports' }, { from: 'b', to: ids[2], rel: 'causes', label: 'so' }, { from: 'a', to: 'b', rel: 'contradicts' }] });
    assert('agent: items are joined by the keys it gave or by their ids, and two items are not joined twice the same way',
      joined.joined.length === 2 && sound().length === 0 && B.linksOf(history.board, [exec.refs.get('b')]).length === 2);
    const before = history.board, steps = history.length;
    const whys = [
      await refused(exec, 'connect', { links: [{ from: 'a', to: 'c' }, { from: 'a', to: 'nobody' }] }),
      await refused(exec, 'connect', { links: [{ from: 'a', to: 'c', rel: 'loves' }] }),
      await refused(exec, 'add_items', { items: [{ kind: 'note', text: 'x' }, { kind: 'note', type: 'opinion', text: 'y' }] }),
      await refused(exec, 'add_items', { items: [{ kind: 'card' }] }),
      await refused(exec, 'add_items', { items: [{ kind: 'note', text: 'x', in: 'a' }] }),
      await refused(exec, 'update_items', { changes: [{ id: 'a', text: 'changed' }, { id: 'a', when: 'next spring' }] }),
      await refused(exec, 'remove_items', { ids: ['a', 'zzz'] }),
      await refused(exec, 'draw', { strokes: [{ points: [[0, 0]] }] }),
      await refused(exec, 'read_table', { name: 'nothing' }),
      await refused(exec, 'arrange', { how: 'spiral' }),
      await refused(exec, 'layers', { action: 'set', layer: 'none', hidden: true }),
      await refused(exec, 'no_such_tool', {}),
      await refused(exec, 'add_items', null),
      await refused(exec, 'add_items', { items: [{ kind: 'schedule', title: 'A plan', source: 'Survey : 2026-11-16, 2w\nReview sales, costs : after Survey, 3d\nDecide : after Review sales, costs, milestone' }] }),
      await refused(exec, 'add_items', { items: [{ kind: 'schedule', title: 'A plan' }] }),
      await refused(exec, 'update_items', { changes: [{ id: joined.joined[0].id, rel: 'adores' }] }),
    ];
    assert('agent: a call that cannot be carried out changes nothing, though part of it could be, and says why in words the model can act on',
      history.board === before && history.length === steps && whys.every((w) => w.length > 12) && /nobody/.test(whys[0]) && /supports/.test(whys[1]) && /opinion/.test(whys[2]) && /idea/.test(whys[2]) && /not a region/.test(whys[4]) && /2026-11-16/.test(whys[5])
      && /line 3/.test(whys[13]) && /no comma/.test(whys[13]) && /no entries/.test(whys[14]) && /adores/.test(whys[15]), whys.find((w) => w.length <= 12) ?? '');
    // A region that stood on the board grows by an operation, as anything else changes: the board it stood on is not written on.
    const standingBoard = history.board, was = JSON.stringify(standingBoard.place[r]);
    await exec.execute('add_items', { items: [1, 2, 3, 4].map((k) => ({ kind: 'note', text: `more ${k}`, in: r })) });
    assert('agent: a region that was on the board grows to hold what it is given by an operation that is undone with the rest',
      JSON.stringify(standingBoard.place[r]) === was && history.board.place[r].h > JSON.parse(was).h && (history.undo(), JSON.stringify(history.board.place[r]) === was) && (history.redo(), true) && sound().length === 0);
    const planned = await exec.execute('add_items', { items: [{ key: 'plan', kind: 'schedule', title: 'A plan', source: 'Survey : 2026-11-16, 2w\nReview : after Survey, 3d\nDecide : after Review, milestone' }, { key: 'sum', kind: 'figure', title: 'Sum', formula: 'total = buyers * nothing_defined' }] });
    assert('agent: a plan that is read is laid; a figure that uses a name no note defines is laid and its trouble told, for the model to mend',
      history.board.items[exec.refs.get('plan')].kind === 'schedule' && planned.problems?.length === 1 && /total/.test(planned.problems[0]) && /nothing_defined/.test(planned.problems[0])
      && !(await exec.execute('update_items', { changes: [{ id: 'sum', formula: 'total = 2 * 3' }] })).problems);
    await exec.execute('remove_items', { ids: ['plan', 'sum'] });

    await exec.execute('update_items', { changes: [{ id: 'a', text: 'Cafés and restaurants: 1,677', tone: 'gold' }, { id: 'c', x: 2000, y: 40 }, { id: joined.joined[0].id, rel: 'contradicts' }] });
    const board3 = history.board;
    assert('agent: items are changed in the fields given and no others',
      board3.items[exec.refs.get('a')].text === 'Cafés and restaurants: 1,677' && board3.items[exec.refs.get('a')].source === 'CLUE 2023' && board3.items[exec.refs.get('a')].tone === 'gold'
      && board3.place[exec.refs.get('c')].x === 2000 && board3.place[exec.refs.get('c')].w === before.place[exec.refs.get('c')].w && board3.items[joined.joined[0].id].rel === 'contradicts' && sound().length === 0);
    // A note moved into a region comes to belong to it.
    const q = board3.place[r];
    await exec.execute('update_items', { changes: [{ id: 'e', x: q.x + 20, y: q.y + q.h - 150 }] });
    assert('agent: an item moved into a region belongs to it', history.board.items[exec.refs.get('e')].in === r && sound().length === 0);

    const drawn = await exec.execute('draw', { strokes: [{ points: [[100, 100], [220, 130], [340, 100]], colour: 'rose', width: 4 }, { points: [[0, 0], [50, 0], [50, 50]], closed: true, marker: true }] });
    const ink = history.board.items[drawn.drawn[0]], at = history.board.place[drawn.drawn[0]], loop = history.board.items[drawn.drawn[1]];
    assert('agent: a stroke of the pen passes through the points given, within its rectangle; a closed one returns to where it began',
      ink.kind === 'ink' && ink.colour === 'rose' && ink.points.length === 3 && ink.points.every(([x, y], k) => Math.abs(x + at.x - [100, 220, 340][k]) < 0.11 && Math.abs(y + at.y - [100, 130, 100][k]) < 0.11 && x >= 0 && y >= 0 && x <= at.w && y <= at.h)
      && loop.points.length === 4 && same(loop.points[0], loop.points[3]) && loop.marker === true && sound().length === 0, sound()[0] || '');

    const tabled = await exec.execute('add_table', { name: 'Sales by town', csv: 'town,lat,lon,sales\nCarlton,-37.80,144.97,120\nFitzroy,-37.80,144.98,95\nRichmond,-37.82,145.00,140\n', source: 'made up for the checks' });
    const rows = await exec.execute('read_table', { name: tabled.name, rows: 2, from: 1 });
    const second = await exec.execute('add_table', { name: 'Sales by town', csv: 'a,b\n1,2\n' });
    assert('agent: a table laid from delimited text is read by its name, and a second of the same name is given another',
      tabled.rows === 3 && /^[A-Za-z_][A-Za-z0-9_]*$/.test(tabled.name) && same(rows.values, [['Fitzroy', -37.8, 144.98, 95], ['Richmond', -37.82, 145, 140]]) && second.name !== tabled.name && sound().length === 0
      && (await exec.execute('read_board', {})).tables.length === 2, tabled.name);
    const card = await exec.execute('add_card', { card: 'map', props: { table: tabled.name, value: 'sales' }, key: 'm' });
    const waiting = await exec.execute('add_card', { card: 'chart', props: { table: 'not_yet' } });
    assert('agent: a card is added with its props; one that names a table not yet on the board is added and said to wait for it',
      history.board.items[card.id].card === 'map' && history.board.items[card.id].props.value === 'sales' && !card.note && /not_yet/.test(waiting.note) && sound().length === 0);

    const laid = await exec.execute('lay_template', { template: everything.templates[0].ref });
    assert('agent: a template is laid beside what is there, and its items are told with their ids',
      laid.laid.length > 0 && laid.laid.every((x) => history.board.items[x.id]) && sound().length === 0 && !overlapping(history.board, [laid.laid.find((x) => x.kind === 'region')?.id || laid.laid[0].id, r]));

    await exec.execute('arrange', { how: 'grid', ids: ['a', 'b', 'c', 'd'] });
    assert('agent: items set out as a grid lie on none of each other', !overlapping(history.board, ['a', 'b', 'c', 'd'].map((k) => exec.refs.get(k))) && sound().length === 0);

    const layer = await exec.execute('layers', { action: 'add', name: 'Sketches' });
    const on = await exec.execute('add_items', { items: [{ key: 'sk', kind: 'note', text: 'a sketch' }] });
    await exec.execute('layers', { action: 'move', layer: layer.layer, ids: ['a'] });
    await exec.execute('layers', { action: 'set', layer: layer.layer, hidden: true, opacity: 0.5 });
    const hiddenNow = B.standing(history.board, on.added[0].id).hidden && B.standing(history.board, exec.refs.get('a')).hidden && !B.standing(history.board, exec.refs.get('b')).hidden;
    await exec.execute('layers', { action: 'set', layer: layer.layer, hidden: false, opacity: 1 });
    const plain = same(history.board.layers, [{ id: layer.layer, name: 'Sketches' }]);
    await exec.execute('layers', { action: 'remove', layer: layer.layer });
    assert('agent: a layer is added and new items are laid on it; hidden, what lies on it is hidden; made plain again it keeps no setting; removed, its items are left on the ground',
      hiddenNow && plain && h.layer() === null && history.board.items[on.added[0].id].layer === undefined && history.board.items[exec.refs.get('a')] && (history.board.layers || []).length === 0 && sound().length === 0, sound()[0] || '');

    await exec.execute('show', { ids: ['a', 'b'] });
    await exec.execute('show', {});
    assert('agent: it brings what it names into the person\'s view, or the whole board', same(h.shown, [[exec.refs.get('a'), exec.refs.get('b')], null]));

    const gone = await exec.execute('remove_items', { ids: ['a', r] });
    assert('agent: an item is removed with the links on it; what lay in a removed region stays',
      gone.removed === 2 && !history.board.items[exec.refs.get('a')] && !history.board.items[r] && within.every((i) => history.board.items[i]) && B.linksOf(history.board, [exec.refs.get('b')]).length === 1 && sound().length === 0, sound()[0] || '');

    // Every call was one step: undone one by one, the board is as it was, exactly.
    const final = history.board, n = history.length;
    while (history.canUndo) history.undo();
    const back = same(history.board, B.emptyBoard({ id: 'b', title: 'A board' }));
    while (history.canRedo) history.redo();
    assert('agent: everything it did is undone step by step to the empty board, and done again to the same board', back && same(history.board, final) && history.length === n, `${n} steps`);
    const described = describeBoard(B, history.board, B.collectTables(history.board), B.evaluate(B.collect(history.board)));
    assert('agent: what it reads of a board names every item that is not a link or a stroke, with its place',
      described.items.length === history.board.order.filter((i) => !['link', 'ink'].includes(history.board.items[i].kind)).length && described.items.every((x) => Number.isFinite(x.x) && x.id && x.kind) && described.strokes_of_the_pen === 2 && described.links.length === history.board.order.filter((i) => history.board.items[i].kind === 'link').length && described.links.some((l) => l.from === exec.refs.get('b')));
  }

  // ---- what is not allowed is refused; modules ----------------------------------------------------------
  {
    const h = harness(some, { answer: (item) => ({ ran: '2026-11-11T00:00:00Z', seconds: 0.1, notes: [`spread with ${JSON.stringify(item.props.params)}`], figures: [],
      tables: [{ name: 'spread', title: '', source: '', rows: 2, columns: [{ name: 'step', kind: 'number' }, { name: 'share', kind: 'number' }], data: { step: [0, 1], share: [0, 0.1] } }] }) });
    const { exec, history } = h;
    const whys = [
      await refused(exec, 'lay_template', { template: 'market-analysis/market-size' }),
      await refused(exec, 'run_module', { module: 'market-analysis/potential.py' }),
      await refused(exec, 'run_module', { module: 'agents/broken.py' }),
      await refused(exec, 'add_card', { card: 'city3d' }),
      await refused(exec, 'add_card', { card: 'module', props: { code: 'import os' } }),
    ];
    assert('agent: a template, a card or a module of a plug-in that is not allowed is refused though the model names it, and a script of its own cannot be laid as a card',
      whys.every((w) => /not a (template|card|module) you may/.test(w)) && history.length === 0 && h.ran.length === 0, whys.join(' | '));
    // What it may use follows the settings at once: the executor reads them when a call comes, not when the conversation began.
    const live = harness(everything), turnedOff = capabilities({ B, packs: all, modules, allowed: ['general'], mayRun: false });
    live.env.caps = turnedOff;
    const lateWhys = [await refused(live.exec, 'run_module', { module: 'agents/diffusion.py' }), await refused(live.exec, 'lay_template', { template: 'agents/spread' }), await refused(live.exec, 'add_card', { card: 'run' })];
    assert('agent: when the person takes something from it in the middle of a conversation, the next call for it is refused',
      /turned that off/.test(lateWhys[0]) && /not a template you may/.test(lateWhys[1]) && /not a card you may/.test(lateWhys[2]) && live.history.length === 0, lateWhys.join(' | '));
    const ran = await exec.execute('run_module', { module: 'agents/diffusion.py' });
    const cardItem = history.board.items[ran.card_id];
    assert('agent: a module that is allowed is laid on the board as its card, run, and its notes and tables are told',
      cardItem.card === 'module' && cardItem.props.module === 'agents/diffusion.py' && cardItem.props.code === undefined && h.ran.length === 1 && ran.tables[0].name === 'spread' && same(ran.tables[0].first, [[0, 0], [1, 0.1]]) && ran.notes.length === 1
      && (await exec.execute('read_table', { name: 'spread' })).rows === 2 && B.checkBoard(history.board).length === 0);
    const everythingH = harness(everything, { answer: (item) => ({ ran: 'now', seconds: 0, notes: [JSON.stringify(item.props.params)], tables: [], figures: [] }) });
    const first = await everythingH.exec.execute('run_module', { module: 'market-analysis/potential.py', params: { year: 2020 } });
    const again = await everythingH.exec.execute('run_module', { module: 'market-analysis/potential.py', params: { by: 'people' }, card_id: first.card_id });
    const cards = everythingH.history.board.order.filter((i) => everythingH.history.board.items[i].kind === 'card');
    assert('agent: a module run again on its card keeps the card and the parameters set before; a parameter it does not have is refused',
      cards.length === 1 && again.card_id === first.card_id && same(everythingH.history.board.items[first.card_id].props.params, { year: 2020, by: 'people' })
      && /no parameter colour/.test(await refused(everythingH.exec, 'run_module', { module: 'market-analysis/potential.py', params: { colour: 'red' } })) && cards.length === everythingH.history.board.order.length);
    const failing = harness(everything, { answer: () => ({ error: 'Traceback\nValueError: no such year', failed: 'now' }) });
    const why = await refused(failing.exec, 'run_module', { module: 'agents/diffusion.py' });
    assert('agent: a module that fails says so with its reason, as an error the model reads', /no such year/.test(why) && !/Traceback/.test(why), why);
  }

  // ---- the conversation ---------------------------------------------------------------------------------
  // A conversation can go on when it begins with the person, every call of a message is answered in the
  // next message, in order and first, and no answer is to a call that was not made.
  const canGoOn = (messages) => {
    if (messages.length && messages[0].role !== 'user') return 'it does not begin with the person';
    for (let k = 0; k < messages.length; k++) {
      const m = messages[k], calls = m.role === 'assistant' ? m.content.filter((b) => b.type === 'tool_use').map((b) => b.id) : [];
      const answers = m.role === 'user' ? m.content.filter((b) => b.type === 'tool_result').map((b) => b.tool_use_id) : [];
      if (calls.length) {
        const next = messages[k + 1];
        if (!next || next.role !== 'user' || !same(next.content.filter((b) => b.type === 'tool_result').map((b) => b.tool_use_id), calls)) return `the calls of message ${k} are not answered in the next`;
      }
      if (answers.length) {
        const prev = messages[k - 1];
        if (!prev || prev.role !== 'assistant' || !same(prev.content.filter((b) => b.type === 'tool_use').map((b) => b.id), answers)) return `message ${k} answers calls that were not made`;
      }
      if (!Array.isArray(m.content) || !m.content.length) return `message ${k} is empty`;
    }
    return '';
  };
  const scripted = (answers) => { const sent = []; let k = 0; return { sent, send: async (request) => { sent.push(JSON.parse(JSON.stringify(request))); const a = answers[k++]; if (a instanceof Error) throw a; if (typeof a === 'function') return a(request); return a; } }; };
  const use = (id, name, input) => ({ type: 'tool_use', id, name, input });
  const say = (text) => ({ type: 'text', text });
  const usage = { input_tokens: 10, output_tokens: 5, cache_read_input_tokens: 90, cache_creation_input_tokens: 0 };
  const agentOn = (h, script, extra = {}) => {
    const events = [];
    const agent = createAgent({ send: script.send, execute: (name, input) => h.exec.execute(name, input), system: () => 'S', tools: () => toolDefinitions(), on: (e) => events.push(e), ...extra });
    return { agent, events };
  };
  {
    const h = harness(everything);
    const thinking = { type: 'thinking', thinking: '', signature: 'sig-1' };
    const script = scripted([
      { content: [thinking, say('I will add two notes.'), use('t1', 'add_items', { items: [{ key: 'x', kind: 'note', text: 'one' }, { key: 'y', kind: 'note', text: 'two' }] }), use('t2', 'connect', { links: [{ from: 'x', to: 'ghost' }] })], stop_reason: 'tool_use', usage, model: 'm' },
      { content: [use('t3', 'connect', { links: [{ from: 'x', to: 'y', rel: 'causes' }] })], stop_reason: 'tool_use', usage, model: 'm' },
      { content: [say('Two notes, joined.')], stop_reason: 'end_turn', usage, model: 'm' },
      { content: [say('Nothing more.')], stop_reason: 'end_turn', usage, model: 'm' },
    ]);
    const { agent, events } = agentOn(h, script);
    const out = await agent.ask('Add two notes and join them.');
    const m = agent.messages;
    assert('agent: the model\'s calls are carried out on the board and answered in one message, in their order; a call that fails is answered as an error and the rest go on',
      out.ended === 'answered' && out.words === 'Two notes, joined.' && m.length === 6 && canGoOn(m) === '' && m[2].content.length === 2 && !m[2].content[0].is_error && m[2].content[1].is_error === true && /ghost/.test(m[2].content[1].content)
      && h.history.board.order.length === 3 && B.checkBoard(h.history.board).length === 0, canGoOn(m));
    assert('agent: what the model said is passed back as it came, its own blocks among them', m[1].content[0] === thinking && same(script.sent[1].messages[1].content[0], thinking) && script.sent[2].messages.length === 5);
    assert('agent: each step is asked with the same system prompt and tools, so that they are read from the cache', script.sent.every((s) => s.system === 'S' && same(s.tools, script.sent[0].tools)));
    assert('agent: what happens is told as it happens: what is said, each call and its answer, and the end',
      events.map((e) => e.type).join() === 'step,said,call,result,call,result,step,call,result,step,said,ended' && events.filter((e) => e.type === 'result').map((e) => e.ok).join() === 'true,false,true');
    await agent.ask('Anything more?', '[Selected on the board: x]');
    assert('agent: the conversation goes on from what was said, and its tokens are counted, those read from the cache among them',
      script.sent[3].messages.length === 7 && canGoOn(agent.messages) === '' && agent.usage.steps === 4 && agent.usage.input === 400 && agent.usage.cached === 360 && agent.usage.output === 20);
    // The provider keeps a model's reasoning only while what came before it stands as it was sent.
    const prefix = (k) => JSON.stringify(script.sent[k].messages);
    assert('agent: a conversation is only added to: each request begins with the whole of the one before, unchanged; what the page adds beside the person\'s words is a part of their message',
      [1, 2, 3].every((k) => prefix(k).startsWith(prefix(k - 1).slice(0, -1))) && same(script.sent[3].messages[6].content.map((c) => c.text), ['Anything more?', '[Selected on the board: x]']));
    agent.reset();
    assert('agent: a new conversation begins with nothing', agent.messages.length === 0 && agent.usage.steps === 0);
  }
  {
    // The provider cannot be reached: the person's words are taken back, to be asked again.
    const h = harness(everything), script = scripted([new Error('the provider could not be reached'), { content: [say('Here.')], stop_reason: 'end_turn', usage }]);
    const { agent } = agentOn(h, script);
    const failed = await agent.ask('Hello');
    const then = await agent.ask('Hello');
    assert('agent: a step that fails ends the turn with the reason, and the words can be asked again', failed.ended === 'failed' && /reached/.test(failed.words) && then.ended === 'answered' && agent.messages.length === 2 && canGoOn(agent.messages) === '');
    // It fails after work was done: what was done is kept in the conversation.
    const script2 = scripted([{ content: [use('a1', 'add_items', { items: [{ kind: 'note', text: 'kept' }] })], stop_reason: 'tool_use', usage }, new Error('too many requests'), { content: [say('Done.')], stop_reason: 'end_turn', usage }]);
    const two = agentOn(harness(everything), script2);
    const midway = await two.agent.ask('Add a note');
    const resumed = await two.agent.ask('go on');
    assert('agent: a failure after work was done leaves the conversation able to go on with that work in it', midway.ended === 'failed' && two.agent.messages.length === 5 && canGoOn(two.agent.messages) === '' && resumed.ended === 'answered');
  }
  {
    // A refusal answers nothing: the turn is taken back. An answer cut short: its calls are not carried out.
    const h = harness(everything);
    const script = scripted([
      { content: [say('Earlier.')], stop_reason: 'end_turn', usage },
      { content: [], stop_reason: 'refusal', usage },
      { content: [say('I began to'), use('c1', 'add_items', { items: [{ kind: 'note' }] })], stop_reason: 'max_tokens', usage },
      { content: [say('On we go.')], stop_reason: 'end_turn', usage },
    ]);
    const { agent } = agentOn(h, script);
    await agent.ask('First');
    const declined = await agent.ask('Second');
    const kept = agent.messages.length;
    const cut = await agent.ask('Third');
    const on = await agent.ask('Fourth');
    assert('agent: a refusal ends the turn and is taken out of the conversation; an answer cut short is kept, its calls answered as not carried out, and nothing is done to the board',
      declined.ended === 'refused' && kept === 2 && cut.ended === 'cut' && h.history.length === 0 && agent.messages[4].content[0].is_error === true && on.ended === 'answered' && canGoOn(agent.messages) === '', canGoOn(agent.messages));
  }
  {
    // Stopped while it works: the calls not yet carried out are answered as stopped. And a bound on the steps.
    const h = harness(everything);
    let agentRef = null;
    const script = scripted([
      () => { queueMicrotask(() => {}); return { content: [use('s1', 'add_items', { items: [{ kind: 'note', text: 'first' }] }), use('s2', 'add_items', { items: [{ kind: 'note', text: 'second' }] })], stop_reason: 'tool_use', usage }; },
      { content: [say('never asked')], stop_reason: 'end_turn', usage },
      { content: [say('Going on.')], stop_reason: 'end_turn', usage },
    ]);
    const made = createAgent({ send: script.send, system: () => 'S', tools: () => [], execute: async (name, input) => { const got = await h.exec.execute(name, input); agentRef.stop(); return got; } });
    agentRef = made;
    const stopped = await made.ask('Add two notes');
    assert('agent: stopped while it works, it finishes the call in hand, answers the rest as not carried out and asks the model no more',
      stopped.ended === 'stopped' && h.history.board.order.length === 1 && script.sent.length === 1 && made.messages[2].content[1].is_error === true && canGoOn(made.messages) === '' && !made.busy);
    script.sent.length = 0;
    const second = await made.ask('go on');
    assert('agent: a stopped conversation goes on when asked', second.ended === 'answered' && canGoOn(made.messages) === '');

    const forever = scripted(Array.from({ length: 10 }, (_, k) => ({ content: [use(`f${k}`, 'read_board', {})], stop_reason: 'tool_use', usage })));
    const bounded = agentOn(harness(everything), forever, { mostSteps: 4 });
    const capped = await bounded.agent.ask('Read the board for ever');
    let twice = '';
    const slow = createAgent({ send: () => new Promise((r) => setTimeout(() => r({ content: [say('ok')], stop_reason: 'end_turn', usage }), 30)), execute: async () => ({}), system: () => '', tools: () => [] });
    const pending = slow.ask('one');
    try { await slow.ask('two'); } catch (error) { twice = error.message; }
    await pending;
    // Stopped while the model is being asked: the page gives the request up, and that is a stop.
    let giveUp = null;
    const waiting = createAgent({ send: () => new Promise((_, no) => { giveUp = () => no(new Error('given up')); }), execute: async () => ({}), system: () => '', tools: () => [], giveUp: () => giveUp() });
    const asked = waiting.ask('a long question');
    await new Promise((r) => setTimeout(r, 5));
    waiting.stop();
    const gaveUp = await asked;
    assert('agent: stopped while the model is being asked, the request is given up and the turn is taken back', gaveUp.ended === 'stopped' && waiting.messages.length === 0 && !waiting.busy);
    assert('agent: it stops after a bound on its steps, able to go on; and it is not asked twice at once', capped.ended === 'steps' && forever.sent.length === 4 && canGoOn(bounded.agent.messages) === '' && /at work/.test(twice) && slow.messages.length === 2);
  }
}
