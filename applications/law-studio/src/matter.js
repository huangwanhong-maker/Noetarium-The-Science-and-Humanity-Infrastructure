// The matter (ADR 0121): drawn as its argument. Each question stands at the top with its answer (clear, arguable, untenable), the
// rules that give it beneath, each naming its clause, then the facts as they stand and the open terms with their readings. A small
// matter is a drawing; one too large to read so is an outline, unfolded where something is still in play. Whatever is chosen opens
// where it stands: a fact with its standing and its evidence, a rule with the words of its clause, a term with who read it how, the
// question with why it is answered so and what it rests on.

import { matterGraph, layoutMatter, svgMatter, labelEnvironments, matterTree, unfoldedAtFirst, rows, restsOn } from '/visualization/js/evidence/index.js';
import { S, base, ensure, keep, load, gloss, factOf, evaluatedFact, answerOf, classOf, verdictMark, provisionOf, STANDINGS, GLYPH } from './common.js';

export function matterView() {
  return {
    id: 'matter', label: 'Matter', draw,
    heard: (e, ctx) => { if (/matter|candidate|document|template|read by a model/.test(e.what)) { S.matter = null; ctx.show(document.querySelector('.tab.on')?.dataset.tab || 'matter'); } },
  };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('matter-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the matter');
  const m = S.matter, redraw = () => ctx.show('matter');
  const change = async (f, what, note) => { await keep(ctx, f, what, note); redraw(); };
  if (!m.scheme) return fill(pane, begin(ctx, redraw));
  const answers = S.evaluation?.issues || [];
  ctx.note(S.wrong ? S.wrong : answers.length ? answers.map((a) => `${a.question}: ${classOf(a.class).word}`).join(' · ') : 'no question yet');

  // The head: what the matter is called, the law it is read under, and its questions.
  const head = el('header', { class: 'matter-head' },
    el('input', { class: 'matter-title', type: 'text', value: m.title || '', placeholder: 'What the matter is called', spellcheck: false, 'aria-label': 'The matter\'s title',
      onchange: (e) => change((x) => { x.title = e.target.value.trim(); }, 'named the matter', e.target.value.trim()) }),
    el('p', { class: 'under' }, 'read under ', el('button', { type: 'button', class: 'key', onclick: () => ctx.show('law') }, S.scheme?.title || m.scheme),
      m.date ? ` as of ${m.date}` : '', m.forum?.name ? `, before ${m.forum.name}` : ''),
    el('div', { class: 'questions' },
      (m.issues || []).map((i) => {
        const a = answerOf(i.id);
        return el('button', { type: 'button', class: `question${S.issue === i.id ? ' on' : ''}`, title: a?.verdict?.reason || '', onclick: () => { S.issue = S.issue === i.id ? null : i.id; S.chosen = null; redraw(); } },
          el('span', { class: 'q' }, gloss(i.question) || i.question), a ? verdictMark(el, a.class) : null);
      }),
      adder(ctx, '+ a question', (S.scheme?.vocabulary || []).filter((v) => v.use === 'consequence' && !(m.issues || []).some((i) => i.question === v.id)),
        (v) => change((x) => { (x.issues ||= []).push({ id: v.id, question: v.id, burden: '' }); }, 'asked a question', v.id)),
      adder(ctx, '+ a fact', (S.scheme?.vocabulary || []).filter((v) => v.use === 'fact' && !(m.facts || []).some((f) => f.id === v.id)),
        (v) => change((x) => { (x.facts ||= []).push({ id: v.id, text: '', standing: 'alleged' }); }, 'entered a fact', v.id))));

  if (!(m.issues || []).length) return fill(pane, head, el('p', { class: 'note' }, 'Ask a question: a consequence of the law that the matter turns on. The rules that give it, the facts they ask for and the terms to be read are then drawn beneath it.'));
  if (S.wrong) return fill(pane, head, el('p', { class: 'note wrong' }, S.wrong));

  const graph = matterGraph(S.scheme, m, S.evaluation, S.issue);
  const layout = layoutMatter(graph);
  // The drawing is fitted to the page's width, or shown at its own size; a matter that would have to be drawn smaller than four
  // fifths of its size to fit is an outline instead, unless the drawing is asked for.
  const room = Math.max(320, pane.clientWidth - 46);
  const form = S.form || (room / layout.width < 0.8 ? 'outline' : 'drawing');
  const formKey = el('button', { type: 'button', class: 'key', onclick: () => { S.form = form === 'outline' ? 'drawing' : 'outline'; S.chosen = null; redraw(); } }, form === 'outline' ? 'As a drawing' : 'As an outline');
  if (form === 'outline') {
    const body = outline(ctx, graph, change, redraw);
    fill(pane, head, el('div', { class: 'outline' }, body), el('p', { class: 'legend' },
      el('span', {}, '◆ found'), el('span', {}, '■ admitted'), el('span', {}, '● proved'), el('span', {}, '○ alleged'), el('span', {}, '◌ not in the record'), el('span', {}, '✕ decided against'), formKey));
    pane.querySelector('.orow.on')?.scrollIntoView({ block: 'nearest' });
    return;
  }
  const k = S.actual ? 1 : Math.max(0.56, Math.min(1, room / layout.width));
  const canvas = el('div', { class: 'canvas', style: { width: `${Math.ceil(layout.width * k)}px`, height: `${Math.ceil(layout.height * k)}px` } });
  const drawn = el('div', { class: 'drawn', style: { width: `${Math.ceil(layout.width)}px`, height: `${Math.ceil(layout.height)}px`, transform: `scale(${k})` } });
  drawn.innerHTML = svgMatter(graph, layout, { selected: S.chosen });
  canvas.append(drawn);
  canvas.addEventListener('click', (e) => {
    const g = e.target.closest('[data-node]');
    if (e.target.closest('.card')) return;
    S.chosen = g ? (S.chosen === g.dataset.node ? null : g.dataset.node) : null;
    redraw();
  });
  const node = graph.nodes.find((n) => n.id === S.chosen);
  if (node) {
    const p = layout.at.get(node.id), W = 340;
    const x = p.x * k, w = p.w * k, wide = Math.max(layout.width * k, room);
    const right = x + w + 14 + W <= wide;
    const box = card(ctx, node, change);
    Object.assign(box.style, { left: `${right ? x + w + 14 : Math.max(8, x - W - 14)}px`, top: `${Math.max(0, p.y * k - 6)}px`, width: `${W}px` });
    canvas.append(box);
  }
  const legend = el('p', { class: 'legend' },
    el('span', {}, '◆ found'), el('span', {}, '■ admitted'), el('span', {}, '● proved'), el('span', {}, '○ alleged'), el('span', {}, '◌ open'), el('span', {}, '✕ decided against'),
    el('span', { class: 'sep' }), el('span', { class: 'l-dash' }, 'an exception'), el('span', { class: 'l-dot' }, 'under a reading'),
    el('span', { class: 'spacer' }), k < 1 || S.actual ? el('button', { type: 'button', class: 'key', onclick: () => { S.actual = !S.actual; redraw(); } }, S.actual ? 'Fit to the page' : 'At its own size') : null, formKey);
  fill(pane, head, el('div', { class: 'ground' }, canvas), legend);
  const chosen = pane.querySelector('.card');
  if (chosen) chosen.scrollIntoView({ block: 'nearest', inline: 'nearest' });
}

