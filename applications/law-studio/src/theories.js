// Theories of the case (ADR 0127): accounts of what happened, made, varied and crossed by an evolutionary search (tekmerion's
// scenarios on heuresis), each judged against the evidence (where it puts people, at what speed, what it must doubt), kept as the
// best account of its kind, read under the law, and tested once on what the matter holds back.
//
// The page is the field of accounts: a band in which each theory kept takes the width of its relative plausibility, toned by what
// the law makes of the question under it; above it, each question's standing over the theories found and whether the alternatives
// are excluded; a theory chosen opens beneath the band as its story, minute by minute. Then the search itself (its generations, its
// operators, who proposed what), a theory of one's own written and assessed, and the ledger of tests, sealed.

import { clockOf } from '/visualization/js/evidence/index.js';
import { S, T, ensure, loadTheories, runTheories, chosenTheory, nameOf, gloss, classOf } from './common.js';

export function theoriesView() {
  return { id: 'theories', label: 'Theories', draw };
}

const TONE = { clear: 'var(--ls-clear)', arguable: 'var(--ls-arguable)', untenable: 'var(--ls-untenable)' };
const MARK = { clear: '✓', arguable: '◐', untenable: '✕' };

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('th-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('theories of the case');
  if (!S.matter?.located?.length)
    return fill(pane, el('p', { class: 'note' }, 'This matter places no evidence in space and time, so there is nothing for a search for theories to work on.'));
  try { await loadTheories(ctx); } catch (error) { return fill(pane, el('p', { class: 'note wrong' }, error.message)); }
  const R = T.report, redraw = () => ctx.show('theories');
  if (!R) return fill(pane, el('p', { class: 'note' }, 'The evidence could not be laid out.'));
  const kept = R.kept || [];
  if (T.chosen === null && kept.length) T.chosen = kept[0].serial;
  const theory = chosenTheory();
  const epoch = R.evidence.epoch;
  ctx.note(`generation ${R.generation}, ${R.evaluations} accounts judged; ${kept.length} kept`);

  const act = async (body, words) => {
    if (T.busy) return;
    redraw();
    try {
      const a = await runTheories(ctx, body, words);
      if (a?.proposed) ctx.log(`${a.proposed.by} proposed ${a.proposed.read.length} theories (grade C), ${a.proposed.refused.length} refused by the engine.`);
    } catch (error) { ctx.log(`The search could not go on: ${error.message}`, 'wrong'); ctx.say(error.message); }
    redraw();
  };
  const head = el('header', { class: 'wall-head' }, el('h2', {}, 'Theories of the case'),
    el('span', { class: 'small' }, `generation ${R.generation} · ${R.evaluations} accounts judged · ${kept.length} kept · ${(R.retired || []).length} retired`),
    el('span', { class: 'spacer' }),
    el('button', { type: 'button', class: 'key main', disabled: T.busy, onclick: () => act({ generations: 10 }, 'ran the search for ten generations') }, T.busy ? 'Searching…' : 'Run ten generations'),
    el('button', { type: 'button', class: 'key edged', disabled: T.busy, title: 'A language model proposes theories in four roles (new ones, a variation, a joining of two, a refinement of a kept one). It is shown the evidence as the search sees it, never what is held back; what it writes is read by the engine and judged like any account, and enters as a proposal of grade C.',
      onclick: () => act({ model: true, budget: 6 }, 'asked a model for theories') }, 'Ask a model'),
    el('button', { type: 'button', class: 'key', disabled: T.busy, onclick: () => { if (confirm('Begin the search again? What it found is set aside.')) act({ reset: true }, 'began the search again'); } }, 'Begin again'),
    el('button', { type: 'button', class: 'key edged', onclick: () => ctx.show('spacetime') }, 'Space and time →'));

  // Each question over the theories found.
  const standings = el('section', { class: 'th-standings' }, (R.standings || []).map((st) => {
    const best = (n) => kept.find((t) => t.serial === n);
    const alt = best(st.best_alternative);
    return el('div', { class: 'th-standing' },
      el('p', { class: 'th-question' }, el('b', {}, gloss(st.question) || st.question), ' ', el('span', { class: `cls ${st.over.split(' ')[0]}` }, `${MARK[st.over] || '·'} ${classOf(st.over).word} over the theories found`)),
      el('p', {}, `Clear under ${st.follows.length}, arguable under ${st.arguable.length}, untenable under ${st.fails.length} of ${kept.length}; `,
        `the theories under which it is clear weigh ${(100 * st.for).toFixed(0)}%${st.open ? `, those under which it turns on a reading ${(100 * st.open).toFixed(0)}%` : ''}.`),
      el('p', { class: `th-burden ${st.verdict.status}` }, el('span', { class: 'th-burden-mark' }, st.verdict.status === 'holds' ? '✓' : st.verdict.status === 'fails' ? '✕' : '·'),
        ` ${st.burden ? `For ${st.burden}: ` : ''}${st.verdict.status === 'holds' ? 'the alternatives are excluded' : 'the alternatives are not excluded'} — ${st.verdict.reason}.`,
        alt ? [' ', el('button', { type: 'button', class: 'key edged', onclick: () => { T.chosen = alt.serial; redraw(); } }, `Open #${alt.serial}`)] : null),
      el('p', { class: 'small' }, `A reasonable alternative, here, is a coherent theory found under which the question is not clear, of a relative plausibility of at least ${st.threshold} (the matter states it; it is no jurisdiction's number). An alternative not found is not thereby excluded.`));
  }));

  // The field: a band of the theories kept, each as wide as its relative plausibility.
  const issue = (R.standings || [])[0]?.issue;
  const band = el('div', { class: 'th-band', role: 'list', 'aria-label': 'The theories kept, by relative plausibility' },
    kept.map((t) => {
      const o = (t.outcomes || []).find((x) => x.issue === issue)?.class || 'untenable';
      return el('button', { type: 'button', role: 'listitem', class: `th-seg${theory?.serial === t.serial ? ' on' : ''}`, style: `flex:${Math.max(0.02, t.plausibility)} 1 0;--tone:${TONE[o] || 'var(--bd-faint)'}`,
        title: `#${t.serial}: ${t.central}; relative plausibility ${(100 * t.plausibility).toFixed(1)}%; ${issue ? `${issue} ${o} under it` : ''}`,
        onclick: () => { T.chosen = t.serial; redraw(); } },
        el('span', { class: 'th-seg-words' }, el('b', {}, `#${t.serial}`), ` ${t.central}`), el('span', { class: 'th-seg-weight' }, `${(100 * t.plausibility).toFixed(0)}% · ${MARK[o] || ''} ${o}`));
    }));
  const field = el('section', { class: 'th-field' }, el('h3', {}, 'The field of accounts'), kept.length ? band : el('p', { class: 'note' }, 'No coherent theory is kept yet: run the search.'),
    el('p', { class: 'small' }, 'Each theory kept is the best coherent account of its kind the search has found: by which kind of event and whose hand, and what the law makes of each question under it. Its width is its relative plausibility, exp(−score) over the theories kept, where the score is what its stance on the evidence costs (believing an item of credibility c costs −log c, doubting it −log(1 − c)), with a nat for each event nothing speaks for. It ranks what was found, and says nothing of what was not.'));

  // The theory chosen, opened as its story.
  const story = theory ? openTheory(el, theory, R, epoch, ctx, redraw) : null;

  // The search itself.
  const loop = searchLoop(el, R);

  // A theory of one's own.
  const text = el('textarea', { class: 'th-write', rows: 4, spellcheck: 'false', placeholder: 'event ignite actor=vane at 0 0 when 21:45:10\ndoubt inn-camera' });
  const out = el('div', { class: 'th-assessed' });
  const own = el('section', { class: 'th-own' }, el('h3', {}, 'A theory of your own'),
    el('p', { class: 'small' }, 'One line each: ', el('code', {}, 'event <kind> [<role>=<person>] at <x> <y> when <HH:MM:SS>'), ', ', el('code', {}, 'doubt <item>'), ', ', el('code', {}, 'believe <item> when <HH:MM:SS>'),
      `. The kinds: ${(S.matter.kinds || []).map((k) => `${k.id}${k.roles?.length ? ` (${k.roles.join(', ')})` : ''}`).join(', ')}; the people: ${(S.matter.agents || []).map((a) => a.id).join(', ')}; places in metres from ${S.matter.place?.scene?.name || 'the scene'}.`),
    text, el('div', { class: 'keys' },
      el('button', { type: 'button', class: 'key edged', onclick: async () => {
        try {
          const a = await ctx.api(`/api/projects/${encodeURIComponent(ctx.name)}/theories/assess`, { method: 'POST', body: { text: text.value } });
          out.replaceChildren(a.read ? el('p', {}, el('span', { class: `cls ${a.coherent ? 'clear' : 'untenable'}` }, a.coherent ? '✓ coherent' : '✕ not coherent'), ` — ${a.reason}`,
            (a.outcomes || []).map((o) => [el('br'), `${gloss(o.issue) || o.issue}: `, el('span', { class: `cls ${o.class.split(' ')[0]}` }, `${MARK[o.class] || ''} ${o.class}`), ' under it'])) : el('p', { class: 'wrong' }, (a.errors || []).join('; ')));
        } catch (error) { out.replaceChildren(el('p', { class: 'wrong' }, error.message)); }
      } }, 'Assess it'),
      el('button', { type: 'button', class: 'key main', disabled: T.busy, onclick: async () => {
        const a = await runTheories(ctx, { text: text.value }, 'entered a theory into the search');
        if (a?.read === false) out.replaceChildren(el('p', { class: 'wrong' }, (a.errors || []).join('; ')));
        else redraw();
      } }, 'Enter it into the search')), out);

  // The ledger of tests on what was held back.
  const held = (R.evidence.located || []).filter((i) => i.held_back);
  const ledger = el('section', { class: 'th-ledger' }, el('h3', {}, 'Tested on what was held back'),
    held.length ? el('p', { class: 'small' }, `Held back from the search: ${held.map((i) => `${i.id} (${i.text || i.says})`).join('; ')}. Each theory kept is tested on it once, the test registered in a journal sealed by a hash chain before the items are read, and its outcome written after. The journal's head is ${String(R.journal_head || '').slice(0, 16)}…`)
      : el('p', { class: 'small' }, 'The matter holds nothing back, so no theory is tested on evidence it was not made from.'),
    (R.tests || []).length ? el('ol', { class: 'th-tests' }, (R.tests || []).map((t) => el('li', { class: t.outcome.replace(/\s+/g, '-') },
      el('b', {}, `#${Number(t.ticket) % 2 ** 48}`), ` ${t.outcome}`, t.doubted?.length ? ` (${t.doubted.join(', ')})` : '', el('span', { class: 'small' }, ` — ${t.reason}; registered ${String(t.registered).slice(0, 10)}…, written ${String(t.written).slice(0, 10)}…`)))) : null);

  fill(pane, head, standings, field, story, el('div', { class: 'th-columns' }, loop, own), ledger);
}

