// Theories of the law (ADR 0133): doctrines, made, varied and crossed by an evolutionary search (the law domain's doctrine search on
// heuresis), each a reading of every open term of the rules and a ratio for every precedent whose court left its reason unstated;
// each judged by how it fits the decided cases (weighted by their force for the forum), the binding readings and the precedents
// taken together, and by the purposes the matter declares; kept as the best doctrine of its kind; and tested once on the decisions
// given after the matter, which the search never reads.
//
// The page is the law as a space of doctrines: the field (what each doctrine kept does not fit against what it is worth, the
// acceptable ones within the tolerance of the best) with each question's standing over them above it; a doctrine chosen opens
// beside the field with its readings, the parts of its score and its test, and beneath it the case law along time and each case base
// as a board, both as the doctrine reads them. Then the search itself, a doctrine of one's own, and the ledger of tests, sealed.

import * as E from '/visualization/js/evidence/index.js';
import { S, D, ensure, hasCaseLaw, loadDoctrines, runDoctrines, chosenDoctrine, classOf } from './common.js';
import { searchLoop } from './theories.js';

export function doctrineView() {
  return { id: 'doctrine', label: 'Doctrine', draw };
}

const MARK = { clear: '✓', arguable: '◐', untenable: '✕' };
const FACT_MARK = { held: '◀', denied: '▶', open: '◆', 'turns on the ratios': '◐' };
const FACT_CLASS = { held: 'clear', denied: 'clear', open: 'arguable', 'turns on the ratios': 'arguable' };
const fmt = (v, d = 2) => (Number.isFinite(v) ? (Math.abs(v - Math.round(v)) < 1e-9 ? String(Math.round(v)) : v.toFixed(d)) : '—');
const words = (s) => String(s || '').replace(/_/g, ' ');

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('dc-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('theories of the law');
  if (!S.scheme || !hasCaseLaw())
    return fill(pane, el('div', { class: 'stv-empty' }, el('h2', {}, 'Theories of the law'),
      el('p', {}, 'The scheme this matter is read under states no decided cases and no precedents by factors, so there is no case law to read as doctrines. A scheme that does (cases on what an open term reaches, and precedents decided on a fact whose courts did not say which factor decided them) is searched here for the doctrines that best fit it. The example ', el('b', {}, 'Council of Avalon v Dashwood'), ' shows one.')));
  try { await loadDoctrines(ctx); } catch (error) { return fill(pane, el('p', { class: 'note wrong' }, error.message)); }
  const R = D.report, redraw = () => ctx.show('doctrine');
  if (!R) return fill(pane, el('p', { class: 'note' }, 'The law of the matter could not be read as doctrines.'));
  const P = R.problem, kept = R.kept || [];
  if (D.chosen === null && kept.length) D.chosen = kept[0].serial;
  const doctrine = chosenDoctrine();
  ctx.note(`generation ${R.generation}, ${R.evaluations} doctrines judged; ${kept.length} kept of ${fmt(P.size, 0)}`);

  const act = async (body, said) => {
    if (D.busy) return;
    redraw();
    try {
      const a = await runDoctrines(ctx, body, said);
      if (a?.proposed) ctx.log(`${a.proposed.by} proposed ${a.proposed.read.length} doctrines (grade C), ${a.proposed.refused.length} refused by the engine.`);
    } catch (error) { ctx.log(`The search could not go on: ${error.message}`, 'wrong'); ctx.say(error.message); }
    redraw();
  };
  const head = el('header', { class: 'wall-head' }, el('h2', {}, 'Theories of the law'),
    el('span', { class: 'small' }, `generation ${R.generation} · ${R.evaluations} doctrines judged · ${kept.length} kept · ${(R.retired || []).length} retired`),
    el('span', { class: 'spacer' }),
    el('button', { type: 'button', class: 'key main', disabled: D.busy, onclick: () => act({ generations: 10 }, 'ran the search for doctrines for ten generations') }, D.busy ? 'Searching…' : 'Run ten generations'),
    el('button', { type: 'button', class: 'key edged', disabled: D.busy, title: 'A language model proposes doctrines in four roles (new ones, a variation, a joining of two, a refinement of a kept one). It is shown the law as the search sees it, never a decision held back; what it writes is read by the engine and judged like any doctrine, and enters as a proposal of grade C.',
      onclick: () => act({ model: true, budget: 6 }, 'asked a model for doctrines') }, 'Ask a model'),
    el('button', { type: 'button', class: 'key', disabled: D.busy, onclick: () => { if (confirm('Begin the search again? What it found is set aside.')) act({ reset: true }, 'began the search for doctrines again'); } }, 'Begin again'));

  // Each question over the acceptable doctrines.
  const standings = el('section', { class: 'th-standings' }, (R.standings || []).map((st) => el('div', { class: 'th-standing' },
    el('p', { class: 'th-question' }, el('b', {}, glossOf(st.question)), ' ', el('span', { class: `cls ${st.verdict.split(' ')[0]}` }, `${MARK[st.verdict] || '·'} ${classOf(st.verdict).word} over the acceptable doctrines`)),
    el('p', {}, `${st.reason[0].toUpperCase()}${st.reason.slice(1)}.`),
    (R.fact_standings || []).map((f) => el('p', { class: 'dc-fact' }, el('b', {}, glossOf(f.fact)), ' ',
      el('span', { class: `cls ${FACT_CLASS[f.verdict] || 'arguable'}` }, `${FACT_MARK[f.verdict] || '·'} ${f.verdict === 'turns on the ratios' ? 'turns on the reasons the precedents are read as decided on' : `${f.verdict} by the precedents`}`),
      el('span', { class: 'small' }, ` — ${f.reason}.`))),
    el('p', { class: 'small' }, `A doctrine is acceptable here when its score is within ${fmt(P.tolerance)} of the best found; the tolerance and every weight are the matter's own, no jurisdiction's. A doctrine not found is not thereby excluded.`))));

  // The field, and the doctrine chosen beside it.
  const F = E.fieldOf(R);
  const plane = el('div', { class: 'dc-plane' });
  plane.innerHTML = E.svgField(F, { chosen: D.chosen, width: 560, height: 380 });
  plane.addEventListener('click', (e) => {
    const g = e.target.closest('[data-serial]');
    if (g) { D.chosen = Number(g.dataset.serial); redraw(); }
  });
  plane.addEventListener('keydown', (e) => {
    const g = e.target.closest('[data-serial]');
    if (g && (e.key === 'Enter' || e.key === ' ')) { e.preventDefault(); D.chosen = Number(g.dataset.serial); redraw(); }
  });
  const legend = el('p', { class: 'dc-legend small' },
    [['holds', '●', `${glossOf(F.question)}: follows`], ['fails', '○', 'does not follow'], ['turns', '◐', 'turns on the precedents'], ['exact', '◌', 'the best of its kind there is']]
      .map(([c, g, w]) => el('span', { class: 'dc-key' }, el('span', { class: `dc-glyph ${c}` }, g), ` ${w}`)));
  const field = el('section', { class: 'dc-field' }, el('h3', {}, 'The field of doctrines'), plane, legend,
    el('p', { class: 'small' }, `Each doctrine kept is the best of its kind the search has found (its kind: what follows under it, and which side the precedents force). It stands to the right by what it does not fit: ${fmt(P.weights.authority)} for a binding reading it goes against, the weight of each decided case it disagrees with (1 for a binding court, ${fmt(P.weights.persuasive)} for a persuasive one, times ${fmt(P.weights.cases)}), ${fmt(P.weights.coherence)} for each pair of precedents its ratios make inconsistent; and upward by what it is worth, the declared purposes it serves, times ${fmt(P.weights.justification)}. Its score is the one less the other, so the lines run where scores are equal; the shaded side holds the acceptable doctrines.`,
      R.exact?.enumerated ? ` There are ${fmt(R.exact.size, 0)} doctrines in all; every one was visited apart from the search, and a ring marks the best of each kind there is, solid where the search has found it.` : ` There are ${fmt(R.exact?.size ?? P.size, 0)} doctrines, too many to visit each.`));
  const card = doctrine ? openDoctrine(el, doctrine, R) : el('section', { class: 'dc-card' }, el('p', { class: 'note' }, 'No doctrine is kept yet: run the search.'));

  // The case law and the precedents, as the doctrine chosen reads them.
  const C = E.caseLawOf(R, doctrine);
  const strip = el('div', { class: 'dc-strip' });
  strip.innerHTML = E.svgCaseLaw(C, { width: 760, height: 150, chosen: D.item });
  strip.addEventListener('click', (e) => {
    const g = e.target.closest('[data-case]');
    if (g) { D.item = D.item === g.dataset.case ? null : g.dataset.case; redraw(); }
  });
  const looked = C.rows.find((r) => r.id === D.item);
  const caseLaw = el('section', { class: 'dc-caselaw' }, el('h3', {}, 'The case law under it'), strip,
    looked ? caseCard(el, looked) : el('p', { class: 'small' }, `A stone is filled for a court that binds the forum, a ring for a persuasive one, dotted for one left out; ✓ where the doctrine agrees with what the court held, ✕ where it does not. Beyond the matter's date, veiled, the decisions held back: the search never reads them, and each doctrine kept is tested on them once. Choose a case to read it.`));
  const boards = (P.bases || []).map((b, k) => {
    const B = E.boardOf(R, doctrine, k);
    const box = el('div', { class: 'dc-board-svg' });
    box.innerHTML = E.svgBoard(B);
    const forced = B.matter.forced;
    return el('section', { class: 'dc-board' }, el('h3', {}, `The precedents on ${words(b.fact)}`), box,
      el('p', { class: 'small' }, forced ? `Read with the reasons ringed, the precedents force ${words(forced)} for the matter: ${B.matter.why}.` : 'Read with the reasons ringed, the precedents force neither side for the matter: it is left open.',
        ' A pip marks a factor a case had; a ring, the factor the doctrine reads the case as decided on, chosen among those it had for its outcome where its court did not say (Horty\'s reason model). The matter\'s factors are the matter\'s to declare.'));
  });

  // A doctrine of one's own.
  const placeholder = [...(P.points || []).map((pt) => `reading ${pt.name} = ${pt.readings?.[0]?.name || ''}`),
    ...(P.bases || []).flatMap((b) => (b.cases || []).filter((c) => !c.stated).slice(0, 1).map((c) => `ratio ${b.fact}/${c.name}: ${(c.for_outcome || [])[0] || ''}`))].join('\n');
  const text = el('textarea', { class: 'th-write', rows: 4, spellcheck: 'false', placeholder });
  const out = el('div', { class: 'th-assessed' });
  const own = el('section', { class: 'th-own' }, el('h3', {}, 'A doctrine of your own'),
    el('p', { class: 'small' }, 'One line each: ', el('code', {}, 'reading <term> = <reading>'), ', ', el('code', {}, 'ratio <fact>/<case>: <factor>, …'),
      `. The terms: ${(P.points || []).map((pt) => `${pt.name} (${(pt.readings || []).map((r) => r.name).join(', ')})`).join('; ')}. The ratios to choose: ${(P.bases || []).flatMap((b) => (b.cases || []).filter((c) => !c.stated).map((c) => `${b.fact}/${c.name} among ${(c.for_outcome || []).join(', ')}`)).join('; ') || 'none'}. What is not named keeps its reading or its ratio as now.`),
    text, el('div', { class: 'keys' },
      el('button', { type: 'button', class: 'key edged', onclick: async () => {
        try {
          const a = await ctx.api(`/api/projects/${encodeURIComponent(ctx.name)}/doctrines/assess`, { method: 'POST', body: { text: text.value } });
          out.replaceChildren(a.read ? el('p', {}, `${a.kind_words}; score ${fmt(a.assessment.score)} (misfit ${fmt(a.assessment.misfit)}, ${a.assessment.conflicts.length} inconsistent pairs, worth ${fmt(a.assessment.justification)}).`) : el('p', { class: 'wrong' }, (a.errors || []).join('; ')));
        } catch (error) { out.replaceChildren(el('p', { class: 'wrong' }, error.message)); }
      } }, 'Assess it'),
      el('button', { type: 'button', class: 'key main', disabled: D.busy, onclick: async () => {
        const a = await runDoctrines(ctx, { text: text.value }, 'entered a doctrine into the search');
        if (a?.read === false) out.replaceChildren(el('p', { class: 'wrong' }, (a.errors || []).join('; ')));
        else redraw();
      } }, 'Enter it into the search')), out);

  // The ledger of tests on the decisions held back.
  const held = P.held_back || [];
  const ledger = el('section', { class: 'th-ledger' }, el('h3', {}, 'Tested on the decisions held back'),
    held.length ? el('p', { class: 'small' }, `Held back from the search, being given after the matter: ${held.map((h) => `${h.name}${h.date ? ` (${h.date})` : ''}`).join('; ')}. Each doctrine kept is tested on them once, the test registered in a journal sealed by a hash chain before they are read, and its outcome written after. The journal's head is ${String(R.journal_head || '').slice(0, 16)}…`)
      : el('p', { class: 'small' }, 'No decision is given after the matter, so no doctrine is tested on a decision it was not made from.'),
    (R.tests || []).length ? el('ol', { class: 'th-tests' }, (R.tests || []).map((t) => el('li', { class: t.outcome },
      el('b', {}, `#${t.serial} ${t.outcome}`), el('span', { class: 'small' }, ` — ${(t.cases || []).map((c) => `${c.id} ${E.VERDICTS[c.verdict]?.word || c.verdict}`).join('; ')}; registered ${String(t.registered).slice(0, 10)}…, written ${String(t.written).slice(0, 10)}…`)))) : null);

  fill(pane, head, standings, el('div', { class: 'dc-top' }, field, card), caseLaw, ...boards, el('div', { class: 'th-columns' }, searchLoop(el, R, 'doctrines'), own), ledger);
}

