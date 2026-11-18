// Needs (ADR 0125): the search for needs as the page. The field of statements is the stage: every statement the search has made, set
// by the people it concerns and the size of its gap on the areas the search may see; the needs it keeps drawn by their domain, their
// standard and their standing, the rings of a confirmed need double. A statement opens where it stands, with its tests in words, the
// argument that decides its standing, and the line of operators that made it drawn back through the field. Beside it, the loop of the
// search, each stage a place to choose its strategy; the areas, coloured by the part of the data each lies in or by where a need lies;
// beneath, the ledger of confirmations against the online rule's levels, and the generations.

import { INKS } from '/visualization/js/dynamics/charts.js';

const enc = encodeURIComponent;
const N = {
  project: null, study: null, cases: [], place: null, report: null, run: null, proposals: [], model: null,
  sel: null, opened: null, mapped: null, gen: null, compose: false, written: '', read: null, tested: null, asked: null, lookfor: '', desc: null,
  queue: [], modelRounds: false, busy: '', wrong: '', poll: null, layer: 'kept',
};
const base = (ctx) => `/api/projects/${enc(ctx.name)}/needs`;
const fig = (x, d = 0) => (x == null || !Number.isFinite(x) ? '–' : Number(x).toLocaleString('en-GB', { maximumFractionDigits: d, minimumFractionDigits: d }));
const pct = (x, d = 1) => (x == null || !Number.isFinite(x) ? '–' : `${(100 * x).toFixed(d)}%`);
const sig = (x) => (x == null || !Number.isFinite(x) ? '–' : Math.abs(x) >= 0.01 ? x.toFixed(3) : x.toExponential(1));

const STAGES = [
  { id: 'generate', label: 'Generate', about: 'how new statements enter' },
  { id: 'evaluate', label: 'Evaluate', about: 'what a statement is judged by, on the areas the search sees' },
  { id: 'select', label: 'Select', about: 'how parents are chosen' },
  { id: 'vary', label: 'Vary', about: 'the operators, and how they are chosen and credited' },
  { id: 'survive', label: 'Survive', about: 'who stays in the population' },
  { id: 'admit', label: 'Admit', about: 'what the archive keeps, and when it retires' },
  { id: 'confirm', label: 'Confirm', about: 'the test on the areas never seen' },
];
const CHOICES = {
  generators: ['enumeration', 'depth-first', 'random', 'beam', 'tree', 'rules', 'simulation'],
  objectives: ['gap', 'effect', 'coverage', 'size', 'quality', 'simplicity', 'stability', 'causal', 'strata'],
  parents: ['tournament', 'uniform', 'roulette', 'ranking', 'boltzmann', 'truncation', 'lexicase', 'epsilon-lexicase', 'novelty'],
  mutations: ['add', 'drop', 'shift', 'flip', 'widen', 'narrow', 'swap', 'indicator', 'standard', 'frame'],
  crossovers: ['join', 'meet', 'one-point', 'uniform', 'graft'],
  schedule: ['fixed', 'probability-matching', 'adaptive-pursuit', 'ucb', 'thompson'],
  credit: ['better-than-parent', 'survival', 'improvement'],
  survival: ['plus', 'generational', 'comma', 'replace-worst', 'aging', 'deterministic-crowding', 'restricted-tournament', 'clearing', 'nsga2', 'alps', 'niches'],
  rule: ['e-lond', 'lond', 'lord++', 'saffron', 'alpha-investing'],
  inference: ['design', 'binomial'],
};
const SAID = {
  enumeration: 'the language, breadth first', 'depth-first': 'the language, depth first', random: 'drawn at random', beam: 'subgroup discovery by beam search',
  tree: 'a regression tree\'s leaves', rules: 'the declared rules', simulation: 'where seeking is left unmet',
  'e-lond': 'e-LOND: any dependence', lond: 'LOND: independence or positive dependence', 'lord++': 'LORD++: independence', saffron: 'SAFFRON: independence, adaptive', 'alpha-investing': 'α-investing: marginal FDR',
};

export function view() {
  return { id: 'needs', label: 'Needs', draw, heard: (e, ctx) => { if (/needs/.test(e.what) && document.querySelector('.tab.on')?.dataset.tab === 'needs' && !N.poll) { N.project = null; ctx.show('needs'); } } };
}

// ---- what the page holds -------------------------------------------------------------------------------------------------------------

async function ensure(ctx) {
  if (N.project === ctx.name && N.study) return;
  Object.assign(N, { project: ctx.name, sel: null, mapped: null, gen: null, read: null, tested: null, asked: null, wrong: '' });
  const r = await ctx.api(base(ctx));
  Object.assign(N, { study: r.study, cases: r.cases || [], report: r.report, run: r.run, proposals: r.proposals || [], model: r.model });
  N.place = await ctx.api(`${base(ctx)}/place?case=${enc(N.study.case)}`);
  try { N.desc = await ctx.api(`${base(ctx)}/describe`, { method: 'POST', body: { case: N.study.case, options: { ...(N.study.options || {}), simulate: false } } }); } catch { N.desc = null; }
  if (N.run?.running) follow(ctx);
}

const opts = () => (N.study.options ||= {});
const strategy = () => (opts().strategy ||= {});
async function keep(ctx) {
  try { await ctx.api(base(ctx), { method: 'PUT', body: { study: { case: N.study.case, options: N.study.options } } }); } catch { /* a page that may only look keeps nothing */ }
}

function objectiveIndex(name) { return (N.report?.objectives || []).indexOf(name); }
// A child's place in the field from its objectives: z from "gap", people from "coverage".
function placeOf(obj) {
  const gi = objectiveIndex('gap'), ci = objectiveIndex('coverage');
  if (!obj || gi < 0 || ci < 0 || obj[gi] == null || obj[ci] == null) return null;
  return { z: -obj[gi], people: 10 ** -obj[ci] };
}

function lineage() {
  const by = new Map();
  for (const l of N.report?.lineage || []) by.set(l[0], { ticket: l[0], generation: l[1], operation: l[2], parents: l[3], failed: !!(l[4] & 1), survived: !!(l[4] & 2), admitted: !!(l[4] & 4), at: placeOf(l[5]) });
  return by;
}

function domainInk(domain) {
  const all = N.place?.domains || [];
  const k = all.indexOf(domain);
  return INKS[(k < 0 ? all.length : k) % INKS.length];
}

