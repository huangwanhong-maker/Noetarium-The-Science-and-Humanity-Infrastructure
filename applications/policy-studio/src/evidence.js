// The evidence (ADR 0122): for each causal claim, its body of studies. A body is drawn as a forest: each study with its interval
// and its weight, the pooled effect beneath them, and the interval within which a new setting's effect is predicted to lie. Beside
// it stands the ladder of its grade under the declared scheme: where its design starts, and each step down or up with its reason,
// the steps a rule read off the synthesis told apart from those a reader assessed. A study chosen opens beneath the forest, with
// what the pool would be without it. Studies are entered one by one or pasted as a table.

import { svgForest, pips, figure, writeTheory, setLevels } from '/visualization/js/policy/index.js';
import { S, ensure, keep, begun, theory, levels, nodeText, bodyAnswer, isRatio, MEASURES, interval, effect, percent, words, numberIn, designs } from './common.js';

export function evidenceView() {
  return { id: 'evidence', label: 'Evidence', draw };
}

const COUNTS = ['treated_events', 'treated_others', 'control_events', 'control_others'];
const levelsOf = (schemeText) => (String(schemeText).split('\n').find((l) => l.trim().split(/\s+/)[0] === 'levels') || '').trim().split(/\s+/).slice(1);

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('evidence-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the evidence');
  const q = S.question, redraw = () => ctx.show('evidence');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin with the theory: the evidence is kept claim by claim.'));
  if (S.wrong) return fill(pane, el('p', { class: 'note wrong' }, S.wrong));
  const change = async (f, what, note) => { await keep(ctx, f, what, note); redraw(); };
  const t = theory(), lv = levels(), n = lv.length;
  const bodies = q.bodies || [];
  if (!bodies.some((b) => b.id === S.body)) S.body = bodies[0]?.id || null;
  ctx.note(`${bodies.length} ${bodies.length === 1 ? 'body' : 'bodies'} of evidence${S.appraisal?.scheme?.name ? `, graded by ${words(S.appraisal.scheme.name)}` : ''}`);

  const newBody = () => {
    let name = 'evidence';
    for (let k = 2; bodies.some((b) => b.id === name); k++) name = `evidence-${k}`;
    S.body = name; S.study = null;
    change((x) => { (x.bodies ||= []).push({ id: name, link: {}, claim: '', design: designs().includes('randomised') ? 'randomised' : designs()[0] || 'randomised', measure: 'as_given', higher_is_better: true, confidence: 0.95, assessed: {}, studies: [] }); }, 'began a body of evidence', name);
  };
  const shelf = el('nav', { class: 'bodies', 'aria-label': 'The bodies of evidence' },
    bodies.map((b) => {
      const a = bodyAnswer(b.id), rank = a && !a.error ? lv.indexOf(a.grade) : -1;
      return el('button', { type: 'button', class: `body-slip${b.id === S.body ? ' on' : ''}${a?.error ? ' failed' : ''}`, onclick: () => { S.body = b.id; S.study = null; redraw(); } },
        el('b', {}, b.claim || b.id),
        el('span', { class: 'on-link' }, b.link?.from ? `${nodeText(b.link.from)} → ${nodeText(b.link.to)}` : 'the whole intervention, on no single claim'),
        a?.error ? el('span', { class: 'grade unshown' }, '✕ it cannot be pooled') : el('span', { class: 'grade' }, el('span', { class: 'pips' }, pips(rank, n)), ` ${words(a?.grade || '')} · ${(b.studies || []).length} ${(b.studies || []).length === 1 ? 'study' : 'studies'}`));
    }),
    el('button', { type: 'button', class: 'body-slip add', onclick: newBody }, el('b', {}, '+ a body of evidence'), el('span', { class: 'on-link' }, 'studies of one design and one measure, for one claim')));

  const scheme = S.appraisal?.scheme;
  const schemeLine = el('p', { class: 'scheme-line' }, scheme?.error ? el('span', { class: 'wrong' }, `The grading scheme does not read: ${scheme.error}. `) : ['Graded by ', el('b', {}, words(scheme?.name || 'no scheme')), `: ${lv.map(words).join(' < ')}. `],
    S.schemes.length ? el('select', { class: 'adder', 'aria-label': 'Grade by another scheme', onchange: (e) => {
      const s = S.schemes.find((z) => z.id === e.target.value);
      if (!s) return;
      const next = levelsOf(s.text), moved = setLevels(S.read.theory, next);
      change((x) => {
        x.scheme = s.text; x.rules = s.rules; x.theory = writeTheory(moved.theory);
        if (!next.includes(x.least)) x.least = next[0] || '';
        for (const b of x.bodies || []) { b.assessed = {}; delete b.rules; }
      }, 'graded by another scheme', `${s.title}${moved.dropped.length ? `; evidence written on ${moved.dropped.join(', ')} was in a level it lacks and is dropped` : ''}`);
    } }, el('option', { value: '' }, 'grade by another scheme…'), S.schemes.map((s) => el('option', { value: s.id, title: s.about }, s.title))) : null);

  const b = bodies.find((x) => x.id === S.body);
  if (!b) return fill(pane, shelf, schemeLine, el('p', { class: 'note' }, 'No body of evidence yet. A claim without studies stands as not shown.'));
  const a = bodyAnswer(b.id), ratio = isRatio(b.measure), counts = b.measure !== 'as_given';
  const set = (f, what, note) => change((x) => { const y = x.bodies.find((z) => z.id === b.id); if (y) f(y, x); }, what, note ?? b.id);

  // The head: what the studies are evidence of, on which claim, of what design and measure.
  const linkValue = b.link?.from ? `${b.link.from} -> ${b.link.to}` : '';
  const head = el('header', { class: 'body-head' },
    el('input', { class: 'q-title', type: 'text', value: b.claim || '', placeholder: 'What the studies are evidence of', 'aria-label': 'The claim the studies are evidence of', spellcheck: true,
      onchange: (e) => set((y) => { y.claim = e.target.value.trim(); }, 'restated what a body is evidence of') }),
    el('p', { class: 'sentence small-sentence' }, 'Set on ',
      el('select', { class: 'q-pick', 'aria-label': 'The claim the body is set on', onchange: (e) => set((y) => { const [from, to] = e.target.value ? e.target.value.split(' -> ') : []; y.link = from ? { from, to } : {}; }, 'set a body on a claim', `${b.id}: ${e.target.value || 'no claim'}`) },
        el('option', { value: '', selected: !linkValue }, 'the whole intervention (no single claim)'),
        (t?.links || []).map((l) => el('option', { value: `${l.from} -> ${l.to}`, selected: linkValue === `${l.from} -> ${l.to}` }, `${nodeText(l.from)} → ${nodeText(l.to)}`))),
      '; ', el('select', { class: 'q-pick', 'aria-label': 'The design of the studies', onchange: (e) => set((y) => { y.design = e.target.value; }, 'changed a body\'s design', `${b.id}: ${e.target.value}`) },
        [...new Set([...designs(), b.design])].filter(Boolean).map((d) => el('option', { value: d, selected: d === b.design }, words(d)))), ' evidence; each study gives ',
      el('select', { class: 'q-pick', 'aria-label': 'What each study gives', onchange: (e) => set((y) => { y.measure = e.target.value; }, 'changed what a body\'s studies give', `${b.id}: ${e.target.value}`) },
        Object.entries(MEASURES).map(([k, w]) => el('option', { value: k, selected: k === b.measure }, w))), '; ',
      el('select', { class: 'q-pick', 'aria-label': 'The side of benefit', onchange: (e) => set((y) => { y.higher_is_better = e.target.value === 'more'; }, 'changed the side of benefit', `${b.id}: ${e.target.value} is better`) },
        el('option', { value: 'more', selected: b.higher_is_better !== false }, 'more is better'), el('option', { value: 'less', selected: b.higher_is_better === false }, 'less is better')), '.'),
    b.source ? el('p', { class: 'small source' }, b.source) : null);

  const studies = studiesTable(ctx, b, set, redraw);
  if (!a || a.error) {
    return fill(pane, shelf, schemeLine, el('article', { class: 'sheet' }, head,
      el('p', { class: 'note wrong' }, (b.studies || []).length ? (a?.error || 'The body could not be assessed.').replace(/^(policy|tekmerion|peira)::[a-z_:]+: /, '') : 'No study yet: enter one, or paste a table.'), studies, removeKey(ctx, b, change)));
  }

  // The forest, and what it says.
  const forest = el('div', { class: 'forest-wrap' });
  forest.innerHTML = svgForest(a, { ratio, selected: S.study, plotW: Math.max(220, Math.min(380, pane.clientWidth - 46 - 300 - 420)) });
  forest.addEventListener('click', (e) => { const g = e.target.closest('[data-study]'); S.study = g && g.dataset.study !== S.study ? g.dataset.study : null; redraw(); });
  const p = a.pooled;
  const said = el('p', { class: 'said' }, `${p.k} ${p.k === 1 ? 'study, as it stands' : `studies pooled by ${p.model}`}: `, el('b', {}, interval(p.estimate, p.lower, p.upper, ratio)), ratio ? ' as a ratio' : '',
    p.k > 1 ? `. The studies differ beyond chance by I² = ${percent(p.i2)} (τ² = ${figure(p.tau2)})` : '',
    a.prediction ? `; in a new setting the effect is predicted between ${effect(a.prediction.lower, ratio)} and ${effect(a.prediction.upper, ratio)}` : '',
    a.egger ? `; Egger's test for small-study effects has p = ${figure(a.egger.p_value)}` : '', '.');

  // A study chosen: its numbers, and the pool without it.
  let chosen = null;
  const s = (b.studies || []).find((x) => String(x.id) === String(S.study));
  if (s) {
    const without = (a.leave_one_out || []).find((o) => String(o.omitted) === String(s.id));
    const field = (label, key, attrs = {}) => el('label', {}, el('span', {}, label), numberIn(el, s[key], (v) => set((y) => { const z = y.studies.find((w) => String(w.id) === String(s.id)); if (v === null) delete z[key]; else z[key] = v; }, 'changed a study', `${b.id}: ${s.label || s.id}`), attrs));
    chosen = el('section', { class: 'study-card' },
      el('button', { type: 'button', class: 'close', 'aria-label': 'Close', onclick: () => { S.study = null; redraw(); } }, '×'),
      el('input', { class: 'study-name', type: 'text', value: s.label || '', placeholder: 'The study', 'aria-label': 'The study\'s name', onchange: (e) => set((y) => { y.studies.find((w) => String(w.id) === String(s.id)).label = e.target.value.trim(); }, 'renamed a study', `${b.id}: ${e.target.value.trim()}`) }),
      el('div', { class: 'fields' }, field('year', 'year', { step: 1 }),
        counts ? [field('events, treated', 'treated_events', { min: 0 }), field('others, treated', 'treated_others', { min: 0 }), field('events, controls', 'control_events', { min: 0 }), field('others, controls', 'control_others', { min: 0 })]
          : [field('effect', 'effect'), s.se !== undefined || s.variance === undefined ? field('standard error', 'se', { min: 0 }) : field('variance', 'variance', { min: 0 })],
        el('label', {}, el('span', {}, 'stratum'), el('input', { type: 'text', value: s.stratum || '', 'aria-label': 'The stratum of the study', onchange: (e) => set((y) => { y.studies.find((w) => String(w.id) === String(s.id)).stratum = e.target.value.trim(); }, 'placed a study in a stratum', `${b.id}: ${s.label || s.id}`) }))),
      without ? el('p', { class: 'small' }, 'Without it the others pool to ', el('b', {}, interval(without.estimate, without.lower, without.upper, ratio)), `, against ${effect(p.estimate, ratio)} with it.`) : null,
      el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key', onclick: () => { S.study = null; set((y) => { y.studies = y.studies.filter((w) => String(w.id) !== String(s.id)); }, 'removed a study', `${b.id}: ${s.label || s.id}`); } }, 'Remove the study')));
  }

  // The ladder of the grade.
  const start = scheme?.start?.[b.design], final = a.grade;
  const rules = Array.isArray(b.rules) ? b.rules : q.rules || [];
  const readByRule = new Set(rules.map((r) => r.factor));
  const ladder = el('ol', { class: 'ladder', 'aria-label': 'The levels of the scheme, highest first' }, [...lv].reverse().map((l, k) => {
    const rank = n - 1 - k;
    return el('li', { class: `${l === final ? 'final' : ''}${l === start ? ' start' : ''}` }, el('span', { class: 'pips' }, pips(rank, n)), el('span', { class: 'level' }, words(l)),
      l === final ? el('b', { class: 'tag' }, 'its grade') : l === start ? el('span', { class: 'tag' }, 'where it starts') : null);
  }));
  const steps = el('ol', { class: 'steps' }, (a.steps || []).map((w) => {
    const factor = w.split(/[: ]/)[0];
    const moves = / lowers it | raises it /.test(w);
    return el('li', { class: `${moves ? 'moves' : ''}${/ lowers it /.test(w) ? ' down' : / raises it /.test(w) ? ' up' : ''}` }, words(w),
      readByRule.has(factor) && !moves ? el('span', { class: 'by' }, ' — read off the synthesis by a rule') : !moves || readByRule.has(factor) ? null : b.assessed?.[factor] ? el('span', { class: 'by' }, ' — assessed by a reader') : null);
  }));
  const readers = (scheme?.factors || []).filter((f) => !readByRule.has(f.name));
  const assess = el('div', { class: 'assess' }, el('h4', {}, 'What a reader assesses'),
    readers.map((f) => el('label', { class: 'factor' },
      el('span', { class: 'name' }, `${words(f.name)} `, el('span', { class: 'small' }, f.direction < 0 ? 'lowers' : 'raises')),
      el('select', { 'aria-label': `Steps for ${words(f.name)}`, onchange: (e) => set((y) => { y.assessed ||= {}; const v = Number(e.target.value); if (v) y.assessed[f.name] = v; else { delete y.assessed[f.name]; if (y.assessed_why) delete y.assessed_why[f.name]; } }, 'assessed a factor', `${b.id}: ${words(f.name)} ${e.target.value} ${e.target.value === '1' ? 'step' : 'steps'}`) },
        Array.from({ length: f.most + 1 }, (_, k) => el('option', { value: k, selected: (b.assessed?.[f.name] || 0) === k }, k === 0 ? 'no step' : `${k} ${k === 1 ? 'step' : 'steps'}`))),
      b.assessed?.[f.name] ? el('input', { type: 'text', class: 'why', value: b.assessed_why?.[f.name] || '', placeholder: 'why, in a sentence', 'aria-label': `Why ${words(f.name)} moves it`,
        onchange: (e) => set((y) => { (y.assessed_why ||= {})[f.name] = e.target.value.trim(); }, 'gave a reason for a factor', `${b.id}: ${words(f.name)}`) }) : null)),
    readers.length ? null : el('p', { class: 'small' }, 'Every factor of the scheme is read by a rule.'),
    el('p', { class: 'small' }, `Read off the synthesis by declared rules: ${rules.map((r) => `${words(r.factor)} (${ruleWords(r)})`).join('; ') || 'nothing'}.`));

  const strata = (a.strata || []).length ? el('table', { class: 'strata' }, el('caption', {}, 'The strata the studies name, each pooled'),
    el('thead', {}, el('tr', {}, ['Stratum', 'Studies', ratio ? 'Ratio [interval]' : 'Effect [interval]', 'I²'].map((h) => el('th', {}, h)))),
    el('tbody', {}, a.strata.map((z) => el('tr', {}, el('td', {}, z.name), el('td', { class: 'n' }, z.k), el('td', { class: 'n' }, interval(z.effect, z.lower, z.upper, ratio)), el('td', { class: 'n' }, z.k > 1 ? percent(z.i2) : '—'))))) : null;

  fill(pane, shelf, schemeLine, el('article', { class: 'sheet' }, head,
    el('div', { class: 'two' },
      el('section', { class: 'forest-side' }, forest, said, chosen, strata),
      el('section', { class: 'grade-side' }, el('h4', {}, 'Its grade, step by step'), ladder, steps, assess)),
    studies, removeKey(ctx, b, change)));
}

