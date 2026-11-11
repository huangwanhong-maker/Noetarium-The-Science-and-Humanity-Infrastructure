// An agent, as a plug-in of the board: the person says in words what they want on the board, and a
// language model carries it out with the board's own operations: notes, regions, links, strokes of the
// pen, tables, plans, figures; and with what the other plug-ins bring, as far as the person allows:
// their templates, their cards, their modules. What it does is done in the open, step by step, and is
// undone like anything else.
//
// The plug-in brings a panel (the conversation, and its settings), not templates. Its parts:
//
//   tools.js    what the agent can do: tool definitions from what is allowed, and their executor (pure)
//   agent.js    the conversation: a loop of asking the model and carrying out its calls (pure)
//   server.py   who answers: a provider and its models, the key (kept by the server, never sent back), one step
//
// The settings say who answers (a provider, a key, a model, how hard it thinks) and what the agent may
// use (which plug-ins, and whether it may run modules).

import { capabilities, toolDefinitions, systemPrompt, capsChange, createExecutor } from './tools.js';
import { createAgent } from './agent.js';

const SUGGESTIONS = [
  'Set out a brainstorm on what makes a neighbourhood café succeed, grouped by theme',
  'Size the market for a specialty café in Melbourne, with every assumption on a note',
  'Draw a six-week plan to test the idea, from next Monday',
  'Tidy the board, and join what bears on what',
];

// What a call is, in a few words: while it is carried out, and once it is.
const count = (n, one, many = `${one}s`) => `${n} ${n === 1 ? one : many}`;
const DOING = {
  read_board: () => 'Reading the board', add_items: (i) => `Adding ${count(i.items?.length || 0, 'item')}`, connect: (i) => `Joining ${count(i.links?.length || 0, 'pair')}`,
  update_items: (i) => `Changing ${count(i.changes?.length || 0, 'item')}`, remove_items: (i) => `Removing ${count(i.ids?.length || 0, 'item')}`, draw: (i) => `Drawing ${count(i.strokes?.length || 0, 'stroke')}`,
  add_table: (i) => `Laying the table ${i.name || ''}`, read_table: (i) => `Reading the table ${i.name || ''}`, arrange: (i) => `Setting out: ${i.how || ''}`, layers: (i) => `Layers: ${i.action || ''}`,
  show: () => 'Bringing it into view', lay_template: (i) => `Laying the template ${i.template || ''}`, add_card: (i) => `Adding a card: ${i.card || ''}`, run_module: (i) => `Running ${i.module || 'a module'}`,
};
const DONE = {
  read_board: (i, a) => `Read the board (${count(a.items?.length || 0, 'item')})`, add_items: (i, a) => `Added ${count(a.added?.length || 0, 'item')}`, connect: (i, a) => `Joined ${count(a.joined?.length || 0, 'pair')}`,
  update_items: (i, a) => `Changed ${count(a.changed || 0, 'item')}`, remove_items: (i, a) => `Removed ${count(a.removed || 0, 'item')}`, draw: (i, a) => `Drew ${count(a.drawn?.length || 0, 'stroke')}`,
  add_table: (i, a) => `Laid the table ${a.name} (${count(a.rows, 'row')})`, read_table: (i, a) => `Read the table ${a.name}`, arrange: (i, a) => `Set out as ${i.how} (${count(a.moved || 0, 'item')} moved)`,
  layers: (i) => `Layers: ${i.action}`, show: () => 'Brought into view', lay_template: (i, a) => `Laid the template ${i.template} (${count(a.laid?.length || 0, 'item')})`,
  add_card: (i) => `Added a card: ${i.card}`, run_module: (i, a) => `Ran ${i.module}${a.failed ? ', which failed' : a.tables?.length ? `: ${a.tables.map((t) => t.name).join(', ')}` : ''}`,
};