// ---- drawing ---------------------------------------------------------------------------------------------------------------------------

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('nd-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  try { await ensure(ctx); } catch (error) { N.wrong = error.message; }
  ctx.here('the needs');
  const redraw = () => draw(pane, ctx);
  if (!N.study) return fill(pane, el('p', { class: 'wrong' }, N.wrong || 'The search could not be read.'));
  const stage = el('div', { class: 'nd-stage' }, el('div', { class: 'nd-main' }, field(ctx, redraw), register(ctx, redraw)), el('div', { class: 'nd-side' }, loop(ctx, redraw), areas(ctx, redraw)));
  fill(pane, header(ctx, redraw), N.compose ? compose(ctx, redraw) : null, stage, ledger(ctx, redraw), ribbon(ctx, redraw));
}

function header(ctx, redraw) {
  const { el } = ctx;
  const r = N.report;
  const confirmed = r ? r.confirmations.filter((c) => c.confirmed).length : 0;
  const cases = el('div', { class: 'nd-cases' }, N.cases.map((c) => el('button', { type: 'button', class: `nd-chip${c.case === N.study.case ? ' on' : ''}`, onclick: async () => {
    N.study.case = c.case; N.place = await ctx.api(`${base(ctx)}/place?case=${enc(c.case)}`); N.desc = null; N.sel = null; N.mapped = null; await keep(ctx); redraw();
  } }, c.title ? c.title.split(':')[0] : c.case)));
  const running = N.run?.running;
  const evolve = el('button', { type: 'button', class: 'nd-go', disabled: running ? 'disabled' : null, onclick: () => start(ctx, redraw, 10) },
    running ? `Generation ${(r?.generation ?? 0)} · running` : r ? 'Evolve ten more generations' : 'Begin the search');
  const stop = running ? el('button', { type: 'button', class: 'nd-quiet', onclick: async () => { await ctx.api(`${base(ctx)}/stop`, { method: 'POST', body: {} }); } }, 'Stop after this step') : null;
  const write = el('button', { type: 'button', class: `nd-quiet${N.compose ? ' on' : ''}`, onclick: () => { N.compose = !N.compose; redraw(); } }, 'Write or ask for statements');
  const again = r && !running ? el('button', { type: 'button', class: 'nd-quiet', onclick: async () => {
    if (!confirm('Set this search aside (it is kept beside the project) and begin again?')) return;
    await ctx.api(`${base(ctx)}/forget`, { method: 'POST', body: {} }); N.report = null; N.sel = null; redraw();
  } }, 'Begin again') : null;
  const words = r
    ? [`Generation ${r.generation}`, `${fig(r.evaluations)} statements judged`, `${r.archive.length} kept`, `${r.confirmations.length} tested on the areas kept aside, ${confirmed} confirmed`,
      `the holdout told ${r.holdout.overfits} of ${r.holdout.budget}`, `${r.online.rule} at α = ${r.online.alpha}`]
    : ['No search yet: the loop on the right says how it will go.'];
  const said = N.run?.words?.length ? el('p', { class: 'nd-said small' }, N.run.words.slice(-1)[0]) : null;
  const queued = N.queue.length ? el('span', { class: 'nd-queued small' }, `${N.queue.length} proposed statement${N.queue.length > 1 ? 's' : ''} wait for the next generation`) : null;
  return el('div', { class: 'nd-head' },
    el('div', { class: 'nd-title-row' }, el('h2', { class: 'nd-title' }, 'Needs'), cases),
    el('p', { class: 'nd-about small' }, N.place?.title || ''),
    el('p', { class: 'nd-stats' }, words.map((w, i) => el('span', { class: i ? '' : 'nd-strong' }, w))),
    el('div', { class: 'nd-actions' }, evolve, stop, write, again, queued),
    said, N.wrong ? el('p', { class: 'wrong' }, N.wrong) : null, N.busy ? el('p', { class: 'note' }, N.busy) : null);
}

async function start(ctx, redraw, generations) {
  N.wrong = '';
  const body = { case: N.study.case, options: N.study.options, generations, step: 2, proposals: N.queue };
  if (N.modelRounds) body.model = { rounds: 1, n: 8 };
  try {
    await ctx.api(`${base(ctx)}/run`, { method: 'POST', body });
    N.queue = [];
    N.run = { running: true, words: ['starting'] };
    follow(ctx, redraw);
  } catch (error) { N.wrong = error.message; }
  redraw();
}

function follow(ctx, redraw) {
  if (N.poll) return;
  N.poll = setInterval(async () => {
    try {
      const was = N.report?.generation;
      N.run = await ctx.api(`${base(ctx)}/run`);
      const r = await ctx.api(base(ctx));
      N.report = r.report, N.proposals = r.proposals || [];
      if (!N.run.running) { clearInterval(N.poll); N.poll = null; }
      if (redraw && (N.report?.generation !== was || !N.run.running)) redraw();
      else if (redraw) { const s = document.querySelector('.nd-said'); if (s && N.run.words?.length) s.textContent = N.run.words.slice(-1)[0]; }
    } catch { clearInterval(N.poll); N.poll = null; }
  }, 1500);
}

// ---- the field of statements ---------------------------------------------------------------------------------------------------------

const W = 720, H = 440, M = { l: 56, r: 18, t: 18, b: 44 };

function scales(points) {
  const people = points.map((p) => p.people).filter((x) => x > 0);
  const zs = points.map((p) => p.z).filter(Number.isFinite);
  const lo = Math.log10(Math.max(1, Math.min(...people, 1e9) / 1.4)), hi = Math.log10(Math.max(...people, 10) * 1.3);
  const top = Math.max(4, ...zs) * 1.06;
  const sz = (z) => Math.sign(z) * Math.sqrt(Math.abs(z));
  return {
    x: (p) => M.l + ((Math.log10(Math.max(p, 1)) - lo) / (hi - lo || 1)) * (W - M.l - M.r),
    y: (z) => H - M.b - ((sz(Math.max(-2, z)) - sz(-2)) / (sz(top) - sz(-2))) * (H - M.t - M.b),
    lo, hi, top,
  };
}

function glyph(svg, kind, x, y, r, attrs) {
  if (kind === 'normative' || kind === 'felt') return svg('rect', { x: x - r * 0.9, y: y - r * 0.9, width: 1.8 * r, height: 1.8 * r, transform: kind === 'felt' ? `rotate(45 ${x} ${y})` : null, ...attrs });
  if (kind === 'expressed') return svg('path', { d: `M${x},${y - r * 1.1}L${x + r},${y + r * 0.8}L${x - r},${y + r * 0.8}Z`, ...attrs });
  return svg('circle', { cx: x, cy: y, r, ...attrs });
}
const standardOf = (text) => (text.match(/ by (normative|comparative|expressed|felt) /) || [])[1] || 'comparative';

