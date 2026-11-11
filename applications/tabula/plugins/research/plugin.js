// Research, as a plug-in of the board: a second subject on the same ground, to show that the board is
// of none. A hypothesis is a kind of note; evidence is tied to it by links that say whether it
// supports or contradicts; a study is planned on a line of time.

export default function research({ region, note, text }) {
  const noteTypes = [
    { id: 'hypothesis', label: 'Hypothesis', glyph: '∴', tone: 'lilac', about: 'A claim that evidence could count against.' },
    { id: 'method', label: 'Method', glyph: '⚙', tone: 'sky', about: 'A way of finding out.' },
  ];

  const evidence = {
    id: 'hypotheses', name: 'Hypotheses and evidence', about: 'Each claim with what speaks for it and against it. Select a link to say which.',
    items: [
      text('What do we claim, and what speaks to it?', 0, 0, { w: 640 }),
      note('hypothesis', 'First hypothesis', 380, 90, { key: 'h1', w: 240 }),
      note('hypothesis', 'A rival hypothesis', 380, 420, { key: 'h2', w: 240 }),
      note('fact', 'An observation', 0, 60, { key: 'e1', source: 'where from?' }),
      note('fact', 'Another', 0, 250, { key: 'e2', source: 'where from?' }),
      note('fact', 'A third', 0, 440, { key: 'e3', source: 'where from?' }),
      note('method', 'What would tell them apart?', 760, 250, { key: 'm' }),
    ],
    links: [
      { from: 'e1', to: 'h1', rel: 'supports' }, { from: 'e2', to: 'h1', rel: 'contradicts' }, { from: 'e2', to: 'h2', rel: 'supports' }, { from: 'e3', to: 'h2', rel: 'supports' },
      { from: 'h1', to: 'm' }, { from: 'h2', to: 'm' },
    ],
  };

  const reading = {
    id: 'reading', name: 'Reading map', about: 'What has been read, what it claims, and what is still to read.',
    items: [
      region('claims', 'What they claim', 0, 0, 620, 560, { tone: 'lilac', hint: 'A note a claim, each with its source.' }),
      region('open', 'What none of them settles', 650, 0, 420, 270, { tone: 'rose' }),
      region('next', 'Still to read', 650, 290, 420, 270, {}),
      note('fact', 'A claim', 20, 60, { in: 'claims', source: 'author, year' }),
      note('question', '', 670, 60, { in: 'open' }),
    ],
  };

  const study = {
    id: 'study', name: 'A study planned', about: 'The steps of a study on a line of time.',
    items: [{
      kind: 'schedule', mode: 'plan', title: 'A study', x: 0, y: 0, w: 860, h: 260,
      source: ['Question and design : 2026-11-16, 2w', 'Register the design : after Question and design, milestone', 'Collect : after Register the design, 6w',
        'Analyse : after Collect, 3w', 'Write : after Analyse, 4w'].join('\n'),
    }],
  };

  return { id: 'research', name: 'Research', about: 'Claims, evidence and the plan of a study.', noteTypes, templates: [evidence, reading, study], cards: {} };
}