export default function aiAgent(B) {
  const { h, clear } = B;

  // Words with a little marking, as models write them: paragraphs, lists, **strong**, *slanted*, `code`. Made as nodes, never as markup.
  function inline(text) {
    const out = [];
    const pattern = /(\*\*([^*]+)\*\*|`([^`]+)`|\*([^*\s][^*]*)\*)/g;
    let last = 0, m;
    while ((m = pattern.exec(text))) {
      if (m.index > last) out.push(text.slice(last, m.index));
      out.push(m[2] !== undefined ? h('b', {}, m[2]) : m[3] !== undefined ? h('code', {}, m[3]) : h('i', {}, m[4]));
      last = m.index + m[0].length;
    }
    if (last < text.length) out.push(text.slice(last));
    return out;
  }
  function written(text) {
    const nodes = [], item = /^\s*([-*•]|\d+[.)])\s+/;
    for (const block of String(text).trim().split(/\n{2,}/)) {
      // Within a block, lines that begin as a list's are a list; the lines between them are paragraphs.
      const lines = block.split('\n');
      for (let k = 0; k < lines.length;) {
        const listed = item.test(lines[k]);
        let end = k + 1;
        while (end < lines.length && item.test(lines[end]) === listed) end++;
        const run = lines.slice(k, end);
        if (listed) nodes.push(h(/^\s*\d/.test(run[0]) ? 'ol' : 'ul', {}, run.map((l) => h('li', {}, inline(l.replace(item, ''))))));
        else if (/^#{1,4}\s+/.test(run[0]) && run.length === 1) nodes.push(h('p', { class: 'ag-strong' }, inline(run[0].replace(/^#{1,4}\s+/, ''))));
        else nodes.push(h('p', {}, run.flatMap((l, j) => (j ? [h('br', {}), ...inline(l)] : inline(l)))));
        k = end;
      }
    }
    return nodes;
  }
  const amount = (n) => (n >= 10000 ? `${Math.round(n / 1000)}k` : n >= 1000 ? `${(n / 1000).toFixed(1)}k` : String(n));

  function mount(host, ctx) {
    const { api, packs, request, close } = ctx;
    const others = packs.filter((p) => p.id !== 'ai-agent');
    let settings = null, providers = [], efforts = [], shows = 'talk', session = null, problem = '';

    const body = h('div', { class: 'ag-body' }), foot = h('div', { class: 'ag-foot' });
    const keys = { fresh: h('button', { class: 'ag-key', type: 'button', title: 'Begin a new conversation', onclick: () => { fresh(); draw(); } }, 'New'),
      settings: h('button', { class: 'ag-key', type: 'button', onclick: () => { shows = shows === 'settings' ? 'talk' : 'settings'; draw(); } }, 'Settings') };
    host.classList.add('ag');
    host.append(h('div', { class: 'ag-head' }, h('h2', {}, 'Agent'), keys.fresh, keys.settings, h('button', { class: 'ag-key', type: 'button', onclick: close }, 'Close')), body, foot);

    // ---- what it may use -------------------------------------------------------------------------------
    const modules = () => (api.services.modules || ctx.modules || []);
    const caps = () => capabilities({ B, packs: others, modules: modules(), allowed: settings?.allowed ?? null, mayRun: settings?.modules !== false });
    const brings = (pack) => {
      const parts = [];
      if (pack.templates?.length) parts.push(count(pack.templates.length, 'template'));
      const cards = Object.keys(pack.cards || {}).length, mods = modules().filter((m) => m.plugin === pack.id && !m.error).length;
      if (cards) parts.push(count(cards, 'card'));
      if (mods) parts.push(count(mods, 'module'));
      return parts;
    };
    // The plug-ins that bring the agent something; one that has only modules (no plugin.js) among them.
    const offered = () => {
      const known = others.map((p) => ({ id: p.id, name: p.name, brings: brings(p) }));
      for (const m of modules()) if (!known.some((p) => p.id === m.plugin)) known.push({ id: m.plugin, name: m.plugin, brings: [count(modules().filter((x) => x.plugin === m.plugin && !x.error).length, 'module')] });
      return known.filter((p) => p.brings.length);
    };

    // ---- the conversation --------------------------------------------------------------------------------
    // A conversation is about one board: its keys name that board's items.
    function fresh() {
      if (session) { session.over = true; session.agent.stop(); }
      const mine = { boardId: api.board.id, log: h('div', { class: 'ag-log' }), over: false, lines: new Map(), caps: caps(), controller: null, model: '', made: new Set(), showed: false };
      const env = {
        B, board: () => api.board, commit: (ops, label) => api.commit(ops, label), tables: () => api.tables, values: () => api.values,
        get caps() { return mine.caps; }, get types() { return api.types; },
        run: async (id) => { await api.run(id); return api.board.items[id]?.out; },
        fit: (ids) => api.fit(ids), layer: () => api.layer, setLayer: (id) => api.setLayer(id),
      };
      const executor = createExecutor(env);
      mine.agent = createAgent({
        execute: (name, input) => executor.execute(name, input),
        system: () => mine.system, tools: () => mine.tools,
        giveUp: () => mine.controller?.abort(),
        send: async (asked) => {
          mine.controller = new AbortController();
          try { return await request('messages', { method: 'POST', body: asked, signal: mine.controller.signal }); } finally { mine.controller = null; }
        },
        on: (event) => { if (!mine.over) told(mine, event); },
      });
      session = mine;
    }

    const toEnd = () => { body.scrollTop = body.scrollHeight; };
    const line = (cls, ...kids) => { const node = h('div', { class: `ag-line ${cls}` }, kids); session.log.append(node); toEnd(); return node; };
    function told(mine, event) {
      mine.waiting?.remove();
      mine.waiting = null;
      if (event.type === 'said') line('says', written(event.text));
      else if (event.type === 'call') mine.lines.set(event.id, line('does doing', h('i', { class: 'ag-mark' }), h('span', {}, `${(DOING[event.name] || (() => event.name))(event.input || {})}…`)));
      else if (event.type === 'result') {
        const node = mine.lines.get(event.id);
        if (!node) return;
        node.classList.remove('doing');
        node.classList.add(event.ok ? 'done' : 'failed');
        let words;
        try { words = event.ok ? (DONE[event.name] || (() => event.name))(event.input || {}, event.answer || {}) : `${event.name.replace(/_/g, ' ')}: ${event.answer}`; } catch { words = event.name; }
        node.lastChild.textContent = words;
        // What it made in this turn, to bring into view at the end unless it showed something itself.
        if (event.ok) {
          const a = event.answer || {};
          for (const made of [...(a.added || []), ...(a.laid || [])]) mine.made.add(made.id);
          for (const id of [...(a.drawn || []), a.id, a.card_id]) if (id) mine.made.add(id);
          if (event.name === 'show') mine.showed = true;
        }
        // What a module said is worth reading as it comes.
        if (event.ok && event.name === 'run_module') for (const n of event.answer.notes || []) node.append(h('small', {}, n));
      } else if (event.type === 'step') { mine.model = event.model || mine.model; drawFoot(); }
      else if (event.type === 'ended') {
        if (event.ended === 'failed') line('wrong', event.words);
        else if (event.ended !== 'answered') line('quiet', event.ended === 'cut' ? 'The answer was cut short.' : event.words);
        drawFoot();
        return;
      }
      if (mine.agent.busy && event.type !== 'call') { mine.waiting = h('div', { class: 'ag-line waiting' }, h('i', {}), h('i', {}), h('i', {})); mine.log.append(mine.waiting); toEnd(); }
    }

    // What is selected, in a line the model reads beside the person's words.
    function selected() {
      const board = api.board, ids = api.selection.filter((id) => board.items[id] && board.items[id].kind !== 'link').slice(0, 24);
      if (!ids.length) return '';
      const name = (it) => (it.text || it.title || it.name || it.card || '').split('\n')[0].slice(0, 60);
      return `[Selected on the board: ${ids.map((id) => `${id} (${board.items[id].kind}${name(board.items[id]) ? ` "${name(board.items[id])}"` : ''})`).join(', ')}]`;
    }

    async function ask(words) {
      words = words.trim();
      if (!words || session.agent.busy) return;
      if (session.boardId !== api.board.id) fresh();
      const mine = session;
      if (shows !== 'talk') shows = 'talk';
      // What it may use is as the settings stand when it is asked. What the conversation was told at its beginning is not
      // rewritten (the model's reasoning is kept only while what came before it stands): a change is told beside these words.
      const now = caps(), change = mine.system ? capsChange(mine.told, now) : '', said = mine.agent.messages.length;
      mine.caps = now;          // the executor follows the settings at once
      if (!mine.system) {
        mine.system = systemPrompt({ caps: now, types: api.types, today: new Date().toISOString().slice(0, 10) });
        mine.tools = toolDefinitions();
        mine.told = now;
      }
      mine.made = new Set();
      mine.showed = false;
      line('asks', words);
      draw();
      mine.waiting = h('div', { class: 'ag-line waiting' }, h('i', {}), h('i', {}), h('i', {}));
      mine.log.append(mine.waiting);
      toEnd();
      drawFoot();
      try { await mine.agent.ask(words, [selected(), change].filter(Boolean).join(' ')); } catch (error) { if (!mine.over) line('wrong', error.message); }
      mine.waiting?.remove();
      if (mine.over) return;
      // The change was told if these words stayed in the conversation; a conversation in which nothing stayed has not begun.
      if (mine.agent.messages.length > said) mine.told = now;
      else if (!mine.agent.messages.length) mine.system = null;
      // What was made is brought into view.
      const made = [...mine.made].filter((id) => api.board.items[id]);
      if (made.length && !mine.showed) api.fit(made);
      drawFoot();
    }

    // ---- the settings ------------------------------------------------------------------------------------
    async function keep(change) {
      try {
        settings = await request('settings', { method: 'POST', body: change });
        problem = '';
      } catch (error) { problem = error.message; }
      draw();
    }

    function settingsView() {
      const provider = providers.find((p) => p.id === settings.provider) || providers[0];
      const held = settings.keys?.[settings.provider] || { kept: false };
      const key = h('input', { type: 'password', autocomplete: 'off', spellcheck: 'false', 'aria-label': 'The key', placeholder: held.kept ? `kept, ending ${held.ends}` : provider?.key_hint || 'the key' });
      const keepKey = () => { if (key.value.trim()) keep({ key: key.value.trim() }); };
      key.addEventListener('keydown', (e) => { if (e.key === 'Enter') keepKey(); });
      const choose = (name, options, value) => h('select', { 'aria-label': name, onchange: (e) => keep({ [name]: e.target.value }) }, options.map(([v, label]) => h('option', { value: v, selected: v === value }, label)));
      const row = (label, ...kids) => h('label', { class: 'ag-row' }, h('span', {}, label), h('div', {}, kids));
      const all = offered(), allowed = settings.allowed;
      const may = (id) => allowed === null || allowed === undefined || allowed.includes(id);
      const toggle = (id, on) => {
        const next = all.map((p) => p.id).filter((p) => (p === id ? on : may(p)));
        // All of them allowed is kept as "all", so that a plug-in installed later is allowed too.
        keep({ allowed: next.length === all.length ? null : next });
      };
      return [
        h('h3', {}, 'Who answers'),
        // Another provider is chosen with the first of its models.
        row('Provider', h('select', { 'aria-label': 'provider', onchange: (e) => keep({ provider: e.target.value, model: providers.find((p) => p.id === e.target.value)?.models[0]?.id }) },
          providers.map((p) => h('option', { value: p.id, selected: p.id === settings.provider }, p.name)))),
        provider?.unavailable ? h('p', { class: 'ag-wrong' }, `It cannot be used yet: ${provider.unavailable}.`) : null,
        row('Key', h('div', { class: 'ag-keyrow' }, key, h('button', { class: 'ag-key edged', type: 'button', onclick: keepKey }, 'Keep'),
          held.kept ? h('button', { class: 'ag-key edged', type: 'button', title: 'Forget the key kept for this provider', onclick: () => keep({ key: '' }) }, 'Forget') : null)),
        h('p', { class: 'ag-tip' }, held.kept ? `A key is kept in your own settings on this computer (${settings.kept_in}), outside the repository. It is never sent back to this page.`
          : `No key is kept: the server's own environment is tried (ANTHROPIC_API_KEY). A key given here is kept in your own settings on this computer (${settings.kept_in}), outside the repository, and is never sent back to this page.`),
        row('Model', choose('model', (provider?.models || []).map((m) => [m.id, m.name]), settings.model)),
        row('Effort', choose('effort', efforts.map((e) => [e, e]), settings.effort)),
        h('p', { class: 'ag-tip' }, 'More effort thinks longer before it acts, and costs more.'),

        h('h3', {}, 'What it may use'),
        h('p', { class: 'ag-tip' }, 'The board\'s own tools are always its to use: notes, regions, links, the pen, tables, plans, figures, charts and maps. Of the plug-ins, it may draw on those ticked.'),
        h('div', { class: 'ag-plugins' }, all.map((p) => h('label', { class: 'ag-plugin' },
          h('input', { type: 'checkbox', checked: may(p.id), 'data-plugin': p.id, onchange: (e) => toggle(p.id, e.target.checked) }), h('b', {}, p.name), h('small', {}, p.brings.join(' · '))))),
        h('label', { class: 'ag-plugin' }, h('input', { type: 'checkbox', checked: settings.modules !== false, 'data-may': 'run', onchange: (e) => keep({ modules: e.target.checked }) }), h('b', {}, 'It may run modules'),
          h('small', {}, 'A module gets data and computes on this computer, with your authority. It runs only those installed; it writes none.')),

        h('h3', {}, 'What it is given'),
        h('p', { class: 'ag-tip' }, 'To work on a board the agent reads it: its notes, tables and quantities go to the provider with what you ask. Everything it does is a step you can undo.'),
        problem ? h('p', { class: 'ag-wrong' }, problem) : null,
      ];
    }

    // ---- drawing -------------------------------------------------------------------------------------------
    const input = h('textarea', { rows: '2', placeholder: 'Say what you want on the board…', 'aria-label': 'What you want on the board', spellcheck: 'true' });
    input.addEventListener('keydown', (e) => {
      e.stopPropagation();
      if (e.key === 'Enter' && !e.shiftKey && !e.isComposing) { e.preventDefault(); const words = input.value; input.value = ''; ask(words); }
    });
    const sendKey = h('button', { class: 'ag-send', type: 'button', onclick: () => { if (session.agent.busy) session.agent.stop(); else { const words = input.value; input.value = ''; ask(words); } } }, 'Send');
    const used = h('div', { class: 'ag-used' });

    function drawFoot() {
      const busy = Boolean(session?.agent.busy), u = session?.agent.usage;
      sendKey.textContent = busy ? 'Stop' : 'Send';
      sendKey.classList.toggle('stop', busy);
      const model = providers.flatMap((p) => p.models).find((m) => m.id === (session?.model || settings?.model))?.name || settings?.model || '';
      used.textContent = [model, u?.steps ? `${amount(u.input)} tokens read, ${amount(u.output)} written` : ''].filter(Boolean).join(' · ');
      keys.fresh.disabled = !session || (!session.agent.messages.length && !busy);
    }

    function draw() {
      keys.settings.classList.toggle('on', shows === 'settings');
      keys.settings.textContent = shows === 'settings' ? 'Done' : 'Settings';
      clear(body);
      clear(foot);
      if (!settings) { body.append(h('p', { class: problem ? 'ag-wrong' : 'ag-tip' }, problem || 'Asking the server…')); return; }
      if (shows === 'settings') { body.append(h('div', { class: 'ag-settings' }, settingsView())); return; }
      const provider = providers.find((p) => p.id === settings.provider);
      if (!session.log.childNodes.length) {
        body.append(h('div', { class: 'ag-welcome' },
          h('p', {}, 'Say what you want on the board, and the agent sets it out: notes and regions, links, sketches, tables, plans, and what the plug-ins you allow can do.'),
          provider?.unavailable ? h('p', { class: 'ag-wrong' }, `It cannot answer yet: ${provider.unavailable}.`) : null,
          !settings.keys?.[settings.provider]?.kept ? h('p', { class: 'ag-tip' }, 'No key is kept yet. ', h('button', { class: 'ag-link', type: 'button', onclick: () => { shows = 'settings'; draw(); } }, 'Set one in the settings'), ', or the server\'s own is tried.') : null,
          h('div', { class: 'ag-suggest' }, SUGGESTIONS.map((words) => h('button', { type: 'button', onclick: () => { input.value = words; input.focus(); } }, words)))));
      }
      body.append(session.log);
      foot.append(h('div', { class: 'ag-compose' }, input, sendKey), used);
      drawFoot();
      toEnd();
    }

    // Another board is opened: the conversation was about the one before.
    const another = () => { if (session && session.boardId !== api.board.id) { fresh(); if (shows === 'talk') draw(); } };
    const offs = [api.on('change', another), api.on('select', another)];

    fresh();
    draw();
    (async () => {
      try {
        const [kept, who] = await Promise.all([request('settings'), request('providers')]);
        settings = kept; providers = who.providers; efforts = who.efforts;
      } catch (error) { problem = `The agent's server did not answer: ${error.message}.`; }
      draw();
    })();

    return {
      shown() { if (shows === 'talk') input.focus(); },
      release() { for (const off of offs) off(); session.over = true; session.agent.stop(); },
      // For the checks that drive the page.
      ask, get session() { return session; }, get settings() { return settings; },
    };
  }

  return {
    id: 'ai-agent', name: 'Agent',
    about: 'An agent that works on the board from what you say, with the board\'s tools and the plug-ins you allow. It is opened from the bar above.',
    templates: [],
    panels: [{ id: 'agent', label: 'Agent', title: 'Say what you want on the board', mount }],
  };
}