function field(ctx, redraw) {
  const { el, svg } = ctx;
  const r = N.report;
  const wrap = el('div', { class: 'nd-fieldwrap' });
  if (!r) {
    wrap.append(el('div', { class: 'nd-empty' }, el('p', {}, 'The field of statements fills as the search goes: every statement it makes is set by the people it concerns and by how large its gap is on the half of the areas it may see; the needs it keeps are drawn by their domain, their standard and their standing.'),
      el('p', { class: 'small' }, 'Begin the search above, or write statements of your own, or ask a language model for some; nothing proposed is taken on its word.')));
    return wrap;
  }
  const lin = lineage();
  const kept = r.archive.map((k) => ({ k, ev: k.evidence || {}, people: k.evidence?.discovery?.coverage, z: k.evidence?.discovery?.z })).filter((p) => p.people > 0 && Number.isFinite(p.z));
  const cloud = [...lin.values()].filter((l) => l.at && !l.failed && l.at.z >= -2);
  const S = scales([...kept, ...cloud.map((l) => l.at)]);
  const g = svg('svg', { class: 'nd-field', viewBox: `0 0 ${W} ${H}`, width: '100%', role: 'img', 'aria-label': 'The field of statements: people concerned against the size of the gap' });
  // Axes and the bounds of admission.
  const zstar = N.study.options?.z_admit ?? 3;
  for (const z of [0, 1, 2, 3, 5, 10, 20, 30, 50].filter((z) => z <= S.top)) {
    g.append(svg('line', { class: 'nd-grid', x1: M.l, x2: W - M.r, y1: S.y(z), y2: S.y(z) }), svg('text', { class: 'nd-tick', x: M.l - 6, y: S.y(z) + 3.5, 'text-anchor': 'end' }, String(z)));
  }
  for (let e = Math.ceil(S.lo); e <= Math.floor(S.hi); e++)
    g.append(svg('line', { class: 'nd-grid', x1: S.x(10 ** e), x2: S.x(10 ** e), y1: M.t, y2: H - M.b }), svg('text', { class: 'nd-tick', x: S.x(10 ** e), y: H - M.b + 15, 'text-anchor': 'middle' }, fig(10 ** e)));
  g.append(svg('line', { class: 'nd-zstar', x1: M.l, x2: W - M.r, y1: S.y(zstar), y2: S.y(zstar) }),
    svg('text', { class: 'nd-zstar-label', x: W - M.r - 4, y: S.y(zstar) - 5, 'text-anchor': 'end' }, `admitted from z = ${zstar}`),
    svg('text', { class: 'nd-axis', x: (M.l + W - M.r) / 2, y: H - 8, 'text-anchor': 'middle' }, 'people the statement concerns, on the areas the search sees'),
    svg('text', { class: 'nd-axis', x: 14, y: (M.t + H - M.b) / 2, transform: `rotate(-90 14 ${(M.t + H - M.b) / 2})`, 'text-anchor': 'middle' }, 'the gap, in its standard errors'));
  // Every statement made, the generation chosen brought forward.
  const cloudG = svg('g', { class: 'nd-cloud' });
  for (const l of cloud) {
    const on = N.gen != null && l.generation === N.gen;
    cloudG.append(svg('circle', { class: `nd-made${l.survived ? ' lived' : ''}${on ? ' on' : ''}`, cx: S.x(l.at.people), cy: S.y(l.at.z), r: on ? 2.6 : 1.7 }));
  }
  g.append(cloudG);
  // The line of operators that made the statement chosen.
  const chosen = kept.find((p) => p.k.ticket === N.sel);
  if (chosen) {
    const thread = svg('g', { class: 'nd-thread' });
    let at = lin.get(chosen.k.ticket), depth = 0, seen = new Set();
    let x0 = S.x(chosen.people), y0 = S.y(chosen.z);
    while (at && depth < 12 && !seen.has(at.ticket)) {
      seen.add(at.ticket);
      const parent = at.parents.map((t) => lin.get(t)).find((p) => p && p.at);
      thread.append(svg('text', { class: 'nd-op', x: x0 + 6, y: y0 - 6 }, at.operation));
      if (!parent) break;
      const x1 = S.x(parent.at.people), y1 = S.y(parent.at.z);
      thread.append(svg('path', { d: `M${x0},${y0} Q${(x0 + x1) / 2},${Math.min(y0, y1) - 18} ${x1},${y1}` }), svg('circle', { class: 'nd-ancestor', cx: x1, cy: y1, r: 3 }));
      x0 = x1, y0 = y1, at = parent, depth++;
    }
    g.append(thread);
  }
  // The needs kept.
  for (const p of kept) {
    const st = p.ev.standing?.standing || 'supported', kind = standardOf(p.k.text), ink = domainInk(p.k.domain);
    const x = S.x(p.people), y = S.y(p.z), on = p.k.ticket === N.sel;
    const node = svg('g', { class: `nd-need nd-${st}${on ? ' on' : ''}`, tabindex: 0, role: 'button', 'aria-label': p.k.text, onclick: () => { N.sel = on ? null : p.k.ticket; N.mapped = null; redraw(); },
      onkeydown: (e) => { if (e.key === 'Enter') { N.sel = on ? null : p.k.ticket; redraw(); } } });
    node.append(svg('title', {}, `${p.k.text}\n${st}`));
    if (st === 'robust') node.append(glyph(svg, kind, x, y, 9.5, { class: 'nd-ring2', stroke: ink }));
    node.append(glyph(svg, kind, x, y, 6.5, { class: 'nd-mark', fill: st === 'unsupported' || st === 'refuted' ? 'none' : ink, stroke: ink }));
    if (st === 'refuted') node.append(svg('line', { class: 'nd-slash', x1: x - 7, y1: y + 7, x2: x + 7, y2: y - 7, stroke: ink }));
    g.append(node);
  }
  wrap.append(g, legend(ctx));
  if (chosen) wrap.append(card(ctx, redraw, chosen, S.x(chosen.people) / W, S.y(chosen.z) / H));
  return wrap;
}

function legend(ctx) {
  const { el, svg } = ctx;
  const doms = (N.place?.domains || []).map((d) => el('span', { class: 'nd-key' }, el('i', { class: 'nd-swatch', style: { background: domainInk(d) } }), d));
  const shape = (kind, words) => el('span', { class: 'nd-key' }, (() => { const s = svg('svg', { width: 14, height: 14, viewBox: '0 0 14 14' }); s.append(glyph(svg, kind, 7, 7, 4.6, { class: 'nd-legend-mark' })); return s; })(), words);
  const ring = (cls, words) => el('span', { class: `nd-key nd-${cls}` }, el('i', { class: `nd-ringkey ${cls}` }), words);
  return el('div', { class: 'nd-legend small' }, doms, el('span', { class: 'nd-sep' }), shape('comparative', 'comparative'), shape('normative', 'normative'), shape('expressed', 'expressed'), shape('felt', 'felt'),
    el('span', { class: 'nd-sep' }), ring('robust', 'confirmed'), ring('supported', 'not yet confirmed'), ring('unsupported', 'unsupported'), el('span', { class: 'nd-key' }, el('i', { class: 'nd-dotkey' }), 'every statement made'));
}

