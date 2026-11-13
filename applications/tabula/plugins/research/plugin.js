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

  // The handbook "Material and Ecological Conditions of Life" (§7.2, §12.2, §16): forms to think with, each field a region with its hint.
  const grid = (fields, cols = 3, w = 330, h = 210, gap = 20) => fields.map(([key, title, hint, tone], k) =>
    region(key, title, (k % cols) * (w + gap), 60 + Math.floor(k / cols) * (h + gap), w, h, { tone: tone || '', hint }));

  const question = {
    id: 'research-question', name: 'A research question', about: 'The question bounded before any tool is chosen (the handbook, §16.1).',
    items: [
      text('The question, before the tools', 0, 0, { w: 640 }),
      ...grid([
        ['object', 'Object of inquiry', 'What is studied: a flow, an interaction, a service, a lived experience, a decision?', 'lilac'],
        ['affected', 'Participants and affected systems', 'Who and what the answer touches, people and not only people.'],
        ['bounds', 'Boundaries in space and time', 'The scale: person to planet, seconds to decades.'],
        ['condition', 'The condition of life studied', 'What is the case now, and for whom.'],
        ['dependencies', 'Material and ecological dependencies', 'Resources, infrastructures, ecosystems it rests on.'],
        ['observed', 'What is observed directly', 'Measurements, records, field evidence.', 'sky'],
        ['inferred', 'What must be inferred or modelled', 'And by which model, with which assumptions.'],
        ['rivals', 'Alternative explanations', 'Structural uncertainties: what else could produce the same.', 'rose'],
        ['normative', 'Normative and rights considerations', 'Thresholds and rights that no score can trade.', 'rose'],
        ['constraints', 'Data and access constraints', 'Licences, privacy, consent, what cannot be had.'],
        ['design', 'The mixed-method design', 'Exploratory, explanatory, convergent, longitudinal, participatory?'],
        ['falsify', 'Validation and falsification', 'What result would count against the answer.', 'sky'],
      ]),
    ],
  };

  const tool = {
    id: 'tool-weighed', name: 'A tool weighed', about: 'A method or piece of software weighed before it is adopted (the handbook, §16.2).',
    items: [
      text('A tool, weighed', 0, 0, { w: 640 }),
      ...grid([
        ['name', 'Name and where it is kept', 'Its official site, version and date checked.', 'lilac'],
        ['class', 'Its class', 'Measurement, accounting, model, qualitative, assessment, foresight.'],
        ['answers', 'The questions it answers', 'And those it does not.'],
        ['inputs', 'Inputs, standards and units', ''],
        ['outputs', 'Outputs and their scales', 'In space and in time.'],
        ['assumes', 'Its core assumptions', '', 'rose'],
        ['validated', 'Validation and calibration', 'Against what, and how well.', 'sky'],
        ['cost', 'Licence, data charges, computing', ''],
        ['implications', 'Privacy, ecological, institutional implications', '', 'rose'],
        ['limits', 'Known limitations', ''],
        ['decision', 'Decision', 'Adopt, pilot, defer or reject, and why.', 'lilac'],
      ]),
    ],
  };

  const protocol = {
    id: 'study-protocol', name: 'A study protocol', about: 'The checks a study passes before its results are trusted (the handbook, §16.3).',
    items: [
      text('Before the results are trusted', 0, 0, { w: 640 }),
      ...['The question is set before the tools are chosen.', 'The system\'s boundaries and accounting units are explicit.', 'Measurements have a justified sampling and calibration.',
        'Economic, ecological and social measures are kept apart.', 'Where every dataset comes from, and its licence, is recorded.', 'Each model\'s assumptions, version and uncertainty are documented.',
        'Qualitative and historical evidence can contradict the model.', 'Unequal access, vulnerability and costs put on others are assessed.', 'A counterfactual claim has a design, or is labelled exploratory.',
        'Observation, inference, scenario and judgement are kept apart in the results.', 'Rights, consent and environmental obligations are considered.', 'Results and code can be reproduced, or their limits are said.']
        .map((q, k) => note('question', q, (k % 3) * 320, 60 + Math.floor(k / 3) * 150, { w: 290 })),
    ],
  };

  const uncertainty = {
    id: 'uncertainty-register', name: 'An uncertainty register', about: 'What is not known, by kind (the handbook, §7.2).',
    items: [
      text('What is not known, and of which kind', 0, 0, { w: 640 }),
      ...grid([
        ['measurement', 'Measurement', 'Sensor error, sampling variability, missing records.', 'sky'],
        ['parameter', 'Parameters', 'Efficiencies, rates, behavioural coefficients: ranges, and what they were set from.'],
        ['structural', 'Structure', 'Competing mechanisms: which model, and what a rival would change.', 'rose'],
        ['scenario', 'Scenarios', 'Different technologies, institutions, demographies.'],
        ['normative', 'Values', 'Disagreement about what matters, and how much.', 'rose'],
        ['done', 'What was done about each', 'Monte Carlo, sensitivity, out-of-sample tests, ensembles, stress tests.', 'lilac'],
      ], 3, 340, 240),
    ],
  };

  const models = {
    id: 'model-register', name: 'A model register', about: 'Each model in a study, and what it cannot answer (the handbook, §12.2).',
    items: [
      text('A model, registered', 0, 0, { w: 640 }),
      ...grid([
        ['model', 'Name, purpose, maintainer', 'The question it serves.', 'lilac'],
        ['version', 'Software version and licence', ''],
        ['boundary', 'Boundary in space, time and population', ''],
        ['data', 'Inputs, units, sources, missingness', 'And their privacy conditions.'],
        ['form', 'Variables, assumptions, equations', ''],
        ['evidence', 'Calibration and validation', '', 'sky'],
        ['outputs', 'Outputs, sensitivities, failures', 'Where it breaks.'],
        ['interfaces', 'Interfaces with other models', 'Units, time steps, sectors: what is translated, and how.'],
        ['cannot', 'What it cannot answer', '', 'rose'],
      ]),
    ],
  };

  return { id: 'research', name: 'Research', about: 'Claims, evidence and the plan of a study; the handbook\'s forms: a research question, a tool weighed, a study protocol, an uncertainty register, a model register.', noteTypes,
    templates: [evidence, reading, study, question, tool, protocol, uncertainty, models], cards: {} };
}