// The matter as an outline: each question unfolded into the rules that give it, what they ask for, and their exceptions.
const UNFOLDED = new Map();   // by project and question: the keys unfolded
function outline(ctx, graph, change, redraw) {
  const { el } = ctx;
  const fixed = S.evaluation?.admissibility?.fixed || [];
  return graph.nodes.filter((n) => n.role === 'issue').map((issue) => {
    const tree = matterTree(graph, issue.id);
    const memo = `${S.project}/${issue.id}`;
    if (!UNFOLDED.has(memo)) UNFOLDED.set(memo, unfoldedAtFirst(tree));
    const open = UNFOLDED.get(memo);
    const word = (t) => {
      const n = t.node;
      if (t.kind === 'atom' && n.role === 'fact') {
        const standing = n.used || n.declared;
        return el('span', { class: `status ${t.state}` }, n.declared === null ? '◌ not in the record' : `${GLYPH[standing] || '○'} ${n.used && n.used !== n.declared ? `${n.declared} → ${n.used}` : standing}`,
          n.evidence && n.evidence !== 'unsupported' ? el('span', { class: 'small' }, ` · evidence ${n.evidence}`) : null);
      }
      if (t.repeat) return el('span', { class: 'status' }, 'set out above');
      if (t.kind === 'atom') return n.role === 'issue' ? verdictMark(el, n.cls) : el('span', { class: `status ${t.stands ? 'stands' : t.state}` }, t.stands ? '✓ stands' : t.state === 'open' ? '◐ in play' : t.state === 'absent' ? '— not alleged' : '— not shown');
      if (t.kind === 'exception') return el('span', { class: `status ${t.stands ? 'defeats' : t.state}` }, t.stands ? '✕ applies: the rule gives way' : t.state === 'open' ? '◐ in play' : t.state === 'absent' ? '— not alleged' : '— does not apply');
      return el('span', { class: `status ${t.stands ? 'stands' : t.state}` }, t.stands ? '✓ applies' : n.applied ? '✕ defeated' : t.state === 'open' ? '◐ in play' : t.state === 'absent' ? '— not alleged' : '— does not apply');
    };
    const line = (t) => {
      const n = t.node;
      if (t.kind === 'atom') {
        const text = n.role === 'fact' ? (factOf(n.atom)?.text || gloss(n.atom) || n.atom) : (gloss(n.atom) || n.atom);
        return [el('span', { class: 'kicker' }, n.role === 'issue' ? 'the question' : n.role === 'fact' ? (t.negated ? 'not the fact' : 'fact') : t.negated ? 'not' : ''),
          el('span', { class: n.role === 'fact' ? 'what' : 'what derived' }, text), el('code', {}, n.atom)];
      }
      const cite = n.provisions.length ? n.provisions.map((q) => `${q.instrument ? `${q.instrument}, ` : ''}${q.label}`).join(' · ') : 'no clause cited';
      const readings = t.readings.map((r) => el('span', { class: `rd${r.fixed ? ' fixed' : ''}${r.aside ? ' aside' : ''}`, title: r.gloss || '' }, `${r.fixed ? '◆' : r.aside ? '✕' : '◇'} ${r.term.replace(/_/g, ' ')}: ${r.reading.replace(/_/g, ' ')}`, el('span', { class: 'small' }, r.fixed ? ' fixed' : r.aside ? ' set aside' : ' open')));
      return [el('span', { class: 'kicker' }, t.kind === 'exception' ? 'unless' : t.kind === 'counts' ? 'counts as it' : 'by'),
        el('span', { class: 'what by' }, n.rule.replace(/_/g, ' ')), readings, el('cite', {}, cite)];
    };
    return el('section', { class: 'otree' }, rows(tree, open).map((t) => {
      const on = S.chosen === t.node.id && S.chosenKey === t.key;
      const row = el('div', { class: `orow k-${t.kind}${t.node.role ? ` r-${t.node.role}` : ''} s-${t.state}${on ? ' on' : ''}${t.repeat ? ' repeat' : ''}`, style: `--depth:${t.depth}`, 'data-node': t.node.id,
        onclick: () => { const same = on; S.chosen = same ? null : t.node.id; S.chosenKey = same ? null : t.key; redraw(); } },
        t.children.length ? el('button', { type: 'button', class: 'twisty', 'aria-label': t.unfolded ? 'Fold' : 'Unfold', 'aria-expanded': t.unfolded,
          onclick: (e) => { e.stopPropagation(); if (open.has(t.key)) open.delete(t.key); else open.add(t.key); redraw(); } }, t.unfolded ? '▾' : '▸') : el('span', { class: 'twisty none' }),
        el('span', { class: 'line' }, line(t)), word(t));
      if (!on) return row;
      const box = card(ctx, t.node, change);
      box.classList.add('inline');
      box.style.marginLeft = `calc(${t.depth} * 22px + 30px)`;
      return [row, box];
    }));
  });
}