// ---- a need opened where it stands ------------------------------------------------------------------------------------------------------

function verdictMark(v) { return v === 'holds' ? '✓' : v === 'fails' ? '✗' : '·'; }

function card(ctx, redraw, p, fx, fy) {
  const { el } = ctx;
  const ev = p.ev, d = ev.discovery || {}, c = ev.confirmation, st = ev.standing || {};
  const left = fx > 0.55;
  const tests = [
    ['on the areas seen', d.z >= (N.study.options?.z_admit ?? 3) ? 'holds' : 'fails', `z ${fig(d.z, 1)}, ${pct(d.group_value, 2)} against ${pct(d.reference_value, 2)}`],
    ['the holdout', ev.validation ? (ev.validation.verdict === 'refused' ? 'fails' : ev.validation.verdict === 'spent' ? 'undecided' : 'holds') : 'undecided',
      ev.validation ? `${ev.validation.verdict}${Number.isFinite(ev.validation.value) ? (ev.validation.value >= 1 ? ': an effect of a spread or more' : `: an effect of ${fig(ev.validation.value, 2)} of a spread`) : ''}` : 'not asked'],
    ['resampled areas', (ev.stability?.share ?? 0) >= (N.study.options?.stability_min ?? 0.8) ? 'holds' : 'fails', `stable in ${pct(ev.stability?.share, 0)}`],
    ['the areas kept aside', c ? (c.confirmed ? 'holds' : c.opposite ? 'fails' : 'undecided') : 'undecided',
      c ? `${c.confirmed ? 'confirmed' : c.opposite ? 'the opposite gap' : 'not confirmed'}: ${pct(c.estimate, 2)} gap, p ${sig(c.p)} at level ${sig(c.level)}` : 'not yet tested'],
    ['provision', ev.causal?.verdict, ev.causal?.applicable ? `${ev.causal.why}${ev.causal.adjustment?.length ? ` (given ${ev.causal.adjustment.join(', ')})` : ''}` : ev.causal?.why],
    ['seeking', ev.simulation?.verdict, ev.simulation?.applicable ? `${pct(ev.simulation.unmet_group)} left unmet against ${pct(ev.simulation.unmet_reference)}` : ev.simulation?.why],
    ['the rules', ev.rules?.verdict, ev.rules?.applicable ? `${pct(ev.rules.share, 0)} entitled${ev.rules.derivations?.length ? `: ${ev.rules.derivations[0]}` : ''}` : ev.rules?.why],
  ];
  const args = (st.arguments || []).map((a) => el('span', { class: `nd-arg ${a.label}`, title: a.text }, a.id));
  const vary = el('button', { type: 'button', class: 'nd-quiet', onclick: async () => { N.compose = true; N.asked = null; await ask(ctx, redraw, { vary: [p.k.text] }); } }, 'Vary with a model');
  const place = el('button', { type: 'button', class: 'nd-quiet', onclick: async () => {
    try { N.mapped = await ctx.api(`${base(ctx)}/map`, { method: 'POST', body: { case: N.study.case, statement: p.k.text } }); } catch (error) { N.wrong = error.message; }
    redraw();
  } }, 'Show where on the map');
  return el('aside', { class: `nd-card${left ? ' left' : ''}`, style: { top: `${Math.min(70, Math.max(2, fy * 100 - 8))}%`, [left ? 'right' : 'left']: left ? `${(1 - fx) * 100 + 3}%` : `${fx * 100 + 3}%` } },
    el('button', { type: 'button', class: 'nd-close', 'aria-label': 'Close', onclick: () => { N.sel = null; N.mapped = null; redraw(); } }, '×'),
    el('p', { class: 'nd-kicker' }, `${p.k.domain} · ${standardOf(p.k.text)} · ${st.standing || ''}`),
    el('p', { class: 'nd-statement' }, p.k.text),
    el('p', { class: 'nd-plain' }, `${pct(d.group_value, 2)} of the ${fig(d.coverage)} people it concerns, against ${pct(d.reference_value, 2)}; the gap ${pct(d.estimate, 2)} [${pct(d.low, 2)}, ${pct(d.high, 2)}].`),
    el('dl', { class: 'nd-tests' }, tests.map(([what, v, words]) => [el('dt', {}, el('span', { class: `nd-v ${v || 'undecided'}` }, verdictMark(v)), what), el('dd', {}, words || '')])),
    el('p', { class: 'nd-args small' }, 'Its standing, from the argument of its tests: ', args, ' (in, or struck out)'),
    (st.qualifiers || []).length ? el('p', { class: 'small nd-qual' }, st.qualifiers.join(' · ')) : null,
    el('p', { class: 'small nd-note' }, p.k.note ? `Kept at generation ${p.k.admitted}: ${p.k.note}. Made by ${p.k.operation}.` : ''),
    el('div', { class: 'nd-actions' }, place, vary));
}

// ---- the register of needs kept -------------------------------------------------------------------------------------------------------

function register(ctx, redraw) {
  const { el } = ctx;
  const r = N.report;
  if (!r || !r.archive.length) return el('div');
  const order = { robust: 0, supported: 1, unsupported: 2, refuted: 3 };
  const rows = [...r.archive].sort((a, b) => (order[a.evidence?.standing?.standing] ?? 9) - (order[b.evidence?.standing?.standing] ?? 9) || (b.evidence?.discovery?.z ?? 0) - (a.evidence?.discovery?.z ?? 0));
  const groups = {};
  for (const k of rows) (groups[k.domain] ||= []).push(k);
  return el('div', { class: 'nd-register' },
    el('h3', { class: 'nd-h' }, 'The needs kept, by domain'),
    Object.entries(groups).map(([dom, ks]) => el('div', { class: 'nd-dom' },
      el('p', { class: 'nd-dom-name' }, el('i', { class: 'nd-swatch', style: { background: domainInk(dom) } }), dom, el('span', { class: 'small' }, ` ${ks.length}`)),
      ks.map((k) => {
        const ev = k.evidence || {}, st = ev.standing?.standing || '', dd = ev.discovery || {};
        return el('button', { type: 'button', class: `nd-row nd-${st}${k.ticket === N.sel ? ' on' : ''}`, onclick: () => { N.sel = k.ticket === N.sel ? null : k.ticket; N.mapped = null; redraw(); window.scrollTo?.({ top: 0 }); } },
          el('span', { class: 'nd-row-st' }, st === 'robust' ? '◎' : st === 'supported' ? '○' : '◌'),
          el('span', { class: 'nd-row-text' }, k.text.replace(/^need \S+ for /, '').replace(/ by (comparative|normative|expressed|felt) /, ' · $1 ')),
          el('span', { class: 'nd-row-num' }, `${pct(dd.group_value, 1)} / ${pct(dd.reference_value, 1)}`));
      }))),
    r.retired.length ? el('details', { class: 'nd-retired small' }, el('summary', {}, `${r.retired.length} retired, with their reasons`),
      r.retired.slice(-30).reverse().map((x) => el('p', {}, el('span', { class: 'nd-mono' }, x.text), ` — ${x.reason} (kept ${x.admitted}–${x.retired}${x.confirmed === true ? ', confirmed' : ''})`))) : null);
}

