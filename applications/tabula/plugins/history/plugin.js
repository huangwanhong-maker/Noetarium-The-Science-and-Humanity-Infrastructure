// History, as a plug-in of the board: the events of a history set on a line of time. Its module
// (modules/canon.py) reads the occasions of one of the history domain's canons; its card draws any
// table of events (a date, perhaps an end, what happened, perhaps a kind) as a chronicle, each kind in
// a colour of its own, on the board's own layout of time (schedule.js).
//
// Caveat. A canon writes each date as its source writes it, in the calendar it names (Julian before
// the reform, Old Style in England until 1752); the line of time here is the proleptic Gregorian
// calendar's. The two differ by ten to thirteen days, which a line of years does not show. The history
// domain converts dates exactly; this card does not.

export default function history(B) {
  const { h, note, text } = B;

  const events = {
    label: 'Events', about: 'A table of events on a line of time: a date, perhaps an end, what happened, each kind in a colour of its own.', size: { w: 980, h: 320 }, height: 'grow',
    props: { table: '', when: 'when', until: 'until', label: 'label', group: 'kind', most: 40, title: '' },
    form: () => [
      { name: 'table', label: 'Table', kind: 'text', role: 'table' },
      { name: 'when', label: 'When', kind: 'text', role: 'column', of: 'table', help: 'a date: 1687, 1687-07 or 1687-07-05; a year before 1 as -0479' },
      { name: 'until', label: 'Until', kind: 'text', role: 'column', of: 'table' },
      { name: 'label', label: 'What', kind: 'text', role: 'column', of: 'table' },
      { name: 'group', label: 'Coloured by', kind: 'text', role: 'column', of: 'table' },
      { name: 'most', label: 'At most', kind: 'integer', min: 5, max: 400, help: 'the first in time' },
      { name: 'title', label: 'Title', kind: 'text' },
    ],
    signature: (ctx) => B.tableStamp(ctx.tables.get(ctx.props.table)),
    render(body, ctx) {
      const p = ctx.props, t = ctx.tables.get(p.table);
      if (!t || t.error) { body.append(h('div', { class: 'bd-hint' }, t?.error || (p.table ? `There is no table '${p.table}' on the board: run the module that answers it.` : 'Choose a table in the panel.'))); return; }
      if (!t.data[p.when]) { body.append(h('div', { class: 'bd-hint' }, `The table '${t.name}' has no column '${p.when}' of dates.`)); return; }
      const short = (words) => (String(words).length > 40 ? `${String(words).slice(0, 39)}…` : String(words));
      const tasks = [], unread = [];
      for (let i = 0; i < t.rows; i++) {
        const from = B.parseDate(String(t.data[p.when][i] ?? ''));
        if (!from) { if (t.data[p.when][i] !== null) unread.push(t.data[p.when][i]); continue; }
        const to = t.data[p.until]?.[i] ? B.parseDate(String(t.data[p.until][i])) : null;
        const full = String(t.data[p.label]?.[i] ?? '');
        tasks.push({ name: short(full), full, start: from.day, end: to && to.next > from.day ? to.next : from.day, precision: from.precision, group: t.data[p.group]?.[i] ?? null, row: i });
      }
      if (!tasks.length) { body.append(h('div', { class: 'bd-hint' }, 'None of its dates could be read: a date is written 1687, 1687-07 or 1687-07-05.')); return; }
      tasks.sort((a, b) => a.start - b.start);
      const shown = tasks.slice(0, Math.max(5, Number(p.most) || 40)).map((task, index) => ({ ...task, index }));
      const span = { start: Math.min(...shown.map((e) => e.start)), end: Math.max(...shown.map((e) => e.end)) };
      const layout = B.layoutChronicle({ tasks: shown, span }, { width: Math.max(260, ctx.width - 28) });
      const svg = B.drawChronicle(layout);
      // Each kind in a colour of its own, in the order the kinds first appear.
      const kinds = [...new Set(shown.map((e) => e.group).filter((g) => g !== null && g !== ''))];
      const hues = ['var(--tb-series-1)', 'var(--tb-series-2)', 'var(--tb-series-3)', 'var(--tb-series-4)', 'var(--tb-series-5)', 'var(--tb-series-6)'];
      const colour = (g) => (kinds.includes(g) ? hues[kinds.indexOf(g) % hues.length] : null);
      const marks = svg.querySelectorAll('circle.bd-event, rect.bd-bar');
      layout.events.forEach((e, k) => {
        const c = colour(e.task.group);
        if (c && marks[k]) { marks[k].style.fill = c; marks[k].style.stroke = 'var(--bd-card)'; }
        const said = marks[k]?.querySelector('title');
        if (said) said.textContent = `${e.task.full} · ${t.data[p.when][e.task.row]}${e.task.group ? ` · ${e.task.group}` : ''}`;
      });
      body.append(h('div', { class: 'bd-module-title' }, p.title || t.title || t.name), svg);
      if (kinds.length) body.append(h('div', { class: 'bd-map-key' }, kinds.slice(0, 12).map((g) => h('span', { class: 'kind' }, h('i', { style: `background:${colour(g)}` }), g)),
        kinds.length > 6 ? h('span', { class: 'bd-map-key-note' }, 'more than six kinds: colours are used again') : null));
      body.append(h('div', { class: 'bd-map-note' }, `${shown.length} of ${tasks.length} events${unread.length ? `; ${unread.length} dates could not be read` : ''}${t.source ? ` · ${t.source}` : ''}`));
    },
  };

  const chronicle = {
    id: 'canon', name: 'The events of a history', about: 'The occasions of one of the history domain\'s canons on a line of time, each kind in a colour of its own. Choose the canon in the module\'s panel and run it.',
    items: [
      text('What happened, and when?', 0, 0),
      { key: 'canon', kind: 'card', card: 'module', props: { module: 'history/canon.py', params: { canon: 'science-1543-1727' } }, x: 0, y: 70, w: 400, h: 260 },
      { key: 'line', kind: 'card', card: 'events', props: { table: 'events', when: 'when', until: 'until', label: 'label', group: 'kind', most: 40 }, x: 450, y: 70, w: 1200, h: 420 },
      note('question', 'What is missing from this record, and who chose what is in it?', 0, 520, {}),
    ],
    links: [{ from: 'canon', to: 'line' }],
  };

  return {
    id: 'history', name: 'History', about: 'Events on a line of time, from the history domain\'s records or from a table of one\'s own.',
    noteTypes: [], templates: [chronicle], cards: { events },
  };
}