function glossOf(id) {
  return (S.scheme?.vocabulary || []).find((v) => v.id === id)?.gloss || (S.matter?.facts || []).find((f) => f.id === id)?.text || words(id);
}

function openDoctrine(el, d, R) {
  const P = R.problem, a = d.assessment;
  const terms = E.scoreTerms(P.weights, a);
  const reading = (term, r) => {
    const pt = (P.points || []).find((p) => p.name === term);
    const g = pt?.readings?.find((x) => x.name === r)?.gloss;
    return el('li', {}, el('b', {}, words(term)), ` read ${r}`, g ? el('span', { class: 'small' }, ` — ${g}`) : null);
  };
  const t = d.test;
  return el('section', { class: 'dc-card' },
    el('header', {}, el('h3', {}, `Doctrine #${d.serial}`), el('span', { class: 'small' }, d.kind_words)),
    el('h4', {}, 'Its readings'), el('ul', { class: 'dc-readings' }, Object.entries(d.readings || {}).map(([term, r]) => reading(term, r))),
    (d.ratios || []).some((r) => r.chosen) ? [el('h4', {}, 'Its ratios'), el('ul', { class: 'dc-readings' }, (d.ratios || []).filter((r) => r.chosen).map((r) => el('li', {}, el('b', {}, r.case), ` decided on ${r.factors.map(words).join(' and ')}`)))] : null,
    el('h4', {}, 'Its score'),
    el('table', { class: 'dc-score' }, el('tbody', {},
      terms.map((x) => el('tr', {}, el('td', {}, x.term), el('td', { class: 'num' }, fmt(x.value)), el('td', { class: 'num small' }, `× ${fmt(x.weight)}`), el('td', { class: 'num' }, fmt(x.sum)))),
      el('tr', { class: 'total' }, el('td', {}, 'score (lower is better)'), el('td', {}), el('td', {}), el('td', { class: 'num' }, fmt(a.score))))),
    (a.conflicts || []).length ? el('p', { class: 'small wrong' }, `Inconsistent under its ratios: ${a.conflicts.join('; ')}.`) : null,
    el('h4', {}, 'Tested on what was held back'),
    t ? el('p', { class: `dc-test ${t.outcome}` }, el('b', {}, t.outcome === 'upheld' ? '✓ upheld' : '✕ contradicted'), ' — ',
      (t.cases || []).map((c, k) => [k ? '; ' : '', el('span', { class: `v-${c.verdict}` }, `${E.VERDICTS[c.verdict]?.glyph || '·'} ${c.id}`), ` ${c.reason}`]))
      : el('p', { class: 'small' }, 'Not tested: no decision is held back.'),
    el('p', { class: 'small th-lineage' }, `Made by ${d.operation || 'the search'}${d.parents?.length ? ` from ${d.parents.length} parent${d.parents.length > 1 ? 's' : ''}` : ''}; kept at generation ${d.admitted}${d.note ? `: ${d.note}` : ''}`));
}

function caseCard(el, r) {
  if (r.held_back)
    return el('div', { class: 'dc-case held' }, el('p', {}, el('b', {}, r.name), ` (${r.date}), held back: given after the matter, the search never read it.`),
      el('p', {}, r.verdict ? [el('b', {}, `${E.VERDICTS[r.verdict]?.glyph || ''} ${E.VERDICTS[r.verdict]?.word || r.verdict}`), ` by this doctrine: ${r.reason}.`] : 'This doctrine has not been tested on it.'));
  return el('div', { class: 'dc-case' },
    el('p', {}, el('b', {}, r.name), ` (${r.court}, ${r.date}) — ${E.FORCES[r.force]?.word || r.force} here; ${r.why}.`),
    el('p', {}, `On ${r.facts.map(words).join(', ')}, the court held that ${r.question} ${r.held ? 'followed' : 'did not follow'}${r.passage ? `: “${r.passage}”` : ''}.`),
    el('p', {}, r.agrees === null ? 'Left out, it weighs nothing.' : r.agrees ? '✓ The doctrine agrees with it.' : '✕ The doctrine does not agree with it.'));
}