// ---- the loop of the search -----------------------------------------------------------------------------------------------------------

function currentOf(stage) {
  const o = opts(), s = strategy();
  const off = (names) => names.filter((n) => o.operators?.[n] === 0);
  switch (stage) {
    case 'generate': { const g = CHOICES.generators.filter((n) => o.operators?.[n] !== 0); return g.length === CHOICES.generators.length ? 'all seven' : g.join(', '); }
    case 'evaluate': return (o.objectives || ['gap', 'coverage', 'simplicity']).join(' · ');
    case 'select': return `${s.parents || 'tournament'}${s.novelty ? ' + novelty' : ''}`;
    case 'vary': return `${s.schedule || 'fixed'}${off([...CHOICES.mutations, ...CHOICES.crossovers]).length ? `, ${off([...CHOICES.mutations, ...CHOICES.crossovers]).length} off` : ''}`;
    case 'survive': return s.survival || 'plus';
    case 'admit': return `z ≥ ${o.z_admit ?? 3}, δ ${o.min_distance ?? 0.25}`;
    default: return `${o.rule || 'e-lond'}, α ${o.alpha ?? 0.05}`;
  }
}

function loop(ctx, redraw) {
  const { el, svg } = ctx;
  const R = 84, cx = 160, cy = 128;
  const g = svg('svg', { class: 'nd-loop', viewBox: '0 0 320 256', width: '100%', role: 'img', 'aria-label': 'The loop of the search, each stage with its strategy' });
  g.append(svg('circle', { class: 'nd-loop-ring', cx, cy, r: R }));
  const n = STAGES.length;
  STAGES.forEach((s, i) => {
    const a = -Math.PI / 2 + (2 * Math.PI * i) / n;
    const x = cx + R * Math.cos(a), y = cy + R * Math.sin(a);
    const on = N.opened === s.id;
    const node = svg('g', { class: `nd-stagenode${on ? ' on' : ''}`, tabindex: 0, role: 'button', onclick: () => { N.opened = on ? null : s.id; redraw(); }, onkeydown: (e) => { if (e.key === 'Enter') { N.opened = on ? null : s.id; redraw(); } } });
    node.append(svg('circle', { cx: x, cy: y, r: on ? 8 : 6 }), svg('title', {}, `${s.label}: ${s.about}`));
    const right = Math.cos(a) > 0.2, leftSide = Math.cos(a) < -0.2, top = Math.sin(a) < -0.85, bottom = Math.sin(a) > 0.6;
    const tx = x + (right ? 12 : leftSide ? -12 : 0), ty = top ? y - 24 : bottom ? y + 20 : y + 1;
    const anchor = right ? 'start' : leftSide ? 'end' : 'middle';
    const said = currentOf(s.id);
    node.append(svg('text', { class: 'nd-stagelabel', x: tx, y: ty, 'text-anchor': anchor }, s.label));
    node.append(svg('text', { class: 'nd-stagechoice', x: tx, y: ty + 11, 'text-anchor': anchor }, said.length > 24 ? `${said.slice(0, 23)}…` : said));
    g.append(node);
  });
  const r = N.report;
  g.append(svg('text', { class: 'nd-loop-centre', x: cx, y: cy - 4, 'text-anchor': 'middle' }, r ? `generation ${r.generation}` : 'not begun'),
    svg('text', { class: 'nd-loop-centre small', x: cx, y: cy + 12, 'text-anchor': 'middle' }, r ? `${r.archive.length} kept` : ''));
  const box = el('div', { class: 'nd-loopbox' }, el('h3', { class: 'nd-h' }, 'The loop'), g);
  if (N.opened) box.append(stagePanel(ctx, redraw, N.opened));
  return box;
}

function chips(ctx, names, isOn, toggle, said = {}) {
  const { el } = ctx;
  return el('div', { class: 'nd-chips' }, names.map((n) => el('button', { type: 'button', class: `nd-chip${isOn(n) ? ' on' : ''}`, title: said[n] || SAID[n] || n, onclick: () => toggle(n) }, n)));
}
function numberField(ctx, label, value, set, step = 0.1) {
  const { el } = ctx;
  return el('label', { class: 'nd-num' }, el('span', {}, label), el('input', { type: 'number', step, value: value ?? '', onchange: (e) => set(e.target.value === '' ? undefined : Number(e.target.value)) }));
}