// A key that opens into a choice among what the scheme declares.
function adder(ctx, label, choices, act) {
  const { el } = ctx;
  if (!choices.length) return null;
  const pick = el('select', { class: 'adder', 'aria-label': label, onchange: (e) => { const v = choices.find((c) => c.id === e.target.value); if (v) act(v); } },
    el('option', { value: '' }, label), choices.map((c) => el('option', { value: c.id, title: c.gloss || '' }, `${c.id}${c.gloss ? ` — ${c.gloss}` : ''}`)));
  return pick;
}

// With no scheme yet: an example laid out, or a scheme chosen.
function begin(ctx, redraw) {
  const { el } = ctx;
  const lay = async (t) => {
    ctx.say('Laying out…');
    try {
      const a = await ctx.api(`${base(ctx)}/example`, { method: 'POST', body: { template: t.id } });
      ctx.log(`${a.matter} laid out: ${a.documents.length} documents kept with their digests.`);
      await load(ctx);
    } catch (error) { ctx.say(error.message); }
    redraw();
  };
  const choose = async (s) => { await keep(ctx, (x) => { x.scheme = s.id; x.title ||= ctx.project.title || ''; }, 'chose the law the matter is read under', s.title); redraw(); };
  return el('section', { class: 'begin' },
    el('h2', {}, 'A matter'),
    el('p', { class: 'lede' }, 'A matter is read under a body of law stated clause by clause. Its questions are consequences of that law; the studio draws what each rests on, keeps the evidence for its facts, reads its open terms with the courts, and says what remains to be proved.'),
    S.templates.length ? el('div', { class: 'choices' }, el('h3', {}, 'An example, laid out'), S.templates.map((t) => el('button', { type: 'button', class: 'choice', onclick: () => lay(t) }, el('b', {}, t.title), el('span', {}, t.about)))) : null,
    el('div', { class: 'choices' }, el('h3', {}, 'A matter of one\'s own, under'), S.schemes.map((s) => el('button', { type: 'button', class: 'choice', onclick: () => choose(s) },
      el('b', {}, s.title), el('span', {}, s.about || s.instruments.map((i) => i.citation || i.title).join(' · '))))));
}