function openTheory(el, t, R, epoch, ctx, redraw) {
  const items = new Map((R.evidence.located || []).map((i) => [i.id, i]));
  const lines = [];
  for (const e of t.events || []) lines.push({ t: e.t, kind: 'event', words: `${e.kind}${Object.values(e.roles || {}).filter(Boolean).length ? ` by ${Object.values(e.roles).filter(Boolean).map(nameOf).join(', ')}` : ', by no one\'s hand'}`, where: `at (${e.x.toFixed(0)}, ${e.y.toFixed(0)})` });
  for (const r of t.readings || []) {
    const i = items.get(r.item);
    if (!i || !r.counted) continue;
    if (i.says === 'sighting' && r.believed) lines.push({ t: r.t ?? i.from, kind: 'seen', words: `${nameOf(i.agent)} — ${i.text || i.id}`, item: i.id });
    else if (!r.believed) lines.push({ t: i.from, kind: 'doubted', words: `${i.text || i.id}`, item: i.id, note: 'doubted' });
  }
  lines.sort((a, b) => a.t - b.t);
  return el('section', { class: 'th-story' },
    el('header', {}, el('h3', {}, `#${t.serial}: ${t.central}`), el('span', { class: 'small' }, `relative plausibility ${(100 * t.plausibility).toFixed(1)}% · score ${Number(t.score).toFixed(2)} (implausibility ${Number(t.implausibility).toFixed(2)} nats) · ${t.doubted.length} doubted`),
      el('span', { class: 'spacer' }), el('button', { type: 'button', class: 'key edged', onclick: () => ctx.show('spacetime') }, 'Draw it in space and time')),
    el('ol', { class: 'th-lines' }, lines.map((l) => el('li', { class: `th-line ${l.kind}` }, el('span', { class: 'th-time' }, clockOf(epoch, l.t)), el('span', { class: 'th-glyph' }, l.kind === 'event' ? '◆' : l.kind === 'doubted' ? '✕' : '●'),
      el('span', {}, l.words, l.where ? el('span', { class: 'small' }, ` ${l.where}`) : null, l.note ? el('span', { class: 'small' }, ` (${l.note})`) : null)))),
    el('div', { class: 'th-law' }, el('h4', {}, 'What the law makes of it'),
      el('p', {}, t.holds.length ? ['It makes hold: ', t.holds.map((h, k) => [k ? ', ' : '', el('b', {}, gloss(h) || h)])] : 'It makes none of the facts the bindings address hold.'),
      (t.outcomes || []).map((o) => el('p', {}, `${gloss(o.issue) || o.issue}: `, el('span', { class: `cls ${o.class.split(' ')[0]}` }, `${MARK[o.class] || ''} ${o.class}`), o.class === 'arguable' ? ' — it turns on how an open term is read: see the readings' : ''))),
    el('div', { class: 'th-test' }, el('h4', {}, 'Tested on what was held back'),
      t.test ? el('p', { class: t.test.outcome.replace(/\s+/g, '-') }, el('b', {}, t.test.outcome === 'admitted' ? '✓ admitted' : t.test.outcome === 'must doubt' ? `✕ must doubt ${t.test.doubted.join(', ')}` : '✕ no longer holds'), ` — ${t.test.reason}`)
        : el('p', { class: 'small' }, 'Not tested: the matter holds nothing back.')),
    el('p', { class: 'small th-lineage' }, `Made by ${t.operation || 'the search'}${t.parents?.length ? ` from #${t.parents.join(', #')}` : ''}; kept at generation ${t.admitted}: ${t.note}`));
}

export function searchLoop(el, R, noun = 'theories') {
  const stats = R.statistics || [];
  const W = 280, H = 64;
  const archive = stats.map((s) => s.archive), best = stats.map((s) => s.best?.[0]).filter((x) => Number.isFinite(x));
  const spark = (v, cls) => {
    if (v.length < 2) return '';
    const lo = Math.min(...v), hi = Math.max(...v), span = hi - lo || 1;
    return `<path class="${cls}" d="${v.map((x, k) => `${k ? 'L' : 'M'}${((W * k) / (v.length - 1)).toFixed(1)},${(H - 4 - ((H - 8) * (x - lo)) / span).toFixed(1)}`).join('')}"/>`;
  };
  const svg = el('div', { class: 'th-spark' });
  svg.innerHTML = `<svg viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="the archive's size and the best score by generation">${spark(archive, 'th-archive')}${spark(best.map((x) => -x), 'th-best')}</svg>`;
  const ops = R.operators || [], prob = R.probability || [];
  const sources = {};
  for (const s of stats) for (const [k, v] of Object.entries(s.proposals || {})) sources[k] = (sources[k] || 0) + v;
  return el('section', { class: 'th-loop' }, el('h3', {}, 'The search'),
    svg, el('p', { class: 'small' }, el('span', { class: 'th-key archive' }), ` ${noun} kept, `, el('span', { class: 'th-key best' }), ' the best score (higher is better), by generation'),
    el('div', { class: 'th-ops' }, ops.map((o, k) => el('span', { class: 'th-op', style: `flex:${Math.max(0.01, prob[k] || 0)} 1 0`, title: `${o}: chosen with probability ${((prob[k] || 0) * 100).toFixed(1)}%` }, o))),
    el('p', { class: 'small' }, 'Each operator\'s share of the children it makes, as the schedule has credited it with what its children did.'),
    Object.keys(sources).length ? el('p', { class: 'small' }, 'Proposed from outside: ', Object.entries(sources).map(([k, v], i) => `${i ? ', ' : ''}${v} by ${k.replace(/^declared:/, '').replace(/^model:/, 'a model, as ').replace(/^person:/, '')}`).join(''), '.') : null);
}