function stagePanel(ctx, redraw, id) {
  const { el } = ctx;
  const o = opts(), s = strategy();
  const set = async (f) => { f(); await keep(ctx); redraw(); };
  const opOn = (n) => o.operators?.[n] !== 0;
  const opToggle = (n) => set(() => { o.operators ||= {}; if (o.operators[n] === 0) delete o.operators[n]; else o.operators[n] = 0; });
  const one = (key, names, into = s) => chips(ctx, names, (n) => (into[key] || names[0]) === n, (n) => set(() => { into[key] = n; }));
  const stage = STAGES.find((x) => x.id === id);
  const parts = [el('p', { class: 'nd-stage-about small' }, `${stage.label}: ${stage.about}.`)];
  if (id === 'generate') {
    parts.push(chips(ctx, CHOICES.generators, opOn, opToggle),
      el('label', { class: 'nd-check small' }, el('input', { type: 'checkbox', checked: N.modelRounds ? 'checked' : null, onchange: (e) => { N.modelRounds = e.target.checked; } }),
        ' and a language model, asked between steps for statements (proposals only, read like anyone\'s; it sees no part kept aside)'),
      numberField(ctx, 'share of children made new', s.generator_rate ?? 0.15, (v) => set(() => { s.generator_rate = v; }), 0.05));
  } else if (id === 'evaluate') {
    const objs = o.objectives || ['gap', 'coverage', 'simplicity'];
    parts.push(chips(ctx, CHOICES.objectives, (n) => objs.includes(n), (n) => set(() => {
      const now = (o.objectives || ['gap', 'coverage', 'simplicity']).slice();
      const k = now.indexOf(n);
      if (k >= 0 && now.length > 1) now.splice(k, 1); else if (k < 0) now.push(n);
      o.objectives = now;
    })), el('p', { class: 'small' }, 'Ordered as chosen; "strata" are the gap in each of five strata of the areas, the cases lexicase reads.'),
    one('scoring', ['first', 'weighted', 'lexicographic', 'pareto']), one('inference', CHOICES.inference, o));
  } else if (id === 'select') {
    parts.push(one('parents', CHOICES.parents), el('label', { class: 'nd-check small' }, el('input', { type: 'checkbox', checked: s.novelty ? 'checked' : null, onchange: (e) => set(() => { s.novelty = e.target.checked; }) }), ' novelty as a further objective'),
      numberField(ctx, 'tournament', s.tournament ?? 3, (v) => set(() => { s.tournament = v; }), 1), numberField(ctx, 'from the archive', s.from_archive ?? 0.15, (v) => set(() => { s.from_archive = v; }), 0.05));
  } else if (id === 'vary') {
    parts.push(el('p', { class: 'small' }, 'Mutations'), chips(ctx, CHOICES.mutations, opOn, opToggle), el('p', { class: 'small' }, 'Crossovers'), chips(ctx, CHOICES.crossovers, opOn, opToggle),
      el('p', { class: 'small' }, 'How an operator is chosen, and what it is credited for'), one('schedule', CHOICES.schedule), one('credit', CHOICES.credit),
      numberField(ctx, 'crossover rate', s.crossover_rate ?? 0.35, (v) => set(() => { s.crossover_rate = v; }), 0.05));
  } else if (id === 'survive') {
    parts.push(one('survival', CHOICES.survival), numberField(ctx, 'population μ', s.mu ?? 60, (v) => set(() => { s.mu = v; }), 1), numberField(ctx, 'children λ', s.lambda ?? 60, (v) => set(() => { s.lambda = v; }), 1));
  } else if (id === 'admit') {
    parts.push(numberField(ctx, 'z on the areas seen', o.z_admit ?? 3, (v) => set(() => { o.z_admit = v; })), numberField(ctx, 'least effect (spreads)', o.min_effect ?? 0.2, (v) => set(() => { o.min_effect = v; }), 0.05),
      numberField(ctx, 'stable in (share)', o.stability_min ?? 0.8, (v) => set(() => { o.stability_min = v; }), 0.05), numberField(ctx, 'each condition\'s own z', o.z_productive ?? 2, (v) => set(() => { o.z_productive = v; })),
      numberField(ctx, 'distinct by δ', o.min_distance ?? 0.25, (v) => set(() => { o.min_distance = v; }), 0.05), numberField(ctx, 'retired after idle generations', s.retire_after ?? 8, (v) => set(() => { s.retire_after = v; }), 1),
      numberField(ctx, 'holdout budget', o.holdout?.budget ?? 100, (v) => set(() => { o.holdout = { ...(o.holdout || {}), budget: v }; }), 1));
  } else {
    parts.push(one('rule', CHOICES.rule, o), el('p', { class: 'small' }, SAID[o.rule || 'e-lond']), numberField(ctx, 'α', o.alpha ?? 0.05, (v) => set(() => { o.alpha = v; }), 0.01),
      el('p', { class: 'small' }, 'Each statement admitted is tested once on the quarter of the areas the search never sees, in the order admitted; its test is sealed in a journal before it runs, and nothing of it reaches the search.'));
  }
  return el('div', { class: 'nd-stagepanel' }, parts);
}

// ---- the areas -----------------------------------------------------------------------------------------------------------------------

function areas(ctx, redraw) {
  const { el, svg } = ctx;
  const rings = N.place?.map?.rings || {};
  const codes = Object.keys(rings);
  if (!codes.length) return el('div');
  let x0 = Infinity, x1 = -Infinity, y0 = Infinity, y1 = -Infinity;
  for (const c of codes) for (const ring of rings[c]) for (const [x, y] of ring) { x0 = Math.min(x0, x); x1 = Math.max(x1, x); y0 = Math.min(y0, y); y1 = Math.max(y1, y); }
  const Wm = 260, Hm = Math.round(Wm * (y1 - y0) / (x1 - x0 || 1));
  const sx = (x) => ((x - x0) / (x1 - x0)) * Wm, sy = (y) => Hm - ((y - y0) / (y1 - y0)) * Hm;
  const g = svg('svg', { class: 'nd-map', viewBox: `0 0 ${Wm} ${Hm}`, width: '100%', role: 'img', 'aria-label': N.mapped ? 'Where the need lies, area by area' : 'The areas by the part of the data each lies in' });
  const by = new Map((N.mapped?.places || []).map((p) => [p.place, p]));
  let top = 0;
  for (const p of by.values()) if (p.group_value != null && p.group_people >= 5) top = Math.max(top, p.group_value);
  const partOf = N.desc?.places || {};
  for (const c of codes) {
    const p = by.get(c);
    let cls = 'nd-area', fill = null, title = c;
    if (N.mapped) {
      if (p && p.group_value != null && p.group_people >= 5) { fill = `color-mix(in srgb, var(--nd-hot) ${Math.round(8 + 87 * (p.group_value / (top || 1)))}%, var(--nd-cold))`; title = `${c}: ${pct(p.group_value)} of ${fig(p.group_people)} in the group (${pct(p.rest_value)} of the others), ${p.part}`; }
      else cls += ' nd-none';
    } else if (partOf[c]) cls += ` nd-part-${partOf[c]}`;
    const d = rings[c].map((ring) => `M${ring.map(([x, y]) => `${sx(x).toFixed(1)},${sy(y).toFixed(1)}`).join('L')}Z`).join('');
    const path = svg('path', { class: cls, d, style: fill ? { fill } : null });
    path.append(svg('title', {}, title));
    g.append(path);
  }
  for (const s of N.place?.suppliers || []) g.append(svg('rect', { class: 'nd-supplier', x: sx(s.x) - 2, y: sy(s.y) - 2, width: 4, height: 4 }, svg('title', {}, `${s.id}: ${fig(s.capacity)} served (declared)`)));
  const said = N.mapped
    ? el('p', { class: 'small' }, `Where the need lies: each area by its share in the group (areas with fewer than five of the group left pale). ${N.mapped.statement}`)
    : el('p', { class: 'small' }, 'Half of the areas are seen by the search, a quarter consulted only through the holdout, a quarter kept aside for confirmation, decided by a keyed hash of each area\'s code, never by its data. Squares are blocks with care floor space, the suppliers of the simulation.');
  const parts = !N.mapped ? el('div', { class: 'nd-legend small' }, ['discovery', 'validation', 'confirmation'].map((p) => el('span', { class: 'nd-key' }, el('i', { class: `nd-swatch nd-part-${p}` }), p))) : null;
  return el('div', { class: 'nd-areasbox' }, el('h3', { class: 'nd-h' }, N.place?.place?.name || 'The areas'), g, parts, said);
}