const ruleWords = (r) => ({
  crosses: `a step when the interval contains ${r.values.join(' or ')}`,
  predicted: `a step when the prediction interval contains ${r.values.join(' or ')}`,
  heterogeneous: `a step for each of I² above ${r.values.map((v) => percent(v)).join(', ')}`,
  small_studies: `a step when Egger's p is below ${r.values.join(', ')}${r.least ? `, with ${r.least} studies or more` : ''}`,
  large: `a step for each magnitude beyond ${r.values.join(', ')} on the side of benefit`,
}[r.reading] || r.reading);

const removeKey = (ctx, b, change) => ctx.el('div', { class: 'keys end' }, ctx.el('button', { type: 'button', class: 'key', onclick: () => { S.body = null; S.study = null; change((x) => { x.bodies = x.bodies.filter((z) => z.id !== b.id); if (x.place?.body === b.id) delete x.place.body; }, 'removed a body of evidence', b.id); } }, 'Remove this body of evidence'));

// The studies as a table a person can paste over.
function studiesTable(ctx, b, set, redraw) {
  const { el } = ctx;
  const counts = b.measure !== 'as_given';
  const cols = ['id', 'label', 'year', 'stratum', ...(counts ? COUNTS : ['effect', 'se'])];
  const cell = (v) => (v === undefined || v === null ? '' : /[",\n]/.test(String(v)) ? `"${String(v).replace(/"/g, '""')}"` : String(v));
  const rows = (b.studies || []).map((s) => cols.map((c) => cell(c === 'se' ? (s.se ?? (Number.isFinite(s.variance) ? +Math.sqrt(s.variance).toPrecision(6) : '')) : s[c])).join(','));
  const area = el('textarea', { class: 'csv', rows: Math.min(16, Math.max(4, rows.length + 2)), spellcheck: false, 'aria-label': 'The studies as a table' }, [cols.join(','), ...rows].join('\n'));
  const read = async () => {
    try {
      const a = await ctx.api('/api/studies', { method: 'POST', body: { csv: area.value } });
      const given = a.studies.some((s) => s.effect !== undefined), four = a.studies.some((s) => s.treated_events !== undefined);
      S.study = null;
      set((y) => { y.studies = a.studies; if (four && y.measure === 'as_given') y.measure = 'log_risk_ratio'; if (given && !four) y.measure = 'as_given'; }, 'read a table of studies', `${b.id}: ${a.studies.length} studies`);
    } catch (error) { ctx.say(error.message); }
  };
  const add = () => {
    let id = (b.studies || []).length + 1;
    while ((b.studies || []).some((s) => String(s.id) === String(id))) id++;
    S.study = String(id);
    set((y) => { (y.studies ||= []).push(counts ? { id: String(id), label: `Study ${id}`, treated_events: 1, treated_others: 99, control_events: 2, control_others: 98 } : { id: String(id), label: `Study ${id}`, effect: 0, se: 1 }); }, 'entered a study', `${b.id}: study ${id} (its numbers are placeholders until they are set)`);
  };
  return el('details', { class: 'studies', open: !(b.studies || []).length },
    el('summary', {}, `The studies as a table (${(b.studies || []).length})`),
    el('p', { class: 'small' }, counts ? 'A row a study: the events and the others among the treated and among the controls. A stratum, where named, lets the effect be carried to a place of another mix.'
      : 'A row a study: its effect and its standard error (or a column named variance). A stratum, where named, lets the effect be carried to a place of another mix.'),
    area, el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key edged', onclick: read }, 'Read the table'), el('button', { type: 'button', class: 'key', onclick: add }, '+ a study')));
}