// What opens beside a chosen node.
function card(ctx, n, change) {
  const { el } = ctx;
  const close = el('button', { type: 'button', class: 'close', 'aria-label': 'Close', onclick: (e) => { e.stopPropagation(); S.chosen = null; ctx.show('matter'); } }, '×');
  const go = (view, set) => el('button', { type: 'button', class: 'key edged', onclick: (e) => { e.stopPropagation(); set?.(); ctx.show(view); } });
  const clauses = (ps) => ps.map((q) => {
    const p = provisionOf(q.id);
    return el('blockquote', { class: 'clause' }, el('cite', {}, `${q.instrument ? `${q.instrument}, ` : ''}${q.label}`), p?.text || el('i', {}, 'its words are not fetched yet'),
      el('button', { type: 'button', class: 'key', onclick: (e) => { e.stopPropagation(); S.provision = q.id; ctx.show('law'); } }, 'read it in the law'));
  });
  if (n.role === 'issue') {
    const a = answerOf(n.issue);
    const issue = S.matter.issues.find((i) => i.id === n.issue);
    const envs = labelEnvironments(a?.label, S.evaluation?.admissibility?.fixed || []);
    const k1 = go('plan', () => { S.issue = n.issue; }); k1.textContent = 'The plan of proof';
    return el('aside', { class: 'card' }, close, el('h3', {}, gloss(n.atom) || n.atom), a ? el('p', { class: 'verdict' }, verdictMark(el, a.class), ' ', a.verdict.reason) : null,
      envs.length ? el('p', {}, 'As the facts stand it follows ', envs.map((e, k) => [k ? ', or ' : '', e.length ? ['under ', e.map((r, j) => [j ? ' with ' : '', el('code', {}, r)])] : 'whatever the open terms are read to mean'])) : el('p', {}, 'It follows under no admissible reading as the facts stand.'),
      el('label', { class: 'row' }, 'The burden is on', el('input', { type: 'text', value: issue?.burden || '', onchange: (e) => change((x) => { x.issues.find((i) => i.id === n.issue).burden = e.target.value.trim(); }, 'set a burden', `${n.issue}: ${e.target.value.trim()}`) })),
      a?.rests_on?.steps?.length ? [el('h4', {}, a.class === 'untenable' ? 'What stands, though it does not follow' : 'What it rests on'),
        el('ol', { class: 'steps' }, restsOn(S.scheme, a).map((s) => el('li', { class: s.holds ? '' : 'fails' }, el('b', {}, s.id), ' gives ', el('code', {}, s.consequence), s.holds ? '' : ' (defeated)',
          s.provisions.length ? el('span', { class: 'cite' }, ` · ${s.provisions.map((p) => p.label).join(', ')}`) : null)))] : null,
      a?.witness ? el('details', {}, el('summary', {}, 'The argument, step by step'), el('pre', {}, a.witness)) : null,
      el('p', { class: 'keys' }, k1,
        el('button', { type: 'button', class: 'key', onclick: (e) => { e.stopPropagation(); change((x) => { x.issues = x.issues.filter((i) => i.id !== n.issue); }, 'withdrew a question', n.issue); } }, 'Withdraw the question')));
  }
  if (n.kind === 'rule' || n.kind === 'exception' || n.kind === 'counts') {
    const def = (S.scheme[n.kind === 'rule' ? 'rules' : n.kind === 'exception' ? 'exceptions' : 'counts'] || []).find((x) => x.id === n.rule);
    return el('aside', { class: 'card' }, close, el('h3', {}, n.rule),
      el('p', {}, n.kind === 'rule' ? ['Where ', def.elements.map((e, k) => [k ? ' and ' : '', el('code', {}, e)]), ', then ', el('code', {}, def.consequence), '.']
        : n.kind === 'exception' ? ['Where ', def.elements.map((e, k) => [k ? ' and ' : '', el('code', {}, e)]), ', the rule ', el('code', {}, def.rule), ' does not apply.']
          : ['Under the reading ', el('code', {}, `[${def.term}=${def.reading}]`), ', ', el('code', {}, def.fact), ' counts as ', el('code', {}, def.as), '.']),
      el('p', { class: 'small' }, n.applied ? 'It applies in the argument for the question.' : 'It does not apply as the facts and readings stand.'),
      n.provisions.length ? clauses(n.provisions) : el('p', { class: 'wrong' }, 'It is cited to no clause.'));
  }
  if (n.kind === 'term') {
    const k = go('readings', () => { S.term = n.term; }); k.textContent = 'Its readings and who read it how';
    return el('aside', { class: 'card' }, close, el('h3', {}, n.term), el('p', {}, n.text),
      el('ul', { class: 'readings' }, n.readings.map((r) => el('li', { class: r.fixed ? 'fixed' : r.aside ? 'aside' : '' }, el('b', {}, `${r.fixed ? '◆' : r.aside ? '✕' : '◇'} ${r.name}`), r.fixed ? ' fixed' : r.aside ? ' set aside' : ' open', r.gloss ? el('span', { class: 'small' }, ` — ${r.gloss}`) : null))),
      n.holdings.length ? el('ul', { class: 'holdings' }, n.holdings.map((h) => el('li', {}, el('b', {}, h.name || h.holding), ` read it ${h.reading}: ${h.force}`))) : null,
      el('p', { class: 'keys' }, k));
  }
  // A fact, or a consequence that rules give.
  if (n.role !== 'fact') return el('aside', { class: 'card' }, close, el('h3', {}, n.atom), el('p', {}, gloss(n.atom) || 'A consequence that rules of the scheme give.'));
  const f = factOf(n.atom), e = evaluatedFact(n.atom);
  const cands = (S.matter.candidates || []).filter((c) => c.proposition === n.atom && c.status !== 'rejected');
  const k = go('evidence', () => { S.fact = n.atom; S.doc = null; }); k.textContent = cands.length ? `Its evidence: ${cands.length} ${cands.length === 1 ? 'passage' : 'passages'}` : 'Look for its evidence';
  const set = (standing) => change((x) => {
    const y = (x.facts ||= []).find((q) => q.id === n.atom);
    if (y) y.standing = standing; else x.facts.push({ id: n.atom, text: '', standing });
  }, 'set a fact\'s standing', `${n.atom}: ${standing}`);
  return el('aside', { class: 'card' }, close, el('h3', {}, n.atom), el('p', {}, f?.text || gloss(n.atom)),
    f?.text && gloss(n.atom) && gloss(n.atom) !== f.text ? el('p', { class: 'small' }, `In the scheme: ${gloss(n.atom)}.`) : null,
    el('label', { class: 'row' }, 'It stands as', el('select', { onchange: (ev) => set(ev.target.value) },
      f ? null : el('option', { value: '', selected: true }, 'not in the record'), STANDINGS.map((s) => el('option', { value: s, selected: f?.standing === s }, `${GLYPH[s]} ${s}`)))),
    e?.used && e.used !== e.declared ? el('p', { class: 'small' }, `Declared ${e.declared}; it enters the case as ${e.used}, since its evidence meets the standard.`) : null,
    e?.evidence ? el('p', {}, 'Its evidence is ', el('b', {}, e.evidence), e.meets ? `: ${e.meets.reason}.` : '.') : el('p', { class: 'small' }, 'No evidence is offered for it yet.'),
    n.place ? el('p', { class: 'small' }, n.place === 'open' ? 'It is open: the plan of proof says what making it out, or not, would do.' : n.place === 'given' ? 'The record gives it.' : 'It is decided against.') : null,
    el('p', { class: 'keys' }, k, f ? el('button', { type: 'button', class: 'key', onclick: (ev) => { ev.stopPropagation(); change((x) => { x.facts = x.facts.filter((q) => q.id !== n.atom); }, 'took a fact out of the record', n.atom); } }, 'Take it out of the record') : null));
}