// ---- the ledger of confirmations ----------------------------------------------------------------------------------------------------

function ledger(ctx, redraw) {
  const { el, svg } = ctx;
  const r = N.report;
  if (!r || !r.confirmations.length) return el('div');
  const e = r.online.rule === 'e-lond';
  const items = r.confirmations.map((c) => ({ c, s: Math.min(14, e ? Math.log10(Math.max(c.e, 1e-12)) : -Math.log10(Math.max(c.p, 1e-14))), t: -Math.log10(Math.max(c.level, 1e-14)) }));
  const Wl = 1000, Hl = 170, ml = 48, mr = 12, mt = 12, mb = 28;
  const top = Math.max(6, ...items.map((i) => Math.max(i.s, i.t))) * 1.05;
  const x = (k) => ml + (k + 0.5) * ((Wl - ml - mr) / items.length), y = (v) => Hl - mb - (Math.max(0, v) / top) * (Hl - mt - mb);
  const g = svg('svg', { class: 'nd-ledger', viewBox: `0 0 ${Wl} ${Hl}`, width: '100%', role: 'img', 'aria-label': 'Each confirmation against the level the online rule spent on it' });
  for (let v = 0; v <= top; v += 2) g.append(svg('line', { class: 'nd-grid', x1: ml, x2: Wl - mr, y1: y(v), y2: y(v) }), svg('text', { class: 'nd-tick', x: ml - 6, y: y(v) + 3.5, 'text-anchor': 'end' }, v >= 14 ? '14+' : String(v)));
  let d = '';
  items.forEach((it, k) => { const xa = ml + k * ((Wl - ml - mr) / items.length), xb = xa + (Wl - ml - mr) / items.length; d += `${k ? 'L' : 'M'}${xa},${y(it.t)}L${xb},${y(it.t)}`; });
  g.append(svg('path', { class: 'nd-level', d }));
  items.forEach((it, k) => {
    const dot = svg('circle', { class: `nd-test${it.c.confirmed ? ' yes' : ''}${it.c.opposite ? ' opp' : ''}${N.sel === it.c.ticket ? ' on' : ''}`, cx: x(k), cy: y(it.s), r: 3.6, onclick: () => { N.sel = it.c.ticket; redraw(); } });
    dot.append(svg('title', {}, `test ${it.c.serial}: ${it.c.text}\n${it.c.confirmed ? 'confirmed' : 'not confirmed'}; p ${sig(it.c.p)}${e ? `, e ${fig(it.c.e, 1)}` : ''}; level ${sig(it.c.level)}`));
    g.append(dot);
  });
  g.append(svg('text', { class: 'nd-axis', x: ml, y: Hl - 6 }, `tests in the order admitted (${items.length}); the line is the level the rule spent on each, ${e ? 'log₁₀ 1/α' : '−log₁₀ α'}; a dot above it is confirmed`));
  const head = r.journal_head ? el('p', { class: 'small nd-seal' }, `Each test was registered in the journal before the areas kept aside were read, and its outcome written after; the journal of ${r.journal.length} entries ends in `, el('span', { class: 'nd-mono' }, `${r.journal_head.slice(0, 24)}…`), '.') : null;
  return el('div', { class: 'nd-ledgerbox' }, el('h3', { class: 'nd-h' }, `The ledger of confirmations · ${r.online.rule} at α = ${r.online.alpha}`), g, head);
}

// ---- the generations ---------------------------------------------------------------------------------------------------------------

function ribbon(ctx, redraw) {
  const { el, svg } = ctx;
  const st = N.report?.statistics || [];
  if (!st.length) return el('div');
  const ops = N.report.operators || [];
  const Wr = 1000, Hr = 120, ml = 48, mb = 22;
  const gi = objectiveIndex('gap');
  const most = Math.max(1, ...st.map((s) => s.archive));
  const bestZ = st.map((s) => (gi >= 0 && s.best[gi] != null ? -s.best[gi] : null));
  const topZ = Math.max(1, ...bestZ.filter((z) => z != null));
  const colW = (Wr - ml - 12) / st.length;
  const g = svg('svg', { class: 'nd-ribbon', viewBox: `0 0 ${Wr} ${Hr}`, width: '100%', role: 'img', 'aria-label': 'Generation by generation: the archive, its admissions and retirements, the best gap, and who made the children' });
  const kinds = ops.map((o) => (CHOICES.generators.includes(o) ? 'gen' : CHOICES.crossovers.includes(o) && !CHOICES.mutations.includes(o) ? 'cross' : 'mut'));
  st.forEach((s, k) => {
    const x = ml + k * colW, on = N.gen === s.generation;
    const col = svg('g', { class: `nd-generation${on ? ' on' : ''}`, onclick: () => { N.gen = on ? null : s.generation; redraw(); } });
    const h = (s.archive / most) * (Hr - mb - 46);
    col.append(svg('rect', { class: 'nd-hit', x, y: 0, width: colW, height: Hr - mb }), svg('rect', { class: 'nd-arch', x: x + colW * 0.2, y: Hr - mb - 40 - h, width: colW * 0.6, height: h }));
    let sum = 0;
    const births = s.births || [];
    const total = births.reduce((a, b) => a + b, 0) || 1;
    for (const kind of ['gen', 'mut', 'cross']) {
      const n = births.reduce((a, b, i) => a + (kinds[i] === kind ? b : 0), 0);
      const w = (n / total) * colW * 0.8;
      col.append(svg('rect', { class: `nd-birth nd-k-${kind}`, x: x + colW * 0.1 + sum, y: Hr - mb - 30, width: w, height: 8 }));
      sum += w;
    }
    col.append(svg('title', {}, `generation ${s.generation}: ${s.archive} kept (+${s.admitted} −${s.retired}), ${s.evaluations} judged; children by generators, mutations, crossovers`));
    if (k % Math.ceil(st.length / 20) === 0) col.append(svg('text', { class: 'nd-tick', x: x + colW / 2, y: Hr - 6, 'text-anchor': 'middle' }, String(s.generation)));
    g.append(col);
  });
  const pts = bestZ.map((z, k) => (z == null ? null : `${ml + (k + 0.5) * colW},${8 + (1 - z / topZ) * 28}`)).filter(Boolean);
  if (pts.length > 1) g.append(svg('polyline', { class: 'nd-bestz', points: pts.join(' ') }));
  g.append(svg('text', { class: 'nd-axis', x: 4, y: 20 }, 'best z'), svg('text', { class: 'nd-axis', x: 4, y: Hr - mb - 40 }, 'kept'), svg('text', { class: 'nd-axis', x: 4, y: Hr - mb - 22 }, 'made by'));
  const keyOf = (cls, words) => el('span', { class: 'nd-key' }, el('i', { class: `nd-swatch nd-k-${cls}` }), words);
  return el('div', { class: 'nd-ribbonbox' }, el('h3', { class: 'nd-h' }, 'The generations'), g,
    el('div', { class: 'nd-legend small' }, keyOf('gen', 'generators'), keyOf('mut', 'mutations'), keyOf('cross', 'crossovers'), el('span', { class: 'small' }, N.gen != null ? ` · the children of generation ${N.gen} are brought forward in the field` : ' · a generation chosen brings its children forward in the field')));
}

// ---- writing and asking ----------------------------------------------------------------------------------------------------------------

async function ask(ctx, redraw, body) {
  N.busy = 'Asking the language model…';
  redraw();
  try { N.asked = await ctx.api(`${base(ctx)}/propose`, { method: 'POST', body: { case: N.study.case, n: 8, ...body } }); } catch (error) { N.asked = { error: error.message }; }
  N.busy = '';
  redraw();
}

function compose(ctx, redraw) {
  const { el } = ctx;
  const text = el('textarea', { class: 'nd-text', rows: 4, spellcheck: 'false', placeholder: 'need care for age >= 75 and renting > 0.6 by comparative need_assist higher', oninput: (e) => { N.written = e.target.value; } }, N.written);
  const readKey = el('button', { type: 'button', class: 'nd-quiet', onclick: async () => {
    try { N.read = await ctx.api(`${base(ctx)}/read`, { method: 'POST', body: { case: N.study.case, text: N.written } }); N.tested = null; } catch (error) { N.read = [{ read: false, text: '', why: error.message }]; }
    redraw();
  } }, 'Read');
  const ok = (N.read || []).filter((r) => r.read);
  const testKey = ok.length ? el('button', { type: 'button', class: 'nd-quiet', onclick: async () => {
    N.busy = 'Testing…'; redraw();
    try { N.tested = await ctx.api(`${base(ctx)}/test`, { method: 'POST', body: { case: N.study.case, statements: ok.map((r) => r.statement) } }); } catch (error) { N.wrong = error.message; }
    N.busy = ''; redraw();
  } }, 'Test them') : null;
  const addKey = ok.length ? el('button', { type: 'button', class: 'nd-quiet', onclick: () => { N.queue.push({ text: ok.map((r) => r.statement).join('\n'), source: 'a person' }); N.read = null; N.written = ''; redraw(); } }, 'Add to the next generations') : null;
  const lines = (N.read || []).map((r) => el('p', { class: `nd-line ${r.read ? 'yes' : 'no'}` }, el('span', { class: 'nd-v' }, r.read ? '✓' : '✗'), el('span', { class: 'nd-mono' }, r.read ? r.statement : r.text), r.read ? null : el('span', { class: 'small' }, ` — ${r.why}`)));
  const tested = (N.tested || []).map((t) => {
    if (t.error) return el('p', { class: 'nd-line no' }, el('span', { class: 'nd-mono' }, t.text), el('span', { class: 'small' }, ` — ${t.error}`));
    const d = t.evidence.discovery;
    return el('p', { class: 'nd-line' }, el('span', { class: 'nd-mono' }, t.statement), el('span', { class: 'small' }, ` — ${pct(d.group_value, 2)} against ${pct(d.reference_value, 2)}, ${fig(d.coverage)} people, z ${fig(d.z, 1)}; ${t.evidence.standing.standing}`));
  });
  const desc = el('textarea', { class: 'nd-text', rows: 2, placeholder: 'What to look for, in words: older people living alone who may lack care…', oninput: (e) => { N.lookfor = e.target.value; } }, N.lookfor);
  const can = N.model?.kept;
  const askKey = el('button', { type: 'button', class: 'nd-quiet', disabled: can ? null : 'disabled', title: can ? '' : 'no key is kept for a language model in your settings folder', onclick: () => ask(ctx, redraw, { description: N.lookfor || null }) }, 'Ask a model for statements');
  const asked = N.asked ? (N.asked.error ? el('p', { class: 'wrong' }, N.asked.error) : el('div', { class: 'nd-asked' },
    el('p', { class: 'small' }, `${N.asked.by}: ${N.asked.read.length} read, ${N.asked.refused.length} refused, each a proposal of grade C.`),
    N.asked.read.map((r) => el('p', { class: 'nd-line yes' }, el('span', { class: 'nd-v' }, '✓'), el('span', { class: 'nd-mono' }, r.statement), r.reason ? el('span', { class: 'small' }, ` — ${r.reason}`) : null)),
    N.asked.refused.map((r) => el('p', { class: 'nd-line no' }, el('span', { class: 'nd-v' }, '✗'), el('span', { class: 'nd-mono' }, r.line), el('span', { class: 'small' }, ` — ${r.why}`))),
    N.asked.text ? el('button', { type: 'button', class: 'nd-quiet', onclick: () => { N.queue.push({ text: N.asked.text, source: N.asked.source || 'a language model' }); N.asked = null; redraw(); } }, 'Add the ones that read to the next generations') : null)) : null;
  return el('div', { class: 'nd-compose' },
    el('div', { class: 'nd-half' }, el('h3', { class: 'nd-h' }, 'Statements of your own'), el('p', { class: 'small' }, 'One to a line, in the language of needs; read and checked against the data, never repaired.'), text, el('div', { class: 'nd-actions' }, readKey, testKey, addKey), lines, tested),
    el('div', { class: 'nd-half' }, el('h3', { class: 'nd-h' }, 'A language model\'s proposals'),
      el('p', { class: 'small' }, 'The model is shown the language, the data\'s vocabulary and the population\'s best statements scored on the areas the search sees, never a validation, a confirmation or a standing; what it writes is read like anyone\'s, and enters as a proposal.'),
      desc, el('div', { class: 'nd-actions' }, askKey, can ? el('span', { class: 'small' }, `key ${N.model.ends}`) : el('span', { class: 'small' }, 'no key kept')), asked));
}
